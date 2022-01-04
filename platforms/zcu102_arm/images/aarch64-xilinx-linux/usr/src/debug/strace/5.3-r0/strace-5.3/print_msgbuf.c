/*
 * Copyright (c) 1993 Ulrich Pegelow <pegelow@moorea.uni-muenster.de>
 * Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 * Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 * Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 * Copyright (c) 2003-2006 Roland McGrath <roland@redhat.com>
 * Copyright (c) 2006-2015 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2015-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

#include <sys/msg.h>

#include DEF_MPERS_TYPE(msgbuf_t)
typedef struct msgbuf msgbuf_t;
#include MPERS_DEFS

MPERS_PRINTER_DECL(void, tprint_msgbuf, struct tcb *const tcp,
		   const kernel_ulong_t addr, const kernel_ulong_t count)
{
	msgbuf_t msg;

	if (!umove_or_printaddr(tcp, addr, &msg)) {
		tprintf("{%" PRI_kld ", ", (kernel_long_t) msg.mtype);
		printstrn(tcp, addr + sizeof(msg.mtype), count);
		tprints("}");
	}
	tprintf(", %" PRI_klu ", ", count);
}
