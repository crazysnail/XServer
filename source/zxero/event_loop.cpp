#include "event_loop.h"
#include "select_poller.h"
#include "channel.h"
#include "timer_queue.h"
#include <numeric>
#include "poller.h"
#include "log.h"
#include <boost/assert.hpp>

namespace zxero
{
	namespace network
	{
		boost::thread_specific_ptr<event_loop>	t_loop_in_this_thread(nullptr);

		event_loop* event_loop::get_event_loop_of_current_thread()
		{
			return t_loop_in_this_thread.get();
		}

		event_loop::event_loop()
			:looping_(false)
			, quit_(false)
			, event_handling_(false)
			, thread_id_(boost::this_thread::get_id())
			, poller_(poller::new_default_poller(this))
			, timer_queue_(new timer_queue(this))
			, current_active_channel_(nullptr)
		{
			pending_functors_.reserve(200);
			pending_functors_temp_.reserve(200);
			LOG_TRACE << "event_loop ctro[" << this << "] in thread " << thread_id_;
			if (t_loop_in_this_thread.get())
			{
				LOG_FATAL << "another event loop " << t_loop_in_this_thread.get() << " exists in this thread " << thread_id_;
			}
			else
			{
				t_loop_in_this_thread.reset(this);
			}
		}

		event_loop::~event_loop()
		{
			ZXERO_ASSERT(!looping_);
			t_loop_in_this_thread.reset();
			LOG_TRACE << "event_loop ctro[" << this << "]";
		}

		void event_loop::loop()
		{
			ZXERO_ASSERT(!looping_);
			assert_in_loop_thread();
			looping_ = true;
			quit_ = false;

			LOG_INFO << "event loop " << this << " start looping";
			while (!quit_)
				{
					active_channels_.clear();

					//	根据timer最早激活的时间设定poller的超时时间，但最大为5ms
					//	注意这里开始就进行了一次timer调用
					time_duration td = timer_queue_->check_expired_timer(now());
					if (td > seconds(5))
					{
						LOG_DEBUG << "#TICK# loop check_expired_timer :" << td.total_seconds();
					}

					if (td == microseconds(0) || td > microseconds(5000))
						td = microseconds(5000);
					uint64 test_start_polltick = now_millisecond();
					timestamp_t overtime = poller_->poll(td, active_channels_);
					uint64 test_end_polltick = now_millisecond();
					uint64 pollelapseTime = test_end_polltick - test_start_polltick;
					if (pollelapseTime > 40)
					{
						efficency_.push_back(int32(pollelapseTime));
						std::sort(efficency_.begin(), efficency_.end(), std::greater<int32>());
						if (efficency_.size() > 10) {
							efficency_.pop_back();
						}
						LOG_DEBUG << "#TICK# loop poll elapseTime:" << pollelapseTime << " active_channels_:" << active_channels_.size();
					}
					//	先timer还是先handler这是一个选择问题
					//	先timer的话，timer还可以认为比较准时，因为是poll之后立刻执行了
					////	然后再处理事件前又调用了一次timer
					//timer_queue_->check_expired_timer(overtime);

					//	处理所有激活channel的事件
					event_handling_ = true;
					for (channel* c : active_channels_)
					{
						current_active_channel_ = c;
						current_active_channel_->handle_event(overtime);
					}
					current_active_channel_ = nullptr;
					event_handling_ = false;

					//	根据overtime更新所有timer_queue的过期情况，并调用相应的callback
					//	后timer的话，存在一个认识问题，其实如果把io事件处理和timer都认为是同一时间发生的话
					//	这两个调用点都一样
					//	zxero：目前先放在后面了，跑跑看看效果
					//	注意：依然是以poll结束的时间算的哦，回头可以考虑要不要以now处理
					timer_queue_->check_expired_timer(overtime);

					//	执行队列函数
					do_pending_functors();
				}

			LOG_INFO << "event loop " << this << " stop looping";
			looping_ = false;
		}

		bool event_loop::is_in_loop_thread() const
		{
			return thread_id_ == boost::this_thread::get_id();
		}

		void event_loop::assert_in_loop_thread()
		{
			ZXERO_ASSERT(is_in_loop_thread()) << this;
		}

		//
		void event_loop::do_pending_functors()
		{
			{
				mutex::scoped_lock lock(mutex_);
				pending_functors_temp_.swap(pending_functors_);
			}

			for (functor_t& cb : pending_functors_temp_)
			{
				try
				{
					cb();
				}
				catch (std::exception& e)
				{
					LOG_ERROR << e.what();
				}
				
			}
			pending_functors_temp_.clear();
		}

		void event_loop::quit(/*bool wait_for_quit*/)
		{
			quit_ = true;

			//if (wait_for_quit)
			//{
			//	//	非事件循环可等待线程结束
			//	if (!is_in_loop_thread())
			//	{
			//	}
			//}
		}

		////
		timer_id event_loop::run_at(timestamp_t time, const boost::function<void()>& cb)
		{
			return timer_queue_->add_timer(cb, time, microseconds(0));
		}
		timer_id event_loop::run_after(time_duration td, const boost::function<void()>& cb)
		{
			return run_at(now() + td, cb);
		}
		timer_id event_loop::run_every(time_duration td, const boost::function<void()>& cb)
		{
			return timer_queue_->add_timer(cb, now() + td, td);
		}

		void event_loop::cancel(timer_id tid)
		{
			timer_queue_->cancel(tid);
		}

		////
		void event_loop::run_in_loop(const functor_t& cb)
		{
			if (is_in_loop_thread())
			{ 
				cb();
			}
			else
				queue_in_loop(cb);
		}

		void event_loop::queue_in_loop(const functor_t& cb)
		{
			ZXERO_ASSERT(cb);
			mutex::scoped_lock guard(mutex_);
			pending_functors_.push_back(cb);
		}


		////
		void event_loop::update_channel(channel* c)
		{
			ZXERO_ASSERT(c->owner_loop() == this);
			assert_in_loop_thread();
			poller_->update_channel(c);
		}

		void event_loop::remove_channel(channel* c)
		{
			ZXERO_ASSERT(c->owner_loop() == this);
			assert_in_loop_thread();

			poller_->remove_channel(c);
		}

		zxero::int32 event_loop::efficency() const
		{
			if (efficency_.empty()) {
				return -1;
			} else {
				int32 total = std::accumulate(efficency_.begin(), efficency_.end(), 0);
				return int32(total / efficency_.size());
			}
		}

	}
}

