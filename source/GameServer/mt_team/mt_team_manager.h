#ifndef MTTEAM_MANAGER_H__
#define MTTEAM_MANAGER_H__
#include "../mt_types.h"
#include "mutex.h"
namespace Mt
{
#define TeamLevelLimitTen	10
	class MtTeamManager : public boost::noncopyable
	{
	public:
		static MtTeamManager& Instance();
		static void lua_reg(lua_State* ls);
		int32	OnLoad();
		MtTeamManager();
		~MtTeamManager();
	public:
		bool OnChangeTeamTypeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ChangeTeamTypeReq>& req, int32 /*source*/);
		bool OnChangeTeamArrangeTypeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ChangeTeamArrangeTypeReq>& req, int32 /*source*/);
		bool OnTeamMemberChangeScene(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::TeamMemberChangeScene>& msg, int32 /*source*/);
		bool OnChatMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::TeamChat>& msg, int32 /*source*/);
		bool OnPlayerAutoMatch(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::PlayerAutoMatch>& msg, int32 /*source*/);
		
		uint64 CreateTeam(MtPlayer* player_, bool sync2client);
		void SyncTeamInfoAfterCreate(Packet::TeamInfo* team, MtPlayer* player);
		bool OnCreateTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreateTeam>& message, int32 /*source*/);
		bool OnCreateRobotTeam(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::CreateRobotTeam>& message, int32 /*source*/);
		bool OnDelTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DelTeam>& message, int32 /*source*/);
		bool OnPlayerApplyTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyTeamReq>& message, int32 /*source*/);
		void PlayerApplyTeam(MtPlayer* player_,uint64 teamid);
		bool OnAgreeApplyTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AgreeApplyTeamReq>& message, int32 /*source*/);
		std::string OnAgreeApplyTeam(MtPlayer* player_, Packet::TeamInfo* teaminfo, int32 index = -1);
		bool OnInviteyAddTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAddTeamReq>& req, int32 /*source*/);
		void SendInviteyTeamToPlayer(MtPlayer* target_player,MtPlayer* source_player, Packet::TeamInfo* teaminfo);
		bool OnInviteyAgreeTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAgreeTeamReq>& message, int32 /*source*/);
		
		bool OnPlayerLeaveTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::LevelTeamReq>& message, int32 /*source*/);
		bool OnAskLevelTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskLevelTeamReq>& message, int32 /*source*/);
		void PlayerLeaveTeam(MtPlayer* player_);

		bool OnSetPurposeInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetPurposeInfoReq>& message, int32 /*source*/);
		bool OnSetTeamAutoMatchReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetTeamAutoMatchReq>& message, int32 /*source*/);
		bool OnTeamInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::TeamInfoReq>& message, int32 /*source*/);
		bool OnFindTeamListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindTeamListReq>& message, int32 /*source*/);
		bool OnTeamApplyListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::TeamApplyListReq>& message, int32 /*source*/);
		void SendTeamApplyList(MtPlayer* player_, Packet::TeamInfo* teaminfo);
		
		void SetTeamLeader(Packet::TeamInfo* teaminfo, uint64 playerid,bool charge = true);
		bool OnSetTeamLeaderReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetTeamLeaderReq>& message, int32 /*source*/);
		bool OnApplyTeamLeaderReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyTeamLeaderReq>& message, int32 /*source*/);
		bool OnAskApplyTeamLeaderReply(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskApplyTeamLeaderReply>& message, int32 /*source*/);
		void ClearApplyLeader(Packet::TeamInfo* teaminfo);
		bool OnSetTeamHeroPositionReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetTeamHeroPositionReq>& message, int32 /*source*/);
	public:
		int32 ApplyCheckAutoMatch(Packet::TeamInfo* teaminfo, MtPlayer* player_);
		bool OnPlayerAutoMatchReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerAutoMatchReq>& message, int32 /*source*/);
		void OnClearPlayerAutoMatch(uint64 playerid);
		void AddPlayerAutoMatch(int32 purpose, MtPlayer* player_);
		bool PlayerIsAutoMatch(uint64 playerid);
		bool OnPlayerSetTeamStatusReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerSetTeamStatusReq>& message, int32 /*source*/);
		void OnPlayerBattleEnd(boost::shared_ptr<MtPlayer> player);
		void SendTeamAutoMatch(Packet::TeamInfo* teaminfo, MtPlayer* player_);
		void SendPlayerAutoMatch(MtPlayer* player_,bool automatch,int32 purpose);
		void S2GSyncTeamMemberLevel(Packet::TeamInfo* teaminfo, uint64 playerid, int32 playerlevel);
	private:
		int32 TeamSize(const Packet::TeamInfo* teaminfo);
		bool isLeader(Packet::TeamInfo* teaminfo, uint64 playerid);
		bool isMember(Packet::TeamInfo* teaminfo, uint64 playerid);
		bool IsFull(const Packet::TeamInfo* teaminfo);
		int32 CheckAutoMatch(const Packet::PlayerAutoMatchInfo &playerinfo, Packet::TeamInfo* teaminfo);
		int32 GetDefenceCount(Packet::TeamInfo* teaminfo);
		int32 GetCureCount(Packet::TeamInfo* teaminfo);
		int32 GetAttackCount(Packet::TeamInfo* teaminfo);
		void ResetAutomatchPosition(Packet::TeamInfo* teaminfo);
		void AutomatchPosition(Packet::TeamInfo* teaminfo);
		void SetAutomatchPositionCount(Packet::TeamInfo* teaminfo,int32 position,int32 count);
		const boost::shared_ptr<Packet::TeamAutomatchConfig> GetTeamAutomatchConfig(bool group,int32 dc,int32 cc,int32 ac);
		void ResetPurpose(Packet::TeamInfo* teaminfo);
	
	public: 
		void OnBigTick(uint64 elapseTime);
		void FillTeamInfoReply(Packet::TeamInfo* teaminfo, Packet::TeamInfoReply& info);
		void FillTeamPlayerInfo(const Packet::TeamPlayerInfo &teamplayerinfo, boost::shared_ptr<MtPlayer> player_,Packet::TeamFullPlayerInfo& info);
		void FillTeamPlayerBasicInfo(Packet::TeamPlayerBasicInfo &baseinfo, MtPlayer* player_,  Packet::PlayerTeamStatus status,int32 position_index);
		bool FillPlayerAutoMatchInfo(int32 purpose,MtPlayer* player_, Packet::PlayerAutoMatchInfo& info);
		//不要暴露任何Broadcast
		//void BroadcastTeamEx(uint64 teamid, bool follow_state, const google::protobuf::Message& msg);
		//void BroadcastTeam(uint64 teamid, const std::vector<uint64>& filter_list, const google::protobuf::Message& msg);

	private:
		Packet::TeamInfo* GetTeamInfo(uint64 teamid);
		void TBroadcastTeam(Packet::TeamInfo* team, const google::protobuf::Message& msg, bool follow_state=false);
		void TBroadcastTeam(Packet::TeamInfo* team, const std::vector<uint64>& filter_list, const google::protobuf::Message& msg);
		void FollowLeader(Packet::TeamInfo* teaminfo, MtPlayer* player);
		
		void SyncPlayerTeamDataToPlayer(Packet::TeamInfo* teaminfo);
		void ResetPlayerTeamDataToPlayer(MtPlayer* player);
		int32 TeamCapacity(const Packet::TeamInfo* teaminfo);
	private:
		std::vector<boost::shared_ptr<Packet::TeamInfo>> teamlist_;	//所有的team管理
		std::vector<Packet::PlayerAutoMatchInfo> automatchplayer_;		//玩家自动匹配列表
		mutable std::mutex mutex_;
	private:
		std::vector<boost::shared_ptr<Packet::TeamAutomatchConfig>> automacthconfig;
	};
}
#endif // MTTEAM_MANAGER_H__
