#include "mt_mission_proxy.h"
#include "mt_mission.h"
#include "module.h"
#include "../data/data_manager.h"
#include "../mt_player/mt_player.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_save_work.h"
#include "../base/mt_game_urlwork.h"
#include "../mt_monster/mt_npc.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_upgrade/mt_upgrade.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_data_cell/mt_data_cell.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_activity/mt_activity.h"
#include "../mt_item/mt_container.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_config/mt_config.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_mission/mt_arrange_proxy.h"
#include <S2GMessage.pb.h>
#include <S2SMessage.pb.h>
#include <BaseConfig.pb.h>
#include <ItemConfig.pb.h>
#include <SceneCommon.pb.h>
#include <MissionConfig.proto.fflua.h>
#include <ActivityPacket.pb.h>
#include <ActivityConfig.pb.h>
#include <MissionPacket.pb.h>
#include <AllPacketEnum.proto.fflua.h>
#include "../zxero/mem_debugger.h"

namespace Mt
{

	void MtMissionProxy::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtMissionProxy, ctor()>(ls, "MtMissionProxy")
			.def(&MtMissionProxy::GetMissionMap, "GetMissionMap")
			.def(&MtMissionProxy::GetMissionById, "GetMissionById")			
			.def(&MtMissionProxy::OnFinishMission, "OnFinishMission")
			.def(&MtMissionProxy::OnUpdateMission, "OnUpdateMission")
			.def(&MtMissionProxy::OnTeamSyncCheck, "OnTeamSyncCheck")
			.def(&MtMissionProxy::MissionGetCheck, "MissionGetCheck")
			.def(&MtMissionProxy::OnAcceptMission, "OnAcceptMission")
			.def(&MtMissionProxy::OnAddMission, "OnAddMission")
			.def(&MtMissionProxy::OnNextMainMission, "OnNextMainMission")
			.def(&MtMissionProxy::OnResetMission, "OnResetMission")
			.def(&MtMissionProxy::OnMissionRemoveByType, "OnMissionRemoveByType")
			.def(&MtMissionProxy::Mission2Member, "Mission2Member")
			.def(&MtMissionProxy::OnAcceptGradeUpMission,"OnAcceptGradeUpMission")
			.def(&MtMissionProxy::OnMissionReward,"OnMissionReward")
			.def(&MtMissionProxy::OnTeamArrangeReward, "OnTeamArrangeReward")
			.def(&MtMissionProxy::OnCloneMission, "OnCloneMission")
			.def(&MtMissionProxy::OnCloneMissionFromLeader,"OnCloneMissionFromLeader")
			.def(&MtMissionProxy::TeamLeaderExtraReward, "TeamLeaderExtraReward")
			;
	}

	void MtMissionProxy::TeamLeaderExtraReward(const Config::MissionConfig* config, const int32 base_exp)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		//��ӻ�����Ƕӳ����ж��⽱��
		if (config == nullptr) {
			return;
		}

		if (config->count_limit() < 3) {
			return;
		}

		if (!player->IsTeamLeader()) {
			return;
		}

		player->AddItemByIdWithNotify(Packet::TokenType::Token_Exp, (int32)(base_exp*0.05), Config::ItemAddLogType::AddType_Mission, config->id());

		//�ӳ����
		auto seed = Server::Instance().RandomNum(1, 100);
		if (seed >= 90) {
			player->AddItemByIdWithNotify(20138011, 1, Config::ItemAddLogType::AddType_Mission, config->id());
		}
	}

	void MtMissionProxy::MissionSyncTimer(const int32 delta)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if ( missionsyc_time_ > 0) {
			missionsyc_time_ -= delta;
			if (missionsyc_time_ <= 0) {
				//��ʱû�������Ϊͬ��
				for (auto child : mission_sync_vote_) {
					if (child.second == -1) {
						mission_sync_vote_[child.first] = 1;
					}
				}
				if (!CheckAllVoteOk()) {
					//�㲥������������ʧ�ܽ��
					Packet::LuaReplyPacket reply;
					reply.set_reply_name("SyncMissionFail");
					player->BroadcastTeamEx(true, reply);

				}
				else{
					//�㲥������������OK���
					Packet::LuaReplyPacket luareply;
					luareply.set_reply_name("SyncMissionOk");
					player->BroadcastTeamEx(true, luareply);

					OnMissionSync(sync_type_);
				}
				missionsyc_time_ = 0;
				sync_type_ = Config::MissionType::InvalidMission;
				mission_sync_vote_.clear();
			}
		}
	}

	std::map<int32, MtMission*>& MtMissionProxy::GetMissionMap()
	{
		return mission_map_;
	}
	void MtMissionProxy::InsertMission(int32 mission_id, MtMission* mission, bool load)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (load == false)
		{
			std::stringstream DBlog;
			DBlog <<"AddMission," << player->PlayerBibase() << "," << mission_id;
			Server::Instance().AddUrlTask(new db_log_require_work(DBlog.str(), __FILE__, __LINE__));
		}
		mission_map_.insert({ mission_id, mission });
	}
	MtMission* MtMissionProxy::GetMissionById(int32 missionid)
	{
		for (auto child : mission_map_) {
			if (child.second->Config()->id() == missionid) {
				return child.second;
			}
		}
		return nullptr;
	}
	void MtMissionProxy::LoadMission(MtMission* mission)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (!mission)
			return;

		int32 mission_id = mission->Config()->id();
		if (mission_map_.find(mission_id) == mission_map_.end())
		{
			InsertMission(mission_id, mission, true);
		}

		LOG_INFO << "LoadMission ok, player guid=" << player->Guid() << " mission type=" << mission->Config()->type() << " mission id=" << mission_id;

		//�Ƴ��������������ǣ��Ż���һ�δ洢Ч��
		mission->MissionDB()->ClearDirty();
	}

	void MtMissionProxy::InitMission()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		for (auto child : mission_map_)
		{
			auto missionDB = child.second->MissionDB();
			if (missionDB != nullptr) {
				//��ԭnpc
				if (child.second->Config()->finish_type() == Config::FinishType::RandomNpc) {

					int32 index = missionDB->param(Packet::MissionDB_Param::Npc_Index);
					int32 posid = missionDB->param(Packet::MissionDB_Param::Pos_Id);
					int32 seris_id = player->AddNpc(posid, index,-1);
					missionDB->set_param(Packet::MissionDB_Param::Npc_Series, seris_id);
				}

				int32 index = child.second->Config()->commit_npc_id();
				if (MtScene::IsDynamicNpc(index)) {//�ύnpcҲ�����Ƕ�̬��
					auto npcdata = MtMonsterManager::Instance().FindDynamicNpc(index);
					if (npcdata != nullptr) {
						int32 seris_id = player->AddNpc(npcdata->pos_id(), index,-1);
						missionDB->set_param(Packet::MissionDB_Param::Commit_Npc_Series, seris_id);
					}
					else {
						ZXERO_ASSERT(false) << "dynamic mission commit npc data is null! index=" << index << "  missionid=" << child.first;
					}
				}

				//����npc
				player->AddOtherNpc(child.second->Config()->other_npc_group(),-1);
			}
		}

		OnAcceptMission(Config::MissionType::MainMission);
	}

	Packet::ResultCode MtMissionProxy::OnAcceptGradeUpMission(const int32 hero_id)
	{
		//ְҵӢ���ж�
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return Packet::ResultCode::InvalidPlayer;
		}
		boost::shared_ptr<MtActor> actor = nullptr;
		for (auto child : player->Actors()) {
			if (child->ConfigId() == hero_id) {
				actor = child;
				break;
			}
		}
		if (actor == nullptr) {
			return Packet::ResultCode::InvalidProfession;
		}

		auto up_color =  actor->DbInfo()->color() + 1;
		if (up_color > Packet::ActorColor_MAX) {
			return Packet::ResultCode::ColorMax;
		}

		auto config = MtUpgradeConfigManager::Instance().UpgradeColorRequirement(actor->Professions(), (Packet::ActorColor)up_color);
		if (config == nullptr) {
			return Packet::ResultCode::InvalidConfigData;
		}

		//�ȼ��ж�
		if (actor->Level() < config->level_) {
			return Packet::ResultCode::LevelLimit;
		}

		//�����ж�
		auto item_base = player->FindItemByConfigId(Packet::ContainerType::Layer_MissionAndItemBag, config->item_id[2]);
		if (item_base != nullptr) {
			return Packet::ResultCode::CountLimit;
		}

		//����accept,ֱ���������
		OnAddMission(config->quest_id_);

		return Packet::ResultCode::ResultOK;
	}
	
	void MtMissionProxy::OnAcceptMission(Config::MissionType type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		//�������Ƽ��
		auto ret = OnAcceptCheck(type);
		if (ret != Packet::ResultCode::ResultOK) {
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, ret);
		}

		int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", type);
		if (script_id <= 0) {
			LOG_ERROR << "TypeToScriptId failed, player guid=" << player->Guid() << " mission type=" << type;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		///////////////////////
		int32 lua_ret = player->OnLuaFunCall_n(script_id, "xOnAccept",
		{
			{ "type",type }
		});

		return player->SendCommonResult(Packet::ResultOption::Mission_Opt, (Packet::ResultCode)lua_ret);
	}

	bool MtMissionProxy::MissionGetCheck(Config::MissionType type)
	{
		for (auto child : mission_map_) {
			auto config = child.second->Config();
			if (config == nullptr)
				continue;

			if (config->type() == type) {
				return true;
			}
		}
		return false;
	}

	bool MtMissionProxy::MissionIdCheck(const int32 mission_id)
	{
		if (mission_map_.find(mission_id) != mission_map_.end()) {
			return true;
		}
		return false;
	}

	void MtMissionProxy::OnAddMission(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto iter = mission_map_.find(mission_id);
		if (iter != mission_map_.end())
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::MissionCountLimit);

		auto mission_config = MtMissionManager::Instance().FindMission(mission_id);
		if (!mission_config) {
			LOG_ERROR << "FindMission failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", mission_config->type());
		if (script_id <= 0) {
			LOG_ERROR << "OnAddMission failed, player guid=" << player->Guid() << " mission type=" << mission_config->type();
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		try {
			int32 lua_ret = thread_lua->call<int32>(script_id, "OnAddBegin", player.get(), mission_config.get());
			if (lua_ret != 0){
				LOG_ERROR << "OnAddMission failed, player guid=" << player->Guid() << " mission id=" << mission_id << " error code=" << lua_ret;
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
			}
		}
		catch (ff::lua_exception& e) {
			e;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::LuaScriptException);
		}

		boost::shared_ptr<Config::MissionExConfig> mission_config_ex = nullptr;
		if (mission_config->finish_type() == Config::FinishType::KillMonster) {
			mission_config_ex = MtMissionManager::Instance().FindMissionEx(mission_config->param(0));
		}
		auto mission = MtMissionManager::Instance().CreateMission(player->Guid(), mission_config.get(), mission_config_ex.get());
		if (mission == nullptr) {
			LOG_ERROR << "CreateMission failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		auto mission_data = mission->MissionDB();
		//
		try {
			auto lua_ret = (Packet::ResultCode)thread_lua->call<int32>(502, "OnPrepare", player.get(), mission_config, mission_data);
			if (lua_ret != Packet::ResultCode::ResultOK) {
				LOG_ERROR << "OnAddMission failed, player guid=" << player->Guid() << " mission id=" << mission_id << " error code=" << lua_ret;
				MtMissionManager::Instance().RecycleMission(mission);
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, lua_ret);
			}
		}
		catch (ff::lua_exception& e) {
			e;
			MtMissionManager::Instance().RecycleMission(mission);
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::LuaScriptException);
		}

		InsertMission(mission_id, mission);
		Packet::MissionUpdateReply reply;
		mission->FillMessage(reply);
		player->SendMessage(reply);

		//��������Ľű����ýӿ�
		try {
			auto lua_ret = (Packet::ResultCode)thread_lua->call<int32>(script_id, "OnAddEnd", player.get(), mission_config.get());
			if (lua_ret != Packet::ResultCode::ResultOK) {
				LOG_ERROR << "OnAddMission OnAddEnd failed, player guid=" << player->Guid() << " mission id=" << mission_id << " error code=" << lua_ret;
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, lua_ret);
			}
		}
		catch (ff::lua_exception& e) {
			e;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::LuaScriptException);
		}

		LOG_INFO << "OnAddMission ok, player guid=" << player->Guid() << " mission type=" << mission_config->type() << " mission id=" << mission_id;

		return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::ResultOK);

	}

	void MtMissionProxy::OnCloneMission(MtPlayer * target_player, const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (target_player == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidTargetPlayer);
		}

		auto mission_config = MtMissionManager::Instance().FindMission(mission_id);
		if (!mission_config) {
			LOG_ERROR << "FindMission failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		//����һ��ͬ��������		
		OnMissionRemoveByType(mission_config->type());

		auto proxy = target_player->GetMissionProxy();
		if (proxy == nullptr) {
			LOG_ERROR << "GetMissionProxy failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		auto missions = proxy->GetMissionMap();
		auto target_iter = missions.find(mission_id);
		if (target_iter == missions.end()) {
			ZXERO_ASSERT(false) << "OnCloneMission failed, target =" << target_player->Guid() << " has no mission=" << mission_id << " player =" << player->Guid();
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}


		//��������������
		int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", mission_config->type());
		if (script_id <= 0) {
			LOG_ERROR << "OnCloneMission failed, player guid=" << player->Guid() << " mission type=" << mission_config->type();
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		try {
			int32 lua_ret = thread_lua->call<int32>(script_id, "OnClone", player.get(), target_player, mission_config.get());
			if (lua_ret != 0){
				LOG_ERROR << "OnCloneMission failed, player guid=" << player->Guid() << " mission id=" << mission_id << " error code=" << lua_ret;
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
			}
		}
		catch (ff::lua_exception& e) {
			e;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::LuaScriptException);
		}

		//������������
		boost::shared_ptr<Config::MissionExConfig> mission_config_ex = nullptr;
		if (mission_config->finish_type() == Config::FinishType::KillMonster) {
			mission_config_ex = MtMissionManager::Instance().FindMissionEx(mission_config->param(0));
		}
		auto new_mission = MtMissionManager::Instance().CreateMission(player->Guid(), mission_config.get(), mission_config_ex.get());
		if (new_mission == nullptr) {
			LOG_ERROR << "CreateMission failed, player guid=" << player->Guid() << " mission id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		auto src_mission = target_iter->second;
		auto mission_data = new_mission->MissionDB();
		mission_data->CopyFrom(*(src_mission->MissionDB()));

		//����npc
		if (mission_config->finish_type() == Config::FinishType::RandomNpc) {
			int32 posid = mission_data->param(Packet::MissionDB_Param::Pos_Id);
			int32 index = mission_data->param(Packet::MissionDB_Param::Npc_Index);
			int32 series_id = player->AddNpc(posid, index,-1);
			//�������һ����ˮ�ţ�ͬ���񣬲�ͬ��ҵ�npc��ˮ���п��ܳ�ͻ
			mission_data->set_param(Packet::MissionDB_Param::Npc_Series, series_id);
		}

		InsertMission(mission_id, new_mission);
		Packet::MissionUpdateReply reply;
		new_mission->FillMessage(reply);
		player->SendMessage(reply);

		//LOG_INFO << "OnCloneMission ok, player guid=" << player->Guid() << " mission type=" << mission_config->type() << " mission id=" << mission_id;

		return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::ResultOK);
	}

	void MtMissionProxy::Mission2Member(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		//���Ƕӳ��Ͳ�����,��Ȼ�͵ݹ���
		if (!player->IsTeamLeader()) {
			return;
		}

		auto memebers = player->GetTeamMember(true);
		for (auto child : memebers) {
			if (child == player->Guid()) {
				continue;
			}
			//���ﲻҪ��leader��ǰ��scene���в�ѯ��������Ӵ��͵������е���һ�û���͹������ж���ʧЧ
			auto tplayer = Server::Instance().FindPlayer(child);
			if (tplayer != nullptr ) {
				auto msg = boost::make_shared<S2S::S2SCommonMessage>();
				msg->set_request_name("S2SCloneMission");
				msg->add_int64_params(player->Guid());
				msg->add_int32_params(mission_id);				
				tplayer->ExecuteMessage(msg);
			}
		}
	}

	void MtMissionProxy::OnFinishMission(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto iter = mission_map_.find(mission_id);
		if (iter == mission_map_.end()) {
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		auto mission_config = iter->second->Config();
		if (!mission_config) {
			LOG_ERROR << "OnFinishMission failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		if (mission_config->finish_type() == Config::FinishType::CommitItem ||
			mission_config->finish_type() == Config::FinishType::MailItem){
			if (!player->DelItemById(mission_config->param(0), mission_config->param(1), Config::ItemDelLogType::DelType_Mission, 0)) {
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::RemoveItemFailed);
			}
		}


		int32 script_id = player->OnLuaFunCall_1(501,"TypeToScriptId",mission_config->type());
		if (script_id <= 0) {
			LOG_ERROR << "OnFinishMission failed, player guid=" << player->Guid() << " mission type=" << mission_config->type();
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		
		//��ǰ���������ʾ
		player->SendLuaPacket("MissionOverNotify", {} ,{} ,{});

		/////////////////////////////////////////���������׶�
		int32 lua_ret = player->OnLuaFunCall_n(script_id, "xOnFinishBegin",
		{
			{ "id", mission_id },
			{ "type", mission_config->type() }
		});

		if (lua_ret != 0) {
			LOG_ERROR << "OnFinishMission failed, player guid=" << player->Guid() << " mission id=" << mission_id << " error code=" << lua_ret;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode(lua_ret));
		}
		//////////////////////////////////////��������
		auto mission_data = iter->second->MissionDB();
		int32 rtid = -1;
		if (mission_data != nullptr) {
			if (mission_data->state() != Packet::MissionState::MissionComplete) {
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionState);
			}
			mission_data->set_state(Packet::MissionState::MissionFinish);
			rtid = mission_data->param(Packet::MissionDB_Param::CarbonRuntimeId);
		}

		OnMissionRemove(iter->second, Packet::MissionState::MissionFinish);
		mission_map_.erase(iter);

		/////////////////////////////////////ע��˴������Ѿ����٣�xOnFinish��ֻ�ܴ���config���ݣ�
		lua_ret = player->OnLuaFunCall_n(script_id, "xOnFinishEnd",
		{
			{ "id", mission_id },
			{ "rtid", rtid }
		});

		if (lua_ret != 0) {
			LOG_ERROR << "OnFinishMission failed, player guid=" << player->Guid() << " mission id=" << mission_id << " error code=" << lua_ret;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode(lua_ret));
		}

		///////////////////////////////////////��ɺ���
		player->OnLuaFunCall_n(501, "OnFinishAcceptMission",
		{
			{ "id", mission_id }
		});

	}

	void MtMissionProxy::OnCloneMissionFromLeader()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto leader_guid = player->GetTeamLeader();// MtTeamManager::Instance().GetTeamLeader(player->GetTeamID());
		auto scene = player->Scene();
		if (scene == nullptr) {
			return;
		}
		//�ӳ�����Ҫ�Լ�ͬ���Լ�
		if (leader_guid == player->Guid()) {
			return;
		}
		auto leader = Server::Instance().FindPlayer(leader_guid);
		if (leader == nullptr) {
			return;
		}
		auto proxy = leader->GetMissionProxy();
		if (proxy == nullptr) {
			return;
		}
		auto raid = leader->GetRaid();
		if (raid == nullptr) {
			return;
		}

		for (auto child : proxy->GetMissionMap()) {
			auto mission_data = child.second->MissionDB();
			if (mission_data == nullptr) {
				continue;
			}
			if (mission_data->param(Packet::MissionDB_Param::CarbonRuntimeId) == raid->RuntimeId()) {
				OnCloneMission(leader.get(), child.first);
				return;
			}
		}		
	}

	void MtMissionProxy::OnUpdateMission(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto iter = mission_map_.find(mission_id);
		if (iter == mission_map_.end())
			return;

		Packet::MissionUpdateReply reply;
		iter->second->FillMessage(reply);
		player->SendMessage(reply);
	}

	void MtMissionProxy::OnResetMission(Config::MissionType type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", type);
		if (script_id <= 0) {
			LOG_ERROR << "OnAcceptMission failed, player guid=" << player->Guid() << " mission type=" << type;
			return;
		}

		int32 lua_ret = player->OnLuaFunCall_n(script_id, "xOnReset", 
		{
			{ "type", type }
		});

		if (lua_ret != 0) {
			LOG_ERROR << "OnResetMission failed, player guid=" << player->Guid() << " mission type=" << type << " error code=" << lua_ret;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode(lua_ret));
		}

		OnMissionRemoveByType(type);
	}

	void MtMissionProxy::OnMissionRemoveByType(Config::MissionType type)
	{
		for (auto iter = mission_map_.begin(); iter != mission_map_.end(); ){
			if (iter->second->Config()->type() == type) {
				OnMissionRemove(iter->second, Packet::MissionState::MissionDrop);
				iter = mission_map_.erase(iter);
			}
			else
				++iter;
		}
	}

	void MtMissionProxy::OnMissionRemove(MtMission* mission, Packet::MissionState state)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (mission == nullptr)
			return;

		auto mission_config = mission->Config();
		if (!mission_config)
			return;

		auto dbinfo = mission->MissionDB();
		//��̬�ύnpc Ҫ�Ƴ�npc
		if (MtScene::IsDynamicNpc(mission_config->commit_npc_id())) {
			player->DelNpc(dbinfo->param(Packet::MissionDB_Param::Commit_Npc_Series));
		}
		//����npcҲҪ�Ƴ�
		if (mission_config->other_npc_group() >= 0) {
			player->DelNpcByGroup(mission_config->other_npc_group());
		}
		//��ɱnpcҲҪ�Ƴ�
		player->DelNpc(dbinfo->param(Packet::MissionDB_Param::Npc_Series));

		//ͬ���ͻ���
		Packet::MissionUpdateReply reply;
		auto mission_packet = reply.mutable_mission();
		mission->FillMessage(reply);
		mission_packet->set_state(state);
		player->SendMessage(reply);

		//���&cache
		dbinfo->SetDirty();
		dbinfo->SetDeleted();
		player->Cache2Save(dbinfo);

		LOG_INFO << "OnFinishMission ok, player guid=" << player->Guid() << " mission id=" << mission_config->id();

		MtMissionManager::Instance().RecycleMission(mission);
	}

	void MtMissionProxy::OnMissionReward(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto config = MtMissionManager::Instance().FindMission(mission_id);
		if (config == nullptr)
			return;
		
		auto package = MtItemPackageManager::Instance().GetPackage(config->package_id());
		if (package) {
			Packet::NotifyItemList notify;
			package->SetMissionId(mission_id);
			package->AddToPlayer(player.get(), nullptr, &notify,0,false,Config::AddType_Mission);
			if (notify.item_size() > 0) {
				player->SendMessage(notify);
			}
		}

		int32 gold_reward = config->gold();
		if (config->gold_rate() > 0 && gold_reward > 0) {
			gold_reward = (int32)(gold_reward*config->gold_rate());
		}

		player->AddItemByIdWithNotify(Packet::TokenType::Token_Gold, gold_reward, Config::ItemAddLogType::AddType_Mission, config->id());

		//ͨ�ý���,ע�⣬�ܶ�������������ǽű�����ɵģ��������յȼ�ƥ�侭��/��ҽ������߼�
		int32 exp_reward = config->exp();
		if (config->exp_rate() > 0 && exp_reward > 0) {
			exp_reward = (int32)(exp_reward* config->exp_rate());
		}

		player->AddItemByIdWithNotify(Packet::TokenType::Token_Exp, exp_reward, Config::ItemAddLogType::AddType_Mission, config->id());

		//�ӳ����⾭�齱��5%
		TeamLeaderExtraReward(config.get(), exp_reward);
	}

	bool MtMissionProxy::OnTeamArrangeReward(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		auto config = MtMissionManager::Instance().FindMission(mission_id);
		if (config == nullptr)
			return false;

		if (!player->IsTeamLeader()) {
			return false;
		}

		auto scene = player->Scene();
		if (scene == nullptr) {
			return false;
		}
		
		auto package = MtItemPackageManager::Instance().GetPackage(config->roll_drop());
		if (package == nullptr) {
			return false;
		}

		auto circle = config->circle();
		if (circle < 0 || circle >= 32) {
			return false;
		}
				
		auto proxy = player->GetArrangeProxy();
		if (proxy != nullptr) {

			std::vector<uint64> filter_list;
			auto members = player->GetTeamMember(true);
			for (auto child : members) {
				auto tplayer = scene->FindPlayer(child);
				if (tplayer == nullptr)
					continue;

				auto cell_container = tplayer->DataCellContainer();
				if (cell_container == nullptr)
					continue;


				auto params = GetMissionParam(config->type());
				if (params.size() != 2) {
					continue;
				}
				auto flag = cell_container->get_data_32(Packet::CellLogicType::MissionCell, params[1]);
				if (cell_container->lua_check_bit_data_32(flag, circle)) {
					filter_list.push_back(child);
				}

			}

			//ȫ������
			if (filter_list.size() == members.size()) {
				return false;
			}

			//����һ��proxy ����
			proxy->OnClearContainer();

			package->SetMissionId(mission_id);
			if (!package->AddToPlayerArrange(player.get() )) {
				return false;
			}

			// ��д!!!
			for (auto child : members) {
				auto tplayer = scene->FindPlayer(child);
				if (tplayer == nullptr)
					continue;
				tplayer->SetParams32("arrange_statistic", mission_id);
			}
			//

			proxy->RefreshData2Client();
			proxy->OnArrangeBegin( filter_list);
		}

		return true;
	}

	void MtMissionProxy::OnNextMainMission()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto datacell_container_ = player->DataCellContainer();
		if (datacell_container_ == nullptr)
			return;

		auto last_config = MtConfigDataManager::Instance().FindConfigValue("mission_join");
		if (last_config == nullptr) {
			ZXERO_ASSERT(false) << "invalid mission begin config data ! guid=" << player->Guid();
			return;
		}

		std::vector<int32> process_indexs = GetMissionParam(Config::MissionType::MainMission);
		std::vector<int32> mission_process;
		for (auto child : process_indexs) {
			int32 id = player->DataCellContainer()->get_data_32(Packet::CellLogicType::MissionCell, child);
			mission_process.push_back(id);
		}
		for (uint32 k = 0; k < mission_process.size(); k++)
		{
			int32 last_mission_id = mission_process[k];
			if (last_mission_id <= 0)
			{
				if (k == 0) //��һ��������Ҫ���⴦��
				{
					auto config = MtConfigDataManager::Instance().FindConfigValue("mission_begin");
					if (config == nullptr) {
						ZXERO_ASSERT(false) << "invalid mission begin config data ! guid=" << player->Guid();
						return;
					}
					if (player->GetCamp() == Packet::Camp::Alliance) {
						OnAddMission(config->value2());
					}
					else {
						OnAddMission(config->value1());
					}
					
					break;
				}
			}
			//else if (last_mission_id == last_config->value1() || last_mission_id == last_config->value2()) {
			//	//�������������ϲ�����д��
			//	OnAddMission(last_config->value3());
			//	break;
			//}

			//�����µĺ�������
			auto mission_add_on = MtMissionManager::Instance().FindMissionAddOn(last_mission_id);
			int32 preid = -1;
			for (auto child : mission_add_on)
			{
				int32 index = MtMissionManager::Instance().GetMissionIndex(child);
				if (index < 0 || index>3) {
					ZXERO_ASSERT(false) << "invalid mission index! mission id=" << child;
					continue;
				}

				//��ɹ��ľͲ��ٴ�����
				if (child <= mission_process[index]) {
					break;
				}

				auto iter = mission_map_.find(child);
				if (iter != mission_map_.end())
					continue;

				auto mission_config = MtMissionManager::Instance().FindMission(child);
				if (mission_config == nullptr)
					continue;

				//���ǰ�������Ƿ�ok
				bool predok = true;
				for (int32 i = 0; i < mission_config->pre_id_size(); i++) {
					preid = mission_config->pre_id(i);
					index = MtMissionManager::Instance().GetMissionIndex(preid);
					if (index < 0 || index>3) {
						ZXERO_ASSERT(false) << "invalid mission index! mission id=" << preid;
						continue;
					}

					if (preid > mission_process[index] && mission_process[index] >= 0) {
						predok = false;
						break;
					}
				}

				//mission_config�������ǰ������ok�ˣ���������
				if (predok) {
					LOG_INFO << "OnNextMainMission id=" << child << " guid=" << player->Guid();
					OnAddMission(child);
				}
			}
		}
	}

	void MtMissionProxy::OnDropMission(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto iter = mission_map_.find(mission_id);
		if (iter == mission_map_.end()) {
			LOG_ERROR << "mission_map_ find failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}

		auto mission_config = MtMissionManager::Instance().FindMission(mission_id);
		if (!mission_config) {
			LOG_ERROR << "FindMission failed, player guid=" << player->Guid() << " mission_id=" << mission_id;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		if (mission_config->can_drop() <= 0) {
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::DropLimit);
		}

		int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", mission_config->type());
		if (script_id <= 0) {
			LOG_ERROR << "OnDropMission failed, player guid=" << player->Guid() << " mission type=" << mission_config->type();
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		
		////////////////////////////
		int32 lua_ret = player->OnLuaFunCall_n(script_id, "xOnDrop",
		{
			{ "id", mission_id },
			{ "type", mission_config->type() }
		});

		if (lua_ret != 0) {
			LOG_ERROR << "OnDropMission failed, player guid=" << player->Guid() << " mission type=" << mission_config->type() << " error code=" << lua_ret;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode(lua_ret));
		}
		//
		try {
			auto mission_data = iter->second->MissionDB();
			lua_ret = thread_lua->call<int32>(502, "OnDrop", player.get(), mission_config, mission_data);
			if (lua_ret != Packet::ResultCode::ResultOK) {
				LOG_ERROR << "OnDropMission failed, player guid=" << player->Guid() << " mission type=" << mission_config->type() << " error code=" << lua_ret;
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode(lua_ret));
			}
		}
		catch (ff::lua_exception& e) {
			e;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::LuaScriptException);
		}
		////////////////////////////////////
		
		OnMissionRemove(iter->second, Packet::MissionState::MissionDrop);
		mission_map_.erase(iter);

		return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::ResultOK);

	}

	void MtMissionProxy::OnMissionFight(const int32 mission_id)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto iter = mission_map_.find(mission_id);
		if (iter == mission_map_.end())
			return;

		auto mission = iter->second;

		//����ս��
		auto mission_data = mission->MissionDB();
		if (mission_data == nullptr) {
			return;
		}
		int32 npc_index = mission_data->param(Packet::MissionDB_Param::Npc_Index);
		int32 monster_group_id = mission_data->param(Packet::MissionDB_Param::Monster_Group);
		int32 pos_id = mission_data->param(Packet::MissionDB_Param::Pos_Id);

		auto battle = player->Scene()->GetBattleManager().CreateFrontBattle(player.get(),nullptr,
		{
			{ "script_id", 1003 },
			{ "pos_id", pos_id },
			{ "mission_id",mission_id },
			{ "relate_npc",npc_index },
			{ "monster_group_id",monster_group_id }
		});
	}

	void MtMissionProxy::SendSyncMissionProc(const int64 src_guid, Config::MissionType type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		uint32 time_mark = MtTimerManager::Instance().CurrentDate();
		Packet::SyncMission reply;
		reply.set_src_guid(src_guid);
		reply.set_type(type);
		reply.set_time_mark((int32)time_mark);
		player->SendMessage(reply);
	}


	////////////////////////////////////////////////////////////////////////////
	//����ͬ���߼���Ŀǰ����Ƚϸ��ӣ�
	/*
	a��������.����ĺ��
	��ȡ�������ֽ���ֱ��ͬ��		
	���������Ҫ������ͬ���߼�
	����ʱ�̽��븱����Ҫ���������ͬ��
	�޸�����������漰�첽����߼�

	b��������.��������
	��û���������£���ȡ�������ֽ���ֱ��ͬ��
	�����������£��������ȡ����Ҫ���ֽ��ȣ�������ͬ����ܽ�ȡ����	
	�����������£������ڽ�ȡ�������ֽ���ֱ��ͬ��
	���������Ҫ������ͬ���߼�
	����ʱ�̽��븱����Ҫ���������ͬ��	
	�и�����������漰�첽����߼�

	1.���ڸ�����������漰�첽����߼�
	Ŀǰ����Ҫ˼·
	��ȡʱ�ӳ���������ͬ���������
	���ʱ��Ա����ͬ���ӳ��������
	2.������������
	Ŀǰ����Ҫ˼·
	��ȡʱ�ӳ���������ͬ���߼�
	3.����ʱ�̽��븱����Ҫ���������ͬ��	
	���븱��ʱ����ͬ���ӳ��������
	*/
	////////////////////////////////////////////////////
	int32 MtMissionProxy::OnSyncMissionAgree(const uint32 time_mark, const uint64 agree_guid)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return -1;
		}
		//�Ƿ�ʱ30s
		if (MtTimerManager::Instance().DiffTimeToNow(time_mark) >= 30) {
			return -1;
		}

		//ͬ�������ж�		
		if (mission_sync_vote_.find(agree_guid) == mission_sync_vote_.end()) {
			return -1;
		}
		mission_sync_vote_[agree_guid] = 1;
		//�㲥ͬ�����
		Packet::LuaReplyPacket reply;
		reply.set_reply_name("AgreeSyncMission");
		reply.add_int64_params(agree_guid);
		player->BroadcastTeamEx(true, reply);

		if (CheckAllVoteOk()) {
			//�㲥������������OK���
			Packet::LuaReplyPacket luareply;
			luareply.set_reply_name("SyncMissionOk");
			player->BroadcastTeamEx(true, luareply);
			missionsyc_time_ = 0;
			sync_type_ = Config::MissionType::InvalidMission;
			return 1;
		}

		//����������ûͬ��
		return 0;
	}
	
	int32 MtMissionProxy::OnSyncMissionDisagree(const uint32 time_mark, const uint64 disagree_guid)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return -1;
		}
		//�Ƿ�ʱ30s
		if (MtTimerManager::Instance().DiffTimeToNow(time_mark) >= 30) {
			return -1;
		}
			
		auto scene = player->Scene();
		if (scene == nullptr) {
			return -1;
		}
		auto dis_player = scene->FindPlayer(disagree_guid);
		if (dis_player == nullptr) {
			return -1;
		}

		//ͬ�������ж�		
		if (mission_sync_vote_.find(disagree_guid) == mission_sync_vote_.end()) {
			return -1;
		}
		mission_sync_vote_[disagree_guid] = 0;

		//�㲥�ܾ����
		Packet::LuaReplyPacket reply;
		reply.set_reply_name("DisagreeSyncMission");
		reply.add_int64_params(disagree_guid);
		player->BroadcastTeamEx(true, reply);

		if (CheckAllVote()) {
			//�㲥������������ʧ�ܽ��
			Packet::LuaReplyPacket luareply;
			luareply.set_reply_name("SyncMissionFail");
			player->BroadcastTeamEx(true, luareply);
			missionsyc_time_ = 0;
			sync_type_ = Config::MissionType::InvalidMission;
			return 1;
		}

		//����Ƶ��
		std::string notify = "disagree_sync_mission|" + dis_player->Name();
		player->SendClientNotify(notify, Packet::ChatChannel::TEAM, -1);

		return 0;
	}

	bool MtMissionProxy::CheckAllVoteOk()
	{
		for (auto child : mission_sync_vote_) {
			if (child.second != 1) {
				return false;
			}
		}
		return true;
	}

	bool MtMissionProxy::CheckAllVote()
	{
		for (auto child : mission_sync_vote_) {
			if (child.second == -1) {
				return false;
			}
		}
		return true;
	}

	std::vector<int32> MtMissionProxy::GetMissionParam(Config::MissionType type)
	{
		//Ŀǰֻ�и�������֧�֣�����������Ϊ���Ϸ�
		std::vector<int32> params;
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return params;
		}
		int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", type);
		if (script_id <= 0) {
			LOG_ERROR << "OnSyncMission failed, player guid=" << player->Guid() << " mission type=" << type;
			return params;
		}

		try {
			params = thread_lua->call<std::vector<int32>>(script_id, "GetMissionParams", player);
		}
		catch (ff::lua_exception& e) {
			e;
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return params;
		}
		return params;
	}

	void MtMissionProxy::OnMissionSync( Config::MissionType type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		std::vector<int32> params = GetMissionParam(type);
		if (params.size() != 2) {
			LOG_ERROR << "GetMissionParam failed, player guid=" << player->Guid() << " mission type=" << type;
			return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
		}
		int32 leader_process_last = -1;
		if (params[0] >= 0) {
			auto cell_container = player->DataCellContainer();
			if (cell_container == nullptr) {
				return;
			}
			leader_process_last = cell_container->get_data_32(Packet::CellLogicType::MissionCell, (Packet::MissionCellIndex)params[0]);
		}
		if (leader_process_last <= 0) {//����ӳ�������	
			int32 script_id = player->OnLuaFunCall_1(501, "TypeToScriptId", type);
			if (script_id <= 0) {
				LOG_ERROR << "OnMissionSync failed, player guid=" << player->Guid() << " mission type=" << type;
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
			}

			leader_process_last = player->OnLuaFunCall_0(script_id, "GetNextMission");
			if (leader_process_last < 0) {
				LOG_ERROR << "GetNextMission failed, player guid=" << player->Guid() << " mission type=" << type;
				return player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
			}					
		}

		auto iter = mission_map_.find(leader_process_last);
		if (iter != mission_map_.end()) {
			OnMissionRemove(iter->second, Packet::MissionState::MissionDrop);
			mission_map_.erase(iter);
		}

		//�������,�����ӣ������������Ƶ�������
		OnAddMission(leader_process_last);
	}


	bool MtMissionProxy::OnTeamSyncCheck(Config::MissionType type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		if (player->GetRaid() != nullptr) {
			return true;//������ǿ��ͬ��
		}

		if (missionsyc_time_ > 0) {
			player->SendClientNotify("wait_member_confirm", -1, -1);
			return false;
		}
		auto scene = player->Scene();
		if (scene == nullptr) {
			return false;
		}
		auto datacell_container_ = player->DataCellContainer();
		bool ok = true;
		if (datacell_container_ == nullptr) {
			return false;
		}

		std::vector<int32> params = GetMissionParam(type);
		if (params.size() != 2) {
			return false;
		}

		mission_sync_vote_.clear();

		int32 leader_process_last = datacell_container_->get_data_32(Packet::CellLogicType::MissionCell, (Packet::MissionCellIndex)params[0]);

		auto members = player->GetTeamMember();
		if (members.empty()) {
			return false;
		}

		for (auto child : members) {
			if (child == player->Guid())
				continue;

			auto team_player = scene->FindPlayer(child);
			if (team_player == nullptr) {
				return false;
			}

			if (team_player->GetRaid() != nullptr && team_player->GetRaid() != player->GetRaid()) {	//���ϲ�������������
				return false;
			}
			auto cell_container = team_player->DataCellContainer();
			if (cell_container != nullptr) {
				int32 process_last = cell_container->get_data_32(Packet::CellLogicType::MissionCell, (Packet::MissionCellIndex)params[0]);
			/*	int32 process_max = cell_container->get_data_32(Packet::CellLogicType::MissionCell, (Packet::MissionCellIndex)params[1]);
				int32 process_date = cell_container->get_data_32(Packet::CellLogicType::MissionCell, (Packet::MissionCellIndex)params[2]);*/

				//���Ȳ�һ��,��������û����ͬ��������Ҫ����ͬ��
				if (leader_process_last != process_last || team_player->GetMissionById(leader_process_last) == nullptr ) {
					//����ϢҪ��ͬ��
					auto proxy = team_player->GetMissionProxy();
					if (proxy != nullptr) {
						proxy->SendSyncMissionProc(player->Guid(), type);
						mission_sync_vote_.insert({ team_player->Guid(),-1 });
						ok = false;
					}
				}
			}
		}

		if (!ok) {
			//��ʼ��ʱ
			missionsyc_time_ = 30000;
			sync_type_ = type;

			//�㲥��ʼͬ��״̬
			Packet::LuaReplyPacket reply;
			reply.set_reply_name("BeginSyncMission");
			player->BroadcastTeamEx(true, reply);

			return false;
		}

		return ok;
	}
	Packet::ResultCode MtMissionProxy::OnMemberCountCheck(std::vector<uint64> &players, int32 count_limit )
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return Packet::ResultCode::InvalidPlayer;
		}
		if (count_limit == -1) {//���뵥��
			//auto memebers = MtTeamManager::Instance().GetTeamMember(player->GetTeamID(), true);
			//if (memebers.size() > 1) {
			//	return Packet::ResultCode::SingleLimit;
			//}
			if (player->GetTeamID() != INVALID_GUID || player->IsTeamGroup()) {
				return Packet::ResultCode::SingleLimit;
			}
			players.push_back(player->Guid());
		}
		else if (count_limit > 1) {
			players = player->GetTeamMember( true);
			if ((int32)players.size() < count_limit) {
				return Packet::ResultCode::MemberNotEnough;
			}

			if (!player->IsTeamLeader()) {
				return Packet::ResultCode::Team_NotLeader;
			}
		}
		else {
			players.push_back(player->Guid());
		}

		if (players.empty()) {
			return Packet::ResultCode::InvalidPlayerData;
		}

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtMissionProxy::OnAcceptCheck(Config::MissionType type)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return Packet::ResultCode::InvalidPlayer;
		}
		auto script_id = player->OnLuaFunCall_1(501, "MissionTypeToAcScriptId", type);
		if (script_id > 0) {
			return (Packet::ResultCode)player->OnLuaFunCall_n(script_id, "OnJoinCheck",
			{
				{ "mission_type",type }
			});
		}
		return Packet::ResultCode::ResultOK;
	}

	MtMissionProxy::MtMissionProxy(MtPlayer& player) :player_(player.weak_from_this())
	{

	}

	MtMissionProxy::~MtMissionProxy()
	{
		for (auto child : mission_map_) {
			MtMissionManager::Instance().RecycleMission(child.second);
		}
		mission_map_;
	}

}
