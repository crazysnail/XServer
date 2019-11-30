#include <stdio.h> 
#include "log.h"
#include "thread.h"
#include "date_time.h"


namespace zxero
{
	int32 G_Server_Time_HoursOfDay = 0;
	int32 G_Server_Time_DayOfYear = 0;
	int32 G_Server_Time_WeekOfYear = 0;
	int32 G_Server_Time_MonthOfYear = 0;
	int32 G_Server_Time_DayOfWeek = 0;
	int32 G_Server_Time_Min = 0;
	int32 G_Server_Time_Second = 0;
	namespace detail
	{
		const char digits[] = "9876543210123456789";
		const char* zero = digits + 9;
		BOOST_STATIC_ASSERT(sizeof(digits) == 20);

		const char digitsHex[] = "0123456789ABCDEF";
		BOOST_STATIC_ASSERT(sizeof(digitsHex) == 17);

		template< typename T>
		size_t convert(char buf[], T value)
		{
			T i = value;
			char* p = buf;

			do
			{
				int lsd = (int)(i % 10);
				i /= 10;
				//	zxero: 这种写法的确够诡异的，居然还利用负索引。。。。啧啧啧
				*p++ = zero[lsd];
			} while( i != 0 );

			if ( value < 0 )
			{
				*p++ = '-';
			}
			*p = '\0';
			std::reverse(buf, p);

			return p - buf;
		}

		size_t convertHex(char buf[], uintptr value)
		{
			uintptr i = value;
			char* p = buf;

			do
			{
				int lsd = i % 16;
				i /= 16;
				*p++ = digitsHex[lsd];
			} while( i != 0);

			*p = '\0';
			std::reverse(buf, p);

			return p - buf;
		}
	}

	//
	template< typename T >
	void log_message::format_integer( T v )
	{
		if ( buffer_.avail() >= k_max_numeric_size )
		{
			size_t len = detail::convert(buffer_.current(), v);
			buffer_.add(len);
		}
	}

	void log_message::append(const char* data, int len)
	{
		buffer_.append(data, len);
	}

	const log_message::buffer_t& log_message::buffer() const {
		return buffer_;
	}

	void log_message::reset()
	{
		buffer_.reset();
	}

	////
	log_message& log_message::operator<<(bool v)
	{
		buffer_.append( v ? "1" : "0", 1 );
		return *this;
	}

	log_message& log_message::operator<<(char v)
	{
		buffer_.append( &v, 1 );
		return *this;
	}

	log_message& log_message::operator<<(const char* v)
	{
		if (v == nullptr)
			return *this;
		buffer_.append( v, strlen(v) );
		return *this;
	}

	log_message& log_message::operator<<(const std::string& str)
	{
		buffer_.append( str.c_str(), str.size() );
		return *this;
	}

	//log_message& log_message::operator<<(const std::string& str)
	//{
	//	buffer_.append( str.c_str(), str.size() );
	//	return *this;
	//}

	log_message& log_message::operator<<(int16 v)
	{
		*this << (int32)v;
		return *this;
	}

	log_message& log_message::operator<<(uint16 v)
	{
		*this << (uint32)v;
		return *this;
	}

	log_message& log_message::operator<<(int32 v)
	{
		format_integer( v );
		return *this;
	}

	log_message& log_message::operator<<(uint32 v)
	{
		format_integer( v );
		return *this;
	}

	log_message& log_message::operator<<(int64 v)
	{
		format_integer( v );
		return *this;
	}

	log_message& log_message::operator<<(uint64 v)
	{
		format_integer( v );
		return *this;
	}

	//log_message& log_message::operator<<(long v)
	//{
	//	format_integer( v );
	//	return *this;
	//}

	//log_message& log_message::operator<<(unsigned long v)
	//{
	//	format_integer( v );
	//	return *this;
	//}

	log_message& log_message::operator<<(const void* p)
	{
		*this << "[0X";
		uintptr v = (uintptr)p;
		if ( buffer_.avail() >= k_max_numeric_size )
		{
			char* buf = buffer_.current();
			size_t len = detail::convertHex( buf, v );
			buffer_.add(len);
		}
		*this << "]";
		return *this;
	}

	log_message& log_message::operator<<(float f)
	{
		*this << (double)f;
		return *this;
	}

