#include "mt_buff.h"
#include "random_generator.h"
#include "skill_context.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_actor/mt_battle_state.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_battle/battle_event.h"
#include "json/json.h"
#include "mt_skill.h"
#include "mt_skill.h"
#include "mt_target_select.h"
#include <SkillConfig.pb.h>
#include <BattleExpression.pb.h>
#include <boost/make_shared.hpp>

namespace Mt
{
namespace BuffImpl
{
	void Buff::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<Buff, int()>(ls, "Buff").
			def(&Buff::CanDespell, "CanDespell").
			def(&Buff::SetLayer, "SetLayer").
			def(&Buff::Flush, "Flush")
			;
	}

	bool Buff::OnTick(zxero::uint64 elapseTime)
	{
		if (life_time_ >= 0) {
			if (life_time_ - int64(elapseTime) <= 0) {
				life_time_ = 0;
			} else {
				life_time_ -= int64(elapseTime);
			}
		}
		return true;
	}

	bool Buff::NeedDestroy() const
	{
		return life_time_ == 0 || GetLayer() == 0;
	}

	void Buff::SerializeMessage(Packet::ActorBuffAttach& msg) const
	{
		if (bg_.expired() == false) {
			msg.set_source_guid(creator_guid_);
			msg.set_target_guid(owner_guid_);
			msg.set_time_stamp(bg_.lock()->TimeStamp());
			auto buff_info = msg.mutable_buff();
			buff_info->set_buff_id(BuffId());
			buff_info->set_buff_level(level_);
			buff_info->set_layer(GetLayer());
		}
	}

	void Buff::SerializeMessage(Packet::ActorBuffDettach& msg) const
	{
		if (bg_.expired() == false) {
			msg.set_source_guid(creator_guid_);
			msg.set_target_guid(owner_guid_);
			msg.set_time_stamp(bg_.lock()->TimeStamp());
			auto buff_info = msg.mutable_buff();
			buff_info->set_buff_id(BuffId());
			buff_info->set_buff_level(level_);
		}

	}

	void Buff::SerializeMessage(Packet::ActorBuffRefresh& msg) const
	{
		if (bg_.expired() == false) {
			msg.set_source_guid(creator_guid_);
			msg.set_target_guid(owner_guid_);
			msg.set_time_stamp(bg_.lock()->TimeStamp());
			auto buff_info = msg.mutable_buff();
			buff_info->set_buff_id(BuffId());
			buff_info->set_buff_level(level_);
			buff_info->set_layer(GetLayer());
		}
	}

	void Buff::OnOwnerDead()
	{
		life_time_ = 0;
	}

	bool Buff::OnDestroy() const
	{
		Packet::ActorBuffDettach notify;
		SerializeMessage(notify);
		if (bg_.expired() == false)
			bg_.lock()->BroadCastMessage(notify);
		//LOG_INFO << owner_ << owner_->ConfigId() << ". buff[" << BuffId() << "] detach";
		return true;
	}

	bool Buff::OnAttach() const
	{
		Packet::ActorBuffAttach message;
		SerializeMessage(message);
		if (bg_.expired() == false)
			bg_.lock()->BroadCastMessage(message);
		//LOG_INFO << owner_ << owner_->ConfigId() << ". buff[" << BuffId() << "] attach";
		return true;
	}

	bool Buff::OnRefresh()
	{
		Packet::ActorBuffRefresh notify;
		SerializeMessage(notify);
		if (bg_.expired() == false)
			bg_.lock()->BroadCastMessage(notify);
		return true;
	}

	Buff::~Buff()
	{

	}

	int32 Buff::DamageType() const
	{
		Packet::DamageType type(Packet::DamageType::PHYSICAL);
		Packet::DamageType_Parse(GetConfig()["damage_type"].asString(), &type);
		return type;
	}

	void Buff::SetLayer(int32 layer)
	{
		if (layer < 0) layer = 0;
		current_layer_ = layer;
		OnLayerChange();
	}

	void Buff::ClearAllDebuff() const
	{
		//清空所有负面效果
		auto owner = GetOwner();
		if (owner != nullptr) {
			boost::for_each(owner->Buffs(), [](boost::shared_ptr<Buff>& buff) {
				if (MtSkillManager::Instance().IsDebuff(buff->BuffType()))
					buff->SetLayer(0);
			});
		}

	}

	zxero::int32 Buff::SkillLevel(int32 level) const
	{
		return level;
	}

	void Buff::SetLifeTime(zxero::int64 life_time)
	{
		life_time_ = life_time;
		if (origin_life_time_ == 0)
			origin_life_time_ = life_time;
	}

	bool Buff::NameEqual(const std::string& name) const
	{
		if (config_ != nullptr) {
			return (*config_)["name"].asString() == name;
		}
		return false;
	}

	bool Buff::IsDebuff() const
	{
		return MtSkillManager::Instance().IsDebuff(BuffType());
	}

	bool Buff::CanDespell() const
	{
		return MtSkillManager::Instance().CanDespell(BuffType());
	}

	MtActor* Buff::GetCreator() const
	{
		if (bg_.expired() == false) {
			return bg_.lock()->FindActor(creator_guid_);
		}
		return nullptr;
	}

	MtActor* Buff::GetOwner() const
	{
		if (bg_.expired() == false) {
			return bg_.lock()->FindActor(owner_guid_);
		}
		return nullptr;
	}

	void Buff::SetBattleGround(MtBattleGround* bg)
	{
		if (bg)
			bg_ = bg->weak_from_this();
	}

