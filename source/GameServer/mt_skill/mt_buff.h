#ifndef MTONLINE_GAMESERVER_MT_BUFF_H__
#define MTONLINE_GAMESERVER_MT_BUFF_H__

#include "../mt_types.h"
#include "json/forwards.h"
#include <bitset>
#include <SkillConfig.pb.h>
#include <BattleExpression.pb.h>
#include <ActorBasicInfo.pb.h>
#include <AllConfigEnum.pb.h>

namespace Mt
{
	class SkillContext;
	struct TransformEvent;

namespace BuffImpl
{

	/**
	* \brief
	* \Buff
	* - buff�ó���ʱ��, ���������������Կ�����������
	* - ���ⲻͬcreator��buff,��������һģһ��, Ҳ���ܺϲ�
	*/
	class Buff : public boost::enable_shared_from_this<Buff>
	{
	public:
		static void lua_reg(lua_State* ls);
		virtual ~Buff();;
		int32 BuffId() const { return buff_id_; }
		void BuffId(int32 id) {	buff_id_ = id;}
		Config::BuffType BuffType() const { return buff_type_; }
		bool IsDebuff() const;
		bool CanDespell() const;
		void BuffType(Config::BuffType buff_type) { buff_type_ = buff_type; }
		int32 BuffLevel() const { return level_; }
		void SetBuffLevel(int32 level) { level_ = level; }
		int32 DamageType() const;
		void SetBattleGround(MtBattleGround* bg);
		virtual int32 Hp() const { return 0; }
		virtual void Flush() {};
		virtual int32 MaxHp() const { return 0; }
		virtual int32 PhysicalAttack() const { return 0; }
		virtual int32 PhysicalArmor() const { return 0; }
		virtual int32 MagicAttack() const { return 0; }
		virtual int32 MagicArmor() const { return 0; }
		virtual int32 Dodge() const { return 0; }
		virtual int32 Toughness() const { return 0; }
		virtual int32 Accurate() const { return 0; }
		virtual int32 Critical() const { return 0; }
		virtual int32 MoveSpeed() const { return 0; }
		virtual int32 SkillTargetCount() const { return 0; }
		virtual real32 SlowDownPercent() const{ return 0.f; }
		virtual real32 CriticalPercent() const { return 0.f; }
		virtual real32 DodgePercent() const { return 0.f; }
		virtual int32 SkillLevel(int32 level) const;
		virtual bool CanBeFear() const { return true; }
		void ClearAllDebuff() const;
		virtual void OnOwnerDamage(Packet::ActorOnDamage& /*damage_info*/, MtActor* /*attacker*/) {};
		virtual bool OnTick(uint64 elapseTime);
		virtual bool AfterCreate() const {return true;}
		virtual bool OnAttach() const;
		virtual void OnOwnerDead();
		virtual void OnCreatorDead() {}
		virtual boost::shared_ptr<Buff> Clone() = 0;
		void SetLayer(int32 layer);
		virtual bool IsBleed() const { return false; }
		virtual void OnLayerChange() {}
		int32 GetLayer() const { return current_layer_; }
		bool OnRefresh();
		void MaxLayer(int32 layer) { max_layer_ = layer; }
		//buff���
		virtual bool HasMark(const std::string& /*mark*/) const { return false; }
		int32 MaxLayer() const { return max_layer_; }
		MtActor* GetCreator() const;
		uint64 GetCreatorGuid() const { return creator_guid_; }
		void SetCreatorGuid(uint64 creator_guid) { creator_guid_ = creator_guid; }
		MtActor* GetOwner() const;
		uint64 GetOwnerGuid() const { return owner_guid_; }
		void SetOwnerGuid(uint64 owner_guid) { owner_guid_ = owner_guid; }
		void SerializeMessage(Packet::ActorBuffAttach& msg) const;
		void SerializeMessage(Packet::ActorBuffRefresh& msg) const;
		void SerializeMessage(Packet::ActorBuffDettach& msg) const;
		int64 LifeTime() const { return life_time_; }
		int64 OriginLifeTime() const { return origin_life_time_; }
		virtual void SetLifeTime(int64 life_time);
		virtual bool NeedDestroy() const;
		//�������ٴ���
		virtual bool OnDestroy() const;
		void SetConfig(const Json::Value* config);
		bool HasPassiveStub(Packet::SkillBuffStubType type) const;
		const Json::Value& GetConfig() const{ return *config_; }
		bool NameEqual(const std::string& name) const;
		void SetData(const std::string& key, int64 value){ runtime_data_[key] = value; }
		int64 GetData(const std::string& key) const;
		void SetCreateSkillId(int32 skill_id) { create_skill_id_ = skill_id; }
		int32 GetCreateSkillId() const { return create_skill_id_; }
	protected:
		Buff& operator=(const Buff& rhs) = delete;
		Buff(const Buff&) = default;
		Buff() = default;
		uint64 creator_guid_ = INVALID_GUID;
		uint64 owner_guid_ = INVALID_GUID;
		boost::weak_ptr<MtBattleGround> bg_;
		const Json::Value* config_ = nullptr;
		mutable int64 life_time_ = -1;
		int32 current_layer_ = 1;
		int32 level_ = 1;
		int32 max_layer_;
		int32 buff_id_;
		int32 create_skill_id_ = 0;
		int64 origin_life_time_ = 0;
		Config::BuffType buff_type_;
		std::map<std::string, int64> runtime_data_;
		std::bitset<Packet::SkillBuffStubType::max_skill_buff_stub_type> stub_bits_;

	};

