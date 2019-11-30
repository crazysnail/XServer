#ifndef MTONLINE_GAMESERVER_MT_LUA_PROXY_H__
#define MTONLINE_GAMESERVER_MT_LUA_PROXY_H__

#include "../mt_types.h"
namespace Mt
{
	class Server;
	class MtLuaCallProxy : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		MtLuaCallProxy() {
		};
		virtual ~MtLuaCallProxy() {
			m_delegates.clear();
		};
		
		void Init();
		void RegFunc(const std::string& funcname, const int32 script_id);
		bool Registed(const std::string& funcname) const;
		int32 CallFunc_n(MtPlayer* player, const int32 script_id, const std::string& funcname, const std::map<std::string, int32>& params);
		int32 CallFunc_x(MtPlayer* player, const std::string& funcname, const std::map<std::string, int32>& params);
		
		int32 OnLuaFunCall_OnMessageHandler(MtPlayer* player, const std::string& funcname, google::protobuf::Message *message);
		void CallMsgHandler(MtPlayer* player, const std::string& funcname, google::protobuf::Message* msg);

		int32 OnLuaFunCall_n_Scene(MtScene * scene, const int32 script_id, const std::string &func_name, const std::map<std::string, int32> params);
		int32 OnLuaFunCall_x_Scene(MtScene * scene, const std::string &func_name, const std::map<std::string, int32> params);

		int32 OnLuaFunCall_n_Server(Server * server, const int32 script_id, const std::string &func_name, const std::map<std::string, int32> params);
		int32 OnLuaFunCall_x_Server(Server * server, const std::string &func_name, const std::map<std::string, int32> params);

	private:
		std::map<std::string, std::set<int32>> m_delegates;
	};


}
#endif // MTONLINE_GAMESERVER_MT_SIMPLE_IMPACT_H__