	zxero::int64 Buff::GetData(const std::string& key) const
	{
		if (runtime_data_.find(key) == runtime_data_.end())
			return 0;
		else
			return runtime_data_.at(key);
	}

	void Buff::SetConfig(const Json::Value* config)
	{
		config_ = config;
		if (config_ != nullptr) {
			for (int i = 0; i < Packet::SkillBuffStubType::max_skill_buff_stub_type; ++i) {
				std::string name = Packet::SkillBuffStubType_Name(Packet::SkillBuffStubType(i));
				stub_bits_[i] = config_->isMember(name);
			}
		}
	}

	bool Buff::HasPassiveStub(Packet::SkillBuffStubType type) const
	{
		return stub_bits_[int32(type)];
	}

	bool PulseBuff::OnTick(zxero::uint64 elapseTime)
	{
		if (Buff::OnTick(elapseTime) == false) {
			return false;
		}
		counter_ += int32(elapseTime);
		if (counter_ >= pulse_time_ && pulse_count_ > 0) {
			counter_ = 0;
			pulse_count_--;
			return OnPulse();
		}
		return true;
	}

	bool DoT::OnPulse()
	{
		if (bg_.expired()) {
			return false;
		}
		int32 damage = int32(damage_ * (rand_gen->intgen(95, 105) / 100.f));
		if (BuffCriticalPercent() > 0 && rand_gen->intgen(0, 100) < BuffCriticalPercent()) {
			damage *= 2;
		}
		auto owner = GetOwner();
		auto creator = GetCreator();
		if (owner == nullptr || creator == nullptr) return false;
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(owner->BattleGround()->TimeStamp());
		Packet::ActorOnDamage& damage_info = *skill_effects.add_damages();
		damage_info.set_source_guid(creator_guid_);
		damage_info.set_target_guid(owner_guid_);
		damage_info.set_damage(damage);
		damage_info.set_type(Packet::DamageType(DamageType()));
		damage_info.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
		auto buff = damage_info.mutable_buff();
		buff->set_buff_id(BuffId());
		buff->set_buff_level(level_);
		if (GetConfig().isMember("on_pulse_stub")) {
			SkillContext ctx;
			ctx.buff_.buff_ = this;
			ctx.source_.actor_ = creator;
			ctx.target_.actor_ = owner;
			auto skill = creator->FindRuntimeSkill(this->GetCreateSkillId());
			if (skill != nullptr) {
				ctx.skill_.skill_ = skill;
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(GetConfig()["on_pulse_stub_func"].asString(), &ctx);
			}
		}
		owner->OnDamage(damage_info, creator);
		owner->BattleGround()->BroadCastMessage(skill_effects);
		return true;
	}


	void DoT::Flush()
	{
		auto owner = GetOwner();
		auto creator = GetCreator();
		if (owner == nullptr || creator == nullptr) return;
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(owner->BattleGround()->TimeStamp());
		Packet::ActorOnDamage& damage_info = *skill_effects.add_damages();
		damage_info.set_source_guid(creator_guid_);
		damage_info.set_target_guid(owner_guid_);
		damage_info.set_damage(damage_ * pulse_count_ * GetLayer());
		damage_info.set_type(Packet::DamageType(DamageType()));
		damage_info.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
		auto buff = damage_info.mutable_buff();
		buff->set_buff_id(BuffId());
		buff->set_buff_level(level_);

		if (GetConfig().isMember("on_pulse_stub")) {
			SkillContext ctx;
			ctx.buff_.buff_ = this;
			ctx.source_.actor_ = creator;
			ctx.target_.actor_ = owner;
			auto skill = creator->FindRuntimeSkill(this->GetCreateSkillId());
			if (skill != nullptr) {
				ctx.skill_.skill_ = skill;
				ctx.CalcBuffEffectValue();
				ctx.buff_effect_value_ *= pulse_count_ * GetLayer();
				thread_lua->call_fun<void>(GetConfig()["on_pulse_stub_func"].asString(), &ctx);
			}
		}
		life_time_ = 0;
		pulse_count_ = 0;
		SetLayer(0);
		owner->OnDamage(damage_info, creator);
		owner->BattleGround()->BroadCastMessage(skill_effects);
	}

	boost::shared_ptr<Buff> DoT::Clone()
	{
		return boost::make_shared<DoT>(*this);
	}


	void HoT::Flush()
	{
		auto owner = GetOwner();
		auto creator = GetCreator();
		if (owner == nullptr || creator == nullptr)
			return;
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(owner->BattleGround()->TimeStamp());
		Packet::ActorOnCure& cure_message = *skill_effects.add_cures();
		cure_message.set_source_guid(creator_guid_);
		cure_message.set_target_guid(owner_guid_);
		cure_message.set_cure(cure_ * pulse_count_ * GetLayer());
		auto buff = cure_message.mutable_buff();
		buff->set_buff_id(BuffId());
		buff->set_buff_level(level_);
		if (GetConfig().isMember("on_pulse_stub")) {
			SkillContext ctx;
			ctx.buff_.buff_ = this;
			ctx.source_.actor_ = creator;
			ctx.target_.actor_ = owner;
			auto skill = creator->FindRuntimeSkill(this->GetCreateSkillId());
			if (skill != nullptr) {
				ctx.skill_.skill_ = skill;
				ctx.CalcBuffEffectValue();
				ctx.buff_effect_value_ *= pulse_count_ * GetLayer();
				thread_lua->call_fun<void>(GetConfig()["on_pulse_stub_func"].asString(), &ctx);
			}
		}
		SetLayer(0);
		life_time_ = 0;
		pulse_count_ = 0;
		owner->OnCure(cure_message, creator);
		owner->BattleGround()->BroadCastMessage(skill_effects);
	}

