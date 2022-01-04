/* pred.c -- execute the expression tree.
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

/* config.h always comes first. */
#include <config.h>

/* system headers. */
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <locale.h>
#include <math.h>
#include <pwd.h>
#include <selinux/selinux.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> /* for unlinkat() */

/* gnulib headers. */
#include "areadlink.h"
#include "dirname.h"
#include "error.h"
#include "fnmatch.h"
#include "gettext.h"
#include "stat-size.h"
#include "stat-time.h"
#include "yesno.h"

/* find headers. */
#include "defs.h"
#include "dircallback.h"
#include "listfile.h"
#include "printquoted.h"



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

#ifdef CLOSEDIR_VOID
/* Fake a return value. */
#define CLOSEDIR(d) (closedir (d), 0)
#else
#define CLOSEDIR(d) closedir (d)
#endif

static bool match_lname (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr, bool ignore_case);

#ifdef	DEBUG
struct pred_assoc
{
  PRED_FUNC pred_func;
  char *pred_name;
};

struct pred_assoc pred_table[] =
{
  {pred_amin, "amin    "},
  {pred_and, "and     "},
  {pred_anewer, "anewer  "},
  {pred_atime, "atime   "},
  {pred_closeparen, ")       "},
  {pred_cmin, "cmin    "},
  {pred_cnewer, "cnewer  "},
  {pred_comma, ",       "},
  {pred_ctime, "ctime   "},
  {pred_delete, "delete  "},
  {pred_empty, "empty   "},
  {pred_exec, "exec    "},
  {pred_execdir, "execdir "},
  {pred_executable, "executable "},
  {pred_false, "false   "},
  {pred_fprint, "fprint  "},
  {pred_fprint0, "fprint0 "},
  {pred_fprintf, "fprintf "},
  {pred_fstype, "fstype  "},
  {pred_gid, "gid     "},
  {pred_group, "group   "},
  {pred_ilname, "ilname  "},
  {pred_iname, "iname   "},
  {pred_inum, "inum    "},
  {pred_ipath, "ipath   "},
  {pred_links, "links   "},
  {pred_lname, "lname   "},
  {pred_ls, "ls      "},
  {pred_mmin, "mmin    "},
  {pred_mtime, "mtime   "},
  {pred_name, "name    "},
  {pred_negate, "not     "},
  {pred_newer, "newer   "},
  {pred_newerXY, "newerXY   "},
  {pred_nogroup, "nogroup "},
  {pred_nouser, "nouser  "},
  {pred_ok, "ok      "},
  {pred_okdir, "okdir   "},
  {pred_openparen, "(       "},
  {pred_or, "or      "},
  {pred_path, "path    "},
  {pred_perm, "perm    "},
  {pred_print, "print   "},
  {pred_print0, "print0  "},
  {pred_prune, "prune   "},
  {pred_quit, "quit    "},
  {pred_readable, "readable    "},
  {pred_regex, "regex   "},
  {pred_samefile,"samefile "},
  {pred_size, "size    "},
  {pred_true, "true    "},
  {pred_type, "type    "},
  {pred_uid, "uid     "},
  {pred_used, "used    "},
  {pred_user, "user    "},
  {pred_writable, "writable "},
  {pred_xtype, "xtype   "},
  {pred_context, "context"},
  {0, "none    "}
};
#endif

/* Returns ts1 - ts2 */
static double ts_difference (struct timespec ts1,
			     struct timespec ts2)
{
  double d =  difftime (ts1.tv_sec, ts2.tv_sec)
    + (1.0e-9 * (ts1.tv_nsec - ts2.tv_nsec));
  return d;
}


static int
compare_ts (struct timespec ts1,
	    struct timespec ts2)
{
  if ((ts1.tv_sec == ts2.tv_sec) &&
      (ts1.tv_nsec == ts2.tv_nsec))
    {
      return 0;
    }
  else
    {
      double diff = ts_difference (ts1, ts2);
      return diff < 0.0 ? -1 : +1;
    }
}

/* Predicate processing routines.

   PATHNAME is the full pathname of the file being checked.
   *STAT_BUF contains information about PATHNAME.
   *PRED_PTR contains information for applying the predicate.

   Return true if the file passes this predicate, false if not. */


