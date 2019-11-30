#include "module.h"
#include "app.h"
#include "mt_operative_config.h"
#include "../data/data_manager.h"
#include "../mt_server/mt_server.h"
#include "Base.pb.h"
#include "json/json.h"
//#include <zlib.h>
//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"
//------------------------------------

namespace Mt
{

	OperativeInstance::OperativeInstance()
	{
		m_config = boost::make_shared<Packet::OperativeConfig>();
		m_switch_open    		 = 0 ;
		m_serial				 =-1 ;
		m_common				 = 0 ;
		m_script_id				 =-1 ;
		m_show_begin_date		 =-1 ;
		m_show_end_date			 =-1 ;
		m_show_begin_days		 =-1 ;
		m_show_end_days			 =-1 ;
		m_show_begin_time		 =-1 ;
		m_show_end_time			 =-1 ;
		m_normal_begin_date		 =-1 ;
		m_normal_end_date		 =-1 ;
		m_normal_begin_days		 =-1 ;
		m_normal_end_days		 =-1 ;
		m_normal_begin_time		 =-1 ;
		m_normal_end_time		 =-1 ;
	}


	void OperativeInstance::lua_reg(lua_State* state)
	{
		ff::fflua_register_t<OperativeInstance, ctor()>(state, "OperativeInstance")
			.def(&OperativeInstance::m_switch_open, "switch_open")
			.def(&OperativeInstance::m_serial, "serial")
			.def(&OperativeInstance::m_common, "common")
			.def(&OperativeInstance::m_script_id, "script_id")
			.def(&OperativeInstance::m_show_begin_date, "show_begin_date")
			.def(&OperativeInstance::m_show_end_date, "show_end_date")
			.def(&OperativeInstance::m_show_begin_days, "show_begin_days")
			.def(&OperativeInstance::m_show_end_days, "show_end_days")
			.def(&OperativeInstance::m_show_begin_time, "show_begin_time")
			.def(&OperativeInstance::m_show_end_time, "show_end_time")
			.def(&OperativeInstance::m_normal_begin_date, "normal_begin_date")
			.def(&OperativeInstance::m_normal_end_date, "normal_end_date")
			.def(&OperativeInstance::m_normal_begin_days, "normal_begin_days")
			.def(&OperativeInstance::m_normal_end_days, "normal_end_days")
			.def(&OperativeInstance::m_normal_begin_time, "normal_begin_time")
			.def(&OperativeInstance::m_normal_end_time, "normal_end_time")
			;
	}

	/////////////////////////////////////////////
	
	static MtOperativeConfigManager* __mt_operative_config_mamager = nullptr;

