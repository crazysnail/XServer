// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ClientMove.proto
#include "ClientMove.pb.h"
#include "ClientMove.proto.fflua.h"
namespace Packet {
bool ClientClickMove_fflua_reg(lua_State* state)
{
//for decltype
	ClientClickMove* message(nullptr);
	message;
	ff::fflua_register_t<ClientClickMove, ctor()>(state, "ClientClickMove", "google::protobuf::Message")
		// required fixed64 guid = 1;
		.def(&ClientClickMove::guid, "guid")
		.def(&ClientClickMove::set_guid, "set_guid")
		// optional .Packet.Position star_pos = 2;
		.def(&ClientClickMove::star_pos, "star_pos")
		.def(&ClientClickMove::mutable_star_pos, "mutable_star_pos")
		.def(&ClientClickMove::has_star_pos, "has_star_pos")
		// repeated .Packet.Position sub_positions = 3;
		.def(&ClientClickMove::sub_positions_size, "sub_positions_size")
		.def<decltype(message->sub_positions(0)) (ClientClickMove::*)(int) const>(&ClientClickMove::sub_positions, "sub_positions")
		.def<decltype(message->mutable_sub_positions(0)) (ClientClickMove::*)(int)>(&ClientClickMove::mutable_sub_positions, "mutable_sub_positions")
		.def(&ClientClickMove::add_sub_positions, "add_sub_positions")
		// optional int32 sub_position_index = 4;
		.def(&ClientClickMove::sub_position_index, "sub_position_index")
		.def(&ClientClickMove::set_sub_position_index, "set_sub_position_index")
		.def(&ClientClickMove::has_sub_position_index, "has_sub_position_index")
		.def(&ClientClickMove::ByteSize, "ByteSize")
		.def(&ClientClickMove::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ClientPlayerMove_fflua_reg(lua_State* state)
{
//for decltype
	ClientPlayerMove* message(nullptr);
	message;
	ff::fflua_register_t<ClientPlayerMove, ctor()>(state, "ClientPlayerMove", "google::protobuf::Message")
		// optional fixed64 guid = 1;
		.def(&ClientPlayerMove::guid, "guid")
		.def(&ClientPlayerMove::set_guid, "set_guid")
		.def(&ClientPlayerMove::has_guid, "has_guid")
		// required float direction = 2;
		.def(&ClientPlayerMove::direction, "direction")
		.def(&ClientPlayerMove::set_direction, "set_direction")
		// required fixed32 move_speed = 3;
		.def(&ClientPlayerMove::move_speed, "move_speed")
		.def(&ClientPlayerMove::set_move_speed, "set_move_speed")
		// required fixed32 move_time = 4;
		.def(&ClientPlayerMove::move_time, "move_time")
		.def(&ClientPlayerMove::set_move_time, "set_move_time")
		// required .Packet.Position old_pos = 5;
		.def(&ClientPlayerMove::old_pos, "old_pos")
		.def(&ClientPlayerMove::mutable_old_pos, "mutable_old_pos")
		// required .Packet.Position new_pos = 6;
		.def(&ClientPlayerMove::new_pos, "new_pos")
		.def(&ClientPlayerMove::mutable_new_pos, "mutable_new_pos")
		// optional bool follow = 7;
		.def(&ClientPlayerMove::follow, "follow")
		.def(&ClientPlayerMove::set_follow, "set_follow")
		.def(&ClientPlayerMove::has_follow, "has_follow")
		.def(&ClientPlayerMove::ByteSize, "ByteSize")
		.def(&ClientPlayerMove::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ClientPlayerStop_fflua_reg(lua_State* state)
{
//for decltype
	ClientPlayerStop* message(nullptr);
	message;
	ff::fflua_register_t<ClientPlayerStop, ctor()>(state, "ClientPlayerStop", "google::protobuf::Message")
		// optional fixed64 guid = 1;
		.def(&ClientPlayerStop::guid, "guid")
		.def(&ClientPlayerStop::set_guid, "set_guid")
		.def(&ClientPlayerStop::has_guid, "has_guid")
		// optional .Packet.Position old_pos = 2;
		.def(&ClientPlayerStop::old_pos, "old_pos")
		.def(&ClientPlayerStop::mutable_old_pos, "mutable_old_pos")
		.def(&ClientPlayerStop::has_old_pos, "has_old_pos")
		// required float direction = 3;
		.def(&ClientPlayerStop::direction, "direction")
		.def(&ClientPlayerStop::set_direction, "set_direction")
		// required .Packet.Position stop_pos = 4;
		.def(&ClientPlayerStop::stop_pos, "stop_pos")
		.def(&ClientPlayerStop::mutable_stop_pos, "mutable_stop_pos")
		// optional bool follow = 5;
		.def(&ClientPlayerStop::follow, "follow")
		.def(&ClientPlayerStop::set_follow, "set_follow")
		.def(&ClientPlayerStop::has_follow, "has_follow")
		.def(&ClientPlayerStop::ByteSize, "ByteSize")
		.def(&ClientPlayerStop::SetInitialized, "SetInitialized")
		;
	return true;
}
bool TeamMemberInfo_fflua_reg(lua_State* state)
{
//for decltype
	TeamMemberInfo* message(nullptr);
	message;
	ff::fflua_register_t<TeamMemberInfo, ctor()>(state, "TeamMemberInfo", "google::protobuf::Message")
		// required fixed64 memberid = 1;
		.def(&TeamMemberInfo::memberid, "memberid")
		.def(&TeamMemberInfo::set_memberid, "set_memberid")
		// required .Packet.PlayerTeamStatus teamstatus = 2;
		.def(&TeamMemberInfo::teamstatus, "teamstatus")
		.def(&TeamMemberInfo::set_teamstatus, "set_teamstatus")
		.def(&TeamMemberInfo::ByteSize, "ByteSize")
		.def(&TeamMemberInfo::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerTeamData_fflua_reg(lua_State* state)
{
//for decltype
	PlayerTeamData* message(nullptr);
	message;
	ff::fflua_register_t<PlayerTeamData, ctor()>(state, "PlayerTeamData", "google::protobuf::Message")
		// required fixed64 teamid = 1;
		.def(&PlayerTeamData::teamid, "teamid")
		.def(&PlayerTeamData::set_teamid, "set_teamid")
		// required fixed64 leaderid = 3;
		.def(&PlayerTeamData::leaderid, "leaderid")
		.def(&PlayerTeamData::set_leaderid, "set_leaderid")
		// required .Packet.ArrangeType arrangetype = 4;
		.def(&PlayerTeamData::arrangetype, "arrangetype")
		.def(&PlayerTeamData::set_arrangetype, "set_arrangetype")
		// required bool group = 5;
		.def(&PlayerTeamData::group, "group")
		.def(&PlayerTeamData::set_group, "set_group")
		// repeated .Packet.TeamMemberInfo member = 6;
		.def(&PlayerTeamData::member_size, "member_size")
		.def<decltype(message->member(0)) (PlayerTeamData::*)(int) const>(&PlayerTeamData::member, "member")
		.def<decltype(message->mutable_member(0)) (PlayerTeamData::*)(int)>(&PlayerTeamData::mutable_member, "mutable_member")
		.def(&PlayerTeamData::add_member, "add_member")
		// optional bool robot_team = 7;
		.def(&PlayerTeamData::robot_team, "robot_team")
		.def(&PlayerTeamData::set_robot_team, "set_robot_team")
		.def(&PlayerTeamData::has_robot_team, "has_robot_team")
		.def(&PlayerTeamData::ByteSize, "ByteSize")
		.def(&PlayerTeamData::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerRunTimeStatus_fflua_reg(lua_State* state)
{
//for decltype
	PlayerRunTimeStatus* message(nullptr);
	message;
	ff::fflua_register_t<PlayerRunTimeStatus, ctor()>(state, "PlayerRunTimeStatus", "google::protobuf::Message")
		// required bool on_hook = 1;
		.def(&PlayerRunTimeStatus::on_hook, "on_hook")
		.def(&PlayerRunTimeStatus::set_on_hook, "set_on_hook")
		// required bool in_battle = 2;
		.def(&PlayerRunTimeStatus::in_battle, "in_battle")
		.def(&PlayerRunTimeStatus::set_in_battle, "set_in_battle")
		// required .Packet.PlayerTeamData team = 3;
		.def(&PlayerRunTimeStatus::team, "team")
		.def(&PlayerRunTimeStatus::mutable_team, "mutable_team")
		// required .Packet.GuildPosition guildposition = 4;
		.def(&PlayerRunTimeStatus::guildposition, "guildposition")
		.def(&PlayerRunTimeStatus::set_guildposition, "set_guildposition")
		.def(&PlayerRunTimeStatus::ByteSize, "ByteSize")
		.def(&PlayerRunTimeStatus::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerRunTimeStatusNotify_fflua_reg(lua_State* state)
{
//for decltype
	PlayerRunTimeStatusNotify* message(nullptr);
	message;
	ff::fflua_register_t<PlayerRunTimeStatusNotify, ctor()>(state, "PlayerRunTimeStatusNotify", "google::protobuf::Message")
		// required fixed64 guid = 1;
		.def(&PlayerRunTimeStatusNotify::guid, "guid")
		.def(&PlayerRunTimeStatusNotify::set_guid, "set_guid")
		// required .Packet.PlayerRunTimeStatus status = 2;
		.def(&PlayerRunTimeStatusNotify::status, "status")
		.def(&PlayerRunTimeStatusNotify::mutable_status, "mutable_status")
		.def(&PlayerRunTimeStatusNotify::ByteSize, "ByteSize")
		.def(&PlayerRunTimeStatusNotify::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ScenePlayerInfo_fflua_reg(lua_State* state)
{
//for decltype
	ScenePlayerInfo* message(nullptr);
	message;
	ff::fflua_register_t<ScenePlayerInfo, ctor()>(state, "ScenePlayerInfo", "google::protobuf::Message")
		// required .Packet.PlayerBasicInfo basic_info = 1;
		.def(&ScenePlayerInfo::basic_info, "basic_info")
		.def(&ScenePlayerInfo::mutable_basic_info, "mutable_basic_info")
		// repeated .Packet.EquipItem main_equips = 2;
		.def(&ScenePlayerInfo::main_equips_size, "main_equips_size")
		.def<decltype(message->main_equips(0)) (ScenePlayerInfo::*)(int) const>(&ScenePlayerInfo::main_equips, "main_equips")
		.def<decltype(message->mutable_main_equips(0)) (ScenePlayerInfo::*)(int)>(&ScenePlayerInfo::mutable_main_equips, "mutable_main_equips")
		.def(&ScenePlayerInfo::add_main_equips, "add_main_equips")
		// optional .Packet.PlayerRunTimeStatus runtime_status = 3;
		.def(&ScenePlayerInfo::runtime_status, "runtime_status")
		.def(&ScenePlayerInfo::mutable_runtime_status, "mutable_runtime_status")
		.def(&ScenePlayerInfo::has_runtime_status, "has_runtime_status")
		.def(&ScenePlayerInfo::ByteSize, "ByteSize")
		.def(&ScenePlayerInfo::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ScenePlayerList_fflua_reg(lua_State* state)
{
//for decltype
	ScenePlayerList* message(nullptr);
	message;
	ff::fflua_register_t<ScenePlayerList, ctor()>(state, "ScenePlayerList", "google::protobuf::Message")
		// repeated .Packet.ScenePlayerInfo player_list = 1;
		.def(&ScenePlayerList::player_list_size, "player_list_size")
		.def<decltype(message->player_list(0)) (ScenePlayerList::*)(int) const>(&ScenePlayerList::player_list, "player_list")
		.def<decltype(message->mutable_player_list(0)) (ScenePlayerList::*)(int)>(&ScenePlayerList::mutable_player_list, "mutable_player_list")
		.def(&ScenePlayerList::add_player_list, "add_player_list")
		.def(&ScenePlayerList::ByteSize, "ByteSize")
		.def(&ScenePlayerList::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerSceneInfoReq_fflua_reg(lua_State* state)
{
//for decltype
	PlayerSceneInfoReq* message(nullptr);
	message;
	ff::fflua_register_t<PlayerSceneInfoReq, ctor()>(state, "PlayerSceneInfoReq", "google::protobuf::Message")
		// required fixed64 guid = 1;
		.def(&PlayerSceneInfoReq::guid, "guid")
		.def(&PlayerSceneInfoReq::set_guid, "set_guid")
		.def(&PlayerSceneInfoReq::ByteSize, "ByteSize")
		.def(&PlayerSceneInfoReq::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerSceneInfoReply_fflua_reg(lua_State* state)
{
//for decltype
	PlayerSceneInfoReply* message(nullptr);
	message;
	ff::fflua_register_t<PlayerSceneInfoReply, ctor()>(state, "PlayerSceneInfoReply", "google::protobuf::Message")
		// required .Packet.ScenePlayerInfo optinfo = 1;
		.def(&PlayerSceneInfoReply::optinfo, "optinfo")
		.def(&PlayerSceneInfoReply::mutable_optinfo, "mutable_optinfo")
		.def(&PlayerSceneInfoReply::ByteSize, "ByteSize")
		.def(&PlayerSceneInfoReply::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ScenePlayerExit_fflua_reg(lua_State* state)
{
//for decltype
	ScenePlayerExit* message(nullptr);
	message;
	ff::fflua_register_t<ScenePlayerExit, ctor()>(state, "ScenePlayerExit", "google::protobuf::Message")
		// required fixed64 guid = 1;
		.def(&ScenePlayerExit::guid, "guid")
		.def(&ScenePlayerExit::set_guid, "set_guid")
		// required int32 scene_id = 2;
		.def(&ScenePlayerExit::scene_id, "scene_id")
		.def(&ScenePlayerExit::set_scene_id, "set_scene_id")
		.def(&ScenePlayerExit::ByteSize, "ByteSize")
		.def(&ScenePlayerExit::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerLeaveZone_fflua_reg(lua_State* state)
{
//for decltype
	PlayerLeaveZone* message(nullptr);
	message;
	ff::fflua_register_t<PlayerLeaveZone, ctor()>(state, "PlayerLeaveZone", "google::protobuf::Message")
		// required fixed64 guid = 1;
		.def(&PlayerLeaveZone::guid, "guid")
		.def(&PlayerLeaveZone::set_guid, "set_guid")
		.def(&PlayerLeaveZone::ByteSize, "ByteSize")
		.def(&PlayerLeaveZone::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ClientMove_fflua_regist_all(lua_State* state)
{
	ClientClickMove_fflua_reg(state);
	ClientPlayerMove_fflua_reg(state);
	ClientPlayerStop_fflua_reg(state);
	TeamMemberInfo_fflua_reg(state);
	PlayerTeamData_fflua_reg(state);
	PlayerRunTimeStatus_fflua_reg(state);
	PlayerRunTimeStatusNotify_fflua_reg(state);
	ScenePlayerInfo_fflua_reg(state);
	ScenePlayerList_fflua_reg(state);
	PlayerSceneInfoReq_fflua_reg(state);
	PlayerSceneInfoReply_fflua_reg(state);
	ScenePlayerExit_fflua_reg(state);
	PlayerLeaveZone_fflua_reg(state);
	return true;
}
}
