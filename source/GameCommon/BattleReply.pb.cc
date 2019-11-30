// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: BattleReply.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "BattleReply.pb.h"

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

const ::google::protobuf::Descriptor* RawReply_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  RawReply_reflection_ = NULL;
const ::google::protobuf::Descriptor* BattleReply_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  BattleReply_reflection_ = NULL;
const ::google::protobuf::Descriptor* CG_GetBattleReply_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  CG_GetBattleReply_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_BattleReply_2eproto() {
  protobuf_AddDesc_BattleReply_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "BattleReply.proto");
  GOOGLE_CHECK(file != NULL);
  RawReply_descriptor_ = file->message_type(0);
  static const int RawReply_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RawReply, data_),
  };
  RawReply_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      RawReply_descriptor_,
      RawReply::default_instance_,
      RawReply_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RawReply, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(RawReply, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(RawReply));
  BattleReply_descriptor_ = file->message_type(1);
  static const int BattleReply_offsets_[8] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, guid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, enter_msg_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, messages_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, stat_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, create_time_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, scene_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, battle_group_id_),
  };
  BattleReply_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      BattleReply_descriptor_,
      BattleReply::default_instance_,
      BattleReply_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(BattleReply, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(BattleReply));
  CG_GetBattleReply_descriptor_ = file->message_type(2);
  static const int CG_GetBattleReply_offsets_[1] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CG_GetBattleReply, guid_),
  };
  CG_GetBattleReply_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      CG_GetBattleReply_descriptor_,
      CG_GetBattleReply::default_instance_,
      CG_GetBattleReply_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CG_GetBattleReply, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(CG_GetBattleReply, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(CG_GetBattleReply));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_BattleReply_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    RawReply_descriptor_, &RawReply::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    BattleReply_descriptor_, &BattleReply::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    CG_GetBattleReply_descriptor_, &CG_GetBattleReply::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_BattleReply_2eproto() {
  delete RawReply::default_instance_;
  delete RawReply_reflection_;
  delete BattleReply::default_instance_;
  delete BattleReply_reflection_;
  delete CG_GetBattleReply::default_instance_;
  delete CG_GetBattleReply_reflection_;
}

void protobuf_AddDesc_BattleReply_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::Packet::protobuf_AddDesc_ProtoBufOption_2eproto();
  ::Packet::protobuf_AddDesc_AllPacketEnum_2eproto();
  ::Packet::protobuf_AddDesc_BattleExpression_2eproto();
  ::Packet::protobuf_AddDesc_BattleGroundInfo_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\021BattleReply.proto\022\006Packet\032\024ProtoBufOpt"
    "ion.proto\032\023AllPacketEnum.proto\032\026BattleEx"
    "pression.proto\032\026BattleGroundInfo.proto\"\030"
    "\n\010RawReply\022\014\n\004data\030\001 \002(\014\"\232\002\n\013BattleReply"
    "\022\014\n\004guid\030\001 \002(\006\022;\n\tenter_msg\030\002 \002(\0132\036.Pack"
    "et.EnterBattleGroundReplyB\010\342\363\030\004blob\0222\n\010m"
    "essages\030\003 \002(\0132\020.Packet.RawReplyB\016\342\363\030\nmed"
    "iumblob\022&\n\004type\030\004 \002(\0162\030.Packet.BattleGro"
    "undType\022$\n\004stat\030\005 \002(\0132\014.Packet.StatB\010\342\363\030"
    "\004blob\022\023\n\013create_time\030\006 \002(\005\022\020\n\010scene_id\030\007"
    " \002(\005\022\027\n\017battle_group_id\030\010 \002(\005\"!\n\021CG_GetB"
    "attleReply\022\014\n\004guid\030\001 \002(\006", 464);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "BattleReply.proto", &protobuf_RegisterTypes);
  RawReply::default_instance_ = new RawReply();
  BattleReply::default_instance_ = new BattleReply();
  CG_GetBattleReply::default_instance_ = new CG_GetBattleReply();
  RawReply::default_instance_->InitAsDefaultInstance();
  BattleReply::default_instance_->InitAsDefaultInstance();
  CG_GetBattleReply::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_BattleReply_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_BattleReply_2eproto {
  StaticDescriptorInitializer_BattleReply_2eproto() {
    protobuf_AddDesc_BattleReply_2eproto();
  }
} static_descriptor_initializer_BattleReply_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int RawReply::kDataFieldNumber;
#endif  // !_MSC_VER

