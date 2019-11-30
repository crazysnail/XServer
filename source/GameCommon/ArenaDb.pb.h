// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ArenaDb.proto

#ifndef PROTOBUF_ArenaDb_2eproto__INCLUDED
#define PROTOBUF_ArenaDb_2eproto__INCLUDED

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
#include "ProtoBufOption.pb.h"
#include "PlayerBasicInfo.pb.h"
#include "ActorBasicInfo.pb.h"
#include "ArenaMessage.pb.h"
// @@protoc_insertion_point(includes)

namespace DB {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_ArenaDb_2eproto();
void protobuf_AssignDesc_ArenaDb_2eproto();
void protobuf_ShutdownFile_ArenaDb_2eproto();

class ArenaUser;
class RepeatedArenaLog;
class RobotInfo;
class PlayerTrialCopy;

// ===================================================================

class ArenaUser : public ::google::protobuf::Message {
 public:
  ArenaUser();
  virtual ~ArenaUser();

  ArenaUser(const ArenaUser& from);

  inline ArenaUser& operator=(const ArenaUser& from) {
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
  static const ArenaUser& default_instance();

  void Swap(ArenaUser* other);

  // implements Message ----------------------------------------------

  ArenaUser* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ArenaUser& from);
  void MergeFrom(const ArenaUser& from);
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

  // required fixed64 player_guid = 1;
  inline bool has_player_guid() const;
  inline void clear_player_guid();
  static const int kPlayerGuidFieldNumber = 1;
  inline ::google::protobuf::uint64 player_guid() const;
  inline void set_player_guid(::google::protobuf::uint64 value);

  // required int32 rank = 2;
  inline bool has_rank() const;
  inline void clear_rank();
  static const int kRankFieldNumber = 2;
  inline ::google::protobuf::int32 rank() const;
  inline void set_rank(::google::protobuf::int32 value);

  // required int32 yesterday_rank = 3;
  inline bool has_yesterday_rank() const;
  inline void clear_yesterday_rank();
  static const int kYesterdayRankFieldNumber = 3;
  inline ::google::protobuf::int32 yesterday_rank() const;
  inline void set_yesterday_rank(::google::protobuf::int32 value);

  // required bool is_robot = 4;
  inline bool has_is_robot() const;
  inline void clear_is_robot();
  static const int kIsRobotFieldNumber = 4;
  inline bool is_robot() const;
  inline void set_is_robot(bool value);

  // required int32 robot_id = 5;
  inline bool has_robot_id() const;
  inline void clear_robot_id();
  static const int kRobotIdFieldNumber = 5;
  inline ::google::protobuf::int32 robot_id() const;
  inline void set_robot_id(::google::protobuf::int32 value);

  // required int32 score = 6;
  inline bool has_score() const;
  inline void clear_score();
  static const int kScoreFieldNumber = 6;
  inline ::google::protobuf::int32 score() const;
  inline void set_score(::google::protobuf::int32 value);

  // required int64 updated_at = 8;
  inline bool has_updated_at() const;
  inline void clear_updated_at();
  static const int kUpdatedAtFieldNumber = 8;
  inline ::google::protobuf::int64 updated_at() const;
  inline void set_updated_at(::google::protobuf::int64 value);

  // required .DB.RepeatedArenaLog log = 10;
  inline bool has_log() const;
  inline void clear_log();
  static const int kLogFieldNumber = 10;
  inline const ::DB::RepeatedArenaLog& log() const;
  inline ::DB::RepeatedArenaLog* mutable_log();
  inline ::DB::RepeatedArenaLog* release_log();
  inline void set_allocated_log(::DB::RepeatedArenaLog* log);

  // required fixed64 last_reward_time = 11;
  inline bool has_last_reward_time() const;
  inline void clear_last_reward_time();
  static const int kLastRewardTimeFieldNumber = 11;
  inline ::google::protobuf::uint64 last_reward_time() const;
  inline void set_last_reward_time(::google::protobuf::uint64 value);

