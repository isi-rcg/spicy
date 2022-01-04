/*******************************************************************************
 * Copyright (c) 2007-2017 Wind River Systems, Inc. and others.
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
 * Diagnostics service.
 * This service is used for framework and agents testing.
 */

#include <tcf/config.h>
#include <signal.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <tcf/framework/json.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/cache.h>
#if ENABLE_Symbols
#  include <tcf/services/symbols.h>
#endif
#if SERVICE_Streams
#  include <tcf/services/streamsservice.h>
#endif
#if ENABLE_RCBP_TEST
#  include <tcf/main/test.h>
#  include <tcf/services/runctrl.h>
#endif
#include <tcf/services/diagnostics.h>

static const char * DIAGNOSTICS = "Diagnostics";

#if ENABLE_RCBP_TEST

typedef struct ContextExtensionDiag {
    int test_process;
    Channel * channel;
} ContextExtensionDiag;

static size_t context_extension_offset = 0;

#define EXT(ctx) ((ContextExtensionDiag *)((char *)(ctx) + context_extension_offset))

int is_test_process(Context * ctx) {
#if defined(_WRS_KERNEL)
    return 1;
#else
    return EXT(context_get_group(ctx, CONTEXT_GROUP_PROCESS))->test_process;
#endif
}

static void channel_close_listener(Channel * c) {
    LINK * l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        l = l->next;
        if (ctx->exited) continue;
        if (EXT(ctx)->channel == c || (ctx->creator != NULL && EXT(ctx->creator)->channel == c)) {
            terminate_debug_context(ctx);
        }
    }
    l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        if (EXT(ctx)->channel == c) EXT(ctx)->channel = NULL;
        l = l->next;
    }
}

#endif /* ENABLE_RCBP_TEST */

typedef struct RunTestDoneArgs RunTestDoneArgs;

struct RunTestDoneArgs {
    Channel * c;
    char token[256];
};

