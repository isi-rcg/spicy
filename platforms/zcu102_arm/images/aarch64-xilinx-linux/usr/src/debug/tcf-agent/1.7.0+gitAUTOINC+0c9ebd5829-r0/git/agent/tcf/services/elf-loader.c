/*******************************************************************************
 * Copyright (c) 2011-2019 Wind River Systems, Inc. and others.
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
 * This module implements access to ELF dynamic loader data.
 */

#include <tcf/config.h>

#if ENABLE_ELF && ENABLE_DebugContext

#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/symbols.h>
#include <tcf/services/dwarfcache.h>
#include <tcf/services/elf-loader.h>

static ContextAddress to_address(uint8_t * buf, size_t size, int big_endian) {
    size_t i;
    ContextAddress addr = 0;
    for (i = 0; i < size; i++) {
        addr = addr << 8;
        addr |= buf[big_endian ? i : size - i - 1];
    }
    return addr;
}

static int get_dynamic_tag(Context * ctx, ELF_File * file, int tag, ContextAddress * addr) {
    unsigned i, j;

    for (i = 1; i < file->section_cnt; i++) {
        ELF_Section * sec = file->sections + i;
        if (sec->size == 0) continue;
        if (sec->name == NULL) continue;
        if (strcmp(sec->name, ".dynamic") == 0) {
            ContextAddress sec_addr = elf_map_to_run_time_address(ctx, file, sec, (ContextAddress)sec->addr);
            if (errno) return -1;
            if (elf_load(sec) < 0) return -1;
            if (file->elf64) {
                unsigned cnt = (unsigned)(sec->size / sizeof(Elf64_Dyn));
                for (j = 0; j < cnt; j++) {
                    Elf64_Dyn dyn = *((Elf64_Dyn *)sec->data + j);
                    if (file->byte_swap) SWAP(dyn.d_tag);
                    if (dyn.d_tag == DT_NULL) break;
                    if (dyn.d_tag == tag) {
                        if (context_read_mem(ctx, sec_addr + j * sizeof(dyn), &dyn, sizeof(dyn)) < 0) return -1;
                        if (file->byte_swap) {
                            SWAP(dyn.d_tag);
                            SWAP(dyn.d_un.d_ptr);
                        }
                        if (dyn.d_tag != tag) continue;
                        if (addr != NULL) *addr = (ContextAddress)dyn.d_un.d_ptr;
                        return 0;
                    }
                }
            }
            else {
                unsigned cnt = (unsigned)(sec->size / sizeof(Elf32_Dyn));
                for (j = 0; j < cnt; j++) {
                    Elf32_Dyn dyn = *((Elf32_Dyn *)sec->data + j);
                    if (file->byte_swap) SWAP(dyn.d_tag);
                    if (dyn.d_tag == DT_NULL) break;
                    if (dyn.d_tag == tag) {
                        if (context_read_mem(ctx, sec_addr + j * sizeof(dyn), &dyn, sizeof(dyn)) < 0) return -1;
                        if (file->byte_swap) {
                            SWAP(dyn.d_tag);
                            SWAP(dyn.d_un.d_ptr);
                        }
                        if (dyn.d_tag != tag) continue;
                        if (addr != NULL) *addr = (ContextAddress)dyn.d_un.d_ptr;
                        return 0;
                    }
                }
            }
        }
    }
    errno = ENOENT;
    return -1;
}

static int sym_name_cmp(const char * x, const char * y) {
    while (*x && *x == *y) {
        x++;
        y++;
    }
    if (*x == 0 && *y == 0) return 0;
    if (*x == '@' && *(x + 1) == '@' && *y == 0) return 0;
    if (*x < *y) return -1;
    return 1;
}

