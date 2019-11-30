//
//  account_manager.cpp
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "account_manager.h"
#include "account_dbwork.h"
#include "module.h"
#include "app.h"
#include "tcp_server.h"
#include "server_session.h"
#include "inet_address.h"
#include "dbcontext.h"
#include "online_user.h"
#include "cppconn/exception.h"

namespace game
{
	account_manager* account_manager::sg_account_manager = nullptr;

    REGISTER_MODULE(account_manager)
    {
        //  设置主线程工作函数
        app::set_main_work(boost::bind(&account_manager::main, this));
        
        //
        register_load_function(STAGE_PRE_LOAD, boost::bind(&account_manager::on_pre_load, this));
        register_unload_function(STAGE_POST_UNLOAD, boost::bind(&account_manager::on_post_unload, this));
        
        //  注册选项
        register_option("ip", "ip for this server, default is \"0.0.0.0\"", boost::program_options::value<std::string>()->default_value("0.0.0.0"), opt_delegate<std::string>(boost::bind(&account_manager::on_ip, this, _1)));
        register_option("port", "port for this server, default is '9986'", boost::program_options::value<int32>()->default_value(9982), opt_delegate<int32>(boost::bind(&account_manager::on_port, this, _1)));
		register_option("db_ip", "ip to db server, default is \"127.0.0.1\"", boost::program_options::value<std::string>()->default_value("127.0.0.1"), opt_delegate<std::string>(boost::bind(&account_manager::on_db_ip, this, _1)));
		register_option("db_port", "port to db server, default is '3306'", boost::program_options::value<int32>()->default_value(3306), opt_delegate<int32>(boost::bind(&account_manager::on_db_port, this, _1)));

	}
    
    account_manager::account_manager()
    : server_ip_("0.0.0.0")
    , server_port_(9982)
	, db_ip_("127.0.0.1")
	, db_port_(3306)
    , work_pool_(&main_loop_, "dbwork_pool")
    , server_codec_(true, boost::bind(&account_manager::on_protobuf_message, this, _1, _2, _3))
    , server_dispatcher_(&main_loop_, boost::bind(&account_manager::on_default_protobuf_message, this, _1, _2, _3))
    {
        sg_account_manager = this;

		//	保留足够的游戏服务器存储位置
		game_servers_.resize( kGameServerNumber );
    }
    
    void account_manager::on_ip(const std::string &ip)
    {
        server_ip_ = ip;
    }
    
    void account_manager::on_port(const int32 &port)
    {
        server_port_ = port;
    }

	void account_manager::on_db_ip(const std::string& ip)
	{
		db_ip_ = ip;
	}

	void account_manager::on_db_port(const int32 &port)
	{
		db_port_ = port;
	}
    
    void account_manager::main()
    {
        main_loop_.loop();
    }

	account_manager* account_manager::instance()
	{
		return sg_account_manager;
	}

	work_context_t account_manager::create_db_context(work_thread*)
	{
		try 
		{
			//暂时让accountserver加载dbcontext
			return boost::make_shared<dbcontext>("diablo_account", db_ip_.c_str(), "diablo_account", "123456");//db_port_
		}
		catch ( sql::SQLException& e )
		{
			LOG_ERROR << "account_manager::create_db_context failed : " << e.what();
			throw e;
		}
	}
    
    int account_manager::on_pre_load()
    {
        //  启动服务器
        startup();
        
        //
        log_message::log_level( LL_TRACE );
        
        LOG_INFO << "start server at " << server_ip_ << ":" << server_port_;
        account_server_.reset(new tcp_server(&main_loop_, inet_address(server_ip_, server_port_), "accountserver"));
        account_server_->connection_callback(boost::bind(&account_manager::on_connection, this, _1));
        account_server_->message_callback(boost::bind(&protobuf_codec::on_message, &server_codec_, _1, _2, _3));
        account_server_->start(2);
        
        //  定期激活，干啥呢，显示当前在线玩家情况
        main_loop_.run_every(seconds(10), boost::bind(&account_manager::on_timer, this));
        
        //  注册协议处理事件
        server_dispatcher_.register_handler<Server::IdentifyRequest>(boost::bind(&account_manager::on_server_identify, this, _1, _2, _3));
        server_dispatcher_.register_handler<Account::RegisterRequest>(boost::bind(&account_manager::on_register_request, this, _1, _2, _3));
        server_dispatcher_.register_handler<Account::LoginRequest>(boost::bind(&account_manager::on_login_request, this, _1, _2, _3));
		server_dispatcher_.register_handler<Account::LogoutRequest>(boost::bind(&account_manager::on_logout_request, this, _1, _2, _3));

		//	启动数据库工作线程

		// 为方便调试，暂时只启用一个线程 [9/6/2014 seed]
		work_pool_.start( 1, boost::bind(&account_manager::create_db_context, this, _1) );
		//work_pool_.start( 4, boost::bind(&account_manager::create_db_context, this, _1) );

        return 0;
    }
    
