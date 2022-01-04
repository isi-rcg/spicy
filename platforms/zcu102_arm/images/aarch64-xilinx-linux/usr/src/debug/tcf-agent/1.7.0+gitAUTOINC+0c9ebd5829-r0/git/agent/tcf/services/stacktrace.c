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
 * Target service implementation: stack trace (TCF name StackTrace)
 */

#include <tcf/config.h>

#if SERVICE_StackTrace

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/json.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/registers.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/symbols.h>
#include <tcf/services/linenumbers.h>
#include <tcf/services/memorymap.h>
#include <tcf/services/stacktrace.h>

#define MAX_FRAMES  1000

static const char * STACKTRACE = "StackTrace";

typedef struct StackTrace {
    int inlined;
    int complete;
    int frame_cnt;
    int frame_max;
    StackFrame * frames; /* ordered top (current) to bottom */
} StackTrace;

static size_t context_extension_offset = 0;

#define EXT(ctx) ((StackTrace *)((char *)(ctx) + context_extension_offset))

static void add_frame(StackTrace * stack, StackFrame * frame) {
    frame->frame = stack->frame_cnt;
    if (stack->frame_cnt >= stack->frame_max) {
        stack->frame_max += 32;
        stack->frames = (StackFrame *)loc_realloc(stack->frames,
            stack->frame_max * sizeof(StackFrame));
    }
    stack->frames[stack->frame_cnt++] = *frame;
}

static void free_frame(StackFrame * frame) {
    int error = errno;
    if (frame->area != NULL) {
        loc_free(frame->area->directory);
        loc_free(frame->area->file);
        loc_free(frame->area);
        frame->area = NULL;
    }
    loc_free(frame->regs);
    loc_free(frame->func_id);
    frame->regs = NULL;
    frame->func_id = NULL;
    errno = error;
}

#if ENABLE_Symbols
static int get_frame_debug_info(StackFrame * frame, StackTracingInfo ** info) {
    uint64_t ip = 0;
    Context * ctx = frame->ctx;
    if (read_reg_value(frame, get_PC_definition(ctx), &ip) < 0) {
        if (frame->is_top_frame) return -1;
        return 0;
    }
    if (ip > 0 && !frame->is_top_frame) {
        StackTrace * stk = EXT(ctx);
        if (frame->frame > stk->frame_cnt) {
            ip--;
        }
        else {
            StackFrame * up = stk->frames + (frame->frame - 1);
            /* Walked reg values are saved before call, use (return address) - 1 to search next frame info.
             * Stack crawl regs are computed after return, use return address */
            if (up->is_walked) {
                ip--;
            }
            else {
                /* Workaround for missing frame info for return address of a function that never returns */
                Symbol * sym = NULL;
                int sym_class = SYM_CLASS_UNKNOWN;
                ContextAddress sym_addr = 0;
                ContextAddress sym_size = 0;
                if (find_symbol_by_addr(ctx, STACK_NO_FRAME, (ContextAddress)(ip - 1), &sym) == 0 &&
                        get_symbol_class(sym, &sym_class) == 0 && sym_class == SYM_CLASS_FUNCTION &&
                        get_symbol_size(sym, &sym_size) == 0 && sym_size != 0 &&
                        get_symbol_address(sym, &sym_addr) == 0 && sym_addr != 0 &&
                        sym_addr + sym_size > sym_addr && sym_addr + sym_size <= ip) {
                    ip--;
                }
            }
        }
    }
    return get_stack_tracing_info(ctx, (ContextAddress)ip, info);
}
#endif

