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
 *     Michael Sills-Lavoie(École Polytechnique de Montréal)  - ZeroCopy support
 *              *                         *            - tcp_splice_block_stream
 *******************************************************************************/

/*
 * Implements input and output stream over TCP/IP transport.
 */

#if defined(__GNUC__) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE
#endif

#include <tcf/config.h>
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#if ENABLE_Unix_Domain
#include <sys/un.h>
#endif
#if ENABLE_SSL
#  include <openssl/ssl.h>
#  include <openssl/rand.h>
#  include <openssl/err.h>
#  if defined(_WIN32) || defined(__CYGWIN__)
#    include <ShlObj.h>
#  endif
#else
   typedef void SSL;
#endif
#include <tcf/framework/mdep-fs.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/tcf.h>
#include <tcf/framework/channel_tcp.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/events.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/json.h>
#include <tcf/framework/peer.h>
#include <tcf/framework/ip_ifc.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/inputbuf.h>
#include <tcf/framework/outputbuf.h>
#include <tcf/services/discovery.h>

#ifndef MSG_MORE
#define MSG_MORE 0
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#  define FD_SETX(a,b) FD_SET((unsigned)a, b)
#  define MKDIR_MODE_TCF 0
#  define MKDIR_MODE_SSL 0
#else
#  define FD_SETX(a,b) FD_SET(a, b)
#  define MKDIR_MODE_TCF (S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)
#  define MKDIR_MODE_SSL (S_IRWXU)
#endif

#define BUF_SIZE OUTPUT_QUEUE_BUF_SIZE
#define CHANNEL_MAGIC 0x27208956
#define MAX_IFC 10

#if !defined(ENABLE_OutputQueue)
#  if ENABLE_SSL || ENABLE_ContextProxy || defined(_WIN32) || defined(__CYGWIN__) || defined(__linux__)
#    define ENABLE_OutputQueue 1
#  else
#    define ENABLE_OutputQueue 0
#  endif
#endif

#ifndef SOCKET_SEND_BUFFER_MINSIZE
#  define SOCKET_SEND_BUFFER_MINSIZE  200 * 1024
#endif
#ifndef SOCKET_RECV_BUFFER_MINSIZE
#  define SOCKET_RECV_BUFFER_MINSIZE  120 * 1024
#endif

typedef struct ChannelTCP ChannelTCP;

struct ChannelTCP {
    Channel * chan;         /* Public channel information */
    int magic;              /* Magic number */
    int socket;             /* Socket file descriptor */
    struct sockaddr * addr_buf; /* Socket remote address */
    int addr_len;
    SSL * ssl;
    int unix_domain;        /* if set, this is a UNIX domain socket, not Internet socket */
    int lock_cnt;           /* Stream lock count, when > 0 channel cannot be deleted */
    int read_pending;       /* Read request is pending */
    unsigned char * read_buf;
    size_t read_buf_size;
    int read_done;

#if ENABLE_Splice
    int pipefd[2];          /* Pipe used to splice data between a fd and the channel */
#endif /* ENABLE_Splice */

    /* Input stream buffer */
    InputBuf ibuf;

    /* Output stream state */
    unsigned char * out_bin_block;
    OutputBuffer * obuf;
    int out_errno;
    int out_flush_cnt;      /* Number of posted lazy flush events */
    int out_eom_cnt;        /* Number of end-of-message markers in the output buffer */
#if ENABLE_OutputQueue
    OutputQueue out_queue;
    AsyncReqInfo wr_req;
#endif /* ENABLE_OutputQueue */

    /* Async read request */
    AsyncReqInfo rd_req;
};

typedef struct ServerTCP ServerTCP;

struct ServerTCP {
    ChannelServer serv;
    int sock;
    struct sockaddr * addr_buf;
    int addr_len;
    LINK servlink;
    AsyncReqInfo accreq;
};

static size_t channel_tcp_extension_offset = 0;

#define EXT(ctx)        ((ChannelTCP **)((char *)(ctx) + channel_tcp_extension_offset))

#define channel2tcp(A)  (*EXT(A))
#define inp2channel(A)  ((Channel *)((char *)(A) - offsetof(Channel, inp)))
#define out2channel(A)  ((Channel *)((char *)(A) - offsetof(Channel, out)))
#define server2tcp(A)   ((ServerTCP *)((char *)(A) - offsetof(ServerTCP, serv)))
#define servlink2tcp(A) ((ServerTCP *)((char *)(A) - offsetof(ServerTCP, servlink)))
#define ibuf2tcp(A)     ((ChannelTCP *)((char *)(A) - offsetof(ChannelTCP, ibuf)))
#define obuf2tcp(A)     ((ChannelTCP *)((char *)(A) - offsetof(ChannelTCP, out_queue)))

static LINK server_list;
static void tcp_channel_read_done(void * x);
static void handle_channel_msg(void * x);

#if ENABLE_SSL
#ifndef OPENSSL_VERSION_NUMBER
#  define OPENSSL_VERSION_NUMBER 0x00000000
#endif

#if OPENSSL_VERSION_NUMBER < 0x10100000
void DH_get0_pqg(const DH * dh, const BIGNUM ** p, const BIGNUM ** q, const BIGNUM ** g) {
    if (p != NULL) *p = dh->p;
    if (q != NULL) *q = dh->q;
    if (g != NULL) *g = dh->g;
}
int DH_set0_pqg(DH * dh, BIGNUM * p, BIGNUM * q, BIGNUM * g) {
    /* q is optional */
    if (p == NULL || g == NULL) return 0;
    if (q != NULL) dh->length = BN_num_bits(q);
    BN_free(dh->p);
    BN_free(dh->q);
    BN_free(dh->g);
    dh->p = p;
    dh->q = q;
    dh->g = g;
    return 1;
}
#endif

static const char * issuer_name = "TCF";
static const char * tcf_dir = "/etc/tcf";
static SSL_CTX * ssl_ctx = NULL;
static RSA * rsa_key = NULL;

static void ini_ssl(void) {
    static int inited = 0;
    if (inited) return;
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    SSL_library_init();
    while (!RAND_status()) {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        RAND_add(&ts.tv_nsec, sizeof(ts.tv_nsec), 0.1);
    }
    inited = 1;
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
        tcf_dir = loc_strdup2(buf, "/TCF");
    }
#endif
}

static int set_ssl_errno(void) {
    const char * msg = ERR_error_string(ERR_get_error(), NULL);
    return set_errno(ERR_OTHER, msg);
}

static int certificate_verify_callback(int preverify_ok, X509_STORE_CTX * ctx) {
    char fnm[FILE_PATH_SIZE];
    DIR * dir = NULL;
    int err = 0;
    int found = 0;

    snprintf(fnm, sizeof(fnm), "%s/ssl", tcf_dir);
    if (!err && (dir = opendir(fnm)) == NULL) err = errno;
    while (!err && !found) {
        int l = 0;
        X509 * cert = NULL;
        FILE * fp = NULL;
        struct dirent * ent = readdir(dir);
        if (ent == NULL) break;
        l = strlen(ent->d_name);
        if (l < 5 || strcmp(ent->d_name + l -5 , ".cert") != 0) continue;
        snprintf(fnm, sizeof(fnm), "%s/ssl/%s", tcf_dir, ent->d_name);
        if (!err && (fp = fopen(fnm, "r")) == NULL) err = errno;
        if (!err && (cert = PEM_read_X509(fp, NULL, NULL, NULL)) == NULL) err = set_ssl_errno();
        if (!err && fclose(fp) != 0) err = errno;
        if (!err && X509_cmp(X509_STORE_CTX_get_current_cert(ctx), cert) == 0) found = 1;
        if (cert) X509_free(cert);
    }
    if (dir != NULL && closedir(dir) < 0 && !err) err = errno;
    if (err) trace(LOG_ALWAYS, "Cannot read certificate %s: %s", fnm, errno_to_str(err));
    else if (!found) trace(LOG_ALWAYS, "Authentication failure: invalid certificate");
    return err == 0 && found;
}

