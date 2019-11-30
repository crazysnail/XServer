#include "mt_db_load_work.h"
#include "client_session.h"
#include "../mt_guid/mt_guid.h"
#include "LoginPacket.pb.h"
#include "CreatePlayerPacket.pb.h"
#include "EnterScene.pb.h"
#include "ActorBasicInfo.pb.h"
#include "DataCellPacket.pb.h"
#include <PlayerBasicInfo.pb.h>
#include <SceneStage.pb.h>
#include <ArenaDb.pb.h>
#include <TrialFieldDB.pb.h>
#include "../mt_actor/mt_actor.h"
#include "../mt_data_cell/mt_data_cell.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_item/mt_container.h"
#include "../mt_market/mt_market.h"
#include "../mt_trial_field/mt_tower.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_mission/mt_mission.h"
#include "../mt_arena/mt_arena.h"
#include "../mt_chat/mt_word_manager.h"
#include "../mt_battle_reply/mt_battle_reply_manager.h"
#include "../mt_capture_point/mt_capture_point.h"
#include <active_model.h>
#include "../mt_config/mt_config.h"
#include <BaseConfig.pb.h>
#include "../mt_server/mt_server.h"
#include "../friend/FriendContainer.h"
#include "../friend/mt_friend_manager.h"
#include "../mail/MessageContainer.h"
#include "../mt_activity/mt_activity.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_trial_field/mt_trial_field.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../ranklist/top_ranklist.h"
#include "../mail/static_message_manager.h"
#include <boost/range/algorithm/copy.hpp>
#include <boost/range/adaptor/indirected.hpp>

namespace Mt
{

	using namespace Packet;


	/**
	* \brief 从数据库中加载玩家所有数据
	* \param db_info 玩家的基本数据
	* \param dbctx 数据库上下文
	* \return boost::shared_ptr<MtPlayer> 玩家对象
	*/
	boost::shared_ptr<MtPlayer> LoadPlayer(const boost::shared_ptr<PlayerDBInfo>& db_info, const boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		if (db_info == nullptr) return nullptr;
		auto account = ActiveModel::Base<AccountModel>(dbctx).FindOne({ { "guid" , db_info->account_guid() } });
		if (account == nullptr) {
			ZXERO_ASSERT(false) << "invalid account info ,guid=" << db_info->guid();
			return nullptr;
		}

		LOG_INFO << "[player load] start. player:" << db_info->guid();
		auto player = boost::make_shared<MtPlayer>(db_info, account);
		ContainerManager::Instance().LoadPlayerContainer(player, dbctx);
		MtMissionManager::Instance().LoadPlayerMission(player, dbctx);
		auto actor_db_infos = ActiveModel::Base<ActorBasicInfo>(dbctx).FindAll({ { "player_guid", player->Guid() } });
		boost::for_each(actor_db_infos, boost::bind(&google::protobuf::Message::ClearDirty, _1));
		for (auto& actor_info : actor_db_infos) {
			if (actor_info->type() == Packet::ActorType::Main) {
				auto actor_hairs = MtActorConfig::Instance().ProfHairs(actor_info->profession());
				if (std::find(actor_hairs.begin(), actor_hairs.end(), actor_info->hair()) == actor_hairs.end()){
					LOG_ERROR << "[load_player] actor:" << actor_info->guid() << " missing hair, fixed";
					actor_info->set_hair(actor_hairs[0]);
				}
			}
			auto skills = ActiveModel::Base<ActorSkill>(dbctx).FindAll({ { "actor_guid", (uint64)actor_info->guid() } });
			boost::for_each(skills, boost::bind(&google::protobuf::Message::ClearDirty, _1));
			auto actor = boost::make_shared<MtActor>(actor_info, *player);
			actor->OnSkillLoad(skills);
			player->LoadActor(actor);
			actor->GetSimpleImpactSet()->OnLoad(dbctx);
		}
		//数据完整性检查！
		auto basic_info = db_info->mutable_basic_info();
		if (player->Actors().empty()) {
			LOG_ERROR << "login data load failed! actors is none! player guid=" << player->Guid();
			player->AddHero(basic_info->init_actor_id(), Packet::ActorType::Main, "INIT");
		}
		if (basic_info->level() != player->PlayerLevel()){
			player->MainActor()->DbInfo()->set_level(basic_info->level());
		}
		//初始化挂机数据
		auto player_stage_db = ActiveModel::Base<PlayerStageDB>(dbctx).FindOrCreateOne({ { "player_guid", db_info->guid() } });
		if (player_stage_db->current_stage() == 0) {
			player_stage_db->set_current_stage(MtSceneStageManager::Instance().DefaultStageId(player->GetCamp()));
		}
		player->SetPlayerStageDbInfo(player_stage_db);
		auto stages = ActiveModel::Base<PlayerStage>(dbctx).FindAll({ {"player_guid", db_info->guid()} });
		boost::for_each(stages, boost::bind(&PlayerStage::ClearDirty, _1));
		player->LoadPlayerStage(stages);
		auto default_stage_id = MtSceneStageManager::Instance().DefaultStageId(player->GetCamp());
		if (player->GetPlayerStage(default_stage_id) == nullptr) {
			player->AddPlayerStage(default_stage_id);
		}
		//加载玩家设置
		auto setttings = ActiveModel::Base<PlayerSetting>(dbctx).FindOrCreateOne({ {"guid", player->Guid()} });
		player->SetPlayerSetting(setttings);
		// 加载阵容 start [10/28/2017 Ryan]
		auto actor_formations = ActiveModel::Base<ActorFormation>(dbctx).FindAll({ { "player_guid", player->Guid() } });
		boost::for_each(actor_formations, boost::bind(&google::protobuf::Message::ClearDirty, _1));
		boost::for_each(actor_formations, boost::bind(&MtPlayer::AddActorFormation, boost::ref(player), _1));
		player->FixActorFormations();
		// 加载阵容 end [10/28/2017 Ryan]
		// 加载工会训练
		auto practice_list = ActiveModel::Base<PracticeInfo>(dbctx).FindAll({ { "playerguid", player->Guid() } });
		boost::for_each(practice_list, boost::bind(&google::protobuf::Message::ClearDirty, _1));
		boost::for_each(practice_list, boost::bind(&MtPlayer::AddPracticeInfo, boost::ref(player), _1));
		
		player->Friend_Container()->OnLoad(dbctx);
		player->GetMessageContainer()->OnLoad(dbctx);
		MtMarketManager::Instance().LoadPlayerMarket(player, dbctx);
		MtDataCellManager::Instance().LoadDataCell(player, dbctx);
		MtActivityManager::Instance().LoadPlayerActivity(player, dbctx);
		player->GetSimpleImpactSet()->OnLoad(dbctx);
		MtWordManager::Instance()->RegName(player->Name());
		LOG_INFO << "[player load] done. player:" << player->Guid();
		return player;
	}

