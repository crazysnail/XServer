#include "json_codec.h"
#include "buffer.h"
#include "tcp_connection.h"
#include "log.h"
#include <boost/make_shared.hpp>

namespace zxero
{
	namespace network
	{
		namespace {
			const std::string kNoErrorStr = "NoError";
			const std::string kInvalidLengthStr = "InvalidLength";
			const std::string kParseErrorStr = "ParseError";

			const std::string kUnknownErrorStr = "UnknownError";
		}

		const std::string& json_codec::error_code_to_string(ErrorCode errorCode)
		{
			switch(errorCode)
			{
			case kNoError: return kNoErrorStr;
			case kInvalidLength: return kInvalidLengthStr;
			case kParseError: return kParseErrorStr;

			default:
				return kUnknownErrorStr;
			}
		}

		//////////////////////////////////////////////////////////////////////////
		void json_codec::default_error_callback(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t t, ErrorCode ec, Json::Reader* reader)
		{
			t, buf;
			LOG_ERROR << "json_codec::default_error_callback - " << error_code_to_string(ec) 
				<< " "
				<< ((ec == kParseError && reader != nullptr) ? reader->getFormattedErrorMessages() : "" );
			if (conn && conn->connected())
			{
				//	zXero: SO IS HERE
				//conn->shutdown();
				conn->force_close();
			}
		}

		//////////////////////////////////////////////////////////////////////////
		json_codec::json_codec(bool int16_header, const json_message_callback_t& cb)
			:message_callback_(cb)
			,error_callback_(default_error_callback)
			,last_check_(0)
			,reader_()
			,writer_()
			,k_int16_header(int16_header)
			,k_header_len(int16_header ? sizeof(int16) : sizeof(int32))
			,k_min_message_len(0)	//	最小消息是一个空白消息，没有啥意义
		{

		}

		json_codec::json_codec(bool int16_header, const json_message_callback_t& cb, const error_callback_t& ec)
			:message_callback_(cb)
			,error_callback_(ec)
			,last_check_(0)
			,k_int16_header(int16_header)
			,k_header_len(int16_header ? sizeof(int16) : sizeof(int32))
			,k_min_message_len(0)	//	最小消息是一个空白消息，没有啥意义
		{

		}

		void json_codec::on_message(const boost::shared_ptr<tcp_connection>& conn, buffer* buf, timestamp_t st)
		{
			size_t total = buf->readable_bytes();
			if (total >= (size_t)k_max_message_len)
			{
				error_callback_(conn, buf, st, kInvalidLength, nullptr);
				return;
			}
			auto c = buf->peek();
			for ( ; last_check_ < buf->readable_bytes(); ++last_check_)
			{
				//	以换行符为命令结束标记，没有使用\r\n就使用了单\n，回复的时候使用\r\n
				if (*(c + last_check_) == '\n')
				{
					json_ptr msg = boost::make_shared<Json::Value>();
					//	解析json内容
					if (reader_.parse(c, c + last_check_, *msg, false))
					{
						//	通知上层接收json命令
						message_callback_(conn, msg, st);
						buf->retrieve(last_check_+1);
						last_check_ = 0;
					}
					else
					{
						error_callback_(conn, buf, st, kParseError, &reader_);
						break;
					}
				}
			}
			//while(buf->readable_bytes() >= (size_t)(k_min_message_len + k_header_len))
			//{
			//	const int32 len = k_int16_header ? buf->peek_int16() : buf->peek_int32();
			//	if (len > k_max_message_len || len < k_min_message_len)
			//	{
			//		error_callback_(conn, buf, st, kInvalidLength, nullptr);
			//		break;
			//	}
			//	else if (buf->readable_bytes() >= (size_t)(len + k_header_len))
			//	{
			//		json_ptr msg = make_shared<Json::Value>();
			//		if (reader_.parse(buf->peek() + k_header_len, buf->peek() + k_header_len + len, *msg, false))
			//		{
			//			message_callback_(conn, msg, st);
			//			buf->retrieve(k_header_len + len);
			//		}
			//		else
			//		{
			//			error_callback_(conn, buf, st, kParseError, &reader_);
			//			break;
			//		}
			//	}
			//}
		}

		void json_codec::send(const boost::shared_ptr<tcp_connection>& conn, const Json::Value& msg)
		{
			auto str = writer_.write(msg);
			conn->send(str.c_str(), str.length());
		}

	}	//	namespace network

	log_message& operator<<(log_message& s, const Json::Value& v)
	{
		s << v.asString();
		return s;
	}

	log_message& operator<<(log_message& s, const network::json_ptr& v)
	{
		s << v->asString();
		return s;
	}
}	//	namespace zxero
