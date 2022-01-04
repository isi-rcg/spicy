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
 * TCF service line Numbers - common part.
 *
 * The service associates locations in the source files with the corresponding
 * machine instruction addresses in the executable object.
 */

#include <tcf/config.h>

#if ENABLE_DebugContext

#include <errno.h>
#include <assert.h>
#include <stdio.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/json.h>
#include <tcf/framework/trace.h>
#include <tcf/services/linenumbers.h>

static void read_code_area_props(InputStream * inp, const char * name, void * args) {
    CodeArea * area = (CodeArea *)args;
    if (strcmp(name, "SLine") == 0) area->start_line = json_read_long(inp);
    else if (strcmp(name, "SCol") == 0) area->start_column = json_read_long(inp);
    else if (strcmp(name, "SAddr") == 0) area->start_address = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "ELine") == 0) area->end_line = json_read_long(inp);
    else if (strcmp(name, "ECol") == 0) area->end_column = json_read_long(inp);
    else if (strcmp(name, "EAddr") == 0) area->end_address = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "NAddr") == 0) area->next_address = (ContextAddress)json_read_uint64(inp);
    else if (strcmp(name, "File") == 0) area->file = json_read_alloc_string(inp);
    else if (strcmp(name, "Dir") == 0) area->directory = json_read_alloc_string(inp);
    else if (strcmp(name, "ISA") == 0) area->isa = json_read_long(inp);
    else if (strcmp(name, "IsStmt") == 0) area->is_statement = json_read_boolean(inp);
    else if (strcmp(name, "BasicBlock") == 0) area->basic_block = json_read_boolean(inp);
    else if (strcmp(name, "PrologueEnd") == 0) area->prologue_end = json_read_boolean(inp);
    else if (strcmp(name, "EpilogueBegin") == 0) area->epilogue_begin = json_read_boolean(inp);
    else if (strcmp(name, "OpIndex") == 0) area->op_index = json_read_long(inp);
    else if (strcmp(name, "Discriminator") == 0) area->discriminator = json_read_long(inp);
    else if (strcmp(name, "NStmtAddr") == 0) area->next_stmt_address = (ContextAddress)json_read_uint64(inp);
    else json_skip_object(inp);
}

void read_code_area(InputStream * inp, CodeArea * area) {
    memset(area, 0, sizeof(CodeArea));
    json_read_struct(inp, read_code_area_props, area);
}

static int client_supports_line_info_extensions(void) {
    /* Older TCF agents don't allow extensions in line area info.
     * See also Bug 500746 */
    int i;
    int rc = 0;
    Channel * c = cache_channel();
    if (c == NULL) return 1; /* Local client */
    for (i = 0; i < c->peer_service_cnt; i++) {
        char * nm = c->peer_service_list[i];
        if (strcmp(nm, "SymbolsProxyV2") == 0) return 1;
        if (strcmp(nm, "RunControl") == 0) rc = 1;
    }
    return !rc;
}

void write_code_area(OutputStream * out, CodeArea * area, CodeArea * prev) {
    write_stream(out, '{');
    json_write_string(out, "SAddr");
    write_stream(out, ':');
    json_write_uint64(out, area->start_address);
    if (area->start_line > 0) {
        write_stream(out, ',');
        json_write_string(out, "SLine");
        write_stream(out, ':');
        json_write_ulong(out, area->start_line);
        if (area->start_column > 0) {
            write_stream(out, ',');
            json_write_string(out, "SCol");
            write_stream(out, ':');
            json_write_ulong(out, area->start_column);
        }
    }
    if (area->end_address != 0) {
        write_stream(out, ',');
        json_write_string(out, "EAddr");
        write_stream(out, ':');
        json_write_uint64(out, area->end_address);
    }
    if (area->end_line > 0) {
        write_stream(out, ',');
        json_write_string(out, "ELine");
        write_stream(out, ':');
        json_write_ulong(out, area->end_line);
        if (area->end_column > 0) {
            write_stream(out, ',');
            json_write_string(out, "ECol");
            write_stream(out, ':');
            json_write_ulong(out, area->end_column);
        }
    }
    if (area->next_address != 0) {
        write_stream(out, ',');
        json_write_string(out, "NAddr");
        write_stream(out, ':');
        json_write_uint64(out, area->next_address);
    }
    if (area->file != NULL && (prev == NULL || prev->file != area->file)) {
        write_stream(out, ',');
        json_write_string(out, "File");
        write_stream(out, ':');
        json_write_string(out, area->file);
    }
    if (area->directory != NULL && (prev == NULL || prev->directory != area->directory)) {
        write_stream(out, ',');
        json_write_string(out, "Dir");
        write_stream(out, ':');
        json_write_string(out, area->directory);
    }
    if (area->isa > 0) {
        write_stream(out, ',');
        json_write_string(out, "ISA");
        write_stream(out, ':');
        json_write_ulong(out, area->isa);
    }
    if (area->is_statement) {
        write_stream(out, ',');
        json_write_string(out, "IsStmt");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }
    if (area->basic_block) {
        write_stream(out, ',');
        json_write_string(out, "BasicBlock");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }
    if (area->prologue_end) {
        write_stream(out, ',');
        json_write_string(out, "PrologueEnd");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }
    if (area->epilogue_begin) {
        write_stream(out, ',');
        json_write_string(out, "EpilogueBegin");
        write_stream(out, ':');
        json_write_boolean(out, 1);
    }
    if (area->op_index) {
        write_stream(out, ',');
        json_write_string(out, "OpIndex");
        write_stream(out, ':');
        json_write_long(out, area->op_index);
    }
    if (area->discriminator) {
        write_stream(out, ',');
        json_write_string(out, "Discriminator");
        write_stream(out, ':');
        json_write_long(out, area->discriminator);
    }
    if (area->next_stmt_address != 0 && client_supports_line_info_extensions()) {
        write_stream(out, ',');
        json_write_string(out, "NStmtAddr");
        write_stream(out, ':');
        json_write_uint64(out, area->next_stmt_address);
    }
    write_stream(out, '}');
}

