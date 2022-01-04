/*******************************************************************************
 * Copyright (c) 2007-2019 Wind River Systems, Inc. and others.
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
 * Symbols service - proxy implementation, gets symbols information from host.
 */

#include <tcf/config.h>

#if ENABLE_SymbolsProxy

#include <assert.h>
#include <stdio.h>
#include <tcf/framework/context.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/json.h>
#include <tcf/framework/events.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/symbols.h>
#include <tcf/services/vm.h>
#if ENABLE_SymbolsMux
#define SYM_READER_PREFIX proxy_reader_
#include <tcf/services/symbols_mux.h>
#endif

#define HASH_SIZE (4 * MEM_USAGE_FACTOR - 1)

#define ACC_SIZE     1
#define ACC_BOUNDS   2
#define ACC_OTHER    3

#ifndef SYMBOLS_PROXY_CLEANUP_DELAY
#define SYMBOLS_PROXY_CLEANUP_DELAY 1000000
#endif

#define SYMBOLS_CACHE_THRESHOLD (MEM_USAGE_FACTOR * 32)

/* Symbols cache, one per channel */
typedef struct SymbolsCache {
    Channel * channel;
    LINK link_root;
    LINK link_sym[HASH_SIZE];
    LINK link_find_by_name[HASH_SIZE];
    LINK link_find_by_addr[HASH_SIZE];
    LINK link_find_in_scope[HASH_SIZE];
    LINK link_list[HASH_SIZE];
    LINK link_file[HASH_SIZE];
    LINK link_frame[HASH_SIZE];
    LINK link_address[HASH_SIZE];
    LINK link_location[HASH_SIZE];
    int service_available;
    int no_find_frame_info;
    int no_find_frame_props;
} SymbolsCache;

/* Symbol properties cache */
typedef struct SymInfoCache {
    unsigned magic;
    LINK link_syms;
    LINK link_flush;
    AbstractCache cache;
    char * id;
    char * type_id;
    char * base_type_id;
    char * index_type_id;
    char * container_id;
    char * name;
    Context * update_owner;
    int update_policy;
    int degraded;
    int sym_class;
    int type_class;
    int has_size;
    int has_length;
    int has_lower_bound;
    int frame;
    SYM_FLAGS flags;
    SymbolProperties props;
    ContextAddress size;
    ContextAddress length;
    int64_t lower_bound;
    char ** children_ids;
    int children_count;
    ReplyHandlerInfo * pending_get_context;
    ReplyHandlerInfo * pending_get_children;
    ErrorReport * error_get_context;
    ErrorReport * error_get_children;
    int done_context;
    int done_children;
    LINK array_syms;
    int disposed;
} SymInfoCache;

/* Cached result of get_array_symbol() */
typedef struct ArraySymCache {
    unsigned magic;
    LINK link_sym;
    AbstractCache cache;
    ContextAddress length;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    char * id;
    int disposed;
} ArraySymCache;

/* Cached result of find_symbol_by_name(), find_symbol_in_scope(), find_symbol_by_addr(), enumerate_symbols() */
typedef struct FindSymCache {
    unsigned magic;
    LINK link_syms;
    LINK link_flush;
    AbstractCache cache;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    int update_policy;
    Context * ctx;
    int frame;
    uint64_t ip;
    uint64_t addr;
    char * scope;
    char * name;
    char ** id_buf;
    int id_cnt;
    int disposed;
} FindSymCache;

typedef struct StackFrameCache {
    unsigned magic;
    LINK link_syms;
    LINK link_flush;
    AbstractCache cache;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    Context * ctx;
    uint64_t ip;
    StackTracingInfo sti;
    int command_props;
    int disposed;
} StackFrameCache;

typedef struct AddressInfoCache {
    unsigned magic;
    LINK link_syms;
    LINK link_flush;
    AbstractCache cache;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    Context * ctx;
    ContextAddress addr;

    const char * isa;
    ContextAddress range_addr;
    ContextAddress range_size;
    ContextAddress plt;

    int disposed;
} AddressInfoCache;

typedef struct FileInfoCache {
    unsigned magic;
    LINK link_syms;
    LINK link_flush;
    AbstractCache cache;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    Context * ctx;
    ContextAddress addr;

    SymbolFileInfo info;
    ErrorReport * file_error;

    int disposed;
} FileInfoCache;

typedef struct LocationInfoCache {
    unsigned magic;
    LINK link_syms;
    LINK link_flush;
    AbstractCache cache;
    ReplyHandlerInfo * pending;
    ErrorReport * error;
    char * sym_id;
    Context * ctx;
    uint64_t ip;

    LocationInfo info;

    int disposed;
} LocationInfoCache;

#define root2syms(A) ((SymbolsCache *)((char *)(A) - offsetof(SymbolsCache, link_root)))
#define syms2sym(A)  ((SymInfoCache *)((char *)(A) - offsetof(SymInfoCache, link_syms)))
#define syms2find(A) ((FindSymCache *)((char *)(A) - offsetof(FindSymCache, link_syms)))
#define syms2frame(A)((StackFrameCache *)((char *)(A) - offsetof(StackFrameCache, link_syms)))
#define syms2address(A)((AddressInfoCache *)((char *)(A) - offsetof(AddressInfoCache, link_syms)))
#define syms2file(A) ((FileInfoCache *)((char *)(A) - offsetof(FileInfoCache, link_syms)))
#define syms2location(A)((LocationInfoCache *)((char *)(A) - offsetof(LocationInfoCache, link_syms)))

#define sym2arr(A)   ((ArraySymCache *)((char *)(A) - offsetof(ArraySymCache, link_sym)))

#define flush2sym(A)  ((SymInfoCache *)((char *)(A) - offsetof(SymInfoCache, link_flush)))
#define flush2find(A) ((FindSymCache *)((char *)(A) - offsetof(FindSymCache, link_flush)))
#define flush2frame(A)((StackFrameCache *)((char *)(A) - offsetof(StackFrameCache, link_flush)))
#define flush2address(A)((AddressInfoCache *)((char *)(A) - offsetof(AddressInfoCache, link_flush)))
#define flush2file(A) ((FileInfoCache *)((char *)(A) - offsetof(FileInfoCache, link_flush)))
#define flush2location(A)((LocationInfoCache *)((char *)(A) - offsetof(LocationInfoCache, link_flush)))

struct Symbol {
#if ENABLE_SymbolsMux
    SymbolReader * reader;
#endif
    unsigned magic;
    SymInfoCache * cache;
};

static LINK root = TCF_LIST_INIT(root);

static char ** find_next_buf = NULL;
static int find_next_pos = 0;
static int find_next_cnt = 0;
static int symbols_cleanup_posted = 0;
static int symbols_cleanup_delayed = 0;

static LINK flush_rc;
static LINK flush_mm;

static const char * SYMBOLS = "Symbols";

#define MAGIC_SYMBOL    0x34875234
#define MAGIC_INFO      0x38254865
#define MAGIC_ARRAY     0x92745446
#define MAGIC_FIND      0x89058765
#define MAGIC_FRAME     0x10837608
#define MAGIC_ADDR      0x28658765
#define MAGIC_FILE      0x87653487
#define MAGIC_LOC       0x09878751

static void flush_symbol(LINK * l);

static void clean_flush_list(LINK * list) {
    if (!list_is_empty(list)) {
        LINK * l;
        unsigned list_count = 0;
        unsigned flush_count = 1;

        list_foreach(l, list) list_count++;
        /* drain faster if we have reached the cache threshold */
        if (list_count > SYMBOLS_CACHE_THRESHOLD) flush_count = (list_count - SYMBOLS_CACHE_THRESHOLD) / 2 + 1;

        l = list->next;
        while (flush_count-- > 0) {
            LINK * n = l;
            l = l->next;
            flush_symbol(n);
        }
    }
}

static void symbols_cleanup_event(void * arg) {
    assert(symbols_cleanup_posted);

    if (symbols_cleanup_delayed) {
        post_event_with_delay(symbols_cleanup_event, NULL, SYMBOLS_PROXY_CLEANUP_DELAY);
        symbols_cleanup_delayed = 0;
        return;
    }
    /* Flush the first entry of each cache */
    clean_flush_list(&flush_rc);
    clean_flush_list(&flush_mm);
    if (!list_is_empty(&flush_rc) || !list_is_empty(&flush_mm)) {
        post_event_with_delay(symbols_cleanup_event, NULL, SYMBOLS_PROXY_CLEANUP_DELAY);
    }
    else {
        symbols_cleanup_posted = 0;
    }
}

static Symbol * alloc_symbol(void) {
    Symbol * s = (Symbol *)tmp_alloc_zero(sizeof(Symbol));
#if ENABLE_SymbolsMux
    s->reader = &symbol_reader;
#endif
    s->magic = MAGIC_SYMBOL;
    return s;
}

static unsigned hash_sym_id(const char * id) {
    int i;
    unsigned h = 0;
    for (i = 0; id[i]; i++) h += id[i];
    return h % HASH_SIZE;
}

static unsigned hash_find(Context * ctx, const char * name, uint64_t ip) {
    int i;
    unsigned h = 0;
    if (name != NULL) for (i = 0; name[i]; i++) h += name[i];
    return (h + ((uintptr_t)ctx >> 4) + (unsigned)ip) % HASH_SIZE;
}

static unsigned hash_list(Context * ctx, uint64_t ip) {
    return (((uintptr_t)ctx >> 4) + (unsigned)ip) % HASH_SIZE;
}

static unsigned hash_frame(Context * ctx) {
    return ((uintptr_t)ctx >> 4) % HASH_SIZE;
}

static unsigned hash_address(Context * ctx) {
    return ((uintptr_t)ctx >> 4) % HASH_SIZE;
}

