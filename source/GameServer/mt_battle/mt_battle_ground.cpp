#include "../mt_player/mt_player.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_scene/mt_scene_manager.h"
#include "../mt_stage/mt_scene_stage.h"
#include "../mt_stage/mt_player_stage.h"
#include "../mt_battle/mt_trap.h"
#include "../mt_server/mt_server.h"
#include "../base/mt_db_load_work.h"
#include "buffer.h"
#include "protobuf_codec.h"
#include "mt_battle_ground.h"
#include "mt_battle_object.h"
#include <SceneConfig.pb.h>
#include <SceneStage.pb.h>
#include <Team.pb.h>
#include <BattleReply.pb.h>
#include <SceneStageConfig.pb.h>
#include <SceneObject.pb.h>
#include <SceneCommon.pb.h>
#include <MonsterConfig.pb.h>
#include <BattleExpression.pb.h>
#include <AllPacketEnum.proto.fflua.h>
#include <S2GMessage.pb.h>
#include "../mt_actor/mt_actor_state.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_actor/mt_actor_config.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_skill/skill_context.h"
#include "../mt_team/mt_team_manager.h"
#include "../mt_mission/mt_mission_proxy.h"
#include "mt_battle_command.h"
#include "battle_event.h"
#include <boost/make_shared.hpp>
#include "BattleGroundInfo.pb.h"

namespace Mt
{
	namespace Battle
	{
		void BattleGround::OnTick(uint64 elapseTime)
		{
			if (status_ == Packet::BattleGroundStatus::FIGHTING) {

				side_1_objects_.erase(std::remove_if(std::begin(side_1_objects_), std::end(side_1_objects_), boost::bind(&BaseObject::OnTick, _1, elapseTime)), std::end(side_1_objects_));
				side_2_objects_.erase(std::remove_if(std::begin(side_2_objects_), std::end(side_2_objects_), boost::bind(&BaseObject::OnTick, _1, elapseTime)), std::end(side_2_objects_));
				for (auto& fun : delay_commands_) { fun(); }

				try {
					auto winner_camp = thread_lua->call<int32>(script_id_, "OnCheckBattleEnd", this);
					if (winner_camp != 0) {
						thread_lua->call<bool>(script_id_, "OnBattleEnd", this, false, winner_camp);
					}
				}
				catch (ff::lua_exception& e) {
					ZXERO_ASSERT(false) << "lua exception:" << e.what();
					return;
				}
			}

		}
		int32 BattleGround::GetWinnerSide()
		{
			if (AllObjectDead(1))	return 1;
			if (AllObjectDead(2))	return 2;
			return 0;
		}

		bool BattleGround::AllObjectDead(int32 camp)
		{
			if (camp == 1) {
				return std::all_of(std::begin(side_1_objects_), std::end(side_1_objects_), boost::bind(&BaseObject::BeSupposeToDead, _1));
			}
			if (camp == 2) {
				return std::all_of(std::begin(side_2_objects_), std::end(side_2_objects_), boost::bind(&BaseObject::BeSupposeToDead, _1));
			}
			return true;
		}
		bool BattleGround::AddObject(int32 camp, const boost::shared_ptr<BaseObject>& object)
		{
			switch (camp)
			{
			case 1:
				if (status_ == Packet::BattleGroundStatus::FIGHTING) {
					delay_commands_.push_back([&]() {side_1_objects_.push_back(object); });//战斗中正在遍历side_1_object, 不能直接插入, 会导致迭代器失效
				}
				else {
					side_1_objects_.push_back(object);
				}
				break;
			case 2:
				if (status_ == Packet::BattleGroundStatus::FIGHTING) {
					delay_commands_.push_back([&]() {side_2_objects_.push_back(object); });
				}
				else {
					side_2_objects_.push_back(object);
				}
				break;
			default:
				ZXERO_ASSERT(false) << " INVALID SIDE";
				return false;
				break;
			}
			return true;
		}

		void BattleGround::BroadCastMessage(const google::protobuf::Message& msg) const
		{
			creator_->SendMessage(msg);
			for (auto& audience : audiences_) {
				if (!audience.expired()) {
					audience.lock()->SendMessage(msg);
				}
			}
		}

		google::protobuf::uint64 BattleGround::FrameElapseTime() const
		{
			return creator_->Scene()->FrameElapseTime();
		}

		bool BattleGround::SendWinnerReward(int32 /*winner_camp*/, int32 /*packet_id*/)
		{
			return false;
		}

		void BattleGround::SendBattleEndMessage(int32 winner_camp)
		{
			Packet::BattleGroundOverNotify notify;
			notify.mutable_brief()->CopyFrom(brief_);
			notify.set_winner_camp(winner_camp);
			notify.set_time_stamp(brief_.elapse_time());
			BroadCastMessage(notify);
		}

		bool BattleGround::StarBattle()
		{
			ZXERO_ASSERT(initialized_) << "battle ground should be initialized properly";
			if (status_ == Packet::BattleGroundStatus::WAIT) {
				status_ = Packet::BattleGroundStatus::FIGHTING;
				return true;
			}
			else {
				ZXERO_ASSERT(false);
			}
			return false;
		}

		bool BattleGround::Init()
		{
			try {
				if (thread_lua->call<bool>(script_id_, "OnInitBattleGround", this)) {
					initialized_ = true;
					return true;
				}
				else {
					ZXERO_ASSERT(false);
					return false;
				}
			}
			catch (ff::lua_exception& e) {
				e;
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return false;
			}
		}

