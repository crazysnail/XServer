// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BattleField.proto

#ifndef PROTOBUF_BattleField_2eproto_2efflua_2eh__INCLUDED
#define PROTOBUF_BattleField_2eproto_2efflua_2eh__INCLUDED

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
#include "AllConfigEnum.proto.fflua.h"
#include "Base.proto.fflua.h"
#include "ProtoBufOption.proto.fflua.h"
#include "BattleField.pb.h"
namespace ff{
//BFConfig
//BattleFeildProxy
//BattleRoomInfo
//BattleFieldPlayerInfo
//BattleFieldInfo
//JoinBattleRoom
//LeaveBattleRoom
//LeaveBattleField
//AgreeEnterBattleField
//DisAgreeEnterBattleField
//EnterRoomNotify
//EnterBattleFieldNotify
//BeginBattleFieldNotify
//BeginBattleField
//BattleFieldFightTarget
//FlagOprate
//BfResult
}
namespace Packet {
bool BFConfig_fflua_reg(lua_State* state);
bool BattleFeildProxy_fflua_reg(lua_State* state);
bool BattleRoomInfo_fflua_reg(lua_State* state);
bool BattleFieldPlayerInfo_fflua_reg(lua_State* state);
bool BattleFieldInfo_fflua_reg(lua_State* state);
bool JoinBattleRoom_fflua_reg(lua_State* state);
bool LeaveBattleRoom_fflua_reg(lua_State* state);
bool LeaveBattleField_fflua_reg(lua_State* state);
bool AgreeEnterBattleField_fflua_reg(lua_State* state);
bool DisAgreeEnterBattleField_fflua_reg(lua_State* state);
bool EnterRoomNotify_fflua_reg(lua_State* state);
bool EnterBattleFieldNotify_fflua_reg(lua_State* state);
bool BeginBattleFieldNotify_fflua_reg(lua_State* state);
bool BeginBattleField_fflua_reg(lua_State* state);
bool BattleFieldFightTarget_fflua_reg(lua_State* state);
bool FlagOprate_fflua_reg(lua_State* state);
bool BfResult_fflua_reg(lua_State* state);
bool BattleField_fflua_regist_all(lua_State* state);
}
#endif //PROTOBUF_BattleField_2eproto_2efflua_2eh__INCLUDED
