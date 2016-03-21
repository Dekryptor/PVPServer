// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: ProtoGateServer.proto

#ifndef PROTOBUF_ProtoGateServer_2eproto__INCLUDED
#define PROTOBUF_ProtoGateServer_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2006000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
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

namespace ProtoGateServer {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_ProtoGateServer_2eproto();
void protobuf_AssignDesc_ProtoGateServer_2eproto();
void protobuf_ShutdownFile_ProtoGateServer_2eproto();

class Heart;
class GameServerList;
class GameServerList_GameServer;
class GameServerList_GameServer_Game;

enum VERSION_NUMBER {
  VERSION = 65536
};
bool VERSION_NUMBER_IsValid(int value);
const VERSION_NUMBER VERSION_NUMBER_MIN = VERSION;
const VERSION_NUMBER VERSION_NUMBER_MAX = VERSION;
const int VERSION_NUMBER_ARRAYSIZE = VERSION_NUMBER_MAX + 1;

const ::google::protobuf::EnumDescriptor* VERSION_NUMBER_descriptor();
inline const ::std::string& VERSION_NUMBER_Name(VERSION_NUMBER value) {
  return ::google::protobuf::internal::NameOfEnum(
    VERSION_NUMBER_descriptor(), value);
}
inline bool VERSION_NUMBER_Parse(
    const ::std::string& name, VERSION_NUMBER* value) {
  return ::google::protobuf::internal::ParseNamedEnum<VERSION_NUMBER>(
    VERSION_NUMBER_descriptor(), name, value);
}
enum RESPONSE_MSG {
  HEART = 0,
  GAME_SERVER_LIST = 1
};
bool RESPONSE_MSG_IsValid(int value);
const RESPONSE_MSG RESPONSE_MSG_MIN = HEART;
const RESPONSE_MSG RESPONSE_MSG_MAX = GAME_SERVER_LIST;
const int RESPONSE_MSG_ARRAYSIZE = RESPONSE_MSG_MAX + 1;

const ::google::protobuf::EnumDescriptor* RESPONSE_MSG_descriptor();
inline const ::std::string& RESPONSE_MSG_Name(RESPONSE_MSG value) {
  return ::google::protobuf::internal::NameOfEnum(
    RESPONSE_MSG_descriptor(), value);
}
inline bool RESPONSE_MSG_Parse(
    const ::std::string& name, RESPONSE_MSG* value) {
  return ::google::protobuf::internal::ParseNamedEnum<RESPONSE_MSG>(
    RESPONSE_MSG_descriptor(), name, value);
}
// ===================================================================

class Heart : public ::google::protobuf::Message {
 public:
  Heart();
  virtual ~Heart();

  Heart(const Heart& from);

  inline Heart& operator=(const Heart& from) {
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
  static const Heart& default_instance();

  void Swap(Heart* other);

  // implements Message ----------------------------------------------

  Heart* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Heart& from);
  void MergeFrom(const Heart& from);
  void Clear();
  bool IsInitialized() const;

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

  // required int32 timestamp = 1;
  inline bool has_timestamp() const;
  inline void clear_timestamp();
  static const int kTimestampFieldNumber = 1;
  inline ::google::protobuf::int32 timestamp() const;
  inline void set_timestamp(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:ProtoGateServer.Heart)
 private:
  inline void set_has_timestamp();
  inline void clear_has_timestamp();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::int32 timestamp_;
  friend void  protobuf_AddDesc_ProtoGateServer_2eproto();
  friend void protobuf_AssignDesc_ProtoGateServer_2eproto();
  friend void protobuf_ShutdownFile_ProtoGateServer_2eproto();

  void InitAsDefaultInstance();
  static Heart* default_instance_;
};
// -------------------------------------------------------------------

class GameServerList_GameServer_Game : public ::google::protobuf::Message {
 public:
  GameServerList_GameServer_Game();
  virtual ~GameServerList_GameServer_Game();

  GameServerList_GameServer_Game(const GameServerList_GameServer_Game& from);

  inline GameServerList_GameServer_Game& operator=(const GameServerList_GameServer_Game& from) {
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
  static const GameServerList_GameServer_Game& default_instance();

  void Swap(GameServerList_GameServer_Game* other);

  // implements Message ----------------------------------------------

  GameServerList_GameServer_Game* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GameServerList_GameServer_Game& from);
  void MergeFrom(const GameServerList_GameServer_Game& from);
  void Clear();
  bool IsInitialized() const;

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

  // required bool private = 1;
  inline bool has_private_() const;
  inline void clear_private_();
  static const int kPrivateFieldNumber = 1;
  inline bool private_() const;
  inline void set_private_(bool value);

