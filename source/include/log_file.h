#ifndef _zxero_log_file_h_
#define _zxero_log_file_h_

#include "log.h"
#include "mutex.h"
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

//	把日志写入文件中
//	参考muduo的异步日志库
namespace zxero
{
	namespace FileUtil
	{
		class AppendFile : boost::noncopyable
		{
		public:
			explicit AppendFile(const char* filename);

			~AppendFile();

			void append(const char* logline, const size_t len);

			void flush();

			size_t writtenBytes() const { return writtenBytes_; }

		private:

			size_t write(const char* logline, size_t len);

			FILE* fp_;
			char buffer_[64*1024];
			size_t writtenBytes_;
		};

	}

	class LogFile : boost::noncopyable
	{
	public:
		LogFile(const std::string& basename,
			size_t rollSize,
			bool threadSafe = true,
			int flushInterval = 3,
			int checkEveryN = 1024);
		~LogFile();

		void append(const char* logline, int len);
		void flush();
		bool rollFile(int32 G_Server_Time_Day);
		void set_thread_safe(bool safe);
		bool get_thread_safe() const;

	private:
		void append_unlocked(const char* logline, int len);
		static std::string getLogFileName(const std::string& basename, time_t* now);
		const std::string basename_;
		const size_t rollSize_;
		bool threadSafe_;
		const int flushInterval_;
		const int checkEveryN_;

		int count_;

		//boost::scoped_ptr<MutexLock> mutex_;
		unzxero::mutex mutex_;


		int32 lastRoll_day;
		time_t lastFlush_;
		boost::scoped_ptr<FileUtil::AppendFile> file_;

	};
}

#endif