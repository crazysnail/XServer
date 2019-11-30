//
//  main.cpp
//  AccountServer
//
//  Created by zXero on 14-4-12.
//  Copyright (c) 2014Äê zxero. All rights reserved.
//

#include "app.h"
#include "log.h"

//#include "hiredis.h"

int main(int argc, char * argv[])
{
	//system("pause");
    LOG_INFO << "start AccountServer";

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

