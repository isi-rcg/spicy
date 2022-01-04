/*******************************************************************************
 * Copyright (c) 2013, 2017 Xilinx, Inc. and others.
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
 *     Xilinx - initial API and implementation
 *******************************************************************************/

/*
 * This module provides CPU specific ELF definitions for MicroBlaze.
 */

#define R_MICROBLAZE_NONE           0
#define R_MICROBLAZE_32             1
#define R_MICROBLAZE_32_PCREL       2
#define R_MICROBLAZE_64_PCREL       3
#define R_MICROBLAZE_32_PCREL_LO    4
#define R_MICROBLAZE_64             5
#define R_MICROBLAZE_32_LO          6
#define R_MICROBLAZE_SRO32          7
#define R_MICROBLAZE_SRW32          8
#define R_MICROBLAZE_64_NONE        9
#define R_MICROBLAZE_32_SYM_OP_SYM 10
#define R_MICROBLAZE_32_NONE       30

static void elf_relocate(void) {
    if (relocs->type == SHT_REL) {
        U4_T x = *(U4_T *)((char *)section->data + reloc_offset);
        if (section->file->type != ET_REL) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        if (section->file->byte_swap) SWAP(x);
        assert(reloc_addend == 0);
        reloc_addend = x;
    }
    switch (reloc_type) {
    case R_MICROBLAZE_NONE:
        *destination_section = NULL;
        break;
    case R_MICROBLAZE_32_NONE:
        if (data_size < 4) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *destination_section = NULL;
        break;
    case R_MICROBLAZE_64_NONE:
        if (data_size < 8) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *destination_section = NULL;
        break;
    case R_MICROBLAZE_32:
        if (data_size < 4) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *(U4_T *)data_buf = (U4_T)(sym_value + reloc_addend);
        break;
    case R_MICROBLAZE_32_PCREL:
        if (data_size < 4) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *(U4_T *)data_buf = (U4_T)(sym_value + reloc_addend - (section->addr + reloc_offset));
        break;
    default:
        str_exception(ERR_INV_FORMAT, "Unsupported relocation type");
    }
}
