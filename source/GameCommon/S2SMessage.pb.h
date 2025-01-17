// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: S2SMessage.proto

#ifndef PROTOBUF_S2SMessage_2eproto__INCLUDED
#define PROTOBUF_S2SMessage_2eproto__INCLUDED

#include <string>

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

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace S2S {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_S2SMessage_2eproto();
void protobuf_AssignDesc_S2SMessage_2eproto();
void protobuf_ShutdownFile_S2SMessage_2eproto();

class S2SCommonMessage;

// ===================================================================

class S2SCommonMessage : public ::google::protobuf::Message {
 public:
  S2SCommonMessage();
  virtual ~S2SCommonMessage();

  S2SCommonMessage(const S2SCommonMessage& from);

  inline S2SCommonMessage& operator=(const S2SCommonMessage& from) {
    CopyFrom(from);
    return *this;
  }

  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }

  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const S2SCommonMessage& default_instance();

  void Swap(S2SCommonMessage* other);

  // implements Message ----------------------------------------------

  S2SCommonMessage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const S2SCommonMessage& from);
  void MergeFrom(const S2SCommonMessage& from);
  void Clear();
  bool IsInitialized() const;
  void SetInitialized();

  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:

  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // required string request_name = 1;
  inline bool has_request_name() const;
  inline void clear_request_name();
  static const int kRequestNameFieldNumber = 1;
  inline const ::std::string& request_name() const;
  inline void set_request_name(const ::std::string& value);
  inline void set_request_name(const char* value);
  inline void set_request_name(const char* value, size_t size);
  inline ::std::string* mutable_request_name();
  inline ::std::string* release_request_name();
  inline void set_allocated_request_name(::std::string* request_name);

  // repeated int32 int32_params = 2;
  inline int int32_params_size() const;
  inline void clear_int32_params();
  static const int kInt32ParamsFieldNumber = 2;
  inline ::google::protobuf::int32 int32_params(int index) const;
  inline void set_int32_params(int index, ::google::protobuf::int32 value);
  inline void add_int32_params(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      int32_params() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_int32_params();

  // repeated fixed64 int64_params = 3;
  inline int int64_params_size() const;
  inline void clear_int64_params();
  static const int kInt64ParamsFieldNumber = 3;
  inline ::google::protobuf::uint64 int64_params(int index) const;
  inline void set_int64_params(int index, ::google::protobuf::uint64 value);
  inline void add_int64_params(::google::protobuf::uint64 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >&
      int64_params() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >*
      mutable_int64_params();

  // repeated string string_params = 4;
  inline int string_params_size() const;
  inline void clear_string_params();
  static const int kStringParamsFieldNumber = 4;
  inline const ::std::string& string_params(int index) const;
  inline ::std::string* mutable_string_params(int index);
  inline void set_string_params(int index, const ::std::string& value);
  inline void set_string_params(int index, const char* value);
  inline void set_string_params(int index, const char* value, size_t size);
  inline ::std::string* add_string_params();
  inline void add_string_params(const ::std::string& value);
  inline void add_string_params(const char* value);
  inline void add_string_params(const char* value, size_t size);
  inline const ::google::protobuf::RepeatedPtrField< ::std::string>& string_params() const;
  inline ::google::protobuf::RepeatedPtrField< ::std::string>* mutable_string_params();

  // @@protoc_insertion_point(class_scope:S2S.S2SCommonMessage)
 private:
  inline void set_has_request_name();
  inline void clear_has_request_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* request_name_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > int32_params_;
  ::google::protobuf::RepeatedField< ::google::protobuf::uint64 > int64_params_;
  ::google::protobuf::RepeatedPtrField< ::std::string> string_params_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(4 + 31) / 32];

  friend void  protobuf_AddDesc_S2SMessage_2eproto();
  friend void protobuf_AssignDesc_S2SMessage_2eproto();
  friend void protobuf_ShutdownFile_S2SMessage_2eproto();

  void InitAsDefaultInstance();
  static S2SCommonMessage* default_instance_;
};
// ===================================================================


// ===================================================================

// S2SCommonMessage

