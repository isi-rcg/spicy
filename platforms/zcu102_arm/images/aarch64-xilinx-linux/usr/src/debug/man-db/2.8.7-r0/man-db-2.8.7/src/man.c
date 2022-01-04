/*
 * man.c: The manual pager
 *
 * Copyright (C) 1990, 1991 John W. Eaton.
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010,
 *               2011, 2012 Colin Watson.
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
 * John W. Eaton
 * jwe@che.utexas.edu
 * Department of Chemical Engineering
 * The University of Texas at Austin
 * Austin, Texas  78712
 *
 * Mostly written/re-written by Wilf, some routines by Markus Armbruster.
 *
 * CJW: Various robustness, security, and internationalization fixes.
 * Improved HTML support (originally written by Fabrizio Polacco).
 * Rewrite of page location routines for improved maintainability and
 * accuracy.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "argp.h"
#include "dirname.h"
#include "gl_array_list.h"
#include "gl_hash_map.h"
#include "gl_list.h"
#include "gl_xlist.h"
#include "gl_xmap.h"
#include "minmax.h"
#include "progname.h"
#include "regex.h"
#include "stat-time.h"
#include "utimens.h"
#include "xgetcwd.h"
#include "xvasprintf.h"
#include "xstdopen.h"

#include "gettext.h"
#include <locale.h>
#define _(String) gettext (String)
#define N_(String) gettext_noop (String)

#include "manconfig.h"

#include "error.h"
#include "cleanup.h"
#include "glcontainers.h"
#include "pipeline.h"
#include "pathsearch.h"
#include "linelength.h"
#include "decompress.h"
#include "xregcomp.h"
#include "security.h"
#include "encodings.h"
#include "orderfiles.h"
#include "sandbox.h"

#include "mydbm.h"
#include "db_storage.h"

#include "filenames.h"
#include "globbing.h"
#include "ult_src.h"
#include "manp.h"
#include "zsoelim.h"
#include "manconv_client.h"

#ifdef MAN_OWNER
extern uid_t ruid;
extern uid_t euid;
#endif /* MAN_OWNER */

/* the default preprocessor sequence */
#ifndef DEFAULT_MANROFFSEQ
#  define DEFAULT_MANROFFSEQ ""
#endif

/* placeholder for the manual page name in the less prompt string */
#define MAN_PN "$MAN_PN"

/* Some systems lack these */
#ifndef STDIN_FILENO
#  define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
#  define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
#  define STDERR_FILENO 2
#endif

char *lang;

/* external formatter programs, one for use without -t, and one with -t */
#define NFMT_PROG "mandb_nfmt"
#define TFMT_PROG "mandb_tfmt"
#undef ALT_EXT_FORMAT	/* allow external formatters located in cat hierarchy */

static bool global_manpath;	/* global or user manual page hierarchy? */
static int skip;		/* page exists but has been skipped */

#if defined _AIX || defined __sgi
char **global_argv;
#endif

struct candidate {
	const char *req_name;
	char from_db;
	char cat;
	const char *path;
	char *ult;
	struct mandata *source;
	int add_index; /* for sort stabilisation */
	struct candidate *next;
};

#define CANDIDATE_FILESYSTEM 0
#define CANDIDATE_DATABASE   1

static void gripe_system (pipeline *p, int status)
{
	error (CHILD_FAIL, 0, _("command exited with status %d: %s"),
	       status, pipeline_tostring (p));
}

enum opts {
	OPT_WARNINGS = 256,
	OPT_REGEX,
	OPT_WILDCARD,
	OPT_NAMES,
	OPT_NO_HYPHENATION,
	OPT_NO_JUSTIFICATION,
	OPT_NO_SUBPAGES,
	OPT_MAX
};

static gl_list_t manpathlist;

/* globals */
int quiet = 1;
char *database = NULL;
extern const char *extension; /* for globbing.c */
extern char *user_config_file;	/* defined in manp.c */
extern bool disable_cache;
extern int min_cat_width, max_cat_width, cat_width;
man_sandbox *sandbox;

/* locals */
static const char *alt_system_name;
static gl_list_t section_list;		
static const char *section;
static char *colon_sep_section_list;
static const char *preprocessors;
static const char *pager;
static const char *locale;
static char *internal_locale, *multiple_locale;
static const char *prompt_string;
static char *less;
static const char *std_sections[] = STD_SECTIONS;
static char *manp;
static const char *external;
static gl_map_t db_map = NULL;

static bool troff;
static const char *roff_device = NULL;
static const char *want_encoding = NULL;
#ifdef NROFF_WARNINGS
static const char default_roff_warnings[] = "mac";
static gl_list_t roff_warnings;
#endif /* NROFF_WARNINGS */
static bool global_apropos;
static bool print_where, print_where_cat;
static bool catman;
static bool local_man_file;
static bool findall;
static bool update;
static bool match_case;
static bool regex_opt;
static bool wildcard;
static bool names_only;
static int ult_flags = SO_LINK | SOFT_LINK | HARD_LINK;
static const char *recode = NULL;
static bool no_hyphenation;
static bool no_justification;
static bool subpages = true;

static bool ascii;		/* insert tr in the output pipe */
static bool save_cat; 		/* security breach? Can we save the cat? */

static int first_arg;

static int found_a_stray;		/* found a straycat */

#ifdef MAN_CATS
static char *tmp_cat_file;	/* for open_cat_stream(), close_cat_stream() */
static int created_tmp_cat;			/* dto. */
#endif
static int tmp_cat_fd;
static struct timespec man_modtime;	/* modtime of man page, for
					 * commit_tmp_cat() */

# ifdef TROFF_IS_GROFF
static bool ditroff;
static const char *gxditview;
static bool htmlout;
static const char *html_pager;
# endif /* TROFF_IS_GROFF */

const char *argp_program_version = "man " PACKAGE_VERSION;
const char *argp_program_bug_address = PACKAGE_BUGREPORT;
error_t argp_err_exit_status = FAIL;

static const char args_doc[] = N_("[SECTION] PAGE...");

# ifdef NROFF_WARNINGS
#  define ONLY_NROFF_WARNINGS 0
# else
#  define ONLY_NROFF_WARNINGS OPTION_HIDDEN
# endif

# ifdef TROFF_IS_GROFF
#  define ONLY_TROFF_IS_GROFF 0
# else
#  define ONLY_TROFF_IS_GROFF OPTION_HIDDEN
# endif

/* Please keep these options in the same order as in parse_opt below. */
static struct argp_option options[] = {
	{ "config-file",	'C',	N_("FILE"),	0,		N_("use this user configuration file") },
	{ "debug",		'd',	0,		0,		N_("emit debugging messages") },
	{ "default",		'D',	0,		0,		N_("reset all options to their default values") },
	{ "warnings",  OPT_WARNINGS,    N_("WARNINGS"), ONLY_NROFF_WARNINGS | OPTION_ARG_OPTIONAL,
									N_("enable warnings from groff") },

	{ 0,			0,	0,		0,		N_("Main modes of operation:"),					10 },
	{ "whatis",		'f',	0,		0,		N_("equivalent to whatis") },
	{ "apropos",		'k',	0,		0,		N_("equivalent to apropos") },
	{ "global-apropos",	'K',	0,		0,		N_("search for text in all pages") },
	{ "where",		'w',	0,		0,		N_("print physical location of man page(s)") },
	{ "path",		0,	0,		OPTION_ALIAS },
	{ "location",		0,	0,		OPTION_ALIAS },
	{ "where-cat",		'W',	0,		0,		N_("print physical location of cat file(s)") },
	{ "location-cat",	0,	0,		OPTION_ALIAS },
	{ "local-file",		'l',	0,		0,		N_("interpret PAGE argument(s) as local filename(s)") },
	{ "catman",		'c',	0,		0,		N_("used by catman to reformat out of date cat pages"),		11 },
	{ "recode",		'R',	N_("ENCODING"),	0,		N_("output source page encoded in ENCODING") },

	{ 0,			0,	0,		0,		N_("Finding manual pages:"),					20 },
	{ "locale",		'L',	N_("LOCALE"),	0,		N_("define the locale for this particular man search") },
	{ "systems",		'm',	N_("SYSTEM"),	0,		N_("use manual pages from other systems") },
	{ "manpath",		'M',	N_("PATH"),	0,		N_("set search path for manual pages to PATH") },
	{ "sections",		'S',	N_("LIST"),	0,		N_("use colon separated section list"),				21 },
	{ 0,			's',	0,		OPTION_ALIAS },
	{ "extension",		'e',	N_("EXTENSION"),
							0,		N_("limit search to extension type EXTENSION"),			22 },
	{ "ignore-case",	'i',	0,		0,		N_("look for pages case-insensitively (default)"),		23 },
	{ "match-case",		'I',	0,		0,		N_("look for pages case-sensitively") },
	{ "regex",	  OPT_REGEX,	0,		0,		N_("show all pages matching regex"),				24 },
	{ "wildcard",  OPT_WILDCARD,	0,		0,		N_("show all pages matching wildcard") },
	{ "names-only",	  OPT_NAMES,	0,		0,		N_("make --regex and --wildcard match page names only, not "
									   "descriptions"),						25 },
	{ "all",		'a',	0,		0,		N_("find all matching manual pages"),				26 },
	{ "update",		'u',	0,		0,		N_("force a cache consistency check") },
	{ "no-subpages",
		    OPT_NO_SUBPAGES,	0,		0,		N_("don't try subpages, e.g. 'man foo bar' => 'man foo-bar'"),	27 },

	{ 0,			0,	0,		0,		N_("Controlling formatted output:"),				30 },
	{ "pager",		'P',	N_("PAGER"),	0,		N_("use program PAGER to display output") },
	{ "prompt",		'r',	N_("STRING"),	0,		N_("provide the `less' pager with a prompt") },
	{ "ascii",		'7',	0,		0,		N_("display ASCII translation of certain latin1 chars"),	31 },
	{ "encoding",		'E',	N_("ENCODING"),	0,		N_("use selected output encoding") },
	{ "no-hyphenation",
		 OPT_NO_HYPHENATION,	0,		0,		N_("turn off hyphenation") },
	{ "nh",			0,	0,		OPTION_ALIAS },
	{ "no-justification",
	       OPT_NO_JUSTIFICATION,	0,		0,		N_("turn off justification") },
	{ "nj",			0,	0,		OPTION_ALIAS },
	{ "preprocessor",	'p',	N_("STRING"),	0,		N_("STRING indicates which preprocessors to run:\n"
									   "e - [n]eqn, p - pic, t - tbl,\n"
									   "g - grap, r - refer, v - vgrind") },
#ifdef HAS_TROFF
	{ "troff",		't',	0,		0,		N_("use %s to format pages"),					32 },
	{ "troff-device",	'T',	N_("DEVICE"),	OPTION_ARG_OPTIONAL,
									N_("use %s with selected device") },
	{ "html",		'H',	N_("BROWSER"),	ONLY_TROFF_IS_GROFF | OPTION_ARG_OPTIONAL,
									N_("use %s or BROWSER to display HTML output"),			33 },
	{ "gxditview",		'X',	N_("RESOLUTION"),
							ONLY_TROFF_IS_GROFF | OPTION_ARG_OPTIONAL,
									N_("use groff and display through gxditview (X11):\n"
									   "-X = -TX75, -X100 = -TX100, -X100-12 = -TX100-12") },
	{ "ditroff",		'Z',	0,		ONLY_TROFF_IS_GROFF,	N_("use groff and force it to produce ditroff") },
#endif /* HAS_TROFF */

	{ 0, 'h', 0, OPTION_HIDDEN, 0 }, /* compatibility for --help */
	{ 0 }
};

