//	事件循环 from moduo
#ifndef _zxero_network_event_loop_h_
#define _zxero_network_event_loop_h_


#include "types.h"
#include "thread.h"
#include "date_time.h"
#include <boost/scoped_ptr.hpp>


namespace zxero
{
	namespace network
	{
		class poller;
		
		class event_loop : boost::noncopyable
		{
		public:
			//	
			typedef boost::function<void()> functor_t;
		public:
			event_loop();
			~event_loop();
		
		public:
			void loop();

			void assert_in_loop_thread();
			int32 efficency() const;
			bool is_in_loop_thread() const;

			//	后续根据需要考虑要不要提供等待处理吧
			void quit(/*bool wait_for_quit = false*/);

		public:
			//	
			timer_id run_at(timestamp_t time, const boost::function<void()>& cb);
			timer_id run_after(time_duration td, const boost::function<void()>& cb);
			timer_id run_every(time_duration td, const boost::function<void()>& cb);

			//
			void cancel(timer_id tid);

		public:
			//	
			void run_in_loop(const functor_t& cb);
			//
			void queue_in_loop(const functor_t& cb);

		public:
			static event_loop* get_event_loop_of_current_thread();

		public:
			//	也不提供唤醒函数，所谓及时这件事也就是5ms的问题，对于跨线程操作来说，就等着吧

			void update_channel(channel*);
			void remove_channel(channel*);

		private:
			void abort_not_in_loop_thread();

			void do_pending_functors();

		private:
			typedef std::vector<channel*>	channel_list_t;
			typedef std::vector<functor_t>	pending_functors_t;

			bool looping_;
			bool quit_;
			bool event_handling_;

			const thread_id_t thread_id_;
			boost::scoped_ptr<poller> poller_;
			boost::scoped_ptr<timer_queue> timer_queue_;
			std::vector<int32> efficency_;
			channel_list_t active_channels_;
			channel* current_active_channel_;
			mutex mutex_;
			pending_functors_t pending_functors_;
			pending_functors_t pending_functors_temp_;
		};

	}
}


#endif	//	#ifndef _zxero_network_event_loop_h_