    void account_manager::on_post_unload()
    {
        account_server_.reset();
        shutdown();
    }
	
    void account_manager::on_connection(const boost::shared_ptr<zxero::network::tcp_connection>& conn)
    {
        main_loop_.run_in_loop(boost::bind(&account_manager::on_connection_in_loop, this, conn));
    }
    
    void account_manager::on_protobuf_message(const boost::shared_ptr<zxero::network::tcp_connection> &conn, const boost::shared_ptr<google::protobuf::Message> &msg, zxero::timestamp_t t)
    {
        server_dispatcher_.on_server_message(conn, msg, t);
    }
    
    void account_manager::on_default_protobuf_message(const server_session_ptr &session, const boost::shared_ptr<google::protobuf::Message> &msg, zxero::timestamp_t t)
    {
        LOG_WARN << "msg " << msg->GetTypeName() << " from " << session << " unhandler";
    }
    
    void account_manager::on_timer()
    {
        //  nothing


    }
    
    void account_manager::on_connection_in_loop(const boost::shared_ptr<zxero::network::tcp_connection> &conn)
    {
        if ( conn->connected() )
        {
            //  建立连接处理
            LOG_INFO << "connection [" << conn << "] incoming from " << conn->peer_address().ip_port();
            
            //  创建服务器会话
            server_session_ptr session = boost::make_shared<server_session>(conn, server_codec_);
            conn->context(session);
            
            //  设置连接授权超时逻辑
            session->wait_timeout(&main_loop_, seconds(10), boost::bind(&account_manager::on_unauthorization_session_timeout, this, session));
            
        }
        else
        {
            //  断开连接处理
            LOG_WARN << "connection [" << conn << "] disconnect from " << conn->peer_address().ip_port();
            
            //  移除服务器会话
            auto& ctx = conn->context();
            if ( ctx.empty() )
            {
                LOG_WARN << "connection [" << conn << "] without context";
                return;
            }
            
            //  获取会话并清除连接会话数据
            server_session_ptr session = boost::any_cast<server_session_ptr>(ctx);
            ctx.clear();

			//	//基于不同的session类型进行清理
			switch ( session->type() )
			{
			case Server::kGameServer:
				{
					remove_game_server( session );
				}break;
			default:
				break;
			}
            
            //  关闭会话
            session->close(false);
            session->cancel_timeout(&main_loop_);
        }
    }
    
    void account_manager::on_unauthorization_session_timeout(const server_session_ptr &session)
    {
        LOG_WARN << "session [" << session << "] wait authorization time out";
        session->close();
    }
    
    void account_manager::on_server_identify(const server_session_ptr &session, const boost::shared_ptr<Server::IdentifyRequest> &req, zxero::timestamp_t t)
    {
        //
        LOG_INFO << "session [" << session << "] send authorization with "
        << "version[" << req->version() << "] "
        << "password[" << req->password() << "] "
        << "serverid[" << req->id() << "] "
        << "type[" << req->type() << "] "
        << "ip[" << req->ip() << "] "
        << "port[" << req->port() << "] "
        ;

		//	进入验证阶段
		session->state( kVerifing );
        
		//	记录会话属性
		session->set_type( req->type() );
		session->set_server_id( req->id() );

		//
		char buf[32];

        //  处理授权请求
		int32 result = 0;
		switch( req->type() )
		{
		case Server::kGameServer:
			{
				zprintf( buf, sizeof(buf), "gameserver#%u", req->id() );
				session->set_name( buf );

				result = add_game_server( session );
			}break;
		default:
			break;
		}
        
        //
		if ( result == 0 )
		{
			LOG_INFO << "session [" << session << "] authorized";
			session->state( kAuthorization );
			session->cancel_timeout( &main_loop_ );
		}
		else
		{
			session->set_type( Server::kUnknownServer );
			LOG_WARN << "session [" << session << "] authorization failed, error code " << result;
		}
        
        //  回应授权结果
        Server::IdentifyReply reply;
        reply.set_result(result);
        
        session->send(reply);
    }
    
    void account_manager::on_register_request(const server_session_ptr &session, const boost::shared_ptr<Account::RegisterRequest> &req, zxero::timestamp_t t)
    {
        LOG_INFO << "session [" << session << "] send register request with "
        << "account[" << req->account() << "] "
        << "password[" << req->password() << "] "
        << "csid[" << req->csid() << "] "
        ;
        
        //  直接往数据库插入
		work_pool_.add_work( new account_register_work( req ) );
    }
    
