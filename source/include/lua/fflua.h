#ifndef _FF_LUA_H_
#define _FF_LUA_H_

#ifndef  _WIN32
#include <stdint.h>
#endif

#include <stdlib.h>
#include <lua.hpp>
#include <lstate.h>

#include <string>
using namespace std;

#include "lua/fflua_type.h"
#include "lua/fflua_register.h"
#include "log.h"

namespace ff
{
class fflua_t;

class LuaStackHelper
{
public:
	LuaStackHelper(lua_State* ls)
		:ls_(ls)
	{
		top_ = lua_gettop(ls_);
	}
	~LuaStackHelper()
	{
		lua_settop(ls_, top_);
	}
	int RetCount() const
	{
		return lua_gettop(ls_) - top_;
	}
private:
	lua_State* ls_ = nullptr;
	int top_ = 0;
};

class FFLuaCounter
{
public:
	FFLuaCounter(fflua_t& lua);
	~FFLuaCounter();
private:
	fflua_t& lua_;
};



int track_back(lua_State *L);
//! 表示void类型，由于void类型不能return，用void_ignore_t适配
template<typename T>
struct void_ignore_t;

template<typename T>
struct void_ignore_t
{
    typedef T value_t;
};

template<>
struct void_ignore_t<void>
{
    typedef cpp_void_t value_t;
};

#define  RET_V typename void_ignore_t<RET>::value_t

#define MAX_SCRIPT_HOLD 4096


class fflua_t
{
    enum STACK_MIN_NUM_e
    {
        STACK_MIN_NUM = 20
    };
public:
    fflua_t(bool b = false):
		m_ls(NULL),
        m_bEnableModFunc(b)
	{
		if (m_ls == NULL)
		{
			m_ls = ::luaL_newstate();
		}
		::luaL_openlibs(m_ls);
	}
    virtual ~fflua_t()
    {
		exit();
    }

	void exit() {
		if (m_ls != NULL) {
			lua_close(m_ls);
			m_ls = NULL;
		}
	}


	bool fflua_init(bool bEnableModFunc, const std::string& script_path, const std::string& config_file);
    void dump_stack() const { fflua_tool_t::dump_stack(m_ls); }
    void setModFuncFlag(bool b) { m_bEnableModFunc = b; }

    lua_State* get_lua_state()
    {
        return m_ls;
    }

    int  add_package_path(const string& str_)
    {
        string new_path = "package.path = package.path .. \"";
        if (str_.empty())
        {
            return -1;
        }

        if (str_[0] != ';')
        {
           new_path += ";";
        }

        new_path += str_;

        if (str_[str_.length() - 1] != '/')
        {
            new_path += "/";
        }

        new_path += "?.lua\" ";

        run_string(new_path);
        return 0;
    }
    int  load_file(const string& file_name_)// 
	{
		if (file_name_.empty())
		{
			return -1;
		}

		//读文件就清空堆栈
		//lua_settop(m_ls, 0); //by ryan, 暂时不管这个, 后面有问题来调
		if (m_ls->nCcalls > 100) {
			ZXERO_ASSERT(false);
		}
		LuaStackHelper helper(m_ls); // luaL_dofile里面调用的lua_pcall用了LUA_MULTRET, 所以需要处理栈
		if (luaL_dofile(m_ls, file_name_.c_str()))
		{
			string err = fflua_tool_t::dump_error(m_ls, "luaL_dofile<%s> failed!", file_name_.c_str());
			LOG_ERROR << "#lua exception#" << err;
			throw lua_exception(err);
		}

		return 0;
	}
    template<typename T>
    void open_lib(T arg_);

    void run_string(const char* str_) 
	{
		LuaStackHelper helper(m_ls);
		if (m_ls->nCcalls > 100) {
			ZXERO_ASSERT(false);
		}
		if (luaL_dostring(m_ls, str_))
		{
			string err = fflua_tool_t::dump_error(m_ls, "fflua_t::run_string ::lua_pcall faled str<%s>", str_);

			LOG_ERROR << "#lua call exception error!#" << err;
			throw lua_exception(err);
		}
	}
    void run_string(const string& str_) 
    {
        run_string(str_.c_str());
    }

    template<typename T>
    int  get_global_variable(const string& field_name_, T& ret_);
    template<typename T>
    int  get_global_variable(const char* field_name_, T& ret_);

    template<typename T>
    int  set_global_variable(const string& field_name_, const T& value_);
    template<typename T>
    int  set_global_variable(const char* field_name_, const T& value_);

    void  register_raw_function(const char* func_name_, lua_function_t func_)
    {
        lua_checkstack(m_ls, STACK_MIN_NUM);

        lua_pushcfunction(m_ls, func_);
        lua_setglobal(m_ls, func_name_);
    }

    template<typename T>
    void  reg(T a);

	void reload(const ScriptID_t scriptid);
	bool load_all();
	bool precall(const ScriptID_t scriptid);
	bool load_one(const ScriptID_t script_id, bool reload);
	
	bool addreturn(std::string& str);
	bool execute_str(const std::string& str, const std::string& chunk_name);

