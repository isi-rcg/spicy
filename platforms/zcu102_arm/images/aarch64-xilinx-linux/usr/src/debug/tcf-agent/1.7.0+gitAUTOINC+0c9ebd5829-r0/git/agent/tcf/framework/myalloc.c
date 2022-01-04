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
 * Local memory heap manager.
 */

#include <tcf/config.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <tcf/framework/link.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/events.h>
#include <tcf/framework/myalloc.h>

#define ALIGNMENT (sizeof(void *) < 8 ? 8 : sizeof(void *))

#if !defined(ENABLE_FastMemAlloc)
#  define ENABLE_FastMemAlloc 1
#endif

#if !defined(USE_libc_malloc)
#  define USE_libc_malloc 1
#endif

#if ENABLE_FastMemAlloc
#define POOL_SIZE (0xfff0 * MEM_USAGE_FACTOR)
static char * tmp_pool = NULL;
static size_t tmp_pool_pos = 0;
static size_t tmp_pool_max = 0;
static size_t tmp_pool_avr = 0;
#endif

static LINK tmp_alloc_list = TCF_LIST_INIT(tmp_alloc_list);
static size_t tmp_alloc_size = 0;
static int tmp_gc_posted = 0;

static void gc_event(void * args) {
    tmp_gc_posted = 0;
    tmp_gc();
}

void tmp_gc(void) {
#if ENABLE_FastMemAlloc
    if (tmp_pool_pos + tmp_alloc_size >= tmp_pool_avr) {
        tmp_pool_avr = tmp_pool_pos + tmp_alloc_size;
    }
    else if (tmp_pool_avr > POOL_SIZE / 0x10) {
        tmp_pool_avr -= POOL_SIZE / 0x10000;
    }
    if (tmp_pool_max < tmp_pool_avr && tmp_pool_max < POOL_SIZE) {
        if (tmp_pool_max < POOL_SIZE / 0x10) tmp_pool_max = POOL_SIZE / 0x10;
        while (tmp_pool_max < tmp_pool_avr) tmp_pool_max *= 2;
        if (tmp_pool_max > POOL_SIZE) tmp_pool_max = POOL_SIZE;
        tmp_pool = (char *)loc_realloc(tmp_pool, tmp_pool_max);
    }
    else if (tmp_pool_avr < tmp_pool_max / 4 && tmp_pool_max > POOL_SIZE / 0x10) {
        tmp_pool_max /= 2;
        tmp_pool = (char *)loc_realloc(tmp_pool, tmp_pool_max);
    }
    tmp_pool_pos = sizeof(LINK);
#endif
    while (!list_is_empty(&tmp_alloc_list)) {
        LINK * l = tmp_alloc_list.next;
        list_remove(l);
        loc_free(l);
    }
    tmp_alloc_size = 0;
}

