#include "MonsterConfig.pb.h"
#include "MissionConfig.pb.h"
#include "SceneObject.pb.h"
#include "mt_monster_manager.h"
#include "mt_npc.h"
#include "../data/data_manager.h"
#include "../mt_item/mt_item_package.h"
#include "../mt_skill/mt_skill.h"
#include "module.h"
#include "../mt_guid/mt_guid.h"
#include <boost/format.hpp>


namespace Mt
{
	static MtMonsterManager* __mt_monster_manager = nullptr;

	REGISTER_MODULE(MtMonsterManager) 
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtMonsterManager::OnLoad, boost::ref(MtMonsterManager::Instance())));
		register_load_function(module_base::STAGE_POST_LOAD, boost::bind(&MtMonsterManager::CheckSkill, __mt_monster_manager));
	}

	void MtMonsterManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtMonsterManager, ctor()>(ls, "MtMonsterManager")
			.def(&MtMonsterManager::FindNpcInGroup, "FindNpcInGroup")
			.def(&MtMonsterManager::FindStaticNpcPosInfo, "FindStaticNpcPosInfo")
			.def(&MtMonsterManager::FindMonsterGroup, "FindMonsterGroup")
			.def(&MtMonsterManager::FindMonster, "FindMonster")
			.def(&MtMonsterManager::GenRandomNpc, "GenRandomNpc")
			.def(&MtMonsterManager::GenRandomPos, "GenRandomPos")
			.def(&MtMonsterManager::FindScenePos, "FindScenePos")
			.def(&MtMonsterManager::FindDynamicNpc, "FindDynamicNpc")
			.def(&MtMonsterManager::FindStaticNpc, "FindStaticNpc")
			.def(&MtMonsterManager::FindMount, "FindMount")
			.def(&MtMonsterManager::FindPosGroup, "FindPosGroup")
			.def(&MtMonsterManager::LoadMonsterGroupTB,"LoadMonsterGroupTB")
			.def(&MtMonsterManager::LoadMonsterConfigTB, "LoadMonsterConfigTB")
			.def(&MtMonsterManager::LoadSceneLayoutTB, "LoadSceneLayoutTB")
			.def(&MtMonsterManager::GetFirstMosterByGroupId,"GetFirstMosterByGroupId");

		ff::fflua_register_t<>(ls)
			.def(&MtMonsterManager::Instance, "LuaMtMonsterManager");
	}

	int32 MtMonsterManager::CheckSkill()
	{
		std::set<int32> need_skill_id;
		for (auto& pair : monster_map_) {
			const boost::shared_ptr<Config::MonsterConfig>& config = pair.second;
			for (auto id : config->skill_ids()) {
				if (MtSkillManager::Instance().SkillConfig(id) == nullptr) {
					need_skill_id.insert(id);
				}
			}
		}
		for (auto& id : need_skill_id)
			LOG_WARN << "skill: " << id << " missing";
		return 0;
	}

	zxero::int32 MtMonsterManager::OnLoad()
	{
		//load monster group
		LoadMonsterGroupTB();

		//load monster attribute 
		auto table = data_manager::instance()->get_table("monster_attribute");
		ZXERO_ASSERT(table) << "can not find table monster_attribute";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto monster_attribute = boost::make_shared <Config::MonsterAttribute>();
			FillMessageByRow(*monster_attribute, *row);
			if (monster_attribute_group_.find(monster_attribute->group()) == monster_attribute_group_.end()) {		
				std::vector<boost::shared_ptr<Config::MonsterAttribute>> attributes;
				attributes.push_back(monster_attribute);
				monster_attribute_group_.insert({ monster_attribute->group(), attributes });

				if ((int32)monster_attribute_group_[monster_attribute->group()].size() != monster_attribute->level())
				{//æ≤Ã¨ºÏ≤È!
					ZXERO_ASSERT(false) << "invalid monster attribute data group =" << monster_attribute->group() << " level =" << monster_attribute->level();
					return 1;
				}

			}
			else {
				monster_attribute_group_[monster_attribute->group()].push_back(monster_attribute);
			}			
		}


		//load monster
		LoadMonsterConfigTB();

		//load layout
		LoadSceneLayoutTB();

		//load npc
		table = data_manager::instance()->get_table("npc_static");
		ZXERO_ASSERT(table) << "can not find table npc";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared<Packet::NpcStaticConfig>();
			FillMessageByRow(*config, *row);			

			auto pos_config = FindScenePos(config->pos_id());
			ZXERO_ASSERT(pos_config != nullptr) << "can not find pos_config_ data, npc_index="<< config->index();

			auto monster_config = FindMonster(config->npc_id());			
			ZXERO_ASSERT(monster_config != nullptr) << "can not find npc, npc_index="<< config->index();

			if (npc_static_.find(config->index()) != npc_static_.end())
			{
				ZXERO_ASSERT(false) << "repeated npc id in npc_static! npc_index=" << config->index();
			}
			npc_static_.insert({ config->index(), config });
		}

		//load npc group
		table = data_manager::instance()->get_table("npc_dynamic");
		ZXERO_ASSERT(table) << "can not find table npc_group";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared<Packet::NpcDynamicConfig>();
			FillMessageByRow(*config, *row);
			int32 group_id = config->group_id();
			auto  iter = npc_group_.find(group_id);
			if (iter != npc_group_.end())
			{
				iter->second.push_back(config);
			}
			else
			{
				std::vector<boost::shared_ptr<Packet::NpcDynamicConfig>> groups;
				groups.push_back(config);
				npc_group_.insert({ group_id, groups });
			}

			//
			if (npc_dynamic_.find(config->index()) != npc_dynamic_.end()) {
				ZXERO_ASSERT(false) << "repeated index!";
			}
			npc_dynamic_.insert({ config->index(), config });
		}

		//load position group
		table = data_manager::instance()->get_table("position_group");
		ZXERO_ASSERT(table) << "can not find table position_group";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared<Packet::PositionGroupConfig>();
			FillMessageByRow(*config, *row);
			int32 group_id = config->group_id();
			auto  iter = postion_group_.find(group_id);
			if (iter != postion_group_.end())
			{
				iter->second.push_back(config);
			}
			else
			{
				std::vector<boost::shared_ptr<Packet::PositionGroupConfig>> groups;
				groups.push_back(config);
				postion_group_.insert({ group_id, groups });
			}
		}

		//load mount
		table = data_manager::instance()->get_table("mount");
		ZXERO_ASSERT(table) << "can not find table mount";
		for (auto i = 0; i < table->get_rows(); ++i) {
			auto row = table->find_row(i);
			auto config = boost::make_shared<Packet::MountConfig>();
			FillMessageByRow(*config, *row);
			if (mount_map_.find(config->id()) != mount_map_.end()) {
				ZXERO_ASSERT(false) << "repeated id in the mount table, id=" << config->id();
				continue;
			}
			mount_map_.insert({ config->id(), config });
		}
		return 0;
	}

	MtMonsterManager& MtMonsterManager::Instance()
	{
		return *__mt_monster_manager;
	}

	MtMonsterManager::MtMonsterManager()
	{
		max_series_id_ = -1;
		__mt_monster_manager = this;
	}

	void MtMonsterManager::LoadMonsterGroupTB(bool reload)
	{
		monster_group_map_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("monster_group");
		}
		else {
			table = data_manager::instance()->get_table("monster_group");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::MonsterGroup>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			monster_group_map_.insert({ config->id(), config });
		}
		
	}
	void MtMonsterManager::LoadMonsterConfigTB(bool reload)
	{
		monster_map_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("npc_and_monster");
		}
		else {
			table = data_manager::instance()->get_table("npc_and_monster");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::MonsterConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			monster_map_.insert({ config->id(), config });
		}
	}
	void MtMonsterManager::LoadSceneLayoutTB(bool reload)
	{
		scenepos_map_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("scenelayout");
		}
		else {
			table = data_manager::instance()->get_table("scenelayout");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Packet::SceneLayout>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			scenepos_map_.insert({ config->id(), config });
		}
	
	}

	const boost::shared_ptr<Config::MonsterGroup> MtMonsterManager::FindMonsterGroup(zxero::int32 id) const 
	{
		auto it = monster_group_map_.find(id);
		if (it != std::end(monster_group_map_)) {
			return it->second;
		}
		return nullptr;
	}

	const boost::shared_ptr<Config::MonsterAttribute> MtMonsterManager::FindMonsterAttribute(const int32 group_id, const int32 level)
	{
		if (level < 1 || level > 60) {
			ZXERO_ASSERT(false) << "invalid level value!";
			return nullptr;
		}
		auto it = monster_attribute_group_.find(group_id);
		if (it != monster_attribute_group_.end()) {
			return monster_attribute_group_[group_id][level-1];
		}
		return nullptr;
	}

	const boost::shared_ptr<Config::MonsterConfig> MtMonsterManager::FindMonster(const int32 monster_id) const
	{
		auto it = monster_map_.find(monster_id);
		if ( it != monster_map_.end()) {
			return it->second;
		}
		return nullptr;
	}

	const boost::shared_ptr<Packet::SceneLayout> MtMonsterManager::FindScenePos(const int32 pos_id) const
	{
		auto it = scenepos_map_.find(pos_id);
		if ( it != scenepos_map_.end()) {
			return it->second;
		}
		return nullptr;
	}

	std::vector<boost::shared_ptr< Packet::NpcDynamicConfig >>* MtMonsterManager::FindNpcGroup(const int32 npc_group)
	{
		if (npc_group_.find(npc_group) != npc_group_.end()) {
			return &npc_group_[npc_group];
		}
		return nullptr;
	}

	const boost::shared_ptr <Packet::NpcStaticConfig> MtMonsterManager::FindStaticNpc(const int32 index)
	{
		if (npc_static_.find(index) != npc_static_.end()) {
			return npc_static_[index];
		}
		return nullptr;
	}
	const boost::shared_ptr <Packet::NpcDynamicConfig> MtMonsterManager::FindDynamicNpc(const int32 index) {
		if (npc_dynamic_.find(index) != npc_dynamic_.end()) {
			return npc_dynamic_[index];
		}
		return nullptr;
	}

	const boost::shared_ptr <Packet::MountConfig> MtMonsterManager::FindMount(const int32 index) {
		if (mount_map_.find(index) != mount_map_.end()) {
			return mount_map_[index];
		}
		return nullptr;
	}

	boost::shared_ptr<Packet::NpcDynamicConfig> MtMonsterManager::FindNpcInGroup(const int32 npc_group, const int32 npc_id)
	{
		if (npc_group_.find(npc_group) != npc_group_.end()) {
			for (auto child : npc_group_[npc_group]) {
				if (npc_id == child->npc_id()) {
					return child;
				}
			}
		}
		return nullptr;
	}

	std::vector<boost::shared_ptr< Packet::PositionGroupConfig >>* MtMonsterManager::FindPosGroup(const int32 pos_group)
	{
		if (postion_group_.find(pos_group) != postion_group_.end()) {
			return &postion_group_[pos_group];
		}
		return nullptr;
	}

	std::vector<Packet::NpcDynamicConfig*> MtMonsterManager::GenRandomNpc(const int32 npc_group, int32 n)
	{
		std::vector<Packet::NpcDynamicConfig*> temp;
		if (n == 0 ) {
			return temp;
		}
		else
		{
			if (npc_group_.find(npc_group) == npc_group_.end()) {
				return temp;
			}
			int32 len = npc_group_[npc_group].size();
			if (len <= 0) {
				return temp;
			}
			if (n > len) {
				return temp;
			}

			if (n == 1)
			{
				int32 seed = (int32)rand_gen->intgen(0, len - 1);
				temp.push_back(npc_group_[npc_group][seed].get());
			}
			else
			{//≥È≈∆
				std::vector<int32> seeds;
				for (int32 i = 0; i < len; i++) {
					seeds.push_back(i);
				}
				
				while( n-- > 0){
					int32 seed = (int32)rand_gen->intgen(0, seeds.size() - 1);
					temp.push_back(npc_group_[npc_group][seeds[seed]].get());

					seeds.erase(seeds.begin() + seed);

					if (seeds.empty()) {
						break;
					}
				}				
			}
		}

		return temp;
	}

	std::vector<Packet::PositionGroupConfig*> MtMonsterManager::GenRandomPos(const int32 pos_group, int32 n)
	{
		std::vector<Packet::PositionGroupConfig*> temp;
		if (n == 0) {
			ZXERO_ASSERT(false);
			return temp;
		}
		else
		{
			if (postion_group_.find(pos_group) == postion_group_.end()) {
				ZXERO_ASSERT(false);
				return temp;
			}
			int32 len = postion_group_[pos_group].size();
			if (len <= 0) {
				ZXERO_ASSERT(false);
				return temp;
			}
			if (n > len) {
				ZXERO_ASSERT(false);
				return temp;
			}

			if (n == 1)
			{
				int32 seed = (int32)rand_gen->intgen(0, len - 1);
				temp.push_back(postion_group_[pos_group][seed].get());
			}
			else
			{//≥È≈∆
				std::vector<int32> seeds;
				for (int32 i = 0; i < len; i++) {
					seeds.push_back(i);
				}

				while (n-- > 0) {
					int32 seed = (int32)rand_gen->intgen(0, seeds.size() - 1);
					temp.push_back(postion_group_[pos_group][seeds[seed]].get());

					seeds.erase(seeds.begin() + seed);

					if (seeds.empty()) {
						break;
					}
				}
			}
		}
		return temp;
	
	}

	Packet::SceneLayout * MtMonsterManager::FindStaticNpcPosInfo(const int32 index)
	{
		int32 pos_id = -1;
		if (npc_static_.find(index) != npc_static_.end()) {
			pos_id= npc_static_[index]->pos_id();
		}

		if (scenepos_map_.find(pos_id) != scenepos_map_.end()) {
			return scenepos_map_[pos_id].get();
		}

		return nullptr;
	 }

	boost::shared_ptr<MtNpc> MtMonsterManager::CreateNpc(const int32 pos_id, const int32 index,const int32 relate_mission_id_, const int32 lifetime)
	{
		auto npc_config = FindDynamicNpc(index);
		if (npc_config == nullptr) {
			return nullptr;				 
		}

		auto pos_config = FindScenePos(pos_id);
		if (pos_config == nullptr) {
			return nullptr;
		}

		max_series_id_+=1;

		auto info = boost::make_shared <Packet::NpcInfo>();
		info->set_index(index);
		info->set_pos_id(pos_id);
		info->set_series_id(max_series_id_);
		info->set_lifetime(lifetime<0?99999999:lifetime);

		return boost::make_shared <MtNpc> (info, npc_config, pos_config, relate_mission_id_);
	}

	const std::vector<boost::shared_ptr<MtItemPackage>> MtMonsterManager::MonsterGroupRandomDrop(zxero::int32 monster_group_id) const
	{
		std::vector<boost::shared_ptr<MtItemPackage>> result;
		auto monster_group = FindMonsterGroup(monster_group_id);
		if (monster_group == nullptr)
			return result;
		for (auto i = 0; i < monster_group->package_id_size(); ++i) {
			auto weight = monster_group->package_weight(i);
			auto rand_result = rand_gen->intgen(1, 10000);
			if (rand_result < weight) {
				auto drop_id = monster_group->package_id(i);
				auto package = MtItemPackageManager::Instance().GetPackage(drop_id);
				ZXERO_ASSERT(package != nullptr);
				if (package != nullptr) {
					result.push_back(package);
				}
			}
		}
		return result;
	}
	const boost::shared_ptr<Config::MonsterConfig> MtMonsterManager::GetFirstMosterByGroupId(int32 monstergroupid)
	{
		auto monstergourpconfig = FindMonsterGroup(monstergroupid);
		if (monstergourpconfig == nullptr)
			return nullptr;

		int32 monsterid = monstergourpconfig->monsters(0).id();
		if (monsterid > 0)
		{
			auto monster = FindMonster(monsterid);
			return monster;
		}
		return nullptr;
	}

	zxero::int32 MtMonsterManager::MonsterCount(int32 group_id) const
	{
		auto config = FindMonsterGroup(group_id);
		if (config == nullptr) {
			return 0;
		} else {
			return std::count_if(config->monsters().begin(), 
				config->monsters().end(),
				boost::bind(&Config::MonsterDefine::id, _1) > 0);
		}
	}

	MtMonsterManager::~MtMonsterManager()
	{

	}


}