static int get_global_symbol_address(Context * ctx, ELF_File * file, const char * name, ContextAddress * addr) {
    unsigned i, j;

    for (i = 1; i < file->section_cnt; i++) {
        ELF_Section * sec = file->sections + i;
        if (sec->size == 0) continue;
        if (sec->type == SHT_SYMTAB) {
            ELF_Section * str = NULL;
            if (sec->link == 0 || sec->link >= file->section_cnt) {
                errno = EINVAL;
                return -1;
            }
            str = file->sections + sec->link;
            if (elf_load(sec) < 0) return -1;
            if (elf_load(str) < 0) return -1;
            if (file->elf64) {
                unsigned cnt = (unsigned)(sec->size / sizeof(Elf64_Sym));
                for (j = 0; j < cnt; j++) {
                    Elf64_Sym sym = *((Elf64_Sym *)sec->data + j);
                    if (ELF64_ST_BIND(sym.st_info) != STB_GLOBAL) continue;
                    if (file->byte_swap) SWAP(sym.st_name);
                    if (sym_name_cmp((char *)str->data + sym.st_name, name) != 0) continue;
                    switch (ELF64_ST_TYPE(sym.st_info)) {
                    case STT_NOTYPE:
                    case STT_OBJECT:
                    case STT_FUNC:
                        if (file->byte_swap) SWAP(sym.st_value);
                        *addr = elf_map_to_run_time_address(ctx, file, NULL, (ContextAddress)sym.st_value);
                        if (errno == 0) return 0;
                    }
                }
            }
            else {
                unsigned cnt = (unsigned)(sec->size / sizeof(Elf32_Sym));
                for (j = 0; j < cnt; j++) {
                    Elf32_Sym sym = *((Elf32_Sym *)sec->data + j);
                    if (ELF32_ST_BIND(sym.st_info) != STB_GLOBAL) continue;
                    if (file->byte_swap) SWAP(sym.st_name);
                    if (sym_name_cmp((char *)str->data + sym.st_name, name) != 0) continue;
                    switch (ELF32_ST_TYPE(sym.st_info)) {
                    case STT_NOTYPE:
                    case STT_OBJECT:
                    case STT_FUNC:
                        if (file->byte_swap) SWAP(sym.st_value);
                        *addr = elf_map_to_run_time_address(ctx, file, NULL, (ContextAddress)sym.st_value);
                        if (errno == 0) return 0;
                    }
                }
            }
        }
    }
    errno = ENOENT;
    return -1;
}

ContextAddress elf_get_debug_structure_address(Context * ctx, ELF_File ** file_ptr) {
    ELF_File * file = NULL;
    ContextAddress addr = 0;

    for (file = elf_list_first(ctx, 0, ~(ContextAddress)0); file != NULL; file = elf_list_next(ctx)) {
        if (file->type != ET_EXEC) {
            /* Check for PIE executable */
            ContextAddress flags = 0;
            if (file->type != ET_DYN) continue;
            if (get_dynamic_tag(ctx, file, DT_FLAGS_1, &flags) != 0) continue;
            if ((flags & DF_1_PIE) == 0) continue;
        }
        if (file_ptr != NULL) *file_ptr = file;
#ifdef DT_MIPS_RLD_MAP
        if (get_dynamic_tag(ctx, file, DT_MIPS_RLD_MAP, &addr) == 0) {
            if (elf_read_memory_word(ctx, file, addr, &addr) < 0) continue;
            break;
        }
#endif
        if (get_dynamic_tag(ctx, file, DT_DEBUG, &addr) == 0) break;
        if (get_global_symbol_address(ctx, file, "_r_debug", &addr) == 0) break;
    }
    elf_list_done(ctx);

    return addr;
}

#if ENABLE_Symbols
static void read_field(Context * ctx, const Symbol * sym, ContextAddress base, ContextAddress * value) {
    LocationInfo * loc_info = NULL;
    LocationExpressionState * state = NULL;
    uint64_t args[1];
    void * buf = NULL;
    size_t size = 0;
    size_t i;

    args[0] = base;
    if (get_location_info(sym, &loc_info) < 0) exception(errno);
    if (loc_info->args_cnt != 1) str_exception(ERR_OTHER, "Wrong object kind");
    state = evaluate_location_expression(ctx, NULL,
        loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, args, 1);
    if (state->pieces_cnt > 0) {
        read_location_pieces(state->ctx, state->stack_frame,
            state->pieces, state->pieces_cnt, loc_info->big_endian, &buf, &size);
    }
    else {
        ContextAddress sym_size = 0;
        if (state->stk_pos != 1) str_exception(ERR_OTHER, "Invalid location expression");
        if (get_symbol_size(sym, &sym_size) < 0) exception(errno);
        size = (size_t)sym_size;
        buf = tmp_alloc(size);
        if (context_read_mem(state->ctx, (ContextAddress)state->stk[0], buf, size) < 0) exception(errno);
    }
    *value = 0;
    for (i = 0; i < size && i < sizeof(ContextAddress); i++) {
        *value = *value << 8;
        *value |= ((uint8_t *)buf)[loc_info->big_endian ? i : size - i - 1];
    }
}
#endif

