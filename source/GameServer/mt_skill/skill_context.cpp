#include "skill_context.h"
#include "mt_buff.h"
#include "mt_skill.h"
#include "../mt_battle/mt_trap.h"
#include "../mt_battle/battle_event.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_battle/mt_battle_ground.h"
#include "json/json.h"
#include "lua/fflua.h"
#include "random_generator.h"
using Mt::BuffImpl::BuffFactory;
namespace Mt
{
	void SkillContext::InitLua()
	{
		ff::fflua_register_t<SkillContext, ctor()>(thread_lua->get_lua_state(), "SkillContext").
			def(&SkillContext::Source, "source").
			def(&SkillContext::Target, "target").
			def(&SkillContext::Skill, "skill").
			def(&SkillContext::PassiveOwner, "passive_owner").
			def(&SkillContext::PassiveSkill, "passive_skill").
			def(&SkillContext::IsTeammate, "is_teammate").
			def(&SkillContext::IsDodge, "is_dodge").
			def(&SkillContext::IsCritical, "is_critical").
			def(&SkillContext::IsCritical, "is_critial").
			def(&SkillContext::Cure, "set_cure").
			def(&SkillContext::Trap, "trap").
			def(&SkillContext::Buff, "buff").
			def(&SkillContext::Physical, "physical").
			def(&SkillContext::Magic, "magic").
			def(&SkillContext::CalcEffectValue, "calc_effect_value").
			def(&SkillContext::SourceValue, "source_value").
			def(&SkillContext::SetSourceValue, "set_source_value").
			def(&SkillContext::TargetValue, "target_value").
			def(&SkillContext::SetTargetValue, "set_target_value").
			def(&SkillContext::EffectValue, "effect_value").
			def(&SkillContext::SetEffectValue, "set_effect_value").
			def(&SkillContext::SourceCriticalValue, "source_critical_value").
			def(&SkillContext::DodgePercentValue, "dodge_percent_value").
			def(&SkillContext::SetDodgePercentValue, "set_dodge_percent_value").
			def(&SkillContext::SetSourceCriticalValue, "set_source_critical_value").
			def(&SkillContext::SourceAccurateValue, "source_accurate_value").
			def(&SkillContext::SetSourceAccurateValue, "set_source_accurate_value").
			def(&SkillContext::TargetDodgeValue, "target_dodge_value").
			def(&SkillContext::SetTargetDodgeValue, "set_target_dodge_value").
			def(&SkillContext::TargetToughnessValue, "target_toughness_value").
			def(&SkillContext::SetTargetToughnessValue, "set_target_toughness_value").
			def(&SkillContext::BuffEffectValue, "buff_effect_value").
			def(&SkillContext::SetBuffEffectValue, "set_buff_effect_value").
			def(&SkillContext::TrapEffectValue, "trap_effect_value").
			def(&SkillContext::SetTrapEffectValue, "set_trap_effect_value").
			def(&SkillContext::CriticalFactor, "critical_factor").
			def(&SkillContext::CriticalPercent, "critical_percent").
			def(&SkillContext::SetCriticalPercent, "set_critical_percent")
			;
		ff::fflua_register_t<ActorWrap, ctor()>(thread_lua->get_lua_state(), "ActorWrap").
			def(&ActorWrap::PhysicalArmor, "physical_armor").
			def(&ActorWrap::PhysicalAttack, "physical_attack").
			def(&ActorWrap::MagicAttack, "magic_attack").
			def(&ActorWrap::MagicArmor, "magic_armor").
			def(&ActorWrap::UseSkill, "use_skill").
			def(&ActorWrap::TeamMates, "teammates").
			def(&ActorWrap::Enemies, "enemies").
			def(&ActorWrap::TeamMateCount, "teammate_count").
			def(&ActorWrap::EnemyCount, "enemy_count").
			def(&ActorWrap::Hp, "hp").
			def(&ActorWrap::MaxHp, "max_hp").
			def(&ActorWrap::Guid, "guid").
			def(&ActorWrap::GetMeta, "get_meta").
			def(&ActorWrap::FindSkill, "find_skill").
			def(&ActorWrap::Level, "level").
			def(&ActorWrap::SetConfigId, "set_config_id").
			def(&ActorWrap::SetMeta, "set_meta").
			def(&ActorWrap::TakeSkillCure, "take_skill_cure").
			def(&ActorWrap::TakeBuffCure, "take_buff_cure").
			def(&ActorWrap::TakeSkillDamage, "take_skill_damage").
			def(&ActorWrap::TakeBuffDamage, "take_buff_damage").
			def(&ActorWrap::ActorLevelFactor, "actor_level_factor").
			def(&ActorWrap::TakeTrapDamage, "take_trap_damage").
			def(&ActorWrap::HpPercent, "hp_percent").
			def(&ActorWrap::BeKilledBy, "be_killed_by").
			def(&ActorWrap::AddBuff, "add_buff").
			def(&ActorWrap::ClearBuff, "clear_buff").
			def(&ActorWrap::HasMark, "has_mark").
			def(&ActorWrap::IsBleed, "is_bleed").
			def(&ActorWrap::IsDead, "is_dead").
			def(&ActorWrap::IsFrozon, "is_frozon").
			def(&ActorWrap::Male, "male").
			def(&ActorWrap::Female, "female").
			def(&ActorWrap::Alliance, "alliance").
			def(&ActorWrap::Horde, "horde").
			def(&ActorWrap::AddRunTimeSkill, "add_runtime_skill").
			def(&ActorWrap::ClearRunTimeSkill, "clear_runtime_skill").
			def(&ActorWrap::Distance, "distance").
			def(&ActorWrap::FindBuff, "find_buff").
			def(&ActorWrap::FindBuffs, "find_buffs").
			def(&ActorWrap::FindBuffsByName, "find_buffs_by_name").
			def(&ActorWrap::BattleGround, "battle_ground")
			;
		ff::fflua_register_t<SkillWrap, ctor()>(thread_lua->get_lua_state(), "SkillWrap").
			def(&SkillWrap::SkillId, "skill_id").
			def(&SkillWrap::SkillLevel, "skill_level").
			def(&SkillWrap::Owner, "owner").
			def(&SkillWrap::AddPassiveTarget, "add_passive_target").
			def(&SkillWrap::SkillRange, "skill_range").
			def(&SkillWrap::BackStub, "back_stab").
			def(&SkillWrap::DamageType, "damage_type").
			def(&SkillWrap::PassiveDamageType, "passive_damage_type").
			def(&SkillWrap::ExtraBuff, "extra_buff").
			def(&SkillWrap::TargetBuff, "target_buff").
			def(&SkillWrap::SetInnerSKill, "set_inner_skill").
			def(&SkillWrap::GetInnerSkill, "get_inner_skill").
			def(&SkillWrap::TriggerCount, "trigger_count").
			def(&SkillWrap::IsLongTimeSkill, "is_long_time_skill").
			def(&SkillWrap::IsFirstStage, "is_first_stage").
			def(&SkillWrap::SkillLevelFactor, "skill_level_factor")
			;
		ff::fflua_register_t<BuffWrap, ctor()>(thread_lua->get_lua_state(), "BuffWrap").
			def(&BuffWrap::SkillLevel, "skill_level").
			def(&BuffWrap::SkillLevel, "buff_level").
			def(&BuffWrap::BuffLevelFactor, "skill_level_factor").
			def(&BuffWrap::BuffLevelFactor, "buff_level_factor").
			def(&BuffWrap::Layer, "layer").
			def(&BuffWrap::DecLayer, "dec_layer").
			def(&BuffWrap::DamageType, "damage_type").
			def(&BuffWrap::Owner, "owner").
			def(&BuffWrap::Creator, "creator").
			def(&BuffWrap::GetData, "get_data").
			def(&BuffWrap::HasControlStatus, "has_control_status").
			def(&BuffWrap::ControlProb, "control_prob").
			def(&BuffWrap::SetControlProb,"set_control_prob").
			def(&BuffWrap::SetData, "set_data").
			def(&BuffWrap::ExtraBuff, "extra_buff").
			def(&BuffWrap::SetLifeTime, "set_life_time").
			def(&BuffWrap::OriginLifeTime, "origin_life_time").
			def(&BuffWrap::CanDespell, "can_despell").
			def(&BuffWrap::SetLayer, "set_layer")
			;
		ff::fflua_register_t<TrapWrap, ctor()>(thread_lua->get_lua_state(), "TrapWrap").
			def(&TrapWrap::Creator, "creator").
			def(&TrapWrap::ExtraBuff, "extra_buff").
			def(&TrapWrap::TrapLevel, "trap_level").
			def(&TrapWrap::TrapLevelFactor, "trap_level_factor")
			;
		ff::fflua_register_t<Json::Value, ctor()>(thread_lua->get_lua_state(), "JsonValue");
		std::string damage_types = 
			"DamageType_PHYSICAL = 1\n"
			"DamageType_FLASH = 2\n"
			"DamageType_ICE = 3\n"
			"DamageType_FIRE = 4\n"
			"DamageType_NATURE = 5\n"
			"DamageType_SHADOW = 6\n"
			"DamageType_HOLY = 7\n"
			;
		thread_lua->execute_str(damage_types, "=skill");
	}