	bool HoT::OnPulse()
	{
		auto owner = GetOwner();
		auto creator = GetCreator();
		if (owner == nullptr || creator == nullptr) return false;
		int32 cure = int32(cure_* (rand_gen->intgen(95, 105) / 100.f));
		if (BuffCriticalPercent() > 0 && rand_gen->intgen(0, 100) < BuffCriticalPercent()) {
			cure *= 2;
		}
		Packet::SkillEffects skill_effects;
		skill_effects.set_time_stamp(owner->BattleGround()->TimeStamp());
		Packet::ActorOnCure& cure_message = *skill_effects.add_cures();
		cure_message.set_source_guid(creator_guid_);
		cure_message.set_target_guid(owner_guid_);
		cure_message.set_cure(cure);
		auto buff = cure_message.mutable_buff();
		buff->set_buff_id(BuffId());
		buff->set_buff_level(level_);

		if (GetConfig().isMember("on_pulse_stub")) {
			SkillContext ctx;
			ctx.buff_.buff_ = this;
			ctx.source_.actor_ = creator;
			ctx.target_.actor_ = owner;
			auto skill = creator->FindRuntimeSkill(this->GetCreateSkillId());
			if (skill != nullptr) {
				ctx.skill_.skill_ = skill;
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(GetConfig()["on_pulse_stub_func"].asString(), &ctx);
			}
		}
		owner->OnCure(cure_message, creator);
		owner->BattleGround()->BroadCastMessage(skill_effects);
		return true;
	}


	boost::shared_ptr<Buff> HoT::Clone()
	{
		return boost::make_shared<HoT>(*this);
	}

	AddLevel2::AddLevel2(Packet::Property level2_type,
		zxero::int32 level2_value)
		:level2_type_(level2_type), level2_value_(level2_value)
	{

	}

	bool AddLevel2::OnAttach() const
	{
		auto owner = GetOwner();
		if (MoveSpeed() > 0 && owner != nullptr) {
			//SYNC move speed
			auto move_speed = owner->MoveSpeed() + MoveSpeed();
			Packet::MoveSpeedUpdate notify;
			notify.set_target_guid(owner_guid_);
			notify.set_move_speed(move_speed);
			notify.mutable_buff()->set_buff_id(BuffId());
			notify.mutable_buff()->set_buff_level(level_);
			notify.set_time_stamp(owner->BattleGround()->TimeStamp());
			owner->BattleGround()->BroadCastMessage(notify);
		}
		return base::OnAttach();
	}


	boost::shared_ptr<Buff> AddLevel2::Clone()
	{
		return boost::make_shared<AddLevel2>(*this);
	}

	AddLevel2::~AddLevel2()
	{
		auto owner = GetOwner();
		if (MoveSpeed() > 0 && owner != nullptr) {
			//SYNC move speed
			auto move_speed = owner->MoveSpeed() - MoveSpeed();
			Packet::MoveSpeedUpdate notify;
			notify.set_target_guid(owner_guid_);
			notify.set_move_speed(move_speed);
			notify.mutable_buff()->set_buff_id(BuffId());
			notify.mutable_buff()->set_buff_level(level_);
			notify.set_time_stamp(owner->BattleGround()->TimeStamp());
			owner->BattleGround()->BroadCastMessage(notify);
		}
	}

