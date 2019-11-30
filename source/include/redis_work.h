////
////  dbwork.h
////  zxero
////
////  Created by zXero on 16-9-12.
////  Copyright (c) 2016�� seed. All rights reserved.
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
//        //  ���ݿ⹤������ӿڣ��ɸ�����������ݿ⹤������
//        virtual void process(  boost::shared_ptr<redis_context>& dbctx ) = 0;
//        
//        //  ���ݿ⹤����ɼ��ɣ���Ĭ��ʵ��
//        virtual void done();
//        
//    private:
//        //  ִ�й����ľ��庯��
//        void do_work( boost::any& ctx );
//        //  ������ɴ���ľ��庯��
//        void done_work( const boost::shared_ptr<work>& w );
//        
//    private:        
//        //
//        virtual const std::string& name() const = 0;
//        
//    protected:
//        //  db����������
//        int redis_error_;
//    };
//    
//    
////  ����һ��redis���ݿ⹤��
//#define DECLARE_REDIS_WORK( )  \
//    private:    \
//	static std::string sg_redis_work_name;	\
//	virtual const std::string& name() const { return sg_redis_work_name; }	\
//    virtual void process( boost::shared_ptr<redis_context>& dbctx )
//
////  ʵ�ּ�ע�����ݿ⹤��
//#define IMPLEMENT_REDIS_WORK( cls ) \
//	std::string cls::sg_redis_work_name( #cls );	\
//    void cls::process( boost::shared_ptr<redis_context>& dbctx )
//
//}
//
//#endif /* defined(__zxero__rbwork__) */
