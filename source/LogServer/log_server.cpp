//
//  account_manager.cpp
//  GameServer
//
//  Created by gg on 2018/2/20
//  Copyright (c) 2018年 gg. All rights reserved.
//

#include "log_server.h"
#include "module.h"
#include "app.h"
#include "tcp_server.h"
#include "inet_address.h"
#include "dbcontext.h"
#include "json/reader.h"
#include "cppconn/exception.h"

namespace game
{
	log_server* log_server::sg_log_server = nullptr;

    REGISTER_MODULE(log_server)
    {
        //  设置主线程工作函数
        app::set_main_work(boost::bind(&log_server::main, this));
        
        //
        register_load_function(STAGE_PRE_LOAD, boost::bind(&log_server::on_pre_load, this));
        register_unload_function(STAGE_POST_UNLOAD, boost::bind(&log_server::on_post_unload, this));
        
        ////  注册选项
        //register_option("ip", "ip for this server, default is \"0.0.0.0\"", program_options::value<string>()->default_value("0.0.0.0"), opt_delegate<string>(boost::bind(&account_manager::on_ip, this, _1)));
        //register_option("port", "port for this server, default is '9986'", program_options::value<int32>()->default_value(9982), opt_delegate<int32>(boost::bind(&account_manager::on_port, this, _1)));
		//register_option("db_ip", "ip to db server, default is \"127.0.0.1\"", program_options::value<string>()->default_value("127.0.0.1"), opt_delegate<string>(boost::bind(&account_manager::on_db_ip, this, _1)));
		//register_option("db_port", "port to db server, default is '3306'", program_options::value<int32>()->default_value(3306), opt_delegate<int32>(boost::bind(&account_manager::on_db_port, this, _1)));

	}
    
	log_server::log_server()
    : server_ip_("0.0.0.0")
    , server_port_(9982)
	, db_ip_("127.0.0.1")
	, db_port_(3306)
    , work_pool_(&main_loop_, "dbwork_pool")
   // , server_codec_(true, boost::bind(&account_manager::on_protobuf_message, this, _1, _2, _3))
   // , server_dispatcher_(&main_loop_, boost::bind(&account_manager::on_default_protobuf_message, this, _1, _2, _3))
    {
        sg_log_server = this;
    }
    
    void log_server::on_ip(const std::string &ip)
    {
        server_ip_ = ip;
    }
    
    void log_server::on_port(const int32 &port)
    {
        server_port_ = port;
    }

	void log_server::on_db_ip(const  std::string& ip)
	{
		db_ip_ = ip;
	}

	void log_server::on_db_port(const int32 &port)
	{
		db_port_ = port;
	}
    
    void log_server::main()
    {
		LOG_INFO << "start server at " << server_ip_ << ":" << server_port_;
		log_server_.reset(new tcp_server(&main_loop_, inet_address(server_ip_, server_port_), "log_server"));
		log_server_->connection_callback(boost::bind(&log_server::on_connection, this, _1));
		log_server_->message_callback(boost::bind(&log_server::on_message, this, _1, _2, _3));
		//log_server_->message_callback(boost::bind(&protobuf_codec::on_message, &server_codec_, _1, _2, _3));
		log_server_->start(1);

		//  定期激活，干啥呢，显示当前在线玩家情况
		main_loop_.run_every(seconds(10), boost::bind(&log_server::on_timer, this));


		//	启动数据库工作线程

		// 为方便调试，暂时只启用一个线程 [9/6/2014 seed]
		//work_pool_.start( 1, boost::bind(&log_server::create_db_context, this, _1) );
		//work_pool_.start( 4, boost::bind(&account_manager::create_db_context, this, _1) );

        main_loop_.loop();
    }

	log_server* log_server::instance()
	{
		return sg_log_server;
	}

	boost::shared_ptr<dbcontext> log_server::create_db_context(work_thread*)
	{
		try 
		{
			//暂时让accountserver加载dbcontext
			return boost::make_shared<dbcontext>(db_ip_.c_str(), "koo_log", "123456", "koo_log");
		}
		catch ( sql::SQLException& e )
		{
			LOG_ERROR << "log_server::create_db_context failed : " << e.what();
			throw e;
		}
	}
    
    int log_server::on_pre_load()
    {
        return 0;
    }
    
    void log_server::on_post_unload()
    {
        log_server_.reset();
        shutdown();
    }
    
    void log_server::on_connection(const tcp_connection_ptr& conn)
    {
        main_loop_.run_in_loop(boost::bind(&log_server::on_connection_in_loop, this, conn));
    }
	void log_server::on_message(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st)
	{
		main_loop_.run_in_loop(boost::bind(&log_server::on_message_in_loop, this, conn, buf, st));
	}

    void log_server::on_timer()
    {
        //  nothing
		
    }
    
    void log_server::on_connection_in_loop(const tcp_connection_ptr &conn)
    {
        if ( conn->connected() )
        {
            //  建立连接处理
            LOG_INFO << "connection [" << conn << "] incoming from " << conn->peer_address().ip_port();
          
        }
        else
        {
            //  断开连接处理
            LOG_WARN << "connection [" << conn << "] disconnect from " << conn->peer_address().ip_port();
			conn->force_close();
        }
    }
     
	void log_server::on_message_in_loop(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st)
	{
		conn;
		buf;
		st;

		if (buf == nullptr) {
			return;
		}
		
		// 解压缩		
		while (buf->readable_bytes() >= 4)	{
			LogMsg * msg = (LogMsg*)(buf->peek());
			if (msg == nullptr) {
				return;
			}
			uLong tlen = 1024;// msg->src_len;
			uLongf blen = msg->head_len - 4;// msg->des_len;
			LogMsg log_msg;
			if (uncompress((Bytef *)(log_msg.content), &tlen, msg->content, blen) != Z_OK)
			{
				ZXERO_ASSERT(false) << "SendLogMessage failed! uncompress error! content=" << msg;
				return;
			}

			buf->retrieve(msg->head_len);

			Json::Reader reader;
			Json::Value root;
			try
			{
				if (reader.parse(std::string((char *)log_msg.content), root)) {
					LOG_INFO << "detail message, ServerGroupId:" << root["ServerGroupId"].asInt() << " Sql:" << root["Sql"].asString();
				}
			}
			catch (std::exception& e)
			{
				ZXERO_ASSERT(false) << "on_message_in_loop exception! " << e.what();
			}
		}
	}

	/////
	bool log_server::send(const tcp_connection_ptr& conn, const buffer* buf)
	{
		if ( !conn)
		{
			LOG_ERROR << "log_server::send msg failed";
			return false;
		}
		LogMsg msg;
		//	发送协议之
		conn->send(&msg,sizeof(LogMsg));
		return true;
	}

}   //  namespace game
