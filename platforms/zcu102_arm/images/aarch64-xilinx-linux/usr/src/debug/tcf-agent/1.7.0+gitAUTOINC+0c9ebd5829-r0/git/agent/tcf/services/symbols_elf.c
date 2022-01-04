/*******************************************************************************
 * Copyright (c) 2007-2019 Wind River Systems, Inc. and others.
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
 * Symbols service - ELF version.
 */

#include <tcf/config.h>

#if SERVICE_Symbols && (!ENABLE_SymbolsProxy || ENABLE_SymbolsMux) && ENABLE_ELF

#if defined(_WRS_KERNEL)
#  include <symLib.h>
#  include <sysSymTbl.h>
#endif
#include <errno.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/tcf_elf.h>
#include <tcf/services/dwarf.h>
#include <tcf/services/dwarfcache.h>
#include <tcf/services/dwarfexpr.h>
#include <tcf/services/dwarfecomp.h>
#include <tcf/services/dwarfframe.h>
#include <tcf/services/dwarfreloc.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/funccall.h>
#include <tcf/services/pathmap.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/symbols.h>
#include <tcf/services/elf-symbols.h>
#include <tcf/services/elf-loader.h>
#include <tcf/services/vm.h>
#if ENABLE_SymbolsMux
#define SYM_READER_PREFIX elf_reader_
#include <tcf/services/symbols_mux.h>
#endif

struct Symbol {
#if ENABLE_SymbolsMux
    SymbolReader * reader;
#endif
    unsigned magic;
    ObjectInfo * obj;
    ObjectInfo * var; /* 'this' object if the symbol represents implicit 'this' reference */
    ObjectInfo * ref; /* reference object for variable types */
    ELF_Section * tbl;
    int8_t sym_class;
    int8_t has_address;
    int8_t has_location;
    int8_t priority;
    int8_t dynsym;
    int8_t weak;
    int8_t assembly_function;
    ContextAddress address;
    ContextAddress size;
    Context * ctx;
    int frame;
    unsigned index;
    unsigned dimension;
    unsigned cardinal;
    ContextAddress length;
    Symbol * base;
    /* Volatile fields, used for sorting */
    Symbol * next;
    unsigned level;
    unsigned pos;
};

#define is_array_type_pseudo_symbol(s) (s->sym_class == SYM_CLASS_TYPE && s->obj == NULL && s->base != NULL)
#define is_std_type_pseudo_symbol(s) (s->sym_class == SYM_CLASS_TYPE && s->obj == NULL && s->base == NULL)
#define is_constant_pseudo_symbol(s) (s->sym_class == SYM_CLASS_VALUE && s->obj == NULL && s->base != NULL)
#define is_pointer_pseudo_symbol(s) (s->sym_class == SYM_CLASS_VALUE && s->obj == NULL && s->base == NULL && s->has_address)

static Context * sym_ctx;
static int sym_frame;
static ContextAddress sym_ip;
static Symbol * find_symbol_list = NULL;

typedef struct {
    ELF_File * file;
    ELF_Section * section;
    ContextAddress lt_addr;
    ContextAddress rt_addr;
    CompUnit * unit;
} UnitAddress;

typedef long ConstantValueType;

static struct ConstantPseudoSymbol {
    const char * name;
    const char * type;
    ConstantValueType value;
} constant_pseudo_symbols[] = {
    { "false", "bool", 0 },
    { "true", "bool", 1 },
    { "false", "boolean", 0 },
    { "true", "boolean", 1 },
    { NULL },
};

static struct TypePseudoSymbol {
    const char * name;
    unsigned size;
    unsigned type_class;
} type_pseudo_symbols[] = {
    { "void",      0, TYPE_CLASS_CARDINAL },
    { "uint8_t",   1, TYPE_CLASS_CARDINAL },
    { "uint16_t",  2, TYPE_CLASS_CARDINAL },
    { "uint32_t",  4, TYPE_CLASS_CARDINAL },
    { "uint64_t",  8, TYPE_CLASS_CARDINAL },
    { "int8_t",    1, TYPE_CLASS_INTEGER },
    { "int16_t",   2, TYPE_CLASS_INTEGER },
    { "int32_t",   4, TYPE_CLASS_INTEGER },
    { "int64_t",   8, TYPE_CLASS_INTEGER },
    { "char16_t",  2, TYPE_CLASS_CARDINAL },
    { "char32_t",  4, TYPE_CLASS_CARDINAL },
    { "float32_t", 4, TYPE_CLASS_REAL },
    { "float64_t", 8, TYPE_CLASS_REAL },
    { NULL },
};

static struct BaseTypeAlias {
    const char * name;
    const char * alias;
} base_types_aliases[] = {
    { "int", "signed int" },
    { "signed", "int" },
    { "signed int", "int" },
    { "unsigned", "unsigned int" },
    { "short", "short int" },
    { "signed short", "short int" },
    { "signed short int", "short int" },
    { "unsigned short", "unsigned short int" },
    { "long", "long int" },
    { "signed long", "long int" },
    { "signed long int", "long int" },
    { "unsigned long", "unsigned long int" },
    { "unsigned long", "long unsigned int" },
    { "long long", "long long int" },
    { "signed long long", "long long int" },
    { "signed long long int", "long long int" },
    { "unsigned long long", "unsigned long long int" },
    { "unsigned long long", "long long unsigned int" },
    { NULL, NULL }
};

#define SYMBOL_MAGIC 0x34875234

#define equ_symbol_names(x, y) (*x == *y && cmp_symbol_names(x, y) == 0)
#define check_in_range(obj, addr) dwarf_check_in_range(obj, (addr)->section, (addr)->lt_addr)

/* This function is used for DWARF reader testing */
extern ObjectInfo * get_symbol_object(Symbol * sym);
ObjectInfo * get_symbol_object(Symbol * sym) {
    return sym->obj;
}

int elf_save_symbols_state(ELFSymbolsRecursiveCall * func, void * args) {
    Context * org_ctx = sym_ctx;
    int org_frame = sym_frame;
    ContextAddress org_ip = sym_ip;
    Symbol * org_symbol_list = find_symbol_list;
    Trap trap;

    if (set_trap(&trap)) {
        func(args);
        clear_trap(&trap);
    }

    find_symbol_list = org_symbol_list;
    sym_ctx = org_ctx;
    sym_frame = org_frame;
    sym_ip = org_ip;

    if (!trap.error) return 0;
    errno = trap.error;
    return -1;
}

static Symbol * alloc_symbol(void) {
    Symbol * s = (Symbol *)tmp_alloc_zero(sizeof(Symbol));
#if ENABLE_SymbolsMux
    s->reader = &symbol_reader;
#endif
    s->magic = SYMBOL_MAGIC;
    return s;
}

static int get_sym_context(Context * ctx, int frame, ContextAddress addr) {
    if (frame == STACK_NO_FRAME) {
        sym_ip = addr;
    }
    else if (is_top_frame(ctx, frame)) {
        if (!is_ctx_stopped(ctx)) return -1;
        if (get_PC(ctx, &sym_ip) < 0) return -1;
    }
    else {
        U8_T ip = 0;
        StackFrame * info = NULL;
        if (get_frame_info(ctx, frame, &info) < 0) return -1;
        if (read_reg_value(info, get_PC_definition(ctx), &ip) < 0) return -1;
        assert(!info->is_top_frame);
        if (ip > 0) ip--;
        sym_ip = (ContextAddress)ip;
    }
    sym_ctx = ctx;
    sym_frame = frame;
    return 0;
}

static int elf_symbol_has_address(ELF_SymbolInfo * info) {
    switch (info->type) {
    case STT_NOTYPE:
        /* Check if the NOTYPE symbol is for a section allocated in memory */
        if (info->section_index == SHN_ABS) return 1;
        if (info->section == NULL) return 0;
        if ((info->section->flags & SHF_ALLOC) == 0) return 0;
        if (info->value < info->section->addr) return 0;
        if (info->value > info->section->addr + info->section->size) return 0;
        return 1;
    case STT_OBJECT:
    case STT_FUNC:
    case STT_GNU_IFUNC:
        return 1;
    }
    return 0;
}

int elf_symbol_address(Context * ctx, ELF_SymbolInfo * info, ContextAddress * address) {
    ELF_File * file = info->sym_section->file;
    ELF_Section * sec = NULL;
    U8_T value = info->value;

#ifdef ELF_SYMS_GET_ADDR
    ELF_SYMS_GET_ADDR;
#endif

    switch (info->type) {
    case STT_NOTYPE:
    case STT_OBJECT:
    case STT_FUNC:
        if (info->section_index == SHN_UNDEF) {
            set_errno(ERR_OTHER, "Cannot get address of ELF symbol: the symbol is undefined");
            return -1;
        }
        if (info->section_index == SHN_ABS) {
            *address = (ContextAddress)value;
            return 0;
        }
        if (info->section_index == SHN_COMMON) {
            set_errno(ERR_OTHER, "Cannot get address of ELF symbol: the symbol is a common block");
            return -1;
        }
        if (!elf_symbol_has_address(info)) break;
        if (info->section != NULL) {
            sec = info->section;
            if (file->type == ET_REL) {
                value += sec->addr;
            }
            if (info->section->size > 0 && info->value == info->section->addr + info->section->size) {
                *address = elf_map_to_run_time_address(ctx, file, sec, (ContextAddress)value - 1);
                if (errno) return -1;
                *address += 1;
                return 0;
            }
        }
        if (IS_PPC64_FUNC_OPD(file, info)) {
            /*
             * For PPC64(v1), an ELF function symbol address is not described by
             * the symbol value. In that case the symbol value points to a
             * function descriptor in the OPD section. The first entry of the
             * descriptor is the real function address. This value is
             * relocatable.
             */
            U8_T offset;
            ELF_Section * opd = file->sections + file->section_opd;
            if (elf_load(opd) < 0) exception(errno);
            offset = value - opd->addr;
            value = *(U8_T *)((U1_T *)opd->data + offset);
            if (file->byte_swap) SWAP(value);
            drl_relocate(opd, offset, &value, sizeof(value), &sec);
        }
        *address = elf_map_to_run_time_address(ctx, file, sec, (ContextAddress)value);
        return errno ? -1 : 0;
    case STT_GNU_IFUNC:
        set_errno(ERR_OTHER, "Cannot get address of ELF symbol: indirect symbol");
        return -1;
    }
    set_errno(ERR_OTHER, "Cannot get address of ELF symbol: wrong symbol type");
    return -1;
}

int elf_tcf_symbol(Context * ctx, ELF_SymbolInfo * sym_info, Symbol ** symbol) {
    Symbol * sym = alloc_symbol();

    sym->frame = STACK_NO_FRAME;
    if (sym_info->type == STT_TLS) {
        sym->ctx = ctx;
    }
    else {
        sym->ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
    }
    sym->tbl = sym_info->sym_section;
    sym->index = sym_info->sym_index;
    sym->dynsym = sym->tbl->type == SHT_DYNSYM;

    switch (sym_info->type) {
    case STT_NOTYPE:
        if (!elf_symbol_has_address(sym_info)) {
            sym->sym_class = SYM_CLASS_VALUE;
            break;
        }
        if (sym_info->section == NULL || (sym_info->section->flags & SHF_EXECINSTR) == 0) {
            sym->sym_class = SYM_CLASS_REFERENCE;
            break;
        }
        sym->sym_class = SYM_CLASS_FUNCTION;
        break;
    case STT_FUNC:
    case STT_GNU_IFUNC:
        sym->sym_class = SYM_CLASS_FUNCTION;
        break;
    case STT_OBJECT:
        sym->sym_class = SYM_CLASS_REFERENCE;
        break;
    default:
        sym->sym_class = SYM_CLASS_VALUE;
        break;
    }
    *symbol = sym;
    return 0;
}

int elf_symbol_info(Symbol * sym, ELF_SymbolInfo * elf_sym) {
    Trap trap;

    assert (sym != NULL && sym->magic == SYMBOL_MAGIC && sym->tbl != NULL);

    if (!set_trap(&trap)) return -1;

    unpack_elf_symbol_info(sym->tbl, sym->index, elf_sym);

    clear_trap(&trap);
    return 0;
}

static void check_addr_and_size(void * args) {
    Symbol * sym = (Symbol *)args;
    ContextAddress addr = 0;
    ContextAddress size = 0;

    assert(sym->frame == STACK_NO_FRAME);

    if (get_symbol_size(sym, &size) < 0) exception(errno);

    if (sym->sym_class == SYM_CLASS_REFERENCE) {
        if (sym->obj->mTag == TAG_member || sym->obj->mTag == TAG_inheritance) {
            if (get_symbol_offset(sym, &addr) < 0) exception(errno);
        }
        else if (get_symbol_address(sym, &addr) < 0) {
            exception(errno);
        }
    }
}

/* Return 1 if evaluation of symbol properties requires a stack frame.
 * Return 0 otherwise.
 * In case of a doubt, should return 1. */
static int is_frame_based_object(Symbol * sym) {

    if (sym->var != NULL) return 1;
    if (sym->obj != NULL && (sym->obj->mFlags & DOIF_need_frame)) return 1;
    if (sym->ref != NULL && (sym->ref->mFlags & DOIF_need_frame)) return 1;
    if (sym->assembly_function) return 0;

    switch (sym->sym_class) {
    case SYM_CLASS_VALUE:
        return 0;
    case SYM_CLASS_BLOCK:
    case SYM_CLASS_NAMESPACE:
    case SYM_CLASS_COMP_UNIT:
    case SYM_CLASS_FUNCTION:
        /* Keep frame reference to allow get_symbol_children() to return
         * frame-based symbols for parameters and local vars */
        return 1;
    case SYM_CLASS_TYPE:
        if (sym->obj != NULL) {
            ObjectInfo * obj = sym->obj;
            while (1) {
                switch (obj->mTag) {
                case TAG_typedef:
                case TAG_packed_type:
                case TAG_const_type:
                case TAG_volatile_type:
                case TAG_restrict_type:
                case TAG_shared_type:
                    if (obj->mType == NULL) break;
                    obj = obj->mType;
                    continue;
                case TAG_base_type:
                case TAG_fund_type:
                case TAG_enumeration_type:
                case TAG_subroutine_type:
                case TAG_ptr_to_member_type:
                    return 0;
                case TAG_pointer_type:
                case TAG_reference_type:
                case TAG_rvalue_reference_type:
                case TAG_structure_type:
                case TAG_class_type:
                case TAG_union_type:
                    if (obj->mCompUnit->mLanguage == LANG_C89) return 0;
                    if (obj->mCompUnit->mLanguage == LANG_C) return 0;
                    break;
                }
                break;
            }
        }
        break;
    }

    if (sym->obj != NULL) {
        if (sym->obj->mCompUnit->mLanguage == LANG_ADA83) return 1;
        if (sym->obj->mCompUnit->mLanguage == LANG_ADA95) return 1;
        if (elf_save_symbols_state(check_addr_and_size, sym) < 0) return 1;
    }

    return 0;
}

/* Return 1 if evaluation of symbol properties requires a thread.
 * Return 0 otherwise.
 * In case of a doubt, should return 1. */
static int is_thread_based_object(Symbol * sym) {
    /* Variables can be thread local */
    if (sym->sym_class == SYM_CLASS_REFERENCE) return 1;
    if (sym->sym_class == SYM_CLASS_TYPE && sym->obj != NULL) {
        /* Thread local static members of a class */
        ObjectInfo * obj = sym->obj;
        switch (obj->mTag) {
        case TAG_structure_type:
        case TAG_class_type:
        case TAG_union_type:
            if (obj->mCompUnit->mLanguage == LANG_C89) break;
            if (obj->mCompUnit->mLanguage == LANG_C) break;
            return 1;
        }
    }
    return 0;
}

static const char * get_linkage_name(ObjectInfo * obj) {
    Trap trap;
    PropertyValue p;
    if ((obj->mFlags & DOIF_mips_linkage_name) && set_trap(&trap)) {
        read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, AT_MIPS_linkage_name, &p);
        clear_trap(&trap);
        if (p.mAddr != NULL) return (char *)p.mAddr;
    }
    if ((obj->mFlags & DOIF_linkage_name) && set_trap(&trap)) {
        read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, AT_linkage_name, &p);
        clear_trap(&trap);
        if (p.mAddr != NULL) return (char *)p.mAddr;
    }
    if ((obj->mFlags & DOIF_mangled_name) && set_trap(&trap)) {
        read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, AT_mangled, &p);
        clear_trap(&trap);
        if (p.mAddr != NULL) return (char *)p.mAddr;
    }
    return obj->mName;
}

static int symbol_priority(ObjectInfo * obj) {
    int p = 0;
    if (obj->mFlags & DOIF_external) p += 2;
    if (obj->mFlags & DOIF_declaration) p -= 4;
    if (obj->mFlags & DOIF_abstract_origin) p += 1;
    switch (obj->mTag) {
    case TAG_class_type:
    case TAG_structure_type:
    case TAG_union_type:
    case TAG_enumeration_type:
        p -= 1;
        break;
    }
    return p;
}

static int symbol_is_weak(ObjectInfo * obj) {
    Trap trap;
    if ((obj->mFlags & DOIF_external) == 0) return 0;
    if ((obj->mFlags & DOIF_low_pc) == 0) return 0;
    if (set_trap(&trap)) {
        const char * name = get_linkage_name(obj);
        if (name != NULL) {
            unsigned h = calc_symbol_name_hash(name);
            ELF_File * file = obj->mCompUnit->mFile;
            unsigned m = 0;

            for (m = 1; m < file->section_cnt; m++) {
                unsigned n;
                ELF_Section * tbl = file->sections + m;
                if (tbl->sym_names_hash == NULL) continue;
                n = tbl->sym_names_hash[h % tbl->sym_names_hash_size];
                while (n) {
                    ELF_SymbolInfo sym_info;
                    unpack_elf_symbol_info(tbl, n, &sym_info);
                    if (sym_info.bind == STB_GLOBAL || sym_info.bind == STB_WEAK) {
                        switch (sym_info.type) {
                        case STT_OBJECT:
                        case STT_FUNC:
                            if (equ_symbol_names(name, sym_info.name)) {
                                ContextAddress sym_addr = 0;
                                ContextAddress obj_addr = 0;
                                if (elf_symbol_address(sym_ctx, &sym_info, &sym_addr) < 0) break;
                                obj_addr = elf_map_to_run_time_address(sym_ctx, file, obj->u.mCode.mSection, obj->u.mCode.mLowPC);
                                if (errno) break;
                                if (obj_addr != sym_addr) {
                                    clear_trap(&trap);
                                    return 1;
                                }
                            }
                            break;
                        }
                    }
                    n = tbl->sym_names_next[n];
                }
            }
        }
        clear_trap(&trap);
    }
    return 0;
}

static void is_assembly_function(Symbol * sym) {
    ObjectInfo * obj = sym->obj;
    Trap trap;
    sym->assembly_function = 0;
    if ((obj->mFlags & DOIF_low_pc) == 0) return;
    if (set_trap(&trap)) {
        const char * name = get_linkage_name(obj);
        if (name != NULL) {
            unsigned h = calc_symbol_name_hash(name);
            ELF_File * file = obj->mCompUnit->mFile;
            unsigned m = 0;

            for (m = 1; m < file->section_cnt; m++) {
                unsigned n;
                ELF_Section * tbl = file->sections + m;
                if (tbl->sym_names_hash == NULL) continue;
                n = tbl->sym_names_hash[h % tbl->sym_names_hash_size];
                while (n) {
                    ELF_SymbolInfo sym_info;
                    unpack_elf_symbol_info(tbl, n, &sym_info);
                    switch (sym_info.type) {
                    case STT_FUNC:
                        if (equ_symbol_names(name, sym_info.name)) {
                            ContextAddress sym_addr = 0;
                            ContextAddress obj_addr = 0;
                            if (elf_symbol_address(sym_ctx, &sym_info, &sym_addr) < 0) break;
                            obj_addr = elf_map_to_run_time_address(sym_ctx, file, obj->u.mCode.mSection, obj->u.mCode.mLowPC);
                            if (errno) break;
                            if (obj_addr == sym_addr) {
                                clear_trap(&trap);
                                sym->has_address = 1;
                                sym->assembly_function = 1;
                                sym->address = sym_addr;
                                sym->size = sym_info.size;
                                return;
                            }
                        }
                        break;
                    }
                    n = tbl->sym_names_next[n];
                }
            }
        }
        clear_trap(&trap);
    }
}

