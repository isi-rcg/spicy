/*******************************************************************************
 * Copyright (c) 2007-2018 Wind River Systems, Inc. and others.
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
 * TCF Processes - process control service.
 * Processes service provides access to the target OS's process information,
 * allows to start and terminate a process, and allows to attach and
 * detach a process for debugging. Debug services, like Memory and Run Control,
 * require a process to be attached before they can access it.
 */

/* TODO: It should be possible to filter processes on a criteria (user, name, etc) */

#if defined(__GNUC__) && !defined(_GNU_SOURCE)
#  define _GNU_SOURCE
#endif

#include <tcf/config.h>

#if SERVICE_Processes || SERVICE_Terminals

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <assert.h>
#include <tcf/framework/mdep-fs.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/context.h>
#include <tcf/framework/json.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/waitpid.h>
#include <tcf/framework/signames.h>
#include <tcf/framework/mdep-fs.h>
#include <tcf/services/streamsservice.h>
#include <tcf/services/runctrl.h>
#include <tcf/services/processes.h>

#if SERVICE_Processes
static const char * PROCESSES[2] = { "Processes", "ProcessesV1" };
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
#  include <tlhelp32.h>
#  ifdef _MSC_VER
#    pragma warning(disable:4201) /* nonstandard extension used : nameless struct/union (in winternl.h) */
#    include <winternl.h>
#  else
#    include <ntdef.h>
#  endif
#  ifndef STATUS_INFO_LENGTH_MISMATCH
#   define STATUS_INFO_LENGTH_MISMATCH      ((NTSTATUS)0xC0000004L)
#  endif
#  ifndef SystemHandleInformation
#    define SystemHandleInformation 16
#  endif
#elif defined(_WRS_KERNEL)
#  include <symLib.h>
#  include <sysSymTbl.h>
#  include <ioLib.h>
#  include <ptyDrv.h>
#  include <taskHookLib.h>
#else
#  include <termios.h>
#  ifndef TIOCGWINSZ
#    include <sys/ioctl.h>
#  endif
#  include <sys/stat.h>
#  include <unistd.h>
#endif

#define PBUF_SIZE 0x400
#define PIPE_SIZE 0x400
#define SBUF_SIZE 0x1000

typedef struct AttachDoneArgs {
    Channel * c;
    char token[256];
    SigSet sig_dont_stop;
    SigSet sig_dont_pass;
    int set_dont_stop;
    int set_dont_pass;
} AttachDoneArgs;

struct ChildProcess {
    LINK link;
    int pid;
    int tty;
    int got_output;
    TCFBroadcastGroup * bcg;
    struct ProcessInput * inp_struct;
    struct ProcessOutput * out_struct;
    struct ProcessOutput * err_struct;
    char name[256];
    char service[256];
    long exit_code;
    EventCallBack * exit_cb;
    void * exit_args;
    unsigned ws_col;
    unsigned ws_row;
};

typedef struct ProcessOutput {
    int fd;
    char id[256];
    ChildProcess * prs;
    AsyncReqInfo req;
    int req_posted;
    char buf[PBUF_SIZE];
    size_t buf_pos;
    int eos;
    VirtualStream * vstream;
} ProcessOutput;

typedef struct ProcessInput {
    int fd;
    char id[256];
    ChildProcess * prs;
    AsyncReqInfo req;
    int req_posted;
    char buf[PBUF_SIZE];
    size_t buf_pos;
    size_t buf_len;
    int eos;
    VirtualStream * vstream;
} ProcessInput;

#define link2prs(A)  ((ChildProcess *)((char *)(A) - offsetof(ChildProcess, link)))

static int init_done = 0;
static LINK prs_list = TCF_LIST_INIT(prs_list);
#if defined(_WRS_KERNEL)
static SEM_ID prs_list_lock = NULL;
#endif

static ChildProcess * find_process(int pid) {
    LINK * qhp = &prs_list;
    LINK * qp = qhp->next;

    while (qp != qhp) {
        ChildProcess * prs = link2prs(qp);
        if (prs->pid == pid) return prs;
        qp = qp->next;
    }
    return NULL;
}

#if SERVICE_Processes

static int is_attached(pid_t pid) {
#if ENABLE_DebugContext
    return context_find_from_pid(pid, 0) != NULL;
#else
    return 0;
#endif
}

static void write_context(OutputStream * out, int pid) {
    ChildProcess * prs = find_process(pid);

    write_stream(out, '{');

    /* the process Name */
#if defined(__linux__)
    /* Use the /proc to get the name */
    {
        char buff[256];
        char fname[256];
        FILE * file;
        const char * name = NULL;

        /* clear out buff */
        buff[0] = 0;

        /* try cmdline */
        snprintf(fname, sizeof(fname), "/proc/%d/cmdline", pid);
        file = fopen(fname, "r");
        if (file) {
            if (fgets(buff, sizeof(buff), file) != NULL) {
                if (buff[0]) name = buff;
            }
            fclose(file);
        }

        if (!name) {
            /* try status */
            snprintf(fname, sizeof(fname), "/proc/%d/status", pid);
            file = fopen(fname, "r");
            if (file) {
                char * p = fgets(buff, sizeof(buff), file);
                if (p != NULL) {
                    /* Find the attribute name */
                    for (; *p; ++p) {
                        if (*p == ':') {
                            /* close off the attr name string */
                            *p++ = 0;

                            /* is it our name? */
                            if (!strcmp(buff, "Name")) {
                                char * n;

                                /* change tab to '[' */
                                *p = '[';

                                /* change trailing new line to ']' */
                                for (n = p; *n; ++n)
                                    if (*n == '\n')
                                        *n = ']';

                                name = p;
                                break;
                            }
                        }
                    }
                }
                fclose(file);
            }
        }

        if (!name) name = pid2id(pid, 0);

        /* Send it out */
        json_write_string(out, "Name");
        write_stream(out, ':');
        json_write_string(out, name);
        write_stream(out, ',');
    }
#else
    json_write_string(out, "Name");
    write_stream(out, ':');
    json_write_string(out, prs ? prs->name : pid2id(pid, 0));
    write_stream(out, ',');
#endif

    json_write_string(out, "CanTerminate");
    write_stream(out, ':');
    json_write_boolean(out, 1);
    write_stream(out, ',');

    if (is_attached(pid)) {
        json_write_string(out, "Attached");
        write_stream(out, ':');
        json_write_boolean(out, 1);
        write_stream(out, ',');
    }

    if (prs != NULL) {
        if (prs->inp_struct) {
            json_write_string(out, "StdInID");
            write_stream(out, ':');
            json_write_string(out, prs->inp_struct->id);
            write_stream(out, ',');
        }
        if (prs->out_struct) {
            json_write_string(out, "StdOutID");
            write_stream(out, ':');
            json_write_string(out, prs->out_struct->id);
            write_stream(out, ',');
        }
        if (prs->err_struct) {
            json_write_string(out, "StdErrID");
            write_stream(out, ':');
            json_write_string(out, prs->err_struct->id);
            write_stream(out, ',');
        }
    }

    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, pid2id(pid, 0));

    write_stream(out, '}');
}

static void send_event_process_exited(OutputStream * out, ChildProcess * prs) {
    int v;
    for (v = 0; v < 2; v++) {
        write_stringz(out, "E");
        write_stringz(out, PROCESSES[v]);
        write_stringz(out, "exited");

        json_write_string(out, pid2id(prs->pid, 0));
        write_stream(out, 0);

        json_write_long(out, prs->exit_code);
        write_stream(out, 0);

        write_stream(out, MARKER_EOM);
    }
}

