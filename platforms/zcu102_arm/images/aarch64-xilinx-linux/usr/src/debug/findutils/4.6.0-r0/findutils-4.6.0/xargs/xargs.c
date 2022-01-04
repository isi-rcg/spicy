/* xargs -- build and execute command lines from standard input
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000, 2003, 2004, 2005,
   2006, 2007, 2008, 2009, 2010, 2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* Written by Mike Rendell <michael@cs.mun.ca>
   and David MacKenzie <djm@gnu.org>.
   Modifications by
        James Youngman
	Dmitry V. Levin
*/

/* We want SIG_ATOMIC_MAX to be defined.  The implementation only does
   this if __STDC_LIMIT_MACROS is #defined before <stdint.h> is
   included (see the footnote to section 7.18.3 of ISO C99).  Because
   some other header may #include <stdint.h>, we define the macro
   here, first. */
#define __STDC_LIMIT_MACROS

/* config.h must be included first. */
#include <config.h>

/* system headers. */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <wchar.h>

/* gnulib headers. */
#include "closein.h"
#include "error.h"
#include "gettext.h"
#include "progname.h"
#include "quotearg.h"
#include "safe-read.h"
#include "xalloc.h"

/* find headers. */
#include "buildcmd.h"
#include "fdleak.h"
#include "findutils-version.h"

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define _(Text) Text
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)
#endif
#ifdef gettext_noop
# define N_(String) gettext_noop(String)
#else
/* See locate.c for explanation as to why not use (String) */
# define N_(String) String
#endif

#ifndef LONG_MAX
#define LONG_MAX (~(1 << (sizeof (long) * 8 - 1)))
#endif

#define ISBLANK(c) (isascii (c) && isblank (c))
#define ISSPACE(c) (ISBLANK (c) || (c) == '\n' || (c) == '\r' \
		    || (c) == '\f' || (c) == '\v')

/* Return nonzero if S is the EOF string.  */
#define EOF_STR(s) (eof_str && *eof_str == *s && !strcmp (eof_str, s))

extern char *version_string;

static FILE *input_stream;

/* Buffer for reading arguments from input.  */
static char *linebuf;

static int keep_stdin = 0;

/* Line number in stdin since the last command was executed.  */
static size_t lineno = 0;

static struct buildcmd_state bc_state;
static struct buildcmd_control bc_ctl;

/* Did we already complain about NUL characters in the input? */
static int nullwarning_given = 0;


/* If nonzero, when this string is read on stdin it is treated as
   end of file.
   IEEE Std 1003.1, 2004 Edition allows this to be NULL.
   In findutils releases up to and including 4.2.8, this was "_".
*/
static char *eof_str = NULL;

/* Number of chars in the initial args.  */
/* static int initial_argv_chars = 0; */

/* true when building up initial arguments in `cmd_argv'.  */
static bool initial_args = true;

/* If nonzero, the maximum number of child processes that can be running
   at once.  */
/* TODO: check conversion safety (i.e. range) for -P option. */
#define MAX_PROC_MAX SIG_ATOMIC_MAX
static volatile sig_atomic_t proc_max = 1;

/* Did we fork a child yet? */
static bool procs_executed = false;

/* The number of elements in `pids'.  */
static unsigned long int procs_executing = 0uL;

/* List of child processes currently executing.  */
static pid_t *pids = NULL;

/* The number of allocated elements in `pids'. */
static size_t pids_alloc = 0u;

/* Process ID of the parent xargs process. */
static pid_t parent;

/* If nonzero, we've been signaled that we can start more child processes. */
static volatile sig_atomic_t stop_waiting = 0;

/* Exit status; nonzero if any child process exited with a
   status of 1-125.  */
static volatile int child_error = EXIT_SUCCESS;

static volatile int original_exit_value;

/* If true, print each command on stderr before executing it.  */
static bool print_command = false; /* Option -t */

/* If true, query the user before executing each command, and only
   execute the command if the user responds affirmatively.  */
static bool query_before_executing = false;

/* The delimiter for input arguments.   This is only consulted if the
 * -0 or -d option had been given.
 */
static char input_delimiter = '\0';


/* Name of the environment variable which indicates which 'slot'
 * the child process is in.   This can be used to do some kind of basic
 * load distribution.   We guarantee not to allow two processes to run
 * at the same time with the same value of this variable.
 */
static char* slot_var_name = NULL;

enum LongOptionIdentifier
  {
    PROCESS_SLOT_VAR = CHAR_MAX+1
  };

static struct option const longopts[] =
{
  {"null", no_argument, NULL, '0'},
  {"arg-file", required_argument, NULL, 'a'},
  {"delimiter", required_argument, NULL, 'd'},
  {"eof", optional_argument, NULL, 'e'},
  {"replace", optional_argument, NULL, 'I'},
  {"max-lines", optional_argument, NULL, 'l'},
  {"max-args", required_argument, NULL, 'n'},
  {"interactive", no_argument, NULL, 'p'},
  {"no-run-if-empty", no_argument, NULL, 'r'},
  {"max-chars", required_argument, NULL, 's'},
  {"verbose", no_argument, NULL, 't'},
  {"show-limits", no_argument, NULL, 'S'},
  {"exit", no_argument, NULL, 'x'},
  {"max-procs", required_argument, NULL, 'P'},
  {"process-slot-var", required_argument, NULL, PROCESS_SLOT_VAR},
  {"version", no_argument, NULL, 'v'},
  {"help", no_argument, NULL, 'h'},
  {NULL, no_argument, NULL, 0}
};

/* xargs exits with status values with specific meanings (this is a POSIX requirement).
   These are the values.
*/
enum XargsStatusValues {
  XARGS_EXIT_CLIENT_EXIT_NONZERO = 123, /* utility exited with nonzero status */
  XARGS_EXIT_CLIENT_EXIT_255 = 124,     /* utility exites with status 255 */
  XARGS_EXIT_CLIENT_FATAL_SIG = 125,    /* utility died from a fatal signal */
  XARGS_EXIT_COMMAND_CANNOT_BE_RUN = 126, /* canot run the command */
  XARGS_EXIT_COMMAND_NOT_FOUND = 127,	  /* cannot find the command */
};
/* Exit status values the child might use. */
enum  ClientStatusValues {
  CHILD_EXIT_PLEASE_STOP_IMMEDIATELY = 255
};

