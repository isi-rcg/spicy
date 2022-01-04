/*******************************************************************************
 * Copyright (c) 2013-2017 Xilinx, Inc. and others.
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
 *     Xilinx - initial API and implementation
 *******************************************************************************/

/*
 * Generic sampling/stack-tracing profiler implementation.
 */
#include <tcf/config.h>

#if ENABLE_ProfilerSST

#include <assert.h>
#include <tcf/framework/link.h>
#include <tcf/framework/json.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/myalloc.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/profiler.h>
#include <tcf/services/profiler_sst.h>

typedef struct SampleStackTrace {
    struct SampleStackTrace * next;
    unsigned len;
    ContextAddress pc[1];
} SampleStackTrace;

typedef struct {
    ContextAddress pc;
    SampleStackTrace * stk;
    unsigned cnt;
} ProfilerSample;

typedef struct {
    ProfilerSample * buf;
    unsigned buf_pos;
    unsigned buf_max;
} ProfilerSampleArray;

#define PSAMPLE_HASH_SIZE 511
#define STRACE_HASH_SIZE 511

typedef struct ProfilerSST {
    LINK link_core;
    Context * ctx;
    Channel * channel;
    unsigned frame_cnt;
    ProfilerSampleArray psample_hash[PSAMPLE_HASH_SIZE];
    unsigned psample_cnt;
    SampleStackTrace * strace_hash[STRACE_HASH_SIZE];
    int stop_pending;
    ContextAddress pc;
    int disposed;
    int posted;
} ProfilerSST;

typedef struct {
    LINK list;
} ContextExtensionPrfSST;

static ProfilerClass profiler_class;

static size_t context_extension_offset = 0;

#define link_core2prf(x) ((ProfilerSST *)((char *)(x) - offsetof(ProfilerSST, link_core)))
#define EXT(ctx) ((ContextExtensionPrfSST *)((char *)(ctx) + context_extension_offset))

static ContextAddress * stk_buf;
static unsigned stk_buf_pos;

static void get_stack_trace(ProfilerSST * prf) {
    StackFrame * info = NULL;
    int frame = get_prev_frame(prf->ctx, STACK_TOP_FRAME);
    RegisterDefinition * reg_pc = NULL;
    unsigned buf_max = prf->frame_cnt - 1;
    uint64_t pc = 0;

    if (frame < 0) return;
    get_frame_info(prf->ctx, frame, &info);
    if (info == NULL) return;
    reg_pc = get_PC_definition(prf->ctx);
    if (read_reg_value(info, reg_pc, &pc) < 0) return;
    stk_buf = (ContextAddress *)tmp_alloc(sizeof(ContextAddress) * buf_max);
    stk_buf[stk_buf_pos++] = (ContextAddress)pc;
    while (stk_buf_pos < buf_max) {
        frame = get_prev_frame(prf->ctx, frame);
        get_frame_info(prf->ctx, frame, &info);
        if (info == NULL) break;
        if (read_reg_value(info, reg_pc, &pc) < 0) break;
        stk_buf[stk_buf_pos++] = (ContextAddress)pc;
    }
}

static SampleStackTrace * find_stack_trace(ProfilerSST * prf) {
    SampleStackTrace * stk = NULL;
    if (stk_buf_pos > 0) {
        unsigned i = 0;
        unsigned h = 0;
        for (i = 0; i < stk_buf_pos; i++) {
            h = (h + (unsigned)(stk_buf[i] >> 4)) % STRACE_HASH_SIZE;
        }
        stk = prf->strace_hash[h];
        while (stk != NULL) {
            if (stk->len == stk_buf_pos && memcmp(stk->pc, stk_buf, stk_buf_pos * sizeof(ContextAddress)) == 0) {
                return stk;
            }
            stk = stk->next;
        }
        stk = (SampleStackTrace *)loc_alloc_zero(sizeof(SampleStackTrace) + sizeof(ContextAddress) * (stk_buf_pos - 1));
        memcpy(stk->pc, stk_buf, stk_buf_pos * sizeof(ContextAddress));
        stk->next = prf->strace_hash[h];
        prf->strace_hash[h] = stk;
        stk->len = stk_buf_pos;
    }
    return stk;
}

