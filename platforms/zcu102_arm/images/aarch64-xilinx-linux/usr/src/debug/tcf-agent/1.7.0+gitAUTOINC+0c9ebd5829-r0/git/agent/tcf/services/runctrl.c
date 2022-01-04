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
 * Target service implementation: run control (TCF name RunControl)
 */

#include <tcf/config.h>

#if SERVICE_RunControl

#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <tcf/framework/channel.h>
#include <tcf/framework/json.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/signames.h>
#include <tcf/framework/cache.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/contextquery.h>
#include <tcf/services/breakpoints.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/diagnostics.h>
#include <tcf/services/symbols.h>
#include <tcf/main/cmdline.h>

#ifndef EN_STEP_OVER
#  define EN_STEP_OVER (SERVICE_Breakpoints && SERVICE_StackTrace && ENABLE_Symbols)
#endif
#ifndef EN_STEP_LINE
#  define EN_STEP_LINE (ENABLE_LineNumbers)
#endif

#define STOP_ALL_TIMEOUT 1000000
#define STOP_ALL_MAX_CNT 20

#ifndef RC_STEP_MAX_STACK_FRAMES
#define RC_STEP_MAX_STACK_FRAMES 10000
#endif

#ifndef SKIP_PROLOGUE_MAX_STEPS
#define SKIP_PROLOGUE_MAX_STEPS 50
#endif

typedef struct Listener {
    RunControlEventListener * listener;
    void * args;
} Listener;

static Listener * listeners = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;

static const char RUN_CONTROL[] = "RunControl";
static const char CONTEXT_PROXY[] = "ContextProxy";

typedef struct ContextExtensionRC {
    int pending_safe_event; /* safe events are waiting for this context to be stopped */
    int intercepted;        /* context is reported to a host as suspended */
    int intercept_group;
    int reverse_run;
    int step_mode;
    int step_cnt;
    int step_line_cnt;
    int step_repeat_cnt;
    int step_into_hidden;
    int stop_group_mark;
    Context * stop_group_ctx;
    int run_ctrl_ctx_lock_cnt;
    ContextAddress step_range_start;
    ContextAddress step_range_end;
    ContextAddress step_frame_fp;
    ContextAddress step_bp_addr;
    BreakpointInfo * step_bp_info;
    char * step_func_id;
    char * step_func_id_out;
    int step_inlined;
    int step_set_frame_level;
    CodeArea * step_code_area;
    ErrorReport * step_error;
    const char * step_done;
    Channel * step_channel;
    int step_continue_mode;
    int safe_single_step;   /* not zero if the context is performing a "safe" single instruction step */
    int cannot_stop;
    ContextAddress pc;
    int pc_error;
    char * state_name;
    char ** bp_ids;
    unsigned bp_cnt;
    unsigned bp_max;
    int skip_prologue;
    LINK link;
} ContextExtensionRC;

static size_t context_extension_offset = 0;
#define EXT(ctx) (ctx ? ((ContextExtensionRC *)((char *)(ctx) + context_extension_offset)) : NULL)
#define link2ctx(lnk) ((Context *)((char *)(lnk) - offsetof(ContextExtensionRC, link) - context_extension_offset))

typedef struct ChannelExtensionRC {
    int cache_lock;
} ChannelExtensionRC;

static size_t channel_extension_offset = 0;
#define EXT_CH(ch) ((ChannelExtensionRC *)((char *)(ch) + channel_extension_offset))

typedef struct SafeEvent {
    Context * ctx;
    EventCallBack * done;
    void * arg;
    struct SafeEvent * next;
} SafeEvent;

typedef struct GetContextArgs {
    Channel * c;
    char token[256];
    Context * ctx;
} GetContextArgs;

static SafeEvent * safe_event_list = NULL;
static SafeEvent * safe_event_last = NULL;
static int safe_event_active = 0;
static int safe_event_pid_count = 0;
static int run_ctrl_lock_cnt = 0;
static int stop_all_timer_cnt = 0;
static int stop_all_timer_posted = 0;
static int run_safe_events_posted = 0;
static int sync_run_state_event_posted = 0;

static AbstractCache safe_events_cache;

static TCFBroadcastGroup * broadcast_group = NULL;

static void run_safe_events(void * arg);

static int get_resume_modes(Context * ctx) {
    int md, modes;
    int has_state = context_has_state(ctx);

    modes = 0;
    for (md = 0; md < RM_UNDEF; md++) {
        if (md == RM_DETACH) continue;
        if (md == RM_TERMINATE) continue;
        if (md == RM_SKIP_PROLOGUE) continue;
        if (context_can_resume(ctx, md)) modes |= 1 << md;
    }
    if (!has_state) {
        modes &= (1 << RM_RESUME) | (1 << RM_REVERSE_RESUME);
    }
    else {
        if (modes & (1 << RM_STEP_INTO)) {
            modes |= (1 << RM_STEP_INTO_RANGE);
#if EN_STEP_OVER
            modes |= (1 << RM_STEP_OVER);
            modes |= (1 << RM_STEP_OVER_RANGE);
            modes |= (1 << RM_STEP_OUT);
#endif
#if EN_STEP_LINE
            modes |= (1 << RM_STEP_INTO_LINE);
#endif
#if EN_STEP_OVER && EN_STEP_LINE
            modes |= (1 << RM_STEP_OVER_LINE);
#endif
        }
        if (modes & (1 << RM_REVERSE_STEP_INTO)) {
            modes |= (1 << RM_REVERSE_STEP_INTO_RANGE);
#if EN_STEP_OVER
            modes |= (1 << RM_REVERSE_STEP_OVER);
            modes |= (1 << RM_REVERSE_STEP_OVER_RANGE);
            modes |= (1 << RM_REVERSE_STEP_OUT);
#endif
#if EN_STEP_LINE
            modes |= (1 << RM_REVERSE_STEP_INTO_LINE);
#endif
#if EN_STEP_OVER && EN_STEP_LINE
            modes |= (1 << RM_REVERSE_STEP_OVER_LINE);
#endif
        }
    }
    return modes;
}

static void write_context(OutputStream * out, Context * ctx) {
    int modes = get_resume_modes(ctx);
    Context * rc_grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
    Context * bp_grp = context_get_group(ctx, CONTEXT_GROUP_BREAKPOINT);
    Context * ss_grp = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
    Context * cpu_grp = context_get_group(ctx, CONTEXT_GROUP_CPU);

    assert(!ctx->exited);

    write_stream(out, '{');

    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, ctx->id);

    if (ctx->parent != NULL) {
        write_stream(out, ',');
        json_write_string(out, "ParentID");
        write_stream(out, ':');
        json_write_string(out, ctx->parent->id);
    }

    if (ctx->creator != NULL) {
        write_stream(out, ',');
        json_write_string(out, "CreatorID");
        write_stream(out, ':');
        json_write_string(out, ctx->creator->id);
    }

    write_stream(out, ',');
    json_write_string(out, "ProcessID");
    write_stream(out, ':');
    json_write_string(out, context_get_group(ctx, CONTEXT_GROUP_PROCESS)->id);

    if (ctx->name != NULL) {
        write_stream(out, ',');
        json_write_string(out, "Name");
        write_stream(out, ':');
        json_write_string(out, ctx->name);
    }

    write_stream(out, ',');
    json_write_string(out, "CanSuspend");
    write_stream(out, ':');
    json_write_boolean(out, 1);

    write_stream(out, ',');
    json_write_string(out, "CanResume");
    write_stream(out, ':');
    json_write_long(out, modes);

    write_stream(out, ',');
    json_write_string(out, "CanCount");
    write_stream(out, ':');
    json_write_long(out, modes &
        ~(1 << RM_RESUME) &
        ~(1 << RM_REVERSE_RESUME));

    if (context_has_state(ctx)) {
        write_stream(out, ',');
        json_write_string(out, "HasState");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }
    else {
        write_stream(out, ',');
        json_write_string(out, "IsContainer");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }

    write_stream(out, ',');
    json_write_string(out, "WordSize");
    write_stream(out, ':');
    json_write_long(out, context_word_size(ctx));

    if (ctx->reg_access) {
        unsigned cnt = 0;
        write_stream(out, ',');
        json_write_string(out, "RegAccessTypes");
        write_stream(out, ':');
        write_stream(out, '[');
        if (ctx->reg_access & REG_ACCESS_RD_RUNNING) {
            if (cnt++) write_stream(out, ',');
            json_write_string(out, "rd-running");
        }
        if (ctx->reg_access & REG_ACCESS_WR_RUNNING) {
            if (cnt++) write_stream(out, ',');
            json_write_string(out, "wr-running");
        }
        if (ctx->reg_access & REG_ACCESS_RD_STOP) {
            if (cnt++) write_stream(out, ',');
            json_write_string(out, "rd-stop");
        }
        if (ctx->reg_access & REG_ACCESS_WR_STOP) {
            if (cnt++) write_stream(out, ',');
            json_write_string(out, "wr-stop");
        }
        write_stream(out, ']');
    }

    if (context_can_resume(ctx, RM_TERMINATE)) {
        write_stream(out, ',');
        json_write_string(out, "CanTerminate");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }

    if (context_can_resume(ctx, RM_DETACH)) {
        write_stream(out, ',');
        json_write_string(out, "CanDetach");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }

    if (rc_grp != NULL) {
        write_stream(out, ',');
        json_write_string(out, "RCGroup");
        write_stream(out, ':');
        json_write_string(out, rc_grp->id);
    }

    if (bp_grp != NULL) {
        write_stream(out, ',');
        json_write_string(out, "BPGroup");
        write_stream(out, ':');
        json_write_string(out, bp_grp->id);
    }

    if (ss_grp != NULL) {
        write_stream(out, ',');
        json_write_string(out, "SymbolsGroup");
        write_stream(out, ':');
        json_write_string(out, ss_grp->id);
    }

    if (cpu_grp != NULL) {
        write_stream(out, ',');
        json_write_string(out, "CPUGroup");
        write_stream(out, ':');
        json_write_string(out, cpu_grp->id);
    }

#if ENABLE_RCBP_TEST
    if (is_test_process(ctx)) {
        write_stream(out, ',');
        json_write_string(out, "DiagnosticTestProcess");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }
#endif

    if (ctx->big_endian) {
        write_stream(out, ',');
        json_write_string(out, "BigEndian");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }

#if ENABLE_ContextExtraProperties
    {
        /* Back-end context properties */
        int cnt = 0;
        const char ** names = NULL;
        const char ** values = NULL;
        if (context_get_extra_properties(ctx, &names, &values, &cnt) == 0) {
            while (cnt > 0) {
                if (*values != NULL) {
                    write_stream(out, ',');
                    json_write_string(out, *names);
                    write_stream(out, ':');
                    write_string(out, *values);
                }
                names++;
                values++;
                cnt--;
            }
        }
    }
#endif

    write_stream(out, '}');
}

static void get_current_pc(Context * ctx) {
    size_t i;
    uint8_t buf[8];
    ContextExtensionRC * ext = EXT(ctx);
    RegisterDefinition * def = get_PC_definition(ctx);
    ext->pc = 0;
    ext->pc_error = 0;
    if (def == NULL) {
        ext->pc_error = set_errno(ERR_OTHER, "Program counter register not found");
        return;
    }
    if ((ctx->reg_access & REG_ACCESS_RD_RUNNING) == 0) {
        if (!ctx->stopped && context_has_state(ctx)) {
            ext->pc_error = ERR_IS_RUNNING;
            return;
        }
    }
    assert(def->size <= sizeof(buf));
    if (context_read_reg(ctx, def, 0, def->size, buf) < 0) {
        ext->pc_error = errno;
        return;
    }
    for (i = 0; i < def->size; i++) {
        ext->pc = ext->pc << 8;
        ext->pc |= buf[def->big_endian ? i : def->size - i - 1];
    }
}

#if ENABLE_ContextStateProperties
static int is_json(const char * s) {
    Trap trap;
    if (set_trap(&trap)) {
        ByteArrayInputStream buf;
        InputStream * inp = create_byte_array_input_stream(&buf, s, strlen(s));
        json_skip_object(inp);
        json_test_char(inp, MARKER_EOS);
        clear_trap(&trap);
    }
    return trap.error == 0;
}
#endif

