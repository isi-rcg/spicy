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
 * TCF Registers - CPU registers access service.
 */

#include <tcf/config.h>

#if SERVICE_Registers

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/json.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/stacktrace.h>
#include <tcf/services/registers.h>

static const char * REGISTERS = "Registers";

static TCFBroadcastGroup * broadcast_group = NULL;

typedef struct Listener {
    RegistersEventListener * func;
    void * args;
} Listener;

static Listener * listeners = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;

typedef struct Notification {
    LINK link_all;
    Context * ctx;
    int frame;
    RegisterDefinition * def;
    char * id;
} Notification;

static int notify_definitions_changed = 0;
static LINK notifications = TCF_LIST_INIT(notifications);
#define all2notification(link) ((Notification *)((char *)(link) - offsetof(Notification, link_all)))

static uint8_t * bbf = NULL;
static unsigned bbf_pos = 0;
static unsigned bbf_len = 0;

static void write_boolean_member(OutputStream * out, const char * name, int val) {
    /* For this service FALSE is same as absence of the member */
    if (!val) return;
    write_stream(out, ',');
    json_write_string(out, name);
    write_stream(out, ':');
    json_write_boolean(out, 1);
}

static void write_context(OutputStream * out, char * id,
        Context * ctx, int frame, RegisterDefinition * reg_def) {

    assert(!ctx->exited);

    write_stream(out, '{');

    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, id);

    write_stream(out, ',');
    json_write_string(out, "ParentID");
    write_stream(out, ':');
    if (reg_def->parent != NULL) {
        json_write_string(out, register2id(ctx, frame, reg_def->parent));
    }
    else if (frame < 0 || is_top_frame(ctx, frame)) {
        json_write_string(out, ctx->id);
    }
    else {
        json_write_string(out, frame2id(ctx, frame));
    }

    write_stream(out, ',');
    json_write_string(out, "ProcessID");
    write_stream(out, ':');
    json_write_string(out, context_get_group(ctx, CONTEXT_GROUP_PROCESS)->id);

    write_stream(out, ',');
    json_write_string(out, "Name");
    write_stream(out, ':');
    json_write_string(out, reg_def->name);

    if (reg_def->size > 0) {
        write_stream(out, ',');
        json_write_string(out, "Size");
        write_stream(out, ':');
        json_write_long(out, reg_def->size);
    }

    if (reg_def->dwarf_id >= 0) {
        write_stream(out, ',');
        json_write_string(out, "DwarfID");
        write_stream(out, ':');
        json_write_long(out, reg_def->dwarf_id);
    }

    if (reg_def->eh_frame_id >= 0) {
        write_stream(out, ',');
        json_write_string(out, "EhFrameID");
        write_stream(out, ':');
        json_write_long(out, reg_def->eh_frame_id);
    }

    write_boolean_member(out, "BigEndian", reg_def->big_endian);
    write_boolean_member(out, "Float", reg_def->fp_value);
    write_boolean_member(out, "Readable", !reg_def->no_read);
    write_boolean_member(out, "Writeable", !reg_def->no_write);
    write_boolean_member(out, "ReadOnce", reg_def->read_once);
    write_boolean_member(out, "WriteOnce", reg_def->write_once);
    write_boolean_member(out, "Volatile", reg_def->volatile_value);
    write_boolean_member(out, "SideEffects", reg_def->side_effects);
    write_boolean_member(out, "LeftToRight", reg_def->left_to_right);

    if (reg_def->first_bit > 0) {
        write_stream(out, ',');
        json_write_string(out, "FirstBit");
        write_stream(out, ':');
        json_write_long(out, reg_def->first_bit);
    }

    if (reg_def->bits != NULL) {
        int i = 0;
        write_stream(out, ',');
        json_write_string(out, "Bits");
        write_stream(out, ':');
        write_stream(out, '[');
        while (reg_def->bits[i] >= 0) {
            if (i > 0) write_stream(out, ',');
            json_write_long(out, reg_def->bits[i++]);
        }
        write_stream(out, ']');
    }

    if (reg_def->values != NULL) {
        int i = 0;
        write_stream(out, ',');
        json_write_string(out, "Values");
        write_stream(out, ':');
        write_stream(out, '[');
        while (reg_def->values[i] != NULL) {
            NamedRegisterValue * v = reg_def->values[i++];
            if (i > 1) write_stream(out, ',');
            write_stream(out, '{');
            json_write_string(out, "Value");
            write_stream(out, ':');
            json_write_binary(out, v->value, reg_def->size);
            if (v->name != NULL) {
                write_stream(out, ',');
                json_write_string(out, "Name");
                write_stream(out, ':');
                json_write_string(out, v->name);
            }
            if (v->description != NULL) {
                write_stream(out, ',');
                json_write_string(out, "Description");
                write_stream(out, ':');
                json_write_string(out, v->description);
            }
            write_stream(out, '}');
        }
        write_stream(out, ']');
    }

    if (reg_def->memory_address > 0) {
        write_stream(out, ',');
        json_write_string(out, "MemoryAddress");
        write_stream(out, ':');
        json_write_uint64(out, reg_def->memory_address);
    }

    if (reg_def->memory_context != NULL) {
        write_stream(out, ',');
        json_write_string(out, "MemoryContext");
        write_stream(out, ':');
        json_write_string(out, reg_def->memory_context);
    }

    if (reg_def->role != NULL) {
        write_stream(out, ',');
        json_write_string(out, "Role");
        write_stream(out, ':');
        json_write_string(out, reg_def->role);
    }
    else if (reg_def == get_PC_definition(ctx)) {
        write_stream(out, ',');
        json_write_string(out, "Role");
        write_stream(out, ':');
        json_write_string(out, "PC");
    }

    if (reg_def->description != NULL) {
        write_stream(out, ',');
        json_write_string(out, "Description");
        write_stream(out, ':');
        json_write_string(out, reg_def->description);
    }

    if (reg_def->size > 0) {
        RegisterDefinition * parent_reg_def = NULL;
        parent_reg_def = reg_def->parent;
        while (parent_reg_def != NULL && parent_reg_def->size == 0) parent_reg_def = parent_reg_def->parent;
        if (parent_reg_def != NULL) {
            if (reg_def->offset >= parent_reg_def->offset &&
                reg_def->offset + reg_def->size <= parent_reg_def->offset + parent_reg_def->size) {
                write_stream(out, ',');
                json_write_string(out, "Offset");
                write_stream(out, ':');
                json_write_uint64(out, reg_def->offset - parent_reg_def->offset);
            }
        }
    }

    write_stream(out, '}');
    write_stream(out, 0);
}

