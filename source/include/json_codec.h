//	基于json的编解码器
#ifndef _zxero_network_json_codec_h_
#define _zxero_network_json_codec_h_

#include "types.h"
#include "json/json.h"
#include "date_time.h"
#include <string>
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace zxero
{
	class buffer;
	class log_message;

	namespace network
	{
		typedef boost::shared_ptr<Json::Value> json_ptr;

		class json_codec : boost::noncopyable
		{
		public:
			enum ErrorCode
			{
				kNoError = 0,
				kInvalidLength,
				kParseError,
			};

			typedef boost::function<void(const boost::shared_ptr<tcp_connection>&, const json_ptr&, timestamp_t)> json_message_callback_t;
			typedef boost::function<void(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t, ErrorCode, Json::Reader*)> error_callback_t;
		
			json_codec(bool int16_header, const json_message_callback_t& cb);
			json_codec(bool int16_header, const json_message_callback_t& cb, const error_callback_t& );

			//
			void on_message(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t);
			void send(const boost::shared_ptr<tcp_connection>&, const Json::Value&);

			//
			static const std::string& error_code_to_string(ErrorCode errorCode);

		private:
			static void default_error_callback(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t, ErrorCode, Json::Reader*);

			json_message_callback_t message_callback_;
			error_callback_t error_callback_;

			//
			size_t last_check_;

			//
			Json::Reader reader_;
			Json::FastWriter writer_;

			const bool k_int16_header;
			const int32 k_header_len;
			const int32 k_min_message_len;
			const static int k_max_message_len = 64 * 1024 * 1024;
		};

	}	//	namespace network

	log_message& operator<<(log_message& s, const Json::Value& v);
	log_message& operator<<(log_message& s, const network::json_ptr& v);
}

#endif	//	#ifndef _zxero_network_json_codec_h_
