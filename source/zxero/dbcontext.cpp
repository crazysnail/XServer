//
//  dbcontext.cpp
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "dbcontext.h"
#include "log.h"
#include "thread.h"
#include "dbwork.h"
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>
#include <mysql_driver.h>

namespace zxero {
    
	static mutex sg_driver_create_mutex;

	dbcontext::dbcontext(
		const sql::SQLString& dbhost,
		const sql::SQLString& dbuser,
		const sql::SQLString& dbpass,
		const sql::SQLString& dbname
    )
    {
        LOG_TRACE << "db context ctor [" << this << "]";
        
        //  连接属性
        connection_properties["hostName"] = dbhost;
        connection_properties["userName"] = dbuser;
        connection_properties["password"] = dbpass;
		if (dbname != "")
			connection_properties["schema"] = dbname;
		//strcpy(dbname_,dbname.c_str());
		dbname_ = dbname.c_str();
        //  为了防止数据库连接超时，设置重连标记
		connection_properties["OPT_RECONNECT"] = true;
		m_curSavePoint=nullptr;

		//  获取数据库驱动
		{
			//	蛋疼的这里必须加锁
			mutex::scoped_lock lock(sg_driver_create_mutex);
			driver_.reset(new sql::mysql::MySQL_Driver());
		}
		
		LOG_INFO << "db driver: " << driver_->getName()
			<< driver_->getMajorVersion() << "."
			<< driver_->getMinorVersion() << "."
			<< driver_->getPatchVersion();
        
		reconnection();
    }
    
    dbcontext::~dbcontext()
    {
        statements_.clear();
		if (connection_ != nullptr && connection_->isClosed() == false) {
			connection_->close();
		}
        connection_.reset();
    }

	bool dbcontext::reconnection()
	{
		if (connection_ && !connection_->isClosed())
			return true;

		//  建立数据库连接
		try {
			connection_.reset(driver_->connect(connection_properties));
		}
		catch (sql::SQLException& e) {
			LOG_ERROR << "db context "<< this << " connection failed: " << e.what();
			throw e;
		}

		if (connection_ != nullptr) {
			//
			LOG_INFO << "db context " << this << " connect to :" << connection_->getSchema();

			//	将隔离级别设置为读取提交后
			connection_->setTransactionIsolation(sql::TRANSACTION_READ_COMMITTED);

			//  初始化所有数据库工作
			//init_all_db_prepared_statement();

			//	完成所有工作后再ping一下
			return !connection_->isClosed();
		}

		return false;
	}
    
    sql::PreparedStatement* dbcontext::get_prepared_statement(int32 idx)
    {
        if (idx >= 0 && idx < (int32)statements_.size()) {
            return statements_[idx].get();
        }
        return nullptr;
    }
    
	void dbcontext::commit()
	{
		if( connection_!= nullptr )
			connection_->commit();
	}

	void dbcontext::commit_begin()
	{
		if( connection_!= nullptr )
		{
			connection_->setAutoCommit(false);
			//sql::Statement *st =connection_->createStatement();
			//st->execute("START TRANSACTION");
			//delete st;
			//m_curSavePoint = connection_->setSavepoint("save");
		}
	}
	void dbcontext::commit_end()
	{
		if( connection_!= nullptr )
		{
			//if(m_curSavePoint!= nullptr)
			//	connection_->releaseSavepoint(m_curSavePoint);

			connection_->commit();
			connection_->setAutoCommit(true);
		}
	}

	//roll_back有点问题,暂时关闭
	void dbcontext::roll_back()
	{
		//if( connection_!= nullptr && m_curSavePoint != nullptr )
		//{
		//	connection_->rollback(m_curSavePoint);
		//	m_curSavePoint=nullptr;
		//}
	}

	//
	void dbcontext::auto_commit(bool enable)
	{
		if( connection_!= nullptr )
		{
			connection_->setAutoCommit(enable);
		}
	}

    db_prepared_statement_register* db_prepared_statement_register::root_;
    void dbcontext::init_all_db_prepared_statement()
    {

		//  遍历所有db工作，创建准备语句
		db_prepared_statement_register* reg = db_prepared_statement_register::root_;

		// db检查 [9/7/2014 seed]
		if(reg->dbname_ != dbname_ )
			return;

		statements_.clear();

        while (reg) {
            try {

				////// 做个autocommit检测，auto_commit需要是true，也就是每次update都会提交mysql  [3/11/2015 SEED]
				//BOOST_ASSERT( connection_->getAutoCommit() );
				//// 做个autocommit检测，auto_commit需要是false，提高性能

				// 做个autocommit检测，auto_commit需要是true，也就是每次update都会提交mysql  [3/11/2015 SEED]
				if( !connection_->getAutoCommit() )
					connection_->setAutoCommit(true);

                boost::shared_ptr<sql::PreparedStatement> ptr(connection_->prepareStatement(reg->sql_));
                if ( reg->index_ != -1 )
                {
                    BOOST_ASSERT( reg->index_ == (int32)statements_.size() );
                }
                else
                {
                    reg->index_ = (int32)statements_.size();
                }
                statements_.push_back(ptr);
            } catch (sql::SQLException& e) {
                LOG_ERROR << "db prepared statement '" << reg->statement() << "' init failed : " << e.what();
                throw e;
            }
            
            reg = reg->next_;
        }
    }
    
    //////
    db_prepared_statement_register::db_prepared_statement_register( const std::string& dbname, int32& stat_idx, const sql::SQLString& sql )
    :dbname_(dbname)
	,index_(stat_idx)
    ,sql_(sql)
    ,next_(root_)
    {
        root_ = this;
    }
    
}


