//
//  account_dbwork.h
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014�� zxero. All rights reserved.
//

#ifndef __GameServer__account_dbwork__
#define __GameServer__account_dbwork__

#include "global.h"
#include "dbwork.h"

namespace game
{
	struct online_user;

    //  �˺�ע�Ṥ��
    class account_register_work : public dbwork
    {
        DECLARE_DBWORK();
    public:
		typedef boost::shared_ptr<Account::RegisterRequest> RegisterRequestPtr;

        account_register_work(const RegisterRequestPtr& req );
        
        virtual void done();
    };
    
    //  �˺ŵ�¼����
    class account_login_work : public dbwork
    {
        DECLARE_DBWORK();

    public:
		typedef boost::shared_ptr<Account::LoginRequest> LoginRequestPtr;

        account_login_work(const LoginRequestPtr& req);
        
        virtual void done();

	private:
		LoginRequestPtr request_;

		//	�����ص��˺��б�
		std::unique_ptr<online_user> loaded_account_;
    };
}

#endif /* defined(__GameServer__account_dbwork__) */
