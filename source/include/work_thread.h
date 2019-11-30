//
//  work_thread.h
//  zxero
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __zxero__work_thread__
#define __zxero__work_thread__

#include "types.h"
#include "thread.h"
#include <boost/any.hpp>

namespace zxero
{
    //  工作线程(这个类不一定需要露出来)
    class work_thread : boost::noncopyable
    {
    public:
        typedef boost::function< boost::any (work_thread*) > work_thread_init_t;
        
    public:
        work_thread(work_pool*, int32 id, const work_thread_init_t& cb = work_thread_init_t());
        ~work_thread();
        
    private:
        //  私有化的内部函数
        friend class work_pool;
        
        //  唤醒工作线程
        void wakeup( boost::shared_ptr<work>& w );
        
        //  工作线程函数
        void do_work();
        
        //  结束工作
        void stop();
        
        //
        std::string make_work_thread_name(work_pool*, int32);
        
    private:
        //  工作线程是否在工作中
        bool working_;
        //  工作线程池
        work_pool* pool_;
        //  当前工作
		boost::shared_ptr<work> work_;
        //  线程对象
        thread_t thread_;
        //  工作环境
        boost::any context_;
		//	工作线程初始化函数
		work_thread_init_t callback_;
        //  信号量
        mutex mutex_;
        condition_variable_any cond_;
    };
}

#endif /* defined(__zxero__work_thread__) */
