/*******************************************************************************
 * Copyright (c) 2008-2018 Wind River Systems, Inc. and others.
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
 *     Intel - implemented terminals service
 *******************************************************************************/

/*
 * TCF Terminals service implementation.
 */

#include <tcf/config.h>

#if SERVICE_Terminals

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
#include <tcf/services/streamsservice.h>
#include <tcf/services/processes.h>
#include <tcf/services/terminals.h>

#ifndef TERMINALS_NO_LOGIN
#  define TERMINALS_NO_LOGIN 1
#endif

static const char * TERMINALS = "Terminals";

#if defined(_WIN32) || defined(__CYGWIN__)
#  define TERM_LAUNCH_EXEC "cmd"
#  define TERM_LAUNCH_ARGS {TERM_LAUNCH_EXEC, NULL}
#else
#  include <sys/stat.h>
#  include <unistd.h>
#  if TERMINALS_NO_LOGIN
#    if defined(ANDROID)
#      define TERM_LAUNCH_EXEC "/system/bin/sh"
#    else
#      define TERM_LAUNCH_EXEC "/bin/bash"
#    endif
#    define TERM_LAUNCH_ARGS {TERM_LAUNCH_EXEC, "-l", NULL}
#    define TERM_EXIT_SIGNAL SIGHUP
#  else
#    define TERM_LAUNCH_EXEC "/bin/login"
#    define TERM_LAUNCH_ARGS {TERM_LAUNCH_EXEC, "-p", NULL}
#    define TERM_EXIT_SIGNAL SIGTERM
#  endif
#endif

#define TERM_PROP_DEF_SIZE 256

typedef struct Terminal {
    LINK link;
    TCFBroadcastGroup * bcg;
    ChildProcess * prs;

    char pty_type[TERM_PROP_DEF_SIZE];
    char encoding[TERM_PROP_DEF_SIZE];
    int terminated;

    Channel * channel;
} Terminal;

#define link2term(A)  ((Terminal *)((char *)(A) - offsetof(Terminal, link)))

static LINK terms_list = TCF_LIST_INIT(terms_list);

static Terminal * find_terminal(int pid) {
    LINK * qhp = &terms_list;
    LINK * qp = qhp->next;

    while (qp != qhp) {
        Terminal * term = link2term(qp);
        if (get_process_pid(term->prs) == pid) return term;
        qp = qp->next;
    }
    return NULL;
}

static char * tid2id(int tid) {
    static char s[64];
    char * p = s + sizeof(s);
    unsigned long n = (long)tid;
    *(--p) = 0;
    do {
        *(--p) = (char) (n % 10 + '0');
        n = n / 10;
    }
    while (n != 0);

    *(--p) = 'T';
    return p;
}

static int id2tid(const char * id) {
    int tid = 0;
    if (id == NULL) return 0;
    if (id[0] != 'T') return 0;
    if (id[1] == 0) return 0;
    tid = (unsigned) strtol(id + 1, (char **) &id, 10);
    if (id[0] != 0) return 0;
    return tid;
}

