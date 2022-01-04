/*******************************************************************************
 * Copyright (c) 2007, 2016 Wind River Systems, Inc. and others.
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
 * This module handles process/thread OS contexts and their state machine.
 */

#include <tcf/config.h>

#include <assert.h>
#include <tcf/framework/context.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>

typedef struct Listener {
    ContextEventListener * func;
    void * args;
} Listener;

static Listener * listeners = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;

static size_t extension_size = 0;
static int context_created = 0;

LINK context_root = TCF_LIST_INIT(context_root);

const char * REASON_USER_REQUEST = "Suspended";
const char * REASON_STEP = "Step";
const char * REASON_ACTIVE = "Active";
const char * REASON_BREAKPOINT = "Breakpoint";
const char * REASON_EXCEPTION = "Exception";
const char * REASON_CONTAINER = "Container";
const char * REASON_WATCHPOINT = "Watchpoint";
const char * REASON_SIGNAL = "Signal";
const char * REASON_SHAREDLIB = "Shared Library";
const char * REASON_ERROR = "Error";

char * pid2id(pid_t pid, pid_t parent) {
    static char s[64];
    char * p = s + sizeof(s);
    unsigned long n = (long)pid;
    *(--p) = 0;
    do {
        *(--p) = (char)(n % 10 + '0');
        n = n / 10;
    }
    while (n != 0);
    if (parent != 0) {
        n = (long)parent;
        *(--p) = '.';
        do {
            *(--p) = (char)(n % 10 + '0');
            n = n / 10;
        }
        while (n != 0);
    }
    *(--p) = 'P';
    return p;
}

pid_t id2pid(const char * id, pid_t * parent) {
    /* TODO: (pid_t)0 is valid value in Windows, should use (pid_t)-1 to indicate an error */
    pid_t pid = 0;
    if (parent != NULL) *parent = 0;
    if (id == NULL) return 0;
    if (*id++ != 'P') return 0;
    while (*id >= '0' && *id <= '9') {
        pid = pid * 10 + (*id++ - '0');
    }
    if (*id == '.') {
        if (parent != NULL) *parent = pid;
        id++;
        pid = 0;
        while (*id >= '0' && *id <= '9') {
            pid = pid * 10 + (*id++ - '0');
        }
    }
    if (*id != 0) return 0;
    return pid;
}

void add_context_event_listener(ContextEventListener * listener, void * client_data) {
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listeners = (Listener *)loc_realloc(listeners, listener_max * sizeof(Listener));
    }
    listeners[listener_cnt].func = listener;
    listeners[listener_cnt].args = client_data;
    listener_cnt++;
}

size_t context_extension(size_t size) {
    size_t offs;
    assert(!context_created);
    while (extension_size % sizeof(void *) != 0) extension_size++;
    offs = sizeof(Context) + extension_size;
    extension_size += size;
    return offs;
}

Context * create_context(const char * id) {
    Context * ctx = (Context *)loc_alloc_zero(sizeof(Context) + extension_size);

    strlcpy(ctx->id, id, sizeof(ctx->id));
    list_init(&ctx->children);
    context_created = 1;
    return ctx;
}

void context_clear_memory_map(MemoryMap * map) {
    unsigned i;
    for (i = 0; i < map->region_cnt; i++) {
        MemoryRegion * r = map->regions + i;
        loc_free(r->file_name);
        loc_free(r->sect_name);
        loc_free(r->query);
        loc_free(r->id);
        while (r->attrs != NULL) {
            MemoryRegionAttribute * x = r->attrs;
            r->attrs = x->next;
            loc_free(x->name);
            loc_free(x->value);
            loc_free(x);
        }
    }
    memset(map->regions, 0, sizeof(MemoryRegion) * map->region_max);
    map->region_cnt = 0;
}

#if ENABLE_DebugContext

static char * buf = NULL;
static size_t buf_pos = 0;
static size_t buf_max = 0;

#if ENABLE_ContextIdHashTable

#define CONTEXT_ID_HASH_SIZE  (32 * MEM_USAGE_FACTOR - 1)
static LINK context_id_hash[CONTEXT_ID_HASH_SIZE];
static size_t context_extension_offset = 0;

#define ctx2idlink(ctx) ((LINK *)((char *)(ctx) + context_extension_offset))
#define idlink2ctx(lnk) ((Context *)((char *)(lnk) - context_extension_offset))

static unsigned id2hash(const char * id) {
    unsigned hash = 0;
    while (*id) hash = (hash >> 16) + hash + (unsigned char)*id++;
    return hash % CONTEXT_ID_HASH_SIZE;
}

Context * id2ctx(const char * id) {
    LINK * h = context_id_hash + id2hash(id);
    LINK * l = h->next;
    if (l == NULL) return NULL;
    while (l != h) {
        Context * ctx = idlink2ctx(l);
        if (strcmp(ctx->id, id) == 0) return ctx;
        l = l->next;
    }
    return NULL;
}

#endif

static void buf_char(char ch) {
    if (buf_pos >= buf_max) {
        buf_max += 0x100;
        buf = (char *)loc_realloc(buf, buf_max);
    }
    buf[buf_pos++] = ch;
}

