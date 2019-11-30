//	导出时间相关的函数和头文件，方便使用
#ifndef _zxero_time_h_
#define _zxero_time_h_

#include "types.h"
#include <boost/date_time.hpp>

namespace zxero
{
	using namespace boost::gregorian;
	using namespace boost::posix_time;

	typedef boost::posix_time::ptime			timestamp_t;
	typedef boost::gregorian::date				date_t;
	//	获取当前时间
	timestamp_t now();

	//	获取当前时间的秒值
	uint64 now_second();
	//	获取当前时间的毫秒值
	uint64 now_millisecond();
	//距离1970年1月1日N秒之后的 是X年X月X日
	date_t seconds_transform_date(uint64 seconds);
	//距离1970年1月1日N秒之后的 ptime 结构
	ptime seconds_transform_ptime(uint64 seconds);
	extern uint64 sss;
	extern bool sss_set;
	extern bool ttt_set;
	extern bool gm_mode;
	extern timestamp_t ttt;
	void set_time_duration(time_duration d);
	void add_time_duration(time_duration d);
	void reset_time_duration();

	//typedef boost::gregorian::date_duration		date_duration_t;

	//typedef boost::gregorian::days				days_t;
	//typedef boost::gregorian::months			months_t;
	//typedef boost::gregorian::weeks				weeks_t;

	//typedef boost::gregorian::day_clock			day_clock_t;


	//
	//typedef boost::posix_time::time_duration	time_duration_t;

	//typedef boost::posix_time::second_clock		second_clock_t;
	//typedef boost::posix_time::microsec_clock	microsec_clock_t;


	//using boost::gregorian::pos_infin;
	//using boost::gregorian::neg_infin;
	//using boost::gregorian::not_a_date_time;

	//using boost::gregorian::max_date_time;
	//using boost::gregorian::min_date_time;

	//using boost::gregorian::from_simple_string;
	//using boost::gregorian::from_string;
	//using boost::gregorian::from_undelimited_string;

	//using boost::gregorian::to_simple_string;
	//using boost::gregorian::to_iso_string;
	//using boost::gregorian::to_iso_extended_string;

	//using boost::gregorian::to_tm;
	//using boost::gregorian::date_from_tm;
}


#endif	//	#ifndef _zxero_time_h_

