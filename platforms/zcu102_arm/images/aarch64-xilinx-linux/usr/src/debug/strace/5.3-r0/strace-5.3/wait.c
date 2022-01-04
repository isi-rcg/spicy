/*
 * Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 * Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 * Copyright (c) 1993-1996 Rick Sladkey <jrs@world.std.com>
 * Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 * Copyright (c) 2002-2004 Roland McGrath <roland@redhat.com>
 * Copyright (c) 2004 Ulrich Drepper <drepper@redhat.com>
 * Copyright (c) 2009-2013 Denys Vlasenko <dvlasenk@redhat.com>
 * Copyright (c) 2014-2015 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2014-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "ptrace.h"

#include "wait.h"

#include "xlat/wait4_options.h"
#include "xlat/ptrace_events.h"

static int
printstatus(int status)
{
	int exited = 0;

	/*
	 * Here is a tricky presentation problem.  This solution
	 * is still not entirely satisfactory but since there
	 * are no wait status constructors it will have to do.
	 */
	if (WIFSTOPPED(status)) {
		int sig = WSTOPSIG(status);
		tprintf("[{WIFSTOPPED(s) && WSTOPSIG(s) == %s%s}",
			sprintsigname(sig & 0x7f),
			sig & 0x80 ? " | 0x80" : "");
		status &= ~W_STOPCODE(sig);
	} else if (WIFSIGNALED(status)) {
		tprintf("[{WIFSIGNALED(s) && WTERMSIG(s) == %s%s}",
			sprintsigname(WTERMSIG(status)),
			WCOREDUMP(status) ? " && WCOREDUMP(s)" : "");
		status &= ~(W_EXITCODE(0, WTERMSIG(status)) | WCOREFLAG);
	} else if (WIFEXITED(status)) {
		tprintf("[{WIFEXITED(s) && WEXITSTATUS(s) == %d}",
			WEXITSTATUS(status));
		exited = 1;
		status &= ~W_EXITCODE(WEXITSTATUS(status), 0);
	}
#ifdef WIFCONTINUED
	else if (WIFCONTINUED(status)) {
		tprints("[{WIFCONTINUED(s)}");
		status &= ~W_CONTINUED;
	}
#endif
	else {
		tprintf("[%#x]", status);
		return 0;
	}

	if (status) {
		unsigned int event = (unsigned int) status >> 16;
		if (event) {
			tprints(" | ");
			printxval(ptrace_events, event, "PTRACE_EVENT_???");
			tprints(" << 16");
			status &= 0xffff;
		}
		if (status)
			tprintf(" | %#x", status);
	}
	tprints("]");

	return exited;
}

static int
printwaitn(struct tcb *const tcp,
	   void (*const print_rusage)(struct tcb *, kernel_ulong_t))
{
	if (entering(tcp)) {
		/* On Linux, kernel-side pid_t is typedef'ed to int
		 * on all arches. Also, glibc-2.8 truncates wait3 and wait4
		 * pid argument to int on 64bit arches, producing,
		 * for example, wait4(4294967295, ...) instead of -1
		 * in strace. We have to use int here, not long.
		 */
		int pid = tcp->u_arg[0];
		tprintf("%d, ", pid);
	} else {
		int status;

		/* status */
		if (tcp->u_rval == 0)
			printaddr(tcp->u_arg[1]);
		else if (!umove_or_printaddr(tcp, tcp->u_arg[1], &status))
			printstatus(status);
		/* options */
		tprints(", ");
		printflags(wait4_options, tcp->u_arg[2], "W???");
		if (print_rusage) {
			/* usage */
			tprints(", ");
			if (tcp->u_rval > 0)
				print_rusage(tcp, tcp->u_arg[3]);
			else
				printaddr(tcp->u_arg[3]);
		}
	}
	return 0;
}

SYS_FUNC(waitpid)
{
	return printwaitn(tcp, NULL);
}

SYS_FUNC(wait4)
{
	return printwaitn(tcp, printrusage);
}

#ifdef ALPHA
SYS_FUNC(osf_wait4)
{
	return printwaitn(tcp, printrusage32);
}
#endif

#include "xlat/waitid_types.h"

SYS_FUNC(waitid)
{
	if (entering(tcp)) {
		printxval(waitid_types, tcp->u_arg[0], "P_???");
		int pid = tcp->u_arg[1];
		tprintf(", %d, ", pid);
	} else {
		/* siginfo */
		printsiginfo_at(tcp, tcp->u_arg[2]);
		/* options */
		tprints(", ");
		printflags(wait4_options, tcp->u_arg[3], "W???");
		/* usage */
		tprints(", ");
		printrusage(tcp, tcp->u_arg[4]);
	}
	return 0;
}
