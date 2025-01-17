// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: SceneConfig.proto

#ifndef PROTOBUF_SceneConfig_2eproto__INCLUDED
#define PROTOBUF_SceneConfig_2eproto__INCLUDED

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
#include "Base.pb.h"
#include "ProtoBufOption.pb.h"
// @@protoc_insertion_point(includes)

namespace Config {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_SceneConfig_2eproto();
void protobuf_AssignDesc_SceneConfig_2eproto();
void protobuf_ShutdownFile_SceneConfig_2eproto();

class Scene;
class SceneBattleGroup;
class SceneStage;

// ===================================================================

class Scene : public ::google::protobuf::Message {
 public:
  Scene();
  virtual ~Scene();

  Scene(const Scene& from);

  inline Scene& operator=(const Scene& from) {
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
  static const Scene& default_instance();

  void Swap(Scene* other);

  // implements Message ----------------------------------------------

  Scene* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Scene& from);
  void MergeFrom(const Scene& from);
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

  // required int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // required string scene_name = 2;
  inline bool has_scene_name() const;
  inline void clear_scene_name();
  static const int kSceneNameFieldNumber = 2;
  inline const ::std::string& scene_name() const;
  inline void set_scene_name(const ::std::string& value);
  inline void set_scene_name(const char* value);
  inline void set_scene_name(const char* value, size_t size);
  inline ::std::string* mutable_scene_name();
  inline ::std::string* release_scene_name();
  inline void set_allocated_scene_name(::std::string* scene_name);

  // repeated .Packet.Position areas = 3;
  inline int areas_size() const;
  inline void clear_areas();
  static const int kAreasFieldNumber = 3;
  inline const ::Packet::Position& areas(int index) const;
  inline ::Packet::Position* mutable_areas(int index);
  inline ::Packet::Position* add_areas();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
      areas() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
      mutable_areas();

  // repeated .Packet.Position spawn_pos = 4;
  inline int spawn_pos_size() const;
  inline void clear_spawn_pos();
  static const int kSpawnPosFieldNumber = 4;
  inline const ::Packet::Position& spawn_pos(int index) const;
  inline ::Packet::Position* mutable_spawn_pos(int index);
  inline ::Packet::Position* add_spawn_pos();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
      spawn_pos() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
      mutable_spawn_pos();

  // repeated .Config.SceneBattleGroup battle_groups = 5;
  inline int battle_groups_size() const;
  inline void clear_battle_groups();
  static const int kBattleGroupsFieldNumber = 5;
  inline const ::Config::SceneBattleGroup& battle_groups(int index) const;
  inline ::Config::SceneBattleGroup* mutable_battle_groups(int index);
  inline ::Config::SceneBattleGroup* add_battle_groups();
  inline const ::google::protobuf::RepeatedPtrField< ::Config::SceneBattleGroup >&
      battle_groups() const;
  inline ::google::protobuf::RepeatedPtrField< ::Config::SceneBattleGroup >*
      mutable_battle_groups();

  // @@protoc_insertion_point(class_scope:Config.Scene)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_scene_name();
  inline void clear_has_scene_name();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::std::string* scene_name_;
  ::google::protobuf::RepeatedPtrField< ::Packet::Position > areas_;
  ::google::protobuf::RepeatedPtrField< ::Packet::Position > spawn_pos_;
  ::google::protobuf::RepeatedPtrField< ::Config::SceneBattleGroup > battle_groups_;
  ::google::protobuf::int32 id_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_SceneConfig_2eproto();
  friend void protobuf_AssignDesc_SceneConfig_2eproto();
  friend void protobuf_ShutdownFile_SceneConfig_2eproto();

  void InitAsDefaultInstance();
  static Scene* default_instance_;
};
// -------------------------------------------------------------------

class SceneBattleGroup : public ::google::protobuf::Message {
 public:
  SceneBattleGroup();
  virtual ~SceneBattleGroup();

  SceneBattleGroup(const SceneBattleGroup& from);

  inline SceneBattleGroup& operator=(const SceneBattleGroup& from) {
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
  static const SceneBattleGroup& default_instance();

  void Swap(SceneBattleGroup* other);

  // implements Message ----------------------------------------------

  SceneBattleGroup* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SceneBattleGroup& from);
  void MergeFrom(const SceneBattleGroup& from);
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