static unsigned hash_file(Context * ctx) {
    return ((uintptr_t)ctx >> 4) % HASH_SIZE;
}

static SymbolsCache * get_symbols_cache(void) {
    LINK * l = NULL;
    SymbolsCache * syms = NULL;
    Channel * c = cache_channel();
    if (c == NULL) str_exception(ERR_OTHER, "Symbols cache: illegal cache access");
    if (is_channel_closed(c)) exception(ERR_CHANNEL_CLOSED);
    if (symbols_cleanup_posted) {
        /* There is some activity on the cache; let's delay the cache flush
         * to avoid flushing usefull entries.
         */
        symbols_cleanup_delayed = 1;
    }
    else {
        symbols_cleanup_posted = 1;
        post_event_with_delay(symbols_cleanup_event, NULL, SYMBOLS_PROXY_CLEANUP_DELAY);
    }
    for (l = root.next; l != &root; l = l->next) {
        SymbolsCache * x = root2syms(l);
        if (x->channel == c) {
            syms = x;
            break;
        }
    }
    if (syms == NULL) {
        int i = 0;
        syms = (SymbolsCache *)loc_alloc_zero(sizeof(SymbolsCache));
        syms->channel = c;
        list_add_first(&syms->link_root, &root);
        for (i = 0; i < HASH_SIZE; i++) {
            list_init(syms->link_sym + i);
            list_init(syms->link_find_by_name + i);
            list_init(syms->link_find_by_addr + i);
            list_init(syms->link_find_in_scope + i);
            list_init(syms->link_list + i);
            list_init(syms->link_file + i);
            list_init(syms->link_frame + i);
            list_init(syms->link_address + i);
            list_init(syms->link_location + i);
        }
        channel_lock_with_msg(c, SYMBOLS);
        for (i = 0; i < c->peer_service_cnt; i++) {
            if (strcmp(c->peer_service_list[i], SYMBOLS) == 0) syms->service_available = 1;
        }
    }
    return syms;
}

static void free_arr_sym_cache(ArraySymCache * a) {
    assert(a->magic == MAGIC_ARRAY);
    assert(!a->disposed || a->pending == NULL);
    if (!a->disposed) {
        list_remove(&a->link_sym);
        a->disposed = 1;
    }
    if (a->pending == NULL) {
        a->magic = 0;
        cache_dispose(&a->cache);
        release_error_report(a->error);
        loc_free(a->id);
        loc_free(a);
    }
}

static void free_sym_info_cache(SymInfoCache * c) {
    assert(c->magic == MAGIC_INFO);
    assert(!c->disposed || (c->pending_get_context == NULL && c->pending_get_children == NULL));
    if (!c->disposed) {
        list_remove(&c->link_syms);
        list_remove(&c->link_flush);
        c->disposed = 1;
    }
    if (c->pending_get_context == NULL && c->pending_get_children == NULL) {
        c->magic = 0;
        cache_dispose(&c->cache);
        loc_free(c->id);
        loc_free(c->type_id);
        loc_free(c->base_type_id);
        loc_free(c->index_type_id);
        loc_free(c->container_id);
        loc_free(c->name);
        loc_free(c->children_ids);
        loc_free(c->props.linkage_name);
        if (c->update_owner != NULL) context_unlock(c->update_owner);
        release_error_report(c->error_get_context);
        release_error_report(c->error_get_children);
        while (!list_is_empty(&c->array_syms)) {
            free_arr_sym_cache(sym2arr(c->array_syms.next));
        }
        loc_free(c);
    }
}

static void free_find_sym_cache(FindSymCache * c) {
    assert(c->magic == MAGIC_FIND);
    assert(!c->disposed || c->pending == NULL);
    if (!c->disposed) {
        list_remove(&c->link_syms);
        list_remove(&c->link_flush);
        c->disposed = 1;
    }
    if (c->pending == NULL) {
        c->magic = 0;
        if (find_next_buf == c->id_buf) {
            find_next_buf = NULL;
            find_next_pos = 0;
            find_next_cnt = 0;
        }
        cache_dispose(&c->cache);
        release_error_report(c->error);
        context_unlock(c->ctx);
        loc_free(c->scope);
        loc_free(c->name);
        loc_free(c->id_buf);
        loc_free(c);
    }
}

static void free_location_command_args(LocationExpressionCommand * cmd) {
    if (cmd->cmd == SFT_CMD_LOCATION) loc_free(cmd->args.loc.code_addr);
    else if (cmd->cmd == SFT_CMD_PIECE) loc_free(cmd->args.piece.value);
}

static void free_location_commands(LocationCommands * cmds) {
    unsigned i = 0;
    while (i < cmds->cnt) free_location_command_args(cmds->cmds + i++);
    loc_free(cmds->cmds);
}

static void free_sft_sequence(StackFrameRegisterLocation * seq) {
    if (seq != NULL) {
        unsigned i = 0;
        while (i < seq->cmds_cnt) free_location_command_args(seq->cmds + i++);
        loc_free(seq);
    }
}

static void free_stack_frame_cache(StackFrameCache * c) {
    assert(c->magic == MAGIC_FRAME);
    assert(!c->disposed || c->pending == NULL);
    if (!c->disposed) {
        list_remove(&c->link_syms);
        list_remove(&c->link_flush);
        c->disposed = 1;
    }
    if (c->pending == NULL) {
        int i;
        c->magic = 0;
        cache_dispose(&c->cache);
        release_error_report(c->error);
        context_unlock(c->ctx);
        for (i = 0; i < c->sti.reg_cnt; i++) free_sft_sequence(c->sti.regs[i]);
        free_sft_sequence(c->sti.fp);
        for (i = 0; i < c->sti.sub_cnt; i++) {
            StackFrameInlinedSubroutine * info = c->sti.subs[i];
            loc_free(info->area.directory);
            loc_free(info->area.file);
            loc_free(info->func_id);
            loc_free(info);
        }
        loc_free(c->sti.subs);
        loc_free(c->sti.regs);
        loc_free(c);
    }
}

static void free_address_info_cache(AddressInfoCache * c) {
    assert(c->magic == MAGIC_ADDR);
    assert(!c->disposed || c->pending == NULL);
    if (!c->disposed) {
        list_remove(&c->link_syms);
        list_remove(&c->link_flush);
        c->disposed = 1;
    }
    if (c->pending == NULL) {
        c->magic = 0;
        cache_dispose(&c->cache);
        release_error_report(c->error);
        context_unlock(c->ctx);
        loc_free(c->isa);
        loc_free(c);
    }
}

static void free_file_info_cache(FileInfoCache * c) {
    assert(c->magic == MAGIC_FILE);
    assert(!c->disposed || c->pending == NULL);
    if (!c->disposed) {
        list_remove(&c->link_syms);
        list_remove(&c->link_flush);
        c->disposed = 1;
    }
    if (c->pending == NULL) {
        c->magic = 0;
        cache_dispose(&c->cache);
        release_error_report(c->error);
        release_error_report(c->file_error);
        context_unlock(c->ctx);
        loc_free(c->info.file_name);
        loc_free(c);
    }
}

static void free_location_info_cache(LocationInfoCache * c) {
    assert(c->magic == MAGIC_LOC);
    assert(!c->disposed || c->pending == NULL);
    if (!c->disposed) {
        list_remove(&c->link_syms);
        list_remove(&c->link_flush);
        c->disposed = 1;
    }
    if (c->pending == NULL) {
        c->magic = 0;
        cache_dispose(&c->cache);
        release_error_report(c->error);
        context_unlock(c->ctx);
        loc_free(c->sym_id);
        free_location_commands(&c->info.value_cmds);
        loc_free(c->info.discr_lst);
        loc_free(c);
    }
}

static void free_symbols_cache(SymbolsCache * syms) {
    int i;
    for (i = 0; i < HASH_SIZE; i++) {
        while (!list_is_empty(syms->link_sym + i)) {
            free_sym_info_cache(syms2sym(syms->link_sym[i].next));
        }
        while (!list_is_empty(syms->link_find_by_name + i)) {
            free_find_sym_cache(syms2find(syms->link_find_by_name[i].next));
        }
        while (!list_is_empty(syms->link_find_by_addr + i)) {
            free_find_sym_cache(syms2find(syms->link_find_by_addr[i].next));
        }
        while (!list_is_empty(syms->link_find_in_scope + i)) {
            free_find_sym_cache(syms2find(syms->link_find_in_scope[i].next));
        }
        while (!list_is_empty(syms->link_list + i)) {
            free_find_sym_cache(syms2find(syms->link_list[i].next));
        }
        while (!list_is_empty(syms->link_file + i)) {
            free_file_info_cache(syms2file(syms->link_file[i].next));
        }
        while (!list_is_empty(syms->link_frame + i)) {
            free_stack_frame_cache(syms2frame(syms->link_frame[i].next));
        }
        while (!list_is_empty(syms->link_address + i)) {
            free_address_info_cache(syms2address(syms->link_address[i].next));
        }
        while (!list_is_empty(syms->link_location + i)) {
            free_location_info_cache(syms2location(syms->link_location[i].next));
        }
    }
    channel_unlock_with_msg(syms->channel, SYMBOLS);
    list_remove(&syms->link_root);
    loc_free(syms);
}

