/*******************************************************************************
 * Copyright (c) 2006, 2014 Wind River Systems, Inc. and others.
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
#ifndef D_dwarfio
#define D_dwarfio

#include <tcf/config.h>

#if ENABLE_ELF

#include <tcf/services/tcf_elf.h>

typedef struct DIO_UnitDescriptor {
    ELF_Section * mSection;
    U2_T mVersion;
    U1_T m64bit;
    U1_T mAddressSize;
    U8_T mUnitOffs;
    U8_T mUnitSize;
    U8_T mAbbrevTableOffs;
    U8_T mTypeSignature;
    U8_T mTypeOffset;
    struct DIO_Abbreviation ** mAbbrevTable;
    U4_T mAbbrevTableSize;
} DIO_UnitDescriptor;

extern U8_T dio_gEntryPos;

extern U8_T dio_gFormData;
extern size_t dio_gFormDataSize;
extern void * dio_gFormDataAddr;
extern ELF_Section * dio_gFormSection;

extern void dio_EnterSection(DIO_UnitDescriptor * Unit, ELF_Section * Section, U8_T Offset);
extern void dio_ExitSection(void);

extern void dio_Skip(I8_T Bytes);
extern void dio_SetPos(U8_T Pos);
extern void dio_Read(U1_T * Buf, U4_T Size);
extern U8_T dio_GetPos(void); /* Offset in the section */
extern U1_T * dio_GetDataPtr(void);

extern U1_T dio_ReadU1(void);
extern U2_T dio_ReadU2(void);
extern U4_T dio_ReadU4(void);
extern U8_T dio_ReadU8(void);

extern U4_T dio_ReadULEB128(void);
extern I4_T dio_ReadSLEB128(void);
extern U8_T dio_ReadU8LEB128(void);
extern I8_T dio_ReadS8LEB128(void);

/*
 * Read link-time address value.
 * Relocation tables are used if necessary to compute the address value.
 * For a file that can be relocated at run-time, 's' is assigned a section that the address points to.
 */
extern U8_T dio_ReadAddressX(ELF_Section ** s, int Size);
extern U8_T dio_ReadAddress(ELF_Section ** s);

extern char * dio_ReadString(void);

extern void dio_ReadAttribute(U2_T Attr, U2_T Form);

typedef void (*DIO_EntryCallBack)(U2_T /* Tag */, U2_T /* Attr */, U2_T /* Form */);
/*
 * CallBack is called before each DWARF entry with Atrr = 0 and Form != 0,
 * then is is called for each entry attribute with appropriate Attr and Form values,
 * and then called after the entry with Attr = 0 and Form = 0.
 * This sequence is repeated for each entry in the debug info unit.
 */
extern void dio_ReadUnit(DIO_UnitDescriptor * Unit, DIO_EntryCallBack CallBack);
extern int dio_ReadEntry(DIO_EntryCallBack CallBack, U2_T Attr);

/* CallBack 'Form' value on unit entry: */
#define DWARF_ENTRY_NO_CHILDREN     2
#define DWARF_ENTRY_HAS_CHILDREN    3

extern void dio_LoadAbbrevTable(ELF_File * File);

extern void dio_ChkFlag(U2_T Form);
extern void dio_ChkRef(U2_T Form);
extern void dio_ChkAddr(U2_T Form);
extern void dio_ChkData(U2_T Form);
extern void dio_ChkBlock(U2_T Form, U1_T ** Buf, size_t * Size);
extern void dio_ChkString(U2_T Form);

#endif /* ENABLE_ELF */

#endif /* D_dwarfio */
