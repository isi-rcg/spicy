/* tag.c -- Functions to handle Info tags (that is, the special
   construct for images, not the "tag table" of starting position.)
   $Id: tag.c 7656 2017-01-30 19:26:05Z gavin $

   Copyright 2012, 2013, 2014 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "info.h"
#include "tag.h" 
#include "info-utils.h"

struct tag_handler
{
  const char *name;
  size_t len;
  int (*handler) (char *, struct text_buffer *);
};

struct info_tag
{
  struct info_tag *next;
  char *kw;
  char *val;
};

static void
info_tag_free (struct info_tag *tag)
{
  while (tag)
    {
      struct info_tag *next = tag->next;
      free (tag->kw);
      free (tag->val);
      free (tag);
      tag = next;
    }
}


/* See if KW is one of the tags in the list starting at TAG.  */

static struct info_tag *
info_tag_find (struct info_tag *tag, const char *kw)
{
  for (; tag; tag = tag->next)
    if (strcmp (tag->kw, kw) == 0)
      return tag;
  return NULL;
}


/* Found a keyword when parsing the full tag string: alt, text, etc.
   Return the new tag, update *TMPBUF_PTR and set *KW.  */

static struct info_tag *
tag_found_keyword (struct text_buffer *tmpbuf_ptr, char **kw)
{
  struct info_tag *tag = xmalloc (sizeof (*tag));
  tag->next = NULL;  /* have to update in caller */

  text_buffer_add_char (tmpbuf_ptr, 0);
  if (*kw != tmpbuf_ptr->base) { /* in case tmpbuf got realloc-ed */
    *kw = tmpbuf_ptr->base;      /* ick */
  }
  tag->kw = xstrdup (*kw);
  tag->val = xstrdup (*kw + strlen(*kw) + 1);
  text_buffer_reset (tmpbuf_ptr);

  return tag;
}

/* Handle the image tag.  */

static int
tag_image (char *text, struct text_buffer *outbuf)
{
  mbi_iterator_t iter;
  enum { state_kw, state_val, state_qstr, state_delim } state = state_kw;
  struct text_buffer tmpbuf;
  char *kw;
  struct info_tag *tag_head = NULL, *tag;
  int escaped = 0;
  
  text_buffer_init (&tmpbuf);
  for (mbi_init (iter, text, strlen (text)); mbi_avail (iter);
       mbi_advance (iter))
    {
      const char *cur_ptr;
      size_t cur_len;
      
      if (mb_isspace (mbi_cur (iter)))
	{
	  if (state == state_val)
	    {
              struct info_tag *new_kw = tag_found_keyword (&tmpbuf, &kw);
              new_kw->next = tag_head;
              tag_head = new_kw;
              state = state_delim;
              continue;
	    }
	  if (state == state_delim)
	    continue;
	}
      else if (state == state_delim)
	state = state_kw;
      cur_len = mb_len (mbi_cur (iter));
      cur_ptr = mbi_cur_ptr (iter);
      
      if (state == state_qstr && escaped)
	{
	  escaped = 0;
	}
      else if (cur_len == 1)
	{
	  switch (*cur_ptr)
	    {
	    case '=':
	      if (state != state_kw)
		break;
	      text_buffer_add_char (&tmpbuf, 0);
	      kw = tmpbuf.base;
	      if (!mbi_avail (iter))
		break;
	      mbi_advance (iter);
	      state = state_val;
	      cur_len = mb_len (mbi_cur (iter));
	      cur_ptr = mbi_cur_ptr (iter);
	      if (!(cur_len == 1 && *cur_ptr == '"'))
		break;
	      /* fall through */

	    case '"':
	      if (state == state_val)
		{
		  state = state_qstr;
		  continue;
		}
	      if (state == state_qstr)
		{
		  struct info_tag *new_kw = tag_found_keyword (&tmpbuf, &kw);
		  new_kw->next = tag_head;
		  tag_head = new_kw;
		  state = state_delim;
		  continue;
		}
	      break;

	    case '\\':
	      if (state == state_qstr)
		{
		  escaped = 1;
		  continue;
		}
	    }
	}
      text_buffer_add_string (&tmpbuf, cur_ptr, cur_len);
    }

  tag = info_tag_find (tag_head, "text");
  if (!tag)
    tag = info_tag_find (tag_head, "alt");

  if (tag)
    {
      text_buffer_add_string (outbuf, tag->val, strlen (tag->val));
    }
  
  text_buffer_free (&tmpbuf);
  info_tag_free (tag_head);
  return 0;
}


/* We don't do anything with the index tag; it'll just be ignored.  */

static struct tag_handler tagtab[] = {
  { "image", 5, tag_image },
  { "index", 5, NULL },
  { NULL }
};

static struct tag_handler *
find_tag_handler (char *tag, size_t taglen)
{
  struct tag_handler *tp;

  for (tp = tagtab; tp->name; tp++)
    if (taglen >= tp->len && strncmp (tp->name, tag, tp->len) == 0)
      return tp;
  return NULL;
}

/* Expand \b[...\b] construct at *INPUT.  If encountered, append the
   expanded text to OUTBUF, advance *INPUT past the tag, and return 1.
   Otherwise, return 0.  If it is an index tag, set IS_INDEX to 1.
   *INPUT points into a null-terminated area which may however contain other 
   null characters.  INPUT_END points to the end of this area. */
int
tag_expand (char **input, char *input_end,
            struct text_buffer *outbuf, int *is_index)
{
  char *p = *input;
  char *q;
  size_t len;
  struct tag_handler *tp;

  if (p >= input_end - 3
    || memcmp(p, "\0\b[", 3) != 0)       /* opening magic? */
    return 0;

  p += 3;
  q = p + strlen (p);
  if (q >= input_end - 3
      || memcmp (q + 1, "\b]", 2)) /* closing magic? */
    return 0; /* Not a proper tag. */

  /* Output is different for index nodes */
  if (!strncmp ("index", p, strlen ("index")))
    *is_index = 1;

  len = strcspn (p, " \t");       /* tag name */
  tp = find_tag_handler (p, len);
  if (tp && tp->handler)
    {
      while (p[len] == ' ' || p[len] == '\t')
        ++len;                      /* move past whitespace */
  
      tp->handler (p + len, outbuf);
    }
  *input = q + 3;
  return 1;
}
