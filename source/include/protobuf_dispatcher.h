//	基于signal2和protobuf的分发器
#ifndef _zxero_network_protobuf_dispatcher_h_
#define _zxero_network_protobuf_dispatcher_h_

#include "types.h"
#include "date_time.h"
#include "event_loop.h"
#include <boost/signals2.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace zxero
{
	namespace network
	{
		using namespace boost;
		typedef shared_ptr<google::protobuf::Message> message_ptr;
		typedef function<void(const shared_ptr<tcp_connection>&, const shared_ptr<google::protobuf::Message>&, timestamp_t)> protobuf_message_callback_t;

		class protobuf_dispatcher : noncopyable
		{
			typedef signals2::signal< void(const shared_ptr<tcp_connection>&, const message_ptr&, timestamp_t) > message_signal;
			typedef std::unique_ptr<message_signal> message_signal_ptr;
			typedef std::map<const google::protobuf::Descriptor*, message_signal* > message_signal_map;
		public:
			protobuf_dispatcher(event_loop* loop, const protobuf_message_callback_t& default_callback);

		public:
			//
			void on_message(const shared_ptr<tcp_connection>&, const shared_ptr<google::protobuf::Message>&, timestamp_t);

			//
			template<typename T>
			signals2::connection register_handler(const function<void(const shared_ptr<tcp_connection>&, const boost::shared_ptr<T>&, timestamp_t)>& f)
			{			
				loop_->assert_in_loop_thread();

				typedef signals2::signal< void(const shared_ptr<tcp_connection>&, const boost::shared_ptr<T>&, timestamp_t) > message_T_signal;
				//	获取sig
				message_T_signal* tsig = nullptr;
				message_signal_map::iterator iter = message_signal_map_.find(T::descriptor());
				if (iter == message_signal_map_.end())
				{
					//	创建新的signal
					tsig = new message_T_signal;
					message_signal_map_[T::descriptor()] = (message_signal*)tsig;
				}
				else
				{
					tsig = (message_T_signal*)iter->second;
				}

				//
				return tsig->connect(f);
			}

		private:
			//
			void on_message_in_loop(const shared_ptr<tcp_connection>&, const message_ptr&, timestamp_t);

			event_loop* loop_;
			protobuf_message_callback_t default_message_callback_;
			message_signal_map	message_signal_map_;
			
		};

	}
}


#endif	//	#ifndef _zxero_network_protobuf_dispatcher_h_