  // required int32 gameid = 2;
  inline bool has_gameid() const;
  inline void clear_gameid();
  static const int kGameidFieldNumber = 2;
  inline ::google::protobuf::int32 gameid() const;
  inline void set_gameid(::google::protobuf::int32 value);

  // required int32 mode = 3;
  inline bool has_mode() const;
  inline void clear_mode();
  static const int kModeFieldNumber = 3;
  inline ::google::protobuf::int32 mode() const;
  inline void set_mode(::google::protobuf::int32 value);

  // required int32 map = 4;
  inline bool has_map() const;
  inline void clear_map();
  static const int kMapFieldNumber = 4;
  inline ::google::protobuf::int32 map() const;
  inline void set_map(::google::protobuf::int32 value);

  // required int32 maxPlayers = 5;
  inline bool has_maxplayers() const;
  inline void clear_maxplayers();
  static const int kMaxPlayersFieldNumber = 5;
  inline ::google::protobuf::int32 maxplayers() const;
  inline void set_maxplayers(::google::protobuf::int32 value);

  // repeated int32 playes = 6;
  inline int playes_size() const;
  inline void clear_playes();
  static const int kPlayesFieldNumber = 6;
  inline ::google::protobuf::int32 playes(int index) const;
  inline void set_playes(int index, ::google::protobuf::int32 value);
  inline void add_playes(::google::protobuf::int32 value);
  inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
      playes() const;
  inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
      mutable_playes();

  // @@protoc_insertion_point(class_scope:ProtoGateServer.GameServerList.GameServer.Game)
 private:
  inline void set_has_private_();
  inline void clear_has_private_();
  inline void set_has_gameid();
  inline void clear_has_gameid();
  inline void set_has_mode();
  inline void clear_has_mode();
  inline void set_has_map();
  inline void clear_has_map();
  inline void set_has_maxplayers();
  inline void clear_has_maxplayers();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  bool private__;
  ::google::protobuf::int32 gameid_;
  ::google::protobuf::int32 mode_;
  ::google::protobuf::int32 map_;
  ::google::protobuf::RepeatedField< ::google::protobuf::int32 > playes_;
  ::google::protobuf::int32 maxplayers_;
  friend void  protobuf_AddDesc_ProtoGateServer_2eproto();
  friend void protobuf_AssignDesc_ProtoGateServer_2eproto();
  friend void protobuf_ShutdownFile_ProtoGateServer_2eproto();

  void InitAsDefaultInstance();
  static GameServerList_GameServer_Game* default_instance_;
};
// -------------------------------------------------------------------

class GameServerList_GameServer : public ::google::protobuf::Message {
 public:
  GameServerList_GameServer();
  virtual ~GameServerList_GameServer();

  GameServerList_GameServer(const GameServerList_GameServer& from);

  inline GameServerList_GameServer& operator=(const GameServerList_GameServer& from) {
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
  static const GameServerList_GameServer& default_instance();

  void Swap(GameServerList_GameServer* other);

  // implements Message ----------------------------------------------

  GameServerList_GameServer* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GameServerList_GameServer& from);
  void MergeFrom(const GameServerList_GameServer& from);
  void Clear();
  bool IsInitialized() const;

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

  typedef GameServerList_GameServer_Game Game;

  // accessors -------------------------------------------------------

  // required string ip = 1;
  inline bool has_ip() const;
  inline void clear_ip();
  static const int kIpFieldNumber = 1;
  inline const ::std::string& ip() const;
  inline void set_ip(const ::std::string& value);
  inline void set_ip(const char* value);
  inline void set_ip(const char* value, size_t size);
  inline ::std::string* mutable_ip();
  inline ::std::string* release_ip();
  inline void set_allocated_ip(::std::string* ip);

  // required int32 port = 2;
  inline bool has_port() const;
  inline void clear_port();
  static const int kPortFieldNumber = 2;
  inline ::google::protobuf::int32 port() const;
  inline void set_port(::google::protobuf::int32 value);

  // repeated .ProtoGateServer.GameServerList.GameServer.Game games = 3;
  inline int games_size() const;
  inline void clear_games();
  static const int kGamesFieldNumber = 3;
  inline const ::ProtoGateServer::GameServerList_GameServer_Game& games(int index) const;
  inline ::ProtoGateServer::GameServerList_GameServer_Game* mutable_games(int index);
  inline ::ProtoGateServer::GameServerList_GameServer_Game* add_games();
  inline const ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer_Game >&
      games() const;
  inline ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer_Game >*
      mutable_games();

