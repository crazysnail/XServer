// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: HeroFragment.proto

#ifndef PROTOBUF_HeroFragment_2eproto__INCLUDED
#define PROTOBUF_HeroFragment_2eproto__INCLUDED

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
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace Packet {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_HeroFragment_2eproto();
void protobuf_AssignDesc_HeroFragment_2eproto();
void protobuf_ShutdownFile_HeroFragment_2eproto();

class FragmentToHeroReq;
class FragmentToHeroReply;

enum FragmentToHeroReply_Result {
  FragmentToHeroReply_Result_SUCCESS = 1,
  FragmentToHeroReply_Result_NOT_ENOUGH_FRAGMENT = 2,
  FragmentToHeroReply_Result_ALREADY_OWN_TARGET_HERO = 3
};
bool FragmentToHeroReply_Result_IsValid(int value);
const FragmentToHeroReply_Result FragmentToHeroReply_Result_Result_MIN = FragmentToHeroReply_Result_SUCCESS;
const FragmentToHeroReply_Result FragmentToHeroReply_Result_Result_MAX = FragmentToHeroReply_Result_ALREADY_OWN_TARGET_HERO;
const int FragmentToHeroReply_Result_Result_ARRAYSIZE = FragmentToHeroReply_Result_Result_MAX + 1;

const ::google::protobuf::EnumDescriptor* FragmentToHeroReply_Result_descriptor();
inline const ::std::string& FragmentToHeroReply_Result_Name(FragmentToHeroReply_Result value) {
  return ::google::protobuf::internal::NameOfEnum(
    FragmentToHeroReply_Result_descriptor(), value);
}
inline bool FragmentToHeroReply_Result_Parse(
    const ::std::string& name, FragmentToHeroReply_Result* value) {
  return ::google::protobuf::internal::ParseNamedEnum<FragmentToHeroReply_Result>(
    FragmentToHeroReply_Result_descriptor(), name, value);
}
// ===================================================================

class FragmentToHeroReq : public ::google::protobuf::Message {
 public:
  FragmentToHeroReq();
  virtual ~FragmentToHeroReq();

  FragmentToHeroReq(const FragmentToHeroReq& from);

  inline FragmentToHeroReq& operator=(const FragmentToHeroReq& from) {
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
  static const FragmentToHeroReq& default_instance();

  void Swap(FragmentToHeroReq* other);

  // implements Message ----------------------------------------------

  FragmentToHeroReq* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const FragmentToHeroReq& from);
  void MergeFrom(const FragmentToHeroReq& from);
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

  // required fixed64 item_guid = 1;
  inline bool has_item_guid() const;
  inline void clear_item_guid();
  static const int kItemGuidFieldNumber = 1;
  inline ::google::protobuf::uint64 item_guid() const;
  inline void set_item_guid(::google::protobuf::uint64 value);

  // @@protoc_insertion_point(class_scope:Packet.FragmentToHeroReq)
 private:
  inline void set_has_item_guid();
  inline void clear_has_item_guid();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint64 item_guid_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_HeroFragment_2eproto();
  friend void protobuf_AssignDesc_HeroFragment_2eproto();
  friend void protobuf_ShutdownFile_HeroFragment_2eproto();

  void InitAsDefaultInstance();
  static FragmentToHeroReq* default_instance_;
};
// -------------------------------------------------------------------

class FragmentToHeroReply : public ::google::protobuf::Message {
 public:
  FragmentToHeroReply();
  virtual ~FragmentToHeroReply();

  FragmentToHeroReply(const FragmentToHeroReply& from);

  inline FragmentToHeroReply& operator=(const FragmentToHeroReply& from) {
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
  static const FragmentToHeroReply& default_instance();

  void Swap(FragmentToHeroReply* other);

  // implements Message ----------------------------------------------

  FragmentToHeroReply* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const FragmentToHeroReply& from);
  void MergeFrom(const FragmentToHeroReply& from);
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

  typedef FragmentToHeroReply_Result Result;
  static const Result SUCCESS = FragmentToHeroReply_Result_SUCCESS;
  static const Result NOT_ENOUGH_FRAGMENT = FragmentToHeroReply_Result_NOT_ENOUGH_FRAGMENT;
  static const Result ALREADY_OWN_TARGET_HERO = FragmentToHeroReply_Result_ALREADY_OWN_TARGET_HERO;
  static inline bool Result_IsValid(int value) {
    return FragmentToHeroReply_Result_IsValid(value);
  }
  static const Result Result_MIN =
    FragmentToHeroReply_Result_Result_MIN;
  static const Result Result_MAX =
    FragmentToHeroReply_Result_Result_MAX;
  static const int Result_ARRAYSIZE =
    FragmentToHeroReply_Result_Result_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  Result_descriptor() {
    return FragmentToHeroReply_Result_descriptor();
  }
  static inline const ::std::string& Result_Name(Result value) {
    return FragmentToHeroReply_Result_Name(value);
  }
  static inline bool Result_Parse(const ::std::string& name,
      Result* value) {
    return FragmentToHeroReply_Result_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // required .Packet.FragmentToHeroReq req = 1;
  inline bool has_req() const;
  inline void clear_req();
  static const int kReqFieldNumber = 1;
  inline const ::Packet::FragmentToHeroReq& req() const;
  inline ::Packet::FragmentToHeroReq* mutable_req();
  inline ::Packet::FragmentToHeroReq* release_req();
  inline void set_allocated_req(::Packet::FragmentToHeroReq* req);

  // required .Packet.FragmentToHeroReply.Result result = 2;
  inline bool has_result() const;
  inline void clear_result();
  static const int kResultFieldNumber = 2;
  inline ::Packet::FragmentToHeroReply_Result result() const;
  inline void set_result(::Packet::FragmentToHeroReply_Result value);

  // @@protoc_insertion_point(class_scope:Packet.FragmentToHeroReply)
 private:
  inline void set_has_req();
  inline void clear_has_req();
  inline void set_has_result();
  inline void clear_has_result();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::Packet::FragmentToHeroReq* req_;
  int result_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_HeroFragment_2eproto();
  friend void protobuf_AssignDesc_HeroFragment_2eproto();
  friend void protobuf_ShutdownFile_HeroFragment_2eproto();

  void InitAsDefaultInstance();
  static FragmentToHeroReply* default_instance_;
};
// ===================================================================


// ===================================================================

// FragmentToHeroReq

// required fixed64 item_guid = 1;
inline bool FragmentToHeroReq::has_item_guid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void FragmentToHeroReq::set_has_item_guid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void FragmentToHeroReq::clear_has_item_guid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void FragmentToHeroReq::clear_item_guid() {
  item_guid_ = GOOGLE_ULONGLONG(0);
  clear_has_item_guid();
  SetDirty();
}
inline ::google::protobuf::uint64 FragmentToHeroReq::item_guid() const {
  return item_guid_;
}
inline void FragmentToHeroReq::set_item_guid(::google::protobuf::uint64 value) {
  SetDirty();
  set_has_item_guid();
  item_guid_ = value;
}

// -------------------------------------------------------------------

// FragmentToHeroReply

// required .Packet.FragmentToHeroReq req = 1;
inline bool FragmentToHeroReply::has_req() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void FragmentToHeroReply::set_has_req() {
  _has_bits_[0] |= 0x00000001u;
}
inline void FragmentToHeroReply::clear_has_req() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void FragmentToHeroReply::clear_req() {
  if (req_ != NULL) req_->::Packet::FragmentToHeroReq::Clear();
  clear_has_req();
  SetDirty();
}
inline const ::Packet::FragmentToHeroReq& FragmentToHeroReply::req() const {
  return req_ != NULL ? *req_ : *default_instance_->req_;
}
inline ::Packet::FragmentToHeroReq* FragmentToHeroReply::mutable_req() {
  SetDirty();
  set_has_req();
  if (req_ == NULL) req_ = new ::Packet::FragmentToHeroReq;
  return req_;
}
inline ::Packet::FragmentToHeroReq* FragmentToHeroReply::release_req() {
  SetDirty();
  clear_has_req();
  ::Packet::FragmentToHeroReq* temp = req_;
  req_ = NULL;
  return temp;
}
inline void FragmentToHeroReply::set_allocated_req(::Packet::FragmentToHeroReq* req) {
  SetDirty();
  delete req_;
  req_ = req;
  if (req) {
    set_has_req();
  } else {
    clear_has_req();
  }
}

// required .Packet.FragmentToHeroReply.Result result = 2;
inline bool FragmentToHeroReply::has_result() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void FragmentToHeroReply::set_has_result() {
  _has_bits_[0] |= 0x00000002u;
}
inline void FragmentToHeroReply::clear_has_result() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void FragmentToHeroReply::clear_result() {
  result_ = 1;
  clear_has_result();
  SetDirty();
}
inline ::Packet::FragmentToHeroReply_Result FragmentToHeroReply::result() const {
  return static_cast< ::Packet::FragmentToHeroReply_Result >(result_);
}
inline void FragmentToHeroReply::set_result(::Packet::FragmentToHeroReply_Result value) {
  assert(::Packet::FragmentToHeroReply_Result_IsValid(value));
  SetDirty();
  set_has_result();
  result_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Packet

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::Packet::FragmentToHeroReply_Result>() {
  return ::Packet::FragmentToHeroReply_Result_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_HeroFragment_2eproto__INCLUDED
