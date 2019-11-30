#ifndef _zxero_log_h_
#define _zxero_log_h_

#include <boost/shared_ptr.hpp>
#include "fixed_buffer.h"
#include <vector>
#include <utility>
#include <map>


//	参考muduo的异步日志库
namespace zxero
{
	class log_finisher;

	enum LogLevel
	{
		LL_TRACE,
		LL_DEBUG,
		LL_INFO,
		LL_WARN,
		LL_ERROR,
		LL_FATAL,
		LL_BI,
		LL_DB,
		NUM_LOG_LEVELS,
	};

	enum LogDomain
	{
		LD_DEFAULT = 0,

		//	最大日志域
		MAX_LOG_DOMAIN = 64,
	};

	class log_message
	{
	public:
		class source_file
		{
		public:
			template< int N >
			source_file(const char(&arr)[N])
				:data_(arr)
				, size_(N - 1)
			{
				const char* slash = strrchr(data_, '\\');
				if (slash)
				{
					data_ = slash + 1;
					size_ -= (int)(data_ - arr);
				}
				slash = strrchr(data_, '/');
				if (slash)
				{
					data_ = slash;
					size_ -= (int)(data_ - arr);
				}
			}

			explicit source_file(const char* filename)
				:data_(filename)
			{
				const char* slash = strrchr(filename, '\\');
				if (slash)
				{
					data_ = slash + 1;
				}
				slash = strrchr(filename, '/');
				if (slash)
				{
					data_ = slash;
				}
				size_ = int(strlen(data_));
			}

			const char* data_;
			int size_;
		};

	public:
		typedef fixed_buffer<k_small_buffer_size> buffer_t;

		//	普通消息
		log_message(LogLevel level, source_file filename, int line, LogDomain ld = LD_DEFAULT, const char* exp = "log without exp");
		//	调试用消息
		log_message(LogLevel level, source_file filename, int line, const char* func, LogDomain ld = LD_DEFAULT);

	public:
		log_message& operator<<(bool v);
		log_message& operator<<(int16);
		log_message& operator<<(uint16);
		log_message& operator<<(int32);
		log_message& operator<<(uint32);
		log_message& operator<<(int64);
		log_message& operator<<(uint64);
		//log_message& operator<<(long);
		//log_message& operator<<(unsigned long);

		log_message& operator<<(const void*);
		log_message& operator<<(real32);
		log_message& operator<<(real64);
		log_message& operator<<(char);
		template<typename First, typename Second>
		log_message& operator<<(const std::pair<First, Second>& pair)
		{
			return *this << "(" << pair.first << "," << pair.second << ")";
		}
		template<typename Ele>
		log_message& operator<<(const std::vector<Ele>& vec)
		{
			*this << "[";
			for (auto& e : vec) {
				*this << e << ",";
			}
			return *this << "]";
		}
		template<typename Key, typename Val>
		log_message& operator<<(const std::map<Key, Val>& map) 
		{
			*this << "{";
			for (auto& p : map) {
				*this << p.first;
				*this << ":";
				*this << p.second;
				*this << ",";
			}
			return *this << "}";
		}
		template < typename T >
		log_message& operator<<(const boost::shared_ptr<T>& ptr)
		{
			*this << ptr.get();
			return *this;
		}
		template < typename T >
		log_message& operator<<(const std::unique_ptr<T>& ptr)
		{
			*this << ptr.get();
			return *this;
		}
		log_message& operator<<(const boost::shared_ptr<void>&);
		log_message& operator<<(const char*);
		log_message& operator<<(const std::string&);
		//log_message& operator<<(const std::string&);

	public:
		void append(const char* data, int len);
		const buffer_t& buffer() const;
		void reset();

	public:
		static LogLevel log_level();
		static void log_level(LogLevel ll);

	private:
		friend class log_finisher;
		void finish();

		void build_header();
		void format_time();

	private:
		template< typename T >
		void format_integer(T);

		LogLevel level_;
		int line_;
		source_file basename_;
		uint64 time_;
		const char* exp_;
		
		buffer_t buffer_;

		static const int k_max_numeric_size = 32;
	public:
		typedef void (*OutputFunc)(const char* msg, int len);
		typedef void (*FlushFunc)();
		static void setOutput(OutputFunc);
		static void setFlush(FlushFunc);
		static void setBiOutput(OutputFunc);
		static void setBiFlush(FlushFunc);
		static void setDBOutput(OutputFunc);
		static void setDBFlush(FlushFunc);
		static void setERROROutput(OutputFunc);
		static void setERRORFlush(FlushFunc);

	};

