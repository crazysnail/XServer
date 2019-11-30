//	事件循环线程, from muduo
#ifndef _zxero_network_event_loop_thread_h_
#define _zxero_network_event_loop_thread_h_

#include "types.h"
#include "thread.h"
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace zxero
{
	namespace network
	{
		class event_loop_thread : boost::noncopyable
		{
		public:
			typedef boost::function<void(event_loop*)>	thread_init_callback_t;

			event_loop_thread(const thread_init_callback_t& cb = thread_init_callback_t());
			~event_loop_thread();

			event_loop* start_loop();
		private:
			void thread_function();

		private:
			event_loop* loop_;
			bool exiting_;
			thread_t thread_;
			mutex mutex_;
			condition_variable_any cond_;
			thread_init_callback_t callback_;
		};
	}
}


#endif	//

