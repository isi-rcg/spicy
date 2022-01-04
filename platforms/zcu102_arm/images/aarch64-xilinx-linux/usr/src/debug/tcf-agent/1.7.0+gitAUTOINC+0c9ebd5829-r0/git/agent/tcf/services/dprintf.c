/*******************************************************************************
 * Copyright (c) 2013, 2014 Xilinx, Inc. and others.
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
 * "dynamic printf" service
 */

#include <tcf/config.h>

#if SERVICE_DPrintf && SERVICE_Expressions && SERVICE_Streams

#include <assert.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/cache.h>
#include <tcf/framework/json.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/streamsservice.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/dprintf.h>

static const char * DPRINTF = "DPrintf";

typedef struct Buffer Buffer;
typedef struct Client Client;

struct Buffer {
    LINK link;
    char * buf;
    size_t done;
    size_t size;
};

struct Client {
    LINK link;
    LINK bufs;
    Channel * channel;
    VirtualStream * vstream;
    Buffer * queue;
    char * tmp_buf;
    unsigned tmp_pos;
    unsigned tmp_max;
};

#define link2buf(x)  ((Buffer *)((char *)(x) - offsetof(Buffer, link)))
#define link2client(x)  ((Client *)((char *)(x) - offsetof(Client, link)))

static LINK clients;

static Client * find_client(Channel * channel) {
    LINK * l;
    if (channel == NULL) return NULL;
    for (l = clients.next; l != &clients; l = l->next) {
        Client * client = link2client(l);
        if (client->channel == channel) return client;
    }
    return NULL;
}

static void add_ch(Client * client, char ch) {
    if (client->tmp_pos >= client->tmp_max) {
        client->tmp_max += 256;
        client->tmp_buf = (char *)loc_realloc(client->tmp_buf, client->tmp_max);
    }
    client->tmp_buf[client->tmp_pos++] = ch;
}

static const void * load_remote_string(Context * ctx, Value * arg_val) {
    int error = 0;
    size_t sbf_pos = 0;
    size_t sbf_max = 128;
    char * sbf = (char *)tmp_alloc(sbf_max);
    ContextAddress addr = 0;

    if (ctx == NULL) error = ERR_INV_CONTEXT;
    if (!error && ctx->mem_access == 0) ctx = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
    if (!error && ctx->mem_access == 0) error = ERR_INV_CONTEXT;
    if (!error && value_to_address(arg_val, &addr) < 0) error = errno;
    while (!error) {
        char ch = 0;
        if (sbf_pos >= sbf_max) {
            sbf_max *= 2;
            sbf = (char *)tmp_realloc(sbf, sbf_max);
        }
        if (context_read_mem(ctx, addr, &ch, 1) < 0) {
            error = errno;
        }
        else {
            sbf[sbf_pos++] = ch;
            if (ch == 0) return sbf;
            addr++;
        }
    }
    return "???";
}

