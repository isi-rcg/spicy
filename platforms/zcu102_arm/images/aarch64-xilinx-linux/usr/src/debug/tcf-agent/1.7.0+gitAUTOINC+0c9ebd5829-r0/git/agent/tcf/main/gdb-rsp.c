/*******************************************************************************
 * Copyright (c) 2016-2020 Xilinx, Inc. and others.
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


#include <tcf/config.h>

#if !defined(ENABLE_GdbRemoteSerialProtocol)
#  define ENABLE_GdbRemoteSerialProtocol 0
#endif

#if ENABLE_GdbRemoteSerialProtocol

#include <assert.h>

#include <tcf/framework/errors.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/context.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/link.h>
#include <tcf/framework/json.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/registers.h>
#include <tcf/services/breakpoints.h>
#include <tcf/services/memorymap.h>

#include <machine/i386/tcf/cpu-regs-gdb.h>
#include <machine/x86_64/tcf/cpu-regs-gdb.h>
#include <machine/arm/tcf/cpu-regs-gdb.h>
#include <machine/a64/tcf/cpu-regs-gdb.h>
#include <machine/powerpc/tcf/cpu-regs-gdb.h>
#include <machine/ppc64/tcf/cpu-regs-gdb.h>
#include <machine/microblaze/tcf/cpu-regs-gdb.h>
#include <machine/microblaze64/tcf/cpu-regs-gdb.h>
#include <machine/riscv/tcf/cpu-regs-gdb.h>
#include <machine/riscv64/tcf/cpu-regs-gdb.h>

#include <tcf/main/gdb-rsp.h>

/*
(gdb) set remotetimeout 1000
(gdb) target extended-remote localhost:3000
*/

#ifndef DEBUG_RSP
#  define DEBUG_RSP 0
#endif

#define ID_ANY ~0u

typedef struct GdbServer {
    LINK link_a2s;
    LINK link_s2c;
    int disposed;
    AsyncReqInfo req;
    char port[32];
    char isa[32];
} GdbServer;

typedef struct GdbClient {
    LINK link_s2c;
    LINK link_c2p;
    size_t buf_max;
    uint8_t * buf;
    AsyncReqInfo req;
    GdbServer * server;
    ClientConnection client;
    int closed;

    /* Command packet */
    char * cmd_buf;
    unsigned cmd_pos;
    unsigned cmd_max;
    unsigned cmd_end;
    int cmd_esc;

    /* Response packet */
    char * res_buf;
    unsigned res_pos;
    unsigned res_max;
    unsigned xfer_range_offs;
    unsigned xfer_range_size;

    unsigned start_timer;
    unsigned process_id_cnt;
    unsigned cur_c_pid;
    unsigned cur_c_tid;
    unsigned cur_g_pid;
    unsigned cur_g_tid;
    int no_ack_mode;
    int multiprocess;
    int swbreak;
    int hwbreak;
    int extended;
    int stopped;
    int waiting;
} GdbClient;

typedef struct GdbProcess {
    LINK link_c2p;
    LINK link_p2t;
    LINK link_p2b;
    GdbClient * client;
    unsigned pid;
    Context * ctx;
    unsigned thread_id_cnt;
    int attached;
} GdbProcess;

typedef struct GdbBreakpoint {
    LINK link_p2b;
    unsigned type;
    unsigned kind;
    uint64_t addr;
    GdbProcess * process;
    BreakpointInfo * bp;
} GdbBreakpoint;

typedef struct GdbThread {
    LINK link_p2t;
    GdbProcess * process;
    unsigned tid;
    Context * ctx;
    RegisterDefinition ** regs_nm_map;
    unsigned regs_nm_map_index_mask;
    int locked;
    GdbBreakpoint * bp_arr;
    unsigned bp_cnt;
    unsigned bp_max;
} GdbThread;

typedef struct GdbRegister {
    unsigned regnum;
    char name[256];
    unsigned bits;
    int id;
} GdbRegister;

typedef struct MonitorCommand {
    const char * name;
    void (*func)(GdbClient *, const char *);
} MonitorCommand;

#define link_a2s(x) ((GdbServer *)((char *)(x) - offsetof(GdbServer, link_a2s)))
#define link_s2c(x) ((GdbClient *)((char *)(x) - offsetof(GdbClient, link_s2c)))
#define link_c2p(x) ((GdbProcess *)((char *)(x) - offsetof(GdbProcess, link_c2p)))
#define link_p2t(x) ((GdbThread *)((char *)(x) - offsetof(GdbThread, link_p2t)))
#define link_p2b(x) ((GdbBreakpoint *)((char *)(x) - offsetof(GdbBreakpoint, link_p2b)))

#define client2gdb(c)  ((GdbClient *)((char *)(c) - offsetof(GdbClient, client)))

static int ini_done = 0;
static LINK link_a2s;

static GdbProcess * add_process(GdbClient * c, Context * ctx) {
    GdbProcess * p = (GdbProcess *)loc_alloc_zero(sizeof(GdbProcess));
    assert(ctx->mem == ctx);
    p->client = c;
    p->pid = ++c->process_id_cnt;
    p->ctx = ctx;
    list_init(&p->link_p2t);
    list_init(&p->link_p2b);
    list_add_last(&p->link_c2p, &c->link_c2p);
    return p;
}

static GdbProcess * find_process_pid(GdbClient * c, unsigned pid) {
    LINK * l;
    for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
        GdbProcess * p = link_c2p(l);
        if (p->pid == pid) return p;
    }
    return NULL;
}

static GdbProcess * find_process_ctx(GdbClient * c, Context * ctx) {
    LINK * l;
    for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
        GdbProcess * p = link_c2p(l);
        if (p->ctx == ctx) return p;
    }
    return NULL;
}

static void add_thread(GdbClient * c, Context * ctx) {
    GdbThread * t = (GdbThread *)loc_alloc_zero(sizeof(GdbThread));
    t->process = find_process_ctx(c, context_get_group(ctx, CONTEXT_GROUP_PROCESS));
    t->tid = ++t->process->thread_id_cnt;
    t->ctx = ctx;
    list_add_last(&t->link_p2t, &t->process->link_p2t);
    if (c->stopped) {
        t->locked = 1;
        run_ctrl_ctx_lock(ctx);
        if (suspend_debug_context(ctx) < 0) {
            char * name = ctx->name;
            if (name == NULL) name = ctx->id;
            trace(LOG_ALWAYS, "GDB Server: cannot suspend context %s: %s", name, errno_to_str(errno));
        }
    }
}

static GdbThread * find_thread(GdbClient * c, unsigned pid, unsigned tid) {
    GdbProcess * p = find_process_pid(c, pid);
    if (p != NULL) {
        LINK * l;
        for (l = p->link_p2t.next; l != &p->link_p2t; l = l->next) {
            GdbThread * t = link_p2t(l);
            if (t->tid == tid) return t;
        }
    }
    return NULL;
}

static void free_thread(GdbThread * t) {
    if (t->process->client->stopped) {
        assert(t->locked);
        run_ctrl_ctx_unlock(t->ctx);
        t->locked = 0;
    }
    loc_free(t->regs_nm_map);
    list_remove(&t->link_p2t);
    loc_free(t);
}

static void free_breakpoint(GdbBreakpoint * b) {
    if (b->bp != NULL) destroy_eventpoint(b->bp);
    list_remove(&b->link_p2b);
    loc_free(b);
}

static void free_process(GdbProcess * p) {
    while (!list_is_empty(&p->link_p2t)) {
        assert(p->attached);
        free_thread(link_p2t(p->link_p2t.next));
    }
    while (!list_is_empty(&p->link_p2b)) {
        free_breakpoint(link_p2b(p->link_p2b.next));
    }
    list_remove(&p->link_c2p);
    loc_free(p);
}

