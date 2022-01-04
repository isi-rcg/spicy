/* util.c -- functions for initializing new tree elements, and other things.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000, 2003, 2004, 2005,
   2008, 2009, 2010, 2011 Free Software Foundation, Inc.

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

/* config.h must always come first. */
#include <config.h>

/* system headers. */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <string.h>
#include <sys/stat.h> /* for fstatat() */
#include <sys/time.h>
#include <sys/utsname.h>

/* gnulib headers. */
#include "error.h"
#include "fdleak.h"
#include "gettext.h"
#include "progname.h"
#include "quotearg.h"
#include "save-cwd.h"
#include "timespec.h"
#include "xalloc.h"

/* find headers. */
#include "defs.h"
#include "dircallback.h"


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


struct debug_option_assoc
{
  char *name;
  int    val;
  char *docstring;
};
static struct debug_option_assoc debugassoc[] =
  {
    { "help", DebugHelp, "Explain the various -D options" },
    { "tree", DebugExpressionTree, "Display the expression tree" },
    { "search",DebugSearch, "Navigate the directory tree verbosely" },
    { "stat", DebugStat, "Trace calls to stat(2) and lstat(2)" },
    { "rates", DebugSuccessRates, "Indicate how often each predicate succeeded" },
    { "opt",  DebugExpressionTree|DebugTreeOpt, "Show diagnostic information relating to optimisation" },
    { "exec", DebugExec,  "Show diagnostic information relating to -exec, -execdir, -ok and -okdir" }
  };
#define N_DEBUGASSOC (sizeof(debugassoc)/sizeof(debugassoc[0]))




/* Add a primary of predicate type PRED_FUNC (described by ENTRY) to the predicate input list.

   Return a pointer to the predicate node just inserted.

   Fills in the following cells of the new predicate node:

   pred_func	    PRED_FUNC
   args(.str)	    NULL
   p_type	    PRIMARY_TYPE
   p_prec	    NO_PREC

   Other cells that need to be filled in are defaulted by
   get_new_pred_chk_op, which is used to insure that the prior node is
   either not there at all (we are the very first node) or is an
   operator. */

struct predicate *
insert_primary_withpred (const struct parser_table *entry,
			 PRED_FUNC pred_func,
			 const char *arg)
{
  struct predicate *new_pred;

  new_pred = get_new_pred_chk_op (entry, arg);
  new_pred->pred_func = pred_func;
  new_pred->p_name = entry->parser_name;
  new_pred->args.str = NULL;
  new_pred->p_type = PRIMARY_TYPE;
  new_pred->p_prec = NO_PREC;
  return new_pred;
}

/* Add a primary described by ENTRY to the predicate input list.

   Return a pointer to the predicate node just inserted.

   Fills in the following cells of the new predicate node:

   pred_func	    PRED_FUNC
   args(.str)	    NULL
   p_type	    PRIMARY_TYPE
   p_prec	    NO_PREC

   Other cells that need to be filled in are defaulted by
   get_new_pred_chk_op, which is used to insure that the prior node is
   either not there at all (we are the very first node) or is an
   operator. */
struct predicate *
insert_primary (const struct parser_table *entry, const char *arg)
{
  assert (entry->pred_func != NULL);
  return insert_primary_withpred (entry, entry->pred_func, arg);
}

struct predicate *
insert_primary_noarg (const struct parser_table *entry)
{
  return insert_primary (entry, NULL);
}



static void
show_valid_debug_options (FILE *fp, int full)
{
  size_t i;
  if (full)
    {
      fprintf (fp, "Valid arguments for -D:\n");
      for (i=0; i<N_DEBUGASSOC; ++i)
	{
	  fprintf (fp, "%-10s %s\n",
		   debugassoc[i].name,
		   debugassoc[i].docstring);
	}
    }
  else
    {
      for (i=0; i<N_DEBUGASSOC; ++i)
	{
	  fprintf (fp, "%s%s", (i>0 ? "|" : ""), debugassoc[i].name);
	}
    }
}

void
usage (FILE *fp, int status, char *msg)
{
  if (msg)
    fprintf (fp, "%s: %s\n", program_name, msg);

  fprintf (fp, _("Usage: %s [-H] [-L] [-P] [-Olevel] [-D "), program_name);
  show_valid_debug_options (fp, 0);
  fprintf (fp, _("] [path...] [expression]\n"));
  if (0 != status)
    exit (status);
}

