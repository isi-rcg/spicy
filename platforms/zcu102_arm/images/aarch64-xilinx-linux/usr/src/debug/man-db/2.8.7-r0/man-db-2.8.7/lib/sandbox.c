/*
 * sandbox.c: Process sandboxing
 *  
 * Copyright (C) 2017 Colin Watson.
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Some of the syscall lists in this file come from systemd, whose
 * copyright/licensing statement is as follows.  Per LGPLv2.1 s. 3, I have
 * altered the original references to LGPLv2.1 to refer to GPLv2 instead.
 *
 * Copyright 2014 Lennart Poettering
 *
 * systemd is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * systemd is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with systemd; If not, see <https://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef HAVE_LIBSECCOMP
#  include <sys/ioctl.h>
#  include <sys/ipc.h>
#  include <sys/mman.h>
#  include <sys/prctl.h>
#  include <sys/shm.h>
#  include <sys/socket.h>
#  include <termios.h>
#  include <seccomp.h>
#endif /* HAVE_LIBSECCOMP */

#include "manconfig.h"

#include "error.h"

#include "sandbox.h"

struct man_sandbox {
#ifdef HAVE_LIBSECCOMP
	scmp_filter_ctx ctx;
	scmp_filter_ctx permissive_ctx;
#else /* !HAVE_LIBSECCOMP */
	char dummy;
#endif /* HAVE_LIBSECCOMP */
};

#ifdef HAVE_LIBSECCOMP
static int seccomp_filter_unavailable = 0;

static void gripe_seccomp_filter_unavailable (void)
{
	debug ("seccomp filtering requires a kernel configured with "
	       "CONFIG_SECCOMP_FILTER\n");
}

static bool search_ld_preload (const char *needle)
{
	const char *ld_preload_env;
	static char *ld_preload_file = NULL;

	ld_preload_env = getenv ("LD_PRELOAD");
	if (ld_preload_env && strstr (ld_preload_env, needle) != NULL)
		return true;

	if (!ld_preload_file) {
		int fd;
		struct stat st;
		char *mapped = NULL;

		fd = open ("/etc/ld.so.preload", O_RDONLY);
		if (fd >= 0 && fstat (fd, &st) >= 0 && st.st_size)
			mapped = mmap (NULL, st.st_size, PROT_READ,
				       MAP_PRIVATE | MAP_FILE, fd, 0);
		if (mapped) {
			ld_preload_file = xstrndup (mapped, st.st_size);
			munmap (mapped, st.st_size);
		} else
			ld_preload_file = xstrdup ("");
		if (fd >= 0)
			close (fd);
	}
	/* This isn't very accurate: /etc/ld.so.preload may contain
	 * comments.  On the other hand, glibc says "it should only be used
	 * for emergencies and testing".  File a bug if this is a problem
	 * for you.
	 */
	if (strstr (ld_preload_file, needle) != NULL)
		return true;

	return false;
}

/* Can we load a seccomp filter into this process?
 *
 * This guard allows us to call sandbox_load in code paths that may
 * conditionally do so again.
 */
static bool can_load_seccomp (void)
{
	const char *man_disable_seccomp;
	int seccomp_status;

	if (seccomp_filter_unavailable) {
		gripe_seccomp_filter_unavailable ();
		return false;
	}

	man_disable_seccomp = getenv ("MAN_DISABLE_SECCOMP");
	if (man_disable_seccomp && *man_disable_seccomp) {
		debug ("seccomp filter disabled by user request\n");
		return false;
	}

	/* Valgrind causes the child process to make some system calls we
	 * don't want to allow in general, so disable seccomp when running
	 * on Valgrind.
	 *
	 * The correct approach seems to be to either require valgrind.h at
	 * build-time or copy valgrind.h into this project and then use the
	 * RUNNING_ON_VALGRIND macro, but I'd really rather not add a
	 * build-dependency for this or take a copy of a >6000-line header
	 * file.  Since the goal of this is only to disable the seccomp
	 * filter under Valgrind, this will do for now.
	 */
	if (search_ld_preload ("/vgpreload")) {
		debug ("seccomp filter disabled while running under "
		       "Valgrind\n");
		return false;
	}

	seccomp_status = prctl (PR_GET_SECCOMP);

	if (seccomp_status == 0)
		return true;

	if (seccomp_status == -1) {
		if (errno == EINVAL)
			debug ("running kernel does not support seccomp\n");
		else
			debug ("unknown error getting seccomp status: %s\n",
			       strerror (errno));
	} else if (seccomp_status == 2)
		debug ("seccomp already enabled\n");
	else
		debug ("unknown return value from PR_GET_SECCOMP: %d\n",
		       seccomp_status);
	return false;
}
#endif /* HAVE_LIBSECCOMP */

