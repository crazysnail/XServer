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
		int32 GroupID;						// ��������Ψһ��ʶ
		std::string GroupName;				// ������������
		std::string GroupUrl;				// ��������Url���ṩ��GameClient��������GameServerʹ��
		std::string ChatServerUrl;			// ���������Url�����������������ʹ��
		std::string DBConnectionConfig;		// ���ݿ���������
		int8 GroupState;					// ��������״̬��1��������2��ά����
		int8 GroupHeat;						// ���������ȶȣ�1��������2���·���3���Ƽ���4�����ţ�
		int8 GroupLoad;						// �������鸺�أ�1��������2���𱬣�
		std::string  OpenDate;				// �������鿪������
		std::string GroupIp;				// ������IP������ManageCenter Server�����Ƿ�����GameServer����(����IP;����IP;�ص�GS�����˿�)
		std::string MaintainBeginTime;		// ��������ά����ʼʱ��
		uint32 MaintainMinutes;				// ��������ά������ʱ�䣨��λ�����ӣ�
		int8 OfficialOrTest;				// ��ʽ������Է���1����ʽ����2�����Է�
		int32 GroupType;					// ������������
		int32 GroupOrder;					// ������������
		std::string MaintainMessage;		// ά����ʾ��Ϣ
		std::string GameListenAddr;			// ��Ϸ������ַ
		std::string CallbackListenAddr;		// �ص�������ַ
		std::string ExternalCallbackUrl;	// �����ص���ַ
		std::string InternalCallbackUrl;	// �����ص���ַ
	};

	class PartnerDB
	{
	public:
		PartnerDB();
		~PartnerDB() {}
		void JsonParse(Json::Value value);
	public:
		int32 PartnerID;				// CPΪ����ƽ̨�����Ψһ���
		std::string PartnerName;		// CPΪƽ̨�����Ψһ����
		std::string PartnerAlias;		// CPΪƽ̨��������Ƶ�Ψһ����
		std::string AppID;				// ƽ̨ΪCPӦ�÷����Ψһ��ţ���ĳЩƽ̨Ҳ��ΪGameID������
		std::string LoginKey;			// ��½�����ַ��������ڵ�½ƽ̨ʱ���м�����֤
		std::string ChargeConfig;		// ��ֵ������Ϣ��JSON�ַ�����
		std::string OtherConfigInfo;	// ����������Ϣ��JSON�ַ�����
		std::string GameVersionUrl;		// ��Ϸ�汾��ַ����Ҫ����AppStore�ϰ汾���Զ�����
	};

	class ServerDB
	{
	public:
		ServerDB();
		~ServerDB() {}
		void JsonParse(Json::Value value);
	public:
		int32 PartnerID;			// CP�������������ĺ�����
		int32 ServerID;				// ��������ţ�Ϊ1~999��Χ�ڵ�����
		std::string ServerName;		// ����������
		int32 GroupID;				// ��������Ψһ��ʶ
		int32 GameVersionID;		// ƽ̨����Ϸ�汾Ψһ��ʶ
		int32 MinGameVersionID;		// ƽ̨���ܽ��ܵ������Ϸ�汾Ψһ��ʶ
	};

	class ResourceVersionDB
	{
	public:
		ResourceVersionDB();
		~ResourceVersionDB() {}
		void JsonParse(Json::Value value);
	public:
		int32 ResourceVersionID;			// ��Դ�汾Ψһ��ʶ
		std::string PartnerIDs;				// ��Դ�����ĺ�����ID����
		std::string GameVersionIDs;			// ��Դ��������Ϸ�汾ID����
		std::string ResourceVersionName;	// ��Դ�汾����
		std::string ResourceVersionUrl;		// ��Դ�汾��url��ַ
		std::string StartTime;				// ��Դ��Чʱ��
		std::string EndTime;				// ��ԴʧЧʱ��
		int32 IfAuditServiceDownload;		// �Ƿ���˷�����
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


		ServerGroupDB		serverGroupDB_;			//�����������
		std::vector<PartnerDB>			partnerList_;				//�����̶���
		std::vector<ServerDB>			serverList_;				//����������
		std::vector<ResourceVersionDB>	resourceVersionList_;		//��Դ�汾����
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