/*
 * Copyright (c) 2018 Chen Jingpiao <chenjingpiao@gmail.com>
 * Copyright (c) 2019 Paul Chaignon <paul.chaignon@gmail.com>
 * Copyright (c) 2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include "ptrace.h"
#include <signal.h>
#include <sys/prctl.h>
#include <sys/wait.h>
#include <linux/audit.h>
#include <linux/filter.h>

#include "filter_seccomp.h"
#include "number_set.h"
#include "syscall.h"
#include "scno.h"

bool seccomp_filtering;
bool seccomp_before_sysentry;

#ifdef HAVE_LINUX_SECCOMP_H

# include <linux/seccomp.h>

# ifndef BPF_MAXINSNS
#  define BPF_MAXINSNS 4096
# endif

# define JMP_PLACEHOLDER_NEXT  ((unsigned char) -1)
# define JMP_PLACEHOLDER_TRACE ((unsigned char) -2)

# define SET_BPF(filter, code, jt, jf, k) \
	(*(filter) = (struct sock_filter) { code, jt, jf, k })

# define SET_BPF_STMT(filter, code, k) \
	SET_BPF(filter, code, 0, 0, k)

# define SET_BPF_JUMP(filter, code, k, jt, jf) \
	SET_BPF(filter, BPF_JMP | code, jt, jf, k)

struct audit_arch_t {
	unsigned int arch;
	unsigned int flag;
};

static const struct audit_arch_t audit_arch_vec[SUPPORTED_PERSONALITIES] = {
# if SUPPORTED_PERSONALITIES > 1
	PERSONALITY0_AUDIT_ARCH,
	PERSONALITY1_AUDIT_ARCH,
#  if SUPPORTED_PERSONALITIES > 2
	PERSONALITY2_AUDIT_ARCH,
#  endif
# endif
};

# ifdef ENABLE_COVERAGE_GCOV
extern void __gcov_flush(void);
# endif

static void ATTRIBUTE_NORETURN
check_seccomp_order_do_child(void)
{
	static const struct sock_filter filter[] = {
		/* return (nr == __NR_gettid) ? RET_TRACE : RET_ALLOW; */
		BPF_STMT(BPF_LD | BPF_W | BPF_ABS,
			 offsetof(struct seccomp_data, nr)),
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, __NR_gettid, 0, 1),
		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_TRACE),
		BPF_STMT(BPF_RET | BPF_K, SECCOMP_RET_ALLOW)
	};
	static const struct sock_fprog prog = {
		.len = ARRAY_SIZE(filter),
		.filter = (struct sock_filter *) filter
	};

	/* Get everything ready before PTRACE_TRACEME.  */
	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0)
		perror_func_msg_and_die("prctl(PR_SET_NO_NEW_PRIVS, 1");
	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog) < 0)
		perror_func_msg_and_die("prctl(PR_SET_SECCOMP)");
	int pid = getpid();

	if (ptrace(PTRACE_TRACEME, 0L, 0L, 0L) < 0) {
		/* Exit with a nonzero exit status.  */
		perror_func_msg_and_die("PTRACE_TRACEME");
	}

# ifdef ENABLE_COVERAGE_GCOV
	__gcov_flush();
# endif

	kill(pid, SIGSTOP);
	syscall(__NR_gettid);
	_exit(0);
}

