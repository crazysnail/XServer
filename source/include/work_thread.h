//
//  work_thread.h
//  zxero
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014�� zxero. All rights reserved.
//

#ifndef __zxero__work_thread__
#define __zxero__work_thread__

#include "types.h"
#include "thread.h"
#include <boost/any.hpp>

namespace zxero
{
    //  �����߳�(����಻һ����Ҫ¶����)
    class work_thread : boost::noncopyable
    {
    public:
        typedef boost::function< boost::any (work_thread*) > work_thread_init_t;
        
    public:
        work_thread(work_pool*, int32 id, const work_thread_init_t& cb = work_thread_init_t());
        ~work_thread();
        
    private:
        //  ˽�л����ڲ�����
        friend class work_pool;
        
        //  ���ѹ����߳�
        void wakeup( boost::shared_ptr<work>& w );
        
        //  �����̺߳���
        void do_work();
        
        //  ��������
        void stop();
        
        //
        std::string make_work_thread_name(work_pool*, int32);
        
    private:
        //  �����߳��Ƿ��ڹ�����
        bool working_;
        //  �����̳߳�
        work_pool* pool_;
        //  ��ǰ����
		boost::shared_ptr<work> work_;
        //  �̶߳���
        thread_t thread_;
        //  ��������
        boost::any context_;
		//	�����̳߳�ʼ������
		work_thread_init_t callback_;
        //  �ź���
        mutex mutex_;
        condition_variable_any cond_;
    };
}

#endif /* defined(__zxero__work_thread__) */
