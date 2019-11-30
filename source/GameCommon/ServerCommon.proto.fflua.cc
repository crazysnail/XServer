// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ServerCommon.proto
#include "ServerCommon.pb.h"
#include "ServerCommon.proto.fflua.h"
namespace Packet {
bool ServerCommonRequest_fflua_reg(lua_State* state)
{
//for decltype
	ServerCommonRequest* message(nullptr);
	message;
	ff::fflua_register_t<ServerCommonRequest, ctor()>(state, "ServerCommonRequest", "google::protobuf::Message")
		// required string request_name = 1;
		.def(&ServerCommonRequest::request_name, "request_name")
		.def<void (ServerCommonRequest::*)(const std::string&)>(&ServerCommonRequest::set_request_name, "set_request_name")
		// repeated int32 int32_params = 2;
		.def(&ServerCommonRequest::int32_params_size, "int32_params_size")
		.def<decltype(message->int32_params(0)) (ServerCommonRequest::*)(int) const>(&ServerCommonRequest::int32_params, "int32_params")
		.def<void(ServerCommonRequest::*)(int, const decltype(message->int32_params(0)))>(&ServerCommonRequest::set_int32_params, "set_int32_params")
		.def(&ServerCommonRequest::add_int32_params, "add_int32_params")
		// repeated fixed64 int64_params = 3;
		.def(&ServerCommonRequest::int64_params_size, "int64_params_size")
		.def<decltype(message->int64_params(0)) (ServerCommonRequest::*)(int) const>(&ServerCommonRequest::int64_params, "int64_params")
		.def<void(ServerCommonRequest::*)(int, const decltype(message->int64_params(0)))>(&ServerCommonRequest::set_int64_params, "set_int64_params")
		.def(&ServerCommonRequest::add_int64_params, "add_int64_params")
		// repeated string string_params = 4;
		.def(&ServerCommonRequest::string_params_size, "string_params_size")
		.def<const std::string& (ServerCommonRequest::*)(int) const>(&ServerCommonRequest::string_params, "string_params")
		.def<void (ServerCommonRequest::*)(int, const std::string&)>(&ServerCommonRequest::set_string_params, "set_string_params")
		.def<void (ServerCommonRequest::*)(const std::string&)>(&ServerCommonRequest::add_string_params, "add_string_params")
		.def(&ServerCommonRequest::ByteSize, "ByteSize")
		.def(&ServerCommonRequest::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ServerCommonReply_fflua_reg(lua_State* state)
{
//for decltype
	ServerCommonReply* message(nullptr);
	message;
	ff::fflua_register_t<ServerCommonReply, ctor()>(state, "ServerCommonReply", "google::protobuf::Message")
		// required string reply_name = 1;
		.def(&ServerCommonReply::reply_name, "reply_name")
		.def<void (ServerCommonReply::*)(const std::string&)>(&ServerCommonReply::set_reply_name, "set_reply_name")
		// repeated int32 int32_params = 2;
		.def(&ServerCommonReply::int32_params_size, "int32_params_size")
		.def<decltype(message->int32_params(0)) (ServerCommonReply::*)(int) const>(&ServerCommonReply::int32_params, "int32_params")
		.def<void(ServerCommonReply::*)(int, const decltype(message->int32_params(0)))>(&ServerCommonReply::set_int32_params, "set_int32_params")
		.def(&ServerCommonReply::add_int32_params, "add_int32_params")
		// repeated fixed64 int64_params = 3;
		.def(&ServerCommonReply::int64_params_size, "int64_params_size")
		.def<decltype(message->int64_params(0)) (ServerCommonReply::*)(int) const>(&ServerCommonReply::int64_params, "int64_params")
		.def<void(ServerCommonReply::*)(int, const decltype(message->int64_params(0)))>(&ServerCommonReply::set_int64_params, "set_int64_params")
		.def(&ServerCommonReply::add_int64_params, "add_int64_params")
		// repeated string string_params = 4;
		.def(&ServerCommonReply::string_params_size, "string_params_size")
		.def<const std::string& (ServerCommonReply::*)(int) const>(&ServerCommonReply::string_params, "string_params")
		.def<void (ServerCommonReply::*)(int, const std::string&)>(&ServerCommonReply::set_string_params, "set_string_params")
		.def<void (ServerCommonReply::*)(const std::string&)>(&ServerCommonReply::add_string_params, "add_string_params")
		.def(&ServerCommonReply::ByteSize, "ByteSize")
		.def(&ServerCommonReply::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ServerCommonResult_fflua_reg(lua_State* state)
{
//for decltype
	ServerCommonResult* message(nullptr);
	message;
	ff::fflua_register_t<ServerCommonResult, ctor()>(state, "ServerCommonResult", "google::protobuf::Message")
		// required .Packet.ResultOption option = 1;
		.def(&ServerCommonResult::option, "option")
		.def(&ServerCommonResult::set_option, "set_option")
		// required .Packet.ResultCode code = 2;
		.def(&ServerCommonResult::code, "code")
		.def(&ServerCommonResult::set_code, "set_code")
		.def(&ServerCommonResult::ByteSize, "ByteSize")
		.def(&ServerCommonResult::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ServerCommon_fflua_regist_all(lua_State* state)
{
	ServerCommonRequest_fflua_reg(state);
	ServerCommonReply_fflua_reg(state);
	ServerCommonResult_fflua_reg(state);
	return true;
}
}
