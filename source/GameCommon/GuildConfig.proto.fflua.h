// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: GuildConfig.proto

#ifndef PROTOBUF_GuildConfig_2eproto_2efflua_2eh__INCLUDED
#define PROTOBUF_GuildConfig_2eproto_2efflua_2eh__INCLUDED

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

#include "ProtoBufOption.proto.fflua.h"
#include "GuildConfig.pb.h"
namespace ff{
//GuildBuildLevelConfig
//GuildLevelFixConfig
//GuildPracticeConfig
//GuildPracticeLevelConfig
//GuildCDKConfig
//GuildWetCopyConfig
//GuildWetCopyStageConfig
//GuildPositionConfig
//GuildDonateConfig
}
namespace Config {
bool GuildBuildLevelConfig_fflua_reg(lua_State* state);
bool GuildLevelFixConfig_fflua_reg(lua_State* state);
bool GuildPracticeConfig_fflua_reg(lua_State* state);
bool GuildPracticeLevelConfig_fflua_reg(lua_State* state);
bool GuildCDKConfig_fflua_reg(lua_State* state);
bool GuildWetCopyConfig_fflua_reg(lua_State* state);
bool GuildWetCopyStageConfig_fflua_reg(lua_State* state);
bool GuildPositionConfig_fflua_reg(lua_State* state);
bool GuildDonateConfig_fflua_reg(lua_State* state);
bool GuildConfig_fflua_regist_all(lua_State* state);
}
#endif //PROTOBUF_GuildConfig_2eproto_2efflua_2eh__INCLUDED
