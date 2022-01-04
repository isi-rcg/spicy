/* print.c -- print/printf-related code.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000, 2001, 2003, 2004,
   2005, 2006, 2007, 2008, 2009, 2010, 2011, 2012 Free Software Foundation,
   Inc.

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

/* We always include config.h first. */
#include <config.h>

/* system headers go here. */
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <grp.h>
#include <locale.h>
#include <math.h>
#include <pwd.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/types.h>

/* gnulib headers. */
#include "areadlink.h"
#include "dirname.h"
#include "error.h"
#include "filemode.h"
#include "gettext.h"
#include "human.h"
#include "printquoted.h"
#include "stat-size.h"
#include "stat-time.h"
#include "verify.h"
#include "xalloc.h"

/* find-specific headers. */
#include "defs.h"
#include "print.h"

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

#if defined STDC_HEADERS
# define ISDIGIT(c) isdigit ((unsigned char)c)
#else
# define ISDIGIT(c) (isascii ((unsigned char)c) && isdigit ((unsigned char)c))
#endif
#undef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))


/* Create a new fprintf segment in *SEGMENT, with type KIND,
   from the text in FORMAT, which has length LEN.
   Return the address of the `next' pointer of the new segment. */
struct segment **
make_segment (struct segment **segment,
              char *format,
              int len,
              int kind,
              char format_char,
              char aux_format_char,
              struct predicate *pred)
{
  enum EvaluationCost mycost = NeedsNothing;
  char *fmt;

  assert (format_char != '{');
  assert (format_char != '[');
  assert (format_char != '(');

  *segment = xmalloc (sizeof (struct segment));

  (*segment)->segkind = kind;
  (*segment)->format_char[0] = format_char;
  (*segment)->format_char[1] = aux_format_char;
  (*segment)->next = NULL;
  (*segment)->text_len = len;

  fmt = (*segment)->text = xmalloc (len + sizeof "d");
  strncpy (fmt, format, len);
  fmt += len;

  if (kind == KIND_PLAIN     /* Plain text string, no % conversion. */
      || kind == KIND_STOP)  /* Terminate argument, no newline. */
    {
      assert (0 == format_char);
      assert (0 == aux_format_char);
      *fmt = '\0';
      if (mycost > pred->p_cost)
        pred->p_cost = NeedsNothing;
      return &(*segment)->next;
    }

  assert (kind == KIND_FORMAT);
  switch (format_char)
    {
    case '%':                   /* literal % */
      *fmt++ = '%';
      break;

    case 'l':                   /* object of symlink */
      pred->need_stat = true;
      mycost = NeedsLinkName;
      *fmt++ = 's';
      break;

    case 'y':                   /* file type */
      pred->need_type = true;
      mycost = NeedsType;
      *fmt++ = 's';
      break;

    case 'i':                   /* inode number */
      pred->need_inum = true;
      mycost = NeedsInodeNumber;
      *fmt++ = 's';
      break;

    case 'a':                   /* atime in `ctime' format */
    case 'A':                   /* atime in user-specified strftime format */
    case 'B':                   /* birth time in user-specified strftime format */
    case 'c':                   /* ctime in `ctime' format */
    case 'C':                   /* ctime in user-specified strftime format */
    case 'F':                   /* file system type */
    case 'g':                   /* group name */
    case 'M':                   /* mode in `ls -l' format (eg., "drwxr-xr-x") */
    case 's':                   /* size in bytes */
    case 't':                   /* mtime in `ctime' format */
    case 'T':                   /* mtime in user-specified strftime format */
    case 'u':                   /* user name */
      pred->need_stat = true;
      mycost = NeedsStatInfo;
      *fmt++ = 's';
      break;

    case 'S':                   /* sparseness */
      pred->need_stat = true;
      mycost = NeedsStatInfo;
      *fmt++ = 'g';
      break;

    case 'Y':                   /* symlink pointed file type */
      pred->need_stat = true;
      mycost = NeedsType;       /* true for amortised effect */
      *fmt++ = 's';
      break;

    case 'f':                   /* basename of path */
    case 'h':                   /* leading directories part of path */
    case 'p':                   /* pathname */
    case 'P':                   /* pathname with ARGV element stripped */
      *fmt++ = 's';
      break;

    case 'Z':                   /* SELinux security context */
      mycost = NeedsAccessInfo;
      *fmt++ = 's';
      break;

    case 'H':                   /* ARGV element file was found under */
      *fmt++ = 's';
      break;

      /* Numeric items that one might expect to honour
       * #, 0, + flags but which do not.
       */
    case 'G':                   /* GID number */
    case 'U':                   /* UID number */
    case 'b':                   /* size in 512-byte blocks (NOT birthtime in ctime fmt)*/
    case 'D':                   /* Filesystem device on which the file exits */
    case 'k':                   /* size in 1K blocks */
    case 'n':                   /* number of links */
      pred->need_stat = true;
      mycost = NeedsStatInfo;
      *fmt++ = 's';
      break;

      /* Numeric items that DO honour #, 0, + flags.
       */
    case 'd':                   /* depth in search tree (0 = ARGV element) */
      *fmt++ = 'd';
      break;

    case 'm':                   /* mode as octal number (perms only) */
      *fmt++ = 'o';
      pred->need_stat = true;
      mycost = NeedsStatInfo;
      break;
    }
  *fmt = '\0';

