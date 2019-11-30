#include <SceneCommon.pb.h>
#include <EnterScene.pb.h>
#include <S2GMessage.pb.h>
#include <G2SMessage.pb.h>
#include "Team.pb.h"
#include "Team.proto.fflua.h"
#include "../base/client_session.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_server/mt_server.h"
#include "mt_team_manager.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_mission/mt_mission_proxy.h"
#include "../mt_battle/mt_battle_ground.h"
#include "module.h"

namespace Mt
{
	static MtTeamManager* __mt_team_manager = nullptr;

	REGISTER_MODULE(MtTeamManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtTeamManager::OnLoad, boost::ref(MtTeamManager::Instance())));
	}

	MtTeamManager::MtTeamManager()
	{
		__mt_team_manager = this;
	}
	MtTeamManager::~MtTeamManager()
	{
	}

	MtTeamManager& MtTeamManager::Instance()
	{
		return *__mt_team_manager;
	}
	int32	MtTeamManager::OnLoad()
	{
		auto table = data_manager::instance()->get_table("teamautomatch");
		ZXERO_ASSERT(table) << "can not find table teamautomatch";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto amconfig = boost::make_shared <Packet::TeamAutomatchConfig>();
			FillMessageByRow(*amconfig, *row);
			automacthconfig.push_back(amconfig);
		}
		return 0;
	}
	void MtTeamManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtTeamManager, ctor()>(ls, "MtTeamManager")
			.def(&MtTeamManager::OnBigTick, "OnBigTick")
			.def(&MtTeamManager::SendInviteyTeamToPlayer,"SendInviteyTeamToPlayer")
			.def(&MtTeamManager::CreateTeam,"CreateTeam")
			.def(&MtTeamManager::GetTeamInfo, "GetTeamInfo")
			.def(&MtTeamManager::IsFull,"IsFull")
			.def(&MtTeamManager::ApplyCheckAutoMatch,"ApplyCheckAutoMatch")
			.def(&MtTeamManager::OnAgreeApplyTeam,"OnAgreeApplyTeam")
			.def(&MtTeamManager::SendTeamApplyList,"SendTeamApplyList")
			.def(&MtTeamManager::AutomatchPosition,"AutomatchPosition")
			.def(&MtTeamManager::AddPlayerAutoMatch,"AddPlayerAutoMatch")
			.def(&MtTeamManager::SendTeamAutoMatch,"SendTeamAutoMatch")
			.def(&MtTeamManager::SendPlayerAutoMatch, "SendPlayerAutoMatch")
			.def(&MtTeamManager::PlayerIsAutoMatch,"PlayerIsAutoMatch")
			.def(&MtTeamManager::OnClearPlayerAutoMatch,"OnClearPlayerAutoMatch")
			.def(&MtTeamManager::PlayerLeaveTeam,"PlayerLeaveTeam")
			.def(&MtTeamManager::ResetPlayerTeamDataToPlayer,"ResetPlayerTeamDataToPlayer")
			.def(&MtTeamManager::S2GSyncTeamMemberLevel,"S2GSyncTeamMemberLevel");
		ff::fflua_register_t<>(ls)
			.def(&MtTeamManager::Instance, "LuaMtTeamManager");
	}
	bool MtTeamManager::OnCreateTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::CreateTeam>& message, int32 /*source*/)
	{
		message;
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		//CreateTeam(player.get());
		try {
			thread_lua->call<void>(8, "OnCreateTeam", player);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
		}
		return true;
	}

	bool MtTeamManager::OnCreateRobotTeam(
		const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::CreateRobotTeam>& message, int32 /*source*/)
	{
		message;
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		if (player->GetTeamID() != INVALID_GUID) {
			return true;
		}
		auto team_id = CreateTeam(player.get(), false);
		if (team_id != INVALID_GUID) {
			auto team = GetTeamInfo(team_id);
			if (team == nullptr)
				return true;
			team->set_group(true);
			team->mutable_purpose()->set_level_limt_min(1);
			team->mutable_purpose()->set_level_limt_max(120);
			team->set_robot_team(true);
			team->set_arrangetype(Packet::ArrangeType::AT_Leader);

			if (TeamCapacity(team) - TeamSize(team) < message->basic_info_size()) {
				LOG_ERROR << "[team_manager] add robot player fail,player:"
					<< player->Guid() << ",cap:" << TeamCapacity(team)
					<< ",size:" << TeamSize(team);
				return true;
			}
			for (auto& robot : message->basic_info()) {
				uint64 playerid = robot.guid();
				if (isMember(team, playerid))
					return true;
				Packet::PlayerTeamStatus status = Packet::PlayerTeamStatus::Team_Follow;

				for (int32 i = 0; i < team->member_size(); i++)
				{
					auto member = team->mutable_member(i);
					if (member->playerguid() == INVALID_GUID)
					{
						member->set_playerguid(playerid);
						member->mutable_basic_info()->CopyFrom(robot);
						member->mutable_teambaseinfo()->set_playerstatus(status);
						member->mutable_teambaseinfo()->set_position_index(i);
						member->mutable_teambaseinfo()->set_hero_position(0);
						member->set_is_robot(true);
						break;
					}
				}
/*
				Packet::AddTeamReply addreply;
				addreply.set_playerguid(playerid);
				addreply.set_playername(robot.name());
				TBroadcastTeam(team, addreply);*/
				ResetAutomatchPosition(team);
			}
			SyncTeamInfoAfterCreate(team, player.get());
			SyncPlayerTeamDataToPlayer(team);
			AutomatchPosition(team);
			auto notify = boost::make_shared<Packet::RobotPlayerInOk>();
			notify->mutable_basic_info()->CopyFrom(message->basic_info());
			notify->mutable_one_key_id()->CopyFrom(message->one_key_id());
			player->ExecuteMessage(notify);
		}
		return true;
	}

	uint64 MtTeamManager::CreateTeam(MtPlayer* player_, bool sync2client)
	{
		auto playerid = player_->Guid();
		auto teaminfo = boost::make_shared<Packet::TeamInfo>();
		if (teaminfo == nullptr)
			return INVALID_GUID;
		// [4/10/2018 Ryan] 由于异步问题,这个时候玩家可能已经有队伍了,需要遍历一遍判断
		for (auto& team : teamlist_) {
			for (auto& team_m : team->member()) {
				if (team_m.playerguid() == INVALID_GUID) {
					break;
				}
				if (team_m.playerguid() == player_->Guid()) {
					LOG_ERROR << "[team_manager] create_team,player"
						<< player_->Guid()
						<< ",old team:" << team->guid();
					return INVALID_GUID;
				}
			}
		}
		uint64 teamguid = MtGuid::Instance()(*teaminfo);
		teaminfo->set_guid(teamguid);
		teaminfo->set_group(false);
		teaminfo->set_arrangetype(Packet::ArrangeType::AT_Roll);
		ClearApplyLeader(teaminfo.get());
		auto leader = teaminfo->add_member();
		leader->set_playerguid(playerid);
		SetTeamLeader(teaminfo.get(), playerid,false);
		FillTeamPlayerBasicInfo(*leader->mutable_teambaseinfo(), player_, Packet::PlayerTeamStatus::Team_Follow,0);
		for (int32 i=1;i<10;i++)
		{
			auto member = teaminfo->add_member();
			member->set_playerguid(INVALID_GUID);
			FillTeamPlayerBasicInfo(*member->mutable_teambaseinfo(),nullptr, Packet::PlayerTeamStatus::Team_Follow,i);
		}
		teaminfo->set_auto_match(false);
		teaminfo->set_setposition(false);
		auto purpose = teaminfo->mutable_purpose();
		purpose->set_purpose(0);
		purpose->set_level_limt_min(10);
		purpose->set_level_limt_max(120);
		teamlist_.push_back(teaminfo);
		if (sync2client)
			SyncTeamInfoAfterCreate(teaminfo.get(), player_);
		LOG_INFO << "[team_manager] create_team,player:" << player_->Guid()
			<< ",team created:" << teamguid;
		return teamguid;
	}

	void MtTeamManager::SyncTeamInfoAfterCreate(Packet::TeamInfo* team, MtPlayer* player)
	{
		if (team == nullptr || player == nullptr)
			return;
		Packet::TeamInfoReply inforeply;
		FillTeamInfoReply(team, inforeply);
		player->SendMessage(inforeply);

		Packet::AddTeamReply addreply;
		addreply.set_playerguid(player->Guid());
		addreply.set_playername(player->Name());
		player->SendMessage(addreply);

		SyncPlayerTeamDataToPlayer(team);
	}

	bool MtTeamManager::OnDelTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::DelTeam>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 playerid = player_->Guid();
		uint64 teamid = player_->GetTeamID();
		if (teamid == INVALID_GUID)
		{
			return true;// Packet::ResultCode::Team_Out;
		}
			
		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
			return true;// Packet::ResultCode::Team_Out;
		if (teaminfo->leader() != playerid) //如果不是队长
			return true;// Packet::ResultCode::Team_NotLeader;
		if (player_->GetCurBattleType() != Packet::BattleGroundType::NO_BATTLE)
		{
			return true;// Packet::ResultCode::Team_CreateInBattle;
		}
		for (auto member : teaminfo->member())
		{
			if (member.has_basic_info()) {
				//ignore
			} else {
				auto memberplayer = Server::Instance().FindPlayer(member.playerguid());
				if (memberplayer)
				{
					ResetPlayerTeamDataToPlayer(memberplayer.get());
				}
			}

		}
		teamlist_.erase(std::remove_if(std::begin(teamlist_), 
			std::end(teamlist_), 
			boost::bind(&Packet::TeamInfo::guid,_1) == teamid),
			teamlist_.end());
		return true;// Packet::ResultCode::ResultOK;
	}
	int32 MtTeamManager::ApplyCheckAutoMatch(Packet::TeamInfo* teaminfo, MtPlayer* player_)
	{
		if (player_ == nullptr || teaminfo == nullptr)
		{
			return -1;
		}
		Packet::PlayerAutoMatchInfo info;
		if (FillPlayerAutoMatchInfo(teaminfo->purpose().purpose(), player_, info) == false)
			return -2;//Packet::ResultCode::UnknownError;
		return CheckAutoMatch(info, teaminfo);
	}
	bool MtTeamManager::OnPlayerApplyTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyTeamReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		PlayerApplyTeam(player_.get(),message->guid());
		return true;
	}
	void MtTeamManager::PlayerApplyTeam(MtPlayer* player_, uint64 teamid)
	{
		if (player_ == nullptr)
			return;
		try {
			thread_lua->call<void>(8, "OnPlayerApplyTeam", player_, teamid);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
		}
	}
	bool MtTeamManager::OnAgreeApplyTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AgreeApplyTeamReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		try {
			thread_lua->call<void>(8, "OnAgreeApplyTeamReq", player_, message);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
		}
		return true;
	}
	std::string MtTeamManager::OnAgreeApplyTeam(MtPlayer* player_, Packet::TeamInfo* teaminfo, int32 index )
	{
		std::lock_guard<std::mutex> guard(mutex_);
		if (teaminfo == nullptr)
			return "Team_NoExist";
		if (teaminfo->robot_team())//不能进入用于引导的队伍
			return "Team_CannotAddPlayerToTeam";
		uint64 playerid = player_->Guid();
		if (player_->GetTeamID() != INVALID_GUID)
			return "Team_In";
		if (isMember(teaminfo, playerid))
			return "Team_In";
		if (IsFull(teaminfo))
		{
			return "Team_Full";
		}
		Packet::PlayerTeamStatus status = Packet::PlayerTeamStatus::Team_Follow;
		Packet::BattleGroundType bgtype = player_->GetCurBattleType();
		if (bgtype != Packet::BattleGroundType::NO_BATTLE && bgtype != Packet::BattleGroundType::PVE_HOOK)
		{
			status = Packet::PlayerTeamStatus::Team_AFKING;
		}

		if (index == -1)
		{
			for (int32 i = 0; i < teaminfo->member_size(); i++)
			{
				auto member = teaminfo->mutable_member(i);
				if (member->playerguid() == INVALID_GUID)
				{
					member->set_playerguid(playerid);
					FillTeamPlayerBasicInfo(*member->mutable_teambaseinfo(), player_, status,i);
					break;
				}
			}
		}
		else
		{
			for (int32 i = 0; i < teaminfo->member_size(); i++)
			{
				auto member = teaminfo->mutable_member(i);
				if (member->playerguid() == INVALID_GUID && i == index)
				{
					member->set_playerguid(playerid);
					FillTeamPlayerBasicInfo(*member->mutable_teambaseinfo(), player_, status,i);
					break;
				}
			}
		}
		if (status == Packet::PlayerTeamStatus::Team_Follow)
		{
			FollowLeader(teaminfo, player_);
		}
		
		Packet::TeamInfoReply inforeply;
		FillTeamInfoReply(teaminfo, inforeply);
		TBroadcastTeam(teaminfo,inforeply);

		Packet::AddTeamReply addreply;
		addreply.set_playerguid(playerid);
		addreply.set_playername(player_->Name());
		TBroadcastTeam(teaminfo, addreply);
		ResetAutomatchPosition(teaminfo);
		AutomatchPosition(teaminfo);

		SyncPlayerTeamDataToPlayer(teaminfo);
		return "";
	}
	void MtTeamManager::FollowLeader(Packet::TeamInfo* teaminfo, MtPlayer* player)
	{
		if (teaminfo == nullptr || player == nullptr)
			return;
		//传送到leader场景位置
		auto leader = Server::Instance().FindPlayer(teaminfo->leader());
		if (leader != nullptr) {
			//auto player_scene = player_->Scene();
			auto player_scene = MtSceneManager::Instance().GetSceneById(player->GetSceneId());
			if (player_scene != nullptr){
				auto& init_pos = leader->Position();
				//std::vector<int32> pos = { init_pos.x(),init_pos.y(),init_pos.z()};
				if (MtScene::IsRaidScene(leader->GetSceneId())) {
					auto raid = leader->GetRaid();
					if (raid != nullptr) {
						Server::Instance().SendG2SCommonMessage(player, "G2SGoRaid", { leader->GetSceneId(), raid->GetScriptId(), raid->RuntimeId(),-1,init_pos.x(),init_pos.y(),init_pos.z() }, {}, {});
						//player->GoRaid(leader->GetSceneId(), raid->GetScriptId(), raid->RuntimeId(),-1, pos);
					}
					else {
						ZXERO_ASSERT(false) << "leader raid is null !!! guid=" << player->Guid();
						return;
					}					
				}
				else {
					Server::Instance().SendG2SCommonMessage(player, "G2SGoTo", { leader->GetSceneId(), init_pos.x(),init_pos.y(),init_pos.z() }, {}, {});
					//player->GoTo(leader->GetSceneId(), pos);
				}
			}
		}
	}
	bool MtTeamManager::OnInviteyAddTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAddTeamReq>& req, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr)
			return true;
		try {
			thread_lua->call<void>(8, "OnInviteyPlayer", player, req->playerguid());
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
		}
		return true;
		
	}
	void MtTeamManager::SendInviteyTeamToPlayer(MtPlayer* target_player, MtPlayer* source_player, Packet::TeamInfo* teaminfo)
	{
		if (target_player && teaminfo && source_player)
		{
			Packet::AskInviteyAgreeTeamReply reply;
			reply.set_teamid(teaminfo->guid());
			reply.set_teaminvitename(source_player->GetScenePlayerInfo()->name());
			reply.set_teaminviteguid(source_player->Guid());
			reply.set_purpose(teaminfo->purpose().purpose());
			target_player->SendMessage(reply);
		}
	}
	bool MtTeamManager::OnInviteyAgreeTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::InviteyAgreeTeamReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr)
			return true;
		try {
			thread_lua->call<void>(8, "OnInviteyAgreeTeamReq", player, message);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
		}
		return true;
	}

	bool MtTeamManager::OnPlayerLeaveTeam(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::LevelTeamReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		PlayerLeaveTeam(player_.get());
		return true;// Packet::ResultCode::ResultOK;
	}
	bool MtTeamManager::OnAskLevelTeamReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskLevelTeamReq>& message, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 teamid = player->GetTeamID();
		if (teamid == INVALID_GUID) {
			return true;//Team_IdNil
		}
		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr) {
			return true;//Team_NotExist
		}
		if (teaminfo->robot_team()) {
			player->SendClientNotify("Team_CanNotFirePlayer", -1, -1);
			return true;
		}
		auto askplayer = Server::Instance().FindPlayer(message->playerguid());
		if (askplayer == nullptr)
		{
			player->SendCommonResult(Packet::ResultOption::Team_Opt, Packet::ResultCode::UnknownError);
			return true;
		}
		PlayerLeaveTeam(askplayer.get());

		return true;
	}
	
	void MtTeamManager::PlayerLeaveTeam(MtPlayer* player_)
	{
		if (player_ == nullptr)
			return;
		std::lock_guard<std::mutex> guard(mutex_);
		uint64 playerid = player_->Guid();
		uint64 teamid = player_->GetTeamID();
		if (teamid == INVALID_GUID)
			return ;//Team_Out
		
		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
		{
			ResetPlayerTeamDataToPlayer(player_);
			return;//Team_NoExist
		}
		bool leader = isLeader(teaminfo, playerid);
		if (teaminfo->robot_team() && leader == false)
			return;//不能踢掉机器人
		if (player_->GetCurBattleType() != Packet::BattleGroundType::NO_BATTLE)
		{
			if (leader)
			{
				player_->SendClientNotify("Team_CreateInBattle", -1, -1); 
				return;
			}
			else
			{
				//for (int32 index = 0; index < teaminfo->member_size(); ++index)
				//{
				//	auto member = teaminfo->mutable_member(index);
				//	if (member->playerguid() == playerid)
				//	{
				//		member->mutable_teambaseinfo()->set_playerstatus(Packet::PlayerTeamStatus::Team_AFKING);
				//		break;
				//	}
				//}
				return ;// Packet::ResultCode::Team_InBattleOpt;
			}
		}
		ResetPlayerTeamDataToPlayer(player_);
		for (int32 index = 0; index < teaminfo->member_size(); ++index)
		{
			auto member = teaminfo->mutable_member(index);
			if (member->playerguid() == playerid)
			{
				member->set_playerguid(INVALID_GUID);
				FillTeamPlayerBasicInfo(*member->mutable_teambaseinfo(), nullptr, Packet::PlayerTeamStatus::Team_Follow, index);
			}
		}
		if (TeamSize(teaminfo) > 0 && teaminfo->robot_team() == false)
		{
			if (leader)
			{
				for (int32 index = 0; index < teaminfo->member_size(); ++index)
				{
					auto memberplayerguid = teaminfo->member(index).playerguid();
					if (memberplayerguid != INVALID_GUID)
					{
						SetTeamLeader(teaminfo, memberplayerguid);
						break;
					}
				}
			}
			Packet::TeamInfoReply inforeply;
			FillTeamInfoReply(teaminfo, inforeply);
			TBroadcastTeam(teaminfo, inforeply);

			Packet::LeaveTeamReply leavereply;
			leavereply.set_playerguid(playerid);
			leavereply.set_playername(player_->Name());
			TBroadcastTeam(teaminfo, leavereply);
			ResetAutomatchPosition(teaminfo);
			AutomatchPosition(teaminfo);
			SyncPlayerTeamDataToPlayer(teaminfo);
		}
		else
		{
			LOG_INFO << "[team_manager] player_leave,player:" << player_->Guid()
				<< ",team dismiss:" << teamid;
			teamlist_.erase(
				std::remove_if(std::begin(teamlist_), std::end(teamlist_), [teamid](auto& iter) {return iter->guid() == teamid;	})
				, teamlist_.end());

			Packet::LeaveTeamReply leavereply;
			leavereply.set_playerguid(playerid);
			leavereply.set_playername(player_->Name());
			player_->SendMessage(leavereply);
		}
		Packet::EmptyTeamReply inforeply;
		player_->SendMessage(inforeply);


		//重置暮光军团任务
		auto scene = player_->Scene();
		if (scene != nullptr) {
			auto proxy = player_->GetMissionProxy();
			if (proxy != nullptr) {
				scene->QueueInLoop(boost::bind(&MtMissionProxy::OnResetMission, proxy, Config::MissionType::CorpsMission));
			}
		}
	}
	Packet::TeamInfo* MtTeamManager::GetTeamInfo(uint64 teamid)
	{
		auto it = std::find_if(std::begin(teamlist_), std::end(teamlist_), [=](auto& iter)
		{
			return iter->guid() == teamid;
		});
		if (it != std::end(teamlist_))
		{
			return (*it).get();
		}
		return nullptr;
	}

	int32 MtTeamManager::TeamSize(const Packet::TeamInfo* teaminfo)
	{
		return std::count_if(teaminfo->member().begin(),
			teaminfo->member().end(),
			boost::bind(&Packet::TeamPlayerInfo::playerguid, _1) != INVALID_GUID);
	}

	bool MtTeamManager::OnSetPurposeInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetPurposeInfoReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;

		uint64 playerid = player_->Guid();
		uint64 teamid = player_->GetTeamID();
		if (teamid == INVALID_GUID)
		{
			player_->SendClientNotify("Team_Out", -1, -1);
			return true;
		}
		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
		{
			player_->SendClientNotify("Team_NoExist", -1, -1);
			return true;
		}
		if (teaminfo->robot_team()) {
			player_->SendClientNotify("Team_CannotChangeSetting", -1, -1);
			return true;
		}
		if (teaminfo->leader() != playerid) //如果不是队长
		{
			player_->SendClientNotify("Team_NotLeader", -1, -1);
			return true;
		}

		teaminfo->mutable_purpose()->CopyFrom(message->purpose());
		Packet::PurposeInfoUpdate update;
		update.mutable_purpose()->CopyFrom(teaminfo->purpose());
		player_->SendMessage(update);
		return true;
	}

	bool MtTeamManager::OnSetTeamAutoMatchReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetTeamAutoMatchReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		try {
			thread_lua->call<void>(8, "OnSetTeamAutoMatchReq", player_, message);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return true;
		}
		return true;
	}
	void MtTeamManager::SendTeamAutoMatch(Packet::TeamInfo* teaminfo, MtPlayer* player_)
	{
		if (teaminfo == nullptr || player_ == nullptr)
			return;
		Packet::TeamAutoMatchUpdate update;
		update.set_auto_match(teaminfo->auto_match());
		player_->SendMessage(update);
	}
	void MtTeamManager::ResetAutomatchPosition(Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr)
			return;
		if (teaminfo->setposition() == true)
			return;
		for (int32 i = 0; i < teaminfo->member_size(); i++)
		{
			auto member = teaminfo->mutable_member(i);
			if (member->playerguid() == INVALID_GUID && member->teambaseinfo().hero_position() != 0)
			{
				member->mutable_teambaseinfo()->set_hero_position(0);
			}
		}
	}
	void MtTeamManager::AutomatchPosition(Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr)
			return;
		if (teaminfo->setposition() == true)
			return;
		auto config = GetTeamAutomatchConfig(teaminfo->group(),GetDefenceCount(teaminfo), GetCureCount(teaminfo), GetAttackCount(teaminfo));
		if (config != nullptr)
		{
			int32 need_attack_count = config->need_attack_count();
			int32 need_cure_count = config->need_cure_count();
			int32 need_defence_count = config->need_defence_count();
			SetAutomatchPositionCount(teaminfo, 1, need_attack_count);
			SetAutomatchPositionCount(teaminfo, 3, need_defence_count);
			SetAutomatchPositionCount(teaminfo, 4, need_cure_count);
		}
	}
	void MtTeamManager::SetAutomatchPositionCount(Packet::TeamInfo* teaminfo, int32 position, int32 count)
	{
		if (teaminfo == nullptr || count == 0)
			return;
		int32 maxcount = 5;
		if (teaminfo->group())
		{
			maxcount = 10;
		}
		for (int32 i = 0; i < teaminfo->member_size() && i < maxcount; i++)
		{
			if (count <= 0 )
			{
				return;
			}
			auto member = teaminfo->mutable_member(i);
			if (member->playerguid() == INVALID_GUID && member->teambaseinfo().hero_position() == 0)
			{
				auto memberinfo = member->mutable_teambaseinfo();
				memberinfo->set_hero_position(position);
				Packet::SetTeamHeroPositionReply reply;
				reply.set_hero_position(position);
				reply.set_position_index(i);
				TBroadcastTeam(teaminfo, reply);
				count--;
			}
		}
	}
	bool MtTeamManager::OnChangeTeamTypeReq(const boost::shared_ptr<client_session>& session,  const boost::shared_ptr<Packet::ChangeTeamTypeReq>& req, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr)
			return true;

		uint64 playerid = player->Guid();
		uint64 teamid = player->GetTeamID();
		if (teamid == INVALID_GUID)
			return true;

		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
			return true;

		if (teaminfo->leader() != playerid) //如果不是队长
		{
			player->SendClientNotify("team_not_leader", -1,-1);
			return true;
		}
		if (teaminfo->robot_team()) {
			player->SendClientNotify("Team_CannotChangeSetting", -1, -1);
			return true;
		}
			
		bool setgroup = req->group();
		if (setgroup == false)
		{
			if (TeamSize(teaminfo) > 5)
			{
				player->SendClientNotify("team_morefive" , -1, -1);
				return true;
			}
			teaminfo->set_group(false);
			//
			player->SendClientNotify("team_to_five",Packet::GUILD,-1);
		}
		else
		{
			teaminfo->set_group(true);
			//
			player->SendClientNotify("team_to_ten", Packet::GUILD, -1);
		}
		Packet::UpdateTeamTypeReply update;
		update.set_group(teaminfo->group());
		TBroadcastTeam(teaminfo, update);
		ResetPurpose(teaminfo);
		ResetAutomatchPosition(teaminfo);
		AutomatchPosition(teaminfo);
		SyncPlayerTeamDataToPlayer(teaminfo);
		return true;
	}
	bool MtTeamManager::OnChangeTeamArrangeTypeReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ChangeTeamArrangeTypeReq>& req, int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr)
			return true;

		uint64 playerid = player->Guid();
		uint64 teamid = player->GetTeamID();
		if (teamid == INVALID_GUID)
			return true;

		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
			return true;

		if (teaminfo->leader() != playerid) //如果不是队长
		{
			player->SendClientNotify("team_not_leader", -1, -1);
			return true;
		}
		if (teaminfo->robot_team()) {
			player->SendClientNotify("Team_CannotChangeSetting", -1, -1);
			return true;
		}
		teaminfo->set_arrangetype(req->type());
		if (req->type() == Packet::ArrangeType::AT_Leader)
		{
			player->SendClientNotify("team_arrange_change|team_arrange_leader", -1, -1);
			player->SendClientNotify("team_arrange_change|team_arrange_leader", Packet::TEAM, -1);
		}
		else if (req->type() == Packet::ArrangeType::AT_Roll)
		{
			player->SendClientNotify("team_arrange_change|team_arrange_roll", -1, -1);
			player->SendClientNotify("team_arrange_change|team_arrange_roll", Packet::TEAM, -1);
		}
		Packet::UpdateTeamArrangeTypeReply update;
		update.set_type(teaminfo->arrangetype());
		TBroadcastTeam(teaminfo, update);
		SyncPlayerTeamDataToPlayer(teaminfo);
		return true;
	}
	bool MtTeamManager::OnTeamInfoReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::TeamInfoReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 teamid = player_->GetTeamID();
		if (teamid == INVALID_GUID)
		{
			Packet::EmptyTeamReply inforeply;
			player_->SendMessage(inforeply);
			return true;
		}
			
		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
			return true;

		Packet::TeamInfoReply inforeply;
		FillTeamInfoReply(teaminfo, inforeply);
		player_->SendMessage(inforeply);
		return true;
	}

	bool MtTeamManager::OnFindTeamListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::FindTeamListReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		Packet::TeamListReply reply;
		for (auto s_team : teamlist_)
		{
			uint64 leaderid = s_team->leader();
			auto leader = Server::Instance().FindPlayer(leaderid);
			if (leader)
			{
				auto team_ = reply.add_teams();
				team_->set_guid(s_team->guid());
				team_->set_purpose(s_team->purpose().purpose());
				team_->set_membersize(TeamSize(s_team.get()));
				team_->set_leadername(leader->GetScenePlayerInfo()->name());
				team_->set_leaderlevel(leader->PlayerLevel());
				team_->set_init_actor_id(leader->GetScenePlayerInfo()->init_actor_id());
			}
		}
		player_->SendMessage(reply);
		return true;
	}
	bool MtTeamManager::OnTeamApplyListReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::TeamApplyListReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 playerid = player_->Guid();
		uint64 teamid = player_->GetTeamID();
		if (teamid == INVALID_GUID)
			return true;
		auto teaminfo = GetTeamInfo(teamid);
		if (teaminfo == nullptr)
			return true;
		bool leader = isLeader(teaminfo, playerid);
		if (leader == false)
			return true;
		SendTeamApplyList(player_.get(), teaminfo);
		return true;
	}

	void MtTeamManager::SendTeamApplyList(MtPlayer* player_, Packet::TeamInfo* teaminfo)
	{
		if (player_ == nullptr || teaminfo == nullptr)
			return;
		Packet::TeamApplyListReply reply;
		for (auto applyid : teaminfo->applylist())
		{
			if (applyid != INVALID_GUID)
			{
				auto apply_ = Server::Instance().FindPlayer(applyid);
				if (apply_)
				{
					auto apply = reply.add_applylist();
					apply->CopyFrom(apply_->GetDBPlayerInfo()->basic_info());
				}
			}
		}
		auto leader = Server::Instance().FindPlayer(teaminfo->leader());
		if (leader != nullptr)
		{
			leader->SendMessage(reply);
		}
		
	}
	bool MtTeamManager::isLeader(Packet::TeamInfo* teaminfo, uint64 playerid)
	{
		if (teaminfo == nullptr)
			return false;
		if (teaminfo->leader() != playerid) //如果不是队长
			return false;
		return true;
	}

	bool MtTeamManager::isMember(Packet::TeamInfo* teaminfo, uint64 playerid)
	{
		if (teaminfo == nullptr)
			return false;
		for (auto member : teaminfo->member())
		{
			if (member.playerguid() == playerid)
			{
				return true;
			}
		}
		return false;
	}

	void MtTeamManager::SetTeamLeader(Packet::TeamInfo* teaminfo, uint64 playerid, bool charge)
	{
		auto newleader = Server::Instance().FindPlayer(playerid);
		if (teaminfo == nullptr || newleader == nullptr)
			return;
		teaminfo->set_leader(playerid);

		Packet::TeamLeaderUpdate updatemsg;
		updatemsg.set_playerguid(playerid);
		updatemsg.set_playername(newleader->Name());
		updatemsg.set_move_speed(newleader->GetMoveSpeed());
		updatemsg.set_charge(charge);
		TBroadcastTeam(teaminfo, updatemsg);
	}

	bool MtTeamManager::OnSetTeamLeaderReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetTeamLeaderReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 playerid = player_->Guid();
		uint64 memberid = message->playerguid();
		if (playerid == memberid)
			return true;
		auto teaminfo = GetTeamInfo(player_->GetTeamID());
		if (teaminfo == nullptr)
			return true;
		if (teaminfo->robot_team()) {
			player_->SendClientNotify("Team_CannotSetPlayerAsLeader", -1, -1);
			return true;
		}
		if (player_->GetCurBattleType() != Packet::BattleGroundType::NO_BATTLE)
		{
			player_->SendClientNotify("Team_InBattleOpt", -1, -1);
			return true;
		}
		if (isLeader(teaminfo, playerid) == false)
			return true;
		if (isMember(teaminfo, memberid) == false)
			return true;
		auto member_player = Server::Instance().FindPlayer(memberid);
		if (member_player == nullptr)
			return true;
		SetTeamLeader(teaminfo, memberid);
		SyncPlayerTeamDataToPlayer(teaminfo);
		return true;
	}

	bool MtTeamManager::OnApplyTeamLeaderReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::ApplyTeamLeaderReq>& message, int32 /*source*/)
	{
		message;
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 playerid = player_->Guid();
		auto teaminfo = GetTeamInfo(player_->GetTeamID());
		if (teaminfo == nullptr)
			return true;
		if (isLeader(teaminfo, playerid) == true)
			return true;
		uint64 oldguid = teaminfo->applyleader();
		if (oldguid != INVALID_GUID)
			return true;
		teaminfo->set_applyleader(playerid);
		teaminfo->set_agreeapplytime(60);
		teaminfo->set_agreeapplyleader(false);
		Packet::AskApplyTeamLeaderReq req;
		req.set_playerguid(playerid);
		TBroadcastTeam(teaminfo, { playerid }, req);
		return true;
	}

	bool MtTeamManager::OnAskApplyTeamLeaderReply(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::AskApplyTeamLeaderReply>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 playerid = player_->Guid();
		auto teaminfo = GetTeamInfo(player_->GetTeamID());
		if (teaminfo == nullptr)
			return true;
		uint64 applyleaderid = teaminfo->applyleader();
		auto applyleaderplayer = Server::Instance().FindPlayer(applyleaderid);
		if (applyleaderplayer == nullptr)
			return true;
		bool agreeapply = message->agree();
		bool isleader = isLeader(teaminfo, playerid);
		if (isleader == true)
		{
			if (agreeapply == true)
			{
				SetTeamLeader(teaminfo, applyleaderid);
				SyncPlayerTeamDataToPlayer(teaminfo);
			}
			else
			{
				std::string  notify = "team_leader_juejvapply|" + applyleaderplayer->Name();
				player_->SendClientNotify(notify, Packet::ChatChannel::TEAM, -1);
			}
			ClearApplyLeader(teaminfo);
		}
		else
		{
			if (agreeapply == true)
			{
				teaminfo->set_agreeapplyleader(true);
			}
		}
		return true;
	}
	void MtTeamManager::ClearApplyLeader(Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr)
			return;
		
		teaminfo->set_applyleader(INVALID_GUID);
		teaminfo->set_agreeapplytime(0);
		teaminfo->set_agreeapplyleader(false);
	}

	int32 MtTeamManager::TeamCapacity(const Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr) return 0;
		if (teaminfo->group()) return 10; //团队是10人
		return 5;//默认5人
	}

	bool MtTeamManager::IsFull(const Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr)
			return true;
		return TeamSize(teaminfo) >= TeamCapacity(teaminfo);
	}

	int32 MtTeamManager::GetDefenceCount(Packet::TeamInfo* teaminfo)
	{
		int32 count = 0;
		if (teaminfo == nullptr)
			return count;
		
		for (auto member : teaminfo->member())
		{
			if (member.playerguid() != INVALID_GUID)
			{
				if (member.teambaseinfo().hero_position() == 3)
				{
					count++;
				}
			}
		}
		return count;
	}
	int32 MtTeamManager::GetCureCount(Packet::TeamInfo* teaminfo)
	{
		int32 count = 0;
		if (teaminfo == nullptr)
			return count;

		for (auto member : teaminfo->member())
		{
			if (member.playerguid() != INVALID_GUID)
			{
				if (member.teambaseinfo().hero_position() == 4)
				{
					count++;
				}
			}
		}
		return count;
	}
	int32 MtTeamManager::GetAttackCount(Packet::TeamInfo* teaminfo)
	{
		int32 count = 0;
		if (teaminfo == nullptr)
			return count;

		for (auto member : teaminfo->member())
		{
			if (member.playerguid() != INVALID_GUID)
			{
				if (member.teambaseinfo().hero_position() == 1 || member.teambaseinfo().hero_position() == 2)
				{
					count++;
				}
			}
		}
		return count;

	}
	int32 MtTeamManager::CheckAutoMatch(const Packet::PlayerAutoMatchInfo &playerinfo, Packet::TeamInfo* teaminfo)
	{ 
		auto purpose_ = teaminfo->purpose();
		if (purpose_.purpose() != 0)
		{
			if (playerinfo.level() < purpose_.level_limt_min() || playerinfo.level() > purpose_.level_limt_max()) //|| 
				return -1;
			if (playerinfo.purpose() != purpose_.purpose())
				return -2;
		}

		int32 maxcount = 5;
		if (teaminfo->group())
		{
			maxcount = 10;
		}
		for (int32 i=0; i<teaminfo->member_size() && i<maxcount; i++)
		{
			auto member = teaminfo->member(i);
			if (member.playerguid() == INVALID_GUID )
			{
				if (member.teambaseinfo().hero_position() == 0 || member.teambaseinfo().hero_position() == playerinfo.hero_position()
					|| (member.teambaseinfo().hero_position() == 1 && playerinfo.hero_position() == 2)
					|| (member.teambaseinfo().hero_position() == 2 && playerinfo.hero_position() == 1))
				{
					return i;
				}
			}
		}
		return -3;
	}
	const boost::shared_ptr<Packet::TeamAutomatchConfig> MtTeamManager::GetTeamAutomatchConfig(bool group,int32 dc, int32 cc, int32 ac)
	{
		for (auto amc : automacthconfig)
		{
			if ((amc->group()==1?true:false)== group && amc->defence_count() == dc && amc->cure_count() == cc && amc->attack_count() == ac)
			{
				return amc;
			}
		}
		return nullptr;
	}
	void MtTeamManager::ResetPurpose(Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr )
			return;
		auto purpose = teaminfo->mutable_purpose();
		purpose->set_purpose(0);
		purpose->set_level_limt_min(10);
		purpose->set_level_limt_max(120);
		Packet::PurposeInfoUpdate update;
		update.mutable_purpose()->CopyFrom(teaminfo->purpose());
		TBroadcastTeam(teaminfo, update);
		teaminfo->set_auto_match(false);
		Packet::TeamAutoMatchUpdate update2;
		update2.set_auto_match(teaminfo->auto_match());
		TBroadcastTeam(teaminfo, update2);
	}
	void MtTeamManager::OnClearPlayerAutoMatch(uint64 playerid)
	{
		for (auto iter= automatchplayer_.begin(); iter != automatchplayer_.end();)
		{
			if ((*iter).playerguid() == playerid)
			{
				iter = automatchplayer_.erase(iter);
			}
			else
			{
				iter++;
			}
		}
	}
	bool MtTeamManager::OnPlayerAutoMatchReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerAutoMatchReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		try {
			thread_lua->call<void>(8, "OnPlayerAutoMatchReq", player_, message);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return true;
		}
		return true;
	}
	
	bool MtTeamManager::PlayerIsAutoMatch(uint64 playerid)
	{
		if (std::find_if(automatchplayer_.begin(), automatchplayer_.end(), [=](auto& iter) {
			return iter.playerguid() == playerid;
		}) != automatchplayer_.end())
		{
			return true;
		}
		return false;
	}
	void MtTeamManager::AddPlayerAutoMatch(int32 purpose, MtPlayer* player_)
	{
		if (player_ == nullptr)
			return;
		Packet::PlayerAutoMatchInfo info;
		if (FillPlayerAutoMatchInfo(purpose, player_, info))
		{
			automatchplayer_.push_back(info);
		}
	}
	void MtTeamManager::SendPlayerAutoMatch(MtPlayer* player_, bool automatch, int32 purpose)
	{
		if (player_ == nullptr)
			return;
		if (automatch == false)
		{
			Packet::PlayerAutoMatchReply reply;
			reply.set_auto_match(false);
			player_->SendMessage(reply);
		}
		else
		{
			Packet::PlayerAutoMatchReply reply;
			reply.set_auto_match(true);
			reply.set_purpose(purpose);
			player_->SendMessage(reply);
		}
	}

	void MtTeamManager::S2GSyncTeamMemberLevel(Packet::TeamInfo* teaminfo, uint64 playerid, int32 playerlevel)
	{
		if (teaminfo == nullptr)
			return;
		Packet::CommonReply reply;
		reply.set_reply_name("SyncTeamMemberLevel");
		reply.add_int32_params(playerlevel);
		reply.add_int64_params(playerid);
		TBroadcastTeam(teaminfo, reply);
	}
	bool MtTeamManager::FillPlayerAutoMatchInfo(int32 purpose, MtPlayer* player_, Packet::PlayerAutoMatchInfo& info)
	{
		if (player_ == nullptr)
			return false;
		auto mainactor = player_->MainActor();
		if (mainactor == nullptr)
			return false;
		auto lhs_config = MtActorConfig::Instance().GetBaseConfig(mainactor->DbInfo()->actor_config_id());
		if (lhs_config == nullptr)
			return false;
		info.set_playerguid(player_->Guid());
		info.set_purpose(purpose);
		info.set_level(player_->PlayerLevel());
		info.set_hero_position(lhs_config->hero_position);
		return true;
	}
	void MtTeamManager::OnBigTick(uint64 elapseTime)
	{
		int32 delta = (int32)(elapseTime / 1000);
		for (auto team_ : teamlist_)
		{
			int32 agreeapplytime = team_->agreeapplytime();
			if (agreeapplytime > 0)
			{
				agreeapplytime -= delta;
				if (agreeapplytime <= 0)
				{
					if (team_->agreeapplyleader())
					{
						SetTeamLeader(team_.get(), team_->applyleader());
						SyncPlayerTeamDataToPlayer(team_.get());
					}
					else
					{
						//广播;
					}
					ClearApplyLeader(team_.get());
				}
				else
				{
					team_->set_agreeapplytime(agreeapplytime);
				}
			}
			auto purposeinfo = team_->purpose();
			if (purposeinfo.purpose() != 0 && team_->auto_match() == true && !IsFull(team_.get()))
			{
				for (auto iter = automatchplayer_.begin(); iter != automatchplayer_.end();)
				{

					int32 checkautoindex = CheckAutoMatch(*iter, team_.get());
					if (checkautoindex >= 0)
					{
						uint64 playerid = (*iter).playerguid();
						iter = automatchplayer_.erase(iter);
						auto player_ = Server::Instance().FindPlayer(playerid);
						if (player_ != nullptr && player_->LogoutStatus() == eLogoutStatus::eOnLine)
						{
							try {
								thread_lua->call<void>(8, "TickAutoMatchTeam", player_, team_, checkautoindex);
							}
							catch (ff::lua_exception& e) {
								e;
							}
						}
					}
					else
					{
						iter++;
					}
				}
			}
		}
	}

	void MtTeamManager::TBroadcastTeam(Packet::TeamInfo* team, const google::protobuf::Message& msg, bool follow_state )
	{
		if (team == nullptr)
			return;
		if (follow_state) {
			for (auto member : team->member()){
				if (member.teambaseinfo().playerstatus() == Packet::PlayerTeamStatus::Team_Follow){
					auto player_ = Server::Instance().FindPlayer(member.playerguid());
					if (player_){
						player_->SendMessage(msg);
					}
				}
			}
		}
		else{
			for (auto member : team->member()){
				auto player_ = Server::Instance().FindPlayer(member.playerguid());
				if (player_){
					player_->SendMessage(msg);
				}
			}
		}
	}
	void MtTeamManager::TBroadcastTeam(Packet::TeamInfo* team, const std::vector<uint64>& filter_list, const google::protobuf::Message& msg)
	{
		if (team == nullptr)
			return;
		for (auto member : team->member())
		{
			uint64 memberid = member.playerguid();
			if(std::find(filter_list.begin(), filter_list.end(), memberid) != filter_list.end())
				continue;
			auto player_ = Server::Instance().FindPlayer(memberid);
			if (player_)
			{
				player_->SendMessage(msg);
			}
		}
	}
	void MtTeamManager::SyncPlayerTeamDataToPlayer(Packet::TeamInfo* teaminfo)
	{
		if (teaminfo == nullptr)
			return;
		boost::shared_ptr<G2S::SyncPlayerTeamData> data = boost::make_shared<G2S::SyncPlayerTeamData>();
		auto team = data->mutable_team();
		team->set_teamid(teaminfo->guid());
		team->set_leaderid(teaminfo->leader());
		team->set_arrangetype(teaminfo->arrangetype());
		team->set_group(teaminfo->group());
		team->set_robot_team(teaminfo->robot_team());
		for (auto member : teaminfo->member())
		{
			uint64 memberid = member.playerguid();
			if (memberid != INVALID_GUID)
			{
				auto tmember = team->add_member();
				tmember->set_memberid(memberid);
				tmember->set_teamstatus(member.teambaseinfo().playerstatus());
			}
		}
		
		for (auto member : teaminfo->member())
		{
			auto player_ = Server::Instance().FindPlayer(member.playerguid());
			if (player_)
			{
				player_->DispatchMessage(data);
			}
		}
	}

	void MtTeamManager::ResetPlayerTeamDataToPlayer(MtPlayer* player)
	{
		if (player == nullptr)
			return;
		boost::shared_ptr<G2S::ResetPlayerTeamData> reset = boost::make_shared<G2S::ResetPlayerTeamData>();
		player->DispatchMessage(reset);
	}
	void MtTeamManager::FillTeamInfoReply(Packet::TeamInfo* teaminfo, Packet::TeamInfoReply& info)
	{
		info.set_guid(teaminfo->guid());
		for (auto member : teaminfo->member())
		{
			auto memberinfo = info.add_member();
			if (member.has_basic_info())
			{
				memberinfo->mutable_teambaseinfo()->CopyFrom(member.teambaseinfo());
				memberinfo->mutable_playerbase()->CopyFrom(member.basic_info());
			} else {
				uint64 memberid = member.playerguid();
				auto member_ = Server::Instance().FindPlayer(memberid);
				FillTeamPlayerInfo(member, member_, *memberinfo);
			}
		}
		info.set_leader(teaminfo->leader());
		info.mutable_purpose()->CopyFrom(teaminfo->purpose());
		info.set_auto_match(teaminfo->auto_match());
		info.set_group(teaminfo->group());
		info.set_arrangetype(teaminfo->arrangetype());
		for (auto applyid : teaminfo->applylist())
		{
			auto apply_ = Server::Instance().FindPlayer(applyid);
			if (apply_){
				auto apply = info.add_applylist();
				apply->CopyFrom(apply_->GetDBPlayerInfo()->basic_info());
			}
		}
	}
	void MtTeamManager::FillTeamPlayerInfo(const Packet::TeamPlayerInfo &teamplayerinfo, boost::shared_ptr<MtPlayer> player_, Packet::TeamFullPlayerInfo& info)
	{
		if (player_ != nullptr){
			auto base_info = info.mutable_playerbase();
			base_info->CopyFrom(*player_->GetScenePlayerInfo());
			base_info->set_move_speed(player_->GetMoveSpeed());
		}
		info.mutable_teambaseinfo()->CopyFrom(teamplayerinfo.teambaseinfo());
	}
	void MtTeamManager::FillTeamPlayerBasicInfo(Packet::TeamPlayerBasicInfo &baseinfo, MtPlayer* player_,Packet::PlayerTeamStatus status, int32 position_index)
	{
		baseinfo.set_playerstatus(status);
		baseinfo.set_hero_position(0);
		//baseinfo.set_curhp(0);
		//baseinfo.set_maxhp(0);
		baseinfo.set_position_index(position_index);
		if (player_ != nullptr)
		{
			//baseinfo.set_curhp(100);
			//baseinfo.set_maxhp(100);
			auto mainactor = player_->MainActor();
			if (mainactor == nullptr)
				return;
			auto lhs_config = MtActorConfig::Instance().GetBaseConfig(mainactor->DbInfo()->actor_config_id());
			if (lhs_config == nullptr)
				return;
			baseinfo.set_hero_position(lhs_config->hero_position);
		}
	}


	bool MtTeamManager::OnPlayerSetTeamStatusReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::PlayerSetTeamStatusReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		uint64 playerid=player_->Guid();
		auto teaminfo = GetTeamInfo(player_->GetTeamID());
		if (teaminfo == nullptr)
			return true;
		for (int32 i=0; i<teaminfo->member_size(); ++i)
		{
			auto member = teaminfo->mutable_member(i);
			if (member->playerguid() == playerid)
			{
				auto memberbaseinfo = member->mutable_teambaseinfo();
				if (message->status() == Packet::PlayerTeamStatus::Team_Follow && memberbaseinfo->playerstatus() != Packet::PlayerTeamStatus::Team_Follow)
				{
					auto member_ = Server::Instance().FindPlayer(playerid);
					int32 can = 1;
					try {
						can = thread_lua->call<int32>(8, "CheckFollowLeader", member_, teaminfo->leader());
					}
					catch (ff::lua_exception& e) {
						ZXERO_ASSERT(false) << "lua exception:" << e.what();
					}
					if (can == 0)
					{
						return true;
					}
					memberbaseinfo->set_playerstatus(Packet::PlayerTeamStatus::Team_Follow);
					FollowLeader(teaminfo, player_.get());
					Packet::TeamPlayerBasicUpdate update;
					FillTeamPlayerInfo(*member, member_, *update.mutable_playerinfo());
					TBroadcastTeam(teaminfo, update);
				}
				else if (message->status() == Packet::PlayerTeamStatus::Team_AFK && memberbaseinfo->playerstatus() == Packet::PlayerTeamStatus::Team_Follow)
				{
					auto member_ = Server::Instance().FindPlayer(playerid);
					if (member_ != nullptr && member_->GetInBattle())//正在战斗中
					{
						memberbaseinfo->set_playerstatus(Packet::PlayerTeamStatus::Team_AFKING);
					}
					else
					{
						memberbaseinfo->set_playerstatus(Packet::PlayerTeamStatus::Team_AFK);
					}
					Packet::TeamPlayerBasicUpdate update;
					FillTeamPlayerInfo(*member, member_, *update.mutable_playerinfo());
					TBroadcastTeam(teaminfo, update);
				}
			}
		}

		SyncPlayerTeamDataToPlayer(teaminfo);
		return true;
	}

	void MtTeamManager::OnPlayerBattleEnd(boost::shared_ptr<MtPlayer> player)
	{
		if (player == nullptr)
			return;
		std::lock_guard<std::mutex> guard(mutex_);
		auto teaminfo = GetTeamInfo(player->GetTeamID());
		if (teaminfo == nullptr)
			return;
		
		for (int32 index = 0; index < teaminfo->member_size(); ++index)
		{
			auto tplayerinfo = teaminfo->mutable_member(index);
			uint64 memberguid = tplayerinfo->playerguid();
			if (memberguid == player->Guid()) {
				 if (tplayerinfo->teambaseinfo().playerstatus() == Packet::PlayerTeamStatus::Team_AFKING)
				 {
					 tplayerinfo->mutable_teambaseinfo()->set_playerstatus(Packet::PlayerTeamStatus::Team_Follow);
					 FollowLeader(teaminfo, player.get());

					 Packet::TeamPlayerBasicUpdate update;
					 FillTeamPlayerInfo(*tplayerinfo, player, *update.mutable_playerinfo());
					 TBroadcastTeam(teaminfo, update);

					 SyncPlayerTeamDataToPlayer(teaminfo);
				 }
				 return;
			}
		}
	}
	bool MtTeamManager::OnSetTeamHeroPositionReq(const boost::shared_ptr<client_session>& session, const boost::shared_ptr<Packet::SetTeamHeroPositionReq>& message, int32 /*source*/)
	{
		auto player_ = session->player();
		if (player_ == nullptr)
			return true;
		auto teaminfo = GetTeamInfo(player_->GetTeamID());
		if (teaminfo == nullptr)
			return true;
		for (int32 i=0;i<teaminfo->member_size();i++)
		{
			if (i == message->position_index())
			{
				auto memberinfo = teaminfo->mutable_member(i)->mutable_teambaseinfo();
				memberinfo->set_hero_position(message->hero_position());
				teaminfo->set_setposition(true);
				Packet::SetTeamHeroPositionReply reply;
				reply.set_hero_position(memberinfo->hero_position());
				reply.set_position_index(i);
				player_->SendMessage(reply);
				return true;
			}
		}
		return true;
	}

	bool MtTeamManager::OnTeamMemberChangeScene(
		const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<S2G::TeamMemberChangeScene>& msg,
		int32 /*source*/)
	{
		if (msg == nullptr || player == nullptr) {
			return true;
		}

		auto team = GetTeamInfo(player->GetTeamID());
		if ( team == nullptr || team->leader() != player->Guid() ) {
			//1 目前只处理跟随队长, 其他情况一律忽略,否则将导致组队切场景递归循环！！！！
			return true;
		}
		for (int32 i = 0; i < team->member_size(); i++) {
			auto mem_info = team->member(i);
			if (mem_info.playerguid() == player->Guid())
				continue;
			if (mem_info.teambaseinfo().playerstatus() != Packet::PlayerTeamStatus::Team_Follow)
				continue;;
			auto mem_player = Server::Instance().FindPlayer(mem_info.playerguid());
			if (mem_player == nullptr)
				continue;

			if (!MtScene::IsRaidScene(msg->scene_id())) {
				//拉队员进场景
				if (msg->has_init_pos()) {
					auto init_pos = msg->init_pos();
					Server::Instance().SendG2SCommonMessage(mem_player.get(), "G2SGoTo", { msg->scene_id(), init_pos.x(),init_pos.y(),init_pos.z() }, {}, {});
					//mem_player->GoTo(msg->scene_id(), { msg->init_pos().x(),msg->init_pos().y(),msg->init_pos().z() });
				}
				else {
					Server::Instance().SendG2SCommonMessage(mem_player.get(), "G2SGoTo", { msg->scene_id(), -1,-1,-1 }, {}, {});
					//mem_player->GoTo(msg->scene_id(), INVALID_POS);
				}
			}
			else {
				auto init_pos = msg->init_pos();
				Server::Instance().SendG2SCommonMessage(mem_player.get(), "G2SGoRaid", { msg->scene_id(), msg->script_id(), msg->rtid(), msg->camp(),init_pos.x(),init_pos.y(),init_pos.z() }, {}, {});
				//mem_player->GoRaid(msg->scene_id(), msg->script_id(), msg->rtid(), msg->camp(), { pos.x(),pos.y(),pos.z()} );
			}
		}
		return true;
	}

	bool MtTeamManager::OnChatMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::TeamChat>& msg, int32 /*source*/)
	{
		if (msg == nullptr ) {
			return true;
		}
		player;
		auto team_msg = msg->mutable_msg();
		auto teaminfo = GetTeamInfo(msg->teamid());
		if (teaminfo)
		{
			TBroadcastTeam(teaminfo, {}, *team_msg);
		}
		return true;
	}

	bool MtTeamManager::OnPlayerAutoMatch(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::PlayerAutoMatch>& msg, int32 /*source*/)
	{
		if (msg == nullptr || player == nullptr) {
			return true;
		}
		//没有找到用的地方呀
		//PlayerAutoMatch(player,msg->purpose(), msg->setauto());
		return true;
	}	

}