static int read_line (void);
static int read_string (void);
static bool print_args (bool ask);
/* static void do_exec (void); */
static int xargs_do_exec (struct buildcmd_control *ctl, void *usercontext, int argc, char **argv);
static void exec_if_possible (void);
static unsigned int add_proc (pid_t pid);
static void wait_for_proc (bool all, unsigned int minreap);
static void wait_for_proc_all (void);
static void increment_proc_max (int);
static void decrement_proc_max (int);
static long parse_num (char *str, int option, long min, long max, int fatal);
static void usage (FILE * stream);


static char
get_char_oct_or_hex_escape (const char *s)
{
  const char * p;
  int base = 8;
  unsigned long val;
  char *endp;

  assert ('\\' == s[0]);

  if ('x' == s[1])
    {
      /* hex */
      p = s+2;
      base = 16;
    }
  else if (isdigit ((unsigned char) s[1]))
    {
      /* octal */
      p = s+1;
      base = 8;
    }
  else
    {
      p = NULL;			/* Silence compiler warning. */
      error (EXIT_FAILURE, 0,
	     _("Invalid escape sequence %s in input delimiter specification."),
	     s);
    }
  errno = 0;
  endp = NULL;
  val = strtoul (p, &endp, base);

  /* This if condition is carefully constructed to do
   * the right thing if UCHAR_MAX has the same
   * value as ULONG_MAX.   IF UCHAR_MAX==ULONG_MAX,
   * then val can never be greater than UCHAR_MAX.
   */
  if ((ULONG_MAX == val && ERANGE == errno)
      || (val > UCHAR_MAX))
    {
      if (16 == base)
	{
	  error (EXIT_FAILURE, 0,
		 _("Invalid escape sequence %s in input delimiter specification; character values must not exceed %lx."),
		 s, (unsigned long)UCHAR_MAX);
	}
      else
	{
	  error (EXIT_FAILURE, 0,
		 _("Invalid escape sequence %s in input delimiter specification; character values must not exceed %lo."),
		 s, (unsigned long)UCHAR_MAX);
	}
    }

  /* check for trailing garbage */
  if (0 != *endp)
    {
      error (EXIT_FAILURE, 0,
	     _("Invalid escape sequence %s in input delimiter specification; trailing characters %s not recognised."),
	     s, endp);
    }

  return (char) val;
}


static char
get_input_delimiter (const char *s)
{
  if (1 == strlen (s))
    {
      return s[0];
    }
  else
    {
      if ('\\' == s[0])
	{
	  /* an escape code */
	  switch (s[1])
	    {
	    case 'a':
	      return '\a';
	    case 'b':
	      return '\b';
	    case 'f':
	      return '\f';
	    case 'n':
	      return '\n';
	    case 'r':
	      return '\r';
	    case 't':
	      return'\t';
	    case 'v':
	      return '\v';
	    case '\\':
	      return '\\';
	    default:
	      return get_char_oct_or_hex_escape (s);
	    }
	}
      else
	{
	  error (EXIT_FAILURE, 0,
		 _("Invalid input delimiter specification %s: the delimiter must be either a single character or an escape sequence starting with \\."),
		 s);
	  /*NOTREACHED*/
	  return 0;
	}
    }
}

static void
noop (void)
{
  /* does nothing. */
}

static void
fail_due_to_env_size (void)
{
  error (EXIT_FAILURE, 0, _("environment is too large for exec"));
}

static size_t
smaller_of (size_t a, size_t b)
{
  if (a < b)
    return a;
  else
    return b;
}


static FILE* fopen_cloexec_for_read_only (const char *file_name)
{
  int fd = open_cloexec (file_name, O_RDONLY);
  if (fd < 0)
    {
      return NULL;
    }
  else
    {
      FILE *result = fdopen (fd, "r");
      if (!result)
	{
	  int saved_errno = errno;
	  close (fd);
	  errno = saved_errno;
	  return NULL;
	}
      return result;
    }
}