static DH * get_dh_key(void) {
#if OPENSSL_VERSION_NUMBER >= 0x10101000
    /* In OpenSSL 1.1.1, required key-length for the Diffie-Hellman parameters was increased to 2048 bits */
    /* Note: Java prior to 1.7 does not support Diffie-Hellman parameters longer than 1024 bits */
    /* Created by: openssl dhparam -5 -C 2048 */
    static const unsigned char dhp_buf[] = {
        0xFA, 0xCB, 0x30, 0x26, 0x0F, 0x29, 0xE0, 0x54, 0xEF, 0x45,
        0xD7, 0x5D, 0x31, 0xA3, 0xFA, 0xD8, 0xD1, 0x42, 0xD5, 0x42,
        0x15, 0x87, 0x33, 0x95, 0x41, 0x11, 0x85, 0xC1, 0x06, 0x47,
        0x28, 0xBF, 0x42, 0xCD, 0xF7, 0x83, 0x50, 0x03, 0xF8, 0xBC,
        0xA6, 0xB6, 0xC1, 0xDB, 0x0A, 0x29, 0xB9, 0xD1, 0x59, 0x48,
        0x2C, 0x3A, 0x0B, 0x3B, 0x55, 0x7F, 0x44, 0x8D, 0x61, 0xAB,
        0xF0, 0x7A, 0x6B, 0x78, 0x5D, 0x52, 0x7C, 0xC4, 0xCB, 0x77,
        0x89, 0xFF, 0x0D, 0x41, 0xA2, 0x7E, 0x19, 0xCC, 0x2E, 0xA9,
        0xCD, 0x1E, 0x9E, 0x04, 0xC7, 0xAD, 0x70, 0xF9, 0xBA, 0x5E,
        0xE1, 0xA1, 0x25, 0xA7, 0xFB, 0xB4, 0xF5, 0xE9, 0x4E, 0xBB,
        0x44, 0x6E, 0x6B, 0xE4, 0xD4, 0xE6, 0x08, 0x78, 0x96, 0x3C,
        0x95, 0x61, 0xF0, 0x10, 0x77, 0x33, 0x8A, 0x6D, 0x61, 0xB6,
        0x2A, 0xB9, 0x2B, 0x64, 0xFF, 0x91, 0xA6, 0x89, 0x9F, 0xD5,
        0xDA, 0x70, 0xC4, 0x15, 0x96, 0xE4, 0xF4, 0x46, 0x3B, 0x1E,
        0x91, 0x0A, 0xA1, 0x42, 0xE7, 0x90, 0x97, 0xA8, 0x71, 0xD9,
        0x52, 0x12, 0xC5, 0x21, 0x6D, 0x48, 0x72, 0x46, 0x5D, 0xC2,
        0xAA, 0x6C, 0x42, 0xA6, 0x19, 0xD1, 0x3E, 0xCC, 0x04, 0x3D,
        0xDB, 0xA7, 0x09, 0x45, 0xCC, 0x9A, 0xB8, 0x32, 0xB7, 0x46,
        0xB9, 0x39, 0x53, 0x53, 0x79, 0xB7, 0xFB, 0xAF, 0x1E, 0xE3,
        0xD8, 0x44, 0x57, 0xCF, 0x50, 0x97, 0x22, 0x9D, 0xBB, 0xD3,
        0x08, 0xB9, 0x9F, 0x30, 0x4D, 0x2D, 0x50, 0x8F, 0xFA, 0x63,
        0x48, 0x99, 0x98, 0x46, 0x67, 0xAB, 0x06, 0xEC, 0x0E, 0xDF,
        0x4C, 0xEE, 0x3A, 0x8E, 0x95, 0xF1, 0x18, 0xFB, 0x10, 0x30,
        0x28, 0x95, 0xF3, 0xDF, 0x59, 0x11, 0x4B, 0xC2, 0x7D, 0x7C,
        0x78, 0x4D, 0xF4, 0x71, 0x93, 0xDF, 0xE7, 0xD3, 0x2B, 0xCC,
        0x21, 0x4A, 0x50, 0xA4, 0x6B, 0x7F
    };
    static const unsigned char dhg_buf[] = {
        0x05
    };
#else
    /* Created by: openssl dhparam -5 -C 1024 */
    /* Note: Bug in  OpenSSL 1.0.2: If generator is not 2 or 5, dh->g=generator is not a usable generator */
    static unsigned char dhp_buf[] = {
        0xD0, 0x8E, 0x12, 0x70, 0x45, 0x22, 0x7D, 0x83, 0x06, 0xB5,
        0x49, 0xD8, 0x86, 0x34, 0x3A, 0x8E, 0x1D, 0xE9, 0x6C, 0x84,
        0x3A, 0x83, 0x2E, 0x0E, 0xD0, 0x7B, 0x5F, 0x69, 0x65, 0xFD,
        0xD4, 0x0A, 0x97, 0x6A, 0x1A, 0xF6, 0xB2, 0xCD, 0xB4, 0x33,
        0x81, 0x9C, 0xC0, 0x45, 0x52, 0x73, 0xA5, 0xEC, 0x32, 0x9D,
        0xAE, 0x90, 0x73, 0x24, 0x53, 0x28, 0x2E, 0x35, 0x22, 0xBC,
        0xD7, 0x43, 0xCA, 0x3E, 0xD4, 0x32, 0x75, 0xB6, 0xBD, 0xD4,
        0x8E, 0x58, 0x7B, 0x1F, 0x61, 0xCF, 0x62, 0x34, 0x95, 0xA0,
        0x36, 0x78, 0x98, 0xEB, 0xD0, 0x2A, 0xDC, 0x31, 0x56, 0x02,
        0x3E, 0xAB, 0x5D, 0x36, 0x65, 0x57, 0x24, 0x79, 0x27, 0x6F,
        0xCE, 0x65, 0x29, 0xC3, 0x97, 0xFC, 0x39, 0x31, 0x3B, 0x9E,
        0x7F, 0xA8, 0xEA, 0x68, 0x2E, 0x19, 0x06, 0x26, 0x3F, 0x9F,
        0x29, 0x07, 0x30, 0xD7, 0xFA, 0xB7, 0xD6, 0xCF
    };
    static unsigned char dhg_buf[] = {
        0x05
    };
#endif
    BIGNUM * dhp_bn = NULL;
    BIGNUM * dhg_bn = NULL;
    DH * dh = DH_new();
    if (dh == NULL) return NULL;
    dhp_bn = BN_bin2bn(dhp_buf, sizeof(dhp_buf), NULL);
    dhg_bn = BN_bin2bn(dhg_buf, sizeof(dhg_buf), NULL);
    if (dhp_bn == NULL || dhg_bn == NULL || !DH_set0_pqg(dh, dhp_bn, NULL, dhg_bn)) {
        BN_free(dhp_bn);
        BN_free(dhg_bn);
        DH_free(dh);
        return NULL;
    }
    return dh;
}
#endif /* ENABLE_SSL */

static void delete_channel(ChannelTCP * c) {
    trace(LOG_PROTOCOL, "Deleting channel %#" PRIxPTR, (uintptr_t)c);
    assert(c->lock_cnt == 0);
    assert(c->out_flush_cnt == 0);
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->read_pending == 0);
    assert(c->ibuf.handling_msg != HandleMsgTriggered);
    channel_clear_broadcast_group(c->chan);
    if (c->socket >= 0) {
        closesocket(c->socket);
        c->socket = -1;
    }
    list_remove(&c->chan->chanlink);
    if (list_is_empty(&channel_root) && list_is_empty(&channel_server_root))
        shutdown_set_stopped(&channel_shutdown);
    c->magic = 0;
#if ENABLE_OutputQueue
    output_queue_clear(&c->out_queue);
#endif /* ENABLE_OutputQueue */
#if ENABLE_SSL
    if (c->ssl) SSL_free(c->ssl);
#endif /* ENABLE_SSL */
#if ENABLE_Splice
    close(c->pipefd[0]);
    close(c->pipefd[1]);
#endif /* ENABLE_Splice */
    output_queue_free_obuf(c->obuf);
    loc_free(c->ibuf.buf);
    loc_free(c->chan->peer_name);
    loc_free(c->addr_buf);
    channel_free(c->chan);
    loc_free(c);
}

static void tcp_lock(Channel * channel) {
    ChannelTCP * c = channel2tcp(channel);
    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    c->lock_cnt++;
}

static void tcp_unlock(Channel * channel) {
    ChannelTCP * c = channel2tcp(channel);
    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->lock_cnt > 0);
    c->lock_cnt--;
    if (c->lock_cnt == 0) {
        assert(!c->read_pending);
        delete_channel(c);
    }
}

static int tcp_is_closed(Channel * channel) {
    ChannelTCP * c = channel2tcp(channel);
    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->lock_cnt > 0);
    return c->chan->state == ChannelStateDisconnected;
}

