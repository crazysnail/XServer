#include "log_file.h"
#include <time.h>
#include "date_time.h"

namespace zxero
{
	FileUtil::AppendFile::AppendFile(const char* filename)
		: fp_(::fopen(filename, "a+")),  // 'e' for O_CLOEXEC
		writtenBytes_(0)
	{
		ZXERO_ASSERT(fp_);
		//::setbuffer(fp_, buffer_, sizeof buffer_);
		setbuf(fp_, buffer_);
		// posix_fadvise POSIX_FADV_DONTNEED ?
	}

	FileUtil::AppendFile::~AppendFile()
	{
		::fclose(fp_);
	}

	void FileUtil::AppendFile::append(const char* logline, const size_t len)
	{
		size_t n = write(logline, len);
		size_t remain = len - n;
		while (remain > 0)
		{
			size_t x = write(logline + n, remain);
			if (x == 0)
			{
				int err = ferror(fp_);
				if (err)
				{
					fprintf(stderr, "AppendFile::append() failed \n" );
				}
				break;
			}
			n += x;
			remain = len - n; // remain -= x
		}

		writtenBytes_ += len;
	}

	void FileUtil::AppendFile::flush()
	{
		::fflush(fp_);
	}

	size_t FileUtil::AppendFile::write(const char* logline, size_t len)
	{
		// #undef fwrite_unlocked
		return ::fwrite(logline, 1, len, fp_);
	}

//////////////////////////////////////////////////////////////////////////

	LogFile::LogFile(const std::string& basename,
		size_t rollSize,
		bool threadSafe,
		int flushInterval,
		int checkEveryN)
		: basename_(basename),
		rollSize_(rollSize),
		threadSafe_(threadSafe),
		flushInterval_(flushInterval),
		checkEveryN_(checkEveryN),
		count_(0),
		lastRoll_day(0),
		lastFlush_(0)
	{
		//ZXERO_ASSERT(basename.find('/') == string::npos);
		rollFile(now().date().day_of_year());
	}

	LogFile::~LogFile()
	{
	}

	void LogFile::append(const char* logline, int len)
	{
		if (threadSafe_ == true )
		{
			unzxero::auto_lock lock(mutex_);
			append_unlocked(logline, len);
		}
		else
		{
			append_unlocked(logline, len);
		}
	}

	void LogFile::flush()
	{
		if (threadSafe_ == true )
		{
			unzxero::auto_lock lock(mutex_);
			file_->flush();
		}
		else
		{
			file_->flush();
		}
	}

	void LogFile::append_unlocked(const char* logline, int len)
	{
		file_->append(logline, len);

		if (file_->writtenBytes() > rollSize_)
		{
			rollFile(now().date().day_of_year());
		}
		else
		{
			time_t now = ::time(NULL);
			if (G_Server_Time_DayOfYear > lastRoll_day)
			{
				rollFile(G_Server_Time_DayOfYear);
			}
			else if (now - lastFlush_ > flushInterval_)
			{
				lastFlush_ = now;
				file_->flush();
			}
		}
	}

	bool LogFile::rollFile(int32 G_Server_Time_Day)
	{
		time_t now = 0;
		std::string filename = getLogFileName(basename_, &now);
		if (G_Server_Time_Day > lastRoll_day)
		{
			lastRoll_day = G_Server_Time_Day;
			lastFlush_ = now;
			file_.reset(new FileUtil::AppendFile(filename.c_str()));
			return true;
		}
		return false;
	}

	std::string LogFile::getLogFileName(const std::string& basename,time_t* now )
	{
		std::string filename;
		filename.reserve(basename.size() + 64);
		filename = basename;


		char timebuf[32];

		*now = time(NULL);
#ifdef  WIN32
		struct tm * tm = gmtime(now); //非线程安全的
		strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", tm);
#else
		struct tm tm;
		*now = time(NULL);
		gmtime_r(now, &tm); // FIXME: localtime_r ?
		strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
#endif
		filename += timebuf;

		//filename += ProcessInfo::hostname();

		//char pidbuf[32];
		//snprintf(pidbuf, sizeof pidbuf, ".%d.", ProcessInfo::pid());
		//filename += pidbuf;

		filename += "log";

		return filename;
	}

	void LogFile::set_thread_safe(bool safe)
	{
		threadSafe_ = safe;
	}

	bool LogFile::get_thread_safe() const
	{
		return threadSafe_;
	}

}