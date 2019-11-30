//
//  test_work_list_2.h
//  zxerotest
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __zxerotest__test_work_list_2__
#define __zxerotest__test_work_list_2__

#include <iostream>
#include <queue>
#include "types.h"
#include "thread.h"
#include "log.h"
#include <boost/ptr_container/ptr_vector.hpp>

using namespace zxero;

class work_pool;
class work_thread
{
public:
    work_thread(work_pool* pool);
    
    void wakeup( int w );
    
    void thread_work();
    
public:
    bool working_;
    int w_;
    work_pool* work_pool_;
    thread_t thread_;
    mutex mutex_;
    condition_variable_any cond_;
};

class work_pool
{
public:
    work_pool(int num);
    
    void stop();
    
    void add_work(int w);
    
public:
    void thread_work();
    
    void free_work_thread(work_thread*);
    
public:
    //
    bool working_;
    //  工作池线程，负责分配线程和具体工作
    thread_t thread_;
    ptr_vector<work_thread> threads_;
    std::queue<work_thread*> free_threads_;
    std::queue<int> work_list_;
    
    //  工作信号量
    mutex work_mutex_;
    condition_variable_any work_cond_;
    
    //  空闲线程信号量
    mutex work_thread_mutex_;
    condition_variable_any work_thread_cond_;
};

#endif /* defined(__zxerotest__test_work_list_2__) */
