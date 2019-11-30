#include "protobuf_dispatcher.h"
#include "google/protobuf/message.h"

namespace zxero
{
	namespace network
	{
		using namespace boost;
		protobuf_dispatcher::protobuf_dispatcher(event_loop* loop, const protobuf_message_callback_t& default_callback)
			:loop_(loop)
			, default_message_callback_(default_callback)
		{

		}

		void protobuf_dispatcher::on_message(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t)
		{
			loop_->run_in_loop(bind(&protobuf_dispatcher::on_message_in_loop, this, conn, msg, t));
		}

		void protobuf_dispatcher::on_message_in_loop(const shared_ptr<tcp_connection>& conn, const shared_ptr<google::protobuf::Message>& msg, timestamp_t t)
		{
			loop_->assert_in_loop_thread();

			message_signal_map::iterator iter = message_signal_map_.find(msg->GetDescriptor());
			if (iter != message_signal_map_.end())
			{
				message_signal& sig = *iter->second;
				sig(conn, msg, t);
			}
			else
			{
				default_message_callback_(conn, msg, t);
			}
		}
	}
}
