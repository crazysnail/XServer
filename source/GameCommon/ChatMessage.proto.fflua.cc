// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ChatMessage.proto
#include "ChatMessage.pb.h"
#include "ChatMessage.proto.fflua.h"
namespace Packet {
bool ChatMessage_fflua_reg(lua_State* state)
{
//for decltype
	ChatMessage* message(nullptr);
	message;
	ff::fflua_register_t<ChatMessage, ctor()>(state, "ChatMessage", "google::protobuf::Message")
		// required .Packet.ChatChannel channel = 1;
		.def(&ChatMessage::channel, "channel")
		.def(&ChatMessage::set_channel, "set_channel")
		// required bytes content = 2;
		.def(&ChatMessage::content, "content")
		.def<void (ChatMessage::*)(const std::string&)>(&ChatMessage::set_content, "set_content")
		// required int32 chat_time = 3;
		.def(&ChatMessage::chat_time, "chat_time")
		.def(&ChatMessage::set_chat_time, "set_chat_time")
		// required fixed64 datetime = 4;
		.def(&ChatMessage::datetime, "datetime")
		.def(&ChatMessage::set_datetime, "set_datetime")
		// optional int32 hair = 5;
		.def(&ChatMessage::hair, "hair")
		.def(&ChatMessage::set_hair, "set_hair")
		.def(&ChatMessage::has_hair, "has_hair")
		// optional string name = 6;
		.def(&ChatMessage::name, "name")
		.def<void (ChatMessage::*)(const std::string&)>(&ChatMessage::set_name, "set_name")
		.def(&ChatMessage::has_name, "has_name")
		// optional .Packet.Race race = 7;
		.def(&ChatMessage::race, "race")
		.def(&ChatMessage::set_race, "set_race")
		.def(&ChatMessage::has_race, "has_race")
		// optional .Packet.Professions profession = 8;
		.def(&ChatMessage::profession, "profession")
		.def(&ChatMessage::set_profession, "set_profession")
		.def(&ChatMessage::has_profession, "has_profession")
		// optional int32 channel_id = 9;
		.def(&ChatMessage::channel_id, "channel_id")
		.def(&ChatMessage::set_channel_id, "set_channel_id")
		.def(&ChatMessage::has_channel_id, "has_channel_id")
		// optional fixed64 target_guid = 10;
		.def(&ChatMessage::target_guid, "target_guid")
		.def(&ChatMessage::set_target_guid, "set_target_guid")
		.def(&ChatMessage::has_target_guid, "has_target_guid")
		.def(&ChatMessage::ByteSize, "ByteSize")
		.def(&ChatMessage::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PrivateChatMessageReq_fflua_reg(lua_State* state)
{
//for decltype
	PrivateChatMessageReq* message(nullptr);
	message;
	ff::fflua_register_t<PrivateChatMessageReq, ctor()>(state, "PrivateChatMessageReq", "google::protobuf::Message")
		// required fixed64 target_guid = 1;
		.def(&PrivateChatMessageReq::target_guid, "target_guid")
		.def(&PrivateChatMessageReq::set_target_guid, "set_target_guid")
		.def(&PrivateChatMessageReq::ByteSize, "ByteSize")
		.def(&PrivateChatMessageReq::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PrivateChatMessageReply_fflua_reg(lua_State* state)
{
//for decltype
	PrivateChatMessageReply* message(nullptr);
	message;
	ff::fflua_register_t<PrivateChatMessageReply, ctor()>(state, "PrivateChatMessageReply", "google::protobuf::Message")
		// optional .Packet.PlayerBasicInfo playerinfo = 1;
		.def(&PrivateChatMessageReply::playerinfo, "playerinfo")
		.def(&PrivateChatMessageReply::mutable_playerinfo, "mutable_playerinfo")
		.def(&PrivateChatMessageReply::has_playerinfo, "has_playerinfo")
		.def(&PrivateChatMessageReply::ByteSize, "ByteSize")
		.def(&PrivateChatMessageReply::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ChatMessage_fflua_regist_all(lua_State* state)
{
	ChatMessage_fflua_reg(state);
	PrivateChatMessageReq_fflua_reg(state);
	PrivateChatMessageReply_fflua_reg(state);
	return true;
}
}