void
set_stat_placeholders (struct stat *p)
{
#if HAVE_STRUCT_STAT_ST_BIRTHTIME
  p->st_birthtime = 0;
#endif
#if HAVE_STRUCT_STAT_ST_BIRTHTIMENSEC
  p->st_birthtimensec = 0;
#endif
#if HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_NSEC
  p->st_birthtimespec.tv_nsec = -1;
#endif
#if HAVE_STRUCT_STAT_ST_BIRTHTIMESPEC_TV_SEC
  p->st_birthtimespec.tv_sec = 0;
#else
  /* Avoid pointless compiler warning about unused parameters if none of these
     macros are set to nonzero values. */
  (void) p;
#endif
}


/* Get the stat information for a file, if it is
 * not already known.  Returns 0 on success.
 */
int
get_statinfo (const char *pathname, const char *name, struct stat *p)
{
  /* Set markers in fields so we have a good idea if the implementation
   * didn't bother to set them (e.g., NetBSD st_birthtimespec for MS-DOS
   * files)
   */
  if (!state.have_stat)
    {
      set_stat_placeholders (p);
      if (0 == (*options.xstat) (name, p))
	{
	  if (00000 == p->st_mode)
	    {
	      /* Savannah bug #16378. */
	      error (0, 0, _("WARNING: file %s appears to have mode 0000"),
		     quotearg_n_style (0, options.err_quoting_style, name));
	      error_severity (1);
	    }
	}
      else
	{
	  if (!options.ignore_readdir_race || (errno != ENOENT) )
	    {
	      nonfatal_target_file_error (errno, pathname);
	    }
	  return -1;
	}
    }
  state.have_stat = true;
  state.have_type = true;
  state.type = p->st_mode;

  return 0;
}

/* Get the stat/type/inode information for a file, if it is not
 * already known.   Returns 0 on success (or if we did nothing).
 */
int
get_info (const char *pathname,
	  struct stat *p,
	  struct predicate *pred_ptr)
{
  bool todo = false;

  /* If we need the full stat info, or we need the type info but don't
   * already have it, stat the file now.
   */
  if (pred_ptr->need_stat)
    {
      todo = true;		/* need full stat info */
    }
  else if (pred_ptr->need_type && !state.have_type)
    {
      todo = true;		/* need to stat to get the type */
    }
  else if (pred_ptr->need_inum)
    {
      if (!p->st_ino)
	{
	  todo = true;		/* need to stat to get the inode number */
	}
      else if ((!state.have_type) || S_ISDIR(p->st_mode))
	{
	  /* For now we decide not to trust struct dirent.d_ino for
	   * directory entries that are subdirectories, in case this
	   * subdirectory is a mount point.  We also need to call a
	   * stat function if we don't have st_ino (i.e. it is zero).
	   */
	  todo = true;
	}
    }
  if (todo)
    {
      int result = get_statinfo (pathname, state.rel_pathname, p);
      if (result != 0)
	{
	  return -1;		/* failure. */
	}
      else
	{
	  /* Verify some postconditions.  We can't check st_mode for
	     non-zero-ness because of Savannah bug #16378 (which is
	     that broken NFS servers can return st_mode==0). */
	  if (pred_ptr->need_type)
	    {
	      assert (state.have_type);
	    }
	  if (pred_ptr->need_inum)
	    {
	      assert (p->st_ino);
	    }
	  return 0;		/* success. */
	}
    }
  else
    {
      return 0;			/* success; nothing to do. */
    }
}

/* Determine if we can use O_NOFOLLOW.
 */
#if defined O_NOFOLLOW
bool
check_nofollow (void)
{
  struct utsname uts;
  float  release;

  if (0 == O_NOFOLLOW)
    {
      return false;
    }

  if (0 == uname (&uts))
    {
      /* POSIX requires that atof ignores "unrecognised suffixes"; we specifically
       * want that behaviour. */
      double (*conversion)(const char*) = atof;  /* avoid sc_prohibit_atoi_atof check. */
      release = conversion (uts.release);

      if (0 == strcmp ("Linux", uts.sysname))
	{
	  /* Linux kernels 2.1.126 and earlier ignore the O_NOFOLLOW flag. */
	  return release >= 2.2; /* close enough */
	}
      else if (0 == strcmp ("FreeBSD", uts.sysname))
	{
	  /* FreeBSD 3.0-CURRENT and later support it */
	  return release >= 3.1;
	}
    }

  /* Well, O_NOFOLLOW was defined, so we'll try to use it. */
  return true;
}
#endif