	SkillContext::~SkillContext()
	{

	}


	void SkillContext::Clear()
	{

	}

	ActorWrap* SkillContext::Target()
	{
		return &target_;
	}
	
	ActorWrap* SkillContext::PassiveOwner()
	{
		return &passive_owner_;
	}


	bool SkillContext::IsTeammate(ActorWrap* lhs, ActorWrap* rhs)
	{
		if (lhs->actor_ != nullptr && rhs->actor_ != nullptr)
			return lhs->actor_->BattleGround()->IsTeamMate(*lhs->actor_, *rhs->actor_);
		return false;
	}

	ActorWrap* SkillContext::Source()
	{
		return &source_;
	}

	SkillWrap* SkillContext::Skill()
	{
		return &skill_;
	}
	
	BuffWrap* SkillContext::Buff()
	{
		return &buff_;
	}


	TrapWrap* SkillContext::Trap()
	{
		return &trap_;
	}

	SkillWrap* SkillContext::PassiveSkill()
	{
		return &passive_skill_;
	}

	real32 SkillContext::DamageForm(real32 source_value, real32 target_value)
	{
		if (source_value == 0.f)
			return 0.f;
		const auto attack_factor = 0.8;
		const auto armor_factor = 9;
		real32 ret = real32(source_value * source_value / (source_value + target_value * armor_factor) * attack_factor);
		ret *= (rand_gen->intgen(95, 105) / 100.f); //最终效果有一个小范围的随机
		return ret;
	}
	
	zxero::real32 SkillContext::CureForm(real32 source_value)
	{
		return source_value * 0.2f;
	}

	void SkillContext::PushCtx()
	{
		auto tmp = boost::make_shared<SkillContext>();
		std::stack<boost::shared_ptr<SkillContext>> tmp_saved(std::move(saved_));
		*tmp = *this;
		saved_ = std::move(tmp_saved);
		saved_.push(tmp);
	}

	void SkillContext::PopCtx()
	{
		std::stack<boost::shared_ptr<SkillContext>> tmp_saved(std::move(saved_));
		auto tmp = tmp_saved.top();
		*this = *tmp;
		saved_ = std::move(tmp_saved);
		saved_.pop();
	}

