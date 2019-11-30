#include "buffer.h"
#include "tcp_connection.h"
#include "log.h"
#include "http_codec.h"
#include "urlcodeing.h"
#include "W2GMessage.pb.h"

#include "utils.h"
using namespace std;
namespace zxero
{
	namespace network
	{

#define MAX_ELEMENT_SIZE 2048
		size_t
			strlncat(char *dst, size_t len, const char *src, size_t n)
		{
			size_t slen;
			size_t dlen;
			size_t rlen;
			size_t ncpy;

			slen = strnlen(src, n);
			dlen = strnlen(dst, len);

			if (dlen < len) {
				rlen = len - dlen;
				ncpy = slen < rlen ? slen : (rlen - 1);
				memcpy(dst + dlen, src, ncpy);
				dst[dlen + ncpy] = '\0';
			}

			ZXERO_ASSERT(len > slen + dlen);
			return slen + dlen;
		}

		http_codec::http_codec(http_message_callback_t message_callback)
		{
			message_callback_ = message_callback;
			parser_settings.on_message_begin = boost::bind(&http_codec::on_parser_begin, this, _1);
			parser_settings.on_message_complete = boost::bind(&http_codec::on_parser_complete, this, _1);
			parser_settings.on_headers_complete = boost::bind(&http_codec::on_parser_headers_complete, this, _1);
			parser_settings.on_header_field = boost::bind(&http_codec::on_parser_header_field_cb, this, _1, _2, _3);
			parser_settings.on_header_value = boost::bind(&http_codec::on_parser_header_value_cb, this, _1, _2, _3);
			parser_settings.on_url = boost::bind(&http_codec::on_parser_url_cb, this, _1, _2, _3);
			parser_settings.on_body = boost::bind(&http_codec::on_parser_body_cb, this, _1, _2, _3);
		}

		int http_codec::on_parser_begin(http_parser *parser)
		{
			parser;
			return 0;
		}
		int http_codec::on_parser_complete(http_parser *parser)
		{
			parser;
			return 0;
		}
		int http_codec::on_parser_headers_complete(http_parser *parser)
		{
			parser;
			return 0;
		}
		int http_codec::on_parser_header_field_cb(http_parser *parser, const char *buf, size_t len)
		{
			try
			{
				parser;
				//char tempbuf[MAX_ELEMENT_SIZE] = { 0 };
				//strlncat(tempbuf, MAX_ELEMENT_SIZE, buf, len);
				//std::string header_field = tempbuf;
				std::string header_field(buf,len);
				if (header_field == "Content-Length")
				{
					Packet::HttpData* request = (Packet::HttpData*)parser->data;
					if (request)
					{
						request->set_head_length(true);
					}
				}
				LOG_INFO << "http_codec on_parser_header_field_cb:" << header_field;
			}
			catch (boost::bad_any_cast& e)
			{
				ZXERO_ASSERT(false) << "http_codec::on_parser_header_value_cb have catch: " << e.what();
			}
			return 0;
		}
		int http_codec::on_parser_header_value_cb(http_parser *parser, const char *buf, size_t len)
		{
			try
			{
				parser;
				//char tempbuf[MAX_ELEMENT_SIZE] = { 0 };
				//strlncat(tempbuf, MAX_ELEMENT_SIZE, buf, len);
				//std::string header_value = tempbuf;
				std::string header_value(buf, len);
				Packet::HttpData* request = (Packet::HttpData*)parser->data;
				if (request)
				{
					if (request->head_length() == true )
					{
						request->set_length(boost::lexical_cast<int32>(header_value));
						request->set_head_length(false);
					}
				}
				LOG_INFO << "http_codec on_parser_header_value_cb:" << header_value;
			}
			catch (boost::bad_any_cast& e)
			{
				ZXERO_ASSERT(false) << "http_codec::on_parser_header_value_cb have catch: " << e.what();
			}
			return 0;
		}
		int http_codec::on_parser_url_cb(http_parser *parser, const char *buf, size_t len)
		{
			try
			{
				//char tempbuf[MAX_ELEMENT_SIZE] = { 0 };
				//strlncat(tempbuf, MAX_ELEMENT_SIZE, buf, len);
				//std::string url = tempbuf;
				std::string url(buf,len);
				Packet::HttpData* request = (Packet::HttpData*)parser->data;
				if (request)
				{
					request->set_url(url);
				}
				LOG_INFO << "http_codec on_parser_url_cb:" << url;
			}
			catch (boost::bad_any_cast& e)
			{
				ZXERO_ASSERT(false) << "http_codec::on_parser_url_cb have catch: " << e.what();
			}
			return 0;
		}
		int http_codec::on_parser_body_cb(http_parser *parser, const char *buf, size_t len)
		{
			try
			{
				//char tempbuf[MAX_ELEMENT_SIZE] = { 0 };
				//strlncat(tempbuf, MAX_ELEMENT_SIZE, buf, len);
				//std::string body = tempbuf;
				std::string body(buf, len);
				Packet::HttpData* request = (Packet::HttpData*)parser->data;
				if (request)
				{
					if ((int32)len == request->length())
					{
						request->set_post_finish(true);
					}
					else
					{
						request->set_post_finish(false);
					}
					std::vector<std::string> parm;
					utils::split_string(body.c_str(),"&", parm);
					for (auto str : parm)
					{
						std::vector<std::string> data;
						utils::split_string(str.c_str(),"=", data);
						if (data.size() == 2)
						{
							auto postdata = request->add_post();
							if (postdata)
							{
								postdata->set_key(data[0]);
								postdata->set_value(strCoding::UrlUTF8Decode(data[1]));
							}
						}
					}
				
				}
				LOG_INFO << "http_codec on_parser_body_cb:"<< body<<" bodysize:"<< len;
			}
			catch (boost::bad_any_cast& e)
			{
				ZXERO_ASSERT(false) << "http_codec::on_parser_body_cb have catch: " << e.what();
			}
			return 0;
		}

		void http_codec::on_message(const tcp_connection_ptr& conn, buffer* buf, timestamp_t st)
		{
			try
			{
				std::string result = buf->to_stdstring();
				std::string resulrlog = result;
				while (resulrlog.find("\r\n") != std::string::npos)
				{
					boost::replace_first(resulrlog, "\r\n", "[#]");
				}
				LOG_INFO << "http_codec on_message begin " << resulrlog;
				boost::shared_ptr<http_parser> parser = boost::make_shared<http_parser>();
				if (parser == nullptr)
				{
					ZXERO_ASSERT(false) << "make_shared http_parser error";
					return;
				}

				http_parser_init(parser.get(), HTTP_REQUEST);

				boost::shared_ptr<Packet::HttpData> httpdata = boost::make_shared<Packet::HttpData>();
				parser->data = httpdata.get();
				http_parser_execute(parser.get(), &parser_settings, result.c_str(), result.length());
				LOG_INFO << "http_codec on_message call back url=" << httpdata->url()<<" postsize:"<< httpdata->post_size()<<" length:"<< httpdata->length()<<" finish:"<<httpdata->post_finish();
				if (httpdata->post_finish() || httpdata->length() == 0)
				{
					message_callback_(conn, httpdata, st);
				}
			}
			catch (boost::bad_any_cast& e)
			{
				ZXERO_ASSERT(false) << "http_codec::on_message [" << conn << "] have catch: " << e.what();
			}
		}
	}
}