void elf_object2symbol(ObjectInfo * ref, ObjectInfo * obj, Symbol ** res) {
    Symbol * sym = alloc_symbol();
    sym->obj = obj;
    switch (obj->mTag) {
    case TAG_global_subroutine:
    case TAG_inlined_subroutine:
    case TAG_subroutine:
    case TAG_subprogram:
    case TAG_entry_point:
        sym->sym_class = SYM_CLASS_FUNCTION;
        break;
    case TAG_array_type:
    case TAG_class_type:
    case TAG_enumeration_type:
    case TAG_pointer_type:
    case TAG_reference_type:
    case TAG_rvalue_reference_type:
    case TAG_mod_pointer:
    case TAG_mod_reference:
    case TAG_string_type:
    case TAG_structure_type:
    case TAG_subroutine_type:
    case TAG_union_type:
    case TAG_ptr_to_member_type:
    case TAG_set_type:
    case TAG_subrange_type:
    case TAG_base_type:
    case TAG_fund_type:
    case TAG_file_type:
    case TAG_packed_type:
    case TAG_thrown_type:
    case TAG_const_type:
    case TAG_volatile_type:
    case TAG_restrict_type:
    case TAG_interface_type:
    case TAG_unspecified_type:
    case TAG_mutable_type:
    case TAG_shared_type:
    case TAG_typedef:
    case TAG_template_type_param:
        sym->sym_class = SYM_CLASS_TYPE;
        break;
    case TAG_global_variable:
    case TAG_inheritance:
    case TAG_member:
    case TAG_formal_parameter:
    case TAG_unspecified_parameters:
    case TAG_local_variable:
    case TAG_variable:
        sym->sym_class = SYM_CLASS_REFERENCE;
        break;
    case TAG_constant:
    case TAG_enumerator:
        sym->sym_class = SYM_CLASS_VALUE;
        break;
    case TAG_compile_unit:
    case TAG_partial_unit:
        sym->sym_class = SYM_CLASS_COMP_UNIT;
        break;
    case TAG_lexical_block:
    case TAG_with_stmt:
    case TAG_try_block:
    case TAG_catch_block:
        sym->sym_class = SYM_CLASS_BLOCK;
        break;
    case TAG_namespace:
        sym->sym_class = SYM_CLASS_NAMESPACE;
        break;
    case TAG_variant_part:
        sym->sym_class = SYM_CLASS_VARIANT_PART;
        break;
    case TAG_variant:
        sym->sym_class = SYM_CLASS_VARIANT;
        break;
    case TAG_label:
        sym->sym_class = SYM_CLASS_REFERENCE;
        /* LLVM compiler uses TAG_label for assembly functions */
        is_assembly_function(sym);
        if (!sym->assembly_function) break;
        sym->sym_class = SYM_CLASS_FUNCTION;
        break;
    }
    sym->ref = ref;
    sym->frame = STACK_NO_FRAME;
    if (sym->sym_class == SYM_CLASS_REFERENCE && obj->mTag != TAG_member) sym->ref = obj;
    sym->ctx = context_get_group(sym_ctx, CONTEXT_GROUP_SYMBOLS);
    if (sym_frame != STACK_NO_FRAME && is_frame_based_object(sym)) {
        sym->frame = sym_frame;
        sym->ctx = sym_ctx;
    }
    else if (sym_ctx != sym->ctx && is_thread_based_object(sym)) {
        sym->ctx = sym_ctx;
    }
    sym->priority = (int8_t)symbol_priority(obj);
    sym->weak = (int8_t)symbol_is_weak(obj);
    *res = sym;
}

static ObjectInfo * get_object_ref_prop(ObjectInfo * obj, U2_T at);

static ObjectInfo * get_object_type(ObjectInfo * obj) {
    if (obj != NULL) {
        switch (obj->mTag) {
        case TAG_compile_unit:
        case TAG_global_subroutine:
        case TAG_inlined_subroutine:
        case TAG_subroutine:
        case TAG_subprogram:
        case TAG_entry_point:
        case TAG_enumerator:
        case TAG_formal_parameter:
        case TAG_unspecified_parameters:
        case TAG_global_variable:
        case TAG_local_variable:
        case TAG_variable:
        case TAG_inheritance:
        case TAG_member:
        case TAG_constant:
            obj = obj->mType;
            break;
        case TAG_variant_part:
            if (obj->mType != NULL) {
                obj = obj->mType;
                break;
            }
            return get_object_type(get_object_ref_prop(obj, AT_discr));
        case TAG_label:
        case TAG_variant:
            return NULL;
        }
    }
    return obj;
}

static int is_modified_type(ObjectInfo * obj) {
    if (obj != NULL) {
        switch (obj->mTag) {
        case TAG_subrange_type:
        case TAG_packed_type:
        case TAG_const_type:
        case TAG_volatile_type:
        case TAG_restrict_type:
        case TAG_shared_type:
        case TAG_typedef:
        case TAG_template_type_param:
            return 1;
        }
    }
    return 0;
}

static void alloc_std_type_pseudo_symbol(Context * ctx, unsigned size, unsigned type_class, Symbol ** type) {
    Symbol * sym = alloc_symbol();
    sym->ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
    sym->frame = STACK_NO_FRAME;
    sym->sym_class = SYM_CLASS_TYPE;
    sym->cardinal = size;
    sym->dimension = type_class;
    assert(is_std_type_pseudo_symbol(sym));
    *type = sym;
}

/* Get object original type, skipping typedefs and all modifications like const, volatile, etc. */
static ObjectInfo * get_original_type(ObjectInfo * obj) {
    obj = get_object_type(obj);
    while (obj != NULL && obj->mType != NULL && is_modified_type(obj)) obj = obj->mType;
    return obj;
}

typedef struct EvaluateRefObjectArgs {
    Context * ctx;
    int frame;
    U2_T at;
    ObjectInfo * ref;
    ObjectInfo * obj;
    PropertyValue * v;
    U8_T res;
} EvaluateRefObjectArgs;

static int is_pointer_type(Symbol * sym) {
    for (;;) {
        int type_class = 0;
        Symbol * next = NULL;
        if (get_symbol_type_class(sym, &type_class) < 0) exception(errno);
        if (type_class == TYPE_CLASS_POINTER) return 1;
        if (get_symbol_type(sym, &next) < 0) exception(errno);
        if (next == sym) break;
        sym = next;
    }
    return 0;
}

static void evaluate_variable_num_prop(void * x) {
    EvaluateRefObjectArgs * args = (EvaluateRefObjectArgs *)x;
    Symbol * sym = NULL;
    ContextAddress addr = 0;
    uint64_t arr[1];
    PropertyValue v;
    void * value = NULL;
    size_t size = 0;
    int big_endian = 0;
    unsigned i;

    sym_ctx = args->ctx;
    sym_frame = args->frame;
    elf_object2symbol(NULL, args->ref, &sym);
    if (is_pointer_type(sym)) {
        if (get_symbol_value(sym, &value, &size, &big_endian) < 0) exception(errno);
        for (i = 0; i < size; i++) {
            ContextAddress b = *((uint8_t *)value + i);
            addr |= b << (big_endian ? (size - i - 1) * 8 : i * 8);
        }
    }
    else {
        if (get_symbol_address(sym, &addr) < 0) exception(errno);
    }
    arr[0] = addr;
    read_and_evaluate_dwarf_object_property_with_args(args->ctx, args->frame, args->obj, args->at, arr, 1, &v);
    args->res = get_numeric_property_value(&v);
}

static void evaluate_reference_num_prop(void * x) {
    EvaluateRefObjectArgs * args = (EvaluateRefObjectArgs *)x;
    ContextAddress addr = 0;
    uint64_t arr[1];
    PropertyValue * v = args->v;
    void * value = NULL;
    size_t size = 0;
    int big_endian = 0;
    ObjectInfo * obj = NULL;
    Symbol * sym_obj = NULL;
    Symbol * sym_ref = NULL;
    ELF_Section * sec = v->mSection;
    LocationInfo * loc_info = NULL;
    StackFrame * frame_info = NULL;
    LocationExpressionState * state = NULL;
    unsigned i;

    if (sec == NULL) sec = args->obj->mCompUnit->mDesc.mSection;
    obj = find_object(sec, (ContextAddress)v->mValue);
    if (obj == NULL) exception(ERR_INV_DWARF);
    elf_object2symbol(args->ref, obj, &sym_obj);
    if (get_location_info(sym_obj, &loc_info) < 0) exception(errno);
    if (loc_info->args_cnt > 0) {
        if (loc_info->args_cnt > 1) str_exception(ERR_OTHER, "Invalid location expression");
        elf_object2symbol(NULL, args->ref, &sym_ref);
        if (is_pointer_type(sym_ref)) {
            if (get_symbol_value(sym_ref, &value, &size, &big_endian) < 0) exception(errno);
            for (i = 0; i < size; i++) {
                ContextAddress b = *((uint8_t *)value + i);
                addr |= b << (big_endian ? (size - i - 1) * 8 : i * 8);
            }
        }
        else {
            if (get_symbol_address(sym_ref, &addr) < 0) exception(errno);
        }
    }
    arr[0] = addr;
    if (args->frame != STACK_NO_FRAME && get_frame_info(args->ctx, args->frame, &frame_info) < 0) exception(errno);
    state = evaluate_location_expression(args->ctx, frame_info,
        loc_info->value_cmds.cmds, loc_info->value_cmds.cnt, arr, loc_info->args_cnt);
    if (state->pieces_cnt > 0) {
        read_location_pieces(state->ctx, state->stack_frame,
            state->pieces, state->pieces_cnt, loc_info->big_endian, &value, &size);
    }
    else {
        ContextAddress sym_size = 0;
        if (state->stk_pos != 1) str_exception(ERR_OTHER, "Invalid location expression");
        if (get_symbol_size(sym_obj, &sym_size) < 0) exception(errno);
        size = (size_t)sym_size;
        value = tmp_alloc(size);
        if (context_read_mem(state->ctx, (ContextAddress)state->stk[0], value, size) < 0) exception(errno);
    }
    big_endian = loc_info->big_endian;
    args->res = 0;
    for (i = 0; i < size; i++) {
        U8_T b = *((uint8_t *)value + i);
        args->res |= b << (big_endian ? (size - i - 1) * 8 : i * 8);
    }
}

static int get_num_prop(ObjectInfo * obj, U2_T at, U8_T * res) {
    Trap trap;
    PropertyValue v;

    if (!set_trap(&trap)) return 0;
    read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, at, &v);
    *res = get_numeric_property_value(&v);
    clear_trap(&trap);
    return 1;
}

static int get_variable_num_prop(ObjectInfo * ref, ObjectInfo * obj, U2_T at, U8_T * res) {
    Trap trap;
    PropertyValue v;
    EvaluateRefObjectArgs args;

    memset(&args, 0, sizeof(args));
    args.ctx = sym_ctx;
    args.frame = sym_frame;
    args.at = at;
    args.ref = ref;
    args.obj = obj;

    if (set_trap(&trap)) {
        read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, at, &v);
        switch (v.mForm) {
        case FORM_REF       :
        case FORM_REF_ADDR  :
        case FORM_REF1      :
        case FORM_REF2      :
        case FORM_REF4      :
        case FORM_REF8      :
        case FORM_REF_UDATA :
            args.v = &v;
            clear_trap(&trap);
            if (elf_save_symbols_state(evaluate_reference_num_prop, &args) < 0) return 0;
            *res = args.res;
            return 1;
        }
        *res = get_numeric_property_value(&v);
        clear_trap(&trap);
        return 1;
    }
    else if (ref != NULL && get_error_code(errno) == ERR_INV_CONT_OBJ) {
        /* Dynamic property - need object address */
        assert(args.frame == sym_frame);
        if (elf_save_symbols_state(evaluate_variable_num_prop, &args) < 0) return 0;
        *res = args.res;
        return 1;
    }
    return 0;
}

static ObjectInfo * get_object_ref_prop(ObjectInfo * obj, U2_T at) {
    Trap trap;
    PropertyValue v;
    ObjectInfo * res;
    ELF_Section * sec;

    if (!set_trap(&trap)) return NULL;
    read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, at, &v);
    sec = v.mSection;
    if (sec == NULL) sec = obj->mCompUnit->mDesc.mSection;
    res = find_object(sec, (ContextAddress)v.mValue);
    if (res == NULL) str_exception(ERR_INV_DWARF, "Invalid debug info entry reference");
    clear_trap(&trap);
    return res;
}

static int cmp_object_profiles(ObjectInfo * x, ObjectInfo * y) {
    if (x == y) return 1;
    while (x != NULL) {
        switch (x->mTag) {
        case TAG_typedef:
        case TAG_const_type:
        case TAG_volatile_type:
            x = x->mType;
            continue;
        }
        break;
    }
    while (y != NULL) {
        switch (y->mTag) {
        case TAG_typedef:
        case TAG_const_type:
        case TAG_volatile_type:
            y = y->mType;
            continue;
        }
        break;
    }
    if (x == NULL || y == NULL) return 0;
    if (x->mTag != y->mTag) return 0;
    if (x->mName != y->mName) {
        if (x->mName == NULL || y->mName == NULL) return 0;
        if (strcmp(x->mName, y->mName) != 0) return 0;
    }
    if (!cmp_object_profiles(x->mType, y->mType)) return 0;
    switch (x->mTag) {
    case TAG_subprogram:
        {
            ObjectInfo * px = get_dwarf_children(x);
            ObjectInfo * py = get_dwarf_children(y);
            for (;;) {
                while (px != NULL) {
                    if (px->mTag == TAG_formal_parameter) break;
                    px = px->mSibling;
                }
                while (py != NULL) {
                    if (py->mTag == TAG_formal_parameter) break;
                    py = py->mSibling;
                }
                if (px == NULL || py == NULL) break;
                if (!cmp_object_profiles(px->mType, py->mType)) return 0;
                px = px->mSibling;
                py = py->mSibling;
            }
            if (x->mName != NULL && x->mName[0] == '~') break;
            if (px != NULL || py != NULL) return 0;
        }
        break;
    }
    return 1;
}

static int cmp_object_linkage_names(ObjectInfo * x, ObjectInfo * y) {
    const char * xname = get_linkage_name(x);
    const char * yname = get_linkage_name(y);
    if (xname == yname) return 1;
    if (xname == NULL) return 0;
    if (yname == NULL) return 0;
    return strcmp(xname, yname) == 0;
}