  // required int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // required int32 index = 2;
  inline bool has_index() const;
  inline void clear_index();
  static const int kIndexFieldNumber = 2;
  inline ::google::protobuf::int32 index() const;
  inline void set_index(::google::protobuf::int32 value);

  // repeated .Packet.Position area_pos = 3;
  inline int area_pos_size() const;
  inline void clear_area_pos();
  static const int kAreaPosFieldNumber = 3;
  inline const ::Packet::Position& area_pos(int index) const;
  inline ::Packet::Position* mutable_area_pos(int index);
  inline ::Packet::Position* add_area_pos();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
      area_pos() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
      mutable_area_pos();

  // repeated .Packet.Position player_pos = 4;
  inline int player_pos_size() const;
  inline void clear_player_pos();
  static const int kPlayerPosFieldNumber = 4;
  inline const ::Packet::Position& player_pos(int index) const;
  inline ::Packet::Position* mutable_player_pos(int index);
  inline ::Packet::Position* add_player_pos();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
      player_pos() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
      mutable_player_pos();

  // repeated .Packet.Position enemy_pos = 5;
  inline int enemy_pos_size() const;
  inline void clear_enemy_pos();
  static const int kEnemyPosFieldNumber = 5;
  inline const ::Packet::Position& enemy_pos(int index) const;
  inline ::Packet::Position* mutable_enemy_pos(int index);
  inline ::Packet::Position* add_enemy_pos();
  inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
      enemy_pos() const;
  inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
      mutable_enemy_pos();

  // @@protoc_insertion_point(class_scope:Config.SceneBattleGroup)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_index();
  inline void clear_has_index();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 id_;
  ::google::protobuf::int32 index_;
  ::google::protobuf::RepeatedPtrField< ::Packet::Position > area_pos_;
  ::google::protobuf::RepeatedPtrField< ::Packet::Position > player_pos_;
  ::google::protobuf::RepeatedPtrField< ::Packet::Position > enemy_pos_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];

  friend void  protobuf_AddDesc_SceneConfig_2eproto();
  friend void protobuf_AssignDesc_SceneConfig_2eproto();
  friend void protobuf_ShutdownFile_SceneConfig_2eproto();

  void InitAsDefaultInstance();
  static SceneBattleGroup* default_instance_;
};
// -------------------------------------------------------------------

class SceneStage : public ::google::protobuf::Message {
 public:
  SceneStage();
  virtual ~SceneStage();

  SceneStage(const SceneStage& from);

  inline SceneStage& operator=(const SceneStage& from) {
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
  static const SceneStage& default_instance();

  void Swap(SceneStage* other);

  // implements Message ----------------------------------------------

  SceneStage* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const SceneStage& from);
  void MergeFrom(const SceneStage& from);
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

  // required int32 id = 1;
  inline bool has_id() const;
  inline void clear_id();
  static const int kIdFieldNumber = 1;
  inline ::google::protobuf::int32 id() const;
  inline void set_id(::google::protobuf::int32 value);

  // required int32 min_level = 2;
  inline bool has_min_level() const;
  inline void clear_min_level();
  static const int kMinLevelFieldNumber = 2;
  inline ::google::protobuf::int32 min_level() const;
  inline void set_min_level(::google::protobuf::int32 value);

  // required int32 pre_stage = 3;
  inline bool has_pre_stage() const;
  inline void clear_pre_stage();
  static const int kPreStageFieldNumber = 3;
  inline ::google::protobuf::int32 pre_stage() const;
  inline void set_pre_stage(::google::protobuf::int32 value);

  // required int32 scene_id = 4;
  inline bool has_scene_id() const;
  inline void clear_scene_id();
  static const int kSceneIdFieldNumber = 4;
  inline ::google::protobuf::int32 scene_id() const;
  inline void set_scene_id(::google::protobuf::int32 value);

  // required int32 refresh_time = 5;
  inline bool has_refresh_time() const;
  inline void clear_refresh_time();
  static const int kRefreshTimeFieldNumber = 5;
  inline ::google::protobuf::int32 refresh_time() const;
  inline void set_refresh_time(::google::protobuf::int32 value);

  // required int32 raid_count = 6;
  inline bool has_raid_count() const;
  inline void clear_raid_count();
  static const int kRaidCountFieldNumber = 6;
  inline ::google::protobuf::int32 raid_count() const;
  inline void set_raid_count(::google::protobuf::int32 value);

