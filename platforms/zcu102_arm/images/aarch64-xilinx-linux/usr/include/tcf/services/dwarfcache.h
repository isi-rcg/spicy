/*******************************************************************************
 * Copyright (c) 2006-2018 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 *******************************************************************************/

/*
 * This module implements caching of DWARF debug information.
 *
 * Cached data stays in memory at least until end of the current event dispatch cycle.
 * To lock data for longer period of time clients can use ELF_File.ref_cnt.
 *
 * Functions in this module use exceptions to report errors, see exceptions.h
 */
#ifndef D_dwarfcache
#define D_dwarfcache

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <tcf/framework/errors.h>
#include <tcf/services/tcf_elf.h>
#include <tcf/services/dwarfio.h>
#include <tcf/services/symbols.h>

#ifndef ENABLE_DWARF_LAZY_LOAD
#  define ENABLE_DWARF_LAZY_LOAD 1
#endif

typedef struct FileInfo FileInfo;
typedef struct ObjectInfo ObjectInfo;
typedef struct PubNamesInfo PubNamesInfo;
typedef struct PubNamesTable PubNamesTable;
typedef struct SymbolInfo SymbolInfo;
typedef struct PropertyValue PropertyValue;
typedef struct LineNumbersState LineNumbersState;
typedef struct CompUnit CompUnit;
typedef struct SymbolSection SymbolSection;
typedef struct UnitAddressRange UnitAddressRange;
typedef struct FrameInfoRange FrameInfoRange;
typedef struct FrameInfoIndex FrameInfoIndex;
typedef struct ObjectHashTable ObjectHashTable;
typedef struct DWARFCache DWARFCache;

struct FileInfo {
    const char * mName;
    const char * mDir;
    U4_T mModTime;
    U4_T mSize;
    unsigned mNameHash;
    FileInfo * mNextInHash;
    CompUnit * mCompUnit;
    unsigned mAreaCnt;
};

#define TAG_fund_type           0x2000
#define TAG_index_range         0x2001
#define TAG_mod_pointer         0x2002
#define TAG_mod_reference       0x2003

#define DOIF_declaration        0x000001
#define DOIF_external           0x000002
#define DOIF_artificial         0x000004
#define DOIF_specification      0x000008
#define DOIF_abstract_origin    0x000010
#define DOIF_extension          0x000020
#define DOIF_private            0x000040
#define DOIF_protected          0x000080
#define DOIF_public             0x000100
#define DOIF_children_loaded    0x000200
#define DOIF_ranges             0x000400
#define DOIF_aranges            0x000800
#define DOIF_find_mark          0x001000
#define DOIF_load_mark          0x002000
#define DOIF_pub_mark           0x004000
#define DOIF_low_pc             0x008000
#define DOIF_need_frame         0x010000
#define DOIF_mips_linkage_name  0x020000
#define DOIF_linkage_name       0x040000
#define DOIF_mangled_name       0x080000
#define DOIF_optional           0x100000
#define DOIF_location           0x200000
#define DOIF_data_location      0x400000
#define DOIF_const_value        0x800000

struct ObjectInfo {

    /* 'mID' is link-time debug information entry address:
     * address of .debug_info section + offset in the section */
    /* TODO: adding section address is not necessary, object ID is valid per section only */
    ContextAddress mID;

    ObjectInfo * mHashNext;
    ObjectInfo * mSibling;
    ObjectInfo * mChildren;
    ObjectInfo * mParent;
    ObjectInfo * mDefinition;

    U2_T mTag;
    U4_T mFlags;
    CompUnit * mCompUnit;
    ObjectInfo * mType;
    const char * mName;

    union {
        U2_T mFundType;
        struct {
            ELF_Section * mSection;
            ContextAddress mLowPC;
            union {
                U8_T mRanges;
                ContextAddress mAddr;
            } mHighPC;
        } mCode;
        struct {
            U2_T mFmt;
            union {
                I8_T mValue;
                struct {
                    U1_T * mAddr;
                    size_t mSize;
                } mExpr;
            } mLow;
            union {
                I8_T mValue;
                struct {
                    U1_T * mAddr;
                    size_t mSize;
                } mExpr;
            } mHigh;
        } mRange;
    } u;
};

struct PubNamesInfo {
    unsigned mNext;
    ObjectInfo * mObject;
};

struct PubNamesTable {
    unsigned mHashSize;
    unsigned * mHash;
    PubNamesInfo * mNext;
    unsigned mCnt;
    unsigned mMax;
};

struct PropertyValue {
    Context * mContext;
    int mFrame;
    ObjectInfo * mObject;
    U2_T mAttr;
    U2_T mForm;
    U8_T mValue;
    U1_T * mAddr;
    size_t mSize;
    int mBigEndian;
    ELF_Section * mSection;
    LocationPiece * mPieces;
    U4_T mPieceCnt;
};

#define LINE_IsStmt         0x01
#define LINE_BasicBlock     0x02
#define LINE_PrologueEnd    0x04
#define LINE_EpilogueBegin  0x08
#define LINE_EndSequence    0x10

struct LineNumbersState {
    ContextAddress mAddress;
    char * mFileName;
    U4_T mSection;
    U4_T mStatesIndexPos;
    U4_T mFile;
    U4_T mLine;
    U2_T mColumn;
    U1_T mFlags;
    U1_T mISA;
    U1_T mOpIndex;
    U1_T mDiscriminator;
};

struct CompUnit {
    ObjectInfo * mObject;

    ELF_File * mFile;
    ELF_Section * mTextSection;

    const char * mProducer;

    U2_T mLanguage;

    DIO_UnitDescriptor mDesc;
    RegisterIdScope mRegIdScope;