	IMPLEMENT_DBWORK(AccountNameVerifyWork)
	{
		if (ActiveModel::Base < AccountModel>(dbctx).CheckExist({ {"userid", message_->userid()} })) {
			reply_.set_result(AccountNameVerifyReply_Result_DUPLICATED);
		}
		else {
			reply_.set_result(AccountNameVerifyReply_Result_SUCCESS);
		}
	}

	AccountNameVerifyWork::AccountNameVerifyWork(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountNameVerify>& message) :
		session_(session), message_(message)
	{
		reply_.mutable_requset()->CopyFrom(*message_.get());
		reply_.set_result(AccountNameVerifyReply_Result::AccountNameVerifyReply_Result_UNKNOWN);
	}

	void AccountNameVerifyWork::done()
	{
		dbwork::done();
		if (session_->get_state() != client_session_state::kVerifing) {
			LOG_WARN << "client status wrong when verify account name duplicate:" << session_->get_state();
			session_->close(true);
		}
		else {
			session_->send(reply_);
		}
	}

	IMPLEMENT_DBWORK(AccountRegistWork)
	{
		if (ActiveModel::Base < AccountModel>(dbctx).CheckExist({ { "userid", message_->userid() } })) {
			reply_.set_result(AccountRegistReply_Result_DUPLICATED);
			return;
		}
		const int ACCOUNT_NAME_MIN_LEN = 8;
		const int ACCOUNT_NAME_MAX_LEN = 50;
		if (message_->userid().length() < ACCOUNT_NAME_MIN_LEN || message_->userid().length() > ACCOUNT_NAME_MAX_LEN) {
			reply_.set_result(AccountRegistReply_Result_ACCOUNT_LENGTH_INVALID);
			return;
		}
		const int PASSWORD_MIN_LEN = 6;
		const int PASSWORDE_MAX_LEN = 30;
		if (message_->password().length() < PASSWORD_MIN_LEN || message_->password().length() > PASSWORDE_MAX_LEN) {
			reply_.set_result(AccountRegistReply_Result_PASSWORD_TOW_SIMPLE);
			return;
		}
		if (ActiveModel::Base < AccountModel>(dbctx).CheckExist({ { "udid", message_->udid()} })) {
			auto account_info = ActiveModel::Base<AccountModel>(dbctx).FindOne({ { "udid", message_->udid() } });
			account_info->set_userid(message_->userid());
			account_info->set_password(message_->password());
			account_info->set_account_type(AccountType::BIND);
			account_info->set_partnerid(message_->partnerid());
			ActiveModel::Base < AccountModel>(dbctx).Update(*account_info.get());
			reply_.set_result(AccountRegistReply_Result::AccountRegistReply_Result_SUCCESS);
		}
		else {
			ActiveModel::Base<AccountModel>(dbctx).Create({
				{ "guid", MtGuid::Instance().GenGuid<AccountModel>()},
				{ "userid", message_->userid() },
				{ "password", message_->password() },
				{ "account_type", AccountType::BIND },
				{ "udid", message_->udid() },
				{ "partnerid", message_->partnerid() },
			});
			reply_.set_result(AccountRegistReply_Result::AccountRegistReply_Result_SUCCESS);

			//mqdblopg 设备注册		
			//std::string tstr = MtTimerManager::Instance().NowTimeString();	
			//std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			//boost::format fmt("REPLACE INTO `device_register_log` (`DeviceIdentifier`,`ServerID`,`PartnerID`,`UserID`,`CrDate`,`CrTime`) VALUES('%s', '%d', '%d', '%s', '%s', '%s'); ");
			//fmt%message_->udid() % MtConfig::Instance().server_group_id_%message_->partnerid() % message_->userid() % dstr %tstr;
			//Server::Instance().SendLogMessage(fmt.str());
		}
	}

	AccountRegistWork::AccountRegistWork(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AccountRegist>& message) :
		session_(session), message_(message)
	{
		reply_.mutable_requset()->CopyFrom(*message_.get());
		reply_.set_result(AccountRegistReply_Result::AccountRegistReply_Result_UNKNOWN);
	}

	void AccountRegistWork::done()
	{
		dbwork::done();
		session_->send(reply_);
	}

	IMPLEMENT_DBWORK(AccountLoadWork)
	{
		if (message_->type() == AccountType::FAST) {
			if (ActiveModel::Base<AccountModel>(dbctx).CheckExist({ { "userid", message_->userid() } })) {
				account_info_ = ActiveModel::Base<AccountModel>(dbctx).FindOne({ { "userid", message_->userid() } });
			}
			else{//创建快速登录账号
				if (!ActiveModel::Base < AccountModel>(dbctx).CheckExist({ { "udid", message_->udid() } })) {
					//mqdblopg 设备注册
					std::string tstr = MtTimerManager::Instance().NowTimeString();
					std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
					if (!ActiveModel::Base < AccountModel>(dbctx).CheckExist({ { "userid", message_->userid() },{ "udid", message_->udid() } })) {
						boost::format fmt("REPLACE INTO `device_register_log` (`DeviceIdentifier`,`ServerID`,`PartnerID`,`UserID`,`CrDate`,`CrTime`) VALUES('%s', '%d', '%d', '%s', '%s', '%s'); ");
						fmt%message_->udid() % MtConfig::Instance().server_group_id_%message_->partnerid() % message_->userid() % dstr %tstr;
						Server::Instance().SendLogMessage(fmt.str());
					}
				}
				account_info_ = ActiveModel::Base<AccountModel>(dbctx).Create({
					{ "guid", MtGuid::Instance().GenGuid<AccountModel>()},
					{ "userid", message_->userid() },
					{ "account_type", AccountType::FAST },
					{ "udid", message_->udid() },
					{ "partnerid", message_->partnerid() }
				});
			}
		}
		else if (message_->type() == AccountType::BIND) {
			account_info_ = ActiveModel::Base<AccountModel>(dbctx).Create({ 
				{ "guid", MtGuid::Instance().GenGuid<AccountModel>() },
				{ "userid", message_->userid() },
				{ "account_type", AccountType::BIND },
				{ "udid", message_->udid() }, 
				{ "partnerid", message_->partnerid() }
			});
		}
		else if (message_->type() == AccountType::THIRD)
		{
			if (ActiveModel::Base<AccountModel>(dbctx).CheckExist({ { "userid", message_->userid() } })) {
				account_info_ = ActiveModel::Base<AccountModel>(dbctx).FindOne({ { "userid", message_->userid() } });
			}
			else {//sdk登录创建
				if (!ActiveModel::Base < AccountModel>(dbctx).CheckExist({ { "udid", message_->udid() } })) {
					//mqdblopg 设备注册
					std::string tstr = MtTimerManager::Instance().NowTimeString();
					std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
					if (!ActiveModel::Base < AccountModel>(dbctx).CheckExist({ { "userid", message_->userid() },{ "udid", message_->udid() } })) {
						boost::format fmt("REPLACE INTO `device_register_log` (`DeviceIdentifier`,`ServerID`,`PartnerID`,`UserID`,`CrDate`,`CrTime`) VALUES('%s', '%d', '%d', '%s', '%s', '%s'); ");
						fmt%message_->udid() % MtConfig::Instance().server_group_id_%message_->partnerid() % message_->userid() % dstr %tstr;
						Server::Instance().SendLogMessage(fmt.str());
					}
				}
				account_info_ = ActiveModel::Base<AccountModel>(dbctx).Create({ 
					{ "guid", MtGuid::Instance().GenGuid<AccountModel>() },
					{ "userid", message_->userid() },
					{ "account_type", AccountType::THIRD },
					{ "udid", message_->udid() },
					{ "partnerid", message_->partnerid() }
				});
			}
		}
		else {
			ZXERO_ASSERT(false);
		}
	}
	AccountLoadWork::AccountLoadWork(const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::LoginRequest>& message)
		:session_(session), message_(message)
	{
	}