/* pred_timewindow
 *
 * Returns true if THE_TIME is
 * COMP_GT: after the specified time
 * COMP_LT: before the specified time
 * COMP_EQ: after the specified time but by not more than WINDOW seconds.
 */
static bool
pred_timewindow (struct timespec ts, struct predicate const *pred_ptr, int window)
{
  switch (pred_ptr->args.reftime.kind)
    {
    case COMP_GT:
      return compare_ts (ts, pred_ptr->args.reftime.ts) > 0;

    case COMP_LT:
      return compare_ts (ts, pred_ptr->args.reftime.ts) < 0;

    case COMP_EQ:
      {
	/* consider "find . -mtime 0".
	 *
	 * Here, the origin is exactly 86400 seconds before the start
	 * of the program (since -daystart was not specified).   This
	 * function will be called with window=86400 and
	 * pred_ptr->args.reftime.ts as the origin.  Hence a file
	 * created the instant the program starts will show a time
	 * difference (value of delta) of 86400.   Similarly, a file
	 * created exactly 24h ago would be the newest file which was
	 * _not_ created today.   So, if delta is 0.0, the file
	 * was not created today.  If the delta is 86400, the file
	 * was created this instant.
	 */
	double delta = ts_difference (ts, pred_ptr->args.reftime.ts);
	return (delta > 0.0 && delta <= window);
      }
    }
  assert (0);
  abort ();
}


bool
pred_amin (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  return pred_timewindow (get_stat_atime(stat_buf), pred_ptr, 60);
}

bool
pred_and (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  if (pred_ptr->pred_left == NULL
      || apply_predicate (pathname, stat_buf, pred_ptr->pred_left))
    {
      return apply_predicate (pathname, stat_buf, pred_ptr->pred_right);
    }
  else
    return false;
}

bool
pred_anewer (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  assert (COMP_GT == pred_ptr->args.reftime.kind);
  return compare_ts (get_stat_atime(stat_buf), pred_ptr->args.reftime.ts) > 0;
}

bool
pred_atime (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  return pred_timewindow (get_stat_atime(stat_buf), pred_ptr, DAYSECS);
}

bool
pred_closeparen (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  (void) &stat_buf;
  (void) &pred_ptr;

  return true;
}

bool
pred_cmin (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  return pred_timewindow (get_stat_ctime(stat_buf), pred_ptr, 60);
}

bool
pred_cnewer (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  assert (COMP_GT == pred_ptr->args.reftime.kind);
  return compare_ts (get_stat_ctime(stat_buf), pred_ptr->args.reftime.ts) > 0;
}

bool
pred_comma (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  if (pred_ptr->pred_left != NULL)
    {
      apply_predicate (pathname, stat_buf,pred_ptr->pred_left);
    }
  return apply_predicate (pathname, stat_buf, pred_ptr->pred_right);
}

bool
pred_ctime (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  return pred_timewindow (get_stat_ctime(stat_buf), pred_ptr, DAYSECS);
}

static bool
perform_delete (int flags)
{
  return 0 == unlinkat (state.cwd_dir_fd, state.rel_pathname, flags);
}


bool
pred_delete (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pred_ptr;
  (void) stat_buf;
  if (strcmp (state.rel_pathname, "."))
    {
      int flags=0;
      if (state.have_stat && S_ISDIR(stat_buf->st_mode))
	flags |= AT_REMOVEDIR;
      if (perform_delete (flags))
	{
	  return true;
	}
      else
	{
	  if (ENOENT == errno && options.ignore_readdir_race)
	    {
	      /* Ignore unlink() error for vanished files.  */
	      errno = 0;
	      return true;
	    }
	  if (EISDIR == errno)
	    {
	      if ((flags & AT_REMOVEDIR) == 0)
		{
		  /* unlink() operation failed because we should have done rmdir(). */
		  flags |= AT_REMOVEDIR;
		  if (perform_delete (flags))
		    return true;
		}
	    }
	}
      error (0, errno, _("cannot delete %s"),
	     safely_quote_err_filename (0, pathname));
      /* Previously I had believed that having the -delete action
       * return false provided the user with control over whether an
       * error message is issued.  While this is true, the policy of
       * not affecting the exit status is contrary to the POSIX
       * requirement that diagnostic messages are accompanied by a
       * nonzero exit status.  While -delete is not a POSIX option and
       * we can therefore opt not to follow POSIX in this case, that
       * seems somewhat arbitrary and confusing.  So, as of
       * findutils-4.3.11, we also set the exit status in this case.
       */
      state.exit_status = 1;
      return false;
    }
  else
    {
      /* nothing to do. */
      return true;
    }
}

