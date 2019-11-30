#ifndef __zxero__redis_context__
#define __zxero__redis_context__

#include "types.h"
#include "log.h"
#pragma warning(disable:4200) //非标准扩展: 结构/联合中的零大小数组
#include "hiredis.h"
#pragma warning(default:4200)

namespace zxero
{
	
	class redis;


	class redis_reply
	{
	public:
		friend class redis;
		redis_reply(void* reply)
			:reply_((redisReply*)reply)
		{

		}

	public:
		~redis_reply()
		{
			freeReplyObject(reply_);
			reply_ = nullptr;
		}

	public:
		int	type()
		{
			return reply_ ? reply_->type : 0;
		}

		int data_len()
		{
			return reply_ ? reply_->len : 0;
		}

		const char* data()
		{
			return reply_ ? reply_->str : nullptr;
		}

		std::string msg()
		{
			return reply_ ? std::string(reply_->str, reply_->len) : std::string();
		}

	private:
		redisReply* reply_;
	};

	/////////////////////////////////////////////////////////////
	class redis_context: boost::noncopyable
	{
		redisContext* context_;
	public:
		redis_context(const std::string& ip, int port, const std::string& pass = std::string())
			:context_(nullptr)
			, port_(port)
			, ip_(ip)
			, password_(pass)
		{

			LOG_INFO << "redis start " << ip.c_str()<<" port "<<port<<" pass "<<pass.c_str();
			connect();
		}

		~redis_context()
		{
			redisFree(context_);
		}

		bool connect()
		{
			if (context_)
			{
				redisFree(context_);
				context_ = nullptr;
			}

			context_ = redisConnect(ip_.c_str(), port_);
			if (context_ == nullptr || context_->err)
				return false;

			if (!password_.empty())
			{
				redis_reply rr = command(std::string("AUTH ") + password_);
				if (!(rr.type() == REDIS_REPLY_STATUS && rr.msg() == "OK"))
				{
					//	授权失败
					redisFree(context_);
					context_ = nullptr;
					return false;
				}
			}

			return true;
		}

		std::string error_msg()
		{
			return context_ ? context_->errstr : "redis context not exist";
		}

		redis_reply command(const std::string& cmd)
		{
			//	考虑下是否需要ping一下redis服务器
			return redis_reply(redisCommand(context_, cmd.c_str()));
		}

	public:
		int port_;
		std::string ip_;
		std::string password_;
	public:
		typedef boost::shared_ptr<redis_context> work_ctx_t;
	};

	typedef boost::shared_ptr<redis_context> redis_context_ptr;

}

#endif	//#ifndef __zxero__urlcontext__

