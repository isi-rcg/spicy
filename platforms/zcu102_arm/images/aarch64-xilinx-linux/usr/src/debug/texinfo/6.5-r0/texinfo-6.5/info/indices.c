/* indices.c -- deal with an Info file index.
   $Id: indices.c 7782 2017-05-14 11:39:19Z gavin $

   Copyright 1993, 1997, 1998, 1999, 2002, 2003, 2004, 2007, 2008, 2011,
   2013, 2014, 2015, 2016, 2017 Free Software Foundation, Inc.

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
#include "info-utils.h"
#include "session.h"
#include "echo-area.h"
#include "indices.h"
#include "variables.h"

/* User-visible variable controls the output of info-index-next. */
int show_index_match = 1;

/* The combined indices of the last file processed by
   info_indices_of_file_buffer. */
static REFERENCE **index_index = NULL;

/* The offset of the most recently selected index element. */
static int index_offset = 0;

/* Whether we are doing initial index search. */
static int index_initial = 0;

/* Whether we are doing partial index search */
static int index_partial = 0;

/* Variable which holds the last string searched for. */
static char *index_search = NULL;

/* A couple of "globals" describing where the initial index was found. */
static char *initial_index_filename = NULL;
static char *initial_index_nodename = NULL;

/* A structure associating index names with index offset ranges. */
typedef struct {
  char *name;                   /* The nodename of this index. */
  int first;                    /* The index in our list of the first entry. */
  int last;                     /* The index in our list of the last entry. */
} INDEX_NAME_ASSOC;

/* An array associating index nodenames with index offset ranges.  Used
   for reporting to the user which index node an index entry was found
   in. */
static INDEX_NAME_ASSOC **index_nodenames = NULL;
static size_t index_nodenames_index = 0;
static size_t index_nodenames_slots = 0;

/* Add the name of NODE, and the range of the associated index elements
   (passed in ARRAY) to index_nodenames.  ARRAY must have at least one
   element. */
static void
add_index_to_index_nodenames (REFERENCE **array, NODE *node)
{
  register int i, last;
  INDEX_NAME_ASSOC *assoc;

  for (last = 0; array[last + 1]; last++);
  assoc = xmalloc (sizeof (INDEX_NAME_ASSOC));
  assoc->name = xstrdup (node->nodename);

  if (!index_nodenames_index)
    {
      assoc->first = 0;
      assoc->last = last;
    }
  else
    {
      for (i = 0; index_nodenames[i + 1]; i++);
      assoc->first = 1 + index_nodenames[i]->last;
      assoc->last = assoc->first + last;
    }
  add_pointer_to_array (assoc, index_nodenames_index, index_nodenames, 
                        index_nodenames_slots, 10);
}

/* Find and concatenate the indices of FILE_BUFFER, saving the result in 
   INDEX_INDEX.  The indices are defined as the first node in the file 
   containing the word "Index" and any immediately following nodes whose names 
   also contain "Index".  All such indices are concatenated and the result 
   returned. */
