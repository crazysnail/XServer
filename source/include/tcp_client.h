//	客户端连接
#ifndef _zxero_network_tcp_client_h_
#define _zxero_network_tcp_client_h_

#include "types.h"
#include "date_time.h"
#include <memory> // for std::unique_ptr
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>

namespace zxero
{

	namespace network
	{
		class tcp_client : boost::noncopyable
		{
		public:
			tcp_client(event_loop* loop, const inet_address& server_address, const std::string& name);
			~tcp_client();

			void connect();
			void disconnect();
			void stop();

		public:
			boost::shared_ptr<tcp_connection> connection() const;

			event_loop* get_loop() const;
			bool retry() const;
			void enable_retry();

			void connection_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&)>& cb);
			void message_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)>& cb);
			void write_complete_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&)>& cb);

			const boost::shared_ptr<connector>& get_connector() { return connector_; };

		private:
			void new_connection(std::unique_ptr<tcp_socket>& sock_);
			void remove_connection(const boost::shared_ptr<tcp_connection>& conn);

		private:
			event_loop* loop_;
			boost::shared_ptr<connector> connector_;
			const std::string name_;
			boost::function<void(const boost::shared_ptr<tcp_connection>&)> connection_callback_;
			boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)> message_callback_;
			boost::function<void(const boost::shared_ptr<tcp_connection>&)> write_complete_callback_;

			bool retry_;
			bool connect_;

			int next_conn_id_;
			mutable boost::mutex mutex_;
			boost::shared_ptr<tcp_connection> connection_;
		};
	}
}


#endif	//	#ifndef _zxero_network_tcp_client_h_



