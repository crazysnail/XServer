#include "mt_scene.h"
#include "mt_raid.h"
#include "../mt_player/mt_player.h"
#include <G2SMessage.pb.h>

namespace Mt
{
	bool MtScene::OnCacheAll(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<G2S::CacheAllNotify>&, int32 /*source*/)
	{
		if (player != nullptr)
		{
			player->OnCache2Shm();
			return true;
		}
		for (auto& it : player_map_) {
			it.second->OnCache2Shm();
		}
		return true;
	}
	bool MtScene::OnXLuaReg(const boost::shared_ptr<MtPlayer>&,
		const boost::shared_ptr<G2S::XLuaRegNotify>&, int32 /*source*/)
	{
		lua_call_.Init();
		try {
			//初始化活动配置数据
			thread_lua->call<int>(4, "OnOperateConfigInit", this);
		}
		catch (ff::lua_exception& e)
		{
			ZXERO_ASSERT(false) << "LuaInit exception:" << e.what() << "scene id=" << SceneId();
		}
		LOG_INFO << "OnXLuaReg done! scene id=" << SceneId();
		return true;
	}
	bool MtScene::OnLuaLoad(const boost::shared_ptr<MtPlayer>&,
		const boost::shared_ptr<G2S::LuaLoadNotify>& msg, int32 /*source*/)
	{
		if (msg->ids_size()==0) {
			thread_lua->load_all();
			LOG_INFO << "OnLuaLoad done! scene id:" << SceneId();
		}
		else {
			for (auto id : msg->ids()) {
				thread_lua->reload(id);
				LOG_INFO << "OnLuaLoad done! scene id:" << SceneId()
					<< ". id:" << id;
			}
		}

		try	{
			//初始化活动配置数据
			thread_lua->call<int>(4, "OnOperateConfigInit", this);
		}
		catch (ff::lua_exception& e)
		{
			ZXERO_ASSERT(false) << "LuaInit exception:" << e.what() << "scene id=" << SceneId();
		}

		return true;
	}
	bool MtScene::OnPlayerOnline(const boost::shared_ptr<MtPlayer>& player,
		const boost::shared_ptr<G2S::PlayerOnline>&, int32 /*source*/)
	{
		if (player) {
			player->OnLogin();
		}
		return true;
	}
}

