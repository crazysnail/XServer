#include "asynclogging.h"
#include "log_file.h"
#include "date_time.h"
#include <stdio.h>

namespace zxero
{
	asynclogging::asynclogging(const std::string& basename,
		size_t rollSize,
		int flushInterval)
		:flushInterval_(flushInterval),
		running_(false),
		basename_(basename),
		rollSize_(rollSize),
		thread_(boost::bind(&asynclogging::threadFunc,this),"Logging"),
		currentBuffer_(new Buffer),
		nextBuffer_(new Buffer),
		buffers_()
	{
		currentBuffer_->bzero();
		nextBuffer_->bzero();
		buffers_.reserve(16);
	}

	void asynclogging::append(const char* logline, int len)
	{
		mutex::scoped_lock lock(mutex_);
		if (currentBuffer_->avail() > len) //当前buff的空间还够放这条日志
		{
			currentBuffer_->append(logline, len);
		}
		else
		{
			buffers_.push_back(currentBuffer_.release());

			if (nextBuffer_)
			{
				currentBuffer_ = boost::ptr_container::move(nextBuffer_);
			}
			else
			{
				currentBuffer_.reset(new Buffer); // Rarely happens
			}
			currentBuffer_->append(logline, len);
			cond_.notify_one();
		}
	}

	void asynclogging::threadFunc()
	{
		ZXERO_ASSERT(running_);

		 LogFile output(basename_, rollSize_, false);
		 BufferPtr newBuffer1(new Buffer);
		 BufferPtr newBuffer2(new Buffer);
		 newBuffer1->bzero();
		 newBuffer2->bzero();
		 BufferVector buffersToWrite;
		 buffersToWrite.reserve(16);
		 while (running_)
		 {
			 ZXERO_ASSERT(newBuffer1 && newBuffer1->length() == 0);
			 ZXERO_ASSERT(newBuffer2 && newBuffer2->length() == 0);
			 ZXERO_ASSERT(buffersToWrite.empty());

			 {
				 mutex::scoped_lock lock(mutex_);
				 if ( buffers_.empty())	// cond_.wait(mutex_,!buffers_.empty());
				 {
					cond_.wait(mutex_);
				 }
				 buffers_.push_back(currentBuffer_.release());
				 currentBuffer_ = boost::ptr_container::move(newBuffer1);
				 buffersToWrite.swap(buffers_);
				 if (!nextBuffer_)
				 {
					 nextBuffer_ = boost::ptr_container::move(newBuffer2);
				 }
			 }

			 ZXERO_ASSERT(!buffersToWrite.empty());

			 //最大队列数为25, 大于25则对之后进行erase
			 if (buffersToWrite.size() > 25)
			 {
				 char buf[256];
				 sprintf(buf,"Dropped log messages at %s, %zd larger buffers\n",
							to_iso_extended_string(now()).c_str(),buffersToWrite.size()-2);
				 fputs(buf, stderr);
				 output.append(buf, static_cast<int>(strlen(buf)));
				 buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
			 }

			 //对buffers_队列进行写操作
			 for (size_t i = 0; i < buffersToWrite.size(); ++i)
			 {
				 // FIXME: use unbuffered stdio FILE ? or use ::writev ?
				 output.append(buffersToWrite[i].data(), buffersToWrite[i].length());
			 }

			 //只留下两个buffer作为零时buffer1和buffer2
			 if (buffersToWrite.size() > 2)
			 {
				 // drop non-bzero-ed buffers, avoid trashing
				 buffersToWrite.resize(2);
			 }

			 //重置buffer1
			 if (!newBuffer1)
			 {
				 assert(!buffersToWrite.empty());
				 newBuffer1 = buffersToWrite.pop_back();
				 newBuffer1->reset();
			 }
			 //重置buffer2
			 if (!newBuffer2)
			 {
				 assert(!buffersToWrite.empty());
				 newBuffer2 = buffersToWrite.pop_back();
				 newBuffer2->reset();
			 }

			 buffersToWrite.clear();
			 output.flush();
		 }
		 output.flush();
	}
}