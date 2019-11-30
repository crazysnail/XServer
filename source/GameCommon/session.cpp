//
//  session.cpp
//  GameCommon
//
//  Created by zXero on 14-4-11.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "session.h"
#include "tcp_connection.h"
#include "protobuf_codec.h"
#include "event_loop.h"
#include "google/protobuf/message.h"

namespace Mt
{
    session::session( const boost::shared_ptr<tcp_connection>& conn, protobuf_codec& codec)
    :state_(kDisconnected)
    ,timeout_(zxero::network::kInvalidTimerId)
    ,conn_(conn)
    ,codec_(codec)
    {
		//LOG_TRACE << "#LOGIN##CONNECT#session [" << this << "] with conneciton [" << conn << "]";
    }
    
    session::~session()
    {
        ZXERO_ASSERT(conn_ == nullptr);
        
        LOG_TRACE << "session dtor[" << this << "]";
    }
    
    void session::close(bool close_conn)
    {
        if ( conn_ )
        {
			LOG_INFO << "[session]  closing conn:" << conn_->peer_address().ip_port() << "," << close_conn;
            //  切换状态未断开连接中
            state(kDisconnecting);
            //  关闭连接， 同时移除对连接的引用
            if ( close_conn )
                conn_->force_close();
            conn_.reset();
        }
    }
    
    bool session::send( const google::protobuf::Message& message )
    {
        if ( state_ != kDisconnected && state_ != kDisconnecting )
        {
			//LOG_INFO << "session " << this << " send message: " << message.GetDescriptor()->full_name();
			if (conn_ == nullptr) {
				ZXERO_ASSERT( conn_ != nullptr ) << " session " << this ;
			} else {
				codec_.send(conn_, message);
			}
            return true;
        }
        
        return false;
    }

	bool session::send(const boost::shared_ptr<google::protobuf::Message>& message)
	{
		return send(*message.get());
	}


    bool session::state(uint32 s)
    {
        if ( s == kDisconnecting || s == kDisconnected )
        {
            LOG_TRACE << "session [" << this << "] move state from " << state_ << " to state " << s;
            return true;
        }
        
        if ( s == kUnauthorization )
        {
            LOG_TRACE << "session [" << this << "] can not move state from " << state_ << " to state " << s;
            return false;
        }
        
        LOG_TRACE << "session [" << this << "] move state from " << state_ << " to state " << s;
        state_ = s;
        return true;
    }
    
    void session::wait_timeout(zxero::network::event_loop *loop, boost::posix_time::time_duration td, const boost::function<void()> &cb)
    {
        ZXERO_ASSERT(!timeout_.valid()) << " session [" << this << "] timer already exist";
        ZXERO_ASSERT(loop != nullptr);
        ZXERO_ASSERT(cb);
        
        //  转换为超时处理
        timeout_ = loop->run_after(td, boost::bind( &session::handle_timeout, this, cb) );
    }
    
    void session::cancel_timeout( event_loop* loop )
    {
        if ( timeout_.valid() )
        {
            loop->cancel( timeout_ );
            timeout_ = zxero::network::kInvalidTimerId;
        }
    }
    
    void session::handle_timeout(const boost::function<void()> &cb)
    {
        timeout_ = zxero::network::kInvalidTimerId;
        cb();
    }

	const boost::shared_ptr<tcp_connection>& session::connection() const
	{
		return conn_;
	}

}   //  namespace Mt