#ifdef HAVE_LIBSECCOMP

#define SC_ALLOW(name) \
	do { \
		int nr = seccomp_syscall_resolve_name (name); \
		if (nr == __NR_SCMP_ERROR) \
			break; \
		if (seccomp_rule_add (ctx, SCMP_ACT_ALLOW, nr, 0) < 0) \
			error (FATAL, errno, "can't add seccomp rule"); \
	} while (0)

#define SC_ALLOW_ARG_1(name, cmp1) \
	do { \
		int nr = seccomp_syscall_resolve_name (name); \
		if (nr == __NR_SCMP_ERROR) \
			break; \
		if (seccomp_rule_add (ctx, SCMP_ACT_ALLOW, nr, 1, cmp1) < 0) \
			error (FATAL, errno, "can't add seccomp rule"); \
	} while (0)

#define SC_ALLOW_ARG_2(name, cmp1, cmp2) \
	do { \
		int nr = seccomp_syscall_resolve_name (name); \
		if (nr == __NR_SCMP_ERROR) \
			break; \
		if (seccomp_rule_add (ctx, SCMP_ACT_ALLOW, nr, \
				      2, cmp1, cmp2) < 0) \
			error (FATAL, errno, "can't add seccomp rule"); \
	} while (0)

/* Create a seccomp filter.
 *
 * If permissive is true, then the returned filter will allow limited file
 * creation (although not making executable files).  This obviously
 * constitutes less effective confinement, but it's necessary for some
 * subprocesses (such as groff) that need the ability to write to temporary
 * files.  Confining these further requires additional tools that can do
 * path-based filtering or similar, such as AppArmor.
 */