    ELF_Section * mLineInfoSection;
    U8_T mLineInfoOffs;
    char * mDir;

    U4_T mFilesCnt;
    U4_T mFilesMax;
    FileInfo * mFiles;

    U4_T mDirsCnt;
    U4_T mDirsMax;
    char ** mDirs;

    U4_T mStatesCnt;
    U4_T mStatesMax;
    LineNumbersState * mStates;
    LineNumbersState ** mStatesIndex;
    U1_T mLineInfoLoaded;

    CompUnit * mBaseTypes;
    CompUnit * mNextTypeUnit;

    ContextAddress mFundTypeID;
};

/* Address range of a compilation unit. A unit can occupy multiple address ranges. */
struct UnitAddressRange {
    CompUnit * mUnit;       /* Compilation unit */
    U4_T mSection;          /* Index of ELF file section that contains the range */
    ContextAddress mAddr;   /* Link-time start address of the range */
    ContextAddress mSize;   /* Size of the range */
};

struct FrameInfoIndex {
    int mRelocatable;
    ELF_Section * mSection;
    FrameInfoRange * mFrameInfoRanges;
    unsigned mFrameInfoRangesCnt;
    unsigned mFrameInfoRangesMax;
    FrameInfoIndex * mNext;
};

struct ObjectHashTable {
    ObjectInfo * mCompUnits;
    ObjectInfo ** mObjectHash;
    unsigned mObjectHashSize;
    CompUnit ** mCompUnitsIndex;
    unsigned mCompUnitsIndexSize;
};

#define DWARF_CACHE_MAGIC 0x34625490

struct DWARFCache {
    int magic;
    ELF_File * mFile;
    ErrorReport * mErrorReport;
    ELF_Section * mDebugLineV1;
    ELF_Section * mDebugLineV2;
    ELF_Section * mDebugLoc;
    ELF_Section * mDebugRanges;
    ObjectHashTable * mObjectHashTable; /* per ELF section */
    struct ObjectArray * mObjectList;
    unsigned mObjectArrayPos;
    ContextAddress mFundTypeID;
    UnitAddressRange * mAddrRanges;
    ContextAddress mAddrRangesMaxSize;
    unsigned mAddrRangesCnt;
    unsigned mAddrRangesMax;
    int mAddrRangesRelocatable;
    PubNamesTable mPubNames;
    FrameInfoIndex * mFrameInfo;
    unsigned mFileInfoHashSize;
    FileInfo ** mFileInfoHash;
    int mLineInfoLoaded;
    CompUnit ** mTypeUnitHash;
    unsigned mTypeUnitHashSize;
    int lazy_loaded;
};

/* Return DWARF cache for given file, create and populate the cache if needed, throw an exception if error */
extern DWARFCache * get_dwarf_cache(ELF_File * file);

#if ENABLE_DWARF_LAZY_LOAD
  /* Load children of DWARF object - if not loaded already. Return obj->mChildren */
  extern ObjectInfo * get_dwarf_children(ObjectInfo * obj);
  /* Load parent of DWARF object - if not loaded already. Return obj->mParent */
  extern ObjectInfo * get_dwarf_parent(ObjectInfo * obj);
#else
#  define get_dwarf_children(obj) ((obj)->mChildren)
#  define get_dwarf_parent(obj) ((obj)->mParent)
#endif

/* Return file name hash. The hash is used to search FileInfo. */
extern unsigned calc_file_name_hash(const char * s);

/* Load line number information for given compilation unit, throw an exception if error */
extern void load_line_numbers(CompUnit * unit);

/* Find ObjectInfo by ID */
extern ObjectInfo * find_object(ELF_Section * sec, ContextAddress ID);

/* Search and return first compilation unit address range in given link-time address range 'addr_min'..'addr_max' (inclusive). */
extern UnitAddressRange * find_comp_unit_addr_range(DWARFCache * cache, ELF_Section * section,
    ContextAddress addr_min, ContextAddress addr_max);

/*
 * Read a property of a DWARF object, perform ELF relocations if any.
 * FORM_ADDR values are mapped to run-time address space.
 */
extern void read_dwarf_object_property(Context * Ctx, int Frame, ObjectInfo * Obj, U2_T Attr, PropertyValue * Value);

/*
 * Read and evaluate a property of a DWARF object, perform ELF relocations if any.
 * FORM_ADDR values are mapped to run-time address space.
 */
extern void read_and_evaluate_dwarf_object_property_with_args(
    Context * ctx, int frame, ObjectInfo * obj, U2_T attr,
    uint64_t * args, unsigned args_cnt, PropertyValue * value);

#define read_and_evaluate_dwarf_object_property(ctx, frame, obj, attr, value) \
    read_and_evaluate_dwarf_object_property_with_args(ctx, frame, obj, attr, NULL, 0, value)

/*
 * Convert PropertyValue to a number.
 * Note: result of location expression evaluation can be converted only if the expression represents a memory address.
 */
extern U8_T get_numeric_property_value(PropertyValue * Value);

/*
 * Search and return first compilation unit address range in given run-time address range 'addr_min'..'addr_max' (inclusive).
 * If 'range_rt_addr' not NULL, *range_rt_addr is assigned run-time address of the range.
 */
extern struct UnitAddressRange * elf_find_unit(Context * ctx, ContextAddress addr_min, ContextAddress addr_max, ContextAddress * range_rt_addr);

/*
 * Get the TCF Symbol from DWARF symbol info.
 */
extern void elf_object2symbol(ObjectInfo * ref, ObjectInfo * obj, Symbol ** res);

#endif /* ENABLE_ELF && ENABLE_DebugContext */

#endif /* D_dwarfcache */
