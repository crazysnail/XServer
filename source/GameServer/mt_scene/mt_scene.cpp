#include "mt_scene.h"
#include "mt_zone.h"
#include <EnterScene.pb.h>
#include <ClientMove.pb.h>
#include "ChatMessage.pb.h"
#include <ActorBasicInfo.pb.h>
#include <Base.pb.h>
#include <BattleGroundInfo.pb.h>
#include <SceneConfig.pb.h>
#include <ItemAndEquip.pb.h>
#include <SceneStage.pb.h>
#include <ItemConfig.pb.h>
#include <Upgrades.pb.h>
#include <HeroFragment.pb.h>
#include <SceneCommon.pb.h>
#include <MonsterConfig.pb.h>
#include <SceneObject.pb.h>
#include <MissionConfig.pb.h>
#include <LootMessage.pb.h>
#include <MissionPacket.pb.h>
#include <BaseConfig.pb.h>
#include <S2GMessage.pb.h>
#include <G2SMessage.pb.h>
#include <S2SMessage.pb.h>
#include <WorldBoss.pb.h>
#include <Team.pb.h>
#include <Guild.pb.h>
#include <DataCellPacket.pb.h>
#include <DrawCard.pb.h>
#include <ActivityPacket.pb.h>
#include <CreatePlayerPacket.pb.h>
#include <ActivityConfig.pb.h>
#include <TrialFieldDB.pb.h>
#include "log.h"
#include "event_loop.h"
#include "tcp_connection.h"
#include "mt_logic_area.h"
#include "../mt_scene/mt_raid.h"
#include "../mt_scene/mt_raid_guildbattle.h"
#include "../base/client_session.h"
#include "../mt_arena/mt_arena.h"
#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_trial_field/mt_trial_field.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_item/mt_item.h"
#include "../mt_item/mt_container.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_stage/mt_player_stage.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_server/mt_server.h"
#include "../mt_market/mt_market.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_monster/mt_npc.h"
#include "../mt_config/mt_config.h"
#include "../mt_mission/mt_mission.h"
#include "../mt_mission/mt_mission_proxy.h"
#include "../base/mt_timer.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_upgrade/mt_upgrade.h"
#include "../mt_chat/mt_chatmessage.h"
#include "../mt_chat/mt_word_manager.h"
#include "../mail/static_message_manager.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_data_cell/mt_data_cell.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_guild/mt_guild_manager.h"
#include "../mt_guild/mt_player_guild_proxy.h"
#include "../mt_lua/mt_lua_initializer.h"
#include "../mt_battle/mt_worldboss_manager.h"
#include "../mt_activity/mt_activity.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_battle_field/mt_battle_field.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_battle_field/mt_battle_field_manager.h"
#include "../mt_config/mt_config.h"
#include "utils.h"
#include "../mt_arena/geometry.h"
#include "../base/client_protocol_dispatcher.h"
#include "../base/mt_timer.h"
#include <google/protobuf/message.h>
#include <boost/make_shared.hpp>
#include <boost/format.hpp>
#include <SceneStageConfig.pb.h>
#include <boost/make_unique.hpp>

void clear_fflua(ff::fflua_t*) {};
namespace Mt
{
	//boost::thread_specific_ptr<ff::fflua_t> thread_lua(clear_fflua);
	boost::thread_specific_ptr<ff::fflua_t> thread_lua;
	void MtScene::lua_reg(lua_State* ls)
	{

		//ff::fflua_register_t<google::protobuf::Message, ctor()>(ls, "Message");

		ff::fflua_register_t<MtScene, ctor()>(ls, "MtScene")
			.def(&MtScene::DelNpcByType, "DelNpcByType")
			.def(&MtScene::AddNpc, "AddNpc")
			.def(&MtScene::SceneId, "SceneId")
			.def(&MtScene::GetSceneData, "GetSceneData")
			.def(&MtScene::DelRaid, "DelRaid")
			.def(&MtScene::Config,"Config")			
			.def(&MtScene::GetRaid,"GetRaid")
			.def(&MtScene::OnPlayerTrialInfoLoad, "OnPlayerTrialInfoLoad")
			.def(&MtScene::BroadcastNotify,"BroadcastNotify")
			.def(&MtScene::GetLuaCallProxy,"GetLuaCallProxy")
			.def(&MtScene::FindPlayer, "FindPlayer")
			.def(&MtScene::LuaDispatchMessage, "DispatchMessage")
			.def(&MtScene::LuaExecuteMessage,  "ExecuteMessage")
			.def(&MtScene::SendS2SCommonMessage,"SendS2SCommonMessage")
			.def(&MtScene::GetBattleManager,"GetBattleManager")
			.def(&MtScene::OnMinTriger, "OnMinTriger")
			.def(&MtScene::OnHourTriger, "OnHourTriger")
			.def(&MtScene::OnDayTriger, "OnDayTriger")
			.def(&MtScene::OnWeekTriger, "OnWeekTriger")
			.def(&MtScene::OnMonthTriger, "OnMonthTriger")
			.def(&MtScene::DelNpc, "DelNpc");

		ff::fflua_register_t<>(ls)
			.def(&send_system_message_toworld, "Lua_send_system_message_toworld")
			.def(&send_run_massage, "Lua_send_run_massage");                //! 注册静态函数
	}

	using namespace zxero;
	MtScene::~MtScene()
	{
		//nothing todo currently.
	}
	MtScene::MtScene(const boost::shared_ptr<Config::Scene>& scene_config, network::event_loop* event_loop_)
		:config_(scene_config), loop_(event_loop_)
	{
		auto celldb = boost::make_shared<Packet::DataCell>();
		for (int32 k = 0; k < MAX_CHUNK_SIZE; k++) {
			celldb->add_data(0);
		}
		scene_cell_ = boost::make_shared<MtDataCell>(celldb);
		scene_cell_->reset();

		scene_area_ = boost::make_shared<Rect<int32>>(
			point_xy<int32>(config_->areas(0).x(), config_->areas(0).z()),
			point_xy<int32>(config_->areas(1).x(), config_->areas(1).z()),
			point_xy<int32>(config_->areas(2).x(), config_->areas(2).z()),
			point_xy<int32>(config_->areas(3).x(), config_->areas(3).z())
			);
		/*scene_area_ = boost::make_shared<Rect<int32>>(point_xy<int32>(0, 0), point_xy<int32>(config_->height(), 0),
		point_xy<int32>(config_->height(), config_->width()), point_xy<int32>(0, config_->width()));*/
		DispatcherInit();

		fix_offset_.x(0);
		fix_offset_.y(0);
		auto x_min_max = std::minmax_element(std::begin(scene_area_->outer()), std::end(scene_area_->outer()), [](auto& l, auto& r) { return l.x() < r.x(); });
		auto y_min_max = std::minmax_element(std::begin(scene_area_->outer()), std::end(scene_area_->outer()), [](auto& l, auto& r) { return l.y() < r.y(); });
		fix_offset_.x(-x_min_max.first->x());
		fix_offset_.y(-y_min_max.first->y());
		width_ = x_min_max.second->x() - x_min_max.first->x();
		if (width_ % (int32)MtZone::ZoneSize::WIDTH != 0) {
			width_ = (width_ / (int32)MtZone::ZoneSize::WIDTH + 1) * (int32)MtZone::ZoneSize::WIDTH;
		}
		height_ = y_min_max.second->y() - y_min_max.first->y();
		if (height_ % (int32)MtZone::ZoneSize::HEIGHT != 0) {
			height_ = (height_ / (int32)MtZone::ZoneSize::HEIGHT + 1) * (int32)MtZone::ZoneSize::HEIGHT;
		}
		zone_count_ = (width_ / (int32)MtZone::ZoneSize::WIDTH) * (height_ / (int32)MtZone::ZoneSize::HEIGHT);
		last_tick_ = now_millisecond();
		loop_->run_in_loop([&]() {LuaInit(); });
		loop_->run_every(milliseconds(100), boost::bind(&MtScene::OnTick, this));
		loop_->run_every(seconds(BIG_TICK), boost::bind(&MtScene::OnBigTick, this));
		loop_->run_every(seconds(1), boost::bind(&MtScene::OnSecondTick, this));
		loop_->run_every(seconds(MIN_TICK), boost::bind(&MtScene::OnMinTick, this));
		//loop_->run_every(seconds(CACHE_TICK), boost::bind(&MtScene::OnCacheTick, this));
		for (int32 i = 0; i < SCENE_TICK_HISTROY; ++i)
		{
			for (int32 j = 0; j < ProfAddrType::MAX_TICK_COUNT; ++j) {
				ticks_[i][j] = 0;
			}
		}


	}

	void  MtScene::DispatcherInit()
	{
		inner_message_dispatcher_ = boost::make_unique<message_dispatcher<MtScene, MtPlayer>>(this, loop_);
		inner_message_dispatcher_->default_handle(&MtScene::UnRegistedMessage);

		inner_message_dispatcher_->add_handler(&MtScene::OnG2SCommonMessage).
			add_handler(&MtScene::OnS2SCommonMessage).
			add_handler(&MtScene::OnCollectTax).
			add_handler(&MtScene::OnCaptureStageToScene).
			add_handler(&MtScene::OnG2SSyncPlayerGuildData).
			add_handler(&static_message_manager::gs2s_add_mail_to_player_s).
			add_handler(&MtScene::OnRobotPlayerInOk).
			add_handler(&MtScene::OnG2SCodeCheckDone)
			;
	
		client_message_dispatcher_ = boost::make_unique<message_dispatcher<MtScene, MtPlayer>>(this, loop_);
		client_message_dispatcher_->default_handle(&MtScene::UnRegistedMessage);
		client_message_dispatcher_->add_handler(&MtScene::OnClientStop).
			add_handler(&MtScene::OnClientMove).
			add_handler(&MtScene::OnChangeHair).
			add_handler(&MtScene::OnArenaPanelOpen).
			add_handler(&MtScene::OnArenaPanelClose).
			add_handler(&MtScene::OnArenaRefreshTarget).
			add_handler(&MtScene::OnArenaChallengePlayer).
			add_handler(&MtScene::OnArenaBuyChallengeCount).
			add_handler(&MtScene::OnTrialPlayerLoadDone).
			add_handler(&MtScene::OnLootPlayer).
			add_handler(&MtScene::OnClientEnterSceneOk).
			add_handler(&MtScene::OnPlayerChangeScene).
			add_handler(&MtScene::OnSessoinChanged).
			add_handler(&MtScene::OnPlayerLeaveScene).
			add_handler(&MtScene::OnActorList).
			add_handler(&MtScene::OnChatMessage).
			add_handler(&MtScene::OnActorFormationPull).
			add_handler(&MtScene::OnSetActorFight).
			add_handler(&MtScene::OnSyncMissionReq).
			add_handler(&MtScene::OnClickChatMissionReq).
			add_handler(&MtScene::ExitBattleGround).
			add_handler(&MtScene::OnUpdateFollowState).
			add_handler(&MtScene::OnChallengeBoss).
			add_handler(&MtScene::OnConstantChallengeBoss).
			add_handler(&MtScene::OnPlayerReachBattlePos).
			add_handler(&MtScene::OnPlayerContainerRequet).
			add_handler(&MtScene::OnActorAddEquipRequest).
			add_handler(&MtScene::OnActorDelEquipRequest).
			add_handler(&MtScene::OnUpgradeLevel).
			add_handler(&MtScene::OnUpgradeColor).
			add_handler(&MtScene::OnUpgradeStar).
			add_handler(&MtScene::OnUpgradeSKill).
			add_handler(&MtScene::OnEnhenceEquip).
			add_handler(&MtScene::OnAddEnchantSlot).
			add_handler(&MtScene::OnEnchantEquip).
			add_handler(&MtScene::OnBatchEnchantEquip).
			add_handler(&MtScene::OnBatchEnchantEquipConfirm).
			add_handler(&MtScene::OnInsetGem2Equip).
			add_handler(&MtScene::OnTakeOutGem).
			add_handler(&MtScene::OnEquipInherit).
			add_handler(&MtScene::OnFixEquipReq).
			add_handler(&MtScene::OnDismantleEquipReq).
			add_handler(&MtScene::OnGemFusionReq).
			add_handler(&MtScene::OnHeroFragmentFusion).
			add_handler(&MtScene::OnCommonRequest).
			add_handler(&MtScene::OnEquipMagicalReq).
			add_handler(&MtScene::OnMoveItemReq).
			add_handler(&MtScene::OnExtendContainerReq).
			add_handler(&MtScene::OnNpcSessionReq).
			add_handler(&MtScene::OnNoNpcSessionReq).
			add_handler(&MtScene::OnPlayerNpcSessionReq).
			add_handler(&MtScene::OnPlayerStartHook).
			add_handler(&MtScene::OnPlayerEndHook).
			add_handler(&MtScene::OnChangeHookInfo).
			add_handler(&MtScene::OnDelMessageReq).
			add_handler(&MtScene::OnBattleGroundMoveFinish).
			add_handler(&MtScene::OnBattleGroundSetControlType).
			add_handler(&MtScene::OnBattleGroundClientActorMove).
			add_handler(&MtScene::OnBattleGroundClientActorStop).
			add_handler(&MtScene::OnBattleGroundClientActorUseSkill).
			add_handler(&MtScene::OnWorldBossInfoReq).
			add_handler(&MtScene::OnWorldBossHpReq).
			add_handler(&MtScene::OnLeaveWorldBossReq).
			add_handler(&MtScene::OnWorldBossRelivenow).
			add_handler(&MtScene::OnWorldBossGoldInspire).
			add_handler(&MtScene::OnWorldBossDiamondInspire).
			add_handler(&MtScene::OnWorldBossStatisticsReq).
			add_handler(&MtScene::OnSetWorldBossAutoFightReq).
			add_handler(&MtScene::OnReNameRequest).
			add_handler(&MtScene::OnPlayerOperateInfoReq).
			add_handler(&MtScene::OnSendGiftReq).
			add_handler(&MtGuildManager::OnGuildBattleFightTarget).
			add_handler(&MtScene::OnLuaRequestPacket).
			add_handler(&MtScene::OnPlayerEnterScene).
			add_handler(&MtScene::OnCacheAll).
			add_handler(&MtScene::OnXLuaReg).
			add_handler(&MtScene::OnLuaLoad).
			add_handler(&MtScene::OnPlayerDisconnect).
			add_handler(&MtScene::OnSyncPlayerTeamData).
			add_handler(&MtScene::OnResetPlayerTeamData)
			;
	}

