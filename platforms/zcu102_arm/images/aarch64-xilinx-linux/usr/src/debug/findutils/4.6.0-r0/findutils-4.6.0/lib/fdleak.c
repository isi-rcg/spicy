/* fdleak.c -- detect file descriptor leaks
   Copyright (C) 2010, 2011 Free Software Foundation, Inc.

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
#include <fcntl.h>
#include <limits.h>
#include <poll.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#if HAVE_GETRLIMIT
# include <sys/resource.h>
#endif
#include <unistd.h>

/* gnulib headers. */
#include "cloexec.h"
#include "dirent-safer.h"
#include "error.h"
#include "fcntl--.h"
#include "gettext.h"

/* find headers. */
#include "extendbuf.h"
#include "fdleak.h"
#include "safe-atoi.h"

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

/* In order to detect FD leaks, we take a snapshot of the open
 * file descriptors which are not FD_CLOEXEC when the program starts.
 * When the program exits, we discover if there are any new
 * file descriptors which aren't FD_CLOEXEC.
 */
static int *non_cloexec_fds;
static size_t num_cloexec_fds;


/* Determine the value of the largest open fd, on systems that
 * offer /proc/self/fd. */
static int
get_proc_max_fd (void)
{
  const char *path = "/proc/self/fd";
  int maxfd = -1;
  /* We don't use readdir_r, because we cannot trust pathconf
   * to tell us the maximum possible length of a path in
   * a given directory (the manpage for readdir_r claims this
   * is the approved method, but the manpage for pathconf indicates
   * that _PC_NAME_MAX is not an upper limit). */
  DIR *dir = opendir_safer (path);
  if (dir)
    {
      int good = 0;
      struct dirent *dent;

      while ((dent=readdir (dir)) != NULL)
	{
	  if (dent->d_name[0] != '.'
	      || (dent->d_name[0] != 0
		  && dent->d_name[1] != 0 && dent->d_name[1] != '.'))
	    {
	      const int fd = safe_atoi (dent->d_name, literal_quoting_style);
	      if (fd > maxfd)
		maxfd = fd;
	      good = 1;
	    }
	}
      closedir (dir);
      if (good)
	return maxfd;
    }
  return -1;
}



/* Estimate the value of the largest possible file descriptor */
static int
get_max_fd (void)
{
  struct rlimit fd_limit;
  long open_max;

  open_max = get_proc_max_fd ();
  if (open_max >= 0)
    return open_max;

  open_max = sysconf (_SC_OPEN_MAX);
  if (open_max == -1)
    open_max = _POSIX_OPEN_MAX;	/* underestimate */

  /* We assume if RLIMIT_NOFILE is defined, all the related macros are, too. */
#if defined HAVE_GETRLIMIT && defined RLIMIT_NOFILE
  if (0 == getrlimit (RLIMIT_NOFILE, &fd_limit))
    {
      if (fd_limit.rlim_cur == RLIM_INFINITY)
	return open_max;
      else
	return (int) fd_limit.rlim_cur;
    }
#endif
  /* cannot determine the limit's value */
  return open_max;
}


static int
visit_open_fds (int fd_min, int fd_max,
		int (*callback)(int, void*), void *cb_context)
{
  enum { MAX_POLL = 64 };
  struct pollfd pf[MAX_POLL];
  int rv = 0;

  while (fd_min < fd_max)
    {
      int i;
      int limit = fd_max - fd_min;
      if (limit > MAX_POLL)
	limit = MAX_POLL;

      for (i=0; i<limit; i++)
	{
	  pf[i].events = POLLIN|POLLOUT;
	  pf[i].revents = 0;
	  pf[i].fd = fd_min + i;
	}
      rv = poll (pf, limit, 0);
      if (-1 == rv)
	{
	  return -1;
	}
      else
	{
	  int j;
	  for (j=0; j<limit; j++)
	    {
	      if (pf[j].revents != POLLNVAL)
		{
		  if (0 != (rv = callback (pf[j].fd, cb_context)))
		    return rv;
		}
	    }
	}
      fd_min += limit;
    }
  return 0;
}

static int
fd_is_cloexec (int fd)
{
  const int flags = fcntl (fd, F_GETFD);
  return flags & FD_CLOEXEC;
}


/* Faking closures in C is a bit of a pain.  */
struct remember_fd_context
{
  int *buf;
  size_t used;
  size_t allocated;
};


