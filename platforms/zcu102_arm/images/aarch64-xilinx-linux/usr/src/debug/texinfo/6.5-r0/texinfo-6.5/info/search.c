/* search.c -- searching large bodies of text.
   $Id: search.c 7907 2017-07-05 19:16:44Z gavin $

   Copyright 1993, 1997, 1998, 2002, 2004, 2007, 2008, 2009, 2011, 2013,
   2014, 2015, 2016, 2017 Free Software Foundation, Inc.

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

   Originally written by Brian Fox. */

#include "info.h"
#include <regex.h>

#include "session.h"
#include "info-utils.h"
#include "search.h"


/* **************************************************************** */
/*                                                                  */
/*                 The Actual Searching Functions                   */
/*                                                                  */
/* **************************************************************** */

/* Search forwards or backwards for the text delimited by BINDING.
   The search is forwards if BINDING->start is greater than BINDING->end. */
enum search_result
search (char *string, SEARCH_BINDING *binding, long *poff)
{
  enum search_result result;

  /* If the search is backwards, then search backwards, otherwise forwards. */
  if (binding->start > binding->end)
    result = search_backward (string, binding, poff);
  else
    result = search_forward (string, binding, poff);

  return result;
}

/* Expand \n and \t in regexp to newlines and tabs */
static char *
regexp_expand_newlines_and_tabs (char *regexp)
{
  char *unescaped_regexp = xmalloc (1 + strlen (regexp));
  char *p, *q;

  for (p = regexp, q = unescaped_regexp; *p != '\0'; p++, q++)
    {
      if (*p == '\\')
        switch(*++p)
          {
          case 'n':
            *q = '\n';
            break;
          case 't':
            *q = '\t';
            break;
          case '\0':
            *q = '\\';
            p--;
            break;
          default:
            *q++ = '\\';
            *q = *p;
            break;
          }
      else
        *q = *p;
    }
  *q = '\0';

  return unescaped_regexp;
}

/* Escape any special characters in SEARCH_STRING. */
static char *
regexp_escape_string (char *search_string)
{
  char *special_chars = "\\[]^$.*(){}|+?";
  char *p, *q;

  char *escaped_string = xmalloc (strlen (search_string) * 2 + 1);

  for (p = search_string, q = escaped_string; *p != '\0'; )
    {
      if (strchr (special_chars, *p))
        {
          *q++ = '\\';
        }
      *q++ = *p++;
    }

  *q = '\0';

  return escaped_string;
}


static void
extend_matches (MATCH_STATE *state)
{
  regmatch_t *matches = state->matches;
  size_t match_alloc = state->match_alloc;
  size_t match_count = state->match_count;
  char *buffer = state->buffer;
  size_t buflen = state->buflen;

  regoff_t offset = 0;
  char saved_char;
  size_t initial_match_count = match_count;

  if (state->finished)
    return;

  saved_char = buffer[buflen];
  buffer[buflen] = '\0';

  if (match_count > 0)
    {
      offset = matches[match_count - 1].rm_eo;

      /* move past zero-length match */
      if (offset == matches[match_count - 1].rm_so)
        offset++;
    }

  while (offset < buflen && match_count < initial_match_count + 5)
    {
      int result = 0;
      regmatch_t m;

      result = regexec (&state->regex, &buffer[offset], 1, &m, REG_NOTBOL);
      if (result == 0)
        {
          if (match_count == match_alloc)
            {
              /* The match list is full. */
              if (match_alloc == 0)
                match_alloc = 50;
              matches = x2nrealloc
                (matches, &match_alloc, sizeof matches[0]);
            }

          matches[match_count] = m;
          matches[match_count].rm_so += offset;
          matches[match_count].rm_eo += offset;
          offset = matches[match_count++].rm_eo;

          if (m.rm_eo == 0)
            offset++; /* Avoid finding match again for a pattern of "$". */
        }
      else
        {
          state->finished = 1;
          break;
        }
    }
  buffer[buflen] = saved_char;

  state->matches = matches;
  state->match_alloc = match_alloc;
  state->match_count = match_count;
}

/* Search BUFFER for REGEXP.  If matches are found, pass back the list of 
   matches in MATCH_STATE. */