  // required .DB.PlayerTrialCopy trial_copy = 12;
  inline bool has_trial_copy() const;
  inline void clear_trial_copy();
  static const int kTrialCopyFieldNumber = 12;
  inline const ::DB::PlayerTrialCopy& trial_copy() const;
  inline ::DB::PlayerTrialCopy* mutable_trial_copy();
  inline ::DB::PlayerTrialCopy* release_trial_copy();
  inline void set_allocated_trial_copy(::DB::PlayerTrialCopy* trial_copy);

  // @@protoc_insertion_point(class_scope:DB.ArenaUser)
 private:
  inline void set_has_player_guid();
  inline void clear_has_player_guid();
  inline void set_has_rank();
  inline void clear_has_rank();
  inline void set_has_yesterday_rank();
  inline void clear_has_yesterday_rank();
  inline void set_has_is_robot();
  inline void clear_has_is_robot();
  inline void set_has_robot_id();
  inline void clear_has_robot_id();
  inline void set_has_score();
  inline void clear_has_score();
  inline void set_has_updated_at();
  inline void clear_has_updated_at();
  inline void set_has_log();
  inline void clear_has_log();
  inline void set_has_last_reward_time();
  inline void clear_has_last_reward_time();
  inline void set_has_trial_copy();
  inline void clear_has_trial_copy();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint64 player_guid_;
  ::google::protobuf::int32 rank_;
  ::google::protobuf::int32 yesterday_rank_;
  bool is_robot_;
  ::google::protobuf::int32 robot_id_;
  ::google::protobuf::int64 updated_at_;
  ::DB::RepeatedArenaLog* log_;
  ::google::protobuf::uint64 last_reward_time_;
  ::DB::PlayerTrialCopy* trial_copy_;
  ::google::protobuf::int32 score_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(10 + 31) / 32];

  friend void  protobuf_AddDesc_ArenaDb_2eproto();
  friend void protobuf_AssignDesc_ArenaDb_2eproto();
  friend void protobuf_ShutdownFile_ArenaDb_2eproto();

  void InitAsDefaultInstance();
  static ArenaUser* default_instance_;
};
// -------------------------------------------------------------------

class RepeatedArenaLog : public ::google::protobuf::Message {
 public:
  RepeatedArenaLog();
  virtual ~RepeatedArenaLog();

  RepeatedArenaLog(const RepeatedArenaLog& from);

  inline RepeatedArenaLog& operator=(const RepeatedArenaLog& from) {
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
  static const RepeatedArenaLog& default_instance();

  void Swap(RepeatedArenaLog* other);

  // implements Message ----------------------------------------------

  RepeatedArenaLog* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RepeatedArenaLog& from);
  void MergeFrom(const RepeatedArenaLog& from);
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

  // repeated .Packet.ArenaLog logs = 1;
  inline int logs_size() const;
  inline void clear_logs();
  static const int kLogsFieldNumber = 1;
  inline const ::Packet::ArenaLog& logs(int index) const;
  inline ::Packet::ArenaLog* mutable_logs(int index);
  inline ::Packet::ArenaLog* add_logs();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::ArenaLog >&
      logs() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::ArenaLog >*
      mutable_logs();

  // @@protoc_insertion_point(class_scope:DB.RepeatedArenaLog)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::Packet::ArenaLog > logs_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];

  friend void  protobuf_AddDesc_ArenaDb_2eproto();
  friend void protobuf_AssignDesc_ArenaDb_2eproto();
  friend void protobuf_ShutdownFile_ArenaDb_2eproto();

  void InitAsDefaultInstance();
  static RepeatedArenaLog* default_instance_;
};
// -------------------------------------------------------------------

class RobotInfo : public ::google::protobuf::Message {
 public:
  RobotInfo();
  virtual ~RobotInfo();

  RobotInfo(const RobotInfo& from);