  if (mycost > pred->p_cost)
    pred->p_cost = mycost;
  return &(*segment)->next;
}

static bool
is_octal_char (char ch)
{
  return ch >= '0' && ch <= '7';
}

static char
parse_octal_escape(const char *p, size_t *consumed)
{
  register int n, i;
  size_t pos = 0;

  for (i = n = 0; i < 3 && is_octal_char(p[pos]); i++, pos++)
    {
      n = 8 * n + p[pos] - '0';
    }
  --pos;
  *consumed = pos;
  return n;
}

static int
parse_escape_char(const char ch)
{
  char value = 0;
  switch (ch)
    {
    case 'a':
      value = '\a';
      break;
    case 'b':
      value = '\b';
      break;
    case 'f':
      value = '\f';
      break;
    case 'n':
      value = '\n';
      break;
    case 'r':
      value = '\r';
      break;
    case 't':
      value = '\t';
      break;
    case 'v':
      value = '\v';
      break;
    case '\\':
      value = '\\';
      break;
    }
  return value;
}


static size_t
get_format_flags_length(const char *p)
{
  size_t n = 0;
  /* Scan past flags, width and precision, to verify kind. */
  for (; p[++n] && strchr ("-+ #", p[n]);)
    {
      /* Do nothing. */
    }
  while (ISDIGIT (p[n]))
    n++;
  if (p[n] == '.')
    for (n++; ISDIGIT (p[n]); n++)
      /* Do nothing. */ ;
  return n;
}

static size_t
get_format_specifer_length(char ch)
{
  if (strchr ("abcdDfFgGhHiklmMnpPsStuUyYZ%", ch))
    {
      return 1;
    }
  else if (strchr ("ABCT", ch))
    {
      return 2;
    }
  else
    {
      return 0;
    }
}


bool
insert_fprintf (struct format_val *vec,
                const struct parser_table *entry,
                char *format)
{
  char *segstart = format;
  char *fmt_editpos;       /* Current address in scanning `format'. */
  struct segment **segmentp;      /* Address of current segment. */
  struct predicate *our_pred;

  our_pred = insert_primary_withpred (entry, pred_fprintf, format);
  our_pred->side_effects = our_pred->no_default_print = true;
  our_pred->args.printf_vec = *vec;
  our_pred->need_type = false;
  our_pred->need_stat = false;
  our_pred->p_cost    = NeedsNothing;

  segmentp = &our_pred->args.printf_vec.segment;
  *segmentp = NULL;

  for (fmt_editpos = segstart; *fmt_editpos; fmt_editpos++)
    {
      if (fmt_editpos[0] == '\\' && fmt_editpos[1] == 'c')
        {
          make_segment (segmentp, segstart, fmt_editpos - segstart,
                        KIND_STOP, 0, 0,
                        our_pred);
          if (our_pred->need_stat && (our_pred->p_cost < NeedsStatInfo))
            our_pred->p_cost = NeedsStatInfo;
          return true;
        }
      else if (*fmt_editpos == '\\')
        {
          size_t readpos = 1;
          if (!fmt_editpos[readpos])
            {
              error (0, 0, _("warning: escape `\\' followed by nothing at all"));
              --readpos;
              /* (*fmt_editpos) is already '\\' and that's a reasonable result. */
            }
          else if (is_octal_char(fmt_editpos[readpos]))
            {
              size_t consumed = 0;
              *fmt_editpos = parse_octal_escape(fmt_editpos + readpos, &consumed);
              readpos += consumed;
            }
          else
            {
              const char val = parse_escape_char(fmt_editpos[readpos]);
              if (val)
                {
                  fmt_editpos[0] = val;
                }
              else
                {
                  error (0, 0, _("warning: unrecognized escape `\\%c'"),
                         fmt_editpos[readpos]);
                  fmt_editpos += readpos;
                  continue;
                }
            }
          segmentp = make_segment (segmentp,
                                   segstart, fmt_editpos - segstart + 1,
                                   KIND_PLAIN, 0, 0,
                                   our_pred);
          segstart = fmt_editpos + readpos + 1; /* Move past the escape. */
          fmt_editpos += readpos;  /* Incremented immediately by `for'. */
        }
      else if (fmt_editpos[0] == '%')
        {
          size_t len;
          if (fmt_editpos[1] == 0)
            {
              /* Trailing %.  We don't like those. */
              error (EXIT_FAILURE, 0,
                     _("error: %s at end of format string"), fmt_editpos);
            }

          if (fmt_editpos[1] == '%') /* %% produces just %. */
            len = 1;
          else
            len = get_format_flags_length(fmt_editpos);
          fmt_editpos += len;

          len = get_format_specifer_length (fmt_editpos[0]);
          if (len && (fmt_editpos[len-1]))
            {
              const char fmt2 = (len == 2) ? fmt_editpos[1] : 0;
              segmentp = make_segment (segmentp, segstart,
                                       fmt_editpos - segstart,
                                       KIND_FORMAT, fmt_editpos[0], fmt2,
                                       our_pred);
              fmt_editpos += (len - 1);
            }
          else
            {
              if (strchr ("{[(", fmt_editpos[0]))
                {
                  error (EXIT_FAILURE, 0,
                         _("error: the format directive `%%%c' is reserved for future use"),
                         (int)fmt_editpos[0]);
                  /*NOTREACHED*/
                }

              if (len == 2 && !fmt_editpos[1])
                {
                  error (0, 0,
                         _("warning: format directive `%%%c' "
                           "should be followed by another character"),
                         fmt_editpos[0]);
                }
              else
                {
                  /* An unrecognized % escape.  Print the char after the %. */
                  error (0, 0,
                         _("warning: unrecognized format directive `%%%c'"),
                         fmt_editpos[0]);
                }
              segmentp = make_segment (segmentp,
                                       segstart, fmt_editpos + 1 - segstart,
                                       KIND_PLAIN, 0, 0,
                                       our_pred);
            }
          segstart = fmt_editpos + 1;
        }
    }

  if (fmt_editpos > segstart)
    make_segment (segmentp, segstart, fmt_editpos - segstart, KIND_PLAIN, 0, 0,
                  our_pred);
  return true;
}

