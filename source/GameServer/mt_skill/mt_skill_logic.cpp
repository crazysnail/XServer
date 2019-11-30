#include "mt_skill_logic.h"
#include "../mt_actor/mt_actor.h"
#include "mt_skill.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_skill/skill_context.h"
#include "../mt_battle/mt_trap.h"
#include "mt_target_select.h"
#include "BattleExpression.pb.h"
#include "ActorBasicInfo.pb.h"
#include "SkillConfig.pb.h"
#include <boost/make_shared.hpp>

namespace Mt
{

	void DirectDamage::Execute(MtActorSkill* skill)
	{
		auto& ctx = skill->Ctx();
		ctx.skill_.skill_ = skill;
		ctx.source_.actor_ = skill->Owner();
		auto battle_ground = skill->Owner()->BattleGround();
		auto skill_target = skill->GetSkillTarget();
		ctx.cure_ = false;
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		for (auto& target_guid : skill_target.targets_) {
			auto target = battle_ground->FindActor(target_guid);
			if (target == nullptr) {
				ZXERO_ASSERT(false) << "canot find target:" << target_guid;
				continue;;
			}

			ctx.dodge_ = true;
			ctx.critical_ = false;
			ctx.target_.actor_ = target;
			//在设置了target之后计算, 因为source_value_func有可能用到target
			ctx.CalcEffectValue();
			ctx.CalcCriticalAndDodge();
			if (ctx.target_.actor_->IsDead() 
				|| skill->ActiveConfig().isMember("source_value") == false)
				continue;
			if (ctx.source_.actor_->Guid() == target->Guid()) {
				LOG_ERROR << "[skill], damage on self! skill:" << skill->SkillId()
					<< ",actor_config:" << target->ConfigId();
				continue;
			}
			ctx.source_.actor_->BeforeDoDamageStub(ctx);
			ctx.target_.actor_->BeforeBeDamageStub(ctx);
			Packet::ActorOnDamage& msg = *skill_effects.add_damages();
			msg.set_source_guid(ctx.source_.Guid());
			msg.set_target_guid(ctx.target_.Guid());
			msg.set_type(Packet::DamageType(ctx.skill_.DamageType()));
			auto skill_info = msg.mutable_skill();
			skill_info->set_skill_id(skill->SkillId());
			skill_info->set_skill_level(skill->SKillLevel());
			msg.set_expression(ctx.DamageExpression());
			msg.set_damage(int32(ctx.effect_value_));
			ctx.target_.actor_->OnDamage(msg, ctx.source_.actor_);
			auto life_steal = ctx.CalcLifeStealValue();
			if ( life_steal > 0) {//吸血
				ctx.source_.TakeCure(life_steal, &ctx.source_);
			}
			if (ctx.dodge_) {//闪避时的被动技能
				ctx.source_.actor_->AfterBeDodgeStub(ctx);
				ctx.target_.actor_->AfterDoDodgeStub(ctx);
			}
			if (ctx.critical_){//暴击时相关被动出发
				ctx.source_.actor_->AfterDoCriticalStub(ctx);
				ctx.target_.actor_->AfterBeCriticalStub(ctx);
			}
			ctx.source_.actor_->AfterDoDamageStub(ctx);
			ctx.target_.actor_->AfterBeDamageStub(ctx);
		}
		for (auto& target_guid : skill_target.extra_target_) {
			ctx.dodge_ = true;
			auto target = battle_ground->FindActor(target_guid);
			if (target == nullptr) {
				ZXERO_ASSERT(false) << "cannot find actor:" << target_guid;
				continue;;
			}
			if (ctx.source_.actor_->Guid() == target->Guid()) {
				LOG_ERROR << "[skill], damage on self! skill:" << skill->SkillId()
					<< ",actor_config:" << target->ConfigId();
				continue;
			}
			ctx.target_.actor_ = target;
			//在设置了target之后计算, 因为source_value_func有可能用到target
			ctx.CalcCriticalAndDodge();
			if (ctx.extra_effect_value_ == 0.f || ctx.target_.actor_->IsDead())
				continue;
			ctx.source_.actor_->BeforeDoDamageStub(ctx);
			ctx.target_.actor_->BeforeBeDamageStub(ctx);
			Packet::ActorOnDamage& msg = *skill_effects.add_damages();
			msg.set_source_guid(ctx.source_.Guid());
			msg.set_target_guid(ctx.target_.Guid());
			msg.set_type(Packet::DamageType(ctx.skill_.ExtraTargetDamageType())); //已经没啥用了
			auto skill_info = msg.mutable_skill();
			skill_info->set_skill_id(skill->SkillId());
			skill_info->set_skill_level(skill->SKillLevel());
			msg.set_expression(ctx.DamageExpression());
			msg.set_damage(int32(ctx.extra_effect_value_));
			ctx.target_.actor_->OnDamage(msg, ctx.source_.actor_);
			auto life_steal = ctx.CalcLifeStealValue();
			if (life_steal > 0) {//吸血
				ctx.source_.TakeCure(life_steal, &ctx.source_);
			}
			if (ctx.dodge_) {//闪避时的被动技能
				ctx.source_.actor_->AfterBeDodgeStub(ctx);
				ctx.target_.actor_->AfterDoDodgeStub(ctx);
			}
			if (ctx.critical_) {//暴击时相关被动出发
				ctx.source_.actor_->AfterDoCriticalStub(ctx);
				ctx.target_.actor_->AfterBeCriticalStub(ctx);
			}
			ctx.source_.actor_->AfterDoDamageStub(ctx);
			ctx.target_.actor_->AfterBeDamageStub(ctx);
		}
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		battle_ground->BroadCastMessage(skill_effects);
	}

