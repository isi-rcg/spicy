/*
 * Copyright (c) 2015 Elvira Khabirova <lineprinter0@gmail.com>
 * Copyright (c) 2015-2016 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2015-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>

#include "xlat.h"
#include "xlat/resource_flags.h"

#ifndef MSG_STAT_ANY
# define MSG_STAT_ANY 13
#endif

/*
 * Before glibc-2.22-122-gbe48165, ppc64 code tried to retrieve data
 * provided in third argument of msgctl call (in case of IPC_SET cmd)
 * which led to segmentation fault.
 */
#undef TEST_MSGCTL_BOGUS_ADDR
#if defined __GLIBC__ && (defined POWERPC64 || defined POWERPC64LE)
# if !(defined __GLIBC_MINOR__) \
   || ((__GLIBC__ << 16) + __GLIBC_MINOR__ < (2 << 16) + 23)
#  define TEST_MSGCTL_BOGUS_ADDR 0
# endif
#endif /* __GLIBC__ && (POWERPC64 || POWERPC64LE) */

#ifndef TEST_MSGCTL_BOGUS_ADDR
# define TEST_MSGCTL_BOGUS_ADDR 1
#endif

#if XLAT_RAW
# define str_ipc_excl_nowait "0xface1c00"
# define str_ipc_private "0"
# define str_ipc_rmid "0"
# define str_ipc_set "0x1"
# define str_ipc_stat "0x2"
# define str_msg_stat "0xb"
# define str_msg_info "0xc"
# define str_msg_stat_any "0xd"
# define str_ipc_64 "0x100"
# define str_bogus_cmd "0xdeadbeef"
#elif XLAT_VERBOSE
# define str_ipc_excl_nowait \
	"0xface1c00 /\\* IPC_EXCL\\|IPC_NOWAIT\\|0xface1000 \\*/"
# define str_ipc_private "0 /\\* IPC_PRIVATE \\*/"
# define str_ipc_rmid "0 /\\* IPC_RMID \\*/"
# define str_ipc_set "0x1 /\\* IPC_SET \\*/"
# define str_ipc_stat "0x2 /\\* IPC_STAT \\*/"
# define str_msg_stat "0xb /\\* MSG_STAT \\*/"
# define str_msg_info "0xc /\\* MSG_INFO \\*/"
# define str_msg_stat_any "0xd /\\* MSG_STAT_ANY \\*/"
# define str_ipc_64 "0x100 /\\* IPC_64 \\*/"
# define str_bogus_cmd "0xdeadbeef /\\* MSG_\\?\\?\\? \\*/"
#else
# define str_ipc_excl_nowait "IPC_EXCL\\|IPC_NOWAIT\\|0xface1000"
# define str_ipc_private "IPC_PRIVATE"
# define str_ipc_rmid "IPC_RMID"
# define str_ipc_set "IPC_SET"
# define str_ipc_stat "IPC_STAT"
# define str_msg_stat "MSG_STAT"
# define str_msg_info "MSG_INFO"
# define str_msg_stat_any "MSG_STAT_ANY"
# define str_ipc_64 "IPC_64"
# define str_bogus_cmd "0xdeadbeef /\\* MSG_\\?\\?\\? \\*/"
#endif

static int id = -1;

static void
cleanup(void)
{
	msgctl(id, IPC_RMID, NULL);
	printf("msgctl\\(%d, (%s\\|)?%s, NULL\\) += 0\n",
	       id, str_ipc_64, str_ipc_rmid);
	id = -1;
}