/* return 0 if symbol has no address, e.g. undef or common */
static int symbol_has_location(Symbol * sym) {
    if (sym->has_address) return 1;
    if (sym->tbl != NULL) {
        if (sym->dimension == 0) {
            ELF_SymbolInfo info;
            unpack_elf_symbol_info(sym->tbl, sym->index, &info);
            if (info.section_index == SHN_UNDEF) return 0;
            if (info.section_index == SHN_COMMON) return 0;
            if (info.type == STT_TLS && !context_has_state(sym->ctx)) return 0;
            if (info.type == STT_COMMON) return 0;
        }
        return 1;
    }
    if (sym->obj != NULL && sym->obj->mTag != TAG_dwarf_procedure) {
        if (sym->obj->mFlags & DOIF_location) {
#ifdef ELF_SYMS_HAS_ADDR
            ELF_SYMS_HAS_ADDR;
#endif
            /* AT_location defined, so we have an address */
            return 1;
        }
        if (sym->obj->mFlags & DOIF_data_location) {
            return 1;
        }
        if (sym->obj->mFlags & DOIF_low_pc) {
            /* AT_low_pc defined, so we have an address */
            return 1;
        }
        if (sym->obj->mFlags & DOIF_external) {
            /* No location info in DWARF, check ELF symbols for symbol address */
            /* Do not call map_to_sym_table() - infinite recursion */
            ELF_File * file = sym->obj->mCompUnit->mFile;
            if (file->debug_info_file) {
                size_t n = strlen(file->name);
                if (n > 6 && strcmp(file->name + n - 6, ".debug") == 0) {
                    char * fnm = (char *)tmp_alloc_zero(n);
                    memcpy(fnm, file->name, n - 6);
                    fnm = canonicalize_file_name(fnm);
                    if (fnm != NULL) {
                        file = elf_open(fnm);
                        free(fnm);
                    }
                }
            }
            if (file != NULL) {
                const char * name = get_linkage_name(sym->obj);
                if (name != NULL) {
                    unsigned m = 0;
                    unsigned h = calc_symbol_name_hash(name);
                    for (m = 1; m < file->section_cnt; m++) {
                        unsigned n;
                        ELF_Section * tbl = file->sections + m;
                        if (tbl->sym_names_hash == NULL) continue;
                        n = tbl->sym_names_hash[h % tbl->sym_names_hash_size];
                        while (n) {
                            ELF_SymbolInfo info;
                            unpack_elf_symbol_info(tbl, n, &info);
                            n = tbl->sym_names_next[n];
                            if (equ_symbol_names(name, info.name) && info.bind == STB_GLOBAL) {
                                if (info.section_index == SHN_UNDEF) continue;
                                if (info.section_index == SHN_COMMON) continue;
                                if (info.type == STT_TLS && !context_has_state(sym->ctx)) continue;
                                if (info.type == STT_COMMON) continue;
                                return 1;
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/* Return 1 if find list has no location info: either common or undef */
static int should_continue_pub_names_search(void) {
    Symbol * s = find_symbol_list;
    while (s != NULL) {
        if (s->has_location) return 0;
        if (s->sym_class == SYM_CLASS_TYPE) return 0;
        s = s->next;
    }
    /* Symbols have no location info, continue search */
    return 1;
}

static int symbol_equ_comparator(const void * x, const void * y) {
    Symbol * sx = *(Symbol **)x;
    Symbol * sy = *(Symbol **)y;

    if (sx->obj < sy->obj) return -1;
    if (sx->obj > sy->obj) return +1;
    if (sx->var < sy->var) return -1;
    if (sx->var > sy->var) return +1;
    if (sx->tbl < sy->tbl) return -1;
    if (sx->tbl > sy->tbl) return +1;
    if (sx->index < sy->index) return -1;
    if (sx->index > sy->index) return +1;
    return 0;
}

static int symbol_prt_comparator(const void * x, const void * y) {
    Symbol * sx = *(Symbol **)x;
    Symbol * sy = *(Symbol **)y;

    /* symbols with no address have lower priority */
    if (sx->has_location && !sy->has_location) return +1;
    if (sy->has_location && !sx->has_location) return -1;

    if (sx->weak && !sy->weak) return -1;
    if (sy->weak && !sx->weak) return +1;

    /* Symbols order by priority, from low to high,
     * most likely match must be last */

    if (sx->level < sy->level) return -1;
    if (sx->level > sy->level) return +1;

    if (sx->obj == NULL && sy->obj != NULL) return -1;
    if (sx->obj != NULL && sy->obj == NULL) return +1;

    if (sx->priority < sy->priority) return -1;
    if (sx->priority > sy->priority) return +1;

    /* 'this' members have lower priority than local variables */
    if (sx->var == NULL && sy->var != NULL) return +1;
    if (sx->var != NULL && sy->var == NULL) return -1;

    /* Main symbol table is preferred over dynamic symbols */
    if (sx->dynsym && !sy->dynsym) return -1;
    if (sy->dynsym && !sx->dynsym) return +1;

    /* First added to the results list has higher priority */
    if (sx->pos < sy->pos) return +1;
    if (sx->pos > sy->pos) return -1;

    return 0;
}

static void add_to_find_symbol_buf(Symbol * sym) {
    sym->has_location = (int8_t)symbol_has_location(sym);
    sym->next = find_symbol_list;
    find_symbol_list = sym;
}

static void add_obj_to_find_symbol_buf(ObjectInfo * obj, unsigned level) {
    Symbol * sym = NULL;
    elf_object2symbol(NULL, obj, &sym);
    add_to_find_symbol_buf(sym);
    sym->level = level;
}

static void add_elf_to_find_symbol_buf(ELF_SymbolInfo * elf_sym) {
    Symbol * sym = NULL;
    elf_tcf_symbol(sym_ctx, elf_sym, &sym);
    add_to_find_symbol_buf(sym);
}

static void sort_find_symbol_buf(void) {
    /* Sort find_symbol_list:
     * 1. inner scope before parent scope
     * 2. DWARF symbols before ELF symbols
     * 3. 'extern' before 'static'
     * 3. definitions before declarations
     * 4. undef after symbols with addresses.
     * 5. etc.
     */
    unsigned cnt = 0;
    unsigned pos = 0;
    unsigned dup = 0;
    Symbol ** buf = NULL;
    Symbol * s = find_symbol_list;
    if (s == NULL) return;
    if (s->next == NULL) return;
    while (s != NULL) {
        s = s->next;
        cnt++;
    }
    pos = 0;
    s = find_symbol_list;
    buf = (Symbol **)tmp_alloc(sizeof(Symbol *) * cnt);
    while (s != NULL) {
        s->pos = cnt - pos;
        buf[pos++] = s;
        s = s->next;
    }
    find_symbol_list = NULL;
    /* Remove duplicate entries */
    qsort(buf, cnt, sizeof(Symbol *), symbol_equ_comparator);
    for (pos = 1, dup = 0; pos < cnt; pos++) {
        if (symbol_equ_comparator(buf + pos, buf + dup) == 0) {
            if (symbol_prt_comparator(buf + pos, buf + dup) > 0) buf[dup] = buf[pos];
        }
        else {
            buf[++dup] = buf[pos];
        }
    }
    cnt = dup + 1;
    /* Final sort */
    qsort(buf, cnt, sizeof(Symbol *), symbol_prt_comparator);
    for (pos = 0; pos < cnt; pos++) {
        s = buf[pos];
        s->next = find_symbol_list;
        find_symbol_list = s;
    }
}

static int same_namespace(ObjectInfo * x, ObjectInfo * y) {
    int xn = x->mParent != NULL && x->mParent->mTag == TAG_namespace;
    int yn = y->mParent != NULL && y->mParent->mTag == TAG_namespace;
    if (xn != yn) return 0;
    if (!xn) return 1;
    x = x->mParent;
    y = y->mParent;
    if (x->mName == y->mName) return 1;
    if (x->mName == NULL) return 0;
    if (y->mName == NULL) return 0;
    if (strcmp(x->mName, y->mName) != 0) return 0;
    return same_namespace(x, y);
}

/* If 'decl' represents a declaration, replace it with definition - if possible */
static ObjectInfo * find_definition(ObjectInfo * decl) {
    while (decl != NULL) {
        int search_pub_names = 0;
        int search_ext_only = 0;
        if (decl->mDefinition != NULL) {
            decl = decl->mDefinition;
            continue;
        }
        if (decl->mName == NULL) return decl;
        if ((decl->mFlags & DOIF_declaration) == 0) return decl;
        switch (decl->mTag) {
        case TAG_structure_type:
        case TAG_interface_type:
        case TAG_union_type:
        case TAG_class_type:
            search_pub_names = 1;
            break;
        default:
            search_pub_names = (decl->mFlags & DOIF_external) != 0;
            search_ext_only = 1;
            break;
        }
        if (search_pub_names) {
            ObjectInfo * def = NULL;
            DWARFCache * cache = get_dwarf_cache(get_dwarf_file(decl->mCompUnit->mFile));
            PubNamesTable * tbl = &cache->mPubNames;
            if (tbl->mHash != NULL) {
                unsigned n = tbl->mHash[calc_symbol_name_hash(decl->mName) % tbl->mHashSize];
                while (n != 0) {
                    ObjectInfo * obj = tbl->mNext[n].mObject;
                    n = tbl->mNext[n].mNext;
                    if (obj == decl) continue;
                    if (obj->mTag != decl->mTag) continue;
                    if (obj->mFlags & DOIF_declaration) continue;
                    if (obj->mFlags & DOIF_specification) continue;
                    if (search_ext_only && (obj->mFlags & DOIF_external) == 0) continue;
                    if (!equ_symbol_names(obj->mName, decl->mName)) continue;
                    if (!cmp_object_profiles(decl, obj)) continue;
                    if (!cmp_object_linkage_names(decl, obj)) continue;
                    if (!same_namespace(decl, obj)) continue;
                    def = obj;
                    break;
                }
            }
            if (def != NULL) {
                decl->mDefinition = def;
                decl = def;
                continue;
            }
        }
        break;
    }
    return decl;
}

static void find_by_name_in_pub_names(DWARFCache * cache, const char * name) {
    PubNamesTable * tbl = &cache->mPubNames;
    if (tbl->mHash != NULL) {
        unsigned n = tbl->mHash[calc_symbol_name_hash(name) % tbl->mHashSize];
        while (n != 0) {
            ObjectInfo * obj = tbl->mNext[n].mObject;
            int ns = obj->mParent != NULL && obj->mParent->mTag == TAG_namespace;
            if (!ns && equ_symbol_names(obj->mName, name)) {
                add_obj_to_find_symbol_buf(obj, 1);
            }
            n = tbl->mNext[n].mNext;
        }
    }
    if (cache->mFile->dwz_file != NULL) {
        find_by_name_in_pub_names(get_dwarf_cache(cache->mFile->dwz_file), name);
    }
}

static int get_object_scope(ObjectInfo * obj, ObjectInfo ** container) {
    ObjectInfo * parent = get_dwarf_parent(obj);
    if (parent != NULL && parent->mTag == TAG_compile_unit) {
        if (obj->mFlags & DOIF_abstract_origin) {
            ObjectInfo * org = get_object_ref_prop(obj, AT_abstract_origin);
            if (org == NULL) return -1;
            obj = org;
        }
        if (obj->mFlags & DOIF_specification) {
            ObjectInfo * spc = get_object_ref_prop(obj, AT_specification_v2);
            if (spc == NULL) return -1;
            obj = spc;
        }
        parent = get_dwarf_parent(obj);
    }
    if (parent == NULL && obj->mTag >= TAG_fund_type && obj->mTag < TAG_fund_type + 0x100) {
        /* Virtual DWARF object that is created by the DWARF reader. */
        parent = obj->mCompUnit->mObject;
    }
    *container = parent;
    return 0;
}

static int find_in_object_tree(ObjectInfo * parent, unsigned level,
                                UnitAddress * ip, const char * name) {
    ObjectInfo * children = get_dwarf_children(parent);
    ObjectInfo * obj = NULL;
    ObjectInfo * sym_this = NULL;
    int obj_ptr_chk = 0;
    U8_T obj_ptr_id = 0;

    if (ip != NULL) {
        /* Search nested scope */
        int found = 0;
        obj = children;
        while (obj != NULL) {
            switch (obj->mTag) {
            case TAG_namespace:
            case TAG_compile_unit:
            case TAG_partial_unit:
            case TAG_module:
            case TAG_global_subroutine:
            case TAG_inlined_subroutine:
            case TAG_lexical_block:
            case TAG_with_stmt:
            case TAG_try_block:
            case TAG_catch_block:
            case TAG_subroutine:
            case TAG_subprogram:
                if (find_in_object_tree(obj, level + 1, ip, name)) found = 1;
                break;
            }
            obj = obj->mSibling;
        }
        if (!found && check_in_range(parent, ip)) found = 1;
        if (!found && ip->unit->mObject != parent) return 0;
    }

    /* Search current scope */
    obj = children;
    while (obj != NULL) {
        if (obj->mTag != TAG_GNU_call_site) {
            if (obj->mName != NULL && equ_symbol_names(obj->mName, name)) {
                if (obj->mTag == TAG_lexical_block && parent->mTag == TAG_subprogram &&
                        parent->mName != NULL && strcmp(obj->mName, parent->mName) == 0) {
                    /* Bad representation of inlined recursive function, skip */
                }
                else {
                    /* Skip out-of-body definitions */
                    ObjectInfo * container = NULL;
                    if (get_object_scope(obj, &container) < 0) exception(errno);
                    if (container == parent) add_obj_to_find_symbol_buf(find_definition(obj), level);
                }
            }
            if (parent->mTag == TAG_subprogram && ip != 0) {
                if (!obj_ptr_chk) {
                    get_num_prop(parent, AT_object_pointer, &obj_ptr_id);
                    obj_ptr_chk = 1;
                }
                if (obj->mID == obj_ptr_id || (obj_ptr_id == 0 && obj->mTag == TAG_formal_parameter &&
                    (obj->mFlags & DOIF_artificial) && obj->mName != NULL && strcmp(obj->mName, "this") == 0)) {
                    sym_this = obj;
                }
            }
        }
        obj = obj->mSibling;
    }

    if (sym_this != NULL) {
        /* Search in 'this' pointer */
        ObjectInfo * type = get_original_type(sym_this);
        if ((type->mTag == TAG_pointer_type || type->mTag == TAG_mod_pointer) && type->mType != NULL) {
            Trap trap;
            Symbol * this_list = NULL;
            Symbol * find_list = find_symbol_list;
            if (set_trap(&trap)) {
                find_symbol_list = NULL;
                type = get_original_type(type->mType);
                find_in_object_tree(type, level, NULL, name);
                sort_find_symbol_buf();
                this_list = find_symbol_list;
                clear_trap(&trap);
            }
            find_symbol_list = find_list;
            while (this_list != NULL) {
                Symbol * s = this_list;
                this_list = this_list->next;
                if (s->obj->mTag != TAG_subprogram) {
                    s->ctx = sym_ctx;
                    s->frame = sym_frame;
                    s->var = sym_this;
                }
                s->next = NULL;
                add_to_find_symbol_buf(s);
            }
        }
    }

    if (parent->mFlags & DOIF_extension) {
        /* If the parent is namespace extension, search in base namespace */
        ObjectInfo * name_space = get_object_ref_prop(parent, AT_extension);
        if (name_space != NULL) find_in_object_tree(name_space, level, NULL, name);
    }

    /* Search imported and inherited objects */
    obj = children;
    while (obj != NULL) {
        switch (obj->mTag) {
        case TAG_enumeration_type:
            find_in_object_tree(obj, level, NULL, name);
            break;
        case TAG_inheritance:
            find_in_object_tree(obj->mType, level, NULL, name);
            break;
        case TAG_imported_declaration:
            if (obj->mName != NULL && equ_symbol_names(obj->mName, name)) {
                ObjectInfo * decl = get_object_ref_prop(obj, AT_import);
                if (decl != NULL) {
                    if (obj->mName != NULL || (decl->mName != NULL && equ_symbol_names(decl->mName, name))) {
                        add_obj_to_find_symbol_buf(find_definition(decl), level);
                    }
                }
            }
            break;
        case TAG_imported_module:
            find_in_object_tree(obj, level, NULL, name);
            {
                ObjectInfo * module = get_object_ref_prop(obj, AT_import);
                if (module != NULL && (module->mFlags & DOIF_find_mark) == 0) {
                    Trap trap;
                    if (set_trap(&trap)) {
                        module->mFlags |= DOIF_find_mark;
                        find_in_object_tree(module, level, NULL, name);
                        clear_trap(&trap);
                        module->mFlags &= ~DOIF_find_mark;
                    }
                    else {
                        module->mFlags &= ~DOIF_find_mark;
                        exception(trap.error);
                    }
                }
            }
            break;
        }
        obj = obj->mSibling;
    }
    return 1;
}

static void find_unit(Context * ctx, ContextAddress addr, UnitAddress * unit) {
    ContextAddress rt_addr = 0;
    UnitAddressRange * range = elf_find_unit(ctx, addr, addr, &rt_addr);
    memset(unit, 0, sizeof(UnitAddress));
    if (range != NULL) {
        unit->file = range->mUnit->mFile;
        if (range->mSection) unit->section = unit->file->sections + range->mSection;
        unit->lt_addr = addr - rt_addr + range->mAddr;
        unit->rt_addr = addr;
        unit->unit = range->mUnit;
    }
}

static void find_in_dwarf(const char * name) {
    UnitAddress addr;
    find_unit(sym_ctx, sym_ip, &addr);
    if (addr.unit != NULL) {
        CompUnit * unit = addr.unit;
        find_in_object_tree(unit->mObject, 2, &addr, name);
        if (unit->mBaseTypes != NULL) find_in_object_tree(unit->mBaseTypes->mObject, 2, NULL, name);
        if (unit->mObject->mName != NULL && equ_symbol_names(unit->mObject->mName, name)) {
            add_obj_to_find_symbol_buf(unit->mObject, 1);
        }
    }
}

static void find_by_name_in_sym_table(ELF_File * file, const char * name, int globals) {
    unsigned m = 0;
    unsigned h = calc_symbol_name_hash(name);
    Context * prs = context_get_group(sym_ctx, CONTEXT_GROUP_SYMBOLS);

    for (m = 1; m < file->section_cnt; m++) {
        unsigned n;
        ELF_Section * tbl = file->sections + m;
        if (tbl->sym_names_hash == NULL) continue;
        n = tbl->sym_names_hash[h % tbl->sym_names_hash_size];
        while (n) {
            ELF_SymbolInfo sym_info;
            unpack_elf_symbol_info(tbl, n, &sym_info);
            if (equ_symbol_names(name, sym_info.name) && (!globals || sym_info.bind == STB_GLOBAL || sym_info.bind == STB_WEAK)) {
                ContextAddress addr = 0;
                if (sym_info.section_index != SHN_ABS && elf_symbol_address(prs, &sym_info, &addr) == 0) {
                    UnitAddressRange * range = elf_find_unit(sym_ctx, addr, addr, NULL);
                    if (range != NULL) {
                        ObjectInfo * obj = get_dwarf_children(range->mUnit->mObject);
                        while (obj != NULL) {
                            if (obj->mName != NULL && (!globals || (obj->mFlags & DOIF_external) != 0)) {
                                switch (obj->mTag) {
                                case TAG_global_subroutine:
                                case TAG_global_variable:
                                case TAG_subroutine:
                                case TAG_subprogram:
                                case TAG_variable:
                                    if (equ_symbol_names(obj->mName, name)) {
                                        add_obj_to_find_symbol_buf(obj, 0);
                                    }
                                    break;
                                }
                            }
                            obj = obj->mSibling;
                        }
                    }
                }

                add_elf_to_find_symbol_buf(&sym_info);
            }
            n = tbl->sym_names_next[n];
        }
    }
}

static void find_relocate(ELF_File * file, const char * name) {
    /* Psedo-symbol, which helps debugger to map link-time address to run-time address */
    size_t l = strlen(file->name);
    size_t i = 0;
    int ok = 0;
    if (strncmp(file->name, name, l) == 0 && name[l] == ':') {
        i = l + 1;
        ok = 1;
    }
    if (!ok) {
        size_t p = l;
        while (p > 0 && file->name[p - 1] != '/' && file->name[p - 1] != '\\') p--;
        if (strncmp(file->name + p, name, l - p) == 0 && name[l - p] == ':') {
            i = l - p + 1;
            ok = 1;
        }
    }
    if (ok) {
        size_t j = i;
        char * section_name = NULL;
        while (name[j] != 0 && name[j] != ':') j++;
        if (j > i) section_name = tmp_strndup(name + i, j - i);
        if (name[j++] == ':') {
            ELF_Section * section = NULL;
            ContextAddress addr = 0;
            for (;;) {
                char ch = name[j++];
                if (ch >= '0' && ch <= '9') addr = (addr << 4) | (ch - '0');
                else if (ch >= 'A' && ch <= 'F') addr = (addr << 4) | (ch - 'A' + 10);
                else if (ch >= 'a' && ch <= 'f') addr = (addr << 4) | (ch - 'a' + 10);
                else break;
            }
            if (section_name != NULL) {
                unsigned n;
                for (n = 1; n < file->section_cnt; n++) {
                    ELF_Section * s = file->sections + n;
                    if (s->name == NULL) continue;
                    if (strcmp(s->name, section_name) == 0) {
                        section = s;
                        break;
                    }
                }
                if (section == NULL) return;
            }
            addr = elf_map_to_run_time_address(sym_ctx, file, section, addr);
            if (errno == 0) {
                Symbol * sym = alloc_symbol();
                sym->ctx = context_get_group(sym_ctx, CONTEXT_GROUP_SYMBOLS);
                sym->frame = STACK_NO_FRAME;
                sym->sym_class = SYM_CLASS_VALUE;
                sym->address = addr;
                sym->size = file->elf64 ? 8 : 4;
                sym->has_address = 1;
                assert(is_pointer_pseudo_symbol(sym));
                add_to_find_symbol_buf(sym);
            }
        }
    }
}

int find_symbol_by_name(Context * ctx, int frame, ContextAddress ip, const char * name, Symbol ** res) {
    int error = 0;
    ELF_File * curr_file = NULL;

    assert(ctx != NULL);
    find_symbol_list = NULL;
    *res = NULL;

    if (get_sym_context(ctx, frame, ip) < 0) error = errno;

    if (error == 0 && (sym_frame != STACK_NO_FRAME || sym_ip != 0)) {

        if (error == 0) {
            /* Search the name in the current compilation unit */
            Trap trap;
            if (set_trap(&trap)) {
                find_in_dwarf(name);
                clear_trap(&trap);
            }
            else {
                error = trap.error;
            }
        }

        assert(sym_ctx == ctx);

        if (error == 0 && should_continue_pub_names_search()) {
            /* Search in pub names of the current file */
            ELF_File * file = elf_list_first(sym_ctx, sym_ip, sym_ip);
            if (file == NULL) error = errno;
            while (error == 0 && file != NULL) {
                Trap trap;
                curr_file = file;
                if (set_trap(&trap)) {
                    DWARFCache * cache = get_dwarf_cache(get_dwarf_file(file));
                    find_by_name_in_pub_names(cache, name);
                    find_by_name_in_sym_table(file, name, 0);
                    clear_trap(&trap);
                }
                else {
                    error = trap.error;
                    break;
                }
                file = elf_list_next(sym_ctx);
                if (file == NULL) error = errno;
            }
            elf_list_done(sym_ctx);
        }

        if (error == 0 && find_symbol_list == NULL) {
            /* Check if the name is one of well known C/C++ names */
            Trap trap;
            if (set_trap(&trap)) {
                if (strcmp(name, "char") == 0) {
                    const char * alias = "signed char";
                    if (curr_file != NULL) {
                        switch (curr_file->machine) {
                        case EM_PPC:
                        case EM_PPC64:
                        case EM_ARM:
                        case EM_AARCH64:
                            alias = "unsigned char";
                            break;
                        }
                    }
                    find_in_dwarf(alias);
                }
                else {
                    unsigned i = 0;
                    while (base_types_aliases[i].name) {
                        if (strcmp(name, base_types_aliases[i].name) == 0) {
                            find_in_dwarf(base_types_aliases[i].alias);
                            if (find_symbol_list != NULL) break;
                        }
                        i++;
                    }
                }
                if (find_symbol_list == NULL) {
                    unsigned i = 0;
                    while (constant_pseudo_symbols[i].name) {
                        if (strcmp(name, constant_pseudo_symbols[i].name) == 0) {
                            Trap trap;
                            if (set_trap(&trap)) {
                                find_in_dwarf(constant_pseudo_symbols[i].type);
                                clear_trap(&trap);
                            }
                        }
                        i++;
                    }
                    if (curr_file != NULL) {
                        i = 0;
                        while (constant_pseudo_symbols[i].name) {
                            if (strcmp(name, constant_pseudo_symbols[i].name) == 0) {
                                Trap trap;
                                if (set_trap(&trap)) {
                                    DWARFCache * cache = get_dwarf_cache(get_dwarf_file(curr_file));
                                    find_by_name_in_pub_names(cache, constant_pseudo_symbols[i].type);
                                    clear_trap(&trap);
                                }
                            }
                            i++;
                        }
                    }
                    if (find_symbol_list != NULL) {
                        Symbol * sym = alloc_symbol();
                        sort_find_symbol_buf();
                        sym->ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
                        sym->frame = STACK_NO_FRAME;
                        sym->sym_class = SYM_CLASS_VALUE;
                        sym->base = find_symbol_list;
                        find_symbol_list = NULL;
                        i = 0;
                        while (constant_pseudo_symbols[i].name) {
                            if (strcmp(name, constant_pseudo_symbols[i].name) == 0 &&
                                strcmp(sym->base->obj->mName, constant_pseudo_symbols[i].type) == 0) {
                                break;
                            }
                            i++;
                        }
                        if (constant_pseudo_symbols[i].name) {
                            sym->index = i;
                            assert(is_constant_pseudo_symbol(sym));
                            add_to_find_symbol_buf(sym);
                        }
                    }
                }
                clear_trap(&trap);
            }
            else {
                error = trap.error;
            }
        }
    }

    if (error == 0 && should_continue_pub_names_search()) {
        /* Search in pub names of all other files */
        ELF_File * file = elf_list_first(sym_ctx, 0, ~(ContextAddress)0);
        if (file == NULL) error = errno;
        while (error == 0 && file != NULL) {
            if (file != curr_file) {
                Trap trap;
                if (set_trap(&trap)) {
                    DWARFCache * cache = get_dwarf_cache(get_dwarf_file(file));
                    if (strncmp(name, "$relocate:", 10) == 0) {
                        find_relocate(file, name + 10);
                    }
                    else {
                        find_by_name_in_pub_names(cache, name);
                        find_by_name_in_sym_table(file, name, 0);
                    }
                    clear_trap(&trap);
                }
                else {
                    error = trap.error;
                    break;
                }
            }
            file = elf_list_next(sym_ctx);
            if (file == NULL) error = errno;
        }
        elf_list_done(sym_ctx);
    }

    if (error == 0 && find_symbol_list == NULL) {
        unsigned i = 0;
        while (type_pseudo_symbols[i].name) {
            if (strcmp(name, type_pseudo_symbols[i].name) == 0) {
                Symbol * type = NULL;
                alloc_std_type_pseudo_symbol(
                    context_get_group(ctx, CONTEXT_GROUP_SYMBOLS),
                    type_pseudo_symbols[i].size, type_pseudo_symbols[i].type_class, &type);
                type->index = i + 1;
                add_to_find_symbol_buf(type);
                break;
            }
            i++;
        }
    }

#if defined(_WRS_KERNEL)
    if (error == 0 && find_symbol_list == NULL) {
        char * ptr;
        SYM_TYPE type;

        if (symFindByName(sysSymTbl, name, &ptr, &type) != OK) {
            error = errno;
            assert(error != 0);
            if (error == S_symLib_SYMBOL_NOT_FOUND) error = 0;
        }
        else {
            Symbol * sym = alloc_symbol();
            sym->ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
            sym->frame = STACK_NO_FRAME;
            sym->address = (ContextAddress)ptr;
            sym->has_address = 1;

            if (SYM_IS_TEXT(type)) {
                sym->sym_class = SYM_CLASS_FUNCTION;
            }
            else {
                sym->sym_class = SYM_CLASS_REFERENCE;
            }
            add_to_find_symbol_buf(sym);
        }
    }
#endif

    if (error == 0 && find_symbol_list == NULL) error = ERR_SYM_NOT_FOUND;

    if (error) {
        find_symbol_list = NULL;
    }
    else {
        sort_find_symbol_buf();
        *res = find_symbol_list;
        find_symbol_list = find_symbol_list->next;
    }

    assert(error || (*res != NULL && (*res)->ctx != NULL));

    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}

int find_symbol_in_scope(Context * ctx, int frame, ContextAddress ip, Symbol * scope, const char * name, Symbol ** res) {
    int error = 0;

    *res = NULL;
    find_symbol_list = NULL;
    if (get_sym_context(ctx, frame, ip) < 0) error = errno;

    if (error == 0 && scope == NULL && (sym_frame != STACK_NO_FRAME || sym_ip != 0)) {
        Trap trap;
        if (set_trap(&trap)) {
            ELF_File * file = NULL;
            ELF_Section * sec = NULL;
            ContextAddress addr = elf_map_to_link_time_address(ctx, sym_ip, 1, &file, &sec);
            if (file != NULL) {
                DWARFCache * cache = get_dwarf_cache(file);
                UnitAddressRange * range = find_comp_unit_addr_range(cache, sec, addr, addr);
                if (range != NULL) {
                    find_in_object_tree(range->mUnit->mObject, 2, NULL, name);
                }
                find_by_name_in_sym_table(file, name, 0);
            }
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }

    if (error == 0 && scope != NULL && scope->obj != NULL) {
        Trap trap;
        if (set_trap(&trap)) {
            find_in_object_tree(scope->obj, 2, NULL, name);
            if (find_symbol_list == NULL && (scope->obj->mTag == TAG_subprogram || scope->obj->mTag == TAG_global_subroutine)) {
                ObjectInfo * obj = get_dwarf_children(scope->obj);
                while (obj != NULL) {
                    if (obj->mTag == TAG_lexical_block) find_in_object_tree(obj, 3, NULL, name);
                    obj = obj->mSibling;
                }
            }
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }

    if (error == 0 && find_symbol_list == NULL) error = ERR_SYM_NOT_FOUND;

    if (error) {
        find_symbol_list = NULL;
    }
    else {
        sort_find_symbol_buf();
        *res = find_symbol_list;
        find_symbol_list = find_symbol_list->next;
    }

    assert(error || (*res != NULL && (*res)->ctx != NULL));

    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}

static int find_by_addr_in_unit(ObjectInfo * parent, int level, UnitAddress * addr, UnitAddress * ip, Symbol ** res) {
    int in_range = ip != NULL && check_in_range(parent, ip);
    ObjectInfo * obj = get_dwarf_children(parent);
    while (obj != NULL) {
        switch (obj->mTag) {
        case TAG_namespace:
        case TAG_compile_unit:
        case TAG_partial_unit:
        case TAG_module:
        case TAG_global_subroutine:
        case TAG_inlined_subroutine:
        case TAG_lexical_block:
        case TAG_with_stmt:
        case TAG_try_block:
        case TAG_catch_block:
        case TAG_subroutine:
        case TAG_subprogram:
            if (check_in_range(obj, addr)) {
                elf_object2symbol(NULL, obj, res);
                return 1;
            }
            if (find_by_addr_in_unit(obj, level + 1, addr, ip, res)) return 1;
            break;
        case TAG_formal_parameter:
        case TAG_unspecified_parameters:
        case TAG_local_variable:
            if (sym_frame == STACK_NO_FRAME) break;
        case TAG_variable:
            if (in_range && (obj->mFlags & DOIF_location) != 0) {
                U8_T lc = 0;
                /* Ignore location evaluation errors. For example, the error can be caused by
                 * the object not being mapped into the context memory */
                if (get_num_prop(obj, AT_location, &lc) && lc <= addr->rt_addr) {
                    U8_T sz = 0;
                    if (!get_num_prop(obj, AT_byte_size, &sz)) {
                        /* If object size unknown, continue search */
                        if (get_error_code(errno) == ERR_SYM_NOT_FOUND) break;
                        exception(errno);
                    }
                    if (lc + sz > addr->rt_addr) {
                        elf_object2symbol(NULL, obj, res);
                        return 1;
                    }
                }
            }
            break;
        }
        obj = obj->mSibling;
    }
    return 0;
}

static int is_valid_elf_symbol(ELF_SymbolInfo * info) {
    switch (info->type) {
    case STT_NOTYPE:
        if (info->name != NULL && info->name[0] == '$') break;
        return elf_symbol_has_address(info);
    case STT_FUNC:
    case STT_OBJECT:
        return 1;
    }
    return 0;
}

static int find_by_addr_in_sym_tables(ContextAddress addr, Symbol ** res) {
    ELF_File * file = NULL;
    ELF_Section * section = NULL;
    ELF_SymbolInfo sym_info;
    ContextAddress lt_addr = elf_map_to_link_time_address(sym_ctx, addr, 1, &file, &section);
    if (section == NULL) return 0;
    elf_find_symbol_by_address(section, lt_addr, &sym_info);
    while (sym_info.sym_section != NULL) {
        if (is_valid_elf_symbol(&sym_info)) {
            U8_T sym_addr = sym_info.value;
            if (file->type == ET_REL) sym_addr += section->addr;
            if (IS_PPC64_FUNC_OPD(file, &sym_info)) {
                /*
                 * For PPC64, an ELF function symbol address is not described by
                 * the symbol value. In that case the symbol value points to a
                 * function descriptor in the OPD section. The first entry of the
                 * descriptor is the real function address. This value is
                 * relocatable.
                 */
                U8_T offset;
                ELF_Section * opd = file->sections + file->section_opd;
                if (elf_load(opd) < 0) exception(errno);
                offset = sym_addr - opd->addr;
                sym_addr = *(U8_T *)((U1_T *)opd->data + offset);
                if (file->byte_swap) SWAP(sym_addr);
                drl_relocate(opd, offset, &sym_addr, sizeof(sym_addr), NULL);
            }
            assert(sym_addr <= (U8_T)lt_addr);
            /* Check if the searched address is part of symbol address range
             * or if symbol is a label (function + size of 0). */
            if (sym_addr + sym_info.size > (U8_T)lt_addr || sym_info.size == 0) {
                elf_tcf_symbol(sym_ctx, &sym_info, res);
                return 1;
            }
            return 0;
        }
        elf_prev_symbol_by_address(&sym_info);
    }
    if (section->name != NULL && strcmp(section->name, ".plt") == 0) {
        /* Create synthetic symbol for PLT section entry */
        unsigned first_size = 0;
        unsigned entry_size = 0;
        if (elf_get_plt_entry_size(file, &first_size, &entry_size) == 0 &&
                lt_addr >= section->addr + first_size && entry_size > 0) {
            Symbol * sym = alloc_symbol();
            sym->sym_class = SYM_CLASS_FUNCTION;
            sym->frame = STACK_NO_FRAME;
            sym->ctx = context_get_group(sym_ctx, CONTEXT_GROUP_SYMBOLS);
            sym->tbl = section;
            sym->index = (unsigned)((lt_addr - section->addr - first_size) / entry_size);
            sym->cardinal = first_size;
            sym->dimension = entry_size;
            *res = sym;
            return 1;
        }
    }
    return 0;
}

int find_symbol_by_addr(Context * ctx, int frame, ContextAddress addr, Symbol ** res) {
    Trap trap;
    int found = 0;
    UnitAddress loc;
    UnitAddress ip;

    find_symbol_list = NULL;
    if (!set_trap(&trap)) return -1;
    if (get_sym_context(ctx, frame, addr) < 0) exception(errno);
    find_unit(sym_ctx, addr, &loc);
    if (addr == sym_ip) ip = loc;
    else find_unit(sym_ctx, sym_ip, &ip);
    if (loc.unit != NULL) {
        found = find_by_addr_in_unit(loc.unit->mObject, 0, &loc, ip.unit ? &ip : NULL, res);
    }
    if (!found) found = find_by_addr_in_sym_tables(addr, res);
    if (!found && ip.unit != NULL) {
        /* Search in compilation unit that contains stack frame PC */
        if (loc.file == NULL) {
            loc.lt_addr = elf_map_to_link_time_address(sym_ctx, addr, 1, &loc.file, &loc.section);
        }
        if (loc.file != NULL) {
            found = find_by_addr_in_unit(ip.unit->mObject, 0, &loc, &ip, res);
        }
    }

#ifdef ELF_SYMS_BY_ADDR
    ELF_SYMS_BY_ADDR;
#endif

    if (!found) exception(ERR_SYM_NOT_FOUND);
    clear_trap(&trap);
    return 0;
}

int find_next_symbol(Symbol ** sym) {
    if (find_symbol_list != NULL) {
        *sym = find_symbol_list;
        find_symbol_list = find_symbol_list->next;
        return 0;
    }
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

typedef struct LocalVarsCallBack {
    EnumerateSymbolsCallBack * call_back;
    void * args;
    Symbol * sym;
} LocalVarsCallBack;

static void local_vars_call_back(void * args) {
    LocalVarsCallBack * cb = (LocalVarsCallBack *)args;
    cb->call_back(cb->args, cb->sym);
}

static void enumerate_local_vars(ObjectInfo * parent, int level,
        UnitAddress * ip, LocalVarsCallBack * call_back) {
    int in_range = check_in_range(parent, ip);
    ObjectInfo * obj = get_dwarf_children(parent);
    while (obj != NULL) {
        switch (obj->mTag) {
        case TAG_namespace:
        case TAG_compile_unit:
        case TAG_partial_unit:
        case TAG_module:
        case TAG_global_subroutine:
        case TAG_lexical_block:
        case TAG_with_stmt:
        case TAG_try_block:
        case TAG_catch_block:
        case TAG_subroutine:
        case TAG_subprogram:
            enumerate_local_vars(obj, level + 1, ip, call_back);
            break;
        case TAG_formal_parameter:
        case TAG_unspecified_parameters:
        case TAG_local_variable:
        case TAG_variable:
            if (level > 0 && in_range) {
                call_back->sym = NULL;
                elf_object2symbol(NULL, find_definition(obj), &call_back->sym);
                if (elf_save_symbols_state(local_vars_call_back, call_back) < 0) exception(errno);
            }
            break;
        }
        obj = obj->mSibling;
    }
}

int enumerate_symbols(Context * ctx, int frame, EnumerateSymbolsCallBack * call_back, void * args) {
    Trap trap;
    StackFrame * info = NULL;
    if (!set_trap(&trap)) return -1;
    /* Note: get_frame_info() destroys sym context, so it must be called before get_sym_context() */
    if (frame != STACK_NO_FRAME && get_frame_info(ctx, frame, &info) < 0) exception(errno);
    if (get_sym_context(ctx, frame, 0) < 0) exception(errno);
    if (sym_frame != STACK_NO_FRAME || sym_ip != 0) {
        UnitAddress ip;
        LocalVarsCallBack cb;
        memset(&cb, 0, sizeof(LocalVarsCallBack));
        cb.args = args;
        cb.call_back = call_back;
        find_unit(sym_ctx, sym_ip, &ip);
        if (ip.unit != NULL) {
            if (info != NULL && info->func_id != NULL) {
                Symbol * sym = NULL;
#if ENABLE_SymbolsMux
                if (symbols_mux_id2symbol(info->func_id, &sym) < 0) exception(errno);
                if (sym->reader != &symbol_reader) exception(set_errno(ERR_OTHER, "Invalid stack framme info"));
#else
                if (id2symbol(info->func_id, &sym) < 0) exception(errno);
#endif
                enumerate_local_vars(sym->obj, 1, &ip, &cb);
            }
            else {
                enumerate_local_vars(ip.unit->mObject, 0, &ip, &cb);
            }
        }
    }
    clear_trap(&trap);
    return 0;
}

static char tmp_buf[256];
static size_t tmp_len = 0;

#define tmp_app_char(ch) { \
    if (tmp_len < sizeof(tmp_buf) - 1) tmp_buf[tmp_len++] = ch; \
}

static void tmp_app_str(char ch, const char * s) {
    tmp_app_char(ch);
    for (;;) {
        ch = *s++;
        if (ch == 0) break;
        tmp_app_char(ch);
    }
}

static void tmp_app_hex(char ch, uint64_t n) {
    char buf[32];
    unsigned i = 0;
    tmp_app_char(ch);
    do {
        int d = (int)(n & 0xf);
        buf[i++] = (char)(d < 10 ? '0' + d : 'A' + d - 10);
        n = n >> 4;
    }
    while (n != 0);
    while (i > 0) {
        ch = buf[--i];
        tmp_app_char(ch);
    }
}

const char * symbol2id(const Symbol * sym) {
    int frame = sym->frame;
    assert(sym->magic == SYMBOL_MAGIC);
    if (frame == STACK_TOP_FRAME) frame = get_top_frame(sym->ctx);
    if (sym->base) {
        char base[256];
        assert(sym->ctx == sym->base->ctx);
        strlcpy(base, symbol2id(sym->base), sizeof(base));
        tmp_len = 0;
        tmp_app_char('@');
        tmp_app_hex('P', sym->sym_class);
        if (frame != STACK_TOP_FRAME) {
            assert(frame + 3 >= 0);
            tmp_app_hex('+', frame + 3);
        }
        tmp_app_hex('.', sym->index);
        tmp_app_hex('.', sym->length);
        tmp_app_str('.', base);
    }
    else {
        ELF_File * obj_file = NULL;
        ELF_File * var_file = NULL;
        ELF_File * ref_file = NULL;
        unsigned obj_sec = 0;
        unsigned var_sec = 0;
        unsigned ref_sec = 0;
        uint64_t obj_id = 0;
        uint64_t var_id = 0;
        uint64_t ref_id = 0;
        unsigned tbl_index = 0;
        if (sym->obj != NULL) obj_file = sym->obj->mCompUnit->mFile;
        if (sym->tbl != NULL) obj_file = sym->tbl->file;
        if (sym->obj != NULL) {
            obj_sec = sym->obj->mCompUnit->mDesc.mSection->index;
            obj_id = sym->obj->mID;
        }
        if (sym->var != NULL) {
            var_file = sym->var->mCompUnit->mFile;
            var_sec = sym->var->mCompUnit->mDesc.mSection->index;
            var_id = sym->var->mID;
        }
        if (sym->ref != NULL) {
            ref_file = sym->ref->mCompUnit->mFile;
            ref_sec = sym->ref->mCompUnit->mDesc.mSection->index;
            ref_id = sym->ref->mID;
        }
        if (sym->tbl != NULL) tbl_index = sym->tbl->index;
        tmp_len = 0;
        tmp_app_char('@');
        tmp_app_hex('S', sym->sym_class);
        if (obj_file != NULL) {
            tmp_app_hex('%', obj_file->dev);
            tmp_app_hex('.', obj_file->ino);
            tmp_app_hex('.', obj_file->mtime);
            tmp_app_hex('.', obj_sec);
            tmp_app_hex('.', obj_id);
        }
        if (var_file != NULL) {
            tmp_app_hex('^', var_file->dev);
            tmp_app_hex('.', var_file->ino);
            tmp_app_hex('.', var_file->mtime);
            tmp_app_hex('.', var_sec);
            tmp_app_hex('.', var_id);
        }
        if (ref_file != NULL) {
            if (ref_file == obj_file && ref_sec == obj_sec && ref_id == obj_id) {
                tmp_app_char('&');
            }
            else {
                tmp_app_hex('*', ref_file->dev);
                tmp_app_hex('.', ref_file->ino);
                tmp_app_hex('.', ref_file->mtime);
                tmp_app_hex('.', ref_sec);
                tmp_app_hex('.', ref_id);
            }
        }
        if (sym->has_address) tmp_app_hex('$', sym->address);
        if (sym->assembly_function) tmp_app_hex('#', sym->size);
        if (tbl_index) tmp_app_hex('-', tbl_index);
        if (frame != STACK_TOP_FRAME) {
            assert(frame + 3 >= 0);
            tmp_app_hex('+', frame + 3);
        }
        if (sym->index) tmp_app_hex('=', sym->index);
        if (sym->dimension) tmp_app_hex('!', sym->dimension);
        if (sym->cardinal) tmp_app_hex('/', sym->cardinal);
        tmp_app_str('.', sym->ctx->id);
    }
    tmp_buf[tmp_len++] = 0;
    return tmp_buf;
}

static uint64_t read_hex(const char ** s) {
    uint64_t res = 0;
    const char * p = *s;
    for (;;) {
        if (*p >= '0' && *p <= '9') res = (res << 4) | (*p - '0');
        else if (*p >= 'A' && *p <= 'F') res = (res << 4) | (*p - 'A' + 10);
        else break;
        p++;
    }
    *s = p;
    return res;
}

typedef struct FileID {
    int valid;
    dev_t dev;
    ino_t ino;
    int64_t mtime;
    unsigned obj_sec;
    ContextAddress obj_id;
    ELF_File * file;
} FileID;

static void read_file_id(const char ** s, FileID * id) {
    const char * p = *s;
    p++;
    id->dev = (dev_t)read_hex(&p);
    if (*p == '.') p++;
    id->ino = (ino_t)read_hex(&p);
    if (*p == '.') p++;
    id->mtime = (int64_t)read_hex(&p);
    if (*p == '.') p++;
    id->obj_sec = (unsigned)read_hex(&p);
    if (*p == '.') p++;
    id->obj_id = (ContextAddress)read_hex(&p);
    id->valid = 1;
    *s = p;
}

int id2symbol(const char * id, Symbol ** res) {
    Symbol * sym = alloc_symbol();
    unsigned tbl_index = 0;
    const char * p;
    Trap trap;

    *res = sym;
    sym->frame = STACK_NO_FRAME;
    if (id != NULL && id[0] == '@' && id[1] == 'P') {
        p = id + 2;
        sym->sym_class = (int8_t)read_hex(&p);
        if (*p == '+') {
            p++;
            sym->frame = (int)read_hex(&p) - 3;
        }
        if (*p == '.') p++;
        sym->index = (unsigned)read_hex(&p);
        if (*p == '.') p++;
        sym->length = (ContextAddress)read_hex(&p);
        if (*p == '.') p++;
        if (id2symbol(p, &sym->base)) return -1;
        sym->ctx = sym->base->ctx;
        return 0;
    }
    else if (id != NULL && id[0] == '@' && id[1] == 'S') {
        FileID obj;
        FileID var;
        FileID ref;
        memset(&obj, 0, sizeof(FileID));
        memset(&var, 0, sizeof(FileID));
        memset(&ref, 0, sizeof(FileID));
        p = id + 2;
        sym->sym_class = (int8_t)read_hex(&p);
        if (*p == '%') read_file_id(&p, &obj);
        if (*p == '^') read_file_id(&p, &var);
        if (*p == '*') read_file_id(&p, &ref);
        if (*p == '&') {
            p++;
            ref = obj;
        }
        if (*p == '$') {
            p++;
            sym->has_address = 1;
            sym->address = (ContextAddress)read_hex(&p);
        }
        if (*p == '#') {
            p++;
            sym->assembly_function = 1;
            sym->size = (ContextAddress)read_hex(&p);
        }
        if (*p == '-') {
            p++;
            tbl_index = (unsigned)read_hex(&p);
        }
        if (*p == '+') {
            p++;
            sym->frame = (int)read_hex(&p) - 3;
        }
        if (*p == '=') {
            p++;
            sym->index = (unsigned)read_hex(&p);
        }
        if (*p == '!') {
            p++;
            sym->dimension = (unsigned)read_hex(&p);
        }
        if (*p == '/') {
            p++;
            sym->cardinal = (unsigned)read_hex(&p);
        }
        if (*p++ != '.') {
            errno = ERR_INV_CONTEXT;
            return -1;
        }
        sym->ctx = id2ctx(p);
        if (sym->ctx == NULL) {
            errno = ERR_INV_CONTEXT;
            return -1;
        }
        if (obj.valid) {
            obj.file = elf_open_inode(sym->ctx, obj.dev, obj.ino, obj.mtime);
            if (obj.file == NULL) return -1;
        }
        if (var.valid) {
            var.file = elf_open_inode(sym->ctx, var.dev, var.ino, var.mtime);
            if (var.file == NULL) return -1;
        }
        if (ref.valid) {
            ref.file = elf_open_inode(sym->ctx, ref.dev, ref.ino, ref.mtime);
            if (ref.file == NULL) return -1;
        }
        if (set_trap(&trap)) {
            if (obj.obj_sec) {
                if (obj.obj_sec >= obj.file->section_cnt) exception(ERR_INV_CONTEXT);
                sym->obj = find_object(obj.file->sections + obj.obj_sec, obj.obj_id);
                if (sym->obj == NULL) exception(ERR_INV_CONTEXT);
            }
            if (var.obj_sec) {
                if (var.obj_sec >= var.file->section_cnt) exception(ERR_INV_CONTEXT);
                sym->var = find_object(var.file->sections + var.obj_sec, var.obj_id);
                if (sym->var == NULL) exception(ERR_INV_CONTEXT);
            }
            if (ref.obj_sec) {
                if (ref.obj_sec >= ref.file->section_cnt) exception(ERR_INV_CONTEXT);
                sym->ref = find_object(ref.file->sections + ref.obj_sec, ref.obj_id);
                if (sym->ref == NULL) exception(ERR_INV_CONTEXT);
            }
            if (tbl_index && obj.valid) {
                if (tbl_index >= obj.file->section_cnt) exception(ERR_INV_CONTEXT);
                sym->tbl = obj.file->sections + tbl_index;
            }
            clear_trap(&trap);
            return 0;
        }
    }
    else {
        errno = ERR_INV_CONTEXT;
    }
    return -1;
}

ContextAddress is_plt_section(Context * ctx, ContextAddress addr) {
    ELF_File * file = NULL;
    ELF_Section * sec = NULL;
    ContextAddress res = elf_map_to_link_time_address(ctx, addr, 0, &file, &sec);
    if (res == 0 || sec == NULL) return 0;
    if (sec->name == NULL) return 0;
    if (strcmp(sec->name, ".plt") != 0) return 0;
    return (ContextAddress)sec->addr + (addr - res);
}

int get_context_isa(Context * ctx, ContextAddress ip, const char ** isa,
        ContextAddress * range_addr, ContextAddress * range_size) {
    ELF_File * file = NULL;
    ELF_Section * sec = NULL;
    ContextAddress lt_addr = elf_map_to_link_time_address(ctx, ip, 1, &file, &sec);
    *isa = NULL;
    *range_addr = ip;
    *range_size = 1;
    if (sec != NULL && file->machine == EM_ARM) {
        /* TODO: faster handling of ARM mapping symbols */
        ELF_SymbolInfo sym_info;
        elf_find_symbol_by_address(sec, lt_addr, &sym_info);
        if (sym_info.type16bit && sym_info.value + sym_info.size > lt_addr) {
            *isa = "Thumb";
            assert(sym_info.size > 0);
            assert(sym_info.value <= lt_addr);
            *range_addr = (ContextAddress)sym_info.value;
            if (file->type == ET_REL) *range_addr += (ContextAddress)sec->addr;
            *range_addr += ip - lt_addr;
            *range_size = sym_info.size;
            return 0;
        }
        while (sym_info.sym_section != NULL) {
            assert(sym_info.section == sec);
            if (sym_info.name != NULL && *sym_info.name == '$') {
                if (strcmp(sym_info.name, "$a") == 0) *isa = "ARM";
                else if (strcmp(sym_info.name, "$t") == 0) *isa = "Thumb";
                else if (strcmp(sym_info.name, "$t.x") == 0) *isa = "ThumbEE";
                else if (strcmp(sym_info.name, "$d") == 0) *isa = "Data";
                else if (strcmp(sym_info.name, "$d.realdata") == 0) *isa = "Data";
                if (*isa) {
                    *range_addr = (ContextAddress)sym_info.value;
                    if (file->type == ET_REL) *range_addr += (ContextAddress)sec->addr;
                    for (;;) {
                        elf_next_symbol_by_address(&sym_info);
                        if (sym_info.sym_section == NULL) {
                            *range_size = (ContextAddress)(sec->addr + sec->size) - *range_addr;
                            *range_addr += ip - lt_addr;
                            return 0;
                        }
                        if (sym_info.name != NULL && *sym_info.name == '$') {
                            ContextAddress sym_addr = (ContextAddress)sym_info.value;
                            if (file->type == ET_REL) sym_addr += (ContextAddress)sec->addr;
                            *range_size = sym_addr - *range_addr;
                            *range_addr += ip - lt_addr;
                            return 0;
                        }
                    }
                }
            }
            elf_prev_symbol_by_address(&sym_info);
        }
    }
    else if (file != NULL) {
        switch (file->machine) {
        case EM_M32        : *isa = "M32"; break;
        case EM_SPARC      : *isa = "SPARC"; break;
        case EM_386        : *isa = "386"; break;
        case EM_68K        : *isa = "68K"; break;
        case EM_88K        : *isa = "88K"; break;
        case EM_860        : *isa = "860"; break;
        case EM_MIPS       : *isa = "MIPS"; break;
        case EM_PPC        : *isa = "PPC"; break;
        case EM_PPC64      : *isa = "PPC64"; break;
        case EM_SH         : *isa = "SH"; break;
        case EM_SPARCV9    : *isa = "SPARCV9"; break;
        case EM_TRICORE    : *isa = "TRICORE"; break;
        case EM_IA_64      : *isa = "IA_64"; break;
        case EM_MIPS_X     : *isa = "MIPS_X"; break;
        case EM_COLDFIRE   : *isa = "COLDFIRE"; break;
        case EM_X86_64     : *isa = "X86_64"; break;
        case EM_MICROBLAZE : *isa = file->elf64 ? "MicroBlaze64" : "MicroBlaze"; break;
        case EM_V800       : *isa = "V800"; break;
        case EM_V850       : *isa = "V850"; break;
        case EM_AARCH64    : *isa = "A64"; break;
        case EM_RISCV      : *isa = file->elf64 ? "Riscv64" : "Riscv32"; break;
        }
    }
    {
        unsigned i;
        static MemoryMap map;
        ContextAddress size = 1 << 16;
        ContextAddress addr = ip & ~(size - 1);
        if (elf_get_map(ctx, addr, addr + size - 1, &map) == 0) {
            for (i = 0; i < map.region_cnt; i++) {
                MemoryRegion * r = map.regions + i;
                ContextAddress x = r->addr;
                ContextAddress y = r->addr + r->size;
                if (x > ip && x < addr + size) size = x - addr;
                if (y > ip && y < addr + size) size = y - addr;
                if (x <= ip && x > addr) {
                    size = addr + size - x;
                    addr = x;
                }
                if (y <= ip && y > addr) {
                    size = addr + size - y;
                    addr = y;
                }
            }
            assert(addr <= ip);
            assert(addr + size - 1 >= ip);
            *range_addr = addr;
            *range_size = size;
        }
    }
    return 0;
}

static int buf_sub_max = 0;

static void add_inlined_subroutine(ObjectInfo * o, CompUnit * unit, ContextAddress addr0, ContextAddress addr1, StackTracingInfo * buf) {
    StackFrameInlinedSubroutine * sub = (StackFrameInlinedSubroutine *)tmp_alloc(sizeof(StackFrameInlinedSubroutine));
    Symbol * sym = NULL;
    U8_T call_file = 0;
    CodeArea area;
    memset(&area, 0, sizeof(area));
    if (get_num_prop(o, AT_call_file, &call_file)) {
        U8_T call_line = 0;
        load_line_numbers(unit);
        area.directory = unit->mDir;
        if (call_file < unit->mFilesCnt) {
            FileInfo * file_info = unit->mFiles + (int)call_file;
            if (is_absolute_path(file_info->mName) || file_info->mDir == NULL) {
                area.file = file_info->mName;
            }
            else if (is_absolute_path(file_info->mDir)) {
                area.directory = file_info->mDir;
                area.file = file_info->mName;
            }
            else {
                char buf[FILE_PATH_SIZE];
                snprintf(buf, sizeof(buf), "%s/%s", file_info->mDir, file_info->mName);
                area.file = tmp_strdup(buf);
            }
            area.file_mtime = file_info->mModTime;
            area.file_size = file_info->mSize;
        }
        else {
            area.file = unit->mObject->mName;
        }
        if (get_num_prop(o, AT_call_line, &call_line)) {
            area.start_line = (int)call_line;
            area.end_line = (int)call_line + 1;
        }
    }
    elf_object2symbol(NULL, o, &sym);
#if ENABLE_SymbolsMux
    sub->func_id = tmp_strdup(symbols_mux_symbol2id(sym));
#else
    sub->func_id = tmp_strdup(symbol2id(sym));
#endif
    sub->area = area;
    sub->area.start_address = addr0;
    sub->area.end_address = addr1;
    if (buf->sub_cnt >= buf_sub_max) {
        buf_sub_max += 16;
        buf->subs = (StackFrameInlinedSubroutine **)tmp_realloc(
            buf->subs, sizeof(StackFrameInlinedSubroutine *) * buf_sub_max);
    }
    buf->subs[buf->sub_cnt++] = sub;
}

static void search_inlined_subroutine(Context * ctx, ObjectInfo * obj, UnitAddress * addr, StackTracingInfo * buf) {
    ObjectInfo * o = get_dwarf_children(obj);
    while (o != NULL) {
        switch (o->mTag) {
        case TAG_compile_unit:
        case TAG_partial_unit:
        case TAG_module:
        case TAG_global_subroutine:
        case TAG_lexical_block:
        case TAG_with_stmt:
        case TAG_try_block:
        case TAG_catch_block:
        case TAG_subroutine:
        case TAG_subprogram:
        case TAG_inlined_subroutine:
            if (o->mFlags & DOIF_ranges) {
                DWARFCache * cache = get_dwarf_cache(addr->file);
                ELF_Section * debug_ranges = cache->mDebugRanges;
                if (debug_ranges != NULL) {
                    CompUnit * unit = addr->unit;
                    ContextAddress base = unit->mObject->u.mCode.mLowPC;
                    dio_EnterSection(&unit->mDesc, debug_ranges, o->u.mCode.mHighPC.mRanges);
                    for (;;) {
                        ELF_Section * x_sec = NULL;
                        ELF_Section * y_sec = NULL;
                        U8_T x = dio_ReadAddress(&x_sec);
                        U8_T y = dio_ReadAddress(&y_sec);
                        if (x == 0 && y == 0) break;
                        if (x == ((U8_T)1 << unit->mDesc.mAddressSize * 8) - 1) {
                            base = (ContextAddress)y;
                        }
                        else {
                            if (x_sec == NULL) x_sec = unit->mTextSection;
                            if (y_sec == NULL) y_sec = unit->mTextSection;
                            if (x_sec == addr->section && y_sec == addr->section && x < y) {
                                ContextAddress addr0 = base + x - addr->lt_addr + addr->rt_addr;
                                ContextAddress addr1 = base + y - addr->lt_addr + addr->rt_addr;
                                if (addr0 <= addr->rt_addr && addr1 > addr->rt_addr) {
                                    U8_T pos = dio_GetPos();
                                    dio_ExitSection();
                                    if (buf->addr < addr0) {
                                        assert(buf->addr + buf->size > addr0);
                                        buf->size = buf->addr + buf->size - addr0;
                                        buf->addr = addr0;
                                    }
                                    if (buf->addr + buf->size > addr1) {
                                        assert(addr1 > buf->addr);
                                        buf->size = addr1 - buf->addr;
                                    }
                                    if (o->mTag == TAG_inlined_subroutine) add_inlined_subroutine(o, unit, addr0, addr1, buf);
                                    search_inlined_subroutine(ctx, o, addr, buf);
                                    dio_EnterSection(&unit->mDesc, debug_ranges, pos);
                                }
                                else if (addr1 <= addr->rt_addr && addr1 > buf->addr) {
                                    buf->size = buf->addr + buf->size - addr1;
                                    buf->addr = addr1;
                                }
                                else if (addr0 > addr->rt_addr && addr0 < buf->addr + buf->size) {
                                    buf->size = addr0 - buf->addr;
                                }
                            }
                        }
                    }
                    dio_ExitSection();
                }
            }
            else if ((o->mFlags & DOIF_low_pc) && o->u.mCode.mHighPC.mAddr > o->u.mCode.mLowPC && o->u.mCode.mSection == addr->section) {
                ContextAddress addr0 = o->u.mCode.mLowPC - addr->lt_addr + addr->rt_addr;
                ContextAddress addr1 = o->u.mCode.mHighPC.mAddr - addr->lt_addr + addr->rt_addr;
                if (addr0 <= addr->rt_addr && addr1 > addr->rt_addr) {
                    if (buf->addr < addr0) {
                        assert(buf->addr + buf->size > addr0);
                        buf->size = buf->addr + buf->size - addr0;
                        buf->addr = addr0;
                    }
                    if (buf->addr + buf->size > addr1) {
                        assert(addr1 > buf->addr);
                        buf->size = addr1 - buf->addr;
                    }
                    if (o->mTag == TAG_inlined_subroutine) add_inlined_subroutine(o, addr->unit, addr0, addr1, buf);
                    search_inlined_subroutine(ctx, o, addr, buf);
                }
                else if (addr1 <= addr->rt_addr && addr1 > buf->addr) {
                    buf->size = buf->addr + buf->size - addr1;
                    buf->addr = addr1;
                }
                else if (addr0 > addr->rt_addr && addr0 < buf->addr + buf->size) {
                    buf->size = addr0 - buf->addr;
                }
            }
            break;
        }
        o = o->mSibling;
    }
}

int get_stack_tracing_info(Context * ctx, ContextAddress rt_addr, StackTracingInfo ** info) {
    /* TODO: no debug info exists for linux-gate.so, need to read stack tracing information from the kernel  */
    Trap trap;

    *info = NULL;

    if (set_trap(&trap)) {
        ELF_File * file = NULL;
        ELF_Section * sec = NULL;
        ContextAddress lt_addr = elf_map_to_link_time_address(ctx, rt_addr, 0, &file, &sec);
        if (file != NULL) {
            get_dwarf_stack_frame_info(ctx, file, sec, lt_addr);
            if (dwarf_stack_trace_fp->cmds_cnt > 0) {
                static StackTracingInfo buf;
                memset(&buf, 0, sizeof(buf));
                assert(dwarf_stack_trace_addr <= lt_addr);
                assert(dwarf_stack_trace_addr + dwarf_stack_trace_size > lt_addr);
                buf.addr = (ContextAddress)dwarf_stack_trace_addr - lt_addr + rt_addr;
                buf.size = (ContextAddress)dwarf_stack_trace_size;
                buf.regs = dwarf_stack_trace_regs;
                buf.reg_cnt = dwarf_stack_trace_regs_cnt;
                buf.fp = dwarf_stack_trace_fp;
                if (get_sym_context(ctx, STACK_NO_FRAME, rt_addr) == 0) {
                    /* Search inlined functions info.
                     * Note: when debug info is a separate file,
                     * 'lt_addr' is not same as 'unit.lt_addr' */
                    UnitAddress unit;
                    find_unit(ctx, rt_addr, &unit);
                    if (unit.unit != NULL) {
                        buf_sub_max = 0;
                        assert(buf.addr <= unit.rt_addr);
                        assert(buf.addr + buf.size == 0 || buf.addr + buf.size > unit.rt_addr);
                        search_inlined_subroutine(ctx, unit.unit->mObject, &unit, &buf);
                    }
                }
                *info = &buf;
            }
        }
        clear_trap(&trap);
        return 0;
    }
    return -1;
}

int get_symbol_file_info(Context * ctx, ContextAddress addr, SymbolFileInfo ** info) {
    MemoryRegion * region = NULL;
    ELF_File * file = elf_list_first(ctx, addr, addr);
    ELF_File * syms = get_dwarf_file(file);
    if (file == NULL) {
        *info = NULL;
        if (errno) return -1;
        return 0;
    }
    region = elf_list_region(ctx);
    *info = (SymbolFileInfo *)tmp_alloc_zero(sizeof(SymbolFileInfo));
    (*info)->file_name = syms->name;
    (*info)->addr = region->addr;
    (*info)->size = region->size;
    {
        size_t l;
        char * fnm = file->name + strlen(file->name);
        while (fnm > file->name && *(fnm - 1) != '/' && *(fnm - 1) != '\\') fnm--;
        l = strlen(fnm);
        if (l > 6 && strncmp(fnm, "ld-", 3) == 0 && strcmp(fnm + l - 3, ".so") == 0) {
            (*info)->dyn_loader = 1;
        }
    }
    elf_list_done(ctx);
    return 0;
}

#if ENABLE_MemoryMap
static void event_map_changed(Context * ctx, void * args) {
    /* Make sure there is no stale data in the ELF cache */
    elf_invalidate();
}

static MemoryMapEventListener map_listener = {
    event_map_changed,
    NULL,
    NULL,
    event_map_changed,
};
#endif

void ini_symbols_lib(void) {
#if ENABLE_MemoryMap
    add_memory_map_event_listener(&map_listener, NULL);
#endif
#if ENABLE_SymbolsMux
    add_symbols_reader(&symbol_reader);
#endif
}

#if ENABLE_SymbolsMux
static int reader_is_valid(Context * ctx, ContextAddress addr) {
    static MemoryMap map;
    unsigned i;
    if (elf_get_map(ctx, addr, addr, &map) < 0) return 0;
    for (i = 0; i < map.region_cnt; i++) {
        if (elf_open_memory_region_file(map.regions + i, NULL) != NULL) return 1;
    }
    return 0;
}
#endif

/*************** Functions for retrieving symbol properties ***************************************/

static int unpack(const Symbol * sym) {
    assert(sym->base == NULL);
    assert(!is_array_type_pseudo_symbol(sym));
    assert(!is_std_type_pseudo_symbol(sym));
    assert(!is_constant_pseudo_symbol(sym));
    assert(sym->obj == NULL || sym->obj->mTag != 0);
    assert(sym->obj == NULL || sym->obj->mCompUnit->mFile->dwarf_dt_cache != NULL);
    return get_sym_context(sym->ctx, sym->frame, 0);
}

static U8_T get_default_lower_bound(ObjectInfo * obj) {
    switch (obj->mCompUnit->mLanguage) {
    case LANG_ADA83:
    case LANG_ADA95:
    case LANG_COBOL74:
    case LANG_COBOL85:
    case LANG_FORTRAN77:
    case LANG_FORTRAN90:
    case LANG_FORTRAN95:
    case LANG_MODULA2:
    case LANG_PASCAL83:
    case LANG_PLI:
        return 1;
    }
    return 0;
}

static U8_T get_array_index_length(ObjectInfo * ref, ObjectInfo * obj) {
    U8_T x, y;

    if (get_variable_num_prop(ref, obj, AT_count, &x)) return x;
    if (get_error_code(errno) != ERR_SYM_NOT_FOUND) exception(errno);

    if (get_variable_num_prop(ref, obj, AT_upper_bound, &x)) {
        if (!get_variable_num_prop(ref, obj, AT_lower_bound, &y)) {
            if (get_error_code(errno) != ERR_SYM_NOT_FOUND) exception(errno);
            y = get_default_lower_bound(obj);
        }
        return x + 1 - y;
    }
    if (get_error_code(errno) != ERR_SYM_NOT_FOUND) {
        str_exception(errno, "Cannot get array upper bound");
    }
    if (obj->mTag == TAG_enumeration_type) {
        ObjectInfo * c = get_dwarf_children(obj);
        x = 0;
        while (c != NULL) {
            x++;
            c = c->mSibling;
        }
        return x;
    }
    return 0;
}

static int map_to_sym_table(ObjectInfo * obj, Symbol ** sym) {
    Trap trap;
    Symbol * list = find_symbol_list;
    *sym = NULL;
    if (set_trap(&trap)) {
        ELF_File * file = obj->mCompUnit->mFile;
        if (file->debug_info_file) {
            size_t n = strlen(file->name);
            if (n > 6 && strcmp(file->name + n - 6, ".debug") == 0) {
                char * fnm = (char *)tmp_alloc_zero(n);
                memcpy(fnm, file->name, n - 6);
                fnm = canonicalize_file_name(fnm);
                if (fnm != NULL) {
                    file = elf_open(fnm);
                    free(fnm);
                }
            }
        }
        if (file != NULL) {
            const char * name = get_linkage_name(obj);
            if (name != NULL) {
                find_symbol_list = NULL;
                /* AT_external means externally visible, so we can limit to global search */
                find_by_name_in_sym_table(file, name, obj->mFlags & DOIF_external ? 1 : 0);
                while (find_symbol_list != NULL) {
                    Symbol * s = find_symbol_list;
                    find_symbol_list = find_symbol_list->next;
                    if (s->obj != obj) {
                        *sym = s;
                        break;
                    }
                }
            }
        }
        clear_trap(&trap);
    }
    find_symbol_list = list;
    return *sym != NULL;
}

static U8_T read_string_length(ObjectInfo * obj);

static int get_object_size(ObjectInfo * ref, ObjectInfo * obj, unsigned dimension, U8_T * byte_size, U8_T * bit_size) {
    U8_T n = 0, m = 0;
    obj = find_definition(obj);
    if (obj->mTag != TAG_string_type && dimension == 0) {
        if (get_variable_num_prop(ref, obj, AT_byte_size, &n)) {
            *byte_size = n;
            return 1;
        }
        if (get_error_code(errno) != ERR_SYM_NOT_FOUND) exception(errno);
        if (get_variable_num_prop(ref, obj, AT_bit_size, &n)) {
            *byte_size = (n + 7) / 8;
            *bit_size = n;
            return 1;
        }
        if (get_error_code(errno) != ERR_SYM_NOT_FOUND) exception(errno);
    }
    switch (obj->mTag) {
    case TAG_enumerator:
    case TAG_formal_parameter:
    case TAG_unspecified_parameters:
    case TAG_template_type_param:
    case TAG_global_variable:
    case TAG_local_variable:
    case TAG_variable:
    case TAG_inheritance:
    case TAG_member:
    case TAG_constant:
    case TAG_const_type:
    case TAG_volatile_type:
    case TAG_restrict_type:
    case TAG_shared_type:
    case TAG_typedef:
    case TAG_subrange_type:
        if (obj->mType == NULL) return 0;
        return get_object_size(ref, obj->mType, 0, byte_size, bit_size);
    case TAG_compile_unit:
    case TAG_partial_unit:
    case TAG_module:
    case TAG_global_subroutine:
    case TAG_inlined_subroutine:
    case TAG_lexical_block:
    case TAG_with_stmt:
    case TAG_try_block:
    case TAG_catch_block:
    case TAG_subroutine:
    case TAG_subprogram:
        if (!(obj->mFlags & DOIF_ranges) && (obj->mFlags & DOIF_low_pc) &&
                obj->u.mCode.mHighPC.mAddr >= obj->u.mCode.mLowPC) {
            *byte_size = obj->u.mCode.mHighPC.mAddr - obj->u.mCode.mLowPC;
            return 1;
        }
        return 0;
    case TAG_string_type:
        *byte_size = read_string_length(obj);
        return 1;
    case TAG_array_type:
        {
            unsigned i = 0;
            U8_T length = 1;
            ObjectInfo * idx = get_dwarf_children(obj);
            while (idx != NULL) {
                if (i++ >= dimension) length *= get_array_index_length(ref, idx);
                idx = idx->mSibling;
            }
            if (get_num_prop(obj, AT_stride_size, &n)) {
                *byte_size = (n * length + 7) / 8;
                *bit_size = n * length;
                return 1;
            }
            if (obj->mType == NULL) return 0;
            if (!get_object_size(ref, obj->mType, 0, &n, &m)) return 0;
            if (m != 0) {
                *byte_size = (m * length + 7) / 8;
                *bit_size = m * length;
            }
            *byte_size = n * length;
        }
        return 1;
    case TAG_structure_type:
        {
            ObjectInfo * type = get_object_ref_prop(obj, AT_GNAT_descriptive_type);
            if (type != NULL) return get_object_size(ref, type, 0, byte_size, bit_size);
        }
        break;
    }
    return 0;
}

static void read_object_value(PropertyValue * v, void ** value, size_t * size, int * big_endian) {
    if (v->mPieces != NULL) {
        StackFrame * frame = NULL;
        if (get_frame_info(v->mContext, v->mFrame, &frame) < 0) exception(errno);
        read_location_pieces(v->mContext, frame, v->mPieces, v->mPieceCnt, v->mBigEndian, value, size);
        *big_endian = v->mBigEndian;
    }
    else if (v->mAddr != NULL) {
        *value = v->mAddr;
        *size = v->mSize;
        *big_endian = v->mBigEndian;
    }
    else {
        U1_T * bf = NULL;
        U8_T val_size = 0;
        U8_T bit_size = 0;

        if (v->mAttr == AT_string_length) {
            if (!get_num_prop(v->mObject, AT_byte_size, &val_size)) {
                val_size = v->mObject->mCompUnit->mDesc.mAddressSize;
            }
        }
        else if (!get_object_size(v->mObject, v->mObject, 0, &val_size, &bit_size)) {
            str_exception(ERR_INV_DWARF, "Unknown object size");
        }
        bf = (U1_T *)tmp_alloc((size_t)val_size);
        if (v->mForm == FORM_EXPR_VALUE) {
            if (context_read_mem(sym_ctx, (ContextAddress)v->mValue, bf, (size_t)val_size) < 0) exception(errno);
            *big_endian = v->mBigEndian;
        }
        else {
            U1_T * p = (U1_T *)&v->mValue;
            if (val_size > sizeof(v->mValue)) str_exception(ERR_INV_DWARF, "Unknown object size");
            if (big_endian_host()) p += sizeof(v->mValue) - (size_t)val_size;
            memcpy(bf, p, (size_t)val_size);
            *big_endian = big_endian_host();
        }
        if (bit_size % 8 != 0) bf[bit_size / 8] &= (1 << (bit_size % 8)) - 1;
        *size = (size_t)val_size;
        *value = bf;
    }
}

static U8_T read_cardinal_object_value(PropertyValue * v) {
    void * value = NULL;
    size_t size = 0;
    size_t i = 0;
    int big_endian = 0;
    U8_T n = 0;

    read_object_value(v, &value, &size, &big_endian);
    if (size > 8) str_exception(ERR_INV_DWARF, "Invalid object size");
    for (i = 0; i < size; i++) {
        n = (n << 8) | ((U1_T *)value)[big_endian ? i : size - i - 1];
    }
    return n;
}

static U8_T read_string_length(ObjectInfo * obj) {
    Trap trap;
    U8_T len = 0;

    assert(obj->mTag == TAG_string_type);
    if (set_trap(&trap)) {
        PropertyValue v;
        read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, AT_string_length, &v);
        len = read_cardinal_object_value(&v);
        clear_trap(&trap);
        return len;
    }
    else if (trap.error != ERR_SYM_NOT_FOUND) {
        exception(trap.error);
    }
    if (get_num_prop(obj, AT_byte_size, &len)) return len;
    str_exception(ERR_INV_DWARF, "Unknown length of a string type");
    return 0;
}

int get_symbol_class(const Symbol * sym, int * sym_class) {
    assert(sym->magic == SYMBOL_MAGIC);
    *sym_class = sym->sym_class;
    return 0;
}

int get_symbol_type(const Symbol * sym, Symbol ** type) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (sym->sym_class == SYM_CLASS_TYPE && obj == NULL) {
        *type = (Symbol *)sym;
        return 0;
    }
    if (is_constant_pseudo_symbol(sym)) {
        *type = sym->base;
        return 0;
    }
    if (sym->sym_class == SYM_CLASS_FUNCTION) {
        if (sym->assembly_function || obj == NULL) {
            *type = NULL;
        }
        else {
            *type = alloc_symbol();
            (*type)->ctx = sym->ctx;
            (*type)->frame = sym->frame;
            (*type)->sym_class = SYM_CLASS_TYPE;
            (*type)->base = (Symbol *)sym;
        }
        return 0;
    }
    if (unpack(sym) < 0) return -1;
    if (is_modified_type(obj)) {
        obj = obj->mType;
    }
    else {
        obj = get_object_type(obj);
    }
    if (obj == NULL) {
        *type = NULL;
    }
    else if (obj == sym->obj) {
        *type = (Symbol *)sym;
    }
    else {
        elf_object2symbol(sym->ref, find_definition(obj), type);
    }
    return 0;
}

static void get_object_type_class(ObjectInfo * obj, int * type_class) {
    while (obj != NULL) {
        switch (obj->mTag) {
        case TAG_global_subroutine:
        case TAG_inlined_subroutine:
        case TAG_subroutine:
        case TAG_subprogram:
        case TAG_entry_point:
        case TAG_subroutine_type:
            *type_class = TYPE_CLASS_FUNCTION;
            return;
        case TAG_array_type:
        case TAG_string_type:
            *type_class = TYPE_CLASS_ARRAY;
            return;
        case TAG_enumeration_type:
        case TAG_enumerator:
            *type_class = TYPE_CLASS_ENUMERATION;
            return;
        case TAG_pointer_type:
        case TAG_reference_type:
        case TAG_rvalue_reference_type:
        case TAG_mod_pointer:
        case TAG_mod_reference:
            *type_class = TYPE_CLASS_POINTER;
            return;
        case TAG_ptr_to_member_type:
            *type_class = TYPE_CLASS_MEMBER_PTR;
            return;
        case TAG_class_type:
        case TAG_structure_type:
        case TAG_union_type:
        case TAG_interface_type:
            *type_class = TYPE_CLASS_COMPOSITE;
            return;
        case TAG_base_type:
            switch (obj->u.mFundType) {
            case ATE_address:
                *type_class = TYPE_CLASS_POINTER;
                return;
            case ATE_boolean:
                *type_class = TYPE_CLASS_ENUMERATION;
                return;
            case ATE_float:
            case ATE_imaginary_float:
                *type_class = TYPE_CLASS_REAL;
                return;
            case ATE_complex_float:
                *type_class = TYPE_CLASS_COMPLEX;
                return;
            case ATE_signed:
            case ATE_signed_char:
            case ATE_signed_fixed:
                *type_class = TYPE_CLASS_INTEGER;
                return;
            case ATE_unsigned:
            case ATE_unsigned_char:
            case ATE_unsigned_fixed:
            case ATE_UTF:
                *type_class = TYPE_CLASS_CARDINAL;
                return;
            }
            *type_class = TYPE_CLASS_UNKNOWN;
            return;
        case TAG_fund_type:
            switch (obj->u.mFundType) {
            case FT_boolean:
                *type_class = TYPE_CLASS_ENUMERATION;
                return;
            case FT_char:
                *type_class = TYPE_CLASS_INTEGER;
                return;
            case FT_dbl_prec_float:
            case FT_ext_prec_float:
            case FT_float:
                *type_class = TYPE_CLASS_REAL;
                return;
            case FT_signed_char:
            case FT_signed_integer:
            case FT_signed_long:
            case FT_signed_short:
            case FT_short:
            case FT_integer:
            case FT_long:
                *type_class = TYPE_CLASS_INTEGER;
                return;
            case FT_unsigned_char:
            case FT_unsigned_integer:
            case FT_unsigned_long:
            case FT_unsigned_short:
                *type_class = TYPE_CLASS_CARDINAL;
                return;
            case FT_pointer:
                *type_class = TYPE_CLASS_POINTER;
                return;
            case FT_void:
                *type_class = TYPE_CLASS_CARDINAL;
                return;
            case FT_label:
            case FT_complex:
            case FT_dbl_prec_complex:
            case FT_ext_prec_complex:
                break;
            }
            *type_class = TYPE_CLASS_UNKNOWN;
            return;
        case TAG_subrange_type:
        case TAG_packed_type:
        case TAG_volatile_type:
        case TAG_restrict_type:
        case TAG_shared_type:
        case TAG_const_type:
        case TAG_typedef:
        case TAG_formal_parameter:
        case TAG_unspecified_parameters:
        case TAG_global_variable:
        case TAG_local_variable:
        case TAG_variable:
        case TAG_inheritance:
        case TAG_member:
        case TAG_constant:
        case TAG_template_type_param:
            obj = obj->mType;
            break;
        default:
            obj = NULL;
            break;
        }
    }
}

int get_symbol_type_class(const Symbol * sym, int * type_class) {
    assert(sym->magic == SYMBOL_MAGIC);
    if (sym->assembly_function) {
        *type_class = TYPE_CLASS_FUNCTION;
        return 0;
    }
    if (is_constant_pseudo_symbol(sym)) return get_symbol_type_class(sym->base, type_class);
    if (is_array_type_pseudo_symbol(sym)) {
        if (sym->base->sym_class == SYM_CLASS_FUNCTION) *type_class = TYPE_CLASS_FUNCTION;
        else if (sym->length > 0) *type_class = TYPE_CLASS_ARRAY;
        else *type_class = TYPE_CLASS_POINTER;
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym)) {
        *type_class = sym->dimension;
        return 0;
    }
    if (is_pointer_pseudo_symbol(sym)) {
        *type_class = TYPE_CLASS_POINTER;
        return 0;
    }
    if (unpack(sym) < 0) return -1;
    *type_class = TYPE_CLASS_UNKNOWN;
    get_object_type_class(sym->obj, type_class);
    if (*type_class == TYPE_CLASS_UNKNOWN && sym->tbl != NULL) {
        ELF_SymbolInfo info;
        if (sym->dimension != 0) {
            *type_class = TYPE_CLASS_FUNCTION;
            return 0;
        }
        unpack_elf_symbol_info(sym->tbl, sym->index, &info);
        if (info.type == STT_FUNC || info.type == STT_GNU_IFUNC) {
            *type_class = TYPE_CLASS_FUNCTION;
            return 0;
        }
        if (info.type == STT_NOTYPE && elf_symbol_has_address(&info) &&
                info.section != NULL && (info.section->flags & SHF_EXECINSTR) != 0) {
            *type_class = TYPE_CLASS_FUNCTION;
            return 0;
        }
    }
    return 0;
}

int get_symbol_update_policy(const Symbol * sym, char ** id, int * policy) {
    assert(sym->magic == SYMBOL_MAGIC);
    *id = sym->ctx->id;
    *policy = sym->frame != STACK_NO_FRAME ? UPDATE_ON_EXE_STATE_CHANGES : UPDATE_ON_MEMORY_MAP_CHANGES;
    return 0;
}

int get_symbol_name(const Symbol * sym, char ** name) {
    assert(sym->magic == SYMBOL_MAGIC);
    if (is_array_type_pseudo_symbol(sym)) {
        *name = NULL;
    }
    else if (is_std_type_pseudo_symbol(sym)) {
        *name = sym->index > 0 ? (char *)type_pseudo_symbols[sym->index - 1].name : NULL;
    }
    else if (is_constant_pseudo_symbol(sym)) {
        *name = (char *)constant_pseudo_symbols[sym->index].name;
    }
    else if (sym->obj != NULL) {
        *name = (char *)sym->obj->mName;
    }
    else if (sym->tbl != NULL) {
        ELF_SymbolInfo sym_info;
        if (sym->dimension == 0) {
            unpack_elf_symbol_info(sym->tbl, sym->index, &sym_info);
            if (sym_info.name != NULL) {
                size_t i;
                for (i = 0;; i++) {
                    if (sym_info.name[i] == 0) {
                        *name = sym_info.name;
                        break;
                    }
                    if (sym_info.name[i] == '@' && sym_info.name[i + 1] == '@') {
                        *name = (char *)tmp_alloc_zero(i + 1);
                        memcpy(*name, sym_info.name, i);
                        break;
                    }
                }
            }
            else {
                *name = NULL;
            }
        }
        else {
            ContextAddress sym_offs = 0;
            if (elf_find_plt_dynsym(sym->tbl, sym->index, &sym_info, &sym_offs) < 0) return -1;
            if (sym_info.name != NULL) {
                *name = tmp_strdup2(sym_info.name, "@plt");
                if (sym_offs > 0) {
                    char buf[64];
                    snprintf(buf, sizeof(buf), "+0x%x", (unsigned)sym_offs);
                    *name = tmp_strdup2(*name, buf);
                }
            }
            else {
                *name = NULL;
            }
        }
    }
    else {
        *name = NULL;
    }
    return 0;
}

static int err_no_info(void) {
    set_errno(ERR_OTHER, "Debug info not available");
    return -1;
}

static int err_wrong_obj(void) {
    set_errno(ERR_OTHER, "Wrong object kind");
    return -1;
}

int get_symbol_size(const Symbol * sym, ContextAddress * size) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (sym->assembly_function) {
        *size = sym->size;
        return 0;
    }
    if (is_constant_pseudo_symbol(sym)) return get_symbol_size(sym->base, size);
    if (is_array_type_pseudo_symbol(sym)) {
        if (sym->base->sym_class == SYM_CLASS_FUNCTION) {
            set_errno(ERR_OTHER, "Size of function type is not defined");
            return -1;
        }
        if (sym->length > 0) {
            if (get_symbol_size(sym->base, size)) return -1;
            *size *= sym->length;
        }
        else {
            Symbol * base = sym->base;
            while (base->obj == NULL && base->base != NULL) base = base->base;
            if (base->obj != NULL) *size = base->obj->mCompUnit->mDesc.mAddressSize;
            else *size = context_word_size(sym->ctx);
        }
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym)) {
        *size = sym->cardinal;
        return 0;
    }
    if (is_pointer_pseudo_symbol(sym)) {
        *size = sym->size;
        return 0;
    }
    if (unpack(sym) < 0) return -1;
    *size = 0;
    if (obj != NULL) {
        Trap trap;
        int ok = 0;
        U8_T sz = 0;
        U8_T n = 0;

        if (!set_trap(&trap)) return -1;
        ok = get_object_size(sym->ref, obj, sym->dimension, &sz, &n);
        clear_trap(&trap);
        if (!ok && sym->sym_class == SYM_CLASS_REFERENCE && (obj->mFlags & DOIF_location) != 0) {
            if (set_trap(&trap)) {
                PropertyValue v;
                read_and_evaluate_dwarf_object_property(sym_ctx, sym_frame, obj, AT_location, &v);
                if (v.mPieces) {
                    U4_T i = 0, j = 0;
                    while (i < v.mPieceCnt) {
                        LocationPiece * p = v.mPieces + i++;
                        if (p->bit_size) j += p->bit_size;
                        else sz += p->size;
                    }
                    sz += (j + 7) / 8;
                    ok = 1;
                }
                clear_trap(&trap);
            }
        }
        if (!ok && sym->sym_class == SYM_CLASS_REFERENCE) {
            Symbol * elf_sym = NULL;
            ContextAddress elf_sym_size = 0;
            if (map_to_sym_table(obj, &elf_sym) && get_symbol_size(elf_sym, &elf_sym_size) == 0) {
                sz = elf_sym_size;
                ok = 1;
            }
        }
        if (!ok) {
            set_errno(ERR_INV_DWARF, "Object has no size attribute");
            return -1;
        }
        *size = (ContextAddress)sz;
    }
    else if (sym->tbl != NULL) {
        if (sym->dimension == 0) {
            ELF_SymbolInfo info;
            unpack_elf_symbol_info(sym->tbl, sym->index, &info);
            if (IS_PPC64_FUNC_OPD(sym->tbl->file, &info) && info.name != NULL) {
                /*
                 * For PPC64, the size of an ELF symbol is either the size
                 * described by the .<name> symbol or, if this symbol does not
                 * exist, the size of the opd symbol.
                 * So check if the symbol .<name> exists as a global.
                 */
                char * dotname = tmp_strdup2(".", info.name);
                find_symbol_list = NULL;
                find_by_name_in_sym_table(sym->tbl->file, dotname, 1);
                if (find_symbol_list != NULL) return get_symbol_size(find_symbol_list, size);
            }
            switch (info.type) {
            case STT_GNU_IFUNC:
                set_errno(ERR_OTHER, "Size not available: indirect symbol");
                return -1;
            default:
                if (elf_symbol_has_address(&info)) {
                    *size = (ContextAddress)info.size;
                }
                else {
                    *size = info.sym_section->file->elf64 ? 8 : 4;
                }
                break;
            }
        }
        else {
            *size = sym->dimension;
        }
    }
    else {
        return err_no_info();
    }
    return 0;
}

int get_symbol_base_type(const Symbol * sym, Symbol ** base_type) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (is_array_type_pseudo_symbol(sym)) {
        if (sym->base->sym_class == SYM_CLASS_FUNCTION) {
            if (sym->base->obj != NULL && sym->base->obj->mType != NULL) {
                if (unpack(sym->base) < 0) return -1;
                elf_object2symbol(sym->ref, sym->base->obj->mType, base_type);
                return 0;
            }
            return err_no_info();
        }
        if (sym->base->sym_class == SYM_CLASS_REFERENCE) {
            return err_no_info();
        }
        *base_type = sym->base;
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym) || is_constant_pseudo_symbol(sym)) {
        return err_wrong_obj();
    }
    if (unpack(sym) < 0) return -1;
    if (sym->sym_class == SYM_CLASS_FUNCTION) {
        if (sym->obj != NULL && sym->obj->mType != NULL) {
            elf_object2symbol(sym->ref, sym->obj->mType, base_type);
            return 0;
        }
        return err_no_info();
    }
    obj = get_original_type(obj);
    if (obj != NULL) {
        if (obj->mTag == TAG_array_type) {
            int i = sym->dimension;
            ObjectInfo * idx = get_dwarf_children(obj);
            while (i > 0 && idx != NULL) {
                idx = idx->mSibling;
                i--;
            }
            if (idx != NULL && idx->mSibling != NULL) {
                elf_object2symbol(sym->ref, obj, base_type);
                (*base_type)->dimension = sym->dimension + 1;
                return 0;
            }
        }
        obj = obj->mType;
        if (obj != NULL) {
            elf_object2symbol(sym->ref, find_definition(obj), base_type);
            (*base_type)->ref = sym->ref;
            return 0;
        }
        return err_wrong_obj();
    }
    return err_no_info();
}

int get_symbol_index_type(const Symbol * sym, Symbol ** index_type) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (is_array_type_pseudo_symbol(sym)) {
        if (sym->base->sym_class != SYM_CLASS_TYPE) return err_wrong_obj();
        alloc_std_type_pseudo_symbol(sym->ctx, context_word_size(sym->ctx), TYPE_CLASS_CARDINAL, index_type);
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym) ||
            is_constant_pseudo_symbol(sym) ||
            sym->sym_class == SYM_CLASS_FUNCTION) {
        return err_wrong_obj();
    }
    if (unpack(sym) < 0) return -1;
    obj = get_original_type(obj);
    if (obj != NULL) {
        if (obj->mTag == TAG_array_type) {
            int i = sym->dimension;
            ObjectInfo * idx = get_dwarf_children(obj);
            while (i > 0 && idx != NULL) {
                idx = idx->mSibling;
                i--;
            }
            if (idx != NULL) {
                elf_object2symbol(sym->ref, idx, index_type);
                return 0;
            }
        }
        if (obj->mTag == TAG_string_type) {
            alloc_std_type_pseudo_symbol(sym->ctx, obj->mCompUnit->mDesc.mAddressSize, TYPE_CLASS_CARDINAL, index_type);
            return 0;
        }
        return err_wrong_obj();
    }
    return err_no_info();
}

int get_symbol_container(const Symbol * sym, Symbol ** container) {
    ObjectInfo * obj = sym->obj;
    while (is_array_type_pseudo_symbol(sym)) {
        sym = sym->base;
        obj = sym->obj;
    }
    if (obj != NULL) {
        ObjectInfo * parent = NULL;
        if (unpack(sym) < 0) return -1;
        if (sym->sym_class == SYM_CLASS_TYPE) {
            ObjectInfo * org = get_original_type(obj);
            if (org->mTag == TAG_ptr_to_member_type) {
                ObjectInfo * type = get_object_ref_prop(org, AT_containing_type);
                if (type != NULL) {
                    elf_object2symbol(NULL, type, container);
                    return 0;
                }
                set_errno(ERR_INV_DWARF, "Invalid AT_containing_type attribute");
                return -1;
            }
        }
        if (get_object_scope(obj, &parent) < 0) return -1;
        if (parent != NULL) {
            elf_object2symbol(NULL, parent, container);
            return 0;
        }
        if (obj->mTag == TAG_compile_unit) {
            *container = NULL;
            return 0;
        }
        return err_wrong_obj();
    }
    return err_no_info();
}

int get_symbol_length(const Symbol * sym, ContextAddress * length) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (is_array_type_pseudo_symbol(sym)) {
        if (sym->base->sym_class != SYM_CLASS_TYPE) return err_wrong_obj();
        *length = sym->length == 0 ? 1 : sym->length;
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym) ||
            is_constant_pseudo_symbol(sym) ||
            sym->sym_class == SYM_CLASS_FUNCTION) {
        return err_wrong_obj();
    }
    if (unpack(sym) < 0) return -1;
    obj = get_original_type(obj);
    if (obj != NULL) {
        if (obj->mTag == TAG_array_type) {
            int i = sym->dimension;
            ObjectInfo * idx = get_dwarf_children(obj);
            while (i > 0 && idx != NULL) {
                idx = idx->mSibling;
                i--;
            }
            if (idx != NULL) {
                Trap trap;
                if (!set_trap(&trap)) return -1;
                *length = (ContextAddress)get_array_index_length(sym->ref, idx);
                clear_trap(&trap);
                return 0;
            }
        }
        if (obj->mTag == TAG_string_type) {
            Trap trap;
            if (!set_trap(&trap)) return -1;
            *length = (ContextAddress)read_string_length(obj);
            clear_trap(&trap);
            return 0;
        }
        return err_wrong_obj();
    }
    return err_no_info();
}

