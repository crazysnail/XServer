//
//  work.h
//  zxero
//
//  Created by zXero on 14-3-18.
//  Copyright (c) 2014�� zxero. All rights reserved.
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
    //  �����ӿڣ���ʵ����������������
    //  ����Ҫע���������ص��������ڲ�ͬ���̵߳��õ�
    //  ���������ǻ��ڹ����߳�ִ�е�
    //  ������ɺ�ʧ�ܻص��������̣߳�work_pool��event_loop���̣߳�ִ��
    struct work : boost::noncopyable
    {
		virtual const std::string& name() const = 0;
		//��������ִ�гɹ��ı�־
		void mark_success() { success_ = true; }
		//	zXero: ����������麯��Ŷ������������ǵ��۵�
		virtual ~work() {};

        //  ����������ĺ���ԭ��
        
        //  ��������
        typedef boost::function< void(boost::any&) >   work_callback_t;
        
        //  ������ɻص�
        typedef boost::function< void(const boost::shared_ptr<work>&) > work_done_callback_t;
        //  ò����ɵ�����£��ɹ���ʧ�ܶ�����Ҫ
        ////  ����ʧ�ܻص�
        //typedef boost::function< void(const work_ptr&) > work_failed_callback_t;
        
        //  ��������
        enum work_type_t {
            //  ��������������ɱ��������ִ��
            WT_RANDOM,
            //  ���й�����ͬ���͹��������Ⱥ�˳��ִ�����
            WT_SEQUENCE,
        };
        work_type_t work_type_;
        //  ������ʾ
        uint32  id_;
        
        //  ����������, �������
        work_callback_t work_;
        //  ������ɺ���, �������
        work_done_callback_t done_;
		bool success_ = false;// ����������(���ʧ����, д����־  [10/28/2017 Ryan]
        ////  ����������, ��ѡ����, ��Ĭ��ʵ��
        //work_failed_callback_t failed_;
    };

}

#endif /* defined(__zxero__work__) */
