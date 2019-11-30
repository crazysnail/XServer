#include "protobuf_dispatcher2.h"
#include "event_loop.h"
#include "google/protobuf/message.h"

namespace zxero
{
	namespace network
	{
		protobuf_dispatcher2::protobuf_dispatcher2(event_loop* loop, const protobuf_codec::protobuf_message_callback_t& default_cb)
			:loop_(loop)
			, default_callback_(default_cb)
		{

		}

		void protobuf_dispatcher2::on_message(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t)
		{
			loop_->run_in_loop(bind(&protobuf_dispatcher2::on_message_in_loop, this, conn, msg, t));
		}

		void protobuf_dispatcher2::on_message_in_loop(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t)
		{
			callback_map::iterator iter = callback_map_.find(msg->GetDescriptor());
			if (iter != callback_map_.end())
			{
				iter->second->on_message(conn, msg, t);
			}
			else
			{
				default_callback_(conn, msg, t);
			}
		}
	}
}