static const char * get_suspend_reason(Context * ctx) {
    const char * reason = NULL;
    ContextExtensionRC * ext = EXT(ctx);
    if (ext->bp_cnt > 0) return REASON_BREAKPOINT;
    if (ctx->exception_description != NULL) return ctx->exception_description;
    if (ext->step_error != NULL) return errno_to_str(set_error_report_errno(ext->step_error));
    if (ext->step_done != NULL) return ext->step_done;
    reason = context_suspend_reason(ctx);
    if (reason != NULL) return reason;
    return REASON_USER_REQUEST;
}

static void write_context_state(OutputStream * out, Context * ctx) {
    int fst = 1;
    ContextExtensionRC * ext = EXT(ctx);

    assert(!ctx->exited);

    if (!ext->intercepted) {
        write_stringz(out, "0");
        write_stringz(out, "null");
    }
    else {

        /* Number: PC */
        json_write_uint64(out, ext->pc);
        write_stream(out, 0);

        /* String: Reason */
        json_write_string(out, get_suspend_reason(ctx));
        write_stream(out, 0);
    }

    /* Object: Additional context state info */
    write_stream(out, '{');
    if (ext->intercepted) {
        if (ext->step_error == NULL && ext->step_done == NULL && ctx->signal) {
            const char * name = signal_name(ctx->signal);
            const char * desc = signal_description(ctx->signal);
            json_write_string(out, "Signal");
            write_stream(out, ':');
            json_write_long(out, ctx->signal);
            if (name != NULL) {
                write_stream(out, ',');
                json_write_string(out, "SignalName");
                write_stream(out, ':');
                json_write_string(out, name);
            }
            if (desc != NULL) {
                write_stream(out, ',');
                json_write_string(out, "SignalDescription");
                write_stream(out, ':');
                json_write_string(out, desc);
            }
            fst = 0;
        }
        if (ext->bp_cnt > 0) {
            unsigned i = 0;
            if (!fst) write_stream(out, ',');
            json_write_string(out, "BPs");
            write_stream(out, ':');
            write_stream(out, '[');
            for (i = 0; i < ext->bp_cnt; i++) {
                if (i > 0) write_stream(out, ',');
                json_write_string(out, ext->bp_ids[i]);
            }
            write_stream(out, ']');
            fst = 0;
        }
        if (ext->pc_error) {
            if (!fst) write_stream(out, ',');
            json_write_string(out, "PCError");
            write_stream(out, ':');
            write_error_object(out, ext->pc_error);
            fst = 0;
        }
        if (ext->step_error != NULL) {
            if (!fst) write_stream(out, ',');
            json_write_string(out, "StepError");
            write_stream(out, ':');
            write_error_object(out, set_error_report_errno(ext->step_error));
            fst = 0;
        }
        if (ctx->stopped_by_funccall) {
            if (!fst) write_stream(out, ',');
            json_write_string(out, "FuncCall");
            write_stream(out, ':');
            json_write_boolean(out, 1);
            fst = 0;
        }
    }
    else {
        if (ext->state_name != NULL) {
            json_write_string(out, "StateName");
            write_stream(out, ':');
            json_write_string(out, ext->state_name);
            fst = 0;
        }
    }
#if ENABLE_ContextStateProperties
    {
        /* Back-end context state properties */
        int cnt = 0;
        const char ** names = NULL;
        const char ** values = NULL;
        if (context_get_state_properties(ctx, &names, &values, &cnt) == 0) {
            while (cnt > 0) {
                if (*values != NULL) {
                    if (!fst) write_stream(out, ',');
                    json_write_string(out, *names);
                    write_stream(out, ':');
                    /* In older versions of the API, value was a simple string.
                     * Latest version requires it to be in JSON.
                     * For backward compatibility, check if the value is in JSON. */
                    if (is_json(*values)) write_string(out, *values);
                    else json_write_string(out, *values);
                    fst = 0;
                }
                names++;
                values++;
                cnt--;
            }
        }
    }
#endif
    write_stream(out, '}');
    write_stream(out, 0);
}

static void command_get_context(char * token, Channel * c) {
    int err = 0;
    char id[256];
    Context * ctx = NULL;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    ctx = id2ctx(id);

    if (ctx == NULL) err = ERR_INV_CONTEXT;
    else if (ctx->exited) err = ERR_ALREADY_EXITED;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    if (err == 0) {
        write_context(&c->out, ctx);
        write_stream(&c->out, 0);
    }
    else {
        write_stringz(&c->out, "null");
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_children(char * token, Channel * c) {
    char id[256];

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    write_errno(&c->out, 0);

    write_stream(&c->out, '[');
    if (id[0] == 0) {
        LINK * l;
        int cnt = 0;
        for (l = context_root.next; l != &context_root; l = l->next) {
            Context * ctx = ctxl2ctxp(l);
            if (ctx->parent != NULL) continue;
            if (ctx->exited) continue;
            if (cnt > 0) write_stream(&c->out, ',');
            json_write_string(&c->out, ctx->id);
            cnt++;
        }
    }
    else {
        Context * parent = id2ctx(id);
        if (parent != NULL) {
            LINK * l;
            int cnt = 0;
            for (l = parent->children.next; l != &parent->children; l = l->next) {
                Context * ctx = cldl2ctxp(l);
                assert(ctx->parent == parent);
                if (ctx->exited) continue;
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, ctx->id);
                cnt++;
            }
        }
    }
    write_stream(&c->out, ']');
    write_stream(&c->out, 0);

    write_stream(&c->out, MARKER_EOM);
}

typedef struct CommandGetStateArgs {
    char token[256];
    char id[256];
} CommandGetStateArgs;

static void command_get_state_cache_client(void * x) {
    CommandGetStateArgs * args = (CommandGetStateArgs *)x;
    Context * ctx = NULL;
    ContextExtensionRC * ext = NULL;
    Channel * c = cache_channel();
    int err = 0;

    ctx = id2ctx(args->id);
    if (ctx != NULL) ext = EXT(ctx);

    if (ctx == NULL || !context_has_state(ctx)) err = ERR_INV_CONTEXT;
    else if (ctx->exited) err = ERR_ALREADY_EXITED;

    if (!err && ext != NULL && ext->intercepted) get_current_pc(ctx);

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);

    write_errno(&c->out, err);

    json_write_boolean(&c->out, ext != NULL && ext->intercepted);
    write_stream(&c->out, 0);

    if (err) {
        write_stringz(&c->out, "0");
        write_stringz(&c->out, "null");
        write_stringz(&c->out, "null");
    }
    else {
        write_context_state(&c->out, ctx);
    }

    write_stream(&c->out, MARKER_EOM);
}

static void command_get_state(char * token, Channel * c) {
    CommandGetStateArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_state_cache_client, c, &args, sizeof(args));
}

#if ENABLE_ContextISA

typedef struct CommandGetISAArgs {
    char token[256];
    char id[256];
    ContextAddress addr;
} CommandGetISAArgs;

static void command_get_isa_cache_client(void * x) {
    CommandGetISAArgs * args = (CommandGetISAArgs *)x;
    Context * ctx = NULL;
    Channel * c = cache_channel();
    ContextISA isa;
    int err = 0;

    ctx = id2ctx(args->id);

    memset(&isa, 0, sizeof(isa));
    if (ctx == NULL) err = ERR_INV_CONTEXT;
    else if (ctx->exited) err = ERR_ALREADY_EXITED;
    else if (context_get_isa(ctx, args->addr, &isa) < 0) err = errno;

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);

    write_errno(&c->out, err);

    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        write_stream(&c->out, '{');
        json_write_string(&c->out, "Addr");
        write_stream(&c->out, ':');
        json_write_uint64(&c->out, isa.addr);
        write_stream(&c->out, ',');
        json_write_string(&c->out, "Size");
        write_stream(&c->out, ':');
        json_write_uint64(&c->out, isa.size);
        write_stream(&c->out, ',');
        json_write_string(&c->out, "Alignment");
        write_stream(&c->out, ':');
        json_write_uint64(&c->out, isa.alignment);
        write_stream(&c->out, ',');
        json_write_string(&c->out, "MaxInstrSize");
        write_stream(&c->out, ':');
        json_write_uint64(&c->out, isa.max_instruction_size);
        if (isa.def != NULL) {
            write_stream(&c->out, ',');
            json_write_string(&c->out, "DefISA");
            write_stream(&c->out, ':');
            json_write_string(&c->out, isa.def);
        }
        if (isa.isa != NULL) {
            write_stream(&c->out, ',');
            json_write_string(&c->out, "ISA");
            write_stream(&c->out, ':');
            json_write_string(&c->out, isa.isa);
        }
        write_stream(&c->out, '}');
        write_stream(&c->out, 0);
    }

    write_stream(&c->out, MARKER_EOM);
}

static void command_get_isa(char * token, Channel * c) {
    CommandGetISAArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    args.addr = (ContextAddress)json_read_uint64(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_isa_cache_client, c, &args, sizeof(args));
}

#endif /* ENABLE_ContextISA */

static void send_simple_result(Channel * c, char * token, int err) {
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void send_event_context_resumed(Context * ctx);

typedef struct ResumeParams {
    ContextAddress range_start;
    ContextAddress range_end;
    int step_into_hidden;
    int error;
} ResumeParams;

static void resume_params_callback(InputStream * inp, const char * name, void * x) {
    ResumeParams * args = (ResumeParams *)x;
    if (strcmp(name, "RangeStart") == 0) args->range_start = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "RangeEnd") == 0) args->range_end = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "StepIntoHidden") == 0) args->step_into_hidden = json_read_boolean(inp);
    else {
        json_skip_object(inp);
        args->error = ERR_UNSUPPORTED;
    }
}

static int resume_context_tree(Context * ctx) {
    if (!context_has_state(ctx)) {
        LINK * l;
        for (l = ctx->children.next; l != &ctx->children; l = l->next) {
            Context * x = cldl2ctxp(l);
            if (!x->exited) resume_context_tree(x);
        }
    }
    else if (EXT(ctx)->intercepted) {
        Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
        send_event_context_resumed(grp);
        assert(!EXT(ctx)->intercepted);
        if (run_ctrl_lock_cnt == 0 && run_safe_events_posted < 4) {
            run_safe_events_posted++;
            post_event(run_safe_events, NULL);
        }
    }
    return 0;
}

static void free_code_area(CodeArea * area) {
    loc_free(area->directory);
    loc_free(area->file);
    loc_free(area);
}

static void cancel_step_mode(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);

    if (ext->step_channel) {
        channel_unlock_with_msg(ext->step_channel, RUN_CONTROL);
        ext->step_channel = NULL;
    }
#if SERVICE_Breakpoints
    if (ext->step_bp_info != NULL) {
        destroy_eventpoint(ext->step_bp_info);
        ext->step_bp_info = NULL;
    }
#endif
    if (ext->step_code_area != NULL) {
        free_code_area(ext->step_code_area);
        ext->step_code_area = NULL;
    }
    if (ext->step_func_id != NULL) {
        loc_free(ext->step_func_id);
        ext->step_func_id = NULL;
    }
    if (ext->step_func_id_out != NULL) {
        loc_free(ext->step_func_id_out);
        ext->step_func_id_out = NULL;
    }
    ext->step_cnt = 0;
    ext->step_line_cnt = 0;
    ext->step_repeat_cnt = 0;
    ext->step_into_hidden = 0;
    ext->step_range_start = 0;
    ext->step_range_end = 0;
    ext->step_frame_fp = 0;
    ext->step_bp_addr = 0;
    ext->step_inlined = 0;
    ext->step_set_frame_level = 0;
    ext->step_continue_mode = RM_RESUME;
    ext->step_mode = RM_RESUME;
}

static void start_step_mode(Context * ctx, Channel * c, int mode, int cnt, ContextAddress range_start, ContextAddress range_end) {
    ContextExtensionRC * ext = EXT(ctx);

    cancel_step_mode(ctx);
    if (ext->step_error) {
        release_error_report(ext->step_error);
        ext->step_error = NULL;
    }
    assert(ext->step_channel == NULL);
    if (mode == RM_RESUME || mode == RM_REVERSE_RESUME ||
        mode == RM_TERMINATE || mode == RM_DETACH) c = NULL;
    if (c != NULL) {
        ext->step_channel = c;
        channel_lock_with_msg(ext->step_channel, RUN_CONTROL);
    }
    ext->step_done = NULL;
    ext->step_mode = mode;
    ext->step_range_start = range_start;
    ext->step_range_end = range_end;
    ext->step_repeat_cnt = cnt;
    ext->step_into_hidden = 0;
}