		bool BattleGround::InitSideOneObjects()
		{
			auto actors = creator_->BattleActor(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
			for (auto& actor : actors) {
				boost::shared_ptr<HeroActor> battle_obj;
				if (actor->MainActor()) {
					battle_obj = boost::make_shared<MainActor>(*this);
				}
				else {
					battle_obj = boost::make_shared<HeroActor>(*this);
				}
				battle_obj->Init(actor);
				AddObject(1, battle_obj);
			}
			return true;
		}

		bool BattleGround::InitSideTwoObjects()
		{
			auto battle_actors = creator_->BattleActor(Packet::ActorFigthFormation::AFF_NORMAL_HOOK);
			std::transform(std::begin(battle_actors),
				std::end(battle_actors),
				std::back_inserter(side_2_objects_),
				[=](auto& actor) {
				auto r = boost::make_shared<HeroActor>(*this);
				r->Init(actor);
				return r;
			}
			);
			return true;
		}

		BattleGround::BattleGround(const boost::shared_ptr<MtPlayer>& creator, const boost::shared_ptr<MtPlayer>& target)
			:creator_(creator), target_(target)
		{
			brief_.set_scene_id(creator_->GetSceneId());
			brief_.set_elapse_time(0);
		}

		bool BattleGround::InitPosition()
		{
			scene_battle_group_ = MtSceneManager::Instance().GetSceneBattleGroup({ creator_->GetSceneId(), brief_.battle_group_id() });
			if (!scene_battle_group_) {
				ZXERO_ASSERT(false) << "无法初始化站位数据";
				return false;
			}
			for (auto& obj : side_1_objects_) {
				const auto pos = scene_battle_group_->player_pos(obj->PositionIndex());
				obj->Position(pos);
			}
			for (auto& obj : side_2_objects_) {
				const auto pos = scene_battle_group_->enemy_pos(obj->PositionIndex());
				obj->Position(pos);
			}
			return true;
			/*std::sort(std::begin(side_1_objects_), std::end(side_1_objects_),
			[](const boost::shared_ptr<MtActor>& lhs, const boost::shared_ptr<MtActor>& rhs) {
			auto lhs_config = MtActorConfig::Instance().GetBaseConfig(lhs->DbInfo()->actor_config_id());
			auto rhs_config = MtActorConfig::Instance().GetBaseConfig(rhs->DbInfo()->actor_config_id());
			auto lhs_batch = lhs_config->ground_batch;
			auto rhs_batch = rhs_config->ground_batch;
			auto lhs_priority = lhs_config->ground_priority;
			auto rhs_priority = rhs_config->ground_priority;
			return (lhs_batch < rhs_batch) || (lhs_batch == rhs_batch && lhs_priority < rhs_priority);
			}
			);
			int32 batch_indexs[3] = { 0,5,10 };
			for (auto i = 0u; i < attackers_.size(); ++i) {
			auto actor = attackers_[i];
			auto base_config = MtActorConfig::Instance().GetBaseConfig(actor->DbInfo()->actor_config_id());
			auto actor_ground_batch = base_config->ground_batch;
			auto position_index = batch_indexs[actor_ground_batch]++;
			const Packet::Position& config_pos = scene_battle_group_->player_pos(position_index);
			actor->Position(config_pos);
			actor->PositionIndex(position_index);
			}
			//side 2
			for (auto i = 0u; i < defenders_.size(); ++i) {
			auto actor = defenders_[i];
			const Packet::Position& config_pos = scene_battle_group_->enemy_pos(actor->PositionIndex());
			actor->Position(config_pos);
			}*/
		}


		bool BattleGround::RefreshBattle()
		{
			status_ = Packet::BattleGroundStatus::WAIT;

			try {
				if (thread_lua->call<bool>(script_id_, "OnRefreshBattle", this)) {
					initialized_ = true;
					return true;
				}
				else {
					ZXERO_ASSERT(false);
					return false;
				}
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return false;
			}

		}

		bool BattleGround::RefreshSideOneObjects()
		{
			std::for_each(std::begin(side_1_objects_), std::end(side_1_objects_), boost::bind(&BaseObject::OnRefresh, _1));
			return true;
		}

		bool BattleGround::RefreshSideTwoObjects()
		{
			std::for_each(std::begin(side_2_objects_), std::end(side_2_objects_), boost::bind(&BaseObject::OnRefresh, _1));
			return true;
		}

		void BattleGround::Clear()
		{
			script_id_ = 0;
			side_1_objects_.clear(), side_2_objects_.clear(), audiences_.clear(), delay_commands_.clear();
			status_ = Packet::BattleGroundStatus::WAIT;
			initialized_ = false; scene_battle_group_ = nullptr;
			brief_.Clear();
		}

		bool BattleGround::BindBattleTypeAndScriptId(Packet::BattleGroundType type, int32 battle_group_id, int32 script_id /*= BATTLE_GROUND_DEFAULT_SCRIPT_ID*/)
		{
			Clear();
			script_id_ = script_id;
			brief_.set_type(type);
			brief_.set_elapse_time(0);
			SetSceneBattleGroup(battle_group_id);
			return Init();
		}

		bool BattleGround::SetSceneBattleGroup(int32 battle_group_id)
		{
			scene_battle_group_ = MtSceneManager::Instance().GetSceneBattleGroup({ 1, battle_group_id });
			if (scene_battle_group_ != nullptr) {
				brief_.set_battle_group_id(battle_group_id);
				return true;
			}
			else {
				return false;
			}
		}

	}
	void MtBattleGround::lua_reg(lua_State* ls)
	{

		ff::fflua_register_t<MtBattleGround, ctor()>(ls, "MtBattleGround")
			.def(&MtBattleGround::GetAllDeadCampID, "GetAllDeadCampID")
			.def(&MtBattleGround::BattleGroupDrop, "BattleGroupDrop")
			.def(&MtBattleGround::ClearDefender, "ClearDefender")
			.def(&MtBattleGround::CreateAttackerPlayerBattleActors, "CreateAttackerPlayerBattleActors")
			.def(&MtBattleGround::CreateDefenderPlayerBattleActors, "CreateDefenderPlayerBattleActors")
			.def(&MtBattleGround::CreateDefenderMonster, "CreateDefenderMonster")
			.def(&MtBattleGround::CreateDefenderFromStageCaptain, "CreateDefenderFromStageCaptain")
			.def(&MtBattleGround::CreateDefenderFromArena, "CreateDefenderFromArena")
			.def(&MtBattleGround::CreateDefenderFromHell, "CreateDefenderFromHell")
			.def(&MtBattleGround::TimeOut, "TimeOut")
			.def(&MtBattleGround::RefreshBattle, "RefreshBattle")
			.def(&MtBattleGround::IsAttacker, "IsAttacker")
			.def(&MtBattleGround::GetAttackers, "GetAttackers")
			.def(&MtBattleGround::GetDefenders, "GetDefenders")
			.def(&MtBattleGround::GetAttackerPlayers, "GetAttackerPlayers")
			.def(&MtBattleGround::GetDefenderPlayers, "GetDefenderPlayers")
			.def(&MtBattleGround::AddBossChallengeProgress, "AddBossChallengeProgress")
			.def(&MtBattleGround::CreateDefenderFromStageCaptain, "CreateDefenderFromStageCaptain")
			.def(&MtBattleGround::PlayerRaidStage, "PlayerRaidStage")
			.def(&MtBattleGround::RefreshBattleGroup, "RefreshBattleGroup")
			.def(&MtBattleGround::InitAttackerFromActorWithoutPlayer, "InitAttackerFromActorWithoutPlayer")
			.def(&MtBattleGround::InitDefenderFromActorWithoutPlayer, "InitDefenderFromActorWithoutPlayer")
			.def(&MtBattleGround::TransformAttackers2BattleActor, "TransformAttackers2BattleActor")
			.def(&MtBattleGround::TransformDefenders2BattleActor, "TransformDefenders2BattleActor")
			.def(&MtBattleGround::GetSceneId, "GetSceneId")
			.def(&MtBattleGround::GetStageId, "GetStageId")
			.def(&MtBattleGround::CreateBattleReply, "CreateBattleReply")
			.def(&MtBattleGround::GetBattleReply, "GetBattleReply")
			.def(&MtBattleGround::SaveBattleReply, "SaveBattleReply")
			.def(&MtBattleGround::AddStat2Reply, "AddStat2Reply")
			.def(&MtBattleGround::Init, "Init")
			.def(&MtBattleGround::OnPlayerReachPos, "OnPlayerReachPos")
			.def(&MtBattleGround::GetCreator, "GetCreator")
			.def(&MtBattleGround::GetTarget, "GetTarget")
			.def(&MtBattleGround::BattleOverNotify, "BattleOverNotify")
			.def(&MtBattleGround::GetRefreshWaitTime,"GetRefreshWaitTime")
			.def(&MtBattleGround::GetStatisticsDamage, "GetStatisticsDamage")		
			.def(&MtBattleGround::CleanDeadBattleActors, "CleanDeadBattleActors")
			.def(&MtBattleGround::PushHookStatisticsToStageManager, "PushHookStatisticsToStageManager")
			.def(&MtBattleGround::GetStatus, "GetStatus")
			.def(&MtBattleGround::SetStatus, "SetStatus")
			.def(&MtBattleGround::AddMonster, "AddMonster")
			.def(&MtBattleGround::DelActor, "DelActor")
			.def(&MtBattleGround::GetParams32, "GetParams32")
			.def(&MtBattleGround::GetParams64, "GetParams64")
			.def(&MtBattleGround::SetParams32, "SetParams32")
			.def(&MtBattleGround::SetParams64, "SetParams64")
			.def(&MtBattleGround::BattleType,"BattleType")
			.def(&MtBattleGround::Brief,"Brief")
			.def(&MtBattleGround::GetStat, "GetStat")
			.def(&MtBattleGround::GetScriptId,"GetScriptId")
			.def(&MtBattleGround::SetScriptId, "SetScriptId")
			.def(&MtBattleGround::BattleEnterNotify,"BattleEnterNotify")
			.def(&MtBattleGround::DeleteActorNow,"DeleteActorNow")
			.def(&MtBattleGround::SetClientInsight, "SetClientInsight")
			.def(&MtBattleGround::GetClientInsight, "GetClientInsight")
			;
	}
	MtBattleGround::MtBattleGround(MtPlayer* create_player, MtPlayer*  target_player)
		:MtBattleGround()
	{
		creator_ = create_player;
		target_ = target_player;
	}

	MtBattleGround::MtBattleGround()
	{
		brief_.set_guid(0);
		brief_.set_elapse_time(0);
		brief_.set_battle_group_id(1);
		brief_.set_scene_id(-1);
		brief_.set_type(Packet::BattleGroundType::NO_BATTLE);
		brief_.set_stage_id(0);
		brief_.set_can_manual_control(false);
		brief_.set_mission_id(-1);
	}


	MtBattleGround::~MtBattleGround() {
		creator_ = nullptr;
		target_ = nullptr;
	}
	
	void MtBattleGround::AddBossChallengeProgress(int32 scene_stage_id, int32 count)
	{
		auto proxy = creator_->GetPlayerStageProxy();
		if (proxy != nullptr){
			proxy->AddBossChallengeProgress(scene_stage_id, count);
			//proxy->SyncSpecStageToClient(scene_stage_id);
		}
	}

	void MtBattleGround::RefreshBattleGroup()
	{
		fight_start_time_ = MtTimerManager::Instance().CurrentTime();
		int32 battle_group_index = 1;
		if (scene_battle_group_ != nullptr) {
			battle_group_index = scene_battle_group_->index() + 1;
		}
		else {
			battle_group_index = brief_.battle_group_id();
		}

		if (battle_group_index > MtSceneManager::Instance().SceneBattleGroupCount(brief_.scene_id())) {
			battle_group_index = 1;
		}

		scene_battle_group_ = MtSceneManager::Instance().GetSceneBattleGroup({ brief_.scene_id(), battle_group_index }).get();
		if (scene_battle_group_ == nullptr) {
			ZXERO_ASSERT(false) << "scene_battle_group_ is null!";
			return;
		}

		brief_.set_battle_group_id(battle_group_index);

		if (creator_ != nullptr) {
			LOG_INFO << "RefreshBattleGroup battle_group_index=" << battle_group_index << " creator guid=" << creator_->Guid();
		}
	}

	bool MtBattleGround::OnNextBattle()
	{
		brief_.set_elapse_time(0);
		if (BattleType() != Packet::BattleGroundType::PVE_HOOK
			&& BattleType() != Packet::BattleGroundType::PVE_STAGE_BOSS
			&& BattleType() != Packet::BattleGroundType::PVE_SIMULATE
			)
		{
			ZXERO_ASSERT(false);
			return false;
		}
		RefreshBattleGroup();
		try {
			int32 ret = thread_lua->call<int>(script_id_, "OnCreateReady", this, creator_);
			if (ret != 0) {
				ZXERO_ASSERT(false) << "OnCreateReady error! code=" << ret << " scriptid=" << script_id_ << " guid=" << creator_->Guid();
				return false;
			}
		}
		catch (ff::lua_exception& e) {
			e;
			return false;
		}
		auto stage_config = MtSceneStageManager::Instance().GetConfig(brief_.stage_id());
		if (stage_config == nullptr) {
			waiter_ = seconds(15).total_milliseconds();
		} else {
			waiter_ = stage_config->avg_run_time();
		}
		status_ = Packet::BattleGroundStatus::WAIT;
		EnumAllActor(boost::bind(&MtActor::EnterBattleGround, _1, this));
		InitPosAndDir();
		Packet::RefreshBattleGround message;
		message.mutable_brief()->CopyFrom(Brief());
		message.set_time_stamp(TimeStamp());
		FillBattleGroundInfo(*message.mutable_new_info());
		BroadCastMessage(message);
		return true;
	}

	void MtBattleGround::RefreshBattle(int32 wait_mill_second /*= 0*/)
	{
		Brief().set_elapse_time(0);
		SetStatus(Packet::BattleGroundStatus::NEXT);
		stat_.Clear();
		waiter_ = wait_mill_second;
		battle_drops_.Clear();
		monster_kills_ = 0;
	}

