#include "mt_battle_field.h"
#include "mt_battle_field_manager.h"
#include "module.h"
#include "../base/client_session.h"
#include "../mt_player/mt_player.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_copy_scene.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_data_cell/mt_data_container.h"

#include <AllPacketEnum.pb.h>
#include <EnterScene.pb.h>
#include <BattleField.pb.h>
#include <S2GMessage.pb.h>
#include <BattleField.proto.fflua.h>
#include <BaseConfig.pb.h>

namespace Mt
{
	//////////////////////////////////////////////////////////////////////////
	static MtBattleFieldManager* __battle_room_manager = nullptr;
	
	void MtBattleFieldManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtBattleFieldManager, ctor()>(ls, "MtBattleFieldManager")
			.def(&MtBattleFieldManager::LeaveBattleField, "LeaveBattleField")
			.def(&MtBattleFieldManager::FindRoomById,"FindRoomById")
			.def(&MtBattleFieldManager::EnterBattleField, "EnterBattleField")
			.def(&MtBattleFieldManager::OnCreatBattleResult,"OnCreatBattleResult")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtBattleFieldManager::Instance, "LuaBattleFieldManager");
	}

	REGISTER_MODULE(MtBattleFieldManager)
	{
		require("data_manager");
		require("MtSceneManager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtBattleFieldManager::OnLoad, __battle_room_manager));
	}

	MtBattleFieldManager::MtBattleFieldManager()
	{
		__battle_room_manager = this;
	}

	MtBattleFieldManager::~MtBattleFieldManager()
	{
		room_pool_.release();
	}

	MtBattleFieldManager& MtBattleFieldManager::Instance()
	{
		return *__battle_room_manager;
	}
	
	zxero::int32 MtBattleFieldManager::OnLoad()
	{
		level_config_.insert({ 18,29 });
		level_config_.insert({ 30,39 });
		level_config_.insert({ 40,49 });
		level_config_.insert({ 50,59 });
		level_config_.insert({ 60,60 });

		auto table = data_manager::instance()->get_table("bf_reward");
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Packet::BFConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			bf_config_.insert({ config->level(), config });
		}

		return 0;
	}


	const boost::shared_ptr<Packet::BFConfig> MtBattleFieldManager::FindBfConfig(const int32 level)
	{
		if (bf_config_.find(level) != bf_config_.end())
		{
			return bf_config_[level];
		}

		ZXERO_ASSERT(false) << "MtConfigDataManager::FindBfConfig failed! level=" << level;
		return nullptr;
	}

	void MtBattleFieldManager::OnSecondTick(int32 elapse_second)
	{
		for (auto child : room_map_) {
			child.second->OnSecondTick(elapse_second);
		}
	}

	void MtBattleFieldManager::OnBigTick(uint64 elapseTime)
	{
		for (auto child : room_map_) {
			child.second->OnBigTick(elapseTime);
		}
	}	

	void MtBattleFieldManager::JoinBattleRoom(MtPlayer * player, const int32 scene_id, const int32 script_id)
	{
		if (player == nullptr) {
			return;
		}
		auto scene = player->Scene();
		if (scene == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::InvalidPlayerData);
		}

		auto raid_scene = player->OnRaidNpc2Scene(scene_id);
		if (raid_scene == nullptr) {
			return;
		}

		if (player->GetTeamID() != INVALID_GUID) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::Team_Exist);
		}

		RaidHelper raid_helper;
		raid_helper.scene_id_ = raid_scene->SceneId();
		raid_helper.portal_pos_ = player->Position();
		raid_helper.script_id_ = script_id;
		raid_helper.portal_scene_id_ = player->GetSceneId();

		MtBattleField * room = nullptr;
		const Packet::BattleFeildProxy* proxy = player->GetBattleFeildProxy();
		if (proxy->raid_rtid() == INVALID_GUID) {		
			room = JoinRoom(raid_helper, player);
			if (room == nullptr) {
				return;//提示一次就好
			}
			//报名成功，更新活动次数
			scene->QueueInLoop(boost::bind(&MtPlayer::OnLuaFunCall_1, player, 501, "AddActivityTimes", 22));
		}
		else {			
			room = FindRoomById(proxy->raid_rtid());
			if (room == nullptr) {
				return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
			}
			if (room->GetBattleFieldState() == Packet::BattleFieldState::bf_state_wait_leave){
				return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::BattleFieldOver);
			}
		} 

		////检查副本还在不在
		//auto raid = raid_scene->GetRaid(room->GetId());
		//if (raid == nullptr) {
		//	return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::BattleFieldOver);
		//}

		//传入战场
		auto ret = Room2BattleFieldRaid(room, raid_scene.get(), player);
		if (ret == Packet::ResultCode::ResultOK) {
			//更新房间信息
			BroadcastToRoom(room->GetId(), room->GetBattleRoomInfo());
		}
			
		return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, ret);
	}

	void MtBattleFieldManager::BroadcastToRoom(uint64 rtid, const google::protobuf::Message& msg)
	{
		auto room = FindRoomById(rtid);
		if (room != nullptr) {
			room->BroadcastMsg(msg);
		}
	}

	void MtBattleFieldManager::EnterBattleField(MtPlayer * player)
	{
		if (player == nullptr)
			return;

		const Packet::BattleFeildProxy* proxy = player->GetBattleFeildProxy();
		auto rtid = proxy->raid_rtid();
		auto iter = room_map_.find(rtid);
		if (iter == room_map_.end()) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}

		auto room = iter->second;
		auto pi = room->GetBattlePlayerInfo(player->Guid());
		if (pi == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}

		if (pi->bf_state() == Packet::BattleFieldState::bf_state_inroom) {

			player->SendMessage(room->GetBattleRoomInfo());
			//进入战场倒计时30s
			Packet::EnterRoomNotify msg;
			msg.set_time(300 - room->GetWaitingTime());
			player->SendMessage(msg);
			
		}
		else if (pi->bf_state() != Packet::BattleFieldState::bf_state_invalid) {
			//战场中
			player->SendMessage(room->GetBattleFieldInfo());
		}
	}

	void MtBattleFieldManager::TryEnterBattleField(MtPlayer * player)
	{
		if (player == nullptr)
			return;
		
		const Packet::BattleFeildProxy* proxy = player->GetBattleFeildProxy();
		auto rtid = proxy->raid_rtid();
		auto iter = room_map_.find(rtid);
		if (iter == room_map_.end()) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}

		auto room = iter->second;
		auto bfplayer = room->GetBattlePlayerInfo(player->Guid());
		if (bfplayer == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}

		if (room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_inbattle
			&& room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_room_waiting
			&& room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_battle_waiting) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::BattleFieldOver);
		}

		auto raid_scene = player->OnRaidNpc2Scene(iter->second->GetSceneId());
		if (raid_scene == nullptr) {
			return;
		}
			
		auto ret = Room2BattleFieldRaid(room, raid_scene.get(), player);

		return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, ret);
	}

	Packet::ResultCode MtBattleFieldManager::Room2BattleFieldRaid( MtBattleField* room, MtScene * scene, MtPlayer * player )
	{
		if (player == nullptr) {
			return  Packet::ResultCode::InvalidPlayerData;
		}		
		if (scene == nullptr) {
			return  Packet::ResultCode::BattleFieldOver;
		}

		if (room == nullptr) {
			return Packet::ResultCode::NotJoinRoom;
		}

		auto raid = scene->GetRaid(room->GetId());
		if (raid == nullptr) {
			return  Packet::ResultCode::BattleFieldOver;
		}

		auto pi = room->GetBattlePlayerInfo(player->Guid());
		if (pi == nullptr) {
			return  Packet::ResultCode::NotJoinRoom;
		}

		Packet::BattleFeildProxy bproxy;
		bproxy.set_raid_rtid(room->GetId());
		bproxy.set_bf_state(Config::BFState::Bf_Normal);
		player->SetBattleFeildProxy(bproxy);

		//同步进入数据
		if (room->GetBattleFieldState() == Packet::BattleFieldState::bf_state_inroom) {
			pi->set_bf_state(Packet::BattleFieldState::bf_state_inroom);
		}
		else {
			pi->set_bf_state(Packet::BattleFieldState::bf_state_inbattle);
			room->OnJoinBattle(pi->guid());
		}
	
		room->BroadcastMsg(*pi);

		//重置血量
		Server::Instance().SendG2SCommonMessage(player, "G2SRelive", { Packet::LastHpType::BFLastHp }, {}, {});


		//-1是候战室位置，0防守方,1攻击方，
		int camp = room->CheckCamp(player->Guid());
		int pos_index = MtBattleField::GetPosIndex(camp);
		auto init_pos = scene->Config()->spawn_pos(pos_index);
		Server::Instance().SendG2SCommonMessage(player, "G2SGoRaid", { room->GetSceneId(), raid->GetScriptId(), room->GetId(),camp,init_pos.x(),init_pos.y(),init_pos.z() }, {}, {});

		return  Packet::ResultCode::ResultOK;

	}

	MtBattleField * MtBattleFieldManager::FindRoomById(const uint64 rtid)
	{
		std::unique_lock<std::mutex> lock(mutex_);

		if (room_map_.find(rtid) != room_map_.end())
			return room_map_[rtid];

		return nullptr;
	}

	MtBattleField *  MtBattleFieldManager::JoinRoom(const RaidHelper& helper, MtPlayer * player)
	{
		if (player == nullptr){
			return nullptr;
		}			
		
		std::unique_lock<std::mutex> lock(mutex_);

		int32 level = player->PlayerLevel();
		auto room = FindRoom(level);
		if (room == nullptr) {
			int32 min_lv = 0;
			int32 max_lv = 0;
			for (auto child : level_config_) {
				if (level >= child.first && level <= child.second) {
					min_lv = child.first;
					max_lv = child.second;
				}
			}
			if (min_lv == 0 || max_lv == 0) {
				player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::LevelLimit);
				return nullptr;
			}

			room = CreateRoom(helper,max_lv, min_lv);
			if (room == nullptr) {
				player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::JoinRoomFailed);
				return nullptr;
			}			
		}

		if (room->IsFull()) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::RoomIsFull);
			return nullptr;
		}
		room->OnJoin(player);

		return room;
	}

	MtBattleField * MtBattleFieldManager::CreateRoom( const RaidHelper& helper, const int32 max_lv, const int32 min_lv)
	{
		if (helper.scene_id_ == -1)
			return nullptr;
		
		auto scene = MtSceneManager::Instance().GetSceneById(helper.scene_id_);
		if (scene == nullptr)
			return nullptr;

		int32 rtid = scene->AddRaid(helper);
		if (rtid == INVALID_GUID)
			return nullptr;

		auto room = room_pool_.create();
		if (room == nullptr)
			return nullptr;

		room->reset();

		auto& room_info = room->GetBattleRoomInfo();		
		room_info.set_scene_id(helper.scene_id_);
		room_info.set_min_level(min_lv);
		room_info.set_max_level(max_lv);
		room_info.set_bf_state(Packet::BattleFieldState::bf_state_inroom);
		room->SetId(rtid);

		//std::unique_lock<std::mutex> lock(mutex_);
		room_map_.insert({ rtid, room });

		return room;
	}
	
	bool MtBattleFieldManager::OnDestroyRoom(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::DestroyRoom>& message, int32 /*source*/)
	{
		std::unique_lock<std::mutex> lock(mutex_);

		player;

		auto iter = room_map_.find(message->rtid());
		if (iter == room_map_.end())
			return false;

		auto room = iter->second;
		if (room == nullptr) {
			return false;
		}

		auto players = room->GetPlayers();
		for (auto child : players) {
			Packet::BattleFeildProxy proxy;
			proxy.set_raid_rtid(INVALID_GUID);
			proxy.set_bf_state(Config::Bf_Invalid);
			child->SetBattleFeildProxy(proxy);
		}

		room_map_.erase(iter);

		return true;
	}

	//bool MtBattleFieldManager::OnBuffPoint(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2G::BuffPoint>& message, int32 /*source*/)
	//{
	//	if (player == nullptr) {
	//		return true;
	//	}

	//	auto bf_proxy = player->GetBattleFeildProxy();
	//	if (bf_proxy == nullptr) {
	//		return true;
	//	}

	//	auto room = FindRoomById(bf_proxy->raid_rtid());
	//	if (room == nullptr) {
	//		return true;
	//	}
	//	
	//	room->OnBuffPoint(player.get(), message->posindex());

	//	return true;
	//}

	MtBattleField * MtBattleFieldManager::FindRoom(const int32 level)
	{
		for (auto child : room_map_) {
			if (!child.second->CheckLevel(level))
				continue;

			//已经开始的候战室就不再补人了
			if( child.second->GetBattleFieldState() != Packet::BattleFieldState::bf_state_inroom)
				continue;

			if (child.second->IsFull() )
				continue;

			return child.second;
		}

		return nullptr;
	}


	bool MtBattleFieldManager::OnJoinBattleRoom(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::JoinBattleRoom>& message,
		int32 /*source*/)
	{
		if (session == nullptr) {
			return true;
		}
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}

		JoinBattleRoom(player.get(), message->scene_id(), message->script_id());
		return true;
	}

	bool MtBattleFieldManager::OnQuitBattleRoom(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::LeaveBattleRoom>& /*message*/,
		int32 /*source*/)
	{
		if (session == nullptr) {
			return true;
		}
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}

		return QuitBattleField(player.get());
	}

	bool MtBattleFieldManager::OnQuitBattleField(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::LeaveBattleField>& message,
		int32 /*source*/)
	{
		if (session == nullptr) {
			return true;
		}
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;

		return QuitBattleField(player.get());
	}
	
	bool MtBattleFieldManager::OnAgreeEnterBattleField(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::AgreeEnterBattleField>& /*message*/,
		int32 /*source*/)
	{
		if (session == nullptr) {
			return true;
		}
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}

		//战斗中就不处理
		if (player->GetInBattle() || player->BattleGround() != nullptr) {
			player->SendClientNotify("finish_battle_come", -1, -1);
			return true;
		}

		if (player->GetTeamID() != INVALID_GUID) {
			player->SendClientNotify("quit_team_come", -1, -1);
			return true;
		}

		TryEnterBattleField(player.get());
		return true;
	}

	bool MtBattleFieldManager::OnDisAgreeEnterBattleField(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::DisAgreeEnterBattleField>& /*message*/, 
		int32 /*source*/)
	{
		if (session == nullptr) {
			return true;
		}
		auto player = session->player();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		const Packet::BattleFeildProxy* proxy = player->GetBattleFeildProxy();
		auto rtid = proxy->raid_rtid();
		//auto cur_raid = player->GetRaid();
		//if (cur_raid != nullptr && cur_raid->RuntimeId() == rtid) {
		//	//在战场中不同意也直接传
		//	TryEnterBattleField(player.get());
		//	return true;
		//}	

		/////////////////////////////
		auto iter = room_map_.find(rtid);
		if (iter == room_map_.end()) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
			return true;
		}

		auto room = iter->second;
		auto bfplayer = room->GetBattlePlayerInfo(player->Guid());
		if (bfplayer==nullptr) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
			return true;
		}
		
		//改为inroom，表示不自动传入副本
		bfplayer->set_bf_state(Packet::BattleFieldState::bf_state_disagree);

		player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtBattleFieldManager::OnFlagOprate(
		const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::FlagOprate>& message,
		int32 /*source*/)
	{
		if (session == nullptr) {
			return true;
		}
		auto player = session->player();
		if (player == nullptr) {
			return true;
		}

		//禁止坐骑
		auto dbinfo = player->GetScenePlayerInfo();
		if (dbinfo == nullptr ){
			return true;
		}
		if (dbinfo->mount_id() > 0) {
			player->SendClientNotify("MountOprateForbid", -1, -1);
			return true;
		}
	
		if (message->option() == Packet::FlagOption::BeginPick) {
			 OnDoPickFlag(player.get());
		}		
		else if (message->option() == Packet::FlagOption::FlagDrop) {
			 OnDoDropFlag(player.get());
		}
		else{
			player->SendClientNotify("Invalid_Request", -1, -1);
		}

		return true;
	}

	Packet::ResultCode MtBattleFieldManager::OnDoPickFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return player->SendClientNotify("InvalidPlayerData", -1, -1);
		}
		auto bf_proxy = player->GetBattleFeildProxy();
		if (bf_proxy == nullptr) {
			return player->SendClientNotify("NotJoinRoom", -1, -1);
		}
		auto room = FindRoomById(bf_proxy->raid_rtid());
		if (room == nullptr) {
			return player->SendClientNotify("NotJoinRoom", -1, -1);
		}
		if (room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_inbattle) {
			return player->SendClientNotify("InvalidBattleState", -1, -1);
		}
		int32 camp = room->CheckCamp(player->Guid());
		if (camp == -1) {
			return player->SendClientNotify("InvalidBFCamp", -1, -1);
		}
		auto scene = player->Scene();
		if (scene == nullptr) {
			return player->SendClientNotify("InvalidPlayerData", -1, -1);
		}
		int pos_index = MtBattleField::GetPosIndex(camp,1);
		auto flagpos = scene->Config()->spawn_pos(pos_index);
		if (!player->ValidPosition(flagpos,5000)) {
			return player->SendClientNotify("InvalidPosition", -1, -1);
		}

		auto bf_info = room->GetBattleFieldInfo();
		if (camp == 1 && !bf_info.defender_flag()) {
			return player->SendClientNotify("NoFlag", -1, -1);
		}
		if (camp == 0 && !bf_info.attacker_flag()) {
			return player->SendClientNotify("NoFlag", -1, -1);
		}

		room->OnBeginPickFlag(player->Guid());

		Packet::FlagOprate msg;
		msg.set_option(Packet::FlagOption::BeginPick);
		msg.set_guid(player->Guid());
		room->BroadcastMsg(msg);

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtBattleFieldManager::OnDoCancelFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return player->SendClientNotify("InvalidPlayerData", -1, -1);
		}
		auto bf_proxy = player->GetBattleFeildProxy();
		if (bf_proxy == nullptr) {
			return player->SendClientNotify("NotJoinRoom", -1, -1);
		}
		auto room = FindRoomById(bf_proxy->raid_rtid());
		if (room == nullptr) {
			return player->SendClientNotify("NotJoinRoom", -1, -1);
		}
		if (room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_inbattle) {
			return player->SendClientNotify("InvalidBattleState", -1, -1);
		}

		room->OnCancelPickFlag(player);

		return Packet::ResultCode::ResultOK;
	}

	Packet::ResultCode MtBattleFieldManager::OnDoDropFlag(MtPlayer * player)
	{
		if (player == nullptr) {
			return player->SendClientNotify("InvalidPlayerData", -1, -1);
		}		
		
		auto bf_proxy = player->GetBattleFeildProxy();
		if (bf_proxy == nullptr) {
			return player->SendClientNotify("NotJoinRoom", -1, -1);
		}
		auto room = FindRoomById(bf_proxy->raid_rtid());
		if (room == nullptr) {
			return player->SendClientNotify("NotJoinRoom", -1, -1);
		}
		if (room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_inbattle) {
			return player->SendClientNotify("InvalidBattleState", -1, -1);
		}

		int32 camp = room->CheckCamp(player->Guid());
		if (camp == -1) {
			return player->SendClientNotify("InvalidBFCamp", -1, -1);
		}

		auto scene = player->Scene();
		if (scene == nullptr) {
			return player->SendClientNotify("InvalidPlayerData", -1, -1);
		}

		int pos_index = MtBattleField::GetPosIndex(camp, 2);
		auto flagpos = scene->Config()->spawn_pos(pos_index);
		if (!player->ValidPosition(flagpos, 5000)) {
			return player->SendClientNotify("InvalidPosition", -1, -1);
		}

		room->OnSuccessWinFlag(player);		

		return Packet::ResultCode::ResultOK;
	}


	bool MtBattleFieldManager::OnBattleFieldFightTarget(
		const boost::shared_ptr<client_session>& session, 
		const boost::shared_ptr<Packet::BattleFieldFightTarget>& message,
		int32 /*source*/)
	{
		auto player = session->player();
		if (player == nullptr) {
			return true;
		}

		auto target = Server::Instance().FindPlayer(message->target_guid());
		if (target == nullptr) {
			player->SendClientNotify("InvalidTargetPlayer", -1, -1);
			return true;
		}

		if (player->GetSceneId() != target->GetSceneId()) {
			player->SendClientNotify("InvalidPlayerData", -1, -1);
			return true;
		}

		auto proxy = player->GetBattleFeildProxy();
		if (proxy == nullptr) {
			player->SendClientNotify("InvalidPlayerData", -1, -1);
			return true;
		}
		auto room = FindRoomById(proxy->raid_rtid());
		if (room == nullptr) {
			player->SendClientNotify("NotJoinRoom", -1, -1);
			return true;
		}

		if (room->GetBattleFieldState() != Packet::BattleFieldState::bf_state_inbattle) {
			player->SendClientNotify("InvalidBattleState", -1, -1);
			return true;
		}

		auto bftarget = room->GetBattlePlayerInfo(message->target_guid());
		if (bftarget == nullptr) {
			player->SendClientNotify("NotJoinRoom", -1, -1);
			return true;
		}
		if (bftarget->locked()) {
			player->SendClientNotify("TargetInBattle", -1, -1);
			return true;
		}

		auto brplayer = room->GetBattlePlayerInfo(player->Guid());
		if (brplayer == nullptr) {
			player->SendClientNotify("NotJoinRoom", -1, -1);
			return true;
		}
		if (brplayer->locked()) {
			 player->SendClientNotify("TargetInBattle", -1, -1);
			return true;
		}

		if (bftarget->dead()) {
			player->SendClientNotify("ActorIsDead", -1, -1);
			return true;
		}
		if (brplayer->dead()) {
			player->SendClientNotify("ActorIsDead", -1, -1);
			return true;
		}

		//上锁先
		bftarget->set_locked(true);
		brplayer->set_locked(true);		

		Server::Instance().SendG2SCommonMessage(player.get(), "G2SCreateBfBattle", { message->battle_group_id() }, { (int64)message->target_guid() }, {});
		
		//auto scene = MtSceneManager::Instance().GetSceneById(player->GetSceneId());
		//if (scene != nullptr) {
		//	auto battle = scene->GetBattleManager().CreatePvpBattle(session->player().get(), target.get(),
		//		{
		//			{ "script_id", 1022 },
		//			{ "group_id", message->battle_group_id() }
		//		});
		//
		//	scene->QueueInLoop(boost::bind(&MtBattleFieldManager::OnCreatBattleResult
		//		, boost::ref(MtBattleFieldManager::Instance())
		//		, session->player().get()
		//		, target->Guid()
		//		, battle != nullptr));			
		//	//scene->QueueInLoop(boost::bind(&MtPlayer::OnGoBattleFieldBattle, session->player(), message->battle_group_id(),target));
		//}
		return true;
	}

	void MtBattleFieldManager::OnCreatBattleResult( MtPlayer * player, const uint64 target_guid, const bool isok)
	{
		if (player == nullptr) {
			return;
		}
		auto proxy = player->GetBattleFeildProxy();
		if (proxy == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}
		auto room = FindRoomById(proxy->raid_rtid());
		if (room == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}

		auto brplayer = room->GetBattlePlayerInfo(player->Guid());
		if (brplayer == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}
		auto bftarget = room->GetBattlePlayerInfo(target_guid);
		if (bftarget == nullptr) {
			return player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}

		if (isok) {
			//可能正在夺旗
			auto target_player = Server::Instance().FindPlayer(target_guid);
			room->OnCancelPickFlag(target_player.get());

			room->BroadcastMsg(*brplayer);
			room->BroadcastMsg(*bftarget);
		}
		else {
			//解锁
			brplayer->set_locked(false);
			bftarget->set_locked(false);
		}
	}

	void MtBattleFieldManager::OnRaidBattleOver(MtPlayer * winner, MtPlayer * losser)
	{
		if (winner == nullptr || losser == nullptr) {
			return;
		}
		auto win_proxy = winner->GetBattleFeildProxy();
		if (win_proxy == nullptr) {
			return winner->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}
		auto room = FindRoomById(win_proxy->raid_rtid());
		if (room == nullptr) {
			return winner->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
		}
		//
		room->OnBattleOver(winner,losser);

		//传送到复活点
		room->OnGoGrave(losser);
	}

	bool MtBattleFieldManager::QuitBattleField(MtPlayer* player)
	{

		auto proxy = player->GetBattleFeildProxy();
		if (proxy == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::InvalidPlayerData);
			return true;
		}
		auto scene = player->Scene();
		if (scene == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::InvalidPlayerData);
			return true;
		}

		auto raid = scene->GetRaid(proxy->raid_rtid());
		if (raid == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::InvalidPlayerData);
			return true;
		}

		Server::Instance().SendG2SCommonMessage(player, "G2SGoTo", { raid->GetOldSceneId(), raid->GetPortalPos().x(),raid->GetPortalPos().y(),raid->GetPortalPos().z() }, {}, {});

		return true;
	}

	bool MtBattleFieldManager::LeaveBattleField(MtPlayer* player)
	{
		if (player == nullptr) {
			return true;
		}
		auto proxy = player->GetBattleFeildProxy();
		if (proxy == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::InvalidPlayerData);
			return true;
		}

		auto room = FindRoomById(proxy->raid_rtid());
		if (room == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::NotJoinRoom);
			return true;
		}

		if (player->BattleGround() != nullptr) {
			//战斗中不能退出战场
			player->SendCommonResult(Packet::ResultOption::BattleField_Opt, Packet::ResultCode::Invalid_Request);
			return false;
		}

		room->OnLeave(player);
		return true;
	}
}