  // required int32 rush_count = 7;
  inline bool has_rush_count() const;
  inline void clear_rush_count();
  static const int kRushCountFieldNumber = 7;
  inline ::google::protobuf::int32 rush_count() const;
  inline void set_rush_count(::google::protobuf::int32 value);

  // repeated int32 monster_group_ids = 8;
  inline int monster_group_ids_size() const;
  inline void clear_monster_group_ids();
  static const int kMonsterGroupIdsFieldNumber = 8;
  inline ::google::protobuf::int32 monster_group_ids(int index) const;
  inline void set_monster_group_ids(int index, ::google::protobuf::int32 value);
  inline void add_monster_group_ids(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      monster_group_ids() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_monster_group_ids();

  // repeated int32 boss_group_ids = 9;
  inline int boss_group_ids_size() const;
  inline void clear_boss_group_ids();
  static const int kBossGroupIdsFieldNumber = 9;
  inline ::google::protobuf::int32 boss_group_ids(int index) const;
  inline void set_boss_group_ids(int index, ::google::protobuf::int32 value);
  inline void add_boss_group_ids(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      boss_group_ids() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_boss_group_ids();

  // required int32 guard_id = 10;
  inline bool has_guard_id() const;
  inline void clear_guard_id();
  static const int kGuardIdFieldNumber = 10;
  inline ::google::protobuf::int32 guard_id() const;
  inline void set_guard_id(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:Config.SceneStage)
 private:
  inline void set_has_id();
  inline void clear_has_id();
  inline void set_has_min_level();
  inline void clear_has_min_level();
  inline void set_has_pre_stage();
  inline void clear_has_pre_stage();
  inline void set_has_scene_id();
  inline void clear_has_scene_id();
  inline void set_has_refresh_time();
  inline void clear_has_refresh_time();
  inline void set_has_raid_count();
  inline void clear_has_raid_count();
  inline void set_has_rush_count();
  inline void clear_has_rush_count();
  inline void set_has_guard_id();
  inline void clear_has_guard_id();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::int32 id_;
  ::google::protobuf::int32 min_level_;
  ::google::protobuf::int32 pre_stage_;
  ::google::protobuf::int32 scene_id_;
  ::google::protobuf::int32 refresh_time_;
  ::google::protobuf::int32 raid_count_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > monster_group_ids_;
  ::google::protobuf::int32 rush_count_;
  ::google::protobuf::int32 guard_id_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > boss_group_ids_;

  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(10 + 31) / 32];

  friend void  protobuf_AddDesc_SceneConfig_2eproto();
  friend void protobuf_AssignDesc_SceneConfig_2eproto();
  friend void protobuf_ShutdownFile_SceneConfig_2eproto();

  void InitAsDefaultInstance();
  static SceneStage* default_instance_;
};
// ===================================================================


// ===================================================================

// Scene

// required int32 id = 1;
inline bool Scene::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Scene::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Scene::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Scene::clear_id() {
  id_ = 0;
  clear_has_id();
  SetDirty();
}
inline ::google::protobuf::int32 Scene::id() const {
  return id_;
}
inline void Scene::set_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_id();
  id_ = value;
}

// required string scene_name = 2;
inline bool Scene::has_scene_name() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void Scene::set_has_scene_name() {
  _has_bits_[0] |= 0x00000002u;
}
inline void Scene::clear_has_scene_name() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void Scene::clear_scene_name() {
  if (scene_name_ != &::google::protobuf::internal::kEmptyString) {
    scene_name_->clear();
  }
  clear_has_scene_name();
  SetDirty();
}
inline const ::std::string& Scene::scene_name() const {
  return *scene_name_;
}
inline void Scene::set_scene_name(const ::std::string& value) {
  SetDirty();
  set_has_scene_name();
  if (scene_name_ == &::google::protobuf::internal::kEmptyString) {
    scene_name_ = new ::std::string;
  }
  scene_name_->assign(value);
}
inline void Scene::set_scene_name(const char* value) {
  SetDirty();
  set_has_scene_name();
  if (scene_name_ == &::google::protobuf::internal::kEmptyString) {
    scene_name_ = new ::std::string;
  }
  scene_name_->assign(value);
}
inline void Scene::set_scene_name(const char* value, size_t size) {
  SetDirty();
  set_has_scene_name();
  if (scene_name_ == &::google::protobuf::internal::kEmptyString) {
    scene_name_ = new ::std::string;
  }
  scene_name_->assign(reinterpret_cast<const char*>(value), size);
}
inline ::std::string* Scene::mutable_scene_name() {
  SetDirty();
  set_has_scene_name();
  if (scene_name_ == &::google::protobuf::internal::kEmptyString) {
    scene_name_ = new ::std::string;
  }
  return scene_name_;
}
inline ::std::string* Scene::release_scene_name() {
  SetDirty();
  clear_has_scene_name();
  if (scene_name_ == &::google::protobuf::internal::kEmptyString) {
    return NULL;
  } else {
    ::std::string* temp = scene_name_;
    scene_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
    return temp;
  }
}
inline void Scene::set_allocated_scene_name(::std::string* scene_name) {
  SetDirty();
  if (scene_name_ != &::google::protobuf::internal::kEmptyString) {
    delete scene_name_;
  }
  if (scene_name) {
    set_has_scene_name();
    scene_name_ = scene_name;
  } else {
    clear_has_scene_name();
    scene_name_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  }
}