static void command_get_context(char * token, Channel * c) {
    int err = 0;
    char id[256];
    pid_t pid, parent;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    pid = id2pid(id, &parent);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    if (pid != 0 && parent == 0) {
#if defined(_WIN32) || defined(__CYGWIN__)
#elif defined(_WRS_KERNEL)
        if (TASK_ID_VERIFY(pid) == ERROR) err = ERR_INV_CONTEXT;
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#else
        struct stat st;
        char dir[FILE_PATH_SIZE];
        snprintf(dir, sizeof(dir), "/proc/%d", pid);
        if (lstat(dir, &st) < 0) err = errno;
        else if (!S_ISDIR(st.st_mode)) err = ERR_INV_CONTEXT;
#endif
    }

    write_errno(&c->out, err);

    if (err == 0 && pid != 0 && parent == 0) {
        write_context(&c->out, pid);
        write_stream(&c->out, 0);
    }
    else {
        write_stringz(&c->out, "null");
    }

    write_stream(&c->out, MARKER_EOM);
}

static void command_get_children(char * token, Channel * c) {
    char id[256];
    int attached_only;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    attached_only = json_read_boolean(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    if (id[0] != 0) {
        write_errno(&c->out, 0);
        write_stringz(&c->out, "null");
    }
    else {
#if defined(_WIN32) || defined(__CYGWIN__)
    DWORD err = 0;
    HANDLE snapshot;
    PROCESSENTRY32 pe32;

    snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) err = set_win32_errno(GetLastError());
    memset(&pe32, 0, sizeof(pe32));
    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (!err && !Process32First(snapshot, &pe32)) {
        err = set_win32_errno(GetLastError());
        CloseHandle(snapshot);
    }
    write_errno(&c->out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        int cnt = 0;
        write_stream(&c->out, '[');
        do {
            if (!attached_only || is_attached(pe32.th32ProcessID)) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, pid2id(pe32.th32ProcessID, 0));
                cnt++;
            }
        }
        while (Process32Next(snapshot, &pe32));
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);
    }
    if (snapshot != INVALID_HANDLE_VALUE) CloseHandle(snapshot);
#elif defined(_WRS_KERNEL)
        int i = 0;
        int cnt = 0;
        int ids_cnt = 0;
        int ids_max = 500;
        int * ids = (int *)loc_alloc(ids_max * sizeof(int));
        for (;;) {
            ids_cnt = taskIdListGet(ids, ids_max);
            if (ids_cnt < ids_max) break;
            loc_free(ids);
            ids_max *= 2;
            ids = (int *)loc_alloc(ids_max * sizeof(int));
        }
        write_errno(&c->out, 0);
        write_stream(&c->out, '[');
        for (i = 0; i < ids_cnt; i++) {
            if (!attached_only || is_attached(ids[i])) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, pid2id(ids[i], 0));
                cnt++;
            }
        }
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#else
        DIR * proc = opendir("/proc");
        if (proc == NULL) {
            write_errno(&c->out, errno);
            write_stringz(&c->out, "null");
        }
        else {
            int cnt = 0;
            write_errno(&c->out, 0);
            write_stream(&c->out, '[');
            for (;;) {
                struct dirent * ent = readdir(proc);
                if (ent == NULL) break;
                if (ent->d_name[0] >= '1' && ent->d_name[0] <= '9') {
                    pid_t pid = atol(ent->d_name);
                    if (!attached_only || is_attached(pid)) {
                        if (cnt > 0) write_stream(&c->out, ',');
                        json_write_string(&c->out, pid2id(pid, 0));
                        cnt++;
                    }
                }
            }
            write_stream(&c->out, ']');
            write_stream(&c->out, 0);
            closedir(proc);
        }
#endif
    }

    write_stream(&c->out, MARKER_EOM);
}

#if ENABLE_DebugContext

static void attach_done(int error, Context * ctx, void * arg) {
    AttachDoneArgs * data = (AttachDoneArgs *)arg;
    Channel * c = data->c;

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, data->token);
        write_errno(&c->out, error);
        write_stream(&c->out, MARKER_EOM);
    }
    channel_unlock_with_msg(c, PROCESSES[0]);
    loc_free(data);
}

