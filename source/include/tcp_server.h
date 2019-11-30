//	tcp·þÎñÆ÷£¬from muduo
#ifndef _zxero_network_tcp_server_h_
#define _zxero_network_tcp_server_h_

#include "types.h"
#include "date_time.h"
#include <boost/scoped_ptr.hpp>
#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace zxero
{
	namespace network
	{
		class tcp_server : boost::noncopyable
		{
		public:
			typedef boost::function<void(event_loop*)> thread_init_callback_t;

			tcp_server(event_loop* loop, const inet_address& listen_addr, const std::string& name);
			~tcp_server();

		public:
			const std::string& hostport() const;
			const std::string& name() const;

			event_loop* get_loop() const;
			bool started() const;

		public:
			void start(uint32 num_threads, const thread_init_callback_t& cb = thread_init_callback_t());

			void stop_acceptor();
			//
			void connection_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&)>& cb);
			void message_callback(const boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)>& cb);
		
		private:
			void new_connection(std::unique_ptr<tcp_socket>&);
			//
			void remove_connection(const  boost::shared_ptr<tcp_connection>& conn);
			//
			void remove_connection_in_loop(const  boost::shared_ptr<tcp_connection>& conn);

		private:
			typedef std::map<std::string, boost::shared_ptr<tcp_connection>> connection_map_t;

			//	acceptor loop
			event_loop* loop_;
			//
			const std::string hostport_;
			//
			const std::string name_;
			//
			boost::scoped_ptr<acceptor> acceptor_;
			boost::scoped_ptr<event_loop_thread_pool> io_pool_;
			//
			boost::function<void(const boost::shared_ptr<tcp_connection>&)> connection_callback_;
			boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t)> message_callback_;
			boost::function<void(const boost::shared_ptr<tcp_connection>&)> write_complete_callback_;
			//
			boost::atomic<uint32> started_;
			uint32 next_connection_id_;
			//
			connection_map_t connection_map_;
		};
	}
}

#endif	//	#ifndef _zxero_network_tcp_server_h_
