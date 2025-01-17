// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: WorldBoss.proto

#ifndef PROTOBUF_WorldBoss_2eproto_2efflua_2eh__INCLUDED
#define PROTOBUF_WorldBoss_2eproto_2efflua_2eh__INCLUDED

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

#include "PlayerBasicInfo.proto.fflua.h"
#include "ItemAndEquip.proto.fflua.h"
#include "BattleExpression.proto.fflua.h"
#include "ProtoBufOption.proto.fflua.h"
#include "WorldBoss.pb.h"
namespace ff{
//WorldBossInfo
//WorldBossInfoReq
//WorldBossInfoReply
//WorldBossHpReq
//WorldBossHpUpdate
//WorldBossReliveCDUpdate
//WorldBossConfig
//WorldBossRelivenow
//WorldBossGoldInspire
//WorldBossDiamondInspire
//WorldBossStatisticsInfo
//WorldBossStatisticsReq
//WorldBossStatisticsReply
//SetWorldBossAutoFightReq
//LeaveWorldBossReq
}
namespace Packet {
bool WorldBossInfo_fflua_reg(lua_State* state);
bool WorldBossInfoReq_fflua_reg(lua_State* state);
bool WorldBossInfoReply_fflua_reg(lua_State* state);
bool WorldBossHpReq_fflua_reg(lua_State* state);
bool WorldBossHpUpdate_fflua_reg(lua_State* state);
bool WorldBossReliveCDUpdate_fflua_reg(lua_State* state);
bool WorldBossConfig_fflua_reg(lua_State* state);
bool WorldBossRelivenow_fflua_reg(lua_State* state);
bool WorldBossGoldInspire_fflua_reg(lua_State* state);
bool WorldBossDiamondInspire_fflua_reg(lua_State* state);
bool WorldBossStatisticsInfo_fflua_reg(lua_State* state);
bool WorldBossStatisticsReq_fflua_reg(lua_State* state);
bool WorldBossStatisticsReply_fflua_reg(lua_State* state);
bool SetWorldBossAutoFightReq_fflua_reg(lua_State* state);
bool LeaveWorldBossReq_fflua_reg(lua_State* state);
bool WorldBoss_fflua_regist_all(lua_State* state);
}
#endif //PROTOBUF_WorldBoss_2eproto_2efflua_2eh__INCLUDED
