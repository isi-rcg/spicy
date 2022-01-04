/*
 * catman.c: create and/or update cat files
 *  
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2006, 2007, 2008, 2009, 2010, 2011
 *               Colin Watson.
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
 * Thu Dec  8 00:03:12 GMT 1994  Wilf. (G.Wilford@ee.surrey.ac.uk) 
 */

/* MAX_ARGS must be >= 7, 5 for options, 1 for page and 1 for NULL */
#define MAX_ARGS	1024

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>  

#ifndef NAME_MAX
#  if defined(_POSIX_VERSION) && defined(_POSIX_NAME_MAX)
#    define NAME_MAX _POSIX_NAME_MAX
#  else /* !_POSIX_VERSION */
#    ifdef MAXNAMLEN
#      define NAME_MAX MAXNAMLEN
#    else /* !MAXNAMLEN */
#      define NAME_MAX 255 		/* default to max */
#    endif /* MAXNAMLEN */
#  endif /* _POSIX_VERSION */
#endif /* !NAME_MAX */

#ifndef ARG_MAX
#  if defined(_POSIX_VERSION) && defined(_POSIX_ARG_MAX)
#    define ARG_MAX _POSIX_ARG_MAX
#  else /* !_POSIX_VERSION */
#    define ARG_MAX 4096 		/* default to min */
#  endif /* _POSIX_VERSION */
#endif /* !ARG_MAX */

#include "argp.h"
#include "gl_list.h"
#include "progname.h"

#include "gettext.h"
#include <locale.h>
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "cleanup.h"
#include "error.h"
#include "glcontainers.h"
#include "pipeline.h"

#include "mydbm.h"
#include "db_storage.h"

#include "manp.h"

/* globals */
int quiet = 1;
MYDBM_FILE dbf_close_post_fork;
char *manp;
extern char *user_config_file;

static const char **sections;

const char *argp_program_version = "catman " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static const char args_doc[] = N_("[SECTION...]");

static struct argp_option options[] = {
	{ "debug",		'd',	0,		0,	N_("emit debugging messages") },
	{ "manpath",		'M',	N_("PATH"),	0,	N_("set search path for manual pages to PATH") },
	{ "config-file",	'C',	N_("FILE"),	0,	N_("use this user configuration file") },
	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	char *mansect;

	switch (key) {
		case 'd':
			debug_level = true;
			return 0;
		case 'M':
			manp = arg;
			return 0;
		case 'C':
			user_config_file = arg;
			return 0;
		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP);
			break;
		case ARGP_KEY_ARGS:
			sections = xmalloc ((state->argc - state->next + 1) *
					    sizeof *sections);
			memcpy (sections, state->argv + state->next,
				(state->argc - state->next) *
				sizeof *sections);
			sections[state->argc - state->next] = NULL;
			return 0;
		case ARGP_KEY_NO_ARGS:
			mansect = getenv ("MANSECT");
			if (mansect && *mansect) {
				/* MANSECT contains sections */
				const char *sec;
				int i = 0;

				mansect = xstrdup (mansect);
				sections = NULL;
				for (sec = strtok (mansect, ":"); sec;
				     sec = strtok (NULL, ":")) {
					sections = xnrealloc
						(sections, i + 2,
						 sizeof *sections);
					sections[i++] = sec;
				}
				if (sections)
					sections[i] = NULL;
				free (mansect);
			} else {
				/* use default sections */
				static const char *std_sections[] =
					STD_SECTIONS;
				sections = std_sections;
			}
			return 0;
	}
	return ARGP_ERR_UNKNOWN;
}

static struct argp argp = { options, parse_opt, args_doc };

static char *locale;

static gl_list_t manpathlist;

static void post_fork (void)
{
	pop_all_cleanups ();
	if (dbf_close_post_fork)
		MYDBM_CLOSE (dbf_close_post_fork);
}

/* Execute man with the appropriate catman args.  Always frees cmd. */
static void catman (pipecmd *cmd)
{
	pipeline *p;
	int status;

	if (debug_level) {
		/* just show the command, but don't execute it */
		fputs ("man command = ", stderr);
		pipecmd_dump (cmd, stderr);
		putc ('\n', stderr);
		pipecmd_free (cmd);
		return;
	}

	p = pipeline_new_commands (cmd, (void *) 0);
	status = pipeline_run (p);
	if (status)
		error (CHILD_FAIL, 0,
		       _("man command failed with exit status %d"), status);
}

/* Add key to this command, stripping off tab-and-following if necessary.
 * Return length of argument.
 */
static size_t add_arg (pipecmd *cmd, datum key)
{
	char *tab;
	size_t len;

	tab = strrchr (MYDBM_DPTR (key), '\t');
	if (tab == MYDBM_DPTR (key))
		tab = NULL;

	if (tab)
		*tab = '\0';
	pipecmd_arg (cmd, MYDBM_DPTR (key));
	len = strlen (MYDBM_DPTR (key));
	debug ("key: '%s' (%zu), len: %zu\n",
	       MYDBM_DPTR (key), (size_t) MYDBM_DSIZE (key), len);
	if (tab)
		*tab = '\t';

	return len;
}

/* find all pages that are in the supplied manpath and section and that are
   ultimate source files. */