int get_stepping_mode(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    return ext->step_mode;
}

int continue_debug_context(Context * ctx, Channel * c,
        int mode, int count, ContextAddress range_start, ContextAddress range_end) {
    ContextExtensionRC * ext = EXT(ctx);
    Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
    int err = 0;

    if (ctx->exited) {
        err = ERR_ALREADY_EXITED;
    }
    else if (context_has_state(ctx) && !ext->intercepted) {
        err = ERR_ALREADY_RUNNING;
    }
    else if (count < 1) {
        err = EINVAL;
    }

    if (!err) {
        EXT(grp)->reverse_run = 0;
        switch (mode) {
        case RM_REVERSE_RESUME:
        case RM_REVERSE_STEP_OVER:
        case RM_REVERSE_STEP_INTO:
        case RM_REVERSE_STEP_OVER_LINE:
        case RM_REVERSE_STEP_INTO_LINE:
        case RM_REVERSE_STEP_OUT:
        case RM_REVERSE_STEP_OVER_RANGE:
        case RM_REVERSE_STEP_INTO_RANGE:
        case RM_REVERSE_UNTIL_ACTIVE:
            EXT(grp)->reverse_run = 1;
            break;
        }

        if (context_has_state(ctx)) start_step_mode(ctx, c, mode, count, range_start, range_end);

        if (resume_context_tree(ctx) < 0) {
            err = errno;
            cancel_step_mode(ctx);
        }
    }

    assert(err || !ext->intercepted);
    if (err) {
        errno = err;
        return -1;
    }

    return 0;
}

static void command_resume(char * token, Channel * c) {
    char id[256];
    int mode;
    long count;
    int err = 0;
    ResumeParams args;
    Context * ctx = NULL;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    mode = (int)json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    count = json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    if (peek_stream(&c->inp) != MARKER_EOM) {
        json_read_struct(&c->inp, resume_params_callback, &args);
        json_test_char(&c->inp, MARKER_EOA);
        err = args.error;
    }
    json_test_char(&c->inp, MARKER_EOM);

    if (err == 0 && (ctx = id2ctx(id)) == NULL) err = ERR_INV_CONTEXT;
    if (err == 0 && ((mode >= RM_UNDEF) || ((get_resume_modes(ctx) & (1 << mode)) == 0))) err = EINVAL;
    if (err == 0 && continue_debug_context(ctx, c, mode, count, args.range_start, args.range_end) < 0) err = errno;
    if (err == 0 && args.step_into_hidden && context_has_state(ctx)) EXT(ctx)->step_into_hidden = 1;
    send_simple_result(c, token, err);
}

int suspend_debug_context(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);

    if (ctx->exited) {
        /* do nothing */
    }
    else if (context_has_state(ctx)) {
        assert(!ctx->stopped || !ext->safe_single_step);
        if (!ctx->stopped) {
            assert(!ext->intercepted);
            if (!ctx->exiting) {
                ctx->pending_intercept = 1;
                if (!ext->safe_single_step && context_stop(ctx) < 0) return -1;
            }
        }
        else if (!ext->intercepted) {
            ctx->pending_intercept = 1;
            if (run_ctrl_lock_cnt == 0 && run_safe_events_posted < 4) {
                run_safe_events_posted++;
                post_event(run_safe_events, NULL);
            }
        }
    }
    else {
        LINK * l;
        for (l = ctx->children.next; l != &ctx->children; l = l->next) {
            suspend_debug_context(cldl2ctxp(l));
        }
    }
    return 0;
}

int suspend_by_breakpoint(Context * ctx, Context * trigger, const char * bp, int skip_prologue) {
    ContextExtensionRC * ext = EXT(ctx);

    if (ctx->exited) {
        /* do nothing */
    }
    else if (context_has_state(ctx)) {
        assert(!ctx->stopped || !ext->safe_single_step);
        if (!ext->intercepted && bp != NULL && ctx == trigger) {
            unsigned i = 0;
            while (i < ext->bp_cnt && strcmp(ext->bp_ids[i], bp)) i++;
            if (i >= ext->bp_cnt) {
                if (ext->bp_cnt + 2 > ext->bp_max) {
                    ext->bp_max += 8;
                    ext->bp_ids = (char **)loc_realloc(ext->bp_ids, ext->bp_max * sizeof(char *));
                }
                ext->bp_ids[ext->bp_cnt++] = loc_strdup(bp);
                ext->bp_ids[ext->bp_cnt] = NULL;
            }
        }
        if (!ctx->stopped) {
            assert(!ext->intercepted);
            if (!ctx->exiting) {
                if (skip_prologue) ext->skip_prologue = 1;
                else ctx->pending_intercept = 1;
                if (!ext->safe_single_step && context_stop(ctx) < 0) return -1;
            }
        }
        else if (!ext->intercepted) {
            if (skip_prologue) ext->skip_prologue = 1;
            else ctx->pending_intercept = 1;
            if (run_ctrl_lock_cnt == 0 && run_safe_events_posted < 4) {
                run_safe_events_posted++;
                post_event(run_safe_events, NULL);
            }
        }
    }
    else {
        LINK * l;
        for (l = ctx->children.next; l != &ctx->children; l = l->next) {
            suspend_by_breakpoint(cldl2ctxp(l), trigger, bp, skip_prologue);
        }
    }
    return 0;
}

char ** get_context_breakpoint_ids(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    if (!ext->intercepted) return NULL;
    if (ext->bp_cnt == 0) return NULL;
    return ext->bp_ids;
}

typedef struct CommandSuspendArgs {
    char token[256];
    char id[256];
} CommandSuspendArgs;

static void command_suspend_cache_client(void * x) {
    CommandSuspendArgs * args = (CommandSuspendArgs *)x;
    Channel * c = cache_channel();
    Context * ctx = NULL;
    int err = 0;

    ctx = id2ctx(args->id);

    if (ctx == NULL) {
        err = ERR_INV_CONTEXT;
    }
    else if (ctx->exited) {
        err = ERR_ALREADY_EXITED;
    }
    else {
        ContextExtensionRC * ext = EXT(ctx);
        if (ext->intercepted) {
            err = ERR_ALREADY_STOPPED;
        }
        else if (suspend_debug_context(ctx) < 0) {
            err = errno;
        }
    }

    cache_exit();

    send_simple_result(c, args->token, err);
}

static void command_suspend(char * token, Channel * c) {
    CommandSuspendArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_suspend_cache_client, c, &args, sizeof(args));
}

static void terminate_context_tree(Context * ctx) {
    if (ctx->exited) return;
    if (context_can_resume(ctx, RM_TERMINATE)) {
        ContextExtensionRC * ext = EXT(ctx);
        cancel_step_mode(ctx);
        ctx->pending_intercept = 0;
        ext->step_mode = RM_TERMINATE;
        resume_context_tree(ctx);
    }
    else if (EXT(ctx)->intercepted) {
        ContextExtensionRC * ext = EXT(ctx);
        cancel_step_mode(ctx);
        ctx->pending_intercept = 0;
        ext->step_mode = RM_RESUME;
        resume_context_tree(ctx);
    }
    else {
        LINK * l = ctx->children.next;
        while (l != &ctx->children) {
            Context * x = cldl2ctxp(l);
            terminate_context_tree(x);
            l = l->next;
        }
    }
}

static void event_terminate(void * args) {
    Context * ctx = (Context *)args;
    terminate_context_tree(ctx);
    context_unlock(ctx);
}

int terminate_debug_context(Context * ctx) {
    int err = 0;
    if (ctx == NULL) {
        err = ERR_INV_CONTEXT;
    }
    else if (ctx->exited) {
        err = ERR_ALREADY_EXITED;
    }
    else {
        context_lock(ctx);
        post_safe_event(ctx, event_terminate, ctx);
    }
    if (err) {
        errno = err;
        return -1;
    }
    return 0;
}

static void command_terminate(char * token, Channel * c) {
    char id[256];
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (terminate_debug_context(id2ctx(id)) != 0) err = errno;

    send_simple_result(c, token, err);
}

static void detach_context_tree(Context * ctx) {
    if (ctx->exited) return;
    if (context_can_resume(ctx, RM_DETACH)) {
        ContextExtensionRC * ext = EXT(ctx);
        cancel_step_mode(ctx);
        ctx->pending_intercept = 0;
        ext->step_mode = RM_DETACH;
        resume_context_tree(ctx);
    }
    else {
        LINK * l = ctx->children.next;
        while (l != &ctx->children) {
            Context * x = cldl2ctxp(l);
            detach_context_tree(x);
            l = l->next;
        }
    }
}

static void event_detach(void * args) {
    Context * ctx = (Context *)args;
    detach_context_tree(ctx);
    context_unlock(ctx);
}

int detach_debug_context(Context * ctx) {
    int err = 0;
    if (ctx == NULL) {
        err = ERR_INV_CONTEXT;
    }
    else if (ctx->exited) {
        err = ERR_ALREADY_EXITED;
    }
    else {
        context_lock(ctx);
        post_safe_event(ctx, event_detach, ctx);
    }
    if (err) {
        errno = err;
        return -1;
    }
    return 0;
}

static void command_detach(char * token, Channel * c) {
    char id[256];
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (detach_debug_context(id2ctx(id)) != 0) err = errno;

    send_simple_result(c, token, err);
}

static void notify_context_released(Context * ctx) {
    unsigned i;
    ContextExtensionRC * ext = EXT(ctx);
    assert(ext->intercepted);
    ext->intercepted = 0;
    ext->skip_prologue = 0;
    while (ext->bp_cnt > 0) loc_free(ext->bp_ids[--ext->bp_cnt]);
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->listener->context_released == NULL) continue;
        l->listener->context_released(ctx, l->args);
    }
}

static void send_event_context_added(Context * ctx) {
    OutputStream * out = &broadcast_group->out;

    write_stringz(out, "E");
    write_stringz(out, RUN_CONTROL);
    write_stringz(out, "contextAdded");

    /* <array of context data> */
    write_stream(out, '[');
    write_context(out, ctx);
    write_stream(out, ']');
    write_stream(out, 0);

    write_stream(out, MARKER_EOM);
}

static void send_event_context_changed(Context * ctx) {
    OutputStream * out = &broadcast_group->out;

    write_stringz(out, "E");
    write_stringz(out, RUN_CONTROL);
    write_stringz(out, "contextChanged");

    /* <array of context data> */
    write_stream(out, '[');
    write_context(out, ctx);
    write_stream(out, ']');
    write_stream(out, 0);

    write_stream(out, MARKER_EOM);
}

static void send_event_context_removed(Context * ctx) {
    OutputStream * out = &broadcast_group->out;
    ContextExtensionRC * ext = EXT(ctx);

    if (ext->intercepted) notify_context_released(ctx);

    write_stringz(out, "E");
    write_stringz(out, RUN_CONTROL);
    write_stringz(out, "contextRemoved");

    /* <array of context IDs> */
    write_stream(out, '[');
    json_write_string(out, ctx->id);
    write_stream(out, ']');
    write_stream(out, 0);

    write_stream(out, MARKER_EOM);
}

static void send_event_context_suspended(void) {
    LINK p0; /* List of contexts intercepted by breakpoint or exception */
    LINK p1; /* List of all other intercepted contexts */
    LINK p2; /* List for notify_context_intercepted() */
    LINK * l = context_root.next;
    unsigned i;

    list_init(&p0);
    list_init(&p1);
    list_init(&p2);

    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        l = l->next;
        if (ctx->pending_intercept && ctx->stopped) {
            LINK * n = &p1;
            ContextExtensionRC * ext = EXT(ctx);
            assert(!ctx->exited);
            assert(!ext->intercepted);
            assert(!ext->safe_single_step);
            cancel_step_mode(ctx);
            assert(!ext->intercepted);
            ext->intercepted = 1;
            ctx->pending_intercept = 0;
            if (strcmp(get_suspend_reason(ctx), REASON_USER_REQUEST)) n = &p0;
            list_add_last(&ext->link, n);
        }
    }

    while (!list_is_empty(&p0) || !list_is_empty(&p1)) {
        OutputStream * out = &broadcast_group->out;
        LINK * n = !list_is_empty(&p0) ? p0.next : p1.next;
        Context * ctx = link2ctx(n);
        int container = list_is_empty(&p0) && p1.next != p1.prev;

        list_remove(n);
        list_add_last(n, &p2);

        write_stringz(out, "E");
        write_stringz(out, RUN_CONTROL);
        write_stringz(out, container ? "containerSuspended" : "contextSuspended");

        json_write_string(out, ctx->id);
        write_stream(out, 0);

        write_context_state(out, ctx);

        if (container) {
            write_stream(out, '[');
            json_write_string(out, ctx->id);
            assert(!list_is_empty(&p1));
            while (!list_is_empty(&p1)) {
                LINK * m = p1.next;
                Context * x = link2ctx(m);
                list_remove(m);
                list_add_last(m, &p2);
                write_stream(out, ',');
                json_write_string(out, x->id);
            }
            write_stream(out, ']');
            write_stream(out, 0);
            assert(list_is_empty(&p1));
        }

        write_stream(out, MARKER_EOM);
    }

    l = p2.next;
    while (l != &p2) {
        Context * x = link2ctx(l);
        l = l->next;
        for (i = 0; i < listener_cnt; i++) {
            Listener * ls = listeners + i;
            if (ls->listener->context_intercepted == NULL) continue;
            ls->listener->context_intercepted(x, ls->args);
        }
        assert(x->pending_intercept == 0);
    }
}

