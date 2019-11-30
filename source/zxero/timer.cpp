#include "timer.h"
#include "log.h"

namespace zxero
{
	namespace network
	{
		boost::atomic<uint64> timer::s_num_created_(0);

		timer::timer(const boost::function<void()>& cb, timestamp_t when, time_duration interval)
			:callback_(cb)
			, expiration_(when)
			, interval_(interval)
			, repeat_(interval.total_microseconds() > 0)
		{
			++s_num_created_;
		}

		void timer::run() const
		{
			try
			{
				callback_();
			}
			catch (std::exception& e)
			{
				LOG_ERROR << e.what();
			}
		}

		timestamp_t timer::expiration() const
		{
			return expiration_;
		}

		bool timer::repeat() const
		{
			return repeat_;
		}

		void timer::restart(timestamp_t when)
		{
			if (repeat_)
			{
				expiration_ = when + interval_;
			}
			else
			{
				expiration_ = boost::gregorian::not_a_date_time;
			}
		}

		bool timer::operator < (const timer& r) const {
			if (expiration_ == r.expiration_)
				return this < &r;

			return expiration_ < r.expiration_;
		}

		//
		uint64 timer::num_created()
		{
			return s_num_created_;
		}
	}
}