static void
info_indices_of_file_buffer (FILE_BUFFER *file_buffer)
{
  register int i;
  REFERENCE **result = NULL;

  /* No file buffer, no indices. */
  if (!file_buffer)
    {
      free (index_index);
      index_index = 0;
      return;
    }

  /* If the file is the same as the one that we last built an
     index for, don't do anything. */
  if (initial_index_filename
      && FILENAME_CMP (initial_index_filename, file_buffer->filename) == 0)
    {
      return;
    }

  /* Display a message because finding the index entries might take a while. */
  if (info_windows_initialized_p)
    window_message_in_echo_area (_("Finding index entries..."));

  /* Reset globals describing where the index was found. */
  free (initial_index_filename);
  free (initial_index_nodename);
  initial_index_filename = NULL;
  initial_index_nodename = NULL;

  if (index_nodenames)
    {
      for (i = 0; index_nodenames[i]; i++)
        {
          free (index_nodenames[i]->name);
          free (index_nodenames[i]);
        }

      index_nodenames_index = 0;
      index_nodenames[0] = NULL;
    }

  /* Grovel the names of the nodes found in this file. */
  if (file_buffer->tags)
    {
      TAG *tag;

      for (i = 0; (tag = file_buffer->tags[i]); i++)
        {
          if (strcasestr (tag->nodename, "Index")
              && tag->cache.nodelen != 0) /* Not an anchor. */
            {
              NODE *node;
              REFERENCE **menu;

              node = info_node_of_tag (file_buffer, &file_buffer->tags[i]);

              if (!node)
                continue;

              if (!initial_index_filename)
                {
                  /* Remember the filename and nodename of this index. */
                  initial_index_filename = xstrdup (file_buffer->filename);
                  initial_index_nodename = xstrdup (tag->nodename);
                }

              menu = node->references;

              /* If we have a non-empty menu, add this index's nodename
                 and range to our list of index_nodenames. */
              if (menu && menu[0])
                {
                  add_index_to_index_nodenames (menu, node);

                  /* Concatenate the references found so far. */
                  {
                  REFERENCE **old_result = result;
                  result = info_concatenate_references (result, menu);
                  free (old_result);
                  }
                }
              free_history_node (node);
            }
        }
    }

  /* If there is a result, clean it up so that every entry has a filename. */
  for (i = 0; result && result[i]; i++)
    if (!result[i]->filename)
      result[i]->filename = xstrdup (file_buffer->filename);

  free (index_index);
  index_index = result;

  if (info_windows_initialized_p)
    window_clear_echo_area ();
}

void info_next_index_match (WINDOW *window, int count);

DECLARE_INFO_COMMAND (info_index_search,
   _("Look up a string in the index for this file"))
{
  FILE_BUFFER *fb;
  char *line;
  int old_offset;

  fb = file_buffer_of_window (window);
  if (fb)
    info_indices_of_file_buffer (fb); /* Sets index_index. */

  if (!fb || !index_index)
    {
      info_error (_("No indices found"));
      return;
    }

  line = info_read_maybe_completing (_("Index entry: "), index_index);

  /* User aborted? */
  if (!line)
    {
      info_abort_key (window, 1);
      return;
    }

  /* Empty line means move to the Index node. */
  if (!*line)
    {
      free (line);

      if (initial_index_filename && initial_index_nodename)
        {
          NODE *node;

          node = info_get_node (initial_index_filename,
                                initial_index_nodename);
          info_set_node_of_window (window, node);
        }
      return;
    }

  /* Start the search either at the first or last index entry. */
  if (count < 0)
    {
      register int i;
      for (i = 0; index_index[i]; i++);
      index_offset = i;
    }
  else
    {
      index_offset = -1;
      index_initial = 0;
      index_partial = 0;
    }
  
  old_offset = index_offset;

  /* The "last" string searched for is this one. */
  free (index_search);
  index_search = line;

  info_next_index_match (window, count);

  /* If the search failed, return the index offset to where it belongs. */
  if (index_offset == old_offset)
    index_offset = 0;
}

/* Return true if ENT->label matches "S( <[0-9]+>)?", where S stands
   for the first LEN characters from STR. */
static int
index_entry_matches (REFERENCE *ent, const char *str, size_t len)
{
  char *p;
  
  if (strncmp (ent->label, str, len))
    return 0;
  p = ent->label + len;
  if (!*p)
    return 1;
  if (p[0] == ' ' && p[1] == '<')
    {
      for (p += 2; *p; p++)
	{
	  if (p[0] == '>' && p[1] == 0)
	    return 1;
	  else if (!isdigit (*p))
	    return 0;
	}
    }
  return 0;
}

/* Search for the next occurence of STRING in FB's indices starting at OFFSET 
   in direction DIR.
   
   Try to get an exact match, If no match found, progress onto looking for 
   initial matches, then non-initial substrings, updating the values of 
   INDEX_INITIAL and INDEX_PARTIAL.

   If a match is found, return a pointer to the matching index entry, and
   set *FOUND_OFFSET to its offset in INDEX_INDEX.  Otherwise, return null.
   If we found a partial match, set *MATCH_OFFSET to the end of the match 
   within the index entry text, else to 0.  */