bool
pred_empty (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) pred_ptr;

  if (S_ISDIR (stat_buf->st_mode))
    {
      int fd;
      DIR *d;
      struct dirent *dp;
      bool empty = true;

      errno = 0;
      if ((fd = openat (state.cwd_dir_fd, state.rel_pathname, O_RDONLY
#if defined O_LARGEFILE
			|O_LARGEFILE
#endif
		       )) < 0)
	{
	  error (0, errno, "%s", safely_quote_err_filename (0, pathname));
	  state.exit_status = 1;
	  return false;
	}
      d = fdopendir (fd);
      if (d == NULL)
	{
	  error (0, errno, "%s", safely_quote_err_filename (0, pathname));
	  state.exit_status = 1;
	  return false;
	}
      for (dp = readdir (d); dp; dp = readdir (d))
	{
	  if (dp->d_name[0] != '.'
	      || (dp->d_name[1] != '\0'
		  && (dp->d_name[1] != '.' || dp->d_name[2] != '\0')))
	    {
	      empty = false;
	      break;
	    }
	}
      if (CLOSEDIR (d))
	{
	  error (0, errno, "%s", safely_quote_err_filename (0, pathname));
	  state.exit_status = 1;
	  return false;
	}
      return (empty);
    }
  else if (S_ISREG (stat_buf->st_mode))
    return (stat_buf->st_size == 0);
  else
    return (false);
}


bool
pred_exec (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  return impl_pred_exec (pathname, stat_buf, pred_ptr);
}

bool
pred_execdir (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
   (void) &pathname;
   return impl_pred_exec (state.rel_pathname, stat_buf, pred_ptr);
}

bool
pred_false (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  (void) &stat_buf;
  (void) &pred_ptr;


  return (false);
}

bool
pred_fls (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  FILE * stream = pred_ptr->args.printf_vec.stream;
  list_file (pathname, state.cwd_dir_fd, state.rel_pathname, stat_buf,
	     options.start_time.tv_sec,
	     options.output_block_size,
	     pred_ptr->literal_control_chars, stream);
  return true;
}

bool
pred_fprint (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  (void) &stat_buf;

  print_quoted (pred_ptr->args.printf_vec.stream,
		pred_ptr->args.printf_vec.quote_opts,
		pred_ptr->args.printf_vec.dest_is_tty,
		"%s\n",
		pathname);
  return true;
}

bool
pred_fprint0 (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  FILE * fp = pred_ptr->args.printf_vec.stream;

  (void) &stat_buf;

  fputs (pathname, fp);
  putc (0, fp);
  return true;
}



bool
pred_fstype (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  if (strcmp (filesystem_type (stat_buf, pathname), pred_ptr->args.str) == 0)
    return true;
  else
    return false;
}

