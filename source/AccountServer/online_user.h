//
//  online_user.h
//  AccountServer
//
//  Created by zXero on 14-4-18.
//  Copyright (c) 2014�� zxero. All rights reserved.
//
//  ������ҵĴ���
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
		//	��ǰ���ӵ�csid
		client_session_id csid;
		//	�˺�id
		uint32 id;
		//	�˺���
		std::string account;
		//	ע������
		std::string password;
		//	�˺�״̬
		uint32 state;
		//	δ����ɫ��ȡ����ʯ
		uint64 token;
		//	����¼ip
		std::string lastip;
		//	�ֻ���
		std::string phone;
		//	��������
		std::string email;
	};
}

#endif /* defined(__AccountServer__online_user__) */
