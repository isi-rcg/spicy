/*******************************************************************************
 * Copyright (c) 2018-2019 Xilinx, Inc. and others.
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
 * Implementation of HTTP interface.
 */

#include <tcf/config.h>

#if ENABLE_HttpServer

#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <assert.h>
#if defined(_WIN32) || defined(__CYGWIN__)
#  include <ShlObj.h>
#endif
#include <tcf/framework/trace.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/events.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/link.h>
#include <tcf/http/http-tcf.h>
#include <tcf/http/http.h>

typedef struct HttpServer {
    LINK link_all;
    LINK link_clients;
    LINK link_cons;
    AsyncReqInfo req_acc;
    struct sockaddr * addr_buf;
    int addr_len;
    int sock;
} HttpServer;

typedef struct HttpConnection {
    LINK link_all;
    HttpServer * server;
    int sock;
    int addr_len;
    struct sockaddr * addr_buf;
    AsyncReqInfo req_rd;
    AsyncReqInfo req_wr;
    int read_posted;
    int write_posted;
    char * recv_buf;
    size_t recv_len;
    size_t recv_pos;
    size_t recv_max;
    char * http_method;
    char * http_uri;
    char * http_ver;
    HttpParam * http_args;
    HttpParam * http_hdrs;
    int read_request_done;
    int keep_alive;
    ByteArrayOutputStream out;
    char * hdrs_data;
    size_t hdrs_size;
    size_t hdrs_done;
    char * send_data;
    size_t send_size;
    size_t send_done;
    int page_code;
    char * page_reason;
    HttpParam * page_hdrs;
    int suspended;
    int close;
    int sse;
} HttpConnection;

static LINK server_list;
static HttpListener ** listener_arr = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;
static char ** directory_arr = NULL;
static unsigned directory_cnt = 0;
static unsigned directory_max = 0;
static HttpConnection * current_con = NULL;

#define all2server(x) ((HttpServer *)((char *)(x) - offsetof(HttpServer, link_all)))
#define all2con(x) ((HttpConnection *)((char *)(x) - offsetof(HttpConnection, link_all)))

static void clear_connection_state(HttpConnection * con) {
    loc_free(con->hdrs_data);
    loc_free(con->send_data);
    loc_free(con->http_method);
    loc_free(con->http_uri);
    loc_free(con->http_ver);
    while (con->http_args != NULL) {
        HttpParam * h = con->http_args;
        con->http_args = h->next;
        loc_free(h->name);
        loc_free(h->value);
        loc_free(h);
    }
    while (con->http_hdrs != NULL) {
        HttpParam * h = con->http_hdrs;
        con->http_hdrs = h->next;
        loc_free(h->name);
        loc_free(h->value);
        loc_free(h);
    }
    while (con->page_hdrs != NULL) {
        HttpParam * h = con->page_hdrs;
        con->page_hdrs = h->next;
        loc_free(h->name);
        loc_free(h->value);
        loc_free(h);
    }
    loc_free(con->page_reason);
    memset(&con->req_wr, 0, sizeof(con->req_wr));
    con->recv_len = 0;
    con->recv_pos = 0;
    con->http_method = NULL;
    con->http_uri = NULL;
    con->http_ver = NULL;
    con->read_request_done = 0;
    con->keep_alive = 0;
    con->hdrs_data = NULL;
    con->hdrs_size = 0;
    con->hdrs_done = 0;
    con->send_data = NULL;
    con->send_size = 0;
    con->send_done = 0;
    con->page_code = 0;
    con->page_reason = NULL;
}

static void close_connection(HttpConnection * con) {
    assert(!con->read_posted);
    assert(!con->write_posted);
    http_connection_closed(&con->out.out);
    clear_connection_state(con);
    closesocket(con->sock);
    list_remove(&con->link_all);
    loc_free(con->recv_buf);
    loc_free(con->addr_buf);
    loc_free(con);
}

OutputStream * get_http_stream(void) {
    return &current_con->out.out;
}

