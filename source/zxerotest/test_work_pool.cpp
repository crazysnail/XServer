//
//  test_work_pool.cpp
//  zxerotest
//
//  Created by zXero on 14-3-19.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "test_work_pool.h"
#include "log.h"
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/exception.h>


using namespace zxero;
using namespace network;

event_loop* g_loop = nullptr;

class my_work : public work
{
    int count;
public:
    my_work( int i )
    :count(i)
    {
        work_ = boost::bind(&my_work::work_func, this, _1);
        done_ = boost::bind(&my_work::done_func, this);
    }
    
private:
    //
    void work_func(work_context_t& t)
    {
        LOG_DEBUG << count;
    }
    
    void done_func()
    {
        LOG_DEBUG << "done " << count;
        if ( count == 999999 )
            g_loop->quit();
    }
};

void gen_works( work_pool* pool, int i )
{
    pool->add_work( new my_work( i ) );
}

work_context_t creator(work_thread*)
{
    return 1;
}

class db_context : noncopyable
{
private:
    boost::shared_ptr< sql::Driver >        driver_ptr_;
    boost::shared_ptr< sql::Connection >    connection_ptr_;
    map< string, boost::shared_ptr< sql::PreparedStatement > >  statements_;
    
public:
    db_context( const sql::SQLString& ip, const sql::SQLString& name, const sql::SQLString& password, const sql::SQLString& dbname, int32 port = 3306 )
    {
        sql::ConnectOptionsMap connection_properties;
        connection_properties["hostName"] = ip;
        connection_properties["userName"] = name;
        connection_properties["password"] = password;
        connection_properties["port"] = port;
        connection_properties["OPT_RECONNECT"] = true;
        connection_properties["schema"] = dbname;
        
        //  数据库驱动获取
        driver_ptr_.reset(sql::mysql::get_driver_instance());
        LOG_INFO << "db driver: " << driver_ptr_->getName()
        << driver_ptr_->getMajorVersion() << "."
        << driver_ptr_->getMinorVersion() << "."
        << driver_ptr_->getPatchVersion();
        
        //  建立数据库连接
        try {
            connection_ptr_.reset(driver_ptr_->connect(connection_properties));
        } catch (sql::SQLException& e) {
            LOG_ERROR << "db connection failed: " << e.what();
            throw e;
        }
        
        LOG_INFO << "connection is: " << connection_ptr_->getClientInfo();
        
        //  准备sql语句
        try {
            statements_["load_account"].reset(connection_ptr_->prepareStatement( "SELECT tguid, taccount, tpassword FROM t_account WHERE taccount = ?" ));
        } catch (sql::SQLException& e) {
            LOG_ERROR << "db prepare statement failed: " << e.what();
            throw e;
        }
    }
    
    //
    static work_context_t create_context(work_thread*)
    {

        //
        return boost::make_shared<db_context>("192.168.1.103", "sotcc", "123456", "sotcc");
    }
};

int main()
{
    log_message::log_level(LL_TRACE);
    event_loop loop;
    g_loop = &loop;
    
    work_pool pool(g_loop, "work");
    pool.start(8, db_context::create_context);
    
    for ( int i = 0; i < 1000000; ++i )
    {
        loop.queue_in_loop( boost::bind(gen_works, &pool, i) );
    }
    
    loop.loop();
    pool.stop();
    return 0;
}

