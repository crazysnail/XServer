// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: PackageConfig.proto

#ifndef PROTOBUF_PackageConfig_2eproto_2efflua_2eh__INCLUDED
#define PROTOBUF_PackageConfig_2eproto_2efflua_2eh__INCLUDED

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

#include "AllConfigEnum.proto.fflua.h"
#include "AllPacketEnum.proto.fflua.h"
#include "ActorBasicInfo.proto.fflua.h"
#include "ProtoBufOption.proto.fflua.h"
#include "PackageConfig.pb.h"
namespace ff{
//ItemPackageElement
//ItemDropGroup
//ItemPackageConfig
}
namespace Config {
bool ItemPackageElement_fflua_reg(lua_State* state);
bool ItemDropGroup_fflua_reg(lua_State* state);
bool ItemPackageConfig_fflua_reg(lua_State* state);
bool PackageConfig_fflua_regist_all(lua_State* state);
}
#endif //PROTOBUF_PackageConfig_2eproto_2efflua_2eh__INCLUDED