int
main (int argc, char **argv)
{
  int optc, option_index;
  int show_limits = 0;			/* --show-limits */
  int always_run_command = 1;
  const char *input_file = "-"; /* "-" is stdin */
  char default_cmd[] = "echo";
  char *default_arglist[1];
  int (*read_args) (void) = read_line;
  void (*act_on_init_result)(void) = noop;
  enum BC_INIT_STATUS bcstatus;
  enum { XARGS_POSIX_HEADROOM = 2048u };
  struct sigaction sigact;

  if (argv[0])
    set_program_name (argv[0]);
  else
    set_program_name ("xargs");

  remember_non_cloexec_fds ();
  parent = getpid ();
  original_exit_value = EXIT_SUCCESS;

#ifdef HAVE_SETLOCALE
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  if (atexit (close_stdin) || atexit (wait_for_proc_all))
    {
      error (EXIT_FAILURE, errno, _("The atexit library function failed"));
    }

  /* xargs is required by POSIX to allow 2048 bytes of headroom
   * for extra environment variables (that perhaps the utliity might
   * want to set before execing something else).
   */
  bcstatus = bc_init_controlinfo (&bc_ctl, XARGS_POSIX_HEADROOM);

  /* The bc_init_controlinfo call may have determined that the
   * environment is too big.  In that case, we will fail with
   * an error message after processing the command-line options,
   * as "xargs --help" should still work even if the environment is
   * too big.
   *
   * Some of the argument processing depends on the contents of
   * bc_ctl, which will be in an undefined state if bc_init_controlinfo ()
   * failed.
   */
  if (BC_INIT_ENV_TOO_BIG == bcstatus)
    {
      act_on_init_result = fail_due_to_env_size;
    }
  else if (BC_INIT_CANNOT_ACCOMODATE_HEADROOM == bcstatus)
    {
      /* All POSIX systems are required to support ARG_MAX of at least
       * 4096.  For everything to work the total of (command line +
       * headroom + environment) must fit into this.  POSIX requires
       * that we use a headroom of 2048 bytes.  The user is in control
       * of the size of the environment.
       *
       * In general if bc_init_controlinfo () returns
       * BC_INIT_CANNOT_ACCOMODATE_HEADROOM, its caller can try again
       * with a smaller headroom.  However, in the case of xargs, this
       * would not be POSIX-compliant.
       */
      act_on_init_result = fail_due_to_env_size;
    }
  else
    {
      /* IEEE Std 1003.1, 2003 specifies that the combined argument and
       * environment list shall not exceed {ARG_MAX}-2048 bytes.  It also
       * specifies that it shall be at least LINE_MAX.
       */
      long val;
#ifdef _SC_ARG_MAX
      val = sysconf (_SC_ARG_MAX);
      if (val > 0)
	{
	  assert (val > XARGS_POSIX_HEADROOM);
	  /* Note that val can in fact be greater than ARG_MAX
	   * and bc_ctl.arg_max can also be greater than ARG_MAX.
	   */
	  bc_ctl.arg_max = smaller_of (bc_ctl.arg_max,
				       (size_t)val-XARGS_POSIX_HEADROOM);
	}
      else
	{
# if defined ARG_MAX
	  assert (ARG_MAX > XARGS_POSIX_HEADROOM);
	  bc_ctl.arg_max = smaller_of (bc_ctl.arg_max,
				       (ARG_MAX - XARGS_POSIX_HEADROOM));
# endif
	}
#else
      /* No _SC_ARG_MAX */
      assert (ARG_MAX > XARGS_POSIX_HEADROOM);
      bc_ctl.arg_max = smaller_of (bc_ctl.arg_max,
				   (ARG_MAX - XARGS_POSIX_HEADROOM));
#endif


#ifdef LINE_MAX
      /* This assertion ensures that this xargs implementation
       * conforms to the POSIX requirement that the default command
       * line length shall be at least LINE_MAX.
       */
      assert (bc_ctl.arg_max >= LINE_MAX);
#endif

      bc_ctl.exec_callback = xargs_do_exec;

      /* Start with a reasonable default size, though this can be
       * adjusted via the -s option.
       */
      bc_use_sensible_arg_max (&bc_ctl);
    }

  while ((optc = getopt_long (argc, argv, "+0a:E:e::i::I:l::L:n:prs:txP:d:",
			      longopts, &option_index)) != -1)
    {
      switch (optc)
	{
	case '0':
	  read_args = read_string;
	  input_delimiter = '\0';
	  break;

	case 'd':
	  read_args = read_string;
	  input_delimiter = get_input_delimiter (optarg);
	  break;

	case 'E':		/* POSIX */
	case 'e':		/* deprecated */
	  if (optarg && (strlen (optarg) > 0))
	    eof_str = optarg;
	  else
	    eof_str = 0;
	  break;

	case 'h':
	  usage (stdout);
	  return 0;

	case 'I':		/* POSIX */
	case 'i':		/* deprecated */
	  if (optarg)
	    bc_ctl.replace_pat = optarg;
	  else
	    bc_ctl.replace_pat = "{}";
	  /* -i excludes -n -l.  */
	  bc_ctl.args_per_exec = 0;
	  bc_ctl.lines_per_exec = 0;
	  break;

	case 'L':		/* POSIX */
	  bc_ctl.lines_per_exec = parse_num (optarg, 'L', 1L, -1L, 1);
	  /* -L excludes -i -n.  */
	  bc_ctl.args_per_exec = 0;
	  bc_ctl.replace_pat = NULL;
	  break;

	case 'l':		/* deprecated */
	  if (optarg)
	    bc_ctl.lines_per_exec = parse_num (optarg, 'l', 1L, -1L, 1);
	  else
	    bc_ctl.lines_per_exec = 1;
	  /* -l excludes -i -n.  */
	  bc_ctl.args_per_exec = 0;
	  bc_ctl.replace_pat = NULL;
	  break;

	case 'n':
	  bc_ctl.args_per_exec = parse_num (optarg, 'n', 1L, -1L, 1);
	  /* -n excludes -i -l.  */
	  bc_ctl.lines_per_exec = 0;
	  if (bc_ctl.args_per_exec == 1 && bc_ctl.replace_pat)
	    /* ignore -n1 in '-i -n1' */
	    bc_ctl.args_per_exec = 0;
	  else
	    bc_ctl.replace_pat = NULL;
	  break;

	  /* The POSIX standard specifies that it is not an error
	   * for the -s option to specify a size that the implementation
	   * cannot support - in that case, the relevant limit is used.
	   */
	case 's':
	  {
	    size_t arg_size;
	    act_on_init_result ();
	    arg_size = parse_num (optarg, 's', 1L,
				  bc_ctl.posix_arg_size_max, 0);
	    if (arg_size > bc_ctl.posix_arg_size_max)
	      {
		error (0, 0,
		       _("warning: value %ld for -s option is too large, "
			 "using %ld instead"),
		       (long) arg_size, (long) bc_ctl.posix_arg_size_max);
		arg_size = bc_ctl.posix_arg_size_max;
	      }
	    bc_ctl.arg_max = arg_size;
	  }
	  break;

	case 'S':
	  show_limits = true;
	  break;

	case 't':
	  print_command = true;
	  break;

	case 'x':
	  bc_ctl.exit_if_size_exceeded = true;
	  break;

	case 'p':
	  query_before_executing = true;
	  print_command = true;
	  break;

	case 'r':
	  always_run_command = 0;
	  break;

	case 'P':
	  /* Allow only up to MAX_PROC_MAX child processes. */
	  proc_max = parse_num (optarg, 'P', 0L, MAX_PROC_MAX, 1);
	  break;

        case 'a':
          input_file = optarg;
          break;

	case 'v':
	  display_findutils_version ("xargs");
	  return 0;

	case PROCESS_SLOT_VAR:
	  if (strchr (optarg, '='))
	    {
	      error (EXIT_FAILURE, 0,
		     _("option --%s may not be set to a value which includes `='"),
		     longopts[option_index].name);
	    }
	  slot_var_name = optarg;
	  if (0 != unsetenv (slot_var_name))
	    {
	      /* This is a fatal error, otherwise some child process
		 may not be able to guarantee that no two children
		 have the same value for this variable; see
		 set_slot_var.
	      */
	      error (EXIT_FAILURE, errno,
		     _("failed to unset environment variable %s"),
		     slot_var_name);
	    }
	  break;

	default:
	  usage (stderr);
	  return 1;
	}
    }

  if (eof_str && (read_args == read_string))
    {
      error (0, 0,
	     _("warning: the -E option has no effect if -0 or -d is used.\n"));
    }

  /* If we had deferred failing due to problems in bc_init_controlinfo (),
   * do it now.
   *
   * We issue this error message after processing command line
   * arguments so that it is possible to use "xargs --help" even if
   * the environment is too large.
   */
  act_on_init_result ();
  assert (BC_INIT_OK == bcstatus);

#ifdef SIGUSR1
#ifdef SIGUSR2
  /* Accept signals to increase or decrease the number of running
     child processes.  Do this as early as possible after setting
     proc_max.  */
  sigact.sa_handler = increment_proc_max;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  if (0 != sigaction (SIGUSR1, &sigact, (struct sigaction *)NULL))
	  error (0, errno, _("Cannot set SIGUSR1 signal handler"));

  sigact.sa_handler = decrement_proc_max;
  sigemptyset(&sigact.sa_mask);
  sigact.sa_flags = 0;
  if (0 != sigaction (SIGUSR2, &sigact, (struct sigaction *)NULL))
	  error (0, errno, _("Cannot set SIGUSR2 signal handler"));
#endif /* SIGUSR2 */
#endif /* SIGUSR1 */


  if (0 == strcmp (input_file, "-"))
    {
      input_stream = stdin;
    }
  else
    {
      keep_stdin = 1;		/* see prep_child_for_exec () */
      input_stream = fopen_cloexec_for_read_only (input_file);
      if (NULL == input_stream)
	{
	  error (EXIT_FAILURE, errno,
		 _("Cannot open input file %s"),
		 quotearg_n_style (0, locale_quoting_style, input_file));
	}
    }

  if (bc_ctl.replace_pat || bc_ctl.lines_per_exec)
    bc_ctl.exit_if_size_exceeded = true;

  if (optind == argc)
    {
      optind = 0;
      argc = 1;
      default_arglist[0] = default_cmd;
      argv = default_arglist;
    }

  if (show_limits)
    {
      fprintf (stderr,
	      _("Your environment variables take up %" PRIuMAX " bytes\n"),
	      (uintmax_t)bc_size_of_environment ());
      fprintf (stderr,
	      _("POSIX upper limit on argument length (this system): %" PRIuMAX "\n"),
	      (uintmax_t)bc_ctl.posix_arg_size_max);
      fprintf (stderr,
	      _("POSIX smallest allowable upper limit on argument length (all systems): %" PRIuMAX "\n"),
	      (uintmax_t)bc_ctl.posix_arg_size_min);
      fprintf (stderr,
	      _("Maximum length of command we could actually use: %" PRIuMAX "\n"),
	      (uintmax_t)(bc_ctl.posix_arg_size_max - bc_size_of_environment ()));
      fprintf (stderr,
	      _("Size of command buffer we are actually using: %" PRIuMAX "\n"),
	      (uintmax_t)bc_ctl.arg_max);
      fprintf (stderr,
	      _("Maximum parallelism (--max-procs must be no greater): %" PRIuMAX "\n"),
	      (uintmax_t)MAX_PROC_MAX);

      if (isatty (STDIN_FILENO))
	{
	  fprintf (stderr,
		  _("\n"
		    "Execution of xargs will continue now, and it will "
		    "try to read its input and run commands; if this is "
		    "not what you wanted to happen, please type the "
		    "end-of-file keystroke.\n"));
	  if (always_run_command)
	    {
	      fprintf (stderr,
		      _("Warning: %s will be run at least once.  "
			"If you do not want that to happen, then press "
			"the interrupt keystroke.\n"),
		      argv[optind]);
	    }
	}
    }

  linebuf = xmalloc (bc_ctl.arg_max + 1);
  bc_state.argbuf = xmalloc (bc_ctl.arg_max + 1);

  /* Make sure to listen for the kids.  */
  signal (SIGCHLD, SIG_DFL);

  if (!bc_ctl.replace_pat)
    {
      for (; optind < argc; optind++)
	bc_push_arg (&bc_ctl, &bc_state,
		     argv[optind], strlen (argv[optind]) + 1,
		     NULL, 0,
		     initial_args);
      initial_args = false;
      bc_ctl.initial_argc = bc_state.cmd_argc;
      bc_state.cmd_initial_argv_chars = bc_state.cmd_argv_chars;
      bc_ctl.initial_argc = bc_state.cmd_argc;
      /*fprintf (stderr, "setting initial_argc=%d\n", bc_state.cmd_initial_argc);*/

      while ((*read_args) () != -1)
	if (bc_ctl.lines_per_exec && lineno >= bc_ctl.lines_per_exec)
	  {
	    bc_do_exec (&bc_ctl, &bc_state);
	    lineno = 0;
	  }

      /* SYSV xargs seems to do at least one exec, even if the
         input is empty.  */
      if (bc_state.cmd_argc != bc_ctl.initial_argc
	  || (always_run_command && procs_executed==0))
	bc_do_exec (&bc_ctl, &bc_state);

    }
  else
    {
      int i, args;
      size_t *arglen = xmalloc (sizeof (size_t) * argc);

      for (i = optind; i < argc; i++)
	arglen[i] = strlen (argv[i]);
      bc_ctl.rplen = strlen (bc_ctl.replace_pat);
      while ((args = (*read_args) ()) != -1)
	{
	  size_t len = (size_t) args;
	  /* Don't do insert on the command name.  */
	  bc_clear_args (&bc_ctl, &bc_state);
	  bc_state.cmd_argv_chars = 0; /* begin at start of buffer */

	  bc_push_arg (&bc_ctl, &bc_state,
		       argv[optind], arglen[optind] + 1,
		       NULL, 0,
		       initial_args);
	  len--;
	  initial_args = false;

	  for (i = optind + 1; i < argc; i++)
	    bc_do_insert (&bc_ctl, &bc_state,
			  argv[i], arglen[i],
			  NULL, 0,
			  linebuf, len,
			  initial_args);
	  bc_do_exec (&bc_ctl, &bc_state);
	}
    }

  original_exit_value = child_error;
  return child_error;
}