static void get_context_full_name(Context * ctx) {
    if (ctx != NULL) {
        char * name = ctx->name;
        get_context_full_name(ctx->parent);
        buf_char('/');
        if (name != NULL) {
            int quote = strchr(name, '/') != NULL;
            if (quote) buf_char('"');
            while (*name) buf_char(*name++);
            if (quote) buf_char('"');
        }
    }
}

const char * context_full_name(Context * ctx) {
    buf_pos = 0;
    get_context_full_name(ctx);
    buf_char(0);
    return buf;
}

void context_lock(Context * ctx) {
    assert(ctx->ref_count > 0);
    ctx->ref_count++;
}

static void dispose_ctx(void * args) {
    Context * ctx = (Context *)args;
    unsigned i;

    assert(ctx->exited);
    assert(ctx->ref_count == 0);
    assert(list_is_empty(&ctx->children));
    if (ctx->parent != NULL) {
        list_remove(&ctx->cldl);
        context_unlock(ctx->parent);
        ctx->parent = NULL;
    }
    if (ctx->creator != NULL) {
        context_unlock(ctx->creator);
        ctx->creator = NULL;
    }

#if ENABLE_ContextIdHashTable
    if (!list_is_empty(ctx2idlink(ctx))) list_remove(ctx2idlink(ctx));
#endif

    assert(!ctx->event_notification);
    ctx->event_notification = 1;
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->context_disposed == NULL) continue;
        l->func->context_disposed(ctx, l->args);
    }
    ctx->event_notification = 0;
    assert(ctx->ref_count == 0);
    list_remove(&ctx->ctxl);
    sigset_clear(&ctx->pending_signals);
    sigset_clear(&ctx->sig_dont_stop);
    sigset_clear(&ctx->sig_dont_pass);
    loc_free(ctx->name);
    loc_free(ctx);
}

void context_unlock(Context * ctx) {
    assert(ctx->ref_count > 0);
    if (--(ctx->ref_count) == 0) {
        assert(ctx->exited);
        post_event(dispose_ctx, ctx);
    }
}

const char * context_state_name(Context * ctx) {
    if (ctx->exited) return "exited";
    if (ctx->stopped) return "stopped";
    return "running";
}

void send_context_created_event(Context * ctx) {
    unsigned i;
    assert(ctx->ref_count > 0);
    assert(!ctx->event_notification);
#if ENABLE_ContextIdHashTable
    if (list_is_empty(ctx2idlink(ctx))) list_add_last(ctx2idlink(ctx), context_id_hash + id2hash(ctx->id));
#endif
    ctx->event_notification = 1;
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->context_created == NULL) continue;
        l->func->context_created(ctx, l->args);
    }
    ctx->event_notification = 0;
}

void send_context_changed_event(Context * ctx) {
    unsigned i;
    assert(ctx->ref_count > 0);
    assert(!ctx->event_notification);
    ctx->event_notification = 1;
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->context_changed == NULL) continue;
        l->func->context_changed(ctx, l->args);
    }
    ctx->event_notification = 0;
}

void send_context_stopped_event(Context * ctx) {
    unsigned i;
    assert(ctx->ref_count > 0);
    assert(ctx->stopped != 0);
    assert(!ctx->event_notification);
    assert(context_has_state(ctx));
    ctx->event_notification = 1;
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->context_stopped == NULL) continue;
        l->func->context_stopped(ctx, l->args);
        assert(ctx->stopped != 0);
    }
    ctx->event_notification = 0;
}

void send_context_started_event(Context * ctx) {
    unsigned i;
    assert(ctx->ref_count > 0);
    assert(context_has_state(ctx));
    ctx->advanced = 0;
    ctx->stopped = 0;
    ctx->stopped_by_bp = 0;
    ctx->stopped_by_cb = NULL;
    ctx->stopped_by_exception = 0;
    ctx->stopped_by_funccall = 0;
    if (ctx->exception_description) {
        loc_free(ctx->exception_description);
        ctx->exception_description = NULL;
    }
    ctx->event_notification++;
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->context_started == NULL) continue;
        l->func->context_started(ctx, l->args);
    }
    ctx->event_notification--;
}

void send_context_exited_event(Context * ctx) {
    unsigned i;
    assert(!ctx->event_notification);
    assert(!ctx->exited);
    ctx->exiting = 0;
    ctx->pending_intercept = 0;
    ctx->exited = 1;
    ctx->event_notification = 1;
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->func->context_exited == NULL) continue;
        l->func->context_exited(ctx, l->args);
    }
    ctx->event_notification = 0;
#if ENABLE_ContextIdHashTable
    if (!list_is_empty(ctx2idlink(ctx))) list_remove(ctx2idlink(ctx));
#endif
    context_unlock(ctx);
}

#if ENABLE_ContextIdHashTable
void add_context_to_id_hash_table(Context * ctx) {
    if (list_is_empty(ctx2idlink(ctx))) list_add_last(ctx2idlink(ctx), context_id_hash + id2hash(ctx->id));
}
#endif

void ini_contexts(void) {
#if ENABLE_ContextIdHashTable
    {
        unsigned i;
        context_extension_offset = context_extension(sizeof(LINK));
        for (i = 0; i < CONTEXT_ID_HASH_SIZE; i++) list_init(context_id_hash + i);
    }
#endif
    ini_cpudefs();
    init_contexts_sys_dep();
}

#endif  /* if ENABLE_DebugContext */
