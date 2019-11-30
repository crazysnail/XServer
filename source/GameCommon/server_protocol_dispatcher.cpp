//
//  server_protocol_dispatcher.cpp
//  GameCommon
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "server_protocol_dispatcher.h"
#include "server_session.h"
#include "event_loop.h"
#include "tcp_connection.h"

namespace Mt
{
	using namespace zxero::network;
    server_protocol_dispatcher::server_protocol_dispatcher( event_loop* loop, const server_protocol_callback_t& default_cb )
    :loop_(loop)
    ,default_callback_(default_cb)
    {
        
    }
    
    void server_protocol_dispatcher::on_server_message(const boost::shared_ptr<tcp_connection> &conn, const boost::shared_ptr<google::protobuf::Message> &msg, timestamp_t t)
    {
        loop_->run_in_loop(boost::bind(&server_protocol_dispatcher::on_server_message_in_loop, this, conn, msg, t));
    }
    
    void server_protocol_dispatcher::on_server_message_in_loop(const boost::shared_ptr<tcp_connection> &conn, const boost::shared_ptr<google::protobuf::Message> &msg, timestamp_t t)
    {
        //  服务器会话
        boost::shared_ptr<server_session> session_;
        
        //  获取会话环境
        boost::any& c = conn->context();
        if ( c.empty() )
        {
            LOG_ERROR << "connection [" << conn << "] has no context for protocol: " << msg->GetTypeName();
            conn->force_close();
            return;
        }
        else
        {
            try {
                session_ = boost::any_cast<boost::shared_ptr<server_session>>(c);
            } catch (boost::bad_any_cast& e) {
                LOG_ERROR << "connection [" << conn << "] have invalid context: " << e.what();
                conn->force_close();
                return;
            }
            ZXERO_ASSERT(session_) << "connection [" << conn << "] get context failed";
        }
        
        //  验证状态处理消息
        switch (session_->get_state()) {
            case kUnauthorization:
                if (msg->GetDescriptor() != Server::IdentifyRequest::descriptor()) {
                    LOG_ERROR << "connection [" << conn << "] receive unauthorization protocol : " << msg->GetTypeName() << " on Unauthorization state";
                    conn->force_close();
                    return;
                }
                break;
            case kVerifing:
                if (msg->GetDescriptor() != Server::IdentifyReply::descriptor()) {
                    LOG_ERROR << "connection [" << conn << "] receive unauthorization protocol : " << msg->GetTypeName() << " on Verifing state";
                    conn->force_close();
                    return;
                }
                break;
            case kDisconnected:
            case kDisconnecting:
            {
                LOG_WARN << "connection [" << conn << "] receive invalid protocol : " << msg->GetTypeName() << " on state " << session_->get_state();
                return;
            }break;
                
            default:
                break;
        }
        
        //  将消息分发出去
        callback_map::iterator iter = callback_map_.find(msg->GetDescriptor());
        if ( iter != callback_map_.end() )
        {
            iter->second->on_message(session_, msg, t);
        }
        else
        {
            default_callback_(session_, msg, t);
        }
    }
    
}   //  namespace Mt