  // @@protoc_insertion_point(class_scope:ProtoGateServer.GameServerList.GameServer)
 private:
  inline void set_has_ip();
  inline void clear_has_ip();
  inline void set_has_port();
  inline void clear_has_port();

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::std::string* ip_;
  ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer_Game > games_;
  ::google::protobuf::int32 port_;
  friend void  protobuf_AddDesc_ProtoGateServer_2eproto();
  friend void protobuf_AssignDesc_ProtoGateServer_2eproto();
  friend void protobuf_ShutdownFile_ProtoGateServer_2eproto();

  void InitAsDefaultInstance();
  static GameServerList_GameServer* default_instance_;
};
// -------------------------------------------------------------------

class GameServerList : public ::google::protobuf::Message {
 public:
  GameServerList();
  virtual ~GameServerList();

  GameServerList(const GameServerList& from);

  inline GameServerList& operator=(const GameServerList& from) {
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
  static const GameServerList& default_instance();

  void Swap(GameServerList* other);

  // implements Message ----------------------------------------------

  GameServerList* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const GameServerList& from);
  void MergeFrom(const GameServerList& from);
  void Clear();
  bool IsInitialized() const;

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

  typedef GameServerList_GameServer GameServer;

  // accessors -------------------------------------------------------

  // repeated .ProtoGateServer.GameServerList.GameServer servers = 1;
  inline int servers_size() const;
  inline void clear_servers();
  static const int kServersFieldNumber = 1;
  inline const ::ProtoGateServer::GameServerList_GameServer& servers(int index) const;
  inline ::ProtoGateServer::GameServerList_GameServer* mutable_servers(int index);
  inline ::ProtoGateServer::GameServerList_GameServer* add_servers();
  inline const ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer >&
      servers() const;
  inline ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer >*
      mutable_servers();

  // @@protoc_insertion_point(class_scope:ProtoGateServer.GameServerList)
 private:

  ::google::protobuf::UnknownFieldSet _unknown_fields_;

  ::google::protobuf::uint32 _has_bits_[1];
  mutable int _cached_size_;
  ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer > servers_;
  friend void  protobuf_AddDesc_ProtoGateServer_2eproto();
  friend void protobuf_AssignDesc_ProtoGateServer_2eproto();
  friend void protobuf_ShutdownFile_ProtoGateServer_2eproto();

  void InitAsDefaultInstance();
  static GameServerList* default_instance_;
};
// ===================================================================


// ===================================================================

// Heart

// required int32 timestamp = 1;
inline bool Heart::has_timestamp() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void Heart::set_has_timestamp() {
  _has_bits_[0] |= 0x00000001u;
}
inline void Heart::clear_has_timestamp() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void Heart::clear_timestamp() {
  timestamp_ = 0;
  clear_has_timestamp();
}
inline ::google::protobuf::int32 Heart::timestamp() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.Heart.timestamp)
  return timestamp_;
}
inline void Heart::set_timestamp(::google::protobuf::int32 value) {
  set_has_timestamp();
  timestamp_ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.Heart.timestamp)
}

// -------------------------------------------------------------------

// GameServerList_GameServer_Game

// required bool private = 1;
inline bool GameServerList_GameServer_Game::has_private_() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void GameServerList_GameServer_Game::set_has_private_() {
  _has_bits_[0] |= 0x00000001u;
}
inline void GameServerList_GameServer_Game::clear_has_private_() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void GameServerList_GameServer_Game::clear_private_() {
  private__ = false;
  clear_has_private_();
}
inline bool GameServerList_GameServer_Game::private_() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.Game.private)
  return private__;
}
inline void GameServerList_GameServer_Game::set_private_(bool value) {
  set_has_private_();
  private__ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.Game.private)
}

// required int32 gameid = 2;
inline bool GameServerList_GameServer_Game::has_gameid() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void GameServerList_GameServer_Game::set_has_gameid() {
  _has_bits_[0] |= 0x00000002u;
}
inline void GameServerList_GameServer_Game::clear_has_gameid() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void GameServerList_GameServer_Game::clear_gameid() {
  gameid_ = 0;
  clear_has_gameid();
}
inline ::google::protobuf::int32 GameServerList_GameServer_Game::gameid() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.Game.gameid)
  return gameid_;
}
inline void GameServerList_GameServer_Game::set_gameid(::google::protobuf::int32 value) {
  set_has_gameid();
  gameid_ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.Game.gameid)
}