static void write_context(OutputStream * out, int tid) {
    Terminal * term = find_terminal(tid);
    const char * id = NULL;

    write_stream(out, '{');

    if (term != NULL) {
        unsigned ws_col = 0;
        unsigned ws_row = 0;
        get_process_tty_win_size(term->prs, &ws_col, &ws_row);

        json_write_string(out, "ProcessID");
        write_stream(out, ':');
        json_write_string(out, pid2id(get_process_pid(term->prs), 0));
        write_stream(out, ',');

        if (*term->pty_type) {
            json_write_string(out, "PtyType");
            write_stream(out, ':');
            json_write_string(out, term->pty_type);
            write_stream(out, ',');
        }

        if (*term->encoding) {
            json_write_string(out, "Encoding");
            write_stream(out, ':');
            json_write_string(out, term->encoding);
            write_stream(out, ',');
        }

        json_write_string(out, "Width");
        write_stream(out, ':');
        json_write_ulong(out, ws_col);
        write_stream(out, ',');

        json_write_string(out, "Height");
        write_stream(out, ':');
        json_write_ulong(out, ws_row);
        write_stream(out, ',');

        id = get_process_stream_id(term->prs, 0);
        if (id) {
            json_write_string(out, "StdInID");
            write_stream(out, ':');
            json_write_string(out, id);
            write_stream(out, ',');
        }
        id = get_process_stream_id(term->prs, 1);
        if (id) {
            json_write_string(out, "StdOutID");
            write_stream(out, ':');
            json_write_string(out, id);
            write_stream(out, ',');
        }
        id = get_process_stream_id(term->prs, 2);
        if (id) {
            json_write_string(out, "StdErrID");
            write_stream(out, ':');
            json_write_string(out, id);
            write_stream(out, ',');
        }
    }

    json_write_string(out, "ID");
    write_stream(out, ':');
    json_write_string(out, tid2id(tid));

    write_stream(out, '}');
}

static void send_event_terminal_exited(OutputStream * out, Terminal * term) {
    write_stringz(out, "E");
    write_stringz(out, TERMINALS);
    write_stringz(out, "exited");

    json_write_string(out, tid2id(get_process_pid(term->prs)));
    write_stream(out, 0);

    json_write_ulong(out, get_process_exit_code(term->prs));
    write_stream(out, 0);

    write_stream(out, MARKER_EOM);
}

static void send_event_terminal_win_size_changed(OutputStream * out, Terminal * term) {
    unsigned ws_col = 0;
    unsigned ws_row = 0;

    get_process_tty_win_size(term->prs, &ws_col, &ws_row);

    write_stringz(out, "E");
    write_stringz(out, TERMINALS);
    write_stringz(out, "winSizeChanged");

    json_write_string(out, tid2id(get_process_pid(term->prs)));
    write_stream(out, 0);

    json_write_long(out, ws_col);
    write_stream(out, 0);

    json_write_long(out, ws_row);
    write_stream(out, 0);

    write_stream(out, MARKER_EOM);
}

#if !defined(_WIN32) && !defined(__CYGWIN__)
static void kill_term_event(void * args) {
    Terminal * term = (Terminal *)args;
    int pid = get_process_pid(term->prs);
    post_event_with_delay(kill_term_event, term, 1000000);
    kill(pid, SIGKILL);
}
#endif

static int kill_term(Terminal * term) {
    int err = 0;
    int pid = get_process_pid(term->prs);

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
    int sig = get_process_out_state(term->prs) ? TERM_EXIT_SIGNAL : SIGKILL;
    if (kill(pid, sig) < 0) err = errno;
    if (!err && sig != SIGKILL) post_event_with_delay(kill_term_event, term, 10000000);
#endif
    term->terminated = 1;
    return err;
}

