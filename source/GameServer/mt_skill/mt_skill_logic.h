#ifndef MTONLINE_GAMESERVER_MT_SKILL_LOGIC_H__
#define MTONLINE_GAMESERVER_MT_SKILL_LOGIC_H__
#include "../mt_types.h"
#include <map>
#include <vector>

namespace Mt
{
	class SkillEffect;
	struct SkillTarget;
	class MtSkillPositionEffect;
	class SkillLogicFactory : public boost::serialization::singleton<SkillLogicFactory>
	{
	public:
		const boost::shared_ptr<SkillEffect>& GetLogic(const std::string& name);
		const boost::shared_ptr<MtSkillPositionEffect>& GetPositionLogic(const std::string& name);
	private:
		const boost::shared_ptr<SkillEffect> CreateLogic(const std::string& name);
		const boost::shared_ptr<MtSkillPositionEffect> CreatePositionLogic(const std::string& name);
		std::map<std::string, boost::shared_ptr<SkillEffect>> logics_;
		std::map<std::string, boost::shared_ptr<MtSkillPositionEffect>> position_logics_;
	};

	class SkillEffect : public boost::noncopyable
	{
	public:
		virtual void Execute(MtActorSkill* skill) = 0;
	};
	
	class DirectDamage : public SkillEffect
	{
	public:
		virtual void Execute(MtActorSkill* skill) override;
	};

	class CureChain : public SkillEffect
	{
	public:
		virtual void Execute(MtActorSkill* skill) override;
	};
	
	//…¡µÁ¡¥
	class FlashChain : public DirectDamage
	{
	public:
		virtual void Execute(MtActorSkill* skill) override;
	};


	class DirectCure : public SkillEffect
	{
	public:
		virtual void Execute(MtActorSkill* skill) override;

	};
	/*class Dummy : public SkillExpression
	{
	public:
		virtual void Execute(const boost::shared_ptr<MtActor>& / *attacker* /,
			const std::vector<boost::shared_ptr<MtActor>>& / *targets* /,
			const boost::shared_ptr<MtActorSkill>& / *skill* /,
			zxero::int32& / *skill_normal_damage_for_buff* /,
			zxero::int32& / *skill_fixed_damage_for_buff* /) override;
	};

	class CreateActor : public SkillExpression
	{
	public:
		virtual void Execute(const boost::shared_ptr<MtActor>& attacker,
			const std::vector<boost::shared_ptr<MtActor>>& targets,
			const boost::shared_ptr<MtActorSkill>& skill,
			zxero::int32& skill_normal_damage_for_buff,
			zxero::int32& skill_fixed_damage_for_buff) override;
	};

	class CreateTrap : public SkillExpression // Õ∑≈œ›⁄Â
	{
	public:
		virtual void Execute(const boost::shared_ptr<MtActor>& attacker,
			const std::vector<boost::shared_ptr<MtActor>>& targets,
			const boost::shared_ptr<MtActorSkill>& skill,
			zxero::int32& skill_normal_damage_for_buff,
			zxero::int32& skill_fixed_damage_for_buff) override;
	};*/

	class MtSkillPositionEffect : public boost::noncopyable
	{
	public:
		virtual bool Execute(MtActorSkill* skill, uint64 elapseTime) = 0;
		virtual void StartLogic(MtActorSkill* skill) = 0;
		virtual int32 TimeCost(MtActorSkill* /*skill*/) {
			return 0;
		}
	};

	//≥Â∑Ê
	class MtSkillPositionCharge : public MtSkillPositionEffect
	{
	public:
		virtual bool Execute(MtActorSkill* skill, uint64 elapseTime) override;
		virtual void StartLogic(MtActorSkill* skill) override;
		virtual int32 TimeCost(MtActorSkill* skill) override;
	};

	//…¡œ÷
	class MtSkillPositionTeleport : public MtSkillPositionEffect
	{
	public:
		virtual bool Execute(MtActorSkill* skill, uint64 elapseTime) override;
		virtual void StartLogic(MtActorSkill* skill) override;
	};

	//◊•»°
	class MtSkillPositionCapture : public MtSkillPositionEffect
	{
	public:
		virtual bool Execute(MtActorSkill* skill, uint64 elapseTime) override;
		virtual void StartLogic(MtActorSkill* skill) override;
	};

	//ª˜∑…
	/*class MtSkillPositionHitOff : public MtSkillPositionEffect
	{
	public:
		virtual bool Execute(const boost::shared_ptr<MtActor>& attacker,
			const std::vector<MtActor*>& targets,
			const boost::shared_ptr<MtActorSkill>& skill,
			zxero::uint64 elapseTime) override
		{

		}
		virtual void StartLogic(const boost::shared_ptr<MtActor>& attacker,
			const std::vector<MtActor*>& targets,
			const boost::shared_ptr<MtActorSkill>& skill) override
		{
			std::vector<boost::shared_ptr<Packet::SkillPositionLogic>> msgs;
			msgs.reserve(targets.size());
			for (auto& target : targets)
			{
				auto msg = boost::make_shared<Packet::SkillPositionLogic>();
				msgs.push_back(msg);
				msg->set_type(Packet::SkillPositionLogicType::CAPTURE_LOGIC);
				msg->set_source_guid(target->Guid());
				msg->set_target_guid(attacker->Guid());
				msg->set_time_stamp(attacker->BattleGround()->TimeStamp());
				auto skill_info = msg->mutable_skill_info();
				skill_info->set_skill_id(skill->DbInfo()->skill_id());
				skill_info->set_skill_level(skill->DbInfo()->skill_level());
			}
			for (auto& message : msgs)
			{
				attacker->BattleGround()->BroadCastMessage(*message);
			}
		}
	};*/
}
#endif // MTONLINE_GAMESERVER_MT_SKILL_LOGIC_H__
