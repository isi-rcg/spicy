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
 *******************************************************************************/

/*
 * Agent main module.
 */

#include <tcf/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <signal.h>
#include <tcf/framework/mdep-threads.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/events.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/framework/channel_tcp.h>
#include <tcf/framework/plugins.h>
#include <tcf/services/discovery.h>
#include <tcf/http/http.h>
#include <tcf/main/test.h>
#include <tcf/main/cmdline.h>
#include <tcf/main/services.h>
#include <tcf/main/framework.h>
#include <tcf/main/gdb-rsp.h>
#include <tcf/main/server.h>
#include <tcf/main/main_hooks.h>

#ifndef ENABLE_SignalHandlers
#  define ENABLE_SignalHandlers 1
#endif

#ifndef DEFAULT_SERVER_URL
#  define DEFAULT_SERVER_URL "TCP:"
#endif

/* Hook before all TCF initialization.  This hook can add local variables. */
#ifndef PRE_INIT_HOOK
#define PRE_INIT_HOOK do {} while(0)
#endif

/* Hook before any TCF threads are created.  This hook can do
 * initialization that will affect all threads and call most basic TCF
 * functions, like post_event. */
#ifndef PRE_THREADING_HOOK
#define PRE_THREADING_HOOK do {} while(0)
#endif

/* Hook before becoming a daemon process.  This hook can output
 * banners and other information. */
#ifndef PRE_DAEMON_HOOK
#define PRE_DAEMON_HOOK do {} while(0)
#endif

/* Hook to add help text. */
#ifndef HELP_TEXT_HOOK
#define HELP_TEXT_HOOK
#endif

/* Hook for illegal option case.  This hook allows for handling off
 * additional options. */
#ifndef ILLEGAL_OPTION_HOOK
#define ILLEGAL_OPTION_HOOK  do {} while(0)
#endif

/* Signal handler. This hook extends behavior when process
 * exits due to received signal. */
#ifndef SIGNAL_HANDLER_HOOK
#define SIGNAL_HANDLER_HOOK do {} while(0)
#endif

/* Hook run immediatly after option parsing loop. */
#ifndef POST_OPTION_HOOK
#define POST_OPTION_HOOK do {} while(0)
#endif

/* Hook for USAGE string */
#ifndef USAGE_STRING_HOOK
#define USAGE_STRING_HOOK                               \
    "Usage: agent [OPTION]...",                         \
    "Start Target Communication Framework agent."
#endif

/* Hook before exiting main().  This hook can cleanup temporary
 * files, etc. */
#ifndef PRE_EXIT_HOOK
#define PRE_EXIT_HOOK do {} while(0)
#endif

static const char * progname;
static unsigned int idle_timeout;
static unsigned int idle_count;

static void check_idle_timeout(void * args) {
    if (list_is_empty(&client_connection_root)) {
        idle_count++;
        if (idle_count > idle_timeout) {
            trace(LOG_ALWAYS, "No connections for %d seconds, shutting down", idle_timeout);
            cancel_event_loop();
            return;
        }
    }
    post_event_with_delay(check_idle_timeout, NULL, 1000000);
}

static void channel_closed(Channel *c) {
    /* Reset idle_count if there are short lived connections */
    idle_count = 0;
}

#if ENABLE_SignalHandlers

static void shutdown_event(void * args) {
    cancel_event_loop();
}

static void signal_handler(int sig) {
    SIGNAL_HANDLER_HOOK;
    if (sig == SIGINT || sig == SIGTERM) {
        exit_event_loop();
    }
    else if (is_dispatch_thread()) {
        signal(sig, SIG_DFL);
        raise(sig);
    }
    else {
        post_event(shutdown_event, NULL);
    }
}

#if defined(_POSIX_C_SOURCE) && !defined(__MINGW32__)
static void * signal_handler_thread(void * arg) {
    int sig  = 0;
    sigset_t * set = (sigset_t *)arg;
    sigwait(set, &sig);
    exit_event_loop();
    return NULL;
}
#endif

