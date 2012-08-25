// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: system.proto

#ifndef PROTOBUF_system_2eproto__INCLUDED
#define PROTOBUF_system_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "core.pb.h"
// @@protoc_insertion_point(includes)

namespace rsctrl {
namespace system {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_system_2eproto();
void protobuf_AssignDesc_system_2eproto();
void protobuf_ShutdownFile_system_2eproto();

class RequestSystemStatus;
class ResponseSystemStatus;

enum ResponseSystemStatus_NetCode {
  ResponseSystemStatus_NetCode_BAD_UNKNOWN = 0,
  ResponseSystemStatus_NetCode_BAD_OFFLINE = 1,
  ResponseSystemStatus_NetCode_BAD_NATSYM = 2,
  ResponseSystemStatus_NetCode_BAD_NODHT_NAT = 3,
  ResponseSystemStatus_NetCode_WARNING_RESTART = 4,
  ResponseSystemStatus_NetCode_WARNING_NATTED = 5,
  ResponseSystemStatus_NetCode_WARNING_NODHT = 6,
  ResponseSystemStatus_NetCode_GOOD = 7,
  ResponseSystemStatus_NetCode_ADV_FORWARD = 8
};
bool ResponseSystemStatus_NetCode_IsValid(int value);
const ResponseSystemStatus_NetCode ResponseSystemStatus_NetCode_NetCode_MIN = ResponseSystemStatus_NetCode_BAD_UNKNOWN;
const ResponseSystemStatus_NetCode ResponseSystemStatus_NetCode_NetCode_MAX = ResponseSystemStatus_NetCode_ADV_FORWARD;
const int ResponseSystemStatus_NetCode_NetCode_ARRAYSIZE = ResponseSystemStatus_NetCode_NetCode_MAX + 1;

const ::google::protobuf::EnumDescriptor* ResponseSystemStatus_NetCode_descriptor();
inline const ::std::string& ResponseSystemStatus_NetCode_Name(ResponseSystemStatus_NetCode value) {
  return ::google::protobuf::internal::NameOfEnum(
    ResponseSystemStatus_NetCode_descriptor(), value);
}
inline bool ResponseSystemStatus_NetCode_Parse(
    const ::std::string& name, ResponseSystemStatus_NetCode* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ResponseSystemStatus_NetCode>(
    ResponseSystemStatus_NetCode_descriptor(), name, value);
}
enum RequestMsgIds {
  MsgId_RequestSystemStatus = 1
};
bool RequestMsgIds_IsValid(int value);
const RequestMsgIds RequestMsgIds_MIN = MsgId_RequestSystemStatus;
const RequestMsgIds RequestMsgIds_MAX = MsgId_RequestSystemStatus;
const int RequestMsgIds_ARRAYSIZE = RequestMsgIds_MAX + 1;

const ::google::protobuf::EnumDescriptor* RequestMsgIds_descriptor();
inline const ::std::string& RequestMsgIds_Name(RequestMsgIds value) {
  return ::google::protobuf::internal::NameOfEnum(
    RequestMsgIds_descriptor(), value);
}
inline bool RequestMsgIds_Parse(
    const ::std::string& name, RequestMsgIds* value) {
  return ::google::protobuf::internal::ParseNamedEnum<RequestMsgIds>(
    RequestMsgIds_descriptor(), name, value);
}
enum ResponseMsgIds {
  MsgId_ResponseSystemStatus = 1
};
bool ResponseMsgIds_IsValid(int value);
const ResponseMsgIds ResponseMsgIds_MIN = MsgId_ResponseSystemStatus;
const ResponseMsgIds ResponseMsgIds_MAX = MsgId_ResponseSystemStatus;
const int ResponseMsgIds_ARRAYSIZE = ResponseMsgIds_MAX + 1;

const ::google::protobuf::EnumDescriptor* ResponseMsgIds_descriptor();
inline const ::std::string& ResponseMsgIds_Name(ResponseMsgIds value) {
  return ::google::protobuf::internal::NameOfEnum(
    ResponseMsgIds_descriptor(), value);
}
inline bool ResponseMsgIds_Parse(
    const ::std::string& name, ResponseMsgIds* value) {
  return ::google::protobuf::internal::ParseNamedEnum<ResponseMsgIds>(
    ResponseMsgIds_descriptor(), name, value);
}
// ===================================================================

class RequestSystemStatus : public ::google::protobuf::Message {
 public:
  RequestSystemStatus();
  virtual ~RequestSystemStatus();
  
