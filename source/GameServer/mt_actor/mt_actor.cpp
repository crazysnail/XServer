
#include "mt_actor.h"
#include "mt_actor_state.h"
#include "mt_actor_config.h"
#include "mt_battle_state.h"
#include "active_model.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_player/mt_player.h"
#include "../mt_guid/mt_guid.h"
#include "../mt_skill/mt_skill.h"
#include "../mt_battle/mt_battle_command.h"
#include "../mt_skill/mt_buff.h"
#include "../mt_battle/battle_event.h"
#include "../mt_skill/skill_context.h"
#include "../mt_item/mt_item.h"
#include "../mt_item/mt_container.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_monster/mt_monster_manager.h"
#include "../mt_market/mt_market.h"
#include "../mt_arena/geometry.h"
#include "../mt_battle/mt_battle_ground.h"
#include "../mt_battle/battle_ability_manager.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../mt_data_cell/mt_data_container.h"
#include "../mt_config/mt_config.h"
#include "../ranklist/top_ranklist.h"
#include <ActorBasicInfo.pb.h>
#include <ActorBasicInfo.proto.fflua.h>
#include <PlayerBasicInfo.pb.h>
#include <PlayerBasicInfo.proto.fflua.h>
#include <MonsterConfig.pb.h>
#include <ActorConfig.pb.h>
#include <BattleExpression.pb.h>
#include <boost/make_shared.hpp>
#include <SkillConfig.pb.h>
#include <SceneBroadCast.pb.h>
#include <EquipmentConfig.pb.h>
#include <SceneCommon.pb.h>
#include <BaseConfig.pb.h>
#include <Upgrades.pb.h>
#include <algorithm>  
#include <limits>

#ifdef ASSERT_MAIN_THREAD
#undef ASSERT_MAIN_THREAD
#define ASSERT_MAIN_THREAD() void()
#endif

namespace Mt
{

	void MtActor::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtActor, ctor()>(ls, "MtActor")
			.def(&MtActor::Guid, "Guid")
			.def(&MtActor::MaxHp, "MaxHp")
			.def(&MtActor::Hp, "Hp")
			.def(&MtActor::SetHp, "SetHp")
			.def(&MtActor::ActorType, "ActorType")
			.def(&MtActor::SetActorType, "SetActorType")
			.def(&MtActor::GetLastHp, "GetLastHp")
			.def(&MtActor::SetLastHp, "SetLastHp")
			.def(&MtActor::OnHpCure, "OnHpCure")
			.def(&MtActor::Level, "Level")
			.def(&MtActor::LuaCurrEquipments,"CurrEquipments")
			.def(&MtActor::LevelUpTo,"LevelUpTo")
			.def(&MtActor::LuaPlayer, "Player")
			.def(&MtActor::OnBattleInfoChanged, "OnBattleInfoChanged")
			.def(&MtActor::Professions, "Professions")
			.def(&MtActor::Score, "Score")
			.def(&MtActor::ChangeColor,"ChangeColor")
			.def(&MtActor::ChangeStar, "ChangeStar")
			.def(&MtActor::DbInfo, "ActorDBInfo")
			.def(&MtActor::DbSkills, "DbSkills")
			.def(&MtActor::RefreshData2Client, "RefreshData2Client")
			.def(&MtActor::RefreshSkillData2Client, "RefreshSkillData2Client")
			.def(&MtActor::AddEquipByConfigId, "AddEquipByConfigId")
			.def(&MtActor::GetSimpleImpactSet, "GetSimpleImpactSet")
			.def(&MtActor::ConfigId, "ConfigId")
			;
	}

	MtActor::MtActor(const boost::shared_ptr<Packet::ActorBasicInfo> info,
		MtPlayer& player)
		:player_(player.weak_from_this()), db_info_(info)
	{
		runtime_info_.battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());
		simple_impacts_.Init(this);
	}

	MtActor::MtActor(const MtActor& rhs)
		:player_(rhs.player_), db_info_(rhs.db_info_)
	{
		runtime_info_.battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());

	}
	void MtActor::InitAi()
	{
		/*
				auto tmp_ai = behaviac::Agent::Create<NormalAiAdaptor>();
				tmp_ai->SetActor(shared_from_this());
				tmp_ai->btsetcurrent("NormalAi");
				BTAgent(tmp_ai);*/
		state_ = boost::make_shared<BattleState::Idle>(this);
	}