	void AccountLoadWork::done()
	{
		dbwork::done();
		if (session_->get_state() != client_session_state::kVerifing) {
			LOG_WARN << "client status wrong after load account info:" << session_->get_state();
			reply_.set_result(LoginReply_Result::LoginReply_Result_WRONG_STATUS);
			session_->send(reply_);
			return;
		}
		switch (message_->type())
		{
		case AccountType::FAST:
			if (account_info_ != nullptr && account_info_->userid() == message_->userid()) {
				reply_.set_result(LoginReply_Result::LoginReply_Result_SUCCESS);
				session_->state(client_session_state::kBeforEnteringScene);
			}
			else {
				reply_.set_result(LoginReply_Result::LoginReply_Result_UNKNOWN);
			}
			break;
		case AccountType::BIND:
			if (account_info_ == nullptr) {
				reply_.set_result(LoginReply_Result_NO_SUCH_ACCOUNT);
				break;
			}
			if (account_info_->password() == message_->password()) {
				reply_.set_result(LoginReply_Result_SUCCESS);
				session_->state(client_session_state::kBeforEnteringScene);
			}
			else {
				reply_.set_result(LoginReply_Result::LoginReply_Result_WRONG_PASSWORD);
			}
			break;
		case AccountType::THIRD:
		{
			if (account_info_ == nullptr) {
				reply_.set_result(LoginReply_Result_NO_SUCH_ACCOUNT);
				break;
			}
			reply_.set_result(LoginReply_Result_SUCCESS);
			session_->state(client_session_state::kBeforEnteringScene);
		}
		break;
		default:
			LOG_WARN << " no support type. message: " << message_->DebugString();
			reply_.set_result(LoginReply_Result::LoginReply_Result_UNKNOWN);
			break;
		}
		if (account_info_) {
			session_->set_account(account_info_);
			session_->set_logininfo(message_);						
		}
		session_->send(reply_);
	}
	