int get_symbol_lower_bound(const Symbol * sym, int64_t * value) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (is_array_type_pseudo_symbol(sym)) {
        if (sym->base->sym_class != SYM_CLASS_TYPE) return err_wrong_obj();
        *value = 0;
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym) ||
            is_constant_pseudo_symbol(sym) ||
            sym->sym_class == SYM_CLASS_FUNCTION) {
        return err_wrong_obj();
    }
    if (unpack(sym) < 0) return -1;
    obj = get_original_type(obj);
    if (obj != NULL) {
        if (obj->mTag == TAG_array_type) {
            int i = sym->dimension;
            ObjectInfo * idx = get_dwarf_children(obj);
            while (i > 0 && idx != NULL) {
                idx = idx->mSibling;
                i--;
            }
            if (idx != NULL) {
                if (get_variable_num_prop(sym->ref, idx, AT_lower_bound, (U8_T *)value)) return 0;
                if (get_error_code(errno) != ERR_SYM_NOT_FOUND) return -1;
                *value = get_default_lower_bound(obj);
                return 0;
            }
        }
        if (obj->mTag == TAG_string_type) {
            *value = 0;
            return 0;
        }
        return err_wrong_obj();
    }
    return err_no_info();
}

static Symbol ** boolean_children(ObjectInfo * ref, ObjectInfo * type_obj) {
    unsigned i = 0;
    unsigned n = 0;
    Symbol * type_sym = NULL;
    Symbol ** buf = (Symbol **)tmp_alloc(sizeof(Symbol *) * 2);
    elf_object2symbol(ref, type_obj, &type_sym);
    while (constant_pseudo_symbols[i].name != NULL) {
        if (n < 2 && strcmp(constant_pseudo_symbols[i].type, "bool") == 0) {
            Symbol * sym = alloc_symbol();
            sym->ctx = sym_ctx;
            sym->frame = STACK_NO_FRAME;
            sym->sym_class = SYM_CLASS_VALUE;
            sym->base = type_sym;
            sym->index = i;
            assert(is_constant_pseudo_symbol(sym));
            buf[n++] = sym;
        }
        i++;
    }
    assert(n == 2);
    return buf;
}

