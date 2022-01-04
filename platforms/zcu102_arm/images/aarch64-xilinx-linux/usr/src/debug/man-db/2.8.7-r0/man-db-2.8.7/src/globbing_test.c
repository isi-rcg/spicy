/*
 * globbing_test.c: test program for file-finding functions
 *  
 * Copyright (C) 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2006, 2007, 2008 Colin Watson.
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
#include <stdio.h>
#include <stdlib.h>

#include "argp.h"
#include "gl_list.h"
#include "progname.h"

#include "gettext.h"
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "error.h"
#include "glcontainers.h"
#include "globbing.h"

extern const char *extension;
static bool match_case = false;
static bool regex_opt = false;
static bool wildcard = false;
static char **remaining_args;

const char *argp_program_version = "globbing " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static const char args_doc[] = N_("PATH SECTION NAME");

static struct argp_option options[] = {
	{ "debug",		'd',	0,			0,	N_("emit debugging messages") },
	{ "extension",		'e',	N_("EXTENSION"),	0,	N_("limit search to extension type EXTENSION") },
	{ "ignore-case",	'i',	0,			0,	N_("look for pages case-insensitively (default)") },
	{ "match-case",		'I',	0,			0,	N_("look for pages case-sensitively") },
	{ "regex",		'r',	0,			0,	N_("interpret page name as a regex") },
	{ "wildcard",		'w',	0,			0,	N_("the page name contains wildcards") },
	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {
		case 'd':
			debug_level = true;
			return 0;
		case 'e':
			extension = arg;
			return 0;
		case 'i':
			match_case = false;
			return 0;
		case 'I':
			match_case = true;
			return 0;
		case 'r':
			regex_opt = true;
			return 0;
		case 'w':
			wildcard = true;
			return 0;
		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP);
			break;
		case ARGP_KEY_ARGS:
			if (state->argc - state->next != 3)
				argp_usage (state);
			remaining_args = state->argv + state->next;
			return 0;
	}
	return ARGP_ERR_UNKNOWN;
}

static struct argp argp = { options, parse_opt, args_doc };

int main (int argc, char **argv)
{
	int i;

	set_program_name (argv[0]);

	init_debug ();
	init_locale ();

	if (argp_parse (&argp, argc, argv, 0, 0, 0))
		exit (FAIL);

	for (i = 0; i <= 1; i++) {
		gl_list_t files;
		const char *file;

		files = look_for_file (remaining_args[0], remaining_args[1],
				       remaining_args[2], i,
				       (match_case ? LFF_MATCHCASE : 0) |
				       (regex_opt ? LFF_REGEX : 0) |
				       (wildcard ? LFF_WILDCARD : 0));
		GL_LIST_FOREACH_START (files, file)
			printf ("%s\n", file);
		GL_LIST_FOREACH_END (files);
		gl_list_free (files);
	}
	return 0;
}
