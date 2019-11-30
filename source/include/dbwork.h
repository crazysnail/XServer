//
//  dbwork.h
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
        //  ���ݿ⹤������ӿڣ��ɸ�����������ݿ⹤������
        virtual void process( boost::shared_ptr<dbcontext>& dbctx ) = 0;
        
        //  ���ݿ⹤����ɼ��ɣ���Ĭ��ʵ��
        virtual void done();
        
    private:
        //  ִ�й����ľ��庯��
        void do_work( boost::any& ctx );
        //  ������ɴ���ľ��庯��
        void done_work( const boost::shared_ptr<work>& w );        
    protected:
        //  db����������
        int sql_error_;
    };
    
    //  ���ݿ�׼�����ע����, �ڲ��࣬�ⲿ��ʹ��
    
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

		// db��ʶ [9/7/2014 seed]
		std::string dbname_;

        //  ׼�����������ţ�Ĭ��Ϊ��1������û�����
        int32& index_;
        //  ��Ӧ�Ĺ������
        const sql::SQLString sql_;
        //  ���ӵĹ���ע���������ڵ�
        db_prepared_statement_register*    next_;
        
    private:
        friend class dbcontext;
        static db_prepared_statement_register* root_;
    };
    
//  ����һ�����ݿ⹤��
#define DECLARE_DBWORK( )  \
    private:    \
	static std::string sg_dbwork_name;	\
	virtual const std::string& name() const { return sg_dbwork_name; }	\
    virtual void process( boost::shared_ptr<zxero::dbcontext>& dbctx )

//  ʵ�ּ�ע�����ݿ⹤��
#define IMPLEMENT_DBWORK( cls ) \
	std::string cls::sg_dbwork_name( #cls );	\
    void cls::process( boost::shared_ptr<zxero::dbcontext>& dbctx )
    
	//	ע�����
#define DECLARE_REG_PREPARED_STATEMENT( v )	\
	zxero::int32 v=-1;						

	//	ע�����
#define IMPLEMENT_REG_PREPARED_STATEMENT( n, v, s )	\
	static zxero::db_prepared_statement_register sg##v( n, v, s );

//	ע�����
#define REG_PREPARED_STATEMENT( n, ps, s )	\
	static zxero::int32 ps = -1;				\
	static zxero::db_prepared_statement_register sg##ps( n, ps, s );

//	���ٻ�ȡ���
#define PS( idx )	dbctx->get_prepared_statement(idx)




}

#endif /* defined(__zxero__dbwork__) */
