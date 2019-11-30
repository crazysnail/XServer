#include "network.h"
#include "app.h"
#include "log_file.h"
#include <boost/any.hpp>
#include "mt_arena/geometry.h"
//必须在最后include！-------------------
#include "../zxero/mem_debugger.h"


boost::scoped_ptr<zxero::LogFile> g_logFile;

#include <iostream>

void outputFunc(const char* msg, int len)
{
	if (g_logFile != nullptr)
	{
		g_logFile->append(msg, len);
	}
	
}

void flushFunc()
{
	if (g_logFile != nullptr)
	{
		g_logFile->flush();
	}
	
}


boost::scoped_ptr<zxero::LogFile> g_bi_logFile;

void bioutputFunc(const char* msg, int len)
{
	if (g_bi_logFile != nullptr)
	{
		g_bi_logFile->append(msg, len);
	}
	
}

void biflushFunc()
{
	if (g_bi_logFile != nullptr)
	{
		g_bi_logFile->flush();
	}
}


boost::scoped_ptr<zxero::LogFile> g_db_logFile;

void dboutputFunc(const char* msg, int len)
{
	if (g_db_logFile != nullptr)
	{
		g_db_logFile->append(msg, len);
	}
	
}

void dbflushFunc()
{
	if (g_db_logFile != nullptr)
	{
		g_db_logFile->flush();
	}
}


boost::scoped_ptr<zxero::LogFile> g_error_logFile;

void erroroutputFunc(const char* msg, int len)
{
	if (g_error_logFile != nullptr)
	{
		g_error_logFile->append(msg, len);
	}
	
}

void errorflushFunc()
{
	if (g_error_logFile != nullptr)
	{
		g_error_logFile->flush();
	}

}

int main(int argc, char** argv)
{
	//  目前全部开启为Trace日志，方便排错
	zxero::log_message::log_level(zxero::LL_INFO);
	//	启动服务器
	zxero::network::startup();
	g_logFile.reset(SAFE_NEW zxero::LogFile("./log/gamelog", 1024*1000*1000));
	g_bi_logFile.reset(SAFE_NEW zxero::LogFile("./log/bilog", 1024*1000*1000));
	g_db_logFile.reset(SAFE_NEW zxero::LogFile("./log/dblog", 1024*1000*1000));
	g_error_logFile.reset(SAFE_NEW zxero::LogFile("./log/errorlog", 1024*1000*1000));
	zxero::log_message::setOutput(outputFunc);
	zxero::log_message::setFlush(flushFunc);
	zxero::log_message::setBiOutput(bioutputFunc);
	zxero::log_message::setBiFlush(biflushFunc);
	zxero::log_message::setDBOutput(dboutputFunc);
	zxero::log_message::setDBFlush(dbflushFunc);
	zxero::log_message::setERROROutput(erroroutputFunc);
	zxero::log_message::setERRORFlush(errorflushFunc);
	LOG_INFO << "start GameServer";
	return zxero::app::main(argc, argv);
}
