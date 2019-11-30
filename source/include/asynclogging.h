#ifndef MUDUO_BASE_ASYNCLOGGING_H
#define MUDUO_BASE_ASYNCLOGGING_H

#include "log.h"
#include "fixed_buffer.h"
#include "mutex.h"
#include "thread.h"
#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
namespace zxero
{

	class asynclogging : boost::noncopyable
	{
	public:
		asynclogging(const std::string& basename,
			size_t rollSize,
			int flushInterval = 3);
		~asynclogging()
		{
			if (running_)
			{ 
				stop();
			}
		}

		void start()
		{
			running_ = true;
			thread_.start();

		}

		void stop()
		{
			running_ = false;
			//cond_.notify();
			thread_.join();
		}

		void append(const char* logline, int len);


		private:
			// declare but not define, prevent compiler-synthesized functions
			asynclogging(const asynclogging&);  // ptr_container
			void operator=(const asynclogging&);  // ptr_container

			void threadFunc();

			typedef fixed_buffer<k_large_buffer_size> Buffer;
			typedef boost::ptr_vector<Buffer> BufferVector;
			typedef BufferVector::auto_type BufferPtr;

			const int flushInterval_;
			bool running_;
			mutex mutex_;
			condition_variable_any cond_;
			std::string basename_;
			size_t rollSize_;
			thread_t thread_;

			//CountDownLatch latch_;

			BufferPtr currentBuffer_;
			BufferPtr nextBuffer_;
			BufferVector buffers_;
	};
}
#endif