RawReply::RawReply()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void RawReply::InitAsDefaultInstance() {
}

RawReply::RawReply(const RawReply& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void RawReply::SharedCtor() {
  _cached_size_ = 0;
  data_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

RawReply::~RawReply() {
  SharedDtor();
}

void RawReply::SharedDtor() {
  if (data_ != &::google::protobuf::internal::kEmptyString) {
    delete data_;
  }
  if (this != default_instance_) {
  }
}

void RawReply::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* RawReply::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return RawReply_descriptor_;
}

const RawReply& RawReply::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_BattleReply_2eproto();
  return *default_instance_;
}

RawReply* RawReply::default_instance_ = NULL;

RawReply* RawReply::New() const {
  return new RawReply;
}

void RawReply::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_data()) {
      if (data_ != &::google::protobuf::internal::kEmptyString) {
        data_->clear();
      }
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
  SetDirty();
}

bool RawReply::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required bytes data = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_data()));
        } else {
          goto handle_uninterpreted;
        }
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

void RawReply::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required bytes data = 1;
  if (has_data()) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->data(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* RawReply::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required bytes data = 1;
  if (has_data()) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->data(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int RawReply::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required bytes data = 1;
    if (has_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::BytesSize(
          this->data());
    }

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

void RawReply::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const RawReply* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const RawReply*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
  SetDirty();
}

void RawReply::MergeFrom(const RawReply& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_data()) {
      set_data(from.data());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  SetDirty();
}

void RawReply::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

void RawReply::CopyFrom(const RawReply& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

bool RawReply::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void RawReply::SetInitialized() {
  _has_bits_[0] |= 0x00000001;

  return;
}

void RawReply::Swap(RawReply* other) {
  if (other != this) {
    std::swap(data_, other->data_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
    SetDirty(), other->SetDirty();
  }
}

::google::protobuf::Metadata RawReply::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = RawReply_descriptor_;
  metadata.reflection = RawReply_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int BattleReply::kGuidFieldNumber;
const int BattleReply::kEnterMsgFieldNumber;
const int BattleReply::kMessagesFieldNumber;
const int BattleReply::kTypeFieldNumber;
const int BattleReply::kStatFieldNumber;
const int BattleReply::kCreateTimeFieldNumber;
const int BattleReply::kSceneIdFieldNumber;
const int BattleReply::kBattleGroupIdFieldNumber;
#endif  // !_MSC_VER

BattleReply::BattleReply()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void BattleReply::InitAsDefaultInstance() {
  enter_msg_ = const_cast< ::Packet::EnterBattleGroundReply*>(&::Packet::EnterBattleGroundReply::default_instance());
  messages_ = const_cast< ::Packet::RawReply*>(&::Packet::RawReply::default_instance());
  stat_ = const_cast< ::Packet::Stat*>(&::Packet::Stat::default_instance());
}

BattleReply::BattleReply(const BattleReply& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void BattleReply::SharedCtor() {
  _cached_size_ = 0;
  guid_ = GOOGLE_ULONGLONG(0);
  enter_msg_ = NULL;
  messages_ = NULL;
  type_ = 0;
  stat_ = NULL;
  create_time_ = 0;
  scene_id_ = 0;
  battle_group_id_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

BattleReply::~BattleReply() {
  SharedDtor();
}

void BattleReply::SharedDtor() {
  if (this != default_instance_) {
    delete enter_msg_;
    delete messages_;
    delete stat_;
  }
}

void BattleReply::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* BattleReply::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return BattleReply_descriptor_;
}

const BattleReply& BattleReply::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_BattleReply_2eproto();
  return *default_instance_;
}

BattleReply* BattleReply::default_instance_ = NULL;

BattleReply* BattleReply::New() const {
  return new BattleReply;
}

void BattleReply::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    guid_ = GOOGLE_ULONGLONG(0);
    if (has_enter_msg()) {
      if (enter_msg_ != NULL) enter_msg_->::Packet::EnterBattleGroundReply::Clear();
    }
    if (has_messages()) {
      if (messages_ != NULL) messages_->::Packet::RawReply::Clear();
    }
    type_ = 0;
    if (has_stat()) {
      if (stat_ != NULL) stat_->::Packet::Stat::Clear();
    }
    create_time_ = 0;
    scene_id_ = 0;
    battle_group_id_ = 0;
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
  SetDirty();
}

bool BattleReply::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required fixed64 guid = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &guid_)));
          set_has_guid();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_enter_msg;
        break;
      }

      // required .Packet.EnterBattleGroundReply enter_msg = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_enter_msg:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_enter_msg()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_messages;
        break;
      }

      // required .Packet.RawReply messages = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_messages:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_messages()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(32)) goto parse_type;
        break;
      }

      // required .Packet.BattleGroundType type = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_type:
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::Packet::BattleGroundType_IsValid(value)) {
            set_type(static_cast< ::Packet::BattleGroundType >(value));
          } else {
            mutable_unknown_fields()->AddVarint(4, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_stat;
        break;
      }

      // required .Packet.Stat stat = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_stat:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_stat()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(48)) goto parse_create_time;
        break;
      }

      // required int32 create_time = 6;
      case 6: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_create_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &create_time_)));
          set_has_create_time();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(56)) goto parse_scene_id;
        break;
      }

      // required int32 scene_id = 7;
      case 7: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_scene_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &scene_id_)));
          set_has_scene_id();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(64)) goto parse_battle_group_id;
        break;
      }

      // required int32 battle_group_id = 8;
      case 8: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_battle_group_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &battle_group_id_)));
          set_has_battle_group_id();
        } else {
          goto handle_uninterpreted;
        }
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