static int
check_seccomp_order_tracer(int pid)
{
	unsigned int step;

	for (step = 0; ; ++step) {
		int status;

		for (;;) {
			long rc = waitpid(pid, &status, 0);
			if (rc < 0 && errno == EINTR)
				continue;
			if (rc == pid)
				break;
			/* Cannot happen.  */
			perror_func_msg("#%d: unexpected wait result %ld",
					step, rc);
			return pid;
		}

		if (WIFEXITED(status)) {
			/* The tracee is no more.  */
			pid = 0;

			int exitstatus = WEXITSTATUS(status);
			if (step == 5 && exitstatus == 0) {
				seccomp_filtering = true;
			} else {
				error_func_msg("#%d: unexpected exit status %u",
					       step, exitstatus);
			}
			break;
		}

		if (WIFSIGNALED(status)) {
			/* The tracee is no more.  */
			pid = 0;

			error_func_msg("#%d: unexpected signal %u",
				       step, WTERMSIG(status));
			break;
		}

		if (!WIFSTOPPED(status)) {
			/* Cannot happen.  */
			error_func_msg("#%d: unexpected wait status %#x",
				       step, status);
			break;
		}

		unsigned int event = (unsigned int) status >> 16;

		switch (WSTOPSIG(status)) {
		case SIGSTOP:
			if (step != 0) {
				error_func_msg("#%d: unexpected signal stop",
					       step);
				return pid;
			}
			if (ptrace(PTRACE_SETOPTIONS, pid, 0L,
				   PTRACE_O_TRACESYSGOOD|
				   PTRACE_O_TRACESECCOMP) < 0) {
				perror_func_msg("PTRACE_SETOPTIONS");
				return pid;
			}
			break;

		case SIGTRAP:
			if (event != PTRACE_EVENT_SECCOMP) {
				error_func_msg("#%d: unexpected trap %#x",
					       step, event);
				return pid;
			}

			switch (step) {
			case 1: /* Seccomp stop before entering gettid.  */
				seccomp_before_sysentry = true;
				break;
			case 2: /* Seccomp stop after entering gettid.  */
				if (!seccomp_before_sysentry)
					break;
				ATTRIBUTE_FALLTHROUGH;
			default:
				error_func_msg("#%d: unexpected seccomp stop",
					       step);
				return pid;
			}
			break;

		case SIGTRAP | 0x80:
			switch (step) {
			case 3: /* Exiting gettid.  */
			case 4: /* Entering exit_group.  */
				break;
			case 1: /* Entering gettid before seccomp stop.  */
				seccomp_before_sysentry = false;
				break;
			case 2: /* Entering gettid after seccomp stop.  */
				if (seccomp_before_sysentry)
					break;
				ATTRIBUTE_FALLTHROUGH;
			default:
				error_func_msg("#%d: unexpected syscall stop",
					       step);
				return pid;
			}
			break;

		default:
			error_func_msg("#%d: unexpected stop signal %#x",
				       step, WSTOPSIG(status));
			return pid;
		}

		if (ptrace(PTRACE_SYSCALL, pid, 0L, 0L) < 0) {
			/* Cannot happen.  */
			perror_func_msg("#%d: PTRACE_SYSCALL", step);
			break;
		}
	}

	return pid;
}

static void
check_seccomp_order(void)
{
	seccomp_filtering = false;

	int pid = fork();
	if (pid < 0) {
		perror_func_msg("fork");
		return;
	}

	if (pid == 0)
		check_seccomp_order_do_child();

	pid = check_seccomp_order_tracer(pid);
	if (pid) {
		kill(pid, SIGKILL);
		for (;;) {
			long rc = waitpid(pid, NULL, 0);
			if (rc < 0 && errno == EINTR)
				continue;
			break;
		}
	}
}

static bool
traced_by_seccomp(unsigned int scno, unsigned int p)
{
	if (is_number_in_set_array(scno, trace_set, p)
	    || sysent_vec[p][scno].sys_flags
	    & (TRACE_INDIRECT_SUBCALL | TRACE_SECCOMP_DEFAULT))
		return true;
	return false;
}

