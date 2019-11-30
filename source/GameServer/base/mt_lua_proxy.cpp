
#include "mt_lua_proxy.h"
#include "active_model.h"
#include "module.h"
#include "../mt_server/mt_server.h"
#include "../mt_scene/mt_scene.h"
#include "../mt_player/mt_player.h"

namespace Mt
{
	void MtLuaCallProxy::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtLuaCallProxy, ctor()>(ls, "MtLuaProxy")
			.def(&MtLuaCallProxy::RegFunc, "RegFunc")
			.def(&MtLuaCallProxy::Init, "Init")
			.def(&MtLuaCallProxy::OnLuaFunCall_n_Scene, "OnLuaFunCall_n_Scene")
			.def(&MtLuaCallProxy::OnLuaFunCall_x_Scene, "OnLuaFunCall_x_Scene")
			.def(&MtLuaCallProxy::OnLuaFunCall_n_Server, "OnLuaFunCall_n_Server")
			.def(&MtLuaCallProxy::OnLuaFunCall_x_Server, "OnLuaFunCall_x_Server")
			;

		ff::fflua_register_t<boost::shared_ptr<MtLuaCallProxy>, ctor()>(ls, "MtLuaCallProxy");
	}

	void MtLuaCallProxy::Init()
	{
		m_delegates.clear();
		auto ids = thread_lua->call<std::vector<int32>>(10, "GetRegisterableId");
		try {
			for (auto child : ids) {
				thread_lua->reload(child);
				thread_lua->call<void>(child, "OnRegisterFunction", this);
			}
		}						   
		catch (ff::lua_exception& e) {
			ZXERO_ASSERT(false) << "lua exception:" << e.what();
			return;
		}
	}

	void MtLuaCallProxy::RegFunc(const std::string& funcname, const int32 script_id)
	{
		if (m_delegates.find(funcname) == m_delegates.end()) {
			std::set<int32> script_set;
			script_set.insert(script_id);
			m_delegates.insert({ funcname, script_set });
		}
		else {
			m_delegates[funcname].insert(script_id);
		}
	}

	int32 MtLuaCallProxy::CallFunc_n(MtPlayer* player, const int32 script_id, const std::string& funcname, const std::map<std::string, int32>& params)
	{
		if (player == nullptr) {
			return -1;
		}
		if (m_delegates.find(funcname) == m_delegates.end()) {
			LOG_WARN << "invalid funcname! name=" << funcname << " player guid=" << player->Guid();
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
		try {
			for (auto c : m_delegates[funcname]) {
				if (c == script_id ) {
					auto ret = (Packet::ResultCode)thread_lua->call<int32>(c, funcname.c_str(), player, params);
					if (ret != Packet::ResultCode::ResultOK) {
						player->SendCommonResult(Packet::ResultOption::Default_Opt, ret);
						return -1;
					}
					break;
				}
			}
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << player->Guid() << e.what();
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
		return 0;
		
	}

	int32 MtLuaCallProxy::CallFunc_x(MtPlayer* player, const std::string& funcname, const std::map<std::string,int32>& params)
	{
		if (player == nullptr) {
			return -1;
		}
		if (m_delegates.find(funcname) == m_delegates.end()) {
			LOG_WARN << "invalid funcname! name=" << funcname << " player guid=" << player->Guid();
			//player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
		try {
			for (auto c : m_delegates[funcname]) {
				auto ret = (Packet::ResultCode)thread_lua->call<int32>(c, funcname.c_str(), player, params);
				if (ret != Packet::ResultCode::ResultOK) {
					player->SendCommonResult(Packet::ResultOption::Default_Opt, ret);
				}
			}
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << player->Guid() << e.what();
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}	
		return 0;
	}

	int MtLuaCallProxy::OnLuaFunCall_n_Scene(MtScene * scene, const int32 script_id, const std::string &func_name, const std::map<std::string, int32> params)
	{
		if (scene == nullptr) {
			return -1;
		}
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), scene, params);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << "LuaScriptException" << e.what();
			return -1;
		}
	}

	int32 MtLuaCallProxy::OnLuaFunCall_x_Scene(MtScene * scene, const std::string& funcname, const std::map<std::string, int32> params)
	{
		if (scene == nullptr ) {
			return -1;
		}
		if (m_delegates.find(funcname) == m_delegates.end()) {
			LOG_WARN << "invalid funcname! name=" << funcname << " sceneid=" << scene->SceneId();
			return -1;
		}
		try {
			for (auto c : m_delegates[funcname]) {
				auto ret = (Packet::ResultCode)thread_lua->call<int32>(c, funcname.c_str(), scene, params);
				if (ret != Packet::ResultCode::ResultOK) {
					LOG_ERROR << " sceneid =" << scene->SceneId() << " OnLuaFunCall_x_Scene error! ret=" << ret;
				}
			}
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " sceneid =" << scene->SceneId() << e.what();
			return -1;
		}
		return 0;
	}

	int MtLuaCallProxy::OnLuaFunCall_n_Server(Server * server, const int32 script_id, const std::string &func_name, const std::map<std::string, int32> params)
	{
		if (server == nullptr) {
			return -1;
		}
		try {
			return thread_lua->call<int32>(script_id, func_name.c_str(), server, params);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << "LuaScriptException OnLuaFunCall_x_Server error!" << e.what();
			return -1;
		}
	}

	int32 MtLuaCallProxy::OnLuaFunCall_x_Server(Server * server, const std::string& funcname, const std::map<std::string, int32> params)
	{
		if (server == nullptr) {
			return -1;
		}
		if (m_delegates.find(funcname) == m_delegates.end()) {
			return -1;
		}
		try {
			for (auto c : m_delegates[funcname]) {
				auto ret = (Packet::ResultCode)thread_lua->call<int32>(c, funcname.c_str(), server, params);
				if (ret != Packet::ResultCode::ResultOK) {
					LOG_ERROR << "OnLuaFunCall_x_Server error! ret=" << ret;
				}
			}
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << "LuaScriptException OnLuaFunCall_x_Server error!" << e.what();
			return -1;
		}
		return 0;
	}

	int32 MtLuaCallProxy::OnLuaFunCall_OnMessageHandler(MtPlayer* player, const std::string& funcname, google::protobuf::Message *message)
	{
		if (player == nullptr) {
			return -1;
		}
		if (m_delegates.find(funcname) == m_delegates.end()) {
			LOG_WARN << "invalid funcname! name=" << funcname << " player guid=" << player->Guid();
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
		try {
			if (funcname == "xOnLuaRequestPacket") {
				auto msg = static_cast<Packet::LuaRequestPacket*>((void*)message);
				for (auto c : m_delegates[funcname]) {
					auto ret = (Packet::ResultCode)thread_lua->call<int32>(c, funcname.c_str(), player, msg);
					if (ret != Packet::ResultCode::ResultOK) {
						player->SendCommonResult(Packet::ResultOption::Default_Opt, ret);
					}
				}
			}
			else if (funcname == "xOnCommonRequestPacket") {
				auto msg = static_cast<Packet::CommonRequest*>((void*)message);
				for (auto c : m_delegates[funcname]) {
					auto ret = (Packet::ResultCode)thread_lua->call<int32>(c, funcname.c_str(), player, msg);
					if (ret != Packet::ResultCode::ResultOK) {
						player->SendCommonResult(Packet::ResultOption::Default_Opt, ret);
					}
				}
			}
			else {
				ZXERO_ASSERT(false) << "invalid message name!" << " player guid=" << player->Guid();
			}
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << " player guid=" << player->Guid() << e.what();
			player->SendCommonResult(Packet::ResultOption::Default_Opt, Packet::ResultCode::LuaScriptException);
			return -1;
		}
		return 0;
	}

	bool MtLuaCallProxy::Registed(const std::string& funcname) const
	{
		return m_delegates.find(funcname) != m_delegates.end();
	}

	void MtLuaCallProxy::CallMsgHandler(MtPlayer* player, const std::string& funcname, google::protobuf::Message* msg)
	{
		for (auto& script_id : m_delegates[funcname]) {
			try	{
				thread_lua->call<void>(script_id, funcname.c_str(), player, msg);
			}
			catch (ff::lua_exception& e){
				LOG_WARN << e.what();
			}			
		}
	}


}