	IMPLEMENT_DBWORK(CreatePlayerWork)
	{
		auto account = session_->get_account();
		if (account == nullptr) {
			return;
		}
		if (account->guid() == INVALID_GUID) {
			return;
		}
		else {
			LOG_INFO << "create player begin,account:" << account->guid();
			if (message_->hair() < 1) {
				notify_.set_notify("InvalidHairValue(0-20)");
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
				return;
			}
			if (MtConfig::Instance().disable_create_actor_) {
				notify_.set_notify("ServerFullCannotCreatePlayer");
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
				return;
			}
			if (MtWordManager::Instance()->UniqueName(message_->name()) == false) {
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_DUMPLICATE_NAME;
				notify_.set_notify("DuplicatePlayerName");
				return;
			}
			if (ActiveModel::Base<PlayerDBInfo>(dbctx).CheckExist({ { "basic_info.name", message_->name() } })) {
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_DUMPLICATE_NAME;
				notify_.set_notify("DuplicatePlayerName");
				return;
			}
			if (!MtWordManager::Instance()->NoSensitiveWord(message_->name())) {
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_INVALID_NAME;
				notify_.set_notify("InvalidPlayerName");
				return;
			}
			if (ActiveModel::Base<PlayerDBInfo>(dbctx).CheckExist({ {"account_guid", account->guid() } })) {
				db_info_ = ActiveModel::Base<PlayerDBInfo>(dbctx).FindOne({ { "account_guid", account->guid() } });
				player_ = LoadPlayer(db_info_, dbctx);
				if (player_ == nullptr) {
					ZXERO_ASSERT(false) << "load player fail";
					create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
					notify_.set_notify("UnknownError");
					return;
				}
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_SUCCESS;
				notify_.set_notify("NewPlayerOk");
				return;
			}
			auto base_config = MtActorConfig::Instance().GetBaseConfig(message_->player_actor_base_id());
			if (base_config == nullptr) {
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_INVALID_CONFIG;
				notify_.set_notify("InvalidPlayerConfig");
				return;
			}
			auto hairs = MtActorConfig::Instance().ProfHairs(base_config->profession);
			if (std::find(hairs.begin(), hairs.end(), message_->hair()) == hairs.end()) {
				notify_.set_notify("InvalidHairValue(0-20)");
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
				return;
			}
			if (!MtWordManager::Instance()->RegName(message_->name())) {
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_DUMPLICATE_NAME;
				notify_.set_notify("DuplicatePlayerName");
				return;
			}

			auto init_scene = MtSceneManager::Instance().DefaultScene(message_->camp());
			if (init_scene == nullptr) {
				ZXERO_ASSERT(false) << "invalid camp data ! account_guid=" << account->guid();
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_INVALID_CONFIG;
				notify_.set_notify("InvalidPlayerConfig");
				return;
			}
			Packet::Position spawn_pos = init_scene->SpawnPos(0);
			if (message_->camp() == Camp::Alliance)
				spawn_pos = init_scene->SpawnPos(1);
			else
				spawn_pos = init_scene->SpawnPos(2);

			auto guid = MtGuid::Instance().GenGuid<PlayerDBInfo>();
			db_info_ = ActiveModel::Base<PlayerDBInfo>(dbctx).Create({
				{ "guid", guid },
				{ "account_guid", account->guid()},
				{ "inited", false },
				{ "hook_stage_tracker", 0 },
				{ "last_logout_time", 0 },
				{ "refresh_time", 0 },	  //保证新号创建时是newday
				{ "create_time", MtTimerManager::Instance().CurrentDate() },
				{ "basic_info.guid", guid }, //保证第一时间进db
				{ "basic_info.level", 1},
				{ "basic_info.name" , message_->name()},
				{ "basic_info.init_actor_id", (zxero::int32)message_->player_actor_base_id() },
				{ "basic_info.icon", icon_},
				{ "basic_info.scene_id", init_scene->SceneId()},			
				{ "basic_info.camp", message_->camp()},
				{ "basic_info.scene_pos.x", spawn_pos.x() },
				{ "basic_info.scene_pos.y", spawn_pos.y() },
				{ "basic_info.scene_pos.z", spawn_pos.z() },				
				{ "basic_info.hair", message_->hair() },
				{ "basic_info.master_level", 0}
			});
			if (db_info_ == nullptr) {
				ZXERO_ASSERT(false) << "create player fail";
				create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
				notify_.set_notify("UnknownError");
				return;
			}
			player_ = boost::make_shared<MtPlayer>(db_info_,account);

			//初始化挂机数据
			auto player_stage_db = ActiveModel::Base<PlayerStageDB>(dbctx).Create({ {"player_guid", db_info_->guid()} });
			auto default_stage_id = MtSceneStageManager::Instance().DefaultStageId(player_->GetCamp());
			player_stage_db->set_current_stage(default_stage_id);
			player_->SetPlayerStageDbInfo(player_stage_db);		
			player_->AddPlayerStage(default_stage_id);

			//加载玩家设置
			auto setttings = ActiveModel::Base<PlayerSetting>(dbctx).Create({
				{ "guid",  db_info_->guid() },
				{ "ignore_add_friend", false},
				{ "reject_fight", true},
			});
			player_->SetPlayerSetting(setttings);
			//db_info_->mutable_hook_option()->SetInitialized();
			//初始化挂机数据
			auto hook_option = db_info_->mutable_hook_option();
			auto default_option = Packet::HookOption::default_instance();//copy from 不支持，蛋疼！
			hook_option->set_auto_buy_count(default_option.auto_buy_count());
			hook_option->set_auto_buy_potion(default_option.auto_buy_potion());
			hook_option->set_auto_equip_durable(default_option.auto_equip_durable());
			hook_option->set_auto_potion_hp(default_option.auto_potion_hp());
			hook_option->set_auto_potion_id(default_option.auto_potion_id());
			hook_option->set_auto_refix(default_option.auto_refix());
			hook_option->set_auto_split_blue(default_option.auto_split_blue());
			hook_option->set_auto_split_green(default_option.auto_split_green());
			hook_option->set_auto_use_potion(default_option.auto_use_potion());

			db_info_->mutable_token_data()->SetInitialized();

			ContainerManager::Instance().LoadPlayerContainer(player_, dbctx);
			MtMissionManager::Instance().LoadPlayerMission(player_, dbctx);
			player_->AddHero(db_info_->mutable_basic_info()->init_actor_id(), Packet::ActorType::Main, "INIT");
			player_->MainActor()->DbInfo()->set_hair(message_->hair());
			auto main_actor_guid = player_->MainActor()->Guid();
			for (auto type = Packet::ActorFigthFormation_MIN + 1; type <= Packet::ActorFigthFormation_MAX; ++type) {
				auto formation = ActiveModel::Base<ActorFormation>(dbctx).Create({
					{"player_guid" , db_info_->guid()},
					{"type" ,type},
					{"actor_guids_0", main_actor_guid}
				});
				if (formation == nullptr) {
					ZXERO_ASSERT(false) << "create actor formation fail:" << type;
					create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_UNKNOWN_ERROR;
					notify_.set_notify("UnknownError");
					return;
				}
				player_->AddActorFormation(formation);
			}
			MtMarketManager::Instance().LoadPlayerMarket(player_, dbctx);
			MtDataCellManager::Instance().LoadDataCell(player_, dbctx);
			MtActivityManager::Instance().LoadPlayerActivity(player_, dbctx);
			player_->AddItemById(Packet::TokenType::Token_Gold, MtConfigDataManager::Instance().FindConfigValue("init_gold")->value1(), Config::ItemAddLogType::AddType_PlayerInit);
			create_result_ = CreatePlayerReply_Result::CreatePlayerReply_Result_SUCCESS;
			notify_.set_notify("CreatePlayerOk");

			//mqdblopg 玩家注册
			std::string tstr = MtTimerManager::Instance().NowTimeString();
			std::string dstr = MtTimerManager::Instance().TimeString2DateString(tstr);
			boost::format fmt2("REPLACE INTO `player_register_log` (`PlayerID`,`ServerID`,`PartnerID`,`UserID`,`DeviceIdentifier`,`CrDate`,`CrTime`) VALUES('%llu', '%d', '%d', '%s', '%s', '%s', '%s'); ");
			fmt2% db_info_->guid()%MtConfig::Instance().server_group_id_%account->partnerid() % account->userid() % account->udid() % dstr %tstr;
			Server::Instance().SendLogMessage(fmt2.str());
			

			LOG_INFO << "create player end,account:" << account->guid();
		}
	}

	void CreatePlayerWork::done()
	{
		dbwork::done();
		if (session_->get_state() != client_session_state::kBeforEnteringScene) {
			LOG_WARN << "client status wrong after load account info:" << session_->get_state();
		}
		else {
			auto account = session_->get_account();
			if (account == nullptr) {
				LOG_WARN << "invalid account info!";
				return;
			}
			LOG_INFO << "create player send reply,account:" << account->guid();
			Packet::CreatePlayerReply reply;
			reply.set_result(create_result_);
			session_->send(reply);
			session_->send(notify_);
			if (create_result_ == CreatePlayerReply_Result::CreatePlayerReply_Result_SUCCESS) {
				AccountPlayerListReply player_list;
				player_list.add_player_list()->CopyFrom(*db_info_);
				session_->send(player_list);

				Server::Instance().OnLoadPlayer(player_);
				player_->DBLog("RaidStage", "0",0);
			}
		}
	}
	
	IMPLEMENT_DBWORK(SceneStageLoader)
	{
		LOG_INFO << "Scene Stage Loader Start";
		auto result = ActiveModel::Base<Packet::SceneStageInfo>(dbctx).FindAll({});
		if (result.empty()) {
			MtSceneStageManager::Instance().InitSceneStageInfo(dbctx);
		} else {
			MtSceneStageManager::Instance().OnLoadSceneStageInfo(result);
		}
		LOG_INFO << "Scene Stage Loader Done";
	}

	void SceneStageLoader::done()
	{
	}

	IMPLEMENT_DBWORK(DeleteOnePlayer)
	{
		ActiveModel::Base<PlayerDBInfo>(dbctx).Delete({ { "guid", guid_ } });
	}

	DeleteOnePlayer::DeleteOnePlayer(uint64 guid,
    const boost::shared_ptr<client_session> session) 
    :guid_(guid), session_(session)
	{

	}

  void DeleteOnePlayer::done()
  {
    session_->close(true);
  }

	IMPLEMENT_DBWORK(PlayerLoginLoadTask)
	{
		auto account = session_->get_account();
		if (account == nullptr) {
			LOG_WARN << "invalid account info!";
			return;
		}
		player_db_infos_ = ActiveModel::Base<PlayerDBInfo>(dbctx).FindAll({ { "account_guid", account->guid() } });
	}

	PlayerLoginLoadTask::PlayerLoginLoadTask(const boost::shared_ptr<client_session>& session)
		:session_(session) {}

	void PlayerLoginLoadTask::done()
	{
		dbwork::done();

		if (session_->get_state() != client_session_state::kBeforEnteringScene) {
			LOG_WARN << "client status wrong after load account info:" << session_->get_state();
		}
		else {
      boost::copy(player_db_infos_ | boost::adaptors::indirected,
        RepeatedFieldBackInserter(reply_.mutable_player_list()));
			session_->send(reply_);
		}
	}

	

