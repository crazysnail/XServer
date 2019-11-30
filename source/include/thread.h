#ifndef _zxero_thread_h_
#define _zxero_thread_h_

//	�����߳̿�
#include "types.h"
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/atomic.hpp>
#include <boost/chrono.hpp>

namespace zxero
{
	//	ֱ�Ӱ�boost���̹߳��ܵ��뵽zxero�����ռ���

	//	������
	using boost::mutex;
	using boost::timed_mutex;

	//	������
	using boost::condition_variable;
	using boost::condition_variable_any;

	//	�߳�id����
	typedef boost::thread::id	thread_id_t;

	//	�߳���
	class thread_t : boost::noncopyable
	{
	public:
		typedef boost::function< void() >	thread_function_t;
		//	ͨ��һ���߳�ִ�к���������һ���߳�
		thread_t(const thread_function_t& func, const std::string& name);
		~thread_t();

	public:
		//	�����߳�
		void start();

		//	�ȴ��߳̽���
		int join();

		//	����߳��Ƿ�����
		bool startd() const;

		//	��ȡ�߳�id
		thread_id_t id() const;

		//	��ȡ�߳�����
		const std::string& name() const;

		//	��ȡ�߳�����
		static uint32 thread_count() {
			return thread_count_;
		}
		
	private:
		//	�߳�����
		std::string	name_;
		//	�̺߳���
		thread_function_t	thread_function_;
		//	�̶߳���
		typedef boost::shared_ptr< boost::thread >	thread_ptr_t;
		thread_ptr_t		thread_;

		//	�̼߳���
		static boost::atomic<uint32>	thread_count_;
	};
	
	//	
	typedef boost::shared_ptr< thread_t > thread_ptr_t;

	//
	class log_message;
	log_message& operator<<(log_message& s, thread_id_t v);
}


#endif