	template<typename RET, typename... ARGS>
	RET_V call_fun(const std::string& func_name, ARGS... args)
	{
		RET_V ret = init_value_traits_t<RET_V>::value();
		FFLuaCounter counter(*this);
		LuaStackHelper helper(m_ls);
		if (m_ls->nCcalls > 10) {
			ZXERO_ASSERT(false);
		}
		lua_pushcfunction(m_ls, track_back);
		int errfun_index = lua_gettop(m_ls);
		if (lua_getglobal(m_ls, func_name.c_str()) == 0) {
			std::string exp = ("get global function fail:");
			exp += func_name;
			throw lua_exception(exp);
		}
		int args_count = sizeof...(args);
		PushArgs(args...);
		if (lua_pcall(m_ls, args_count, 1, errfun_index) != 0)
		{
			const char *msg = lua_tostring(m_ls, -1);
			if (msg == NULL) {  /* is error object not a string? */
				msg = "unknown lua script error";
			}
			LOG_ERROR << msg;
			throw lua_exception(msg);
		}

		if (lua_op_t<RET_V>::get_ret_value(m_ls, -1, ret))
		{
			char buff[512];
			SPRINTF_F(buff, sizeof(buff), "callfunc func_name<%s> get_ret_value failed", func_name.c_str());
			LOG_ERROR << "#lua call exception error!#" << buff;
			throw lua_exception(buff);
		}
		return ret;
	}

	template<typename RET, typename... ARGS>
	RET_V call(ScriptID_t script_id, const char* func_name, ARGS... args)
	{
		RET_V ret = init_value_traits_t<RET_V>::value();
		if (!precall(script_id)) {
			ZXERO_ASSERT(false) << "load script with id " << script_id << " error";
			return ret;
		}
		FFLuaCounter counter(*this);
		LuaStackHelper helper(m_ls);
		if (m_ls->nCcalls > 10) {
			LOG_ERROR<<"m_ls->nCcalls is more!";
		}
		lua_pushcfunction(m_ls, track_back);
		int errfun_index = lua_gettop(m_ls);
		char function_name[128] = { 0 };
		zprintf(function_name, 128, "x%.6d_%s", script_id, func_name);
		int tmp_args_count = get_func_by_name(function_name);
		int args_count = sizeof...(args);
		PushArgs(args...);
		if (lua_pcall(m_ls, tmp_args_count + args_count, 1, errfun_index) != 0)
		{
			const char *msg = lua_tostring(m_ls, -1);
			if (msg == NULL) {  /* is error object not a string? */
				msg = "unknown lua script error";
			}
			LOG_ERROR << msg;
			throw lua_exception(msg);
		}

		if (lua_op_t<RET_V>::get_ret_value(m_ls, -1, ret))	
		{
			char buff[512];
			SPRINTF_F(buff, sizeof(buff), "callfunc func_name<%s> get_ret_value failed", function_name);
			LOG_ERROR << "#lua call exception error!#" << buff;
			throw lua_exception(buff);
		}
		return ret;
	}

	template<typename ARG, typename... ARGS>
	void PushArgs(const ARG& arg, const ARGS&... args)
	{
		lua_op_t<ARG>::push_stack(m_ls, arg);
		return PushArgs(args...);
	}
	void PushArgs()
	{
	}
	std::map<int, std::string> &get_luaset() { return m_script_set; };
	const std::string get_name(const ScriptID_t script_id);
protected:
	friend class FFLuaCounter;
	void IncrCallCount() 
	{
		//c++ -> lua -> c++ -> lua -> c++ -> lua, 依赖调用关系
		const int max_call_reference = 7;//
		m_lua_call_count++; 
		ZXERO_ASSERT(m_lua_call_count <= max_call_reference);
	}
	void DecrCallCount() { m_lua_call_count--; }

private:
	int  get_func_by_name(const char* func_name_);	
private:
    lua_State*  m_ls;
	int			m_lua_call_count = 0;
    bool        m_bEnableModFunc;
	std::string m_script_path;
	std::string m_config_file;
	std::map<int, std::string> m_script_set;
};

template<typename T>
void fflua_t::open_lib(T arg_)
{
    arg_(m_ls);
}

template<typename T>
int  fflua_t::get_global_variable(const string& field_name_, T& ret_)
{
    return get_global_variable<T>(field_name_.c_str(), ret_);
}

template<typename T>
int  fflua_t::get_global_variable(const char* field_name_, T& ret_)
{
	LuaStackHelper helper(m_ls);
    int ret = 0;

    lua_getglobal(m_ls, field_name_);
    ret = lua_op_t<T>::get_ret_value(m_ls, -1, ret_);
    return ret;
}

template<typename T>
int  fflua_t::set_global_variable(const string& field_name_, const T& value_)
{
    return set_global_variable<T>(field_name_.c_str(), value_);
}

template<typename T>
int  fflua_t::set_global_variable(const char* field_name_, const T& value_)
{
    lua_op_t<T>::push_stack(m_ls, value_);
    lua_setglobal(m_ls, field_name_);
    return 0;
}

template<typename T>
void  fflua_t::reg(T a)
{
    a(this->get_lua_state());
}

}
#endif
