#include "mt_actor.h"
#include "mt_actor_state.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_battle/mt_battle_command.h"
#include <boost/make_shared.hpp>

namespace Mt
{
	void MtActorIdleState::OnTick(zxero::uint64 elapseTime)
	{
		if (idle_time_ > 0) {
			idle_time_ -= elapseTime;
		}
		else {
			ZXERO_ASSERT(self_->EnterState(boost::make_shared<MtActorSelectTarState>(self_)));
		}
	}

	bool MtActorIdleState::EnterState(const boost::shared_ptr<MtActorState>& strategy)
	{
		if (strategy->State() == Packet::ActorStateType::ACTOR_SELECT_TAR
			|| strategy->State() == Packet::ActorStateType::ACTOR_IDEL) {
			return true;
		}
		else {
			return false;
		}
	}

	void MtActorSelectTarState::OnTick(zxero::uint64 /*elapseTime*/)
	{
	/*	ZXERO_ASSERT(target_ == nullptr) << "enter select tar, target_ not null";
		//TODO　选择一个技能之后才有攻击距离, 才能选择目标
		const boost::shared_ptr<MtBattleGround> battle_ground = self_->BattleGround();
		auto enemies = battle_ground->Enemies(self_, [&](auto& actor) { return actor->Attackable(self_); });
		auto enemy = std::min_element(std::begin(enemies), std::end(enemies), [&](auto& lhs, auto& rhs) {
			auto lhs_distance = zxero::distance2d(lhs->Position(), self_->Position());
			auto rhs_distance = zxero::distance2d(rhs->Position(), self_->Position());
			return lhs_distance < rhs_distance;
		});
		if (enemy != std::end(enemies)) {//找到目标了
			auto distance = zxero::distance2d(self_->Position(), (*enemy)->Position());
			if (distance <= self_->AttackRadius() + (*enemy)->ObjectRadius()) {
				//LOG_INFO << "MtActor" << self_ << "found target and in attack radius attack";
				self_->EnterState(boost::make_shared<MtActorFightState>(self_, *enemy));
			} else {
				//LOG_INFO << "MtActor" << self_ << "found target and but not in attack radius. move";
				self_->EnterState(boost::make_shared<MtActorMoveState>(self_, *enemy));
			}
		}
		else { // 没有目标, 对面死完了?
			self_->EnterState(boost::make_shared<MtActorIdleState>(self_));
		}*/
	}


	bool MtActorSelectTarState::EnterState(const boost::shared_ptr<MtActorState>& strategy)
	{
		if (strategy->State() == Packet::ActorStateType::ACTOR_MOVE
			|| strategy->State() == Packet::ActorStateType::ACTOR_FIGHT
			|| strategy->State() == Packet::ActorStateType::ACTOR_DEAD
			|| strategy->State() == Packet::ActorStateType::ACTOR_IDEL) {
			return true;
		}
		else {
			return false;
		}
	}

	void MtActorMoveState::OnTick(zxero::uint64 /*elapseTime*/)
	{
		/*if (target_ != nullptr && target_->Attackable(self_)) {
			if (moving){
				//waiting 
			} else {
				self_->Direction(zxero::get_direction(self_->Position(), target_->Position()));
				auto distance = zxero::distance2d(self_->Position(), target_->Position());
				auto target_pos = zxero::move_point(self_->Position(), distance - self_->AttackRadius() - target_->ObjectRadius(), self_->Direction());
/ *
				auto command = boost::make_shared<MoveCommand>(self_, target_, target_pos);
				self_->SendCommand(command);* /
				moving = true;
			}
		}
		else {//目标无法攻击了, 重新选择
			self_->EnterState(boost::make_shared<MtActorSelectTarState>(self_));
		}*/
	}

	bool MtActorMoveState::EnterState(const boost::shared_ptr<MtActorState>& strategy)
	{
		if (strategy->State() == Packet::ActorStateType::ACTOR_FIGHT
			|| strategy->State() == Packet::ActorStateType::ACTOR_DEAD
			|| strategy->State() == Packet::ActorStateType::ACTOR_IDEL) {
			return true;
		}
		else {
			return false;
		}
	}

	void MtActorFightState::OnTick(zxero::uint64 /*elapseTime*/)
	{
	/*	if (target_ != nullptr && target_->Attackable(self_)) {
			if (skill_cd_) {
				//Waiting
			} else {
				LOG_INFO << "MtActor" << self_ << "send skill command";
/ *				self_->SendCommand(boost::make_shared<SkillCommand>(self_, target_, self_->GetSkill()));* /
				skill_cd_ = true;
			}
		}
		else {//目标无法攻击了, 重新选择
			self_->EnterState(boost::make_shared<MtActorSelectTarState>(self_));
		}*/
	}

	bool MtActorFightState::EnterState(const boost::shared_ptr<MtActorState>& strategy)
	{
		if (strategy->State() == Packet::ActorStateType::ACTOR_SELECT_TAR
			|| strategy->State() == Packet::ActorStateType::ACTOR_DEAD) {
			return true;
		}
		else {
			return false;
		}
	}

}