static const char * get_regs(GdbClient * c) {
    if (strcmp(c->server->isa, "i386") == 0) return cpu_regs_gdb_i386;
    if (strcmp(c->server->isa, "i486") == 0) return cpu_regs_gdb_i386;
    if (strcmp(c->server->isa, "i586") == 0) return cpu_regs_gdb_i386;
    if (strcmp(c->server->isa, "i686") == 0) return cpu_regs_gdb_i386;
    if (strcmp(c->server->isa, "x86") == 0) return cpu_regs_gdb_i386;
    if (strcmp(c->server->isa, "ia32") == 0) return cpu_regs_gdb_i386;
    if (strcmp(c->server->isa, "x86_64") == 0) return cpu_regs_gdb_x86_64;
    if (strcmp(c->server->isa, "amd64") == 0) return cpu_regs_gdb_x86_64;
    if (strcmp(c->server->isa, "x64") == 0) return cpu_regs_gdb_x86_64;
    if (strcmp(c->server->isa, "arm") == 0) return cpu_regs_gdb_arm;
    if (strcmp(c->server->isa, "a32") == 0) return cpu_regs_gdb_arm;
    if (strcmp(c->server->isa, "arm64") == 0) return cpu_regs_gdb_a64;
    if (strcmp(c->server->isa, "aarch64") == 0) return cpu_regs_gdb_a64;
    if (strcmp(c->server->isa, "a64") == 0) return cpu_regs_gdb_a64;
    if (strcmp(c->server->isa, "ppc") == 0) return cpu_regs_gdb_powerpc;
    if (strcmp(c->server->isa, "ppc32") == 0) return cpu_regs_gdb_powerpc;
    if (strcmp(c->server->isa, "power32") == 0) return cpu_regs_gdb_powerpc;
    if (strcmp(c->server->isa, "powerpc") == 0) return cpu_regs_gdb_powerpc;
    if (strcmp(c->server->isa, "ppc64") == 0) return cpu_regs_gdb_ppc64;
    if (strcmp(c->server->isa, "power64") == 0) return cpu_regs_gdb_ppc64;
    if (strcmp(c->server->isa, "microblaze") == 0) return cpu_regs_gdb_microblaze;
    if (strcmp(c->server->isa, "microblaze64") == 0) return cpu_regs_gdb_microblaze64;
    if (strcmp(c->server->isa, "mb") == 0) return cpu_regs_gdb_microblaze;
    if (strcmp(c->server->isa, "mb64") == 0) return cpu_regs_gdb_microblaze64;
    if (strcmp(c->server->isa, "riscv32") == 0) return cpu_regs_gdb_riscv32;
    if (strcmp(c->server->isa, "riscv64") == 0) return cpu_regs_gdb_riscv64;
    if (strcmp(c->server->isa, "rv32") == 0) return cpu_regs_gdb_riscv32;
    if (strcmp(c->server->isa, "rv64") == 0) return cpu_regs_gdb_riscv64;
    set_fmt_errno(ERR_OTHER, "Unsupported ISA %s", c->server->isa);
    return NULL;
}

static int check_process_isa(GdbClient * c, Context * prs) {
    ContextISA isa;
    const char * regs = get_regs(c);
    if (regs != NULL) {
        memset(&isa, 0, sizeof(isa));
        if (context_get_isa(prs, 0, &isa) < 0) {
            trace(LOG_ALWAYS, "Cannot get process ISA: %s", errno_to_str(errno));
            return 0;
        }
        if (isa.def != NULL) {
            if (strcmp(isa.def, "386") == 0) return regs == cpu_regs_gdb_i386;
            if (strcmp(isa.def, "X86_64") == 0) return regs == cpu_regs_gdb_x86_64;
            if (strcmp(isa.def, "ARM") == 0) return regs == cpu_regs_gdb_arm;
            if (strcmp(isa.def, "Thumb") == 0) return regs == cpu_regs_gdb_arm;
            if (strcmp(isa.def, "ThumbEE") == 0) return regs == cpu_regs_gdb_arm;
            if (strcmp(isa.def, "Jazelle") == 0) return regs == cpu_regs_gdb_arm;
            if (strcmp(isa.def, "A64") == 0) return regs == cpu_regs_gdb_a64;
            if (strcmp(isa.def, "PPC") == 0) return regs == cpu_regs_gdb_powerpc;
            if (strcmp(isa.def, "PPC64") == 0) return regs == cpu_regs_gdb_ppc64;
            if (strcmp(isa.def, "MicroBlaze") == 0) return regs == cpu_regs_gdb_microblaze;
            if (strcmp(isa.def, "MicroBlaze64") == 0) return regs == cpu_regs_gdb_microblaze64;
            if (strcmp(isa.def, "Riscv32") == 0) return regs == cpu_regs_gdb_riscv32;
            if (strcmp(isa.def, "Riscv64") == 0) return regs == cpu_regs_gdb_riscv64;
        }
    }
    return 0;
}

static unsigned reg_name_hash(const char * name) {
    unsigned h = 5381;
    while (*name) h = ((h << 5) + h) + *name++;
    return h;
}

static RegisterDefinition * find_register(GdbThread * t, GdbRegister * r) {
    RegisterDefinition ** map = t->regs_nm_map;
    unsigned n = 0;

    if (r->id >= 0) {
        RegisterDefinition * def = get_reg_definitions(t->ctx);
        if (def == NULL) return NULL;
        while (def->name != NULL) {
            if (def->dwarf_id == r->id) return def;
            def++;
        }
        return NULL;
    }
    if (map == NULL) {
        unsigned map_len = 0;
        unsigned map_len_p2 = 1;
        RegisterDefinition * def = get_reg_definitions(t->ctx);
        if (def == NULL) return NULL;
        while (def->name != NULL) {
            map_len++;
            def++;
        }
        if (map_len == 0) return NULL;
        while (map_len_p2 < map_len * 3) map_len_p2 <<= 2;
        map = (RegisterDefinition **)loc_alloc_zero(sizeof(RegisterDefinition *) * map_len_p2);
        t->regs_nm_map_index_mask = map_len_p2 - 1;
        def = get_reg_definitions(t->ctx);
        while (def->name != NULL) {
            unsigned h = reg_name_hash(def->name) & t->regs_nm_map_index_mask;
            while (map[h] != NULL) h = (h + 1) & t->regs_nm_map_index_mask;
            map[h] = def;
            def++;
        }
        t->regs_nm_map = map;
    }
    n = reg_name_hash(r->name) & t->regs_nm_map_index_mask;
    while (map[n] != NULL) {
        if (strcmp(map[n]->name, r->name) == 0) return map[n];
        n = (n + 1) & t->regs_nm_map_index_mask;
    }
    return NULL;
}

static int open_server(const char * port) {
    int err = 0;
    int sock = -1;
    struct addrinfo hints;
    struct addrinfo * reslist = NULL;
    struct addrinfo * res = NULL;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    err = loc_getaddrinfo(NULL, port, &hints, &reslist);
    if (err) {
        set_gai_errno(err);
        return -1;
    }

    for (res = reslist; res != NULL; res = res->ai_next) {
        const int i = 1;
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) continue;

        if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&i, sizeof(i)) < 0) err = errno;
        if (!err && bind(sock, res->ai_addr, res->ai_addrlen)) err = errno;
        if (!err && listen(sock, 4)) err = errno;
        if (!err) break;

        closesocket(sock);
        sock = -1;
    }

    freeaddrinfo(reslist);
    return sock;
}

static void dispose_server(GdbServer * s) {
    list_remove(&s->link_a2s);
    closesocket(s->req.u.acc.sock);
    s->req.u.acc.sock = -1;
    s->disposed = 1;
    if (list_is_empty(&s->link_s2c)) {
        loc_free(s);
    }
}

static void lock_threads(GdbClient * c) {
    LINK * l;
    assert(!c->closed);
    if (c->stopped) return;
    for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
        LINK * m;
        GdbProcess * p = link_c2p(l);
        for (m = p->link_p2t.next; m != &p->link_p2t; m = m->next) {
            GdbThread * t = link_p2t(m);
            Context * ctx = t->ctx;
            assert(!t->locked);
            assert(!t->ctx->exited);
            run_ctrl_ctx_lock(ctx);
            if (suspend_debug_context(ctx) < 0) {
                char * name = ctx->name;
                if (name == NULL) name = ctx->id;
                trace(LOG_ALWAYS, "GDB Server: cannot suspend context %s: %s", name, errno_to_str(errno));
            }
            t->locked = 1;
        }
    }
    c->stopped = 1;
}

static void unlock_threads(GdbClient * c) {
    LINK * l;
    if (!c->stopped) return;
    for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
        LINK * m;
        GdbProcess * p = link_c2p(l);
        for (m = p->link_p2t.next; m != &p->link_p2t; m = m->next) {
            GdbThread * t = link_p2t(m);
            Context * ctx = t->ctx;
            assert(t->locked);
            assert(!t->ctx->exited);
            run_ctrl_ctx_unlock(ctx);
            t->locked = 0;
            t->bp_cnt = 0;
        }
    }
    c->stopped = 0;
}

