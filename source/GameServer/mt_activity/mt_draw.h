#ifndef MTONLINE_GAMESERVER_MT_DRAW_H__
#define MTONLINE_GAMESERVER_MT_DRAW_H__
#include "../mt_types.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
	//enum DrawGroup
	//{
	//InvalidDrawGroup = -1; //无效值
	//CrystalFree = 0;	//钻石免费
	//CrystalMust = 1;	//钻石保底
	//HellBigDraw = 2;	//试炼大宝箱
	//CoinFree = 3;		//金币免费
	//CoinMust = 4;		//金币保底
	//HellSmallDraw = 5;	//试炼小宝箱
	//CrystalBigMust = 6;	//高级招募3星保底
	//}

	class MtDrawManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:

		static MtDrawManager& Instance();
		MtDrawManager();
		int32 OnLoad();

		void GetDrawList(const int32 function_group, const int32 level, std::vector<boost::shared_ptr<Config::DrawCardConfig>> &pool, int32 &all_weiht);
		std::vector<Config::DrawCardConfig *> LuaGetDrawList(const int32 function_group, const int32 level );
		void LoadDrawTB(bool reload = false);
	private:
		std::map<int32, std::vector<boost::shared_ptr<Config::DrawCardConfig>>> draw_configs_;
	};
	
}
#endif // MTONLINE_GAMESERVER_MT_ACTIVITY_H__
