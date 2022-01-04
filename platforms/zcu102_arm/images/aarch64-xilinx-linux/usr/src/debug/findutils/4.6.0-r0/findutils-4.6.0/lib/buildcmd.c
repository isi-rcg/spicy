/* buildcmd.c -- build command lines from a list of arguments.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000, 2003, 2005, 2006,
   2007, 2008, 2010, 2011 Free Software Foundation, Inc.

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
/* config.h must be included first. */
#include <config.h>

/* system headers. */
#include <assert.h>
#include <errno.h>
#include <error.h>
#include <limits.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#ifndef _POSIX_SOURCE
# include <sys/param.h>
#endif
#include <unistd.h>
#include <wchar.h>
#include <xalloc.h>

/* gnulib headers. */
#include "gettext.h"
#include "xstrtol.h"

/* find headers. */
#include "buildcmd.h"

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define _(Text) Text
#define textdomain(Domain)
#define bindtextdomain(Package, Directory)
#endif
#ifdef gettext_noop
# define N_(String) gettext_noop (String)
#else
/* See locate.c for explanation as to why not use (String) */
# define N_(String) String
#endif

/* COMPAT:  SYSV version defaults size (and has a max value of) to 470.
   We try to make it as large as possible.  See bc_get_arg_max() below. */
#if defined NCARGS && !defined ARG_MAX
/* We include sys/param.h in order to detect this case. */
#error "You have an unusual system.  Once you remove this error message from buildcmd.c, it should work, but please make sure that DejaGnu is installed on your system and that 'make check' passes before using the findutils programs.  Please mail bug-findutils@gnu.org to tell us about your system."
#define ARG_MAX NCARGS
#endif


static const char *special_terminating_arg = "do_not_care";



/* Add a terminator to the argument list. */
static void
bc_args_complete (struct buildcmd_control *ctl,
		  struct buildcmd_state *state)
{
  bc_push_arg (ctl, state, special_terminating_arg, 0, NULL, 0, 0);
}


/* Replace all instances of `replace_pat' in ARG with `linebuf',
   and add the resulting string to the list of arguments for the command
   to execute.
   ARGLEN is the length of ARG, not including the null.
   LBLEN is the length of LINEBUF, not including the null.
   PFXLEN is the length of PREFIX.  Substitution is not performed on
   the prefix.   The prefix is used if the argument contains replace_pat.

   COMPAT: insertions on the SYSV version are limited to 255 chars per line,
   and a max of 5 occurrences of replace_pat in the initial-arguments.
   Those restrictions do not exist here.  */

void
bc_do_insert (struct buildcmd_control *ctl,
              struct buildcmd_state *state,
              char *arg, size_t arglen,
              const char *prefix, size_t pfxlen,
              const char *linebuf, size_t lblen,
              int initial_args)
{
  /* Temporary copy of each arg with the replace pattern replaced by the
     real arg.  */
  static char *insertbuf;
  char *p;
  size_t bytes_left = ctl->arg_max - 1;    /* Bytes left on the command line.  */

  /* XXX: on systems lacking an upper limit for exec args, ctl->arg_max
   *      may have been set to LONG_MAX (see bc_get_arg_max()).  Hence
   *      this xmalloc call may be a bad idea, especially since we are
   *      adding 1 to it...
   */
  if (!insertbuf)
    insertbuf = xmalloc (ctl->arg_max + 1);
  p = insertbuf;

  do
    {
      size_t len;               /* Length in ARG before `replace_pat'.  */
      char *s = mbsstr (arg, ctl->replace_pat);
      if (s)
        {
          len = s - arg;
        }
      else
        {
          len = arglen;
        }

      if (bytes_left <= len)
        break;
      else
	bytes_left -= len;

      strncpy (p, arg, len);
      p += len;
      arg += len;
      arglen -= len;

      if (s)
        {
	  if (bytes_left <= (lblen + pfxlen))
	    break;
	  else
	    bytes_left -= (lblen + pfxlen);

	  if (prefix)
	    {
	      strcpy (p, prefix);
	      p += pfxlen;
	    }
          strcpy (p, linebuf);
          p += lblen;

          arg += ctl->rplen;
          arglen -= ctl->rplen;
        }
    }
  while (*arg);
  if (*arg)
    error (EXIT_FAILURE, 0, _("command too long"));
  *p++ = '\0';

  bc_push_arg (ctl, state,
	       insertbuf, p - insertbuf,
               NULL, 0,
               initial_args);
}


/* Update our best guess as to how many arguments we should pass to the next
 * invocation of the command.
 */