int get_next_stack_frame(StackFrame * frame, StackFrame * down) {
#if ENABLE_Symbols
    int error = 0;
    Context * ctx = frame->ctx;
    StackTracingInfo * info = NULL;
    int frame_idx = frame->frame;

    memset(down, 0, sizeof(StackFrame));

    if (get_frame_debug_info(frame, &info) < 0) {
        error = errno;
    }
    else if (info != NULL) {
        Trap trap;
        if (set_trap(&trap)) {
            int i;
            LocationExpressionState * state;
            state = evaluate_location_expression(ctx, frame, info->fp->cmds, info->fp->cmds_cnt, NULL, 0);
            if (state->stk_pos != 1) str_exception(ERR_OTHER, "Invalid stack trace expression");
            frame->fp = (ContextAddress)state->stk[0];
            if (info->sub_cnt > 0) {
                size_t buf_size = 8;
                uint8_t * buf = (uint8_t *)tmp_alloc(buf_size);
                StackTrace * stk = EXT(ctx);
#if ENABLE_StackRegisterLocations
                LocationExpressionCommand cmd;
                memset(&cmd, 0, sizeof(cmd));
                cmd.cmd = SFT_CMD_RD_REG;
#endif
                frame->inlined = info->sub_cnt;
                for (i = 0; i < info->sub_cnt; i++) {
                    RegisterDefinition * def = get_reg_definitions(ctx);
                    StackFrame * prev = stk->frames + stk->frame_cnt - 1;
                    down->ctx = ctx;
                    down->fp = frame->fp;
                    while (def && def->name) {
                        if (def->dwarf_id >= 0) {
#if ENABLE_StackRegisterLocations
                            cmd.args.reg = def;
                            if (write_reg_location(down, def, &cmd, 1) == 0) {
                                down->has_reg_data = 1;
                                def++;
                                continue;
                            }
#endif
                            if (buf_size < def->size) buf = (uint8_t *)tmp_realloc(buf, buf_size = def->size);
                            if (read_reg_bytes(prev, def, 0, def->size, buf) == 0) {
                                if (write_reg_bytes(down, def, 0, def->size, buf) < 0) exception(errno);
                                down->has_reg_data = 1;
                            }
                        }
                        def++;
                    }
                    down->is_walked = 1;
                    down->inlined = info->sub_cnt - i - 1;
                    down->area = (CodeArea *)loc_alloc(sizeof(CodeArea));
                    *down->area = info->subs[down->inlined]->area;
                    if (down->area->directory) down->area->directory = loc_strdup(down->area->directory);
                    if (down->area->file) down->area->file = loc_strdup(down->area->file);
                    prev->func_id = loc_strdup(info->subs[down->inlined]->func_id);
                    add_frame(stk, down);
                    frame = stk->frames + frame_idx;
                    memset(down, 0, sizeof(StackFrame));
                }
            }
            down->ctx = ctx;
            for (i = 0; i < info->reg_cnt; i++) {
                RegisterDefinition * reg_def = info->regs[i]->reg;
                uint8_t * buf = NULL;
                size_t size = 0;
                Trap trap_reg;
#if ENABLE_StackRegisterLocations
                if (write_reg_location(down, reg_def, info->regs[i]->cmds, info->regs[i]->cmds_cnt) == 0) {
                    down->has_reg_data = 1;
                    continue;
                }
#endif
                if (set_trap(&trap_reg)) {
                    /* If a saved register value cannot be evaluated - ignore it */
                    state = evaluate_location_expression(ctx, frame, info->regs[i]->cmds, info->regs[i]->cmds_cnt, NULL, 0);
                    if (state->stk_pos == 1) {
                        unsigned j;
                        uint64_t v = state->stk[0];
                        buf = (uint8_t *)tmp_alloc_zero(reg_def->size);
                        for (j = 0; j < reg_def->size; j++) {
                            buf[reg_def->big_endian ? reg_def->size - j - 1 : j] = (uint8_t)v;
                            v = v >> 8;
                        }
                        size = reg_def->size;
                    }
                    else if (state->stk_pos == 0 && state->pieces_cnt > 0) {
                        read_location_pieces(state->ctx, state->stack_frame,
                            state->pieces, state->pieces_cnt, state->reg_id_scope.big_endian, (void **)&buf, &size);
                    }
                    clear_trap(&trap_reg);
                }
                if (buf != NULL && size > 0) {
                    if (size > reg_def->size) size = reg_def->size;
                    if (write_reg_bytes(down, reg_def, 0, size, buf) < 0) exception(errno);
                    down->has_reg_data = 1;
                }
            }
            /* Cache miss on register access should not be ignored, abort trace */
            if (cache_miss_count() > 0) exception(ERR_CACHE_MISS);
            clear_trap(&trap);
            frame->is_walked = 1;
        }
        else {
            error = trap.error;
            frame->fp = 0;
        }
    }
    if (error) {
        free_frame(down);
        errno = error;
        return -1;
    }
#else
    memset(down, 0, sizeof(StackFrame));
#endif
    return 0;
}

static void invalidate_stack_trace(StackTrace * stack) {
    int i;
    for (i = 0; i < stack->frame_cnt; i++) {
        free_frame(stack->frames + i);
    }
    stack->frame_cnt = 0;
    stack->complete = 0;
}