static void attach_process(GdbProcess * p) {
    GdbClient * c = p->client;
    LINK * l;
    if (p->attached) return;
    p->attached = 1;
    for (l = context_root.next; l != &context_root; l = l->next) {
        Context * ctx = ctxl2ctxp(l);
        if (!ctx->exited && context_has_state(ctx) && context_get_group(ctx, CONTEXT_GROUP_PROCESS) == p->ctx) {
            add_thread(c, ctx);
        }
    }
}

static void detach_process(GdbProcess * p) {
    if (!p->attached) return;
    while (!list_is_empty(&p->link_p2t)) {
        free_thread(link_p2t(p->link_p2t.next));
    }
    p->attached = 0;
}

static int is_all_intercepted(GdbClient * c) {
    LINK * l, * m;
    for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
        GdbProcess * p = link_c2p(l);
        for (m = p->link_p2t.next; m != &p->link_p2t; m = m->next) {
            GdbThread * t = link_p2t(m);
            assert(p->attached);
            assert(!t->ctx->exited);
            assert(context_has_state(t->ctx));
            if (!is_intercepted(t->ctx)) return 0;
        }
    }
    return 1;
}

static void start_client(void * args) {
    GdbClient * c = (GdbClient *)args;

    if (c->start_timer > 10 || (c->stopped && is_all_intercepted(c))) {
        if (c->stopped && !is_all_intercepted(c)) {
            LINK * l;
            c->cur_g_pid = 0;
            c->cur_g_tid = 0;
            for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
                GdbProcess * p = link_c2p(l);
                detach_process(p);
            }
        }
        c->req.u.sio.rval = 0;
        async_req_post(&c->req);
        return;
    }

    if (!c->stopped) {
        LINK * l;
        /* Select initial debug target */
        for (l = context_root.next; l != &context_root; l = l->next) {
            Context * ctx = ctxl2ctxp(l);
            if (!ctx->exited && context_has_state(ctx)) {
                Context * prs = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
                GdbProcess * p = find_process_ctx(c, prs);
                if (p != NULL) {
                    attach_process(p);
                    lock_threads(c);
                    break;
                }
            }
        }
    }

    post_event_with_delay(start_client, args, 500000);
    c->start_timer++;
}

static void close_client(GdbClient * c) {
    if (!c->closed) {
        c->closed = 1;
        unlock_threads(c);
        closesocket(c->req.u.sio.sock);
        notify_client_disconnected(&c->client);
    }
}

static void dispose_client(ClientConnection * cc) {
    GdbClient * c = client2gdb(cc);
    GdbServer * s = c->server;

    assert(c->closed);
    while (!list_is_empty(&c->link_c2p)) {
        free_process(link_c2p(c->link_c2p.next));
    }
    list_remove(&c->link_s2c);
    loc_free(c->cmd_buf);
    loc_free(c->buf);
    loc_free(c);

    if (s->disposed && list_is_empty(&s->link_s2c)) {
        loc_free(s);
    }
}

static char hex_digit(unsigned d) {
    assert(d < 0x10);
    if (d < 10) return (char)('0' + d);
    /* Note: don't use capital 'E' - GDB can take it as error reply */
    return (char)('a' + d - 10);
}

static void add_res_ch_no_esc(GdbClient * c, char ch) {
    if (c->res_pos >= c->res_max) {
        c->res_max = c->res_max == 0 ? 0x1000 : c->res_max * 2;
        c->res_buf = (char *)loc_realloc(c->res_buf, c->res_max);
    }
    c->res_buf[c->res_pos++] = ch;
}

static void add_res_ch(GdbClient * c, char ch) {
    switch (ch) {
    case '}':
    case '$':
    case '#':
    case '*':
        add_res_ch_no_esc(c, '}');
        ch ^= 0x20;
        break;
    }
    add_res_ch_no_esc(c, ch);
}

static void add_res_str(GdbClient * c, const char * s) {
    while (*s != 0) add_res_ch(c, *s++);
}

static void add_res_hex(GdbClient * c, uint64_t n) {
    char s[17];
    unsigned i = sizeof(s);
    s[--i] = 0;
    do {
        unsigned d = n & 0xf;
        s[--i] = hex_digit(d);
        n = n >> 4;
    }
    while (n != 0 && i > 0);
    add_res_str(c, s + i);
}

static void add_res_hex8(GdbClient * c, unsigned n) {
    char s[3];
    unsigned i = sizeof(s);
    s[--i] = 0;
    do {
        unsigned d = n & 0xf;
        s[--i] = hex_digit(d);
        n = n >> 4;
    }
    while (i > 0);
    add_res_str(c, s);
}

static void add_res_hex8_str(GdbClient * c, const char * s) {
    while (*s) add_res_hex8(c, *s++);
}

static void add_res_ptid(GdbClient * c, unsigned pid, unsigned tid) {
    if (c->multiprocess) {
        add_res_ch(c, 'p');
        add_res_hex(c, pid);
        add_res_ch(c, '.');
    }
    add_res_hex(c, tid);
}

static int add_res_target_info(GdbClient * c) {
    const char * regs = get_regs(c);
    if (regs == NULL) return -1;
    add_res_str(c, "l<?xml version=\"1.0\"?>\n");
    add_res_str(c, "<!DOCTYPE target SYSTEM \"gdb-target.dtd\">\n");
    add_res_str(c, "<target version=\"1.0\">\n");
    add_res_str(c, regs);
    add_res_str(c, "</target>\n");
    return 0;
}

static int add_res_exec_file(GdbClient * c, unsigned pid) {
    GdbProcess * p = find_process_pid(c, pid);
    if (p != NULL) {
        MemoryMap * client_map;
        MemoryMap * target_map;
        unsigned i;
        if (memory_map_get(p->ctx, &client_map, &target_map) < 0) return -1;
        for (i = 0; i < target_map->region_cnt; i++) {
            MemoryRegion * r = target_map->regions + i;
            if (r->file_name != NULL) {
                add_res_ch(c, 'l');
                add_res_str(c, r->file_name);
                return 0;
            }
        }
    }
    add_res_str(c, "E01");
    return 0;
}

static void add_res_reg_value(GdbClient * c, GdbThread * t, GdbRegister * r) {
    RegisterDefinition * def = NULL;
    unsigned size = (r->bits + 7) / 8;
    void * buf = tmp_alloc_zero(size);
    unsigned i = 0;
    if (t != NULL) def = find_register(t, r);
    if (def != NULL) {
        unsigned rd = def->size < size ? def->size : size;
        if (context_read_reg(t->ctx, def, 0, rd, buf) >= 0) {
            /* Register bytes are transmitted in the target byte order */
            if (def->big_endian) swap_bytes(buf, rd);
            if (t->ctx->big_endian) swap_bytes(buf, size);
        }
        else {
            def = NULL;
        }
    }
    while (i < size) {
        if (def == NULL) {
            add_res_str(c, "xx");
        }
        else {
            add_res_hex8(c, ((uint8_t *)buf)[i]);
        }
        i++;
    }
}

static void add_res_stop_reason(GdbClient * c) {
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    if (t != NULL) {
        unsigned i;
        add_res_str(c, "T05");
        add_res_str(c, "thread:");
        add_res_ptid(c, c->cur_g_pid, c->cur_g_tid);
        add_res_ch(c, ';');
        for (i = 0; i < t->bp_cnt; i++) {
            GdbBreakpoint * bp = t->bp_arr + i;
            switch (bp->type) {
            case 0:
                if (c->swbreak) add_res_str(c, "swbreak:;");
                break;
            case 1:
                if (c->hwbreak) add_res_str(c, "hwbreak:;");
                break;
            case 2:
            case 3:
            case 4:
                if (bp->type == 3) add_res_ch(c, 'r');
                if (bp->type == 4) add_res_ch(c, 'a');
                add_res_str(c, "watch:");
                add_res_hex(c, bp->addr);
                add_res_ch(c, ';');
                break;
            }
        }
    }
    else {
        add_res_str(c, "W00");
    }
}

static int send_res(GdbClient * c) {
    unsigned i;
    unsigned char sum = 0;
    assert(c->res_pos > 0);
    assert(c->res_buf[0] == '$');
    for (i = 1; i < c->res_pos; i++) {
        sum += (unsigned char)c->res_buf[i];
    }
    add_res_ch_no_esc(c, '#');
    add_res_hex8(c, sum);
#if DEBUG_RSP
    printf("GDB <- %.*s\n", c->res_pos, c->res_buf);
#endif
    return send(c->req.u.sio.sock, c->res_buf, c->res_pos, 0);
}

