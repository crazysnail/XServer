//	tcp连接，from muduo
#ifndef _zxero_network_tcp_connection_h_
#define _zxero_network_tcp_connection_h_

#include "inet_address.h"
#include "buffer.h"
#include "date_time.h"
#include <string>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>

namespace zxero
{
	namespace network
	{
		//
		typedef boost::shared_ptr<tcp_connection> tcp_connection_ptr;

		//
		typedef boost::function<void(const tcp_connection_ptr&)> connection_callback_t;
		typedef boost::function<void(const tcp_connection_ptr&)> close_callback_t;
		typedef boost::function<void(const tcp_connection_ptr&)> write_complete_callback_t;

		typedef boost::function<void(const tcp_connection_ptr&, buffer*, timestamp_t)> message_callback_t;
		typedef std::unique_ptr<tcp_socket> tcp_socket_ptr;


		class tcp_connection : boost::noncopyable, public boost::enable_shared_from_this<tcp_connection>
		{
		public:
			tcp_connection(event_loop* loop, const std::string& name, tcp_socket_ptr& sock);
			~tcp_connection();

		public:
			event_loop* get_loop() const;
			const std::string& name() const;
			const inet_address& local_address() const;
			const inet_address& peer_address() const;

			bool connected() const;

			//
			void send(const void* message, size_t len);
			void send(const std::string& message);
			void send(buffer* message);
			void shutdown();
			void force_close();

			//
			void no_delay(bool on);

		public:
			void context(const boost::any& context);
			const boost::any& context() const;
			boost::any& context();

		public:
			void connection_callback(const connection_callback_t& cb);
			void message_callback(const message_callback_t& cb);
			void write_complete_callback(const write_complete_callback_t& cb);
			void close_callback(const close_callback_t& cb);

		public:
			void connect_established();
			void connect_destroyed();

		private:
			enum StateE {
				kDisconnected,
				kConnecting,
				kConnected,
				kDisconnecting,
			};
			void handle_read(timestamp_t overtime);
			void handle_write();
			void handle_close();
			void handle_error();

			//
			void send_in_loop(const void* msg, size_t len);
			void send_in_loop(const std::string& msg);
			void shutdown_in_loop();
			void force_close_in_loop();
			void state(StateE s);

		private:

			event_loop*	loop_;
			std::string name_;
			StateE state_;
			tcp_socket_ptr socket_;
			boost::scoped_ptr<channel> channel_;
			inet_address local_address_;
			inet_address peer_address_;

			connection_callback_t connection_callback_;
			message_callback_t message_callback_;
			write_complete_callback_t write_complete_callback_;
			close_callback_t close_callback_;

			//	输入输出缓冲
			buffer input_buffer_;
			buffer output_buffer_;

			//	用于关联到其他对象
			boost::any context_;
		};

		//
		void default_connection_callback(const tcp_connection_ptr&);
		void default_message_callback(const tcp_connection_ptr& conn, buffer* buf, timestamp_t overtime);
	}
}

#endif	//	#ifndef _zxero_network_tcp_connection_h_