HttpParam * get_http_params(void) {
    return current_con->http_args;
}

HttpParam * get_http_headers(void) {
    return current_con->http_hdrs;
}

void http_content_type(const char * type) {
    http_response_header("Content-Type", type);
}

void http_response_header(const char * name, const char * value) {
    HttpParam ** p = &current_con->page_hdrs;
    HttpParam * h = current_con->page_hdrs;
    while (h != NULL) {
        if (strcmp(h->name, name) == 0) {
            loc_free(h->value);
            h->value = loc_strdup(value);
            return;
        }
        p = &h->next;
        h = h->next;
    }
    *p = h = (HttpParam *)loc_alloc_zero(sizeof(HttpParam));
    h->name = loc_strdup(name);
    h->value = loc_strdup(value);
}

void http_response_status(int code, const char * reason) {
    current_con->page_code = code;
    loc_free(current_con->page_reason);
    current_con->page_reason = loc_strdup(reason);
}

void http_send(char ch) {
    write_stream(&current_con->out.out, (unsigned char)ch);
}

void http_send_block(const char * buf, size_t size) {
    write_block_stream(&current_con->out.out, buf, size);
}

void http_printf(const char * fmt, ...) {
    va_list ap;
    char arr[0x100];
    void * mem = NULL;
    char * buf = arr;
    size_t len = sizeof(arr);
    int n = 0;

    while (1) {
        va_start(ap, fmt);
        n = vsnprintf(buf, len, fmt, ap);
        va_end(ap);
        if (n < 0) {
            if (len > 0x100000) break;
            len *= 2;
        }
        else {
            if (n < (int)len) break;
            len = n + 1;
        }
        mem = loc_realloc(mem, len);
        buf = (char *)mem;
    }
    write_block_stream(&current_con->out.out, buf, n);
    if (mem != NULL) loc_free(mem);
}

static void http_send_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    HttpConnection * con = (HttpConnection *)req->client_data;
    ssize_t len = con->req_wr.u.sio.rval;

    assert(con->write_posted);
    assert(is_dispatch_thread());
    con->write_posted = 0;

    if (len >= 0) {
        if (con->hdrs_done < con->hdrs_size) {
            assert(con->req_wr.u.sio.bufp == con->hdrs_data + con->hdrs_done);
            assert(con->req_wr.u.sio.bufsz == con->hdrs_size - con->hdrs_done);
            con->hdrs_done += len;
            if (con->hdrs_done < con->hdrs_size) {
                con->req_wr.u.sio.bufp = con->hdrs_data + con->hdrs_done;
                con->req_wr.u.sio.bufsz = con->hdrs_size - con->hdrs_done;
                con->write_posted = 1;
                async_req_post(&con->req_wr);
                return;
            }
        }
        else {
            assert(con->req_wr.u.sio.bufp == con->send_data + con->send_done);
            assert(con->req_wr.u.sio.bufsz == con->send_size - con->send_done);
            con->send_done += len;
        }
        if (con->send_done < con->send_size) {
            con->req_wr.u.sio.bufp = con->send_data + con->send_done;
            con->req_wr.u.sio.bufsz = con->send_size - con->send_done;
            con->write_posted = 1;
            async_req_post(&con->req_wr);
            return;
        }
        if (!con->close) {
            if (con->sse) {
                if (con->out.pos > 0) {
                    con->send_done = 0;
                    loc_free(con->send_data);
                    get_byte_array_output_stream_data(&con->out, &con->send_data, &con->send_size);
                    con->req_wr.u.sio.bufp = con->send_data + con->send_done;
                    con->req_wr.u.sio.bufsz = con->send_size - con->send_done;
                    con->write_posted = 1;
                    async_req_post(&con->req_wr);
                }
                return;
            }
            if (con->keep_alive) {
                clear_connection_state(con);
                con->req_rd.u.sio.bufp = con->recv_buf;
                con->req_rd.u.sio.bufsz = con->recv_max;
                con->read_posted = 1;
                async_req_post(&con->req_rd);
                return;
            }
        }
    }
    if (con->read_posted) {
        shutdown(con->sock, SHUT_RDWR);
        con->close = 1;
        return;
    }
    close_connection(con);
}

