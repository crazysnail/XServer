// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ArenaDb.proto
#include "ArenaDb.pb.h"
#include "ArenaDb.proto.fflua.h"
namespace DB {
bool ArenaUser_fflua_reg(lua_State* state)
{
//for decltype
	ArenaUser* message(nullptr);
	message;
	ff::fflua_register_t<ArenaUser, ctor()>(state, "ArenaUser", "google::protobuf::Message")
		// required fixed64 player_guid = 1;
		.def(&ArenaUser::player_guid, "player_guid")
		.def(&ArenaUser::set_player_guid, "set_player_guid")
		// required int32 rank = 2;
		.def(&ArenaUser::rank, "rank")
		.def(&ArenaUser::set_rank, "set_rank")
		// required int32 yesterday_rank = 3;
		.def(&ArenaUser::yesterday_rank, "yesterday_rank")
		.def(&ArenaUser::set_yesterday_rank, "set_yesterday_rank")
		// required bool is_robot = 4;
		.def(&ArenaUser::is_robot, "is_robot")
		.def(&ArenaUser::set_is_robot, "set_is_robot")
		// required int32 robot_id = 5;
		.def(&ArenaUser::robot_id, "robot_id")
		.def(&ArenaUser::set_robot_id, "set_robot_id")
		// required int32 score = 6;
		.def(&ArenaUser::score, "score")
		.def(&ArenaUser::set_score, "set_score")
		// required int64 updated_at = 8;
		.def(&ArenaUser::updated_at, "updated_at")
		.def(&ArenaUser::set_updated_at, "set_updated_at")
		// required .DB.RepeatedArenaLog log = 10;
		.def(&ArenaUser::log, "log")
		.def(&ArenaUser::mutable_log, "mutable_log")
		// required fixed64 last_reward_time = 11;
		.def(&ArenaUser::last_reward_time, "last_reward_time")
		.def(&ArenaUser::set_last_reward_time, "set_last_reward_time")
		// required .DB.PlayerTrialCopy trial_copy = 12;
		.def(&ArenaUser::trial_copy, "trial_copy")
		.def(&ArenaUser::mutable_trial_copy, "mutable_trial_copy")
		.def(&ArenaUser::ByteSize, "ByteSize")
		.def(&ArenaUser::SetInitialized, "SetInitialized")
		;
	return true;
}
bool RepeatedArenaLog_fflua_reg(lua_State* state)
{
//for decltype
	RepeatedArenaLog* message(nullptr);
	message;
	ff::fflua_register_t<RepeatedArenaLog, ctor()>(state, "RepeatedArenaLog", "google::protobuf::Message")
		// repeated .Packet.ArenaLog logs = 1;
		.def(&RepeatedArenaLog::logs_size, "logs_size")
		.def<decltype(message->logs(0)) (RepeatedArenaLog::*)(int) const>(&RepeatedArenaLog::logs, "logs")
		.def<decltype(message->mutable_logs(0)) (RepeatedArenaLog::*)(int)>(&RepeatedArenaLog::mutable_logs, "mutable_logs")
		.def(&RepeatedArenaLog::add_logs, "add_logs")
		.def(&RepeatedArenaLog::ByteSize, "ByteSize")
		.def(&RepeatedArenaLog::SetInitialized, "SetInitialized")
		;
	return true;
}
bool RobotInfo_fflua_reg(lua_State* state)
{
//for decltype
	RobotInfo* message(nullptr);
	message;
	ff::fflua_register_t<RobotInfo, ctor()>(state, "RobotInfo", "google::protobuf::Message")
		// optional .Packet.PlayerBasicInfo player = 1;
		.def(&RobotInfo::player, "player")
		.def(&RobotInfo::mutable_player, "mutable_player")
		.def(&RobotInfo::has_player, "has_player")
		// repeated .Packet.ActorFullInfo actors = 2;
		.def(&RobotInfo::actors_size, "actors_size")
		.def<decltype(message->actors(0)) (RobotInfo::*)(int) const>(&RobotInfo::actors, "actors")
		.def<decltype(message->mutable_actors(0)) (RobotInfo::*)(int)>(&RobotInfo::mutable_actors, "mutable_actors")
		.def(&RobotInfo::add_actors, "add_actors")
		.def(&RobotInfo::ByteSize, "ByteSize")
		.def(&RobotInfo::SetInitialized, "SetInitialized")
		;
	return true;
}
bool PlayerTrialCopy_fflua_reg(lua_State* state)
{
//for decltype
	PlayerTrialCopy* message(nullptr);
	message;
	ff::fflua_register_t<PlayerTrialCopy, ctor()>(state, "PlayerTrialCopy", "google::protobuf::Message")
		// optional int32 init_actor_id = 1;
		.def(&PlayerTrialCopy::init_actor_id, "init_actor_id")
		.def(&PlayerTrialCopy::set_init_actor_id, "set_init_actor_id")
		.def(&PlayerTrialCopy::has_init_actor_id, "has_init_actor_id")
		// repeated .Packet.ActorFullInfo actors = 2;
		.def(&PlayerTrialCopy::actors_size, "actors_size")
		.def<decltype(message->actors(0)) (PlayerTrialCopy::*)(int) const>(&PlayerTrialCopy::actors, "actors")
		.def<decltype(message->mutable_actors(0)) (PlayerTrialCopy::*)(int)>(&PlayerTrialCopy::mutable_actors, "mutable_actors")
		.def(&PlayerTrialCopy::add_actors, "add_actors")
		.def(&PlayerTrialCopy::ByteSize, "ByteSize")
		.def(&PlayerTrialCopy::SetInitialized, "SetInitialized")
		;
	return true;
}
bool ArenaDb_fflua_regist_all(lua_State* state)
{
	ArenaUser_fflua_reg(state);
	RepeatedArenaLog_fflua_reg(state);
	RobotInfo_fflua_reg(state);
	PlayerTrialCopy_fflua_reg(state);
	return true;
}
}
