//
//  tcp_client_pool.cpp
//  zxero
//
//  Created by zXero on 14-4-15.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "tcp_client_pool.h"
#include "tcp_client.h"
#include "event_loop_thread_pool.h"
#include "event_loop_thread.h"
#include "event_loop.h"
#include "log.h"
#include "tcp_connection.h"

namespace zxero
{
    namespace network
    {
        
        tcp_client_pool::tcp_client_pool( event_loop* loop, const std::string& name )
        :loop_(loop)
        ,name_(name)
        ,io_pool_(new event_loop_thread_pool(loop))
        ,next_client_id_(0)
        {
            LOG_TRACE << "tcp_client_pool ctor[" << this << "] " << name;
        }
        
        tcp_client_pool::~tcp_client_pool()
        {
            loop_->assert_in_loop_thread();
            for (auto& iter : client_map_)
            {
                auto client = iter.second;
                iter.second.reset();
                client->stop();
                client->disconnect();
            }
            
            LOG_TRACE << "tcp_client_pool dtor[" << this << "]";
        }
        
        const std::string& tcp_client_pool::name() const
        {
            return name_;
        }
        
        event_loop* tcp_client_pool::get_loop() const
        {
            return loop_;
        }
        
        void tcp_client_pool::connection_callback(const connection_callback_t &cb)
        {
            connection_callback_ = cb;
        }
        
        void tcp_client_pool::message_callback(const message_callback_t &cb)
        {
            message_callback_ = cb;
        }
        
        void tcp_client_pool::start(uint32 num_threads, const thread_init_callback_t& cb)
        {
            if (started_.exchange(1) == 0)
            {
                //  只用启动工作线程就好了
                io_pool_->start(num_threads, cb);
            }
        }
        
        boost::shared_ptr<tcp_client> tcp_client_pool::connect(const zxero::network::inet_address &server_address, bool retry)
        {
            loop_->assert_in_loop_thread();
            
            char buf[32];
            zprintf(buf, sizeof(buf), "%s#%d", name_.c_str(), next_client_id_);

            ++next_client_id_;
            
            LOG_INFO << "tcp_client_pool [" << name_ << "] open connection [" << buf << "] to " << server_address.ip_port();
            
            //  获取工作线程循环
            event_loop* io_loop = io_pool_->next_loop();
            
            //  创建客户端连接
			boost::shared_ptr<tcp_client> client = boost::make_shared<tcp_client>(io_loop, server_address, buf);
            ZXERO_ASSERT(client) << " tcp_client_pool connect to " << server_address.ip_port() << " failed";
            
            client_map_.insert(std::make_pair(std::string(buf), client));
            
            //  设置连接回调
            client->connection_callback(connection_callback_);
            client->message_callback(message_callback_);
            
            if (retry) {
                client->enable_retry();
            }
            
            //  启动连接
            client->connect();
            
            return client;
        }
        
    }   //  namespace network
}   //  namespace zxero