typedef struct GetContextArgs {
    char token[256];
    char id[256];
} GetContextArgs;

static void command_get_context_cache_client(void * x) {
    GetContextArgs * args = (GetContextArgs *)x;
    Channel * c  = cache_channel();
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    RegisterDefinition * reg_def = NULL;
    Trap trap;

    if (set_trap(&trap)) {
        if (id2register(args->id, &ctx, &frame, &reg_def) < 0) exception(errno);
        clear_trap(&trap);
    }

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);
    write_errno(&c->out, trap.error);
    if (reg_def != NULL) {
        write_context(&c->out, args->id, ctx, frame, reg_def);
    }
    else {
        write_stringz(&c->out, "null");
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_context(char * token, Channel * c) {
    GetContextArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_context_cache_client, c, &args, sizeof(args));
}

typedef struct GetChildrenArgs {
    char token[256];
    char id[256];
} GetChildrenArgs;

static void command_get_children_cache_client(void * x) {
    GetChildrenArgs * args = (GetChildrenArgs *)x;
    Channel * c  = cache_channel();
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    StackFrame * frame_info = NULL;
    RegisterDefinition * defs = NULL;
    RegisterDefinition * parent = NULL;
    Trap trap;

    if (set_trap(&trap)) {
        if (id2register(args->id, &ctx, &frame, &parent) == 0) {
            if (frame != STACK_TOP_FRAME && get_frame_info(ctx, frame, &frame_info) < 0) exception(errno);
        }
        else if (id2frame(args->id, &ctx, &frame) == 0) {
            if (get_frame_info(ctx, frame, &frame_info) < 0) exception(errno);
        }
        else {
            ctx = id2ctx(args->id);
            frame = STACK_TOP_FRAME;
        }
        if (ctx != NULL) defs = get_reg_definitions(ctx);
        clear_trap(&trap);
    }

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);

    write_errno(&c->out, trap.error);

    write_stream(&c->out, '[');
