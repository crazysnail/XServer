//
//  dbcontext.h
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014年 zxero. All rights reserved.
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
		//	连接属性
		sql::ConnectOptionsMap connection_properties;
		//	数据库驱动
		sql_driver_ptr driver_;
        //  数据库连接
        sql_connection_ptr connection_;
        //  对应连接的sql准备语句
        sql_prepared_statements statements_;
        
    public:
        dbcontext(
                  const sql::SQLString& dbHost,
                  const sql::SQLString& dbuser,
                  const sql::SQLString& dbpass,
				  const sql::SQLString& dbname);
        
        ~dbcontext();
        
    public:
        //  根据索引号获取对应的语句
        sql::PreparedStatement* get_prepared_statement( int32 idx );

		//	获取数据库连接
		const sql_connection_ptr& get_connection() const {
			return connection_;
		}

		//	重新连接数据库
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
        //  初始化准备语句，进行相应的配置
        void init_all_db_prepared_statement();
		// 数据库标识 [9/7/2014 seed]
		std::string dbname_;
		sql::Savepoint* m_curSavePoint;
        friend class dbwork;
    };
    
	typedef boost::shared_ptr<dbcontext> dbcontext_ptr;
}

#endif /* defined(__zxero__dbcontext__) */
