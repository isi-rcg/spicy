/* argparse.c - Argument Parser for option handling
 * Copyright (C) 1997-2001, 2006-2008, 2013-2017 Werner Koch
 * Copyright (C) 1998-2001, 2006-2008, 2012 Free Software Foundation, Inc.
 * Copyright (C) 2015-2018 g10 Code GmbH
 *
 * This file is part of Libgpg-error.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * This file was originally a part of GnuPG.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>

#include "gpgrt-int.h"

/* Special short options which are auto-inserterd.  */
#define ARGPARSE_SHORTOPT_HELP 32768
#define ARGPARSE_SHORTOPT_VERSION 32769
#define ARGPARSE_SHORTOPT_WARRANTY 32770
#define ARGPARSE_SHORTOPT_DUMP_OPTIONS 32771

/* A mask for the types.  */
#define ARGPARSE_TYPE_MASK  7  /* Mask for the type values.  */

/* Internal object of the public gpgrt_argparse_t object.  */
struct _gpgrt_argparse_internal_s
{
  int idx;
  int inarg;
  int stopped;
  const char *last;
  void *aliases;
  const void *cur_alias;
  void *iio_list;
  gpgrt_opt_t **opts;  /* Malloced array of pointer to user provided opts.  */
};


typedef struct alias_def_s *ALIAS_DEF;
struct alias_def_s {
    ALIAS_DEF next;
    char *name;   /* malloced buffer with name, \0, value */
    const char *value; /* ptr into name */
};


/* Object to store the names for the --ignore-invalid-option option.
   This is a simple linked list.  */
typedef struct iio_item_def_s *IIO_ITEM_DEF;
struct iio_item_def_s
{
  IIO_ITEM_DEF next;
  char name[1];      /* String with the long option name.  */
};


/* The almost always needed user handler for strusage.  */
static const char *(*strusage_handler)( int ) = NULL;
/* Optional handler to write strings.  See _gpgrt_set_usage_outfnc.  */
static int (*custom_outfnc) (int, const char *);
/* Optional handler to map strings.  See _gpgrt_set_fixed_string_mapper.  */
static const char *(*fixed_string_mapper)(const char*);

static int  set_opt_arg (gpgrt_argparse_t *arg, unsigned int flags, char *s);
static void show_help (gpgrt_opt_t **opts, unsigned int flags);
static void show_version (void);
static int writestrings (int is_error, const char *string,
                         ...) GPGRT_ATTR_SENTINEL(0);
static int arg_parse (gpgrt_argparse_t *arg, gpgrt_opt_t *opts);





/* Return true if the native charset is utf-8.  */
static int
is_native_utf8 (void)
{
  static char result;

  if (!result)
    {
      const char *p = _gpgrt_strusage (8);
      if (!p || !*p || !strcmp (p, "utf-8"))
        result = 1;
      result |= 128;
    }

  return (result & 1);
}


static char *
trim_spaces (char *str)
{
  char *string, *p, *mark;

  string = str;
  /* Find first non space character. */
  for (p=string; *p && isspace (*(unsigned char*)p) ; p++)
    ;
  /* Move characters. */
  for ((mark = NULL); (*string = *p); string++, p++)
    if (isspace (*(unsigned char*)p))
      {
        if (!mark)
          mark = string;
      }
    else
      mark = NULL;
  if (mark)
    *mark = '\0' ;  /* Remove trailing spaces. */

  return str ;
}


static const char *
map_fixed_string (const char *string)
{
  return fixed_string_mapper? fixed_string_mapper (string) : string;
}


/* Write STRING and all following const char * arguments either to
   stdout or, if IS_ERROR is set, to stderr.  The list of strings must
   be terminated by a NULL.  */
static int
writestrings (int is_error, const char *string, ...)
{
  va_list arg_ptr;
  const char *s;
  int count = 0;

  if (string)
    {
      s = string;
      va_start (arg_ptr, string);
      do
        {
          if (custom_outfnc)
            custom_outfnc (is_error? 2:1, s);
          else
            fputs (s, is_error? stderr : stdout);
          count += strlen (s);
        }
      while ((s = va_arg (arg_ptr, const char *)));
      va_end (arg_ptr);
    }
  return count;
}


static void
flushstrings (int is_error)
{
  if (custom_outfnc)
    custom_outfnc (is_error? 2:1, NULL);
  else
    _gpgrt_fflush (is_error? es_stderr : es_stdout);
}


static void
deinitialize (gpgrt_argparse_t *arg)
{
  if (arg->internal)
    {
      xfree (arg->internal->opts);
      xfree (arg->internal);
      arg->internal = NULL;
    }

  arg->lineno = 0;
  arg->err = 0;
}

/* Our own exit handler to clean up used memory.  */
static void
my_exit (gpgrt_argparse_t *arg, int code)
{
  deinitialize (arg);
  exit (code);
}


