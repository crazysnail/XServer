//	protobuf编解码器，from muduo
#ifndef _zxero_network_protobuf_codec_h_
#define _zxero_network_protobuf_codec_h_

#include "types.h"
#include "date_time.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>

namespace zxero
{
	class buffer;

	namespace network
	{
		typedef boost::shared_ptr<google::protobuf::Message> message_ptr;
		
		class protobuf_codec : boost::noncopyable
		{
		public:
			enum ErrorCode
			{
				kNoError = 0,
				kInvalidLength,
				kInvalidNameLen,
				kUnknownMessageType,
				kParseError,
			};

			typedef boost::function<void(const boost::shared_ptr<tcp_connection>&, const boost::shared_ptr<google::protobuf::Message>&, timestamp_t)> protobuf_message_callback_t;
			typedef boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t, ErrorCode)> error_callback_t;

			protobuf_codec(bool int16_header, const protobuf_message_callback_t& cb);
			protobuf_codec(bool int16_header, const protobuf_message_callback_t&, const error_callback_t&);

			//	
			void on_message(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t);
			
			void send(boost::shared_ptr<tcp_connection>, const google::protobuf::Message&);

			static const std::string& error_code_to_string(ErrorCode errorCode);
			static void fill_empty_buffer(bool int16_header, buffer*, const google::protobuf::Message&);
			static google::protobuf::Message* create_message(const std::string& type_name);
			static boost::shared_ptr<google::protobuf::Message> parse(const char* buf, int buf_len, ErrorCode* errorCode);

			//	模板构造方法
			template<typename T>
			static boost::shared_ptr<T> create_message() {
				return boost::shared_ptr<T>((T*)create_message(T::default_instance().GetTypeName().c_str()));
			}

		private:
			static void default_error_callback(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t, ErrorCode);

			protobuf_message_callback_t message_callback_;
			error_callback_t error_callback_;

			const bool k_int16_header;
			const int32 k_header_len;
			const int32 k_min_message_len;
			const static int k_max_message_len = 64 * 1024 * 1024;
		};

	}
}

#endif	//	#ifndef _zxero_network_protobuf_codec_h_

