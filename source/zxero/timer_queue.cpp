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
			
			//	直接插入
			timers_.insert(t);
		}

		void timer_queue::cancel_in_loop(timer_id tid)
		{
			ZXERO_ASSERT(tid.valid());
			loop_->assert_in_loop_thread();
			
			timer_list_t::iterator iter = timers_.find(*tid.timer_);
			if (iter != timers_.end())
			{
				//	直接释放就好了，没有其他需求
				timers_.release(iter);
			}
			else
			{
				//	如果是正在执行的timer
				if (tid == current_run_timer)
				{
					//	将其置空，此时表明该timer会被自然删除
					current_run_timer.reset();
				}
			}
		}

		//
		time_duration timer_queue::check_expired_timer(timestamp_t now)
		{
			loop_->assert_in_loop_thread();

			//	timer的过期时间与当前时间的间隔
			time_duration td;
			
			for (timer_list_t::iterator iter = timers_.begin(); iter != timers_.end(); iter = timers_.begin())
			{
				//	timer的过期时间减去当前时间，如果小于等于0，则timer过期，否则，中断timer处理，返回下个timer到期的间隔
				td = iter->expiration() - now;
				if (microseconds(0) < td)
					break;

				//	从时间队列中移除
				auto spt = timers_.release(iter);
				//	执行之, 此时有可能移除timer本身
				current_run_timer = spt.get();
				spt->run();

				//	如果已经被移除了
				if (current_run_timer.valid() && spt->repeat())
				{
					//	如果是重复执行的timer, 重新启动，然后将其再插入到timers_中
					//	zxero: 这里有两种重新启动方法
					//			方法1：基于当前时间再增加一个周期间隔，这是目前使用的方法
					//			方法2：基于timer本身的过期时间增加周期间隔，这种方法可能会在超长周期中，让timer执行多次，作为tick是比较有用的
					spt->restart(now);

					//	重新加入定时器队列
					timers_.insert(spt.release());
				}
			}

			return td;
		}
	}
}