	void SkillContext::CalcEffectValue()
	{
		if (skill_.skill_ == nullptr || source_.actor_ == nullptr
			|| target_.actor_ == nullptr)
			return;
		try {
			source_value_ = target_value_ = effect_value_ = 0;
			extra_effect_value_ = source_critical_value_ = target_dodge_value_ = 0;
			source_accurate_value_ = target_toughness_value_ = critical_percent_value_ = 0;
			accurate_percent_value_ = 1.f;
			dodge_percent_value_ = 0.f;
			critical_factor_value_ = 2.f;
			buff_effect_value_ = passive_effect_value_ = trap_effect_value_ = 0.f;
			auto& config = skill_.skill_->ActiveConfig();
			if (config.isMember("source_value_func"))
				source_value_ = thread_lua->call_fun<real32>(
					config["source_value_func"].asString(), this);
			if (config.isMember("target_value_func"))
				target_value_ = thread_lua->call_fun<real32>(
					config["target_value_func"].asString(), this);
			source_.actor_->AfterSourceValueCalcStub(*this);
			source_.actor_->AfterTargetValueCalcStub(*this);
			target_.actor_->TargetAfterSourceValueCalcStub(*this);
			target_.actor_->TargetAfterTargetValueCalcStub(*this);

			auto source_critical_value = source_.actor_->Critical();
			auto target_dodge_value = target_.actor_->Dodge();
			auto source_accurate_value = source_.actor_->Accurate();
			auto target_toughness_value = target_.actor_->Toughness();
			source_.actor_->AfterSourceCriticalEtcCalcStub(*this);
			target_.actor_->AfterTargetCriticalEtcCalcStub(*this);

			if (cure_) {
				int32 actor_level = source_.Level() - 1;
				critical_percent_value_ = source_critical_value / float(source_critical_value + (actor_level * 6 + 12)* 9.f);
			}
			else
				critical_percent_value_ = source_critical_value / (source_critical_value + target_toughness_value * 9 + 0.000001f);
			critical_percent_value_ += source_.actor_->ExtraCriticalPercent();
			dodge_percent_value_ = target_dodge_value / (source_accurate_value * 9 + target_dodge_value + 0.000001f);
			source_.actor_->AfterCalcCriticalPercentStub(*this);
			target_.actor_->AfterCalcDodgePercentStub(*this);
			if (source_value_ != 0.f) {
				if (cure_)
					effect_value_ = CureForm(source_value_);
				else {
					effect_value_ = DamageForm(source_value_, target_value_);
				}
				if (effect_value_ < source_value_ * 0.1f) //最终效果不会小于起始攻击与法术强度的10%
					effect_value_ = source_value_ * 0.1f;
			}
			//技能主动修正生效值
			if (config.isMember("effect_value_func")) {
				effect_value_ = thread_lua->call_fun<real32>(
					config["effect_value_func"].asString(), this);
			}
			if (config.isMember("extra_effect_value_func")) {
				extra_effect_value_ = thread_lua->call_fun<real32>(
					config["extra_effect_value_func"].asString(), this);
			} else {
				extra_effect_value_ = effect_value_;
			}
			if (effect_value_ < 1.f && !cure_) effect_value_ = 1.f;
			if (config.isMember("critical_percent_value_func"))//暴击率
				critical_percent_value_ = thread_lua->call_fun<real32>(
					config["critical_percent_value_func"].asString(), this);
			if (config.isMember("critical_factor_value"))//暴击倍率
				critical_factor_value_ = thread_lua->call_fun<real32>(
					config["critical_factor_value_func"].asString(), this);
			if (config.isMember("accurate_percent_value"))//命中率
				accurate_percent_value_ = thread_lua->call_fun<real32>(
					config["accurate_percent_value_func"].asString(), this);
		}
		catch (ff::lua_exception& e)
		{
			LOG_WARN << "skill ctx execute with lua error:" << e.what();
		}
	}


	int32 SkillContext::CalcLifeStealValue()
	{
		if (skill_.skill_ == nullptr) return 0;
		auto& config = skill_.skill_->ActiveConfig();
		if (config.isMember("life_steal_value_func")) {
			return thread_lua->call_fun<int32>(config["life_steal_value_func"].asString(), this);
		}
		return 0;
	}

	void SkillContext::CalcBuffEffectValue()
	{
		if (buff_.buff_ == nullptr) return;
		try {
			auto& buff_config = buff_.buff_->GetConfig();
			real32 source_value, target_value;
			buff_effect_value_= source_value = target_value = 0;
			if (buff_config.isMember("source_value_func"))
				source_value = thread_lua->call_fun<real32>(
					buff_config["source_value_func"].asString(), this);
			if (buff_config.isMember("target_value_func"))
				target_value = thread_lua->call_fun<real32>(
					buff_config["target_value_func"].asString(), this);
			if (buff_config["cure"].asBool()) {
				buff_effect_value_ = CureForm(source_value);
			} else {
				buff_effect_value_ = DamageForm(source_value, target_value);
			}
			if (buff_config.isMember("effect_value_func")) {
				buff_effect_value_ = thread_lua->call_fun<real32>(
					buff_config["effect_value_func"].asString(), this);
			}
			if (buff_effect_value_ < 1.f) buff_effect_value_ = 1.f;
		}
		catch (ff::lua_exception& e)
		{
			LOG_WARN << "skill ctx execute with lua error:" << e.what();
		}
	}

	void SkillContext::CalcPassiveEffectValue()
	{
		if (passive_skill_.skill_ == nullptr) return;
		try
		{
			auto& passive_config = passive_skill_.skill_->PassiveConfig();
			real32 source_value, target_value;
			passive_effect_value_ = source_value = target_value = 0.f;
			if (passive_config.isMember("source_value_func")) {
				source_value = thread_lua->call_fun<real32>(
					passive_config["source_value_func"].asString(), this);
			}
			if (passive_config.isMember("target_value_func")) {
				target_value = thread_lua->call_fun<real32>(
					passive_config["target_value_func"].asString(), this);
			}
			if (passive_config["cure"].asBool())
				passive_effect_value_ = CureForm(source_value);
			else
				passive_effect_value_ = DamageForm(source_value, target_value);
			if (passive_config.isMember("effect_value_func")) {
				passive_effect_value_ = thread_lua->call_fun<real32>(
					passive_config["effect_value_func"].asString(), this);
			}
			if (passive_effect_value_ < 1.f) passive_effect_value_ = 0.f;
		}
		catch (ff::lua_exception& e)
		{
			LOG_WARN << "skill ctx calc passive effect value execute with lua error: " << e.what();
		}
	}