REFERENCE *
next_index_match (FILE_BUFFER *fb, char *string, int offset, int dir,
                  int *found_offset, int *match_offset)
{
  int i;
  int partial_match;
  size_t search_len;
  REFERENCE *result;

  partial_match = 0;
  search_len = strlen (string);

  info_indices_of_file_buffer (fb); /* Sets index_index. */
  if (!index_index)
    {
      info_error (_("No indices found."));
      return 0;
    }

  if (index_search != string)
    {
      free (index_search); index_search = string;
    }

  if (!index_initial && !index_partial)
    {
      /* First try to find an exact match. */
      for (i = offset + dir; i > -1 && index_index[i]; i += dir)
        if (index_entry_matches (index_index[i], string, search_len))
          {
            *match_offset = 0;
            break;
          }

      if (i < 0 || !index_index[i])
	{
          offset = 0;
          index_initial = 1;
	}
    }

  if (index_initial)
    {
      for (i = offset + dir; i > -1 && index_index[i]; i += dir)
        if (!index_entry_matches (index_index[i], string, search_len)
            && !strncmp (index_index[i]->label, string, search_len))
          {
            *match_offset = search_len;
            break;
          }

      if (i < 0 || !index_index[i])
	{
          offset = 0;
          index_initial = 0;
          index_partial = 1;
	}
    }

  if (index_partial)
    {
      /* Look for substrings, excluding case-matching inital matches. */
      for (i = offset + dir; i > -1 && index_index[i]; i += dir)
        {
          if (strncmp (index_index[i]->label, string, search_len) != 0)
            {
              partial_match = string_in_line (string, index_index[i]->label);
              if (partial_match != -1)
                {
                  *match_offset = partial_match;
                  break;
                }
            }
        }
      if (partial_match <= 0)
        index_partial = 0;
    }

  if (i < 0 || !index_index[i])
    result = 0;
  else
    {
      index_offset = i;
      result = index_index[i];
    }

  *found_offset = i;
  return result;
}

/* Display a message saying where the index match was found. */
void
report_index_match (int i, int match_offset)
{
  register int j;
  const char *name = "CAN'T SEE THIS";
  char *match;

  for (j = 0; index_nodenames[j]; j++)
    {
      if ((i >= index_nodenames[j]->first) &&
          (i <= index_nodenames[j]->last))
        {
          name = index_nodenames[j]->name;
          break;
        }
    }

  /* If we had a partial match, indicate to the user which part of the
     string matched. */
  match = xstrdup (index_index[i]->label);

  if (match_offset > 0 && show_index_match)
    {
      int k, ls, start, upper;

      ls = strlen (index_search);
      start = match_offset - ls;
      upper = isupper (match[start]) ? 1 : 0;

      for (k = 0; k < ls; k++)
        if (upper)
          match[k + start] = tolower (match[k + start]);
        else
          match[k + start] = toupper (match[k + start]);
    }

    {
      char *format;

      format = replace_in_documentation
        (_("Found '%s' in %s. ('\\[next-index-match]' tries to find next.)"),
         0);

      window_message_in_echo_area (format, match, (char *) name);
    }

  free (match);
}

DECLARE_INFO_COMMAND (info_next_index_match,
 _("Go to the next matching index item from the last '\\[index-search]' command"))
{
  int i;
  int match_offset;
  int dir;
  REFERENCE *result;
  
  /* If there is no previous search string, the user hasn't built an index
     yet. */
  if (!index_search)
    {
      info_error (_("No previous index search string"));
      return;
    }

  /* The direction of this search is controlled by the value of the
     numeric argument. */
  if (count < 0)
    dir = -1;
  else
    dir = 1;

  result = next_index_match (file_buffer_of_window (window), index_search, 
                             index_offset, dir, &i, &match_offset);

  /* If that failed, print an error. */
  if (!result)
    {
      info_error (index_offset > 0 ?
                  _("No more index entries containing '%s'") :
                  _("No index entries containing '%s'"),
                  index_search);
      index_offset = 0;
      return;
    }

  /* Report to the user on what we have found. */
  report_index_match (i, match_offset);

  info_select_reference (window, result);
}

