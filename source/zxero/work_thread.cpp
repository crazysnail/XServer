//
//  work_thread.cpp
//  zxero
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "work_thread.h"
#include "work.h"
#include "work_pool.h"
#include "log.h"

namespace zxero {
    work_thread::work_thread(work_pool* pool, int32 id, const work_thread_init_t& cb)
    :working_(true)
    ,pool_(pool)
    ,work_(nullptr)
    ,thread_(boost::bind(&work_thread::do_work, this), make_work_thread_name(pool, id))
	//	将线程环境的创建延迟到线程上去
    ,context_(/*cb(this)*/)
	,callback_(cb)
    {
		LOG_TRACE << "work_thread ctor[" << this << "] " << thread_.name();
        //  创建了就可以启动工作了
        thread_.start();
    }
    
    work_thread::~work_thread()
    {
        ZXERO_ASSERT(!working_);
        ZXERO_ASSERT(!work_);
        LOG_TRACE << "work_thread dtor[" << this << "] " << thread_.name();
    }
    
    std::string work_thread::make_work_thread_name(work_pool * work_pool_ , int32 id)
    {
        char buf[128];
        zprintf( buf, sizeof(buf), "%s#%d", work_pool_->name().c_str(), id);
        return buf;
    }
    
    void work_thread::wakeup(boost::shared_ptr<work> &w)
    {
        ZXERO_ASSERT(working_);
        ZXERO_ASSERT(!work_);

		//LOG_TRACE << "work thread " << this << " wakeup by work " << w;
        
        //  加锁，设置具体工作，同时唤醒线程开始执行
        mutex::scoped_lock lock(mutex_);
        work_ = w;
        cond_.notify_one();
    }
    
    void work_thread::do_work()
    {
		if (callback_)
			context_ = callback_(this);

        mutex::scoped_lock lock(mutex_);
        while (working_) {
            //  如果没有工作，就等待信号量
            if (!work_){
                cond_.wait(mutex_);
                //  信号量被激活后，跳转到循环开始
                continue;
            }

			//
			//LOG_TRACE << "work thread " << this << " start work " << work_;
            
            //  执行具体的工作内容
            work_->work_(context_);
            
            //  执行完成后，告知线程池自己空闲了
            pool_->free_work_thread(this);
        }
        
        if ( work_ )
        {
			LOG_WARN << "work thread " << this << " not done work " << work_;
            work_.reset();
        }
    }
    
    void work_thread::stop()
    {
        if (!working_) {
            return;
        }

		LOG_INFO << "work thread " << this << " stop";
        
        //  唤醒工作线程通知其关闭
        {
            mutex::scoped_lock lock(mutex_);
            working_ = false;
            cond_.notify_one();
        }
        
        //  等待线程结束
        thread_.join();

		LOG_INFO << "work thread " << this << " stoped";
    }
}

