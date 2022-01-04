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
 * This module implements low-level functions for reading DWARF debug information.
 *
 * Functions in this module use exceptions to report errors, see exceptions.h
 */

#include <tcf/config.h>

#if ENABLE_ELF

#include <assert.h>
#include <string.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/dwarfio.h>
#include <tcf/services/dwarfreloc.h>
#include <tcf/services/dwarf.h>

#define ABBREV_TABLE_SIZE  (4 * MEM_USAGE_FACTOR - 1)

struct DIO_Abbreviation {
    U2_T mTag;
    U1_T mChildren;
    U4_T mAttrLen;
    U2_T mAttrs[2];
};

typedef struct DIO_Abbreviation DIO_Abbreviation;

struct DIO_AbbrevSet {
    U8_T mOffset;
    U4_T mSize;
    DIO_Abbreviation ** mTable;
    struct DIO_AbbrevSet * mNext;
};

typedef struct DIO_AbbrevSet DIO_AbbrevSet;

struct DIO_Cache {
    U1_T * mStringTable;
    U8_T mStringTableAddr;
    U4_T mStringTableSize;
    DIO_AbbrevSet ** mAbbrevTable;
    U8_T mAbbrevSectionAddr;
};

typedef struct DIO_Cache DIO_Cache;

U8_T dio_gEntryPos = 0;

U8_T dio_gFormData = 0;
size_t dio_gFormDataSize = 0;
void * dio_gFormDataAddr = NULL;
ELF_Section * dio_gFormSection = NULL;

static ELF_Section * sSection;
static int sBigEndian;
static int sAddressSize;
static int sRefAddressSize;
static U1_T * sData;
static U8_T sDataPos;
static U8_T sDataLen;
static DIO_UnitDescriptor * sUnit;

static void dio_CloseELF(ELF_File * File) {
    U4_T n, m;
    DIO_Cache * Cache = (DIO_Cache *)File->dwarf_io_cache;

    if (Cache == NULL) return;
    if (Cache->mAbbrevTable != NULL) {
        for (n = 0; n < ABBREV_TABLE_SIZE; n++) {
            DIO_AbbrevSet * Set = Cache->mAbbrevTable[n];
            while (Set != NULL) {
                DIO_AbbrevSet * Next = Set->mNext;
                for (m = 0; m < Set->mSize; m++) {
                    loc_free(Set->mTable[m]);
                }
                loc_free(Set->mTable);
                loc_free(Set);
                Set = Next;
            }
        }
        loc_free(Cache->mAbbrevTable);
    }
    loc_free(Cache);
    File->dwarf_io_cache = NULL;
}

static DIO_Cache * dio_GetCache(ELF_File * File) {
    static int Inited = 0;
    DIO_Cache * Cache = (DIO_Cache *)File->dwarf_io_cache;

    if (!Inited) {
        elf_add_close_listener(dio_CloseELF);
        Inited = 1;
    }
    if (Cache == NULL) {
        Cache = (DIO_Cache *)(File->dwarf_io_cache = loc_alloc_zero(sizeof(DIO_Cache)));
    }
    return Cache;
}

void dio_EnterSection(DIO_UnitDescriptor * Unit, ELF_Section * Section, U8_T Offset) {
    if (elf_load(Section)) exception(errno);
    if (Offset > Section->size) {
        if (Section->name == NULL) exception(ERR_INV_DWARF);
        str_fmt_exception(ERR_INV_DWARF, "Invalid offset in '%s' section", Section->name);
    }
    sSection = Section;
    sData = (U1_T *)Section->data;
    sDataPos = Offset;
    sDataLen = Section->size;
    sBigEndian = Section->file->big_endian;
    if (Unit != NULL) {
        sAddressSize = Unit->mAddressSize;
        if (Unit->mVersion < 3) sRefAddressSize = Unit->mAddressSize;
        else sRefAddressSize = Unit->m64bit ? 8 : 4;
    }
    else if (Section->file->elf64) {
        sAddressSize = 8;
        sRefAddressSize = 8;
    }
    else {
        sAddressSize = 4;
        sRefAddressSize = 4;
    }
    sUnit = Unit;
    dio_gEntryPos = 0;
    assert(sData != NULL);
    assert(sDataPos < sDataLen);
}

void dio_ExitSection(void) {
    sSection = NULL;
    sDataPos = 0;
    sDataLen = 0;
    sData = NULL;
    sUnit = NULL;
}

