//
//  work_pool.h
//  zxero
//
//  �����أ������첽�Ĵ��������������������������Ͷ�ݻ����߼��߳�
//
//  Created by zXero on 14-3-16.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
    
    //  ������
    class work_pool : boost::noncopyable
    {
    public:
        typedef boost::function< boost::any (work_thread*) > work_thread_init_t;
        static boost::any default_work_thread_init(work_thread*);
        
    public:
        work_pool(network::event_loop*, const std::string& name );
        ~work_pool();
        
    public:
        //  ����������
        void start(int num_thread, const work_thread_init_t& cb = default_work_thread_init);
        //  ����������
        void stop();
        //  ����ͣ������ô
		    uint32 work_count() const { return works_.size(); }
		    uint32 thread_count() const { return threads_.size(); }
		    uint32 free_thread_count() const { return free_threads_.size(); }
        const std::string& name() const;
        //�����߳����ڹ���
        bool working() const;
    public:
        //  ���Ҫִ�еĹ��������ض�Ӧ�Ĺ���id
        //  ��Ҫ��ʱ����ô��
        uint32 add_work( work* );
        //  ��Ҫ�ṩ��ֹ�����Ľӿ�ô��
        
    private:
        //
        friend class work_thread;
        
        //  �ͷŹ����߳�
        void free_work_thread( work_thread* wt );
        
        //  �����̣߳������Ź������߳���ִ��
        void schedule_work();
        
    private:
        //  �¼�ѭ��
        network::event_loop* loop_;
        //  ����������
        const std::string name_;
        //  �Ƿ��ڹ�����
        bool working_;
        //  �����߳�
        thread_t manager_thread_;
        //  �̶߳���
        boost::ptr_vector<work_thread> threads_;
        //  �����̶߳���
        std::deque<work_thread*> free_threads_;
        //  ��������
		std::deque<boost::shared_ptr<work>> works_;
        
        //  �����ź���
        mutex work_mutex_;
        condition_variable_any work_cond_;
        
        //  �����߳��ź���
        mutex thread_mutex_;
        condition_variable_any thread_cond_;
    };
}

#endif
