/*******************************************************************************
 * Copyright (c) 2011, 2017 Wind River Systems, Inc. and others.
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
 * TCF service line Numbers - proxy version.
 *
 * The service associates locations in the source files with the corresponding
 * machine instruction addresses in the executable object.
 */

#include <tcf/config.h>

#if ENABLE_LineNumbersProxy

#include <assert.h>
#include <stdio.h>
#include <tcf/framework/context.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/json.h>
#include <tcf/framework/events.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/linenumbers.h>
#if ENABLE_LineNumbersMux
#define LINENUMBERS_READER_PREFIX proxy_reader_
#include <tcf/services/linenumbers_mux.h>
#endif

#define HASH_SIZE (16 * MEM_USAGE_FACTOR - 1)

/* Line numbers cache, one per channel */
typedef struct LineNumbersCache {
    unsigned magic;
    Channel * channel;
    LINK link_root;
    LINK link_entries[HASH_SIZE];
    int service_available;
} LineNumbersCache;

/* Cache entry */
typedef struct LineNumbersCacheEntry {
    unsigned magic;
    LINK link_cache;
    AbstractCache cache;
    Context * ctx;
    char * file;
    int line;
    int column;
    ContextAddress addr0;
    ContextAddress addr1;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    unsigned areas_cnt;
    CodeArea * areas;
    int disposed;
} LineNumbersCacheEntry;

#define LINE_NUMBERS_CACHE_MAGIC 0x19873654

#define root2cache(A) ((LineNumbersCache *)((char *)(A) - offsetof(LineNumbersCache, link_root)))
#define cache2entry(A) ((LineNumbersCacheEntry *)((char *)(A) - offsetof(LineNumbersCacheEntry, link_cache)))

static const char * LINENUMBERS = "LineNumbers";

static LINK root = TCF_LIST_INIT(root);

static int code_area_cnt = 0;
static int code_area_max = 0;
static CodeArea * code_area_buf = NULL;

static void free_cache_entry(LineNumbersCacheEntry * cache) {
    assert(cache->magic == LINE_NUMBERS_CACHE_MAGIC);
    list_remove(&cache->link_cache);
    cache->disposed = 1;
    if (cache->pending == NULL) {
        unsigned i;
        cache->magic = 0;
        cache_dispose(&cache->cache);
        release_error_report(cache->error);
        context_unlock(cache->ctx);
        for (i = 0; i < cache->areas_cnt; i++) {
            CodeArea * area = cache->areas + i;
            loc_free(area->file);
            loc_free(area->directory);
        }
        loc_free(cache->areas);
        loc_free(cache->file);
        loc_free(cache);
    }
}

static void free_line_numbers_cache(LineNumbersCache * cache) {
    unsigned i;
    assert(cache->magic == LINE_NUMBERS_CACHE_MAGIC);
    cache->magic = 0;
    for (i = 0; i < HASH_SIZE; i++) {
        while (!list_is_empty(cache->link_entries + i)) {
            free_cache_entry(cache2entry(cache->link_entries[i].next));
        }
    }
    channel_unlock_with_msg(cache->channel, LINENUMBERS);
    list_remove(&cache->link_root);
    loc_free(cache);
}

static LineNumbersCache * get_line_numbers_cache(void) {
    LINK * l = NULL;
    LineNumbersCache * cache = NULL;
    Channel * c = cache_channel();
    if (c == NULL) str_exception(ERR_OTHER, "Line numbers cache: illegal cache access");
    if (is_channel_closed(c)) exception(ERR_CHANNEL_CLOSED);
    for (l = root.next; l != &root; l = l->next) {
        LineNumbersCache * x = root2cache(l);
        if (x->channel == c) {
            cache = x;
            break;
        }
    }
    if (cache == NULL) {
        int i = 0;
        cache = (LineNumbersCache *)loc_alloc_zero(sizeof(LineNumbersCache));
        cache->magic = LINE_NUMBERS_CACHE_MAGIC;
        cache->channel = c;
        list_add_first(&cache->link_root, &root);
        for (i = 0; i < HASH_SIZE; i++) {
            list_init(cache->link_entries + i);
        }
        channel_lock_with_msg(c, LINENUMBERS);
        for (i = 0; i < c->peer_service_cnt; i++) {
            if (strcmp(c->peer_service_list[i], LINENUMBERS) == 0) cache->service_available = 1;
        }
    }
    return cache;
}

