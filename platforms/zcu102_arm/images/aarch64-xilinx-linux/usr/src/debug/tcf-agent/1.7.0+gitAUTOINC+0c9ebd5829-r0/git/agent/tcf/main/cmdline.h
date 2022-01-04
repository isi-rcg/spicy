/*******************************************************************************
 * Copyright (c) 2007, 2011 Wind River Systems, Inc. and others.
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
 * Command line interpreter.
 */

#ifndef D_cmdline
#define D_cmdline

#if ENABLE_Cmdline

#include <tcf/framework/protocol.h>

extern void cmdline_suspend(void);
extern void cmdline_resume(void);
/*
 * Mode can be either :
 *  0 = Non interactive script mode
 *  1 = Interactive mode
 *  2 = Single command mode
 */
extern void ini_cmdline_handler(int mode, Protocol * proto);
extern void set_single_command(int keep_alive, const char * host, const char * command);
extern void open_script_file(const char * script_name);

typedef int CmdLineHandler(char *);
extern int add_cmdline_cmd(const char * cmd_name, const char * cmd_desc, CmdLineHandler * hnd);
extern void done_cmdline_cmd(int error);

#else /* ENABLE_Cmdline */

#define cmdline_suspend() 0
#define cmdline_resume() 0

#endif /* ENABLE_Cmdline */

#endif /* D_cmdline */
