#include "battle_event.h"
#include "../mt_skill/skill_context.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_monster/mt_monster_manager.h"
#include <BattleExpression.pb.h>
#include <MonsterConfig.pb.h>

namespace Mt
{
	void AddBuffEvent::HandleEvent()
	{
		if (bg_.expired()) return;
		auto target = bg_.lock()->FindActor(target_guid_);
		if (target == nullptr) return;
		if (target->IsDead())
			return;
		target->AddBuff(buff_);
	}


	void TransformEvent::HandleEvent()
	{
		auto monster_config = MtMonsterManager::Instance().FindMonster(transform_buff_->config_id_);
		if (monster_config == nullptr)
		{
			LOG_WARN << "transform event can not find monster config with id :" << transform_buff_->config_id_;
			return;
		}
		if (bg_.expired()) return;
		auto buff_owner = transform_buff_->GetOwner();
		if (buff_owner == nullptr) return;
		Packet::ActorTransform tran_msg;
		tran_msg.set_source_guid(transform_buff_->GetOwnerGuid());
		tran_msg.set_new_res_id(monster_config->id());
		tran_msg.set_new_hp(transform_buff_->new_hp_);
		tran_msg.set_radius(int32(monster_config->radius()*1000));
		tran_msg.set_new_max_hp(buff_owner->MaxHp());
		tran_msg.set_time_stamp(buff_owner->BattleGround()->TimeStamp());
		buff_owner->BattleGround()->BroadCastMessage(tran_msg);
		if (transform_buff_->new_physical_attack_ != -1) {
			transform_buff_->old_physical_attack_ = buff_owner->RunTimeBattleInfo()->physical_attack();
			buff_owner->RunTimeBattleInfo()->set_physical_attack(transform_buff_->new_physical_attack_);
		}
		if (transform_buff_->new_magic_attack_ != -1) {
			transform_buff_->old_magic_attack_ = buff_owner->RunTimeBattleInfo()->physical_attack();
			buff_owner->RunTimeBattleInfo()->set_physical_attack(transform_buff_->new_magic_attack_);
		}
		buff_owner->RunTimeBattleInfo()->set_hp(transform_buff_->new_hp_);
		buff_owner->Transform(monster_config.get());
	}


	void TransformCancel::HandleEvent()
	{
		if (bg_.expired()) return;
		auto bg = bg_.lock();
		auto transformer = bg->FindActor(transformer_guid_);
		if (transformer == nullptr) return;
		if (new_physical_attack_ != -1)
			transformer->RunTimeBattleInfo()->set_physical_attack(old_physical_attack_);
		if (new_magic_attack_ != -1)
			transformer->RunTimeBattleInfo()->set_magic_attack(old_magic_attack_);
		transformer->TransformCancel();
		Packet::ActorTransformCancel msg;
		msg.set_source_guid(transformer->Guid());
		msg.set_radius(transformer->ObjectRadius());
		msg.set_time_stamp(transformer->BattleGround()->TimeStamp());
		transformer->BattleGround()->BroadCastMessage(msg);
	}


	void DelayDead::HandleEvent()
	{
		if (bg_.expired()) return;
		auto bg = bg_.lock();
		auto dead_one = bg->FindActor(dead_guid_);
		if (dead_one == nullptr)
			return;
		auto killer = bg->FindActor(killer_guid_);
		dead_one->OnDead(killer);
	}

	void DeleteActorEvent::HandleEvent()
	{
		if (bg_.expired()) return;
		auto bg = bg_.lock();
		for (auto guid : guids_)
			bg->DeleteActorNow(guid);
	}

	void UseSkillEvent::HandleEvent()
	{
		if (bg_.expired()) return;
		auto owner = bg_.lock()->FindActor(owner_guid_);
		if (owner == nullptr) return;
		owner->PushSkill(skill_id_, target_ids_);
	}


	void ClearSkillEvent::HandleEvent()
	{
		if (bg_.expired()) return;
		auto target = bg_.lock()->FindActor(actor_guid_);
		if (target == nullptr) return;
		target->DoClearRunTimeSkill();
	}


	void AddSkillEvent::HandleEvent()
	{
		if (bg_.expired()) return;
		auto target = bg_.lock()->FindActor(actor_guid_);
		if (target == nullptr) return;
		for (auto& id_and_level : id_and_levels_) {
			target->DoAddRuntimeSkill(id_and_level.first, id_and_level.second);
		}

	}

}