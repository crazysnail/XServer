// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: LootDb.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "LootDb.pb.h"

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

namespace Db {

namespace {

const ::google::protobuf::Descriptor* LootPlayer_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  LootPlayer_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_LootDb_2eproto() {
  protobuf_AddDesc_LootDb_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "LootDb.proto");
  GOOGLE_CHECK(file != NULL);
  LootPlayer_descriptor_ = file->message_type(0);
  static const int LootPlayer_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, last_loot_player_guid_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, unbind_coin_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, be_loot_count_today_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, last_be_loot_time_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, last_loot_time_),
  };
  LootPlayer_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      LootPlayer_descriptor_,
      LootPlayer::default_instance_,
      LootPlayer_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(LootPlayer, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(LootPlayer));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_LootDb_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    LootPlayer_descriptor_, &LootPlayer::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_LootDb_2eproto() {
  delete LootPlayer::default_instance_;
  delete LootPlayer_reflection_;
}

void protobuf_AddDesc_LootDb_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\014LootDb.proto\022\002Db\"\220\001\n\nLootPlayer\022\035\n\025las"
    "t_loot_player_guid\030\001 \002(\006\022\023\n\013unbind_coin\030"
    "\002 \002(\005\022\033\n\023be_loot_count_today\030\003 \002(\005\022\031\n\021la"
    "st_be_loot_time\030\004 \002(\006\022\026\n\016last_loot_time\030"
    "\005 \002(\006", 165);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "LootDb.proto", &protobuf_RegisterTypes);
  LootPlayer::default_instance_ = new LootPlayer();
  LootPlayer::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_LootDb_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_LootDb_2eproto {
  StaticDescriptorInitializer_LootDb_2eproto() {
    protobuf_AddDesc_LootDb_2eproto();
  }
} static_descriptor_initializer_LootDb_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int LootPlayer::kLastLootPlayerGuidFieldNumber;
const int LootPlayer::kUnbindCoinFieldNumber;
const int LootPlayer::kBeLootCountTodayFieldNumber;
const int LootPlayer::kLastBeLootTimeFieldNumber;
const int LootPlayer::kLastLootTimeFieldNumber;
#endif  // !_MSC_VER

LootPlayer::LootPlayer()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void LootPlayer::InitAsDefaultInstance() {
}

LootPlayer::LootPlayer(const LootPlayer& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void LootPlayer::SharedCtor() {
  _cached_size_ = 0;
  last_loot_player_guid_ = GOOGLE_ULONGLONG(0);
  unbind_coin_ = 0;
  be_loot_count_today_ = 0;
  last_be_loot_time_ = GOOGLE_ULONGLONG(0);
  last_loot_time_ = GOOGLE_ULONGLONG(0);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

LootPlayer::~LootPlayer() {
  SharedDtor();
}

void LootPlayer::SharedDtor() {
  if (this != default_instance_) {
  }
}

void LootPlayer::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* LootPlayer::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return LootPlayer_descriptor_;
}

const LootPlayer& LootPlayer::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_LootDb_2eproto();
  return *default_instance_;
}

LootPlayer* LootPlayer::default_instance_ = NULL;

LootPlayer* LootPlayer::New() const {
  return new LootPlayer;
}

void LootPlayer::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    last_loot_player_guid_ = GOOGLE_ULONGLONG(0);
    unbind_coin_ = 0;
    be_loot_count_today_ = 0;
    last_be_loot_time_ = GOOGLE_ULONGLONG(0);
    last_loot_time_ = GOOGLE_ULONGLONG(0);
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
  SetDirty();
}

bool LootPlayer::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required fixed64 last_loot_player_guid = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &last_loot_player_guid_)));
          set_has_last_loot_player_guid();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_unbind_coin;
        break;
      }

      // required int32 unbind_coin = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_unbind_coin:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &unbind_coin_)));
          set_has_unbind_coin();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(24)) goto parse_be_loot_count_today;
        break;
      }

      // required int32 be_loot_count_today = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_be_loot_count_today:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &be_loot_count_today_)));
          set_has_be_loot_count_today();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(33)) goto parse_last_be_loot_time;
        break;
      }

      // required fixed64 last_be_loot_time = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_last_be_loot_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &last_be_loot_time_)));
          set_has_last_be_loot_time();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(41)) goto parse_last_loot_time;
        break;
      }

      // required fixed64 last_loot_time = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_last_loot_time:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, &last_loot_time_)));
          set_has_last_loot_time();
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

