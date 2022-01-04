/*******************************************************************************
 * Copyright (c) 2007, 2015 Wind River Systems, Inc. and others.
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
 * Symbols service.
 */

#ifndef D_symbolsMux
#define D_symbolsMux

#include <tcf/framework/context.h>
#include <tcf/framework/protocol.h>

#if ENABLE_SymbolsMux

#include <tcf/services/symbols.h>

typedef struct SymbolReader {
    int (*find_symbol_by_name)(Context * ctx, int frame, ContextAddress ip, const char * name, Symbol ** sym);
    int (*find_symbol_in_scope)(Context * ctx, int frame, ContextAddress ip, Symbol * scope, const char * name, Symbol ** sym);
    int (*find_symbol_by_addr)(Context * ctx, int frame, ContextAddress addr, Symbol ** sym);
    int (*find_next_symbol)(Symbol ** sym);
    int (*enumerate_symbols)(Context * ctx, int frame, EnumerateSymbolsCallBack *, void * args);
    const char * (*symbol2id)(const Symbol * sym);
    int (*id2symbol)(const char * id, Symbol ** sym);
    int (*get_symbol_class)(const Symbol * sym, int * symbol_class);
    int (*get_symbol_type)(const Symbol * sym, Symbol ** type);
    int (*get_symbol_type_class)(const Symbol * sym, int * type_class);
    int (*get_symbol_update_policy)(const Symbol * sym, char ** parent_id, int * policy);
    int (*get_symbol_name)(const Symbol * sym, char ** name);
    int (*get_symbol_size)(const Symbol * sym, ContextAddress * size);
    int (*get_symbol_base_type)(const Symbol * sym, Symbol ** base_type);
    int (*get_symbol_index_type)(const Symbol * sym, Symbol ** index_type);
    int (*get_symbol_container)(const Symbol * sym, Symbol ** container);
    int (*get_symbol_length)(const Symbol * sym, ContextAddress * length);
    int (*get_symbol_lower_bound)(const Symbol * sym, int64_t * value);
    int (*get_symbol_children)(const Symbol * sym, Symbol *** children, int * count);
    int (*get_symbol_flags)(const Symbol * sym, SYM_FLAGS * flags);
    int (*get_symbol_props)(const Symbol * sym, SymbolProperties * props);
    int (*get_symbol_frame)(const Symbol * sym, Context ** ctx, int * frame);
    int (*get_array_symbol)(const Symbol * sym, ContextAddress length, Symbol ** ptr);
    ContextAddress (*is_plt_section)(Context * ctx, ContextAddress addr);
    int (*get_location_info)(const Symbol * sym, LocationInfo ** info);
    int (*get_funccall_info)(const Symbol * func,
            const Symbol ** args, unsigned args_cnt, FunctionCallInfo ** info);
    int (*get_stack_tracing_info)(Context * ctx, ContextAddress addr, StackTracingInfo ** info);
    int (*get_symbol_file_info)(Context * ctx, ContextAddress addr, SymbolFileInfo ** info);
    int (*get_context_isa)(Context * ctx, ContextAddress ip, const char ** isa,
        ContextAddress * range_addr, ContextAddress * range_size);
    int (*reader_is_valid)(Context * ctx, ContextAddress addr);
    unsigned reader_index;
} SymbolReader;

#ifdef SYM_READER_PREFIX

#define JOIN(a,b) JOIN1(a,b)
#define JOIN1(a,b) a##b
#define READER_NAME(name)  JOIN(SYM_READER_PREFIX,name)


#define find_symbol_by_name READER_NAME(find_symbol_by_name)
#define find_symbol_in_scope READER_NAME(find_symbol_in_scope)
#define find_symbol_by_addr READER_NAME(find_symbol_by_addr)
#define find_next_symbol READER_NAME(find_next_symbol)
#define enumerate_symbols READER_NAME(enumerate_symbols)
#define symbol2id READER_NAME(symbol2id)
#define id2symbol READER_NAME(id2symbol)
#define get_symbol_class READER_NAME(get_symbol_class)
#define get_symbol_type READER_NAME(get_symbol_type)
#define get_symbol_type_class READER_NAME(get_symbol_type_class)
#define get_symbol_update_policy READER_NAME(get_symbol_update_policy)
#define get_symbol_base_type READER_NAME(get_symbol_base_type)
#define get_symbol_index_type READER_NAME(get_symbol_index_type)
#define get_symbol_container READER_NAME(get_symbol_container)
#define get_symbol_length READER_NAME(get_symbol_length)
#define get_symbol_lower_bound READER_NAME(get_symbol_lower_bound)
#define get_symbol_children READER_NAME(get_symbol_children)
#define get_symbol_flags READER_NAME(get_symbol_flags)
#define get_symbol_props READER_NAME(get_symbol_props)
#define get_symbol_frame READER_NAME(get_symbol_frame)
#define get_array_symbol READER_NAME(get_array_symbol)
#define get_symbol_name READER_NAME(get_symbol_name)
#define get_symbol_size READER_NAME(get_symbol_size)
#define is_plt_section READER_NAME(is_plt_section)
#define get_location_info READER_NAME(get_location_info)
#define get_funccall_info READER_NAME(get_funccall_info)
#define get_stack_tracing_info READER_NAME(get_stack_tracing_info)
#define get_symbol_file_info READER_NAME(get_symbol_file_info)
#define get_context_isa READER_NAME(get_context_isa)
#define symbol_reader READER_NAME(symbol_reader)
#define ini_symbols_lib READER_NAME(ini_symbols_lib)

