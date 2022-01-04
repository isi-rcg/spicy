/*******************************************************************************
 * Copyright (c) 2012, 2017 Wind River Systems, Inc. and others.
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
 * Symbols Muliplexer - Provides a multiplexer to support several
 * file format in the same TCF agent and in the same debug session.
 *
 * The multiplexer delegates actual search for symbols info to info readers
 * that are registed with add_symbols_reader().
 */

#include <tcf/config.h>

#if ENABLE_Symbols && ENABLE_SymbolsMux

#include <assert.h>
#include <stdio.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/context.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/symbols.h>
#include <tcf/services/symbols_mux.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/stacktrace.h>

static SymbolReader ** readers = NULL;
static unsigned reader_cnt = 0;
static unsigned max_reader_cnt = 0;
static Context * find_symbol_ctx = NULL;
static Symbol ** find_symbol_list = NULL;

static int get_sym_addr(Context * ctx, int frame, ContextAddress addr, ContextAddress * sym_addr) {
    if (frame == STACK_NO_FRAME) {
        *sym_addr = addr;
    }
    else if (is_top_frame(ctx, frame)) {
        if (!is_ctx_stopped(ctx)) return -1;
        if (get_PC(ctx, sym_addr) < 0) return -1;
    }
    else {
        uint64_t ip = 0;
        StackFrame * info = NULL;
        if (get_frame_info(ctx, frame, &info) < 0) return -1;
        if (read_reg_value(info, get_PC_definition(ctx), &ip) < 0) return -1;
        assert(!info->is_top_frame);
        if (ip > 0) ip--;
        *sym_addr = (ContextAddress)ip;
    }
    return 0;
}

static int get_symbol_reader(Context * ctx, int frame, ContextAddress addr, SymbolReader ** sym_reader) {
    ContextAddress sym_addr;
    unsigned i;

    *sym_reader = NULL;
    if (reader_cnt == 1) {
        *sym_reader = readers[0];
        return 0;
    }
    if (get_sym_addr(ctx, frame, addr, &sym_addr) < 0) return -1;
    for (i = 0; i < reader_cnt; i++) {
        int valid = readers[i]->reader_is_valid(ctx, sym_addr);
        if (cache_miss_count() > 0) {
            errno = ERR_CACHE_MISS;
            return -1;
        }
        if (valid) {
            *sym_reader = readers[i];
            return 0;
        }
    }
    return 0;
}