static int
exec_cb (void *context)
{
  struct exec_val *execp = context;
  bc_do_exec (&execp->ctl, &execp->state);
  return 0;
}

static void
do_exec (struct exec_val *execp)
{
  run_in_dir (execp->wd_for_exec, exec_cb, execp);
  if (execp->wd_for_exec != initial_wd)
    {
      free_cwd (execp->wd_for_exec);
      free (execp->wd_for_exec);
      execp->wd_for_exec = NULL;
    }
}


/* Examine the predicate list for instances of -execdir or -okdir
 * which have been terminated with '+' (build argument list) rather
 * than ';' (singles only).  If there are any, run them (this will
 * have no effect if there are no arguments waiting).
 */
static void
do_complete_pending_execdirs (struct predicate *p)
{
  if (NULL == p)
    return;

  assert (state.execdirs_outstanding);

  do_complete_pending_execdirs (p->pred_left);

  if (pred_is (p, pred_execdir) || pred_is(p, pred_okdir))
    {
      /* It's an exec-family predicate.  p->args.exec_val is valid. */
      if (p->args.exec_vec.multiple)
	{
	  struct exec_val *execp = &p->args.exec_vec;

	  /* This one was terminated by '+' and so might have some
	   * left... Run it if necessary.
	   */
	  if (execp->state.todo)
	    {
	      /* There are not-yet-executed arguments. */
	      do_exec (execp);
	    }
	}
    }

  do_complete_pending_execdirs (p->pred_right);
}

void
complete_pending_execdirs (void)
{
  if (state.execdirs_outstanding)
    {
      do_complete_pending_execdirs (get_eval_tree());
      state.execdirs_outstanding = false;
    }
}



/* Examine the predicate list for instances of -exec which have been
 * terminated with '+' (build argument list) rather than ';' (singles
 * only).  If there are any, run them (this will have no effect if
 * there are no arguments waiting).
 */
void
complete_pending_execs (struct predicate *p)
{
  if (NULL == p)
    return;

  complete_pending_execs (p->pred_left);

  /* It's an exec-family predicate then p->args.exec_val is valid
   * and we can check it.
   */
  /* XXX: what about pred_ok() ? */
  if (pred_is (p, pred_exec) && p->args.exec_vec.multiple)
    {
      struct exec_val *execp = &p->args.exec_vec;

      /* This one was terminated by '+' and so might have some
       * left... Run it if necessary.  Set state.exit_status if
       * there are any problems.
       */
      if (execp->state.todo)
	{
	  /* There are not-yet-executed arguments. */
	  bc_do_exec (&execp->ctl, &execp->state);
	}
    }

  complete_pending_execs (p->pred_right);
}

void
record_initial_cwd (void)
{
  initial_wd = xmalloc (sizeof (*initial_wd));
  if (0 != save_cwd (initial_wd))
    {
      error (EXIT_FAILURE, errno,
	     _("Failed to save initial working directory%s%s"),
	     (initial_wd->desc < 0 && initial_wd->name) ? ": " : "",
	     (initial_wd->desc < 0 && initial_wd->name) ? initial_wd->name : "");
    }
}

static void
cleanup_initial_cwd (void)
{
  if (0 == restore_cwd (initial_wd))
    {
      free_cwd (initial_wd);
      free (initial_wd);
      initial_wd = NULL;
    }
  else
    {
      /* since we may already be in atexit, die with _exit(). */
      error (0, errno,
	     _("Failed to restore initial working directory%s%s"),
	     (initial_wd->desc < 0 && initial_wd->name) ? ": " : "",
	     (initial_wd->desc < 0 && initial_wd->name) ? initial_wd->name : "");
      _exit (EXIT_FAILURE);
    }
}


static void
traverse_tree (struct predicate *tree,
			  void (*callback)(struct predicate*))
{
  if (tree->pred_left)
    traverse_tree (tree->pred_left, callback);

  callback (tree);

  if (tree->pred_right)
    traverse_tree (tree->pred_right, callback);
}

/* After sharefile_destroy is called, our output file
 * pointers will be dangling (fclose will already have
 * been called on them).  NULL these out.
 */
