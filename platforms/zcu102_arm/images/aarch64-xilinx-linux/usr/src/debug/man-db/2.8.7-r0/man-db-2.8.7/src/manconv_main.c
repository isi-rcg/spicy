/*
 * manconv_main.c: convert manual page from one encoding to another
 *
 * Copyright (C) 2007, 2008 Colin Watson.
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
#include <string.h>
#include <unistd.h>

#include "argp.h"
#include "progname.h"

#include "gettext.h"
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "cleanup.h"
#include "error.h"
#include "pipeline.h"
#include "decompress.h"
#include "sandbox.h"

#include "manconv.h"

int quiet = 0;
man_sandbox *sandbox;

static const char *from_codes;
static char *to_code;
static char **from_code;
static const char *filename;

static char **split_codes (const char *codestr)
{
	char *codestrtok = xstrdup (codestr);
	char *codestrtok_ptr = codestrtok;
	char *tok;
	size_t codearray_cur = 0, codearray_alloc = 0;
	char **codearray = NULL;

	for (tok = strsep (&codestrtok_ptr, ":"); tok;
	     tok = strsep (&codestrtok_ptr, ":")) {
		if (!*tok)
			continue;	/* ignore empty fields */
		if (codearray_cur >= codearray_alloc)
			codearray = x2nrealloc
				(codearray,
				 &codearray_alloc, sizeof *codearray);
		codearray[codearray_cur++] = xstrdup (tok);
	}

	if (codearray_cur >= codearray_alloc)
		codearray = x2nrealloc (codearray,
					&codearray_alloc, sizeof *codearray);
	codearray[codearray_cur] = NULL;

	free (codestrtok);

	return codearray;
}

const char *argp_program_version = "manconv " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static const char args_doc[] = N_("-f CODE[:...] -t CODE [FILENAME]");

static struct argp_option options[] = {
	{ "from-code",	'f',	N_("CODE[:...]"),
						0,	N_("possible encodings of original text") },
	{ "to-code",	't',	N_("CODE"),	0,	N_("encoding for output") },
	{ "debug",	'd',	0,		0,	N_("emit debugging messages") },
	{ "quiet",	'q',	0,		0,	N_("produce fewer warnings") },
	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	switch (key) {
		case 'f':
			from_codes = arg;
			return 0;
		case 't':
			to_code = xstrdup (arg);
			if (!strstr (to_code, "//"))
				to_code = appendstr (to_code, "//TRANSLIT",
						     (void *) 0);
			return 0;
		case 'd':
			debug_level = true;
			return 0;
		case 'q':
			quiet = 1;
			return 0;
		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP);
			break;
		case ARGP_KEY_ARG:
			if (filename)
				argp_usage (state);
			filename = arg;
			return 0;
		case ARGP_KEY_SUCCESS:
			if (!from_codes)
				argp_error (state,
					    _("must specify an input "
					      "encoding"));
			if (!to_code)
				argp_error (state,
					    _("must specify an output "
					      "encoding"));
			from_code = split_codes (from_codes);
			if (!from_code || !*from_code)
				argp_error (state,
					    _("must specify an input "
					      "encoding"));
			return 0;
	}
	return ARGP_ERR_UNKNOWN;
}

static struct argp argp = { options, parse_opt, args_doc };

int main (int argc, char *argv[])
{
	pipeline *p;
	char **try_from_code;

	set_program_name (argv[0]);

	init_debug ();
	pipeline_install_post_fork (pop_all_cleanups);
	sandbox = sandbox_init ();
	init_locale ();

	if (argp_parse (&argp, argc, argv, 0, 0, 0))
		exit (FAIL);

	if (filename) {
		p = decompress_open (filename);
		if (!p)
			error (FAIL, 0, _("can't open %s"), filename);
	} else
		p = decompress_fdopen (dup (STDIN_FILENO));
	pipeline_start (p);

	manconv (p, from_code, to_code);

	for (try_from_code = from_code; *try_from_code; ++try_from_code)
		free (*try_from_code);
	free (to_code);
	free (from_code);

	pipeline_wait (p);

	sandbox_free (sandbox);

	return 0;
}