#if ENABLE_OutputQueue
static void done_write_request(void * args) {
    ChannelTCP * c = (ChannelTCP *)((AsyncReqInfo *)args)->client_data;
    int size = 0;
    int error = 0;

    assert(args == &c->wr_req);
    assert(c->socket >= 0);

    if (c->wr_req.u.sio.rval < 0) error = c->wr_req.error;
    else if (c->wr_req.type == AsyncReqSend) size = c->wr_req.u.sio.rval;
    output_queue_done(&c->out_queue, error, size);
    if (error) c->out_errno = error;
    if (output_queue_is_empty(&c->out_queue) &&
        c->chan->state == ChannelStateDisconnected) shutdown(c->socket, SHUT_WR);
    tcp_unlock(c->chan);
}

static void post_write_request(OutputBuffer * bf) {
    ChannelTCP * c = obuf2tcp(bf->queue);

    assert(c->socket >= 0);

    c->wr_req.client_data = c;
    c->wr_req.done = done_write_request;
#if ENABLE_SSL
    if (c->ssl) {
        int wr = SSL_write(c->ssl, bf->buf + bf->buf_pos, bf->buf_len - bf->buf_pos);
        if (wr <= 0) {
            int err = SSL_get_error(c->ssl, wr);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                c->wr_req.type = AsyncReqSelect;
                c->wr_req.u.select.nfds = c->socket + 1;
                FD_ZERO(&c->wr_req.u.select.readfds);
                FD_ZERO(&c->wr_req.u.select.writefds);
                FD_ZERO(&c->wr_req.u.select.errorfds);
                if (err == SSL_ERROR_WANT_WRITE) FD_SETX(c->socket, &c->wr_req.u.select.writefds);
                if (err == SSL_ERROR_WANT_READ) FD_SETX(c->socket, &c->wr_req.u.select.readfds);
                FD_SETX(c->socket, &c->wr_req.u.select.errorfds);
                c->wr_req.u.select.timeout.tv_sec = 10;
                async_req_post(&c->wr_req);
            }
            else {
                int error = set_ssl_errno();
                trace(LOG_PROTOCOL, "Can't SSL_write() on channel %#" PRIxPTR ": %s", (uintptr_t)c, errno_to_str(error));
                c->wr_req.type = AsyncReqSend;
                c->wr_req.error = error;
                c->wr_req.u.sio.rval = -1;
                post_event(done_write_request, &c->wr_req);
            }
        }
        else {
            c->wr_req.type = AsyncReqSend;
            c->wr_req.error = 0;
            c->wr_req.u.sio.rval = wr;
            post_event(done_write_request, &c->wr_req);
        }
    }
    else
#endif
    {
        c->wr_req.type = AsyncReqSend;
        c->wr_req.u.sio.sock = c->socket;
        c->wr_req.u.sio.bufp = bf->buf + bf->buf_pos;
        c->wr_req.u.sio.bufsz = bf->buf_len - bf->buf_pos;
        c->wr_req.u.sio.flags = c->out_queue.queue.next == c->out_queue.queue.prev ? 0 : MSG_MORE;
        async_req_post(&c->wr_req);
    }
    tcp_lock(c->chan);
}
#endif /* ENABLE_OutputQueue */

static void tcp_flush_with_flags(ChannelTCP * c, int flags) {
    unsigned char * p = c->obuf->buf;
    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->chan->out.end == p + sizeof(c->obuf->buf));
    assert(c->out_bin_block == NULL);
    assert(c->chan->out.cur >= p);
    assert(c->chan->out.cur <= p + sizeof(c->obuf->buf));
    if (c->chan->out.cur == p) return;
    if (c->chan->state != ChannelStateDisconnected && c->out_errno == 0) {
#if ENABLE_OutputQueue
        c->obuf->buf_len = c->chan->out.cur - p;
        c->out_queue.post_io_request = post_write_request;
        output_queue_add_obuf(&c->out_queue, c->obuf);
        c->obuf = output_queue_alloc_obuf();
        c->chan->out.end = c->obuf->buf + sizeof(c->obuf->buf);
#else
        assert(c->ssl == NULL);
        while (p < c->chan->out.cur) {
            size_t sz = c->chan->out.cur - p;
            ssize_t wr = send(c->socket, p, sz, flags);
            if (wr < 0) {
                int err = errno;
                trace(LOG_PROTOCOL, "Can't send() on channel %#" PRIxPTR ": %s", (uintptr_t)c, errno_to_str(err));
                c->out_errno = err;
                c->chan->out.cur = c->obuf->buf;
                c->out_eom_cnt = 0;
                return;
            }
            p += wr;
        }
        assert(p == c->chan->out.cur);
#endif
    }
    c->chan->out.cur = c->obuf->buf;
    c->out_eom_cnt = 0;
}

static void tcp_flush_event(void * x) {
    ChannelTCP * c = (ChannelTCP *)x;
    assert(c->magic == CHANNEL_MAGIC);
    if (--c->out_flush_cnt == 0) {
        int congestion_level = c->chan->congestion_level;
        if (congestion_level > 0) usleep(congestion_level * 2500);
        tcp_flush_with_flags(c, 0);
        tcp_unlock(c->chan);
    }
    else if (c->out_eom_cnt > 3) {
        tcp_flush_with_flags(c, 0);
    }
}

static void tcp_bin_block_start(ChannelTCP * c) {
    *c->chan->out.cur++ = ESC;
    *c->chan->out.cur++ = 3;
#if BUF_SIZE > 0x4000
    *c->chan->out.cur++ = 0;
#endif
    *c->chan->out.cur++ = 0;
    *c->chan->out.cur++ = 0;
    c->out_bin_block = c->chan->out.cur;
}

static void tcp_bin_block_end(ChannelTCP * c) {
    size_t len = c->chan->out.cur - c->out_bin_block;
    if (len == 0) {
#if BUF_SIZE > 0x4000
        c->chan->out.cur -= 5;
#else
        c->chan->out.cur -= 4;
#endif
    }
    else {
#if BUF_SIZE > 0x4000
        *(c->out_bin_block - 3) = (len & 0x7fu) | 0x80u;
        *(c->out_bin_block - 2) = ((len >> 7) & 0x7fu) | 0x80u;
        *(c->out_bin_block - 1) = (unsigned char)(len >> 14);
#else
        *(c->out_bin_block - 2) = (len & 0x7fu) | 0x80u;
        *(c->out_bin_block - 1) = (unsigned char)(len >> 7);
#endif
    }
    c->out_bin_block = NULL;
}

static void tcp_write_stream(OutputStream * out, int byte) {
    ChannelTCP * c = channel2tcp(out2channel(out));
    assert(c->magic == CHANNEL_MAGIC);
    if (!c->chan->out.supports_zero_copy || c->chan->out.cur >= c->chan->out.end - 32 || byte < 0) {
        if (c->out_bin_block != NULL) tcp_bin_block_end(c);
        if (c->chan->out.cur == c->chan->out.end) tcp_flush_with_flags(c, MSG_MORE);
        if (byte < 0 || byte == ESC) {
            char esc = 0;
            *c->chan->out.cur++ = ESC;
            if (byte == ESC) esc = 0;
            else if (byte == MARKER_EOM) esc = 1;
            else if (byte == MARKER_EOS) esc = 2;
            else assert(0);
            if (c->chan->out.cur == c->chan->out.end) tcp_flush_with_flags(c, MSG_MORE);
            *c->chan->out.cur++ = esc;
            if (byte == MARKER_EOM) {
                c->out_eom_cnt++;
                if (c->out_flush_cnt < 2) {
                    if (c->out_flush_cnt++ == 0) tcp_lock(c->chan);
                    post_event_with_delay(tcp_flush_event, c, 0);
                }
            }
            return;
        }
    }
    else if (c->out_bin_block == NULL) {
        tcp_bin_block_start(c);
    }
    *c->chan->out.cur++ = (char)byte;
}

static void tcp_write_block_stream(OutputStream * out, const char * bytes, size_t size) {
    unsigned char * src = (unsigned char *)bytes;
    ChannelTCP * c = channel2tcp(out2channel(out));
    while (size > 0) {
        size_t n = out->end - out->cur;
        if (n > size) n = size;
        if (n == 0) {
            tcp_write_stream(out, *src++);
            size--;
        }
        else if (c->out_bin_block) {
            memcpy(out->cur, src, n);
            out->cur += n;
            size -= n;
            src += n;
        }
        else if (*src != ESC) {
            unsigned char * dst = out->cur;
            unsigned char * end = dst + n;
            do {
                unsigned char ch = *src;
                if (ch == ESC) break;
                *dst++ = ch;
                src++;
            }
            while (dst < end);
            size -= dst - out->cur;
            out->cur = dst;
        }
        else {
            tcp_write_stream(out, *src++);
            size--;
        }
    }
}

