#include "module.h"
#include "app.h"
#include "luafile_manager.h"
#include <Base.pb.h>
#include <zlib.h>
#include "../mt_config/mt_config.h"
//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------

namespace Mt
{
	using namespace zxero;
	static luafile_manager *sg_luafile_manager = nullptr;
	REGISTER_MODULE(luafile_manager)
	{
		//require("data_manager");
		register_load_function(STAGE_LOAD, boost::bind(&luafile_manager::on_load, sg_luafile_manager));
		register_unload_function(STAGE_POST_UNLOAD, boost::bind(&luafile_manager::on_unload, sg_luafile_manager));

	}

	void luafile_manager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<luafile_manager, ctor()>(ls, "luafile_manager")
			.def(&luafile_manager::on_load_luafile, "on_load_luafile")
			.def(&luafile_manager::on_reload_luafile, "on_reload_luafile")
			.def(&luafile_manager::get_luafile, "get_luafile" )
			.def(&luafile_manager::get_luafiles,"get_luafiles")
			.def(&luafile_manager::on_update_luafile, "on_update_luafile")
			;
		ff::fflua_register_t<>(ls)
			.def(&luafile_manager::instance, "luafile_manager");
	}

	luafile_manager::luafile_manager()
	{
		sg_luafile_manager = this;
	}

	luafile_manager::~luafile_manager(void)
	{
		on_release();
	}

	luafile_manager& luafile_manager::instance()
	{
		return *sg_luafile_manager;
	}
	int32 luafile_manager::on_load()
	{
		on_load_luafile();

		return 0;
	}

	Packet::LuaFile * luafile_manager::get_luafile(const int32 script_id)
	{
		auto iter = m_luafiles.find(script_id);
		if (iter != m_luafiles.end()) {
			return iter->second;
		}
		return nullptr;
	}
	void luafile_manager::on_load_luafile()
	{	
		fflua.fflua_init(false, MtConfig::Instance().script_data_dir_,"script_config.lua");
		try {
			auto luaset = fflua.call_fun<std::vector<int>>("GetClientLuaScripts");
			for (auto child : luaset) {
				on_reload_luafile(child);
			}
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "on_load_luafile lua exception:" << e.what();
		}
	}

	void luafile_manager::on_update_luafile()
	{
		m_dirtylua.clear();
		on_load_luafile();		
	}

	std::vector<Packet::LuaFile *> luafile_manager::get_luafiles(bool dirty)
	{
		if (dirty) {
			return m_dirtylua;
		}
		else {
			std::vector<Packet::LuaFile *> temp;
			for (auto child : m_luafiles) {
				temp.push_back(child.second);
			}
			return temp;
		}
	}

	bool luafile_manager::on_reload_luafile(const int32 script_id)
	{
		auto name = fflua.get_name(script_id);
		char* lbuffer = 0;
		FILE * fp = fopen(name.c_str(), "rb");
		if (fp == nullptr) {
			ZXERO_ASSERT(false) << "fopen lua file failed! name=" << name;
			return false;
		}

		fseek(fp, 0, SEEK_END);
		auto size = ftell(fp);
		lbuffer = new char[size+1];//补个收尾符
		memset(lbuffer, 0, size+1);
		fseek(fp, 0, SEEK_SET);
		auto read_size = fread(lbuffer, 1, size, fp);
		if (size != (long)read_size) {
			ZXERO_ASSERT(false) << "fread lua file failed! name=" << name;
			fclose(fp);
			delete[] lbuffer;
			return false;
		}
		fclose(fp);
				
		uint64 crc_code = crc32(0, (const Bytef*)lbuffer, size);

		auto iter = m_luafiles.find(script_id);
		if (iter != m_luafiles.end()) {
			if (iter->second->crc() != crc_code) {
				iter->second->set_script_id(script_id);
				iter->second->set_crc(crc_code);
				iter->second->set_content(lbuffer);

				m_dirtylua.push_back(iter->second);
			}
		}
		else {
			Packet::LuaFile* luafile = SAFE_NEW Packet::LuaFile();
			luafile->set_script_id(script_id);
			luafile->set_crc(crc_code);
			luafile->set_content(lbuffer);
			m_luafiles.insert({ script_id,luafile });

			m_dirtylua.push_back(luafile);
		}
		delete[] lbuffer;
		return true;
	}

	int32 luafile_manager::on_unload()
	{
		on_release();
		return 0;
	}

	void luafile_manager::on_release()
	{
		for (auto child : m_luafiles) {
			SAFE_DELETE(child.second);
		}
		m_luafiles.clear();
	}
	
}