/* Read a line of arguments from the input and add them to the list of
   arguments to pass to the command.  Ignore blank lines and initial blanks.
   Single and double quotes and backslashes quote metacharacters and blanks
   as they do in the shell.
   Return -1 if eof (either physical or logical) is reached,
   otherwise the length of the last string read (including the null).  */

static int
read_line (void)
{
/* States for read_line. */
  enum read_line_state
    {
      NORM = 0,
      SPACE = 1,
      QUOTE = 2,
      BACKSLASH = 3
    };
  static bool eof = false;
  /* Start out in mode SPACE to always strip leading spaces (even with -i).  */
  enum read_line_state state = SPACE; /* The type of character we last read.  */
  int prevc;			/* The previous value of c.  */
  int quotc = 0;		/* The last quote character read.  */
  int c = EOF;
  bool first = true;		/* true if reading first arg on line.  */
  bool seen_arg = false;      /* true if we have seen any arg (or part of one) yet */
  int len;
  char *p = linebuf;
  /* Including the NUL, the args must not grow past this point.  */
  char *endbuf = linebuf + bc_ctl.arg_max - bc_state.cmd_initial_argv_chars - 1;

  if (eof)
    return -1;
  while (1)
    {
      prevc = c;
      c = getc (input_stream);

      if (c == EOF)
	{
	  /* COMPAT: SYSV seems to ignore stuff on a line that
	     ends without a \n; we don't.  */
	  eof = true;
	  if (p == linebuf)
	    return -1;
	  *p++ = '\0';
	  len = p - linebuf;
	  if (state == QUOTE)
	    {
	      exec_if_possible ();
	      error (EXIT_FAILURE, 0, _("unmatched %s quote; by default quotes are special to xargs unless you use the -0 option"),
		     quotc == '"' ? _("double") : _("single"));
	    }
	  if (first && EOF_STR (linebuf))
	    return -1;
	  if (!bc_ctl.replace_pat)
	    bc_push_arg (&bc_ctl, &bc_state,
			 linebuf, len,
			 NULL, 0,
			 initial_args);
	  return len;
	}
      switch (state)
	{
	case SPACE:
	  if (ISSPACE (c))
	    continue;
	  state = NORM;
	  /* aaahhhh....  */

	case NORM:
	  if (c == '\n')
	    {
	      if (!ISBLANK (prevc))
		lineno++;	/* For -l.  */
	      if (p == linebuf)
		{
		  if (seen_arg)
		    {
		      /* An empty argument, add it to the list as normal. */
		    }
		  else
		    {
		      /* Blank line.  */
		      state = SPACE;
		      continue;
		    }
		}
	      *p++ = '\0';
	      len = p - linebuf;
	      if (EOF_STR (linebuf))
		{
		  eof = true;
		  return first ? -1 : len;
		}
	      if (!bc_ctl.replace_pat)
		bc_push_arg (&bc_ctl, &bc_state,
			     linebuf, len,
			     NULL, 0,
			     initial_args);
	      return len;
	    }
	  seen_arg = true;

	  /* POSIX: In the POSIX locale, the separators are <SPC> and
	   * <TAB>, but not <FF> or <VT>.
	   */
	  if (!bc_ctl.replace_pat && ISBLANK (c))
	    {
	      *p++ = '\0';
	      len = p - linebuf;
	      if (EOF_STR (linebuf))
		{
		  eof = true;
		  return first ? -1 : len;
		}
	      bc_push_arg (&bc_ctl, &bc_state,
			   linebuf, len,
			   NULL, 0,
			   initial_args);
	      p = linebuf;
	      state = SPACE;
	      first = false;
	      continue;
	    }
	  switch (c)
	    {
	    case '\\':
	      state = BACKSLASH;
	      continue;

	    case '\'':
	    case '"':
	      state = QUOTE;
	      quotc = c;
	      continue;
	    }
	  break;

	case QUOTE:
	  if (c == '\n')
	    {
	      exec_if_possible ();
	      error (EXIT_FAILURE, 0, _("unmatched %s quote; by default quotes are special to xargs unless you use the -0 option"),
		     quotc == '"' ? _("double") : _("single"));
	    }
	  if (c == quotc)
	    {
	      state = NORM;
	      seen_arg = true; /* Makes a difference for e.g. just '' or "" as the first arg on a line */
	      continue;
	    }
	  break;

	case BACKSLASH:
	  state = NORM;
	  break;
	}

      if ( (0 == c) && !nullwarning_given )
	{
	  /* This is just a warning message.  We only issue it once. */
	  error (0, 0,
		 _("WARNING: a NUL character occurred in the input.  "
		   "It cannot be passed through in the argument list.  "
		   "Did you mean to use the --null option?"));
	  nullwarning_given = 1;
	}

#if 1
      if (p >= endbuf)
        {
	  exec_if_possible ();
	  error (EXIT_FAILURE, 0, _("argument line too long"));
	}
      *p++ = c;
#else
      append_char_to_buf (&linebuf, &endbuf, &p, c);
#endif
    }
}

