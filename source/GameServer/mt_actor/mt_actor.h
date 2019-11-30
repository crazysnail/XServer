#ifndef ZXERO_GAMESERVER_MT_ACTOR_H__
#define ZXERO_GAMESERVER_MT_ACTOR_H__

#include "../mt_types.h"
#include <bitset>
#include <AbilityAndStatus.pb.h>
#include <BattleGroundInfo.pb.h>
#include "json/json.h"
#include "../mt_skill/mt_simple_impact.h"
#include "../mt_skill/mt_skill.h"

namespace Mt 
{
	namespace BattleState { class BaseState; }
	struct attribute_set;
	class MtActorState;
	class NormalAiAdaptor;
	class MtSimpleImpactSet;
	using BuffImpl::Buff;
	struct MtActorAbility {
		MtActorAbility() { Clear(); }
		bool AddState(Config::BattleObjectStatus state);
		void ForceRemoveState(Config::BattleObjectStatus state);
		bool GetState(Config::BattleObjectStatus state);
		bool ClearState(Config::BattleObjectStatus state);
		bool CanDo(Config::BattleObjectAbility type) const;
		void Clear();
		int8 status_[Config::BattleObjectStatus_ARRAYSIZE];
		int32 ability_ = 0xffffff;
	};

	class ActorBattleCopy;
	class MtActor : public boost::noncopyable, public boost::enable_shared_from_this<MtActor>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		/**
		* \brief 通过角色的战斗快照创建一个战斗用的Actor
		* \param full_info
		*/
		//MtActor(const Packet::ActorFullInfo& full_info);;
		MtActor(const boost::shared_ptr<Packet::ActorBasicInfo> info,
			MtPlayer& player);
		MtActor(const MtActor& rhs);
		MtActor() {}
		virtual ~MtActor();
		Packet::ActorType ActorType() const;
		void SetActorType(Packet::ActorType type);
		virtual Packet::BattleActorType BattleActorType() const;
		virtual void InitBattleInfo(const MtActor& rhs);
		virtual void InitBattleInfo(const Packet::ActorFullInfo& msg);
		virtual void SetRuntimeActorId(int32 /*config_id*/) {}
		/**
		* \brief
		* \param elapseTime
		* \retval true actor可以被删除了
		* \retval false actor需要保留
		*/
		virtual	bool OnBigTick(zxero::uint64 elapseTime);
		virtual	bool OnTick(zxero::uint64 elapseTime);
		virtual const boost::shared_ptr<ActorBattleCopy> CreateBattleCopy();
		static boost::shared_ptr<ActorBattleCopy> CreateBattleCopyFromMsg(const Packet::ActorFullInfo& full_info);
		virtual void OnBattleEnd();
		virtual bool PlayerActor() const { return true; }
		bool MainActor() const;
		int32 EnhanceMasterLv() { return m_min_equips_enhance_lv; }
		bool Male() const;
		bool Female() const;
		bool IsAlliance() const;
		bool IsHorde() const;
		virtual void AddExp(int32 exp, bool notify);
		bool EnterBattleGround(MtBattleGround* battle_ground);
		void SummonEnter();
		bool EnterState(const boost::shared_ptr<MtActorState>& ai_strategy);
		bool AddState(Config::BattleObjectStatus state);
		bool ClearState(Config::BattleObjectStatus state);
		bool GetState(Config::BattleObjectStatus state);
		bool ManualControl() const;
		virtual bool OnDead(MtActor* killer);
		virtual void DeadDurableCost(int times = 1, bool simulate = false);
		void OnKillSomeOne(MtActor* /*target*/);
		bool IsDead();
		int32 MoveSerial();
		void ClientMoveStart();
		void ClearRunTimeSkill();
		void DoClearRunTimeSkill();
		void AddRuntimeSkill(int32 id, int32 level);
		void DoAddRuntimeSkill(int32 id, int32 level);
		bool ClientMoveFinished();
		void OnMoveFinishMsg(const boost::shared_ptr<MtPlayer>& player,
			const boost::shared_ptr<Packet::ActorMoveToTargetFinish>& msg);
		MtBattleGround* BattleGround() const;
		int32 MoveSpeed() const;
		const zxero::real64 Direction() const;
		void Direction(const zxero::real64 dir);
		void Position(const Packet::Position& pos);
		const Packet::Position& Position() const;
		void PushSkill(int32 skill_id, std::vector<uint64> targets);
		MtActorSkill* CurrSkill() const;
		void PopSkill();
		void UnEquipment(MtEquipItem* equip); //卸装备
		bool AddEquipment(MtEquipItem* equip, Config::EquipmentSlot slot); //穿装备
		MtEquipItem* AddEquipByConfigId(const int32 config_id, Config::EquipmentSlot slot); //穿装备
		void NotifyEquipChange(MtEquipItem* equip, bool load = true);
		std::vector<MtEquipItem*> LuaCurrEquipments(bool durable_filter);
		void CurrEquipments(std::vector<MtEquipItem*>& result, bool durable_filter = false ) const;
		void OnSkillLoad(const std::vector<boost::shared_ptr<Packet::ActorSkill>>& skills);
		bool AddSkill(int32 skill_id, int32 skill_level = 1);
		void RemoveSkill(const int32 skill_id);
		void AddSkill(const boost::shared_ptr<MtActorSkill>& skill);
		bool CheckSkill(const int32 skill_id);
		void ClearSkill();
		int32 ConfigId() const;
		zxero::uint64 Guid() const;
		bool operator==(MtActor* rhs);
		boost::shared_ptr<Packet::ActorBasicInfo>& DbInfo() { return db_info_; };
		const boost::shared_ptr<Packet::BattleInfo> RunTimeBattleInfo() const;
		int32 Level() const;
		real32 BeforeCureExecute(real32 cure_hp) const; //返回新的治疗数值
		/**
		* \brief 角色受到伤害
		* \param damage_info 伤害数据
		* \param killer 伤害来源
		* \param ctx 伤害上下文
		* \retval true 死亡
		* \retval false 没有死亡 
		*/
		bool OnDamage(Packet::ActorOnDamage& damage_info, 
			MtActor* killer,
			SkillContext* ctx = nullptr);