	class PulseBuff : public Buff
	{
	public:
		virtual void SetBuffCriticalPercent(int32 percent) {critical_percent_ = percent;}
		int32 BuffCriticalPercent() const { return critical_percent_; }
		virtual bool OnTick(zxero::uint64 elapseTime) override;
		virtual bool OnPulse() = 0;
		int32 PulseTime() const { return pulse_time_; }
		void PulseTime(int32 pulse_time) { pulse_time_ = pulse_time; }
		void PulseCount(int32 count) { pulse_count_ = count; }
		virtual void SetLifeTime(zxero::int64 life_time) override
		{
			Buff::SetLifeTime(life_time);
			if (pulse_time_ > 0)
				pulse_count_ = int32(life_time / pulse_time_);
		}
	protected:
		mutable int32 counter_ = 0;
		mutable int32 pulse_count_ = 0;
		int32 pulse_time_ = 0;
		int32 critical_percent_ = 0;
	};

	//�����buff
	class MarkBuff : public Buff
	{
	public:
		MarkBuff(const std::string& mark) :mark_(mark) {}
		virtual bool HasMark(const std::string& mark) const override
		{
			return mark_ == mark;
		}
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		std::string mark_;
	};
	
	class PulseIntance : public PulseBuff
	{
	public:
		virtual bool OnPulse() override;
		virtual boost::shared_ptr<Buff> Clone() override;
	};
	//�����Ե�Ѫ
	class DoT : public PulseBuff
	{
	public:
		DoT(int32 damage)
			:damage_(damage)
		{
		}
		virtual bool OnPulse() override;
		virtual void Flush() override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 damage_ = 0;
	};

	//�����Լ�Ѫ
	class HoT : public PulseBuff
	{
	public:
		HoT(int32 cure)
			:cure_(cure)
		{
		}
		virtual void Flush() override;
		virtual bool OnPulse() override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 cure_ = 0;
	};
	
	//��Ѫ
	class Bleed : public DoT {
	public:
		Bleed(int32 damage) :DoT(damage) {}
		virtual bool IsBleed() const override { return true; }
	};

	class ContinueBuff : public Buff
	{
	public:
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	class HunterMark : public MarkBuff
	{
	public:
		HunterMark() :MarkBuff(std::string("hunter_mark")) {}
	};

