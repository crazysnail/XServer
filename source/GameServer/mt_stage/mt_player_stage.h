#ifndef MTONLINE_GAMESERVER_MT_PLAYER_STAGE_H__
#define MTONLINE_GAMESERVER_MT_PLAYER_STAGE_H__

#include "../mt_types.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
#include <map>
#include <vector>

namespace Mt
{
	class MtPlayerStageProxy : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
		MtPlayerStageProxy() { LOG_ERROR << "should not create"; }
		MtPlayerStageProxy(boost::weak_ptr<MtPlayer> p);
		void PlayerCaptrueStage(int32 stage_id, bool win);
		bool CanRaidStage(int32 stage_id);
		bool IsStageRaid(zxero::int32 stage_id);
		bool CanRushStage(zxero::int32 stage_id) { return IsStageRaid(stage_id); }
		int32 CalcChallengeBossCount(const int32 cur_count, int32 rush_count);
		bool AddBossChallengeProgress(int32 stage_id, int32 count);
		void UpdateCapturedStage();
		bool OnPlayerRaidStage(int32 stage_id);
		void SyncSpecStageToClient(int32 stage_id);
		void GmPlayerRaidAllStage(); //GM指令需要的, 其他地方不要调用
		bool CanAutoHook();
		Packet::PlayerStage* GetPlayerStage(int32 stage_id);
		bool IncBossChallengeCount(int32 stage_id, int32 count);
		bool DecBossChallengeCount(int32 stage_id, int32 count);
		bool GiveRealTimeHookItem(uint64 elapse_seconds);
		void ResetTo(int32 stage_id);
		void OnPlayerOnline(int32 offline_sec);
		/**
		* \brief 同步当前挂机的效率给server
		* \return void
		*/
		void SyncHookData2Server();
		/**
		* \brief 根据离线时间和配置, 生成奖励
		* \return int32 未使用的离线时间
		*/
		//同步当前挂机效率和挂机配置到客户端
		void SyncEffe2Client();
		uint64 GenerateReward(uint64 offline_mill_sec, Packet::RTHC& config, bool last_one);
		//int32 MakeTax(int32 stage_id, int32 gold);
		void ItemDrops(const std::map<int32, int32>& drop_items);
		
		bool UpdateRTHCData(uint64 battle_time);
		int32 FirstNeedCalcId();
		void RecalcRealTimeHookData();
		void OnBattleEnd(MtBattleGround* battle);
		void OnActorDead(MtActor* actor);
		Packet::RTHC* CurrentRTHC();
		void ResetStageTime(int32 stage_id);
		void ClearEfficentLog();
		void OnMonsterDead(int32 count = 1);
		void OnItemDrop(int32 id, int32 count);
		void OnAddExp(int32 exp);
		void OnAddExtraExp(int32 exp);
		void OnAddGold(int32 gold);
		void AddStageBossToEffi(int32 stage_id, int32 count);
		void AddItemToEffi(int32 id, int32 count);
		std::tuple <int32, std::vector<Packet::ActorFullInfo>> GetCaptureStageTempInfo();
		void SetCaptureStageTempInfo(int32 stage_id, const std::vector<Packet::ActorFullInfo>& actors);
		void ClearCaptureStageTempInfo();
	private:
		int32 CheckDismantleEquip(const int32 id, const int32 count );
	private:
		boost::weak_ptr<MtPlayer> player_;
		Packet::PlayerStage* default_stage_ = nullptr;
		boost::shared_ptr<Packet::RealTimeHookEfficient> efficient_;
		uint64 dead_timer_ = 0;
		int32 battle_timer_ = 0;
		std::tuple<int32, std::vector<Packet::ActorFullInfo>> capture_stage_info_;
	};
}
#endif // MTONLINE_GAMESERVER_MT_PLAYER_STAGE_H__