#if ENABLE_RegisterChildrenRefs
    if (defs != NULL && (defs->children != NULL || defs->sibling != NULL) &&
            defs->parent == NULL && (frame < 0 || frame_info->is_top_frame)) {
        int cnt = 0;
        RegisterDefinition * reg_def = parent == NULL ? defs : parent->children;
        while (reg_def != NULL) {
            assert(reg_def->parent == parent);
            if (cnt > 0) write_stream(&c->out, ',');
            json_write_string(&c->out, register2id(ctx, frame, reg_def));
            reg_def = reg_def->sibling;
            cnt++;
        }
        defs = NULL;
    }
#endif
    if (defs != NULL) {
        int cnt = 0;
        RegisterDefinition * reg_def;
        for (reg_def = defs; reg_def->name != NULL; reg_def++) {
            if (reg_def->parent != parent) continue;
            if (frame < 0 || frame_info->is_top_frame ||
                    reg_def->size == 0 || read_reg_value(frame_info, reg_def, NULL) == 0) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, register2id(ctx, frame, reg_def));
                cnt++;
            }
        }
    }
    write_stream(&c->out, ']');
    write_stream(&c->out, 0);

    write_stream(&c->out, MARKER_EOM);
}

static void command_get_children(char * token, Channel * c) {
    GetChildrenArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_children_cache_client, c, &args, sizeof(args));
}

static void flush_notifications(void) {
    OutputStream * out = &broadcast_group->out;

    assert(cache_miss_count() == 0);

    if (notify_definitions_changed) {
        unsigned i;
        for (i = 0; i < listener_cnt; i++) {
            Listener * l = listeners + i;
            if (l->func->register_definitions_changed == NULL) continue;
            l->func->register_definitions_changed(l->args);
        }

        write_stringz(out, "E");
        write_stringz(out, REGISTERS);
        write_stringz(out, "contextChanged");
        write_stream(out, 0);
        write_stream(out, MARKER_EOM);
    }

    while (!list_is_empty(&notifications)) {
        Notification * n = all2notification(notifications.next);
        list_remove(&n->link_all);

        if (!notify_definitions_changed && !n->ctx->exited) {
            unsigned i;
            for (i = 0; i < listener_cnt; i++) {
                Listener * l = listeners + i;
                if (l->func->register_changed == NULL) continue;
                l->func->register_changed(n->ctx, n->frame, n->def, l->args);
            }

            write_stringz(out, "E");
            write_stringz(out, REGISTERS);
            write_stringz(out, "registerChanged");
            json_write_string(out, n->id);
            write_stream(out, 0);
            write_stream(out, MARKER_EOM);
        }
        context_unlock(n->ctx);
        loc_free(n->id);
        loc_free(n);
    }

    notify_definitions_changed = 0;
}

void send_event_register_changed(const char * id) {
    Context * ctx = NULL;
    int frame = STACK_NO_FRAME;
    RegisterDefinition * def = NULL;
    Notification * n = NULL;
    LINK * l = NULL;

    id2register(id, &ctx, &frame, &def);
    if (ctx == NULL) return;
    assert(!ctx->exited);
    for (l = notifications.next; l != &notifications; l = l->next) {
        n = all2notification(l);
        if (n->ctx == ctx && n->frame == frame && n->def == def) return;
    }
    if (frame >= 0 && frame == get_top_frame(ctx)) {
        id = register2id(ctx, STACK_TOP_FRAME, def);
    }
    n = (Notification *)loc_alloc_zero(sizeof(Notification));
    n->ctx = ctx;
    n->frame = frame;
    n->def = def;
    n->id = loc_strdup(id);
    context_lock(n->ctx);
    list_add_last(&n->link_all, &notifications);

    /* Delay notifications until cache transaction is committed */
    if (cache_transaction_id() == 0) flush_notifications();
}

