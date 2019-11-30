#include "mt_upgrade.h"
#include "module.h"
#include <EquipmentConfig.pb.h>
#include <ItemAndEquip.pb.h>
#include <HeroFragmentConfig.pb.h>
#include "../data/data_table.h"
#include "../data/data_manager.h"
#include <boost/format.hpp>
#include "random_generator.h"
#include "../mt_item/mt_item.h"

namespace Mt
{
	static MtUpgradeConfigManager* __mt_upgrade_config_manager = nullptr;
	REGISTER_MODULE(MtUpgradeConfigManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtUpgradeConfigManager::OnLoad, __mt_upgrade_config_manager));
	}
	MtUpgradeConfigManager::MtUpgradeConfigManager()
	{
		__mt_upgrade_config_manager = this;
	}

	MtUpgradeConfigManager& MtUpgradeConfigManager::Instance()
	{
		return *__mt_upgrade_config_manager;
	}

	zxero::int32 MtUpgradeConfigManager::OnLoad()
	{
		
		auto level_table = data_manager::instance()->get_table("upgrade_level");
		if (level_table) {
			for (auto i = 0; i < level_table->get_rows(); ++i) {
				auto row = level_table->find_row(i);
				for (auto j = 1; j <= 3; ++j) {
					auto item_id_key = (boost::format("item_%1%_id") % j).str();
					auto item_exp_key = (boost::format("item_%1%_exp") % j).str();
					level_configs_.push_back({ row->getInt(item_id_key.c_str()),row->getInt(item_exp_key.c_str()) });
				}
			}
		}
		auto skill_require_table = data_manager::instance()->get_table("skill_cost");
		if (skill_require_table) {
			for (auto i = 0; i < skill_require_table->get_rows(); ++i) {
				auto row = skill_require_table->find_row(i);
				auto level = row->getInt("level");
				auto gold_need = row->getInt("level_up_gold");
				skill_upgrade_gold_require_.push_back({ level, gold_need });
			}
		}
		auto equip_enhence_cost = data_manager::instance()->get_table("equip_enhence_cost");
		if (equip_enhence_cost) {
			for (auto i = 0; i < equip_enhence_cost->get_rows(); ++i) {
				auto row = equip_enhence_cost->find_row(i);
				auto level = row->getInt("level");
				Config::EquipEnhenceCost config;
				FillMessageByRow(config, *row);
				equip_enhence_configs_[level] = config;
			}
		}
		auto equip_enchant_cost = data_manager::instance()->get_table("equip_enchant_cost");
		if (equip_enchant_cost) {
			for (auto i = 0; i < equip_enchant_cost->get_rows(); ++i) {
				auto row = equip_enchant_cost->find_row(i);
				auto level = row->getInt("level");
				Config::EquipEnchantCost config;
				FillMessageByRow(config, *row);
				equip_enchant_configs_[level] = config;
			}
		}

		auto equip_fix_cost = data_manager::instance()->get_table("equip_fix_cost");
		if (equip_fix_cost) {
			for (auto i = 0; i < equip_fix_cost->get_rows(); ++i) {
				auto row = equip_fix_cost->find_row(i);
				auto level = row->getInt("level");
				Config::EquipFixCost config;
				FillMessageByRow(config, *row);
				equip_fix_configs_[level] = config;
			}
		}

		auto equip_enchant_result = data_manager::instance()->get_table("equip_enchant");
		if (equip_enchant_result) {
			for (auto i = 0; i < equip_enchant_result->get_rows(); ++i) {
				auto row = equip_enchant_result->find_row(i);
				auto property_name = row->getString("property");
				auto max_value = row->getInt("max_value");
				enchant_effects_.push_back(std::make_tuple(property_name, max_value));
			}
		}

		auto equip_dismantle = data_manager::instance()->get_table("equip_dismantle");
		if (equip_dismantle) {
			for (auto i = 0; i < equip_dismantle->get_rows(); ++i) {
				auto row = equip_dismantle->find_row(i);
				Config::EquipDismantle config;
				FillMessageByRow(config, *row);
				for (auto j = 1; j <= 3; ++j) {
					auto item_id_key = (boost::format("item_id_%1%") % j).str();
					auto item_amount_key = (boost::format("item_amount_%1%") % j).str();
					auto item_id = row->getInt(item_id_key.c_str());
					auto item_amount = row->getInt(item_amount_key.c_str());
					if (item_id > 0) {
						auto item_info = config.add_dismantle_items();
						item_info->set_item_id(item_id);
						item_info->set_item_amount(item_amount);
					}
				}
				dismantle_configs_[std::make_tuple(config.level(), config.color())] = config;
			}
		}

		auto hero_fragment = data_manager::instance()->get_table("hero_fragment");
		if (hero_fragment) {
			for (auto i = 0; i < hero_fragment->get_rows(); ++i) {
				auto row = hero_fragment->find_row(i);
				Config::FragmentConfig config;
				FillMessageByRow(config, *row);
				hero_fragment_configs_[config.fragment_id()] = config;
			}
		}


		auto color_table = data_manager::instance()->get_table("upgrade_color");
		if (color_table)
		{
			for (auto i = 0; i < color_table->get_rows(); ++i) {
				auto row = color_table->find_row(i);
				UpgradeColorKey key;
				key.color = (Packet::ActorColor)row->getInt("color");
				key.prof = (Packet::Professions)row->getInt("prof");

				auto value = boost::make_shared<UpgradeColorValue>();
				for (auto j = 0; j < 3; ++j) {
					auto item_id_key = (boost::format("item_id_%1%") % (j+1)).str();
					auto item_amount_key = (boost::format("item_amount_%1%") % (j+1)).str();
					value->item_id[j] = row->getInt(item_id_key.c_str());
					value->item_count[j] = row->getInt(item_amount_key.c_str());
				}
				value->level_ = row->getInt("level");
				value->quest_id_ = row->getInt("quest_id");

				color_configs_.insert({ key,value });
			}
		}
		auto star_table = data_manager::instance()->get_table("upgrade_star");
		if (star_table) {
			for (auto i = 0; i < star_table->get_rows(); ++i) {
				auto row = star_table->find_row(i);
				UpgradeStarKey key;			
				key.actor_config_id = row->getInt("actor_config_id");
				key.star = (Packet::ActorStar)row->getInt("star");

				auto value = boost::make_shared<UpgradeStarValue>();

				value->star_item_id = row->getInt("star_item_id");
				value->star_item_amount = row->getInt("star_item_amount");
				value->soul_item_id = row->getInt("soul_item_id");
				value->soul_item_amount = row->getInt("soul_item_amount");
				value->need_gold = row->getInt("need_gold");

				star_configs_.insert({ key,value });
			}
		}

		return 0;
	}


	boost::shared_ptr<UpgradeStarValue> MtUpgradeConfigManager::UpgradeStarRequirement(int32 actor_id, Packet::ActorStar star)
	{
		if (star_configs_.find({ actor_id, star }) == std::end(star_configs_)) {
			return nullptr;
		} else {
			UpgradeStarKey key{ actor_id, star };
			return star_configs_[key];
		}
	}

	boost::shared_ptr<UpgradeColorValue> MtUpgradeConfigManager::UpgradeColorRequirement(Packet::Professions prof, Packet::ActorColor color)
	{
		if (color_configs_.find({ prof, color }) == std::end(color_configs_)) {
			return nullptr;
	}
		else {
			UpgradeColorKey key{ prof, color };
			return color_configs_[key];
		}
	}

	const Config::EquipDismantle* MtUpgradeConfigManager::DismantleData(Config::EquipmentConfig* config) const
	{
		if (config == nullptr)
			return nullptr;

		auto it = dismantle_configs_.find(std::make_tuple(config->level_limit(), config->color()));
		if (it == std::end(dismantle_configs_)) {
			return nullptr;
		}
		else {
			return &dismantle_configs_.at(std::make_tuple(config->level_limit(), config->color()));
		}
	}

	zxero::int32 MtUpgradeConfigManager::ItemExp(int32 item_id)
	{
		for (auto& p : level_configs_) {
			if (p.first == item_id) {
				return p.second;
			}
		}
		return 0;
	}

	bool MtUpgradeConfigManager::IsExpItem(int32 item_id) const
	{
		return std::any_of(std::begin(level_configs_), 
			std::end(level_configs_),
			[=](auto& p) { return p.first == item_id; });
	}

	zxero::int32 MtUpgradeConfigManager::MaxEnhenceLevel() const
	{
		return (int32)equip_enhence_configs_.size();
	}

	zxero::int32 MtUpgradeConfigManager::EquipEnhenceCost(int32 equip_level)
	{
		if (equip_level < 0 || equip_level > 59) {
			return -1;
		}
		return equip_enhence_configs_[equip_level].enhence_gold();
	}

	zxero::int32 MtUpgradeConfigManager::EquipFixCost(const MtEquipItem& equip)
	{
		if (equip.Config()->level_limit() < 0)
			return 0;

		int32 equip_durable = int32(equip.DbInfo()->durable());
		switch (equip.Config()->color())
		{
		case Config::EquipmentColor::EQUIP_WHITE:
			return equip_fix_configs_[equip.Config()->level_limit()].white_fix_gold() * (equip.Config()->init_durable() - equip_durable);
		case Config::EquipmentColor::EQUIP_GREEN:
			return equip_fix_configs_[equip.Config()->level_limit()].green_fix_gold() * (equip.Config()->init_durable() - equip_durable);
		case Config::EquipmentColor::EQUIP_BLUE:
			return equip_fix_configs_[equip.Config()->level_limit()].blue_fix_gold()* (equip.Config()->init_durable() - equip_durable);
		case Config::EquipmentColor::EQUIP_PURPLE:
			return equip_fix_configs_[equip.Config()->level_limit()].purple_fix_gold()* (equip.Config()->init_durable() - equip_durable);
		case Config::EquipmentColor::EQUIP_ORANGE:
			return equip_fix_configs_[equip.Config()->level_limit()].orange_fix_gold()* (equip.Config()->init_durable() - equip_durable);

		default:
			break;
		}
		ZXERO_ASSERT(false);
		return 0;
	}

	const Config::EquipEnchantCost* MtUpgradeConfigManager::EquipEnchantRequirement(int32 equip_level) const
	{
		auto it = equip_enchant_configs_.find(equip_level);
		if ( it == std::end(equip_enchant_configs_)) {
			return nullptr;
		}
		else {
			return &equip_enchant_configs_.at(equip_level);
		}
	}

	std::tuple<std::string, int32> MtUpgradeConfigManager::RandomEnchantEffect(
		const std::vector<std::string>& exclude)
	{
		if (exclude.size() >= enchant_effects_.size()) {
			LOG_ERROR << "exclude list size should less than pool size";
			return enchant_effects_[0];
		} else {
			int32 index = 0;
			std::string type;
			int32 max_try = 32;
			bool conflict = false;
			do {
				index = rand_gen->intgen(0, enchant_effects_.size() - 1);
				type = std::get<0>(enchant_effects_[index]);
				conflict = std::find(exclude.begin(), exclude.end(), type) != exclude.end();
			} while (conflict && max_try-- > 0);
			return enchant_effects_[index];
		}
	}

	const Config::FragmentConfig* MtUpgradeConfigManager::FragmentConfig(const int32 config_id) const
	{
		auto it = hero_fragment_configs_.find(config_id);
		if (it != std::end(hero_fragment_configs_)) {
			return &hero_fragment_configs_.at(config_id);
		}
		else {
			return nullptr;
		}
	}

	bool operator<(const std::tuple<int32, Config::EquipmentColor>& lhs, const std::tuple<int32, Config::EquipmentColor>& rhs)
	{
		return (std::get<0>(lhs) < std::get<0>(rhs)) || (std::get<0>(lhs) == std::get<0>(rhs) && std::get<1>(lhs) < std::get<1>(rhs));
	}


}

