//
//  server_session.cpp
//  GameCommon
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014Äê zxero. All rights reserved.
//

#include "server_session.h"
#include "tcp_connection.h"

namespace Mt {
    server_session::server_session( Server::ServerType type, protobuf_codec& codec, const std::string& name )
    :session(nullptr, codec)
    ,type_(type)
	,id_(0)
    ,name_(name)
    {
        state_ = kUnauthorization;
        LOG_TRACE << "server_session ctor[" << this << "]";
    }
    
    server_session::server_session( const tcp_connection_ptr& conn, protobuf_codec& codec )
    :session(conn, codec)
    ,type_(Server::kUnknownServer)
	,id_(0)
    ,name_("unknown#" + conn->name())
    {
        state_ = kUnauthorization;
        LOG_TRACE << "server_session ctor[" << this << "] with connection [" << conn << "]";
    }
    
    server_session::~server_session()
    {
        LOG_TRACE << "server_session dtor[" << this << "]";
    }
    
    bool server_session::state(uint32 s)
    {
        if ( s == kDisconnecting || s == kDisconnected )
        {
            LOG_TRACE << "server_session [" << this << "] move state from " << state_ << " to state " << s;
            state_ = s;
            return true;
        }
        
        switch (s) {
            case kUnauthorization:
                if ( state_ != kDisconnected )
                {
                    LOG_ERROR << "server_session [" << this << "] can not move state from " << state_ << " to state " << s;
                    return false;
                }
                break;
            case kVerifing:
                if ( state_ != kUnauthorization ) {
                    LOG_ERROR << "server_session [" << this << "] can not move state from " << state_ << " to state " << s;
                    return false;
                }
                break;
            case kAuthorization:
                if ( state_ != kVerifing ) {
                    LOG_ERROR << "server_session [" << this << "] can not move state from " << state_ << " to state " << s;
                    return false;
                }
                break;
            default:
                LOG_ERROR << "server_session [" << this << "] can not move state from " << state_ << " to state " << s;
                return false;
                break;
        }
        
        LOG_TRACE << "server_session [" << this << "] move state from " << state_ << " to state " << s;
        state_ = s;
        return true;
    }
    
    tcp_connection_ptr server_session::rebind_connection(const tcp_connection_ptr &conn)
    {
        tcp_connection_ptr old = conn_;
        conn_ = conn;
        return old;
    }
}

