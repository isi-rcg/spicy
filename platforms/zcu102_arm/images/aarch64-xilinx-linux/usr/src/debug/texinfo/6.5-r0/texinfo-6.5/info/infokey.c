/* infokey.c -- compile ~/.infokey to ~/.info.
   $Id: infokey.c 7947 2017-09-02 13:07:20Z gavin $

   Copyright 1999, 2001, 2002, 2003, 2004, 2005, 2007, 2008, 2009,
   2010, 2011, 2012, 2013, 2014 Free Software Foundation, Inc.

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

   Originally written by Andrew Bettison. */

#include "info.h"
#include "doc.h"
#include "session.h"
#include "funs.h"
#include "getopt.h"
#include "variables.h"

extern char *program_name;  /* in info.c */

enum sect_e
  {
    info = 0,
    ea = 1,
    var = 2
  };

static void syntax_error (const char *filename, unsigned int linenum,
			  const char *fmt, ...) TEXINFO_PRINTFLIKE(3,4);

/* Compilation - the real work.

	Source file syntax
	------------------
	The source file is a line-based text file with the following
	structure:

		# comments
		# more comments

		#info
		u	prev-line
		d	next-line
		^a	invalid		# just beep
		\ku	prev-line
		#stop
		\kd	next-line
		q	quit		# of course!

		#echo-area
		^a	echo-area-beg-of-line
		^e	echo-area-end-of-line
		\kr	echo-area-forward
		\kl	echo-area-backward
		\kh	echo-area-beg-of-line
		\ke	echo-area-end-of-line

		#var
		scroll-step=1
		ISO-Latin=Off

	Lines starting with '#' are comments, and are ignored.  Blank
	lines are ignored.  Each section is introduced by one of the
	following lines:

		#info
		#echo-area
		#var

	The sections may occur in any order.  Each section may be
	omitted completely.  If the 'info' section is the first in the
	file, its '#info' line may be omitted.

	The 'info' and 'echo-area' sections
	-----------------------------------
	Each line in the 'info' or 'echo-area' sections has the
	following syntax:

		key-sequence SPACE action-name [ SPACE [ # comment ] ] \n

	Where SPACE is one or more white space characters excluding
	newline, "action-name" is the name of a GNU Info command,
	"comment" is any sequence of characters excluding newline, and
	"key-sequence" is a concatenation of one or more key definitions
	using the following syntax:

	   1.	A carat ^ followed by one character indicates a single
	   	control character;

	   2.	A backslash \ followed by one, two, or three octal
		digits indicates a single character having that ASCII
		code;

	   3.	\n indicates a single NEWLINE;
		\e indicates a single ESC;
		\r indicates a single CR;
		\t indicates a single TAB;
		\b indicates a single BACKSPACE;

	   4.	\ku indicates the Up Arrow key;
	   	\kd indicates the Down Arrow key;
	   	\kl indicates the Left Arrow key;
	   	\kr indicates the Right Arrow key;
	   	\kP indicates the Page Up (PRIOR) key;
	   	\kN indicates the Page Down (NEXT) key;
	   	\kh indicates the Home key;
	   	\ke indicates the End key;
	   	\kx indicates the DEL key;
		\k followed by any other character indicates a single
		control-K, and the following character is interpreted
		as in rules 1, 2, 3, 5 and 6.

	   5.	\m followed by any sequence defined in rules 1, 2, 3, 4
		or 6 indicates the "Meta" modification of that key.

	   6.	A backslash \ followed by any character not described
	   	above indicates that character itself.  In particular:
		\\ indicates a single backslash \,
		\  (backslash-space) indicates a single space,
		\^ indicates a single caret ^,

	If the following line:

		#stop

	occurs anywhere in an 'info' or 'echo-area' section, that
	indicates to GNU Info to suppress all of its default key
	bindings in that context.

	The 'var' section
	-----------------
	Each line in the 'var' section has the following syntax:

		variable-name = value \n

	Where "variable-name" is the name of a GNU Info variable and
	"value" is the value that GNU Info will assign to that variable
	when commencing execution.  There must be no white space in the
	variable name, nor between the variable name and the '='.  All
	characters immediately following the '=', up to but not
	including the terminating newline, are considered to be the
	value that will be assigned.  In other words, white space
	following the '=' is not ignored.
 */