	void MtScene::LuaInit()
	{
		try
		{
			lua_ = thread_lua.get();
			MtLuaInitializer::Instance().SceneDataInit(*lua_);
			MtSkillManager::Instance().InitLuaContext();
			lua_call_.Init();
			//初始化活动配置数据
			thread_lua->call<int>(4, "OnOperateConfigInit", this);
		}
		catch (ff::lua_exception& e)
		{
			ZXERO_ASSERT(false) << "LuaInit exception:" << e.what() << "scene id=" << SceneId();
		}
	}

	bool MtScene::BroadcastNotify(const std::string& content) {
		Packet::Notify notify;
		notify.set_notify(content);
		BroadcastMessage(notify);
		return true;
	}

	bool MtScene::BroadcastMessage(const google::protobuf::Message& message, const std::vector<uint64>& except_guids)
	{
		for (auto it : zone_map_) {
			it.second->BroadcastMessage(message, except_guids);
		}
		return true;
	}

	bool MtScene::OnPlayerEnterBegin(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message)
	{ 
		player;
		message;
		return true;
	}

	Packet::Position MtScene::SpawnPos(int32 index)
	{
		if (index >= 0 && index < config_->spawn_pos_size())
			return config_->spawn_pos(index);
		return Packet::Position::default_instance();
	}

	bool MtScene::OnPlayerEnter(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::PlayerEnterScene>& message,
		bool full_sync)
	{
		auto & init_pos = message->has_init_pos() ? message->init_pos() : config_->spawn_pos(0);
		player->Direction(0.f);
		player->SetMoveSpeed(4000);		
		player->Position(init_pos);

		if (!ValidPos(player->Position())) {
			LOG_ERROR << "invalid position! scene id=" << config_->id() << " player guid=" << player->Guid();
			player->Position(config_->spawn_pos(0));
			message->mutable_init_pos()->CopyFrom(player->Position());
		}
		if (player_map_.find(player->Guid()) != std::end(player_map_)) {
			ZXERO_ASSERT(false) << "the player is in the scene ! scene id=" << SceneId() << " guid=" << player->Guid();
			//容错,说明之前没有正常的离开这个场景
			OnPlayerLeave(player);
		}

		OnPlayerEnterBegin(player, message);

		__regist_player_2_scene(player);
		player->CheckInit();
		//这个时候再设置idel状态，保证entering期间逻辑不会被客户端频繁的包打断
		player->GetScenePlayerInfo()->set_player_status(Packet::PlayerSceneStatus::IDEL);
		if (full_sync) {
			player->OnLogin();
		}
		__after_player_regist_2_scene(player);
		auto init_area = GetInitLogicArea(message);
		if (!init_area.Empty()) {
			init_area.OnPlayerEnter(player, nullptr, message);
		}
		else {
			ZXERO_ASSERT(false) << "invalid init_area!";
		}
		player->EnterSceneSync();
		OnPlayerEnterEnd(player, message);	

		return true;
	}

	bool MtScene::OnPlayerEnterEnd(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEnterScene>& message)
	{
		if (player->GetTeamID() != INVALID_GUID) {//同步给队友
			auto member_change_scene = boost::make_shared<S2G::TeamMemberChangeScene>();
			member_change_scene->set_scene_id(player->GetSceneId());
			if (message->has_init_pos()) {
				member_change_scene->mutable_init_pos()->CopyFrom(message->init_pos());
			}
			else {
				//取队长位置
				member_change_scene->mutable_init_pos()->CopyFrom(player->Position());
			}
			Server::Instance().SendMessage(member_change_scene, player);
		}
		return true;
	}
	bool MtScene::TransportByPosIndex(const boost::shared_ptr<MtPlayer>& player, int32 posindex)
	{
		auto sceneconfig = MtSceneManager::Instance().GetSceneConfig(SceneId());
		if (sceneconfig == nullptr)
			return false;
		auto init_pos = sceneconfig->spawn_pos(posindex);
		Transport(player, init_pos);
		return true;
	}

	bool MtScene::Transport(const boost::shared_ptr<MtPlayer>& player, const Packet::Position& init_pos)
	{
		if (!player->CanGo()) {
			return false;
		}
		player->Direction(0.f);
		player->SetMoveSpeed(4000);
		player->GetScenePlayerInfo()->set_player_status(Packet::PlayerSceneStatus::IDEL);
		
		if (init_pos.x() >= 0 && init_pos.y() >= 0 && init_pos.z()>=0) {
			player->Position(init_pos);
		}
		else {
			player->Position(config_->spawn_pos(0));			
		}

		if (!ValidPos(player->Position())) {
			ZXERO_ASSERT(false) << "invalid position! scene id=" << config_->id() << " player guid=" << player->Guid();
			player->Position(config_->spawn_pos(0));
		}

		//顺带更新一下自己的客户端
		Packet::ScenePlayerInfo msg;
		player->FillScenePlayerInfo(msg);
		player->BroadCastMessage(msg);

		if (player->GetTeamID() != INVALID_GUID) {//同步给队友
			auto member_change_scene = boost::make_shared<S2G::TeamMemberChangeScene>();
			member_change_scene->set_scene_id(player->GetSceneId());
			member_change_scene->mutable_init_pos()->CopyFrom(init_pos);
			Server::Instance().SendMessage(member_change_scene, player);
		}
		player->ChangeSceneOk();
		return true;
	}

	bool MtScene::OnPlayerReenter(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player_map_.find(player->Guid()) == std::end(player_map_)) {
			ZXERO_ASSERT(false) << "player not exist sceneid= " << SceneId() << "player guid=" << player->Guid();
			return false;
		}