	IMPLEMENT_DBWORK(PlayerLoadTask) {
		if (ActiveModel::Base<PlayerDBInfo>(dbctx).CheckExist({ {"guid", guid_} })){
			auto player_db_info = ActiveModel::Base<PlayerDBInfo>(dbctx).FindOne({ {"guid", guid_} });
			player_ = LoadPlayer(player_db_info, dbctx);
		}
	}

	PlayerLoadTask::PlayerLoadTask(uint64 guid, const std::function<void(boost::shared_ptr<MtPlayer>&)>& cb) : guid_(guid), cb_(cb)
	{

	}

	void PlayerLoadTask::done()
	{
		dbwork::done();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false) << "PlayerLoadTask failed:" << guid_;
			return;
		}
		Server::Instance().OnLoadPlayer(player_);
		cb_(player_);
	}

	IMPLEMENT_DBWORK(LoadPlayerZoneInfo) {
		if (playerzonedata_)
		{
			if (ActiveModel::Base<Packet::ZoneInfo>(dbctx).CheckExist({ { "guid" , playerid_ } })) {
				auto zone_data = ActiveModel::Base<Packet::ZoneInfo>(dbctx).FindOne({ { "guid", playerid_ } });
				playerzonedata_->zone_data_->CopyFrom(*zone_data.get());
			}

			auto messageboardlist = ActiveModel::Base<Packet::MessageBoard>(dbctx).FindAll({ { "owerid", playerid_ } });
			for (auto& message_ : messageboardlist) {
				playerzonedata_->messageboardlist_.push_back(message_);
			}
		}
		else
		{
			ZXERO_ASSERT(false);
		}
	}

	LoadPlayerZoneInfo::LoadPlayerZoneInfo(uint64 playerid,const std::function<void(boost::shared_ptr<PlayerZone>&)>& cb) : playerid_(playerid),cb_(cb)
	{
		playerzonedata_ = boost::make_shared<PlayerZone>();
	}

	void LoadPlayerZoneInfo::done()
	{
		dbwork::done();
		if (cb_ && playerzonedata_)
		{
			cb_(playerzonedata_);
		}
	}

	IMPLEMENT_DBWORK(LoadPlayerScenePlayerInfo) {
		playersceneinfo_ = Server::Instance().LoadScenePlayerInfoByGuid(dbctx, targetid_);
	}

	LoadPlayerScenePlayerInfo::LoadPlayerScenePlayerInfo( uint64 targetid, const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)>& cb) : targetid_(targetid),cb_(cb)
	{
		
	}

	void LoadPlayerScenePlayerInfo::done()
	{
		dbwork::done();
		if (playersceneinfo_)
		{
			Server::Instance().InsertScenePlayerInfo(targetid_, playersceneinfo_);
		}
		cb_(playersceneinfo_);
	}

	IMPLEMENT_DBWORK(LoadPlayerScenePlayerInfoByName) {
		playersceneinfo_ = Server::Instance().LoadScenePlayerInfoByName(dbctx, targetname_);
	}

	LoadPlayerScenePlayerInfoByName::LoadPlayerScenePlayerInfoByName(std::string targetname, const std::function<void(boost::shared_ptr<Packet::ScenePlayerInfo>&)>& cb) : targetname_(targetname), cb_(cb)
	{

	}

	void LoadPlayerScenePlayerInfoByName::done()
	{
		dbwork::done();
		if (playersceneinfo_)
		{
			Server::Instance().InsertScenePlayerInfo(playersceneinfo_->basic_info().guid(), playersceneinfo_);
		}
		cb_(playersceneinfo_);
	}

	IMPLEMENT_DBWORK(ServerLoader)
	{
		Server::Instance().OnLoadSelf(dbctx);
	}

	void ServerLoader::done()
	{
		dbwork::done();
	}

	IMPLEMENT_DBWORK(GuildLoader)
	{
		MtGuildManager::Instance().OnLoadAll(dbctx);
	}

	void GuildLoader::done()
	{
		dbwork::done();
	}

	IMPLEMENT_DBWORK(CapturePointLoader) 
	{
		cps_ = ActiveModel::Base<Packet::CapturePointDB>(dbctx).FindAll({});
		guild_cps_ = ActiveModel::Base<Packet::GuildCapturePointDB>(dbctx).FindAll({});
		std::string sql = (boost::format(" `battle_time` >= %1% ") %
			((MtTimerManager::Instance().CurrentTime() / 1000) - hours(24 * 7).total_seconds())).str();
		logs_ = ActiveModel::Base<Packet::GuildCaptureBattleLogDB>(dbctx).FindBy(sql, 1000);
	}

	void CapturePointLoader::done()
	{
		MtCapturePointManager::Instance().OnLoadAll(cps_, guild_cps_, logs_);
	}

	IMPLEMENT_DBWORK(TopListLoader)
	{
		MtTopRankManager::Instance().OnLoadAll(dbctx);
	}

	void TopListLoader::done()
	{
		dbwork::done();
	}
	
	IMPLEMENT_DBWORK(LoadArenaDailyRewardGuids)
	{
		const boost::shared_ptr< sql::Connection > connection = dbctx->get_connection();
		if (connection)
		{
			std::string sql = "select rank,player_guid from ArenaUser where is_robot = 0;";
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(sql));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			while (dbresult->next())
				rank_and_guids_.push_back({ dbresult->getInt("rank"),
					dbresult->getUInt64("player_guid") });
		}
	}

	void LoadArenaDailyRewardGuids::done()
	{
		MtArenaManager::Instance().DrawDailyArenaReward(rank_and_guids_);
	}

	IMPLEMENT_DBWORK(LoadPlayerArenaMainData)
	{
		auto now_date = seconds_transform_date(now_second());
		if (main_user_ == nullptr) {
			if (ActiveModel::Base<DB::ArenaUser>(dbctx).CheckExist({ { "player_guid", self_->Guid() } })) {
				main_user_ = ActiveModel::Base<DB::ArenaUser>(dbctx).FindOne({ { "player_guid", self_->Guid() } });
				auto updated_time = seconds_transform_ptime(main_user_->updated_at());
				if (now_date.day_of_year() != updated_time.date().day_of_year()) {
					main_user_->set_updated_at(now_second());
				}
			} else {
				auto rank = ActiveModel::Base<DB::ArenaUser>(dbctx).Count() + 1;
				main_user_ = ActiveModel::Base<DB::ArenaUser>(dbctx).Create({
					{ "rank", rank },{ "yesterday_rank", rank },{ "player_guid", self_->Guid() },
					{ "score", self_->BattleScore() },
					{ "is_robot", false },{ "updated_at", now_second() },{ "log","" },
					{ "last_reward_time", now_second() - hours(24).total_seconds() } },
					true);
				main_user_->mutable_trial_copy()->CopyFrom(trial_copy_);
				ActiveModel::Base<DB::ArenaUser>(dbctx).Save(main_user_);
				copy_formation_ = true;
			}
		}
		auto rank = main_user_->rank();
		if (rank <= 4) { //前4名只能看到其他三人
			auto condition = (boost::format(" `rank` <= 4 and `rank` != %1% order by `rank`") % rank).str();
			targets_ = ActiveModel::Base<DB::ArenaUser>(dbctx).FindBy(condition);
		} else {
			std::vector<std::pair<int32, int32>> rank_ranges;
			rank_ranges.push_back({ int32(std::ceil(rank * 0.4)), int32(std::floor(rank * 0.59)) });
			rank_ranges.push_back({ int32(std::ceil(rank * 0.6)), int32(std::floor(rank * 0.79)) });
			rank_ranges.push_back({ int32(std::ceil(rank * 0.8)), int32(std::floor(rank * 0.99)) });
			LOG_INFO << "[arena load] player:" << self_->Guid() << ",ranks:" << rank_ranges;
			for (auto& range : rank_ranges) {
				auto random_rank = int32(rand_gen->intgen(range.first, range.second ));
				if (random_rank == rank) continue; //不能选自己
				if (std::find_if(std::begin(targets_), std::end(targets_), boost::bind(&DB::ArenaUser::rank, _1) == random_rank) == std::end(targets_)) {
					auto player = ActiveModel::Base<DB::ArenaUser>(dbctx).FindOne({ { "rank", random_rank } });
					if (player != nullptr) targets_.push_back(player);
				}
			}
		}
		std::for_each(std::begin(targets_), std::end(targets_), [&](auto& target) {
			if (now_date.day_of_year() != seconds_transform_date(target->updated_at()).day_of_year()) {
				target->set_updated_at(now_second());
			}
			LOG_INFO << "[arena load] player:" << self_->Guid()
				<< ",target:" << target->player_guid()
				<< ",robot:" << target->is_robot()
				<< ",rank:" << target->rank()
				<< ",online:" << Server::Instance().HasPlayer(target->player_guid());
			if (target->is_robot() == false && Server::Instance().HasPlayer(target->player_guid()) == false)
			{
				if (ActiveModel::Base<PlayerDBInfo>(dbctx).CheckExist({ { "guid", target->player_guid() } })) {
					auto db_info = ActiveModel::Base<PlayerDBInfo>(dbctx).FindOne({ {"guid", target->player_guid()} });
					if (db_info) {
						auto player = LoadPlayer(db_info, dbctx);
						if (player != nullptr) {
							LOG_INFO << "[arena load] player:" << self_->Guid()
								<< ", target:" << target->player_guid() << " ok";
							load_players_.push_back(player);
						} else {
							LOG_ERROR << "[arena load] player:" << self_->Guid()
								<< ", target:" << target->player_guid() << " empty;";
						}
					} else {
						LOG_ERROR << "[arena load] player:" << self_->Guid()
							<< ", target:" << target->player_guid() << " dbinfo empty";
					}
				} else {
					LOG_ERROR << "[arena load] player:" << self_->Guid()
						<< ", target:" << target->player_guid() << " check exist false";
				}
			}
		});
		if (new_score_ > 0) main_user_->set_score(new_score_);
	}
	
	LoadPlayerArenaMainData::LoadPlayerArenaMainData(const boost::shared_ptr<MtPlayer>& self, const boost::shared_ptr<DB::ArenaUser>& main_user, int32 new_score /*= 0*/) :self_(self), main_user_(main_user), new_score_(new_score)
	{
		trial_copy_.set_init_actor_id(self_->GetScenePlayerInfo()->init_actor_id());
		auto battle_actors = self_->BattleActor(Packet::ActorFigthFormation::AFF_PVP_GUARD);
		for (auto& actor : battle_actors) {
			actor->SerializationToMessage(*trial_copy_.add_actors());
		}
	}

	void LoadPlayerArenaMainData::done()
	{
		dbwork::done();
		std::for_each(std::begin(load_players_), std::end(load_players_),
			boost::bind(&Server::OnLoadPlayer, boost::ref(Server::Instance()), _1));
		if (copy_formation_) {
			auto pvp_guard_formation = self_->GetActorFormation(Packet::ActorFigthFormation::AFF_PVP_GUARD);
			auto pvp_formation = self_->GetActorFormation(Packet::ActorFigthFormation::AFF_PVP_ARENA);
			auto hook_formation = self_->GetActorFormation(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
			pvp_guard_formation->mutable_actor_guids()->CopyFrom(hook_formation->actor_guids());
			pvp_formation->mutable_actor_guids()->CopyFrom(hook_formation->actor_guids());
		}
		MtArenaManager::Instance().OnLoadPlayerArenaMainDataDone(main_user_, targets_);
	}

	IMPLEMENT_DBWORK(LoadPlayerGuildApplyData) {
		if (ActiveModel::Base<PlayerDBInfo>(dbctx).CheckExist({ { "guid", applyplayerid_ } })) {
			auto db_info = ActiveModel::Base<PlayerDBInfo>(dbctx).FindOne({ { "guid", applyplayerid_ } });
			if (db_info) {
				load_player_ = LoadPlayer(db_info, dbctx);
			}
		}
	}
	
	LoadPlayerGuildApplyData::LoadPlayerGuildApplyData(const boost::shared_ptr<MtPlayer>& self, uint64 applyplayerid) :self_(self), applyplayerid_(applyplayerid)
	{
		
	}

	void LoadPlayerGuildApplyData::done()
	{
		dbwork::done();
		if (load_player_ == nullptr)
		{
			self_->SendClientNotify("PlayerNoExist", -1, -1);
			return;
		}
		Server::Instance().OnLoadPlayer(load_player_);
		
		if (load_player_->GetGuildID() != INVALID_GUID)
		{
			self_->SendClientNotify("Guild_Have", -1, -1);
			return;
		}
		auto guild = MtGuildManager::Instance().GetGuildByid(self_->GetGuildID());
		if (guild == nullptr)
			return ;
		Packet::ResultCode ret = guild->AddGuild(load_player_);
		self_->SendCommonResult(Packet::ResultOption::Guild_Opt, ret);
	}

	IMPLEMENT_DBWORK(WorldBossLoader)
	{
		MtWorldBossManager::Instance().OnLoadAll(dbctx);
	}

	void WorldBossLoader::done()
	{
	}

	IMPLEMENT_DBWORK(LoadTrialPlayerInfoWork)
	{
		LOG_INFO << "[trial_field] Load Start. player:" << player_->Guid();
		if (ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).CheckExist({ {"guid", player_->Guid()} })) {
			info_ = ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).FindOne({ {"guid", player_->Guid()} });
			info_->mutable_target()->clear_targets();
		} else {
			//info_ = ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).Create({ {"guid", player_->Guid()} });
			info_ = boost::make_shared<DB::PlayerTrialInfo>();
		}
		if (info_ == nullptr) {
			ZXERO_ASSERT(false) << "LoadTrialPlayerInfoWork failed! guid=" << player_->Guid();
			return;
		}
		int32 max_score = (int32)(player_power_count_ * ((6 + wave_  + 1) / 10.f));
		LOG_INFO << "[trial_field] Load Start. player:" << player_->Guid()
			<< ",ware:" << wave_ << ",score:" << max_score;
		std::string sql = (boost::format("`player_guid` != %1% and `score` <= %2% ORDER BY `score` DESC") % player_->Guid() % max_score).str();
		auto arena_user_infos = ActiveModel::Base<DB::ArenaUser>(dbctx).FindBy(sql, 3);
		info_->clear_target();
		info_->set_guid(player_->Guid());
		auto target = info_->mutable_target();
		for (const boost::shared_ptr<DB::ArenaUser>& arena_user : arena_user_infos) {
			LOG_INFO << "[trial_field] Loaing. target:" << arena_user->player_guid()
				<< ",score:" << arena_user->score()
				<< ",is_robot" << arena_user->is_robot();
			if (arena_user->is_robot()) {
				auto robot = MtArenaManager::Instance().FindRobotPlayer(*arena_user);
				if (robot != nullptr) {
					auto trial_target_formation = target->add_targets();
					trial_target_formation->set_passed(false);
					trial_target_formation->set_posid(-1);
					trial_target_formation->set_rewarded(false);
					DB::TrialPlayerInfo& player_info = *trial_target_formation->mutable_player();
					player_info.set_actor_config_id(robot->ConfigId());
					player_info.set_name(robot->name_);
					player_info.set_guid(arena_user->player_guid());
					for (auto& actor : robot->actors_) {
						MtArenaManager::Instance().ArenaActor2ActorFullInfo(actor,
							*trial_target_formation->add_actors());
					}
				} else {
					ZXERO_ASSERT(false);
				}
			} else {
				auto guid = arena_user->player_guid();
				auto trial_target_formation = target->add_targets();
				trial_target_formation->set_passed(false);
				trial_target_formation->set_posid(-1);
				trial_target_formation->set_rewarded(false);
				DB::TrialPlayerInfo& player_info = *trial_target_formation->mutable_player();
				player_info.set_actor_config_id(arena_user->trial_copy().init_actor_id());				
				//能用真名用真名
				auto player_db_info = ActiveModel::Base<PlayerDBInfo>(dbctx).FindOne({ { "guid", guid } });
				if (player_db_info != nullptr) {
					player_info.set_name(player_db_info->basic_info().name());
				}
				else {
					player_info.set_name(MtWordManager::Instance()->RandomName(Packet::Gender::MALE));
				}				
				player_info.set_guid(guid);
				trial_target_formation->mutable_actors()->CopyFrom(arena_user->trial_copy().actors());
			}
		}
		LOG_INFO << "[trial_field] Load end. player:" << player_->Guid()
			<< ",load " << info_->target().targets_size() << " loaded";
	}


	LoadTrialPlayerInfoWork::LoadTrialPlayerInfoWork(const boost::shared_ptr<MtPlayer>& player, int32 wave, int32 player_power_count) :player_(player),
		wave_(wave), player_power_count_(player_power_count)
	{
		if (player_power_count_ < player_->BattleScore()) {
			LOG_WARN << "[trial_field] player power_count:"
				<< player_power_count_ 
				<< ",battle_score:" << player_->BattleScore();
			player_power_count_ = player_->BattleScore();
		}
	}

	void LoadTrialPlayerInfoWork::done()
	{
		auto target = info_->mutable_target();
		auto need_copy = 3 - target->targets_size();
		LOG_INFO << "[trial_field] Load done. player:" << player_->Guid()
			<< ",curr_size:" << info_->target().targets_size();
		for (auto i = 0; i < need_copy; ++i) {
			LOG_INFO << "[trial_field] Load Start. player:" << player_->Guid()
				<< ", copy self as target," << i;
			auto trial_target_formation = target->add_targets();
			trial_target_formation->set_passed(false);
			trial_target_formation->set_posid(-1);
			trial_target_formation->set_rewarded(false);
			DB::TrialPlayerInfo& player_info = *trial_target_formation->mutable_player();
			player_info.set_actor_config_id(player_->GetScenePlayerInfo()->init_actor_id());
			auto gender = player_->MainActor()->DbInfo()->gender();
			player_info.set_name(MtWordManager::Instance()->RandomName(gender));
			player_info.set_guid(MtGuid::Instance().GenGuid<Packet::PlayerDBInfo>());
			auto battle_actors = player_->BattleActor(Packet::ActorFigthFormation::AFF_PVP_GUARD);
			for (auto& actor : battle_actors) {
				actor->SerializationToMessage(*trial_target_formation->add_actors());
			}
		}
		for (auto it = target->mutable_targets()->begin(); it != target->mutable_targets()->end(); ++it) {
			DB::TrialTarget& trial_target = *it;
			for (auto actor_it = trial_target.mutable_actors()->begin(); actor_it != trial_target.mutable_actors()->end(); ++actor_it) {
				Packet::ActorFullInfo& actor_full = *actor_it;
				actor_full.mutable_actor()->set_guid(MtGuid::Instance().GenGuid<Packet::PlayerDBInfo>());
			}
		}
		TrialFieldManager::Instance().OnDataLoad(wave_, info_, player_);
	}


	IMPLEMENT_DBWORK(LoadAllPlayerMailPay)
	{
		const boost::shared_ptr< sql::Connection > connection = dbctx->get_connection();
		if (connection)
		{
			//七天内的才可以
			uint32 daytimebefore7days = MtTimerManager::Instance().TimeDateFromNow(-7*24*3600);
			int32 refresh_year = MtTimerManager::Instance().GetYearFromUT(daytimebefore7days);
			int32 refresh_month = MtTimerManager::Instance().GetMonthFromUT(daytimebefore7days);
			int32 refresh_day = MtTimerManager::Instance().GetDayFromUT(daytimebefore7days);
			int32 refresh_week = MtTimerManager::Instance().GetWeekFromUT(daytimebefore7days);
			int32 refresh_hour = MtTimerManager::Instance().GetHourFromUT(daytimebefore7days);
			int32 refresh_minute = MtTimerManager::Instance().GetMinuteFromUT(daytimebefore7days);
			refresh_year; refresh_month; refresh_day; refresh_week; refresh_hour; refresh_minute;
			std::string select_ = (boost::format("select guid from PlayerDBInfo where last_logout_time >= %1%;") % daytimebefore7days).str();
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			while (dbresult->next())
			{
				allplayerguild.push_back(dbresult->getUInt64("guid"));
			}
		}
	}
	LoadAllPlayerMailPay::LoadAllPlayerMailPay(std::string title, std::string content, google::protobuf::RepeatedPtrField<Packet::ItemCount> items)
	{
		title_ = title;
		content_ = content;
		items_.CopyFrom(items);
	}
	void LoadAllPlayerMailPay::done()
	{
		LOG_INFO << "LoadAllPlayerMailPay :"<< allplayerguild.size();
		static_message_manager::Instance().mail_payback_g(allplayerguild, title_, content_, items_);
	}

	IMPLEMENT_DBWORK(serial_load_work)
	{
		try
		{
			const boost::shared_ptr< sql::Connection > connection = dbctx->get_connection();
			if (connection)
			{
				std::string select_ = " select max(guid) FROM GuildDBInfo;";
				boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
				boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
				ZXERO_ASSERT(dbresult && dbresult->next());
				if (dbresult)
				{
					max_serial_ = dbresult->getUInt64(1);
				}
			}
			uint64 min_s = INVALID_GUID;
			if (max_serial_ <= min_s)
				max_serial_ = min_s;
		}
		catch (sql::SQLException& e)
		{
			LOG_ERROR << "serial_load_work error!";
			//app::exit(0);
			throw e;
		}

	}

	void serial_load_work::done()
	{
		dbwork::done();

		if (max_serial_ != INVALID_GUID)
		{
			MtGuildManager::Instance().SetSerial(max_serial_);
		}
		LOG_TRACE << "serial_load_work done" << max_serial_;
	}