		void OnDamageRefix(Packet::ActorOnDamage& damage_info, MtActor* killer);
		void OnPracticeDamage(Packet::ActorOnDamage& damage_info, MtActor* killer);
		int32 GetPractice(Packet::Property pro);
		//被治疗
		bool OnCure(Packet::ActorOnCure& damage_info,
			MtActor*source,
			SkillContext* ctx = nullptr);
		bool OnHpCure(int32 hp_value);
		bool OnRelive();
		virtual void OnBattleRefresh();
		virtual int32 MaxHp() const;
		real32 HpPercent() 
		{
			return Hp() / real32(MaxHp());
		}
		virtual void SendMessage(const google::protobuf::Message& message);
		boost::weak_ptr<MtPlayer> Player() const { return player_; }
		MtPlayer* LuaPlayer() const;
		virtual void SerializationToBattleInfo(Packet::ActorBattleInfo& message) const;
		//攻击半径
		int32 AttackRadius() const;
		//模型半径, 被攻击的时候用来计算
		int32 ObjectRadius() const { return runtime_info_.object_radius; }
		int32 Score() const { return db_info_->score(); }
		std::vector<boost::shared_ptr<MtActorSkill>>& Skills();
		std::vector<boost::shared_ptr<Buff>>& Buffs();
		std::vector<boost::shared_ptr<MtActorSkill>>& DbSkills();