void LootPlayer::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required fixed64 last_loot_player_guid = 1;
  if (has_last_loot_player_guid()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(1, this->last_loot_player_guid(), output);
  }

  // required int32 unbind_coin = 2;
  if (has_unbind_coin()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->unbind_coin(), output);
  }

  // required int32 be_loot_count_today = 3;
  if (has_be_loot_count_today()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->be_loot_count_today(), output);
  }

  // required fixed64 last_be_loot_time = 4;
  if (has_last_be_loot_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(4, this->last_be_loot_time(), output);
  }

  // required fixed64 last_loot_time = 5;
  if (has_last_loot_time()) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(5, this->last_loot_time(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* LootPlayer::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required fixed64 last_loot_player_guid = 1;
  if (has_last_loot_player_guid()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(1, this->last_loot_player_guid(), target);
  }

  // required int32 unbind_coin = 2;
  if (has_unbind_coin()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->unbind_coin(), target);
  }

  // required int32 be_loot_count_today = 3;
  if (has_be_loot_count_today()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->be_loot_count_today(), target);
  }

  // required fixed64 last_be_loot_time = 4;
  if (has_last_be_loot_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(4, this->last_be_loot_time(), target);
  }

  // required fixed64 last_loot_time = 5;
  if (has_last_loot_time()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteFixed64ToArray(5, this->last_loot_time(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int LootPlayer::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required fixed64 last_loot_player_guid = 1;
    if (has_last_loot_player_guid()) {
      total_size += 1 + 8;
    }

    // required int32 unbind_coin = 2;
    if (has_unbind_coin()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->unbind_coin());
    }

    // required int32 be_loot_count_today = 3;
    if (has_be_loot_count_today()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->be_loot_count_today());
    }

    // required fixed64 last_be_loot_time = 4;
    if (has_last_be_loot_time()) {
      total_size += 1 + 8;
    }

    // required fixed64 last_loot_time = 5;
    if (has_last_loot_time()) {
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

void LootPlayer::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const LootPlayer* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const LootPlayer*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
  SetDirty();
}

void LootPlayer::MergeFrom(const LootPlayer& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_last_loot_player_guid()) {
      set_last_loot_player_guid(from.last_loot_player_guid());
    }
    if (from.has_unbind_coin()) {
      set_unbind_coin(from.unbind_coin());
    }
    if (from.has_be_loot_count_today()) {
      set_be_loot_count_today(from.be_loot_count_today());
    }
    if (from.has_last_be_loot_time()) {
      set_last_be_loot_time(from.last_be_loot_time());
    }
    if (from.has_last_loot_time()) {
      set_last_loot_time(from.last_loot_time());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  SetDirty();
}

void LootPlayer::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

void LootPlayer::CopyFrom(const LootPlayer& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

bool LootPlayer::IsInitialized() const {
  if ((_has_bits_[0] & 0x0000001f) != 0x0000001f) return false;

  return true;
}

void LootPlayer::SetInitialized() {
  _has_bits_[0] |= 0x0000001f;

  return;
}

void LootPlayer::Swap(LootPlayer* other) {
  if (other != this) {
    std::swap(last_loot_player_guid_, other->last_loot_player_guid_);
    std::swap(unbind_coin_, other->unbind_coin_);
    std::swap(be_loot_count_today_, other->be_loot_count_today_);
    std::swap(last_be_loot_time_, other->last_be_loot_time_);
    std::swap(last_loot_time_, other->last_loot_time_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
    SetDirty(), other->SetDirty();
  }
}

::google::protobuf::Metadata LootPlayer::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = LootPlayer_descriptor_;
  metadata.reflection = LootPlayer_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Db

// @@protoc_insertion_point(global_scope)