U8_T dio_GetPos(void) {
    return sDataPos;
}

U1_T * dio_GetDataPtr(void) {
    return sData + sDataPos;
}

void dio_Skip(I8_T Bytes) {
    if (sDataPos + Bytes > sDataLen) exception(ERR_EOF);
    sDataPos += Bytes;
}

void dio_SetPos(U8_T Pos) {
    if (Pos > sDataLen) exception(ERR_EOF);
    sDataPos = Pos;
}

void dio_Read(U1_T * Buf, U4_T Size) {
    if (sDataPos + Size > sDataLen) exception(ERR_EOF);
    memcpy(Buf, sData + sDataPos, Size);
    sDataPos += Size;
}

static U1_T dio_ReadU1F(void) {
    if (sDataPos >= sDataLen) exception(ERR_EOF);
    return sData[sDataPos++];
}

U1_T dio_ReadU1(void) {
    return sDataPos < sDataLen ? sData[sDataPos++] : dio_ReadU1F();
}

#define dio_ReadU1() (sDataPos < sDataLen ? sData[sDataPos++] : dio_ReadU1F())

U2_T dio_ReadU2(void) {
    U2_T x0, x1;
    if (sDataPos + 2 > sDataLen) exception(ERR_EOF);
    x0 = sData[sDataPos++];
    x1 = sData[sDataPos++];
    return sBigEndian ? (x0 << 8) | x1 : x0 | (x1 << 8);
}

U4_T dio_ReadU4(void) {
#if defined(__BYTE_ORDER__)
    U4_T x;
    if (sDataPos + 4 > sDataLen) exception(ERR_EOF);
    x = *(U4_T *)(sData + sDataPos);
    if ((sBigEndian == 0) != (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)) SWAP(x);
    sDataPos += 4;
    return x;
#else
    U4_T x0, x1, x2, x3;
    if (sDataPos + 4 > sDataLen) exception(ERR_EOF);
    x0 = sData[sDataPos++];
    x1 = sData[sDataPos++];
    x2 = sData[sDataPos++];
    x3 = sData[sDataPos++];
    return sBigEndian ?
        (x0 << 24) | (x1 << 16) | (x2 << 8) | x3:
        x0 | (x1 << 8) | (x2 << 16) | (x3 << 24);
#endif
}

U8_T dio_ReadU8(void) {
#if defined(__BYTE_ORDER__) && !defined(__arm__)
    U8_T x;
    if (sDataPos + 8 > sDataLen) exception(ERR_EOF);
    x = *(U8_T *)(sData + sDataPos);
    if ((sBigEndian == 0) != (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)) SWAP(x);
    sDataPos += 8;
    return x;
#else
    U8_T x0 = dio_ReadU4();
    U8_T x1 = dio_ReadU4();
    return sBigEndian ? (x0 << 32) | x1 : x0 | (x1 << 32);
#endif
}

U4_T dio_ReadULEB128(void) {
    U4_T Res = 0;
    int i = 0;
    for (;; i += 7) {
        U1_T n = dio_ReadU1();
        Res |= (U4_T)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) break;
    }
    return Res;
}

I4_T dio_ReadSLEB128(void) {
    U4_T Res = 0;
    int i = 0;
    for (;; i += 7) {
        U1_T n = dio_ReadU1();
        Res |= (U4_T)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) {
            Res |= -(I4_T)(n & 0x40) << i;
            break;
        }
    }
    return (I4_T)Res;
}

U8_T dio_ReadU8LEB128(void) {
    U8_T Res = 0;
    int i = 0;
    for (;; i += 7) {
        U1_T n = dio_ReadU1();
        Res |= (U8_T)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) break;
    }
    return Res;
}

I8_T dio_ReadS8LEB128(void) {
    U8_T Res = 0;
    int i = 0;
    for (;; i += 7) {
        U1_T n = dio_ReadU1();
        Res |= (U8_T)(n & 0x7Fu) << i;
        if ((n & 0x80) == 0) {
            Res |= -(I8_T)(n & 0x40) << i;
            break;
        }
    }
    return (I8_T)Res;
}