static gpg_err_code_t
initialize (gpgrt_argparse_t *arg, gpgrt_opt_t *opts, estream_t fp)
{
  if (!arg->internal || (arg->flags & ARGPARSE_FLAG_RESET))
    {
      /* Allocate internal data.  */
      if (!arg->internal)
        {
          arg->internal = xtrymalloc (sizeof *arg->internal);
          if (!arg->internal)
            return _gpg_err_code_from_syserror ();
        }
      else if (arg->internal->opts)
        xfree (arg->internal->opts);
      arg->internal->opts = NULL;

      /* Initialize this instance. */
      arg->internal->idx = 0;
      arg->internal->last = NULL;
      arg->internal->inarg = 0;
      arg->internal->stopped = 0;
      arg->internal->aliases = NULL;
      arg->internal->cur_alias = NULL;
      arg->internal->iio_list = NULL;

      /* Clear the copy of the option list.  */
      /* Clear the error indicator.  */
      arg->err = 0;

      /* Usually an option file will be parsed from the start.
       * However, we do not open the stream and thus we have no way to
       * know the current lineno.  Using this flag we can allow the
       * user to provide a lineno which we don't reset.  */
      if (fp || !(arg->flags & ARGPARSE_FLAG_NOLINENO))
        arg->lineno = 0;

      /* Need to clear the reset request.  */
      arg->flags &= ~ARGPARSE_FLAG_RESET;

      /* Check initial args.  */
      if ( *arg->argc < 0 )
        _gpgrt_log_bug ("invalid argument passed to gpgrt_argparse\n");

    }

  /* Create an array with pointers to the provided list of options.
   * Keeping a copy is useful to sort that array and thus do a binary
   * search and to allow for extra space at the end to insert the
   * hidden options.  An ARGPARSE_FLAG_RESET can be used to reinit
   * this array.  */
  if (!arg->internal->opts)
    {
      static gpgrt_opt_t help_opt
        = ARGPARSE_s_n (ARGPARSE_SHORTOPT_HELP, "help", "@");
      static gpgrt_opt_t version_opt
        = ARGPARSE_s_n (ARGPARSE_SHORTOPT_VERSION, "version", "@");
      static gpgrt_opt_t warranty_opt
        = ARGPARSE_s_n (ARGPARSE_SHORTOPT_WARRANTY, "warranty", "@");
      static gpgrt_opt_t dump_options_opt
        = ARGPARSE_s_n(ARGPARSE_SHORTOPT_DUMP_OPTIONS, "dump-options", "@");
      static gpgrt_opt_t end_marker
        = ARGPARSE_end ();
      int seen_help = 0;
      int seen_version = 0;
      int seen_warranty = 0;
      int seen_dump_options = 0;
      int i;

      for (i=0; opts[i].short_opt; i++)
        {
          if (opts[i].long_opt)
            {
              if (!strcmp(opts[i].long_opt, help_opt.long_opt))
                seen_help = 1;
              else if (!strcmp(opts[i].long_opt, version_opt.long_opt))
                seen_version = 1;
              else if (!strcmp(opts[i].long_opt, warranty_opt.long_opt))
                seen_warranty = 1;
              else if (!strcmp(opts[i].long_opt, dump_options_opt.long_opt))
                seen_dump_options = 1;
            }
        }
      i += 4; /* The number of the above internal options.  */
      i++;    /* End of list marker.  */
      arg->internal->opts = xtrycalloc (i, sizeof *arg->internal->opts);
      if (!arg->internal->opts)
        return _gpg_err_code_from_syserror ();
      for(i=0; opts[i].short_opt; i++)
        arg->internal->opts[i] = opts + i;
      if (!seen_help)
        arg->internal->opts[i++] = &help_opt;
      if (!seen_version)
        arg->internal->opts[i++] = &version_opt;
      if (!seen_warranty)
        arg->internal->opts[i++] = &warranty_opt;
      if (!seen_dump_options)
        arg->internal->opts[i++] = &dump_options_opt;
      arg->internal->opts[i] = &end_marker;
    }

  if (arg->err)
    {
      /* Last option was erroneous.  */
      const char *s;

      if (fp)
        {
          if ( arg->r_opt == ARGPARSE_UNEXPECTED_ARG )
            s = _("argument not expected");
          else if ( arg->r_opt == ARGPARSE_READ_ERROR )
            s = _("read error");
          else if ( arg->r_opt == ARGPARSE_KEYWORD_TOO_LONG )
            s = _("keyword too long");
          else if ( arg->r_opt == ARGPARSE_MISSING_ARG )
            s = _("missing argument");
          else if ( arg->r_opt == ARGPARSE_INVALID_ARG )
            s = _("invalid argument");
          else if ( arg->r_opt == ARGPARSE_INVALID_COMMAND )
            s = _("invalid command");
          else if ( arg->r_opt == ARGPARSE_INVALID_ALIAS )
            s = _("invalid alias definition");
          else if ( arg->r_opt == ARGPARSE_OUT_OF_CORE )
            s = _("out of core");
          else
            s = _("invalid option");
          _gpgrt_log_error ("%s:%u: %s\n",
                            _gpgrt_fname_get (fp), arg->lineno, s);
	}
      else
        {
          s = arg->internal->last? arg->internal->last:"[??]";

          if ( arg->r_opt == ARGPARSE_MISSING_ARG )
            _gpgrt_log_error (_("missing argument for option \"%.50s\"\n"), s);
          else if ( arg->r_opt == ARGPARSE_INVALID_ARG )
            _gpgrt_log_error (_("invalid argument for option \"%.50s\"\n"), s);
          else if ( arg->r_opt == ARGPARSE_UNEXPECTED_ARG )
            _gpgrt_log_error (_("option \"%.50s\" does not expect "
                                "an argument\n"), s);
          else if ( arg->r_opt == ARGPARSE_INVALID_COMMAND )
            _gpgrt_log_error (_("invalid command \"%.50s\"\n"), s);
          else if ( arg->r_opt == ARGPARSE_AMBIGUOUS_OPTION )
            _gpgrt_log_error (_("option \"%.50s\" is ambiguous\n"), s);
          else if ( arg->r_opt == ARGPARSE_AMBIGUOUS_COMMAND )
            _gpgrt_log_error (_("command \"%.50s\" is ambiguous\n"),s );
          else if ( arg->r_opt == ARGPARSE_OUT_OF_CORE )
            _gpgrt_log_error ("%s\n", _("out of core\n"));
          else
            _gpgrt_log_error (_("invalid option \"%.50s\"\n"), s);
	}
      if (arg->err != ARGPARSE_PRINT_WARNING)
        my_exit (arg, 2);
      arg->err = 0;
    }

  /* Zero out the return value union.  */
  arg->r.ret_str = NULL;
  arg->r.ret_long = 0;

  return 0;
}


static void
store_alias( gpgrt_argparse_t *arg, char *name, char *value )
{
    /* TODO: replace this dummy function with a rea one
     * and fix the probelms IRIX has with (ALIAS_DEV)arg..
     * used as lvalue
     */
  (void)arg;
  (void)name;
  (void)value;
#if 0
    ALIAS_DEF a = xmalloc( sizeof *a );
    a->name = name;
    a->value = value;
    a->next = (ALIAS_DEF)arg->internal->aliases;
    (ALIAS_DEF)arg->internal->aliases = a;
#endif
}


