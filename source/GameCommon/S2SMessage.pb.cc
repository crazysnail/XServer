// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: S2SMessage.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "S2SMessage.pb.h"

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

namespace S2S {

namespace {

const ::google::protobuf::Descriptor* S2SCommonMessage_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  S2SCommonMessage_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_S2SMessage_2eproto() {
  protobuf_AddDesc_S2SMessage_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "S2SMessage.proto");
  GOOGLE_CHECK(file != NULL);
  S2SCommonMessage_descriptor_ = file->message_type(0);
  static const int S2SCommonMessage_offsets_[4] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(S2SCommonMessage, request_name_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(S2SCommonMessage, int32_params_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(S2SCommonMessage, int64_params_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(S2SCommonMessage, string_params_),
  };
  S2SCommonMessage_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      S2SCommonMessage_descriptor_,
      S2SCommonMessage::default_instance_,
      S2SCommonMessage_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(S2SCommonMessage, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(S2SCommonMessage, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(S2SCommonMessage));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_S2SMessage_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    S2SCommonMessage_descriptor_, &S2SCommonMessage::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_S2SMessage_2eproto() {
  delete S2SCommonMessage::default_instance_;
  delete S2SCommonMessage_reflection_;
}

void protobuf_AddDesc_S2SMessage_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\020S2SMessage.proto\022\003S2S\"k\n\020S2SCommonMess"
    "age\022\024\n\014request_name\030\001 \002(\t\022\024\n\014int32_param"
    "s\030\002 \003(\005\022\024\n\014int64_params\030\003 \003(\006\022\025\n\rstring_"
    "params\030\004 \003(\t", 132);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "S2SMessage.proto", &protobuf_RegisterTypes);
  S2SCommonMessage::default_instance_ = new S2SCommonMessage();
  S2SCommonMessage::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_S2SMessage_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_S2SMessage_2eproto {
  StaticDescriptorInitializer_S2SMessage_2eproto() {
    protobuf_AddDesc_S2SMessage_2eproto();
  }
} static_descriptor_initializer_S2SMessage_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int S2SCommonMessage::kRequestNameFieldNumber;
const int S2SCommonMessage::kInt32ParamsFieldNumber;
const int S2SCommonMessage::kInt64ParamsFieldNumber;
const int S2SCommonMessage::kStringParamsFieldNumber;
#endif  // !_MSC_VER

S2SCommonMessage::S2SCommonMessage()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void S2SCommonMessage::InitAsDefaultInstance() {
}

S2SCommonMessage::S2SCommonMessage(const S2SCommonMessage& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void S2SCommonMessage::SharedCtor() {
  _cached_size_ = 0;
  request_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

S2SCommonMessage::~S2SCommonMessage() {
  SharedDtor();
}

void S2SCommonMessage::SharedDtor() {
  if (request_name_ != &::google::protobuf::internal::kEmptyString) {
    delete request_name_;
  }
  if (this != default_instance_) {
  }
}

void S2SCommonMessage::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* S2SCommonMessage::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return S2SCommonMessage_descriptor_;
}

const S2SCommonMessage& S2SCommonMessage::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_S2SMessage_2eproto();
  return *default_instance_;
}

S2SCommonMessage* S2SCommonMessage::default_instance_ = NULL;

S2SCommonMessage* S2SCommonMessage::New() const {
  return new S2SCommonMessage;
}

void S2SCommonMessage::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_request_name()) {
      if (request_name_ != &::google::protobuf::internal::kEmptyString) {
        request_name_->clear();
      }
    }
  }
  int32_params_.Clear();
  int64_params_.Clear();
  string_params_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
  SetDirty();
}