	bool MtBattleGround::Init()
	{
		uint64 creator_guid = INVALID_GUID;
		if (creator_) creator_guid = creator_->Guid();
		try {
			auto FormType = thread_lua->call<int32>(10, "BattleType2Formation", brief_.type());
			if (creator_ && creator_->IsEmptyFormation((Packet::ActorFigthFormation)FormType)) {
				return false;
			}
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return false;
		}

		battle_time_ = 0;
		scene_battle_group_ = MtSceneManager::Instance().GetSceneBattleGroupById(brief_.scene_id(), brief_.battle_group_id()).get();
		statistics_ = boost::make_shared<Statistics>();

		try {
			int32 ret = thread_lua->call<int>(script_id_, "OnCreateReady", this, creator_);
			if (ret != 0) {
				ZXERO_ASSERT(false) << "OnCreateReady error! code=" << ret << " scriptid=" << script_id_ << " guid=" << creator_guid;
				return false;
			}
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "OnCreateReady exception! " << e.what() << " guid=" << creator_guid;
			return false;
		}

		auto relate_npc = GetParams32("relate_npc");
		if (creator_ != nullptr && relate_npc >= 0) {
			creator_->LockNpc(relate_npc, true);
		}

		//站位
		EnumAllActor(boost::bind(&MtActor::EnterBattleGround, _1, this));

		if (BattleType() != Packet::BattleGroundType::PVP_GUILD_CAPTURE_POINT) {
			//领土争夺可以出现某一方没人的情况
			if (attackers_.empty() || defenders_.empty()) {
				status_ = Packet::BattleGroundStatus::END;
				return false;
			}
		}
		
		InitPosAndDir();
		status_ = Packet::BattleGroundStatus::WAIT;
		waiter_ = seconds(15).total_milliseconds();
		SkillContext ctx;
		std::for_each(attackers_.begin(), attackers_.end(),
			boost::bind(&MtActor::BeforeBattleStartStub, _1, boost::ref(ctx)));
		std::for_each(defenders_.begin(), defenders_.end(),
			boost::bind(&MtActor::BeforeBattleStartStub, _1, boost::ref(ctx)));
		return true;
	}

	void MtBattleGround::InitPosAndDir() {
		InitAttackerPos();
		InitDefenderPos();
		InitDirection();
	}
	void MtBattleGround::InitAttackerPos() {
		if (scene_battle_group_ == nullptr) {
			ZXERO_ASSERT(false) << "scene_battle_group_ is null!";
			return;
		}
		std::for_each(attackers_.begin(), attackers_.end(), [](boost::shared_ptr<MtActor>& a) { a->SetSide(0); });
		std::sort(std::begin(attackers_), std::end(attackers_), 
			[](const boost::shared_ptr<MtActor>& lhs, const boost::shared_ptr<MtActor>& rhs) {
			auto lhs_config = MtActorConfig::Instance().GetBaseConfig(lhs->DbInfo()->actor_config_id());
			auto rhs_config = MtActorConfig::Instance().GetBaseConfig(rhs->DbInfo()->actor_config_id());

			if (lhs_config == nullptr){
				ZXERO_ASSERT(false) << "invalid config data ! configid="<<lhs->DbInfo()->actor_config_id();
				return false;
			}if (rhs_config == nullptr) {
				ZXERO_ASSERT(false) << "invalid config data ! configid="<<rhs->DbInfo()->actor_config_id();
				return false;
			}
			auto lhs_batch = lhs_config->ground_batch;
			auto rhs_batch = rhs_config->ground_batch;
			auto lhs_priority = lhs_config->ground_priority;
			auto rhs_priority = rhs_config->ground_priority;
			return (lhs_batch < rhs_batch) || (lhs_batch == rhs_batch && lhs_priority < rhs_priority);
			}
		);
		int32 batch_indexs[3] = { 0,6,10 };
		for (auto i = 0u; i < attackers_.size(); ++i) {
			auto actor = attackers_[i];
			auto base_config = MtActorConfig::Instance().GetBaseConfig(actor->DbInfo()->actor_config_id());
			if (base_config==nullptr) {
				ZXERO_ASSERT(false) << "invalid config data ! configid=" << actor->DbInfo()->actor_config_id();
				return;
			}
			auto actor_ground_batch = base_config->ground_batch;
			auto position_index = batch_indexs[actor_ground_batch]++;
			if (position_index >= scene_battle_group_->player_pos_size() - 1) {
				position_index = 0;
			}
			const Packet::Position& config_pos = scene_battle_group_->player_pos(position_index);
			actor->Position(config_pos);
			actor->SetPositionIndex(position_index);
		}
	}
	void MtBattleGround::InitDefenderPos() {
		if (scene_battle_group_ == nullptr) {
			ZXERO_ASSERT(false) << "scene_battle_group_ is null!";
			return;
		}
		std::for_each(defenders_.begin(), defenders_.end(), [](boost::shared_ptr<MtActor>& a) { a->SetSide(1); });
		if (defenders_.empty() == false && defenders_.front()->ConfigId() < 20000) { //20000之前是玩家actor, 30000开始是怪物
			std::sort(std::begin(defenders_), std::end(defenders_),
				[](const boost::shared_ptr<MtActor>& lhs, const boost::shared_ptr<MtActor>& rhs) {
				auto lhs_config = MtActorConfig::Instance().GetBaseConfig(lhs->DbInfo()->actor_config_id());
				auto rhs_config = MtActorConfig::Instance().GetBaseConfig(rhs->DbInfo()->actor_config_id());

				if (lhs_config == nullptr) {
					ZXERO_ASSERT(false) << "invalid config data ! configid=" << lhs->DbInfo()->actor_config_id();
					return false;
				}if (rhs_config == nullptr) {
					ZXERO_ASSERT(false) << "invalid config data ! configid=" << rhs->DbInfo()->actor_config_id();
					return false;
				}
				auto lhs_batch = lhs_config->ground_batch;
				auto rhs_batch = rhs_config->ground_batch;
				auto lhs_priority = lhs_config->ground_priority;
				auto rhs_priority = rhs_config->ground_priority;
				return (lhs_batch < rhs_batch) || (lhs_batch == rhs_batch && lhs_priority < rhs_priority);
			}
			);
			int32 batch_indexs[3] = { 0,6,10 };
			for (auto i = 0u; i < defenders_.size(); ++i) {
				auto actor = defenders_[i];
				auto base_config = MtActorConfig::Instance().GetBaseConfig(actor->DbInfo()->actor_config_id());
				if (base_config == nullptr) {
					ZXERO_ASSERT(false) << "invalid config data ! configid=" << actor->DbInfo()->actor_config_id();
					return;
				}
				auto actor_ground_batch = base_config->ground_batch;
				auto position_index = batch_indexs[actor_ground_batch]++;
				if (position_index >= scene_battle_group_->enemy_pos_size() - 1) {
					position_index = 0;
				}
				const Packet::Position& config_pos = scene_battle_group_->enemy_pos(position_index);
				actor->Position(config_pos);
				actor->SetPositionIndex(position_index);
			}
		}
		for (auto i = 0u; i < defenders_.size(); ++i) {
			auto actor = defenders_[i];
			auto position_index = actor->GetPositionIndex();
			if (position_index < 0 || position_index >= scene_battle_group_->enemy_pos_size() - 1) {
				position_index = 0;
			}
			const Packet::Position& config_pos = scene_battle_group_->enemy_pos(position_index);
			actor->Position(config_pos);
		}
	}
	void MtBattleGround::InitDirection()
	{
		if (scene_battle_group_ == nullptr) {
			ZXERO_ASSERT(false) << "scene_battle_group_ is null!";
			return;
		}
		auto left_side_pos = scene_battle_group_->player_pos(0);
		auto right_side_pos = scene_battle_group_->enemy_pos(0);
		if (!defenders_.empty()) {
			for (auto& attack_actor : attackers_) {
				auto dir = zxero::get_direction(attack_actor->Position(), (*defenders_.begin())->Position());
				attack_actor->Direction(dir);
			}
		}

		if (!attackers_.empty()) {
			for (auto& defend_actor : defenders_) {
				auto dir = zxero::get_direction(defend_actor->Position(), (*attackers_.begin())->Position());
				defend_actor->Direction(dir);
			}
		}
	}

	void MtBattleGround::EnumAllActor(const boost::function<void(const boost::shared_ptr<MtActor>& actor)>& functor)
	{
		EnumActors(attackers_, functor);
		EnumActors(defenders_, functor);
	}

	std::vector<MtActor*> MtBattleGround::Enemies(MtActor& self, ActorSelector selector /*= ActorSelector()*/)
	{
	auto it = std::find_if(std::begin(attackers_), std::end(attackers_), [&](auto& actor) {
		return actor->Guid() == self.Guid();
	});
	if (selector.empty()) {
		selector = [](const boost::shared_ptr<MtActor>& /*actor*/) {return true; };
	}
	std::vector<MtActor*> result;
	if (it != std::end(attackers_)) { //自己是攻击方
		std::for_each(defenders_.begin(), defenders_.end(),
			[&](auto& actor) { if (selector(actor)) result.push_back(actor.get()); });
	} else
	{
		std::for_each(attackers_.begin(), attackers_.end(),
			[&](auto& actor) { if (selector(actor)) result.push_back(actor.get()); });
	}
	return result;
	}

	std::vector<MtActor*> MtBattleGround::TeamMates(MtActor& self, ActorSelector selector /*= ActorSelector()*/)
	{
		auto it = std::find_if(std::begin(attackers_), std::end(attackers_), [&](auto& actor) {
			return actor->Guid() == self.Guid();
		});
		if (selector.empty()) {
			selector = [](const boost::shared_ptr<MtActor>& /*actor*/) {return true; };
		}
		std::vector<MtActor*> result;
		if (it != std::end(attackers_)) { //自己是攻击方
			std::for_each(attackers_.begin(), attackers_.end(),
				[&](auto& actor) { if (selector(actor)) result.push_back(actor.get()); });
		} else
		{
			std::for_each(defenders_.begin(), defenders_.end(),
				[&](auto& actor) { if (selector(actor)) result.push_back(actor.get()); });
		}
		return result;
	}

	void MtBattleGround::EnumActors(const std::vector<boost::shared_ptr<MtActor>>& actors, const boost::function<void(const boost::shared_ptr<MtActor>& actor)>& functor)
	{
		boost::for_each(actors, functor);
	}