	void FlashChain::Execute(MtActorSkill* skill)
	{
		auto skill_target = skill->GetSkillTarget();
		if (skill_target.targets_.empty())
			return;
		auto& ctx = skill->Ctx();
		ctx.skill_.skill_ = skill;
		ctx.dodge_ = true;
		ctx.critical_ = false;
		ctx.source_.actor_ = skill->Owner();
		ctx.cure_ = false;
		auto battle_ground = skill->Owner()->BattleGround();
		auto target = battle_ground->FindActor(skill_target.FirstTargetGuid());
		if (target == nullptr) {
			ZXERO_ASSERT(false) << "cannot find actor:" << skill_target.FirstTargetGuid();
			return;
		}
		if (ctx.source_.actor_->Guid() == target->Guid()) {
			LOG_ERROR << "[skill], damage on self! skill:" << skill->SkillId()
				<< ",actor_config:" << target->ConfigId();
			return;
		}
		Packet::FlashChainDamage skill_effects;
		skill_effects.set_time_stamp(ctx.source_.actor_->BattleGround()->TimeStamp());
		real32 first_damage = 0;
		ctx.target_.actor_ = target;
		ctx.CalcEffectValue();
		ctx.CalcCriticalAndDodge();
		if (ctx.effect_value_ == 0.f || ctx.target_.actor_->IsDead())
			return;
		Packet::SkillInfo skill_info;
		skill_info.set_skill_id(skill->SkillId());
		skill_info.set_skill_level(skill->SKillLevel());
		ctx.source_.actor_->BeforeDoDamageStub(ctx);
		ctx.target_.actor_->BeforeBeDamageStub(ctx);
		Packet::ActorOnDamage& damage_msg = *skill_effects.add_damages();
		damage_msg.set_source_guid(ctx.source_.Guid());
		damage_msg.set_target_guid(ctx.target_.Guid());
		damage_msg.set_type(Packet::DamageType(ctx.skill_.DamageType()));
		damage_msg.mutable_skill()->CopyFrom(skill_info);
		damage_msg.set_expression(ctx.DamageExpression());
		damage_msg.set_damage(int32(ctx.effect_value_));
		ctx.target_.actor_->OnDamage(damage_msg, ctx.source_.actor_);
		auto life_steal = ctx.CalcLifeStealValue();
		if (life_steal > 0) {//吸血
			ctx.source_.TakeCure(life_steal, &ctx.source_);
		}
		if (ctx.dodge_) {//闪避时的被动技能
			ctx.source_.actor_->AfterBeDodgeStub(ctx);
			ctx.target_.actor_->AfterDoDodgeStub(ctx);
		}
		if (ctx.critical_) {//暴击时相关被动出发
			ctx.source_.actor_->AfterDoCriticalStub(ctx);
			ctx.target_.actor_->AfterBeCriticalStub(ctx);
		}
		ctx.source_.actor_->AfterDoDodgeStub(ctx);
		ctx.target_.actor_->AfterBeDamageStub(ctx);

		uint32 i = 1;
		const Json::Value& reduce = skill->ActiveConfig()["chain_reduce"];
		for (auto& target_guid : skill_target.extra_target_) {
			ctx.dodge_ = true;
			ctx.critical_ = false;
			target = battle_ground->FindActor(target_guid);
			if (target == nullptr) {
				ZXERO_ASSERT(false) << "cannot find actor:" << target_guid;
				continue;;
			}
			if (ctx.source_.actor_->Guid() == target->Guid()) {
				LOG_ERROR << "[skill], damage on self! skill:" << skill->SkillId()
					<< ",actor_config:" << target->ConfigId();
				continue;
			}
			ctx.target_.actor_ = target;
			ctx.CalcEffectValue();
			ctx.CalcCriticalAndDodge();
			real32 damage = ctx.effect_value_;
			if (reduce.size() > i)
				damage = ctx.effect_value_ * reduce[i].asFloat();
			++i;
			ctx.source_.actor_->BeforeDoDamageStub(ctx);
			ctx.target_.actor_->BeforeBeDamageStub(ctx);
			Packet::ActorOnDamage& sub_msg = *skill_effects.add_damages();
			sub_msg.set_source_guid(ctx.source_.Guid());
			sub_msg.set_target_guid(ctx.target_.Guid());
			sub_msg.set_type(Packet::DamageType(ctx.skill_.DamageType())); //已经没啥用了
			sub_msg.mutable_skill()->CopyFrom(skill_info);
			sub_msg.set_expression(ctx.DamageExpression());
			sub_msg.set_damage(int32(ctx.effect_value_));
			first_damage = ctx.effect_value_;
			target->OnDamage(sub_msg, ctx.source_.actor_);
			life_steal = ctx.CalcLifeStealValue();
			if (life_steal > 0) {//吸血
				ctx.source_.TakeCure(life_steal, &ctx.source_);
			}
			if (ctx.dodge_) {//闪避时的被动技能
				ctx.source_.actor_->AfterBeDodgeStub(ctx);
				ctx.target_.actor_->AfterDoDodgeStub(ctx);
			}
			if (ctx.critical_) {//暴击时相关被动出发
				ctx.source_.actor_->AfterDoCriticalStub(ctx);
				ctx.target_.actor_->AfterBeCriticalStub(ctx);
			}
			ctx.source_.actor_->AfterDoDamageStub(ctx);
			ctx.target_.actor_->AfterBeDamageStub(ctx);
		}
		skill->Owner()->BattleGround()->BroadCastMessage(skill_effects);
	}

