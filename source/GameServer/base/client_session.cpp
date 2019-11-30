//
//  client_session.cpp
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "client_session.h"
#include "protobuf_codec.h"
#include "tcp_connection.h"


namespace Mt
{
   	using namespace zxero::network;
    client_session::client_session( const boost::shared_ptr<tcp_connection>& conn, protobuf_codec& codec)
    :session(conn, codec)
    ,player_(nullptr)
    {
        state_ = kUnauthorization;
		//LOG_TRACE << "#LOGIN##CONNECT#client_session[" << this << "] with connection [" << conn << "] with csid " << csid;
    }
        
    client_session::~client_session()
    {
        //  当客户端会话析构时，连接应当不存在，玩家应当不存在
        ZXERO_ASSERT(player_ == nullptr);
            
        LOG_TRACE << "client_session dtor[" << this << "]";
    }
        
    bool client_session::state(uint32 s)
    {
        if ( s == kDisconnecting || s == kDisconnected )
        {
            LOG_TRACE << "client_session [" << this << "] move state from " << state_ << " to state " << s;
			state_ = s;
            return true;
        }
            
        switch (s) {
            case kUnauthorization:
				if ( state_ != kDisconnected )
				{
					LOG_ERROR << "client_session [" << this << "] can not move state from " << state_ << " to state " << s;
					return false;
				}
                break;
            case kVerifing:
                if (state_ != kUnauthorization) {
                    LOG_ERROR << "client_session [" << this << "] can not move state from " << state_ << " to state " << s;
                    return false;
                }
                break;
            case kInQueue:
                if (state_ != kVerifing) {
                    LOG_ERROR << "client_session [" << this << "] can not move state from " << state_ << " to state " << s;
                    return false;
                }
                break;
			case kBeforEnteringScene:
				if (state_ != kVerifing) {
					LOG_ERROR << "client_session [" << this << "] can not move state from " << state_ << " to state " << s;
					return false;
				}
				break;
            case kGaming:
                if (state_ != kBeforEnteringScene) {
                    LOG_ERROR << "client_session [" << this << "] can not move state from " << state_ << " to state " << s;
                    return false;
                }
                break;
            default:
                LOG_ERROR << "client_session [" << this << "] can not move state from " << state_ << " to state " << s;
                return false;
                break;
        }
            
        LOG_TRACE << "client_session [" << this << "] move state from " << state_ << " to state " << s;
        state_ = s;
        return true;
    }
    
	std::string client_session::get_ip()
	{
		if (connection())
		{
			return connection()->peer_address().ip_port();
		}
		return "0.0.0.0";
	}
        
}   //  namespace Mt


