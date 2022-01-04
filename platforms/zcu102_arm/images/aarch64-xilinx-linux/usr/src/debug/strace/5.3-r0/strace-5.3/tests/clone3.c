/*
 * Check decoding of clone3 syscall.
 *
 * Copyright (c) 2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#ifdef HAVE_LINUX_SCHED_H
# include <linux/sched.h>
#endif

#ifdef HAVE_STRUCT_USER_DESC
# include <asm/ldt.h>
#endif

#include "scno.h"

#ifndef VERBOSE
# define VERBOSE 0
#endif
#ifndef RETVAL_INJECTED
# define RETVAL_INJECTED 0
#endif

#ifndef HAVE_STRUCT_CLONE_ARGS
# include <stdint.h>
# include <linux/types.h>

# define XLAT_MACROS_ONLY
#  include "xlat/clone_flags.h"
# undef XLAT_MACROS_ONLY

struct clone_args {
	uint64_t flags;
	uint64_t pidfd;
	uint64_t child_tid;
	uint64_t parent_tid;
	uint64_t exit_signal;
	uint64_t stack;
	uint64_t stack_size;
	uint64_t tls;
};
#endif /* !HAVE_STRUCT_CLONE_ARGS */

enum validity_flag_bits {
	STRUCT_VALID_BIT,
	PIDFD_VALID_BIT,
	CHILD_TID_VALID_BIT,
	PARENT_TID_VALID_BIT,
	TLS_VALID_BIT,
};

#define _(x_) x_ = 1 << x_##_BIT

enum validity_flags {
	_(STRUCT_VALID),
	_(PIDFD_VALID),
	_(CHILD_TID_VALID),
	_(PARENT_TID_VALID),
	_(TLS_VALID),
};

#undef _

static const int child_exit_status = 42;

#if RETVAL_INJECTED
static const long injected_retval = 42;

# define INJ_STR " (INJECTED)\n"
#else /* !RETVAL_INJECTED */
# define INJ_STR "\n"
#endif /* RETVAL_INJECTED */


#if !RETVAL_INJECTED
static void
wait_cloned(int pid)
{
	int status;

	errno = 0;
	while (waitpid(pid, &status, WEXITED | __WCLONE) != pid) {
		if (errno != EINTR)
			perror_msg_and_fail("waitpid(%d)", pid);
	}
}
#endif

static long
do_clone3_(void *args, kernel_ulong_t size, bool should_fail, int line)
{
	long rc = syscall(__NR_clone3, args, size);

#if RETVAL_INJECTED
	if (rc != injected_retval)
		perror_msg_and_fail("%d: Unexpected injected return value "
				    "of a clone3() call (%ld instead of %ld)",
				    line, rc, injected_retval);
#else

	static int unimplemented_error = -1;

	if (should_fail) {
		if (rc >= 0)
			error_msg_and_fail("%d: Unexpected success"
					   " of a clone3() call", line);
		if (unimplemented_error < 0)
			unimplemented_error =
				(errno == EINVAL) ? ENOSYS : errno;
	} else {
		if (rc < 0 && errno != unimplemented_error)
			perror_msg_and_fail("%d: Unexpected failure"
					    " of a clone3() call", line);
	}

	if (!rc)
		_exit(child_exit_status);

	if (rc > 0 && ((struct clone_args *) args)->exit_signal)
		wait_cloned(rc);
#endif

	return rc;
}

#define do_clone3(args_, size_, should_fail_) \
	do_clone3_((args_), (size_), (should_fail_), __LINE__)

static inline void
print_addr64(const char *pfx, uint64_t addr)
{
	if (addr)
		printf("%s%#" PRIx64, pfx, addr);
	else
		printf("%sNULL", pfx);
}