	class log_finisher
	{
	public:
		void operator=(log_message& msg);
	};
	extern int32 G_Server_Time_HoursOfDay;
	extern int32 G_Server_Time_DayOfYear;
	extern int32 G_Server_Time_DayOfWeek;
	extern int32 G_Server_Time_WeekOfYear;
	extern int32 G_Server_Time_MonthOfYear;

	extern int32 G_Server_Time_Min;
	extern int32 G_Server_Time_Second;

#define LOG(LEVEL,FILE_,LINE_) \
	zxero::log_finisher() = zxero::log_message(LEVEL, FILE_, LINE_)
#define LOG_IF(LEVEL, CONDITION)	\
	!(CONDITION) ? (void)0 : LOG(LEVEL,__FILE__,__LINE__)
#define LOG_EXP(LEVEL, FILE_, LINE_, EXPRESSION) \
	zxero::log_finisher() = zxero::log_message(LEVEL, FILE_, LINE_, zxero::LogDomain::LD_DEFAULT, #EXPRESSION)

#define CHECK(EXPRESSION) \
	LOG_IF(zxero::LL_FATAL, !(EXPRESSION)) << "CHECK failed: " #EXPRESSION ": "
#define CHECK_EQ(A, B) CHECK((A) == (B))
#define CHECK_NE(A, B) CHECK((A) != (B))
#define CHECK_LT(A, B) CHECK((A) <  (B))
#define CHECK_LE(A, B) CHECK((A) <= (B))
#define CHECK_GT(A, B) CHECK((A) >  (B))
#define CHECK_GE(A, B) CHECK((A) >= (B))

#ifdef ZXERO_OS_WINDOWS
#define LOG_TRACE if (zxero::log_message::log_level() <= zxero::LL_TRACE) \
	zxero::log_finisher() = zxero::log_message(zxero::LL_TRACE, __FILE__, __LINE__, __FUNCTION__)
#define LOG_DEBUG if (zxero::log_message::log_level() <= zxero::LL_DEBUG) \
	zxero::log_finisher() = zxero::log_message(zxero::LL_DEBUG, __FILE__, __LINE__, __FUNCTION__)
#define ZXERO_ASSERT( EXPRESSION ) \
	!!(EXPRESSION) ? (void)0 : LOG_EXP(zxero::LL_FATAL,__FILE__,__LINE__, EXPRESSION) << "exp [" << #EXPRESSION << "] asseration failed at [" << __FUNCTION__ << "] of [" << __LINE__ << ":" << __FILE__ << "] "
#else
#define LOG_TRACE if (zxero::log_message::log_level() <= zxero::LL_TRACE) \
	zxero::log_finisher() = zxero::log_message(zxero::LL_TRACE, __FILE__, __LINE__, __func__)
#define LOG_DEBUG if (zxero::log_message::log_level() <= zxero::LL_DEBUG) \
	zxero::log_finisher() = zxero::log_message(zxero::LL_DEBUG, __FILE__, __LINE__, __func__)
#define ZXERO_ASSERT( EXPRESSION ) \
	!!(EXPRESSION) ? (void)0 : LOG_EXP(zxero::LL_FATAL,__FILE__,__LINE__, EXPRESSION) << "exp [" << #EXPRESSION << "] asseration failed at [" << __func__ << "] of [" << __LINE__ << ":" << __FILE__ << "] "
#endif
#define LOG_INFO if (zxero::log_message::log_level() <= zxero::LL_INFO) \
	LOG(zxero::LL_INFO,__FILE__,__LINE__)
#define LOG_WARN LOG(zxero::LL_WARN,__FILE__,__LINE__)
#define LOG_ERROR LOG(zxero::LL_ERROR,__FILE__,__LINE__)
#define LOG_FATAL LOG(zxero::LL_FATAL,__FILE__,__LINE__)
#define LOG_BI LOG(zxero::LL_BI,__FILE__,__LINE__)
#define LOG_DB(fn_,ln_) \
	LOG(zxero::LL_DB,fn_,ln_)
}	//	namespace zxero


#endif	//	#ifndef _zxero_log_h_
