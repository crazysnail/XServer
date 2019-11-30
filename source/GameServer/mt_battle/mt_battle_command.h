#ifndef ZXERO_GAMESERVER_MT_BATTLE_COMMAND_H__
#define ZXERO_GAMESERVER_MT_BATTLE_COMMAND_H__

#include "../mt_types.h"
#include "../mt_arena/geometry.h"

namespace Mt
{
	class MtBattleGround;
	/*enum class CommandResult {
		COMMAND_RUNNING,		//正在执行
		COMMAND_DONE,			//执行完毕
		COMMAND_CANCLE_ALL,		//取消所有
	};
	enum class CommandType {
		COMMAND_SKILL,
		COMMAND_BUFF,
		COMMAND_MOVE,
		COMMAND_DAMAGE,
		COMMAND_DEAD,
	};
	//执行命令
	class BattleCommand : public boost::noncopyable
	{
	public:
		BattleCommand(zxero::uint64 time_stamp) :time_stamp_(time_stamp) {}
		virtual CommandResult Execute(const boost::shared_ptr<MtBattleGround>& battle_ground, zxero::uint64 elapseTime) = 0;
		virtual CommandType Type() const = 0;
		zxero::uint64 TimeStamp() const { return time_stamp_; }
		void TimeStamp(zxero::uint64 time_stamp) { time_stamp_ = time_stamp; }
	protected:
		zxero::uint64 time_stamp_;
	};

	class SkillCommand : public BattleCommand
	{
	public:
		SkillCommand(const boost::shared_ptr<MtActor>& attacker,
			const boost::shared_ptr<MtActor>& defender,
			const boost::shared_ptr<MtActorSkill>& skill);
		virtual CommandResult Execute(const boost::shared_ptr<MtBattleGround>& / *battle_ground* /, zxero::uint64 / *elapseTime* /) override;
		virtual CommandType Type() const override { return CommandType::COMMAND_SKILL; }
	private:
		boost::shared_ptr<MtActor> attacker_;
		boost::shared_ptr<MtActor> defender_;
		boost::shared_ptr<MtActorSkill> skill_;
		zxero::int64	perpare_time_ = 500;
		zxero::int64	suffix_time_ = 1 * 1000;
	};
	class BuffCommand : public BattleCommand
	{
	public:
		virtual CommandResult Execute(const boost::shared_ptr<MtBattleGround>& / *battle_ground* /, zxero::uint64 elapseTime) override;
		virtual CommandType Type() const override { return CommandType::COMMAND_BUFF; }
	private:
		boost::shared_ptr<MtActor> attacker_;
		boost::shared_ptr<MtActor> defender_;
		zxero::int32	buff_id_;
		uint64			last_time_ = 2 * 1000;
		uint64			effect_times_ = 5;
	};

	class MoveCommand : public BattleCommand
	{
	public:
		MoveCommand(const boost::shared_ptr<MtActor>& source,
			const boost::shared_ptr<MtActor>& target,
			const Packet::Position& target_pos);
		virtual CommandResult Execute(const boost::shared_ptr<MtBattleGround>& / *battle_ground* /, zxero::uint64 elapseTime) override;
		virtual CommandType Type() const override { return CommandType::COMMAND_MOVE; }
	private:
		boost::shared_ptr<MtActor> source_;
		boost::shared_ptr<MtActor> target_;
		Packet::Position	target_pos_;
	};

/ *
	struct DamageData {
		enum class DamageType {
			PHYSICAL = 1, //物理伤害
			MAGIC = 2, //法术伤害
			OTHER = 3, //不属于上面两类
		};
		zxero::int32 skill_id;
		zxero::int32 damange;
		DamageType type;
	};* /

	class DamageCommand : public BattleCommand
	{
	public:
		DamageCommand(const boost::shared_ptr<MtActor>& attacker,
			const boost::shared_ptr<MtActor>& defener,
			const boost::shared_ptr<Packet::ActorOnDamage>& damage);
		virtual CommandResult Execute(const boost::shared_ptr<MtBattleGround>& / *battle_ground* /, zxero::uint64 / *elapseTime* /) override;
		virtual CommandType Type() const override { return CommandType::COMMAND_DAMAGE; }
	private:
		boost::shared_ptr<MtActor> attacker_;
		boost::shared_ptr<MtActor> defender_;
		boost::shared_ptr<Packet::ActorOnDamage> damage_;
	};
	
	class DeadCommand : public BattleCommand
	{
	public:
		DeadCommand(const boost::shared_ptr<MtActor>& attacker,
			const boost::shared_ptr<MtActor>& defener);
		virtual CommandResult Execute(const boost::shared_ptr<MtBattleGround>& battle_ground, zxero::uint64 / *elapseTime* /) override;
		virtual CommandType Type() const override { return CommandType::COMMAND_DEAD; }
	private:
		boost::shared_ptr<MtActor> attacker_;
		boost::shared_ptr<MtActor> defender_;
	};*/
}
#endif // ZXERO_GAMESERVER_MT_BATTLE_COMMAND_H__