static void send_reply(HttpConnection * con) {
    unsigned i;
    current_con = con;
    create_byte_array_output_stream(&con->out);
    for (i = listener_cnt; i > 0; i--) {
        if (listener_arr[i - 1]->get_page(con->http_uri)) break;
    }
    if (con->suspended) {
        current_con = NULL;
    }
    else if (current_con != NULL) {
        http_flush();
    }
}

static void read_http_request(HttpConnection * con) {
    char * buf = con->recv_buf;
    size_t len = con->recv_len;
    while (len > 0 && !con->read_request_done) {
        unsigned i = con->recv_pos;
        while (i < len && buf[i] != '\n') i++;
        if (i++ >= len) {
            /* Incomplete line in the buffer */
            con->recv_pos = len;
            if (con->recv_len == len) return;
            break;
        }
        con->recv_pos = 0;
        if (con->http_method == NULL) {
            unsigned j = 0;
            unsigned k = 0;
            while (j < i) {
                unsigned l = j;
                char * s = buf + j;
                while (j < i && buf[j] > ' ') j++;
                switch (k++) {
                case 0: con->http_method = loc_strndup(s, j - l); break;
                case 1: con->http_uri = loc_strndup(s, j - l); break;
                case 2: con->http_ver = loc_strndup(s, j - l); break;
                }
                while (j < i && buf[j] <= ' ') j++;
            }
        }
        else {
            unsigned k = i;
            while (k > 0 && buf[k - 1] <= ' ') k--;
            if (k == 0) {
                con->read_request_done = 1;
            }
            else {
                unsigned j = 0;
                while (j < k && buf[j] != ':') j++;
                if (j < k) {
                    HttpParam * h = (HttpParam *)loc_alloc_zero(sizeof(HttpParam));
                    h->name = loc_strndup(buf, j++);
                    while (j < k && buf[j] == ' ') j++;
                    h->value = loc_strndup(buf + j, k - j);
                    h->next = con->http_hdrs;
                    if (strcmp(h->name, "Connection") == 0 && strcmp(h->value, "keep-alive") == 0) {
                        con->keep_alive = 1;
                    }
                    con->http_hdrs = h;
                }
            }
        }
        buf += i;
        len -= i;
    }
    con->recv_len = len;
    memmove(con->recv_buf, buf, len);
}

static void http_read_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    HttpConnection * con = (HttpConnection *)req->client_data;
    ssize_t len = 0;

    assert(con->read_posted);
    assert(is_dispatch_thread());
    assert(con->req_rd.u.sio.bufp == con->recv_buf + con->recv_len);
    assert(con->req_rd.u.sio.bufsz == con->recv_max - con->recv_len);
    len = con->req_rd.u.sio.rval;
    con->read_posted = 0;

    if (len < 0) {
        if (con->write_posted) {
            shutdown(con->sock, SHUT_RDWR);
            con->close = 1;
            return;
        }
        close_connection(con);
    }
    else if (len > 0) {
        con->recv_len += len;
        assert(con->recv_len <= con->recv_max);
        read_http_request(con);
        if (con->read_request_done) {
            send_reply(con);
        }
        else {
            if (con->recv_len >= con->recv_max) {
                con->recv_max *= 2;
                con->recv_buf = (char *)loc_realloc(con->recv_buf, con->recv_max);
            }
            req->u.sio.bufp = con->recv_buf + con->recv_len;
            req->u.sio.bufsz = con->recv_max - con->recv_len;
            con->read_posted = 1;
            async_req_post(&con->req_rd);
        }
    }
}

void http_suspend(void) {
    current_con->suspended = 1;
}