static void trace_stack(Context * ctx, StackTrace * stack, int max_frames) {
    StackFrame down;

    if (stack->frame_cnt == 0) {
        memset(&down, 0, sizeof(down));
        down.is_top_frame = 1;
        down.ctx = ctx;
        add_frame(stack, &down);
    }

    trace(LOG_STACK, "Stack trace, ctx %s, max_frames %d", ctx->id, max_frames);
    while (stack->frame_cnt < max_frames) {
        int frame_idx = stack->frame_cnt - 1;
        StackFrame * frame = stack->frames + frame_idx;
#if ENABLE_Trace
        if (LOG_STACK & log_mode) {
            uint64_t v;
            RegisterDefinition * def;
            trace(LOG_STACK, "Frame %d", stack->frame_cnt - 1);
            for (def = get_reg_definitions(ctx); def && def->name; def++) {
                if (def->no_read || def->read_once || def->bits || !def->size) continue;
                if (read_reg_value(frame, def, &v) != 0) continue;
                trace(LOG_STACK, "  %-8s %16" PRIx64, def->name, v);
            }
        }
#endif
        if (get_next_stack_frame(frame, &down) < 0) {
            if (cache_miss_count() > 0) break;
            trace(LOG_STACK, "Stack trace error: %s", errno_to_str(errno));
        }
        frame = stack->frames + frame_idx; /* stack->frames might be realloc-ed */
        if (frame->is_walked == 0) {
            trace(LOG_STACK, "  *** frame info not available ***");
            free_frame(&down);
            memset(&down, 0, sizeof(down));
            down.ctx = ctx;
            if (crawl_stack_frame(frame, &down) < 0) {
                free_frame(&down);
                if (cache_miss_count() > 0) break;
                trace(LOG_STACK, "  crawl error: %s", errno_to_str(errno));
                stack->complete = 1;
                break;
            }
        }
        assert(down.area == NULL);
        trace(LOG_STACK, "  cfa      %16" PRIx64, (uint64_t)frame->fp);
        if (!down.has_reg_data) {
            stack->complete = 1;
            free_frame(&down);
            break;
        }
        if (stack->frame_cnt > 1 && frame->fp == stack->frames[stack->frame_cnt - 2].fp) {
            /* Compare registers in current and next frame */
            int equ = 1;
            size_t buf_size = 8;
            uint8_t * buf0 = (uint8_t *)tmp_alloc(buf_size);
            uint8_t * buf1 = (uint8_t *)tmp_alloc(buf_size);
            RegisterDefinition * def;
            for (def = get_reg_definitions(ctx); def && def->name; def++) {
                int f0, f1;
                if (buf_size < def->size) {
                    buf_size = def->size;
                    buf0 = (uint8_t *)tmp_realloc(buf0, buf_size);
                    buf1 = (uint8_t *)tmp_realloc(buf1, buf_size);
                }
                f0 = read_reg_bytes(frame, def, 0, def->size, buf0) == 0;
                f1 = read_reg_bytes(&down, def, 0, def->size, buf1) == 0;
                if (f0 != f1 || (f0 && memcmp(buf0, buf1, def->size) != 0)) {
                    equ = 0;
                    break;
                }
            }
            if (equ) {
                /* All registers are same - stop tracing */
                stack->complete = 1;
                free_frame(&down);
                break;
            }
        }
#ifdef TRACE_STACK_BOTTOM_CHECK
        TRACE_STACK_BOTTOM_CHECK;
#endif
        add_frame(stack, &down);
    }
}

static StackTrace * create_stack_trace(Context * ctx, int max_frames) {
    StackTrace * stack = EXT(ctx);
    max_frames++; /* Frame pointer and return address calculation needs one more frame */
    if (!stack->complete && stack->frame_cnt < max_frames) {
        trace_stack(ctx, stack, max_frames);
        if (cache_miss_count() > 0) {
            errno = ERR_CACHE_MISS;
            return NULL;
        }
    }
    return stack;
}

typedef struct CommandGetContextData {
    Context * ctx;
    int frame;
    StackTrace * stack;
    StackFrame * info;
    StackFrame * down;
    uint64_t ip;
    uint64_t rp;
    int ip_error;
    int rp_error;
} CommandGetContextData;