	class AddLevel2 : public ContinueBuff
	{
	public:
		typedef ContinueBuff base;
		AddLevel2(Packet::Property level2_type, int32 level2_value);
		virtual bool OnAttach() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
		~AddLevel2();
		Packet::Property Level2Type() const
		{
			return level2_type_;
		}
		zxero::int32 Level2Value() const
		{
			return GetLayer() * level2_value_;
		}
		virtual zxero::int32 Hp() const override {
			if (level2_type_ == Packet::Property::HP)
				return Level2Value();
			return Buff::Hp();
		}
		virtual zxero::int32 PhysicalAttack() const override {
			if (level2_type_ == Packet::Property::PHYSICAL_ATTACK)
				return Level2Value();
			return Buff::PhysicalAttack();
		}
		virtual zxero::int32 PhysicalArmor() const override {
			if (level2_type_ == Packet::Property::PHYSICAL_ARMOR)
				return Level2Value();
			return Buff::PhysicalArmor();
		}
		virtual zxero::int32 MagicAttack() const override {
			if (level2_type_ == Packet::Property::MAGIC_ATTACK)
				return Level2Value();
			return Buff::MagicAttack();
		}
		virtual zxero::int32 MagicArmor() const override {
			if (level2_type_ == Packet::Property::MAGIC_ARMOR)
				return Level2Value();
			return Buff::MagicArmor();
		}
		virtual zxero::int32 Dodge() const override {
			if (level2_type_ == Packet::Property::DODGE)
				return Level2Value();
			return Buff::Dodge();
		}
		virtual zxero::int32 Toughness() const override {
			if (level2_type_ == Packet::Property::TOUGHNESS)
				return Level2Value();
			return Buff::Toughness();
		}
		virtual zxero::int32 Accurate() const override {
			if (level2_type_ == Packet::Property::ACCURATE)
				return Level2Value();
			return Buff::Accurate();
		}
		virtual zxero::int32 Critical() const override {
			if (level2_type_ == Packet::Property::CRITICAL)
				return Level2Value();
			return Buff::Critical();
		}
		virtual zxero::int32 MoveSpeed() const override
		{
			if (level2_type_ == Packet::Property::MOVE_SPEED)
				return Level2Value();
			return Buff::MoveSpeed();
		}

	private:
		Packet::Property level2_type_ = Packet::Property::INVALID_PROPERTY;
		zxero::int32 level2_value_ = 0;
	};

	//��������. ��Ϊbuff��Ҫ��������ͼ���, ���Բ���ֱ����. ʹ�ü̳�����һ��������
	class ReduceLevel2 : public AddLevel2
	{
	public:
		ReduceLevel2(Packet::Property level2_type, zxero::int32 level2_value)
			:AddLevel2(level2_type, -level2_value) {}
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	//����״̬, ����Ϊdebuff
	class AddStatus : public Buff
	{
	public:
		AddStatus(Config::BattleObjectStatus status) :status_(status) {};
		virtual bool AfterCreate()  const override;
		virtual bool OnAttach() const override;
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
		virtual bool IsControlSkill() const 
		{
			if (status_ == Config::BattleObjectStatus::FEAR //�־�
				|| status_ == Config::BattleObjectStatus::FREEZE //����
				|| status_ == Config::BattleObjectStatus::STUN //ѣ��
				|| status_ == Config::BattleObjectStatus::SHEEP //����
				|| status_ == Config::BattleObjectStatus::SNEER //����
				|| status_ == Config::BattleObjectStatus::CONFUSION //����
				)
			{
				return true;
			}
			return false;
		}
		bool HasControlStatus(Config::BattleObjectStatus status) const
		{
			return status_ == status;
		}
		int32 ControlProb() const { return control_prob_; }
		void SetControlProb(int32 prob) { control_prob_ = prob; }
	private:
		Config::BattleObjectStatus status_;
		int32 control_prob_ = 100;
	};

	//����. �̳й�������Ѫ��
	class Transform : public ContinueBuff
	{
	public:
		Transform(int32 config_id, int32 new_hp, int32 new_physical_attack,
			int32 new_magic_attack)
			:config_id_(config_id), new_hp_(new_hp)
			, new_physical_attack_(new_physical_attack)
			, new_magic_attack_(new_magic_attack){}
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
		friend struct TransformEvent;
	public:
		int32 config_id_;
		int32 new_hp_;
		int32 new_physical_attack_;
		int32 new_magic_attack_;
		mutable int32 old_physical_attack_;
		mutable int32 old_magic_attack_;
	};

	//��������״̬, ����
	class Immune : public AddStatus
	{
	public:
		Immune() :AddStatus(Config::BattleObjectStatus::IMMUNE) {};
	};

	//���ܶ��⹥��Ŀ����
	class AddSkillTargetCount: public ContinueBuff
	{
	public:
		AddSkillTargetCount(int32 count) :count_(count) {}