static void send_event_context_resumed(Context * grp) {
    LINK * l = NULL;
    LINK p;

    list_init(&p);
    l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        if (ext->intercepted && context_get_group(ctx, CONTEXT_GROUP_INTERCEPT) == grp) {
            assert(!ctx->pending_intercept);
            assert(!ext->safe_single_step);
            notify_context_released(ctx);
            list_add_last(&ext->link, &p);
        }
        l = l->next;
    }

    if (!list_is_empty(&p)) {
        OutputStream * out = &broadcast_group->out;

        write_stringz(out, "E");
        write_stringz(out, RUN_CONTROL);

        if (p.next == p.prev) {
            Context * ctx = link2ctx(p.next);
            write_stringz(out, "contextResumed");
            json_write_string(out, ctx->id);
        }
        else {
            l = p.next;
            write_stringz(out, "containerResumed");
            write_stream(out, '[');
            while (l != &p) {
                Context * ctx = link2ctx(l);
                if (l != p.next) write_stream(out, ',');
                json_write_string(out, ctx->id);
                l = l->next;
            }
            write_stream(out, ']');
        }
        write_stream(out, 0);

        write_stream(out, MARKER_EOM);
    }
}

static void send_event_context_exception(Context * ctx) {
    OutputStream * out = &broadcast_group->out;
    const char * msg = NULL;

    write_stringz(out, "E");
    write_stringz(out, RUN_CONTROL);
    write_stringz(out, "contextException");

    /* String: Context ID */
    json_write_string(out, ctx->id);
    write_stream(out, 0);

    /* String: Human-readable description of the exception */
    if (ctx->exception_description) {
        msg = ctx->exception_description;
    }
    else if (ctx->signal > 0) {
        const char * desc = signal_description(ctx->signal);
        if (desc == NULL) desc = signal_name(ctx->signal);
        if (desc == NULL) msg = tmp_printf("Signal %d", ctx->signal);
        else msg = tmp_printf("Signal %d: %s", ctx->signal, desc);
    }
    else {
        msg = context_suspend_reason(ctx);
    }
    json_write_string(out, msg);
    write_stream(out, 0);

    write_stream(out, MARKER_EOM);
}

int is_ctx_stopped(Context * ctx) {
    if (ctx == NULL) {
        errno = ERR_INV_CONTEXT;
        return 0;
    }
    if (ctx->stopped) return 1;
    if (ctx->exited) {
        errno = ERR_ALREADY_EXITED;
        return 0;
    }
    if (!context_has_state(ctx)) {
        errno = ERR_INV_CONTEXT;
        return 0;
    }
    if (EXT(ctx)->state_name) {
        set_fmt_errno(ERR_OTHER, "Context %s state: %s", ctx->name ? ctx->name : ctx->id, get_context_state_name(ctx));
        return 0;
    }
    errno = ERR_IS_RUNNING;
    return 0;
}

int is_all_stopped(Context * grp) {
    LINK * l;
    grp = context_get_group(grp, CONTEXT_GROUP_STOP);
    for (l = context_root.next; l != &context_root; l = l->next) {
        Context * ctx = ctxl2ctxp(l);
        if (ctx->stopped || ctx->exited || ctx->exiting) continue;
        if (!context_has_state(ctx)) continue;
        if (context_get_group(ctx, CONTEXT_GROUP_STOP) != grp) continue;
        if (EXT(ctx)->state_name) {
            set_fmt_errno(ERR_OTHER, "Context %s state: %s", ctx->name ? ctx->name : ctx->id, get_context_state_name(ctx));
            return 0;
        }
        errno = ERR_IS_RUNNING;
        return 0;
    }
    return 1;
}

static int is_all_stopped_or_cannot_stop(Context * grp) {
    LINK * l;
    grp = context_get_group(grp, CONTEXT_GROUP_STOP);
    for (l = context_root.next; l != &context_root; l = l->next) {
        Context * ctx = ctxl2ctxp(l);
        if (ctx->stopped || ctx->exited || ctx->exiting) continue;
        if (EXT(ctx)->cannot_stop) continue;
        if (!context_has_state(ctx)) continue;
        if (context_get_group(ctx, CONTEXT_GROUP_STOP) != grp) continue;
        return 0;
    }
    return 1;
}

int is_intercepted(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    return ext->intercepted;
}

#if EN_STEP_LINE

static int is_same_line(CodeArea * x, CodeArea * y) {
    if (x == NULL || y == NULL) return 0;
    if (x->start_line != y->start_line) return 0;
    if (x->directory != y->directory && (x->directory == NULL || strcmp(x->directory, y->directory))) return 0;
    if (x->file != y->file && (x->file == NULL || strcmp(x->file, y->file))) return 0;
    return 1;
}

static void update_step_machine_code_area(CodeArea * area, void * args) {
    ContextExtensionRC * ext = (ContextExtensionRC *)args;
    if (ext->step_code_area != NULL) return;
    ext->step_code_area = (CodeArea *)loc_alloc(sizeof(CodeArea));
    *ext->step_code_area = *area;
    if (area->directory != NULL) ext->step_code_area->directory = loc_strdup(area->directory);
    if (area->file != NULL) ext->step_code_area->file = loc_strdup(area->file);
}

static int is_function_prologue(Context * ctx, ContextAddress ip, CodeArea * area) {
#if ENABLE_Symbols
    Symbol * sym = NULL;
    int sym_class = SYM_CLASS_UNKNOWN;
    ContextAddress sym_addr = 0;
    ContextAddress sym_size = 0;
    assert(ip >= area->start_address && ip < area->end_address);
    if (area->prologue_end) return 0;
    if (find_symbol_by_addr(ctx, STACK_NO_FRAME, ip, &sym) < 0) return 0;
    if (get_symbol_class(sym, &sym_class) < 0) return 0;
    if (sym_class != SYM_CLASS_FUNCTION) return 0;
    if (get_symbol_size(sym, &sym_size) < 0) return 0;
    if (sym_size == 0) return 0;
    if (get_symbol_address(sym, &sym_addr) < 0) return 0;
    if (sym_addr >= area->start_address && sym_addr < area->end_address) return 1;
#endif
    return 0;
}

static int is_hidden_function(Context * ctx, ContextAddress ip,
        ContextAddress * addr0, ContextAddress * addr1) {
#if ENABLE_Symbols
    Symbol * sym = NULL;
    char * name = NULL;
    ContextAddress sym_addr = 0;
    ContextAddress sym_size = 0;
    SymbolFileInfo * file = NULL;
    HIDDEN_HOOK;
    if (find_symbol_by_addr(ctx, STACK_NO_FRAME, ip, &sym) == 0 &&
            get_symbol_name(sym, &name) == 0 && name != NULL &&
            ((strcmp(name, "__i686.get_pc_thunk.bx") == 0) ||
             (strcmp(name, "__x86.get_pc_thunk.bx") == 0)) &&
            get_symbol_address(sym, &sym_addr) == 0) {
        if (get_symbol_size(sym, &sym_size) < 0 || sym_size == 0) {
            *addr0 = ip;
            *addr1 = ip + 1;
        }
        else {
            *addr0 = sym_addr;
            *addr1 = sym_addr + sym_size;
        }
        return 1;
    }
    if (get_symbol_file_info(ctx, ip, &file) == 0 && file != NULL && file->dyn_loader) {
        *addr0 = file->addr;
        *addr1 = file->addr + file->size;
        return 1;
    }
#endif
    return 0;
}

static void get_machine_code_area(CodeArea * area, void * args) {
    *(CodeArea **)args = (CodeArea *)tmp_alloc(sizeof(CodeArea));
    memcpy(*(CodeArea **)args, area, sizeof(CodeArea));
}

#if EN_STEP_OVER

static int is_within_function_epilogue(Context * ctx, ContextAddress ip) {
    Symbol * sym = NULL;
    int sym_class = SYM_CLASS_UNKNOWN;
    ContextAddress sym_addr = 0;
    ContextAddress sym_size = 0;
    CodeArea * area = NULL;
    if (find_symbol_by_addr(ctx, STACK_NO_FRAME, ip, &sym) < 0) return 0;
    if (get_symbol_class(sym, &sym_class) < 0) return 0;
    if (sym_class != SYM_CLASS_FUNCTION) return 0;
    if (get_symbol_size(sym, &sym_size) < 0) return 0;
    if (sym_size == 0) return 0;
    if (get_symbol_address(sym, &sym_addr) < 0) return 0;
    if (address_to_line(ctx, sym_addr + sym_size - 1, sym_addr + sym_size, get_machine_code_area, &area) < 0) return 0;
    if (area != NULL && ip > area->start_address && ip < area->end_address) return 1;
    return 0;
}

#endif /* EN_STEP_OVER */
#endif /* EN_STEP_LINE */

#if EN_STEP_OVER
static void step_machine_breakpoint(Context * ctx, void * args) {
}

static BreakpointInfo * create_step_machine_breakpoint(ContextAddress addr, Context * ctx) {
    static const char * attr_list[] = { BREAKPOINT_ENABLED, BREAKPOINT_LOCATION, BREAKPOINT_SERVICE };
    BreakpointAttribute * attrs = NULL;
    BreakpointAttribute ** ref = &attrs;
    char str[32];
    unsigned i;

    for (i = 0; i < sizeof(attr_list) / sizeof(char *); i++) {
        ByteArrayOutputStream buf;
        BreakpointAttribute * attr = (BreakpointAttribute *)loc_alloc_zero(sizeof(BreakpointAttribute));
        OutputStream * out = create_byte_array_output_stream(&buf);
        attr->name = loc_strdup(attr_list[i]);
        switch (i) {
        case 0:
            json_write_boolean(out, 1);
            break;
        case 1:
            snprintf(str, sizeof(str), "%#" PRIx64, (uint64_t)addr);
            json_write_string(out, str);
            break;
        case 2:
            json_write_string(out, RUN_CONTROL);
            break;
        }
        write_stream(out, 0);
        get_byte_array_output_stream_data(&buf, &attr->value, NULL);
        *ref = attr;
        ref = &attr->next;
    }
    return create_eventpoint_ext(attrs, ctx, step_machine_breakpoint, NULL);
}
#endif

static int update_step_machine_state(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    ContextAddress addr = ext->pc;
    int do_reverse = 0;

    if (!context_has_state(ctx) || ctx->exited || ctx->pending_intercept) {
        cancel_step_mode(ctx);
        return 0;
    }

    switch (ext->step_mode) {
    case RM_REVERSE_STEP_OVER:
    case RM_REVERSE_STEP_INTO:
    case RM_REVERSE_STEP_OVER_LINE:
    case RM_REVERSE_STEP_INTO_LINE:
    case RM_REVERSE_STEP_OUT:
    case RM_REVERSE_STEP_OVER_RANGE:
    case RM_REVERSE_STEP_INTO_RANGE:
    case RM_REVERSE_UNTIL_ACTIVE:
        do_reverse = 1;
        break;
    }

    if (ext->pc_error) {
        if (ctx->stopped && get_error_code(ext->pc_error) == ERR_NOT_ACTIVE) {
            if (!do_reverse) {
                if (context_can_resume(ctx, ext->step_continue_mode = RM_UNTIL_ACTIVE)) return 0;
            }
            else {
                if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_UNTIL_ACTIVE)) return 0;
            }
        }
        errno = ext->pc_error;
        return -1;
    }

    assert(ctx->stopped);
    assert(ctx->pending_intercept == 0);
    assert(ext->intercepted == 0);
    assert(ext->step_repeat_cnt > 0);
    assert(ext->step_done == NULL);

    if (ext->step_cnt == 0) {
        /* In case of cache miss, clear stale data */
        if (ext->step_code_area != NULL) {
            free_code_area(ext->step_code_area);
            ext->step_code_area = NULL;
        }
        if (ext->step_func_id != NULL) {
            loc_free(ext->step_func_id);
            ext->step_func_id = NULL;
        }
        if (ext->step_func_id_out != NULL) {
            loc_free(ext->step_func_id_out);
            ext->step_func_id_out = NULL;
        }
        ext->step_inlined = 0;
        ext->step_frame_fp = 0;
        ext->step_set_frame_level = 0;
    }