enum search_result
regexp_search (char *regexp, int is_literal, int is_insensitive,
               char *buffer, size_t buflen,
               MATCH_STATE *match_state)
{
  regex_t preg; /* Compiled pattern buffer for regexp. */
  int result;
  char *regexp_str;

  if (!is_literal)
    regexp_str = regexp_expand_newlines_and_tabs (regexp);
  else
    regexp_str = regexp_escape_string (regexp);

  result = regcomp (&preg, regexp_str,
                    REG_EXTENDED | REG_NEWLINE
                    | (is_insensitive ? REG_ICASE : 0));
  free (regexp_str);

  if (result != 0)
    {
      int size = regerror (result, &preg, NULL, 0);
      char *buf = xmalloc (size);
      regerror (result, &preg, buf, size);
      info_error (_("regexp error: %s"), buf);
      free (buf);
      return search_invalid;
    }

  match_state->matches = 0;
  match_state->match_count = 0;
  match_state->match_alloc = 0;
  match_state->finished = 0;
  match_state->regex = preg;
  match_state->buffer = buffer;
  match_state->buflen = buflen;

  extend_matches (match_state);

  if (match_state->match_count == 0)
    {
      free_matches (match_state);
      return search_not_found;
    }
  else
    return search_success;
}

/* Search forwards for STRING through the text delimited in BINDING. */
enum search_result
search_forward (char *string, SEARCH_BINDING *binding, long *poff)
{
  register int c, i, len;
  register char *buff, *end;
  char *alternate = NULL;

  len = strlen (string);

  /* We match characters in the search buffer against STRING and ALTERNATE.
     ALTERNATE is a case reversed version of STRING; this is cheaper than
     case folding each character before comparison.   Alternate is only
     used if the case folding bit is turned on in the passed BINDING. */

  if (binding->flags & S_FoldCase)
    {
      alternate = xstrdup (string);

      for (i = 0; i < len; i++)
        {
          if (islower (alternate[i]))
            alternate[i] = toupper (alternate[i]);
          else if (isupper (alternate[i]))
            alternate[i] = tolower (alternate[i]);
        }
    }

  buff = binding->buffer + binding->start;
  end = binding->buffer + binding->end + 1;

  while (buff < (end - len))
    {
      for (i = 0; i < len; i++)
        {
          c = buff[i];

          if ((c != string[i]) && (!alternate || c != alternate[i]))
            break;
        }

      if (!string[i])
        {
          if (alternate)
            free (alternate);
          if (binding->flags & S_SkipDest)
            buff += len;
          *poff = buff - binding->buffer;
	  return search_success;
        }

      buff++;
    }

  if (alternate)
    free (alternate);

  return search_not_found;
}

/* Search for STRING backwards through the text delimited in BINDING. */
enum search_result
search_backward (char *input_string, SEARCH_BINDING *binding, long *poff)
{
  register int c, i, len;
  register char *buff, *end;
  char *string;
  char *alternate = NULL;

  len = strlen (input_string);

  /* Reverse the characters in the search string. */
  string = xmalloc (1 + len);
  for (c = 0, i = len - 1; input_string[c]; c++, i--)
    string[i] = input_string[c];

  string[c] = '\0';

  /* We match characters in the search buffer against STRING and ALTERNATE.
     ALTERNATE is a case reversed version of STRING; this is cheaper than
     case folding each character before comparison.   ALTERNATE is only
     used if the case folding bit is turned on in the passed BINDING. */

  if (binding->flags & S_FoldCase)
    {
      alternate = xstrdup (string);

      for (i = 0; i < len; i++)
        {
          if (islower (alternate[i]))
            alternate[i] = toupper (alternate[i]);
          else if (isupper (alternate[i]))
            alternate[i] = tolower (alternate[i]);
        }
    }

  buff = binding->buffer + binding->start - 1;
  end = binding->buffer + binding->end;

  while (buff > (end + len))
    {
      for (i = 0; i < len; i++)
        {
          c = *(buff - i);

          if (c != string[i] && (!alternate || c != alternate[i]))
            break;
        }

      if (!string[i])
        {
          free (string);
          if (alternate)
            free (alternate);

          if (binding->flags & S_SkipDest)
            buff -= len;
          *poff = 1 + buff - binding->buffer;
	  return search_success;
        }

      buff--;
    }

  free (string);
  if (alternate)
    free (alternate);

  return search_not_found;
}

/* Find STRING in LINE, returning the offset of the end of the string.
   Return an offset of -1 if STRING does not appear in LINE.  The search
   is bound by the end of the line (i.e., either NEWLINE or 0). */