extern int find_symbol_by_name(Context * ctx, int frame, ContextAddress ip, const char * name, Symbol ** sym);
extern int find_symbol_in_scope(Context * ctx, int frame, ContextAddress ip, Symbol * scope, const char * name, Symbol ** sym);
extern int find_symbol_by_addr(Context * ctx, int frame, ContextAddress addr, Symbol ** sym);
extern int find_next_symbol(Symbol ** sym);
extern int enumerate_symbols(Context * ctx, int frame, EnumerateSymbolsCallBack *, void * args);
extern const char * symbol2id(const Symbol * sym);
extern int id2symbol(const char * id, Symbol ** sym);
extern int get_symbol_class(const Symbol * sym, int * symbol_class);
extern int get_symbol_type(const Symbol * sym, Symbol ** type);
extern int get_symbol_type_class(const Symbol * sym, int * type_class);
extern int get_symbol_update_policy(const Symbol * sym, char ** parent_id, int * policy);
extern int get_symbol_name(const Symbol * sym, char ** name);
extern int get_symbol_size(const Symbol * sym, ContextAddress * size);
extern int get_symbol_base_type(const Symbol * sym, Symbol ** base_type);
extern int get_symbol_index_type(const Symbol * sym, Symbol ** index_type);
extern int get_symbol_container(const Symbol * sym, Symbol ** container);
extern int get_symbol_length(const Symbol * sym, ContextAddress * length);
extern int get_symbol_lower_bound(const Symbol * sym, int64_t * value);
extern int get_symbol_children(const Symbol * sym, Symbol *** children, int * count);
extern int get_symbol_flags(const Symbol * sym, SYM_FLAGS * flags);
extern int get_symbol_props(const Symbol * sym, SymbolProperties * props);
extern int get_symbol_frame(const Symbol * sym, Context ** ctx, int * frame);
extern ContextAddress is_plt_section(Context * ctx, ContextAddress addr);
extern int get_location_info(const Symbol * sym, LocationInfo ** info);
extern int get_funccall_info(const Symbol * func,
        const Symbol ** args, unsigned args_cnt, FunctionCallInfo ** info);
extern int get_stack_tracing_info(Context * ctx, ContextAddress addr, StackTracingInfo ** info);
extern int get_symbol_file_info(Context * ctx, ContextAddress addr, SymbolFileInfo ** info);
extern int get_array_symbol(const Symbol * sym, ContextAddress length, Symbol ** ptr);
extern int get_context_isa(Context * ctx, ContextAddress ip, const char ** isa,
        ContextAddress * range_addr, ContextAddress * range_size);
static int reader_is_valid(Context * ctx, ContextAddress addr);
extern void ini_symbols_lib(void);

static SymbolReader symbol_reader = {
    find_symbol_by_name, find_symbol_in_scope,
    find_symbol_by_addr, find_next_symbol, enumerate_symbols, symbol2id, id2symbol,
    get_symbol_class, get_symbol_type, get_symbol_type_class, get_symbol_update_policy,
    get_symbol_name, get_symbol_size, get_symbol_base_type, get_symbol_index_type,
    get_symbol_container, get_symbol_length, get_symbol_lower_bound, get_symbol_children,
    get_symbol_flags, get_symbol_props, get_symbol_frame, get_array_symbol, is_plt_section, get_location_info,
    get_funccall_info, get_stack_tracing_info, get_symbol_file_info,
    get_context_isa, reader_is_valid
};

#endif  /* SYM_READER_PREFIX */

extern int symbols_mux_id2symbol(const char * id, Symbol ** sym);
extern const char * symbols_mux_symbol2id(Symbol * sym);

extern int add_symbols_reader(SymbolReader * reader);

#endif /* ENABLE_SymbolsMux */

#endif /* D_symbolsMux */
