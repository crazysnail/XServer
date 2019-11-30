#include "timer_queue.h"
#include "timer.h"
#include "event_loop.h"
#include "log.h"
#include <boost/bind.hpp>

namespace zxero
{
	namespace network
	{
		timer_queue::timer_queue(event_loop* loop)
			:loop_(loop)
			, calling_expired_timers_( false )
		{

		}

		timer_queue::~timer_queue()
		{
		}

		timer_id timer_queue::add_timer(const boost::function<void()>& cb, timestamp_t when, time_duration interval)
		{
			timer* t = new timer(cb, when, interval);
			loop_->run_in_loop(boost::bind(&timer_queue::add_timer_in_loop, this, t));
			return timer_id(t);			
		}

		void timer_queue::cancel(timer_id tid)
		{
			ZXERO_ASSERT(tid.valid());
			loop_->run_in_loop(boost::bind(&timer_queue::cancel_in_loop, this, tid));
		}

		//
		void timer_queue::add_timer_in_loop(timer* t)
		{
			loop_->assert_in_loop_thread();
			
			//	ֱ�Ӳ���
			timers_.insert(t);
		}

		void timer_queue::cancel_in_loop(timer_id tid)
		{
			ZXERO_ASSERT(tid.valid());
			loop_->assert_in_loop_thread();
			
			timer_list_t::iterator iter = timers_.find(*tid.timer_);
			if (iter != timers_.end())
			{
				//	ֱ���ͷžͺ��ˣ�û����������
				timers_.release(iter);
			}
			else
			{
				//	���������ִ�е�timer
				if (tid == current_run_timer)
				{
					//	�����ÿգ���ʱ������timer�ᱻ��Ȼɾ��
					current_run_timer.reset();
				}
			}
		}

		//
		time_duration timer_queue::check_expired_timer(timestamp_t now)
		{
			loop_->assert_in_loop_thread();

			//	timer�Ĺ���ʱ���뵱ǰʱ��ļ��
			time_duration td;
			
			for (timer_list_t::iterator iter = timers_.begin(); iter != timers_.end(); iter = timers_.begin())
			{
				//	timer�Ĺ���ʱ���ȥ��ǰʱ�䣬���С�ڵ���0����timer���ڣ������ж�timer���������¸�timer���ڵļ��
				td = iter->expiration() - now;
				if (microseconds(0) < td)
					break;

				//	��ʱ��������Ƴ�
				auto spt = timers_.release(iter);
				//	ִ��֮, ��ʱ�п����Ƴ�timer����
				current_run_timer = spt.get();
				spt->run();

				//	����Ѿ����Ƴ���
				if (current_run_timer.valid() && spt->repeat())
				{
					//	������ظ�ִ�е�timer, ����������Ȼ�����ٲ��뵽timers_��
					//	zxero: ����������������������
					//			����1�����ڵ�ǰʱ��������һ�����ڼ��������Ŀǰʹ�õķ���
					//			����2������timer����Ĺ���ʱ���������ڼ�������ַ������ܻ��ڳ��������У���timerִ�ж�Σ���Ϊtick�ǱȽ����õ�
					spt->restart(now);

					//	���¼��붨ʱ������
					timers_.insert(spt.release());
				}
			}

			return td;
		}
	}
}

