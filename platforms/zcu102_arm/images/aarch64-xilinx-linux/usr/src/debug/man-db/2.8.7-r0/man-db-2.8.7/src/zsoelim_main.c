/*
 * zsoelim_main.c: eliminate .so includes within *roff source
 *  
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 1997 Fabrizio Polacco.
 * Copyright (C) 2001, 2002, 2003, 2004, 2006, 2007, 2008, 2009, 2010
 * Colin Watson.
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
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <stdlib.h>

#include "argp.h"
#include "gl_list.h"
#include "progname.h"
#include "xvasprintf.h"

#include "gettext.h"
#include <locale.h>
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "cleanup.h"
#include "error.h"
#include "pipeline.h"
#include "decompress.h"
#include "sandbox.h"

#include "manp.h"
#include "zsoelim.h"

int quiet = 1;
man_sandbox *sandbox;

static gl_list_t manpathlist;

static char **files;
static int num_files;

const char *argp_program_version = "zsoelim " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static const char args_doc[] = N_("FILE...");

static struct argp_option options[] = {
	{ "debug",	'd',	0,	0,	N_("emit debugging messages") },
	{ "compatible",	'C',	0,	0,	N_("compatibility switch (ignored)"),	1 },
	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static error_t parse_opt (int key, char *arg _GL_UNUSED,
			  struct argp_state *state)
{
	switch (key) {
		case 'd':
			debug_level = true;
			return 0;
		case 'C':
			return 0; /* compatibility with GNU soelim */
		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP);
			break;
		case ARGP_KEY_NO_ARGS:
			/* open stdin */
			files = xmalloc (sizeof *files);
			files[0] = xstrdup ("-");
			num_files = 1;
			return 0;
		case ARGP_KEY_ARGS:
			files = state->argv + state->next;
			num_files = state->argc - state->next;
			return 0;
	}
	return ARGP_ERR_UNKNOWN;
}

static struct argp argp = { options, parse_opt, args_doc };

int main (int argc, char *argv[])
{
	char *multiple_locale = NULL, *internal_locale, *all_locales;
	char *manp;
	int i;

	set_program_name (argv[0]);

	init_debug ();
	pipeline_install_post_fork (pop_all_cleanups);
	sandbox = sandbox_init ();
	init_locale ();

	internal_locale = setlocale (LC_MESSAGES, NULL);
	/* Use LANGUAGE only when LC_MESSAGES locale category is
	 * neither "C" nor "POSIX". */
	if (internal_locale && strcmp (internal_locale, "C") &&
	    strcmp (internal_locale, "POSIX"))
		multiple_locale = getenv ("LANGUAGE");
	internal_locale = xstrdup (internal_locale ? internal_locale : "C");

	if (argp_parse (&argp, argc, argv, 0, 0, 0))
		exit (FAIL);

	if (multiple_locale && *multiple_locale) {
		if (internal_locale && *internal_locale)
			all_locales = xasprintf ("%s:%s",
						 multiple_locale,
						 internal_locale);
		else
			all_locales = xstrdup (multiple_locale);
	} else {
		if (internal_locale && *internal_locale)
			all_locales = xstrdup (internal_locale);
		else
			all_locales = NULL;
	}

	manp = add_nls_manpaths (get_manpath (NULL), all_locales);
	free (all_locales);

	manpathlist = create_pathlist (manp);

	/* parse files in command line order */
	for (i = 0; i < num_files; ++i) {
		if (zsoelim_open_file (files[i], manpathlist, NULL))
			continue;
		zsoelim_parse_file (manpathlist, NULL);
	}

	free_pathlist (manpathlist);
	free (manp);
	free (internal_locale);
	sandbox_free (sandbox);

	return OK;
}