static void flush_symbol(LINK * l) {
    unsigned magic = flush2sym(l)->magic;
    if (magic == MAGIC_INFO) {
        SymInfoCache * c = flush2sym(l);
        if (c->cache.wait_list_cnt == 0) free_sym_info_cache(c);
    }
    else if (magic == MAGIC_FIND) {
        FindSymCache * c = flush2find(l);
        if (c->cache.wait_list_cnt == 0) free_find_sym_cache(c);
    }
    else if (magic == MAGIC_FRAME) {
        StackFrameCache * c = flush2frame(l);
        if (c->cache.wait_list_cnt == 0) free_stack_frame_cache(c);
    }
    else if (magic == MAGIC_ADDR) {
        AddressInfoCache * c = flush2address(l);
        if (c->cache.wait_list_cnt == 0) free_address_info_cache(c);
    }
    else if (magic == MAGIC_FILE) {
        FileInfoCache * c = flush2file(l);
        if (c->cache.wait_list_cnt == 0) free_file_info_cache(c);
    }
    else if (magic == MAGIC_LOC) {
        LocationInfoCache * c = flush2location(l);
        if (c->cache.wait_list_cnt == 0) free_location_info_cache(c);
    }
}

static Channel * get_channel(SymbolsCache * syms) {
    if (is_channel_closed(syms->channel)) str_exception(ERR_SYM_NOT_FOUND, "Channel is closed");
    if (!syms->service_available) str_exception(ERR_SYM_NOT_FOUND, "Symbols service not available");
    return syms->channel;
}

static uint64_t get_symbol_ip(Context * ctx, int * frame, ContextAddress addr) {
    uint64_t ip = 0;
    if (*frame == STACK_NO_FRAME) {
        ip = (uint64_t)addr;
    }
    else if (is_top_frame(ctx, *frame)) {
        ContextAddress pc = 0;
        if (!is_ctx_stopped(ctx)) exception(errno);
        *frame = get_top_frame(ctx);
        if (get_PC(ctx, &pc) < 0) exception(errno);
        ip = (uint64_t)pc;
    }
    else {
        StackFrame * info = NULL;
        if (get_frame_info(ctx, *frame, &info) < 0) exception(errno);
        if (read_reg_value(info, get_PC_definition(ctx), &ip) < 0) exception(errno);
        assert(!info->is_top_frame);
        *frame = info->frame;
        if (ip > 0) ip--;
    }
    return ip;
}

static void read_context_data(InputStream * inp, const char * name, void * args) {
    char id[256];
    SymInfoCache * s = (SymInfoCache *)args;
    if (strcmp(name, "ID") == 0) { json_read_string(inp, id, sizeof(id)); assert(strcmp(id, s->id) == 0); }
    else if (strcmp(name, "OwnerID") == 0) { json_read_string(inp, id, sizeof(id)); s->update_owner = id2ctx(id); }
    else if (strcmp(name, "Name") == 0) s->name = json_read_alloc_string(inp);
    else if (strcmp(name, "UpdatePolicy") == 0) s->update_policy = json_read_long(inp);
    else if (strcmp(name, "Class") == 0) s->sym_class = json_read_long(inp);
    else if (strcmp(name, "TypeClass") == 0) s->type_class = json_read_long(inp);
    else if (strcmp(name, "TypeID") == 0) s->type_id = json_read_alloc_string(inp);
    else if (strcmp(name, "BaseTypeID") == 0) s->base_type_id = json_read_alloc_string(inp);
    else if (strcmp(name, "IndexTypeID") == 0) s->index_type_id = json_read_alloc_string(inp);
    else if (strcmp(name, "ContainerID") == 0) s->container_id = json_read_alloc_string(inp);
    else if (strcmp(name, "Size") == 0) { s->size = json_read_long(inp); s->has_size = 1; }
    else if (strcmp(name, "Length") == 0) { s->length = json_read_long(inp); s->has_length = 1; }
    else if (strcmp(name, "LowerBound") == 0) { s->lower_bound = json_read_int64(inp); s->has_lower_bound = 1; }
    else if (strcmp(name, "BinaryScale") == 0) s->props.binary_scale = (int)json_read_long(inp);
    else if (strcmp(name, "DecimalScale") == 0) s->props.decimal_scale = (int)json_read_long(inp);
    else if (strcmp(name, "BitStride") == 0) s->props.bit_stride = (unsigned)json_read_ulong(inp);
    else if (strcmp(name, "LocalEntryOffset") == 0) s->props.local_entry_offset = (unsigned)json_read_ulong(inp);
    else if (strcmp(name, "LinkageName") == 0) s->props.linkage_name = json_read_alloc_string(inp);
    else if (strcmp(name, "Flags") == 0) s->flags = json_read_ulong(inp);
    else if (strcmp(name, "Frame") == 0) s->frame = (int)json_read_long(inp);
    else json_skip_object(inp);
}

static void validate_context(Channel * c, void * args, int error) {
    SymInfoCache * s = (SymInfoCache *)args;
    assert(s->magic == MAGIC_INFO);
    assert(s->pending_get_context != NULL);
    assert(s->error_get_context == NULL);
    assert(s->update_owner == NULL);
    assert(!s->done_context);
    assert(!s->degraded);
    s->pending_get_context = NULL;
    s->done_context = 1;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            error = read_errno(&c->inp);
            json_read_struct(&c->inp, read_context_data, s);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            if (!error && s->update_owner == NULL) error = ERR_INV_CONTEXT;
            if (!error && s->update_owner->exited) error = ERR_ALREADY_EXITED;
            if (!s->disposed && s->update_policy != UPDATE_ON_MEMORY_MAP_CHANGES) {
                list_remove(&s->link_flush);
                list_add_last(&s->link_flush, &flush_rc);
            }
            clear_trap(&trap);
        }
        else {
            error = trap.error;
            s->update_owner = NULL;
        }
    }
    if (s->update_owner != NULL) context_lock(s->update_owner);
    s->error_get_context = get_error_report(error);
    cache_notify_later(&s->cache);
    if (s->disposed) free_sym_info_cache(s);
    run_ctrl_unlock();
}

static SymInfoCache * get_sym_info_cache(const Symbol * sym, int acc_mode) {
    Trap trap;
    SymInfoCache * s = sym->cache;
    assert(sym->magic == MAGIC_SYMBOL);
    assert(s->magic == MAGIC_INFO);
    assert(s->id != NULL);
    if (!set_trap(&trap)) return NULL;
    if (s->pending_get_context != NULL) {
        cache_wait(&s->cache);
    }
    if (s->error_get_context != NULL) {
        exception(set_error_report_errno(s->error_get_context));
    }
    if (s->done_context && s->degraded) {
        /* Symbol info is partially outdated */
        int update = 0;
        assert(s->update_owner != NULL);
        assert(context_has_state(s->update_owner));
        switch (acc_mode) {
        case ACC_SIZE:
        case ACC_BOUNDS:
            if (s->type_class == TYPE_CLASS_ARRAY) update = 1;
            if (s->type_class == TYPE_CLASS_COMPOSITE) update = 1;
            if (s->type_class == TYPE_CLASS_UNKNOWN) update = 1;
            break;
        }
        if (update) {
            if (!is_ctx_stopped(s->update_owner)) exception(errno);
            s->degraded = 0;
            s->done_context = 0;
            s->has_size = 0;
            s->has_length = 0;
            s->has_lower_bound = 0;
            context_unlock(s->update_owner);
            loc_free(s->name);
            loc_free(s->type_id);
            loc_free(s->base_type_id);
            loc_free(s->index_type_id);
            loc_free(s->container_id);
            loc_free(s->props.linkage_name);
            s->update_owner = NULL;
            s->name = NULL;
            s->type_id = NULL;
            s->base_type_id = NULL;
            s->index_type_id = NULL;
            s->container_id = NULL;
            s->props.linkage_name = NULL;
        }
    }
    if (!s->done_context) {
        Channel * c = cache_channel();
        if (c == NULL || is_channel_closed(c)) exception(ERR_SYM_NOT_FOUND);
        run_ctrl_lock();
        s->pending_get_context = protocol_send_command(c, SYMBOLS, "getContext", validate_context, s);
        json_write_string(&c->out, s->id);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&s->cache);
    }
    clear_trap(&trap);
    return s;
}

static char ** string_to_symbol_list(char * id, int * cnt) {
    if (id[0]) {
        char ** buf = (char **)loc_alloc_zero(sizeof(char *) * 2 + strlen(id) + 1);
        buf[0] = (char *)(buf + 2);
        strcpy(buf[0], id);
        *cnt = 1;
        return buf;
    }
    *cnt = 0;
    return NULL;
}

static char ** read_symbol_list(InputStream * inp, int * id_cnt) {
    char id[256];
    if (json_peek(inp) == '[') return json_read_alloc_string_array(inp, id_cnt);
    json_read_string(inp, id, sizeof(id));
    return string_to_symbol_list(id, id_cnt);
}

static void validate_find(Channel * c, void * args, int error) {
    FindSymCache * f = (FindSymCache *)args;
    assert(f->magic == MAGIC_FIND);
    assert(f->pending != NULL);
    assert(f->error == NULL);
    f->pending = NULL;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            error = read_errno(&c->inp);
            f->id_buf = read_symbol_list(&c->inp, &f->id_cnt);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    f->error = get_error_report(error);
    cache_notify_later(&f->cache);
    if (f->disposed) free_find_sym_cache(f);
    run_ctrl_unlock();
}