/* Read a string (terminated by the delimiter, which may be NUL) from
   the input and add it to the list of arguments to pass to the
   command.

   The return value is the length of the added argument, including its
   terminating NUL.  The added argument is always terminated by NUL,
   even if that is not the delimiter.

   If we reach physical EOF before seeing the delimiter, we treat any
   characters read as the final argument.

   If no argument was read (that is, we reached physical EOF before
   reading any characters) then -1 is returned. */
static int
read_string (void)
{
  static bool eof = false;
  int len;
  char *p = linebuf;
  /* Including the NUL, the args must not grow past this point.  */
  char *endbuf = linebuf + bc_ctl.arg_max - bc_state.cmd_initial_argv_chars - 1;

  if (eof)
    return -1;
  while (1)
    {
      int c = getc (input_stream);
      if (c == EOF)
	{
	  eof = true;
	  if (p == linebuf)
	    return -1;
	  *p++ = '\0';
	  len = p - linebuf;
	  if (!bc_ctl.replace_pat)
	    bc_push_arg (&bc_ctl, &bc_state,
			 linebuf, len,
			 NULL, 0,
			 initial_args);
	  return len;
	}
      if (c == input_delimiter)
	{
	  lineno++;		/* For -l.  */
	  *p++ = '\0';
	  len = p - linebuf;
	  if (!bc_ctl.replace_pat)
	    bc_push_arg (&bc_ctl, &bc_state,
			 linebuf, len,
			 NULL, 0,
			 initial_args);
	  return len;
	}
      if (p >= endbuf)
        {
	  exec_if_possible ();
	  error (EXIT_FAILURE, 0, _("argument line too long"));
	}
      *p++ = c;
    }
}

