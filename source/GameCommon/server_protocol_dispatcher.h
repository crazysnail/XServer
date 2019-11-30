//
//  server_protocol_dispatcher.h
//  GameCommon
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014Äê zxero. All rights reserved.
//

#ifndef __GameCommon__server_protocol_dispatcher__
#define __GameCommon__server_protocol_dispatcher__
#include "boost/date_time/posix_time/ptime.hpp"
#include "boost/noncopyable.hpp"
#include "boost/function.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
namespace zxero
{
	namespace network
	{
		class event_loop;
		class tcp_connection;
	}
}

namespace boost
{
	template <typename T>
	class shared_ptr;
}

namespace google
{
	namespace protobuf
	{
		class Message;
		class Descriptor;
	}
}
namespace Mt
{
	using zxero::network::event_loop;
	class server_session;
	typedef boost::posix_time::ptime timestamp_t;
    typedef boost::function< void(const boost::shared_ptr<server_session> &, const boost::shared_ptr<google::protobuf::Message>&, timestamp_t) > server_protocol_callback_t;
    
    /////
    class server_protocol_callback : boost::noncopyable
    {
    public:
        virtual ~server_protocol_callback() {}
        virtual void on_message(const boost::shared_ptr<server_session>&, const boost::shared_ptr<google::protobuf::Message>& msg, timestamp_t ) = 0;
    };
    
    /////
    template< typename T >
    class server_protocol_callback_template : public server_protocol_callback
    {
    public:
        typedef boost::function< void(const boost::shared_ptr<server_session>&, boost::shared_ptr<T>&, timestamp_t ) > callback;
        server_protocol_callback_template(const callback& cb)
        :callback_(cb)
        {
            
        }
        
        void on_message(const boost::shared_ptr<server_session>& s, const boost::shared_ptr<google::protobuf::Message>& msg, timestamp_t t)
        {
            callback_(s, *(boost::shared_ptr<T>*)&msg, t);
        }
        
    public:
        callback callback_;
    };
    
    /////
    class server_protocol_dispatcher : boost::noncopyable
    {
    public:
        server_protocol_dispatcher( zxero::network::event_loop* loop, const server_protocol_callback_t& default_cb);
        
        void on_server_message( const boost::shared_ptr<zxero::network::tcp_connection>& conn,
			const boost::shared_ptr<google::protobuf::Message>& msg, timestamp_t t);
        
        template< typename T >
        void register_handler(const typename server_protocol_callback_template<T>::callback& cb)
        {
            auto pcb = boost::make_shared<server_protocol_callback_template<T> >(cb);
            callback_map_[T::descriptor()] = pcb;
        }
        
    protected:
        void on_server_message_in_loop(const boost::shared_ptr<zxero::network::tcp_connection>& conn,
			const boost::shared_ptr<google::protobuf::Message>& msg, timestamp_t t);
        
    protected:
        event_loop* loop_;
        
        typedef std::map<const google::protobuf::Descriptor*, boost::shared_ptr<server_protocol_callback> > callback_map;
        callback_map callback_map_;
        server_protocol_callback_t default_callback_;
    };
    
}   //  namespace Mt

#endif /* defined(__GameCommon__server_protocol_dispatcher__) */
