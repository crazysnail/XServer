#ifndef ZXERO_GAMESERVER_MT_ACTOR_CONFIG_H__
#define ZXERO_GAMESERVER_MT_ACTOR_CONFIG_H__

#include "../mt_types.h"
#include <vector>
#include <boost/ptr_container/ptr_map.hpp>
#include "ActorBasicInfo.pb.h"

namespace Mt
{
	using namespace zxero;
	struct ActorConfigKey {
		Packet::Professions profession;
		Packet::Talent		talent;
		bool operator<(const ActorConfigKey& rhs) const {
			return (profession < rhs.profession) || (profession == rhs.profession && talent < rhs.talent);
		}
	};
	struct ActorBaseConfig {
		real32 star_fix[Packet::ActorStar_ARRAYSIZE];
		int32 color_fix[Packet::ActorColor_ARRAYSIZE];
		Packet::Gender  gender;
		Packet::BattleInfo property;
		Packet::Race	race;
		Packet::Professions profession;
		Packet::Talent	talent;
		Packet::HeroPosition	hero_position;
		std::vector<int32> skills;
		std::vector<int32> equips;
		int32 green_skill = 0;
		int32 blue_skill = 0;
		int32 purple_skill = 0;
		int32 star_skill = 0;
		int32 client_res_id = 0;
		int32 ground_batch = 0; //前中后排
		int32 ground_priority = 0;
		std::string icon = "";
		std::vector<Config::WeaponType> main_hand_weapons;
		std::vector<Config::WeaponType> assi_hand_weapons;
	};

	struct attribute_set;
	
	class MtActorConfig : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtActorConfig();
		~MtActorConfig() {}
		static MtActorConfig& Instance();
		int32	OnLoad();
		const boost::shared_ptr<ActorBaseConfig> GetBaseConfig(zxero::int32 id) {
			if (configs_.find(id) == std::end(configs_)) {
				return nullptr;
			}
			return configs_[id];
		}
		bool IsStarSkill(int32 actor_config_id, int32 skill_id);
		bool NeedLevelUp(Packet::ActorType type, int32 level, int32 curr_exp);
		void LevelOne2LevelTwo(Packet::Talent talent, Packet::BattleInfo &binfo);
		/**
		* \brief 根据装备类型选择一个可装备的槽位, 由于不需要提供actor,
		*　所以主副手武器都一定返回主手槽位
		*/
		Config::EquipmentSlot EquipmentType2Slot(Config::EquipmentType type);
		static int32 BattleInfo2Score(const Packet::BattleInfo& battle_info);
		static void BattleInfoHelper(const attribute_set& attributes,Packet::BattleInfo &binfo );
		static void BattleInfoFinalize( Packet::BattleInfo &binfo);
		static void BattleInfoToMap(const Packet::BattleInfo &binfo, attribute_set& attributes);
		static void AddAttribute(attribute_set& attributes, std::string name, int32 value);
		int32 ActorMaxLevel() const;
		boost::shared_ptr<Config::LevelConfig> GetLevelData(int32 level);
		const boost::shared_ptr<Config::OneKeyConfig> GetOneKeyConfig(int32 id);
		const boost::shared_ptr<Config::LackeysConfig> GetLackeysConfig(int32 index);
		int32 CalcServerLevel(int32 delta_day);
		std::array<int32, 3> ProfHairs(Packet::Professions prof);
		std::vector<int32> TeamRobotIds(int32 group) const;
		bool OnekeyActor2FullInfo(Packet::ActorFullInfo& full_actor, int32 one_key_id);
		void EquipEffect(const Packet::EquipItem& db_info_,
			attribute_set& changes);
		/**
		* \brief 根据actor_config_id, star, color, level, attributes 计算战斗数据
		*/
		void InitBaseActorBattleInfo(Packet::ActorBasicInfo& basic_info,
			const attribute_set& attributes);
	private:
		void add_team_robot_id(int32 group, int32 id);
		int32 base_property_fix(int32 base, const ActorBaseConfig& config, int32 level, int32 color, int32 star);
		std::map<int32, boost::shared_ptr<ActorBaseConfig>> configs_;
		std::map<int32, boost::shared_ptr<Config::OneKeyConfig>> onekey_configs_;
		std::map<int32, boost::shared_ptr<Config::LevelConfig>> level_configs_; //<等级, 配置>
		std::map<int32, std::array<int32, 3>> hairs_; //头发配置
		std::map<int32, boost::shared_ptr<Config::LackeysConfig>> lackeys_config_;
		int32 actor_max_level_ = 10;
		std::map<int32, std::vector<int32>> team_robot_ids_;
		std::map<Packet::Talent, std::map<std::string, std::map<std::string, real32>>> talent_map_;/* = {
			{
				Packet::Talent::Warrior_Weapon,
					{
						{ "hp" ,
							{
								{ "endurance",10 },
							},
						},
						{ "physical_attack",
							{
								{ "physical_strength" , 2 },
								{ "agile" , 1 },
							},
						},
						{ "physical_armor" ,
							{
								{ "agile" , 1 },
							},
						},
						{ "magic_attack" ,
							{
								{ "intelligence", 1 },
							},
						},
						{ "magic_armor" ,
							{
								{ "spirit", 1 },
							},
						},
					},

			},
			{
				Packet::Talent::Warrior_Rage,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 2 },
						{ "agile" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Warrior_Armor,
				{
					{ "hp" ,
						{
							{ "endurance",10 },
						},
					},
					{ "physical_attack",
						{
							{ "physical_strength" , 2 },
							{ "agile" , 1 },
						},
					},
					{ "physical_armor" ,
						{
							{ "agile" , 1 },
						},
					},
					{ "magic_attack" ,
						{
							{ "intelligence", 1 },
						},
					},
					{ "magic_armor" ,
						{
							{ "spirit", 1 },
						},
					},
				},
				
			},
			{
				Packet::Talent::Shaman_Element,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Shaman_Strenth,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Shaman_Cure,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
						{ "spirit", 2 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Hunter_Beast,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },

					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Hunter_Shoot,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},
			},
			{
				Packet::Talent::Hunter_Suivival,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },

					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Mage_Profound,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Mage_Ice,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Mage_Fire,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Druid_Cat,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },

					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Druid_Bear,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },

					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Druid_Cure,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
						{ "spirit", 2 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Druid_Bird,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
						{ "spirit", 2 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Warlock_Pain,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Warlock_Demon,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Warlock_Ruin,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Priest_Precept,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Priest_Holy,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
						{ "spirit", 2 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Priest_Dark,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 2 },
						{ "spirit", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Paladin_Cure,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
						{ "spirit", 2 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Paladin_Armor,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 2 },
						{ "agile" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Paladin_Precept,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 2 },
						{ "agile" , 1 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Rogue_Assassin,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Rogue_Fighter,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
			{
				Packet::Talent::Rogue_Agile,
				{
					{ "hp" ,
					{
						{ "endurance",10 },
					},
					},
					{ "physical_attack",
					{
						{ "physical_strength" , 1 },
						{ "agile" , 2 },
					},
					},
					{ "physical_armor" ,
					{
						{ "agile" , 1 },
					},
					},
					{ "magic_attack" ,
					{
						{ "intelligence", 1 },
					},
					},
					{ "magic_armor" ,
					{
						{ "spirit", 1 },
					},
					},
				},

			},
		}; */
	}; //class
}
#endif // ZXERO_GAMESERVER_MT_ACTOR_CONFIG_H__