U8_T dio_ReadAddressX(ELF_Section ** s, int size) {
    U8_T pos = sDataPos;
    switch (size) {
    case 2: {
        U2_T x = dio_ReadU2();
        drl_relocate(sSection, pos, &x, sizeof(x), s);
        return x;
    }
    case 4: {
        U4_T x = dio_ReadU4();
        drl_relocate(sSection, pos, &x, sizeof(x), s);
        return x;
    }
    case 8: {
        U8_T x = dio_ReadU8();
        drl_relocate(sSection, pos, &x, sizeof(x), s);
        return x;
    }
    default:
        str_exception(ERR_INV_DWARF, "Invalid data size");
        return 0;
    }
}

U8_T dio_ReadAddress(ELF_Section ** s) {
    return dio_ReadAddressX(s, sAddressSize);
}

char * dio_ReadString(void) {
    char * Res = (char *)(sData + sDataPos);
    U4_T Length = 0;
    while (dio_ReadU1() != 0) Length++;
    if (Length == 0) return NULL;
    return Res;
}

static U1_T * dio_LoadStringTable(ELF_File * File, U8_T * StringTableAddr, U4_T * StringTableSize) {
    DIO_Cache * Cache = dio_GetCache(File);

    if (Cache->mStringTable == NULL) {
        U4_T ID;
        ELF_Section * Section = NULL;

        for (ID = 1; ID < File->section_cnt; ID++) {
            if (strcmp(File->sections[ID].name, ".debug_str") == 0) {
                if (Section != NULL) {
                    str_exception(ERR_INV_DWARF, "More then one .debug_str section in a file");
                }
                Section = File->sections + ID;
                assert(Section->file == File);
            }
        }

        if (Section == NULL) {
            str_exception(ERR_INV_DWARF, "Section .debug_str not found");
        }

        if (elf_load(Section) < 0) {
            str_exception(errno, "Cannot read .debug_str section");
        }

        Cache->mStringTableAddr = Section->addr;
        Cache->mStringTableSize = (size_t)Section->size;
        Cache->mStringTable = (U1_T *)Section->data;
    }

    *StringTableAddr = Cache->mStringTableAddr;
    *StringTableSize = Cache->mStringTableSize;
    return Cache->mStringTable;
}

static U1_T * dio_LoadAltStringTable(ELF_File * File, U8_T * StringTableAddr, U4_T * StringTableSize) {
    if (File->dwz_file == NULL) str_exception(errno, "Cannot open DWZ file");
    return dio_LoadStringTable(File->dwz_file, StringTableAddr, StringTableSize);
}

static void dio_ReadFormAddr(void) {
    dio_gFormData = dio_ReadAddressX(&dio_gFormSection, sAddressSize);
    dio_gFormDataSize = sAddressSize;
}

static void dio_ReadFormBlock(U4_T Size) {
    dio_gFormDataAddr = sData + sDataPos;
    dio_gFormDataSize = Size;
    if (sDataPos + Size > sDataLen) exception(ERR_EOF);
    sDataPos += Size;
}

static void dio_ReadFormData(U1_T Size, U8_T Data) {
    dio_gFormDataAddr = sData + sDataPos - Size;
    dio_gFormData = Data;
    dio_gFormDataSize = Size;
}

static void dio_ReadFormRef(void) {
    dio_gFormData = dio_ReadU4();
    dio_gFormDataSize = 4;
}

static void dio_ReadFormAltRef(void) {
    int size = sUnit->m64bit ? 8 : 4;
    dio_gFormData = dio_ReadAddressX(&dio_gFormSection, size);
    dio_gFormDataSize = size;
    dio_gFormData += sSection->addr;
}

static void dio_ReadFormRelRef(U2_T Attr, U8_T Offset) {
    if (sUnit->mUnitSize == 0) {
        /* OK. This occurs in DWARF 1, where unit header does not provide unit size */
    }
    else if (Attr == AT_sibling && Offset == sUnit->mUnitSize) {
        /* OK. This occurs in DWARF 2 for AT_sibling attribute of TAG_compile_unit */
    }
    else if (Offset >= sUnit->mUnitSize) {
        str_exception(ERR_INV_DWARF, "Invalid REF attribute value");
    }
    dio_gFormData = sSection->addr + sUnit->mUnitOffs + Offset;
    dio_gFormDataSize = sAddressSize;
}

static void dio_ReadFormRefAddr(void) {
    dio_gFormData = dio_ReadAddressX(&dio_gFormSection, sRefAddressSize);
    dio_gFormDataSize = sRefAddressSize;
    dio_gFormData += sSection->addr;
}