static void command_attach(char * token, Channel * c) {
    int err = 0;
    char id[256];
    pid_t pid, parent;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    pid = id2pid(id, &parent);

    if (parent != 0) {
        err = ERR_INV_CONTEXT;
    }
    else if (is_attached(pid)) {
        err = ERR_ALREADY_ATTACHED;
    }
    else {
        AttachDoneArgs * data = (AttachDoneArgs *)loc_alloc_zero(sizeof *data);
        data->c = c;
        strlcpy(data->token, token, sizeof(data->token));
        if (context_attach(pid, attach_done, data, 0) == 0) {
            channel_lock_with_msg(c, PROCESSES[0]);
            return;
        }
        err = errno;
        loc_free(data);
    }
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_detach(char * token, Channel * c) {
    int err = 0;
    char id[256];
    Context * ctx = NULL;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    ctx = id2ctx(id);
    if (ctx == NULL) err = ERR_INV_CONTEXT;
    if (!err && ctx->exited) err = ERR_ALREADY_EXITED;
    if (!err && detach_debug_context(ctx) < 0) err = errno;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void write_sigset(OutputStream * out, SigSet * set) {
    unsigned bit = 0;
    uint64_t bits = 0;
    while (sigset_get_next(set, &bit)) {
        if (bit >= sizeof(bits) * 8) {
            /* Does not fit into int bitset */
            unsigned cnt = 0;
            bit = 0;
            write_stream(out, '[');
            while (sigset_get_next(set, &bit)) {
                if (cnt > 0) write_stream(out, ',');
                json_write_ulong(out, bit);
                cnt++;
            }
            write_stream(out, ']');
            return;
        }
        bits |= (uint64_t)1 << bit;
    }
    json_write_uint64(out, bits);
}

static void command_get_signal_mask(char * token, Channel * c) {
    int err = 0;
    char id[256];
    Context * ctx = NULL;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    ctx = id2ctx(id);
    if (ctx == NULL) err = ERR_INV_CONTEXT;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);

    if (ctx == NULL) {
        write_stringz(&c->out, "null");
        write_stringz(&c->out, "null");
        write_stringz(&c->out, "null");
    }
    else {
        write_sigset(&c->out, &ctx->sig_dont_stop);
        write_stream(&c->out, 0);
        write_sigset(&c->out, &ctx->sig_dont_pass);
        write_stream(&c->out, 0);
        write_sigset(&c->out, &ctx->pending_signals);
        write_stream(&c->out, 0);
    }

    write_stream(&c->out, MARKER_EOM);
}

static void read_sigset_bit(InputStream * inp, void * args) {
    SigSet * set = (SigSet *)args;
    unsigned bit = (unsigned)json_read_ulong(inp);
    sigset_set(set, bit, 1);
}

static void read_sigset(InputStream * inp, SigSet * set, int * not_null) {
    memset(set, 0, sizeof(SigSet));
    if (json_peek(inp) == '[' || json_peek(inp) == 'n') {
        *not_null = json_read_array(inp, read_sigset_bit, set);
    }
    else {
        unsigned bit;
        uint64_t bits = json_read_uint64(inp);
        for (bit = 0; bit < sizeof(bits) * 8; bit++) {
            sigset_set(set, bit, (bits & ((uint64_t)1 << bit)) != 0);
        }
        *not_null = 1;
    }
}

static void get_deafult_sig_dont_stop(SigSet * set) {
    int i;
    int n = signal_cnt();
    for (i = 0; i < n; i++) {
        const char * nm = signal_name(i);
        if (nm == NULL) continue;
        if (strcmp(nm, "SIGCHLD") == 0 ||
            strcmp(nm, "SIGPIPE") == 0 ||
            strcmp(nm, "SIGWINCH") == 0) {
            sigset_set(set, i, 1);
        }
    }
}

static void get_deafult_sig_dont_pass(SigSet * set) {
    /* Empty */
}

static void command_set_signal_mask(char * token, Channel * c) {
    int err = 0;
    char id[256];
    Context * ctx = NULL;
    SigSet dont_stop;
    SigSet dont_pass;
    int set_dont_stop = 0;
    int set_dont_pass = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    read_sigset(&c->inp, &dont_stop, &set_dont_stop);
    json_test_char(&c->inp, MARKER_EOA);
    read_sigset(&c->inp, &dont_pass, &set_dont_pass);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    ctx = id2ctx(id);
    if (ctx == NULL) {
        err = ERR_INV_CONTEXT;
        sigset_clear(&dont_stop);
        sigset_clear(&dont_pass);
    }
    else {
        sigset_clear(&ctx->sig_dont_stop);
        sigset_clear(&ctx->sig_dont_pass);
        if (set_dont_stop) ctx->sig_dont_stop = dont_stop;
        else get_deafult_sig_dont_stop(&ctx->sig_dont_stop);
        if (set_dont_pass) ctx->sig_dont_pass = dont_pass;
        else get_deafult_sig_dont_pass(&ctx->sig_dont_pass);
    }

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void start_done(int error, Context * ctx, void * arg) {
    AttachDoneArgs * data = (AttachDoneArgs *)arg;
    Channel * c = data->c;

    if (ctx == NULL) {
        sigset_clear(&data->sig_dont_stop);
        sigset_clear(&data->sig_dont_pass);
    }
    else {
        LINK * l = ctx->children.next;
        while (l != &ctx->children) {
            Context * x = cldl2ctxp(l);
            sigset_clear(&x->sig_dont_stop);
            sigset_clear(&x->sig_dont_pass);
            if (data->set_dont_stop) sigset_copy(&x->sig_dont_stop, &data->sig_dont_stop);
            else get_deafult_sig_dont_stop(&x->sig_dont_stop);
            if (data->set_dont_pass) sigset_copy(&x->sig_dont_pass, &data->sig_dont_pass);
            else get_deafult_sig_dont_pass(&x->sig_dont_pass);
            l = l->next;
        }
        sigset_clear(&ctx->sig_dont_stop);
        sigset_clear(&ctx->sig_dont_pass);
        if (data->set_dont_stop) ctx->sig_dont_stop = data->sig_dont_stop;
        else get_deafult_sig_dont_stop(&ctx->sig_dont_stop);
        if (data->set_dont_pass) ctx->sig_dont_pass = data->sig_dont_pass;
        else get_deafult_sig_dont_pass(&ctx->sig_dont_pass);
    }

    if (!is_channel_closed(c)) {
        write_stringz(&c->out, "R");
        write_stringz(&c->out, data->token);
        write_errno(&c->out, error);
        if (ctx == NULL) write_string(&c->out, "null");
        else write_context(&c->out, id2pid(ctx->id, NULL));
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);
    }

    channel_unlock_with_msg(c, PROCESSES[0]);
    loc_free(data);
}

#endif /* ENABLE_DebugContext */

static void command_terminate(char * token, Channel * c) {
    int err = 0;
    char id[256];
    pid_t pid, parent;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    pid = id2pid(id, &parent);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    if (parent != 0) {
        err = ERR_INV_CONTEXT;
    }
#if ENABLE_DebugContext
    else if (is_attached(pid)) {
        if (terminate_debug_context(id2ctx(id)) < 0) err = errno;
    }
#endif
    else {
#if defined(_WIN32) || defined(__CYGWIN__)
        HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (h == NULL) {
            err = set_win32_errno(GetLastError());
        }
        else {
            if (!TerminateProcess(h, 1)) err = set_win32_errno(GetLastError());
            if (!CloseHandle(h) && !err) err = set_win32_errno(GetLastError());
        }
#else
        if (kill(pid, SIGTERM) < 0) err = errno;
#endif
    }

    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_signal(char * token, Channel * c) {
    int err = 0;
    char id[256];
    int signal = 0;
    pid_t pid, parent;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    signal = (int)json_read_long(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    pid = id2pid(id, &parent);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

#if defined(_WIN32) || defined(__CYGWIN__)
    if (parent != 0) {
        err = ERR_INV_CONTEXT;
    }
    else if (signal_code(signal) == 0x40010005) {
        /* Control-C */
        HANDLE h = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if (h == NULL) {
            err = set_win32_errno(GetLastError());
        }
        else {
            if (!TerminateProcess(h, 1)) err = set_win32_errno(GetLastError());
            if (!CloseHandle(h) && !err) err = set_win32_errno(GetLastError());
        }
    }
    else {
        err = ENOSYS;
    }
#elif defined(_WRS_KERNEL)
    if (kill(pid, signal) < 0) err = errno;
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
    if (kill(pid, signal) < 0) err = errno;
#else
    if (parent == 0) {
        if (kill(pid, signal) < 0) err = errno;
    }
    else {
        if (tkill(pid, signal) < 0) err = errno;
    }
#endif

    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_get_signal_list(char * token, Channel * c) {
    int err = 0;
    char id[256];

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    /* pid is ignored, same signal list for all */
    id2pid(id, NULL);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    write_errno(&c->out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        int i = 0;
        int n = 0;
        int cnt = signal_cnt();
        write_stream(&c->out, '[');
        for (i = 0; i < cnt; i++) {
            const char * name = signal_name(i);
            const char * desc = signal_description(i);
            if (name != NULL || desc != NULL) {
                if (n > 0) write_stream(&c->out, ',');
                write_stream(&c->out, '{');
                json_write_string(&c->out, "Index");
                write_stream(&c->out, ':');
                json_write_long(&c->out, i);
                if (name != NULL) {
                    write_stream(&c->out, ',');
                    json_write_string(&c->out, "Name");
                    write_stream(&c->out, ':');
                    json_write_string(&c->out, name);
                }
                if (desc != NULL) {
                    write_stream(&c->out, ',');
                    json_write_string(&c->out, "Description");
                    write_stream(&c->out, ':');
                    json_write_string(&c->out, desc);
                }
                write_stream(&c->out, ',');
                json_write_string(&c->out, "Code");
                write_stream(&c->out, ':');
                json_write_ulong(&c->out, signal_code(i));
                write_stream(&c->out, '}');
                n++;
            }
        }
        write_stream(&c->out, ']');
        write_stream(&c->out, 0);
    }

    write_stream(&c->out, MARKER_EOM);
}

static void command_get_environment(char * token, Channel * c) {
    char ** p = environ;

    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, '[');
    if (p != NULL) {
        while (*p != NULL) {
            if (p != environ) write_stream(&c->out, ',');
            json_write_string(&c->out, *p++);
        }
    }
    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

#endif

static void process_exited(ChildProcess * prs) {
#if SERVICE_Processes
    send_event_process_exited(&prs->bcg->out, prs);
#endif
#if defined(_WRS_KERNEL)
    semTake(prs_list_lock, WAIT_FOREVER);
#endif
    list_remove(&prs->link);
    broadcast_group_unlock(prs->bcg);
    if (prs->inp_struct) {
        ProcessInput * inp = prs->inp_struct;
        if (!inp->req_posted) {
            virtual_stream_delete(inp->vstream);
            close(inp->fd);
            loc_free(inp);
        }
        else {
            inp->prs = NULL;
        }
    }
    if (prs->out_struct) prs->out_struct->prs = NULL;
    if (prs->err_struct) prs->err_struct->prs = NULL;
    if (prs->exit_cb) prs->exit_cb(prs->exit_args);
    loc_free(prs);
#if defined(_WRS_KERNEL)
    semGive(prs_list_lock);
#endif
}

static void process_input_streams_callback(VirtualStream * stream, int event_code, void * args) {
    ProcessInput * inp = (ProcessInput *)args;

    assert(inp->vstream == stream);
    if (!inp->req_posted) {
        if (inp->buf_pos >= inp->buf_len && !inp->eos) {
            inp->buf_pos = inp->buf_len = 0;
            virtual_stream_get_data(stream, inp->buf, sizeof(inp->buf), &inp->buf_len, &inp->eos);
        }
        if (inp->buf_pos < inp->buf_len) {
            inp->req.u.fio.bufp = inp->buf + inp->buf_pos;
            inp->req.u.fio.bufsz = inp->buf_len - inp->buf_pos;
            inp->req_posted = 1;
            async_req_post(&inp->req);
        }
    }
}

static void write_process_input_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    ProcessInput * inp = (ProcessInput *)req->client_data;

    inp->req_posted = 0;
    if (inp->prs == NULL) {
        /* Process has exited */
        virtual_stream_delete(inp->vstream);
        close(inp->fd);
        loc_free(inp);
    }
    else {
        int wr = inp->req.u.fio.rval;

        if (wr < 0) {
            trace(LOG_ALWAYS, "Can't write process input stream: %s", errno_to_str(inp->req.error));
            inp->buf_pos = inp->buf_len = 0;
        }
        else {
            inp->buf_pos += wr;
        }

        process_input_streams_callback(inp->vstream, 0, inp);
    }
}

static ProcessInput * write_process_input(ChildProcess * prs, int fd) {
    ProcessInput * inp = (ProcessInput *)loc_alloc_zero(sizeof(ProcessInput));
    inp->fd = fd;
    inp->prs = prs;
    inp->req.client_data = inp;
    inp->req.done = write_process_input_done;
    inp->req.type = AsyncReqWrite;
    inp->req.u.fio.fd = fd;
    virtual_stream_create(prs->service, pid2id(prs->pid, 0), SBUF_SIZE, VS_ENABLE_REMOTE_WRITE,
        process_input_streams_callback, inp, &inp->vstream);
    virtual_stream_get_id(inp->vstream, inp->id, sizeof(inp->id));
    return inp;
}

static void post_out_read_req(void * args) {
    ProcessOutput * out = (ProcessOutput *)args;
    assert(out->req_posted);
    async_req_post(&out->req);
}

static void process_output_streams_callback(VirtualStream * stream, int event_code, void * args) {
    ProcessOutput * out = (ProcessOutput *)args;

    assert(out->vstream == stream);
    if (!out->req_posted) {
        int buf_len = out->req.u.fio.rval;
        int err = 0;
        int eos = 0;

        if (buf_len < 0) {
            buf_len = 0;
            err = out->req.error;
        }
        if (buf_len == 0) eos = 1;
        if (err && out->prs == NULL) err = 0;

        assert((size_t)buf_len <= sizeof(out->buf));
        assert(out->buf_pos <= (size_t)buf_len);
        assert(out->req.u.fio.bufp == out->buf);
#ifdef __linux__
        if (err == EIO) err = 0;
#endif
        if (err) trace(LOG_ALWAYS, "Can't read process output stream: %d %s", err, errno_to_str(err));

        if (out->buf_pos < (size_t)buf_len || out->eos != eos) {
            size_t done = 0;
            virtual_stream_add_data(stream, out->buf + out->buf_pos, buf_len - out->buf_pos, &done, eos);
            out->buf_pos += done;
            assert(out->buf_pos <= (size_t)buf_len);
            if (out->buf_pos == (size_t)buf_len && eos) out->eos = 1;
        }

        if (out->buf_pos >= (size_t)buf_len) {
            if (!eos) {
                out->req_posted = 1;
                post_event_with_delay(post_out_read_req, out, 10000);
            }
            else if (virtual_stream_is_empty(stream)) {
                if (out->prs != NULL) {
                    if (out == out->prs->out_struct) out->prs->out_struct = NULL;
                    if (out == out->prs->err_struct) out->prs->err_struct = NULL;
                }
                virtual_stream_delete(stream);
                close(out->fd);
                loc_free(out);
            }
        }
    }
}

static void read_process_output_done(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;
    ProcessOutput * out = (ProcessOutput *)req->client_data;

    out->buf_pos = 0;
    out->req_posted = 0;
    if (out->prs && out->req.u.fio.rval > 0) out->prs->got_output = 1;
    process_output_streams_callback(out->vstream, 0, out);
}

static ProcessOutput * read_process_output(ChildProcess * prs, int fd) {
    ProcessOutput * out = (ProcessOutput *)loc_alloc_zero(sizeof(ProcessOutput));
    out->fd = fd;
    out->prs = prs;
    out->req.client_data = out;
    out->req.done = read_process_output_done;
    out->req.type = AsyncReqRead;
    out->req.u.fio.bufp = out->buf;
    out->req.u.fio.bufsz = sizeof(out->buf);
    out->req.u.fio.fd = fd;
    virtual_stream_create(prs->service, pid2id(prs->pid, 0), SBUF_SIZE, VS_ENABLE_REMOTE_READ,
        process_output_streams_callback, out, &out->vstream);
    virtual_stream_get_id(out->vstream, out->id, sizeof(out->id));
    out->req_posted = 1;
    async_req_post(&out->req);
    return out;
}

#if !ENABLE_DebugContext
#  define context_attach(pid, done, client_data, mode) (errno = ERR_UNSUPPORTED, -1)
#  define context_attach_self() (errno = ERR_UNSUPPORTED, -1)
#endif

#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)

#if defined(_MSC_VER)
static int setenv(const char * name, const char * val, int overwrite) {
    assert(overwrite);
    _putenv_s(name, val);
    return 0;
}
#elif defined(__MINGW32__) && __MINGW32_MAJOR_VERSION < 5
static int setenv(const char * name, const char * val, int overwrite) {
    int len = strlen(name) + strlen(val) + 2;
    char * str = (char *)loc_alloc(len);
    snprintf(str, len, "%s=%s", name, val);
    putenv(str);
    return 0;
}
#endif

static int start_process_imp(Channel * c, char ** envp, const char * dir, const char * exe, char ** args,
                  ProcessStartParams * params, int * selfattach, ChildProcess ** prs) {
    typedef struct _HANDLE_INFORMATION {
            USHORT ProcessId;
            USHORT CreatorBackTraceIndex;
            UCHAR ObjectTypeNumber;
            UCHAR Flags;
            USHORT Handle;
            PVOID Object;
            ACCESS_MASK GrantedAccess;
    } HANDLE_INFORMATION;
    typedef struct _SYSTEM_HANDLE_INFORMATION {
        ULONG Count;
        HANDLE_INFORMATION Handles[1];
    } SYSTEM_HANDLE_INFORMATION;
    typedef NTSTATUS (FAR WINAPI * QuerySystemInformationTypedef)(int, PVOID, ULONG, PULONG);
    QuerySystemInformationTypedef QuerySystemInformationProc = (QuerySystemInformationTypedef)GetProcAddress(
        GetModuleHandle("NTDLL.DLL"), "NtQuerySystemInformation");
    DWORD size;
    DWORD pid = 0;
    NTSTATUS status;
    SYSTEM_HANDLE_INFORMATION * hi = NULL;
    int fpipes[3][2];
    HANDLE hpipes[3][2];
    struct stat st;
    int err = 0;
    int i;

    size = sizeof(SYSTEM_HANDLE_INFORMATION) + sizeof(HANDLE_INFORMATION) * 256;
    hi = (SYSTEM_HANDLE_INFORMATION *)tmp_alloc_zero(size);
    for (;;) {
        status = QuerySystemInformationProc(SystemHandleInformation, hi, size, &size);
        if (status != STATUS_INFO_LENGTH_MISMATCH) break;
        hi = (SYSTEM_HANDLE_INFORMATION *)tmp_realloc(hi, size);
    }
    if (status == 0) {
        ULONG l;
        DWORD id = GetCurrentProcessId();
        for (l = 0; l < hi->Count; l++) {
            if (hi->Handles[l].ProcessId != id) continue;
            SetHandleInformation((HANDLE)(uintptr_t)hi->Handles[l].Handle, HANDLE_FLAG_INHERIT, FALSE);
        }
    }
    else {
        err = set_win32_errno(status);
    }

    memset(hpipes, 0, sizeof(hpipes));
    for (i = 0; i < 3; i++) fpipes[i][0] = fpipes[i][1] = -1;
    if (!err) {
#if defined(__CYGWIN__)
        for (i = 0; i < 3; i++) {
            if (pipe(fpipes[i]) < 0) {
                err = errno;
                break;
            }
            hpipes[i][0] = (HANDLE)get_osfhandle(fpipes[i][0]);
            hpipes[i][1] = (HANDLE)get_osfhandle(fpipes[i][1]);
        }
#else
        for (i = 0; i < 3; i++) {
            if (!CreatePipe(&hpipes[i][0], &hpipes[i][1], NULL, PIPE_SIZE)) {
                err = set_win32_errno(GetLastError());
                break;
            }
            fpipes[i][0] = _open_osfhandle((intptr_t)hpipes[i][0], O_TEXT);
            fpipes[i][1] = _open_osfhandle((intptr_t)hpipes[i][1], O_TEXT);
        }
#endif
    }

    if (!err && params->dir != NULL && chdir(params->dir) < 0) err = errno;

    if (!err) {
        STARTUPINFO si;
        PROCESS_INFORMATION prs_info;
        const char * fnm = exe;
        char * cmd = NULL;
        char * env = NULL;
        const char * env_path = NULL;
        SetHandleInformation(hpipes[0][0], HANDLE_FLAG_INHERIT, TRUE);
        SetHandleInformation(hpipes[1][1], HANDLE_FLAG_INHERIT, TRUE);
        SetHandleInformation(hpipes[2][1], HANDLE_FLAG_INHERIT, TRUE);
        memset(&si, 0, sizeof(si));
        memset(&prs_info, 0, sizeof(prs_info));
        si.cb = sizeof(si);
        si.dwFlags |= STARTF_USESTDHANDLES;
        si.hStdInput  = hpipes[0][0];
        si.hStdOutput = hpipes[1][1];
        si.hStdError  = hpipes[2][1];
        if (args != NULL) {
            int cmd_size = 0;
            int cmd_pos = 0;
#           define cmd_append(ch) { \
                if (cmd_pos >= cmd_size) { \
                    cmd_size += 0x1000; \
                    cmd = (char *)tmp_realloc(cmd, cmd_size); \
                } \
                cmd[cmd_pos++] = (ch); \
            }
            i = 0;
            while (args[i] != NULL) {
                const char * p = args[i++];
                if (cmd_pos > 0) cmd_append(' ');
                cmd_append('"');
                while (*p) {
                    if (*p == '"') cmd_append('\\');
                    cmd_append(*p);
                    p++;
                }
                cmd_append('"');
            }
            cmd_append(0);
#           undef cmd_append
        }
        if (envp != NULL) {
            if (*envp == NULL) {
                env = (char *)tmp_alloc_zero(2);
            }
            else {
                char ** p = envp;
                size_t env_size = 1;
                char * s = NULL;
                env_path = getenv("PATH");
                if (env_path == NULL) env_path = "";
                env_path = tmp_strdup(env_path);
                while (*p != NULL) env_size += strlen(*p++) + 1;
                s = env = (char *)tmp_alloc(env_size);
                for (p = envp; *p != NULL; p++) {
                    size_t l = strlen(*p) + 1;
                    if (strncmp(*p, "PATH=", 5) == 0) setenv("PATH", *p + 5, 1);
                    memcpy(s, *p, l);
                    s += l;
                }
                *s++ = 0;
                assert((size_t)(s - env) == env_size);
            }
        }
        /* If 'exe' is not a full file path, use PATH to search for program file */
        if (stat(exe, &st) != 0) fnm = NULL;
        if (CreateProcess(fnm, cmd, NULL, NULL, TRUE,
                (params->attach ? CREATE_SUSPENDED : 0),
                (LPVOID)env, dir, &si, &prs_info) == 0)
        {
            err = set_win32_errno(GetLastError());
        }
        else {
            pid = prs_info.dwProcessId;
            if (!CloseHandle(prs_info.hThread)) err = set_win32_errno(GetLastError());
            if (!CloseHandle(prs_info.hProcess)) err = set_win32_errno(GetLastError());
        }
        if (env_path != NULL) setenv("PATH", env_path, 1);
    }
    if (fpipes[0][0] >= 0 && close(fpipes[0][0]) < 0 && !err) err = errno;
    if (fpipes[1][1] >= 0 && close(fpipes[1][1]) < 0 && !err) err = errno;
    if (fpipes[2][1] >= 0 && close(fpipes[2][1]) < 0 && !err) err = errno;
    if (!err) {
        *prs = (ChildProcess *)loc_alloc_zero(sizeof(ChildProcess));
        (*prs)->pid = pid;
        (*prs)->tty = -1;
        (*prs)->bcg = c->bcg;
        broadcast_group_lock(c->bcg);
        strlcpy((*prs)->service, params->service, sizeof((*prs)->service));
        (*prs)->inp_struct = write_process_input(*prs, fpipes[0][1]);
        (*prs)->out_struct = read_process_output(*prs, fpipes[1][0]);
        (*prs)->err_struct = read_process_output(*prs, fpipes[2][0]);
        list_add_first(&(*prs)->link, &prs_list);
    }
    else {
        if (fpipes[0][1] >= 0) close(fpipes[0][1]);
        if (fpipes[1][0] >= 0) close(fpipes[1][0]);
        if (fpipes[2][0] >= 0) close(fpipes[2][0]);
    }
    if (!err) return 0;
    trace(LOG_ALWAYS, "Can't start process '%s': %s", exe, errno_to_str(err));
    errno = err;
    return -1;
}

#elif defined(_WRS_KERNEL)

static void task_create_hook(WIND_TCB * tcb) {
    ChildProcess * prs;

    semTake(prs_list_lock, WAIT_FOREVER);
    prs = find_process(taskIdSelf());
    /* TODO: vxWork: standard IO inheritance */
    semGive(prs_list_lock);
}

static void task_delete_hook(WIND_TCB * tcb) {
    int i;
    ChildProcess * prs;

    semTake(prs_list_lock, WAIT_FOREVER);
    prs = find_process((UINT32)tcb);
    if (prs != NULL) {
        close(ioTaskStdGet(prs->pid, 1));
        close(ioTaskStdGet(prs->pid, 2));
        for (i = 0; i < 2; i++) {
            char pnm[32];
            snprintf(pnm, sizeof(pnm), "/pty/tcf-%0*lx-%d", sizeof(prs->pid) * 2, prs->pid, i);
            ptyDevRemove(pnm);
        }
    }
    semGive(prs_list_lock);
}

static int start_process_imp(Channel * c, char ** envp, const char * dir, const char * exe, char ** args,
                  ProcessStartParams * params, int * selfattach, ChildProcess ** prs) {
    int err = 0;
    char * ptr;
    SYM_TYPE type;

    if (symFindByName(sysSymTbl, (char *)exe, &ptr, &type) != OK) {
        err = errno;
        if (err == S_symLib_SYMBOL_NOT_FOUND) err = ERR_SYM_NOT_FOUND;
        assert(err != 0);
    }
    else {
        int i;
        int pipes[2][2];
        /* TODO: arguments, environment */
        int pid = taskCreate("tTcf", 100, 0, 0x4000, (FUNCPTR)ptr, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
        for (i = 0; i < 2; i++) {
            char pnm[32];
            char pnm_m[32];
            char pnm_s[32];
            snprintf(pnm, sizeof(pnm), "/pty/tcf-%0*lx-%d", sizeof(pid) * 2, pid, i);
            snprintf(pnm_m, sizeof(pnm_m), "%sM", pnm);
            snprintf(pnm_s, sizeof(pnm_m), "%sS", pnm);
            if (ptyDevCreate(pnm, PIPE_SIZE, PIPE_SIZE) == ERROR) {
                err = errno;
                break;
            }
            pipes[i][0] = open(pnm_m, O_RDWR, 0);
            pipes[i][1] = open(pnm_s, O_RDWR, 0);
            if (pipes[i][0] < 0 || pipes[i][1] < 0) {
                err = errno;
                break;
            }
        }
        if (err) {
            taskDelete(pid);
            pid = 0;
        }
        else {
            semTake(prs_list_lock, WAIT_FOREVER);
            ioTaskStdSet(pid, 0, pipes[0][1]);
            ioTaskStdSet(pid, 1, pipes[0][1]);
            ioTaskStdSet(pid, 2, pipes[1][1]);
            *prs = loc_alloc_zero(sizeof(ChildProcess));
            (*prs)->pid = pid;
            (*prs)->tty = -1;
            (*prs)->bcg = c->bcg;
            broadcast_group_lock(c->bcg);
            strlcpy((*prs)->service, params->service, sizeof((*prs)->service));
            (*prs)->inp_struct = write_process_input(*prs, pipes[0][0]);
            (*prs)->out_struct = read_process_output(*prs, pipes[0][0]);
            (*prs)->err_struct = read_process_output(*prs, pipes[1][0]);
            list_add_first(&(*prs)->link, &prs_list);
            if (params->attach) {
                taskStop(pid);
                taskActivate(pid);
                assert(taskIsStopped(pid));
            }
            else {
                taskActivate(pid);
            }
            semGive(prs_list_lock);
        }
    }
    if (!err) return 0;
    errno = err;
    return -1;
}

#else

static int start_process_imp(Channel * c, char ** envp, const char * dir, const char * exe, char ** args,
                  ProcessStartParams * params, int * selfattach, ChildProcess ** prs) {
    int err = 0;
    int pid = 0;
    int p_log[2];

    if (pipe(p_log) < 0) return -1;

    if (!params->use_terminal) {
        int p_inp[2];
        int p_out[2];
        int p_err[2];

        p_inp[0] = p_inp[1] = -1;
        p_out[0] = p_out[1] = -1;
        p_err[0] = p_err[1] = -1;
        if (pipe(p_inp) < 0 || pipe(p_out) < 0 || pipe(p_err) < 0) err = errno;

        if (err == 0 && (p_inp[0] < 3 || p_out[1] < 3 || p_err[1] < 3)) {
            int fd0 = p_inp[0];
            int fd1 = p_out[1];
            int fd2 = p_err[1];
            if ((p_inp[0] = dup(p_inp[0])) < 0 ||
                (p_out[1] = dup(p_out[1])) < 0 ||
                (p_err[1] = dup(p_err[1])) < 0 ||
                close(fd0) < 0 ||
                close(fd1) < 0 ||
                close(fd2) < 0) err = errno;
        }

        if (!err) {
            pid = fork();
            if (pid < 0) err = errno;
            if (pid == 0) {
                int fd = -1;
                int fd_flags = 0;

                if (close(p_log[0]) < 0 && !err) err = errno;
                if (!err && (fd_flags = fcntl(p_log[1], F_GETFD)) < 0) err = errno;
                fd_flags |= FD_CLOEXEC;
                if (!err && fcntl(p_log[1], F_SETFD, fd_flags) < 0) err = errno;

                if (!err && (fd = sysconf(_SC_OPEN_MAX)) < 0) err = errno;
                if (!err && dup2(p_inp[0], 0) < 0) err = errno;
                if (!err && dup2(p_out[1], 1) < 0) err = errno;
                if (!err && dup2(p_err[1], 2) < 0) err = errno;
                while (!err && fd > 3 && fd - 1 != p_log[1]) close(--fd);
                if (!err && params->attach && context_attach_self() < 0) err = errno;
                if (!err && params->dir != NULL && chdir(params->dir) < 0) err = errno;
#if defined(_POSIX_C_SOURCE)
                if (!err) {
                    sigset_t set;
                    sigemptyset(&set);
                    if (sigprocmask(SIG_SETMASK, &set, NULL) < 0) err = errno;
                }
#endif
                if (!err) {
                    if (envp != NULL) environ = envp;
                    execvp(exe, args);
                    err = errno;
                }
                if (write(p_log[1], &err, sizeof(err)) != sizeof(err)) exit(2);
                exit(1);
            }
        }
        if (!err) {
            *prs = (ChildProcess *)loc_alloc_zero(sizeof(ChildProcess));
            (*prs)->pid = pid;
            (*prs)->tty = -1;
            (*prs)->bcg = c->bcg;
            broadcast_group_lock(c->bcg);
            strlcpy((*prs)->service, params->service, sizeof((*prs)->service));
            (*prs)->inp_struct = write_process_input(*prs, p_inp[1]);
            (*prs)->out_struct = read_process_output(*prs, p_out[0]);
            (*prs)->err_struct = read_process_output(*prs, p_err[0]);
            list_add_first(&(*prs)->link, &prs_list);
        }
        else {
            if (p_inp[1] >= 0) close(p_inp[1]);
            if (p_out[0] >= 0) close(p_out[0]);
            if (p_err[0] >= 0) close(p_err[0]);
        }
        if ((close(p_inp[0]) < 0 || close(p_out[1]) < 0 || close(p_err[1]) < 0) && !err) err = errno;
    }
    else {
        int fd_tty_master = -1;
        int fd_tty_slave = -1;
        int fd_tty_out = -1;
        char * tty_slave_name = NULL;

#if defined(__sun__)
        /* Solaris: See STREAMS-based Pseudo-Terminal Subsystem */
        /* http://docs.oracle.com/cd/E18752_01/html/816-4855/termsub15-44781.html */
        fd_tty_master = open("/dev/ptmx", O_RDWR);
#else
        fd_tty_master = posix_openpt(O_RDWR | O_NOCTTY);
#endif
        if (fd_tty_master < 0 || grantpt(fd_tty_master) < 0 || unlockpt(fd_tty_master) < 0) err = errno;
        if (!err && (tty_slave_name = ptsname(fd_tty_master)) == NULL) err = EINVAL;
        if (!err && (fd_tty_slave = open(tty_slave_name, O_RDWR | O_NOCTTY)) < 0) err = errno;
        if (!err && (fd_tty_out = dup(fd_tty_master)) < 0) err = errno;

        if (!err) {
            pid = fork();
            if (pid < 0) err = errno;
            if (pid == 0) {
                int fd = -1;
                int fd_flags = 0;

                if (close(p_log[0]) < 0 && !err) err = errno;
                if (!err && (fd_flags = fcntl(p_log[1], F_GETFD)) < 0) err = errno;
                fd_flags |= FD_CLOEXEC;
                if (!err && fcntl(p_log[1], F_SETFD, fd_flags) < 0) err = errno;

                if (!err && setsid() < 0) err = errno;;
                if (!err && (fd = open(tty_slave_name, O_RDWR)) < 0) err = errno;
#if defined(__sun__)
                if (!err && (ioctl(fd, I_PUSH, "ptem")) < 0) err = errno;
                if (!err && (ioctl(fd, I_PUSH, "ldterm")) < 0) err = errno;
#endif
                while (!err && fd < 3) {
                    int fd0 = fd;
                    if ((fd = dup(fd)) < 0 || close(fd0)) err = errno;
                }

#if defined(TIOCSCTTY)
                if (!err && (ioctl(fd, TIOCSCTTY, NULL)) < 0) err = errno;
#endif
                if (!err && dup2(fd, 0) < 0) err = errno;
                if (!err && dup2(fd, 1) < 0) err = errno;
                if (!err && dup2(fd, 2) < 0) err = errno;
                if (!err && (fd = sysconf(_SC_OPEN_MAX)) < 0) err = errno;
                while (!err && fd > 3 && fd - 1 != p_log[1]) close(--fd);
                if (!err && params->attach && context_attach_self() < 0) err = errno;
                if (!err && params->dir != NULL && chdir(params->dir) < 0) err = errno;
#if defined(_POSIX_C_SOURCE)
                if (!err) {
                    sigset_t set;
                    sigemptyset(&set);
                    if (sigprocmask(SIG_SETMASK, &set, NULL) < 0) err = errno;
                }
#endif
                if (!err) {
                    if (envp != NULL) environ = envp;
                    execvp(exe, args);
                    err = errno;
                }
                if (write(p_log[1], &err, sizeof(err)) != sizeof(err)) exit(2);
                exit(1);
            }
        }
        if (!err) {
            *prs = (ChildProcess *)loc_alloc_zero(sizeof(ChildProcess));
            (*prs)->pid = pid;
            (*prs)->tty = fd_tty_master;
            (*prs)->bcg = c->bcg;
            broadcast_group_lock(c->bcg);
            strlcpy((*prs)->service, params->service, sizeof((*prs)->service));
            (*prs)->inp_struct = write_process_input(*prs, fd_tty_master);
            (*prs)->out_struct = read_process_output(*prs, fd_tty_out);
            list_add_first(&(*prs)->link, &prs_list);
        }
        else {
            if (fd_tty_master >= 0) close(fd_tty_master);
            if (fd_tty_out >= 0) close(fd_tty_out);
        }
        if (fd_tty_slave >= 0) close(fd_tty_slave);
    }

    if (close(p_log[1]) < 0 && !err) err = errno;
    if (!err) {
        int e = 0;
        int n = read(p_log[0], &e, sizeof(e));
        if (n < 0) err = errno;
        else if (n == sizeof(e)) err = e;
    }
    if (close(p_log[0]) < 0 && !err) err = errno;

    *selfattach = 1;
    if (!err) return 0;
    errno = err;
    return -1;
}

#endif

static void waitpid_listener(int pid, int exited, int exit_code, int signal, int event_code, int syscall, void * args) {
    if (exited) {
        ChildProcess * prs = find_process(pid);
        if (prs) {
            if (signal != 0) prs->exit_code = -signal;
            else prs->exit_code = exit_code;
            process_exited(prs);
        }
    }
}

static void init(void) {
    if (init_done) return;
    init_done = 1;
#if defined(_WRS_KERNEL)
    prs_list_lock = semMCreate(SEM_Q_PRIORITY);
    if (prs_list_lock == NULL) check_error(errno);
    if (taskCreateHookAdd((FUNCPTR)task_create_hook) != OK) check_error(errno);
    if (taskDeleteHookAdd((FUNCPTR)task_delete_hook) != OK) check_error(errno);
#endif /* defined(_WRS_KERNEL) */
    add_waitpid_listener(waitpid_listener, NULL);
}

int start_process(Channel * c, ProcessStartParams * params, int * selfattach, ChildProcess ** prs) {
    int err = 0;
    init();
    if (start_process_imp(c, params->envp, params->dir, params->exe,
        params->args, params, selfattach, prs) < 0) err = errno;
    if (*prs != NULL) {
        if (!params->attach || err) add_waitpid_process((*prs)->pid);
        strlcpy((*prs)->name, params->exe, sizeof((*prs)->name));
        (*prs)->exit_args = params->exit_args;
        (*prs)->exit_cb = params->exit_cb;
    }
    if (!err) {
#if defined(_WIN32) || defined(__CYGWIN__)
#elif defined(_WRS_KERNEL)
#else
        if ((*prs)->tty >= 0) {
            struct winsize size;
            memset(&size, 0, sizeof(struct winsize));
            if ((*prs)->tty < 0 || ioctl((*prs)->tty, TIOCGWINSZ, &size) < 0 || size.ws_col <= 0 || size.ws_row <= 0) {
                size.ws_col = 80;
                size.ws_row = 24;
            }
            (*prs)->ws_col = size.ws_col;
            (*prs)->ws_row = size.ws_row;
        }
#endif
    }
    if (!err) return 0;
    errno = err;
    return -1;
}

const char * get_process_stream_id(ChildProcess * prs, int stream) {
    switch (stream) {
    case 0:
        if (prs->inp_struct == NULL) return NULL;
        return prs->inp_struct->id;
    case 1:
        if (prs->out_struct == NULL) return NULL;
        return prs->out_struct->id;
    case 2:
        if (prs->err_struct == NULL) return NULL;
        return prs->err_struct->id;
    }
    return NULL;
}

int get_process_tty(ChildProcess * prs) {
    return prs->tty;
}

int get_process_pid(ChildProcess * prs) {
    return prs->pid;
}

int get_process_out_state(ChildProcess * prs) {
    return prs->got_output;
}

int get_process_exit_code(ChildProcess * prs) {
    return prs->exit_code;
}

int get_process_tty_win_size(ChildProcess * prs, unsigned * ws_col, unsigned * ws_row) {
    *ws_col = prs->ws_col;
    *ws_row = prs->ws_row;
    return 0;
}

int set_process_tty_win_size(ChildProcess * prs, unsigned ws_col, unsigned ws_row, int * changed) {
    if (changed) *changed = 0;
    if (prs->ws_col != ws_col || prs->ws_row != ws_row) {
#if defined(_WIN32) || defined(__CYGWIN__)
#elif defined(_WRS_KERNEL)
#else
        struct winsize size;
        if (prs->tty < 0) {
            set_errno(ERR_OTHER, "Process is not connected to sudo-terminal");
            return -1;
        }
        memset(&size, 0, sizeof(size));
        size.ws_col = ws_col;
        size.ws_row = ws_row;
        if (ioctl(prs->tty, TIOCSWINSZ, &size) < 0) return -1;
#endif
        prs->ws_col = ws_col;
        prs->ws_row = ws_row;
        if (changed) *changed = 1;
    }
    return 0;
}

#if SERVICE_Processes

static void read_start_params(InputStream * inp, const char * nm, void * arg) {
    ProcessStartParams * params = (ProcessStartParams *)arg;
    if (strcmp(nm, "Attach") == 0) params->attach = json_read_boolean(inp);
    else if (strcmp(nm, "AttachChildren") == 0) params->attach_mode |= json_read_boolean(inp) ? CONTEXT_ATTACH_CHILDREN : 0;
    else if (strcmp(nm, "StopAtEntry") == 0) params->attach_mode |= json_read_boolean(inp) ? 0 : CONTEXT_ATTACH_NO_STOP;
    else if (strcmp(nm, "StopAtMain") == 0) params->attach_mode |= json_read_boolean(inp) ? 0 : CONTEXT_ATTACH_NO_MAIN;
    else if (strcmp(nm, "UseTerminal") == 0) params->use_terminal = json_read_boolean(inp);
#if ENABLE_DebugContext
    else if (strcmp(nm, "SigDontStop") == 0) read_sigset(inp, &params->sig_dont_stop, &params->set_dont_stop);
    else if (strcmp(nm, "SigDontPass") == 0) read_sigset(inp, &params->sig_dont_pass, &params->set_dont_pass);
#endif
    else json_skip_object(inp);
}

static void command_start(char * token, Channel * c, void * x) {
    int err = 0;
    int version = *(int *)x;
    int args_len = 0;
    int envp_len = 0;
    ProcessStartParams params;
    int selfattach = 0;
    int pending = 0;
    ChildProcess * prs = NULL;
    Trap trap;

    memset(&params, 0, sizeof(params));

    if (set_trap(&trap)) {
        params.dir = json_read_alloc_string(&c->inp);
        json_test_char(&c->inp, MARKER_EOA);
        params.exe = json_read_alloc_string(&c->inp);
        json_test_char(&c->inp, MARKER_EOA);
        params.args = json_read_alloc_string_array(&c->inp, &args_len);
        json_test_char(&c->inp, MARKER_EOA);
        params.envp = json_read_alloc_string_array(&c->inp, &envp_len);
        json_test_char(&c->inp, MARKER_EOA);
        if (version > 0 && (json_peek(&c->inp) == '{' || json_peek(&c->inp) == 'n')) {
            json_read_struct(&c->inp, read_start_params, &params);
        }
        else {
            params.attach = json_read_boolean(&c->inp);
        }
        json_test_char(&c->inp, MARKER_EOA);
        json_test_char(&c->inp, MARKER_EOM);

        if (params.dir != NULL && params.dir[0] == 0) {
            loc_free(params.dir);
            params.dir = NULL;
        }

        params.service = PROCESSES[version];
        if (err == 0 && start_process(c, &params, &selfattach, &prs) < 0) err = errno;
        if (!err && params.attach) {
            int mode = params.attach_mode;
            AttachDoneArgs * data = (AttachDoneArgs *)loc_alloc_zero(sizeof *data);
            data->c = c;
            strlcpy(data->token, token, sizeof(data->token));
            data->set_dont_stop = params.set_dont_stop;
            data->set_dont_pass = params.set_dont_pass;
            sigset_copy(&data->sig_dont_stop, &params.sig_dont_stop);
            sigset_copy(&data->sig_dont_pass, &params.sig_dont_pass);
            if (selfattach) mode |= CONTEXT_ATTACH_SELF;
            pending = context_attach(prs->pid, start_done, data, mode) == 0;
            if (pending) {
                channel_lock_with_msg(c, PROCESSES[0]);
            }
            else {
                err = errno;
                loc_free(data);
            }
        }
        if (!pending) {
            write_stringz(&c->out, "R");
            write_stringz(&c->out, token);
            write_errno(&c->out, err);
            if (err || prs->pid == 0) {
                write_stringz(&c->out, "null");
            }
            else {
                write_context(&c->out, prs->pid);
                write_stream(&c->out, 0);
            }
            write_stream(&c->out, MARKER_EOM);
        }
        clear_trap(&trap);
    }

    sigset_clear(&params.sig_dont_stop);
    sigset_clear(&params.sig_dont_pass);
    loc_free(params.dir);
    loc_free(params.exe);
    loc_free(params.args);
    loc_free(params.envp);

    if (trap.error) exception(trap.error);
}

static void command_set_win_size(char * token, Channel * c) {
    int err = 0;
    char id[256];
    pid_t pid, parent;
    ChildProcess * prs = NULL;
    unsigned ws_col;
    unsigned ws_row;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    ws_col = json_read_ulong(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    ws_row = json_read_ulong(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    pid = id2pid(id, &parent);

    if (pid == 0 || parent != 0 || (prs = find_process(pid)) == NULL) {
        err = ERR_INV_CONTEXT;
    }
    else if (set_process_tty_win_size(prs, ws_col, ws_row, NULL) < 0) {
        err = errno;
    }

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);

}

static void command_get_capabilities(char * token, Channel * c) {
    char id[256];
    pid_t pid = 0, parent = 0;
    OutputStream * out = &c->out;
    int err = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (strlen(id) > 0) {
        pid = id2pid(id, &parent);
        if (find_process(pid) == NULL) {
            err = ERR_INV_CONTEXT;
        }
    }

    write_stringz(out, "R");
    write_stringz(out, token);
    write_errno(out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        write_stream(out, '{');
        json_write_string(out, "ID");
        write_stream(out, ':');
        json_write_string(out, id);
        write_stream(out, ',');
        json_write_string(out, "CanTerminate");
        write_stream(out, ':');
        json_write_boolean(out, 1);
#if ENABLE_DebugContext
        write_stream(out, ',');
        json_write_string(out, "CanTerminateAttached");
        write_stream(out, ':');
        json_write_boolean(out, 1);
#endif
        write_stream(out, '}');
        write_stream(out, 0);
    }

    write_stream(out, MARKER_EOM);
}

void ini_processes_service(Protocol * proto) {
    int v;
    static int vs[] = { 0, 1 };
    init();
    for (v = 0; v < 2; v++) {
        add_command_handler(proto, PROCESSES[v], "getContext", command_get_context);
        add_command_handler(proto, PROCESSES[v], "getChildren", command_get_children);
        add_command_handler(proto, PROCESSES[v], "terminate", command_terminate);
        add_command_handler(proto, PROCESSES[v], "signal", command_signal);
        add_command_handler(proto, PROCESSES[v], "getSignalList", command_get_signal_list);
        add_command_handler(proto, PROCESSES[v], "getEnvironment", command_get_environment);
        add_command_handler2(proto, PROCESSES[v], "start", command_start, vs + v);
#if ENABLE_DebugContext
        add_command_handler(proto, PROCESSES[v], "attach", command_attach);
        add_command_handler(proto, PROCESSES[v], "detach", command_detach);
        add_command_handler(proto, PROCESSES[v], "getSignalMask", command_get_signal_mask);
        add_command_handler(proto, PROCESSES[v], "setSignalMask", command_set_signal_mask);
#endif /* ENABLE_DebugContext */
    }
    add_command_handler(proto, PROCESSES[1], "setWinSize", command_set_win_size);
    add_command_handler(proto, PROCESSES[1], "getCapabilities", command_get_capabilities);
}
#endif /* SERVICE_Processes */

#endif
