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
 * Line Numbers Muliplexer - Provides a multiplexer to support several
 * file format in the same TCF agent and in the same debug session.
 *
 * The multiplexer delegates actual search for line number info to info readers
 * that are registed with add_line_numbers_reader().
 */

#include <tcf/config.h>

#if ENABLE_LineNumbers && ENABLE_LineNumbersMux

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <tcf/framework/myalloc.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/linenumbers_mux.h>

static LineNumbersReader ** readers = NULL;
static unsigned reader_count = 0;
static unsigned max_reader_count = 0;

int line_to_address(Context * ctx, const char * file_name, int line, int column,
                    LineNumbersCallBack * client, void * args) {
    unsigned i;
    for (i = 0; i < reader_count; i++) {
        if (readers[i]->line_to_address(ctx, file_name, line, column, client, args) < 0) {
            return -1;
        }
    }
    return 0;
}

int address_to_line(Context * ctx, ContextAddress addr0, ContextAddress addr1,
                    LineNumbersCallBack * client, void * args) {
    unsigned i;
    for (i = 0; i < reader_count; i++) {
        if (readers[i]->address_to_line(ctx, addr0, addr1, client, args) < 0) {
            return -1;
        }
    }
    return 0;
}

int add_line_numbers_reader(LineNumbersReader * reader) {
    if (reader_count >= max_reader_count) {
        max_reader_count += 2;
        readers = (LineNumbersReader **)loc_realloc(readers, max_reader_count * sizeof(LineNumbersReader *));
    }
    reader->reader_index = reader_count;
    readers[reader_count++] = reader;
    return 0;
}

extern void elf_reader_ini_line_numbers_lib(void);
extern void win32_reader_ini_line_numbers_lib(void);
extern void proxy_reader_ini_line_numbers_lib(void);

void ini_line_numbers_lib(void) {
    /*
     * We keep this to limit the impact of changes. In the ideal world, those
     * initialization routines should be called from the agent initialization code.
     */
#if ENABLE_ELF
    elf_reader_ini_line_numbers_lib();
#endif
#if ENABLE_PE
    win32_reader_ini_line_numbers_lib();
#endif
#if ENABLE_LineNumbersProxy
    proxy_reader_ini_line_numbers_lib();
#endif
}

#endif /* ENABLE_LineNumbers && ENABLE_LineNumbersMux */