static int parse_for_sec (const char *database,
			  const char *manpath, const char *section)
{
	MYDBM_FILE dbf;
	pipecmd *basecmd, *cmd;
	datum key;
	size_t arg_size, initial_bit;
	int message = 1, first_arg;

	dbf = MYDBM_RDOPEN (database);
	if (!dbf) {
		error (0, errno, _("cannot read database %s"), database);
		return 1;
	}
	if (dbver_rd (dbf)) {
		MYDBM_CLOSE (dbf);
		return 1;
	}
	dbf_close_post_fork = dbf;

	basecmd = pipecmd_new (MAN);
	pipecmd_clearenv (basecmd);

	/* As we supply a NULL environment to save precious execve() space,
	   we must also supply a locale if necessary */
	if (locale) {
		pipecmd_args (basecmd, "-L", locale, (void *) 0);
		initial_bit = sizeof "-L" + strlen (locale) + 1;
	} else
		initial_bit = 0;

	pipecmd_args (basecmd, "-caM", manpath, (void *) 0);	/* manpath */
	pipecmd_args (basecmd, "-S", section, (void *) 0);	/* section */

	initial_bit += sizeof MAN + sizeof "-caM" +
		       strlen (manpath) + strlen (section) + 2;

	cmd = pipecmd_dup (basecmd);
	first_arg = pipecmd_get_nargs (cmd);

	arg_size = initial_bit;
	key = MYDBM_FIRSTKEY (dbf);

	while (MYDBM_DPTR (key) != NULL) {
		datum nextkey;

		/* ignore db identifier keys */
		if (*MYDBM_DPTR (key) != '$') { 
			datum content;

			content = MYDBM_FETCH (dbf, key);

			if (!MYDBM_DPTR (content))
				error (FATAL, 0,
				       _( "NULL content for key: %s"),
				       MYDBM_DPTR (key));

			/* ignore overflow entries */
			if (*MYDBM_DPTR (content) != '\t') { 
				struct mandata entry;

				split_content (dbf, MYDBM_DPTR (content),
					       &entry);

				/* Accept if the entry is an ultimate manual
				   page and the section matches the one we're
				   currently dealing with */
				if (entry.id == ULT_MAN && 
				    strcmp (entry.sec, section) == 0) {
					if (message) {
						printf (_("\nUpdating cat files for section %s of man hierarchy %s\n"),
							section, manpath);
						message = 0;
					}

					arg_size += add_arg (cmd, key) + 1;

					debug ("arg space free: %zu bytes\n",
					       ARG_MAX - arg_size);

					/* Check to see if we have enough room 
					   to add another max sized filename 
					   and that we haven't run out of array 
					   space too */ 
				    	if (arg_size >= ARG_MAX - NAME_MAX ||
				    	    pipecmd_get_nargs (cmd) ==
						    MAX_ARGS) {
						catman (cmd);

						cmd = pipecmd_dup (basecmd);
				    		arg_size = initial_bit;
				    	}
				}

				/* == MYDBM_DPTR (content), freed below */
				entry.addr = NULL;
				free_mandata_elements (&entry);
			}
			
			/* we don't need the content ever again */
			assert (MYDBM_DPTR (content)); /* just to be sure */
			MYDBM_FREE_DPTR (content);
		}

		nextkey = MYDBM_NEXTKEY (dbf, key);
		MYDBM_FREE_DPTR (key);
		key = nextkey;
	}

	dbf_close_post_fork = NULL;
	MYDBM_CLOSE (dbf);
	if (pipecmd_get_nargs (cmd) > first_arg)
		catman (cmd);
	else
		pipecmd_free (cmd);

	pipecmd_free (basecmd);

	return 0;
}

static int check_access (const char *directory)
{
	if (!CAN_ACCESS (directory, W_OK)) {
		error (0, errno, _("cannot write within %s"), directory);
		return 1;
	}

	return 0;
}
		
int main (int argc, char *argv[])
{
	char *sys_manp;
	char *mp;
	const char **sp;

	set_program_name (argv[0]);

	init_debug ();
	pipeline_install_post_fork (post_fork);

	init_locale ();
	locale = setlocale (LC_MESSAGES, NULL);
	if (locale)
		locale = xstrdup (locale);
	else
		locale = xstrdup ("C");

	if (argp_parse (&argp, argc, argv, 0, 0, 0))
		exit (FAIL);

	for (sp = sections; *sp; sp++)
		debug ("sections: %s\n", *sp);

	/* Deal with the MANPATH */

	/* This is required for get_catpath(), regardless */
	sys_manp = get_manpath (NULL);

	/* pick up the system manpath or use the supplied one */
	if (!manp) {
		manp = get_mandb_manpath ();
		if (!manp)
			manp = sys_manp;
	}

	debug ("manpath=%s\n", manp);

	/* get the manpath as a list of pointers */
	manpathlist = create_pathlist (manp); 

	GL_LIST_FOREACH_START (manpathlist, mp) {
		char *catpath, *database;
		size_t len;

		catpath = get_catpath (mp, SYSTEM_CAT | USER_CAT);

		if (catpath) { 
			if (is_directory (catpath) != 1) {
				free (catpath);
				continue;
			}
			database = mkdbname (catpath);
		} else {
			if (is_directory (mp) != 1)
				continue;
			database = mkdbname (mp);
			catpath = xstrdup (mp);
		}

		len = strlen (catpath);
		
		for (sp = sections; *sp; sp++) {
			*(catpath + len) = '\0';
			catpath = appendstr (catpath, "/cat", *sp, (void *) 0);
			if (is_directory (catpath) != 1)
				continue;
			if (check_access (catpath))
				continue;
			if (parse_for_sec (database, mp, *sp)) {
				error (0, 0, _("unable to update %s"), mp);
				break;
			}
		}

		free (database);
		free (catpath);
	} GL_LIST_FOREACH_END (manpathlist);

	free_pathlist (manpathlist);
	free (locale);
	exit (OK);
}
