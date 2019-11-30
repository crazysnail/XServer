//
//  test_work_list_2.cpp
//  zxerotest
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "test_work_list_2.h"

work_thread::work_thread(work_pool* pool)
:working_(true)
,w_(0)
,work_pool_(pool)
,thread_(boost::bind(&work_thread::thread_work, this), "work")
{
    thread_.start();
}

void work_thread::wakeup(int w)
{
    mutex::scoped_lock lock(mutex_);
    w_ = w;
    cond_.notify_all();
}

void work_thread::thread_work()
{
    mutex::scoped_lock lock(mutex_);
    while(working_)
    {
        cond_.wait(mutex_);
        if (!working_) {
            return;
        }
        LOG_INFO << w_;
        work_pool_->free_work_thread(this);
    }
}

////
work_pool::work_pool(int num)
:working_(true),thread_(boost::bind(&work_pool::thread_work, this), "work_pool")
{
    for ( int i = 0; i < num; ++i )
    {
        threads_.push_back(new work_thread(this));
        free_threads_.push(&threads_.back());
    }
    
    thread_.start();
}

void work_pool::stop()
{
    working_ = false;
    work_cond_.notify_one();
    work_thread_cond_.notify_one();
    for ( size_t i = 0; i < threads_.size(); ++i )
    {
        threads_[i].working_ = false;
        threads_[i].cond_.notify_one();
    }
}

void work_pool::add_work(int w)
{
    LOG_WARN << w ;
    mutex::scoped_lock lock(work_mutex_);
    work_list_.push(w);
    work_cond_.notify_one();
}

void work_pool::free_work_thread(work_thread* wt)
{
    mutex::scoped_lock lock(work_thread_mutex_);
    free_threads_.push(wt);
    work_thread_cond_.notify_one();
}

void work_pool::thread_work()
{
    int w = 0;
    work_thread* wt = nullptr;
    while (working_)
    {
        {
            //  先获取工作
            mutex::scoped_lock lock(work_mutex_);
            while (work_list_.size() == 0)
            {
                work_cond_.wait(work_mutex_);
            }
            if ( !working_ )
            {
                LOG_INFO << "work pool stoped";
                return;
            }
            w = work_list_.front();
            work_list_.pop();
        }
        {
            //  在获取工作线程
            mutex::scoped_lock lock(work_thread_mutex_);
            while (free_threads_.size() == 0)
            {
                work_thread_cond_.wait(work_thread_mutex_);
            }
            if ( !working_ )
            {
                LOG_INFO << "work pool stoped";
                return;
            }
            wt = free_threads_.front();
            free_threads_.pop();
        }
        
        wt->wakeup( w );
    }
}

int main()
{
    work_pool pool(8);
    
    for ( int i = 0; i < 1000000; ++i )
    {
        pool.add_work(i);
    }
    
    while ( pool.work_list_.size() )
    {
        this_thread::sleep( seconds(1) );
    }
    
    this_thread::sleep( seconds(20) );
    
    pool.stop();
    
    LOG_INFO << "all stop";
    
    return 0;
}




