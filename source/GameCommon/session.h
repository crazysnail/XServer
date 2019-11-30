//
//  session.h
//  GameCommon
//
//  Created by zXero on 14-4-11.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __GameCommon__session__
#define __GameCommon__session__
#include "types.h"
#include "date_time.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>



namespace Mt
{
	using zxero::network::protobuf_codec;
	using zxero::network::event_loop;
	using zxero::network::tcp_connection;
	using namespace zxero;
    enum session_state
    {
        //  连接断开
        kDisconnected,
        //  连接正在断开中, 这是服务器发起的连接断开
        kDisconnecting,
        //  未授权状态
        kUnauthorization,
        
        //  下一个可用状态
        kNextAviableState,
    };
    
    //  标准会话逻辑
    class session : boost::noncopyable
    {
    public:
        session( const boost::shared_ptr<tcp_connection>& conn, protobuf_codec& codec );
        virtual ~session();
        
    public:
		//
		const boost::shared_ptr<tcp_connection>& connection() const;

        //  关闭会话
        void close(bool close_conn = true);

        //  向客户端发送协议, 连接断开的话，协议会发送失败
        bool send( const google::protobuf::Message& message );
		bool send(const boost::shared_ptr<google::protobuf::Message>& message);
        
        //  获取当前会话状态
        uint32 get_state() const {
            return state_;
        }
        
        //  设置当前会话状态
        //  如果设置失败，则返回false，否则返回true
        virtual bool state(uint32 s);
        
    public:
        //  给会话提供超时管理机制，方便相关工作超时处理
        void wait_timeout( event_loop* loop, boost::posix_time::time_duration td, const boost::function<void()> & cb );
        void cancel_timeout( event_loop* loop );
        
    protected:
        //  超时处理
        void handle_timeout( const boost::function<void()>& cb );
        
    protected:
        //  会话状态
        uint32  state_;
        //  超时计时器id
        zxero::network::timer_id timeout_;
        //  当前连接
        boost::shared_ptr<tcp_connection> conn_;
        //  编解码器
        protobuf_codec& codec_;
    };
}

#endif /* defined(__GameCommon__session__) */
