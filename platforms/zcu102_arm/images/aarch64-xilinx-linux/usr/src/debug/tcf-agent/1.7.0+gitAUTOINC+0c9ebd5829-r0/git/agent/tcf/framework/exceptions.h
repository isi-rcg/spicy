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
 * Exception handling. Functionality is similar to C++ try/catch.
 * Usage example:
    Trap trap;
    if (set_trap(&trap)) {
        // Some code that can throw an exception by calling exception()
        ...

        clear_trap(&trap);
    }
    else {
        // Exception handling code
        if (trap.error == ...
        ...
    }
 * Only main thread is allowed to use exceptions.
 */

#ifndef D_exceptions
#define D_exceptions

#include <setjmp.h>
#include <tcf/framework/errors.h>

typedef struct Trap Trap;

struct Trap {
    jmp_buf env;
    Trap * next;
    int error;
};

#define set_trap(trap) (set_trap_a(trap), setjmp((trap)->env), set_trap_b(trap))

extern int set_trap_a(Trap * trap);
extern int set_trap_b(Trap * trap);

extern void clear_trap(Trap * trap);

#if defined(_MSC_VER)
    __declspec(noreturn) extern void exception(int error);
    __declspec(noreturn) extern void str_exception(int error, const char * msg);
    __declspec(noreturn) extern void str_fmt_exception(int error, const char * fmt, ...);
#else
    extern void exception(int error) ATTR_NORETURN;
    extern void str_exception(int error, const char * msg) ATTR_NORETURN;
    extern void str_fmt_exception(int error, const char * fmt, ...) ATTR_NORETURN ATTR_PRINTF(2, 3);
#endif

#endif /* D_exceptions */