static void
undangle_file_pointers (struct predicate *p)
{
  if (pred_is (p, pred_fprint)
      || pred_is (p, pred_fprintf)
      || pred_is (p, pred_fls)
      || pred_is (p, pred_fprint0))
    {
      /* The file was already fclose()d by sharefile_destroy. */
      p->args.printf_vec.stream = NULL;
    }
}

/* Return nonzero if file descriptor leak-checking is enabled.
 */
bool
fd_leak_check_is_enabled (void)
{
  if (getenv ("GNU_FINDUTILS_FD_LEAK_CHECK"))
    return true;
  else
    return false;

}

/* Complete any outstanding commands.
 * Flush and close any open files.
 */
void
cleanup (void)
{
  struct predicate *eval_tree = get_eval_tree ();
  if (eval_tree)
    {
      traverse_tree (eval_tree, complete_pending_execs);
      complete_pending_execdirs ();
    }

  /* Close ouptut files and NULL out references to them. */
  sharefile_destroy (state.shared_files);
  if (eval_tree)
    traverse_tree (eval_tree, undangle_file_pointers);

  cleanup_initial_cwd ();

  if (fd_leak_check_is_enabled ())
    {
      complain_about_leaky_fds ();
      forget_non_cloexec_fds ();
    }

  if (fflush (stdout) == EOF)
    nonfatal_nontarget_file_error (errno, "standard output");
}


static int
fallback_stat (const char *name, struct stat *p, int prev_rv)
{
  /* Our original stat() call failed.  Perhaps we can't follow a
   * symbolic link.  If that might be the problem, lstat() the link.
   * Otherwise, admit defeat.
   */
  switch (errno)
    {
    case ENOENT:
    case ENOTDIR:
      if (options.debug_options & DebugStat)
	fprintf(stderr, "fallback_stat(): stat(%s) failed; falling back on lstat()\n", name);
      return fstatat(state.cwd_dir_fd, name, p, AT_SYMLINK_NOFOLLOW);

    case EACCES:
    case EIO:
    case ELOOP:
    case ENAMETOOLONG:
#ifdef EOVERFLOW
    case EOVERFLOW:	    /* EOVERFLOW is not #defined on UNICOS. */
#endif
    default:
      return prev_rv;
    }
}


/* optionh_stat() implements the stat operation when the -H option is
 * in effect.
 *
 * If the item to be examined is a command-line argument, we follow
 * symbolic links.  If the stat() call fails on the command-line item,
 * we fall back on the properties of the symbolic link.
 *
 * If the item to be examined is not a command-line argument, we
 * examine the link itself.
 */
int
optionh_stat (const char *name, struct stat *p)
{
  if (AT_FDCWD != state.cwd_dir_fd)
    assert (state.cwd_dir_fd >= 0);
  set_stat_placeholders (p);
  if (0 == state.curdepth)
    {
      /* This file is from the command line; deference the link (if it
       * is a link).
       */
      int rv;
      rv = fstatat (state.cwd_dir_fd, name, p, 0);
      if (0 == rv)
	return 0;		/* success */
      else
	return fallback_stat (name, p, rv);
    }
  else
    {
      /* Not a file on the command line; do not dereference the link.
       */
      return fstatat (state.cwd_dir_fd, name, p, AT_SYMLINK_NOFOLLOW);
    }
}

/* optionl_stat() implements the stat operation when the -L option is
 * in effect.  That option makes us examine the thing the symbolic
 * link points to, not the symbolic link itself.
 */
int
optionl_stat(const char *name, struct stat *p)
{
  int rv;
  if (AT_FDCWD != state.cwd_dir_fd)
    assert (state.cwd_dir_fd >= 0);

  set_stat_placeholders (p);
  rv = fstatat (state.cwd_dir_fd, name, p, 0);
  if (0 == rv)
    return 0;			/* normal case. */
  else
    return fallback_stat (name, p, rv);
}

/* optionp_stat() implements the stat operation when the -P option is
 * in effect (this is also the default).  That option makes us examine
 * the symbolic link itself, not the thing it points to.
 */
int
optionp_stat (const char *name, struct stat *p)
{
  assert ((state.cwd_dir_fd >= 0) || (state.cwd_dir_fd==AT_FDCWD));
  set_stat_placeholders (p);
  return fstatat (state.cwd_dir_fd, name, p, AT_SYMLINK_NOFOLLOW);
}


static uintmax_t stat_count = 0u;