static bool
scan_for_digit_differences (const char *p, const char *q,
                            size_t *first, size_t *n)
{
  bool seen = false;
  size_t i;

  for (i=0; p[i] && q[i]; i++)
    {
      if (p[i] != q[i])
        {
          if (!isdigit ((unsigned char)q[i]) || !isdigit ((unsigned char)q[i]))
            return false;

          if (!seen)
            {
              *first = i;
              *n = 1;
              seen = 1;
            }
          else
            {
              if (i-*first == *n)
                {
                  /* Still in the first sequence of differing digits. */
                  ++*n;
                }
              else
                {
                  /* More than one differing contiguous character sequence. */
                  return false;
                }
            }
        }
    }
  if (p[i] || q[i])
    {
      /* strings are different lengths. */
      return false;
    }
  return true;
}

static char*
do_time_format (const char *fmt, const struct tm *p, const char *ns, size_t ns_size)
{
  static char *buf = NULL;
  static size_t buf_size;
  char *timefmt = NULL;
  struct tm altered_time;


  /* If the format expands to nothing (%p in some locales, for
   * example), strftime can return 0.  We actually want to distinguish
   * the error case where the buffer is too short, so we just prepend
   * an otherwise uninteresting character to prevent the no-output
   * case.
   */
  timefmt = xmalloc (strlen (fmt) + 2u);
  timefmt[0] = '_';
  memcpy (timefmt + 1, fmt, strlen (fmt) + 1);

  /* altered_time is a similar time, but in which both
   * digits of the seconds field are different.
   */
  altered_time = *p;
  if (altered_time.tm_sec >= 11)
    altered_time.tm_sec -= 11;
  else
    altered_time.tm_sec += 11;

  /* If we call strftime() with buf_size=0, the program will coredump
   * on Solaris, since it unconditionally writes the terminating null
   * character.
   */
  if (buf == NULL)
    {
      buf_size = 1u;
      buf = xmalloc (buf_size);
    }
  while (true)
    {
      /* I'm not sure that Solaris will return 0 when the buffer is too small.
       * Therefore we do not check for (buf_used != 0) as the termination
       * condition.
       */
      size_t buf_used = strftime (buf, buf_size, timefmt, p);
      if (buf_used              /* Conforming POSIX system */
          && (buf_used < buf_size)) /* Solaris workaround */
        {
          char *altbuf;
          size_t i = 0, n = 0;
          size_t final_len = (buf_used
                              + 1u /* for \0 */
                              + ns_size);
          buf = xrealloc (buf, final_len);
          buf_size = final_len;
          altbuf = xmalloc (final_len);
          strftime (altbuf, buf_size, timefmt, &altered_time);

          /* Find the seconds digits; they should be the only changed part.
           * In theory the result of the two formatting operations could differ in
           * more than just one sequence of decimal digits (for example %X might
           * in theory return a spelled-out time like "thirty seconds past noon").
           * When that happens, we just avoid inserting the nanoseconds field.
           */
          if (scan_for_digit_differences (buf, altbuf, &i, &n)
              && (2==n) && !isdigit ((unsigned char)buf[i+n]))
            {
              const size_t end_of_seconds = i + n;
              const size_t suffix_len = buf_used-(end_of_seconds)+1;

              /* Move the tail (including the \0).  Note that this
               * is a move of an overlapping memory block, so we
               * must use memmove instead of memcpy.  Then insert
               * the nanoseconds (but not its trailing \0).
               */
              assert (end_of_seconds + ns_size + suffix_len == final_len);
              memmove (buf+end_of_seconds+ns_size,
                       buf+end_of_seconds,
                       suffix_len);
              memcpy (buf+i+n, ns, ns_size);
            }
          else
            {
              /* No seconds digits.  No need to insert anything. */
            }
          /* The first character of buf is the underscore, which we actually
           * don't want.
           */
          free (timefmt);
          free (altbuf);
          return buf+1;
        }
      else
        {
          buf = x2nrealloc (buf, &buf_size, sizeof *buf);
        }
    }
}