	void CureChain::Execute(MtActorSkill* skill)
	{
		auto skill_target = skill->GetSkillTarget();
		if (skill_target.targets_.empty())
			return;
		auto& ctx = skill->Ctx();
		ctx.dodge_ = false;
		ctx.critical_ = false;
		ctx.skill_.skill_ = skill;
		ctx.source_.actor_ = skill->Owner();
		auto battle_ground = skill->Owner()->BattleGround();
		auto target = battle_ground->FindActor(skill_target.FirstTargetGuid());
		ctx.cure_ = true;
		Packet::CureChain skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		Packet::SkillInfo skill_info;
		skill_info.set_skill_id(skill->SkillId());
		skill_info.set_skill_level(skill->SKillLevel());
		if (target != nullptr) {
			ctx.target_.actor_ = target;
			ctx.CalcEffectValue();
			ctx.CalcCriticalAndDodge();
			if (ctx.effect_value_ == 0.f || ctx.target_.actor_->IsDead())
				return;
			ctx.source_.actor_->BeforeDoCureStub(ctx);
			ctx.target_.actor_->BeforeBeCureStub(ctx);
			Packet::ActorOnCure& cure_msg = *skill_effects.add_cures();
			cure_msg.set_source_guid(ctx.source_.Guid());
			cure_msg.set_target_guid(ctx.target_.Guid());
			cure_msg.mutable_skill()->CopyFrom(skill_info);
			cure_msg.set_cure(int32(ctx.effect_value_));
			target->OnCure(cure_msg, ctx.source_.actor_);
			ctx.source_.actor_->AfterDoCureStub(ctx);
			ctx.target_.actor_->AfterBeCureStub(ctx);
		}
		
		uint32 i = 1;
		const Json::Value& reduce = skill->ActiveConfig()["chain_reduce"];
		for (auto& target_guid : skill_target.extra_target_) {
			ctx.dodge_ = false;
			ctx.critical_ = false;
			target = battle_ground->FindActor(target_guid);
			if (target == nullptr) {
				ZXERO_ASSERT(false) << "cannot find actor:" << target_guid;
				continue;;
			}
			ctx.target_.actor_ = target;
			real32 damage = ctx.effect_value_;
			if (reduce.size() > i)
				damage = ctx.effect_value_ * reduce[i].asFloat();
			++i;
			ctx.source_.actor_->BeforeDoCureStub(ctx);
			ctx.target_.actor_->BeforeBeCureStub(ctx);
			Packet::ActorOnCure& sub_msg = *skill_effects.add_cures();
			sub_msg.set_source_guid(ctx.source_.Guid());
			sub_msg.set_target_guid(ctx.target_.Guid());
			sub_msg.mutable_skill()->CopyFrom(skill_info);
			sub_msg.set_cure(int32(ctx.effect_value_));
			target->OnCure(sub_msg, ctx.source_.actor_);
			skill_effects.add_cures()->CopyFrom(sub_msg);
			ctx.source_.actor_->AfterDoCureStub(ctx);
			ctx.target_.actor_->AfterBeCureStub(ctx);
		}
		skill->Owner()->BattleGround()->BroadCastMessage(skill_effects);
	}

