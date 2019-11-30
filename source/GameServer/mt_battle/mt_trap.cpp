#include "mt_trap.h"
#include "TrapConfig.pb.h"
#include "BattleExpression.pb.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_skill/skill_context.h"
#include "../mt_arena/geometry.h"
#include "../mt_battle/mt_battle_ground.h"

namespace Mt
{
	namespace TrapImpl
	{
		MtActor* Trap::Creator() const
		{
			return creator_.get();
		}

		void Trap::Creator(MtActor* creator)
		{
			creator_ = creator->shared_from_this();
		}

		void Trap::OnTick(uint64 elapseTime)
		{
			if (life_time_ > 0) {
				if (life_time_ - int32(elapseTime) < 0) {
					life_time_ = 0;
				} else {
					life_time_ -= int32(elapseTime);
				}
			}
		}

		void Trap::ActorPositionChanged(MtActor& actor)
		{
			auto actor_it = std::find(in_trap_actors_.begin(),
				in_trap_actors_.end(), actor.Guid());
			auto distance = zxero::distance2d(actor.Position(), pos_);
			if (actor_it != in_trap_actors_.end()) {
				if (distance > range_) {
					OnActorLeave(actor);
					in_trap_actors_.erase(actor_it);
				}
			} else {
				if (distance <= range_) {
					OnActorEnter(actor);
					in_trap_actors_.push_back(actor.Guid());
				}
			}
		}

		void Trap::OnActorEnter(MtActor& actor)
		{
			if (Config().isMember("enemy_enter_stub_func")) {
				SkillContext ctx;
				ctx.source_.actor_ = Creator();
				ctx.target_.actor_ = &actor;
				ctx.trap_.trap_ = this;
				try {
					thread_lua->call_fun<void>(Config()["enemy_enter_stub_func"].asString(), &ctx);
				}
				catch (ff::lua_exception& e)
				{
					LOG_INFO << "trap enemy_enter_stub_func execute with lua error:" << e.what();
				}
			}
		}

		void Trap::OnActorLeave(MtActor& actor)
		{
			if (Config().isMember("enemy_leave_stub_func")) {
				SkillContext ctx;
				ctx.source_.actor_ = Creator();
				ctx.target_.actor_ = &actor;
				ctx.trap_.trap_ = this;
				try {
					thread_lua->call_fun<void>(Config()["enemy_leave_stub_func"].asString(), &ctx);
				}
				catch (ff::lua_exception& e)
				{
					LOG_INFO << "trap enemy_enter_stub_func execute with lua error:" << e.what();
				}
			}
		}

		void Trap::AfterCreate()
		{
			if (triggerd_) return;
			triggerd_ = true;
			auto bg = Creator()->BattleGround();
			if (bg == nullptr) {
				return;
			}
			SendCreateMsg();
			auto enemies = bg->Enemies(*Creator(),
				[](const boost::shared_ptr<MtActor>& actor) { return !actor->IsDead(); });
			SkillContext ctx;
			ctx.source_.actor_ = Creator();
			for (auto& actor : enemies) {
				auto distance = zxero::distance2d(actor->Position(), pos_);
				if (distance > range_) continue;
				ctx.target_.actor_ = actor;
				ctx.trap_.trap_ = this;
				ctx.CalcTrapEffectValue();
				if (ctx.trap_effect_value_ > 0.f) {
					ctx.target_.TakeTrapDamage(ctx.trap_effect_value_,
						ctx.trap_.DamageType(), &ctx.source_, &ctx.trap_);
				}
				ActorPositionChanged(*actor);
			}
		}

		bool Trap::NeedDestroy()
		{
			return life_time_ == 0;
		}

		void Trap::OnDestroy()
		{
			auto bg = Creator()->BattleGround();
			if (bg == nullptr) {
				LOG_ERROR << "[trap] on destroy battle ground nullptr" << this->Config()["name"].asString();
			}
			for (auto actor_guid : in_trap_actors_) {
				auto actor = bg->FindActor(actor_guid);
				if (actor != nullptr) {
					OnActorLeave(*actor);
				}
			}
			Packet::TrapDestroy msg;
			msg.set_guid(guid_);
			msg.set_time_stamp(bg->TimeStamp());
			bg->BroadCastMessage(msg);

		}

		void Trap::Position(Packet::Position& pos)
		{
			pos_ = pos;
		}

		Trap::Trap()
		{
			guid_ = MtGuid::Instance()(*this);
		}

		void Trap::SendCreateMsg() const
		{
			auto bg = Creator()->BattleGround();
			if (bg != nullptr) {

				Packet::TrapCreate msg;
				msg.set_guid(guid_);
				msg.set_time_stamp(bg->TimeStamp());
				auto& trap_info = *msg.mutable_trap();
				trap_info.set_trap_id(Config()["id"].asInt());
				trap_info.set_trap_level(level_);
				trap_info.mutable_pos()->CopyFrom(pos_);
				trap_info.set_range(Config()["range"].asInt());
				bg->BroadCastMessage(msg);
			}
		}