#if SERVICE_LineNumbers

#define MAX_AREA_CNT 0x1000

typedef struct MapToSourceArgs {
    char token[256];
    char id[256];
    ContextAddress addr0;
    ContextAddress addr1;
} MapToSourceArgs;

typedef struct MapToMemoryArgs {
    char token[256];
    char id[256];
    char * file;
    int line;
    int column;
} MapToMemoryArgs;

static int code_area_cnt = 0;
static int code_area_max = 0;
static CodeArea * code_area_buf = NULL;

static const char * LINENUMBERS = "LineNumbers";

static void write_line_info(OutputStream * out, int cnt) {
    CodeArea * area = code_area_buf + cnt;
    CodeArea * prev = cnt == 0 ? NULL : code_area_buf + cnt - 1;
    write_code_area(out, area, prev);
}


static void add_code_area(CodeArea * area, void * args) {
    if (code_area_cnt >= code_area_max) {
        if (code_area_max >= MAX_AREA_CNT) exception(ERR_BUFFER_OVERFLOW);
        code_area_max = code_area_max == 0 ? 16 : code_area_max * 2;
        code_area_buf = (CodeArea *)loc_realloc(code_area_buf, sizeof(CodeArea) * code_area_max);
    }
    code_area_buf[code_area_cnt++] = *area;
}

static void map_to_source_cache_client(void * x) {
    int err = 0;
    Context * ctx = NULL;
    MapToSourceArgs * args = (MapToSourceArgs *)x;
    Channel * c = cache_channel();

    ctx = id2ctx(args->id);
    if (ctx == NULL) err = ERR_INV_CONTEXT;
    else if (ctx->exited) err = ERR_ALREADY_EXITED;
    else ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);

    code_area_cnt = 0;
    if (err == 0 && address_to_line(ctx, args->addr0, args->addr1, add_code_area, NULL) < 0) err = errno;

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);
    write_errno(&c->out, err);
    if (code_area_cnt == 0) {
        write_stringz(&c->out, "null");
    }
    else {
        int cnt = 0;
        write_stream(&c->out, '[');
        while (cnt < code_area_cnt) {
            if (cnt > 0) write_stream(&c->out, ',');
            write_line_info(&c->out, cnt);
            cnt++;
        }
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_map_to_source(char * token, Channel * c) {
    MapToSourceArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    args.addr0 = (ContextAddress)json_read_uint64(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    args.addr1 = (ContextAddress)json_read_uint64(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(map_to_source_cache_client, c, &args, sizeof(args));
}

static void map_to_memory_cache_client(void * x) {
    int err = 0;
    Context * ctx = NULL;
    MapToMemoryArgs * args = (MapToMemoryArgs *)x;
    Channel * c = cache_channel();

    ctx = id2ctx(args->id);
    if (ctx == NULL) err = ERR_INV_CONTEXT;
    else if (ctx->exited) err = ERR_ALREADY_EXITED;
    else ctx = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);


    code_area_cnt = 0;
    if (err == 0 && line_to_address(ctx, args->file,
        args->line, args->column, add_code_area, NULL) < 0) err = errno;

    cache_exit();

    write_stringz(&c->out, "R");
    write_stringz(&c->out, args->token);
    write_errno(&c->out, err);
    if (code_area_cnt == 0) {
        write_stringz(&c->out, "null");
    }
    else {
        int cnt = 0;
        write_stream(&c->out, '[');
        while (cnt < code_area_cnt) {
            if (cnt > 0) write_stream(&c->out, ',');
            write_line_info(&c->out, cnt);
            cnt++;
        }
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);
    }
    write_stream(&c->out, MARKER_EOM);
    loc_free(args->file);
}

static void command_map_to_memory(char * token, Channel * c) {
    MapToMemoryArgs args;

    json_read_string(&c->inp, args.id, sizeof(args.id));
    json_test_char(&c->inp, MARKER_EOA);
    args.file = json_read_alloc_string(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    args.line = json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    args.column = json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    strlcpy(args.token, token, sizeof(args.token));
    cache_enter(map_to_memory_cache_client, c, &args, sizeof(args));
}

void ini_line_numbers_service(Protocol * proto) {
    static int ini_done = 0;
    if (!ini_done) {
        ini_line_numbers_lib();
        ini_done = 1;
    }
    add_command_handler(proto, LINENUMBERS, "mapToSource", command_map_to_source);
    add_command_handler(proto, LINENUMBERS, "mapToMemory", command_map_to_memory);
}

#endif /* SERVICE_LineNumbers */
#endif /* ENABLE_DebugContext */