int
main(void)
{
	static const key_t private_key =
		(key_t) (0xffffffff00000000ULL | IPC_PRIVATE);
	static const key_t bogus_key = (key_t) 0xeca86420fdb9f531ULL;
	static const int bogus_msgid = 0xfdb97531;
	static const int bogus_cmd = 0xdeadbeef;
#if TEST_MSGCTL_BOGUS_ADDR
	static void * const bogus_addr = (void *) -1L;
#endif
	static const int bogus_flags = 0xface1e55 & ~IPC_CREAT;

	int rc;
	struct msqid_ds ds;

	rc = msgget(bogus_key, bogus_flags);
	printf("msgget\\(%#llx, %s\\|%#04o\\) = %s\n",
	       zero_extend_signed_to_ull(bogus_key),
	       str_ipc_excl_nowait,
	       bogus_flags & 0777, sprintrc_grep(rc));

	id = msgget(private_key, 0600);
	if (id < 0)
		perror_msg_and_skip("msgget");
	printf("msgget\\(%s, 0600\\) = %d\n", str_ipc_private, id);
	atexit(cleanup);

	rc = msgctl(bogus_msgid, bogus_cmd, NULL);
	printf("msgctl\\(%d, (%s\\|)?%s, NULL\\) = %s\n",
	       bogus_msgid, str_ipc_64, str_bogus_cmd, sprintrc_grep(rc));

#if TEST_MSGCTL_BOGUS_ADDR
	rc = msgctl(bogus_msgid, IPC_SET, bogus_addr);
	printf("msgctl\\(%d, (%s\\|)?%s, %p\\) = %s\n",
	       bogus_msgid, str_ipc_64, str_ipc_set, bogus_addr,
	       sprintrc_grep(rc));
#endif

	if (msgctl(id, IPC_STAT, &ds))
		perror_msg_and_skip("msgctl IPC_STAT");
	printf("msgctl\\(%d, (%s\\|)?%s, \\{msg_perm=\\{uid=%u"
	       ", gid=%u, mode=%#o, key=%u, cuid=%u, cgid=%u\\}, msg_stime=%u"
	       ", msg_rtime=%u, msg_ctime=%u, msg_qnum=%u, msg_qbytes=%u"
	       ", msg_lspid=%u, msg_lrpid=%u\\}\\) = 0\n",
	       id, str_ipc_64, str_ipc_stat,
	       (unsigned) ds.msg_perm.uid, (unsigned) ds.msg_perm.gid,
	       (unsigned) ds.msg_perm.mode, (unsigned) ds.msg_perm.__key,
	       (unsigned) ds.msg_perm.cuid, (unsigned) ds.msg_perm.cgid,
	       (unsigned) ds.msg_stime, (unsigned) ds.msg_rtime,
	       (unsigned) ds.msg_ctime, (unsigned) ds.msg_qnum,
	       (unsigned) ds.msg_qbytes, (unsigned) ds.msg_lspid,
	       (unsigned) ds.msg_lrpid);

	if (msgctl(id, IPC_SET, &ds))
		perror_msg_and_skip("msgctl IPC_SET");
	printf("msgctl\\(%d, (%s\\|)?%s, \\{msg_perm=\\{uid=%u"
	       ", gid=%u, mode=%#o\\}, ...\\}\\) = 0\n",
	       id, str_ipc_64, str_ipc_set, (unsigned) ds.msg_perm.uid,
	       (unsigned) ds.msg_perm.gid, (unsigned) ds.msg_perm.mode);

	rc = msgctl(0, MSG_INFO, &ds);
	printf("msgctl\\(0, (%s\\|)?%s, %p\\) = %s\n",
	       str_ipc_64, str_msg_info, &ds, sprintrc_grep(rc));

	rc = msgctl(id, MSG_STAT, &ds);
	printf("msgctl\\(%d, (%s\\|)?%s, %p\\) = %s\n",
	       id, str_ipc_64, str_msg_stat, &ds, sprintrc_grep(rc));

	rc = msgctl(id, MSG_STAT_ANY, &ds);
	printf("msgctl\\(%d, (%s\\|)?%s, %p\\) = %s\n",
	       id, str_ipc_64, str_msg_stat_any, &ds, sprintrc_grep(rc));

	return 0;
}
