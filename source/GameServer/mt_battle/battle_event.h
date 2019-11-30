#ifndef MTONLINE_GAMESERVER_BATTLE_EVENT_H__
#define MTONLINE_GAMESERVER_BATTLE_EVENT_H__
#include "../mt_types.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_skill/mt_buff.h"

namespace Mt
{
	struct Event
	{
		virtual void HandleEvent() = 0;
		boost::weak_ptr<MtBattleGround> bg_;
	};

	struct AddBuffEvent : public Event
	{
		virtual void HandleEvent() override;
		boost::shared_ptr<BuffImpl::Buff> buff_;
		uint64 target_guid_;
	};
	
	struct TransformEvent : public Event
	{
		virtual void HandleEvent() override;
		boost::shared_ptr<BuffImpl::Transform> transform_buff_;
	};

	struct ClearSkillEvent : public Event
	{
		virtual void HandleEvent() override;
		uint64 actor_guid_ = INVALID_GUID;
	};
	struct AddSkillEvent : public Event
	{
		virtual void HandleEvent() override;
		uint64 actor_guid_ = INVALID_GUID;
		std::vector<std::pair<int32, int32>> id_and_levels_;
	};
	struct TransformCancel : public Event
	{
		virtual void HandleEvent() override;
		int32 new_physical_attack_ = -1;
		int32 old_physical_attack_ = -1;
		int32 new_magic_attack_ = -1;
		int32 old_magic_attack_ = -1;
		uint64 transformer_guid_ = INVALID_GUID;
	};

	struct DelayDead : public Event
	{
		virtual void HandleEvent() override;
		uint64 dead_guid_ = INVALID_GUID;
		uint64 killer_guid_ = INVALID_GUID;
	};

	struct UseSkillEvent : public Event
	{
		virtual void HandleEvent() override;
		uint64 owner_guid_ = INVALID_GUID;
		int32 skill_id_ = 0;
		std::vector<uint64> target_ids_;
	};

	struct DeleteActorEvent : public Event 
	{
		virtual void HandleEvent() override;
		std::set<uint64> guids_;

	};
}
#endif // MTONLINE_GAMESERVER_BATTLE_EVENT_H__
