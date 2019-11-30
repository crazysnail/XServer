//
//  server_session.h
//  GameCommon
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014�� zxero. All rights reserved.
//

#ifndef __GameCommon__server_session__
#define __GameCommon__server_session__

#include "types.h"
#include "global.h"
#include "session.h"
#include "Server.pb.h"

namespace Mt
{
    enum server_session_state
    {
        //  ����Ȩ��
        kVerifing = kNextAviableState,
        //  ��Ȩ���
        kAuthorization,
    };
    
    //enum server_session_type
    //{
    //    //  δ֪����������
    //    kUnknownServer,
    //    //  �˺ŷ�����
    //    kAccountServer,
    //    //  ���������
    //    kWorldServer,
    //    //  ���ط�����
    //    kBalanceServer,
    //    //  ��Ϸ������
    //    kGameServer,
    //};
    
	using namespace zxero::network;
    class server_session : public session
    {
    public:
        server_session( Server::ServerType type, protobuf_codec& codec, const std::string& name );
        server_session( const boost::shared_ptr<tcp_connection>& conn, protobuf_codec& codec );
        ~server_session();
        
    public:
        //  ���°�����
		boost::shared_ptr<tcp_connection> rebind_connection( const boost::shared_ptr<tcp_connection>& conn );
        
        //  ���ûỰ����
        void set_name(const std::string& name) {
            name_ = name;
        }
        
        //
        const std::string& name() const {
            return name_;
        }
        
        //  ���÷������Ự����
		void set_type(const Server::ServerType type) {
            type_ = type;
        }
        
		const Server::ServerType& type() const {
            return type_;
        }

		//	���÷�������ʶ
		void set_server_id(uint32 sid) {
			id_ = sid;
		}

		uint32 server_id() {
			return id_;
		}
        
        
    public:
        //  �л��Ự״̬
        virtual bool state( uint32 s );
        
    protected:
        //
        Server::ServerType type_;
		//
		uint32 id_;
        //
        std::string name_;
    };
    
}   //  namespace game

#endif /* defined(__GameCommon__server_session__) */
