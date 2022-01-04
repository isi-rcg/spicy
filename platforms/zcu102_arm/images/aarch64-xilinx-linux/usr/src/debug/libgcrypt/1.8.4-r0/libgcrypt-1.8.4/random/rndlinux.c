/* rndlinux.c  -  raw random number for OSes with /dev/random
 * Copyright (C) 1998, 2001, 2002, 2003, 2007,
 *               2009  Free Software Foundation, Inc.
 *
 * This file is part of Libgcrypt.
 *
 * Libgcrypt is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Libgcrypt is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef HAVE_GETTIMEOFDAY
# include <sys/times.h>
#endif
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(__linux__) && defined(HAVE_SYSCALL)
# include <sys/syscall.h>
#endif

#include "types.h"
#include "g10lib.h"
#include "rand-internal.h"

static int open_device (const char *name, int retry);


static int
set_cloexec_flag (int fd)
{
  int oldflags;

  oldflags= fcntl (fd, F_GETFD, 0);
  if (oldflags < 0)
    return oldflags;
  oldflags |= FD_CLOEXEC;
  return fcntl (fd, F_SETFD, oldflags);
}



/*
 * Used to open the /dev/random devices (Linux, xBSD, Solaris (if it
 * exists)).  If RETRY is true, the function does not terminate with
 * a fatal error but retries until it is able to reopen the device.
 */
static int
open_device (const char *name, int retry)
{
  int fd;

  if (retry)
    _gcry_random_progress ("open_dev_random", 'X', 1, 0);
 again:
  fd = open (name, O_RDONLY);
  if (fd == -1 && retry)
    {
      struct timeval tv;

      tv.tv_sec = 5;
      tv.tv_usec = 0;
      _gcry_random_progress ("wait_dev_random", 'X', 0, (int)tv.tv_sec);
      select (0, NULL, NULL, NULL, &tv);
      goto again;
    }
  if (fd == -1)
    log_fatal ("can't open %s: %s\n", name, strerror(errno) );

  if (set_cloexec_flag (fd))
    log_error ("error setting FD_CLOEXEC on fd %d: %s\n",
               fd, strerror (errno));

  /* We used to do the following check, however it turned out that this
     is not portable since more OSes provide a random device which is
     sometimes implemented as another device type.

     struct stat sb;

     if( fstat( fd, &sb ) )
        log_fatal("stat() off %s failed: %s\n", name, strerror(errno) );
     if( (!S_ISCHR(sb.st_mode)) && (!S_ISFIFO(sb.st_mode)) )
        log_fatal("invalid random device!\n" );
  */
  return fd;
}


/* Note that the caller needs to make sure that this function is only
 * called by one thread at a time.  The function returns 0 on success
 * or true on failure (in which case the caller will signal a fatal
 * error).  This function should be entered only by one thread at a
 * time. */