static void command_exit(char * token, Channel * c) {
    int err = 0;
    char id[256];
    unsigned tid;
    Terminal * term = NULL;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    tid = id2tid(id);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    if (tid == 0 || (term = find_terminal(tid)) == NULL) {
        err = ERR_INV_CONTEXT;
    }
    else {
        err = kill_term(term);
    }

    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void terminal_exited(void * args) {
    Terminal * term = (Terminal *)args;
    Trap trap;

    if (set_trap(&trap)) {
        send_event_terminal_exited(&term->bcg->out, term);
        clear_trap(&trap);
    }
    else {
        trace(LOG_ALWAYS, "Exception sending terminal exited event: %d %s",
            trap.error, errno_to_str(trap.error));
    }

    list_remove(&term->link);
    broadcast_group_unlock(term->bcg);
    channel_unlock_with_msg(term->channel, TERMINALS);
#if !defined(_WIN32) && !defined(__CYGWIN__)
    cancel_event(kill_term_event, term, 0);
#endif
    loc_free(term);
}

#if !defined(_WIN32) && !defined(__CYGWIN__)
/*
 * Set the environment variable "name" to the value "value". If the variable
 * exists already, override it or just skip.
 */
static void envp_add(char *** envp, const char * name, const char * value, int env_override) {
    int i = 0;
    size_t len = strlen(name);
    char ** env = *envp;

    assert(name);
    assert(value);

    if (env == NULL) {
        env = *envp = (char **)tmp_alloc_zero(sizeof(char *) * 2);
    }
    else {
        for (i = 0; env[i]; i++) {
            if (strncmp(env[i], name, len) == 0 && env[i][len] == '=') break;
        }
        if (env[i]) {
            /* override */
            if (!env_override) return;
        }
        else {
            /* new variable */
            env = *envp = (char **)tmp_realloc(env, sizeof(char *) * (i + 2));
            env[i + 1] = NULL;
        }
    }
    len += strlen(value) + 2;
    env[i] = (char *)tmp_alloc(len);
    snprintf(env[i], len, "%s=%s", name, value);
}

static void set_terminal_env(char *** envp, const char * pty_type,
        const char * encoding, const char * exe) {
#if TERMINALS_NO_LOGIN
    int i;
    char * value;
    const char * env_array[] = { "USER", "LOGNAME", "HOME", "PATH", NULL };
#endif

    if (*pty_type) envp_add(envp, "TERM", pty_type, 1);
    if (*encoding) envp_add(envp, "LANG", encoding, 1);
    envp_add(envp, "SHELL", exe, 1);

#if TERMINALS_NO_LOGIN
    i = 0;
    while (env_array[i]) {
        value = getenv(env_array[i]);
        if (value) envp_add(envp, env_array[i], value, 0);
        i++;
    }
#endif
}

#endif

static void command_get_context(char * token, Channel * c) {
    int err = 0;
    char id[256];
    int tid;
    Terminal * term = NULL;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    tid = id2tid(id);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);

    if (tid == 0 || (term = find_terminal(tid)) == NULL) {
        err = ERR_INV_CONTEXT;
    }

    write_errno(&c->out, err);
    if (term != NULL) {
        write_context(&c->out, tid);
        write_stream(&c->out, 0);
    }
    else {
        write_stringz(&c->out, "null");
    }
    write_stream(&c->out, MARKER_EOM);
}

static char ** read_env(InputStream * inp) {
    int i = 0;
    int len = 0;
    char ** env = json_read_alloc_string_array(inp, &len);
    char ** tmp = (char **)tmp_alloc_zero((len + 1) * sizeof(char *));

    json_test_char(inp, MARKER_EOA);
    if (env == NULL) return NULL;
    /* convert the env memory layout */
    for (i = 0; i < len; i++) tmp[i] = tmp_strdup(env[i]);
    loc_free(env);

    return tmp;
}