static void init_html_pager (void)
{
	html_pager = getenv ("BROWSER");
	if (!html_pager)
		html_pager = WEB_BROWSER;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
	static bool apropos, whatis; /* retain values between calls */

	/* Please keep these keys in the same order as in options above. */
	switch (key) {
		case 'C':
			user_config_file = arg;
			return 0;
		case 'd':
			debug_level = true;
			return 0;
		case 'D':
			/* discard all preset options */
			local_man_file = findall = update = catman =
				debug_level = troff = global_apropos =
				print_where = print_where_cat =
				ascii = match_case =
				regex_opt = wildcard = names_only =
				no_hyphenation = no_justification = false;
#ifdef TROFF_IS_GROFF
			ditroff = false;
			gxditview = NULL;
			htmlout = false;
			init_html_pager ();
#endif
			roff_device = want_encoding = extension = pager =
				locale = alt_system_name = external =
				preprocessors = NULL;
			colon_sep_section_list = manp = NULL;
			return 0;

		case OPT_WARNINGS:
#ifdef NROFF_WARNINGS
			{
				char *s = xstrdup
					(arg ? arg : default_roff_warnings);
				const char *warning;

				for (warning = strtok (s, ","); warning;
				     warning = strtok (NULL, ","))
					gl_list_add_last (roff_warnings,
							  xstrdup (warning));

				free (s);
			}
#endif /* NROFF_WARNINGS */
			return 0;

		case 'f':
			external = WHATIS;
			whatis = true;
			return 0;
		case 'k':
			external = APROPOS;
			apropos = true;
			return 0;
		case 'K':
			global_apropos = true;
			return 0;
		case 'w':
			print_where = true;
			return 0;
		case 'W':
			print_where_cat = true;
			return 0;
		case 'l':
			local_man_file = true;
			return 0;
		case 'c':
			catman = true;
			return 0;
		case 'R':
			recode = arg;
			ult_flags &= ~SO_LINK;
			return 0;

		case 'L':
			locale = arg;
			return 0;
		case 'm':
			alt_system_name = arg;
			return 0;
		case 'M':
			manp = arg;
			return 0;
		case 'S':
		case 's':
			if (*arg)
				colon_sep_section_list = arg;
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
		case OPT_REGEX:
			regex_opt = true;
			findall = true;
			return 0;
		case OPT_WILDCARD:
			wildcard = true;
			findall = true;
			return 0;
		case OPT_NAMES:
			names_only = true;
			return 0;
		case 'a':
			findall = true;
			return 0;
		case 'u':
			update = true;
			return 0;
		case OPT_NO_SUBPAGES:
			subpages = false;
			return 0;

		case 'P':
			pager = arg;
			return 0;
		case 'r':
			prompt_string = arg;
			return 0;
		case '7':
			ascii = true;
			return 0;
		case 'E':
			want_encoding = arg;
			if (is_roff_device (want_encoding))
				roff_device = want_encoding;
			return 0;
		case OPT_NO_HYPHENATION:
			no_hyphenation = true;
			return 0;
		case OPT_NO_JUSTIFICATION:
			no_justification = true;
			return 0;
		case 'p':
			preprocessors = arg;
			return 0;
#ifdef HAS_TROFF
		case 't':
			troff = true;
			return 0;
		case 'T':
			/* Traditional nroff knows -T; troff does not (gets
			 * ignored). All incarnations of groff know it. Why
			 * does -T imply -t?
			 */
			roff_device = (arg ? arg : "ps");
			troff = true;
			return 0;
		case 'H':
# ifdef TROFF_IS_GROFF
			if (arg)
				html_pager = arg;
			htmlout = true;
			troff = true;
			roff_device = "html";
# endif /* TROFF_IS_GROFF */
			return 0;
		case 'X':
# ifdef TROFF_IS_GROFF
			troff = true;
			gxditview = (arg ? arg : "75");
# endif /* TROFF_IS_GROFF */
			return 0;
		case 'Z':
# ifdef TROFF_IS_GROFF
			ditroff = true;
			troff = true;
# endif /* TROFF_IS_GROFF */
			return 0;
#endif /* HAS_TROFF */

		case 'h':
			argp_state_help (state, state->out_stream,
					 ARGP_HELP_STD_HELP);
			break;
		case ARGP_KEY_SUCCESS:
			/* check for incompatible options */
			if ((int) troff + (int) whatis + (int) apropos +
			    (int) catman +
			    (int) (print_where || print_where_cat) > 1) {
				char *badopts = xasprintf
					("%s%s%s%s%s%s",
					 troff ? "-[tTZH] " : "",
					 whatis ? "-f " : "",
					 apropos ? "-k " : "",
					 catman ? "-c " : "",
					 print_where ? "-w " : "",
					 print_where_cat ? "-W " : "");
				argp_error (state,
					    _("%s: incompatible options"),
					    badopts);
			}
			if ((int) regex_opt + (int) wildcard > 1) {
				char *badopts = xasprintf
					("%s%s",
					 regex_opt ? "--regex " : "",
					 wildcard ? "--wildcard " : "");
				argp_error (state,
					    _("%s: incompatible options"),
					    badopts);
			}
			return 0;
	}
	return ARGP_ERR_UNKNOWN;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
static char *help_filter (int key, const char *text, void *input _GL_UNUSED)
{
#ifdef HAS_TROFF
# ifdef TROFF_IS_GROFF
	static const char formatter[] = "groff";
	const char *browser;
# else
	static const char formatter[] = "troff";
# endif /* TROFF_IS_GROFF */
#endif /* HAS_TROFF */

	switch (key) {
#ifdef HAS_TROFF
		case 't':
		case 'T':
			return xasprintf (text, formatter);
# ifdef TROFF_IS_GROFF
		case 'H':
			browser = html_pager;
			assert (browser);
			if (STRNEQ (browser, "exec ", 5))
				browser += 5;
			return xasprintf (text, browser);
# endif /* TROFF_IS_GROFF */
#endif /* HAS_TROFF */
		default:
			return (char *) text;
	}
}
#pragma GCC diagnostic pop

static struct argp argp = { options, parse_opt, args_doc, 0, 0, help_filter };

/*
 * changed these messages from stdout to stderr,
 * (Fabrizio Polacco) Fri, 14 Feb 1997 01:30:07 +0200
 */
static void gripe_no_name (const char *sect)
{
	if (sect) {
		fprintf (stderr, _("No manual entry for %s\n"), sect);
		fprintf (stderr,
			 _("(Alternatively, what manual page do you want from "
			   "section %s?)\n"),
			 sect);
	} else
		fputs (_("What manual page do you want?\n"), stderr);

	exit (FAIL);
}

static struct termios tms;
static int tms_set = 0;
static pid_t tms_pid = 0;

static void set_term (void)
{
	if (tms_set && getpid () == tms_pid)
		tcsetattr (STDIN_FILENO, TCSANOW, &tms);
}

static void get_term (void)
{
	if (isatty (STDOUT_FILENO)) {
		debug ("is a tty\n");
		tcgetattr (STDIN_FILENO, &tms);
		if (!tms_set++) {
			/* Work around pipecmd_exec calling exit(3) rather
			 * than _exit(2), which means our atexit-registered
			 * functions are called at the end of each child
			 * process created using pipecmd_new_function and
			 * friends.  It would probably be good to fix this
			 * in libpipeline at some point, but it would
			 * require care to avoid breaking compatibility.
			 */
			tms_pid = getpid ();
			atexit (set_term);
		}
	}
}

#if defined(TROFF_IS_GROFF) || defined(HEIRLOOM_NROFF)
static int get_roff_line_length (void)
{
	int line_length = cat_width ? cat_width : get_line_length ();

	/* groff >= 1.18 defaults to 78. */
	if ((!troff || ditroff) && line_length != 80) {
		int length = line_length * 39 / 40;
		if (length > line_length - 2)
			return line_length - 2;
		else
			return length;
	} else
		return 0;
}

#ifdef HEIRLOOM_NROFF
static void heirloom_line_length (void *data)
{
	printf (".ll %sn\n", (const char *) data);
	/* TODO: This fails to do anything useful.  Why? */
	printf (".lt %sn\n", (const char *) data);
	printf (".lf 1\n");
}
#endif /* HEIRLOOM_NROFF */

static pipecmd *add_roff_line_length (pipecmd *cmd, bool *save_cat_p)
{
	int length;
	pipecmd *ret = NULL;

	if (!catman) {
		int line_length = get_line_length ();
		debug ("Terminal width %d\n", line_length);
		if (line_length >= min_cat_width &&
		    line_length <= max_cat_width)
			debug ("Terminal width %d within cat page range "
			       "[%d, %d]\n",
			       line_length, min_cat_width, max_cat_width);
		else {
			debug ("Terminal width %d not within cat page range "
			       "[%d, %d]\n",
			       line_length, min_cat_width, max_cat_width);
			*save_cat_p = false;
		}
	}

	length = get_roff_line_length ();
	if (length) {
#ifdef HEIRLOOM_NROFF
		char *name;
		char *lldata;
		pipecmd *llcmd;
#endif /* HEIRLOOM_NROFF */

		debug ("Using %d-character lines\n", length);
#if defined(TROFF_IS_GROFF)
		pipecmd_argf (cmd, "-rLL=%dn", length);
		pipecmd_argf (cmd, "-rLT=%dn", length);
#elif defined(HEIRLOOM_NROFF)
		name = xasprintf ("echo .ll %dn && echo .lt %dn",
				  length, length);
		lldata = xasprintf ("%d", length);
		llcmd = pipecmd_new_function (name, heirloom_line_length, free,
					      lldata);
		ret = pipecmd_new_sequence ("line-length", llcmd,
					    pipecmd_new_passthrough (), NULL);
		free (name);
#endif /* HEIRLOOM_NROFF */
	}

	return ret;
}
#endif /* TROFF_IS_GROFF || HEIRLOOM_NROFF */

static void gripe_no_man (const char *name, const char *sec)
{
	/* On AIX and IRIX, fall back to the vendor supplied browser. */
#if defined _AIX || defined __sgi
	if (!troff) {
		pipecmd *vendor_man;
		int i;

		vendor_man = pipecmd_new ("/usr/bin/man");
		for (i = 1; i < argc; ++i)
			pipecmd_arg (vendor_man, global_argv[i]);
		pipecmd_unsetenv (vendor_man, "MANPATH");
		pipecmd_exec (vendor_man);
	}
#endif

	if (sec)
		fprintf (stderr, _("No manual entry for %s in section %s\n"),
			 name, sec);
	else
		fprintf (stderr, _("No manual entry for %s\n"), name);

#ifdef UNDOC_COMMAND
	if (getenv ("MAN_TEST_DISABLE_UNDOCUMENTED") == NULL &&
	    pathsearch_executable (name))
		fprintf (stderr,
			 _("See '%s' for help when manual pages are not "
			   "available.\n"), UNDOC_COMMAND);
#endif
}

/* fire up the appropriate external program */
static void do_extern (int argc, char *argv[])
{
	pipeline *p;
	pipecmd *cmd;

	cmd = pipecmd_new (external);
	/* Please keep these in the same order as they are in whatis.c. */
	if (debug_level)
		pipecmd_arg (cmd, "-d");
	if (local_man_file)  /* actually apropos/whatis --long */
		pipecmd_arg (cmd, "-l");
	if (colon_sep_section_list)
		pipecmd_args (cmd, "-s", colon_sep_section_list, (void *) 0);
	if (alt_system_name)
		pipecmd_args (cmd, "-m", alt_system_name, (void *) 0);
	if (manp)
		pipecmd_args (cmd, "-M", manp, (void *) 0);
	if (locale)
		pipecmd_args (cmd, "-L", locale, (void *) 0);
	if (user_config_file)
		pipecmd_args (cmd, "-C", user_config_file, (void *) 0);
	while (first_arg < argc)
		pipecmd_arg (cmd, argv[first_arg++]);
	p = pipeline_new_commands (cmd, (void *) 0);

	/* privs are already dropped */
	exit (pipeline_run (p));
}

/* lookup $MANOPT and if available, put in *argv[] format for argp */
static char **manopt_to_env (int *argc)
{
	char *manopt, *manopt_copy, *opt_start, **argv;

	manopt = getenv ("MANOPT");
	if (manopt == NULL || *manopt == '\0')
		return NULL;

	opt_start = manopt = manopt_copy = xstrdup (manopt);

	/* allocate space for the program name */
	*argc = 0;
	argv = XNMALLOC (*argc + 3, char *);
	argv[(*argc)++] = base_name (program_name);
	
	/* for each [ \t]+ delimited string, allocate an array space and fill
	   it in. An escaped space is treated specially */	
	while (*manopt) {
		switch (*manopt) {
			case ' ':
			case '\t':
				if (manopt != opt_start) {
					*manopt = '\0';
					argv = xnrealloc (argv, *argc + 3,
							  sizeof (char *));
					argv[(*argc)++] = xstrdup (opt_start);
				}
				while (CTYPE (isspace, *(manopt + 1)))
					*++manopt = '\0';
				opt_start = manopt + 1;
				break;
			case '\\':
				if (*(manopt + 1) == ' ')
					manopt++;
				break;
			default:
				break;
		}
		manopt++;
	}

	if (*opt_start)
		argv[(*argc)++] = xstrdup (opt_start);
	argv[*argc] = NULL;			

	free (manopt_copy);
	return argv;
}

/* Return char array with 'less' special chars escaped. Uses static storage. */
static const char *escape_less (const char *string)
{
	static char *escaped_string; 
	char *ptr;

	/* 2*strlen will always be long enough to hold the escaped string */
	ptr = escaped_string = xrealloc (escaped_string, 
					 2 * strlen (string) + 1);
	
	while (*string) {
		if (*string == '?' ||
		    *string == ':' ||
		    *string == '.' ||
		    *string == '%' ||
		    *string == '\\')
			*ptr++ = '\\';

		*ptr++ = *string++;
	}

	*ptr = *string;
	return escaped_string;
}

#if defined(MAN_DB_CREATES) || defined(MAN_DB_UPDATES)
/* Run mandb to ensure databases are up to date. Only used with -u.
 * Returns the exit status of mandb.
 *
 * If filename is non-NULL, uses mandb's -f option to update a single file.
 */
static int run_mandb (int create, const char *manpath, const char *filename)
{
	pipeline *mandb_pl = pipeline_new ();
	pipecmd *mandb_cmd = pipecmd_new ("mandb");

	if (debug_level)
		pipecmd_arg (mandb_cmd, "-d");
	else
		pipecmd_arg (mandb_cmd, "-q");

	if (user_config_file)
		pipecmd_args (mandb_cmd, "-C", user_config_file, (void *) 0);

	if (filename)
		pipecmd_args (mandb_cmd, "-f", filename, (void *) 0);
	else if (create) {
		pipecmd_arg (mandb_cmd, "-c");
		pipecmd_setenv (mandb_cmd, "MAN_MUST_CREATE", "1");
	} else
		pipecmd_arg (mandb_cmd, "-p");

	if (manpath)
		pipecmd_arg (mandb_cmd, manpath);

	pipeline_command (mandb_pl, mandb_cmd);

	if (debug_level) {
		debug ("running mandb: ");
		pipeline_dump (mandb_pl, stderr);
	}

	return pipeline_run (mandb_pl);
}
#endif /* MAN_DB_CREATES || MAN_DB_UPDATES */


static char *locale_manpath (const char *manpath)
{
	char *all_locales;
	char *new_manpath;

	if (multiple_locale && *multiple_locale) {
		if (internal_locale && *internal_locale)
			all_locales = xasprintf ("%s:%s", multiple_locale,
						 internal_locale);
		else
			all_locales = xstrdup (multiple_locale);
	} else {
		if (internal_locale && *internal_locale)
			all_locales = xstrdup (internal_locale);
		else
			all_locales = NULL;
	}

	new_manpath = add_nls_manpaths (manpath, all_locales);
	free (all_locales);

	return new_manpath;
}

/*
 * Check to see if the argument is a valid section number. 
 * If the name matches one of
 * the sections listed in section_list, we'll assume that it's a section.
 * The list of sections in config.h simply allows us to specify oddly
 * named directories like .../man3f.  Yuk.
 */
static const char *is_section (const char *name)
{
	const char *vs;

	GL_LIST_FOREACH_START (section_list, vs) {
		if (STREQ (vs, name))
			return name;
		/* allow e.g. 3perl but disallow 8139too and libfoo */
		if (strlen (vs) == 1 && CTYPE (isdigit, *vs) &&
		    strlen (name) > 1 && !CTYPE (isdigit, name[1]) &&
		    STRNEQ (vs, name, 1))
			return name;
	} GL_LIST_FOREACH_END (section_list);
	return NULL;
}

/* Snarf pre-processors from file, return string or NULL on failure */
static char *get_preprocessors_from_file (pipeline *decomp, int prefixes)
{
#ifdef PP_COOKIE
	const size_t block = 4096;
	int i;
	char *line = NULL;
	size_t previous_len = 0;

	if (!decomp)
		return NULL;

	/* Prefixes are inserted into the stream by man itself, and we must
	 * skip over them to find any preprocessors line that exists.  Each
	 * one ends with an .lf macro.
	 */
	for (i = 0; ; ++i) {
		size_t len = block * (i + 1);
		const char *buffer, *scan, *end;
		int j;

		scan = buffer = pipeline_peek (decomp, &len);
		if (!buffer || len == 0)
			return NULL;

		for (j = 0; j < prefixes; ++j) {
			scan = memmem (scan, len - (scan - buffer),
				       "\n.lf ", strlen ("\n.lf "));
			if (!scan)
				break;
			++scan;
			scan = memchr (scan, '\n', len - (scan - buffer));
			if (!scan)
				break;
			++scan;
		}
		if (!scan)
			continue;

		end = memchr (scan, '\n', len - (scan - buffer));
		if (!end && len == previous_len)
			/* end of file, no newline found */
			end = buffer + len - 1;
		if (end) {
			line = xstrndup (scan, end - scan + 1);
			break;
		}
		previous_len = len;
	}
	if (!line)
		return NULL;

	if (!strncmp (line, PP_COOKIE, 4)) {
		const char *newline = strchr (line, '\n');
		if (newline)
			return xstrndup (line + 4, newline - (line + 4));
		else
			return xstrdup (line + 4);
	}
#endif
	return NULL;
}


/* Determine pre-processors, set save_cat and return string */
static char *get_preprocessors (pipeline *decomp, const char *dbfilters,
				int prefixes)
{
	char *pp_string;
	const char *pp_source;
	const char *env;

	/* try in order: database, command line, file, environment, default */
	/* command line overrides the database, but database empty overrides default */
	if (dbfilters && (dbfilters[0] != '-') && !preprocessors) {
		pp_string = xstrdup (dbfilters);
		pp_source = "database";
		save_cat = true;
	} else if (preprocessors) {
		pp_string = xstrdup (preprocessors);
		pp_source = "command line";
		save_cat = false;
	} else if ((pp_string = get_preprocessors_from_file (decomp,
							     prefixes))) {
		pp_source = "file";
		save_cat = true;
	} else if ((env = getenv ("MANROFFSEQ"))) {
		pp_string = xstrdup (env);
		pp_source = "environment";
		save_cat = false;
	} else if (!dbfilters) {
		pp_string = xstrdup (DEFAULT_MANROFFSEQ);
		pp_source = "default";
		save_cat = true;
	} else {
		pp_string = xstrdup ("");
		pp_source = "no filters";
		save_cat = true;
	}

	debug ("pre-processors `%s' from %s\n", pp_string, pp_source);
	return pp_string;
}

static const char *my_locale_charset (void)
{
	if (want_encoding && !is_roff_device (want_encoding))
		return want_encoding;
	else
		return get_locale_charset ();
}

static void add_col (pipeline *p, const char *locale_charset, ...)
{
	pipecmd *cmd;
	va_list argv;
	char *col_locale = NULL;

	cmd = pipecmd_new (COL);
	va_start (argv, locale_charset);
	pipecmd_argv (cmd, argv);
	va_end (argv);
	pipecmd_pre_exec (cmd, sandbox_load, sandbox_free, sandbox);

	if (locale_charset)
		col_locale = find_charset_locale (locale_charset);
	if (col_locale) {
		pipecmd_setenv (cmd, "LC_CTYPE", col_locale);
		free (col_locale);
	}

	pipeline_command (p, cmd);
}

/* Return pipeline to format file to stdout. */
static pipeline *make_roff_command (const char *dir, const char *file,
				    pipeline *decomp, const char *pp_string,
				    char **result_encoding)
{
	const char *roff_opt;
	char *fmt_prog = NULL;
	pipeline *p = pipeline_new ();
	pipecmd *cmd;
	char *page_encoding = NULL;
	const char *output_encoding = NULL;
	const char *locale_charset = NULL;

	*result_encoding = xstrdup ("UTF-8"); /* optimistic default */

	roff_opt = getenv ("MANROFFOPT");
	if (!roff_opt)
		roff_opt = "";

	if (dir && !recode) {
#ifdef ALT_EXT_FORMAT
		char *catpath = get_catpath
			(dir, global_manpath ? SYSTEM_CAT : USER_CAT);

		/* If we have an alternate catpath, look for an external
		 * formatter there.
		 */
		if (catpath) {
			fmt_prog = appendstr (catpath, "/",
					      troff ? TFMT_PROG : NFMT_PROG, 
					      (void *) 0);
			if (!CAN_ACCESS (fmt_prog, X_OK)) {
				free (fmt_prog);
				fmt_prog = NULL;
			}
		}
#endif /* ALT_EXT_FORMAT */

		/* If the page is in a proper manual page hierarchy (as
		 * opposed to being read using --local-file or similar),
		 * look for an external formatter there.
		 */
		if (!fmt_prog) {
			fmt_prog = appendstr (NULL, dir, "/",
					      troff ? TFMT_PROG : NFMT_PROG,
					      (void *) 0);
			if (!CAN_ACCESS (fmt_prog, X_OK)) {
				free (fmt_prog);
				fmt_prog = NULL;
			}
		}
	}

	if (fmt_prog)
		debug ("External formatter %s\n", fmt_prog);
				
	if (!fmt_prog) {
		/* we don't have an external formatter script */
		const char *source_encoding, *roff_encoding;
		const char *groff_preconv;

		if (!recode) {
			struct zsoelim_stdin_data *zsoelim_data;

			zsoelim_data = zsoelim_stdin_data_new (dir,
							       manpathlist);
			cmd = pipecmd_new_function (ZSOELIM, &zsoelim_stdin,
						    zsoelim_stdin_data_free,
						    zsoelim_data);
			pipecmd_pre_exec (cmd, sandbox_load, sandbox_free,
					  sandbox);
			pipeline_command (p, cmd);
		}

		page_encoding = check_preprocessor_encoding (decomp);
		if (!page_encoding)
			page_encoding = get_page_encoding (lang);
		if (page_encoding && !STREQ (page_encoding, "UTF-8"))
			source_encoding = page_encoding;
		else
			source_encoding = get_source_encoding (lang);
		debug ("page_encoding = %s\n", page_encoding);
		debug ("source_encoding = %s\n", source_encoding);

		/* Load the roff_device value dependent on the language dir
		 * in the path.
		 */
		if (!troff) {
#define STRC(s, otherwise) ((s) ? (s) : (otherwise))

			locale_charset = my_locale_charset ();
			debug ("locale_charset = %s\n",
			       STRC (locale_charset, "NULL"));

			/* Pick the default device for this locale if there
			 * wasn't one selected explicitly.
			 */
			if (!roff_device) {
				roff_device =
					get_default_device (locale_charset,
							    source_encoding);
#ifdef HEIRLOOM_NROFF
				/* In Heirloom, if LC_CTYPE is a UTF-8
				 * locale, then -Tlocale will be equivalent
				 * to -Tutf8 except that it will do a
				 * slightly better job of rendering some
				 * special characters.
				 */
				if (STREQ (roff_device, "utf8")) {
					const char *real_locale_charset =
						get_locale_charset ();
					if (real_locale_charset &&
					    STREQ (real_locale_charset,
						   "UTF-8"))
						roff_device = "locale";
				}
#endif /* HEIRLOOM_NROFF */
				debug ("roff_device (locale) = %s\n",
				       STRC (roff_device, "NULL"));
			}
		}

		roff_encoding = get_roff_encoding (roff_device,
						   source_encoding);
		debug ("roff_encoding = %s\n", roff_encoding);

		/* We may need to recode:
		 *   from page_encoding to roff_encoding on input;
		 *   from output_encoding to locale_charset on output
		 *     (if not troff).
		 * If we have preconv, then use it to recode the
		 * input to a safe escaped form.
		 * The --recode option overrides everything else.
		 */
		groff_preconv = get_groff_preconv ();
		if (recode)
			add_manconv (p, page_encoding, recode);
		else if (groff_preconv) {
			pipecmd *preconv_cmd;
			add_manconv (p, page_encoding, "UTF-8");
			preconv_cmd = pipecmd_new_args
				(groff_preconv, "-e", "UTF-8", (void *) 0);
			pipecmd_pre_exec (preconv_cmd, sandbox_load,
					  sandbox_free, sandbox);
			pipeline_command (p, preconv_cmd);
		} else if (roff_encoding)
			add_manconv (p, page_encoding, roff_encoding);
		else
			add_manconv (p, page_encoding, page_encoding);

		if (!troff && !recode) {
			output_encoding = get_output_encoding (roff_device);
			if (!output_encoding)
				output_encoding = source_encoding;
			debug ("output_encoding = %s\n", output_encoding);
			free (*result_encoding);
			*result_encoding = xstrdup (output_encoding);

			if (!getenv ("LESSCHARSET")) {
				const char *less_charset =
					get_less_charset (locale_charset);
				debug ("less_charset = %s\n", less_charset);
				setenv ("LESSCHARSET", less_charset, 1);
			}

			if (!getenv ("JLESSCHARSET")) {
				const char *jless_charset =
					get_jless_charset (locale_charset);
				if (jless_charset) {
					debug ("jless_charset = %s\n",
					       jless_charset);
					setenv ("JLESSCHARSET",
						jless_charset, 1);
				}
			}
		}
	}

	if (recode)
		;
	else if (!fmt_prog) {
#ifndef GNU_NROFF
		int using_tbl = 0;
#endif /* GNU_NROFF */

		do {
#ifdef NROFF_WARNINGS
			const char *warning;
#endif /* NROFF_WARNINGS */
			int wants_dev = 0; /* filter wants a dev argument */
			int wants_post = 0; /* postprocessor arguments */

			cmd = NULL;
			/* set cmd according to *pp_string, on
                           errors leave cmd as NULL */
			switch (*pp_string) {
			case 'e':
				if (troff)
					cmd = pipecmd_new_argstr
						(get_def ("eqn", EQN));
				else
					cmd = pipecmd_new_argstr
						(get_def ("neqn", NEQN));
				wants_dev = 1;
				break;
			case 'g':
				cmd = pipecmd_new_argstr
					(get_def ("grap", GRAP));
				break;
			case 'p':
				cmd = pipecmd_new_argstr
					(get_def ("pic", PIC));
				break;
			case 't':
				cmd = pipecmd_new_argstr
					(get_def ("tbl", TBL));
#ifndef GNU_NROFF
				using_tbl = 1;
#endif /* GNU_NROFF */
				break;
			case 'v':
				cmd = pipecmd_new_argstr
					(get_def ("vgrind", VGRIND));
				break;
			case 'r':
				cmd = pipecmd_new_argstr
					(get_def ("refer", REFER));
				break;
			case ' ':
			case '-':
			case 0:
				/* done with preprocessors, now add roff */
				if (troff) {
					cmd = pipecmd_new_argstr
						(get_def ("troff", TROFF));
					save_cat = false;
				} else
					cmd = pipecmd_new_argstr
						(get_def ("nroff", NROFF));

#ifdef TROFF_IS_GROFF
				if (troff && ditroff)
					pipecmd_arg (cmd, "-Z");
#endif /* TROFF_IS_GROFF */

#if defined(TROFF_IS_GROFF) || defined(HEIRLOOM_NROFF)
				{
					pipecmd *seq = add_roff_line_length
						(cmd, &save_cat);
					if (seq)
						pipeline_command (p, seq);
				}
#endif /* TROFF_IS_GROFF || HEIRLOOM_NROFF */

#ifdef NROFF_WARNINGS
				GL_LIST_FOREACH_START (roff_warnings, warning)
					pipecmd_argf (cmd, "-w%s", warning);
				GL_LIST_FOREACH_END (roff_warnings);
#endif /* NROFF_WARNINGS */

#ifdef HEIRLOOM_NROFF
				if (running_setuid ())
					pipecmd_unsetenv (cmd, "TROFFMACS");
#endif /* HEIRLOOM_NROFF */

				pipecmd_argstr (cmd, roff_opt);

				wants_dev = 1;
				wants_post = 1;
				break;
			}

			if (!cmd) {
				assert (*pp_string); /* didn't fail on roff */
				error (0, 0,
				       _("ignoring unknown preprocessor `%c'"),
				       *pp_string);
				continue;
			}

			if (wants_dev) {
				if (roff_device)
					pipecmd_argf (cmd,
						      "-T%s", roff_device);
#ifdef TROFF_IS_GROFF
				else if (gxditview)
					pipecmd_argf (cmd, "-TX%s", gxditview);
#endif /* TROFF_IS_GROFF */
			}

			if (wants_post) {
#ifdef TROFF_IS_GROFF
				if (gxditview)
					pipecmd_arg (cmd, "-X");
#endif /* TROFF_IS_GROFF */

				if (roff_device && STREQ (roff_device, "ps"))
					/* Tell grops to guess the page
					 * size.
					 */
					pipecmd_arg (cmd, "-P-g");
			}

			pipecmd_pre_exec (cmd, sandbox_load_permissive,
					  sandbox_free, sandbox);
			pipeline_command (p, cmd);

			if (*pp_string == ' ' || *pp_string == '-')
				break;
		} while (*pp_string++);

		if (!troff && *COL) {
			const char *man_keep_formatting =
				getenv ("MAN_KEEP_FORMATTING");
			if ((!man_keep_formatting || !*man_keep_formatting) &&
			    !isatty (STDOUT_FILENO))
				/* we'll run col later, but prepare for it */
				setenv ("GROFF_NO_SGR", "1", 1);
#ifndef GNU_NROFF
			/* tbl needs col */
			else if (using_tbl && !troff && *COL)
				add_col (p, locale_charset, (void *) 0);
#endif /* GNU_NROFF */
		}
	} else {
		/* use external formatter script, it takes arguments
		   input file, preprocessor string, and (optional)
		   output device */
		cmd = pipecmd_new_args (fmt_prog, file, pp_string, (void *) 0);
		if (roff_device)
			pipecmd_arg (cmd, roff_device);
		pipeline_command (p, cmd);
	}

	free (fmt_prog);
	free (page_encoding);
	return p;
}

#ifdef TROFF_IS_GROFF
/* Return pipeline to run a browser on a given file, observing
 * http://www.tuxedo.org/~esr/BROWSER/.
 *
 * (Actually, I really implement
 * https://www.dwheeler.com/browse/secure_browser.html, but it's
 * backward-compatible.)
 *
 * TODO: Is there any way to use the pipeline library better here?
 */
static pipeline *make_browser (const char *pattern, const char *file)
{
	pipeline *p;
	pipecmd *cmd;
	char *browser = xmalloc (1);
	bool found_percent_s = false;
	char *percent;
	char *esc_file;

	*browser = '\0';

	percent = strchr (pattern, '%');
	while (percent) {
		size_t len = strlen (browser);
		browser = xrealloc (browser, len + 1 + (percent - pattern));
		strncat (browser, pattern, percent - pattern);
		switch (*(percent + 1)) {
			case '\0':
			case '%':
				browser = appendstr (browser, "%", (void *) 0);
				break;
			case 'c':
				browser = appendstr (browser, ":", (void *) 0);
				break;
			case 's':
				esc_file = escape_shell (file);
				browser = appendstr (browser, esc_file,
						     (void *) 0);
				free (esc_file);
				found_percent_s = true;
				break;
			default:
				len = strlen (browser); /* cannot be NULL */
				browser = xrealloc (browser, len + 3);
				strncat (browser, percent, 2);
				break;
		}
		if (*(percent + 1))
			pattern = percent + 2;
		else
			pattern = percent + 1;
		percent = strchr (pattern, '%');
	}
	browser = appendstr (browser, pattern, (void *) 0);
	if (!found_percent_s) {
		esc_file = escape_shell (file);
		browser = appendstr (browser, " ", esc_file, (void *) 0);
		free (esc_file);
	}

	cmd = pipecmd_new_args ("/bin/sh", "-c", browser, (void *) 0);
	pipecmd_pre_exec (cmd, drop_privs, NULL, NULL);
	p = pipeline_new_commands (cmd, (void *) 0);
	pipeline_ignore_signals (p, 1);
	free (browser);

	return p;
}
#endif /* TROFF_IS_GROFF */

static void setenv_less (pipecmd *cmd, const char *title)
{
	const char *esc_title;
	char *less_opts, *man_pn;

	esc_title = escape_less (title);
	less_opts = xasprintf (LESS_OPTS, prompt_string, prompt_string);
	less_opts = appendstr (less_opts, less, (void *) 0);
	man_pn = strstr (less_opts, MAN_PN);
	while (man_pn) {
		char *subst_opts =
			xmalloc (strlen (less_opts) - strlen (MAN_PN) +
				 strlen (esc_title) + 1);
		strncpy (subst_opts, less_opts, man_pn - less_opts);
		subst_opts[man_pn - less_opts] = '\0';
		strcat (subst_opts, esc_title);
		strcat (subst_opts, man_pn + strlen (MAN_PN));
		free (less_opts);
		less_opts = subst_opts;
		man_pn = strstr (less_opts, MAN_PN);
	}

	debug ("Setting LESS to %s\n", less_opts);
	pipecmd_setenv (cmd, "LESS", less_opts);

	debug ("Setting MAN_PN to %s\n", esc_title);
	pipecmd_setenv (cmd, "MAN_PN", esc_title);

	free (less_opts);
}

static void add_output_iconv (pipeline *p,
			      const char *source, const char *target)
{
	debug ("add_output_iconv: source %s, target %s\n", source, target);
	if (source && target && !STREQ (source, target)) {
		char *target_translit = xasprintf ("%s//TRANSLIT", target);
		pipecmd *iconv_cmd;
		iconv_cmd = pipecmd_new_args
			("iconv", "-c", "-f", source, "-t", target_translit,
			 (void *) 0);
		pipecmd_pre_exec (iconv_cmd, sandbox_load, sandbox_free,
				  sandbox);
		pipeline_command (p, iconv_cmd);
		free (target_translit);
	}
}

/* Pipeline command to squeeze multiple blank lines into one.
 *
 */
static void squeeze_blank_lines (void *data _GL_UNUSED)
{
	char *line = NULL;
	size_t len = 0;

	while (getline (&line, &len, stdin) != -1) {
		int in_blank_line  = 1;
		int got_blank_line = 0;

		while (in_blank_line) {
			char *p;
			for (p = line; *p; ++p) {
				if (!CTYPE (isspace, *p)) {
					in_blank_line = 0;
					break;
				}
			}

			if (in_blank_line) {
				got_blank_line = 1;
				free (line);
				line = NULL;
				len  = 0;
				if (getline (&line, &len, stdin) == -1)
					break;
			}
		}

		if (got_blank_line && putchar ('\n') < 0)
			break;

		if (!in_blank_line && fputs (line, stdout) < 0)
			break;

		free (line);
		line = NULL;
		len  = 0;
	}

	free (line);
	return;
}

/* Return pipeline to display file provided on stdin.
 *
 * TODO: htmlout case is pretty weird now. I'd like the intelligence to be
 * somewhere other than format_display.
 */
static pipeline *make_display_command (const char *encoding, const char *title)
{
	pipeline *p = pipeline_new ();
	const char *locale_charset = NULL;
	pipecmd *pager_cmd = NULL;

	locale_charset = my_locale_charset ();

	if (!troff && (!want_encoding || !is_roff_device (want_encoding)))
		add_output_iconv (p, encoding, locale_charset);

	if (!troff && *COL) {
		/* get rid of special characters if not writing to a
		 * terminal
		 */
		const char *man_keep_formatting =
			getenv ("MAN_KEEP_FORMATTING");
		if ((!man_keep_formatting || !*man_keep_formatting) &&
		    !isatty (STDOUT_FILENO))
			add_col (p, locale_charset, "-b", "-p", "-x",
				 (void *) 0);
	}

	/* emulate pager -s, the sed code is just for information */
	{
		pipecmd *cmd;
		const char *name = "sed -e '/^[[:space:]]*$/{ N; /^[[:space:]]*\\n[[:space:]]*$/D; }'";
		cmd = pipecmd_new_function (name, &squeeze_blank_lines, NULL, NULL);
		pipeline_command (p, cmd);
	}

	if (isatty (STDOUT_FILENO)) {
		if (ascii) {
			pipecmd *tr_cmd;
			tr_cmd = pipecmd_new_argstr
				(get_def_user ("tr", TR TR_SET1 TR_SET2));
			pipecmd_pre_exec (tr_cmd, sandbox_load, sandbox_free,
					  sandbox);
			pipeline_command (p, tr_cmd);
			pager_cmd = pipecmd_new_argstr (pager);
		} else
#ifdef TROFF_IS_GROFF
		if (!htmlout)
			/* format_display deals with html_pager */
#endif
			pager_cmd = pipecmd_new_argstr (pager);
	}

	if (pager_cmd) {
		setenv_less (pager_cmd, title);
		pipeline_command (p, pager_cmd);
	}
	pipeline_ignore_signals (p, 1);

	if (!pipeline_get_ncommands (p))
		/* Always return at least a dummy pipeline. */
		pipeline_command (p, pipecmd_new_passthrough ());
	return p;
}


/* return a (malloced) temporary name in cat_file's directory */
static char *tmp_cat_filename (const char *cat_file)
{
	char *name;

	if (debug_level) {
		name = xstrdup ("/dev/null");
		tmp_cat_fd = open (name, O_WRONLY);
	} else {
		char *slash;
		name = xstrdup (cat_file);
		slash = strrchr (name, '/');
		if (slash)
			*(slash + 1) = '\0';
		else
			*name = '\0';
		name = appendstr (name, "catXXXXXX", (void *) 0);
		tmp_cat_fd = mkstemp (name);
	}

	if (tmp_cat_fd == -1) {
		free (name);
		return NULL;
	} else
		return name;
}


/* If delete unlink tmp_cat, else commit tmp_cat to cat_file.
   Return non-zero on error.
 */
static int commit_tmp_cat (const char *cat_file, const char *tmp_cat,
			   int delete)
{
	int status = 0;

#ifdef MAN_OWNER
	if (!delete && global_manpath && euid == 0) {
		if (debug_level) {
			debug ("fixing temporary cat's ownership\n");
			status = 0;
		} else {
			struct passwd *man_owner = get_man_owner ();
			status = chown (tmp_cat, man_owner->pw_uid,
					man_owner->pw_gid);
			if (status)
				error (0, errno, _("can't chown %s"), tmp_cat);
		}
	}
#endif /* MAN_OWNER */

	if (!delete && !status) {
		if (debug_level) {
			debug ("fixing temporary cat's mode\n");
			status = 0;
		} else {
			status = chmod (tmp_cat, CATMODE);
			if (status)
				error (0, errno, _("can't chmod %s"), tmp_cat);
		}
	}

	if (!delete && !status) {
		if (debug_level) {
			debug ("renaming temporary cat to %s\n", cat_file);
			status = 0;
		} else {
			status = rename (tmp_cat, cat_file);
			if (status)
				error (0, errno, _("can't rename %s to %s"),
				       tmp_cat, cat_file);
		}
	}

	if (!delete && !status) {
		if (debug_level) {
			debug ("setting modtime on cat file %s\n", cat_file);
			status = 0;
		} else {
			struct timespec times[2];

			times[0].tv_sec = 0;
			times[0].tv_nsec = UTIME_NOW;
			times[1] = man_modtime;
			status = utimens (cat_file, times);
			if (status)
				error (0, errno, _("can't set times on %s"),
				       cat_file);
		}
	}

	if (delete || status) {
		if (debug_level)
			debug ("unlinking temporary cat\n");
		else if (unlink (tmp_cat))
			error (0, errno, _("can't unlink %s"), tmp_cat);
	}

	return status;
}

/* TODO: This should all be refactored after work on the decompression
 * library is complete.
 */
static void discard_stderr (pipeline *p)
{
	int i;

	for (i = 0; i < pipeline_get_ncommands (p); ++i)
		pipecmd_discard_err (pipeline_get_command (p, i), 1);
}

static void maybe_discard_stderr (pipeline *p)
{
	const char *man_keep_stderr = getenv ("MAN_KEEP_STDERR");
	if ((!man_keep_stderr || !*man_keep_stderr) && isatty (STDOUT_FILENO))
		discard_stderr (p);
}

static void chdir_commands (pipeline *p, const char *dir)
{
	int i;

	for (i = 0; i < pipeline_get_ncommands (p); ++i)
		pipecmd_chdir (pipeline_get_command (p, i), dir);
}

static void cleanup_unlink (void *arg)
{
	const char *path = arg;

	if (unlink (path))
		error (0, errno, _("can't unlink %s"), path);
}

#ifdef MAN_CATS

/* Return pipeline to write formatted manual page to for saving as cat file. */
static pipeline *open_cat_stream (const char *cat_file, const char *encoding)
{
	pipeline *cat_p;
#  ifdef COMP_CAT
	pipecmd *comp_cmd;
#  endif

	created_tmp_cat = 0;

	debug ("creating temporary cat for %s\n", cat_file);

	tmp_cat_file = tmp_cat_filename (cat_file);
	if (tmp_cat_file)
		created_tmp_cat = 1;
	else {
		if (!debug_level && (errno == EACCES || errno == EROFS)) {
			/* No permission to write to the cat file. Oh well,
			 * return NULL and let the caller sort it out.
			 */
			debug ("can't write to temporary cat for %s\n",
			       cat_file);
			return NULL;
		} else
			error (FATAL, errno,
			       _("can't create temporary cat for %s"),
			       cat_file);
	}

	if (!debug_level)
		push_cleanup (cleanup_unlink, tmp_cat_file, 1);

	cat_p = pipeline_new ();
	add_output_iconv (cat_p, encoding, "UTF-8");
#  ifdef COMP_CAT
	/* fork the compressor */
	comp_cmd = pipecmd_new_argstr (get_def ("compressor", COMPRESSOR));
	pipecmd_nice (comp_cmd, 10);
	pipecmd_pre_exec (comp_cmd, sandbox_load, sandbox_free, sandbox);
	pipeline_command (cat_p, comp_cmd);
#  endif
	/* pipeline_start will close tmp_cat_fd */
	pipeline_want_out (cat_p, tmp_cat_fd);

	return cat_p;
}

/* Close the cat page stream, return non-zero on error.
   If delete don't update the cat file.
 */
static int close_cat_stream (pipeline *cat_p, const char *cat_file,
			     int delete)
{
	int status;

	status = pipeline_wait (cat_p);
	debug ("cat-saver exited with status %d\n", status);

	pipeline_free (cat_p);

	if (created_tmp_cat) {
		status |= commit_tmp_cat (cat_file, tmp_cat_file,
					  delete || status);
		if (!debug_level)
			pop_cleanup (cleanup_unlink, tmp_cat_file);
	}
	free (tmp_cat_file);
	return status;
}

/*
 * format a manual page with format_cmd, display it with disp_cmd, and
 * save it to cat_file
 */
static int format_display_and_save (pipeline *decomp,
				    pipeline *format_cmd,
				    pipeline *disp_cmd,
				    const char *cat_file, const char *encoding)
{
	pipeline *sav_p = open_cat_stream (cat_file, encoding);
	int instat;

	if (global_manpath)
		drop_effective_privs ();

	maybe_discard_stderr (format_cmd);

	pipeline_connect (decomp, format_cmd, (void *) 0);
	if (sav_p) {
		pipeline_connect (format_cmd, disp_cmd, sav_p, (void *) 0);
		pipeline_pump (decomp, format_cmd, disp_cmd, sav_p,
			       (void *) 0);
	} else {
		pipeline_connect (format_cmd, disp_cmd, (void *) 0);
		pipeline_pump (decomp, format_cmd, disp_cmd, (void *) 0);
	}

	if (global_manpath)
		regain_effective_privs ();

	pipeline_wait (decomp);
	instat = pipeline_wait (format_cmd);
	if (sav_p)
		close_cat_stream (sav_p, cat_file, instat);
	pipeline_wait (disp_cmd);
	return instat;
}
#endif /* MAN_CATS */

/* Format a manual page with format_cmd and display it with disp_cmd.
 * Handle temporary file creation if necessary.
 * TODO: merge with format_display_and_save
 */
static void format_display (pipeline *decomp,
			    pipeline *format_cmd, pipeline *disp_cmd,
			    const char *man_file)
{
	int format_status = 0, disp_status = 0;
#ifdef TROFF_IS_GROFF
	char *htmldir = NULL, *htmlfile = NULL;
#endif /* TROFF_IS_GROFF */

	if (format_cmd)
		maybe_discard_stderr (format_cmd);

	drop_effective_privs ();

#ifdef TROFF_IS_GROFF
	if (format_cmd && htmlout) {
		char *man_base, *man_ext;
		int htmlfd;

		htmldir = create_tempdir ("hman");
		if (!htmldir)
			error (FATAL, errno,
			       _("can't create temporary directory"));
		chdir_commands (format_cmd, htmldir);
		chdir_commands (disp_cmd, htmldir);
		man_base = base_name (man_file);
		man_ext = strchr (man_base, '.');
		if (man_ext)
			*man_ext = '\0';
		htmlfile = xasprintf ("%s/%s.html", htmldir, man_base);
		free (man_base);
		htmlfd = open (htmlfile, O_CREAT | O_EXCL | O_WRONLY, 0644);
		if (htmlfd == -1)
			error (FATAL, errno, _("can't open temporary file %s"),
			       htmlfile);
		pipeline_want_out (format_cmd, htmlfd);
		pipeline_connect (decomp, format_cmd, (void *) 0);
		pipeline_pump (decomp, format_cmd, (void *) 0);
		pipeline_wait (decomp);
		format_status = pipeline_wait (format_cmd);
	} else
#endif /* TROFF_IS_GROFF */
	    if (format_cmd) {
		pipeline_connect (decomp, format_cmd, (void *) 0);
		pipeline_connect (format_cmd, disp_cmd, (void *) 0);
		pipeline_pump (decomp, format_cmd, disp_cmd, (void *) 0);
		pipeline_wait (decomp);
		format_status = pipeline_wait (format_cmd);
		disp_status = pipeline_wait (disp_cmd);
	} else {
		pipeline_connect (decomp, disp_cmd, (void *) 0);
		pipeline_pump (decomp, disp_cmd, (void *) 0);
		pipeline_wait (decomp);
		disp_status = pipeline_wait (disp_cmd);
	}

#ifdef TROFF_IS_GROFF
	if (format_cmd && htmlout) {
		char *browser_list, *candidate;

		if (format_status) {
			if (remove_directory (htmldir, 0) == -1)
				error (0, errno,
				       _("can't remove directory %s"),
				       htmldir);
			free (htmlfile);
			free (htmldir);
			gripe_system (format_cmd, format_status);
		}

		browser_list = xstrdup (html_pager);
		for (candidate = strtok (browser_list, ":"); candidate;
		     candidate = strtok (NULL, ":")) {
			pipeline *browser;
			debug ("Trying browser: %s\n", candidate);
			browser = make_browser (candidate, htmlfile);
			disp_status = pipeline_run (browser);
			if (!disp_status)
				break;
		}
		if (!candidate) {
			if (html_pager && *html_pager)
				error (CHILD_FAIL, 0,
				       "couldn't execute any browser from %s",
				       html_pager);
			else
				error (CHILD_FAIL, 0,
				       "no browser configured, so cannot show "
				       "HTML output");
		}
		free (browser_list);
		if (remove_directory (htmldir, 0) == -1)
			error (0, errno, _("can't remove directory %s"),
			       htmldir);
		free (htmlfile);
		free (htmldir);
	} else
#endif /* TROFF_IS_GROFF */
	{
		if (format_status && format_status != (SIGPIPE + 0x80) * 256)
			gripe_system (format_cmd, format_status);
		if (disp_status && disp_status != (SIGPIPE + 0x80) * 256)
			gripe_system (disp_cmd, disp_status);
	}

	regain_effective_privs ();
}

/* "Display" a page in catman mode, which amounts to saving it. */
/* TODO: merge with format_display_and_save? */
static void display_catman (const char *cat_file, pipeline *decomp,
			    pipeline *format_cmd, const char *encoding)
{
	char *tmpcat = tmp_cat_filename (cat_file);
#ifdef COMP_CAT
	pipecmd *comp_cmd;
#endif /* COMP_CAT */
	int status;

	add_output_iconv (format_cmd, encoding, "UTF-8");

#ifdef COMP_CAT
	comp_cmd = pipecmd_new_argstr (get_def ("compressor", COMPRESSOR));
	pipecmd_pre_exec (comp_cmd, sandbox_load, sandbox_free, sandbox);
	pipeline_command (format_cmd, comp_cmd);
#endif /* COMP_CAT */

	maybe_discard_stderr (format_cmd);
	pipeline_want_out (format_cmd, tmp_cat_fd);

	push_cleanup (cleanup_unlink, tmpcat, 1);

	/* save the cat as real user
	 * (1) required for user man hierarchy
	 * (2) else depending on ruid's privs is ok, effectively disables
	 *     catman for non-root.
	 */
	drop_effective_privs ();
	pipeline_connect (decomp, format_cmd, (void *) 0);
	pipeline_pump (decomp, format_cmd, (void *) 0);
	pipeline_wait (decomp);
	status = pipeline_wait (format_cmd);
	regain_effective_privs ();
	if (status)
		gripe_system (format_cmd, status);

	close (tmp_cat_fd);
	commit_tmp_cat (cat_file, tmpcat, status);
	pop_cleanup (cleanup_unlink, tmpcat);
	free (tmpcat);
}

static void disable_hyphenation (void *data _GL_UNUSED)
{
	fputs (".nh\n"
	       ".de hy\n"
	       "..\n"
	       ".lf 1\n", stdout);
}

static void disable_justification (void *data _GL_UNUSED)
{
	fputs (".na\n"
	       ".de ad\n"
	       "..\n"
	       ".lf 1\n", stdout);
}

#ifdef TROFF_IS_GROFF
static void locale_macros (void *data)
{
	const char *macro_lang = data;
	const char *hyphen_lang = STREQ (lang, "en") ? "us" : macro_lang;

	debug ("Macro language %s; hyphenation language %s\n",
	       macro_lang, hyphen_lang);

	printf (
		/* If we're using groff >= 1.20.2 (for the 'file' warning
		 * category):
		 */
		".if \\n[.g] \\{\\\n"
		".  ds Ystring \\n[.Y]\n"
		".  while (\\B'\\*[Ystring]' = 0) .chop Ystring\n"
		".  if ((\\n[.x] > 1) :"
		" ((\\n[.x] == 1) & (\\n[.y] > 20)) :"
		" ((\\n[.x] == 1) & (\\n[.y] == 20) & (\\*[Ystring] >= 2))) "
		"\\{\\\n"
		/*   disable warnings of category 'file' */
		".    warn (\\n[.warn] -"
		" (\\n[.warn] / 1048576 %% 2 * 1048576))\n"
		/*   and load the appropriate per-locale macros */
		".    mso %s.tmac\n"
		".  \\}\n"
		".  rm Ystring\n"
		".\\}\n"
		/* set the hyphenation language anyway, to make sure groff
		 * only hyphenates languages it knows about
		 */
		".hla %s\n"
		".lf 1\n", macro_lang, hyphen_lang);
}
#endif /* TROFF_IS_GROFF */

/* allow user to skip a page or quit after viewing desired page 
   return 1 to skip
   return 0 to view
 */
static int do_prompt (const char *name)
{
	int ch;
	FILE *tty = NULL;

	skip = 0;
	if (!isatty (STDOUT_FILENO) || !isatty (STDIN_FILENO))
		return 0; /* noninteractive */
	tty = fopen ("/dev/tty", "r+");
	if (!tty)
		return 0;

	fprintf (tty, _( 
		 "--Man-- next: %s "
		 "[ view (return) | skip (Ctrl-D) | quit (Ctrl-C) ]\n"), 
		 name);
	fflush (tty);

	do {
		ch = getc (tty);
		switch (ch) {
			case '\n':
				fclose (tty);
				return 0;
			case EOF:
				skip = 1;
				fclose (tty);
				return 1;
			default:
				break;
		}
	} while (1);

	fclose (tty);
	return 0;
}

/*
 * optionally chdir to dir, if necessary update cat_file from man_file
 * and display it.  if man_file is NULL cat_file is a stray cat.  If
 * !save_cat or cat_file is NULL we must not save the formatted cat.
 * If man_file is "" this is a special case -- we expect the man page
 * on standard input.
 */
static int display (const char *dir, const char *man_file,
		    const char *cat_file, const char *title,
		    const char *dbfilters)
{
	int found;
	static int prompt;
	int prefixes = 0;
	pipeline *format_cmd;	/* command to format man_file to stdout */
	char *formatted_encoding = NULL;
	bool display_to_stdout;
	pipeline *decomp = NULL;
	int decomp_errno = 0;

	/* define format_cmd */
	if (man_file) {
		pipecmd *seq = pipecmd_new_sequence ("decompressor",
						     (void *) 0);

		if (*man_file)
			decomp = decompress_open (man_file);
		else
			decomp = decompress_fdopen (dup (STDIN_FILENO));

		if (!recode && no_hyphenation) {
			pipecmd *hcmd = pipecmd_new_function (
				"echo .nh && echo .de hy && echo ..",
				disable_hyphenation, NULL, NULL);
			pipecmd_sequence_command (seq, hcmd);
			++prefixes;
		}

		if (!recode && no_justification) {
			pipecmd *jcmd = pipecmd_new_function (
				"echo .na && echo .de ad && echo ..",
				disable_justification, NULL, NULL);
			pipecmd_sequence_command (seq, jcmd);
			++prefixes;
		}

#ifdef TROFF_IS_GROFF
		/* This only works with preconv, since the per-locale macros
		 * may change the assumed input encoding.
		 */
		if (!recode && *man_file && get_groff_preconv ()) {
			char *page_lang = lang_dir (man_file);

			if (page_lang && *page_lang &&
			    !STREQ (page_lang, "C")) {
				struct locale_bits bits;
				char *name;
				pipecmd *lcmd;

				unpack_locale_bits (page_lang, &bits);
				name = xasprintf ("echo .mso %s.tmac",
						  bits.language);
				lcmd = pipecmd_new_function (
					name, locale_macros, free,
					xstrdup (bits.language));
				pipecmd_sequence_command (seq, lcmd);
				++prefixes;
				free (name);
				free_locale_bits (&bits);
			}
			free (page_lang);
		}
#endif /* TROFF_IS_GROFF */

		if (prefixes) {
			assert (pipeline_get_ncommands (decomp) <= 1);
			if (pipeline_get_ncommands (decomp)) {
				pipecmd_sequence_command
					(seq,
					 pipeline_get_command (decomp, 0));
				pipeline_set_command (decomp, 0, seq);
			} else {
				pipecmd_sequence_command
					(seq, pipecmd_new_passthrough ());
				pipeline_command (decomp, seq);
			}
		} else
			pipecmd_free (seq);
	}

	if (decomp) {
		char *pp_string;

		pipeline_start (decomp);
		pp_string = get_preprocessors (decomp, dbfilters, prefixes);
		format_cmd = make_roff_command (dir, man_file, decomp,
						pp_string,
						&formatted_encoding);
		if (dir)
			chdir_commands (format_cmd, dir);
		debug ("formatted_encoding = %s\n", formatted_encoding);
		free (pp_string);
	} else {
		format_cmd = NULL;
		decomp_errno = errno;
	}

	/* Get modification time, for commit_tmp_cat(). */
	if (man_file && *man_file) {
		struct stat stb;
		if (stat (man_file, &stb)) {
			man_modtime.tv_sec = 0;
			man_modtime.tv_nsec = 0;
		} else
			man_modtime = get_stat_mtime (&stb);
	}

	display_to_stdout = troff;
#ifdef TROFF_IS_GROFF
	if (htmlout)
		display_to_stdout = false;
#endif
	if (recode)
		display_to_stdout = true;

	if (display_to_stdout) {
		/* If we're reading stdin via '-l -', man_file is "". See
		 * below.
		 */
		assert (man_file);
		if (!decomp) {
			assert (!format_cmd); /* no need to free it */
			error (0, decomp_errno, _("can't open %s"), man_file);
			return 0;
		}
		if (*man_file == '\0')
			found = 1;
		else
			found = CAN_ACCESS (man_file, R_OK);
		if (found) {
			int status;
			if (prompt && do_prompt (title)) {
				pipeline_free (format_cmd);
				pipeline_free (decomp);
				free (formatted_encoding);
				return 0;
			}
			drop_effective_privs ();
			pipeline_connect (decomp, format_cmd, (void *) 0);
			pipeline_pump (decomp, format_cmd, (void *) 0);
			pipeline_wait (decomp);
			status = pipeline_wait (format_cmd);
			regain_effective_privs ();
			if (status != 0)
				gripe_system (format_cmd, status);
		}
	} else {
		int format = 1;
		int status;

		/* The caller should already have checked for any
		 * FSSTND-style (same hierarchy) cat page that may be
		 * present, and we don't expect to have to update the cat
		 * page in that case. If by some chance we do have to update
		 * it, then there's no harm trying; open_cat_stream() will
		 * refuse gracefully if the file isn't writeable.
		 */

		/* In theory we might be able to get away with saving cats
		 * for want_encoding, but it does change the roff device so
		 * perhaps that's best avoided.
		 */
		if (want_encoding
#ifdef TROFF_IS_GROFF
		    || htmlout
#endif
		    || local_man_file
		    || recode
		    || disable_cache
		    || no_hyphenation
		    || no_justification)
			save_cat = false;

		if (!man_file) {
			/* Stray cat. */
			assert (cat_file);
			format = 0;
		} else if (!cat_file) {
			assert (man_file);
			save_cat = false;
			format = 1;
		} else if (format && save_cat) {
			char *cat_dir;
			char *tmp;

			status = is_changed (man_file, cat_file);
			format = (status == -2) || ((status & 1) == 1);

			/* don't save if we haven't a cat directory */
			cat_dir = xstrdup (cat_file);
			tmp = strrchr (cat_dir, '/');
			if (tmp)
				*tmp = 0;
			save_cat = is_directory (cat_dir) == 1;
			if (!save_cat)
				debug ("cat dir %s does not exist\n", cat_dir);
			free (cat_dir);
		}

		if (format && (!format_cmd || !decomp)) {
			assert (man_file);
			/* format_cmd is NULL iff decomp is NULL; no need to
			 * free either of them.
			 */
			assert (!format_cmd);
			assert (!decomp);
			error (0, decomp_errno, _("can't open %s"), man_file);
			return 0;
		}

		/* if we're trying to read stdin via '-l -' then man_file
		 * will be "" which access() obviously barfs on, but all is
		 * well because the format_cmd will have been created to
		 * expect input via stdin. So we special-case this to avoid
		 * the bogus access() check.
		*/
		if (format == 1 && *man_file == '\0')
			found = 1;
		else
			found = CAN_ACCESS
				(format ? man_file : cat_file, R_OK);

		debug ("format: %d, save_cat: %d, found: %d\n",
		       format, (int) save_cat, found);

		if (!found) {
			pipeline_free (format_cmd);
			pipeline_free (decomp);
			return found;
		}

		if (print_where || print_where_cat) {
			int printed = 0;
			if (print_where && man_file) {
				printf ("%s", man_file);
				printed = 1;
			}
			if (print_where_cat && cat_file && !format) {
				if (printed)
					putchar (' ');
				printf ("%s", cat_file);
				printed = 1;
			}
			if (printed)
				putchar ('\n');
		} else if (catman) {
			if (format) {
				if (!save_cat)
					error (0, 0,
					       _("\ncannot write to "
						 "%s in catman mode"),
					       cat_file);
				else
					display_catman (cat_file, decomp,
							format_cmd,
							formatted_encoding);
			}
		} else if (format) {
			/* no cat or out of date */
			pipeline *disp_cmd;

			if (prompt && do_prompt (title)) {
				pipeline_free (format_cmd);
				pipeline_free (decomp);
				free (formatted_encoding);
				if (local_man_file)
					return 1;
				else
					return 0;
			}

			disp_cmd = make_display_command (formatted_encoding,
							 title);

#ifdef MAN_CATS
			if (save_cat) {
				/* save cat */
				assert (disp_cmd); /* not htmlout for now */
				format_display_and_save (decomp,
							 format_cmd,
							 disp_cmd,
							 cat_file,
							 formatted_encoding);
			} else 
#endif /* MAN_CATS */
				/* don't save cat */
				format_display (decomp, format_cmd, disp_cmd,
						man_file);

			pipeline_free (disp_cmd);

		} else {
			/* display preformatted cat */
			pipeline *disp_cmd;
			pipeline *decomp_cat;

			if (prompt && do_prompt (title)) {
				pipeline_free (format_cmd);
				pipeline_free (decomp);
				return 0;
			}

			decomp_cat = decompress_open (cat_file);
			if (!decomp_cat) {
				error (0, errno, _("can't open %s"), cat_file);
				pipeline_free (format_cmd);
				pipeline_free (decomp);
				return 0;
			}
			disp_cmd = make_display_command ("UTF-8", title);
			format_display (decomp_cat, NULL, disp_cmd, man_file);
			pipeline_free (disp_cmd);
			pipeline_free (decomp_cat);
		}
	}

	free (formatted_encoding);

	pipeline_free (format_cmd);
	pipeline_free (decomp);

	if (!prompt)
		prompt = found;

	return found;
}

static _Noreturn void gripe_converting_name (const char *name)
{
	error (FATAL, 0, _("Can't convert %s to cat name"), name);
	abort (); /* error should have exited; help compilers prove noreturn */
}

/* Convert the trailing part of 'name' to be a cat page path by altering its
 * extension appropriately. If fsstnd is set, also try converting the
 * containing directory name from "man1" to "cat1" etc., returning NULL if
 * that doesn't work.
 *
 * fsstnd should only be set if name is the original path of a man page
 * found in a man hierarchy, not something like a symlink target or a file
 * named with 'man -l'. Otherwise, a symlink to "/home/manuel/foo.1.gz"
 * would be converted to "/home/catuel/foo.1.gz", which would be bad.
 */
static char *convert_name (const char *name, int fsstnd)
{
	char *to_name, *t1 = NULL;
	char *t2 = NULL;
#ifdef COMP_SRC
	struct compression *comp;
#endif /* COMP_SRC */
	char *namestem;

#ifdef COMP_SRC
	comp = comp_info (name, 1);
	if (comp)
		namestem = comp->stem;
	else
#endif /* COMP_SRC */
		namestem = xstrdup (name);

#ifdef COMP_CAT
	/* TODO: BSD layout requires .0. */
	to_name = xasprintf ("%s.%s", namestem, COMPRESS_EXT);
#else /* !COMP_CAT */
	to_name = xstrdup (namestem);
#endif /* COMP_CAT */
	free (namestem);

	if (fsstnd) {
		t1 = strrchr (to_name, '/');
		if (!t1)
			gripe_converting_name (name);
		*t1 = '\0';

		t2 = strrchr (to_name, '/');
		if (!t2)
			gripe_converting_name (name);
		++t2;
		*t1 = '/';

		if (STRNEQ (t2, "man", 3)) {
			/* If the second-last component starts with "man",
			 * replace "man" with "cat".
			 */
			*t2 = 'c';
			*(t2 + 2) = 't';
		} else {
			free (to_name);
			debug ("couldn't convert %s to FSSTND cat file\n",
			       name);
			return NULL;
		}
	}

	debug ("converted %s to %s\n", name, to_name);

	return to_name;
}

static char *find_cat_file (const char *path, const char *original,
			    const char *man_file)
{
	size_t path_len = strlen (path);
	char *cat_file, *cat_path;
	int status;

	/* Try the FSSTND way first, namely a cat page in the same hierarchy
	 * as the original path to the man page. We don't create these
	 * unless no alternate cat hierarchy is available, but will use them
	 * if they happen to exist already and have the same timestamp as
	 * the corresponding man page. (In practice I'm betting that this
	 * means we'll hardly ever use them at all except for user
	 * hierarchies; but compatibility, eh?)
	 */
	cat_file = convert_name (original, 1);
	if (cat_file) {
		status = is_changed (original, cat_file);
		if (status != -2 && (!(status & 1)) == 1) {
			debug ("found valid FSSTND cat file %s\n", cat_file);
			return cat_file;
		}
		free (cat_file);
	}

	/* Otherwise, find the cat page we actually want to use or create,
	 * taking any alternate cat hierarchy into account. If the original
	 * path and man_file differ (i.e. original was a symlink or .so
	 * link), try the link target and then the source.
	 */
	if (!STREQ (man_file, original)) {
		global_manpath = is_global_mandir (man_file);
		cat_path = get_catpath
			(man_file, global_manpath ? SYSTEM_CAT : USER_CAT);

		if (cat_path) {
			cat_file = convert_name (cat_path, 0);
			free (cat_path);
		} else if (STRNEQ (man_file, path, path_len) &&
			   man_file[path_len] == '/')
			cat_file = convert_name (man_file, 1);
		else
			cat_file = NULL;

		if (cat_file) {
			char *cat_dir = xstrdup (cat_file);
			char *tmp = strrchr (cat_dir, '/');
			if (tmp)
				*tmp = 0;
			if (is_directory (cat_dir)) {
				debug ("will try cat file %s\n", cat_file);
				free (cat_dir);
				return cat_file;
			} else
				debug ("cat dir %s does not exist\n", cat_dir);
			free (cat_dir);
		} else
			debug ("no cat path for %s\n", man_file);
	}

	global_manpath = is_global_mandir (original);
	cat_path = get_catpath
		(original, global_manpath ? SYSTEM_CAT : USER_CAT);

	if (cat_path) {
		cat_file = convert_name (cat_path, 0);
		free (cat_path);
	} else
		cat_file = convert_name (original, 1);

	if (cat_file)
		debug ("will try cat file %s\n", cat_file);
	else
		debug ("no cat path for %s\n", original);

	return cat_file;
}

static int get_ult_flags (char from_db, char id)
{
	if (!from_db)
		return ult_flags;
	else if (id == ULT_MAN)
		/* Checking .so links is expensive, as we have to open the
		 * file. Therefore, if the database lists it as ULT_MAN,
		 * that's good enough for us and we won't recheck that. This
		 * does mean that if a page changes from ULT_MAN to SO_MAN
		 * then you might get duplicates until mandb is next run,
		 * but that isn't a big deal.
		 */
		return ult_flags & ~SO_LINK;
	else
		return ult_flags;
}

/* Is this candidate substantially a duplicate of a previous one?
 * Returns true if so, otherwise false.
 */
static bool duplicate_candidates (struct candidate *left,
				  struct candidate *right)
{
	const char *slash1, *slash2;
	struct locale_bits bits1, bits2;
	bool ret;

	if (left->ult && right->ult && STREQ (left->ult, right->ult))
		return true; /* same ultimate source file */

	if (!STREQ (left->source->name, right->source->name) ||
	    !STREQ (left->source->sec, right->source->sec) ||
	    !STREQ (left->source->ext, right->source->ext))
		return false; /* different name/section/extension */

	if (STREQ (left->path, right->path))
		return true; /* same path */

	/* Figure out if we've had a sufficiently similar candidate for this
	 * language already.
	 */
	slash1 = strrchr (left->path, '/');
	slash2 = strrchr (right->path, '/');
	if (!slash1 || !slash2 ||
	    !STRNEQ (left->path, right->path,
		     MAX (slash1 - left->path, slash2 - right->path)))
		return false; /* different path base */

	unpack_locale_bits (++slash1, &bits1);
	unpack_locale_bits (++slash2, &bits2);

	if (!STREQ (bits1.language, bits2.language) ||
	    !STREQ (bits1.territory, bits2.territory) ||
	    !STREQ (bits1.modifier, bits2.modifier))
		ret = false; /* different language/territory/modifier */
	else
		/* Everything seems to be the same; we can find nothing to
		 * choose between them.
		 */
		ret = true;

	free_locale_bits (&bits1);
	free_locale_bits (&bits2);
	return ret;
}

static int compare_candidates (const struct candidate *left,
			       const struct candidate *right)
{
	const struct mandata *lsource = left->source, *rsource = right->source;
	int cmp;
	const char *slash1, *slash2;

	/* If one candidate matches the requested name exactly, sort it
	 * first. This makes --ignore-case behave more sensibly.
	 */
	/* name is never NULL here, see add_candidate() */
	if (STREQ (lsource->name, left->req_name)) {
		if (!STREQ (rsource->name, right->req_name))
			return -1;
	} else {
		if (STREQ (rsource->name, right->req_name))
			return 1;
	}

	/* Compare pure sections first, then ids, then extensions.
	 * Rationale: whatis refs get the same section and extension as
	 * their source, but may be supplanted by a real page with a
	 * slightly different extension, possibly in another hierarchy (!);
	 * see Debian bug #204249 for the gory details.
	 *
	 * Any extension spelt out in full in section_list effectively
	 * becomes a pure section; this allows extensions to be selectively
	 * moved out of order with respect to their parent sections.
	 */
	if (strcmp (lsource->ext, rsource->ext)) {
		size_t index_left, index_right;

		/* If the user asked for an explicit section, sort exact
		 * matches first.
		 */
		if (section) {
			if (STREQ (lsource->ext, section)) {
				if (!STREQ (rsource->ext, section))
					return -1;
			} else {
				if (STREQ (rsource->ext, section))
					return 1;
			}
		}

		/* Find out whether lsource->ext is ahead of rsource->ext in
		 * section_list.  Sections missing from section_list are
		 * sorted to the end.
		 */
		index_left = gl_list_indexof (section_list, lsource->ext);
		if (index_left == (size_t) -1 && strlen (lsource->ext) > 1) {
			char *sec_left = xstrndup (lsource->ext, 1);
			index_left = gl_list_indexof (section_list, sec_left);
			free (sec_left);
			if (index_left == (size_t) -1)
				index_left = gl_list_size (section_list);
		}
		index_right = gl_list_indexof (section_list, rsource->ext);
		if (index_right == (size_t) -1 && strlen (rsource->ext) > 1) {
			char *sec_right = xstrndup (rsource->ext, 1);
			index_right = gl_list_indexof (section_list,
						       sec_right);
			free (sec_right);
			if (index_right == (size_t) -1)
				index_right = gl_list_size (section_list);
		}
		if (index_left < index_right)
			return -1;
		else if (index_left > index_right)
			return 1;

		cmp = strcmp (lsource->sec, rsource->sec);
		if (cmp)
			return cmp;
	}

	/* ULT_MAN comes first, etc. Consider SO_MAN equivalent to ULT_MAN. */
	cmp = compare_ids (lsource->id, rsource->id, 1);
	if (cmp)
		return cmp;

	/* The order in section_list has already been compared above. For
	 * everything not mentioned explicitly there, we just compare
	 * lexically.
	 */
	cmp = strcmp (lsource->ext, rsource->ext);
	if (cmp)
		return cmp;

	/* Try comparing based on language. We used to prefer to display a
	 * page in the user's preferred language than a page from a better
	 * section, but that attracted objections, so now we prefer to get
	 * the section right. See Debian bug #519547.
	 */
	slash1 = strrchr (left->path, '/');
	slash2 = strrchr (right->path, '/');
	if (slash1 && slash2) {
		char *locale_copy, *p;
		struct locale_bits bits1, bits2, lbits;
		const char *codeset1, *codeset2;

		unpack_locale_bits (++slash1, &bits1);
		unpack_locale_bits (++slash2, &bits2);

		/* We need the current locale as well. */
		locale_copy = xstrdup (internal_locale);
		p = strchr (locale_copy, ':');
		if (p)
			*p = '\0';
		unpack_locale_bits (locale_copy, &lbits);
		free (locale_copy);

#define COMPARE_LOCALE_ELEMENTS(elt) do { \
	/* For different elements, prefer one that matches the locale if
	 * possible.
	 */ \
	if (*lbits.elt) { \
		if (STREQ (lbits.elt, bits1.elt)) { \
			if (!STREQ (lbits.elt, bits2.elt)) { \
				cmp = -1; \
				goto out; \
			} \
		} else { \
			if (STREQ (lbits.elt, bits2.elt)) { \
				cmp = 1; \
				goto out; \
			} \
		} \
	} \
	cmp = strcmp (bits1.territory, bits2.territory); \
	if (cmp) \
		/* No help from locale; might as well sort lexically. */ \
		goto out; \
} while (0)

		COMPARE_LOCALE_ELEMENTS (language);
		COMPARE_LOCALE_ELEMENTS (territory);
		COMPARE_LOCALE_ELEMENTS (modifier);

#undef COMPARE_LOCALE_ELEMENTS

		/* Prefer UTF-8 if available. Otherwise, consider them
		 * equal.
		 */
		codeset1 = get_canonical_charset_name (bits1.codeset);
		codeset2 = get_canonical_charset_name (bits2.codeset);
		if (STREQ (codeset1, "UTF-8")) {
			if (!STREQ (codeset2, "UTF-8")) {
				cmp = -1;
				goto out;
			}
		} else {
			if (STREQ (codeset2, "UTF-8")) {
				cmp = 1;
				goto out;
			}
		}

out:
		free_locale_bits (&lbits);
		free_locale_bits (&bits1);
		free_locale_bits (&bits2);
		if (cmp)
			return cmp;
	}

	/* Explicitly stabilise the sort as a last resort, so that manpath
	 * ordering (e.g. language-specific hierarchies) works.
	 */
	if (left->add_index < right->add_index)
		return -1;
	else if (left->add_index > right->add_index)
		return 1;
	else
		return 0;

	return 0;
}