int
string_in_line (char *string, char *line)
{
  register int end;
  SEARCH_BINDING binding;
  long offset;
  
  /* Find the end of the line. */
  for (end = 0; line[end] && line[end] != '\n'; end++);

  /* Search for STRING within these confines. */
  binding.buffer = line;
  binding.start = 0;
  binding.end = end;
  binding.flags = S_FoldCase | S_SkipDest;

  if (search_forward (string, &binding, &offset) == search_success)
    return offset;
  return -1;
}

/* Return non-zero if STRING is the first text to appear at BINDING. */
int
looking_at (char *string, SEARCH_BINDING *binding)
{
  long search_end;

  if (search (string, binding, &search_end) != search_success)
    return 0;

  /* If the string was not found, SEARCH_END is -1.  If the string was found,
     but not right away, SEARCH_END is != binding->start.  Otherwise, the
     string was found at binding->start. */
  return search_end == binding->start;
}

/* Return non-zero if POINTER is looking at the text at STRING before an 
   end-of-line. */
int
looking_at_line (char *string, char *pointer)
{
  int len;

  len = strlen (string);
  if (strncasecmp (pointer, string, len) != 0)
    return 0;

  pointer += len;
  if (*pointer == '\n' || !strncmp (pointer, "\r\n", 2)
      || *pointer == '\0')
    return 1;
  return 0;
}

/* **************************************************************** */
/*                                                                  */
/*                      Accessing matches                           */
/*                                                                  */
/* **************************************************************** */
/* Search forwards or backwards for entries in MATCHES that start within
   the search area.  The search is forwards if DIR > 0, backward if
   DIR < 0.  Return index of match in *MATCH_INDEX. */
enum search_result
match_in_match_list (MATCH_STATE *match_state,
                     long start, long end, int dir,
                     int *match_index)
{
  regmatch_t *matches = match_state->matches;
  size_t match_count = match_state->match_count;

  int i;
  int index = -1;

  for (i = 0; i < match_count || !match_state->finished; i++)
    {
      /* get more matches as we need them */
      if (i == match_count)
        {
          extend_matches (match_state);
          matches = match_state->matches;
          match_count = match_state->match_count;

          if (i == match_count)
            break;
        }

      if (matches[i].rm_so >= end)
        break; /* No  more matches found in search area. */

      if (matches[i].rm_so >= start)
        {
          index = i;
          if (dir > 0)
            {
              *match_index = index;
              return search_success;
            }
        }
    }

  if (index != -1)
    {
      *match_index = index;
      return search_success;
    }

  /* not found */
  return search_not_found;
}

/* Return match INDEX in STATE.  INDEX must be a valid index. */
regmatch_t
match_by_index (MATCH_STATE *state, int index)
{
  while (state->match_alloc <= index)
    extend_matches (state);
  return state->matches[index];
}

/* Free and clear all data in STATE. */
void
free_matches (MATCH_STATE *state)
{
  free (state->matches);
  state->matches = 0;
  state->match_count = state->match_alloc = state->finished = 0;
  state->buffer = 0; /* do not free as it is kept elsewhere */
  state->buflen = 0;
  regfree (&state->regex);
}

int
matches_ready (MATCH_STATE *state)
{
  return state->matches ? 1 : 0;
}

/* Starting at index *MATCH_INDEX, decide if we are inside a match
   in MATCHES at offset OFF.  The matches are assumed not to overlap
   and to be in order. */
void
decide_if_in_match (long off, int *in_match,
                    MATCH_STATE *matches, size_t *match_index)
{
  size_t i = *match_index;
  int m = *in_match;

  for (; !at_end_of_matches (matches, i); i++)
    {
      if (match_by_index (matches, i).rm_so > off)
        break;

      m = 1;

      if (match_by_index (matches, i).rm_eo > off)
        break;

      m = 0;
    }

  *match_index = i;
  *in_match = m;
}

/* Used for iterating through a match list. */
int
at_end_of_matches (MATCH_STATE *state, int index)
{
  if (index < state->match_count)
    return 0;
  else
    {
      if (!state->finished)
        extend_matches (state);

      if (state->finished)
        return (state->match_count == index) ? 1 : 0;
      else
        return 0;
    }
}



/* **************************************************************** */
/*                                                                  */
/*                    Small String Searches                         */
/*                                                                  */
/* **************************************************************** */

/* Function names that start with "skip" are passed a string, and return
   an offset from the start of that string.  Function names that start
   with "find" are passed a SEARCH_BINDING, and return an absolute position
   marker of the item being searched for.  "Find" functions return a value
   of -1 if the item being looked for couldn't be found. */