void http_resume(OutputStream * out) {
    if (current_con == NULL) {
        LINK * l, *n;
        for (l = server_list.next; l != &server_list; l = l->next) {
            HttpServer * s = all2server(l);
            for (n = s->link_cons.next; n != &s->link_cons; n = n->next) {
                HttpConnection * c = all2con(n);
                if (out == &c->out.out) {
                    assert(c->suspended);
                    c->suspended = 0;
                    current_con = c;
                    break;
                }
            }
        }
    }
    assert(current_con == NULL || &current_con->out.out == out);
}

void http_flush(void) {
    HttpConnection * con = current_con;
    const char * reason = con->page_reason;
    int code = con->page_code;
    int cache_control = 0;
    int content_type = 0;

    if (con->sse) {
        con->suspended = 1;
        if (!con->write_posted) {
            con->send_done = 0;
            loc_free(con->send_data);
            get_byte_array_output_stream_data(&con->out, &con->send_data, &con->send_size);
            con->req_wr.u.sio.bufp = con->send_data + con->send_done;
            con->req_wr.u.sio.bufsz = con->send_size - con->send_done;
            con->write_posted = 1;
            async_req_post(&con->req_wr);
        }
        current_con = NULL;
        return;
    }

    if (con->out.pos == 0 && con->page_hdrs == NULL) {
        code = 404;
        reason = "NOT FOUND";
        http_printf("Not found: %s\n", con->http_uri);
    }
    get_byte_array_output_stream_data(&con->out, &con->send_data, &con->send_size);

    create_byte_array_output_stream(&con->out);
    if (code == 0) code = 200;
    if (reason == NULL) reason = "OK";
    http_printf("HTTP/1.1 %d %s\n", code, reason);
    if (con->page_hdrs) {
        HttpParam * h = con->page_hdrs;
        while (h != NULL) {
            if (strcmp(h->name, "Content-Type") == 0) {
                con->sse = strcmp(h->value, "text/event-stream") == 0;
                content_type = 1;
            }
            if (strcmp(h->name, "Cache-Control") == 0) cache_control = 1;
            http_printf("%s: %s\n", h->name, h->value);
            h = h->next;
        }
    }
    if (!content_type) http_printf("Content-Type: text/html\n");
    if (!cache_control) http_printf("Cache-Control: no-cache\n");
    if (con->keep_alive && !con->close) http_printf("Connection: keep-alive\n");
    if (con->sse) {
        con->suspended = 1;
    }
    else {
        http_printf("Content-Length: %u\n", (unsigned)con->send_size);
    }
    http_send('\n');
    get_byte_array_output_stream_data(&con->out, &con->hdrs_data, &con->hdrs_size);

    current_con = NULL;

    con->req_wr.done = http_send_done;
    con->req_wr.client_data = con;
    con->req_wr.type = AsyncReqSend;
    con->req_wr.u.sio.sock = con->sock;
    con->req_wr.u.sio.bufp = con->hdrs_data;
    con->req_wr.u.sio.bufsz = con->hdrs_size;
    con->req_wr.u.sio.flags = 0;
    con->write_posted = 1;
    async_req_post(&con->req_wr);
}

void http_close(void) {
    current_con->close = 1;
}