int find_symbol_by_name(Context * ctx, int frame, ContextAddress ip, const char * name, Symbol ** res) {
    unsigned i;
    Context * symbol_ctx = NULL;
    Symbol ** symbol_list = NULL;

    *res = NULL;
    find_symbol_ctx = NULL;
    find_symbol_list = NULL;

    for (i = 0; i < reader_cnt; i++) {
        Symbol * sym = NULL;
        SymbolReader * reader = readers[i];
        if (reader->find_symbol_by_name(ctx, frame, ip, name, &sym) == 0) {
            assert(sym != NULL);
            if (symbol_ctx == NULL) {
                symbol_list = (Symbol **)tmp_alloc_zero(sizeof(Symbol *) * reader_cnt);
                if (reader->find_next_symbol(&symbol_list[i]) < 0) symbol_list[i] = NULL;
                symbol_ctx = ctx;
                *res = sym;
            }
            else {
                symbol_list[i] = sym;
            }
        }
        else if (get_error_code(errno) != ERR_SYM_NOT_FOUND) {
            return -1;
        }
    }
    if (symbol_ctx != NULL) {
        find_symbol_ctx = symbol_ctx;
        find_symbol_list = symbol_list;
        assert(*res != NULL);
        return 0;
    }
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

int find_symbol_in_scope(Context * ctx, int frame, ContextAddress ip, Symbol * scope,
        const char * name, Symbol ** res) {
    unsigned i;
    Context * symbol_ctx = NULL;
    Symbol ** symbol_list = NULL;

    *res = NULL;
    find_symbol_ctx = NULL;
    find_symbol_list = NULL;

    for (i = 0; i < reader_cnt; i++) {
        Symbol * sym = NULL;
        SymbolReader * reader = readers[i];
        if (scope != NULL && *(SymbolReader **)scope != reader) continue;
        if (reader->find_symbol_in_scope(ctx, frame, ip, scope, name, &sym) == 0) {
            assert(sym != NULL);
            if (symbol_ctx == NULL) {
                symbol_list = (Symbol **)tmp_alloc_zero(sizeof(Symbol *) * reader_cnt);
                if (reader->find_next_symbol(&symbol_list[i]) < 0) symbol_list[i] = NULL;
                symbol_ctx = ctx;
                *res = sym;
            }
            else {
                symbol_list[i] = sym;
            }
        }
        else if (get_error_code(errno) != ERR_SYM_NOT_FOUND) {
            return -1;
        }
    }
    if (symbol_ctx != NULL) {
        find_symbol_ctx = symbol_ctx;
        find_symbol_list = symbol_list;
        assert(*res != NULL);
        return 0;
    }
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

int find_symbol_by_addr(Context * ctx, int frame, ContextAddress addr, Symbol ** res) {
    SymbolReader * reader;

    *res = NULL;
    find_symbol_ctx = NULL;
    find_symbol_list = NULL;

    if (get_symbol_reader(ctx, frame, addr, &reader) < 0) return -1;
    if (reader != NULL) {
        Symbol ** symbol_list = NULL;
        unsigned i = reader->reader_index;
        if (reader->find_symbol_by_addr(ctx, frame, addr, res) < 0) return -1;
        symbol_list = (Symbol **)tmp_alloc_zero(sizeof(Symbol *) * reader_cnt);
        if (reader->find_next_symbol(&symbol_list[i]) < 0) symbol_list[i] = NULL;
        find_symbol_ctx = ctx;
        find_symbol_list = symbol_list;
        assert(*res != NULL);
        return 0;
    }
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

int find_next_symbol(Symbol ** sym) {
    unsigned i;

    *sym = NULL;
    if (find_symbol_ctx == NULL) {
        errno = ERR_SYM_NOT_FOUND;
        return -1;
    }
    for (i = 0; i < reader_cnt; i++) {
        if (find_symbol_list[i] != NULL) {
            *sym = find_symbol_list[i];
            if (readers[i]->find_next_symbol(&find_symbol_list[i]) < 0) {
                find_symbol_list[i] = NULL;
            }
            assert(*sym != NULL);
            return 0;
        }
    }
    find_symbol_ctx = NULL;
    find_symbol_list = NULL;
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

int enumerate_symbols(Context * ctx, int frame, EnumerateSymbolsCallBack * call_back, void * args) {
    SymbolReader * reader = NULL;
    if (get_symbol_reader(ctx, frame, 0, &reader) < 0) return -1;
    if (reader) return reader->enumerate_symbols(ctx, frame, call_back, args);
    return 0;
}

const char * symbol2id(const Symbol * sym) {
    SymbolReader * reader = *(SymbolReader **)sym;
    static char buf[256];
    const char * id;
    assert (reader != NULL);
    id = reader->symbol2id(sym);
    if (id) {
        buf[0] = '@';
        buf[1] = 'M';
        buf[2] = (uint8_t)reader->reader_index + '0';
        buf[3] = '.';
        strlcpy(&buf[4], id, sizeof(buf) - 4);
        return buf;
    }
    return id;
}

int id2symbol(const char * id, Symbol ** res) {
    unsigned reader_index;
    if (id != NULL && id[0] == '@' && id[1] == 'M' && id[3] == '.') {
        reader_index = id[2] - '0';
        assert (reader_index < reader_cnt);
        return (readers[reader_index]->id2symbol(id + 4, res));
    }
    errno = ERR_INV_CONTEXT;
    return -1;
}

ContextAddress is_plt_section(Context * ctx, ContextAddress addr) {
    SymbolReader * reader = NULL;
    if (get_symbol_reader(ctx, STACK_NO_FRAME, addr, &reader) < 0) return 0;
    if (reader) return reader->is_plt_section(ctx, addr);
    return 0;
}

int get_stack_tracing_info(Context * ctx, ContextAddress addr, StackTracingInfo ** info) {
    SymbolReader * reader = NULL;
    *info  = NULL;
    if (get_symbol_reader(ctx, STACK_NO_FRAME, addr, &reader) < 0) return -1;
    if (reader) return reader->get_stack_tracing_info(ctx, addr, info);
    return 0;
}

static int error_priority(int error) {
    switch (get_error_code(error)) {
    case 0: return 0;
    case ERR_INV_FORMAT: return 1;
    case ENOENT: return 2;
    }
    return 3;
}

int get_symbol_file_info(Context * ctx, ContextAddress addr, SymbolFileInfo ** info) {
    int error = 0;
    unsigned i;
    for (i = 0; i < reader_cnt; i++) {
        int r = readers[i]->get_symbol_file_info(ctx, addr, info);
        if (r == 0 && *info != NULL) return 0;
        if (error_priority(errno) > error_priority(error)) error = errno;
    }
    *info = NULL;
    if (error) {
        errno = error;
        return -1;
    }
    return 0;
}

int get_symbol_class(const Symbol * sym, int * sym_class) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_class(sym, sym_class);
}

int get_symbol_type(const Symbol * sym, Symbol ** type) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_type(sym, type);
}

int get_symbol_type_class(const Symbol * sym, int * type_class) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_type_class(sym, type_class);
}

int get_symbol_update_policy(const Symbol * sym, char ** id, int * policy) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_update_policy(sym, id, policy);
}