// required string request_name = 1;
inline bool S2SCommonMessage::has_request_name() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void S2SCommonMessage::set_has_request_name() {
  _has_bits_[0] |= 0x00000001u;
}
inline void S2SCommonMessage::clear_has_request_name() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void S2SCommonMessage::clear_request_name() {
  if (request_name_ != &::google::protobuf::internal::kEmptyString) {
    request_name_->clear();
  }
  clear_has_request_name();
  SetDirty();
}
inline const ::std::string& S2SCommonMessage::request_name() const {
  return *request_name_;
}
inline void S2SCommonMessage::set_request_name(const ::std::string& value) {
  SetDirty();
  set_has_request_name();
  if (request_name_ == &::google::protobuf::internal::kEmptyString) {
    request_name_ = new ::std::string;
  }
  request_name_->assign(value);
}
inline void S2SCommonMessage::set_request_name(const char* value) {
  SetDirty();
  set_has_request_name();
  if (request_name_ == &::google::protobuf::internal::kEmptyString) {
    request_name_ = new ::std::string;
  }
  request_name_->assign(value);
}
inline void S2SCommonMessage::set_request_name(const char* value, size_t size) {
  SetDirty();
  set_has_request_name();
  if (request_name_ == &::google::protobuf::internal::kEmptyString) {
    request_name_ = new ::std::string;
  }
  request_name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* S2SCommonMessage::mutable_request_name() {
  SetDirty();
  set_has_request_name();
  if (request_name_ == &::google::protobuf::internal::kEmptyString) {
    request_name_ = new ::std::string;
  }
  return request_name_;
}
inline ::std::string* S2SCommonMessage::release_request_name() {
  SetDirty();
  clear_has_request_name();
  if (request_name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = request_name_;
    request_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void S2SCommonMessage::set_allocated_request_name(::std::string* request_name) {
  SetDirty();
  if (request_name_ != &::google::protobuf::internal::kEmptyString) {
    delete request_name_;
  }
  if (request_name) {
    set_has_request_name();
    request_name_ = request_name;
  } else {
    clear_has_request_name();
    request_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated int32 int32_params = 2;
inline int S2SCommonMessage::int32_params_size() const {
  return int32_params_.size();
}
inline void S2SCommonMessage::clear_int32_params() {
  int32_params_.Clear();
  SetDirty();
}
inline ::google::protobuf::int32 S2SCommonMessage::int32_params(int index) const {
  return int32_params_.Get(index);
}
inline void S2SCommonMessage::set_int32_params(int index, ::google::protobuf::int32 value) {
  SetDirty();
  int32_params_.Set(index, value);
}
inline void S2SCommonMessage::add_int32_params(::google::protobuf::int32 value) {
  SetDirty();
  int32_params_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
S2SCommonMessage::int32_params() const {
  return int32_params_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
S2SCommonMessage::mutable_int32_params() {
  SetDirty();
  return &int32_params_;
}

// repeated fixed64 int64_params = 3;
inline int S2SCommonMessage::int64_params_size() const {
  return int64_params_.size();
}
inline void S2SCommonMessage::clear_int64_params() {
  int64_params_.Clear();
  SetDirty();
}
inline ::google::protobuf::uint64 S2SCommonMessage::int64_params(int index) const {
  return int64_params_.Get(index);
}
inline void S2SCommonMessage::set_int64_params(int index, ::google::protobuf::uint64 value) {
  SetDirty();
  int64_params_.Set(index, value);
}
inline void S2SCommonMessage::add_int64_params(::google::protobuf::uint64 value) {
  SetDirty();
  int64_params_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >&
S2SCommonMessage::int64_params() const {
  return int64_params_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::uint64 >*
S2SCommonMessage::mutable_int64_params() {
  SetDirty();
  return &int64_params_;
}

// repeated string string_params = 4;
inline int S2SCommonMessage::string_params_size() const {
  return string_params_.size();
}
inline void S2SCommonMessage::clear_string_params() {
  string_params_.Clear();
  SetDirty();
}
inline const ::std::string& S2SCommonMessage::string_params(int index) const {
  return string_params_.Get(index);
}
inline ::std::string* S2SCommonMessage::mutable_string_params(int index) {
  SetDirty();
  return string_params_.Mutable(index);
}
inline void S2SCommonMessage::set_string_params(int index, const ::std::string& value) {
  SetDirty();
  string_params_.Mutable(index)->assign(value);
}
inline void S2SCommonMessage::set_string_params(int index, const char* value) {
  SetDirty();
  string_params_.Mutable(index)->assign(value);
}
inline void S2SCommonMessage::set_string_params(int index, const char* value, size_t size) {
  SetDirty();
  string_params_.Mutable(index)->assign(
    reinterpret_cast<const char*>(value), size);
}
inline ::std::string* S2SCommonMessage::add_string_params() {
  SetDirty();
  return string_params_.Add();
}
inline void S2SCommonMessage::add_string_params(const ::std::string& value) {
  SetDirty();
  string_params_.Add()->assign(value);
}
inline void S2SCommonMessage::add_string_params(const char* value) {
  SetDirty();
  string_params_.Add()->assign(value);
}
inline void S2SCommonMessage::add_string_params(const char* value, size_t size) {
  SetDirty();
  string_params_.Add()->assign(reinterpret_cast<const char*>(value), size);
}
inline const ::google::protobuf::RepeatedPtrField< ::std::string>&
S2SCommonMessage::string_params() const {
  return string_params_;
}
inline ::google::protobuf::RepeatedPtrField< ::std::string>*
S2SCommonMessage::mutable_string_params() {
  SetDirty();
  return &string_params_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace S2S

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_S2SMessage_2eproto__INCLUDED