// required int32 mode = 3;
inline bool GameServerList_GameServer_Game::has_mode() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void GameServerList_GameServer_Game::set_has_mode() {
  _has_bits_[0] |= 0x00000004u;
}
inline void GameServerList_GameServer_Game::clear_has_mode() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void GameServerList_GameServer_Game::clear_mode() {
  mode_ = 0;
  clear_has_mode();
}
inline ::google::protobuf::int32 GameServerList_GameServer_Game::mode() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.Game.mode)
  return mode_;
}
inline void GameServerList_GameServer_Game::set_mode(::google::protobuf::int32 value) {
  set_has_mode();
  mode_ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.Game.mode)
}

// required int32 map = 4;
inline bool GameServerList_GameServer_Game::has_map() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void GameServerList_GameServer_Game::set_has_map() {
  _has_bits_[0] |= 0x00000008u;
}
inline void GameServerList_GameServer_Game::clear_has_map() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void GameServerList_GameServer_Game::clear_map() {
  map_ = 0;
  clear_has_map();
}
inline ::google::protobuf::int32 GameServerList_GameServer_Game::map() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.Game.map)
  return map_;
}
inline void GameServerList_GameServer_Game::set_map(::google::protobuf::int32 value) {
  set_has_map();
  map_ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.Game.map)
}

// required int32 maxPlayers = 5;
inline bool GameServerList_GameServer_Game::has_maxplayers() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void GameServerList_GameServer_Game::set_has_maxplayers() {
  _has_bits_[0] |= 0x00000010u;
}
inline void GameServerList_GameServer_Game::clear_has_maxplayers() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void GameServerList_GameServer_Game::clear_maxplayers() {
  maxplayers_ = 0;
  clear_has_maxplayers();
}
inline ::google::protobuf::int32 GameServerList_GameServer_Game::maxplayers() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.Game.maxPlayers)
  return maxplayers_;
}
inline void GameServerList_GameServer_Game::set_maxplayers(::google::protobuf::int32 value) {
  set_has_maxplayers();
  maxplayers_ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.Game.maxPlayers)
}

// repeated int32 playes = 6;
inline int GameServerList_GameServer_Game::playes_size() const {
  return playes_.size();
}
inline void GameServerList_GameServer_Game::clear_playes() {
  playes_.Clear();
}
inline ::google::protobuf::int32 GameServerList_GameServer_Game::playes(int index) const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.Game.playes)
  return playes_.Get(index);
}
inline void GameServerList_GameServer_Game::set_playes(int index, ::google::protobuf::int32 value) {
  playes_.Set(index, value);
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.Game.playes)
}
inline void GameServerList_GameServer_Game::add_playes(::google::protobuf::int32 value) {
  playes_.Add(value);
  // @@protoc_insertion_point(field_add:ProtoGateServer.GameServerList.GameServer.Game.playes)
}
inline const ::google::protobuf::RepeatedField< ::google::protobuf::int32 >&
GameServerList_GameServer_Game::playes() const {
  // @@protoc_insertion_point(field_list:ProtoGateServer.GameServerList.GameServer.Game.playes)
  return playes_;
}
inline ::google::protobuf::RepeatedField< ::google::protobuf::int32 >*
GameServerList_GameServer_Game::mutable_playes() {
  // @@protoc_insertion_point(field_mutable_list:ProtoGateServer.GameServerList.GameServer.Game.playes)
  return &playes_;
}

// -------------------------------------------------------------------

// GameServerList_GameServer

