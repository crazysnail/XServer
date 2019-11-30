#ifndef MTONLINE_GAMESERVER_MT_UPGRADE_H__
#define MTONLINE_GAMESERVER_MT_UPGRADE_H__
#include "../mt_types.h"
#include <tuple>
#include <map>
#include <vector>
#include <boost/ptr_container/ptr_map.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
	struct UpgradeColorKey
	{
		Packet::Professions prof;
		Packet::ActorColor color;
		bool operator<(const UpgradeColorKey& rhs) const {
			return (prof < rhs.prof) || (prof == rhs.prof && color < rhs.color);
		}
	};
	struct UpgradeColorValue
	{
		int32 item_id[3];
		int32 item_count[3];
		int32 quest_id_;
		int32 level_;
	};

	struct UpgradeStarKey
	{
		int32 actor_config_id;
		Packet::ActorStar star;
		bool operator<(const UpgradeStarKey& rhs) const {
			return (actor_config_id < rhs.actor_config_id) || (actor_config_id == rhs.actor_config_id && star < rhs.star);
		}
	};
	bool operator<(const std::tuple<int32, Config::EquipmentColor>& lhs, const std::tuple<int32, Config::EquipmentColor>& rhs);
	struct UpgradeStarValue {
		int32 star_item_id; //星魂
		int32 star_item_amount; //数量
		int32 soul_item_id; //魂魄
		int32 soul_item_amount; //魂魄数量
		int32 need_gold; //金币消耗
	};
	class MtUpgradeConfigManager : public boost::noncopyable
	{
	public:
		MtUpgradeConfigManager();
		static MtUpgradeConfigManager& Instance();
		int32 OnLoad();
		boost::shared_ptr<UpgradeStarValue> UpgradeStarRequirement(int32 actor_id, Packet::ActorStar star);
		boost::shared_ptr<UpgradeColorValue> UpgradeColorRequirement(Packet::Professions prof, Packet::ActorColor color);
		int32 UpgradeSkillRequirement(int32 target_level) const {
			ZXERO_ASSERT(skill_upgrade_gold_require_.size() >= (std::size_t(target_level - 1)));
			auto ret_pair = skill_upgrade_gold_require_.at(target_level - 1);
			ZXERO_ASSERT(ret_pair.first == target_level);
			return ret_pair.second;
		}
		const std::vector<std::pair<int32, int32>>& ExpItem() const
		{
			return level_configs_;
		}
		int32 ItemExp(int32 item_id);
		bool IsExpItem(int32 item_id) const;
		int32 MaxEnhenceLevel() const;
		int32 EquipEnhenceCost(int32 equip_level);
		int32 EquipFixCost(const MtEquipItem& equip);
		const Config::EquipEnchantCost* EquipEnchantRequirement(int32 equip_level) const;
		int32 EquipEnchantLoadItemId() const { return 20000001; }
		std::tuple<std::string, int32> RandomEnchantEffect(const std::vector<std::string>& exclude);
		const Config::EquipDismantle* DismantleData(Config::EquipmentConfig* config) const;
		const Config::FragmentConfig* FragmentConfig(const int32 config_id) const;
		int32 MaxEnchantValue(const std::string& type);
	private:
		std::map<UpgradeColorKey, boost::shared_ptr<UpgradeColorValue>> color_configs_; //< key: <职业, 颜色> , value : <道具id, 道具数量><任务id, 领取等级>..
		std::map<UpgradeStarKey, boost::shared_ptr<UpgradeStarValue>> star_configs_;
		std::vector<std::pair<int32, int32>> level_configs_; //<道具id, 对应经验>
		boost::ptr_map<int32, Config::EquipFixCost> equip_fix_configs_;
		boost::ptr_map<int32, Config::EquipEnchantCost> equip_enchant_configs_;
		boost::ptr_map<int32, Config::EquipEnhenceCost> equip_enhence_configs_;
		std::vector<std::tuple<std::string, int32>> enchant_effects_;
		boost::ptr_map<std::tuple<int32, Config::EquipmentColor>, Config::EquipDismantle> dismantle_configs_;
		boost::ptr_map<int32, Config::FragmentConfig> hero_fragment_configs_;
		std::vector<std::pair<int32,int32>> skill_upgrade_gold_require_;
	};
}
#endif // MTONLINE_GAMESERVER_MT_UPGRADE_H__