static void add_to_sample_array(ProfilerSST * prf, ContextAddress pc, SampleStackTrace * stk) {
    unsigned i;
    unsigned h = (unsigned)(pc >> 4) % PSAMPLE_HASH_SIZE;
    ProfilerSampleArray * a = prf->psample_hash + h;
    ProfilerSample * s = NULL;
    for (i = 0; i < a->buf_pos; i++) {
        ProfilerSample * p = a->buf + i;
        if (p->pc == pc && p->stk == stk) {
            s = p;
            break;
        }
    }
    if (s == NULL) {
        if (a->buf_pos >= a->buf_max) {
            a->buf = (ProfilerSample *)loc_realloc(a->buf, sizeof(ProfilerSample) * (a->buf_max += 64));
        }
        s = a->buf + a->buf_pos++;
        prf->psample_cnt += 3;
        if (stk != NULL) prf->psample_cnt += stk->len;
        s->pc = pc;
        s->stk = stk;
        s->cnt = 0;
    }
    s->cnt++;
}

static void add_sample_cache_client(void * x) {
    ProfilerSST * prf = *(ProfilerSST **)x;
    int error = 0;
    stk_buf_pos = 0;
    stk_buf = NULL;
    if (prf->frame_cnt > 1) {
        if (!prf->ctx->stopped) {
            prf->stop_pending = 1;
            context_stop(prf->ctx);
        }
        else if (get_PC(prf->ctx, &prf->pc) < 0) {
            error = errno;
        }
        else {
            get_stack_trace(prf);
        }
    }
    cache_exit();
    if (error == 0 && !prf->disposed && !prf->stop_pending) {
        SampleStackTrace * stk = find_stack_trace(prf);
        add_to_sample_array(prf, prf->pc, stk);
    }
    prf->posted = 0;
    if (prf->disposed) loc_free(prf);
    run_ctrl_unlock();
}

static void add_sample_event(void * args) {
    ProfilerSST * prf = (ProfilerSST *)args;
    cache_enter(add_sample_cache_client, prf->channel, &prf, sizeof(prf));
}

static void add_sample(ProfilerSST * prf) {
    assert(!prf->disposed);
    if (prf->posted) return;
    prf->posted = 1;
    run_ctrl_lock();
    post_event(add_sample_event, prf);
}

int profiler_sst_is_enabled(Context * ctx) {
    ContextExtensionPrfSST * ext = EXT(ctx);
    return !list_is_empty(&ext->list);
}

void profiler_sst_sample(Context * ctx, ContextAddress pc) {
    LINK * l;
    ContextExtensionPrfSST * ext = EXT(ctx);
    for (l = ext->list.next; l != &ext->list; l = l->next) {
        ProfilerSST * prf = link_core2prf(l);
        if (prf->frame_cnt <= 1) {
            /* Shortcut for non-hierarchical profiling */
            if (prf->frame_cnt > 0) add_to_sample_array(prf, pc, NULL);
            continue;
        }
        prf->pc = pc;
        add_sample(prf);
    }
}

static void free_buffers(ProfilerSST * prf) {
    unsigned i;
    assert(!prf->disposed);
    for (i = 0; i < PSAMPLE_HASH_SIZE; i++) {
        loc_free(prf->psample_hash[i].buf);
    }
    memset(prf->psample_hash, 0, sizeof(prf->psample_hash));
    prf->psample_cnt = 0;
    for (i = 0; i < STRACE_HASH_SIZE; i++) {
        SampleStackTrace * s = prf->strace_hash[i];
        while (s != NULL) {
            prf->strace_hash[i] = s->next;
            loc_free(s);
            s = prf->strace_hash[i];
        }
    }
}

void profiler_sst_reset(Context * ctx) {
    LINK * l;
    ContextExtensionPrfSST * ext = EXT(ctx);
    for (l = ext->list.next; l != &ext->list; l = l->next) {
        free_buffers(link_core2prf(l));
    }
}

static void profiler_dispose(void * args) {
    ProfilerSST * prf = (ProfilerSST *)args;
    assert(!prf->disposed);
    list_remove(&prf->link_core);
    free_buffers(prf);
    prf->disposed = 1;
    if (!prf->posted) loc_free(prf);
}

static char * profiler_capabilities(Context * ctx) {
    char * res = NULL;
    ByteArrayOutputStream buf;
    OutputStream * out = create_byte_array_output_stream(&buf);

    json_write_string(out, "StackTraces");
    write_stream(out, ':');
    write_stream(out, '{');
    write_stream(out, '}');
    write_stream(out, 0);

    get_byte_array_output_stream_data(&buf, &res, NULL);
    return res;
}

