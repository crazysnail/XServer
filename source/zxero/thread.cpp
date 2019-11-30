#include "thread.h"
#include "log.h"
#include <boost/assert.hpp>
#include <boost/make_shared.hpp>

namespace zxero
{
	//	线程数量
	boost::atomic<uint32>	thread_t::thread_count_(0);

	//
	thread_t::thread_t(const thread_function_t& func, const std::string& name)
		:name_(name)
		, thread_function_(func)
		, thread_()
	{
		ZXERO_ASSERT(func);
		ZXERO_ASSERT(!name.empty());
		LOG_TRACE << "thread_t ctor[" << this << "] " << name;
	}

	thread_t::~thread_t()
	{
		LOG_TRACE << "thread_t dtor[" << this << "] " << name_;
		//	自动析构就好了
		if (thread_)
		{
			--thread_count_;
		}
	}

	//
	void thread_t::start()
	{
		if (!thread_)
		{
			
			try {
				LOG_TRACE << "thread_t " << this << " start";
				thread_ = boost::make_shared< boost::thread >(thread_function_);
				++thread_count_;
				LOG_TRACE << "thread_t " << this << " started";
			}
			catch (std::exception& e) {
				LOG_FATAL << "thread_t " << this << " start failed: " << e.what() ;
			}
		}
	}

	int thread_t::join()
	{
		if (thread_ && thread_->joinable())
		{
			thread_->join();
			return 0;
		}
		
		return 1;
	}

	bool thread_t::startd() const
	{
		return thread_ && thread_->joinable();
	}

	thread_id_t thread_t::id() const
	{
		return thread_ ? thread_->get_id() : thread_id_t();
	}

	const std::string& thread_t::name() const
	{
		return name_;
	}

}	//	namespace zxero