		void RefreshSkillData2Client();
		bool SendCommand(const boost::shared_ptr<BattleCommand> command);
		void SetConrolType(Packet::ActorControlType type);
		Packet::Talent Talent() const;
		Packet::Race Race() const;
		Packet::Professions Professions() const;
		void SetName(const std::string &name);
		//二级属性 start
		zxero::int32 Hp();
		void SetHp(int32 hp);
		zxero::int32 GetLastHp(Packet::LastHpType type);
		void SetLastHp(Packet::LastHpType type,int32 hp);
		bool CanBeFear() const;
		zxero::int32 PhysicalAttack();
		zxero::int32 PhysicalArmor();
		zxero::int32 MagicAttack();
		zxero::int32 MagicArmor();
		virtual zxero::int32 GetPositionIndex() const { return position_index_; }
		void SetPositionIndex(zxero::int32 position_index) { position_index_ = position_index; }
		zxero::int32 Critical() const;
		float ExtraCriticalPercent() const;
		zxero::int32 Toughness() const;
		zxero::int32 Dodge() const;
		zxero::int32 Accurate() const;
		//二级属性 end
		//各种触发点 start
		//被治疗之前
		void BeforeBeCureStub(SkillContext& ctx);
		//被治疗之后
		void AfterBeCureStub(SkillContext& ctx);
		//治疗目标之前
		void BeforeDoCureStub(SkillContext& ctx);
		//治疗目标之后
		void AfterDoCureStub(SkillContext& ctx);
		//受到攻击之前
		void BeforeBeDamageStub(SkillContext& ctx);
		//受到攻击之后
		void AfterBeDamageStub(SkillContext& ctx);
		//攻击他人之前
		void BeforeDoDamageStub(SkillContext& ctx);
		//攻击他人之后
		void AfterDoDamageStub(SkillContext& ctx);
		//攻击被闪避之后
		void AfterBeDodgeStub(SkillContext& ctx);
		//暴击目标后
		void AfterDoCriticalStub(SkillContext& ctx);
		//被暴击后
		void AfterBeCriticalStub(SkillContext& ctx);
		//闪避攻击之后
		void AfterDoDodgeStub(SkillContext& ctx);
		//攻击目标是, 计算暴击率自后
		void AfterCalcCriticalPercentStub(SkillContext& ctx);
		//actor要被打了, 在计算完自己的闪避概率后调用
		void AfterCalcDodgePercentStub(SkillContext& ctx);
		//战斗开始前,执行点东西(现在只有改变觉得configid)
		void BeforeBattleStartStub(SkillContext& ctx);
		//战场刷新了, 有些被动需要出发
		void AfterBattleStartStub(SkillContext& ctx);
		//攻击力计算之后
		void AfterSourceValueCalcStub(SkillContext& ctx);
		//计算攻击方攻击力后
		void TargetAfterSourceValueCalcStub(SkillContext& ctx);
		//计算防御方防御值后
		void TargetAfterTargetValueCalcStub(SkillContext& ctx);
		//防御力计算之后
		void AfterTargetValueCalcStub(SkillContext& ctx);
		//作为攻击方时,获取完暴击,韧性,命中,闪避后
		void AfterSourceCriticalEtcCalcStub(SkillContext& ctx);
		//作为防御方时,获取完暴击,韧性,命中,闪避后
		void AfterTargetCriticalEtcCalcStub(SkillContext& ctx);
		//选择技能之后
		void AfterSelectSkill(SkillContext& ctx);
		//生命值改变
		virtual void AfterHpChange(SkillContext& ctx);
		//添加buff前
		void BeforeBuffAddStub(SkillContext& ctx);
		//添加buff后
		void AfterBuffAddStub(SkillContext& ctx);
		//计算完控制类型buff命中概率后
		void AfterControlBuffProbCalcStub(SkillContext& ctx);
		//自己创建,并添加给其他的buff, 最大层数触发时机
		void AfterBuffMaxLayer(SkillContext& ctx);
		//使用过主动技能后
		void AfterUseActiveSkillStub(SkillContext& ctx);
		//时间出发技能
		void OnTimeElapseStub(SkillContext& ctx);
		/**
		* \brief 死亡前触发
		* \retval true 被动技能/buff响应了改事件, 需要跳过死亡流程. 由被动技能/buff接管
		*/
		bool BeforeDeadStub(SkillContext& ctx);
		//有人使用吟唱技能
		void AfterActorUseLongTimeSkill(SkillContext& ctx);
		//各种触发点 end
		int32 SkillTargetCount() const;
		void AddBuff(const boost::shared_ptr<Buff>& buff);
		void OnCache();
		void SerializationToMessage(Packet::ActorFullInfo& message);
		void OnBattleInfoChanged(bool init=false);
		/**
		* \brief 技能评分,普通技能4点每级, 星级技能40点每级
		* \return int32
		*/
		int32 SkillScore() const;
		int32 LevelUpNeedExp() const;
		bool FullExp() const;
		bool LevelUpTo(int32 level, bool gm = false, bool notify = true);
		void InitEquip();
		void ChangeColor(Packet::ActorColor color);
		void ChangeStar(Packet::ActorStar star);		
		virtual void BattleDurableCost(int32 cost = 1, bool simulate = false);
		const Config::SkillBuildConfig* SkillBuild() const {
			return skill_build_;
		}
		Packet::ActorSkill* FindDBSkill(uint64 guid);
		int32 CurrSkillIndex() const { return curr_skill_index; }
		void CurrSkillIndex(int32 index) { curr_skill_index = index; }
		void UpdateSuitEquipEffect(attribute_set& changes);
		void UpdateMountCountEffect(attribute_set& changes);
		void UpdateSimpleImpcatEffect(attribute_set& changes);
		void UpdateLackesImpcatEffect(attribute_set& changes);
		void UpdateEnhanceMasterEffect(attribute_set& changes);
		void UpdateGemMasterEffect(attribute_set& changes);
		void UpdateSkillAttributeEffect(attribute_set& changes);
		void SetMoveTowardTargetGuid(uint64 guid);
		/**
		* \brief 将要移向的目标
		* \return const boost::shared_ptr<MtActor>
		*/
		uint64 MoveTowardTargetGuid() const;
		void OnExitBattle();
		Packet::Position MoveTargetPos() const {
			return target_pos_;
		}
		void MoveTargetPos(const Packet::Position& target_pos) {
			target_pos_ = target_pos;
		}
		const Json::Value& TarExtra() const;
		void TarExtra(const Json::Value& extra);
		MtSimpleImpactSet* GetSimpleImpactSet() { return &simple_impacts_; }
		SkillTarget& ActorTargets();
		void RefreshData2Client();
		void Refresh2ClientImpactList();
		void AutoMedic();
		void InitAi();
		void BattleFollowing(bool status) { battle_following_ = status; }
		bool BattleFollowing() const { return battle_following_; }
		void SetPlayer(const boost::weak_ptr<MtPlayer>& player);
		virtual void Transform(Config::MonsterConfig* /*config*/) {}
		virtual void TransformCancel() {}
		int32 SkillRange() const;
		int32 SkillListSize() const;
		Json::Value& Meta();
		bool CanMove() const;
		/**
		* \brief 能否使用技能
		* \return bool
		*/
		bool CanUseSkill() const;
		/**
		* \brief 能否使用普通攻击
		* \return bool
		*/
		bool CanUseBaseSkill() const;
		/**
		* \brief 能否使用除普通攻击意外的技能
		* \return bool
		*/
		bool CanUseMagicSkill() const;
		bool CanChangeTarget() const;
		bool CanBeDamage() const;
		bool CanBeSelect(MtActor* source) const;
		bool CanSelectTarget(MtActor* target) const;
		void BattleState(const boost::shared_ptr<BattleState::BaseState>& state);
		void BeforeBuffAdd(const boost::shared_ptr<Buff>& buff);
		void AfterBuffAdd(const boost::shared_ptr<Buff>& buff);
		void SetLevel(int32 level, bool notify);
		void UpdateSkillCreatedActor(int32 skill_id, std::set<uint64>&& set);
		std::set<uint64> SkillCreatedActor(int32 skill_id) const;
		void SetSide(int32 side) { runtime_info_.side_ = side; }
		MtActorSkill* FindRuntimeSkill(int32 skill_id);
		std::vector<uint64> CurrSkillTarget() const;
	protected:

		struct FightData //战斗中会用到的数据, 不存盘
		{
			Packet::Position						pos;
			real64							direction = 0.f;
			int32							object_radius = 500;
			boost::shared_ptr<Packet::BattleInfo>	battle_info_;
			boost::shared_ptr<MtActorState>			strategy_;
			std::vector<boost::shared_ptr<MtActorSkill>> skills_;
			std::list<std::tuple<int32, std::vector<uint64>>> skill_list_;//要依次使用的技能
			int32 side_ = 0;
		};
		struct MoveToAndBlockBit//每个角色周边有8个站位点, 用于避免角色拥挤在一起
		{
			boost::shared_ptr<MtActor> target_;
			int32 block_bits[8] = { 0 };
		};
		Json::Value tar_extra_;
		zxero::int32 position_index_ = 0;
		boost::shared_ptr<BattleState::BaseState> state_;

		boost::weak_ptr<MtPlayer> player_;
		boost::shared_ptr<Packet::ActorBasicInfo> db_info_;
		MtBattleGround* battle_ground_ = nullptr;
		FightData runtime_info_;
		MtActorAbility ability_;
		std::vector<boost::shared_ptr<MtActorSkill>> skills_;
		std::list<boost::shared_ptr<BattleCommand>> commands_;
		
		const Config::SkillBuildConfig* skill_build_ = nullptr;
		int32 curr_skill_index = 0;
		uint64 move_toward_guid_; //将要移动过去的对象
		SkillTarget targets_;
		std::map<int32, std::vector<int32>> suit_set_; //身上的套装集合
		std::vector<boost::shared_ptr<Buff>> buffs_;//战斗buff，生命期受战斗影响
		int32 m_min_equips_enhance_lv = 0;
		Packet::ActorControlType actor_ai_control_type_;
		bool battle_following_ = false;
		Packet::Position target_pos_;
		MtActorSimpleImpactSet simple_impacts_;
		Json::Value meta_;
		int32 move_serial_ = 0;
		Packet::ActorFullInfo full_info_;
		//当前装备技能id
		std::vector<int32> equip_skill_set_;
	private:
		std::map<int32, std::set<uint64>> created_actors_;
	};

	class ActorFullInfoDummy : public MtActor
	{
	public:
		ActorFullInfoDummy(const Packet::ActorFullInfo& info) :info_(info) {}
		const boost::shared_ptr<ActorBattleCopy> CreateBattleCopy() override;
	private:
		Packet::ActorFullInfo info_;
	};
	//TODO 感觉这个必须要加一个BattleActor一类的东西, 把战斗中的Actor和卡牌Actor分开
	//不然很多方法放在MtActor里不合适, 作为卡牌的Actor根本用不上. 放在子类又需要再ActorBattleCopy和MonsterBattleActor了各写一段一模一样的代码
	class ActorBattleCopy : public MtActor
	{
	public:
		ActorBattleCopy(const boost::shared_ptr<MtActor>& rhs);
		ActorBattleCopy(const Packet::ActorFullInfo& msg);
		virtual void AddExp(int32 exp, bool notify) override {
			if (source_)
				return source_->AddExp(exp, notify);
		}
		virtual void DeadDurableCost(int times = 1, bool simulate = false) override;
		virtual void BattleDurableCost(int32 cost = 1, bool simulate = false) override;
		virtual void OnBattleRefresh() override; //重写下刷新, 复制最新的技能
		virtual int32 MaxHp() const override;
		virtual void SetRuntimeActorId(int32 config_id) override { transform_id_ = config_id; }
		virtual void SerializationToBattleInfo(Packet::ActorBattleInfo& message) const override;
		virtual void OnBeginBattleInfo();
		virtual void Transform(Config::MonsterConfig* config) override;
		virtual void TransformCancel() override;
		virtual bool OnDead(MtActor* killer) override;

	private:
		const boost::shared_ptr<MtActor> source_;
		boost::shared_ptr<Packet::BattleInfo> max_battle_info_;
		int32 transform_id_ = 0;
		std::vector<boost::shared_ptr<MtActorSkill>> back_skills_;
		const Config::SkillBuildConfig* back_skill_build_;
		int32 back_radius_;
	};
	class MonsterBattleActor : public MtActor
	{
	public:
		virtual void Transform(Config::MonsterConfig* config) override;
		virtual void TransformCancel() override;
		virtual void SendMessage(const google::protobuf::Message& /*message*/) override {  }
		MonsterBattleActor(const boost::shared_ptr<Config::MonsterConfig>& config, const int32 level);
		virtual void InitBattleInfo(const boost::shared_ptr<Config::MonsterConfig>& config);
		virtual void AddExp(int32 /*exp*/, bool /*notify*/) override {}
		virtual void SetRuntimeActorId(int32 config_id) override { monster_config_id_ = config_id; }
		void LifeTime(int32 life_time) { life_time_ = life_time; }
		virtual bool OnTick(zxero::uint64 elapseTime) override;
		virtual void BattleDurableCost(int32 /*cost = 1*/, bool /*simulate = false*/) override { return; }
		virtual int32 MaxHp() const override;
		virtual bool PlayerActor() const override{ return false; }
		virtual bool OnDead(MtActor* killer) override;

		virtual void SerializationToBattleInfo(Packet::ActorBattleInfo& message) const override;

		virtual void AfterHpChange(SkillContext& ctx) override;

/*		virtual void OnBattleEnd() override;*/

	private:
		int32 ai_strategy_ = 0;
		boost::shared_ptr<Packet::BattleInfo> max_battle_info_;
		int32 exp_drop_ = 0;
		int32 gold_drop_ = 0;
		int32 monster_class_ = -1;
		int32 monster_config_id_ = 0;
		int32 life_time_ = 0;
		int32 back_monster_config_id_ = 0;
		std::vector<boost::shared_ptr<MtActorSkill>> back_skills_;
		int32 back_radius_;
	};

	class SummonMonsterActor : public MonsterBattleActor
	{
	public:
		SummonMonsterActor(
			const boost::shared_ptr<Config::MonsterConfig>& config,
			const int32 level,
			MtActor* creator,
			bool can_be_select);
		virtual void InitBattleInfo(const boost::shared_ptr<Config::MonsterConfig>& config) override;
		virtual void OnBattleEnd() override;
		virtual Packet::BattleActorType BattleActorType() const override;

	};

	struct GlobalSkillDoolDown {
		void OnTick(zxero::uint64 elapseTime);
		bool Started() const { return started_; }
		void Start(const MtActor& actor);
		bool Finished() const { return total_elapse_ > total_cd_time_; }
		void Reset() { started_ = false; total_cd_time_ = 0; total_elapse_ = 0; }
	private:
		bool started_ = false;
		uint64 total_cd_time_ = 0;
		uint64 total_elapse_ = 0;
	};
}
#endif // ZXERO_GAMESERVER_MT_ACTOR_H__