void dprintf_expression_ctx(Context * ctx, const char * fmt, Value * args, unsigned args_cnt) {
    unsigned fmt_pos = 0;
    unsigned arg_pos = 0;
    Client * client = find_client(cache_channel());

    if (client == NULL) return;

    while (fmt[fmt_pos]) {
        char ch = fmt[fmt_pos];
        if (ch == '%' && fmt[fmt_pos + 1] == '%') {
            fmt_pos++;
        }
        else if (ch == '%' && arg_pos < args_cnt) {
            char arg_buf[256];
            char arg_fmt[256];
            unsigned arg_fmt_pos = 0;
            unsigned arg_len = 0;
            unsigned flag_l = 0;
            unsigned flag_h = 0;
            unsigned flag_L = 0;
            unsigned flag_j = 0;
            unsigned flag_z = 0;
            unsigned flag_t = 0;
            char fmt_ch = 0;
            Value * arg_val = args + arg_pos++;
            arg_fmt[arg_fmt_pos++] = ch;
            fmt_pos++;
            while (fmt[fmt_pos]) {
                ch = fmt[fmt_pos++];
                if (arg_fmt_pos < sizeof(arg_fmt) - 1) {
                    arg_fmt[arg_fmt_pos++] = ch;
                }
                switch (ch) {
                case 'l': flag_l++; continue;
                case 'L': flag_L++; continue;
                case 'h': flag_h++; continue;
                case 'j': flag_j++; continue;
                case 'z': flag_z++; continue;
                case 't': flag_t++; continue;
                }
                if (ch == '%' || ch >= 'A') {
                    fmt_ch = ch;
                    break;
                }
                if (ch == '*') {
                    arg_fmt_pos--;
                    if (arg_pos < args_cnt) {
                        uint64_t n = 0;
                        unsigned m = 0;
                        char ns[256];
                        value_to_unsigned(arg_val, &n);
                        snprintf(ns, sizeof(ns), "%u", (unsigned)n);
                        while (arg_fmt_pos < sizeof(arg_fmt) - 1 && ns[m] != 0) {
                            arg_fmt[arg_fmt_pos++] = ns[m++];
                        }
                        arg_val = args + arg_pos++;
                    }
                }
            }
            arg_fmt[arg_fmt_pos++] = 0;
            if (fmt_ch != '%') {
                int64_t n = 0;
                uint64_t u = 0;
                double d = 0;
                arg_buf[0] = 0;
                switch (fmt_ch) {
                case 'd':
                case 'i':
                    if (arg_val->type_class == TYPE_CLASS_INTEGER || arg_val->type_class == TYPE_CLASS_ENUMERATION) {
                        if (value_to_signed(arg_val, &n) < 0) exception(errno);
                    }
                    else {
                        if (value_to_unsigned(arg_val, &u) < 0) exception(errno);
                        n = u;
                    }
                    if (flag_l > 1) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (long long)n);
                    else if (flag_l) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (long)n);
                    else if (flag_j) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (long long)n);
                    else if (flag_z) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (size_t)n);
                    else if (flag_t) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (ptrdiff_t)n);
                    else if (flag_h > 1) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (char)n);
                    else if (flag_h) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (short)n);
                    else snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (int)n);
                    break;
                case 'o':
                case 'u':
                case 'x':
                case 'X':
                    if (arg_val->type_class == TYPE_CLASS_INTEGER || arg_val->type_class == TYPE_CLASS_ENUMERATION) {
                        if (value_to_signed(arg_val, &n) < 0) exception(errno);
                    }
                    else {
                        if (value_to_unsigned(arg_val, &u) < 0) exception(errno);
                        n = u;
                    }
                    if (flag_l > 1) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (unsigned long long)n);
                    else if (flag_l) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (unsigned long)n);
                    else if (flag_j) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (unsigned long long)n);
                    else if (flag_z) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (size_t)n);
                    else if (flag_t) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (ptrdiff_t)n);
                    else if (flag_h > 1) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (unsigned char)n);
                    else if (flag_h) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (unsigned short)n);
                    else snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (unsigned int)n);
                    break;
                case 'c':
                case 'C':
                    if (value_to_signed(arg_val, &n) < 0) exception(errno);
                    snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (int)n);
                    break;
                case 'f':
                case 'F':
                case 'e':
                case 'E':
                case 'g':
                case 'G':
                case 'a':
                case 'A':
                    if (value_to_double(arg_val, &d) < 0) exception(errno);
                    if (flag_L) snprintf(arg_buf, sizeof(arg_buf), arg_fmt, (long double)d);
                    else snprintf(arg_buf, sizeof(arg_buf), arg_fmt, d);
                    break;
                case 's':
                    {
                        const void * value = arg_val->value;
                        if (arg_val->type_class == TYPE_CLASS_POINTER) {
                            value = load_remote_string(ctx, arg_val);
                        }
                        snprintf(arg_buf, sizeof(arg_buf), arg_fmt, value);
                    }
                    break;
                default:
                    snprintf(arg_buf, sizeof(arg_buf), arg_fmt, arg_val->value);
                    break;
                }
                arg_len = strlen(arg_buf);
                if (client->tmp_pos + arg_len >= client->tmp_max) {
                    client->tmp_max += arg_len + 256;
                    client->tmp_buf = (char *)loc_realloc(client->tmp_buf, client->tmp_max);
                }
                memcpy(client->tmp_buf + client->tmp_pos, arg_buf, arg_len);
                client->tmp_pos += arg_len;
                continue;
            }
        }
        add_ch(client, ch);
        fmt_pos++;
    }
}

static void streams_callback(VirtualStream * stream, int event_code, void * args) {
    Client * client = (Client *)args;
    assert(stream == client->vstream);
    if (event_code == VS_EVENT_SPACE_AVAILABLE && !list_is_empty(&client->bufs)) {
        size_t done = 0;
        Buffer * b = link2buf(client->bufs.next);
        virtual_stream_add_data(stream, b->buf + b->done, b->size - b->done, &done, 0);
        b->done += done;
        if (b->done >= b->size) {
            list_remove(&b->link);
            if (list_is_empty(&client->bufs)) run_ctrl_unlock();
            loc_free(b->buf);
            loc_free(b);
        }
    }
}