static ssize_t tcp_splice_block_stream(OutputStream * out, int fd, size_t size, int64_t * offset) {
    assert(is_dispatch_thread());
    if (size == 0) return 0;
#if ENABLE_Splice
    {
        ChannelTCP * c = channel2tcp(out2channel(out));
        if (!c->ssl && out->supports_zero_copy) {
            ssize_t rd = splice(fd, offset, c->pipefd[1], NULL, size, SPLICE_F_MOVE);
            if (rd > 0) {
                /* Send the binary data escape seq */
                size_t n = rd;
                if (c->out_bin_block != NULL) tcp_bin_block_end(c);
                if (c->chan->out.cur >= c->chan->out.end - 8) tcp_flush_with_flags(c, MSG_MORE);
                *c->chan->out.cur++ = ESC;
                *c->chan->out.cur++ = 3;
                for (;;) {
                    if (n <= 0x7fu) {
                        *c->chan->out.cur++ = (char)n;
                        break;
                    }
                    *c->chan->out.cur++ = (n & 0x7fu) | 0x80u;
                    n = n >> 7;
                }
                /* We need to flush the buffer then send our data */
                tcp_flush_with_flags(c, MSG_MORE);

#if ENABLE_OutputQueue
                while (!output_queue_is_empty(&c->out_queue)) {
                    cancel_event(done_write_request, &c->wr_req, 1);
                    done_write_request(&c->wr_req);
                }
#endif

                if (c->chan->state == ChannelStateDisconnected) return rd;
                if (c->out_errno) return rd;

                n = rd;
                while (n > 0) {
                    ssize_t wr = splice(c->pipefd[0], NULL, c->socket, NULL, n, SPLICE_F_MORE);

                    if (wr < 0) {
                        c->out_errno = errno;
                        trace(LOG_PROTOCOL, "Error in socket splice: %s", errno_to_str(errno));
                        break;
                    }
                    n -= wr;
                }
            }
            return rd;
        }
    }
#endif /* ENABLE_Splice */
    {
        ssize_t rd;
        char buffer[BUF_SIZE];
        if (size > BUF_SIZE) size = BUF_SIZE;
        if (offset != NULL) {
            rd = pread(fd, buffer, size, (off_t)*offset);
            if (rd > 0) *offset += rd;
        }
        else {
            rd = read(fd, buffer, size);
        }
        if (rd > 0) tcp_write_block_stream(out, buffer, rd);
        return rd;
    }
}

static void tcp_post_read(InputBuf * ibuf, unsigned char * buf, size_t size) {
    ChannelTCP * c = ibuf2tcp(ibuf);

    if (c->read_pending) return;
    c->read_pending = 1;
    c->read_buf = buf;
    c->read_buf_size = size;
    if (c->ssl) {
#if ENABLE_SSL
        c->read_done = SSL_read(c->ssl, c->read_buf, c->read_buf_size);
        if (c->read_done <= 0) {
            int err = SSL_get_error(c->ssl, c->read_done);
            if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                FD_ZERO(&c->rd_req.u.select.readfds);
                FD_ZERO(&c->rd_req.u.select.writefds);
                FD_ZERO(&c->rd_req.u.select.errorfds);
                if (err == SSL_ERROR_WANT_WRITE) FD_SETX(c->socket, &c->rd_req.u.select.writefds);
                if (err == SSL_ERROR_WANT_READ) FD_SETX(c->socket, &c->rd_req.u.select.readfds);
                FD_SETX(c->socket, &c->rd_req.u.select.errorfds);
                c->rd_req.u.select.timeout.tv_sec = 10;
                c->read_done = -1;
                async_req_post(&c->rd_req);
            }
            else {
                if (c->chan->state != ChannelStateDisconnected) {
                    trace(LOG_ALWAYS, "Can't SSL_read() on channel %#" PRIxPTR ": %s", (uintptr_t)c, errno_to_str(set_ssl_errno()));
                }
                c->read_done = 0;
                post_event(c->rd_req.done, &c->rd_req);
            }
        }
        else {
            post_event(c->rd_req.done, &c->rd_req);
        }
#else
        assert(0);
#endif
    }
    else {
        c->rd_req.u.sio.bufp = buf;
        c->rd_req.u.sio.bufsz = size;
        async_req_post(&c->rd_req);
    }
}

static void tcp_wait_read(InputBuf * ibuf) {
    ChannelTCP * c = ibuf2tcp(ibuf);

    /* Wait for read to complete */
    assert(c->lock_cnt > 0);
    assert(c->read_pending != 0);
    cancel_event(tcp_channel_read_done, &c->rd_req, 1);
    tcp_channel_read_done(&c->rd_req);
}

static int tcp_read_stream(InputStream * inp) {
    Channel * channel = inp2channel(inp);
    ChannelTCP * c = channel2tcp(channel);

    assert(c->lock_cnt > 0);
    if (inp->cur < inp->end) return *inp->cur++;
    return ibuf_get_more(&c->ibuf, 0);
}

static int tcp_peek_stream(InputStream * inp) {
    Channel * channel = inp2channel(inp);
    ChannelTCP * c = channel2tcp(channel);

    assert(c->lock_cnt > 0);
    if (inp->cur < inp->end) return *inp->cur;
    return ibuf_get_more(&c->ibuf, 1);
}

static void send_eof_and_close(Channel * channel, int err) {
    ChannelTCP * c = channel2tcp(channel);

    assert(c->magic == CHANNEL_MAGIC);
    if (channel->state == ChannelStateDisconnected) return;
    ibuf_flush(&c->ibuf);
    if (c->ibuf.handling_msg == HandleMsgTriggered) {
        /* Cancel pending message handling */
        cancel_event(handle_channel_msg, c, 0);
        c->ibuf.handling_msg = HandleMsgIdle;
    }
    write_stream(&c->chan->out, MARKER_EOS);
    write_errno(&c->chan->out, err);
    write_stream(&c->chan->out, MARKER_EOM);
    tcp_flush_with_flags(c, 0);
#if ENABLE_OutputQueue
    if (output_queue_is_empty(&c->out_queue))
#endif
    shutdown(c->socket, SHUT_WR);
    c->chan->state = ChannelStateDisconnected;
    tcp_post_read(&c->ibuf, c->ibuf.buf, c->ibuf.buf_size);
    notify_channel_closed(channel);
    if (channel->disconnected) {
        channel->disconnected(channel);
    }
    else {
        trace(LOG_PROTOCOL, "channel %#" PRIxPTR " disconnected", (uintptr_t)c);
        if (channel->protocol != NULL) protocol_release(channel->protocol);
    }
    channel->protocol = NULL;
}

static void handle_channel_msg(void * x) {
    Trap trap;
    ChannelTCP * c = (ChannelTCP *)x;
    int has_msg;

    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->ibuf.handling_msg == HandleMsgTriggered);
    assert(c->ibuf.message_count);

    has_msg = ibuf_start_message(&c->ibuf);
    if (has_msg <= 0) {
        if (has_msg < 0 && c->chan->state != ChannelStateDisconnected) {
            trace(LOG_PROTOCOL, "Socket is shutdown by remote peer, channel %#" PRIxPTR " %s", (uintptr_t)c, c->chan->peer_name);
            channel_close(c->chan);
        }
    }
    else if (set_trap(&trap)) {
        if (c->chan->receive) {
            c->chan->receive(c->chan);
        }
        else {
            handle_protocol_message(c->chan);
            assert(c->out_bin_block == NULL);
        }
        clear_trap(&trap);
    }
    else {
        trace(LOG_ALWAYS, "Exception in message handler: %s", errno_to_str(trap.error));
        send_eof_and_close(c->chan, trap.error);
    }
}

static void channel_check_pending(Channel * channel) {
    ChannelTCP * c = channel2tcp(channel);

    assert(is_dispatch_thread());
    if (c->ibuf.handling_msg == HandleMsgIdle && c->ibuf.message_count) {
        post_event(handle_channel_msg, c);
        c->ibuf.handling_msg = HandleMsgTriggered;
    }
}

static void tcp_trigger_message(InputBuf * ibuf) {
    ChannelTCP * c = ibuf2tcp(ibuf);

    assert(is_dispatch_thread());
    assert(c->ibuf.message_count > 0);
    if (c->ibuf.handling_msg == HandleMsgIdle) {
        post_event(handle_channel_msg, c);
        c->ibuf.handling_msg = HandleMsgTriggered;
    }
}