int find_symbol_by_name(Context * ctx, int frame, ContextAddress addr, const char * name, Symbol ** sym) {
    uint64_t ip = 0;
    LINK * l = NULL;
    SymbolsCache * syms = NULL;
    FindSymCache * f = NULL;
    unsigned h;
    Trap trap;

    if (cache_channel() == NULL) {
        /* This is needed for eventpoints to work without a client connected */
        errno = ERR_SYM_NOT_FOUND;
        return -1;
    }

    if (!set_trap(&trap)) return -1;

    ip = get_symbol_ip(ctx, &frame, addr);
    h = hash_find(ctx, name, ip);
    syms = get_symbols_cache();
    for (l = syms->link_find_by_name[h].next; l != syms->link_find_by_name + h; l = l->next) {
        FindSymCache * c = syms2find(l);
        if (c->ctx == ctx && c->frame == frame && c->ip == ip && strcmp(c->name, name) == 0) {
            f = c;
            break;
        }
    }

    if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (FindSymCache *)loc_alloc_zero(sizeof(FindSymCache));
        list_add_first(&f->link_syms, syms->link_find_by_name + h);
        if (ip) {
            list_add_last(&f->link_flush, &flush_rc);
            f->update_policy = UPDATE_ON_EXE_STATE_CHANGES;
        }
        else {
            list_add_last(&f->link_flush, &flush_mm);
            f->update_policy = UPDATE_ON_MEMORY_MAP_CHANGES;
        }
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_FIND;
        f->frame = frame;
        f->ip = ip;
        f->name = loc_strdup(name);
        f->pending = protocol_send_command(c, SYMBOLS, "findByName", validate_find, f);
        if (frame != STACK_NO_FRAME) {
            json_write_string(&c->out, frame2id(ctx, frame));
        }
        else {
            json_write_string(&c->out, ctx->id);
        }
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, ip);
        write_stream(&c->out, 0);
        json_write_string(&c->out, name);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->pending != NULL) {
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Symbol '%s' not found", name);
        exception(set_errno(set_error_report_errno(f->error), msg));
    }
    else if (id2symbol(f->id_buf[0], sym) < 0) {
        exception(errno);
    }
    else {
        find_next_buf = f->id_buf;
        find_next_cnt = f->id_cnt;
        find_next_pos = 1;
    }
    clear_trap(&trap);
    return 0;
}

int find_symbol_by_addr(Context * ctx, int frame, ContextAddress addr, Symbol ** sym) {
    uint64_t ip = 0;
    LINK * l = NULL;
    SymbolsCache * syms = NULL;
    FindSymCache * f = NULL;
    unsigned h;
    Trap trap;

    if (!set_trap(&trap)) return -1;

    ip = get_symbol_ip(ctx, &frame, addr);
    h = hash_find(ctx, NULL, ip);
    syms = get_symbols_cache();
    for (l = syms->link_find_by_addr[h].next; l != syms->link_find_by_addr + h; l = l->next) {
        FindSymCache * c = syms2find(l);
        if (c->ctx == ctx && c->frame == frame && c->ip == ip && c->addr == addr) {
            f = c;
            break;
        }
    }

    if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (FindSymCache *)loc_alloc_zero(sizeof(FindSymCache));
        list_add_first(&f->link_syms, syms->link_find_by_addr + h);
        if (ip) {
            list_add_last(&f->link_flush, &flush_rc);
            f->update_policy = UPDATE_ON_EXE_STATE_CHANGES;
        }
        else {
            list_add_last(&f->link_flush, &flush_mm);
            f->update_policy = UPDATE_ON_MEMORY_MAP_CHANGES;
        }
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_FIND;
        f->frame = frame;
        f->ip = ip;
        f->addr = addr;
        f->pending = protocol_send_command(c, SYMBOLS, "findByAddr", validate_find, f);
        if (frame != STACK_NO_FRAME) {
            json_write_string(&c->out, frame2id(ctx, frame));
        }
        else {
            json_write_string(&c->out, ctx->id);
        }
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, addr);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->pending != NULL) {
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        exception(set_error_report_errno(f->error));
    }
    else if (id2symbol(f->id_buf[0], sym) < 0) {
        exception(errno);
    }
    else {
        find_next_buf = f->id_buf;
        find_next_cnt = f->id_cnt;
        find_next_pos = 1;
    }
    clear_trap(&trap);
    return 0;
}

int find_symbol_in_scope(Context * ctx, int frame, ContextAddress addr, Symbol * scope, const char * name, Symbol ** sym) {
    uint64_t ip = 0;
    LINK * l = NULL;
    SymbolsCache * syms = NULL;
    FindSymCache * f = NULL;
    unsigned h;
    Trap trap;

    if (!set_trap(&trap)) return -1;

    ip = get_symbol_ip(ctx, &frame, addr);
    h = hash_find(ctx, name, ip);
    syms = get_symbols_cache();
    for (l = syms->link_find_in_scope[h].next; l != syms->link_find_in_scope + h; l = l->next) {
        FindSymCache * c = syms2find(l);
        if (c->ctx == ctx && c->frame == frame && c->ip == ip && strcmp(c->name, name) == 0) {
            if (scope == NULL && c->scope == NULL) {
                f = c;
                break;
            }
            if (scope == NULL || c->scope == NULL) continue;
            if (strcmp(scope->cache->id, c->scope) == 0) {
                f = c;
                break;
            }
        }
    }

    if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (FindSymCache *)loc_alloc_zero(sizeof(FindSymCache));
        list_add_first(&f->link_syms, syms->link_find_in_scope + h);
        if (ip) {
            list_add_last(&f->link_flush, &flush_rc);
            f->update_policy = UPDATE_ON_EXE_STATE_CHANGES;
        }
        else {
            list_add_last(&f->link_flush, &flush_mm);
            f->update_policy = UPDATE_ON_MEMORY_MAP_CHANGES;
        }
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_FIND;
        f->frame = frame;
        f->ip = ip;
        if (scope != NULL) f->scope = loc_strdup(scope->cache->id);
        f->name = loc_strdup(name);
        f->pending = protocol_send_command(c, SYMBOLS, "findInScope", validate_find, f);
        if (frame != STACK_NO_FRAME) {
            json_write_string(&c->out, frame2id(ctx, frame));
        }
        else {
            json_write_string(&c->out, ctx->id);
        }
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, ip);
        write_stream(&c->out, 0);
        json_write_string(&c->out, scope ? scope->cache->id : NULL);
        write_stream(&c->out, 0);
        json_write_string(&c->out, name);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->pending != NULL) {
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        char msg[256];
        snprintf(msg, sizeof(msg), "Symbol '%s' not found", name);
        exception(set_errno(set_error_report_errno(f->error), msg));
    }
    else if (id2symbol(f->id_buf[0], sym) < 0) {
        exception(errno);
    }
    else {
        find_next_buf = f->id_buf;
        find_next_cnt = f->id_cnt;
        find_next_pos = 1;
    }
    clear_trap(&trap);
    return 0;
}

int find_next_symbol(Symbol ** sym) {
    if (find_next_buf != NULL && find_next_pos < find_next_cnt) {
        if (id2symbol(find_next_buf[find_next_pos], sym) < 0) return -1;
        find_next_pos++;
        return 0;
    }
    errno = ERR_SYM_NOT_FOUND;
    return -1;
}

int enumerate_symbols(Context * ctx, int frame, EnumerateSymbolsCallBack * func, void * args) {
    uint64_t ip = 0;
    unsigned h;
    LINK * l;
    Trap trap;
    SymbolsCache * syms = NULL;
    FindSymCache * f = NULL;

    if (!set_trap(&trap)) return -1;

    ip = get_symbol_ip(ctx, &frame, 0);
    h = hash_list(ctx, ip);
    syms = get_symbols_cache();
    for (l = syms->link_list[h].next; l != syms->link_list + h; l = l->next) {
        FindSymCache * c = syms2find(l);
        if (c->ctx == ctx && c->frame == frame && c->ip == ip) {
            f = c;
            break;
        }
    }

    if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (FindSymCache *)loc_alloc_zero(sizeof(FindSymCache));
        list_add_first(&f->link_syms, syms->link_list + h);
        if (ip) {
            list_add_last(&f->link_flush, &flush_rc);
            f->update_policy = UPDATE_ON_EXE_STATE_CHANGES;
        }
        else {
            list_add_last(&f->link_flush, &flush_mm);
            f->update_policy = UPDATE_ON_MEMORY_MAP_CHANGES;
        }
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_FIND;
        f->frame = frame;
        f->ip = ip;
        f->pending = protocol_send_command(c, SYMBOLS, "list", validate_find, f);
        if (frame != STACK_NO_FRAME) {
            json_write_string(&c->out, frame2id(ctx, frame));
        }
        else {
            json_write_string(&c->out, ctx->id);
        }
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->pending != NULL) {
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        exception(set_error_report_errno(f->error));
    }
    else {
        int i;
        for (i = 0; i < f->id_cnt; i++) {
            Symbol * sym = NULL;
            if (id2symbol(f->id_buf[i], &sym) < 0) exception(errno);
            func(args, sym);
        }
    }
    clear_trap(&trap);
    return 0;
}

const char * symbol2id(const Symbol * sym) {
    SymInfoCache * s = sym->cache;
    assert(s->magic == MAGIC_INFO);
    assert(s->id != NULL);
    return s->id;
}

int id2symbol(const char * id, Symbol ** sym) {
    LINK * l;
    SymInfoCache * s = NULL;
    unsigned h = hash_sym_id(id);
    SymbolsCache * syms = NULL;
    Trap trap;

    if (!set_trap(&trap)) return -1;
    syms = get_symbols_cache();
    for (l = syms->link_sym[h].next; l != syms->link_sym + h; l = l->next) {
        SymInfoCache * x = syms2sym(l);
        if (strcmp(x->id, id) == 0) {
            s = x;
            break;
        }
    }
    if (s == NULL) {
        s = (SymInfoCache *)loc_alloc_zero(sizeof(SymInfoCache));
        s->magic = MAGIC_INFO;
        s->id = loc_strdup(id);
        s->frame = STACK_NO_FRAME;
        s->update_policy = UPDATE_ON_MEMORY_MAP_CHANGES;
        list_add_first(&s->link_syms, syms->link_sym + h);
        list_add_last(&s->link_flush, &flush_mm);
        list_init(&s->array_syms);
    }
    else if (!s->disposed) {
        /* Move used item at the end of the flush list */
        list_remove(&s->link_flush);
        if (s->update_policy == UPDATE_ON_EXE_STATE_CHANGES) list_add_last(&s->link_flush, &flush_rc)
        else list_add_last(&s->link_flush, &flush_mm);
    }
    *sym = alloc_symbol();
    (*sym)->cache = s;
    clear_trap(&trap);
    return 0;
}