/* Return true if KEYWORD is in the ignore-invalid-option list.  */
static int
ignore_invalid_option_p (gpgrt_argparse_t *arg, const char *keyword)
{
  IIO_ITEM_DEF item = arg->internal->iio_list;

  for (; item; item = item->next)
    if (!strcmp (item->name, keyword))
      return 1;
  return 0;
}


/* Add the keywords up to the next LF to the list of to be ignored
   options.  After returning FP will either be at EOF or the next
   character read wll be the first of a new line.  The function
   returns 0 on success or true on malloc failure.  */
static int
ignore_invalid_option_add (gpgrt_argparse_t *arg, estream_t fp)
{
  IIO_ITEM_DEF item;
  int c;
  char name[100];
  int namelen = 0;
  int ready = 0;
  enum { skipWS, collectNAME, skipNAME, addNAME} state = skipWS;

  while (!ready)
    {
      c = _gpgrt_fgetc (fp);
      if (c == '\n')
        ready = 1;
      else if (c == EOF)
        {
          c = '\n';
          ready = 1;
        }
    again:
      switch (state)
        {
        case skipWS:
          if (!isascii (c) || !isspace(c))
            {
              namelen = 0;
              state = collectNAME;
              goto again;
            }
          break;

        case collectNAME:
          if (isspace (c))
            {
              state = addNAME;
              goto again;
            }
          else if (namelen < DIM(name)-1)
            name[namelen++] = c;
          else /* Too long.  */
            state = skipNAME;
          break;

        case skipNAME:
          if (isspace (c))
            {
              state = skipWS;
              goto again;
            }
          break;

        case addNAME:
          name[namelen] = 0;
          if (!ignore_invalid_option_p (arg, name))
            {
              item = xtrymalloc (sizeof *item + namelen);
              if (!item)
                return 1;
              strcpy (item->name, name);
              item->next = (IIO_ITEM_DEF)arg->internal->iio_list;
              arg->internal->iio_list = item;
            }
          state = skipWS;
          goto again;
        }
    }
  return 0;
}


/* Clear the entire ignore-invalid-option list.  */
static void
ignore_invalid_option_clear (gpgrt_argparse_t *arg)
{
  IIO_ITEM_DEF item, tmpitem;

  for (item = arg->internal->iio_list; item; item = tmpitem)
    {
      tmpitem = item->next;
      xfree (item);
    }
  arg->internal->iio_list = NULL;
}



/****************
 * Get options from a file.
 * Lines starting with '#' are comment lines.
 * Syntax is simply a keyword and the argument.
 * Valid keywords are all keywords from the long_opt list without
 * the leading dashes. The special keywords "help", "warranty" and "version"
 * are not valid here.
 * The special keyword "alias" may be used to store alias definitions,
 * which are later expanded like long options.
 * The option
 *   ignore-invalid-option OPTIONNAMEs
 * is recognized and updates a list of option which should be ignored if they
 * are not defined.
 * Caller must free returned strings.
 * If called with FP set to NULL command line args are parse instead.
 *
 * Q: Should we allow the syntax
 *     keyword = value
 *    and accept for boolean options a value of 1/0, yes/no or true/false?
 * Note: Abbreviation of options is here not allowed.
 */