    void account_manager::on_login_request(const server_session_ptr &session, const boost::shared_ptr<Account::LoginRequest> &req, zxero::timestamp_t t)
    {
        LOG_INFO << "session [" << session << "] send login request with "
        << "version[" << req->version() << "] "
        << "account[" << req->account() << "] "
        << "password[" << req->password() << "] "
        << "csid[" << req->csid() << "] "
        ;
        
        //  先查询在线玩家情况
        
        //  如果没有在线玩家，向数据库查询账号状态
		work_pool_.add_work( new account_login_work( req ) );
    }

	void account_manager::on_logout_request(const server_session_ptr &session, const boost::shared_ptr<Account::LogoutRequest> &req, zxero::timestamp_t t)
	{
		LOG_INFO << "session [" << session << "] send logout request with "
			<< "csid[" << req->csid() << "] "
			<< "reason[" << req->reason() << "] ";

		//	zXero: 需要保存账号数据, 如果有其它玩家要上线, 通知其上线, 否则移除在线玩家
		//	先直接移除在线玩家了， 没有其他处理
		remove_online_user( client_session_id(req->csid()) );
	}


	/////
	bool account_manager::send( client_session_id csid, const google::protobuf::Message& message )
	{
		auto& session = get_game_server(csid.game_server_id);
		if ( !session )
		{
			LOG_ERROR << "account_manager::send msg " << message.GetTypeName() << " to " << csid << " failed";
			return false;
		}

		//	发送协议之
		return session->send(message);
	}

	bool account_manager::add_online_user( online_user* ou )
	{
		ZXERO_ASSERT( ou != nullptr );

		online_user_ptr ouptr(ou);
		
		//	同时加入两个列表中
		//	需要去重逻辑
		online_user_csid_map_.insert( std::make_pair( ouptr->csid, ouptr ) );
		online_user_name_map_.insert( std::make_pair( ouptr->account, ou ) );

		//	
		LOG_INFO << "account_manager::add_online_user " << ouptr->account << " with csid " << ouptr->csid;

		//	通知对应的游戏服务器上线成功
		Account::LoginReply reply;
		reply.set_csid( ouptr->csid );
		//Sotcc::Result::S_OK 
		reply.set_result( 0 );
		reply.set_account( ou->account.c_str() );
		reply.set_accountid( ou->id );

		return send( ouptr->csid, reply );
	}

	void account_manager::remove_online_user( client_session_id csid )
	{
		auto iter = online_user_csid_map_.find( csid );
		if ( iter == online_user_csid_map_.end() )
		{
			LOG_ERROR << "account_manager::remove_online_user failed, csid " << csid << " not exist";
			return;
		}

		online_user_name_map_.erase( iter->second->account );
		online_user_csid_map_.erase( iter );

		LOG_INFO << "account_manager::remove_online_user, csid " << csid;
	}

	online_user_ptr& account_manager::get_online_user( client_session_id csid )
	{
		auto iter = online_user_csid_map_.find( csid );
		if ( iter == online_user_csid_map_.end() )
		{
			static online_user_ptr null_online_user_ptr;
			return null_online_user_ptr;
		}

		return iter->second;
	}
    
	int32 account_manager::add_game_server( const server_session_ptr& session )
	{
		ZXERO_ASSERT( session->type() == Server::kGameServer );
		if ( session->server_id() >= kGameServerNumber )
		{
			LOG_ERROR << "account_manager::add_game_server failed, session [" << session << "] has invalid server id " << session->server_id();
			return 1;
		}

		auto& gs = game_servers_[session->server_id()];
		if ( gs )
		{
			LOG_ERROR << "account_manager::add_game_server failed, session [" << session << "] with server id " << session->server_id() << " already exit";
			return 1;
		}

		gs = session;
		LOG_INFO << "account_manager::add_game_server, session [" << session << "] with server id " << session->server_id();
		return 0;
	}

	int32 account_manager::remove_game_server( const server_session_ptr& session )
	{
		ZXERO_ASSERT( session->type() == Server::kGameServer );

		if ( session->server_id() >= kGameServerNumber )
		{
			LOG_ERROR << "account_manager::remove_game_server failed, session [" << session << "] has invalid server id " << session->server_id();
			return 1;
		}

		auto& gs = game_servers_[session->server_id()];
		if ( gs && gs == session )
		{
			gs.reset();
			LOG_INFO << "account_manager::remove_game_server, session [" << session << "] with server id " << session->server_id();
			return 0;
		}
		else
		{
			ZXERO_ASSERT( gs != nullptr );
			LOG_WARN << "account_manager::remove_game_server failed, session [" << session << "] with server id " << session->server_id() << " not in game server list";
			return 1;
		}
	}

	server_session_ptr& account_manager::get_game_server( uint32 gsid )
	{
		ZXERO_ASSERT( gsid < kGameServerNumber );

		return game_servers_[gsid];
	}

}   //  namespace game
