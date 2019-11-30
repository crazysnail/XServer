#include <EnterScene.pb.h>
#include <Team.pb.h>
#include <ClientMove.pb.h>
#include <ActivityPacket.pb.h>
#include "mt_scene.h"
#include "mt_scene_manager.h"
#include "../mt_server/mt_server.h"
#include "../mt_team/mt_team_manager.h"
#include "../base/mt_timer.h"
#include "../mt_player/mt_player.h"
#include "mt_raid.h"
#include "../mt_battle_field/mt_battle_field.h"
#include "../mt_battle_field/mt_battle_field_manager.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_monster/mt_npc.h"
#include <SceneCommon.proto.fflua.h>
#include <S2GMessage.pb.h>

namespace Mt
{

	void MtRaid::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtRaid, ctor()>(ls, "MtRaid")
			.def(&MtRaid::GetBeginDate, "GetBeginDate")
			.def(&MtRaid::GetAttackers, "GetAttackers")
			.def(&MtRaid::GetDefenders, "GetDefenders")
			.def(&MtRaid::RuntimeId, "RuntimeId")
			.def(&MtRaid::GetScriptId,"GetScriptId")
			.def(&MtRaid::OnDestroy, "OnDestroy")
			.def(&MtRaid::GetOwner, "GetOwner")
			.def(&MtRaid::GetState, "GetState")
			.def(&MtRaid::SetState, "SetState")
			.def(&MtRaid::IsEmpty,"IsEmpty")
			.def(&MtRaid::SetRaidTimer, "SetRaidTimer")
			.def(&MtRaid::GetRaidTimer, "GetRaidTimer")
			.def(&MtRaid::OnRaidBattleOver,"OnRaidBattleOver")
			.def(&MtRaid::BroadCastLuaPacket,"BroadCastLuaPacket")
			.def(&MtRaid::Scene, "Scene")
			.def(&MtRaid::GetParams32, "GetParams32")
			.def(&MtRaid::GetParams64, "GetParams64")
			.def(&MtRaid::SetParams32, "SetParams32")
			.def(&MtRaid::SetParams64, "SetParams64")
			.def(&MtRaid::AddNpc, "AddNpc")
			.def(&MtRaid::DelNpc, "DelNpc")		
			.def(&MtRaid::OnGenBuff,"OnGenBuff")
			.def(&MtRaid::OnBuffPoint, "OnBuffPoint")
			;
	}

	MtRaid::MtRaid(const RaidHelper & helper, const int32 rtid)
		: script_id_(helper.script_id_)
		, runtime_id_(rtid)
	{	
		begin_date_ = MtTimerManager::Instance().CurrentDate();
		old_scene_id_ = helper.portal_scene_id_;
		old_portal_pos_ = helper.portal_pos_;
		scene_ = MtSceneManager::Instance().GetSceneById(helper.scene_id_);
		if (scene_ == nullptr)
		{
			ZXERO_ASSERT(false) << "raid scene is null! scene id=" << helper.scene_id_;
			return;
		}

		////
		try {
			thread_lua->call<void>(script_id_, "OnCreate", this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
	}

	void MtRaid::SetRaidTimer(int32 index, int32 second, int32 times) {

		auto iter = raidtimer_.find(index);
		if (iter != raidtimer_.end()) {
			//重新开始
			iter->second->CleanUp();
			iter->second->BeginTimer(index, second, times);
		}
		else {
			auto timer = boost::make_shared<CMyTimer>();
			timer->SetCallBack(boost::bind(&MtRaid::OnRaidTimer,this,_1));
			timer->BeginTimer(index, second, times);
			raidtimer_.insert({ index,timer });
		}
	}

	void MtRaid::OnRaidTimer(int32 index)
	{
		try {
			thread_lua->call<void>(script_id_, "OnRaidTimer", this, index);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
		if (index == 999) {
			OnDestroy();
		}
	}

	boost::shared_ptr<CMyTimer> MtRaid::GetRaidTimer(int32 index)
	{
		auto iter = raidtimer_.find(index);
		if (iter != raidtimer_.end()) {
			return iter->second;
		}
		return nullptr;
	}

	void MtRaid::OnTick(zxero::uint64 elapseTime)
	{
		for (auto iter = raidtimer_.begin(); iter != raidtimer_.end(); ) {
			if (iter->second->IsOver()) {
				iter = raidtimer_.erase(iter);
			}
			else {
				iter->second->OnTick(elapseTime);
				++iter;
			}
		}
	}

	void MtRaid::OnNpcRefreshTime(const int32 delta)
	{
		//npc刷新
		std::vector<boost::shared_ptr<MtNpc>> to_delete;
		for (auto iter = npc_map_.begin(); iter != npc_map_.end(); ) {
			iter->second->OnBigTick(delta);
			if (iter->second->IsDead()) {
				to_delete.push_back(iter->second);
				iter = npc_map_.erase(iter);
			}
			else {
				++iter;
			}
		}

		//通知客户端
		if (!to_delete.empty()) {
			Packet::NpcList reply;
			reply.set_option(Packet::NpcOption::NpcDel);
			for (auto child : to_delete) {
				auto npc_info = reply.add_npc_info();
				npc_info->CopyFrom(*(child->NpcInfo()));
			}
			BroadCastMessage(reply);
		}
	}

	void MtRaid::OnBigTick(uint64 elapseTime) 
	{
		elapseTime;
		if (scene_ == nullptr)
			return;
		
		OnNpcRefreshTime((int32)(elapseTime / 1000));

		try {
			thread_lua->call<void>(script_id_, "OnBigTick", this, elapseTime);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}		
	}

	void MtRaid::OnSecondTick()
	{
		if (scene_ == nullptr)
			return;
		try {
			thread_lua->call<void>(script_id_, "OnSecondTick", this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "OnSecondTick raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
	}

	void MtRaid::OnMinTick()
	{
		if (scene_ == nullptr)
			return;
		try {
			thread_lua->call<void>(script_id_, "OnMinTick", this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "OnMinTick raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
	}

	bool MtRaid::AddPlayer(const boost::shared_ptr<MtPlayer> player, const int32 camp)
	{
		LOG_INFO << "[MtRaid::AddPlayer] guid:" << player->Guid() << " camp=" << camp << " scene id=" << scene_->SceneId() << " pre scene id=" << old_scene_id_;
		if (camp == 1) {
			if (std::find(defenders_.begin(), defenders_.end(), player) == defenders_.end()) {
				defenders_.push_back(player);
				return true;
			}
		}
		else {
			if (std::find(attackers_.begin(), attackers_.end(), player) == attackers_.end()) {
				attackers_.push_back(player);
				return true;
			}
		}

		return false;
	}

	bool MtRaid::DelPlayer(const boost::shared_ptr<MtPlayer> player)
	{
		LOG_INFO << "[MtRaid::DelPlayer] guid:" << player->Guid() << " scene id=" << scene_->SceneId() << " pre scene id=" << old_scene_id_;

		auto aiter = std::find(attackers_.begin(), attackers_.end(), player);
		if (aiter != attackers_.end()) {
			attackers_.erase(aiter);
			return true;
		}
		else {
			auto diter = std::find(defenders_.begin(), defenders_.end(), player);
			if (diter != defenders_.end()) {
				defenders_.erase(diter);
				return true;
			}
		}
		return false;
	}

	void MtRaid::SyncInfo(const boost::shared_ptr<MtPlayer>& player)
	{
		//先离开在进来
		Packet::PlayerLeaveZone leave_notify;
		leave_notify.set_guid(player->Guid());
		BroadCastMessage(leave_notify);

		//通知副本内已有玩家, 有新人加入
		Packet::ScenePlayerInfo msg;
		player->FillScenePlayerInfo(msg);
		BroadCastMessage(msg);

		//取其他人的数据
		Packet::ScenePlayerList player_lists;
		std::for_each(std::begin(attackers_), std::end(attackers_), [&](auto& team_member) {
			if (team_member->Guid() != player->Guid())
				team_member->FillScenePlayerInfo(*player_lists.add_player_list());
		});
		std::for_each(std::begin(defenders_), std::end(defenders_), [&](auto& team_member) {
			if (team_member->Guid() != player->Guid())
				team_member->FillScenePlayerInfo(*player_lists.add_player_list());
		});
		player->ResetRobotPos();
		player->FillRobotPlayer(player_lists);
		player->SendMessage(player_lists);
	}
	
	boost::shared_ptr<MtNpc> MtRaid::FindNpc(const int32 series_id)
	{
		if (npc_map_.find(series_id) != npc_map_.end()) {
			return npc_map_[series_id];
		}
		return nullptr;
	}

	int32 MtRaid::AddNpc(const int32 pos_id, const int32 index, const int32 lifetime)
	{
		if (scene_ == nullptr)
			return -1;
		int32 sceneid = scene_->SceneId();
		auto npc = MtMonsterManager::Instance().CreateNpc(pos_id, index,-1, lifetime);
		if (!npc) {
			ZXERO_ASSERT(false) << "can not find monster data ! index=" << index << " scene_id" << sceneid << " pos_id" << pos_id;
			return -1;
		}

		auto info = npc->NpcInfo();
		int32 series_id = info->series_id();
		auto iter = npc_map_.find(series_id);
		if (iter != npc_map_.end())
			return -1;

		auto npc_data = MtMonsterManager::Instance().FindDynamicNpc(index);
		if (!npc_data) {
			ZXERO_ASSERT(false) << "FindNpcInGroup failed ! index=" << index << " scene_id" << sceneid << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		int32 npc_id = npc_data->npc_id();
		auto monster_config = MtMonsterManager::Instance().FindMonster(npc_id);
		if (!monster_config) {
			ZXERO_ASSERT(false) << "FindMonster failed ! npc_id=" << npc_id << " scene_id" << sceneid << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}
		auto pos_config = MtMonsterManager::Instance().FindScenePos(pos_id);
		if (!pos_config) {
			ZXERO_ASSERT(false) << "FindScenePos failed ! npc_id=" << npc_id << " scene_id" << sceneid << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		if (sceneid != pos_config->scene_id()) {
			ZXERO_ASSERT(false) << "npc owner scene id do not match with current scene ! npc_id=" << npc_id << " scene_id" << sceneid << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		npc_map_.insert({ series_id, npc });

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcAdd);
		auto npc_info = reply.add_npc_info();
		npc_info->CopyFrom(*info);
		BroadCastMessage(reply);

		return series_id;
	}
	void MtRaid::DelNpc(const int32 series_id)
	{
		if (scene_ == nullptr)
			return;

		auto iter = npc_map_.find(series_id);
		if (iter == npc_map_.end())
			return;

		auto npc = iter->second;
		if (npc == nullptr) {
			ZXERO_ASSERT(false) << "can not find npc data !  scene_id" << scene_->SceneId() << "series_id " << series_id;
			return;
		}

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);
		auto npc_info = reply.add_npc_info();
		npc_info->CopyFrom(*(npc->NpcInfo()));

		npc_map_.erase(iter);

		BroadCastMessage(reply);

	}

	void MtRaid::PlayerEnterAddNpc(MtPlayer* player)
	{
		if (player)
		{
			if (npc_map_.size() > 0)
			{
				Packet::NpcList reply;
				reply.set_option(Packet::NpcOption::NpcAdd);
				for (auto npc : npc_map_)
				{
					auto npc_info = reply.add_npc_info();
					npc_info->CopyFrom(*(npc.second->NpcInfo()));
				}
				player->SendMessage(reply);
			}
		}
	}
	void MtRaid::PlayerLeaveDelNpc(MtPlayer* player)
	{
		if (player)
		{
			if (npc_map_.size() > 0)
			{
				Packet::NpcList reply;
				reply.set_option(Packet::NpcOption::NpcDel);
				for (auto npc : npc_map_)
				{
					auto npc_info = reply.add_npc_info();
					npc_info->CopyFrom(*(npc.second->NpcInfo()));
				}
				player->SendMessage(reply);
			}
		}
	}
	void MtRaid::OnEnter(const boost::shared_ptr<MtPlayer> player, const int32 camp)
	{
		if (player == nullptr || scene_ == nullptr)
			return;

		AddPlayer(player, camp);
		player->EnterRaid(shared_from_this());		

		try {
			thread_lua->call<void>(script_id_, "OnEnter", player.get(),this);
			SetState(Packet::RaidState::RaidEnter);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}

		//广播数据
		SyncInfo(player);
		PlayerEnterAddNpc(player.get());
	}

	void MtRaid::OnClientEnterOk(const boost::shared_ptr<MtPlayer>& player)
	{
		try {			
			thread_lua->call<void>(script_id_, "OnClientEnterOk", player.get(), this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
	}

	void MtRaid::OnLeave(const boost::shared_ptr<MtPlayer> player)
	{
		if (player == nullptr)
			return;

		//有战场要先退出战场
		auto proxy = player->GetBattleFeildProxy();
		if (proxy != nullptr  && proxy->raid_rtid() != INVALID_GUID) {
			if (proxy->bf_state() != Config::BFState::Bf_Leave) {
				auto message = boost::make_shared<S2G::S2GCommonMessage>();
				message->set_request_name("S2GQuitBattleField");
				message->add_int64_params(player->Guid());
				Server::Instance().SendMessage(message, nullptr);
			}
		}

		DelPlayer(player);

		//通知剩余玩家, 有人离开了
		Packet::PlayerLeaveZone leave_notify;
		leave_notify.set_guid(player->Guid());
		BroadCastMessage(leave_notify);

		player->SetSceneId(old_scene_id_);
		player->Position(old_portal_pos_);
		player->LeaveRaid();

		PlayerLeaveDelNpc(player.get());
		try {
			thread_lua->call<void>(script_id_, "OnLeave", player.get(), this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
	}

	const boost::shared_ptr<MtPlayer> MtRaid::GetOwner() const {
		if (!attackers_.empty()) {
			return attackers_[0];
		}
		else if (!defenders_.empty()) {
			return defenders_[0];
		}
		return nullptr;
	}

	uint32 MtRaid::RegenBeginDate() {//
		begin_date_ = MtTimerManager::Instance().CurrentDate();
		return begin_date_;
	};

	void MtRaid::OnDestroy()
	{

		auto gotoscene = MtSceneManager::Instance().GetSceneById(old_scene_id_);
		if (gotoscene == nullptr)
			return;

		auto memebers_temp = attackers_;
		for (auto child : memebers_temp){
			//这里会存在副本组队离开的情况，有队伍只管队长，队员自动会跟随传走
			if (child->GetTeamID() == INVALID_GUID || child->IsTeamLeader() ) {
				child->GoTo(old_scene_id_, { old_portal_pos_.x(),old_portal_pos_.y(),old_portal_pos_.z() });
			}		
		}
		
		memebers_temp.clear();

		memebers_temp = defenders_;
		for (auto child : memebers_temp) {
			if (child->GetTeamID() == INVALID_GUID || child->IsTeamLeader() ) {
				child->GoTo(old_scene_id_, { old_portal_pos_.x(),old_portal_pos_.y(),old_portal_pos_.z() });
			}	
		}
		try {
			thread_lua->call<void>(script_id_, "OnDestroy", this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId()<<" script id="<<script_id_;
			return;
		}
		attackers_.clear();
		defenders_.clear();

		SetState(Packet::RaidState::ToDestroy);
	}
	void MtRaid::BroadCastLuaPacket(const std::string &name, const std::vector<int32>& int32values, const std::vector<uint64>& int64values, const std::vector<std::string>& stringvalues)
	{
		Packet::LuaReplyPacket reply;
		reply.set_reply_name(name);
		for (auto child : int32values) {
			reply.add_int32_params(child);
		}
		for (auto child : int64values) {
			reply.add_int64_params(child);
		}
		for (auto child : stringvalues) {
			reply.add_string_params(child);
		}
		BroadCastMessage(reply);
	}
	bool MtRaid::BroadCastMessage(const google::protobuf::Message& msg, const std::vector<uint64>& except_guid)
	{
		for (auto& m : attackers_) {
			if (std::any_of(std::begin(except_guid), std::end(except_guid), [&](auto guid) { return guid == m->Guid(); })) {
				continue;
			}
			else {
				m->SendMessage(msg);
			}
		}
		for (auto& m : defenders_) {
			if (std::any_of(std::begin(except_guid), std::end(except_guid), [&](auto guid) { return guid == m->Guid(); })) {
				continue;
			}
			else {
				m->SendMessage(msg);
			}
		}
		return true;
	}

	//给lua用的一个很疼的接口
	Packet::BattleFieldState MtRaid::GetBattleFieldState()
	{		
		auto room = MtBattleFieldManager::Instance().FindRoomById(runtime_id_);
		if (room == nullptr) {
			return Packet::BattleFieldState::bf_state_invalid;
		}
		return room->GetBattleFieldState();
	}

	bool MtRaid::IsEmpty(const int32 camp) {
		if (camp == -1) {
			return attackers_.empty() && defenders_.empty();
		}
		else if (camp == 0) {
			return attackers_.empty();
		}
		else {
			return defenders_.empty();
		}
	}


	void MtRaid::OnRaidBattleOver(const uint64 winner,const uint64 losser)
	{
		if (scene_ != nullptr) {
			auto winner_player = scene_->FindPlayer(winner);
			auto looser_player = scene_->FindPlayer(losser);
			if (winner_player != nullptr && looser_player != nullptr) {
				scene_->QueueInLoop(boost::bind(&MtBattleFieldManager::OnRaidBattleOver
					, boost::ref(MtBattleFieldManager::Instance())
					, winner_player.get()
					, looser_player.get()));
			}
		}
	}

	void MtRaid::OnGenBuff()
	{
		try {
			thread_lua->call<void>(script_id_, "OnGenBuff", this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}
	}
	
	void MtRaid::OnBuffPoint(MtPlayer * player, const int32 posindex)
	{
		if (player == nullptr) {
			return;
		}

		try {
			thread_lua->call<void>(script_id_, "OnBuffPoint", player,this, posindex);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "raid lua exception:" << e.what() << "scene id=" << scene_->SceneId() << " script id=" << script_id_;
			return;
		}

		return;
	}
}