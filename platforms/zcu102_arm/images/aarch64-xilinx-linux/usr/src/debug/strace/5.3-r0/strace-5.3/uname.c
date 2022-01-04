/*
 * Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 * Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 * Copyright (c) 1993-1996 Rick Sladkey <jrs@world.std.com>
 * Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 * Copyright (c) 2012-2015 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2014-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "print_fields.h"
#include <sys/utsname.h>

SYS_FUNC(uname)
{
	struct utsname uname;

	if (entering(tcp))
		return 0;

	if (!umove_or_printaddr(tcp, tcp->u_arg[0], &uname)) {
		PRINT_FIELD_CSTRING("{", uname, sysname);
		PRINT_FIELD_CSTRING(", ", uname, nodename);
		if (abbrev(tcp)) {
			tprints(", ...}");
			return 0;
		}
		PRINT_FIELD_CSTRING(", ", uname, release);
		PRINT_FIELD_CSTRING(", ", uname, version);
		PRINT_FIELD_CSTRING(", ", uname, machine);
#ifdef HAVE_STRUCT_UTSNAME_DOMAINNAME
		PRINT_FIELD_CSTRING(", ", uname, domainname);
#endif
		tprints("}");
	}

	return 0;
}