int get_symbol_name(const Symbol * sym, char ** name) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_name(sym, name);
}

int get_symbol_size(const Symbol * sym, ContextAddress * size) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_size(sym, size);
}

int get_symbol_base_type(const Symbol * sym, Symbol ** base_type) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_base_type(sym, base_type);
}

int get_symbol_index_type(const Symbol * sym, Symbol ** index_type) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_index_type(sym, index_type);
}

int get_symbol_container(const Symbol * sym, Symbol ** container) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_container(sym, container);
}

int get_symbol_length(const Symbol * sym, ContextAddress * length) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_length(sym, length);
}

int get_symbol_lower_bound(const Symbol * sym, int64_t * value) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_lower_bound(sym, value);
}

int get_symbol_children(const Symbol * sym, Symbol *** children, int * count) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_children(sym, children, count);
}

int get_array_symbol(const Symbol * sym, ContextAddress length, Symbol ** ptr) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_array_symbol(sym, length, ptr);
}

int get_location_info(const Symbol * sym, LocationInfo ** res) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_location_info(sym, res);
}

int get_symbol_flags(const Symbol * sym, SYM_FLAGS * flags) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_flags(sym, flags);
}

int get_symbol_props(const Symbol * sym, SymbolProperties * props) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_props(sym, props);
}

int get_symbol_frame(const Symbol * sym, Context ** ctx, int * frame) {
    SymbolReader * reader = *(SymbolReader **)sym;
    return reader->get_symbol_frame(sym, ctx, frame);
}

int get_funccall_info(const Symbol * func, const Symbol ** args, unsigned args_cnt, FunctionCallInfo ** res) {
    SymbolReader * reader = *(SymbolReader **)func;
    return reader->get_funccall_info(func, args, args_cnt, res);
}

int get_context_isa(Context * ctx, ContextAddress addr, const char ** isa,
        ContextAddress * range_addr, ContextAddress * range_size) {
    SymbolReader * reader = NULL;
    *isa = NULL;
    *range_addr = addr;
    *range_size = 1;

    if (get_symbol_reader(ctx, STACK_NO_FRAME, addr, &reader) < 0) return -1;
    if (reader != NULL) return reader->get_context_isa(ctx, addr, isa, range_addr, range_size);
    return 0;
}

int add_symbols_reader(SymbolReader * reader) {
    if (reader_cnt >= max_reader_cnt) {
        max_reader_cnt += 2;
        readers = (SymbolReader **)loc_realloc(readers, max_reader_cnt * sizeof(SymbolReader *));
    }
    reader->reader_index = reader_cnt;
    readers[reader_cnt++] = reader;
    return 0;
}

int symbols_mux_id2symbol(const char * id, Symbol ** res) {
    return id2symbol(id, res);
}

const char * symbols_mux_symbol2id(Symbol * sym) {
    return symbol2id(sym);
}

extern void elf_reader_ini_symbols_lib(void);
extern void win32_reader_ini_symbols_lib(void);
extern void proxy_reader_ini_symbols_lib(void);

void ini_symbols_lib(void) {
    /*
     * We keep this to limit the impact of changes. In the ideal world, those
     * initialization routines should be called from the agent initialization code.
     */
#if ENABLE_ELF
    elf_reader_ini_symbols_lib();
#endif
#if ENABLE_PE
    win32_reader_ini_symbols_lib();
#endif
#if ENABLE_SymbolsProxy
    proxy_reader_ini_symbols_lib();
#endif
}
#endif /* ENABLE_Symbols && ENABLE_SymbolsMux */