#if EN_STEP_OVER
    {
        StackFrame * info = NULL;
        ContextAddress step_bp_addr = 0;

        int n = get_top_frame(ctx);
        if (n < 0) return -1;

        switch (ext->step_mode) {
        case RM_STEP_INTO_LINE:
        case RM_REVERSE_STEP_INTO_LINE:
            if (ext->step_cnt == 0) {
                int m = n + get_inlined_frame_level(ctx);
                if (get_frame_info(ctx, m, &info) < 0) return -1;
                if (info->func_id != NULL) ext->step_func_id = loc_strdup(info->func_id);
                if (info->area != NULL) update_step_machine_code_area(info->area, ext);
                ext->step_inlined = info->inlined;
                ext->step_frame_fp = info->fp;
                ext->step_set_frame_level = 1;
            }
            break;
        case RM_STEP_OVER:
        case RM_STEP_OVER_RANGE:
        case RM_STEP_OVER_LINE:
        case RM_REVERSE_STEP_OVER:
        case RM_REVERSE_STEP_OVER_RANGE:
        case RM_REVERSE_STEP_OVER_LINE:
        case RM_SKIP_PROLOGUE:
            if (ext->step_cnt == 0) {
                int m = n + get_inlined_frame_level(ctx);
                if (get_frame_info(ctx, m, &info) < 0) return -1;
                if (info->func_id != NULL) ext->step_func_id = loc_strdup(info->func_id);
                if (info->area != NULL) update_step_machine_code_area(info->area, ext);
                ext->step_inlined = info->inlined;
                ext->step_frame_fp = info->fp;
                ext->step_set_frame_level = 1;
            }
            else if (addr < ext->step_range_start || addr >= ext->step_range_end) {
                if (get_frame_info(ctx, n, &info) < 0) return -1;
                if (ext->step_frame_fp != info->fp) {
                    unsigned frame_cnt;
                    if (ext->step_bp_info != NULL) {
                        if (!do_reverse || ext->step_line_cnt > 1) {
                            ext->step_continue_mode = RM_RESUME;
                        }
                        else {
                            ext->step_continue_mode = RM_REVERSE_RESUME;
                        }
                        return 0;
                    }
                    if (do_reverse) {
                        int function_epilogue = is_within_function_epilogue(ctx, addr);
                        if (cache_miss_count() > 0) {
                            errno = ERR_CACHE_MISS;
                            return -1;
                        }
                        if (function_epilogue) {
                            /* With some compilers, the stack walking code based on debug information does not work
                             * correctly if we are in the middle of the function epilogue. In this case, an invalid
                             * return address is provided but no error is raised. To avoid this issue, do not try to
                             * get the stack trace of a function while it is in the midle of the epilogue but instead
                             * skip the epilogue. This code is done only in reverse stepping mode because it is very
                             * unlikely to meet this condition while doing forward stepping.
                             */
                            ext->step_continue_mode = RM_REVERSE_STEP_INTO;
                            return 0;
                        }
                    }
                    for (frame_cnt = 0; frame_cnt < RC_STEP_MAX_STACK_FRAMES; frame_cnt++) {
                        n = get_prev_frame(ctx, n);
                        if (n < 0) {
                            if (cache_miss_count() > 0) {
                                errno = ERR_CACHE_MISS;
                                return -1;
                            }
                            break;
                        }
                        if (get_frame_info(ctx, n, &info) < 0) return -1;
                        if (ext->step_frame_fp == info->fp) {
                            uint64_t pc = 0;
                            if (read_reg_value(info, get_PC_definition(ctx), &pc) < 0) return -1;
                            step_bp_addr = (ContextAddress)pc;
                            break;
                        }
                    }
                }
                else if (do_reverse) {
                    /* Workaround for GCC debug information that contains
                     * invalid stack frame information for function epilogues */
                    Symbol * sym_org = NULL;
                    Symbol * sym_now = NULL;
                    ContextAddress sym_org_addr = ext->step_range_start;
                    ContextAddress sym_now_addr = addr;
                    int anomaly =
                            find_symbol_by_addr(ctx, n, sym_now_addr, &sym_now) >= 0 &&
                            find_symbol_by_addr(ctx, n, sym_org_addr, &sym_org) >= 0 &&
                            get_symbol_address(sym_now, &sym_now_addr) >= 0 &&
                            get_symbol_address(sym_org, &sym_org_addr) >= 0 &&
                            sym_now_addr != sym_org_addr;
                    if (anomaly) {
                        /* Step over the anomaly */
                        ext->step_continue_mode = RM_REVERSE_STEP_INTO;
                        return 0;
                    }
                }
            }
            break;
        case RM_STEP_OUT:
        case RM_REVERSE_STEP_OUT:
            if (ext->step_cnt == 0) {
                uint64_t ip = 0;
                int m = n + get_inlined_frame_level(ctx);
                int p = get_prev_frame(ctx, m);
                if (p < 0) {
                    set_errno(errno, "Cannot step out");
                    return -1;
                }
                if (get_frame_info(ctx, m, &info) < 0) return -1;
                if (info->func_id != NULL) ext->step_func_id_out = loc_strdup(info->func_id);
                if (get_frame_info(ctx, p, &info) < 0) return -1;
                if (info->func_id != NULL) ext->step_func_id = loc_strdup(info->func_id);
                if (info->area != NULL) update_step_machine_code_area(info->area, ext);
                ext->step_inlined = info->inlined;
                ext->step_frame_fp = info->fp;
                ext->step_set_frame_level = 1;
                if (info->area == NULL) {
                    if (read_reg_value(info, get_PC_definition(ctx), &ip) < 0) return -1;
                    step_bp_addr = (ContextAddress)ip;
                    break;
                }
            }
            if (get_frame_info(ctx, n, &info) < 0) return -1;
            if (ext->step_frame_fp != info->fp) {
                unsigned frame_cnt;
                if (ext->step_bp_info != NULL) {
                    if (!do_reverse || ext->step_line_cnt > 1) {
                        ext->step_continue_mode = RM_RESUME;
                    }
                    else {
                        ext->step_continue_mode = RM_REVERSE_RESUME;
                    }
                    return 0;
                }
                for (frame_cnt = 0; frame_cnt < RC_STEP_MAX_STACK_FRAMES; frame_cnt++) {
                    n = get_prev_frame(ctx, n);
                    if (n < 0) {
                        if (cache_miss_count() > 0) {
                            errno = ERR_CACHE_MISS;
                            return -1;
                        }
                        break;
                    }
                    if (get_frame_info(ctx, n, &info) < 0) return -1;
                    if (ext->step_frame_fp == info->fp) {
                        uint64_t pc = 0;
                        if (read_reg_value(info, get_PC_definition(ctx), &pc) < 0) return -1;
                        step_bp_addr = (ContextAddress)pc;
                        break;
                    }
                }
            }
            break;
        }

#if SERVICE_Breakpoints
        if (step_bp_addr) {
            if (!do_reverse || ext->step_line_cnt > 1) {
                ext->step_continue_mode = RM_RESUME;
            }
            else {
                step_bp_addr--;
                ext->step_continue_mode = RM_REVERSE_RESUME;
            }
            if (ext->step_bp_info == NULL || ext->step_bp_addr != step_bp_addr) {
                if (ext->step_bp_info != NULL) destroy_eventpoint(ext->step_bp_info);
                ext->step_bp_info = create_step_machine_breakpoint(step_bp_addr, ctx);
                ext->step_bp_addr = step_bp_addr;
            }
            return 0;
        }

        if (ext->step_bp_info != NULL) {
            destroy_eventpoint(ext->step_bp_info);
            ext->step_bp_info = NULL;
        }
#endif

        if (ext->step_set_frame_level && get_frame_info(ctx, STACK_TOP_FRAME, &info) < 0) return -1;
        if (ext->step_set_frame_level && ext->step_frame_fp == info->fp && ext->step_inlined <= info->inlined) {
            StackFrame * tgt = info;
            int same_line = 1;
            int same_func = 1;
            if (ext->step_inlined < info->inlined) {
                if (get_frame_info(ctx, info->inlined - ext->step_inlined, &tgt) < 0) return -1;
            }
            if (ext->step_code_area != NULL && tgt->area != NULL) same_line = tgt->area->start_line == ext->step_code_area->start_line;
            else if (ext->step_code_area != NULL || tgt->area != NULL) same_line = 0;
            if (ext->step_func_id != NULL && tgt->func_id != NULL) same_func = strcmp(tgt->func_id, ext->step_func_id) == 0;
            else if (ext->step_func_id != NULL || tgt->func_id != NULL) same_func = 0;
            switch (ext->step_mode) {
            case RM_STEP_INTO_LINE:
            case RM_REVERSE_STEP_INTO_LINE:
                if (ext->step_inlined < info->inlined) {
                    if (same_line) ext->step_inlined++;
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                if (!same_func) {
                    if (ext->step_inlined > 0) ext->step_inlined--;
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                break;
            case RM_STEP_OVER_LINE:
            case RM_REVERSE_STEP_OVER_LINE:
                if (!same_func) {
                    if (ext->step_inlined > 0) ext->step_inlined--;
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                if (ext->step_inlined < info->inlined && !same_line) {
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                break;
            case RM_STEP_OUT:
            case RM_REVERSE_STEP_OUT:
                if (ext->step_inlined < info->inlined) {
                    if (ext->step_inlined == info->inlined - 1 && ext->step_func_id_out != NULL &&
                            (info->func_id == NULL || strcmp(info->func_id, ext->step_func_id_out))) {
                        ext->step_done = REASON_STEP;
                        return 0;
                    }
                }
                if (!same_func) {
                    if (ext->step_inlined > 0) ext->step_inlined--;
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                if (ext->step_inlined < info->inlined && !same_line) {
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                if (ext->step_inlined < info->inlined) {
                    if (!do_reverse || ext->step_line_cnt > 1) {
                        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_OVER)) return 0;
                        ext->step_continue_mode = RM_STEP_INTO;
                    }
                    else {
                        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_OVER)) return 0;
                        ext->step_continue_mode = RM_REVERSE_STEP_INTO;
                    }
                    return 0;
                }
                break;
            }
        }
    }
#endif /* EN_STEP_OVER */

    switch (ext->step_mode) {
    case RM_RESUME:
    case RM_REVERSE_RESUME:
        {
            int mode = ext->step_mode;
            cancel_step_mode(ctx);
            ext->step_continue_mode = mode;
        }
        return 0;
    case RM_UNTIL_ACTIVE:
    case RM_REVERSE_UNTIL_ACTIVE:
        ext->step_done = REASON_ACTIVE;
        return 0;
    case RM_STEP_INTO:
    case RM_STEP_OVER:
    case RM_REVERSE_STEP_INTO:
    case RM_REVERSE_STEP_OVER:
        if (ext->step_cnt == 0) {
            ext->step_range_start = addr;
            ext->step_range_end = addr + 1;
        }
        /* fall through */
    case RM_STEP_INTO_RANGE:
    case RM_STEP_OVER_RANGE:
    case RM_REVERSE_STEP_INTO_RANGE:
    case RM_REVERSE_STEP_OVER_RANGE:
        if (ext->step_cnt > 0 && (addr < ext->step_range_start || addr >= ext->step_range_end)) {
            ext->step_done = REASON_STEP;
            return 0;
        }
        break;
#if EN_STEP_LINE
    case RM_STEP_INTO_LINE:
    case RM_STEP_OVER_LINE:
    case RM_REVERSE_STEP_INTO_LINE:
    case RM_REVERSE_STEP_OVER_LINE:
        if (ext->step_cnt == 0) {
            if (ext->step_code_area == NULL) {
                if (address_to_line(ctx, addr, addr + 1, update_step_machine_code_area, ext) < 0) return -1;
            }
            if (ext->step_code_area != NULL) {
                ext->step_range_start = ext->step_code_area->start_address;
                ext->step_range_end = ext->step_code_area->end_address;
            }
            else {
                ext->step_range_start = addr;
                ext->step_range_end = addr + 1;
            }
        }
        else if (addr < ext->step_range_start || addr >= ext->step_range_end) {
            int function_prologue = 0;
            CodeArea * area = ext->step_code_area;
            if (area == NULL) {
                ext->step_done = REASON_STEP;
                return 0;
            }
            if (!ext->step_into_hidden) {
                int hidden_function = is_hidden_function(ctx, addr, &ext->step_range_start, &ext->step_range_end);
                if (cache_miss_count() > 0) {
                    errno = ERR_CACHE_MISS;
                    return -1;
                }
                if (hidden_function) {
                    /* Don't stop in a function that should be hidden during source level stepping */
                    break;
                }
            }
            ext->step_code_area = NULL;
            if (address_to_line(ctx, addr, addr + 1, update_step_machine_code_area, ext) < 0) {
                if (ext->step_code_area) free_code_area(ext->step_code_area);
                ext->step_code_area = area;
                return -1;
            }
            if (ext->step_code_area == NULL) {
                /* Line info not available for current IP */
#if ENABLE_Symbols
                if (is_plt_section(ctx, addr) != 0) {
                    /* Continue stepping to skip PLT entry */
                    ext->step_code_area = area;
                    ext->step_range_start = addr;
                    ext->step_range_end = addr + 1;
                    break;
                }
                else if (errno) {
                    ext->step_code_area = area;
                    return -1;
                }
#endif
                free_code_area(area);
                ext->step_done = REASON_STEP;
                return 0;
            }

            if (ext->step_code_area->start_line == 0) {
                /* Clang associates some instructions in the middle of functions to line 0.
                 * That is valid DWARF, we need to step over such no-line-info instructions.
                 */
                ext->step_range_start = ext->step_code_area->start_address;
                ext->step_range_end = ext->step_code_area->end_address;
                free_code_area(ext->step_code_area);
                ext->step_code_area = area;
            }
            else {
                int same_line = is_same_line(ext->step_code_area, area);
                if (!same_line) {
                    function_prologue = is_function_prologue(ctx, addr, ext->step_code_area);
                    if (cache_miss_count() > 0) {
                        free_code_area(ext->step_code_area);
                        ext->step_code_area = area;
                        errno = ERR_CACHE_MISS;
                        return -1;
                    }
                }
                free_code_area(area);

                /* We are doing reverse step-over/into line. The first line has already been skipped, we are now trying to reach
                 * the beginning of previous line. If we are still on same line but have reached the beginning of the line, then we
                 * are done.
                 */
                if (same_line && do_reverse && ext->step_line_cnt > 0 && addr == ext->step_code_area->start_address) {
                    ext->step_done = REASON_STEP;
                    return 0;
                }
                if (!same_line && !function_prologue) {
                    if (!do_reverse ||
                            (ext->step_line_cnt == 0 && addr == ext->step_code_area->start_address) ||
                            ext->step_line_cnt >= 2) {
                        ext->step_done = REASON_STEP;
                        return 0;
                    }
                    /* Current IP is in the middle of a source line.
                     * Continue stepping to get to the beginning of the line */
                    ext->step_line_cnt++;
                }
                ext->step_range_start = ext->step_code_area->start_address;
                ext->step_range_end = ext->step_code_area->end_address;
            }

            /* When doing reverse step-into/over line, if we have already skipped the first line, we want to reach
             * the beginning of current line, fix step range to handle this.
             */
            if (do_reverse && ext->step_line_cnt > 0) ext->step_range_start += 1;
        }
        break;
#if EN_STEP_OVER
    case RM_SKIP_PROLOGUE:
        {
            CodeArea * area = NULL;
            if (ext->step_cnt >= SKIP_PROLOGUE_MAX_STEPS) {
                /* Infinite loop in the prologue? */
                ext->step_done = REASON_USER_REQUEST;
                return 0;
            }
            if (address_to_line(ctx, addr, addr + 1, get_machine_code_area, &area) < 0) return -1;
            if (area == NULL || (area->start_line != 0 && !is_function_prologue(ctx, addr, area))) {
                if (cache_miss_count() > 0) {
                    errno = ERR_CACHE_MISS;
                    return -1;
                }
                ext->step_done = REASON_USER_REQUEST;
                return 0;
            }
        }
        break;
#endif /* EN_STEP_OVER */
#endif /* EN_STEP_LINE */
    case RM_STEP_OUT:
    case RM_REVERSE_STEP_OUT:
#if EN_STEP_LINE
        {
            /* Clang associates some instructions in the middle of functions to line 0.
             * That is valid DWARF, we need to step over such no-line-info instructions.
             */
            CodeArea * area = NULL;
            if (address_to_line(ctx, addr, addr + 1, get_machine_code_area, &area) < 0) return -1;
            if (area != NULL && area->start_line == 0) {
                ext->step_range_start = area->start_address;
                ext->step_range_end = area->end_address;
                switch (ext->step_mode) {
                case RM_STEP_OUT:
                    if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO_RANGE)) return 0;
                    if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO)) return 0;
                    break;
                case RM_REVERSE_STEP_OUT:
                    if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_INTO_RANGE)) return 0;
                    if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_INTO)) return 0;
                    break;
                }
            }
        }
