#include "module.h"
#include "../data/data_table.h"
#include "../data/data_manager.h"
#include "mt_actor_config.h"
#include "ActorConfig.pb.h"
#include "EquipmentConfig.pb.h"
#include "ActorBasicInfo.proto.fflua.h"
#include "../mt_item/mt_item.h"
#include "../mt_server/mt_server.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace Mt
{
	using namespace zxero;
	static MtActorConfig* __mt_actor_config = nullptr;
	REGISTER_MODULE(MtActorConfig)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtActorConfig::OnLoad, boost::ref(MtActorConfig::Instance())));
	}

	MtActorConfig& MtActorConfig::Instance()
	{
		return *__mt_actor_config;
	}

	MtActorConfig::MtActorConfig()
	{
		__mt_actor_config = this;
	}

	void MtActorConfig::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtActorConfig, ctor()>(ls, "MtActorConfig")
			.def(&MtActorConfig::GetLevelData, "GetLevelData")
			.def(&MtActorConfig::EquipmentType2Slot,"EquipmentType2Slot")
			.def(&MtActorConfig::GetOneKeyConfig,"GetOneKeyConfig")
			.def(&MtActorConfig::GetLackeysConfig, "GetLackeysConfig")
			.def(&MtActorConfig::IsStarSkill, "IsStarSkill")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtActorConfig::Instance, "LuaMtActorConfig");
	}

	int32 MtActorConfig::base_property_fix(int32 base, const ActorBaseConfig& config, int32 level, int32 color, int32 star) {
		return (int32)(base + base * config.star_fix[star] * (level - 1) + base * config.color_fix[color]);
	}

	void MtActorConfig::BattleInfoHelper(const attribute_set& attributes, Packet::BattleInfo &binfo )
	{
		auto reflection = binfo.GetReflection();
		auto descriptor = binfo.GetDescriptor();

		for (auto& key_value : attributes.attribute_set_) {
			if(key_value.first == "skill")
				continue;

			if (key_value.first.empty()){
				ZXERO_ASSERT(false) << "invalid attribute, name=null";
				continue;
			}

			auto field = descriptor->FindFieldByName(key_value.first);
			if (field == nullptr)	{
				ZXERO_ASSERT(false) << "invalid attribute, name=" << key_value.first;
				continue;
			}	
			reflection->SetInt32(&binfo, field, key_value.second);
		}
	}

	void MtActorConfig::BattleInfoFinalize(Packet::BattleInfo &binfo)
	{
		//最终化，主要计算%比属性影响
		int32 percent_hp = binfo.percent_hp();
		int32 percent_physical_attack = binfo.percent_physical_attack();
		int32 percent_physical_armor = binfo.percent_physical_armor();
		int32 percent_magic_attack = binfo.percent_magic_attack();
		int32 percent_magic_armor = binfo.percent_magic_armor();

		int32 hp = binfo.hp();
		int32 physical_attack = binfo.physical_attack();
		int32 physical_armor = binfo.physical_armor();
		int32 magic_attack = binfo.magic_attack();
		int32 magic_armor = binfo.magic_armor();

		if(percent_hp>0) binfo.set_hp((int)ceill((float)hp*(1.0 + (float)percent_hp / 1000)));
		if(percent_physical_attack>0) binfo.set_physical_attack((int)ceill((float)physical_attack*(1.0 + (float)percent_physical_attack / 1000)));
		if(percent_physical_armor>0) binfo.set_physical_armor((int)ceill((float)physical_armor*(1.0 + (float)percent_physical_armor / 1000)));
		if(percent_magic_attack>0) binfo.set_magic_attack((int)ceill((float)magic_attack*(1.0 + (float)percent_magic_attack / 1000)));
		if(percent_magic_armor>0) binfo.set_magic_armor((int)ceill((float)magic_armor*(1.0 + (float)percent_magic_armor / 1000)));
	}

	void MtActorConfig::BattleInfoToMap(const Packet::BattleInfo &binfo, attribute_set& attributes)
	{
		auto reflection = binfo.GetReflection();
		auto descriptor = binfo.GetDescriptor();

		for (auto i = 0; i < descriptor->field_count(); ++i) {
			auto field_descriptor = descriptor->field(i);
			int32 value = reflection->GetInt32(binfo, field_descriptor);
			if (value <= 0) continue;
			
			if (field_descriptor->name().empty() || field_descriptor->name() == "skill") {
				ZXERO_ASSERT(false) << "invalid attribute, name=null";
				continue;
			}

			auto iter = attributes.attribute_set_.find(field_descriptor->name());
			if (iter == attributes.attribute_set_.end()) {
				attributes.attribute_set_.insert(std::make_pair(field_descriptor->name(), value));
			}
			else {
				iter->second += reflection->GetInt32(binfo, field_descriptor);
			}
		}

	}

	void MtActorConfig::AddAttribute(attribute_set& attributes, std::string name, int32 value)
	{
		if (name.empty()) return;
		if (value <= 0) return;

		if (name == "skill"){
			attributes.skill_set_.push_back(value);
		}
		else {
			auto iter = attributes.attribute_set_.find(name);
			if (iter == attributes.attribute_set_.end()) {
				attributes.attribute_set_.insert(std::make_pair(name, value));
			}
			else{
				iter->second  += value;
			}			
		}	
	}

	void MtActorConfig::LevelOne2LevelTwo( Packet::Talent talent, Packet::BattleInfo &binfo) {
		auto map = talent_map_[talent];
		auto reflection = binfo.GetReflection();
		auto descriptor = binfo.GetDescriptor();

		for (auto& level_two_key_value : map) {
			for (auto& level_one_key_value : level_two_key_value.second) {
				auto level_one_field = descriptor->FindFieldByName(level_one_key_value.first);
				auto level_two_field = descriptor->FindFieldByName(level_two_key_value.first);
				if (level_one_field && level_two_field) {
					auto old_value = reflection->GetInt32(binfo, level_two_field); //二级属性当前值
					auto level_one_2_level_two = (int32)(reflection->GetInt32(binfo, level_one_field) * level_one_key_value.second); //一级属性转到二级属性
					auto new_value = old_value + level_one_2_level_two; //二级属性新值
					reflection->SetInt32(&binfo, level_two_field, new_value);
				}
			}
		}
	}

	int32 MtActorConfig::OnLoad()
	{
		auto table = data_manager::instance()->get_table("player_actor_base");
		if (table != nullptr) {
			for (auto icon = 0; icon < table->get_rows(); ++icon) {
				auto row = table->find_row(icon);
				auto config = boost::make_shared <ActorBaseConfig>();
				zxero::int32 id = row->getInt("id");
				config->icon = row->getString("icon1");
				ZXERO_ASSERT(Packet::Race_IsValid(row->getInt("race")));
				config->race = (Packet::Race)row->getInt("race");
				ZXERO_ASSERT(Packet::Gender_IsValid(row->getInt("gender")));
				config->gender = (Packet::Gender)row->getInt("gender");
				ZXERO_ASSERT(Packet::Professions_IsValid(row->getInt("profession")));
				config->profession = (Packet::Professions)row->getInt("profession");
				ZXERO_ASSERT(Packet::Talent_IsValid(row->getInt("talent")));
				config->talent = (Packet::Talent)row->getInt("talent");
				ZXERO_ASSERT(Packet::HeroPosition_IsValid(row->getInt("hero_position")));
				config->hero_position = (Packet::HeroPosition)row->getInt("hero_position");
				config->ground_batch = row->getInt("ground_batch");
				config->ground_priority = row->getInt("ground_priority");
				config->client_res_id = row->getInt("client_res_id");
				config->property.set_endurance(row->getInt("endurance"));
				config->property.set_physical_strength(row->getInt("physical_strength"));
				config->property.set_agile(row->getInt("agile"));
				config->property.set_intelligence(row->getInt("intelligence"));
				config->property.set_spirit(row->getInt("spirit"));
				config->property.set_hp(row->getInt("hp"));
				config->property.set_physical_attack(row->getInt("physical_attack"));
				config->property.set_physical_armor(row->getInt("physical_armor"));
				config->property.set_magic_attack(row->getInt("magic_attack"));
				config->property.set_magic_armor(row->getInt("magic_armor"));
				config->property.set_accurate(row->getInt("accurate"));
				config->property.set_dodge(row->getInt("dodge"));
				config->property.set_critical(row->getInt("critical"));
				config->property.set_toughness(row->getInt("toughness"));
				config->property.set_move_speed(row->getInt("move_speed"));
				config->property.set_attack_speed(row->getInt("attack_speed"));
				config->property.set_skill(row->getInt("skill"));

				//optional
				config->property.set_percent_hp(0);
				config->property.set_percent_physical_attack(0);
				config->property.set_percent_physical_armor(0);
				config->property.set_percent_magic_attack(0);
				config->property.set_percent_magic_armor(0);

				config->star_fix[Packet::ActorStar::ONE] = row->getFloat("star_1_fix");
				config->star_fix[Packet::ActorStar::TWO] = row->getFloat("star_2_fix");
				config->star_fix[Packet::ActorStar::THREE] = row->getFloat("star_3_fix");
				config->star_fix[Packet::ActorStar::FOUR] = row->getFloat("star_4_fix");
				config->star_fix[Packet::ActorStar::FIVE] = row->getFloat("star_5_fix");
				config->color_fix[Packet::ActorColor::WHILE] = 0;
				config->color_fix[Packet::ActorColor::GREEN_0] = row->getInt("color_fix")*Packet::ActorColor::WHILE;
				config->color_fix[Packet::ActorColor::GREEN_1] = row->getInt("color_fix")*Packet::ActorColor::GREEN_0;
				config->color_fix[Packet::ActorColor::BLUE_0] = row->getInt("color_fix")*Packet::ActorColor::GREEN_1;
				config->color_fix[Packet::ActorColor::BLUE_1] = row->getInt("color_fix")*Packet::ActorColor::BLUE_0;
				config->color_fix[Packet::ActorColor::BLUE_2] = row->getInt("color_fix")*Packet::ActorColor::BLUE_1;
				config->color_fix[Packet::ActorColor::PURPLE_0] = row->getInt("color_fix")*Packet::ActorColor::BLUE_2;
				for (auto j = 1; j <= 3; ++j) {
					auto key = (boost::format("init_skill%1%") % j).str();
					auto skill_id = row->getInt(key.c_str());
					if (skill_id > 0)
						config->skills.push_back(skill_id);
				}
				for (auto j = 1; j <= 7; ++j) {
					auto key = (boost::format("init_equip%1%") % j).str();
					auto equip_id = row->getInt(key.c_str());
					if (equip_id > 0) config->equips.push_back(equip_id);
				}
				config->green_skill = row->getInt("green_skill");
				config->blue_skill = row->getInt("blue_skill");
				config->purple_skill = row->getInt("purple_skill");
				config->star_skill = row->getInt("star_skill");
				std::vector<std::string> main_hand_weapon_types;
				std::string main_hand_weapons(row->getString("main_hand_weapons"));
				boost::split(main_hand_weapon_types,main_hand_weapons, boost::is_any_of("|"));
				std::transform(std::begin(main_hand_weapon_types), std::end(main_hand_weapon_types), 
					std::inserter(config->main_hand_weapons, std::begin(config->main_hand_weapons)),
					[](std::string& t) {return Config::WeaponType(boost::lexical_cast<int>(t)); }
				);
				std::vector<std::string> assi_hand_weapon_types;
				std::string assi_hand_weapons(row->getString("assi_hand_weapons"));
				boost::split(assi_hand_weapon_types, assi_hand_weapons,  boost::is_any_of("|"));
				std::transform(std::begin(assi_hand_weapon_types), std::end(assi_hand_weapon_types),
					std::inserter(config->assi_hand_weapons, std::begin(config->assi_hand_weapons)),
					[](std::string& t) {return Config::WeaponType(boost::lexical_cast<int>(t)); }
				);
				configs_.insert({ id, config });
			}
		}
		else {
			ZXERO_ASSERT(false);
		}
		table = data_manager::instance()->get_table("create_player");
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			int32 prof = row->getInt("profession");
			int32 hair_0 = row->getInt("hair0");
			int32 hair_1 = row->getInt("hair1");
			int32 hair_2 = row->getInt("hair2");
			hairs_.insert({ prof, { hair_0, hair_1,hair_2 } });
		}

		table = data_manager::instance()->get_table("exp");
		ZXERO_ASSERT(table) << "can not find table exp";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared <Config::LevelConfig>();
			FillMessageByRow(*config, *row);
			level_configs_.insert({ config->level(), config });
			actor_max_level_ = config->level();
		}

		table = data_manager::instance()->get_table("onekey");
		ZXERO_ASSERT(table) << "can not find table onekey";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared <Config::OneKeyConfig>();
			FillMessageByRow(*config, *row);
			onekey_configs_.insert({ config->id(), config });
		}
		table = data_manager::instance()->get_table("team_robot");
		ZXERO_ASSERT(table) << "can not find table team_robot.csv";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto group = row->getInt("group");
			auto id = row->getInt("one_key_id");
			ZXERO_ASSERT(onekey_configs_.find(id) != onekey_configs_.end());
			add_team_robot_id(group, id);
		}
		//
		table = data_manager::instance()->get_table("lackeys");
		ZXERO_ASSERT(table) << "can not find table lackeys";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared <Config::LackeysConfig>();
			FillMessageByRow(*config, *row);
			lackeys_config_.insert({ config->index(), config });
		}

		//		

		table = data_manager::instance()->get_table("property_transform");
		ZXERO_ASSERT(table != nullptr);
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto _talnet = row->getInt("talnet");
			ZXERO_ASSERT(Packet::Talent_IsValid(_talnet));
			auto talent = (Packet::Talent)_talnet;
			auto level1 = std::string(row->getString("level1_name"));
			auto level2 = std::string(row->getString("level2_name"));
			auto level2_value = row->getFloat("level2_value");
			if (talent_map_.find(talent) == std::end(talent_map_)) {
				talent_map_[talent] = { {level2, {{level1, level2_value}}} };
			}
			else {
				auto& curr_map = talent_map_[talent];
				if (curr_map.find(level2) == std::end(curr_map)) {
					curr_map[level2] = { {level1, level2_value} };
				}
				else {
					auto& level2_map = curr_map[level2];
					level2_map[level1] = level2_value;
				}
			}
		}
		return 0;
	}

	bool MtActorConfig::NeedLevelUp(Packet::ActorType type, int32 level, int32 curr_exp)
	{
		if (level >= actor_max_level_ || level_configs_.find(level) == std::end(level_configs_)) {
			return false;
		}
		auto& curr_level_config = level_configs_[level];
		switch (type)
		{
		case Packet::Main:
			if (curr_level_config->main_actor_exp() <= curr_exp)
				return true;
			break;
		case Packet::Hero:
			if (curr_level_config->hero_actor_exp() <= curr_exp)
				return true;
			break;
		default:
			ZXERO_ASSERT(false);
			break;
		}
		return false;
	}

	boost::shared_ptr<Config::LevelConfig> MtActorConfig::GetLevelData(int32 level)
	{
		if (level > actor_max_level_ || level_configs_.find(level) == std::end(level_configs_)) {
			return nullptr;
		}
		return level_configs_[level];
	}

	const boost::shared_ptr<Config::OneKeyConfig> MtActorConfig::GetOneKeyConfig(int32 id)
	{
		auto iter = onekey_configs_.find(id);
		if (iter != onekey_configs_.end()) {
			return iter->second;
		}

		return nullptr;
	}

	const boost::shared_ptr<Config::LackeysConfig> MtActorConfig::GetLackeysConfig(int32 index)
	{
		auto iter = lackeys_config_.find(index);
		if (iter != lackeys_config_.end()) {
			return iter->second;
		}

		return nullptr;
	}

	int32 MtActorConfig::CalcServerLevel(int32 delta_day)
	{
		int32 level = 1;
		if (delta_day < 0) {
			delta_day = 0;
		}
		for (auto child : level_configs_)
		{
			if (child.second->server_level_day() <= delta_day) {
				level = child.first;
			}
			else {
				break;
			}
		}
		return level;
	}

	zxero::int32 MtActorConfig::ActorMaxLevel() const
	{
		return actor_max_level_;
	}

	zxero::int32 MtActorConfig::BattleInfo2Score(const Packet::BattleInfo& battle_info)
	{
		return battle_info.hp() / 5
			+ battle_info.physical_attack() + battle_info.magic_attack()
			+ battle_info.physical_armor() + battle_info.magic_armor()
			+ battle_info.accurate() * 2 + battle_info.critical() * 2
			+ battle_info.dodge() * 2 + battle_info.toughness() * 2;
	}

	Config::EquipmentSlot MtActorConfig::EquipmentType2Slot(Config::EquipmentType type)
	{
		Config::EquipmentSlot ret = Config::EquipmentSlot::EQUIPMENT_SLOT_INVALID;
		switch (type)
		{
		case Config::EquipmentType::EQUIPMENT_TYPE_INVALID:
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_MAIN_HAND:
		case Config::EquipmentType::EQUIPMENT_TYPE_ASSI_HAND:
		case Config::EquipmentType::EQUIPMENT_TYPE_BOTH_HAND:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_MAIN_HAND;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_HELMET:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_HEAD;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_SHOULDER:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_SHOULDER;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_CHEST:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_CHEST;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_MAIN_CUFF:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_CUFF;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_SHOES:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_SHOES;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_CLOAK:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_CLOAK;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_NECKLACE:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_NECK;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_RING:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_RING_ONE;
			break;
		case Config::EquipmentType::EQUIPMENT_TYPE_ORNAMENT:
			ret = Config::EquipmentSlot::EQUIPMENT_SLOT_ORNAMENT;
			break;
		default:
			break;
		}
		return ret;
	}

	std::array<int32, 3> MtActorConfig::ProfHairs(Packet::Professions prof)
	{
		if (hairs_.find(int32(prof)) == hairs_.end()) {
			LOG_INFO << "can not find prof:" << prof;
			return{ -1 };
		} else {
			return hairs_.at(int32(prof));
		}
	}

	bool MtActorConfig::IsStarSkill(int32 actor_config_id, int32 skill_id)
	{
		auto config = GetBaseConfig(actor_config_id);
		if (config == nullptr) {
			return false;
		}
		return config->star_skill == skill_id;
	}

	bool MtActorConfig::OnekeyActor2FullInfo(Packet::ActorFullInfo& full_actor, int32 one_key_id)
	{
		auto one_key_config = GetOneKeyConfig(one_key_id);
		if (one_key_config == nullptr) 
			return false;
		uint64 actor_guid = full_actor.actor().guid();
		int32 level = one_key_config->level();
		Packet::ActorColor color = Packet::ActorColor(one_key_config->color());
		Packet::ActorStar star = Packet::ActorStar(one_key_config->star());
		auto actor_config = MtActorConfig::Instance().GetBaseConfig(one_key_config->hero_id());
		full_actor.SetInitialized();
		Packet::ActorBasicInfo& actor_info = *full_actor.mutable_actor();
		//init basic info
		actor_info.set_guid(actor_guid);
		actor_info.set_player_guid(actor_guid);
		actor_info.set_race(actor_config->race);
		actor_info.set_name(one_key_config->display_name());
		actor_info.set_hair(one_key_config->hair());
		actor_info.set_profession(actor_config->profession);
		actor_info.set_talent(actor_config->talent);
		actor_info.set_level(level);
		actor_info.set_type(Packet::ActorType::Main);
		actor_info.set_color(Packet::ActorColor(one_key_config->color()));
		actor_info.set_star(Packet::ActorStar(one_key_config->star()));
		actor_info.set_client_res_id(one_key_config->hero_id());
		actor_info.set_actor_config_id(one_key_config->hero_id());
		actor_info.mutable_battle_info();
/*		actor_info.set_score(MtActorConfig::BattleInfo2Score(actor_info.battle_info()));*/
		//init skill
		if (actor_config) {
			for (auto skill_id : actor_config->skills) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(skill_id);
				skill->set_skill_level(level);
			}
			if (star >= Packet::ActorStar::THREE && actor_config->star_skill > 0) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(actor_config->star_skill);
				skill->set_skill_level(int32(star - Packet::ActorStar::TWO));
			}
			if (color >= Packet::ActorColor::GREEN_0 && actor_config->green_skill > 0) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(actor_config->green_skill);
				skill->set_skill_level(level);
			}
			if (color >= Packet::ActorColor::BLUE_0 && actor_config->blue_skill > 0) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(actor_config->blue_skill);
				skill->set_skill_level(level);
			}
			if (color >= Packet::ActorColor::PURPLE_0 && actor_config->purple_skill > 0) {
				auto skill = full_actor.add_skills();
				skill->set_actor_guid(actor_guid);
				skill->set_guid(0);
				skill->set_skill_id(actor_config->purple_skill);
				skill->set_skill_level(level);
			}
		}
		//init equip
		attribute_set attributes;
		for (auto i = 0; i < one_key_config->equips_size(); ++i) {
			auto& one_key_equip_config = one_key_config->equips(i);
			auto equip_config = MtItemManager::Instance().GetEquipmentConfig(one_key_equip_config.id());
			if (equip_config == nullptr) {
				continue;;
			}
			auto equip = full_actor.add_equips();
			equip->SetInitialized();
			equip->set_actor_guid(actor_guid);
			equip->set_config_id(one_key_equip_config.id());
			Config::EquipmentSlot slot = Config::EquipmentSlot::EQUIPMENT_SLOT_INVALID;
			switch (i) {
			case 0:
				slot = Config::EquipmentSlot::EQUIPMENT_SLOT_MAIN_HAND;
				break;
			case 1:
				slot = Config::EquipmentSlot::EQUIPMENT_SLOT_ASSI_HAND;
				break;
			case 10:
				slot = Config::EquipmentSlot::EQUIPMENT_SLOT_RING_ONE;
				break;
			case 11:
				slot = Config::EquipmentSlot::EQUIPMENT_SLOT_RING_TWO;
				break;
			default:
				slot = EquipmentType2Slot(equip_config->type());
				break;
			}
			equip->set_slot(slot);
			equip->set_durable(100);
			equip->set_enhence_level(1);
			equip->add_gems(0); equip->add_gems(0); equip->add_gems(0);
			if (one_key_equip_config.gem0() > 0)
				equip->set_gems(0, one_key_equip_config.gem0());
			if (one_key_equip_config.gem1() > 0)
				equip->set_gems(1, one_key_equip_config.gem1());
			if (one_key_equip_config.gem2() > 0)
				equip->set_gems(2, one_key_equip_config.gem2());
			EquipEffect(*equip, attributes);
		}
		InitBaseActorBattleInfo(actor_info, attributes);
		return true;
	}

	void MtActorConfig::InitBaseActorBattleInfo(
		Packet::ActorBasicInfo& basic_info, const attribute_set& attributes)
	{
		auto config = MtActorConfig::Instance().GetBaseConfig(basic_info.actor_config_id());
		if (config == nullptr)
			return;

		auto prop_base = config->property;
		auto prop_base_origin = config->property;
		auto level_one_prop_base_color = prop_base;
		auto level_one_prop_base_star = prop_base;
		MessageMulti(level_one_prop_base_color, (zxero::real32)(config->color_fix[(int32)(basic_info.color())])); //卡片颜色
		MessageMulti(level_one_prop_base_star, (basic_info.level() - 1) * config->star_fix[basic_info.star()]);
		MessageAdd(prop_base, level_one_prop_base_color);
		MessageAdd(prop_base, level_one_prop_base_star);
		auto final_info = basic_info.mutable_battle_info();
		final_info->CopyFrom(prop_base);

		//移动速度, 攻击速度不变
		Packet::BattleInfo critical_and_dodge;
		critical_and_dodge.set_accurate(prop_base_origin.accurate());
		critical_and_dodge.set_critical(prop_base_origin.critical());
		critical_and_dodge.set_dodge(prop_base_origin.dodge());
		critical_and_dodge.set_toughness(prop_base_origin.toughness());
		MessageAdd(critical_and_dodge, (basic_info.level() - 1) * 6.f);
		critical_and_dodge.set_move_speed(prop_base_origin.move_speed());
		critical_and_dodge.set_attack_speed(prop_base_origin.attack_speed());
		final_info->MergeFrom(critical_and_dodge);
		//统一换算
		Packet::BattleInfo final_changes;
		MtActorConfig::BattleInfoHelper(attributes, final_changes);
		MessageAdd(*basic_info.mutable_battle_info(), final_changes);
		MtActorConfig::Instance().LevelOne2LevelTwo(basic_info.talent(), *basic_info.mutable_battle_info());
		MtActorConfig::BattleInfoFinalize(*basic_info.mutable_battle_info());
	}

	void MtActorConfig::EquipEffect(const Packet::EquipItem& equip_info, attribute_set& changes)
	{
		//基础属性
		auto base_effect = MtItemManager::Instance().GetEquipmentConfig(equip_info.config_id());
		MtActorConfig::BattleInfoToMap(base_effect->property_effect(), changes);

		//强化
		int32 level = equip_info.enhence_level();
		int32 strength_value = (int32)ceill(base_effect->strength_value()*level);
		MtActorConfig::AddAttribute(changes, base_effect->strength_type(), strength_value);

		//附魔
		for (auto child : equip_info.enchants())
		{
			MtActorConfig::AddAttribute(changes, child.effect_type(), child.effect_value());
		}

		//宝石
		for (auto gem_id : equip_info.gems())
		{
			auto gem_config = MtItemManager::Instance().GetGemConfig(gem_id);
			if (gem_config != nullptr) {
				std::string attr = gem_config->property();
				int32 value = gem_config->value();
				MtActorConfig::AddAttribute(changes, attr, value);
			}
		}
	}

	std::vector<int32> MtActorConfig::TeamRobotIds(int32 group) const
	{
		if (team_robot_ids_.find(group) != team_robot_ids_.end()) {
			return team_robot_ids_.at(group);
		}
		return{};
	}

	void MtActorConfig::add_team_robot_id(int32 group, int32 id)
	{
		if (team_robot_ids_.find(group) == team_robot_ids_.end()) {
			team_robot_ids_.insert({group, { id }});
		} else {
			team_robot_ids_[group].push_back(id);
		}
	}

}
