//
//  work.h
//  zxero
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014年 zxero. All rights reserved.
//

#ifndef __zxero__work__
#define __zxero__work__

#include "types.h"
#include <boost/function.hpp>
#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace zxero
{
    //  工作接口，其实就是三个函数对象
    //  不过要注意这三个回调函数是在不同的线程调用的
    //  工作函数是会在工作线程执行的
    //  工作完成和失败回调都在主线程（work_pool的event_loop的线程）执行
    struct work : boost::noncopyable
    {
		virtual const std::string& name() const = 0;
		//设置任务执行成功的标志
		void mark_success() { success_ = true; }
		//	zXero: 这个必须是虚函数哦，否则后续就是蛋疼的
		virtual ~work() {};

        //  工作所必需的函数原型
        
        //  工作函数
        typedef boost::function< void(boost::any&) >   work_callback_t;
        
        //  工作完成回调
        typedef boost::function< void(const boost::shared_ptr<work>&) > work_done_callback_t;
        //  貌似完成的情况下，成功与失败都不重要
        ////  工作失败回调
        //typedef boost::function< void(const work_ptr&) > work_failed_callback_t;
        
        //  工作类型
        enum work_type_t {
            //  随机工作，工作可被任意队列执行
            WT_RANDOM,
            //  序列工作，同类型工作必须先后顺序执行完成
            WT_SEQUENCE,
        };
        work_type_t work_type_;
        //  工作标示
        uint32  id_;
        
        //  主工作函数, 必须存在
        work_callback_t work_;
        //  工作完成函数, 必须存在
        work_done_callback_t done_;
		bool success_ = false;// 工作完成情况(如果失败了, 写个日志  [10/28/2017 Ryan]
        ////  工作错误函数, 可选存在, 有默认实现
        //work_failed_callback_t failed_;
    };

}

#endif /* defined(__zxero__work__) */