static int get_page(const char * uri) {
    unsigned i = 0;
    char * nm = tmp_strdup(uri);
    while (nm[i] != 0 && nm[i] != '?') i++;
    if (i <= 1) return 0;
    nm[i] = 0;
    for (i = directory_cnt; i > 0; i--) {
        struct stat statbuf;
        char * fnm = tmp_strdup2(directory_arr[i - 1], nm);
        if (stat(fnm, &statbuf) == 0 && statbuf.st_size > 0) {
            int f = open(fnm, O_BINARY | O_RDONLY, 0);
            if (f < 0) {
                trace(LOG_ALWAYS, "HTTP: Cannot open file '%s': %s", fnm, errno_to_str(errno));
            }
            else {
                char * buf = NULL;
                size_t bsz = statbuf.st_size;
                if (bsz > 0x10000) bsz = 0x10000;
                buf = (char *)tmp_alloc(bsz);
                for (;;) {
                    int rd = read(f, buf, bsz);
                    if (rd < 0) {
                        trace(LOG_ALWAYS, "HTTP: Cannot read file '%s': %s", fnm, errno_to_str(errno));
                        break;
                    }
                    if (rd == 0) break;
                    http_send_block(buf, rd);
                }
                if (current_con->out.pos > 0) {
                    size_t l = strlen(fnm) - 1;
                    while (l > 0 && fnm[l] != '/' && fnm[l] != '.') l--;
                    if (strcmp(fnm + l, ".css") == 0) http_content_type("text/css");
                    else if (strcmp(fnm + l, ".js") == 0) http_content_type("text/javascript");
                    else if (strcmp(fnm + l, ".gif") == 0) http_content_type("image/gif");
                    else if (strcmp(fnm + l, ".jpg") == 0) http_content_type("image/jpeg");
                    else if (strcmp(fnm + l, ".png") == 0) http_content_type("image/png");
                    http_response_header("Cache-Control", "private, max-age=300");
                }
                close(f);
                return 1;
            }
        }
    }
    return 0;
}

static void http_server_accept_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    HttpServer * server = (HttpServer *)req->client_data;
    const int i = 1;

    if (server->sock < 0) {
        /* Server closed. */
        assert(list_is_empty(&server->link_all));
        assert(list_is_empty(&server->link_clients));
        assert(list_is_empty(&server->link_cons));
        loc_free(server->addr_buf);
        loc_free(server);
        return;
    }
    if (req->error) {
        trace(LOG_ALWAYS, "HTTP Socket accept failed: %s", errno_to_str(req->error));
    }
    else if (setsockopt(req->u.acc.rval, IPPROTO_TCP, TCP_NODELAY, (char *)&i, sizeof(i)) < 0) {
        trace(LOG_ALWAYS, "Can't set TCP_NODELAY option on a socket: %s", errno_to_str(errno));
        closesocket(req->u.acc.rval);
    }
    else {
        HttpConnection * con = (HttpConnection *)loc_alloc_zero(sizeof(HttpConnection));
        list_add_first(&con->link_all, &server->link_cons);
        con->server = server;
        con->sock = req->u.acc.rval;
        con->addr_buf = (struct sockaddr *)loc_alloc(server->addr_len);
        memcpy(con->addr_buf, server->addr_buf, server->addr_len);
        con->addr_len = server->addr_len;
        con->recv_max = 0x300;
        con->recv_buf = (char *)loc_alloc(con->recv_max);
        con->req_rd.done = http_read_done;
        con->req_rd.client_data = con;
        con->req_rd.type = AsyncReqRecv;
        con->req_rd.u.sio.sock = con->sock;
        con->req_rd.u.sio.bufp = con->recv_buf;
        con->req_rd.u.sio.bufsz = con->recv_max;
        con->req_rd.u.sio.flags = 0;
        con->read_posted = 1;
        async_req_post(&con->req_rd);
    }
    server->req_acc.u.acc.addrlen = server->addr_len;
    async_req_post(&server->req_acc);
}

static HttpServer * start_http_server(const char * host, const char * port) {
    struct addrinfo hints;
    struct addrinfo * reslist = NULL;
    struct addrinfo * res;
    const char * reason = NULL;
    HttpServer * server = NULL;
    int error = 0;
    int sock = -1;

    assert(is_dispatch_thread());
    if (port == NULL) port = "80";

    memset(&hints, 0, sizeof hints);
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    error = loc_getaddrinfo(host, port, &hints, &reslist);
    if (error) {
        trace(LOG_ALWAYS, "getaddrinfo error: %s", loc_gai_strerror(error));
        set_gai_errno(error);
        return NULL;
    }

    for (res = reslist; res != NULL; res = res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) {
            error = errno;
            reason = "create";
            continue;
        }
#if !(defined(_WIN32) || defined(__CYGWIN__))
        {
            const int i = 1;
            if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&i, sizeof(i)) < 0) {
                error = errno;
                reason = "setsockopt";
                closesocket(sock);
                sock = -1;
                continue;
            }
        }
