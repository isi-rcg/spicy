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
 * Machine and OS dependent definitions for ptrace.
 */

#ifndef D_mdep_ptrace
#define D_mdep_ptrace

#include <sys/ptrace.h>

#ifndef USE_enum_ptrace_request
#  if defined(__GLIBC__)
#    define USE_enum_ptrace_request 1
#  else
#    define USE_enum_ptrace_request 0
#  endif
#endif
#if USE_enum_ptrace_request
#  define ptrace(req, pid, addr, data) (ptrace)((enum __ptrace_request)(req), pid, addr, data)
#endif

#if !defined(PTRACE_SETOPTIONS)
#  define PTRACE_SETOPTIONS       0x4200
#  define PTRACE_GETEVENTMSG      0x4201
#  define PTRACE_GETSIGINFO       0x4202
#  define PTRACE_SETSIGINFO       0x4203

#  define PTRACE_O_TRACESYSGOOD   0x00000001
#  define PTRACE_O_TRACEFORK      0x00000002
#  define PTRACE_O_TRACEVFORK     0x00000004
#  define PTRACE_O_TRACECLONE     0x00000008
#  define PTRACE_O_TRACEEXEC      0x00000010
#  define PTRACE_O_TRACEVFORKDONE 0x00000020
#  define PTRACE_O_TRACEEXIT      0x00000040

#  define PTRACE_EVENT_FORK       1
#  define PTRACE_EVENT_VFORK      2
#  define PTRACE_EVENT_CLONE      3
#  define PTRACE_EVENT_EXEC       4
#  define PTRACE_EVENT_VFORK_DONE 5
#  define PTRACE_EVENT_EXIT       6
#endif

#endif /* D_mdep_ptrace */
