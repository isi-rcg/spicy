/*
 * watch -- execute a program repeatedly, displaying output fullscreen
 *
 * Based on the original 1991 'watch' by Tony Rems <rembo@unisoft.com>
 * (with mods and corrections by Francois Pinard).
 *
 * Substantially reworked, new features (differences option, SIGWINCH
 * handling, unlimited command length, long line handling) added Apr
 * 1999 by Mike Coleman <mkc@acm.org>.
 *
 * Changes by Albert Cahalan, 2002-2003.
 * stderr handling, exec, and beep option added by Morty Abzug, 2008
 * Unicode Support added by Jarrod Lowe <procps@rrod.net> in 2009.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "c.h"
#include "config.h"
#include "fileutils.h"
#include "nls.h"
#include "strutils.h"
#include "xalloc.h"
#include <ctype.h>
#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#ifdef WITH_WATCH8BIT
# define _XOPEN_SOURCE_EXTENDED 1
# include <wchar.h>
# include <wctype.h>
#endif	/* WITH_WATCH8BIT */
#include <ncurses.h>

#ifdef FORCE_8BIT
# undef isprint
# define isprint(x) ( (x>=' '&&x<='~') || (x>=0xa0) )
#endif


/* Boolean command line options */
static int flags;
#define WATCH_DIFF	(1 << 1)
#define WATCH_CUMUL	(1 << 2)
#define WATCH_EXEC	(1 << 3)
#define WATCH_BEEP	(1 << 4)
#define WATCH_COLOR	(1 << 5)
#define WATCH_ERREXIT	(1 << 6)
#define WATCH_CHGEXIT	(1 << 7)

static int curses_started = 0;
static long height = 24, width = 80;
static int screen_size_changed = 0;
static int first_screen = 1;
static int show_title = 2;	/* number of lines used, 2 or 0 */
static int precise_timekeeping = 0;

#define min(x,y) ((x) > (y) ? (y) : (x))
#define MAX_ANSIBUF 100

