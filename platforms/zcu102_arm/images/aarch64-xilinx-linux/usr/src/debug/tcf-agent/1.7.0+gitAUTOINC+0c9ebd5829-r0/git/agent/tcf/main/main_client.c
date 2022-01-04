/*******************************************************************************
 * Copyright (c) 2007, 2012 Wind River Systems, Inc. and others.
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
 * TCF client main module.
 */

#include <tcf/config.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/events.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/channel.h>
#include <tcf/framework/protocol.h>
#include <tcf/framework/proxy.h>
#include <tcf/framework/plugins.h>
#include <tcf/services/discovery.h>
#include <tcf/main/cmdline.h>
#include <tcf/main/framework.h>

static const char * progname;
static Protocol * proto;

/*
 * main entry point for TCF client
 *
 * The client is a simple shell permitting communication with the TCF agent.
 * By default the client will run in interactive mode. The client accepts
 * 3 command line options:
 * -L <log_file>        : specify a log file
 * -l <log_mode>        : logging level see trace.c for more details
 * -S <script_file>     : script of commands to run - non-interactive mode
 */

#if defined(_WRS_KERNEL)
int tcf_client(void);
int tcf_client(void) {
#else
int main(int argc, char ** argv) {
    int c;
    int ind;
    const char * log_name = "-";
    const char * log_level = NULL;
#endif
#if ENABLE_Cmdline
    int keep_alive = 0;
    int mode = 1; /* interactive */
    const char * host_name = "localhost";
    const char * command = NULL;
    const char * script_name = NULL;
#endif

    log_mode = 0;

    ini_framework();

#if defined(_WRS_KERNEL)

    progname = "tcf";
    open_log_file("-");

#else

    progname = argv[0];

    /* Parse arguments */
    for (ind = 1; ind < argc; ind++) {
        const char * s = argv[ind];
        if (*s != '-') {
            break;
        }
        s++;
        while ((c = *s++) != '\0') {
            switch (c) {
#if ENABLE_Cmdline
            case 'd':
                keep_alive = 1;
                break;
#endif

            case 'l':
            case 'L':
            case 'S':
            case 'h':
            case 'c':
#if ENABLE_Plugins
            case 'P':
#endif
                if (*s == '\0') {
                    if (++ind >= argc) {
                        fprintf(stderr, "%s: error: no argument given to option '%c'\n", progname, c);
                        exit(1);
                    }
                    s = argv[ind];
                }
                switch (c) {
                case 'l':
                    log_level = s;
                    parse_trace_mode(log_level, &log_mode);
                    break;

                case 'L':
                    log_name = s;
                    break;

#if ENABLE_Cmdline
                case 'S':
                    script_name = s;
                    mode = 0;
                    break;

                case 'h':
                    host_name = s;
                    break;

                case 'c':
                    /* TODO: allow multiple -c options */
                    command = s;
                    mode = 2;
                    break;
#endif

#if ENABLE_Plugins
                case 'P':
                    plugins_path = s;
                    break;
#endif

                default:
                    fprintf(stderr, "%s: error: illegal option '%c'\n", progname, c);
                    exit(1);
                }
                s = "";
                break;

            default:
                fprintf(stderr, "%s: error: illegal option '%c'\n", progname, c);
                exit(1);
            }
        }
    }

#if ENABLE_Cmdline
    if (script_name != NULL && command != NULL) {
        fprintf(stderr, "%s: error: illegal option -S and -c are mutually exclusive\n", progname);
        exit(1);
    }
#endif

    open_log_file(log_name);

#endif

    discovery_start();

    proto = protocol_alloc();

#if ENABLE_Cmdline
    if (script_name != NULL) open_script_file(script_name);
    if (command != NULL) set_single_command(keep_alive, host_name, command);
    ini_cmdline_handler(mode, proto);
#endif

#if ENABLE_Plugins
    plugins_load(proto, NULL);
#endif

#if !defined(_WRS_KERNEL)
    /* Reparse log level in case initialization cause additional
     * levels to be registered */
    if (log_level != NULL && parse_trace_mode(log_level, &log_mode) != 0) {
        fprintf(stderr, "Cannot parse log level: %s\n", log_level);
        exit(1);
    }
#endif

    run_event_loop();
    return 0;
}