		void Bomb::AfterCreate()
		{
			if (triggerd_) return;
			triggerd_ = true;
			auto bg = Creator()->BattleGround();
			if (bg == nullptr) {
				return;
			}
			SendCreateMsg();
			auto enemies = bg->Enemies(*Creator(),
				[](const boost::shared_ptr<MtActor>& actor) { return !actor->IsDead(); });
			auto exclude_it = std::partition(enemies.begin(), enemies.end(),
				[&](MtActor* actor) {
				auto distance = zxero::distance2d(actor->Position(), pos_);
				return distance <= range_;
			});
			SkillContext ctx;
			ctx.trap_.trap_ = this;
			ctx.source_.actor_ = Creator();
			for (auto actor_it = enemies.begin(); actor_it != exclude_it; ++actor_it) {
				auto distance = zxero::distance2d((*actor_it)->Position(), pos_);
				if (distance > range_) continue;
				ctx.target_.actor_ = *actor_it;
				ctx.CalcTrapEffectValue();
				if (ctx.trap_effect_value_ > 0.f) {
					ctx.target_.TakeTrapDamage(ctx.trap_effect_value_,
						ctx.trap_.DamageType(), &ctx.source_, &ctx.trap_);
				}
				ActorPositionChanged(*ctx.target_.actor_);
			}
			//溅射
			int32 spuring_range = Config()["spurting_range"].asInt();
			auto spurting_it = std::partition(exclude_it, enemies.end(),
				[&](MtActor* actor) {
					auto distance = zxero::distance2d(actor->Position(), pos_);
					return distance <= spuring_range;
				});
			for (auto actor_it = exclude_it; actor_it != spurting_it; ++actor_it) {
				ctx.target_.actor_ = *actor_it;
				ctx.CalcTrapEffectValue();
				if (Config().isMember("spurting_effect_value_func")) {
					ctx.trap_effect_value_ = thread_lua->call_fun<real32>(
						Config()["spurting_effect_value_func"].asString(), &ctx);
				}
				if (ctx.trap_effect_value_ > 0.f) {
					ctx.target_.TakeTrapDamage(ctx.trap_effect_value_,
						ctx.trap_.DamageType(), &ctx.source_, &ctx.trap_);
				}
			}
		}

		boost::shared_ptr<Trap> CreateTrap(const Json::Value& config,
			Packet::Position& target_pos,
			SkillContext& ctx,
			MtActor* creator)
		{
			boost::shared_ptr<Trap> trap = nullptr;
			if (config["name"].asString() == "ice_trap"
				|| config["name"].asString() == "sacrifice"
				|| config["name"].asString() == "devotion") {
				trap = boost::make_shared<Trap>();
			} else if (config["name"].asString() == "bomb"){
				trap = boost::make_shared<Bomb>();
			} else {
				ZXERO_ASSERT(false) << "no such trap type" << config["name"].asString();
				return nullptr;
			}
			trap->SetTrapId(config["id"].asInt());
			trap->TrapLevel(ctx.skill_.SkillLevel());
			trap->Creator(creator);
			trap->Config(&config);
			trap->Position(target_pos);
			trap->Range(config["range"].asInt());
			return trap;
		}


	}

	/*MtTrap::MtTrap(boost::shared_ptr<Config::TrapConfig>& config,
		const Packet::Position& pos, 
		const boost::shared_ptr<MtActor>& creator, int32 damage)
		:config_(config), pos_(pos), creator_(creator), damage_(damage)
	{
		if (config_->damage() > 0) {
			damage_ = config_->damage();
		}
		left_time_ = config->last_time();
		guid_ = MtGuid::Instance()(*this);
		Packet::TrapCreate msg;
		msg.set_guid(guid_);
		msg.mutable_trap()->set_config_id(config_->id());
		msg.mutable_trap()->mutable_pos()->CopyFrom(pos_);
		msg.set_time_stamp(creator_->BattleGround()->TimeStamp());
		creator->BattleGround()->BroadCastMessage(msg);
	}

	MtTrap::MtTrap(boost::shared_ptr<Config::TrapConfig>& config, 
		int32 battle_group_index, 
		const boost::shared_ptr<MtActor>& creator, int32 damage)
		:config_(config), battle_group_index_(battle_group_index), creator_(creator), damage_(damage)
	{
		if (config_->damage() > 0) {
			damage_ = config_->damage();
		}
		auto battle_ground = creator_->BattleGround();
		auto scene_battle_group = battle_ground->SceneBattleGroup();
		if (scene_battle_group) {
			pos_.CopyFrom(scene_battle_group->enemy_pos(battle_group_index));
		}
		left_time_ = config->last_time();
		guid_ = MtGuid::Instance()(*this);
		Packet::TrapCreate msg;
		msg.set_guid(guid_);
		msg.mutable_trap()->set_config_id(config_->id());
		msg.mutable_trap()->mutable_pos()->CopyFrom(pos_);
		msg.set_time_stamp(creator_->BattleGround()->TimeStamp());
		creator->BattleGround()->BroadCastMessage(msg);
	}

	bool MtTrap::OnTick(uint64 elapseTime)
	{
		left_time_ -= elapseTime;
		if (left_time_ <= 0) {
			auto enemies = creator_->BattleGround()->Enemies(creator_, boost::bind(&MtActor::IsDead, _1) == false);
			for (auto& target : enemies) {
				if (zxero::distance2d(pos_, target->Position()) <= config_->range()) {
					auto origin_damage = config_->damage();
					Packet::ActorOnDamage msg;
					msg.set_source_guid(creator_->Guid());
					msg.set_target_guid(target->Guid());
					msg.set_type(Packet::DamageType::PHYSICAL_TO_PHYSICAL);
					msg.set_damage(origin_damage);
					msg.mutable_trap()->set_config_id(config_->id());
					msg.mutable_trap()->mutable_pos()->CopyFrom(pos_);
					msg.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
					target->OnDamage(msg, creator_);
				}
			}
			Packet::TrapDestroy msg;
			msg.set_guid(guid_);
			msg.set_time_stamp(creator_->BattleGround()->TimeStamp());
			creator_->BattleGround()->BroadCastMessage(msg);
			return false;
		}
		else {
			return true;
		}
	}*/


}