static void dio_ReadFormString(void) {
    dio_gFormDataAddr = sData + sDataPos;
    dio_gFormDataSize = 1;
    while (dio_ReadU1()) dio_gFormDataSize++;
}

static void dio_ReadFormStringRef(void) {
    U8_T StringTableAddr = 0;
    U4_T StringTableSize = 0;
    U1_T * StringTable = dio_LoadStringTable(sSection->file, &StringTableAddr, &StringTableSize);
    U8_T Offset = dio_ReadAddressX(&dio_gFormSection, sUnit->m64bit ? 8 : 4) - StringTableAddr;
    dio_gFormDataAddr = StringTable + Offset;
    dio_gFormDataSize = 1;
    for (;;) {
        if (Offset >= StringTableSize) {
            str_exception(ERR_INV_DWARF, "Invalid FORM_STRP attribute");
        }
        if (StringTable[Offset++] == 0) break;
        dio_gFormDataSize++;
    }
}

static void dio_ReadFormAltStringRef(void) {
    U8_T StringTableAddr = 0;
    U4_T StringTableSize = 0;
    U1_T * StringTable = dio_LoadAltStringTable(sSection->file, &StringTableAddr, &StringTableSize);
    U8_T Offset = dio_ReadAddressX(&dio_gFormSection, sUnit->m64bit ? 8 : 4) - StringTableAddr;
    dio_gFormDataAddr = StringTable + Offset;
    dio_gFormDataSize = 1;
    for (;;) {
        if (Offset >= StringTableSize) {
            str_exception(ERR_INV_DWARF, "Invalid FORM_STRP_ALT attribute");
        }
        if (StringTable[Offset++] == 0) break;
        dio_gFormDataSize++;
    }
}

void dio_ReadAttribute(U2_T Attr, U2_T Form) {
    dio_gFormSection = NULL;
    dio_gFormDataAddr = NULL;
    dio_gFormDataSize = 0;
    dio_gFormData = 0;
    if (sSection->relocate &&
            (Attr == AT_stmt_list || Attr == AT_ranges ||
                (Attr == AT_high_pc && Form != FORM_ADDR))) {
        U4_T Size = 0;
        switch (Form) {
        case FORM_DATA2     : Size = 2; break;
        case FORM_DATA4     : Size = 4; break;
        case FORM_DATA8     : Size = 8; break;
        case FORM_SEC_OFFSET: Size = sUnit->m64bit ? 8 : 4; break;
        default: str_exception(ERR_INV_DWARF, "FORM_DATA or FORM_SEC_OFFSET was expected");
        }
        dio_gFormData = dio_ReadAddressX(&dio_gFormSection, Size);
        dio_gFormDataSize = Size;
        return;
    }
    switch (Form) {
    case FORM_ADDR          : dio_ReadFormAddr(); break;
    case FORM_REF           : dio_ReadFormRef(); break;
    case FORM_GNU_REF_ALT   : dio_ReadFormAltRef(); break;
    case FORM_BLOCK1        : dio_ReadFormBlock(dio_ReadU1()); break;
    case FORM_BLOCK2        : dio_ReadFormBlock(dio_ReadU2()); break;
    case FORM_BLOCK4        : dio_ReadFormBlock(dio_ReadU4()); break;
    case FORM_BLOCK         : dio_ReadFormBlock(dio_ReadULEB128()); break;
    case FORM_DATA1         : dio_ReadFormData(1, dio_ReadU1()); break;
    case FORM_DATA2         : dio_ReadFormData(2, dio_ReadU2()); break;
    case FORM_DATA4         : dio_ReadFormData(4, dio_ReadU4()); break;
    case FORM_DATA8         : dio_ReadFormData(8, dio_ReadU8()); break;
    case FORM_SDATA         : dio_ReadFormData(8, dio_ReadS8LEB128()); dio_gFormDataAddr = NULL; break;
    case FORM_UDATA         : dio_ReadFormData(8, dio_ReadU8LEB128()); dio_gFormDataAddr = NULL; break;
    case FORM_FLAG          : dio_ReadFormData(1, dio_ReadU1()); break;
    case FORM_FLAG_PRESENT  : dio_ReadFormData(0, 1); break;
    case FORM_STRING        : dio_ReadFormString(); break;
    case FORM_STRP          : dio_ReadFormStringRef(); break;
    case FORM_GNU_STRP_ALT  : dio_ReadFormAltStringRef(); break;
    case FORM_REF_ADDR      : dio_ReadFormRefAddr(); break;
    case FORM_REF1          : dio_ReadFormRelRef(Attr, dio_ReadU1()); break;
    case FORM_REF2          : dio_ReadFormRelRef(Attr, dio_ReadU2()); break;
    case FORM_REF4          : dio_ReadFormRelRef(Attr, dio_ReadU4()); break;
    case FORM_REF8          : dio_ReadFormRelRef(Attr, dio_ReadU8()); break;
    case FORM_REF_UDATA     : dio_ReadFormRelRef(Attr, dio_ReadULEB128()); break;
    case FORM_SEC_OFFSET:   if (sUnit->m64bit) dio_ReadFormData(8, dio_ReadAddressX(&dio_gFormSection,8));
                            else dio_ReadFormData(4, dio_ReadAddressX(&dio_gFormSection,4));
                            break;
    case FORM_EXPRLOC       : dio_ReadFormBlock(dio_ReadULEB128()); break;
    case FORM_REF_SIG8      : dio_ReadFormData(8, dio_ReadU8()); break;
    default: str_fmt_exception(ERR_INV_DWARF, "Invalid FORM code 0x%04x", Form);
    }
}