/* Print the arguments of the command to execute.
   If ASK is nonzero, prompt the user for a response, and
   if the user responds affirmatively, return true;
   otherwise, return false.  */

static bool
print_args (bool ask)
{
  size_t i;

  for (i = 0; i < bc_state.cmd_argc - 1; i++)
    {
      if (fprintf (stderr, "%s ", bc_state.cmd_argv[i]) < 0)
	error (EXIT_FAILURE, errno, _("Failed to write to stderr"));
    }

  if (ask)
    {
      static FILE *tty_stream;
      int c, savec;

      if (!tty_stream)
	{
	  tty_stream = fopen_cloexec_for_read_only ("/dev/tty");
	  if (!tty_stream)
	    error (EXIT_FAILURE, errno,
		   _("failed to open /dev/tty for reading"));
	}
      fputs ("?...", stderr);
      if (fflush (stderr) != 0)
	error (EXIT_FAILURE, errno, _("Failed to write to stderr"));

      c = savec = getc (tty_stream);
      while (c != EOF && c != '\n')
	c = getc (tty_stream);
      if (EOF == c)
	error (EXIT_FAILURE, errno, _("Failed to read from stdin"));
      if (savec == 'y' || savec == 'Y')
	return true;
    }
  else
    putc ('\n', stderr);

  return false;
}

/* Set SOME_ENVIRONMENT_VARIABLE=n in the environment. */
static void
set_slot_var (unsigned int n)
{
  static const char *fmt = "%u";
  int size;
  char *buf;


  /* Determine the length of the buffer we need.

     If the result would be zero-length or have length (not value) >
     INT_MAX, the assumptions we made about how snprintf behaves (or
     what UINT_MAX is) are wrong.  Hence we have a design error (not
     an environmental error).
  */
  size = snprintf (NULL, 0u, fmt, n);
  assert (size > 0);


  /* Failures here are undesirable but not fatal, since we can still
     guarantee that this child does not have a duplicate value of the
     indicated environment variable set (since the parent unset it on
     startup).
  */
  if (NULL == (buf = malloc (size+1)))
    {
      error (0, errno, _("unable to allocate memory"));
    }
  else
    {
      snprintf (buf, size+1, fmt, n);

      /* If the user doesn't want us to set the variable, there is
	 nothing to do.  However, we defer the bail-out until this
	 point in order to get better test coverage.
      */
      if (slot_var_name)
	{
	  if (setenv (slot_var_name, buf, 1) < 0)
	    {
	      error (0, errno,
		     _("failed to set environment variable %s"), slot_var_name);
	    }
	}
      free (buf);
    }
}


/* Close stdin and attach /dev/null to it.
 * This resolves Savannah bug #3992.
 */
static void
prep_child_for_exec (void)
{
  complain_about_leaky_fds ();

  /* The parent will call add_proc to allocate a slot.  We do the same in the
     child to make sure we get the same value.

     We use 0 here in order to avoid generating a data structure that appears
     to indicate that we (the child) have a child. */
  unsigned int slot = add_proc (0);
  set_slot_var (slot);

  if (!keep_stdin)
    {
      const char inputfile[] = "/dev/null";
      /* fprintf (stderr, "attaching stdin to /dev/null\n"); */

      close (0);
      if (open (inputfile, O_RDONLY) < 0)
	{
	  /* This is not entirely fatal, since
	   * executing the child with a closed
	   * stdin is almost as good as executing it
	   * with its stdin attached to /dev/null.
	   */
	  error (0, errno, "%s",
		 quotearg_n_style (0, locale_quoting_style, inputfile));
	}
    }
}


/* Execute the command that has been built in `cmd_argv'.  This may involve
   waiting for processes that were previously executed.

   There are a number of cases where we want to terminate the current (child)
   process.  We do this by calling _exit () rather than exit () in order to
   avoid the invocation of wait_for_proc_all (), which was registered by the parent
   as an atexit () function.
*/
static int
xargs_do_exec (struct buildcmd_control *ctl, void *usercontext, int argc, char **argv)
{
  pid_t child;
  int fd[2];
  int buf;
  size_t r;

  (void) ctl;
  (void) argc;
  (void) usercontext;

  if (proc_max)
    {
      while (procs_executing >= proc_max)
        {
          wait_for_proc (false, 1u);
        }
    }

  if (!query_before_executing || print_args (true))
    {
      if (!query_before_executing && print_command)
	print_args (false);

      /* Before forking, reap any already-exited child. We do this so
	 that we don't leave unreaped children around while we build a
	 new command line.  For example this command will spend most
	 of its time waiting for sufficient arguments to launch
	 another command line:

	 seq 1 1000 | fmt | while read x ; do echo $x; sleep 1 ; done |
	 ./xargs -P 200 -n 20  sh -c 'echo "$@"; sleep $((1 + $RANDOM % 5))' sleeper
      */
      wait_for_proc (false, 0u);

      if (pipe (fd))
	error (EXIT_FAILURE, errno, _("could not create pipe before fork"));
      fcntl (fd[1], F_SETFD, FD_CLOEXEC);

      /* If we run out of processes, wait for a child to return and
         try again.  */
      while ((child = fork ()) < 0 && errno == EAGAIN && procs_executing)
	wait_for_proc (false, 1u);

      switch (child)
	{
	case -1:
	  error (EXIT_FAILURE, errno, _("cannot fork"));

	case 0:		/* Child.  */
	  {
	    close (fd[0]);
	    child_error = EXIT_SUCCESS;

	    prep_child_for_exec ();

	    if (bc_args_exceed_testing_limit (argv))
	      errno = E2BIG;
	    else
	      execvp (argv[0], argv);
	    if (errno)
	      {
		/* Write errno value to parent.  We do this even if
		 * the error was not E2BIG, because we need to
		 * distinguish successful execs from unsuccessful
		 * ones.  The reason we need to do this is to know
		 * whether to reap the child here (preventing the
		 * exit status processing in wait_for_proc () from
		 * changing the value of child_error) or leave it
		 * for wait_for_proc () to handle.  We need to have
		 * wait_for_proc () handle the exit values from the
		 * utility if we run it, for POSIX compliance on the
		 * handling of exit values.
		 */
		write (fd[1], &errno, sizeof (int));
	      }

	    close (fd[1]);
	    if (E2BIG != errno)
	      {
		error (0, errno, "%s", argv[0]);
	      }
	    /* The actual value returned here should be irrelevant,
	     * because the parent will test our value of errno.
	     */
	    _exit (errno == ENOENT ? XARGS_EXIT_COMMAND_NOT_FOUND : XARGS_EXIT_COMMAND_CANNOT_BE_RUN);

	  /*NOTREACHED*/
	  } /* child */

	default:
	  {
	    /* Parent */
	    close (fd[1]);
	  }

	} /* switch (child) */
      /*fprintf (stderr, "forked child (bc_state.cmd_argc=%d) -> ", bc_state.cmd_argc);*/

      /* We use safe_read here in order to avoid an error if
	 SIGUSR[12] is handled during the read system call. */
      switch (r = safe_read (fd[0], &buf, sizeof (int)))
	{
	case SAFE_READ_ERROR:
	  {
	    close (fd[0]);
	    error (0, errno,
		   _("errno-buffer safe_read failed in xargs_do_exec "
		     "(this is probably a bug, please report it)"));
	    break;
	  }

	case sizeof (int):
	  {
	    /* Failure */
	    int childstatus;

	    close (fd[0]);

	    /* we know the child is about to exit, so wait for that.
	     * We have to do this so that wait_for_proc () does not
	     * change the value of child_error on the basis of the
	     * return value -- since in this case we did not launch
	     * the utility.
	     *
	     * We do the wait before deciding if we failed in order to
	     * avoid creating a zombie, even briefly.
	     */
	    waitpid (child, &childstatus, 0);


	    if (E2BIG == buf)
	      {
		return 0; /* Failure; caller should pass fewer args */
	      }
	    else if (ENOENT == buf)
	      {
		exit (XARGS_EXIT_COMMAND_NOT_FOUND); /* command cannot be found */
	      }
	    else
	      {
		exit (XARGS_EXIT_COMMAND_CANNOT_BE_RUN); /* command cannot be run */
	      }
	    break;
	  }

	case 0:
	  {
	    /* Failed to read data from pipe; the exec must have
	     * succeeded.  We call add_proc only in this case,
	     * because it increments procs_executing, and we only
	     * want to do that if we didn't already wait for the
	     * child.
	     */
	    add_proc (child);
	    break;
	  }
	default:
	  {
	    error (EXIT_FAILURE, errno,
		   _("read returned unexpected value %zu; "
		     "this is probably a bug, please report it"), r);
	  }
	} /* switch on bytes read */
      close (fd[0]);
    }
  return 1;			/* Success */
}