	bool MtBattleGround::OnTick(zxero::uint64 elapseTime)
	{
		if (status_ == Packet::BattleGroundStatus::END) {//战斗可能由外部置为end
			return true;
		}

		if (status_ == Packet::BattleGroundStatus::NEXT) {
			waiter_ -= elapseTime;
			if (waiter_ > 0) {
				return false;
			} else {
				if (OnNextBattle()) {
					return false;
				} else {
					status_ = Packet::BattleGroundStatus::END;
					return true;
				}
			}
		}
		brief_.set_elapse_time(brief_.elapse_time() + elapseTime);
		if (sim_pause_ == true)
			return false;
		if (status_ == Packet::BattleGroundStatus::WAIT) {
			waiter_ -= elapseTime;
			if (waiter_ > 0) {
				return false;
			} else {
				SetFightStatus();
/*
				if (!client_insight_) {
					SetFightStatus();
				} else {
					return false;
				}*/
			}
		}
		if (status_ == Packet::BattleGroundStatus::PAUSE)
			return false;
		brief_.set_fight_time(brief_.fight_time() + elapseTime);
		boost::for_each(traps_, boost::bind(&Trap::OnTick, _1, elapseTime));
		auto delete_it = boost::partition(traps_, boost::bind(&Trap::NeedDestroy, _1) == false);
		std::for_each(delete_it, traps_.end(), boost::bind(&Trap::OnDestroy, _1));
		traps_.erase(delete_it, traps_.end());
		boost::for_each(attackers_, boost::bind(&MtActor::OnTick, _1, elapseTime));
		boost::for_each(defenders_, boost::bind(&MtActor::OnTick, _1, elapseTime));
		SkillContext ctx;
		boost::for_each(attackers_, boost::bind(&MtActor::OnTimeElapseStub, _1, boost::ref(ctx)));
		boost::for_each(defenders_, boost::bind(&MtActor::OnTimeElapseStub, _1, boost::ref(ctx)));
		ProcessEvents();
		if (!new_attackers_.empty()) {
			boost::for_each(new_attackers_, boost::bind(&MtActor::EnterBattleGround, _1, this));
			boost::for_each(new_attackers_, boost::bind(&MtActor::SummonEnter, _1));
			std::copy(std::begin(new_attackers_), std::end(new_attackers_), std::inserter(attackers_, std::end(attackers_)));
			new_attackers_.clear();
		}
		if (!new_defenders_.empty()) {
			boost::for_each(new_defenders_, boost::bind(&MtActor::EnterBattleGround, _1, this));
			boost::for_each(new_defenders_, boost::bind(&MtActor::SummonEnter, _1));
			std::copy(std::begin(new_defenders_), std::end(new_defenders_), std::inserter(defenders_, std::end(defenders_)));
			new_defenders_.clear();
		}

		if (status_ == Packet::BattleGroundStatus::END) {
			return true;
		}
		try {
			winnercamp_ = thread_lua->call<int32>(script_id_, "OnCheckBattleEnd", this);
			if (winnercamp_ != 0 && status_ != Packet::BattleGroundStatus::END) {
				OnBattleEnd(false, creator_ == nullptr ? INVALID_GUID : creator_->Guid());
			}
		}
		catch (ff::lua_exception& e) {e;
			return false;
		}

		return false;
	}

	bool MtBattleGround::TimeOut() const
	{
		return brief_.has_time_out() && brief_.time_out() > 0
			&& brief_.elapse_time() >= brief_.time_out();
	}

	int32 MtBattleGround::GetAllDeadCampID()
	{
		if (std::all_of(std::begin(attackers_), std::end(attackers_), [](auto& it) {
			if (it->ActorType() != Packet::ActorType::SUMMON_NO_SELECT)
				return it->IsDead();
			else
				return true;
		}) == true) {
			return 1;
		}
		if (attackers_.empty() && defenders_.empty() == false) {
			return 1;
		}
		if (std::all_of(std::begin(defenders_), std::end(defenders_), [](auto& it) {
			if (it->ActorType() != Packet::ActorType::SUMMON_NO_SELECT)
				return it->IsDead();
			else
				return true;
		}) == true) {
			return 2;
		}
		if (attackers_.empty() == false && defenders_.empty()) {
			return 2;
		}
		return 0;
	}

	void MtBattleGround::OnBattleEnd(bool force_end, uint64 source)
	{
		if (status_ == Packet::BattleGroundStatus::END) {
			//避免组队多重end的问题
			return;
		}
		
		status_ = Packet::BattleGroundStatus::END;
		if (creator_ && Brief().type() != Packet::BattleGroundType::PVE_HOOK) {
			creator_->SetInBattle(false);
			LOG_INFO << "[battle_manager] end battle. player:"
				<< creator_->Guid() << ",battle:" << this;
		}
		if (target_ != nullptr && Brief().type() != Packet::BattleGroundType::PVE_HOOK) {
			target_->SetInBattle(false);
		}
		battle_time_ += int32(brief_.elapse_time() / 1000);
		EquipDurableCost();

		//删除召唤出来的怪物
		std::for_each(std::begin(attackers_), std::end(attackers_), boost::bind(&MtActor::OnBattleEnd, _1));
		std::for_each(std::begin(defenders_), std::end(defenders_), boost::bind(&MtActor::OnBattleEnd, _1));
		ProcessEvents();
		std::for_each(std::begin(players_), std::end(players_), boost::bind(&MtPlayer::OnBattleEnd, _1, shared_from_this(), force_end));

		if (force_end) {
			if (winnercamp_ == 0) {//还没战斗结束是强制中断战斗算输
				IsAttacker(source) ? winnercamp_ = 2 : winnercamp_ = 1;
			}
		}
		else{
			if (winnercamp_ == 2 && statistics_ != nullptr ) {
				statistics_->self_dead_times_++;
			}
		}
		//
		try {
			if (force_end == false) {
				//团队分配优先处理
				OnCheckArrange();
				thread_lua->call<bool>(script_id_, "OnBattleReward", this, winnercamp_);
			}
			thread_lua->call<bool>(script_id_, "OnBattleEnd", this, force_end, winnercamp_);
		}
		catch (ff::lua_exception& e) {
			e;
			BattleOverNotify(winnercamp_);
		}

		auto relate_npc = GetParams32("relate_npc");
		if (relate_npc >= 0) {
			creator_->LockNpc(relate_npc, false);
		}

		winnercamp_ = 0;

		//清理一下战斗数据
		ClearAttacker();
		ClearDefender();

		return;
	}

	bool MtBattleGround::OnCheckArrange()
	{		
		if (winnercamp_ != 1)
			return false;

		auto mission_id = GetParams32("mission_id");
		if (mission_id <= 0)
			return false;

		auto winners = GetAttackerPlayers();
		for (auto child : winners) {
			if (child->IsTeamLeader()) {
				return child->GetMissionProxy()->OnTeamArrangeReward(mission_id);
			}
		}	
		return false;
	}

	void MtBattleGround::BattleEnterNotify()
	{
		//通知
		Packet::EnterBattleGroundReply notify;
		FillBattleGroundInfo(*notify.mutable_info());
		notify.mutable_brief()->CopyFrom(Brief());

		BroadCastMessage(notify);		
	}

	int32 MtBattleGround::GetRefreshWaitTime() const
	{
		if (brief_.type() == Packet::BattleGroundType::PVE_HOOK) {
			auto stage_config = MtSceneStageManager::Instance().GetConfig(brief_.stage_id());
			if (stage_config) {
				int64 wait = stage_config->avg_fight_time() - brief_.fight_time();
				if (wait > 0)
					return int32(wait);
			}
		}
		return 0;
	}

	//脚本根据具体逻辑流程进行调用
	void MtBattleGround::BattleOverNotify(int32 winnercamp)
	{
		//得通知客户端
		Packet::BattleGroundOverNotify notify;
		notify.mutable_brief()->CopyFrom(this->Brief());
		notify.set_winner_camp(winnercamp);
		notify.set_time_stamp(TimeStamp());
		BroadCastMessage(notify);
	}
	
	void MtBattleGround::OnActorCopyDead(MtActor* dead_one, MtActor* killer)
	{
		killer;
		if (creator_) {
			creator_->OnActorDead(this, dead_one);
		}
	}

	void MtBattleGround::OnMonsterDead(const MonsterBattleActor& monster, MtActor* killer)
	{
		if (killer == nullptr || killer->Player().expired()
			|| monster.BattleActorType()== Packet::BattleActorType::BAT_SUMMON) {
			return;
		}
		auto player = killer->Player().lock();
		auto monster_config = MtMonsterManager::Instance().FindMonster(monster.ConfigId());
		if (monster_config != nullptr) {
			if (std::find_if(std::begin(attackers_), std::end(attackers_), [&](auto& actor) {return actor->Guid() == killer->Guid(); }) != std::end(attackers_)) {
				//掉落
				int32 monster_exp = monster_config->exp_drop();
				int32 extra_exp = 0;
				auto gold = monster_config->gold_drop();
				auto type = Config::ItemAddLogType::AddType_MonsterDrop;
				if (this->BattleType() == Packet::BattleGroundType::PVE_SIMULATE
					|| this->BattleType() == Packet::BattleGroundType::PVE_HOOK) {
					type = Config::ItemAddLogType::AddType_Hook;
				}
				if (BattleType() == Packet::BattleGroundType::PVE_HOOK && brief_.scene_id() > 0) {
					gold -= MtSceneStageManager::Instance().CollectTax(player, brief_.stage_id(), gold);
					//挂机双倍经验点效果
					if (player->DelItemById(Packet::TokenType::Token_ExpPoint, 1, Config::ItemDelLogType::DelType_UseItem, 0)) {
						extra_exp = monster_exp;
					}
				}
				player->AddItemByIdWithNotify(Packet::TokenType::Token_Gold, gold, type, monster.Guid());
				auto item = battle_drops_.add_items();
				item->set_item_id(Packet::TokenType::Token_Gold);
				item->set_item_amount(monster_config->gold_drop());
				player->AddItemByIdWithNotify(
					Packet::TokenType::Token_FormationExp,
					monster_exp, type, monster.Guid());
				player->AddItemByIdWithNotify(
					Packet::TokenType::Token_FormationExp,
					extra_exp, type, monster.Guid());
				item = battle_drops_.add_items();
				item->set_item_id(Packet::TokenType::Token_Exp);
				item->set_item_amount(monster_exp);
				battle_drops_.set_time_stamp(TimeStamp());
				if (extra_exp > 0) {
					item = battle_drops_.add_items();
					item->set_item_id(Packet::TokenType::Token_Hero_Exp);
					item->set_item_amount(extra_exp);
				}
			}
			if (BattleType() != Packet::BattleGroundType::PVP_GUILD_CAPTURE_POINT) {
				player->OnLuaFunCall_x("xOnMonsterDead",
				{
					{ "monster_id",monster_config->id() },
					{ "stage_id",brief_.stage_id() },
					{ "scene_id",brief_.scene_id() },
					{ "monster_class",monster_config->monster_class() },
					{ "battle_type",(int32)brief_.type() },
					{ "monster_count" , 1 },
				});
			}
			if (player->GetPlayerStageProxy() != nullptr
				&& BattleType() == Packet::BattleGroundType::PVE_HOOK) {
				monster_kills_++;
			}
		}
	}

