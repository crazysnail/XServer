#include "urlcontext.h"
#include "urlwork.h"
#include "log.h"
#include "thread.h"

#include <boost/format.hpp>

namespace zxero {
	
	//////////////////////////////////////////////////////////////////////////
	class curl_global_initializer
	{
		bool initialize_;
	public:
		curl_global_initializer()
			:initialize_(false)
		{
			//	初始化curl系统
			auto res = curl_global_init(CURL_GLOBAL_ALL);
			if (res != CURLE_OK)
			{
				LOG_ERROR << "curl global init failed: " << curl_easy_strerror(res);
			}
			else
			{
				initialize_ = true;
				auto version = curl_version_info(CURLVERSION_NOW);
				LOG_INFO << "curl init start: age " << version->age << ", version " << version->version << ", host " << version->host;
			}
		}

		~curl_global_initializer()
		{
			//	反初始化curl系统
			if (initialize_)
				curl_global_cleanup();
			initialize_ = false;
		}
	};
	static curl_global_initializer sg_curl_global_initializer;

	//////////////////////////////////////////////////////////////////////////
	urlcontext::urlcontext()
		:curl_(nullptr)
		,post_(nullptr)
		,last_(nullptr)
		,result_(CURLE_OK)
	{
		LOG_TRACE << "url context ctor [" << this << "]";

		curl_ = curl_easy_init();
		ZXERO_ASSERT(curl_ != nullptr) << " curl handler init failed ";

		//	默认参数
		curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, curl_write_data);
		curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);
		
		header_data_.header = true;
		header_data_.ctx = this;
		curl_easy_setopt(curl_, CURLOPT_HEADERDATA, (void*)&header_data_);

		body_data_.header = false;
		body_data_.ctx = this;
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void*)&body_data_);
	}

	urlcontext::~urlcontext()
	{
		if (post_)
		{
			curl_formfree(post_);
			post_ = last_ = nullptr;
		}
		if (curl_)
		{
			curl_easy_cleanup(curl_);
			curl_ = nullptr;
		}
	}

	void urlcontext::set_url(const std::string& str)
	{
		curl_easy_setopt(curl_, CURLOPT_URL, str.c_str());
	}

	void urlcontext::add_post_param(const std::string& key, const std::string& value)
	{
		curl_formadd(&post_, &last_,
			CURLFORM_COPYNAME, key.c_str(),
			CURLFORM_COPYCONTENTS, value.c_str(),
			CURLFORM_END);
	}

	void urlcontext::add_post_param(const std::string& key, const char* value)
	{
		curl_formadd(&post_, &last_,
			CURLFORM_COPYNAME, key.c_str(),
			CURLFORM_COPYCONTENTS, value,
			CURLFORM_END);

	}

	void urlcontext::request()
	{
		load_data_.clear();
		if (post_)
		{
			curl_easy_setopt(curl_, CURLOPT_HTTPPOST, post_);
		}

		result_ = CURLE_OK;
		result_ = curl_easy_perform(curl_);
		if (post_)
		{
			curl_formfree(post_);
			post_ = last_ = nullptr;
		}
	}

	const std::string& urlcontext::get_data() const
	{
		return load_data_;
	}

	const char* urlcontext::get_error() const
	{
		return curl_easy_strerror(result_);
	}

	uint32 urlcontext::get_response_code()
	{
		long code = 0;
		auto res = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &code);
		if (res != CURLE_OK)
		{
			LOG_WARN << "url context [" << this << "] get response code failed: " << curl_easy_strerror(res);
			return 0;
		}

		return (uint32)code;
	}

	//
	size_t urlcontext::curl_write_data(void* ptr, size_t size, size_t nmemb, url_write_data* stream)
	{
		ZXERO_ASSERT(ptr != nullptr);
		ZXERO_ASSERT(size * nmemb != 0);
		ZXERO_ASSERT(stream != nullptr);

		std::string data((char*)ptr, size * nmemb);
		if (stream->header)
			stream->ctx->on_header(data);
		else
			stream->ctx->on_data(data);

		return size * nmemb;
	}

	void urlcontext::on_header(const std::string& str)
	{
		str;
		//	nothing to do for now
	}

	void urlcontext::on_data(const std::string& str)
	{
		load_data_ += str;
	}

	template<>
	void urlcontext::add_param<bool>(const std::string& key, const bool& value) {
		add_post_param(key, (boost::format("%s") % (value ? "true" : "false")).str().c_str() );
	}

	template<>
	void urlcontext::add_param<std::string>(const std::string& key, const std::string& value) {
		add_post_param(key, value.c_str());
	}

	//template<>
	//void urlcontext::add_param<std::string>(const std::string& key, const std::string& value) {
	//	add_post_param(key, value.c_str());
	//}

	//template<>
	//void urlcontext::add_param< std::basic_string<char> >(const string& key, std::basic_string<char> value) {
	//	add_post_param(key, value.c_str());
	//}

#undef tpl_helper
#define tpl_helper(type, fmt)	\
	template<> void urlcontext::add_param<type>(const std::string& key, const type& value) {\
		add_post_param(key, (boost::format(fmt) % value).str().c_str() );	}

	tpl_helper(int8, "%d");
	tpl_helper(int16, "%d");
	tpl_helper(int32, "%d");
	tpl_helper(int64, "%lld");
	tpl_helper(uint8, "%u");
	tpl_helper(uint16, "%u");
	tpl_helper(uint32, "%u");
	tpl_helper(uint64, "%llu");
	tpl_helper(real32, "%g");
	tpl_helper(real64, "%g");


#undef tpl_helper
}