/*
	MtActor::MtActor(const Packet::ActorFullInfo& full_info)
	{
		db_info_ = boost::make_shared<Packet::ActorBasicInfo>(full_info.actor());
		runtime_info_.battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());
		skill_build_ = MtSkillManager::Instance().GetSkillBuild(db_info_->actor_config_id(), db_info_->color());
		full_info_.CopyFrom(full_info);
	}*/

	void MtActor::InitBattleInfo(const MtActor& rhs)
	{
		if (runtime_info_.skills_.size() != rhs.skills_.size()) {
			runtime_info_.skills_.clear();
			//复制技能, 使用独立的cool down
			for (auto& skill : rhs.skills_)
			{
				runtime_info_.skills_.push_back(boost::make_shared<MtActorSkill>(*skill->DbInfo(), this));
			}
			skill_build_ = MtSkillManager::Instance().GetSkillBuild(rhs.db_info_->actor_config_id(), rhs.db_info_->color());
		}
		InitAi();
	}

	void MtActor::InitBattleInfo(const Packet::ActorFullInfo& msg)
	{
		for (auto& skill : msg.skills()) {
			runtime_info_.skills_.push_back(boost::make_shared<MtActorSkill>(skill, this));
		}
		skill_build_ = MtSkillManager::Instance().GetSkillBuild(msg.actor().actor_config_id(), msg.actor().color());
		InitAi();
	}

	bool MtActor::OnBigTick(zxero::uint64 elapseTime)
	{
		simple_impacts_.SimpleImpactUpdate(elapseTime);

		return true;
	}

	bool MtActor::OnTick(zxero::uint64 elapseTime)
	{
		std::for_each(std::begin(runtime_info_.skills_), std::end(runtime_info_.skills_), boost::bind(&MtActorSkill::OnTick, _1, elapseTime));
		decltype(buffs_) tmp_buff(buffs_.begin(), buffs_.end());
		std::for_each(tmp_buff.begin(), tmp_buff.end(), boost::bind(&Buff::OnTick, _1, elapseTime));
		auto it = std::partition(buffs_.begin(), buffs_.end(), !boost::bind(&Buff::NeedDestroy, _1));
		std::for_each(it, buffs_.end(), boost::bind(&Buff::OnDestroy, _1));
		buffs_.erase(it, buffs_.end());

		if (battle_ground_->GetAllDeadCampID() == 0
			&& state_ != nullptr) {
			state_->OnTick(elapseTime);
		}
		/*
				if (bt_agent_ && battle_ground_.lock()->GetAllDeadCampID() == 0) {
					bt_agent_->btexec();
				}*/


		return false;
	}

	void MtActor::RefreshData2Client()
	{
		Packet::ActorListReply reply;
		SerializationToMessage(*reply.add_actors());
		SendMessage(reply);
	}

	void MtActor::Refresh2ClientImpactList()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return;
		}
		Packet::SimpleImpactUpdate msg;
		msg.set_actor_guid(Guid());

		simple_impacts_.foreach([&](auto ptr) {
			auto impact = msg.add_simple_impact();
			impact->CopyFrom(*ptr);
		});
		player->SendMessage(msg);
	}

	void MtActor::AutoMedic()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return;
		}
		auto bg = BattleGround();
		if (bg != nullptr) {
			if (!player->GetOnHookOption().auto_use_potion()) {
				return;
			}
			if ( bg->GetParams32("script_id") == 1002 ) {
				int32 hp_limit = (int32)(player->GetOnHookOption().auto_potion_hp()*0.01*MaxHp());
				if (Hp() <= hp_limit) {
					int32 id = player->GetOnHookOption().auto_potion_id();
					auto item = player->FindItemByConfigId(Packet::ContainerType::Layer_ItemBag, id);
					if (item != nullptr) {
						player->UseItem(item->Guid(), Guid());
					}

					if (!player->GetOnHookOption().auto_buy_potion())
						return;

					//自动买药
					auto co = player->FindContainer(Packet::ContainerType::Layer_ItemBag);
					if (co == nullptr)
						return;
					int32 count = co->GetItemCount(id);
					int32 buycount = player->GetOnHookOption().auto_buy_count() - count;
					if (buycount > 0) {
						auto item_config = MtMarketManager::Instance().FindItemByItemId(id);
						if (item_config != nullptr) {
							int32 cost = buycount*item_config->price_value();
							if (cost <= player->GetPlayerToken(Packet::TokenType::Token_Gold)) {
								player->BuyMarketItem(item_config->market_id(), item_config->id(), buycount, true);
							}
						}
					}
				}
			}
		}
	}

	bool MtActor::EnterBattleGround(MtBattleGround* battle_ground)
	{
		auto player = player_.lock();
		battle_ground_ = battle_ground;
		if (player != nullptr && !player->EnterBattleGround(battle_ground))
			return false;

		return true;
	}

	bool MtActor::EnterState(const boost::shared_ptr<MtActorState>& strategy)
	{
		if (runtime_info_.strategy_ == nullptr) {
			LOG_INFO << "MtActor " << this << " enter state: " << typeid(*strategy).name();
			runtime_info_.strategy_ = strategy;
			return true;
		}
		else {
			if (runtime_info_.strategy_->EnterState(strategy)) {
				LOG_INFO << "MtActor " << this << " change  state: " << typeid(*runtime_info_.strategy_).name() << " -> " << typeid(*strategy).name();
				runtime_info_.strategy_ = strategy;
				return true;
			}
			else {
				ZXERO_ASSERT(false) << "wrong state change " << runtime_info_.strategy_->State() << " -> " << strategy->State();
				return false;
			}
		}
	}

	bool MtActor::CanMove() const
	{
		return ability_.CanDo(Config::BattleObjectAbility::MOVE);
	}

	bool MtActor::CanUseBaseSkill() const
	{
		return runtime_info_.skills_.size() > 0
			&& ability_.CanDo(Config::BattleObjectAbility::USE_BASE_SKILL);
	}

	bool MtActor::CanUseSkill() const
	{
		return CanUseBaseSkill() || CanUseMagicSkill();
	}

	bool MtActor::CanUseMagicSkill() const
	{
		return runtime_info_.skills_.size() > 0 && ability_.CanDo(Config::BattleObjectAbility::USE_MAGIC_SKILL);
	}

	bool MtActor::CanChangeTarget() const
	{
		return ability_.CanDo(Config::BattleObjectAbility::CHANGE_CURRENT_TARGET);
	}

	bool MtActor::CanBeDamage() const
	{
		return ability_.CanDo(Config::BattleObjectAbility::CAN_BE_DAMAGE);
	}

	bool MtActor::CanBeSelect(MtActor* source) const
	{
		if (BattleGround()->IsTeamMate(*this, *source))
			return ability_.CanDo(Config::BattleObjectAbility::BE_TEAMMATE_SELECT);
		else
			return ability_.CanDo(Config::BattleObjectAbility::BE_ENEMY_SELECT);
	}

	bool MtActor::CanSelectTarget(MtActor* target) const
	{
		if (BattleGround()->IsTeamMate(*this, *target))
			return ability_.CanDo(Config::BattleObjectAbility::SELECT_TEAMMATE);
		else
			return ability_.CanDo(Config::BattleObjectAbility::SELECT_ENEMY);
	}

	bool MtActor::OnDead(MtActor* killer)
	{
		if (IsDead())
			return false;
		uint64 killer_guid = INVALID_GUID;
		if (killer != nullptr) killer_guid = killer->Guid();
		std::for_each(std::begin(buffs_), std::end(buffs_), boost::bind(&Buff::OnDestroy, _1));
		std::for_each(std::begin(buffs_), std::end(buffs_), boost::bind(&Buff::OnOwnerDead, _1));
		Packet::ActorDead dead_message;
		dead_message.set_source_guid(killer_guid);
		dead_message.set_target_guid(Guid());
		dead_message.set_time_stamp(BattleGround()->TimeStamp());
		BattleGround()->BroadCastMessage(dead_message);
		AddState(Config::BattleObjectStatus::DEAD);
		state_ = boost::make_shared<BattleState::Idle>(this);
		/*
				if (bt_agent_)
					bt_agent_->btresetcurrent();*/
		if (killer != nullptr)
			killer->OnKillSomeOne(this);
		return true;
	}

	void MtActor::OnKillSomeOne(MtActor* /*target*/)
	{
		/*
				//重置ai
				if (behavior_node_)
					behavior_node_->RunningNode(nullptr);*/
	}

	bool MtActor::IsDead()
	{
		return GetState(Config::BattleObjectStatus::DEAD);
	}

	MtBattleGround* MtActor::BattleGround() const
	{
		return	battle_ground_;
	}

	const zxero::real64 MtActor::Direction() const
	{
		return runtime_info_.direction;
	}

	void MtActor::Direction(const zxero::real64 dir)
	{
		runtime_info_.direction = dir;
	}

	void MtActor::Position(const Packet::Position& pos)
	{
		boost::geometry::assign(runtime_info_.pos, pos);
	}

	const Packet::Position& MtActor::Position() const
	{
		return runtime_info_.pos;
	}

	int32 MtActor::SkillRange() const
	{
		if (CurrSkill())
			return CurrSkill()->SkillRange();
		else
			return 0;
	}
	int32 MtActor::AttackRadius() const
	{
		return SkillRange() + ObjectRadius();
	}

	zxero::uint64 MtActor::Guid() const
	{
		return db_info_->guid();
	}


	bool MtActor::operator==(MtActor* rhs)
	{
		return this->Guid() == rhs->Guid();
	}

	const boost::shared_ptr<ActorBattleCopy> MtActor::CreateBattleCopy()
	{
		auto result = boost::make_shared<ActorBattleCopy>(shared_from_this());
		result->InitBattleInfo(*this);
		return result;
	}

	boost::shared_ptr<ActorBattleCopy> MtActor::CreateBattleCopyFromMsg(const Packet::ActorFullInfo& full_info)
	{
		auto actor = boost::make_shared<ActorBattleCopy>(full_info);
		actor->InitBattleInfo(full_info);
		return actor;
	}


	bool MtActor::MainActor() const {
		return db_info_->type() == Packet::ActorType::Main;
	}


	void MtActor::AddExp(int32 exp, bool notify)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}

		//上限保护
		if (db_info_->exp() + exp >= MAX_INT32) {
			db_info_->set_exp(MAX_INT32-1);
		}
		else {
			db_info_->set_exp(db_info_->exp() + exp);
		}

		while (MtActorConfig::Instance().NeedLevelUp(ActorType(), db_info_->level(), db_info_->exp())) {
			if (!LevelUpTo(db_info_->level() + 1, false, notify)) {
				break;//防死循环
			}
		}
		if (notify) {
			Packet::ActorLevelAndExpUpdate msg;
			msg.set_guid(Guid());
			msg.set_new_level(db_info_->level());
			msg.set_new_exp(db_info_->exp());
			player->SendMessage(msg);
		}
	}

	void MtActor::ChangeColor(Packet::ActorColor color)
	{
		if (db_info_->color() != color) {
			auto config = MtActorConfig::Instance().GetBaseConfig(db_info_->actor_config_id());
			auto skill_count = skills_.size();
			if (config) {
				switch (color)
				{
				case Packet::ActorColor::WHILE:
					break;
				case Packet::ActorColor::GREEN_0:
					if (config->green_skill > 0
						&& std::find_if(std::begin(skills_), std::end(skills_),
							boost::bind(&MtActorSkill::SkillId, _1) == config->green_skill) == std::end(skills_)) {
						this->AddSkill(config->green_skill);
					}
					break;
				case Packet::ActorColor::GREEN_1:
					break;
				case Packet::ActorColor::BLUE_0:
					if (config->blue_skill > 0
						&& std::find_if(std::begin(skills_), std::end(skills_),
							boost::bind(&MtActorSkill::SkillId, _1) == config->blue_skill) == std::end(skills_)) {
						this->AddSkill(config->blue_skill);
					}
					break;
				case Packet::ActorColor::BLUE_1:
					break;
				case Packet::ActorColor::BLUE_2:
					break;
				case Packet::ActorColor::PURPLE_0:
					if (config->purple_skill > 0
						&& std::find_if(std::begin(skills_), std::end(skills_),
							boost::bind(&MtActorSkill::SkillId, _1) == config->purple_skill) == std::end(skills_)) {
						this->AddSkill(config->purple_skill);
					}
					break;
				default:
					break;
				}
			}
			if (skill_count != skills_.size()) {
				Packet::ActorSkillRefresh msg;
				msg.set_actor_guid(Guid());
				for (auto& skill : skills_) {
					msg.add_skills()->CopyFrom(*skill->DbInfo());
				}
				SendMessage(msg);
			}
			db_info_->set_color(color);
			OnBattleInfoChanged();

			auto player = player_.lock();
			if (player == nullptr) {
				ZXERO_ASSERT(false);
			} else {
				player->OnLuaFunCall_x("xOnUpgradeColor",
				{
					{ "value",color }
				});
			}
		}
	}

	void MtActor::ChangeStar(Packet::ActorStar star)
	{
		if (db_info_->star() != star) {
			auto config = MtActorConfig::Instance().GetBaseConfig(db_info_->actor_config_id());
			if (config) {
				auto star_skill = config->star_skill;
				switch (star)
				{
				case Packet::ActorStar::ONE:
					break;
				case Packet::ActorStar::TWO:
					break;
				case Packet::ActorStar::THREE:
				case Packet::ActorStar::FOUR:
				case Packet::ActorStar::FIVE:
					if (star_skill> 0 && std::none_of(skills_.begin(), skills_.end(),
							[=](auto& s) {return s->SkillId() == star_skill;})) {
						this->AddSkill(star_skill, int32(star - Packet::ActorStar::TWO));
					}
					break;
				}
				if (star >= Packet::ActorStar::THREE) {
					auto skill_it = std::find_if(skills_.begin(), skills_.end(), [&](auto& s) {return s->SkillId() == config->star_skill; });
					if (skill_it != skills_.end()) {
						auto& skill = *skill_it;
						skill->DbInfo()->set_skill_level(int32(star - Packet::ActorStar::TWO));
						Packet::ActorSkillRefresh msg;
						msg.set_actor_guid(Guid());
						msg.add_skills()->CopyFrom(*skill->DbInfo());
						SendMessage(msg);
					}
				}
			}
			db_info_->set_star(star);
			OnBattleInfoChanged();
			auto player = player_.lock();
			if (player == nullptr) {
				ZXERO_ASSERT(false);
			} else {
				player->OnLuaFunCall_x("xOnUpgradeStar",
				{
					{ "value",star }
				});
			}
		}
	}

	bool MtActor::FullExp() const
	{
		auto level_config = MtActorConfig::Instance().GetLevelData(Level());
		if (level_config == nullptr) {
			ZXERO_ASSERT(level_config == nullptr);
			return false;
		}
		if (ActorType() == Packet::ActorType::Hero) {
			return db_info_->exp() + 1 == level_config->hero_actor_exp();
		}
		else {
			return db_info_->exp() + 1 == level_config->main_actor_exp();
		}
	}

	int32 MtActor::LevelUpNeedExp() const
	{
		auto level_config = MtActorConfig::Instance().GetLevelData(Level());
		if (level_config == nullptr) {
			//ZXERO_ASSERT(level_config);
			return  std::numeric_limits<int32>::max();
		}
		int need_exp = level_config->main_actor_exp();
		if (ActorType() == Packet::ActorType::Hero) {
			need_exp = level_config->hero_actor_exp();
		}
		return need_exp - db_info_->exp();
	}
	void MtActor::SetLevel(int32 level, bool notify)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		auto update = false;
		if (db_info_->level() != level) update = true;
		db_info_->set_level(level);
		if (update && notify) {
			Packet::ActorLevelAndExpUpdate msg;
			msg.set_guid(Guid());
			msg.set_new_level(db_info_->level());
			msg.set_new_exp(db_info_->exp());
			player->SendMessage(msg);
			OnBattleInfoChanged();

			//player升级判定,这里务必放到最后判定
			if (ActorType() == Packet::ActorType::Main) {
				player->OnLevelUpTo(level);
			}
		}
		player->OnLuaFunCall_x("xActorLevelUpTo",
		{
			{ "value", level }
		});
	}

	bool MtActor::LevelUpTo(int32 level, bool gm /*= false*/, bool notify /*= true*/)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		if (level < 1 || level > 60) {
			return false;
		}

		bool isMain = ActorType() == Packet::ActorType::Main;
		if (isMain && !player->ServerLevelCheck(level)) {
			return false;
		}

		auto level_config = MtActorConfig::Instance().GetLevelData(Level());
		if (level_config == nullptr)
			return false;

		int32 dec_exp = 0;
		if (isMain) {
			dec_exp = level_config->main_actor_exp();
		}
		else {
			auto main_actor_level = player->MainActor()->Level();
			if (level > main_actor_level) {//actor等级不能超过主角
				db_info_->set_exp(level_config->hero_actor_exp() - 1);
				db_info_->set_level(main_actor_level);
				Packet::ActorLevelAndExpUpdate msg;
				msg.set_guid(Guid());
				msg.set_new_level(db_info_->level());
				msg.set_new_exp(db_info_->exp());
				player->SendMessage(msg);
				return false;
			}
			dec_exp = level_config->hero_actor_exp();
		}
		if (gm == false) {
			db_info_->set_exp(db_info_->exp() - dec_exp);
		}
		else {
			db_info_->set_exp(0);
		}
		SetLevel(level, notify);

		return true;
	}

	void MtActor::RefreshSkillData2Client()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}

		Packet::ActorSkillRefresh msg;
		msg.set_actor_guid(Guid());
		auto skills = DbSkills();
		for (auto& skill : skills) {
			msg.add_skills()->CopyFrom(*(skill->DbInfo()));
		}
		player->SendMessage(msg);
	}
	void MtActor::OnBattleInfoChanged(bool init) {

		//profiler timer2("OnBattleInfoChanged");
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (db_info_->type() != Packet::ActorType::Main && db_info_->type() != Packet::ActorType::Hero)
			return;
		attribute_set attributes;
		std::vector<MtEquipItem*> curr_equips;
		CurrEquipments(curr_equips, true);
		for (auto& equip : curr_equips) {
			equip->EquipEffect(attributes);
		}
		//强化大师[废弃]
		//UpdateEnhanceMasterEffect(attributes);
		//宝石大师
		UpdateGemMasterEffect(attributes);
		//处理套装属性
		UpdateSuitEquipEffect(attributes);
		//坐骑属性解锁
		UpdateMountCountEffect(attributes);
		//buff
		UpdateSimpleImpcatEffect(attributes);
		MtActorConfig::Instance().InitBaseActorBattleInfo(*db_info_, attributes);
		auto battle_score = MtActorConfig::BattleInfo2Score(db_info_->battle_info());
		db_info_->set_score(battle_score + SkillScore());
		Packet::ActorBattleInfoNotify notify;
		notify.set_guid(Guid());
		notify.mutable_battle_info()->CopyFrom(db_info_->battle_info());
		notify.set_score(db_info_->score());
		player->SendMessage(notify);
		player->UpdateBattleScore(init);
		MtTopRankManager::Instance().OnMainActorScoreChanged(player);
	}

	const boost::shared_ptr<Packet::BattleInfo> MtActor::RunTimeBattleInfo() const
	{
		return runtime_info_.battle_info_;
	}

	void MtActor::OnDamageRefix(Packet::ActorOnDamage& damage_info, MtActor* killer)
	{
		auto src_player = killer->Player();
		if (src_player.expired()) {
			return;
		}
		auto container = src_player.lock()->DataCellContainer();
		if (container == nullptr) {
			return;
		}
		if (container->check_bit_data(Packet::BitFlagCellIndex::MonthCardFlag)  ) {

			auto config = MtConfigDataManager::Instance().FindConfigValue("card_damage_value");
			if (config != nullptr && config->value1() > 0) {
				int32 value = config->value1();
				damage_info.set_damage((int32)(damage_info.damage()*(1.0 + (float)value/(float)100)));
			}

		}
	}

	bool MtActor::OnDamage(Packet::ActorOnDamage& damage_info,
		MtActor* killer,
		SkillContext* ctx /*= nullptr*/)
	{
		if (killer == nullptr) {
			return false;
		}
		//修炼
		OnPracticeDamage(damage_info, killer);
		//
		OnDamageRefix(damage_info, killer);
		boost::for_each(buffs_, boost::bind(&BuffImpl::Buff::OnOwnerDamage, _1, boost::ref(damage_info), killer));
		if (BattleGround())
		{
			BattleGround()->OnDamageRefix(damage_info, killer);
		}
		auto level_two = runtime_info_.battle_info_;
		auto old_hp = level_two->hp();
		if (CanBeDamage() == false) {
			damage_info.set_expression(Packet::DamageExpression::ActorOnDamage_DODGE);
			damage_info.set_old_hp(old_hp);
			damage_info.set_new_hp(old_hp);
			return false;
		}
		auto new_hp = old_hp - damage_info.damage();

		if (damage_info.expression() == Packet::DamageExpression::ActorOnDamage_DODGE) {
			new_hp = old_hp;
		}
		damage_info.set_old_hp(old_hp);
		damage_info.set_new_hp(new_hp);
		level_two->set_hp(new_hp);

		//自动吃药
		AutoMedic();
		BattleGround()->OnDamage(damage_info);
		if (new_hp <= 0) {
			SkillContext dead_ctx;
			dead_ctx.source_.actor_ = killer;
			if (this->BeforeDeadStub(dead_ctx)) {
				LOG_INFO << "[battle actor] guid:" << Guid()
					<< ",config:" << ConfigId() << ",dead event handle by skill or buff";
			} else {
				auto event = boost::make_shared<DelayDead>();
				event->dead_guid_ = Guid();
				event->killer_guid_ = killer->Guid();
				this->BattleGround()->PushEvent(event);
			}

			return false;
		}
		else {
			if (ctx == nullptr) {
				SkillContext skill_ctx;
				skill_ctx.source_.actor_ = killer;
				skill_ctx.target_.actor_ = this;
				skill_ctx.effect_value_ = real32(damage_info.damage());
				AfterHpChange(skill_ctx);
			}
			return false;
		}
	}


	void MtActor::OnPracticeDamage(Packet::ActorOnDamage& damage_info, MtActor* killer)
	{
		int32 add_damage = 0;
		switch (damage_info.type())
		{
		case Packet::DamageType::PHYSICAL:
		{
			add_damage += killer->GetPractice(Packet::Property::PERCENT_MAGIC_ATTACK);
			add_damage -= GetPractice(Packet::Property::PERCENT_MAGIC_ARMOR);
		}
		break;

		default:
		{
			add_damage += killer->GetPractice(Packet::Property::PERCENT_PHYSICAL_ATTACK);
			add_damage -= GetPractice(Packet::Property::PERCENT_PHYSICAL_ARMOR);
		}
		break;
		}

		damage_info.set_damage((int32)(damage_info.damage()*(1 + add_damage*0.01)));

	}

	int32 MtActor::GetPractice(Packet::Property pro)
	{
		auto player = player_.lock();
		if (player != nullptr) {
			player->GetPracticeProperty(pro);
		}
		return 0;
	}
	bool MtActor::OnRelive()
	{
		auto level_two = db_info_->battle_info();
		level_two.set_hp(MaxHp());
		ability_.ForceRemoveState(Config::BattleObjectStatus::DEAD);
		return true;
	}

	bool MtActor::OnHpCure(int32 hp_value)
	{
		if (GetState(Config::BattleObjectStatus::DEAD)) {
			return false;
		}
		else {
			auto level_two = runtime_info_.battle_info_;
			auto old_hp = level_two->hp();
			auto new_hp = old_hp + hp_value;

			if (new_hp > MaxHp()) {//恢复的hp需要考虑最大值
				new_hp = MaxHp();
			}
			level_two->set_hp(new_hp);
			Packet::SkillEffects skill_effect;
			Packet::ActorOnCure& cure_info = *skill_effect.add_cures();
			cure_info.set_source_guid(Guid());
			cure_info.set_target_guid(Guid());
			cure_info.set_cure(hp_value);
			cure_info.set_old_hp(old_hp);
			cure_info.set_new_hp(new_hp);

			LOG_INFO << "MtActor " << this << "On Cure." << cure_info.DebugString();
			if (BattleGround() != nullptr) {
				skill_effect.set_time_stamp(BattleGround()->TimeStamp());
				BattleGround()->BroadCastMessage(skill_effect);
			}

		}
		return false;
	}

	bool MtActor::OnCure(Packet::ActorOnCure& cure_info,
		MtActor* source,
		SkillContext* ctx /*= nullptr*/)
	{
		source;
		if (GetState(Config::BattleObjectStatus::DEAD)) {
			cure_info.set_new_hp(0);
			cure_info.set_old_hp(0);
			return false;
		}
		else {
			auto level_two = runtime_info_.battle_info_;
			auto old_hp = level_two->hp();
			auto new_hp = old_hp + cure_info.cure();

			cure_info.set_old_hp(old_hp);
			cure_info.set_new_hp(new_hp);

			if (new_hp > MaxHp()) {//恢复的hp需要考虑最大值
				new_hp = MaxHp();
			}
			level_two->set_hp(new_hp);
			if (ctx == nullptr) {
				SkillContext skill_ctx;
				skill_ctx.source_.actor_ = source;
				skill_ctx.target_.actor_ = this;
				skill_ctx.effect_value_ = real32(cure_info.cure());
				skill_ctx.cure_ = true;
				AfterHpChange(skill_ctx);
			}
			BattleGround()->OnCure(cure_info);
		}
		return false;
	}

	void MtActor::SendMessage(const google::protobuf::Message& message)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return;
		}
		player->SendMessage(message);
	}

	void MtActor::SerializationToBattleInfo(Packet::ActorBattleInfo& message) const
	{
		message.mutable_base_info()->CopyFrom(*db_info_);
		message.mutable_battle_info()->CopyFrom(*runtime_info_.battle_info_);
		message.set_state(Packet::ActorStateType::ACTOR_IDEL);
		boost::geometry::assign(*message.mutable_positon(), runtime_info_.pos);
		message.set_direction(runtime_info_.direction);
		message.set_object_radius(runtime_info_.object_radius);
		message.set_position_index(GetPositionIndex());
		message.set_side(runtime_info_.side_);
		std::vector<MtEquipItem*> equips;
		CurrEquipments(equips);
		for (auto& equip : equips) {
			equip->FillEquipInfo(*message.add_equips());
		}

		for (auto& e : full_info_.equips()) {
			message.add_equips()->CopyFrom(e);
		}
	}

	std::vector<boost::shared_ptr<MtActorSkill>>& MtActor::Skills()
	{
		return runtime_info_.skills_;
	}
	std::vector<boost::shared_ptr<MtActorSkill>>& MtActor::DbSkills()
	{
		return skills_;
	}
	void MtActor::OnSkillLoad(const std::vector<boost::shared_ptr<Packet::ActorSkill>>& skills)
	{
		for (auto child : skills) {
			child->ClearDirty();
			auto instance = boost::make_shared<MtActorSkill>(*child, this);
			if (instance != nullptr) {
				skills_.push_back(instance);
				instance->DbInfo()->ClearDirty();
			}				
		}
	}

	void MtActor::AddSkill(const boost::shared_ptr<MtActorSkill>& skill)
	{
		skills_.push_back(skill);
	}

	bool MtActor::AddSkill(int32 skill_id, int32 skill_level /*= 1*/)
	{
		if (CheckSkill(skill_id)){
			return false;
		}
		auto skill_db = boost::make_shared<Packet::ActorSkill>();
		skill_db->set_guid(MtGuid::Instance()(*skill_db));
		skill_db->set_actor_guid(this->Guid());
		skill_db->set_skill_id(skill_id);
		skill_db->set_skill_level(skill_level);
	
		auto mt_actor_skill = boost::make_shared<MtActorSkill>(*skill_db, this);
		skills_.push_back(mt_actor_skill);
		return true;
	}

	void MtActor::RemoveSkill(const int32 skill_id)
	{
		for (auto iter = skills_.begin(); iter != skills_.end(); ) {
			if ((*iter)->SkillId() == skill_id) {
				iter = skills_.erase(iter);
			}
			else {
				++iter;
			}
		}
	}

	bool MtActor::CheckSkill(const int32 skill_id)
	{
		for (auto child : skills_) {
			if (child->SkillId() == skill_id) {
				return true;
			}
		}
		return false;
	}
	void MtActor::ClearSkill() {
		skills_.clear();

		//装备技能需要保留
		OnBattleInfoChanged();
	}

	void MtActor::OnBattleRefresh() {
		//behavior_node_->RunningNode(nullptr);
		ability_.Clear();
		runtime_info_.battle_info_->CopyFrom(db_info_->battle_info());
		curr_skill_index = 0;
		state_ = boost::make_shared<BattleState::Idle>(this);
		//bt_agent_->btresetcurrent();
	}

	void MtActor::UnEquipment(MtEquipItem* equip)
	{
		ZXERO_ASSERT(equip->ActorGuid() == Guid());
		equip->Slot(Config::EquipmentSlot::EQUIPMENT_SLOT_INVALID);
		equip->ActorGuid(INVALID_GUID);
		OnBattleInfoChanged();
		NotifyEquipChange(equip, false);
	}

	MtEquipItem* MtActor::AddEquipByConfigId(const int32 config_id, Config::EquipmentSlot slot)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return nullptr;
		}
		auto item = MtItemManager::Instance().CreateItem(player.get(), config_id, 1);
		if (nullptr == item)
			return nullptr;
		auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
		if (nullptr == equip)
			return nullptr;
		if (!player->AddItem(equip, Config::ItemAddLogType::AddType_PlayerInit, 0))
			return nullptr;

		if (AddEquipment(equip.get(),slot)){
			return equip.get();
		}

		return nullptr;
	}

	bool MtActor::AddEquipment(MtEquipItem* new_equip, Config::EquipmentSlot slot)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return false;
		}
		if (new_equip == nullptr) {
			return false;
		}
		auto config = new_equip->Config();
		if (config == nullptr) {
			return false;
		}

		ZXERO_ASSERT(new_equip->Container() != nullptr);
		//替换同类装备
		std::vector<MtEquipItem*> curr_equips;
		CurrEquipments(curr_equips);
		auto to_be_replace_it = std::find_if(std::begin(curr_equips), std::end(curr_equips), boost::bind(&MtEquipItem::Slot, _1) == slot);
		if (to_be_replace_it != std::end(curr_equips)) {
			(*to_be_replace_it)->ActorGuid(INVALID_GUID);
			(*to_be_replace_it)->Slot(Config::EquipmentSlot::EQUIPMENT_SLOT_INVALID);
			auto container = player->FindContainer(Packet::ContainerType::Layer_EquipBag);
			if (container == nullptr)
				return false;

			container->AddItem((*to_be_replace_it)->shared_from_this(), Config::ItemAddLogType::AddType_Gm,false,true);
		}

		auto econtainer = player->FindContainer(Packet::ContainerType::Layer_Equip);
		if (econtainer == nullptr)
			return false;

		new_equip->ActorGuid(Guid());
		//new_equip->SetBindActorGuid(Guid());
		new_equip->Slot(slot);
		if (econtainer->AddItem(new_equip->shared_from_this())) {
			OnBattleInfoChanged();
			NotifyEquipChange(new_equip, true);

			player->OnLuaFunCall_x("xAddEquipment",
			{
				{ "value",slot }
			});

			return true;
		}
		return false;
	}

	void MtActor::UpdateMountCountEffect(attribute_set& changes)
	{	
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}

		std::map<std::string, int32> effect;
		auto container = player->DataCellContainer();
		if (container == nullptr) {
			return;
		}
		auto mount_count = container->get_count_data_32(Packet::CellLogicType::MountCell);
		if (mount_count > 0) {
			try {
				effect = thread_lua->call<std::map<std::string, int32>>(10, "GetMountCountEffect", mount_count);
			}
			catch (ff::lua_exception& e) {
				ZXERO_ASSERT(false) << "lua exception:" << e.what();
				return;
			}
		}
		for (auto child : effect) {
			MtActorConfig::AddAttribute(changes, child.first, child.second);
		}
	}

	//根据套装id，计算套装附加属性
	void MtActor::UpdateSuitEquipEffect(attribute_set& changes)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		std::map<int32, std::vector<int32>> old_suit_set;

		old_suit_set.clear();
		old_suit_set.insert(suit_set_.begin(), suit_set_.end());
		suit_set_.clear();

		std::vector<MtEquipItem*> curr_equips;
		CurrEquipments(curr_equips,true);

		//重新计算套装部件数量		
		int32 equip_id = -1;
		int32 suit_id = -1;
		Config::EquipmentConfig* edate = nullptr;

		for (auto &child : curr_equips)
		{
			edate = child->Config();
			suit_id = edate->suit_group_id();
			equip_id = edate->id();
			if (suit_id >= 0)
			{
				auto iter = suit_set_.find(suit_id);
				if ( iter != suit_set_.end())
				{
					auto equip = std::find(iter->second.begin(), iter->second.end(), equip_id);
					if (equip == iter->second.end())
					{//不同id计算进套装
						iter->second.push_back(equip_id);
					}
				}
				else
				{
					std::vector<int32> suit;
					suit.push_back(equip_id);
					suit_set_.insert(std::make_pair(suit_id, suit));
				}
			}
		}

		//新数据生效
		int32 now_count = 0;
		for (auto &child : suit_set_){
			now_count = (int32)child.second.size();
			const boost::shared_ptr<Config::EquipmentSuitConfig> suit_attr = MtItemManager::Instance().GetEquipmentSuitConfig(child.first);
			if (suit_attr != nullptr){
				for (int32 i = 0; i < suit_attr->part_count_size(); i++)
				{
					int32 active_count = suit_attr->part_count(i);
					if (active_count >= 0 && now_count >= active_count)
					{
						auto attr_id = suit_attr->part_attr(i);
						auto attr_value = suit_attr->part_value(i);
						MtActorConfig::AddAttribute(changes, attr_id, attr_value);
					}
				}
				if (player->Scene() != nullptr) {
					player->OnLuaFunCall_x("xUpdateSuitEquipEffect",
					{
						{ "value",suit_attr->part_count_size() }
					});
				}
			}
		}		
	}

	void MtActor::NotifyEquipChange(MtEquipItem* equip, bool load /*= true*/)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (this->ActorType() != Packet::ActorType::Main) {
			return;
		}
		if (load) {
			Packet::PlayerLoadEquip broad_msg;
			broad_msg.set_guid(player->Guid());
			equip->FillEquipInfo(*broad_msg.mutable_equip());
			player->BroadCastMessage(broad_msg);
		}
		else {
			Packet::PlayerUnloadEquip broad_msg;
			broad_msg.set_guid(player->Guid());
			equip->FillEquipInfo(*broad_msg.mutable_equip());
			player->BroadCastMessage(broad_msg);
		}
	}

	std::vector<MtEquipItem*> MtActor::LuaCurrEquipments(bool durable_filter)
	{
		std::vector<MtEquipItem*> temp;
		CurrEquipments(temp, durable_filter);
		return temp;
	}
	void MtActor::CurrEquipments(std::vector<MtEquipItem*>& result, bool durable_filter ) const
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return;
		}
		auto container = player->FindContainer(Packet::ContainerType::Layer_Equip);
		if (container == nullptr){
			return;
		}
		auto items = container->Items();
		for (auto& item : items) {
			auto equip = boost::dynamic_pointer_cast<MtEquipItem>(item);
			if (durable_filter&& equip->DbInfo()->durable()<=0.00001f) {
				continue;
			}
			if (equip->ActorGuid() != Guid()) {
				continue;
			}
			result.push_back(equip.get());
		}
	}
	void MtActor::PushSkill(int32 skill_id, std::vector<uint64> targets)
	{
		ZXERO_ASSERT(runtime_info_.skill_list_.size() <= 3) 
			<< "push skill:" << skill_id;
		runtime_info_.skill_list_.push_back(std::make_tuple(skill_id, targets));
	}

	int32 MtActor::SkillListSize() const 
	{
		return runtime_info_.skill_list_.size();
	}
	void MtActor::PopSkill()
	{
		if (runtime_info_.skill_list_.empty() == false)
			runtime_info_.skill_list_.pop_front();
	}

	MtActorSkill* MtActor::CurrSkill() const
	{
		if (runtime_info_.skill_list_.empty())
			return nullptr;
		int32 id = std::get<0>(runtime_info_.skill_list_.front());
		auto it = std::find_if(runtime_info_.skills_.begin(),
			runtime_info_.skills_.end(), boost::bind(&MtActorSkill::SkillId, _1) == id);
		if (it == runtime_info_.skills_.end())
			return nullptr;
		else
			return (*it).get();
	}

	std::vector<uint64> MtActor::CurrSkillTarget() const 
	{
		std::vector<uint64> ret;
		if (runtime_info_.skill_list_.empty())
			return ret;
		ret = std::get<1>(runtime_info_.skill_list_.front());
		return ret;
	}

	bool MtActor::SendCommand(const boost::shared_ptr<BattleCommand> command)
	{
		commands_.push_back(command);
		return true;
	}
	
	zxero::int32 MtActor::Hp()
	{
		int32 extra = std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->Hp();
		});
		if (extra < 0) { extra = 0; }
		return runtime_info_.battle_info_->hp() + extra;
	}

	void MtActor::SetHp(int32 hp)
	{
		runtime_info_.battle_info_->set_hp(hp);
	}

	zxero::int32 MtActor::GetLastHp(Packet::LastHpType type)
	{
		return db_info_->last_hp(type);
	}

	void MtActor::SetLastHp(Packet::LastHpType type, int32 hp)
	{
		db_info_->set_last_hp(type,hp);
	}

	zxero::int32 MtActor::PhysicalAttack()
	{
		int32 r = runtime_info_.battle_info_->physical_attack()
			+ std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->PhysicalAttack();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}

	zxero::int32 MtActor::PhysicalArmor()
	{
		int32 r = runtime_info_.battle_info_->physical_armor()
			+ std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->PhysicalArmor();
		});
		if (r < 0) {
			r =  1;
		}
		return r;
	}


	zxero::int32 MtActor::SkillTargetCount() const
	{
		return std::accumulate(buffs_.begin(), buffs_.end(), 0, [](auto accu, auto& impact) {
			return accu + impact->SkillTargetCount();
		});
	}

	zxero::int32 MtActor::MagicAttack()
	{
		int32 r =  runtime_info_.battle_info_->magic_attack() 
			+ std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->MagicAttack();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}

	int32 MtActor::MagicArmor()
	{
		int32 r = runtime_info_.battle_info_->magic_armor()
			+ std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->MagicArmor();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}
	int32 MtActor::Critical() const {
		int32 r = runtime_info_.battle_info_->critical() +
			std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->Critical();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}

	float MtActor::ExtraCriticalPercent() const
	{
		return std::accumulate(buffs_.begin(), buffs_.end(), 0.f,
			[](auto accu, auto& buff) {return accu + buff->CriticalPercent(); });
	}

	int32 MtActor::Toughness() const {
		int32 r = runtime_info_.battle_info_->toughness() +
			std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->Toughness();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}
	int32 MtActor::Dodge() const {
		int32 r = runtime_info_.battle_info_->dodge() +
			std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->Dodge();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}
	int32 MtActor::Accurate() const {
		int32 r = runtime_info_.battle_info_->accurate() +
			std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->Accurate();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}

	int32 MtActor::MoveSpeed() const
	{
		int32 r = runtime_info_.battle_info_->move_speed() +
			std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->MoveSpeed();
		});
		if (r < 0) {
			r = 1;
		}
		return r;
	}

	bool MtActorAbility::AddState(Config::BattleObjectStatus state)
	{
		status_[(zxero::uint32)state]++;
		ability_ = 0xffffff;
		for (auto i = 0; i < Config::BattleObjectStatus_ARRAYSIZE; ++i) {
			if (status_[i] > 0)
				ability_ &= BattleAbilityManager::Instance().StatusToAbility(Config::BattleObjectStatus(i));
		}
		return true;
	}

	void MtActorAbility::ForceRemoveState(Config::BattleObjectStatus state)
	{
		status_[(zxero::uint32)state] = 0;
		ability_ = 0xffffff;
		for (auto i = 0; i < Config::BattleObjectStatus_ARRAYSIZE; ++i) {
			if (status_[i] > 0)
				ability_ &= BattleAbilityManager::Instance().StatusToAbility(Config::BattleObjectStatus(i));
		}
	}

	bool MtActorAbility::GetState(Config::BattleObjectStatus state)
	{
		return status_[(zxero::uint32)state] > 0;
	}

	bool MtActorAbility::ClearState(Config::BattleObjectStatus state)
	{
		status_[(zxero::uint32)state]--;
		ability_ = 0xffffff;
		for (auto i = 0; i < Config::BattleObjectStatus_ARRAYSIZE; ++i) {
			if (status_[i] > 0)
				ability_ &= BattleAbilityManager::Instance().StatusToAbility(Config::BattleObjectStatus(i));
		}
		return true;
	}

	void MtActor::BeforeBuffAdd(const boost::shared_ptr<Buff>& buff)
	{
		SkillContext ctx;
		ctx.buff_.buff_ = buff.get();
		BeforeBuffAddStub(ctx);
		boost::shared_ptr<BuffImpl::AddStatus> control_buff 
			= boost::dynamic_pointer_cast<BuffImpl::AddStatus>(buff);
		if (control_buff == nullptr) return;
		if (control_buff->IsControlSkill()){
			int32 prob = 100;
			int32 actor_level = Level();
			int32 buff_skill_level = buff->BuffLevel();
			int32 diff = actor_level - buff_skill_level;
			int32 prob_reduce = 0;
			if (1 < diff && diff <= 5)  prob_reduce = diff * 2;
			if (5 < diff && diff <= 10) prob_reduce = diff * 3 - 5;
			if (10 < diff && diff <= 15) prob_reduce = diff * 5 - 25;
			if (15 < diff && diff <= 20) prob_reduce = diff * 6 - 40;
			if (20 < diff) prob_reduce = 80;
			control_buff->SetControlProb(prob - prob_reduce);
			AfterControlBuffProbCalcStub(ctx);
		}
	}

	void MtActor::AfterBuffAdd(const boost::shared_ptr<Buff>& buff)
	{
		SkillContext ctx;
		ctx.buff_.buff_ = buff.get();
		AfterBuffAddStub(ctx);
	}

	void MtActor::AddBuff(const boost::shared_ptr<Buff>& buff)
	{
		if (GetState(Config::BattleObjectStatus::DEAD))
			return;
		buff->SetOwnerGuid(Guid());
		BeforeBuffAdd(buff);
		if (buff->AfterCreate() == false) {//buff 无法添加
			return;
		}
		auto same_buff = std::find_if(std::begin(buffs_), 
			std::end(buffs_),
			[&](auto& buff_ele) {
				return buff_ele->BuffId() == buff->BuffId()
					&& buff_ele->GetCreator() != nullptr
					&& buff->GetCreator() != nullptr
					&& buff_ele->GetCreator()->Guid() == buff->GetCreator()->Guid() ;
			});
		if (same_buff != std::end(buffs_))
		{//覆盖原有buff
			auto old_buff = *same_buff;
			old_buff->SetLifeTime(buff->LifeTime());
			if (old_buff->GetLayer() + buff->GetLayer() >= buff->MaxLayer()) {
				old_buff->SetLayer(buff->MaxLayer());
			}
			else {
				old_buff->SetLayer(old_buff->GetLayer() + buff->GetLayer());
			}
			old_buff->OnRefresh();
			AfterBuffAdd(old_buff);
		}
		else if (buff->OnAttach())
		{
			buffs_.push_back(buff);
			AfterBuffAdd(buff);
		}
		else {
			ZXERO_ASSERT(false) << "create buff fail";
		}
	}

	void MtActor::UpdateSimpleImpcatEffect(attribute_set& changes)
	{
		simple_impacts_.foreach([&](auto ptr) {
			if (ptr != nullptr) {
				auto config = MtItemManager::Instance().ImpactType2Config(ptr->impact_type());
				if (config && config->effect() == Packet::SimpleImpactEffectType::EF_ActorAttribute) {
					int32 type = ptr->param1();
					std::string name = Packet::Property_Name((Packet::Property)type);
					boost::algorithm::to_lower(name);
					MtActorConfig::AddAttribute(changes, name, ptr->param2());
				}
			}
		});

		/////////////////////////主角药水buff
		//auto player = player_.lock();
		//if (player == nullptr) {
		//	ZXERO_ASSERT(false);
		//	return;
		//}
		//if (ActorType() == Packet::ActorType::Main && player != nullptr) {
		//	auto impcat_set = player->GetSimpleImpactSet();
		//	if (impcat_set != nullptr) {
		//		impcat_set->foreach([&](auto ptr) {
		//			if (ptr != nullptr) {
		//				auto config = MtItemManager::Instance().ImpactType2Config(ptr->impact_type());
		//				if (config && config->effect() == Packet::SimpleImpactEffectType::EF_ActorAttribute) {
		//					int32 type = ptr->param1();
		//					std::string name = Packet::Property_Name((Packet::Property)type);
		//					boost::algorithm::to_lower(name);
		//					MtActorConfig::AddAttribute(changes, name, ptr->param2());
		//				}
		//			}
		//		});
		//	}
		//}
	}

	void MtActor::UpdateLackesImpcatEffect(attribute_set& changes)
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		if (db_info_ == nullptr) {
			return;
		}
		if (db_info_->type() != Packet::ActorType::Main) {
			return;
		}
		auto reflection = db_info_->battle_info().GetReflection();
		auto descriptor = db_info_->battle_info().GetDescriptor();
		if (descriptor == nullptr || reflection == nullptr) {
			return;
		}

		auto impacts = player->GetSimpleImpactSet();
		if (impacts == nullptr) {
			return;
		}
		auto main_actor = player->MainActor();
		if (main_actor == nullptr)		{
			return;
		}
		auto main_config = MtActorConfig::Instance().GetBaseConfig(main_actor->ConfigId());
		if (main_config == nullptr) {
			return;
		}
		//PlayerLackeysImpact
		if (impacts->OnCheckImpact(1016)) {
			auto dbinfo = player->GetDBPlayerInfo();
			if (dbinfo != nullptr) {
				for (int32 i = 0; i < dbinfo->lackeys_size(); i++) {
					auto hero = player->FindActor(dbinfo->lackeys(i));
					if (hero == nullptr) {
						continue;
					}
					auto config = MtActorConfig::Instance().GetLackeysConfig(i);
					if (config == nullptr) {
						continue;
					}

					std::vector<std::string> attributes;
					int32 value = 0;
					std::string name = "";
					boost::split(attributes, config->attribute1(), boost::is_any_of("|"));
					if (attributes.size() == 2) {
						if (main_config->hero_position == Packet::HeroPosition::MagicAttack ||
							main_config->hero_position == Packet::HeroPosition::Nanny) {
							name = "physical_attack";
						}
						else {
							name = "magic_attack";
						}			
					}
					else {
						name = config->attribute1();
					}
					if (name != "") {
						auto desc0 = descriptor->FindFieldByName(name);
						if (desc0 != nullptr) {
							value = reflection->GetInt32(db_info_->battle_info(), desc0);
						}
						if (value > 0) {
							float power_rate = (float)(hero->Score()) / (float)(main_actor->Score());
							float refix = power_rate*(float)(config->value()) / 1000.0f;
							MtActorConfig::AddAttribute(changes, name, (int32)(value*(1.0f + refix)));
						}
					}
				}
			}
		}
	}

	void MtActor::UpdateEnhanceMasterEffect(attribute_set& changes)
	{
		int32 old_min_level = m_min_equips_enhance_lv;
		int32 temp = 999;
		//计算新等级
		std::vector<MtEquipItem*> curr_equips;
		CurrEquipments(curr_equips);

		if (curr_equips.size() == Config::EquipmentSlot_ARRAYSIZE - 1) {
			for (auto &child : curr_equips) {
				if (temp > child->EnhenceLevel()){
					temp = child->EnhenceLevel();
				}
			}
		}

		m_min_equips_enhance_lv  = (temp == 999 ? 0 : temp);

		//计算效果
		if (old_min_level/10 != m_min_equips_enhance_lv/10)
		{
			int32 step_level = m_min_equips_enhance_lv - m_min_equips_enhance_lv % 10;
			auto data = MtItemManager::Instance().GetEnhanceMasterConfig(step_level);
			if (data != nullptr)
			{
				for (int32 i = 0; i < data->attr_size(); i++) {
					MtActorConfig::AddAttribute(changes, data->attr(i), data->value(i));
				}
			}
		}
	}
	/*
	1.单件装备取宝石最高等级，记为A1~A12
	2.从A1~A12中去掉最小的n个，剩下12-n个记为B1~B12-n
	3.从B1~B12-n中取最小的一个作为宝石大师等级

	考虑特殊情况
	a:[1,0,0][2,3,2][2,3,4][2,3,4][2,3,4][2,3,4][2,3,4][2,3,4][2,3,4][2,3,4][2,3,0][4,0,0]
	正确结果是3
	*/
	void MtActor::UpdateGemMasterEffect(attribute_set& changes)
	{
		if (db_info_ == nullptr)
			return;

		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}

		int32 old_min_level = db_info_->gem_master_lv();
		int32 min_temp = 999;
		uint32 part_count = 10;
		//计算新等级
		std::vector<MtEquipItem*> curr_equips;
		CurrEquipments(curr_equips);

		if (curr_equips.size() >= part_count) {
			std::map<int32, int32> all_max;
			//部位->宝石分布
			for (auto &child : curr_equips) {
				std::vector<int32> gems;
				child->GatherGemId(gems);
				if (gems.empty()) {
					continue;
				}

				int32 part_max = 0;
				for (auto gem : gems) {
					auto config = MtItemManager::Instance().GetGemConfig(gem);
					if (config != nullptr) {
						//单个槽的最大宝石等级
						if (part_max < config->level()) {
							part_max = config->level();
						}
						all_max[(int32)(child->Slot())] = part_max;
					}
				}
			}
			
			int32 more = (int32)all_max.size() - part_count;
			if (more>=0) {
				for (int32 i = 0; i < more; i++) {	
					min_temp = 999;
					auto iter = all_max.begin();
					auto temp = all_max.begin();
					for (; iter != all_max.end(); ++iter) {
						//取等级最低的宝石等级
						if (min_temp > iter->second) {
							min_temp = iter->second;
							temp = iter;
						}
					}
					all_max.erase(temp);
				}

				//再取一次最小的就是宝石大师等级
				min_temp = 999;
				for (auto child : all_max) {
					if (min_temp > child.second) {
						min_temp = child.second;
					}
				}
			}
			
		}

		db_info_->set_gem_master_lv( min_temp==999?0:min_temp );
		int32 step_level = db_info_->gem_master_lv();
		//计算效果,需要累计计算
		for (int32 k = 1; k <= step_level; k++) {
			auto data = MtItemManager::Instance().GetEnhanceMasterConfig(k);
			if (data != nullptr) {
				for (int32 i = 0; i < data->attr_size(); i++) {
					MtActorConfig::AddAttribute(changes, data->attr(i), data->value(i));
				}
			}
		}

		if (old_min_level != step_level) {
			player->SendCommonReply("UpdateGemMasterLv", { step_level }, { Guid() }, {});
			player->BroadCastCommonReply("BoradCastGemMasterLv", { step_level }, { player->Guid() }, {});
			if (MainActor()) {
				player->GetScenePlayerInfo()->set_master_level(step_level);
			}
		}
	}

	void MtActor::UpdateSkillAttributeEffect(attribute_set& changes)
	{
		for (auto child : equip_skill_set_) {
			RemoveSkill(child);
		}
		equip_skill_set_ = changes.skill_set_;
		for (auto child : equip_skill_set_) {
			AddSkill(child);
		}
	}

	void MtActor::OnCache()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			ZXERO_ASSERT(false);
			return;
		}
		player->Cache2Save(db_info_.get());
		for (auto& skill : skills_) {
			player->Cache2Save(skill->DbInfo());
		}

		simple_impacts_.OnCache();
	}

	void MtActor::SerializationToMessage(Packet::ActorFullInfo& message)
	{
		message.mutable_actor()->CopyFrom(*db_info_);
		std::vector<MtEquipItem*> equips;
		CurrEquipments(equips);
		for (auto& equip : equips) {
			equip->FillEquipInfo(*message.add_equips());
		}
		for (auto& skill : skills_) {
			
			Packet::ActorSkill info;
			message.add_skills()->CopyFrom(*skill->DbInfo());
		}
	}
	void MtActor::SetName(const std::string &name)
	{
		db_info_->set_name(name);
	}
	Packet::Talent MtActor::Talent() const
	{
		return db_info_->talent();
	}

	Packet::Race MtActor::Race() const
	{
		return db_info_->race();
	}

	Packet::Professions MtActor::Professions() const
	{
		return db_info_->profession();
	}

	Packet::ActorType MtActor::ActorType() const
	{
		return db_info_->type();
	}
	
	void MtActor::SetActorType(Packet::ActorType type)
	{
		db_info_->set_type(type);
	}

	Packet::BattleActorType MtActor::BattleActorType() const
	{
		switch (db_info_->type())
		{
		case Packet::ActorType::Main:
			return Packet::BattleActorType::BAT_MAIN;
		case Packet::ActorType::Hero:
			return Packet::BattleActorType::BAT_HERO;
		case Packet::ActorType::Monster:
			return Packet::BattleActorType::BAT_MONSTER;
		case Packet::ActorType::SUMMON_NORMAL:
		case Packet::ActorType::SUMMON_NO_SELECT:
			return Packet::BattleActorType::BAT_SUMMON;
		case Packet::ActorType::Boss:
			return Packet::BattleActorType::BAT_BOSS;
		default:
			return Packet::BattleActorType::BAT_HERO;
		}
	}

	zxero::int32 MtActor::Level() const
	{
		return db_info_->level();
	}

	Packet::ActorSkill* MtActor::FindDBSkill(uint64 guid)
	{
		for (auto child : skills_) {
			if (child->Guid() == guid) {
				return child->DbInfo();
			}
		}
		return nullptr;
	}

	void MtActor::BattleDurableCost(int32 cost /*= 1*/, bool simulate /*= false*/)
{
		std::vector<MtEquipItem*> equips;
		CurrEquipments(equips);
		if (equips.empty()) 
			return;
		//随机一件装备掉耐久
		for (auto i = 0; i < cost; ++i) {
			auto equip = equips[rand_gen->uintgen(0, equips.size() - 1)];

			//装备耐久不掉完不刷新装备属性
			if (!equip->DurableCost(0.1f, simulate)) {
				if (simulate) 
					continue;
				OnBattleInfoChanged();
			}
		}

	}


	void MtActor::DeadDurableCost(int times /*= 1*/, bool simulate /*= false*/)
	{
		std::vector<MtEquipItem*> equips;
		CurrEquipments(equips);
		bool changed = false;
		for (auto i = 0; i < times; ++i) {
			for (auto& equip : equips) {
				auto equip_config = equip->Config();
				if (equip_config != nullptr) {
					auto init_durable = float(equip_config->init_durable());
					auto durable_cost = std::minmax(equip->DbInfo()->durable(), init_durable * 0.01f).first;
					if (equip->DurableCost(durable_cost, simulate)) {
						changed = true;
					}
				}
			}
		}
		if (changed && !simulate)
			OnBattleInfoChanged();
	}

	zxero::int32 MtActor::ConfigId() const
	{
		return db_info_->actor_config_id();
	}

	zxero::int32 MtActor::MaxHp() const
	{
		return db_info_->battle_info().hp() 
			+ std::accumulate(std::begin(buffs_), std::end(buffs_), 0, [](auto accu, auto& impact) {
			return accu + impact->MaxHp();
		});
	}

	real32 MtActor::BeforeCureExecute(real32 cure_hp) const
	{
		return cure_hp;
	}

	uint64 MtActor::MoveTowardTargetGuid() const
	{
		return move_toward_guid_;
	}

	void MtActor::SetMoveTowardTargetGuid(uint64 guid)
	{
		//无法切换目标并且目标已经尚未死亡, 那就不改变目标
		auto target = battle_ground_->FindActor(guid);
		if (target != nullptr && target->Guid() != this->Guid())
			Direction(zxero::get_direction(Position(), target->Position()));

		move_toward_guid_ = guid;
	}

	SkillTarget& MtActor::ActorTargets()
	{
		return targets_;
	}

	bool MtActor::ManualControl() const
	{
		return actor_ai_control_type_ == Packet::ActorControlType::MANUAL;
	}

	void MtActor::SetConrolType(Packet::ActorControlType type)
	{
		if (actor_ai_control_type_ == type)
			return;
		actor_ai_control_type_ = type;
	}

	void MtActor::SetPlayer(const boost::weak_ptr<MtPlayer>& player)
	{
		player_ = player;
	}

	void MtActor::InitEquip()
	{
		auto config = MtActorConfig::Instance().GetBaseConfig(ConfigId());
		for (auto& equpi_id : config->equips) {
			auto econfig = MtItemManager::Instance().GetEquipmentConfig(equpi_id);
			if (econfig != nullptr) {
				AddEquipByConfigId(equpi_id, MtActorConfig::Instance().EquipmentType2Slot(econfig->type()));
			}
		}
	}

	Json::Value& MtActor::Meta()
	{
		return meta_;
	}

	const Json::Value& MtActor::TarExtra() const
	{
		return tar_extra_;
	}

	void MtActor::TarExtra(const Json::Value& extra)
	{
		tar_extra_ = extra;
	}

	std::vector<boost::shared_ptr<Buff>>& MtActor::Buffs()
	{
		return buffs_;
	}

	bool MtActor::AddState(Config::BattleObjectStatus state)
	{
		if (Config::BattleObjectStatus::FEAR == state
			&& !CanBeFear()) {
			return false;
		}
		ability_.AddState(state);
		if (CurrSkill() && CurrSkill()->BaseSkill() == false
			&& !ability_.CanDo(Config::BattleObjectAbility::USE_MAGIC_SKILL))
			CurrSkill()->BeBreak();
		return true;
	}

	bool MtActor::ClearState(Config::BattleObjectStatus state)
	{
		ability_.ClearState(state);
		return true;
	}
	
	bool MtActor::GetState(Config::BattleObjectStatus state)
	{
		return ability_.GetState(state);
	}

	bool MtActor::Male() const
	{
		return db_info_->gender() == Packet::Gender::MALE;
	}

	bool MtActor::Female() const
	{
		return db_info_->gender() == Packet::Gender::FEMAIL;
	}

	bool MtActor::IsAlliance() const
	{
		return db_info_->race() == Packet::Race::Human || db_info_->race() == Packet::Race::NigthElf || db_info_->race() == Packet::Race::Gnome;
	}

	bool MtActor::IsHorde() const
	{
		return db_info_->race() == Packet::Race::Tauren || db_info_->race() == Packet::Race::Undead
			|| db_info_->race() == Packet::Race::Orc || db_info_->race() == Packet::Race::Troll;
	}

	MtActor::~MtActor()
	{

	}

	void MtActor::SummonEnter()
	{
		static Json::Value blind_buff;
		if (blind_buff["life_time"].asInt() == 0) {
			blind_buff["id"] = Json::Value(0);
			blind_buff["name"] = Json::Value("add_status");
			blind_buff["status"] = Json::Value("BLIND");
			blind_buff["life_time"] = Json::Value(3000);
		}
		SkillContext ctx;
		auto buff = BuffImpl::BuffFactory::get_mutable_instance().CreateBuff(
			blind_buff, ctx, battle_ground_, Guid());
		this->AddBuff(buff);
	}

	void MtActor::ClientMoveStart()
	{
		if (battle_ground_ != nullptr) {
			move_serial_ = battle_ground_->GenMoveSerial();
		}
	}

	bool MtActor::ClientMoveFinished()
	{
		return move_serial_ == 0;
	}

	void MtActor::OnMoveFinishMsg(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<Packet::ActorMoveToTargetFinish>& msg)
	{
		player, msg;
		/*
		if (!player->IsDisConnected() && player->Guid() != player->Guid()) {
			return;
		}
		//LOG_INFO << this << "," << this->ConfigId() << ". move serial:" << move_serial_ << " finish";
		if (move_serial_ == msg->move_serial()) {
			move_serial_ = 0;
			Position(msg->pos());
		}*/
	}

	zxero::int32 MtActor::MoveSerial()
	{
		return move_serial_;
	}

	void MtActor::OnExitBattle()
	{
		move_toward_guid_ = INVALID_GUID;
		targets_.Reset();
		runtime_info_.skills_.clear();
		runtime_info_.skill_list_.clear();
		skills_.clear();
		buffs_.clear();
	}

	void MtActor::OnBattleEnd()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return;
		}
		std::for_each(std::begin(buffs_), std::end(buffs_), boost::bind(&Buff::OnDestroy, _1));
		buffs_.clear();
		std::for_each(std::begin(runtime_info_.skills_), std::end(runtime_info_.skills_), [](auto& skill) {
			skill->SetReuse();
		});
		if (IsDead()) DeadDurableCost();

		auto impacts = player->GetSimpleImpactSet();
		if (impacts != nullptr) {
			impacts->OnClearImpact(Packet::SimpleImpactEffectType::EF_PlayerLackeys);
		}
	}

	void MtActor::BattleState(const boost::shared_ptr<BattleState::BaseState>& state)
	{
		state_ = state;
	}

	bool MtActor::CanBeFear() const
	{
		return std::all_of(buffs_.begin(), buffs_.end(),
			boost::bind(&Buff::CanBeFear, _1) == true);
	}

	void MtActor::ClearRunTimeSkill()
	{
		auto event = boost::make_shared<ClearSkillEvent>();
		event->actor_guid_ = Guid();
		this->BattleGround()->PushEvent(event);
	}
	
	void MtActor::DoClearRunTimeSkill()
	{
		boost::for_each(runtime_info_.skills_, boost::bind(&MtActorSkill::OnDestroy, _1));
		runtime_info_.skills_.clear();
	}

	void MtActor::DoAddRuntimeSkill(int32 id, int32 level)
	{
		if (MtSkillManager::Instance().SkillConfig(id) == nullptr){
			ZXERO_ASSERT(false) << "add runtime skill error:" << id;
			return;
		}
		auto skill_info = boost::make_shared<Packet::ActorSkill>();
		skill_info->set_actor_guid(INVALID_GUID);
		skill_info->set_guid(INVALID_GUID);
		skill_info->set_skill_id(id);
		skill_info->set_skill_level(level);
		runtime_info_.skills_.push_back(boost::make_shared<MtActorSkill>(*skill_info, this));
	}

	void MtActor::AddRuntimeSkill(int32 id, int32 level)
	{
		auto event = boost::make_shared<AddSkillEvent>();
		event->actor_guid_ = Guid();
		event->id_and_levels_.push_back({ id, level });
		this->BattleGround()->PushEvent(event);
	}
	

	std::set<uint64> MtActor::SkillCreatedActor(int32 skill_id) const
	{
		if (created_actors_.find(skill_id) == created_actors_.end()) {
			return std::set<uint64>();
		} else {
			return created_actors_.at(skill_id);
		}
	}

	void MtActor::UpdateSkillCreatedActor(int32 skill_id, std::set<uint64>&& set)
	{
		created_actors_[skill_id] = set;
	}

	MtPlayer* MtActor::LuaPlayer() const
	{
		if (player_.expired()) {
			return nullptr;
		} else {
			return player_.lock().get();
		}
	}

	int32 MtActor::SkillScore() const
	{
		auto config = MtActorConfig::Instance().GetBaseConfig(db_info_->actor_config_id());
		auto star_skill_id = config->star_skill;
		int32 sum = 0;
		for (auto& skill : skills_) {
			if (skill->SkillId() == star_skill_id) {
				sum += 40 * skill->SKillLevel();
			} else {
				sum += 4 * skill->SKillLevel();
			}
		}
		if (ActorType() == Packet::ActorType::Main && !player_.expired()) {
			sum += player_.lock()->GuildPracticeLevelSum() *  20;
		}
		return sum;
	}

	bool MtActorAbility::CanDo(Config::BattleObjectAbility type) const 
	{
		int32 bit = 0x1 << (Config::BattleObjectAbility_MAX - int32(type));
		return (ability_ & bit) > 0;
	}

	void MtActorAbility::Clear()
	{
		for (auto i = 0; i < Config::BattleObjectStatus_ARRAYSIZE; ++i) {
			status_[i] = 0;
		}
	}


	void MonsterBattleActor::Transform(Config::MonsterConfig* config)
	{

		back_radius_ = runtime_info_.object_radius;
		back_monster_config_id_ = monster_config_id_;
		monster_config_id_ = config->id();
		back_skills_ = std::move(runtime_info_.skills_);
		runtime_info_.skill_list_.clear();
		runtime_info_.object_radius = int32(config->radius() * 1000);
		for (auto i = 0; i < config->skill_ids_size(); ++i) {
			auto skill_id = config->skill_ids(i);
			auto skill_info = boost::make_shared<Packet::ActorSkill>();
			skill_info->set_actor_guid(INVALID_GUID);
			skill_info->set_guid(INVALID_GUID);
			skill_info->set_skill_id(skill_id);
			skill_info->set_skill_level(1);
			runtime_info_.skills_.push_back(boost::make_shared<MtActorSkill>(*skill_info, this));
		}
		state_ = boost::make_shared<BattleState::Idle>(this, seconds(2).total_milliseconds());
		//bt_agent_->btresetcurrent();
	}


	void MonsterBattleActor::TransformCancel()
	{
		runtime_info_.object_radius = back_radius_;
		monster_config_id_ = back_monster_config_id_;
		runtime_info_.skills_ = std::move(back_skills_);
		std::for_each(runtime_info_.skills_.begin(), runtime_info_.skills_.end(),
			boost::bind(&MtActorSkill::SetReuse, _1));
		runtime_info_.skill_list_.clear();
		back_radius_ = 0;
		state_ = boost::make_shared<BattleState::Idle>(this);
		//bt_agent_->btresetcurrent();
	}

	MonsterBattleActor::MonsterBattleActor(const boost::shared_ptr<Config::MonsterConfig>& config, const int32 level)
	{
		player_.reset();
		exp_drop_ = config->exp_drop();
		gold_drop_ = config->gold_drop();
		life_time_ = config->life_time();
		monster_class_ = config->monster_class();
		db_info_ = boost::make_shared<Packet::ActorBasicInfo>();
		db_info_->set_guid(MtGuid::Instance()(*(MtActor*)this));
		db_info_->set_player_guid(INVALID_GUID);
		db_info_->set_race(Packet::Race::Human);
		db_info_->set_profession(Packet::Professions::Druid);
		db_info_->set_talent(Packet::Talent::Druid_Bear);
		db_info_->set_gender(Packet::Gender::UNKNOWN);
		db_info_->set_level(level);
		
		db_info_->set_type(Packet::ActorType::Monster);
		db_info_->set_exp(0);
		db_info_->set_star(Packet::ActorStar::ONE);
		db_info_->set_color(Packet::ActorColor::WHILE);
		db_info_->set_level_2(0);
		db_info_->set_exp_2(0);
		db_info_->set_name("");
		db_info_->set_battle_type(Packet::ActorBattleType::USE_PHYSICAL);
		db_info_->set_client_res_id(config->client_res_id());
		db_info_->set_actor_config_id(config->id());
		db_info_->set_score(0);		
		db_info_->set_hair(0);
		db_info_->set_gem_master_lv(0);

		//
		if (config->adapt_group() < 0) {
			db_info_->mutable_battle_info()->CopyFrom(config->battle_info());
		}
		else {//走自适应属性模板
			auto attribute = MtMonsterManager::Instance().FindMonsterAttribute(config->adapt_group(), level);
			if (attribute == nullptr) {
				ZXERO_ASSERT(false) << "invalid monster attribute! group ="<<config->adapt_group() << " level =" << level;
			}
			else {
				auto battle_info = db_info_->mutable_battle_info();
				battle_info->CopyFrom(attribute->battle_info());
				//速度，技能取配置表里的值
				battle_info->set_attack_speed(config->battle_info().attack_speed());
				battle_info->set_move_speed(config->battle_info().move_speed());
				battle_info->set_skill(config->battle_info().skill());
			}			
		}

		//Packet::SimpleImpactData impact_data;
		//impact_data.set_left_time(0);
		//impact_data.set_impact_type(Packet::SimpleImpactType::InvalidImpact);
		//impact_data.set_param1(0);
		//impact_data.set_param2(0);
		//for (int32 i = 0; i < 5; i++) {
		//	auto impact = db_info_->add_simple_impact();
		//	impact->CopyFrom(impact_data);
		//}

		runtime_info_.battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());;
		max_battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());

		db_info_->clear_last_hp();
		for (int32 i = 0; i < Packet::LastHpType::LastHpMax; i++) {
			db_info_->add_last_hp(Hp());
		}
		



	/*	behavior_node_ = boost::make_shared<RootNode<MtActor>>("root");
		auto target_in_range_condition = boost::make_shared<ConditonTargetInRange<MtActor>>("check_range");

		auto select_skill_seq = boost::make_shared<SequenceNode<MtActor>>("select_skill_seq");
		select_skill_seq->AddNode(boost::make_shared<ConditonCanUseSkill<MtActor>>("condition can use"));
		select_skill_seq->AddNode(boost::make_shared<ActionSelectSkill<MtActor>>("action select skill"));
		auto select_target_seq = boost::make_shared<ActionSelectMoveTarget<MtActor>>("actoin select target");

		auto move_to_target_seq = boost::make_shared<SequenceNode<MtActor>>("move to target seq");
		move_to_target_seq->AddNode(select_skill_seq);
		move_to_target_seq->AddNode(select_target_seq);
		move_to_target_seq->AddNode(boost::make_shared<ConditionNot<MtActor>>(target_in_range_condition, "condition not"));
		move_to_target_seq->AddNode(boost::make_shared<ActionMoveToTarget<MtActor>>("action move to target"));

		auto use_skill_seq = boost::make_shared<SequenceNode<MtActor>>("use_skill_seq");
		use_skill_seq->AddNode(select_skill_seq);
		//use_skill_seq->AddNode(select_target_seq);
		//use_skill_seq->AddNode(target_in_range_condition);
		use_skill_seq->AddNode(boost::make_shared<ActionUseSkill<MtActor>>("action use skill"));

		auto actor_sel = boost::make_shared<SelectorNode<MtActor>>("actor sel");
		actor_sel->AddNode(boost::make_shared<ActionGlobalCoolDown<MtActor>>("global cooldown"));
		actor_sel->AddNode(move_to_target_seq);
		actor_sel->AddNode(use_skill_seq);
		behavior_node_->AddNode(actor_sel);*/

	}

	void MonsterBattleActor::InitBattleInfo(const boost::shared_ptr<Config::MonsterConfig>& config)
	{
		for (auto i = 0; i < config->skill_ids_size(); ++i) {
			auto skill_id = config->skill_ids(i);
			Packet::ActorSkill skill_info;
			skill_info.set_actor_guid(INVALID_GUID);
			skill_info.set_guid(INVALID_GUID);
			skill_info.set_skill_id(skill_id);
			skill_info.set_skill_level(1);
			runtime_info_.skills_.push_back(boost::make_shared<MtActorSkill>(skill_info, this));
		}
		skill_build_ = MtSkillManager::Instance().GetSkillBuild(config->id(), Packet::ActorColor::WHILE);
		runtime_info_.object_radius = int32(config->radius() * 1000);
		monster_config_id_ = config->id();
		state_ = boost::make_shared<BattleState::Idle>(this);
		ai_strategy_ = config->ai_strategy();
/*
		auto tmp_ai = behaviac::Agent::Create<NormalAiAdaptor>();
		tmp_ai->SetActor(shared_from_this());
		tmp_ai->btsetcurrent(config->ai_stretegy().c_str());
		BTAgent(tmp_ai);*/
	}

	bool MonsterBattleActor::OnTick(zxero::uint64 elapseTime)
	{
		if (life_time_ > 0) {
			life_time_ -= (int32)elapseTime;
			if (life_time_ <= 0) {
				auto delete_event = boost::make_shared<DeleteActorEvent>();
				delete_event->guids_ = { Guid() };
				this->BattleGround()->PushEvent(delete_event);
				LOG_INFO << "actor dismiss later:" << Guid();
				return true;
			}
		}
		if (ai_strategy_ > 0) {
			ActorWrap actor_wrap;
			actor_wrap.actor_ = this;
			try
			{
				thread_lua->call<void>(ai_strategy_, "OnTick", actor_wrap, elapseTime);
			}
			catch (ff::lua_exception& )
			{
			}
			
		}
		return MtActor::OnTick(elapseTime);
	}


	zxero::int32 MonsterBattleActor::MaxHp() const
	{
		return max_battle_info_->hp();
	}


	bool MonsterBattleActor::OnDead(MtActor* killer)
	{
		if (MtActor::OnDead(killer)) {
			BattleGround()->OnMonsterDead(*this, killer);
			return true;
		}
		return false;
	}


	void MonsterBattleActor::SerializationToBattleInfo(Packet::ActorBattleInfo& message) const
	{
		MtActor::SerializationToBattleInfo(message);
		message.set_monster_config_id(monster_config_id_);
	}


	void MonsterBattleActor::AfterHpChange(SkillContext& ctx)
	{
		MtActor::AfterHpChange(ctx);
		if (ai_strategy_ > 0) {//改变hp时, 目前自己都是target
			try
			{
				thread_lua->call<void>(ai_strategy_, "AfterHpChange", ctx.target_);
			}
			catch (ff::lua_exception& e)
			{
				LOG_ERROR << "[MonsterBattleActor] " << e.what();
			}
		}
	}

	/*

	void MonsterBattleActor::OnBattleEnd()
	{
		Packet::ActorDismiss msg;
		msg.set_guid(Guid());
		msg.set_time_stamp(BattleGround()->TimeStamp());
		BattleGround()->BroadCastMessage(msg);
		MtActor::OnBattleEnd();
	}
*/

	ActorBattleCopy::ActorBattleCopy(const boost::shared_ptr<MtActor>& rhs) :MtActor(*rhs), source_(rhs)
	{
		max_battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());
		//战斗中临时生效的属性逻辑
		OnBeginBattleInfo();
	}


	ActorBattleCopy::ActorBattleCopy(const Packet::ActorFullInfo& msg)
	{
		max_battle_info_ = boost::make_shared<Packet::BattleInfo>(msg.actor().battle_info());
		db_info_ = boost::make_shared<Packet::ActorBasicInfo>(msg.actor());
		runtime_info_.battle_info_ = boost::make_shared<Packet::BattleInfo>(db_info_->battle_info());
		full_info_.CopyFrom(msg);
	}

	void ActorBattleCopy::SerializationToBattleInfo(Packet::ActorBattleInfo& message) const
	{
		MtActor::SerializationToBattleInfo(message);
		if (transform_id_ > 0) {
			message.mutable_base_info()->set_type(Packet::ActorType::Monster);
			message.mutable_base_info()->set_actor_config_id(transform_id_);
			message.mutable_base_info()->set_client_res_id(transform_id_);
		}
		message.set_monster_config_id(transform_id_);
	}

	void ActorBattleCopy::OnBeginBattleInfo()
	{
		auto player = player_.lock();
		if (player == nullptr) {
			return;
		}
		attribute_set attributes;
		UpdateLackesImpcatEffect(attributes);
		//统一换算
		Packet::BattleInfo final_changes;
		MtActorConfig::BattleInfoHelper(attributes, final_changes);
		MtActorConfig::Instance().LevelOne2LevelTwo(db_info_->talent(), final_changes);
		MessageAdd(*max_battle_info_, final_changes);
		MtActorConfig::BattleInfoFinalize(*max_battle_info_);
	}


	void ActorBattleCopy::DeadDurableCost(int times /*= 1*/, bool simulate /*= false*/)
	{
		if (source_)
			source_->DeadDurableCost(times, simulate);
	}

	void ActorBattleCopy::BattleDurableCost(int32 cost /*= 1*/, bool simulate /*= false*/)
	{
		if (source_)
			return source_->BattleDurableCost(cost, simulate);
	}

	void ActorBattleCopy::OnBattleRefresh()
	{
		if (source_) {
			InitBattleInfo(*source_);
			MtActor::OnBattleRefresh();
		} else {
			LOG_ERROR << "no source actor, cannot refresh:" << BattleGround()->BattleType();
		}

	}


	zxero::int32 ActorBattleCopy::MaxHp() const
	{
		return max_battle_info_->hp();
	}

	void ActorBattleCopy::Transform(Config::MonsterConfig* config)
	{
		transform_id_ = config->id();
		back_skills_ = std::move(runtime_info_.skills_);
		back_skill_build_ = skill_build_;
		skill_build_ = nullptr;
		runtime_info_.skill_list_.clear();
		for (auto i = 0; i < config->skill_ids_size(); ++i) {
			auto skill_id = config->skill_ids(i);
			Packet::ActorSkill skill_info;
			skill_info.set_actor_guid(INVALID_GUID);
			skill_info.set_guid(INVALID_GUID);
			skill_info.set_skill_id(skill_id);
			skill_info.set_skill_level(1);
			runtime_info_.skills_.push_back(boost::make_shared<MtActorSkill>(skill_info, this));
		}
		back_radius_ = runtime_info_.object_radius;
		runtime_info_.object_radius = int32(config->radius() * 1000);
		state_ = boost::make_shared<BattleState::Idle>(this,seconds(2).total_milliseconds());
		//bt_agent_->btresetcurrent();
	}


	void ActorBattleCopy::TransformCancel()
	{
		runtime_info_.object_radius = back_radius_;
		runtime_info_.skills_ = std::move(back_skills_);
		std::for_each(runtime_info_.skills_.begin(), runtime_info_.skills_.end(),
			boost::bind(&MtActorSkill::SetReuse, _1));
		curr_skill_index = 0;
		skill_build_ = back_skill_build_;
		runtime_info_.skill_list_.clear();
		back_radius_ = 0;
		back_skill_build_ = nullptr;
		transform_id_ = 0;
		state_ = boost::make_shared<BattleState::Idle>(this);
		//bt_agent_->btresetcurrent();
	}


	bool ActorBattleCopy::OnDead(MtActor* killer)
	{
		if (MtActor::OnDead(killer)) {
			BattleGround()->OnActorCopyDead(this, killer);
			return true;
		}
		return false;
	}

	void GlobalSkillDoolDown::OnTick(zxero::uint64 elapseTime)
	{
		if (started_) {
			total_elapse_ += elapseTime;
		}
	}


	void GlobalSkillDoolDown::Start(const MtActor& actor)
	{
		started_ = true;
		auto init_cd = 1000; //默认两秒, 前10级都是2秒, 10级以后每一级减0.01秒
		auto min_cd = 500;// 最小0.5秒
		auto level_fix = (actor.Level() - 10) > 0 ? actor.Level() - 10 : 0;
		auto new_cd = init_cd - level_fix * 10;
		total_cd_time_ = new_cd > min_cd ? new_cd : min_cd; 
		total_elapse_ = 0;
	}


	SummonMonsterActor::SummonMonsterActor(
		const boost::shared_ptr<Config::MonsterConfig>& config, 
		const int32 level, MtActor* creator,
		bool can_be_select)
		:MonsterBattleActor(config, level)
	{
		db_info_->set_type(Packet::ActorType::SUMMON_NO_SELECT);
		if (can_be_select) {
			db_info_->set_type(Packet::ActorType::SUMMON_NORMAL);
		}
		SetPlayer(creator->Player());
	}

	void SummonMonsterActor::InitBattleInfo(const boost::shared_ptr<Config::MonsterConfig>& config)
	{
		MonsterBattleActor::InitBattleInfo(config);
		state_ = boost::make_shared<BattleState::Idle>(this, seconds(3).total_milliseconds());
	}

	void SummonMonsterActor::OnBattleEnd()
	{
		Packet::ActorDismiss msg;
		msg.set_guid(Guid());
		msg.set_time_stamp(BattleGround()->TimeStamp());
		BattleGround()->BroadCastMessage(msg);
		MonsterBattleActor::OnBattleEnd();
	}


	Packet::BattleActorType SummonMonsterActor::BattleActorType() const
	{
		return Packet::BattleActorType::BAT_SUMMON;
	}


	const boost::shared_ptr<ActorBattleCopy> ActorFullInfoDummy::CreateBattleCopy()
	{
		return CreateBattleCopyFromMsg(info_);
	}

}
