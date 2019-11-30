//	第二版消息分发器，from muduo
#ifndef _zxero_network_protobuf_dispatcher2_h_
#define _zxero_network_protobuf_dispatcher2_h_

#include "protobuf_codec.h"

namespace zxero
{
	namespace network
	{
		using namespace boost;
		class message_callback : noncopyable
		{
		public:
			virtual ~message_callback() {};
			virtual void on_message(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t) = 0;
		};

		template<typename T>
		class message_callback_template : public message_callback
		{
		public:
			typedef function<void(const shared_ptr<tcp_connection>&, boost::shared_ptr<T>&, timestamp_t)> callback;

			message_callback_template(const callback& cb)
				:callback_(cb)
			{

			}

			void on_message(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t)
			{
				callback_(conn, *(boost::shared_ptr<T>*)&msg, t);
			}

		public:
			callback callback_;
		};

		class protobuf_dispatcher2 : noncopyable
		{
		public:
			protobuf_dispatcher2(event_loop* loop, const protobuf_codec::protobuf_message_callback_t& default_cb);

			void on_message(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t);

			template< typename T >
			void register_handler(const typename message_callback_template<T>::callback& cb)
			{
				boost::shared_ptr< message_callback_template<T> > pcb = boost::make_shared< message_callback_template<T> >(cb);
				callback_map_[T::descriptor()] = pcb;
			}

		protected:
			//
			void on_message_in_loop(const shared_ptr<tcp_connection>& conn, const message_ptr& msg, timestamp_t t);
		protected:
			event_loop* loop_;

			typedef std::map<const google::protobuf::Descriptor*, boost::shared_ptr<message_callback> > callback_map;
			callback_map callback_map_;
			protobuf_codec::protobuf_message_callback_t default_callback_;
		};
	}
}



#endif	//	#define _zxero_network_protobuf_dispatcher2_h_