static char * get_cmd_word(GdbClient * c, char ** p) {
    char * s = *p;
    char * e = s;
    char * w = NULL;
    while (e < c->cmd_buf + c->cmd_end) {
        if (*e == ':') break;
        if (*e == ';') break;
        if (*e == ',') break;
        e++;
    }
    w = (char *)tmp_alloc_zero(e - s + 1);
    memcpy(w, s, e - s);
    *p = e;
    return w;
}

static uint8_t get_cmd_uint8(GdbClient * c, char ** p) {
    char * s = *p;
    uint8_t n = 0;
    while (s < c->cmd_buf + c->cmd_end && s < *p + 2) {
        char ch = *s;
        if (ch >= '0' && ch <= '9') n = (n << 4) + (ch - '0');
        else if (ch >= 'A' && ch <= 'F') n = (n << 4) + (ch - 'A' + 10);
        else if (ch >= 'a' && ch <= 'f') n = (n << 4) + (ch - 'a' + 10);
        else break;
        s++;
    }
    *p = s;
    return n;
}

static unsigned get_cmd_uint(GdbClient * c, char ** p) {
    char * s = *p;
    unsigned n = 0;
    while (s < c->cmd_buf + c->cmd_end) {
        char ch = *s;
        if (ch >= '0' && ch <= '9') n = (n << 4) + (ch - '0');
        else if (ch >= 'A' && ch <= 'F') n = (n << 4) + (ch - 'A' + 10);
        else if (ch >= 'a' && ch <= 'f') n = (n << 4) + (ch - 'a' + 10);
        else break;
        s++;
    }
    *p = s;
    return n;
}

static uint64_t get_cmd_uint64(GdbClient * c, char ** p) {
    char * s = *p;
    uint64_t n = 0;
    while (s < c->cmd_buf + c->cmd_end && s < *p + 16) {
        char ch = *s;
        if (ch >= '0' && ch <= '9') n = (n << 4) + (ch - '0');
        else if (ch >= 'A' && ch <= 'F') n = (n << 4) + (ch - 'A' + 10);
        else if (ch >= 'a' && ch <= 'f') n = (n << 4) + (ch - 'a' + 10);
        else break;
        s++;
    }
    *p = s;
    return n;
}

static void get_cmd_ptid(GdbClient * c, char ** pp, unsigned * res_pid, unsigned * res_tid) {
    char * s = *pp;
    unsigned pid = 0;
    unsigned tid = 0;
    int neg_pid = 0;
    int neg_tid = 0;
    if (*s == 'p') {
        s++;
        if (*s == '-') {
            neg_pid = 1;
            s++;
        }
        pid = get_cmd_uint(c, &s);
    }
    if (*s == '.') s++;
    if (*s == '-') {
        neg_tid = 1;
        s++;
    }
    tid = get_cmd_uint(c, &s);
    if (neg_pid) {
        pid = ID_ANY;
    }
    else if (pid == 0) {
        LINK * l;
        pid = 0;
        for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
            GdbProcess * p = link_c2p(l);
            if (p->attached) {
                pid = p->pid;
                break;
            }
        }
    }
    if (neg_tid || pid == ID_ANY) {
        tid = ID_ANY;
    }
    else if (tid == 0) {
        GdbProcess * p = find_process_pid(c, pid);
        tid = 0;
        if (p != NULL && !list_is_empty(&p->link_p2t)) {
            tid = link_p2t(p->link_p2t.next)->tid;
        }
    }
    *pp = s;
    *res_pid = pid;
    *res_tid = tid;
}

static void get_xfer_range(GdbClient * c, char ** p) {
    c->xfer_range_offs = get_cmd_uint(c, p);
    if (**p != ',') return;
    (*p)++;
    c->xfer_range_size = get_cmd_uint(c, p);
}

static int read_reg_attributes(const char * p, unsigned n, GdbRegister * r) {
    const char * p0 = p;
    memset(r, 0, sizeof(GdbRegister));
    r->regnum = n;
    r->id = -1;
    while (*p == ' ') p++;
    if (strncmp(p, "<reg ", 5) != 0) return 0;
    p += 5;
    while (*p) {
        if (*p == '\n') break;
        if (p[0] == '=' && (p[1] == '"' || p[1] == '\'')) {
            char q = p[1];
            const char * n0 = p;
            const char * n1 = p;
            const char * v0 = p + 2;
            const char * v1 = p + 2;
            while (*v1 != 0 && *v1 != q) v1++;
            while (n0 > p0 && *(n0 - 1) != ' ') n0--;
            if (n1 - n0 == 4 && strncmp(n0, "name", 4) == 0) {
                size_t l = v1 - v0 + 1;
                if (l > sizeof(r->name)) l = sizeof(r->name);
                strlcpy(r->name, v0, l);
            }
            if (n1 - n0 == 7 && strncmp(n0, "bitsize", 7) == 0) {
                r->bits = (unsigned)atoi(v0);
            }
            if (n1 - n0 == 6 && strncmp(n0, "regnum", 6) == 0) {
                r->regnum = (unsigned)atoi(v0);
            }
            if (n1 - n0 == 2 && strncmp(n0, "id", 2) == 0) {
                r->id = atoi(v0);
            }
            if (*v1 != q) break;
            p = v1;
        }
        p++;
    }
    return r->name[0] && r->bits > 0;
}

static void breakpoint_cb(Context * ctx, void * args) {
    GdbBreakpoint * b = (GdbBreakpoint *)args;
    GdbProcess * p = b->process;
    LINK * l;
    for (l = p->link_p2t.next; l != &p->link_p2t; l = l->next) {
        GdbThread * t = link_p2t(l);
        if (t->ctx == ctx) {
            if (t->bp_cnt >= t->bp_max) {
                t->bp_max += 8;
                t->bp_arr = (GdbBreakpoint *)loc_realloc(t->bp_arr, t->bp_max * sizeof(GdbBreakpoint));
            }
            t->bp_arr[t->bp_cnt++] = *b;
            ctx->pending_intercept = 1;
        }
    }
}

static void monitor_ps(GdbClient * c, const char * args) {
    LINK * l;
    unsigned cnt = 0;
    for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
        char * m = NULL;
        GdbProcess * p = link_c2p(l);
        if (context_has_state(p->ctx)) {
            const char * state = get_context_state_name(p->ctx);
            m = tmp_printf("%u: %s (%s)\n", (unsigned)p->pid, p->ctx->name ? p->ctx->name : p->ctx->id, state);
        }
        else {
            m = tmp_printf("%u: %s\n", (unsigned)p->pid, p->ctx->name ? p->ctx->name : p->ctx->id);
        }
        add_res_hex8_str(c, m);
        cnt++;
    }
    if (cnt == 0) add_res_hex8_str(c, "No debug targets found\n");
}

static void monitor_info(GdbClient * c, const char * args) {
    unsigned pid = 0;
    char * s = (char *)args;
    while (*s == ' ') s++;
    pid = (unsigned)strtol(s, &s, 10);
    while (*s == ' ') s++;
    if (*s == 0 && pid != 0) {
        GdbProcess * prs = find_process_pid(c, pid);
        if (prs != NULL) {
            Context * ctx = prs->ctx;
            add_res_hex8_str(c, tmp_printf("Target %u properties:\n", pid));
            for (;;) {
                add_res_hex8_str(c, tmp_printf(" ID        : \"%s\"\n", ctx->id));
                if (ctx->parent != NULL) add_res_hex8_str(c, tmp_printf(" ParentID  : \"%s\"\n", ctx->parent->id));
                if (ctx->name != NULL) add_res_hex8_str(c, tmp_printf(" Name      : \"%s\"\n", ctx->name));
                add_res_hex8_str(c, tmp_printf(" WordSize  : %u\n", context_word_size(ctx)));
                add_res_hex8_str(c, tmp_printf(" BigEndian : %d\n", ctx->big_endian));
#if ENABLE_ContextExtraProperties
                {
                    /* Back-end context properties */
                    int cnt = 0;
                    const char ** names = NULL;
                    const char ** values = NULL;
                    if (context_get_extra_properties(ctx, &names, &values, &cnt) == 0) {
                        while (cnt > 0) {
                            if (*values != NULL) add_res_hex8_str(c, tmp_printf(" %-10s: %s\n", *names, *values));
                            names++;
                            values++;
                            cnt--;
                        }
                    }
                }
#endif
                if (ctx->parent == NULL) break;
                add_res_hex8_str(c, "Parent properties:\n");
                ctx = ctx->parent;
            }
            return;
        }
    }
    add_res_hex8_str(c, "Invalid target ID.\n");
    add_res_hex8_str(c, "Available targets:\n");
    monitor_ps(c, "");
}