static int compare_candidates_qsort (const void *l, const void *r)
{
	const struct candidate *left = *(const struct candidate **)l;
	const struct candidate *right = *(const struct candidate **)r;

	return compare_candidates (left, right);
}

static void free_candidate (struct candidate *candidate)
{
	if (candidate)
		free (candidate->ult);
	free (candidate);
}

/* Add an entry to the list of candidates. */
static int add_candidate (struct candidate **head, char from_db, char cat,
			  const char *req_name, const char *path,
			  const char *ult, struct mandata *source)
{
	struct candidate *search, *prev, *insert, *candp;
	static int add_index = 0;

	if (!ult) {
		const char *name;
		char *filename;

		if (*source->pointer != '-')
			name = source->pointer;
		else if (source->name)
			name = source->name;
		else
			name = req_name;

		filename = make_filename (path, name, source, cat ? "cat" : "man");
		if (!filename)
			return 0;
		ult = ult_src (filename, path, NULL,
			       get_ult_flags (from_db, source->id), NULL);
		free (filename);
	}

	debug ("candidate: %d %d %s %s %s %c %s %s %s\n",
	       from_db, cat, req_name, path, ult,
	       source->id, source->name ? source->name : "-",
	       source->sec, source->ext);

	if (!source->name)
		source->name = xstrdup (req_name);

	candp = XMALLOC (struct candidate);
	candp->req_name = req_name;
	candp->from_db = from_db;
	candp->cat = cat;
	candp->path = path;
	candp->ult = ult ? xstrdup (ult) : NULL;
	candp->source = source;
	candp->add_index = add_index++;
	candp->next = NULL;

	/* insert will be NULL (insert at start) or a pointer to the element
	 * after which this element should be inserted.
	 */
	insert = NULL;
	search = *head;
	prev = NULL;
	/* This search produces quadratic-time behaviour, although in
	 * practice it doesn't seem to be too bad at the moment since the
	 * run-time is dominated by calls to ult_src. In future it might be
	 * worth optimising this; the reason I haven't done this yet is that
	 * it involves quite a bit of tedious bookkeeping. A practical
	 * approach would be to keep two hashes, one that's just a set to
	 * keep track of whether candp->ult has been seen already, and one
	 * that keeps a list of candidates for each candp->name that could
	 * then be quickly checked by brute force.
	 */
	while (search) {
		int dupcand = duplicate_candidates (candp, search);

		debug ("search: %d %d %s %s %s %c %s %s %s "
		       "(dup: %d)\n",
		       search->from_db, search->cat, search->req_name,
		       search->path, search->ult, search->source->id,
		       search->source->name ? search->source->name : "-",
		       search->source->sec, search->source->ext, dupcand);

		/* Check for duplicates. */
		if (dupcand) {
			int cmp = compare_candidates (candp, search);

			if (cmp >= 0) {
				debug ("other duplicate is at least as "
				       "good\n");
				free_candidate (candp);
				return 0;
			} else {
				debug ("this duplicate is better; removing "
				       "old one\n");
				if (prev) {
					prev->next = search->next;
					free_candidate (search);
					search = prev->next;
				} else {
					*head = search->next;
					free_candidate (search);
					search = *head;
				}
				continue;
			}
		}

		prev = search;
		if (search->next)
			search = search->next;
		else
			break;
	}
	/* Insert the new candidate at the end of the list (having had to go
	 * through them all looking for duplicates anyway); we'll sort it
	 * into place later.
	 */
	insert = prev;

	candp->next = insert ? insert->next : *head;
	if (insert)
		insert->next = candp;
	else
		*head = candp;

	return 1;
}

