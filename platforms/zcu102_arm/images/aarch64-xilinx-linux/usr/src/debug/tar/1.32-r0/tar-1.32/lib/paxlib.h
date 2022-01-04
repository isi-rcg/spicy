/* This file is part of GNU paxutils

   Copyright (C) 1994, 1995, 1996, 1997, 1998, 1999, 2000, 2001, 2003,
   2005, 2007 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#ifndef _paxlib_h_
#define _paxlib_h_

#include <inttostr.h>

/* Error reporting functions and definitions */

/* Exit status for paxutils app.  Let's try to keep this list as simple as
   possible. tar -d option strongly invites a status different for unequal
   comparison and other errors.  */
#define PAXEXIT_SUCCESS 0
#define PAXEXIT_DIFFERS 1
#define PAXEXIT_FAILURE 2

extern void (*error_hook) (void);

/* Both WARN and ERROR write a message on stderr and continue processing,
   however ERROR manages so tar will exit unsuccessfully.  FATAL_ERROR
   writes a message on stderr and aborts immediately, with another message
   line telling so.  USAGE_ERROR works like FATAL_ERROR except that the
   other message line suggests trying --help.  All four macros accept a
   single argument of the form ((0, errno, _("FORMAT"), Args...)).  errno
   is zero when the error is not being detected by the system.  */

#define WARN(Args) \
  do { if (error_hook) error_hook (); error Args; } while (0)
#define ERROR(Args) \
  do						\
    {						\
      if (error_hook) error_hook ();		\
      error Args;				\
      exit_status = PAXEXIT_FAILURE;		\
    }						\
  while (0)
#define FATAL_ERROR(Args) \
  do						\
    {						\
      if (error_hook) error_hook ();		\
      error Args;				\
      fatal_exit ();				\
    }						\
  while (0)
#define USAGE_ERROR(Args) \
  do						\
    {						\
      if (error_hook) error_hook ();		\
      error Args;				\
      usage (PAXEXIT_FAILURE);			\
    }						\
  while (0)

extern int exit_status;

void pax_decode_mode (mode_t mode, char *string);
void call_arg_error (char const *call, char const *name);
void call_arg_fatal (char const *call, char const *name) __attribute__ ((noreturn));
void call_arg_warn (char const *call, char const *name);
void chmod_error_details (char const *name, mode_t mode);
void chown_error_details (char const *name, uid_t uid, gid_t gid);

void decode_mode (mode_t, char *);

void chdir_fatal (char const *) __attribute__ ((noreturn));
void chmod_error_details (char const *, mode_t);
void chown_error_details (char const *, uid_t, gid_t);
void close_error (char const *);
void close_warn (char const *);
void exec_fatal (char const *) __attribute__ ((noreturn));
void link_error (char const *, char const *);
void mkdir_error (char const *);
void mkfifo_error (char const *);
void mknod_error (char const *);
void open_error (char const *);
void open_fatal (char const *) __attribute__ ((noreturn));
void open_warn (char const *);
void read_error (char const *);
void read_error_details (char const *, off_t, size_t);
void read_fatal (char const *) __attribute__ ((noreturn));
void read_fatal_details (char const *, off_t, size_t) __attribute__ ((noreturn));
void read_warn_details (char const *, off_t, size_t);
void readlink_error (char const *);
void readlink_warn (char const *);
void rmdir_error (char const *);
void savedir_error (char const *);
void savedir_warn (char const *);
void seek_error (char const *);
void seek_error_details (char const *, off_t);
void seek_warn (char const *);
void seek_warn_details (char const *, off_t);
void stat_fatal (char const *) __attribute__ ((noreturn));
void stat_error (char const *);
void stat_warn (char const *);
void symlink_error (char const *, char const *);
void truncate_error (char const *);
void truncate_warn (char const *);
void unlink_error (char const *);
void utime_error (char const *);
void waitpid_error (char const *);
void write_error (char const *);
void write_error_details (char const *, size_t, size_t);

void pax_exit (void) __attribute__ ((noreturn));
void fatal_exit (void) __attribute__ ((noreturn));

#define STRINGIFY_BIGINT(i, b) umaxtostr (i, b)


/* Name-related functions */
bool removed_prefixes_p (void);
char *safer_name_suffix (char const *file_name, bool link_target, bool absolute_names);

#endif