static int lookup_action (const char *actname);

/* Read the init file.  Return true if no error was encountered.  Set
   SUPPRESS_INFO or SUPPRESS_EA to true if the init file specified to ignore
   default key bindings. */
int
compile (FILE *fp, const char *filename, int *suppress_info, int *suppress_ea)
{
  int error = 0; /* Set if there was a fatal error in reading init file. */
  char rescan = 0; /* Whether to reuse the same character when moving onto the
                      next state. */
  unsigned int lnum = 0;
  int c = 0;

  /* This parser is a true state machine, with no sneaky fetching
     of input characters inside the main loop.  In other words, all
     state is fully represented by the following variables:
   */
  enum
    {
      start_of_line,
      start_of_comment,
      in_line_comment,
      in_trailing_comment,
      get_keyseq,
      got_keyseq,
      get_action,
      got_action,
      get_varname,
      got_varname,
      get_equals,
      got_equals,
      get_value
    }
  state = start_of_line;
  enum sect_e section = info;
  enum
    {
      normal,
      slosh,
      control,
      octal,
      special_key
    }
  seqstate = normal;	/* used if state == get_keyseq */
  char meta = 0;
  char ocnt = 0;	/* used if state == get_keyseq && seqstate == octal */

  /* Data is accumulated in the following variables.  The code
     avoids overflowing these strings, and throws an error
     where appropriate if a string limit is exceeded.  These string
     lengths are arbitrary (and should be large enough) and their
     lengths are not hard-coded anywhere else, so increasing them
     here will not break anything.  */
  char oval = 0;
  char comment[10];
  unsigned int clen = 0;
  int seq[20];
  unsigned int slen = 0;
  char act[80];
  unsigned int alen = 0;
  char varn[80];
  unsigned int varlen = 0;
  char val[80];
  unsigned int vallen = 0;

#define	To_seq(c) \
		  do { \
		    if (slen < sizeof seq/sizeof(int)) \
		      seq[slen++] = meta ? KEYMAP_META(c) : (c); \
		    else \
		      { \
			syntax_error(filename, lnum, \
				     _("key sequence too long")); \
			error = 1; \
		      } \
		    meta = 0; \
		  } while (0)

  while (!error && (rescan || (c = fgetc (fp)) != EOF))
    {
      rescan = 0;
      switch (state)
	{
	case start_of_line:
	  lnum++;
	  if (c == '#')
	    state = start_of_comment;
	  else if (c != '\n')
	    {
	      switch (section)
		{
		case info:
		case ea:
		  state = get_keyseq;
		  seqstate = normal;
		  slen = 0;
		  break;
		case var:
		  state = get_varname;
		  varlen = 0;
		  break;
		}
	      rescan = 1;
	    }
	  break;

	case start_of_comment:
	  clen = 0;
	  state = in_line_comment;
	  /* fall through */
	case in_line_comment:
	  if (c == '\n')
	    {
	      state = start_of_line;
	      comment[clen] = '\0';
	      if (strcmp (comment, "info") == 0)
		section = info;
	      else if (strcmp (comment, "echo-area") == 0)
		section = ea;
	      else if (strcmp (comment, "var") == 0)
		section = var;
	      else if (strcmp (comment, "stop") == 0
		       && (section == info || section == ea))
                {
                  if (section == info)
                    *suppress_info = 1;
                  else
                    *suppress_ea = 1;
                }
	    }
	  else if (clen < sizeof comment - 1)
	    comment[clen++] = c;
	  break;

	case in_trailing_comment:
	  if (c == '\n')
	    state = start_of_line;
	  break;

	case get_keyseq:
	  switch (seqstate)
	    {
	    case normal:
	      if (c == '\n' || isspace (c))
		{
		  state = got_keyseq;
		  rescan = 1;
		  if (slen == 0)
		    {
		      syntax_error (filename, lnum, _("missing key sequence"));
		      error = 1;
		    }
		}
	      else if (c == '\\')
		seqstate = slosh;
	      else if (c == '^')
		seqstate = control;
	      else
		To_seq (c);
	      break;

	    case slosh:
	      switch (c)
		{
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		  seqstate = octal;
		  oval = c - '0';
		  ocnt = 1;
		  break;
		case 'b':
		  To_seq ('\b');
		  seqstate = normal;
		  break;
		case 'e':
		  To_seq ('\033');
		  seqstate = normal;
		  break;
		case 'n':
		  To_seq ('\n');
		  seqstate = normal;
		  break;
		case 'r':
		  To_seq ('\r');
		  seqstate = normal;
		  break;
		case 't':
		  To_seq ('\t');
		  seqstate = normal;
		  break;
		case 'm':
		  meta = 1;
		  seqstate = normal;
		  break;
		case 'k':
		  seqstate = special_key;
		  break;
		default:
		  /* Backslash followed by any other char
		     just means that char.  */
		  To_seq (c);
		  seqstate = normal;
		  break;
		}
	      break;

	    case octal:
	      switch (c)
		{
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		  if (++ocnt <= 3)
		    oval = oval * 8 + c - '0';
		  if (ocnt == 3)
		    seqstate = normal;
		  break;
		default:
		  ocnt = 4;
		  seqstate = normal;
		  rescan = 1;
		  break;
		}
	      if (seqstate != octal)
		{
		  if (oval)
		    To_seq (oval);
		  else
		    {
		      syntax_error (filename, lnum,
				    _("NUL character (\\000) not permitted"));
		      error = 1;
		    }
		}
	      break;

	    case special_key:
	      switch (c)
		{
		case 'u': To_seq (KEY_UP_ARROW); break;
		case 'd': To_seq (KEY_DOWN_ARROW); break;
		case 'r': To_seq (KEY_RIGHT_ARROW); break;
		case 'l': To_seq (KEY_LEFT_ARROW); break;
		case 'U': To_seq (KEY_PAGE_UP); break;
		case 'D': To_seq (KEY_PAGE_DOWN); break;
		case 'h': To_seq (KEY_HOME); break;
		case 'e': To_seq (KEY_END); break;
		case 'x': To_seq (KEY_DELETE); break;
		default:  To_seq (c); rescan = 1; break;
		}
	      seqstate = normal;
	      break;

	    case control:
	      if (CONTROL (c))
		To_seq (CONTROL (c));
	      else
		{
		  syntax_error (filename, lnum,
				_("NUL character (^%c) not permitted"), c);
		  error = 1;
		}
	      seqstate = normal;
	      break;
	    }
	  break;

	case got_keyseq:
	  if (isspace (c) && c != '\n')
	    break;
	  state = get_action;
	  alen = 0;
	  /* fall through */
	case get_action:
	  if (c == '\n' || isspace (c))
	    {
	      int a;

	      state = got_action;
	      rescan = 1;
	      if (alen == 0)
		{
		  syntax_error (filename, lnum, _("missing action name"));
		  error = 1;
		}
	      else
		{
                  int keymap_bind_keyseq (Keymap, int *, KEYMAP_ENTRY *);

		  act[alen] = '\0';
		  a = lookup_action (act);
                  if (a == A_info_menu_digit)
		    {
                      /* Only allow "1 menu-digit".  (This is useful if
                         this default binding is disabled with "#stop".)
                         E.g. do not allow "b menu-digit".  */
                      if (seq[0] != '1' || seq[1] != '\0'
                          || section != info)
                        {
                          syntax_error (filename, lnum,
                                 _("cannot bind key sequence to menu-digit"));
                        }
                      else
                        {
                          /* Bind each key from '1' to '9' to 'menu-digit'. */
                          KEYMAP_ENTRY ke;
                          int i;
                      
                          ke.type = ISFUNC;
                          ke.value.function = &function_doc_array[a];

                          for (i = '1'; i <= '9'; i++)
                            {
                              seq[0] = i;
                              keymap_bind_keyseq (info_keymap, seq, &ke);
                            }
                        }
		    }
		  else if (a == -1)
		    {
                      /* Print an error message, but keep going (don't set
                         error = 1) for compatibility with infokey files aimed
                         at future versions which may have different
                         actions. */
		      syntax_error (filename, lnum, _("unknown action `%s'"),
				    act);
		    }
                  else
		    {
                      KEYMAP_ENTRY ke;
                      static InfoCommand invalid_function = { 0 };
                      
                      ke.type = ISFUNC;
                      ke.value.function = a != A_INVALID
                                            ? &function_doc_array[a]
                                            : &invalid_function;
                      To_seq (0);

                      if (section == info)
                        keymap_bind_keyseq (info_keymap, seq, &ke);
                      else /* section == ea */
                        keymap_bind_keyseq (echo_area_keymap, seq, &ke);
		    }
		}
	    }
	  else if (alen < sizeof act - 1)
	    act[alen++] = c;
	  else
	    {
	      syntax_error (filename, lnum, _("action name too long"));
	      error = 1;
	    }
	  break;

	case got_action:
	  if (c == '#')
	    state = in_trailing_comment;
	  else if (c == '\n')
	    state = start_of_line;
	  else if (!isspace (c))
	    {
	      syntax_error (filename, lnum,
			    _("extra characters following action `%s'"),
			    act);
	      error = 1;
	    }
	  break;

	case get_varname:
	  if (c == '=')
	    {
	      if (varlen == 0)
		{
		  syntax_error (filename, lnum, _("missing variable name"));
		  error = 1;
		}
	      state = get_value;
	      vallen = 0;
	    }
	  else if (c == '\n' || isspace (c))
	    {
	      syntax_error (filename, lnum,
			    _("missing `=' immediately after variable name"));
	      error = 1;
	    }
	  else if (varlen < sizeof varn - 1)
	    varn[varlen++] = c;
	  else
	    {
	      syntax_error (filename, lnum, _("variable name too long"));
	      error = 1;
	    }
	  break;

	case get_value:
	  if (c == '\n')
	    {
              VARIABLE_ALIST *v;

              state = start_of_line;
              varn[varlen] = '\0';
              val[vallen] = '\0';
              v = variable_by_name (varn);
              if (!v)
                info_error (_("%s: no such variable"), varn);
              else if (!set_variable_to_value (v, val, SET_IN_CONFIG_FILE))
                info_error (_("value %s is not valid for variable %s"),
                              val, varn);
	    }
	  else if (vallen < sizeof val - 1)
	    val[vallen++] = c;
	  else
	    {
	      syntax_error (filename, lnum, _("value too long"));
	      error = 1;
	    }
	  break;

        case get_equals:
        case got_equals:
        case got_varname:
          break;
	}
    }

#undef To_seq

  return !error;
}

/* Return the numeric code of an Info command given its name.  If not found,
   return -1.  This uses the auto-generated array in doc.c. */
static int
lookup_action (const char *name)
{
  int i;

  if (!strcmp (name, "invalid"))
    return A_INVALID;
  for (i = 0; function_doc_array[i].func_name; i++)
    if (!strcmp (function_doc_array[i].func_name, name))
      return i;
  return -1;
}



/* Error handling. */

/* Give the user a generic error message in the form
	progname: message
 */
static void
syntax_error (const char *filename,
	      unsigned int linenum, const char *fmt, ...)
{
  va_list ap;
  
  fprintf (stderr, "%s: ", program_name);
  fprintf (stderr, _("\"%s\", line %u: "), filename, linenum);
  va_start(ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end(ap);
  fprintf (stderr, "\n");
}