bool
pred_gid (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  switch (pred_ptr->args.numinfo.kind)
    {
    case COMP_GT:
      if (stat_buf->st_gid > pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_LT:
      if (stat_buf->st_gid < pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_EQ:
      if (stat_buf->st_gid == pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    }
  return (false);
}

bool
pred_group (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  if (pred_ptr->args.gid == stat_buf->st_gid)
    return (true);
  else
    return (false);
}

bool
pred_ilname (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  return match_lname (pathname, stat_buf, pred_ptr, true);
}

/* Common code between -name, -iname.  PATHNAME is being visited, STR
   is name to compare basename against, and FLAGS are passed to
   fnmatch.  Recall that 'find / -name /' is one of the few times where a '/'
   in the -name must actually find something. */
static bool
pred_name_common (const char *pathname, const char *str, int flags)
{
  bool b;
  /* We used to use last_component() here, but that would not allow us to modify the
   * input string, which is const.   We could optimise by duplicating the string only
   * if we need to modify it, and I'll do that if there is a measurable
   * performance difference on a machine built after 1990...
   */
  char *base = base_name (pathname);
  /* remove trailing slashes, but leave  "/" or "//foo" unchanged. */
  strip_trailing_slashes (base);

  /* FNM_PERIOD is not used here because POSIX requires that it not be.
   * See http://standards.ieee.org/reading/ieee/interp/1003-2-92_int/pasc-1003.2-126.html
   */
  b = fnmatch (str, base, flags) == 0;
  free (base);
  return b;
}

bool
pred_iname (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) stat_buf;
  return pred_name_common (pathname, pred_ptr->args.str, FNM_CASEFOLD);
}

bool
pred_inum (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  assert (stat_buf->st_ino != 0);

  switch (pred_ptr->args.numinfo.kind)
    {
    case COMP_GT:
      if (stat_buf->st_ino > pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_LT:
      if (stat_buf->st_ino < pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_EQ:
      if (stat_buf->st_ino == pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    }
  return (false);
}

bool
pred_ipath (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) stat_buf;

  if (fnmatch (pred_ptr->args.str, pathname, FNM_CASEFOLD) == 0)
    return (true);
  return (false);
}

bool
pred_links (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  switch (pred_ptr->args.numinfo.kind)
    {
    case COMP_GT:
      if (stat_buf->st_nlink > pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_LT:
      if (stat_buf->st_nlink < pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_EQ:
      if (stat_buf->st_nlink == pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    }
  return (false);
}

bool
pred_lname (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  return match_lname (pathname, stat_buf, pred_ptr, false);
}

static bool
match_lname (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr, bool ignore_case)
{
  bool ret = false;
#ifdef S_ISLNK
  if (S_ISLNK (stat_buf->st_mode))
    {
      char *linkname = areadlinkat (state.cwd_dir_fd, state.rel_pathname);
      if (linkname)
	{
	  if (fnmatch (pred_ptr->args.str, linkname,
		       ignore_case ? FNM_CASEFOLD : 0) == 0)
	    ret = true;
	}
      else
	{
	  nonfatal_target_file_error (errno, pathname);
	  state.exit_status = 1;
	}
      free (linkname);
    }
#endif /* S_ISLNK */
  return ret;
}

bool
pred_ls (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  return pred_fls (pathname, stat_buf, pred_ptr);
}

bool
pred_mmin (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) &pathname;
  return pred_timewindow (get_stat_mtime(stat_buf), pred_ptr, 60);
}

bool
pred_mtime (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  return pred_timewindow (get_stat_mtime(stat_buf), pred_ptr, DAYSECS);
}

bool
pred_name (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) stat_buf;
  return pred_name_common (pathname, pred_ptr->args.str, 0);
}

bool
pred_negate (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  return !apply_predicate (pathname, stat_buf, pred_ptr->pred_right);
}

bool
pred_newer (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;

  assert (COMP_GT == pred_ptr->args.reftime.kind);
  return compare_ts (get_stat_mtime(stat_buf), pred_ptr->args.reftime.ts) > 0;
}

bool
pred_newerXY (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  struct timespec ts;
  bool collected = false;

  assert (COMP_GT == pred_ptr->args.reftime.kind);

  switch (pred_ptr->args.reftime.xval)
    {
    case XVAL_TIME:
      assert (pred_ptr->args.reftime.xval != XVAL_TIME);
      return false;

    case XVAL_ATIME:
      ts = get_stat_atime (stat_buf);
      collected = true;
      break;

    case XVAL_BIRTHTIME:
      ts = get_stat_birthtime (stat_buf);
      collected = true;
      if (ts.tv_nsec < 0)
	{
	  /* XXX: Cannot determine birth time.  Warn once. */
	  error (0, 0, _("WARNING: cannot determine birth time of file %s"),
		 safely_quote_err_filename (0, pathname));
	  return false;
	}
      break;

    case XVAL_CTIME:
      ts = get_stat_ctime (stat_buf);
      collected = true;
      break;

    case XVAL_MTIME:
      ts = get_stat_mtime (stat_buf);
      collected = true;
      break;
    }

  assert (collected);
  return compare_ts (ts, pred_ptr->args.reftime.ts) > 0;
}

bool
pred_nogroup (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) pred_ptr;
  return getgrgid (stat_buf->st_gid) == NULL;
}

bool
pred_nouser (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) pred_ptr;
  return getpwuid (stat_buf->st_uid) == NULL;
}


static bool
is_ok (const char *program, const char *arg)
{
  fflush (stdout);
  /* The draft open standard requires that, in the POSIX locale,
     the last non-blank character of this prompt be '?'.
     The exact format is not specified.
     This standard does not have requirements for locales other than POSIX
  */
  /* XXX: printing UNTRUSTED data here. */
  if (fprintf (stderr, _("< %s ... %s > ? "), program, arg) < 0)
    {
      error (EXIT_FAILURE, errno, _("Failed to write prompt for -ok"));
    }
  fflush (stderr);
  return yesno ();
}

bool
pred_ok (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  if (is_ok (pred_ptr->args.exec_vec.replace_vec[0], pathname))
    return impl_pred_exec (pathname, stat_buf, pred_ptr);
  else
    return false;
}

bool
pred_okdir (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  if (is_ok (pred_ptr->args.exec_vec.replace_vec[0], pathname))
    return impl_pred_exec (state.rel_pathname, stat_buf, pred_ptr);
  else
    return false;
}

bool
pred_openparen (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) stat_buf;
  (void) pred_ptr;
  return true;
}

bool
pred_or (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  if (pred_ptr->pred_left == NULL
      || !apply_predicate (pathname, stat_buf, pred_ptr->pred_left))
    {
      return apply_predicate (pathname, stat_buf, pred_ptr->pred_right);
    }
  else
    return true;
}

bool
pred_path (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) stat_buf;
  if (fnmatch (pred_ptr->args.str, pathname, 0) == 0)
    return (true);
  return (false);
}

bool
pred_perm (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  mode_t mode = stat_buf->st_mode;
  mode_t perm_val = pred_ptr->args.perm.val[S_ISDIR (mode) != 0];
  (void) pathname;
  switch (pred_ptr->args.perm.kind)
    {
    case PERM_AT_LEAST:
      return (mode & perm_val) == perm_val;
      break;

    case PERM_ANY:
      /* True if any of the bits set in the mask are also set in the file's mode.
       *
       *
       * Otherwise, if onum is prefixed by a hyphen, the primary shall
       * evaluate as true if at least all of the bits specified in
       * onum that are also set in the octal mask 07777 are set.
       *
       * Eric Blake's interpretation is that the mode argument is zero,

       */
      if (0 == perm_val)
	return true;		/* Savannah bug 14748; we used to return false */
      else
	return (mode & perm_val) != 0;
      break;

    case PERM_EXACT:
      return (mode & MODE_ALL) == perm_val;
      break;

    default:
      abort ();
      break;
    }
}


bool
pred_executable (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) stat_buf;
  (void) pred_ptr;

  /* As for access, the check is performed with the real user id. */
  return 0 == faccessat (state.cwd_dir_fd, state.rel_pathname, X_OK, 0);
}