	void MtBattleGround::ClearAttacker() 
	{
		std::for_each(attackers_.begin(), attackers_.end(), boost::bind(&MtActor::OnExitBattle, _1));
		attackers_.clear(); 
	}
	void MtBattleGround::ClearDefender()
	{
		std::for_each(defenders_.begin(), defenders_.end(), boost::bind(&MtActor::OnExitBattle, _1));
		defenders_.clear();
	}


/*
	bool MtBattleGround::operator==(const boost::shared_ptr<MtBattleGround>& rhs) const
	{
		return brief_.guid() == rhs->brief_.guid();
	}*/

	Packet::BattleGroundBrief& MtBattleGround::Brief()
	{
		if (scene_battle_group_ != nullptr) {
			brief_.set_battle_group_id(scene_battle_group_->index());
		}
	
		return brief_;
	}

	bool MtBattleGround::SendCommand(const boost::shared_ptr<BattleCommand> command)
	{
		commands_.push_back(command);
		return true;
	}

	void MtBattleGround::BroadCastMessage(const google::protobuf::Message& message)
	{
		LogMessage(message);
		if (client_insight_ == false) {
			return;
		}
		boost::for_each(players_, boost::bind(&MtPlayer::SendMessage, _1, boost::cref(message)));
	}

	void MtBattleGround::LogMessage(const google::protobuf::Message& msg)
	{
		if (reply_ == nullptr)
			return;
		if (msg.GetDescriptor()->name() == "EnterBattleGroundReply") {
			reply_->mutable_enter_msg()->CopyFrom(msg);
		}
		buffer buf;
		protobuf_codec::protobuf_codec::fill_empty_buffer(false, &buf, msg);
		reply_->mutable_messages()->mutable_data()->append(buf.retrieve_all_as_string());
	}
	void MtBattleGround::FillBattleGroundInfo(Packet::BattleGroundInfo& info)
	{
		info.mutable_brief()->CopyFrom(this->Brief());
		for (auto& actor : attackers_) {
			Packet::ActorBattleInfo& message = *info.add_attackers();
			actor->SerializationToBattleInfo(message);
		}
		for (auto& actor : defenders_) {
			Packet::ActorBattleInfo& message = *info.add_defenders();
			actor->SerializationToBattleInfo(message);
		}
	}
	
	void MtBattleGround::AddPlayer(const boost::shared_ptr<MtPlayer>& player)
	{
		if (std::any_of(std::begin(players_), std::end(players_), boost::bind(&MtPlayer::Guid, _1) == player->Guid()) == false)
		{
			players_.push_back(player);
		}
	}

	void MtBattleGround::AddActor(uint64 teammate_guid,
		const boost::shared_ptr<MtActor>& newbie)
	{
		if (std::any_of(std::begin(attackers_), std::end(attackers_), 
			boost::bind(&MtActor::Guid, _1) == teammate_guid)) {
			AddActor(0, newbie);

		} else if (std::any_of(std::begin(defenders_), std::end(defenders_),
			boost::bind(&MtActor::Guid, _1) == teammate_guid)) {
			AddActor(1, newbie);

		} else {
			ZXERO_ASSERT(false) << "invalid side";
		}
	}

	void MtBattleGround::AddActor(int32 side, const boost::shared_ptr<MtActor>& newbie)
	{
		Packet::NewActorCreated msg;
		msg.set_creator_guid(INVALID_GUID);
		ZXERO_ASSERT(newbie != nullptr) << "invalid newbie";
		newbie->SetSide(side);
		if (side == 0) {
			new_attackers_.push_back(newbie);
			newbie->SerializationToBattleInfo(*msg.add_attackers());
		} else if (side == 1) {
			new_defenders_.push_back(newbie);
			newbie->SerializationToBattleInfo(*msg.add_defenders());
		}
		msg.set_time_stamp(TimeStamp());
		BroadCastMessage(msg);
		LOG_INFO << "Create New actor guid " << newbie->Guid();
	}

	void MtBattleGround::AddTrap(const boost::shared_ptr<Trap>& trap)
	{
		if (traps_.size() <= 20) {
			traps_.push_back(trap);
		}
		else {
			LOG_INFO << "trap size got: " << traps_.size() << " can not add any more";
		}
	}

	void MtBattleGround::EquipDurableCost()
	{
		EnumAllActor([](auto& actor) { actor->BattleDurableCost(); });
	}

	//Packet::ActorFigthFormation MtBattleGroundManager::BattleType2Formation(const Packet::BattleGroundType type) {
	//	if (type == Packet::BattleGroundType::PVE_HOOK
	//		|| type == Packet::BattleGroundType::PVE_STAGE_BOSS
	//		|| type == Packet::BattleGroundType::PVE_SINGLE) {
	//		return Packet::ActorFigthFormation::AFF_NORMAL_HOOK;
	//	}
	//	else if (type == Packet::BattleGroundType::PVP_PVE_CAPTURE_STAGE) {
	//		return Packet::ActorFigthFormation::AFF_STAGE_CAPTURE;
	//	}
	//	else if (type == Packet::BattleGroundType::PVP_LOOT) {
	//		return Packet::ActorFigthFormation::AFF_PVP_LOOT_FORMATION;
	//	}
	//	else  if (type == Packet::BattleGroundType::PVP_ARENA) {
	//		return Packet::ActorFigthFormation::AFF_PVP_ARENA;
	//	}
	//	else if (type == Packet::BattleGroundType::PVP_TRIAL_FIELD) {
	//		return Packet::ActorFigthFormation::AFF_PVP_TRIAL_FIELD;
	//	}
	//	else if (type == Packet::BattleGroundType::PVE_GUILD_BOSS) {
	//		return Packet::ActorFigthFormation::AFF_PVE_GUILD_BOSS;
	//	}
	//	else if (type == Packet::BattleGroundType::PVP_WORLDBOSS_BATTLE){
	//		int32 weekday = MtTimerManager::Instance().GetWeekDay();
	//		return (Packet::ActorFigthFormation)(Packet::ActorFigthFormation::AFF_PVP_WORLDBOSS_Sun + weekday);
	//	}
	//	else if (type == Packet::BattleGroundType::PVP_GUILD_BATTLE){
	//		return Packet::ActorFigthFormation::AFF_PVE_GUILD_BOSS;
	//	}
	//	else if (type == Packet::BattleGroundType::PVE_GUILD_COPY) {
	//		return Packet::ActorFigthFormation::AFF_PVE_GUILD_COPY;
	//	}
	//	else if (type == Packet::BattleGroundType::PVP_BATTEL_FIELD) {
	//		return Packet::ActorFigthFormation::AFF_PVP_BATTLE_FIELD;
	//	}
	//	else {
	//		return Packet::ActorFigthFormation::AFF_NORMAL_HOOK;
	//	}
	//}
 //	const boost::shared_ptr<MtBattleGround> MtBattleGroundManager::CreateBattle(const MtBattleGroundIniter& initer)
	//{
	//	////阵容有效性检查
	//	//auto formation = MtBattleGroundManager::BattleType2Formation(initer.type);
	//	//if (initer.creator == nullptr) {
	//	//	return nullptr;
	//	//}
	//	//if (initer.creator->IsEmptyFormation(formation)) {
	//	//	return nullptr;
	//	//}

	//	////重复战斗类型检查,防止客户端大量发同种战斗类型包
	//	//auto battle = initer.creator->BattleGround();
	//	//if (battle != nullptr && battle->BattleType() == initer.type) {
	//	//	return nullptr;
	//	//}

	//	//auto new_battle = boost::make_shared<MtBattleGround>(initer.creator,initer.target, initer.scene,initer.scene_battle_group, initer.cdata);
	//	//new_battle->SetType(initer.type);
	//	//new_battle->Init();
	//	//new_battle->EnumAllActor(boost::bind(&MtActor::EnterBattleGround, _1, new_battle.get()));;
	//	//return new_battle;

	//	return CreatePureBattle();
	//}
	
