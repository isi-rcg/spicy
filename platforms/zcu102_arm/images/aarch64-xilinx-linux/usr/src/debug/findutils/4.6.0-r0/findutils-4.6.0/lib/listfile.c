/* listfile.c -- display a long listing of a file
   Copyright (C) 1991, 1993, 2000, 2004, 2005, 2007, 2008, 2010, 2011
   Free Software Foundation, Inc.

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
#include <alloca.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <locale.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h> /* for readlink() */

/* gnulib headers. */
#include "areadlink.h"
#include "error.h"
#include "filemode.h"
#include "human.h"
#include "mbswidth.h"
#include "idcache.h"
#include "pathmax.h"
#include "stat-size.h"
#include "gettext.h"

/* find headers. */
#include "listfile.h"

/* Since major is a function on SVR4, we can't use `ifndef major'.  */
#ifdef MAJOR_IN_MKDEV
#include <sys/mkdev.h>
#define HAVE_MAJOR
#endif
#ifdef MAJOR_IN_SYSMACROS
#include <sys/sysmacros.h>
#define HAVE_MAJOR
#endif

#ifdef major                    /* Might be defined in sys/types.h.  */
#define HAVE_MAJOR
#endif
#ifndef HAVE_MAJOR
#define major(dev)  (((dev) >> 8) & 0xff)
#define minor(dev)  ((dev) & 0xff)
#endif
#undef HAVE_MAJOR

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

static bool print_name (register const char *p, FILE *stream, int literal_control_chars);

/* We have some minimum field sizes, though we try to widen these fields on systems
 * where we discover examples where the field width we started with is not enough. */
static int inode_number_width = 9;
static int block_size_width = 6;
static int nlink_width = 3;
static int owner_width = 8;
static int group_width = 8;
/* We don't print st_author even if the system has it. */
static int major_device_number_width = 3;
static int minor_device_number_width = 3;
static int file_size_width = 8;

static bool print_num(FILE *stream, unsigned long num, int *width)
{
  const int chars_out = fprintf (stream, "%*lu", *width, num);
  if (chars_out >= 0)
    {
      if (*width < chars_out)
        *width = chars_out;
      return true;
    }
  return false;
}


/* NAME is the name to print.
   RELNAME is the path to access it from the current directory.
   STATP is the results of stat or lstat on it.
   Use CURRENT_TIME to decide whether to print yyyy or hh:mm.
   Use OUTPUT_BLOCK_SIZE to determine how to print file block counts
   and sizes.
   STREAM is the stdio stream to print on.  */

