// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: object_detection/protos/matcher.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_object_5fdetection_2fprotos_2fmatcher_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_object_5fdetection_2fprotos_2fmatcher_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3009000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3009002 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "object_detection/protos/argmax_matcher.pb.h"
#include "object_detection/protos/bipartite_matcher.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_object_5fdetection_2fprotos_2fmatcher_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_object_5fdetection_2fprotos_2fmatcher_2eproto {
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::PROTOBUF_NAMESPACE_ID::internal::FieldMetadata field_metadata[];
  static const ::PROTOBUF_NAMESPACE_ID::internal::SerializationTable serialization_table[];
  static const ::PROTOBUF_NAMESPACE_ID::uint32 offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_object_5fdetection_2fprotos_2fmatcher_2eproto;
namespace object_detection {
namespace protos {
class Matcher;
class MatcherDefaultTypeInternal;
extern MatcherDefaultTypeInternal _Matcher_default_instance_;
}  // namespace protos
}  // namespace object_detection
PROTOBUF_NAMESPACE_OPEN
template<> ::object_detection::protos::Matcher* Arena::CreateMaybeMessage<::object_detection::protos::Matcher>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace object_detection {
namespace protos {

// ===================================================================

class Matcher :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:object_detection.protos.Matcher) */ {
 public:
  Matcher();
  virtual ~Matcher();

  Matcher(const Matcher& from);
  Matcher(Matcher&& from) noexcept
    : Matcher() {
    *this = ::std::move(from);
  }

  inline Matcher& operator=(const Matcher& from) {
    CopyFrom(from);
    return *this;
  }
  inline Matcher& operator=(Matcher&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  inline const ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet& unknown_fields() const {
    return _internal_metadata_.unknown_fields();
  }
  inline ::PROTOBUF_NAMESPACE_ID::UnknownFieldSet* mutable_unknown_fields() {
    return _internal_metadata_.mutable_unknown_fields();
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return GetMetadataStatic().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return GetMetadataStatic().reflection;
  }
  static const Matcher& default_instance();

  enum MatcherOneofCase {
    kArgmaxMatcher = 1,
    kBipartiteMatcher = 2,
    MATCHER_ONEOF_NOT_SET = 0,
  };

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Matcher* internal_default_instance() {
    return reinterpret_cast<const Matcher*>(
               &_Matcher_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Matcher& a, Matcher& b) {
    a.Swap(&b);
  }
  inline void Swap(Matcher* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline Matcher* New() const final {
    return CreateMaybeMessage<Matcher>(nullptr);
  }

  Matcher* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<Matcher>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const Matcher& from);
  void MergeFrom(const Matcher& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  #else
  bool MergePartialFromCodedStream(
      ::PROTOBUF_NAMESPACE_ID::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::PROTOBUF_NAMESPACE_ID::io::CodedOutputStream* output) const final;
  ::PROTOBUF_NAMESPACE_ID::uint8* InternalSerializeWithCachedSizesToArray(
      ::PROTOBUF_NAMESPACE_ID::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  inline void SharedCtor();
  inline void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Matcher* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "object_detection.protos.Matcher";
  }
  private:
  inline ::PROTOBUF_NAMESPACE_ID::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;
  private:
  static ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadataStatic() {
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_object_5fdetection_2fprotos_2fmatcher_2eproto);
    return ::descriptor_table_object_5fdetection_2fprotos_2fmatcher_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kArgmaxMatcherFieldNumber = 1,
    kBipartiteMatcherFieldNumber = 2,
  };
  // optional .object_detection.protos.ArgMaxMatcher argmax_matcher = 1;
  bool has_argmax_matcher() const;
  void clear_argmax_matcher();
  const ::object_detection::protos::ArgMaxMatcher& argmax_matcher() const;
  ::object_detection::protos::ArgMaxMatcher* release_argmax_matcher();
  ::object_detection::protos::ArgMaxMatcher* mutable_argmax_matcher();
  void set_allocated_argmax_matcher(::object_detection::protos::ArgMaxMatcher* argmax_matcher);

  // optional .object_detection.protos.BipartiteMatcher bipartite_matcher = 2;
  bool has_bipartite_matcher() const;
  void clear_bipartite_matcher();
  const ::object_detection::protos::BipartiteMatcher& bipartite_matcher() const;
  ::object_detection::protos::BipartiteMatcher* release_bipartite_matcher();
  ::object_detection::protos::BipartiteMatcher* mutable_bipartite_matcher();
  void set_allocated_bipartite_matcher(::object_detection::protos::BipartiteMatcher* bipartite_matcher);

  void clear_matcher_oneof();
  MatcherOneofCase matcher_oneof_case() const;
  // @@protoc_insertion_point(class_scope:object_detection.protos.Matcher)
 private:
  class _Internal;
  void set_has_argmax_matcher();
  void set_has_bipartite_matcher();

  inline bool has_matcher_oneof() const;
  inline void clear_has_matcher_oneof();

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  union MatcherOneofUnion {
    MatcherOneofUnion() {}
    ::object_detection::protos::ArgMaxMatcher* argmax_matcher_;
    ::object_detection::protos::BipartiteMatcher* bipartite_matcher_;
  } matcher_oneof_;
  ::PROTOBUF_NAMESPACE_ID::uint32 _oneof_case_[1];

  friend struct ::TableStruct_object_5fdetection_2fprotos_2fmatcher_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Matcher

// optional .object_detection.protos.ArgMaxMatcher argmax_matcher = 1;
inline bool Matcher::has_argmax_matcher() const {
  return matcher_oneof_case() == kArgmaxMatcher;
}
inline void Matcher::set_has_argmax_matcher() {
  _oneof_case_[0] = kArgmaxMatcher;
}
inline ::object_detection::protos::ArgMaxMatcher* Matcher::release_argmax_matcher() {
  // @@protoc_insertion_point(field_release:object_detection.protos.Matcher.argmax_matcher)
  if (has_argmax_matcher()) {
    clear_has_matcher_oneof();
      ::object_detection::protos::ArgMaxMatcher* temp = matcher_oneof_.argmax_matcher_;
    matcher_oneof_.argmax_matcher_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const ::object_detection::protos::ArgMaxMatcher& Matcher::argmax_matcher() const {
  // @@protoc_insertion_point(field_get:object_detection.protos.Matcher.argmax_matcher)
  return has_argmax_matcher()
      ? *matcher_oneof_.argmax_matcher_
      : *reinterpret_cast< ::object_detection::protos::ArgMaxMatcher*>(&::object_detection::protos::_ArgMaxMatcher_default_instance_);
}
inline ::object_detection::protos::ArgMaxMatcher* Matcher::mutable_argmax_matcher() {
  if (!has_argmax_matcher()) {
    clear_matcher_oneof();
    set_has_argmax_matcher();
    matcher_oneof_.argmax_matcher_ = CreateMaybeMessage< ::object_detection::protos::ArgMaxMatcher >(
        GetArenaNoVirtual());
  }
  // @@protoc_insertion_point(field_mutable:object_detection.protos.Matcher.argmax_matcher)
  return matcher_oneof_.argmax_matcher_;
}

// optional .object_detection.protos.BipartiteMatcher bipartite_matcher = 2;
inline bool Matcher::has_bipartite_matcher() const {
  return matcher_oneof_case() == kBipartiteMatcher;
}
inline void Matcher::set_has_bipartite_matcher() {
  _oneof_case_[0] = kBipartiteMatcher;
}
inline ::object_detection::protos::BipartiteMatcher* Matcher::release_bipartite_matcher() {
  // @@protoc_insertion_point(field_release:object_detection.protos.Matcher.bipartite_matcher)
  if (has_bipartite_matcher()) {
    clear_has_matcher_oneof();
      ::object_detection::protos::BipartiteMatcher* temp = matcher_oneof_.bipartite_matcher_;
    matcher_oneof_.bipartite_matcher_ = nullptr;
    return temp;
  } else {
    return nullptr;
  }
}
inline const ::object_detection::protos::BipartiteMatcher& Matcher::bipartite_matcher() const {
  // @@protoc_insertion_point(field_get:object_detection.protos.Matcher.bipartite_matcher)
  return has_bipartite_matcher()
      ? *matcher_oneof_.bipartite_matcher_
      : *reinterpret_cast< ::object_detection::protos::BipartiteMatcher*>(&::object_detection::protos::_BipartiteMatcher_default_instance_);
}
inline ::object_detection::protos::BipartiteMatcher* Matcher::mutable_bipartite_matcher() {
  if (!has_bipartite_matcher()) {
    clear_matcher_oneof();
    set_has_bipartite_matcher();
    matcher_oneof_.bipartite_matcher_ = CreateMaybeMessage< ::object_detection::protos::BipartiteMatcher >(
        GetArenaNoVirtual());
  }
  // @@protoc_insertion_point(field_mutable:object_detection.protos.Matcher.bipartite_matcher)
  return matcher_oneof_.bipartite_matcher_;
}

inline bool Matcher::has_matcher_oneof() const {
  return matcher_oneof_case() != MATCHER_ONEOF_NOT_SET;
}
inline void Matcher::clear_has_matcher_oneof() {
  _oneof_case_[0] = MATCHER_ONEOF_NOT_SET;
}
inline Matcher::MatcherOneofCase Matcher::matcher_oneof_case() const {
  return Matcher::MatcherOneofCase(_oneof_case_[0]);
}
#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace protos
}  // namespace object_detection

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_object_5fdetection_2fprotos_2fmatcher_2eproto