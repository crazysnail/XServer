#ifndef MTONLINE_GAMESERVER_MT_ARENA_H__
#define MTONLINE_GAMESERVER_MT_ARENA_H__
#include "../mt_types.h"
#include "../mt_server/mt_server.h"
#include "../data/data_manager.h"
#include "../mt_actor/mt_actor_config.h"
#include <map>
#include <ArenaDb.pb.h>
#include <ArenaMessage.pb.h>
#include "../base/mt_timer.h"

namespace Mt
{
	struct ArenaMemUser
	{
		enum class LoadStatus
		{
			START_LOADING = 1,
			LOADED = 2,
			NEED_RELOAD = 3,
		};
		LoadStatus status_;
		boost::shared_ptr<DB::ArenaUser> user_;
		std::vector<uint64> target_guids_;
		uint64 last_reload_tick_;
		bool panel_open_ = false;
		bool in_battle = false; //战斗中
	};
	class MtArenaManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
		struct RobotActor
		{
			int32 actor_config_id_ = 0;
			int32 level_ = 0;
			int32 rank_ = 0;
			std::string icon_;
			int32 score_ = 0;
			Packet::BattleInfo battle_info_;
			std::vector<int32> equip_ids_;
			std::vector<int32> skill_ids_;
			void LoadFrom(data_row& row);
		};
		struct RobotPlayer
		{
			int32 rank_ = 0;
		    std::string icon_;
			int32 level_ = 0;
			std::string name_;
			int32 score_ = 0;
			std::vector<RobotActor> actors_;
			int32 ConfigId() const
			{
				if (actors_.empty())
					return 0;
				else
					return actors_.front().actor_config_id_;
			}
			void PushBack(const RobotActor& actor);
		};
		struct RankReward
		{//每日结算奖励
			int32 rank_0_;
			int32 rank_1_;
			int32 honor_ = 0;
			int32 crystal_ = 0;
			int32 gold_ = 0;
		};

		MtArenaManager();
		static MtArenaManager& Instance();
		/**
		* \brief 通知DB线程加载一个玩家的竞技场排名和对手数据
		* \param player
		* \return void
		*/
		void OnOpenArenaPanel(const boost::shared_ptr<MtPlayer>& player);
		/**
		* \brief 刷新对手
		* \param player
		* \return void
		*/
		void OnRefreshPlayerArenaMainData(const boost::shared_ptr<MtPlayer>& player);
		/**
		* \brief  购买挑战次数后, 刷新数据
		* \param player
		*/
		void OnBuyChallengeCount(const boost::shared_ptr<MtPlayer>& player);
		void OnChallengePlayer(uint64 src_guid, uint64 target_guid, int32 battle_group_index);
		void OnCloseArenaPanel(const boost::shared_ptr<MtPlayer>& player);
		void OnChallengeDone(const boost::shared_ptr<MtPlayer>& challenger,
			const boost::shared_ptr<ArenaMemUser>& target,
			uint64 reply_guid, bool win);
		const boost::shared_ptr<DB::ArenaUser> FindArenaUser(uint64 guid);
		bool TargetBusy(uint64 guid);
		void OnNewDay();
		void OnSave(boost::shared_ptr<zxero::dbcontext>& dbctx);
		void OnCacheAll( );
		int32 OnLoad();
		void OnPlayerScoreChanged(const boost::shared_ptr<MtPlayer>& player, int32 score);
		void OnLoadPlayerArenaMainDataDone(
			boost::shared_ptr<DB::ArenaUser>& self,
			std::vector<boost::shared_ptr<DB::ArenaUser>>& targets);
		void OnCheckInit(boost::shared_ptr<zxero::dbcontext>& dbctx);
		void AddLogToUsers(const boost::shared_ptr<DB::ArenaUser>& challenger,
			const boost::shared_ptr<DB::ArenaUser>& target,
			uint64 reply_guid, bool win);
		bool ArenaRobot2ArenaPlayer(DB::ArenaUser& db_info, Packet::ArenaPlayer& palyer_info);
		const RobotPlayer* FindRobotPlayer(DB::ArenaUser& arene_user);
		void ArenaActor2ActorFullInfo(const RobotActor& actor,
			Packet::ActorFullInfo& full_actor);
		void SkipArenaRobot(bool skip);
		/**
		* \brief 发放日常奖励
		* \return void
		*/
		void DrawDailyArenaReward(std::vector<std::pair<int32, uint64>>& guids);
	private:
		void ChallengeReward(const boost::shared_ptr<MtPlayer>& player, bool win);
		void SendDataToClient(const boost::shared_ptr<MtPlayer>& player);
		void PackArenaMessage(uint64 player_guid, Packet::ArenaMainData& msg);
		bool ArenaUser2ArenaPlayer(const boost::shared_ptr<DB::ArenaUser>& db_user, Packet::ArenaPlayer& arena_player);
		MtArenaManager::RankReward FindRankReward(int32 rank);
		std::map<uint64, boost::shared_ptr<ArenaMemUser>> arena_users_map_; //加载状态
		std::map<int32, boost::shared_ptr<RobotPlayer>> robot_player_map_;
		std::recursive_mutex arena_user_map_lock_;
		bool skip_ = false;
		std::list<RankReward> rank_reward_list_;
	};
}

#endif // MTONLINE_GAMESERVER_MT_ARENA_H__