static void write_context(OutputStream * out, char * id, CommandGetContextData * d) {
    write_stream(out, '{');

    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, id);

    write_stream(out, ',');
    json_write_string(out, "ParentID");
    write_stream(out, ':');
    json_write_string(out, d->ctx->id);

    write_stream(out, ',');
    json_write_string(out, "ProcessID");
    write_stream(out, ':');
    json_write_string(out, context_get_group(d->ctx, CONTEXT_GROUP_PROCESS)->id);

    write_stream(out, ',');
    json_write_string(out, "Index");
    write_stream(out, ':');
    json_write_long(out, d->frame);

    if (d->stack->complete) {
        write_stream(out, ',');
        json_write_string(out, "Level");
        write_stream(out, ':');
        json_write_long(out, d->stack->frame_cnt - d->frame - 1);
    }

    if (d->info->is_top_frame) {
        write_stream(out, ',');
        json_write_string(out, "TopFrame");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }

    if (d->info->is_walked) {
        write_stream(out, ',');
        json_write_string(out, "Walk");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }

    if (d->info->fp) {
        write_stream(out, ',');
        json_write_string(out, "FP");
        write_stream(out, ':');
        json_write_uint64(out, d->info->fp);
    }

    if (d->info->inlined) {
        write_stream(out, ',');
        json_write_string(out, "Inlined");
        write_stream(out, ':');
        json_write_long(out, d->info->inlined);
    }

    if (d->info->func_id != NULL) {
        write_stream(out, ',');
        json_write_string(out, "FuncID");
        write_stream(out, ':');
        json_write_string(out, d->info->func_id);
    }

    if (d->info->area != NULL) {
        write_stream(out, ',');
        json_write_string(out, "CodeArea");
        write_stream(out, ':');
        write_code_area(out, d->info->area, NULL);
    }

    if (d->ip_error == 0) {
        write_stream(out, ',');
        json_write_string(out, "IP");
        write_stream(out, ':');
        json_write_uint64(out, d->ip);
    }

    if (d->rp_error == 0) {
        write_stream(out, ',');
        json_write_string(out, "RP");
        write_stream(out, ':');
        json_write_uint64(out, d->rp);
    }

    write_stream(out, '}');
}

typedef struct CommandGetContextArgs {
    char token[256];
    int id_cnt;
    char ** ids;
} CommandGetContextArgs;

static void command_get_context_cache_client(void * x) {
    int i;
    int err = 0;
    Channel * c = cache_channel();
    CommandGetContextArgs * args = (CommandGetContextArgs *)x;
    CommandGetContextData * data = (CommandGetContextData *)
        tmp_alloc_zero(sizeof(CommandGetContextData) * args->id_cnt);

    for (i = 0; i < args->id_cnt; i++) {
        StackTrace * stack = NULL;
        CommandGetContextData * d = data + i;
        RegisterDefinition * reg_ip = NULL;
        if (id2frame(args->ids[i], &d->ctx, &d->frame) < 0) {
            err = errno;
            break;
        }
        if (!is_ctx_stopped(d->ctx)) {
            err = errno;
            break;
        }
        assert(d->frame >= 0);
        stack = create_stack_trace(d->ctx, d->frame + 1);
        if (stack == NULL) {
            err = errno;
            break;
        }
        if (d->frame >= stack->frame_cnt) {
            assert(stack->complete);
            err = ERR_INV_CONTEXT;
            break;
        }
        d->stack = stack;
        d->info = stack->frames + d->frame;
        d->down = d->frame < stack->frame_cnt - 1 ? d->info + 1 : NULL;

        reg_ip = get_PC_definition(d->ctx);
        if (reg_ip == NULL || d->info == NULL) d->ip_error = ERR_OTHER;
        else if (read_reg_value(d->info, reg_ip, &d->ip) < 0) d->ip_error = errno;
        if (reg_ip == NULL || d->down == NULL) d->rp_error = ERR_OTHER;
        else if (read_reg_value(d->down, reg_ip, &d->rp) < 0) d->rp_error = errno;
    }

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);
    write_stream(&c->out, '[');
    for (i = 0; i < args->id_cnt; i++) {
        CommandGetContextData * d = data + i;
        if (i > 0) write_stream(&c->out, ',');
        if (d->info == NULL) {
            write_string(&c->out, "null");
        }
        else {
            write_context(&c->out, args->ids[i], d);
        }
    }
    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);

    loc_free(args->ids);
}

