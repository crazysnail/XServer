// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ProtoBufOption.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "ProtoBufOption.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace Packet {

namespace {


}  // namespace


void protobuf_AssignDesc_ProtoBufOption_2eproto() {
  protobuf_AddDesc_ProtoBufOption_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "ProtoBufOption.proto");
  GOOGLE_CHECK(file != NULL);
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_ProtoBufOption_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
}

}  // namespace

void protobuf_ShutdownFile_ProtoBufOption_2eproto() {
}

void protobuf_AddDesc_ProtoBufOption_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::protobuf_AddDesc_google_2fprotobuf_2fdescriptor_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\024ProtoBufOption.proto\022\006Packet\032 google/p"
    "rotobuf/descriptor.proto:5\n\014column_count"
    "\022\035.google.protobuf.FieldOptions\030\270\216\003 \001(\005:"
    "8\n\017sql_primary_key\022\035.google.protobuf.Fie"
    "ldOptions\030\271\216\003 \001(\010:@\n\027sql_non_unique_key_"
    "name\022\035.google.protobuf.FieldOptions\030\272\216\003 "
    "\001(\t:<\n\023sql_unique_key_name\022\035.google.prot"
    "obuf.FieldOptions\030\273\216\003 \001(\t:1\n\010sql_type\022\035."
    "google.protobuf.FieldOptions\030\274\216\003 \001(\t", 356);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "ProtoBufOption.proto", &protobuf_RegisterTypes);
  ::google::protobuf::internal::ExtensionSet::RegisterExtension(
    &::google::protobuf::FieldOptions::default_instance(),
    51000, 5, false, false);
  ::google::protobuf::internal::ExtensionSet::RegisterExtension(
    &::google::protobuf::FieldOptions::default_instance(),
    51001, 8, false, false);
  ::google::protobuf::internal::ExtensionSet::RegisterExtension(
    &::google::protobuf::FieldOptions::default_instance(),
    51002, 9, false, false);
  ::google::protobuf::internal::ExtensionSet::RegisterExtension(
    &::google::protobuf::FieldOptions::default_instance(),
    51003, 9, false, false);
  ::google::protobuf::internal::ExtensionSet::RegisterExtension(
    &::google::protobuf::FieldOptions::default_instance(),
    51004, 9, false, false);
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_ProtoBufOption_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_ProtoBufOption_2eproto {
  StaticDescriptorInitializer_ProtoBufOption_2eproto() {
    protobuf_AddDesc_ProtoBufOption_2eproto();
  }
} static_descriptor_initializer_ProtoBufOption_2eproto_;
::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< ::google::protobuf::int32 >, 5, false >
  column_count(kColumnCountFieldNumber, 0);
::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< bool >, 8, false >
  sql_primary_key(kSqlPrimaryKeyFieldNumber, false);
const ::std::string sql_non_unique_key_name_default("");
::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::StringTypeTraits, 9, false >
  sql_non_unique_key_name(kSqlNonUniqueKeyNameFieldNumber, sql_non_unique_key_name_default);
const ::std::string sql_unique_key_name_default("");
::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::StringTypeTraits, 9, false >
  sql_unique_key_name(kSqlUniqueKeyNameFieldNumber, sql_unique_key_name_default);
const ::std::string sql_type_default("");
::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::StringTypeTraits, 9, false >
  sql_type(kSqlTypeFieldNumber, sql_type_default);

// @@protoc_insertion_point(namespace_scope)

}  // namespace Packet

// @@protoc_insertion_point(global_scope)
