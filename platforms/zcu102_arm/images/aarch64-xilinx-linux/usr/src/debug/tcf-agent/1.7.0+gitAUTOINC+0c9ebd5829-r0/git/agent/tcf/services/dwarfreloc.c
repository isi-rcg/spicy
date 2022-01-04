/*******************************************************************************
 * Copyright (c) 2010-2019 Wind River Systems, Inc. and others.
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
 * This module implements ELF relocation records handling for reading DWARF debug information.
 *
 * Functions in this module use exceptions to report errors, see exceptions.h
 */

#include <tcf/config.h>

#if ENABLE_ELF

#include <assert.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/services/elf-symbols.h>
#include <tcf/services/dwarfreloc.h>

static Context * ctx = NULL;
static ELF_Section * section = NULL;
static ELF_Section * relocs = NULL;
static ELF_Section * symbols = NULL;
static ELF_Section ** destination_section = NULL;
static int * run_time_addr = NULL;

static U8_T reloc_offset = 0;
static U4_T reloc_type = 0;
static U8_T reloc_addend = 0;
static U4_T sym_index = 0;
static U8_T sym_value = 0;

static void * data_buf = NULL;
static size_t data_size = 0;

typedef struct ElfRelocateFunc {
    int machine;
    void (*func)(void);
} ElfRelocateFunc;

#include <tcf/services/dwarfreloc-ext.h>

static void relocate(void * r) {
    ElfRelocateFunc * func;
    if (!relocs->file->elf64) {
        if (relocs->type == SHT_REL) {
            Elf32_Rel bf = *(Elf32_Rel *)r;
            if (relocs->file->byte_swap) {
                SWAP(bf.r_offset);
                SWAP(bf.r_info);
            }
            sym_index = ELF32_R_SYM(bf.r_info);
            reloc_type = ELF32_R_TYPE(bf.r_info);
            reloc_addend = 0;
        }
        else {
            Elf32_Rela bf = *(Elf32_Rela *)r;
            if (relocs->file->byte_swap) {
                SWAP(bf.r_offset);
                SWAP(bf.r_info);
                SWAP(bf.r_addend);
            }
            sym_index = ELF32_R_SYM(bf.r_info);
            reloc_type = ELF32_R_TYPE(bf.r_info);
            reloc_addend = bf.r_addend;
        }
    }
    else {
        if (relocs->type == SHT_REL) {
            Elf64_Rel bf = *(Elf64_Rel *)r;
            if (relocs->file->byte_swap) {
                SWAP(bf.r_offset);
                SWAP(bf.r_info);
            }
            sym_index = ELF64_R_SYM(bf.r_info);
            reloc_type = ELF64_R_TYPE(bf.r_info);
            reloc_addend = 0;
        }
        else {
            Elf64_Rela bf = *(Elf64_Rela *)r;
            if (relocs->file->byte_swap) {
                SWAP(bf.r_offset);
                SWAP(bf.r_info);
                SWAP(bf.r_addend);
            }
            sym_index = ELF64_R_SYM(bf.r_info);
            reloc_type = ELF64_R_TYPE(bf.r_info);
            reloc_addend = bf.r_addend;
        }
    }

    if (sym_index != STN_UNDEF) {
        ELF_SymbolInfo info;
        unpack_elf_symbol_info(symbols, sym_index, &info);
        switch (info.section_index) {
        case SHN_ABS:
            sym_value = info.value;
            break;
        case SHN_COMMON:
#if SERVICE_Symbols && (!ENABLE_SymbolsProxy || ENABLE_SymbolsMux)
            if (ctx != NULL) {
                ContextAddress addr = 0;
                if (elf_symbol_address(ctx, &info, &addr) < 0) exception(errno);
                sym_value = addr;
                if (run_time_addr) *run_time_addr = 1;
                break;
            }
#endif
            str_exception(ERR_INV_FORMAT, "Common relocation record unsupported");
            break;
        case SHN_UNDEF:
            str_exception(ERR_INV_FORMAT, "Invalid relocation record");
            break;
        default:
            if (info.section == NULL) str_exception(ERR_INV_FORMAT, "Invalid relocation record");
            if (symbols->file->type != ET_EXEC) {
                sym_value = info.section->addr + info.value;
            }
            else {
                sym_value = info.value;
            }
            *destination_section = info.section;
            break;
        }
    }

    /* For executable file we don't need to apply the relocation,
     * all we need is destination_section */
    if (section->file->type != ET_REL) return;

    func = elf_relocate_funcs;
    while (func->machine != section->file->machine) {
        if (func->func == NULL) str_exception(ERR_INV_FORMAT, "Unsupported ELF machine code");
        func++;
    }
    func->func();
}

static U4_T get_offs4(uint8_t * x) {
    U4_T offs = *(U4_T *)x;
    if (relocs->file->byte_swap) SWAP(offs);
    if (section->file->type != ET_REL) offs -= (U4_T)section->addr;
    return offs;
}

static U8_T get_offs8(uint8_t * x) {
    U8_T offs = *(U8_T *)x;
    if (relocs->file->byte_swap) SWAP(offs);
    if (section->file->type != ET_REL) offs -= section->addr;
    return offs;
}

