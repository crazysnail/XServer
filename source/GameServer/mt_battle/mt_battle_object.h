#ifndef MTONLINE_GAMESERVER_MT_BATTLE_OBJECT_H__
#define MTONLINE_GAMESERVER_MT_BATTLE_OBJECT_H__

#include "../mt_types.h"
#include <bitset>
#include <BattleGroundInfo.pb.h>
#include <AbilityAndStatus.pb.h>

namespace Mt
{
	class MtActor;
namespace Battle {
	class BattleGround;
	class MtSkill;
/*
	struct BattleObjectType
	{
		bool operator()(BaseObject& obj) const
		{
			switch (obj.Type())

			{

			case Packet::BattleObjectType::MAIN_ACTOR:

			case Packet::BattleObjectType::HERO_ACTOR:

			case Packet::BattleObjectType::NORMAL_MONSTER:

			case Packet::BattleObjectType::BOSS:

				return true;

			default:
				break;

			}
			return false;
		}
	};*/
	//BaseObject 是战场里最基础的实例化类型, 且不具备移动能力
	class BaseObject : public boost::noncopyable, public boost::enable_shared_from_this<BaseObject>
	{
	public:
		BaseObject(BattleGround& battle_ground)
			:battle_ground_(battle_ground) {}
		//virtual
		virtual ~BaseObject() {}
		/**
		* \brief 心跳
		* \param elapseTime
		* \retval true 可以被删除
		* \retval false 还有逻辑需要继续执行
		*/
		virtual bool OnTick(uint64 /*elapseTime*/) = 0;
		virtual Packet::BattleObjectType Type() const { return Packet::BattleObjectType::INVALID_TYPE; }
		virtual void OnRefresh() {}
		/**
		* \brief 可以被看做已死亡, 主要用在判断胜负时, trap什么的都可以看做已经死亡了
		* \return bool
		*/
		virtual bool BeSupposeToDead() const = 0;
		//模型半径
		int32 Radius() const { return radius_; }
		void Radius(int32 radius) { radius_ = radius; }
		BattleGround& CurrentBattleGround() const {
			return battle_ground_;
		}
		real64 Direction() const { return dir_; }
		void Direction(real64 dir) { dir; }
		Packet::Position Position() { return pos_; }
		void Position(const Packet::Position& pos) { pos_.CopyFrom(pos); }
		int32 PositionIndex() const { return position_index_; }
		void PositionIndex(int32 index) { position_index_ = index; }
	protected:
		int32 radius_ = 0;
		real64 dir_ = 0.f;
		int32 position_index_ = -1;
		Packet::Position pos_;
		BattleGround& battle_ground_;
	};
	class MoveableObject : public BaseObject
	{
	public:
		MoveableObject(BattleGround& battle_ground)
			:BaseObject(battle_ground) {}
		virtual ~MoveableObject() override {}
		//non virtual
		int32 MoveSpeed() const { return speed_; }
		void MoveSpeed(int32 speed) { speed_ = speed; }
		/*
		* \param target 向target所在位置移动, target每帧都可能更新自己的位置
		* \retval true 已经移动到目标附近
		* \retval false 尚未移动到目标附近
		*/
		bool MoveToTarget(const boost::shared_ptr<BaseObject>& target);
		/**
		* @brief 向目标点移动
		* @retval true 已经到达目标点附近
		* @retval false 尚未到达目标点附近
		*/
		bool MoveToPosition(const Packet::Position& target_pos, int32 stop_range);
		/**
		* \brief 两个物体间的相对停止距离
		* \param target
		* \return int32
		*/
		int32 StopRange(const boost::shared_ptr<BaseObject>& target)
		{
			return StopRange() + target->Radius();
		}
	protected:
		/**
		* \brief 向物体移动时的停止距离
		* \return google::protobuf::int32
		*/
		virtual  int32 StopRange() const { return Radius(); }
		int32 speed_ = 0;
		boost::shared_ptr<BaseObject> move_toward_target_;
	};

	class Trap : public BaseObject
	{
	public:
		Trap(BattleGround& bg) :BaseObject(bg) {}
		virtual ~Trap() override;
		//初始化数据
		void Init(BaseObject* creator, Config::TrapConfig& config) { creator; config; }
		virtual Packet::BattleObjectType Type() const override { return Packet::BattleObjectType::TRAP; }
		virtual bool OnTick(uint64 /*elapseTime*/) override {  return true;}
		virtual bool BeSupposeToDead() const override {return true;}