void BattleReply::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required fixed64 guid = 1;
  if (has_guid()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(1, this->guid(), output);
  }

  // required .Packet.EnterBattleGroundReply enter_msg = 2;
  if (has_enter_msg()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->enter_msg(), output);
  }

  // required .Packet.RawReply messages = 3;
  if (has_messages()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->messages(), output);
  }

  // required .Packet.BattleGroundType type = 4;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      4, this->type(), output);
  }

  // required .Packet.Stat stat = 5;
  if (has_stat()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      5, this->stat(), output);
  }

  // required int32 create_time = 6;
  if (has_create_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(6, this->create_time(), output);
  }

  // required int32 scene_id = 7;
  if (has_scene_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(7, this->scene_id(), output);
  }

  // required int32 battle_group_id = 8;
  if (has_battle_group_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(8, this->battle_group_id(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* BattleReply::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required fixed64 guid = 1;
  if (has_guid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(1, this->guid(), target);
  }

  // required .Packet.EnterBattleGroundReply enter_msg = 2;
  if (has_enter_msg()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->enter_msg(), target);
  }

  // required .Packet.RawReply messages = 3;
  if (has_messages()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->messages(), target);
  }

  // required .Packet.BattleGroundType type = 4;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      4, this->type(), target);
  }

  // required .Packet.Stat stat = 5;
  if (has_stat()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        5, this->stat(), target);
  }

  // required int32 create_time = 6;
  if (has_create_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(6, this->create_time(), target);
  }

  // required int32 scene_id = 7;
  if (has_scene_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(7, this->scene_id(), target);
  }

  // required int32 battle_group_id = 8;
  if (has_battle_group_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(8, this->battle_group_id(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int BattleReply::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required fixed64 guid = 1;
    if (has_guid()) {
      total_size += 1 + 8;
    }

    // required .Packet.EnterBattleGroundReply enter_msg = 2;
    if (has_enter_msg()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->enter_msg());
    }

    // required .Packet.RawReply messages = 3;
    if (has_messages()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->messages());
    }

    // required .Packet.BattleGroundType type = 4;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }

    // required .Packet.Stat stat = 5;
    if (has_stat()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->stat());
    }

    // required int32 create_time = 6;
    if (has_create_time()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->create_time());
    }

    // required int32 scene_id = 7;
    if (has_scene_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->scene_id());
    }

    // required int32 battle_group_id = 8;
    if (has_battle_group_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->battle_group_id());
    }

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

void BattleReply::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const BattleReply* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const BattleReply*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
  SetDirty();
}

void BattleReply::MergeFrom(const BattleReply& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_guid()) {
      set_guid(from.guid());
    }
    if (from.has_enter_msg()) {
      mutable_enter_msg()->::Packet::EnterBattleGroundReply::MergeFrom(from.enter_msg());
    }
    if (from.has_messages()) {
      mutable_messages()->::Packet::RawReply::MergeFrom(from.messages());
    }
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_stat()) {
      mutable_stat()->::Packet::Stat::MergeFrom(from.stat());
    }
    if (from.has_create_time()) {
      set_create_time(from.create_time());
    }
    if (from.has_scene_id()) {
      set_scene_id(from.scene_id());
    }
    if (from.has_battle_group_id()) {
      set_battle_group_id(from.battle_group_id());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  SetDirty();
}

