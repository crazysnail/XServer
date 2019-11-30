#ifndef MTONLINE_GAMESERVER_MT_ITEM_PACKAGE_H__
#define MTONLINE_GAMESERVER_MT_ITEM_PACKAGE_H__
#include "../mt_types.h"
#include <AllConfigEnum.pb.h>
#include <map>

namespace Mt
{
	struct MtItemDropGroup
	{
		std::vector < boost::shared_ptr < Config::ItemDropGroup>> items;
		int32 all_weight;
	};

	class MtItemPackage;
	class GuildInfo;
	class MtItemPackageManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtItemPackageManager();
		static MtItemPackageManager& Instance();
		boost::shared_ptr<MtItemPackage> GetPackage(int32 id);
		boost::shared_ptr<MtItemDropGroup> GetDropGroup(int32 id);
		const Config::ItemPackageElement*  GenDropFromGroup(int32 id);
		int32 OnLoad();

		void LoadPackageTB(bool reload = false);

	private:
		std::map<zxero::int32, boost::shared_ptr<MtItemPackage>> configs_;
		std::map<zxero::int32, boost::shared_ptr<MtItemDropGroup>> drop_configs_;

	};

	class MtItemPackage : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtItemPackage(){}
		MtItemPackage(const boost::shared_ptr<Config::ItemPackageConfig>& db_info_);
		zxero::int32 ItemCount();
		void CalcGroupWeight();
		bool CanAddToPlayer(MtPlayer* player);
		const boost::shared_ptr<Config::ItemPackageConfig> GetConfig() { return config_info_; }
		bool AddToPlayer(MtPlayer* player, Packet::StatisticInfo* result=nullptr,
						 Packet::NotifyItemList* notify = nullptr,
						 std::function<int(int)> tax_collector = std::function<int(int)>(),
						 bool from_battle=false,
						 Config::ItemAddLogType type = Config::ItemAddLogType::AddType_Package );
		bool LuaAddToPlayer(MtPlayer* player);
		std::map<int32, int32> LuaGenItems();
		bool GenItems(std::map<int32, int32>& items);
		//掉落至分配背包
		bool AddToPlayerArrange(MtPlayer* player );
		//
		bool AddToGuildSpoil(GuildInfo* guild, MtPlayer* player,int32 copyid);
		void SetMissionId(const int32 mission_id) {
			mission_id_ = mission_id;
		}
	private:
		void DropGroup(std::map<int32, int32>& items);
	private:
		boost::shared_ptr<Config::ItemPackageConfig> config_info_;
		int32 all_group_weight_ = 0;
		int32 mission_id_ = 0;
	};
}
#endif // MTONLINE_GAMESERVER_MT_ITEM_PACKAGE_H__
