//
//  dbwork.h
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __zxero__dbwork__
#define __zxero__dbwork__

#include "types.h"
#include "work.h"

#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4251)
#endif


//#define CPPCONN_LIB_BUILD 

#include <cppconn/sqlstring.h>
#include <cppconn/prepared_statement.h>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(disable:4100)
#endif
#include <mysql_connection.h>
#ifdef ZXERO_OS_WINDOWS
#pragma warning(default:4100)
#endif

namespace zxero
{
	class dbcontext;

    class dbwork : public work
    {
    public:
        dbwork();
        virtual ~dbwork();
        
    public:
        //  数据库工作处理接口，由各个具体的数据库工作处理
        virtual void process( boost::shared_ptr<dbcontext>& dbctx ) = 0;
        
        //  数据库工作完成即可，有默认实现
        virtual void done();
        
    private:
        //  执行工作的具体函数
        void do_work( boost::any& ctx );
        //  工作完成处理的具体函数
        void done_work( const boost::shared_ptr<work>& w );        
    protected:
        //  db操作错误码
        int sql_error_;
    };
    
    //  数据库准备语句注册器, 内部类，外部不使用
    
    class db_prepared_statement_register : boost::noncopyable
    {
    public:
        db_prepared_statement_register( const std::string& dbname, int32& stat_idx, const sql::SQLString& sql );
        
        int32 index() const {
            return index_;
        }

		const sql::SQLString& statement() const {
			return sql_;
		}

    private:

		// db标识 [9/7/2014 seed]
		std::string dbname_;

        //  准备语句的索引号，默认为－1，表明没有语句
        int32& index_;
        //  对应的工作语句
        const sql::SQLString sql_;
        //  连接的工作注册器后续节点
        db_prepared_statement_register*    next_;
        
    private:
        friend class dbcontext;
        static db_prepared_statement_register* root_;
    };
    
//  定义一个数据库工作
#define DECLARE_DBWORK( )  \
    private:    \
	static std::string sg_dbwork_name;	\
	virtual const std::string& name() const { return sg_dbwork_name; }	\
    virtual void process( boost::shared_ptr<zxero::dbcontext>& dbctx )

//  实现及注册数据库工作
#define IMPLEMENT_DBWORK( cls ) \
	std::string cls::sg_dbwork_name( #cls );	\
    void cls::process( boost::shared_ptr<zxero::dbcontext>& dbctx )
    
	//	注册语句
#define DECLARE_REG_PREPARED_STATEMENT( v )	\
	zxero::int32 v=-1;						

	//	注册语句
#define IMPLEMENT_REG_PREPARED_STATEMENT( n, v, s )	\
	static zxero::db_prepared_statement_register sg##v( n, v, s );

//	注册语句
#define REG_PREPARED_STATEMENT( n, ps, s )	\
	static zxero::int32 ps = -1;				\
	static zxero::db_prepared_statement_register sg##ps( n, ps, s );

//	快速获取语句
#define PS( idx )	dbctx->get_prepared_statement(idx)




}

#endif /* defined(__zxero__dbwork__) */
