#include "urlwork.h"
#include "urlcontext.h"
#include "log.h"
#include "thread.h"

namespace zxero
{
	urlwork::urlwork()
	{
		work_ = boost::bind( &urlwork::do_work, this, _1 );
		done_ = boost::bind( &urlwork::done_work, this, _1 );
		//LOG_TRACE << "urlwork ctor[" << this << "]";
	}

	urlwork::~urlwork()
	{
		//LOG_TRACE << "urlwork dtor[" << this << "]";
	}

	void urlwork::done()
	{
		//	nothing to do
		if (response_code_ != 200) {
			LOG_WARN << "urlwork " << name() << " done not finish, response code " << response_code_; 
		}
	}


	//////////////////////////////////////////////////////////////////////////
	void urlwork::do_work( boost::any& ctx )
	{
		auto urlctx = boost::any_cast<boost::shared_ptr<urlcontext>>(ctx);
		
		response_code_ = 0;
		try {
			//	准备处理工作参数
			process(urlctx);
			response_code_ = urlctx->get_response_code();
			mark_success();
		} catch (std::exception& e) {
			response_code_ = 0;
			LOG_ERROR << "url work " << name() << "failed : " << e.what();
		} catch ( ... ) {
			response_code_ = 0;
			LOG_ERROR << "url work " << name() << " unknown failed";
		}
	}

	void urlwork::done_work( const boost::shared_ptr<work>& w )
	{
		w;
		done();
	}
}
