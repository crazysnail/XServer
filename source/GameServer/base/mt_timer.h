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
	class CMyTimer	//��ʱ��
	{
	public:
		static void lua_reg(lua_State* ls);
	public:
		CMyTimer();
		~CMyTimer();
		void SetCallBack(const std::function<void (int32)> func );
		void BeginTimer(int32 index, uint32 second, int32 times); //������֮�� �����ִ�д���[-1��ʾ����]
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
		* \brief ��ǰʱ�����ֵ����ʼֵ����ϵͳ��ͬ������
		* \return zxero::uint64 ���ص�ֵΪ�����뵥λ��ʱ��ֵ
		*/
		uint64			CurrentTime();
		//ȡ�÷������˳�������ʱ��ʱ�����ֵ
		uint32			StartTime() { return m_StartTime; };

		//����ǰ��ϵͳʱ���ʽ����ʱ���������
		void			SetTime();

		// �õ���׼ʱ��
		time_t			GetANSITime();

	public:
		//***ע�⣺
		//���½ӿڵ���û��ϵͳ���ã�ֻ���ʱ��������ڵ�����
		//

		//ȡ������ʱ��ʱ��ġ��ꡢ�¡��ա�Сʱ���֡��롢���ڵ�ֵ��
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

		//ת��Ϊ����
		uint64				FromUT(uint32 Date);
		//ȡ�ý��촦�ڱ���ȵĵڼ���
		int32			GetWeekNumber();
		//ȡ�õ�ǰ�����ڼ���0��ʾ�������죬1��6��ʾ������һ��������
		uint32			GetWeekDay() { return m_TM.tm_wday; };
		//����ǰ��ʱ�䣨�ꡢ�¡��գ�ת����һ��uint32����ʾ
		//���磺200,507,21 ��ʾ "2005.07.21"
		uint32			Date2Num();
		//����ǰ��ʱ�䣨ʱ���֡��룩ת����һ��uint32����ʾ
		//���磺2205��ʾ "22.05"
		uint32			Time2Num();
		//��20050721ת���� CurrentDate()ģʽ
		uint32			Num2Date(uint32 Num);
		//ȡ�õ�ǰ������[5bit 0-30][4bit 0-11][5bit 0-30][5bit 0-23][6bit 0-59][6bit 0-59]
		uint32			CurrentDate();
		//ȡ�õ�ǰDay����[5bit 0-30][4bit 0-11][5bit 0-30][5bit 00][6bit 00][6bit 00]
		uint32			CurrentDayDate();
		//ȡ�÷����������������ʱ�䣨���룩
		uint32			RunTime() {
			CurrentTime();
			return (m_CurrentTime - m_StartTime);
		};
		//ȡ����������ʱ���ʱ����λ�����룩, Ret = Date2-Data1
		uint32			DiffTime(uint32 Date1, uint32 Date2);
		//ȡ��һ����ȥ����ʱ�䵽��ǰ��ʱ����λ���룩, Ret = now-Data
		uint32			DiffTimeToNow(uint32 Date);	
		//ȡ��һ����ȥ����ʱ�䵽��ǰ��ʱ����λ���죩,�����ڼ��� Ret = now-Data
		uint32			DiffDayToNow(uint32 Date);
		//ȡ��һ����ȥ����[20050721����]����ǰ�յ��������λ���죩,�����ڼ��� Ret = now-Data
		uint32			DiffDayToNowByNumerDate(uint32 DateNumber);
		//ȡ����������[20050721����]���������λ���죩,�����ڼ��� Ret = now-Data
		uint32			DiffDayToDateByNumerDate(uint32 BeginDateNumber, uint32 EndDateNumber);
		//ȡ��һ����ȥ����ʱ�䵽��ǰ��ʱ����λ���죩,���ۼ��������� Ret = now-Data
		uint32			DiffDayToNowEx(uint32 Date);
		//ȡ��һ����ȥ����ʱ�䵽��ǰ��ʱ����λ���ܣ�, Ret = now-Data
		uint32			DiffWeekToNow(uint32 Date);
		//ȡ�õ�ǰ��δ��һ�����ڵ�ʱ����λ���룩, Ret = Date-now
		uint32			DiffTimeFromNow(uint32 Date);
		//ȡ����ǰN����һ�����ڣ�������N��Ǯ��
		uint32			TimeDateFromNow(int32 second);
		//ȡ��0�㵽���ڵ��ۼ�����
		uint32			ZeorToNow();

		//��һ��uint32������ת����һ��tm�ṹ
		void			ConvertUT(uint32 Date, tm* TM);
		//��һ��tm�ṹת����һ��uint32������
		void			ConvertTU(tm* TM, uint32& Date);
		//��һ��tm�ṹת����һ��uint32�����ڣ������գ�
		void			ConvertDayTU(tm* TM, uint32& Date);
		//ȡ������Ϊ��λ��ʱ��ֵ, ǧλ��������ݣ�������λ����ʱ�䣨������
		uint32			GetDayTime();
		//�õ���ǰ�ǽ����ʲôʱ��2310��ʾ23��10��
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