/*************** Functions for retrieving symbol properties ***************************************/

int get_symbol_class(const Symbol * sym, int * symbol_class) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    *symbol_class = c->sym_class;
    return 0;
}

int get_symbol_type(const Symbol * sym, Symbol ** type) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    if (c->type_id) {
        if (strcmp(c->type_id, c->id)) return id2symbol(c->type_id, type);
        *type = (Symbol *)sym;
    }
    else {
        *type = NULL;
    }
    return 0;
}

int get_symbol_type_class(const Symbol * sym, int * type_class) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    *type_class = c->type_class;
    return 0;
}

int get_symbol_update_policy(const Symbol * sym, char ** id, int * policy) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    if (c->update_owner == NULL) {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    *id = c->update_owner->id;
    *policy = c->update_policy;
    return 0;
}

int get_symbol_name(const Symbol * sym, char ** name) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    *name = c->name;
    return 0;
}

int get_symbol_base_type(const Symbol * sym, Symbol ** type) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    if (c->base_type_id) return id2symbol(c->base_type_id, type);
    return 0;
}

int get_symbol_index_type(const Symbol * sym, Symbol ** type) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    if (c->index_type_id) return id2symbol(c->index_type_id, type);
    return 0;
}

int get_symbol_container(const Symbol * sym, Symbol ** container) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    if (c->container_id) return id2symbol(c->container_id, container);
    return 0;
}

int get_symbol_size(const Symbol * sym, ContextAddress * size) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_SIZE);
    if (c == NULL) return -1;
    if (!c->has_size) {
        set_errno(ERR_OTHER, "Debug info not available");
        return -1;
    }
    *size = c->size;
    return 0;
}

int get_symbol_length(const Symbol * sym, ContextAddress * length) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_BOUNDS);
    if (c == NULL) return -1;
    if (c->has_length) {
        *length = c->length;
        return 0;
    }
    errno = ERR_INV_CONTEXT;
    return -1;
}

int get_symbol_lower_bound(const Symbol * sym, int64_t * lower_bound) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_BOUNDS);
    if (c == NULL) return -1;
    if (!c->has_lower_bound) {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    *lower_bound = c->lower_bound;
    return 0;
}

int get_symbol_flags(const Symbol * sym, SYM_FLAGS * flags) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    *flags = c->flags;
    return 0;
}

int get_symbol_props(const Symbol * sym, SymbolProperties * props) {
    SymInfoCache * c;
    memset(props, 0, sizeof(SymbolProperties));
    c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    *props = c->props;
    return 0;
}

int get_symbol_frame(const Symbol * sym, Context ** ctx, int * frame) {
    SymInfoCache * c = get_sym_info_cache(sym, ACC_OTHER);
    if (c == NULL) return -1;
    *ctx = c->update_owner;
    *frame = c->frame;
    return 0;
}

static void validate_children(Channel * c, void * args, int error) {
    SymInfoCache * s = (SymInfoCache *)args;
    assert(s->magic == MAGIC_INFO);
    assert(s->pending_get_children != NULL);
    assert(s->error_get_children == NULL);
    assert(!s->done_children);
    s->pending_get_children = NULL;
    s->done_children = 1;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            error = read_errno(&c->inp);
            s->children_ids = read_symbol_list(&c->inp, &s->children_count);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    s->error_get_children = get_error_report(error);
    cache_notify_later(&s->cache);
    if (s->disposed) free_sym_info_cache(s);
    run_ctrl_unlock();
}

int get_symbol_children(const Symbol * sym, Symbol *** children, int * count) {
    Trap trap;
    SymInfoCache * s = get_sym_info_cache(sym, ACC_OTHER);
    *children = NULL;
    *count = 0;
    if (s == NULL) return -1;
    if (!set_trap(&trap)) return -1;
    if (s->pending_get_children) {
        cache_wait(&s->cache);
    }
    else if (s->error_get_children) {
        exception(set_error_report_errno(s->error_get_children));
    }
    else if (!s->done_children) {
        Channel * c = cache_channel();
        if (c == NULL || is_channel_closed(c)) exception(ERR_SYM_NOT_FOUND);
        run_ctrl_lock();
        s->pending_get_children = protocol_send_command(c, SYMBOLS, "getChildren", validate_children, s);
        json_write_string(&c->out, s->id);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&s->cache);
    }
    else if (s->children_count > 0) {
        int i, cnt = s->children_count;
        Symbol ** buf = (Symbol **)tmp_alloc(cnt * sizeof(Symbol *));
        for (i = 0; i < cnt; i++) {
            if (id2symbol(s->children_ids[i], buf + i) < 0) exception(errno);
        }
        *children = buf;
        *count = cnt;
    }
    clear_trap(&trap);
    return 0;
}

static void validate_array_type_id(Channel * c, void * args, int error) {
    ArraySymCache * s = (ArraySymCache *)args;
    assert(s->magic == MAGIC_ARRAY);
    assert(s->pending != NULL);
    assert(s->error == NULL);
    assert(s->id == NULL);
    s->pending = NULL;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            error = read_errno(&c->inp);
            s->id = json_read_alloc_string(&c->inp);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    s->error = get_error_report(error);
    cache_notify_later(&s->cache);
    if (s->disposed) free_arr_sym_cache(s);
    run_ctrl_unlock();
}

int get_array_symbol(const Symbol * sym, ContextAddress length, Symbol ** ptr) {
    LINK * l;
    Trap trap;
    ArraySymCache * a = NULL;
    SymInfoCache * s = get_sym_info_cache(sym, ACC_OTHER);
    if (s == NULL) return -1;
    if (!set_trap(&trap)) return -1;
    for (l = s->array_syms.next; l != &s->array_syms; l = l->next) {
        ArraySymCache * x = sym2arr(l);
        if (x->length == length) {
            a = x;
            break;
        }
    }
    if (a == NULL) {
        Channel * c = cache_channel();
        if (c == NULL || is_channel_closed(c)) exception(ERR_SYM_NOT_FOUND);
        a = (ArraySymCache *)loc_alloc_zero(sizeof(*a));
        list_add_first(&a->link_sym, &s->array_syms);
        run_ctrl_lock();
        a->magic = MAGIC_ARRAY;
        a->length = length;
        a->pending = protocol_send_command(c, SYMBOLS, "getArrayType", validate_array_type_id, a);
        json_write_string(&c->out, s->id);
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, length);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&a->cache);
    }
    else if (a->pending != NULL) {
        cache_wait(&a->cache);
    }
    else if (a->error != NULL) {
        exception(set_error_report_errno(a->error));
    }
    else if (id2symbol(a->id, ptr) < 0) {
        exception(errno);
    }
    clear_trap(&trap);
    return 0;
}

/*************************************************************************************************/

static void read_address_attrs(InputStream * inp, const char * name, void * x) {
    AddressInfoCache * f = (AddressInfoCache *)x;
    if (strcmp(name, "Addr") == 0) f->range_addr = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "Size") == 0) f->range_size = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "ISA") == 0) f->isa = json_read_alloc_string(inp);
    else if (strcmp(name, "PLT") == 0) f->plt = (ContextAddress)json_read_uint64(inp);
    else json_skip_object(inp);
}

static void validate_address_info(Channel * c, void * args, int error) {
    AddressInfoCache * f = (AddressInfoCache *)args;
    assert(f->magic == MAGIC_ADDR);
    assert(f->pending != NULL);
    assert(f->error == NULL);
    f->pending = NULL;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            error = read_errno(&c->inp);
            json_read_struct(&c->inp, read_address_attrs, f);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    if (f->range_addr != 0 || f->range_size != 0) {
        if (f->range_addr + f->range_size < f->range_addr) {
            f->range_size = ~f->range_addr + 1;
        }
        if (f->addr < f->range_addr || f->addr > f->range_addr + f->range_size - 1) {
            if (!error) error = set_errno(ERR_OTHER, "Invalid reply of getAddressInfo command");
            f->range_addr = f->addr;
            f->range_size = 1;
        }
    }
    f->error = get_error_report(error);
    cache_notify_later(&f->cache);
    if (f->disposed) free_address_info_cache(f);
    run_ctrl_unlock();
}

static int get_address_info(Context * ctx, ContextAddress addr, AddressInfoCache ** info) {
    Trap trap;
    unsigned h;
    LINK * l;
    SymbolsCache * syms = NULL;
    AddressInfoCache * f = NULL;

    if (!set_trap(&trap)) return -1;

    syms = get_symbols_cache();
    if (!syms->service_available) {
        clear_trap(&trap);
        *info = NULL;
        return 0;
    }

    h = hash_address(ctx);
    for (l = syms->link_address[h].next; l != syms->link_address + h; l = l->next) {
        AddressInfoCache * c = syms2address(l);
        if (c->ctx == ctx) {
            if (c->pending != NULL) {
                cache_wait(&c->cache);
            }
            else if (c->range_addr == 0 && c->range_size == 0) {
                f = c;
                break;
            }
            else if (addr >= c->range_addr && addr <= c->range_addr + c->range_size - 1) {
                f = c;
                break;
            }
        }
    }

    assert(f == NULL || f->pending == NULL);

    if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (AddressInfoCache *)loc_alloc_zero(sizeof(AddressInfoCache));
        list_add_first(&f->link_syms, syms->link_address + h);
        list_add_last(&f->link_flush, &flush_mm);
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_ADDR;
        f->addr = addr;
        context_lock(f->ctx = ctx);
        f->pending = protocol_send_command(c, SYMBOLS, "getAddressInfo", validate_address_info, f);
        json_write_string(&c->out, ctx->id);
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, addr);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        exception(set_error_report_errno(f->error));
    }
    else {
        *info = f;
    }

    clear_trap(&trap);
    return 0;
}