void send_event_register_definitions_changed(void) {
    notify_definitions_changed = 1;
    /* Delay notifications until cache transaction is committed */
    if (cache_transaction_id() == 0) flush_notifications();
}

typedef struct GetArgs {
    char token[256];
    char id[256];
} GetArgs;

static void command_get_cache_client(void * x) {
    GetArgs * args = (GetArgs *)x;
    Channel * c  = cache_channel();
    Trap trap;

    bbf_pos = 0;
    if (set_trap(&trap)) {
        int frame = 0;
        Context * ctx = NULL;
        RegisterDefinition * reg_def = NULL;

        if (id2register(args->id, &ctx, &frame, &reg_def) < 0) exception(errno);
        if (ctx->exited) exception(ERR_ALREADY_EXITED);
        if ((ctx->reg_access & REG_ACCESS_RD_STOP) != 0) {
            check_all_stopped(ctx);
        }
        if ((ctx->reg_access & REG_ACCESS_RD_RUNNING) == 0) {
            if (context_has_state(ctx) && !is_ctx_stopped(ctx))
                str_exception(errno, "Cannot read register if not stopped");
        }
        if (reg_def->size > bbf_len) {
            bbf_len += 0x100 + reg_def->size;
            bbf = (uint8_t *)loc_realloc(bbf, bbf_len);
        }

        bbf_pos = reg_def->size;
        memset(bbf, 0, reg_def->size);
        if (frame < 0 || is_top_frame(ctx, frame)) {
            if (context_read_reg(ctx, reg_def, 0, reg_def->size, bbf) < 0) exception(errno);
        }
        else {
            StackFrame * info = NULL;
            if (get_frame_info(ctx, frame, &info) < 0) exception(errno);
            if (read_reg_bytes(info, reg_def, 0, reg_def->size, bbf) < 0) exception(errno);
        }

        clear_trap(&trap);
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, trap.error);
        json_write_binary(&c->out, bbf, bbf_pos);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
    }

    run_ctrl_unlock();
}

static void command_get(char * token, Channel * c) {
    GetArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_get_cache_client, c, &args, sizeof(args));
}

typedef struct SetArgs {
    char token[256];
    char id[256];
    size_t data_len;
    uint8_t * data;
} SetArgs;

static void command_set_cache_client(void * x) {
    SetArgs * args = (SetArgs *)x;
    Channel * c  = cache_channel();
    Trap trap;

    if (set_trap(&trap)) {
        int frame = 0;
        Context * ctx = NULL;
        RegisterDefinition * reg_def = NULL;

        if (id2register(args->id, &ctx, &frame, &reg_def) < 0) exception(errno);
        if (frame >= 0 && !is_top_frame(ctx, frame)) exception(ERR_INV_CONTEXT);
        if (ctx->exited) exception(ERR_ALREADY_EXITED);
        if ((ctx->reg_access & REG_ACCESS_WR_STOP) != 0) {
            check_all_stopped(ctx);
        }
        if ((ctx->reg_access & REG_ACCESS_WR_RUNNING) == 0) {
            if (context_has_state(ctx) && !is_ctx_stopped(ctx))
                str_exception(errno, "Cannot write register if not stopped");
        }
        if ((size_t)args->data_len > reg_def->size) exception(ERR_INV_DATA_SIZE);
        if (args->data_len > 0) {
            if (context_write_reg(ctx, reg_def, 0, args->data_len, args->data) < 0) exception(errno);
            send_event_register_changed(args->id);
        }
        clear_trap(&trap);
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, trap.error);
        write_stream(&c->out, MARKER_EOM);
    }

    loc_free(args->data);
    run_ctrl_unlock();
}