/* Return the index of the first non-whitespace character in STRING. */
int
skip_whitespace (char *string)
{
  register int i;

  for (i = 0; string && whitespace (string[i]); i++);
  return i;
}

/* Return the index of the first non-whitespace or newline character in
   STRING. */
int
skip_whitespace_and_newlines (char *string)
{
  register int i;

  for (i = 0; string && whitespace_or_newline (string[i]); i++);
  return i;
}

/* Return the index of the first whitespace character in STRING. */
int
skip_non_whitespace (char *string)
{
  register int i;

  for (i = 0; string && string[i] && !whitespace (string[i]); i++);
  return i;
}

/* **************************************************************** */
/*                                                                  */
/*                   Searching FILE_BUFFER's                        */
/*                                                                  */
/* **************************************************************** */

/* Return the absolute position of the first occurence of a node separator
   starting in BINDING->buffer between BINDING->start and BINDING->end 
   inclusive.  Return -1 if no node separator was found. */
long
find_node_separator (SEARCH_BINDING *binding)
{
  register long i;
  char *body;
  int dir;

  body = binding->buffer;
  dir = binding->start < binding->end ? 1 : -1;

  /* A node is started by [^L]^_[^L][\r]\n.  That is to say, the C-l's are
     optional, but the US and NEWLINE are not.  This separator holds
     true for all separated elements in an Info file, including the tags
     table (if present) and the indirect tags table (if present). */
  i = binding->start;
  while (1)
    {
      /* Note that bytes are read in order from the buffer, so if at any
         point a null byte is encountered signifying the end of the buffer,
         no more bytes will be read past that point. */
      if (body[i] == INFO_COOKIE)
        {
          int j = i + 1;

          if (body[j] == INFO_FF)
            j++;
          if (body[j] == '\r')
            j++;

          if (body[j] == '\n')
            return i;
        }

      if (i == binding->end)
        break;
      i += dir;
    }

  return -1;
}

/* Return the length of the node separator characters that BODY is currently
   pointing at.  If it's not pointing at a node separator, return 0. */
int
skip_node_separator (char *body)
{
  register int i;

  i = 0;

  if (body[i] == INFO_FF)
    i++;

  if (body[i++] != INFO_COOKIE)
    return 0;

  if (body[i] == INFO_FF)
    i++;

  if (body[i] == '\r')
    i++;

  if (body[i++] != '\n')
    return 0;

  return i;
}

/* Return the absolute position of the beginning of a section in this file
   whose first line is LABEL, starting the search at binding->start. */
long
find_file_section (SEARCH_BINDING *binding, char *label)
{
  SEARCH_BINDING s;
  long position;
  int dir;

  s.buffer = binding->buffer;
  s.start = binding->start;
  s.end = binding->end;
  s.flags = S_FoldCase;
  dir = binding->start < binding->end ? 1 : -1;

  while ((position = find_node_separator (&s)) != -1 )
    {
      long offset = position;
      offset += skip_node_separator (s.buffer + offset);
      if (looking_at_line (label, s.buffer + offset))
        return position;

      if (dir > 0)
        {
          s.start = offset;
          if (s.start >= s.end)
            break;
        }
      else
        {
          s.start = position - 1;
          if (s.start <= s.end)
            break;
        }
    }
  return -1;
}

/* Return the absolute position of the node named NODENAME in BINDING.
   This is a brute force search, and we wish to avoid it when possible.
   This function is called when a tag (indirect or otherwise) doesn't
   really point to the right node.  It returns the absolute position of
   the separator preceding the node. */
long
find_node_in_binding (char *nodename, SEARCH_BINDING *binding)
{
  long position;
  int offset;
  SEARCH_BINDING s;

  s.buffer = binding->buffer;
  s.start = binding->start;
  s.end = binding->end;
  s.flags = 0;

  while ((position = find_node_separator (&s)) != -1)
    {
      char *nodename_start;
      char *read_nodename;
      int found;

      s.start = position;
      s.start += skip_node_separator (s.buffer + s.start);

      offset = string_in_line (INFO_NODE_LABEL, s.buffer + s.start);

      if (offset == -1)
        continue;

      s.start += offset;
      s.start += skip_whitespace (s.buffer + s.start); 
      nodename_start = s.buffer + s.start;
      read_quoted_string (nodename_start, "\n\r\t,", 0, &read_nodename);
      if (!read_nodename)
        return -1;

      found = !strcmp (read_nodename, nodename);
      free (read_nodename);

      if (found)
        return position;
    }
  return -1;
}
