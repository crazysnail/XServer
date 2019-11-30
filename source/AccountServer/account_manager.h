//
//  account_manager.h
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
    //  �˺Ź�����
    class account_manager : boost::noncopyable
    {
    public:
        account_manager();
    public:
        //	ģ����غ�ж��
        int on_pre_load();
        void on_post_unload();
        
        //
        void on_ip(const std::string& ip);
        void on_port(const int32& port);
		void on_db_ip(const std::string& ip);
		void on_db_port(const int32& port);
        
        //  ���߳�
        void main();

	public:
		//	��ȡ�˺Ź�����ʵ�������ڴ���
		static account_manager* instance();

	public:
		//	��������ֻ���ض��߼���ʹ�õĺ���
		bool add_online_user( online_user* ou );

		//	�Ƴ��������
		void remove_online_user( client_session_id csid );

	public:
		//	���ض��Ŀͻ��˻Ự����Э�飬ע�⣬����˵�ǿͻ��ˣ���ʵ�ǵ�ǰ�ͻ����ڵķ�����
		bool send( client_session_id csid, const google::protobuf::Message& message );
        
		//	��ȡ�ض��������
		online_user_ptr& get_online_user( client_session_id csid );

		//	���ݷ������ţ���ȡ��Ϸ������
		server_session_ptr& get_game_server( uint32 gsid );

    private:
        //  ���ӽ����뿪
        void on_connection( const boost::shared_ptr<zxero::network::tcp_connection>& conn );
        
        //  ���̴߳�������
        void on_connection_in_loop( const boost::shared_ptr<zxero::network::tcp_connection>& conn );
        
        //  Э�鴦����
        void on_protobuf_message(const boost::shared_ptr<zxero::network::tcp_connection>& conn, const boost::shared_ptr<google::protobuf::Message>& msg, zxero::timestamp_t t);
        
        void on_default_protobuf_message(const server_session_ptr& session, const boost::shared_ptr<google::protobuf::Message>& msg, zxero::timestamp_t t );
        
        void on_timer();
        
        //  δ��Ȩ�ĻỰ��ʱ����
        void on_unauthorization_session_timeout( const server_session_ptr& session );
        
    private:
        //  �������ݿ⹤������
        work_context_t create_db_context(work_thread*);

		//	�����Ϸ������
		int32 add_game_server( const server_session_ptr& session );
        
		//	�Ƴ���Ϸ������
		int32 remove_game_server( const server_session_ptr& session );

		

    private:
        //  �����������֤
        void on_server_identify(const server_session_ptr& session, const boost::shared_ptr<Server::IdentifyRequest>& req, zxero::timestamp_t t);
        //  �˺ŷ������յ���ע������
        void on_register_request(const server_session_ptr& session, const boost::shared_ptr<Account::RegisterRequest>& req, zxero::timestamp_t t);
        //  �˺ŷ������յ��ĵ�¼����
        void on_login_request(const server_session_ptr& session, const boost::shared_ptr<Account::LoginRequest>& req, zxero::timestamp_t t);
        //	�˺ŷ������յ�����������
		void on_logout_request(const server_session_ptr& session, const boost::shared_ptr<Account::LogoutRequest>& req, zxero::timestamp_t t);

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
        
        //  �˺ŷ�����������
        std::unique_ptr<tcp_server> account_server_;

		//
		std::vector< server_session_ptr > game_servers_;
        
        //  Э�������
        protobuf_codec server_codec_;
        //  ��Ϣ�ַ���
        server_protocol_dispatcher server_dispatcher_;
        
        //	����ӳ���
		typedef std::map< client_session_id, online_user_ptr > online_user_csid_map;
		typedef std::map< std::string, online_user* > online_user_name_map;

		online_user_csid_map online_user_csid_map_;
		online_user_name_map online_user_name_map_;

	private:
		static account_manager* sg_account_manager;

    };
}

#endif /* defined(__GameServer__account_manager__) */