// required string ip = 1;
inline bool GameServerList_GameServer::has_ip() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void GameServerList_GameServer::set_has_ip() {
  _has_bits_[0] |= 0x00000001u;
}
inline void GameServerList_GameServer::clear_has_ip() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void GameServerList_GameServer::clear_ip() {
  if (ip_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_->clear();
  }
  clear_has_ip();
}
inline const ::std::string& GameServerList_GameServer::ip() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.ip)
  return *ip_;
}
inline void GameServerList_GameServer::set_ip(const ::std::string& value) {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  ip_->assign(value);
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.ip)
}
inline void GameServerList_GameServer::set_ip(const char* value) {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  ip_->assign(value);
  // @@protoc_insertion_point(field_set_char:ProtoGateServer.GameServerList.GameServer.ip)
}
inline void GameServerList_GameServer::set_ip(const char* value, size_t size) {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  ip_->assign(reinterpret_cast<const char*>(value), size);
  // @@protoc_insertion_point(field_set_pointer:ProtoGateServer.GameServerList.GameServer.ip)
}
inline ::std::string* GameServerList_GameServer::mutable_ip() {
  set_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    ip_ = new ::std::string;
  }
  // @@protoc_insertion_point(field_mutable:ProtoGateServer.GameServerList.GameServer.ip)
  return ip_;
}
inline ::std::string* GameServerList_GameServer::release_ip() {
  clear_has_ip();
  if (ip_ == &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    return NULL;
  } else {
    ::std::string* temp = ip_;
    ip_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
    return temp;
  }
}
inline void GameServerList_GameServer::set_allocated_ip(::std::string* ip) {
  if (ip_ != &::google::protobuf::internal::GetEmptyStringAlreadyInited()) {
    delete ip_;
  }
  if (ip) {
    set_has_ip();
    ip_ = ip;
  } else {
    clear_has_ip();
    ip_ = const_cast< ::std::string*>(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  }
  // @@protoc_insertion_point(field_set_allocated:ProtoGateServer.GameServerList.GameServer.ip)
}

// required int32 port = 2;
inline bool GameServerList_GameServer::has_port() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void GameServerList_GameServer::set_has_port() {
  _has_bits_[0] |= 0x00000002u;
}
inline void GameServerList_GameServer::clear_has_port() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void GameServerList_GameServer::clear_port() {
  port_ = 0;
  clear_has_port();
}
inline ::google::protobuf::int32 GameServerList_GameServer::port() const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.port)
  return port_;
}
inline void GameServerList_GameServer::set_port(::google::protobuf::int32 value) {
  set_has_port();
  port_ = value;
  // @@protoc_insertion_point(field_set:ProtoGateServer.GameServerList.GameServer.port)
}

// repeated .ProtoGateServer.GameServerList.GameServer.Game games = 3;
inline int GameServerList_GameServer::games_size() const {
  return games_.size();
}
inline void GameServerList_GameServer::clear_games() {
  games_.Clear();
}
inline const ::ProtoGateServer::GameServerList_GameServer_Game& GameServerList_GameServer::games(int index) const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.GameServer.games)
  return games_.Get(index);
}
inline ::ProtoGateServer::GameServerList_GameServer_Game* GameServerList_GameServer::mutable_games(int index) {
  // @@protoc_insertion_point(field_mutable:ProtoGateServer.GameServerList.GameServer.games)
  return games_.Mutable(index);
}
inline ::ProtoGateServer::GameServerList_GameServer_Game* GameServerList_GameServer::add_games() {
  // @@protoc_insertion_point(field_add:ProtoGateServer.GameServerList.GameServer.games)
  return games_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer_Game >&
GameServerList_GameServer::games() const {
  // @@protoc_insertion_point(field_list:ProtoGateServer.GameServerList.GameServer.games)
  return games_;
}
inline ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer_Game >*
GameServerList_GameServer::mutable_games() {
  // @@protoc_insertion_point(field_mutable_list:ProtoGateServer.GameServerList.GameServer.games)
  return &games_;
}

// -------------------------------------------------------------------

// GameServerList

// repeated .ProtoGateServer.GameServerList.GameServer servers = 1;
inline int GameServerList::servers_size() const {
  return servers_.size();
}
inline void GameServerList::clear_servers() {
  servers_.Clear();
}
inline const ::ProtoGateServer::GameServerList_GameServer& GameServerList::servers(int index) const {
  // @@protoc_insertion_point(field_get:ProtoGateServer.GameServerList.servers)
  return servers_.Get(index);
}
inline ::ProtoGateServer::GameServerList_GameServer* GameServerList::mutable_servers(int index) {
  // @@protoc_insertion_point(field_mutable:ProtoGateServer.GameServerList.servers)
  return servers_.Mutable(index);
}
inline ::ProtoGateServer::GameServerList_GameServer* GameServerList::add_servers() {
  // @@protoc_insertion_point(field_add:ProtoGateServer.GameServerList.servers)
  return servers_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer >&
GameServerList::servers() const {
  // @@protoc_insertion_point(field_list:ProtoGateServer.GameServerList.servers)
  return servers_;
}
inline ::google::protobuf::RepeatedPtrField< ::ProtoGateServer::GameServerList_GameServer >*
GameServerList::mutable_servers() {
  // @@protoc_insertion_point(field_mutable_list:ProtoGateServer.GameServerList.servers)
  return &servers_;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace ProtoGateServer

#ifndef SWIG
namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::ProtoGateServer::VERSION_NUMBER> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ProtoGateServer::VERSION_NUMBER>() {
  return ::ProtoGateServer::VERSION_NUMBER_descriptor();
}
template <> struct is_proto_enum< ::ProtoGateServer::RESPONSE_MSG> : ::google::protobuf::internal::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::ProtoGateServer::RESPONSE_MSG>() {
  return ::ProtoGateServer::RESPONSE_MSG_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_ProtoGateServer_2eproto__INCLUDED
