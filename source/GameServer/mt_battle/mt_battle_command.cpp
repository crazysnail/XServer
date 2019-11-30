#include "mt_battle_command.h"
#include "mt_battle_ground.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_actor/mt_actor_state.h"
#include "../mt_skill/mt_skill.h"
#include "BattleExpression.pb.h"
#include <SkillConfig.pb.h>
#include <boost/make_shared.hpp>


namespace Mt
{

	/*SkillCommand::SkillCommand(const boost::shared_ptr<MtActor>& attacker, const boost::shared_ptr<MtActor>& defender,
		const boost::shared_ptr<MtActorSkill>& skill)
		:BattleCommand(attacker->BattleGround()->TimeStamp()), attacker_(attacker), defender_(defender), skill_(skill)
	{
		auto distance = zxero::distance2d(attacker->Position(), defender->Position());
		perpare_time_ = skill->SkillConfig()->prefix_time();
		if (skill->SkillConfig()->bullet_speed() > 0) {//有子弹
			perpare_time_ += (zxero::int64) (distance / skill->SkillConfig()->bullet_speed() * 1000);
			LOG_INFO << "skill with bullet:" << perpare_time_;
		}
		suffix_time_ = skill->SkillConfig()->cd_time();
	}

	CommandResult SkillCommand::Execute(const boost::shared_ptr<MtBattleGround>& battle_ground, zxero::uint64 elapseTime)
{
	if (attacker_->CanDo(Config::BattleObjectAbility::USE_SKILL) && defender_->Attackable(attacker_)) {
		//LOG_INFO << "MtAcotr" << attacker_ << "skill command " << perpare_time_ << "," << suffix_time_;
		//SEND PACKET, apply skill
		if (perpare_time_ > 0) { //技能前摇
			perpare_time_ -= elapseTime;
			if (perpare_time_ <= 0) {
				auto damage_data = boost::make_shared<Packet::ActorOnDamage>();
				damage_data->set_source_guid(attacker_->Guid());
				damage_data->set_target_guid(defender_->Guid());
				damage_data->set_type(Packet::DamageType::PHYSICAL_TO_PHYSICAL);
				damage_data->set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
				auto skill_info = damage_data->mutable_skill();
				skill_info->set_skill_id(skill_->SkillConfig()->id());
				skill_info->set_skill_level(skill_->DbInfo()->skill_level());
				Packet::ActorUseSkill notify;
				notify.set_source_guid(attacker_->Guid());
				notify.add_target_guid(defender_->Guid());
				notify.set_time_stamp(time_stamp_);
				notify.mutable_skill()->CopyFrom(*skill_info);
				battle_ground->BroadCastMessage(notify);
				attacker_->SendCommand(boost::make_shared<DamageCommand>(attacker_, defender_, damage_data));
				return CommandResult::COMMAND_RUNNING;
			}
			return CommandResult::COMMAND_RUNNING;
 		}  else if (suffix_time_ > 0) {// 技能后摇
			suffix_time_-= elapseTime;
			return CommandResult::COMMAND_RUNNING;
		}
		else {
			//LOG_INFO << "MtActor" << attacker_ << "skill action done, select target again";
			attacker_->EnterState(boost::make_shared<MtActorSelectTarState>(attacker_));
			return CommandResult::COMMAND_DONE;
		}
	}
	else {
		LOG_INFO << "MtActor" << attacker_ << " skill command can not execute";
		return CommandResult::COMMAND_DONE;
	}
}

CommandResult BuffCommand::Execute(const boost::shared_ptr<MtBattleGround>& / *battle_ground* /, zxero::uint64 elapseTime)
{
	if (last_time_ - elapseTime > 0) {
		last_time_ -= elapseTime;
		return CommandResult::COMMAND_RUNNING;
	}
	else {
		if (effect_times_-- > 0) {
			last_time_ = 2 * 1000;
			return CommandResult::COMMAND_RUNNING;
		}
		else {
			return CommandResult::COMMAND_DONE;
		}
	}
}

DamageCommand::DamageCommand(const boost::shared_ptr<MtActor>& attacker, const boost::shared_ptr<MtActor>& defener, const boost::shared_ptr<Packet::ActorOnDamage>& damage) : BattleCommand(attacker->BattleGround()->TimeStamp()),
attacker_(attacker), defender_(defener), damage_(damage)
{

}

CommandResult DamageCommand::Execute(const boost::shared_ptr<MtBattleGround>& battle_ground, zxero::uint64 / *elapseTime* /)
{
	damage_->set_time_stamp(time_stamp_);
	attacker_->ReviseDamage(*damage_, defender_);
	if (defender_->OnDamage(*damage_, attacker_)) {
		defender_->SendCommand(boost::make_shared<DeadCommand>(attacker_, defender_));
	}
	battle_ground->BroadCastMessage(*damage_);
	return CommandResult::COMMAND_DONE;
}

DeadCommand::DeadCommand(const boost::shared_ptr<MtActor>& attacker, const boost::shared_ptr<MtActor>& defener) :BattleCommand(attacker->BattleGround()->TimeStamp()),
attacker_(attacker), defender_(defener)
{

}

CommandResult DeadCommand::Execute(const boost::shared_ptr<MtBattleGround>& battle_ground, zxero::uint64 / *elapseTime* /)
{
	if (defender_->IsDead()) {
		return CommandResult::COMMAND_DONE;
	}
	defender_->EnterState(boost::make_shared<MtActorDeadState>(attacker_, defender_));
	battle_ground->OnActorDead(defender_, attacker_);
	Packet::ActorDead notify;
	notify.set_source_guid(attacker_->Guid());
	notify.set_target_guid(defender_->Guid());
	notify.set_time_stamp(time_stamp_);
	battle_ground->BroadCastMessage(notify);
	return CommandResult::COMMAND_CANCLE_ALL;
}

MoveCommand::MoveCommand(const boost::shared_ptr<MtActor>& source, const boost::shared_ptr<MtActor>& target, const Packet::Position& target_pos) 
	:BattleCommand(source->BattleGround()->TimeStamp()),
	source_(source), target_(target), target_pos_(target_pos)
{
	Packet::ActorMoveToTarget notify;
	notify.set_source_guid(source_->Guid());
	notify.set_target_guid(target_->Guid());
	notify.set_time_stamp(time_stamp_);
	notify.set_move_speed(source_->MoveSpeed());
	notify.set_stop_range(source_->AttackRadius() + target_->ObjectRadius());
	source_->BattleGround()->BroadCastMessage(notify);
}

Mt::CommandResult MoveCommand::Execute(const boost::shared_ptr<MtBattleGround>& / *battle_ground* /, zxero::uint64 elapseTime)
{
	source_->Direction(zxero::get_direction(source_->Position(), target_->Position()));
	auto curr_distance = zxero::distance2d(source_->Position(), target_->Position());
	if (curr_distance <= source_->AttackRadius() + target_->ObjectRadius()) {//不需要移动
		source_->EnterState(boost::make_shared<MtActorFightState>(source_, target_));
		return CommandResult::COMMAND_DONE;
	}
	auto move_distance = int(source_->MoveSpeed() * elapseTime / 1000.f);
	auto new_pos = zxero::move_point(source_->Position(), move_distance, source_->Direction());
	auto new_distance = zxero::distance2d(new_pos, target_->Position());
	if (new_distance < target_->ObjectRadius() + source_->AttackRadius()) {
		new_pos = zxero::move_point(source_->Position(), curr_distance + 1 - target_->ObjectRadius() - source_->AttackRadius(), source_->Direction());
		source_->Position(new_pos);
		source_->EnterState(boost::make_shared<MtActorFightState>(source_, target_));
		return CommandResult::COMMAND_DONE;
	} else {//继续移动
		source_->Position(new_pos);
		return CommandResult::COMMAND_RUNNING;
	}
}*/

}