	void MtOperativeConfigManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtOperativeConfigManager, ctor()>(ls, "MtOperativeConfigManager")
			.def(&MtOperativeConfigManager::FindConfig,"FindConfig")
			.def(&MtOperativeConfigManager::GetConfigs,"GetConfigs")
			.def(&MtOperativeConfigManager::FlushCacheConfig, "FlushCacheConfig")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtOperativeConfigManager::Instance, "LuaOperativeConfigManager");
	}

	REGISTER_MODULE(MtOperativeConfigManager)
	{
		require("data_manager");
		require("MtTimerManager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtOperativeConfigManager::OnLoad, __mt_operative_config_mamager));
	}

	MtOperativeConfigManager::MtOperativeConfigManager()
	{
		__mt_operative_config_mamager = this;
	}

	MtOperativeConfigManager::~MtOperativeConfigManager()
	{
		activity_configs_.clear();

		for (auto child : delay_activity_configs_)
		{
			SAFE_DELETE(child.second);
		}
		delay_activity_configs_.clear();

		origin_jsons_.clear();
	}
	
	MtOperativeConfigManager& MtOperativeConfigManager::Instance()
	{
		return *__mt_operative_config_mamager;
	}

	int32 MtOperativeConfigManager::OnLoad()
	{
		return 0;
	}


	OperativeInstance* MtOperativeConfigManager::Decode(const std::string &json)
	{		
		try {
			std::map<string, int32> params = thread_lua->call<std::map<string, int32>>(11, "GOperateJsonDecodeToC", json);
			OperativeInstance* instance = SAFE_NEW OperativeInstance();
			if (instance != nullptr) {		

				instance->m_switch_open			= params["switch_open"];
				instance->m_serial				= params["serial"];
				instance->m_common				= params["common"];
				instance->m_script_id			= params["script_id"];

				instance->m_show_begin_date		= params["show_begin_date"];
				instance->m_show_end_date		= params["show_end_date"];
				instance->m_show_begin_days		= params["show_begin_days"];
				instance->m_show_end_days		= params["show_end_days"];
				instance->m_show_begin_time		= params["show_begin_time"];
				instance->m_show_end_time		= params["show_end_time"];

				instance->m_normal_begin_date	= params["normal_begin_date"];
				instance->m_normal_end_date		= params["normal_end_date"];
				instance->m_normal_begin_days	= params["normal_begin_days"];
				instance->m_normal_end_days		= params["normal_end_days"];
				instance->m_normal_begin_time	= params["normal_begin_time"];
				instance->m_normal_end_time		= params["normal_end_time"];

				instance->m_config->set_script_id(instance->m_script_id);
				instance->m_config->set_content(json);

				return instance;
			}
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << "LuaScriptException OnLuaFunCall_x_Server error!" << e.what();
			return nullptr;
		}

		return nullptr;
	}

	bool MtOperativeConfigManager::Check(const OperativeInstance* instance)
	{
		if (instance == nullptr) {
			return false;
		}

		try {
			return thread_lua->call<bool>(11, "GOperateCheckShowToC", instance);
		}
		catch (ff::lua_exception& e) {
			LOG_ERROR << "LuaScriptException OnLuaFunCall_x_Server error!" << e.what();
			return false;
		}

	}

	void MtOperativeConfigManager::UpdateOriginJsons(const std::map<int32, std::string>& json_set)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		for (auto child : json_set) {
			origin_jsons_[child.first] = child.second;
		}

		LOG_INFO << "UpdateOriginJsons ok!";
	}

	void MtOperativeConfigManager::UpdateCurrentData(const std::map<int32, std::string>& json_set )
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		for (auto child : json_set) {
			auto iter = activity_configs_.find(child.first);
			if (iter != activity_configs_.end()) {
				iter->second->set_content(child.second);
			}
			else {
				Packet::OperativeConfig* config = SAFE_NEW Packet::OperativeConfig();
				if (config != nullptr) {
					config->set_content(child.second);
					config->set_script_id(child.first);

					//注意，活动缓存里的scriptid是唯一的，如果配置了多个相同scriptid的活动(流水号不一样)，只会启用最新的同一个scriptiud的活动
					//如果有同时开启多个相同规则的活动，每个活动需要新增scriptid!
					activity_configs_.insert({ child.first,config });
				}
			}
		}

		Server::Instance().GetLuaCallProxy()->OnLuaFunCall_n_Server(&Server::Instance(), 5, "OnUpdateAcrConfig",
		{
			{ "script_id", -1 }
		});

	}

	void MtOperativeConfigManager::FlushCacheConfig()
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);
		
		for (auto child : origin_jsons_) {
			auto iter = delay_activity_configs_.find(child.first);
			if (iter != delay_activity_configs_.end()) {
				auto instance = iter->second;
				instance->m_config->set_content(child.second);
			}
			else {
				auto instance = Decode(child.second);
				if (instance != nullptr) {
					delay_activity_configs_.insert({ child.first,instance });
				}
			}
		}

		//根据时间进行运行时开启活动队列更新
		std::map<int32, std::string> open_set;
		for (auto child : delay_activity_configs_) {
			if (Check(child.second)) {
				auto script_id = child.second->m_config->script_id();
				open_set.insert({ script_id, child.second->m_config->content() });
			}
		}
		UpdateCurrentData(open_set);

		LOG_INFO << "FlushDelayConfig ok!";
	}

	Packet::OperativeConfig* MtOperativeConfigManager::FindConfig(int32 activity_id)
	{
		std::lock_guard<std::recursive_mutex> lock(mutex_);

		auto iter = activity_configs_.find(activity_id);
		if (iter != activity_configs_.end()) {
			return iter->second;
		}

		return nullptr;
	}

	const std::map<int32,Packet::OperativeConfig *>& MtOperativeConfigManager::GetConfigs()
	{
		return activity_configs_;
	}

}