  inline RobotInfo& operator=(const RobotInfo& from) {
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
  static const RobotInfo& default_instance();

  void Swap(RobotInfo* other);

  // implements Message ----------------------------------------------

  RobotInfo* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RobotInfo& from);
  void MergeFrom(const RobotInfo& from);
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

  // optional .Packet.PlayerBasicInfo player = 1;
  inline bool has_player() const;
  inline void clear_player();
  static const int kPlayerFieldNumber = 1;
  inline const ::Packet::PlayerBasicInfo& player() const;
  inline ::Packet::PlayerBasicInfo* mutable_player();
  inline ::Packet::PlayerBasicInfo* release_player();
  inline void set_allocated_player(::Packet::PlayerBasicInfo* player);

  // repeated .Packet.ActorFullInfo actors = 2;
  inline int actors_size() const;
  inline void clear_actors();
  static const int kActorsFieldNumber = 2;
  inline const ::Packet::ActorFullInfo& actors(int index) const;
  inline ::Packet::ActorFullInfo* mutable_actors(int index);
  inline ::Packet::ActorFullInfo* add_actors();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >&
      actors() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >*
      mutable_actors();

  // @@protoc_insertion_point(class_scope:DB.RobotInfo)
 private:
  inline void set_has_player();
  inline void clear_has_player();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::Packet::PlayerBasicInfo* player_;
  ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo > actors_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_ArenaDb_2eproto();
  friend void protobuf_AssignDesc_ArenaDb_2eproto();
  friend void protobuf_ShutdownFile_ArenaDb_2eproto();

  void InitAsDefaultInstance();
  static RobotInfo* default_instance_;
};
// -------------------------------------------------------------------

class PlayerTrialCopy : public ::google::protobuf::Message {
 public:
  PlayerTrialCopy();
  virtual ~PlayerTrialCopy();

  PlayerTrialCopy(const PlayerTrialCopy& from);

  inline PlayerTrialCopy& operator=(const PlayerTrialCopy& from) {
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
  static const PlayerTrialCopy& default_instance();

  void Swap(PlayerTrialCopy* other);

  // implements Message ----------------------------------------------

  PlayerTrialCopy* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const PlayerTrialCopy& from);
  void MergeFrom(const PlayerTrialCopy& from);
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

  // optional int32 init_actor_id = 1;
  inline bool has_init_actor_id() const;
  inline void clear_init_actor_id();
  static const int kInitActorIdFieldNumber = 1;
  inline ::google::protobuf::int32 init_actor_id() const;
  inline void set_init_actor_id(::google::protobuf::int32 value);

  // repeated .Packet.ActorFullInfo actors = 2;
  inline int actors_size() const;
  inline void clear_actors();
  static const int kActorsFieldNumber = 2;
  inline const ::Packet::ActorFullInfo& actors(int index) const;
  inline ::Packet::ActorFullInfo* mutable_actors(int index);
  inline ::Packet::ActorFullInfo* add_actors();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >&
      actors() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >*
      mutable_actors();

  // @@protoc_insertion_point(class_scope:DB.PlayerTrialCopy)
 private:
  inline void set_has_init_actor_id();
  inline void clear_has_init_actor_id();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo > actors_;
  ::google::protobuf::int32 init_actor_id_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(2 + 31) / 32];

  friend void  protobuf_AddDesc_ArenaDb_2eproto();
  friend void protobuf_AssignDesc_ArenaDb_2eproto();
  friend void protobuf_ShutdownFile_ArenaDb_2eproto();

  void InitAsDefaultInstance();
  static PlayerTrialCopy* default_instance_;
};
// ===================================================================


// ===================================================================

// ArenaUser

// required fixed64 player_guid = 1;
inline bool ArenaUser::has_player_guid() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ArenaUser::set_has_player_guid() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ArenaUser::clear_has_player_guid() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ArenaUser::clear_player_guid() {
  player_guid_ = GOOGLE_ULONGLONG(0);
  clear_has_player_guid();
  SetDirty();
}
inline ::google::protobuf::uint64 ArenaUser::player_guid() const {
  return player_guid_;
}
inline void ArenaUser::set_player_guid(::google::protobuf::uint64 value) {
  SetDirty();
  set_has_player_guid();
  player_guid_ = value;
}

