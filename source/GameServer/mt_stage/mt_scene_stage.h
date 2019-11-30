#ifndef MTONLINE_GAMESERVER_MT_SCENE_STAGE_H__
#define MTONLINE_GAMESERVER_MT_SCENE_STAGE_H__

#include "../mt_types.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
#include <map>
#include <vector>

namespace Mt
{
	class MtSceneStageManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtSceneStageManager& Instance();
		MtSceneStageManager();
		zxero::int32 OnLoad();
		int32 DefaultStageId(int32 camp) const;
		bool OnCacheAll( );
		void OnDbConnected();

		//server线程, 全局数据 start
		/**
		* \brief 获取全局关卡数据
		*/
		Packet::SceneStageInfo* GetSceneStageInfo(int32 stage_id) const;
		const Config::SceneStageConfig* GetConfig(zxero::int32 stage_id) const;
		int32 GetStageDefaultDen(int32 stage_id) const;
		void ClearPlayerOldCaptrueStage(const boost::shared_ptr<MtPlayer>& player);
		bool OnPlayerCaptureStage(const boost::shared_ptr<MtPlayer>& player, 
			const boost::shared_ptr<S2G::PlayerCaptureStatge>& message,
			int32 /*source*/);
		bool OnPlayerHookDataUpdate(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<S2G::PlayerHookDataUpdate>& message,
			int32 /*source*/);
		bool OnPlayerUpdateCaptureForm(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<S2G::PlayerUpdateCaptureForm>& message,
			int32 /*source*/);

		bool OnUpdateStageTax(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::UpdateStageTax>& message,
			int32 /*source*/);

		bool OnGetSceneStageInfo(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::GetSceneStageInfo>& message,
			int32 /*source*/);

		bool OnCollectTax(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::CollectAllTax>& message,
			int32 /*source*/);
		bool OnCaptureStage(const boost::shared_ptr<client_session>& session,
			const boost::shared_ptr<Packet::CaptureStage>& message,
			int32 /*source*/);
		//server线程, 全局数据 end

		//场景线程, 玩家数据 start
		//bool PlayerCaptureStage(zxero::int32 stage_id, bool success,  MtPlayer* player, const std::vector<boost::shared_ptr<MtActor>>& formation);
		//void UpdateCapturedStage(const boost::shared_ptr<MtPlayer>& player);
		//bool AddBossChallengeProgress(const MtPlayer* player, int32 stage_id, int32 count);
		//bool PlayerRaidStage(zxero::int32 stage_id, int32 time_cost, int32 formation_score, MtPlayer* player, const std::vector<boost::shared_ptr<MtActor>>& formation);
		void OnLoadSceneStageInfo(const std::vector<boost::shared_ptr<Packet::SceneStageInfo>>& stages);
		//void OnLoadSceneStageNew(const std::vector<boost::shared_ptr<Packet::SceneStageNew>>& stages);
		//void OnLoadPlayerSceneStage(const std::vector<boost::shared_ptr<Packet::PlayerSceneStage>>& player_scene_stages);
		//bool IsStageRaid(zxero::int32 stage_id, zxero::uint64 player_guid);
		//boost::shared_ptr<Packet::PlayerSceneStage> MutablePlayerSceneStageInfo(uint64 player_guid, int32 scene_stage_id);
		//std::vector<boost::shared_ptr<MtActor>> StageCaptain(int32 stage_id);
		//uint64 StageCaptainGuid(int32 stage_id) const;
		//前一场景通过后便可Raid
		//bool CanRaidStage(zxero::int32 stage_id, zxero::uint64 player_guid);
		//通关后便可rush
		//bool CanRushStage(zxero::int32 stage_id, zxero::uint64 player_guid);
		//void GmPlayerRaidAllStage(const boost::shared_ptr<MtPlayer>& player);
		//Packet::PlayerSceneStage RunTimePlayerSceneStageInfo(uint64 player_guid, int32 scene_stage_id);
		//bool PlayerHasSceneStageInfo(uint64 player_guid, int32 scene_stage_id);
		//bool IncBossChallengeCount(const boost::shared_ptr<MtPlayer>& player, int32 stage_id, int32 count = 1);
		//bool DecBossChallengeCount(const boost::shared_ptr<MtPlayer>& player, int32 stage_id, int32 count = 1);
		//bool OnPlayerKillBoss(int32 stage_id, const boost::shared_ptr<MtPlayer>& player, const std::vector<boost::shared_ptr<MtActor>> actors, int32 time_cost, int32 score);
		//const boost::shared_ptr<Packet::SceneStageNew> MutableSceneStageInfo(int32 stage_id);
		int32 CollectTax(const boost::shared_ptr<MtPlayer>& player, int32 stage_id, int32 gold);
		void InitSceneStageInfo(const boost::shared_ptr<dbcontext>& dbctx);
		int32 GetNextStageId(int32 stage_id) const;
		Packet::SceneStageInfo* GetGroupSceneStageByGroupId(int32 group_id) const;
		/*void UpdateSceneStageOnHookRank(int32 stage_id, uint64 player_guid,
			const std::string& player_name, int32 total_time, int32 total_income);
		bool SceneStageInfo2ClientInfo(const boost::shared_ptr<Packet::SceneStageNew>& stage_info,
			Packet::ClientSceneStageInfo& info,
			uint64 player_guid);
		int32 calc_challenge_boss_count(const MtPlayer* player, const int32 cur_count,int32 rush_count);
		void LoadPlayerStages(uint64 player_guid) const;
		void OnLoadPlayerStages(const boost::shared_ptr<Packet::PlayerStages>& data);
		Packet::PlayerStages* FindPlayerStages(uint64 player_guid) const;*/
	private:
		std::map<zxero::int32, boost::shared_ptr<Config::SceneStageConfig>> configs_; //<关卡id, 关卡配置>
		int32 alliance_default_stage_ = 0;
		int32 horde_default_stage_ = 0;
		std::map<int32, boost::shared_ptr<Packet::SceneStageInfo>> scene_stages_; // <关卡id, 关卡db数据>
		std::map<int32, int32> first_group_stage_map_; //<group_id,stage_id> //公会占点group对应的第一个关卡(税金,税率,播报标志放这里)

		/*std::map<zxero::int32, boost::shared_ptr<Packet::SceneStage>> scene_stages_; //<关卡id, 关卡db数据>
		std::map<int32, boost::shared_ptr<Packet::SceneStageNew>> scene_stages_new_; //<关卡id, 关卡db数据>
		std::map<PlayerSceneStageKey, boost::shared_ptr<Packet::PlayerSceneStage>> player_scene_stages_; //<<玩家guid,关卡id>, db数据>
		std::recursive_mutex player_scene_stage_mutex_; //玩家挂机数据锁
		std::map<int32, boost::shared_ptr<Packet::OnHookRankUserRuntime>> run_time_on_hook_rank_users_; //<关卡id, 挂机排行>
		std::map<uint64, boost::shared_ptr<Packet::PlayerStages>> player_stages_; //玩家挂机数据*/
	};
}
#endif // MTONLINE_GAMESERVER_MT_SCENE_STAGE_H__