/* Look for the best match of STRING in the indices of FB.  If SLOPPY, allow 
   case-insensitive initial substrings to match.  Return null if no match is 
   found.  Return value should not be freed or modified.  This differs from the 
   behaviour of next_index_match in that only _initial_ substrings are 
   considered. */
REFERENCE *
look_in_indices (FILE_BUFFER *fb, char *string, int sloppy)
{
  REFERENCE **index_ptr;
  REFERENCE *nearest = 0;

  /* Remember the search string so we can use it as the default for 
     'virtual-index' or 'next-index-match'. */
  free (index_search);
  index_search = xstrdup (string);

  info_indices_of_file_buffer (fb); /* Sets index_index. */
  if (!index_index)
    return 0;

  for (index_ptr = index_index; *index_ptr; index_ptr++)
    {
      if (!strcmp (string, (*index_ptr)->label))
        {
          nearest = *index_ptr;
          break;
        }
      /* Case-insensitive initial substring. */
      if (sloppy && !nearest && !mbsncasecmp (string, (*index_ptr)->label,
                                    mbslen (string)))
        {
          nearest = *index_ptr;
        }
    }
  return nearest;
}

/* **************************************************************** */
/*                                                                  */
/*                 Info APROPOS: Search every known index.          */
/*                                                                  */
/* **************************************************************** */

/* For every menu item in DIR, search the indices of that file for
   SEARCH_STRING. */
REFERENCE **
apropos_in_all_indices (char *search_string, int inform)
{
  size_t i, dir_index;
  REFERENCE **all_indices = NULL;
  REFERENCE **dir_menu = NULL;
  NODE *dir_node;

  dir_node = get_dir_node ();

  /* It should be safe to assume that dir nodes do not contain any
     cross-references, i.e., its references list only contains
     menu items. */
  if (dir_node)
    dir_menu = dir_node->references;

  if (!dir_menu)
    {
      free (dir_node);
      return NULL;
    }

  /* For every menu item in DIR, get the associated file buffer and
     read the indices of that file buffer.  Gather all of the indices into
     one large one. */
  for (dir_index = 0; dir_menu[dir_index]; dir_index++)
    {
      REFERENCE **this_index, *this_item;
      FILE_BUFFER *this_fb, *loaded_file = 0;

      this_item = dir_menu[dir_index];
      if (!this_item->filename)
        continue;

      /* If we already scanned this file, don't do that again.
         In addition to being faster, this also avoids having
         multiple identical entries in the *Apropos* menu.  */
      for (i = 0; i < dir_index; i++)
        if (dir_menu[i]->filename
            && FILENAME_CMP (this_item->filename, dir_menu[i]->filename) == 0)
          break;
      if (i < dir_index)
        continue;

      this_fb = check_loaded_file (this_item->filename);

      if (!this_fb)
        this_fb = loaded_file = info_find_file (this_item->filename);

      if (!this_fb)
        continue; /* Couldn't load file. */

      if (this_fb && inform)
        message_in_echo_area (_("Scanning indices of '%s'..."), this_item->filename);

      info_indices_of_file_buffer (this_fb);
      this_index = index_index;

      if (this_fb && inform)
        unmessage_in_echo_area ();

      if (this_index)
        {
          /* Remember the filename which contains this set of references. */
          for (i = 0; this_index && this_index[i]; i++)
            if (!this_index[i]->filename)
              this_index[i]->filename = xstrdup (this_fb->filename);

          /* Concatenate with the other indices.  */
          {
          REFERENCE **old_indices = all_indices;
          all_indices = info_concatenate_references (all_indices, this_index);
          free (old_indices);
          }
        }

      /* Try to avoid running out of memory by not loading all of the
         Info files on the system into memory.  This is risky because we
         may have a pointer into the file buffer, so only free the contents
         if we have just loaded the file. */
      if (loaded_file)
        {
          free (loaded_file->contents);
          loaded_file->contents = NULL;
        }
    }

  /* Build a list of the references which contain SEARCH_STRING. */
  if (all_indices)
    {
      REFERENCE *entry, **apropos_list = NULL;
      size_t apropos_list_index = 0;
      size_t apropos_list_slots = 0;

      for (i = 0; (entry = all_indices[i]); i++)
        {
          if (string_in_line (search_string, entry->label) != -1)
            {
              add_pointer_to_array (entry, apropos_list_index, apropos_list, 
                                    apropos_list_slots, 100);
            }
        }

      free (all_indices);
      all_indices = apropos_list;
    }
  free (dir_node);
  return all_indices;
}

