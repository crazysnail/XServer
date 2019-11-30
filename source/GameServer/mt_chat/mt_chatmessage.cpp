#include "ChatMessage.pb.h"
#include "EnterScene.pb.h"
#include "SceneStage.pb.h"
#include "MissionPacket.pb.h"
#include "MonsterConfig.pb.h"
#include "SceneCommon.pb.h"
#include "Base.pb.h"
#include "SceneObject.pb.h"
#include <SceneStageConfig.pb.h>
#include <G2SMessage.pb.h>
#include <S2GMessage.pb.h>
#include <Team.pb.h>
#include "BaseConfig.pb.h"
#include "../base/client_session.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_config/mt_config.h"
#include "../mt_player/mt_player.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_stage/mt_player_stage.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_market/mt_market.h"
#include "../mt_skill/skill_context.h"
#include "../mt_server/mt_server.h"
#include "../mt_item/mt_container.h"
#include "../mt_mission/mt_mission.h"
#include "../mt_mission/mt_mission_proxy.h"
#include "../mt_item/mt_item.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mail/static_message_manager.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../mt_battle_field/mt_battle_field_manager.h"
#include "../mt_battle_field/mt_battle_field.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_item/mt_item_package.h"
#include "urlcodeing.h"
#include "utils.h"
#include "mt_chatmessage.h"
namespace Mt
{
	void chatmessage_lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<>(ls).
			def(&send_guild_system_message, "lua_send_guild_system_message").
			def(&utils::md5, "lua_md5")
			;
	}

	void scene_chat_message(const boost::shared_ptr<MtScene> scene,
		const boost::shared_ptr<MtPlayer> player,
		const boost::shared_ptr<Packet::ChatMessage>& message)
	{
		try
		{
			if (scene == nullptr)
				return;
			
			if (message->content().size() > 0 && message->content()[0] == '!'
				&& Server::Instance().CanUseGmCmd()) {
				gm_command(scene,player, message->content());
			}
			else
			{
				Packet::ChatChannel channel = message->channel();
				if (player != nullptr)
				{
					Packet::ResultCode ret = player->CanChat(channel);
					if (ret != Packet::ResultCode::ResultOK) {
						player->SendCommonResult(Packet::ResultOption::Chat, ret);
						return ;
					}
				}
				
				int32 cdtime = send_message_byplayer(channel,message->content(),message->chat_time(),player);
				if (cdtime > 0)	{
					player->SetChatChannelCD(channel,cdtime);
				}
			}
		}
		catch (boost::bad_lexical_cast& e)
		{
			LOG_WARN << "execute gm command error " << e.what();
		}
	}

	void fill_message_hyper(const std::string &chatmessage,boost::shared_ptr<Packet::ChatMessage>& message, const boost::shared_ptr<MtPlayer>& player)
	{
		if (player != nullptr && message != nullptr)
		{
			chatmessage;
			//auto iteminfo = message->mutable_iteminfo();
			//player->fill_hyper_iteminfo(chatmessage, *iteminfo);
			message->set_race(player->MainActor()->Race());
			message->set_profession(player->MainActor()->Professions());
			message->set_hair(player->GetScenePlayerInfo()->hair());
			message->set_name(player->GetScenePlayerInfo()->name());
			message->set_target_guid(player->Guid());
		}
	}
	boost::shared_ptr<Packet::ChatMessage> make_player_msg(Packet::ChatChannel channel, const std::string &chatmessage, int32 chat_time, const boost::shared_ptr<MtPlayer>& player)
	{
		auto msg = boost::make_shared<Packet::ChatMessage>();
		msg->set_channel(channel);
		msg->set_chat_time(chat_time);
		msg->set_datetime(now_second());
		msg->set_content(chatmessage);
		//Packet::TeamLeaderUpdate updatemsg;
		//updatemsg.set_playerguid(player->Guid());
		//updatemsg.set_playername(player->Name());
		//updatemsg.set_move_speed(player->GetMoveSpeed());
		//updatemsg.set_charge(false);
		//std::string testchat = updatemsg.SerializeAsString();
		//msg->set_content(testchat);
		fill_message_hyper(chatmessage,msg,player);
		return msg;
	}
	boost::shared_ptr<Packet::ChatMessage> make_normal_msg(Packet::ChatChannel channel, const std::string &chatmessage)
	{
		auto msg = boost::make_shared<Packet::ChatMessage>();
		msg->set_channel(channel);
		msg->set_chat_time(0);
		msg->set_datetime(now_second());
		msg->set_content(chatmessage);
		return msg;
	}
	int32 send_message_byplayer(Packet::ChatChannel channel,const std::string &chatmessage, const int32 chat_time, const boost::shared_ptr<MtPlayer>& player)
	{
		if (player == nullptr)
		{
			ZXERO_ASSERT(false) << "send_message_byplayer player null";
			return -1;
		}
		auto msg = make_player_msg(channel, chatmessage, chat_time, player);
		int32 cdtime = 0;
		switch (channel)
		{
		case Packet::ChatChannel::WORLD:
		{
			MtSceneManager::Instance().BroadCastMessage(msg);
			auto world_msg = boost::make_shared<S2G::WorldChat>();
			world_msg->mutable_msg()->CopyFrom(*msg);
			Server::Instance().SendMessage(world_msg, nullptr);
			cdtime = 30;
		}
		break;
		case Packet::ChatChannel::SCENE:
		{
			if (player->GetRaid() != nullptr) //有raid就先发raid
			{
				player->GetRaid()->BroadCastMessage(*msg);
			}
			else if (player->Scene() != nullptr)
			{
				player->Scene()->BroadcastMessage(*msg);
			}
			cdtime = 2;
		}
		break;
		case Packet::ChatChannel::GUILD:
		{
			if (player->GetGuildID() == INVALID_GUID)
			{
				player->SendClientNotify("Guild_NoExist", -1, -1);
				return -1;
			}
			auto guild_msg = boost::make_shared<S2G::GuildChat>();
			guild_msg->mutable_msg()->CopyFrom(*msg);
			guild_msg->set_guildid(player->GetGuildID());
			Server::Instance().SendMessage(guild_msg, nullptr);
			cdtime = 2;
		}
			break;
		case Packet::ChatChannel::TEAM:
		{
			if (player->GetTeamID() == INVALID_GUID)
			{
				player->SendClientNotify("Team_Out", -1, -1);
				return -1;
			}
			auto team_msg = boost::make_shared<S2G::TeamChat>();
			team_msg->mutable_msg()->CopyFrom(*msg);
			team_msg->set_teamid(player->GetTeamID());
			Server::Instance().SendMessage(team_msg, player);
			cdtime = 2;
		}
		break;
		case Packet::ChatChannel::FARMSG:
		{

			////////////////////////////////////////
			auto token_config = MtConfigDataManager::Instance().FindConfigValue("transform_msg");
			if (token_config == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Chat, Packet::ResultCode::InvalidConfigData);
				return -1;
			}
			/////////////////////////////////////优先扣代币
			if (!player->DelItemById(token_config->value2(), 1, Config::ItemDelLogType::DelType_ChatGarMsg)) {
				//再扣钻石
				if (!player->DelItemById(Packet::TokenType::Token_Crystal, token_config->value1(), Config::ItemDelLogType::DelType_ChatGarMsg)) {
					player->SendCommonResult(Packet::ResultOption::Chat, Packet::ResultCode::CrystalNotEnough);
					return -1;
				}
			}
			MtSceneManager::Instance().BroadCastMessage(msg);
		}
		break;
		case Packet::ChatChannel::TEAMINFO:
		{
			MtSceneManager::Instance().BroadCastMessage(msg);
		}
		break;
		default:
			break;
		}
		return cdtime;
	}

	void send_system_message_toworld(const std::string &chatmessage)
	{
		auto msg = make_normal_msg(Packet::SYSTEM, chatmessage);
		MtSceneManager::Instance().BroadCastMessage(msg);
	}
	void send_system_message_toplayer(const std::string &chatmessage, const boost::shared_ptr<MtPlayer> player)
	{
		if (player != nullptr)
		{
			auto msg = make_normal_msg(Packet::SYSTEM, chatmessage);
			player->SendMessage(*msg.get());
		}
	}
	void send_team_system_message(const std::string &chatmessage, uint64 teamid)
	{
		auto msg = make_normal_msg(Packet::ChatChannel::TEAM, chatmessage);
		auto team_msg = boost::make_shared<S2G::TeamChat>();
		team_msg->mutable_msg()->CopyFrom(*msg);
		team_msg->set_teamid(teamid);
		Server::Instance().SendMessage(team_msg);
	}

	void send_guild_system_message(const std::string &chatmessage, uint64 guildid)
	{
		auto msg = make_normal_msg(Packet::ChatChannel::GUILD, chatmessage);
		auto guild_msg = boost::make_shared<S2G::GuildChat>();
		guild_msg->mutable_msg()->CopyFrom(*msg);
		guild_msg->set_guildid(guildid);
		Server::Instance().SendMessage(guild_msg, nullptr);
	}
	void send_run_massage(const std::string &chatmessage)
	{
		if (chatmessage.empty()) {
			return;
		}
		auto msg = make_normal_msg(Packet::ChatChannel::RUNMSG, chatmessage);
		MtSceneManager::Instance().BroadCastMessage(msg);
	}
	void gm_command(const boost::shared_ptr<MtScene> scene, 
		const boost::shared_ptr<MtPlayer> player,std::string msg)
	{
		std::vector<std::string> command;
		utils::trim_string(msg);
		boost::split(command, msg, boost::is_any_of(","));
		if (command.size() < 1) {
			return;
		}
		if (command.front() == "!disable_create_actor") {
			MtConfig::Instance().disable_create_actor_ = !MtConfig::Instance().disable_create_actor_;
			return;
		}
		if (command.front() == "!output_tick")
		{
			MtConfig::Instance().output_scene_tick_info_ = !MtConfig::Instance().output_scene_tick_info_;
			return;
		}
		if (command.size() == 3 && command.front() == "!use_skill") {
			auto bg = player->BattleGround();
			int32 actor_config_id = boost::lexical_cast<int32>(command[1]);
			if (bg == nullptr) return;
			auto actors = bg->GetAttackers();
			auto actor_it = std::find_if(actors.begin(), actors.end(), boost::bind(&MtActor::ConfigId, _1) == actor_config_id);
			if (actor_it == actors.end()) return;
			MtActor& actor = **actor_it;
			auto skill = actor.FindRuntimeSkill(boost::lexical_cast<int32>(command[2]));
			if (skill == nullptr) return;
			SkillContext ctx;
			ctx.source_.actor_ = &actor;
			ctx.skill_.skill_ = skill;
			ctx.source_.UseSkill(&(ctx.skill_));
			return;
		}
		if (command.front() == "!gm_command") {
			Server::Instance().GetServerInfo()->set_active_gm_command(!Server::Instance().GetServerInfo()->active_gm_command());
			return;
		}
		if (command.front() == "!goto") {
			if (command.size() == 2) { //切场景
				auto scene_id = boost::lexical_cast<int32>(command[1]);
				auto gotoscene = MtSceneManager::Instance().GetSceneById(scene_id);
				if (gotoscene) {
					player->GoTo(scene_id, INVALID_POS);
					return;
				}
			}
			if (command.size() == 3) {//设置坐标
				auto x = boost::lexical_cast<int32>(command[1]);
				auto z = boost::lexical_cast<int32>(command[2]);
				auto new_pos = player->Position();
				new_pos.set_x(x), new_pos.set_z(z);
				if (scene->ValidPos(new_pos)) {
					player->GoTo(scene->SceneId(), { x,new_pos.y(),z });
					return;
				}
			}
		}
		if (command.front() == "!add_monster" && command.size() == 2) {
			if (player->BattleGround() != nullptr) {
				int32 monster_id = boost::lexical_cast<int32>(command[1]);
				player->BattleGround()->AddMonster(1, monster_id);
			}
			return;
		}
		if (command.front() == "!battle" && command.size() == 2) {
			if (player->BattleGround() == nullptr) {
				int32 monster_group_id = boost::lexical_cast<int32>(command[1]);

				auto battle = player->GetBattleManager()->CreateFrontBattle(player.get(), nullptr,{
					{ "script_id",1003 },
					{ "monster_group_id",monster_group_id },
					{ "group_id" , 1},
				});
			}
			return;
		}
		if (command.front() == "!use_skill" && command.size() == 2) {
			auto bg = player->BattleGround();
			if (bg != nullptr) {
				int32 skill_id = boost::lexical_cast<int32>(command[1]);
				auto& attackers = bg->GetAttackers();
				boost::for_each(attackers, boost::bind(&MtActor::ClearRunTimeSkill, _1));
				boost::for_each(attackers, boost::bind(&MtActor::AddRuntimeSkill, _1, skill_id, 1));
			}
			return;
		}
		if (command.front() == "!speed" && command.size() == 2) {
			auto speed = boost::lexical_cast<int32>(command[1]);
			player->SetMoveSpeed(speed * 1000);
			return;
		}
		if (*command.begin() == "!gopos") {
			if (command.size() == 2) { //切场景
				auto pos_id = boost::lexical_cast<int32>(command[1]);
				auto pos_node = MtMonsterManager::Instance().FindScenePos(pos_id);
				if (pos_node != nullptr)
				{
					int32 scene_id = pos_node->scene_id();
					auto gotoscene = MtSceneManager::Instance().GetSceneById(scene_id);
					if (gotoscene) {
						player->GoTo(scene_id, { pos_node->position().x(),pos_node->position().y(),pos_node->position().z() });
						return;
					}
				}
			}
			return;
		}
		if (*command.begin() == "!reset") {
			auto session = player->Session();
			if (session != nullptr) {
				auto old_player = player;

				auto message = boost::make_shared<S2G::S2GCommonMessage>();
				message->set_request_name("S2GDeletePlayer");
				message->add_int64_params(old_player->Guid());
				Server::Instance().SendMessage(message, player);
				scene->OnPlayerLeave(old_player);
				return;
			}
			return;
		}
		if (*command.begin() == "!unlockscene") {
			player->GetPlayerStageProxy()->GmPlayerRaidAllStage();
			return;
		}
		if (*command.begin() == "!addhero") {
			player->AddHero(boost::lexical_cast<zxero::int32>(command[1]), Packet::ActorType::Hero, "GM");
			return;
		}
		if (*command.begin() == "!dice_times") {
			auto times = boost::lexical_cast<zxero::int32>(command[1]);
			auto container = player->DataCellContainer();
			if( container != nullptr )
				container->set_data_32(Packet::CellLogicType::AcRewardCell, 3, times,true);
			return;
		}
		if (*command.begin() == "!logout") {
			player->LogoutStatus(eLogoutStatus::eCanLogout);

			if (command.size() == 2) {
				// 此处只用来做登录测试用！会导致数据回档！ [7/1/2017 SEED]
				// 真正的下线需要等save线程存储完最后一次数据后才能提出	
				player->SendCommonReply("Shutdown", {}, {}, {});

				auto message = boost::make_shared<S2G::CheckOffLine>();
				message->add_guid(player->Guid());
				message->add_stamp(player->CacheStamp());
				Server::Instance().SendMessage(message, player);



			}
			return;
		}
		if (*command.begin() == "!clearskill") {
			std::for_each(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::ClearSkill, _1));
			return;
		}
		if (*command.begin() == "!addskill" && command.size() == 3) {
			auto actor_id = boost::lexical_cast<std::size_t>(command[1]);
			if (actor_id > player->Actors().size())
				return;
			auto skill_id = boost::lexical_cast<zxero::int32>(command[2]);
			auto actor = player->Actors()[actor_id - 1];
			if (actor)
				actor->AddSkill(skill_id);
			return;
		}
		if (*command.begin() == "!reachpos") {
			auto bg = player->BattleGround();
			if (bg != nullptr) {
				bg->OnPlayerReachPos();
			}
			return;
		}
		if (*command.begin() == "!additem" && command.size() > 1)
		{
			auto count = 1;
			if (command.size() > 2) {
				count = boost::lexical_cast<int32>(command[2]);
			}
			player->AddItemByIdWithNotify(boost::lexical_cast<zxero::int32>(command[1]), count, Config::ItemAddLogType::AddType_Gm);
			return;
		}
		if (*command.begin() == "!addexp" && command.size() == 2) {
			auto exp = boost::lexical_cast<int32>(command[1]);
			std::for_each(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::AddExp, _1, exp, true));
			return;
		}
		if (*command.begin() == "!acr_reset"&& command.size() == 2) {
			auto index = boost::lexical_cast<int32>(command[1]);
			player->DataCellContainer()->set_data_32(Packet::CellLogicType::AcRewardCell, index,0,true);
			return;
		}
		if (*command.begin() == "!setlevel" && command.size() == 2) {
			auto level = boost::lexical_cast<int32>(command[1]);
			std::for_each(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::LevelUpTo, _1, level, true, true));
			return;
		}
		if (*command.begin() == "!setactorlevel" && command.size() == 2) {
			auto level = boost::lexical_cast<int32>(command[1]);
			auto actor = player->OtherActors();
			std::for_each(actor.begin(), actor.end(),
				boost::bind(&MtActor::LevelUpTo, _1, level, true, true));
			return;
		}
		if (*command.begin() == "!setstar" && command.size() == 2) {
			auto star = boost::lexical_cast<int32>(command[1]);
			if (Packet::ActorStar_IsValid(star) == false) {
				return;
			}
			auto star_ = (Packet::ActorStar)star;
			std::for_each(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::ChangeStar, _1, star_));
			return;
		}
		if (*command.begin() == "!setcolor" && command.size() == 2) {
			auto color = boost::lexical_cast<int32>(command[1]);
			
			if (Packet::ActorColor_IsValid(color) == false) {
				return;
			}
			for (int32 i = 1; i <= color; ++i) {
				std::for_each(std::begin(player->Actors()), std::end(player->Actors()), boost::bind(&MtActor::ChangeColor, _1, (Packet::ActorColor)i));
			}
			return;
		}
		if (*command.begin() == "!clearitem") {
			if (command.size() == 2 && command[1] == "1") {
				auto econtainer = player->FindContainer(Packet::ContainerType::Layer_Equip);
				if (econtainer != nullptr)
					econtainer->Reset();
			}
			else {
				auto econtainer = player->FindContainer(Packet::ContainerType::Layer_EquipBag);
				if (econtainer != nullptr)
					econtainer->Reset();

				econtainer = player->FindContainer(Packet::ContainerType::Layer_ItemBag);
				if (econtainer != nullptr)
					econtainer->Reset();

				econtainer = player->FindContainer(Packet::ContainerType::Layer_MissionBag);
				if (econtainer != nullptr)
					econtainer->Reset();
			}
			return;
		}
		if (*command.begin() == "!addcrystal"&& command.size() == 2)
		{
			auto count = boost::lexical_cast<int32>(command[1]);
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Crystal, count, Config::ItemAddLogType::AddType_Gm);
			return;
		}
		if (*command.begin() == "!addgold"&& command.size() == 2)
		{
			auto count = boost::lexical_cast<int32>(command[1]);
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Gold, count, Config::ItemAddLogType::AddType_Gm);
			return;
		}
		if (*command.begin() == "!addhonor"&& command.size() == 2)
		{
			auto count = boost::lexical_cast<int32>(command[1]);
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Honor, count, Config::ItemAddLogType::AddType_Gm);
			return;
		}
		if (*command.begin() == "!addnpc")
		{
			if (command.size() == 3)
			{
				auto pos_id = boost::lexical_cast<int32>(command[1]);
				auto index = boost::lexical_cast<int32>(command[2]);
				scene->AddNpc(pos_id, index,-1);
			}
			else if (command.size() == 4)
			{
				auto pos_id = boost::lexical_cast<int32>(command[1]);
				auto index = boost::lexical_cast<int32>(command[2]);
				player->AddNpc(pos_id, index,-1);
			}
			return;
		}
		if (*command.begin() == "!delnpc")
		{
			if (command.size() == 2)
			{
				auto serise_id = boost::lexical_cast<int32>(command[1]);
				scene->DelNpc(serise_id);
			}
			if (command.size() == 3)
			{
				auto serise_id = boost::lexical_cast<int32>(command[1]);
				player->DelNpc(serise_id);
			}
			return;
		}
		if (*command.begin() == "!profile" && command.size() == 1)
		{
			profiler profiler("to test");
			return;
		}
		if (*command.begin() == "!testsystem")
		{
			std::stringstream testmessage;
			testmessage << "friendaddmessage_1|" << player->Guid() <<"|"<< player->Name() <<"|"<< player->Guid();
			player->send_friendsystemmessage(testmessage.str());
			return;
		}
		if (*command.begin() == "!shutdown" && command.size() == 1)
		{
			auto message = boost::make_shared<S2G::S2GCommonMessage>();
			message->set_request_name("S2GShutdown");
			Server::Instance().SendMessage(message, player);
			//Server::Instance().RunInLoop([]() {Server::Instance().OnShutdown(); });
			return;
		}
		if (*command.begin() == "!addmail")
		{
			google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
			for (int32 i = 0; i < 3; i++)
			{
				auto item_ = items.Add();
				item_->set_itemid(20231019);
				item_->set_itemcount(i);
			}

			boost::shared_ptr<Packet::MessageInfo> mail_ = static_message_manager::Instance().create_message(player->Guid(), player,  MAIL_OPT_TYPE::GM_ADD, "gm test", "XX love GG, GG be XXed by XX", &items);
			return;
		}
		if (*command.begin() == "!kickall") {
			auto message = boost::make_shared<S2G::S2GCommonMessage>();
			message->set_request_name("S2GKickAll");
			message->add_int64_params(player->Guid());
			Server::Instance().SendMessage(message,player);
			//Server::Instance().RunInLoop([=]() {Server::Instance().KickAll(player->Guid()); });
			return;
		}
		if (*command.begin() == "!kill_monster")
		{
			if (command.size() == 4) {
				auto id = boost::lexical_cast<int32>(command[1]);
				auto count = boost::lexical_cast<int32>(command[2]);
				auto monster = MtMonsterManager::Instance().FindMonster(id);
				auto battle_type = boost::lexical_cast<int32>(command[3]);
				if (monster != nullptr) {
					for (int32 i = 0; i < count; i++) {
						player->OnLuaFunCall_x("xOnMonsterDead",
						{
							{ "monster_id",monster->id() },
							{ "stage_id",-1 },
							{ "scene_id",player->GetSceneId() },
							{ "monster_class",monster->monster_class() },
							{ "battle_type",battle_type },
							{ "monster_count" , 1 },
						});
					}
				}					
			}
			return;
		}
		if (*command.begin() == "!killall")
		{
			auto battle = player->BattleGround();
			if (battle != nullptr) {
				auto monsters = battle->GetDefenders();
				for (auto child : monsters) {
					child->OnDead(player->MainActor().get());
				}
			}
			auto hook_battle = player->HookBattle();
			if (hook_battle != nullptr) {
				auto monsters = hook_battle->GetDefenders();
				for (auto child : monsters) {
					child->OnDead(player->MainActor().get());
				}
			}
			return;
		}
		if (*command.begin() == "!battle_reward" && command.size() >= 3)
		{
			int32 count = boost::lexical_cast<int32>(command[1]);
			int32 monster_group_id = boost::lexical_cast<int32>(command[2]);
			Packet::NotifyItemList notify;
			for (int32 i = 0; i < count; i++) {
				auto drop_packages = MtMonsterManager::Instance().MonsterGroupRandomDrop(monster_group_id);
				for (auto& package : drop_packages) {
					package->AddToPlayer(player.get());
				}
			}
			if (notify.item_size() > 0) {
				player->SendMessage(notify);
			}
			return;
		}

		if (*command.begin() == "!bosstimes" && command.size() >= 2)
		{
			auto battle = player->HookBattle();
			if (battle != nullptr) {
				int32 count = boost::lexical_cast<int32>(command[1]);
				player->GetPlayerStageProxy()->IncBossChallengeCount(battle->GetStageId(), count);
			}		
			return;
		}
		if (*command.begin() == "!callluascript" && command.size() >= 2)
		{
			int32 scriptid = boost::lexical_cast<int32>(command[1]);
			std::string functionname = command[2];
			try {
				thread_lua->call<void>(scriptid, functionname.c_str());
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}	
			return;
		}
		if (*command.begin() == "!clear_arena_log") {
			auto db_user = MtArenaManager::Instance().FindArenaUser(player->Guid());
			db_user->mutable_log()->clear_logs();
		}
		if (*command.begin() == "!activityfunction") {
			std::string functionname = command[1];
			try {
				thread_lua->call<void>(4, functionname.c_str(), scene);
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}
			return;
		}
		if (*command.begin() == "!OnNpcOption" && command.size() == 4)
		{
			std::string string_param = command[1];
			int64 long_param = (command.size()>=3)?boost::lexical_cast<int64>(command[2]):0;
			int32 int_param = (command.size()>=4) ? boost::lexical_cast<int32>(command[3]):0;
			try {
				thread_lua->call<void>(3003, "OnNpcOption", player, string_param, long_param, int_param);
			}
			catch (ff::lua_exception& e) {
				e;
				return;
			}
			return;
		}
		if (*command.begin() == "!guide_step"&& command.size() == 3)
		{
			int32 group = boost::lexical_cast<int32>(command[1]);
			int32 step = boost::lexical_cast<int32>(command[2]);
			player->SetGuideStep(group,step);
			return;
		}
		if (*command.begin() == "!createguildboss")
		{
			try {
				thread_lua->call<int32>(4, "OnGuildBossBegin", scene);
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}
			return;
			//MtGuildManager::Instance().OnCreateGuildBoss();
		}
		if (*command.begin() == "!next_stage") {
			auto bg = player->HookBattle();
			if (bg != nullptr) {
				bg->PlayerRaidStage();
			}
			return;
		}
		if (*command.begin() == "!guildboss")
		{
			//player->OnGoGuildBossBattle();
			scene->GetBattleManager().CreateFrontBattle(player.get(),nullptr,
			{
				{ "script_id", 1020 },
				{ "group_id",1 }
			});
			return;
		}

		if (command.front() == "!addguildmoney"&& command.size() == 2) {
			int32 money = boost::lexical_cast<int32>(command[1]);
			uint64 guildid = player->GetScenePlayerInfo()->guildid();
			auto guild = MtGuildManager::Instance().GetGuildByid(guildid);
			if (guild != nullptr)
			{ 
				guild->AddGuildMoney(money, player.get());
				Packet::GuildMoneyUpdate update;
				update.set_guildmoney(guild->GetGuildMoney());
				player->SendMessage(update);
			}
			return;
				
		}
		if (command.front() == "!addguilditem"&& command.size() == 2) {
			int32 itemid = boost::lexical_cast<int32>(command[1]);
			auto guild = MtGuildManager::Instance().GetGuildByid(player->GetGuildID());
			if (guild != nullptr)
			{
				guild->AddWarehouse(itemid);
			}
			return;
		}
		if (command.front() == "!addguildspoil"&& command.size() == 3) {
			int32 itemid = boost::lexical_cast<int32>(command[1]);
			int32 copyid = boost::lexical_cast<int32>(command[2]);
			auto guild = MtGuildManager::Instance().GetGuildByid(player->GetGuildID());
			if (guild != nullptr)
			{
				guild->AddSpoilItem(itemid, copyid, player.get());
				guild->SendSpoilItem(player.get(), copyid);
			}
			return;
		}
		if (command.front() == "!guildbonus")
		{
			auto guild = MtGuildManager::Instance().GetGuildByid(player->GetGuildID());
			if (guild != nullptr)
			{
				guild->AddGuildWeekBonus(10000);
				guild->OnWeekBonus();
			}
			return;
		}
		if (*command.begin() == "!fightwetcopy"&& command.size() == 3)
		{
			int32 copyid = boost::lexical_cast<int32>(command[1]);
			int32 bosslevel = boost::lexical_cast<int32>(command[2]);
			//player->OnFightGuildWetCopy(copyid, bosslevel);
			if (scene != nullptr) {
				scene->GetBattleManager().CreateFrontBattle(player.get(),nullptr,
				{
					{ "script_id", 1021 },
					{ "stage_id",bosslevel },
					{ "copyid", copyid }
				});
			}

			return;
		}
		if (*command.begin() == "!worldboss")
		{
			//player->OnGoWorldBossBattle();
			scene->GetBattleManager().CreateFrontBattle(player.get(),nullptr,
			{
				{ "script_id", 1011 },
				{ "group_id",3 }
			});

			return;
		}
		if (*command.begin() == "!durable"&& command.size() == 2)
		{
			auto value = boost::lexical_cast<real32>(command[1]);
			auto actors = player->Actors();
			for (auto& actor : actors) {
				std::vector<MtEquipItem*> equips;
				actor->CurrEquipments(equips);
				for (auto& eqiup : equips) {
					eqiup->DurableCost(value);
				}
				actor->OnBattleInfoChanged();
			}
			return;
		}
		if (*command.begin() == "!fast_mission"&& command.size() == 2)
		{
			int32 type = boost::lexical_cast<int32>(command[1]);
			auto mission_proxy = player->GetMissionProxy();
			
			auto mission_map = mission_proxy->GetMissionMap();
			for (auto child : mission_map) {
				auto config = child.second->Config();
				if (config->type() == type) {
					child.second->MissionDB()->set_state(Packet::MissionState::MissionComplete);
					mission_proxy->OnFinishMission(config->id());
				}
			}
			return;
		}

		if (*command.begin() == "!sysnewday")
		{//系统时间刷新一天，间接newday，内部测试专用！！！！
			time_duration ddd;
			ddd = time_duration(24, 0, 0);
			zxero::add_time_duration(ddd);

			MtTimerManager::Instance().SetTime();
			std::string timestr = MtTimerManager::Instance().NowTimeString();
			player->SendClientNotify(timestr, Packet::SYSTEM,-1);
			return;
		}

		if (*command.begin() == "!newtime"&& command.size() == 4)
		{//系统时间刷新一天，间接newday，内部测试专用！！！！
			int32 hour = boost::lexical_cast<int32>(command[1]);
			int32 minute = boost::lexical_cast<int32>(command[2]);
			int32 second = boost::lexical_cast<int32>(command[3]);
			if (hour < 0 || minute < 0 || second < 0) return;

			time_duration ddd;
			ddd = time_duration(hour, minute, second);
			zxero::add_time_duration(ddd);

			MtTimerManager::Instance().SetTime();
			std::string timestr = MtTimerManager::Instance().NowTimeString();
			player->SendClientNotify(timestr, Packet::SYSTEM, -1);

			//同步服务器时间
			Packet::ServerTime stime;
			stime.set_time(now_second());
			player->Scene()->BroadcastMessage(stime);
			return;
		}

		if (*command.begin() == "!settime"&& command.size() == 4)
		{//系统时间刷新一天，间接newday，内部测试专用！！！！
			int32 hour = boost::lexical_cast<int32>(command[1]);
			int32 minute = boost::lexical_cast<int32>(command[2]);
			int32 second = boost::lexical_cast<int32>(command[3]);
			if (hour < 0 || minute < 0 || second < 0) return;

			//seconds dt(hour, minute, second);
			int32 now_hour = MtTimerManager::Instance().GetHour();
			int32 now_minute = MtTimerManager::Instance().GetMinute();
			int32 now_seconds = MtTimerManager::Instance().GetSecond();
			if (hour < now_hour) {
				return;
			}
			else if (hour == now_hour ) {
				if (minute < now_minute) {
					return;
				}
				else if (minute == now_minute) {
					if (second < now_seconds) {
						return;
					}
				}				
			}

			int32 dthour = hour - now_hour;
			int32 dtminute = minute - now_minute;
			int32 dtsecond = second - now_seconds;

			time_duration ddd = time_duration(dthour,dtminute,dtsecond);
			zxero::add_time_duration(ddd);

			MtTimerManager::Instance().SetTime();
			std::string timestr = MtTimerManager::Instance().NowTimeString();
			player->SendClientNotify(timestr, Packet::SYSTEM, -1);

			//同步服务器时间
			Packet::ServerTime stime;
			stime.set_time(now_second());
			player->SendMessage(stime);
			return;
		}
		if (*command.begin() == "!draw"&& command.size() >= 2){
			int32 draw_type = boost::lexical_cast<int32>(command[1]);
			Packet::ResultCode ret = player->OnDrawCard((Config::DrawType)draw_type);
			player->SendCommonResult(Packet::ResultOption::Player_Draw, ret);
			return;
		}

		if (*command.begin() == "!sku_draw"&& command.size() >= 2) {
			auto container = player->DataCellContainer();
			if (container == nullptr) {
				return;
			}
			int32 times = boost::lexical_cast<int32>(command[1]);
			container->set_data_32(Packet::CellLogicType::AcRewardCell, 3, times, true);
			return;
		}
		if (*command.begin() == "!guildbattlestage"&& command.size() >= 2)
		{
			int32 stage = boost::lexical_cast<int32>(command[1]);
			MtGuildManager::Instance().SetGuildBattleStage(stage);
			return;
		}
		if (*command.begin() == "!guildbattleend"&& command.size() >= 2)
		{
			int32 camp = boost::lexical_cast<int32>(command[1]);
			auto raid = player->GetRaid();
			boost::shared_ptr<GuildBattle> guildbattle = MtGuildManager::Instance().GetGuildBattle(player->GetGuildID());
			if (raid != nullptr && guildbattle != nullptr)
			{
				thread_lua->call<int32>(4, "OnGuildWarEnd", raid, guildbattle, camp);
			}
		}

		if (*command.begin() == "!player_luacall" && command.size() == 2) {
			std::string functionname = command[1];
			try {
				thread_lua->reload(10000);
				thread_lua->call<int32>(10000, functionname.c_str(), player);
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}
			return;
		}
		if (*command.begin() == "!lua_xreg") {
			MtSceneManager::Instance().DispatchMessage(boost::make_shared<G2S::XLuaRegNotify>());

			//
			auto  sernotify = boost::make_shared<S2G::XLuaRegNotify>();
			Server::Instance().SendMessage(sernotify, nullptr);

			return;
		}
		if (*command.begin() == "!lua_test_s") {
			try {
				thread_lua->call<int32>(1023, "testplayers", player);
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}
		}
		if (*command.begin() == "!lua_test") {
			try {
				thread_lua->call<int32>(1023, "testplayer", player);
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}
		}
		if (*command.begin() == "!lua_load")
		{
			auto  notify = boost::make_shared<G2S::LuaLoadNotify>();
			for (uint32 i = 1; i < command.size(); i++) {
				auto id = boost::lexical_cast<int32>(command[i]);
				notify->add_ids(id);
			}
			MtSceneManager::Instance().DispatchMessage(notify);
			//
			auto  sernotify = boost::make_shared<S2G::LuaLoadNotify>();
			for (uint32 i = 1; i < command.size(); i++) {
				auto id = boost::lexical_cast<int32>(command[i]);
				sernotify->add_ids(id);
			}
			Server::Instance().SendMessage(sernotify, nullptr);
			
			return;
		}
		if (*command.begin() == "!test_bianma") {
			std::string str = "编码测试";
			std::string strgb;
			strCoding::UTF_8ToGB2312(strgb, str.c_str(), str.length());
			std::string struf8;
			strCoding::GB2312ToUTF_8(struf8, strgb.c_str(), strgb.length());
			struf8;
		}
		//////x019999_gm_handler
		if (!command.empty()) {
			try {
				thread_lua->reload(19999);
				thread_lua->call<int32>(19999, "gm_handler", player, command);
			}
			catch (ff::lua_exception& e) {				
				player->SendClientNotify(command[0] + "call lua exception="+ e.what(), -999, 1);
				return;
			}
		}
		return;

	}
}