static void
check_bpf_program_size(void)
{
	unsigned int nb_insns = SUPPORTED_PERSONALITIES > 1 ? 1 : 0;

	/*
	 * Implements a simplified form of init_sock_filter()'s bytecode
	 * generation algorithm, to count the number of instructions that will
	 * be generated.
	 */
	for (int p = SUPPORTED_PERSONALITIES - 1;
	     p >= 0 && nb_insns < BPF_MAXINSNS; --p) {
		unsigned int nb_insns_personality = 0;
		unsigned int lower = UINT_MAX;

		nb_insns_personality++;
# if SUPPORTED_PERSONALITIES > 1
		nb_insns_personality++;
		if (audit_arch_vec[p].flag)
			nb_insns_personality += 3;
# endif

		for (unsigned int i = 0; i < nsyscall_vec[p]; ++i) {
			if (traced_by_seccomp(i, p)) {
				if (lower == UINT_MAX)
					lower = i;
				continue;
			}
			if (lower == UINT_MAX)
				continue;
			if (lower + 1 == i)
				nb_insns_personality++;
			else
				nb_insns_personality += 2;
			lower = UINT_MAX;
		}
		if (lower != UINT_MAX) {
			if (lower + 1 == nsyscall_vec[p])
				nb_insns_personality++;
			else
				nb_insns_personality += 2;
		}

		nb_insns_personality += 3;

		/*
		 * Within generated BPF programs, the origin and destination of
		 * jumps are always in the same personality section.  The
		 * largest jump is therefore the jump from the first
		 * instruction of the section to the last, to skip the
		 * personality and try to compare .arch to the next
		 * personality.
		 * If we have a personality section with more than 255
		 * instructions, the jump offset will overflow.  Such program
		 * is unlikely to happen, so we simply disable seccomp filter
		 * is such a case.
		 */
		if (nb_insns_personality > UCHAR_MAX) {
			debug_msg("seccomp filter disabled due to "
				  "possibility of overflow");
			seccomp_filtering = false;
			return;
		}
		nb_insns += nb_insns_personality;
	}

# if SUPPORTED_PERSONALITIES > 1
	nb_insns++;
# endif

	if (nb_insns > BPF_MAXINSNS) {
		debug_msg("seccomp filter disabled due to BPF program being "
			  "oversized (%u > %d)", nb_insns, BPF_MAXINSNS);
		seccomp_filtering = false;
	}
}

static void
check_seccomp_filter_properties(void)
{
	if (NOMMU_SYSTEM) {
		seccomp_filtering = false;
		return;
	}

	int rc = prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, NULL, 0, 0);
	seccomp_filtering = rc < 0 && errno != EINVAL;
	if (!seccomp_filtering)
		debug_func_perror_msg("prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER)");

	if (seccomp_filtering)
		check_bpf_program_size();
	if (seccomp_filtering)
		check_seccomp_order();
}

static void
dump_seccomp_bpf(const struct sock_filter *filter, unsigned short len)
{
	for (unsigned int i = 0; i < len; ++i) {
		switch (filter[i].code) {
		case BPF_LD | BPF_W | BPF_ABS:
			switch (filter[i].k) {
			case offsetof(struct seccomp_data, arch):
				error_msg("STMT(BPF_LDWABS, data->arch)");
				break;
			case offsetof(struct seccomp_data, nr):
				error_msg("STMT(BPF_LDWABS, data->nr)");
				break;
			default:
				error_msg("STMT(BPF_LDWABS, 0x%x)",
					  filter[i].k);
			}
			break;
		case BPF_RET | BPF_K:
			switch (filter[i].k) {
			case SECCOMP_RET_TRACE:
				error_msg("STMT(BPF_RET, SECCOMP_RET_TRACE)");
				break;
			case SECCOMP_RET_ALLOW:
				error_msg("STMT(BPF_RET, SECCOMP_RET_ALLOW)");
				break;
			default:
				error_msg("STMT(BPF_RET, 0x%x)", filter[i].k);
			}
			break;
		case BPF_JMP | BPF_JEQ | BPF_K:
			error_msg("JUMP(BPF_JEQ, %u, %u, %u)",
				  filter[i].jt, filter[i].jf,
				  filter[i].k);
			break;
		case BPF_JMP | BPF_JGE | BPF_K:
			error_msg("JUMP(BPF_JGE, %u, %u, %u)",
				  filter[i].jt, filter[i].jf,
				  filter[i].k);
			break;
		case BPF_JMP | BPF_JA:
			error_msg("JUMP(BPF_JA, %u)", filter[i].k);
			break;
		default:
			error_msg("STMT(0x%x, %u, %u, 0x%x)", filter[i].code,
				  filter[i].jt, filter[i].jf, filter[i].k);
		}
	}
}

