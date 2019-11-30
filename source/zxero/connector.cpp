#include "connector.h"
#include "event_loop.h"
#include "tcp_socket.h"
#include "network_errno.h"
#include "channel.h"
#include "log.h"


namespace zxero
{
	namespace network
	{
		const time_duration connector::kMaxRetryDelayMs = seconds(30);
		const time_duration connector::kInitRetryDelayMs = milliseconds(500);

		connector::connector(event_loop* loop, const inet_address& server_address)
			:loop_(loop)
			, server_address_(server_address)
			, connect_(false)
			, state_(kDisconnected)
			, retry_delay_(kInitRetryDelayMs)
		{
			LOG_TRACE << "connector ctor [" << this << "] of " << server_address.ip_port();
		}

		connector::~connector()
		{
			LOG_TRACE << "connector dtor [" << this << "]" ;
			ZXERO_ASSERT(!channel_) << this;
		}

		void connector::new_connection_callback(const new_connection_callback_t& cb)
		{
			new_connection_callback_ = cb;
		}

		void connector::state(StateE s)
		{
			state_ = s;
		}

		const inet_address& connector::server_address() const
		{
			return server_address_;
		}

		void connector::start()
		{
			connect_ = true;
			loop_->run_in_loop(bind(&connector::start_in_loop, shared_from_this()));
		}

		void connector::start_in_loop()
		{
			loop_->assert_in_loop_thread();

			ZXERO_ASSERT(state_ == kDisconnected) << this;
			if (connect_)
			{
				connect();
			}
			else
			{
				LOG_WARN << this << " not start connection" ;
			}
		}

		void connector::stop()
		{
			connect_ = false;
			loop_->run_in_loop(bind(&connector::stop_in_loop, shared_from_this()));
		}

		void connector::stop_in_loop()
		{
			loop_->assert_in_loop_thread();

			if (state_ == kConnecting)
			{
				state(kDisconnected);
				remove_and_reset_channel();
				retry();
			}
		}

		//
		void connector::connect()
		{
			socket_.reset(new tcp_socket(INVALID_SOCKET));
			socket_->nonblocking(false);
			LOG_DEBUG << "connector [" << this << "] start connect to " << server_address_.ip_port() << " at " << socket_->fd();
			int ret = socket_->connect(server_address_);
			int savedErrno = (ret == 0) ? 0 : net_errno();
			switch (savedErrno)
			{
			case 0:
			case E_IN_PROGRESS:
			case E_INTR:
			case E_IS_CONN:
				connecting();
				break;
			case E_AGAIN:
			case E_ADDR_INUSE:
			case E_ADDR_NOTAVAIL:
			case E_CONN_REFUSED:
			case E_NET_UNREACH:
				retry();
				break;
			case E_ACCES:
#ifndef ZXERO_OS_WINDOWS
			case E_PERM:
#endif
			case E_AF_NO_SUPPORT:
			case E_ALREADY:
			case E_BADF:
			case E_FAULT:
			case E_NOT_SOCK:
				LOG_ERROR << "connector [" << this << "] connect error in connector::start in loop " << savedErrno ;
				socket_.reset();
				break;
			default:
				LOG_ERROR << "connector [" << this << "] unexpected error in connector::start in loop " << savedErrno ;
				socket_.reset();
				break;
			}
		}

		//
		void connector::restart()
		{
			loop_->assert_in_loop_thread();
			state(kDisconnected);
			retry_delay_ = kInitRetryDelayMs;
			connect_ = true;
			start_in_loop();
		}

		//
		void connector::connecting()
		{
			state(kConnecting);
			ZXERO_ASSERT(!channel_);

			channel_.reset(new channel(loop_, socket_->fd()));
			channel_->write_callback(bind(&connector::handle_write, shared_from_this()));
			channel_->error_callback(bind(&connector::handle_error, shared_from_this()));

			channel_->enable_writing();
		}

		//
		void connector::remove_and_reset_channel()
		{
			channel_->disable_all();
			channel_->remove();
			loop_->queue_in_loop(bind(&connector::reset_channel, shared_from_this()));
		}

		void connector::reset_channel()
		{
			channel_.reset();
		}

		void connector::handle_write()
		{
			LOG_TRACE << "connector [" << this << "] handle_write " << state_ ;

			if (state_ == kConnecting)
			{
				remove_and_reset_channel();
				int err = socket_->error();
				if (err)
				{
					LOG_ERROR << "connector [" << this << "] handle_write - SO_ERROR = " << err ;
					retry();
				}
				else if (socket_->self_connect())
				{
					LOG_ERROR << "connector [" << this << "] handle_write - self connect" ;
					retry();
				}
				else
				{
					state(kConnected);
					if (connect_)
					{
						new_connection_callback_(socket_);
					}

					//	如果连接完成了，没人要这个socket就关闭了
					socket_.reset();
				}
			}
			else
			{
				ZXERO_ASSERT(state_ == kDisconnected) << this;
			}
		}

		void connector::handle_error()
		{
			LOG_ERROR << "connector [" << this << "] handle_error - state = " << state_ ;
			if (state_ == kConnecting)
			{
				remove_and_reset_channel();
				int err = socket_->error();
				LOG_ERROR << "connector [" << this << "] handle_error - SO_ERROR = " << err ;
				retry();
			}
		}

		void connector::retry()
		{
			socket_.reset();
			state(kDisconnected);
			if (connect_)
			{
				LOG_INFO << "connector [" << this << "] retry - retry connecting to " << server_address_.ip_port() << " in " << retry_delay_.total_milliseconds() << " milliseconds." ;
				loop_->run_after(retry_delay_, boost::bind(&connector::start_in_loop, shared_from_this()));
				retry_delay_ *= 2;
				if (retry_delay_ > kMaxRetryDelayMs)
					retry_delay_ = kMaxRetryDelayMs;
			}
			else
			{
				LOG_WARN << "connector [" << this << "] not start connection" ;
			}
		}
	}
}