ContextAddress is_plt_section(Context * ctx, ContextAddress addr) {
    AddressInfoCache * i = NULL;
    errno = 0;
    if (get_address_info(ctx, addr, &i) < 0) return 0;
    if (i == NULL) return 0;
    return i->plt;
}

int get_context_isa(Context * ctx, ContextAddress addr, const char ** isa,
        ContextAddress * range_addr, ContextAddress * range_size) {
    AddressInfoCache * i = NULL;
    if (get_address_info(ctx, addr, &i) < 0) return -1;
    if (i == NULL) {
        *isa = NULL;
        *range_addr = 0;
        *range_size = 0;
    }
    else {
        *isa = i->isa;
        *range_addr = i->range_addr;
        *range_size = i->range_size;
    }
    return 0;
}

/*************************************************************************************************/

static LocationCommands location_cmds = { NULL, 0, 0};

static unsigned trace_regs_cnt = 0;
static unsigned trace_regs_max = 0;
static StackFrameRegisterLocation ** trace_regs = NULL;

static unsigned trace_subs_cnt = 0;
static unsigned trace_subs_max = 0;
static StackFrameInlinedSubroutine ** trace_subs = NULL;

static unsigned discriminant_cnt = 0;
static unsigned discriminant_max = 0;
static DiscriminantRange * discriminant_lst = NULL;

static int id2register_error = 0;

static LocationExpressionCommand * add_location_command(int op) {
    LocationExpressionCommand * cmd = NULL;
    if (location_cmds.cnt >= location_cmds.max) {
        location_cmds.max += 16;
        location_cmds.cmds = (LocationExpressionCommand *)loc_realloc(location_cmds.cmds,
            sizeof(LocationExpressionCommand) * location_cmds.max);
    }
    cmd = location_cmds.cmds + location_cmds.cnt++;
    memset(cmd, 0, sizeof(LocationExpressionCommand));
    cmd->cmd = op;
    return cmd;
}

static void read_dwarf_location_params(InputStream * inp, const char * nm, void * arg) {
    LocationExpressionCommand * cmd = (LocationExpressionCommand *)arg;
    if (strcmp(nm, "Machine") == 0) cmd->args.loc.reg_id_scope.machine = (uint16_t)json_read_long(inp);
    else if (strcmp(nm, "ABI") == 0) cmd->args.loc.reg_id_scope.os_abi = (uint8_t)json_read_long(inp);
    else if (strcmp(nm, "FPABI") == 0) cmd->args.loc.reg_id_scope.fp_abi = (uint8_t)json_read_long(inp);
    else if (strcmp(nm, "ELF64") == 0) cmd->args.loc.reg_id_scope.elf64 = (uint8_t)json_read_boolean(inp);
    else if (strcmp(nm, "RegIdType") == 0) cmd->args.loc.reg_id_scope.id_type = (uint8_t)json_read_long(inp);
    else if (strcmp(nm, "AddrSize") == 0) cmd->args.loc.addr_size = (size_t)json_read_long(inp);
    else if (strcmp(nm, "BigEndian") == 0) cmd->args.loc.reg_id_scope.big_endian = (uint8_t)json_read_boolean(inp);
}

static void read_location_command(InputStream * inp, void * args) {
    char id[256];
    size_t val_size = 0;
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    LocationExpressionCommand * cmd = NULL;
    cmd = add_location_command((int)json_read_long(inp));
    switch (cmd->cmd) {
    case SFT_CMD_NUMBER:
        json_test_char(inp, ',');
        cmd->args.num = json_read_int64(inp);
        break;
    case SFT_CMD_ARG:
    case SFT_CMD_SET_ARG:
        json_test_char(inp, ',');
        cmd->args.num = (unsigned)json_read_ulong(inp);
        break;
    case SFT_CMD_RD_REG:
    case SFT_CMD_WR_REG:
        json_test_char(inp, ',');
        json_read_string(inp, id, sizeof(id));
        if (id2register(id, &ctx, &frame, &cmd->args.reg) < 0) id2register_error = errno;
        break;
    case SFT_CMD_RD_MEM:
    case SFT_CMD_WR_MEM:
    case SFT_CMD_LOAD:
        json_test_char(inp, ',');
        cmd->args.mem.size = json_read_ulong(inp);
        json_test_char(inp, ',');
        cmd->args.mem.big_endian = json_read_boolean(inp);
        break;
    case SFT_CMD_LOCATION:
        json_test_char(inp, ',');
        cmd->args.loc.code_addr = (uint8_t *)json_read_alloc_binary(inp, &cmd->args.loc.code_size);
        json_test_char(inp, ',');
        json_read_struct(inp, read_dwarf_location_params, cmd);
        cmd->args.loc.func = evaluate_vm_expression;
        break;
    case SFT_CMD_PIECE:
        json_test_char(inp, ',');
        cmd->args.piece.bit_offs = (unsigned)json_read_ulong(inp);
        json_test_char(inp, ',');
        cmd->args.piece.bit_size = (unsigned)json_read_ulong(inp);
        json_test_char(inp, ',');
        if (json_read_string(inp, id, sizeof(id)) > 0) {
            if (id2register(id, &ctx, &frame, &cmd->args.piece.reg) < 0) id2register_error = errno;
        }
        json_test_char(inp, ',');
        cmd->args.piece.value = json_read_alloc_binary(inp, &val_size);
        if (cmd->args.piece.value != NULL && val_size < (cmd->args.piece.bit_size + 7) / 8) {
            exception(ERR_JSON_SYNTAX);
        }
        break;
    }
}

static void read_location_command_array(InputStream * inp, LocationCommands * cmds) {
    location_cmds.cnt = 0;
    if (json_read_array(inp, read_location_command, NULL)) {
        cmds->cmds = (LocationExpressionCommand *)loc_alloc(location_cmds.cnt * sizeof(LocationExpressionCommand));
        memcpy(cmds->cmds, location_cmds.cmds, location_cmds.cnt * sizeof(LocationExpressionCommand));
        cmds->cnt = cmds->max = location_cmds.cnt;
    }
}

static void read_discriminant_range(InputStream * inp, const char * name, void * args) {
    DiscriminantRange * r = (DiscriminantRange *)args;
    if (strcmp(name, "X") == 0) r->x = json_read_int64(inp);
    else if (strcmp(name, "Y") == 0) r->y = json_read_int64(inp);
    else json_skip_object(inp);
}

static void read_discriminant_value(InputStream * inp, void * args) {
    DiscriminantRange * r;
    if (discriminant_cnt >= discriminant_max) {
        discriminant_max += 16;
        discriminant_lst = (DiscriminantRange *)loc_realloc(discriminant_lst, sizeof(DiscriminantRange) * discriminant_max);
    }
    r = discriminant_lst + discriminant_cnt++;
    if (json_peek(inp) == '{') {
        memset(r, 0, sizeof(DiscriminantRange));
        json_read_struct(inp, read_discriminant_range, r);
    }
    else {
        r->x = r->y = json_read_int64(inp);
    }
}

static void read_discriminant_array(InputStream * inp, LocationInfo * info) {
    discriminant_cnt = 0;
    if (json_read_array(inp, read_discriminant_value, NULL)) {
        info->discr_lst = (DiscriminantRange *)loc_alloc(discriminant_cnt * sizeof(DiscriminantRange));
        memcpy(info->discr_lst, discriminant_lst, discriminant_cnt * sizeof(DiscriminantRange));
        info->discr_cnt = discriminant_cnt;
    }
}

static void read_location_attrs(InputStream * inp, const char * name, void * x) {
    LocationInfoCache * f = (LocationInfoCache *)x;
    if (strcmp(name, "ArgCnt") == 0) f->info.args_cnt = (unsigned)json_read_ulong(inp);
    else if (strcmp(name, "CodeAddr") == 0) f->info.code_addr = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "CodeSize") == 0) f->info.code_size = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "BigEndian") == 0) f->info.big_endian = json_read_boolean(inp);
    else if (strcmp(name, "ValueCmds") == 0) read_location_command_array(inp, &f->info.value_cmds);
    else if (strcmp(name, "Discriminant") == 0) read_discriminant_array(inp, &f->info);
    else json_skip_object(inp);
}

static void validate_location_info(Channel * c, void * args, int error) {
    LocationInfoCache * f = (LocationInfoCache *)args;
    assert(f->magic == MAGIC_LOC);
    assert(f->pending != NULL);
    assert(f->error == NULL);
    f->pending = NULL;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            id2register_error = 0;
            error = read_errno(&c->inp);
            json_read_struct(&c->inp, read_location_attrs, f);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            if (!error && id2register_error) error = id2register_error;
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    f->error = get_error_report(error);
    cache_notify_later(&f->cache);
    if (f->disposed) free_location_info_cache(f);
    run_ctrl_unlock();
}