int
_gpgrt_argparse (estream_t fp, gpgrt_argparse_t *arg, gpgrt_opt_t *opts_orig)
{
  gpgrt_opt_t **opts;
  int state, i, c;
  int idx = 0;
  char keyword[100];
  char *buffer = NULL;
  size_t buflen = 0;
  int in_alias=0;
  int unread_buf[3];  /* We use an int so that we can store EOF.  */
  int unread_buf_count = 0;

  if (arg && !opts_orig)
    {
      deinitialize (arg);
      return 0;
    }

  if (!fp) /* Divert to arg_parse() in this case.  */
    return arg_parse (arg, opts_orig);

  if (initialize (arg, opts_orig, fp))
    return (arg->r_opt = ARGPARSE_OUT_OF_CORE);

  opts = arg->internal->opts;

  /* If the LINENO is zero we assume that we are at the start of a
   * file and we skip over a possible Byte Order Mark.  */
  if (!arg->lineno)
    {
      unread_buf[0] = _gpgrt_fgetc (fp);
      unread_buf[1] = _gpgrt_fgetc (fp);
      unread_buf[2] = _gpgrt_fgetc (fp);
      if (unread_buf[0] != 0xef
          || unread_buf[1] != 0xbb
          || unread_buf[2] != 0xbf)
        unread_buf_count = 3;
    }

  /* Find the next keyword.  */
  state = i = 0;
  for (;;)
    {
      if (unread_buf_count)
        c = unread_buf[3 - unread_buf_count--];
      else
        c = _gpgrt_fgetc (fp);
      if (c == '\n' || c== EOF )
        {
          if ( c != EOF )
            arg->lineno++;
          if (state == -1)
            break;
          else if (state == 2)
            {
              keyword[i] = 0;
              for (i=0; opts[i]->short_opt; i++ )
                {
                  if (opts[i]->long_opt && !strcmp (opts[i]->long_opt, keyword))
                    break;
                }
              idx = i;
              arg->r_opt = opts[idx]->short_opt;
              if ((opts[idx]->flags & ARGPARSE_OPT_IGNORE))
                {
                  state = i = 0;
                  continue;
                }
              else if (!opts[idx]->short_opt )
                {
                  if (!strcmp (keyword, "ignore-invalid-option"))
                    {
                      /* No argument - ignore this meta option.  */
                      state = i = 0;
                      continue;
                    }
                  else if (ignore_invalid_option_p (arg, keyword))
                    {
                      /* This invalid option is in the iio list.  */
                      state = i = 0;
                      continue;
                    }
                  arg->r_opt = ((opts[idx]->flags & ARGPARSE_OPT_COMMAND)
                                ? ARGPARSE_INVALID_COMMAND
                                : ARGPARSE_INVALID_OPTION);
                }
              else if (!(opts[idx]->flags & ARGPARSE_TYPE_MASK))
                arg->r_type = 0; /* Does not take an arg. */
              else if ((opts[idx]->flags & ARGPARSE_OPT_OPTIONAL) )
                arg->r_type = 0; /* Arg is optional.  */
              else
                arg->r_opt = ARGPARSE_MISSING_ARG;

              break;
	    }
          else if (state == 3)
            {
              /* No argument found.  */
              if (in_alias)
                arg->r_opt = ARGPARSE_MISSING_ARG;
              else if (!(opts[idx]->flags & ARGPARSE_TYPE_MASK))
                arg->r_type = 0; /* Does not take an arg. */
              else if ((opts[idx]->flags & ARGPARSE_OPT_OPTIONAL))
                arg->r_type = 0; /* No optional argument. */
              else
                arg->r_opt = ARGPARSE_MISSING_ARG;

              break;
	    }
          else if (state == 4)
            {
              /* Has an argument. */
              if (in_alias)
                {
                  if (!buffer)
                    arg->r_opt = ARGPARSE_UNEXPECTED_ARG;
                  else
                    {
                      char *p;

                      buffer[i] = 0;
                      p = strpbrk (buffer, " \t");
                      if (p)
                        {
                          *p++ = 0;
                          trim_spaces (p);
			}
                      if (!p || !*p)
                        {
                          xfree (buffer);
                          arg->r_opt = ARGPARSE_INVALID_ALIAS;
                        }
                      else
                        {
                          store_alias (arg, buffer, p);
                        }
		    }
		}
              else if (!(opts[idx]->flags & ARGPARSE_TYPE_MASK))
                arg->r_opt = ARGPARSE_UNEXPECTED_ARG;
              else
                {
                  char *p;

                  if (!buffer)
                    {
                      keyword[i] = 0;
                      buffer = xtrystrdup (keyword);
                      if (!buffer)
                        arg->r_opt = ARGPARSE_OUT_OF_CORE;
		    }
                  else
                    buffer[i] = 0;

                  if (buffer)
                    {
                      trim_spaces (buffer);
                      p = buffer;
                      if (*p == '"')
                        {
                          /* Remove quotes. */
                          p++;
                          if (*p && p[strlen(p)-1] == '\"' )
                            p[strlen(p)-1] = 0;
                        }
                      if (!set_opt_arg (arg, opts[idx]->flags, p))
                        xfree (buffer);
                      else
                        gpgrt_annotate_leaked_object (buffer);
                    }
                }
              break;
            }
          else if (c == EOF)
            {
              ignore_invalid_option_clear (arg);
              if (_gpgrt_ferror (fp))
                arg->r_opt = ARGPARSE_READ_ERROR;
              else
                arg->r_opt = 0; /* EOF. */
              break;
            }
          state = 0;
          i = 0;
        }
      else if (state == -1)
        ; /* Skip. */
      else if (state == 0 && isascii (c) && isspace(c))
        ; /* Skip leading white space.  */
      else if (state == 0 && c == '#' )
        state = 1;	/* Start of a comment.  */
      else if (state == 1)
        ; /* Skip comments. */
      else if (state == 2 && isascii (c) && isspace(c))
        {
          /* Check keyword.  */
          keyword[i] = 0;
          for (i=0; opts[i]->short_opt; i++ )
            if (opts[i]->long_opt && !strcmp (opts[i]->long_opt, keyword))
              break;
          idx = i;
          arg->r_opt = opts[idx]->short_opt;
          if ((opts[idx]->flags & ARGPARSE_OPT_IGNORE))
            {
              state = 1; /* Process like a comment.  */
            }
          else if (!opts[idx]->short_opt)
            {
              if (!strcmp (keyword, "alias"))
                {
                  in_alias = 1;
                  state = 3;
                }
              else if (!strcmp (keyword, "ignore-invalid-option"))
                {
                  if (ignore_invalid_option_add (arg, fp))
                    {
                      arg->r_opt = ARGPARSE_OUT_OF_CORE;
                      break;
                    }
                  state = i = 0;
                  arg->lineno++;
                }
              else if (ignore_invalid_option_p (arg, keyword))
                state = 1; /* Process like a comment.  */
              else
                {
                  arg->r_opt = ((opts[idx]->flags & ARGPARSE_OPT_COMMAND)
                                ? ARGPARSE_INVALID_COMMAND
                                : ARGPARSE_INVALID_OPTION);
                  state = -1; /* Skip rest of line and leave.  */
                }
            }
          else
            state = 3;
        }
      else if (state == 3)
        {
          /* Skip leading spaces of the argument.  */
          if (!isascii (c) || !isspace(c))
            {
              i = 0;
              keyword[i++] = c;
              state = 4;
            }
        }
      else if (state == 4)
        {
          /* Collect the argument. */
          if (buffer)
            {
              if (i < buflen-1)
                buffer[i++] = c;
              else
                {
                  char *tmp;
                  size_t tmplen = buflen + 50;

                  tmp = xtryrealloc (buffer, tmplen);
                  if (tmp)
                    {
                      buflen = tmplen;
                      buffer = tmp;
                      buffer[i++] = c;
                    }
                  else
                    {
                      xfree (buffer);
                      arg->r_opt = ARGPARSE_OUT_OF_CORE;
                      break;
                    }
                }
            }
          else if (i < DIM(keyword)-1)
            keyword[i++] = c;
          else
            {
              size_t tmplen = DIM(keyword) + 50;
              buffer = xtrymalloc (tmplen);
              if (buffer)
                {
                  buflen = tmplen;
                  memcpy(buffer, keyword, i);
                  buffer[i++] = c;
                }
              else
                {
                  arg->r_opt = ARGPARSE_OUT_OF_CORE;
                  break;
                }
            }
        }
      else if (i >= DIM(keyword)-1)
        {
          arg->r_opt = ARGPARSE_KEYWORD_TOO_LONG;
          state = -1; /* Skip rest of line and leave.  */
        }
      else
        {
          keyword[i++] = c;
          state = 2;
        }
    }

  return arg->r_opt;
}


