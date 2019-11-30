#include "tcp_server.h"
#include "tcp_socket.h"
#include "event_loop.h"
#include "tcp_connection.h"
#include "event_loop_thread_pool.h"
#include "acceptor.h"
#include "log.h"
#include <string.h>
#include <boost/make_shared.hpp>

namespace zxero
{
	namespace network
	{
		tcp_server::tcp_server(event_loop* loop, const inet_address& listen_addr, const std::string& name)
			:loop_(loop)
			, hostport_(listen_addr.ip_port())
			, name_(name)
			, acceptor_(new acceptor(loop, listen_addr))
			, io_pool_(new event_loop_thread_pool(loop))
			, connection_callback_(default_connection_callback)
			, message_callback_(default_message_callback)
			, started_(0)
			, next_connection_id_(0)
		{
			LOG_TRACE << "tcp_server ctor[" << this << "] " << name << " on " << listen_addr.ip_port();
			acceptor_->new_connection_callback(boost::bind(&tcp_server::new_connection, this, _1));
		}

		tcp_server::~tcp_server()
		{
			//	根据需要移除connection_map中的内容
			loop_->assert_in_loop_thread();
			for (auto& iter : connection_map_)
			{
				tcp_connection_ptr conn = iter.second;
				iter.second.reset();
				conn->get_loop()->run_in_loop(bind(&tcp_connection::connect_destroyed, conn));
				conn.reset();
			}

			LOG_TRACE << "tcp_server dtor[" << this << "]";
		}

		const std::string& tcp_server::hostport() const
		{
			return hostport_;
		}

		const std::string& tcp_server::name() const
		{
			return name_;
		}

		event_loop* tcp_server::get_loop() const
		{
			return loop_;
		}

		void tcp_server::connection_callback(const connection_callback_t& cb)
		{
			connection_callback_ = cb;
		}

		void tcp_server::message_callback(const message_callback_t& cb)
		{
			message_callback_ = cb;
		}


		/////
		void tcp_server::start(uint32 num_threads, const thread_init_callback_t& cb)
		{
			if (started_.exchange(1) == 0)
			{
				io_pool_->start(num_threads, cb);

				ZXERO_ASSERT(!acceptor_->listening());
				loop_->run_in_loop(boost::bind(&acceptor::listen, acceptor_.get()));
			}
		}

		void tcp_server::stop_acceptor()
		{
			acceptor_->disablelistening();
		}

		void tcp_server::new_connection(tcp_socket_ptr& sock)
		{
			loop_->assert_in_loop_thread();

			event_loop* io_loop = io_pool_->next_loop();

			char buf[32];
			zprintf(buf, sizeof(buf), "#%d", next_connection_id_);

			++next_connection_id_;
			std::string conn_name = name_ + buf;

			//
			LOG_INFO << "tcp_server [" << name_
				<< "] - new connection [" << conn_name
				<< "] from " << sock->peer_address().ip_port()
				;

			tcp_connection_ptr conn = boost::make_shared<tcp_connection>(io_loop, conn_name, sock);
			connection_map_[conn_name] = conn;

			//	设置各项回调
			conn->connection_callback(connection_callback_);
			conn->message_callback(message_callback_);
			conn->write_complete_callback(write_complete_callback_);
			conn->close_callback(bind(&tcp_server::remove_connection, this, _1));
			//
			io_loop->run_in_loop(bind(&tcp_connection::connect_established, conn));
		}

		void tcp_server::remove_connection(const tcp_connection_ptr& conn)
		{
			loop_->run_in_loop(bind(&tcp_server::remove_connection_in_loop, this, conn));
		}

		void tcp_server::remove_connection_in_loop(const tcp_connection_ptr& conn)
		{
			loop_->assert_in_loop_thread();

			size_t n = connection_map_.erase(conn->name());
			(void)n;
			if (n != 1)
			{
				ZXERO_ASSERT(false) << "remove con more "<<conn->name();
				return;
			}
			
			event_loop* io_loop = conn->get_loop();
			io_loop->queue_in_loop(bind(&tcp_connection::connect_destroyed, conn));
		}

		bool tcp_server::started() const
		{
			return started_ == 1;
		}

	}
}