serial_load_work::serial_load_work(uint32 server_id)
	:server_id_(server_id)
	, max_serial_(INVALID_GUID)
{

}

/*
IMPLEMENT_DBWORK(PlayerStagesLoadWork)
{
	if (ActiveModel::Base<Packet::PlayerStages>(dbctx).CheckExist({ { "guid", player_guid_ } })) {
		data_ = ActiveModel::Base<Packet::PlayerStages>(dbctx).FindOne({ { "guid", player_guid_ } });
	} else {
		//info_ = ActiveModel::Base<DB::PlayerTrialInfo>(dbctx).Create({ {"guid", player_->Guid()} });
		data_ = boost::make_shared<Packet::PlayerStages>();
		MtSceneStageManager::Instance().InitPlayerStages(data_.get());
	}
}

void PlayerStagesLoadWork::done()
{
	MtSceneStageManager::Instance().OnLoadPlayerStages(data_);
}*/

IMPLEMENT_DBWORK(UpdateStrongHoldWork)
{
	try
	{
		const boost::shared_ptr< sql::Connection > connection = dbctx->get_connection();
		for (auto& strong_hold : (*data_.mutable_strong_holds())) {
			std::string sql_pre = R"(
					SELECT `player_guid`,`actor_config_id`,`star`,
						`color`,`score`,`level`,`hair`
						FROM ActorBasicInfo WHERE `player_guid` IN(%1%,%2%,%3%,%4%,%5%) AND `type` = %6%
				)";
			std::string sql = (boost::format(sql_pre) % strong_hold.heros(0).player_guid()
				% strong_hold.heros(1).player_guid()
				% strong_hold.heros(2).player_guid()
				% strong_hold.heros(3).player_guid()
				% strong_hold.heros(4).player_guid()
				% Packet::ActorType::Main).str();
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(sql));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			int32 score = 0;
			int32 index = 0;
			while (dbresult->next())
			{
				auto& hero = (*strong_hold.mutable_heros(index++));
				hero.set_player_guid(dbresult->getUInt64("player_guid"));
				hero.set_actor_config_id(dbresult->getInt("actor_config_id"));
				hero.set_star(Packet::ActorStar(dbresult->getInt("star")));
				hero.set_color(Packet::ActorColor(dbresult->getInt("color")));
				hero.set_level(dbresult->getInt("level"));
				hero.set_hair(dbresult->getInt("hair"));
				score += dbresult->getInt("score");
			}
			strong_hold.set_score(score);
		}
		done_ = true;
	}
	catch (sql::SQLException& e)
	{
		LOG_ERROR << "update strong hold:" << guild_guid_ << " fails:" << e.what();
	}
}