int get_symbol_children(const Symbol * sym, Symbol *** children, int * count) {
    ObjectInfo * obj = sym->obj;
    assert(sym->magic == SYMBOL_MAGIC);
    if (is_array_type_pseudo_symbol(sym)) {
        obj = sym->base->obj;
        if (sym->base->sym_class == SYM_CLASS_FUNCTION) {
            if (obj == NULL) {
                *children = NULL;
                *count = 0;
                errno = ERR_SYM_NOT_FOUND;
                return -1;
            }
            else {
                int n = 0;
                int buf_len = 0;
                Symbol ** buf = NULL;
                ObjectInfo * i = get_dwarf_children(obj);
                if (unpack(sym->base) < 0) return -1;
                while (i != NULL) {
                    if (i->mTag == TAG_formal_parameter || i->mTag == TAG_unspecified_parameters) {
                        Symbol * x = NULL;
                        Symbol * y = NULL;
                        elf_object2symbol(sym->ref, i, &x);
                        if (get_symbol_type(x, &y) < 0) return -1;
                        if (y == NULL && i->mTag == TAG_unspecified_parameters) {
                            y = alloc_symbol();
                            y->ctx = sym->ctx;
                            y->frame = STACK_NO_FRAME;
                            y->sym_class = SYM_CLASS_TYPE;
                            y->base = (Symbol *)x;
                        }
                        if (y == NULL) {
                            set_errno(ERR_INV_DWARF, "Invalid function arguments info");
                            return -1;
                        }
                        if (buf_len <= n) {
                            buf_len += 16;
                            buf = (Symbol **)tmp_realloc(buf, sizeof(Symbol *) * buf_len);
                        }
                        buf[n++] = y;
                    }
                    i = i->mSibling;
                }
                *children = buf;
                *count = n;
                return 0;
            }
        }
        *children = NULL;
        *count = 0;
        return 0;
    }
    if (is_std_type_pseudo_symbol(sym) || is_constant_pseudo_symbol(sym)) {
        *children = NULL;
        *count = 0;
        return 0;
    }
    if (unpack(sym) < 0) return -1;
    if (obj != NULL && obj->mTag != TAG_variant_part && obj->mTag != TAG_variant && obj->mTag != TAG_compile_unit) {
        obj = get_original_type(obj);
    }
    if (obj != NULL) {
        int n = 0;
        Symbol ** buf = NULL;
        if (obj->mTag == TAG_base_type) {
            if (obj->u.mFundType == ATE_boolean) {
                buf = boolean_children(sym->ref, obj);
                n = 2;
            }
        }
        else if (obj->mTag == TAG_fund_type) {
            if (obj->u.mFundType == FT_boolean) {
                buf = boolean_children(sym->ref, obj);
                n = 2;
            }
        }
        else {
            int buf_len = 0;
            ObjectInfo * i = get_dwarf_children(find_definition(obj));
            while (i != NULL) {
                Symbol * x = NULL;
                elf_object2symbol(sym->ref, find_definition(i), &x);
                if (buf_len <= n) {
                    buf_len += 16;
                    buf = (Symbol **)tmp_realloc(buf, sizeof(Symbol *) * buf_len);
                }
                buf[n++] = x;
                i = i->mSibling;
            }
        }
        *children = buf;
        *count = n;
        return 0;
    }
    *children = NULL;
    *count = 0;
    return 0;
}