	void DirectCure::Execute(MtActorSkill* skill)
	{
		auto skill_target = skill->GetSkillTarget();
		auto& ctx = skill->Ctx();
		ctx.source_.actor_ = skill->Owner();
		auto battle_ground = skill->Owner()->BattleGround();
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		ctx.cure_ = true;
		for (auto& target_guid : skill_target.targets_) {
			ctx.dodge_ = false;
			ctx.critical_ = false;
			auto target = battle_ground->FindActor(target_guid);
			if (target == nullptr) {
				ZXERO_ASSERT(false) << "cannot find actor:" << target_guid;
				continue;;
			}
			ctx.target_.actor_ = target;
			//在设置了target之后计算, 因为source_value_func有可能用到target
			ctx.CalcEffectValue();
			ctx.CalcCriticalAndDodge();
			if (ctx.effect_value_ == 0.f || ctx.target_.actor_->IsDead())
				continue;
			ctx.source_.actor_->BeforeDoCureStub(ctx);
			ctx.target_.actor_->BeforeBeCureStub(ctx);
			Packet::ActorOnCure& cure_message = *skill_effects.add_cures();
			cure_message.set_source_guid(ctx.source_.Guid());
			cure_message.set_target_guid(ctx.target_.Guid());
			cure_message.set_cure(int32(ctx.effect_value_));
			auto skill_info = cure_message.mutable_skill();
			skill_info->set_skill_id(skill->SkillId());
			skill_info->set_skill_level(skill->SKillLevel());
			target->OnCure(cure_message, ctx.source_.actor_);

			ctx.source_.actor_->AfterDoCureStub(ctx);
			ctx.target_.actor_->AfterBeCureStub(ctx);
		}
		for (auto& target_guid : skill_target.extra_target_) {
			ctx.dodge_ = false;
			ctx.critical_ = false;
			auto target = battle_ground->FindActor(target_guid);
			if (target == nullptr) {
				ZXERO_ASSERT(false) << "cannot find actor:" << target_guid;
				continue;;
			}
			ctx.target_.actor_ = target;
			//在设置了target之后计算, 因为source_value_func有可能用到target
			ctx.CalcCriticalAndDodge();
			if (ctx.effect_value_ == 0.f || ctx.target_.actor_->IsDead())
				continue;
			ctx.source_.actor_->BeforeDoCureStub(ctx);
			ctx.target_.actor_->BeforeBeCureStub(ctx);
			Packet::ActorOnCure& cure_message = *skill_effects.add_cures();
			cure_message.set_source_guid(ctx.source_.Guid());
			cure_message.set_target_guid(ctx.target_.Guid());
			cure_message.set_cure(int32(ctx.effect_value_));
			auto skill_info = cure_message.mutable_skill();
			skill_info->set_skill_id(skill->SkillId());
			skill_info->set_skill_level(skill->SKillLevel());
			target->OnCure(cure_message, ctx.source_.actor_);

			ctx.source_.actor_->AfterDoCureStub(ctx);
			ctx.target_.actor_->AfterBeCureStub(ctx);
		}
		battle_ground->BroadCastMessage(skill_effects);
	}
	