/* Return a static string formatting the time WHEN according to the
 * strftime format character KIND.
 *
 * This function contains a number of assertions.  These look like
 * runtime checks of the results of computations, which would be a
 * problem since external events should not be tested for with
 * "assert" (instead you should use "if").  However, they are not
 * really runtime checks.  The assertions actually exist to verify
 * that the various buffers are correctly sized.
 */
static char *
format_date (struct timespec ts, int kind)
{
  /* In theory, we use an extra 10 characters for 9 digits of
   * nanoseconds and 1 for the decimal point.  However, the real
   * world is more complex than that.
   *
   * For example, some systems return junk in the tv_nsec part of
   * st_birthtime.  An example of this is the NetBSD-4.0-RELENG kernel
   * (at Sat Mar 24 18:46:46 2007) running a NetBSD-3.1-RELEASE
   * runtime and examining files on an msdos filesytem.  So for that
   * reason we set NS_BUF_LEN to 32, which is simply "long enough" as
   * opposed to "exactly the right size".  Note that the behaviour of
   * NetBSD appears to be a result of the use of uninitialized data,
   * as it's not 100% reproducible (more like 25%).
   */
  enum {
    NS_BUF_LEN = 32,
    DATE_LEN_PERCENT_APLUS=21   /* length of result of %A+ (it's longer than %c)*/
  };
  static char buf[128u+10u + MAX(DATE_LEN_PERCENT_APLUS,
                            MAX (LONGEST_HUMAN_READABLE + 2, NS_BUF_LEN+64+200))];
  char ns_buf[NS_BUF_LEN]; /* -.9999999990 (- sign can happen!)*/
  int  charsprinted, need_ns_suffix;
  struct tm *tm;
  char fmt[6];

  /* human_readable() assumes we pass a buffer which is at least as
   * long as LONGEST_HUMAN_READABLE.  We use an assertion here to
   * ensure that no nasty unsigned overflow happened in our calculation
   * of the size of buf.  Do the assertion here rather than in the
   * code for %@ so that we find the problem quickly if it exists.  If
   * you want to submit a patch to move this into the if statement, go
   * ahead, I'll apply it.  But include performance timings
   * demonstrating that the performance difference is actually
   * measurable.
   */
  verify (sizeof (buf) >= LONGEST_HUMAN_READABLE);

  charsprinted = 0;
  need_ns_suffix = 0;

  /* Format the main part of the time. */
  if (kind == '+')
    {
      strcpy (fmt, "%F+%T");
      need_ns_suffix = 1;
    }
  else
    {
      fmt[0] = '%';
      fmt[1] = kind;
      fmt[2] = '\0';

      /* %a, %c, and %t are handled in ctime_format() */
      switch (kind)
        {
        case 'S':
        case 'T':
        case 'X':
        case '@':
          need_ns_suffix = 1;
          break;
        default:
          need_ns_suffix = 0;
          break;
        }
    }

  if (need_ns_suffix)
    {
      /* Format the nanoseconds part.  Leave a trailing zero to
       * discourage people from writing scripts which extract the
       * fractional part of the timestamp by using column offsets.
       * The reason for discouraging this is that in the future, the
       * granularity may not be nanoseconds.
       */
      charsprinted = snprintf (ns_buf, NS_BUF_LEN, ".%09ld0", (long int)ts.tv_nsec);
      assert (charsprinted < NS_BUF_LEN);
    }
  else
    {
      charsprinted = 0;
      ns_buf[0] = 0;
    }

  if (kind != '@')
    {
      tm = localtime (&ts.tv_sec);
      if (tm)
        {
          char *s = do_time_format (fmt, tm, ns_buf, charsprinted);
          if (s)
            return s;
        }
    }

  /* If we get to here, either the format was %@, or we have fallen back to it
   * because strftime failed.
   */
  if (1)
    {
      uintmax_t w = ts.tv_sec;
      size_t used, len, remaining;

      /* XXX: note that we are negating an unsigned type which is the
       * widest possible unsigned type.
       */
      char *p = human_readable (ts.tv_sec < 0 ? -w : w, buf + 1,
                                human_ceiling, 1, 1);
      assert (p > buf);
      assert (p < (buf + (sizeof buf)));
      if (ts.tv_sec < 0)
        *--p = '-'; /* XXX: Ugh, relying on internal details of human_readable(). */

      /* Add the nanoseconds part.  Because we cannot enforce a
       * particlar implementation of human_readable, we cannot assume
       * any particular value for (p-buf).  So we need to be careful
       * that there is enough space remaining in the buffer.
       */
      if (need_ns_suffix)
        {
          len = strlen (p);
          used = (p-buf) + len; /* Offset into buf of current end */
          assert (sizeof buf > used); /* Ensure we can perform subtraction safely. */
          remaining = sizeof buf - used - 1u; /* allow space for NUL */

          if (strlen (ns_buf) >= remaining)
            {
              error (0, 0,
                     "charsprinted=%ld but remaining=%lu: ns_buf=%s",
                     (long)charsprinted, (unsigned long)remaining, ns_buf);
            }
          assert (strlen (ns_buf) < remaining);
          strcat (p, ns_buf);
        }
      return p;
    }
}

