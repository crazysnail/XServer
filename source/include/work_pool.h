//
//  work_pool.h
//  zxero
//
//  工作池，用于异步的处理工作，并在任务结束后将任务结果投递回主逻辑线程
//
//  Created by zXero on 14-3-16.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef zxero_work_pool_h
#define zxero_work_pool_h

#include "types.h"
#include "thread.h"
#include <deque>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/noncopyable.hpp>
#include <boost/any.hpp>

namespace zxero
{
    
    //  工作池
    class work_pool : boost::noncopyable
    {
    public:
        typedef boost::function< boost::any (work_thread*) > work_thread_init_t;
        static boost::any default_work_thread_init(work_thread*);
        
    public:
        work_pool(network::event_loop*, const std::string& name );
        ~work_pool();
        
    public:
        //  启动工作池
        void start(int num_thread, const work_thread_init_t& cb = default_work_thread_init);
        //  结束工作池
        void stop();
        //  有暂停的需求么
		    uint32 work_count() const { return works_.size(); }
		    uint32 thread_count() const { return threads_.size(); }
		    uint32 free_thread_count() const { return free_threads_.size(); }
        const std::string& name() const;
        //尚有线程正在工作
        bool working() const;
    public:
        //  添加要执行的工作，返回对应的工作id
        //  需要超时机制么？
        uint32 add_work( work* );
        //  需要提供终止工作的接口么？
        
    private:
        //
        friend class work_thread;
        
        //  释放工作线程
        void free_work_thread( work_thread* wt );
        
        //  调度线程，负责安排工作到线程中执行
        void schedule_work();
        
    private:
        //  事件循环
        network::event_loop* loop_;
        //  工作池名称
        const std::string name_;
        //  是否在工作中
        bool working_;
        //  工作线程
        thread_t manager_thread_;
        //  线程队列
        boost::ptr_vector<work_thread> threads_;
        //  空闲线程队列
        std::deque<work_thread*> free_threads_;
        //  工作队列
		std::deque<boost::shared_ptr<work>> works_;
        
        //  工作信号量
        mutex work_mutex_;
        condition_variable_any work_cond_;
        
        //  空闲线程信号量
        mutex thread_mutex_;
        condition_variable_any thread_cond_;
    };
}

#endif
