//	定长缓冲区, from muduo
#ifndef _zxero_fixed_buffer_h_
#define _zxero_fixed_buffer_h_

#include "types.h"
#include "boost/noncopyable.hpp"
#include <string.h>


namespace zxero
{
	const uint32 k_small_buffer_size = 2000;
	const uint32 k_large_buffer_size = 2000 * 2000;

	template< uint32 SIZE >
	class fixed_buffer : boost::noncopyable
	{
	public:
		fixed_buffer() :cur_(data_){ cookie(cookie_start); }
		~fixed_buffer() { cookie(cookie_end); }

		void append(const char* buf, size_t len) {
			if ((size_t)avail() > len)
			{
				memcpy(cur_, buf, len);
				cur_ += len;
			}
		}

		const char* data() const { return data_; }
		int length() const { return (int)(cur_ - data_); }

		char* current() { return cur_; }
		int avail() const { return (int)(end() - cur_); }
		void add(size_t len) { cur_ += len; }

		void reset() { cur_ = data_; }
		void bzero() { memset(data_, 0, sizeof(data_)); }

	public:
		const char* debug_string();
		void cookie(void(*c)()) { cookie_ = c; }

	private:
		const char* end() const {
			return data_ + sizeof(data_);
		}
		//	cookies 技术还没用过
		static void cookie_start();
		static void cookie_end();

		void(*cookie_)();
		char data_[SIZE];
		char* cur_;
	};
	
}



#endif	//	#ifndef _zxero_fixed_buffer_h_


