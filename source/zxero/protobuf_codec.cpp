#include "protobuf_codec.h"
#include "buffer.h"
#include "tcp_connection.h"
#include "google/protobuf/message.h"
#include "log.h"
#include <string>
using namespace std;

namespace zxero
{
	namespace network
	{
		////
		void byte_size_consistency_error(int byte_size_before_serialization, int byte_size_after_serialization, int bytes_produced_by_serialization)
		{
			CHECK_EQ(byte_size_before_serialization, byte_size_after_serialization)
				<< "protobuf message was modified concurrently during serialization";
			CHECK_EQ(bytes_produced_by_serialization, byte_size_before_serialization)
				<< "byte size calculation and serialization were inconsistent. this "
				"may indicate a bug in protocol buffers or it may be caused by "
				"concurrent modification of the message.";
			LOG_FATAL << "this shouldn't be called if all the size are equal";
		}
		
		std::string initialization_error_message(const char* action, const google::protobuf::MessageLite& msg)
		{
			std::string result;
			result += "can't ";
			result += action;
			result += " message of type \"";
			result += msg.GetTypeName();
			result += "\" because it is missing required fields: ";
			result += msg.InitializationErrorString();
			return result;
		}

		////
		void protobuf_codec::fill_empty_buffer(bool int16_header, buffer* buf, const google::protobuf::Message& msg)
		{
			ZXERO_ASSERT(buf->readable_bytes() == 0);
			
			const std::string& name = msg.GetTypeName();
			uint32 name_len = name.length();

			buf->append_int8((int8)name_len);
			buf->append(name.c_str(), name_len);

			//	这个写法很好，也大概明白日志系统要咋玩了
			CHECK(msg.IsInitialized()) << initialization_error_message("serialize", msg);

			//
			int byte_size = msg.ByteSize();
			buf->ensure_writable_bytes(byte_size);

			uint8* start = (uint8*)buf->begin_write();
			uint8* end = msg.SerializeWithCachedSizesToArray(start);
			if (end - start != byte_size)
			{
				byte_size_consistency_error(byte_size, msg.ByteSize(), (int)(end - start));
			}
			buf->has_written(byte_size);

			//	没有校验逻辑
			
			ZXERO_ASSERT(buf->readable_bytes() == sizeof(int8) + name_len + byte_size);
			if (int16_header)
				buf->prepend_int16((int16)buf->readable_bytes());
			else
				buf->prepend_int32(buf->readable_bytes());
		}

		////
		namespace {
			const string kNoErrorStr = "NoError";
			const string kInvalidLengthStr = "InvalidLength";
			const string kInvalidNameLenStr = "InvalidNameLen";
			const string kUnknownMessageTypeStr = "UnknownMessageType";
			const string kParseErrorStr = "ParseError";
			const string kUnknownErrorStr = "UnknownError";
		}

		const string& protobuf_codec::error_code_to_string(ErrorCode ec)
		{
			switch (ec)
			{
			case kNoError: return kNoErrorStr;
			case kInvalidLength: return kInvalidLengthStr;
			case kInvalidNameLen: return kInvalidNameLenStr;
			case kUnknownMessageType: return kUnknownMessageTypeStr;
			case kParseError: return kParseErrorStr;
			default:
				return kUnknownErrorStr;
			}
		}

		void protobuf_codec::default_error_callback(const tcp_connection_ptr& conn, buffer* buf, timestamp_t t, ErrorCode ec)
		{
			t, buf;
			LOG_ERROR << "protobuf_codec::default_error_callback - " << error_code_to_string(ec);
			if (conn && conn->connected())
			{
				conn->shutdown();
			}
		}

		////
		protobuf_codec::protobuf_codec(bool int16_header, const protobuf_message_callback_t& cb)
			:message_callback_(cb)
			, error_callback_(default_error_callback)
			, k_int16_header(int16_header)
			, k_header_len(int16_header?sizeof(int16): sizeof(int32))
			, k_min_message_len(k_header_len + sizeof(int8))	//	没有结尾的checkSum
		{

		}