	/*const boost::shared_ptr<MtBattleGround> MtBattleGroundManager::CreatePvpBattle(MtPlayer * player, MtPlayer * target, std::map<std::string, int32> params)
	{
		if (player == nullptr) {
			return nullptr;
		}
		if (target != nullptr) {
			LOG_INFO << "[battle_manager] CreatePvpBattle player:"
				<< player->Guid() << " target:" << target->Guid() << " params:" << params;
		} else {
				LOG_INFO << "[battle_manager] CreatePvpBattle player:"
					<< player->Guid() << " params:" << params;
		}
		auto iter = params.find("script_id");
		if (iter == params.end()) {
			return nullptr;
		}
		auto script_id = iter->second;
		//战斗重复性检测
		auto hook_battle = player->HookBattle();
		if (hook_battle) {
			hook_battle->OnBattleEnd(true, player->Guid());
		}
		auto old_battle = player->BattleGround();
		if (old_battle != nullptr) {
			if (old_battle->GetScriptId() == script_id) {
				return nullptr;
			}
			player->QuitBattle();
		}

		auto battle = boost::make_shared<MtBattleGround>(player,target);
		auto time_out_iter = params.find("time_out");
		if (time_out_iter == params.end()) {
			params["time_out"] = 60 * 3 * 1000;
			battle->Brief().set_time_out(params["time_out"]);
		}
		try {
			auto ret= thread_lua->call<int32>(script_id, "OnGoBattle", battle, params);
			if (ret != Packet::ResultCode::ResultOK) {
				return nullptr;
			}
		}
		catch (ff::lua_exception& e) {
			e;
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::LuaScriptException);
			return nullptr;
		}

		battles_.push_back(battle);
		if (target != nullptr) {
			LOG_INFO << "[battle_manager] CreatePvpBattle player:"
				<< player->Guid() << " target:" << target->Guid()
				<< " params:" << params << " success";
		} else {
			if (target != nullptr) {
				LOG_INFO << "[battle_manager] CreatePvpBattle player:"
					<< player->Guid() << " params:" << params << " success";
			}
		}
		return battle;
	}*/
	const boost::shared_ptr<MtBattleGround> MtBattleGroundManager::CreateHookBattle(
		MtPlayer* player, std::map<std::string, int32> params)
	{
		if (player == nullptr) {
			return nullptr;
		}
		LOG_INFO << "[battle_manager] CreateHookBattle player:"
			<< player->Guid() << " params:" << params;
		auto script_iter = params.find("script_id");
		auto type_iter = params.find("battle_type");
		if (script_iter == params.end() || type_iter == params.end()
			|| params.at("battle_type") != Packet::BattleGroundType::PVE_HOOK) {
			LOG_ERROR << "[battle_manager] CreateHookBattle player:" <<
				player->Guid() << " without script_id or battle_type";
			return nullptr;
		}
		auto script_id = script_iter->second;
		auto hook_battle = player->HookBattle();
		if (hook_battle != nullptr) {
			LOG_ERROR << "[battle_manager] CreateHookBattle player:" <<
				player->Guid() << " old hook battle exist:" << hook_battle;
			return nullptr;
		}
		auto battle = boost::make_shared<MtBattleGround>(player, nullptr);
		auto time_out_iter = params.find("time_out");
		if (time_out_iter == params.end()) {
			params["time_out"] = 60 * 3 * 1000;
			battle->Brief().set_time_out(params["time_out"]);
		}
		LOG_INFO << "[battle_manager] CreateHookBattle player:" << player->Guid()
			<< " battle:" << battle << " created";
		try {
			auto ret = thread_lua->call<int32>(script_id, "OnGoBattle", battle, params);
			if (ret != Packet::ResultCode::ResultOK) {
				LOG_INFO << "[battle_manager] CreateHookBattle player:"
					<< player->Guid() << " battle:" << battle << " fail1:" << ret;
				return nullptr;
			}
		}
		catch (ff::lua_exception& e) {
			e;
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::LuaScriptException);
			LOG_INFO << "[battle_manager] CreateHookBattle player:"
				<< player->Guid() << " battle:" << battle << " fail2:" << e.what();
			return nullptr;
		}

		battles_.push_back(battle);
		LOG_INFO << "[battle_manager] CreateHookBattle player:"
			<< player->Guid() << " params:" << params << " battle" << battle << " success";
		return battle;

	}
	const boost::shared_ptr<MtBattleGround> MtBattleGroundManager::CreateFrontBattle(
		MtPlayer* player, MtPlayer* target, std::map<std::string, int32> params)
	{
		if (player == nullptr) {
			return nullptr;
		}
		if (target != nullptr) {
			LOG_INFO << "[battle_manager] CreateFrontBattle player:"
				<< player->Guid() << " target:" << target->Guid() << " params:" << params;
		} else {
			LOG_INFO << "[battle_manager] CreateFrontBattle player:"
				<< player->Guid() << " params:" << params;
		}
		auto iter = params.find("script_id");
		if (iter == params.end()) {
			LOG_ERROR << "[battle_manager] CreateFrontBattle player:" <<
				player->Guid() << " without script_id";
			return nullptr;
		}
		if (params.find("battle_type") != params.end()
			&& params.at("battle_type") == Packet::BattleGroundType::PVE_HOOK)
		{
			LOG_ERROR << "[battle_manager] CreateFrontBattle player:" <<
				player->Guid() << " pve_hook type";
			return nullptr;
		}
		auto script_id = iter->second;
		auto hook_battle = player->HookBattle();
		if (hook_battle != nullptr ) {
			if (hook_battle->GetClientInsight()) {
				LOG_INFO << "[battle_manager] CreateFrontBattle, player:" << player->Guid()
					<< ",create front battle, hide hook battle";
				hook_battle->SetClientInsight(false);
				player->SendCommonResult(Packet::ResultOption::OnHook_Opt, Packet::ResultCode::HookEndOK);
				player->SetOnHook(false);
			}
		}
		auto old_battle = player->BattleGround().get();
		if (old_battle != nullptr ) {
			if (old_battle->GetScriptId() == script_id) {
				LOG_ERROR << "[battle_manager] CreateFrontBattle player:"
					<< player->Guid() << " battle:" << old_battle
					<< " fail0, same script_id:" << script_id;
				return nullptr;
			}
			LOG_INFO << "[battle_manager] CreateFrontBattle player:"
				<< player->Guid() << " quit old front battle:"
				<< old_battle << "script:" << old_battle->GetScriptId();
			player->QuitBattle();
		}

		auto battle = boost::make_shared<MtBattleGround>(player, target);
		auto time_out_iter = params.find("time_out");
		if (time_out_iter == params.end()) {
			params["time_out"] = 60 * 3 * 1000;
			battle->Brief().set_time_out(params["time_out"]);
		}
		LOG_INFO << "[battle_manager] CreateFrontBattle player:" << player->Guid()
			<< " battle:" << battle << " created";
		try {
			auto ret = thread_lua->call<int32>(script_id, "OnGoBattle", battle, params);
			if (ret != Packet::ResultCode::ResultOK) {
				LOG_INFO << "[battle_manager] CreateFrontBattle player:"
					<< player->Guid() << " battle:" << battle << " fail1:" << ret;
				player->SendClientNotify("BattleCreateFailed", -1, -1);
				return nullptr;
			}
		}
		catch (ff::lua_exception& e) {
			e;
			player->SendCommonResult(Packet::ResultOption::BATTLE, Packet::ResultCode::LuaScriptException);
			LOG_INFO << "[battle_manager] CreateFrontBattle player:"
				<< player->Guid() << " battle:" << battle << " fail2:" << e.what();
			return nullptr;
		}

		battles_.push_back(battle);		
		LOG_INFO << "[battle_manager] CreateFrontBattle player:"
			<< player->Guid() << " params:" << params << " battle" << battle << " success";
		return battle;
	}

	const boost::shared_ptr<MtBattleGround> MtBattleGroundManager::CreatePureBattle()
	{
		auto battle = boost::make_shared<MtBattleGround>();
		LOG_INFO << "[battle_manager] CreatePureBattle" << battle;
		battles_.push_back(battle);
		return battle;
	}

