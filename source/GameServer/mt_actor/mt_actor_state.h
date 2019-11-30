#ifndef ZXERO_GAMESERVER_MT_ACTOR_AI_H__
#define ZXERO_GAMESERVER_MT_ACTOR_AI_H__

#include "../mt_types.h"
#include "BattleGroundInfo.pb.h"
#include "../mt_arena/geometry.h"


namespace Mt
{
	class MtActorState : public boost::noncopyable
	{
	public:
		MtActorState(const boost::shared_ptr<MtActor>& self,
			const boost::shared_ptr<MtActor>& target = nullptr)
			:self_(self), target_(target) {};
		virtual void OnTick(zxero::uint64 elapseTime) = 0;
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& /*strategy*/) { return false; }
		virtual Packet::ActorStateType State() const = 0;
		virtual bool Attackable(const boost::shared_ptr<MtActor>& /*attacker*/) { return true; }
	protected:
		boost::shared_ptr<MtActor> self_;
		boost::shared_ptr<MtActor> target_;
	};
	class MtAlwaysIdleState : public MtActorState {
	public:
		MtAlwaysIdleState(const boost::shared_ptr<MtActor>& self) : MtActorState(self) {}
		virtual void OnTick(zxero::uint64 /*elapseTime*/) override { return; }
		virtual Packet::ActorStateType State() const override { return Packet::ActorStateType::ACTOR_IDEL; }
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& /*strategy*/) override { return true; }
	};
	class MtActorIdleState : public MtActorState
	{
	public:
		MtActorIdleState(const boost::shared_ptr<MtActor>& self)
			:MtActorState(self) {}
		virtual void OnTick(zxero::uint64 elapseTime) override;
		virtual Packet::ActorStateType State() const override { return Packet::ActorStateType::ACTOR_IDEL; }
		//ֻ��ת��Ŀ��ѡ�������
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& strategy) override;
	private:
		zxero::int64 idle_time_ = 1000 * 1;
	};

	class MtActorSelectTarState : public MtActorState
	{
	public:
		MtActorSelectTarState(const boost::shared_ptr<MtActor>& self)
			: MtActorState(self){}
		virtual void OnTick(zxero::uint64 elapseTime) override;
		virtual Packet::ActorStateType State() const override { return Packet::ActorStateType::ACTOR_SELECT_TAR; }
		//����ת������,����, ����, �ƶ�
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& strategy);

	};

	class MtActorMoveState : public MtActorState
	{
	public:
		MtActorMoveState(const boost::shared_ptr<MtActor>& self,
			const boost::shared_ptr<MtActor>& target)
			:MtActorState(self, target) {}
		virtual void OnTick(zxero::uint64 elapseTime) override;
		virtual Packet::ActorStateType State() const override { return Packet::ActorStateType::ACTOR_MOVE; }
		//����ת������, ����, ����
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& strategy);
	private:
		bool moving = false;
	};

	class MtActorFightState : public MtActorState
	{
	public:
		MtActorFightState(const boost::shared_ptr<MtActor>& self,
			const boost::shared_ptr<MtActor>& target)
			:MtActorState(self, target) {}
		virtual void OnTick(zxero::uint64 elapseTime) override;
		virtual Packet::ActorStateType State() const override { return Packet::ActorStateType::ACTOR_FIGHT; }
		//����ת��Ŀ��ѡ�������
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& strategy);
	private:
		bool skill_cd_ = false;
	};
	class MtActorDeadState : public MtActorState
	{
	public:
		MtActorDeadState(const boost::shared_ptr<MtActor>& self,
			const boost::shared_ptr<MtActor>& target)
			:MtActorState(self, target) {}
		//��ǰû�и���
		virtual void OnTick(zxero::uint64 /*elapseTime*/) override { return; }
		virtual bool Attackable(const boost::shared_ptr<MtActor>& /*attacker*/) { return false; }
		virtual Packet::ActorStateType State() const override { return Packet::ActorStateType::ACTOR_DEAD; }
		//��ǰû�и����, �����޷��ı�
		virtual bool EnterState(const boost::shared_ptr<MtActorState>& /*strategy*/) { return false; }
	};
}
#endif // ZXERO_GAMESERVER_MT_ACTOR_AI_H__