		virtual zxero::int32 SkillTargetCount() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 count_ = 0;
	};

	//³ç, ���߿־�
	class Reckless : public ContinueBuff
	{
	public:
		virtual bool CanBeFear() const override { return false; }
	};

	//��ɨ, ���Ӽ��ܹ���Ŀ������������������
	class SweepAway : public AddSkillTargetCount
	{
	public:
		SweepAway(int32 count, int32 physical_attack)
			:AddSkillTargetCount(count), physical_attack_(physical_attack) {}

		virtual int32 PhysicalAttack() const override
		{
			return physical_attack_;
		}
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 physical_attack_;
	};

	//����hp����
	class AddHpMax : public ContinueBuff
	{
	public:
		AddHpMax(int32 add) :add_(add) {}
		virtual int32 MaxHp() const override { return add_; }
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 add_;
	};

	//����
	class SlowDown : public ContinueBuff
	{
	public:
		SlowDown(real32 percent) :percent_(percent) {}
		virtual real32 SlowDownPercent() const override { return percent_; }
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		real32 percent_;
	};

	//�����. ����Ŀ��, ����һ��������ѣĿ��
	class Shocker : public SlowDown
	{
	public:
		Shocker(real32 percent_, bool stun)
			:SlowDown(percent_),stun_(stun)		{}
		virtual bool AfterCreate() const override;
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		bool stun_;
	};
	
	//����
	class Sneer : public AddStatus
	{
	public:
		Sneer() :AddStatus(Config::BattleObjectStatus::SNEER) {}
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	//����ʱ�ͷż���
	class CastSkillWhenDestroy : public ContinueBuff
	{
	public:
		CastSkillWhenDestroy(int32 skill_id, int32 skill_level)
			:skill_id_(skill_id), skill_level_(skill_level) {}
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 skill_id_;
		int32 skill_level_;
	};
	
	//��������
	class IceBlock : public CastSkillWhenDestroy
	{
	public:
		IceBlock(int32 skill_id, int32 skill_level)
			:CastSkillWhenDestroy(skill_id, skill_level) {}
		virtual bool AfterCreate() const override;
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	//������
	class Sheep : public AddStatus
	{
	public:
		Sheep() :AddStatus(Config::BattleObjectStatus::SHEEP) {}
		//�յ��˺�ʱȡ��
		virtual void OnOwnerDamage(Packet::ActorOnDamage&, MtActor*) override;
		virtual boost::shared_ptr<Buff> Clone() override;
	};
	

	//�����˺������Ļ���
	class Shield : public ContinueBuff
	{
	public:
		Shield( int32 shield)
			:shield_(shield){}
		virtual void OnOwnerDamage(Packet::ActorOnDamage& damage_info, MtActor* attacker) override;
		virtual bool NeedDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 shield_ = 0;
	};

	//�޵�
	class God : public AddStatus
	{
	public:
		God() :AddStatus(Config::BattleObjectStatus::GOD) {}
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	//����
	class FakeDead : public AddStatus
	{
	public:
		FakeDead() :AddStatus(Config::BattleObjectStatus::FEAKDEAD) {}
		virtual boost::shared_ptr<Buff> Clone() override;
		virtual bool AfterCreate() const override;
	};

	//��������. ���Ӷ��⹥��Ŀ������(���⹥����Ŀ�겻���¼����˺�,ֱ����ɵ�ǰĿ��ĵ����˺�)
	//���ӹ�����
	class BladeFlurry : public AddLevel2
	{
	public:
		typedef AddLevel2 base_class;
		BladeFlurry(int32 extra_count, int32 range, Packet::Property type,
			int32 value)
			: AddLevel2(type, value),extra_count_(extra_count), range_(range){}
		virtual bool AfterCreate() const override;
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 extra_count_;
		int32 range_;
	};

	//���ӱ�����
	class AddCriticalPercent : public ContinueBuff
	{
	public:
		AddCriticalPercent(real32 percent) :percent_(percent) {}

		virtual real32 CriticalPercent() const override { return percent_; }
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		real32 percent_;
	};
	//����������
	class AddDodgePercent : public ContinueBuff
	{
	public:
		AddDodgePercent(real32 percent) : percent_(percent) {}
		virtual real32 DodgePercent() const override { return percent_; }
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		real32 percent_;
	};