/* Execute the command if possible.  */

static void
exec_if_possible (void)
{
  if (bc_ctl.replace_pat || initial_args ||
      bc_state.cmd_argc == bc_ctl.initial_argc || bc_ctl.exit_if_size_exceeded)
    return;
  bc_do_exec (&bc_ctl, &bc_state);
}

/* Add the process with id PID to the list of processes that have
   been executed.  */

static unsigned int
add_proc (pid_t pid)
{
  unsigned int i, j;

  /* Find an empty slot.  */
  for (i = 0; i < pids_alloc && pids[i]; i++)
    ;

  /* Extend the array if we failed. */
  if (i == pids_alloc)
    {
      pids = x2nrealloc (pids, &pids_alloc, sizeof *pids);

      /* Zero out the new slots. */
      for (j=i; j<pids_alloc; ++j)
	pids[j] = (pid_t)0;
    }
  /* Verify that we are not destroying the record of some existing child. */
  assert (0 == pids[i]);

  /* Remember the child. */
  pids[i] = pid;
  procs_executing++;
  procs_executed = true;
  return i;
}


/* If ALL is true, wait for all child processes to finish;
   otherwise, wait for one child process to finish, or for another signal
   that tells us that we can run more child processes.
   Remove the processes that finish from the list of executing processes.  */

static void
wait_for_proc (bool all, unsigned int minreap)
{
  unsigned int reaped = 0;

  while (procs_executing)
    {
      unsigned int i;
      int status;
      pid_t pid;
      int wflags = 0;

      if (!all)
	{
	  if (reaped >= minreap)
	    {
	      /* We already reaped enough children.  To save system
	       * resources, reap any dead children anyway, but do not
	       * wait for any currently executing children to exit.

	       */
	      wflags = WNOHANG;
	    }
	}

      stop_waiting = 0;
      do
	{
	  /* Wait for any child.   We used to use wait () here, but it's
	   * unlikely that that offers any portability advantage over
	   * wait these days.
	   */
	  while ((pid = waitpid (-1, &status, wflags)) == (pid_t) -1)
	    {
	      if (errno != EINTR)
		error (EXIT_FAILURE, errno,
		       _("error waiting for child process"));

	      if (stop_waiting && !all)
		{
		  /* Receipt of SIGUSR1 gave us an extra slot and we
		   * don't need to wait for all processes to finish.
		   * We can stop reaping now, but in any case check for
		   * further dead children without waiting for another
		   * to exit.
		   */
		  wflags = WNOHANG;
		}
	    }

	  /* Find the entry in `pids' for the child process
	     that exited.  */
	  if (pid)
	    {
	      for (i = 0; i < pids_alloc && pid != pids[i]; i++)
		;
	    }
	}
      while (pid && i == pids_alloc);	/* A child died that we didn't start? */

      if (!pid)
	{
	  if (!(wflags & WNOHANG))
	    {
	      /* Nothing remained to be reaped.  This should not
	       * happen, because procs_executing should contain the
	       * number of child processes still executing, so the
	       * loop should have terminated.
	       */
	      error (0, 0, _("WARNING: Lost track of %lu child processes"),
		     procs_executing);
	    }
	  else
	    {
	      /* Children are (probably) executing but are not ready
	       * to be reaped at the moment.
	       */
	    }
	  break;
	}

      /* Remove the child from the list.  */
      pids[i] = 0;
      procs_executing--;
      reaped++;

      if (WEXITSTATUS (status) == CHILD_EXIT_PLEASE_STOP_IMMEDIATELY)
	error (XARGS_EXIT_CLIENT_EXIT_255, 0,
	       _("%s: exited with status 255; aborting"), bc_state.cmd_argv[0]);
      if (WIFSTOPPED (status))
	error (XARGS_EXIT_CLIENT_FATAL_SIG, 0,
	       _("%s: stopped by signal %d"), bc_state.cmd_argv[0], WSTOPSIG (status));
      if (WIFSIGNALED (status))
	error (XARGS_EXIT_CLIENT_FATAL_SIG, 0,
	       _("%s: terminated by signal %d"), bc_state.cmd_argv[0], WTERMSIG (status));
      if (WEXITSTATUS (status) != 0)
	child_error = XARGS_EXIT_CLIENT_EXIT_NONZERO;
    }
}