static size_t
update_limit (struct buildcmd_control *ctl,
	      struct buildcmd_state *state,
	      bool success,
	      size_t limit)
{
  if (success)
    {
      if (limit > state->largest_successful_arg_count)
	state->largest_successful_arg_count = limit;
    }
  else
    {
      if (limit < state->smallest_failed_arg_count
	  || (0 == state->smallest_failed_arg_count))
	state->smallest_failed_arg_count = limit;
    }

  if (0 == (state->largest_successful_arg_count)
      || (state->smallest_failed_arg_count <= state->largest_successful_arg_count))
    {
      /* No success yet, or running on a system which has
	 limits on total argv length, but not arg count. */
      if (success)
	{
	  if (limit < SIZE_MAX)
	    ++limit;
	}
      else
	{
	  limit /= 2;
	}
    }
  else  /* We can use bisection. */
    {
      const size_t shift = (state->smallest_failed_arg_count
			  - state->largest_successful_arg_count) / 2;
      if (success)
	{
	  if (shift)
	    limit += shift;
	  else
	    ++limit;
	}
      else
	{
	  if (shift)
	    limit -= shift;
	  else
	    --limit;
	}
    }

  /* Make sure the returned value is such that progress is
   * actually possible.
   */
  if (ctl->initial_argc && (limit <= ctl->initial_argc + 1u))
    limit = ctl->initial_argc + 1u;
  if (0 == limit)
    limit = 1u;

  return limit;
}


/* Copy some of the program arguments into an argv list.   Copy all the
 * initial arguments, plus up to LIMIT additional arguments.
 */
static size_t
copy_args (struct buildcmd_control *ctl,
	   struct buildcmd_state *state,
	   char** working_args, size_t limit, size_t done)
{
  size_t dst_pos = 0;
  size_t src_pos = 0;

  while (src_pos < ctl->initial_argc)
    {
      working_args[dst_pos++] = state->cmd_argv[src_pos++];
    }
  src_pos += done;
  while (src_pos < state->cmd_argc && dst_pos < limit)
    {
      working_args[dst_pos++] = state->cmd_argv[src_pos++];
    }
  assert (dst_pos >= ctl->initial_argc);
  working_args[dst_pos] = NULL;
  return dst_pos;
}




/* Execute the program with the currently-built list of arguments. */
void
bc_do_exec (struct buildcmd_control *ctl,
	    struct buildcmd_state *state)
{
    char** working_args;
    size_t limit, done;

    /* Terminate the args. */
    bc_args_complete (ctl, state);
    /* Verify that the argument list is terminated. */
    assert (state->cmd_argc > 0);
    assert (state->cmd_argv[state->cmd_argc-1] == NULL);

    working_args = xmalloc ((1+state->cmd_argc) * sizeof (char*));
    done = 0;
    limit = state->cmd_argc;

    do
      {
	const size_t dst_pos = copy_args (ctl, state, working_args,
					  limit, done);
	if (ctl->exec_callback (ctl, state->usercontext, dst_pos, working_args))
	  {
	    limit = update_limit (ctl, state, true, limit);
	    done += (dst_pos - ctl->initial_argc);
	  }
	else  /* got E2BIG, adjust arguments */
	  {
	    if (limit <= ctl->initial_argc + 1)
	      {
		/* No room to reduce the length of the argument list.
		   Issue an error message and give up. */
		error (EXIT_FAILURE, 0,
		       _("can't call exec() due to argument size restrictions"));
	      }
	    else
	      {
		/* Try fewer arguments. */
		limit = update_limit (ctl, state, false, limit);
	      }
	  }
      }
    while ((done + 1) < (state->cmd_argc - ctl->initial_argc));
    /* (state->cmd_argc - ctl->initial_argc) includes the terminating NULL,
     * which is why we add 1 to done in the test above. */

    free (working_args);
    bc_clear_args (ctl, state);
}


/* Return nonzero if there would not be enough room for an additional
 * argument.  We check the total number of arguments only, not the space
 * occupied by those arguments.
 *
 * If we return zero, there still may not be enough room for the next
 * argument, depending on its length.
 */
static int
bc_argc_limit_reached (int initial_args,
		       const struct buildcmd_control *ctl,
		       struct buildcmd_state *state)
{
  /* Check to see if we about to exceed a limit set by xargs' -n option */
  if (!initial_args && ctl->args_per_exec &&
      ( (state->cmd_argc - ctl->initial_argc) == ctl->args_per_exec))
    return 1;

  /* We deliberately use an equality test here rather than >= in order
   * to force a software failure if the code is modified in such a way
   * that it fails to call this function for every new argument.
   */
  return state->cmd_argc == ctl->max_arg_count;
}