	void MtBattleGroundManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtBattleGroundManager, ctor()>(ls, "MtBattleGroundManager")
			.def(&MtBattleGroundManager::CreateHookBattle, "CreateHookBattle")
			.def(&MtBattleGroundManager::CreateFrontBattle, "CreateFrontBattle")
			.def(&MtBattleGroundManager::CreatePureBattle, "CreatePureBattle")
			;
		//ff::fflua_register_t<>(ls)
		//	.def(&MtBattleGroundManager::get_mutable_instance, "LuaMtBattleGroundManager");
	}
	
	void MtBattleGroundManager::OnTick(uint64 elapseTime)
	{
		for (auto iter = battles_.begin(); iter != battles_.end(); ) {
			if ((*iter)->GetStatus() == Packet::BattleGroundStatus::END) {
				if ((*iter)->GetCreator() != nullptr) {
					LOG_INFO << "[battle_manager] Delete battle player:"
						<< (*iter)->GetCreator()->Guid() << " battle:" << (*iter);
				}
				iter = battles_.erase(iter);
			}
			else {
				(*iter)->OnTick(elapseTime);
				iter++;
			}			
		}
	}

	/////////////////////////////////////////////////////////////

	void MtBattleGround::BattleGroupDrop(const int32 monster_group_id )
	{
		if (attackers_.size() < 1)
			return;

		std::vector<boost::shared_ptr<MtPlayer>> players;
		for (auto child : attackers_) {
			bool same = false;
			for (auto iter : players) {
				auto child_player = child->Player().lock();
				if (iter == child_player) {
					same = true;
				}
			}
			if (!same && !child->Player().expired()) {
				players.push_back(child->Player().lock());
			}
		}

		auto monster_group = MtMonsterManager::Instance().FindMonsterGroup(monster_group_id);
		if (monster_group) {			
			for (auto p : players) {
				auto mission_proxy = p->GetMissionProxy();
				if (mission_proxy == nullptr) {
					continue;
				}
				if (monster_group->mission_id() >= 0 && !mission_proxy->MissionIdCheck(monster_group->mission_id())) {
					//有关联任务的玩家才能获得掉落包
					continue;
				}
				Packet::NotifyItemList notify;
				auto drop_packages = MtMonsterManager::Instance().MonsterGroupRandomDrop(monster_group_id);
				bool is_hook = (BattleType() == Packet::BattleGroundType::PVE_HOOK || BattleType() == Packet::BattleGroundType::PVE_SIMULATE);
				for (auto& package : drop_packages)
				{
					Packet::StatisticInfo info;
					auto tax_collector = [&](int gold) {
						if (is_hook)
							return MtSceneStageManager::Instance().CollectTax(p, brief_.stage_id(), gold);
						else
							return 0;
					};
					if (package->AddToPlayer(p.get(), &info, &notify, tax_collector, true,
						is_hook?Config::ItemAddLogType::AddType_Hook:Config::ItemAddLogType::AddType_Package)){
					}
				}
				if (notify.item_size() > 0) {
					p->SendMessage(notify);
				}
			}
		}
	}

	int32 MtBattleGround::GetStatisticsDamage()
	{
		return std::accumulate(stat_.damages().begin(),
			stat_.damages().end(), 0, 
			[](auto init, auto& actor_damage) {return init + actor_damage.value();});
	}

	void MtBattleGround::UpdateBattleEarning()
	{
		if (BattleType() == Packet::BattleGroundType::PVE_HOOK
			&& creator_ != nullptr && creator_->GetPlayerStageProxy() != nullptr) {
			auto proxy = creator_->GetPlayerStageProxy();
			for (auto& item : battle_drops_.items()) {
				proxy->OnItemDrop(item.item_id(), item.item_amount());
			}
			proxy->OnMonsterDead(monster_kills_);
		}
	}
	
	bool MtBattleGround::IsAttacker(uint64 guid)
	{
		if (std::find_if(std::begin(attackers_), std::end(attackers_), [&](auto& actor) {return actor->Guid() == guid; }) != std::end(attackers_)) {
			return true;
		}
		return false;
	}

	void MtBattleGround::OnCure(Packet::ActorOnCure& cure_info)
	{
		try {
			thread_lua->call<int32>(1026, "OnCure", this,
				IsAttacker(cure_info.source_guid()),
				cure_info.source_guid(), cure_info.target_guid(), cure_info.cure());
		}
		catch (ff::lua_exception& e) {
			e;
		}

	}
	void MtBattleGround::OnDamageRefix(Packet::ActorOnDamage& damage_info, MtActor* killer)
	{
		//脚本
		try {
			thread_lua->call<void>(script_id_, "OnDamageRefix",this,&damage_info, killer);
		}
		catch (ff::lua_exception& e) {
			e;
		}
	}
	void MtBattleGround::OnDamage(const Packet::ActorOnDamage& damage_info)
	{
		uint64 source_guid = damage_info.source_guid();
		uint64 target_guid = damage_info.target_guid();
		int32 damage = damage_info.damage();
		bool attacker = IsAttacker(source_guid);
		try {
			thread_lua->call<int32>(script_id_, "OnDamage", this, attacker, source_guid, target_guid, damage);
		}
		catch (ff::lua_exception& e) {
			e;
		}
		try {
			thread_lua->call<int32>(1026, "OnDamage", this, attacker, source_guid, target_guid, damage);
		}
		catch (ff::lua_exception& e) {
			e;
		}
		
	}

	std::vector<MtActor*> MtBattleGround::AllActors(ActorSelector selector /*= ActorSelector()*/)
	{
		if (selector.empty()) {
			selector = [](const boost::shared_ptr<MtActor>& /*actor*/) {return true; };
		}
		std::vector<MtActor*> result;
		result.reserve(attackers_.size() + defenders_.size());
		std::for_each(attackers_.begin(), attackers_.end(),
			[&](auto& actor) { if (selector(actor)) result.push_back(actor.get()); });
		std::for_each(defenders_.begin(), defenders_.end(),
			[&](auto& actor) { if (selector(actor)) result.push_back(actor.get()); });
		return result;
	}

	void MtBattleGround::OnPlayerReachPos()
	{
		if (status_ == Packet::BattleGroundStatus::WAIT)
		{
			fight_start_time_ = MtTimerManager::Instance().CurrentTime();
			SetFightStatus();
		}
	}

	void MtBattleGround::PlayerRaidStage()
	{
/*
		auto time_cost = MtTimerManager::Instance().CurrentTime() - fight_start_time_;
		auto formation_score = std::accumulate(attackers_.begin(), attackers_.end(),
			0, [](int32 sum, auto& actor) {return sum + actor->Score(); });*/
		creator_->GetPlayerStageProxy()->OnPlayerRaidStage(brief_.stage_id());
	}

	void MtBattleGround::CreateDefenderMonster(const std::map<int32, int32> monster_info)
	{
		int32 creator_level = 1;
		if (creator_ != nullptr) creator_level = creator_->PlayerLevel();
		if (creator_ != nullptr && creator_->GetTeamID() != INVALID_GUID) {
			int32 team_level = creator_->TeamLevel();
			if (team_level > 0) {
				creator_level = team_level;
			}
		}
		ClearDefender();

		for (auto child : monster_info) {
			auto monster_config = MtMonsterManager::Instance().FindMonster(child.second);
			if (monster_config)	{
				auto level = monster_config->level();
				auto monster = boost::make_shared<MonsterBattleActor>(monster_config, level<0? creator_level : level);
				monster->InitBattleInfo(monster_config);
				monster->SetPositionIndex(child.first);
				defenders_.push_back(monster);
			}
		}
	}

	bool MtBattleGround::CleanDeadBattleActors()//重置一下参展角色[剔除初始血量为0的角色]
	{
		//
		for (auto iter = attackers_.begin(); iter != attackers_.end();) {
			if ((*iter)->Hp() <= 0 || (*iter)->IsDead()) {
				iter = attackers_.erase(iter);
			}
			else {
				++iter;
			}
		}

		//
		for (auto iter = defenders_.begin(); iter != defenders_.end();) {
			if ((*iter)->Hp() <= 0 || (*iter)->IsDead()) {
				iter = defenders_.erase(iter);
			}
			else {
				++iter;
			}
		}
		return true;
	}

	void MtBattleGround::CheckLackeysBuff(std::vector<boost::shared_ptr<MtActor>> &actor_list)
	{
		for (auto child : actor_list) {
			auto player = child->Player().lock();
			if (player != nullptr) {
				auto impacts = player->GetSimpleImpactSet();
				if (impacts != nullptr) {
					//PlayerLackeysImpact
					impacts->AddSimpleImpact(1016, 999999999, 0, 0, false);
				}
			}
		}
	}
	bool MtBattleGround::CreateAttackerPlayerBattleActors(bool is_team, bool is_main_actor, Packet::ActorFigthFormation form)
	{
		if (creator_ == nullptr) {
			return false;
		}
		
		std::vector<boost::shared_ptr<MtActor>> temp;
		if (is_team) {
			if (is_main_actor) {
				temp = creator_->TeamMainBattleActor();

				//team main 类型的战斗，都会拥有仆从守护效果
				CheckLackeysBuff(temp);
			}
			else {
				temp = creator_->TeamFormBattleActor(form);
			}
		}
		else {
			if (is_main_actor) {
				temp.push_back(creator_->MainActor());
			}
			else {
				temp = creator_->BattleActor(form);
			}
		}

		ClearAttacker();
		attackers_.swap(temp);
		TransformAttackers2BattleActor();	
		return true;
	}
	
	bool MtBattleGround::CreateDefenderPlayerBattleActors(bool is_team, bool is_main_actor, Packet::ActorFigthFormation form)
	{
		if (target_ == nullptr) {
			return false;
		}

		std::vector<boost::shared_ptr<MtActor>> temp;
		if (is_team) {
			if (is_main_actor) {
				temp = target_->TeamMainBattleActor();
			}
			else {
				temp = target_->TeamFormBattleActor(form);
			}
		}
		else {
			if (is_main_actor) {
				temp.push_back(target_->MainActor());
			}
			else {
				temp = target_->BattleActor(form);
			}
		}
		if (temp.empty())
		{
			return false;
		}
		ClearDefender();
		defenders_.swap(temp);
		TransformDefenders2BattleActor();		
		return true;
	}

	bool MtBattleGround::CreateDefenderFromStageCaptain()
	{
		int32 stage_id;
		std::vector<Packet::ActorFullInfo> actors;
		std::tie(stage_id, actors) = creator_->GetPlayerStageProxy()->GetCaptureStageTempInfo();
		if (stage_id == Brief().stage_id() && actors.empty() == false) {
			ClearDefender();
			std::transform(actors.begin(), actors.end(), std::back_inserter(defenders_), [](auto& info) {
				return MtActor::CreateBattleCopyFromMsg(info);
			});
			return true;
		}
		return false;
	}

	bool MtBattleGround::CreateDefenderFromArena()
	{
		ClearDefender();

		auto user = creator_->GetArenaTarget();
		if (user == nullptr) {
			return false;
		}
				
		if (user->user_->is_robot()) {
			std::vector<Packet::ActorFullInfo> defenders;
			Packet::ArenaPlayer arena_robot_player;
			if (MtArenaManager::Instance().ArenaRobot2ArenaPlayer(*(user->user_), arena_robot_player)) {
				for (int i = 0; i < arena_robot_player.actors_size(); ++i) {
					defenders.push_back(arena_robot_player.actors(i));
				}
			}
			if (defenders.size() <= 0) {
				return false;
			}

			std::transform(std::begin(defenders), std::end(defenders), std::back_inserter(defenders_), [](auto& info) {
				return MtActor::CreateBattleCopyFromMsg(info);
			});
		}
		else {
			auto target_player = Server::Instance().FindPlayer(user->user_->player_guid());
			if (target_player == nullptr) {
				return false;
			}
			auto defenders = target_player->BattleActor(Packet::ActorFigthFormation::AFF_PVP_GUARD);
			if (defenders.size() <= 0) {
				return false;
			}
			std::vector<Packet::ActorFullInfo> defender_as_msgs;
			defender_as_msgs.reserve(defenders.size());
			for (std::size_t i = 0; i < defenders.size(); ++i) {
				Packet::ActorFullInfo msg;
				defenders[i]->SerializationToMessage(msg);
				defender_as_msgs.push_back(msg);
			}
			std::transform(std::begin(defender_as_msgs), std::end(defender_as_msgs), std::back_inserter(defenders_), [](auto& info) {
				return MtActor::CreateBattleCopyFromMsg(info);
			});
		}
		boost::for_each(defenders_, boost::bind(&MtActor::SetPlayer, _1, boost::weak_ptr<MtPlayer>()));
		return true;
	}

	bool MtBattleGround::CreateDefenderFromHell()
	{

		ClearDefender();

		auto target = creator_->GetPlayerNpc(creator_->GetParams64("target_guid"));
		if (target == nullptr) {
			return false;
		}

		std::vector<Packet::ActorFullInfo> defenders;
		for (int32 k = 0; k < target->actors_size(); k++) {
			defenders.push_back(target->actors(k));
		}
		if (defenders.size()<=0) {
			return false;
		}
		
		std::transform(std::begin(defenders), std::end(defenders), std::back_inserter(defenders_), [](auto& info) {
			return MtActor::CreateBattleCopyFromMsg(info);
		});
		return true;
	}

	

	std::vector<boost::shared_ptr<MtPlayer>> MtBattleGround::GetDefenderPlayers() {
		std::vector<boost::shared_ptr<MtPlayer>> players;
		for (auto child : defenders_) {
			auto p = child->Player().lock();
			if (p != nullptr) {
				if (std::find(players.begin(), players.end(), p) == players.end()) {
					players.push_back(p);
				}
			}
		}
		return players;
	}

	std::vector<boost::shared_ptr<MtPlayer>> MtBattleGround::GetAttackerPlayers() {
		std::vector<boost::shared_ptr<MtPlayer>> players;
		for (auto child : attackers_) {
			auto p = child->Player().lock();
			if (p != nullptr) {
				if (std::find(players.begin(), players.end(), p) == players.end()) {
					players.push_back(p);
				}				
			}
		}
		return players;
	}

	void MtBattleGround::PushHookStatisticsToStageManager()
	{
		if (statistics_ == nullptr) {
			return;
		}
		auto income_total = 0;
		auto& items = statistics_->items_;
		if (items.find(Packet::Token_Gold) != std::end(items)) {
			income_total = items[Packet::Token_Gold];
		}
/*
		MtSceneStageManager::Instance().UpdateSceneStageOnHookRank(
			brief_.stage_id(), creator_->Guid(), creator_->Name(), 
			battle_time_, int32(income_total / (battle_time_ / 60.f)));*/
	}


	Packet::Position MtBattleGround::RelativePosition(uint64 actor_guid,
		int32 index, bool opponent /*= false*/)
	{
		bool left_side = true;
		if (std::any_of(attackers_.begin(), attackers_.end(),
			boost::bind(&MtActor::Guid, _1) == actor_guid) == true) {
			if (opponent == true) left_side = false;
		} else {
			if (opponent == false) left_side = false;
		}
		if (left_side) {
			return scene_battle_group_->player_pos(index);
		} else {
			return scene_battle_group_->enemy_pos(index);
		}
	}

	bool MtBattleGround::IsTeamMate(const MtActor& lhs, const MtActor& rhs)
	{
		if (std::any_of(attackers_.begin(),attackers_.end(),
			[&](auto& actor) {return actor->Guid() == lhs.Guid(); })
			&& std::any_of(attackers_.begin(), attackers_.end(), [&](auto& actor) {return actor->Guid() == rhs.Guid(); })
			)
			return true;
		if (std::any_of(defenders_.begin(), defenders_.end(),
			[&](auto& actor) {return actor->Guid() == lhs.Guid(); })
			&& std::any_of(defenders_.begin(), defenders_.end(), [&](auto& actor) {return actor->Guid() == rhs.Guid(); })
			)
			return true;
		return false;
	}

	void MtBattleGround::PushEvent(const boost::shared_ptr<Event>& e)
	{
		e->bg_ = weak_from_this();
		events_.push_back(e);
	}


	zxero::int32 MtBattleGround::GenMoveSerial()
	{
		return ++move_serial_;
	}

	bool MtBattleGround::MoveByClient()
	{
		if (creator_)
			return !creator_->IsDisConnected();
		return false;
	}

	void MtBattleGround::ProcessEvents()
	{
		decltype(events_) tmp = events_;
		events_.clear();
		std::for_each(tmp.begin(), tmp.end(), boost::bind(&Event::HandleEvent, _1));
	}

	void MtBattleGround::AddMonster(int32 side, int32 config_id, int32 pos_index /*= 0*/)
	{
		if (scene_battle_group_ == nullptr) {
			ZXERO_ASSERT(false) << "scene_battle_group_ is null!";
			return;
		}

		auto config = MtMonsterManager::Instance().FindMonster(config_id);
		if (config == nullptr) return;
		auto actor = boost::make_shared<MonsterBattleActor>(config,1);
		actor->InitBattleInfo(config);
		actor->SetPositionIndex(pos_index);
		if (side == 0) {
			auto pos = scene_battle_group_->player_pos(0);
			if (actor->GetPositionIndex() < scene_battle_group_->player_pos_size()) {
				pos = scene_battle_group_->player_pos(actor->GetPositionIndex());
			}
			actor->Position(pos);
		} else if (side == 1) {
			auto pos = scene_battle_group_->enemy_pos(0);
			if (actor->GetPositionIndex() < scene_battle_group_->enemy_pos_size()) {
				pos = scene_battle_group_->enemy_pos(actor->GetPositionIndex());
			}
		} else {
			ZXERO_ASSERT(false) << "add monster." << side << "," << config_id;
		}
		AddActor(side, actor);
	}

	void MtBattleGround::DelActor(uint64 guid)
	{
		auto delete_event = boost::make_shared<DeleteActorEvent>();
		delete_event->guids_ = { guid };
		PushEvent(delete_event);
	}

	void MtBattleGround::DeleteActorNow(uint64 guid)
	{
		auto it = std::find_if(attackers_.begin(), attackers_.end(), boost::bind(&MtActor::Guid, _1) == guid);
		if (it != attackers_.end()) {
			auto actor = *it;			
			Packet::ActorDismiss msg;
			msg.set_guid(actor->Guid());
			msg.set_time_stamp(TimeStamp());
			BroadCastMessage(msg);
			attackers_.erase(it);

			LOG_INFO << "actor dismiss:" << actor->Guid();
		}
		it = std::find_if(defenders_.begin(), defenders_.end(), boost::bind(&MtActor::Guid, _1) == guid);
		if (it != defenders_.end()) {
			auto actor = *it;			
			Packet::ActorDismiss msg;
			msg.set_guid(actor->Guid());
			msg.set_time_stamp(TimeStamp());
			BroadCastMessage(msg);
			defenders_.erase(it);

			LOG_INFO << "actor dismiss:" << actor->Guid();
		}
	}

	uint64 MtBattleGround::CreateBattleReply()
	{
		reply_ = boost::make_shared<Packet::BattleReply>();
		reply_->set_guid(MtGuid::Instance()(*reply_));
		reply_->set_type(BattleType());
		reply_->mutable_messages();
		reply_->mutable_stat();
		reply_->set_scene_id(brief_.scene_id());
		reply_->set_battle_group_id(brief_.battle_group_id());
		if (creator_ && creator_->Scene() != nullptr) {
			reply_->set_scene_id(creator_->GetSceneId());
		}
		if (scene_battle_group_) {
			reply_->set_battle_group_id(scene_battle_group_->id());
		}
		reply_->set_create_time(int32(MtTimerManager::Instance().CurrentTime() / 1000));
		return 0;
	}

	Packet::BattleReply* MtBattleGround::GetBattleReply()
	{
		return reply_.get();
	}

	void MtBattleGround::InitAttackerFromActorWithoutPlayer(MtActor* actor)
	{
		auto new_actor = actor->CreateBattleCopy();
		new_actor->SetPlayer(boost::weak_ptr<MtPlayer>());
		attackers_.push_back(new_actor);
	}

	void MtBattleGround::InitDefenderFromActorWithoutPlayer(MtActor* actor)
	{
		auto new_actor = actor->CreateBattleCopy();
		new_actor->SetPlayer(boost::weak_ptr<MtPlayer>());
		defenders_.push_back(new_actor);
	}

	void MtBattleGround::TransformAttackers2BattleActor()
	{
		decltype(attackers_) temp;
		temp.swap(attackers_);
		std::transform(std::begin(temp),
			std::end(temp),
			std::back_inserter(attackers_),
			boost::bind(&MtActor::CreateBattleCopy, _1));
	}

	void MtBattleGround::TransformDefenders2BattleActor()
	{
		decltype(attackers_) temp;
		temp.swap(defenders_);
		std::transform(std::begin(temp),
			std::end(temp),
			std::back_inserter(defenders_),
			boost::bind(&MtActor::CreateBattleCopy, _1));
	}

	MtActor* MtBattleGround::FindActor(uint64 guid)
	{
		auto it = std::find_if(attackers_.begin(), attackers_.end(), boost::bind(&MtActor::Guid, _1) == guid);
		if (it != attackers_.end()) {
			return (*it).get();
		}
		it = std::find_if(defenders_.begin(), defenders_.end(), boost::bind(&MtActor::Guid, _1) == guid);
		if (it != defenders_.end()) {
			return (*it).get();
		}
		return nullptr;
	}

	void MtBattleGround::SetHookPause(bool pause)
	{
		sim_pause_ = pause;
	}

	bool MtBattleGround::AddStat2Reply()
	{
		if (reply_ == nullptr) {
			return false;
		}
		for (auto& stat : (*stat_.mutable_damages())) {
			auto actor = FindActor(stat.guid());
			if (actor != nullptr) {
				stat.set_config_id(actor->ConfigId());
			}
		}
		for (auto& stat : (*stat_.mutable_be_damages())) {
			auto actor = FindActor(stat.guid());
			if (actor != nullptr) {
				stat.set_config_id(actor->ConfigId());
			}
		}
		for (auto& stat : (*stat_.mutable_cures())) {
			auto actor = FindActor(stat.guid());
			if (actor != nullptr) {
				stat.set_config_id(actor->ConfigId());
			}
		}
		reply_->mutable_stat()->CopyFrom(stat_);
		return true;
	}

	void MtBattleGround::SetClientInsight(bool insight)
	{
		client_insight_ = insight;
	}

	void MtBattleGround::SaveBattleReply() const
	{
		if (reply_ != nullptr && reply_->IsInitialized()) {
			auto msg = boost::make_shared<S2G::SaveBattleReply>();
			msg->mutable_reply()->CopyFrom(*reply_);
			Server::Instance().SendMessage(msg, nullptr);
		}
	}

	void MtBattleGround::SetFightStatus()
	{
		status_ = Packet::BattleGroundStatus::FIGHTING;
		brief_.set_fight_time(0);
		if (creator_) {
			LOG_INFO << "[battle] battle start, creator:" << creator_->Guid()
				<< ",type" << Brief().type() << "," << this;
			Packet::BattleStartNotify notify;
			notify.mutable_brief()->CopyFrom(Brief());
			BroadCastMessage(notify);
			if (Brief().type() != Packet::BattleGroundType::PVE_HOOK) {
				creator_->SetInBattle(true);
				if (target_ != nullptr) {
					target_->SetInBattle(true);
				}
			}
		}
		SkillContext ctx;
		std::for_each(attackers_.begin(), attackers_.end(),
			boost::bind(&MtActor::AfterBattleStartStub, _1, boost::ref(ctx)));
		std::for_each(defenders_.begin(), defenders_.end(),
			boost::bind(&MtActor::AfterBattleStartStub, _1, boost::ref(ctx)));
	}


}