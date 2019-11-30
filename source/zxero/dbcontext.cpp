//
//  dbcontext.cpp
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
        
        //  ��������
        connection_properties["hostName"] = dbhost;
        connection_properties["userName"] = dbuser;
        connection_properties["password"] = dbpass;
		if (dbname != "")
			connection_properties["schema"] = dbname;
		//strcpy(dbname_,dbname.c_str());
		dbname_ = dbname.c_str();
        //  Ϊ�˷�ֹ���ݿ����ӳ�ʱ�������������
		connection_properties["OPT_RECONNECT"] = true;
		m_curSavePoint=nullptr;

		//  ��ȡ���ݿ�����
		{
			//	���۵�����������
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

		//  �������ݿ�����
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

			//	�����뼶������Ϊ��ȡ�ύ��
			connection_->setTransactionIsolation(sql::TRANSACTION_READ_COMMITTED);

			//  ��ʼ���������ݿ⹤��
			//init_all_db_prepared_statement();

			//	������й�������pingһ��
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

	//roll_back�е�����,��ʱ�ر�
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

		//  ��������db����������׼�����
		db_prepared_statement_register* reg = db_prepared_statement_register::root_;

		// db��� [9/7/2014 seed]
		if(reg->dbname_ != dbname_ )
			return;

		statements_.clear();

        while (reg) {
            try {

				////// ����autocommit��⣬auto_commit��Ҫ��true��Ҳ����ÿ��update�����ύmysql  [3/11/2015 SEED]
				//BOOST_ASSERT( connection_->getAutoCommit() );
				//// ����autocommit��⣬auto_commit��Ҫ��false���������

				// ����autocommit��⣬auto_commit��Ҫ��true��Ҳ����ÿ��update�����ύmysql  [3/11/2015 SEED]
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