static LocationExpressionCommand * add_location_command(LocationInfo * l, int op) {
    LocationCommands * cmds = &l->value_cmds;
    LocationExpressionCommand * cmd = NULL;
    if (cmds->cnt >= cmds->max) {
        cmds->max += 4;
        cmds->cmds = (LocationExpressionCommand *)tmp_realloc(cmds->cmds,
            sizeof(LocationExpressionCommand) * cmds->max);
    }
    cmd = cmds->cmds + cmds->cnt++;
    memset(cmd, 0, sizeof(LocationExpressionCommand));
    cmd->cmd = op;
    return cmd;
}

static void add_dwarf_location_command(LocationInfo * l, PropertyValue * v) {
    DWARFExpressionInfo * info = NULL;
    LocationExpressionCommand * cmd = NULL;

    dwarf_get_expression_list(v, &info);
    dwarf_transform_expression(sym_ctx, v->mFrame, info);
    if (l->code_size == 0) {
        l->code_addr = info->code_addr;
        l->code_size = info->code_size;
    }
    else if (info->code_size > 0) {
        if (l->code_addr < info->code_addr) {
            assert(l->code_addr + l->code_size > info->code_addr);
            l->code_size = l->code_addr + l->code_size - info->code_addr;
            l->code_addr = info->code_addr;
        }
        if (l->code_addr + l->code_size > info->code_addr + info->code_size) {
            assert(l->code_addr < info->code_addr + info->code_size);
            l->code_size = info->code_addr + info->code_size - l->code_addr;
        }
    }
    if (info->expr_size >= 2 && info->expr_addr[0] == OP_plus_uconst) {
        unsigned pos = 1;
        int64_t offs = 0;
        unsigned i;
        for (i = 0;; i += 7) {
            U1_T n = info->expr_addr[pos++];
            offs |= (n & 0x7f) << i;
            if ((n & 0x80) == 0) break;
            if (pos >= info->expr_size) exception(ERR_INV_DWARF);
        }
        if (pos == info->expr_size) {
            add_location_command(l, SFT_CMD_NUMBER)->args.num = offs;
            add_location_command(l, SFT_CMD_ADD);
            return;
        }
    }
    /* Only create the command if no exception was thrown */
    cmd = add_location_command(l, SFT_CMD_LOCATION);
    cmd->args.loc.code_addr = info->expr_addr;
    cmd->args.loc.code_size = info->expr_size;
    cmd->args.loc.reg_id_scope = v->mObject->mCompUnit->mRegIdScope;
    cmd->args.loc.addr_size = v->mObject->mCompUnit->mDesc.mAddressSize;
    cmd->args.loc.func = evaluate_vm_expression;
}