static void
replace_jmp_placeholders(unsigned char *jmp_offset, unsigned char jmp_next,
			 unsigned char jmp_trace)
{
	switch (*jmp_offset) {
	case JMP_PLACEHOLDER_NEXT:
		*jmp_offset = jmp_next;
		break;
	case JMP_PLACEHOLDER_TRACE:
		*jmp_offset = jmp_trace;
		break;
	default:
		break;
	}
}

static unsigned short
bpf_syscalls_cmp(struct sock_filter *filter,
		 unsigned int lower, unsigned int upper)
{
	if (lower + 1 == upper) {
		/* if (nr == lower) return RET_TRACE; */
		SET_BPF_JUMP(filter, BPF_JEQ | BPF_K, lower,
			     JMP_PLACEHOLDER_TRACE, 0);
		return 1;
	} else {
		/* if (nr >= lower && nr < upper) return RET_TRACE; */
		SET_BPF_JUMP(filter, BPF_JGE | BPF_K, lower, 0, 1);
		SET_BPF_JUMP(filter + 1, BPF_JGE | BPF_K, upper, 0,
			     JMP_PLACEHOLDER_TRACE);
		return 2;
	}
}

static unsigned short
init_sock_filter(struct sock_filter *filter)
{
	/*
	 * Generated program looks like:
	 * if (arch == AUDIT_ARCH_A && nr >= flag) {
	 *	if (nr == 59)
	 *		return SECCOMP_RET_TRACE;
	 *	if (nr >= 321 && nr <= 323)
	 *		return SECCOMP_RET_TRACE;
	 *	...
	 *	return SECCOMP_RET_ALLOW;
	 * }
	 * if (arch == AUDIT_ARCH_A) {
	 *	...
	 * }
	 * if (arch == AUDIT_ARCH_B) {
	 *	...
	 * }
	 * return SECCOMP_RET_TRACE;
	 */
	unsigned short pos = 0;

# if SUPPORTED_PERSONALITIES > 1
	SET_BPF_STMT(&filter[pos++], BPF_LD | BPF_W | BPF_ABS,
		     offsetof(struct seccomp_data, arch));
# endif

	/*
	 * Personalities are iterated in reverse-order in the BPF program so
	 * that the x86 case is naturally handled.  On x86, the first and third
	 * personalities have the same arch identifier.  The third can be
	 * distinguished based on its associated syscall flag, so we check it
	 * first.  The only drawback here is that the first personality is more
	 * common, which may make the BPF program slower to match syscalls on
	 * average.
	 */
	for (int p = SUPPORTED_PERSONALITIES - 1; p >= 0; --p) {
		unsigned int lower = UINT_MAX;
		unsigned short start = pos, end;

# if SUPPORTED_PERSONALITIES > 1
		/* if (arch != audit_arch_vec[p].arch) goto next; */
		SET_BPF_JUMP(&filter[pos++], BPF_JEQ | BPF_K,
			     audit_arch_vec[p].arch, 0, JMP_PLACEHOLDER_NEXT);
# endif
		SET_BPF_STMT(&filter[pos++], BPF_LD | BPF_W | BPF_ABS,
			     offsetof(struct seccomp_data, nr));

# if SUPPORTED_PERSONALITIES > 1
		if (audit_arch_vec[p].flag) {
			/* if (nr < audit_arch_vec[p].flag) goto next; */
			SET_BPF_JUMP(&filter[pos++], BPF_JGE | BPF_K,
				     audit_arch_vec[p].flag, 2, 0);
			SET_BPF_STMT(&filter[pos++], BPF_LD | BPF_W | BPF_ABS,
				     offsetof(struct seccomp_data, arch));
			SET_BPF_JUMP(&filter[pos++], BPF_JA,
				     JMP_PLACEHOLDER_NEXT, 0, 0);
		}
# endif

		for (unsigned int i = 0; i < nsyscall_vec[p]; ++i) {
			if (traced_by_seccomp(i, p)) {
				if (lower == UINT_MAX)
					lower = i;
				continue;
			}
			if (lower == UINT_MAX)
				continue;
			pos += bpf_syscalls_cmp(filter + pos,
						lower | audit_arch_vec[p].flag,
						i | audit_arch_vec[p].flag);
			lower = UINT_MAX;
		}
		if (lower != UINT_MAX)
			pos += bpf_syscalls_cmp(filter + pos,
						lower | audit_arch_vec[p].flag,
						nsyscall_vec[p]
						| audit_arch_vec[p].flag);
		end = pos;

		/* if (nr >= max_nr) return RET_TRACE; */
		SET_BPF_JUMP(&filter[pos++], BPF_JGE | BPF_K,
			     nsyscall_vec[p] | audit_arch_vec[p].flag, 1, 0);

		SET_BPF_STMT(&filter[pos++], BPF_RET | BPF_K,
			     SECCOMP_RET_ALLOW);
		SET_BPF_STMT(&filter[pos++], BPF_RET | BPF_K,
			     SECCOMP_RET_TRACE);

		for (unsigned int i = start; i < end; ++i) {
			if (BPF_CLASS(filter[i].code) != BPF_JMP)
				continue;
			unsigned char jmp_next = pos - i - 1;
			unsigned char jmp_trace = pos - i - 2;
			replace_jmp_placeholders(&filter[i].jt, jmp_next,
						 jmp_trace);
			replace_jmp_placeholders(&filter[i].jf, jmp_next,
						 jmp_trace);
			if (BPF_OP(filter[i].code) == BPF_JA)
				filter[i].k = (unsigned int) jmp_next;
		}
	}

# if SUPPORTED_PERSONALITIES > 1
	/* Jumps conditioned on .arch default to this RET_TRACE. */
	SET_BPF_STMT(&filter[pos++], BPF_RET | BPF_K, SECCOMP_RET_TRACE);
# endif

	if (debug_flag)
		dump_seccomp_bpf(filter, pos);

	return pos;
}