void
list_file (const char *name,
           int dir_fd,
           char *relname,
           const struct stat *statp,
           time_t current_time,
           int output_block_size,
           int literal_control_chars,
           FILE *stream)
{
  char modebuf[12];
  struct tm const *when_local;
  char const *user_name;
  char const *group_name;
  char hbuf[LONGEST_HUMAN_READABLE + 1];
  bool output_good = true;
  int chars_out;
  int failed_at = 000;
  int inode_field_width;

#if HAVE_ST_DM_MODE
  /* Cray DMF: look at the file's migrated, not real, status */
  strmode (statp->st_dm_mode, modebuf);
#else
  strmode (statp->st_mode, modebuf);
#endif

  chars_out = fprintf (stream, "%*s", inode_number_width,
                       human_readable ((uintmax_t) statp->st_ino, hbuf,
                                       human_ceiling,
                                       1u, 1u));
  if (chars_out < 0)
    {
      output_good = false;
      failed_at = 100;
    }
  else if (chars_out > inode_number_width)
    {
      inode_number_width = chars_out;
    }
  if (output_good)
    {
      if (EOF == putc(' ', stream))
        {
          output_good = false;
          failed_at = 150;
        }
      chars_out = fprintf (stream, "%*s",
                           block_size_width,
                           human_readable ((uintmax_t) ST_NBLOCKS (*statp), hbuf,
                                           human_ceiling,
                                           ST_NBLOCKSIZE, output_block_size));
      if (chars_out < 0)
        {
          output_good = false;
          failed_at = 200;
        }
      else
        {
          if (chars_out > block_size_width)
            block_size_width = chars_out;
        }
    }

  if (output_good)
    {
      if (EOF == putc(' ', stream))
        {
          output_good = false;
          failed_at = 250;
        }
      /* modebuf includes the space between the mode and the number of links,
         as the POSIX "optional alternate access method flag".  */
      if (fprintf (stream, "%s%3lu ", modebuf, (unsigned long) statp->st_nlink) < 0)
        {
          output_good = false;
          failed_at = 300;
        }
    }

  if (output_good)
    {
      if (EOF == putc(' ', stream))
        {
          output_good = false;
          failed_at = 250;
        }
      user_name = getuser (statp->st_uid);
      if (user_name)
        {
          int len = mbswidth (user_name, 0);
          if (len > owner_width)
            owner_width = len;
          output_good = (fprintf (stream, "%-*s ", owner_width, user_name) >= 0);
          if (!output_good)
            failed_at = 400;
        }
      else
        {
          chars_out = fprintf (stream, "%-8lu ", (unsigned long) statp->st_uid);
          if (chars_out > owner_width)
            owner_width = chars_out;
          output_good = (chars_out > 0);
          if (!output_good)
            failed_at = 450;
        }
    }

  if (output_good)
    {
      group_name = getgroup (statp->st_gid);
      if (group_name)
        {
          int len = mbswidth (group_name, 0);
          if (len > group_width)
            group_width = len;
          output_good = (fprintf (stream, "%-*s ", group_width, group_name) >= 0);
          if (!output_good)
            failed_at = 500;
        }
      else
        {
          chars_out = fprintf (stream, "%-*lu",
                               group_width, (unsigned long) statp->st_gid);
          if (chars_out > group_width)
            group_width = chars_out;
          output_good = (chars_out >= 0);
          if (output_good)
            {
              if (EOF == putc(' ', stream))
                {
                  output_good = false;
                  failed_at = 525;
                }
            }
          else
            {
              if (!output_good)
                failed_at = 550;
            }
        }
    }

  if (output_good)
    {
      if (S_ISCHR (statp->st_mode) || S_ISBLK (statp->st_mode))
        {
#ifdef HAVE_STRUCT_STAT_ST_RDEV
          if (!print_num (stream,
                          (unsigned long) major (statp->st_rdev),
                          &major_device_number_width))
            {
              output_good = false;
              failed_at = 600;
            }
          if (output_good)
            {
              if (fprintf (stream, ", ") < 0)
                {
                  output_good = false;
                  failed_at = 625;
                }
            }
          if (output_good)
            {
              if (!print_num (stream,
                              (unsigned long) minor (statp->st_rdev),
                              &minor_device_number_width))
                {
                  output_good = false;
                  failed_at = 650;
                }
            }
#else
          if (fprintf (stream, "%*s  %*s",
                       major_device_number_width,
                       minor_device_number_width) < 0)
            {
              output_good = false;
              failed_at = 700;
            }
#endif
        }
      else
        {
          const int blocksize = output_block_size < 0 ? output_block_size : 1;
          chars_out = fprintf (stream, "%*s",
                               file_size_width,
                               human_readable ((uintmax_t) statp->st_size, hbuf,
                                               human_ceiling,
                                               1, blocksize));
          if (chars_out < 0)
            {
              output_good = false;
              failed_at = 800;
            }
          else
            {
              if (chars_out > file_size_width)
                {
                  file_size_width = chars_out;
                }
            }
        }
    }

  if (output_good)
    {
      if (EOF == putc(' ', stream))
        {
          output_good = false;
          failed_at = 850;
        }
    }

  if (output_good)
    {
      if ((when_local = localtime (&statp->st_mtime)))
        {
          char init_bigbuf[256];
          char *buf = init_bigbuf;
          size_t bufsize = sizeof init_bigbuf;

          /* Use strftime rather than ctime, because the former can produce
             locale-dependent names for the month (%b).

             Output the year if the file is fairly old or in the future.
             POSIX says the cutoff is 6 months old;
             approximate this by 6*30 days.
             Allow a 1 hour slop factor for what is considered "the future",
             to allow for NFS server/client clock disagreement.  */
          char const *fmt =
            ((current_time - 6 * 30 * 24 * 60 * 60 <= statp->st_mtime
              && statp->st_mtime <= current_time + 60 * 60)
             ? "%b %e %H:%M"
             : "%b %e  %Y");

          while (!strftime (buf, bufsize, fmt, when_local))
            buf = alloca (bufsize *= 2);

          if (fprintf (stream, "%s ", buf) < 0)
            {
              output_good = false;
              failed_at = 900;
            }
        }
      else
        {
          /* The time cannot be represented as a local time;
             print it as a huge integer number of seconds.  */
          int width = 12;

          if (statp->st_mtime < 0)
            {
              char const *num = human_readable (- (uintmax_t) statp->st_mtime,
                                                hbuf, human_ceiling, 1, 1);
              int sign_width = width - strlen (num);
              if (fprintf (stream, "%*s%s ",
                           sign_width < 0 ? 0 : sign_width, "-", num) < 0)
                {
                  output_good = false;
                  failed_at = 1000;
                }
            }
          else
            {
              if (fprintf (stream, "%*s ", width,
                           human_readable ((uintmax_t) statp->st_mtime, hbuf,
                                           human_ceiling,
                                           1, 1)) < 0)
                {
                  output_good = false;
                  failed_at = 1100;
                }
            }
        }
    }

  if (output_good)
    {
      output_good = print_name (name, stream, literal_control_chars);
      if (!output_good)
        {
          failed_at = 1200;
        }
    }

  if (output_good)
    {
      if (S_ISLNK (statp->st_mode))
        {
          char *linkname = areadlinkat (dir_fd, relname);
          if (linkname)
            {
              if (fputs (" -> ", stream) < 0)
                {
                  output_good = false;
                  failed_at = 1300;
                }
              if (output_good)
                {
                  output_good = print_name (linkname, stream, literal_control_chars);
                  if (!output_good)
                    {
                      failed_at = 1350;
                    }
                }
            }
          else
            {
              /* POSIX requires in the case of find that if we issue a
               * diagnostic we should have a nonzero status.  However,
               * this function doesn't have a way of telling the caller to
               * do that.  However, since this function is only used when
               * processing "-ls", we're already using an extension.
               */
              error (0, errno, "%s", name);
            }
          free (linkname);
        }
      if (output_good)
        {
          if (EOF == putc ('\n', stream))
            {
              output_good = false;
              if (!output_good)
                {
                  failed_at = 1400;
                }
            }
        }
    }
  if (!output_good)
    {
      error (EXIT_FAILURE, errno, _("Failed to write output (at stage %d)"), failed_at);
    }
}