	void SkillContext::CalcTrapEffectValue()
	{
		try
		{
			auto& trap_config = trap_.trap_->Config();
			real32 source_value, target_value;
			trap_effect_value_ = source_value = target_value = 0;
			if (trap_config.isMember("source_value_func"))
				source_value = thread_lua->call_fun<real32>(
					trap_config["source_value_func"].asString(), this);
			if (trap_config.isMember("target_value_func"))
				target_value = thread_lua->call_fun<real32>(
					trap_config["target_value_func"].asString(), this);
			trap_effect_value_ = DamageForm(source_value, target_value);
			if (trap_config.isMember("effect_value_func")) {
				trap_effect_value_ = thread_lua->call_fun<real32>(
					trap_config["effect_value_func"].asString(), this);
			}
			if (trap_effect_value_ < 1.f) trap_effect_value_ = 1.f;

		}
		catch (ff::lua_exception& e)
		{
			LOG_WARN << "skill ctx execute with lua error:" << e.what();
		}

	}

	Packet::DamageExpression SkillContext::DamageExpression() const
	{
		if (dodge_)
			return Packet::DamageExpression::ActorOnDamage_DODGE;
		if (critical_ == true)
			return Packet::DamageExpression::ActorOnDamage_CRITICAL;
		return Packet::DamageExpression::ActorOnDamage_NORMAL;
	}

	void SkillContext::CalcCriticalAndDodge()
	{
		if (rand_gen->intgen(1, 100) / 100.f < critical_percent_value_) {
			effect_value_ *= critical_factor_value_;
			critical_ = true;
		}
		if (!cure_ && rand_gen->intgen(1, 100) / 100.f < accurate_percent_value_ - dodge_percent_value_) {
			dodge_ = false;
		}
	}

	int32 ActorWrap::PhysicalAttack() const
	{
		if (actor_ == nullptr) {
			ZXERO_ASSERT(false);
			return 0;
		}
		return actor_->PhysicalAttack();
	}

	zxero::int32 ActorWrap::PhysicalArmor() const
	{
		if (actor_ == nullptr) {
			ZXERO_ASSERT(false);
			return 0;
		}
		return actor_->PhysicalArmor();
	}

	zxero::int32 ActorWrap::Hp() const
	{
		if (actor_ == nullptr) {
			ZXERO_ASSERT(false);
			return 0;
		}
		return actor_->Hp();
	}


	zxero::int32 ActorWrap::MaxHp() const
	{
		if (actor_ == nullptr) {
			ZXERO_ASSERT(false);
			return 0;
		}
		return actor_->MaxHp();
	}


	const std::vector<ActorWrap>& ActorWrap::TeamMates(bool include_summon)
	{
		teammates_.clear();
		if (actor_ == nullptr) {
			return teammates_;
		}
		auto teammates = actor_->BattleGround()->TeamMates(*actor_);
		if (!include_summon) {
			teammates.erase(std::remove_if(teammates.begin(), teammates.end(),
				boost::bind(&MtActor::ActorType, _1) == Packet::ActorType::SUMMON_NO_SELECT),
				teammates.end());
		}
		boost::transform(teammates, std::back_inserter(teammates_),
			[](auto& actor) {
			ActorWrap wrap;
			wrap.actor_ = actor;
			return wrap;
		});
		return teammates_;
	}


