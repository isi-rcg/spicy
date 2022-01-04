/*
 * manpath.c: display either the manpath or catpath
 *  
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002 Colin Watson.
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
 * Thu Nov 17 08:29:39 GMT 1994  Wilf. (G.Wilford@ee.surrey.ac.uk) 
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>

#include "argp.h"
#include "progname.h"

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "error.h"

#include "manp.h"

int quiet = 0;

static bool cat = false;
static bool global = false;
static const char *alt_system = "";
extern char *user_config_file;

const char *argp_program_version = "manpath " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static struct argp_option options[] = {
	{ "catpath",		'c',	0,		0,	N_("show relative catpaths") },
	{ "global",		'g',	0,		0,	N_("show the entire global manpath") },
	{ "debug",		'd',	0,		0,	N_("emit debugging messages") },
	{ "quiet",		'q',	0,		0,	N_("produce fewer warnings") },
	{ "config-file",	'C',	N_("FILE"),	0,	N_("use this user configuration file") },
	{ "systems",		'm',	N_("SYSTEM"),	0,	N_("use manual pages from other systems") },
	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {
		case 'c':
			cat = true;
			return 0;
		case 'g':
			global = true;
			quiet = 1;
			return 0;
		case 'd':
			debug_level = true;
			return 0;
		case 'q':
			quiet = 1;
			return 0;
		case 'C':
			user_config_file = arg;
			return 0;
		case 'm':
			alt_system = arg;
			return 0;
		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP);
			break;
	}
	return ARGP_ERR_UNKNOWN;
}

static struct argp argp = { options, parse_opt };

/*
 * Examine user's PATH and print a reasonable MANPATH.
 */
int main (int argc, char *argv[])
{
	char *path_string;

	set_program_name (argv[0]);

	init_debug ();
	init_locale ();

	if (argp_parse (&argp, argc, argv, 0, 0, 0))
		exit (FAIL);

	path_string = get_manpath (alt_system);

	if (global) {
		path_string = get_mandb_manpath ();
		if (!path_string)
			error (FAIL, 0,
			       _("warning: no global manpaths set in "
				 "config file %s"),
			       CONFIG_FILE);
	}
	if (cat)
		path_string = cat_manpath (path_string);

	printf ("%s\n", path_string);
	exit (OK);
}