static const char *weekdays[] =
  {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };
static const char * months[] =
  {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
  };


static char *
ctime_format (struct timespec ts)
{
  const struct tm * ptm;
#define TIME_BUF_LEN 1024
  static char resultbuf[TIME_BUF_LEN];
  int nout;

  ptm = localtime (&ts.tv_sec);
  if (ptm)
    {
      assert (ptm->tm_wday >=  0);
      assert (ptm->tm_wday <   7);
      assert (ptm->tm_mon  >=  0);
      assert (ptm->tm_mon  <  12);
      assert (ptm->tm_hour >=  0);
      assert (ptm->tm_hour <  24);
      assert (ptm->tm_min  <  60);
      assert (ptm->tm_sec  <= 61); /* allows 2 leap seconds. */

      /* wkday mon mday hh:mm:ss.nnnnnnnnn yyyy */
      nout = snprintf (resultbuf, TIME_BUF_LEN,
                       "%3s %3s %2d %02d:%02d:%02d.%09ld0 %04d",
                       weekdays[ptm->tm_wday],
                       months[ptm->tm_mon],
                       ptm->tm_mday,
                       ptm->tm_hour,
                       ptm->tm_min,
                       ptm->tm_sec,
                       (long int)ts.tv_nsec,
                       1900 + ptm->tm_year);

      assert (nout < TIME_BUF_LEN);
      return resultbuf;
    }
  else
    {
      /* The time cannot be represented as a struct tm.
         Output it as an integer.  */
      return format_date (ts, '@');
    }
}

static double
file_sparseness (const struct stat *p)
{
  if (0 == p->st_size)
    {
      if (0 == ST_NBLOCKS(*p))
        return 1.0;
      else
        return ST_NBLOCKS(*p) < 0 ? -HUGE_VAL : HUGE_VAL;
    }
  else
    {
      double blklen = ST_NBLOCKSIZE * (double)ST_NBLOCKS(*p);
      return blklen / p->st_size;
    }
}

static void
checked_fprintf (struct format_val *dest, const char *fmt, ...)
{
  int rv;
  va_list ap;

  va_start (ap, fmt);
  rv = vfprintf (dest->stream, fmt, ap);
  if (rv < 0)
    nonfatal_nontarget_file_error (errno, dest->filename);
}

static void
checked_print_quoted (struct format_val *dest,
                           const char *format, const char *s)
{
  int rv = print_quoted (dest->stream, dest->quote_opts, dest->dest_is_tty,
                         format, s);
  if (rv < 0)
    nonfatal_nontarget_file_error (errno, dest->filename);
}


static void
checked_fwrite (void *p, size_t siz, size_t nmemb, struct format_val *dest)
{
  const size_t items_written = fwrite (p, siz, nmemb, dest->stream);
  if (items_written < nmemb)
    nonfatal_nontarget_file_error (errno, dest->filename);
}

static void
checked_fflush (struct format_val *dest)
{
  if (0 != fflush (dest->stream))
    {
      nonfatal_nontarget_file_error (errno, dest->filename);
    }
}

