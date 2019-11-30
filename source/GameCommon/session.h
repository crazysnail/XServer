//
//  session.h
//  GameCommon
//
//  Created by zXero on 14-4-11.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
        //  ���ӶϿ�
        kDisconnected,
        //  �������ڶϿ���, ���Ƿ�������������ӶϿ�
        kDisconnecting,
        //  δ��Ȩ״̬
        kUnauthorization,
        
        //  ��һ������״̬
        kNextAviableState,
    };
    
    //  ��׼�Ự�߼�
    class session : boost::noncopyable
    {
    public:
        session( const boost::shared_ptr<tcp_connection>& conn, protobuf_codec& codec );
        virtual ~session();
        
    public:
		//
		const boost::shared_ptr<tcp_connection>& connection() const;

        //  �رջỰ
        void close(bool close_conn = true);

        //  ��ͻ��˷���Э��, ���ӶϿ��Ļ���Э��ᷢ��ʧ��
        bool send( const google::protobuf::Message& message );
		bool send(const boost::shared_ptr<google::protobuf::Message>& message);
        
        //  ��ȡ��ǰ�Ự״̬
        uint32 get_state() const {
            return state_;
        }
        
        //  ���õ�ǰ�Ự״̬
        //  �������ʧ�ܣ��򷵻�false�����򷵻�true
        virtual bool state(uint32 s);
        
    public:
        //  ���Ự�ṩ��ʱ������ƣ�������ع�����ʱ����
        void wait_timeout( event_loop* loop, boost::posix_time::time_duration td, const boost::function<void()> & cb );
        void cancel_timeout( event_loop* loop );
        
    protected:
        //  ��ʱ����
        void handle_timeout( const boost::function<void()>& cb );
        
    protected:
        //  �Ự״̬
        uint32  state_;
        //  ��ʱ��ʱ��id
        zxero::network::timer_id timeout_;
        //  ��ǰ����
        boost::shared_ptr<tcp_connection> conn_;
        //  �������
        protobuf_codec& codec_;
    };
}

#endif /* defined(__GameCommon__session__) */