static void command_get_context(char * token, Channel * c) {
    CommandGetContextArgs args;

    args.ids = json_read_alloc_string_array(&c->inp, &args.id_cnt);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_context_cache_client, c, &args, sizeof(args));
}

typedef struct CommandGetChildrenArgs {
    char token[256];
    char id[256];
    int min_frame;
    int max_frame;
    int all_frames;
} CommandGetChildrenArgs;

static void command_get_children_cache_client(void * x) {
    int err = 0;
    Context * ctx = NULL;
    StackTrace * stack = NULL;
    CommandGetChildrenArgs * args = (CommandGetChildrenArgs *)x;
    Channel * c = cache_channel();

    ctx = id2ctx(args->id);
    if (ctx == NULL || !context_has_state(ctx)) {
        /* no children */
    }
    else if (!is_ctx_stopped(ctx)) {
        err = errno;
    }
    else if (args->all_frames) {
        stack = create_stack_trace(ctx, MAX_FRAMES);
        if (stack == NULL) err = errno;
    }
    else {
        stack = create_stack_trace(ctx, EXT(ctx)->inlined + args->max_frame + 1);
        if (stack == NULL) err = errno;
    }

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);

    write_errno(&c->out, err);

    if (stack == NULL) {
        write_stringz(&c->out, "null");
    }
    else {
        int i;
        int j = 0;
        int inlined = EXT(ctx)->inlined;
        write_stream(&c->out, '[');
        if (args->all_frames) {
            for (i = 0; i < stack->frame_cnt - inlined; i++) {
                if (j > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, frame2id(ctx, stack->frame_cnt - i - 1));
                j++;
            }
        }
        else {
            for (i = inlined + args->min_frame; i <= inlined + args->max_frame && i < stack->frame_cnt; i++) {
                if (j > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, frame2id(ctx, i));
                j++;
            }
        }
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);
    }

    write_stream(&c->out, MARKER_EOM);
}

static void command_get_children(char * token, Channel * c) {
    CommandGetChildrenArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    args.all_frames = 1;
    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_children_cache_client, c, &args, sizeof(args));
}

static void command_get_children_range(char * token, Channel * c) {
    CommandGetChildrenArgs args;

    memset(&args, 0, sizeof(args));
    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    args.min_frame = (int)json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    args.max_frame = (int)json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_children_cache_client, c, &args, sizeof(args));
}

int get_top_frame(Context * ctx) {
    if (!is_ctx_stopped(ctx)) return STACK_TOP_FRAME;
    return 0;
}

int get_bottom_frame(Context * ctx) {
    StackTrace * stack;

    if (!is_ctx_stopped(ctx)) return STACK_BOTTOM_FRAME;

    stack = create_stack_trace(ctx, MAX_FRAMES);
    if (stack == NULL) return STACK_BOTTOM_FRAME;

    assert(stack->frame_cnt > 0);
    return stack->frame_cnt - 1;
}

int get_prev_frame(Context * ctx, int frame) {
    StackTrace * stack;

    if (frame == STACK_TOP_FRAME) {
        frame = get_top_frame(ctx);
        if (frame < 0) return frame;
    }

    if (frame < 0) {
        set_errno(ERR_OTHER, "No previous stack frame");
        return STACK_NO_FRAME;
    }

    stack = create_stack_trace(ctx, frame + 2);
    if (stack == NULL) return STACK_NO_FRAME;

    if (frame + 1 >= stack->frame_cnt) {
        set_errno(ERR_OTHER, "No previous stack frame");
        return STACK_NO_FRAME;
    }

    return frame + 1;
}

int get_next_frame(Context * ctx, int frame) {

    if (frame == STACK_BOTTOM_FRAME) {
        frame = get_bottom_frame(ctx);
        if (frame < 0) return frame;
    }

    if (frame <= 0) {
        set_errno(ERR_OTHER, "No next stack frame");
        return STACK_NO_FRAME;
    }

    return frame - 1;
}

