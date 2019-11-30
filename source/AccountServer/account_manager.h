//
//  account_manager.h
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __GameServer__account_manager__
#define __GameServer__account_manager__

#include "global.h"
#include "work_pool.h"
#include "event_loop.h"
#include "network.h"
#include "protobuf_codec.h"
#include "server_session.h"
#include "server_protocol_dispatcher.h"
#include "tcp_connection.h"

namespace game
{
	struct online_user;
	typedef boost::shared_ptr<online_user> online_user_ptr;
	using namespace Mt;
    //  账号管理器
    class account_manager : boost::noncopyable
    {
    public:
        account_manager();
    public:
        //	模块加载和卸载
        int on_pre_load();
        void on_post_unload();
        
        //
        void on_ip(const std::string& ip);
        void on_port(const int32& port);
		void on_db_ip(const std::string& ip);
		void on_db_port(const int32& port);
        
        //  主线程
        void main();

	public:
		//	获取账号管理器实例，用于处理
		static account_manager* instance();

	public:
		//	公开但是只有特定逻辑能使用的函数
		bool add_online_user( online_user* ou );

		//	移除在线玩家
		void remove_online_user( client_session_id csid );

	public:
		//	向特定的客户端会话发送协议，注意，这里说是客户端，其实是当前客户端在的服务器
		bool send( client_session_id csid, const google::protobuf::Message& message );
        
		//	获取特定在线玩家
		online_user_ptr& get_online_user( client_session_id csid );

		//	根据服务器号，获取游戏服务器
		server_session_ptr& get_game_server( uint32 gsid );

    private:
        //  连接进入离开
        void on_connection( const boost::shared_ptr<zxero::network::tcp_connection>& conn );
        
        //  主线程处理连接
        void on_connection_in_loop( const boost::shared_ptr<zxero::network::tcp_connection>& conn );
        
        //  协议处理函数
        void on_protobuf_message(const boost::shared_ptr<zxero::network::tcp_connection>& conn, const boost::shared_ptr<google::protobuf::Message>& msg, zxero::timestamp_t t);
        
        void on_default_protobuf_message(const server_session_ptr& session, const boost::shared_ptr<google::protobuf::Message>& msg, zxero::timestamp_t t );
        
        void on_timer();
        
        //  未授权的会话超时处理
        void on_unauthorization_session_timeout( const server_session_ptr& session );
        
    private:
        //  创建数据库工作环境
        work_context_t create_db_context(work_thread*);

		//	添加游戏服务器
		int32 add_game_server( const server_session_ptr& session );
        
		//	移除游戏服务器
		int32 remove_game_server( const server_session_ptr& session );

		

    private:
        //  服务器身份验证
        void on_server_identify(const server_session_ptr& session, const boost::shared_ptr<Server::IdentifyRequest>& req, zxero::timestamp_t t);
        //  账号服务器收到的注册请求
        void on_register_request(const server_session_ptr& session, const boost::shared_ptr<Account::RegisterRequest>& req, zxero::timestamp_t t);
        //  账号服务器收到的登录请求
        void on_login_request(const server_session_ptr& session, const boost::shared_ptr<Account::LoginRequest>& req, zxero::timestamp_t t);
        //	账号服务器收到的下线请求
		void on_logout_request(const server_session_ptr& session, const boost::shared_ptr<Account::LogoutRequest>& req, zxero::timestamp_t t);

    private:
        //
        std::string server_ip_;
        int32 server_port_;

		std::string db_ip_;
		int32 db_port_;
        
        //  主事件循环
		event_loop main_loop_;
        //  数据库工作池
        work_pool work_pool_;
        
        //  账号服务器管理器
        std::unique_ptr<tcp_server> account_server_;

		//
		std::vector< server_session_ptr > game_servers_;
        
        //  协议解码器
        protobuf_codec server_codec_;
        //  消息分发器
        server_protocol_dispatcher server_dispatcher_;
        
        //	两个映射表
		typedef std::map< client_session_id, online_user_ptr > online_user_csid_map;
		typedef std::map< std::string, online_user* > online_user_name_map;

		online_user_csid_map online_user_csid_map_;
		online_user_name_map online_user_name_map_;

	private:
		static account_manager* sg_account_manager;

    };
}

#endif /* defined(__GameServer__account_manager__) */