/* Sort the entire list of candidates. */
static void sort_candidates (struct candidate **candidates)
{
	struct candidate *cand, **allcands;
	size_t count = 0, i;

	for (cand = *candidates; cand; cand = cand->next)
		++count;

	if (count == 0)
		return;

	allcands = XNMALLOC (count, struct candidate *);
	i = 0;
	for (cand = *candidates; cand; cand = cand->next) {
		assert (i < count);
		allcands[i++] = cand;
	}
	assert (i == count);

	qsort (allcands, count, sizeof *allcands, compare_candidates_qsort);

	*candidates = cand = allcands[0];
	for (i = 1; i < count; ++i) {
		cand->next = allcands[i];
		cand = cand->next;
	}
	cand->next = NULL;

	free (allcands);
}

/*
 * See if the preformatted man page or the source exists in the given
 * section.
 */
static int try_section (const char *path, const char *sec, const char *name,
			struct candidate **cand_head)
{
	int found = 0;
	gl_list_t names = NULL;
	const char *found_name;
	char cat = 0;
	int lff_opts = (match_case ? LFF_MATCHCASE : 0) |
		       (regex_opt ? LFF_REGEX : 0) |
		       (wildcard ? LFF_WILDCARD : 0);

	debug ("trying section %s with globbing\n", sec);

#ifndef NROFF_MISSING /* #ifdef NROFF */
	/*
  	 * Look for man page source files.
  	 */

