//
//  work_pool.cpp
//  zxero
//
//  Created by zXero on 14-3-17.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#include "work_pool.h"
#include "work_thread.h"
#include "work.h"
#include "event_loop.h"
#include "log.h"

namespace zxero {
    
    using namespace network;
    
    work_pool::work_pool(event_loop* loop, const std::string& name)
    :loop_(loop)
    ,name_(name)
    ,working_(false)
    ,manager_thread_(boost::bind(&work_pool::schedule_work, this), name)
    {
        LOG_TRACE << "work_pool ctor[" << this << "] " << name;
    }
    
    work_pool::~work_pool()
    {
        ZXERO_ASSERT(!working_);
        LOG_TRACE << "work_pool dtor[" << this << "] " << name();
        //  检查是否有未执行的工作
        while ( works_.size() )
        {
            const boost::shared_ptr<work>& work = works_.front();
            LOG_WARN << "unfinish work " << work.get();
            works_.pop_front();
        }
    }
    
    void work_pool::start(int num_thread, const work_thread_init_t& cb)
    {
        if ( working_ )
        {
            LOG_WARN << "work_pool " << this << " already started with " << threads_.size() << " work thread, now need " << num_thread;
            return;
        }
        
        //  创建工作线程，同时将其都加入到空闲工作线程队列中
        for (int i = 0; i < num_thread; ++i) {
            threads_.push_back(new work_thread(this, i, cb));
            free_threads_.push_back(&threads_.back());
        }
        
        //  启动调度线程
        working_ = true;
        manager_thread_.start();
		LOG_DEBUG << "work pool " << this << " start";
    }
    
    void work_pool::stop()
    {
        if (!working_)
            return;
        
		LOG_DEBUG << "work pool " << this << " stop";
        {
            //  设置工作停止标记，激活信号量
            mutex::scoped_lock thread_lock(thread_mutex_);
            mutex::scoped_lock work_lock(work_mutex_);
            working_ = false;
            thread_cond_.notify_one();
            work_cond_.notify_one();
        }
        
		LOG_DEBUG << "work pool " << this << " stop thread";
        //  关闭所有工作线程
        for ( auto& t : threads_ )
        {
            t.stop();
        }
		LOG_DEBUG << "work pool " << this << " stop thread finish";

        {
            //  这里需要加锁清除线程对象么？
            mutex::scoped_lock lock(thread_mutex_);
            free_threads_.clear();
            threads_.clear();
        }
        
        //  检查是否有未执行的工作
        while ( works_.size() )
        {
            const boost::shared_ptr<work>& work = works_.front();
            LOG_WARN << "work pool " << this << " has unfinish work " << work;
            works_.pop_front();
        }
    }
    
    const std::string& work_pool::name() const
    {
        return name_;
    }
    
    uint32 work_pool::add_work(zxero::work * w)
    {
		//LOG_TRACE << "work pool " << this << " add work " << w;
        //  加锁，添加工作，然后唤醒调度
        mutex::scoped_lock lock(work_mutex_);
        works_.push_back(boost::shared_ptr<work>(w));
        work_cond_.notify_one();
        return 0;
    }
    
    void work_pool::free_work_thread(zxero::work_thread *wt)
    {
        //  通知主线程处理工作结果
		boost::shared_ptr<work> work;
        swap(wt->work_, work);
		if (!work->success_){
			LOG_ERROR << "work pool:" << name_ << ". work:" << work->name() << " fails.";
		}
		loop_->run_in_loop(boost::bind(work->done_, work));
        //  工作线程进入空闲队列，并激发信号
        mutex::scoped_lock lock(thread_mutex_);
        free_threads_.push_back(wt);
        thread_cond_.notify_one();
    }
    
    
    //  核心调度线程
    void work_pool::schedule_work()
    {
        work_thread* free_thread = nullptr;
        while (working_) {
            //  等待空闲的线程
            {
                mutex::scoped_lock lock(thread_mutex_);
                while (free_threads_.size() == 0)
                {
                    thread_cond_.wait(thread_mutex_);
                    if (!working_) return;
                }
                free_thread = free_threads_.front();
                free_threads_.pop_front();
            }
            
            //  等待可用的工作
            {
                mutex::scoped_lock lock(work_mutex_);
                while (works_.size() == 0) {
                    work_cond_.wait(work_mutex_);
                    if (!working_) return;
                }
                //  激活工作
                //  这里应该存在free_thread失效的可能性，其原因是在这执行前，stop已经完成threads_的清理了，那么这里的free_thread就是失效的
                free_thread->wakeup(works_.front());
                works_.pop_front();
				//LOG_INFO << "test url size:" << works_.size();
            }
        }
    }
    
    //
    boost::any work_pool::default_work_thread_init(work_thread* t)
    {
        return t;
    }

    bool work_pool::working() const
    {
      return std::any_of(threads_.begin(), threads_.end(), boost::bind(&work_thread::work_, _1) != nullptr);
    }

}

