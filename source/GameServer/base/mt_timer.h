#ifndef MTONLINE_GAMESERVER_MT_TIMER_H__
#define MTONLINE_GAMESERVER_MT_TIMER_H__
#include "../mt_types.h"

#ifdef ZXERO_OS_LINUX
#include <sys/time.h>
#endif
namespace Mt
{
	using namespace zxero;
	const int BASE_YEAR = 115;
	class CMyTimer	//计时器
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		CMyTimer();
		~CMyTimer();
		void SetCallBack(const std::function<void (int32)> func );
		void BeginTimer(int32 index, uint32 second, int32 times); //多少秒之后 ，间隔执行次数[-1表示无限]
		void CleanUp();
		void OnTick(zxero::uint64 elapseTime);
		bool IsOver() { return over_; };
	private:
		int32 times_;
		uint64 elapsetime_;
		uint64 interval_;
		int32 index_;
		std::function<void (int32)> call_back_;
		bool over_ = false;
	};
	class MtTimerManager : public boost::noncopyable
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		static MtTimerManager& Instance();
		MtTimerManager();
		zxero::int32 OnLoad();
		/**
		* \brief 当前时间计数值，起始值根据系统不同有区别
		* \return zxero::uint64 返回的值为：毫秒单位的时间值
		*/
		uint64			CurrentTime();
		//取得服务器端程序启动时的时间计数值
		uint32			StartTime() { return m_StartTime; };

		//将当前的系统时间格式化到时间管理器里
		void			SetTime();

		// 得到标准时间
		time_t			GetANSITime();

	public:
		//***注意：
		//以下接口调用没有系统调用，只针对时间管理器内的数据
		//

		//取得设置时间时候的“年、月、日、小时、分、秒、星期的值”
		int32				GetYear() { return m_TM.tm_year + 1900; };	//[1900,????]
		int32				GetMonth() { return m_TM.tm_mon + 1; };		//[1,12]
		int32				GetDay() { return m_TM.tm_mday; };		//[1,31]
		int32				GetHour() { return m_TM.tm_hour; };		//[0,23]
		int32				GetMinute() { return m_TM.tm_min; };	//[0,59]
		int32				GetSecond() { return m_TM.tm_sec; };	//[0,59]

		int32				GetYearFromUT(uint32 Date) { return ((Date >> 26) & 0xf)+BASE_YEAR+1900; };		 //[1900,????]
		int32				GetMonthFromUT(uint32 Date) { return (((Date >> 22) & 0xf) + 1); };//[1,12]
		int32				GetDayFromUT(uint32 Date) { return (Date >> 17) & 0x1f; };		 //[1,31]
		int32				GetHourFromUT(uint32 Date) { return (Date >> 12) & 0x1f; };		 //[0,23]
		int32				GetMinuteFromUT(uint32 Date) { return (Date >> 6) & 0x3f; };	 //[0,59]
		int32				GetSecondFromUT(uint32 Date) { return Date  & 0x3f; };			 //[0,59]
		int32				GetWeekFromUT(uint32 Date);

		//转换为秒数
		uint64				FromUT(uint32 Date);
		//取得今天处于本年度的第几周
		int32			GetWeekNumber();
		//取得当前是星期几；0表示：星期天，1～6表示：星期一～星期六
		uint32			GetWeekDay() { return m_TM.tm_wday; };
		//将当前的时间（年、月、日）转换成一个uint32来表示
		//例如：200,507,21 表示 "2005.07.21"
		uint32			Date2Num();
		//将当前的时间（时、分、秒）转换成一个uint32来表示
		//例如：2205表示 "22.05"
		uint32			Time2Num();
		//把20050721转换成 CurrentDate()模式
		uint32			Num2Date(uint32 Num);
		//取得当前的日期[5bit 0-30][4bit 0-11][5bit 0-30][5bit 0-23][6bit 0-59][6bit 0-59]
		uint32			CurrentDate();
		//取得当前Day日期[5bit 0-30][4bit 0-11][5bit 0-30][5bit 00][6bit 00][6bit 00]
		uint32			CurrentDayDate();
		//取得服务器启动后的运行时间（毫秒）
		uint32			RunTime() {
			CurrentTime();
			return (m_CurrentTime - m_StartTime);
		};
		//取得两个日期时间的时间差（单位：毫秒）, Ret = Date2-Data1
		uint32			DiffTime(uint32 Date1, uint32 Date2);
		//取得一个过去日期时间到当前的时间差（单位：秒）, Ret = now-Data
		uint32			DiffTimeToNow(uint32 Date);	
		//取得一个过去日期时间到当前的时间差（单位：天）,按日期计算 Ret = now-Data
		uint32			DiffDayToNow(uint32 Date);
		//取得一个过去日期[20050721类型]到当前日的天数差（单位：天）,按日期计算 Ret = now-Data
		uint32			DiffDayToNowByNumerDate(uint32 DateNumber);
		//取得两个日期[20050721类型]的天数差（单位：天）,按日期计算 Ret = now-Data
		uint32			DiffDayToDateByNumerDate(uint32 BeginDateNumber, uint32 EndDateNumber);
		//取得一个过去日期时间到当前的时间差（单位：天）,按累计秒数计算 Ret = now-Data
		uint32			DiffDayToNowEx(uint32 Date);
		//取得一个过去日期时间到当前的时间差（单位：周）, Ret = now-Data
		uint32			DiffWeekToNow(uint32 Date);
		//取得当前到未来一个日期的时间差（单位：秒）, Ret = Date-now
		uint32			DiffTimeFromNow(uint32 Date);
		//取到当前N秒后的一个日期（负的是N秒钱）
		uint32			TimeDateFromNow(int32 second);
		//取得0点到现在的累计秒数
		uint32			ZeorToNow();

		//将一个uint32的日期转换成一个tm结构
		void			ConvertUT(uint32 Date, tm* TM);
		//将一个tm结构转换成一个uint32的日期
		void			ConvertTU(tm* TM, uint32& Date);
		//将一个tm结构转换成一个uint32的日期（年月日）
		void			ConvertDayTU(tm* TM, uint32& Date);
		//取得已天为单位的时间值, 千位数代表年份，其他三位代表时间（天数）
		uint32			GetDayTime();
		//得到当前是今天的什么时间2310表示23点10分
		uint32			GetTodayTime();
		bool			FormatTodayTime(uint32& nTime);

		std::string		UTimeString(uint32 Date) {
			tm TM;
			ConvertUT(Date, &TM);
			char buffer[64] = { 0 };
			strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", &TM);
			return buffer;
		}

		std::string		UDateString(uint32 Date) {
			tm TM;
			ConvertUT(Date, &TM);
			char buffer[64] = { 0 };
			strftime(buffer, 64, "%Y-%m-%d", &m_TM);
			return buffer;
		}
		std::string		NowTimeString() {
			char buffer[64] = { 0 };
			strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", &m_TM);
			return buffer;
		}

		std::string		NowDateString() {
			char buffer[64] = { 0 };
			strftime(buffer, 64, "%Y-%m-%d", &m_TM);
			return buffer;
		}
		std::string		TimeString2DateString(std::string& timestr);

	public:


		uint32			m_StartTime;
		uint32			m_CurrentTime;
		time_t			m_SetTime;
		tm				m_TM;
#ifdef ZXERO_OS_LINUX
		struct timeval _tstart, _tend;
		struct timezone tz;
#endif
		
	};	

}
#endif // MTONLINE_GAMESERVER_MT_TIMER_H__
