// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Team.proto

#ifndef PROTOBUF_Team_2eproto_2efflua_2eh__INCLUDED
#define PROTOBUF_Team_2eproto_2efflua_2eh__INCLUDED

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2005000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2005000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include<lua/fflua_register.h>

#include "AllPacketEnum.proto.fflua.h"
#include "PlayerBasicInfo.proto.fflua.h"
#include "ItemAndEquip.proto.fflua.h"
#include "ProtoBufOption.proto.fflua.h"
#include "Team.pb.h"
namespace ff{
//PurposeInfo
//TeamPlayerBasicInfo
//TeamPlayerInfo
//SetTeamHeroPositionReq
//SetTeamHeroPositionReply
//TeamInfo
//CreateTeam
//CreateRobotTeam
//DelTeam
//ChangeTeamTypeReq
//UpdateTeamTypeReply
//ChangeTeamArrangeTypeReq
//UpdateTeamArrangeTypeReply
//ApplyTeamReq
//ApplyMissionTeamReq
//LevelTeamReq
//AskLevelTeamReq
//AgreeApplyTeamReq
//InviteyAddTeamReq
//AskInviteyAgreeTeamReply
//InviteyAgreeTeamReq
//AddTeamReply
//LeaveTeamReply
//SetPurposeInfoReq
//PurposeInfoUpdate
//SetTeamAutoMatchReq
//TeamAutoMatchUpdate
//TeamFullPlayerInfo
//TeamListInfo
//TeamInfoReq
//EmptyTeamReply
//TeamInfoReply
//FindTeamListReq
//TeamListReply
//TeamApplyListReq
//TeamApplyListReply
//PlayerAutoMatchInfo
//PlayerAutoMatchReq
//PlayerAutoMatchReply
//PlayerSetTeamStatusReq
//TeamPlayerBasicUpdate
//SetTeamLeaderReq
//ApplyTeamLeaderReq
//AskApplyTeamLeaderReq
//AskApplyTeamLeaderReply
//TeamLeaderUpdate
//RobotPlayerInOk
//TeamAutomatchConfig
}
namespace Packet {
bool PurposeInfo_fflua_reg(lua_State* state);
bool TeamPlayerBasicInfo_fflua_reg(lua_State* state);
bool TeamPlayerInfo_fflua_reg(lua_State* state);
bool SetTeamHeroPositionReq_fflua_reg(lua_State* state);
bool SetTeamHeroPositionReply_fflua_reg(lua_State* state);
bool TeamInfo_fflua_reg(lua_State* state);
bool CreateTeam_fflua_reg(lua_State* state);
bool CreateRobotTeam_fflua_reg(lua_State* state);
bool DelTeam_fflua_reg(lua_State* state);
bool ChangeTeamTypeReq_fflua_reg(lua_State* state);
bool UpdateTeamTypeReply_fflua_reg(lua_State* state);
bool ChangeTeamArrangeTypeReq_fflua_reg(lua_State* state);
bool UpdateTeamArrangeTypeReply_fflua_reg(lua_State* state);
bool ApplyTeamReq_fflua_reg(lua_State* state);
bool ApplyMissionTeamReq_fflua_reg(lua_State* state);
bool LevelTeamReq_fflua_reg(lua_State* state);
bool AskLevelTeamReq_fflua_reg(lua_State* state);
bool AgreeApplyTeamReq_fflua_reg(lua_State* state);
bool InviteyAddTeamReq_fflua_reg(lua_State* state);
bool AskInviteyAgreeTeamReply_fflua_reg(lua_State* state);
bool InviteyAgreeTeamReq_fflua_reg(lua_State* state);
bool AddTeamReply_fflua_reg(lua_State* state);
bool LeaveTeamReply_fflua_reg(lua_State* state);
bool SetPurposeInfoReq_fflua_reg(lua_State* state);
bool PurposeInfoUpdate_fflua_reg(lua_State* state);
bool SetTeamAutoMatchReq_fflua_reg(lua_State* state);
bool TeamAutoMatchUpdate_fflua_reg(lua_State* state);
bool TeamFullPlayerInfo_fflua_reg(lua_State* state);
bool TeamListInfo_fflua_reg(lua_State* state);
bool TeamInfoReq_fflua_reg(lua_State* state);
bool EmptyTeamReply_fflua_reg(lua_State* state);
bool TeamInfoReply_fflua_reg(lua_State* state);
bool FindTeamListReq_fflua_reg(lua_State* state);
bool TeamListReply_fflua_reg(lua_State* state);
bool TeamApplyListReq_fflua_reg(lua_State* state);
bool TeamApplyListReply_fflua_reg(lua_State* state);
bool PlayerAutoMatchInfo_fflua_reg(lua_State* state);
bool PlayerAutoMatchReq_fflua_reg(lua_State* state);
bool PlayerAutoMatchReply_fflua_reg(lua_State* state);
bool PlayerSetTeamStatusReq_fflua_reg(lua_State* state);
bool TeamPlayerBasicUpdate_fflua_reg(lua_State* state);
bool SetTeamLeaderReq_fflua_reg(lua_State* state);
bool ApplyTeamLeaderReq_fflua_reg(lua_State* state);
bool AskApplyTeamLeaderReq_fflua_reg(lua_State* state);
bool AskApplyTeamLeaderReply_fflua_reg(lua_State* state);
bool TeamLeaderUpdate_fflua_reg(lua_State* state);
bool RobotPlayerInOk_fflua_reg(lua_State* state);
bool TeamAutomatchConfig_fflua_reg(lua_State* state);
bool Team_fflua_regist_all(lua_State* state);
}
#endif //PROTOBUF_Team_2eproto_2efflua_2eh__INCLUDED
