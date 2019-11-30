#ifndef MTONLINE_GAMESERVER_SKILL_CONTEXT_H__
#define MTONLINE_GAMESERVER_SKILL_CONTEXT_H__
#include "../mt_types.h"
#include <json/forwards.h>
#include <stack>
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
	class SkillWrap;
	class BuffWrap;
	class TrapWrap;
	class ActorWrap
	{
	public:
		int32 PhysicalAttack() const;
		int32 PhysicalArmor() const;
		int32 MagicAttack() const;
		int32 MagicArmor() const;
		int32 Hp() const;
		int32 MaxHp() const;
		const std::vector<ActorWrap>& TeamMates(bool include_summon);
		const std::vector<ActorWrap>& Enemies(bool include_summon);
		int32 TeamMateCount();
		int32 EnemyCount();
		bool AddBuff(const Json::Value* config, ActorWrap* creator, SkillContext* ctx);
		bool IsFrozon() const;
		bool ClearBuff(Json::Value* config, ActorWrap* creator);
		real32 HpPercent() const;
		int32 GetMeta(const std::string& name) const;
		MtActorSkill* FindSkill(int32 id);
		int32 Level() const;
		void SetConfigId(int32 config_id);
		float ActorLevelFactor() const;
		void SetMeta(const std::string& name, int32 value);
		bool HasMark(const std::string& mark_name, ActorWrap* source);
		bool IsBleed() const;
		bool IsDead() const;
		void BeKilledBy(uint64 killer_guid);
		void UseSkill(SkillWrap* skill_wrap);
		void TakeSkillDamage(real32 damage, int32 damage_type, ActorWrap* source,
			SkillWrap* skill);
		void TakeBuffDamage(real32 damage, int32 damage_type,
			ActorWrap* source, BuffWrap* buff);
		void TakeTrapDamage(real32 damage, int32 damage_type,
			ActorWrap* source, TrapWrap* trap);
		void TakeSkillCure(real32 cure, ActorWrap* source, SkillWrap* skill);
		void TakeBuffCure(real32 cure, ActorWrap* source, BuffWrap* buff);
		void TakeCure(int32 cure, ActorWrap* source);
		bool Male() const;
		void AddRunTimeSkill(int32 id, int32 level);
		MtBattleGround* BattleGround() const;
		void ClearRunTimeSkill();
		bool Female() const;
		bool Alliance() const;
		/**
		* \brief 
		* \param id
		* \param creator_guid
		* \param type 
			-1:debuff(减益)
			1 buff(增益)
			0:所有
		* \return std::vector<BuffImpl::Buff*>
		*/
		std::vector<BuffImpl::Buff*> FindBuffs(int32 id,
			uint64 creator_guid, int32 type);
		/**
		* \brief 根据buff_id查找buff, 同一creator_guid不能创建重复的buff, 所以返回值不是数组
		* \param buff_id
		* \param creator_guid
		* \return BuffImpl::Buff*
		*/
		BuffImpl::Buff* FindBuff(int32 buff_id, uint64 creator_guid);
		/**
		* \brief 根据名称查找buff
		* \return std::vector<BuffImpl::Buff*>
		*/
		std::vector<BuffImpl::Buff*> FindBuffsByName(const std::string& name,
			uint64 creator_guid);
		bool Horde() const;
		uint64 Guid() const;
		real32 Distance(ActorWrap* other);
		MtActor* actor_ = nullptr;
		std::vector<ActorWrap> teammates_;
		std::vector<ActorWrap> enemies_;;
		SkillContext* ctx_ = nullptr;
	};
	class BuffWrap
	{
	public:
		void SetLifeTime(int32 life_time) const;
		int64 OriginLifeTime() const;
		int32 SkillLevel() const;
		float BuffLevelFactor() const;
		int32 Layer() const;
		void DecLayer(int32 dec);
		ActorWrap* Owner();
		ActorWrap* Creator();
		int32 DamageType() const;
		int64 GetData(const std::string& key) const;
		void SetData(const std::string& key, int64 val);
		BuffImpl::Buff* buff_ = nullptr;
		const Json::Value*  ExtraBuff(int32 id);
		void SetCriticalPercent(int32 percent);
		void SetControlProb(int32 prob);
		int32 ControlProb() const;
		void SetLayer(int32 l);
		bool CanDespell() const;
		bool HasControlStatus(int32 status);
	private:
		ActorWrap owner_;
		ActorWrap creator_;
	};
	class SkillWrap
	{
	public:
		SkillWrap() {}
		int32 SkillLevel() const;
		float SkillLevelFactor() const;
		int32 SkillId() const;
		bool CureSkill() const;
		int32 PassiveDamageType() const;
		int32 DamageType() const;
		int32 ExtraTargetDamageType() const;
		void AddPassiveTarget(ActorWrap* target);
		int32 TriggerCount();
		int32 SkillRange() const;
		bool BackStub() const { return false; }
		const Json::Value* ExtraBuff(int32 id);
		const Json::Value* TargetBuff(int32 id);
		void SetInnerSKill(MtActorSkill* skill) { skill_ = skill; }
		MtActorSkill* GetInnerSkill() const { return skill_; }
		ActorWrap* Owner();
		bool IsLongTimeSkill() const;
		bool IsFirstStage() const;
		MtActorSkill* skill_ = nullptr;
		ActorWrap owner_;
		std::vector<uint64> targets_;
		std::vector<ActorWrap> skill_targets_;
	};

	class TrapWrap
	{
	public:
		ActorWrap* Creator();
		const Json::Value*  ExtraBuff(int32 id);
		int32 TrapLevel() const;
		float TrapLevelFactor() const;
		int32 DamageType() const;
		TrapImpl::Trap* trap_ = nullptr;
		ActorWrap creator_;
	};

	class SkillContext
	{
	public:
		static void	InitLua();
		SkillContext() 
		{
			source_.ctx_ = this;
			target_.ctx_ = this;
			passive_owner_.ctx_ = this;
		}
		~SkillContext();
		void Clear();
		real32 SourceValue() const { return source_value_; }
		void SetSourceValue(real32 value) { source_value_ = value; }
		real32 TargetValue() const { return target_value_; }
		void SetTargetValue(real32 value) { target_value_ = value; }
		real32 EffectValue() const { return effect_value_; }
		void SetEffectValue(real32 value) { effect_value_ = value; }
		real32 SourceCriticalValue() const { return source_critical_value_; }
		void SetSourceCriticalValue(real32 value) { source_critical_value_ = value; }
		real32 SourceAccurateValue() const { return source_accurate_value_; }
		void SetSourceAccurateValue(real32 value) { accurate_percent_value_ = value; }
		real32 TargetDodgeValue() const { return target_dodge_value_; }
		void SetTargetDodgeValue(real32 value) { target_dodge_value_ = value; }
		real32 TargetToughnessValue() const { return target_toughness_value_; }
		void SetTargetToughnessValue(real32 value) { target_toughness_value_ = value; }
		real32 BuffEffectValue() const { return buff_effect_value_; }
		void SetBuffEffectValue(real32 value) { buff_effect_value_ = value; }
		real32 TrapEffectValue() const {	return trap_effect_value_;	}
		void SetTrapEffectValue(real32 value) { trap_effect_value_ = value; }
		real32 TargetHpPercent() const { return 0.2f; }
		real32 SourceHpPercent() const { return 0.2f; }
		bool IsTeammate(ActorWrap* lhs, ActorWrap* rhs);
		ActorWrap* Source();
		ActorWrap* Target();
		ActorWrap* PassiveOwner();
		SkillWrap* Skill();
		BuffWrap* Buff();
		TrapWrap* Trap();
		SkillWrap* PassiveSkill();
		void Cure(bool cure){cure_ = cure;}
		bool Physical() const { return physical_; }
		bool Magic() const { return !physical_; }
		bool IsCritical() const { return critical_; }
		bool IsDodge() const { return dodge_; }
		void CalcEffectValue();
		int32 CalcLifeStealValue();
		void SetDodgePercentValue(real32 value)
		{
			dodge_percent_value_ = value;
		}
		real32 DodgePercentValue() const { return dodge_percent_value_; }
		void SetCriticalFactor(real32 value)
		{
			critical_factor_value_ = value;
		}
		real32 CriticalFactor() const 
		{
			return critical_factor_value_;
		}
		void SetCriticalPercent(real32 value)
		{
			critical_percent_value_ = value;
		}
		real32 CriticalPercent() const { return critical_percent_value_; }

		void CalcBuffEffectValue();
		void CalcTrapEffectValue();
		Packet::DamageExpression DamageExpression() const;
		void CalcCriticalAndDodge();
		void CalcPassiveEffectValue();
		real32 DamageForm(real32 source_value, real32 target_value);
		real32 CureForm(real32 source_value);
		void PushCtx();
		void PopCtx();
		ActorWrap source_ ;
		ActorWrap target_ ;
		ActorWrap passive_owner_;
		SkillWrap skill_ ;
		SkillWrap passive_skill_;
		TrapWrap trap_;
		std::vector<ActorWrap> passive_target_;
		BuffWrap buff_;
		real32 source_value_ = 0.f;
		real32 target_value_ = 0.f;
		real32 effect_value_ = 0.f;
		real32 extra_effect_value_ = 0.f;
		real32 source_critical_value_ = 0.f;
		real32 target_dodge_value_ = 0.f;
		real32 source_accurate_value_ = 0.f;
		real32 target_toughness_value_ = 0.f;
		real32 critical_percent_value_ = 0.f;
		real32 accurate_percent_value_ = 1.f; //命中率, 默认100%,再减去闪避率
		real32 dodge_percent_value_ = 0.f;
		real32 critical_factor_value_ = 2.0f;
		bool critical_ = false;
		bool dodge_  = true;
		real32 buff_effect_value_ = 0.f;
		real32 passive_effect_value_ = 0.f;
		real32 trap_effect_value_ = 0.f;
		bool cure_ = false;
		bool  physical_ = true;
		std::stack<boost::shared_ptr<SkillContext>> saved_;
	private:
		SkillContext& operator=(const SkillContext& rhs) = default;
	};
}
#endif // MTONLINE_GAMESERVER_SKILL_CONTEXT_H__