bool
pred_readable (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) stat_buf;
  (void) pred_ptr;

  /* As for access, the check is performed with the real user id. */
  return 0 == faccessat (state.cwd_dir_fd, state.rel_pathname, R_OK, 0);
}

bool
pred_writable (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) stat_buf;
  (void) pred_ptr;

  /* As for access, the check is performed with the real user id. */
  return 0 == faccessat (state.cwd_dir_fd, state.rel_pathname, W_OK, 0);
}

bool
pred_print (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) stat_buf;
  (void) pred_ptr;

  print_quoted (pred_ptr->args.printf_vec.stream,
		pred_ptr->args.printf_vec.quote_opts,
		pred_ptr->args.printf_vec.dest_is_tty,
		"%s\n", pathname);
  return true;
}

bool
pred_print0 (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  return pred_fprint0(pathname, stat_buf, pred_ptr);
}

bool
pred_prune (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) pred_ptr;

  if (options.do_dir_first == true) { /* no effect with -depth */
    assert (state.have_stat);
    if (stat_buf != NULL &&
	S_ISDIR(stat_buf->st_mode))
      state.stop_at_current_level = true;
  }

  /* findutils used to return options.do_dir_first here, so that -prune
   * returns true only if -depth is not in effect.   But POSIX requires
   * that -prune always evaluate as true.
   */
  return true;
}