static void read_open_args(InputStream * inp, const char * name, void * x) {
    json_skip_object(inp);
}

static void command_open(char * token, Channel * c) {
    char id[256];
    Client * client = find_client(c);

    json_read_struct(&c->inp, read_open_args, NULL);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (client == NULL) {
        client = (Client *)loc_alloc_zero(sizeof(Client));
        virtual_stream_create(DPRINTF, NULL, 0x1000,
            VS_ENABLE_REMOTE_READ, streams_callback, client, &client->vstream);
        list_add_first(&client->link, &clients);
        list_init(&client->bufs);
        client->channel = c;
    }
    virtual_stream_get_id(client->vstream, id, sizeof(id));

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    json_write_string(&c->out, id);
    write_stream(&c->out, MARKER_EOA);
    write_stream(&c->out, MARKER_EOM);
}

static void free_client(Client * client) {
    virtual_stream_delete(client->vstream);
    list_remove(&client->link);
    if (!list_is_empty(&client->bufs)) {
        do {
            Buffer * bf = link2buf(client->bufs.next);
            list_remove(&bf->link);
            loc_free(bf->buf);
            loc_free(bf);
        }
        while (!list_is_empty(&client->bufs));
        run_ctrl_unlock();
    }
    loc_free(client->tmp_buf);
    loc_free(client);
}

static void command_close(char * token, Channel * c) {
    Client * client = find_client(c);

    json_test_char(&c->inp, MARKER_EOM);

    if (client != NULL) free_client(client);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void cache_transaction_listener(int evt) {
    LINK * l;
    switch (evt) {
    case CTLE_START:
    case CTLE_RETRY:
        for (l = clients.next; l != &clients; l = l->next) {
            Client * client = link2client(l);
            client->tmp_pos = 0;
        }
        break;
    case CTLE_ABORT:
        break;
    case CTLE_COMMIT:
        for (l = clients.next; l != &clients; l = l->next) {
            Client * client = link2client(l);
            if (client->tmp_pos > 0) {
                size_t done = 0;
                virtual_stream_add_data(client->vstream, client->tmp_buf, client->tmp_pos, &done, 0);
                if (done < client->tmp_pos) {
                    Buffer * b = (Buffer *)loc_alloc_zero(sizeof(Buffer));
                    b->size = client->tmp_pos - done;
                    b->buf = (char *)loc_alloc(b->size);
                    memcpy(b->buf, client->tmp_buf + done, b->size);
                    if (list_is_empty(&client->bufs)) run_ctrl_lock();
                    list_add_last(&b->link, &client->bufs);
                }
            }
        }
        break;
    }
}

static void channel_close_listener(Channel * c) {
    Client * client = find_client(c);
    if (client != NULL) free_client(client);
}

static void function_callback(int mode, Value * v, Value * args, unsigned args_cnt) {
    if (args_cnt == 0) {
        str_exception(ERR_INV_EXPRESSION, "$printf mush have at least one argument");
    }
    if (mode != EXPRESSION_MODE_SKIP && args[0].type_class != TYPE_CLASS_ARRAY) {
        str_exception(ERR_INV_EXPRESSION, "$printf first argument must be a string");
    }
    if (mode == EXPRESSION_MODE_NORMAL) {
        dprintf_expression_ctx(v->ctx, (char *)args[0].value, args + 1, args_cnt - 1);
    }
    set_value(v, NULL, 0, 0);
}

static int identifier_callback(Context * ctx, int frame, char * name, Value * v) {
    if (strcmp(name, "$printf") == 0) {
        v->func_cb = function_callback;
        v->type_class = TYPE_CLASS_FUNCTION;
        return 1;
    }
    return 0;
}

void ini_dprintf_service(Protocol * p) {
    list_init(&clients);
    add_command_handler(p, DPRINTF, "open", command_open);
    add_command_handler(p, DPRINTF, "close", command_close);
    add_cache_transaction_listener(cache_transaction_listener);
    add_channel_close_listener(channel_close_listener);
    add_identifier_callback(identifier_callback);
}

#endif /* SERVICE_DPrintf */