void * tmp_alloc(size_t size) {
    void * p;
    LINK * l;
    assert(is_dispatch_thread());
    if (!tmp_gc_posted) {
        post_event(gc_event, NULL);
        tmp_gc_posted = 1;
    }
#if ENABLE_FastMemAlloc
    if (tmp_pool_pos + size + ALIGNMENT + sizeof(size_t *) > tmp_pool_max) {
        if (tmp_pool != NULL) {
            l = (LINK *)tmp_pool;
            list_add_last(l, &tmp_alloc_list);
            tmp_alloc_size += tmp_pool_pos;
        }
        tmp_pool_max = POOL_SIZE / 0x10 + size;
        tmp_pool = (char *)loc_alloc(tmp_pool_max);
        tmp_pool_pos = sizeof(LINK);
    }
    tmp_pool_pos += sizeof(size_t *);
    tmp_pool_pos = (tmp_pool_pos + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    p = tmp_pool + tmp_pool_pos;
    *((size_t *)p - 1) = size;
    tmp_pool_pos += size;
    return p;
#else
    l = (LINK *)loc_alloc(sizeof(LINK) + size);
    list_add_last(l, &tmp_alloc_list);
    tmp_alloc_size += size + ALIGNMENT + sizeof(size_t *);
    p = l + 1;
    return p;
#endif
}

void * tmp_alloc_zero(size_t size) {
    return memset(tmp_alloc(size), 0, size);
}

void * tmp_realloc(void * ptr, size_t size) {
    if (ptr == NULL) return tmp_alloc(size);
    assert(is_dispatch_thread());
    assert(tmp_gc_posted);
#if ENABLE_FastMemAlloc
    {
        void * p;
        size_t m = *((size_t *)ptr - 1);
        if (m >= size) return ptr;
        if ((char *)ptr >= tmp_pool && (char *)ptr <= tmp_pool + tmp_pool_max) {
            size_t pos = tmp_pool_pos - m;
            if (ptr == tmp_pool + pos && pos + size <= tmp_pool_max) {
                tmp_pool_pos = pos + size;
                *((size_t *)ptr - 1) = size;
                return ptr;
            }
        }
        p = tmp_alloc(size);
        if (m > size) m = size;
        return memcpy(p, ptr, m);
    }
#else
    {
        LINK * l = (LINK *)ptr - 1;
        list_remove(l);
        l = (LINK *)loc_realloc(l, sizeof(LINK) + size);
        list_add_last(l, &tmp_alloc_list);
        return l + 1;
    }
#endif
}

char * tmp_strdup(const char * s) {
    size_t len = strlen(s) + 1;
    return (char *)memcpy(tmp_alloc(len), s, len);
}

char * tmp_strdup2(const char * s1, const char * s2) {
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    char * rval = (char *)tmp_alloc(l1 + l2 + 1);
    memcpy(rval, s1, l1);
    memcpy(rval + l1, s2, l2 + 1);
    return rval;
}

char * tmp_strndup(const char * s, size_t len) {
    char * rval = (char *)tmp_alloc(len + 1);
    rval[len] = '\0';
    return strncpy(rval, s, len);
}

char * tmp_printf(const char * fmt, ...) {
    va_list ap;
    char * buf = NULL;
    va_start(ap, fmt);
    buf = tmp_vprintf(fmt, ap);
    va_end(ap);
    return buf;
}

char * tmp_vprintf(const char * fmt, va_list ap) {
    char arr[0x100];
    void * mem = NULL;
    char * buf = arr;
    size_t len = sizeof(arr);
    int n;

    while (1) {
        n = vsnprintf(buf, len, fmt, ap);
        if (n < 0) {
            if (len > 0x1000) break;
            len *= 2;
        }
        else {
            if (n < (int)len) break;
            len = n + 1;
        }
        mem = tmp_realloc(mem, len);
        buf = (char *)mem;
    }
    if (buf == arr) buf = tmp_strdup(arr);
    return buf;
}

#if USE_libc_malloc

void * loc_alloc(size_t size) {
    void * p;

    if (size == 0) {
        size = 1;
    }
    if ((p = malloc(size)) == NULL) {
        perror("malloc");
        exit(1);
    }
    trace(LOG_ALLOC, "loc_alloc(%u) = %#" PRIxPTR, (unsigned)size, (uintptr_t)p);
    return p;
}

void * loc_alloc_zero(size_t size) {
    void * p;

    if (size == 0) size = 1;
    if ((p = malloc(size)) == NULL) {
        perror("malloc");
        exit(1);
    }
    memset(p, 0, size);
    trace(LOG_ALLOC, "loc_alloc_zero(%u) = %#" PRIxPTR, (unsigned)size, (uintptr_t)p);
    return p;
}

void * loc_realloc(void * ptr, size_t size) {
    void * p;

    if (size == 0) size = 1;
    if ((p = realloc(ptr, size)) == NULL) {
        perror("realloc");
        exit(1);
    }
    trace(LOG_ALLOC, "loc_realloc(%#" PRIxPTR ", %u) = %#" PRIxPTR, (uintptr_t)ptr, (unsigned)size, (uintptr_t)p);
    return p;
}

void loc_free(const void * p) {
    trace(LOG_ALLOC, "loc_free %#" PRIxPTR, (uintptr_t)p);
    free((void *)p);
}

#endif /* USE_libc_malloc */

/* strdup() with end-of-memory checking. */
char * loc_strdup(const char * s) {
    size_t len = strlen(s) + 1;
    return (char *)memcpy(loc_alloc(len), s, len);
}

/* strdup2() with concatenation and  end-of-memory checking. */
char * loc_strdup2(const char * s1, const char * s2) {
    size_t l1 = strlen(s1);
    size_t l2 = strlen(s2);
    char * rval = (char *)loc_alloc(l1 + l2 + 1);
    memcpy(rval, s1, l1);
    memcpy(rval + l1, s2, l2 + 1);
    return rval;
}

/* strndup() with end-of-memory checking. */
char * loc_strndup(const char * s, size_t len) {
    char * rval = (char *)loc_alloc(len + 1);
    rval[len] = '\0';
    return strncpy(rval, s, len);
}

char * loc_printf(const char * fmt, ...) {
    va_list ap;
    char * buf = NULL;
    va_start(ap, fmt);
    buf = loc_vprintf(fmt, ap);
    va_end(ap);
    return buf;
}

char * loc_vprintf(const char * fmt, va_list ap) {
    char arr[0x100];
    void * mem = NULL;
    char * buf = arr;
    size_t len = sizeof(arr);
    int n;

    while (1) {
        n = vsnprintf(buf, len, fmt, ap);
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
    if (buf == arr) buf = loc_strdup(arr);
    return buf;
}