static unsigned calc_hash(Context * ctx, const char * file, int line, int column, ContextAddress addr) {
    unsigned h = (unsigned)addr;
    if (file) {
        unsigned i;
        for (i = 0; file[i]; i++) h += file[i];
    }
    return (h + ((uintptr_t)ctx >> 4) + (unsigned)line + (unsigned)column) % HASH_SIZE;
}

static void read_code_area_array(InputStream * inp, void * args) {
    CodeArea * area = NULL;
    if (code_area_cnt >= code_area_max) {
        code_area_max += 8;
        code_area_buf = (CodeArea *)loc_realloc(code_area_buf, sizeof(CodeArea) * code_area_max);
    }
    area = code_area_buf + code_area_cnt++;
    read_code_area(inp, area);
}

static void validate_cache_entry(Channel * c, void * args, int error) {
    Trap trap;
    LineNumbersCacheEntry * entry = (LineNumbersCacheEntry *)args;
    assert(entry->magic == LINE_NUMBERS_CACHE_MAGIC);
    assert(entry->pending != NULL);
    assert(entry->error == NULL);
    if (set_trap(&trap)) {
        entry->pending = NULL;
        if (!error) {
            error = read_errno(&c->inp);
            code_area_cnt = 0;
            json_read_array(&c->inp, read_code_area_array, NULL);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            if (code_area_cnt > 0) {
                entry->areas_cnt = code_area_cnt;
                entry->areas = (CodeArea *)loc_alloc(sizeof(CodeArea) * code_area_cnt);
                memcpy(entry->areas, code_area_buf, sizeof(CodeArea) * code_area_cnt);
            }
        }
        clear_trap(&trap);
    }
    else {
        error = trap.error;
    }
    entry->error = get_error_report(error);
    cache_notify_later(&entry->cache);
    if (entry->disposed) free_cache_entry(entry);
}

int line_to_address(Context * ctx, const char * file, int line, int column,
                    LineNumbersCallBack * client, void * args) {
    LINK * l = NULL;
    LineNumbersCache * cache = NULL;
    LineNumbersCacheEntry * entry = NULL;
    unsigned h;
    Trap trap;

    if (!set_trap(&trap)) return -1;

    ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
    h = calc_hash(ctx, file, line, column, 0);
    cache = get_line_numbers_cache();
    assert(cache->magic == LINE_NUMBERS_CACHE_MAGIC);
    if (!cache->service_available) {
        clear_trap(&trap);
        return 0;
    }

    for (l = cache->link_entries[h].next; l != cache->link_entries + h; l = l->next) {
        LineNumbersCacheEntry * c = cache2entry(l);
        if (c->ctx == ctx && c->line == line && c->column == column && c->file && strcmp(c->file, file) == 0) {
            assert(c->magic == LINE_NUMBERS_CACHE_MAGIC);
            entry = c;
            break;
        }
    }

    if (entry == NULL) {
        Channel * c = cache->channel;
        entry = (LineNumbersCacheEntry *)loc_alloc_zero(sizeof(LineNumbersCacheEntry));
        list_add_first(&entry->link_cache, cache->link_entries + h);
        entry->magic = LINE_NUMBERS_CACHE_MAGIC;
        context_lock(entry->ctx = ctx);
        entry->file = loc_strdup(file);
        entry->line = line;
        entry->column = column;
        entry->pending = protocol_send_command(c, "LineNumbers", "mapToMemory", validate_cache_entry, entry);
        json_write_string(&c->out, ctx->id);
        write_stream(&c->out, 0);
        json_write_string(&c->out, file);
        write_stream(&c->out, 0);
        json_write_long(&c->out, line);
        write_stream(&c->out, 0);
        json_write_long(&c->out, column);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&entry->cache);
    }
    else if (entry->pending != NULL) {
        cache_wait(&entry->cache);
    }
    else if (entry->error != NULL) {
        exception(set_fmt_errno(set_error_report_errno(entry->error),
            "Text position '%s:%d' not found", file, line));
    }
    else {
        unsigned i;
        for (i = 0; i < entry->areas_cnt; i++) {
            client(entry->areas + i, args);
        }
    }
    clear_trap(&trap);
    return 0;
}