static void command_set(char * token, Channel * c) {
    SetArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    args.data = (uint8_t *)json_read_alloc_binary(&c->inp, &args.data_len);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_set_cache_client, c, &args, sizeof(args));
}

typedef struct Location {
    char id[256];
    Context * ctx;
    int frame;
    StackFrame * frame_info;
    RegisterDefinition * reg_def;
    unsigned offs;
    unsigned size;
} Location;

static Location * buf = NULL;
static unsigned buf_pos = 0;
static unsigned buf_len = 0;
static unsigned loc_pos = 0;

static void read_location_element(InputStream * inp, void * args) {
    Location * loc = (Location *)args;
    switch (loc_pos) {
    case 0:
        json_read_string(inp, loc->id, sizeof(loc->id));
        break;
    case 1:
        loc->offs = json_read_ulong(inp);
        break;
    case 2:
        loc->size = json_read_ulong(inp);
        break;
    default:
        json_skip_object(inp);
        break;
    }
    loc_pos++;
}

static void read_location(InputStream * inp, void * args) {
    Location * loc = NULL;
    if (buf_pos >= buf_len) {
        buf_len = buf_len == 0 ? 0x10 : buf_len * 2;
        buf = (Location *)loc_realloc(buf, buf_len * sizeof(Location));
    }
    loc_pos = 0;
    loc = buf + buf_pos++;
    memset(loc, 0, sizeof(Location));
    json_read_array(inp, read_location_element, loc);
}

static Location * read_location_list(InputStream * inp, unsigned * cnt) {
    Location * locs = NULL;

    buf_pos = 0;
    json_read_array(inp, read_location, NULL);
    locs = (Location *)loc_alloc(buf_pos * sizeof(Location));
    memcpy(locs, buf, buf_pos * sizeof(Location));
    *cnt = buf_pos;
    return locs;
}

static void check_location_list(Location * locs, unsigned cnt, int setm) {
    unsigned pos;
    for (pos = 0; pos < cnt; pos++) {
        Location * loc = locs + pos;

        if (id2register(loc->id, &loc->ctx, &loc->frame, &loc->reg_def) < 0) exception(errno);
        if (loc->ctx->exited) exception(ERR_ALREADY_EXITED);
        if ((loc->ctx->reg_access & setm ? REG_ACCESS_WR_STOP : REG_ACCESS_RD_STOP) != 0) {
            check_all_stopped(loc->ctx);
        }
        if ((loc->ctx->reg_access & setm ? REG_ACCESS_WR_RUNNING : REG_ACCESS_RD_RUNNING) == 0) {
            if (context_has_state(loc->ctx) && !is_ctx_stopped(loc->ctx))
                str_fmt_exception(errno, "Cannot %s register if not stopped", setm ? "write" : "read");
        }
        if (loc->offs + loc->size > loc->reg_def->size) exception(ERR_INV_DATA_SIZE);

        if (loc->frame < 0 || is_top_frame(loc->ctx, loc->frame)) continue;

        if (setm) exception(ERR_INV_CONTEXT);
        if (get_frame_info(loc->ctx, loc->frame, &loc->frame_info) < 0) exception(errno);
    }
}

typedef struct GetmArgs {
    char token[256];
    unsigned locs_cnt;
    Location * locs;
} GetmArgs;

