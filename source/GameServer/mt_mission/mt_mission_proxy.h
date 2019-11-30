#ifndef MTONLINE_GAMESERVER_MT_MISSION_PROXY_H__
#define MTONLINE_GAMESERVER_MT_MISSION_PROXY_H__
#include "../mt_types.h"
#include <vector>
#include "object_pool.h"
#include "MissionConfig.pb.h"
#include "MissionPacket.pb.h"


namespace Mt
{
	class MtMissionProxy : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtMissionProxy(){
			ZXERO_ASSERT(false) << "warning!!!!";
		}
		MtMissionProxy(MtPlayer& player);
		~MtMissionProxy();


		////////mission begin
		Packet::ResultCode OnAcceptGradeUpMission(const int32 hero_id);
		//Packet::ResultCode OnAcceptDayKillMission();
		//Packet::ResultCode OnAcceptLevelMission();
		void OnAcceptMission(Config::MissionType type);
		//void OnFinishAcceptMission(Config::MissionType type);
		void LoadMission(MtMission* mission);
		void InitMission();
		std::map<int32, MtMission*>& GetMissionMap();
		void InsertMission(int32 mission_id, MtMission* mission, bool load = false);
		MtMission* GetMissionById(int32 missionid);
		bool MissionGetCheck(Config::MissionType type);	//任务检查接口
		bool MissionIdCheck(const int32 mission_id);
		void SendSyncMissionProc(const int64 src_guid, Config::MissionType type);	//同步任务进度请求
		int32 OnSyncMissionAgree(const uint32 time_mark, const uint64 agree_guid);
		int32 OnSyncMissionDisagree(const uint32 time_mark, const uint64 disagree_guid);
		void OnMissionSync(Config::MissionType type);
		Packet::ResultCode OnAcceptCheck(Config::MissionType type);
		Packet::ResultCode OnMemberCountCheck(std::vector<uint64> &players, int32 count_limit);

		bool OnTeamSyncCheck(Config::MissionType type);
		std::vector<int32> GetMissionParam(Config::MissionType type);
		void OnAddMission(const int32 mission_id);
		void OnCloneMission(MtPlayer* target_player, const int32 mission_id);
		void OnCloneMissionFromLeader();
		void Mission2Member(const int32 mission_id);
		void OnFinishMission(const int32 mission_id);
		void OnDropMission(const int32 mission_id);
		void OnMissionFight(const int32 mission_id);
		void OnNextMainMission();
		void OnResetMission(Config::MissionType type);
		void OnMissionRemoveByType(Config::MissionType type);
		void OnMissionRemove(MtMission* mission, Packet::MissionState state);
		void OnMissionReward(const int32 mission_id);
		bool OnTeamArrangeReward(const int32 mission_id);
		void OnUpdateMission(const int32 mission_id);
		void MissionSyncTimer(const int32 delta);
		void TeamLeaderExtraReward(const Config::MissionConfig * config, const int32 base_exp);

	private:
		bool CheckAllVoteOk();
		bool CheckAllVote();

	private:
		std::map<uint64,int32> mission_sync_vote_;
		int32 missionsyc_time_ = 0;
		Config::MissionType sync_type_ = Config::MissionType::InvalidMission;
		std::map<int32, MtMission*> mission_map_;
		const boost::weak_ptr<MtPlayer> player_;

	};

}
#endif // MTONLINE_GAMESERVER_MT_MISSION_H__
