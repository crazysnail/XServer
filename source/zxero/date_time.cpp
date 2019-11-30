#include <boost/date_time.hpp>
#include "date_time.h"


namespace zxero
{
	//timestamp_t ttt;
	bool ttt_set = false;
	time_duration ddd;
	bool gm_mode = false;

	void set_time_duration(time_duration d)
	{
		ttt_set = true;
		ddd = d;
	}

	void add_time_duration(time_duration d)
	{
		ttt_set = true;
		ddd += d;
	}

	void reset_time_duration()
	{
		ttt_set = false;
	}

	timestamp_t now()
	{
		if (gm_mode)
		{
			if (ttt_set)
			{
				return microsec_clock::local_time() + ddd;
			}
			else
			{
				return microsec_clock::local_time();
			}
		}
		else
		{
			return microsec_clock::local_time();
		}
	}


	uint64 sss;
	bool sss_set = false;
	uint64 now_second()
	{
		if (gm_mode)
		{
			if (sss_set)
			{
				return sss;
			}
			else
			{
				return now_millisecond() / 1000;
			}
		}
		else
		{
			return now_millisecond() / 1000;
		}
	}

	//距离1970年1月1日N秒之后的 是X年X月X日
	date_t seconds_transform_date(uint64 seconds)
	{
		static ptime beginTime(date(1970,1,1));
		time_duration td(0,0,(uint32)seconds);
		ptime endtime = beginTime+td;
		return endtime.date();
	}

	ptime seconds_transform_ptime(uint64 seconds)
	{
		static ptime beginTime(date(1970,1,1));
		time_duration td(0,0, (uint32)seconds);
		ptime endtime = beginTime+td;
		return endtime;
	}

	uint64 now_millisecond()
	{
		//static ptime beginTime(min_date_time);
		static ptime beginTime(date(1970,1,1));
		return (now() - beginTime).total_milliseconds();
	}
}
