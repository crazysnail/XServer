#ifndef ZXERO_GAMESERVER_LUAFILE_MANAGER_H__
#define ZXERO_GAMESERVER_LUAFILE_MANAGER_H__


#include "../mt_types.h"
#include "lua/fflua.h"

namespace Mt
{

	class luafile_manager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		luafile_manager();
		~luafile_manager();
		int32 on_load();
		int32 on_unload();

		void on_release();
		void on_load_luafile();
		bool on_reload_luafile(const int32 script_id);
		void on_update_luafile();
		std::vector<Packet::LuaFile *> get_luafiles(bool dirty);
		Packet::LuaFile * get_luafile(const int32 script_id);

		static luafile_manager& instance();

	private:
		ff::fflua_t fflua;
		std::map<int32, Packet::LuaFile *> m_luafiles;
		std::vector<Packet::LuaFile *> m_dirtylua;
	};

}
#endif // ZXERO_GAMESERVER_LUAFILE_MANAGER_H__