#if defined(_WIN32) || defined(__CYGWIN__)
static LONG NTAPI VectoredExceptionHandler(PEXCEPTION_POINTERS x) {
    if (is_dispatch_thread()) {
        DWORD exception_code = x->ExceptionRecord->ExceptionCode;
        if (exception_code == EXCEPTION_IN_PAGE_ERROR) {
            int error = ERR_OTHER;
            if (x->ExceptionRecord->NumberParameters >= 3) {
                ULONG status = (ULONG)x->ExceptionRecord->ExceptionInformation[2];
                if (status != 0) error = set_nt_status_errno(status);
            }
            str_exception(error, "In page error");
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}

static BOOL CtrlHandler(DWORD ctrl) {
    switch(ctrl) {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        exit_event_loop();
        return TRUE;
    }
    return FALSE;
}
#endif

static void ini_signal_handlers(void) {
#if defined(_POSIX_C_SOURCE) && !defined(__MINGW32__)
    pthread_t thread;
    static sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    sigaddset(&set, SIGTERM);
    if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) check_error(errno);
    check_error(pthread_create(&thread, NULL, &signal_handler_thread, (void *)&set));
#else
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
#endif
    signal(SIGABRT, signal_handler);
    signal(SIGILL, signal_handler);
#if defined(_WIN32) || defined(__CYGWIN__)
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
    AddVectoredExceptionHandler(1, VectoredExceptionHandler);
#endif
}

#endif /* ENABLE_SignalHandlers */

#if !defined(_WRS_KERNEL)
static const char * help_text[] = {
    USAGE_STRING_HOOK,
    "  -d               run in daemon mode (output is sent to system logger)",
#if ENABLE_Cmdline
    "  -i               run in interactive mode",
#endif
    "  -L<file>         enable logging, use -L- to send log to stderr",
#if ENABLE_Trace
    "  -l<level>        set log level, the level is comma separated list of:",
    "@",
#endif
    "  -s<url>          set agent listening port and protocol, default is " DEFAULT_SERVER_URL,
    "  -S               print server properties in Json format to stdout",
#if ENABLE_GdbRemoteSerialProtocol
    "  -g<port>         start GDB Remote Serial Protocol server at the specified TCP port",
#endif
    "  -I<idle-seconds> exit if there are no connections for the specified time",
#if ENABLE_Plugins
    "  -P<dir>          set agent plugins directory name",
#endif
#if ENABLE_HttpServer
    "  -H<dir>          add HTML directory name",
#endif
#if ENABLE_SSL
    "  -c               generate SSL certificate and exit",
#endif
    HELP_TEXT_HOOK
    NULL
};

static void show_help(void) {
    const char ** p = help_text;
    while (*p != NULL) {
        if (**p == '@') {
#if ENABLE_Trace
            struct trace_mode * tm = trace_mode_table;
            while (tm->mode != 0) {
                fprintf(stderr, "      %-12s %s (%#x)\n", tm->name, tm->description, tm->mode);
                tm++;
            }
#endif
            p++;
        }
        else {
            fprintf(stderr, "%s\n", *p++);
        }
    }
}
#endif

#if defined(_WRS_KERNEL)
int tcf(void);
int tcf(void) {
#else
int main(int argc, char ** argv) {
    int c;
    int ind;
    int daemon = 0;
    const char * log_name = NULL;
    const char * log_level = NULL;
#endif
    int interactive = 0;
    int print_server_properties = 0;
    unsigned url_cnt = 0;
    unsigned url_max = 0;
    const char ** url_arr = NULL;
    TCFBroadcastGroup * bcg = NULL;
    Protocol * proto = NULL;

    PRE_INIT_HOOK;
    ini_framework();
    PRE_THREADING_HOOK;

#if !defined(_WRS_KERNEL)
#if ENABLE_RCBP_TEST
    for (ind = 1; ind < argc; ind++) {
        char * s = argv[ind];
        if (*s++ != '-') break;
        if (*s++ == 't') {
            test_proc();
            exit(0);
        }
    }
#endif
#endif

#if ENABLE_SignalHandlers
    ini_signal_handlers();
#endif

#if defined(_WRS_KERNEL)

    progname = "tcf";
    open_log_file("-");
    log_mode = 0;

#else

    progname = argv[0];

    /* Parse arguments */
    for (ind = 1; ind < argc; ind++) {
        char * s = argv[ind];
        if (*s++ != '-') break;
        while (s && (c = *s++) != '\0') {
            switch (c) {
            case 'i':
                interactive = 1;
                break;

            case 'd':
#if defined(_WIN32) || defined(__CYGWIN__)
                /* For Windows the only way to detach a process is to
                 * create a new process, so we patch the -d option to
                 * -D for the second time we get invoked so we don't
                 * keep on creating new processes forever. */
                s[-1] = 'D';
                daemon = 2;
                break;

            case 'D':
#endif
                daemon = 1;
                break;

            case 'c':
                generate_ssl_certificate();
                exit(0);
                break;

            case 'S':
                print_server_properties = 1;
                break;

            case 'h':
                show_help();
                exit(0);

            case 'I':
#if ENABLE_Trace
            case 'l':
#endif
            case 'L':
            case 's':
#if ENABLE_GdbRemoteSerialProtocol
            case 'g':
#endif
#if ENABLE_Plugins
            case 'P':
#endif
#if ENABLE_HttpServer
            case 'H':
#endif
                if (*s == '\0') {
                    if (++ind >= argc) {
                        fprintf(stderr, "%s: error: no argument given to option '%c'\n", progname, c);
                        exit(1);
                    }
                    s = argv[ind];
                }
                switch (c) {
                case 'I':
                    idle_timeout = strtol(s, 0, 0);
                    break;

#if ENABLE_Trace
                case 'l':
                    log_level = s;
                    parse_trace_mode(log_level, &log_mode);
                    break;
#endif

                case 'L':
                    log_name = s;
                    break;

                case 's':
                    if (url_cnt >= url_max) url_arr = (const char **)loc_realloc((void *)url_arr, sizeof(const char *) * (url_max += 16));
                    url_arr[url_cnt++] = s;
                    break;

#if ENABLE_GdbRemoteSerialProtocol
                case 'g':
                    if (ini_gdb_rsp(s) < 0) {
                        fprintf(stderr, "Cannot create GDB server: %s\n", errno_to_str(errno));
                        exit(1);
                    }
                    break;
#endif

#if ENABLE_Plugins
                case 'P':
                    plugins_path = s;
                    break;
#endif
#if ENABLE_HttpServer
                case 'H':
                    {
                        struct stat st;
                        char * fnm = canonicalize_file_name(s);
                        if (fnm == NULL || stat(fnm, &st) < 0) {
                            fprintf(stderr, "%s: invalid option '-H %s': %s\n", progname, s, errno_to_str(errno));
                            free(fnm);
                            exit(1);
                        }
                        add_http_directory(fnm);
                        free(fnm);
                    }
                    break;
#endif
                }
                s = NULL;
                break;

            default:
                ILLEGAL_OPTION_HOOK;
                fprintf(stderr, "%s: error: illegal option '%c'\n", progname, c);
                show_help();
                exit(1);
            }
        }
    }

    if (url_cnt == 0) {
        if (url_cnt >= url_max) url_arr = (const char **)loc_realloc((void *)url_arr, sizeof(const char *) * (url_max += 16));
        url_arr[url_cnt++] = DEFAULT_SERVER_URL;
    }

    POST_OPTION_HOOK;

    if (daemon) {
#if defined(_WIN32) || defined(__CYGWIN__)
        become_daemon(daemon > 1 ? argv : NULL);
#else
        become_daemon();
#endif
    }
    open_log_file(log_name);

#endif

    bcg = broadcast_group_alloc();
    proto = protocol_alloc();

    /* The static services must be initialised before the plugins */
#if ENABLE_Cmdline
    if (interactive) ini_cmdline_handler(interactive, proto);
#else
    if (interactive) fprintf(stderr, "Warning: This version does not support interactive mode.\n");
#endif

    ini_services(proto, bcg);

#if !defined(_WRS_KERNEL)
    /* Reparse log level in case initialization cause additional
     * levels to be registered */
    if (log_level != NULL && parse_trace_mode(log_level, &log_mode) != 0) {
        fprintf(stderr, "Cannot parse log level: %s\n", log_level);
        exit(1);
    }
#endif

    {
        unsigned i;

        for (i = 0; i < url_cnt; i++) {
            if (ini_server(url_arr[i], proto, bcg) < 0) {
                fprintf(stderr, "Cannot create listening port: %s\n", errno_to_str(errno));
                exit(1);
            }
        }
        loc_free(url_arr);
        url_cnt = 0;
        url_max = 0;

        discovery_start();
    }

    if (print_server_properties) {
        ChannelServer * s;
        char * server_properties;
        assert(!list_is_empty(&channel_server_root));
        s = servlink2channelserverp(channel_server_root.next);
        server_properties = channel_peer_to_json(s->ps);
        printf("Server-Properties: %s\n", server_properties);
        fflush(stdout);
        trace(LOG_ALWAYS, "Server-Properties: %s", server_properties);
        loc_free(server_properties);
    }

    PRE_DAEMON_HOOK;
#if !defined(_WRS_KERNEL)
    if (daemon) close_out_and_err();
#endif

    if (idle_timeout != 0) {
        add_channel_close_listener(channel_closed);
        check_idle_timeout(NULL);
    }

    /* Process events - must run on the initial thread since ptrace()
     * returns ECHILD otherwise, thinking we are not the owner. */
    run_event_loop();

    discovery_stop();
#if ENABLE_Plugins
    plugins_destroy();
#endif /* ENABLE_Plugins */

    PRE_EXIT_HOOK;

    return 0;
}