// required int32 rank = 2;
inline bool ArenaUser::has_rank() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ArenaUser::set_has_rank() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ArenaUser::clear_has_rank() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ArenaUser::clear_rank() {
  rank_ = 0;
  clear_has_rank();
  SetDirty();
}
inline ::google::protobuf::int32 ArenaUser::rank() const {
  return rank_;
}
inline void ArenaUser::set_rank(::google::protobuf::int32 value) {
  SetDirty();
  set_has_rank();
  rank_ = value;
}

// required int32 yesterday_rank = 3;
inline bool ArenaUser::has_yesterday_rank() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ArenaUser::set_has_yesterday_rank() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ArenaUser::clear_has_yesterday_rank() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ArenaUser::clear_yesterday_rank() {
  yesterday_rank_ = 0;
  clear_has_yesterday_rank();
  SetDirty();
}
inline ::google::protobuf::int32 ArenaUser::yesterday_rank() const {
  return yesterday_rank_;
}
inline void ArenaUser::set_yesterday_rank(::google::protobuf::int32 value) {
  SetDirty();
  set_has_yesterday_rank();
  yesterday_rank_ = value;
}

// required bool is_robot = 4;
inline bool ArenaUser::has_is_robot() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ArenaUser::set_has_is_robot() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ArenaUser::clear_has_is_robot() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ArenaUser::clear_is_robot() {
  is_robot_ = false;
  clear_has_is_robot();
  SetDirty();
}
inline bool ArenaUser::is_robot() const {
  return is_robot_;
}
inline void ArenaUser::set_is_robot(bool value) {
  SetDirty();
  set_has_is_robot();
  is_robot_ = value;
}

// required int32 robot_id = 5;
inline bool ArenaUser::has_robot_id() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ArenaUser::set_has_robot_id() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ArenaUser::clear_has_robot_id() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ArenaUser::clear_robot_id() {
  robot_id_ = 0;
  clear_has_robot_id();
  SetDirty();
}
inline ::google::protobuf::int32 ArenaUser::robot_id() const {
  return robot_id_;
}
inline void ArenaUser::set_robot_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_robot_id();
  robot_id_ = value;
}

// required int32 score = 6;
inline bool ArenaUser::has_score() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void ArenaUser::set_has_score() {
  _has_bits_[0] |= 0x00000020u;
}
inline void ArenaUser::clear_has_score() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void ArenaUser::clear_score() {
  score_ = 0;
  clear_has_score();
  SetDirty();
}
inline ::google::protobuf::int32 ArenaUser::score() const {
  return score_;
}
inline void ArenaUser::set_score(::google::protobuf::int32 value) {
  SetDirty();
  set_has_score();
  score_ = value;
}

// required int64 updated_at = 8;
inline bool ArenaUser::has_updated_at() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void ArenaUser::set_has_updated_at() {
  _has_bits_[0] |= 0x00000040u;
}
inline void ArenaUser::clear_has_updated_at() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void ArenaUser::clear_updated_at() {
  updated_at_ = GOOGLE_LONGLONG(0);
  clear_has_updated_at();
  SetDirty();
}
inline ::google::protobuf::int64 ArenaUser::updated_at() const {
  return updated_at_;
}
inline void ArenaUser::set_updated_at(::google::protobuf::int64 value) {
  SetDirty();
  set_has_updated_at();
  updated_at_ = value;
}

