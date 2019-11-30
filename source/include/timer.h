//	定时器，from muduo
#ifndef _zxero_network_timer_h_
#define _zxero_network_timer_h_

#include "types.h"
#include "date_time.h"
#include <boost/atomic.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace zxero
{
	namespace network
	{
		class timer : boost::noncopyable
		{
		public:
			timer(const boost::function<void()>& cb, timestamp_t when, time_duration interval);

			void run() const;

		public:
			timestamp_t expiration() const;
			bool repeat() const;
			void restart(timestamp_t);

		public:
			//	比较器，用于timer在容器中排序
			bool operator < (const timer& r) const;

		public:
			static uint64 num_created();

		private:
			const boost::function<void()> callback_;
			timestamp_t expiration_;
			const time_duration interval_;
			const bool repeat_;

			static boost::atomic<uint64> s_num_created_;
		};
	}
}

#endif	//	#ifndef _zxero_network_timer_h_

