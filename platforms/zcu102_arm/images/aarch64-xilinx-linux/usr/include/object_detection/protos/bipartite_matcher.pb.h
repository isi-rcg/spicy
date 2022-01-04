// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: object_detection/protos/bipartite_matcher.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto

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
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto {
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
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto;
namespace object_detection {
namespace protos {
class BipartiteMatcher;
class BipartiteMatcherDefaultTypeInternal;
extern BipartiteMatcherDefaultTypeInternal _BipartiteMatcher_default_instance_;
}  // namespace protos
}  // namespace object_detection
PROTOBUF_NAMESPACE_OPEN
template<> ::object_detection::protos::BipartiteMatcher* Arena::CreateMaybeMessage<::object_detection::protos::BipartiteMatcher>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace object_detection {
namespace protos {

// ===================================================================

class BipartiteMatcher :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:object_detection.protos.BipartiteMatcher) */ {
 public:
  BipartiteMatcher();
  virtual ~BipartiteMatcher();

  BipartiteMatcher(const BipartiteMatcher& from);
  BipartiteMatcher(BipartiteMatcher&& from) noexcept
    : BipartiteMatcher() {
    *this = ::std::move(from);
  }

  inline BipartiteMatcher& operator=(const BipartiteMatcher& from) {
    CopyFrom(from);
    return *this;
  }
  inline BipartiteMatcher& operator=(BipartiteMatcher&& from) noexcept {
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
  static const BipartiteMatcher& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const BipartiteMatcher* internal_default_instance() {
    return reinterpret_cast<const BipartiteMatcher*>(
               &_BipartiteMatcher_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(BipartiteMatcher& a, BipartiteMatcher& b) {
    a.Swap(&b);
  }
  inline void Swap(BipartiteMatcher* other) {
    if (other == this) return;
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  inline BipartiteMatcher* New() const final {
    return CreateMaybeMessage<BipartiteMatcher>(nullptr);
  }

  BipartiteMatcher* New(::PROTOBUF_NAMESPACE_ID::Arena* arena) const final {
    return CreateMaybeMessage<BipartiteMatcher>(arena);
  }
  void CopyFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void MergeFrom(const ::PROTOBUF_NAMESPACE_ID::Message& from) final;
  void CopyFrom(const BipartiteMatcher& from);
  void MergeFrom(const BipartiteMatcher& from);
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
  void InternalSwap(BipartiteMatcher* other);
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "object_detection.protos.BipartiteMatcher";
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
    ::PROTOBUF_NAMESPACE_ID::internal::AssignDescriptors(&::descriptor_table_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto);
    return ::descriptor_table_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto.file_level_metadata[kIndexInFileMessages];
  }

  public:

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kUseMatmulGatherFieldNumber = 6,
  };
  // optional bool use_matmul_gather = 6 [default = false];
  bool has_use_matmul_gather() const;
  void clear_use_matmul_gather();
  bool use_matmul_gather() const;
  void set_use_matmul_gather(bool value);

  // @@protoc_insertion_point(class_scope:object_detection.protos.BipartiteMatcher)
 private:
  class _Internal;

  ::PROTOBUF_NAMESPACE_ID::internal::InternalMetadataWithArena _internal_metadata_;
  ::PROTOBUF_NAMESPACE_ID::internal::HasBits<1> _has_bits_;
  mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  bool use_matmul_gather_;
  friend struct ::TableStruct_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// BipartiteMatcher

// optional bool use_matmul_gather = 6 [default = false];
inline bool BipartiteMatcher::has_use_matmul_gather() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void BipartiteMatcher::clear_use_matmul_gather() {
  use_matmul_gather_ = false;
  _has_bits_[0] &= ~0x00000001u;
}
inline bool BipartiteMatcher::use_matmul_gather() const {
  // @@protoc_insertion_point(field_get:object_detection.protos.BipartiteMatcher.use_matmul_gather)
  return use_matmul_gather_;
}
inline void BipartiteMatcher::set_use_matmul_gather(bool value) {
  _has_bits_[0] |= 0x00000001u;
  use_matmul_gather_ = value;
  // @@protoc_insertion_point(field_set:object_detection.protos.BipartiteMatcher.use_matmul_gather)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace protos
}  // namespace object_detection

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_object_5fdetection_2fprotos_2fbipartite_5fmatcher_2eproto