	log_message& log_message::operator<<(double d)
	{
		if ( buffer_.avail() >= k_max_numeric_size )
		{
			int len = zprintf( buffer_.current(), k_max_numeric_size, "%.12g", d );
			buffer_.add(len);
		}
		return *this;
	}

	log_message& operator<<(log_message& s, thread_id_t t)
	{
		//std::stringstream str_stream;
		//str_stream << t;
		//s << str_stream.str() << ' ';
	  void* ptr = &t;
	  uintptr* uptr = (uintptr*)ptr;
		s << (uint32)*uptr  << ' ';
		return s;
	}


	////
	static LogLevel sgLogLevel = LL_INFO;

	LogLevel log_message::log_level()
	{
		return sgLogLevel;
	}

	void log_message::log_level( LogLevel ll )
	{
		sgLogLevel = ll;
	}

	////
	const char* LogLevelName[NUM_LOG_LEVELS] = 
	{
		"TRACE ",
		"DEBUG ",
		"INFO  ",
		"WARN  ",
		"ERROR ",
		"FATAL ",
		"BI    ",
		"DB    ",
	};

	class T
	{
	public:
		T(const char* str, uint32 len)
			:str_(str)
			,len_(len)
		{
			ZXERO_ASSERT(strlen(str) == len_);
		}

		const char* str_;
		const uint32 len_;
	};

	log_message& operator<<(log_message& s, T v)
	{
		s.append(v.str_, v.len_);
		return s;
	}
	log_message& operator<<(log_message& s, log_message::source_file f)
	{
		s.append(f.data_, f.size_);
		return s;
	}

	void log_message::build_header()
	{
		format_time();
		*this << boost::this_thread::get_id();
		*this << T(LogLevelName[level_], 6);
	}

#ifdef  WIN32
	__declspec(thread) uint64 lasttime_;
	__declspec(thread) char t_time_str_[32];
	__declspec(thread) bool need_flush_ = false;
#else
	__thread uint64 lasttime_;
	__thread char t_time_str_[32];
	__thread bool need_flush_ = false;
#endif

	void log_message::format_time()
	{
		*this << time_ << ' ';
		if (time_ - lasttime_ >= 1000)
		{
			lasttime_ = time_;
			boost::posix_time::ptime tm_time = seconds_transform_ptime(time_/1000);
			strncpy(t_time_str_,to_iso_extended_string(tm_time).c_str(),sizeof(t_time_str_));
			need_flush_ = true;
		}
		*this << t_time_str_ << ' ';
		//*this << to_iso_extended_string(time_) << ' ';
	}

	void defaultOutput(const char* msg, int len)
	{
		size_t n = fwrite(msg, 1, len, stdout);
		//FIXME check n
		(void)n;
	}

	void defaultFlush()
	{
		fflush(stdout);
	}

	log_message::OutputFunc g_output = defaultOutput;
	log_message::FlushFunc g_flush = defaultFlush;

	log_message::OutputFunc g_bi_output = defaultOutput;
	log_message::FlushFunc g_bi_flush = defaultFlush;

	log_message::OutputFunc g_db_output = defaultOutput;
	log_message::FlushFunc g_db_flush = defaultFlush;

	log_message::OutputFunc g_error_output = defaultOutput;
	log_message::FlushFunc g_error_flush = defaultFlush;