#endif /* EN_STEP_LINE */
        ext->step_done = REASON_STEP;
        return 0;
    default:
        errno = ERR_UNSUPPORTED;
        return -1;
    }

    if (ext->step_line_cnt > 1) {
        if (ext->step_mode == RM_REVERSE_STEP_INTO_LINE) ext->step_continue_mode = RM_STEP_INTO_LINE;
        if (ext->step_mode == RM_REVERSE_STEP_OVER_LINE) ext->step_continue_mode = RM_STEP_OVER_LINE;
    }
    else {
        ext->step_continue_mode = ext->step_mode;
        if (ext->step_line_cnt == 0 && context_can_resume(ctx, ext->step_continue_mode)) return 0;
    }

    switch (ext->step_continue_mode) {
    case RM_STEP_INTO_LINE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO_RANGE)) return 0;
        break;
    case RM_STEP_OVER_LINE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_OVER_RANGE)) return 0;
        break;
    case RM_REVERSE_STEP_INTO_LINE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_INTO_RANGE)) return 0;
        break;
    case RM_REVERSE_STEP_OVER_LINE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_OVER_RANGE)) return 0;
        break;
    }

    switch (ext->step_continue_mode) {
    case RM_STEP_OVER:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO)) return 0;
        break;
    case RM_STEP_OVER_RANGE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_OVER)) return 0;
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO_RANGE)) return 0;
        break;
    case RM_REVERSE_STEP_OVER:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_INTO)) return 0;
        break;
    case RM_REVERSE_STEP_OVER_RANGE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_OVER)) return 0;
        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_INTO_RANGE)) return 0;
        break;
    case RM_SKIP_PROLOGUE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO)) return 0;
        break;
    }

    switch (ext->step_continue_mode) {
    case RM_STEP_INTO_RANGE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_STEP_INTO)) return 0;
        break;
    case RM_REVERSE_STEP_INTO_RANGE:
        if (context_can_resume(ctx, ext->step_continue_mode = RM_REVERSE_STEP_INTO)) return 0;
        break;
    }

    errno = ERR_UNSUPPORTED;
    return -1;
}

static int update_step_machine_state_inlined(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    for (;;) {
        if (update_step_machine_state(ctx) < 0) return -1;
        if (ext->step_done == NULL) break;
        ext->step_repeat_cnt--;
        if (ext->step_repeat_cnt > 0) {
            ext->step_cnt = 0;
            ext->step_line_cnt = 0;
            ext->step_done = NULL;
        }
        else {
            ctx->pending_intercept = 1;
            break;
        }
    }
    if (ctx->pending_intercept && ext->step_set_frame_level) {
        StackFrame * info = NULL;
        if (get_frame_info(ctx, STACK_TOP_FRAME, &info) < 0) return -1;
        if (ext->step_frame_fp != info->fp) {
            set_inlined_frame_level(ctx, info->inlined);
        }
        else if (info->inlined >= ext->step_inlined) {
            set_inlined_frame_level(ctx, info->inlined - ext->step_inlined);
        }
    }
    return 0;
}

static void stop_all_timer(void * args) {
    stop_all_timer_posted = 0;
    stop_all_timer_cnt++;
    run_safe_events_posted++;
    post_event(run_safe_events, NULL);
}

static void resume_error(Context * ctx, int error) {
    ContextExtensionRC * ext = EXT(ctx);
    trace(LOG_ALWAYS, "Cannot resume context %s: %s", ctx->id, errno_to_str(error));
    if (context_has_state(ctx)) {
        error = set_errno(error, ext->step_bp_info ? "Cannot continue stepping" : "Cannot resume");
        ctx->signal = 0;
        ctx->stopped = 1;
        ctx->stopped_by_bp = 0;
        ctx->stopped_by_cb = NULL;
        ctx->pending_intercept = 1;
        ctx->stopped_by_exception = 1;
        loc_free(ctx->exception_description);
        ctx->exception_description = loc_strdup(errno_to_str(error));
    }
}

static void check_step_breakpoint_instances(InputStream * inp, void * args);

static void check_step_breakpoint_status(InputStream * inp, const char * name, void * args) {
    if (strcmp(name, "Error") == 0) {
        char * msg = json_read_alloc_string(inp);
        if (msg != NULL) {
            *(int *)args = set_errno(ERR_OTHER, msg);
            loc_free(msg);
        }
    }
    else if (strcmp(name, "Instances") == 0) {
        json_read_array(inp, check_step_breakpoint_instances, args);
    }
    else {
        json_skip_object(inp);
    }
}

static void check_step_breakpoint_instances(InputStream * inp, void * args) {
    json_read_struct(inp, check_step_breakpoint_status, args);
}

static int check_step_breakpoint(Context * ctx) {
#if SERVICE_Breakpoints
    /* Return error if step machine breakpoint cannot be planted */
    Trap trap;
    int error = 0;
    char * status = NULL;
    ContextExtensionRC * ext = EXT(ctx);
    if (ext->step_bp_info == NULL) return 0;
    status = get_breakpoint_status(ext->step_bp_info);
    if (set_trap(&trap)) {
        ByteArrayInputStream buf;
        InputStream * inp = create_byte_array_input_stream(&buf, status, strlen(status));
        json_read_struct(inp, check_step_breakpoint_status, &error);
        json_test_char(inp, MARKER_EOS);
        clear_trap(&trap);
    }
    else {
        error = trap.error;
    }
    loc_free(status);
    if (!error) return 0;
    errno = error;
    return -1;
#else
    return 0;
#endif
}

#if EN_STEP_OVER
static Channel * select_skip_prologue_channel(void) {
    /* TODO: better logic to select symbols server channel for skipping function prologue */
    Channel * def = NULL;
    LINK * l = channel_root.next;
    while (l != &channel_root) {
        Channel * c = chanlink2channelp(l);
        if (!is_channel_closed(c)) {
            int i;
            for (i = 0; i < c->peer_service_cnt; i++) {
                char * nm = c->peer_service_list[i];
                if (strcmp(nm, "Symbols") == 0) return c;
            }
            def = c;
        }
        l = l->next;
    }
    return def;
}
#endif

#ifndef NDEBUG
int print_not_stopped_contexts(Context * ctx) {
    LINK * l;
    Context * grp;
    if (is_all_stopped_or_cannot_stop(ctx)) return 1;
    grp = context_get_group(ctx, CONTEXT_GROUP_STOP);
    fprintf(stderr, "Context group '%s':\n", grp->name ? grp->name : grp->id);
    for (l = context_root.next; l != &context_root; l = l->next) {
        Context * c = ctxl2ctxp(l);
        if (context_get_group(c, CONTEXT_GROUP_STOP) != grp) continue;
        fprintf(stderr, "  ID %s, stopped %d, exiting %d, exited %d, signal %d, cannot stop %d\n",
            c->id, c->stopped, c->exiting, c->exited, c->signal, EXT(c)->cannot_stop);
    }
    return 0;
}
#endif

