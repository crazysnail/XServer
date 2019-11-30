//	消息循环线程池，from muduo
#ifndef _zxero_network_event_loop_thread_pool_h_
#define _zxero_network_event_loop_thread_pool_h_

#include "types.h"
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace zxero
{
	namespace network
	{
		class event_loop_thread_pool : boost::noncopyable
		{
		public:
			typedef boost::function<void(event_loop*)> thread_init_callback_t;

			event_loop_thread_pool(event_loop* base_loop);
			~event_loop_thread_pool();

			void start(uint32 num_thread, const thread_init_callback_t& cb = thread_init_callback_t());
			event_loop* next_loop();

		private:
			event_loop*	base_loop_;
			bool started_;
			uint32 num_threads_;
			uint32 next_;
			//	需要threads_有一个可维护的生命期
			boost::ptr_vector<event_loop_thread> threads_;
			std::vector<event_loop*> loops_;
		};
	}
}


#endif //	#ifndef _zxero_network_event_loop_thread_pool_h_