/* Given the list of options OPTS and a keyword, return the index of
 * the long option macthing KEYWORD.  On error -1 is retruned for not
 * found or -2 for ambigious keyword.  */
static int
find_long_option (gpgrt_argparse_t *arg, gpgrt_opt_t **opts,
                  const char *keyword)
{
  int i;
  size_t n;

  (void)arg;  /* Not yet required.  */

  /* Would be better if we can do a binary search, but it is not
   * possible to reorder our option table because we would mess up our
   * help strings.  What we can do is: Build an option lookup table
   * when this function is first invoked.  */
  if (!*keyword)
    return -1;
  for (i=0; opts[i]->short_opt; i++ )
    if (opts[i]->long_opt && !strcmp (opts[i]->long_opt, keyword))
      return i;
#if 0
  {
    ALIAS_DEF a;
    /* see whether it is an alias */
    for (a = args->internal->aliases; a; a = a->next)
      {
        if (!strcmp( a->name, keyword))
          {
            /* todo: must parse the alias here */
            args->internal->cur_alias = a;
            return -3; /* alias available */
          }
      }
  }
#endif
  /* Not found.  See whether it is an abbreviation.  Aliases may not
   * be abbreviated, though. */
  n = strlen (keyword);
  for (i=0; opts[i]->short_opt; i++)
    {
      if (opts[i]->long_opt && !strncmp (opts[i]->long_opt, keyword, n))
        {
          int j;
          for (j=i+1; opts[j]->short_opt; j++)
            {
              if (opts[j]->long_opt
                  && !strncmp (opts[j]->long_opt, keyword, n)
                  && !(opts[j]->short_opt == opts[i]->short_opt
                       && opts[j]->flags == opts[i]->flags ) )
                return -2;  /* Abbreviation is ambiguous.  */
	    }
          return i;
	}
    }
  return -1;  /* Not found.  */
}