  RequestSystemStatus(const RequestSystemStatus& from);
  
  inline RequestSystemStatus& operator=(const RequestSystemStatus& from) {
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
  static const RequestSystemStatus& default_instance();
  
  void Swap(RequestSystemStatus* other);
  
  // implements Message ----------------------------------------------
  
  RequestSystemStatus* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const RequestSystemStatus& from);
  void MergeFrom(const RequestSystemStatus& from);
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
  
  // @@protoc_insertion_point(class_scope:rsctrl.system.RequestSystemStatus)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[1];
  
  friend void  protobuf_AddDesc_system_2eproto();
  friend void protobuf_AssignDesc_system_2eproto();
  friend void protobuf_ShutdownFile_system_2eproto();
  
  void InitAsDefaultInstance();
  static RequestSystemStatus* default_instance_;
};
// -------------------------------------------------------------------

class ResponseSystemStatus : public ::google::protobuf::Message {
 public:
  ResponseSystemStatus();
  virtual ~ResponseSystemStatus();
  
  ResponseSystemStatus(const ResponseSystemStatus& from);
  
  inline ResponseSystemStatus& operator=(const ResponseSystemStatus& from) {
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
  static const ResponseSystemStatus& default_instance();
  
  void Swap(ResponseSystemStatus* other);
  
  // implements Message ----------------------------------------------
  
  ResponseSystemStatus* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const ResponseSystemStatus& from);
  void MergeFrom(const ResponseSystemStatus& from);
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
  
  typedef ResponseSystemStatus_NetCode NetCode;
  static const NetCode BAD_UNKNOWN = ResponseSystemStatus_NetCode_BAD_UNKNOWN;
  static const NetCode BAD_OFFLINE = ResponseSystemStatus_NetCode_BAD_OFFLINE;
  static const NetCode BAD_NATSYM = ResponseSystemStatus_NetCode_BAD_NATSYM;
  static const NetCode BAD_NODHT_NAT = ResponseSystemStatus_NetCode_BAD_NODHT_NAT;
  static const NetCode WARNING_RESTART = ResponseSystemStatus_NetCode_WARNING_RESTART;
  static const NetCode WARNING_NATTED = ResponseSystemStatus_NetCode_WARNING_NATTED;
  static const NetCode WARNING_NODHT = ResponseSystemStatus_NetCode_WARNING_NODHT;
  static const NetCode GOOD = ResponseSystemStatus_NetCode_GOOD;
  static const NetCode ADV_FORWARD = ResponseSystemStatus_NetCode_ADV_FORWARD;
  static inline bool NetCode_IsValid(int value) {
    return ResponseSystemStatus_NetCode_IsValid(value);
  }
  static const NetCode NetCode_MIN =
    ResponseSystemStatus_NetCode_NetCode_MIN;
  static const NetCode NetCode_MAX =
    ResponseSystemStatus_NetCode_NetCode_MAX;
  static const int NetCode_ARRAYSIZE =
    ResponseSystemStatus_NetCode_NetCode_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  NetCode_descriptor() {
    return ResponseSystemStatus_NetCode_descriptor();
  }
  static inline const ::std::string& NetCode_Name(NetCode value) {
    return ResponseSystemStatus_NetCode_Name(value);
  }
  static inline bool NetCode_Parse(const ::std::string& name,
      NetCode* value) {
    return ResponseSystemStatus_NetCode_Parse(name, value);
  }
  
  // accessors -------------------------------------------------------
  
  // required .rsctrl.core.Status status = 1;
  inline bool has_status() const;
  inline void clear_status();
  static const int kStatusFieldNumber = 1;
  inline const ::rsctrl::core::Status& status() const;
  inline ::rsctrl::core::Status* mutable_status();
  inline ::rsctrl::core::Status* release_status();
  
