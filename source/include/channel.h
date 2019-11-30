//	socket的代理，注意，这里没有完全兼容linux
#ifndef _zxero_network_channel_h_
#define _zxero_network_channel_h_

#include "types.h"
#include "date_time.h"
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

namespace zxero
{
	namespace network
	{
		class channel : boost::noncopyable
		{
		public:
			typedef boost::function<void()> event_callback_t;
			typedef boost::function<void(timestamp_t)> read_event_callback_t;
		public:
			channel(event_loop* loop, socket_t sock);
			~channel();

		public:
			void handle_event(timestamp_t overtime);

			void read_callback(const read_event_callback_t& cb);
			void write_callback(const event_callback_t& cb);
			void error_callback(const event_callback_t& cb);
			void close_callback(const event_callback_t& cb);

			//
			void tie(const boost::shared_ptr<void>&);

			socket_t fd() const;
			
			int events() const;

			void revents(int revt);
			int revents() const;

			bool nonevent() const;
			bool writing() const;

			void enable_reading();
			void enable_writing();
			void disable_reading();
			void disable_writing();
			void disable_all();

			//
			int index() const;
			void index(int idx);

			event_loop* owner_loop() const;
			void remove();

		public:
			static const int k_none_event;
			static const int k_read_event;
			static const int k_write_event;
			static const int k_error_event;
			static const int k_close_event;
			static const int k_invalid_event;
			static const int k_remote_close_event;

		private:
			void update();
			void handle_event_with_guard(timestamp_t overtime);

			event_loop* loop_;
			const socket_t socket_;
			int events_;
			int revents_;
			int index_;

			bool event_handling_;
			bool tied_;
			boost::weak_ptr<void> tie_;

			read_event_callback_t read_callback_;
			event_callback_t write_callback_;
			event_callback_t error_callback_;
			event_callback_t close_callback_;
		};
	}
}

#endif	//