	names = look_for_file (path, sec, name, 0, lff_opts);
	if (!gl_list_size (names))
		/*
    		 * No files match.  
    		 * See if there's a preformatted page around that
    		 * we can display.
    		 */
#endif /* NROFF_MISSING */
	{
		if (catman)
			return 1;

		if (!troff && !want_encoding && !recode) {
			gl_list_free (names);
			names = look_for_file (path, sec, name, 1, lff_opts);
			cat = 1;
		}
	}

	order_files (path, &names);

	GL_LIST_FOREACH_START (names, found_name) {
		struct mandata *info = infoalloc ();
		char *info_buffer = filename_info (found_name, info, name);
		const char *ult;
		int f;

		if (!info_buffer) {
			free_mandata_struct (info);
			continue;
		}
		info->addr = info_buffer;

		/* What kind of page is this? Since it's a real file, it
		 * must be either ULT_MAN or SO_MAN. ult_src() can tell us
		 * which.
		 */
		ult = ult_src (found_name, path, NULL, ult_flags, NULL);
		if (!ult) {
			/* already warned */
			debug ("try_section(): bad link %s\n", found_name);
			free (info_buffer);
			info->addr = NULL;
			free_mandata_struct (info);
			continue;
		}
		if (STREQ (ult, found_name))
			info->id = ULT_MAN;
		else
			info->id = SO_MAN;

		f = add_candidate (cand_head, CANDIDATE_FILESYSTEM,
				   cat, name, path, ult, info);
		found += f;
		/* Free info and info_buffer if they weren't added to the
		 * candidates.
		 */
		if (f == 0) {
			free (info_buffer);
			info->addr = NULL;
			free_mandata_struct (info);
		}
		/* Don't free info and info_buffer here. */
	} GL_LIST_FOREACH_END (names);

