#ifndef MTONLINE_GAMESERVER_MT_MONSTER_MANAGER_H__
#define MTONLINE_GAMESERVER_MT_MONSTER_MANAGER_H__
#include "../mt_types.h"
#include <map>
#include <atomic>

namespace Mt
{
	class MtNpc;

	class MtMonsterManager : public boost::noncopyable {
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtMonsterManager& Instance();
		MtMonsterManager();
		~MtMonsterManager();
		zxero::int32	OnLoad();
		const boost::shared_ptr<Config::MonsterGroup> FindMonsterGroup(int32 id) const;
		int32 MonsterCount(int32 group_id) const;
		const std::vector<boost::shared_ptr<MtItemPackage>> MonsterGroupRandomDrop(int32 monster_group_id) const;
		const boost::shared_ptr<Config::MonsterConfig> FindMonster(const int32 monster_id) const;
		const boost::shared_ptr<Packet::SceneLayout> FindScenePos(const int32 pos_id) const;
		const boost::shared_ptr<Config::MonsterAttribute> FindMonsterAttribute(const int32 group_id, const int32 level);
		const boost::shared_ptr<Packet::NpcStaticConfig> FindStaticNpc(const int32 index);
		const boost::shared_ptr<Packet::NpcDynamicConfig> FindDynamicNpc(const int32 index);
		const boost::shared_ptr<Packet::MountConfig> FindMount(const int32 index);
		boost::shared_ptr <Packet::NpcDynamicConfig> FindNpcInGroup(const int32 npc_group, const int32 npc_id);
		std::vector<boost::shared_ptr< Packet::NpcDynamicConfig >>* FindNpcGroup(const int32 npc_group);

		std::vector<boost::shared_ptr< Packet::PositionGroupConfig >>* FindPosGroup(const int32 pos_group);
		std::vector<Packet::NpcDynamicConfig*> GenRandomNpc(const int32 npc_group, int32 n);
		std::vector<Packet::PositionGroupConfig*> GenRandomPos(const int32 pos_group, int32 n);
		
		Packet::SceneLayout * FindStaticNpcPosInfo(const int32 index);

		boost::shared_ptr<MtNpc> CreateNpc(const int32 pos_id, const int32 index, const int32 relate_mission_id_=-1,const int32 lifetime = -1);
		const boost::shared_ptr<Config::MonsterConfig> GetFirstMosterByGroupId(int32 monstergroupid);
		int32 CheckSkill();

		//
		//
		void LoadMonsterGroupTB(bool reload = false);
		void LoadMonsterConfigTB(bool reload = false);
		void LoadSceneLayoutTB(bool reload = false);
	private:
		std::map<int32, const boost::shared_ptr<Config::MonsterGroup>> monster_group_map_;
		std::map<int32, const boost::shared_ptr<Config::MonsterConfig>> monster_map_;

		std::map<int32, const boost::shared_ptr<Packet::SceneLayout>> scenepos_map_;

		std::map<int32, const boost::shared_ptr<Packet::NpcStaticConfig>> npc_static_;
		std::map<int32, const boost::shared_ptr<Packet::NpcDynamicConfig>> npc_dynamic_;

		std::map<int32, std::vector<boost::shared_ptr<Packet::NpcDynamicConfig>>> npc_group_;
		std::map<int32, std::vector<boost::shared_ptr<Packet::PositionGroupConfig>>> postion_group_;
		std::map<int32, const boost::shared_ptr<Packet::MountConfig>> mount_map_;

		std::map<int32, std::vector<boost::shared_ptr<Config::MonsterAttribute>>> monster_attribute_group_;

		atomic_int max_series_id_;
	};
}

#endif // MTONLINE_GAMESERVER_MT_MONSTER_MANAGER_H__
