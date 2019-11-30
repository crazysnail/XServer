//	连接器,from muduo
#ifndef _zxero_network_connector_h_
#define _zxero_network_connector_h_

#include "types.h"
#include "inet_address.h"
#include "date_time.h"
#include "tcp_socket.h"
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace zxero
{

	namespace network
	{

		class connector : boost::noncopyable, public boost::enable_shared_from_this<connector>
		{
		public:
			typedef boost::function<void(std::unique_ptr<tcp_socket>&)> new_connection_callback_t;

			connector(event_loop*, const inet_address&);
			~connector();

		public:
			void new_connection_callback(const new_connection_callback_t&);

			void start();
			void stop();

			//	loop线程才能调用
			void restart();

			const inet_address& server_address() const;

		private:
			enum StateE {
				kDisconnected,
				kConnecting,
				kConnected,
			};

			static const time_duration kMaxRetryDelayMs/* = seconds(30)*/;
			static const time_duration kInitRetryDelayMs/* = milliseconds(500)*/;

			void state(StateE s);
			void start_in_loop();
			void stop_in_loop();
			void connect();
			void connecting();
			void handle_write();
			void handle_error();
			void retry();
			void remove_and_reset_channel();
			void reset_channel();

		private:
			event_loop* loop_;
			inet_address server_address_;
			bool connect_;
			StateE state_;
			boost::scoped_ptr<channel> channel_;
			std::unique_ptr<tcp_socket> socket_;
			new_connection_callback_t new_connection_callback_;
			time_duration retry_delay_;
		};

	}

}



#endif	//	#ifndef _zxero_network_connector_h_