static void command_getm_cache_client(void * x) {
    GetmArgs * args = (GetmArgs *)x;
    Channel * c  = cache_channel();
    Trap trap;

    bbf_pos = 0;
    if (bbf == NULL) bbf = (uint8_t *)loc_alloc(bbf_len = 0x100);
    if (set_trap(&trap)) {
        unsigned locs_pos = 0;
        check_location_list(args->locs, args->locs_cnt, 0);
        while (locs_pos < args->locs_cnt) {
            Location * l = args->locs + locs_pos++;
            if (bbf_pos + l->size > bbf_len) {
                bbf_len += 0x100 + l->size;
                bbf = (uint8_t *)loc_realloc(bbf, bbf_len);
            }
            memset(bbf + bbf_pos, 0, l->size);
            if (l->frame_info == NULL) {
                if (context_read_reg(l->ctx, l->reg_def, l->offs, l->size, bbf + bbf_pos) < 0) exception(errno);
            }
            else {
                if (read_reg_bytes(l->frame_info, l->reg_def, l->offs, l->size, bbf + bbf_pos) < 0) exception(errno);
            }
            bbf_pos += l->size;
        }
        clear_trap(&trap);
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, trap.error);
        json_write_binary(&c->out, bbf, bbf_pos);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
    }

    loc_free(args->locs);
    run_ctrl_unlock();
}

static void command_getm(char * token, Channel * c) {
    GetmArgs args;

    args.locs = read_location_list(&c->inp, &args.locs_cnt);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_getm_cache_client, c, &args, sizeof(args));
}

typedef struct SetmArgs {
    char token[256];
    unsigned locs_cnt;
    Location * locs;
    size_t data_len;
    uint8_t * data;
} SetmArgs;

static void command_setm_cache_client(void * x) {
    SetmArgs * args = (SetmArgs *)x;
    Channel * c  = cache_channel();
    Trap trap;

    if (set_trap(&trap)) {
        unsigned locs_pos = 0;
        unsigned data_pos = 0;
        check_location_list(args->locs, args->locs_cnt, 1);
        while (locs_pos < args->locs_cnt) {
            Location * l = args->locs + locs_pos++;
            assert(l->frame_info == NULL);
            if (l->size > 0) {
                if (context_write_reg(l->ctx, l->reg_def, l->offs, l->size, args->data + data_pos) < 0) exception(errno);
                send_event_register_changed(l->id);
                data_pos += l->size;
            }
        }
        clear_trap(&trap);
    }

    cache_exit();

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        write_errno(&c->out, trap.error);
        write_stream(&c->out, MARKER_EOM);
    }

    loc_free(args->locs);
    loc_free(args->data);
    run_ctrl_unlock();
}

static void command_setm(char * token, Channel * c) {
    SetmArgs args;

    args.locs = read_location_list(&c->inp, &args.locs_cnt);
    json_test_char(&c->inp, MARKER_EOA);
    args.data = (uint8_t *)json_read_alloc_binary(&c->inp, &args.data_len);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    run_ctrl_lock();
    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(command_setm_cache_client, c, &args, sizeof(args));
}

static void read_filter_attrs(InputStream * inp, const char * nm, void * arg) {
    json_skip_object(inp);
}

static void command_search(char * token, Channel * c) {
    char id[256];

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_read_struct(&c->inp, read_filter_attrs, NULL);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, ERR_UNSUPPORTED);
    write_stringz(&c->out, "null");
    write_stream(&c->out, MARKER_EOM);
}

void add_registers_event_listener(RegistersEventListener * listener, void * args) {
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listeners = (Listener *)loc_realloc(listeners, listener_max * sizeof(Listener));
    }
    listeners[listener_cnt].func = listener;
    listeners[listener_cnt].args = args;
    listener_cnt++;
}

static void cache_transaction_listener(int evt) {
    if (evt == CTLE_COMMIT) flush_notifications();
}

void ini_registers_service(Protocol * proto, TCFBroadcastGroup * bcg) {
    broadcast_group = bcg;
    add_command_handler(proto, REGISTERS, "getContext", command_get_context);
    add_command_handler(proto, REGISTERS, "getChildren", command_get_children);
    add_command_handler(proto, REGISTERS, "get", command_get);
    add_command_handler(proto, REGISTERS, "set", command_set);
    add_command_handler(proto, REGISTERS, "getm", command_getm);
    add_command_handler(proto, REGISTERS, "setm", command_setm);
    add_command_handler(proto, REGISTERS, "search", command_search);
    add_cache_transaction_listener(cache_transaction_listener);
}

#endif /* SERVICE_Registers */