	gl_list_free (names);
	return found;
}

static int display_filesystem (struct candidate *candp)
{
	char *filename = make_filename (candp->path, NULL, candp->source,
					candp->cat ? "cat" : "man");
	char *title;
	int found = 0;

	if (!filename)
		return 0;
	/* source->name is never NULL thanks to add_candidate() */
	title = xasprintf ("%s(%s)", candp->source->name, candp->source->ext);

	if (candp->cat) {
		if (troff || want_encoding || recode)
			goto out;
		found = display (candp->path, NULL, filename, title, NULL);
	} else {
		const char *man_file;
		char *cat_file;

		man_file = ult_src (filename, candp->path, NULL, ult_flags,
				    NULL);
		if (man_file == NULL)
			goto out;

		debug ("found ultimate source file %s\n", man_file);
		lang = lang_dir (man_file);

		cat_file = find_cat_file (candp->path, filename, man_file);
		found = display (candp->path, man_file, cat_file, title, NULL);
		free (cat_file);
		free (lang);
		lang = NULL;
	}

out:
	free (title);
	free (filename);
	return found;
}

#ifdef MAN_DB_UPDATES
/* wrapper to dbdelete which deals with opening/closing the db */
static void dbdelete_wrapper (const char *page, struct mandata *info)
{
	if (!catman) {
		MYDBM_FILE dbf;

		dbf = MYDBM_RWOPEN (database);
		if (dbf) {
			if (dbdelete (dbf, page, info) == 1)
				debug ("%s(%s) not in db!\n", page, info->ext);
			MYDBM_CLOSE (dbf);
		}
	}
}
#endif /* MAN_DB_UPDATES */

/* This started out life as try_section, but a lot of that routine is 
   redundant wrt the db cache. */
