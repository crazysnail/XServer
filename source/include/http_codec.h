//	protobuf±à½âÂëÆ÷£¬from muduo
#ifndef _zxero_network_http_codec_h_
#define _zxero_network_http_codec_h_

#include "types.h"
#include "date_time.h"
#include "http_parser.h"

namespace Packet
{
	class HttpData;
}
namespace zxero
{
	namespace network
	{
		typedef boost::function<void(const boost::shared_ptr<tcp_connection>&, const boost::shared_ptr<Packet::HttpData>&, timestamp_t)> http_message_callback_t;

		class http_codec : boost::noncopyable
		{
		public:
			http_codec(http_message_callback_t message_callback);
			~http_codec() {}
			void on_message(const boost::shared_ptr<tcp_connection>&, buffer*, timestamp_t);
			int on_parser_begin(http_parser *parser);
			int on_parser_complete(http_parser *parser);
			int on_parser_headers_complete(http_parser *parser);
			int on_parser_header_field_cb(http_parser *parser, const char *buf, size_t len);
			int on_parser_header_value_cb(http_parser *parser, const char *buf, size_t len);
			int on_parser_url_cb(http_parser *parser, const char *buf, size_t len);
			int on_parser_body_cb(http_parser *parser, const char *buf, size_t len);
		private:
			http_parser_settings parser_settings;

			http_message_callback_t message_callback_;
		};
	}
}
#endif