static char *apropos_list_nodename = "*Apropos*";

DECLARE_INFO_COMMAND (info_index_apropos,
   _("Grovel all known info file's indices for a string and build a menu"))
{
  char *line, *prompt;
  REFERENCE **apropos_list;
  NODE *apropos_node;
  struct text_buffer message;

  if (index_search)
    asprintf (&prompt, "%s [%s]: ", _("Index apropos"), index_search);
  else
    asprintf (&prompt, "%s: ", _("Index apropos"));
  line = info_read_in_echo_area (prompt);
  free (prompt);

  window = active_window;

  /* User aborted? */
  if (!line)
    {
      info_abort_key (window, 1);
      return;
    }

  /* User typed something? */
  if (*line)
    {
      free (index_search);
      index_search = line;
    }
  else
    free (line); /* Try to use the last search string. */

  if (index_search && *index_search)
    {
      apropos_list = apropos_in_all_indices (index_search, 1);

      if (!apropos_list)
        { 
          info_error (_(APROPOS_NONE), index_search);
          return;
        }
      else
        {
          /* Create the node.  FIXME: Labels and node names taken from the
             indices of Info files may be in a different character encoding to 
             the one currently being used.
             This problem is reduced by makeinfo not putting quotation marks 
             from @samp, etc., into node names and index entries. */
          register int i;

          text_buffer_init (&message);
          text_buffer_add_char (&message, '\n');
          text_buffer_printf (&message, _("Index entries containing "
                              "'%s':\n"), index_search);
          text_buffer_printf (&message, "\n* Menu:");
          text_buffer_add_string (&message, "\0\b[index\0\b]", 11);
          text_buffer_add_char (&message, '\n');

          for (i = 0; apropos_list[i]; i++)
            {
              int line_start = text_buffer_off (&message);
              char *filename;

              /* Remove file extension. */
              filename = program_name_from_file_name
                (apropos_list[i]->filename);

              /* The label might be identical to that of another index
                 entry in another Info file.  Therefore, we make the file
                 name part of the menu entry, to make them all distinct.  */
              text_buffer_printf (&message, "* %s [%s]: ",
                      apropos_list[i]->label, filename);

              while (text_buffer_off (&message) - line_start < 40)
                text_buffer_add_char (&message, ' ');
              text_buffer_printf (&message, "(%s)%s.",
                                  filename, apropos_list[i]->nodename);
              text_buffer_printf (&message, " (line %ld)\n",
                                  apropos_list[i]->line_number);
              free (filename);
            }
        }

      apropos_node = text_buffer_to_node (&message);
      {
        char *old_contents = apropos_node->contents;
        scan_node_contents (apropos_node, 0, 0);
        if (old_contents != apropos_node->contents)
          free (old_contents);
      }

      name_internal_node (apropos_node, xstrdup (apropos_list_nodename));

      /* Find/Create a window to contain this node. */
      {
        WINDOW *new;
        NODE *node;

        /* If a window is visible and showing an apropos list already,
           re-use it. */
        for (new = windows; new; new = new->next)
          {
            node = new->node;

            if (internal_info_node_p (node) &&
                (strcmp (node->nodename, apropos_list_nodename) == 0))
              break;
          }

        /* If we couldn't find an existing window, try to use the next window
           in the chain. */
        if (!new && window->next)
          new = window->next;

        /* If we still don't have a window, make a new one to contain
           the list. */
        if (!new)
          new = window_make_window ();

        /* If we couldn't make a new window, use this one. */
        if (!new)
          new = window;

        /* Lines do not wrap in this window. */
        new->flags |= W_NoWrap;

        info_set_node_of_window (new, apropos_node);
        active_window = new;
      }
      free (apropos_list);
    }
}

