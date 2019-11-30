#include "module.h"
#include "../mt_guid/mt_guid.h"
#include "active_model.h"
#include "ActorBasicInfo.pb.h"
#include <S2GMessage.pb.h>
#include "../base/client_session.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../mail/static_message_manager.h"
#include "top_ranklist.h"

namespace Mt
{
	static MtTopRankManager* __top_list_manager = nullptr;

	REGISTER_MODULE(MtTopRankManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtTopRankManager::OnLoad, __top_list_manager));
	}

	MtTopRankManager::MtTopRankManager()
	{
		__top_list_manager = this;
	}

	MtTopRankManager& MtTopRankManager::Instance()
	{
		return *__top_list_manager;
	}

	zxero::int32 MtTopRankManager::OnLoad()
	{
		return 0;
	}
	bool MtTopRankManager::OnLoadAll(boost::shared_ptr<zxero::dbcontext>& dbctx)
	{
		std::set<uint64> loadplayersceneinfo;
		//不判断null就让你崩溃
		const boost::shared_ptr< sql::Connection > connection = dbctx->get_connection();
		{
			std::string select_ = R"sql(
										select p.`guid` as `player_guid`, p.`basic_info.level` as `player_level` ,
										p.`basic_info.init_actor_id` as `init_actor_id` ,p.`basic_info.name` as `player_name` ,
										a.`score` as `actor_score` from PlayerDBInfo p inner join ActorBasicInfo a on 
										p.`basic_info.init_actor_id` = a.`actor_config_id` and p.`guid` = a.`player_guid` 
										order by a.`score` desc limit 50;
								  )sql";
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			while (dbresult->next())
			{
				uint64 playerguid = dbresult->getUInt64("player_guid");
				TopData info;
				info.data.set_playerguid(playerguid);
				info.data.set_playername(dbresult->getString("player_name"));
				info.data.set_level(dbresult->getInt("player_level"));
				info.data.set_init_actor_id(dbresult->getInt("init_actor_id"));
				info.data.add_value(dbresult->getInt("actor_score"));
				fightvaluelist_[Packet::Professions::Non_Prof].nosortinsert(info);
				loadplayersceneinfo.insert(playerguid);
			}
		}
		{
			for (int32 i=1;i<Packet::Professions_ARRAYSIZE;i++)
			{
				std::string select_ = (boost::format(R"sql(
															select p.`guid` as `player_guid` ,p.`basic_info.level` as `player_level`,
															p.`basic_info.init_actor_id` as `init_actor_id`,p.`basic_info.name` as `player_name`,
															a.`race` as `actor_race`, a.`score` as `actor_score` from PlayerDBInfo p inner join 
															ActorBasicInfo a on p.`basic_info.init_actor_id` = a.`actor_config_id` and p.`guid` = a.`player_guid` and 
															a.`profession` = %d order by a.`score` desc limit 50;
														)sql") % i).str();
				boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
				boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
				while (dbresult->next())
				{
					uint64 playerguid = dbresult->getUInt64("player_guid");
					TopData info;
					info.data.set_playerguid(playerguid);
					info.data.set_playername(dbresult->getString("player_name"));
					info.data.set_level(dbresult->getInt("player_level"));
					info.data.set_init_actor_id(dbresult->getInt("init_actor_id"));
					info.data.add_value(dbresult->getInt("actor_score"));
					fightvaluelist_[i].nosortinsert(info);
					loadplayersceneinfo.insert(playerguid);
				}
			}
		}
		{
			OnLoadArena(dbctx, loadplayersceneinfo);
		}
		{
			std::string select_ = R"sql(
										select `guid`, `basic_info.name`, `basic_info.level`, `basic_info.init_actor_id` 
										from PlayerDBInfo order by `basic_info.level` desc limit 50;
								  )sql";
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			while (dbresult->next())
			{
				uint64 playerguid = dbresult->getUInt64("guid");
				TopData info;
				info.data.set_playerguid(playerguid);
				info.data.set_playername(dbresult->getString("basic_info.name"));
				info.data.set_init_actor_id(dbresult->getInt("basic_info.init_actor_id"));
				info.data.add_value(dbresult->getInt("basic_info.level"));
				playerlevellist_.nosortinsert(info);
				loadplayersceneinfo.insert(playerguid);
			}
		}
		//玩家战斗力
		{
			std::string select_ = R"sql(
										select `guid`, `basic_info.name`, `basic_info.level`, `basic_info.init_actor_id`, `basic_info.battle_score`
										from PlayerDBInfo order by `basic_info.battle_score` desc limit 50;
								  )sql";
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			while (dbresult->next())
			{
				uint64 playerguid = dbresult->getUInt64("guid");
				TopData info;
				info.data.set_playerguid(playerguid);
				info.data.set_playername(dbresult->getString("basic_info.name"));
				info.data.set_init_actor_id(dbresult->getInt("basic_info.init_actor_id"));
				info.data.set_level(dbresult->getInt("basic_info.level"));
				info.data.add_value(dbresult->getInt("basic_info.battle_score"));
				playerfightvaluelist_.nosortinsert(info);
				loadplayersceneinfo.insert(playerguid);
			}
		}
		//通天塔
		{
			std::string select_ = R"sql(
										select p.`guid` as `player_guid`, p.`basic_info.level` as `player_level` ,
										p.`basic_info.init_actor_id` as `init_actor_id` ,p.`basic_info.name` as `player_name` ,
										a.`tower_level` as `tower_level` , a.`tower_time` as `tower_time` from PlayerDBInfo p inner join PlayerTowerInfo a on 
										p.`guid` = a.`guid` order by a.`tower_level` desc, a.`tower_time` limit 50;
								  )sql";
			boost::scoped_ptr<sql::PreparedStatement> statement(connection->prepareStatement(select_));
			boost::scoped_ptr<sql::ResultSet> dbresult(statement->executeQuery());
			while (dbresult->next())
			{
				uint64 playerguid = dbresult->getUInt64("player_guid");
				TopData info;
				info.data.set_playerguid(playerguid);
				info.data.set_playername(dbresult->getString("player_name"));
				info.data.set_level(dbresult->getInt("player_level"));
				info.data.set_init_actor_id(dbresult->getInt("init_actor_id"));
				info.data.add_value(dbresult->getInt("tower_level"));
				info.data.add_value(-dbresult->getInt("tower_time"));
				towerlist_.nosortinsert(info);
				loadplayersceneinfo.insert(playerguid);
			}
		}
		//最后统一做一次加载
		for (uint64 playerid : loadplayersceneinfo)
		{
			boost::shared_ptr<Packet::ScenePlayerInfo> sceneinfo = Server::Instance().LoadScenePlayerInfoByGuid(dbctx, playerid);
			if (sceneinfo)
			{
				Server::Instance().InsertScenePlayerInfo(playerid, sceneinfo);
			}
		}
		return true;
	}
	
	bool MtTopRankManager::OnLoadArena(boost::shared_ptr<zxero::dbcontext>& dbctx, std::set<uint64>& loadplayersceneinfo)
	{
		auto arenauserlist = ActiveModel::Base<DB::ArenaUser>(dbctx).FindBy(" `rank` <= 50 ");
		for (auto arenauser : arenauserlist)
		{
			TopData info;
			uint64 playerguid = arenauser->player_guid();
			info.data.set_playerguid(playerguid);
			info.data.add_value(arenauser->rank());
			info.data.add_value(arenauser->score());
			info.data.set_playername("MtOnline");
			info.data.set_init_actor_id(1);
			info.data.set_level(1);
			if (arenauser->is_robot() == false)
			{
				if (ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).CheckExist({ { "guid", playerguid } }))
				{
					auto player = ActiveModel::Base<Packet::PlayerDBInfo>(dbctx).FindOne({ { "guid", playerguid } });
					if (player != nullptr)
					{
						auto basic_info = player->mutable_basic_info();
						info.data.set_playername(basic_info->name());
						info.data.set_init_actor_id(basic_info->init_actor_id());
						info.data.set_level(basic_info->level());
						loadplayersceneinfo.insert(playerguid);
					}
				}

			}
			else
			{
				auto robotconfig = MtArenaManager::Instance().FindRobotPlayer(*arenauser);
				if (robotconfig != nullptr)
				{
					info.data.set_playername(robotconfig->name_);
					info.data.set_init_actor_id(robotconfig->ConfigId());
					info.data.set_level(robotconfig->level_);
				}
			}
			arenalist_.nosortinsertbydecs(info);
		}
		return true;
	}
	bool MtTopRankManager::OnTopRankDataReq(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::TopRankDataReq>& message,
		int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto group = message->group();
		if (group < 0 || group >= Packet::RankGroup_ARRAYSIZE)
			return true;
		int32 type = message->type();
		Packet::TopRankDataReply reply;
		reply.set_refreshtime(0);
		switch (group)
		{
		case Packet::RankGroup::Rank_FormationFightValue:
		{
			for (auto iter_ : playerfightvaluelist_.m_data)
			{
				reply.add_list()->CopyFrom(iter_.data);
			}
		}
		break;
		case Packet::RankGroup::Rank_FightValue:
		{
			if (type < 0 || type >= Packet::Professions_ARRAYSIZE)
				return true;
			for (auto iter_ : fightvaluelist_[type].m_data)
			{
				reply.add_list()->CopyFrom(iter_.data);
			}
		}
		break;
		case Packet::RankGroup::Rank_Arena:
		{
			for (auto iter_ : arenalist_.m_data)
			{
				reply.add_list()->CopyFrom(iter_.data);
			}
			
		}
		break;
		case Packet::RankGroup::Rank_Level:
		{
			for (auto iter_ : playerlevellist_.m_data)
			{
				reply.add_list()->CopyFrom(iter_.data);
			}
		}
		break;
		case Packet::RankGroup::Rank_Tower:
		{
			for (auto iter_ : towerlist_.m_data)
			{
				reply.add_list()->CopyFrom(iter_.data);
			}
		}
		break;
		default:
			return true;
		}
		player->SendMessage(reply);
		return true;
	}
	void MtTopRankManager::OnPlayerScoreChanged(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player == nullptr)
			return;

		TopData info;
		info.data.set_playerguid(player->Guid());
		info.data.set_playername(player->GetScenePlayerInfo()->name());
		info.data.set_init_actor_id(player->GetScenePlayerInfo()->init_actor_id());
		info.data.set_level(player->PlayerLevel());
		info.data.add_value(player->BattleScore());
		{
			boost::upgrade_lock<boost::shared_mutex> l(fightvalue_shared_mutex_);
			bool newdata = playerfightvaluelist_.nosortinsert(info);
			if (newdata)
			{
				player->SendScenePlayerInfoToServer();
			}
		}
		{
			boost::upgrade_lock<boost::shared_mutex> l(arena_shared_mutex_);
			for (auto& arenadata : arenalist_.m_data)
			{
				if (arenadata.data.playerguid() == player->Guid())
				{
					if (arenadata.data.value_size() == 2)
					{
						arenadata.data.set_value(1, player->BattleScore());
					}
					break;
				}
			}
		}
	}
	void MtTopRankManager::OnMainActorScoreChanged(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player == nullptr)
			return;
		auto actor = player->MainActor();
		if (actor != nullptr) {
			TopData info;
			info.data.set_playerguid(player->Guid());
			info.data.set_playername(player->GetScenePlayerInfo()->name());
			info.data.set_init_actor_id(player->GetScenePlayerInfo()->init_actor_id());
			info.data.set_level(player->PlayerLevel());
			info.data.add_value(actor->Score());
			boost::upgrade_lock<boost::shared_mutex> l(professions_shared_mutex_);
			bool newdatar = fightvaluelist_[Packet::Professions::Non_Prof].nosortinsert(info);
			bool newdatal = fightvaluelist_[actor->Professions()].nosortinsert(info);
			if (newdatar || newdatal)
			{
				player->SendScenePlayerInfoToServer();
			}
		}
	}
	void MtTopRankManager::OnPlayerLevelChanged(const boost::shared_ptr<MtPlayer> player)
	{
		if (player == nullptr)
			return;
		TopData info;
		info.data.set_playerguid(player->Guid());
		info.data.set_playername(player->GetScenePlayerInfo()->name());
		info.data.set_init_actor_id(player->GetScenePlayerInfo()->init_actor_id());
		info.data.set_level(player->PlayerLevel());
		info.data.add_value(player->PlayerLevel());
		boost::upgrade_lock<boost::shared_mutex> l(level_shared_mutex_);
		bool newdata = playerlevellist_.nosortinsert(info);
		if (newdata)
		{
			player->SendScenePlayerInfoToServer();
		}
	}
	
	void MtTopRankManager::OnPlayerArenaChanged(const boost::shared_ptr<MtPlayer>& challenger,
		int32 user_rank, int32 user_score, int32 target_rank,
		int32 target_score, uint64 target_guid,
		std::string targetname, int32 target_actor_id, int32 target_level)
	{
		if (challenger == nullptr )
			return;
		if (user_rank > 50 )
			return;

		TopData targetinfo;
		targetinfo.data.set_playerguid(target_guid);
		targetinfo.data.add_value(target_rank);
		targetinfo.data.add_value(target_score);
		targetinfo.data.set_playername(targetname);
		targetinfo.data.set_init_actor_id(target_actor_id);
		targetinfo.data.set_level(target_level);
		
		TopData info;
		info.data.set_playerguid(challenger->Guid());
		info.data.add_value(user_rank);
		info.data.add_value(user_score);
		info.data.set_playername(challenger->Name());
		info.data.set_init_actor_id(challenger->GetScenePlayerInfo()->init_actor_id());
		info.data.set_level(challenger->PlayerLevel());

		boost::upgrade_lock<boost::shared_mutex> l(arena_shared_mutex_);
		arenalist_.nosortinsertbydecs(targetinfo);
		bool newdata = arenalist_.nosortinsertbydecs(info);
		if (newdata)
		{
			challenger->SendScenePlayerInfoToServer();
		}
	}

	void MtTopRankManager::OnPlayerTowerChanged(const boost::shared_ptr<MtPlayer>& player, int32 towerlevel, int32 towertime)
	{
		if (player == nullptr)
			return;
		TopData info;
		info.data.set_playerguid(player->Guid());
		info.data.set_playername(player->GetScenePlayerInfo()->name());
		info.data.set_init_actor_id(player->GetScenePlayerInfo()->init_actor_id());
		info.data.set_level(player->PlayerLevel());
		info.data.add_value(towerlevel);
		info.data.add_value(-towertime);
		boost::upgrade_lock<boost::shared_mutex> l(tower_shared_mutex_);
		bool newdata = towerlist_.nosortinsert(info);
		if (newdata)
		{
			player->SendScenePlayerInfoToServer();
		}
	}
	void MtTopRankManager::OnWeekCleanPlayerTower()
	{
		boost::upgrade_lock<boost::shared_mutex> l(tower_shared_mutex_);
		towerlist_.greater_sort();
		int32 i = 0;
		for (auto t_p: towerlist_.m_data)
		{
			i++;
			std::vector<int32> items;
			try {
				items = thread_lua->call<std::vector<int32>>(5, "TowerRankReward", i);
			}
			catch (ff::lua_exception& e) {
				LOG_ERROR << "LuaScriptException OnLuaFunCall_x_Server error!" << e.what();				
			}

			if (items.size() > 0) {
				uint64 playerid = t_p.data.playerguid();
				google::protobuf::RepeatedPtrField<Packet::ItemCount> mail_items;
				for (uint32 k = 0; k < items.size(); k++) {
					auto item_ = mail_items.Add();
					item_->set_itemid(items[k]);
					item_->set_itemcount(1);
				}
				std::stringstream notify;
				notify << "tower_reward_mail|" << i;
				static_message_manager::Instance().create_message(playerid, nullptr, MAIL_OPT_TYPE::TOWER_ADD, "rank_reward_tile", notify.str(), &mail_items);
			}
		}

		towerlist_.cleardata();
	}
}