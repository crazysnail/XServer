#include "event_loop_thread.h"
#include "event_loop.h"
#include "log.h"
#ifndef ZXERO_OS_WINDOWS
#include <signal.h>
#endif

namespace zxero
{
	namespace network
	{
		event_loop_thread::event_loop_thread(const thread_init_callback_t& cb)
			:loop_(nullptr)
			, exiting_(false)
			, thread_(boost::bind(&event_loop_thread::thread_function, this), "event_loop_thread")
			, mutex_()
			, cond_()
			, callback_(cb)
		{

		}

		event_loop_thread::~event_loop_thread()
		{
			exiting_ = true;
			if (loop_ != nullptr)
			{
				loop_->quit();
				thread_.join();
			}
		}

		event_loop* event_loop_thread::start_loop()
		{
			ZXERO_ASSERT(!thread_.startd());

			#ifndef ZXERO_OS_WINDOWS
			sigset_t signal_mask;
			sigemptyset (&signal_mask);
			sigaddset (&signal_mask, SIGPIPE);
			int rc = pthread_sigmask (SIG_BLOCK, &signal_mask, NULL);
			if (rc != 0) {
				printf("block sigpipe error\n");
			}
			#endif
			thread_.start();

			{
				mutex::scoped_lock guard(mutex_);
				while (loop_ == nullptr)
				{
					cond_.wait(mutex_);
				}
			}

			return loop_;
		}

		void event_loop_thread::thread_function()
		{
			event_loop loop;
			if (callback_)
				callback_(&loop);

			{
				mutex::scoped_lock guard(mutex_);
				loop_ = &loop;
				cond_.notify_all();
			}

			loop.loop();
			loop_ = nullptr;
		}
	}
}

