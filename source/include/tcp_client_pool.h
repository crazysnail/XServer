//
//  tcp_client_pool.h
//  zxero
//
//  Created by zXero on 14-4-13.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef zxero_tcp_client_pool_h
#define zxero_tcp_client_pool_h

#include "types.h"
#include "date_time.h"
#include <string>
#include <boost/atomic.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>


namespace zxero {
    namespace network
    {
        class tcp_client_pool : boost::noncopyable
        {
        public:
            typedef boost::function<void(event_loop*)> thread_init_callback_t;
            
            tcp_client_pool(event_loop* loop, const std::string& name);
            ~tcp_client_pool();
            
        public:
            const std::string& name() const;
            
            event_loop* get_loop() const;
            
        public:
            void start(uint32 num_threads, const thread_init_callback_t& cb = thread_init_callback_t());
            
            //  向目标服务器连接，返回对应的客户端连接指针
            boost::shared_ptr<tcp_client> connect( const inet_address& server_address, bool retry = true );
            
            //  设置连接回调和消息回调
            void connection_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&)>& cb);
            void message_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)>& cb);
            
        private:
            typedef std::map<std::string, boost::shared_ptr<tcp_client>> client_map_t;
            
            //  event_loop
            event_loop* loop_;
            //
            const std::string name_;
            //  连接线程池
            boost::scoped_ptr<event_loop_thread_pool> io_pool_;
            //  连接回调
			boost::function<void(const boost::shared_ptr<tcp_connection>&)> connection_callback_;
			boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)> message_callback_;
            
            //
            boost::atomic<uint32> started_;
            uint32 next_client_id_;
            
            //
            client_map_t client_map_;
            
        };
    }   //  namespace network
}   //  namespace game

#endif