int get_location_info(const Symbol * sym, LocationInfo ** loc) {
    Trap trap;
    unsigned h;
    LINK * l;
    SymbolsCache * syms = NULL;
    LocationInfoCache * f = NULL;
    SymInfoCache * sym_cache = NULL;
    Context * ctx = NULL;
    uint64_t ip = 0;

    sym_cache = get_sym_info_cache(sym, ACC_OTHER);
    if (sym_cache == NULL) return -1;

    ctx = sym_cache->update_owner;

    if (!set_trap(&trap)) return -1;

    if (sym_cache->frame != STACK_NO_FRAME) {
        StackFrame * frame = NULL;
        if (get_frame_info(ctx, sym_cache->frame, &frame) < 0) exception(errno);
        if (read_reg_value(frame, get_PC_definition(ctx), &ip) < 0) exception(errno);
    }

    h = hash_sym_id(sym_cache->id);
    syms = get_symbols_cache();
    for (l = syms->link_location[h].next; l != syms->link_location + h; l = l->next) {
        LocationInfoCache * c = syms2location(l);
        if (c->ctx == ctx && strcmp(sym_cache->id, c->sym_id) == 0) {
            if (c->pending != NULL) {
                cache_wait(&c->cache);
            }
            else if (c->info.code_size == 0 ||
                    (c->info.code_addr <= ip && c->info.code_addr + c->info.code_size > ip)) {
                f = c;
                break;
            }
        }
    }

    assert(f == NULL || f->pending == NULL);

    if (f == NULL) {
        f = (LocationInfoCache *)loc_alloc_zero(sizeof(LocationInfoCache));
        list_add_first(&f->link_syms, syms->link_location + h);
        list_add_last(&f->link_flush, &flush_mm);
        context_lock(f->ctx = ctx);
        f->magic = MAGIC_LOC;
        f->ip = ip;
    }
    if (f->sym_id == NULL) {
        Channel * c = get_channel(syms);
        run_ctrl_lock();
        f->sym_id = loc_strdup(sym_cache->id);
        f->pending = protocol_send_command(c, SYMBOLS, "getLocationInfo", validate_location_info, f);
        json_write_string(&c->out, f->sym_id);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        exception(set_error_report_errno(f->error));
    }
    else {
        *loc = &f->info;
    }

    clear_trap(&trap);
    return 0;
}

static void read_stack_trace_register(InputStream * inp, const char * id, void * args) {
    if (trace_regs_cnt >= trace_regs_max) {
        trace_regs_max += 16;
        trace_regs = (StackFrameRegisterLocation **)loc_realloc(trace_regs, trace_regs_max * sizeof(StackFrameRegisterLocation *));
    }
    location_cmds.cnt = 0;
    if (json_read_array(inp, read_location_command, NULL) && location_cmds.cnt > 0) {
        Context * ctx = NULL;
        int frame = STACK_NO_FRAME;
        StackFrameRegisterLocation * reg = (StackFrameRegisterLocation *)loc_alloc(
            sizeof(StackFrameRegisterLocation) + (location_cmds.cnt - 1) * sizeof(LocationExpressionCommand));
        if (id2register(id, &ctx, &frame, &reg->reg) < 0) {
            id2register_error = errno;
            loc_free(reg);
        }
        else {
            reg->cmds_cnt = location_cmds.cnt;
            reg->cmds_max = location_cmds.cnt;
            memcpy(reg->cmds, location_cmds.cmds, location_cmds.cnt * sizeof(LocationExpressionCommand));
            trace_regs[trace_regs_cnt++] = reg;
        }
    }
}

static void read_inlined_subroutine_props(InputStream * inp, const char * name, void * args) {
    StackFrameInlinedSubroutine * s = (StackFrameInlinedSubroutine *)args;
    if (strcmp(name, "ID") == 0) s->func_id = json_read_alloc_string(inp);
    else if (strcmp(name, "Area") == 0) read_code_area(inp, &s->area);
    else json_skip_object(inp);
}

static void read_inlined_subroutine(InputStream * inp, void * args) {
    if (trace_subs_cnt >= trace_subs_max) {
        trace_subs_max += 16;
        trace_subs = (StackFrameInlinedSubroutine **)loc_realloc(trace_subs, trace_subs_max * sizeof(StackFrameInlinedSubroutine *));
    }
    trace_subs[trace_subs_cnt] = (StackFrameInlinedSubroutine *)loc_alloc_zero(sizeof(StackFrameInlinedSubroutine));
    json_read_struct(inp, read_inlined_subroutine_props, trace_subs[trace_subs_cnt++]);
}

static void read_stack_frame_fp(InputStream * inp, StackFrameCache * f) {
    location_cmds.cnt = 0;
    if (json_read_array(inp, read_location_command, NULL) && location_cmds.cnt > 0) {
        f->sti.fp = (StackFrameRegisterLocation *)loc_alloc(sizeof(StackFrameRegisterLocation) +
            (location_cmds.cnt - 1) * sizeof(LocationExpressionCommand));
        f->sti.fp->reg = NULL;
        f->sti.fp->cmds_cnt = location_cmds.cnt;
        f->sti.fp->cmds_max = location_cmds.cnt;
        memcpy(f->sti.fp->cmds, location_cmds.cmds, location_cmds.cnt * sizeof(LocationExpressionCommand));
    }
}

static void read_stack_frame_regs(InputStream * inp, StackFrameCache * f) {
    trace_regs_cnt = 0;
    if (json_read_struct(inp, read_stack_trace_register, NULL)) {
        f->sti.reg_cnt = trace_regs_cnt;
        f->sti.regs = (StackFrameRegisterLocation **)loc_alloc(trace_regs_cnt * sizeof(StackFrameRegisterLocation *));
        memcpy(f->sti.regs, trace_regs, trace_regs_cnt * sizeof(StackFrameRegisterLocation *));
    }
}

static void read_stack_frame_inlined(InputStream * inp, StackFrameCache * f) {
    trace_subs_cnt = 0;
    if (json_read_array(inp, read_inlined_subroutine, NULL)) {
        f->sti.sub_cnt = trace_subs_cnt;
        f->sti.subs = (StackFrameInlinedSubroutine **)loc_alloc(trace_subs_cnt * sizeof(StackFrameInlinedSubroutine *));
        memcpy(f->sti.subs, trace_subs, trace_subs_cnt * sizeof(StackFrameInlinedSubroutine *));
    }
}

static void read_stack_frame_props(InputStream * inp, const char * name, void * args) {
    StackFrameCache * f = (StackFrameCache *)args;
    if (strcmp(name, "CodeAddr") == 0) f->sti.addr = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "CodeSize") == 0) f->sti.size = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "FP") == 0) read_stack_frame_fp(inp, f);
    else if (strcmp(name, "Regs") == 0) read_stack_frame_regs(inp, f);
    else if (strcmp(name, "Inlined") == 0) read_stack_frame_inlined(inp, f);
    else json_skip_object(inp);
}

static void validate_frame(Channel * c, void * args, int error) {
    StackFrameCache * f = (StackFrameCache *)args;
    assert(f->magic == MAGIC_FRAME);
    assert(f->pending != NULL);
    assert(f->error == NULL);
    f->pending = NULL;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            id2register_error = 0;
            error = read_errno(&c->inp);
            if (f->command_props) {
                json_read_struct(&c->inp, read_stack_frame_props, f);
                json_test_char(&c->inp, MARKER_EOA);
            }
            else {
                /* Deprecated, use findFrameProps */
                f->sti.addr = (ContextAddress)json_read_uint64(&c->inp);
                json_test_char(&c->inp, MARKER_EOA);
                f->sti.size = (ContextAddress)json_read_uint64(&c->inp);
                json_test_char(&c->inp, MARKER_EOA);
                read_stack_frame_fp(&c->inp, f);
                json_test_char(&c->inp, MARKER_EOA);
                read_stack_frame_regs(&c->inp, f);
                json_test_char(&c->inp, MARKER_EOA);
            }
            json_test_char(&c->inp, MARKER_EOM);
            if (!error && id2register_error) error = id2register_error;
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    if (error || f->sti.size == 0) {
        f->sti.addr = (ContextAddress)f->ip;
        f->sti.size = 1;
    }
    assert(f->sti.addr <= f->ip);
    assert(f->sti.addr + f->sti.size == 0 || f->sti.addr + f->sti.size > f->ip);
    f->error = get_error_report(error);
    cache_notify_later(&f->cache);
    if (f->disposed) free_stack_frame_cache(f);
    run_ctrl_unlock();
}

int get_stack_tracing_info(Context * ctx, ContextAddress ip, StackTracingInfo ** info) {
    Trap trap;
    unsigned h;
    LINK * l;
    SymbolsCache * syms = NULL;
    StackFrameCache * f = NULL;

    *info = NULL;
    if (!set_trap(&trap)) return -1;

    h = hash_frame(ctx);
    syms = get_symbols_cache();
    for (l = syms->link_frame[h].next; l != syms->link_frame + h; l = l->next) {
        StackFrameCache * c = syms2frame(l);
        if (c->ctx == ctx) {
            if (c->pending != NULL) {
                cache_wait(&c->cache);
            }
            else if (c->sti.addr <= ip &&
                    (c->sti.addr + c->sti.size > ip ||
                     c->sti.addr + c->sti.size < c->sti.addr)) {
                f = c;
                break;
            }
        }
    }

    assert(f == NULL || f->pending == NULL);

    if (f != NULL && f->error != NULL && get_error_code(set_error_report_errno(f->error)) == ERR_INV_COMMAND) {
        if (f->command_props) {
            syms->no_find_frame_props = 1;
        }
        else {
            syms->no_find_frame_info = 1;
        }
        free_stack_frame_cache(f);
        f = NULL;
    }

    if (f == NULL && !syms->service_available) {
        /* nothing */
    }
    else if (f == NULL && syms->no_find_frame_info && syms->no_find_frame_props) {
        /* nothing */
    }
    else if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (StackFrameCache *)loc_alloc_zero(sizeof(StackFrameCache));
        list_add_first(&f->link_syms, syms->link_frame + h);
        list_add_last(&f->link_flush, &flush_mm);
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_FRAME;
        f->ip = ip;
        if (syms->no_find_frame_props) {
            /* Deprecated, use findFrameProps */
            f->pending = protocol_send_command(c, SYMBOLS, "findFrameInfo", validate_frame, f);
            f->command_props = 0;
        }
        else {
            f->pending = protocol_send_command(c, SYMBOLS, "findFrameProps", validate_frame, f);
            f->command_props = 1;
        }
        json_write_string(&c->out, f->ctx->id);
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, ip);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        exception(set_error_report_errno(f->error));
    }
    else if (f->sti.fp != NULL) {
        *info = &f->sti;
    }

    clear_trap(&trap);
    return 0;
}

