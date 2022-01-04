/*******************************************************************************
 * Copyright (c) 2007, 2014 Wind River Systems, Inc. and others.
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

#ifndef D_linenumbersMux
#define D_linenumbersMux

#include <tcf/framework/context.h>

#if ENABLE_LineNumbersMux

#include <tcf/services/linenumbers.h>

typedef struct LineNumbersReader {
    int (*line_to_address)(Context * ctx, const char * file, int line, int column,
            LineNumbersCallBack * client, void * args);
    int (*address_to_line)(Context * ctx, ContextAddress addr0, ContextAddress addr1,
            LineNumbersCallBack * client, void * args);
    unsigned reader_index;
} LineNumbersReader;

#ifdef LINENUMBERS_READER_PREFIX

#define JOIN(a,b) JOIN1(a,b)
#define JOIN1(a,b) a##b
#define READER_NAME(name)  JOIN(LINENUMBERS_READER_PREFIX,name)


#define line_to_address READER_NAME(line_to_address)
#define address_to_line READER_NAME(address_to_line)
#define ini_line_numbers_lib READER_NAME(ini_line_numbers_lib)

extern int line_to_address(Context * ctx, const char * file, int line, int column, LineNumbersCallBack * client, void * args);
extern int address_to_line(Context * ctx, ContextAddress addr0, ContextAddress addr1, LineNumbersCallBack * client, void * args);
extern void ini_line_numbers_lib(void);

static LineNumbersReader line_numbers_reader = { line_to_address, address_to_line };

#endif  /* SYM_READER_PREFIX */

extern int add_line_numbers_reader(LineNumbersReader * reader);

#endif /* ENABLE_LineNumbersMux */

#endif /* D_linenumbersMux */