static int channel_get_message_count(Channel * channel) {
    ChannelTCP * c = channel2tcp(channel);
    assert(is_dispatch_thread());
    if (c->ibuf.handling_msg != HandleMsgTriggered) return 0;
    return c->ibuf.message_count;
}

static void tcp_channel_read_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    ChannelTCP * c = (ChannelTCP *)req->client_data;
    ssize_t len = 0;

    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->read_pending != 0);
    assert(c->lock_cnt > 0);
    c->read_pending = 0;
    if (c->ssl) {
#if ENABLE_SSL
        if (c->read_done < 0) {
            tcp_post_read(&c->ibuf, c->read_buf, c->read_buf_size);
            return;
        }
        len = c->read_done;
#else
        assert(0);
#endif
    }
    else {
        assert(c->read_buf == c->rd_req.u.sio.bufp);
        assert((size_t)c->read_buf_size == c->rd_req.u.sio.bufsz);
        len = c->rd_req.u.sio.rval;
        if (req->error) {
            if (c->chan->state != ChannelStateDisconnected) {
                trace(LOG_ALWAYS, "Can't read from socket: %s", errno_to_str(req->error));
            }
            len = 0; /* Treat error as EOF */
        }
    }
    if (c->chan->state != ChannelStateDisconnected) {
        ibuf_read_done(&c->ibuf, len);
    }
    else if (len > 0) {
        tcp_post_read(&c->ibuf, c->ibuf.buf, c->ibuf.buf_size);
    }
    else {
        tcp_unlock(c->chan);
    }
}

static void start_channel(Channel * channel) {
    ChannelTCP * c = channel2tcp(channel);

    assert(is_dispatch_thread());
    assert(c->magic == CHANNEL_MAGIC);
    assert(c->socket >= 0);
    notify_channel_created(c->chan);
    if (c->chan->connecting) {
        c->chan->connecting(c->chan);
    }
    else {
        trace(LOG_PROTOCOL, "channel server connecting");
        send_hello_message(c->chan);
    }
    ibuf_trigger_read(&c->ibuf);
}

static ChannelTCP * create_channel(int sock, int en_ssl, int server, int unix_domain) {
    const int i = 1;
    ChannelTCP * c;
    SSL * ssl = NULL;

    assert(sock >= 0);
    if (!unix_domain) {
        if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&i, sizeof(i)) < 0) {
            int error = errno;
            trace(LOG_ALWAYS, "Can't set TCP_NODELAY option on a socket: %s", errno_to_str(error));
            errno = error;
            return NULL;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&i, sizeof(i)) < 0) {
            int error = errno;
            trace(LOG_ALWAYS, "Can't set SO_KEEPALIVE option on a socket: %s", errno_to_str(error));
            errno = error;
            return NULL;
        }
    }

    {
        /* Buffer sizes need to be large enough to avoid deadlocking when agent connects to itself */
        int snd_org = 0;
        int rcv_org = 0;
        socklen_t snd_len = sizeof(snd_org);
        socklen_t rcv_len = sizeof(rcv_org);
        int snd_buf = SOCKET_SEND_BUFFER_MINSIZE;
        int rcv_buf = SOCKET_RECV_BUFFER_MINSIZE;

        if (getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&snd_org, &snd_len) < 0) {
            trace(LOG_ALWAYS, "getsockopt(SOL_SOCKET,SO_SNDBUF,...) error: %s", errno_to_str(errno));
        }
        else if (snd_org < snd_buf && setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&snd_buf, sizeof(snd_buf)) < 0) {
            trace(LOG_ALWAYS, "setsockopt(SOL_SOCKET,SO_SNDBUF,%d) error: %s", snd_buf, errno_to_str(errno));
        }

        if (getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_org, &rcv_len) < 0) {
            trace(LOG_ALWAYS, "getsockopt(SOL_SOCKET,SO_RCVBUF,...) error: %s", errno_to_str(errno));
        }
        else if (rcv_org < rcv_buf && setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (char *)&rcv_buf, sizeof(rcv_buf)) < 0) {
            trace(LOG_ALWAYS, "setsockopt(SOL_SOCKET,SO_RCVBUF,%d) error: %s", rcv_buf, errno_to_str(errno));
        }
    }

    if (en_ssl) {
#if ENABLE_SSL
        if (ssl_ctx == NULL) {
            int err = 0;
            const char * agent_id = get_agent_id();
            unsigned char buf[SSL_MAX_SSL_SESSION_ID_LENGTH];
            unsigned buf_pos = 0;
            char fnm[FILE_PATH_SIZE];
            FILE * fp = NULL;
            DH * ssl_dh = NULL;
            X509 * ssl_cert = NULL;

            ini_ssl();
            ssl_ctx = SSL_CTX_new(SSLv23_method());
            SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, certificate_verify_callback);
            while (buf_pos < sizeof(buf) && *agent_id) {
                if (*agent_id != '-') buf[buf_pos++] = *agent_id;
                agent_id++;
            }
            if (!SSL_CTX_set_session_id_context(ssl_ctx, buf, buf_pos)) err = set_ssl_errno();
            if (!err && (ssl_dh = get_dh_key()) == NULL) err = set_ssl_errno();
            if (!err) {
                int codes = 0;
                if (!DH_check(ssl_dh, &codes)) {
                    err = set_ssl_errno();
                }
                else {
                    const BIGNUM * p = NULL;
                    const BIGNUM * g = NULL;
                    DH_get0_pqg(ssl_dh, &p, NULL, &g);
                    if (BN_is_word(g, DH_GENERATOR_2)) {
                        long residue = BN_mod_word(p, 24);
                        if (residue == 11 || residue == 23) {
                            codes &= ~DH_NOT_SUITABLE_GENERATOR;
                        }
                    }
                    if (codes & DH_UNABLE_TO_CHECK_GENERATOR) {
                        err = set_errno(ERR_OTHER, "DH_check: failed to test generator");
                    }
                    else if (codes & DH_NOT_SUITABLE_GENERATOR) {
                        err = set_errno(ERR_OTHER, "DH_check: not a suitable generator");
                    }
                    else if (codes & DH_CHECK_P_NOT_PRIME) {
                        err = set_errno(ERR_OTHER, "DH_check: not a prime");
                    }
                    else if (codes & DH_CHECK_P_NOT_SAFE_PRIME) {
                        err = set_errno(ERR_OTHER, "DH_check: not a safe prime");
                    }
                }
            }
            if (!err && !SSL_CTX_set_tmp_dh(ssl_ctx, ssl_dh)) err = set_ssl_errno();
            if (ssl_dh != NULL) {
                DH_free(ssl_dh);
                ssl_dh = NULL;
            }
            if (err) {
                SSL_CTX_free(ssl_ctx);
                ssl_ctx = NULL;
                trace(LOG_ALWAYS, "Cannot create SSL context: %s", errno_to_str(err));
                set_errno(err, "Cannot create SSL context");
                return NULL;
            }
            snprintf(fnm, sizeof(fnm), "%s/ssl/local.priv", tcf_dir);
            if (!err && (fp = fopen(fnm, "r")) == NULL) err = errno;
            if (!err && (rsa_key = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL)) == NULL) err = set_ssl_errno();
            if (!err && fclose(fp) != 0) err = errno;
            if (!err) {
                snprintf(fnm, sizeof(fnm), "%s/ssl/local.cert", tcf_dir);
                if (!err && (fp = fopen(fnm, "r")) == NULL) err = errno;
                if (!err && (ssl_cert = PEM_read_X509(fp, NULL, NULL, NULL)) == NULL) err = set_ssl_errno();
                if (!err && fclose(fp) != 0) err = errno;
            }
            if (!err) {
                SSL_CTX_use_certificate(ssl_ctx, ssl_cert);
                SSL_CTX_use_RSAPrivateKey(ssl_ctx, rsa_key);
                if (!SSL_CTX_check_private_key(ssl_ctx)) err = set_ssl_errno();
            }
            if (err) {
                SSL_CTX_free(ssl_ctx);
                ssl_ctx = NULL;
                trace(LOG_ALWAYS, "Cannot read SSL certificate %s: %s", fnm, errno_to_str(err));
                set_errno(err, "Cannot read SSL certificate");
                return NULL;
            }
        }

#if defined(_WIN32) || defined(__CYGWIN__)
        {
            unsigned long opts = 1;
            if (ioctlsocket((SOCKET)sock, FIONBIO, &opts) < 0) return NULL;
        }
