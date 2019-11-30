#include "mt_actor.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_skill/mt_buff.h"
#include "../mt_skill/skill_context.h"
#include "json/json.h"

namespace Mt
{
	void MtActor::AfterDoDodgeStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_do_dodge_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_do_dodge_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}

		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_do_dodge_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_do_dodge_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterDoCriticalStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_do_critical_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_do_critical_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_do_critical_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_do_critical_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterBattleStartStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_battle_start_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_battle_start_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
	}

	void MtActor::BeforeBattleStartStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_battle_start_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_battle_start_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
	}

	void MtActor::AfterSourceValueCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_source_value_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_source_value_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_source_value_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_source_value_calc_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::TargetAfterSourceValueCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::target_after_source_value_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["target_after_source_value_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::target_after_source_value_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["target_after_source_value_calc_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::TargetAfterTargetValueCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::target_after_target_value_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["target_after_target_value_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::target_after_target_value_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["target_after_target_value_calc_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterTargetValueCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_target_value_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_target_value_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_target_value_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_target_value_calc_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterSourceCriticalEtcCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_source_critical_etc_value_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_source_critical_etc_value_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_source_critical_etc_value_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_source_critical_etc_value_calc_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::BeforeBuffAddStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_buff_add_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_buff_add_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::before_buff_add_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["before_buff_add_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterBuffAddStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_buff_add_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_buff_add_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_buff_add_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_buff_add_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::OnTimeElapseStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::on_time_elapse_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["on_time_elapse_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::on_time_elapse_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				thread_lua->call_fun<void>(buff->GetConfig()["on_time_elapse_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterControlBuffProbCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_control_prob_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_control_prob_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_control_prob_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_control_prob_calc_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterTargetCriticalEtcCalcStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_target_critical_etc_value_calc_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_target_critical_etc_value_calc_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_target_critical_etc_value_calc_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_target_critical_etc_value_calc_stub_func"].asString(), &ctx);
			}
		}
	}


	void MtActor::AfterBeCureStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_be_cure_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_be_cure_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_be_cure_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_be_cure_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::BeforeBeCureStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_be_cure_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_be_cure_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::before_be_cure_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["before_be_cure_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::BeforeDoCureStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_do_cure_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_do_cure_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::before_do_cure_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["before_do_cure_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterDoCureStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_do_cure_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_do_cure_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_do_cure_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_do_cure_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::BeforeBeDamageStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_be_damage_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_be_damage_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::before_be_damage_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["before_be_damage_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterBeDamageStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_be_damage_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_be_damage_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_be_damage_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_be_damage_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::BeforeDoDamageStub(SkillContext& ctx)
	{

		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_do_damage_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_do_damage_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::before_do_damage_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["before_do_damage_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterDoDamageStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_do_damage_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_do_damage_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_do_damage_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_do_damage_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterBeDodgeStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_be_dodge_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_be_dodge_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_be_dodge_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_be_dodge_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterBeCriticalStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_be_critical_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_be_critical_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_be_critical_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_be_critical_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterCalcCriticalPercentStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_calc_critical_percent_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_calc_critical_percent_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_calc_critical_percent_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_calc_critical_percent_stub_func"].asString(), &ctx);
			}
		}
	}

	void MtActor::AfterCalcDodgePercentStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_calc_dodge_percent_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_calc_dodge_percent_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_calc_dodge_percent_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_calc_dodge_percent_stub_func"].asString(), &ctx);
			}
		}
	}
	MtActorSkill* MtActor::FindRuntimeSkill(int32 skill_id)
	{
		for (auto& skill : runtime_info_.skills_) {
			if (skill->SkillId() == skill_id) {
				return skill.get();
			}
		}
		return nullptr;
	}

	void MtActor::AfterSelectSkill(SkillContext& ctx)
	{
		try
		{
			ctx.passive_owner_.actor_ = this;
			for (auto& skill : runtime_info_.skills_) {
				if (skill->HasPassiveConfig()
					&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_select_skill_stub_func)
					&& skill->CanTrigger()) {
					ctx.passive_skill_.skill_ = skill.get();
					if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_select_skill_stub_func"].asString(), &ctx))
						skill->AddTriggerCount();
				}
			}
			for (auto& buff : buffs_) {
				if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_select_skill_stub_func)) {
					ctx.buff_.buff_ = buff.get();
					ctx.CalcBuffEffectValue();
					thread_lua->call_fun<void>(buff->GetConfig()["after_select_skill_stub_func"].asString(), &ctx);
				}
			}
		}
		catch (ff::lua_exception& e)
		{
			LOG_INFO << "after select skill stub execute fail with:" << e.what();
		}
	}

	void MtActor::AfterHpChange(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_hp_change_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_hp_change_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_hp_change_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_hp_change_stub_func"].asString(), &ctx);
			}
		}
	}


	void MtActor::AfterBuffMaxLayer(SkillContext& ctx)
	{
		if (ctx.buff_.buff_->BuffType() == Config::BuffType::combo_point) {
			ctx.passive_owner_.actor_ = this;
			for (auto& skill : runtime_info_.skills_) {
				if (skill->HasPassiveConfig()
					&& skill->HasPassiveStub(Packet::SkillBuffStubType::combo_point_overflow_stub_func)
					&& skill->CanTrigger()) {
					ctx.passive_skill_.skill_ = skill.get();
					if (thread_lua->call_fun<bool>(skill->PassiveConfig()["combo_point_overflow_stub_func"].asString(), &ctx))
						skill->AddTriggerCount();
				}
			}
		}
	}

	void MtActor::AfterUseActiveSkillStub(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::after_use_active_skill_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["after_use_active_skill_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::after_use_active_skill_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["after_use_active_skill_stub_func"].asString(), &ctx);
			}
		}
	}

	bool MtActor::BeforeDeadStub(SkillContext& ctx)
	{
		bool ret = false;
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::before_dead_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["before_dead_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
				ret = true;
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::before_dead_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["before_dead_stub_func"].asString(), &ctx);
				ret = true;
			}
		}
		return ret;
	}

	void MtActor::AfterActorUseLongTimeSkill(SkillContext& ctx)
	{
		ctx.passive_owner_.actor_ = this;
		for (auto& skill : runtime_info_.skills_) {
			if (skill->HasPassiveConfig()
				&& skill->HasPassiveStub(Packet::SkillBuffStubType::actor_use_long_time_skill_stub_func)
				&& skill->CanTrigger()) {
				ctx.passive_skill_.skill_ = skill.get();
				if (thread_lua->call_fun<bool>(skill->PassiveConfig()["actor_use_long_time_skill_stub_func"].asString(), &ctx))
					skill->AddTriggerCount();
			}
		}
		for (auto& buff : buffs_) {
			if (buff->HasPassiveStub(Packet::SkillBuffStubType::actor_use_long_time_skill_stub_func)) {
				ctx.buff_.buff_ = buff.get();
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(buff->GetConfig()["actor_use_long_time_skill_stub_func"].asString(), &ctx);
			}
		}
	}

}
