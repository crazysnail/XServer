#ifndef ZXERO_GAMESERVER_MT_CONFIG_H__
#define ZXERO_GAMESERVER_MT_CONFIG_H__

#include "../mt_types.h"
#include "dbcontext.h"
#include "urlcontext.h"
#include "json/json.h"
#include <atomic>
#include <string>

namespace Mt {

	class ServerGroupDB
	{
	public:
		ServerGroupDB();
		~ServerGroupDB() {}
		void JsonParse(std::string str);
	public:
		int32 GroupID;						// 服务器组唯一标识
		std::string GroupName;				// 服务器组名称
		std::string GroupUrl;				// 服务器组Url，提供给GameClient，供连接GameServer使用
		std::string ChatServerUrl;			// 聊天服务器Url，供连接聊天服务器使用
		std::string DBConnectionConfig;		// 数据库连接配置
		int8 GroupState;					// 服务器组状态（1：正常；2：维护）
		int8 GroupHeat;						// 服务器组热度（1：正常，2：新服；3：推荐；4：热门）
		int8 GroupLoad;						// 服务器组负载（1：正常；2：火爆）
		std::string  OpenDate;				// 服务器组开服日期
		std::string GroupIp;				// 服务器IP，用于ManageCenter Server控制是否允许GameServer访问(外网IP;内网IP;回调GS内网端口)
		std::string MaintainBeginTime;		// 服务器组维护开始时间
		uint32 MaintainMinutes;				// 服务器组维护持续时间（单位：分钟）
		int8 OfficialOrTest;				// 正式服或测试服，1：正式服，2：测试服
		int32 GroupType;					// 服务器组类型
		int32 GroupOrder;					// 服务器组排序
		std::string MaintainMessage;		// 维护提示信息
		std::string GameListenAddr;			// 游戏监听地址
		std::string CallbackListenAddr;		// 回调监听地址
		std::string ExternalCallbackUrl;	// 外网回调地址
		std::string InternalCallbackUrl;	// 内网回调地址
	};

	class PartnerDB
	{
	public:
		PartnerDB();
		~PartnerDB() {}
		void JsonParse(Json::Value value);
	public:
		int32 PartnerID;				// CP为合作平台分配的唯一编号
		std::string PartnerName;		// CP为平台分配的唯一名称
		std::string PartnerAlias;		// CP为平台分配的名称的唯一别名
		std::string AppID;				// 平台为CP应用分配的唯一编号，在某些平台也称为GameID等名称
		std::string LoginKey;			// 登陆加密字符串，用于登陆平台时进行加密验证
		std::string ChargeConfig;		// 充值配置信息（JSON字符串）
		std::string OtherConfigInfo;	// 其它配置信息（JSON字符串）
		std::string GameVersionUrl;		// 游戏版本地址，主要用于AppStore上版本的自动升级
	};

	class ServerDB
	{
	public:
		ServerDB();
		~ServerDB() {}
		void JsonParse(Json::Value value);
	public:
		int32 PartnerID;			// CP服务器所从属的合作商
		int32 ServerID;				// 服务器编号，为1~999范围内的数字
		std::string ServerName;		// 服务器名称
		int32 GroupID;				// 服务器组唯一标识
		int32 GameVersionID;		// 平台的游戏版本唯一标识
		int32 MinGameVersionID;		// 平台所能接受的最低游戏版本唯一标识
	};

	class ResourceVersionDB
	{
	public:
		ResourceVersionDB();
		~ResourceVersionDB() {}
		void JsonParse(Json::Value value);
	public:
		int32 ResourceVersionID;			// 资源版本唯一标识
		std::string PartnerIDs;				// 资源所属的合作商ID集体
		std::string GameVersionIDs;			// 资源所属的游戏版本ID集体
		std::string ResourceVersionName;	// 资源版本名称
		std::string ResourceVersionUrl;		// 资源版本的url地址
		std::string StartTime;				// 资源生效时间
		std::string EndTime;				// 资源失效时间
		int32 IfAuditServiceDownload;		// 是否审核服下载
	};

	class MtConfig : public boost::noncopyable{
	public:
		static void lua_reg(lua_State* ls);
	public:
		int32 open_server_date()
		{
			return open_server_date_;
		}
		void set_open_server_date(int32 date)
		{
			open_server_date_ = date;
		}
		void reset_open_server_date()
		{
			ConvertOpenServerDate();
		}
	public:
		int OnLoad();
		MtConfig();
		bool ServerActive();
		bool EnableActive( bool init );
		bool ConvertOpenServerDate();
		const PartnerDB* GetPartnerItem(int32 partnerId);
		const ServerDB* GetServerItem(int32 partnerId,int32 servergroupId);

		int32 server_id_;
		int32 server_group_id_;
		std::string server_name_;
		std::string server_host_;
		zxero::uint16 server_port_;
		int32 max_player_;
		zxero::uint16 http_port_;
		int32 serverstatus_;
		std::string  tag_gourp_;

		std::string log_host_;
		zxero::uint16 log_port_;

		std::string db_host_;
		std::string db_user_;
		std::string db_pass_;
		std::string db_name_;

		std::string web_host_;

		std::string mc_host_;
		std::string login_host_;
		std::string genorderid_host_;

		std::string conf_data_dir_ = "../../db_config/";
		std::string conf_json_dir_ = "../../json/";
		std::string script_data_dir_ = "../../lua_script/";
		zxero::int32 tcp_client_thread_count_ = 2;
		zxero::int32 db_save_thread_count_ = 5;
		zxero::int32 db_load_thread_count_ = 2;
		zxero::int32 url_thread_count_ = 2;
		int32 world_chat_level_ = 10;
		std::string server_version_ = "0.0.1";
		boost::shared_ptr<dbcontext> CreateDbContext(bool has_db = true);
		boost::shared_ptr<urlcontext> CreateUrlContext(work_thread*);
		//boost::shared_ptr<urlcontext> CreateUrlContext(work_thread*);
		static MtConfig& Instance();
		uint32 max_actor_count = 41;
		bool disable_create_actor_ = false;		
		bool output_scene_tick_info_ = false;
		int32 open_server_date_;
		int32 sm_block_size_ = 512;
		bool gm_mode_ = false;


		ServerGroupDB		serverGroupDB_;			//服务器组对象
		std::vector<PartnerDB>			partnerList_;				//合作商对象
		std::vector<ServerDB>			serverList_;				//服务器对象
		std::vector<ResourceVersionDB>	resourceVersionList_;		//资源版本对象
	private:
		enum LoadStrategy {
			LOCAL_FILE,
			COORDINATOR,
		};
		std::string server_config_file_ = "./server_config.conf";
		LoadStrategy load_strategy_ = LoadStrategy::LOCAL_FILE;
		std::atomic<int32> dbcontext_count_;

	};


	class MtConfigDataManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtConfigDataManager& Instance();
		MtConfigDataManager();
		zxero::int32 OnLoad();
		const boost::shared_ptr<Config::ConstConfig> FindConfigValue(const std::string & key);
		const boost::shared_ptr<Config::FunctionConfig> FindFunction(const std::string & name);
		void LoadConstConfigTB(bool reload = false);
		void LoadFunctionConfigTB(bool reload = false);
	private:
		std::map<std::string, boost::shared_ptr<Config::ConstConfig>> const_config_;
		std::map<std::string, boost::shared_ptr<Config::FunctionConfig>> function_config_;
		
	};
}
#endif // ZXERO_GAMESERVER_MT_CONFIG_H__