/* The option parser for command line options.  */
static int
arg_parse (gpgrt_argparse_t *arg, gpgrt_opt_t *opts_orig)
{
  int idx;
  gpgrt_opt_t **opts;
  int argc;
  char **argv;
  char *s, *s2;
  int i;

  if (initialize (arg, opts_orig, NULL))
    return (arg->r_opt = ARGPARSE_OUT_OF_CORE);

  opts = arg->internal->opts;
  argc = *arg->argc;
  argv = *arg->argv;
  idx = arg->internal->idx;

  if (!idx && argc && !(arg->flags & ARGPARSE_FLAG_ARG0))
    {
      /* Skip the first argument.  */
      argc--; argv++; idx++;
    }

 next_one:
  if (!argc)
    {
      /* No more args.  */
      arg->r_opt = 0;
      goto leave; /* Ready. */
    }

  s = *argv;
  arg->internal->last = s;

  if (arg->internal->stopped && (arg->flags & ARGPARSE_FLAG_ALL))
    {
      arg->r_opt = ARGPARSE_IS_ARG;  /* Not an option but an argument.  */
      arg->r_type = 2;
      arg->r.ret_str = s;
      argc--; argv++; idx++; /* set to next one */
    }
  else if( arg->internal->stopped )
    {
      arg->r_opt = 0;
      goto leave; /* Ready.  */
    }
  else if ( *s == '-' && s[1] == '-' )
    {
      /* Long option.  */
      char *argpos;

      arg->internal->inarg = 0;
      if (!s[2] && !(arg->flags & ARGPARSE_FLAG_NOSTOP))
        {
          /* Stop option processing.  */
          arg->internal->stopped = 1;
          arg->flags |= ARGPARSE_FLAG_STOP_SEEN;
          argc--; argv++; idx++;
          goto next_one;
	}

      argpos = strchr( s+2, '=' );
      if ( argpos )
        *argpos = 0;
      i = find_long_option ( arg, opts, s+2 );
      if ( argpos )
        *argpos = '=';

      if (i > 0 && opts[i]->short_opt == ARGPARSE_SHORTOPT_HELP)
        show_help (opts, arg->flags);
      else if (i > 0 && opts[i]->short_opt == ARGPARSE_SHORTOPT_VERSION)
        {
          if (!(arg->flags & ARGPARSE_FLAG_NOVERSION))
            {
              show_version ();
              my_exit (arg, 0);
            }
	}
      else if (i > 0 && opts[i]->short_opt == ARGPARSE_SHORTOPT_WARRANTY)
        {
          writestrings (0, _gpgrt_strusage (16), "\n", NULL);
          my_exit (arg, 0);
	}
      else if (i > 0 && opts[i]->short_opt == ARGPARSE_SHORTOPT_DUMP_OPTIONS)
        {
          for (i=0; opts[i]->short_opt; i++ )
            {
              if (opts[i]->long_opt && !(opts[i]->flags & ARGPARSE_OPT_IGNORE))
                writestrings (0, "--", opts[i]->long_opt, "\n", NULL);
	    }
          my_exit (arg, 0);
	}

      if ( i == -2 )
        arg->r_opt = ARGPARSE_AMBIGUOUS_OPTION;
      else if ( i == -1 )
        {
          arg->r_opt = ARGPARSE_INVALID_OPTION;
          arg->r.ret_str = s+2;
	}
      else
        arg->r_opt = opts[i]->short_opt;
      if ( i < 0 )
        ;
      else if ( (opts[i]->flags & ARGPARSE_TYPE_MASK) )
        {
          if ( argpos )
            {
              s2 = argpos+1;
              if ( !*s2 )
                s2 = NULL;
	    }
          else
            s2 = argv[1];
          if ( !s2 && (opts[i]->flags & ARGPARSE_OPT_OPTIONAL) )
            {
              arg->r_type = ARGPARSE_TYPE_NONE; /* Argument is optional.  */
	    }
          else if ( !s2 )
            {
              arg->r_opt = ARGPARSE_MISSING_ARG;
	    }
          else if ( !argpos && *s2 == '-'
                    && (opts[i]->flags & ARGPARSE_OPT_OPTIONAL) )
            {
              /* The argument is optional and the next seems to be an
                 option.  We do not check this possible option but
                 assume no argument */
              arg->r_type = ARGPARSE_TYPE_NONE;
	    }
          else
            {
              set_opt_arg (arg, opts[i]->flags, s2);
              if ( !argpos )
                {
                  argc--; argv++; idx++; /* Skip one.  */
		}
	    }
	}
      else
        {
          /* Does not take an argument. */
          if ( argpos )
            arg->r_type = ARGPARSE_UNEXPECTED_ARG;
          else
            arg->r_type = 0;
	}
      argc--; argv++; idx++; /* Set to next one.  */
    }
    else if ( (*s == '-' && s[1]) || arg->internal->inarg )
      {
        /* Short option.  */
	int dash_kludge = 0;

	i = 0;
	if ( !arg->internal->inarg )
          {
	    arg->internal->inarg++;
	    if ( (arg->flags & ARGPARSE_FLAG_ONEDASH) )
              {
                for (i=0; opts[i]->short_opt; i++ )
                  if ( opts[i]->long_opt && !strcmp (opts[i]->long_opt, s+1))
                    {
                      dash_kludge = 1;
                      break;
		    }
              }
          }
	s += arg->internal->inarg;

	if (!dash_kludge )
          {
	    for (i=0; opts[i]->short_opt; i++ )
              if ( opts[i]->short_opt == *s )
                break;
          }

	if ( !opts[i]->short_opt && ( *s == 'h' || *s == '?' ) )
          show_help (opts, arg->flags);

	arg->r_opt = opts[i]->short_opt;
	if (!opts[i]->short_opt )
          {
	    arg->r_opt = (opts[i]->flags & ARGPARSE_OPT_COMMAND)?
              ARGPARSE_INVALID_COMMAND:ARGPARSE_INVALID_OPTION;
	    arg->internal->inarg++; /* Point to the next arg.  */
	    arg->r.ret_str = s;
          }
	else if ( (opts[i]->flags & ARGPARSE_TYPE_MASK) )
          {
	    if ( s[1] && !dash_kludge )
              {
		s2 = s+1;
		set_opt_arg (arg, opts[i]->flags, s2);
              }
	    else
              {
		s2 = argv[1];
		if ( !s2 && (opts[i]->flags & ARGPARSE_OPT_OPTIONAL) )
                  {
		    arg->r_type = ARGPARSE_TYPE_NONE;
                  }
		else if ( !s2 )
                  {
		    arg->r_opt = ARGPARSE_MISSING_ARG;
                  }
		else if ( *s2 == '-' && s2[1]
                          && (opts[i]->flags & ARGPARSE_OPT_OPTIONAL) )
                  {
		    /* The argument is optional and the next seems to
	               be an option.  We do not check this possible
	               option but assume no argument.  */
		    arg->r_type = ARGPARSE_TYPE_NONE;
                  }
		else
                  {
		    set_opt_arg (arg, opts[i]->flags, s2);
		    argc--; argv++; idx++; /* Skip one.  */
                  }
              }
	    s = "x"; /* This is so that !s[1] yields false.  */
          }
	else
          {
            /* Does not take an argument.  */
	    arg->r_type = ARGPARSE_TYPE_NONE;
	    arg->internal->inarg++; /* Point to the next arg.  */
          }
	if ( !s[1] || dash_kludge )
          {
            /* No more concatenated short options.  */
	    arg->internal->inarg = 0;
	    argc--; argv++; idx++;
          }
      }
  else if ( arg->flags & ARGPARSE_FLAG_MIXED )
    {
      arg->r_opt = ARGPARSE_IS_ARG;
      arg->r_type = 2;
      arg->r.ret_str = s;
      argc--; argv++; idx++; /* Set to next one.  */
    }
  else
    {
      arg->internal->stopped = 1; /* Stop option processing.  */
      goto next_one;
    }

 leave:
  *arg->argc = argc;
  *arg->argv = argv;
  arg->internal->idx = idx;
  return arg->r_opt;
}


/* Returns: -1 on error, 0 for an integer type and 1 for a non integer
   type argument.  */
static int
set_opt_arg (gpgrt_argparse_t *arg, unsigned flags, char *s)
{
  int base = (flags & ARGPARSE_OPT_PREFIX)? 0 : 10;
  long l;

  switch ( (arg->r_type = (flags & ARGPARSE_TYPE_MASK)) )
    {
    case ARGPARSE_TYPE_LONG:
    case ARGPARSE_TYPE_INT:
      errno = 0;
      l = strtol (s, NULL, base);
      if ((l == LONG_MIN || l == LONG_MAX) && errno == ERANGE)
        {
          arg->r_opt = ARGPARSE_INVALID_ARG;
          return -1;
        }
      if (arg->r_type == ARGPARSE_TYPE_LONG)
        arg->r.ret_long = l;
      else if ( (l < 0 && l < INT_MIN) || l > INT_MAX )
        {
          arg->r_opt = ARGPARSE_INVALID_ARG;
          return -1;
        }
      else
        arg->r.ret_int = (int)l;
      return 0;

    case ARGPARSE_TYPE_ULONG:
      while (isascii (*s) && isspace(*s))
        s++;
      if (*s == '-')
        {
          arg->r.ret_ulong = 0;
          arg->r_opt = ARGPARSE_INVALID_ARG;
          return -1;
        }
      errno = 0;
      arg->r.ret_ulong = strtoul (s, NULL, base);
      if (arg->r.ret_ulong == ULONG_MAX && errno == ERANGE)
        {
          arg->r_opt = ARGPARSE_INVALID_ARG;
          return -1;
        }
      return 0;

    case ARGPARSE_TYPE_STRING:
    default:
      arg->r.ret_str = s;
      return 1;
    }
}


