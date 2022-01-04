/* exec.c -- Implementation of -exec, -execdir, -ok, -okdir.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000, 2003,
                 2004, 2005, 2006, 2007, 2008, 2009,
                 2010 Free Software Foundation, Inc.

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
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>


/* gnulib headers */
#include "cloexec.h"
#include "dirname.h"
#include "error.h"
#include "gettext.h"
#include "save-cwd.h"
#include "xalloc.h"

/* findutils headers */
#include "buildcmd.h"
#include "defs.h"
#include "fdleak.h"

#if ENABLE_NLS
# include <libintl.h>
# define _(Text) gettext (Text)
#else
# define _(Text) Text
#endif
#ifdef gettext_noop
# define N_(String) gettext_noop (String)
#else
/* See locate.c for explanation as to why not use (String) */
# define N_(String) String
#endif


/* Initialize exec->wd_for_exec.

   We save in exec->wd_for_exec the directory whose path relative to
   cwd_df is dir.
 */
static bool
initialize_wd_for_exec (struct exec_val *execp, int cwd_fd, const char *dir)
{
  execp->wd_for_exec = xmalloc (sizeof (*execp->wd_for_exec));
  execp->wd_for_exec->name = NULL;
  execp->wd_for_exec->desc = openat (cwd_fd, dir, O_RDONLY);
  if (execp->wd_for_exec->desc < 0)
    return false;
  set_cloexec_flag (execp->wd_for_exec->desc, true);
  return true;
}


static bool
record_exec_dir (struct exec_val *execp)
{
  if (!execp->state.todo)
    {
      /* working directory not already known, so must be a *dir variant,
	 and this must be the first arg we added.   However, this may
	 be -execdir foo {} \; (i.e. not multiple).  */
      assert (!execp->state.todo);

      /* Record the WD. If we're using -L or fts chooses to do so for
	 any other reason, state.cwd_dir_fd may in fact not be the
	 directory containing the target file.  When this happens,
	 rel_path will contain directory components (since it is the
	 path from state.cwd_dir_fd to the target file).

	 We deal with this by extracting any directory part and using
	 that to adjust what goes into execp->wd_for_exec.
      */
      if (strchr (state.rel_pathname, '/'))
	{
	  char *dir = mdir_name (state.rel_pathname);
	  bool result = initialize_wd_for_exec (execp, state.cwd_dir_fd, dir);
	  free (dir);
	  return result;
	}
      else
	{
	  return initialize_wd_for_exec (execp, state.cwd_dir_fd, ".");
	}
    }
  return true;
}




bool
impl_pred_exec (const char *pathname,
                struct stat *stat_buf,
                struct predicate *pred_ptr)
{
  struct exec_val *execp = &pred_ptr->args.exec_vec;
  char *buf = NULL;
  const char *target;
  bool result;
  const bool local = is_exec_in_local_dir (pred_ptr->pred_func);
  char *prefix;
  size_t pfxlen;

  (void) stat_buf;
  if (local)
    {
      /* For -execdir/-okdir predicates, the parser did not fill in
         the wd_for_exec member of struct exec_val.  So for those
         predicates, we do so now.
      */
      if (!record_exec_dir (execp))
        {
          error (EXIT_FAILURE, errno,
                 _("Failed to save working directory in order to "
                   "run a command on %s"),
                 safely_quote_err_filename (0, pathname));
          /*NOTREACHED*/
        }
      target = buf = base_name (state.rel_pathname);
      if ('/' == target[0])
        {
          /* find / execdir ls -d {} \; */
          prefix = NULL;
          pfxlen = 0;
        }
      else
        {
          prefix = "./";
          pfxlen = 2u;
        }
    }
  else
    {
      /* For the others (-exec, -ok), the parser should
         have set wd_for_exec to initial_wd, indicating
         that the exec should take place from find's initial
         working directory.
      */
      assert (execp->wd_for_exec == initial_wd);
      target = pathname;
      prefix = NULL;
      pfxlen = 0u;
    }

  if (execp->multiple)
    {
      /* Push the argument onto the current list.
       * The command may or may not be run at this point,
       * depending on the command line length limits.
       */
      bc_push_arg (&execp->ctl,
                   &execp->state,
                   target, strlen (target)+1,
                   prefix, pfxlen,
                   0);

      /* remember that there are pending execdirs. */
      if (execp->state.todo)
        state.execdirs_outstanding = true;

      /* POSIX: If the primary expression is punctuated by a plus
       * sign, the primary shall always evaluate as true
       */
      result = true;
    }
  else
    {
      int i;

      for (i=0; i<execp->num_args; ++i)
        {
          bc_do_insert (&execp->ctl,
                        &execp->state,
                        execp->replace_vec[i],
                        strlen (execp->replace_vec[i]),
                        prefix, pfxlen,
                        target, strlen (target),
                        0);
        }

      /* Actually invoke the command. */
      bc_do_exec (&execp->ctl, &execp->state);
      if (WIFEXITED(execp->last_child_status))
        {
          if (0 == WEXITSTATUS(execp->last_child_status))
            result = true;        /* The child succeeded. */
          else
            result = false;
        }
      else
        {
          result = false;
        }
      if (local)
        free_cwd (execp->wd_for_exec);
    }
  if (buf)
    {
      assert (local);
      free (buf);
    }
  return result;
}



