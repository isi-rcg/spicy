/*
 * Copyright (c) 2018 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2018-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "kill_save_errno.h"
#include "print_fields.h"
#include "ptrace.h"
#include "ptrace_syscall_info.h"
#include "scno.h"

#include <signal.h>
#include <sys/wait.h>

#include "xlat/ptrace_syscall_info_op.h"

bool ptrace_get_syscall_info_supported;

static int
kill_tracee(pid_t pid)
{
	return kill_save_errno(pid, SIGKILL);
}

#define FAIL	do { ptrace_stop = -1U; goto done; } while (0)

static const unsigned int expected_none_size =
	offsetof(struct ptrace_syscall_info, entry);
static const unsigned int expected_entry_size =
	offsetofend(struct ptrace_syscall_info, entry.args);
static const unsigned int expected_exit_size =
	offsetofend(struct ptrace_syscall_info, exit.is_error);
static const unsigned int expected_seccomp_size =
	offsetofend(struct ptrace_syscall_info, seccomp.ret_data);

/*
 * Test that PTRACE_GET_SYSCALL_INFO API is supported by the kernel, and
 * that the semantics implemented in the kernel matches our expectations.
 */
bool
test_ptrace_get_syscall_info(void)
{
	/*
	 * NOMMU provides no forks necessary for PTRACE_GET_SYSCALL_INFO test,
	 * leave the default unchanged.
	 */
#ifdef HAVE_FORK
	static const unsigned long args[][7] = {
		/* a sequence of architecture-agnostic syscalls */
		{
			__NR_chdir,
			(unsigned long) "",
			0xbad1fed1,
			0xbad2fed2,
			0xbad3fed3,
			0xbad4fed4,
			0xbad5fed5
		},
		{
			__NR_gettid,
			0xcaf0bea0,
			0xcaf1bea1,
			0xcaf2bea2,
			0xcaf3bea3,
			0xcaf4bea4,
			0xcaf5bea5
		},
		{
			__NR_exit_group,
			0,
			0xfac1c0d1,
			0xfac2c0d2,
			0xfac3c0d3,
			0xfac4c0d4,
			0xfac5c0d5
		}
	};
	const unsigned long *exp_args;

	int pid = fork();
	if (pid < 0)
		perror_func_msg_and_die("fork");

	if (pid == 0) {
		/* get the pid before PTRACE_TRACEME */
		pid = getpid();
		if (ptrace(PTRACE_TRACEME, 0L, 0L, 0L) < 0) {
			/* exit with a nonzero exit status */
			perror_func_msg_and_die("PTRACE_TRACEME");
		}
		kill(pid, SIGSTOP);
		for (unsigned int i = 0; i < ARRAY_SIZE(args); ++i) {
			syscall(args[i][0],
				args[i][1], args[i][2], args[i][3],
				args[i][4], args[i][5], args[i][6]);
		}
		/* unreachable */
		_exit(1);
	}

	const struct {
		unsigned int is_error;
		int rval;
	} *exp_param, exit_param[] = {
		{ 1, -ENOENT },	/* chdir */
		{ 0, pid }	/* gettid */
	};

	unsigned int ptrace_stop;

	for (ptrace_stop = 0; ; ++ptrace_stop) {
		struct ptrace_syscall_info info = {
			.op = 0xff	/* invalid PTRACE_SYSCALL_INFO_* op */
		};
		const size_t size = sizeof(info);
		int status;
		long rc = waitpid(pid, &status, 0);
		if (rc != pid) {
			/* cannot happen */
			kill_tracee(pid);
			perror_func_msg_and_die("#%d: unexpected wait result"
						" %ld", ptrace_stop, rc);
		}
		if (WIFEXITED(status)) {
			/* tracee is no more */
			pid = 0;
			if (WEXITSTATUS(status) == 0)
				break;
			debug_func_msg("#%d: unexpected exit status %u",
				       ptrace_stop, WEXITSTATUS(status));
			FAIL;
		}
		if (WIFSIGNALED(status)) {
			/* tracee is no more */
			pid = 0;
			debug_func_msg("#%d: unexpected signal %u",
				       ptrace_stop, WTERMSIG(status));
			FAIL;
		}
		if (!WIFSTOPPED(status)) {
			/* cannot happen */
			kill_tracee(pid);
			error_func_msg_and_die("#%d: unexpected wait status"
					       " %#x", ptrace_stop, status);
		}

		switch (WSTOPSIG(status)) {
		case SIGSTOP:
			if (ptrace_stop) {
				debug_func_msg("#%d: unexpected signal stop",
					       ptrace_stop);
				FAIL;
			}
			if (ptrace(PTRACE_SETOPTIONS, pid, 0L,
				   PTRACE_O_TRACESYSGOOD) < 0) {
				/* cannot happen */
				kill_tracee(pid);
				perror_func_msg_and_die("PTRACE_SETOPTIONS");
			}
			rc = ptrace(PTRACE_GET_SYSCALL_INFO, pid,
				    (void *) size, &info);
			if (rc < 0) {
				debug_perror_msg("PTRACE_GET_SYSCALL_INFO");
				FAIL;
			}
			if (rc < (long) expected_none_size
			    || info.op != PTRACE_SYSCALL_INFO_NONE
			    || !info.arch
			    || !info.instruction_pointer
			    || !info.stack_pointer) {
				debug_func_msg("signal stop mismatch");
				FAIL;
			}
			break;

		case SIGTRAP | 0x80:
			rc = ptrace(PTRACE_GET_SYSCALL_INFO, pid,
				    (void *) size, &info);
			if (rc < 0) {
				debug_perror_msg("#%d: PTRACE_GET_SYSCALL_INFO",
						 ptrace_stop);
				FAIL;
			}
			switch (ptrace_stop) {
			case 1: /* entering chdir */
			case 3: /* entering gettid */
			case 5: /* entering exit_group */
				exp_args = args[ptrace_stop / 2];
				if (rc < (long) expected_entry_size
				    || info.op != PTRACE_SYSCALL_INFO_ENTRY
				    || !info.arch
				    || !info.instruction_pointer
				    || !info.stack_pointer
				    || (info.entry.nr != exp_args[0])
				    || (info.entry.args[0] != exp_args[1])
				    || (info.entry.args[1] != exp_args[2])
				    || (info.entry.args[2] != exp_args[3])
				    || (info.entry.args[3] != exp_args[4])
				    || (info.entry.args[4] != exp_args[5])
				    || (info.entry.args[5] != exp_args[6])) {
					debug_func_msg("#%d: entry stop"
						       " mismatch",
						       ptrace_stop);
					FAIL;
				}
				break;
			case 2: /* exiting chdir */
			case 4: /* exiting gettid */
				exp_param = &exit_param[ptrace_stop / 2 - 1];
				if (rc < (long) expected_exit_size
				    || info.op != PTRACE_SYSCALL_INFO_EXIT
				    || !info.arch
				    || !info.instruction_pointer
				    || !info.stack_pointer
				    || info.exit.is_error != exp_param->is_error
				    || info.exit.rval != exp_param->rval) {
					debug_func_msg("#%d: exit stop"
						       " mismatch",
						       ptrace_stop);
					FAIL;
				}
				break;
			default:
				debug_func_msg("#%d: unexpected syscall stop",
					       ptrace_stop);
				FAIL;
			}
			break;

		default:
			debug_func_msg("#%d: unexpected stop signal %#x",
				       ptrace_stop, WSTOPSIG(status));
			FAIL;
		}

		if (ptrace(PTRACE_SYSCALL, pid, 0L, 0L) < 0) {
			/* cannot happen */
			kill_tracee(pid);
			perror_func_msg_and_die("PTRACE_SYSCALL");
		}
	}

done:
	if (pid) {
		kill_tracee(pid);
		waitpid(pid, NULL, 0);
		ptrace_stop = -1U;
	}

	ptrace_get_syscall_info_supported =
		ptrace_stop == ARRAY_SIZE(args) * 2;

	if (ptrace_get_syscall_info_supported)
		debug_msg("PTRACE_GET_SYSCALL_INFO works");
	else
		debug_msg("PTRACE_GET_SYSCALL_INFO does not work");
#endif /* HAVE_FORK */

	return ptrace_get_syscall_info_supported;
}