// repeated .Packet.Position areas = 3;
inline int Scene::areas_size() const {
  return areas_.size();
}
inline void Scene::clear_areas() {
  areas_.Clear();
  SetDirty();
}
inline const ::Packet::Position& Scene::areas(int index) const {
  return areas_.Get(index);
}
inline ::Packet::Position* Scene::mutable_areas(int index) {
  SetDirty();
  return areas_.Mutable(index);
}
inline ::Packet::Position* Scene::add_areas() {
  SetDirty();
  return areas_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
Scene::areas() const {
  return areas_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
Scene::mutable_areas() {
  SetDirty();
  return &areas_;
}

// repeated .Packet.Position spawn_pos = 4;
inline int Scene::spawn_pos_size() const {
  return spawn_pos_.size();
}
inline void Scene::clear_spawn_pos() {
  spawn_pos_.Clear();
  SetDirty();
}
inline const ::Packet::Position& Scene::spawn_pos(int index) const {
  return spawn_pos_.Get(index);
}
inline ::Packet::Position* Scene::mutable_spawn_pos(int index) {
  SetDirty();
  return spawn_pos_.Mutable(index);
}
inline ::Packet::Position* Scene::add_spawn_pos() {
  SetDirty();
  return spawn_pos_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
Scene::spawn_pos() const {
  return spawn_pos_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
Scene::mutable_spawn_pos() {
  SetDirty();
  return &spawn_pos_;
}

// repeated .Config.SceneBattleGroup battle_groups = 5;
inline int Scene::battle_groups_size() const {
  return battle_groups_.size();
}
inline void Scene::clear_battle_groups() {
  battle_groups_.Clear();
  SetDirty();
}
inline const ::Config::SceneBattleGroup& Scene::battle_groups(int index) const {
  return battle_groups_.Get(index);
}
inline ::Config::SceneBattleGroup* Scene::mutable_battle_groups(int index) {
  SetDirty();
  return battle_groups_.Mutable(index);
}
inline ::Config::SceneBattleGroup* Scene::add_battle_groups() {
  SetDirty();
  return battle_groups_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Config::SceneBattleGroup >&
Scene::battle_groups() const {
  return battle_groups_;
}
inline ::google::protobuf::RepeatedPtrField< ::Config::SceneBattleGroup >*
Scene::mutable_battle_groups() {
  SetDirty();
  return &battle_groups_;
}

// -------------------------------------------------------------------

// SceneBattleGroup

// required int32 id = 1;
inline bool SceneBattleGroup::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SceneBattleGroup::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SceneBattleGroup::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SceneBattleGroup::clear_id() {
  id_ = 0;
  clear_has_id();
  SetDirty();
}
inline ::google::protobuf::int32 SceneBattleGroup::id() const {
  return id_;
}
inline void SceneBattleGroup::set_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_id();
  id_ = value;
}

// required int32 index = 2;
inline bool SceneBattleGroup::has_index() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SceneBattleGroup::set_has_index() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SceneBattleGroup::clear_has_index() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SceneBattleGroup::clear_index() {
  index_ = 0;
  clear_has_index();
  SetDirty();
}
inline ::google::protobuf::int32 SceneBattleGroup::index() const {
  return index_;
}
inline void SceneBattleGroup::set_index(::google::protobuf::int32 value) {
  SetDirty();
  set_has_index();
  index_ = value;
}

// repeated .Packet.Position area_pos = 3;
inline int SceneBattleGroup::area_pos_size() const {
  return area_pos_.size();
}
inline void SceneBattleGroup::clear_area_pos() {
  area_pos_.Clear();
  SetDirty();
}
inline const ::Packet::Position& SceneBattleGroup::area_pos(int index) const {
  return area_pos_.Get(index);
}
inline ::Packet::Position* SceneBattleGroup::mutable_area_pos(int index) {
  SetDirty();
  return area_pos_.Mutable(index);
}
inline ::Packet::Position* SceneBattleGroup::add_area_pos() {
  SetDirty();
  return area_pos_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
SceneBattleGroup::area_pos() const {
  return area_pos_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
SceneBattleGroup::mutable_area_pos() {
  SetDirty();
  return &area_pos_;
}

// repeated .Packet.Position player_pos = 4;
inline int SceneBattleGroup::player_pos_size() const {
  return player_pos_.size();
}
inline void SceneBattleGroup::clear_player_pos() {
  player_pos_.Clear();
  SetDirty();
}
inline const ::Packet::Position& SceneBattleGroup::player_pos(int index) const {
  return player_pos_.Get(index);
}
inline ::Packet::Position* SceneBattleGroup::mutable_player_pos(int index) {
  SetDirty();
  return player_pos_.Mutable(index);
}
inline ::Packet::Position* SceneBattleGroup::add_player_pos() {
  SetDirty();
  return player_pos_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
SceneBattleGroup::player_pos() const {
  return player_pos_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
SceneBattleGroup::mutable_player_pos() {
  SetDirty();
  return &player_pos_;
}

// repeated .Packet.Position enemy_pos = 5;
inline int SceneBattleGroup::enemy_pos_size() const {
  return enemy_pos_.size();
}
inline void SceneBattleGroup::clear_enemy_pos() {
  enemy_pos_.Clear();
  SetDirty();
}
inline const ::Packet::Position& SceneBattleGroup::enemy_pos(int index) const {
  return enemy_pos_.Get(index);
}
inline ::Packet::Position* SceneBattleGroup::mutable_enemy_pos(int index) {
  SetDirty();
  return enemy_pos_.Mutable(index);
}
inline ::Packet::Position* SceneBattleGroup::add_enemy_pos() {
  SetDirty();
  return enemy_pos_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::Packet::Position >&
SceneBattleGroup::enemy_pos() const {
  return enemy_pos_;
}
inline ::google::protobuf::RepeatedPtrField< ::Packet::Position >*
SceneBattleGroup::mutable_enemy_pos() {
  SetDirty();
  return &enemy_pos_;
}

// -------------------------------------------------------------------

// SceneStage

// required int32 id = 1;
inline bool SceneStage::has_id() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void SceneStage::set_has_id() {
  _has_bits_[0] |= 0x00000001u;
}
inline void SceneStage::clear_has_id() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void SceneStage::clear_id() {
  id_ = 0;
  clear_has_id();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::id() const {
  return id_;
}
inline void SceneStage::set_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_id();
  id_ = value;
}

// required int32 min_level = 2;
inline bool SceneStage::has_min_level() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void SceneStage::set_has_min_level() {
  _has_bits_[0] |= 0x00000002u;
}
inline void SceneStage::clear_has_min_level() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void SceneStage::clear_min_level() {
  min_level_ = 0;
  clear_has_min_level();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::min_level() const {
  return min_level_;
}
inline void SceneStage::set_min_level(::google::protobuf::int32 value) {
  SetDirty();
  set_has_min_level();
  min_level_ = value;
}

// required int32 pre_stage = 3;
inline bool SceneStage::has_pre_stage() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void SceneStage::set_has_pre_stage() {
  _has_bits_[0] |= 0x00000004u;
}
inline void SceneStage::clear_has_pre_stage() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void SceneStage::clear_pre_stage() {
  pre_stage_ = 0;
  clear_has_pre_stage();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::pre_stage() const {
  return pre_stage_;
}
inline void SceneStage::set_pre_stage(::google::protobuf::int32 value) {
  SetDirty();
  set_has_pre_stage();
  pre_stage_ = value;
}

// required int32 scene_id = 4;
inline bool SceneStage::has_scene_id() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void SceneStage::set_has_scene_id() {
  _has_bits_[0] |= 0x00000008u;
}
inline void SceneStage::clear_has_scene_id() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void SceneStage::clear_scene_id() {
  scene_id_ = 0;
  clear_has_scene_id();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::scene_id() const {
  return scene_id_;
}
inline void SceneStage::set_scene_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_scene_id();
  scene_id_ = value;
}

// required int32 refresh_time = 5;
inline bool SceneStage::has_refresh_time() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void SceneStage::set_has_refresh_time() {
  _has_bits_[0] |= 0x00000010u;
}
inline void SceneStage::clear_has_refresh_time() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void SceneStage::clear_refresh_time() {
  refresh_time_ = 0;
  clear_has_refresh_time();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::refresh_time() const {
  return refresh_time_;
}
inline void SceneStage::set_refresh_time(::google::protobuf::int32 value) {
  SetDirty();
  set_has_refresh_time();
  refresh_time_ = value;
}

// required int32 raid_count = 6;
inline bool SceneStage::has_raid_count() const {
  return (_has_bits_[0] & 0x00000020u) != 0;
}
inline void SceneStage::set_has_raid_count() {
  _has_bits_[0] |= 0x00000020u;
}
inline void SceneStage::clear_has_raid_count() {
  _has_bits_[0] &= ~0x00000020u;
}
inline void SceneStage::clear_raid_count() {
  raid_count_ = 0;
  clear_has_raid_count();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::raid_count() const {
  return raid_count_;
}
inline void SceneStage::set_raid_count(::google::protobuf::int32 value) {
  SetDirty();
  set_has_raid_count();
  raid_count_ = value;
}

// required int32 rush_count = 7;
inline bool SceneStage::has_rush_count() const {
  return (_has_bits_[0] & 0x00000040u) != 0;
}
inline void SceneStage::set_has_rush_count() {
  _has_bits_[0] |= 0x00000040u;
}
inline void SceneStage::clear_has_rush_count() {
  _has_bits_[0] &= ~0x00000040u;
}
inline void SceneStage::clear_rush_count() {
  rush_count_ = 0;
  clear_has_rush_count();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::rush_count() const {
  return rush_count_;
}
inline void SceneStage::set_rush_count(::google::protobuf::int32 value) {
  SetDirty();
  set_has_rush_count();
  rush_count_ = value;
}

// repeated int32 monster_group_ids = 8;
inline int SceneStage::monster_group_ids_size() const {
  return monster_group_ids_.size();
}
inline void SceneStage::clear_monster_group_ids() {
  monster_group_ids_.Clear();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::monster_group_ids(int index) const {
  return monster_group_ids_.Get(index);
}
inline void SceneStage::set_monster_group_ids(int index, ::google::protobuf::int32 value) {
  SetDirty();
  monster_group_ids_.Set(index, value);
}
inline void SceneStage::add_monster_group_ids(::google::protobuf::int32 value) {
  SetDirty();
  monster_group_ids_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
SceneStage::monster_group_ids() const {
  return monster_group_ids_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
SceneStage::mutable_monster_group_ids() {
  SetDirty();
  return &monster_group_ids_;
}

// repeated int32 boss_group_ids = 9;
inline int SceneStage::boss_group_ids_size() const {
  return boss_group_ids_.size();
}
inline void SceneStage::clear_boss_group_ids() {
  boss_group_ids_.Clear();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::boss_group_ids(int index) const {
  return boss_group_ids_.Get(index);
}
inline void SceneStage::set_boss_group_ids(int index, ::google::protobuf::int32 value) {
  SetDirty();
  boss_group_ids_.Set(index, value);
}
inline void SceneStage::add_boss_group_ids(::google::protobuf::int32 value) {
  SetDirty();
  boss_group_ids_.Add(value);
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
SceneStage::boss_group_ids() const {
  return boss_group_ids_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
SceneStage::mutable_boss_group_ids() {
  SetDirty();
  return &boss_group_ids_;
}

// required int32 guard_id = 10;
inline bool SceneStage::has_guard_id() const {
  return (_has_bits_[0] & 0x00000200u) != 0;
}
inline void SceneStage::set_has_guard_id() {
  _has_bits_[0] |= 0x00000200u;
}
inline void SceneStage::clear_has_guard_id() {
  _has_bits_[0] &= ~0x00000200u;
}
inline void SceneStage::clear_guard_id() {
  guard_id_ = 0;
  clear_has_guard_id();
  SetDirty();
}
inline ::google::protobuf::int32 SceneStage::guard_id() const {
  return guard_id_;
}
inline void SceneStage::set_guard_id(::google::protobuf::int32 value) {
  SetDirty();
  set_has_guard_id();
  guard_id_ = value;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace Config

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_SceneConfig_2eproto__INCLUDED