/*  1) fork to get a child; parent remembers the child pid
    2) child execs the command requested
    3) parent waits for child; checks for proper pid of child

    Possible returns:

    ret		errno	status(h)   status(l)

    pid		x	signal#	    0177	stopped
    pid		x	exit arg    0		term by _exit
    pid		x	0	    signal #	term by signal
    -1		EINTR				parent got signal
    -1		other				some other kind of error

    Return true only if the pid matches, status(l) is
    zero, and the exit arg (status high) is 0.
    Otherwise return false, possibly printing an error message. */
static bool
prep_child_for_exec (bool close_stdin, const struct saved_cwd *wd)
{
  bool ok = true;
  if (close_stdin)
    {
      const char inputfile[] = "/dev/null";

      if (close (0) < 0)
	{
	  error (0, errno, _("Cannot close standard input"));
	  ok = false;
	}
      else
	{
	  if (open (inputfile, O_RDONLY
#if defined O_LARGEFILE
		   |O_LARGEFILE
#endif
		   ) < 0)
	    {
	      /* This is not entirely fatal, since
	       * executing the child with a closed
	       * stdin is almost as good as executing it
	       * with its stdin attached to /dev/null.
	       */
	      error (0, errno, "%s", safely_quote_err_filename (0, inputfile));
	      /* do not set ok=false, it is OK to continue anyway. */
	    }
	}
    }

  /* Even if DebugSearch is set, don't announce our change of
   * directory, since we're not going to emit a subsequent
   * announcement of a call to stat() anyway, as we're about to exec
   * something.
   */
  if (0 != restore_cwd (wd))
    {
      error (0, errno, _("Failed to change directory%s%s"),
	     (wd->desc < 0 && wd->name) ? ": " : "",
	     (wd->desc < 0 && wd->name) ? wd->name : "");
      ok = false;
    }
  return ok;
}


int
launch (struct buildcmd_control *ctl, void *usercontext, int argc, char **argv)
{
  pid_t child_pid;
  static int first_time = 1;
  struct exec_val *execp = usercontext;

  /* Make sure output of command doesn't get mixed with find output. */
  fflush (stdout);
  fflush (stderr);

  /* Make sure to listen for the kids.  */
  if (first_time)
    {
      first_time = 0;
      signal (SIGCHLD, SIG_DFL);
    }

  child_pid = fork ();
  if (child_pid == -1)
    error (EXIT_FAILURE, errno, _("cannot fork"));
  if (child_pid == 0)
    {
      /* We are the child. */
      assert (NULL != execp->wd_for_exec);
      if (!prep_child_for_exec (execp->close_stdin, execp->wd_for_exec))
	{
	  _exit (1);
	}
      else
	{
	  if (fd_leak_check_is_enabled ())
	    {
	      complain_about_leaky_fds ();
	    }
	}

      if (bc_args_exceed_testing_limit (argv))
	errno = E2BIG;
      else
	execvp (argv[0], argv);
      /* TODO: use a pipe to pass back the errno value, like xargs does */
      error (0, errno, "%s",
	     safely_quote_err_filename (0, argv[0]));
      _exit (1);
    }

  while (waitpid (child_pid, &(execp->last_child_status), 0) == (pid_t) -1)
    {
      if (errno != EINTR)
	{
	  error (0, errno, _("error waiting for %s"),
		 safely_quote_err_filename (0, argv[0]));
	  state.exit_status = 1;
	  return 0;		/* FAIL */
	}
    }

  if (WIFSIGNALED (execp->last_child_status))
    {
      error (0, 0, _("%s terminated by signal %d"),
	     quotearg_n_style (0, options.err_quoting_style, argv[0]),
	     WTERMSIG (execp->last_child_status));

      if (execp->multiple)
	{
	  /* -exec   \; just returns false if the invoked command fails.
	   * -exec {} + returns true if the invoked command fails, but
	   *            sets the program exit status.
	   */
	  state.exit_status = 1;
	}

      return 1;			/* OK */
    }

  if (0 == WEXITSTATUS (execp->last_child_status))
    {
      return 1;			/* OK */
    }
  else
    {
      if (execp->multiple)
	{
	  /* -exec   \; just returns false if the invoked command fails.
	   * -exec {} + returns true if the invoked command fails, but
	   *            sets the program exit status.
	   */
	  state.exit_status = 1;
	}
      /* The child failed, but this is the exec callback.  We
       * don't want to run the child again in this case anwyay.
       */
      return 1;			/* FAIL (but don't try again) */
    }

}
