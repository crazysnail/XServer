#ifndef _zxero_thread_h_
#define _zxero_thread_h_

//	基本线程库
#include "types.h"
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>

namespace zxero
{
	//	直接把boost的线程功能导入到zxero命名空间下

	//	互斥量
	using boost::mutex;
	using boost::timed_mutex;

	//	条件量
	using boost::condition_variable;
	using boost::condition_variable_any;

	//	线程id类型
	typedef boost::thread::id	thread_id_t;

	//	线程类
	class thread_t : boost::noncopyable
	{
	public:
		typedef boost::function< void() >	thread_function_t;
		//	通过一个线程执行函数来构造一个线程
		thread_t(const thread_function_t& func, const std::string& name);
		~thread_t();

	public:
		//	启动线程
		void start();

		//	等待线程结束
		int join();

		//	检查线程是否启动
		bool startd() const;

		//	获取线程id
		thread_id_t id() const;

		//	获取线程名称
		const std::string& name() const;

		//	获取线程数量
		static uint32 thread_count() {
			return thread_count_;
		}
		
	private:
		//	线程名称
		std::string	name_;
		//	线程函数
		thread_function_t	thread_function_;
		//	线程对象
		typedef boost::shared_ptr< boost::thread >	thread_ptr_t;
		thread_ptr_t		thread_;

		//	线程计数
		static boost::atomic<uint32>	thread_count_;
	};
	
	//	
	typedef boost::shared_ptr< thread_t > thread_ptr_t;

	//
	class log_message;
	log_message& operator<<(log_message& s, thread_id_t v);
}


#endif
