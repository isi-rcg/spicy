/*******************************************************************************
 * Copyright (c) 2007-2018 Wind River Systems, Inc. and others.
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
 * TCF service Line Numbers
 * The service associates locations in the source files with the corresponding
 * machine instruction addresses in the executable object.
 */

#ifndef D_linenumbers
#define D_linenumbers

#include <tcf/framework/protocol.h>
#include <tcf/framework/context.h>

#if ENABLE_DebugContext

/*
 * Utility function: read code area data from JSON stream.
 */
extern void read_code_area(InputStream * inp, CodeArea * area);

/*
 * Utility function: write code area data into JSON stream.
 */
extern void write_code_area(OutputStream * out, CodeArea * area, CodeArea * prev);

#if ENABLE_LineNumbers

typedef void LineNumbersCallBack(CodeArea *, void *);

/*
 * Search line number information for given file, line and column.
 * column = 0 means all columns in the line.
 */
extern int line_to_address(Context * ctx, const char * file, int line, int column, LineNumbersCallBack * client, void * args);

/*
 * Search line number information in given run-time address range 'addr0' (inclusive) .. 'addr1' (exclusive).
 */
extern int address_to_line(Context * ctx, ContextAddress addr0, ContextAddress addr1, LineNumbersCallBack * client, void * args);

/*
 * Initialize Line Numbers service.
 */
extern void ini_line_numbers_service(Protocol *);
extern void ini_line_numbers_lib(void);

#endif /* ENABLE_LineNumbers */
#endif /* ENABLE_DebugContext */

#endif /* D_linenumbers */
