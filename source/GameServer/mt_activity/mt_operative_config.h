#ifndef MT_OPERATIVE_CONFIG_H__
#define MT_OPERATIVE_CONFIG_H__
#include "../mt_types.h"
#include "date_time.h"

namespace Mt
{

	struct OperativeInstance
	{
	public:
		static void lua_reg(lua_State* ls);

		OperativeInstance();
		~OperativeInstance() {};

	public:
		boost::shared_ptr<Packet::OperativeConfig> m_config;

		int32 m_switch_open;
		int32 m_serial;
		int32 m_common;
		int32 m_script_id;

		int32 m_show_begin_date;
		int32 m_show_end_date;
		int32 m_show_begin_days;
		int32 m_show_end_days;
		int32 m_show_begin_time;
		int32 m_show_end_time;

		int32 m_normal_begin_date;
		int32 m_normal_end_date;
		int32 m_normal_begin_days;
		int32 m_normal_end_days;
		int32 m_normal_begin_time;
		int32 m_normal_end_time;

	};
	class MtOperativeConfigManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtOperativeConfigManager& Instance();
		MtOperativeConfigManager();
		virtual ~MtOperativeConfigManager();
		int32 OnLoad();

		void UpdateOriginJsons(const std::map<int32, std::string>& json_set);
		void UpdateCurrentData(const std::map<int32, std::string>& json_set);
		void FlushCacheConfig();
		Packet::OperativeConfig* FindConfig(int32 activity_id);
		const std::map<int32, Packet::OperativeConfig*>& GetConfigs();

		OperativeInstance* Decode(const std::string &json);
		bool Check(const OperativeInstance* instance );

	private:
		std::map<int32, Packet::OperativeConfig*> activity_configs_;
		std::map<int32, OperativeInstance*> delay_activity_configs_;
		std::map<int32, std::string> origin_jsons_;
		std::recursive_mutex mutex_;
	};
}
#endif