int
debug_stat (const char *file, struct stat *bufp)
{
  ++stat_count;
  fprintf (stderr, "debug_stat (%s)\n", file);

  switch (options.symlink_handling)
    {
    case SYMLINK_ALWAYS_DEREF:
      return optionl_stat (file, bufp);
    case SYMLINK_DEREF_ARGSONLY:
      return optionh_stat (file, bufp);
    case SYMLINK_NEVER_DEREF:
      return optionp_stat (file, bufp);
    }
  /*NOTREACHED*/
  assert (0);
  return -1;
}


bool
following_links(void)
{
  switch (options.symlink_handling)
    {
    case SYMLINK_ALWAYS_DEREF:
      return true;
    case SYMLINK_DEREF_ARGSONLY:
      return (state.curdepth == 0);
    case SYMLINK_NEVER_DEREF:
    default:
      return false;
    }
}


/* Take a "mode" indicator and fill in the files of 'state'.
 */
bool
digest_mode (mode_t *mode,
	     const char *pathname,
	     const char *name,
	     struct stat *pstat,
	     bool leaf)
{
  /* If we know the type of the directory entry, and it is not a
   * symbolic link, we may be able to avoid a stat() or lstat() call.
   */
  if (*mode)
    {
      if (S_ISLNK(*mode) && following_links())
	{
	  /* mode is wrong because we should have followed the symlink. */
	  if (get_statinfo (pathname, name, pstat) != 0)
	    return false;
	  *mode = state.type = pstat->st_mode;
	  state.have_type = true;
	}
      else
	{
	  state.have_type = true;
	  pstat->st_mode = state.type = *mode;
	}
    }
  else
    {
      /* Mode is not yet known; may have to stat the file unless we
       * can deduce that it is not a directory (which is all we need to
       * know at this stage)
       */
      if (leaf)
	{
	  state.have_stat = false;
	  state.have_type = false;
	  state.type = 0;
	}
      else
	{
	  if (get_statinfo (pathname, name, pstat) != 0)
	    return false;

	  /* If -L is in effect and we are dealing with a symlink,
	   * st_mode is the mode of the pointed-to file, while mode is
	   * the mode of the directory entry (S_IFLNK).  Hence now
	   * that we have the stat information, override "mode".
	   */
	  state.type = *mode = pstat->st_mode;
	  state.have_type = true;
	}
    }

  /* success. */
  return true;
}


/* Return true if there are no predicates with no_default_print in
   predicate list PRED, false if there are any.
   Returns true if default print should be performed */

bool
default_prints (struct predicate *pred)
{
  while (pred != NULL)
    {
      if (pred->no_default_print)
	return (false);
      pred = pred->pred_next;
    }
  return (true);
}

bool
looks_like_expression (const char *arg, bool leading)
{
  switch (arg[0])
    {
    case '-':
      if (arg[1])		/* "-foo" is an expression.  */
	return true;
      else
	return false;		/* Just "-" is a filename. */
      break;

    case ')':
    case ',':
      if (arg[1])
	return false;		/* )x and ,z are not expressions */
      else
	return !leading;	/* A leading ) or , is not either */

      /* ( and ! are part of an expression, but (2 and !foo are
       * filenames.
       */
    case '!':
    case '(':
      if (arg[1])
	return false;
      else
	return true;

    default:
      return false;
    }
}

static void
process_debug_options (char *arg)
{
  const char *p;
  char *token_context = NULL;
  const char delimiters[] = ",";
  bool empty = true;
  size_t i;

  p = strtok_r (arg, delimiters, &token_context);
  while (p)
    {
      empty = false;

      for (i=0; i<N_DEBUGASSOC; ++i)
	{
	  if (0 == strcmp (debugassoc[i].name, p))
	    {
	      options.debug_options |= debugassoc[i].val;
	      break;
	    }
	}
      if (i >= N_DEBUGASSOC)
	{
	  error (0, 0, _("Ignoring unrecognised debug flag %s"),
		 quotearg_n_style (0, options.err_quoting_style, arg));
	}
      p = strtok_r (NULL, delimiters, &token_context);
    }
  if (empty)
    {
      error(EXIT_FAILURE, 0, _("Empty argument to the -D option."));
    }
  else if (options.debug_options & DebugHelp)
    {
      show_valid_debug_options (stdout, 1);
      exit (EXIT_SUCCESS);
    }
}