static void
print_tls(const char *pfx, uint64_t arg_ptr, enum validity_flags vf)
{
#if defined HAVE_STRUCT_USER_DESC && defined __i386__
	if (!(vf & TLS_VALID)) {
		print_addr64(pfx, arg_ptr);
		return;
	}

	struct user_desc *arg = (struct user_desc *) (uintptr_t) arg_ptr;

	printf("%s{entry_number=%d"
	       ", base_addr=%#08x"
	       ", limit=%#08x"
	       ", seg_32bit=%u"
	       ", contents=%u"
	       ", read_exec_only=%u"
	       ", limit_in_pages=%u"
	       ", seg_not_present=%u"
	       ", useable=%u}",
	       pfx,
	       arg->entry_number,
	       arg->base_addr,
	       arg->limit,
	       arg->seg_32bit,
	       arg->contents,
	       arg->read_exec_only,
	       arg->limit_in_pages,
	       arg->seg_not_present,
	       arg->useable);
#else
	print_addr64(pfx, arg_ptr);
#endif
}

static inline void
print_clone3(struct clone_args *const arg, long rc, kernel_ulong_t sz,
	     enum validity_flags valid,
	     const char *flags_str, const char *es_str)
{
	int saved_errno = errno;

	if (!(valid & STRUCT_VALID)) {
		printf("%p", arg);
		goto out;
	}

#if XLAT_RAW
	printf("{flags=%#" PRIx64, (uint64_t) arg->flags);
#elif XLAT_VERBOSE
	if (flags_str[0] == '0')
		printf("{flags=%#" PRIx64, (uint64_t) arg->flags);
	else
		printf("{flags=%#" PRIx64 " /* %s */",
		       (uint64_t) arg->flags, flags_str);
#else
	printf("{flags=%s", flags_str);
#endif

	if (arg->flags & CLONE_PIDFD)
		print_addr64(", pidfd=", arg->pidfd);

	if (arg->flags & (CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID)) {
		if (valid & CHILD_TID_VALID)
			printf(", child_tid=[%d]",
			       *(int *) (uintptr_t) arg->child_tid);
		else
			print_addr64(", child_tid=", arg->child_tid);
	}

	if (arg->flags & CLONE_PARENT_SETTID)
		print_addr64(", parent_tid=", arg->parent_tid);

	printf(", exit_signal=%s", es_str);
	print_addr64(", stack=", arg->stack);
	printf(", stack_size=%" PRIx64, (uint64_t) arg->stack_size);

	if (arg->flags & CLONE_SETTLS)
		print_tls("tls=", arg->tls, valid);

	printf("}");

	if (rc < 0)
		goto out;

	bool comma = false;

	if (arg->flags & CLONE_PIDFD) {
		if (valid & PIDFD_VALID)
			printf(" => {pidfd=[%d]",
			       *(int *) (uintptr_t) arg->pidfd);
		else
			print_addr64(" => {pidfd=", arg->pidfd);

		comma = true;
	}

	if (arg->flags & CLONE_PARENT_SETTID) {
		printf(comma ? ", " : " => {");

		if (valid & PARENT_TID_VALID)
			printf("parent_tid=[%d]",
			       *(int *) (uintptr_t) arg->parent_tid);
		else
			print_addr64("parent_tid=", arg->parent_tid);

		comma = true;
	}

	if (comma)
		printf("}");

out:
	errno = saved_errno;
}