int dio_ReadEntry(DIO_EntryCallBack CallBack, U2_T TargetAttr) {
    DIO_Abbreviation * Abbr = NULL;
    U2_T Tag = 0;
    U4_T AttrPos = 0;
    U4_T EntrySize = 0;
    int Init = 1;

    dio_gEntryPos = sDataPos;
    if (sUnit->mVersion >= 2) {
        U4_T AbbrCode = dio_ReadULEB128();
        if (AbbrCode == 0) return 0;
        if (AbbrCode >= sUnit->mAbbrevTableSize || sUnit->mAbbrevTable[AbbrCode] == NULL) {
            str_exception(ERR_INV_DWARF, "Invalid abbreviation code");
        }
        Abbr =  sUnit->mAbbrevTable[AbbrCode];
        Tag = Abbr->mTag;
    }
    else {
        EntrySize = dio_ReadU4();
        if (EntrySize < 8) {
            while (EntrySize > 4) {
                dio_ReadU1();
                EntrySize--;
            }
            return 0;
        }
        Tag = dio_ReadU2();
    }
    for (;;) {
        U2_T Attr = 0;
        U2_T Form = 0;
        if (Init) {
            Form = DWARF_ENTRY_NO_CHILDREN;
            if (Abbr != NULL && Abbr->mChildren) Form = DWARF_ENTRY_HAS_CHILDREN;
            Init = 0;
        }
        else if (Abbr != NULL) {
            if (AttrPos < Abbr->mAttrLen) {
                Attr = Abbr->mAttrs[AttrPos++];
                Form = Abbr->mAttrs[AttrPos++];
                if (Form == FORM_INDIRECT) Form = (U2_T)dio_ReadULEB128();
            }
        }
        else if (sDataPos <= dio_gEntryPos + EntrySize - 2) {
            if (sBigEndian) {
                Attr = (U2_T)sData[sDataPos++] << 8;
                Attr |= (U2_T)sData[sDataPos++];
            }
            else {
                Attr = (U2_T)sData[sDataPos++];
                Attr |= (U2_T)sData[sDataPos++] << 8;
            }
            Form = Attr & 0xF;
            Attr = (Attr & 0xfff0) >> 4;
        }
        if (TargetAttr && Attr != TargetAttr) {
            /* Shortcut for attributes that the caller is not interested in */
            switch (Attr) {
            case 0:
                if (Form != 0) continue;
                return 1;
            case AT_specification_v1:
            case AT_specification_v2:
            case AT_abstract_origin:
            case AT_extension:
                break;
            default:
                switch (Form) {
                case FORM_ADDR          : sDataPos += sAddressSize; continue;
                case FORM_REF           : sDataPos += 4; continue;
                case FORM_GNU_REF_ALT   : sDataPos += (sUnit->m64bit ? 8 : 4); continue;
                case FORM_BLOCK1        : sDataPos += dio_ReadU1F(); continue;
                case FORM_BLOCK2        : sDataPos += dio_ReadU2(); continue;
                case FORM_BLOCK4        : sDataPos += dio_ReadU4(); continue;
                case FORM_BLOCK         : sDataPos += dio_ReadULEB128(); continue;
                case FORM_DATA1         : sDataPos++; continue;
                case FORM_DATA2         : sDataPos += 2; continue;
                case FORM_DATA4         : sDataPos += 4; continue;
                case FORM_DATA8         : sDataPos += 8; continue;
                case FORM_SDATA         : dio_ReadS8LEB128(); continue;
                case FORM_UDATA         : dio_ReadU8LEB128(); continue;
                case FORM_FLAG          : sDataPos++; continue;
                case FORM_FLAG_PRESENT  : continue;
                case FORM_STRING        : dio_ReadFormString(); continue;
                case FORM_STRP          : sDataPos += (sUnit->m64bit ? 8 : 4); continue;
                case FORM_GNU_STRP_ALT  : sDataPos += (sUnit->m64bit ? 8 : 4); continue;
                case FORM_REF_ADDR      : sDataPos += sRefAddressSize; continue;
                case FORM_REF1          : sDataPos++; continue;
                case FORM_REF2          : sDataPos += 2; continue;
                case FORM_REF4          : sDataPos += 4; continue;
                case FORM_REF8          : sDataPos += 8; continue;
                case FORM_REF_UDATA     : dio_ReadULEB128(); continue;
                case FORM_SEC_OFFSET    : sDataPos += (sUnit->m64bit ? 8 : 4); continue;
                case FORM_EXPRLOC       : sDataPos += dio_ReadULEB128(); continue;
                case FORM_REF_SIG8      : sDataPos += 8; continue;
                }
            }
        }
        if (Attr != 0 && Form != 0) dio_ReadAttribute(Attr, Form);
        if (Tag == TAG_compile_unit || Tag == TAG_partial_unit || Tag == TAG_type_unit) {
            if (Attr == AT_sibling && sUnit->mUnitSize == 0) {
                dio_ChkRef(Form);
                assert(sUnit->mVersion == 1);
                sUnit->mUnitSize = (U4_T)(dio_gFormData - sSection->addr - sUnit->mUnitOffs);
                assert(sUnit->mUnitOffs < sDataPos);
                assert(sUnit->mUnitOffs + sUnit->mUnitSize >= sDataPos);
            }
            else if (Attr == 0 && Form == 0) {
                if (sUnit->mUnitSize == 0) str_exception(ERR_INV_DWARF, "Missing compilation unit sibling attribute");
            }
        }
        if (CallBack != NULL) CallBack(Tag, Attr, Form);
        if (Attr == 0 && Form == 0) break;
    }
    return 1;
}

