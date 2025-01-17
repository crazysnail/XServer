// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BaseConfig.proto
#include "BaseConfig.pb.h"
#include "BaseConfig.proto.fflua.h"
namespace Config {
bool ConstConfig_fflua_reg(lua_State* state)
{
//for decltype
	ConstConfig* message(nullptr);
	message;
	ff::fflua_register_t<ConstConfig, ctor()>(state, "ConstConfig", "google::protobuf::Message")
		// required string key = 1;
		.def(&ConstConfig::key, "key")
		.def<void (ConstConfig::*)(const std::string&)>(&ConstConfig::set_key, "set_key")
		// required int32 value1 = 2;
		.def(&ConstConfig::value1, "value1")
		.def(&ConstConfig::set_value1, "set_value1")
		// required int32 value2 = 3;
		.def(&ConstConfig::value2, "value2")
		.def(&ConstConfig::set_value2, "set_value2")
		// required int32 value3 = 4;
		.def(&ConstConfig::value3, "value3")
		.def(&ConstConfig::set_value3, "set_value3")
		// required int32 value4 = 5;
		.def(&ConstConfig::value4, "value4")
		.def(&ConstConfig::set_value4, "set_value4")
		.def(&ConstConfig::ByteSize, "ByteSize")
		.def(&ConstConfig::SetInitialized, "SetInitialized")
		;
	return true;
}
bool FunctionConfig_fflua_reg(lua_State* state)
{
//for decltype
	FunctionConfig* message(nullptr);
	message;
	ff::fflua_register_t<FunctionConfig, ctor()>(state, "FunctionConfig", "google::protobuf::Message")
		// required string name = 1;
		.def(&FunctionConfig::name, "name")
		.def<void (FunctionConfig::*)(const std::string&)>(&FunctionConfig::set_name, "set_name")
		// required int32 unlock = 2;
		.def(&FunctionConfig::unlock, "unlock")
		.def(&FunctionConfig::set_unlock, "set_unlock")
		// required int32 param = 3;
		.def(&FunctionConfig::param, "param")
		.def(&FunctionConfig::set_param, "set_param")
		.def(&FunctionConfig::ByteSize, "ByteSize")
		.def(&FunctionConfig::SetInitialized, "SetInitialized")
		;
	return true;
}
bool BaseConfig_fflua_regist_all(lua_State* state)
{
	ConstConfig_fflua_reg(state);
	FunctionConfig_fflua_reg(state);
	return true;
}
}