	private:
		boost::shared_ptr<BaseObject> creator_;
	};

	class BattleObject : public MoveableObject
	{
	public:
		BattleObject(BattleGround& bg) :MoveableObject(bg) {};
		virtual ~BattleObject() override {}
		virtual bool OnTick(uint64 elapseTime) override { elapseTime; return true;}
		const Packet::BattleInfo& BattleInfo() const { return battle_info_; }
		/**
		* \brief 选择下一个应该释放的技能
		* \return bool
		* \retval true 成功选择技能
		* \retval false 无法选择技能, 由于状态限制等原因造成
		*/
		virtual bool SelectSkill() = 0;
		/**
		* \brief 当前选定的技能
		* \return shared_ptr<MtActorSkill>
		*/
		boost::shared_ptr<MtSkill> CurrentSKill() const {
			return	curr_skill_;
		}
		/**
		* \brief 使用技能
		* \return std::tuple<bool, bool> <技能使用成功, 是否还有子技能> 如果释放成功并且还有子技能,需要继续执行
		*/
		std::tuple<bool, bool> UseSkill() 
		{
			return std::make_tuple(true, false);
		};
		/**
		* \brief 对象是否已经死亡
		* \return bool
		*/
		virtual bool IsDead() const
		{
			return battle_info_.hp() <= 0;
		}

		virtual int32 Hp() const { return battle_info_.hp(); }
		virtual int32 MaxHp() const { return battle_info_max_.hp(); }
		void InitSkills(std::vector<std::pair<int32, int32>>& skill_info);
		void AddState(Config::BattleObjectStatus state)
		{
			data_[(int32)state] = true;
		}
		bool HasState(Config::BattleObjectStatus state)
		{
			return data_[(int32)state];
		}
		void DelState(Config::BattleObjectStatus state)
		{
			data_[(int32)state] = false;
		}
		bool CanDo(Config::BattleObjectAbility type)
		{
			type;
			return true;
		}

		virtual bool BeSupposeToDead() const override;

	protected:
		Packet::BattleInfo battle_info_;
		Packet::BattleInfo battle_info_max_;
		std::vector<boost::shared_ptr<MtSkill>> skills_;
		boost::shared_ptr<BattleObject> main_target_; //当前选定的攻击目标
		boost::shared_ptr<MtSkill> curr_skill_;
		std::bitset<(int32)Config::BattleObjectStatus::STUN + 1> data_ = 0x00000000;
	};

	class HeroActor : public BattleObject
	{
	public:
		HeroActor(BattleGround& bg) : BattleObject(bg) {}
		virtual ~HeroActor() override {}
		void Init(boost::shared_ptr<Mt::MtActor>& source);
		virtual Packet::BattleObjectType Type() const override { return Packet::BattleObjectType::HERO_ACTOR; }

		virtual bool SelectSkill() override;

	protected:
		boost::shared_ptr<Mt::MtActor> source_;
	};

	class MainActor : public HeroActor
	{
	public:
		MainActor(BattleGround& bg) : HeroActor(bg) {}
		virtual Packet::BattleObjectType Type() const override { return Packet::BattleObjectType::MAIN_ACTOR; }
	};

	class Monster : public BattleObject
	{
	public:
		Monster(BattleGround& bg) :BattleObject(bg) {}
		virtual ~Monster() override;
		virtual Packet::BattleObjectType Type() const override { return Packet::BattleObjectType::NORMAL_MONSTER; }
		void Init(const Config::MonsterConfig& config)
		{
			config;
		}
	};

	//Boss和monster应该主要就是在AI上有所区别
	class Boss : public Monster
	{
	public:
		Boss(BattleGround& bg) :Monster(bg) {}
		virtual ~Boss() override;
		virtual Packet::BattleObjectType Type() const override { return Packet::BattleObjectType::BOSS; }
		void Init(const Config::MonsterConfig& config)
		{
			Monster::Init(config);
			//设置自己的更高级的AI
		}
	};
}


}
#endif // MTONLINE_GAMESERVER_MT_BATTLE_OBJECT_H__