static void dio_FindAbbrevTable(void);

void dio_ReadUnit(DIO_UnitDescriptor * Unit, DIO_EntryCallBack CallBack) {
    memset(Unit, 0, sizeof(DIO_UnitDescriptor));
    sUnit = Unit;
    sUnit->mSection = sSection;
    sUnit->mUnitOffs = sDataPos;
    sUnit->m64bit = 0;
    if (strcmp(sSection->name, ".debug") != 0) {
        ELF_Section * Sect = NULL;
        DIO_Cache * Cache = dio_GetCache(sSection->file);
        sUnit->mUnitSize = dio_ReadU4();
        if (sUnit->mUnitSize == 0xffffffffu) {
            sUnit->m64bit = 1;
            sUnit->mUnitSize = dio_ReadU8();
            sUnit->mUnitSize += 12;
        }
        else {
            sUnit->mUnitSize += 4;
        }
        sUnit->mVersion = dio_ReadU2();
        sUnit->mAbbrevTableOffs = dio_ReadAddressX(&Sect, sUnit->m64bit ? 8 : 4);
        sUnit->mAbbrevTableOffs -= Cache->mAbbrevSectionAddr;
        sUnit->mAddressSize = dio_ReadU1();
        if (strcmp(sSection->name, ".debug_types") == 0) {
            sUnit->mTypeSignature = dio_ReadU8();
            sUnit->mTypeOffset = sUnit->m64bit ? dio_ReadU8() : dio_ReadU4();
        }
        dio_FindAbbrevTable();
    }
    else {
        sUnit->mVersion = 1;
        sUnit->mAddressSize = 4;
    }
    sAddressSize = sUnit->mAddressSize;
    if (sUnit->mVersion < 3) sRefAddressSize = sUnit->mAddressSize;
    else sRefAddressSize = sUnit->m64bit ? 8 : 4;
    while (sUnit->mUnitSize == 0 || sDataPos < sUnit->mUnitOffs + sUnit->mUnitSize) {
        dio_ReadEntry(CallBack, 0);
    }
    sUnit = NULL;
}