int get_funccall_info(const Symbol * func,
        const Symbol ** args, unsigned args_cnt, FunctionCallInfo ** info) {
    /* TODO: get_funccall_info() in symbols proxy */
    set_errno(ERR_OTHER, "get_funccall_info() is not supported yet by TCF server");
    return -1;
}

static void read_file_info_props(InputStream * inp, const char * name, void * args) {
    FileInfoCache * f = (FileInfoCache *)args;
    if (strcmp(name, "Addr") == 0) {
        f->info.addr = (ContextAddress)json_read_uint64(inp);
    }
    else if (strcmp(name, "Size") == 0) {
        f->info.size = (ContextAddress)json_read_uint64(inp);
    }
    else if (strcmp(name, "FileName") == 0) {
        loc_free(f->info.file_name);
        f->info.file_name = json_read_alloc_string(inp);
    }
    else if (strcmp(name, "FileError") == 0 && f->file_error == NULL) {
        release_error_report(f->file_error);
        f->file_error = get_error_report(read_error_object(inp));
    }
    else if (strcmp(name, "DynLoader") == 0) {
        f->info.dyn_loader = json_read_boolean(inp);
    }
    else {
        json_skip_object(inp);
    }
}

static void validate_file(Channel * c, void * args, int error) {
    FileInfoCache * f = (FileInfoCache *)args;
    assert(f->magic == MAGIC_FILE);
    assert(f->pending != NULL);
    assert(f->error == NULL);
    f->pending = NULL;
    if (!error) {
        Trap trap;
        if (set_trap(&trap)) {
            error = read_errno(&c->inp);
            json_read_struct(&c->inp, read_file_info_props, f);
            json_test_char(&c->inp, MARKER_EOA);
            json_test_char(&c->inp, MARKER_EOM);
            clear_trap(&trap);
        }
        else {
            error = trap.error;
        }
    }
    if (get_error_code(error) != ERR_INV_COMMAND) f->error = get_error_report(error);
    cache_notify_later(&f->cache);
    if (f->disposed) free_file_info_cache(f);
    run_ctrl_unlock();
}

static FileInfoCache * get_file_info_cache(Context * ctx, ContextAddress addr) {
    Trap trap;
    unsigned h;
    LINK * l;
    SymbolsCache * syms = NULL;
    FileInfoCache * f = NULL;

    if (!set_trap(&trap)) return NULL;

    h = hash_file(ctx);
    syms = get_symbols_cache();
    for (l = syms->link_file[h].next; l != syms->link_file + h; l = l->next) {
        FileInfoCache * c = syms2file(l);
        if (c->ctx == ctx) {
            if (c->pending != NULL) {
                cache_wait(&c->cache);
            }
            else if (c->addr == addr) {
                f = c;
                break;
            }
            else if (c->info.addr <= addr && c->info.addr + c->info.size > addr) {
                f = c;
                break;
            }
        }
    }

    assert(f == NULL || f->pending == NULL);

    if (f == NULL && !syms->service_available) {
        /* nothing */
    }
    else if (f == NULL) {
        Channel * c = get_channel(syms);
        f = (FileInfoCache *)loc_alloc_zero(sizeof(FileInfoCache));
        list_add_first(&f->link_syms, syms->link_file + h);
        list_add_last(&f->link_flush, &flush_mm);
        context_lock(f->ctx = ctx);
        run_ctrl_lock();
        f->magic = MAGIC_FILE;
        f->addr = addr;
        f->pending = protocol_send_command(c, SYMBOLS, "getSymFileInfo", validate_file, f);
        json_write_string(&c->out, f->ctx->id);
        write_stream(&c->out, 0);
        json_write_uint64(&c->out, addr);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
        cache_wait(&f->cache);
    }
    else if (f->error != NULL) {
        exception(set_error_report_errno(f->error));
    }

    clear_trap(&trap);
    errno = 0;
    return f;
}

int get_symbol_file_info(Context * ctx, ContextAddress addr, SymbolFileInfo ** info) {
    FileInfoCache * f = get_file_info_cache(ctx, addr);
    *info = NULL;
    if (f != NULL) {
        f->info.file_error = set_error_report_errno(f->file_error);
        *info = &f->info;
        if (f->info.file_name != NULL) return 0;
    }
    if (errno) return -1;
    return 0;
}

/*************************************************************************************************/

static int check_policy(Context * ctx, int mode, Context * sym_grp, Context * sym_ctx, int policy) {
    if ((mode & (1 << policy)) && sym_ctx == ctx) return 1;
    if (mode & (1 << UPDATE_ON_MEMORY_MAP_CHANGES)) {
        if (context_get_group(sym_ctx, CONTEXT_GROUP_SYMBOLS) == sym_grp) return 1;
    }
    return 0;
}

static void flush_one(Context * ctx, int mode, Context * sym_grp, LINK * l) {
    unsigned magic = flush2sym(l)->magic;
    if (magic == MAGIC_INFO) {
        SymInfoCache * c = flush2sym(l);
        if (!c->done_context || c->error_get_context != NULL) {
            free_sym_info_cache(c);
        }
        else if (c->update_owner == NULL || c->update_owner->exited) {
            free_sym_info_cache(c);
        }
        else if (check_policy(ctx, mode, sym_grp, c->update_owner, c->update_policy)) {
            if (mode == (1 << UPDATE_ON_EXE_STATE_CHANGES)) {
                c->degraded = 1;
            }
            else {
                free_sym_info_cache(c);
            }
        }
        return;
    }
    if (magic == MAGIC_FIND) {
        FindSymCache * c = flush2find(l);
        if (check_policy(ctx, mode, sym_grp, c->ctx, c->update_policy)) {
            free_find_sym_cache(c);
        }
        return;
    }
    if (magic == MAGIC_FRAME) {
        StackFrameCache * c = flush2frame(l);
        if (check_policy(ctx, mode, sym_grp, c->ctx, UPDATE_ON_MEMORY_MAP_CHANGES)) {
            free_stack_frame_cache(c);
        }
        return;
    }
    if (magic == MAGIC_ADDR) {
        AddressInfoCache * c = flush2address(l);
        if (check_policy(ctx, mode, sym_grp, c->ctx, UPDATE_ON_MEMORY_MAP_CHANGES)) {
            free_address_info_cache(c);
        }
        return;
    }
    if (magic == MAGIC_FILE) {
        FileInfoCache * c = flush2file(l);
        if (check_policy(ctx, mode, sym_grp, c->ctx, UPDATE_ON_MEMORY_MAP_CHANGES)) {
            free_file_info_cache(c);
        }
        return;
    }
    if (magic == MAGIC_LOC) {
        LocationInfoCache * c = flush2location(l);
        if (check_policy(ctx, mode, sym_grp, c->ctx, UPDATE_ON_MEMORY_MAP_CHANGES)) {
            free_location_info_cache(c);
        }
        return;
    }
    assert(0);
}

static void flush_syms(Context * ctx, int mode) {
    LINK * l;
    Context * sym_grp = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);

    for (l = flush_rc.next; l != &flush_rc;) {
        LINK * n = l;
        l = l->next;
        flush_one(ctx, mode, sym_grp, n);
    }

    if ((mode & (1 << UPDATE_ON_MEMORY_MAP_CHANGES)) == 0) return;

    for (l = flush_mm.next; l != &flush_mm;) {
        LINK * n = l;
        l = l->next;
        flush_one(ctx, mode, sym_grp, n);
    }
}

static void event_context_created(Context * ctx, void * x) {
    flush_syms(ctx, ~0);
}

static void event_context_exited(Context * ctx, void * x) {
    flush_syms(ctx, ~0);
}

static void event_context_stopped(Context * ctx, void * x) {
    flush_syms(ctx, (1 << UPDATE_ON_EXE_STATE_CHANGES));
}

static void event_context_started(Context * ctx, void * x) {
    flush_syms(ctx, (1 << UPDATE_ON_EXE_STATE_CHANGES));
}

static void event_context_changed(Context * ctx, void * x) {
    flush_syms(ctx, ~0);
}

#if SERVICE_MemoryMap
static void event_code_unmapped(Context * ctx, ContextAddress addr, ContextAddress size, void * x) {
    flush_syms(ctx, ~0);
}
#endif

#if ENABLE_SymbolsMux
static int reader_is_valid(Context * ctx, ContextAddress addr) {
    FileInfoCache * f = get_file_info_cache(ctx, addr);
    return f != NULL && f->info.file_name != NULL;
}
#endif

static void channel_close_listener(Channel * c) {
    LINK * l = root.next;
    while (l != &root) {
        SymbolsCache * s = root2syms(l);
        l = l->next;
        if (s->channel == c) free_symbols_cache(s);
    }
}

void ini_symbols_lib(void) {
    {
        static ContextEventListener listener = {
            event_context_created,
            event_context_exited,
            event_context_stopped,
            event_context_started,
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
    list_init(&flush_rc);
    list_init(&flush_mm);
#if ENABLE_SymbolsMux
    add_symbols_reader(&symbol_reader);
#endif
}

#endif