#else
        {
            long opts = 0;
            if ((opts = fcntl(sock, F_GETFL, NULL)) < 0) return NULL;
            opts |= O_NONBLOCK;
            if (fcntl(sock, F_SETFL, opts) < 0) return NULL;
        }
#endif
        ssl = SSL_new(ssl_ctx);
#if OPENSSL_VERSION_NUMBER >= 0x10100000
        SSL_set_min_proto_version(ssl, TLS1_1_VERSION);
#endif
        if (log_mode & LOG_PROTOCOL) {
            int index = 0;
            const char * name = NULL;
            trace(LOG_PROTOCOL, "Enabled SSL cipher suites:");
            for (index = 0;; index++) {
                name = SSL_get_cipher_list(ssl, index);
                if (name == NULL) break;
                trace(LOG_PROTOCOL, "  %s", name);
            }
        }
        SSL_set_fd(ssl, sock);
        if (server) SSL_set_accept_state(ssl);
        else SSL_set_connect_state(ssl);
#endif
    }

    c = (ChannelTCP *)loc_alloc_zero(sizeof *c);
    c->chan = channel_alloc();
    channel2tcp(c->chan) = c;
#if ENABLE_Splice
    if (pipe(c->pipefd) == -1) {
        int err = errno;
        channel_free(c->chan);
        loc_free(c);
        trace(LOG_ALWAYS, "Cannot create channel pipe : %s", errno_to_str(err));
        errno = err;
        return NULL;
    }
#endif /* ENABLE_Splice */
    c->magic = CHANNEL_MAGIC;
    c->ssl = ssl;
    c->unix_domain = unix_domain;
    c->chan->inp.read = tcp_read_stream;
    c->chan->inp.peek = tcp_peek_stream;
    c->obuf = output_queue_alloc_obuf();
    c->chan->out.cur = c->obuf->buf;
    c->chan->out.end = c->obuf->buf + sizeof(c->obuf->buf);
    c->chan->out.write = tcp_write_stream;
    c->chan->out.write_block = tcp_write_block_stream;
    c->chan->out.splice_block = tcp_splice_block_stream;
    list_add_last(&c->chan->chanlink, &channel_root);
    shutdown_set_normal(&channel_shutdown);
    c->chan->state = ChannelStateStartWait;
    c->chan->incoming = server;
    c->chan->start_comm = start_channel;
    c->chan->check_pending = channel_check_pending;
    c->chan->message_count = channel_get_message_count;
    c->chan->lock = tcp_lock;
    c->chan->unlock = tcp_unlock;
    c->chan->is_closed = tcp_is_closed;
    c->chan->close = send_eof_and_close;
    ibuf_init(&c->ibuf, &c->chan->inp);
    c->ibuf.post_read = tcp_post_read;
    c->ibuf.wait_read = tcp_wait_read;
    c->ibuf.trigger_message = tcp_trigger_message;
    c->socket = sock;
    c->lock_cnt = 1;
    c->rd_req.done = tcp_channel_read_done;
    c->rd_req.client_data = c;
    if (c->ssl) {
#if ENABLE_SSL
        c->rd_req.type = AsyncReqSelect;
        c->rd_req.u.select.nfds = c->socket + 1;
#else
        assert(0);
#endif
    }
    else {
        c->rd_req.type = AsyncReqRecv;
        c->rd_req.u.sio.sock = c->socket;
        c->rd_req.u.sio.flags = 0;
    }
#if ENABLE_OutputQueue
    output_queue_ini(&c->out_queue);
#endif
    return c;
}

static void refresh_peer_server(int sock, PeerServer * ps) {
    unsigned i;
    const char * transport = peer_server_getprop(ps, "TransportName", NULL);
    assert(transport != NULL);
    if (strcmp(transport, "UNIX") == 0) {
        PeerServer * ps2 = peer_server_alloc();
        char * str_id = loc_printf("%s:%s", transport, peer_server_getprop(ps, "Host", ""));
        ps2->flags = ps->flags;
        for (i = 0; i < ps->ind; i++) {
            peer_server_addprop(ps2, loc_strdup(ps->list[i].name), loc_strdup(ps->list[i].value));
        }
        for (i = 0; str_id[i]; i++) {
            /* Character '/' is prohibited in a peer ID string */
            if (str_id[i] == '/') str_id[i] = '|';
        }
        peer_server_addprop(ps2, loc_strdup("ID"), str_id);
        peer_server_add(ps2, PEER_DATA_RETENTION_PERIOD * 2);
    }
    else {
        struct sockaddr_in sin;
#if defined(_WRS_KERNEL)
        int sinlen;
#else
        socklen_t sinlen;
#endif
        const char * str_port = peer_server_getprop(ps, "Port", NULL);
        int ifcind;
        struct in_addr src_addr;
        ip_ifc_info ifclist[MAX_IFC];
        sinlen = sizeof sin;
        if (getsockname(sock, (struct sockaddr *)&sin, &sinlen) != 0) {
            trace(LOG_ALWAYS, "refresh_peer_server: getsockname error: %s", errno_to_str(errno));
            return;
        }
        ifcind = build_ifclist(sock, MAX_IFC, ifclist);
        while (ifcind-- > 0) {
            char str_host[64];
            PeerServer * ps2;
            if (sin.sin_addr.s_addr != INADDR_ANY &&
                (ifclist[ifcind].addr & ifclist[ifcind].mask) !=
                (sin.sin_addr.s_addr & ifclist[ifcind].mask)) {
                continue;
            }
            src_addr.s_addr = ifclist[ifcind].addr;
            ps2 = peer_server_alloc();
            ps2->flags = ps->flags;
            for (i = 0; i < ps->ind; i++) {
                peer_server_addprop(ps2, loc_strdup(ps->list[i].name), loc_strdup(ps->list[i].value));
            }
            inet_ntop(AF_INET, &src_addr, str_host, sizeof(str_host));
            peer_server_addprop(ps2, loc_strdup("ID"), loc_printf("%s:%s:%s", transport, str_host, str_port));
            peer_server_addprop(ps2, loc_strdup("Host"), loc_strdup(str_host));
            peer_server_addprop(ps2, loc_strdup("Port"), loc_strdup(str_port));
            peer_server_add(ps2, PEER_DATA_RETENTION_PERIOD * 2);
        }
    }
}

static void refresh_all_peer_servers(void * x) {
    LINK * l = server_list.next;
    while (l != &server_list) {
        ServerTCP * si = servlink2tcp(l);
        refresh_peer_server(si->sock, si->serv.ps);
        l = l->next;
    }
    post_event_with_delay(refresh_all_peer_servers, NULL, PEER_DATA_REFRESH_PERIOD * 1000000);
}

static void set_peer_addr(ChannelTCP * c, struct sockaddr * addr, int addr_len) {
    /* Create a human readable channel name that uniquely identifies remote peer */
#if ENABLE_Unix_Domain
    if (c->unix_domain) {
        assert(addr->sa_family == AF_UNIX);
        c->chan->peer_name = loc_printf("UNIX:%s", ((struct sockaddr_un *)addr)->sun_path);
    }
    else
#endif
    {
        char nbuf[128];
        assert(addr->sa_family == AF_INET);
        c->chan->peer_name = loc_printf("%s:%s:%d",
                c->ssl != NULL ? "SSL" : "TCP",
                inet_ntop(addr->sa_family, &((struct sockaddr_in *)addr)->sin_addr, nbuf, sizeof(nbuf)),
                ntohs(((struct sockaddr_in *)addr)->sin_port));
    }
    c->addr_len = addr_len;
    c->addr_buf = (struct sockaddr *)loc_alloc(addr_len);
    memcpy(c->addr_buf, addr, addr_len);
}

static void tcp_server_accept_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    ServerTCP * si = (ServerTCP *)req->client_data;

    if (si->sock < 0) {
        /* Server closed. */
        loc_free(si->addr_buf);
        loc_free(si);
        return;
    }
    if (req->error) {
        trace(LOG_ALWAYS, "Socket accept failed: %s", errno_to_str(req->error));
    }
    else {
        int ssl = strcmp(peer_server_getprop(si->serv.ps, "TransportName", ""), "SSL") == 0;
        int unix_domain = si->addr_buf->sa_family == AF_UNIX;
        ChannelTCP * c = create_channel(req->u.acc.rval, ssl, 1, unix_domain);
        if (c == NULL) {
            trace(LOG_ALWAYS, "Cannot create channel for accepted connection: %s", errno_to_str(errno));
            closesocket(req->u.acc.rval);
        }
        else {
            set_peer_addr(c, si->addr_buf, si->addr_len);
            si->serv.new_conn(&si->serv, c->chan);
        }
    }
    si->accreq.u.acc.addrlen = si->addr_len;
    async_req_post(req);
}