  // required uint32 no_peers = 2;
  inline bool has_no_peers() const;
  inline void clear_no_peers();
  static const int kNoPeersFieldNumber = 2;
  inline ::google::protobuf::uint32 no_peers() const;
  inline void set_no_peers(::google::protobuf::uint32 value);
  
  // required uint32 no_connected = 3;
  inline bool has_no_connected() const;
  inline void clear_no_connected();
  static const int kNoConnectedFieldNumber = 3;
  inline ::google::protobuf::uint32 no_connected() const;
  inline void set_no_connected(::google::protobuf::uint32 value);
  
  // required .rsctrl.system.ResponseSystemStatus.NetCode net_status = 4;
  inline bool has_net_status() const;
  inline void clear_net_status();
  static const int kNetStatusFieldNumber = 4;
  inline ::rsctrl::system::ResponseSystemStatus_NetCode net_status() const;
  inline void set_net_status(::rsctrl::system::ResponseSystemStatus_NetCode value);
  
  // required .rsctrl.core.Bandwidth bw_total = 5;
  inline bool has_bw_total() const;
  inline void clear_bw_total();
  static const int kBwTotalFieldNumber = 5;
  inline const ::rsctrl::core::Bandwidth& bw_total() const;
  inline ::rsctrl::core::Bandwidth* mutable_bw_total();
  inline ::rsctrl::core::Bandwidth* release_bw_total();
  
  // @@protoc_insertion_point(class_scope:rsctrl.system.ResponseSystemStatus)
 private:
  inline void set_has_status();
  inline void clear_has_status();
  inline void set_has_no_peers();
  inline void clear_has_no_peers();
  inline void set_has_no_connected();
  inline void clear_has_no_connected();
  inline void set_has_net_status();
  inline void clear_has_net_status();
  inline void set_has_bw_total();
  inline void clear_has_bw_total();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::rsctrl::core::Status* status_;
  ::google::protobuf::uint32 no_peers_;
  ::google::protobuf::uint32 no_connected_;
  ::rsctrl::core::Bandwidth* bw_total_;
  int net_status_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(5 + 31) / 32];
  
  friend void  protobuf_AddDesc_system_2eproto();
  friend void protobuf_AssignDesc_system_2eproto();
  friend void protobuf_ShutdownFile_system_2eproto();
  