		auto area = GetCurrentLogicArea(player);
		if (area.Empty()) {
			ZXERO_ASSERT(false) << "reenter area is null ! guid=" << player->Guid();
			return false;
		}
		return area.OnPlayerReenter(player);
	}

	bool MtScene::OnPlayerLeave(const boost::shared_ptr<MtPlayer>& player)
	{
		auto logic_area = GetCurrentLogicArea(player);
		if (!logic_area.Empty()) {
			Packet::ScenePlayerExit notify;
			notify.set_guid(player->Guid());
			notify.set_scene_id(SceneId());
			logic_area.BroadCastMessage(notify, { player->Guid() });
			logic_area.OnPlayerLeave(player);
		}
		else {
			ZXERO_ASSERT(false) << "logic area is null ! player guid=" 
				<< player->Guid() << " scene id=" << SceneId();
		}
		__on_player_leave(player);

		try {
			thread_lua->call<void>(4, "OnPlayerLeave", this, player);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what() << "scene id=" << SceneId();
			return true;
		}
		return true;
	}

	boost::shared_ptr<MtZone> MtScene::GetZoneByPos(const Packet::Position&  pos)
	{
		if (!ValidPos(pos)) {
			LOG_INFO<< "invalid pos :x=" << pos.x() << " y = " << pos.y() << " z=" << pos.z();
			return nullptr;
		}
		else {
			point_xy<int32> fix_pos;
			fix_pos.x(fix_offset_.x() + pos.x());
			fix_pos.y(fix_offset_.y() + pos.z());
			auto zone_id = fix_pos.x() / (int32)MtZone::ZoneSize::WIDTH +
				fix_pos.y() / (int32)MtZone::ZoneSize::HEIGHT * (width_ / (int32)MtZone::ZoneSize::WIDTH);
			return this->GetZoneById(zone_id);
		}
	}

	boost::shared_ptr<MtZone> MtScene::GetZoneById(zxero::int32 zone_id)
	{
		if (zone_map_.find(zone_id) == zone_map_.end()) {
			auto tmp = boost::make_shared<MtZone>(zone_id, this->shared_from_this());
			zone_map_.insert(std::make_pair(zone_id, tmp));
			return tmp;
		}
		else {
			return zone_map_[zone_id];
		}
	}

	void MtScene::OnTick()
	{
		auto current_time = MtTimerManager::Instance().CurrentTime();
		auto elapseTime = current_time - last_tick_;
		MtSceneManager::Instance().UpdateSceneTick(SceneId(), int32(elapseTime));
		frame_elapseTime_ = elapseTime;
		last_tick_ = current_time;
		decltype(player_map_) temp_map;
		{
			profiler2addr prof(ticks_[tick_index_] + ProfAddrType::PLAYER_COPY_TICK);
			temp_map = player_map_;//防止迭代器失效
		}
		{//tick中需要避免player_map_被修改
			tick_player_map_locked_ = true;
			profiler2addr prof(ticks_[tick_index_] + ProfAddrType::PLAYER_TICK);
			boost::for_each(temp_map, [=](auto it) {
				it.second->OnTick(elapseTime);
			});
			tick_player_map_locked_ = false;
		}
		{
			profiler2addr prof(ticks_[tick_index_] + ProfAddrType::RAIDS_TICK);
			boost::for_each(raids_, [=](auto it) {
				it.second->OnTick(elapseTime);
			});
		}
		{
			profiler2addr prof(ticks_[tick_index_] + ProfAddrType::BATTLE_TICK);
			battle_manager_.OnTick(elapseTime);
		}


		{//scene有可能为空哦
			profiler2addr prof(ticks_[tick_index_] + ProfAddrType::PLAYER_MSG_TICK);
			boost::for_each(temp_map, [=](auto& it) {
				it.second->HandleMessage();
			});
		}
		if (++tick_index_ >= SCENE_TICK_HISTROY) { tick_index_ = 0; }
	}

	void MtScene::OnNpcRefreshTime(const int32 delta)
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
			BroadcastMessage(reply);
		}
	}

	void MtScene::OnBigTick()
	{
		//低频tick
		profiler2addr profa(ticks_[big_tick_index_] + ProfAddrType::SCENE_BIG_TICK);
		auto current_time = MtTimerManager::Instance().CurrentTime();
		auto elapseTime = current_time - last_big_tick_;
		last_big_tick_ = current_time;
		{
			profiler2addr prof(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK);
			std::for_each(std::begin(player_map_), std::end(player_map_), [=](auto it) {
				it.second->OnBigTick(elapseTime);
			});
			{
				profiler2addr prof1(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK_1);
				std::map<int32, int32> hook_distributes;
				std::for_each(std::begin(player_map_), std::end(player_map_), [&](auto it) {
					it.second->OnBigTick_1(elapseTime);
					if (it.second->GetPlayerStageDB()) {
						hook_distributes[it.second->GetPlayerStageDB()->current_stage()]++;
					}
				});
				if (SceneId() < 100) {//普通场景统计挂机人数, 副本不统计
					auto msg = boost::make_shared<S2G::HookStageDistributeUpdate>();
					msg->set_scene_id(SceneId());
					for (auto& pair : hook_distributes) {
						msg->add_stage_id(pair.first);
						msg->add_player_count(pair.second);
					}
					Server::Instance().SendMessage(msg, nullptr);
				}
			}
			{
				profiler2addr prof2(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK_2);
				std::for_each(std::begin(player_map_), std::end(player_map_), [=](auto it) {
					it.second->OnBigTick_2(elapseTime);
				});
			}
			{
				profiler2addr prof3(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK_3);
				std::for_each(std::begin(player_map_), std::end(player_map_), [=](auto it) {
					it.second->OnBigTick_3(elapseTime);
				});
			}
			{
				profiler2addr prof4(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK_4);
				std::for_each(std::begin(player_map_), std::end(player_map_), [=](auto it) {
					it.second->OnBigTick_4(elapseTime);
				});
			}
			{
				profiler2addr prof5(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK_5);
				std::for_each(std::begin(player_map_), std::end(player_map_), [=](auto it) {
					it.second->OnBigTick_5(elapseTime);
				});
			}
			{
				profiler2addr prof6(ticks_[big_tick_index_] + ProfAddrType::SCENE_PLAYER_BIG_TICK_6);
				std::for_each(std::begin(player_map_), std::end(player_map_), [=](auto it) {
					it.second->OnBigTick_6(elapseTime);
				});
			}
		}
		{
			profiler2addr prof(ticks_[big_tick_index_] + ProfAddrType::SCENE_RAID_BIG_TICK);
			std::for_each(std::begin(raids_), std::end(raids_), [=](auto it) {
				it.second->OnBigTick(elapseTime);
			});
		}

		{
			profiler2addr prof(ticks_[big_tick_index_] + ProfAddrType::SCENE_NPC_BIG_TICK);
			OnNpcRefreshTime((int32)(elapseTime / 1000));
		}
		{
			profiler2addr prof(ticks_[big_tick_index_] + ProfAddrType::SCENE_RAID_DES_BIG_TICK);
			//回收raid
			for (auto iter = raids_.begin(); iter != raids_.end();) {
				if (iter->second->CanDestroy()) {
					//如果有对应的房间，通知主线程销毁
					//Server::Instance().RunInLoop(boost::bind(&MtBattleFieldManager::OnDestroyRoom, boost::ref(MtBattleFieldManager::Instance()), iter->first));
					auto msg = boost::make_shared<S2G::DestroyRoom>();
					msg->set_rtid(iter->first);
					Server::Instance().SendMessage(msg);

					iter = raids_.erase(iter);
				} else {
					++iter;
				}
			}
		}

		if (++big_tick_index_ >= SCENE_TICK_HISTROY) big_tick_index_ = 0;
	}

	void MtScene::OnSecondTick()
	{
		profiler2addr prof(ticks_[sec_tick_index_++] + ProfAddrType::SCENE_SEC_TICK);
		if (sec_tick_index_ >= SCENE_TICK_HISTROY) sec_tick_index_ = 0;
		std::for_each(std::begin(raids_), std::end(raids_), [=](auto it) {
			it.second->OnSecondTick();
		});

		try {
			thread_lua->call<void>(4, "OnSecondTick", this);
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what() << "scene id=" << SceneId();
			return;
		}
	}

	void MtScene::OnMonthTriger(int32 month)
	{
		for (auto child : player_map_) {
			child.second->OnMonthTriger(month);
		}
	}

	void MtScene::OnWeekTriger(int32 week)
	{
		for (auto child : player_map_) {
			child.second->OnWeekTriger(week);
		}
	}
	void MtScene::OnDayTriger(int32 day)
	{
		for (auto child : player_map_) {
			child.second->OnDayTriger(day,true);
		}
	}
	void MtScene::OnHourTriger(int32 hour)
	{
		for (auto child : player_map_) {
			child.second->OnHourTriger(hour);
		}
	}
	void MtScene::OnMinTriger(int32 min)
	{
		for (auto child : player_map_) {
			child.second->OnMinTriger(min);
		}
	}

	void MtScene::OnMinTick()
	{
		{
			profiler2addr prof(ticks_[min_tick_index_++] + ProfAddrType::SCENE_MIN_TICK);
			if (min_tick_index_ >= SCENE_TICK_HISTROY) min_tick_index_ = 0;
			std::for_each(std::begin(raids_), std::end(raids_), [=](auto it) {
				it.second->OnMinTick();
			});
		}
		OutputTickProf();
	}

	void MtScene::OutputTickProf() const 
	{
		if (SceneId() > 100) return;
		if (!MtConfig::Instance().output_scene_tick_info_) return;
		LOG_ERROR << "scene:" << SceneId() << ",player:" << player_map_.size()
			<< ",battle:" << battle_manager_.BattleCount();
		for (int32 i = 0; i < SCENE_TICK_HISTROY; ++i) {
			LOG_ERROR << "tick -> scene:"<< SceneId()
				<< "," << ticks_[i][ProfAddrType::PLAYER_COPY_TICK]
				<< "," << ticks_[i][ProfAddrType::PLAYER_TICK]
				<< "," << ticks_[i][ProfAddrType::RAIDS_TICK]
				<< "," << ticks_[i][ProfAddrType::BATTLE_TICK]
				<< "," << ticks_[i][ProfAddrType::PLAYER_MSG_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_BIG_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK_1]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK_2]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK_3]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK_4]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK_5]
				<< "," << ticks_[i][ProfAddrType::SCENE_PLAYER_BIG_TICK_6]
				<< "," << ticks_[i][ProfAddrType::SCENE_RAID_BIG_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_NPC_BIG_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_RAID_DES_BIG_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_SEC_TICK]
				<< "," << ticks_[i][ProfAddrType::SCENE_MIN_TICK];

		}

	}

	std::vector<boost::shared_ptr<MtZone>> MtScene::GetRelatedZone(const boost::shared_ptr<MtZone>& centre_zone)
	{
		auto zone_id = centre_zone->GetZoneId();
		std::vector<boost::shared_ptr<MtZone>> zones;

		//九宫格, 获取附近的8个格子. 1 找到centre_zone,以及上下的两个格子, 现在共有三个格子 2 取这三个格子的各自的左右格子, 共六个, 合九个
		//step 1 上下格子
		auto x_len = width_ / (int32)MtZone::ZoneSize::WIDTH;
		//auto y_len = height_ / (int32)MtZone::ZoneSize::HEIGHT;

		decltype(zones) step_one_zones;
		if (zone_id > x_len &&
			zone_id < zone_count_ - x_len) {
			step_one_zones.push_back(this->GetZoneById(zone_id - x_len)); //上面的格子
			step_one_zones.push_back(centre_zone);
			step_one_zones.push_back(this->GetZoneById(zone_id + x_len)); //下面的格子
		}
		else if (zone_id <= x_len) {//没有下面的格子
			step_one_zones.push_back(centre_zone);
			step_one_zones.push_back(this->GetZoneById(zone_id + x_len));
		}
		else {//没有上面的格子
			step_one_zones.push_back(this->GetZoneById(zone_id - x_len));
			step_one_zones.push_back(centre_zone);
		}

		//setp 2 三个格子的左右格子
		for (auto& zone : step_one_zones) {

			auto sub_zone_id = zone->GetZoneId();
			if (sub_zone_id % x_len == 0) { //没有左边格子
				zones.push_back(zone);
				zones.push_back(this->GetZoneById(sub_zone_id + 1));
			}
			else if ((sub_zone_id + 1) % x_len == 0) { //没有右边格子
				zones.push_back(this->GetZoneById(sub_zone_id - 1));
				zones.push_back(zone);
			}
			else {
				zones.push_back(this->GetZoneById(sub_zone_id - 1)); //左边格子
				zones.push_back(zone);
				zones.push_back(this->GetZoneById(sub_zone_id + 1)); //右边格子
			}
		}
		return zones;
	}
	bool MtScene::BroadcastMessageRelatedZones(const boost::shared_ptr<MtZone> centre_zone,
		const google::protobuf::Message& message, const std::vector<uint64>&except_guids)
	{

		auto related_zones = GetRelatedZone(centre_zone);
		//发送消息
		std::for_each(std::begin(related_zones), std::end(related_zones), [&](auto it) {
			it->BroadcastMessage(message, except_guids);
		});
		return true;
	}

	void MtScene::QueueInLoop(boost::function<void()> functor)
	{
		if (loop_ != nullptr)
			loop_->queue_in_loop(functor);
	}
	
	bool MtScene::ValidPos(const Packet::Position& pos)
	{
		return scene_area_->Contains(point_xy<int32>((int32)pos.x(), (int32)pos.z()));
	}

	bool MtScene::OnSessoinChanged(const boost::shared_ptr<MtPlayer>& player, 
		const boost::shared_ptr<Packet::SessionChanged>& /*msg*/,
		int32 /*source*/)
	{
		player->ReConnected();
		__after_player_regist_2_scene(player);
		OnPlayerReenter(player);
		try {
			thread_lua->call<void>(4, "OnPlayerEnter", this, player);
		}
		catch (ff::lua_exception& e) {
			LOG_WARN << "[OnPlayerReenter] guid:" << player->Guid()
				<< ".lua exception:" << e.what();
		}

		return true;
	}

	bool MtScene::OnPlayerEnterScene(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::PlayerEnterScene>& message,
		int32 source)
	{
		LOG_INFO << "[enter_scene] got msg. session " << player->Session()
			<< " player" << player << " guid:" << player->Guid()
			<< ",enter scene. target_scene_id:" << message->scene_id()
			<< ",source:" << source << ",status:"
			<< player->GetScenePlayerInfo()->player_status()
			<< ",curr_id:" << SceneId();
		if (player->GetScenePlayerInfo()->player_status() == Packet::PlayerSceneStatus::WAIT_FOR_ENTER)
		{
			if (player->Scene() != nullptr) {
				LOG_WARN << "[enter_scene] guid:" << player->Guid()
					<< ". curr scene: " << SceneId()
					<< ". orig scene:" << player->Scene()->SceneId();
			}
			//把玩家注册到当前场景
			LOG_INFO << "[enter_scene] guid:" << player->Guid() << " wait for enter regist";
			OnPlayerEnter(player, message, true);
			try {
				thread_lua->call<void>(4, "OnPlayerEnter", this, player);
			}
			catch (ff::lua_exception& e) {
				LOG_WARN << "[enter_scene] guid:" << player->Guid()
					<< ".lua exception:" << e.what();
				return true;
			}
			LOG_INFO << "[enter_scene] guid:" << player->Guid() << " wait for enter regist done";
		} else if (player->GetScenePlayerInfo()->player_status() == Packet::PlayerSceneStatus::READY_TO_ENTER) {
			if (player->Scene() != nullptr) {
				LOG_WARN << "[enter_scene] guid:" << player->Guid()
					<< ". already has scene. curr scene: " << SceneId()
					<< ". orig scene:" << player->Scene()->SceneId();
			}
			//把玩家注册到当前场景
			LOG_INFO << "[enter_scene] guid:" << player->Guid() << " ready to enter regist";
			OnPlayerEnter(player, message, false);
			try {
				thread_lua->call<void>(4, "OnPlayerEnter", this, player);
			}
			catch (ff::lua_exception& e) {
				LOG_WARN << "lua exception:" << e.what() << "scene id=" << SceneId();
				return true;
			}
			LOG_INFO << "[enter_scene] guid:" << player->Guid() << " ready to enter regist done";
		} else if (player->GetScenePlayerInfo()->player_status() == Packet::PlayerSceneStatus::CHANGE_SCENE) {
			if (!player->CanGo()) {
				//切换失败
				LOG_WARN << "[enter_scene] guid:" << player->Guid() << " change scene cannot change";
				return true;
			}
			player->SetChangeSceneCD(8000);
			//1 从当前场景释放player
			//2 给目标场景发送进入消息
			LOG_INFO << "[enter_scene] guid:" << player->Guid() << " change scene.";
			auto target_scene = MtSceneManager::Instance().GetSceneById(message->scene_id());
			if (message->scene_id() == SceneId()) {//同一场景, 不需要切换
				if (message->has_init_pos()) {
					LOG_INFO << "[enter_scene] guid:" << player->Guid() << " change scene same scene transport";
					Transport(player, message->init_pos());
					return true;
				} else {
					LOG_WARN << "[enter_scene] guid:" << player->Guid() << " change scene same scene fail";
					return true;
				}
			}
			if (target_scene == nullptr) {//目标场景为空,无法切换
				LOG_WARN << "[enter_scene] guid:" << player->Guid() << " change scene target scene null";
				return true;
			}
			LOG_INFO << "[enter_scene] guid:" << player->Guid() << " change scene unregist";
			OnPlayerLeave(player);
			player->GetScenePlayerInfo()->set_player_status(Packet::PlayerSceneStatus::READY_TO_ENTER);
			target_scene->DispatchMessage(message, player, MessageSource::GAME_EXECUTE);
			return false; //!!! 退出当前玩家消息执行循环
		} else {
			LOG_INFO << "[enter_scene] guid:" << player->Guid()
				<< ",unkown error" << ",scene_id:" << SceneId()
				<< ",status:" << (int)player->GetScenePlayerInfo()->player_status();
		}
		return true;
	}

	bool MtScene::OnPlayerChangeScene(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerChangeSceneRequest>& message, int32 /*source*/)
	{
		auto scene = MtSceneManager::Instance().GetSceneById(message->scene_id());
		if (scene == nullptr) {
			player->SendClientNotify("InvalidDestScene", -1, -1);
			return true;
		}

		//if (message->scene_id() == player->GetSceneId()) {
		//	player->SendClientNotify("SameScene", -1, -1);
		//	return true;
		//}

		if (player->GetTeamID() != INVALID_GUID && !player->IsTeamLeader() && player->GetTeamStatus() == Packet::PlayerTeamStatus::Team_Follow) {
			player->SendClientNotify("follow_forbind", -1, -1);
			return true;
		}

		if (IsRaidScene(message->scene_id())) {
			//副本是不允许客户端发起切换的
			player->SendClientNotify("InvalidDestScene", -1, -1);
			return true;
		}

		player->GoTo(message->scene_id(), INVALID_POS);
		return true;
	}


	bool MtScene::OnPlayerDisconnect(const boost::shared_ptr<MtPlayer>& player, 
		const boost::shared_ptr<Packet::PlayerDisconnect>& /*msg*/,
		int32 /*source*/)
	{
		//time
		player->SendLogoutLog();
		player->DisConnected();
		return true;
	}

	bool MtScene::OnClientEnterSceneOk(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientEnterSceneOk>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			return true;
		}
		player->ServerLevelCheck(player->PlayerLevel());
		try
		{
			thread_lua->call<void>(4, "OnClientEnterOk", this, player);
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "player:" << player->Guid() << e.what();
		}

		auto raid = player->GetRaid();
		if (raid && raid->Scene() == shared_from_this()) {//保险起见，场景一致性判定
			player->GetRaid()->OnClientEnterOk(player);
		}
		//战斗还原
		if (message->has_login_init() && message->login_init()) {
			auto battle_ground = player->BattleGround();
			if (battle_ground != nullptr) {
				if (battle_ground->BattleType() == Packet::BattleGroundType::PVE_HOOK || battle_ground->BattleType() == Packet::BattleGroundType::PVE_STAGE_BOSS){
					player->QuitBattle();//客户端会单独发挂机请求
				}
				else {
					battle_ground->BattleEnterNotify();
				}
				
			}		
		}

		//进场景快速跟到队长身边，解决客户端加载速度不理想带来的位移偏差问题!
		if (player->GetTeamID() != INVALID_GUID && !player->IsTeamLeader() && player->GetTeamStatus() == Packet::PlayerTeamStatus::Team_Follow) {
			auto leader = FindPlayer(player->GetTeamLeader());
			if (leader != nullptr) {
				Transport(player, leader->Position());
			}			
		}
		return true;
	}

	bool MtScene::OnPlayerLeaveScene(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerLeaveScene>& message, int32 /*source*/)
	{
		message;
		if (player->GetTeamID() != INVALID_GUID && !player->IsTeamLeader() && player->GetTeamStatus() == Packet::Team_Follow) {
			//组队跟随需要由队长处理
			return true;
		}
		PlayerLeaveScene(player);
		return true;
		
	}
	void MtScene::PlayerLeaveScene(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player != nullptr)
		{
			auto raid = player->GetRaid();
			if (raid != nullptr) {
				//场景切换
				if (raid->GetOldSceneId() != SceneId())
				{
					auto& init_pos = raid->GetPortalPos();
					std::vector<int32> pos = { init_pos.x(),init_pos.y(),init_pos.z() };
					player->GoTo(raid->GetOldSceneId(), pos);
				}
			}
			else {
				ZXERO_ASSERT(false) << "can not leave common scene! guid=" << player->Guid();
				return;
			}
		}
	}
	bool MtScene::OnClientMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientPlayerMove>& message, int32 /*source*/)
	{
		auto tplayer = player;
		if (message->guid() != player->Guid()) {//更新队长客户端同步上来的玩家位置，跑火车有用
			if (player->OnRobotMove(message)) {
				return true;
			}
			tplayer = FindPlayer(message->guid());
		}
		if (tplayer == nullptr) {

			return true;
		}
		auto data = tplayer->GetScenePlayerInfo();
		if (data->player_status() == Packet::PlayerSceneStatus::IDEL
			|| data->player_status() == Packet::PlayerSceneStatus::MOVING)
		{
			auto old_pos = tplayer->Position();
			data->set_direction(message->direction());
			tplayer->Position(message->new_pos());
			data->set_player_status(Packet::PlayerSceneStatus::MOVING);
			auto logic_area = GetCurrentLogicArea(tplayer);
			if (!logic_area.Empty()) {
				logic_area.BroadCastMessage(*message, { tplayer->Guid() });
			}
		}
		else { //can't move
			LOG_INFO << "player status:" << data->player_status() << " cannot move";
			Packet::ClientPlayerStop notify;
			notify.set_guid(tplayer->Guid());
			notify.set_direction(tplayer->Direction());
			auto stop_pos = notify.mutable_stop_pos();
			*stop_pos = data->scene_pos();
			player->SendMessage(notify);
		}
		return true;
	}

	void MtScene::OnTeamFollowMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<LogicArea> logic_area, const uint32 time_flag)
	{
		if (player == nullptr)
			return;

		int32 pos_index = 0;

		auto members = player->GetTeamMember(true);
		if (members.size() > 1) {
			auto data = player->GetScenePlayerInfo();
			if (data == nullptr) {
				return;
			}
			auto follow_path = player->GetFollowPath();
			for (auto child : members) {
				if (child == player->Guid()) {
					continue;
				}

				auto tplayer = FindPlayer(child);
				if (tplayer != nullptr && tplayer->GetSceneId() == SceneId() && tplayer->Scene() != nullptr) {

					if (pos_index >= (int32)follow_path.size()) {
						break;
					}

					auto tdata = tplayer->GetScenePlayerInfo();
					if (tdata != nullptr) {

						if (tdata->player_status() == Packet::PlayerSceneStatus::IDEL
							|| tdata->player_status() == Packet::PlayerSceneStatus::MOVING) {
							auto told_pos = tplayer->Position();

							//tdata->set_direction(data->direction());
							tplayer->Position(follow_path[pos_index]);
							tdata->set_player_status(Packet::PlayerSceneStatus::MOVING);

							//直接广播，解决延迟问题
							auto msg = boost::make_shared<Packet::ClientPlayerMove>();
							msg->set_guid(tplayer->Guid());
							msg->set_direction(tdata->direction());
							msg->set_move_speed((int32)(data->move_speed()));
							auto old_pos = msg->mutable_old_pos();
							old_pos->CopyFrom(told_pos);
							auto new_pos = msg->mutable_new_pos();
							new_pos->CopyFrom(follow_path[pos_index]);
							msg->set_follow(true);
							msg->set_move_time(time_flag);
							if (logic_area) {
								logic_area->BroadCastMessage(*msg);
							}
						}
					}

					pos_index++;
				}
			}
		}
	}

	void MtScene::OnTeamFollowStop(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<LogicArea> logic_area)
	{
		if (player == nullptr)
			return;

		int32 pos_index = 0;
		auto members = player->GetTeamMember( true);
		if (members.size() > 1) {
			auto data = player->GetScenePlayerInfo();
			if (data == nullptr) {
				return;
			}
			auto follow_path = player->GetFollowPath();
			for (auto child : members) {
				if (child == player->Guid()) {
					continue;
				}
				auto tplayer = FindPlayer(child);
				if (tplayer != nullptr && tplayer->GetSceneId() == SceneId() && tplayer->Scene() != nullptr) {

					if (pos_index >= (int32)follow_path.size()) {
						break;
					}

					auto tdata = tplayer->GetScenePlayerInfo();
					if (tdata->player_status() == Packet::PlayerSceneStatus::MOVING
						|| tdata->player_status() == Packet::PlayerSceneStatus::IDEL) {

						tdata->set_direction(data->direction());
						tdata->set_player_status(Packet::PlayerSceneStatus::IDEL);
						tplayer->Position(follow_path[pos_index]);

						auto msg = boost::make_shared<Packet::ClientPlayerStop>();
						msg->set_guid(tplayer->Guid());
						msg->set_direction(tdata->direction());
						auto stop_pos = msg->mutable_stop_pos();
						stop_pos->CopyFrom(follow_path[pos_index]);
						msg->set_follow(true);

						if (logic_area) {
							logic_area->BroadCastMessage(*msg);
						}

						//
						tplayer->OnResetFollowPath();
					}

					pos_index++;
				}
			}
		}
	}

	bool MtScene::OnSyncPlayerTeamData(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::SyncPlayerTeamData>& msg, int32 /*source*/)
	{
		player->OnSyncPlayerTeamData(msg);
		return true;
	}
	bool MtScene::OnResetPlayerTeamData(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::ResetPlayerTeamData>& msg, int32 /*source*/)
	{
		msg;
		player->ResetTeamData();
		return true;
	}
	bool MtScene::OnClientStop(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientPlayerStop>& message, int32 /*source*/)
	{
		auto tplayer = player;
		if (message->guid() != player->Guid()) {//更新队长客户端同步上来的玩家位置，跑火车有用
			tplayer = FindPlayer(message->guid());
		}
		if (tplayer == nullptr) {
			return true;
		}
		auto data = tplayer->GetScenePlayerInfo();
		/*LOG_INFO << "session " << player->Session() << " move try stop move status [" << data->player_status() << "]";*/
		if (data->player_status() == Packet::PlayerSceneStatus::MOVING
			|| data->player_status() == Packet::PlayerSceneStatus::IDEL) {

			data->set_player_status(Packet::PlayerSceneStatus::IDEL);
			tplayer->Position(message->stop_pos());
			auto logic_area = GetCurrentLogicArea(player);
			if (!logic_area.Empty()) {

				//if (tplayer->GetTeamID() != INVALID_GUID) {//在队伍里
				//	if (MtTeamManager::Instance().IsLeaderByTeamId(tplayer->GetTeamID(), tplayer->Guid())) {
				//		//只有队长位置需要做同步
				logic_area.BroadCastMessage(*message, { tplayer->Guid() });
				//	}
				//}
				//else {//不在队伍里
				//	logic_area->BroadCastMessage(*message, { tplayer->Guid() });
				//}
			}
		}
		else {
			/*LOG_INFO << "session " << player->Session() << " move stop move status [" << data->player_status() << "]";*/
		}
		return true;
	}


	bool MtScene::UnRegistedMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<google::protobuf::Message>& message, int32 source)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false) << "invalid scene message!";
			return true;
		}
		if (source == MessageSource::GAME_BROADCAST) {
			//未注册的广播消息,直接广播
			BroadcastMessage(*message);
			return true;
		}

		LOG_WARN << "message unregisted" << message->GetTypeName()
			<< "," << source;
		if (player->Session() != nullptr) {
			LOG_WARN << "unregisted message received "
				<< player->Session()->get_state() << "," << message->GetTypeName()
				<< "," << source;
			try
			{
				std::string func_name = "xOn" + message->GetDescriptor()->name();
				if (lua_call_.Registed(func_name)) {
					lua_call_.CallMsgHandler(player.get(), func_name, message.get());
				} else {
					Server::Instance().SendMessage(message, player);
				}
			}
			catch (const ff::lua_exception& e) 
			{
				LOG_ERROR << "handle message:" 
					<< message->GetDescriptor()->name() << "error:" << e.what();
			}
		}
		return false;
	}

	bool MtScene::UnRegistedInnerMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<google::protobuf::Message>& message, int32 /*source*/)
	{
		player;
		ZXERO_ASSERT(false) << message->GetTypeName();
		return false;
	}

	bool MtScene::OnActorList(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorList>& /*message*/, int32 /*source*/)
	{
		player->Refresh2ClientActorList();
		return true;
	}

	bool MtScene::OnChatMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChatMessage>& message, int32 source)
	{
		if (source == MessageSource::CLIENT) {
			//来自客户端的消息. 需要逻辑处理
			scene_chat_message(shared_from_this(), player, message);
		} else if (source == MessageSource::GAME_BROADCAST) {
			//广播消息
			BroadcastMessage(*message);
		}
		return true;
	}

	bool MtScene::OnLootPlayer(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::LootPlayer>& msg, int32 /*source*/)
	{
		auto battle = player->BattleGround();
		auto target_it = player_map_.find(msg->target_guid());
		if (target_it == std::end(player_map_)) {
			player->SendCommonResult(Packet::ResultOption::Player_Loot, Packet::ResultCode::Target_In_Protect);
			return true;
		}
		auto target = target_it->second;
		const int32 max_loot_per_day = 5;
		if (player->DataCellContainer()->get_data_64(Packet::Cell64LogicType::LootCell64Type, Packet::LootCellIndex::LOOT_COUNT_TODAY) >= max_loot_per_day)
		{
			player->SendCommonResult(Packet::ResultOption::Player_Loot, Packet::ResultCode::Self_Max_Loot_Count);
			return true;
		}
		if (target->InLootProtect()) {
			player->SendCommonResult(Packet::ResultOption::Player_Loot, Packet::ResultCode::Target_In_Protect);
			return true;
		}

		GetBattleManager().CreateFrontBattle(player.get(), target.get(),
		{
			{ "script_id", 1009 },
			{ "group_id",msg->battle_group_id() }
		});
		
		return true;
	}

	zxero::int32 MtScene::SceneId() const
	{
		return config_->id();
	}

	bool MtScene::OnPlayerReachBattlePos(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerReachBattlePos>& message, int32 /*source*/)
	{
		auto battle_ground = player->BattleGround().get();
		if (battle_ground == nullptr) {
			battle_ground = player->HookBattle();
		}
		if (battle_ground == nullptr || battle_ground->Guid() != message->battle_ground_guid()) {
			//player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::Invalid_Request);
			return true;
		}
		else {
			auto& attackers = battle_ground->GetAttackers();
			if (int32(attackers.size()) != message->guids_size() ||
				message->guids_size() != message->positions_size()) {
				if (!player->IsDisConnected()) {
					//player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::Invalid_Request);
					return true;
				}
			}
			const int32 ten_meter = 10 * 1000;
			if (message->guids_size() > 0) {
				for (auto i = 0; i < (int32)attackers.size(); ++i) {
					auto& attcker = attackers[i];
					if (attcker->Guid() != message->guids(i)
						|| zxero::distance2d(attcker->Position(), message->positions(i)) > ten_meter) {
						//player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::Invalid_Request);
						return true;
					}
					attcker->Position(message->positions(i));
				}
			}
			battle_ground->OnPlayerReachPos();
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::ReachPos);
		}
		return true;
	}



	bool MtScene::OnPlayerContainerRequet(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerContainerRequest>& message, int32 /*source*/)
	{
		player->UpdateContainer(message->container_type());
		return true;
	}

	bool MtScene::OnActorAddEquipRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorAddEquipRequest>& message, int32 /*source*/)
	{
		auto actors = player->Actors();
		auto actor_it = std::find_if(std::begin(actors), std::end(actors), boost::bind(&MtActor::Guid, _1) == message->actor_guid());
		if (actor_it == std::end(actors)){
			player->SendCommonResult(Packet::ResultOption::Actor_Opt,Packet::ResultCode::NoActor);
			return true;
		}
		//if (player->BagLeftCapacity(Packet::ContainerType::Layer_EquipBag, 1) == false) {
		//	player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::BagIsFull);
		//	return true;
		//}
		auto actor = *actor_it;
		//检查数据
		for (auto i = 0; i < message->equip_guid_size(); ++i) {
			auto equip_guid = message->equip_guid(i);
			auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag ,equip_guid);
			if (item == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidEquip);
				return true;;
			}
			if (message->slot(i) == Config::EquipmentSlot::EQUIPMENT_SLOT_INVALID || Config::EquipmentSlot_IsValid(message->slot(i)) == false) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidEquipType);
				return true;;
			}
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
			if (equip == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidEquip);
				return true;;
			}