static scmp_filter_ctx make_seccomp_filter (int permissive)
{
	scmp_filter_ctx ctx;
	mode_t mode_mask = S_ISUID | S_ISGID | S_IXUSR | S_IXGRP | S_IXOTH;
	int create_mask = O_CREAT
#ifdef O_TMPFILE
		| O_TMPFILE
#endif /* O_TMPFILE */
		;

	debug ("initialising seccomp filter (permissive: %d)\n", permissive);
	ctx = seccomp_init (SCMP_ACT_ERRNO (EPERM));
	if (!ctx)
		error (FATAL, errno, "can't initialise seccomp filter");

	/* Allow sibling architectures for x86, since people sometimes mix
	 * and match architectures there for performance reasons.
	 */
	switch (seccomp_arch_native ()) {
		case SCMP_ARCH_X86:
			seccomp_arch_add (ctx, SCMP_ARCH_X86_64);
			seccomp_arch_add (ctx, SCMP_ARCH_X32);
			break;
		case SCMP_ARCH_X86_64:
			seccomp_arch_add (ctx, SCMP_ARCH_X86);
			seccomp_arch_add (ctx, SCMP_ARCH_X32);
			break;
		case SCMP_ARCH_X32:
			seccomp_arch_add (ctx, SCMP_ARCH_X86);
			seccomp_arch_add (ctx, SCMP_ARCH_X86_64);
			break;
	}

	/* This sandbox is intended to allow operations that might
	 * reasonably be needed in simple data-transforming pipes: it should
	 * allow the process to do most reasonable things to itself, to read
	 * and write data from and to already-open file descriptors, to open
	 * files in read-only mode, and to fork new processes with the same
	 * restrictions.  (If permissive is true, then it should also allow
	 * limited file creation; see the header comment above.)
	 *
	 * Since I currently know of no library with suitable syscall lists,
	 * the syscall lists here are taken from
	 * systemd:src/shared/seccomp-util.c, last updated from commit
	 * bca5a0eaccc849a669b4279e4bfcc6507083a07b (2019-08-01).
	 */

	/* systemd: SystemCallFilter=@default */
	SC_ALLOW ("clock_getres");
	SC_ALLOW ("clock_gettime");
	SC_ALLOW ("clock_nanosleep");
	SC_ALLOW ("execve");
	SC_ALLOW ("exit");
	SC_ALLOW ("exit_group");
	SC_ALLOW ("futex");
	SC_ALLOW ("get_robust_list");
	SC_ALLOW ("get_thread_area");
	SC_ALLOW ("getegid");
	SC_ALLOW ("getegid32");
	SC_ALLOW ("geteuid");
	SC_ALLOW ("geteuid32");
	SC_ALLOW ("getgid");
	SC_ALLOW ("getgid32");
	SC_ALLOW ("getgroups");
	SC_ALLOW ("getgroups32");
	SC_ALLOW ("getpgid");
	SC_ALLOW ("getpgrp");
	SC_ALLOW ("getpid");
	SC_ALLOW ("getppid");
	SC_ALLOW ("getresgid");
	SC_ALLOW ("getresgid32");
	SC_ALLOW ("getresuid");
	SC_ALLOW ("getresuid32");
	SC_ALLOW ("getrlimit");
	SC_ALLOW ("getsid");
	SC_ALLOW ("gettid");
	SC_ALLOW ("gettimeofday");
	SC_ALLOW ("getuid");
	SC_ALLOW ("getuid32");
	SC_ALLOW ("membarrier");
	SC_ALLOW ("nanosleep");
	SC_ALLOW ("pause");
	SC_ALLOW ("prlimit64");
	SC_ALLOW ("restart_syscall");
	SC_ALLOW ("rseq");
	SC_ALLOW ("rt_sigreturn");
	SC_ALLOW ("sched_yield");
	SC_ALLOW ("set_robust_list");
	SC_ALLOW ("set_thread_area");
	SC_ALLOW ("set_tid_address");
	SC_ALLOW ("set_tls");
	SC_ALLOW ("sigreturn");
	SC_ALLOW ("time");
	SC_ALLOW ("ugetrlimit");

	/* systemd: SystemCallFilter=@basic-io */
	SC_ALLOW ("_llseek");
	SC_ALLOW ("close");
	SC_ALLOW ("dup");
	SC_ALLOW ("dup2");
	SC_ALLOW ("dup3");
	SC_ALLOW ("lseek");
	SC_ALLOW ("pread64");
	SC_ALLOW ("preadv");
	SC_ALLOW ("preadv2");
	SC_ALLOW ("pwrite64");
	SC_ALLOW ("pwritev");
	SC_ALLOW ("pwritev2");
	SC_ALLOW ("read");
	SC_ALLOW ("readv");
	SC_ALLOW ("write");
	SC_ALLOW ("writev");

	/* systemd: SystemCallFilter=@file-system (subset) */
	SC_ALLOW ("access");
	SC_ALLOW ("chdir");
	if (permissive) {
		SC_ALLOW_ARG_1 ("chmod",
				SCMP_A1 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
		SC_ALLOW_ARG_1 ("creat",
				SCMP_A1 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
	}
	SC_ALLOW ("faccessat");
	SC_ALLOW ("fallocate");
	SC_ALLOW ("fchdir");
	if (permissive) {
		SC_ALLOW_ARG_1 ("fchmod",
				SCMP_A1 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
		SC_ALLOW_ARG_1 ("fchmodat",
				SCMP_A2 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
	}
	SC_ALLOW ("fcntl");
	SC_ALLOW ("fcntl64");
	SC_ALLOW ("fstat");
	SC_ALLOW ("fstat64");
	SC_ALLOW ("fstatat64");
	SC_ALLOW ("fstatfs");
	SC_ALLOW ("fstatfs64");
	SC_ALLOW ("ftruncate");
	SC_ALLOW ("ftruncate64");
	if (permissive) SC_ALLOW ("futimesat");
	SC_ALLOW ("getcwd");
	SC_ALLOW ("getdents");
	SC_ALLOW ("getdents64");
	if (permissive) SC_ALLOW ("link");
	if (permissive) SC_ALLOW ("linkat");
	SC_ALLOW ("lstat");
	SC_ALLOW ("lstat64");
	if (permissive) SC_ALLOW ("mkdir");
	if (permissive) SC_ALLOW ("mkdirat");
	SC_ALLOW ("mmap");
	SC_ALLOW ("mmap2");
	SC_ALLOW ("munmap");
	SC_ALLOW ("newfstatat");
	SC_ALLOW ("oldfstat");
	SC_ALLOW ("oldlstat");
	SC_ALLOW ("oldstat");
	if (permissive) {
		SC_ALLOW_ARG_2 ("open",
				SCMP_A1 (SCMP_CMP_MASKED_EQ, O_CREAT, O_CREAT),
				SCMP_A2 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
		SC_ALLOW_ARG_2 ("openat",
				SCMP_A2 (SCMP_CMP_MASKED_EQ, O_CREAT, O_CREAT),
				SCMP_A3 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
#ifdef O_TMPFILE
		SC_ALLOW_ARG_2 ("open",
				SCMP_A1 (SCMP_CMP_MASKED_EQ,
					 O_TMPFILE, O_TMPFILE),
				SCMP_A2 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
		SC_ALLOW_ARG_2 ("openat",
				SCMP_A2 (SCMP_CMP_MASKED_EQ,
					 O_TMPFILE, O_TMPFILE),
				SCMP_A3 (SCMP_CMP_MASKED_EQ, mode_mask, 0));
#endif /* O_TMPFILE */
		SC_ALLOW_ARG_1 ("open",
				SCMP_A1 (SCMP_CMP_MASKED_EQ, create_mask, 0));
		SC_ALLOW_ARG_1 ("openat",
				SCMP_A2 (SCMP_CMP_MASKED_EQ, create_mask, 0));
	} else {
		SC_ALLOW_ARG_1 ("open",
				SCMP_A1 (SCMP_CMP_MASKED_EQ, O_ACCMODE,
					 O_RDONLY));
		SC_ALLOW_ARG_1 ("openat",
				SCMP_A2 (SCMP_CMP_MASKED_EQ, O_ACCMODE,
					 O_RDONLY));
	}
	SC_ALLOW ("readlink");
	SC_ALLOW ("readlinkat");
	if (permissive) SC_ALLOW ("rename");
	if (permissive) SC_ALLOW ("renameat");
	if (permissive) SC_ALLOW ("renameat2");
	if (permissive) SC_ALLOW ("rmdir");
	SC_ALLOW ("stat");
	SC_ALLOW ("stat64");
	SC_ALLOW ("statfs");
	SC_ALLOW ("statfs64");
	SC_ALLOW ("statx");
	if (permissive) SC_ALLOW ("symlink");
	if (permissive) SC_ALLOW ("symlinkat");
	if (permissive) SC_ALLOW ("truncate");
	if (permissive) SC_ALLOW ("truncateat");
	if (permissive) SC_ALLOW ("unlink");
	if (permissive) SC_ALLOW ("unlinkat");
	if (permissive) SC_ALLOW ("utime");
	if (permissive) SC_ALLOW ("utimensat");
	if (permissive) SC_ALLOW ("utimes");

	/* systemd: SystemCallFilter=@io-event */
	SC_ALLOW ("_newselect");
	SC_ALLOW ("epoll_create");
	SC_ALLOW ("epoll_create1");
	SC_ALLOW ("epoll_ctl");
	SC_ALLOW ("epoll_ctl_old");
	SC_ALLOW ("epoll_pwait");
	SC_ALLOW ("epoll_wait");
	SC_ALLOW ("epoll_wait_old");
	SC_ALLOW ("eventfd");
	SC_ALLOW ("eventfd2");
	SC_ALLOW ("poll");
	SC_ALLOW ("ppoll");
	SC_ALLOW ("pselect6");
	SC_ALLOW ("select");

	/* systemd: SystemCallFilter=@ipc (subset) */
	SC_ALLOW ("pipe");
	SC_ALLOW ("pipe2");

	/* systemd: SystemCallFilter=@process (subset) */
	SC_ALLOW ("arch_prctl");
	SC_ALLOW ("capget");
	SC_ALLOW ("clone");
	SC_ALLOW ("execveat");
	SC_ALLOW ("fork");
	SC_ALLOW ("getrusage");
	SC_ALLOW ("prctl");
	SC_ALLOW ("vfork");
	SC_ALLOW ("wait4");
	SC_ALLOW ("waitid");
	SC_ALLOW ("waitpid");

	/* systemd: SystemCallFilter=@signal */
	SC_ALLOW ("rt_sigaction");
	SC_ALLOW ("rt_sigpending");
	SC_ALLOW ("rt_sigprocmask");
	SC_ALLOW ("rt_sigsuspend");
	SC_ALLOW ("rt_sigtimedwait");
	SC_ALLOW ("sigaction");
	SC_ALLOW ("sigaltstack");
	SC_ALLOW ("signal");
	SC_ALLOW ("signalfd");
	SC_ALLOW ("signalfd4");
	SC_ALLOW ("sigpending");
	SC_ALLOW ("sigprocmask");
	SC_ALLOW ("sigsuspend");

	/* systemd: SystemCallFilter=@sync */
	SC_ALLOW ("fdatasync");
	SC_ALLOW ("fsync");
	SC_ALLOW ("msync");
	SC_ALLOW ("sync");
	SC_ALLOW ("sync_file_range");
	SC_ALLOW ("syncfs");

	/* systemd: SystemCallFilter=@system-service (subset) */
	SC_ALLOW ("brk");
	SC_ALLOW ("fadvise64");
	SC_ALLOW ("fadvise64_64");
	SC_ALLOW ("getrandom");
	if (permissive)
		SC_ALLOW ("ioctl");
	else {
		SC_ALLOW_ARG_1 ("ioctl", SCMP_A1 (SCMP_CMP_EQ, TCGETS));
		SC_ALLOW_ARG_1 ("ioctl", SCMP_A1 (SCMP_CMP_EQ, TIOCGWINSZ));
	}
	SC_ALLOW ("madvise");
	SC_ALLOW ("mprotect");
	SC_ALLOW ("mremap");
	SC_ALLOW ("sched_getaffinity");
	SC_ALLOW ("sysinfo");
	SC_ALLOW ("uname");

	/* Extra syscalls not in any of systemd's sets. */
	SC_ALLOW ("arm_fadvise64_64");
	SC_ALLOW ("arm_sync_file_range");
	SC_ALLOW ("sync_file_range2");

	/* Allow killing processes and threads.  This is unfortunate but
	 * unavoidable: groff uses kill to explicitly pass on SIGPIPE to its
	 * child processes, and we can't do any more sophisticated filtering
	 * in seccomp.
	 */
	SC_ALLOW ("kill");
	SC_ALLOW ("tgkill");

	/* Allow some relatively harmless System V shared memory operations.
	 * These seem to be popular among the sort of program that wants to
	 * install itself in /etc/ld.so.preload or similar (e.g. antivirus
	 * programs and VPNs).
	 */
	SC_ALLOW_ARG_1 ("shmat", SCMP_A2 (SCMP_CMP_EQ, SHM_RDONLY));
	SC_ALLOW_ARG_1 ("shmctl", SCMP_A1 (SCMP_CMP_EQ, IPC_STAT));
	SC_ALLOW ("shmdt");
	SC_ALLOW ("shmget");

	/* Some antivirus programs use an LD_PRELOAD wrapper that wants to
	 * talk to a private daemon using a Unix-domain socket.  We really
	 * don't want to allow these syscalls in general, but if such a
	 * thing is in use we probably have no choice.
	 *
	 * snoopy is an execve monitoring tool that may log messages to
	 * /dev/log.
	 */
	if (search_ld_preload ("libesets_pac.so") ||
	    search_ld_preload ("libscep_pac.so") ||
	    search_ld_preload ("libsnoopy.so")) {
		SC_ALLOW ("connect");
		SC_ALLOW ("recvmsg");
		SC_ALLOW ("sendmsg");
		SC_ALLOW ("sendto");
		SC_ALLOW ("setsockopt");
		SC_ALLOW_ARG_1 ("socket", SCMP_A0 (SCMP_CMP_EQ, AF_UNIX));
	}
	/* ESET sends messages to a System V message queue. */
	if (search_ld_preload ("libesets_pac.so") ||
	    search_ld_preload ("libscep_pac.so")) {
		SC_ALLOW_ARG_1 ("msgget", SCMP_A1 (SCMP_CMP_EQ, 0));
		SC_ALLOW ("msgsnd");
	}

	return ctx;
}

#undef SC_ALLOW_ARG_2
#undef SC_ALLOW_ARG_1
#undef SC_ALLOW

#endif /* HAVE_LIBSECCOMP */

/* Create a sandbox for processing untrusted data.
 *
 * This only sets up data structures; the caller must call sandbox_load to
 * actually enter the sandbox.
 */
man_sandbox *sandbox_init (void)
{
	man_sandbox *sandbox = XZALLOC (man_sandbox);

#ifdef HAVE_LIBSECCOMP
	sandbox->ctx = make_seccomp_filter (0);
	sandbox->permissive_ctx = make_seccomp_filter (1);
#else /* !HAVE_LIBSECCOMP */
	sandbox->dummy = 0;
#endif /* HAVE_LIBSECCOMP */

	return sandbox;
}

#ifdef HAVE_LIBSECCOMP
static void _sandbox_load (man_sandbox *sandbox, int permissive) {
	if (can_load_seccomp ()) {
		scmp_filter_ctx ctx;

		debug ("loading seccomp filter (permissive: %d)\n",
		       permissive);
		if (permissive)
			ctx = sandbox->permissive_ctx;
		else
			ctx = sandbox->ctx;
		if (seccomp_load (ctx) < 0) {
			if (errno == EINVAL || errno == EFAULT) {
				/* The kernel doesn't give us particularly
				 * fine-grained errors.  EINVAL could in
				 * theory be an invalid BPF program, but
				 * it's much more likely that the running
				 * kernel doesn't support seccomp filtering.
				 * EFAULT normally means a programming
				 * error, but it could also be returned here
				 * by some versions of qemu-user
				 * (https://bugs.launchpad.net/bugs/1726394).
				 */
				gripe_seccomp_filter_unavailable ();
				/* Don't try this again. */
				seccomp_filter_unavailable = 1;
			} else
				error (FATAL, errno,
				       "can't load seccomp filter");
		}
	}
}
#else /* !HAVE_LIBSECCOMP */
static void _sandbox_load (man_sandbox *sandbox _GL_UNUSED,
			   int permissive _GL_UNUSED)
{
}
#endif /* HAVE_LIBSECCOMP */

/* Enter a sandbox for processing untrusted data. */
void sandbox_load (void *data)
{
	man_sandbox *sandbox = data;

	_sandbox_load (sandbox, 0);
}

/* Enter a sandbox for processing untrusted data, allowing limited file
 * creation.
 */
void sandbox_load_permissive (void *data)
{
	man_sandbox *sandbox = data;

	_sandbox_load (sandbox, 1);
}

/* Free a sandbox for processing untrusted data. */
void sandbox_free (void *data) {
	man_sandbox *sandbox = data;

#ifdef HAVE_LIBSECCOMP
	seccomp_release (sandbox->ctx);
	seccomp_release (sandbox->permissive_ctx);
#endif /* HAVE_LIBSECCOMP */

	free (sandbox);
}