static void * profiler_configure(void * args, Context * ctx, ProfilerParams * params) {
    ProfilerSST * prf = (ProfilerSST *)args;
    ContextExtensionPrfSST * ext = EXT(ctx);
    if (params->frame_cnt > 0) {
        /* Enabled */
        if (prf == NULL) {
            prf = (ProfilerSST *)loc_alloc_zero(sizeof(ProfilerSST));
            if (list_is_empty(&ext->list)) list_init(&ext->list);
            list_add_last(&prf->link_core, &ext->list);
            prf->channel = params->channel;
            prf->ctx = ctx;
        }
        else {
            assert(!prf->disposed);
            free_buffers(prf);
        }
        prf->frame_cnt = params->frame_cnt;
    }
    else {
        /* Disabled */
        if (prf != NULL) {
            profiler_dispose(prf);
            prf = NULL;
        }
    }
    return prf;
}

static void add_num(uint8_t * buf, unsigned * pos, unsigned size, ContextAddress v) {
    unsigned n;
    for (n = 0; n < size; n++) {
        buf[(*pos)++] = (uint8_t)(v >> (n * 8));
    }
}

static void profiler_read(void * args, OutputStream * out) {
    ProfilerSST * prf = (ProfilerSST *)args;
    RegisterDefinition * pc_def = get_PC_definition(prf->ctx);

    assert(!prf->disposed);
    write_stream(out, '{');
    json_write_string(out, "Format");
    write_stream(out, ':');
    json_write_string(out, "StackTraces");
    if (prf->psample_cnt > 0 && pc_def != NULL) {
        unsigned i, j;
        uint8_t * buf = (uint8_t *)tmp_alloc(pc_def->size * (prf->frame_cnt + 2));
        JsonWriteBinaryState state;
        assert(pc_def->size <= sizeof(ContextAddress));
        write_stream(out, ',');
        json_write_string(out, "AddrSize");
        write_stream(out, ':');
        json_write_long(out, pc_def->size);
        write_stream(out, ',');
        json_write_string(out, "Data");
        write_stream(out, ':');
        json_write_binary_start(&state, out, prf->psample_cnt * pc_def->size);
        for (i = 0; i < PSAMPLE_HASH_SIZE; i++) {
            ProfilerSampleArray * arr = prf->psample_hash + i;
            for (j = 0; j < arr->buf_pos; j++) {
                unsigned p = 0;
                unsigned n = 1;
                unsigned m = 1;
                ProfilerSample * s = arr->buf + j;
                add_num(buf, &p, pc_def->size, s->cnt);
                if (s->stk != NULL) m += s->stk->len;
                add_num(buf, &p, pc_def->size, m);
                add_num(buf, &p, pc_def->size, s->pc);
                while (n < m) {
                    add_num(buf, &p, pc_def->size, s->stk->pc[n++ - 1]);
                }
                json_write_binary_data(&state, buf, p);
            }
        }
        json_write_binary_end(&state);
    }
    write_stream(out, '}');
    free_buffers(prf);
}

void profiler_sst_add(Context * ctx) {
    add_profiler(ctx, &profiler_class);
}

static void event_context_created(Context * ctx, void * args) {
    ContextExtensionPrfSST * ext = EXT(ctx);
    if (list_is_empty(&ext->list)) list_init(&ext->list);
}

static void event_context_stopped(Context * ctx, void * args) {
    LINK * l;
    ContextExtensionPrfSST * ext = EXT(ctx);
    for (l = ext->list.next; l != &ext->list; l = l->next) {
        ProfilerSST * prf = link_core2prf(l);
        if (prf->stop_pending) {
            prf->stop_pending = 0;
            add_sample(prf);
        }
    }
}

void ini_profiler_sst(void) {
    static ContextEventListener listener = {
        event_context_created,
        NULL,
        event_context_stopped,
        NULL,
        NULL,
        NULL
    };
    add_context_event_listener(&listener, NULL);
    context_extension_offset = context_extension(sizeof(ContextExtensionPrfSST));
    profiler_class.capabilities = profiler_capabilities;
    profiler_class.configure = profiler_configure;
    profiler_class.dispose = profiler_dispose;
    profiler_class.read = profiler_read;
}

#endif /* ENABLE_ProfilerSST */