void
print_ptrace_syscall_info(struct tcb *tcp, kernel_ulong_t addr,
			  kernel_ulong_t user_len)
{
	struct ptrace_syscall_info info;
	kernel_ulong_t kernel_len = tcp->u_rval;
	kernel_ulong_t ret_len = MIN(user_len, kernel_len);
	kernel_ulong_t fetch_size = MIN(ret_len, expected_seccomp_size);

	if (!fetch_size || !tfetch_mem(tcp, addr, fetch_size, &info)) {
		printaddr(addr);
		return;
	}

	PRINT_FIELD_XVAL("{", info, op, ptrace_syscall_info_op,
			 "PTRACE_SYSCALL_INFO_???");
	if (fetch_size < offsetofend(struct ptrace_syscall_info, arch))
		goto printed;
	PRINT_FIELD_XVAL(", ", info, arch, audit_arch, "AUDIT_ARCH_???");

	if (fetch_size < offsetofend(struct ptrace_syscall_info,
				     instruction_pointer))
		goto printed;
	PRINT_FIELD_ADDR64(", ", info, instruction_pointer);

	if (fetch_size < offsetofend(struct ptrace_syscall_info, stack_pointer))
		goto printed;
	PRINT_FIELD_ADDR64(", ", info, stack_pointer);

	if (fetch_size < offsetofend(struct ptrace_syscall_info, entry.nr))
		goto printed;

	switch(info.op) {
		case PTRACE_SYSCALL_INFO_ENTRY:
		case PTRACE_SYSCALL_INFO_SECCOMP:
			PRINT_FIELD_U((info.op == PTRACE_SYSCALL_INFO_ENTRY
				       ? ", entry={" : ", seccomp={"),
				      info.entry, nr);
			for (unsigned int i = 0;
			     i < ARRAY_SIZE(info.entry.args); ++i) {
				const unsigned int i_size =
					offsetofend(struct ptrace_syscall_info,
						    entry.args[i]);
				if (fetch_size < i_size) {
					if (i)
						break;
					goto entry_printed;
				}
				tprintf(", %s%#" PRIx64,
					(i ? "" : "arg=["),
					(uint64_t) info.entry.args[i]);
			}
			tprints("]");
			if (info.op == PTRACE_SYSCALL_INFO_SECCOMP
			    && fetch_size >= expected_seccomp_size)
				PRINT_FIELD_U(", ", info.seccomp, ret_data);
entry_printed:
			tprints("}");
			break;
		case PTRACE_SYSCALL_INFO_EXIT:
			tprints(", exit={");
			if (fetch_size >= expected_exit_size
			    && info.exit.is_error) {
				PRINT_FIELD_ERR_D("", info.exit, rval);
			} else {
				PRINT_FIELD_D("", info.exit, rval);
			}
			if (fetch_size >= expected_exit_size)
				PRINT_FIELD_U(", ", info.exit, is_error);
			tprints("}");
			break;
	}

printed:
	tprints("}");
}