void UpdateStrongHoldWork::done()
{
	if (done_)
		MtCapturePointManager::Instance().OnStrongHoldUpdateDone(guild_guid_, data_);
}

IMPLEMENT_DBWORK(LoadCapturePlayerAndActor)
{
	try
	{
		for (auto guid : player_guids_) {
			if (guid == INVALID_GUID)
				continue;
			if (ActiveModel::Base<PlayerDBInfo>(dbctx).CheckExist({ { "guid" , guid } }) == false)
				return;
			auto db_info = ActiveModel::Base<PlayerDBInfo>(dbctx).FindOne({ { "guid", guid } });
			auto player = LoadPlayer(db_info, dbctx);
			if (player != nullptr)
				players_.push_back(player);

		}
		success_ = true;
	}
	catch (sql::SQLException& e)
	{
		LOG_WARN << "load_capture_player_and_actor:" << attacker_guid_
			<< "," << defender_guid_ << " load fails:" << e.what();
	}
}


LoadCapturePlayerAndActor::LoadCapturePlayerAndActor(uint64 attacker_guid,
	uint64 defender_guid, int32 chapter_id, std::set<uint64>& player_guids)
	:attacker_guid_(attacker_guid), defender_guid_(defender_guid),
	chapter_id_(chapter_id)
{
	for (auto guid : player_guids) {
		if (guid == INVALID_GUID) continue;;
		if (Server::Instance().FindPlayer(guid) == nullptr) {
			player_guids_.insert(guid);
		}
	}
}