static const char*
mode_to_filetype (mode_t m)
{
#define HANDLE_TYPE(t,letter) if (m==t) { return letter; }
#ifdef S_IFREG
  HANDLE_TYPE(S_IFREG,  "f");   /* regular file */
#endif
#ifdef S_IFDIR
  HANDLE_TYPE(S_IFDIR,  "d");   /* directory */
#endif
#ifdef S_IFLNK
  HANDLE_TYPE(S_IFLNK,  "l");   /* symbolic link */
#endif
#ifdef S_IFSOCK
  HANDLE_TYPE(S_IFSOCK, "s");   /* Unix domain socket */
#endif
#ifdef S_IFBLK
  HANDLE_TYPE(S_IFBLK,  "b");   /* block device */
#endif
#ifdef S_IFCHR
  HANDLE_TYPE(S_IFCHR,  "c");   /* character device */
#endif
#ifdef S_IFIFO
  HANDLE_TYPE(S_IFIFO,  "p");   /* FIFO */
#endif
#ifdef S_IFDOOR
  HANDLE_TYPE(S_IFDOOR, "D");   /* Door (e.g. on Solaris) */
#endif
  return "U";                   /* Unknown */
}



static void
do_fprintf (struct format_val *dest,
            struct segment *segment,
            const char *pathname,
            const struct stat *stat_buf)
{
  char hbuf[LONGEST_HUMAN_READABLE + 1];
  const char *cp;

  switch (segment->segkind)
    {
    case KIND_PLAIN:    /* Plain text string (no % conversion). */
      /* trusted */
      checked_fwrite(segment->text, 1, segment->text_len, dest);
      break;

    case KIND_STOP:             /* Terminate argument and flush output. */
      /* trusted */
      checked_fwrite (segment->text, 1, segment->text_len, dest);
      checked_fflush (dest);
      break;

    case KIND_FORMAT:
      switch (segment->format_char[0])
        {
        case 'a':               /* atime in `ctime' format. */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text, ctime_format (get_stat_atime (stat_buf)));
          break;
        case 'b':               /* size in 512-byte blocks */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text,
                           human_readable ((uintmax_t) ST_NBLOCKS (*stat_buf),
                                           hbuf, human_ceiling,
                                           ST_NBLOCKSIZE, 512));
          break;
        case 'c':               /* ctime in `ctime' format */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text, ctime_format (get_stat_ctime (stat_buf)));
          break;
        case 'd':               /* depth in search tree */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text, state.curdepth);
          break;
        case 'D':               /* Device on which file exists (stat.st_dev) */
          /* trusted */
          checked_fprintf (dest, segment->text,
                           human_readable ((uintmax_t) stat_buf->st_dev, hbuf,
                                           human_ceiling, 1, 1));
          break;
        case 'f':               /* base name of path */
          /* sanitised */
          {
            char *base = base_name (pathname);
            checked_print_quoted (dest, segment->text, base);
            free (base);
          }
          break;
        case 'F':               /* file system type */
          /* trusted */
          checked_print_quoted (dest, segment->text, filesystem_type (stat_buf, pathname));
          break;
        case 'g':               /* group name */
          /* trusted */
          /* (well, the actual group is selected by the user but
           * its name was selected by the system administrator)
           */
          {
            struct group *g;

            g = getgrgid (stat_buf->st_gid);
            if (g)
              {
                segment->text[segment->text_len] = 's';
                checked_fprintf (dest, segment->text, g->gr_name);
                break;
              }
            else
              {
                /* Do nothing. */
                /*FALLTHROUGH*/
              }
          }
          /*FALLTHROUGH*/ /*...sometimes, so 'G' case.*/

        case 'G':               /* GID number */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text,
                           human_readable ((uintmax_t) stat_buf->st_gid, hbuf,
                                           human_ceiling, 1, 1));
          break;
        case 'h':               /* leading directories part of path */
          /* sanitised */
          {
            cp = strrchr (pathname, '/');
            if (cp == NULL)     /* No leading directories. */
              {
                /* If there is no slash in the pathname, we still
                 * print the string because it contains characters
                 * other than just '%s'.  The %h expands to ".".
                 */
                checked_print_quoted (dest, segment->text, ".");
              }
            else
              {
                char *s = strdup (pathname);
                s[cp - pathname] = 0;
                checked_print_quoted (dest, segment->text, s);
                free (s);
              }
          }
          break;

        case 'H':               /* ARGV element file was found under */
          /* trusted */
          {
            char *s = xmalloc (state.starting_path_length+1);
            memcpy (s, pathname, state.starting_path_length);
            s[state.starting_path_length] = 0;
            checked_fprintf (dest, segment->text, s);
            free (s);
          }
          break;

        case 'i':               /* inode number */
          /* UNTRUSTED, but not exploitable I think */
          /* POSIX does not guarantee that ino_t is unsigned or even
           * integral (except as an XSI extension), but we'll work on
           * fixing that if we ever get a report of a system where
           * ino_t is indeed a signed integral type or a non-integral
           * arithmetic type. */
          checked_fprintf (dest, segment->text,
                           human_readable ((uintmax_t) stat_buf->st_ino, hbuf,
                                           human_ceiling,
                                           1, 1));
          break;
        case 'k':               /* size in 1K blocks */
          /* UNTRUSTED, but not exploitable I think */
          checked_fprintf (dest, segment->text,
                           human_readable ((uintmax_t) ST_NBLOCKS (*stat_buf),
                                           hbuf, human_ceiling,
                                           ST_NBLOCKSIZE, 1024));
          break;
        case 'l':               /* object of symlink */
          /* sanitised */