// required .DB.RepeatedArenaLog log = 10;
inline bool ArenaUser::has_log() const {
  return (_has_bits_[0] & 0x00000080u) != 0;
}
inline void ArenaUser::set_has_log() {
  _has_bits_[0] |= 0x00000080u;
}
inline void ArenaUser::clear_has_log() {
  _has_bits_[0] &= ~0x00000080u;
}
inline void ArenaUser::clear_log() {
  if (log_ != NULL) log_->::DB::RepeatedArenaLog::Clear();
  clear_has_log();
  SetDirty();
}
inline const ::DB::RepeatedArenaLog& ArenaUser::log() const {
  return log_ != NULL ? *log_ : *default_instance_->log_;
}
inline ::DB::RepeatedArenaLog* ArenaUser::mutable_log() {
  SetDirty();
  set_has_log();
  if (log_ == NULL) log_ = new ::DB::RepeatedArenaLog;
  return log_;
}
inline ::DB::RepeatedArenaLog* ArenaUser::release_log() {
  SetDirty();
  clear_has_log();
  ::DB::RepeatedArenaLog* temp = log_;
  log_ = NULL;
  return temp;
}
inline void ArenaUser::set_allocated_log(::DB::RepeatedArenaLog* log) {
  SetDirty();
  delete log_;
  log_ = log;
  if (log) {
    set_has_log();
  } else {
    clear_has_log();
  }
}

// required fixed64 last_reward_time = 11;
inline bool ArenaUser::has_last_reward_time() const {
  return (_has_bits_[0] & 0x00000100u) != 0;
}
inline void ArenaUser::set_has_last_reward_time() {
  _has_bits_[0] |= 0x00000100u;
}
inline void ArenaUser::clear_has_last_reward_time() {
  _has_bits_[0] &= ~0x00000100u;
}
inline void ArenaUser::clear_last_reward_time() {
  last_reward_time_ = GOOGLE_ULONGLONG(0);
  clear_has_last_reward_time();
  SetDirty();
}
inline ::google::protobuf::uint64 ArenaUser::last_reward_time() const {
  return last_reward_time_;
}
inline void ArenaUser::set_last_reward_time(::google::protobuf::uint64 value) {
  SetDirty();
  set_has_last_reward_time();
  last_reward_time_ = value;
}

// required .DB.PlayerTrialCopy trial_copy = 12;
inline bool ArenaUser::has_trial_copy() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void ArenaUser::set_has_trial_copy() {
  _has_bits_[0] |= 0x00000200u;
}
inline void ArenaUser::clear_has_trial_copy() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void ArenaUser::clear_trial_copy() {
  if (trial_copy_ != NULL) trial_copy_->::DB::PlayerTrialCopy::Clear();
  clear_has_trial_copy();
  SetDirty();
}
inline const ::DB::PlayerTrialCopy& ArenaUser::trial_copy() const {
  return trial_copy_ != NULL ? *trial_copy_ : *default_instance_->trial_copy_;
}
inline ::DB::PlayerTrialCopy* ArenaUser::mutable_trial_copy() {
  SetDirty();
  set_has_trial_copy();
  if (trial_copy_ == NULL) trial_copy_ = new ::DB::PlayerTrialCopy;
  return trial_copy_;
}
inline ::DB::PlayerTrialCopy* ArenaUser::release_trial_copy() {
  SetDirty();
  clear_has_trial_copy();
  ::DB::PlayerTrialCopy* temp = trial_copy_;
  trial_copy_ = NULL;
  return temp;
}
inline void ArenaUser::set_allocated_trial_copy(::DB::PlayerTrialCopy* trial_copy) {
  SetDirty();
  delete trial_copy_;
  trial_copy_ = trial_copy;
  if (trial_copy) {
    set_has_trial_copy();
  } else {
    clear_has_trial_copy();
  }
}

// -------------------------------------------------------------------

// RepeatedArenaLog

