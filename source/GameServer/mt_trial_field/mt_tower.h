#ifndef MTONLINE_GAMESERVER_MT_TOWER_H__
#define MTONLINE_GAMESERVER_MT_TOWER_H__
#include "../mt_types.h"

namespace Mt
{
	class MtTowerManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtTowerManager& Instance();
		MtTowerManager();
		~MtTowerManager() {};
		
		
		boost::shared_ptr<DB::PlayerTowerInfo> GetPlayerTowerInfo(const uint64 guid);
		void UpdatePlayerTowerInfo(const uint64 guid,const int32 tower_level, const uint32 tower_time);
		void ClearPlayerTowerInfo();
		int32 OnLoad();
		void OnDBLoad(const std::vector<boost::shared_ptr<DB::PlayerTowerInfo>>& infos);
		void OnCacheAll();
	private:
		std::map<zxero::uint64, boost::shared_ptr<DB::PlayerTowerInfo>> tower_info_map_;
	};
}
#endif // MTONLINE_GAMESERVER_MT_TRIAL_FIELD_H__