/* Wait for all child processes to finish.  */

static void
wait_for_proc_all (void)
{
  static bool waiting = false;

  /* This function was registered by the original, parent, process.
   * The child processes must not call exit () to terminate, since this
   * will mean that their exit status will be inappropriately changed.
   * Instead the child processes should call _exit ().  If someone
   * forgets this rule, you may see the following assert () fail.
   */
  assert (getpid () == parent);

  if (waiting)
    return;

  waiting = true;
  wait_for_proc (true, 0u);
  waiting = false;

  if (original_exit_value != child_error)
    {
      /* wait_for_proc () changed the value of child_error ().  This
       * function is registered via atexit (), and so may have been
       * called from exit ().  We now know that the original value
       * passed to exit () is no longer the exit status we require.
       * The POSIX standard states that the behaviour if exit () is
       * called more than once is undefined.  Therefore we now have to
       * exit with _exit () instead of exit ().
       */
      _exit (child_error);
    }

}


/* Increment or decrement the number of processes we can start simultaneously,
   when we receive a signal from the outside world.

   We must take special care around proc_max == 0 (unlimited children),
   proc_max == 1 (don't decrement to zero).  */
static void
increment_proc_max (int ignore)
{
        (void) ignore;
	/* If user increments from 0 to 1, we'll take it and serialize. */
	if (proc_max < MAX_PROC_MAX)
	  proc_max++;
	/* If we're waiting for a process to die before doing something,
	   no need to wait any more. */
	stop_waiting = 1;
}

static void
decrement_proc_max (int ignore)
{
        (void) ignore;
	if (proc_max > 1)
		proc_max--;
}


/* Return the value of the number represented in STR.
   OPTION is the command line option to which STR is the argument.
   If the value does not fall within the boundaries MIN and MAX,
   Print an error message mentioning OPTION.  If FATAL is true,
   we also exit. */

static long
parse_num (char *str, int option, long int min, long int max, int fatal)
{
  char *eptr;
  long val;

  val = strtol (str, &eptr, 10);
  if (eptr == str || *eptr)
    {
      fprintf (stderr, _("%s: invalid number for -%c option\n"),
	       program_name, option);
      usage (stderr);
      exit (EXIT_FAILURE);
    }
  else if (val < min)
    {
      fprintf (stderr, _("%s: value for -%c option should be >= %ld\n"),
	       program_name, option, min);
      if (fatal)
	{
	  usage (stderr);
	  exit (EXIT_FAILURE);
	}
      else
	{
	  val = min;
	}
    }
  else if (max >= 0 && val > max)
    {
      fprintf (stderr, _("%s: value for -%c option should be <= %ld\n"),
	       program_name, option, max);
      if (fatal)
	{
	  usage (stderr);
	  exit (EXIT_FAILURE);
	}
      else
	{
	  val = max;
	}
    }
  return val;
}

static void
usage (FILE *stream)
{
  fprintf (stream,
           _("Usage: %s [OPTION]... COMMAND [INITIAL-ARGS]...\n"),
           program_name);

#define HTL(t) fputs (t, stream);

  HTL (_("Run COMMAND with arguments INITIAL-ARGS and more arguments read from input.\n"
         "\n"));
  HTL (_("Mandatory and optional arguments to long options are also\n"
         "mandatory or optional for the corresponding short option.\n"));
  HTL (_("  -0, --null                   items are separated by a null, not whitespace;\n"
         "                                 disables quote and backslash processing and\n"
	 "                                 logical EOF processing\n"));
  HTL (_("  -a, --arg-file=FILE          read arguments from FILE, not standard input\n"));
  HTL (_("  -d, --delimiter=CHARACTER    items in input stream are separated by CHARACTER,\n"
         "                                 not by whitespace; disables quote and backslash\n"
         "                                 processing and logical EOF processing\n"));
  HTL (_("  -E END                       set logical EOF string; if END occurs as a line\n"
	 "                                 of input, the rest of the input is ignored\n"
	 "                                 (ignored if -0 or -d was specified)\n"));
  HTL (_("  -e, --eof[=END]              equivalent to -E END if END is specified;\n"
         "                                 otherwise, there is no end-of-file string\n"));
  HTL (_("  -I R                         same as --replace=R\n"));
  HTL (_("  -i, --replace[=R]            replace R in INITIAL-ARGS with names read\n"
         "                                 from standard input; if R is unspecified,\n"
         "                                 assume {}\n"));
  HTL (_("  -L, --max-lines=MAX-LINES    use at most MAX-LINES non-blank input lines per\n"
         "                                 command line\n"));
  HTL (_("  -l[MAX-LINES]                similar to -L but defaults to at most one non-\n"
         "                                 blank input line if MAX-LINES is not specified\n"));
  HTL (_("  -n, --max-args=MAX-ARGS      use at most MAX-ARGS arguments per command line\n"));
  HTL (_("  -P, --max-procs=MAX-PROCS    run at most MAX-PROCS processes at a time\n"));
  HTL (_("  -p, --interactive            prompt before running commands\n"));
  HTL (_("      --process-slot-var=VAR   set environment variable VAR in child processes\n"));
  HTL (_("  -r, --no-run-if-empty        if there are no arguments, then do not run COMMAND;\n"
         "                                 if this option is not given, COMMAND will be\n"
         "                                 run at least once\n"));
  HTL (_("  -s, --max-chars=MAX-CHARS    limit length of command line to MAX-CHARS\n"));
  HTL (_("      --show-limits            show limits on command-line length\n"));
  HTL (_("  -t, --verbose                print commands before executing them\n"));
  HTL (_("  -x, --exit                   exit if the size (see -s) is exceeded\n"));

  HTL (_("      --help                   display this help and exit\n"));
  HTL (_("      --version                output version information and exit\n"));
  HTL (_("\n"
         "Report bugs to <bug-findutils@gnu.org>.\n"));
}