static void __attribute__ ((__noreturn__))
    usage(FILE * out)
{
	fputs(USAGE_HEADER, out);
	fprintf(out,
              _(" %s [options] command\n"), program_invocation_short_name);
	fputs(USAGE_OPTIONS, out);
	fputs(_("  -b, --beep             beep if command has a non-zero exit\n"), out);
	fputs(_("  -c, --color            interpret ANSI color and style sequences\n"), out);
	fputs(_("  -d, --differences[=<permanent>]\n"
                "                         highlight changes between updates\n"), out);
	fputs(_("  -e, --errexit          exit if command has a non-zero exit\n"), out);
	fputs(_("  -g, --chgexit          exit when output from command changes\n"), out);
	fputs(_("  -n, --interval <secs>  seconds to wait between updates\n"), out);
	fputs(_("  -p, --precise          attempt run command in precise intervals\n"), out);
	fputs(_("  -t, --no-title         turn off header\n"), out);
	fputs(_("  -x, --exec             pass command to exec instead of \"sh -c\"\n"), out);
	fputs(USAGE_SEPARATOR, out);
	fputs(USAGE_HELP, out);
	fputs(_(" -v, --version  output version information and exit\n"), out);
	fprintf(out, USAGE_MAN_TAIL("watch(1)"));

	exit(out == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}

static int nr_of_colors;
static int attributes;
static int fg_col;
static int bg_col;


static void reset_ansi(void)
{
	attributes = A_NORMAL;
	fg_col = 0;
	bg_col = 0;
}

static void init_ansi_colors(void)
{
	short ncurses_colors[] = {
		-1, COLOR_BLACK, COLOR_RED, COLOR_GREEN, COLOR_YELLOW,
		COLOR_BLUE, COLOR_MAGENTA, COLOR_CYAN, COLOR_WHITE
	};

	nr_of_colors = sizeof(ncurses_colors) / sizeof(short);

	for (bg_col = 0; bg_col < nr_of_colors; bg_col++)
		for (fg_col = 0; fg_col < nr_of_colors; fg_col++)
			init_pair(bg_col * nr_of_colors + fg_col + 1, ncurses_colors[fg_col], ncurses_colors[bg_col]);
	reset_ansi();
}


static int set_ansi_attribute(const int attrib)
{
	switch (attrib) {
	case -1:	/* restore last settings */
		break;
	case 0:		/* restore default settings */
		reset_ansi();
		break;
	case 1:		/* set bold / increased intensity */
		attributes |= A_BOLD;
		break;
	case 2:		/* set decreased intensity (if supported) */
		attributes |= A_DIM;
		break;
#ifdef A_ITALIC
	case 3:		/* set italic (if supported) */
		attributes |= A_ITALIC;
		break;
#endif
	case 4:		/* set underline */
		attributes |= A_UNDERLINE;
		break;
	case 5:		/* set blinking */
		attributes |= A_BLINK;
		break;
	case 7:		/* set inversed */
		attributes |= A_REVERSE;
		break;
	case 21:	/* unset bold / increased intensity */
		attributes &= ~A_BOLD;
		break;
	case 22:	/* unset bold / any intensity modifier */
		attributes &= ~(A_BOLD | A_DIM);
		break;
#ifdef A_ITALIC
	case 23:	/* unset italic */
		attributes &= ~A_ITALIC;
		break;
#endif
	case 24:	/* unset underline */
		attributes &= ~A_UNDERLINE;
		break;
	case 25:	/* unset blinking */
		attributes &= ~A_BLINK;
		break;
	case 27:	/* unset inversed */
		attributes &= ~A_REVERSE;
		break;
    case 39:
        fg_col = 0;
        break;
    case 49:
        bg_col = 0;
        break;
	default:
		if (attrib >= 30 && attrib <= 37) {	/* set foreground color */
			fg_col = attrib - 30 + 1;
		} else if (attrib >= 40 && attrib <= 47) { /* set background color */
			bg_col = attrib - 40 + 1;
		} else {
			return 0; /* Not understood */
		}
	}
	attrset(attributes | COLOR_PAIR(bg_col * nr_of_colors + fg_col + 1));
    return 1;
}

static void process_ansi(FILE * fp)
{
	int i, c;
	char buf[MAX_ANSIBUF];
	char *numstart, *endptr;

	c = getc(fp);
	if (c != '[') {
		ungetc(c, fp);
		return;
	}
	for (i = 0; i < MAX_ANSIBUF; i++) {
		c = getc(fp);
		/* COLOUR SEQUENCE ENDS in 'm' */
		if (c == 'm') {
			buf[i] = '\0';
			break;
		}
		if ((c < '0' || c > '9') && c != ';') {
			return;
		}
		buf[i] = (char)c;
	}
	/*
	 * buf now contains a semicolon-separated list of decimal integers,
	 * each indicating an attribute to apply.
	 * For example, buf might contain "0;1;31", derived from the color
	 * escape sequence "<ESC>[0;1;31m". There can be 1 or more
	 * attributes to apply, but typically there are between 1 and 3.
	 */

    /* Special case of <ESC>[m */
    if (buf[0] == '\0')
        set_ansi_attribute(0);

    for (endptr = numstart = buf; *endptr != '\0'; numstart = endptr + 1) {
        if (!set_ansi_attribute(strtol(numstart, &endptr, 10)))
            break;
    }
}

static void __attribute__ ((__noreturn__)) do_exit(int status)
{
	if (curses_started)
		endwin();
	exit(status);
}

/* signal handler */
static void die(int notused __attribute__ ((__unused__)))
{
	do_exit(EXIT_SUCCESS);
}

static void winch_handler(int notused __attribute__ ((__unused__)))
{
	screen_size_changed = 1;
}

static char env_col_buf[24];
static char env_row_buf[24];
static int incoming_cols;
static int incoming_rows;

static void get_terminal_size(void)
{
	struct winsize w;
	if (!incoming_cols) {
		/* have we checked COLUMNS? */
		const char *s = getenv("COLUMNS");
		incoming_cols = -1;
		if (s && *s) {
			long t;
			char *endptr;
			t = strtol(s, &endptr, 0);
			if (!*endptr && 0 < t)
				incoming_cols = t;
			width = incoming_cols;
			snprintf(env_col_buf, sizeof env_col_buf, "COLUMNS=%ld",
				 width);
			putenv(env_col_buf);
		}
	}
	if (!incoming_rows) {
		/* have we checked LINES? */
		const char *s = getenv("LINES");
		incoming_rows = -1;
		if (s && *s) {
			long t;
			char *endptr;
			t = strtol(s, &endptr, 0);
			if (!*endptr && 0 < t)
				incoming_rows = t;
			height = incoming_rows;
			snprintf(env_row_buf, sizeof env_row_buf, "LINES=%ld",
				 height);
			putenv(env_row_buf);
		}
	}
	if (ioctl(STDERR_FILENO, TIOCGWINSZ, &w) == 0) {
		if (incoming_cols < 0 || incoming_rows < 0) {
			if (incoming_rows < 0 && w.ws_row > 0) {
				height = w.ws_row;
				snprintf(env_row_buf, sizeof env_row_buf,
					 "LINES=%ld", height);
				putenv(env_row_buf);
			}
			if (incoming_cols < 0 && w.ws_col > 0) {
				width = w.ws_col;
				snprintf(env_col_buf, sizeof env_col_buf,
					 "COLUMNS=%ld", width);
				putenv(env_col_buf);
			}
		}
	}
}

/* get current time in usec */
typedef unsigned long long watch_usec_t;
#define USECS_PER_SEC (1000000ull)
static watch_usec_t get_time_usec()
{
	struct timeval now;
	gettimeofday(&now, NULL);
	return USECS_PER_SEC * now.tv_sec + now.tv_usec;
}

#ifdef WITH_WATCH8BIT
/* read a wide character from a popen'd stream */
#define MAX_ENC_BYTES 16
wint_t my_getwc(FILE * s);
wint_t my_getwc(FILE * s)
{
	/* assuming no encoding ever consumes more than 16 bytes */
	char i[MAX_ENC_BYTES];
	int byte = 0;
	int convert;
	int x;
	wchar_t rval;
	while (1) {
		i[byte] = getc(s);
		if (i[byte] == EOF) {
			return WEOF;
		}
		byte++;
		errno = 0;
		mbtowc(NULL, NULL, 0);
		convert = mbtowc(&rval, i, byte);
		x = errno;
		if (convert > 0) {
			/* legal conversion */
			return rval;
		}
		if (byte == MAX_ENC_BYTES) {
			while (byte > 1) {
				/* at least *try* to fix up */
				ungetc(i[--byte], s);
			}
			errno = -EILSEQ;
			return WEOF;
		}
	}
}
#endif	/* WITH_WATCH8BIT */

#ifdef WITH_WATCH8BIT
static void output_header(wchar_t *restrict wcommand, int wcommand_characters, double interval)
#else
static void output_header(char *restrict command, double interval)
#endif	/* WITH_WATCH8BIT */
{
	time_t t = time(NULL);
	char *ts = ctime(&t);
	char *header;
	char *right_header;
    int max_host_name_len = (int) sysconf(_SC_HOST_NAME_MAX);
	char hostname[max_host_name_len + 1];
	int command_columns = 0;	/* not including final \0 */

	gethostname(hostname, sizeof(hostname));

	/*
	 * left justify interval and command, right justify hostname and time,
	 * clipping all to fit window width
	 */
	int hlen = asprintf(&header, _("Every %.1fs: "), interval);
	int rhlen = asprintf(&right_header, _("%s: %s"), hostname, ts);

	/*
	 * the rules:
	 *   width < rhlen : print nothing
	 *   width < rhlen + hlen + 1: print hostname, ts
	 *   width = rhlen + hlen + 1: print header, hostname, ts
	 *   width < rhlen + hlen + 4: print header, ..., hostname, ts
	 *   width < rhlen + hlen + wcommand_columns: print header,
	 *                           truncated wcommand, ..., hostname, ts
	 *   width > "": print header, wcomand, hostname, ts
	 * this is slightly different from how it used to be
	 */
	if (width < rhlen) {
		free(header);
		free(right_header);
		return;
	}
	if (rhlen + hlen + 1 <= width) {
		mvaddstr(0, 0, header);
		if (rhlen + hlen + 2 <= width) {
			if (width < rhlen + hlen + 4) {
				mvaddstr(0, width - rhlen - 4, "... ");
			} else {
#ifdef WITH_WATCH8BIT
	            command_columns = wcswidth(wcommand, -1);
				if (width < rhlen + hlen + command_columns) {
					/* print truncated */
					int available = width - rhlen - hlen;
					int in_use = command_columns;
					int wcomm_len = wcommand_characters;
					while (available - 4 < in_use) {
						wcomm_len--;
						in_use = wcswidth(wcommand, wcomm_len);
					}
					mvaddnwstr(0, hlen, wcommand, wcomm_len);
					mvaddstr(0, width - rhlen - 4, "... ");
				} else {
					mvaddwstr(0, hlen, wcommand);
				}
#else
                command_columns = strlen(command);
                if (width < rhlen + hlen + command_columns) {
                    /* print truncated */
                    mvaddnstr(0, hlen, command, width - rhlen - hlen - 4);
                    mvaddstr(0, width - rhlen - 4, "... ");
                } else {
                    mvaddnstr(0, hlen, command, width - rhlen - hlen);
                }
#endif	/* WITH_WATCH8BIT */
			}
		}
	}
	mvaddstr(0, width - rhlen + 1, right_header);
	free(header);
	free(right_header);
	return;
}

static int run_command(char *restrict command, char **restrict command_argv)
{
	FILE *p;
	int x, y;
	int oldeolseen = 1;
	int pipefd[2];
	pid_t child;
	int exit_early = 0;
	int status;

	/* allocate pipes */
	if (pipe(pipefd) < 0)
		xerr(7, _("unable to create IPC pipes"));

	/* flush stdout and stderr, since we're about to do fd stuff */
	fflush(stdout);
	fflush(stderr);

	/* fork to prepare to run command */
	child = fork();

	if (child < 0) {		/* fork error */
		xerr(2, _("unable to fork process"));
	} else if (child == 0) {	/* in child */
		close(pipefd[0]);		/* child doesn't need read side of pipe */
		close(1);			/* prepare to replace stdout with pipe */
		if (dup2(pipefd[1], 1) < 0) {	/* replace stdout with write side of pipe */
			xerr(3, _("dup2 failed"));
		}
		close(pipefd[1]);		/* once duped, the write fd isn't needed */
		dup2(1, 2);			/* stderr should default to stdout */

		if (flags & WATCH_EXEC) {	/* pass command to exec instead of system */
			if (execvp(command_argv[0], command_argv) == -1) {
				xerr(4, _("unable to execute '%s'"),
				     command_argv[0]);
			}
		} else {
			status = system(command);	/* watch manpage promises sh quoting */
			/* propagate command exit status as child exit status */
			if (!WIFEXITED(status)) {	/* child exits nonzero if command does */
				exit(EXIT_FAILURE);
			} else {
				exit(WEXITSTATUS(status));
			}
		}
	}

	/* otherwise, we're in parent */
	close(pipefd[1]);	/* close write side of pipe */
	if ((p = fdopen(pipefd[0], "r")) == NULL)
		xerr(5, _("fdopen"));

	reset_ansi();
	for (y = show_title; y < height; y++) {
		int eolseen = 0, tabpending = 0, tabwaspending = 0;
		if (flags & WATCH_COLOR)
			set_ansi_attribute(-1);
#ifdef WITH_WATCH8BIT
		wint_t carry = WEOF;
#endif
		for (x = 0; x < width; x++) {
#ifdef WITH_WATCH8BIT
			wint_t c = ' ';
#else
			int c = ' ';
#endif
			int attr = 0;

			if (tabwaspending && (flags & WATCH_COLOR))
				set_ansi_attribute(-1);
			tabwaspending = 0;

			if (!eolseen) {
				/* if there is a tab pending, just
				 * spit spaces until the next stop
				 * instead of reading characters */
				if (!tabpending)
#ifdef WITH_WATCH8BIT
					do {
						if (carry == WEOF) {
							c = my_getwc(p);
						} else {
							c = carry;
							carry = WEOF;
						}
					} while (c != WEOF && !iswprint(c)
						 && c < 128
						 && wcwidth(c) == 0
						 && c != L'\n'
						 && c != L'\t'
						 && (c != L'\033'
						     || !(flags & WATCH_COLOR)));
#else
					do
						c = getc(p);
					while (c != EOF && !isprint(c)
					       && c != '\n'
					       && c != '\t'
					       && (c != L'\033'
						   || !(flags & WATCH_COLOR)));
#endif
				if (c == L'\033' && (flags & WATCH_COLOR)) {
					x--;
					process_ansi(p);
					continue;
				}
				if (c == L'\n')
					if (!oldeolseen && x == 0) {
						x = -1;
						continue;
					} else
						eolseen = 1;
				else if (c == L'\t')
					tabpending = 1;
#ifdef WITH_WATCH8BIT
				if (x == width - 1 && wcwidth(c) == 2) {
					y++;
					x = -1;		/* process this double-width */
					carry = c;	/* character on the next line */
					continue;	/* because it won't fit here */
				}
				if (c == WEOF || c == L'\n' || c == L'\t') {
					c = L' ';
					if (flags & WATCH_COLOR)
						attrset(A_NORMAL);
				}
#else
				if (c == EOF || c == '\n' || c == '\t') {
					c = ' ';
					if (flags & WATCH_COLOR)
						attrset(A_NORMAL);
				}
#endif
				if (tabpending && (((x + 1) % 8) == 0)) {
					tabpending = 0;
					tabwaspending = 1;
				}
			}
			move(y, x);

			if (!first_screen && !exit_early && (flags & WATCH_CHGEXIT)) {
#ifdef WITH_WATCH8BIT
				cchar_t oldc;
				in_wch(&oldc);
				exit_early = (wchar_t) c != oldc.chars[0];
#else
				chtype oldch = inch();
				unsigned char oldc = oldch & A_CHARTEXT;
				exit_early = (unsigned char)c != oldc;
#endif
			}
			if (flags & WATCH_DIFF) {
#ifdef WITH_WATCH8BIT
				cchar_t oldc;
				in_wch(&oldc);
				attr = !first_screen
				    && ((wchar_t) c != oldc.chars[0]
					||
					((flags & WATCH_CUMUL)
					 && (oldc.attr & A_ATTRIBUTES)));
#else
				chtype oldch = inch();
				unsigned char oldc = oldch & A_CHARTEXT;
				attr = !first_screen
				    && ((unsigned char)c != oldc
					||
					((flags & WATCH_CUMUL)
					 && (oldch & A_ATTRIBUTES)));
#endif
			}
			if (attr)
				standout();
#ifdef WITH_WATCH8BIT
			addnwstr((wchar_t *) & c, 1);
#else
			addch(c);
#endif
			if (attr)
				standend();
#ifdef WITH_WATCH8BIT
			if (wcwidth(c) == 0) {
				x--;
			}
			if (wcwidth(c) == 2) {
				x++;
			}
#endif
		}
		oldeolseen = eolseen;
	}

	fclose(p);


	/* harvest child process and get status, propagated from command */
	if (waitpid(child, &status, 0) < 0)
		xerr(8, _("waitpid"));

	/* if child process exited in error, beep if option_beep is set */
	if ((!WIFEXITED(status) || WEXITSTATUS(status))) {
		if (flags & WATCH_BEEP)
			beep();
		if (flags & WATCH_ERREXIT) {
			mvaddstr(height - 1, 0,
				 _("command exit with a non-zero status, press a key to exit"));
			refresh();
			fgetc(stdin);
			endwin();
			exit(8);
		}
	}
	first_screen = 0;
	refresh();
	return exit_early;
}

int main(int argc, char *argv[])
{
	int optc;
	double interval = 2;
	char *command;
	char **command_argv;
	int command_length = 0;	/* not including final \0 */
	watch_usec_t next_loop;	/* next loop time in us, used for precise time
				 * keeping only */
#ifdef WITH_WATCH8BIT
	wchar_t *wcommand = NULL;
	int wcommand_characters = 0;	/* not including final \0 */
#endif	/* WITH_WATCH8BIT */

	static struct option longopts[] = {
		{"color", no_argument, 0, 'c'},
		{"differences", optional_argument, 0, 'd'},
		{"help", no_argument, 0, 'h'},
		{"interval", required_argument, 0, 'n'},
		{"beep", no_argument, 0, 'b'},
		{"errexit", no_argument, 0, 'e'},
		{"chgexit", no_argument, 0, 'g'},
		{"exec", no_argument, 0, 'x'},
		{"precise", no_argument, 0, 'p'},
		{"no-title", no_argument, 0, 't'},
		{"version", no_argument, 0, 'v'},
		{0, 0, 0, 0}
	};

#ifdef HAVE_PROGRAM_INVOCATION_NAME
	program_invocation_name = program_invocation_short_name;
#endif
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	atexit(close_stdout);

	while ((optc =
		getopt_long(argc, argv, "+bced::ghn:pvtx", longopts, (int *)0))
	       != EOF) {
		switch (optc) {
		case 'b':
			flags |= WATCH_BEEP;
			break;
		case 'c':
			flags |= WATCH_COLOR;
			break;
		case 'd':
			flags |= WATCH_DIFF;
			if (optarg)
				flags |= WATCH_CUMUL;
			break;
		case 'e':
			flags |= WATCH_ERREXIT;
			break;
		case 'g':
			flags |= WATCH_CHGEXIT;
			break;
		case 't':
			show_title = 0;
			break;
		case 'x':
			flags |= WATCH_EXEC;
			break;
		case 'n':
			interval = strtod_nol_or_err(optarg, _("failed to parse argument"));
			if (interval < 0.1)
				interval = 0.1;
			if (interval > UINT_MAX)
				interval = UINT_MAX;
			break;
		case 'p':
			precise_timekeeping = 1;
			break;
		case 'h':
			usage(stdout);
			break;
		case 'v':
			printf(PROCPS_NG_VERSION);
			return EXIT_SUCCESS;
		default:
			usage(stderr);
			break;
		}
	}

	if (optind >= argc)
		usage(stderr);

	/* save for later */
	command_argv = &(argv[optind]);

	command = xstrdup(argv[optind++]);
	command_length = strlen(command);
	for (; optind < argc; optind++) {
		char *endp;
		int s = strlen(argv[optind]);
		/* space and \0 */
		command = xrealloc(command, command_length + s + 2);
		endp = command + command_length;
		*endp = ' ';
		memcpy(endp + 1, argv[optind], s);
		/* space then string length */
		command_length += 1 + s;
		command[command_length] = '\0';
	}

#ifdef WITH_WATCH8BIT
	/* convert to wide for printing purposes */
	/*mbstowcs(NULL, NULL, 0); */
	wcommand_characters = mbstowcs(NULL, command, 0);
	if (wcommand_characters < 0) {
		fprintf(stderr, _("unicode handling error\n"));
		exit(EXIT_FAILURE);
	}
	wcommand =
	    (wchar_t *) malloc((wcommand_characters + 1) * sizeof(wcommand));
	if (wcommand == NULL) {
		fprintf(stderr, _("unicode handling error (malloc)\n"));
		exit(EXIT_FAILURE);
	}
	mbstowcs(wcommand, command, wcommand_characters + 1);
#endif	/* WITH_WATCH8BIT */

	get_terminal_size();

	/* Catch keyboard interrupts so we can put tty back in a sane
	 * state.  */
	signal(SIGINT, die);
	signal(SIGTERM, die);
	signal(SIGHUP, die);
	signal(SIGWINCH, winch_handler);

	/* Set up tty for curses use.  */
	curses_started = 1;
	initscr();
	if (flags & WATCH_COLOR) {
		if (has_colors()) {
			start_color();
			use_default_colors();
			init_ansi_colors();
		} else {
			flags |= WATCH_COLOR;
		}
	}
	nonl();
	noecho();
	cbreak();

	if (precise_timekeeping)
		next_loop = get_time_usec();

	while (1) {
		if (screen_size_changed) {
			get_terminal_size();
			resizeterm(height, width);
			clear();
			/* redrawwin(stdscr); */
			screen_size_changed = 0;
			first_screen = 1;
		}

		if (show_title)
#ifdef WITH_WATCH8BIT
			output_header(wcommand, wcommand_characters, interval);
#else
			output_header(command, interval);
#endif	/* WITH_WATCH8BIT */

		if (run_command(command, command_argv))
			break;


		if (precise_timekeeping) {
			watch_usec_t cur_time = get_time_usec();
			next_loop += USECS_PER_SEC * interval;
			if (cur_time < next_loop)
				usleep(next_loop - cur_time);
		} else
			if (interval < UINT_MAX / USECS_PER_SEC)
				usleep(interval * USECS_PER_SEC);
			else
				sleep(interval);
	}

	endwin();
	return EXIT_SUCCESS;
}