int
main(int argc, char *argv[])
{
	static const struct {
		struct clone_args args;
		bool should_fail;
		enum validity_flags vf;
		const char *flags_str;
		const char *es_str;
	} arg_vals[] = {
		{ { .flags = 0 },
			false, 0, "0", "0" },
		{ { .flags = CLONE_PARENT_SETTID },
			false, 0, "CLONE_PARENT_SETTID", "0" },
	};

	struct clone_args *arg = tail_alloc(sizeof(*arg));
	struct clone_args *arg2 = tail_alloc(sizeof(*arg2) + 8);
	int *pidfd = tail_alloc(sizeof(*pidfd));
	int *child_tid = tail_alloc(sizeof(*child_tid));
	int *parent_tid = tail_alloc(sizeof(*parent_tid));
	long rc;

#if defined HAVE_STRUCT_USER_DESC
	struct user_desc *tls = tail_alloc(sizeof(*tls));

	fill_memory(tls, sizeof(*tls));
#else
	int *tls = tail_alloc(sizeof(*tls));
#endif

	*pidfd = 0xbadc0ded;
	*child_tid = 0xdeadface;
	*parent_tid = 0xfeedbeef;

	rc = do_clone3(NULL, 0, true);
	printf("clone3(NULL, 0) = %s" INJ_STR, sprintrc(rc));

	rc = do_clone3(arg + 1, sizeof(*arg), true);
	printf("clone3(%p, %zu) = %s" INJ_STR,
	       arg + 1, sizeof(*arg), sprintrc(rc));

	rc = do_clone3((char *) arg + sizeof(uint64_t),
		       sizeof(*arg) - sizeof(uint64_t), true);
	printf("clone3(%p, %zu) = %s" INJ_STR,
	       (char *) arg + sizeof(uint64_t), sizeof(*arg) - sizeof(uint64_t),
	       sprintrc(rc));


	memset(arg, 0, sizeof(*arg));
	memset(arg2, 0, sizeof(*arg2) + 8);

	rc = do_clone3(arg, 64, false);
	printf("clone3({flags=0, exit_signal=0, stack=NULL, stack_size=0}, 64)"
	       " = %s" INJ_STR,
	       sprintrc(rc));

	rc = do_clone3(arg, sizeof(*arg) + 8, true);
	printf("clone3({flags=0, exit_signal=0, stack=NULL, stack_size=0, ???}"
#if RETVAL_INJECTED
	       " => {???}"
#endif
	       ", %zu) = %s" INJ_STR,
	       sizeof(*arg) + 8, sprintrc(rc));

	rc = do_clone3(arg2, sizeof(*arg2) + 8, false);
	printf("clone3({flags=0, exit_signal=0, stack=NULL, stack_size=0}"
	       ", %zu) = %s" INJ_STR,
	       sizeof(*arg2) + 8, sprintrc(rc));

	/*
	 * NB: the following check is purposedly fragile (it will break
	 *     when system's struct clone_args has additional fields,
	 *     so it signalises that the decoder needs to be updated.
	 */
	arg2[1].flags = 0xfacefeeddeadc0de;
	arg2->exit_signal = 0xdeadface00000000ULL | SIGCHLD;
	rc = do_clone3(arg2, sizeof(*arg2) + 8, true);
	printf("clone3({flags=0, exit_signal=%llu, stack=NULL, stack_size=0"
	       ", /* bytes %zu..%zu */ "
#if WORDS_BIGENDIAN
	       "\"\\xfa\\xce\\xfe\\xed\\xde\\xad\\xc0\\xde\""
#else
	       "\"\\xde\\xc0\\xad\\xde\\xed\\xfe\\xce\\xfa\""
#endif
#if RETVAL_INJECTED
	       "} => {/* bytes %zu..%zu */ "
# if WORDS_BIGENDIAN
	       "\"\\xfa\\xce\\xfe\\xed\\xde\\xad\\xc0\\xde\""
# else
	       "\"\\xde\\xc0\\xad\\xde\\xed\\xfe\\xce\\xfa\""
# endif
#endif /* RETVAL_INJECTED */
	       "}, %zu) = %s" INJ_STR,
	       0xdeadface00000000ULL | SIGCHLD,
	       sizeof(*arg2), sizeof(*arg2) + 7,
#if RETVAL_INJECTED
	       sizeof(*arg2), sizeof(*arg2) + 7,
#endif
	       sizeof(*arg2) + 8, sprintrc(rc));

	arg2->exit_signal = 0xdeadc0de;
	rc = do_clone3(arg2, sizeof(*arg) + 16, true);
	printf("clone3({flags=0, exit_signal=3735929054, stack=NULL"
	       ", stack_size=0, ???}"
#if RETVAL_INJECTED
	       " => {???}"
#endif
	       ", %zu) = %s" INJ_STR,
	       sizeof(*arg) + 16, sprintrc(rc));

	arg->flags = 0xfacefeedbeefc0de;
	arg->exit_signal = 0x1e55c0de;
	rc = do_clone3(arg, 64, true);
	printf("clone3({flags=%s, child_tid=NULL, exit_signal=508936414"
	       ", stack=NULL, stack_size=0, tls=NULL}, 64) = %s" INJ_STR,
	       XLAT_KNOWN(0xfacefeedbeefc0de, "CLONE_VFORK|CLONE_PARENT"
	       "|CLONE_THREAD|CLONE_NEWNS|CLONE_SYSVSEM|CLONE_SETTLS"
	       "|CLONE_CHILD_CLEARTID|CLONE_UNTRACED|CLONE_NEWCGROUP"
	       "|CLONE_NEWUTS|CLONE_NEWIPC|CLONE_NEWUSER|CLONE_NEWPID|CLONE_IO"
	       "|0xfacefeed004000de"), sprintrc(rc));

	arg->flags = 0xdec0dead004000ffULL;
	arg->exit_signal = 250;
	arg->stack = 0xface1e55beeff00dULL;
	arg->stack_size = 0xcaffeedefacedca7ULL;
	rc = do_clone3(arg, 64, true);
	printf("clone3({flags=%s, exit_signal=250"
	       ", stack=0xface1e55beeff00d, stack_size=0xcaffeedefacedca7}, 64)"
	       " = %s" INJ_STR,
	       XLAT_UNKNOWN(0xdec0dead004000ff, "CLONE_???"),
	       sprintrc(rc));

	arg->exit_signal = SIGCHLD;

	struct {
		uint64_t flag;
		const char *flag_str;
		uint64_t *field;
		const char *field_name;
		int *ptr;
		bool deref_exiting;
	} pid_fields[] = {
		{ ARG_STR(CLONE_PIDFD),
			(uint64_t *) &arg->pidfd,
			"pidfd", pidfd, true },
		{ ARG_STR(CLONE_CHILD_SETTID),
			(uint64_t *) &arg->child_tid,
			"child_tid", child_tid },
		{ ARG_STR(CLONE_CHILD_CLEARTID),
			(uint64_t *) &arg->child_tid,
			"child_tid", child_tid },
		{ ARG_STR(CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID),
			(uint64_t *) &arg->child_tid,
			"child_tid", child_tid },
		{ ARG_STR(CLONE_PARENT_SETTID),
			(uint64_t *) &arg->parent_tid,
			"parent_tid", parent_tid, true },
	};

	for (size_t i = 0; i < ARRAY_SIZE(pid_fields); i++) {
		char flag_str[128];
		const char *rc_str;

		arg->flags = 0xbad0000000000001ULL | pid_fields[i].flag;

#if XLAT_RAW
		snprintf(flag_str, sizeof(flag_str), "%#" PRIx64,
			 (uint64_t) arg->flags);
#elif XLAT_VERBOSE
		snprintf(flag_str, sizeof(flag_str),
			 "%#" PRIx64 " /* %s|0xbad0000000000001 */",
			 (uint64_t) arg->flags, pid_fields[i].flag_str);
#else
		snprintf(flag_str, sizeof(flag_str), "%s|0xbad0000000000001",
			 pid_fields[i].flag_str);
#endif

		pid_fields[i].field[0] = 0;
		rc = do_clone3(arg, 64, true);
		rc_str = sprintrc(rc);
		printf("clone3({flags=%s, %s=NULL"
		       ", exit_signal=" XLAT_KNOWN(SIGCHLD, "SIGCHLD")
		       ", stack=0xface1e55beeff00d"
		       ", stack_size=0xcaffeedefacedca7}",
		       flag_str, pid_fields[i].field_name);
#if RETVAL_INJECTED
		if (pid_fields[i].deref_exiting)
			printf(" => {%s=NULL}", pid_fields[i].field_name);
#endif /* RETVAL_INJECTED */
		printf(", 64) = %s" INJ_STR, rc_str);

		pid_fields[i].field[0] = (uintptr_t) (pid_fields[i].ptr + 1);
		rc = do_clone3(arg, 64, true);
		rc_str = sprintrc(rc);
		printf("clone3({flags=%s, %s=%p"
		       ", exit_signal=" XLAT_KNOWN(SIGCHLD, "SIGCHLD")
		       ", stack=0xface1e55beeff00d"
		       ", stack_size=0xcaffeedefacedca7}",
		       flag_str, pid_fields[i].field_name,
		       pid_fields[i].ptr + 1);
#if RETVAL_INJECTED
		if (pid_fields[i].deref_exiting)
			printf(" => {%s=%p}",
			       pid_fields[i].field_name, pid_fields[i].ptr + 1);
#endif /* RETVAL_INJECTED */
		printf(", 64) = %s" INJ_STR, rc_str);

		pid_fields[i].field[0] = (uintptr_t) pid_fields[i].ptr;
		rc = do_clone3(arg, 64, true);
		rc_str = sprintrc(rc);
		printf("clone3({flags=%s, %s=%p"
		       ", exit_signal=" XLAT_KNOWN(SIGCHLD, "SIGCHLD")
		       ", stack=0xface1e55beeff00d"
		       ", stack_size=0xcaffeedefacedca7}",
		       flag_str, pid_fields[i].field_name,
		       pid_fields[i].ptr);
#if RETVAL_INJECTED
		if (pid_fields[i].deref_exiting)
			printf(" => {%s=[%d]}",
			       pid_fields[i].field_name, *pid_fields[i].ptr);
#endif /* RETVAL_INJECTED */
		printf(", 64) = %s" INJ_STR, rc_str);
	}

	arg->flags = 0xbad0000000000001ULL | CLONE_SETTLS;
	rc = do_clone3(arg, 64, true);
	printf("clone3({flags="
	       XLAT_KNOWN(0xbad0000000080001, "CLONE_SETTLS|0xbad0000000000001")
	       ", exit_signal=" XLAT_KNOWN(SIGCHLD, "SIGCHLD")
	       ", stack=0xface1e55beeff00d"
	       ", stack_size=0xcaffeedefacedca7, tls=NULL}, 64) = %s" INJ_STR,
	       sprintrc(rc));

	arg->tls = (uintptr_t) (tls + 1);
	rc = do_clone3(arg, 64, true);
	printf("clone3({flags="
	       XLAT_KNOWN(0xbad0000000080001, "CLONE_SETTLS|0xbad0000000000001")
	       ", exit_signal=" XLAT_KNOWN(SIGCHLD, "SIGCHLD")
	       ", stack=0xface1e55beeff00d"
	       ", stack_size=0xcaffeedefacedca7, tls=%p}, 64) = %s" INJ_STR,
	       tls + 1, sprintrc(rc));

	arg->tls = (uintptr_t) tls;
	rc = do_clone3(arg, 64, true);
	printf("clone3({flags="
	       XLAT_KNOWN(0xbad0000000080001, "CLONE_SETTLS|0xbad0000000000001")
	       ", exit_signal=" XLAT_KNOWN(SIGCHLD, "SIGCHLD")
	       ", stack=0xface1e55beeff00d, stack_size=0xcaffeedefacedca7, tls="
#if defined HAVE_STRUCT_USER_DESC && defined __i386__
	       "{entry_number=2206368128, base_addr=0x87868584"
	       ", limit=0x8b8a8988, seg_32bit=0, contents=2, read_exec_only=1"
	       ", limit_in_pages=0, seg_not_present=0, useable=0}"
#else
	       "%p"
#endif
	       "}, 64) = %s" INJ_STR,
#if !defined HAVE_STRUCT_USER_DESC || !defined __i386__
	       tls,
#endif
	       sprintrc(rc));

	for (size_t i = 0; i < ARRAY_SIZE(arg_vals); i++) {
		memcpy(arg, &arg_vals[i].args, sizeof(*arg));

		rc = do_clone3(arg, sizeof(*arg), arg_vals[i].should_fail);
		printf("clone3(");
		print_clone3(arg, rc, sizeof(*arg),
			     arg_vals[i].vf | STRUCT_VALID,
			     arg_vals[i].flags_str, arg_vals[i].es_str);
		printf(", %zu) = %s" INJ_STR, sizeof(*arg), sprintrc(rc));
	}

	puts("+++ exited with 0 +++");

	return 0;
}