static void command_launch(char * token, Channel * c) {
    int err = 0;
    char encoding[TERM_PROP_DEF_SIZE];
    char pty_type[TERM_PROP_DEF_SIZE];
    const char * args[] = TERM_LAUNCH_ARGS;
    const char * exec = TERM_LAUNCH_EXEC;

    int selfattach = 0;
    ProcessStartParams prms;
    Terminal * term = (Terminal *)loc_alloc_zero(sizeof(Terminal));

    memset(&prms, 0, sizeof(prms));
    json_read_string(&c->inp, pty_type, sizeof(pty_type));
    json_test_char(&c->inp, MARKER_EOA);
    json_read_string(&c->inp, encoding, sizeof(encoding));
    json_test_char(&c->inp, MARKER_EOA);
    prms.envp = read_env(&c->inp);
    json_test_char(&c->inp, MARKER_EOM);

#if !defined(_WIN32) && !defined(__CYGWIN__)
    {
        struct stat st;
        if (err == 0 && stat(exec, &st) != 0) {
            err = errno;
            if (err == ENOENT) {
                static char fnm[FILE_PATH_SIZE];
                /* On some systems (e.g. Free DSB) bash is installed under /usr/local */
                assert(exec[0] == '/');
                snprintf(fnm, sizeof(fnm), "/usr/local%s", exec);
                if (stat(fnm, &st) == 0) {
                    args[0] = exec = fnm;
                    err = 0;
                }
            }
            if (err == ENOENT && strcmp(exec, "/bin/bash") == 0) {
                /* "bash" not found, try "sh" */
                const char * fnm = "/bin/sh";
                if (stat(fnm, &st) == 0) {
                    args[0] = exec = fnm;
                    err = 0;
                }
            }
            if (err) err = set_fmt_errno(err, "Cannot start %s", exec);
        }
    }
    set_terminal_env(&prms.envp, pty_type, encoding, exec);
    prms.dir = getenv("HOME");
    if (prms.dir) prms.dir = tmp_strdup(prms.dir);
#else
    {
        const char * home_drv = getenv("HOMEDRIVE");
        const char * home_dir = getenv("HOMEPATH");
        if (home_drv && home_dir) {
            prms.dir = tmp_strdup2(home_drv, home_dir);
        }
    }
#endif

    prms.exe = exec;
    prms.args = (char **)args;
    prms.service = TERMINALS;
    prms.use_terminal = 1;
    prms.exit_cb = terminal_exited;
    prms.exit_args = term;

    if (err == 0 && start_process(c, &prms, &selfattach, &term->prs) < 0) err = errno;

    if (!err) {
        term->bcg = c->bcg;
        broadcast_group_lock(c->bcg);
        channel_lock_with_msg(term->channel = c, TERMINALS);
        strlcpy(term->pty_type, pty_type, sizeof(term->pty_type));
        strlcpy(term->encoding, encoding, sizeof(term->encoding));
        list_add_first(&term->link, &terms_list);
        assert(find_terminal(get_process_pid(term->prs)) == term);
    }
    else {
        assert(term->prs == NULL);
        loc_free(term);
    }

    /* write result back */
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    if (err) {
        write_stringz(&c->out, "null");
    }
    else {
        write_context(&c->out, get_process_pid(term->prs));
        write_stream(&c->out, 0);
    }
    write_stream(&c->out, MARKER_EOM);
}

static void command_set_win_size(char * token, Channel * c) {
    int err = 0;
    char id[256];
    unsigned tid;
    Terminal * term = NULL;
    unsigned ws_col;
    unsigned ws_row;
    int changed = 0;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    ws_col = json_read_ulong(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    ws_row = json_read_ulong(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    tid = id2tid(id);

    if (tid == 0 || (term = find_terminal(tid)) == NULL) {
        err = ERR_INV_CONTEXT;
    }
    else if (set_process_tty_win_size(term->prs, ws_col, ws_row, &changed) < 0) {
        err = errno;
    }

    if (changed) send_event_terminal_win_size_changed(&term->bcg->out, term);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);

}

static void channel_close_listener(Channel * c) {
    LINK * l = NULL;

    for (l = terms_list.next; l != &terms_list;) {
        Terminal * term = link2term(l);
        l = l->next;
        if (term->channel == c && !term->terminated) {
            trace(LOG_ALWAYS, "Terminal is left launched: %s", tid2id(get_process_pid(term->prs)));
            kill_term(term);
        }
    }
}

void ini_terminals_service(Protocol * proto) {
    add_channel_close_listener(channel_close_listener);

    add_command_handler(proto, TERMINALS, "getContext", command_get_context);
    add_command_handler(proto, TERMINALS, "launch", command_launch);
    add_command_handler(proto, TERMINALS, "exit", command_exit);
    add_command_handler(proto, TERMINALS, "setWinSize", command_set_win_size);
}

#endif /* SERVICE_Terminals */