	const boost::shared_ptr<Buff> BuffFactory::CreateBuff(
		const Json::Value& config, SkillContext& ctx,
		MtBattleGround* bg, uint64 creator_guid)
	{
		std::string buff_name = config["name"].asString();
		Config::BuffType buff_type;
		int32 life_time = 0;
		int32 max_layer = 1;
		int32 init_layer = 1;
		int32 buff_id = config["buff_id"].asInt();
		boost::shared_ptr<Buff> buff;
		ctx.PushCtx();  auto defer = Defer([&]() {ctx.PopCtx(); });
		if (config.isMember("life_time"))
			life_time = config["life_time"].asInt();
		if (config.isMember("max_layer"))
			max_layer = config["max_layer"].asInt();
		if (config.isMember("init_layer"))
			init_layer = config["init_layer"].asInt();

		if (Config::BuffType_Parse(buff_name, &buff_type)) {
			try
			{

				switch (buff_type)
				{
				case Config::BuffType::weapon_enhence://风怒武器
				case Config::BuffType::flash_ball://闪电球
				case Config::BuffType::holy_shield://神圣之盾
				case Config::BuffType::divine_shield://圣盾术
				case Config::BuffType::seal_of_command://命令印记
				case Config::BuffType::instant_poison://速效毒药
				case Config::BuffType::deadly_poison://致命毒药
				case Config::BuffType::hemorrhage://出血
				case Config::BuffType::seal_of_justice://正义印记
				case Config::BuffType::damage_link://牺牲祝福
				case Config::BuffType::dark_mark://吸血鬼的拥抱
				case Config::BuffType::add_source_hp://圣光审判
				case Config::BuffType::add_cure_effect://光明祝福
				case Config::BuffType::wild://杀戮命令
				case Config::BuffType::add_dodge_percent://增加闪避率
				//case Config::BuffType::reduce_dodge_percent: //降低闪避
				case Config::BuffType::impaling://穿刺
				case Config::BuffType::reduce_cure_effect://治疗效果降低
				case Config::BuffType::be_attack_damage: //反击风暴
				case Config::reduce_damage:
				case Config::BuffType::life_steal://吸血
				case Config::BuffType::double_attack://切割,增加伤害
				case Config::BuffType::knight_bless://庇佑祝福
				case Config::BuffType::add_damage://伤害提升
				case Config::BuffType::reduce_accurate_rate://降低命中
				case Config::BuffType::punish_halo: //惩戒光环
				{
					buff = boost::make_shared<ContinueBuff>();
				}
				break;
				case Config::BuffType::pulse_instance:
				{
					auto pulse = boost::make_shared<PulseIntance>();
					int32 count = config["pulse"].asInt();
					pulse->PulseCount(count);
					pulse->PulseTime(life_time / count);
					buff = pulse;
				}
				break;
				case Config::BuffType::reckless://鲁莽
				{
					buff = boost::make_shared<Reckless>();
				}
				break;
				break;
				case Config::BuffType::add_level2:
				{
					Packet::Property level2_type;
					int32 level2_value = 0;
					if (!config["level2_type_value_func"].empty()) {
						level2_value = thread_lua->call_fun<int32>(config["level2_type_value_func"].asString(), &ctx);
					} else {
						level2_value = config["number"].asInt();
					}
					std::string level2_name = config["level2_type"].asString();
					boost::to_upper(level2_name);
					if (Packet::Property_Parse(level2_name, &level2_type)) {
						buff = boost::make_shared<AddLevel2>(level2_type, level2_value);
					}
				}
					break;
				case Config::BuffType::reduce_level2:
				{
					Packet::Property level2_type;
					int32 level2_value = 0;
					if (!config["level2_type_value_func"].empty()) {
						level2_value = thread_lua->call_fun<int32>(config["level2_type_value_func"].asString(), &ctx);
					} else {
						level2_value = config["number"].asInt();
					}
					std::string level2_name = config["level2_type"].asString();
					boost::to_upper(level2_name);
					if (Packet::Property_Parse(level2_name, &level2_type)) {
						buff = boost::make_shared<ReduceLevel2>(level2_type, level2_value);
					}
				}
					break;
				case Config::BuffType::hunter_mark:
				{
					buff = boost::make_shared<HunterMark>();
				}
				break;
				case Config::BuffType::bleed://流血,每秒造成伤害
				case Config::BuffType::sting://毒蛇钉刺 dot
				case Config::BuffType::burning://点燃, dot
				case Config::BuffType::corruption://腐败, dot
				case Config::BuffType::death_coil://死亡缠绕 dot
				case Config::BuffType::pain://暗言术-痛 dot
				case Config::BuffType::plague://瘟疫 dot
				case Config::BuffType::sacrifice://牺牲陷阱的牺牲buff,dot
				case Config::BuffType::devotion://奉献陷阱的奉献buff, dot
				case Config::BuffType::warlock_sacrifice://术士,献祭
				case Config::BuffType::warlock_curse://术士,诅咒
				case Config::BuffType::poison://毒药
				{
					if (config.isMember("source_value_func")) {
						real32 source_value = thread_lua->call_fun<real32>(config["source_value_func"].asString(), &ctx);
						real32 target_value = thread_lua->call_fun<real32>(config["target_value_func"].asString(), &ctx);
						ctx.source_value_ = source_value;
						ctx.target_value_ = target_value;
						if (!config["cure"].asBool())
							ctx.buff_effect_value_ = ctx.DamageForm(source_value, target_value);
						else
							ctx.buff_effect_value_ = ctx.CureForm(ctx.source_value_);
					} else {
						ctx.buff_effect_value_ = config["number"].asFloat();
					}
					int32 dot_value = int32(ctx.buff_effect_value_);
					if (config.isMember("effect_value_func"))
						dot_value = int32(thread_lua->call_fun<int32>(config["effect_value_func"].asString(), &ctx));
					ZXERO_ASSERT(dot_value >= 0) << "value shoud be positive";
					if (dot_value == 0) {
						dot_value = 1;
					}
					boost::shared_ptr<DoT> dot = nullptr;
					if (buff_type == Config::BuffType::bleed) {
						dot = boost::make_shared<Bleed>(int32(dot_value));
					} else {
						dot = boost::make_shared<DoT>(int32(dot_value));
					}
					int32 count = config["pulse"].asInt();
					dot->PulseCount(count);
					dot->PulseTime(life_time / count);
					buff = dot;
				}
				break;
				case Config::BuffType::continue_cure:
				{
					real32 cure_value = thread_lua->call_fun<real32>(config["cure_value_func"].asString(), &ctx);
					auto hot = boost::make_shared<HoT>(int32(cure_value));
					int32 count = config["pulse"].asInt();
					hot->PulseCount(count);
					hot->PulseTime(life_time / count);
					buff = hot;
				}
				break;
				case Config::add_status:
				{
					Config::BattleObjectStatus status;
					if (Config::BattleObjectStatus_Parse(config["status"].asString(), &status)) {
						buff = boost::make_shared<AddStatus>(status);
					}
				}
					break;
				case Config::BuffType::immune://免疫控制
				{
					buff = boost::make_shared<Immune>();
				}
				break;
				case Config::BuffType::sweep_away://横扫, 增加二级属性和技能攻击目标数
				{
					int32 physical_attack = thread_lua->call_fun<int32>(config["level2_type_value_func"].asString(), &ctx);
					int32 add_count = config["add_count"].asInt();
					buff = boost::make_shared<SweepAway>(add_count, physical_attack);
				}
				break;
				case Config::BuffType::add_hp_max://增加hp上限
				{
					real32 hp_max_value = 0;
					if (config.isMember("hp_max_value_func")) {
						hp_max_value = thread_lua->call_fun<real32>(config["hp_max_value_func"].asString(), &ctx);
					} else if (config.isMember("number")) {
						hp_max_value = config["number"].asFloat();
					} else {
						ZXERO_ASSERT(false);
					}
					buff = boost::make_shared<AddHpMax>(int32(hp_max_value));
				}
					break;
				case Config::BuffType::shocker://震荡
				{
					real32 slow_down_percent = config["slow_down_percent"].asFloat();
					bool stun = thread_lua->call_fun<bool>(config["stun_value_func"].asString(), &ctx);
					buff = boost::make_shared<Shocker>(slow_down_percent, stun);
				}
				case Config::sneer://嘲讽
				{
					buff = boost::make_shared<Sneer>();
				}
				break;
				case Config::BuffType::electric_overload://电能超载
				{
					int32 skill_id = config["skill_id"].asInt();
					int32 skill_level = 0;
					if (config.isMember("skill_level_value"))
						skill_level = thread_lua->call_fun<int32>(config["skill_level_value_func"].asString(), &ctx);
					buff = boost::make_shared<CastSkillLayerTrigger>(skill_id, skill_level);
				}
				break;
				case Config::BuffType::combo_point://连击点
				{
					int32 layer = config["max_layer"].asInt();
					buff = boost::make_shared<ComboPoint>(layer);
				}
				break;
				case Config::BuffType::ice_block:
				{
					int32 skill_id = config["skill_id"].asInt();
					int32 skill_level = thread_lua->call_fun<int32>(config["skill_level_value_func"].asString(), &ctx);
					buff = boost::make_shared<IceBlock>(skill_id, skill_level);
				}
				break;
				case Config::BuffType::sheep:
				{
					buff = boost::make_shared<Sheep>();
				}
				break;
				case Config::BuffType::shield://护盾系列
				{
					real32 shield_value = thread_lua->call_fun<real32>(
						config["shield_value_func"].asString(), &ctx);
					buff = boost::make_shared<Shield>(int32(shield_value));
				}
				break;
				case Config::BuffType::fake_dead://假死
				{
					buff = boost::make_shared<FakeDead>();
					break;
				}
				case Config::BuffType::blade_flurry:
				{
					int32 add_extra_count = config["add_extra_count"].asInt();
					int32 range = config["extra_range"].asInt();
					real32 level2_value = thread_lua->call_fun<real32>(config["level2_type_value_func"].asString(), &ctx);
					buff = boost::make_shared<BladeFlurry>(add_extra_count, range, 
						Packet::Property::PHYSICAL_ATTACK, int32(level2_value));
				}
				break;
				case Config::BuffType::add_critical_percent:
				case Config::weapon_deterrence://武装威慑
				{
					real32 critical_percent = thread_lua->call_fun<real32>(config["critical_percent_value_func"].asString(), &ctx);
					buff = boost::make_shared<AddCriticalPercent>(critical_percent);
				}
				break;
				case Config::BuffType::ghostly_strike://增加移动速度
				{
					real32 move_speed = thread_lua->call_fun<real32>(config["add_move_speed_value_func"].asString(), &ctx);
					buff = boost::make_shared<AddLevel2>(Packet::Property::MOVE_SPEED, int32(move_speed));
				}
				break;
				case Config::BuffType::disappear:
				{
					int32 skill_id = config["skill_id"].asInt();
					int32 skill_level = thread_lua->call_fun<int32>(config["skill_level_value_func"].asString(), &ctx);
					buff = boost::make_shared<Disappear>(skill_id, skill_level);
				}
				break;

				case Config::BuffType::life_burst:
				{
					real32 cure_value = thread_lua->call_fun<real32>(config["cure_value_func"].asString(), &ctx);
					real32 on_destroy_value = thread_lua->call_fun<real32>(config["on_destroy_value_func"].asString(), &ctx);
					auto life_burst = boost::make_shared<LifeBurst>(int32(cure_value), int32(on_destroy_value));
					int32 count = config["pulse"].asInt();
					life_burst->PulseTime(life_time / count);
					life_burst->PulseCount(count);
					buff = life_burst;
				}
				break;
				case Config::BuffType::redemption_ghost:
				{
					int32 skill_level = ctx.passive_skill_.skill_->SKillLevel();
					buff = boost::make_shared<RedemptionGhost>(skill_level, ctx.source_.actor_->Guid());
				}
				break;
				case Config::BuffType::mark://各类标记
				{
					buff = boost::make_shared<MarkBuff>(config["mark_name"].asString());
				}
				break;

				case Config::BuffType::fury://狂怒
				{
					real32 level2_value = thread_lua->call_fun<real32>(config["physical_attack_value_func"].asString(), &ctx);
					real32 critical_percent_value = thread_lua->call_fun<real32>(config["critical_percent_value_func"].asString(), &ctx);
					buff = boost::make_shared<Fury>(int32(level2_value), critical_percent_value);
				}
				case Config::slow_down:
				{
					real32 slow_down_percent = config["slow_down_percent"].asFloat();
					buff = boost::make_shared<SlowDown>(slow_down_percent);
				}
				break;
				case Config::transform:
				{
					if (bg == nullptr) {
						LOG_ERROR << "[create buff] with null battle ground";
						return nullptr;
					}
					int32 config_id = config["config_id"].asInt();
					int32 hp = thread_lua->call_fun<int32>(config["hp_value_func"].asString(), &ctx);
					int32 attack = -1;
					if (config.isMember("physical_attack_value_func"))
						thread_lua->call_fun<int32>(config["physical_attack_value_func"].asString(), &ctx);
					int32 magic = 1;
					if (config.isMember("magic_attack_value_func"))
						thread_lua->call_fun<int32>(config["magic_attack_value_func"].asString(), &ctx);
					auto tran_buff = boost::make_shared<Transform>(config_id, hp, attack, magic);
					auto trans_event = boost::make_shared<TransformEvent>();
					trans_event->transform_buff_ = tran_buff;
					bg->PushEvent(trans_event);
					buff = tran_buff;
				}
				break;
				default:
					ZXERO_ASSERT(false);
					break;
				}
			}
			catch (ff::lua_exception& e)
			{
				LOG_INFO << "create buff execute with lua error:" << e.what();
				return nullptr;
			}
		}
		if (buff) {
			if (ctx.skill_.skill_ != nullptr) {
				buff->SetBuffLevel(ctx.skill_.SkillLevel());
				buff->SetCreateSkillId(ctx.skill_.SkillId());
			}
			buff->BuffId(buff_id);
			buff->BuffType(buff_type);
			buff->SetConfig(&config);
			buff->SetLifeTime(life_time);
			buff->MaxLayer(max_layer);
			buff->SetLayer(init_layer);
			buff->SetCreatorGuid(creator_guid);
			buff->SetBattleGround(bg);
			return buff;
		}
		LOG_INFO << " create buff fails:" << config["buff_id"].asInt()
			<< "," << config["name"].asString();
		return nullptr;
	}


