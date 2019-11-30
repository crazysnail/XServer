#include "mt_battle_state.h"
#include "mt_actor.h"
#include "date_time.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_skill/skill_context.h"
#include "../mt_battle/mt_battle_ground.h"
#include "random_generator.h"
#include <SkillConfig.pb.h>
#include <BattleExpression.pb.h>

namespace Mt
{
namespace BattleState
{

	MtActor* BaseState::Owner()
	{
		return actor_;
	}

	void Idle::OnTick(uint64 elapseTime)
	{
		MtActor* owner = Owner();
		if (owner == nullptr) return;
		if (owner->CanUseSkill() == false) return;

		if (wait_ > 0) {
			wait_ -= int64(elapseTime);
		} else {
			owner->BattleState(boost::make_shared<SelectSkill>(owner));
		}
	}

	void SelectSkill::OnTick(uint64 /*elapseTime*/)
	{
		MtActor* self = Owner();
		if (self == nullptr) return;
		if (self->CanUseSkill() == false) {
			self->BattleState(boost::make_shared<Idle>(self));
			return;
		}
		if (self->CanChangeTarget()) {
			self->ActorTargets().Reset();
		}
		if (self->SkillListSize() > 0) {
			self->BattleState(boost::make_shared<SelectMoveToTarget>(self));
			return; //进入下一个状态
		}
		auto& skills = self->Skills();
		auto curr_index = self->CurrSkillIndex();
		auto skill_build = self->SkillBuild();

		if (skill_build == nullptr) {//随机使用技能
			std::vector<int32> skill_ids;
			for (auto& skill : skills) {
				if (skill->IsIdle() && skill->DirectUsable())
					skill_ids.push_back(skill->SkillId());
			}
			if (skill_ids.size() == 0) {
				self->BattleState(boost::make_shared<Idle>(self));
				return;
			}
			auto skill_index = rand_gen->intgen(0, skill_ids.size() - 1);
			self->PushSkill(skill_ids[skill_index], {});
			SkillContext ctx;
			self->AfterSelectSkill(ctx);
			self->BattleState(boost::make_shared<SelectMoveToTarget>(self));
			return;
		} else { //根据skill build 走
			if (curr_index >= skill_build->skill_ids_size()) {
				ZXERO_ASSERT(false);
				self->BattleState(boost::make_shared<Idle>(self));
				return;
			} else {
				auto skill_id = skill_build->skill_ids(curr_index);
				auto new_skill_it = std::find_if(std::begin(skills), std::end(skills), [=](auto skill) {
					return skill->SkillId() == skill_id && skill->IsIdle() && skill->DirectUsable();
				});
				SkillContext ctx;
				if (new_skill_it != std::end(skills)) {
					self->PushSkill(skill_id, {});
					self->AfterSelectSkill(ctx);
					self->BattleState(boost::make_shared<SelectMoveToTarget>(self));
					return;
				} else {
					//LOG_WARN << "skill build dismatch with actor skills";
					auto skill_it = std::find_if(std::begin(skills), std::end(skills),
						[](auto& skill) { return skill->IsIdle() && skill->DirectUsable(); });
					if (skill_it != std::end(skills)) {
						self->PushSkill((*skill_it)->SkillId(), {});
						self->AfterSelectSkill(ctx);
						self->BattleState(boost::make_shared<SelectMoveToTarget>(self));
						return;
					} else {
						self->BattleState(boost::make_shared<Idle>(self));
						return;
					}
				}
			}
		}
	}

	void SelectMoveToTarget::OnTick(uint64 /*elapseTime*/)
	{
		MtActor* self = Owner();
		if (self == nullptr) return;
		auto battle_ground = self->BattleGround();
		if (self->CurrSkill() != nullptr) {
			SkillTarget targets = self->ActorTargets();
			if (self->CanChangeTarget()) {
				targets.targets_ = self->CurrSkillTarget();
				if (targets.targets_.empty()) {
					auto new_target = self->CurrSkill()->SelectMoveTarget();
					if (new_target != INVALID_GUID) {
						targets.targets_.push_back(new_target);
					}
				}
			}
			auto target = battle_ground->FindActor(targets.FirstTargetGuid());
			if (target != nullptr) {
				self->SetMoveTowardTargetGuid(target->Guid());
				auto curr_distance = zxero::distance2d(self->Position(), target->Position());
				if (curr_distance <= self->AttackRadius() + target->ObjectRadius()) {
					self->BattleState(boost::make_shared<SelectSkillTarget>(self));
					return;
				} else {
					self->BattleState(boost::make_shared<MoveToTarget>(self));
					return;
				}
			}
		}
		self->PopSkill();
		self->BattleState(boost::make_shared<Idle>(self));
	}

