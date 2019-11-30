//
//  dbwork.cpp
//  zxero
//
//  Created by zXero on 14-3-26.
//  Copyright (c) 2014�� zxero. All rights reserved.
//

#include "dbwork.h"
#include "log.h"
#include "dbcontext.h"
#include<thread>
#include "date_time.h"
#include <cppconn/exception.h>

namespace zxero {
    
    //
    dbwork::dbwork()
    :sql_error_(0)
    {
        work_ = boost::bind( &dbwork::do_work, this, _1 );
        done_ = boost::bind( &dbwork::done_work, this, _1 );
		//LOG_TRACE << "dbwork ctor[" << this << "]";
    }
    
    dbwork::~dbwork()
    {
        //LOG_TRACE << "dbwork dtor[" << this << "]";
    }
    
    void dbwork::done()
    {
        if (sql_error_)
		{
            LOG_ERROR << "dbwork " << name() << " done failed, error code " << sql_error_;
        }
    }
    
    //////
    void dbwork::do_work(boost::any &ctx)
    {
        auto dbctx = boost::any_cast<dbcontext::work_ctx_t>(ctx);
		uint32 retryCount = 0;

work_start:
        try {
            process( dbctx );
			mark_success();
        } catch (sql::SQLException& e) {
            sql_error_ = e.getErrorCode();
 			if ((sql_error_ == 2003 || sql_error_ == 2013 || sql_error_ == 2006 )&& retryCount <= 10)
			{
				++retryCount;
				try
				{
					LOG_ERROR<<"close connection " <<sql_error_ <<" "<<dbctx->get_connection()->isClosed();
					
					//	ȷʵ�ر�����
					dbctx->get_connection()->close();
				}
				catch(sql::SQLException& e)
				{// ��ֹclose�쳣��ϵͳ�ػ�abort [6/18/2016 SEED]
					sql_error_ = EFAULT;
					LOG_ERROR << "db work error"<< name() << e.what();
				}
				catch(...)
				{
					sql_error_ = EFAULT;
					LOG_ERROR << "db work error" << name() << " unknown failed";
				}

				//	ѭ���ȴ������ɹ�����������10�Σ��ȴ�ʱ��Ϊ0s��10s��20s����������100s���ܼ�550s��������и�9���ӡ������Գ�
				uint32 retry = 0;
				while (retry <= 10)
				{
					//	��mysql�Ͽ�������, ���Եȴ�һ��ʱ��������
					std::this_thread::sleep_for(std::chrono::seconds(retry*10));
					++retry;

					auto result = false;
					try
					{
						LOG_INFO << "db context [" << dbctx << "] lost would reconnection retry time " << retry;
						result = dbctx->reconnection();
					}
					catch (sql::SQLException& re)
					{
						//	����ٴ�����ʧ�ܡ������ȴ���һ������
						auto error_code = re.getErrorCode();
						if (error_code == 2003 || error_code == 2006 || error_code == 2013)
						{
							LOG_INFO << "db context [" << dbctx << "] not connect done and will retry:" << error_code << "," << re.what();
							continue;
						}
						else
						{
							//	������������
							LOG_ERROR << "db work " << name() << " can not finished because db can not reset:" << error_code << "," << re.what();
							sql_error_ = re.getErrorCode();
							break;
						}
					}

					//	������ӳɹ���
					if (result)
					{
						sql_error_ = 0;
						//	�ٴ���������
						goto work_start;
					}
				}
				ZXERO_ASSERT(false) << "db work " << name() << " can not finished because db shutdown";
			}
			else{
				//�쳣�ͱ�����������ʱ����ؼ��洢����
				LOG_ERROR << "db work " << name() << " sql failed : " << e.what();
			}
		
        } catch (std::exception& e) {
            sql_error_ = errno;
            LOG_ERROR << "db work " << name() << " std failed : " << e.what();
        } catch ( ... ) {
            sql_error_ = EFAULT;
            LOG_ERROR << "db work " << name() << " unknown failed";
        }
    }
    
    void dbwork::done_work(const boost::shared_ptr<work> &w)
    {
		w;
        done();
    }
}