int get_frame_info(Context * ctx, int frame, StackFrame ** info) {
    StackTrace * stack;
    int max_frames = 0;

    *info = NULL;
    if (!is_ctx_stopped(ctx)) return -1;

    if (frame >= 0) max_frames = frame + 1;
    else if (frame == STACK_TOP_FRAME) max_frames = 1;
    else if (frame == STACK_BOTTOM_FRAME) max_frames = MAX_FRAMES;

    stack = create_stack_trace(ctx, max_frames);
    if (stack == NULL) return -1;

    if (frame == STACK_TOP_FRAME) {
        frame = 0;
    }
    else if (frame == STACK_BOTTOM_FRAME) {
        if (!stack->complete) {
            set_errno(ERR_INV_CONTEXT, "No such stack frame");
            return -1;
        }
        frame = stack->frame_cnt - 1;
    }
    else if (frame < 0 || frame >= stack->frame_cnt) {
        set_errno(ERR_INV_CONTEXT, "No such stack frame");
        return -1;
    }

    *info = stack->frames + frame;
    return 0;
}

int get_cached_frame_info(Context * ctx, int frame, StackFrame ** info) {
    StackTrace * stack = EXT(ctx);

    *info = NULL;
    if (ctx == NULL || !context_has_state(ctx)) {
        errno = ERR_INV_CONTEXT;
        return -1;
    }
    if (!ctx->stopped) {
        errno = ERR_IS_RUNNING;
        return -1;
    }

    if (frame == STACK_TOP_FRAME) {
        frame = 0;
    }
    else if (frame == STACK_BOTTOM_FRAME) {
        if (!stack->complete) {
            set_errno(ERR_INV_CONTEXT, "No such stack frame");
            return -1;
        }
        frame = stack->frame_cnt - 1;
    }
    else if (frame < 0 || frame >= stack->frame_cnt) {
        set_errno(ERR_INV_CONTEXT, "No such stack frame");
        return -1;
    }

    *info = stack->frames + frame;
    return 0;
}

void set_inlined_frame_level(Context * ctx, int level) {
    EXT(ctx)->inlined = level;
}

int get_inlined_frame_level(Context * ctx) {
    return EXT(ctx)->inlined;
}

static void flush_stack_trace(Context * ctx, void * args) {
    invalidate_stack_trace(EXT(ctx));
    EXT(ctx)->inlined = 0;
}

#if SERVICE_Registers
static void flush_on_register_change(Context * ctx, int frame, RegisterDefinition * def, void * args) {
    invalidate_stack_trace(EXT(ctx));
}
#endif

static void delete_stack_trace(Context * ctx, void * args) {
    invalidate_stack_trace(EXT(ctx));
    loc_free(EXT(ctx)->frames);
    memset(EXT(ctx), 0, sizeof(StackTrace));
}

#if SERVICE_MemoryMap
static void event_map_changed(Context * ctx, void * args) {
    if (ctx->mem_access && context_get_group(ctx, CONTEXT_GROUP_PROCESS) == ctx) {
        /* If the context is a memory space, we need to invalidate
         * stack traces on all members of the group, since they can
         * dependent on the symbol information. */
        LINK * l = context_root.next;
        while (l != &context_root) {
            Context * x = ctxl2ctxp(l);
            l = l->next;
            if (x->exited) continue;
            if (context_get_group(x, CONTEXT_GROUP_PROCESS) != ctx) continue;
            invalidate_stack_trace(EXT(x));
        }
    }
}
#endif

void ini_stack_trace_service(Protocol * proto, TCFBroadcastGroup * bcg) {
    static int ini_done = 0;
    if (!ini_done) {
        static ContextEventListener context_listener = {
            NULL,
            flush_stack_trace,
            NULL,
            flush_stack_trace,
            flush_stack_trace,
            delete_stack_trace
        };
#if SERVICE_Registers
        static RegistersEventListener registers_listener = {
            flush_on_register_change,
        };
#endif
#if SERVICE_MemoryMap
        static MemoryMapEventListener map_listener = {
            NULL,
            NULL,
            NULL,
            event_map_changed,
        };
#endif
        ini_done = 1;
        add_context_event_listener(&context_listener, NULL);
#if SERVICE_Registers
        add_registers_event_listener(&registers_listener, NULL);
#endif
#if SERVICE_MemoryMap
        add_memory_map_event_listener(&map_listener, NULL);
#endif
        context_extension_offset = context_extension(sizeof(StackTrace));
    }
    add_command_handler(proto, STACKTRACE, "getContext", command_get_context);
    add_command_handler(proto, STACKTRACE, "getChildren", command_get_children);
    add_command_handler(proto, STACKTRACE, "getChildrenRange", command_get_children_range);
}

#endif
