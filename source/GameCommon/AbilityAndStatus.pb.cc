// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: AbilityAndStatus.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "AbilityAndStatus.pb.h"

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

namespace Config {

namespace {

const ::google::protobuf::Descriptor* AbilityAndStatusConfig_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AbilityAndStatusConfig_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_AbilityAndStatus_2eproto() {
  protobuf_AddDesc_AbilityAndStatus_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "AbilityAndStatus.proto");
  GOOGLE_CHECK(file != NULL);
  AbilityAndStatusConfig_descriptor_ = file->message_type(0);
  static const int AbilityAndStatusConfig_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AbilityAndStatusConfig, status_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AbilityAndStatusConfig, can_do_),
  };
  AbilityAndStatusConfig_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AbilityAndStatusConfig_descriptor_,
      AbilityAndStatusConfig::default_instance_,
      AbilityAndStatusConfig_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AbilityAndStatusConfig, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AbilityAndStatusConfig, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AbilityAndStatusConfig));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_AbilityAndStatus_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AbilityAndStatusConfig_descriptor_, &AbilityAndStatusConfig::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_AbilityAndStatus_2eproto() {
  delete AbilityAndStatusConfig::default_instance_;
  delete AbilityAndStatusConfig_reflection_;
}

void protobuf_AddDesc_AbilityAndStatus_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::Packet::protobuf_AddDesc_ProtoBufOption_2eproto();
  ::Config::protobuf_AddDesc_AllConfigEnum_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\026AbilityAndStatus.proto\022\006Config\032\024ProtoB"
    "ufOption.proto\032\023AllConfigEnum.proto\"Z\n\026A"
    "bilityAndStatusConfig\022*\n\006status\030\001 \002(\0162\032."
    "Config.BattleObjectStatus\022\024\n\006can_do\030\002 \003("
    "\010B\004\300\363\030\t", 167);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "AbilityAndStatus.proto", &protobuf_RegisterTypes);
  AbilityAndStatusConfig::default_instance_ = new AbilityAndStatusConfig();
  AbilityAndStatusConfig::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_AbilityAndStatus_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_AbilityAndStatus_2eproto {
  StaticDescriptorInitializer_AbilityAndStatus_2eproto() {
    protobuf_AddDesc_AbilityAndStatus_2eproto();
  }
} static_descriptor_initializer_AbilityAndStatus_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int AbilityAndStatusConfig::kStatusFieldNumber;
const int AbilityAndStatusConfig::kCanDoFieldNumber;
#endif  // !_MSC_VER

AbilityAndStatusConfig::AbilityAndStatusConfig()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void AbilityAndStatusConfig::InitAsDefaultInstance() {
}

AbilityAndStatusConfig::AbilityAndStatusConfig(const AbilityAndStatusConfig& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void AbilityAndStatusConfig::SharedCtor() {
  _cached_size_ = 0;
  status_ = 1;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AbilityAndStatusConfig::~AbilityAndStatusConfig() {
  SharedDtor();
}

void AbilityAndStatusConfig::SharedDtor() {
  if (this != default_instance_) {
  }
}

void AbilityAndStatusConfig::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AbilityAndStatusConfig::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AbilityAndStatusConfig_descriptor_;
}

const AbilityAndStatusConfig& AbilityAndStatusConfig::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_AbilityAndStatus_2eproto();
  return *default_instance_;
}

AbilityAndStatusConfig* AbilityAndStatusConfig::default_instance_ = NULL;

AbilityAndStatusConfig* AbilityAndStatusConfig::New() const {
  return new AbilityAndStatusConfig;
}

void AbilityAndStatusConfig::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    status_ = 1;
  }
  can_do_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
  SetDirty();
}

bool AbilityAndStatusConfig::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .Config.BattleObjectStatus status = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::Config::BattleObjectStatus_IsValid(value)) {
            set_status(static_cast< ::Config::BattleObjectStatus >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_can_do;
        break;
      }

      // repeated bool can_do = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_can_do:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 1, 16, input, this->mutable_can_do())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   bool, ::google::protobuf::internal::WireFormatLite::TYPE_BOOL>(
                 input, this->mutable_can_do())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_can_do;
        if (input->ExpectAtEnd()) return true;
        break;
      }

      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  SetDirty();
  return true;
#undef DO_
}

void AbilityAndStatusConfig::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .Config.BattleObjectStatus status = 1;
  if (has_status()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->status(), output);
  }

  // repeated bool can_do = 2;
  for (int i = 0; i < this->can_do_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteBool(
      2, this->can_do(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* AbilityAndStatusConfig::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .Config.BattleObjectStatus status = 1;
  if (has_status()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->status(), target);
  }

  // repeated bool can_do = 2;
  for (int i = 0; i < this->can_do_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteBoolToArray(2, this->can_do(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int AbilityAndStatusConfig::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .Config.BattleObjectStatus status = 1;
    if (has_status()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->status());
    }

  }
  // repeated bool can_do = 2;
  {
    int data_size = 0;
    data_size = 1 * this->can_do_size();
    total_size += 1 * this->can_do_size() + data_size;
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void AbilityAndStatusConfig::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const AbilityAndStatusConfig* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const AbilityAndStatusConfig*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
  SetDirty();
}

void AbilityAndStatusConfig::MergeFrom(const AbilityAndStatusConfig& from) {
  GOOGLE_CHECK_NE(&from, this);
  can_do_.MergeFrom(from.can_do_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_status()) {
      set_status(from.status());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  SetDirty();
}

void AbilityAndStatusConfig::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

void AbilityAndStatusConfig::CopyFrom(const AbilityAndStatusConfig& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

bool AbilityAndStatusConfig::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void AbilityAndStatusConfig::SetInitialized() {
  _has_bits_[0] |= 0x00000001;

  return;
}

void AbilityAndStatusConfig::Swap(AbilityAndStatusConfig* other) {
  if (other != this) {
    std::swap(status_, other->status_);
    can_do_.Swap(&other->can_do_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
    SetDirty(), other->SetDirty();
  }
}

::google::protobuf::Metadata AbilityAndStatusConfig::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AbilityAndStatusConfig_descriptor_;
  metadata.reflection = AbilityAndStatusConfig_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Config

// @@protoc_insertion_point(global_scope)