static void server_close(ChannelServer * serv) {
    ServerTCP * s = server2tcp(serv);

    assert(is_dispatch_thread());
    if (s->sock < 0) return;
    list_remove(&s->serv.servlink);
    if (list_is_empty(&channel_root) && list_is_empty(&channel_server_root))
        shutdown_set_stopped(&channel_shutdown);
    list_remove(&s->servlink);
    peer_server_free(s->serv.ps);
    shutdown(s->sock, SHUT_RDWR);
    closesocket(s->sock);
    s->sock = -1;
    /* TODO: free server struct */
}

static ChannelServer * channel_server_create(PeerServer * ps, int sock) {
    ServerTCP * si = (ServerTCP *)loc_alloc_zero(sizeof *si);
    /* TODO: need to investigate usage of sizeof(sockaddr_storage) for address buffer size */
#if defined(_WRS_KERNEL)
    /* vxWorks requires buffer size to be exactly sizeof(struct sockaddr) */
    si->addr_len = sizeof(struct sockaddr);
#elif defined(SOCK_MAXADDRLEN)
    si->addr_len = SOCK_MAXADDRLEN;
#else
    si->addr_len = 0x1000;
#endif
    si->addr_buf = (struct sockaddr *)loc_alloc_zero(si->addr_len);
    si->serv.close = server_close;
    si->sock = sock;
    si->serv.ps = ps;
    if (server_list.next == NULL) {
        list_init(&server_list);
        post_event_with_delay(refresh_all_peer_servers, NULL, PEER_DATA_REFRESH_PERIOD * 1000000);
    }
    list_add_last(&si->serv.servlink, &channel_server_root);
    shutdown_set_normal(&channel_shutdown);
    list_add_last(&si->servlink, &server_list);
    refresh_peer_server(sock, ps);

    si->accreq.done = tcp_server_accept_done;
    si->accreq.client_data = si;
    si->accreq.type = AsyncReqAccept;
    si->accreq.u.acc.sock = sock;
    si->accreq.u.acc.addr = si->addr_buf;
    si->accreq.u.acc.addrlen = si->addr_len;
    async_req_post(&si->accreq);
    return &si->serv;
}

#if ENABLE_Unix_Domain

static int setup_unix_sockaddr(PeerServer * ps, struct sockaddr_un * localhost) {
    const char * host = peer_server_getprop(ps, "Host", NULL);
    if (host == NULL) return ERR_UNKNOWN_PEER;

    memset(localhost, 0, sizeof(struct sockaddr_un));

    if (strlen(host) >= sizeof(localhost->sun_path)) {
        trace(LOG_ALWAYS, "Socket file path is too long (%u > %u)",
            (unsigned)strlen(host), (unsigned)sizeof(localhost->sun_path) - 1);
        return E2BIG;
    }

    localhost->sun_family = AF_UNIX;
    // length checked above
    strlcpy(localhost->sun_path, host, sizeof(localhost->sun_path));

#if defined _WIN32 || defined __SYMBIAN32__
    // For Windows and Symbian, the path needs to be in Unix format
    // (the ':' will otherwise delineate the unused port), so convert that back
    // to an actual host filename
    if (host[0] == '/' && isalpha(host[1]) && host[2] == '/') {
        localhost->sun_path[0] = localhost->sun_path[1];
        localhost->sun_path[1] = ':';
    }
#endif

    return 0;
}

ChannelServer * channel_unix_server(PeerServer * ps) {
    int sock = -1;
    int error = 0;
    const char * reason = NULL;
    struct sockaddr_un localhost;
    struct stat st;

    assert(is_dispatch_thread());

    if ((error = setup_unix_sockaddr(ps, &localhost)) != 0) {
        reason = "address setup";
    }
    if (!error && stat(localhost.sun_path, &st) == 0 && S_ISSOCK(st.st_mode) && remove(localhost.sun_path) < 0) {
        error = errno;
        reason = "remove";
    }
    if (!error && (sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) {
        error = errno;
        reason = "create";
    }
    if (!error && bind(sock, (struct sockaddr *)&localhost, SUN_LEN(&localhost))) {
        error = errno;
        reason = "bind";
    }
    if (!error && listen(sock, 16)) {
        error = errno;
        reason = "listen";
    }
    if (error) {
        if (sock >= 0) closesocket(sock);
        trace(LOG_ALWAYS, "Socket %s error on %s: %s", reason, localhost.sun_path, errno_to_str(error));
        set_fmt_errno(error, "Socket %s error", reason);
        return NULL;
    }

    return channel_server_create(ps, sock);
}
#else
ChannelServer * channel_unix_server(PeerServer * ps) {
    errno = ERR_UNSUPPORTED;
    return NULL;
}
#endif

ChannelServer * channel_tcp_server(PeerServer * ps) {
    int sock;
    int error;
    const char * reason = NULL;
    struct addrinfo hints;
    struct addrinfo * reslist = NULL;
    struct addrinfo * res;
    const char * host = peer_server_getprop(ps, "Host", NULL);
    const char * port = peer_server_getprop(ps, "Port", NULL);
    int def_port = 0;
    char port_str[16];
    struct sockaddr_in sin;
#if defined(_WRS_KERNEL)
    int sinlen;
#else
    socklen_t sinlen;
#endif

    assert(is_dispatch_thread());
    if (port == NULL) {
        sprintf(port_str, "%d", DISCOVERY_TCF_PORT);
        port = port_str;
        def_port = 1;
    }
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
    sock = -1;

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
            if (def_port && res->ai_addr->sa_family == AF_INET) {
                struct sockaddr_in addr;
                trace(LOG_ALWAYS, "Cannot bind to default TCP port %d: %s",
                    DISCOVERY_TCF_PORT, errno_to_str(error));
                assert(sizeof(addr) >= res->ai_addrlen);
                memset(&addr, 0, sizeof(addr));
                memcpy(&addr, res->ai_addr, res->ai_addrlen);
                addr.sin_port = 0;
                error = 0;
                if (bind(sock, (struct sockaddr *)&addr, sizeof(addr))) {
                    error = errno;
                }
            }
            if (error) {
                reason = "bind";
                closesocket(sock);
                sock = -1;
                continue;
            }
        }
        if (listen(sock, 16)) {
            error = errno;
            reason = "listen on";
            closesocket(sock);
            sock = -1;
            continue;
        }

        /* Only create one listener - don't see how getaddrinfo with
         * the given arguments could return more then one anyway */
        break;
    }
    loc_freeaddrinfo(reslist);
    if (sock < 0) {
        trace(LOG_ALWAYS, "Socket %s error: %s", reason, errno_to_str(error));
        set_fmt_errno(error, "Socket %s error", reason);
        return NULL;
    }

    /* Get port property in case the default port could not be used or
     * the client specified a port that the system converts to a
     * dynamic port number. */
    sinlen = sizeof sin;
    if (getsockname(sock, (struct sockaddr *)&sin, &sinlen) < 0) {
        error = errno;
        trace(LOG_ALWAYS, "getsockname error: %s", errno_to_str(errno));
        closesocket(sock);
        errno = error;
        return NULL;
    }
    snprintf(port_str, sizeof(port_str), "%d", ntohs(sin.sin_port));
    peer_server_addprop(ps, loc_strdup("Port"), loc_strdup(port_str));

    return channel_server_create(ps, sock);
}

typedef struct ChannelConnectInfo {
    ChannelConnectCallBack callback;
    void * callback_args;
    int ssl;
    struct sockaddr * addr_buf;
    int addr_len;
    int sock;
    AsyncReqInfo req;
} ChannelConnectInfo;

static void channel_tcp_connect_done(void * args) {
    ChannelConnectInfo * info = (ChannelConnectInfo *)((AsyncReqInfo *)args)->client_data;
    if (info->req.error) {
        info->callback(info->callback_args, info->req.error, NULL);
        closesocket(info->sock);
    }
    else {
        ChannelTCP * c = create_channel(info->sock, info->ssl, 0, info->addr_buf->sa_family == AF_UNIX);
        if (c == NULL) {
            info->callback(info->callback_args, errno, NULL);
            closesocket(info->sock);
        }
        else {
            set_peer_addr(c, info->addr_buf, info->addr_len);
            info->callback(info->callback_args, 0, c->chan);
        }
    }
    loc_free(info->addr_buf);
    loc_free(info);
}