#endif
        if (bind(sock, res->ai_addr, res->ai_addrlen)) {
            error = errno;
            reason = "bind";
            closesocket(sock);
            sock = -1;
            continue;
        }
        if (listen(sock, 16)) {
            error = errno;
            reason = "listen on";
            closesocket(sock);
            sock = -1;
            continue;
        }

        /* Only create one server at a time */
        break;
    }
    loc_freeaddrinfo(reslist);
    if (sock < 0) {
        trace(LOG_ALWAYS, "Socket %s error: %s", reason, errno_to_str(error));
        set_fmt_errno(error, "Socket %s error", reason);
        return NULL;
    }

    server = (HttpServer *)loc_alloc_zero(sizeof(HttpServer));
    list_add_first(&server->link_all, &server_list);
    list_init(&server->link_clients);
    list_init(&server->link_cons);
    server->sock = sock;
#if defined(_WRS_KERNEL)
    /* vxWorks requires buffer size to be exactly sizeof(struct sockaddr) */
    server->addr_len = sizeof(struct sockaddr);
#elif defined(SOCK_MAXADDRLEN)
    server->addr_len = SOCK_MAXADDRLEN;
#else
    server->addr_len = 0x1000;
#endif
    server->addr_buf = (struct sockaddr *)loc_alloc_zero(server->addr_len);
    server->req_acc.done = http_server_accept_done;
    server->req_acc.client_data = server;
    server->req_acc.type = AsyncReqAccept;
    server->req_acc.u.acc.sock = sock;
    server->req_acc.u.acc.addr = server->addr_buf;
    server->req_acc.u.acc.addrlen = server->addr_len;
    async_req_post(&server->req_acc);

    return server;
}

void add_http_listener(HttpListener * l) {
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listener_arr = (HttpListener **)loc_realloc(listener_arr, listener_max * sizeof(HttpListener *));
    }
    listener_arr[listener_cnt++] = l;
}

void add_http_directory(const char * dir) {
    if (directory_cnt >= directory_max) {
        directory_max += 8;
        directory_arr = (char **)loc_realloc(directory_arr, directory_max * sizeof(char *));
    }
    directory_arr[directory_cnt++] = loc_strdup(dir);
}

static ChannelServer * http_channel_server_create(PeerServer * ps) {
    const char * host = peer_server_getprop(ps, "Host", NULL);
    const char * port = peer_server_getprop(ps, "Port", NULL);
    HttpServer * server = start_http_server(host, port);
    if (server == NULL) return NULL;
    return ini_http_tcf(server->sock, ps);
}

static void http_channel_connect(PeerServer * ps,  ChannelConnectCallBack cb, void * args) {
    cb(args, ERR_UNSUPPORTED, NULL);
}

void ini_http(void) {
    static HttpListener l = { get_page };
    list_init(&server_list);
    add_channel_transport("HTTP", http_channel_server_create, http_channel_connect);
#if defined(_WIN32) || defined(__CYGWIN__)
    {
        WCHAR fnm[MAX_PATH];
        char buf[MAX_PATH];
        if (SHGetFolderPathW(0, CSIDL_WINDOWS, NULL, 0, fnm) != S_OK) {
            check_error(set_errno(ERR_OTHER, "Cannot get WINDOWS folder path"));
        }
        if (!WideCharToMultiByte(CP_UTF8, 0, fnm, -1, buf, sizeof(buf), NULL, NULL)) {
            check_error(set_win32_errno(GetLastError()));
        }
        add_http_directory(tmp_strdup2(buf, "/TCF/http"));
    }
#else
    add_http_directory("/etc/tcf/http");
#endif
    add_http_listener(&l);
}

#endif