static void sync_run_state(void) {
    int err_cnt = 0;
    LINK * l;
    LINK p;

    if (run_ctrl_lock_cnt != 0) return;
    assert(safe_event_list == NULL);
    stop_all_timer_cnt = 0;

    /* Clear intercept group flags, get current PCs */
    safe_event_pid_count = 0;
    l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        l = l->next;
        ext->pc = 0;
        ext->pc_error = ERR_OTHER;
        ext->pending_safe_event = 0;
        if (context_has_state(ctx)) {
            Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
            EXT(grp)->intercept_group = 0;
        }
        else if (ext->step_mode == RM_TERMINATE || ext->step_mode == RM_DETACH) {
            int md = ext->step_mode;
            assert_all_stopped(ctx);
            if (context_resume(ctx, md, 0, 0) < 0) {
                if (cache_miss_count() > 0) return;
                resume_error(ctx, errno);
            }
            ext->step_mode = RM_RESUME;
        }
    }

    /* Set intercept group flags */
    l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        l = l->next;
        if (ctx->exited) continue;
        if (ctx->pending_intercept || ext->intercepted) {
            Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
            EXT(grp)->intercept_group = 1;
            continue;
        }
        if (!ctx->stopped) continue;
        if (ext->step_mode == RM_RESUME && ext->skip_prologue) {
#if EN_STEP_OVER
            start_step_mode(ctx, select_skip_prologue_channel(), RM_SKIP_PROLOGUE, 1, 0, 0);
#else
            Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
            EXT(grp)->intercept_group = 1;
            continue;
#endif
        }
        if (ext->step_mode == RM_RESUME || ext->step_mode == RM_REVERSE_RESUME ||
            ext->step_mode == RM_TERMINATE || ext->step_mode == RM_DETACH) {
                ext->step_continue_mode = ext->step_mode;
        }
        else if (ext->step_channel != NULL && is_channel_closed(ext->step_channel)) {
            cancel_step_mode(ctx);
        }
        else {
            get_current_pc(ctx);
            cache_set_def_channel(ext->step_channel);
            if (update_step_machine_state_inlined(ctx) < 0) {
                int error = errno;
                Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
                if (cache_miss_count() > 0) return;
                release_error_report(ext->step_error);
                ext->step_error = get_error_report(error);
                cancel_step_mode(ctx);
                EXT(grp)->intercept_group = 1;
            }
            else if (ctx->pending_intercept) {
                Context * grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
                EXT(grp)->intercept_group = 1;
            }
            cache_set_def_channel(NULL);
        }
    }

    /* Stop or continue contexts as needed */
    list_init(&p);
    l = context_root.next;
    while (err_cnt == 0 && run_ctrl_lock_cnt == 0 && l != &context_root) {
        Context * grp = NULL;
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        l = l->next;
        if (ctx->exited) continue;
        if (ext->intercepted) continue;
        if (!context_has_state(ctx)) continue;
        grp = context_get_group(ctx, CONTEXT_GROUP_INTERCEPT);
        if (EXT(grp)->intercept_group) {
            ctx->pending_intercept = 1;
            if (!ctx->stopped && !ctx->exiting) {
                assert(!ext->safe_single_step);
                context_stop(ctx);
                ext->pending_safe_event = 1;
                safe_event_pid_count++;
                assert(!ctx->stopped);
            }
        }
        else if (ctx->stopped && !ctx->pending_intercept && ext->run_ctrl_ctx_lock_cnt == 0) {
            if (check_step_breakpoint(ctx) < 0) {
                if (cache_miss_count() > 0) return;
                resume_error(ctx, errno);
                err_cnt++;
            }
            else if (ext->step_continue_mode != RM_RESUME) {
                list_add_last(&ext->link, &p);
            }
            else if (context_resume(ctx, EXT(grp)->reverse_run ? RM_REVERSE_RESUME : RM_RESUME, 0, 0) < 0) {
                if (cache_miss_count() > 0) return;
                resume_error(ctx, errno);
                err_cnt++;
            }
        }
        if (ctx->pending_intercept && ctx->stopped) {
            if (ext->pc_error == ERR_OTHER) get_current_pc(ctx);
        }
    }

    /* Resume contexts with resume mode other then RM_RESUME */
    l = p.next;
    while (err_cnt == 0 && run_ctrl_lock_cnt == 0 && l != &p) {
        Context * ctx = link2ctx(l);
        ContextExtensionRC * ext = EXT(ctx);
        assert(!ext->intercepted);
        l = l->next;
        if (context_resume(ctx, ext->step_continue_mode, ext->step_range_start, ext->step_range_end) < 0) {
            if (cache_miss_count() > 0) return;
            resume_error(ctx, errno);
            err_cnt++;
        }
    }

    if (safe_event_pid_count > 0 || run_ctrl_lock_cnt > 0) return;

    if (err_cnt > 0 && run_safe_events_posted < 4) {
        run_safe_events_posted++;
        post_event(run_safe_events, NULL);
    }
}

static void sync_run_state_cache_client(void * args) {
    sync_run_state();
    cache_exit();
    assert(sync_run_state_event_posted > 0);
    sync_run_state_event_posted--;
    if (run_safe_events_posted || sync_run_state_event_posted > 0 || run_ctrl_lock_cnt > 0) return;
    send_event_context_suspended();
}

static void sync_run_state_event(void * args) {
    cache_enter(sync_run_state_cache_client, NULL, NULL, 0);
}

static void mark_cannot_stop(Context * ctx, const char * err_msg) {
    ContextExtensionRC * ext = EXT(ctx);
    const char * name = ctx->name;
    if (name == NULL) name = ctx->id;
    trace(LOG_ALWAYS, "Cannot stop '%s': %s", name, err_msg);
    cancel_step_mode(ctx);
    ext->safe_single_step = 0;
    ext->cannot_stop = 1;
}

static void run_safe_events(void * arg) {
    LINK * l;
    SafeEvent * i;

    run_safe_events_posted--;
    if (run_safe_events_posted > 0) return;

    if (run_ctrl_lock_cnt == 0) {
        sync_run_state_event_posted++;
        post_event(sync_run_state_event, NULL);
        return;
    }

    if (safe_event_list == NULL) return;

    safe_event_pid_count = 0;
    i = safe_event_list;
    while (i != NULL) {
        ContextExtensionRC * ext = EXT(i->ctx);
        assert(i->ctx->ref_count > 0);
        ext->stop_group_ctx = NULL;
        ext->stop_group_mark = 0;
        i = i->next;
    }
    l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        ext->stop_group_ctx = context_get_group(ctx, CONTEXT_GROUP_STOP);
        ext->stop_group_mark = 0;
        assert(ext->stop_group_ctx != NULL);
        l = l->next;
    }
    i = safe_event_list;
    while (i != NULL) {
        Context * grp = EXT(i->ctx)->stop_group_ctx;
        if (grp != NULL) EXT(grp)->stop_group_mark = 1;
        i = i->next;
    }
    l = context_root.next;
    while (l != &context_root) {
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        l = l->next;
        ext->pending_safe_event = 0;
        ext->stop_group_mark = EXT(ext->stop_group_ctx)->stop_group_mark;
        if (ctx->exited || ctx->exiting || ext->cannot_stop) continue;
        if (!ext->safe_single_step && !ext->stop_group_mark) continue;
        if (ctx->stopped || !context_has_state(ctx)) continue;
        if (stop_all_timer_cnt >= STOP_ALL_MAX_CNT) {
            mark_cannot_stop(ctx, "timeout");
            continue;
        }
        if (stop_all_timer_cnt >= 2 && ext->state_name != NULL) {
            mark_cannot_stop(ctx, ext->state_name);
            continue;
        }
#if ENABLE_Trace
        if (stop_all_timer_cnt == STOP_ALL_MAX_CNT / 2) {
            const char * msg = ext->safe_single_step ? "finish single step" : "stop";
            trace(LOG_ALWAYS, "Warning: waiting too long for context %s to %s", ctx->id, msg);
        }
#endif
        if (!ext->safe_single_step || stop_all_timer_cnt >= STOP_ALL_MAX_CNT / 2) {
            if (context_stop(ctx) < 0) {
                mark_cannot_stop(ctx, errno_to_str(errno));
                continue;
            }
            assert(!ctx->stopped);
        }
        ext->pending_safe_event = 1;
        safe_event_pid_count++;
    }

    if (safe_event_pid_count == 0) {
        stop_all_timer_cnt = 0;
        if (stop_all_timer_posted) {
            cancel_event(stop_all_timer, NULL, 0);
            stop_all_timer_posted = 0;
        }
    }

    while (safe_event_list) {
        Trap trap;
        i = safe_event_list;
        if (!EXT(i->ctx)->stop_group_mark) {
            assert(run_ctrl_lock_cnt > 0);
            if (run_safe_events_posted == 0) {
                run_safe_events_posted++;
                post_event(run_safe_events, NULL);
            }
            break;
        }
        if (safe_event_pid_count > 0) {
            if (!stop_all_timer_posted) {
                stop_all_timer_posted = 1;
                post_event_with_delay(stop_all_timer, NULL, STOP_ALL_TIMEOUT);
            }
            break;
        }
        assert_all_stopped(i->ctx);
        safe_event_list = i->next;
        if (safe_event_list == NULL) safe_event_last = NULL;
        if (i->done != NULL) {
            assert(EXT(i->ctx)->stop_group_mark);
            assert(!EXT(i->ctx)->pending_safe_event);
            safe_event_active = 1;
            if (set_trap(&trap)) {
                i->done(i->arg);
                clear_trap(&trap);
            }
            else {
                trace(LOG_ALWAYS, "Unhandled exception in \"safe\" event dispatch: %d %s",
                      trap.error, errno_to_str(trap.error));
            }
            safe_event_active = 0;
        }
        run_ctrl_unlock();
        context_unlock(i->ctx);
        loc_free(i);
    }
    cache_notify(&safe_events_cache);
}

static void check_safe_events(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    assert(ctx->stopped || ctx->exited);
    assert(ext->pending_safe_event);
    assert(safe_event_pid_count > 0);
    ext->pending_safe_event = 0;
    safe_event_pid_count--;
    if (safe_event_pid_count == 0) {
        run_safe_events_posted++;
        post_event(run_safe_events, NULL);
    }
}

void post_safe_event(Context * ctx, EventCallBack * done, void * arg) {
    SafeEvent * i = (SafeEvent *)loc_alloc_zero(sizeof(SafeEvent));
    run_ctrl_lock();
    context_lock(ctx);
    if (safe_event_list == NULL) {
        run_safe_events_posted++;
        post_event(run_safe_events, NULL);
    }
    /* Note: context stop group can change
     * while the event is waiting in the queue */
    i->ctx = ctx;
    i->done = done;
    i->arg = arg;
    if (safe_event_list == NULL) safe_event_list = i;
    else safe_event_last->next = i;
    safe_event_last = i;
}

int is_safe_event(void) {
    return safe_event_active;
}

void check_all_stopped(Context * ctx) {
    if (is_all_stopped_or_cannot_stop(ctx)) return;
    post_safe_event(ctx, NULL, NULL);
    cache_wait(&safe_events_cache);
}

void wait_safe_events_done(void) {
    if (safe_event_list != NULL) cache_wait(&safe_events_cache);
}

int safe_context_single_step(Context * ctx) {
    int res = 0;
    ContextExtensionRC * ext = EXT(ctx);
    assert(run_ctrl_lock_cnt > 0);
    assert(safe_event_list != NULL);
    assert(ext->safe_single_step == 0);
    ext->safe_single_step = 1;
    res = context_resume(ctx, RM_STEP_INTO, 0, 0);
    assert(res < 0 || !ctx->stopped);
    if (res < 0) ext->safe_single_step = 0;
    return res;
}

void run_ctrl_lock(void) {
    if (run_ctrl_lock_cnt == 0) {
        assert(safe_event_list == NULL);
#if ENABLE_Cmdline
        cmdline_suspend();
#endif
    }
    run_ctrl_lock_cnt++;
}

