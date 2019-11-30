#ifndef MTONLINE_GAMESERVER_MT_SKILL_H__
#define MTONLINE_GAMESERVER_MT_SKILL_H__
#include "../mt_types.h"
#include "../mt_arena/geometry.h"
#include <bitset>
#include <json/forwards.h>
#include <map>
#include <array>
#include "ActorBasicInfo.pb.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
	using BuffImpl::Buff;
	class SkillContext;
	namespace Battle
	{
		class MtSkill : public boost::noncopyable
		{
		public:
			MtSkill(int32 skill_id, int32 skill_level)
				:skill_id_(skill_id), skill_level_(skill_level) {}
			int32 SkillId() const
			{
				return skill_id_;
			}
			int32 SkillLevel() const
			{
				return skill_level_;
			}
		private:
			int32 skill_id_;
			int32 skill_level_;
		};
	}


	struct SkillTarget
	{
		void Reset() { 
			targets_.clear(), extra_target_.clear(); 
		}
		void AddTarget(uint64 acotr_guid);
		void AddExtraTarget(uint64 acotr_guid);
		uint64 FirstTargetGuid() const;
		//����ѡ��ļ���Ŀ��, Ч����������
		std::vector<uint64> targets_;
		//����Ŀ��, Ч����������Ŀ������������
		std::vector<uint64> extra_target_;
	};
	class MtActorSkill : public boost::noncopyable , public boost::enable_shared_from_this<MtActorSkill>
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtActorSkill() {};
		MtActorSkill(const Packet::ActorSkill& db_info, MtActor* owner);
		MtActorSkill( MtActorSkill& rhs);
		virtual ~MtActorSkill();
		void Clear();
		Packet::ActorSkill* DbInfo() ;
		uint64 Guid() const;
		int32 SkillRange() const;
		int32 ChargeStopRange() const;
		const int32 SkillId() const;
		const int32 SKillLevel() const;
		virtual void Start();
		virtual bool Started() const { return status_ != SkillStatus::IDEL; }
		/**
		* \brief ��ͨ����(0��β�ľ�����ͨ����)
		* \return bool
		*/
		bool BaseSkill() const;
		void OnSkillHitTarget(const boost::shared_ptr<MtActor>& target, const Packet::ActorOnDamage& damage_info) const;
		zxero::int32 SubSkillCount() const { return sub_skill_count_; }
		virtual bool Finished() const { return status_ == SkillStatus::DONE; }
		Json::Value* SkillConfig() const { return skill_json_config_; }
		virtual void SetReuse();
		void OnDestroy();
		bool IsIdle() const;
		bool IsCoolDown() const;
		int32 TriggerCount() const { return trigger_count_; }
		void TriggerCount(int32 count){ trigger_count_ = count; }
		void AddTriggerCount(int32 count = 1){trigger_count_ += count;}
		bool CanTrigger() const;
		void OnTick(zxero::uint64 elapseTime);
		bool ValidateToActor(uint64 guid) const;
		bool NeedTeammate() const;
		void Refresh();
		void BeBreak();
		uint64 SelectMoveTarget();
		bool SelectSkillTarget(SkillTarget* result);
		bool SelectSkillTargetInner(const Json::Value& tar_logic, SkillTarget& target_result);
		const Json::Value& JsonConfig() const { return *skill_json_config_; }
		void InitPassiveBits();
		bool HasPassiveStub(Packet::SkillBuffStubType type);
		//�ü��ܵı������ܲ���
		const Json::Value& PassiveConfig() const;
		//�ü��ܵ��������ܲ���
		const Json::Value& ActiveConfig() const;
		SkillContext& Ctx();
		//���ü�����ʱ�ȼ�
		void TempLevel(int32 level);
		MtActor* Owner(){	return owner_; }
		/**
		* \brief ������, ������������, ������������
		* \return bool
		*/
		bool HasPassiveConfig() const;
		bool DirectUsable() const; //�����Ƿ�ֱ�ӿ���
		/**
		* \brief �������ܱ�����ʱ����cd
		*/
		void EnterCoolDown();
		bool IsLongTimeSkill() const;
		bool IsFirstStage() const;
		void SetSkillTarget(const SkillTarget& target);
		const SkillTarget& GetSkillTarget() const;
	protected:
		void SetCoolDownTime();
		virtual void TakeSelfEffect();
		bool TargetDistanceAvailable(MtActor* target) const;
		/**
		* \brief ��ɢЧ��
		*/
		void Despelling();
		/**
		* \brief ����Ŀ��
		*/
		void ReliveTarget();
		void BuffEffect();
		void TrapEffect();
		void SummonEffect();
		void HitOffEffect();
		/**
		* \brief ����һ��actor����ӵ�ս��
		*/
		bool CreateActor(const Json::Value& data, std::set<uint64>& r_set);
		void CancelSkillMsg();
		bool ReliveSkill() const;
		bool SelectSelf() const;
		/**
		* \brief ֻ�б�������, ���߱������ܰ�����������
		* \return bool
		*/
		bool MainPassive() const;
		Packet::ActorSkill db_info_;
		Json::Value* skill_json_config_ = nullptr;
		boost::shared_ptr<MtSkillPositionEffect> position_effect_;
		int64 cool_down_time_ = 0;
		int32 repeat_times_ = 0;
		int64 prepare_time_ = 0;
		int64 post_time_ = 0;
		int32 trigger_count_ = 0;
		MtActor* const owner_ = nullptr;
		int sub_skill_count_ = 0;
		int	sub_skill_index_ = 0;
		bool started_ = false;
		bool finished_ = false;
		int32 temp_level_ = 0;
		enum class SkillStatus
		{
			IDEL, //�ȴ���ѡ��
			START, //��ѡ����
			PREPARE, //ǰҡ��
			POSITION_LOGIC, //��λ�߼�
			EXECUTING, //ִ����
			POST, //��ҡ��
			COOLDOWN,  //��ȴ��
			DONE, //ִ�����
		};
		SkillTarget targets_;
		SkillStatus status_ = SkillStatus::IDEL;
		boost::shared_ptr<SkillContext> ctx_;
		bool has_passive_config_ = false;
		std::bitset<Packet::SkillBuffStubType::max_skill_buff_stub_type> stub_bits_;
	};

	struct ActorSkillKey
	{
		zxero::int32 skill_id_;
		zxero::int32 skill_level_;
		bool operator<(const ActorSkillKey& rhs) const {
			return (skill_id_ < rhs.skill_id_) || (skill_level_ == rhs.skill_level_ && skill_level_ < rhs.skill_level_);
		}
	};
	bool operator<(const std::tuple<int32, Packet::ActorColor>& lhs, const std::tuple<int32, Packet::ActorColor>& rhs);
	class MtSkillManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
		static MtSkillManager& Instance();
		MtSkillManager();
		zxero::int32 OnLoad();
		const Config::SkillBuildConfig* GetSkillBuild(int32 actor_config_id, Packet::ActorColor color) const;
		int32 MaxSkillLevel() const { return 60; }
		Json::Value* SkillConfig(int32 skill_id);
		void InitLuaContext();
		void __InitLuaContext(const std::string& json_file);
		void OnValidateSkill(bool validate);
		void ParseSkill(Json::Value& skill_define);
		void TarLogicCheck(Json::Value& skill_define);
		void TarExtraCheck(const Json::Value& tar_logic);
		void ExtraBuffParse(int32 skill_id, Json::Value& buff_define);
		bool CanDespell(Config::BuffType type) const;
		bool IsDebuff(Config::BuffType type) const;
		/**
		* \brief ��ǰ���ܶ�Ӧ�ļ���ϵ��, ���ڼ��㼼����������ֵ
		*/
		float SkillFactor(int32 level) const;
	private:
		std::map<Config::BuffType, boost::shared_ptr<Config::BuffDefine>> buff_define_map_;
		std::map<ActorSkillKey, boost::shared_ptr<Config::SkillConfig>> skill_map_;
		std::map<zxero::int32, boost::shared_ptr<Config::ImpactConfig>> impact_map_;
		std::map<zxero::int32, boost::shared_ptr<Config::TrapConfig>> trap_map_;
		std::map<std::pair<int32, Packet::ActorColor>, boost::shared_ptr<Config::SkillBuildConfig>> skill_build_map_;
		std::array<float, 90> skill_factor_;
		std::map<int32, boost::shared_ptr<Json::Value>> json_skill_map_;
	};
	
}
#endif // MTONLINE_GAMESERVER_MT_SKILL_H__