  void InitAsDefaultInstance();
  static ResponseSystemStatus* default_instance_;
};
// ===================================================================


// ===================================================================

// RequestSystemStatus

// -------------------------------------------------------------------

// ResponseSystemStatus

// required .rsctrl.core.Status status = 1;
inline bool ResponseSystemStatus::has_status() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void ResponseSystemStatus::set_has_status() {
  _has_bits_[0] |= 0x00000001u;
}
inline void ResponseSystemStatus::clear_has_status() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void ResponseSystemStatus::clear_status() {
  if (status_ != NULL) status_->::rsctrl::core::Status::Clear();
  clear_has_status();
}
inline const ::rsctrl::core::Status& ResponseSystemStatus::status() const {
  return status_ != NULL ? *status_ : *default_instance_->status_;
}
inline ::rsctrl::core::Status* ResponseSystemStatus::mutable_status() {
  set_has_status();
  if (status_ == NULL) status_ = new ::rsctrl::core::Status;
  return status_;
}
inline ::rsctrl::core::Status* ResponseSystemStatus::release_status() {
  clear_has_status();
  ::rsctrl::core::Status* temp = status_;
  status_ = NULL;
  return temp;
}

// required uint32 no_peers = 2;
inline bool ResponseSystemStatus::has_no_peers() const {
  return (_has_bits_[0] & 0x00000002u) != 0;
}
inline void ResponseSystemStatus::set_has_no_peers() {
  _has_bits_[0] |= 0x00000002u;
}
inline void ResponseSystemStatus::clear_has_no_peers() {
  _has_bits_[0] &= ~0x00000002u;
}
inline void ResponseSystemStatus::clear_no_peers() {
  no_peers_ = 0u;
  clear_has_no_peers();
}
inline ::google::protobuf::uint32 ResponseSystemStatus::no_peers() const {
  return no_peers_;
}
inline void ResponseSystemStatus::set_no_peers(::google::protobuf::uint32 value) {
  set_has_no_peers();
  no_peers_ = value;
}

// required uint32 no_connected = 3;
inline bool ResponseSystemStatus::has_no_connected() const {
  return (_has_bits_[0] & 0x00000004u) != 0;
}
inline void ResponseSystemStatus::set_has_no_connected() {
  _has_bits_[0] |= 0x00000004u;
}
inline void ResponseSystemStatus::clear_has_no_connected() {
  _has_bits_[0] &= ~0x00000004u;
}
inline void ResponseSystemStatus::clear_no_connected() {
  no_connected_ = 0u;
  clear_has_no_connected();
}
inline ::google::protobuf::uint32 ResponseSystemStatus::no_connected() const {
  return no_connected_;
}
inline void ResponseSystemStatus::set_no_connected(::google::protobuf::uint32 value) {
  set_has_no_connected();
  no_connected_ = value;
}

// required .rsctrl.system.ResponseSystemStatus.NetCode net_status = 4;
inline bool ResponseSystemStatus::has_net_status() const {
  return (_has_bits_[0] & 0x00000008u) != 0;
}
inline void ResponseSystemStatus::set_has_net_status() {
  _has_bits_[0] |= 0x00000008u;
}
inline void ResponseSystemStatus::clear_has_net_status() {
  _has_bits_[0] &= ~0x00000008u;
}
inline void ResponseSystemStatus::clear_net_status() {
  net_status_ = 0;
  clear_has_net_status();
}
inline ::rsctrl::system::ResponseSystemStatus_NetCode ResponseSystemStatus::net_status() const {
  return static_cast< ::rsctrl::system::ResponseSystemStatus_NetCode >(net_status_);
}
inline void ResponseSystemStatus::set_net_status(::rsctrl::system::ResponseSystemStatus_NetCode value) {
  GOOGLE_DCHECK(::rsctrl::system::ResponseSystemStatus_NetCode_IsValid(value));
  set_has_net_status();
  net_status_ = value;
}

// required .rsctrl.core.Bandwidth bw_total = 5;
inline bool ResponseSystemStatus::has_bw_total() const {
  return (_has_bits_[0] & 0x00000010u) != 0;
}
inline void ResponseSystemStatus::set_has_bw_total() {
  _has_bits_[0] |= 0x00000010u;
}
inline void ResponseSystemStatus::clear_has_bw_total() {
  _has_bits_[0] &= ~0x00000010u;
}
inline void ResponseSystemStatus::clear_bw_total() {
  if (bw_total_ != NULL) bw_total_->::rsctrl::core::Bandwidth::Clear();
  clear_has_bw_total();
}
inline const ::rsctrl::core::Bandwidth& ResponseSystemStatus::bw_total() const {
  return bw_total_ != NULL ? *bw_total_ : *default_instance_->bw_total_;
}
inline ::rsctrl::core::Bandwidth* ResponseSystemStatus::mutable_bw_total() {
  set_has_bw_total();
  if (bw_total_ == NULL) bw_total_ = new ::rsctrl::core::Bandwidth;
  return bw_total_;
}
inline ::rsctrl::core::Bandwidth* ResponseSystemStatus::release_bw_total() {
  clear_has_bw_total();
  ::rsctrl::core::Bandwidth* temp = bw_total_;
  bw_total_ = NULL;
  return temp;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace system
}  // namespace rsctrl

#ifndef SWIG
namespace google {
namespace protobuf {

template <>
inline const EnumDescriptor* GetEnumDescriptor< ::rsctrl::system::ResponseSystemStatus_NetCode>() {
  return ::rsctrl::system::ResponseSystemStatus_NetCode_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< rsctrl::system::RequestMsgIds>() {
  return rsctrl::system::RequestMsgIds_descriptor();
}
template <>
inline const EnumDescriptor* GetEnumDescriptor< rsctrl::system::ResponseMsgIds>() {
  return rsctrl::system::ResponseMsgIds_descriptor();
}

}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_system_2eproto__INCLUDED