/* Add ARG to the end of the list of arguments `cmd_argv' to pass
   to the command.
   LEN is the length of ARG, including the terminating null.
   If this brings the list up to its maximum size, execute the command.
*/
void
bc_push_arg (struct buildcmd_control *ctl,
             struct buildcmd_state *state,
             const char *arg, size_t len,
             const char *prefix, size_t pfxlen,
             int initial_args)
{
  const int terminate = (arg == special_terminating_arg);

  assert (arg != NULL);

  if (!initial_args)
    {
      state->todo = 1;
    }

  if (!terminate)
    {
      if (state->cmd_argv_chars + len + pfxlen > ctl->arg_max)
        {
          if (initial_args || state->cmd_argc == ctl->initial_argc)
            error (EXIT_FAILURE, 0,
		   _("cannot fit single argument within argument list size limit"));

          /* xargs option -i (replace_pat) implies -x (exit_if_size_exceeded) */
          if (ctl->replace_pat
              || (ctl->exit_if_size_exceeded &&
                  (ctl->lines_per_exec || ctl->args_per_exec)))
            error (EXIT_FAILURE, 0, _("argument list too long"));
            bc_do_exec (ctl, state);
        }
      if (bc_argc_limit_reached (initial_args, ctl, state))
            bc_do_exec (ctl, state);
    }

  if (state->cmd_argc >= state->cmd_argv_alloc)
    {
      /* XXX: we could use extendbuf() here. */
      if (!state->cmd_argv)
        {
          state->cmd_argv_alloc = 64;
          state->cmd_argv = xmalloc (sizeof (char *) * state->cmd_argv_alloc);
        }
      else
        {
          state->cmd_argv_alloc *= 2;
          state->cmd_argv = xrealloc (state->cmd_argv,
				      sizeof (char *) * state->cmd_argv_alloc);
        }
    }

  if (terminate)
    state->cmd_argv[state->cmd_argc++] = NULL;
  else
    {
      state->cmd_argv[state->cmd_argc++] = state->argbuf + state->cmd_argv_chars;
      if (prefix)
        {
          strcpy (state->argbuf + state->cmd_argv_chars, prefix);
          state->cmd_argv_chars += pfxlen;
        }

      strcpy (state->argbuf + state->cmd_argv_chars, arg);
      state->cmd_argv_chars += len;

      /* If we have now collected enough arguments,
       * do the exec immediately.
       */
      if (bc_argc_limit_reached (initial_args, ctl, state))
	{
	  bc_do_exec (ctl, state);
	}
    }

  /* If this is an initial argument, set the high-water mark. */
  if (initial_args)
    {
      state->cmd_initial_argv_chars = state->cmd_argv_chars;
    }
}


size_t
bc_get_arg_max (void)
{
  long val;

  /* We may resort to using LONG_MAX, so check it fits. */
  /* XXX: better to do a compile-time check */
  assert ( (~(size_t)0) >= LONG_MAX);

#ifdef _SC_ARG_MAX
  val = sysconf (_SC_ARG_MAX);
#else
  val = -1;
#endif

  if (val > 0)
    return val;

  /* either _SC_ARG_MAX was not available or
   * there is no particular limit.
   */
#ifdef ARG_MAX
  val = ARG_MAX;
  if (val > 0)
    return val;
#endif

  /* The value returned by this function bounds the
   * value applied as the ceiling for the -s option.
   * Hence it the system won't tell us what its limit
   * is, we allow the user to specify more or less
   * whatever value they like.
   */
  return LONG_MAX;
}


static int
cb_exec_noop (struct buildcmd_control * ctl,
	      void *usercontext,
	      int argc,
	      char **argv)
{
  /* does nothing. */
  (void) ctl;
  (void) usercontext;
  (void) argc;
  (void) argv;

  return 0;
}


/* Return how much of ARG_MAX is used by the environment.  */
size_t
bc_size_of_environment (void)
{
  size_t len = 0u;
  char **envp = environ;

  while (*envp)
    len += strlen (*envp++) + 1;

  return len;
}


