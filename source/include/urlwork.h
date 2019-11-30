#ifndef __zxero__urlwork_h__
#define __zxero__urlwork_h__

#include "types.h"
#include "work.h"

namespace zxero
{
	class urlcontext;

	class urlwork : public work
	{
	public:
		urlwork();
		virtual ~urlwork();

	public:
		//	执行具体工作
		virtual void process( boost::shared_ptr<urlcontext>& ctx ) = 0;

		//	请求完成处理
		virtual void done();

	private:
		//
		void do_work( boost::any& ctx );
		//
		void done_work( const boost::shared_ptr<work>& w );

	private:
		//
		virtual const std::string& name() const = 0;

		//
	protected:
		//
		uint32 response_code_;
	};

	//	定义一个url工作
	#define DECLARE_URLWORK()	\
	private:	\
	static std::string sg_urlwork_name;	\
	virtual const std::string& name() const { return sg_urlwork_name; }	\
	virtual void process( boost::shared_ptr<urlcontext>& ctx )

	//	实现url工作
	#define IMPLEMENT_URLWORK( cls )	\
	std::string cls::sg_urlwork_name( #cls );	\
	void cls::process( boost::shared_ptr<urlcontext>& ctx )
}

#endif	//	#ifndef __zxero__urlwork_h__