static ContextAddress find_module(Context * ctx, ELF_File * exe_file, ELF_File * module,
                                  ContextAddress r_map, ContextAddress r_brk, ContextAddress * tls_offs) {
#if ENABLE_Symbols
    Symbol * sym = NULL;
    int i = 0, n = 0;
    Symbol ** children = NULL;
    ContextAddress link = r_map;
    Symbol * sym_l_addr = NULL;
    Symbol * sym_l_next = NULL;
    Symbol * sym_l_tls_modid = NULL;
    Symbol * sym_l_tls_offset = NULL;
    if (find_symbol_by_name(ctx, STACK_NO_FRAME, r_brk, "link_map", &sym) < 0)
        str_exception(errno, "Cannot find loader symbol: link_map");
    if (get_symbol_children(sym, &children, &n) < 0) exception(errno);
    for (i = 0; i < n; i++) {
        char * name = NULL;
        if (get_symbol_name(children[i], &name) < 0) exception(errno);
        if (name == NULL) continue;
        if (strcmp(name, "l_map_start") == 0) sym_l_addr = children[i];
        else if (strcmp(name, "l_next") == 0) sym_l_next = children[i];
        else if (strcmp(name, "l_tls_modid") == 0) sym_l_tls_modid = children[i];
        else if (strcmp(name, "l_tls_offset") == 0) sym_l_tls_offset = children[i];
    }
    if (sym_l_addr == NULL || sym_l_next == NULL || sym_l_tls_modid == NULL)
        str_exception(ERR_OTHER, "Invalid 'link_map' fields");
    while (link != 0) {
        ContextAddress l_tls_modid = 0;
        read_field(ctx, sym_l_tls_modid, link, &l_tls_modid);
        if (l_tls_modid != 0) {
            ContextAddress l_addr = 0;
            ELF_File * link_file = NULL;
            read_field(ctx, sym_l_addr, link, &l_addr);
            elf_map_to_link_time_address(ctx, l_addr, 0, &link_file, NULL);
            if (link_file != NULL) {
                if (link_file == module || get_dwarf_file(link_file) == module) {
                    if (sym_l_tls_offset != NULL) read_field(ctx, sym_l_tls_offset, link, tls_offs);
                    return l_tls_modid;
                }
            }
        }
        read_field(ctx, sym_l_next, link, &link);
    }
#endif
    return 0;
}

static ContextAddress get_module_id(Context * ctx, ELF_File * module, ContextAddress * tls_offs) {
    ELF_File * exe_file = NULL;
    ContextAddress addr = elf_get_debug_structure_address(ctx, &exe_file);
    size_t word_size = exe_file && exe_file->elf64 ? 8 : 4;
    Trap trap;

    if (addr == 0 || exe_file == NULL) str_exception(ERR_OTHER, "Cannot find loader debug data");
    if (set_trap(&trap)) {
        ContextAddress r_map = 0;
        ContextAddress r_brk = 0;
        ContextAddress mod_id = 0;
        if (elf_read_memory_word(ctx, exe_file, addr + word_size * 1, &r_map) < 0) exception(errno);
        if (elf_read_memory_word(ctx, exe_file, addr + word_size * 2, &r_brk) < 0) exception(errno);
        if (r_map != 0 && r_brk != 0) mod_id = find_module(ctx, exe_file, module, r_map, r_brk, tls_offs);
        clear_trap(&trap);
        if (mod_id) return mod_id;
    }
    else {
        str_exception(trap.error, "Cannot access target ELF loader data");
    }
    str_exception(ERR_OTHER, "Cannot get TLS module ID");
    return 0;
}