void
init_seccomp_filter(void)
{
	struct sock_filter filter[BPF_MAXINSNS];
	unsigned short len;

	len = init_sock_filter(filter);

	struct sock_fprog prog = {
		.len = len,
		.filter = filter
	};

	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) < 0)
		perror_func_msg_and_die("prctl(PR_SET_NO_NEW_PRIVS)");

	if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER, &prog) < 0)
		perror_func_msg_and_die("prctl(PR_SET_SECCOMP, SECCOMP_MODE_FILTER)");
}

int
seccomp_filter_restart_operator(const struct tcb *tcp)
{
	if (exiting(tcp) && tcp->scno < nsyscall_vec[current_personality]
	    && traced_by_seccomp(tcp->scno, current_personality))
		return PTRACE_SYSCALL;
	return PTRACE_CONT;
}

#else /* !HAVE_LINUX_SECCOMP_H */

# warning <linux/seccomp.h> is not available, seccomp filtering is not supported

static void
check_seccomp_filter_properties(void)
{
	seccomp_filtering = false;
}

void
init_seccomp_filter(void)
{
}

int
seccomp_filter_restart_operator(const struct tcb *tcp)
{
	return PTRACE_SYSCALL;
}

#endif

void
check_seccomp_filter(void)
{
	/* Let's avoid enabling seccomp if all syscalls are traced. */
	seccomp_filtering = !is_complete_set_array(trace_set, nsyscall_vec,
						   SUPPORTED_PERSONALITIES);
	if (!seccomp_filtering) {
		error_msg("Seccomp filter is requested "
			  "but there are no syscalls to filter.  "
			  "See -e trace to filter syscalls.");
		return;
	}

	check_seccomp_filter_properties();

	if (!seccomp_filtering)
		error_msg("seccomp filter is requested but unavailable");
}