	const boost::shared_ptr<SkillEffect>& SkillLogicFactory::GetLogic(const std::string& name)
	{
		if (logics_.find(name) == std::end(logics_)) {
			logics_[name] = CreateLogic(name);
		}
		return logics_[name];
	}

	const boost::shared_ptr<SkillEffect> SkillLogicFactory::CreateLogic(const std::string& name)
	{
		if (name == "direct_damage")
			return boost::make_shared<DirectDamage>();
		if (name == "direct_cure") 
			return boost::make_shared<DirectCure>();
		if (name == "flash_chain")
			return boost::make_shared<FlashChain>();
		if (name == "cure_chain")
			return boost::make_shared<CureChain>();
		ZXERO_ASSERT(false);
		return nullptr;
	}

	const boost::shared_ptr<MtSkillPositionEffect>& SkillLogicFactory::GetPositionLogic(const std::string& name)
	{
		if (position_logics_.find(name) == std::end(position_logics_))
		{
			position_logics_[name] = CreatePositionLogic(name);
		}
		return position_logics_[name];
	}

	const boost::shared_ptr<MtSkillPositionEffect> SkillLogicFactory::CreatePositionLogic(const std::string& name)
	{
		if (name == "charge")
		{
			return boost::make_shared<MtSkillPositionCharge>();
		}
		if (name == "teleport")
		{
			return boost::make_shared<MtSkillPositionTeleport>();
		}
		if (name == "capture")
		{
			return boost::make_shared<MtSkillPositionCapture>();
		}
		ZXERO_ASSERT(false);
		return nullptr;
	}

	bool MtSkillPositionCharge::Execute(MtActorSkill* skill, zxero::uint64 elapseTime)
	{
		auto& skill_target = skill->GetSkillTarget();
		auto attacker = skill->Owner();
		auto battle_ground = skill->Owner()->BattleGround();
		auto first_target = battle_ground->FindActor(skill_target.FirstTargetGuid());
		if (first_target == nullptr) {
			return false;
		}
		auto speed = attacker->MoveSpeed() * 2.0f;
		attacker->Direction(zxero::get_direction(attacker->Position(), first_target->Position()));
		auto curr_distance = zxero::distance2d(attacker->Position(), first_target->Position());
		auto stop_range = attacker->ObjectRadius() + skill->ChargeStopRange() + first_target->ObjectRadius();
		if (curr_distance <= stop_range) {//完成
			return true;
		}
		auto move_distance = int(speed * elapseTime / 1000.f);
		auto new_pos = zxero::move_point(attacker->Position(), move_distance, attacker->Direction());
		auto new_distance = zxero::distance2d(new_pos, first_target->Position());
		if (new_distance <  stop_range || new_distance > curr_distance) {
			//一次tick遍可以移动到合适位置, 修正为移动到目标前方, 保留模型距离
			auto dir = zxero::get_direction(first_target->Position(), attacker->Position());
			new_pos = zxero::move_point(first_target->Position(), stop_range, dir);
			attacker->Position(new_pos);
			return true;
		}
		else {//继续移动
			attacker->Position(new_pos);
			return false;
		}
	}