ContextAddress get_tls_address(Context * ctx, ELF_File * file) {

    /* Note: handling TLS needs libc debug info on the target machine (apt install libc6-dbg) */

    uint8_t buf[8];
    ContextAddress mod_tls_addr = 0;
    RegisterIdScope reg_id_scope;
    ContextAddress tcb_addr = 0;
    ContextAddress tls_addr = 0;
    ContextAddress dtv_addr = 0;  /* Address of Dynamic Thread Vector */
    ContextAddress tls_offs = 0;
    ContextAddress mod_id = get_module_id(ctx, file, &tls_offs);
    RegisterDefinition * reg_def = NULL;

    memset(&reg_id_scope, 0, sizeof(reg_id_scope));
    reg_id_scope.machine = file->machine;
    reg_id_scope.os_abi = file->os_abi;
    reg_id_scope.elf64 = file->elf64;
    reg_id_scope.big_endian = file->big_endian;
    reg_id_scope.id_type = REGNUM_DWARF;

    /* Element type of the DTV:
    typedef union {
        size_t counter;
        struct {
            void * val;
            void * to_free;
        } pointer;
    } dtv_t;

    val == (void *) -1l means allocation is delayed
    */

    switch (file->machine) {
    case EM_386:
    case EM_X86_64:
        reg_def = get_reg_by_id(ctx, 58, &reg_id_scope);
        if (reg_def == NULL) exception(errno);
        if (context_read_reg(ctx, reg_def, 0, reg_def->size, buf) < 0)
            str_exception(errno, "Cannot read TCB base register");
        tcb_addr = to_address(buf, reg_def->size, reg_def->big_endian);
        if (elf_read_memory_word(ctx, file, tcb_addr + (file->elf64 ? 8 : 4), &dtv_addr) < 0)
            str_exception(errno, "Cannot read TCB");
        break;
    case EM_ARM:
    case EM_AARCH64:
        reg_def = get_reg_definitions(ctx);
        if (reg_def == NULL) str_exception(ERR_OTHER, "TLS register not found");
        while (reg_def->name != NULL && strcmp(reg_def->name, "tls") != 0) reg_def++;
        if (reg_def->name == NULL) str_exception(ERR_OTHER, "TLS register not found");
        if (context_read_reg(ctx, reg_def, 0, reg_def->size, buf) < 0)
            str_exception(errno, "Cannot read TLS register");
        tls_addr = to_address(buf, reg_def->size, reg_def->big_endian);
        if (elf_read_memory_word(ctx, file, tls_addr, &dtv_addr) < 0)
            str_exception(errno, "Cannot read TLS");
        break;
    case EM_RISCV:
        reg_def = get_reg_by_id(ctx, 4, &reg_id_scope);
        if (reg_def == NULL) exception(errno);
        if (context_read_reg(ctx, reg_def, 0, reg_def->size, buf) < 0)
            str_exception(errno, "Cannot read TCB base register");
        tcb_addr = to_address(buf, reg_def->size, reg_def->big_endian);
        if (elf_read_memory_word(ctx, file, tcb_addr - (file->elf64 ? 16 : 8), &dtv_addr) < 0)
            str_exception(errno, "Cannot read TCB");
        break;
    case EM_PPC:
    case EM_PPC64:
        reg_def = get_reg_by_id(ctx, (file->elf64 ? 13 : 2), &reg_id_scope);
        if (reg_def == NULL) exception(errno);
        if (context_read_reg(ctx, reg_def, 0, reg_def->size, buf) < 0)
            str_exception(errno, "Cannot read TCB base register");
        tcb_addr = to_address(buf, reg_def->size, reg_def->big_endian) - 0x7000;
        if (elf_read_memory_word(ctx, file, tcb_addr - (file->elf64 ? 8 : 4), &dtv_addr) < 0)
            str_exception(errno, "Cannot read TCB");
        break;
    }
    if (dtv_addr == 0) {
        str_fmt_exception(ERR_INV_CONTEXT,
            "Thread local storage access is not supported yet for machine type %d",
            file->machine);
    }
    if (elf_read_memory_word(ctx, file, dtv_addr + mod_id * (file->elf64 ? 16 : 8), &mod_tls_addr) < 0)
        str_exception(errno, "Cannot read DTV");
    if (mod_tls_addr == ~(ContextAddress)0 && tls_offs != 0 && tls_offs != ~(ContextAddress)0) {
        mod_tls_addr = tcb_addr ? tcb_addr - tls_offs : tls_addr + tls_offs;
    }
    if (mod_tls_addr == 0 || mod_tls_addr == ~(ContextAddress)0)
        str_exception(ERR_OTHER, "Thread local storage is not allocated yet");
    return mod_tls_addr;
}

#endif