	//��ʧ, �޷���ѡ�к͹���
	class Disappear : public AddStatus
	{
	public:
		typedef AddStatus base;
		Disappear(int32 skill_id, int32 skill_level)
			:AddStatus(Config::BattleObjectStatus::DISAPPEAR)
		, skill_id_(skill_id), skill_level_(skill_level){}
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 skill_id_;
		int32 skill_level_;
	};

	//����ʱ����
	class CureWhenDestroy : public ContinueBuff
	{
	public:
		CureWhenDestroy(int32 value) :value_(value) {}
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 value_;
	};

	//��������. ����ʱ��������
	class LifeBurst : public HoT
	{
	public:
		LifeBurst(int32 hot_value, int32 destroy_value)
			:HoT(hot_value), cure_value_(destroy_value) {}
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 cure_value_;
	};

	//����֮��:
	//1 Ӧ�����޵�Ч��,������ֱ������
	//2 ���ھ���֮��״̬��, ���м��ܵȼ�������֮����
	class RedemptionGhost : public AddStatus
	{
	public:
		typedef AddStatus base_class;
		RedemptionGhost(int32 skill_level, uint64 killer_guid)
			:AddStatus(Config::BattleObjectStatus::GOD),
			skill_level_(skill_level), killer_guid_(killer_guid){}
		virtual bool OnDestroy() const override;
		virtual boost::shared_ptr<Buff> Clone() override;
		virtual bool OnTick(uint64 elapseTime) override;
		virtual int32 SkillLevel(int32 level) const override;

	private:
		int32 skill_level_;
		uint64 killer_guid_ = INVALID_GUID;
	};

	//���Ƽӳ�
	class AddCureEffect : public ContinueBuff
	{
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	//���Ƽ���
	class ReduceCureEffect : public ContinueBuff
	{
		virtual boost::shared_ptr<Buff> Clone() override;
	};

	//��ŭ, �����������ͱ�����
	class Fury : public ContinueBuff
	{
	public:
		Fury(int32 physical_attack, real32 critical_percent)
			:physical_attack_(physical_attack)
			, critical_percent_(critical_percent) {}

		virtual int32 PhysicalAttack() const override
		{
			return GetLayer() * physical_attack_;
		}

		virtual real32 CriticalPercent() const override
		{
			return GetLayer() * critical_percent_;
		}
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 physical_attack_;
		real32 critical_percent_;
	};

	
	class MaxLayerTrigger : public Buff
	{
	public:
		MaxLayerTrigger() = default;
		virtual void OnLayerChange();
		/**
		* \brief ����������ʱ, ��Ҫ���õķ���
		* \return void
		*/
		virtual void TakeEffect() = 0;
	};

	//������ʱ,buffӵ����ʹ�ü���
	class CastSkillLayerTrigger : public MaxLayerTrigger
	{
	public:
		CastSkillLayerTrigger(int32 skill_id, int32 skill_level)
			:skill_id_(skill_id), skill_level_(skill_level) {}
		virtual void TakeEffect() override;
		virtual boost::shared_ptr<Buff> Clone() override;
	private:
		int32 skill_id_;
		int32 skill_level_;
	};

	//������ʱ,buff�Ĵ�����ʹ�ü���
	class ComboPoint : public MaxLayerTrigger
	{
	public:
		ComboPoint(int32 max_layer){
			MaxLayer(max_layer); 
		}
		virtual void TakeEffect() override;
		virtual boost::shared_ptr<Buff> Clone() override;
	};


/*
	//�ﵽ������ʱ, �ܵ������˺�
	class DamageMaxLayer : MaxLayerTrigger
	{
	public:
		DamageMaxLayer(int32 damage, int32 max_layer) 
			:damage_(damage), MaxLayerTrigger(max_layer) {}
		virtual void TakeEffect() const override;
	private:
		int32 damage_ = 0;
	};*/

	class BuffFactory : public boost::serialization::singleton<BuffFactory>
	{
	public:
		const boost::shared_ptr<Buff> CreateBuff(const Json::Value& config,
			SkillContext& ctx, MtBattleGround* bg, uint64 creator_guid);
	};
}
}
#endif // MTONLINE_GAMESERVER_MT_BUFF_H__
