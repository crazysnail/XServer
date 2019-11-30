//
//  account_manager.h
//  GameServer
//
//  Created by gg on 2018/2/20
//  Copyright (c) 2018年 gg. All rights reserved.
//

#ifndef __GameServer__log_manager__
#define __GameServer__log_manager__

#include "types.h"
#include "work_pool.h"
#include "event_loop.h"
#include "network.h"
#include "tcp_connection.h"
#include "tcp_server.h"
#include "protobuf_codec.h"
#include "server_session.h"
#include "server_protocol_dispatcher.h"
#include <zlib.h>

namespace game
{
	struct LogMsg {
		int32 head_len;
		Bytef content[1024];
	};
	using namespace zxero::network;
    class log_server : public boost::noncopyable
    {
    public:
		log_server();
    public:
        //	模块加载和卸载
        int on_pre_load();
        void on_post_unload();
        
        //
        void on_ip(const  std::string& ip);
        void on_port(const int32& port);
		void on_db_ip(const  std::string& ip);
		void on_db_port(const int32& port);
        
        //  主线程
        void main();

	public:
		//	获取账号管理器实例，用于处理
		static log_server* instance();

	public:
		//	向特定的客户端会话发送协议，注意，这里说是客户端，其实是当前客户端在的服务器
		bool send(const tcp_connection_ptr& conn, const buffer* buf);
    private:
        //  连接进入离开
        void on_connection( const tcp_connection_ptr& conn );
		void on_message(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st);
        
        //  主线程处理连接
        void on_connection_in_loop( const tcp_connection_ptr& conn );
		void on_message_in_loop(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st);
        
		void on_timer();

    private:
        //  创建数据库工作环境
		boost::shared_ptr<dbcontext> create_db_context(work_thread*);

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
        
        //  服务器管理器
        std::unique_ptr<tcp_server> log_server_;

		////
		//vector< server_session_ptr > game_servers_;
  //      
  //      //  协议解码器
  //      protobuf_codec server_codec_;
  //      //  消息分发器
  //      server_protocol_dispatcher server_dispatcher_;

	private:
		static log_server* sg_log_server;

    };
}

#endif /* defined(__GameServer__log_manager__) */
