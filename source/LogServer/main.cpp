//
//  main.cpp
//  AccountServer
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014年 zxero. All rights reserved.
//
#include "network.h"
#include "app.h"
#include "log.h"
#include "log_file.h"
//#include "hiredis.h"
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

int main(int argc, char * argv[])
{
	//  目前全部开启为Trace日志，方便排错
	zxero::log_message::log_level(zxero::LL_INFO);
	//	启动服务器
	zxero::network::startup();
	g_logFile.reset(SAFE_NEW zxero::LogFile("./log/logserver", 1024 * 1000 * 1000));
	
	zxero::log_message::setOutput(outputFunc);
	zxero::log_message::setFlush(flushFunc);

    LOG_INFO << "start LogServer";

	//WSADATA wsd;
	//WSAStartup(MAKEWORD(2,2), &wsd);

	//redisContext *conn  = redisConnect("127.0.0.1",6379); 
	//if(conn != NULL && conn->err) 
	//{   
	//	printf("connection error: %s\n",conn->errstr); 
	//	return 0; 
	//}   
	//redisReply *reply = (redisReply*)redisCommand(conn,"set foo 1234"); 
	//freeReplyObject(reply); 
	//reply = (redisReply*)redisCommand(conn,"get foo"); 
	//printf("%s\n",reply->str); 
	//freeReplyObject(reply); 
	//redisFree(conn); 

	//WSACleanup();

    return zxero::app::main(argc, argv);
}

