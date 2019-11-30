#include "types.h"
#include "json/json.h" 
//#include <google/protobuf/descriptor.h>  
//#include <google/protobuf/message.h>  


namespace zxero
{
	void FormatToJson(Json::Value& value, const ::google::protobuf::Message& msg);
	std::string JsonToString(Json::Value& value);

}