	void MaxLayerTrigger::OnLayerChange()
	{
		if (current_layer_ == max_layer_) {
			TakeEffect();
			life_time_ = 0; //下个tick消失, 可能有bug
			current_layer_ = 0;
		}
	}

/*
	void Mt::MtActorImpactJoint::TakeEffect() const
	{
		Packet::ActorOnDamage msg;
		msg.set_source_guid(creator_guid_);
		msg.set_target_guid(owner_guid_);
		msg.set_time_stamp(owner_->BattleGround()->TimeStamp());
		msg.set_damage(damage_);
		msg.set_type(Packet::DamageType::BUFF);
		msg.set_expression(Packet::DamageExpression::ActorOnDamage_NORMAL);
		msg.mutable_buff()->set_buff_id(Id());
		msg.mutable_buff()->set_buff_level(1);
		owner_->OnDamage(msg, creator_);
	}*/




	bool AddStatus::AfterCreate() const
	{
		if (rand_gen->intgen(0, 99) > control_prob_) return false;
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		if (Config::BattleObjectStatus::SNEER == status_) {//嘲讽
			owner->BattleState(boost::make_shared<BattleState::BeSneer>(owner, creator_guid_));
		}
		Packet::ActorAddState notify;
		notify.set_target_guid(owner_guid_);
		notify.set_state(status_);
		notify.set_time_stamp(owner->BattleGround()->TimeStamp());
		if (status_ == Config::BattleObjectStatus::FEAR) {
			notify.set_fear_dir((rand_gen->intgen(-314, 314) / 100.f));
		}
		owner->BattleGround()->BroadCastMessage(notify);
		return true;
	}