int
_gcry_rndlinux_gather_random (void (*add)(const void*, size_t,
                                          enum random_origins),
                              enum random_origins origin,
                              size_t length, int level )
{
  static int fd_urandom = -1;
  static int fd_random = -1;
  static int only_urandom = -1;
  static unsigned char ever_opened;
  static volatile pid_t my_pid; /* The volatile is there to make sure
                                 * the compiler does not optimize the
                                 * code away in case the getpid
                                 * function is badly attributed. */
  volatile pid_t apid;
  int fd;
  int n;
  byte buffer[768];
  size_t n_hw;
  size_t want = length;
  size_t last_so_far = 0;
  int any_need_entropy = 0;
  int delay;

  /* On the first call read the conf file to check whether we want to
   * use only urandom.  */
  if (only_urandom == -1)
    {
      my_pid = getpid ();
      if ((_gcry_random_read_conf () & RANDOM_CONF_ONLY_URANDOM))
        only_urandom = 1;
      else
        only_urandom = 0;
    }

  if (!add)
    {
      /* Special mode to close the descriptors.  */
      if (fd_random != -1)
        {
          close (fd_random);
          fd_random = -1;
        }
      if (fd_urandom != -1)
        {
          close (fd_urandom);
          fd_urandom = -1;
        }
      return 0;
    }

  /* Detect a fork and close the devices so that we don't use the old
   * file descriptors.  Note that open_device will be called in retry
   * mode if the devices was opened by the parent process.  */
  apid = getpid ();
  if (my_pid != apid)
    {
      if (fd_random != -1)
        {
          close (fd_random);
          fd_random = -1;
        }
      if (fd_urandom != -1)
        {
          close (fd_urandom);
          fd_urandom = -1;
        }
      my_pid = apid;
    }


  /* First read from a hardware source.  However let it account only
     for up to 50% (or 25% for RDRAND) of the requested bytes.  */
  n_hw = _gcry_rndhw_poll_slow (add, origin);
  if ((_gcry_get_hw_features () & HWF_INTEL_RDRAND))
    {
      if (n_hw > length/4)
        n_hw = length/4;
    }
  else
    {
      if (n_hw > length/2)
        n_hw = length/2;
    }
  if (length > 1)
    length -= n_hw;

  /* When using a blocking random generator try to get some entropy
   * from the jitter based RNG.  In this case we take up to 50% of the
   * remaining requested bytes.  */
  if (level >= GCRY_VERY_STRONG_RANDOM)
    {
      n_hw = _gcry_rndjent_poll (add, origin, length/2);
      if (n_hw > length/2)
        n_hw = length/2;
      if (length > 1)
        length -= n_hw;
    }


  /* Open the requested device.  The first time a device is to be
     opened we fail with a fatal error if the device does not exists.
     In case the device has ever been closed, further open requests
     will however retry indefinitely.  The rationale for this behaviour is
     that we always require the device to be existent but want a more
     graceful behaviour if the rarely needed close operation has been
     used and the device needs to be re-opened later. */
  if (level >= GCRY_VERY_STRONG_RANDOM && !only_urandom)
    {
      if (fd_random == -1)
        {
          fd_random = open_device (NAME_OF_DEV_RANDOM, (ever_opened & 1));
          ever_opened |= 1;
        }
      fd = fd_random;
    }
  else
    {
      if (fd_urandom == -1)
        {
          fd_urandom = open_device (NAME_OF_DEV_URANDOM, (ever_opened & 2));
          ever_opened |= 2;
        }
      fd = fd_urandom;
    }

  /* Enter the read loop.  */
  delay = 0;  /* Start with 0 seconds so that we do no block on the
                 first iteration and in turn call the progress function
                 before blocking.  To give the OS a better chance to
                 return with something we will actually use 100ms. */
  while (length)
    {
      fd_set rfds;
      struct timeval tv;
      int rc;

      /* If we have a modern Linux kernel, we first try to use the new
       * getrandom syscall.  That call guarantees that the kernel's
       * RNG has been properly seeded before returning any data.  This
       * is different from /dev/urandom which may, due to its
       * non-blocking semantics, return data even if the kernel has
       * not been properly seeded.  And it differs from /dev/random by never
       * blocking once the kernel is seeded. Unfortunately we need to use a
       * syscall and not a new device and thus we are not able to use
       * select(2) to have a timeout. */
#if defined(__linux__) && defined(HAVE_SYSCALL) && defined(__NR_getrandom)
        {
          long ret;
          size_t nbytes;

          do
            {
              nbytes = length < sizeof(buffer)? length : sizeof(buffer);
              if (nbytes > 256)
                nbytes = 256;
              _gcry_pre_syscall ();
              ret = syscall (__NR_getrandom,
                             (void*)buffer, (size_t)nbytes, (unsigned int)0);
              _gcry_post_syscall ();
            }
          while (ret == -1 && errno == EINTR);
          if (ret == -1 && errno == ENOSYS)
            ; /* The syscall is not supported - fallback to pulling from fd.  */
          else
            { /* The syscall is supported.  Some sanity checks.  */
              if (ret == -1)
                log_fatal ("unexpected error from getrandom: %s\n",
                           strerror (errno));
              else if (ret != nbytes)
                log_fatal ("getrandom returned only"
                           " %ld of %zu requested bytes\n", ret, nbytes);

              (*add)(buffer, nbytes, origin);
              length -= nbytes;
              continue; /* until LENGTH is zero.  */
            }
        }
#endif

      /* If we collected some bytes update the progress indicator.  We
         do this always and not just if the select timed out because
         often just a few bytes are gathered within the timeout
         period.  */
      if (any_need_entropy || last_so_far != (want - length) )
        {
          last_so_far = want - length;
          _gcry_random_progress ("need_entropy", 'X',
                                 (int)last_so_far, (int)want);
          any_need_entropy = 1;
        }

      /* If the system has no limit on the number of file descriptors
         and we encounter an fd which is larger than the fd_set size,
         we don't use the select at all.  The select code is only used
         to emit progress messages.  A better solution would be to
         fall back to poll() if available.  */
#ifdef FD_SETSIZE
      if (fd < FD_SETSIZE)
#endif
        {
          FD_ZERO(&rfds);
          FD_SET(fd, &rfds);
          tv.tv_sec = delay;
          tv.tv_usec = delay? 0 : 100000;
          _gcry_pre_syscall ();
          rc = select (fd+1, &rfds, NULL, NULL, &tv);
          _gcry_post_syscall ();
          if (!rc)
            {
              any_need_entropy = 1;
              delay = 3; /* Use 3 seconds henceforth.  */
              continue;
            }
          else if( rc == -1 )
            {
              log_error ("select() error: %s\n", strerror(errno));
              if (!delay)
                delay = 1; /* Use 1 second if we encounter an error before
                              we have ever blocked.  */
              continue;
            }
        }

      do
        {
          size_t nbytes;

          nbytes = length < sizeof(buffer)? length : sizeof(buffer);
          n = read (fd, buffer, nbytes);
          if (n >= 0 && n > nbytes)
            {
              log_error("bogus read from random device (n=%d)\n", n );
              n = nbytes;
            }
        }
      while (n == -1 && errno == EINTR);
      if  (n == -1)
        log_fatal("read error on random device: %s\n", strerror(errno));
      (*add)(buffer, n, origin);
      length -= n;
    }
  wipememory (buffer, sizeof buffer);

  if (any_need_entropy)
    _gcry_random_progress ("need_entropy", 'X', (int)want, (int)want);

  return 0; /* success */
}