// repeated .Packet.ArenaLog logs = 1;
inline int RepeatedArenaLog::logs_size() const {
  return logs_.size();
}
inline void RepeatedArenaLog::clear_logs() {
  logs_.Clear();
  SetDirty();
}
inline const ::Packet::ArenaLog& RepeatedArenaLog::logs(int index) const {
  return logs_.Get(index);
}
inline ::Packet::ArenaLog* RepeatedArenaLog::mutable_logs(int index) {
  SetDirty();
  return logs_.Mutable(index);
}
inline ::Packet::ArenaLog* RepeatedArenaLog::add_logs() {
  SetDirty();
  return logs_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::ArenaLog >&
RepeatedArenaLog::logs() const {
  return logs_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::ArenaLog >*
RepeatedArenaLog::mutable_logs() {
  SetDirty();
  return &logs_;
}

// -------------------------------------------------------------------

// RobotInfo

// optional .Packet.PlayerBasicInfo player = 1;
inline bool RobotInfo::has_player() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void RobotInfo::set_has_player() {
  _has_bits_[0] |= 0x00000001u;
}
inline void RobotInfo::clear_has_player() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void RobotInfo::clear_player() {
  if (player_ != NULL) player_->::Packet::PlayerBasicInfo::Clear();
  clear_has_player();
  SetDirty();
}
inline const ::Packet::PlayerBasicInfo& RobotInfo::player() const {
  return player_ != NULL ? *player_ : *default_instance_->player_;
}
inline ::Packet::PlayerBasicInfo* RobotInfo::mutable_player() {
  SetDirty();
  set_has_player();
  if (player_ == NULL) player_ = new ::Packet::PlayerBasicInfo;
  return player_;
}
inline ::Packet::PlayerBasicInfo* RobotInfo::release_player() {
  SetDirty();
  clear_has_player();
  ::Packet::PlayerBasicInfo* temp = player_;
  player_ = NULL;
  return temp;
}
inline void RobotInfo::set_allocated_player(::Packet::PlayerBasicInfo* player) {
  SetDirty();
  delete player_;
  player_ = player;
  if (player) {
    set_has_player();
  } else {
    clear_has_player();
  }
}

// repeated .Packet.ActorFullInfo actors = 2;
inline int RobotInfo::actors_size() const {
  return actors_.size();
}
inline void RobotInfo::clear_actors() {
  actors_.Clear();
  SetDirty();
}
inline const ::Packet::ActorFullInfo& RobotInfo::actors(int index) const {
  return actors_.Get(index);
}
inline ::Packet::ActorFullInfo* RobotInfo::mutable_actors(int index) {
  SetDirty();
  return actors_.Mutable(index);
}
inline ::Packet::ActorFullInfo* RobotInfo::add_actors() {
  SetDirty();
  return actors_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >&
RobotInfo::actors() const {
  return actors_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >*
RobotInfo::mutable_actors() {
  SetDirty();
  return &actors_;
}

// -------------------------------------------------------------------

// PlayerTrialCopy

// optional int32 init_actor_id = 1;
inline bool PlayerTrialCopy::has_init_actor_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void PlayerTrialCopy::set_has_init_actor_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void PlayerTrialCopy::clear_has_init_actor_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void PlayerTrialCopy::clear_init_actor_id() {
  init_actor_id_ = 0;
  clear_has_init_actor_id();
  SetDirty();
}
inline ::google::protobuf::int32 PlayerTrialCopy::init_actor_id() const {
  return init_actor_id_;
}
inline void PlayerTrialCopy::set_init_actor_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_init_actor_id();
  init_actor_id_ = value;
}

// repeated .Packet.ActorFullInfo actors = 2;
inline int PlayerTrialCopy::actors_size() const {
  return actors_.size();
}
inline void PlayerTrialCopy::clear_actors() {
  actors_.Clear();
  SetDirty();
}
inline const ::Packet::ActorFullInfo& PlayerTrialCopy::actors(int index) const {
  return actors_.Get(index);
}
inline ::Packet::ActorFullInfo* PlayerTrialCopy::mutable_actors(int index) {
  SetDirty();
  return actors_.Mutable(index);
}
inline ::Packet::ActorFullInfo* PlayerTrialCopy::add_actors() {
  SetDirty();
  return actors_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >&
PlayerTrialCopy::actors() const {
  return actors_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::ActorFullInfo >*
PlayerTrialCopy::mutable_actors() {
  SetDirty();
  return &actors_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace DB

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ArenaDb_2eproto__INCLUDED