#define NODECOL 41
#define LINECOL 62

static void
format_reference (REFERENCE *ref, const char *filename, struct text_buffer *buf)
{
  size_t n;
  
  n = text_buffer_printf (buf, "* %s: ", ref->label);
  if (n < NODECOL)
    n += text_buffer_fill (buf, ' ', NODECOL - n);
  
  if (ref->filename && strcmp (ref->filename, filename))
    n += text_buffer_printf (buf, "(%s)", ref->filename);
  n += text_buffer_printf (buf, "%s. ", ref->nodename);

  if (n < LINECOL)
    n += text_buffer_fill (buf, ' ', LINECOL - n);
  else
    {
      text_buffer_add_char (buf, '\n');
      text_buffer_fill (buf, ' ', LINECOL);
    }
  
  text_buffer_printf (buf, "(line %4d)\n", ref->line_number);
}

NODE *
create_virtual_index (FILE_BUFFER *file_buffer, char *index_search)
{
  struct text_buffer text;
  int i;
  size_t cnt;
  NODE *node;

  text_buffer_init (&text);
  text_buffer_printf (&text,
                      "File: %s,  Node: Index for '%s'\n\n",
                      file_buffer->filename, index_search);
  text_buffer_printf (&text, _("Virtual Index\n"
                               "*************\n\n"
                               "Index entries that match '%s':\n"),
                      index_search);
  text_buffer_add_string (&text, "\0\b[index\0\b]", 11);
  text_buffer_printf (&text, "\n* Menu:\n\n");

  cnt = 0;

  index_offset = 0;
  index_initial = 0;
  index_partial = 0;
  while (1)
    {
      REFERENCE *result;
      int match_offset;

      result = next_index_match (file_buffer, index_search, index_offset, 1,
                                 &i, &match_offset);
      if (!result)
        break;
      format_reference (index_index[i],
                        file_buffer->filename, &text);
      cnt++;
    }
  text_buffer_add_char (&text, '\0');

  if (cnt == 0)
    {
      text_buffer_free (&text);
      return 0;
    }

  node = info_create_node ();
  asprintf (&node->nodename, "Index for '%s'", index_search);
  node->fullpath = file_buffer->filename;
  node->contents = text_buffer_base (&text);
  node->nodelen = text_buffer_off (&text) - 1;
  node->body_start = strcspn (node->contents, "\n");
  node->flags |= N_IsInternal | N_WasRewritten;

  scan_node_contents (node, 0, 0);

  return node;
}

DECLARE_INFO_COMMAND (info_virtual_index,
   _("List all matches of a string in the index"))
{
  char *prompt, *line;
  FILE_BUFFER *fb;
  NODE *node;
  
  fb = file_buffer_of_window (window);

  if (!initial_index_filename ||
      !fb ||
      (FILENAME_CMP (initial_index_filename, fb->filename) != 0))
    {
      window_message_in_echo_area (_("Finding index entries..."));
      info_indices_of_file_buffer (fb);
    }

  if (!index_index)
    {
      info_error (_("No indices found."));
      return;
    }
    
  /* Default to last search if there is one. */
  if (index_search)
    asprintf (&prompt, "%s [%s]: ", _("Index topic"), index_search);
  else
    asprintf (&prompt, "%s: ", _("Index topic"));
  line = info_read_maybe_completing (prompt, index_index);
  free (prompt);

  /* User aborted? */
  if (!line)
    {
      info_abort_key (window, 1);
      return;
    }

  if (*line)
    {
      free (index_search);
      index_search = line;
    }
  else if (!index_search)
    {
      free (line);
      return; /* No previous search string, and no string given. */
    }
  
  node = create_virtual_index (fb, index_search);
  if (!node)
    {
      info_error (_("No index entries containing '%s'."), index_search);
      return;
    }
  info_set_node_of_window (window, node);
}
