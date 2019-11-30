#ifndef __zxero__urlcontext__
#define __zxero__urlcontext__

#include "types.h"
#include <string>
#include <curl/curl.h>
#include <boost/noncopyable.hpp>

namespace zxero
{
	class urlcontext : boost::noncopyable
	{
	private:
		//	用于
		CURL*	curl_;
	public:
		urlcontext();
		~urlcontext();

	public:
		void set_url(const std::string& str);
		void add_post_param(const std::string& key, const std::string& value);
		void add_post_param(const std::string& key, const char* value);

		template<typename T>
		void add_param(const std::string& key, const T& value);/* {
			ZXERO_ASSERT(false) << " unaccept post param type";
		}*/

		void request();

	public:
		const std::string& get_data() const;

		const char* get_error() const;

		uint32 get_response_code();

	private:
		//	读取到的数据
		std::string load_data_;

		//	post参数使用
		curl_httppost* post_;
		curl_httppost* last_;

		//
		CURLcode result_;
	private:
		//	用于区分不同类别的数据
		struct url_write_data
		{
			bool header;
			urlcontext* ctx;
		};
		static size_t curl_write_data(void* ptr, size_t size, size_t nmemb, url_write_data* stream);
	
		void on_header(const std::string& str);
		void on_data(const std::string& str);

		url_write_data header_data_;
		url_write_data body_data_;

	};


#undef tpl_helper
#define tpl_helper(type)	template<> void urlcontext::add_param<type>(const std::string& key, const type& value)

	tpl_helper(bool);
	tpl_helper(int8);
	tpl_helper(int16);
	tpl_helper(int32);
	tpl_helper(int64);

	tpl_helper(uint8);
	tpl_helper(uint16);
	tpl_helper(uint32);
	tpl_helper(uint64);

	tpl_helper(real32);
	tpl_helper(real64);
	tpl_helper(std::string);
	tpl_helper(std::string);

#undef tpl_helper
}

#endif	//#ifndef __zxero__urlcontext__