	void log_message::finish()
	{
		*this << " - " << basename_ << ":" << line_ << '\n';

		//fwrite( buffer_.data(), 1, buffer_.length(), stdout );
		//fflush( stdout );
		if (level_ == LL_BI)
		{
#ifdef _DEBUG
			defaultOutput(buffer_.data(), buffer_.length());
#ifdef ZXERO_OS_WINDOWS
			boost::scoped_array<char> str_buff(new char[buffer_.length() + 1]);
			std::memcpy(str_buff.get(), buffer_.data(), buffer_.length());
			str_buff[buffer_.length()] = 0;
			::OutputDebugString(str_buff.get());
#endif //ZXERO_OS_WINDOWS
#endif // DEBUG
			g_bi_output(buffer_.data(),buffer_.length());
			//g_bi_flush();		//fwrite[1]  函数写到用户空间缓冲区，并未同步到文件中，所以修改后要将内存与文件同步可以用fflush（FILE *fp）函数同步
		}
		else if (LL_DB == level_)
		{
#ifdef _DEBUG
			defaultOutput(buffer_.data(), buffer_.length());
#ifdef ZXERO_OS_WINDOWS
			boost::scoped_array<char> str_buff(new char[buffer_.length() + 1]);
			std::memcpy(str_buff.get(), buffer_.data(), buffer_.length());
			str_buff[buffer_.length()] = 0;
			::OutputDebugString(str_buff.get());
#endif //ZXERO_OS_WINDOWS
#endif // _DEBUG
			g_db_output(buffer_.data(),buffer_.length());
		}
		else if (LL_ERROR == level_ || LL_FATAL == level_ )
		{
#ifdef _DEBUG
			defaultOutput(buffer_.data(), buffer_.length());
#ifdef ZXERO_OS_WINDOWS
			boost::scoped_array<char> str_buff(new char[buffer_.length() + 1]);
			std::memcpy(str_buff.get(), buffer_.data(), buffer_.length());
			str_buff[buffer_.length()] = 0;
			::OutputDebugString(str_buff.get());
#endif //ZXERO_OS_WINDOWS
#endif // _DEBUG
			g_error_output(buffer_.data(),buffer_.length());
		}
		else
		{
#ifdef _DEBUG
			defaultOutput(buffer_.data(), buffer_.length());
#ifdef ZXERO_OS_WINDOWS
			boost::scoped_array<char> str_buff(new char[buffer_.length() + 1]);
			std::memcpy(str_buff.get(), buffer_.data(), buffer_.length());
			str_buff[buffer_.length()] = 0;
			::OutputDebugString(str_buff.get());
#endif //ZXERO_OS_WINDOWS
#endif // _DEBUG

			g_output(buffer_.data(),buffer_.length());
		}
		
		
		if ( level_ == LL_FATAL )
		{
			g_bi_flush();
			g_flush();
			g_db_flush();
			g_error_flush();
#ifdef ZXERO_OS_WINDOWS
#ifdef _DEBUG
			if (1 == _CrtDbgReport(_CRT_ASSERT, basename_.data_, line_, "GameServer", exp_))
#endif
			_CrtDbgBreak();
#else
			//abort();
			//assert(false);
#endif
		}	
		else
		{
			if (need_flush_ == true)
			{
				g_bi_flush();
				g_flush();
				g_db_flush();
				g_error_flush();
				need_flush_ = false;
			}
		}
	}

	log_message::log_message(LogLevel level, source_file filename, int line, LogDomain ld /*= LD_DEFAULT*/, const char* exp /*= "log without exp"*/)
		:level_(level)
		,line_(line)
		,basename_(filename)
		,time_(now_millisecond()),exp_(exp)
	{
		ld;
		build_header();
	}


	log_message::log_message(LogLevel level, source_file filename, int line, const char* func, LogDomain ld)
		:level_(level)
		,line_(line)
		,basename_(filename)
		,time_(now_millisecond())
	{
		ld;
		build_header();
		*this << " [" << func << "] ";
	}


	void log_message::setOutput(OutputFunc out)
	{
		g_output = out;
	}

	void log_message::setFlush(FlushFunc flush)
	{
		g_flush = flush;
	}


	void log_message::setBiOutput(OutputFunc out)
	{
		g_bi_output = out;
	}
	void log_message::setBiFlush(FlushFunc flush)
	{
		g_bi_flush = flush;
	}

	void log_message::setDBOutput(OutputFunc out)
	{
		g_db_output = out;
	}
	void log_message::setDBFlush(FlushFunc flush)
	{
		g_db_flush = flush;
	}

	void log_message::setERROROutput(OutputFunc out)
	{
		g_error_output = out;
	}
	void log_message::setERRORFlush(FlushFunc flush)
	{
		g_error_flush = flush;
	}

	/////
	void log_finisher::operator=(log_message& msg)
	{
		msg.finish();
	}

}

/////
//	这里实现了关于assert断言时的处理
namespace boost
{
	void assertion_failed(char const* expr, char const* function, char const* file, long line)
	{
		LOG_FATAL << "exp [" << expr << "] assertion failed at [" << function  << "] of [" << line << ":" << file << "]";
	}

	void assertion_failed_msg(char const* expr, char const* msg, char const* function, char const* file, long line)
	{
		LOG_FATAL << "exp [" << expr << "] assertion failed at [" << function << "] of [" << line << ":" << file << "] " << msg;
	}
}

