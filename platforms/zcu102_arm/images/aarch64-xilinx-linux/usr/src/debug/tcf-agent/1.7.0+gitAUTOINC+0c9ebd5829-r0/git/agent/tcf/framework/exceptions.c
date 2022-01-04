/*******************************************************************************
 * Copyright (c) 2007, 2011 Wind River Systems, Inc. and others.
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

#include <tcf/config.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>
#include <tcf/framework/trace.h>

static Trap * chain = NULL;

int set_trap_a(Trap * trap) {
    assert(is_dispatch_thread());
    memset(trap, 0, sizeof(Trap));
    trap->next = chain;
    chain = trap;
    return 0;
}

int set_trap_b(Trap * trap) {
    if (trap->error == 0) return 1;
    assert(trap == chain);
    chain = trap->next;
    return 0;
}

void clear_trap(Trap * trap) {
    assert(is_dispatch_thread());
    assert(trap == chain);
    chain = trap->next;
}

void exception(int error) {
    assert(is_dispatch_thread());
    assert(error != 0);
    if (chain == NULL) {
        trace(LOG_ALWAYS, "Unhandled exception %d: %s.",
            error, errno_to_str(error));
        exit(error);
    }
    errno = chain->error = error;
    longjmp(chain->env, 1);
}

void str_exception(int error, const char * msg) {
    exception(set_errno(error, msg));
}

void str_fmt_exception(int error, const char * fmt, ...) {
    va_list ap;
    char arr[0x100];
    void * mem = NULL;
    char * buf = arr;
    size_t len = sizeof(arr);
    int n;

    while (1) {
        va_start(ap, fmt);
        n = vsnprintf(buf, len, fmt, ap);
        va_end(ap);
        if (n < 0) {
            if (len > 0x1000) break;
            len *= 2;
        }
        else {
            if (n < (int)len) break;
            len = n + 1;
        }
        mem = loc_realloc(mem, len);
        buf = (char *)mem;
    }

    error = set_errno(error, buf);
    if (mem != NULL) loc_free(mem);
    exception(error);
}