static void
process_optimisation_option (const char *arg)
{
  if (0 == arg[0])
    {
      error (EXIT_FAILURE, 0,
	     _("The -O option must be immediately followed by a decimal integer"));
    }
  else
    {
      unsigned long opt_level;
      char *end;

      if (!isdigit ( (unsigned char) arg[0] ))
	{
	  error (EXIT_FAILURE, 0,
		 _("Please specify a decimal number immediately after -O"));
	}
      else
	{
	  int prev_errno = errno;
	  errno  = 0;

	  opt_level = strtoul (arg, &end, 10);
	  if ( (0==opt_level) && (end==arg) )
	    {
	      error (EXIT_FAILURE, 0,
		     _("Please specify a decimal number immediately after -O"));
	    }
	  else if (*end)
	    {
	      /* unwanted trailing characters. */
	      error (EXIT_FAILURE, 0, _("Invalid optimisation level %s"), arg);
	    }
	  else if ( (ULONG_MAX==opt_level) && errno)
	    {
	      error (EXIT_FAILURE, errno,
		     _("Invalid optimisation level %s"), arg);
	    }
	  else if (opt_level > USHRT_MAX)
	    {
	      /* tricky to test, as on some platforms USHORT_MAX and ULONG_MAX
	       * can have the same value, though this is unusual.
	       */
	      error (EXIT_FAILURE, 0,
		     _("Optimisation level %lu is too high.  "
		       "If you want to find files very quickly, "
		       "consider using GNU locate."),
		     opt_level);
	    }
	  else
	    {
	      options.optimisation_level = opt_level;
	      errno = prev_errno;
	    }
	}
    }
}

int
process_leading_options (int argc, char *argv[])
{
  int i, end_of_leading_options;

  for (i=1; (end_of_leading_options = i) < argc; ++i)
    {
      if (0 == strcmp ("-H", argv[i]))
	{
	  /* Meaning: dereference symbolic links on command line, but nowhere else. */
	  set_follow_state (SYMLINK_DEREF_ARGSONLY);
	}
      else if (0 == strcmp ("-L", argv[i]))
	{
	  /* Meaning: dereference all symbolic links. */
	  set_follow_state (SYMLINK_ALWAYS_DEREF);
	}
      else if (0 == strcmp ("-P", argv[i]))
	{
	  /* Meaning: never dereference symbolic links (default). */
	  set_follow_state (SYMLINK_NEVER_DEREF);
	}
      else if (0 == strcmp ("--", argv[i]))
	{
	  /* -- signifies the end of options. */
	  end_of_leading_options = i+1;	/* Next time start with the next option */
	  break;
	}
      else if (0 == strcmp ("-D", argv[i]))
	{
	  process_debug_options (argv[i+1]);
	  ++i;			/* skip the argument too. */
	}
      else if (0 == strncmp ("-O", argv[i], 2))
	{
	  process_optimisation_option (argv[i]+2);
	}
      else
	{
	  /* Hmm, must be one of
	   * (a) A path name
	   * (b) A predicate
	   */
	  end_of_leading_options = i; /* Next time start with this option */
	  break;
	}
    }
  return end_of_leading_options;
}

static struct timespec
now(void)
{
  struct timespec retval;
  struct timeval tv;
  time_t t;

  if (0 == gettimeofday (&tv, NULL))
    {
      retval.tv_sec  = tv.tv_sec;
      retval.tv_nsec = tv.tv_usec * 1000; /* convert unit from microseconds to nanoseconds */
      return retval;
    }
  t = time (NULL);
  assert (t != (time_t)-1);
  retval.tv_sec = t;
  retval.tv_nsec = 0;
  return retval;
}