#ifdef S_ISLNK
          {
            char *linkname = 0;

            if (S_ISLNK (stat_buf->st_mode))
              {
                linkname = areadlinkat (state.cwd_dir_fd, state.rel_pathname);
                if (linkname == NULL)
                  {
                    nonfatal_target_file_error (errno, pathname);
                    state.exit_status = 1;
                  }
              }
            if (linkname)
              {
                checked_print_quoted (dest, segment->text, linkname);
              }
            else
              {
                /* We still need to honour the field width etc., so this is
                 * not a no-op.
                 */
                checked_print_quoted (dest, segment->text, "");
              }
            free (linkname);
          }
#endif                          /* S_ISLNK */
          break;

        case 'M':               /* mode as 10 chars (eg., "-rwxr-x--x" */
          /* UNTRUSTED, probably unexploitable */
          {
            char modestring[16] ;
            filemodestring (stat_buf, modestring);
            modestring[10] = '\0';
            checked_fprintf (dest, segment->text, modestring);
          }
          break;

        case 'm':               /* mode as octal number (perms only) */
          /* UNTRUSTED, probably unexploitable */
          {
            /* Output the mode portably using the traditional numbers,
               even if the host unwisely uses some other numbering
               scheme.  But help the compiler in the common case where
               the host uses the traditional numbering scheme.  */
            mode_t m = stat_buf->st_mode;
            bool traditional_numbering_scheme =
              (S_ISUID == 04000 && S_ISGID == 02000 && S_ISVTX == 01000
               && S_IRUSR == 00400 && S_IWUSR == 00200 && S_IXUSR == 00100
               && S_IRGRP == 00040 && S_IWGRP == 00020 && S_IXGRP == 00010
               && S_IROTH == 00004 && S_IWOTH == 00002 && S_IXOTH == 00001);
            checked_fprintf (dest, segment->text,
                     (traditional_numbering_scheme
                      ? m & MODE_ALL
                      : ((m & S_ISUID ? 04000 : 0)
                         | (m & S_ISGID ? 02000 : 0)
                         | (m & S_ISVTX ? 01000 : 0)
                         | (m & S_IRUSR ? 00400 : 0)
                         | (m & S_IWUSR ? 00200 : 0)
                         | (m & S_IXUSR ? 00100 : 0)
                         | (m & S_IRGRP ? 00040 : 0)
                         | (m & S_IWGRP ? 00020 : 0)
                         | (m & S_IXGRP ? 00010 : 0)
                         | (m & S_IROTH ? 00004 : 0)
                         | (m & S_IWOTH ? 00002 : 0)
                         | (m & S_IXOTH ? 00001 : 0))));
          }
          break;

        case 'n':               /* number of links */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text,
                   human_readable ((uintmax_t) stat_buf->st_nlink,
                                   hbuf,
                                   human_ceiling,
                                   1, 1));
          break;

        case 'p':               /* pathname */
          /* sanitised */
          checked_print_quoted (dest, segment->text, pathname);
          break;

        case 'P':               /* pathname with ARGV element stripped */
          /* sanitised */
          if (state.curdepth > 0)
            {
              cp = pathname + state.starting_path_length;
              if (*cp == '/')
                /* Move past the slash between the ARGV element
                   and the rest of the pathname.  But if the ARGV element
                   ends in a slash, we didn't add another, so we've
                   already skipped past it.  */
                cp++;
            }
          else
            {
              cp = "";
            }
          checked_print_quoted (dest, segment->text, cp);
          break;

        case 's':               /* size in bytes */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text,
                   human_readable ((uintmax_t) stat_buf->st_size,
                                   hbuf, human_ceiling, 1, 1));
          break;

        case 'S':               /* sparseness */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text, file_sparseness (stat_buf));
          break;

        case 't':               /* mtime in `ctime' format */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text,
                           ctime_format (get_stat_mtime (stat_buf)));
          break;

        case 'u':               /* user name */
          /* trusted */
          /* (well, the actual user is selected by the user on systems
           * where chown is not restricted, but the user name was
           * selected by the system administrator)
           */
          {
            struct passwd *p;

            p = getpwuid (stat_buf->st_uid);
            if (p)
              {
                segment->text[segment->text_len] = 's';
                checked_fprintf (dest, segment->text, p->pw_name);
                break;
              }
            /* else fallthru */
          }
          /* FALLTHROUGH*/ /* .. to case U */

        case 'U':               /* UID number */
          /* UNTRUSTED, probably unexploitable */
          checked_fprintf (dest, segment->text,
                           human_readable ((uintmax_t) stat_buf->st_uid, hbuf,
                                           human_ceiling, 1, 1));
          break;

          /* %Y: type of file system entry like `ls -l`:
           *     (d,-,l,s,p,b,c,n) n=nonexistent (symlink)
           */
        case 'Y':               /* in case of symlink */
          /* trusted */
          {
#ifdef S_ISLNK
            if (S_ISLNK (stat_buf->st_mode))
              {
                struct stat sbuf;
                /* If we would normally follow links, do not do so.
                 * If we would normally not follow links, do so.
                 */
                if ((following_links () ? optionp_stat : optionl_stat)
                    (state.rel_pathname, &sbuf) != 0)
                  {
                    if ( errno == ENOENT )
                      {
                        checked_fprintf (dest, segment->text, "N");
                        break;
                      }
                    else if ( errno == ELOOP )
                      {
                        checked_fprintf (dest, segment->text, "L");
                        break;
                      }
                    else
                      {
                        checked_fprintf (dest, segment->text, "?");
                        error (0, errno, "%s",
                               safely_quote_err_filename (0, pathname));
                        /* exit_status = 1;
                           return ; */
                        break;
                      }
                  }
                checked_fprintf (dest, segment->text,
                                 mode_to_filetype (sbuf.st_mode & S_IFMT));
              }
#endif /* S_ISLNK */
            else
              {
                checked_fprintf (dest, segment->text,
                                 mode_to_filetype (stat_buf->st_mode & S_IFMT));
              }
          }
          break;

        case 'y':
          /* trusted */
          {
            checked_fprintf (dest, segment->text,
                             mode_to_filetype (stat_buf->st_mode & S_IFMT));
          }
          break;

        case 'Z':               /* SELinux security context */
          {
            security_context_t scontext;
            int rv = (*options.x_getfilecon) (state.cwd_dir_fd, state.rel_pathname,
                                              &scontext);
            if (rv < 0)
              {
                /* If getfilecon fails, there will in the general case
                   still be some text to print.   We just make %Z expand
                   to an empty string. */
                checked_fprintf (dest, segment->text, "");

                error (0, errno, _("getfilecon failed: %s"),
                    safely_quote_err_filename (0, pathname));
                state.exit_status = 1;
              }
            else
              {
                checked_fprintf (dest, segment->text, scontext);
                freecon (scontext);
              }
          }
          break;

        case 0:
        case '%':
          checked_fprintf (dest, segment->text);
          break;
        }
      /* end of KIND_FORMAT case */
      break;
    }
}