	void MoveToTarget::OnTick(uint64 elapseTime)
	{
		MtActor* self = Owner();
		if (self == nullptr) return;

		if (self->CanMove() == false) {
			self->BattleState(boost::make_shared<Idle>(self));
			//LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << "idel";
			return;
		}
		auto target = self->BattleGround()->FindActor(self->MoveTowardTargetGuid());
		if (target == nullptr || target->DbInfo() == nullptr ) {
			ZXERO_ASSERT(false);
			self->BattleState(boost::make_shared<Idle>(self));
			return;
		}
		if (target->Guid() == self->Guid()) {
			self->BattleState(boost::make_shared<SelectSkillTarget>(self));
			//LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << "idel2";
			return;
		}
		/*LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << " tick."
			<< "waiter:" << waiter_ << ".move_by_clinet:" << self->BattleGround()->MoveByClient()
			<< ".moving:" << self->GetState(Config::BattleObjectStatus::MOVING)
			<< ".x" << self->Position().x()
			<< ".z" << self->Position().z();*/
		self->Direction(zxero::get_direction(self->Position(), target->Position()));
		if (self->GetState(Config::BattleObjectStatus::MOVING)) { //移动中, 继续执行
																	/*auto bg = self->BattleGround();
			waiter_ = waiter_ - elapseTime;
			if (bg->MoveByClient() && waiter_ > 0) {
				if (self->ClientMoveFinished()) {
					self->ClearState(Config::BattleObjectStatus::MOVING);
					self->BattleState(boost::make_shared<SelectSkillTarget>(self));
					LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << "sst";
					return;
				} else {
					LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << " waiting";
					return;
				}
			} else {*/
			auto curr_distance = zxero::distance2d(self->Position(), target->Position());
			if (curr_distance <= self->AttackRadius() + target->ObjectRadius()) {//不需要移动
				self->ClearState(Config::BattleObjectStatus::MOVING);
				self->BattleState(boost::make_shared<SelectSkillTarget>(self));
				//LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << "sst2";
				return;
			}
			auto move_distance = int(self->MoveSpeed() * int32(elapseTime) / 1000.f);
			auto new_pos = zxero::move_point(self->Position(), move_distance, self->Direction());
			auto new_distance = zxero::distance2d(new_pos, target->Position());
			if (new_distance <  self->AttackRadius() + target->ObjectRadius() || new_distance > curr_distance) {
				auto dir = zxero::get_direction(target->Position(), self->Position());
				new_pos = zxero::move_point(target->Position(), self->AttackRadius() + target->ObjectRadius(), dir);
				self->Position(new_pos);
				self->ClearState(Config::BattleObjectStatus::MOVING);
				//LOG_INFO << self << self->ConfigId() << ". move to:" << target->ConfigId() << " end";
				self->BattleState(boost::make_shared<SelectSkillTarget>(self));
				//LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << "sst3";
				return;
			} else {//继续移动
				self->Position(new_pos);
				return;
			}
			/*			}*/
		} else {
			waiter_ = seconds(3).total_milliseconds();
			self->AddState(Config::BattleObjectStatus::MOVING);
			self->ClientMoveStart();
			//LOG_INFO << self << "," << self->ConfigId() << ". move serial:" << self->MoveSerial() << " start";
			Packet::ActorMoveToTarget move_message;
			move_message.set_source_guid(self->Guid());
			move_message.set_target_guid(target->Guid());
			move_message.set_stop_range(self->AttackRadius() + target->ObjectRadius());
			move_message.set_move_speed(self->MoveSpeed());
			move_message.set_move_serial(self->MoveSerial());
			move_message.set_time_stamp(self->BattleGround()->TimeStamp());
			self->BattleGround()->BroadCastMessage(move_message);
			return;
		}
	}