	void MtSkillPositionCharge::StartLogic(MtActorSkill* skill)
	{
		Packet::SkillPositionLogic msg;
		auto attacker = skill->Owner();
		msg.set_type(Packet::SkillPositionLogicType::CHARGE_LOGIC);
		msg.set_source_guid(attacker->Guid());
		msg.set_target_guid(skill->GetSkillTarget().FirstTargetGuid());
		msg.set_move_speed(attacker->MoveSpeed() * 2);
		msg.set_time_stamp(attacker->BattleGround()->TimeStamp());
		msg.set_range(skill->ChargeStopRange());
		auto skill_info = msg.mutable_skill_info();
		skill_info->set_skill_id(skill->DbInfo()->skill_id());
		skill_info->set_skill_level(skill->DbInfo()->skill_level());
		attacker->BattleGround()->BroadCastMessage(msg);
	}

	int32 MtSkillPositionCharge::TimeCost(MtActorSkill* skill)
	{
		auto attacker = skill->Owner();
		auto battle_ground = attacker->BattleGround();
		auto first_target = battle_ground->FindActor(skill->GetSkillTarget().FirstTargetGuid());
		if (first_target == nullptr)
			return 0;
		auto distance = zxero::distance2d(attacker->Position(), first_target->Position()) - attacker->ObjectRadius() - first_target->ObjectRadius();
		return (zxero::int32)(distance * 1000 / (attacker->MoveSpeed() * 2.f));
	}

	bool MtSkillPositionTeleport::Execute(MtActorSkill* skill, uint64 /*elapseTime*/)
	{
		auto attacker = skill->Owner();
		auto battle_ground = attacker->BattleGround();
		auto first_target = battle_ground->FindActor(skill->GetSkillTarget().FirstTargetGuid());
		auto dir = zxero::get_direction(attacker->Position(), first_target->Position());
		auto distance = zxero::distance2d(attacker->Position(), first_target->Position()) - attacker->ObjectRadius() - first_target->ObjectRadius();
		auto teleport_pos = zxero::move_point(attacker->Position(), distance , dir);
		attacker->Position(teleport_pos);
		return true;
	}

	void MtSkillPositionTeleport::StartLogic(MtActorSkill* skill)
	{
		Packet::SkillPositionLogic msg;
		auto attacker = skill->Owner();
		msg.set_type(Packet::SkillPositionLogicType::TELEPORT_LOGIC);
		msg.set_source_guid(attacker->Guid());
		msg.set_target_guid(skill->GetSkillTarget().FirstTargetGuid());
		msg.set_time_stamp(attacker->BattleGround()->TimeStamp());
		auto skill_info = msg.mutable_skill_info();
		skill_info->set_skill_id(skill->DbInfo()->skill_id());
		skill_info->set_skill_level(skill->DbInfo()->skill_level());
		attacker->BattleGround()->BroadCastMessage(msg);
	}

	bool MtSkillPositionCapture::Execute(MtActorSkill* skill, uint64 /*elapseTime*/)
	{
		auto attacker = skill->Owner();
		auto& skill_target = skill->GetSkillTarget();
		auto battle_ground = attacker->BattleGround();
		for (auto& target_guid : skill_target.targets_) {
			auto target = battle_ground->FindActor(target_guid);
			auto dir = zxero::get_direction(target->Position(), attacker->Position());
			auto range = zxero::distance2d(target->Position(), attacker->Position()) - target->ObjectRadius() - attacker->ObjectRadius();
			auto new_pos = zxero::move_point(target->Position(), range, dir);
			target->Position(new_pos);
		}
		return true;
	}

	void MtSkillPositionCapture::StartLogic(MtActorSkill* skill)
	{
		Packet::SkillPositionLogic msg;
		auto attacker = skill->Owner();
		auto battle_ground = attacker->BattleGround();
		for (auto& target_guid :skill->GetSkillTarget().targets_)
		{
			msg.set_type(Packet::SkillPositionLogicType::CAPTURE_LOGIC);
			msg.set_source_guid(target_guid);
			msg.set_target_guid(attacker->Guid());
			msg.set_time_stamp(attacker->BattleGround()->TimeStamp());
			auto skill_info = msg.mutable_skill_info();
			skill_info->set_skill_id(skill->DbInfo()->skill_id());
			skill_info->set_skill_level(skill->DbInfo()->skill_level());
			battle_ground->BroadCastMessage(msg);
			msg.Clear();
		}
	}

}