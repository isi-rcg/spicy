/*******************************************************************************
 * Copyright (c) 2012, 2014 Wind River Systems, Inc. and others.
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
 * This module implements ELF-specific symbol's API
 */
#ifndef D_symbols_elf
#define D_symbols_elf

#include <tcf/config.h>

#if SERVICE_Symbols && (!ENABLE_SymbolsProxy || ENABLE_SymbolsMux) && ENABLE_ELF

#include <tcf/framework/context.h>
#include <tcf/services/symbols.h>
#include <tcf/services/tcf_elf.h>
#include <tcf/services/elf-symbols-ext.h>

typedef struct EnumerateSymbols EnumerateSymbols;
typedef int EnumerateBatchSymbolsCallBack(void *, Symbol *);

/*
 * Enumerate ELF symbols of a specific file for a given context.
 * The API supports enumeration by batchs of symbols. If the callback returns
 * 0, the function returns, ending a batch. Subsequent calls with a NULL
 * context and file_name is used to enumerate the next batch of symbols. If
 * the callback return -1, the enumeration ends.
 * On error returns -1 and sets errno.
 * On success returns 0 if there isn't more symbols, or a positive value if
 * there are other symbols to retrieve.
 */
extern int elf_enumerate_symbols(Context * ctx, const char * file_name, EnumerateSymbols ** enum_syms,
                                 EnumerateBatchSymbolsCallBack * call_back, void * args);

/*
 * Get the ELF symbol info from a TCF symbol.
 * On error returns -1 and sets errno.
 * On success returns 0.
 */
extern int elf_symbol_info(Symbol * sym, ELF_SymbolInfo * elf_sym);

/*
 * Get the TCF Symbol from ELF symbol info.
 * On error returns -1 and sets errno.
 * On success returns 0.
 */
extern int elf_tcf_symbol(Context * ctx, ELF_SymbolInfo * elf_sym, Symbol ** sym);

/*
 * Map ELF symbol table entry value to run-time address in given context address space.
 */
extern int elf_symbol_address(Context * ctx, ELF_SymbolInfo * info, ContextAddress * address);

#endif /* SERVICE_Symbols && (!ENABLE_SymbolsProxy || ENABLE_SymbolsMux) && ENABLE_ELF */

#if ENABLE_ELF

/*
 * Save/restore ELF/DWARF symbols reader state to allow recursion in the symbols service implementation.
 */
typedef void ELFSymbolsRecursiveCall(void *);
extern int elf_save_symbols_state(ELFSymbolsRecursiveCall * func, void * args);

#endif /* ENABLE_ELF */

#endif /* D_symbols_elf */