static void add_member_location_command(LocationInfo * info, ObjectInfo * obj) {
    U8_T bit_size = 0;
    U8_T bit_offs = 0;
    PropertyValue v;
    read_dwarf_object_property(sym_ctx, sym_frame, obj, AT_data_member_location, &v);
    switch (v.mForm) {
    case FORM_DATA1     :
    case FORM_DATA2     :
    case FORM_DATA4     :
    case FORM_DATA8     :
    case FORM_SDATA     :
    case FORM_UDATA     :
        add_location_command(info, SFT_CMD_ARG)->args.arg_no = 0;
        add_location_command(info, SFT_CMD_NUMBER)->args.num = get_numeric_property_value(&v);
        add_location_command(info, SFT_CMD_ADD);
        break;
    case FORM_BLOCK1    :
    case FORM_BLOCK2    :
    case FORM_BLOCK4    :
    case FORM_BLOCK     :
    case FORM_EXPRLOC   :
    case FORM_SEC_OFFSET:
        add_location_command(info, SFT_CMD_ARG)->args.arg_no = 0;
        add_dwarf_location_command(info, &v);
        break;
    default:
        str_fmt_exception(ERR_OTHER, "Invalid AT_data_member_location form 0x%04x", v.mForm);
        break;
    }
    if (get_num_prop(obj, AT_bit_size, &bit_size)) {
        LocationExpressionCommand * cmd = add_location_command(info, SFT_CMD_PIECE);
        cmd->args.piece.bit_size = (unsigned)bit_size;
        if (get_num_prop(obj, AT_bit_offset, &bit_offs)) {
            if (obj->mCompUnit->mFile->big_endian) {
                cmd->args.piece.bit_offs = (unsigned)bit_offs;
            }
            else {
                U8_T byte_size = 0;
                U8_T type_byte_size = 0;
                U8_T type_bit_size = 0;
                if (get_num_prop(obj, AT_byte_size, &byte_size)) {
                    cmd->args.piece.bit_offs = (unsigned)(byte_size * 8 - bit_offs - bit_size);
                }
                else if (obj->mType != NULL && get_object_size(obj, obj->mType, 0, &type_byte_size, &type_bit_size)) {
                    cmd->args.piece.bit_offs = (unsigned)(type_byte_size * 8 - bit_offs - bit_size);
                }
                else {
                    str_exception(ERR_INV_DWARF, "Unknown field size");
                }
            }
        }
    }
}

static int add_member_location(LocationInfo * info, ObjectInfo * type, ObjectInfo * member) {
    ObjectInfo * obj = NULL;
    if (member->mParent == type) {
        add_member_location_command(info, member);
        return 1;
    }
    obj = get_dwarf_children(type);
    while (obj != NULL) {
        if (obj->mTag == TAG_inheritance) {
            unsigned cnt = info->value_cmds.cnt;
            add_member_location_command(info, obj);
            add_location_command(info, SFT_CMD_SET_ARG)->args.arg_no = 0;
            if (add_member_location(info, obj->mType, member)) return 1;
            info->value_cmds.cnt = cnt;
        }
        obj = obj->mSibling;
    }
    return 0;
}

static int read_discriminant_values(ObjectInfo * obj, LocationInfo * info) {
    Trap trap;
    U8_T value = 0;

    if (get_num_prop(obj, AT_discr_value, &value)) {
        info->discr_cnt = 1;
        info->discr_lst = (DiscriminantRange *)tmp_alloc_zero(sizeof(DiscriminantRange));
        info->discr_lst->x = value;
        info->discr_lst->y = value;
    }
    else if (errno != ERR_SYM_NOT_FOUND) {
        set_errno(errno, "Cannot read discriminant value");
        return -1;
    }
    else if (set_trap(&trap)) {
        ObjectInfo * type;
        int type_class = TYPE_CLASS_UNKNOWN;
        int discr_signed = 0;
        PropertyValue v;
        unsigned discr_max = 4;
        U8_T end;

        /*
         * We need to check the type of the discriminant to know if the
         * values has to be read as signed or unsigned LEB128. We consider
         * that the parent of this object contains the info to the
         * discriminant.
         */
        assert(obj->mParent != NULL);
        type = get_original_type(obj->mParent);
        get_object_type_class(type, &type_class);
        discr_signed = type_class == TYPE_CLASS_INTEGER;

        read_dwarf_object_property(sym_ctx, sym_frame, obj, AT_discr_list, &v);
        dio_EnterSection(&obj->mCompUnit->mDesc, obj->mCompUnit->mDesc.mSection,
                v.mAddr - (U1_T *)obj->mCompUnit->mDesc.mSection->data);
        info->discr_lst = (DiscriminantRange *)tmp_alloc(sizeof(DiscriminantRange) * discr_max);
        end = dio_GetPos() + v.mSize;
        while (dio_GetPos() < end) {
            U1_T code = dio_ReadU1();
            if (info->discr_cnt >= discr_max) {
                discr_max *= 2;
                info->discr_lst = (DiscriminantRange *)tmp_realloc(info->discr_lst, sizeof(DiscriminantRange) * discr_max);
            }
            if (code == DSC_label) {
                value = discr_signed ? dio_ReadS8LEB128() : (int64_t)dio_ReadU8LEB128();
                info->discr_lst[info->discr_cnt].x = value;
                info->discr_lst[info->discr_cnt].y = value;
                info->discr_cnt++;
            }
            else if (code == DSC_range) {
                info->discr_lst[info->discr_cnt].x = discr_signed ? dio_ReadS8LEB128() : (int64_t)dio_ReadU8LEB128();
                info->discr_lst[info->discr_cnt].y = discr_signed ? dio_ReadS8LEB128() : (int64_t)dio_ReadU8LEB128();
                info->discr_cnt++;
            }
            else {
                str_exception(ERR_UNSUPPORTED, "Unsupported discriminant type");
            }
        }
        dio_ExitSection();
        clear_trap(&trap);
    }
    else if (errno != ERR_SYM_NOT_FOUND) {
        set_errno(errno, "Cannot read discriminant list");
        return -1;
    }

    return 0;
}

