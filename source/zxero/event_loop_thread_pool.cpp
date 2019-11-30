#include "event_loop_thread_pool.h"
#include "event_loop.h"
#include "event_loop_thread.h"
#include "log.h"

namespace zxero
{
	namespace network
	{
		event_loop_thread_pool::event_loop_thread_pool(event_loop* base_loop)
			:base_loop_(base_loop)
			, started_(false)
			, num_threads_(0)
			, next_(0)
		{
		}

		event_loop_thread_pool::~event_loop_thread_pool()
		{
		}

		void event_loop_thread_pool::start(uint32 num_thread, const thread_init_callback_t& cb)
		{
			ZXERO_ASSERT(!started_);
			base_loop_->assert_in_loop_thread();

			num_threads_ = num_thread;
			started_ = true;


			for (uint32 i = 0; i < num_threads_; ++i)
			{
				event_loop_thread* t = new event_loop_thread(cb);
				threads_.push_back(t);
				loops_.push_back(t->start_loop());
			}
			if (num_threads_ == 0 && cb)
			{
				cb(base_loop_);
			}
		}

		event_loop* event_loop_thread_pool::next_loop()
		{
			base_loop_->assert_in_loop_thread();
			event_loop* loop = base_loop_;

			if (!loops_.empty())
			{
				loop = loops_[next_];
				++next_;
				if (next_ >= loops_.size())
					next_ = 0;
			}

			return loop;
		}
	}
}
