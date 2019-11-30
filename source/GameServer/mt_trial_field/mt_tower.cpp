#include "mt_tower.h"
#include "module.h"
#include "TrialFieldDB.pb.h"
#include "active_model.h"
#include "../mt_server/mt_server.h"
#include "../ranklist/top_ranklist.h"
#include "../base/mt_db_load_work.h"
#include "../mt_cache/mt_shm_manager.h"

namespace Mt
{

	static MtTowerManager* s_tower_manager = nullptr;
	REGISTER_MODULE(MtTowerManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtTowerManager::OnLoad, s_tower_manager));
	}

	void MtTowerManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtTowerManager, ctor()>(ls, "MtTowerManager")
			.def(&MtTowerManager::UpdatePlayerTowerInfo, "UpdatePlayerTowerInfo")
			.def(&MtTowerManager::ClearPlayerTowerInfo, "ClearPlayerTowerInfo")
			.def(&MtTowerManager::GetPlayerTowerInfo, "GetPlayerTowerInfo")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtTowerManager::Instance, "LuaMtTowerManager");
	}

	MtTowerManager& MtTowerManager::Instance()
	{
		return *s_tower_manager;
	}

	MtTowerManager::MtTowerManager()
	{
		s_tower_manager = this;
	}

	int32 MtTowerManager::OnLoad()
	{
		return 0;
	}
	
	void MtTowerManager::OnDBLoad(const std::vector<boost::shared_ptr<DB::PlayerTowerInfo>>& infos)
	{
		for (auto child : infos) {
			tower_info_map_.insert({ child->guid(),child });
		}
	}
	void MtTowerManager::ClearPlayerTowerInfo()
	{
		std::set<google::protobuf::Message *> msg_set;
		for (auto child : tower_info_map_) {
			child.second->SetDeleted();	
			msg_set.insert(child.second.get());
			MtShmManager::Instance().Cache2Shm(msg_set, "TowerManager");			
		}
		tower_info_map_.clear();
		MtTopRankManager::Instance().OnWeekCleanPlayerTower();
	}

	void MtTowerManager::OnCacheAll()
	{
		std::set<google::protobuf::Message *> msg_set;
		for (auto child : tower_info_map_) {
			msg_set.insert(child.second.get());
			MtShmManager::Instance().Cache2Shm(msg_set, "TowerManager");
		}
	}

	void MtTowerManager::UpdatePlayerTowerInfo(const uint64 guid, const int32 tower_level, const uint32 tower_time)
	{
		auto iter = tower_info_map_.find(guid);
		if (iter != tower_info_map_.end()) {
			iter->second->set_guid(guid);
			iter->second->set_tower_level(tower_level);
			iter->second->set_tower_time(tower_time);
		}
		else {
			auto info = boost::make_shared<DB::PlayerTowerInfo>();
			info->set_guid(guid);
			info->set_tower_level(tower_level);
			info->set_tower_time(tower_time);
			tower_info_map_.insert({ guid,info });
		}
		auto player_ = Server::Instance().FindPlayer(guid);
		if (player_)
		{
			MtTopRankManager::Instance().OnPlayerTowerChanged(player_, tower_level, tower_time);
		}
	}

	boost::shared_ptr<DB::PlayerTowerInfo> MtTowerManager::GetPlayerTowerInfo(const uint64 guid)
	{
		auto it = tower_info_map_.find(guid);
		if (it == std::end(tower_info_map_)) {
			return nullptr;
		}
		return it->second;
	}

}