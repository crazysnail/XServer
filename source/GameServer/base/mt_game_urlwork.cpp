#include "client_session.h"
#include "mt_db_load_work.h"
#include "../mt_server/mt_server.h"
#include "../mt_config/mt_config.h"
#include "../mt_player/mt_player.h"
#include "utils.h"
#include "mt_game_urlwork.h"
#include "json/json.h"

namespace Mt
{
	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_URLWORK(ActiveServerWork)
	{
		try
		{
			ctx->set_url("http://managecentertest-mt.moqiplayer.com/API/ServerActivate.ashx");// + "cdplay/exchange_code/require"
			ctx->add_param("ServerGroupID", MtConfig::Instance().server_group_id_);
			ctx->request();
			auto rcode = ctx->get_response_code();
			if (rcode == 200)
			{
				result_ = ctx->get_data();
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "ActiveServerWork error";
		}
	}

	void ActiveServerWork::done()
	{
		
	}

	ActiveServerWork::ActiveServerWork():result_("")
	{

	}

	IMPLEMENT_URLWORK(serverstatus_require_work)
	{
		try
		{
			ctx->set_url(MtConfig::Instance().web_host_ + "/mt_web/update_server.php");// + "cdplay/exchange_code/require"
			ctx->add_param("sql", sql_);
			ctx->request();
			auto rcode = ctx->get_response_code();
			if (rcode == 200)
			{

			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "serverstatus_require_work error";
		}
	}

	void serverstatus_require_work::done()
	{
		
	}

	serverstatus_require_work::serverstatus_require_work(std::string sql)
		: sql_(sql)
	{

	}

	//////////////////////////////////////////////////////////////////////////
	IMPLEMENT_URLWORK(login_log_require_work)
	{
		try
		{
			LOG_DB(file_, line_) << log_;
			ctx->set_url(MtConfig::Instance().web_host_ + "/mt_web/login_log.php");// + "cdplay/exchange_code/require"
			ctx->add_param("log", log_);
			ctx->request();
			auto rcode = ctx->get_response_code();
			if (rcode == 200)
			{

			}

		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "login_log_require_work error";
		}
	}

	void login_log_require_work::done()
	{

	}

	login_log_require_work::login_log_require_work(const std::string& log, const log_message::source_file& file, int32 line)
		:log_(log)
		, file_(file)
		, line_(line)
	{

	}

	IMPLEMENT_URLWORK(db_log_require_work)
	{
		try
		{
			LOG_DB(file_, line_) << log_;
			ctx->set_url(MtConfig::Instance().web_host_ + "/mt_web/db_log.php");// + "cdplay/exchange_code/require"
			ctx->add_param("log", log_);
			ctx->request();
			auto rcode = ctx->get_response_code();
			if (rcode == 200)
			{

			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "db_log_require_work error ";
		}
	}

	void db_log_require_work::done()
	{

	}

	db_log_require_work::db_log_require_work(const std::string& log, const log_message::source_file& file, int32 line)
		:log_(log)
		, file_(file)
		, line_(line)
	{

	}

	IMPLEMENT_URLWORK(CheckCDKCODEWork)
	{
		try
		{
			ctx->set_url(MtConfig::Instance().web_host_ + "/mt_web/check_cdkcode.php");
			ctx->add_param("playerid", playerid_);
			ctx->add_param("partnerid", partnerid_);//
			ctx->add_param("code", code_);//
			ctx->add_param("serverid", Server::Instance().ServerId());
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
						ret_ = root["Code"].asInt();
						if (ret_ == 0)
						{
							for (int32 i = 0; i < 10; i++)
							{
								char Member[64] = { 0 };
								zprintf(Member, sizeof(Member), "item_id_%d", i);
								if (root.isMember(Member))
								{
									if (root[Member].isInt())
									{
										int32 itemid = root[Member].asInt();
										if (itemid > 0)
										{
											memset(Member, 0, sizeof(Member));
											zprintf(Member, sizeof(Member), "item_count_%d", i);
											if (root.isMember(Member))
											{
												if (root[Member].isInt())
												{
													int32 itemcount = root[Member].asInt();
													if (itemcount > 0)
													{
														item_map.insert({ itemid ,itemcount });
													}
												}
											}
										}
									}
								}
							}
						}
					}
					else
					{
						ZXERO_ASSERT(false) << "ActiveServerWork no Code";
					}
				}
			}
			else
			{
				LOG_ERROR << "get check_cdkcode error "<< rcode;
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "check_cdkcode error catch";
		}
	}

	CheckCDKCODEWork::CheckCDKCODEWork(uint64 playerid, int32 partnerid, std::string code)
		:playerid_(playerid), partnerid_(partnerid), code_(code)
	{
		ret_ = -1;
	}


	void CheckCDKCODEWork::done()
	{
		urlwork::done();

		auto player_ = Server::Instance().FindPlayer(playerid_);
		if (player_)
		{
			boost::shared_ptr<G2S::G2SCodeCheckDone> msg = boost::make_shared<G2S::G2SCodeCheckDone>();
			msg->set_ret(ret_);
			for(auto i : item_map )
			{
				auto item = msg->add_items();
				item->set_itemid(i.first);
				item->set_itemcount(i.second);
			}
			player_->ExecuteMessage(msg);
		}
		else
		{
			ZXERO_ASSERT(false) << "CheckCDKCODEWork done no player=" << playerid_ << ", code=" << code_;
		}
	}


	IMPLEMENT_URLWORK(AccountLoginWork)
	{
		try
		{
			std::vector<std::string> paramList;
			utils::split_string(message_->password().c_str(), "_", paramList);
			if (paramList.size() == 2)
			{
				ctx->set_url(MtConfig::Instance().login_host_);
				ctx->add_param("UserId", message_->userid());
				ctx->add_param("LoginKey", paramList[0]);//
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
							int32 ret = root["Code"].asInt();
							urlret = (ret == 0) ? true : false;
						}
						else
						{
							ZXERO_ASSERT(false) << "ActiveServerWork no Code";
						}
					}
				}
				else
				{
					LOG_ERROR << "";
				}
			}
			else
			{
				LOG_ERROR << "AccountLoginWork loginInfo erorr UserID="<< message_->userid() <<" PartnerID="<< message_->partnerid() <<" loginInfo="<< message_->password();
			}
			