static void monitor_help(GdbClient * c, const char * args) {
    add_res_hex8_str(c, "Usage: monitor <command> [<arguments>]\n");
    add_res_hex8_str(c, "Commands:\n");
    add_res_hex8_str(c, " ps - list of debug targets\n");
    add_res_hex8_str(c, " info <target ID> - properties of a target\n");
    add_res_hex8_str(c, " help - print this text\n");
}

static MonitorCommand mon_cmds[] = {
    { "ps", monitor_ps },
    { "info", monitor_info },
    { "help", monitor_help },
    { NULL }
};

static int handle_g_command(GdbClient * c) {
    /* Read general registers */
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    const char * regs = get_regs(c);
    const char * p = regs;
    const char * q = regs;
    unsigned regnum = 0;
    if (p == NULL) return -1;
    while (*p) {
        if (*p++ == '\n') {
            GdbRegister r;
            if (read_reg_attributes(q, regnum, &r)) {
                add_res_reg_value(c, t, &r);
                regnum = r.regnum + 1;
            }
            q = p;
        }
    }
    return 0;
}

static int handle_m_command(GdbClient * c) {
    /* Read memory */
    char * s = c->cmd_buf + 2;
    ContextAddress addr = (ContextAddress)get_cmd_uint64(c, &s);
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    void * buf = NULL;
    size_t size = 0;
    if (*s == ',') {
        s++;
        size = (size_t)get_cmd_uint(c, &s);
    }
    buf = tmp_alloc_zero(size);
    if (t == NULL || context_read_mem(t->ctx, addr, buf, size) < 0) {
        add_res_str(c, "E01");
    }
    else {
        unsigned i = 0;
        while (i < size) {
            add_res_hex8(c, ((uint8_t *)buf)[i++]);
        }
    }
    return 0;
}

static int handle_M_command(GdbClient * c) {
    /* Write memory */
    char * s = c->cmd_buf + 2;
    ContextAddress addr = (ContextAddress)get_cmd_uint64(c, &s);
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    void * buf = NULL;
    size_t size = 0;
    if (*s == ',') {
        s++;
        size = (size_t)get_cmd_uint(c, &s);
    }
    buf = tmp_alloc_zero(size);
    if (*s == ':') {
        unsigned i = 0;
        s++;
        while (i < size) {
            ((uint8_t *)buf)[i++] = get_cmd_uint8(c, &s);
        }
    }
    if (t == NULL || context_write_mem(t->ctx, addr, buf, size) < 0) {
        add_res_str(c, "E01");
    }
    else {
        add_res_str(c, "OK");
    }
    return 0;
}

static int handle_p_command(GdbClient * c) {
    /* Read register */
    char * s = c->cmd_buf + 2;
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    unsigned reg = get_cmd_uint(c, &s);
    const char * regs = get_regs(c);
    const char * p = regs;
    const char * q = regs;
    unsigned regnum = 0;
    if (p == NULL) return -1;
    while (*p) {
        if (*p++ == '\n') {
            GdbRegister r;
            if (read_reg_attributes(q, regnum, &r)) {
                if (r.regnum == reg) {
                    add_res_reg_value(c, t, &r);
                    return 0;
                }
                regnum = r.regnum + 1;
            }
            q = p;
        }
    }
    add_res_str(c, "E01");
    return 0;
}

static int handle_P_command(GdbClient * c) {
    /* Write register */
    char * s = c->cmd_buf + 2;
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    unsigned reg = get_cmd_uint(c, &s);
    const char * regs = get_regs(c);
    const char * p = regs;
    const char * q = regs;
    unsigned regnum = 0;
    if (p == NULL) return -1;
    while (*p) {
        if (*p++ == '\n') {
            GdbRegister r;
            if (read_reg_attributes(q, regnum, &r)) {
                if (r.regnum == reg) {
                    RegisterDefinition * def = NULL;
                    unsigned size = (r.bits + 7) / 8;
                    void * buf = tmp_alloc_zero(size);
                    if (*s++ == '=') {
                        unsigned i = 0;
                        while (i < size) ((uint8_t *)buf)[i++] = get_cmd_uint8(c, &s);
                    }
                    if (t != NULL) def = find_register(t, &r);
                    if (def != NULL && context_write_reg(t->ctx, def, 0, def->size < size ? def->size : size, buf) == 0) {
                        add_res_str(c, "OK");
                        return 0;
                    }
                    break;
                }
                regnum = r.regnum + 1;
            }
            q = p;
        }
    }
    add_res_str(c, "E01");
    return 0;
}

static int handle_q_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    char * w = get_cmd_word(c, &s);
    if (strcmp(w, "Supported") == 0) {
        if (*s++ == ':') {
            while (s < c->cmd_buf + c->cmd_end) {
                char name[256];
                char value[256];
                unsigned i = 0;
                while (s < c->cmd_buf + c->cmd_end) {
                    if (*s == ';' || *s == '+' || *s == '-' || *s == '=') break;
                    if (i < sizeof(name) - 1) name[i++] = *s;
                    s++;
                }
                name[i] = 0;
                if (*s == '+') {
                    if (strcmp(name, "multiprocess") == 0) c->multiprocess = 1;
                    if (strcmp(name, "swbreak") == 0) c->swbreak = 1;
                    if (strcmp(name, "hwbreak") == 0) c->hwbreak = 1;
                    s++;
                }
                else if (*s == '-') {
                    s++;
                }
                else if (*s == '=') {
                    s++;
                    i = 0;
                    while (s < c->cmd_buf + c->cmd_end) {
                        if (*s == ';') break;
                        if (i < sizeof(value) - 1) value[i++] = *s;
                        s++;
                    }
                    value[i] = 0;
                }
                if (*s == ';') s++;
            }
        }
        add_res_str(c, "PacketSize=4000");
        add_res_str(c, ";QStartNoAckMode+");
        add_res_str(c, ";qXfer:features:read+");
        add_res_str(c, ";qXfer:exec-file:read+");
        if (c->multiprocess) add_res_str(c, ";multiprocess+");
        if (c->swbreak) add_res_str(c, ";swbreak+");
        if (c->hwbreak) add_res_str(c, ";hwbreak+");
#if 0
        add_res_str(c, ";QNonStop+;QAgent+");
        add_res_str(c, ";QPassSignals+;QProgramSignals+");
        add_res_str(c, ";ConditionalBreakpoints+;BreakpointCommands+");
        add_res_str(c, ";qXfer:osdata:read+;qXfer:threads:read+");
        add_res_str(c, ";qXfer:libraries-svr4:read+");
        add_res_str(c, ";qXfer:auxv:read+");
        add_res_str(c, ";qXfer:spu:read+;qXfer:spu:write+");
        add_res_str(c, ";qXfer:siginfo:read+;qXfer:siginfo:write+");
#endif
        return 0;
    }
    if (strcmp(w, "Attached") == 0) {
        add_res_str(c, "1");
        return 0;
    }
    if (strcmp(w, "TStatus") == 0) {
        add_res_str(c, "T0");
        return 0;
    }
    if (strcmp(w, "C") == 0) {
        add_res_str(c, "QC");
        add_res_ptid(c, c->cur_g_pid, c->cur_g_tid);
        return 0;
    }
    if (strcmp(w, "Xfer") == 0 && *s++ == ':') {
        w = get_cmd_word(c, &s);
        if (strcmp(w, "features") == 0 && *s++ == ':') {
            w = get_cmd_word(c, &s);
            if (strcmp(w, "read") == 0 && *s++ == ':') {
                w = get_cmd_word(c, &s);
                if (strcmp(w, "target.xml") == 0) {
                    if (add_res_target_info(c) < 0) return -1;
                    if (*s++ == ':') get_xfer_range(c, &s);
                    return 0;
                }
            }
        }
        if (strcmp(w, "exec-file") == 0 && *s++ == ':') {
            w = get_cmd_word(c, &s);
            if (strcmp(w, "read") == 0 && *s++ == ':') {
                int pid = get_cmd_uint(c, &s);
                if (pid == 0) pid = c->cur_g_pid;
                if (add_res_exec_file(c, pid) < 0) return -1;
                if (*s++ == ':') get_xfer_range(c, &s);
                return 0;
            }
        }
        return 0;
    }
    if (strcmp(w, "fThreadInfo") == 0) {
        LINK * l;
        unsigned cnt = 0;
        for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
            GdbProcess * p = link_c2p(l);
            LINK * m;
            for (m = p->link_p2t.next; m != &p->link_p2t; m = m->next) {
                GdbThread * t = link_p2t(m);
                if (cnt == 0) add_res_ch(c, 'm');
                else add_res_ch(c, ',');
                add_res_ptid(c, p->pid, t->tid);
                cnt++;
            }
        }
        if (cnt == 0) add_res_ch(c, 'l');
        return 0;
    }
    if (strcmp(w, "sThreadInfo") == 0) {
        add_res_ch(c, 'l');
        return 0;
    }
    if (strcmp(w, "ThreadExtraInfo") == 0) {
        const char * m = NULL;
        if (*s++ == ',') {
            unsigned pid = 0;
            unsigned tid = 0;
            GdbThread * t = NULL;
            get_cmd_ptid(c, &s, &pid, &tid);
            t = find_thread(c, pid, tid);
            if (t != NULL) {
                Context * ctx = t->ctx;
                const char * state = get_context_state_name(ctx);
                m = ctx->name;
                if (m == NULL) m = ctx->id;
                if (state != NULL && *state) {
                    m = tmp_strdup2(m, ": ");
                    m = tmp_strdup2(m, state);
                }
            }
        }
        if (m == NULL) m = "Invalid ID";
        add_res_hex8_str(c, m);
        return 0;
    }
    if (strcmp(w, "Rcmd") == 0) {
        if (*s++ == ',') {
            unsigned i = 0;
            unsigned max = (c->cmd_buf + c->cmd_end - s) / 2 + 2;
            char * cmd = (char *)tmp_alloc_zero(max);
            MonitorCommand * mon_cmd = NULL;
            unsigned mon_cnt = 0;
            unsigned cmd_pos = 0;
            while (i < max - 1) {
                char ch = get_cmd_uint8(c, &s);
                if (ch == 0) break;
                cmd[i++] = ch;
            }
            for (i = 0;; i++) {
                unsigned j;
                MonitorCommand * m = mon_cmds + i;
                if (m->name == NULL) break;
                for (j = 0;; j++) {
                    if (cmd[j] != m->name[j] || m->name[j] == 0) {
                        if (j > 0 && (cmd[j] == ' ' || cmd[j] == 0)) {
                            mon_cmd = m;
                            cmd_pos = j;
                            mon_cnt++;
                        }
                        break;
                    }
                }
            }
            if (mon_cnt > 1) {
                add_res_hex8_str(c, "Ambiguous command.\n");
            }
            else if (mon_cmd == NULL) {
                add_res_hex8_str(c, "Invalid command.\n");
                monitor_help(c, "");
            }
            else {
                while (cmd[cmd_pos] == ' ') cmd_pos++;
                mon_cmd->func(c, cmd + cmd_pos);
            }
            return 0;
        }
        add_res_str(c, "E02");
    }
    return 0;
}