int get_location_info(const Symbol * sym, LocationInfo ** res) {
    ObjectInfo * obj = sym->obj;
    LocationInfo * info = *res = (LocationInfo *)tmp_alloc_zero(sizeof(LocationInfo));

    assert(sym->magic == SYMBOL_MAGIC);

    if (sym->has_address && sym->sym_class != SYM_CLASS_VALUE) {
        info->big_endian = big_endian_host();
        add_location_command(info, SFT_CMD_NUMBER)->args.num = sym->address;
        return 0;
    }

    if (is_constant_pseudo_symbol(sym)) {
        void * value = NULL;
        ContextAddress size = 0;
        LocationExpressionCommand * cmd = add_location_command(info, SFT_CMD_PIECE);

        if (get_symbol_size(sym->base, &size) < 0) return -1;
        info->big_endian = big_endian_host();
        cmd->args.piece.bit_size = (unsigned)(size * 8);
        cmd->args.piece.value = tmp_alloc((size_t)size);
        value = &constant_pseudo_symbols[sym->index].value;
        if (big_endian_host() && size < sizeof(ConstantValueType)) {
            value = (uint8_t *)value + (sizeof(ConstantValueType) - size);
        }
        memcpy(cmd->args.piece.value, value, (size_t)size);
        return 0;
    }

    if (is_array_type_pseudo_symbol(sym) ||
            is_std_type_pseudo_symbol(sym)) {
        return err_wrong_obj();
    }

    if (is_pointer_pseudo_symbol(sym)) {
        void const * value = NULL;
        LocationExpressionCommand * cmd = add_location_command(info, SFT_CMD_PIECE);

        info->big_endian = big_endian_host();
        cmd->args.piece.bit_size = (unsigned)(sym->size * 8);
        cmd->args.piece.value = tmp_alloc((size_t)sym->size);
        value = &sym->address;
        if (big_endian_host() && sym->size < sizeof(ContextAddress)) {
            value = (uint8_t *)value + (sizeof(ContextAddress) - sym->size);
        }
        memcpy(cmd->args.piece.value, value, (size_t)sym->size);
        return 0;
    }

    if (unpack(sym) < 0) return -1;

    if (obj != NULL) {
        Trap trap;
        PropertyValue v;
        ObjectInfo * org_type;

        if (obj->mTag == TAG_variant) return read_discriminant_values(obj, info);

        if (obj->mTag == TAG_variant_part) {
            ObjectInfo * discr = get_object_ref_prop(obj, AT_discr);
            if (discr == NULL) {
                set_errno(ERR_OTHER, "Discriminant value not available");
                return -1;
            }
            obj = discr;
        }

        obj = find_definition(obj);
        org_type = obj;
        while (org_type != NULL && org_type->mType != NULL && is_modified_type(org_type)) org_type = org_type->mType;
        info->big_endian = obj->mCompUnit->mFile->big_endian;
        if ((obj->mFlags & DOIF_external) == 0 && sym->var != NULL) {
            /* The symbol represents a member of a class instance */
            LocationExpressionCommand * cmd = NULL;
            ObjectInfo * type = get_original_type(sym->var);
            if (!set_trap(&trap)) {
                if (errno == ERR_SYM_NOT_FOUND) set_errno(ERR_OTHER, "Location attribute not found");
                set_errno(errno, "Cannot evaluate location of 'this' pointer");
                return -1;
            }
            if ((type->mTag != TAG_pointer_type && type->mTag != TAG_mod_pointer) || type->mType == NULL) exception(ERR_INV_CONTEXT);
            read_dwarf_object_property(sym_ctx, sym_frame, sym->var, AT_location, &v);
            add_dwarf_location_command(info, &v);
            cmd = add_location_command(info, SFT_CMD_LOAD);
            cmd->args.mem.size = obj->mCompUnit->mDesc.mAddressSize;
            cmd->args.mem.big_endian = obj->mCompUnit->mFile->big_endian;
            add_location_command(info, SFT_CMD_SET_ARG)->args.arg_no = 0;
            type = get_original_type(type->mType);
            if (!add_member_location(info, type, obj)) exception(ERR_INV_CONTEXT);
            clear_trap(&trap);
            return 0;
        }
        if (org_type->mTag == TAG_ptr_to_member_type) {
            add_location_command(info, SFT_CMD_ARG)->args.arg_no = 1;
            add_location_command(info, SFT_CMD_ARG)->args.arg_no = 0;
            info->args_cnt = 2;
            if (set_trap(&trap)) {
                read_dwarf_object_property(sym_ctx, sym_frame, org_type, AT_use_location, &v);
                add_dwarf_location_command(info, &v);
                clear_trap(&trap);
                return 0;
            }
            else if (errno != ERR_SYM_NOT_FOUND) {
                set_errno(errno, "Cannot read member location expression");
                return -1;
            }
            add_location_command(info, SFT_CMD_ADD);
            return 0;
        }
        if (obj->mTag != TAG_inlined_subroutine && (obj->mFlags & DOIF_const_value) != 0) {
            U8_T val_size = 0;
            U8_T bit_size = 0;
            LocationExpressionCommand * cmd = NULL;
            if (!set_trap(&trap)) return -1;
            read_dwarf_object_property(sym_ctx, sym_frame, obj, AT_const_value, &v);
            assert(v.mObject == obj);
            assert(v.mPieces == NULL);
            assert(v.mForm != FORM_EXPRLOC);
            if (!get_object_size(sym->ref, obj, 0, &val_size, &bit_size)) {
                str_exception(ERR_INV_DWARF, "Unknown object size");
            }
            if (v.mAddr != NULL && v.mSize == val_size) {
                assert(v.mBigEndian == info->big_endian);
                cmd = add_location_command(info, SFT_CMD_PIECE);
                cmd->args.piece.bit_size = v.mSize * 8;
                cmd->args.piece.value = v.mAddr;
            }
            else {
                U1_T * bf = (U1_T *)tmp_alloc_zero((size_t)val_size);
                U1_T * p = v.mAddr;
                size_t p_size = v.mSize;
                int has_endianess = 0;
                if (p == NULL) {
                    assert(v.mForm != FORM_EXPR_VALUE);
                    p_size = sizeof(v.mValue);
                    p = (U1_T *)tmp_alloc(p_size);
                    memcpy(p, &v.mValue, p_size);
                    if (big_endian_host()) swap_bytes(p, p_size);
                    has_endianess = 1;
                }
                else {
                    int type_class = 0;
                    get_object_type_class(obj, &type_class);
                    switch (type_class) {
                    case TYPE_CLASS_CARDINAL:
                    case TYPE_CLASS_INTEGER:
                    case TYPE_CLASS_ENUMERATION:
                        has_endianess = 1;
                        break;
                    }
                    if (has_endianess && v.mBigEndian) {
                        p = (U1_T *)tmp_alloc(p_size);
                        memcpy(p, v.mAddr, p_size);
                        swap_bytes(p, p_size);
                    }
                }
                if (val_size > p_size) {
                    memcpy(bf, p, p_size);
                }
                else {
                    memcpy(bf, p, (size_t)val_size);
                }
                if (bit_size % 8 != 0) bf[bit_size / 8] &= (1 << (bit_size % 8)) - 1;
                if (has_endianess && info->big_endian) swap_bytes(bf, (size_t)val_size);
                cmd = add_location_command(info, SFT_CMD_PIECE);
                cmd->args.piece.bit_size = (unsigned)(bit_size ? bit_size : val_size * 8);
                cmd->args.piece.value = bf;
            }
            clear_trap(&trap);
            return 0;
        }
        if (obj->mTag == TAG_member || obj->mTag == TAG_inheritance) {
            if (set_trap(&trap)) {
                add_member_location_command(info, obj);
                info->args_cnt = 1;
                clear_trap(&trap);
                return 0;
            }
            else {
                if (errno != ERR_SYM_NOT_FOUND) set_errno(errno, "Cannot read member location expression");
                else set_errno(ERR_OTHER, "Member location info not available");
                return -1;
            }
        }
        if (obj->mTag == TAG_label && (obj->mFlags & DOIF_ranges) == 0 && (obj->mFlags & DOIF_low_pc) != 0) {
            ContextAddress addr = elf_map_to_run_time_address(sym_ctx, obj->mCompUnit->mFile, obj->u.mCode.mSection, obj->u.mCode.mLowPC);
            if (errno) return -1;
            add_location_command(info, SFT_CMD_NUMBER)->args.num = addr;
            return 0;
        }
        if (obj->mTag == TAG_variable && (obj->mFlags & DOIF_external) != 0 && obj->mCompUnit->mFile->type == ET_DYN) {
            /* Check if symbol is victim of "copy relocation" */
            const char * name = get_linkage_name(obj);
            if (name != NULL) {
                unsigned h = calc_symbol_name_hash(name);
                ELF_File * file = elf_list_first(sym_ctx, 0, ~(ContextAddress)0);
                if (file == NULL && errno) return -1;
                while (file != NULL) {
                    unsigned m = 0;
                    for (m = 1; m < file->section_cnt; m++) {
                        unsigned n;
                        ELF_Section * tbl = file->sections + m;
                        if (tbl->sym_names_hash == NULL) continue;
                        n = tbl->sym_names_hash[h % tbl->sym_names_hash_size];
                        while (n) {
                            ELF_SymbolInfo sym_info;
                            unpack_elf_symbol_info(tbl, n, &sym_info);
                            n = tbl->sym_names_next[n];
                            if (sym_info.bind != STB_GLOBAL || sym_info.type != STT_OBJECT) continue;
                            if (sym_info.section == NULL || sym_info.section->name == NULL) continue;
                            if (strcmp(sym_info.section->name, ".bss") != 0) continue;
                            if (!equ_symbol_names(name, sym_info.name)) continue;
                            if (elf_symbol_has_address(&sym_info)) {
                                ContextAddress address = 0;
                                if (elf_symbol_address(sym_ctx, &sym_info, &address)) return -1;
                                add_location_command(info, SFT_CMD_NUMBER)->args.num = address;
                                elf_list_done(sym_ctx);
                                return 0;
                            }
                        }
                    }
                    file = elf_list_next(sym_ctx);
                    if (file == NULL && errno) return -1;
                }
                elf_list_done(sym_ctx);
            }
        }
#if 0
#if SERVICE_StackTrace || ENABLE_ContextProxy
        if (obj->mTag == TAG_formal_parameter) {
            /* Search call site info */
            if (set_trap(&trap)) {
                RegisterDefinition * reg_def = get_PC_definition(sym_ctx);
                if (reg_def != NULL) {
                    uint64_t addr = 0;
                    ContextAddress rt_addr = 0;
                    UnitAddressRange * range = NULL;
                    Symbol * caller = NULL;
                    StackFrame * info = NULL;
                    save_sym_context();
                    int frame = get_prev_frame(sym_ctx, sym_frame);
                    restore_sym_context();
                    if (get_frame_info(sym_ctx, frame, &info) < 0) exception(errno);
                    if (read_reg_value(info, reg_def, &addr) < 0) exception(errno);
                    range = elf_find_unit(sym_ctx, addr, addr, &rt_addr);
                    if (range != NULL) find_by_addr_in_unit(range->mUnit->mObject,
                        0, rt_addr - range->mAddr, addr, &caller);
                    if (caller != NULL && caller->obj != NULL) {
                        ObjectInfo * l = get_dwarf_children(caller->obj);
                        while (l != NULL) {
                            U8_T call_addr = 0;
                            if (l->mTag == TAG_GNU_call_site && get_num_prop(l, AT_low_pc, &call_addr)) {
                                call_addr += rt_addr - range->mAddr;
                                if (call_addr == addr) {
                                    /*
                                    clear_trap(&trap);
                                    return 0;
                                    */
                                }
                            }
                            l = l->mSibling;
                        }
                    }
                }
                exception(ERR_SYM_NOT_FOUND);
            }
        }
#endif
#endif
        if (obj->mTag != TAG_dwarf_procedure) {
            U8_T addr = 0;
            Symbol * s = NULL;
            if (obj->mFlags & DOIF_location) {
                if (!set_trap(&trap)) return -1;
                read_dwarf_object_property(sym_ctx, sym_frame, obj, AT_location, &v);
                add_dwarf_location_command(info, &v);
                clear_trap(&trap);
                return 0;
            }
            if (obj->mFlags & DOIF_data_location) {
                if (!set_trap(&trap)) return -1;
                read_dwarf_object_property(sym_ctx, sym_frame, obj, AT_data_location, &v);
                add_dwarf_location_command(info, &v);
                clear_trap(&trap);
                return 0;
            }
            switch (sym->sym_class) {
            case SYM_CLASS_FUNCTION:
            case SYM_CLASS_COMP_UNIT:
            case SYM_CLASS_BLOCK:
                if (get_num_prop(obj, AT_entry_pc, &addr)) {
                    add_location_command(info, SFT_CMD_NUMBER)->args.num = addr;
                    return 0;
                }
                else if (get_error_code(errno) != ERR_SYM_NOT_FOUND &&
                            get_error_code(errno) != ERR_INV_ADDRESS) {
                    return -1;
                }
                if (!(obj->mFlags & DOIF_ranges) && (obj->mFlags & DOIF_low_pc)) {
                    addr = elf_map_to_run_time_address(sym_ctx, obj->mCompUnit->mFile, obj->u.mCode.mSection, obj->u.mCode.mLowPC);
                    if (errno) return -1;
                    add_location_command(info, SFT_CMD_NUMBER)->args.num = addr;
                    return 0;
                }
                break;
            }
            if (map_to_sym_table(obj, &s)) return get_location_info(s, res);
        }
        set_errno(ERR_OTHER, "No object location info found in DWARF data");
        return -1;
    }

    if (sym->tbl != NULL) {
        LocationExpressionCommand * cmd = NULL;
        ELF_SymbolInfo elf_sym_info;
        ContextAddress address = 0;
        info->big_endian = sym->tbl->file->big_endian;
        if (sym->dimension != 0) {
            /* @plt symbol */
            address = sym->tbl->addr + sym->cardinal + sym->index * sym->dimension;
            address = elf_map_to_run_time_address(sym_ctx, sym->tbl->file, sym->tbl, address);
            if (errno) return -1;
            add_location_command(info, SFT_CMD_NUMBER)->args.num = address;
            return 0;
        }
        unpack_elf_symbol_info(sym->tbl, sym->index, &elf_sym_info);
        if (elf_sym_info.type == STT_GNU_IFUNC && elf_sym_info.name != NULL) {
            int error = 0;
            int found = 0;
            ELF_File * file = elf_list_first(sym_ctx, 0, ~(ContextAddress)0);
            if (file == NULL) error = errno;
            while (error == 0 && file != NULL) {
                ContextAddress got_addr = 0;
                if (elf_find_got_entry(file, elf_sym_info.name, &got_addr) < 0) {
                    error = errno;
                }
                else if (got_addr != 0) {
                    got_addr = elf_map_to_run_time_address(sym_ctx, file, NULL, got_addr);
                    if (got_addr != 0 && elf_read_memory_word(sym_ctx, file, got_addr, &address) == 0) {
                        found = 1;
                        break;
                    }
                }
                file = elf_list_next(sym_ctx);
                if (file == NULL) error = errno;
            }
            elf_list_done(sym_ctx);
            if (found) {
                add_location_command(info, SFT_CMD_NUMBER)->args.num = address;
                return 0;
            }
            if (error) {
                errno = error;
                return -1;
            }
        }
        if (elf_sym_info.type == STT_TLS) {
            Trap trap;
            if (!set_trap(&trap)) return -1;
            address = get_tls_address(sym->ctx, elf_sym_info.sym_section->file);
            clear_trap(&trap);
            cmd = add_location_command(info, SFT_CMD_NUMBER);
            cmd->args.num = address + elf_sym_info.value;
            return 0;
        }
        if (elf_symbol_has_address(&elf_sym_info)) {
            if (elf_symbol_address(sym_ctx, &elf_sym_info, &address)) return -1;
            add_location_command(info, SFT_CMD_NUMBER)->args.num = address;
            return 0;
        }
        info->big_endian = big_endian_host();
        cmd = add_location_command(info, SFT_CMD_PIECE);
        if (elf_sym_info.sym_section->file->elf64) {
            U8_T * buf = (U8_T *)tmp_alloc(8);
            *buf = elf_sym_info.value;
            cmd->args.piece.bit_size = 64;
            cmd->args.piece.value = buf;
        }
        else {
            U4_T * buf = (U4_T *)tmp_alloc(4);
            *buf = (U4_T)elf_sym_info.value;
            cmd->args.piece.bit_size = 32;
            cmd->args.piece.value = buf;
        }
        return 0;
    }

    set_errno(ERR_OTHER, "Object does not have location information");
    return -1;
}

int get_symbol_flags(const Symbol * sym, SYM_FLAGS * flags) {
    U8_T v = 0;
    ObjectInfo * obj = sym->obj;
    *flags = 0;
    assert(sym->magic == SYMBOL_MAGIC);
    if (sym->base || is_std_type_pseudo_symbol(sym)) {
        if (is_array_type_pseudo_symbol(sym) && sym->base->sym_class == SYM_CLASS_REFERENCE) {
            *flags |= SYM_FLAG_VARARG;
        }
        return 0;
    }
    if (unpack(sym) < 0) return -1;
    if (sym->tbl != NULL && sym->dimension == 0) {
        Trap trap;
        ELF_SymbolInfo info;
        if (!set_trap(&trap)) return -1;
        unpack_elf_symbol_info(sym->tbl, sym->index, &info);
        if (info.bind == STB_GLOBAL || info.bind == STB_WEAK) *flags |= SYM_FLAG_EXTERNAL;
        clear_trap(&trap);
    }
    if (obj != NULL) {
        if (obj->mFlags & DOIF_external) *flags |= SYM_FLAG_EXTERNAL;
        if (obj->mFlags & DOIF_artificial) *flags |= SYM_FLAG_ARTIFICIAL;
        if (obj->mFlags & DOIF_private) *flags |= SYM_FLAG_PRIVATE;
        if (obj->mFlags & DOIF_protected) *flags |= SYM_FLAG_PROTECTED;
        if (obj->mFlags & DOIF_public) *flags |= SYM_FLAG_PUBLIC;
        if (obj->mFlags & DOIF_optional) *flags |= SYM_FLAG_OPTIONAL;
        if (obj->mFlags & DOIF_data_location) *flags |= SYM_FLAG_INDIRECT;
        switch (obj->mTag) {
        case TAG_subrange_type:
            *flags |= SYM_FLAG_SUBRANGE_TYPE;
            break;
        case TAG_packed_type:
            *flags |= SYM_FLAG_PACKET_TYPE;
            break;
        case TAG_const_type:
            *flags |= SYM_FLAG_CONST_TYPE;
            break;
        case TAG_volatile_type:
            *flags |= SYM_FLAG_VOLATILE_TYPE;
            break;
        case TAG_restrict_type:
            *flags |= SYM_FLAG_RESTRICT_TYPE;
            break;
        case TAG_shared_type:
            *flags |= SYM_FLAG_SHARED_TYPE;
            break;
        case TAG_typedef:
            *flags |= SYM_FLAG_TYPEDEF;
            break;
        case TAG_template_type_param:
            *flags |= SYM_FLAG_TYPE_PARAMETER;
            break;
        case TAG_reference_type:
        case TAG_mod_reference:
            *flags |= SYM_FLAG_REFERENCE;
            break;
        case TAG_rvalue_reference_type:
            *flags |= SYM_FLAG_REFERENCE;
            *flags |= SYM_FLAG_RVALUE;
            break;
        case TAG_union_type:
            *flags |= SYM_FLAG_UNION_TYPE;
            break;
        case TAG_class_type:
            *flags |= SYM_FLAG_CLASS_TYPE;
            break;
        case TAG_structure_type:
            *flags |= SYM_FLAG_STRUCT_TYPE;
            break;
        case TAG_string_type:
            *flags |= SYM_FLAG_STRING_TYPE;
            break;
        case TAG_enumeration_type:
            *flags |= SYM_FLAG_ENUM_TYPE;
            break;
        case TAG_interface_type:
            *flags |= SYM_FLAG_INTERFACE_TYPE;
            break;
        case TAG_unspecified_parameters:
            *flags |= SYM_FLAG_PARAMETER;
            *flags |= SYM_FLAG_VARARG;
            break;
        case TAG_formal_parameter:
        case TAG_variable:
        case TAG_constant:
        case TAG_base_type:
            if (obj->mTag == TAG_formal_parameter) {
                *flags |= SYM_FLAG_PARAMETER;
            }
            else if (obj->mTag == TAG_base_type) {
                if (obj->u.mFundType == ATE_boolean) *flags |= SYM_FLAG_BOOL_TYPE;
            }
            if (get_num_prop(obj, AT_endianity, &v)) {
                if (v == DW_END_big) *flags |= SYM_FLAG_BIG_ENDIAN;
                if (v == DW_END_little) *flags |= SYM_FLAG_LITTLE_ENDIAN;
            }
            break;
        case TAG_fund_type:
            if (obj->u.mFundType == FT_boolean) *flags |= SYM_FLAG_BOOL_TYPE;
            break;
        case TAG_inheritance:
            *flags |= SYM_FLAG_INHERITANCE;
            break;
        }
    }
    if (obj != NULL && !(*flags & (SYM_FLAG_BIG_ENDIAN|SYM_FLAG_LITTLE_ENDIAN))) {
        switch (sym->sym_class) {
        case SYM_CLASS_TYPE:
        case SYM_CLASS_VALUE:
        case SYM_CLASS_REFERENCE:
            *flags |= obj->mCompUnit->mFile->big_endian ? SYM_FLAG_BIG_ENDIAN : SYM_FLAG_LITTLE_ENDIAN;
            break;
        }
    }
    return 0;
}

static void get_local_entry_offset(const Symbol * sym, SymbolProperties * props) {
    ELF_SymbolInfo elf_sym_info;

#ifndef STO_PPC64_LOCAL_MASK
#  define STO_PPC64_LOCAL_BIT 5
#  define STO_PPC64_LOCAL_MASK    (7 << STO_PPC64_LOCAL_BIT)
#endif
#define IS_PPC64_V2(elfsym) ((elfsym->file->machine == EM_PPC64) && (elfsym->file->flags & 0x3) == 2)

    /* PowerPC64 ABIv2 computes local offset from st_other */
    if (sym->tbl != NULL) {
        /* Only do that on PPC64 v2 */
        if (!IS_PPC64_V2(sym->tbl)) return;
        unpack_elf_symbol_info(sym->tbl, sym->index, &elf_sym_info);
    }
    else {
        Symbol * elf_symbol = NULL;
        /* From Dwarf object to Elf symbol */
        map_to_sym_table(sym->obj, &elf_symbol);
        if (elf_symbol == NULL) return;
        if (!IS_PPC64_V2(elf_symbol->tbl)) return;
        unpack_elf_symbol_info(elf_symbol->tbl, elf_symbol->index, &elf_sym_info);
    }
    /* We can compute the offset now */
    props->local_entry_offset = (((1 << (((elf_sym_info.other) & STO_PPC64_LOCAL_MASK) >> STO_PPC64_LOCAL_BIT)) >> 2) << 2);
}

int get_symbol_props(const Symbol * sym, SymbolProperties * props) {
    ObjectInfo * obj = sym->obj;
    Trap trap;

    assert(sym->magic == SYMBOL_MAGIC);
    memset(props, 0, sizeof(SymbolProperties));
    if (sym->base || is_std_type_pseudo_symbol(sym)) return 0;
    if (unpack(sym) < 0) return -1;

    if (!set_trap(&trap)) return -1;

    if (obj != NULL) {
        U8_T n = 0;
        if (obj->mTag == TAG_base_type) {
            if (get_num_prop(obj, AT_binary_scale, &n)) props->binary_scale = (int)n;
            if (get_num_prop(obj, AT_decimal_scale, &n)) props->decimal_scale = (int)n;
        }
        if (obj->mTag == TAG_array_type) {
            if (get_num_prop(obj, AT_stride_size, &n)) props->bit_stride = (unsigned)n;
        }
    }

    get_local_entry_offset(sym, props);

    if (obj != NULL) {
        const char * linkage_name = get_linkage_name(obj);
        if (linkage_name != NULL && linkage_name != obj->mName) {
            props->linkage_name = linkage_name;
        }
    }

    clear_trap(&trap);
    return 0;
}

int get_symbol_frame(const Symbol * sym, Context ** ctx, int * frame) {
    int n = sym->frame;
    if (n == STACK_TOP_FRAME) {
        n = get_top_frame(sym->ctx);
        if (n < 0) return -1;
    }
    *ctx = sym->ctx;
    *frame = n;
    return 0;
}

int get_array_symbol(const Symbol * sym, ContextAddress length, Symbol ** ptr) {
    assert(sym->magic == SYMBOL_MAGIC);
    if (sym->sym_class != SYM_CLASS_UNKNOWN && sym->sym_class != SYM_CLASS_TYPE) return err_wrong_obj();
    *ptr = alloc_symbol();
    (*ptr)->ctx = sym->ctx;
    (*ptr)->frame = sym->frame;
    (*ptr)->sym_class = SYM_CLASS_TYPE;
    (*ptr)->base = (Symbol *)sym;
    (*ptr)->length = length;
    return 0;
}

int get_funccall_info(const Symbol * func,
        const Symbol ** args, unsigned args_cnt, FunctionCallInfo ** res) {
    if (func->obj != NULL) {
        FunctionCallInfo * info = (FunctionCallInfo *)tmp_alloc_zero(sizeof(FunctionCallInfo));
        info->ctx = func->ctx;
        info->func = func;
        info->scope = func->obj->mCompUnit->mRegIdScope;
        info->args_cnt = args_cnt;
        info->args = args;
        if (get_function_call_location_expression(info) < 0) return -1;
        *res = info;
        return 0;
    }
    set_errno(ERR_OTHER, "Func call injection info not available");
    return -1;
}

#endif /* SERVICE_Symbols && ENABLE_ELF */