/* Record FD is it's not FD_CLOEXEC. */
static int
remember_fd_if_non_cloexec (int fd, void *context)
{
  if (fd_is_cloexec (fd))
    {
      return 0;
    }
  else
    {
      struct remember_fd_context * const p = context;
      void *newbuf = extendbuf (p->buf,
				sizeof (p->buf[0])*(p->used+1),
				&(p->allocated));
      if (newbuf)
	{
	  p->buf = newbuf;
	  p->buf[p->used] = fd;
	  ++p->used;
	  return 0;
	}
      else
	{
	  return -1;
	}
    }
}

void
remember_non_cloexec_fds (void)
{
  int max_fd = get_max_fd ();
  struct remember_fd_context cb_data;
  cb_data.buf = NULL;
  cb_data.used = cb_data.allocated = 0;

  if (max_fd < INT_MAX)
    ++max_fd;
  visit_open_fds (0, max_fd, remember_fd_if_non_cloexec, &cb_data);

  non_cloexec_fds = cb_data.buf;
  num_cloexec_fds = cb_data.used;
}


struct fd_leak_context
{
  const int *prev_buf;
  size_t used;
  size_t lookup_pos;
  int leaked_fd;
};

/* FD is open and not close-on-exec.
 * If it's not in the list of non-cloexec file descriptors we saw before, it's a leak.
 */
static int
find_first_leak_callback (int fd, void *context)
{
  if (!fd_is_cloexec (fd))
    {
      struct fd_leak_context *p = context;
      while (p->lookup_pos < p->used)
	{
	  if (p->prev_buf[p->lookup_pos] < fd)
	    {
	      ++p->lookup_pos;
	    }
	  else if (p->prev_buf[p->lookup_pos] == fd)
	    {
	      /* FD was open and still is, it's not a leak. */
	      return 0;
	    }
	  else
	    {
	      break;
	    }
	}
      /* We come here if p->prev_buf[p->lookup_pos] > fd, or
	 if we ran out of items in the lookup table.
	 Either way, this is a leak. */
      p->leaked_fd = fd;
      return -1;		/* No more callbacks needed. */
    }
  return 0;
}


static int
find_first_leaked_fd (const int* prev_non_cloexec_fds, size_t n)
{
  struct fd_leak_context context;
  int max_fd = get_max_fd ();

  if (max_fd < INT_MAX)
    ++max_fd;
  context.prev_buf = prev_non_cloexec_fds;
  context.used = n;
  context.lookup_pos = 0;
  context.leaked_fd = -1;
  visit_open_fds (0, max_fd, find_first_leak_callback, &context);
  return context.leaked_fd;
}

/* Determine if O_CLOEXEC actually works (Savannah bug #29435:
   fd_is_cloexec () does not work on Fedora buildhosts).
 */
static bool
o_cloexec_works (void)
{
  bool result = false;
  int fd = open ("/", O_RDONLY|O_CLOEXEC);
  if (fd >= 0)
    {
      result = fd_is_cloexec (fd);
      close (fd);
    }
  return result;
}


int
open_cloexec (const char *path, int flags, ...)
{
  int fd;
  mode_t mode = 0;
  static bool cloexec_works = false;
  static bool cloexec_status_known = false;

  if (flags & O_CREAT)
    {
      /* this code is copied from gnulib's open-safer.c. */
      va_list ap;
      va_start (ap, flags);

      /* We have to use PROMOTED_MODE_T instead of mode_t, otherwise GCC 4
         creates crashing code when 'mode_t' is smaller than 'int'.  */
      mode = va_arg (ap, PROMOTED_MODE_T);

      va_end (ap);
    }

  /* Kernels usually ignore open flags they don't recognise, so it
   * is possible this program was built against a library which
   * defines O_CLOEXEC, but is running on a kernel that (silently)
   * does not recognise it.   We figure this out by just trying it,
   * once.
   */
  if (!cloexec_status_known)
    {
      cloexec_works = o_cloexec_works ();
      cloexec_status_known = true;
    }
  fd = open (path, flags|O_CLOEXEC, mode);
  if ((fd >= 0) && !(O_CLOEXEC && cloexec_works))
    {
      set_cloexec_flag (fd, true);
    }
  return fd;
}

void
forget_non_cloexec_fds (void)
{
  free (non_cloexec_fds);
  non_cloexec_fds = NULL;
  num_cloexec_fds = 0;
}


void
complain_about_leaky_fds (void)
{
  int no_leaks = 1;
  const int leaking_fd = find_first_leaked_fd (non_cloexec_fds, num_cloexec_fds);

  if (leaking_fd >= 0)
    {
      no_leaks = 0;
      error (0, 0,
	     _("File descriptor %d will leak; please report this as a bug, "
	       "remembering to include a detailed description of the simplest "
	       "way to reproduce this problem."),
	     leaking_fd);
    }
  assert (no_leaks);
}