static int handle_Q_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    char * w = get_cmd_word(c, &s);
    if (strcmp(w, "StartNoAckMode") == 0) {
        add_res_str(c, "OK");
        c->no_ack_mode = 1;
        return 0;
    }
    return 0;
}

static int handle_H_command(GdbClient * c) {
    if (c->cmd_end > 2) {
        char * s = c->cmd_buf + 3;
        if (c->cmd_buf[2] == 'c') {
            get_cmd_ptid(c, &s, &c->cur_c_pid, &c->cur_c_tid);
        }
        else {
            get_cmd_ptid(c, &s, &c->cur_g_pid, &c->cur_g_tid);
        }
    }
    add_res_str(c, "OK");
    return 0;
}

static int handle_qm_command(GdbClient * c) {
    GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
    if (t != NULL) {
        if (is_intercepted(t->ctx)) {
            add_res_stop_reason(c);
        }
        else {
            suspend_debug_context(t->ctx);
            c->waiting = 1;
        }
        return 0;
    }
    add_res_str(c, "W00");
    return 0;
}

static int handle_v_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    char * w = get_cmd_word(c, &s);
    if (strcmp(w, "Attach") == 0) {
        if (*s++ == ';') {
            unsigned pid = get_cmd_uint(c, &s);
            GdbProcess * p = find_process_pid(c, pid);
            if (p != NULL) {
                if (!p->attached) attach_process(p);
                if (list_is_empty(&p->link_p2t)) {
                    add_res_str(c, "N");
                }
                else {
                    GdbThread * t = link_p2t(p->link_p2t.next);
                    c->cur_g_pid = p->pid;
                    c->cur_g_tid = t->tid;
                    lock_threads(c);
                    if (is_all_intercepted(c)) {
                        add_res_stop_reason(c);
                    }
                    else {
                        c->waiting = 1;
                    }
                }
                return 0;
            }
        }
        add_res_str(c, "E01");
        return 0;
    }
    if (strcmp(w, "Cont?") == 0) {
        add_res_str(c, "vCont;c;C;s;S;t;r");
        return 0;
    }
    if (strcmp(w, "Cont") == 0) {
        while (*s++ == ';') {
            char mode = *s++;
            unsigned sig = 0;
            ContextAddress range_fr = 0;
            ContextAddress range_to = 0;
            switch (mode) {
            case 'C':
            case 'S':
                sig = get_cmd_uint8(c, &s);
                break;
            case 'r':
                range_fr = (ContextAddress)get_cmd_uint64(c, &s);
                if (*s == ',') {
                    s++;
                    range_to = (ContextAddress)get_cmd_uint64(c, &s);
                }
                break;
            }
            if (*s == ':') {
                s++;
                get_cmd_ptid(c, &s, &c->cur_g_pid, &c->cur_g_tid);
            }
            if (c->cur_g_tid == ID_ANY) {
                GdbProcess * p = find_process_pid(c, c->cur_g_pid);
                switch (mode) {
                case 'c':
                    continue_debug_context(p->ctx, NULL, RM_RESUME, 1, 0, 0);
                    break;
                case 't':
                    suspend_debug_context(p->ctx);
                    break;
                }
            }
            else {
                GdbThread * t = find_thread(c, c->cur_g_pid, c->cur_g_tid);
                if (t != NULL) {
                    sigset_clear(&t->ctx->pending_signals);
                    switch (mode) {
                    case 'c':
                        continue_debug_context(t->ctx, NULL, RM_RESUME, 1, 0, 0);
                        break;
                    case 'C':
                        sigset_set(&t->ctx->pending_signals, sig, 1);
                        continue_debug_context(t->ctx, NULL, RM_RESUME, 1, 0, 0);
                        break;
                    case 's':
                        continue_debug_context(t->ctx, NULL, RM_STEP_INTO, 1, 0, 0);
                        break;
                    case 'S':
                        sigset_set(&t->ctx->pending_signals, sig, 1);
                        continue_debug_context(t->ctx, NULL, RM_STEP_INTO, 1, 0, 0);
                        break;
                    case 'r':
                        continue_debug_context(t->ctx, NULL, RM_STEP_INTO_RANGE, 1, range_fr, range_to);
                        break;
                    case 't':
                        suspend_debug_context(t->ctx);
                        break;
                    }
                }
            }
        }
        if (list_is_empty(&c->link_c2p)) {
            add_res_str(c, "N");
        }
        else {
            unlock_threads(c);
            c->waiting = 1;
        }
        return 0;
    }
    return 0;
}

static int handle_T_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    unsigned pid = 0;
    unsigned tid = 0;
    GdbThread * t = NULL;
    get_cmd_ptid(c, &s, &pid, &tid);
    t = find_thread(c, pid, tid);
    if (t != NULL) {
        add_res_str(c, "OK");
    }
    else {
        add_res_str(c, "E01");
    }
    return 0;
}

static int handle_D_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    if (*s++ == ';') {
        unsigned pid = get_cmd_uint(c, &s);
        GdbProcess * p = find_process_pid(c, pid);
        if (p != NULL) {
            if (c->cur_g_pid == p->pid) {
                c->cur_g_pid = 0;
                c->cur_g_tid = 0;
            }
            /* According to the GDB manual: Detaching the process continues its execution. */
            if (!p->ctx->exited) continue_debug_context(p->ctx, NULL, RM_RESUME, 1, 0, 0);
            detach_process(p);
            add_res_str(c, "OK");
            return 0;
        }
    }
    add_res_str(c, "E01");
    return 0;
}