bool
pred_quit (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) stat_buf;
  (void) pred_ptr;

  /* Run any cleanups.  This includes executing any command lines
   * we have partly built but not executed.
   */
  cleanup ();

  /* Since -exec and friends don't leave child processes running in the
   * background, there is no need to wait for them here.
   */
  exit (state.exit_status);	/* 0 for success, etc. */
}

bool
pred_regex (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  int len = strlen (pathname);
(void) stat_buf;
  if (re_match (pred_ptr->args.regex, pathname, len, 0,
		(struct re_registers *) NULL) == len)
    return (true);
  return (false);
}

bool
pred_size (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  uintmax_t f_val;

  (void) pathname;
  f_val = ((stat_buf->st_size / pred_ptr->args.size.blocksize)
	   + (stat_buf->st_size % pred_ptr->args.size.blocksize != 0));
  switch (pred_ptr->args.size.kind)
    {
    case COMP_GT:
      if (f_val > pred_ptr->args.size.size)
	return (true);
      break;
    case COMP_LT:
      if (f_val < pred_ptr->args.size.size)
	return (true);
      break;
    case COMP_EQ:
      if (f_val == pred_ptr->args.size.size)
	return (true);
      break;
    }
  return (false);
}

bool
pred_samefile (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  /* Potential optimisation: because of the loop protection, we always
   * know the device of the current directory, hence the device number
   * of the file we're currently considering.  If -L is not in effect,
   * and the device number of the file we're looking for is not the
   * same as the device number of the current directory, this
   * predicate cannot return true.  Hence there would be no need to
   * stat the file we're looking at.
   *
   * For the moment, we simply compare inode numbers, which should cut
   * down greatly on the number of calls to stat.  Some of the
   * remainder will be unnecessary, but the additional complexity
   * probably isn't worthwhile.
   */
  (void) pathname;

  /* We will often still have an fd open on the file under consideration,
   * but that's just to ensure inode number stability by maintaining
   * a reference to it; we don't need the file for anything else.
   */
  if (stat_buf->st_ino)
    {
      if (stat_buf->st_ino != pred_ptr->args.samefileid.ino)
	return false;
    }
  /* Now stat the file to check the device number. */
  if (0 == get_statinfo (pathname, state.rel_pathname, stat_buf))
    {
      /* the repeated test here is necessary in case stat_buf.st_ino had been zero. */
      return stat_buf->st_ino == pred_ptr->args.samefileid.ino
	&& stat_buf->st_dev == pred_ptr->args.samefileid.dev;
    }
  else
    {
      /* get_statinfo will already have emitted an error message. */
      return false;
    }
}

bool
pred_true (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  (void) stat_buf;
  (void) pred_ptr;
  return true;
}

bool
pred_type (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  mode_t mode;
  mode_t type = pred_ptr->args.type;

  assert (state.have_type);

  if (0 == state.type)
    {
      /* This can sometimes happen with broken NFS servers.
       * See Savannah bug #16378.
       */
      return false;
    }

  (void) pathname;

  if (state.have_stat)
     mode = stat_buf->st_mode;
  else
     mode = state.type;

#ifndef S_IFMT
  /* POSIX system; check `mode' the slow way. */
  if ((S_ISBLK (mode) && type == S_IFBLK)
      || (S_ISCHR (mode) && type == S_IFCHR)
      || (S_ISDIR (mode) && type == S_IFDIR)
      || (S_ISREG (mode) && type == S_IFREG)
#ifdef S_IFLNK
      || (S_ISLNK (mode) && type == S_IFLNK)
#endif
#ifdef S_IFIFO
      || (S_ISFIFO (mode) && type == S_IFIFO)
#endif
#ifdef S_IFSOCK
      || (S_ISSOCK (mode) && type == S_IFSOCK)
#endif
#ifdef S_IFDOOR
      || (S_ISDOOR (mode) && type == S_IFDOOR)
#endif
      )
#else /* S_IFMT */
  /* Unix system; check `mode' the fast way. */
  if ((mode & S_IFMT) == type)
#endif /* S_IFMT */
    return (true);
  else
    return (false);
}

