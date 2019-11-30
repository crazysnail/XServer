#include "mt_timer.h"
#include "module.h"
#include "date_time.h"
#include "../mt_player/mt_player.h"

namespace Mt
{
	
	CMyTimer::CMyTimer()
	{
		CleanUp();
	}
	CMyTimer::~CMyTimer()
	{
		CleanUp();
	}
	void CMyTimer::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<CMyTimer, ctor()>(ls, "CMyTimer")
			.def(&CMyTimer::BeginTimer, "BeginTimer")
			.def(&CMyTimer::CleanUp, "CleanUp")
			.def(&CMyTimer::IsOver,"IsOver")
			;
	}

	void CMyTimer::SetCallBack(const std::function<void (int32)> func)
	{
		call_back_ = func;
	}

	void CMyTimer::BeginTimer(int32 index, uint32 second, int32 times )
	{
		CleanUp();

		index_ = index;
		interval_ = second*1000;
		times_ = (times == -1)?INT_MAX:times;
	}
	
	void CMyTimer::CleanUp()
	{
		over_ = false;
		index_ = -1;
		elapsetime_ = 0;
		interval_ = 0;
		times_ = 0;
	}

	void CMyTimer::OnTick(zxero::uint64 elapseTime)
	{
		if (times_ <= 0) {
			return;
		}
		elapsetime_ += elapseTime;
		if (elapsetime_ > interval_ ) {
			elapsetime_ = 0;
			call_back_(index_);
			if (times_-- <= 0) {
				over_ = true;
			}
		}
	}

	static MtTimerManager* __timer_manager = nullptr;
	void MtTimerManager::lua_reg(lua_State* ls)
	{
		ff::fflua_register_t<MtTimerManager, ctor()>(ls, "MtTimerManager")
			.def(&MtTimerManager::GetYear, "GetYear")
			.def(&MtTimerManager::GetMonth, "GetMonth")
			.def(&MtTimerManager::GetDay, "GetDay")
			.def(&MtTimerManager::GetHour, "GetHour")
			.def(&MtTimerManager::GetMinute, "GetMinute")
			.def(&MtTimerManager::GetSecond, "GetSecond")

			.def(&MtTimerManager::GetYearFromUT, "GetYearFromUT")
			.def(&MtTimerManager::GetMonthFromUT, "GetMonthFromUT")
			.def(&MtTimerManager::GetDayFromUT, "GetDayFromUT")
			.def(&MtTimerManager::GetHourFromUT, "GetHourFromUT")
			.def(&MtTimerManager::GetMinuteFromUT, "GetMinuteFromUT")
			.def(&MtTimerManager::GetSecondFromUT, "GetSecondFromUT")
			.def(&MtTimerManager::GetWeekFromUT, "GetWeekFromUT")
			.def(&MtTimerManager::FromUT,"FromUT")
			.def(&MtTimerManager::DiffTimeToNow, "DiffTimeToNow")
			.def(&MtTimerManager::DiffDayToNow, "DiffDayToNow")
			.def(&MtTimerManager::DiffDayToNowEx, "DiffDayToNowEx")
			.def(&MtTimerManager::DiffWeekToNow, "DiffWeekToNow")
			.def(&MtTimerManager::DiffDayToNowByNumerDate,"DiffDayToNowByNumerDate")
			.def(&MtTimerManager::DiffDayToDateByNumerDate, "DiffDayToDateByNumerDate")			
			
			.def(&MtTimerManager::DiffTimeFromNow, "DiffTimeFromNow")
			.def(&MtTimerManager::GetWeekNumber, "GetWeekNumber")
			.def(&MtTimerManager::GetWeekDay, "GetWeekDay")
			.def(&MtTimerManager::Date2Num, "Date2Num")
			.def(&MtTimerManager::Time2Num, "Time2Num")			
			.def(&MtTimerManager::CurrentDate, "CurrentDate")
			.def(&MtTimerManager::CurrentTime, "CurrentTime")
			.def(&MtTimerManager::RunTime, "RunTime")
			.def(&MtTimerManager::DiffTime, "DiffTime")
			.def(&MtTimerManager::GetDayTime, "GetDayTime")
			.def(&MtTimerManager::GetTodayTime, "GetTodayTime")
			.def(&MtTimerManager::Num2Date, "Num2Date")
			.def(&MtTimerManager::ZeorToNow,"ZeorToNow")
			;



		ff::fflua_register_t<>(ls)
			.def(&MtTimerManager::Instance, "LuaMtTimerManager");
	}

	REGISTER_MODULE(MtTimerManager)
	{
		register_load_function(module_base::STAGE_PRE_LOAD, boost::bind(&MtTimerManager::OnLoad, __timer_manager));
	}

	MtTimerManager::MtTimerManager()
	{
		OnLoad();
		__timer_manager = this;
	}

	MtTimerManager& MtTimerManager::Instance()
	{
		return *__timer_manager;
	}

	zxero::int32 MtTimerManager::OnLoad()
	{
#if defined(ZXERO_OS_WINDOWS)
		m_StartTime = GetTickCount();
		m_CurrentTime = GetTickCount();
#elif defined(ZXERO_OS_LINUX)
		m_StartTime = 0;
		m_CurrentTime = 0;
		gettimeofday(&_tstart, &tz);
#endif
		SetTime();

		return 0;
	}


	uint64 MtTimerManager::CurrentTime()
	{
		return now_millisecond();
	}

	std::string		MtTimerManager::TimeString2DateString(std::string& timestr) 
	{
		std::vector<std::string> temp;
		boost::split(temp, timestr, boost::is_any_of(" "));
		if (temp.size() > 0) {
			return temp[0];
		}
		return "";
	}

	uint32	MtTimerManager::CurrentDate()
	{
		//SetTime();
		uint32 Date = 0;
		ConvertTU(&m_TM, Date);

		return Date;
	}

	uint32	MtTimerManager::CurrentDayDate()
	{
		uint32 Date = 0;
		ConvertDayTU(&m_TM, Date);

		return Date;
	}
	void MtTimerManager::SetTime()
	{
		m_TM = to_tm(now());
		m_SetTime = mktime(&m_TM);
	}

	// 得到标准时间
	time_t MtTimerManager::GetANSITime()
	{
		//SetTime();
		return m_SetTime;
	}

	uint32 MtTimerManager::Date2Num()
	{
		uint32 uRet = 0;

		uRet += GetYear();
		uRet = uRet * 100;

		uRet += GetMonth();
		uRet = uRet * 100;

		uRet += GetDay();

		return uRet;
	}

	uint32 MtTimerManager::Time2Num()
	{
		uint32 uRet = 0;

		uRet += GetHour();
		uRet = uRet * 100;

		uRet += GetMinute();
		uRet = uRet;

		return uRet;
	}

	uint32	MtTimerManager::Num2Date(uint32 Num)
	{
		tm TM;
		uint32 rnum = Num / 10000;
		uint32 lnum = Num % 10000;
		TM.tm_year = rnum - 1900;
		rnum = lnum / 100;
		lnum = lnum % 100;
		TM.tm_mon = rnum - 1;
		TM.tm_mday = lnum;
		uint32 date;
		ConvertTU(&TM, date);
		return date;
	}
	uint32 MtTimerManager::DiffTime(uint32 Date1, uint32 Date2)
	{
		tm S_D1, S_D2;
		ConvertUT(Date1, &S_D1);
		ConvertUT(Date2, &S_D2);
		time_t t1, t2;
		t1 = mktime(&S_D1);
		t2 = mktime(&S_D2);
		uint32 dif = (uint32)(difftime(t2, t1) * 1000);
		return dif;
	}
	uint32 MtTimerManager::DiffTimeToNow(uint32 Date)
	{
		tm S_D1;
		ConvertUT(Date, &S_D1);

		time_t t1;
		t1 = mktime(&S_D1);

		//SetTime();
		uint32 dif = (uint32)(difftime(m_SetTime, t1) );
		return dif;
	}

	uint32	MtTimerManager::DiffDayToNowByNumerDate(uint32 DateNumber)
	{
		uint16 begin_year_ = (uint16)(DateNumber / 10000);
		uint16 begin_month_ = (uint16)(DateNumber % 10000 / 100);
		uint16 begin_day_ = (uint16)(DateNumber % 100);

		uint16 end_year_ = (uint16)GetYear();
		uint16 end_month_ = (uint16)GetMonth();
		uint16 end_day_ = (uint16)GetDay();
		
		date begin{ (date::year_type)begin_year_, (date::month_type)begin_month_, (date::day_type)begin_day_ };
		date end{ (date::year_type)end_year_, (date::month_type)end_month_, (date::day_type)end_day_ };

		return(end - begin).days();

	}

	uint32	MtTimerManager::DiffDayToDateByNumerDate(uint32 BeginDateNumber, uint32 EndDateNumber)
	{
		uint16 begin_year_ = (uint16)(BeginDateNumber / 10000);
		uint16 begin_month_ = (uint16)(BeginDateNumber % 10000 / 100);
		uint16 begin_day_ = (uint16)(BeginDateNumber % 100);

		uint16 end_year_ = (uint16)(EndDateNumber / 10000);
		uint16 end_month_ = (uint16)(EndDateNumber % 10000 / 100);
		uint16 end_day_ = (uint16)(EndDateNumber % 100);

		date begin{ (date::year_type)begin_year_, (date::month_type)begin_month_, (date::day_type)begin_day_ };
		date end{ (date::year_type)end_year_, (date::month_type)end_month_, (date::day_type)end_day_ };

		return(end - begin).days();
	}
	//取得一个过去日期时间到当前的时间差（单位：天）,按日期计算 Ret = now-Data
	uint32	MtTimerManager::DiffDayToNow(uint32 Date)
	{
		tm S_D1;
		ConvertUT(Date, &S_D1);

		time_t t1, t2;
		tm date1 = { 0 }, date2 = { 0 };
		date1.tm_year = S_D1.tm_year;
		date1.tm_mon = S_D1.tm_mon;
		date1.tm_mday = S_D1.tm_mday;
		t1 = mktime(&date1);

		//
		date2.tm_year = m_TM.tm_year;
		date2.tm_mon = m_TM.tm_mon;
		date2.tm_mday = m_TM.tm_mday;
		t2 = mktime(&date2);

		uint32 dif = (uint32)(difftime(t2, t1));
		return dif / 86400;
	}
	//取得一个过去日期时间到当前的时间差（单位：天）,按累计秒数计算 Ret = now-Data
	uint32	MtTimerManager::DiffDayToNowEx(uint32 Date)
	{
		return DiffTimeToNow(Date) / 86400;
	}
	//取得一个过去日期时间到当前的时间差（单位：周）, Ret = now-Data
	uint32	MtTimerManager::DiffWeekToNow(uint32 Date)
	{
		return DiffTimeToNow(Date) / 604800;
	}

	uint32	MtTimerManager::ZeorToNow()
	{
		auto hour = GetHour();
		auto min = GetMinute();
		auto second = GetSecond();

		return hour * 60 + min * 60 + second;
	}
	uint32 MtTimerManager::DiffTimeFromNow(uint32 Date)
	{
		tm S_D1;
		ConvertUT(Date, &S_D1);
		time_t t1;
		t1 = mktime(&S_D1);

		//SetTime();
		uint32 dif = (uint32)(difftime( t1, m_SetTime));
		return dif;
	}
	uint32 MtTimerManager::TimeDateFromNow(int32 second)
	{
		time_t timedatesecond = m_SetTime + second;
		tm* ptm = localtime(&timedatesecond);
		uint32 Date = 0;
		ConvertTU(ptm, Date);
		return Date;
	}
	void MtTimerManager::ConvertUT(uint32 Date, tm* TM)
	{
		ZXERO_ASSERT(TM);
		memset(TM, 0, sizeof(tm));
		TM->tm_year = ((Date >> 26) & 0xf)+ BASE_YEAR;
		TM->tm_mon = (Date >> 22) & 0xf;
		TM->tm_mday = (Date >> 17) & 0x1f;
		TM->tm_hour = (Date >> 12) & 0x1f;
		TM->tm_min = (Date >> 6) & 0x3f;
		TM->tm_sec = (Date) & 0x3f;
	}

	void MtTimerManager::ConvertTU(tm* TM, uint32& Date)
	{
		ZXERO_ASSERT(TM);
		Date = 0;
		Date += (TM->tm_year - BASE_YEAR) & 0xf;
		Date = (Date << 4);
		Date += TM->tm_mon & 0xf;
		Date = (Date << 5);
		Date += TM->tm_mday & 0x1f;
		Date = (Date << 5);
		Date += TM->tm_hour & 0x1f;
		Date = (Date << 6);
		Date += TM->tm_min & 0x3f;
		Date = (Date << 6);
		Date += TM->tm_sec & 0x3f;
	}
	void MtTimerManager::ConvertDayTU(tm* TM, uint32& Date)
	{
		ZXERO_ASSERT(TM);
		Date = 0;
		Date += (TM->tm_year - BASE_YEAR) & 0xf;
		Date = (Date << 4);
		Date += TM->tm_mon & 0xf;
		Date = (Date << 5);
		Date += TM->tm_mday & 0x1f;
		Date = (Date << 5);
		//Date += TM->tm_hour & 0x1f;
		Date = (Date << 6);
		//Date += TM->tm_min & 0x3f;
		Date = (Date << 6);
		//Date += TM->tm_sec & 0x3f;
	}

	int32	MtTimerManager::GetWeekFromUT(uint32 Date)
	{
		uint16 y = (uint16)GetYearFromUT(Date);
		uint16 m = (uint16)GetMonthFromUT(Date);
		uint16 r = (uint16)GetDayFromUT(Date);

		boost::gregorian::date d(y, m, r);
		return d.week_number();
	}

	uint32 MtTimerManager::GetDayTime()
	{
		time_t st;
		time(&st);
		tm* ptm = localtime(&m_SetTime);

		uint32 uRet = 0;

		uRet = (ptm->tm_year - 100) * 1000;
		uRet += ptm->tm_yday;

		return uRet;
	}

	uint32 MtTimerManager::GetTodayTime()
	{
		time_t st;
		time(&st);
		tm* ptm = localtime(&m_SetTime);

		uint32 uRet = 0;

		uRet = ptm->tm_hour * 100;
		uRet += ptm->tm_min;

		return uRet;
	}

	bool MtTimerManager::FormatTodayTime(uint32& nTime)
	{
		bool ret = false;

		uint32 wHour = nTime / 100;
		uint32 wMin = nTime % 100;
		uint32 wAddHour = 0;
		if (wMin > 59)
		{
			wAddHour = wMin / 60;
			wMin = wMin % 60;
		}
		wHour += wAddHour;
		if (wHour > 23)
		{
			ret = true;
			wHour = wHour % 60;
		}

		return ret;
	}

	int32	MtTimerManager::GetWeekNumber()
	{
		return now().date().week_number();
	}

	//转换为秒数
	uint64	MtTimerManager::FromUT(uint32 Date)
	{
		tm S_D1;
		ConvertUT(Date, &S_D1);
		return  mktime(&S_D1); 
	}	
}