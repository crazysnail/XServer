#ifndef MTONLINE_GAMESERVER_BATTLE_ROOM_H__
#define MTONLINE_GAMESERVER_BATTLE_ROOM_H__
#include "../mt_types.h"
#include "object_pool.h"
#include <BattleField.pb.h>

namespace Mt
{
	class MtBattleField : public boost::noncopyable
	{
	public:
		//
		static void lua_reg(lua_State* ls);
	public:

		MtBattleField();
		~MtBattleField();
		std::vector<MtPlayer*> GetPlayers();

		bool CheckLevel(const int32 level);
		bool IsFull();
		bool IsEmpty();
		bool IsHasPlayer(const uint64 guid);
		bool OnJoin(MtPlayer * player);
		bool OnLeave(MtPlayer * player);
		bool OnJoinBattle(const uint64 guid);
		bool OnLeaveBattleField(MtPlayer * player);
		int CheckCamp(const uint64 guid);		
		void OnBeginPickFlag(const uint64 guid);
		void OnSuccessPickFlag(MtPlayer * player);
		void OnCancelPickFlag(MtPlayer * player);
		void OnSuccessWinFlag(MtPlayer * player);
		void OnBattleOver(MtPlayer * winner, MtPlayer * losser);
		void OnBFOver();

		void OnForceBegin();
		void OnBattlePrepare();
		void OnBattleEnter();
		void OnBeginBattleField();

		int32 GetId() { return raid_rtid_; };
		void SetId(const int32 raid_rtid) { raid_rtid_ = raid_rtid; };
	
		int32 GetSceneId() { return room_info_.scene_id(); };
		Packet::BattleRoomInfo& GetBattleRoomInfo() { return room_info_; };
		Packet::BattleFieldInfo& GetBattleFieldInfo() { return bf_info_; };		

		Packet::BattleFieldState GetBattleFieldState() { return room_info_.bf_state(); };
		void SetBattleFieldState(Packet::BattleFieldState bf_state) { room_info_.set_bf_state(bf_state); };
		
		Packet::BattleFieldPlayerInfo* GetBattlePlayerInfo(const uint64 guid);

		void OnSecondTick(int32 elapse_second);
		void OnBigTick(uint64 elapseTime);

		//更新房间信息
		void BroadcastMsg(const google::protobuf::Message& msg, const int32 camp = -1, const std::vector<uint64>& filter_list = {INVALID_GUID});
		static int32 GetPosIndex(const int32 camp, int32 flag = -1);
		void reset();
		void ResultCalc();
		void EndAllBattle();
		void OnGoGrave(MtPlayer * player);
		int32 GetWaitingTime() { return wait_timer_; };
		void OnDropFlag(MtPlayer * player, bool iswin);
		

	private:
		void AlignPlayers();
		void PickFlagCheck(std::map<uint64, int32>& queue, int32 elapse_second);
		void ReliveCheck(int32 elapse_second);
		void EndCheck(int32 elapse_second);
		void DoReward(int32 winner);		
		void OnPickFlag(MtPlayer * player);
		void OnRelive( MtPlayer * player);
		Packet::BattleFieldPlayerInfo*  OnBattleLoss(MtPlayer * player);
		Packet::BattleFieldPlayerInfo*  OnBattleWin(MtPlayer * player);
		//void BuffPointCheck(int32 elapse_second);		
		
	private:
		int32 raid_rtid_;
		Packet::BattleRoomInfo room_info_;		
		Packet::BattleFieldInfo bf_info_;
		int32 wait_timer_ = 0;
		int32 relive_timer_ = 0;
		int32 attacker_timer_ = 0;
		int32 defender_timer_ = 0;
		std::mutex mutex_;
		std::map<uint64, int32> attacker_flag_queue_;
		std::map<uint64, int32> defender_flag_queue_;

		//std::map<int32, uint32> buff_list_;
	};
}
#endif // MTONLINE_GAMESERVER_BATTLE_ROOM_H__