void
set_option_defaults (struct options *p)
{
  if (getenv ("POSIXLY_CORRECT"))
    p->posixly_correct = true;
  else
    p->posixly_correct = false;

  /* We call check_nofollow() before setlocale() because the numbers
   * for which we check (in the results of uname) definitiely have "."
   * as the decimal point indicator even under locales for which that
   * is not normally true.   Hence atof would do the wrong thing
   * if we call it after setlocale().
   */
#ifdef O_NOFOLLOW
  p->open_nofollow_available = check_nofollow ();
#else
  p->open_nofollow_available = false;
#endif

  p->regex_options = RE_SYNTAX_EMACS;

  if (isatty (0))
    {
      p->warnings = true;
      p->literal_control_chars = false;
    }
  else
    {
      p->warnings = false;
      p->literal_control_chars = false; /* may change */
    }
  if (p->posixly_correct)
    {
      p->warnings = false;
    }

  p->do_dir_first = true;
  p->explicit_depth = false;
  p->maxdepth = p->mindepth = -1;

  p->start_time = now ();
  p->cur_day_start.tv_sec = p->start_time.tv_sec - DAYSECS;
  p->cur_day_start.tv_nsec = p->start_time.tv_nsec;

  p->full_days = false;
  p->stay_on_filesystem = false;
  p->ignore_readdir_race = false;

  if (p->posixly_correct)
    p->output_block_size = 512;
  else
    p->output_block_size = 1024;

  p->debug_options = 0uL;
  p->optimisation_level = 2;

  if (getenv ("FIND_BLOCK_SIZE"))
    {
      error (EXIT_FAILURE, 0,
	     _("The environment variable FIND_BLOCK_SIZE is not supported, the only thing that affects the block size is the POSIXLY_CORRECT environment variable"));
    }

#if LEAF_OPTIMISATION
  /* The leaf optimisation is enabled. */
  p->no_leaf_check = false;
#else
  /* The leaf optimisation is disabled. */
  p->no_leaf_check = true;
#endif

  set_follow_state (SYMLINK_NEVER_DEREF); /* The default is equivalent to -P. */

  p->err_quoting_style = locale_quoting_style;
}


/* apply_predicate
 *
 */
bool
apply_predicate(const char *pathname, struct stat *stat_buf, struct predicate *p)
{
  ++p->perf.visits;

  if (p->need_stat || p->need_type || p->need_inum)
    {
      /* We may need a stat here. */
      if (get_info(pathname, stat_buf, p) != 0)
	    return false;
    }
  if ((p->pred_func)(pathname, stat_buf, p))
    {
      ++(p->perf.successes);
      return true;
    }
  else
    {
      return false;
    }
}


/* is_exec_in_local_dir
 *
 */
bool
is_exec_in_local_dir (const PRED_FUNC pred_func)
{
  return pred_execdir == pred_func || pred_okdir == pred_func;
}

/* safely_quote_err_filename
 *
 */
const char *
safely_quote_err_filename (int n, char const *arg)
{
  return quotearg_n_style (n, options.err_quoting_style, arg);
}

/* We have encountered an error which should affect the exit status.
 * This is normally used to change the exit status from 0 to 1.
 * However, if the exit status is already 2 for example, we don't want to
 * reduce it to 1.
 */
void
error_severity (int level)
{
  if (state.exit_status < level)
    state.exit_status = level;
}


/* report_file_err
 */
static void
report_file_err(int exitval, int errno_value,
		bool is_target_file, const char *name)
{
  /* It is important that the errno value is passed in as a function
   * argument before we call safely_quote_err_filename(), because otherwise
   * we might find that safely_quote_err_filename() changes errno.
   */
  if (!is_target_file || !state.already_issued_stat_error_msg)
    {
      error (exitval, errno_value, "%s", safely_quote_err_filename (0, name));
      error_severity (1);
    }
  if (is_target_file)
    {
      state.already_issued_stat_error_msg = true;
    }
}

/* nonfatal_target_file_error
 */
void
nonfatal_target_file_error (int errno_value, const char *name)
{
  report_file_err (0, errno_value, true, name);
}

/* fatal_target_file_error
 *
 * Report an error on a target file (i.e. a file we are searching).
 * Such errors are only reported once per searched file.
 *
 */
void
fatal_target_file_error(int errno_value, const char *name)
{
  report_file_err (1, errno_value, true, name);
  /*NOTREACHED*/
  abort ();
}

/* nonfatal_nontarget_file_error
 *
 */
void
nonfatal_nontarget_file_error (int errno_value, const char *name)
{
  report_file_err (0, errno_value, false, name);
}

/* fatal_nontarget_file_error
 *
 */
void
fatal_nontarget_file_error(int errno_value, const char *name)
{
  /* We're going to exit fatally, so make sure we always isssue the error
   * message, even if it will be duplicate.   Motivation: otherwise it may
   * not be clear what went wrong.
   */
  state.already_issued_stat_error_msg = false;
  report_file_err (1, errno_value, false, name);
  /*NOTREACHED*/
  abort ();
}
