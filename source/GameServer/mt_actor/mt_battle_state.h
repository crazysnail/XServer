#ifndef MTONLINE_GAMESERVER_MT_BATTLE_STATE_H__
#define MTONLINE_GAMESERVER_MT_BATTLE_STATE_H__
#include "../mt_types.h"
#include <Base.pb.h>

namespace Mt
{
	namespace BattleState
	{
		class BaseState : public boost::noncopyable
		{
		public:
			BaseState(MtActor* actor):actor_(actor) {}
			virtual void OnTick(uint64 elapseTime) = 0;
			MtActor* Owner();
		private:
			MtActor* actor_;
		};

		class Idle : public BaseState
		{//开始状态
		public:
			typedef BaseState base;
			Idle(MtActor* actor, int64 wait = 0) :base(actor), wait_(wait) {}
			virtual void OnTick(uint64 elapseTime) override;
		private:
			int64 wait_ = 0;
		};

		class SelectSkill : public BaseState 
		{//选择技能,选择移动目标
		public:
			typedef BaseState base;
			SelectSkill(MtActor* actor) :base(actor) {}
			virtual void OnTick(uint64 elapseTime) override;
		};

		class SelectMoveToTarget : public BaseState
		{
		public:
			typedef BaseState base;
			SelectMoveToTarget(MtActor* actor) : base(actor) {}
			virtual void OnTick(uint64 elapseTime) override;
		};

		class MoveToTarget : public BaseState
		{//向目标移动
		public:
			typedef BaseState base;
			MoveToTarget(MtActor* actor) :base(actor) {}
			virtual void OnTick(uint64 elapseTime) override;
		private:
			int64 waiter_ = 0;
		};

		class SelectSkillTarget : public BaseState
		{//抵达目标点后, 重新选择目标
		public:
			typedef BaseState base;
			SelectSkillTarget(MtActor* actor) :base(actor){}
			virtual void OnTick(uint64 elapseTime) override;
		};

		class UseSkill : public BaseState 
		{
		public:
			typedef BaseState base;
			UseSkill(MtActor* actor) :base(actor) {}
			virtual void OnTick(uint64 elapseTime) override;
		private:
			int64 waiter_ = 0;
		};

		class WaitSkillFinish : public BaseState
		{
		public:
			typedef BaseState base;
			WaitSkillFinish(MtActor* actor) :base(actor) {}
			virtual void OnTick(uint64 elapseTime) override;
		};
		
		class HitOff : public BaseState
		{
		public:
			typedef BaseState base;
			HitOff(MtActor* actor, MtActor* attacker,
				const MtActorSkill* skill);
			virtual void OnTick(uint64 elapseTime) override;
		private:
			Packet::Position new_pos_;
			int32 move_range_ = 0;
		};

		class BeSneer : public BaseState
		{
		public:
			typedef BaseState base;
			BeSneer(MtActor* actor, uint64 sneer_srouce)
				:base(actor), sneer_source_(sneer_srouce) {}
			BeSneer() = delete;
			virtual void OnTick(uint64 elapseTime) override;
		private:
			uint64 sneer_source_;
		};
	}

}
#endif // MTONLINE_GAMESERVER_MT_BATTLE_STATE_H__