void LoadCapturePlayerAndActor::done()
{
	for (auto& p : players_)
		Server::Instance().OnLoadPlayer(p);
	MtCapturePointManager::Instance().OnLoadPlayerAndActorDone(
		attacker_guid_, defender_guid_, chapter_id_, success_);
}

IMPLEMENT_DBWORK(LoadBattleReply)
{
	try
	{
		if (ActiveModel::Base<Packet::BattleReply>(dbctx).CheckExist({ { "guid", reply_guid_ } }) == false) {
			LOG_INFO << "[LoadBattleReply], check exist false:" << reply_guid_;
			return;
		}
		reply_ = ActiveModel::Base<Packet::BattleReply>(dbctx).FindOne({ {"guid", reply_guid_} });

	}
	catch (sql::SQLException& e)
	{
		LOG_WARN << "[LoadBattleReply]," << reply_guid_
			<< ", load fails:" << e.what();
	}
}

void LoadBattleReply::done()
{
	MtBattleReplyManager::Instance().OnBattleReplyLoad(reply_, player_guid_);
}



IMPLEMENT_DBWORK(TowerInfoLoader)
{
	try
	{
		const boost::shared_ptr< sql::Connection > connection = dbctx->get_connection();
		if (connection)	{
			infos_ = ActiveModel::Base<DB::PlayerTowerInfo>(dbctx).FindBy("1=1",10000);
		}
		LOG_INFO << "[TowerInfoLoader], ok, count =" << infos_.size();

	}
	catch (sql::SQLException& e)
	{
		LOG_WARN << "[TowerInfoLoader],"<< ", load fails:" << e.what();
	}
}

void TowerInfoLoader::done()
{
	MtTowerManager::Instance().OnDBLoad(infos_);
}

}