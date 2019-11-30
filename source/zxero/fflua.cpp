#include "lua/fflua.h"
#include "lstate.h"

namespace ff
{
	int track_back(lua_State *L) {
		const char *msg = lua_tostring(L, -1);
		if (msg == NULL) {  /* is error object not a string? */
			if (luaL_callmeta(L, -1, "__tostring") &&  /* does it have a metamethod */
				lua_type(L, -1) == LUA_TSTRING)  /* that produces a string? */
				return 1;  /* that is the message */
			else
				msg = lua_pushfstring(L, "(error object is a %s value)",
					luaL_typename(L, -1));
		}
		luaL_traceback(L, L, msg, 1);  /* append a standard traceback */
		return 1;  /* return the traceback */
	}

	bool fflua_t::fflua_init(bool bEnableModFunc, const std::string& script_path, const std::string& config_file)
	{
		m_bEnableModFunc = bEnableModFunc;
		m_script_path = script_path;
		m_config_file = config_file;

		return load_all();
	}

	void fflua_t::reload(const ScriptID_t scriptid)
	{
		load_one(scriptid, true);
	}

	bool fflua_t::load_all()
	{
		try {
			m_script_set.clear();

			if (load_file(m_script_path + m_config_file) != 0) {
				return false;
			}
			auto configs = call_fun<std::map<int, std::string>>("GetRegLuaScripts");
			for (auto child : configs) {
				if (m_script_set.find(child.first) != m_script_set.end()) {
					ZXERO_ASSERT(false) << "lua script already loaded! id=" << child.first << " name=" << child.second;
					continue;
				}
				try {
					if (load_file(m_script_path + child.second) == 0) {
						m_script_set[child.first] = m_script_path + child.second;
					}
				}
				catch (ff::lua_exception& e) {
					ZXERO_ASSERT(false) << "lua exception:" << e.what();
				}
			}
		}
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();			
			return false;
		}
		return true;
	}

	bool fflua_t::precall(const ScriptID_t scriptid)
	{
#ifdef LUA_DEBUG
		return load_one( scriptid, true);
#else
		return load_one( scriptid, false);
#endif
	}

	bool fflua_t::addreturn(std::string& str)
	{
		LuaStackHelper helper(m_ls);
		std::string new_str = "return " + str;
		int ret = luaL_loadbuffer(m_ls, new_str.c_str(), new_str.size(), "=inner");
		if (ret == LUA_OK) {
			str = new_str;
			return true;
		}
		if (ret == LUA_ERRSYNTAX) {
			ret = luaL_loadbuffer(m_ls, str.c_str(), str.size(), "=inner");
			if (ret == LUA_OK) {
				return true;
			}
		}
		LOG_INFO << lua_tostring(m_ls, -1);
		LOG_INFO << str;
		return false;
	}

	bool fflua_t::execute_str(const std::string& str, const std::string& chunk_name)
	{
		int base = lua_gettop(m_ls);
		int ret = luaL_loadbuffer(m_ls, str.c_str(), str.size(), chunk_name.c_str());
		if (ret == LUA_OK) {
			int status = lua_pcall(m_ls, 0, LUA_MULTRET, 0);
			return status == LUA_OK;
		}
		if (ret == LUA_ERRSYNTAX) {
			const char* err = lua_tostring(m_ls, base + 1);
			LOG_ERROR << "load [" << str << "] err:" << err;
		}
		return false;
	}

	int  fflua_t::get_func_by_name(const char* func_name_)
	{

		if (false == m_bEnableModFunc)
		{
			lua_getglobal(m_ls, func_name_);
			return 0;
		}
		char tmpBuff[512] = { 0 };
		char* begin = tmpBuff;
		for (unsigned int i = 0; i < sizeof(tmpBuff); ++i)
		{
			char c = func_name_[i];
			tmpBuff[i] = c;
			if (c == '\0')
			{
				break;
			}

			if (c == '.')
			{
				tmpBuff[i] = '\0';
				lua_getglobal(m_ls, lua_string_tool_t::c_str(begin));
				const char* begin2 = func_name_ + i + 1;
				lua_getfield(m_ls, -1, begin2);
				lua_remove(m_ls, -2);
				return 0;
			}
			else if (c == ':')
			{
				tmpBuff[i] = '\0';
				lua_getglobal(m_ls, begin);
				const char* begin2 = func_name_ + i + 1;
				lua_getfield(m_ls, -1, begin2);
				lua_pushvalue(m_ls, -2);
				lua_remove(m_ls, -3);
				return 1;
			}
		}

		ZXERO_ASSERT(lua_getglobal(m_ls, func_name_) != 0) << "lua function null! name=" << func_name_;
		return 0;
	}
	
	bool fflua_t::load_one(const ScriptID_t script_id, bool reload)
	{
		auto script_name = get_name(script_id);
		if (script_name == "") {
			return false;
		}
		if (reload) {
			if (load_file(script_name) != 0) {
				return false;
			}
		}
		return true;
	}

	const std::string fflua_t::get_name(const ScriptID_t script_id)
	{
		if (m_script_set.find(script_id) == m_script_set.end()) {
			return "";
		}
		return m_script_set[script_id];
	}

	//////////////////////////////////

	FFLuaCounter::FFLuaCounter(fflua_t& lua) :lua_(lua)
	{
		lua_.IncrCallCount();
	}


	FFLuaCounter::~FFLuaCounter()
	{
		lua_.DecrCallCount();
	}

}