enum BC_INIT_STATUS
bc_init_controlinfo (struct buildcmd_control *ctl,
		     size_t headroom)
{
  size_t size_of_environment = bc_size_of_environment ();

  /* POSIX requires that _POSIX_ARG_MAX is 4096.  That is the lowest
   * possible value for ARG_MAX on a POSIX compliant system.  See
   * http://www.opengroup.org/onlinepubs/009695399/basedefs/limits.h.html
   */
  ctl->posix_arg_size_min = _POSIX_ARG_MAX;
  ctl->posix_arg_size_max = bc_get_arg_max ();

  ctl->exit_if_size_exceeded = 0;

  /* Take the size of the environment into account.  */
  if (size_of_environment > ctl->posix_arg_size_max)
    {
      return BC_INIT_ENV_TOO_BIG;
    }
  else if ((headroom + size_of_environment) >= ctl->posix_arg_size_max)
    {
      /* POSIX.2 requires xargs to subtract 2048, but ARG_MAX is
       * guaranteed to be at least 4096.  Although xargs could use an
       * assertion here, we use a runtime check which returns an error
       * code, because our caller may not be xargs.
       */
      return BC_INIT_CANNOT_ACCOMODATE_HEADROOM;
    }
  else
    {
      ctl->posix_arg_size_max -= size_of_environment;
      ctl->posix_arg_size_max -= headroom;
    }

  /* need to subtract 2 on the following line - for Linux/PPC */
  ctl->max_arg_count = (ctl->posix_arg_size_max / sizeof (char*)) - 2u;
  assert (ctl->max_arg_count > 0);
  ctl->rplen = 0u;
  ctl->replace_pat = NULL;
  ctl->initial_argc = 0;
  ctl->exec_callback = cb_exec_noop;
  ctl->lines_per_exec = 0;
  ctl->args_per_exec = 0;

  /* Set the initial value of arg_max to the largest value we can
   * tolerate.
   */
  ctl->arg_max = ctl->posix_arg_size_max;

  return BC_INIT_OK;
}

void
bc_use_sensible_arg_max (struct buildcmd_control *ctl)
{
#ifdef DEFAULT_ARG_SIZE
  enum { arg_size = DEFAULT_ARG_SIZE };
#else
  enum { arg_size = (128u * 1024u) };
#endif

  /* Check against the upper and lower limits. */
  if (arg_size > ctl->posix_arg_size_max)
    ctl->arg_max = ctl->posix_arg_size_max;
  else if (arg_size < ctl->posix_arg_size_min)
    ctl->arg_max = ctl->posix_arg_size_min;
  else
    ctl->arg_max = arg_size;
}




void
bc_init_state (const struct buildcmd_control *ctl,
	       struct buildcmd_state *state,
	       void *context)
{
  state->cmd_argc = 0;
  state->cmd_argv_chars = 0;
  state->cmd_argv = NULL;
  state->cmd_argv_alloc = 0;
  state->largest_successful_arg_count = 0;
  state->smallest_failed_arg_count = 0;

  /* XXX: the following memory allocation is inadvisable on systems
   * with no ARG_MAX, because ctl->arg_max may actually be close to
   * LONG_MAX.   Adding one to it is safe though because earlier we
   * subtracted 2048.
   */
  assert (ctl->arg_max <= (LONG_MAX - 2048L));
  state->argbuf = xmalloc (ctl->arg_max + 1u);

  state->cmd_argv_chars = state->cmd_initial_argv_chars = 0;
  state->todo = 0;
  state->dir_fd = -1;
  state->usercontext = context;
}

void
bc_clear_args (const struct buildcmd_control *ctl,
	       struct buildcmd_state *state)
{
  state->cmd_argc = ctl->initial_argc;
  state->cmd_argv_chars = state->cmd_initial_argv_chars;
  state->todo = 0;
  state->dir_fd = -1;
}


/* Return nonzero if the value stored in the environment variable ENV_VAR_NAME
 * exceeds QUANTITY.
 */
static int
exceeds (const char *env_var_name, size_t quantity)
{
  const char *val = getenv (env_var_name);
  if (val)
    {
      char *tmp;
      unsigned long limit;

      if (xstrtoul (val, &tmp, 10, &limit, NULL) == LONGINT_OK)
	{
	  if (quantity > limit)
	    return 1;
	}
      else
	{
	  error (EXIT_FAILURE, errno,
		 _("Environment variable %s is not set to a "
		   "valid decimal number"),
		 env_var_name);
	  return 0;
	}
    }
  return 0;
}

/* Return nonzero if the indicated argument list exceeds a testing limit.
 * NOTE: argv could be declared 'const char *const *argv', but it works as
 * expected only with C++ compilers <http://c-faq.com/ansi/constmismatch.html>.
 */
bool
bc_args_exceed_testing_limit (char **argv)
{
  size_t chars, args;

  for (chars=args=0; *argv; ++argv)
    {
      ++args;
      chars += strlen(*argv);
    }

  return (exceeds ("__GNU_FINDUTILS_EXEC_ARG_COUNT_LIMIT", args) ||
	  exceeds ("__GNU_FINDUTILS_EXEC_ARG_LENGTH_LIMIT", chars));
}
