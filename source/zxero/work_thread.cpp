//
//  work_thread.cpp
//  zxero
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
	//	���̻߳����Ĵ����ӳٵ��߳���ȥ
    ,context_(/*cb(this)*/)
	,callback_(cb)
    {
		LOG_TRACE << "work_thread ctor[" << this << "] " << thread_.name();
        //  �����˾Ϳ�������������
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
        
        //  ���������þ��幤����ͬʱ�����߳̿�ʼִ��
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
            //  ���û�й������͵ȴ��ź���
            if (!work_){
                cond_.wait(mutex_);
                //  �ź������������ת��ѭ����ʼ
                continue;
            }

			//
			//LOG_TRACE << "work thread " << this << " start work " << work_;
            
            //  ִ�о���Ĺ�������
            work_->work_(context_);
            
            //  ִ����ɺ󣬸�֪�̳߳��Լ�������
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
        
        //  ���ѹ����߳�֪ͨ��ر�
        {
            mutex::scoped_lock lock(mutex_);
            working_ = false;
            cond_.notify_one();
        }
        
        //  �ȴ��߳̽���
        thread_.join();

		LOG_INFO << "work thread " << this << " stoped";
    }
}

