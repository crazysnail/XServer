#ifndef _utils_h
#define _utils_h

#include "types.h"
#include <vector>
#include <list>

#ifdef ZXERO_OS_LINUX
#include <sys/stat.h>
#include <dirent.h>
#endif

namespace zxero
{
	const static int32 MAX_LINE_SIZE = 2048;
	class utils
	{
	public:
		//判定单前系统时间是否在某个时间区间，格式如下：begin-end 20151001-20151031
		static bool check_date( int32 begin, int32 end );
		//判定单前系统时间是否在指定时间的后n天
		static bool check_date_next_days( int32 source_date, int32 n=1 );
		//根据单前系统时间生成一个精确到天的int32时间戳，格式类似20151001
		static int32 gen_date_day_number();
		//将一个uint64时间转换成一个int32的时间撮，格式类似20151001
		static int32 convert_data_day_number( uint64 time );
		static bool name_check( std::string name );

		
		static void split_file_string(const char * src_string, const char * key, std::vector<std::string> &sub_lst );		
		static void split_string(const char * src_string, const char * key, std::vector<std::string> &sub_lst );
		static void split_string(const char * src_string, const char * key, std::vector<int32> &sub_lst );
		static unsigned char* get_file_data(const char * filename, const char* mode, size_t *size,bool is_utf8 );
		static void trim_string(std::string& text)  
		{  
			if(!text.empty())  
			{  
				text.erase(0, text.find_first_not_of((" \n\r\t")));  
				text.erase(text.find_last_not_of((" \n\r\t")) + 1);  
			}  
		};
		//char* 转 16进制字符串
		static std::string bytestohexstring(char* bytes, int bytelength);

		static char * l_trim(char * szOutput, const char *szInput);
		static char * r_trim(char *szOutput, const char *szInput);
		static char * a_trim(char * szOutput, const char * szInput);
		static void regexstring(const std::string sourcestr, std::string reg_esp, std::vector<std::string>& out_match);
		static uint64 guid_string_to_uint64(std::string guid_str);
		static int64 atoi64(const char * str_)
		{
			#if ZXERO_OS_LINUX
				return strtol(str_, NULL, 10 );
			#elif ZXERO_OS_WINDOWS
				return _atoi64(str_);
			#endif
		}
	#ifdef ZXERO_OS_WINDOWS
		static int32 enum_dir_files(std::string src_path, std::vector<std::string>& file_list);
	#endif

	#ifdef ZXERO_OS_LINUX
		static bool is_dir(const char *pszName);
		static int32 enum_dir_files(std::string src_path, std::vector<std::string>& file_list);
	#endif

		static char * get_ini_key_string(const char *title,const char *key,const char *filename);
		static int get_ini_key_int(const char *title,const char *key,const char *filename);
		static long get_ini_key_long(const char *title,const char *key,const char *filename);

		template <class T, class A>
		static T join(const A &begin, const A &end, const T &t)
		{
			T result;
			for (A it = begin;
				it != end;
				it++)
			{
				if (!result.empty())
					result.append(t);
				result.append(*it);
			}
			return result;
		}

		//检查一个int数的某一位为1
		template<typename T>
		static bool bit_check(T source, int32 bit_index)
		{
			//ZXERO_ASSERT(  bit_index >=0 && bit_index < sizeof(T)*8 );
			if (bit_index < 0 || bit_index >= int32(sizeof(T) * 8))
			{
				return false;
			}

			if ((source & (T)((T)1 << bit_index)) == 0) {
				return false;
			}
			return true;
		}

		//设置一个int数的某一位为1
		template<typename T>
		static void bit_set(T &source, int32 bit_index)
		{
			//ZXERO_ASSERT(  bit_index >=0 && bit_index < sizeof(T)*8 );
			if (bit_index < 0 || bit_index >= int32(sizeof(T) * 8))
			{
				return;
			}

			source |= (T)((T)1 << bit_index);
		}

		//设置一个int数的某一位为0
		template<typename T>
		static void bit_reset(T &source, int32 bit_index)
		{
			//ZXERO_ASSERT(  bit_index >=0 && bit_index < sizeof(T)*8 );
			if (bit_index < 0 || bit_index >= int32(sizeof(T) * 8))
			{
				return;
			}

			T nMark = 0;
			nMark = (~nMark) ^ (T)((T)1 << bit_index);
			source &= nMark;
		}
		static void Q_strncpyz(char *dest, const char *src, int destsize);

		static std::string md5(const std::string str);
	};

	class profiler
	{		
	public:
		profiler(std::string log);

		~profiler();

	private:
		uint64 m_time;
		std::string m_log;
	};

	class profiler2addr
	{
	public:
		profiler2addr(int32* addr);
		~profiler2addr();;
	private:
		uint64 time_;
		int32* addr_ = nullptr;
	};



}	//	namespace game
#endif	//	#ifndef _utils_h
