#include "mt_target_select.h"
#include "../mt_actor/mt_actor.h"
#include "../mt_battle/mt_battle_ground.h"
#include "json/json.h"
#include "mt_skill.h"
#include <SkillConfig.pb.h>

namespace Mt
{
	

	/*bool MtSkillSelectTarStrategyQuick::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto temp = result;
		if (skill->SkillConfig()->target_camp() == Config::TargetCamp::ENEMY) {
			temp = self->BattleGround()->Enemies(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else if (skill->SkillConfig()->target_camp() == Config::TargetCamp::TEAM) {
			temp = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else {
			ZXERO_ASSERT(false);
			return false;
		}
		for (auto i = 0; i < skill->SkillConfig()->target_count() && (std::size_t)i < temp.size(); ++i) {
			result.push_back(temp.at(i));
		}
		return true;
	}

	bool MtSkillSelectTarStrategyRange::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto temp = result;
		if (skill->SkillConfig()->target_camp() == Config::TargetCamp::ENEMY) {
			temp = self->BattleGround()->Enemies(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else if (skill->SkillConfig()->target_camp() == Config::TargetCamp::TEAM) {
			temp = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else {
			ZXERO_ASSERT(false);
			return false;
		}
		std::sort(std::begin(temp), std::end(temp), [&](auto& lhs, auto& rhs) {
			return zxero::distance2d(self->Position(), lhs->Position()) < zxero::distance2d(self->Position(), rhs->Position());
		});
		for (auto i = 0; i < skill->SkillConfig()->target_count() && (std::size_t)i < temp.size(); ++i) {
			result.push_back(temp.at(i));
		}
		return true;
	}


	bool MtSkillSelectTarStrategyStableTargetCount::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
		const boost::shared_ptr<MtActor> self,
		std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto temp = result;
		if (skill->SkillConfig()->target_camp() == Config::TargetCamp::ENEMY) {
			temp = self->BattleGround()->Enemies(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else if (skill->SkillConfig()->target_camp() == Config::TargetCamp::TEAM) {
			temp = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else {
			ZXERO_ASSERT(false);
			return false;
		}
		std::sort(std::begin(temp), std::end(temp), [&](auto& lhs, auto& rhs) {
			return zxero::distance2d(self->Position(), lhs->Position()) < zxero::distance2d(self->Position(), rhs->Position());
		});
		std::size_t temp_index = 0;
		for (auto i = 0; i < skill->SkillConfig()->target_count() && (std::size_t)i < temp.size(); ++i) {
			temp_index %= temp.size();
			result.push_back(temp.at(temp_index));
		}
		return true;
	}

	bool MtSkillSelectTarStrategyConfusion::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		self->BattleGround()->EnumAllActor([&](auto& actor) {
			if (result.size() < (std::size_t)skill->SkillConfig()->target_count())
				result.push_back(actor);
		});
		return true;
	}

	bool MtSkillSelectTarStrategyRandom::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, 
		const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto temp = result;
		if (skill->SkillConfig()->target_camp() == Config::TargetCamp::ENEMY) {
			temp = self->BattleGround()->Enemies(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else if (skill->SkillConfig()->target_camp() == Config::TargetCamp::TEAM) {
			temp = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else {
			ZXERO_ASSERT(false);
			return false;
		}
		std::random_shuffle(std::begin(temp), std::end(temp));
		for (auto i = 0; i < skill->SkillConfig()->target_count() && (std::size_t)i < temp.size(); ++i) {
			result.push_back(temp.at(i));
		}
		return true;
	}

	bool MtSkillSelectTarStrategyHatred::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto temp = result;
		if (skill->SkillConfig()->target_camp() == Config::TargetCamp::ENEMY) {
			temp = self->BattleGround()->Enemies(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else if (skill->SkillConfig()->target_camp() == Config::TargetCamp::TEAM) {
			temp = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else {
			ZXERO_ASSERT(false);
			return false;
		}
		for (auto i = 0; i < skill->SkillConfig()->target_count() && (std::size_t)i < temp.size(); ++i) {
			result.push_back(temp.at(i));
		}
		return true;
	}

	bool MtSkillSelectTarStrategyLowHp::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill, const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto temp = result;
		if (skill->SkillConfig()->target_camp() == Config::TargetCamp::ENEMY) {
			temp = self->BattleGround()->Enemies(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else if (skill->SkillConfig()->target_camp() == Config::TargetCamp::TEAM) {
			temp = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		}
		else {
			ZXERO_ASSERT(false);
			return false;
		}
		std::sort(std::begin(temp), std::end(temp),
			[](auto& lhs, auto& rhs) {
				auto lhs_reduce_hp = lhs->MaxHp() - lhs->Hp();
				auto lhs_reduce_hp_percent = 0.f;
				if (lhs_reduce_hp > 0) {
					lhs_reduce_hp_percent = (real32)lhs_reduce_hp / lhs->MaxHp();
				}
				auto rhs_reduce_hp = rhs->MaxHp() - rhs->Hp();
				auto rhs_reduce_hp_percent = 0.f;
				if (rhs_reduce_hp > 0) {
					rhs_reduce_hp_percent = (real32)rhs_reduce_hp / rhs->MaxHp();
				}
				return lhs_reduce_hp_percent > rhs_reduce_hp_percent;
		});
		for (auto i = 0; i < skill->SkillConfig()->target_count() && (std::size_t)i < temp.size(); ++i) {
			result.push_back(temp.at(i));
		}
		return true;
	}


	bool MtSkillSelectTarStrategySpurting::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
		const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto team_mates = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		auto temp = team_mates;
		temp.clear();
		auto spurting_range = skill->SkillConfig()->effect_param_0();
		auto target_count = boost::lexical_cast<std::size_t>(skill->SkillConfig()->effect_param_1());
		std::copy_if(std::begin(team_mates), std::end(team_mates), std::back_inserter(temp), [&](auto& actor) {
			return zxero::distance2d(self->Position(), actor->Position()) <= spurting_range
				&& actor->Guid() != self->Guid();
		});
		std::sort(std::begin(temp), std::end(temp), [&](auto& lhs, auto& rhs) {
			return zxero::distance2d(self->Position(), lhs->Position()) < zxero::distance2d(self->Position(), rhs->Position());
		});
		auto copy_count = temp.size() > target_count ? target_count : temp.size();
		std::copy_n(std::begin(temp), copy_count, std::back_inserter(result));
		return true;
	}

	bool MtSkillSelectTarStrategyFlashChain::SelectTarget(const boost::shared_ptr<MtActorSkill>& skill,
		const boost::shared_ptr<MtActor> self, std::vector<boost::shared_ptr<MtActor>>& result) const
	{
		auto team_mates = self->BattleGround()->TeamMates(self, boost::bind(&MtActor::IsDead, _1) == false);
		auto temp = team_mates;
		temp.clear();
		std::copy_if(std::begin(team_mates), std::end(team_mates), std::back_inserter(temp), [&](auto& actor) {
			return actor->Guid() != self->Guid();
		});
		std::sort(std::begin(temp), std::end(temp), [&](auto& lhs, auto& rhs) {
			return zxero::distance2d(self->Position(), lhs->Position()) < zxero::distance2d(self->Position(), rhs->Position());
		});
		uint32 target_count = std::count_if(std::begin(skill->SkillConfig()->effect_param_str()), std::end(skill->SkillConfig()->effect_param_str()), [](auto& str) {
			return boost::lexical_cast<int32>(str) > 0;
		}) - 1;//有一个已经作为初始目标被选中了
		auto copy_count = temp.size() > target_count ? target_count : temp.size();
		std::copy_n(std::begin(temp), copy_count, std::back_inserter(result));
		return true;
	}


	const boost::shared_ptr<MtSkillSelectTarStrategy>& MtSkillSelectTarStrategyFactory::CreateStrategy(const Config::TargetSortType type)
	{
		if (strategys_.find(type) != std::end(strategys_)) {
			return strategys_[type];
		}
		else {
			strategys_[type] = boost::shared_ptr<MtSkillSelectTarStrategy>(factories_[type]());
			return strategys_[type];
		}
	}

	MtSkillSelectTarStrategyFactory::MtSkillSelectTarStrategyFactory()
	{
		factories_[Config::TargetSortType::Quick] = boost::factory<MtSkillSelectTarStrategyQuick*>();
		factories_[Config::TargetSortType::Random] = boost::factory<MtSkillSelectTarStrategyRandom*>();
		factories_[Config::TargetSortType::Confusion] = boost::factory<MtSkillSelectTarStrategyConfusion*>();
		factories_[Config::TargetSortType::LowHp] = boost::factory<MtSkillSelectTarStrategyLowHp*>();
		factories_[Config::TargetSortType::Range] = boost::factory<MtSkillSelectTarStrategyRange*>();
		factories_[Config::TargetSortType::Hatred] = boost::factory<MtSkillSelectTarStrategyHatred*>();
		factories_[Config::TargetSortType::StableTargetCount] = boost::factory<MtSkillSelectTarStrategyStableTargetCount*>();
		factories_[Config::TargetSortType::Spurting] = boost::factory<MtSkillSelectTarStrategySpurting*>();
		factories_[Config::TargetSortType::FlashChain] = boost::factory<MtSkillSelectTarStrategyFlashChain*>();
	}

	const boost::shared_ptr<MtSkillSelectTarStrategy>& MtSkillSelectTarStrategyFactory::CreateStrategyBySkillType(const MtActorSkill& skill)
	{
		return CreateStrategy(skill.SkillConfig()->select_tar_logic());
	}*/




/*
auto MtSkillSelectTar::SelectForMove(const MtActor& owner,
	const std::vector<boost::shared_ptr<MtActor>>& teammates,
	const ::std::vector<boost::shared_ptr<MtActor>>& enemies, 
	Json::Value& skill_config) -> std::remove_reference<decltype(teammates)>::type::value_type
{
	Config::TargetCamp camp = Config::TargetCamp::ENEMY;
	auto& tar_logic = skill_config["tar_logic"];
	if (tar_logic["teammates"] == true)
		camp = Config::TargetCamp::TEAM;
	if (tar_logic["both"] == true)
		camp = Config::TargetCamp::BOTH;
	auto camp_result = CampSelect(teammates, enemies, camp);
	camp_result.erase(std::remove_if(std::begin(camp_result), std::end(camp_result), boost::bind(&MtActor::IsDead, _1)), std::end(camp_result));
	if (camp_result.size() > 0) {
		SortTarget(owner, camp_result, select_param.sort_type());
		return camp_result.front();
	}
	else {
		return nullptr;
	}
}


auto MtSkillSelectTar::SelectForSkill(const MtActor& owner, 
	const std::vector<boost::shared_ptr<MtActor>>& teammates,
	const ::std::vector<boost::shared_ptr<MtActor>>& enemies,
	Json::Value& skill_config) -> std::remove_reference<decltype(teammates)>::type
{
	std::remove_const<std::remove_reference<decltype(teammates)>::type>::type result;
	//第一步 根据阵营缩小范围
	auto camp_result = CampSelect(teammates, enemies, select_param.target_camp());
	if (camp_result.size() > 0) {
		//第二步 根据规则排序
		//HACK 选活的, 后面再处理死亡类型
		camp_result.erase(std::remove_if(std::begin(camp_result), std::end(camp_result), boost::bind(&MtActor::IsDead, _1)), std::end(camp_result));
		SortTarget(owner, camp_result, select_param.sort_type());
		//第三步 用一个范围去筛选第二步结果
		auto area_result = AreaSelect(owner, camp_result, select_param);
		if (area_result.empty()) {
			//空了也不能崩啊！
			//ZXERO_ASSERT((int32)result.size() >= select_param.min_count()) << "should greater than min count";
			return result;
		}
		//第四步 限制最大数量
		decltype(result) max_count_result;
		for (auto i = 0; i < (int32)area_result.size() && i < select_param.max_count(); ++i) {
			max_count_result.push_back(area_result[i]);
		}
		//第五步 限制最小数量
		if (max_count_result.size() < (std::size_t)select_param.min_count()) {
			//目标数量不够, 补齐
			auto max_count_index = 0;
			for (auto i = 0; i < select_param.min_count(); ++i, ++max_count_index) {
				max_count_index %= max_count_result.size();
				result.push_back(max_count_result[max_count_index]);
			}
		}
		else {
			result.assign(std::begin(max_count_result), std::end(max_count_result));
		}

		ZXERO_ASSERT((int32)result.size() >= select_param.min_count()) << "should greater than min count";

		//第六步 溅射,闪电链等附加目标. 目前只有伤害技能有附加目标
		if (extra_param) {
			AddExtraTarget(result, enemies, extra_param);
		}
		return result;
	}
	else {
		ZXERO_ASSERT(false) << "should select one target in any case";
		return result;
	}
}


void MtSkillSelectTar::AddExtraTarget(std::vector<boost::shared_ptr<MtActor>>& targets, const std::vector<boost::shared_ptr<MtActor>>& actors, const Config::TargetSelectExtra* param)
{
	std::remove_reference<decltype(targets)>::type added;
	std::remove_reference<decltype(targets)>::type selectable_actors;
	std::copy_if(std::begin(actors), std::end(actors), std::back_inserter(selectable_actors), [&](auto& ele) {
		return std::none_of(std::begin(targets), std::end(targets), [&](auto& ele2) {return ele2->Guid() == ele->Guid(); });
	});
	for (auto target : targets) {
		std::copy_if(std::begin(selectable_actors), std::end(selectable_actors), std::back_inserter(added),
			[&](auto& ele) {
			return zxero::distance2d(ele->Position(), target->Position()) <= param->range();
		});
	}
	auto count = (int32)added.size() > param->max_count() ? param->max_count() : added.size();
	targets.insert(std::end(targets), std::begin(added), std::begin(added) + count);
}


void MtSkillSelectTar::SortTarget(const MtActor& owner, std::vector<boost::shared_ptr<MtActor>>& actors, Config::TargetSortType type)
{
	switch (type)
	{
	case Config::INVALID:
		actors.clear();
		break;
	case Config::RANGE:
		std::sort(std::begin(actors), std::end(actors), [&](auto& lhs, auto& rhs) {
			auto lhs_distance = zxero::distance2d(lhs->Position(), owner.Position());
			auto rhs_distance = zxero::distance2d(rhs->Position(), owner.Position());
			return lhs_distance < rhs_distance;
		});
		break;
	case Config::REVERSER_RANGE:
		std::sort(std::begin(actors), std::end(actors), [&](auto& lhs, auto& rhs) {
			auto lhs_distance = zxero::distance2d(lhs->Position(), owner.Position());
			auto rhs_distance = zxero::distance2d(rhs->Position(), owner.Position());
			return lhs_distance > rhs_distance;
		});
		break;
	case Config::RANDOM_SEL:
		std::random_shuffle(std::begin(actors), std::end(actors));
		break;
	case Config::HATRED:
		ZXERO_ASSERT(false) << "TODO";
		break;
	case Config::LOW_HP_PERCENT:
		std::sort(std::begin(actors), std::end(actors), [](auto& lhs, auto& rhs) {
			auto lhs_hp_percent = lhs->Hp() / lhs->MaxHp();
			auto rhs_hp_percent = rhs->Hp() / rhs->MaxHp();
			return lhs_hp_percent < rhs_hp_percent;
		});
		break;
	case Config::LOW_HP:
		std::sort(std::begin(actors), std::end(actors), boost::bind(&MtActor::Hp, _1) < boost::bind(&MtActor::Hp, _2));
		break;
	case Config::USE_SKILL_TAR_LOGIC:
		break;
	default:
		break;
	}
}


auto MtSkillSelectTar::AreaSelect(const MtActor& owner, const std::vector<boost::shared_ptr<MtActor>>& actors, const Config::TargetSelectLogic& select_param) -> std::remove_reference<decltype(actors)>::type
{
	std::remove_const<std::remove_reference<decltype(actors)>::type>::type result;
	switch (select_param.area_type())
	{
	case Config::AreaType::Circle:
		std::copy_if(std::begin(actors), std::end(actors), std::back_inserter(result), [&](auto& ele) {
			auto distance = zxero::distance2d(ele->Position(), owner.Position());
			return distance <= (select_param.range() + owner.ObjectRadius() + ele->ObjectRadius());
		});
		break;
	case Config::AreaType::Sector:
		std::copy_if(std::begin(actors), std::end(actors), std::back_inserter(result), [&](auto& ele) {
			auto distance = zxero::distance2d(ele->Position(), owner.Position());
			auto dir = zxero::get_direction(owner.Position(), ele->Position());
			auto left_dir = owner.Direction() - select_param.param_1() / 2;
			auto right_dir = owner.Direction() + select_param.param_1() / 2;
			return distance <= (select_param.range() + owner.ObjectRadius() + ele->ObjectRadius()) && left_dir < dir && dir < right_dir;
		});
		break;
	default:
		ZXERO_ASSERT(false);
		break;
	}
	return result;
}
*/

auto MtSkillSelectTar::CampSelect(
	const std::vector<MtActor*>& teammates,
	const std::vector<MtActor*>& enemies,
	Config::TargetCamp camp) -> std::remove_reference<decltype(teammates)>::type
{
	std::remove_const<std::remove_reference<decltype(teammates)>::type>::type result;
	switch (camp)
	{
	case Config::TEAM:
		result.assign(std::begin(teammates), std::end(teammates));
		break;
	case Config::ENEMY:
		result.assign(std::begin(enemies), std::end(enemies));
		break;
	case Config::BOTH:
		result.reserve(teammates.size() + enemies.size());
		result.insert(std::end(result), std::begin(teammates), std::end(teammates));
		result.insert(std::end(result), std::begin(enemies), std::end(enemies));
		break;
	default:
		ZXERO_ASSERT(false);
		break;
	}
	return result;
}


}