	void SelectSkillTarget::OnTick(uint64 /*elapseTime*/)
	{
		MtActor* self = Owner();
		if (self == nullptr) return;

		auto skill = self->CurrSkill();
		if (skill == nullptr) {
			self->BattleState(boost::make_shared<Idle>(self));
			return;
		}
		SkillTarget targets = self->ActorTargets();
		if (targets.targets_.empty()) {//当前没有固定目标, 需要重新选择
			targets.targets_ = self->CurrSkillTarget();
			if (targets.targets_.empty()) {
				skill->SelectSkillTarget(&targets);
			}
		} else {//有固定目标, 检查可否重新选择
			if (self->CanChangeTarget()) {
				targets.targets_ = self->CurrSkillTarget();
				if (targets.targets_.empty()) {
					skill->SelectSkillTarget(&targets);
				}
			}
		}
		if (targets.targets_.empty() == false) {
			self->CurrSkill()->SetSkillTarget(targets);
			self->BattleState(boost::make_shared<UseSkill>(self));
			return;
		}
		self->PopSkill();
		self->BattleState(boost::make_shared<Idle>(self));
		return;
	}

	void UseSkill::OnTick(uint64 /*elapseTime*/)
	{
		auto self = Owner();
		if (self == nullptr) return;
		auto skill_build = self->SkillBuild();
		if (skill_build) {
			auto curr_index = self->CurrSkillIndex();
			auto new_skill_index = (curr_index + 1) % skill_build->skill_ids_size();
			self->CurrSkillIndex(new_skill_index);
		}
		auto skill = self->CurrSkill();
		if (skill == nullptr) {
			self->BattleState(boost::make_shared<Idle>(self));
			return;
		}
		if (skill != nullptr) {
			do 
			{
				if (self->CanUseSkill() == false) {
					break;
				}
				if (skill->BaseSkill() == false && self->CanUseMagicSkill() == false) {
					break;

				}
				if (skill->BaseSkill() && self->CanUseBaseSkill() == false) {
					break;
				}
				if (skill->Started() == false) {
					skill->Start();
				}
			} while (false);	
		}
		self->BattleState(boost::make_shared<WaitSkillFinish>(self));
		return;
	}

	void WaitSkillFinish::OnTick(uint64 /*elapseTime*/)
	{
		MtActor* self = Owner();
		auto skill = self->CurrSkill();
		if (skill== nullptr || skill->IsIdle() || skill->IsCoolDown()) {
			//LOG_INFO << self << self->ConfigId() << ". skill done" << skill->SkillId();
			self->PopSkill();
			self->BattleState(boost::make_shared<Idle>(self));
			return;
		}
		return;
	}


	HitOff::HitOff(MtActor* actor, MtActor* attacker,
		const MtActorSkill* skill) : base(actor)
	{
		Packet::SkillPositionLogic msg;
		msg.set_type(Packet::SkillPositionLogicType::HITOFF_LOGIC);
		msg.set_source_guid(attacker->Guid());
		msg.set_target_guid(actor->Guid());
		msg.set_move_speed(4000 * 2);
		msg.set_time_stamp(actor->BattleGround()->TimeStamp());
		move_range_ = 2000;//默认击飞两米
		if (!skill->ActiveConfig()["hit_off"].empty()) {
			move_range_ = skill->ActiveConfig()["hit_off"]["distance"].asInt();
		}
		msg.set_range(move_range_);
		auto skill_info = msg.mutable_skill_info();
		skill_info->set_skill_id(skill->SkillId());
		skill_info->set_skill_level(skill->SKillLevel());
		actor->BattleGround()->BroadCastMessage(msg);
		auto dir = zxero::get_direction(attacker->Position(), actor->Position());
		new_pos_ = zxero::move_point(actor->Position(), move_range_, dir);
	}

	void HitOff::OnTick(uint64 elapseTime)
	{
		int32 move = int32(4000 * (elapseTime / 1000.f));
		move_range_ -= int32(move);
		if (move_range_ <= 0) {
			Owner()->Position(new_pos_);
			Owner()->BattleState(boost::make_shared<Idle>(Owner()));
		}
	}


	void BeSneer::OnTick(uint64 /*elapseTime*/)
	{
		MtActor* self = Owner();
		if (self->ActorTargets().FirstTargetGuid() != sneer_source_) {
			self->ActorTargets().Reset();
			self->ActorTargets().targets_.push_back(sneer_source_);
			if (self->CurrSkill() != nullptr) {
				self->CurrSkill()->SetReuse();
			}
		}
		if (self->GetState(Config::BattleObjectStatus::MOVING)) {
			self->ClearState(Config::BattleObjectStatus::MOVING);
		}
		self->BattleState(boost::make_shared<BattleState::Idle>(self));
	}

}
}