bool
pred_fprintf (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr)
{
  struct format_val *dest = &pred_ptr->args.printf_vec;
  struct segment *segment;

  for (segment = dest->segment; segment; segment = segment->next)
    {
      if ( (KIND_FORMAT == segment->segkind) && segment->format_char[1]) /* Component of date. */
        {
          struct timespec ts;
          int valid = 0;

          switch (segment->format_char[0])
            {
            case 'A':
              ts = get_stat_atime (stat_buf);
              valid = 1;
              break;
            case 'B':
              ts = get_stat_birthtime (stat_buf);
              if ('@' == segment->format_char[1])
                valid = 1;
              else
                valid = (ts.tv_nsec >= 0);
              break;
            case 'C':
              ts = get_stat_ctime (stat_buf);
              valid = 1;
              break;
            case 'T':
              ts = get_stat_mtime (stat_buf);
              valid = 1;
              break;
            default:
              assert (0);
              abort ();
            }
          /* We trust the output of format_date not to contain
           * nasty characters, though the value of the date
           * is itself untrusted data.
           */
          if (valid)
            {
              /* trusted */
              checked_fprintf (dest, segment->text,
                               format_date (ts, segment->format_char[1]));
            }
          else
            {
              /* The specified timestamp is not available, output
               * nothing for the timestamp, but use the rest (so that
               * for example find foo -printf '[%Bs] %p\n' can print
               * "[] foo").
               */
              /* trusted */
              checked_fprintf (dest, segment->text, "");
            }
        }
      else
        {
          /* Print a segment which is not a date. */
          do_fprintf (dest, segment, pathname, stat_buf);
        }
    }
  return true;
}