static void command_echo(char * token, Channel * c) {
    char str[0x1000];
    int len = json_read_string(&c->inp, str, sizeof(str));
    if ((len < 0) || (len >= (int)sizeof(str))) exception(ERR_JSON_SYNTAX);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    json_write_string_len(&c->out, str, len);
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_echo_fp(char * token, Channel * c) {
    double x = json_read_double(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    json_write_double(&c->out, x);
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_echo_int(char * token, Channel * c) {
    long v0 = 0; unsigned long v1 = 0; int64_t v2 = 0; uint64_t v3 = 0;
    int type = (int)json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    switch (type) {
    case 0: v0 = json_read_long(&c->inp); break;
    case 1: v1 = json_read_ulong(&c->inp); break;
    case 2: v2 = json_read_int64(&c->inp); break;
    case 3: v3 = json_read_uint64(&c->inp); break;
    }
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    switch (type) {
    case 0: json_write_long(&c->out, v0); break;
    case 1: json_write_ulong(&c->out, v1); break;
    case 2: json_write_int64(&c->out, v2); break;
    case 3: json_write_uint64(&c->out, v3); break;
    }
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_echo_err(char * token, Channel * c) {
    int no;
    for (;;) {
        no = read_errno(&c->inp);
        if (peek_stream(&c->inp) == MARKER_EOM) break;
    }
    read_stream(&c->inp);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, no);
    json_write_string(&c->out, errno_to_str(no));
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_test_list(char * token, Channel * c) {
    const char * arr = "[]";
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
#if ENABLE_RCBP_TEST
    arr = "[\"RCBP1\"]";
#endif
    write_stringz(&c->out, arr);
    write_stream(&c->out, MARKER_EOM);
}

#if ENABLE_RCBP_TEST
void test_process_done(Context * ctx) {
    assert(EXT(context_get_group(ctx, CONTEXT_GROUP_PROCESS))->test_process);
    EXT(context_get_group(ctx, CONTEXT_GROUP_PROCESS))->test_process = 0;
    if (!ctx->exited) send_context_changed_event(ctx);
}

static void run_test_done(int error, Context * ctx, void * arg) {
    RunTestDoneArgs * data = (RunTestDoneArgs *)arg;
    Channel * c = data->c;

    if (ctx != NULL) {
        EXT(context_get_group(ctx, CONTEXT_GROUP_PROCESS))->test_process = 1;
        if (!is_channel_closed(c)) EXT(ctx)->channel = c;
        send_context_changed_event(ctx);
    }
    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, data->token);
        write_errno(&c->out, error);
        json_write_string(&c->out, ctx ? ctx->id : NULL);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
    }
    else if (ctx != NULL) {
        terminate_debug_context(ctx);
    }
    channel_unlock_with_msg(c, DIAGNOSTICS);
    loc_free(data);
}
#endif

static void command_run_test(char * token, Channel * c) {
    int err;
    char id[256];

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (strcmp(id, "RCBP1") == 0) {
#if ENABLE_RCBP_TEST
        RunTestDoneArgs * data = (RunTestDoneArgs *)loc_alloc_zero(sizeof(RunTestDoneArgs));
        data->c = c;
        strlcpy(data->token, token, sizeof(data->token));
        channel_lock_with_msg(c, DIAGNOSTICS);
        if (run_test_process(run_test_done, data) == 0) return;
        err = errno;
        channel_unlock_with_msg(c, DIAGNOSTICS);
        loc_free(data);
#else
        err = EINVAL;
#endif
    }
    else {
        err = EINVAL;
    }
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    json_write_string(&c->out, NULL);
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_cancel_test(char * token, Channel * c) {
    char id[256];
    int err;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if ENABLE_RCBP_TEST
    err = (terminate_debug_context(id2ctx(id)) != 0) ? errno : 0;
#else
    err = ERR_UNSUPPORTED;
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void write_symbol(Channel * c, ContextAddress address) {
    if (address == 0) {
        write_stringz(&c->out, "null");
    }
    else {
        write_stream(&c->out, '{');
        json_write_string(&c->out, "Abs");
        write_stream(&c->out, ':');
        json_write_boolean(&c->out, 1);
        write_stream(&c->out, ',');
        json_write_string(&c->out, "Value");
        write_stream(&c->out, ':');
        json_write_uint64(&c->out, address);
        write_stream(&c->out, '}');
        write_stream(&c->out, 0);
    }
}

#if ENABLE_DebugContext

typedef struct GetSymbolArgs {
    char token[256];
    Context * ctx;
    char * name;
} GetSymbolArgs;

static void get_symbol_cache_client(void * x) {
    GetSymbolArgs * args = (GetSymbolArgs *)x;
    Channel * c = cache_channel();
    Context * ctx = args->ctx;
    ContextAddress addr = 0;
    int error = ERR_SYM_NOT_FOUND;

    if (ctx->exited) error = ERR_ALREADY_EXITED;

#if ENABLE_Symbols
    if (get_error_code(error) == ERR_SYM_NOT_FOUND) {
        Symbol * sym = NULL;
        error = 0;
        if (find_symbol_by_name(ctx, STACK_NO_FRAME, 0, args->name, &sym) < 0) error = errno;
        if (!error && get_symbol_address(sym, &addr) < 0) error = errno;
    }
#endif

#if ENABLE_RCBP_TEST
    if (get_error_code(error) == ERR_SYM_NOT_FOUND) {
        void * ptr = NULL;
        int cls = 0;
        error = 0;
        if (find_test_symbol(ctx, args->name, &ptr, &cls) < 0) error = errno;
        addr = (ContextAddress)(uintptr_t)ptr;
    }
#endif

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);
    write_errno(&c->out, error);
    write_symbol(c, addr);
    write_stream(&c->out, MARKER_EOM);

    context_unlock(ctx);
    loc_free(args->name);
}

#endif /* ENABLE_DebugContext */

static void command_get_symbol(char * token, Channel * c) {
    char id[256];
    char * name;
    int error;
    ContextAddress addr = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    name = json_read_alloc_string(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if ENABLE_DebugContext
    {
        Context *ctx = id2ctx(id);
        if (ctx == NULL) {
            error = ERR_INV_CONTEXT;
        }
        else if (ctx->exited) {
            error = ERR_ALREADY_EXITED;
        }
        else {
            GetSymbolArgs args;
            strlcpy(args.token, token, sizeof(args.token));
            context_lock(ctx);
            args.ctx = ctx;
            args.name = name;
            cache_enter(get_symbol_cache_client, c, &args, sizeof(args));
            return;
        }
    }
#else
    error = ERR_UNSUPPORTED;
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, error);
    write_symbol(c, addr);
    write_stream(&c->out, MARKER_EOM);
    loc_free(name);
}

#if SERVICE_Streams

typedef struct StreamsTest {
    VirtualStream * inp;
    VirtualStream * out;
    char buf[111];
    size_t buf_pos;
    size_t buf_len;
    int inp_eos;
    int out_eos;
} StreamsTest;

static void streams_test_callback(VirtualStream * stream, int event_code, void * args) {
    StreamsTest * st = (StreamsTest *)args;

    switch (event_code) {
    case VS_EVENT_SPACE_AVAILABLE:
        if (stream != st->out) return;
        break;
    case VS_EVENT_DATA_AVAILABLE:
        if (stream != st->inp) return;
        break;
    }

    if (st->buf_pos == st->buf_len && !st->inp_eos) {
        st->buf_pos = st->buf_len = 0;
        virtual_stream_get_data(st->inp, st->buf, sizeof(st->buf), &st->buf_len, &st->inp_eos);
    }

    if (st->buf_len > st->buf_pos || st->inp_eos != st->out_eos) {
        size_t done = 0;
        virtual_stream_add_data(st->out, st->buf + st->buf_pos, st->buf_len - st->buf_pos, &done, st->inp_eos);
        st->buf_pos += done;
        if (st->buf_pos == st->buf_len && st->inp_eos) st->out_eos = 1;
    }

    if (st->inp_eos && st->out_eos) loc_free(st);
}

#endif /* SERVICE_Streams */

static void command_create_test_streams(char * token, Channel * c) {
    char id_inp[256];
    char id_out[256];
    long buf_size0;
    long buf_size1;
    int err = 0;

    buf_size0 = json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    buf_size1 = json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (buf_size0 <= 0 || buf_size1 <= 0) err = ERR_INV_NUMBER;

#if SERVICE_Streams
    if (!err) {
        StreamsTest * st = (StreamsTest *)loc_alloc_zero(sizeof(StreamsTest));
        virtual_stream_create(DIAGNOSTICS, NULL, (unsigned)buf_size0,
            VS_ENABLE_REMOTE_WRITE, streams_test_callback, st, &st->inp);
        virtual_stream_create(DIAGNOSTICS, NULL, (unsigned)buf_size1,
            VS_ENABLE_REMOTE_READ, streams_test_callback, st, &st->out);
        virtual_stream_get_id(st->inp, id_inp, sizeof(id_inp));
        virtual_stream_get_id(st->out, id_out, sizeof(id_out));
    }
#else
    if (!err) err = ERR_UNSUPPORTED;
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    if (err) {
        write_stringz(&c->out, "null");
        write_stringz(&c->out, "null");
    }
    else {
        json_write_string(&c->out, id_inp);
        write_stream(&c->out, 0);
        json_write_string(&c->out, id_out);
        write_stream(&c->out, 0);
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_dispose_test_stream(char * token, Channel * c) {
    char id[256];
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if SERVICE_Streams
    if (!err) {
        VirtualStream * stream = virtual_stream_find(id);
        if (stream == NULL) err = errno;
        else virtual_stream_delete(stream);
    }
#else
    err = ERR_UNSUPPORTED;
#endif
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

void ini_diagnostics_service(Protocol * proto) {
    add_command_handler(proto, DIAGNOSTICS, "echo", command_echo);
    add_command_handler(proto, DIAGNOSTICS, "echoFP", command_echo_fp);
    add_command_handler(proto, DIAGNOSTICS, "echoINT", command_echo_int);
    add_command_handler(proto, DIAGNOSTICS, "echoERR", command_echo_err);
    add_command_handler(proto, DIAGNOSTICS, "getTestList", command_get_test_list);
    add_command_handler(proto, DIAGNOSTICS, "runTest", command_run_test);
    add_command_handler(proto, DIAGNOSTICS, "cancelTest", command_cancel_test);
    add_command_handler(proto, DIAGNOSTICS, "getSymbol", command_get_symbol);
    add_command_handler(proto, DIAGNOSTICS, "createTestStreams", command_create_test_streams);
    add_command_handler(proto, DIAGNOSTICS, "disposeTestStream", command_dispose_test_stream);
#if ENABLE_RCBP_TEST
    context_extension_offset = context_extension(sizeof(ContextExtensionDiag));
    add_channel_close_listener(channel_close_listener);
#endif
}
