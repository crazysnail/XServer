////
////  dbwork.h
////  zxero
////
////  Created by zXero on 16-9-12.
////  Copyright (c) 2016年 seed. All rights reserved.
////
//
//#ifndef __zxero__redis_work__
//#define __zxero__redis_work__
//
//#include "types.h"
//#include "work.h"
//
//#ifdef ZXERO_OS_WINDOWS
//#pragma warning(disable:4251)
//#endif
//
//
////#define CPPCONN_LIB_BUILD 
////
////#include <cppconn/sqlstring.h>
////#include <cppconn/prepared_statement.h>
////#include <mysql_connection.h>
//
//namespace zxero
//{
//	class redis_context;
//
//    class redis_work : public work
//    {
//    public:
//        redis_work();
//        virtual ~redis_work();
//        
//    public:
//        //  数据库工作处理接口，由各个具体的数据库工作处理
//        virtual void process(  boost::shared_ptr<redis_context>& dbctx ) = 0;
//        
//        //  数据库工作完成即可，有默认实现
//        virtual void done();
//        
//    private:
//        //  执行工作的具体函数
//        void do_work( boost::any& ctx );
//        //  工作完成处理的具体函数
//        void done_work( const boost::shared_ptr<work>& w );
//        
//    private:        
//        //
//        virtual const std::string& name() const = 0;
//        
//    protected:
//        //  db操作错误码
//        int redis_error_;
//    };
//    
//    
////  定义一个redis数据库工作
//#define DECLARE_REDIS_WORK( )  \
//    private:    \
//	static std::string sg_redis_work_name;	\
//	virtual const std::string& name() const { return sg_redis_work_name; }	\
//    virtual void process( boost::shared_ptr<redis_context>& dbctx )
//
////  实现及注册数据库工作
//#define IMPLEMENT_REDIS_WORK( cls ) \
//	std::string cls::sg_redis_work_name( #cls );	\
//    void cls::process( boost::shared_ptr<redis_context>& dbctx )
//
//}
//
//#endif /* defined(__zxero__rbwork__) */