/*
			if ((equip->GetBindActorGuid() != INVALID_GUID && equip->GetBindActorGuid() != actor->Guid())) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::Bind_Actor);
				return true;;
			}*/
			auto equip_config = equip->Config();
			if (equip_config->level_limit() > 0) {
				if (actor->Level() < equip_config->level_limit()) {
					player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::LevelLimit);
					return true;;
				}
			}
			if (equip_config->race_limit().size() > 0 &&
				equip_config->race_limit().Get(0) != 0 &&
				std::find(std::begin(equip_config->race_limit()), std::end(equip_config->race_limit()), actor->DbInfo()->race()) == std::end(equip_config->race_limit())) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidRace);
				return true;;
			}
			if (equip_config->prof_limit().size() > 0 &&
				equip_config->prof_limit().Get(0) != 0 &&
				std::find(std::begin(equip_config->prof_limit()), std::end(equip_config->prof_limit()), actor->DbInfo()->profession()) == std::end(equip_config->prof_limit())) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidProfession);
				return true;;
			}
			if (equip_config->talent_limit().size() > 0 &&
				equip_config->talent_limit().Get(0) != 0 &&
				std::find(std::begin(equip_config->talent_limit()), std::end(equip_config->talent_limit()), actor->DbInfo()->talent()) == std::end(equip_config->talent_limit())) {
				player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidTalent);
				return true;;
			}
			auto weapon_type = equip->Config()->weapon_type();
			if (weapon_type > 0) {//角色武器类型支持
				auto slot = message->slot(i);
				auto actor_config = MtActorConfig::Instance().GetBaseConfig(actor->ConfigId());
				ZXERO_ASSERT(slot == Config::EquipmentSlot::EQUIPMENT_SLOT_ASSI_HAND || slot == Config::EquipmentSlot::EQUIPMENT_SLOT_MAIN_HAND);
				ZXERO_ASSERT(actor_config != nullptr);
				switch (slot)
				{
				case Config::EQUIPMENT_SLOT_MAIN_HAND:
					if (std::find(std::begin(actor_config->main_hand_weapons), std::end(actor_config->main_hand_weapons), weapon_type) == std::end(actor_config->main_hand_weapons)) {
						player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidProfession);
						return true;;
					}
					break;
				case Config::EQUIPMENT_SLOT_ASSI_HAND:
					if (std::find(std::begin(actor_config->assi_hand_weapons), std::end(actor_config->assi_hand_weapons), weapon_type) == std::end(actor_config->assi_hand_weapons)) {
						player->SendCommonResult(Packet::ResultOption::Wear_Equip, Packet::ResultCode::InvalidProfession);
						return true;;
					}
					break;
				default:
					ZXERO_ASSERT(false) << "invalid slot type";
					return true;;
				}
			}
		}
		for (auto i = 0; i < message->equip_guid_size(); ++i) {
			auto equip_guid = message->equip_guid(i);
			auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag ,equip_guid);
			if (item) {
				auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
				if (equip) {
					actor->AddEquipment(equip.get(), message->slot(i));
				}
			}
		}
		return true;
	}

	bool MtScene::OnActorDelEquipRequest(
		const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::ActorDelEquipRequest>& message,
		int32 /*source*/)
	{
		auto actors = player->Actors();

		auto item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip, message->equip_guid());
		if (item == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Item_Common, Packet::ResultCode::InvalidEquip);
			return true;
		}
		auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		if (equip->ActorGuid() == INVALID_GUID) {
			player->SendCommonResult(Packet::ResultOption::Item_Common, Packet::ResultCode::EquipNotOnBody);
			return true;
		}
		auto actor_it = std::find_if(std::begin(actors), std::end(actors), boost::bind(&MtActor::Guid, _1) == equip->ActorGuid());
		if (actor_it == std::end(actors)){
			player->SendCommonResult(Packet::ResultOption::Item_Common, Packet::ResultCode::NoActor);
			return true;
		}
		auto equip_bag = player->FindContainer(Packet::ContainerType::Layer_EquipBag);
		if (equip_bag == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Item_Common, Packet::ResultCode::UnknownError);
			return true;
		}
		if (equip_bag->AddItem(item) == false) {
			player->SendClientNotify("ItemBagIsFull", -1, -1);
			return true;
		} else {
			(*actor_it)->UnEquipment(equip.get());
		}
		return true;
	}

	bool MtScene::OnFixEquipReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::FixEquipReq>& message, int32 /*source*/)
	{
		std::vector<boost::shared_ptr<MtEquipItem>> equips;

		for (auto item_guid : message->equip_guid()) {
			auto item = player->FindItemByGuid(Packet::ContainerType::Layer_EquipBag, item_guid);
			if (item != nullptr) {
				auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
				if (equip) {
					equips.push_back(equip);
				}
				continue;
			}

			item = player->FindItemByGuid(Packet::ContainerType::Layer_Equip ,item_guid);
			if (item != nullptr) {
				auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
				if (equip) {
					equips.push_back(equip);
				}
			}
		}

		if (equips.size() < 1) {
			player->SendCommonResult(Packet::ResultOption::Equip_Refix, Packet::ResultCode::InvalidEquip);
			return true;
		}

		std::set<uint64> actors;
		int32 cost = 0;
		for (auto child : equips) {
			cost = MtUpgradeConfigManager::Instance().EquipFixCost(*child);
			if (!player->DelItemById(Packet::TokenType::Token_Gold, cost, Config::ItemDelLogType::DelType_Refix, player->Guid())) {
				player->SendCommonResult(Packet::ResultOption::Equip_Refix, Packet::ResultCode::GoldNotEnough);
				return true;
			}
			child->DbInfo()->set_durable(float(child->Config()->init_durable()));
			child->RefreshData2Client();

			if (child->ActorGuid() != INVALID_GUID) {
				actors.insert(child->ActorGuid());
			}
		}

		for (auto child : actors) {
			auto actor = player->FindActorByGuid(child);
			if (actor != nullptr) {
				actor->OnBattleInfoChanged();
			}
		}

		player->EquipDurableCheck();

		player->SendCommonResult(Packet::ResultOption::Equip_Refix, Packet::ResultCode::ResultOK);
		return true;
	}

	bool MtScene::OnConstantChallengeBoss(
		const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::ConstantChallengeBoss>& message, int32 /*source*/)
	{
		message;
		player;
		return true;
		//auto battle_ground = player->BattleGround();
		//auto player_stage = player->GetPlayerStage(message->stage_id());
		//if (player_stage == nullptr) {
		//	player->SendClientNotify("InvalidConfigData",-1,-1);
		//	return true;
		//}
		//const Packet::PlayerStage& stage = *player_stage;
		//if (battle_ground == nullptr) {
		//	player->SendClientNotify("BattleOver", -1, -1);
		//	return true;
		//}
		//if( battle_ground->BattleType() != Packet::BattleGroundType::PVE_STAGE_BOSS	|| battle_ground->GetStageId() != message->stage_id()) {
		//	player->SendClientNotify("Invalid_Request", -1, -1);
		//	return true;;
		//}
		//if (message->status()) {//请求持续挑战boss
		//	auto curr_challenge_counter = player->ChallengeBossCounter();
		//	player->SetChallengeBossCounter(curr_challenge_counter + stage.boss());
		//	player->SendMessage(*message);//设置成功
		//	return true;
		//} else {
		//	player->SetChallengeBossCounter(0);
		//	player->SendMessage(*message);//设置成功
		//	return true;;
		//}
	}

	bool MtScene::OnChallengeBoss(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::ChallengeBossReq>& message,
		int32 /*source*/)
	{
		if (!player->PlayerCanHook(message->scene_stage_id())) {
			player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::LevelLimit);
			return true;
		}

		auto battle_grounds = player->BattleGround();
		auto config = MtSceneStageManager::Instance().GetConfig(message->scene_stage_id());
		if (config == nullptr) {
			player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::InvalidConfigData);
			return true;
		}

		auto player_stage = player->GetPlayerStageProxy()->GetPlayerStage(message->scene_stage_id());
		if (player_stage == nullptr) {
			player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::Invalid_Request);
			return true;
		}
		if (player_stage->boss() < message->rush_count()
			|| player_stage->boss() < 1) {
			player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::ChallengeTimeLimit);
			return true;
		}
		if (message->rush()) {
			//春晓说,扫荡最多掉一个装备, 2种材料
			if (!player->BagLeftCapacity(Packet::ContainerType::Layer_EquipBag, message->rush_count())) {
				player->SendClientNotify("EquipBagIsFull", -1, -1);
				return true;
			}
			if (!player->BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, 2)) {
				player->SendClientNotify("ItemBagIsFull", -1, -1);
				return true;
			}

			//扫荡, 不用管战场了
			if (player_stage->boss() > 0) {
				auto rush_ticket_config = MtConfigDataManager::Instance().FindConfigValue("boss_rush_ticket");
				if (rush_ticket_config == nullptr) {
					player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::InvalidConfigData);
					return true;
				}

				auto scene_stage_config = MtSceneStageManager::Instance().GetConfig(message->scene_stage_id());
				if (scene_stage_config == nullptr) {
					player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::InvalidConfigData);
					return true;
				}

				auto monster_group_id = scene_stage_config->bosses(rand_gen->intgen(0, scene_stage_config->bosses_size() - 1));
				auto monster_group_config = MtMonsterManager::Instance().FindMonsterGroup(monster_group_id);
				if (monster_group_config == nullptr) {
					player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::InvalidConfigData);
					return true;
				}

				if (!player->DelItemById(rush_ticket_config->value1(), message->rush_count(), Config::ItemDelLogType::DelType_ChallengeBoss)) {
					player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::CostItemNotEnough);
					return true;
				}
				player->GetPlayerStageProxy()->DecBossChallengeCount(player_stage->id(), message->rush_count());
				Packet::RushRewardNotify notify;
				for (auto i = 0; i < message->rush_count(); ++i) {
					Packet::NotifyItemList notify_list;
					auto packages = MtMonsterManager::Instance().MonsterGroupRandomDrop(monster_group_id);
					for (auto& package : packages) {
						package->AddToPlayer(player.get(), nullptr, &notify_list);
					}
					if (notify_list.IsInitialized()) {
						notify.add_list()->CopyFrom(notify_list);
					}
				}
				
				player->OnLuaFunCall_x("xOnMonsterDead",
				{
					{ "monster_id",-1 },
					{ "stage_id",player_stage->id() },
					{ "scene_id",SceneId() },
					{ "monster_class",-1 },
					{ "battle_type",Packet::BattleGroundType::PVE_STAGE_BOSS },
					{ "monster_count" , message->rush_count() },
				});

				player->SendMessage(notify);
			}
		}
		else {
			if (player->GetTeamID() != INVALID_GUID) {
				player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::SingleLimit);
				return true;
			}
			if (SceneId() == config->scene_id()) {
				if (player_stage->boss() > 0) {
					auto battle = battle_manager_.CreateFrontBattle(player.get(),nullptr,
					{
						{ "script_id", 1008 },
						{ "group_id", message->battle_group_id()},
						{ "stage_id",message->scene_stage_id()}
					});

				}
				else {
					player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::CountLimit);
					return true;
				}
			}
			else {//挑战场景不是当前场景需要先切换场景

				auto target_scene = MtSceneManager::Instance().GetSceneById(config->scene_id());
				if (target_scene == nullptr) {
					player->SendCommonResult(Packet::ResultOption::ChallengeBoss, Packet::ResultCode::Wrong_Scene);
					return true;
				}

				if (player->GoTo(target_scene->SceneId(), INVALID_POS)) {

					player->DispatchMessage(message);//投递挂机战斗消息
				}				
			}
		}
		return true;
	}

	boost::shared_ptr<MtPlayer> MtScene::FindPlayerBySceneServer(uint64 guid)
	{
		auto player_ = FindPlayer(guid);
		if (player_ == nullptr)
		{
			player_ = Server::Instance().FindPlayer(guid);
		}
		return player_;
	}

	bool MtScene::OnPlayerOperateInfoReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerOperateInfoReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		uint64 targetguid = message->guid();
		auto targetplayer = FindPlayerBySceneServer(targetguid);
		if (targetplayer != nullptr)
		{
			Packet::PlayerOperateInfoReply reply;
			targetplayer->FillPlayerOperateInfo(*reply.mutable_optinfo());
			player->SendMessage(reply);
		}
		else
		{
			boost::shared_ptr<S2G::GetOperateInfo> getmsg = boost::make_shared<S2G::GetOperateInfo>();
			if (getmsg)
			{
				getmsg->set_playerid(targetguid);
				Server::Instance().SendMessage(getmsg, player);
			}
		}
		return true;
	}

	bool MtScene::OnWorldBossInfoReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossInfoReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		player->OnSendWorldBossBattleReply();
		return true;
	}

	bool MtScene::OnWorldBossHpReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossHpReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		player->OnSendWorldBossHpUpdate();
		MtWorldBossManager::Instance().OnWorldBossStatisticsReq(player);
		return true;
	}
	bool MtScene::OnLeaveWorldBossReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::LeaveWorldBossReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;

		player->GoTo(20, INVALID_POS);
		return true;
	}
	bool MtScene::OnWorldBossRelivenow(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossRelivenow>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		auto ret = player->OnWorldBossRelive();
		player->SendCommonResult(Packet::ResultOption::WolrdBoss_Opt, ret);
		return true;
	}
	bool MtScene::OnWorldBossGoldInspire(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossGoldInspire>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		auto ret = player->OnInspire(1);
		player->SendCommonResult(Packet::ResultOption::WolrdBoss_Opt, ret);
		return true;
	}
	bool MtScene::OnWorldBossDiamondInspire(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossDiamondInspire>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		auto ret = player->OnInspire(0);
		player->SendCommonResult(Packet::ResultOption::WolrdBoss_Opt, ret);
		return true;
	}
	bool MtScene::OnWorldBossStatisticsReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::WorldBossStatisticsReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		message;
		MtWorldBossManager::Instance().OnWorldBossStatisticsReq(player);
		return true;
	}
	bool MtScene::OnSetWorldBossAutoFightReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetWorldBossAutoFightReq>& message, int32 /*source*/)
	{
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return true;
		}
		auto autofight = message->autofight();
		player->DataCellContainer()->set_bit_data(Packet::BitFlagCellIndex::WorldBossAutoFightFlag, autofight);
		if (autofight == false)
		{
			player->StopMove();
		}
		return true;
	}
	

	boost::shared_ptr<MtNpc> MtScene::FindNpc(const int32 series_id)
	{
		if (npc_map_.find(series_id) != npc_map_.end()) {
			return npc_map_[series_id];
		}
		return nullptr;
	}
	//

	int32 MtScene::AddNpc(const int32 pos_id, const int32 index, const int32 lifetime)
	{
		auto npc = MtMonsterManager::Instance().CreateNpc(pos_id, index,-1, lifetime);
		if (!npc) {
			ZXERO_ASSERT(false) << "can not find monster data ! index=" << index << " scene_id" << config_->id() << " pos_id" << pos_id;
			return -1;
		}

		auto info = npc->NpcInfo();
		int32 series_id = info->series_id();
		auto iter = npc_map_.find(series_id);
		if (iter != npc_map_.end())
			return -1;

		auto npc_data = MtMonsterManager::Instance().FindDynamicNpc(index);
		if (!npc_data) {
			ZXERO_ASSERT(false) << "FindNpcInGroup failed ! index=" << index << " scene_id" << config_->id() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		int32 npc_id = npc_data->npc_id();
		auto monster_config = MtMonsterManager::Instance().FindMonster(npc_id);
		if (!monster_config) {
			ZXERO_ASSERT(false) << "FindMonster failed ! npc_id=" << npc_id << " scene_id" << config_->id() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}
		auto pos_config = MtMonsterManager::Instance().FindScenePos(pos_id);
		if (!pos_config) {
			ZXERO_ASSERT(false) << "FindScenePos failed ! npc_id=" << npc_id << " scene_id" << config_->id() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		if (config_->id() != pos_config->scene_id()) {
			ZXERO_ASSERT(false) << "npc owner scene id do not match with current scene ! npc_id=" << npc_id << " scene_id" << config_->id() << " pos_id" << pos_id << "series_id " << series_id;
			return -1;
		}

		npc_map_.insert({ series_id, npc });

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcAdd);
		auto npc_info = reply.add_npc_info();
		npc_info->CopyFrom(*info);
		BroadcastMessage(reply);

		//LOG_INFO << "add scene npc ok ! index=" << index << " pos_id" << pos_id << " scene_id" << SceneId() << " series_id=" << series_id;

		return series_id;
	}

	void MtScene::LockNpc(const int32 series_id, const bool lock)
	{
		auto iter = npc_map_.find(series_id);
		if (iter != npc_map_.end())
		{
			iter->second->Lock(lock);
		}
	}

	void MtScene::DelNpc(const int32 series_id)
	{
		auto iter = npc_map_.find(series_id);
		if (iter == npc_map_.end())
			return;

		auto npc = iter->second;
		if (npc == nullptr) {
			ZXERO_ASSERT(false) << "can not find npc data !  scene_id" << config_->id() << "series_id " << series_id;
			return;
		}

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);
		auto npc_info = reply.add_npc_info();
		npc_info->CopyFrom(*(npc->NpcInfo()));

		npc_map_.erase(iter);

		BroadcastMessage(reply);

		//LOG_INFO << "del scene npc ok ! scene_id" << SceneId() << " series_id=" << series_id;

	}

	void MtScene::DelNpcByType(const std::string src_type)
	{
		if (npc_map_.empty()) {
			return;
		}

		Packet::NpcList reply;
		reply.set_option(Packet::NpcOption::NpcDel);

		auto iter = npc_map_.begin();
		for (; iter != npc_map_.end(); ) {
			if (iter->second->NpcDynamicConfig()->npc_tag() == src_type) {

				auto npc_info = reply.add_npc_info();
				npc_info->CopyFrom(*(iter->second->NpcInfo()));
				iter = npc_map_.erase(iter);
			}
			else {
				++iter;
			}
		}

		BroadcastMessage(reply);

		//LOG_INFO << "del scene npc ok ! scene_id" << SceneId() << " src_type =" << src_type;
	}

	int32 MtScene::AddRaid(const RaidHelper &helper)
	{
		std::unique_lock<std::mutex> lock(raid_mutex_);

		raid_count_++;
				
		int32 runtime_id = (((SceneId() << 16) & 0xffff0000) | (raid_count_ & 0x0000ffff)); 
		auto raid = boost::make_shared<MtRaid>(helper, runtime_id);
		raids_.insert({ runtime_id, raid });

		return runtime_id;
	}
	
	boost::shared_ptr<MtRaid> MtScene::GetRaid(int32 rt_id)
	{
		std::unique_lock<std::mutex> lock(raid_mutex_);

		if (raids_.find(rt_id) == raids_.end()) {
			return nullptr;
		}

		return raids_[rt_id];
	}

	void MtScene::DelRaid(const int32 rt_id)
	{
		std::unique_lock<std::mutex> lock(raid_mutex_);
		if (raids_.find(rt_id) == raids_.end()) {
			return;
		}

		raids_.erase(rt_id);
	}

	//玩家npc对话
	bool MtScene::OnPlayerNpcSessionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerNpcSession>& message, int32 /*source*/)
	{
		auto trial_info = player->GetTrialInfo();
		if (trial_info == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Hell_Opt, Packet::ResultCode::Target_Dismatch);
			return true;
		}

		if (player->IsEmptyFormation(Packet::ActorFigthFormation::AFF_PVP_TRIAL_FIELD)) {
			player->SendCommonResult(Packet::ResultOption::Hell_Opt, Packet::ResultCode::NoActor);
			return true;
		}
		auto target = player->GetPlayerNpc(message->target_guid());
		if (target == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Hell_Opt, Packet::ResultCode::Target_Dismatch);
			return true;
		}

		LOG_INFO << "OnPlayerNpcSessionReq ! target guid" << target->mutable_player()->guid() << " passed=" << target->passed() << " reward=" << target->rewarded() << " player guid=" << player->Guid();
		
		if (target->passed()) {
			player->SendCommonResult(Packet::ResultOption::Hell_Opt, Packet::ResultCode::Target_Challenge_Done);
			return true;
		}

		auto battle = GetBattleManager().CreateFrontBattle(player.get(),nullptr,
		{
			{ "script_id", 1013 },
			{ "group_id", 1 }
		});

		return true;
	}
	//npc对话
	bool MtScene::OnNpcSessionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::NpcSession>& message, int32 /*source*/)
	{
		int32 pos_id = -1;
		if (message->has_series_id() && message->series_id() >= 0) {//动态npc

			auto npc = FindNpc(message->series_id());
			if (npc == nullptr) {
				npc = player->FindNpc(message->series_id());
				if (npc == nullptr) {
					ZXERO_ASSERT(false) << "can not find npc, series_id=" << message->series_id() << " scene =" << config_->id() << " player guid=" << player->Guid();
					player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode::NpcNotExist);
					return true;
				}
			}
			if (npc->IsLock()) {
				player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode::NpcLock);
				return true;
			}

			pos_id = npc->PositionConfig()->id();
			//位置校验
			if (!player->ValidPositionById(pos_id)) {
				player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode::InvalidNpcPosition);
				return true;
			}

			if (message->has_mission_id() && message->mission_id() >= 0) {
				//走任务逻辑
			}
			else {
				auto npc_config = npc->NpcDynamicConfig();
				if (npc_config != nullptr) {
					try
					{
						thread_lua->call<int32>(3002, "OnNpcOption", player, npc, message->option_index());
					}
					catch (ff::lua_exception &)
					{
					}
				}
				return true;
			}
		}
		else {//静态npc
			auto npc_config = MtMonsterManager::Instance().FindStaticNpc(message->index());
			if (npc_config != nullptr) {
				pos_id = npc_config->pos_id();
				//位置校验
				if (!player->ValidPositionById(pos_id)) {
					player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode::InvalidNpcPosition);
					return true;
				}

				if (message->has_mission_id() && message->mission_id() >= 0) {
					//走任务逻辑
				}
				else {
					try
					{
						int32 lua_ret = thread_lua->call<int32>(3001, "OnNpcOption", player, npc_config, message->option_index());
						if (lua_ret != 0) {
							player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode(lua_ret));
							return true;
						}
					}
					catch (ff::lua_exception& e)
					{
						LOG_ERROR << "player:" << player->Guid() << ",npc_config:" << npc_config
							<< ",index:" << message->option_index() << e.what();
					}					

					player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode::ResultOK);
					return true;
				}
			}
		}

		//任务逻辑
		if (message->has_mission_id() && message->mission_id() >= 0) {
			auto proxy = player->GetMissionProxy();
			if (proxy != nullptr) {
				if (message->option() == Packet::MissionOption::Accept) {
					if (!player->BagLeftCapacity(Packet::ContainerType::Layer_MissionBag,1)) {
						player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::BagIsFull);
						return true;
					}
					proxy->OnAcceptMission((Config::MissionType)message->mission_id());
				}
				else if (message->option() == Packet::MissionOption::Add) {
					proxy->OnAddMission(message->mission_id());
				}
				else if (message->option() == Packet::MissionOption::Finish) {
					if (!player->BagLeftCapacity(Packet::ContainerType::Layer_AllBag, 1)) {
						player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::BagIsFull);
						return true;
					}
					proxy->OnFinishMission(message->mission_id());
				}
				else if (message->option() == Packet::MissionOption::Drop) {
					proxy->OnDropMission(message->mission_id());
				}
				else if (message->option() == Packet::MissionOption::Fight) {
					if (!player->BagLeftCapacity(Packet::ContainerType::Layer_AllBag, 1)) {
						player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::BagIsFull);
						return true;
					}
					proxy->OnMissionFight(message->mission_id());
				}
				else if (message->option() == Packet::MissionOption::Raid) {
					player->OnMissionRaid( message->mission_id());

				}
				else {
					player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMission);
					return true;
				}
			}

			player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::ResultOK);
			return true;
		}

		player->SendCommonResult(Packet::ResultOption::Npc_Option, Packet::ResultCode::InvalidNpcOption);
		return true;
	}

	bool MtScene::OnNoNpcSessionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::NoNpcSession>& message, int32 /*source*/)
	{
		try
		{
			int32 lua_ret = thread_lua->call<int32>(3003, "OnNpcOption", player, message->string_param(), message->long_param(), message->int_param());
			if (lua_ret != 0) {
				player->SendCommonResult(Packet::ResultOption::Npc_Option, (Packet::ResultCode)lua_ret);
			}
		}
		catch (ff::lua_exception& e)
		{
			LOG_ERROR << "player:" << player->Guid() << "," << message->string_param() << ","
				<< message->long_param() << "," << message->int_param() << e.what();
		}

		return true;
	}


	bool MtScene::OnSyncMissionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SyncMissionRequest>& message, int32 /*source*/)
	{
		auto src_player = FindPlayer(message->snc_msg().src_guid());
		if (src_player == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::MemenberTempLeave);
			return true;
		}

		auto proxy = src_player->GetMissionProxy();
		if (proxy == nullptr) {
			LOG_ERROR << "OnSyncMissionReq failed, player guid=" << player->Guid();
			player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::InvalidMissionData);
			return true;
		}
		if (message->agree()) {
			int32 ret = proxy->OnSyncMissionAgree(message->snc_msg().time_mark(), player->Guid());
			if (ret == -1) {
				//说明超时了
				//player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::TimeOut);
				return true;
			}
			else if (ret == 1) {
				auto scene = src_player->Scene();
				if (scene != nullptr) {
					scene->QueueInLoop(boost::bind(&MtMissionProxy::OnMissionSync, proxy, message->snc_msg().type()));
				}
			}
			else {
				//等待
			}
		}
		else {
			//广播给其他人
			std::string content = "mission_disagree|" + player->Name();
			Packet::Notify notify;
			notify.set_notify(content);
			player->BroadcastTeamEx(true, notify);

			proxy->OnSyncMissionDisagree(message->snc_msg().time_mark(), player->Guid());
		}
		return true;
	}
	bool MtScene::OnClickChatMissionReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClickChatMissionReq>& message, int32 /*source*/)
	{
		if (player == nullptr)
		{
			ZXERO_ASSERT(false);
			return true;
		}
		auto src_player = FindPlayerBySceneServer(message->srcplayer_guid());
		if (src_player == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::MemenberTempLeave);
			return true;
		}
		auto mission = src_player->GetMissionById(message->missionid());
		if (mission == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Mission_Opt, Packet::ResultCode::MISSION_NOT_EXIT);
			return true;
		}
		Packet::ClickChatMissionReply reply;
		reply.set_missionid(mission->Config()->id());
		reply.set_t_desc(message->t_desc());
		reply.set_s_desc(message->s_desc());
		reply.set_d_desc(message->d_desc());
		player->SendMessage(reply);
		return true;
	}
	bool MtScene::ExitBattleGround(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ExitBattleGround>& message, int32 /*source*/)
	{
		message;
		auto hook_battle = player->HookBattle();
		if (hook_battle != nullptr) {//退出挂机战斗时, 战斗不停止, 只是客户端不再收到战斗数据
			LOG_INFO << "[exit_battle] player:" << player->Guid() << " exit hook battle";
			Packet::CommonReply reply;
			reply.set_reply_name("exit_battle_ground_reply");
			if (hook_battle->GetClientInsight()){
				player->HookBattle()->BattleOverNotify(0);
				hook_battle->SetClientInsight(false);
				player->SetOnHook(false);

				reply.add_int32_params(1);
				player->SendMessage(reply);
				
				player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::HookEndOK);
				return true;
			} 
		}
		auto battle = player->BattleGround();
		if (battle != nullptr) {
			if (player->BattleGround()->BattleType() == Packet::BattleGroundType::PVP_GUILD_BATTLE)
			{
				player->SendClientNotify("TeamNoEndBattle", -1, -1);
				return true;
			}
			auto teamId = player->GetTeamID();
			if (teamId != INVALID_GUID && player->GetTeamStatus() == Packet::Team_Follow ) {//有队伍
				if (!player->IsTeamLeader() ) {
					player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::Team_NotLeader);
					return true;
				}
				auto members = player->GetTeamMember(true);
				for (auto child : members) {
					auto tplayer = FindPlayer(child);
					if (tplayer != nullptr) {
						Packet::CommonReply reply;
						auto bg = tplayer->BattleGround();
						if (bg != nullptr) {
							tplayer->QuitBattle();
							reply.add_int32_params(1);
						} else {
							reply.add_int32_params(0);
						}
						reply.set_reply_name("exit_battle_ground_reply");
						tplayer->SendMessage(reply);
					}
				}
			} else {
				Packet::CommonReply reply;
				player->QuitBattle();
				reply.add_int32_params(1);
				reply.set_reply_name("exit_battle_ground_reply");
				player->SendMessage(reply);
				return true;
			}
		}
		Packet::CommonReply reply;
		reply.add_int32_params(0);

		return true;
	}


	bool MtScene::OnPlayerStartHook(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerStartHook>& message, int32 /*source*/)
	{
		LOG_INFO << "[hook] player:" << player->Guid() << " start hook stage_id:" << message->stage_id();
		if (player->GetTeamID() != INVALID_GUID) {
			player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::SingleLimit);
			return true;
		}
		if (!player->PlayerCanHook(message->stage_id())) {
			player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::LevelLimit);
			return true;
		}
		if (player->GetPlayerStageProxy()->CanRaidStage(message->stage_id()) == false) {
			player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::InvalidSceneStage);
			return true;
		}
		////
		auto stage_config = MtSceneStageManager::Instance().GetConfig(message->stage_id());
		if (stage_config == nullptr || stage_config->normal_monsters_size() == 0
			|| stage_config->group()== 0) {
			player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::InvalidConfigData);
			return true;
		}

		if (stage_config->scene_id() == SceneId()) {
			if (player->GetPlayerStageDB()->current_stage() != message->stage_id()) {
				player->GetPlayerStageProxy()->ResetTo(message->stage_id());
			}
			auto ret = player->StartHookBattle(message->stage_id(), message->battle_group_id(), 1, 1);
			if (!ret) {
				return true;
			}
		}
		else {
			auto target_scene = MtSceneManager::Instance().GetSceneById(stage_config->scene_id());
			if (target_scene == nullptr) {
				player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::Wrong_Scene);
				return true;
			}

			if (player->GoTo(stage_config->scene_id(), INVALID_POS)) {
				//结束当前战斗
				player->QuitBattle();
				player->DispatchMessage(message);//投递挂机战斗消息			
			}
			
		}
		return true;
	}

	bool MtScene::OnPlayerEndHook(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::PlayerEndHook>& message, int32 /*source*/)
	{
		player; message;
		return true;
	}

	bool MtScene::OnChangeHookInfo(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChangeHookInfo>& message, int32 /*source*/)
	{
		player->SetOnHookOption(message->hook_option());
		return true;
	}

	bool MtScene::OnUpdateFollowState(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::UpdateFollowState>& message, int32 /*source*/)
	{
		auto bg = player->BattleGround();
		if (bg == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
			return true;
		}
		auto& attackers = bg->GetAttackers();
		auto first_actor = attackers[0];
		if (first_actor->Player().lock() != nullptr && first_actor->Player().lock()->Guid() == player->Guid()) {
			std::for_each(std::begin(attackers), std::end(attackers), [=](auto& actor) {
				if (!actor->MainActor()) {
					actor->BattleFollowing(message->follow());
				}
			});
		}
		else {
			auto defenders = bg->GetDefenders();
			first_actor = defenders[0];
			if (first_actor->Player().lock() != nullptr && first_actor->Player().lock()->Guid() == player->Guid()) {
				std::for_each(std::begin(attackers), std::end(attackers), [=](auto& actor) {
					if (!actor->MainActor()) {
						actor->BattleFollowing(message->follow());
					}
				});
			}
		};
		player->SendMessage(*message);
		return true;
	}

	bool MtScene::OnBattleGroundSetControlType(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetControlType>& message, int32 /*source*/)
	{
		auto bg = player->BattleGround();
		if (bg == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
			return true;
		}

		bool success = false;
		bg->EnumAllActor([&](const boost::shared_ptr<MtActor>& actor) {
			if (actor->Player().lock() != nullptr && actor->Player().lock()->Guid() == player->Guid()
				&& actor->MainActor()) {
				actor->SetConrolType(message->contral_type());
				success = true;
			}
		});

		if (success) {
			player->SendMessage(*message);
		}
		else {
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::SETTING_UNACCEPTABLE);
		}
		return true;
	}

	bool MtScene::OnBattleGroundMoveFinish(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorMoveToTargetFinish>& msg, int32 /*source*/)
	{
		player, msg;
		/*auto bg = player->BattleGround();
		if (bg == nullptr) {
			/ *LOG_INFO << "got move msg after battle over. player:" << player->Guid()
				<< " scene:" << SceneId();* /
			return true; / *player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);* /
		}
		bg->EnumAllActor([&](const boost::shared_ptr<MtActor>& actor) {
			if (actor->Guid() == msg->source_guid())
				actor->OnMoveFinishMsg(player, msg);
		});*/
		return true;
	}

	bool MtScene::OnBattleGroundClientActorMove(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientActorMove>& message, int32 /*source*/)
	{
		auto bg = player->BattleGround();
		if (bg == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
			return true;
		}
		bg->EnumAllActor([&](const boost::shared_ptr<MtActor>& actor) {
			if (actor->Guid() == message->guid() && actor->Player().lock() != nullptr && actor->Player().lock()->Guid() == player->Guid()) {
				if (actor->ManualControl() == false) {
					player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
				}
				else {
/*
					auto command = boost::make_shared<ClientMoveCommand>(message);
					actor->SetClientCommand(command);*/
				}

			}
		});
		return true;
	}

	bool MtScene::OnBattleGroundClientActorStop(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientActorStop>& message, int32 /*source*/)
	{
		auto bg = player->BattleGround();
		if (bg == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
			return true;
		}
		bg->EnumAllActor([&](const boost::shared_ptr<MtActor>& actor) {
			if (actor->Guid() == message->guid() && actor->Player().lock() != nullptr && actor->Player().lock()->Guid() == player->Guid()) {
				if (actor->ManualControl() == false) {
					player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
				}
				else {
/*
					auto command = boost::make_shared<ClientStopCommand>(message);
					actor->SetClientCommand(command);*/
				}
			}
		});
		return true;
	}

	bool MtScene::OnBattleGroundClientActorUseSkill(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ClientActorUseSkill>& message, int32 /*source*/)
	{
		auto bg = player->BattleGround();
		if (bg == nullptr) {
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
			return true;
		}
		bg->EnumAllActor([&](const boost::shared_ptr<MtActor>& actor) {
			if (actor->Guid() == message->guid() && actor->Player().lock() != nullptr && actor->Player().lock()->Guid() == player->Guid()) {
				if (actor->ManualControl() == false) {
					player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::UnknownError);
				}
				else {
/*					auto command = boost::make_shared<ClientActorUseSkill>(message, [=]() {return actor->UseSkill(); });*/
/*					actor->SetClientCommand(command);*/
				}
			}
		});
		return true;
	}

	LogicArea MtScene::GetCurrentLogicArea(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player->Zone()) {
			return LogicArea(player->Zone());
		}
		return LogicArea();
	}

	LogicArea MtScene::GetPlayerLogicArea(const boost::shared_ptr<MtPlayer>& player)
	{
		if (player == nullptr) {
			return LogicArea();
		}
		auto zone = GetZoneByPos(player->Position());
		if (zone) {
			return LogicArea(zone);
		}
		else {
			return LogicArea();
		}
	}
	LogicArea MtScene::GetInitLogicArea(const boost::shared_ptr<Packet::PlayerEnterScene>& message)
	{		
		auto & init_pos = message->has_init_pos() ? message->init_pos() : config_->spawn_pos(0);
		auto zone = GetZoneByPos(init_pos);
		if (zone) {
			return LogicArea(zone);
		}
		else {
			return LogicArea();
		}
	}

	void MtScene::__regist_player_2_scene(const boost::shared_ptr<MtPlayer>& player)
	{
		if (tick_player_map_locked_ == true) {
			throw IteratorException("__on_player_enter tick makes server crash");
		}
		auto first_enter = player_map_.find(player->Guid()) == player_map_.end();
		LOG_INFO << "[player enter scene]," << player->Guid()
			<< "scene:" << config_->id() <<",first:" << first_enter;
		if (loop_) loop_->assert_in_loop_thread();
		if (first_enter) {
			player_map_.insert(std::make_pair(player->Guid(), player));
		}
		player->SetScene(shared_from_this());
	}

	void MtScene::__on_player_leave(const boost::shared_ptr<MtPlayer>& player)
	{
		if (tick_player_map_locked_ == true) {
			throw IteratorException("__on_player_leave tick makes server crash");
		}
		if (player_map_.find(player->Guid()) == player_map_.end()) {
			ZXERO_ASSERT(false) << "should find player when player leave scene" << SceneId() << player->Guid();
			//return;
		}
		if (loop_) loop_->assert_in_loop_thread();
		player->QuitBattle();
		player->QuitHookBattle();
		player->OnLeaveScene();
		player_map_.erase(player->Guid());
		player->SetScene(nullptr);
	}

	bool MtScene::OnArenaPanelOpen(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaPanelOpen>& message, int32 /*source*/)
	{
		message;
		MtArenaManager::Instance().OnOpenArenaPanel(player);
		return true;
	}

	bool MtScene::OnArenaRefreshTarget(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaRefreshTarget>& message, int32 /*source*/)
	{
		message;
		MtArenaManager::Instance().OnRefreshPlayerArenaMainData(player);
		return true;
	}

	bool MtScene::OnArenaPanelClose(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaPanelClose>& message, int32 /*source*/)
	{
		message;
		MtArenaManager::Instance().OnCloseArenaPanel(player);
		return true;
	}

	bool MtScene::OnArenaChallengePlayer(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ArenaChallengePlayer>& message, int32 /*source*/)
	{
		auto curr_formation = player->ActorFormation();
		auto target_form_it = player->GetActorFormation(Packet::ActorFigthFormation::AFF_PVP_ARENA);
		if (target_form_it->actor_guids_size() <= 0) {
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::InvalidFormData);
		}

		auto arena_user = MtArenaManager::Instance().FindArenaUser(player->Guid());
		if (arena_user == nullptr) {
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Wait_Loading);
			return true;
		}
		if (MtArenaManager::Instance().TargetBusy(message->target_guid())) {
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::Target_Busy);
			return true;
		}

		auto config_data = MtConfigDataManager::Instance().FindConfigValue("arena_max_time_per_day");
		auto arena_max_time_per_day = config_data->value1();
		auto challenge_count_today = player->DataCellContainer()->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ArenaChallengeTimeToday);
		if (challenge_count_today >= arena_max_time_per_day) {
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::MaxLimit);
			return true;
		}

		MtArenaManager::Instance().OnChallengePlayer(player->Guid(), message->target_guid(), 1);

		return true;
	}

	bool MtScene::OnArenaBuyChallengeCount(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::ArenaBuyChallengeCount>& msg, int32 /*source*/)
	{
		msg;
		auto config_data = MtConfigDataManager::Instance().FindConfigValue("arena_buy_challange_time_gold");
		auto crystal = config_data->value1();
		crystal *= (1 + player->DataCellContainer()->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ArenaBuyTimesToday));
		if (crystal < 0) {
			player->SendClientNotify("server_config_error", -1, -1);
		}
		if (!player->DelItemById(Packet::TokenType::Token_Crystal, crystal, Config::ItemDelLogType::DelType_Arena)) {
			player->SendCommonResult(Packet::ResultOption::ArenaOp, Packet::ResultCode::CrystalNotEnough);
			return true;
		}
		player->DataCellContainer()->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ArenaBuyTimesToday, 1, true);
		player->DataCellContainer()->set_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ArenaChallengeTimeToday, 0, true);
		MtArenaManager::Instance().OnBuyChallengeCount(player);
		return true;
	}

	bool MtScene::OnReNameRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ReNameRequest>& message, int32 /*source*/)
	{
		auto container = player->DataCellContainer();
		if (container == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::UnknownError);
			return true;
		}
		if (!MtWordManager::Instance()->InvalidName(message->name())) {
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::InvalidName);
			return true;
		}
		int32 count = container->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ReNameCount);
		if (count <= 0) {
			//首次免费
			container->set_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ReNameCount, 1,true);
		}
		else {
			auto config = MtConfigDataManager::Instance().FindConfigValue("change_player_name");
			if (config == nullptr) {
				player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::InvalidConfigData);
				return true;
			}

			//优先扣代币
			if (!player->DelItemById(config->value4(), 1, Config::ItemDelLogType::DelType_Rename)) {
				//其次钻石
				if (!player->DelItemById(Packet::TokenType::Token_Crystal, config->value3(), Config::ItemDelLogType::DelType_Rename)) {
					player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::CrystalNotEnough);
					return true;
				}
			}
			container->add_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::ReNameCount, 1,true);
		}

		player->SetName(message->name());
		if (player->GetGuildID() != INVALID_GUID)
		{
			Server::Instance().SendS2GCommonMessage("S2GSyncGuildUserName", {}, { (int64)player->Guid(),(int64)player->GetGuildID() }, { message->name() });
		}

		//广播一下
		Packet::CommonReply reply;
		reply.set_reply_name("rename_ok");
		reply.add_int64_params(player->Guid());
		reply.add_string_params(message->name());

		player->BroadCastMessage(reply);
		player->SendClientNotify("action_notify_1",-1,-1);
		return true;
	}

	bool MtScene::OnSendGiftReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SendGiftReq>& message, int32 /*source*/)
	{
		auto target_player = FindPlayerBySceneServer(message->player_guid());
		if (target_player == nullptr)
		{
			ZXERO_ASSERT(false);
			return true;
		}
		auto targetscene = target_player->Scene();
		if (targetscene == nullptr)
		{
			ZXERO_ASSERT(false);
			return true;
		}

		uint32 itemcount = message->items().size();
		if (itemcount <= 0 || itemcount >= 5){
			return true;
		}
		//判断空间todo
		if (!target_player->BagLeftCapacity(Packet::ContainerType::Layer_ItemBag, 5)) {
			player->SendClientNotify("ItemBagIsFull", -1, -1);
			return true;
		}
		
		bool del = true;
		for (auto item_ : message->items())
		{
			int32 item_id = item_.itemid();
			int32 item_count = item_.itemcount();
			bool ret = player->DelItemById(item_id, item_count, Config::ItemDelLogType::DelType_Friend);
			if (ret == false)
			{
				ret = false;
			}
		}
		if (del == false)
		{
			return true;
		}
		if (targetscene->SceneId() != SceneId())
		{
			targetscene->QueueInLoop(boost::bind(&MtPlayer::OnSendGiftGiveReq, target_player, player, message));
			return true;
		}
		target_player->OnSendGiftGiveReq( player , message);
		return true;
	}

	bool MtScene::OnActorFormationPull(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ActorFormationPull>& message, int32 /*source*/)
	{
		message;
		Packet::ActorFormationAll reply;
		auto curr_formations = player->ActorFormation();
		for (auto& form : curr_formations) {
			auto tmp_form = reply.add_all_formations();
			tmp_form->CopyFrom(*form);
		}
		player->SendMessage(reply);
		return true;
	}
	bool MtScene::CheckFormation(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetActorFightReq>& message)
	{
		if (player == nullptr || message == nullptr) {
			return false;
		}

		Packet::LastHpType lasthp = Packet::LastHpType::LastHpMax;
		if (message->type() == Packet::ActorFigthFormation::AFF_PVP_TRIAL_FIELD) {
			lasthp = Packet::LastHpType::HellLastHp;
		}
		else if (message->type() == Packet::ActorFigthFormation::AFF_PVP_BATTLE_FIELD) {
			lasthp = Packet::LastHpType::BFLastHp;
		}
		else if (message->type() == Packet::ActorFigthFormation::AFF_PVP_GUARD) {
			lasthp = Packet::LastHpType::GBLastHp;
		}
		else if (message->type() == Packet::ActorFigthFormation::AFF_PVE_GUILD_BOSS) {
			lasthp = Packet::LastHpType::GBOSSLastHp;
		}
		
		if (lasthp != Packet::LastHpType::LastHpMax){
			for (auto actor_guid : message->guids()) {
				auto actor = player->FindActor(actor_guid);
				if (actor == nullptr) {
					player->SendClientNotify("NoActor", -1, -1);
					return false;
				}
				if (actor->GetLastHp(lasthp) <= 0) {
					//试炼场血量为0不许上阵	
					player->SendClientNotify("ActorIsDead", -1, -1);
					return false;
				}
			}
		}
		return true;
	}

	bool MtScene::OnSetActorFight(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::SetActorFightReq>& message, int32 /*source*/)
	{
		//1 检查一下客户端数据有效性
		//2 清空原有设置, 把新的设置覆盖过去, 注意保证actor_guids_size大小不变
		if (!Packet::ActorFigthFormation_IsValid(message->type())) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::InvalidFormData);
			return true;
		}
		std::set<uint64> guid_set;//检查一下guid重复了没
		for (auto guid : message->guids())
		{
			guid_set.insert(guid);
		}
		if (int32(guid_set.size()) != message->guids_size())
		{
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::Invalid_Request);
			return true;
		}
		if (message->guids_size() <= 0 || message->guids_size() > 5) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::Invalid_Request);
			return true;
		}

		if (!CheckFormation(player, message)) {
			return true;
		}
		if (message->type() != Packet::ActorFigthFormation::AFF_PVP_TRIAL_FIELD && message->type() != Packet::ActorFigthFormation::AFF_PVE_GUILD_BOSS) {
			bool has_main = false;
			for (auto actor_guid : message->guids()) {
				auto actor = player->FindActor(actor_guid);
				if (actor == nullptr) {
					player->SendClientNotify("NoActor", -1, -1);
					return true;
				}
				if (actor->MainActor()) {
					has_main = true;
				}
			}
			if (!has_main) {
				player->SendClientNotify("bukegenghuanzhujue", -1, -1);
				return true;
			}
		}
		auto curr_formation = player->ActorFormation();
		auto target_form_it = player->GetActorFormation(message->type());
		boost::shared_ptr<Packet::ActorFormation> new_form;
		if (target_form_it == nullptr) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::Invalid_Request);
			return true;
		}
		else {
			new_form = target_form_it;
		} 
		for (auto i = 0; i < new_form->actor_guids_size(); ++i) {
			new_form->set_actor_guids(i, INVALID_GUID);
		}
		for (auto i = 0; i < message->guids_size() && i < new_form->actor_guids_size(); ++i) {
			new_form->set_actor_guids(i, message->guids(i));
		}
		if (message->type() == Packet::ActorFigthFormation::AFF_STAGE_GUARD) {
			//通知Server更新占领关卡的玩家阵容
			auto update_msg = boost::make_shared<S2G::PlayerUpdateCaptureForm>();
			update_msg->mutable_player_info()->CopyFrom(*player->GetScenePlayerInfo());
			for (auto a : player->BattleActor(Packet::ActorFigthFormation::AFF_STAGE_GUARD)) {
				a->SerializationToMessage(*update_msg->add_actors());
			}
			Server::Instance().SendMessage(update_msg, player);
		}
		player->SendMessage(*new_form);

		player->OnLuaFunCall_x("xOnSetActorFight",
		{
			{ "value",message->guids_size() }
		});
		
		//更新战力
		player->UpdateBattleScore();
		return true;
	}

	bool MtScene::OnPlayerTrialInfoLoad(const uint64 player_guid)
	{
		auto player = FindPlayer(player_guid);
		if (player == nullptr) {
			return true;
		}

		int32 wave = 1;
		auto msg = boost::make_shared<S2G::RefreshTarget>();		
		auto container = player->DataCellContainer();
		if (container != nullptr) {
			wave = container->get_data_32(Packet::CellLogicType::NumberCell, Packet::NumberCellIndex::HellDayWaveCount);
		}
		msg->set_wave(wave);
		Server::Instance().SendMessage(msg, player);

		//Server::Instance().RunInLoop(boost::bind(&TrialFieldManager::RefreshTarget, boost::ref(TrialFieldManager::Instance()), player, 1));
		return true;
	}


	bool MtScene::OnTrialPlayerLoadDone(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<DB::PlayerTrialInfo>& msg, int32 /*source*/)
	{
		if (player == nullptr || msg == nullptr) {
			return true;
		}
		//根据msg里面的数据让客户端刷出模型
		//int32 n = msg->target().targets_size();
		//std::vector<int32> poslist;
		//try {
		//	poslist = thread_lua->call<vector<int>>(2002, "GetPostionList", this);
		//	if ((int32)poslist.size() < n) {
		//		ZXERO_ASSERT(false) << "position list not enough!";
		//		return;
		//	}
		//}
		//catch (ff::lua_exception& e) {
		//	ZXERO_ASSERT(false) << "lua exception:" << e.what() << "scene id=" << SceneId();
		//	return;
		//}

		////抽牌
		//std::vector<int32> seeds;
		//for (uint32 i = 0; i < poslist.size(); i++) {
		//	seeds.push_back(i);
		//}
		//std::vector<int32> temp;
		//while (n-- > 0) {
		//	int32 seed = (int32)rand_gen->intgen(0, seeds.size() - 1);
		//	temp.push_back(poslist[seeds[seed]]);
		//	seeds.erase(seeds.begin() + seed);
		//	if (seeds.empty()) {
		//		break;
		//	}
		//}

		//更新posid
		for (int i = 0; i < msg->target().targets_size(); i++) {
			auto form = msg->mutable_target();
			auto target = form->mutable_targets(i);
			target->set_posid(1140100004);
		}
		player->SetPlayerNpc(msg);
		return true;
	}

	bool MtScene::IsRaidScene(const int32 sceneid)
	{
		return sceneid >= 1000 && sceneid < 3000;
	}

	bool MtScene::IsDynamicNpc(const int32 npcid)
	{
		return npcid >= 10000;
	}

	boost::shared_ptr<MtPlayer> MtScene::FindPlayer(uint64 guid)
	{
		auto it = player_map_.find(guid);
		if (it == std::end(player_map_)) {
			return nullptr;
		} else {
			return it->second;
		}
	}

	bool MtScene::OnDelMessageReq(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::DelMessageReq>& message, int32 /*source*/)
	{
		uint64 messageguid = message->message_id();
		Packet::ResultCode ret = player->del_message(messageguid);
		player->SendCommonResult(Packet::ResultOption::Message_Opt, ret);

		if (ret == Packet::ResultCode::ResultOK)
		{
			Packet::DelMessageReq delm;
			delm.set_message_id(messageguid);
			player->SendMessage(delm);
		}
		return true;
	}

	void MtScene::OnShutDown()
	{
		LOG_INFO << "[MtScene] " << SceneId() << " quit";
		loop_ = nullptr;
	}

	bool MtScene::OnChangeHair(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::ChangeHair>& message, int32 /*source*/)
	{
		auto main_actor = player->MainActor();
		auto hairs = MtActorConfig::Instance().ProfHairs(main_actor->Professions());
		if (std::find(hairs.begin(), hairs.end(), message->new_hair()) == hairs.end()) {
			player->SendCommonResult(Packet::ResultOption::Actor_Opt, Packet::ResultCode::Invalid_Request);
			return true;
		} else {
			main_actor->DbInfo()->set_hair(message->new_hair());
		}
		Packet::CommonReply reply;
		reply.set_reply_name("change_hair_ok");
		reply.add_int64_params(player->Guid());
		reply.add_int32_params(message->new_hair());
		player->BroadCastMessage(reply);
		return true;
	}

	void MtScene::DispatchMessage(const boost::shared_ptr<google::protobuf::Message>& msg,
		const boost::shared_ptr<MtPlayer>& player,
		int32 source)
	{
		if (loop_ == nullptr) {
			return;
		}
		client_message_dispatcher_->handle_message(player, msg, source);
	}

	void MtScene::ExecuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg,
		const boost::shared_ptr<MtPlayer>& player)
	{
		if (loop_ == nullptr) {
			return;
		}
		inner_message_dispatcher_->handle_message(player, msg, MessageSource::GAME_EXECUTE);
	}

	void MtScene::BroadcastExcuteMessage(const boost::shared_ptr<google::protobuf::Message>& msg)
	{
		if (loop_ == nullptr) {
			return;
		}
		auto temp_map = player_map_;//防止迭代器失效
		boost::for_each(temp_map, [=](auto& it) {
			inner_message_dispatcher_->handle_message(it.second, msg, MessageSource::GAME_EXECUTE);
		});
	}

	void MtScene::LuaDispatchMessage(const google::protobuf::Message* msg)
	{
		if (msg != nullptr) {
			boost::shared_ptr<google::protobuf::Message> m = nullptr;
			m.reset(const_cast<google::protobuf::Message*>(msg));
			DispatchMessage(m, nullptr, MessageSource::SCENE);
		}
	}
	void MtScene::LuaExecuteMessage(const google::protobuf::Message* msg)
	{
		if (msg != nullptr) {
			boost::shared_ptr<google::protobuf::Message> m = nullptr;
			m.reset(const_cast<google::protobuf::Message*>(msg));
			ExecuteMessage(m,nullptr);
		}
	}

	void MtScene::SendS2SCommonMessage(const std::string &name, const std::vector<int32>& int32values, const std::vector<int64>& int64values, const std::vector<std::string>& stringvalues)
	{
		auto message = boost::make_shared<S2S::S2SCommonMessage>();
		message->set_request_name(name);

		for (auto child : int32values) {
			message->add_int32_params(child);
		}
		for (auto child : int64values) {
			message->add_int64_params(child);
		}
		for (auto child : stringvalues) {
			message->add_string_params(child);
		}
		ExecuteMessage(message, nullptr);
	}

	bool MtScene::HandlePlayerMessage(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<google::protobuf::Message>& msg, int32 source)
	{
		if (loop_ == nullptr) {
			return true;
		}
		return client_message_dispatcher_->direct_handle_message(player, msg, source);
	}

	bool MtScene::HandleInnerMessage(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<google::protobuf::Message>& msg, int32 source)
	{
		if (loop_ == nullptr) {
			return true;
		}
		return inner_message_dispatcher_->direct_handle_message(player, msg, source);
	}

	bool MtScene::OnG2SCommonMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::G2SCommonMessage>& message, int32 /*source*/)
	{
		//player不是必须的，player可以是null
		try {
			thread_lua->call<int32>(10003, "HandleG2SCommonMessage", this, player, message);
			return true;
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return true;
		}
	}

	bool MtScene::OnS2SCommonMessage(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<S2S::S2SCommonMessage>& message, int32 /*source*/)
	{
		//player不是必须的，player可以是null
		try {
			thread_lua->call<int32>(10003, "HandleS2SCommonMessage", this, player, message);
			return true;
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return true;
		}
	}

	bool MtScene::OnCommonRequest(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::CommonRequest>& message, int32 /*source*/)
	{
		if (player == nullptr)
			return true;

		player->OnLuaFunCall_OnMessageHandler("xOnCommonRequestPacket", message.get());

		return true;
	}

	bool MtScene::OnG2SSyncPlayerGuildData(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::SyncPlayerGuildData>& message, int32 /*source*/)
	{
		if (player == nullptr)
			return true;
		player->GetPlayerGuildProxy()->G2SSyncPlayerGuildData(message);
		return true;
	}
	bool MtScene::OnG2SCodeCheckDone(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<G2S::G2SCodeCheckDone>& message, int32 /*source*/)
	{
		if (player == nullptr)
		{
			ZXERO_ASSERT(false);
			return true;
		}
		int32 ret = message->ret();
		if (ret == 0)
		{
			for (auto item : message->items())
			{
				auto itemid = item.itemid();
				auto itemcount = item.itemcount();
				if (itemid > 0 && itemcount > 0)
				{
					player->AddItemByIdWithNotify(itemid, itemcount, Config::ItemAddLogType::AddType_CDK);
				}
			}
			player->SendClientNotify("cdk_ok", -1, -1);
		}
		else if (ret < 0)
		{
			player->SendClientNotify("cdk_error", -1, -1); 
		}
		else 
		{
			player->SendClientNotify((boost::format("cdk_ret_%1%")%ret).str(), -1, -1);
		}
		LOG_INFO << "G2SCodeCheckDone playerid="<< player->Guid()<<" ";
		return true;
	}
	bool MtScene::OnLuaRequestPacket(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::LuaRequestPacket>& message,int32 /*source*/)
	{
		if (player == nullptr) {
			return true;
		}

		player->OnLuaFunCall_OnMessageHandler("xOnLuaRequestPacket", message.get());
		return true;
	}

	MtBattleGroundManager& MtScene::GetBattleManager() {
		return battle_manager_;
	}
	MtLuaCallProxy* MtScene::GetLuaCallProxy() 
	{ 
		return &lua_call_; 
	}
	
	bool MtScene::OnCollectTax(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::CollectAllTax>& message, 
		int32 source)
	{
		if (source != MessageSource::GAME_EXECUTE || player == nullptr) {
			return false;
		}
		LOG_INFO << "[scene_stage] player:" << player->Guid() << " collect tax" << message->tax();
		player->AddItemById(Packet::TokenType::Token_Gold, message->tax(), Config::ItemAddLogType::AddType_Stage_Tax);
		std::string notify = (boost::format("SCENE_STAGE_TAX_COLLECT_OK|%1%") % message->tax()).str();
		player->SendClientNotify(notify, -1, -1);

		auto config = MtSceneStageManager::Instance().GetConfig(message->stage_id());
		if (config != nullptr && message->tax() >= config->tax_broadcast_mark()) {
			std::stringstream msg;
			msg << "ac_notify_030|" << player->Name() << "|" << config->name() << "|" << MtItemManager::Instance().GetItemName(Packet::TokenType::Token_Gold) << "|" << message->tax();
			send_run_massage(msg.str());
		}

		return true;
	}

	bool MtScene::OnCaptureStageToScene(
		const boost::shared_ptr<MtPlayer>& player, 
		const boost::shared_ptr<Packet::CaptureStageToScene>& msg, int source)
	{
		if (source != MessageSource::GAME_EXECUTE || player == nullptr) {
			return false;
		}
		LOG_INFO << "[scene_stage] player:" << player->Guid()
			<< " capture stage:" << msg->client_req().stage_id()
			<< " req got in scene:" << SceneId();
		auto stage_config = MtSceneStageManager::Instance().GetConfig(msg->client_req().stage_id());
		if (stage_config == nullptr) {
			player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::InvalidConfigData);
			return true;
		}
		if (stage_config->scene_id() == SceneId()) {
			auto defer = Defer([&]() {player->GetPlayerStageProxy()->ClearCaptureStageTempInfo(); });
			std::vector<Packet::ActorFullInfo> actors;
			actors.reserve(msg->actors_size());
			std::transform(msg->actors().begin(), msg->actors().end(), std::back_inserter(actors),
				[](auto& a) {return a; });
			player->GetPlayerStageProxy()->SetCaptureStageTempInfo(
				msg->client_req().stage_id(),
				actors);
			auto battle = battle_manager_.CreateFrontBattle(player.get(),nullptr,
			{
				{ "script_id", 1004 },
				{ "group_id", msg->client_req().battle_group_id() },
				{ "stage_id",msg->client_req().stage_id() }
			});
		} else {
			auto target_scene = MtSceneManager::Instance().GetSceneById(stage_config->scene_id());
			if (target_scene == nullptr) {
				player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::Wrong_Scene);
				return true;
			}

			if (player->GoTo(stage_config->scene_id(), INVALID_POS)) {
				//结束当前战斗
				player->QuitBattle();
				player->ExecuteMessage(msg);//投递挂机战斗消息			
			}
		}
		return true;
	}

	bool MtScene::OnRobotPlayerInOk(const boost::shared_ptr<MtPlayer>& player, const boost::shared_ptr<Packet::RobotPlayerInOk>& msg, int32 /*source*/)
	{
		player->OnRobotPlayerInOk(msg);
		return true;
	}

	void MtScene::__after_player_regist_2_scene(const boost::shared_ptr<MtPlayer>& player)
	{
		Packet::PlayerEnterSceneReply reply;
		auto info = reply.mutable_info();
		info->CopyFrom(player->GetDBPlayerInfo()->basic_info());
		info->set_move_speed(player->GetMoveSpeed());
		reply.mutable_main_actor()->CopyFrom(*player->MainActor()->DbInfo().get());
		player->SendMessage(reply);
	}

}