bool S2SCommonMessage::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required string request_name = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_request_name()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->request_name().data(), this->request_name().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_int32_params;
        break;
      }

      // repeated int32 int32_params = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_int32_params:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 16, input, this->mutable_int32_params())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_int32_params())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(16)) goto parse_int32_params;
        if (input->ExpectTag(25)) goto parse_int64_params;
        break;
      }

      // repeated fixed64 int64_params = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_FIXED64) {
         parse_int64_params:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 1, 25, input, this->mutable_int64_params())));
        } else if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag)
                   == ::google::protobuf::internal::WireFormatLite::
                      WIRETYPE_LENGTH_DELIMITED) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::uint64, ::google::protobuf::internal::WireFormatLite::TYPE_FIXED64>(
                 input, this->mutable_int64_params())));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(25)) goto parse_int64_params;
        if (input->ExpectTag(34)) goto parse_string_params;
        break;
      }

      // repeated string string_params = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_string_params:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_string_params()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->string_params(this->string_params_size() - 1).data(),
            this->string_params(this->string_params_size() - 1).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_string_params;
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

void S2SCommonMessage::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required string request_name = 1;
  if (has_request_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->request_name().data(), this->request_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      1, this->request_name(), output);
  }

  // repeated int32 int32_params = 2;
  for (int i = 0; i < this->int32_params_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      2, this->int32_params(i), output);
  }

  // repeated fixed64 int64_params = 3;
  for (int i = 0; i < this->int64_params_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteFixed64(
      3, this->int64_params(i), output);
  }

  // repeated string string_params = 4;
  for (int i = 0; i < this->string_params_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->string_params(i).data(), this->string_params(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->string_params(i), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* S2SCommonMessage::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required string request_name = 1;
  if (has_request_name()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->request_name().data(), this->request_name().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        1, this->request_name(), target);
  }

  // repeated int32 int32_params = 2;
  for (int i = 0; i < this->int32_params_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(2, this->int32_params(i), target);
  }

  // repeated fixed64 int64_params = 3;
  for (int i = 0; i < this->int64_params_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteFixed64ToArray(3, this->int64_params(i), target);
  }

  // repeated string string_params = 4;
  for (int i = 0; i < this->string_params_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->string_params(i).data(), this->string_params(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(4, this->string_params(i), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int S2SCommonMessage::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required string request_name = 1;
    if (has_request_name()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->request_name());
    }

  }
  // repeated int32 int32_params = 2;
  {
    int data_size = 0;
    for (int i = 0; i < this->int32_params_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->int32_params(i));
    }
    total_size += 1 * this->int32_params_size() + data_size;
  }

  // repeated fixed64 int64_params = 3;
  {
    int data_size = 0;
    data_size = 8 * this->int64_params_size();
    total_size += 1 * this->int64_params_size() + data_size;
  }

  // repeated string string_params = 4;
  total_size += 1 * this->string_params_size();
  for (int i = 0; i < this->string_params_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->string_params(i));
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

void S2SCommonMessage::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const S2SCommonMessage* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const S2SCommonMessage*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
  SetDirty();
}

void S2SCommonMessage::MergeFrom(const S2SCommonMessage& from) {
  GOOGLE_CHECK_NE(&from, this);
  int32_params_.MergeFrom(from.int32_params_);
  int64_params_.MergeFrom(from.int64_params_);
  string_params_.MergeFrom(from.string_params_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_request_name()) {
      set_request_name(from.request_name());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
  SetDirty();
}

void S2SCommonMessage::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

void S2SCommonMessage::CopyFrom(const S2SCommonMessage& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
  SetDirty();
}

bool S2SCommonMessage::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;

  return true;
}

void S2SCommonMessage::SetInitialized() {
  _has_bits_[0] |= 0x00000001;

  return;
}

void S2SCommonMessage::Swap(S2SCommonMessage* other) {
  if (other != this) {
    std::swap(request_name_, other->request_name_);
    int32_params_.Swap(&other->int32_params_);
    int64_params_.Swap(&other->int64_params_);
    string_params_.Swap(&other->string_params_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
    SetDirty(), other->SetDirty();
  }
}

::google::protobuf::Metadata S2SCommonMessage::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = S2SCommonMessage_descriptor_;
  metadata.reflection = S2SCommonMessage_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace S2S

// @@protoc_insertion_point(global_scope)