	const std::vector<ActorWrap>& ActorWrap::Enemies(bool include_summon)
	{
		enemies_.clear();
		if (actor_ == nullptr) {
			return enemies_;
		}
		auto enemies = actor_->BattleGround()->Enemies(*actor_);
		if (!include_summon) {
			enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
				boost::bind(&MtActor::ActorType, _1) == Packet::ActorType::SUMMON_NO_SELECT),
				enemies.end());
		}

		boost::transform(enemies, std::back_inserter(enemies_),
			[](auto& actor) {
			ActorWrap wrap;
			wrap.actor_ = actor;
			return wrap;
		});
		return enemies_;
	}


	google::protobuf::int32 ActorWrap::TeamMateCount()
	{
		if (actor_ == nullptr) return 0;
		auto const& teammates = actor_->BattleGround()->TeamMates(*actor_);
		return teammates.size();
	}


	google::protobuf::int32 ActorWrap::EnemyCount()
	{
		if (actor_ == nullptr)return 0;
		auto const& enemies = actor_->BattleGround()->Enemies(*actor_);
		return enemies.size();
	}

	bool ActorWrap::AddBuff(const Json::Value* config, ActorWrap* creator, SkillContext* ctx)
	{
		if (actor_ == nullptr) return false;
		uint64 creator_guid = INVALID_GUID;
		if (creator->actor_ != nullptr) creator_guid = creator->actor_->Guid();
		if (config == nullptr) {
			int32 creator_config_id = 0;
			if (creator != nullptr && creator->actor_ != nullptr) {
				creator_config_id = creator->actor_->ConfigId();
			}
			LOG_WARN << "lua_add_buff without config,creator:" << creator_config_id;
			return false;
		}
		if (ctx == nullptr) {
			LOG_WARN << "lua_add_buff without ctx:" << (*config)["name"].asCString();
			return false;
		}
		if (config != nullptr && creator != nullptr) {
			auto buff = BuffImpl::BuffFactory::get_mutable_instance().CreateBuff(
				*config, *ctx, actor_->BattleGround(), creator_guid);
			if (buff) {
				auto e = boost::make_shared<AddBuffEvent>();
				e->target_guid_ = actor_->Guid();
				e->buff_ = buff;
				actor_->BattleGround()->PushEvent(e);
				return true;
			}
		}
		return false;
	}
	
	bool ActorWrap::IsFrozon() const
	{
		if (actor_ == nullptr) return false;
		return actor_->GetState(Config::BattleObjectStatus::FREEZE);
	}

	bool ActorWrap::ClearBuff(Json::Value* config, ActorWrap* creator)
	{
		if (actor_ == nullptr) return false;
		auto& buffs = actor_->Buffs();
		auto target_it = std::find_if(buffs.begin(), buffs.end(), [=](auto& buff) {
			return buff->BuffId() == (*config)["id"].asInt()
				&& buff->GetCreatorGuid() == creator->Guid();
		});
		if (target_it != buffs.end()) {
			(*target_it)->SetLayer(0);
		}
		return true;
	}

	zxero::real32 ActorWrap::HpPercent() const
	{
		if (actor_ == nullptr) return 0;
		return actor_->Hp() / real32(actor_->MaxHp());
	}


	zxero::int32 ActorWrap::GetMeta(const std::string& name) const
	{
		if (actor_ == nullptr) return 0;
		return actor_->Meta()[name].asInt();
	}


	MtActorSkill* ActorWrap::FindSkill(int32 id)
	{
		if (actor_ != nullptr) {
			auto it = std::find_if(actor_->Skills().begin(), actor_->Skills().end(),
				boost::bind(&MtActorSkill::SkillId, _1) == id);
			if (it != actor_->Skills().end())
				return (*it).get();
		}
		return nullptr;
	}

	int32 ActorWrap::Level() const
	{
		if (actor_ == nullptr) return 0;
		return actor_->Level();
	}


	void ActorWrap::SetConfigId(int32 config_id)
	{
		if (actor_ == nullptr) return;
		actor_->SetRuntimeActorId(config_id);
	}


	float ActorWrap::ActorLevelFactor() const
	{
		if (actor_ == nullptr)
			return 0.f;
		return MtSkillManager::Instance().SkillFactor(actor_->Level());
	}

	bool ActorWrap::Male() const
	{
		if (actor_ == nullptr) return false;
		return actor_->Male();
	}

	void ActorWrap::AddRunTimeSkill(int32 id, int32 level)
	{
		actor_->AddRuntimeSkill(id, level);
	}


	MtBattleGround* ActorWrap::BattleGround() const
	{
		if (actor_ && actor_->BattleGround())
			return actor_->BattleGround();
		else
			return nullptr;
	}

	void ActorWrap::ClearRunTimeSkill()
	{
		actor_->ClearRunTimeSkill();
	}

	bool ActorWrap::Female() const
	{
		if (actor_ == nullptr) return false;
		return actor_->Female();
	}
	bool ActorWrap::Alliance() const
	{
		if (actor_ == nullptr) return false;
		return actor_->IsAlliance(); 
	}


	std::vector<BuffImpl::Buff*> ActorWrap::FindBuffs(int32 id, uint64 creator_guid, int32 type)
	{
		std::vector<BuffImpl::Buff*> ret;
		if (actor_ == nullptr) return ret;
		auto& buffs = actor_->Buffs();
		for (auto& buff : buffs) {
			if (id > 0 && buff->BuffId() != id) continue;
			if (creator_guid != INVALID_GUID && creator_guid != buff->GetCreatorGuid()) continue;
			if (type == 1 && buff->IsDebuff() == true) continue; //1表示选择增益
			if (type == -1 && buff->IsDebuff() == false) continue; //-1表示减益
			ret.push_back(buff.get());
		}
		return ret;
	}

	BuffImpl::Buff* ActorWrap::FindBuff(int32 buff_id, uint64 creator_guid)
	{
		if (actor_ == nullptr) return nullptr;
		auto& buffs = actor_->Buffs();
		for (auto& buff : buffs) {
			if (buff->BuffId() == buff_id
				&& buff->GetCreatorGuid() == creator_guid) {
				return buff.get();
			}
		}
		return nullptr;
	}


	std::vector<BuffImpl::Buff*> ActorWrap::FindBuffsByName(const std::string& name, uint64 creator_guid)
	{
		std::vector<BuffImpl::Buff*> ret;
		if (actor_ == nullptr) return ret;
		auto& buffs = actor_->Buffs();
		for (auto& buff : buffs) {
			if (buff->NameEqual(name)
				&& buff->GetCreatorGuid() == creator_guid)
				ret.push_back(buff.get());
		}
		return ret;

	}

	bool ActorWrap::Horde() const
	{
		if (actor_ == nullptr)  return false;
		return actor_->IsHorde();
	}

	void ActorWrap::SetMeta(const std::string& name, int32 value)
	{
		if (actor_ == nullptr) return;
		actor_->Meta()[name] = value;
	}


	bool ActorWrap::HasMark(const std::string& mark_name, ActorWrap* source)
	{
		if (actor_ == nullptr) return false;
		auto& buffs = actor_->Buffs();
		if (source == nullptr) 
			return std::any_of(buffs.begin(), buffs.end(), boost::bind(&Buff::HasMark, _1, mark_name));

		return std::any_of(buffs.begin(), buffs.end(), 
			[&](boost::shared_ptr<Buff>& buff) {
			return buff->HasMark(mark_name) &&
				buff->GetCreatorGuid() == source->actor_->Guid();
			});
	}


	bool ActorWrap::IsBleed() const
	{
		if (actor_ == nullptr) return false;
		auto& buffs = actor_->Buffs();
		return std::any_of(buffs.begin(), buffs.end(), boost::bind(&Buff::IsBleed, _1));
	}


	bool ActorWrap::IsDead() const
	{
		if (actor_ == nullptr) return false;
		return actor_->IsDead();
	}


	void ActorWrap::BeKilledBy(uint64 killer_guid)
	{
		if (actor_ == nullptr) {
			LOG_ERROR << "[actor wrap] be killed by no target, killer:" << killer_guid;
			return;
		}
		if (this->BattleGround() == nullptr) {
			LOG_ERROR << "[actor wrap] be killed by no bg, killer:" << killer_guid;
			return;
		}
		auto event = boost::make_shared<DelayDead>();
		event->dead_guid_ = actor_->Guid();
		event->killer_guid_ = killer_guid;
		this->BattleGround()->PushEvent(event);
	}

	void ActorWrap::UseSkill(SkillWrap* skill_wrap)
	{
		if (skill_wrap->skill_ == nullptr)
			return;
		auto skill = skill_wrap->skill_;
		auto& config = skill->PassiveConfig();
		auto ctx = ctx_;
		if (skill->HasPassiveConfig()) {//被动技能
			if (config.isMember("active_skill")) {//包含主动技能
					//技能需要释放动作, 放到技能队列中, 下次使用技能时自动使用
				auto e = boost::make_shared<UseSkillEvent>();
				e->owner_guid_ = actor_->Guid();
				e->skill_id_ = skill->SkillId();
				e->target_ids_ = skill_wrap->targets_;
				skill_wrap->targets_.clear();
				actor_->BattleGround()->PushEvent(e);
			} else {//不包含主动技能, 直接造成伤害和添加buff
				skill->EnterCoolDown();
				ctx->passive_skill_.skill_ = skill;
				ctx->CalcPassiveEffectValue();
				for (auto& buff_config : config["self_buff"]) {
					AddBuff(&buff_config, this, ctx);
					/*auto buff = BuffFactory::get_mutable_instance().CreateBuff(
						buff_config, *ctx, skill->Owner());
					if (buff) actor_->AddBuff(buff);*/
				}
				SkillTarget skill_taraget;
				skill_taraget.targets_ = skill_wrap->targets_;
				skill_wrap->targets_.clear();
				if (skill_taraget.targets_.empty() && ctx->target_.actor_ != nullptr) {
					skill_taraget.targets_.push_back(ctx->target_.Guid());
				}
				auto battle_ground = skill->Owner()->BattleGround();
				for (auto& buff_config : config["target_buff"]) {
					for (auto& target_guid : skill_taraget.targets_) {
						ActorWrap target_wrap;
						auto target = battle_ground->FindActor(target_guid);
						if (target == nullptr) continue;;
						target_wrap.actor_ = target;
						if (ctx->passive_effect_value_ > 0.f) {
							if (config["cure"].asBool())
								target_wrap.TakeCure(int32(ctx->passive_effect_value_), this);
							else
								target_wrap.TakeSkillDamage(ctx->passive_effect_value_, skill_wrap->DamageType(), this, skill_wrap);
						}
						target_wrap.AddBuff(&buff_config, this, ctx);
						/*auto buff = BuffImpl::BuffFactory::get_mutable_instance().CreateBuff(
							buff_config, *ctx, skill->Owner());
						if (buff) target->AddBuff(buff);*/
					}
					for (auto& target_guid : skill_taraget.extra_target_) {
						auto target = battle_ground->FindActor(target_guid);
						if (target == nullptr) continue;
						ActorWrap target_wrap;
						target_wrap.actor_ = target;
						if (config["cure"].asBool())
							target_wrap.TakeCure(int32(ctx->passive_effect_value_), this);
						else
							target_wrap.TakeSkillDamage(ctx->passive_effect_value_, skill_wrap->DamageType(), this, skill_wrap);
						target_wrap.AddBuff(&buff_config, this, ctx);
/*
						auto buff = BuffImpl::BuffFactory::get_mutable_instance().CreateBuff(
							buff_config, *ctx, skill->Owner());
						if (buff) target->AddBuff(buff);*/
					}
				}
			}
		}
	}


	void ActorWrap::TakeSkillDamage(real32 damage, int32 damage_type, ActorWrap* source, SkillWrap* skill)
	{
		if (damage <= 1.f)
			return;
		if (actor_ == nullptr || source->actor_ == nullptr) {
			LOG_WARN << "[skill] take skill damage no source actor";
			return;
		}
		if (skill == nullptr || skill->skill_ == nullptr) {
			LOG_WARN << "[skill] take skill damage no source skill";
			return;
		}
		SkillContext dummy;
		auto battle_ground = source->actor_->BattleGround();
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		Packet::ActorOnDamage& msg = *skill_effects.add_damages();
		msg.set_damage(int32(damage));
		msg.set_source_guid(source->actor_->Guid());
		msg.set_target_guid(actor_->Guid());
		msg.set_type(Packet::DamageType(damage_type));
		msg.mutable_skill()->set_skill_id(skill->skill_->SkillId());
		msg.mutable_skill()->set_skill_level(skill->skill_->SKillLevel());
		msg.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
		actor_->OnDamage(msg, source->actor_, &dummy);
		battle_ground->BroadCastMessage(skill_effects);
	}

	void ActorWrap::TakeBuffDamage(real32 damage, int32 damage_type,
		ActorWrap* source, BuffWrap* buff)
	{
		if (damage <= 1.f)
			return;
		if (actor_ == nullptr || source->actor_ == nullptr) {
			LOG_WARN << "[skill] take buff damage no source actor";
			return;
		}
		if (buff == nullptr || buff->buff_ == nullptr) {
			LOG_WARN << "[skill] take buff damage no source buff";
			return;
		}
		SkillContext dummy;
		auto battle_ground = source->actor_->BattleGround();
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		Packet::ActorOnDamage& msg = *skill_effects.add_damages();
		msg.set_damage(int32(damage));
		msg.set_source_guid(source->actor_->Guid());
		msg.set_target_guid(actor_->Guid());
		msg.set_type(Packet::DamageType(damage_type));
		msg.mutable_buff()->set_buff_id(buff->buff_->BuffId());
		msg.mutable_buff()->set_buff_level(buff->buff_->BuffLevel());
		msg.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
		actor_->OnDamage(msg, source->actor_, &dummy);
		battle_ground->BroadCastMessage(skill_effects);
	}


	void ActorWrap::TakeSkillCure(real32 cure, ActorWrap* source, SkillWrap* skill)
	{
		if (cure <= 1.f)
			return;
		if (actor_ == nullptr || source->actor_ == nullptr) {
			LOG_WARN << "[skill] take skill cure no source actor";
			return;
		}
		if (skill == nullptr || skill->skill_ == nullptr) {
			LOG_WARN << "[skill] take skill cure no source skill";
			return;
		}
		SkillContext dummy;
		auto battle_ground = source->actor_->BattleGround();
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		Packet::ActorOnCure& msg = *skill_effects.add_cures();
		msg.set_cure(int32(cure));
		msg.set_source_guid(source->actor_->Guid());
		msg.set_target_guid(actor_->Guid());
		msg.mutable_skill()->set_skill_id(skill->skill_->SkillId());
		msg.mutable_skill()->set_skill_level(skill->skill_->SKillLevel());
		actor_->OnCure(msg, source->actor_, &dummy);
		battle_ground->BroadCastMessage(skill_effects);
	}


	void ActorWrap::TakeBuffCure(real32 cure, ActorWrap* source, BuffWrap* buff)
	{
		if (cure <= 1.f)
			return;
		if (actor_ == nullptr || source->actor_ == nullptr) {
			LOG_WARN << "[skill] take buff cure no source actor";
			return;
		}
		if (buff == nullptr || buff->buff_ == nullptr) {
			LOG_WARN << "[skill] take buff cure no source skill";
			return;
		}
		SkillContext dummy;
		auto battle_ground = source->actor_->BattleGround();
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		Packet::ActorOnCure& msg = *skill_effects.add_cures();
		msg.set_cure(int32(cure));
		msg.set_source_guid(source->actor_->Guid());
		msg.set_target_guid(actor_->Guid());
		msg.mutable_buff()->set_buff_id(buff->buff_->BuffId());
		msg.mutable_buff()->set_buff_level(buff->buff_->BuffLevel());
		actor_->OnCure(msg, source->actor_, &dummy);
		battle_ground->BroadCastMessage(skill_effects);
	}

	void ActorWrap::TakeTrapDamage(real32 damage, int32 damage_type,
		ActorWrap* source, TrapWrap* trap)
	{
		if (damage <= 1.f)
			return;
		if (actor_ == nullptr || source->actor_ == nullptr) {
			LOG_WARN << "[skill] take trap damage no source actor";
			return;
		}
		if (trap == nullptr || trap->trap_ == nullptr) {
			LOG_WARN << "[skill] take trap damage no source trap";
			return;
		}
		SkillContext dummy;
		auto battle_ground = source->actor_->BattleGround();
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(battle_ground->TimeStamp());
		Packet::ActorOnDamage& msg = *skill_effects.add_damages();
		msg.set_damage(int32(damage));
		msg.set_source_guid(source->actor_->Guid());
		msg.set_target_guid(actor_->Guid());
		msg.set_type(Packet::DamageType(damage_type));
		msg.mutable_trap()->set_trap_id(trap->trap_->GetTrapId());
		msg.mutable_trap()->set_trap_level(trap->trap_->TrapLevel());
		msg.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
		actor_->OnDamage(msg, source->actor_, &dummy);
		battle_ground->BroadCastMessage(skill_effects);
	}

	void ActorWrap::TakeCure(int32 cure, ActorWrap* source)
	{
		if (actor_ == nullptr || source->actor_ == nullptr)
			return;
		if (cure <= 0) return;
		SkillContext dummy;
		Packet::ActorOnCure msg;
		msg.set_cure(cure);
		msg.set_source_guid(source->actor_->Guid());
		msg.set_target_guid(actor_->Guid());
		actor_->OnCure(msg, source->actor_, &dummy);
	}

	uint64 ActorWrap::Guid() const
	{
		if (actor_ == nullptr) return INVALID_GUID;
		return actor_->Guid();
	}


	zxero::real32 ActorWrap::Distance(ActorWrap* other)
	{
		if (actor_ == nullptr || other->actor_ == nullptr) {
			return 0;
		}
		return real32(zxero::distance2d(actor_->Position(), other->actor_->Position()));
	}

	zxero::int32 ActorWrap::MagicAttack() const
	{
		if (actor_ == nullptr) return 0;
		return actor_->MagicAttack();
	}

	zxero::int32 ActorWrap::MagicArmor() const
	{
		if (actor_ == nullptr) return 0;
		return actor_->MagicArmor();
	}


	zxero::int32 SkillWrap::SkillLevel() const
	{
		if (skill_ == nullptr) return 0;
		return skill_->SKillLevel();
	}


	float SkillWrap::SkillLevelFactor() const
	{
		if (skill_ == nullptr)
			return 0.f;
		return MtSkillManager::Instance().SkillFactor(skill_->SKillLevel());
	}

	int32 SkillWrap::SkillId() const
	{
		if (skill_ == nullptr) return 0;
		return skill_->SkillId();
	}

	bool SkillWrap::CureSkill() const
	{
		if (skill_ == nullptr) return false;
		return skill_->JsonConfig()["cure"].asBool();
	}


	int32 SkillWrap::PassiveDamageType() const
	{
		Packet::DamageType type(Packet::DamageType::PHYSICAL);
		Packet::DamageType_Parse(skill_->PassiveConfig()["damage_type"].asString(), &type);
		return type;
	}


	int32 SkillWrap::DamageType() const
	{
		Packet::DamageType type(Packet::DamageType::PHYSICAL);
		Packet::DamageType_Parse(skill_->ActiveConfig()["damage_type"].asString(), &type);
		return type;
	}


	zxero::int32 SkillWrap::ExtraTargetDamageType() const
	{
		std::string t = skill_->ActiveConfig()["extar_target_damage_type"].asString();
		if (t.empty() == false){
			Packet::DamageType type(Packet::DamageType::PHYSICAL);
			Packet::DamageType_Parse(t, &type);
			return type;
		} else {
			return DamageType();
		}
	}

	void SkillWrap::AddPassiveTarget(ActorWrap* target)
	{
		targets_.push_back(target->actor_->Guid());
	}


	zxero::int32 SkillWrap::TriggerCount()
	{
		if (skill_ == nullptr)
			return 0;
		return skill_->TriggerCount();
	}

	zxero::int32 SkillWrap::SkillRange() const
	{
		if (skill_ == nullptr) return 0;
		return skill_->SkillRange();
	}


	const Json::Value* SkillWrap::ExtraBuff(int32 id)
	{
		if (skill_ == nullptr) return nullptr;
		return &(skill_->ActiveConfig()["extra_buff"][id]);
	}

	const Json::Value* SkillWrap::TargetBuff(int32 id)
	{
		if (skill_ == nullptr) return nullptr;
		return &(skill_->ActiveConfig()["target_buff"][id]);
	}

	ActorWrap* SkillWrap::Owner()
	{
		if (skill_ == nullptr) return nullptr;
		owner_.actor_ = skill_->Owner();
		return &owner_;
	}

	bool SkillWrap::IsLongTimeSkill() const
	{
		if (skill_ == nullptr) return false;
		return skill_->IsLongTimeSkill();
	}


	bool SkillWrap::IsFirstStage() const
	{
		if (skill_ == nullptr) return false;
		return skill_->IsFirstStage();
	}

	void BuffWrap::SetLifeTime(int32 life_time) const
	{
		if (buff_ == nullptr) return;
		buff_->SetLifeTime(life_time);
	}

	int64 BuffWrap::OriginLifeTime() const
	{
		if (buff_ == nullptr) return 0;
		return buff_->OriginLifeTime();
	}

	int32 BuffWrap::SkillLevel() const
	{
		if (buff_ == nullptr) return 0;
		return buff_->BuffLevel();
	}

	float BuffWrap::BuffLevelFactor() const
	{
		if (buff_ == nullptr)
			return 0.f;
		return MtSkillManager::Instance().SkillFactor(SkillLevel());
	}

	int32 BuffWrap::Layer() const
	{
		if (buff_ == nullptr) return 0;
		return buff_->GetLayer();
	}


	void BuffWrap::DecLayer(int32 dec)
	{
		if (buff_ == nullptr) return;
		auto curr = buff_->GetLayer();
		buff_->SetLayer(curr - dec);
	}

	ActorWrap* BuffWrap::Owner()
	{
		if (buff_ == nullptr) return nullptr;
		owner_.actor_ = buff_->GetOwner();
		return &owner_;
	}


	ActorWrap* BuffWrap::Creator()
	{
		if (buff_ == nullptr) return nullptr;
		creator_.actor_ = buff_->GetCreator();
		return &creator_;
	}


	int32 BuffWrap::DamageType() const
	{
		if (buff_ == nullptr) return 0;
		return buff_->DamageType();
	}


	zxero::int64 BuffWrap::GetData(const std::string& key) const
	{
		if (buff_)
			return buff_->GetData(key);
		return 0;
	}


	void BuffWrap::SetData(const std::string& key, int64 val)
	{
		if (buff_)
			buff_->SetData(key, val);
	}

	const Json::Value*  BuffWrap::ExtraBuff(int32 id)
	{
		if (buff_ == nullptr) return nullptr;
		return &buff_->GetConfig()["extra_buff"][id];
	}


	void BuffWrap::SetCriticalPercent(int32 percent)
	{
		if (buff_ == nullptr) return;
		BuffImpl::PulseBuff* pulse_buff = dynamic_cast<BuffImpl::PulseBuff*>(buff_);
		if (pulse_buff != nullptr) {
			pulse_buff->SetBuffCriticalPercent(percent);
		}
	}


	void BuffWrap::SetControlProb(int32 prob)
	{
		BuffImpl::AddStatus* control_buff
			= dynamic_cast<BuffImpl::AddStatus*>(buff_);
		if (control_buff == nullptr) return;
		control_buff->SetControlProb(prob);
	}

	int32 BuffWrap::ControlProb() const
	{
		BuffImpl::AddStatus* control_buff
			= dynamic_cast<BuffImpl::AddStatus*>(buff_);
		if (control_buff == nullptr) return 0;
		return control_buff->ControlProb();
	}


	void BuffWrap::SetLayer(int32 l)
	{
		if (buff_ == nullptr) return;
		buff_->SetLayer(l);
	}

	bool BuffWrap::CanDespell() const
	{
		if (buff_ == nullptr) return false;
		return buff_->CanDespell();
	}

	bool BuffWrap::HasControlStatus(int32 status)
	{
		BuffImpl::AddStatus* control_buff
			= dynamic_cast<BuffImpl::AddStatus*>(buff_);
		if (control_buff == nullptr) return false;
		return control_buff->HasControlStatus(Config::BattleObjectStatus(status));
	}

	ActorWrap* TrapWrap::Creator()
	{
		if (trap_ == nullptr) return nullptr;
			creator_.actor_ = trap_->Creator();
			return &creator_;
	}


	const Json::Value* TrapWrap::ExtraBuff(int32 id)
	{
		if (trap_ == nullptr) return nullptr;
		return &trap_->Config()["extra_buff"][id];
	}


	int32 TrapWrap::TrapLevel() const
	{
		if (trap_ == nullptr) return 0;
		return trap_->TrapLevel();
	}


	float TrapWrap::TrapLevelFactor() const
	{
		if (trap_ == nullptr)
			return 0.f;
		return MtSkillManager::Instance().SkillFactor(TrapLevel());
	}

	zxero::int32 TrapWrap::DamageType() const
	{
		if (trap_ == nullptr) return 0;
		Packet::DamageType type(Packet::DamageType::PHYSICAL);
		Packet::DamageType_Parse(trap_->Config()["damage_type"].asString(), &type);
		return type;
	}

}