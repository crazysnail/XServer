//
//  dbcontext.h
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014�� zxero. All rights reserved.
//

#ifndef __zxero__dbcontext__
#define __zxero__dbcontext__

#include "types.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <vector>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4251)
#endif
#include <cppconn/sqlstring.h>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4100)
#endif
#include <cppconn/connection.h>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(default:4100)
#endif

namespace sql
{
    class PreparedStatement;
    class Connection;
	class Driver;
}

namespace zxero
{
    class dbwork;
    
    class dbcontext : boost::noncopyable
    {
    private:
        typedef boost::shared_ptr< sql::PreparedStatement > sql_prepared_statement_ptr;
        typedef boost::shared_ptr< sql::Connection > sql_connection_ptr;
		typedef boost::shared_ptr< sql::Driver > sql_driver_ptr;
        typedef std::vector< sql_prepared_statement_ptr > sql_prepared_statements;
        
    private:
		//	��������
		sql::ConnectOptionsMap connection_properties;
		//	���ݿ�����
		sql_driver_ptr driver_;
        //  ���ݿ�����
        sql_connection_ptr connection_;
        //  ��Ӧ���ӵ�sql׼�����
        sql_prepared_statements statements_;
        
    public:
        dbcontext(
                  const sql::SQLString& dbHost,
                  const sql::SQLString& dbuser,
                  const sql::SQLString& dbpass,
				  const sql::SQLString& dbname);
        
        ~dbcontext();
        
    public:
        //  ���������Ż�ȡ��Ӧ�����
        sql::PreparedStatement* get_prepared_statement( int32 idx );

		//	��ȡ���ݿ�����
		const sql_connection_ptr& get_connection() const {
			return connection_;
		}

		//	�����������ݿ�
		bool reconnection();
        
    public:
        //
        typedef boost::shared_ptr<dbcontext> work_ctx_t;
		// commit
		void commit();
		//
		void auto_commit(bool enable);

		//
		void commit_begin();
		void commit_end();
		void roll_back();
    private:
        //  ��ʼ��׼����䣬������Ӧ������
        void init_all_db_prepared_statement();
		// ���ݿ��ʶ [9/7/2014 seed]
		std::string dbname_;
		sql::Savepoint* m_curSavePoint;
        friend class dbwork;
    };
    
	typedef boost::shared_ptr<dbcontext> dbcontext_ptr;
}

#endif /* defined(__zxero__dbcontext__) */
