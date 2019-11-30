//
//  account_dbwork.cpp
//  GameServer
//
//  Created by zXero on 14-4-7.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "account_dbwork.h"
#include "account_manager.h"
#include "online_user.h"
#include "dbcontext.h"

namespace game
{
	typedef boost::scoped_ptr<sql::ResultSet> result_set_ptr;
	REG_PREPARED_STATEMENT( "diablo_account", account_load_ps, "SELECT tid, taccount, tpassword, tstate, ttoken, tlastip, tphone, temail FROM t_account WHERE taccount = ?" );
	REG_PREPARED_STATEMENT( "diablo_account", account_create_ps, "INSERT INTO t_account (taccount, tpassword, tstate, ttoken, tlastip, tphone, temail) VALUES (?,?,?,?,?,?,?)" );

	IMPLEMENT_DBWORK(account_login_work)
	{
		auto ps_load = PS(account_load_ps);
		auto ps_create = PS(account_create_ps);

		ps_load->setString( 1, request_->account() );
		result_set_ptr result(ps_load->executeQuery());

		
		if ( !result || !result->next() )
		{
			if( request_->is_fast() )
			{// 快速登陆如果没有账号会自动生成记录插入数据库 [12/17/2014 SEED]

				std::string account = request_->account();
				if( account.empty() )
				{
					// 快速注册账号不能为空 [12/17/2014 SEED]
					//	
					sql_error_ = 13;
					LOG_ERROR << "account_login_work failed with account " << request_->account();
					return;
				}

				ps_create->setString( 1, request_->account() );
				ps_create->setString( 2, request_->password() );
				ps_create->setUInt( 3, 0 );
				ps_create->setUInt64( 4, 0 );
				ps_create->setString( 5, request_->ip() );
				ps_create->setString( 6, "00000000000" );
				ps_create->setString( 7, "no@email.com" );
				int r = ps_create->executeUpdate();
				ZXERO_ASSERT(r == 1) << "failed with account " << request_->account();

				//	再次执行准备好的语句
				result.reset(ps_load->executeQuery());

				if ( !result || !result->next() )
				{
					//	
					sql_error_ = 13;
					LOG_ERROR << "account_login_work failed with account " << request_->account();
					return;
				}
			}
			else
			{// 否则直接提示登陆错误
				//Sotcc::Result::E_LOGIN_NOACCOUNT;
				sql_error_ = 13;
				LOG_ERROR << "account_login_work account not in db.[account]:" << request_->account();
				return ;
			}

		}

		//	获取到账号数据了
		loaded_account_.reset(new online_user);
		loaded_account_->id = result->getUInt(1);
		loaded_account_->account = result->getString(2).c_str();
		loaded_account_->password = result->getString(3).c_str();
		loaded_account_->state = result->getUInt(4);
		loaded_account_->token = result->getUInt64(5);
		loaded_account_->lastip = result->getString(6).c_str();
		loaded_account_->phone = result->getString(7).c_str();
		loaded_account_->email = result->getString(8).c_str();
	}

	void account_login_work::done()
	{
		//	
		dbwork::done();

		if ( sql_error_ )
		{
			//	通知GameServer该账号登陆失败
			Account::LoginReply reply;
			reply.set_csid( request_->csid() );
			reply.set_result( sql_error_ );
			account_manager::instance()->send( client_session_id(request_->csid()), reply );
		}
		else
		{
			//	匹配一下密码
			if ( request_->password() != loaded_account_->password.c_str() )
			{
				//	密码不匹配的处理，暂时没有
				Account::LoginReply reply;
				reply.set_csid( request_->csid() );
				//Sotcc::Result::E_LOGIN_PASSWORD
				reply.set_result( 14 );
				account_manager::instance()->send( client_session_id(request_->csid()), reply );
				LOG_ERROR << "account_login_work password wrong. account=" << request_->account()<<".password="<<request_->password();
				return;
			}

			//	加入在线玩家列表, 将加载到的账号数据，转给account_manager处理
			loaded_account_->csid = request_->csid();
			account_manager::instance()->add_online_user( loaded_account_.release() );
		}
	}

	account_login_work::account_login_work(const LoginRequestPtr& req)
		:request_(req)
	{
	}


	/////
	IMPLEMENT_DBWORK(account_register_work)
	{
		//	设置登录消息
	}

	void account_register_work::done()
	{
		
	}

	account_register_work::account_register_work( const RegisterRequestPtr& req )
	{
	}
}

