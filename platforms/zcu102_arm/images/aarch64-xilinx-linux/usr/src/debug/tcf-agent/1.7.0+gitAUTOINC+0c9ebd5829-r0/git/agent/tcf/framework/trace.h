/*******************************************************************************
 * Copyright (c) 2007-2020 Wind River Systems, Inc. and others.
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
 * Log file and tracing.
 */

#ifndef D_trace
#define D_trace

#include <tcf/config.h>
#include <stdio.h>

/* Always update trace.c when adding or removing predefined log levels */
#define LOG_ALWAYS      0x0
#define LOG_ALLOC       0x1
#define LOG_EVENTCORE   0x2
#define LOG_WAITPID     0x4
#define LOG_EVENTS      0x8
#define LOG_CHILD       0x10
#define LOG_PROTOCOL    0x20
#define LOG_CONTEXT     0x40
#define LOG_DISCOVERY   0x80
#define LOG_ASYNCREQ    0x100
#define LOG_PROXY       0x200
#define LOG_TCFLOG      0x400
#define LOG_ELF         0x800
#define LOG_LUA         0x1000
#define LOG_STACK       0x2000
#define LOG_PLUGIN      0x4000
#define LOG_SHUTDOWN    0x8000
#define LOG_DISASM      0x10000

#define LOG_NAME_STDERR "-"

extern int log_mode;

#if ENABLE_Trace

/*
 * Print a trace message into log file.
 * Use macro 'trace' instead of calling this function directly.
 */
extern int print_trace(int mode, const char * fmt, ...) ATTR_PRINTF(2, 3);

extern FILE * log_file;

#ifndef trace
#  define trace log_file == NULL ? (void)0 : (void)print_trace
#endif /* not def trace */

#else /* not ENABLE_Trace */

#ifndef trace
#  if (defined(_MSC_VER) && _MSC_VER >= 1400) || defined(__GNUC__)
#    define trace(...) ((void)0)
#  else
#    define trace 0 &&
#  endif
#endif /* not def trace */

#endif /* ENABLE_Trace */

struct trace_mode {
    int mode;
    const char * name;
    const char * description;
};

extern struct trace_mode trace_mode_table[];

extern int parse_trace_mode(const char * mode, int * result);

extern int add_trace_mode(int mode, const char * name, const char * description);

extern void ini_trace(void);

extern void open_log_file(const char * name);

#endif /* D_trace */