void drl_relocate_in_context(Context * c, ELF_Section * s, U8_T offset,
                             void * buf, size_t size, ELF_Section ** dst, int * rt_addr) {
    ELF_Section * r = s->relocate;
    ELF_Section * d = NULL;
    unsigned ix;

    if (rt_addr) *rt_addr = 0;
    if (dst == NULL) dst = &d;
    else *dst = NULL;
    if (r == NULL) return;

    ctx = c;
    section = s;
    relocs = r;
    destination_section = dst;
    reloc_offset = offset;
    data_buf = buf;
    data_size = size;
    run_time_addr = rt_addr;
    symbols = s->file->sections + r->link;
    if (elf_load(relocs) < 0) exception(errno);
    if (elf_load(symbols) < 0) exception(errno);
    if (r->entsize == 0 || r->size % r->entsize != 0) str_exception(ERR_INV_FORMAT, "Invalid sh_entsize");

    if (r->reloc_num_zones == 0) {
        U8_T prev_offs = 0;
        unsigned max_bondaries = 2; /* default is two bondaries ... */
        r->reloc_num_zones = 1; /* ... for one zone */
        r->reloc_zones_bondaries = (unsigned *)loc_alloc_zero(sizeof (unsigned) * max_bondaries);
        r->reloc_zones_bondaries[0] = 0;  /* first zone starting index */
        for (ix = 0; ix < r->size / r->entsize; ix++) {
            U8_T offs;
            uint8_t * x = (uint8_t *)r->data + ix * r->entsize;
            if (r->file->elf64) {
                offs = *(U8_T *)x;
                if (r->file->byte_swap) SWAP(offs);
            }
            else {
                U4_T offs4 = *(U4_T *)x;
                if (r->file->byte_swap) SWAP(offs4);
                offs = offs4;
            }
            if (offs < prev_offs) {
                /*
                 * Relocation offsets are not ordered. Store the start
                 * index of the new zone.
                 */
                if ((r->reloc_num_zones + 1) == max_bondaries) {
                    max_bondaries += 5;
                    r->reloc_zones_bondaries =
                        (unsigned *)loc_realloc(r->reloc_zones_bondaries,
                        sizeof (unsigned) * max_bondaries);
                }
                r->reloc_zones_bondaries[r->reloc_num_zones++] = ix;
            }
            prev_offs = offs;
        }
        /* Store the last zone boundary index */
        r->reloc_zones_bondaries[r->reloc_num_zones] = ix;
        if (r->reloc_num_zones > 1) {
            trace(LOG_ELF, "ELF relocations are not ordered; the performances "\
                           "may be degraded.");
        }
        /*
         * As we parsed the relocation section, it would be possible to
         * localize the searched offset at the same time, to optimize the
         * first lookup. But I don't know if it worth it, compare to some
         * code duplication with the rest of the routine below.
         */
    }

    /* Perform a dichotomic look up for each ordered area */
    for (ix = 0; ix < r->reloc_num_zones; ix++) {
        uint8_t * p = (uint8_t *)r->data + r->reloc_zones_bondaries[ix] * r->entsize;
        uint8_t * q = (uint8_t *)r->data + r->reloc_zones_bondaries[ix + 1] * r->entsize;
        while (p < q) {
            unsigned n = (q - p) / r->entsize / 2;
            uint8_t * x = p + n * r->entsize;
            assert(x < q);
            if (r->file->elf64) {
                U8_T offs = get_offs8(x);
                if (offset > offs) {
                    p = x + r->entsize;
                    continue;
                }
                if (offset < offs) {
                    q = x;
                    continue;
                }
                {
                    /* ELF can have multiple relocations at same offset */
                    uint8_t * x0 = x;
                    uint8_t * x1 = x;
                    while (x0 > p) {
                        uint8_t * y = x0 - r->entsize;
                        U8_T z = get_offs8(y);
                        assert(z <= offs);
                        if (z != offs) break;
                        x0 = y;
                    }
                    while (x1 + r->entsize < q) {
                        uint8_t * y = x1 + r->entsize;
                        U8_T z = get_offs8(y);
                        assert(z >= offs);
                        if (z != offs) break;
                        x1 = y;
                    }
                    while (x0 <= x1) {
                        relocate(x0);
                        x0 += r->entsize;
                    }
                }
                return;
            }
            else {
                U4_T offs = *(U4_T *)x;
                if (r->file->byte_swap) SWAP(offs);
                if (s->file->type != ET_REL) offs -= (U4_T)s->addr;
                if (offset > offs) {
                    p = x + r->entsize;
                    continue;
                }
                if (offset < offs) {
                    q = x;
                    continue;
                }
                {
                    /* ELF can have multiple relocations at same offset */
                    uint8_t * x0 = x;
                    uint8_t * x1 = x;
                    while (x0 > p) {
                        uint8_t * y = x0 - r->entsize;
                        U4_T z = get_offs4(y);
                        assert(z <= offs);
                        if (z != offs) break;
                        x0 = y;
                    }
                    while (x1 + r->entsize < q) {
                        uint8_t * y = x1 + r->entsize;
                        U4_T z = get_offs4(y);
                        assert(z >= offs);
                        if (z != offs) break;
                        x1 = y;
                    }
                    while (x0 <= x1) {
                        relocate(x0);
                        x0 += r->entsize;
                    }
                }
                return;
            }
        }
    }
}

void drl_relocate(ELF_Section * s, U8_T offset, void * buf, size_t size, ELF_Section ** dst) {
    drl_relocate_in_context(NULL, s, offset, buf, size, dst, NULL);
}

#endif /* ENABLE_ELF */
