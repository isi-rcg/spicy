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
 * TCF Processes - process control service.
 * Processes service provides access to the target OS's process information,
 * allows to start and terminate a process, and allows to attach and
 * detach a process for debugging. Debug services, like Memory and Run Control,
 * require a process to be attached before they can access it.
 */

#ifndef D_processes
#define D_processes

#include <tcf/framework/events.h>
#include <tcf/framework/protocol.h>

typedef struct ChildProcess ChildProcess;

typedef struct ProcessStartParams {
    char ** envp;
    const char * dir;
    const char * exe;
    char ** args;
    int attach;
    int attach_mode;
    int use_terminal;
    SigSet sig_dont_stop;
    SigSet sig_dont_pass;
    int set_dont_stop;
    int set_dont_pass;
    const char * service;
    EventCallBack * exit_cb;
    void * exit_args;
} ProcessStartParams;

extern int start_process(Channel * c, ProcessStartParams * params,
                         int * selfattach, ChildProcess ** prs);

extern const char * get_process_stream_id(ChildProcess * prs, int stream);
extern int get_process_tty(ChildProcess * prs);
extern int get_process_pid(ChildProcess * prs);
extern int get_process_out_state(ChildProcess * prs);
extern int get_process_exit_code(ChildProcess * prs);
extern int get_process_tty_win_size(ChildProcess * prs, unsigned * ws_col, unsigned * ws_row);
extern int set_process_tty_win_size(ChildProcess * prs, unsigned ws_col, unsigned ws_row, int * changed);


/*
 * Initialize process control service.
 */
extern void ini_processes_service(Protocol *);


#endif
