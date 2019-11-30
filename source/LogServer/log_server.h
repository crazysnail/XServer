//
//  account_manager.h
//  GameServer
//
//  Created by gg on 2018/2/20
//  Copyright (c) 2018�� gg. All rights reserved.
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
        //	ģ����غ�ж��
        int on_pre_load();
        void on_post_unload();
        
        //
        void on_ip(const  std::string& ip);
        void on_port(const int32& port);
		void on_db_ip(const  std::string& ip);
		void on_db_port(const int32& port);
        
        //  ���߳�
        void main();

	public:
		//	��ȡ�˺Ź�����ʵ�������ڴ���
		static log_server* instance();

	public:
		//	���ض��Ŀͻ��˻Ự����Э�飬ע�⣬����˵�ǿͻ��ˣ���ʵ�ǵ�ǰ�ͻ����ڵķ�����
		bool send(const tcp_connection_ptr& conn, const buffer* buf);
    private:
        //  ���ӽ����뿪
        void on_connection( const tcp_connection_ptr& conn );
		void on_message(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st);
        
        //  ���̴߳�������
        void on_connection_in_loop( const tcp_connection_ptr& conn );
		void on_message_in_loop(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st);
        
		void on_timer();

    private:
        //  �������ݿ⹤������
		boost::shared_ptr<dbcontext> create_db_context(work_thread*);

    private:
        //
        std::string server_ip_;
        int32 server_port_;

		std::string db_ip_;
		int32 db_port_;
        
        //  ���¼�ѭ��
        event_loop main_loop_;
        //  ���ݿ⹤����
        work_pool work_pool_;
        
        //  ������������
        std::unique_ptr<tcp_server> log_server_;

		////
		//vector< server_session_ptr > game_servers_;
  //      
  //      //  Э�������
  //      protobuf_codec server_codec_;
  //      //  ��Ϣ�ַ���
  //      server_protocol_dispatcher server_dispatcher_;

	private:
		static log_server* sg_log_server;

    };
}

#endif /* defined(__GameServer__log_manager__) */
