/*
 * Copyright (c) 1990 - 1994, Julianne Frances Haugh
 * Copyright (c) 1996 - 2000, Marek Michałkiewicz
 * Copyright (c) 2001       , Michał Moskal
 * Copyright (c) 2005       , Tomasz Kłoczko
 * Copyright (c) 2007 - 2009, Nicolas François
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the copyright holders or contributors may not be used to
 *    endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <config.h>

#ident "$Id$"

#include "prototypes.h"
#include "defines.h"
#include <shadow.h>
#include <stdio.h>
#include "commonio.h"
#include "getdef.h"
#include "shadowio.h"
#ifdef WITH_TCB
#include <tcb.h>
#include "tcbfuncs.h"
#endif				/* WITH_TCB */

static /*@null@*/ /*@only@*/void *shadow_dup (const void *ent)
{
	const struct spwd *sp = ent;

	return __spw_dup (sp);
}

static void shadow_free (/*@out@*//*@only@*/void *ent)
{
	struct spwd *sp = ent;

	spw_free (sp);
}

static const char *shadow_getname (const void *ent)
{
	const struct spwd *sp = ent;

	return sp->sp_namp;
}

static void *shadow_parse (const char *line)
{
	return (void *) sgetspent (line);
}

static int shadow_put (const void *ent, FILE * file)
{
	const struct spwd *sp = ent;

	if (   (NULL == sp)
	    || (valid_field (sp->sp_namp, ":\n") == -1)
	    || (valid_field (sp->sp_pwdp, ":\n") == -1)) {
		return -1;
	}

	return (putspent (sp, file) == -1) ? -1 : 0;
}

static struct commonio_ops shadow_ops = {
	shadow_dup,
	shadow_free,
	shadow_getname,
	shadow_parse,
	shadow_put,
	fgets,
	fputs,
	NULL,			/* open_hook */
	NULL			/* close_hook */
};

static struct commonio_db shadow_db = {
	SHADOW_FILE,		/* filename */
	&shadow_ops,		/* ops */
	NULL,			/* fp */
#ifdef WITH_SELINUX
	NULL,			/* scontext */
#endif				/* WITH_SELINUX */
	0400,                   /* st_mode */
	0,                      /* st_uid */
	0,                      /* st_gid */
	NULL,			/* head */
	NULL,			/* tail */
	NULL,			/* cursor */
	false,			/* changed */
	false,			/* isopen */
	false,			/* locked */
	false			/* readonly */
};

int spw_setdbname (const char *filename)
{
	return commonio_setname (&shadow_db, filename);
}

/*@observer@*/const char *spw_dbname (void)
{
	return shadow_db.filename;
}

bool spw_file_present (void)
{
	if (getdef_bool ("FORCE_SHADOW"))
		return true;
	return commonio_present (&shadow_db);
}

int spw_lock (void)
{
#ifdef WITH_TCB
	int retval = 0;

	if (!getdef_bool ("USE_TCB")) {
#endif				/* WITH_TCB */
		return commonio_lock (&shadow_db);
#ifdef WITH_TCB
	}
	if (shadowtcb_drop_priv () == SHADOWTCB_FAILURE) {
		return 0;
	}
	if (lckpwdf_tcb (shadow_db.filename) == 0) {
		shadow_db.locked = 1;
		retval = 1;
	}
	if (shadowtcb_gain_priv () == SHADOWTCB_FAILURE) {
		return 0;
	}
	return retval;
#endif				/* WITH_TCB */
}

int spw_open (int mode)
{
	int retval = 0;
#ifdef WITH_TCB
	bool use_tcb = getdef_bool ("USE_TCB");

	if (use_tcb && (shadowtcb_drop_priv () == SHADOWTCB_FAILURE)) {
		return 0;
	}
#endif				/* WITH_TCB */
	retval = commonio_open (&shadow_db, mode);
#ifdef WITH_TCB
	if (use_tcb && (shadowtcb_gain_priv () == SHADOWTCB_FAILURE)) {
		return 0;
	}
#endif				/* WITH_TCB */
	return retval;
}

/*@observer@*/ /*@null@*/const struct spwd *spw_locate (const char *name)
{
	return commonio_locate (&shadow_db, name);
}

int spw_update (const struct spwd *sp)
{
	return commonio_update (&shadow_db, (const void *) sp);
}

int spw_remove (const char *name)
{
	return commonio_remove (&shadow_db, name);
}

int spw_rewind (void)
{
	return commonio_rewind (&shadow_db);
}

/*@observer@*/ /*@null@*/const struct spwd *spw_next (void)
{
	return commonio_next (&shadow_db);
}

int spw_close (void)
{
	int retval = 0;
#ifdef WITH_TCB
	bool use_tcb = getdef_bool ("USE_TCB");

	if (use_tcb && (shadowtcb_drop_priv () == SHADOWTCB_FAILURE)) {
		return 0;
	}
#endif				/* WITH_TCB */
	retval = commonio_close (&shadow_db);
#ifdef WITH_TCB
	if (use_tcb && (shadowtcb_gain_priv () == SHADOWTCB_FAILURE)) {
		return 0;
	}
#endif				/* WITH_TCB */
	return retval;
}

int spw_unlock (void)
{
#ifdef WITH_TCB
	int retval = 0;

	if (!getdef_bool ("USE_TCB")) {
#endif				/* WITH_TCB */
		return commonio_unlock (&shadow_db);
#ifdef WITH_TCB
	}
	if (shadowtcb_drop_priv () == SHADOWTCB_FAILURE) {
		return 0;
	}
	if (ulckpwdf_tcb () == 0) {
		shadow_db.locked = 0;
		retval = 1;
	}
	if (shadowtcb_gain_priv () == SHADOWTCB_FAILURE) {
		return 0;
	}
	return retval;
#endif				/* WITH_TCB */
}

struct commonio_entry *__spw_get_head (void)
{
	return shadow_db.head;
}

void __spw_del_entry (const struct commonio_entry *ent)
{
	commonio_del_entry (&shadow_db, ent);
}

/* Sort with respect to passwd ordering. */
int spw_sort ()
{
#ifdef WITH_TCB
	if (getdef_bool ("USE_TCB")) {
		return 0;
	}
#endif				/* WITH_TCB */
	return commonio_sort_wrt (&shadow_db, __pw_get_db ());
}