static int display_database (struct candidate *candp)
{
	int found = 0;
	char *file;
	const char *name;
	char *title;
	struct mandata *in = candp->source;

	debug ("trying a db located file.\n");
	dbprintf (in);

	/* if the pointer holds some data, this is a reference to the 
	   real page, use that instead. */
	if (*in->pointer != '-')
		name = in->pointer;
	else if (in->name)
		name = in->name;
	else
		name = candp->req_name;

	if (in->id == WHATIS_MAN || in->id == WHATIS_CAT)
		debug (_("%s: relying on whatis refs is deprecated\n"), name);

	title = xasprintf ("%s(%s)",
			   in->name ? in->name : candp->req_name, in->ext);

#ifndef NROFF_MISSING /* #ifdef NROFF */
	/*
  	 * Look for man page source files.
  	 */

	if (in->id < STRAY_CAT) {	/* There should be a src page */
		file = make_filename (candp->path, name, in, "man");
		if (file) {
			const char *man_file;
			char *cat_file;

			man_file = ult_src (file, candp->path, NULL,
					    get_ult_flags (1, in->id), NULL);
			if (man_file == NULL) {
				free (title);
				return found; /* zero */
			}

			debug ("found ultimate source file %s\n", man_file);
			lang = lang_dir (man_file);

			cat_file = find_cat_file (candp->path, file, man_file);
			found += display (candp->path, man_file, cat_file,
					  title, in->filter);
			free (cat_file);
			free (lang);
			lang = NULL;
			free (file);
		} /* else {drop through to the bottom and return 0 anyway} */
	} else

#endif /* NROFF_MISSING */

	if (in->id <= WHATIS_CAT) {
		/* The db says we have a stray cat or whatis ref */

		if (catman) {
			free (title);
			return ++found;
		}

		/* show this page but force an update later to make sure
		   we haven't just added the new page */
		found_a_stray = 1;

		/* If explicitly asked for troff or a different encoding,
		 * don't show a stray cat.
		 */
		if (troff || want_encoding || recode) {
			free (title);
			return found;
		}

		file = make_filename (candp->path, name, in, "cat");
		if (!file) {
			char *catpath;
			catpath = get_catpath (candp->path,
					       global_manpath ? SYSTEM_CAT
							      : USER_CAT);

			if (catpath && strcmp (catpath, candp->path) != 0) {
				file = make_filename (catpath, name,
						      in, "cat");
				free (catpath);
				if (!file) {
					/* don't delete here, 
					   return==0 will do that */
					free (title);
					return found; /* zero */
				}
			} else {
				free (catpath);
				free (title);
				return found; /* zero */
			}
		}

		found += display (candp->path, NULL, file, title, in->filter);
		free (file);
	}
	free (title);
	return found;
}

/* test for existence, if fail: call dbdelete_wrapper, else return amount */
static int display_database_check (struct candidate *candp)
{
	int exists = display_database (candp);

#ifdef MAN_DB_UPDATES
	if (!exists && !skip) {
		debug ("dbdelete_wrapper (%s, %p)\n",
		       candp->req_name, candp->source);
		dbdelete_wrapper (candp->req_name, candp->source);
	}
#endif /* MAN_DB_UPDATES */

	return exists;
}

#ifdef MAN_DB_UPDATES
static int maybe_update_file (const char *manpath, const char *name,
			      struct mandata *info)
{
	const char *real_name;
	char *file;
	struct stat buf;
	struct timespec file_mtime;
	int status;

	if (!update)
		return 0;

	/* If the pointer holds some data, then we need to look at that
	 * name in the filesystem instead.
	 */
	if (!STRNEQ (info->pointer, "-", 1))
		real_name = info->pointer;
	else if (info->name)
		real_name = info->name;
	else
		real_name = name;

	file = make_filename (manpath, real_name, info, "man");
	if (!file)
		return 0;
	if (lstat (file, &buf) != 0)
		return 0;
	file_mtime = get_stat_mtime (&buf);
	if (timespec_cmp (file_mtime, info->mtime) == 0)
		return 0;

	debug ("%s needs to be recached: %ld.%09ld %ld.%09ld\n",
	       file,
	       (long) info->mtime.tv_sec, (long) info->mtime.tv_nsec,
	       (long) file_mtime.tv_sec, (long) file_mtime.tv_nsec);
	status = run_mandb (0, manpath, file);
	if (status)
		error (0, 0, _("mandb command failed with exit status %d"),
		       status);
	free (file);

	return 1;
}
#endif /* MAN_DB_UPDATES */

/* Special return values from try_db(). */

#define TRY_DATABASE_OPEN_FAILED  -1

#ifdef MAN_DB_CREATES
#define TRY_DATABASE_CREATED      -2
#endif /* MAN_DB_CREATES */

#ifdef MAN_DB_UPDATES
#define TRY_DATABASE_UPDATED      -3
#endif /* MAN_DB_UPDATES */

static void db_map_value_free (const void *value)
{
	/* The value may be NULL to indicate that opening the database at
	 * this location already failed.
	 */
	if (value)
		gl_list_free ((gl_list_t) value);
}

/* Look for a page in the database. If db not accessible, return -1,
   otherwise return number of pages found. */
static int try_db (const char *manpath, const char *sec, const char *name,
		   struct candidate **cand_head)
{
	gl_list_t matches;
	struct mandata *loc;
	char *catpath;
	int found = 0;
#ifdef MAN_DB_UPDATES
	bool found_stale = false;
#endif /* MAN_DB_UPDATES */

	/* find out where our db for this manpath should be */

	catpath = get_catpath (manpath, global_manpath ? SYSTEM_CAT : USER_CAT);
	free (database);
	if (catpath) {
		database = mkdbname (catpath);
		free (catpath);
	} else
		database = mkdbname (manpath);

	if (!db_map)
		db_map = new_string_map (GL_HASH_MAP, db_map_value_free);

	/* If we haven't looked here already, do so now. */
	if (!gl_map_search (db_map, manpath, (const void **) &matches)) {
		MYDBM_FILE dbf;

		dbf = MYDBM_RDOPEN (database);
		if (dbf && dbver_rd (dbf)) {
			MYDBM_CLOSE (dbf);
			dbf = NULL;
		}
		if (dbf) {
			debug ("Succeeded in opening %s O_RDONLY\n", database);

			/* if section is set, only return those that match,
			   otherwise NULL retrieves all available */
			if (regex_opt || wildcard)
				matches = dblookup_pattern
					(dbf, name, section, match_case,
					 regex_opt, !names_only);
			else
				matches = dblookup_all (dbf, name, section,
							match_case);
			gl_map_put (db_map, xstrdup (manpath), matches);
			MYDBM_CLOSE (dbf);
			dbf = NULL;
#ifdef MAN_DB_CREATES
		} else if (!global_manpath) {
			/* create one */
			debug ("Failed to open %s O_RDONLY\n", database);
			if (run_mandb (1, manpath, NULL)) {
				gl_map_put (db_map, xstrdup (manpath), NULL);
				return TRY_DATABASE_OPEN_FAILED;
			}
			return TRY_DATABASE_CREATED;
#endif /* MAN_DB_CREATES */
		} else {
			debug ("Failed to open %s O_RDONLY\n", database);
			gl_map_put (db_map, xstrdup (manpath), NULL);
			return TRY_DATABASE_OPEN_FAILED;
		}
		assert (matches != NULL);
	}

	/* We already tried (and failed) to open this db before. */
	if (!matches)
		return TRY_DATABASE_OPEN_FAILED;

#ifdef MAN_DB_UPDATES
	/* Check that all the entries found are up to date. If not, the
	 * caller should try again.
	 */
	GL_LIST_FOREACH_START (matches, loc)
		if (STREQ (sec, loc->sec) &&
		    (!extension || STREQ (extension, loc->ext)
				|| STREQ (extension, loc->ext + strlen (sec))))
			if (maybe_update_file (manpath, name, loc))
				found_stale = true;
	GL_LIST_FOREACH_END (matches);

	if (found_stale) {
		gl_map_remove (db_map, manpath);
		return TRY_DATABASE_UPDATED;
	}
#endif /* MAN_DB_UPDATES */

	/* cycle through the mandata structures (there's usually only 
	   1 or 2) and see what we have w.r.t. the current section */
	GL_LIST_FOREACH_START (matches, loc)
		if (STREQ (sec, loc->sec) &&
		    (!extension || STREQ (extension, loc->ext)
				|| STREQ (extension, loc->ext + strlen (sec))))
			found += add_candidate (cand_head, CANDIDATE_DATABASE,
						0, name, manpath, NULL, loc);
	GL_LIST_FOREACH_END (matches);

	return found;
}

/* Try to locate the page under the specified manpath, in the desired section,
 * with the supplied name. Glob if necessary. Initially search the filesystem;
 * if that fails, try finding it via a db cache access. */
static int locate_page (const char *manpath, const char *sec, const char *name,
			struct candidate **candidates)
{
	int found, db_ok;

	/* sort out whether we want to treat this hierarchy as 
	   global or user. Differences:

	   global: if setuid, use privs; don't create db.
	   user  : if setuid, drop privs; allow db creation. */

	global_manpath = is_global_mandir (manpath);
	if (!global_manpath)
		drop_effective_privs ();

	debug ("searching in %s, section %s\n", manpath, sec);

	found = try_section (manpath, sec, name, candidates);

	if ((!found || findall) && !global_apropos) {
		db_ok = try_db (manpath, sec, name, candidates);

#ifdef MAN_DB_CREATES
		if (db_ok == TRY_DATABASE_CREATED)
			/* we created a db in the last call */
			db_ok = try_db (manpath, sec, name, candidates);
#endif /* MAN_DB_CREATES */

#ifdef MAN_DB_UPDATES
		if (db_ok == TRY_DATABASE_UPDATED)
			/* We found some outdated entries and rebuilt the
			 * database in the last call. If this keeps
			 * happening, though, give up and punt to the
			 * filesystem.
			 */
			db_ok = try_db (manpath, sec, name, candidates);
#endif /* MAN_DB_UPDATES */

		if (db_ok > 0)  /* we found/opened a db and found something */
			found += db_ok;
	}

	if (!global_manpath)
		regain_effective_privs ();

	return found;
}

static int display_pages (struct candidate *candidates)
{
	struct candidate *candp;
	int found = 0;

	for (candp = candidates; candp; candp = candp->next) {
		global_manpath = is_global_mandir (candp->path);
		if (!global_manpath)
			drop_effective_privs ();

		switch (candp->from_db) {
			case CANDIDATE_FILESYSTEM:
				found += display_filesystem (candp);
				break;
			case CANDIDATE_DATABASE:
				found += display_database_check (candp);
				break;
			default:
				error (0, 0,
				       _("internal error: candidate type %d "
					 "out of range"), candp->from_db);
		}

		if (!global_manpath)
			regain_effective_privs ();

		if (found && !findall)
			return found;
	}

	return found;
}

/*
 * Search for text in all manual pages.
 *
 * This is not a real full-text search, but a brute-force on-demand search.
 * The idea, name, and approach originate in the 'man' package, added (I
 * believe) by Andries Brouwer, although the implementation is new for
 * man-db and much faster due to running in-process.
 *
 * Conceptually, this really belongs in whatis.c, as part of apropos.
 * However, the implementation in 'man' offers pages for immediate display
 * on request rather than simply listing them, which is currently awkward to
 * do in apropos. If we ever add support to apropos/whatis for either
 * calling back to man or displaying pages directly, we should revisit this.
 */
static int grep (const char *file, const char *string, const regex_t *search)
{
	struct stat st;
	pipeline *decomp;
	const char *line;
	int ret = 0;

	/* pipeline_start makes file open failures unconditionally fatal.
	 * Here, we'd rather just ignore any such files.
	 */
	if (stat (file, &st) < 0)
		return 0;

	decomp = decompress_open (file);
	if (!decomp)
		return 0;
	pipeline_start (decomp);
	while ((line = pipeline_readline (decomp)) != NULL) {
		if (regex_opt) {
			if (regexec (search, line,
				     0, (regmatch_t *) 0, 0) == 0) {
				ret = 1;
				break;
			}
		} else {
			if (match_case ?
			    strstr (line, string) :
			    strcasestr (line, string)) {
				ret = 1;
				break;
			}
		}
	}

	pipeline_free (decomp);
	return ret;
}

static int do_global_apropos_section (const char *path, const char *sec,
				      const char *name)
{
	int found = 0;
	gl_list_t names;
	const char *found_name;
	regex_t search;

	global_manpath = is_global_mandir (path);
	if (!global_manpath)
		drop_effective_privs ();

	debug ("searching in %s, section %s\n", path, sec);

	names = look_for_file (path, sec, "*", 0, LFF_WILDCARD);

	if (regex_opt)
		xregcomp (&search, name,
			  REG_EXTENDED | REG_NOSUB |
			  (match_case ? 0 : REG_ICASE));
	else
		memset (&search, 0, sizeof search);

	order_files (path, &names);

	GL_LIST_FOREACH_START (names, found_name) {
		struct mandata *info;
		char *info_buffer;
		char *title = NULL;
		const char *man_file;
		char *cat_file = NULL;

		if (!grep (found_name, name, &search))
			continue;

		info = infoalloc ();
		info_buffer = filename_info (found_name, info, NULL);
		if (!info_buffer)
			goto next;
		info->addr = info_buffer;

		title = xasprintf ("%s(%s)", strchr (info_buffer, '\0') + 1,
				   info->ext);
		man_file = ult_src (found_name, path, NULL, ult_flags, NULL);
		if (!man_file)
			goto next;
		lang = lang_dir (man_file);
		cat_file = find_cat_file (path, found_name, man_file);
		if (display (path, man_file, cat_file, title, NULL))
			found = 1;
		free (lang);
		lang = NULL;

next:
		free (cat_file);
		free (title);
		free_mandata_struct (info);
	} GL_LIST_FOREACH_END (names);

	gl_list_free (names);

	if (regex_opt)
		regfree (&search);

	if (!global_manpath)
		regain_effective_privs ();

	return found;
}

static int do_global_apropos (const char *name, int *found)
{
	gl_list_t my_section_list;
	const char *sec;

	if (section) {
		my_section_list = gl_list_create_empty (GL_ARRAY_LIST, NULL,
							NULL, NULL, false);
		gl_list_add_last (my_section_list, section);
	} else
		my_section_list = section_list;

	GL_LIST_FOREACH_START (my_section_list, sec) {
		char *mp;

		GL_LIST_FOREACH_START (manpathlist, mp)
			*found += do_global_apropos_section (mp, sec, name);
		GL_LIST_FOREACH_END (manpathlist);
	} GL_LIST_FOREACH_END (my_section_list);

	if (section)
		gl_list_free (my_section_list);

	return *found ? OK : NOT_FOUND;
}

/* Each of local_man_loop and man sometimes calls the other. */
static int man (const char *name, int *found);

/* man issued with `-l' option */
static int local_man_loop (const char *argv)
{
	int exit_status = OK;
	bool local_mf = local_man_file;

	drop_effective_privs ();
	local_man_file = true;
	if (strcmp (argv, "-") == 0)
		display (NULL, "", NULL, "(stdin)", NULL);
	else {
		struct stat st;

		/* Check that the file exists and isn't e.g. a directory */
		if (stat (argv, &st)) {
			error (0, errno, "%s", argv);
			return NOT_FOUND;
		}

		if (S_ISDIR (st.st_mode)) {
			error (0, EISDIR, "%s", argv);
			return NOT_FOUND;
		}

		if (S_ISCHR (st.st_mode) || S_ISBLK (st.st_mode)) {
			/* EINVAL is about the best I can do. */
			error (0, EINVAL, "%s", argv);
			return NOT_FOUND;
		}

		if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
			/* Perhaps an executable. If its directory is on
			 * $PATH, then we want to look up the corresponding
			 * manual page in the appropriate hierarchy rather
			 * than displaying the executable.
			 */
			char *argv_dir = dir_name (argv);
			int found = 0;

			if (directory_on_path (argv_dir)) {
				char *argv_base = base_name (argv);
				char *new_manp, *nm;
				gl_list_t old_manpathlist;

				debug ("recalculating manpath for executable "
				       "in %s\n", argv_dir);

				new_manp = get_manpath_from_path (argv_dir, 0);
				if (!new_manp || !*new_manp) {
					debug ("no useful manpath for "
					       "executable\n");
					goto executable_out;
				}
				nm = locale_manpath (new_manp);
				free (new_manp);
				new_manp = nm;

				old_manpathlist = manpathlist;
				manpathlist = create_pathlist (new_manp);

				man (argv_base, &found);

				free_pathlist (manpathlist);
				manpathlist = old_manpathlist;
executable_out:
				free (new_manp);
				free (argv_base);
			}
			free (argv_dir);

			if (found)
				return OK;
		}

		if (exit_status == OK) {
			char *argv_base = base_name (argv);
			char *argv_abs;
			if (argv[0] == '/')
				argv_abs = xstrdup (argv);
			else {
				argv_abs = xgetcwd ();
				if (argv_abs)
					argv_abs = appendstr (argv_abs, "/",
							      argv,
							      (void *) 0);
				else
					argv_abs = xstrdup (argv);
			}
			lang = lang_dir (argv_abs);
			free (argv_abs);
			if (!display (NULL, argv, NULL, argv_base, NULL)) {
				if (local_mf)
					error (0, errno, "%s", argv);
				exit_status = NOT_FOUND;
			}
			free (lang);
			lang = NULL;
			free (argv_base);
		}
	}
	local_man_file = local_mf;
	regain_effective_privs ();
	return exit_status;
}