#define dio_AbbrevTableHash(Offset) (((unsigned)(Offset)) / 16 % ABBREV_TABLE_SIZE)

void dio_LoadAbbrevTable(ELF_File * File) {
    U8_T TableOffset = 0;
    ELF_Section * Section = NULL;
    static U2_T * AttrBuf = NULL;
    static U4_T AttrBufSize = 0;
    static DIO_Abbreviation ** AbbrevBuf = NULL;
    static U4_T AbbrevBufSize = 0;
    U4_T AbbrevBufPos = 0;
    DIO_Cache * Cache = dio_GetCache(File);
    unsigned i;

    if (Cache->mAbbrevTable != NULL) return;
    Cache->mAbbrevTable = (DIO_AbbrevSet **)loc_alloc_zero(sizeof(DIO_AbbrevSet *) * ABBREV_TABLE_SIZE);

    for (i = 1; i < File->section_cnt; i++) {
        if (strcmp(File->sections[i].name, ".debug_abbrev") == 0) {
            if (Section != NULL) {
                str_exception(ERR_INV_DWARF, "More then one .debug_abbrev section in a file");
            }
            Section = File->sections + i;
        }
    }
    if (Section == NULL) return;
    Cache->mAbbrevSectionAddr = Section->addr;
    dio_EnterSection(NULL, Section, 0);
    for (;;) {
        U4_T AttrPos = 0;
        U2_T Tag = 0;
        U1_T Children = 0;
        U4_T ID = dio_ReadULEB128();
        if (ID == 0) {
            /* End of compilation unit */
            U4_T Hash = dio_AbbrevTableHash(TableOffset);
            DIO_AbbrevSet * AbbrevSet = (DIO_AbbrevSet *)loc_alloc_zero(sizeof(DIO_AbbrevSet));
            AbbrevSet->mOffset = TableOffset;
            AbbrevSet->mTable = (DIO_Abbreviation **)loc_alloc(sizeof(DIO_Abbreviation *) * AbbrevBufPos);
            AbbrevSet->mSize = AbbrevBufPos;
            AbbrevSet->mNext = Cache->mAbbrevTable[Hash];
            Cache->mAbbrevTable[Hash] = AbbrevSet;
            memcpy(AbbrevSet->mTable, AbbrevBuf, sizeof(DIO_Abbreviation *) * AbbrevBufPos);
            memset(AbbrevBuf, 0, sizeof(DIO_Abbreviation *) * AbbrevBufPos);
            AbbrevBufPos = 0;
            if (sDataPos >= Section->size) break;
            TableOffset = sDataPos;
            continue;
        }
        if (ID >= 0x1000000) str_exception(ERR_INV_DWARF, "Invalid abbreviation table");
        if (ID >= AbbrevBufPos) {
            U4_T Pos = AbbrevBufPos;
            AbbrevBufPos = ID + 1;
            if (AbbrevBufPos > AbbrevBufSize) {
                U4_T Size = AbbrevBufSize;
                AbbrevBufSize = AbbrevBufPos + 128;
                AbbrevBuf = (DIO_Abbreviation **)loc_realloc(AbbrevBuf, sizeof(DIO_Abbreviation *) * AbbrevBufSize);
                memset(AbbrevBuf + Size, 0, sizeof(DIO_Abbreviation *) * (AbbrevBufSize - Size));
            }
            while (Pos < AbbrevBufPos) {
                loc_free(AbbrevBuf[Pos]);
                AbbrevBuf[Pos] = NULL;
                Pos++;
            }
        }
        Tag = (U2_T)dio_ReadULEB128();
        Children = (U2_T)dio_ReadU1() != 0;
        for (;;) {
            U4_T Attr = dio_ReadULEB128();
            U4_T Form = dio_ReadULEB128();
            if (Attr >= 0x10000 || Form >= 0x10000) str_exception(ERR_INV_DWARF, "Invalid abbreviation table");
            if (Attr == 0 && Form == 0) {
                DIO_Abbreviation * Abbr;
                if (AbbrevBuf[ID] != NULL) str_exception(ERR_INV_DWARF, "Invalid abbreviation table");
                Abbr = (DIO_Abbreviation *)loc_alloc_zero(sizeof(DIO_Abbreviation) - sizeof(U2_T) * 2 + sizeof(U2_T) * AttrPos);
                Abbr->mTag = Tag;
                Abbr->mChildren = Children;
                Abbr->mAttrLen = AttrPos;
                memcpy(Abbr->mAttrs, AttrBuf, sizeof(U2_T) * AttrPos);
                AbbrevBuf[ID] = Abbr;
                break;
            }
            if (AttrBufSize < AttrPos + 2) {
                AttrBufSize = AttrPos + 256;
                AttrBuf = (U2_T *)loc_realloc(AttrBuf, sizeof(U2_T) * AttrBufSize);
            }
            AttrBuf[AttrPos++] = (U2_T)Attr;
            AttrBuf[AttrPos++] = (U2_T)Form;
        }
    }
    assert(AbbrevBufPos == 0);
    dio_ExitSection();
}