int address_to_line(Context * ctx, ContextAddress addr0, ContextAddress addr1, LineNumbersCallBack * client, void * args) {
    LINK * l = NULL;
    LineNumbersCache * cache = NULL;
    LineNumbersCacheEntry * entry = NULL;
    unsigned h;
    Trap trap;

    if (cache_channel() == NULL) {
        /* This is needed for run control to work without a client connected */
        return 0;
    }

    if (!set_trap(&trap)) return -1;

    ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
    h = calc_hash(ctx, NULL, 0, 0, addr0);
    cache = get_line_numbers_cache();
    assert(cache->magic == LINE_NUMBERS_CACHE_MAGIC);
    if (!cache->service_available) {
        clear_trap(&trap);
        return 0;
    }

    for (l = cache->link_entries[h].next; l != cache->link_entries + h; l = l->next) {
        LineNumbersCacheEntry * c = cache2entry(l);
        if (c->ctx == ctx && c->file == NULL && c->addr0 == addr0 && c->addr1 == addr1) {
            assert(c->magic == LINE_NUMBERS_CACHE_MAGIC);
            entry = c;
            break;
        }
    }

    if (entry == NULL) {
        Channel * c = cache->channel;
        entry = (LineNumbersCacheEntry *)loc_alloc_zero(sizeof(LineNumbersCacheEntry));
        list_add_first(&entry->link_cache, cache->link_entries + h);
        entry->magic = LINE_NUMBERS_CACHE_MAGIC;
        context_lock(entry->ctx = ctx);
        entry->addr0 = addr0;
        entry->addr1 = addr1;
        entry->pending = protocol_send_command(c, "LineNumbers", "mapToSource", validate_cache_entry, entry);
        json_write_string(&c->out, ctx->id);
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, addr0);
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, addr1);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&entry->cache);
    }
    else if (entry->pending != NULL) {
        cache_wait(&entry->cache);
    }
    else if (entry->error != NULL) {
        exception(set_fmt_errno(set_error_report_errno(entry->error),
            "Text position not found for address %#" PRIx64 "..%#" PRIx64, (uint64_t)addr0, (uint64_t)addr1));
    }
    else {
        unsigned i;
        for (i = 0; i < entry->areas_cnt; i++) {
            client(entry->areas + i, args);
        }
    }
    clear_trap(&trap);
    return 0;
}

static void flush_cache(Context * ctx) {
    LINK * l;
    LINK * m;
    int i;

    for (m = root.next; m != &root; m = m->next) {
        LineNumbersCache * cache = root2cache(m);
        for (i = 0; i < HASH_SIZE; i++) {
            l = cache->link_entries[i].next;
            while (l != cache->link_entries + i) {
                LineNumbersCacheEntry * c = cache2entry(l);
                l = l->next;
                if (c->ctx == ctx) free_cache_entry(c);
            }
        }
    }
}

static void event_context_created(Context * ctx, void * x) {
    if (ctx == context_get_group(ctx, CONTEXT_GROUP_SYMBOLS)) flush_cache(ctx);
}

static void event_context_exited(Context * ctx, void * x) {
    if (ctx == context_get_group(ctx, CONTEXT_GROUP_SYMBOLS)) flush_cache(ctx);
}

static void event_context_changed(Context * ctx, void * x) {
    flush_cache(context_get_group(ctx, CONTEXT_GROUP_SYMBOLS));
}

#if SERVICE_MemoryMap
static void event_code_unmapped(Context * ctx, ContextAddress addr, ContextAddress size, void * x) {
    event_context_changed(ctx, x);
}
#endif

static void channel_close_listener(Channel * c) {
    LINK * l = root.next;
    while (l != &root) {
        LineNumbersCache * cache = root2cache(l);
        l = l->next;
        if (cache->channel == c) free_line_numbers_cache(cache);
    }
}

void ini_line_numbers_lib(void) {
    {
        static ContextEventListener listener = {
            event_context_created,
            event_context_exited,
            NULL,
            NULL,
            event_context_changed
        };
        add_context_event_listener(&listener, NULL);
    }
#if SERVICE_MemoryMap
    {
        static MemoryMapEventListener listener = {
            event_context_changed,
            event_code_unmapped,
            event_context_changed,
            event_context_changed,
        };
        add_memory_map_event_listener(&listener, NULL);
    }
#endif
    add_channel_close_listener(channel_close_listener);
#if ENABLE_LineNumbersMux
    add_line_numbers_reader(&line_numbers_reader);
#endif
}

#endif /* ENABLE_LineNumbersProxy */
