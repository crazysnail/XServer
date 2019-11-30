#include <SceneCommon.pb.h>
#include <Guild.pb.h>
#include <GuildConfig.pb.h>
#include <DataCellPacket.pb.h>
#include <MonsterConfig.pb.h>
#include <BaseConfig.pb.h>
#include <SceneObject.pb.h>
#include <ActorConfig.pb.h>
#include <G2SMessage.pb.h>
#include <S2GMessage.pb.h>
#include <EnterScene.pb.h>
#include <ActivityConfig.pb.h>
#include "../base/client_session.h"
#include "../base/mt_timer.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_server/mt_server.h"
#include "../data/data_manager.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mail/static_message_manager.h"
#include "../base/mt_timer.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_item/mt_item.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_chat/mt_word_manager.h"
#include "../mt_activity/mt_activity.h"
#include "../base/mt_db_load_work.h"
#include "utils.h"
#include "mt_guild_manager.h"
#include "module.h"
#include "active_model.h"


namespace Mt
{
	static MtGuildManager* __mt_guild_manager = nullptr;
	REGISTER_MODULE(MtGuildManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtGuildManager::OnLoad, boost::ref(MtGuildManager::Instance())));
		Packet::GuildSpoilItem spoilinfo;
		WetCopySpoilApplySize = spoilinfo.GetDescriptor()->FindFieldByName("playerid")->options().GetExtension(Packet::column_count);
		Packet::GuildWetCopyInfo wetcopyinfo;
		WetCopyMonsterSzie = wetcopyinfo.GetDescriptor()->FindFieldByName("bosshp")->options().GetExtension(Packet::column_count);
	}

	void MtGuildManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtGuildManager, ctor()>(ls, "MtGuildManager")
			.def(&MtGuildManager::CreateGuild, "CreateGuild")
			.def(&MtGuildManager::OnWeekTriger, "OnWeekTriger")
			.def(&MtGuildManager::OnStartGuildBoss, "OnStartGuildBoss")
			.def(&MtGuildManager::OnEndGuildBoss, "OnEndGuildBoss")
			.def(&MtGuildManager::GetGuildWetCopyStage, "GetGuildWetCopyStage")
			.def(&MtGuildManager::InGuildWetCopyScene, "InGuildWetCopyScene")
			.def(&MtGuildManager::GetGuildDonateConfig, "GetGuildDonateConfig")
			.def(&MtGuildManager::SendGuildWetCopy,"SendGuildWetCopy")
			.def(&MtGuildManager::GetGuildByid, "GetGuildByid")
			.def(&MtGuildManager::OnGetGuildInfo, "OnGetGuildInfo")
			.def(&MtGuildManager::GetGuildUser, "GetGuildUser")
			.def(&MtGuildManager::GetGuildBattle,"GetGuildBattle")
			.def(&MtGuildManager::GetPlayerGuildPosition,"GetPlayerGuildPosition")
			.def(&MtGuildManager::SendGuildUserPosition,"SendGuildUserPosition")
			.def(&MtGuildManager::GenGuildBattleList,"GenGuildBattleList")
			.def(&MtGuildManager::SetLastDate, "SetLastDate")
			.def(&MtGuildManager::GetLastDate, "GetLastDate")
			.def(&MtGuildManager::GetGuildBattleStage, "GetGuildBattleStage")
			.def(&MtGuildManager::SetGuildBattleStage, "SetGuildBattleStage")
			.def(&MtGuildManager::BroadcastGuild, "BroadcastGuild")
			.def(&MtGuildManager::OnGetGuildBossUserDamage, "OnGetGuildBossUserDamage")
			.def(&MtGuildManager::OnGetGuildCopyDamage, "OnGetGuildCopyDamage")
			.def(&MtGuildManager::OnGetGuildCopyServerDamage, "OnGetGuildCopyServerDamage")
			.def(&MtGuildManager::GetGuildWetCopyConfig, "GetGuildWetCopyConfig")
			.def(&MtGuildManager::SetGuildWetCopyStageMonster,"SetGuildWetCopyStageMonster")
			.def(&MtGuildManager::EndGuildWetCopy,"EndGuildWetCopy")
			.def(&MtGuildManager::GetGuildLevelFixConfig,"GetGuildLevelFixConfig")
			.def(&MtGuildManager::ClearGuildBattleScore,"ClearGuildBattleScore")
			.def(&MtGuildManager::ClearGuildBattleRoundScore, "ClearGuildBattleRoundScore")
			.def(&MtGuildManager::GuildBattleRoundReward, "GuildBattleRoundReward")
			.def(&MtGuildManager::SetGuildBattleWeekRank,"SetGuildBattleWeekRank")
			.def(&MtGuildManager::SetGuildBattleAllRank, "SetGuildBattleAllRank")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtGuildManager::Instance, "LuaMtGuildManager");
	}

	MtGuildManager& MtGuildManager::Instance()
	{
		return *__mt_guild_manager;
	}

	MtGuildManager::MtGuildManager()
	{
		__mt_guild_manager = this;
	}

	MtGuildManager::~MtGuildManager()
	{

	}
	uint64 MtGuildManager::GenerateGuid()
	{
		guid_serial_++;
		return guid_serial_;
	}
	void MtGuildManager::SetSerial(uint64 max_serial)
	{
		guid_serial_ = max_serial;
	}

	//void guilduserdamage::Filldata(Packet::GuildUserDamage &data)
	//{
	//	data.set_playerid_(playerid_);
	//	data.set_playername_(playername_);
	//	data.set_guildname_(guildname_);
	//	data.set_playerhair_(playerhair_);
	//	data.set_damagevalue_(damagevalue_);
	//	for (int32 i = 0; i < 5; ++i)
	//	{
	//		data.add_actor_config_id_(actor_config_id_[i]);
	//	}
	//}
	int32	MtGuildManager::OnLoad()
	{
		auto table = data_manager::instance()->get_table("guild_buildlevel");
		ZXERO_ASSERT(table) << "can not find table guild_buildlevel";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_buildlevel = boost::make_shared <Config::GuildBuildLevelConfig>();
			FillMessageByRow(*guild_buildlevel, *row);
			guildbuild_.push_back(guild_buildlevel);
		}

		table = data_manager::instance()->get_table("guildlevel_fix");
		ZXERO_ASSERT(table) << "can not find table guildlevel_fix";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guildlevelfix = boost::make_shared <Config::GuildLevelFixConfig>();
			FillMessageByRow(*guildlevelfix, *row);
			guildlevelfix_.push_back(guildlevelfix);
		}

		table = data_manager::instance()->get_table("guild_practice");
		ZXERO_ASSERT(table) << "can not find table guild_practice";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_practice = boost::make_shared <Config::GuildPracticeConfig>();
			FillMessageByRow(*guild_practice, *row);
			guildpractice_.insert({ guild_practice->id(), guild_practice });
		}
		table = data_manager::instance()->get_table("guild_practicelevel");
		ZXERO_ASSERT(table) << "can not find table guild_practicelevel";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_practicelevel = boost::make_shared <Config::GuildPracticeLevelConfig>();
			FillMessageByRow(*guild_practicelevel, *row);
			guildpracticelevel_.insert({ guild_practicelevel->level(), guild_practicelevel });
		}
		table = data_manager::instance()->get_table("guild_cdk");
		ZXERO_ASSERT(table) << "can not find table guild_cdk";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_cdk = boost::make_shared <Config::GuildCDKConfig>();
			FillMessageByRow(*guild_cdk, *row);
			guildcdk_.insert({ guild_cdk->id(), guild_cdk });
		}
		table = data_manager::instance()->get_table("guild_wetcopy");
		ZXERO_ASSERT(table) << "can not find table guild_wetcopy";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_wet = boost::make_shared <Config::GuildWetCopyConfig>();
			FillMessageByRow(*guild_wet, *row);
			guildwetcopy_.insert({ guild_wet->id(), guild_wet });
		}
		
		table = data_manager::instance()->get_table("guild_wetcopy_stage");
		ZXERO_ASSERT(table) << "can not find table guild_wetcopy_stage";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_wet_stage = boost::make_shared <Config::GuildWetCopyStageConfig>();
			FillMessageByRow(*guild_wet_stage, *row);
			guildwetcopystage_.push_back( guild_wet_stage );
		}

		table = data_manager::instance()->get_table("guild_position");
		ZXERO_ASSERT(table) << "can not find table guild_position";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_position = boost::make_shared <Config::GuildPositionConfig>();
			FillMessageByRow(*guild_position, *row);
			guildposition_.push_back(guild_position);
		}

		table = data_manager::instance()->get_table("guild_donate");
		ZXERO_ASSERT(table) << "can not find table guild_donate";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto guild_donate = boost::make_shared <Config::GuildDonateConfig>();
			FillMessageByRow(*guild_donate, *row);
			guilddonate_.push_back(guild_donate);
		}
		return 0;
	}
	
	void MtGuildManager::OnSecondTick(uint32 elapseTime)
	{
		for (auto iter = guildlist_.begin(); iter != guildlist_.end();){
			(*iter)->OnSecondTick((int32)elapseTime);
			if ((*iter)->GetGuildUserSize() == 0 || (*iter)->tickdelete == true) {
				LOG_INFO << "TickDeleteGuild guildid="<< (*iter)->GetGuildId()<<" UserSize="<< (*iter)->GetGuildUserSize()<<" tickdelete=" << (*iter)->GetGuildUserSize();
				for (auto useriter = (*iter)->guilduserlist.begin(); useriter != (*iter)->guilduserlist.end(); useriter++)	{
					LOG_INFO << "TickDeleteGuild guildid=" << (*iter)->GetGuildId() << " playerid=" << (*useriter)->playerguid();
					//标记和删除
					(*useriter)->SetDeleted();
					auto player_ = Server::Instance().FindPlayer((*useriter)->playerguid());
					if (player_ != nullptr)	{
						player_->Cache2Save((*useriter).get());
						Server::Instance().SendG2SCommonMessage(player_.get(),"G2SUpdateGuildInfo", {}, { (int64)INVALID_GUID }, { "","TickDeleteGuild" });
					}
				}
				(*iter)->guilduserlist.clear();
				(*iter)->guilddb->SetDeleted();
				Cache2Save((*iter)->guilddb.get(),"GuildInfo");
				iter = guildlist_.erase(iter);
			}
			else {
				iter++;
			}
		}
		
	}

	void MtGuildManager::OnHourTriger(int32 hour)
	{
		for (auto guild : guildlist_)
		{
			guild->OnHourTriger(hour);
		}
	}
	void MtGuildManager::OnWeekTriger(int32 cur_day)
	{
		cur_day;
		for (auto guild : guildlist_)
		{
			guild->OnWeekBonus();
		}
	}
	void MtGuildManager::OnDayTriger(int32 day)
	{
		for (auto guild : guildlist_)
		{
			guild->OnDayTriger(day);
		}
		serverranklist_.cleardata();
	}
	void  MtGuildManager::OnMinTriger(int32 min)
	{
		try {
			thread_lua->call<void>(7, "OnMinTriger", this, min);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "MtGuildManager lua exception:" << e.what();
			return;
		}
	}
	void MtGuildManager::OnSecondTriger(int32 second)
	{
		second;
	}
	void MtGuildManager::OnWeekBonus()
	{
		
	}
	void MtGuildManager::OnMainTain()
	{

	}


	bool MtGuildManager::OnLoadAll(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto dbguildlist = ActiveModel::Base<Packet::GuildDBInfo>(dbctx).FindAll({ });
		for (auto& db_guild : dbguildlist) {
			boost::shared_ptr<GuildInfo> guild = boost::make_shared<GuildInfo>();
			if (guild != nullptr)
			{
				guild->guilddb = db_guild;
				if (guild->guilddb->createname() == "")
				{
					guild->guilddb->set_createname(guild->guilddb->chieftainname());
				}
				if (guild->guilddb->createguid() == INVALID_GUID)
				{
					guild->guilddb->set_createguid(guild->guilddb->chieftain());
				}
				auto dbguilduserlist = ActiveModel::Base<Packet::GuildUser>(dbctx).FindAll({ { "guildid", db_guild->guid() } });
				auto dbguildwetcopy = ActiveModel::Base<Packet::GuildWetCopyInfo>(dbctx).FindAll({ { "guildid", db_guild->guid() } });
				auto dbguildwarehouse = ActiveModel::Base<Packet::GuildWarehouse>(dbctx).FindAll({ { "guildid", db_guild->guid() } });
				auto dbguildwarehouserecord = ActiveModel::Base<Packet::GuildWarehouseRecord>(dbctx).FindAll({ { "guildid", db_guild->guid() } });
				auto dbguildspoilitem = ActiveModel::Base<Packet::GuildSpoilItem>(dbctx).FindAll({ { "guildid", db_guild->guid() } });
				auto userdamage = ActiveModel::Base<Packet::GuildUserDamage>(dbctx).FindAll({ { "guildid_", db_guild->guid() } });
				auto records = ActiveModel::Base<Packet::GuildRecord>(dbctx).FindAll({ { "guildid", db_guild->guid() } });
				guild->guilduserlist = dbguilduserlist;
				guild->wetcopyinfo_ = dbguildwetcopy;
				guild->warehouse_ = dbguildwarehouse;
				for (auto wh : guild->warehouse_)
				{
					guild->warehouseindex_ = (wh->index() > guild->warehouseindex_) ? wh->index(): guild->warehouseindex_;
				}
				guild->warehouse_record_ = dbguildwarehouserecord;
				guild->guildspoil_item_ = dbguildspoilitem;
				for (auto sp : guild->guildspoil_item_)
				{
					guild->guildspoilindex_ = (sp->index() > guild->guildspoilindex_) ? sp->index() : guild->guildspoilindex_;
				}
				
				guild->guildrecord_ = records;
				for (auto damage : userdamage)
				{
					guilduserdamage gudamage;
					gudamage.data->CopyFrom(*damage.get());
					guild->OnGuildWetCopyDamage(damage->paramid_(), gudamage);
					serverwetcopyranklist_.nosortinsert(gudamage);
				}
				guildlist_.push_back(guild);
				for (auto copyinfo : guild->wetcopyinfo_)
				{
					copyinfo->set_fighterid(INVALID_GUID);
					copyinfo->set_fightname("");
				}
				if (guild->guilddb->battlescore() > 0 || guild->guilddb->battletimes() > 0)
				{
					SetGuildBattleWeekRank(guild->guilddb->guid(), guild->guilddb->name(), guild->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN), guild->guilddb->battlescore());
					SetGuildBattleAllRank(guild->guilddb->guid(), guild->guilddb->name(), guild->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN), guild->guilddb->battlewintimes(), guild->guilddb->battletimes());
				}
			}
		}
		auto dbleaveuser = ActiveModel::Base<Packet::GuildUser>(dbctx).FindAll({ { "guildid", INVALID_GUID } });
		leaveuserlist = dbleaveuser;
		auto dbbattlelist = ActiveModel::Base<Packet::GuildBattleInfo>(dbctx).FindAll({});
		for (auto& db_battle : dbbattlelist) {
			boost::shared_ptr<GuildBattle> gb = boost::make_shared<GuildBattle>();
			if (gb)
			{
				gb->GetBattleInfo()->CopyFrom(*db_battle.get());
				guildbattlepairlist_.push_back(gb);
			}
		}
		return true;
	}
	void MtGuildManager::OnSaveAll(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		for (auto guild : guildlist_)
		{
			guild->OnSaveAll(dbctx);
		}
		for (auto user : leaveuserlist)
		{
			ActiveModel::Base<Packet::GuildUser>(dbctx).Save(user);
		}
	}

	void MtGuildManager::OnCacheAll( )
	{
		for (auto guild : guildlist_)
		{
			guild->OnCacheAll( );
		}
		{
			boost::shared_lock<boost::shared_mutex> l(guildbattle_shared_mutex_);
			for (auto guildbattle : guildbattlepairlist_)
			{
				guildbattle->OnCacheAll();
			}
		}

		std::set<google::protobuf::Message *> msg_set;
		for (auto user : leaveuserlist)
		{
			msg_set.insert(user.get());
		}

		MtShmManager::Instance().Cache2Shm(msg_set, "GuildLeaveUser");
	}

	void MtGuildManager::Cache2Save(google::protobuf::Message * msg, std::string MessageName)
	{
		if (msg == nullptr)
			return;
		std::set<google::protobuf::Message *> msg_set;
		msg_set.insert(msg);
		MtShmManager::Instance().Cache2Shm(msg_set, MessageName);
	}

	bool GuildInfo::OnSaveAll(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		ActiveModel::Base<Packet::GuildDBInfo>(dbctx).Save(guilddb);
		for (auto user : guilduserlist)
		{
			ActiveModel::Base<Packet::GuildUser>(dbctx).Save(user);
		}
		for (auto copy : wetcopyinfo_)
		{
			ActiveModel::Base<Packet::GuildWetCopyInfo>(dbctx).Save(copy);
		}
		for (auto wh : warehouse_)
		{
			ActiveModel::Base<Packet::GuildWarehouse>(dbctx).Save(wh);
		}
		for (auto whr : warehouse_record_)
		{
			ActiveModel::Base<Packet::GuildWarehouseRecord>(dbctx).Save(whr);
		}
		for (auto spoil : guildspoil_item_)
		{
			ActiveModel::Base<Packet::GuildSpoilItem>(dbctx).Save(spoil);
		}
		for (auto record : guildrecord_)
		{
			ActiveModel::Base<Packet::GuildRecord>(dbctx).Save(record);
		}
		for (auto wetcopydamage : guildwetcopyranklist_)
		{
			for (auto damage : wetcopydamage.second.m_data)
			{
				ActiveModel::Base<Packet::GuildUserDamage>(dbctx).Save(*damage.data.get());
			}
		}
		return true;
	}

	bool GuildInfo::OnCacheAll( )
	{
		std::set<google::protobuf::Message *> msg_set;
		msg_set.insert(guilddb.get());
		//
		for (auto user : guilduserlist) {
			msg_set.insert(user.get());
		}
		//
		for (auto copy : wetcopyinfo_) {
			msg_set.insert(copy.get());
		}
		for (auto ware : warehouse_)
		{
			msg_set.insert(ware.get());
		}
		for (auto whr : warehouse_record_)
		{
			msg_set.insert(whr.get());
		}
		for (auto spoil : guildspoil_item_)
		{
			msg_set.insert(spoil.get());
		}
		for (auto record : guildrecord_)
		{
			msg_set.insert(record.get());
		}
		for (auto& wetcopydamage : guildwetcopyranklist_)
		{
			for (auto& damage : wetcopydamage.second.m_data)
			{
				msg_set.insert(damage.data.get());
			}
		}
		MtShmManager::Instance().Cache2Shm(msg_set,"GuildInfo");

		return true;		
	}

	bool MtGuildManager::OnCreateGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreateGuildReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}

		std::string createname = req->name();
		std::string createnotice = req->notice();
		if (!MtWordManager::Instance()->NoSensitiveWord(createname))
		{
			player_->SendClientNotify("neirongfeifa", -1, -1);
			return true;
		}
		if (!MtWordManager::Instance()->NoSensitiveWord(createnotice))
		{
			player_->SendClientNotify("neirongfeifa", -1, -1);
			return true;
		}
		auto guild = MtGuildManager::Instance().GetGuildByName(createname);
		if (guild != nullptr)
		{
			player_->SendCommonResult(Packet::ResultOption::Guild_Opt, Packet::ResultCode::Guild_DuplicateName);
			return true;
		}
		Server::Instance().SendG2SCommonMessage(player_.get(), "G2SCreateGuildReq", {}, {}, { createname ,createnotice });
		return true;
	}

	void MtGuildManager::CreateGuild(MtPlayer* player_, std::string createname, std::string createnotice)
	{
		if (player_ == nullptr) {
			return ;
		}

		//功能解锁校验
		if (player_->OnLuaFunCall_n(100, "CheckFunction", { { "gonghui", 999 } }) != Packet::ResultCode::ResultOK) {
			return ;
		}
		boost::shared_ptr<GuildInfo> guid_ = boost::make_shared<GuildInfo>();
		if (guid_ == nullptr)
			return ;

		boost::shared_ptr<Packet::GuildUser> guilduser = boost::make_shared<Packet::GuildUser>();
		if (guilduser == nullptr)
			return ;
		guid_->guilddb = boost::make_shared<Packet::GuildDBInfo>();
		if (guid_->guilddb == nullptr)
			return ;
		uint64 guildid = GenerateGuid();
		if (MAX_GUILD_ID <= guildid)
		{
			ZXERO_ASSERT(false)<<"MAX GUILD ID!!";
			return;
		}
		guid_->InitGuildUser(player_, guilduser, Packet::GuildPosition::CHIEFTAIN, guildid);
		guid_->UpdateGuildUser(player_, guilduser);
		guid_->guilddb->set_guid(guildid);
		guid_->guilddb->set_chieftain(player_->Guid());
		guid_->guilddb->set_chieftainname(player_->Name());
		guid_->guilddb->set_name(createname);
		guid_->guilddb->set_icon("item_gonghuitouxiang+equip_20_fagong_shipin");
		guid_->guilddb->set_lastname("");
		guid_->guilddb->set_notice(createnotice);
		guid_->guilddb->set_guildlevel(1);
		guid_->guilddb->set_cashboxlevel(0);
		guid_->guilddb->set_shoplevel(0);
		guid_->guilddb->set_storagelevel(0);
		guid_->guilddb->set_guildcd(0);
		guid_->guilddb->set_cashboxcd(0);
		guid_->guilddb->set_shopcd(0);
		guid_->guilddb->set_storagecd(0);

		guid_->guilddb->set_limitapplylevel(1);
		guid_->guilddb->set_maxwetcopyid(0);
		guid_->guilddb->set_weekbonus(0);
		guid_->guilddb->set_lowmaintainday(0);
		guid_->guilddb->set_battlewintimes(0);
		guid_->guilddb->set_battletimes(0);
		guid_->guilddb->set_battlelefttimes(0);
		guid_->guilddb->set_battleroundwintimes(0);
		guid_->guilddb->set_battleroundtimes(0);
		guid_->guilddb->set_battlescore(0);
		int32 initguildmoney = guid_->GetGuildMaintain() * 3;
		guid_->guilddb->set_guildmoney(initguildmoney);
		guid_->guilddb->set_createtime(MtTimerManager::Instance().CurrentDate());
		guid_->guilddb->set_createguid(player_->Guid());
		guid_->guilddb->set_createname(player_->Name());

		guildlist_.push_back(guid_);


		guid_->guilduserlist.push_back(guilduser);

		Server::Instance().SendG2SCommonMessage(player_, "G2SUpdateGuildInfo", {}, { (int64)guildid }, { createname,"CreateGuild" });
		Server::Instance().SendG2SCommonMessage(player_, "G2SJoinGuildOk", {}, {}, {});
		guid_->G2SSyncGuildUserData(player_, guilduser.get());

		Packet::GetGuildInfoReply reply;
		reply.mutable_info()->CopyFrom(*guid_->guilddb.get());
		reply.set_membersize(1);
		reply.set_maxmembersize(guid_->GetGuildUserMaxSize());
		reply.mutable_selfinfo()->CopyFrom(*guilduser.get());
		player_->SendMessage(reply);
		std::string str = "Guild_CreateXX|" + player_->Name();
		guid_->AddRecord(str);

		LOG_INFO << "OnCreateGuildReq player=" << player_->Guid() << " guid=" << guildid << " name=" << createname;
	}
	bool MtGuildManager::OnSetGuildNameRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetGuildNameRR>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		if (GetPlayerGuildPosition(player_->Guid(), guild.get()) != Packet::GuildPosition::CHIEFTAIN)
		{
			return true;
		}
		std::string name = req->name();
		if (!MtWordManager::Instance()->NoSensitiveWord(name))
		{
			player_->SendClientNotify("neirongfeifa", -1, -1);
			return true;
		}
		guild->SetGuildName(name, player_.get());
		
		return true;
	}
	bool MtGuildManager::OnSetGuildNoticeRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetGuildNoticeRR>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		if (GetPlayerGuildPosition(player_->Guid(), guild.get()) != Packet::GuildPosition::CHIEFTAIN)
		{
			return true;
		}
		std::string notice = req->notice();
		if (!MtWordManager::Instance()->NoSensitiveWord(notice))
		{
			player_->SendClientNotify("neirongfeifa", -1, -1);
			return true;
		}
		guild->guilddb->set_notice(notice);
		Packet::SetGuildNoticeRR reply;
		reply.set_notice(notice);
		player_->SendMessage(reply);
		return true;
	}
	bool MtGuildManager::OnSetGuildIconRR(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetGuildIconRR>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		if (GetPlayerGuildPosition(player_->Guid(), guild.get()) != Packet::GuildPosition::CHIEFTAIN)
		{
			return true;
		}
		std::string icon = req->icon();
		guild->guilddb->set_icon(icon);
		Packet::SetGuildIconRR reply;
		reply.set_icon(icon);
		player_->SendMessage(reply);
		return true;
	}
	
	void MtGuildManager::SendGuildUserPosition(MtPlayer* player_, uint64 targetguid,int32 position)
	{
		if (player_)
		{
			Packet::SetGuildUserPositionRR rr;
			rr.set_playerguid(targetguid);
			rr.set_position((Packet::GuildPosition)position);
			player_->SendMessage(rr);
		}
		return ;
	}
	bool MtGuildManager::OnGetGuildBonusReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildBonusReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		auto guilduser = guild->GetGuildUser(player_->Guid());
		if (guilduser == nullptr)
			return true;
		int32 bonus = guilduser->bbonus();
		if (bonus <= 0)
			return true;
		guilduser->set_bbonus(0);
		player_->SendCommonReply("UpdateGuildBonus", { 0 }, {}, {});
		Server::Instance().SendG2SCommonMessage(player_.get(), "GuildBonus", { bonus }, {}, {});
		return true;
	}

	bool MtGuildManager::OnCreateGuildCDKReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreateGuildCDKReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
 		if (player_ == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		
		for (auto cdk : req->cdks())
		{
			int32 configid = cdk.configid();
			int32 addcount = cdk.count();
			//判断一下表格
			auto cdkconfig = GetGuildCDKConfig(configid);
			if (cdkconfig == nullptr)
				continue;
			int32 cost = cdkconfig->cost()*addcount;				
			if (!player_->DelItemById(Packet::TokenType::Token_Crystal,cost, Config::ItemDelLogType::DelType_Guild)){
				player_->SendCommonResult(Packet::ResultOption::Guild_Opt, Packet::ResultCode::CrystalNotEnough);
				continue;
			}
			//判断一下是否有钱
			//扣钱
			//有没有之前的
			uint64 playerguid = player_->Guid();

			auto havecdkinfo = guild->GetGuildCDKInfoByPlayer(playerguid, configid);
			if (havecdkinfo != nullptr)
			{
				guild->AddCDKCount(player_, havecdkinfo, addcount);
			}
			else
			{
				guild->newcdkguid++;
				boost::shared_ptr<Packet::GuildCDKInfo> cdkinfo = boost::make_shared<Packet::GuildCDKInfo>();
				if (cdkinfo != nullptr)
				{
					cdkinfo->set_guid(guild->newcdkguid);
					cdkinfo->set_playerid(playerguid);
					cdkinfo->set_configid(configid);
					guild->AddCDKCount(player_, cdkinfo, addcount);
					cdkinfo->set_playername(player_->GetScenePlayerInfo()->name());
					cdkinfo->set_limitday(cdk.limitday());
					cdkinfo->set_limitlevel(cdk.limitlevel());
					cdkinfo->set_limitdonation(cdk.limitdonation());
					cdkinfo->set_datetime(MtTimerManager::Instance().GetDayTime());
					cdkinfo->set_wish(cdk.wish());
					guild->cdklist.push_back(cdkinfo);
				}
			}
		}
		
		guild->SendCDKList(player_);
		return true;
	}
	bool MtGuildManager::OnGetGuildCDKReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildCDKReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		uint64 cdkguid = req->guid();
		auto cdkinfo=guild->GetGuildCDKInfoById(cdkguid);
		if (cdkinfo == nullptr)
		{
			player_->SendClientNotify("Guild_CDKNotEnough", -1, -1);
			return true;
		}
		if (guild->HaveGetCDKInfo(cdkinfo, player_->Guid()))
		{
			player_->SendClientNotify("Guild_HaveCDK", -1, -1);
			return true;
		}

		int32 count = cdkinfo->count();
		if (count <= 0)
		{
			player_->SendClientNotify("Guild_CDKNotEnough", -1, -1);
			return true;
		}
	
		count--;
		cdkinfo->set_count(count);
		player_->AddItemById(cdkinfo->configid(), 1, Config::ItemAddLogType::AddType_BankStore);
		cdkinfo->add_getplayerid(player_->Guid());
		if (count == 0)
		{
			guild->DeleteGuildCDKInfoById(cdkguid);
		}
		guild->SendCDKList(player_);
		return true;
	}
	bool MtGuildManager::OnGetGuildCDKListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildCDKListReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		guild->SendCDKList(player_);
		return true;
	}

	bool MtGuildManager::OnAddGuildMoney(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::AddGuildMoney>& msg, int32 /*source*/)
	{
		auto add_count = msg->addcount();
		auto guildid = msg->guildid();
		auto guild = GetGuildByid(guildid);
		if (guild)
		{
			guild->AddGuildMoney(add_count, player.get());
			uint64 playerid = INVALID_GUID;
			if (player)
			{
				playerid = player->Guid();
				Packet::GuildMoneyUpdate update;
				update.set_guildmoney(guild->GetGuildMoney());
				player->SendMessage(update);
			}
			LOG_INFO << "S2GADDGUILDMOENY," << guildid << "," << add_count << "," << playerid;
		}
		return true;
	}
	bool MtGuildManager::OnBuildLevelUpReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::BuildLevelUpReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;
		auto const_union_levelup_cd = MtConfigDataManager::Instance().FindConfigValue("union_levelup_cd");
		if (const_union_levelup_cd == nullptr) 
			return true;
		
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		Packet::GuildPosition position = (Packet::GuildPosition)GetPlayerGuildPosition(player_->Guid(), guild.get());
		if (position != Packet::GuildPosition::CHIEFTAIN)
		{
			player_->SendClientNotify("Guild_Notchieftain", -1, -1);
			return true;
		}
			
		int32 buildtype = req->buildtype();
		int32 buildlevel = guild->GetBuildLevel(buildtype);
		if (buildlevel < 0 || buildlevel >= 5)
			return true;
		int32 cdtime = guild->GetBuildLevelCDTime(buildtype);
		if (cdtime !=0 )
		{
			uint32 diffsec= MtTimerManager::Instance().DiffTimeToNow(cdtime);
			if (diffsec < (uint32)const_union_levelup_cd->value1())
			{
				player_->SendClientNotify("Guild_LevelUpCD", -1, -1);
				return true;
			}
		}
		auto buildlevelconfig  = MtGuildManager::Instance().GetBuildLevelConfig(buildtype, buildlevel);
		if (buildlevelconfig == nullptr)
			return true;
		int32 cost = buildlevelconfig->cost();
		int32 curguildmoney = guild->guilddb->guildmoney();
		int32 needguildmoney = cost + 2 * guild->GetGuildMaintain();
		if (curguildmoney < needguildmoney)
		{
			player_->SendClientNotify("Guild_MoneyNotEnough", -1, -1);
			return true;
		}
			
		if (buildtype == 0)//帮会等级
		{
			int32 cashboxlevel= guild->GetBuildLevel(1);
			int32 shoplevel = guild->GetBuildLevel(2);
			int32 storagelevel = guild->GetBuildLevel(3);
			int32 alllevel = cashboxlevel + shoplevel + storagelevel;
			if (alllevel < buildlevelconfig->param(1))
			{
				player_->SendClientNotify("Guild_BuildLevelNotEnough", -1, -1);
				return true;
			}
		}
		else
		{
			int32 guildlevel = guild->GetBuildLevel(0);
			if (guildlevel <= buildlevel)
			{
				player_->SendClientNotify("Guild_BuildLevelMore", -1, -1);
				return true;
			}
		}
		guild->AddGuildMoney(-cost, player_.get());
		buildlevel++;
		uint32 cddtime = MtTimerManager::Instance().CurrentDate();
		guild->SettBuildLevel(buildtype, buildlevel, cddtime);
		
		std::string notify = (boost::format("gonghuishengjixx|gonghuibuild_%1%|%2%") % buildtype % buildlevel).str();
		send_guild_system_message(notify, guildid);
		guild->AddRecord(notify);
		Packet::GuildMoneyUpdate update;
		update.set_guildmoney(guild->GetGuildMoney());
		player_->SendMessage(update);

		Packet::BuildLevelUpReply reply;
		reply.set_buildtype(buildtype);
		reply.set_buildlevel(buildlevel);
		reply.set_cdtime(cddtime);
		player_->SendMessage(reply);

		return true;
	}

	bool MtGuildManager::OnPlayerPracticeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerPracticeReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr) {
			return true;
		}

		//功能解锁校验
		//if (player_->OnLuaFunCall_n(100, "CheckFunction", { { "gonghuixiulian", 999 } }) != Packet::ResultCode::ResultOK) {
		//	return true;
		//}

		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		auto guilduser = guild->GetGuildUser(player_->Guid());
		if (guilduser == nullptr)
			return true;
		
		Server::Instance().SendG2SCommonMessage(player_.get(), "G2SAddPracticeExp", { req->id() , req->type(),guild->guilddb->guildlevel(),guilduser->allcontribution() }, {}, {});
		//int32 type = req->type();
		//int32 practiceid = req->id();
		//player_->AddPracticeExp(practiceid, type, guild->guilddb->guildlevel(), guilduser->allcontribution());
		
		return true;
	}
	bool MtGuildManager::OnGetPracticeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetPracticeReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		player->OnGetPractice();
		return true;
	}
	bool MtGuildManager::OnGetGuildWetCopyInfosReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildWetCopyInfosReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}

		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		auto playeruser = guild->GetGuildUser(player_->Guid());
		if (playeruser == nullptr)
			return true;
		Packet::GetGuildWetCopyInfosReply reply;
		for (auto wetcopy_ : guild->wetcopyinfo_)
		{
			auto wetcopy = reply.add_wetcopys_();
			wetcopy->CopyFrom(*wetcopy_.get());
		}
		reply.set_times(playeruser->wetcopytimes());
		player_->SendMessage(reply);
		return true;
	}
	bool MtGuildManager::SetGuildWetCopyStageMonster(Packet::GuildWetCopyInfo* wetcopy,Config::GuildWetCopyStageConfig* stage)
	{
		if (stage == nullptr)
			return false;
		auto monstergroup = MtMonsterManager::Instance().FindMonsterGroup(stage->monster_id());
		if (monstergroup == nullptr)
			return false;
		int32 index = 0;
		uint64 maxmonsterhp = 0;
		for (auto monsterdef : monstergroup->monsters())
		{
			auto monsterid = monsterdef.id();
			if (monsterid > 0)
			{
				auto monster = MtMonsterManager::Instance().FindMonster(monsterid);
				if (monster)
				{
					auto monsterhp = monster->battle_info().hp();
					wetcopy->set_bosshp(index,monsterhp);
					maxmonsterhp += monsterhp;
					index++;
				}
			}
		}
		if (maxmonsterhp <= 0)
			return false;
		for (int32 i= index;i<wetcopy->bosshp_size();i++)
		{
			wetcopy->set_bosshp(i,0);
		}
		wetcopy->set_maxbosshp(maxmonsterhp);
		wetcopy->set_stageid(stage->id());
		return true;
	}
	bool MtGuildManager::InitGuildWetCopyMonster(boost::shared_ptr<Packet::GuildWetCopyInfo> wetcopy)
	{
		if (wetcopy == nullptr)
			return false;
		int32 copyid = wetcopy->copyid();
		auto stage = GetGuildWetCopyFristStage(copyid);
		if (stage == nullptr)
			return false;
		return SetGuildWetCopyStageMonster(wetcopy.get(),stage.get());
	}
	bool MtGuildManager::OnOpenGuildWetCopyReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::OpenGuildWetCopy>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		Packet::GuildPosition position = (Packet::GuildPosition)GetPlayerGuildPosition(player_->Guid(), guild.get());
		if (position != Packet::GuildPosition::CHIEFTAIN && position != Packet::GuildPosition::VICE_CHIEFTAIN)
		{
			player_->SendClientNotify("Guild_NotchieftainAndVICE", -1, -1);
			return true;
		}
			
		int32 copyid = message->copyid();
		if (copyid > (guild->guilddb->maxwetcopyid()+1))
			return true;

		auto guildwetcopy = guild->GetGuildWetCopyInfo(copyid);
		if (guildwetcopy != nullptr)
			return true;
		auto wetcopyconfig = GetGuildWetCopyConfig(copyid);
		if (wetcopyconfig == nullptr)
			return true;
		if (guild->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN) < wetcopyconfig->guildlevellimit())
		{
			player_->SendClientNotify("openwetcopyguildlevel|"+ boost::lexical_cast<std::string>(wetcopyconfig->guildlevellimit()), -1, -1);
			return true;
		}
		//cost
		int32 coincost = wetcopyconfig->coincost();
		if (guild->GetGuildMoney() < coincost)
		{
			player_->SendClientNotify("Guild_MoneyNotEnough", -1, -1);
			return true;
		}
		guild->AddGuildMoney(-coincost, player_.get());
		
		boost::shared_ptr<Packet::GuildWetCopyInfo> wetcopy = boost::make_shared<Packet::GuildWetCopyInfo>();
		if (wetcopy == nullptr)
			return true;
		for (int32 i = 0; i < WetCopyMonsterSzie; i++)
		{
			wetcopy->add_bosshp(0);
		}
		wetcopy->set_copyid(copyid);
		bool ret = InitGuildWetCopyMonster(wetcopy);
		if (ret== false)
			return true;
		wetcopy->set_guildid(guildid);
		wetcopy->set_status(true);
		wetcopy->set_fighterid(INVALID_GUID);
		wetcopy->set_fightname("");
		wetcopy->set_opentime(MtTimerManager::Instance().CurrentDate());
		guild->wetcopyinfo_.push_back(wetcopy);
		guild->guilddb->set_maxwetcopyid(copyid);

		SendGuildWetCopy(player_.get(), wetcopy.get());

		return true;
	}
	
	bool MtGuildManager::OnResetGuildWetCopyReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ResetGuildWetCopy>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		Packet::GuildPosition position = (Packet::GuildPosition)GetPlayerGuildPosition(player_->Guid(), guild.get());
		if (position != Packet::GuildPosition::CHIEFTAIN && position != Packet::GuildPosition::VICE_CHIEFTAIN)
		{
			player_->SendClientNotify("Guild_NotchieftainAndVICE", -1, -1);
			return true;
		}
		int32 copyid = message->copyid();
		auto guildwetcopy = guild->GetGuildWetCopyInfo(copyid);
		if (guildwetcopy == nullptr)
			return true;
		auto wetcopyconfig = GetGuildWetCopyConfig(copyid);
		if (wetcopyconfig == nullptr)
			return true;

		//cost
		int32 coincost = wetcopyconfig->coincost();
		if (guild->GetGuildMoney() < coincost)
		{
			player_->SendClientNotify("Guild_MoneyNotEnough", -1, -1);
			return true;
		}
		guild->AddGuildMoney(-coincost, player_.get());
		bool ret = InitGuildWetCopyMonster(guildwetcopy);
		if (ret == false)
			return true;
		guildwetcopy->set_status(true);
		guildwetcopy->set_fighterid(INVALID_GUID);
		guildwetcopy->set_fightname("");
		guildwetcopy->set_opentime(MtTimerManager::Instance().CurrentDate());
		SendGuildWetCopy(player_.get(), guildwetcopy.get());
		return true;
	}

	void MtGuildManager::SendGuildWetCopy(MtPlayer* player, Packet::GuildWetCopyInfo* wetcopy)
	{
		if (player == nullptr || wetcopy == nullptr)
			return;
		Packet::GuildWetCopyInfoUpdate update;
		auto wetcopy_ = update.mutable_wetcopy();
		wetcopy_->CopyFrom(*wetcopy);
		player->SendMessage(update);
	}
	bool MtGuildManager::OnGetGuildListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildListReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		MtGuildManager::Instance().OnGetGuildList(player_);
		return true;
	}
	bool MtGuildManager::OnGetGuildInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildInfoReq>& message, int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		MtGuildManager::Instance().OnGetGuildInfo(player.get());
		return true;
	}
	bool MtGuildManager::OnGetGuildUserListInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildUserListInfoReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		guild->SendGuildUserList(player_.get());
		return true;
	}
	void MtGuildManager::OnGetGuildList(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
			return;
		Packet::GuildBaseListReply reply;
		for (auto guild : guildlist_)
		{
			auto bguild = reply.add_list();
			bguild->set_guid(guild->guilddb->guid());
			bguild->set_name(guild->guilddb->name());
			bguild->set_lastname(guild->guilddb->lastname());
			bguild->set_notice(guild->guilddb->notice());
			bguild->set_guildlevel(guild->guilddb->guildlevel());
			bguild->set_membersize(guild->guilduserlist.size());
			bguild->set_maxmembersize(guild->GetGuildUserMaxSize());
			bguild->set_chieftainname(guild->guilddb->chieftainname());
			bguild->set_chieftainguid(guild->guilddb->chieftain());
			bguild->set_createtime(guild->guilddb->createtime());
		}
		player_->SendMessage(reply);
	}
	void MtGuildManager::OnGetGuildInfo(MtPlayer* player_)
	{
		if (player_ == nullptr)
			return;
		uint64 guildid = player_->GetGuildID();
		if (guildid == INVALID_GUID)
		{
			ZXERO_ASSERT(false) << "OnGetGuildInfo playerid="<<player_->Guid();
			return;
		}
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
		{
			Server::Instance().SendG2SCommonMessage(player_, "G2SUpdateGuildInfo", {}, { (int64)INVALID_GUID }, { "","GetGuildNil" });
			return;
		}
		auto guilduser = guild->GetGuildUser(player_->Guid());
		if (guilduser == nullptr)
		{
			Server::Instance().SendG2SCommonMessage(player_, "G2SUpdateGuildInfo", {}, { (int64)INVALID_GUID }, { "","GetGuildUserNil" });
			return;
		}
		guild->SendGuildUserList(player_);

		Packet::GetGuildInfoReply reply;
		reply.mutable_info()->CopyFrom(*guild->guilddb.get());
		reply.set_membersize(GetGuildMemberSize(guild));
		reply.set_maxmembersize(guild->GetGuildUserMaxSize());
		reply.mutable_selfinfo()->CopyFrom(*guilduser.get());
		player_->SendMessage(reply);
		if (guild->guilddb->chieftain() == player_->Guid())
		{
			guild->SendApplyList(player_);
		}
	}

	void MtGuildManager::FillGuildUserDamage(guilduserdamage& damage, MtPlayer* player_, GuildInfo* guild, int32 copyid, int32 damagevalue, Packet::ActorFigthFormation formation)
	{
		if (player_ == nullptr || guild == nullptr)
			return;
		damage.data->set_guid(MtGuid::Instance()(*damage.data));
		damage.data->set_playerid_(player_->Guid());
		damage.data->set_guildid_(player_->GetGuildID());
		damage.data->set_paramid_(copyid);
		damage.data->set_playerhair_(player_->GetScenePlayerInfo()->hair());
		damage.data->set_playername_(player_->GetScenePlayerInfo()->name());
		damage.data->set_damagevalue_(damagevalue);
		damage.data->set_guildname_(guild->guilddb->name());

		auto battleactors = player_->BattleActor(formation);
		for (size_t i = 0; i < battleactors.size() && i < 5; ++i)
		{
			damage.data->set_actor_config_id_(i,battleactors[i]->ConfigId());
		}
	}
	bool MtGuildManager::OnGetGuildBossCompetitionReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildBossCompetitionReq>& message, int32 /*source*/)
	{
		message;
		session;
		//auto player_ = session->player();
		//if (player_ == nullptr) {
		//	ZXERO_ASSERT(false);
		//	return true;
		//}
		//uint64 guildid = player_->GetGuildID();
		//auto guild = GetGuildByid(guildid);
		//if (guild == nullptr)
		//	return true;
		//Packet::GetGuildBossCompetitionReply reply;
		//reply.mutable_info()->CopyFrom(*boss_->guildbossdb.get());
		return true;
	}
	bool MtGuildManager::OnGetGuildUserDamageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetGuildUserDamageReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		Packet::GetGuildUserDamageReply reply;
		for (auto guid : guild->guildranklist_.m_data)
		{
			auto user = reply.add_users_();
			user->CopyFrom(*guid.data.get());
		}
		player_->SendMessage(reply);
		return true;
	}
	bool MtGuildManager::OnGetServerUserDamageReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GetServerUserDamageReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}

		Packet::GetGuildAllDamageReply reply;
		for (auto guid : serverranklist_.m_data)
		{
			auto user = reply.add_guilds_();
			user->CopyFrom(*guid.data.get());
		}
		player_->SendMessage(reply);
		return true;
	}
	void MtGuildManager::OnGetGuildBossUserDamage(MtPlayer* player_)
	{
		if (player_ == nullptr)
			return;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return;
		Packet::GetGuildBossUserDamageReply reply;
		for (auto user : guild->guildranklist_.m_data)
		{
			auto userdata = reply.add_users_();
			userdata->CopyFrom(*user.data.get());
		}
		player_->SendMessage(reply);
	}
	void MtGuildManager::OnGetGuildCopyDamage(MtPlayer* player_, int32 copyid)
	{
		if (player_ == nullptr)
			return;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return;
		auto iter = guild->guildwetcopyranklist_.find(copyid);
		if (iter == guild->guildwetcopyranklist_.end())
		{
			return;
		}

		Packet::GetGuildUserDamageReply reply;
		for (auto guid : iter->second.m_data)
		{
			auto user = reply.add_users_();
			user->CopyFrom(*guid.data.get());
		}
		player_->SendMessage(reply);
	}
	void MtGuildManager::OnGetGuildCopyServerDamage(MtPlayer* player_)
	{
		if (player_ == nullptr)
			return;
		Packet::GetGuildUserDamageReply reply;
		for (auto guid : serverwetcopyranklist_.m_data)
		{
			auto user = reply.add_users_();
			user->CopyFrom(*guid.data.get());
		}
		player_->SendMessage(reply);
	}
	void MtGuildManager::OnGetGuildWarehouseInfos(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
			return;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return;
		Packet::GuildWarehouseReply reply;
		for (auto warehouse_ : guild->warehouse_)
		{
			if (warehouse_->itemid() != INVALID_INT32)
			{
				auto warehouse = reply.add_whlist();
				warehouse->CopyFrom(*warehouse_.get());
			}
		}
		player_->SendMessage(reply);
	}
	bool MtGuildManager::OnGuildWarehouseReceiveReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::GuildWarehouseReceiveReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto guild = GetGuildByid(player->GetGuildID());
		if (guild == nullptr) 
			return true;

		uint64 playerguid = player->Guid();
		auto user = guild->GetGuildUser(playerguid);
		if (user == nullptr)
			return true;
		auto warehouse = guild->GetWarehouse(message->index());
		if (warehouse== nullptr)
		{
			LOG_ERROR << "OnGuildWarehouseReceiveReq no warehouse " << playerguid << "," << message->index();
			return true;
		}
		int32 itemid = warehouse->itemid();
		if (itemid == INVALID_INT32)
		{
			LOG_ERROR << "OnGuildWarehouseReceiveReq no itemid " << playerguid << "," << message->index();
			return true;
		}
		if (warehouse->receiveplayerid() != playerguid)
		{
			LOG_ERROR << "OnGuildWarehouseReceiveReq no player " << playerguid << "," << message->index()<<","<<itemid<<","<< warehouse->receiveplayerid();
			return true;
		}

		google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
		auto item_ = items.Add();
		item_->set_itemid(itemid);
		item_->set_itemcount(1);
		static_message_manager::Instance().create_message(playerguid, nullptr, MAIL_OPT_TYPE::GUILD_ADD, "zhanliping", "zhanlipingcontent", &items);

		guild->AddWarehouseRecord(itemid, player->Name());
		warehouse->set_itemid(INVALID_INT32);
		warehouse->set_receiveplayerid(INVALID_GUID);

		Packet::GuildWarehouseUpdate update;
		update.mutable_item()->CopyFrom(*warehouse.get());
		player->SendMessage(update);

		//del
		guild->DelWarehouse(warehouse->index());
		LOG_INFO << "OnGuildWarehouseReceiveReq "<<playerguid<<","<< message->index()<<","<< itemid;
		return true;
	}
	bool MtGuildManager::OnGuildWarehouseSetReceivePlayerReq(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::GuildWarehouseSetReceivePlayerReq>& message,
		int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto guild = GetGuildByid(player->GetGuildID());
		if (guild == nullptr)
			return true;
		auto user = guild->GetGuildUser(player->Guid());
		if (user == nullptr)
			return true;
		if (user->position() != Packet::GuildPosition::CHIEFTAIN)
		{
			return true;
		}
		uint64 receiveplayerguid = message->receiveplayerid();
		auto receiveuser = guild->GetGuildUser(receiveplayerguid);
		if (receiveuser == nullptr)
			return true;
		
		auto warehouse = guild->GetWarehouse(message->index());
		if (warehouse == nullptr)
			return true;
		
		int32 itemid = warehouse->itemid();
		if (itemid == INVALID_INT32)
			return true;
		
		warehouse->set_receiveplayerid(receiveplayerguid);
		Packet::GuildWarehouseUpdate update;
		auto item = update.mutable_item();
		item->CopyFrom(*warehouse.get());
		player->SendMessage(update);
		return true;
	}

	bool MtGuildManager::OnFindGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindGuildReq>& req, int32 /*source*/)
	{
		try
		{
			auto player_ = session->player();
			if (player_ == nullptr) {
				ZXERO_ASSERT(false);
				return true;
			}

			uint64 guid = req->guid();
			std::string name = req->name();
			if (name == "" && guid != INVALID_GUID)
			{
				name = boost::lexical_cast<std::string>(guid);
			}
			auto it = std::find_if(guildlist_.begin(), guildlist_.end(), [=](auto& iter)
			{
				return (iter->guilddb->guid() == guid);
			});
			
			if (it == guildlist_.end())
			{
				it = std::find_if(guildlist_.begin(), guildlist_.end(), [=](auto& iter)
				{
					return (iter->guilddb->name() == name);
				});
				if (it == guildlist_.end())
				{
					Packet::GuildBaseListReply reply;
					player_->SendMessage(reply);
					return true;
				}
			}

			Packet::GuildBaseListReply reply;
			auto info = reply.add_list();
			info->set_guid((*it)->guilddb->guid());
			info->set_name((*it)->guilddb->name());
			info->set_lastname((*it)->guilddb->lastname());
			info->set_notice((*it)->guilddb->notice());
			info->set_guildlevel((*it)->guilddb->guildlevel());
			info->set_chieftainname((*it)->guilddb->chieftainname());
			info->set_chieftainguid((*it)->guilddb->chieftain());
			info->set_membersize(GetGuildMemberSize((*it)));
			info->set_maxmembersize((*it)->GetGuildUserMaxSize());
			info->set_createtime((*it)->guilddb->createtime());
			player_->SendMessage(reply);
		}
		catch (...)
		{

		}
		return true;
	}
	bool MtGuildManager::OnApplyGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyGuildReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto guild=GetGuildByid(req->guid());
		if (guild == nullptr)
		{
			player_->SendClientNotify("Guild_NoCunZai", -1, -1);
			return true;
		}

		Packet::ResultCode ret = guild->ApplyGuild(player_);
		if (ret == Packet::ResultCode::ResultOK)
		{
			std::stringstream notify;
			notify << "Guild_ApplyOK|" << guild->guilddb->name();
			std::string notifystr = notify.str();
			player_->SendClientNotify(notifystr, -1, -1);
		}
		else
		{
			player_->SendCommonResult(Packet::ResultOption::Guild_Opt, ret);
		}
		return true;
	}
	bool MtGuildManager::OnOneKeyApplyGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::OneKeyApplyGuildReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;

		for (auto guild : guildlist_)
		{
			guild->ApplyGuild(player_);
		}
		std::stringstream notify;
		notify << "Guild_ApplyOneKeyOK" ;
		std::string notifystr = notify.str();
		player_->SendClientNotify(notifystr, -1, -1);
		return true;
	}
	bool MtGuildManager::OnAgreeApplyGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AgreeApplyGuildReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		uint64 applyguid = req->playerguid();
		if (std::find_if(guild->applyplayerlist.begin(), guild->applyplayerlist.end(), [=](auto& iter)
			{
				return (iter.playerguid() == applyguid);
			}) == guild->applyplayerlist.end())
		{
			return true;
		}
		for (auto iter = guild->applyplayerlist.begin(); iter != guild->applyplayerlist.end();)
		{
			if ((*iter).playerguid() == applyguid)
			{
				iter = guild->applyplayerlist.erase(iter);
			}
			else
			{
				iter++;
			}
		}
		guild->SendApplyList(player_.get());
		bool agree = req->agree();
		if (agree == true)
		{
			auto applyplayer = Server::Instance().FindPlayer(applyguid);
			if (applyplayer == nullptr)
			{
				auto sceneinfo = Server::Instance().FindPlayerSceneInfo(applyguid);
				if (sceneinfo != nullptr)
				{
					if (sceneinfo->basic_info().guildid() != INVALID_GUID)
					{
						player_->SendClientNotify("Guild_Have", -1, -1);
						return true;
					}
				}
				Server::Instance().AddDbLoadTask(new LoadPlayerGuildApplyData(player_, applyguid));
				return true;
			}

			if (applyplayer->GetGuildID() != INVALID_GUID)
			{
				player_->SendClientNotify("Guild_Have", -1, -1);
				return true;
			}
			Packet::ResultCode ret = guild->AddGuild(applyplayer);
			player_->SendCommonResult(Packet::ResultOption::Guild_Opt, ret);
		}
		return true;
	}
	bool MtGuildManager::OnApplyGuildPlayerListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyGuildPlayerListReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		guild->SendApplyList(player_.get());
		return true;
	}
	bool MtGuildManager::OnClearApplyGuildPlayerListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ClearApplyGuildPlayerListReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_ == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		if (GetPlayerGuildPosition(player_->Guid(), guild.get()) != Packet::GuildPosition::CHIEFTAIN)
		{
			return true;
		}
		guild->applyplayerlist.clear();
		guild->SendApplyList(player_.get());
		return true;
	}

	bool MtGuildManager::OnInviteyAddGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAddGuildReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto targetplayer = Server::Instance().FindPlayer(req->playerguid());
		if (targetplayer == nullptr)
			return true;
		uint64 guildid = player_->GetGuildID();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		uint64 targetguildid = targetplayer->GetGuildID();
		if (targetguildid != INVALID_GUID)
		{
			player_->SendClientNotify("Guild_Have", -1, -1);
			return true;
		}
		if (guild->IsFullGuildUser() == true)
		{
			player_->SendClientNotify("Guild_Full", -1, -1);
			return true;
		}
		Packet::AskInviteyAgreeGuildReply reply;
		reply.set_guildid(guildid);
		reply.set_guildname(guild->guilddb->name());
		reply.set_guildinvitename(player_->GetScenePlayerInfo()->name());
		reply.set_guildinviteguid(player_->Guid());
		targetplayer->SendMessage(reply);
		return true;
	}

	bool MtGuildManager::OnInviteyAgreeGuildReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAgreeGuildReq>& req, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player_->GetGuildID() != INVALID_GUID)
			return true;
		uint64 guildid = req->guildid();
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return true;
		uint64 guildidinviteguid = req->guildidinviteguid();
		auto guildidinviteuser = guild->GetGuildUser(guildidinviteguid);
		if (guildidinviteuser == nullptr)
			return true;
		
		bool agree = req->agree();
		if (agree == false)
		{
			auto guildidinviteplayer = Server::Instance().FindPlayer(guildidinviteguid);
			if (guildidinviteplayer != nullptr)
			{
				guildidinviteplayer->SendClientNotify("jujue", -1,-1);
				return true;
			}
		}
		Packet::ResultCode ret = Packet::ResultCode::ResultOK;
		Packet::GuildPosition position = guildidinviteuser->position();
		if (position == Packet::GuildPosition::TRAINEE)
		{
			ret = guild->ApplyGuild(player_);
		}
		else
		{
			ret = guild->AddGuild(player_);
		}
		player_->SendCommonResult(Packet::ResultOption::Guild_Opt, ret);
		return true;
	}
	//只能主线程使用
	boost::shared_ptr<Packet::GuildUser> MtGuildManager::GetGuildUser(uint64 guildid, uint64 playerid)
	{
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return nullptr;
		auto guilduser = guild->GetGuildUser(playerid);
		return guilduser;
	}
	
	int32 MtGuildManager::GetGuildMemberSize(const boost::shared_ptr<GuildInfo> guild_)
	{
		if (guild_ == nullptr)
			return 0;
		return guild_->guilduserlist.size();
	}
	boost::shared_ptr<GuildInfo> MtGuildManager::GetGuildByid(uint64 guid)
	{
		auto it = std::find_if(guildlist_.begin(), guildlist_.end(), [=](auto& iter)
		{
			return (iter->guilddb->guid() == guid );
		});
		if (it == guildlist_.end())
		{
			return nullptr;
		}
		return *it;
	}
	void MtGuildManager::AddLeaveUser(boost::shared_ptr<Packet::GuildUser> user)
	{
		if (user)
		{
			uint64 guildid = user->guildid();
			user->set_lastguildid(guildid);
			user->set_guildid(INVALID_GUID);
			leaveuserlist.push_back(user);
		}
	}
	boost::shared_ptr<Packet::GuildUser> MtGuildManager::GetLeaveUser(uint64 playerid)
	{
		for (auto user= leaveuserlist.begin(); user != leaveuserlist.end(); user++)
		{
			if ((*user)->playerguid() == playerid)
			{
				return (*user);
			}
		}
		return nullptr;
	}
	void MtGuildManager::DelLeaveUser(uint64 playerid)
	{
		for (auto user = leaveuserlist.begin(); user != leaveuserlist.end(); )
		{
			if ((*user)->playerguid() == playerid)
			{
				user = leaveuserlist.erase(user);
			}
			else
			{
				user++;
			}
		}
	}
	boost::shared_ptr<GuildInfo> MtGuildManager::GetGuildByName(std::string name)
	{
		auto it = std::find_if(guildlist_.begin(), guildlist_.end(), [=](auto& iter)
		{
			return (iter->guilddb->name() == name);
		});
		if (it == guildlist_.end())
		{
			return nullptr;
		}
		return *it;
	}
	void MtGuildManager::BroadcastGuild(uint64 guildid, const google::protobuf::Message& msg)
	{
		auto guild=GetGuildByid(guildid);
		if (guild != nullptr)
		{
			guild->BroadcastUser(msg);
		}
	}
	int32 MtGuildManager::GetPlayerGuildPosition(uint64 playerguid, GuildInfo* guild)
	{
		auto guilduser = guild->GetGuildUser(playerguid);
		if (guilduser == nullptr)
			return Packet::GuildPosition::TRAINEE;
		
		return guilduser->position();
	}
	boost::shared_ptr<Config::GuildBuildLevelConfig> MtGuildManager::GetBuildLevelConfig(int32 buildtype, int32 buildlevel)
	{
		auto it = std::find_if(guildbuild_.begin(), guildbuild_.end(), [=](auto& iter)
		{
			return (iter->buildtype() == buildtype && iter->buildlevel() == buildlevel);
		});
		if (it == guildbuild_.end())
		{
			return nullptr;
		}
		return *it;
	}
	boost::shared_ptr<Config::GuildLevelFixConfig> MtGuildManager::GetGuildLevelFixConfig(int32 level)
	{
		level--;
		if (level < 0 || level >= (int32)guildlevelfix_.size())
			return nullptr;
		return guildlevelfix_[level];
	}
	boost::shared_ptr<Config::GuildPracticeLevelConfig> MtGuildManager::GetPracticeLevelConfig(int32 level)
	{
		auto it = guildpracticelevel_.find(level);
		if (it != guildpracticelevel_.end())
		{
			return it->second;
		}
		return nullptr;
	}
	boost::shared_ptr<Config::GuildPracticeConfig> MtGuildManager::GetPracticeConfig(int32 id)
	{
		auto it = guildpractice_.find(id);
		if (it != guildpractice_.end())
		{
			return it->second;
		}
		return nullptr;
	}
	boost::shared_ptr<Config::GuildCDKConfig> MtGuildManager::GetGuildCDKConfig(int32 id)
	{
		auto it = guildcdk_.find(id);
		if (it != guildcdk_.end())
		{
			return it->second;
		}
		return nullptr;
	}
	boost::shared_ptr<Config::GuildWetCopyConfig> MtGuildManager::GetGuildWetCopyConfig(int32 id)
	{
		auto it = guildwetcopy_.find(id);
		if (it != guildwetcopy_.end())
		{
			return it->second;
		}
		return nullptr;
	}

	boost::shared_ptr<Config::GuildPositionConfig> MtGuildManager::GetGuildGuildPositionConfig(int32 id)
	{
		if (id>= 0 && id < (int32)(guildposition_.size()))
		{
			return guildposition_[id];
		}
		return nullptr;
	}

	boost::shared_ptr<Config::GuildDonateConfig> MtGuildManager::GetGuildDonateConfig(int32 type)
	{
		if (type >= 0 && type < (int32)(guilddonate_.size()))
		{
			return guilddonate_[type];
		}
		return nullptr;
	}
	const boost::shared_ptr<Config::GuildWetCopyStageConfig> MtGuildManager::GetGuildWetCopyFristStage(int32 copyid)
	{
		for (auto stage : guildwetcopystage_)
		{
			if (stage->copy_id() == copyid )
			{
				return stage;
			}
		}
		return nullptr;
	}
	const boost::shared_ptr<Config::GuildWetCopyStageConfig> MtGuildManager::GetGuildWetCopyStage(int32 stageid)
	{
		for (auto stage : guildwetcopystage_)
		{
			if (stage->id() == stageid)
			{
				return stage;
			}
		}
		return nullptr;
	}
	bool MtGuildManager::InGuildWetCopyScene(int32 sceneid)
	{
		for (auto wc : guildwetcopy_)
		{
			if (wc.second->sceneid() == sceneid)
			{
				return true;
			}
		}
		return false;
	}
	bool MtGuildManager::NeedPracticeLevelUp(int32 level, int32 exp)
	{
		boost::shared_ptr<Config::GuildPracticeLevelConfig> config = GetPracticeLevelConfig(level);
		if (config == nullptr)
			return false;
		if (exp >= config->exp())
			return true;
		return false;
	}
	bool MtGuildManager::AddPracticeLevel(boost::shared_ptr<Packet::PracticeInfo> practice)
	{
		if (practice == nullptr)
			return false;
		int32 level = practice->level();
		int32 exp = practice->exp();
		boost::shared_ptr<Config::GuildPracticeLevelConfig> config = GetPracticeLevelConfig(level);
		if (config == nullptr)
			return false;
		level++;
		exp -= config->exp();
		practice->set_exp(exp);
		practice->set_level(level);
		return true;
	}


	void GuildInfo::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<GuildInfo, ctor()>(ls, "GuildInfo")
			.def(&GuildInfo::GetGuildWetCopyInfo, "GetGuildWetCopyInfo")
			.def(&GuildInfo::GetBuildLevel, "GetBuildLevel")
			.def(&GuildInfo::CanenterBattle,"CanenterBattle")
			.def(&GuildInfo::GetGuildId,"GetGuildId")
			.def(&GuildInfo::GetGuildUser,"GetGuildUser")
			.def(&GuildInfo::GetGuildUserByPosition,"GetGuildUserByPosition")
			.def(&GuildInfo::G2SSyncGuildUserData,"G2SSyncGuildUserData")
			.def(&GuildInfo::SendGuildUserList,"SendGuildUserList")
			.def(&GuildInfo::DeleteUser,"DeleteUser")
			.def(&GuildInfo::GetGuildUserSizeLessLoginDay, "GetGuildUserSizeLessLoginDay")
			.def(&GuildInfo::GetGuildUserSize,"GetGuildUserSize")
			.def(&GuildInfo::AddUserContribution,"AddUserContribution")
			.def(&GuildInfo::GetDBInfo, "GetDBInfo")
			.def(&GuildInfo::AddWarehouse, "AddWarehouse")
			.def(&GuildInfo::ChieftainGuid,"ChieftainGuid")
			.def(&GuildInfo::SettBuildLevel,"SettBuildLevel")
			.def(&GuildInfo::GetWetCopyTimes,"GetWetCopyTimes")
			.def(&GuildInfo::AddWetCopytimes, "AddWetCopytimes")
			.def(&GuildInfo::AddGuildMoney,"AddGuildMoney")
			.def(&GuildInfo::AddGuildMoney_S, "AddGuildMoney_S")
			.def(&GuildInfo::AddGuildWeekBonus,"AddGuildWeekBonus")
			.def(&GuildInfo::SendSpoilItem,"SendSpoilItem")
			.def(&GuildInfo::SendUpdateSpoilItem,"SendUpdateSpoilItem")
			.def(&GuildInfo::GetSpoilItem,"GetSpoilItem")
			.def(&GuildInfo::GetSpoilItemByIndex,"GetSpoilItemByIndex")
			.def(&GuildInfo::SpoilItemJumpPlayer,"SpoilItemJumpPlayer")
			.def(&GuildInfo::AddRecord, "AddRecord")
			.def(&GuildInfo::SendRecord,"SendRecord")
			.def(&GuildInfo::GetGuildImpeach, "GetGuildImpeach")
			.def(&GuildInfo::SendGuildImpeach, "SendGuildImpeach")
			.def(&GuildInfo::SetGuildWetCDTime,"SetGuildWetCDTime")
			;
	}
	GuildInfo::GuildInfo()
	{
		applyplayerlist.clear();
		cdklist.clear();
		newcdkguid = 0;
		impeadch_ = boost::make_shared<Packet::GuildImpeach>();
		ResetGuildImpeach();
	}
	GuildInfo::~GuildInfo()
	{
		applyplayerlist.clear();
		cdklist.clear();
		newcdkguid = 0;
	}
	void GuildInfo::OnSecondTick(int32 elapseTime)
	{
		for (auto spitem = guildspoil_item_.begin(); spitem != guildspoil_item_.end();)
		{
			int32 applyplayersize = 0;
			for (int32 i=0;i<(*spitem)->playerid_size();i++)
			{
				if ((*spitem)->playerid(i) != INVALID_GUID)
				{
					applyplayersize++;
				}
			}
			if (applyplayersize == 0)
			{
				spitem++;
			}
			else
			{
				int32 cd = (*spitem)->timecd();
				if (cd > elapseTime)
				{
					(*spitem)->set_timecd(cd - elapseTime);
					spitem++;
				}
				else
				{
					int32 itemid = (*spitem)->itemid();
					uint64 playerid = (*spitem)->playerid(0);
					google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
					auto item = items.Add();
					item->set_itemid(itemid);
					item->set_itemcount(1);
					static_message_manager::Instance().create_message(
						playerid,
						nullptr,
						MAIL_OPT_TYPE::GUILD_ADD,
						"zhanliping",
						"zhanliping",
						&items);
					(*spitem)->SetDeleted();
					MtGuildManager::Instance().Cache2Save((*spitem).get(), "SpoilItem");
					spitem = guildspoil_item_.erase(spitem);
				}
			}
		}

		if (impeadch_)
		{
			int32 endtime = impeadch_->endtime();
			if (impeadch_->impeachguild() != INVALID_GUID && endtime > 0)
			{
				if (impeadch_->respondguild_size() >= impeadch_->needcount())
				{
					auto impeadchuser = GetGuildUser(impeadch_->impeachguild());
					if (impeadchuser)
					{
						auto chiefuser = GetGuildUser(guilddb->chieftain());
						if (chiefuser)
						{
							chiefuser->set_position(Packet::TRAINEE);
						}
						impeadchuser->set_position(Packet::CHIEFTAIN);
						std::string recordstr = "xxtanhechengweihuizhang|" + impeadchuser->playername() + "|" + guilddb->chieftainname();
						AddRecord(recordstr);
						guilddb->set_chieftain(impeadch_->impeachguild());
						guilddb->set_chieftainname(impeadchuser->playername());
						send_guild_system_message(recordstr, guilddb->guid());
					}
					ResetGuildImpeach();
				}

				if (endtime > elapseTime)
				{
					impeadch_->set_endtime(endtime - elapseTime);
				}
				else
				{
					ResetGuildImpeach();
				}
			}
		}

		//副本保护时间
		for (auto& cd : wetcopycdtime_)
		{
			if (cd.second > 0)
			{
				if (cd.second > elapseTime)
				{
					cd.second -= elapseTime;
				}
				else
				{
					cd.second = 0;
					auto wetcopy = GetGuildWetCopyInfo(cd.first);
					if (wetcopy == nullptr)
					{
						ZXERO_ASSERT(false) << "copy null guid = " << GetGuildId() << " copyid=" << cd.first;
						continue;
					}
					if (wetcopy->fighterid() != INVALID_GUID || wetcopy->fightname() != "")
					{
						wetcopy->set_fighterid(INVALID_GUID);
						wetcopy->set_fightname("");
						ZXERO_ASSERT(false) << "fighter no reset guid=" << GetGuildId() << " copyid=" << cd.first << " playerid="<<wetcopy->fighterid() <<" fightname="<<wetcopy->fightname();
					}
				}
			}
		}
	}
	uint64 GuildInfo::GetGuildId()
	{
		if (guilddb != nullptr)
		{
			return guilddb->guid();
		}
		return INVALID_GUID;
	}
	void GuildInfo::SetGuildName(std::string name, MtPlayer* player)
	{
		std::string lasetname = guilddb->name();
		guilddb->set_lastname(lasetname);
		guilddb->set_name(name);
		if (player)
		{
			Packet::SetGuildNameRR reply;
			reply.set_name(name);
			reply.set_lastname(lasetname);
			player->SendMessage(reply);
		}
		BroadG2SCommonMessage("G2SUpdateGuildName", {}, {}, { name });
	}
	std::string GuildInfo::GetGuildName()
	{
		return guilddb->name();
	}
	boost::shared_ptr<Packet::GuildUser> GuildInfo::GetGuildUser(uint64 playerguid)
	{
		auto it = std::find_if(guilduserlist.begin(), guilduserlist.end(), [=](auto& iter)
		{
			return (iter->playerguid() == playerguid);
		});
		if (it != guilduserlist.end())
			return *it;
		return nullptr;
	}
	boost::shared_ptr<Packet::GuildUser>  GuildInfo::GetGuildUserByPosition(int32 position)
	{
		auto it = std::find_if(guilduserlist.begin(), guilduserlist.end(), [=](auto& iter)
		{
			return (iter->position() == position);
		});
		if (it != guilduserlist.end())
			return *it;
		return nullptr;
	}
	int32 GuildInfo::GetGuildUserSize() const
	{
		return guilduserlist.size();
	}
	int32 GuildInfo::GetGuildBattleUserSize() const
	{
		auto active = MtActivityManager::Instance().FindActivity(25);
		if (active == nullptr)
		{
			return 0;
		}
		int32 size_ = 0;
		for (auto user : guilduserlist)
		{
			if (user->playerlevel() >= active->open_level())//7*24*60*60
			{
				size_++;
			}
		}
		return size_;
	}
	int32 GuildInfo::GetActiveGuildUserSize()
 	{
		auto active = MtActivityManager::Instance().FindActivity(25);
		if (active == nullptr)
		{
			return 0;
		}
		int32 size_ = 0;
		for (auto user : guilduserlist)
		{
			
			if (user->playerlevel() >= active->open_level() && MtTimerManager::Instance().DiffTimeToNow(user->lastlogouttime())<= 604800)//7*24*60*60
			{
				size_++;
			}
		}
		return size_;
	}
	int32 GuildInfo::GetGuildUserSizeLessLoginDay(int32 day)
	{
		uint32 allsecond = day * 24 * 3600;
		int32 size_ = 0;
		for (auto user : guilduserlist)
		{
			if ( MtTimerManager::Instance().DiffTimeToNow(user->lastlogouttime()) <= allsecond)//7*24*60*60
			{
				size_++;
			}
		}
		return size_;
	}
	int32 GuildInfo::GetGuildUserMaxSize()
	{
		boost::shared_ptr<Config::GuildBuildLevelConfig> config = MtGuildManager::Instance().GetBuildLevelConfig(3, GetBuildLevel(3));
		if (config == nullptr)
			return 0;
		return config->param(0);
	}
	bool GuildInfo::IsFullGuildUser()
	{
		if (GetGuildUserSize() >= GetGuildUserMaxSize())
			return true;
		return false;
	}
	
	void GuildInfo::DeleteUser(uint64 playerid)
	{
		bool ischieftain = false;
		for (auto iter = guilduserlist.begin(); iter != guilduserlist.end();)
		{
			if ((*iter)->playerguid() == playerid) {
				//标记和删除
				//(*iter)->SetDeleted();
				//MtShmManager::Instance().Cache2Shm((*iter).get());
				MtGuildManager::Instance().AddLeaveUser((*iter));
				if ((*iter)->position() == Packet::GuildPosition::CHIEFTAIN)
					ischieftain = true;
				iter = guilduserlist.erase(iter);
			}
			else {
				iter++;
			}
		}

		if (ischieftain == true)
		{
			int32 maxcontribute = 0;
			uint64 maxcontributeguild = INVALID_GUID;
			for (auto iter = guilduserlist.begin(); iter != guilduserlist.end(); iter++)
			{
				if ((*iter)->position() == Packet::GuildPosition::VICE_CHIEFTAIN)
				{
					(*iter)->set_position(Packet::GuildPosition::CHIEFTAIN);
					guilddb->set_chieftain((*iter)->playerguid());
					guilddb->set_chieftainname((*iter)->playername());
					return;
				}
				if ((*iter)->allcontribution() > maxcontribute)
				{
					maxcontributeguild = (*iter)->playerguid();
				}
			}
			if (maxcontributeguild != INVALID_GUID)
			{
				auto playeruser = GetGuildUser(maxcontributeguild);
				if (playeruser)
				{
					playeruser->set_position(Packet::GuildPosition::CHIEFTAIN);
					guilddb->set_chieftain(maxcontributeguild);
					guilddb->set_chieftainname(playeruser->playername());
				}
			}
		}
	}
	void GuildInfo::InitGuildUser(MtPlayer* player_, boost::shared_ptr<Packet::GuildUser> user, Packet::GuildPosition position,  uint64 guildid)
	{
		if (player_ == nullptr)
			return;
		auto main_actor = player_->MainActor();
		if (main_actor != nullptr) {
			auto& db_info = main_actor->DbInfo();
			user->set_main_actor_score(db_info->score());
			user->set_main_actor_config_id(db_info->actor_config_id());
			user->set_main_actor_color(db_info->color());
			user->set_main_actor_star(db_info->star());
		} else {
			user->SetInitialized();
		}
		uint64 playerid = player_->Guid();
		user->set_playerguid(playerid);
		user->set_guildid(guildid);
		user->set_jointime(MtTimerManager::Instance().CurrentDate());
		user->set_position(position);
		user->set_bdonation(0);
		user->set_bbonus(false);
		user->set_curcontribution(0);
		user->set_weekcontribution(0);
		user->set_allcontribution(0);
		user->set_battletime(0);
		user->set_bossdamage(0);
		user->set_wetcopytimes(0);
		user->set_spoiljumptimes(0);
		auto lastuser = MtGuildManager::Instance().GetLeaveUser(playerid);
		if (lastuser)
		{
			user->set_lastguildid(guildid);
			if (lastuser->lastguildid() == guildid)//回到原来的帮会
			{
				user->set_curcontribution(lastuser->curcontribution());
				user->set_allcontribution(lastuser->allcontribution());
			}
			else
			{
				user->set_curcontribution(lastuser->curcontribution()/2);
				user->set_allcontribution(lastuser->allcontribution());
			}
			MtGuildManager::Instance().DelLeaveUser(playerid);
		}
		else
		{
			user->set_lastguildid(INVALID_GUID);
		}
	}

	void GuildInfo::UpdateGuildUser(MtPlayer* player_, boost::shared_ptr<Packet::GuildUser> user)
	{
		if (player_ == nullptr)
			return;

		auto db_info = player_->GetDBPlayerInfo();
		if (db_info == nullptr) {
			return;
		}
		user->set_playername(db_info->basic_info().name());
		user->set_playerlevel(db_info->basic_info().level());
		user->set_online(player_->Online());
		user->set_lastlogouttime(db_info->last_logout_time());
		user->set_hair(db_info->basic_info().hair());
	}

	void GuildInfo::G2SSyncGuildUserData(MtPlayer* player_, Packet::GuildUser* guilduser)
	{
		if (player_ && guilduser&&guilddb)
		{
			auto syncdata = boost::make_shared<G2S::SyncPlayerGuildData>();
			if (syncdata)
			{
				auto playerguilddata = syncdata->mutable_guild();
				if (playerguilddata)
				{
					playerguilddata->set_position(guilduser->position());
					playerguilddata->set_bdonation(guilduser->bdonation());
					playerguilddata->set_bbonus(guilduser->bbonus());
					playerguilddata->set_curcontribution(guilduser->curcontribution());
					playerguilddata->set_weekcontribution(guilduser->weekcontribution());
					playerguilddata->set_allcontribution(guilduser->allcontribution());
					playerguilddata->set_jointime(guilduser->jointime());
					playerguilddata->set_guildlevel(guilddb->guildlevel());
					playerguilddata->set_cashboxlevel(guilddb->cashboxlevel());
					playerguilddata->set_shoplevel(guilddb->shoplevel());
					playerguilddata->set_storagelevel(guilddb->storagelevel());
					playerguilddata->set_guildname(guilddb->name());
					player_->ExecuteMessage(syncdata);
				}
			}
		}
	}
	void GuildInfo::SendGuildUserList(MtPlayer* player)
	{
		if (player)
		{
			Packet::GetGuildUserListInfoReply reply;
			for (auto g_user : guilduserlist)
			{
				auto user = reply.add_userlist();
				user->CopyFrom(*g_user.get());
			}
			player->SendMessage(reply);
		}
	}
	void GuildInfo::AddUserContribution(uint64 playerid, int32 add)
	{
		auto user = GetGuildUser(playerid);
		if (user != nullptr)
		{
			auto contribution = user->curcontribution();
			contribution += add;
			if (contribution < 0)
			{
				contribution = 0;
			}
			user->set_curcontribution(contribution);
		}
	}
	Packet::ResultCode GuildInfo::AddGuild(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
			return Packet::ResultCode::UnknownError;

		//功能解锁校验
		if (player_->OnLuaFunCall_n(100, "CheckFunction", { { "gonghui", 999 } }) != Packet::ResultCode::ResultOK) {
			return Packet::ResultCode::InternalResult;
		}

		boost::shared_ptr<Packet::GuildUser> guilduser = boost::make_shared<Packet::GuildUser>();
		if (guilduser == nullptr)
			return Packet::ResultCode::UnknownError;
		if (IsFullGuildUser())
			return Packet::ResultCode::Guild_Full;
		uint64 guildid = guilddb->guid();
		std::string guildname = guilddb->name();
		InitGuildUser(player_.get(),guilduser, Packet::GuildPosition::TRAINEE,  guildid);
		UpdateGuildUser(player_.get(), guilduser);
		AddGuildUser(guilduser);

		Server::Instance().SendG2SCommonMessage(player_.get(), "G2SUpdateGuildInfo", {}, { (int64)guildid }, { guildname,"AddGuild" });
		Server::Instance().SendG2SCommonMessage(player_.get(), "G2SJoinGuildOk", {}, {}, {});
		G2SSyncGuildUserData(player_.get(), guilduser.get());

		Packet::GetGuildInfoReply reply;
		reply.mutable_info()->CopyFrom(*guilddb.get());
		reply.set_membersize(GetGuildUserSize());
		reply.set_maxmembersize(GetGuildUserMaxSize());
		reply.mutable_selfinfo()->CopyFrom(*guilduser.get());
		player_->SendMessage(reply);

		std::string notify = "jiaruxx|"+ player_->Name();
		send_guild_system_message(notify, guildid);
		AddRecord(notify);
		LOG_INFO << "AddGuild playerid="<< player_->Guid()<<" guildid="<< guildid;
		return Packet::ResultCode::ResultOK;
	}
	void GuildInfo::AddGuildUser(boost::shared_ptr<Packet::GuildUser> guilduser)
	{
		if (guilduser == nullptr)
			return;
		guilduserlist.push_back(guilduser);
	}
	Packet::ResultCode GuildInfo::ApplyGuild(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
			return Packet::ResultCode::UnknownError;
		if (player_->PlayerLevel() < guilddb->limitapplylevel())
			return Packet::ResultCode::LevelLimit;//
		if (IsFullGuildUser())
			return Packet::ResultCode::Guild_Full;
		for (auto apply : applyplayerlist)
		{
			if (apply.playerguid() == player_->Guid())
			{
				return Packet::ResultCode::Guild_InApplyList;
			}
		}
		Packet::ApplyGuildPlayerInfo playerinfo;
		playerinfo.set_playerguid(player_->Guid());
		playerinfo.set_playername(player_->GetScenePlayerInfo()->name());
		playerinfo.set_playerlevel(player_->PlayerLevel());
		playerinfo.set_profession(player_->MainActor()->Professions());
		applyplayerlist.push_back(playerinfo);
		for (int32 i=Packet::GuildPosition::FOUR_LEADER;i<= Packet::GuildPosition::CHIEFTAIN;i++)
		{
			auto guilduser = GetGuildUserByPosition(i);
			if (guilduser)
			{
				auto guilduserplayer = Server::Instance().FindPlayer(guilduser->playerguid());
				if (guilduserplayer)
				{
					SendApplyList(guilduserplayer.get());
				}
			}
		}
		
		return Packet::ResultCode::ResultOK;
	}
	int32 GuildInfo::AddGuildMoney_S(int32 money)
	{
		auto config = MtGuildManager::Instance().GetBuildLevelConfig(1, GetBuildLevel(1));
		int32 maxguildmony = config->param(0);
		int32 oldmoney = guilddb->guildmoney();
		oldmoney += money;
		if (oldmoney > maxguildmony)
		{
			oldmoney = maxguildmony;
		}
		else if (oldmoney < 0)
		{
			oldmoney = 0;
		}
		guilddb->set_guildmoney(oldmoney);
		return oldmoney;
	}
	void GuildInfo::AddGuildMoney(int32 money, MtPlayer* player)
	{
		int32 newmoney =  AddGuildMoney_S(money);
		if (player != nullptr)
		{
			Packet::CommonReply reply;
			reply.set_reply_name("Token_GuildMoney");
			reply.add_int64_params(newmoney);
			player->SendMessage(reply);
		}
	}

	void GuildInfo::AddGuildWeekBonus(int32 money)
	{
		int32 oldmoney = guilddb->weekbonus();
		oldmoney += money;
		guilddb->set_weekbonus(oldmoney);
	}
	int32 GuildInfo::GetGuildMoney()
	{
		return guilddb->guildmoney();
	}
	int32 GuildInfo::GetGuildMaintain()
	{
		return 120000*(2*GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN)+
			GetBuildLevel(Packet::GuildBuildType::GUILD_CASHBOX)+ GetBuildLevel(Packet::GuildBuildType::GUILD_SHOP)+ GetBuildLevel(Packet::GuildBuildType::GUILD_STORAGE));
	}
	void GuildInfo::AddCDKCount(boost::shared_ptr<MtPlayer> player_, boost::shared_ptr<Packet::GuildCDKInfo> cdk, int32 addcount)
	{
		if (player_ == nullptr || cdk == nullptr)
			return;
		int32 count=cdk->count();
		count += addcount;
		cdk->set_count(count);

	}
	void GuildInfo::SendApplyList(MtPlayer* player_)
	{
		if (player_ == nullptr)
			return;
		Packet::ApplyGuildPlayerListReply reply;
		for (auto applyplayer : applyplayerlist)
		{
			auto apply = reply.add_list();
			apply->CopyFrom(applyplayer);
		}
		player_->SendMessage(reply);
	}
	void GuildInfo::SendCDKList(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
			return;
		Packet::GetGuildCDKListReply reply;
		for (auto cdk : cdklist)
		{
			reply.add_cdks()->CopyFrom(*cdk.get());
		}
		player_->SendMessage(reply);
	}

	void GuildInfo::OnGuildWetCopyDamage(int32 copyid, guilduserdamage &userdamage)
	{
		auto iter = guildwetcopyranklist_.find(copyid);
		if (iter != guildwetcopyranklist_.end())
		{
			iter->second.nosortinsert(userdamage);
		}
		else
		{
			ranklist_top<guilduserdamage, 10> list;
			list.nosortinsert(userdamage);
			guildwetcopyranklist_.insert(std::make_pair(copyid, list));
		}
		
	}
	int32 GuildInfo::GetWetCopyTimes(uint64 playerid, int32 copyid)
	{
		auto user = GetGuildUser(playerid);
		if (user == nullptr)
			return 0;
		
		int32 times = (user->wetcopytimes() >> (copyid*2)) & 0x0003;
		return times;
	}
	void GuildInfo::AddWetCopytimes(MtPlayer* player_, int32 copyid)
	{
		if (player_ == nullptr)
			return;
		uint64 playerid = player_->Guid();
		auto user = GetGuildUser(playerid);
		if (user == nullptr)
			return ;
		int32 times = GetWetCopyTimes(playerid, copyid);
		times++;
		times = user->wetcopytimes() | (times << (copyid * 2));
		user->set_wetcopytimes(times);
		player_->SendCommonReply("UpdateWetCopytimes", { times }, {}, {});
	}
	void GuildInfo::OnWeekBonus()
	{
		int32 lastbonus = guilddb->weekbonus();
		int32 sumbonus = 0;
		for (auto user : guilduserlist)
		{
			auto config = MtGuildManager::Instance().GetGuildGuildPositionConfig(user->position());
			if (config == nullptr)
			{
				continue;
			}
			sumbonus += config->bonus()*user->weekcontribution();
		}
		if (sumbonus == 0)
			return;
		for (auto& user : guilduserlist)
		{
			auto config = MtGuildManager::Instance().GetGuildGuildPositionConfig(user->position());
			if (config == nullptr)
			{
				continue;
			}
			float proportion = ((float)(config->bonus()*user->weekcontribution()) / (float)sumbonus);
			int32 bonus = (int32)(lastbonus*proportion);
			user->set_bbonus(bonus);
			user->set_weekcontribution(0);
			auto player_ = Server::Instance().FindPlayer(user->playerguid());
			if (player_)
			{
				player_->SendCommonReply("UpdateGuildBonus", { bonus }, {}, {});
			}
		}
		guilddb->set_weekbonus(0);
	}
	void GuildInfo::OnHourTriger(int32 hour)
	{
		switch (hour)
		{
		case 5:
			applyplayerlist.clear();
			break;
		default:
			break;
		}
	}
	void GuildInfo::OnDayTriger(int32 day)
	{
		OnMainTain();
		guildranklist_.cleardata();
		for (auto user : guilduserlist)
		{
			user->set_spoiljumptimes(0);
			user->set_wetcopytimes(0);
			user->set_bdonation(0);
			user->set_bossdamage(0);
		}
		day;
	}
	void GuildInfo::OnMainTain()
	{
		int32 maintain = GetGuildMaintain();
		int32 nowmoney = GetGuildMoney();
		AddGuildMoney(-maintain,nullptr);
		if (maintain > nowmoney)
		{
			int32 lowmaintainday = guilddb->lowmaintainday();

			if (lowmaintainday == 0) //第一天
			{
				int32 nowday = MtTimerManager::Instance().CurrentDayDate();
				guilddb->set_lowmaintainday(nowday);
				//发邮件
				static_message_manager::Instance().create_message(guilddb->chieftain(), nullptr, MAIL_OPT_TYPE::GUILD_ADD, "gonghuijinggao", "zhuyigonghuiziji");
			}
			else//第二天
			{
				int32 guildlevel = GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN);
				if (guildlevel == 1)
				{
					tickdelete = true;
				}
				else
				{
					guildlevel--;
					guilddb->set_guildlevel(guildlevel);
				}
			}
		}
		else
		{
			guilddb->set_lowmaintainday(0);
		}
	}

	void GuildInfo::BroadcastUser(const google::protobuf::Message& msg)
	{
		for (auto user : guilduserlist)
		{
			auto player = Server::Instance().FindPlayer(user->playerguid());
			if (player )
			{
				player->SendMessage(msg);
			}
		}
	}
	void GuildInfo::BroadG2SCommonMessage(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues)
	{
		auto message = boost::make_shared<G2S::G2SCommonMessage>();
		message->set_request_name(name);
		for (auto child : int32values) {
			message->add_int32_params(child);
		}
		for (auto child : int64values) {
			message->add_int64_params(child);
		}
		for (auto child : stringvalues) {
			message->add_string_params(child);
		}
		for (auto user : guilduserlist)
		{
			auto player = Server::Instance().FindPlayer(user->playerguid());
			if (player)
			{
				player->ExecuteMessage(message);
			}
		}
	}
	zxero::uint64 GuildInfo::ChieftainGuid() const
	{
		if (guilddb == nullptr)
			return INVALID_GUID;
		return guilddb->chieftain();
	}


	Packet::GuildDBInfo* GuildInfo::GetDBInfo() const
	{
		return guilddb.get();
	}

	boost::shared_ptr<Packet::GuildCDKInfo> GuildInfo::GetGuildCDKInfoByPlayer(uint64 playerguid, int32 configid)
	{
		auto it = std::find_if(cdklist.begin(), cdklist.end(), [=](auto& iter)
		{
			return (iter->playerid() == playerguid && iter->configid() == configid);
		});
		if (it == cdklist.end())
		{
			return nullptr;
		}
		return *it;
	}
	boost::shared_ptr<Packet::GuildCDKInfo> GuildInfo::GetGuildCDKInfoById(uint64 cdkguid)
	{
		auto it = std::find_if(cdklist.begin(), cdklist.end(), [=](auto& iter)
		{
			return (iter->guid() == cdkguid);
		});
		if (it == cdklist.end())
		{
			return nullptr;
		}
		return *it;
	}
	void GuildInfo::DeleteGuildCDKInfoById(uint64 cdkguid)
	{
		for (auto iter = cdklist.begin(); iter != cdklist.begin();)
		{
			if ((*iter)->guid() == cdkguid)
			{
				iter = cdklist.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
	bool GuildInfo::HaveGetCDKInfo(boost::shared_ptr<Packet::GuildCDKInfo> cdkinfo,uint64 playerguid)
	{
		if (cdkinfo == nullptr)
			return true;
		
		for (auto  playerid  : cdkinfo->getplayerid())
		{
			if (playerid == playerguid)
			{
				return true;
			}
		}
		return false;
	}
	uint32 GuildInfo::GetBuildLevelCDTime(int32 buildtype)
	{
		switch (buildtype)
		{
		case Packet::GuildBuildType::GUILD_MAIN:
			return guilddb->guildcd();
		case Packet::GuildBuildType::GUILD_CASHBOX:
			return guilddb->cashboxcd();
		case Packet::GuildBuildType::GUILD_SHOP:
			return guilddb->shopcd();
		case Packet::GuildBuildType::GUILD_STORAGE:
			return guilddb->storagecd();
		default:
			break;
		}
		return 0;
	}
	int32 GuildInfo::GetBuildLevel(int32 buildtype) const
	{
		switch (buildtype)
		{
		case Packet::GuildBuildType::GUILD_MAIN:
			return guilddb->guildlevel();
		case Packet::GuildBuildType::GUILD_CASHBOX:
			return guilddb->cashboxlevel();
		case Packet::GuildBuildType::GUILD_SHOP:
			return guilddb->shoplevel();
		case Packet::GuildBuildType::GUILD_STORAGE:
			return guilddb->storagelevel();
		default:
			break ;
		}
		return -1;
	}
	void GuildInfo::SettBuildLevel(int32 buildtype, int32 buildlevel, uint32 cdtime)
	{
		switch (buildtype)
		{
		case Packet::GuildBuildType::GUILD_MAIN:
		{
			guilddb->set_guildcd(cdtime);
			guilddb->set_guildlevel(buildlevel);
		}
		break;
		case Packet::GuildBuildType::GUILD_CASHBOX:
		{
			guilddb->set_cashboxcd(cdtime);
			guilddb->set_cashboxlevel(buildlevel);
		}
		break;
		case Packet::GuildBuildType::GUILD_SHOP:
		{
			guilddb->set_shopcd(cdtime);
			guilddb->set_shoplevel(buildlevel);
		}
		break;
		case Packet::GuildBuildType::GUILD_STORAGE:
		{
			guilddb->set_storagecd(cdtime);
			guilddb->set_storagelevel(buildlevel);
		}
		break;
		default:
			break;
		}
		//广播G2s消息
		auto message = boost::make_shared<G2S::G2SCommonMessage>();
		message->set_request_name("G2SGuildBuildLevelUp");
		message->add_int32_params(buildtype);
		message->add_int32_params(buildlevel);
		for (auto user : guilduserlist) {
			auto player = Server::Instance().FindPlayer(user->playerguid());
			if (player) {
				player->ExecuteMessage(message);
			}
		}
	}

	boost::shared_ptr<Packet::GuildWetCopyInfo> GuildInfo::GetGuildWetCopyInfo(int32 copyid)
	{
		for (auto iter : wetcopyinfo_)
		{
			if (iter->copyid() == copyid)
			{
				return iter;
			}
		}
		
		//auto it = std::find_if(wetcopyinfo_.begin(), wetcopyinfo_.end(), [=](auto& iter)
		//{
		//	return (iter->copyid() == copyid);
		//});
		return nullptr;
	}
	void GuildInfo::SetGuildWetCDTime(int32 copyid, int32 cdtime)
	{
		for (auto& cd : wetcopycdtime_)
		{
			if (cd.first == copyid)
			{
				cd.second = cdtime;
				return;
			}
		}
		wetcopycdtime_.insert({ copyid,cdtime });
	}
	boost::shared_ptr<Packet::GuildWarehouse> GuildInfo::GetWarehouse(int32 index)
	{
		for (auto iter : warehouse_)
		{
			if (iter->index() == index)
			{
				return iter;
			}
		}
		return nullptr;
	}
	void GuildInfo::AddWarehouse(int32 itemid)
	{
		int32 size = warehouse_.size();
		if (size >= 25)
		{
			LOG_ERROR << "GuildInfo more Warehouse"<< itemid;
			return;
		}
		boost::shared_ptr<Packet::GuildWarehouse> wh = boost::make_shared<Packet::GuildWarehouse>();
		if (wh == nullptr)
			return;
		warehouseindex_++;
		wh->set_guildid(GetGuildId());
		wh->set_index(warehouseindex_);
		wh->set_itemid(itemid);
		wh->set_receiveplayerid(INVALID_GUID);
		warehouse_.push_back(wh);
		LOG_INFO << "GuildInfo add Warehouse = " << itemid;
	}
	void GuildInfo::DelWarehouse(int32 index)
	{
		for (auto iter = warehouse_.begin();iter !=warehouse_.end();)
		{
			if ((*iter)->index() == index || (*iter)->itemid() == INVALID_INT32)
			{
				(*iter)->SetDeleted();
				MtGuildManager::Instance().Cache2Save((*iter).get(), "Warehouse");
				iter = warehouse_.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
	void GuildInfo::AddWarehouseRecord(int32 itemid, std::string playername)
	{
		boost::shared_ptr<Packet::GuildWarehouseRecord> record = boost::make_shared<Packet::GuildWarehouseRecord>();
		if (record == nullptr)
		{
			return;
		}
		
		record->set_guildid(GetGuildId());
		record->set_id(warehouse_record_.size());
		record->set_date(MtTimerManager::Instance().CurrentDate());
		record->set_player_name(playername);
		record->set_itemid(itemid);
		warehouse_record_.push_back(record);
	}

	std::vector<boost::shared_ptr<Packet::GuildSpoilItem>> GuildInfo::GetSpoilItem()
	{
		return guildspoil_item_;
	}

	void GuildInfo::AddSpoilItem(int32 itemid, int32 copyid, MtPlayer* player)
	{
		boost::shared_ptr<Packet::GuildSpoilItem> item = boost::make_shared<Packet::GuildSpoilItem>();
		if (item == nullptr)
		{
			return;
		}
		guildspoilindex_++;
		auto guildid = GetGuildId();
		item->set_guildid(GetGuildId());
		item->set_index(guildspoilindex_);
		item->set_copyid(copyid);
		item->set_itemid(itemid);
		item->set_timecd(3600);
		for (int32 i=0;i<MtGuildManager::Instance().WetCopySpoilApplySize;i++)
		{
			item->add_playerid(INVALID_GUID);
		}
		guildspoil_item_.push_back(item);
		
		std::string str = (boost::format("addzhanlipin|%1%") % MtItemManager::Instance().GetItemName(itemid)).str();
		send_guild_system_message(str, guildid);
		if (player != nullptr)
		{
			player->SendClientNotify(str,-1,-1);
		}
		LOG_INFO << "GuildInfo::AddSpoilItem ok,"<< guildid<<","<< guildspoil_item_.size() << "," << copyid << "," << itemid;

	}
	boost::shared_ptr<Packet::GuildSpoilItem> GuildInfo::GetSpoilItemByIndex(int32 index)
	{
		for (auto item : guildspoil_item_)
		{
			if (item->index() == index)
			{
				return item;
			}
		}
		return nullptr;
	}
	void GuildInfo::SendSpoilItem(MtPlayer* player_, int32 copyid)
	{
		if (player_)
		{
			Packet::GuildSpoilItemReply reply;
			for (auto item : guildspoil_item_)
			{
				if (item->copyid() == copyid)
				{
					auto msgitem = reply.add_items();
					msgitem->set_index(item->index());
					msgitem->set_copyid(item->copyid());
					msgitem->set_itemid(item->itemid());
					msgitem->set_timecd(item->timecd());
					for ( int32 i=0;i< item->playerid_size();i++)
					{
						auto playerid = item->playerid(i);
						if (playerid != INVALID_GUID)
						{
							auto playeruser = GetGuildUser(playerid);
							if (playeruser)
							{
								auto msgap = msgitem->add_applyplayer();
								msgap->set_playerid(playerid);
								msgap->set_playername(playeruser->playername());
								msgap->set_hair(playeruser->hair());
							}
						}
					}
				}
			}
			player_->SendMessage(reply);
		}
	}

	void GuildInfo::SpoilItemJumpPlayer(MtPlayer* player_, int32 index)
	{
		if (player_)
		{
			uint64 playerid = player_->Guid();
			boost::shared_ptr<Packet::GuildSpoilItem> spoilitem = GetSpoilItemByIndex(index);
			if (spoilitem)
			{
				if (spoilitem->playerid_size() <= 1)
				{
					return;
				}
				auto playeriter = spoilitem->mutable_playerid();
				for (int32 i = (spoilitem->playerid_size() - 1);  i > 0; i--)
				{
					uint64 iterid = spoilitem->playerid(i);
					if (iterid == playerid)
					{
						//uint64 iterid_ = spoilitem->playerid(i - 1);
						playeriter->SwapElements(i, i - 1);
					}
				}
			}
		}
	}
	void GuildInfo::SendUpdateSpoilItem(MtPlayer* player_, int32 index)
	{
		if (player_)
		{
			Packet::UpdateGuildSpoilItemClient reply;
			for (auto item : guildspoil_item_)
			{
				if (item->index() == index)
				{
					reply.set_index(index);
					for (int32 i = 0; i < item->playerid_size(); i++)
					{
						auto playerid = item->playerid(i);
						auto playeruser = GetGuildUser(playerid);
						if (playeruser)
						{
							auto msgap = reply.add_applyplayer();
							msgap->set_playerid(playerid);
							msgap->set_playername(playeruser->playername());
							msgap->set_hair(playeruser->hair());
						}
					}
				}
			}
			player_->SendMessage(reply);
		}
	}

	void GuildInfo::AddRecord(std::string record)
	{
		boost::shared_ptr<Packet::GuildRecord> r = boost::make_shared<Packet::GuildRecord>();
		if (r)
		{
			r->set_guildid(GetGuildId());
			r->set_index(guildrecord_.size());
			r->set_record(record);
			r->set_date(MtTimerManager::Instance().CurrentDate());
			guildrecord_.push_back(r);
		}
	}

	void GuildInfo::SendRecord(MtPlayer* player_)
	{
		if (player_)
		{
			Packet::GuildRecordReply reply;
			for (auto record : guildrecord_)
			{
				auto rc = reply.add_records();
				rc->CopyFrom(*record.get());
			}
			player_->SendMessage(reply);
		}
	}

	boost::shared_ptr<Packet::GuildImpeach> GuildInfo::GetGuildImpeach()
	{
		return impeadch_;
	}
	void GuildInfo::SendGuildImpeach(MtPlayer* player_)
	{
		if (player_ && impeadch_)
		{
			Packet::GuildImpeachReply reply;
			reply.mutable_impeach()->CopyFrom(*impeadch_.get());
			player_->SendMessage(reply);
		}
	}

	void GuildInfo::ResetGuildImpeach()
	{
		if (impeadch_)
		{
			impeadch_->set_impeachguild(INVALID_GUID);
			impeadch_->set_impeachname("");
			impeadch_->set_needcount(0);
			impeadch_->set_endtime(0);
			impeadch_->clear_respondguild();
		}
	}
	void GuildInfo::OnGuilDBossReward(int32 rankfix, int32 monsterid)
	{
		guildranklist_.greater_sort();
		auto fixlevel = MtGuildManager::Instance().GetGuildLevelFixConfig(GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN));
		if (fixlevel == nullptr)
			return;
		int32 playercount = guildranklist_.realsize();
		AddGuildMoney(fixlevel->battle()*playercount*rankfix / 10, nullptr);
		int32 i = 0;
		for (auto playerinfo : guildranklist_.m_data)
		{
			uint64 playerguid = playerinfo.data->playerid_();
			int32 valuedamage = playerinfo.data->damagevalue_()*fixlevel->playergold()/100;
			google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
			auto item_ = items.Add();
			item_->set_itemid(Packet::TokenType::Token_Gold);
			item_->set_itemcount(valuedamage);
			int32 itemcount = 0;
			int32 guildcon = 0;
			if (i == 0) 
			{
				itemcount = 5;
				guildcon = 400;
			}
			else if (i == 1)
			{
				itemcount = 3;
				guildcon = 300;
			}
			else if (i == 2)
			{
				itemcount = 2;
				guildcon = 200;
			}
			else if (i >=3 && i < 10)
			{
				itemcount = 1;
				guildcon = 100;
			}
			else
			{
				itemcount = 1;
				guildcon = 50;
			}
			item_ = items.Add();
			item_->set_itemid(20234025);
			item_->set_itemcount(itemcount);
			item_ = items.Add();
			item_->set_itemid(Packet::TokenType::Token_GuildContribute);
			item_->set_itemcount(guildcon);

			std::stringstream notify;
			notify << "union_boss_self_end|" <<"monster_name_" << monsterid << "|" << (i+1) ;
			boost::shared_ptr<Packet::MessageInfo> mail_ = static_message_manager::Instance().create_message(playerguid, nullptr,  MAIL_OPT_TYPE::GUILD_ADD, "wb_mail_title", notify.str(), &items);
			i++;
		}
	}

	bool GuildInfo::CanenterBattle()
	{
		auto data = MtConfigDataManager::Instance().FindConfigValue("guild_battle_enter");
		if (data == nullptr)
			return false;
		return (GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN) >= data->value1() &&
			GetGuildBattleUserSize() >= data->value2());
	}
	
	float GuildInfo::GetBattleWinRate() const
	{
		int32 m = guilddb->battletimes();
		int32 n = guilddb->battlewintimes();
		if (m == 0 || n == 0)
			return 0;
		return (float)n / (float)m;
	}
	float GuildInfo::GetBattleRoundWinRate() const
	{
		int32 m = guilddb->battleroundtimes();
		int32 n = guilddb->battleroundwintimes();
		if (m == 0 || n == 0)
			return 0;
		return (float)n / (float)m;
	}

	int32 MtGuildManager::GetGuildBossStatus()
	{
		return bossstatus_;
	}
	void MtGuildManager::OnStartGuildBoss()
	{
		bossstatus_ = 1;
	}
	void MtGuildManager::OnEndGuildBoss()
	{
		if (bossstatus_ != 0)
		{
			int32 monsterid = 0;
			auto data = MtConfigDataManager::Instance().FindConfigValue("guild_boss_config");
			if (data != nullptr)
			{
				monsterid = data->value1();
			}

			bossstatus_ = 0;
			serverranklist_.greater_sort();
			int32 i = 1;
			for (auto gr : serverranklist_.m_data)
			{
				auto guildid = gr.data->guildid_();
				auto guild = GetGuildByid(guildid);
				if (guild)
				{
					int32 rankfix = 10;
					if (i == 1)
					{
						rankfix = 20;
					}
					else if (i == 2)
					{
						rankfix = 16;
					}
					else if (i == 3)
					{
						rankfix = 14;
					}
					else if (i >= 4 && i <= 10)
					{
						rankfix = 12;
					}
					guild->OnGuilDBossReward(rankfix, monsterid);
					std::stringstream notify;
					notify << "union_boss_end" << "|" << guild->GetGuildName() << "|" << "monster_name_" << monsterid << "|" << (i);
					if (i < 3)
					{
						send_run_massage(notify.str());
					}
					i++;
				}
			}
		}
	}
	void MtGuildManager::EndGuildWetCopy(MtPlayer* player_, GuildInfo* guild, int32 copyid, int32 damage)
	{
		if (player_ == nullptr || guild == nullptr)
			return;
		guilduserdamage userdamage;
		FillGuildUserDamage(userdamage, player_, guild, copyid, damage, Packet::ActorFigthFormation::AFF_PVE_GUILD_COPY);
		serverwetcopyranklist_.nosortinsert(userdamage);
		guild->OnGuildWetCopyDamage(copyid,userdamage);
	}
	void MtGuildManager::testplayers(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
		{
			return;
		}
		LOG_INFO << "ok";
	}
	void MtGuildManager::testplayer(MtPlayer* player_)
	{
		if (player_ == nullptr)
		{
			return;
		}
		LOG_INFO << "ok";
	}

	void MtGuildManager::EndGuildBoss(uint64 guildid, uint64 playerid, int32 damage)
	{
		auto player_ = Server::Instance().FindPlayer(playerid);
		if(player_ == nullptr)
			return;
		auto guild = GetGuildByid(guildid);
		if (guild == nullptr)
			return;
		auto guilduser = guild->GetGuildUser(playerid);
		if (guilduser == nullptr)
			return;
		int32 userdamagevalue = (guilduser->bossdamage() + damage);
		guilduser->set_bossdamage(userdamagevalue);
		guild->bossalldamage += damage;

		guildbossuserdamage userdamage;
		userdamage.data->set_playerid_(player_->Guid());
		userdamage.data->set_playerhair_(player_->GetScenePlayerInfo()->hair());
		userdamage.data->set_playername_(player_->GetScenePlayerInfo()->name());
		userdamage.data->set_damagevalue_(userdamagevalue);
		guild->guildranklist_.nosortinsert(userdamage);

		guildalldamage guilddamage;
		guilddamage.data->set_damagevalue_(guild->bossalldamage);
		guilddamage.data->set_guildid_(guild->GetGuildId());
		guilddamage.data->set_guildname_(guild->GetGuildName());
		serverranklist_.nosortinsert(guilddamage);
	}

	void GuildBattle::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<GuildBattle, ctor()>(ls, "GuildBattle")
			.def(&GuildBattle::GetBattleInfo, "GetBattleInfo")
			.def(&GuildBattle::GetPlayerinfo, "GetPlayerinfo")
			.def(&GuildBattle::CreateGuildBattleRaid, "CreateGuildBattleRaid")
			.def(&GuildBattle::GetGuildBattleRaid, "GetGuildBattleRaid")
			.def(&GuildBattle::GetGuildBattleRaidID, "GetGuildBattleRaidID")
			.def(&GuildBattle::UpdatePlayerList,"UpdatePlayerList")
			.def(&GuildBattle::UpdatePlayerByGuid,"UpdatePlayerByGuid")
			.def(&GuildBattle::SendGuildBattleInfo,"SendGuildBattleInfo")
			.def(&GuildBattle::EnterPlayer,"EnterPlayer")
			.def(&GuildBattle::LeavePlayer,"LeavePlayer")
			.def(&GuildBattle::GetGuildBattlePlayer,"GetGuildBattlePlayer")
			.def(&GuildBattle::GetAttackerScore,"GetAttackerScore")
			.def(&GuildBattle::GetDefenderScore, "GetDefenderScore")
			.def(&GuildBattle::GetAttackerCount, "GetAttackerCount")
			.def(&GuildBattle::GetDefenderCount, "GetDefenderCount")
			.def(&GuildBattle::GetAttackerGuid, "GetAttackerGuid")
			.def(&GuildBattle::GetDefenderGuid, "GetDefenderGuid")
			.def(&GuildBattle::SortPlayerRank,"SortPlayerRank")
			.def(&GuildBattle::GetWinGuild,"GetWinGuild")
			.def(&GuildBattle::PlayerOnGuildWarEnd, "PlayerOnGuildWarEnd")
			.def(&GuildBattle::AddLeavePlayerExp, "AddLeavePlayerExp")
			.def(&GuildBattle::OnSecondTick, "OnSecondTick")
			;
	}

	GuildBattle::GuildBattle() 
	{
		dbinfo = boost::make_shared<Packet::GuildBattleInfo>();
		dbinfo->set_a_guildid(INVALID_GUID);
		dbinfo->set_b_guildid(INVALID_GUID);
		dbinfo->set_a_guildname("");
		dbinfo->set_b_guildname("");
		dbinfo->set_win_guildid(INVALID_GUID);
	};
	void GuildBattle::OnCacheAll()
	{
		std::set<google::protobuf::Message *> msg_set;
		msg_set.insert(dbinfo.get());
		MtShmManager::Instance().Cache2Shm(msg_set, "GuildBattleInfo");
	}
	void GuildBattle::OnDeletedAll()
	{
		//标记和删除
		std::set<google::protobuf::Message *> msg_set;
		dbinfo->SetDeleted();
		msg_set.insert(dbinfo.get());
		MtShmManager::Instance().Cache2Shm(msg_set, "GuildBattleInfo");
	}
	boost::shared_ptr<Packet::GuildBattleInfo> GuildBattle::GetBattleInfo()
	{
		return dbinfo;
	}

	boost::shared_ptr<Packet::GuildBattlePlayerInfo> GuildBattle::GetPlayerinfo(uint64 playerid)
	{
		for (auto player : playerlist)
		{
			if (player->player_id() == playerid)
			{
				return player;
			}
		}
		return nullptr;
	}
	int32 GuildBattle::GetPlayerCountByid(uint64 guildid)
	{
		int32 count = 0;
		for (auto player : playerlist)
		{
			if (player->guild_id() == guildid)
			{
				count++;
			}
		}
		return count;
	}
	int32 GuildBattle::CreateGuildBattleRaid(int32 sceneid,int32 script_id,int32 npcid, int32 oldsceneid)
	{
		auto npc = MtMonsterManager::Instance().FindStaticNpc(npcid);
		if (npc == nullptr)
			return 0;
		auto pos_config = MtMonsterManager::Instance().FindScenePos(npc->pos_id());
		if (pos_config == nullptr)
			return 0;
		RaidHelper helper;
		helper.scene_id_ = sceneid;
		helper.script_id_ = script_id;
		helper.portal_pos_ = pos_config->position();
		helper.portal_scene_id_ = oldsceneid;

		auto scene = MtSceneManager::Instance().GetSceneById(helper.scene_id_);
		if (scene == nullptr)
			return 0;
		int32 rtid = scene->AddRaid(helper);
		if (rtid == INVALID_GUID)
			return 0;
		raid_ = scene->GetRaid(rtid);
		return rtid;
	}
	int32 GuildBattle::GetGuildBattleRaidID(int32 sceneid)
	{
		if (raid_ == nullptr)
		{
			return 0;
		}
		auto scene = MtSceneManager::Instance().GetSceneById(sceneid);
		if (scene == nullptr)
			return 0;
		auto raid = scene->GetRaid(raid_->RuntimeId());
		if (raid == nullptr)
			return 0;
		return raid_->RuntimeId();
	}
	const boost::shared_ptr<MtRaid> GuildBattle::GetGuildBattleRaid()
	{
		return raid_;
	}
	int32 GuildBattle::GetAttackerScore()
	{
		int32 score = 0;
		for (auto player : playerlist)
		{
			if (player->guild_id() == dbinfo->a_guildid())
			{
				score += player->score();
			}
		}
		return score;
	}
	int32 GuildBattle::GetDefenderScore()
	{
		int32 score = 0;
		for (auto player : playerlist)
		{
			if (player->guild_id() == dbinfo->b_guildid())
			{
				score += player->score();
			}
		}
		return score;
	}
	int32 GuildBattle::GetAttackerCount()
	{
		int32 count = 0;
		for (auto player : playerlist)
		{
			if (player->guild_id() == dbinfo->a_guildid())
			{
				count++;
			}
		}
		return count;
	}
	int32 GuildBattle::GetDefenderCount()
	{
		int32 count = 0;
		for (auto player : playerlist)
		{
			if (player->guild_id() == dbinfo->b_guildid())
			{
				count++;
			}
		}
		return count;
	}
	uint64 GuildBattle::GetAttackerGuid()
	{
		if (dbinfo == nullptr)
			return INVALID_GUID;
		return dbinfo->a_guildid();
	}
	uint64 GuildBattle::GetDefenderGuid()
	{
		if (dbinfo == nullptr)
			return INVALID_GUID;
		return dbinfo->b_guildid();
	}
	uint64 GuildBattle::GetWinGuild()
	{
		if (dbinfo == nullptr)
			return INVALID_GUID;
		return dbinfo->win_guildid();
	}
	void GuildBattle::OnWin(int32 camp)
	{
		if (raid_ == nullptr)
		{
			return;
		}

		raid_->SetRaidTimer(999, 10 * 60, -1);
		uint64 winguildid = INVALID_GUID;
		uint64 lostguildid = INVALID_GUID;
		int32 winscore = 0;
		int32 lostscore = 0;
		int32 winplayercount = 0;
		int32 lostplayercount = 0;
		int32 winallplayercount = 0;
		int32 lostallplayercount = 0;
		switch (camp)
		{
		case 0:
		{

		}
		break;
		case 1:
		{
			for (auto attacker : raid_->GetAttackers())
			{
				Packet::GuildBattleResultReply reply;
				reply.set_win(true);
				reply.set_time(600);
				attacker->SendMessage(reply);
				AddLeavePlayerExp(attacker.get(), true);
				winplayercount++;
			}
			
			for (auto defender : raid_->GetDefenders())
			{
				Packet::GuildBattleResultReply reply;
				reply.set_win(false);
				reply.set_time(0);
				defender->SendMessage(reply);
				AddLeavePlayerExp(defender.get(), false);
				//defender->SetWaitToLeaveRaid(true);
				defender->GoingLeaveScene();
				lostplayercount++;
			}
			winguildid = dbinfo->a_guildid();
			lostguildid = dbinfo->b_guildid();
			winscore = GetAttackerScore();
			lostscore = GetDefenderScore();
			winallplayercount = GetAttackerCount();
			lostallplayercount = GetDefenderCount();
		}
		break;
		case 2:
		{
			for (auto attacker : raid_->GetAttackers())
			{
				Packet::GuildBattleResultReply reply;
				reply.set_win(false);
				reply.set_time(0);
				attacker->SendMessage(reply);
				attacker->GoingLeaveScene();
				lostplayercount++;
			}
			for (auto defender : raid_->GetDefenders())
			{
				Packet::GuildBattleResultReply reply;
				reply.set_win(true);
				reply.set_time(600);
				defender->SendMessage(reply);
				winplayercount++;
			}
			winguildid = dbinfo->b_guildid();
			lostguildid = dbinfo->a_guildid();
			lostscore = GetAttackerScore();
			winscore = GetDefenderScore();
			lostallplayercount = GetAttackerCount();
			winallplayercount = GetDefenderCount();
		}
		break;
		
		default:
			break;
		}
		
		Server::Instance().SendS2GCommonMessage("S2GGuildBattleWin", { 1,winscore,winplayercount,winallplayercount }, { (int64)winguildid }, {});
		Server::Instance().SendS2GCommonMessage("S2GGuildBattleWin", { 0,lostscore,lostplayercount,lostallplayercount }, { (int64)lostguildid }, {});
	}
	const boost::shared_ptr<Packet::GuildBattlePlayerInfo> GuildBattle::GetGuildBattlePlayer(uint64 playerid)
	{
		for (auto player : playerlist)
		{
			if (player->player_id() == playerid)
			{
				return player;
			}
		}
		return nullptr;
	}
	bool GuildBattle::EnterPlayer(uint64 playerid, std::string playername, int32 mobility,uint64 guildid, int32 initid)
	{
		boost::shared_ptr<Packet::GuildBattlePlayerInfo> player = GetPlayerinfo(playerid);
		if (player == nullptr)
		{
			player = boost::make_shared<Packet::GuildBattlePlayerInfo>();
			if (player == nullptr)
				return false;
			player->set_player_id(playerid);
			player->set_score(0);
			player->set_fighting(false);
			player->set_streakwin(0);
			player->set_mobility(mobility);
			player->set_protectcd(0);
			player->set_guild_id(guildid);
			player->set_player_name(playername);
			player->set_battlereward(0);
			player->set_init_actorid(initid);
			playerlist.push_back(player);
			Server::Instance().SendS2GCommonMessage("S2GAddGuildBattleTimes", {}, {(int64)guildid,(int64)playerid }, {});
			return true;
		}
		else
		{
			player->set_fighting(false);
		}
		return false;
	}
	void GuildBattle::LeavePlayer(uint64 playerid)
	{
		playerid;
	}
	void GuildBattle::PlayerOnGuildWarEnd(MtPlayer* player, bool win)
	{
		if (player == nullptr)
			return;
		if (win == true)
		{
			Packet::GuildBattleResultReply reply;
			reply.set_win(true);
			reply.set_time(600);
			player->SendMessage(reply);
			AddLeavePlayerExp(player, true);
		}
		else
		{
			Packet::GuildBattleResultReply reply;
			reply.set_win(false);
			reply.set_time(0);
			player->SendMessage(reply);
			AddLeavePlayerExp(player, false);
			player->GoingLeaveScene();
		}
	}
	void GuildBattle::AddLeavePlayerExp(MtPlayer* player,bool win)
	{
		if (player == nullptr)
			return;
		auto reward_config = MtActorConfig::Instance().GetLevelData(player->PlayerLevel());
		if (reward_config == nullptr)
			return;
		if (win)
		{
			int32 exp = reward_config->guildbattle_win_exp() + reward_config->guildbattle_min_exp()*(30 - MtTimerManager::Instance().GetMinute());
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Exp, exp, Config::ItemAddLogType::AddType_Guild,25);
		}
		else
		{
			int32 exp = reward_config->guildbattle_lost_exp() + reward_config->guildbattle_min_exp()*(30 - MtTimerManager::Instance().GetMinute());
			player->AddItemByIdWithNotify(Packet::TokenType::Token_Exp, exp, Config::ItemAddLogType::AddType_Guild,25);
		}
	}
	void GuildBattle::UpdatePlayerList()
	{
		Packet::GuildBattlePlayerListInfo listmsg;
		for (auto player : playerlist)
		{
			auto playermsg = listmsg.add_playerlist();
			playermsg->CopyFrom(*player.get());
		}
		BroadCastMessage(listmsg);
	}
	void GuildBattle::UpdatePlayerByGuid(uint64 playerid)
	{
		auto battleplayer = GetGuildBattlePlayer(playerid);
		if (battleplayer)
		{
			Packet::UpdateGuildBattlePlayerInfo info;
			info.mutable_playerinfo()->CopyFrom(*battleplayer.get());
			BroadCastMessage(info);
		}
	}
	void GuildBattle::SendGuildBattleInfo(MtPlayer* player)
	{
		if (player)
		{
			Packet::SelfGuildBattleInfo info;
			info.mutable_gbinfo()->CopyFrom(*dbinfo.get());
			player->SendMessage(info);
		}
	}
	void GuildBattle::BroadCastMessage(const google::protobuf::Message& msg)
	{
		if (raid_ != nullptr)
		{
			raid_->BroadCastMessage(msg);
		}
	}
	void GuildBattle::UpdateBattleStage(Packet::GuildBattle_Stage stage)
	{
		Packet::UpdateGuildBattleStage update;
		update.set_stage(stage);
		if (raid_ != nullptr)
		{
			raid_->BroadCastMessage(update);
		}
	}
	void GuildBattle::OnSecondTick()
	{
		for (auto player :  playerlist)
		{
			auto protectcd = player->protectcd();
			if (protectcd > 0)
			{
				protectcd--;
				player->set_protectcd(protectcd);
				if (protectcd <= 0)
				{
					Packet::UpdateGuildBattlePlayerInfo info;
					info.mutable_playerinfo()->CopyFrom(*player.get());
					BroadCastMessage(info);
				}
			}
			
		}
	}
	bool CampPlayerRank(const boost::shared_ptr<Packet::GuildBattlePlayerInfo>& r, const boost::shared_ptr<Packet::GuildBattlePlayerInfo>&l)
	{
		if (r->score() > l->score())
		{
			return true;
		}
		return false;
	}
	void GuildBattle::SortPlayerRank()
	{
		std::sort(playerlist.begin(), playerlist.end(), CampPlayerRank);
	}

	boost::shared_ptr<GuildBattle> MtGuildManager::GetGuildBattle(uint64 guild)
	{
		//lock
		boost::shared_lock<boost::shared_mutex> l(guildbattle_shared_mutex_);
		for (auto gb : guildbattlepairlist_)
		{
			if (gb->GetBattleInfo()->a_guildid() == guild || gb->GetBattleInfo()->b_guildid() == guild)
			{
				return gb;
			}
		}
		return nullptr;
	}
	void MtGuildManager::SetGuildBattleStage(int32 stage)
	{
		battle_stage_ = (Packet::GuildBattle_Stage)stage;
		//messsage to server 
		for (auto player : Server::Instance().OnlinePlayer())
		{
			if (player.second->CheckGuildBattleQualify())
			{
				player.second->SendGuildBattleStage(battle_stage_);
			}
		}
	}
	void MtGuildManager::InsertGuildBattleList(boost::shared_ptr<GuildInfo> a, boost::shared_ptr<GuildInfo> b)
	{
		boost::shared_ptr<GuildBattle> gb = boost::make_shared<GuildBattle>();
		if (a == nullptr || b == nullptr || gb == nullptr)
			return;
		a->guilddb->set_battlelefttimes(0);
		b->guilddb->set_battlelefttimes(0);
		gb->GetBattleInfo()->set_a_guildid(a->guilddb->guid());
		gb->GetBattleInfo()->set_b_guildid(b->guilddb->guid());
		gb->GetBattleInfo()->set_a_guildname(a->guilddb->name());
		gb->GetBattleInfo()->set_b_guildname(b->guilddb->name());
		gb->GetBattleInfo()->set_raidid(0);
		guildbattlepairlist_.push_back(gb);
	}
	bool compbattleguild(const boost::shared_ptr<GuildInfo>& r, const boost::shared_ptr<GuildInfo>& l)
	{
		float rwinrate = r->GetBattleWinRate();
		float lwinrate = l->GetBattleWinRate();
		if (rwinrate > lwinrate)
		{
			return true;
		}
		else if (rwinrate == lwinrate)
		{
			if (r->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN) > l->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN))
			{
				return true;
			}
			else if (r->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN) == l->GetBuildLevel(Packet::GuildBuildType::GUILD_MAIN))
			{
				if (r->GetGuildUserSize() > l->GetGuildUserSize())
				{
					return true;
				}
			}
		}
		return false;
	}
	void MtGuildManager::GenGuildBattleList()
	{
		boost::upgrade_lock<boost::shared_mutex> l(guildbattle_shared_mutex_);
		for (auto guildbattle : guildbattlepairlist_)
		{
			guildbattle->OnDeletedAll();
		}
		guildbattlepairlist_.clear();
		std::vector<boost::shared_ptr<GuildInfo>> battleguildlist;
		for (auto guild : guildlist_)
		{
			if (guild->CanenterBattle() == true)
			{
				battleguildlist.push_back(guild);
			}
		}
		std::sort(battleguildlist.begin(), battleguildlist.end(), compbattleguild);
		int32 allsize = battleguildlist.size();
		int32 gbtimes = Server::Instance().GetGuildBattleTimes();
		if (gbtimes > 0)
		{
			Server::Instance().SetGuildBattleTimes(++gbtimes);
			LOG_INFO << "GenGuildBattleList : "<< gbtimes;
		}
		if (allsize <= 1)
		{
			return;
		}
		if (gbtimes == 0)
		{
			if (MtTimerManager::Instance().GetWeekDay() == 1)
			{
				Server::Instance().SetGuildBattleTimes(1);
			}
			else//如果是周三就直接算2次
			{
				Server::Instance().SetGuildBattleTimes(2);
			}
		}
		int32 groupsize = allsize / 4;
		int32 leftsize = allsize % 4;
		if (leftsize == 1 || leftsize == 3)
		{
			if (battleguildlist[allsize-1]->guilddb->battlelefttimes()>=2)
			{
				boost::shared_ptr<GuildInfo> tmp = battleguildlist[allsize - 1];
				battleguildlist[allsize - 1] = battleguildlist[allsize - 2];
				battleguildlist[allsize - 2] = tmp;
			}
			int32 lefttimes = battleguildlist[allsize - 1]->guilddb->battlelefttimes()+1;
			battleguildlist[allsize - 1]->guilddb->set_battlelefttimes(lefttimes);
			if (leftsize == 3)
			{
				InsertGuildBattleList(battleguildlist[groupsize * 4], battleguildlist[groupsize * 4 + 1]);
			}
		}
		else if (leftsize == 2)
		{
			InsertGuildBattleList(battleguildlist[groupsize * 4], battleguildlist[groupsize * 4 + 1]);
		}

		for (int32 i=0;i<groupsize;i++)
		{
			int32 seed = rand_gen->intgen(1, 3);
			InsertGuildBattleList(battleguildlist[i * 4], battleguildlist[i * 4 + seed]);
			if (seed == 1)
			{
				InsertGuildBattleList(battleguildlist[i * 4 + 2], battleguildlist[i * 4 + 3]);
			}
			else if (seed == 2)
			{
				InsertGuildBattleList(battleguildlist[i * 4 + 1], battleguildlist[i * 4 + 3]);
			}
			else if (seed == 3)
			{
				InsertGuildBattleList(battleguildlist[i * 4 + 1], battleguildlist[i * 4 + 2]);
			}
			else
			{
				ZXERO_ASSERT(false);
			}
		}
	}
	bool MtGuildManager::GetGuildBattleList(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::GuildBattleListReq>& message,
		int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		boost::shared_lock<boost::shared_mutex> l(guildbattle_shared_mutex_);
		Packet::GuildBattleInfoList list;
		for (auto gb : guildbattlepairlist_)
		{
			list.add_gblist()->CopyFrom(*(gb->GetBattleInfo().get()));
		}
		list.set_battletimes(Server::Instance().GetGuildBattleTimes());
		player->SendMessage(list);
		return true;
	}
	bool compbattleroundguild(const boost::shared_ptr<GuildInfo>& r, const boost::shared_ptr<GuildInfo>& l)
	{
		int32 rwintimes = r->GetDBInfo()->battlewintimes();
		int32 lwintimes = l->GetDBInfo()->battlewintimes();
		if (rwintimes > lwintimes)
		{
			return true;
		}
		else if (rwintimes == lwintimes)
		{
			if (r->GetDBInfo()->battlescore() > l->GetDBInfo()->battlescore())
			{
				return true;
			}
		}
		return false;
	}
	void MtGuildManager::GuildBattleRoundReward()
	{
		std::vector<boost::shared_ptr<GuildInfo>> battleguildlist;
		for (auto guild : guildlist_)
		{
			battleguildlist.push_back(guild);
		}
		std::sort(battleguildlist.begin(), battleguildlist.end(), compbattleroundguild);
		int32 allsize = battleguildlist.size();
		LOG_INFO << "GuildBattleRoundReward all guild size=" << allsize;
		if (allsize <= 0)
		{
			return;
		}
		int32 ten = (int32)((float)allsize *0.1f + 0.5f);
		if (ten == 0)
		{
			ten = 1;
		}
		for (int32 i=0;i<ten;i++)
		{
			uint64 id = battleguildlist[i]->guilddb->guid();
			auto gb = GetGuildBattle(id);
			if (gb)
			{
				int32 count = gb->GetPlayerCountByid(id);
				if (count > 10)
				{
					battleguildlist[i]->AddWarehouse(20139006);
					battleguildlist[i]->AddWarehouse(20236001);
					LOG_INFO << "GuildBattleRoundReward ten ok id="<<id<<",num="<<i;
				}
				else
				{
					LOG_INFO << "GuildBattleRoundReward ten no count id=" << id << ",num=" << i;
				}
			}
			else
			{
				LOG_INFO << "GuildBattleRoundReward ten no gb id=" << id << ",num=" << i;
			}
		}
		int32 thirty = (int32)((float)allsize * 0.3f + 0.5f);
		for (int32 i = ten; i<thirty; i++)
		{
			uint64 id = battleguildlist[i]->guilddb->guid();
			auto gb = GetGuildBattle(id);
			if (gb)
			{
				int32 count = gb->GetPlayerCountByid(id);
				if (count > 10)
				{
					battleguildlist[i]->AddWarehouse(20139005);
					battleguildlist[i]->AddWarehouse(20139004);
					LOG_INFO << "GuildBattleRoundReward thirty ok id=" << id << ",num=" << i;
				}
				else
				{
					LOG_INFO << "GuildBattleRoundReward thirty no count id=" << id << ",num=" << i;
				}
			}
			else
			{
				LOG_INFO << "GuildBattleRoundReward thirty no gb id=" << id << ",num=" << i;
			}
		}
	}
	void MtGuildManager::ClearGuildBattleRoundScore()
	{
		for (auto guild : guildlist_)
		{
			guild->guilddb->set_battleroundtimes(0);
			guild->guilddb->set_battleroundwintimes(0);
		}
	}
	void MtGuildManager::ClearGuildBattleScore()
	{
		for (auto guild : guildlist_)
		{
			guild->guilddb->set_battletimes(0);
			guild->guilddb->set_battlewintimes(0);
			guild->guilddb->set_battlelefttimes(0);
		}
	}
	bool MtGuildManager::SendGuildBattleWeekRankList(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::GuildBattleWeekRankReq>& message,
		int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		Packet::GuildBattleWeekRankReply list;
		for (auto i : gbweekranklist_.m_data)
		{
			auto info = list.add_list();
			info->set_guildid(i.guildid_);
			info->set_guildname(i.guildname_);
			info->set_level(i.level_);
			info->set_score(i.score_);
		}
		player->SendMessage(list);
		return true;
	}
	bool MtGuildManager::SendGuildBattleAllRankList(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::GuildBattleAllRankReq>& message,
		int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		Packet::GuildBattleAllRankReply list;
		for (auto i : gballranklist_.m_data)
		{
			auto info = list.add_list();
			info->set_guildid(i.guildid_);
			info->set_guildname(i.guildname_);
			info->set_level(i.level_);
			info->set_battletimes(i.battletimes_);
			info->set_battlewintimes(i.battlewintimes_);
			if (i.battletimes_ == 0 || i.battlewintimes_ == 0)
			{
				info->set_winrate(0);
			}
			else
			{
				info->set_winrate((float)i.battlewintimes_/(float)(i.battletimes_));
			}
		}
		player->SendMessage(list);
		return true;
	}

	bool MtGuildManager::OnGetGuildWarehouseInfosReq(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::GuildWarehouseReq>& message,
		int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		OnGetGuildWarehouseInfos(player);
		return true;
	}
	bool MtGuildManager::OnGetGuildWarehouseRecordReq(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::GuildWarehouseRecordReq>& message,
		int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto guild = GetGuildByid(player->GetGuildID());
		if (guild == nullptr)
			return true;
		Packet::GuildWarehouseRecordReply reply;
		for (auto record : guild->warehouse_record_)
		{
			auto recordmsg = reply.add_record();
			recordmsg->CopyFrom(*record.get());
		}
		player->SendMessage(reply);
		return true;
	}
	bool MtGuildManager::SetGuildBattleWeekRank(uint64 guid, std::string guildname, int32 level,int32 score)
	{
		guildbattleweekrankinfo info;
		info.guildid_ = guid;
		info.guildname_ = guildname;
		info.level_ = level;
		info.score_ = score;
		gbweekranklist_.nosortinsert(info);
		return true;
	}
	bool MtGuildManager::SetGuildBattleAllRank(uint64 guid,std::string guildname,int32 level,
		int32 battlewintimes,int32  battletimes)
	{
		guildbattleallrankinfo info;
		info.guildid_ = guid;
		info.guildname_ = guildname;
		info.level_ = level;
		info.battlewintimes_ = battlewintimes;
		info.battletimes_ = battletimes;
		gballranklist_.nosortinsert(info);
		return true;
	}
	bool MtGuildManager::OnGuildBattleFightTarget(
		const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::GuildBattleFightTarget>& msg, int32 /*source*/)
	{
		if (player == nullptr )
			return true;
		boost::shared_ptr<GuildBattle> gb =  GetGuildBattle(player->GetGuildID());
		if (gb == nullptr)
			return true;
		auto selfplayerinfo = gb->GetPlayerinfo(player->Guid());
		if (selfplayerinfo == nullptr)
			return true;
		
		uint64 targetguid = msg->target_guid();
		auto targetplayerinfo = gb->GetPlayerinfo(targetguid);
		if (targetplayerinfo == nullptr)
			return true;

		//if (targetplayerinfo->fighting() == true)
		//{
		//	player->SendClientNotify("GuildBattle_Targetfight", -1, -1);
		//	return true;
		//}
		auto scene = player->Scene();
		if (scene == nullptr) {
			return true;
		}
		//int32 groupid = msg->battle_group_id();
		auto target_player = scene->FindPlayer(targetguid);
		if (target_player == nullptr) {
			return true;
		}
		Packet::BattleGroundType targetBattleType = target_player->GetCurBattleType();
		if (targetBattleType != Packet::BattleGroundType::NO_BATTLE
			&& targetBattleType != Packet::BattleGroundType::PVE_HOOK)
		{
			player->SendClientNotify("GuildBattle_Targetfight", -1, -1);
			return true;
		}
		auto battle = player->GetBattleManager()->CreateFrontBattle(player.get(), target_player.get(),
		{
			{ "script_id", 1023 },
			{ "group_id",msg->battle_group_id() }
		});
		////bool ret = player->OnGoGuildBattle(targetguild, groupid);
		//if (battle != nullptr)
		//{
		//	targetplayerinfo->set_fighting(true);
		//	selfplayerinfo->set_fighting(true);
		//	//扣除行动力

		//	Packet::UpdateGuildBattlePlayerInfo tarupdate;
		//	tarupdate.mutable_playerinfo()->CopyFrom(*targetplayerinfo.get());
		//	gb->BroadCastMessage(tarupdate);
		//	Packet::UpdateGuildBattlePlayerInfo selfupdate;
		//	selfupdate.mutable_playerinfo()->CopyFrom(*selfplayerinfo.get());
		//	gb->BroadCastMessage(selfupdate);
		//}
		return true;
	}

	bool MtGuildManager::OnGuildChatMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::GuildChat>& msg, int32 /*source*/)
	{
		if (msg == nullptr ) {
			return true;
		}
		player;
		BroadcastGuild(msg->guildid(), *msg->mutable_msg());

		return true;
	}

	bool MtGuildManager::OnS2GEndGuildBoss(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::EndGuildBoss>& msg, int32 /*source*/)
	{
		if (msg == nullptr || player == nullptr) {
			return true;
		}

		EndGuildBoss(msg->guild_id(), msg->guid(), msg->damage());
		return true;
	}
}