void run_ctrl_unlock(void) {
    assert(run_ctrl_lock_cnt > 0);
    run_ctrl_lock_cnt--;
    if (run_ctrl_lock_cnt == 0) {
        assert(safe_event_list == NULL);
#if ENABLE_Cmdline
        cmdline_resume();
#endif
        /* Lazily continue execution of temporary stopped contexts */
        run_safe_events_posted++;
        post_event(run_safe_events, NULL);
    }
}

void run_ctrl_ctx_lock(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    assert(context_has_state(ctx));
    ext->run_ctrl_ctx_lock_cnt++;
}

void run_ctrl_ctx_unlock(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    assert(context_has_state(ctx));
    assert(ext->run_ctrl_ctx_lock_cnt > 0);
    ext->run_ctrl_ctx_lock_cnt--;
    if (ext->run_ctrl_ctx_lock_cnt == 0) {
        /* Lazily continue execution of temporary stopped contexts */
        run_safe_events_posted++;
        post_event(run_safe_events, NULL);
    }
}

void set_context_state_name(Context * ctx, const char * name) {
    ContextExtensionRC * ext = EXT(ctx);
    OutputStream * out = &broadcast_group->out;

    if (name != NULL) {
        if (ext->state_name != NULL) {
            if (strcmp(ext->state_name, name) == 0) return;
            loc_free(ext->state_name);
        }
        ext->state_name = loc_strdup(name);
    }
    else {
        if (ext->state_name == NULL) return;
        loc_free(ext->state_name);
        ext->state_name = NULL;
    }

    if (!ext->intercepted) {
        write_stringz(out, "E");
        write_stringz(out, RUN_CONTROL);
        write_stringz(out, "contextStateChanged");

        json_write_string(out, ctx->id);
        write_stream(out, 0);

        write_stream(out, MARKER_EOM);
    }
}

const char * get_context_state_name(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    if (ext->intercepted) return get_suspend_reason(ctx);
    if (ext->state_name) return ext->state_name;
    return "Running";
}

int is_run_ctrl_idle(void) {
    if (safe_event_list == NULL && run_ctrl_lock_cnt == 0 &&
            run_safe_events_posted == 0 && sync_run_state_event_posted == 0) {
        LINK * l = context_root.next;
        while (l != &context_root) {
            ContextExtensionRC * ext = EXT(ctxl2ctxp(l));
            if (ext->run_ctrl_ctx_lock_cnt > 0) return 0;
            l = l->next;
        }
        return 1;
    }
    return 0;
}

void add_run_control_event_listener(RunControlEventListener * listener, void * args) {
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listeners = (Listener *)loc_realloc(listeners, listener_max * sizeof(Listener));
    }
    listeners[listener_cnt].listener = listener;
    listeners[listener_cnt].args = args;
    listener_cnt++;
}

void rem_run_control_event_listener(RunControlEventListener * listener) {
    unsigned i = 0;
    while (i < listener_cnt) {
        if (listeners[i++].listener == listener) {
            while (i < listener_cnt) {
                listeners[i - 1] = listeners[i];
                i++;
            }
            listener_cnt--;
            break;
        }
    }
}

static void stop_if_safe_events(Context * ctx) {
    ContextExtensionRC * ext = EXT(ctx);
    if (safe_event_active && EXT(ctx)->stop_group_mark &&
            !ctx->exiting && context_has_state(ctx)) {
        assert(run_ctrl_lock_cnt > 0);
        if (!ext->safe_single_step && !ctx->stopped) {
            context_stop(ctx);
        }
        if (!ext->pending_safe_event) {
            ext->pending_safe_event = 1;
            safe_event_pid_count++;
        }
    }
}

static void event_context_created(Context * ctx, void * args) {
    assert(!ctx->exited);
    assert(!ctx->stopped);
    send_event_context_added(ctx);
    stop_if_safe_events(ctx);
}

static void event_context_changed(Context * ctx, void * args) {
    assert(!ctx->exited);
    send_event_context_changed(ctx);
    stop_if_safe_events(ctx);
}

static void context_proxy_reply(Channel * c, void * args, int error) {
    if (!error) json_test_char(&c->inp, MARKER_EOM);
}

static void clear_context_proxy_cache(Context * ctx) {
    LINK * l = channel_root.next;
    while (l != &channel_root) {
        Channel * c = chanlink2channelp(l);
        if (!is_channel_closed(c)) {
            int i;
            for (i = 0; i < c->peer_service_cnt; i++) {
                char * nm = c->peer_service_list[i];
                if (strcmp(nm, CONTEXT_PROXY) == 0) {
                    protocol_send_command(c, CONTEXT_PROXY, "clear", context_proxy_reply, NULL);
                    json_write_string(&c->out, ctx->id);
                    write_stream(&c->out, 0);
                    write_stream(&c->out, MARKER_EOM);
                    break;
                }
            }
        }
        l = l->next;
    }
}

static void event_context_stopped(Context * ctx, void * args) {
    ContextExtensionRC * ext = EXT(ctx);
    assert(ctx->stopped);
    assert(!ctx->exited);
    assert(!ext->intercepted);
    ext->safe_single_step = 0;
    ext->cannot_stop = 0;
    if (ext->step_mode) {
        ext->step_cnt++;
    }
    else {
        if (ext->step_error != NULL) {
            release_error_report(ext->step_error);
            ext->step_error = NULL;
        }
        ext->step_done = NULL;
    }
    clear_context_proxy_cache(ctx);
#if SERVICE_Breakpoints
    if (ctx->stopped_by_bp || ctx->stopped_by_cb) evaluate_breakpoint(ctx);
#endif
    if (ext->pending_safe_event) check_safe_events(ctx);
    if (ctx->stopped_by_exception) send_event_context_exception(ctx);
    if (run_ctrl_lock_cnt == 0 && run_safe_events_posted < 4) {
        /* Lazily continue execution of temporary stopped contexts */
        run_safe_events_posted++;
        post_event(run_safe_events, NULL);
    }
}

static void event_context_started(Context * ctx, void * args) {
    ContextExtensionRC * ext = EXT(ctx);
    assert(!ctx->stopped);
    assert(!ctx->exited);
    if (ext->intercepted) resume_context_tree(ctx);
    stop_if_safe_events(ctx);
}

static void event_context_exited(Context * ctx, void * args) {
    ContextExtensionRC * ext = EXT(ctx);
    ext->safe_single_step = 0;
    cancel_step_mode(ctx);
    send_event_context_removed(ctx);
    if (ext->pending_safe_event) check_safe_events(ctx);
}

static void channel_closed(Channel * c) {
    ChannelExtensionRC * ext_ch = EXT_CH(c);
    LINK * l;
    for (l = context_root.next; l != &context_root; l = l ->next) {
        Context * ctx = ctxl2ctxp(l);
        ContextExtensionRC * ext = EXT(ctx);
        if (ext->step_channel == c) cancel_step_mode(ctx);
    }
    if (ext_ch->cache_lock) {
        ext_ch->cache_lock = 0;
        run_ctrl_unlock();
    }
}

static void context_cache_lock(Channel * c) {
    ChannelExtensionRC * ext = EXT_CH(c);
    json_test_char(&c->inp, MARKER_EOM);
    assert(ext->cache_lock == 0);
    ext->cache_lock = 1;
    run_ctrl_lock();
}

static void context_cache_unlock(Channel * c) {
    ChannelExtensionRC * ext = EXT_CH(c);
    json_test_char(&c->inp, MARKER_EOM);
    assert(ext->cache_lock == 1);
    ext->cache_lock = 0;
    run_ctrl_unlock();
}

static void channel_opened(Channel * c) {
    add_event_handler(c, CONTEXT_PROXY, "lock", context_cache_lock);
    add_event_handler(c, CONTEXT_PROXY, "unlock", context_cache_unlock);
}

static void event_context_disposed(Context * ctx, void * args) {
    ContextExtensionRC * ext = EXT(ctx);
    cancel_step_mode(ctx);
    if (ext->step_error) {
        release_error_report(ext->step_error);
        ext->step_error = NULL;
    }
    loc_free(ext->state_name);
    while (ext->bp_cnt > 0) loc_free(ext->bp_ids[--ext->bp_cnt]);
    loc_free(ext->bp_ids);
}

static int cmp_has_state(Context * ctx, const char * v) {
    int x = strcmp(v, "true") == 0 || strcmp(v, "1") == 0;
    int y = context_has_state(ctx) != 0;
    return x == y;
}

static int cmp_parent_id(Context * ctx, const char * v) {
    return ctx->parent != NULL && strcmp(ctx->parent->id, v) == 0;
}

static int cmp_parent_name(Context * ctx, const char * v) {
    return ctx->parent != NULL && strcmp(ctx->parent->name ? ctx->parent->name : ctx->parent->id, v) == 0;
}

static int cmp_ancestor_id(Context * ctx, const char * v) {
    ctx = ctx->parent;
    while (ctx != NULL) {
        if (strcmp(ctx->id, v) == 0) return 1;
        ctx = ctx->parent;
    }
    return 0;
}

static int cmp_ancestor_name(Context * ctx, const char * v) {
    ctx = ctx->parent;
    while (ctx != NULL) {
        if (strcmp(ctx->name ? ctx->name : ctx->id, v) == 0) return 1;
        ctx = ctx->parent;
    }
    return 0;
}

static int cmp_creator_id(Context * ctx, const char * v) {
    return ctx->creator != NULL && strcmp(ctx->creator->id, v) == 0;
}

static int cmp_process_id(Context * ctx, const char * v) {
    ctx = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
    return ctx != NULL && strcmp(ctx->id, v) == 0;
}

void ini_run_ctrl_service(Protocol * proto, TCFBroadcastGroup * bcg) {
    static ContextEventListener listener = {
        event_context_created,
        event_context_exited,
        event_context_stopped,
        event_context_started,
        event_context_changed,
        event_context_disposed
    };
    broadcast_group = bcg;
    add_context_event_listener(&listener, NULL);
    add_channel_close_listener(channel_closed);
    add_channel_open_listener(channel_opened);
    context_extension_offset = context_extension(sizeof(ContextExtensionRC));
    channel_extension_offset = channel_extension(sizeof(ChannelExtensionRC));
    add_command_handler(proto, RUN_CONTROL, "getContext", command_get_context);
    add_command_handler(proto, RUN_CONTROL, "getChildren", command_get_children);
    add_command_handler(proto, RUN_CONTROL, "getState", command_get_state);
#if ENABLE_ContextISA
    add_command_handler(proto, RUN_CONTROL, "getISA", command_get_isa);
#endif
    add_command_handler(proto, RUN_CONTROL, "resume", command_resume);
    add_command_handler(proto, RUN_CONTROL, "suspend", command_suspend);
    add_command_handler(proto, RUN_CONTROL, "terminate", command_terminate);
    add_command_handler(proto, RUN_CONTROL, "detach", command_detach);
    add_context_query_comparator("HasState", cmp_has_state);
    add_context_query_comparator("ParentID", cmp_parent_id);
    add_context_query_comparator("ParentName", cmp_parent_name);
    add_context_query_comparator("AncestorID", cmp_ancestor_id);
    add_context_query_comparator("AncestorName", cmp_ancestor_name);
    add_context_query_comparator("CreatorID", cmp_creator_id);
    add_context_query_comparator("ProcessID", cmp_process_id);
}

#else

#include <tcf/services/runctrl.h>

int is_ctx_stopped(Context * ctx) {
#if ENABLE_DebugContext
    if (ctx == NULL) {
        errno = ERR_INV_CONTEXT;
        return 0;
    }
    if (ctx->stopped) return 1;
    if (ctx->exited) {
        errno = ERR_ALREADY_EXITED;
        return 0;
    }
    if (!context_has_state(ctx)) {
        errno = ERR_INV_CONTEXT;
        return 0;
    }
#endif
    errno = ERR_IS_RUNNING;
    return 0;
}

int is_all_stopped(Context * grp) {
#if ENABLE_DebugContext
    LINK * l;
    grp = context_get_group(grp, CONTEXT_GROUP_STOP);
    for (l = context_root.next; l != &context_root; l = l->next) {
        Context * ctx = ctxl2ctxp(l);
        if (ctx->stopped || ctx->exited || ctx->exiting) continue;
        if (!context_has_state(ctx)) continue;
        if (context_get_group(ctx, CONTEXT_GROUP_STOP) != grp) continue;
        errno = ERR_IS_RUNNING;
        return 0;
    }
#endif
    return 1;
}

#endif /* SERVICE_RunControl */
