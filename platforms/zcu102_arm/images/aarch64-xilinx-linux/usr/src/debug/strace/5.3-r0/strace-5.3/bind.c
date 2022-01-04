/*
 * Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 * Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 * Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 * Copyright (c) 1996-2000 Wichert Akkerman <wichert@cistron.nl>
 * Copyright (c) 1999-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"

SYS_FUNC(bind)
{
	printfd(tcp, tcp->u_arg[0]);
	tprints(", ");
	const int addrlen = tcp->u_arg[2];
	decode_sockaddr(tcp, tcp->u_arg[1], addrlen);
	tprintf(", %d", addrlen);

	return RVAL_DECODED;
}
