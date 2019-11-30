#ifndef MTONLINE_GAMESERVER_MT_DRAW_H__
#define MTONLINE_GAMESERVER_MT_DRAW_H__
#include "../mt_types.h"
#include "AllPacketEnum.pb.h"
#include "AllConfigEnum.pb.h"
namespace Mt
{
	//enum DrawGroup
	//{
	//InvalidDrawGroup = -1; //��Чֵ
	//CrystalFree = 0;	//��ʯ���
	//CrystalMust = 1;	//��ʯ����
	//HellBigDraw = 2;	//��������
	//CoinFree = 3;		//������
	//CoinMust = 4;		//��ұ���
	//HellSmallDraw = 5;	//����С����
	//CrystalBigMust = 6;	//�߼���ļ3�Ǳ���
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