static int handle_Z_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    unsigned type = get_cmd_uint(c, &s);
    if (*s++ == ',') {
        uint64_t addr = get_cmd_uint64(c, &s);
        if (*s++ == ',') {
            unsigned kind = get_cmd_uint(c, &s);
            GdbProcess * p = find_process_pid(c, c->cur_g_pid);
            unsigned size = kind;
            unsigned mode = 0;

            if (type < 2) {
                size = 1;
                mode = CTX_BP_ACCESS_INSTRUCTION;
            }
            else if (type == 2) {
                mode = CTX_BP_ACCESS_DATA_WRITE;
            }
            else if (type == 3) {
                mode = CTX_BP_ACCESS_DATA_READ;
            }
            else if (type == 4) {
                mode = CTX_BP_ACCESS_DATA_READ | CTX_BP_ACCESS_DATA_WRITE;
            }
            else {
                /* not supported */
                return 0;
            }

            if (p != NULL) {
                GdbBreakpoint * b = (GdbBreakpoint *)loc_alloc_zero(sizeof(GdbBreakpoint));
                static const char * attr_list[] = {
                    BREAKPOINT_ENABLED,
                    BREAKPOINT_ACCESSMODE,
                    BREAKPOINT_CONTEXTIDS,
                    BREAKPOINT_LOCATION,
                    BREAKPOINT_SIZE,
                    BREAKPOINT_SERVICE
                };
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
                        json_write_long(out, mode);
                        break;
                    case 2:
                        write_stream(out, '[');
                        json_write_string(out, p->ctx->id);
                        write_stream(out, ']');
                        break;
                    case 3:
                        snprintf(str, sizeof(str), "%#" PRIx64, (uint64_t)addr);
                        json_write_string(out, str);
                        break;
                    case 4:
                        json_write_long(out, size);
                        break;
                    case 5:
                        json_write_string(out, "GDB-RSP");
                        break;
                    }
                    write_stream(out, 0);
                    get_byte_array_output_stream_data(&buf, &attr->value, NULL);
                    *ref = attr;
                    ref = &attr->next;
                }
                b->type = type;
                b->addr = addr;
                b->kind = kind;
                b->process = p;
                list_add_last(&b->link_p2b, &p->link_p2b);
                b->bp = create_eventpoint_ext(attrs, NULL, breakpoint_cb, b);
                add_res_str(c, "OK");
                return 0;
            }
        }
    }
    add_res_str(c, "E01");
    return 0;
}

static int handle_z_command(GdbClient * c) {
    char * s = c->cmd_buf + 2;
    unsigned type = get_cmd_uint(c, &s);
    if (*s++ == ',') {
        uint64_t addr = get_cmd_uint64(c, &s);
        if (*s++ == ',') {
            unsigned kind = get_cmd_uint(c, &s);
            GdbProcess * p = find_process_pid(c, c->cur_g_pid);
            if (p != NULL) {
                LINK * l;
                for (l = p->link_p2b.next; l != &p->link_p2b; l = l->next) {
                    GdbBreakpoint * b = link_p2b(l);
                    if (b->type == type && b->addr == addr && b->kind == kind) {
                        free_breakpoint(b);
                        add_res_str(c, "OK");
                        return 0;
                    }
                }
            }
        }
    }
    add_res_str(c, "E01");
    return 0;
}

static int handle_command(GdbClient * c) {
    if (c->cmd_end < 2) return 0;
    switch (c->cmd_buf[1]) {
    case 'A': add_res_str(c, "E01"); return 0;
    case 'b': return 0;
    case 'B': return 0;
    case 'd': return 0;
    case 'g': return handle_g_command(c);
    case 'm': return handle_m_command(c);
    case 'M': return handle_M_command(c);
    case 'p': return handle_p_command(c);
    case 'P': return handle_P_command(c);
    case 'q': return handle_q_command(c);
    case 'Q': return handle_Q_command(c);
    case 'H': return handle_H_command(c);
    case '!': c->extended = 1; add_res_str(c, "OK"); return 0;
    case '?': return handle_qm_command(c);
    case 'v': return handle_v_command(c);
    case 'T': return handle_T_command(c);
    case 'D': return handle_D_command(c);
    case 'Z': return handle_Z_command(c);
    case 'z': return handle_z_command(c);
    }
    return 0;
}

static int read_packet(GdbClient * c, unsigned len) {
    unsigned char * b = c->buf;
    unsigned char * e = b + len;

    while (b < e) {
        char ch = *b++;
        if (c->cmd_pos > 0 || ch == '$') {
            if (ch == 0x7d && !c->cmd_esc) {
                c->cmd_esc = 1;
                continue;
            }
            if (ch == '#') {
                c->cmd_end = c->cmd_pos;
            }
            if (c->cmd_esc) {
                c->cmd_esc = 0;
                ch = (char)(ch ^ 0x20);
            }
            if (c->cmd_pos >= c->cmd_max) {
                c->cmd_max = c->cmd_max == 0 ? 0x100 : c->cmd_max * 2;
                c->cmd_buf = (char *)loc_realloc(c->cmd_buf, c->cmd_max);
            }
            c->cmd_buf[c->cmd_pos++] = ch;
            if (c->cmd_end > 0 && c->cmd_pos == c->cmd_end + 3) {
                if (!c->no_ack_mode && send(c->req.u.sio.sock, "+", 1, 0) < 0) return -1;
#if DEBUG_RSP
                printf("GDB -> %.*s\n", c->cmd_pos, c->cmd_buf);
#endif
                c->waiting = 0;
                lock_threads(c);
                c->res_pos = 0;
                c->xfer_range_offs = 0;
                c->xfer_range_size = 0;
                add_res_ch_no_esc(c, '$');
                if (handle_command(c) < 0) return -1;
                if (!c->waiting || c->res_pos > 1) {
                    c->waiting = 0;
                    if (c->xfer_range_offs > 0 || (c->xfer_range_size > 0 && c->xfer_range_size + 2 < c->res_pos)) {
                        unsigned offs = c->xfer_range_offs + 2; /* First bytes are "$l" */
                        unsigned size = c->xfer_range_size;
                        assert(c->res_buf[0] == '$');
                        assert(c->res_buf[1] == 'l');
                        if (offs >= c->res_pos) {
                            offs = 2;
                            size = 0;
                        }
                        else if (offs + size > c->res_pos) {
                            size = c->res_pos - offs;
                        }
                        else {
                            c->res_buf[1] = 'm';
                        }
                        memmove(c->res_buf + 2, c->res_buf + offs, size);
                        c->res_pos = size + 2;
                    }
                    if (send_res(c) < 0) return -1;
                }
                c->cmd_pos = 0;
                c->cmd_end = 0;
                c->cmd_esc = 0;
            }
        }
        else if (!c->no_ack_mode && ch == '-' && c->res_pos > 0) {
            if (send(c->req.u.sio.sock, c->res_buf, c->res_pos, 0) < 0) return -1;
        }
        else if (ch == 3) {
            LINK * l;
            for (l = c->link_c2p.next; l != &c->link_c2p; l = l->next) {
                LINK * m;
                GdbProcess * p = link_c2p(l);
                for (m = p->link_p2t.next; m != &p->link_p2t; m = m->next) {
                    GdbThread * t = link_p2t(m);
                    Context * ctx = t->ctx;
                    if (suspend_debug_context(ctx) < 0) {
                        char * name = ctx->name;
                        if (name == NULL) name = ctx->id;
                        trace(LOG_ALWAYS, "GDB Server: cannot suspend context %s: %s", name, errno_to_str(errno));
                    }
                }
            }
        }
    }

    return 0;
}

static void recv_done(void * args) {
    GdbClient * c = (GdbClient *)((AsyncReqInfo *)args)->client_data;
    if (c->req.error) {
        trace(LOG_ALWAYS, "GDB Server connection closed: %s", errno_to_str(c->req.error));
        close_client(c);
    }
    else if (c->req.u.sio.rval == 0) {
        close_client(c);
    }
    else {
        if (read_packet(c, c->req.u.sio.rval) < 0) {
            trace(LOG_ALWAYS, "GDB Server connection terminated: %s", errno_to_str(errno));
            close_client(c);
            return;
        }
        c->req.u.sio.rval = 0;
        async_req_post(&c->req);
    }
}