/* Return the length of the option O.  This needs to consider the
 * description as weel as the option name.  */
static size_t
long_opt_strlen (gpgrt_opt_t *o)
{
  size_t n = strlen (o->long_opt);

  if ( o->description && *o->description == '|' )
    {
      const char *s;
      int is_utf8 = is_native_utf8 ();

      s=o->description+1;
      if ( *s != '=' )
        n++;
      /* For a (mostly) correct length calculation we exclude
       * continuation bytes (10xxxxxx) if we are on a native utf8
       * terminal. */
      for (; *s && *s != '|'; s++ )
        if ( is_utf8 && (*s&0xc0) != 0x80 )
          n++;
    }
  return n;
}


/****************
 * Print formatted help. The description string has some special
 * meanings:
 *  - A description string which is "@" suppresses help output for
 *    this option
 *  - a description,ine which starts with a '@' and is followed by
 *    any other characters is printed as is; this may be used for examples
 *    ans such.
 *  - A description which starts with a '|' outputs the string between this
 *    bar and the next one as arguments of the long option.
 */
static void
show_help (gpgrt_opt_t **opts, unsigned int flags)
{
  const char *s;
  char tmp[2];

  show_version ();
  writestrings (0, "\n", NULL);
  s = _gpgrt_strusage (42);
  if (s && *s == '1')
    {
      s = _gpgrt_strusage (40);
      writestrings (1, s, NULL);
      if (*s && s[strlen(s)] != '\n')
        writestrings (1, "\n", NULL);
    }
  s = _gpgrt_strusage(41);
  writestrings (0, s, "\n", NULL);
  if ( opts[0]->description )
    {
      /* Auto format the option description.  */
      int i,j, indent;

      /* Get max. length of long options.  */
      for (i=indent=0; opts[i]->short_opt; i++ )
        {
          if ( opts[i]->long_opt )
            if ( !opts[i]->description || *opts[i]->description != '@' )
              if ( (j=long_opt_strlen(opts[i])) > indent && j < 35 )
                indent = j;
	}

      /* Example: " -v, --verbose   Viele Sachen ausgeben" */
      indent += 10;
      if ( *opts[0]->description != '@' )
        writestrings (0, "Options:", "\n", NULL);
      for (i=0; opts[i]->short_opt; i++ )
        {
          s = map_fixed_string (_( opts[i]->description ));
          if ( s && *s== '@' && !s[1] ) /* Hide this line.  */
            continue;
          if ( s && *s == '@' )  /* Unindented comment only line.  */
            {
              for (s++; *s; s++ )
                {
                  if ( *s == '\n' )
                    {
                      if( s[1] )
                        writestrings (0, "\n", NULL);
		    }
                  else
                    {
                      tmp[0] = *s;
                      tmp[1] = 0;
                      writestrings (0, tmp, NULL);
                    }
                }
              writestrings (0, "\n", NULL);
              continue;
	    }

          j = 3;
          if ( opts[i]->short_opt < 256 )
            {
              tmp[0] = opts[i]->short_opt;
              tmp[1] = 0;
              writestrings (0, " -", tmp, NULL );
              if ( !opts[i]->long_opt )
                {
                  if (s && *s == '|' )
                    {
                      writestrings (0, " ", NULL); j++;
                      for (s++ ; *s && *s != '|'; s++, j++ )
                        {
                          tmp[0] = *s;
                          tmp[1] = 0;
                          writestrings (0, tmp, NULL);
                        }
                      if ( *s )
                        s++;
		    }
		}
	    }
          else
            writestrings (0, "   ", NULL);
          if ( opts[i]->long_opt )
            {
              tmp[0] = opts[i]->short_opt < 256?',':' ';
              tmp[1] = 0;
              j += writestrings (0, tmp, " --", opts[i]->long_opt, NULL);
              if (s && *s == '|' )
                {
                  if ( *++s != '=' )
                    {
                      writestrings (0, " ", NULL);
                      j++;
		    }
                  for ( ; *s && *s != '|'; s++, j++ )
                    {
                      tmp[0] = *s;
                      tmp[1] = 0;
                      writestrings (0, tmp, NULL);
                    }
                  if ( *s )
                    s++;
		}
              writestrings (0, "   ", NULL);
              j += 3;
	    }
          for (;j < indent; j++ )
            writestrings (0, " ", NULL);
          if ( s )
            {
              if ( *s && j > indent )
                {
                  writestrings (0, "\n", NULL);
                  for (j=0;j < indent; j++ )
                    writestrings (0, " ", NULL);
		}
              for (; *s; s++ )
                {
                  if ( *s == '\n' )
                    {
                      if ( s[1] )
                        {
                          writestrings (0, "\n", NULL);
                          for (j=0; j < indent; j++ )
                            writestrings (0, " ", NULL);
			}
		    }
                  else
                    {
                      tmp[0] = *s;
                      tmp[1] = 0;
                      writestrings (0, tmp, NULL);
                    }
		}
	    }
          writestrings (0, "\n", NULL);
	}
	if ( (flags & ARGPARSE_FLAG_ONEDASH) )
          writestrings (0, "\n(A single dash may be used "
                        "instead of the double ones)\n", NULL);
    }
  if ( (s=_gpgrt_strusage(19)) )
    {
      writestrings (0, "\n", NULL);
      writestrings (0, s, NULL);
    }
  flushstrings (0);
  exit (0);
}


