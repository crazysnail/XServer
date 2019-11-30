#include "module.h"
#include "WorldBoss.pb.h"
#include "MonsterConfig.pb.h"
#include "../data/data_manager.h"
#include "../base/mt_timer.h"
#include "../mt_server/mt_server.h"
#include "../mt_player/mt_player.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_config/mt_config.h"
#include "../mt_item/mt_item.h"
#include "../mail/static_message_manager.h"
#include "mt_worldboss_manager.h"
#include "active_model.h"

namespace Mt
{
	static MtWorldBossManager* __mt_worldboss_manager = nullptr;
	REGISTER_MODULE(MtWorldBossManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtWorldBossManager::OnLoad, boost::ref(MtWorldBossManager::Instance())));
	}

	MtWorldBossManager::MtWorldBossManager()
	{
		__mt_worldboss_manager = this;
	}

	MtWorldBossManager::~MtWorldBossManager()
	{

	}

	void MtWorldBossManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtWorldBossManager, ctor()>(ls, "MtWorldBossManager")
			.def(&MtWorldBossManager::WorldBossDead, "WorldBossDead")
			.def(&MtWorldBossManager::OnCreateBoss, "OnCreateBoss")
			.def(&MtWorldBossManager::OnEndBoss, "OnEndBoss")
			.def(&MtWorldBossManager::GetCurWorldBossInfo,"GetCurWorldBossInfo")
			.def(&MtWorldBossManager::GetCurWorldBossConfig, "GetCurWorldBossConfig")
			;
		ff::fflua_register_t<>(ls)
			.def(&MtWorldBossManager::Instance, "LuaMtWorldBossManager");

	}
	MtWorldBossManager& MtWorldBossManager::Instance()
	{
		return *__mt_worldboss_manager;
	}
	int32 MtWorldBossManager::OnLoad()
	{
		auto table = data_manager::instance()->get_table("world_boss");
		ZXERO_ASSERT(table) << "can not find table world_boss";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto boss_c = boost::make_shared <Packet::WorldBossConfig>();
			FillMessageByRow(*boss_c, *row);
			bossconfiglist_.push_back(boss_c);
		}
		return 0;
	}
	bool MtWorldBossManager::OnLoadAll(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		auto dbworldbosslist = ActiveModel::Base<Packet::WorldBossInfo>(dbctx).FindAll({});
		if (dbworldbosslist.empty())
		{
			for (auto& config_boss : bossconfiglist_) {
				boost::shared_ptr<Packet::WorldBossInfo> bossc = boost::make_shared<Packet::WorldBossInfo>();
				if (bossc != nullptr)
				{
					bossc->set_bossid(config_boss->day());
					bossc->set_bosslevel(1);
					bossc->set_hp(0);
					bossc->set_monstergourpid(config_boss->gourpid());
					bosslist_.push_back(bossc);
				}
			}
		}
		else
		{
			for (auto& db_boss : dbworldbosslist) {
				db_boss->set_hp(0);
				bosslist_.push_back(db_boss);
			}
		}
		
		return 0;
	}
	void MtWorldBossManager::OnSaveAll(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		for (auto boss : bosslist_)
		{
			ActiveModel::Base<Packet::WorldBossInfo>(dbctx).Save(boss);;
		}
	}

	void MtWorldBossManager::OnCacheAll( )
	{
		std::set<google::protobuf::Message *> msg_set;
		for (auto boss : bosslist_){
			msg_set.insert(boss.get());
		}
		MtShmManager::Instance().Cache2Shm(msg_set,"MtWorldBossManager");
	}
	const boost::shared_ptr<Packet::WorldBossInfo> MtWorldBossManager::GetCurWorldBossInfo()
	{
		int32 weekday = MtTimerManager::Instance().GetWeekDay();
		return GetWorldBossInfo(weekday);
	}

	boost::shared_ptr<Packet::WorldBossInfo> MtWorldBossManager::GetWorldBossInfo(int32 bossid)
	{
		for (auto iter : bosslist_)
		{
			if (iter->bossid() == bossid)
			{
				return iter;
			}
		}
		return nullptr;
	}
	void MtWorldBossManager::OnCreateBoss(int32 seriesid, int32 sceneid)
	{
		boost::shared_ptr<Packet::WorldBossInfo> boss = GetCurWorldBossInfo();
		if (boss == nullptr)
			return;
		auto monster = MtMonsterManager::Instance().GetFirstMosterByGroupId(boss->monstergourpid());
		if (monster == nullptr)
			return;
		allhp_ = monster->battle_info().hp();
		allhp_ += 4000000 * (boss->bosslevel()-1);
		boss->set_hp(allhp_);
		boss_seriesid_ = seriesid;
		killerguild = INVALID_GUID;
		ranklist_.cleardata();
		auto scene = MtSceneManager::Instance().GetSceneById(sceneid);
		if (scene != nullptr)
		{
			Packet::WorldBossInfoReply bossreply;
			bossreply.mutable_boss()->CopyFrom(*boss.get());
			bossreply.set_boss_seriesid(MtWorldBossManager::Instance().BossSeriesid());
			bossreply.set_allhp(allhp_);
			scene->BroadcastMessage(bossreply);
		}
	}
	void MtWorldBossManager::OnEndBoss(bool killboss)
	{
		boost::shared_ptr<Packet::WorldBossInfo> boss = GetCurWorldBossInfo();
		if (boss == nullptr)
			return;
		auto config = GetCurWorldBossConfig();
		if (config == nullptr)
			return;

		auto bossmonster = MtMonsterManager::Instance().GetFirstMosterByGroupId(boss->monstergourpid());
		if (bossmonster == nullptr)
			return;
		if (boss->hp() <= 0)
		{
			LOG_INFO << "ENDBOSS TWICE";
			return;
		}
		boss->set_hp(0);
		ranklist_.greater_sort();
		if (killboss == true)
		{
			int32 level = boss->bosslevel();
			if (level < 100)
			{
				level++;
				//boss->set_bosslevel(level);
				//##又要改成所有等级一样。。。
				for (auto iter : bosslist_)
				{
					iter->set_bosslevel(level);
				}
			}
			
			std::stringstream notify;
			notify << "wb_win" << "|" << "monster_name_"<< bossmonster->id();
			//send_system_message_toworld(notify.str());
			send_run_massage(notify.str());
		}
		else
		{
			int32 level = boss->bosslevel();
			if (level > 1)
			{
				level--;
				//boss->set_bosslevel(level);
				//##又要改成所有等级一样。。。
				for (auto iter : bosslist_)
				{
					iter->set_bosslevel(level);
				}
			}
			std::stringstream notify;
			notify << "wb_lost" << "|" << "monster_name_" << bossmonster->id();
			//send_system_message_toworld(notify.str());
			send_run_massage(notify.str());
		}
		
		int32 i = 0;
		for (auto playerinfo : ranklist_.m_data)
		{
			uint64 playerid = playerinfo.playerid_;
			auto player_ = Server::Instance().FindPlayer(playerid);
			if (player_ != nullptr)
			{
				int32 valuedamage = playerinfo.damagevalue_;
				google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
				auto item_ = items.Add();
				item_->set_itemid(Packet::TokenType::Token_Gold);
				item_->set_itemcount((valuedamage/10));
				int32 lawful = 0;
				int count = 1;
				if (i == 0) {
					count = 5;
					lawful = 800;
					LOG_INFO << "WORLDBOSSEND "<<i <<","<< player_->Guid();
				}
				else if (i == 1) {
					count = 3;
					lawful = 600;
					LOG_INFO << "WORLDBOSSEND " << i << "," << player_->Guid();
				}
				else if (i == 2){
					count = 2;
					lawful = 400;
					LOG_INFO << "WORLDBOSSEND " << i << "," << player_->Guid();
				}
				else if (i >= 3 && i < 10) {
					lawful = 200;
					LOG_INFO << "WORLDBOSSEND " << i << "," << player_->Guid();
				}
				else {
					lawful = 100;
				}
				item_ = items.Add();
				item_->set_itemid(config->reward());
				item_->set_itemcount(count);

				std::stringstream notify;
				notify << "wb_end" << "|" << player_->Name() << "|" << "monster_name_" << bossmonster->id()<<"|"<<(i+1)<< "|" << MtItemManager::Instance().GetItemName(config->reward())<<"|"<< count;
				if (i<3)
				{
					//send_system_message_toworld(notify.str());
					send_run_massage(notify.str());
				}
				auto dc_container = player_->DataCellContainer();				
				if(dc_container != nullptr && dc_container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)) {
					lawful = (int32)((float)lawful*(1.0 + 0.05));
				}
				item_ = items.Add();
				item_->set_itemid(Packet::TokenType::Token_LawFul);
				item_->set_itemcount(lawful);
				boost::shared_ptr<Packet::MessageInfo> mail_ = static_message_manager::Instance().create_message(player_->Guid(), player_, MAIL_OPT_TYPE::WORLDBOSS_ADD, "wb_mail_title", notify.str(), &items);
				i++;
			}
		}
		
		auto killer_ = Server::Instance().FindPlayer(killerguild);
		google::protobuf::RepeatedPtrField<Packet::ItemCount> items;
		auto item_ = items.Add();
		item_->set_itemid(Packet::TokenType::Token_Gold);
		item_->set_itemcount(200000);
		item_ = items.Add();
		item_->set_itemid(config->reward());
		item_->set_itemcount(5);
		
		if (killer_ != nullptr)
		{
			boost::shared_ptr<Packet::MessageInfo> mail_ = static_message_manager::Instance().create_message(killerguild, killer_, MAIL_OPT_TYPE::WORLDBOSS_ADD, "wb_mail_title", "wb_mail_killer", &items);

			LOG_INFO << "WORLDBOSSEND killer "<< killerguild;

			std::stringstream killernotify;
			killernotify << "wb_killer" <<"|"<< killer_->Name()<< "|" << "monster_name_" << bossmonster->id() <<"|" << MtItemManager::Instance().GetItemName(config->reward()) << "|" << 5<<"|"<< MtItemManager::Instance().GetItemName(Packet::TokenType::Token_Gold)<<"|"<< 200000;
			//send_system_message_toworld(killernotify.str());
			send_run_massage(killernotify.str());
		}
	}
	bool MtWorldBossManager::IsWorldBossScene(int sceneid)
	{
		for (auto config : bossconfiglist_)
		{
			if (config->sceneid() == sceneid)
			{
				return true;
			}
		}
		return false;
	}
	bool MtWorldBossManager::WorldBossDead()
	{
		boost::shared_ptr<Packet::WorldBossInfo> boss = GetCurWorldBossInfo();
		if (boss == nullptr)
			return true;
		if (boss->hp() <= 0)
		{
			return true;
		}
		return false;
	}
	void MtWorldBossManager::OnBattleEndDamageBoss(boost::shared_ptr<MtPlayer> player_, int32 damage)
	{
		if (player_ == nullptr )
			return;
		boost::shared_ptr<Packet::WorldBossInfo> boss = GetCurWorldBossInfo();
		if (boss == nullptr)
			return;
		if (boss->hp() <= 0)
			return;

		int64 playerdamagevalue = player_->AddWorldBossDamage(damage);
		worldbossuserdamage playerdamage;
		playerdamage.playerid_ = player_->Guid();
		playerdamage.playername_ = player_->GetScenePlayerInfo()->name();
		playerdamage.damagevalue_ = (int32)playerdamagevalue;
		playerdamage.allbosshp_ = allhp_;
		ranklist_.nosortinsert(playerdamage);

		//for (auto ad : statistics->actordamage_)
		//{
		//	player_->AddWorldBossActorDamage(ad.first, (int32)(ad.second*(1 + times*0.2f)));
		//}

		int64 hp = boss->hp();
		if (hp > (int64)damage)
		{
			hp -= (int64)damage;
			boss->set_hp(hp);
		}
		else
		{
			killerguild = player_->Guid();
			OnEndBoss(true);
		}
		
		

		
	}
	void MtWorldBossManager::OnWorldBossStatisticsReq(boost::shared_ptr<MtPlayer> player_)
	{
		if (player_ == nullptr)
			return;
		Packet::WorldBossStatisticsReply reply;
		for (auto dama : ranklist_.m_data)
		{
			auto info=reply.add_infos();
			info->set_playerguid(dama.playerid_);
			info->set_allbosshp(dama.allbosshp_);
			info->set_playername(dama.playername_);
			info->set_damagevalue(dama.damagevalue_);
		}
		//auto wbad = player_->GetWorldBossActorDamage();
		//for (auto ad: wbad)
		//{
		//	auto info = reply.add_selfactordamage();
		//	info->set_guid(ad.first);
		//	info->set_damage(ad.second);
		//}
		player_->SendMessage(reply);
	}
	const boost::shared_ptr<Packet::WorldBossConfig> MtWorldBossManager::GetCurWorldBossConfig()
	{
		int32 weekday = MtTimerManager::Instance().GetWeekDay();
		for (auto boss_ : bossconfiglist_)
		{
			if (boss_->day() == weekday)
			{
				return boss_;
			}
		}
		return nullptr;
	}
}