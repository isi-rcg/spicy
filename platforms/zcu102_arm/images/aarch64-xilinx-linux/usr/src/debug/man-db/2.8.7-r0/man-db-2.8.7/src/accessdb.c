/*
 * accessdb.c: show every key/content pair in the database.
 *
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2002 Colin Watson.
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
 * Tue Apr 26 12:56:44 BST 1994  Wilf. (G.Wilford@ee.surrey.ac.uk) 
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "argp.h"
#include "progname.h"
#include "xvasprintf.h"

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "error.h"

#include "mydbm.h"

const char *cat_root;

char *database;

const char *argp_program_version = "accessdb " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static const char args_doc[] = N_("[MAN DATABASE]");
static const char doc[] = "\v" N_("The man database defaults to %s%s.");

static struct argp_option options[] = {
	{ "debug",	'd',	0,	0,	N_("emit debugging messages") },
	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {
		case 'd':
			debug_level = true;
			return 0;
		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP &
					 ~ARGP_HELP_PRE_DOC);
			break;
		case ARGP_KEY_ARG:
			if (database)
				argp_usage (state);
			database = arg;
			return 0;
		case ARGP_KEY_NO_ARGS:
			database = mkdbname (cat_root);
			return 0;
	}
	return ARGP_ERR_UNKNOWN;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
static char *help_filter (int key, const char *text, void *input _GL_UNUSED)
{
	switch (key) {
		case ARGP_KEY_HELP_PRE_DOC:
			/* We have no pre-options help text, but the input
			 * text may contain header junk due to gettext ("").
			 */
			return NULL;
		case ARGP_KEY_HELP_POST_DOC:
			return xasprintf (text, cat_root, MAN_DB);
		default:
			return (char *) text;
	}
}
#pragma GCC diagnostic pop

static struct argp argp = { options, parse_opt, args_doc, doc, 0,
			    help_filter };

int main (int argc, char *argv[])
{
	MYDBM_FILE dbf;
	datum key;
	int ret = OK;

	set_program_name (argv[0]);

	init_debug ();
	init_locale ();

	if (is_directory (FHS_CAT_ROOT) == 1)
		cat_root = FHS_CAT_ROOT;
	else if (is_directory (CAT_ROOT) == 1)
		cat_root = CAT_ROOT;

	if (argp_parse (&argp, argc, argv, 0, 0, 0))
		exit (FAIL);

	dbf = MYDBM_RDOPEN (database);
	if (dbf && dbver_rd (dbf)) {
		MYDBM_CLOSE (dbf);
		dbf = NULL;
	}
	if (!dbf)
		error (FATAL, errno, _("can't open %s for reading"), database);
	assert (dbf);  /* help the compiler prove that later accesses are OK */

	key = MYDBM_FIRSTKEY (dbf);

	while (MYDBM_DPTR (key) != NULL) {
		datum content, nextkey;
		char *t, *nicekey;

		content = MYDBM_FETCH (dbf, key);
		if (!MYDBM_DPTR (content)) {
			debug ("key %s has no content!\n", MYDBM_DPTR (key));
			ret = FATAL;
			goto next;
		}
		nicekey = xstrdup (MYDBM_DPTR (key));
		while ( (t = strchr (nicekey, '\t')) )
			*t = '~';
		while ( (t = strchr (MYDBM_DPTR (content), '\t')) )
			*t = ' ';
		printf ("%s -> \"%s\"\n", nicekey, MYDBM_DPTR (content));
		free (nicekey); 
		MYDBM_FREE_DPTR (content);
next:
		nextkey = MYDBM_NEXTKEY (dbf, key);
		MYDBM_FREE_DPTR (key);
		key = nextkey;
	}

	MYDBM_CLOSE (dbf);
	exit (ret);
}