	bool AddStatus::OnAttach() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		owner->AddState(status_);
		return Buff::OnAttach();
	}

	bool AddStatus::OnDestroy() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		Packet::ActorRemoveState notify;
		notify.set_target_guid(owner_guid_);
		notify.set_state(status_);
		notify.set_time_stamp(owner->BattleGround()->TimeStamp());
		owner->BattleGround()->BroadCastMessage(notify);
		owner->ClearState(status_);
		return Buff::OnDestroy();
	}


	boost::shared_ptr<Buff> AddStatus::Clone()
	{
		return boost::make_shared<AddStatus>(*this);
	}

	void Shield::OnOwnerDamage(Packet::ActorOnDamage& damage_info, MtActor* /*attacker*/)
	{
		auto damage = damage_info.damage();
		if (shield_ >= damage) {
			damage_info.set_damage(0);
			shield_ -= damage;
		} else {
			damage_info.set_damage(damage - shield_);
			shield_ = 0;
		}
	}


	bool Shield::NeedDestroy() const
	{
		return shield_ <= 0 || Buff::NeedDestroy();
	}


	boost::shared_ptr<Buff> Shield::Clone()
	{
		return boost::make_shared<Shield>(*this);
	}

	zxero::int32 AddSkillTargetCount::SkillTargetCount() const
	{
		return count_;
	}


	boost::shared_ptr<Buff> AddSkillTargetCount::Clone()
	{
		return boost::make_shared<AddSkillTargetCount>(*this);
	}

	bool CureWhenDestroy::OnDestroy() const
	{
		auto owner = GetOwner();
		auto creator = GetCreator();
		if (owner == nullptr || creator == nullptr) return false;
		Packet::ActorOnCure cure_msg;
		cure_msg.set_cure(value_);
		cure_msg.set_source_guid(creator_guid_);
		cure_msg.set_target_guid(owner_guid_);
		owner->OnCure(cure_msg, creator);
		return Buff::OnDestroy();
	}


	boost::shared_ptr<Buff> CureWhenDestroy::Clone()
	{
		return boost::make_shared<CureWhenDestroy>(*this);
	}

	bool RedemptionGhost::OnDestroy() const
	{
		auto ret = Buff::OnDestroy();
		if (bg_.expired()) return false;
		auto bg = bg_.lock();
		auto killer = bg->FindActor(killer_guid_);
		if (killer)
		{
			auto dead_event = boost::make_shared<DelayDead>();
			dead_event->killer_guid_ = killer_guid_;
			dead_event->dead_guid_ = owner_guid_;
			bg->PushEvent(dead_event);
		}
		return ret;
	}


	boost::shared_ptr<Buff> RedemptionGhost::Clone()
	{
		return boost::make_shared<RedemptionGhost>(*this);
	}


	bool RedemptionGhost::OnTick(uint64 elapseTime)
	{
		auto owner = GetOwner();
		uint64 self_guid = owner->Guid();
		auto bg = bg_.lock();
		if (owner && bg) {//队友全部死亡时, 结束buff
			auto teammates = bg->TeamMates(*owner);
			auto all_dead = std::all_of(teammates.begin(), teammates.end(), [=](auto& actor) {
				if (actor->ActorType() != Packet::ActorType::SUMMON_NO_SELECT
					&& actor->Guid() != self_guid) {
					return actor->IsDead();
				} else {
					return true;
				}
			});
			if (all_dead) { 
				SetLayer(0); 
			}
		}
		return AddStatus::OnTick(elapseTime);
	}

	zxero::int32 RedemptionGhost::SkillLevel(int32 level) const
	{
		if (skill_level_ > 0) return skill_level_;
		return base_class::SkillLevel(level);
	}


	boost::shared_ptr<Buff> Transform::Clone()
	{
		return boost::make_shared<Transform>(*this);
	}

	bool Transform::OnDestroy() const
	{
		if (bg_.expired()) return false;
 		auto e = boost::make_shared<TransformCancel>();
		e->transformer_guid_ = owner_guid_;
		bg_.lock()->PushEvent(e);
		return Buff::OnDestroy();
	}

	void CastSkillLayerTrigger::TakeEffect()
	{
		auto owner = GetOwner();
		if (owner == nullptr) return;
		auto& skills = owner->Skills();
		auto skill_it = std::find_if(skills.begin(), skills.end(),
			boost::bind(&MtActorSkill::SkillId, _1) == skill_id_);
		if (skill_it != skills.end()) {
			auto e = boost::make_shared<UseSkillEvent>();
			e->owner_guid_ = owner->Guid();
			e->skill_id_ = skill_id_;
			owner->BattleGround()->PushEvent(e);
		}
	}


	boost::shared_ptr<Buff> CastSkillLayerTrigger::Clone()
	{
		return boost::make_shared<CastSkillLayerTrigger>(*this);
	}

	void ComboPoint::TakeEffect()
	{
		auto creator = GetCreator();
		auto owner = GetOwner();
		if (creator == nullptr || owner == nullptr) return;
		SkillContext ctx;
		ctx.source_.actor_ = creator;
		ctx.target_.actor_ = owner;
		ctx.buff_.buff_ = this;
		creator->AfterBuffMaxLayer(ctx);
	}


	boost::shared_ptr<Buff> ComboPoint::Clone()
	{
		return boost::make_shared<ComboPoint>(*this);
	}

	bool LifeBurst::OnDestroy() const
	{
		auto owner = GetOwner();
		auto creator = GetCreator();
		if (owner == nullptr|| creator == nullptr) return false;
		Packet::ActorOnCure cure_msg;
		cure_msg.set_cure(cure_value_);
		cure_msg.set_source_guid(creator_guid_);
		cure_msg.set_target_guid(owner_guid_);
		owner->OnCure(cure_msg, creator);
		return Buff::OnDestroy();
	}


	boost::shared_ptr<Buff> LifeBurst::Clone()
	{
		return boost::make_shared<LifeBurst>(*this);
	}

	bool CastSkillWhenDestroy::OnDestroy() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		auto skill_it = boost::find_if(owner->Skills(),
			boost::bind(&MtActorSkill::SkillId, _1) == skill_id_);
		if (skill_it != owner->Skills().end()) {
			(*skill_it)->TempLevel(skill_level_);
			owner->PushSkill(skill_id_, {});
		}
		return Buff::OnDestroy();
	}


	boost::shared_ptr<Buff> CastSkillWhenDestroy::Clone()
	{
		return boost::make_shared<CastSkillWhenDestroy>(*this);
	}

	bool Disappear::OnDestroy() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		auto skill_it = boost::find_if(owner->Skills(),
			boost::bind(&MtActorSkill::SkillId, _1) == skill_id_);
		if (skill_it != owner->Skills().end()) {
			owner->PushSkill(skill_id_, {});
		}
		return base::OnDestroy();
	}


	boost::shared_ptr<Buff> Disappear::Clone()
	{
		return boost::make_shared<Disappear>(*this);
	}

	bool Shocker::AfterCreate() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		if (!stun_) return false;
		Packet::ActorAddState notify;
		notify.set_target_guid(owner_guid_);
		notify.set_state(Config::BattleObjectStatus::STUN);
		notify.set_time_stamp(owner->BattleGround()->TimeStamp());
		owner->AddState(Config::BattleObjectStatus::STUN);
		return true;
	}


	bool Shocker::OnDestroy() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		if (stun_) {
			Packet::ActorRemoveState notify;
			notify.set_target_guid(owner_guid_);
			notify.set_state(Config::BattleObjectStatus::STUN);
			notify.set_time_stamp(owner->BattleGround()->TimeStamp());
			owner->ClearState(Config::BattleObjectStatus::STUN);
		}
		return Buff::OnDestroy();
	}


	boost::shared_ptr<Buff> Shocker::Clone()
	{
		return boost::make_shared<Shocker>(*this);
	}

	bool IceBlock::AfterCreate() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		ClearAllDebuff();
		Packet::ActorAddState notify;
		notify.set_target_guid(owner_guid_);
		notify.set_state(Config::BattleObjectStatus::FREEZE);
		notify.set_time_stamp(owner->BattleGround()->TimeStamp());
		owner->AddState(Config::BattleObjectStatus::FREEZE);
		return true;
	}


	bool IceBlock::OnDestroy() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		Packet::ActorRemoveState notify;
		notify.set_target_guid(owner_guid_);
		notify.set_state(Config::BattleObjectStatus::FREEZE);
		notify.set_time_stamp(owner->BattleGround()->TimeStamp());
		owner->ClearState(Config::BattleObjectStatus::FREEZE);
		return CastSkillWhenDestroy::OnDestroy();
	}


	boost::shared_ptr<Buff> IceBlock::Clone()
	{
		return boost::make_shared<IceBlock>(*this);
	}

	void Sheep::OnOwnerDamage(Packet::ActorOnDamage&, MtActor*)
	{
		SetLayer(0);
	}


	boost::shared_ptr<Buff> Sheep::Clone()
	{
		return boost::make_shared<Sheep>(*this);
	}

	bool BladeFlurry::AfterCreate() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		Json::Value extra;
		extra["count"] = Json::Value(Json::ValueType::arrayValue);
		extra["count"][0] = 1;
		extra["count"][1] = extra_count_;
		extra["type"] = "circle";
		extra["range"] = range_;
		owner->TarExtra(extra);
		return base_class::AfterCreate();
	}


	bool BladeFlurry::OnDestroy() const
	{
		auto owner = GetOwner();
		if (owner == nullptr) return false;
		Json::Value extra;
		owner->TarExtra(extra);
		return base_class::OnDestroy();
	}


	boost::shared_ptr<Buff> BladeFlurry::Clone()
	{
		return boost::make_shared<BladeFlurry>(*this);
	}

	boost::shared_ptr<Buff> MarkBuff::Clone()
	{
		return boost::make_shared<MarkBuff>(*this);
	}


	boost::shared_ptr<Buff> ContinueBuff::Clone()
	{
		return boost::make_shared<ContinueBuff>(*this);
	}


	boost::shared_ptr<Buff> ReduceLevel2::Clone()
	{
		return boost::make_shared<ReduceLevel2>(*this);
	}


	boost::shared_ptr<Buff> SweepAway::Clone()
	{
		return boost::make_shared<SweepAway>(*this);
	}


	boost::shared_ptr<Buff> AddHpMax::Clone()
	{
		return boost::make_shared<AddHpMax>(*this);
	}


	boost::shared_ptr<Buff> SlowDown::Clone()
	{
		return boost::make_shared<SlowDown>(*this);
	}


	boost::shared_ptr<Buff> Sneer::Clone()
	{
		return boost::make_shared<Sneer>(*this);
	}


	boost::shared_ptr<Buff> God::Clone()
	{
		return boost::make_shared<God>(*this);
	}


	boost::shared_ptr<Buff> FakeDead::Clone()
	{
		return boost::make_shared<FakeDead>(*this);
	}


	bool FakeDead::AfterCreate() const
	{
		ClearAllDebuff();
		return true;
	}

	boost::shared_ptr<Buff> AddCriticalPercent::Clone()
	{
		return boost::make_shared<AddCriticalPercent>(*this);
	}


	boost::shared_ptr<Buff> AddDodgePercent::Clone()
	{
		return boost::make_shared<AddDodgePercent>(*this);
	}


	boost::shared_ptr<Buff> AddCureEffect::Clone()
	{
		return boost::make_shared<AddCureEffect>(*this);
	}


	boost::shared_ptr<Buff> ReduceCureEffect::Clone()
	{
		return boost::make_shared<ReduceCureEffect>(*this);
	}


	boost::shared_ptr<Buff> Fury::Clone()
	{
		return boost::make_shared<Fury>(*this);
	}


	bool PulseIntance::OnPulse()
	{
		if (bg_.expired()) {
			return false;
		}
		if (GetConfig().isMember("on_pulse_stub")) {
			auto owner = GetOwner();
			auto creator = GetCreator();
			SkillContext ctx;
			ctx.buff_.buff_ = this;
			ctx.source_.actor_ = creator;
			ctx.target_.actor_ = owner;
			if (owner == nullptr || creator == nullptr) return false;
			auto skill = creator->FindRuntimeSkill(this->GetCreateSkillId());
			if (skill != nullptr) {
				ctx.skill_.skill_ = skill;
				ctx.CalcBuffEffectValue();
				thread_lua->call_fun<void>(GetConfig()["on_pulse_stub_func"].asString(), &ctx);
			}
			return true;
		}
		return false;
	}

	boost::shared_ptr<Buff> PulseIntance::Clone()
	{
		return boost::make_shared<PulseIntance>(*this);
	}

}
}