static void dio_FindAbbrevTable(void) {
    DIO_Cache * Cache = dio_GetCache(sSection->file);
    if (Cache->mAbbrevTable != NULL) {
        U4_T Hash = dio_AbbrevTableHash(sUnit->mAbbrevTableOffs);
        DIO_AbbrevSet * AbbrevSet = Cache->mAbbrevTable[Hash];
        while (AbbrevSet != NULL) {
            if (AbbrevSet->mOffset == sUnit->mAbbrevTableOffs) {
                sUnit->mAbbrevTable = AbbrevSet->mTable;
                sUnit->mAbbrevTableSize = AbbrevSet->mSize;
                return;
            }
            AbbrevSet = AbbrevSet->mNext;
        }
    }
    sUnit->mAbbrevTable = NULL;
    sUnit->mAbbrevTableSize = 0;
    str_exception(ERR_INV_DWARF, "Invalid abbreviation table offset");
}

void dio_ChkFlag(U2_T Form) {
    switch (Form) {
    case FORM_DATA1         :
    case FORM_FLAG          :
    case FORM_FLAG_PRESENT  :
        return;
    }
    str_exception(ERR_INV_DWARF, "FORM_FLAG expected");
}

void dio_ChkRef(U2_T Form) {
    switch (Form) {
    case FORM_REF       :
    case FORM_REF_ADDR  :
    case FORM_REF1      :
    case FORM_REF2      :
    case FORM_REF4      :
    case FORM_REF8      :
    case FORM_REF_UDATA :
        return;
    }
    str_exception(ERR_INV_DWARF, "FORM_REF expected");
}

void dio_ChkAddr(U2_T Form) {
    switch (Form) {
    case FORM_ADDR      :
        return;
    }
    str_exception(ERR_INV_DWARF, "FORM_ADDR expected");
}

void dio_ChkData(U2_T Form) {
    switch (Form) {
    case FORM_DATA1     :
    case FORM_DATA2     :
    case FORM_DATA4     :
    case FORM_DATA8     :
    case FORM_SDATA     :
    case FORM_UDATA     :
    case FORM_SEC_OFFSET:
        return;
    }
    str_exception(ERR_INV_DWARF, "FORM_DATA expected");
}

void dio_ChkBlock(U2_T Form, U1_T ** Buf, size_t * Size) {
    switch (Form) {
    case FORM_BLOCK1    :
    case FORM_BLOCK2    :
    case FORM_BLOCK4    :
    case FORM_BLOCK     :
    case FORM_DATA1     :
    case FORM_DATA2     :
    case FORM_DATA4     :
    case FORM_DATA8     :
    case FORM_EXPRLOC   :
    case FORM_SEC_OFFSET:
        assert(dio_gFormDataAddr >= sSection->data);
        assert((U1_T *)dio_gFormDataAddr < (U1_T *)sSection->data + sSection->size);
        *Size = dio_gFormDataSize;
        *Buf = (U1_T *)dio_gFormDataAddr;
        break;
    default:
        str_exception(ERR_INV_DWARF, "FORM_BLOCK expected");
    }
}

void dio_ChkString(U2_T Form) {
    if (Form == FORM_STRING) return;
    if (Form == FORM_STRP) return;
    if (Form == FORM_GNU_STRP_ALT) return;
    str_exception(ERR_INV_DWARF, "FORM_STRING expected");
}

#endif /* ENABLE_ELF */
