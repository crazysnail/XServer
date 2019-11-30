//
//  account_dbwork.h
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __GameServer__account_dbwork__
#define __GameServer__account_dbwork__

#include "global.h"
#include "dbwork.h"

namespace game
{
	struct online_user;

    //  账号注册工作
    class account_register_work : public dbwork
    {
        DECLARE_DBWORK();
    public:
		typedef boost::shared_ptr<Account::RegisterRequest> RegisterRequestPtr;

        account_register_work(const RegisterRequestPtr& req );
        
        virtual void done();
    };
    
    //  账号登录工作
    class account_login_work : public dbwork
    {
        DECLARE_DBWORK();

    public:
		typedef boost::shared_ptr<Account::LoginRequest> LoginRequestPtr;

        account_login_work(const LoginRequestPtr& req);
        
        virtual void done();

	private:
		LoginRequestPtr request_;

		//	被加载的账号列表
		std::unique_ptr<online_user> loaded_account_;
    };
}

#endif /* defined(__GameServer__account_dbwork__) */
