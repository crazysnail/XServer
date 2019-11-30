//	��ʱ������
#ifndef _zxero_network_timer_queue_h_
#define _zxero_network_timer_queue_h_

#include "types.h"
#include "date_time.h"
#include "timer_id.h"
#include <boost/ptr_container/ptr_set.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace zxero
{
	namespace network
	{
		class timer_queue : boost::noncopyable
		{
		public:
			timer_queue(event_loop* loop);
			~timer_queue();

		public:
			//	
			timer_id add_timer(const boost::function<void()>& cb, timestamp_t when, time_duration interval);
			void cancel(timer_id);

		public:
		//	zxero: �����е㲻һ���������¼�������ǳ���������, 
		//	����ͨ����ȡ������һ��timer��ʱ����������poller�ĳ�ʱʱ�䣬����poll�����󣬼���Ƿ��ж�ʱ����ʱ
			time_duration check_expired_timer(timestamp_t);

		private:
			typedef boost::ptr_set<timer> timer_list_t;

			void add_timer_in_loop(timer*);
			void cancel_in_loop(timer_id);	

		private:
			event_loop* loop_;
			timer_list_t timers_;
			timer_list_t canceling_timers_;

			//
			timer_id	current_run_timer;
			

			//
			bool calling_expired_timers_;
		};
	}
}


#endif	//	#ifndef _zxero_network_timer_queue_h_