bool
pred_uid (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  switch (pred_ptr->args.numinfo.kind)
    {
    case COMP_GT:
      if (stat_buf->st_uid > pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_LT:
      if (stat_buf->st_uid < pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    case COMP_EQ:
      if (stat_buf->st_uid == pred_ptr->args.numinfo.l_val)
	return (true);
      break;
    }
  return (false);
}

bool
pred_used (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  struct timespec delta, at, ct;

  (void) pathname;

  /* TODO: this needs to be retested carefully (manually, if necessary) */
  at = get_stat_atime (stat_buf);
  ct = get_stat_ctime (stat_buf);
  delta.tv_sec  = at.tv_sec  - ct.tv_sec;
  delta.tv_nsec = at.tv_nsec - ct.tv_nsec;
  if (delta.tv_nsec < 0)
    {
      delta.tv_nsec += 1000000000;
      delta.tv_sec  -=          1;
    }
  return pred_timewindow (delta, pred_ptr, DAYSECS);
}

bool
pred_user (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  (void) pathname;
  if (pred_ptr->args.uid == stat_buf->st_uid)
    return (true);
  else
    return (false);
}

bool
pred_xtype (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  struct stat sbuf;		/* local copy, not stat_buf because we're using a different stat method */
  int (*ystat) (const char*, struct stat *p);

  /* If we would normally stat the link itself, stat the target instead.
   * If we would normally follow the link, stat the link itself instead.
   */
  if (following_links ())
    ystat = optionp_stat;
  else
    ystat = optionl_stat;

  set_stat_placeholders (&sbuf);
  if ((*ystat) (state.rel_pathname, &sbuf) != 0)
    {
      if (following_links () && errno == ENOENT)
	{
	  /* If we failed to follow the symlink,
	   * fall back on looking at the symlink itself.
	   */
	  /* Mimic behavior of ls -lL. */
	  return (pred_type (pathname, stat_buf, pred_ptr));
	}
      else
	{
	  error (0, errno, "%s", safely_quote_err_filename (0, pathname));
	  state.exit_status = 1;
	}
      return false;
    }
  /* Now that we have our stat() information, query it in the same
   * way that -type does.
   */
  return (pred_type (pathname, &sbuf, pred_ptr));
}


bool
pred_context (const char *pathname, struct stat *stat_buf,
	      struct predicate *pred_ptr)
{
  security_context_t scontext;
  int rv = (*options.x_getfilecon) (state.cwd_dir_fd, state.rel_pathname,
				    &scontext);
  (void) stat_buf;

  if (rv < 0)
    {
      error (0, errno, _("getfilecon failed: %s"),
	     safely_quote_err_filename (0, pathname));
      return false;
    }

  rv = (fnmatch (pred_ptr->args.scontext, scontext, 0) == 0);
  freecon (scontext);
  return rv;
}

/* Copy STR into BUF and trim blanks from the end of BUF.
   Return BUF. */

static char *
blank_rtrim (const char *str, char *buf)
{
  int i;

  if (str == NULL)
    return (NULL);
  strcpy (buf, str);
  i = strlen (buf) - 1;
  while ((i >= 0) && ((buf[i] == ' ') || buf[i] == '\t'))
    i--;
  buf[++i] = '\0';
  return buf;
}

/* Print out the predicate list starting at NODE. */
void
print_list (FILE *fp, struct predicate *node)
{
  struct predicate *cur;
  char name[256];

  cur = node;
  while (cur != NULL)
    {
      fprintf (fp, "[%s] ", blank_rtrim (cur->p_name, name));
      cur = cur->pred_next;
    }
  fprintf (fp, "\n");
}

/* Print out the predicate list starting at NODE. */
static void
print_parenthesised (FILE *fp, struct predicate *node)
{
  int parens = 0;

  if (node)
    {
      if ((pred_is (node, pred_or) || pred_is (node, pred_and))
	  && node->pred_left == NULL)
	{
	  /* We print "<nothing> or  X" as just "X"
	   * We print "<nothing> and X" as just "X"
	   */
	  print_parenthesised(fp, node->pred_right);
	}
      else
	{
	  if (node->pred_left || node->pred_right)
	    parens = 1;

	  if (parens)
	    fprintf (fp, "%s", " ( ");
	  print_optlist (fp, node);
	  if (parens)
	    fprintf (fp, "%s", " ) ");
	}
    }
}

void
print_optlist (FILE *fp, const struct predicate *p)
{
  if (p)
    {
      print_parenthesised (fp, p->pred_left);
      fprintf (fp,
	       "%s%s%s",
	       p->need_stat ? "[call stat] " : "",
	       p->need_type ? "[need type] " : "",
	       p->need_inum ? "[need inum] " : "");
      print_predicate (fp, p);
      fprintf (fp, " [%g] ", p->est_success_rate);
      if (options.debug_options & DebugSuccessRates)
	{
	  fprintf (fp, "[%ld/%ld", p->perf.successes, p->perf.visits);
	  if (p->perf.visits)
	    {
	      double real_rate = (double)p->perf.successes / (double)p->perf.visits;
	      fprintf (fp, "=%g] ", real_rate);
	    }
	  else
	    {
	      fprintf (fp, "=_] ");
	    }
	}
      print_parenthesised (fp, p->pred_right);
    }
}

void show_success_rates (const struct predicate *p)
{
  if (options.debug_options & DebugSuccessRates)
    {
      fprintf (stderr, "Predicate success rates after completion:\n");
      print_optlist (stderr, p);
      fprintf (stderr, "\n");
    }
}




#ifdef _NDEBUG
/* If _NDEBUG is defined, the assertions will do nothing.   Hence
 * there is no point in having a function body for pred_sanity_check()
 * if that preprocessor macro is defined.
 */
void
pred_sanity_check (const struct predicate *predicates)
{
  /* Do nothing, since assert is a no-op with _NDEBUG set */
  return;
}
#else
void
pred_sanity_check (const struct predicate *predicates)
{
  const struct predicate *p;

  for (p=predicates; p != NULL; p=p->pred_next)
    {
      /* All predicates must do something. */
      assert (p->pred_func != NULL);

      /* All predicates must have a parser table entry. */
      assert (p->parser_entry != NULL);

      /* If the parser table tells us that just one predicate function is
       * possible, verify that that is still the one that is in effect.
       * If the parser has NULL for the predicate function, that means that
       * the parse_xxx function fills it in, so we can't check it.
       */
      if (p->parser_entry->pred_func)
	{
	  assert (p->parser_entry->pred_func == p->pred_func);
	}

      switch (p->parser_entry->type)
	{
	  /* Options all take effect during parsing, so there should
	   * be no predicate entries corresponding to them.  Hence we
	   * should not see any ARG_OPTION or ARG_POSITIONAL_OPTION
	   * items.
	   *
	   * This is a silly way of coding this test, but it prevents
	   * a compiler warning (i.e. otherwise it would think that
	   * there would be case statements missing).
	   */
	case ARG_OPTION:
	case ARG_POSITIONAL_OPTION:
	  assert (p->parser_entry->type != ARG_OPTION);
	  assert (p->parser_entry->type != ARG_POSITIONAL_OPTION);
	  break;

	case ARG_ACTION:
	  assert (p->side_effects); /* actions have side effects. */
	  if (!pred_is (p, pred_prune) && !pred_is(p, pred_quit))
	    {
	      /* actions other than -prune and -quit should
	       * inhibit the default -print
	       */
	      assert (p->no_default_print);
	    }
	  break;

	/* We happen to know that the only user of ARG_SPECIAL_PARSE
	 * is a test, so handle it like ARG_TEST.
	 */
	case ARG_SPECIAL_PARSE:
	case ARG_TEST:
	case ARG_PUNCTUATION:
	case ARG_NOOP:
	  /* Punctuation and tests should have no side
	   * effects and not inhibit default print.
	   */
	  assert (!p->no_default_print);
	  assert (!p->side_effects);
	  break;
	}
    }
}
#endif