/*
 * Splits a "name[.section]" into { "name", "section" }.
 * Section would be NULL if not present.
 * The caller is responsible for freeing *ret_name and *ret_section.
 * */
static void split_page_name (const char *page_name,
			     char **ret_name,
			     char **ret_section)
{
	char *dot;

	dot = strrchr (page_name, '.');

	if (dot && is_section (dot + 1)) {
		*ret_name = xstrndup (page_name, dot - page_name);
		*ret_section = xstrdup (dot + 1);
	} else {
		*ret_name = xstrdup (page_name);
		*ret_section = NULL;
	}
}

static void locate_page_in_manpath (const char *page_section,
				    const char *page_name,
				    struct candidate **candidates,
				    int *found)
{
	char *mp;

	GL_LIST_FOREACH_START (manpathlist, mp)
		*found += locate_page (mp, page_section, page_name,
				       candidates);
	GL_LIST_FOREACH_END (manpathlist);
}

/*
 * Search for manual pages.
 *
 * If preformatted manual pages are supported, look for the formatted
 * file first, then the man page source file.  If they both exist and
 * the man page source file is newer, or only the source file exists,
 * try to reformat it and write the results in the cat directory.  If
 * it is not possible to write the cat file, simply format and display
 * the man file.
 *
 * If preformatted pages are not supported, or the troff option is
 * being used, only look for the man page source file.
 *
 */
static int man (const char *name, int *found)
{
	char *page_name, *page_section;
	struct candidate *candidates = NULL, *cand, *candnext;

	*found = 0;
	fflush (stdout);

	if (strchr (name, '/')) {
		int status = local_man_loop (name);
		if (status == OK)
			*found = 1;
		return status;
	}

	if (section)
		locate_page_in_manpath (section, name, &candidates, found);
	else {
		const char *sec;

		GL_LIST_FOREACH_START (section_list, sec)
			locate_page_in_manpath (sec, name, &candidates, found);
		GL_LIST_FOREACH_END (section_list);
	}

	split_page_name (name, &page_name, &page_section);

	if (!*found && page_section)
		locate_page_in_manpath (page_section, page_name, &candidates,
					found);

	free (page_name);
	free (page_section);

	sort_candidates (&candidates);

	if (*found)
		*found = display_pages (candidates);

	for (cand = candidates; cand; cand = candnext) {
		candnext = cand->next;
		free_candidate (cand);
	}

	return *found ? OK : NOT_FOUND;
}


static gl_list_t get_section_list (void)
{
	gl_list_t config_sections, sections;
	const char *sec;

	/* Section list from configuration file, or STD_SECTIONS if it's
	 * empty.
	 */
	config_sections = get_sections ();
	if (!gl_list_size (config_sections)) {
		int i;
		for (i = 0; std_sections[i]; ++i)
			gl_list_add_last (config_sections,
					  xstrdup (std_sections[i]));
	}

	if (colon_sep_section_list == NULL)
		colon_sep_section_list = getenv ("MANSECT");
	if (colon_sep_section_list == NULL || *colon_sep_section_list == '\0')
		return config_sections;

	/* Although this is documented as colon-separated, at least Solaris
	 * man's -s option takes a comma-separated list, so we accept that
	 * too for compatibility.
	 */
	sections = new_string_list (GL_ARRAY_LIST, true);
	for (sec = strtok (colon_sep_section_list, ":,"); sec; 
	     sec = strtok (NULL, ":,"))
		gl_list_add_last (sections, xstrdup (sec));

	if (gl_list_size (sections)) {
		gl_list_free (config_sections);
		return sections;
	} else {
		gl_list_free (sections);
		return config_sections;
	}
}

/*
 * Returns the first token of a libpipeline/sh-style command. See SUSv4TC2:
 * 2.2 Shell Command Language: Quoting.
 *
 * Free the returned value.
 *
 * Examples:
 * sh_lang_first_word ("echo 3") returns "echo"
 * sh_lang_first_word ("'e ho' 3") returns "e ho"
 * sh_lang_first_word ("e\\cho 3") returns "echo"
 * sh_lang_first_word ("e\\\ncho 3") returns "echo"
 * sh_lang_first_word ("\"echo t\" 3") returns "echo t"
 * sh_lang_first_word ("\"ech\\o t\" 3") returns "ech\\o t"
 * sh_lang_first_word ("\"ech\\\\o t\" 3") returns "ech\\o t"
 * sh_lang_first_word ("\"ech\\\no t\" 3") returns "echo t"
 * sh_lang_first_word ("\"ech\\$ t\" 3") returns "ech$ t"
 * sh_lang_first_word ("\"ech\\` t\" 3") returns "ech` t"
 * sh_lang_first_word ("e\"ch\"o 3") returns "echo"
 * sh_lang_first_word ("e'ch'o 3") returns "echo"
 */
static char *sh_lang_first_word (const char *cmd)
{
	int i, o = 0;
	char *ret = xmalloc (strlen (cmd) + 1);

	for (i = 0; cmd[i] != '\0'; i++) {
		if (cmd[i] == '\\') {
			/* Escape Character (Backslash) */
			i++;
			if (cmd[i] == '\0')
				break;
			if (cmd[i] != '\n')
				ret[o++] = cmd[i];
		} else if (cmd[i] == '\'') {
			/* Single-Quotes */
			i++;
			while (cmd[i] != '\0' && cmd[i] != '\'')
				ret[o++] = cmd[i++];
		} else if (cmd[i] == '"') {
			/* Double-Quotes */
			i++;
			while (cmd[i] != '\0' && cmd[i] != '"') {
				if (cmd[i] == '\\') {
					if (cmd[i + 1] == '$' ||
					    cmd[i + 1] == '`' ||
					    cmd[i + 1] == '"' ||
					    cmd[i + 1] == '\\')
						ret[o++] = cmd[++i];
					else if (cmd[i + 1] == '\n')
						i++;
					else
						ret[o++] = cmd[i];
				} else
					ret[o++] = cmd[i];

				i++;
			}
		} else if (cmd[i] == '\t' || cmd[i] == ' ' || cmd[i] == '\n' ||
			   cmd[i] == '#')
			break;
		else
			ret[o++] = cmd[i];
	}

	ret[o] = '\0';

	return ret;
}

int main (int argc, char *argv[])
{
	int argc_env, exit_status = OK;
	char **argv_env;
	const char *tmp;

	set_program_name (argv[0]);

	init_debug ();
	pipeline_install_post_fork (pop_all_cleanups);
	sandbox = sandbox_init ();

	umask (022);
	init_locale ();

	internal_locale = setlocale (LC_MESSAGES, NULL);
	/* Use LANGUAGE only when LC_MESSAGES locale category is
	 * neither "C" nor "POSIX". */
	if (internal_locale && strcmp (internal_locale, "C") &&
	    strcmp (internal_locale, "POSIX"))
		multiple_locale = getenv ("LANGUAGE");
	internal_locale = xstrdup (internal_locale ? internal_locale : "C");

	xstdopen ();

/* export argv, it might be needed when invoking the vendor supplied browser */
#if defined _AIX || defined __sgi
	global_argv = argv;
#endif

#ifdef TROFF_IS_GROFF
	/* used in --help, so initialise early */
	if (!html_pager)
		init_html_pager ();
#endif /* TROFF_IS_GROFF */

#ifdef NROFF_WARNINGS
	roff_warnings = new_string_list (GL_ARRAY_LIST, true);
#endif /* NROFF_WARNINGS */

	/* First of all, find out if $MANOPT is set. If so, put it in 
	   *argv[] format for argp to play with. */
	argv_env = manopt_to_env (&argc_env);
	if (argv_env)
		if (argp_parse (&argp, argc_env, argv_env, ARGP_NO_ARGS, 0, 0))
			exit (FAIL);

	/* parse the actual program args */
	if (argp_parse (&argp, argc, argv, ARGP_NO_ARGS, &first_arg, 0))
		exit (FAIL);

	/* record who we are and drop effective privs for later use */
	init_security ();

	read_config_file (local_man_file || user_config_file);

	/* if the user wants whatis or apropos, give it to them... */
	if (external)
		do_extern (argc, argv);

	get_term (); /* stores terminal settings */
#ifdef MAN_OWNER
	debug ("real user = %lu; effective user = %lu\n",
	       (unsigned long) ruid, (unsigned long) euid);
#endif /* MAN_OWNER */

	/* close this locale and reinitialise if a new locale was 
	   issued as an argument or in $MANOPT */
	if (locale) {
		free (internal_locale);
		internal_locale = setlocale (LC_ALL, locale);
		if (internal_locale)
			internal_locale = xstrdup (internal_locale);
		else
			internal_locale = xstrdup (locale);

		debug ("main(): locale = %s, internal_locale = %s\n",
		       locale, internal_locale);
		if (internal_locale) {
			setenv ("LANGUAGE", internal_locale, 1);
			locale_changed ();
			multiple_locale = NULL;
		}
	}

#ifdef TROFF_IS_GROFF
	if (htmlout)
		pager = html_pager;
#endif /* TROFF_IS_GROFF */

	if (pager == NULL)
		pager = getenv ("MANPAGER");
	if (pager == NULL)
		pager = getenv ("PAGER");
	if (pager == NULL)
		pager = get_def_user ("pager", NULL);
	if (pager == NULL) {
		char *pager_program = sh_lang_first_word (PAGER);
		if (pathsearch_executable (pager_program))
			pager = PAGER;
		else
			pager = "";
		free (pager_program);
	}
	if (*pager == '\0')
		pager = get_def_user ("cat", CAT);

	if (prompt_string == NULL)
		prompt_string = getenv ("MANLESS");

	if (prompt_string == NULL)
#ifdef LESS_PROMPT
		prompt_string = LESS_PROMPT;
#else
		prompt_string = _(
				" Manual page " MAN_PN
				" ?ltline %lt?L/%L.:byte %bB?s/%s..?e (END):"
				"?pB %pB\\%.. "
				"(press h for help or q to quit)");
#endif

	/* Restore and save $LESS in $MAN_ORIG_LESS so that recursive uses
	 * of man work as expected.
	 */
	less = getenv ("MAN_ORIG_LESS");
	if (less == NULL)
		less = getenv ("LESS");
	setenv ("MAN_ORIG_LESS", less ? less : "", 1);

	debug ("\nusing %s as pager\n", pager);

	if (first_arg == argc) {
		if (print_where) {
			manp = get_manpath ("");
			printf ("%s\n", manp);
			exit (OK);
		} else {
			free (internal_locale);
			gripe_no_name (NULL);
		}
	}

	section_list = get_section_list ();

	if (manp == NULL) {
		char *mp = get_manpath (alt_system_name);
		manp = locale_manpath (mp);
		free (mp);
	} else
		free (get_manpath (NULL));

	debug ("manpath search path (with duplicates) = %s\n", manp);

	manpathlist = create_pathlist (manp);

	/* man issued with `-l' option */
	if (local_man_file) {
		while (first_arg < argc) {
			exit_status = local_man_loop (argv[first_arg]);
			++first_arg;
		}
		free (internal_locale);
		exit (exit_status);
	}

	/* finished manpath processing, regain privs */
	regain_effective_privs ();

#ifdef MAN_DB_UPDATES
	/* If `-u', do it now. */
	if (update) {
		int status = run_mandb (0, NULL, NULL);
		if (status)
			error (0, 0,
			       _("mandb command failed with exit status %d"),
			       status);
	}
#endif /* MAN_DB_UPDATES */

	while (first_arg < argc) {
		int status = OK;
		int found = 0;
		static int maybe_section = 0;
		const char *nextarg = argv[first_arg++];

		/*
     		 * See if this argument is a valid section name.  If not,
      		 * is_section returns NULL.
      		 */
		if (!catman) {
			tmp = is_section (nextarg);
			if (tmp) {
				section = tmp;
				debug ("\nsection: %s\n", section);
				maybe_section = 1;
			}
		}

		if (maybe_section) {
			if (first_arg < argc)
				/* e.g. 'man 3perl Shell' */
				nextarg = argv[first_arg++];
			else
				/* e.g. 'man 9wm' */
				section = NULL;
				/* ... but leave maybe_section set so we can
				 * tell later that this happened.
				 */
		}

		/* this is where we actually start looking for the man page */
		skip = 0;
		if (global_apropos)
			status = do_global_apropos (nextarg, &found);
		else {
			bool found_subpage = false;
			if (subpages && first_arg < argc) {
				char *subname = xasprintf (
					"%s-%s", nextarg, argv[first_arg]);
				status = man (subname, &found);
				free (subname);
				if (status == OK) {
					found_subpage = true;
					++first_arg;
				}
			}
			if (!found_subpage && subpages && first_arg < argc) {
				char *subname = xasprintf (
					"%s_%s", nextarg, argv[first_arg]);
				status = man (subname, &found);
				free (subname);
				if (status == OK) {
					found_subpage = true;
					++first_arg;
				}
			}
			if (!found_subpage)
				status = man (nextarg, &found);
		}

		/* clean out the cache of database lookups for each man page */
		if (db_map) {
			gl_map_free (db_map);
			db_map = NULL;
		}

		if (section && maybe_section) {
			if (status != OK && !catman) {
				/* Maybe the section wasn't a section after
				 * all? e.g. 'man 9wm fvwm'.
				 */
				bool found_subpage = false;
				debug ("\nRetrying section %s as name\n",
				       section);
				tmp = section;
				section = NULL;
				if (subpages) {
					char *subname = xasprintf (
						"%s-%s", tmp, nextarg);
					status = man (subname, &found);
					free (subname);
					if (status == OK) {
						found_subpage = true;
						++first_arg;
					}
				}
				if (!found_subpage)
					status = man (tmp, &found);
				if (db_map) {
					gl_map_free (db_map);
					db_map = NULL;
				}
				/* ... but don't gripe about it if it doesn't
				 * work!
				 */
				if (status == OK) {
					/* It was a name after all, so arrange
					 * to try the next page again with a
					 * null section.
					 */
					nextarg = tmp;
					--first_arg;
				} else
					/* No go, it really was a section. */
					section = tmp;
			}
		}

		if (status != OK && !catman) {
			if (!skip) {
				exit_status = status;
				if (exit_status == NOT_FOUND) {
					if (!section && maybe_section &&
					    CTYPE (isdigit, nextarg[0]))
						gripe_no_name (nextarg);
					else
						gripe_no_man (nextarg, section);
				}
			}
		} else {
			debug ("\nFound %d man pages\n", found);
			if (catman) {
				printf ("%s", nextarg);
				if (section)
					printf ("(%s)", section);
				if (first_arg != argc)
					fputs (", ", stdout);
				else
					fputs (".\n", stdout);
			}
		}

		maybe_section = 0;

		chkr_garbage_detector ();
	}
	if (db_map) {
		gl_map_free (db_map);
		db_map = NULL;
	}

	drop_effective_privs ();

	free (database);
	gl_list_free (section_list);
	free_pathlist (manpathlist);
	free (internal_locale);
	sandbox_free (sandbox);
	exit (exit_status);
}