static void
show_version ()
{
  const char *s;
  int i;

  /* Version line.  */
  writestrings (0, _gpgrt_strusage (11), NULL);
  if ((s=_gpgrt_strusage (12)))
    writestrings (0, " (", s, ")", NULL);
  writestrings (0, " ", _gpgrt_strusage (13), "\n", NULL);
  /* Additional version lines. */
  for (i=20; i < 30; i++)
    if ((s=_gpgrt_strusage (i)))
      writestrings (0, s, "\n", NULL);
  /* Copyright string.  */
  if ((s=_gpgrt_strusage (14)))
    writestrings (0, s, "\n", NULL);
  /* Licence string.  */
  if( (s=_gpgrt_strusage (10)) )
    writestrings (0, s, "\n", NULL);
  /* Copying conditions. */
  if ( (s=_gpgrt_strusage(15)) )
    writestrings (0, s, NULL);
  /* Thanks. */
  if ((s=_gpgrt_strusage(18)))
    writestrings (0, s, NULL);
  /* Additional program info. */
  for (i=30; i < 40; i++ )
    if ( (s=_gpgrt_strusage (i)) )
      writestrings (0, s, NULL);
  flushstrings (0);
}


void
_gpgrt_usage (int level)
{
  const char *p;

  if (!level)
    {
      writestrings (1, _gpgrt_strusage(11), " ", _gpgrt_strusage(13), "; ",
                    _gpgrt_strusage (14), "\n", NULL);
      flushstrings (1);
    }
  else if (level == 1)
    {
      p = _gpgrt_strusage (40);
      writestrings (1, p, NULL);
      if (*p && p[strlen(p)] != '\n')
        writestrings (1, "\n", NULL);
      exit (2);
    }
  else if (level == 2)
    {
      p = _gpgrt_strusage (42);
      if (p && *p == '1')
        {
          p = _gpgrt_strusage (40);
          writestrings (1, p, NULL);
          if (*p && p[strlen(p)] != '\n')
            writestrings (1, "\n", NULL);
        }
      writestrings (0, _gpgrt_strusage(41), "\n", NULL);
      exit (0);
    }
}

/* Level
 *     0: Print copyright string to stderr
 *     1: Print a short usage hint to stderr and terminate
 *     2: Print a long usage hint to stdout and terminate
 *     8: Return NULL for UTF-8 or string with the native charset.
 *     9: Return the SPDX License tag.
 *    10: Return license info string
 *    11: Return the name of the program
 *    12: Return optional name of package which includes this program.
 *    13: version  string
 *    14: copyright string
 *    15: Short copying conditions (with LFs)
 *    16: Long copying conditions (with LFs)
 *    17: Optional printable OS name
 *    18: Optional thanks list (with LFs)
 *    19: Bug report info
 *20..29: Additional lib version strings.
 *30..39: Additional program info (with LFs)
 *    40: short usage note (with LF)
 *    41: long usage note (with LF)
 *    42: Flag string:
 *          First char is '1':
 *             The short usage notes needs to be printed
 *             before the long usage note.
 */
const char *
_gpgrt_strusage (int level)
{
  const char *p = strusage_handler? strusage_handler(level) : NULL;
  const char *tmp;

  if ( p )
    return map_fixed_string (p);

  switch ( level )
    {

    case 8: break; /* Default to utf-8.  */
    case 9:
      p = "GPL-3.0-or-later"; /* Suggested license.  */
      break;

    case 10:
      tmp = _gpgrt_strusage (9);
      if (tmp && !strcmp (tmp, "GPL-2.0-or-later"))
        p = ("License GPL-2.0-or-later <https://gnu.org/licenses/>");
      else if (tmp && !strcmp (tmp, "LGPL-2.1-or-later"))
        p = ("License LGPL-2.1-or-later <https://gnu.org/licenses/>");
      else /* Default to GPLv3+.  */
        p = ("License GPL-3.0-or-later <https://gnu.org/licenses/gpl.html>");
      break;
    case 11: p = "foo"; break;
    case 13: p = "0.0"; break;
    case 14: p = "Copyright (C) YEAR NAME"; break;
    case 15: p =
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n";
      break;
    case 16:
      tmp = _gpgrt_strusage (9);
      if (tmp && !strcmp (tmp, "GPL-2.0-or-later"))
        p =
"This is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 2 of the License, or\n"
"(at your option) any later version.\n\n"
"It is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n\n"
"You should have received a copy of the GNU General Public License\n"
"along with this software.  If not, see <https://gnu.org/licenses/>.\n";
      else if (tmp && !strcmp (tmp, "LGPL-2.1-or-later"))
        p =
"This is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU Lesser General Public License as\n"
"published by the Free Software Foundation; either version 2.1 of\n"
"the License, or (at your option) any later version.\n\n"
"It is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU Lesser General Public License for more details.\n\n"
"You should have received a copy of the GNU General Public License\n"
"along with this software.  If not, see <https://gnu.org/licenses/>.\n";
      else /* Default */
        p =
"This is free software; you can redistribute it and/or modify\n"
"it under the terms of the GNU General Public License as published by\n"
"the Free Software Foundation; either version 3 of the License, or\n"
"(at your option) any later version.\n\n"
"It is distributed in the hope that it will be useful,\n"
"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
"GNU General Public License for more details.\n\n"
"You should have received a copy of the GNU General Public License\n"
"along with this software.  If not, see <https://gnu.org/licenses/>.\n";
      break;
    case 40: /* short and long usage */
    case 41: p = ""; break;
    }

  return p;
}


/* Set the usage handler.  This function is basically a constructor.  */
void
_gpgrt_set_strusage (const char *(*f)(int) )
{
  strusage_handler = f;
}


/* Set a function to write strings which is then used instead of
 * estream.  The first arg of that function is MODE and the second the
 * STRING to write.  A mode of 1 is used for writing to stdout and a
 * mode of 2 to write to stderr.  Other modes are reserved and should
 * not output anything.  A NULL for STRING requests a flush.  */
void
_gpgrt_set_usage_outfnc (int (*f)(int, const char *))
{
  custom_outfnc = f;
}


/* Register function F as a string mapper which takes a string as
 * argument, replaces known "@FOO@" style macros and returns a new
 * fixed string.  Warning: The input STRING must have been allocated
 * statically.  */
void
_gpgrt_set_fixed_string_mapper (const char *(*f)(const char*))
{
  fixed_string_mapper = f;
}
