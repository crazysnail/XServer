#include "mt_config.h"
#include "module.h"
#include "log.h"
#include "../mt_server/mt_server.h"
#include "random_generator.h"
#include "utils.h"
#include <BaseConfig.pb.h>
#include "../data/data_manager.h"
#include "../mt_activity/mt_operative_config.h"
#include "../mt_cache/mt_shm_manager.h"
#include "../zxero/shm/share_memory.h"
#include <zlib.h>
#include <boost/format.hpp>
#include "utils.h"
#include <cstdlib>

namespace Mt {
	ServerGroupDB::ServerGroupDB():GroupID(0), GroupName(""), GroupUrl(""), ChatServerUrl(""),
		DBConnectionConfig(""), GroupState(2), GroupHeat(1), GroupLoad(1), OpenDate(""), GroupIp(""),
		MaintainBeginTime(""), MaintainMinutes(0), OfficialOrTest(0), GroupType(0), GroupOrder(0),
		MaintainMessage(""), GameListenAddr(""), CallbackListenAddr(""), ExternalCallbackUrl(""),
		InternalCallbackUrl("")
	{

	}
	void ServerGroupDB::JsonParse(std::string str)
	{
		try
		{
			Json::Reader reader;
			Json::Value root;
			if (reader.parse(str, root))
			{
				if (root.isMember("GroupID"))
				{
					GroupID = root["GroupID"].asInt();
				}
				if (root.isMember("GroupName"))
				{
					GroupName = root["GroupName"].asString();
				}
				if (root.isMember("GroupUrl"))
				{
					GroupUrl = root["GroupUrl"].asString();
				}
				if (root.isMember("ChatServerUrl"))
				{
					ChatServerUrl = root["ChatServerUrl"].asString();
				}
				if (root.isMember("DBConnectionConfig"))
				{
					DBConnectionConfig = root["DBConnectionConfig"].asString();
				}
				if (root.isMember("GroupState"))
				{
					GroupState = (int8)root["GroupState"].asInt();
				}
				if (root.isMember("GroupHeat"))
				{
					GroupHeat = (int8)root["GroupHeat"].asInt();
				}
				if (root.isMember("GroupLoad"))
				{
					GroupLoad = (int8)root["GroupLoad"].asInt();
				}
				if (root.isMember("OpenDate"))
				{
					OpenDate = root["OpenDate"].asString();					
				}
				if (root.isMember("GroupIp"))
				{
					GroupIp = root["GroupIp"].asString();
				}
				if (root.isMember("MaintainBeginTime"))
				{
					MaintainBeginTime = root["MaintainBeginTime"].asString();
				}
				if (root.isMember("MaintainMinutes"))
				{
					MaintainMinutes = root["MaintainMinutes"].asInt();
				}
				if (root.isMember("OfficialOrTest"))
				{
					GroupType = root["OfficialOrTest"].asInt();
				}
				if (root.isMember("GroupType"))
				{
					GroupType = root["GroupType"].asInt();
				}
				if (root.isMember("GroupOrder"))
				{
					GroupOrder = root["GroupOrder"].asInt();
				}
				if (root.isMember("MaintainMessage"))
				{
					MaintainMessage = root["MaintainMessage"].asString();
				}
				if (root.isMember("GameListenAddr"))
				{
					GameListenAddr = root["GameListenAddr"].asString();
				}
				if (root.isMember("CallbackListenAddr"))
				{
					CallbackListenAddr = root["CallbackListenAddr"].asString();
				}
				if (root.isMember("ExternalCallbackUrl"))
				{
					ExternalCallbackUrl = root["ExternalCallbackUrl"].asString();
				}
				if (root.isMember("InternalCallbackUrl"))
				{
					InternalCallbackUrl = root["InternalCallbackUrl"].asString();
				}
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "ActiveServerWork ServerGroupDB JsonParse error";
			EXIT();
		}
	}
	PartnerDB::PartnerDB() :PartnerID(0), PartnerName(""), PartnerAlias(""), AppID(""),
		LoginKey(""), ChargeConfig(""), OtherConfigInfo(""), GameVersionUrl("")
	{

	}
	void PartnerDB::JsonParse(Json::Value value)
	{
		try
		{
			if (value.isMember("PartnerID"))
			{
				PartnerID = value["PartnerID"].asInt();
			}
			if (value.isMember("PartnerName"))
			{
				PartnerName = value["PartnerName"].asString();
			}
			if (value.isMember("PartnerAlias"))
			{
				PartnerAlias = value["PartnerAlias"].asString();
			}
			if (value.isMember("AppID"))
			{
				AppID = value["AppID"].asString();
			}
			if (value.isMember("LoginKey"))
			{
				LoginKey = value["LoginKey"].asString();
			}
			if (value.isMember("ChargeConfig"))
			{
				ChargeConfig = value["ChargeConfig"].asString();
			}
			if (value.isMember("OtherConfigInfo"))
			{
				OtherConfigInfo = value["OtherConfigInfo"].asString();
			}
			if (value.isMember("GameVersionUrl"))
			{
				GameVersionUrl = value["GameVersionUrl"].asString();
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "ActiveServerWork PartnerDB JsonParse error";
			EXIT();
		}
	}
	ServerDB::ServerDB() :PartnerID(0), ServerID(0), ServerName(""), GroupID(0),
		GameVersionID(0), MinGameVersionID(0)
	{

	}
	void ServerDB::JsonParse(Json::Value value)
	{
		try
		{
			if (value.isMember("PartnerID"))
			{
				PartnerID = value["PartnerID"].asInt();
			}
			if (value.isMember("ServerID"))
			{
				ServerID = value["ServerID"].asInt();
			}
			if (value.isMember("ServerName"))
			{
				ServerName = value["ServerName"].asString();
			}
			if (value.isMember("GroupID"))
			{
				GroupID = value["GroupID"].asInt();
			}
			if (value.isMember("GameVersionID"))
			{
				GameVersionID = value["GameVersionID"].asInt();
			}
			if (value.isMember("MinGameVersionID"))
			{
				MinGameVersionID = value["MinGameVersionID"].asInt();
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "ActiveServerWork ServerDB JsonParse error";
			EXIT();
		}
	}
	ResourceVersionDB::ResourceVersionDB() :ResourceVersionID(0), PartnerIDs(""), GameVersionIDs(""), 
		ResourceVersionName(""),ResourceVersionUrl(""), StartTime(""), EndTime(""), IfAuditServiceDownload(0)
	{

	}
	void ResourceVersionDB::JsonParse(Json::Value value)
	{
		try
		{
			if (value.isMember("ResourceVersionID"))
			{
				ResourceVersionID = value["ResourceVersionID"].asInt();
			}
			if (value.isMember("PartnerIDs"))
			{
				PartnerIDs = value["PartnerIDs"].asString();
			}
			if (value.isMember("GameVersionIDs"))
			{
				GameVersionIDs = value["GameVersionIDs"].asString();
			}
			if (value.isMember("ResourceVersionName"))
			{
				ResourceVersionName = value["ResourceVersionName"].asString();
			}
			if (value.isMember("ResourceVersionUrl"))
			{
				ResourceVersionUrl = value["ResourceVersionUrl"].asString();
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "ActiveServerWork ResourceVersionDB JsonParse error";
			EXIT();
		}
	}
	using namespace zxero;
	static MtConfig* __mt_config = nullptr;
	REGISTER_MODULE(MtConfig)
	{		
		register_load_function(module_base::STAGE_PRE_LOAD, boost::bind(&MtConfig::OnLoad, boost::ref(MtConfig::Instance())));
	}
	void MtConfig::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtConfig, ctor()>(ls, "MtConfig")
			.def(&MtConfig::open_server_date, "open_server_date")
			.def(&MtConfig::set_open_server_date,"set_open_server_date")
			.def(&MtConfig::reset_open_server_date, "reset_open_server_date")
			.def(&MtConfig::EnableActive,"EnableActive")
			;

		ff::fflua_register_t<>(ls)
			.def(&MtConfig::Instance, "LuaMtConfig");
	}

	int MtConfig::OnLoad()
	{
		if (load_strategy_ == LoadStrategy::LOCAL_FILE) {
			server_id_		=   utils::get_ini_key_int("game_server", "server_id", server_config_file_.c_str());		
			server_name_	=	utils::get_ini_key_string("game_server", "name", server_config_file_.c_str());
			server_host_	=	utils::get_ini_key_string("game_server", "game_server_host", server_config_file_.c_str());
			server_port_	=	(uint16)utils::get_ini_key_int("game_server", "game_server_port", server_config_file_.c_str());
			log_host_		= utils::get_ini_key_string("log_server", "log_host", server_config_file_.c_str());
			log_port_		= (uint16)utils::get_ini_key_int("log_server", "log_port", server_config_file_.c_str());
			server_group_id_ = utils::get_ini_key_int("game_server", "group_id", server_config_file_.c_str());
			max_player_		= (int32)utils::get_ini_key_int("game_server", "max_player", server_config_file_.c_str());
			http_port_		= (uint16)utils::get_ini_key_int("game_server", "http_server_port", server_config_file_.c_str());
			serverstatus_	=	utils::get_ini_key_int("game_server", "server_status", server_config_file_.c_str());
			tag_gourp_		=	utils::get_ini_key_string("game_server", "tag_gourp", server_config_file_.c_str());
			world_chat_level_ = utils::get_ini_key_int("game_server", "world_chat_level", server_config_file_.c_str());
			url_thread_count_ = utils::get_ini_key_int("game_server", "url_thread_count", server_config_file_.c_str());
			db_host_		=	utils::get_ini_key_string("db_server", "db_host", server_config_file_.c_str());
			db_user_		=	utils::get_ini_key_string("db_server", "db_user", server_config_file_.c_str());
			db_pass_		=	utils::get_ini_key_string("db_server", "db_pass", server_config_file_.c_str());
			web_host_		=	utils::get_ini_key_string("master", "web_host", server_config_file_.c_str());
			open_server_date_ = utils::get_ini_key_int("game_server", "open_server_date", server_config_file_.c_str());
			gm_mode_ = utils::get_ini_key_int("game_server", "gm_mode", server_config_file_.c_str())==0?false:true;
			sm_block_size_= utils::get_ini_key_int("game_server", "sm_block_size", server_config_file_.c_str());
			mc_host_		=	utils::get_ini_key_string("mq_config", "mc_host", server_config_file_.c_str());;
			login_host_		=	utils::get_ini_key_string("mq_config", "login_host", server_config_file_.c_str());;
			genorderid_host_ = utils::get_ini_key_string("mq_config", "genorderid_host", server_config_file_.c_str());;
			if (std::getenv("SERVER_ID"))
				server_id_ = boost::lexical_cast<int32>(std::getenv("SERVER_ID"));
			if (std::getenv("GROUP_ID"))
				server_group_id_ = boost::lexical_cast<int32>(std::getenv("GROUP_ID"));
			if (std::getenv("DB_HOST"))
				db_host_ = boost::lexical_cast<std::string>(std::getenv("DB_HOST"));
			if (std::getenv("DB_USER"))
				db_user_ = boost::lexical_cast<std::string>(std::getenv("DB_USER"));
			if (std::getenv("DB_PASS"))
				db_pass_ = boost::lexical_cast<std::string>(std::getenv("DB_PASS"));
			gm_mode = gm_mode_;
			db_name_ = (boost::format("mt_game_%d") % (int32)server_id_).str();
		}
		else {
			ZXERO_ASSERT(false) << "load config file from coordinator not implemntd";
		}
		
		//seed todo
		// bool ret = ServerActive();
		// if (ret)
		// {
		// 	LOG_INFO << "ServerActive True";
		// 	Server::Instance().ServerInit();
		// }
		// else
		// {
		// 	LOG_INFO << "ServerActive False";
		// 	EXIT();
		// }
		// ret = EnableActive(true);
		// if (!ret)
		// {
		// 	LOG_INFO << "EnableActive False";
		// 	if (!gm_mode_)
		// 	{
		// 		EXIT();
		// 	}
		// }
		Server::Instance().ServerInit();

		MtShmManager::Instance().ShmInit();

		return 0;
	}
	bool MtConfig::ServerActive()
	{
		try
		{
			boost::shared_ptr<urlcontext> ctx = boost::make_shared<urlcontext>();
			ctx->set_url(mc_host_);// + "cdplay/exchange_code/require"
			ctx->add_param("ServerGroupID", server_group_id_);
			ctx->request();
			int32 ret = -99;
			auto rcode = ctx->get_response_code();
			if (rcode == 200)
			{
				std::string result = ctx->get_data();
				int32 sourceLen = result.length();
				Bytef content[1024 * 100] = { 0 };
				uLong destlen = 1024 * 100;
				// Ω‚—πÀı
				int32 zlibret = uncompress(content, &destlen, (Bytef*)result.c_str(), sourceLen);
				if (zlibret == Z_OK)
				{
					Json::Reader reader;
					Json::Value root;
					if (reader.parse((char*)content, (char*)(content + destlen), root))
					{
						if (!root.isMember("Code"))
						{
							ZXERO_ASSERT(false) << "ServerActive:ActiveServerWork no Code";
							return false;
						}
						ret = root["Code"].asInt();
						if (ret != 0)
						{
							std::string message = "no message";
							if (root.isMember("Message"))
							{
								message = root["Message"].asString();
							}
							ZXERO_ASSERT(false) << "ServerActive:ActiveServerWork fail code=" << ret << " message=" << message;
							return false;
						}
						else
						{
							//Ω‚ŒˆData
							if (root.isMember("Data"))
							{
								std::string datastr = root["Data"].asString();
								Json::Reader datareader;
								Json::Value dataroot;
								if (datareader.parse(datastr, dataroot))
								{
									if (dataroot.isMember("ServerGroupInfo"))
									{
										std::string ServerGroupInfostr = dataroot["ServerGroupInfo"].asString();
										serverGroupDB_.JsonParse(ServerGroupInfostr);
										LOG_INFO << "ServerActive:ServerGroupDB GroupID=" << serverGroupDB_.GroupID << ",OpenDate=" << serverGroupDB_.OpenDate << ",GameListenAddr=" << serverGroupDB_.GameListenAddr << ",CallbackListenAddr=" << serverGroupDB_.CallbackListenAddr << ",ExternalCallbackUrl=" << serverGroupDB_.ExternalCallbackUrl << ",InternalCallbackUrl=" << serverGroupDB_.InternalCallbackUrl;
										//"2017-10-30T15:48:50"
										if (!ConvertOpenServerDate()) {
											ZXERO_ASSERT(false) << "ServerActive:open_server_date convert failed!";
										}	

									}
									if (dataroot.isMember("PartnerList"))
									{
										std::string PartnerListstr = dataroot["PartnerList"].asString();
										Json::Reader Listreader;
										Json::Value Listroot;
										partnerList_.clear();
										if (Listreader.parse(PartnerListstr, Listroot))
										{
											for (int32 i=0;i<(int32)Listroot.size();i++)
											{
												PartnerDB p;
												p.JsonParse(Listroot[i]);
												partnerList_.push_back(p);
											}
										}
										LOG_INFO << "ServerActive:PartnerDB Begin";
										for (auto r : partnerList_)
										{
											LOG_INFO << "ServerActive:PartnerID=" << r.PartnerID << ",PartnerName=" << r.PartnerName << ",PartnerAlias=" << r.PartnerAlias << ",AppID=" << r.AppID << ",LoginKey=" << r.LoginKey << ",ChargeConfig=" << r.ChargeConfig << ",OtherConfigInfo=" << r.OtherConfigInfo << ",GameVersionUrl=" << r.GameVersionUrl;
										}
										LOG_INFO << "ServerActive:PartnerDB End";
									}
									if (dataroot.isMember("ServerList"))
									{
										std::string ServerListstr = dataroot["ServerList"].asString();
										Json::Reader Listreader;
										Json::Value Listroot;
										serverList_.clear();
										if (Listreader.parse(ServerListstr, Listroot))
										{
											for (int32 i = 0; i < (int32)Listroot.size(); i++)
											{
												ServerDB p;
												p.JsonParse(Listroot[i]);
												serverList_.push_back(p);
											}
										}
										LOG_INFO << "ServerActive:ServerDB Begin";
										for (auto s : serverList_)
										{
											LOG_INFO << "ServerActive:PartnerID=" << s.PartnerID << ",ServerID=" << s.ServerID << ",ServerName=" << s.ServerName << ",GroupID=" << s.GroupID << ",GameVersionID=" << s.GameVersionID << ",MinGameVersionID=" << s.MinGameVersionID;
										}
										LOG_INFO << "ServerActive:ServerDB End";
									}
									if (dataroot.isMember("ResourceVersionList"))
									{
										std::string ResourceVersionListstr = dataroot["ResourceVersionList"].asString();
										Json::Reader Listreader;
										Json::Value Listroot;
										resourceVersionList_.clear();
										if (Listreader.parse(ResourceVersionListstr, Listroot))
										{
											for (int32 i = 0; i < (int32)Listroot.size(); i++)
											{
												ResourceVersionDB p;
												p.JsonParse(Listroot[i]);
												resourceVersionList_.push_back(p);
											}
										}
										LOG_INFO << "ServerActive:ResourceVersionDB Begin";
										for (auto r : resourceVersionList_)
										{
											LOG_INFO << "ServerActive:ResourceVersionID=" << r.ResourceVersionID << ",PartnerIDs=" << r.PartnerIDs << ",GameVersionIDs=" << r.GameVersionIDs << ",ResourceVersionName=" << r.ResourceVersionName << ",ResourceVersionUrl=" << r.ResourceVersionUrl << ",StartTime=" << r.StartTime << ",EndTime=" << r.EndTime << ",IfAuditServiceDownload=" << r.IfAuditServiceDownload;
										}
										LOG_INFO << "ServerActive:ResourceVersionDB End";
									}
								}
							}
						}
					}
				}
				else
				{
					ZXERO_ASSERT(false) << "ServerActive:ActiveServerWork zlibret =" << zlibret <<" str="<< result;
					return false;
				}
			}


			if (ret == 0 || MtConfig::Instance().gm_mode_ == true)
			{
				return true;
			}
			else
			{
				ZXERO_ASSERT(false) << "active server fail";
				return false;
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "ActiveServerWork error";
			return false;
		}
	}

	bool MtConfig::EnableActive( bool init )
	{
		try
		{
 			boost::shared_ptr<urlcontext> ctx = boost::make_shared<urlcontext>();
			ctx->set_url(MtConfig::Instance().web_host_ + "/mt_web/get_activity.php");// + "cdplay/exchange_code/require"
			ctx->add_param("ServerGroupID", server_id_);
			ctx->request();
			auto rcode = ctx->get_response_code();
			if (rcode == 200)
			{
				std::string result = ctx->get_data();
				Json::Reader reader;
				Json::Value root;
				if (reader.parse(result.c_str(), root))
				{
					if (root.isMember("Code"))
					{
						int32 ret_ = root["Code"].asInt();
						if (ret_ == 0)
						{
							std::map<int32, std::string> activitymap;
							std::map<int32, std::string> delay_activitymap;
							if (root.isMember("activitylist"))
							{
								if (root["activitylist"].isArray())
								{
									int32 acticitysize = root["activitylist"].size();
									for (int32 i=0;i<acticitysize;i++)
									{
									  if (root["activitylist"][i].isMember("id"))
									  {
										  int32 activityid = root["activitylist"][i]["id"].asInt();
										  if (root["activitylist"][i].isMember("activity"))
										  {
											  std::string activitystr = root["activitylist"][i]["activity"].asString();
											  Json::Value acr_root;
											  Json::Reader acr_reader;
											  if (acr_reader.parse(activitystr.c_str(), acr_root)) {
											  	 if (acr_root.isMember("Id")) {
													 string id = acr_root["Id"].asString();
													 int32 serial_id = atoi(id.c_str());
											  		 activitymap.insert({ serial_id ,activitystr });
											  	 }
												 else {
													 ZXERO_ASSERT(false) << "EnableActive json data error! script id =" << activityid << " content=" << activitystr;
												 }
											  }								
										  }
									  }
									}
								}
								else
								{
									return false;
								}
							}
							else
							{
								return false;
							}
							MtOperativeConfigManager::Instance().UpdateOriginJsons(activitymap);
							return true;
						}
					}
				}
			}
			LOG_INFO << "EnableActive : "<< init <<","<< rcode;
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "EnableActiveWork error";
			return false;
		}
		return false;
	}

	bool MtConfig::ConvertOpenServerDate() {
		if (serverGroupDB_.OpenDate == "") {
			return false;
		}
		else {
			std::vector<std::string> params;
			std::vector<int32> dates;
			zxero::utils::trim_string(serverGroupDB_.OpenDate);
			zxero::utils::split_string(serverGroupDB_.OpenDate.c_str(), "T", params);
			if (params.empty()) {
				return false;
			}
			else {
				zxero::utils::split_string(params[0].c_str(), "-", dates);
				if (dates.size() != 3) {
					return false;
				}
				else {
					open_server_date_ = dates[0] * 100 * 100 + dates[1] * 100 + dates[2];
				}
			}
		}
		return true;
	}
	boost::shared_ptr<zxero::dbcontext> MtConfig::CreateDbContext(bool has_db /*= true*/)
	{
		rand_gen.reset(new Mt::MT19937());
		if (has_db)
		{
			if (++dbcontext_count_ == db_save_thread_count_ + db_load_thread_count_) {
				Server::Instance().OnDbConnected();
			}
			return boost::make_shared<zxero::dbcontext>(db_host_, db_user_, db_pass_, db_name_);
		} else {
			return boost::make_shared<zxero::dbcontext>(db_host_, db_user_, db_pass_, "mysql");
		}
	}

	boost::shared_ptr<zxero::urlcontext> MtConfig::CreateUrlContext(work_thread*)
	{
		return boost::make_shared<zxero::urlcontext>();
	}

	const PartnerDB* MtConfig::GetPartnerItem(int32 partnerId)
	{
		for (auto& p : partnerList_)
		{
			if (p.PartnerID == partnerId)
			{
				return &p;
			}
		}
		return nullptr;
	}
	const ServerDB* MtConfig::GetServerItem(int32 partnerId,int32 servergroupId)
	{
		for (auto& p : serverList_)
		{
			if (p.ServerID == servergroupId && p.PartnerID == partnerId)
			{
				return &p;
			}
		}
		return nullptr;
	}
	MtConfig& MtConfig::Instance()
	{
		return *__mt_config;
	}

	MtConfig::MtConfig()
	{
		__mt_config = this;
		dbcontext_count_ = 0;
	}
	
	using namespace zxero;
	static MtConfigDataManager* __mt_config_manager = nullptr;
	REGISTER_MODULE(MtConfigDataManager)
	{
		require("data_manager");
		register_load_function(module_base::STAGE_LOAD, boost::bind(&MtConfigDataManager::OnLoad, boost::ref(MtConfigDataManager::Instance())));
	}

	void MtConfigDataManager::LoadConstConfigTB(bool reload)
	{
		const_config_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("const_config");
		}
		else {
			table = data_manager::instance()->get_table("const_config");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::ConstConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			const_config_.insert({ config->key(), config });
		}
	}

	void MtConfigDataManager::LoadFunctionConfigTB(bool reload)
	{
		function_config_.clear();
		data_table* table = nullptr;
		if (reload) {
			table = data_manager::instance()->on_reload_file("functions");
		}
		else {
			table = data_manager::instance()->get_table("functions");
		}
		for (auto i = 0; i < table->get_rows(); ++i)
		{
			auto config = boost::make_shared<Config::FunctionConfig>();
			auto row = table->find_row(i);
			FillMessageByRow(*config, *row);
			function_config_.insert({ config->name(), config });
		}
	}

	int MtConfigDataManager::OnLoad()
	{
		LoadConstConfigTB();
		LoadFunctionConfigTB();
		return 0;
	}
	
	const boost::shared_ptr<Config::ConstConfig> MtConfigDataManager::FindConfigValue(const std::string & key)
	{
		if (const_config_.find(key) != const_config_.end())
		{
			return const_config_[key];
		}

		ZXERO_ASSERT(false) << "MtConfigDataManager::FindConfigValue failed! key=" << key;
		return nullptr;
	}

	const boost::shared_ptr<Config::FunctionConfig> MtConfigDataManager::FindFunction(const std::string & name)
	{
		if (function_config_.find(name) != function_config_.end())
		{
			return function_config_[name];
		}

		ZXERO_ASSERT(false) << "MtConfigDataManager::FindFunction failed! key=" << name;
		return nullptr;
	}
	
	void MtConfigDataManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtConfigDataManager, ctor()>(ls, "MtConfigDataManager")
			.def(&MtConfigDataManager::FindConfigValue, "FindConfigValue")
			.def(&MtConfigDataManager::LoadConstConfigTB,"LoadConstConfigTB")
			.def(&MtConfigDataManager::FindFunction, "FindFunction")
			.def(&MtConfigDataManager::LoadFunctionConfigTB, "LoadFunctionConfigTB")

			;
		ff::fflua_register_t<>(ls)
			.def(&MtConfigDataManager::Instance, "LuaMtConfigDataManager");
	}
	MtConfigDataManager& MtConfigDataManager::Instance()
	{
		return *__mt_config_manager;
	}

	MtConfigDataManager::MtConfigDataManager()
	{
		__mt_config_manager = this;
	}
}