void channel_tcp_connect(PeerServer * ps, ChannelConnectCallBack callback, void * callback_args) {
    int error;
    const char * host = peer_server_getprop(ps, "Host", NULL);
    const char * port = peer_server_getprop(ps, "Port", NULL);
    struct addrinfo hints;
    struct addrinfo * reslist = NULL;
    ChannelConnectInfo * info = NULL;
    char port_str[16];

    if (port == NULL) {
        sprintf(port_str, "%d", DISCOVERY_TCF_PORT);
        port = port_str;
    }
    memset(&hints, 0, sizeof hints);
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    error = loc_getaddrinfo(host, port, &hints, &reslist);
    if (error) error = set_gai_errno(error);
    if (!error) {
        struct addrinfo * res;
        info = (ChannelConnectInfo *)loc_alloc_zero(sizeof(ChannelConnectInfo));
        info->sock = -1;
        for (res = reslist; res != NULL; res = res->ai_next) {
            info->addr_len = res->ai_addrlen;
            info->addr_buf = (struct sockaddr *)loc_alloc(res->ai_addrlen);
            memcpy(info->addr_buf, res->ai_addr, res->ai_addrlen);
            info->sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
            if (info->sock < 0) {
                error = errno;
            }
            else {
                error = 0;
                break;
            }
        }
        loc_freeaddrinfo(reslist);
    }
    if (!error && info->addr_buf == NULL) error = ENOENT;
    if (error) {
        if (info != NULL) {
            if (info->sock >= 0) closesocket(info->sock);
            loc_free(info->addr_buf);
            loc_free(info);
        }
        callback(callback_args, error, NULL);
    }
    else {
        info->callback = callback;
        info->callback_args = callback_args;
        info->ssl = strcmp(peer_server_getprop(ps, "TransportName", ""), "SSL") == 0;
        info->req.client_data = info;
        info->req.done = channel_tcp_connect_done;
        info->req.type = AsyncReqConnect;
        info->req.u.con.sock = info->sock;
        info->req.u.con.addr = info->addr_buf;
        info->req.u.con.addrlen = info->addr_len;
        async_req_post(&info->req);
    }
}

#if ENABLE_Unix_Domain
void channel_unix_connect(PeerServer * ps, ChannelConnectCallBack callback, void * callback_args) {
    int error = 0;
    ChannelConnectInfo * info = (ChannelConnectInfo *)loc_alloc_zero(sizeof(ChannelConnectInfo));

    info->sock = -1;
    info->addr_len = sizeof(struct sockaddr_un);
    info->addr_buf = (struct sockaddr *)loc_alloc(sizeof(struct sockaddr_un));
    error = setup_unix_sockaddr(ps, (struct sockaddr_un *)info->addr_buf);
    if (!error && (info->sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0) error = errno;

    if (error) {
        if (info->sock >= 0) closesocket(info->sock);
        loc_free(info->addr_buf);
        loc_free(info);
        callback(callback_args, error, NULL);
    }
    else {
        info->callback = callback;
        info->callback_args = callback_args;
        info->ssl = 0;
        info->req.client_data = info;
        info->req.done = channel_tcp_connect_done;
        info->req.type = AsyncReqConnect;
        info->req.u.con.sock = info->sock;
        info->req.u.con.addr = info->addr_buf;
        info->req.u.con.addrlen = info->addr_len;
        async_req_post(&info->req);
    }
}

#else
void channel_unix_connect(PeerServer * ps, ChannelConnectCallBack callback, void * callback_args) {
    callback(callback_args, ERR_INV_TRANSPORT, NULL);
}
#endif

void channel_tcp_network_changed(void) {
    if (list_is_empty(&server_list)) return;
    cancel_event(refresh_all_peer_servers, NULL, 0);
    post_event(refresh_all_peer_servers, NULL);
}

void generate_ssl_certificate(void) {
#if ENABLE_SSL
    char subject_name[256];
    char fnm[FILE_PATH_SIZE];
    X509 * cert = NULL;
    RSA * rsa = NULL;
    BIGNUM * bne = NULL;
    EVP_PKEY * rsa_key = NULL;
    ASN1_INTEGER * serial = NULL;
    X509_NAME * name = NULL;
    int err = 0;
    struct stat st;
    FILE * fp = NULL;

    ini_ssl();
#if OPENSSL_VERSION_NUMBER >= 0x10100000
    /* RSA_generate_key() is deprecated in OpenSSL 1.1.0 */
    bne = BN_new();
    rsa = RSA_new();
    if (!err && !BN_set_word(bne, 3)) err = set_ssl_errno();
    if (!err && !RSA_generate_key_ex(rsa, 2048, bne, NULL)) err = set_ssl_errno();
#else
    if (!err && (rsa = RSA_generate_key(2048, 3, NULL, NULL)) == NULL) err = set_ssl_errno();
#endif
    if (!err && !RSA_check_key(rsa)) err = set_ssl_errno();
    if (!err && gethostname(subject_name, sizeof(subject_name)) != 0) err = errno;
    if (!err) {
        rsa_key = EVP_PKEY_new();
        EVP_PKEY_assign_RSA(rsa_key, rsa);
        cert = X509_new();
        X509_set_version(cert, 2L);
        serial = ASN1_INTEGER_new();
        ASN1_INTEGER_set(serial, 1);
        X509_set_serialNumber(cert, serial);
        ASN1_INTEGER_free(serial);
        X509_gmtime_adj(X509_get_notBefore(cert), 0L);
        X509_gmtime_adj(X509_get_notAfter(cert), 60 * 60 * 24 * 365L * 10L);
        name = X509_get_subject_name(cert);
        X509_NAME_add_entry_by_txt(name, (char *) "commonName", MBSTRING_ASC,
            (unsigned char *)subject_name, strlen(subject_name), -1, 0);
        name = X509_get_issuer_name(cert);
        X509_NAME_add_entry_by_txt(name, (char *) "commonName", MBSTRING_ASC,
            (unsigned char *)issuer_name, strlen(issuer_name), -1, 0);
    }
    if (!err && !X509_set_pubkey(cert, rsa_key)) err = set_ssl_errno();
    if (!err) X509_sign(cert, rsa_key, EVP_sha256());
    if (!err && !X509_verify(cert, rsa_key)) err = set_ssl_errno();
    if (stat(tcf_dir, &st) != 0 && mkdir(tcf_dir, MKDIR_MODE_TCF) != 0) err = errno;
    snprintf(fnm, sizeof(fnm), "%s/ssl", tcf_dir);
    if (stat(fnm, &st) != 0 && mkdir(fnm, MKDIR_MODE_SSL) != 0) err = errno;
    snprintf(fnm, sizeof(fnm), "%s/ssl/local.priv", tcf_dir);
    if (!err && (fp = fopen(fnm, "w")) == NULL) err = errno;
    if (!err && !PEM_write_PKCS8PrivateKey(fp, rsa_key, NULL, NULL, 0, NULL, NULL)) err = set_ssl_errno();
    if (!err && fclose(fp) != 0) err = errno;
#if !(defined(_WIN32) || defined(__CYGWIN__))
    if (!err && chmod(fnm, S_IRUSR|S_IWUSR) != 0) err = errno;
#endif
    snprintf(fnm, sizeof(fnm), "%s/ssl/local.cert", tcf_dir);
    if (!err && (fp = fopen(fnm, "w")) == NULL) err = errno;
    if (!err && !PEM_write_X509(fp, cert)) err = set_ssl_errno();
    if (!err && fclose(fp) != 0) err = errno;
#if !(defined(_WIN32) || defined(__CYGWIN__))
    if (!err && chmod(fnm, S_IRUSR|S_IWUSR) != 0) err = errno;
#endif
    if (err) {
        fprintf(stderr, "Cannot create SSL certificate: %s\n", errno_to_str(err));
    }
    if (cert != NULL) X509_free(cert);
    if (rsa_key != NULL) EVP_PKEY_free(rsa_key);
    else if (rsa != NULL) RSA_free(rsa);
    if (bne != NULL) BN_free(bne);
#else /* ENABLE_SSL */
    fprintf(stderr, "SSL support not available\n");
#endif /* ENABLE_SSL */
}

void ini_channel_tcp(void) {
    channel_tcp_extension_offset = channel_extension(sizeof(ChannelTCP *));
}
