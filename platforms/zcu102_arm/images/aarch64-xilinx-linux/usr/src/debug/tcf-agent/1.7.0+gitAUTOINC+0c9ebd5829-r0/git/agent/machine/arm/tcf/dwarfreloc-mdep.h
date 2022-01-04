/*******************************************************************************
 * Copyright (c) 2013, 2014 Xilinx, Inc. and others.
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
 * This module provides CPU specific ELF definitions for ARM.
 */

#define R_ARM_NONE      0
#define R_ARM_ABS32     2
#define R_ARM_REL32     3
#define R_ARM_LDR_PC_G0 4
#define R_ARM_ABS16     5

static void elf_relocate(void) {
    if (relocs->type == SHT_REL && reloc_type != R_ARM_NONE) {
        if (section->file->type != ET_REL) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        assert(reloc_addend == 0);
        switch (reloc_type) {
        case R_ARM_ABS32:
        case R_ARM_REL32:
        case R_ARM_LDR_PC_G0:
            {
                U4_T x = *(U4_T *)((char *)section->data + reloc_offset);
                if (section->file->byte_swap) SWAP(x);
                reloc_addend = x;
            }
            break;
        case R_ARM_ABS16:
            {
                U2_T x = *(U2_T *)((char *)section->data + reloc_offset);
                if (section->file->byte_swap) SWAP(x);
                reloc_addend = x;
            }
            break;
        default:
            str_exception(ERR_INV_FORMAT, "Unsupported relocation type");
        }
    }
    switch (reloc_type) {
    case R_ARM_NONE:
        *destination_section = NULL;
        break;
    case R_ARM_ABS32:
        if (data_size < 4) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *(U4_T *)data_buf = (U4_T)(sym_value + reloc_addend);
        break;
    case R_ARM_REL32:
    case R_ARM_LDR_PC_G0:
        if (data_size < 4) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *(U4_T *)data_buf = (U4_T)(sym_value + reloc_addend - (section->addr + reloc_offset));
        break;
    case R_ARM_ABS16:
        if (data_size < 2) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
        *(U2_T *)data_buf = (U2_T)(sym_value + reloc_addend);
        break;
    default:
        str_exception(ERR_INV_FORMAT, "Unsupported relocation type");
    }
}