void BattleReply::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

void BattleReply::CopyFrom(const BattleReply& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

bool BattleReply::IsInitialized() const {
  if ((_has_bits_[0] & 0x000000ff) != 0x000000ff) return false;

  if (has_enter_msg()) {
    if (!this->enter_msg().IsInitialized()) return false;
  }
  if (has_messages()) {
    if (!this->messages().IsInitialized()) return false;
  }
  if (has_stat()) {
    if (!this->stat().IsInitialized()) return false;
  }
  return true;
}

void BattleReply::SetInitialized() {
  _has_bits_[0] |= 0x000000ff;

  if (has_enter_msg()) {
    this->mutable_enter_msg()->SetInitialized();
  }
  if (has_messages()) {
    this->mutable_messages()->SetInitialized();
  }
  if (has_stat()) {
    this->mutable_stat()->SetInitialized();
  }
  return;
}

void BattleReply::Swap(BattleReply* other) {
  if (other != this) {
    std::swap(guid_, other->guid_);
    std::swap(enter_msg_, other->enter_msg_);
    std::swap(messages_, other->messages_);
    std::swap(type_, other->type_);
    std::swap(stat_, other->stat_);
    std::swap(create_time_, other->create_time_);
    std::swap(scene_id_, other->scene_id_);
    std::swap(battle_group_id_, other->battle_group_id_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
    SetDirty(), other->SetDirty();
  }
}

::google::protobuf::Metadata BattleReply::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = BattleReply_descriptor_;
  metadata.reflection = BattleReply_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int CG_GetBattleReply::kGuidFieldNumber;
#endif  // !_MSC_VER

CG_GetBattleReply::CG_GetBattleReply()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void CG_GetBattleReply::InitAsDefaultInstance() {
}

CG_GetBattleReply::CG_GetBattleReply(const CG_GetBattleReply& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void CG_GetBattleReply::SharedCtor() {
  _cached_size_ = 0;
  guid_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

CG_GetBattleReply::~CG_GetBattleReply() {
  SharedDtor();
}

void CG_GetBattleReply::SharedDtor() {
  if (this != default_instance_) {
  }
}

void CG_GetBattleReply::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* CG_GetBattleReply::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return CG_GetBattleReply_descriptor_;
}

const CG_GetBattleReply& CG_GetBattleReply::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_BattleReply_2eproto();
  return *default_instance_;
}

CG_GetBattleReply* CG_GetBattleReply::default_instance_ = NULL;

CG_GetBattleReply* CG_GetBattleReply::New() const {
  return new CG_GetBattleReply;
}

void CG_GetBattleReply::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    guid_ = GOOGLE_ULONGLONG(0);
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
  SetDirty();
}

bool CG_GetBattleReply::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required fixed64 guid = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &guid_)));
          set_has_guid();
        } else {
          goto handle_uninterpreted;
        }
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

void CG_GetBattleReply::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required fixed64 guid = 1;
  if (has_guid()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(1, this->guid(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* CG_GetBattleReply::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required fixed64 guid = 1;
  if (has_guid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(1, this->guid(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int CG_GetBattleReply::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required fixed64 guid = 1;
    if (has_guid()) {
      total_size += 1 + 8;
    }

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

void CG_GetBattleReply::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const CG_GetBattleReply* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const CG_GetBattleReply*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
  SetDirty();
}

void CG_GetBattleReply::MergeFrom(const CG_GetBattleReply& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_guid()) {
      set_guid(from.guid());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  SetDirty();
}

void CG_GetBattleReply::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

void CG_GetBattleReply::CopyFrom(const CG_GetBattleReply& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

bool CG_GetBattleReply::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void CG_GetBattleReply::SetInitialized() {
  _has_bits_[0] |= 0x00000001;

  return;
}

void CG_GetBattleReply::Swap(CG_GetBattleReply* other) {
  if (other != this) {
    std::swap(guid_, other->guid_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
    SetDirty(), other->SetDirty();
  }
}

::google::protobuf::Metadata CG_GetBattleReply::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = CG_GetBattleReply_descriptor_;
  metadata.reflection = CG_GetBattleReply_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Packet

// @@protoc_insertion_point(global_scope)
