#include "acceptor.h"
#include "log.h"

namespace zxero
{
	namespace network
	{
		acceptor::acceptor(event_loop* loop, const inet_address& listen_addr)
			:loop_(loop)
			, accept_socket_(INVALID_SOCKET)
			, accept_channel_(loop, accept_socket_.fd())
			, listening_(false)
		{
			accept_socket_.reuse_address(true);
			accept_socket_.bind(listen_addr);
			accept_channel_.read_callback(boost::bind(&acceptor::handle_read, this));
			LOG_TRACE << "acceptor [" << this << "] of " << listen_addr.ip_port() << " at " << accept_socket_.fd();
		}

		void acceptor::new_connection_callback(const new_connection_callback_t& cb)
		{
			new_connection_callback_ = cb;
		}

		bool acceptor::listening() const
		{
			return listening_;
		}

		void acceptor::listen()
		{
			loop_->assert_in_loop_thread();
			listening_ = true;
			accept_socket_.listen();
			accept_channel_.enable_reading();
			LOG_TRACE << "acceptor [" << this << "] started";
		}

		void acceptor::disablelistening()
		{
			loop_->assert_in_loop_thread();
			listening_ = false;
			accept_socket_.socket_close();
		}

		void acceptor::handle_read()
		{
			loop_->assert_in_loop_thread();
			//	zxero： 据说多次accept会适合短连接
			std::unique_ptr<tcp_socket> conn = accept_socket_.accept();
			if (conn)
			{
				LOG_TRACE << "acceptor [" << this << "] accept connection [" << conn << "] at " << conn->peer_address().ip_port();
				if (new_connection_callback_)
					new_connection_callback_(conn);

				//	如果没人需要这个连接，conn将会被自动析构
			}
		}

	}
}