		protobuf_codec::protobuf_codec(bool int16_header, const protobuf_message_callback_t& cb, const error_callback_t& ecb)
			:message_callback_(cb)
			, error_callback_(ecb)
			, k_int16_header(int16_header)
			, k_header_len(int16_header ? sizeof(int16) : sizeof(int32))
			, k_min_message_len(k_header_len + sizeof(int8))	//	没有结尾的checkSum
		{

		}

		void protobuf_codec::on_message(const tcp_connection_ptr& conn, buffer* buf, timestamp_t st)
		{
			while (buf->readable_bytes() >= (size_t)(k_min_message_len + k_header_len))
			{
				const int32 len = k_int16_header ? buf->peek_int16() : buf->peek_int32();
				if (len > k_max_message_len || len < k_min_message_len)
				{
					error_callback_(conn, buf, st, kInvalidLength);
					break;
				}
				else if (buf->readable_bytes() >= (size_t)(len + k_header_len))
				{
					ErrorCode ec = kNoError;
					message_ptr msg = parse(buf->peek() + k_header_len, len, &ec);
					if (ec == kNoError && msg)
					{
						message_callback_(conn, msg, st);
						buf->retrieve(k_header_len + len);
					}
					else
					{
						error_callback_(conn, buf, st, ec);
						break;
					}
				}
				else
				{
					break;
				}
			}
		}

		
		google::protobuf::Message* protobuf_codec::create_message(const string& type_name)
		{
			google::protobuf::Message* msg = nullptr;
			const google::protobuf::Descriptor* descriptor =
				google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name.c_str());
			if (descriptor)
			{
				const google::protobuf::Message* prototype =
					google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
				if (prototype)
					msg = prototype->New();
			}
			return msg;
		}

		message_ptr protobuf_codec::parse(const char* buf, int len, ErrorCode* error)
		{
			message_ptr msg;

			int32 nameLen = *(uint8*)buf;
			if (nameLen > 1 && (size_t)nameLen <= len - sizeof(int8))
			{
				string type_name(buf + sizeof(int8), nameLen);
				msg.reset(create_message(type_name));

				if (msg)
				{
					const char* data = buf + sizeof(int8) + nameLen;
					int32 dataLen = len - sizeof(int8) - nameLen;
					if (dataLen == 0 || msg->ParseFromArray(data, dataLen))
					{
						*error = kNoError;
					}
					else
					{
						//ZXERO_ASSERT(false)<<"kParseError assert!"<<buf;
						LOG_ERROR << "kParseError" << buf;
						*error = kParseError;
					}
				}
				else
				{
					//ZXERO_ASSERT(false)<<"kUnknownMessageType assert!"<<buf;
					LOG_ERROR << "kParseError" << buf;
					*error = kUnknownMessageType;
				}
			}
			else
			{
				//ZXERO_ASSERT(false)<<"kInvalidNameLen assert!"<<buf;
				LOG_ERROR << "kParseError" << buf;
				*error = kInvalidNameLen;
			}

			return msg;
		}

		void protobuf_codec::send(tcp_connection_ptr conn, const google::protobuf::Message& msg)
		{
			buffer buf;
			fill_empty_buffer(k_int16_header, &buf, msg);
			
			if (buf.readable_bytes() <= 10 * 1024 * 1024) //64*1024B
			{
				conn->send(&buf);
				if (buf.readable_bytes() >= 64 * 1024) {
					LOG_WARN << msg.GetTypeName() << " message too big, need tuning.(" << buf.readable_bytes() << ")";
				}
			}
			else
			{
				LOG_ERROR << "[Send Message] error:" << msg.GetTypeName() << " length:" << buf.readable_bytes() << " size:" << msg.ByteSize();
			}
			
		}
	}
}

