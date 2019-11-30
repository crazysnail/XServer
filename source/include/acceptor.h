//	socket½ÓÊÕÆ÷£¬from muduo
#ifndef _zxero_network_acceptor_h_
#define _zxero_network_acceptor_h_

#include "types.h"
#include "tcp_socket.h"
#include "channel.h"
#include "inet_address.h"
#include "event_loop.h"

namespace zxero
{
	namespace network
	{
		class acceptor : boost::noncopyable
		{
		public:
			typedef boost::function<void(std::unique_ptr<tcp_socket>&)> new_connection_callback_t;

			acceptor(event_loop* loop, const inet_address& listen_addr);

		public:
			void new_connection_callback(const new_connection_callback_t& cb);

			bool listening() const;
			void listen();

			void disablelistening();
		
		private:
			void handle_read();

			event_loop* loop_;
			tcp_socket accept_socket_;
			channel accept_channel_;
			new_connection_callback_t new_connection_callback_;
			bool listening_;
		};

	}
}

#endif	//	#ifndef _zxero_network_acceptor_h_

