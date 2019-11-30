#include "tcp_client.h"
#include "tcp_connection.h"
#include "tcp_socket.h"
#include "connector.h"
#include "network_errno.h"
#include "event_loop.h"
#include "log.h"
#include "boost/container/string.hpp"

namespace zxero
{
	namespace network
	{
		namespace detail
		{
			void remove_connection(event_loop* loop, const tcp_connection_ptr& conn)
			{
				loop->queue_in_loop(bind(&tcp_connection::connect_destroyed, conn));
			}

			void remove_connector(const boost::shared_ptr<connector>&)
			{

			}
		}

		tcp_client::tcp_client(event_loop* loop, const inet_address& server_address, const std::string& name)
			:loop_(loop)
			, connector_(new connector(loop, server_address))
			, name_(name)
			, connection_callback_(default_connection_callback)
			, message_callback_(default_message_callback)
			, retry_(false)
			, connect_(true)
			, next_conn_id_(1)
		{
			connector_->new_connection_callback(boost::bind(&tcp_client::new_connection, this, _1));
			LOG_TRACE << "tcp_client ctor[" << name_ << "] - connector" << get_pointer(connector_) ;
		}

		tcp_client::~tcp_client()
		{
			LOG_TRACE << "tcp_client dtor[" << name_ << "] - connector" << get_pointer(connector_) ;
			tcp_connection_ptr conn;
			{
				mutex::scoped_lock lock(mutex_);
				conn = connection_;
			}
			if (conn)
			{
				close_callback_t cb = bind(&detail::remove_connection, loop_, _1);
				loop_->run_in_loop(boost::bind(&tcp_connection::close_callback, conn, cb));
			}
			else
			{
				connector_->stop();
				loop_->run_after(seconds(1), bind(&detail::remove_connector, connector_));
			}
		}

		//
		void tcp_client::connect()
		{
			connect_ = true;
			connector_->start();
		}

		void tcp_client::disconnect()
		{
			connect_ = false;

			{
				mutex::scoped_lock lock(mutex_);
				if (connection_)
				{
					connection_->shutdown();
				}
			}
		}

		void tcp_client::stop()
		{
			connect_ = false;
			connector_->stop();
		}

		void tcp_client::new_connection(std::unique_ptr<tcp_socket>& sock_)
		{
			loop_->assert_in_loop_thread();

			char buf[32];
			zprintf(buf, 31, ":%s#%d", sock_->peer_address().ip_port().c_str(), next_conn_id_);

			++next_conn_id_;
			std::string conn_name = name_ + buf;
			sock_->nonblocking(true);
			tcp_connection_ptr conn(new tcp_connection(loop_, conn_name, sock_));

			conn->connection_callback(connection_callback_);
			conn->message_callback(message_callback_);
			conn->write_complete_callback(write_complete_callback_);
			conn->close_callback(bind(&tcp_client::remove_connection, this, _1));
			{
				mutex::scoped_lock lock(mutex_);
				connection_ = conn;
			}
			conn->connect_established();
		}

		void tcp_client::remove_connection(const boost::shared_ptr<tcp_connection>& conn)
		{
			loop_->assert_in_loop_thread();
			ZXERO_ASSERT(loop_ == conn->get_loop());

			{
				mutex::scoped_lock lock(mutex_);
				ZXERO_ASSERT(conn == connection_);
				connection_.reset();
			}

			loop_->queue_in_loop(bind(&tcp_connection::connect_destroyed, conn));
			if (retry_ && connect_)
			{
				LOG_DEBUG << "tcp_client::connect[" << name_ << "] - reconnecting to " << connector_->server_address().ip_port() ;
				connector_->restart();
			}
		}


		////
		boost::shared_ptr<tcp_connection> tcp_client::connection() const
		{
			mutex::scoped_lock lock(mutex_);
			return connection_;
		}

		event_loop* tcp_client::get_loop() const
		{
			return loop_;
		}

		bool tcp_client::retry() const
		{
			return retry_;
		}

		void tcp_client::enable_retry()
		{
			retry_ = true;
		}

		void tcp_client::connection_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&)>& cb) { connection_callback_ = cb; }
		void tcp_client::message_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)>& cb) { message_callback_ = cb; }
		void tcp_client::write_complete_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&)>& cb) { write_complete_callback_ = cb; }
	}
}
