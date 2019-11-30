//
//  online_user.h
//  AccountServer
//
//  Created by zXero on 14-4-18.
//  Copyright (c) 2014年 zxero. All rights reserved.
//
//  在线玩家的处理
//

#ifndef __AccountServer__online_user__
#define __AccountServer__online_user__

#include "global.h"

namespace game
{
	enum online_user_state
	{
		kWaitOnline	= 0,
		kWaitKickOther,
		kLoading,
		kOnline,
		kOfflineSaving,
		kOffline,
	};

	struct online_user : boost::noncopyable
	{
		//	当前连接的csid
		client_session_id csid;
		//	账号id
		uint32 id;
		//	账号名
		std::string account;
		//	注册密码
		std::string password;
		//	账号状态
		uint32 state;
		//	未被角色获取的钻石
		uint64 token;
		//	最后登录ip
		std::string lastip;
		//	手机号
		std::string phone;
		//	电子邮箱
		std::string email;
	};
}

#endif /* defined(__AccountServer__online_user__) */