			urlret = true;
		}
		catch (...)
		{
			//ZXERO_ASSERT(false) << "AccountLoginWork error ";
		}
	}

	AccountLoginWork::AccountLoginWork(const boost::shared_ptr<client_session>& session,
		const boost::shared_ptr<Packet::LoginRequest>& message)
		:session_(session), message_(message)
	{
	}


	void AccountLoginWork::done()
	{
		urlwork::done();
		if (urlret == true)
		{
			Server::Instance().AddDbLoadTask(new AccountLoadWork(session_, message_));
			LOG_TRACE << "url ret ok " << message_->userid();
		}
		else//·µ»ØµÇÂ¼Ê§°Ü£¬ÇëÈ·ÈÏÕËºÅÃÜÂëÖ®ºóÖØÐÂµÇÂ¼
		{
			Packet::LoginReply reply_;
			reply_.set_result(Packet::LoginReply_Result_WRONG_PASSWORD);
			session_->send(reply_);
			LOG_ERROR << "url ret error "<<message_->userid();
		}
		
	}


	IMPLEMENT_URLWORK(GenerateOrderIDWork)
	{
		try
		{
			if (player_)
			{
				std::string paramnull = "null";
				std::string paramfalse = "false";
				ctx->set_url(MtConfig::Instance().genorderid_host_);
				ctx->add_param("ProductId", ProductId_);
				ctx->add_param("PartnerId", player_->GetAccount()->partnerid());//
				ctx->add_param("ServerId", MtConfig::Instance().serverGroupDB_.GroupID);//
				ctx->add_param("UserId", player_->GetAccount()->userid());//
				ctx->add_param("PlayerId", player_->Guid());//
				ctx->add_param("MAC", paramnull);//
				ctx->add_param("IDFA", player_->GetAccount()->udid());//
				ctx->add_param("IP", player_->GetIP());//
				ctx->add_param("IMEI", paramnull);//
				ctx->add_param("Extra", Extra_);//
				ctx->add_param("IsMonthCard", paramfalse);//
				ctx->request();
				auto rcode = ctx->get_response_code();
				if (rcode == 200)
				{
					orderid_ = ctx->get_data();
				}
				else
				{
					LOG_ERROR << "GenerateOrderIDWork Code Null "<< rcode;
				}
			}
			else
			{
				ZXERO_ASSERT(false) << "GenerateOrderIDWork PLAYER ==null ";
			}
		}
		catch (...)
		{
			ZXERO_ASSERT(false) << "GenerateOrderIDWork error ";
		}
	}

	GenerateOrderIDWork::GenerateOrderIDWork(const boost::shared_ptr<MtPlayer>& player,std::string ProductId, std::string Extra)
		:player_(player), ProductId_(ProductId), Extra_(Extra), orderid_("")
	{
	}

	void GenerateOrderIDWork::done()
	{
		urlwork::done();
		if (player_)
		{
			if (orderid_ != "")
			{
				player_->SendLuaPacket("ChargeOderIdReply", {0}, {}, { orderid_,ProductId_ ,Extra_});
				LOG_INFO << "ChargeOderIdReply,"<< player_->Guid()<<","<< orderid_ << "," << ProductId_ << "," << Extra_;
				return;
			}
			else
			{
				ZXERO_ASSERT(false) << "GenerateOrderIDWork Get Order Null player="<< player_->Guid()<<",ProductId="<< ProductId_;
			}
		}
		else
		{
			ZXERO_ASSERT(false) << "GenerateOrderIDWork Get player null ProductId=" << ProductId_;
		}
		player_->SendLuaPacket("ChargeOderIdReply", {-1}, {}, { "","" ,"" });
	}
}