static void accept_done(void * args) {
    GdbServer * s = (GdbServer *)((AsyncReqInfo *)args)->client_data;
    GdbClient * c = NULL;
    const int opt = 1;
    int sock = 0;
    LINK * l;

    if (s->req.error) {
        trace(LOG_ALWAYS, "GDB Server terminated: %s", errno_to_str(s->req.error));
        dispose_server(s);
        return;
    }

    sock = s->req.u.acc.rval;
    c = (GdbClient *)loc_alloc_zero(sizeof(GdbClient));
    c->server = s;
    c->buf_max = 0x1000;
    c->buf = (uint8_t *)loc_alloc(c->buf_max);
    c->req.type = AsyncReqRecv;
    c->req.client_data = c;
    c->req.done = recv_done;
    c->req.u.sio.sock = sock;
    c->req.u.sio.bufp = c->buf;
    c->req.u.sio.bufsz = c->buf_max;
    c->req.u.sio.flags = 0;
    list_init(&c->link_c2p);
    list_add_last(&c->link_s2c, &s->link_s2c);
    c->client.dispose = dispose_client;

    for (l = context_root.next; l != &context_root; l = l->next) {
        Context * ctx = ctxl2ctxp(l);
        if (!ctx->exited && context_has_state(ctx)) {
            Context * prs = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
            if (check_process_isa(c, prs)) {
                GdbProcess * p = find_process_ctx(c, prs);
                if (p == NULL) p = add_process(c, prs);
            }
        }
    }

    notify_client_connected(&c->client);
    async_req_post(&s->req);

    if (setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (char *)&opt, sizeof(opt)) < 0) {
        trace(LOG_ALWAYS, "GDB Server setsockopt failed: %s", errno_to_str(errno));
        close_client(c);
        return;
    }

    if (setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (char *)&opt, sizeof(opt)) < 0) {
        trace(LOG_ALWAYS, "GDB Server setsockopt failed: %s", errno_to_str(errno));
        close_client(c);
        return;
    }

    post_event(start_client, c);
}

static void event_context_created(Context * ctx, void * args) {
    if (context_has_state(ctx)) {
        LINK * l, * n;
        Context * prs = context_get_group(ctx, CONTEXT_GROUP_PROCESS);
        for (l = link_a2s.next; l != &link_a2s; l = l->next) {
            GdbServer * s = link_a2s(l);
            for (n = s->link_s2c.next; n != &s->link_s2c; n = n->next) {
                GdbClient * c = link_s2c(n);
                if (check_process_isa(c, prs)) {
                    GdbProcess * p = find_process_ctx(c, prs);
                    if (p == NULL) p = add_process(c, prs);
                    else if (p->attached) add_thread(c, ctx);
                }
            }
        }
    }
}

static void event_context_exited(Context * ctx, void * args) {
    LINK * l, *n, *m, *o;
    for (l = link_a2s.next; l != &link_a2s; l = l->next) {
        GdbServer * s = link_a2s(l);
        for (n = s->link_s2c.next; n != &s->link_s2c; n = n->next) {
            GdbClient * c = link_s2c(n);
            for (m = c->link_c2p.next; m != &c->link_c2p; m = m->next) {
                GdbProcess * p = link_c2p(m);
                if (p->ctx == ctx) {
                    if (c->waiting) {
                        lock_threads(c);
                        if (is_all_intercepted(c)) {
                            c->res_pos = 0;
                            c->waiting = 0;
                            add_res_ch_no_esc(c, '$');
                            add_res_str(c, "W00;process:");
                            add_res_hex(c, p->pid);
                            if (send_res(c) < 0) trace(LOG_ALWAYS, "GDB Server send error: %s", errno_to_str(errno));
                        }
                    }
                    free_process(p);
                    break;
                }
                for (o = p->link_p2t.next; o != &p->link_p2t; o = o->next) {
                    GdbThread * t = link_p2t(o);
                    if (t->ctx == ctx) {
                        free_thread(t);
                        break;
                    }
                }
            }
        }
    }
}

static void event_register_definitions_changed(void * args) {
    LINK * l, * n, * m, * o;
    for (l = link_a2s.next; l != &link_a2s; l = l->next) {
        GdbServer * s = link_a2s(l);
        for (n = s->link_s2c.next; n != &s->link_s2c; n = n->next) {
            GdbClient * c = link_s2c(n);
            for (m = c->link_c2p.next; m != &c->link_c2p; m = m->next) {
                GdbProcess * p = link_c2p(m);
                for (o = p->link_p2t.next; o != &p->link_p2t; o = o->next) {
                    GdbThread * t = link_p2t(o);
                    loc_free(t->regs_nm_map);
                    t->regs_nm_map = NULL;
                }
            }
        }
    }
}

static void event_context_intercepted(Context * ctx, void * args) {
    LINK * l, *n, *m, *o;
    for (l = link_a2s.next; l != &link_a2s; l = l->next) {
        GdbServer * s = link_a2s(l);
        for (n = s->link_s2c.next; n != &s->link_s2c; n = n->next) {
            GdbClient * c = link_s2c(n);
            if (c->waiting) {
                for (m = c->link_c2p.next; m != &c->link_c2p; m = m->next) {
                    GdbProcess * p = link_c2p(m);
                    for (o = p->link_p2t.next; o != &p->link_p2t; o = o->next) {
                        GdbThread * t = link_p2t(o);
                        if (t->ctx == ctx) {
                            if (!c->stopped) {
                                c->cur_g_pid = p->pid;
                                c->cur_g_tid = t->tid;
                                lock_threads(c);
                            }
                        }
                    }
                }
                if (c->stopped && is_all_intercepted(c)) {
                    c->res_pos = 0;
                    c->waiting = 0;
                    add_res_ch_no_esc(c, '$');
                    add_res_stop_reason(c);
                    if (send_res(c) < 0) trace(LOG_ALWAYS, "GDB Server send error: %s", errno_to_str(errno));
                }
            }
        }
    }
}

static ContextEventListener context_listener = {
    event_context_created,
    event_context_exited,
    NULL,
    NULL,
    NULL,
    NULL
};

static RegistersEventListener registers_listener = {
    NULL,
    event_register_definitions_changed
};

static RunControlEventListener run_ctrl_listener = {
    event_context_intercepted,
    NULL,
};

int ini_gdb_rsp(const char * conf) {
    GdbServer * s = NULL;
    char port[64];
    const char * isa = NULL;
    const char * sep = strchr(conf, ':');
    int sock = -1;
    strlcpy(port, conf, sizeof(port));
    if (sep != NULL) {
        isa = sep + 1;
        if ((size_t)(sep - conf) < sizeof(port)) port[sep - conf] = 0;
    }
    else {
#if defined(_AMD64_) || defined(__x86_64__)
        isa = "amd64";
#elif defined(__aarch64__)
        isa = "aarch64";
#elif defined(__arm__)
        isa = "arm";
#elif defined(__powerpc64__)
        isa = "powerpc64";
#elif defined(__powerpc__)
        isa = "powerpc";
#elif defined(__MICROBLAZE__)
        isa = "microblaze";
#elif defined(__MICROBLAZE64__)
        isa = "microblaze64";
#elif defined(__riscv) && __riscv_xlen == 32
        isa = "riscv32";
#elif defined(__riscv) && __riscv_xlen == 64
        isa = "riscv64";
#elif defined(__riscv) && __riscv_xlen == 128
        isa = "riscv128";
#else
        isa = "i386";
#endif
    }
    if (ini_done) {
        LINK * l;
        for (l = link_a2s.next; l != &link_a2s; l = l->next) {
            GdbServer * g = link_a2s(l);
            if (strcmp(g->port, port) == 0) {
                if (strcmp(g->isa, isa) == 0) return 0;
                set_fmt_errno(ERR_OTHER, "Port is used by '%s:%s' server", g->port, g->isa);
                return -1;
            }
        }
    }
    sock = open_server(port);
    if (sock < 0) return -1;
    if (!ini_done) {
        list_init(&link_a2s);
        add_context_event_listener(&context_listener, NULL);
        add_registers_event_listener(&registers_listener, NULL);
        add_run_control_event_listener(&run_ctrl_listener, NULL);
        ini_done = 1;
    }
    s = (GdbServer *)loc_alloc_zero(sizeof(GdbServer));
    list_init(&s->link_s2c);
    list_add_last(&s->link_a2s, &link_a2s);
    s->req.type = AsyncReqAccept;
    s->req.client_data = s;
    s->req.done = accept_done;
    s->req.u.acc.sock = sock;
    s->req.u.acc.rval = 0;
    strlcpy(s->port, port, sizeof(s->port));
    strlcpy(s->isa, isa, sizeof(s->isa));
    async_req_post(&s->req);
    return 0;
}

#endif /* ENABLE_GdbRemoteSerialProtocol */