static bool
print_name_without_quoting (const char *p, FILE *stream)
{
  return (fprintf (stream, "%s", p) >= 0);
}


static bool
print_name_with_quoting (register const char *p, FILE *stream)
{
  register unsigned char c;

  while ((c = *p++) != '\0')
    {
      int fprintf_result = -1;
      switch (c)
        {
        case '\\':
          fprintf_result = fprintf (stream, "\\\\");
          break;

        case '\n':
          fprintf_result = fprintf (stream, "\\n");
          break;

        case '\b':
          fprintf_result = fprintf (stream, "\\b");
          break;

        case '\r':
          fprintf_result = fprintf (stream, "\\r");
          break;

        case '\t':
          fprintf_result = fprintf (stream, "\\t");
          break;

        case '\f':
          fprintf_result = fprintf (stream, "\\f");
          break;

        case ' ':
          fprintf_result = fprintf (stream, "\\ ");
          break;

        case '"':
          fprintf_result = fprintf (stream, "\\\"");
          break;

        default:
          if (c > 040 && c < 0177)
            {
              if (EOF == putc (c, stream))
                return false;
              fprintf_result = 1; /* otherwise it's used uninitialized. */
            }
          else
            {
              fprintf_result = fprintf (stream, "\\%03o", (unsigned int) c);
            }
        }
      if (fprintf_result < 0)
        return false;
    }
  return true;
}

static bool print_name (register const char *p, FILE *stream, int literal_control_chars)
{
  if (literal_control_chars)
    return print_name_without_quoting (p, stream);
  else
    return print_name_with_quoting (p, stream);
}
