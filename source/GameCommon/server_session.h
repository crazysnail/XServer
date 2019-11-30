//
//  server_session.h
//  GameCommon
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014年 zxero. All rights reserved.
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
        //  待授权中
        kVerifing = kNextAviableState,
        //  授权完成
        kAuthorization,
    };
    
    //enum server_session_type
    //{
    //    //  未知服务器类型
    //    kUnknownServer,
    //    //  账号服务器
    //    kAccountServer,
    //    //  世界服务器
    //    kWorldServer,
    //    //  负载服务器
    //    kBalanceServer,
    //    //  游戏服务器
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
        //  重新绑定连接
		boost::shared_ptr<tcp_connection> rebind_connection( const boost::shared_ptr<tcp_connection>& conn );
        
        //  设置会话名称
        void set_name(const std::string& name) {
            name_ = name;
        }
        
        //
        const std::string& name() const {
            return name_;
        }
        
        //  设置服务器会话类型
		void set_type(const Server::ServerType type) {
            type_ = type;
        }
        
		const Server::ServerType& type() const {
            return type_;
        }

		//	设置服务器标识
		void set_server_id(uint32 sid) {
			id_ = sid;
		}

		uint32 server_id() {
			return id_;
		}
        
        
    public:
        //  切换会话状态
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
