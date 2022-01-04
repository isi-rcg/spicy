/* nodes.c -- how to get an Info file and node.
   $Id: nodes.c 7915 2017-07-09 18:51:00Z gavin $

   Copyright 1993, 1998, 1999, 2000, 2002, 2003, 2004, 2006, 2007,
   2008, 2009, 2011, 2012, 2013, 2014, 2015, 2016, 2017 Free Software 
   Foundation, Inc.

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

#include "nodes.h"
#include "search.h"
#include "filesys.h"
#include "info-utils.h"
#include "tag.h"
#include "man.h"
#include "variables.h"


/* Global variables.  */

/* When non-zero, this is a string describing the recent file error. */
char *info_recent_file_error = NULL;

/* The list of already loaded files. */
FILE_BUFFER **info_loaded_files = NULL;

/* Number of loaded files. */
size_t info_loaded_files_index = 0;

/* The number of slots currently allocated to LOADED_FILES. */
size_t info_loaded_files_slots = 0;

/* Functions for tag table creation and destruction. */

static void build_tag_table (FILE_BUFFER *file_buffer);
static void get_nodes_of_tags_table (FILE_BUFFER *file_buffer,
    SEARCH_BINDING *buffer_binding);
static void get_tags_of_indirect_tags_table (FILE_BUFFER *file_buffer,
    SEARCH_BINDING *indirect_binding, SEARCH_BINDING *tags_binding);
static void free_file_buffer_tags (FILE_BUFFER *file_buffer);
static void free_info_tag (TAG *tag);

/* Grovel FILE_BUFFER->contents finding tags and nodes, and filling in the
   various slots.  This can also be used to rebuild a tag or node table. */
void
build_tags_and_nodes (FILE_BUFFER *file_buffer)
{
  SEARCH_BINDING binding;
  long position;
  long tags_table_begin, tags_table_end;

  free_file_buffer_tags (file_buffer);
  file_buffer->flags &= ~N_HasTagsTable;

  /* See if there is a tags table in this info file. */
  binding.buffer = file_buffer->contents;
  binding.start = file_buffer->filesize;
  binding.end = binding.start - 1000;
  if (binding.end < 0)
    binding.end = 0;
  binding.flags = S_FoldCase;

  position = find_file_section (&binding, TAGS_TABLE_END_LABEL);
  if (position == -1)
    goto no_tags_table;

  /* If there is a tag table, find the start of it, and grovel over it
     extracting tag information. */

  /* Remember the end of the tags table. */
  if (position == 0)
    goto no_tags_table;
  else
    tags_table_end = position - 1;

  /* Locate the start of the tags table. */
  binding.start = tags_table_end;
  binding.end = 0;
  position = find_file_section (&binding, TAGS_TABLE_BEG_LABEL);
  if (position == -1)
    goto no_tags_table;

  /* The file contains a valid tags table.  Fill the FILE_BUFFER's
     tags member. */
  file_buffer->flags |= N_HasTagsTable;
  tags_table_begin = position;

  position += skip_node_separator (file_buffer->contents + position);
  position += strlen (TAGS_TABLE_BEG_LABEL);
  position += strspn (file_buffer->contents + position, "\r\n");
  if (!looking_at_line (TAGS_TABLE_IS_INDIRECT_LABEL,
                        file_buffer->contents + position))
    {
      /* If this isn't an indirect tags table, just remember the nodes
         described locally in this tags table. */
      binding.start = tags_table_begin;
      binding.end = tags_table_end;
      get_nodes_of_tags_table (file_buffer, &binding);
    }
  else
    {
      /* This is an indirect tags table.  Find the indirect table
         preceding the tags table. */
      SEARCH_BINDING indirect;

      indirect.start = tags_table_begin;
      indirect.end = 0;
      indirect.buffer = file_buffer->contents;
      indirect.flags = S_FoldCase;

      position = find_file_section (&indirect, INDIRECT_TABLE_LABEL);
      if (position == -1)
        /* This file is malformed.  Give up. */
        return;

      /* Skip "Indirect:" line. */
      position += skip_node_separator (file_buffer->contents + position);
      position += strlen (INDIRECT_TABLE_LABEL);
      position += strspn (file_buffer->contents + position, "\r\n");

      indirect.start = position;
      indirect.end = tags_table_begin;

      binding.start = tags_table_begin;
      binding.end = tags_table_end;
      get_tags_of_indirect_tags_table (file_buffer, &indirect, &binding);
    }
  return;

no_tags_table:
  /* This file doesn't have a tag table. */
  build_tag_table (file_buffer);
}

/* Set fields on new tag table entry. */
static void
init_file_buffer_tag (FILE_BUFFER *fb, TAG *entry)
{
  if (fb->flags & N_HasTagsTable)
    {
      entry->flags |= N_HasTagsTable;
      entry->filename = fb->fullpath;

      if (fb->flags & N_TagsIndirect)
        entry->flags |= N_TagsIndirect;
    }
}

/* Search through FILE_BUFFER->contents building an array of NODE *,
   one entry per each node present in the file.  Store the tags in
   FILE_BUFFER->tags, and the number of allocated slots in
   FILE_BUFFER->tags_slots. */
static void
build_tag_table (FILE_BUFFER *file_buffer)
{
  long nodestart;
  size_t tags_index = 0;
  SEARCH_BINDING binding;

  binding.buffer = file_buffer->contents;
  binding.start = 0;
  binding.end = file_buffer->filesize;
  binding.flags = S_FoldCase;

  while ((nodestart = find_node_separator (&binding)) != -1)
    {
      int start;
      char *nodeline;
      TAG *entry;
      int anchor = 0;

      /* Skip past the characters just found. */
      binding.start = nodestart;
      binding.start += skip_node_separator (binding.buffer + binding.start);

      /* Move to the start of the line defining the node. */
      nodeline = binding.buffer + binding.start;

      /* Find "Node:" */
      start = string_in_line (INFO_NODE_LABEL, nodeline);
      /* No Node:.  Maybe it's a Ref:.  */
      if (start == -1)
        {
          start = string_in_line (INFO_REF_LABEL, nodeline);
          if (start != -1)
            anchor = 1;
        }

      /* If not there, this is not the start of a node. */
      if (start == -1)
        continue;

      /* Find the start of the nodename. */
      start += skip_whitespace (nodeline + start);

      /* Record nodename and nodestart. */
      entry = info_create_tag ();
      read_quoted_string (nodeline + start, ",\n\r\t", 0, &entry->nodename);
      if (!entry->nodename || !*entry->nodename)
        {
          free (entry);
          continue;
        }
      entry->nodestart = nodestart;

      init_file_buffer_tag (file_buffer, entry);

      if (anchor)
        entry->cache.nodelen = 0;
      else
        /* Record that the length is unknown. */
        entry->cache.nodelen = -1;

      entry->filename = file_buffer->fullpath;

      /* Add this tag to the array of tag structures in this FILE_BUFFER. */
      add_pointer_to_array (entry, tags_index, file_buffer->tags,
                            file_buffer->tags_slots, 100);
    }
}

/* Build and save the array of nodes in FILE_BUFFER by searching through the
   contents of BUFFER_BINDING for a tags table, and groveling the contents. */
static void
get_nodes_of_tags_table (FILE_BUFFER *file_buffer,
    SEARCH_BINDING *buffer_binding)
{
  int name_offset;
  SEARCH_BINDING s;
  long position;
  size_t tags_index = 0;

  /* Copy buffer_binding */
  s = *buffer_binding;

  /* Find the start of the tags table. */
  position = buffer_binding->start;

  /* If none, we're all done. */
  if (position == -1)
    return;

  /* Move to one character before the start of the actual table. */
  s.start = position;
  s.start += skip_node_separator (s.buffer + s.start);
  s.start += strlen (TAGS_TABLE_BEG_LABEL);

  /* The tag table consists of lines containing node names and positions.
     Do each line until we find one that doesn't contain a node name. */
  while (search_forward ("\n", &s, &position) == search_success)
    {
      TAG *entry;
      char *nodedef;
      unsigned p;
      int anchor = 0;

      /* Prepare to skip this line. */
      s.start = position;
      s.start++;

      /* Skip past informative "(Indirect)" tags table line. */
      if (!tags_index && looking_at (TAGS_TABLE_IS_INDIRECT_LABEL, &s))
        continue;

      /* Find the label preceding the node name. */
      name_offset = string_in_line (INFO_NODE_LABEL, s.buffer + s.start);

      /* If no node label, maybe it's an anchor.  */
      if (name_offset == -1)
        {
          name_offset = string_in_line (INFO_REF_LABEL, s.buffer + s.start);
          if (name_offset != -1)
            anchor = 1;
        }

      /* If not there, not a defining line, so we must be out of the
         tags table.  */
      if (name_offset == -1)
        break;

      entry = info_create_tag ();

      init_file_buffer_tag (file_buffer, entry);

      /* Find the beginning of the node definition. */
      s.start += name_offset;
      nodedef = s.buffer + s.start;
      nodedef += skip_whitespace (nodedef);

      /* Move past the node's name in this tag to the TAGSEP character. */
      for (p = 0; nodedef[p] && nodedef[p] != INFO_TAGSEP; p++)
        ;
      if (nodedef[p] != INFO_TAGSEP)
        continue;

      entry->nodename = xmalloc (p + 1);
      strncpy (entry->nodename, nodedef, p);
      entry->nodename[p] = 0;
      p++;
      entry->nodestart = atol (nodedef + p);

      /* If a node, we don't know the length yet, but if it's an
         anchor, the length is 0. */
      entry->cache.nodelen = anchor ? 0 : -1;

      /* The filename of this node is currently known as the same as the
         name of this file. */
      entry->filename = file_buffer->fullpath;

      /* Add this node structure to the array of node structures in this
         FILE_BUFFER. */
      add_pointer_to_array (entry, tags_index, file_buffer->tags,
                            file_buffer->tags_slots, 100);
    }
}

/* Remember in FILE_BUFFER the nodenames, subfilenames, and offsets within the
   subfiles of every node which appears in the tags table at TAGS_BINDING.  The 
   indirect files list is at INDIRECT_BINDING. */
static void
get_tags_of_indirect_tags_table (FILE_BUFFER *file_buffer,
    SEARCH_BINDING *indirect_binding, SEARCH_BINDING *tags_binding)
{
  int i;

  /* A structure used only in `get_tags_of_indirect_tags_table' to hold onto
     an intermediate value. */
  typedef struct {
    char *filename;
    long first_byte;
  } SUBFILE;

  SUBFILE **subfiles = NULL;
  size_t subfiles_index = 0, subfiles_slots = 0;
  TAG *entry;

  /* Remember that tags table was indirect. */
  file_buffer->flags |= N_TagsIndirect;

  /* First get the list of tags from the tags table.  Then lookup the
     associated file in the indirect list for each tag, and update it. */
  get_nodes_of_tags_table (file_buffer, tags_binding);
  if (!file_buffer->tags)
    return;

  /* We have the list of tags in file_buffer->tags.  Get the list of
     subfiles from the indirect table. */
  {
    char *start, *end, *line;
    SUBFILE *subfile;

    start = indirect_binding->buffer + indirect_binding->start;
    end = indirect_binding->buffer + indirect_binding->end;
    line = start;

    while (line < end)
      {
        int colon;

        colon = string_in_line (":", line);

        if (colon == -1)
          break;

        subfile = xmalloc (sizeof (SUBFILE));
        subfile->filename = xmalloc (colon);
        strncpy (subfile->filename, line, colon - 1);
        subfile->filename[colon - 1] = 0;
        subfile->first_byte = (long) atol (line + colon);

        add_pointer_to_array (subfile, subfiles_index, subfiles, 
                              subfiles_slots, 10);

        while (*line++ != '\n');
      }
  }

  /* If we have successfully built the indirect files table, then
     merge the information in the two tables. */
  if (!subfiles)
    {
      free_file_buffer_tags (file_buffer);
      return;
    }
  else
    {
      int tags_index;
      long header_length;
      SEARCH_BINDING binding;

      /* Find the length of the header of the file containing the indirect
         tags table.  This header appears at the start of every file.  We
         want the absolute position of each node within each subfile, so
         we subtract the start of the containing subfile from the logical
         position of the node, and then add the length of the header in. */
      binding.buffer = file_buffer->contents;
      binding.start = 0;
      binding.end = file_buffer->filesize;
      binding.flags = S_FoldCase;

      header_length = find_node_separator (&binding);
      if (header_length == -1)
        header_length = 0;

      /* Build the file buffer's list of subfiles. */
      {
        char *containing_dir = xstrdup (file_buffer->fullpath);
        char *temp = filename_non_directory (containing_dir);
        int len_containing_dir;

        if (temp > containing_dir)
          {
            if (HAVE_DRIVE (file_buffer->fullpath) &&
                temp == containing_dir + 2)
              {
                /* Avoid converting "d:foo" into "d:/foo" below.  */
                *temp = '.';
                temp += 2;
              }
            temp[-1] = 0;
          }

        len_containing_dir = strlen (containing_dir);

        for (i = 0; subfiles[i]; i++);

        file_buffer->subfiles = xmalloc ((1 + i) * sizeof (char *));

        for (i = 0; subfiles[i]; i++)
          {
            char *fullpath;

            fullpath = xmalloc
              (2 + strlen (subfiles[i]->filename) + len_containing_dir);

            sprintf (fullpath, "%s/%s",
                     containing_dir, subfiles[i]->filename);

            file_buffer->subfiles[i] = fullpath;
          }
        file_buffer->subfiles[i] = NULL;
        free (containing_dir);
      }

      /* For each node in the file's tags table, remember the starting
         position. */
      for (tags_index = 0; (entry = file_buffer->tags[tags_index]);
           tags_index++)
        {
          for (i = 0;
               subfiles[i] && entry->nodestart >= subfiles[i]->first_byte;
               i++);

          /* If the Info file containing the indirect tags table is
             malformed, then give up. */
          if (!i)
            {
              /* The Info file containing the indirect tags table is
                 malformed.  Give up. */
              for (i = 0; subfiles[i]; i++)
                {
                  free (subfiles[i]->filename);
                  free (subfiles[i]);
                  free (file_buffer->subfiles[i]);
                }
              file_buffer->subfiles = NULL;
              free_file_buffer_tags (file_buffer);
              return;
            }

          /* SUBFILES[i] is the index of the first subfile whose logical
             first byte is greater than the logical offset of this node's
             starting position.  This means that the subfile directly
             preceding this one is the one containing the node. */

          entry->filename = file_buffer->subfiles[i - 1];
          entry->nodestart -= subfiles[i - 1]->first_byte;
          entry->nodestart += header_length;
        }
    }

  /* Free the structures assigned to SUBFILES.  Free the names as well
     as the structures themselves, then finally, the array. */
  for (i = 0; subfiles[i]; i++)
    {
      free (subfiles[i]->filename);
      free (subfiles[i]);
    }
  free (subfiles);
}

/* Free the tags (if any) associated with FILE_BUFFER. */
static void
free_file_buffer_tags (FILE_BUFFER *file_buffer)
{
  int i;

  if (file_buffer->tags)
    {
      TAG *tag;

      for (i = 0; (tag = file_buffer->tags[i]); i++)
        free_info_tag (tag);

      free (file_buffer->tags);
      file_buffer->tags = NULL;
      file_buffer->tags_slots = 0;
    }

  if (file_buffer->subfiles)
    {
      for (i = 0; file_buffer->subfiles[i]; i++)
        free (file_buffer->subfiles[i]);

      free (file_buffer->subfiles);
      file_buffer->subfiles = NULL;
    }
}

/* Free the data associated with TAG, as well as TAG itself. */
static void
free_info_tag (TAG *tag)
{
  free (tag->nodename);
  
  /* We don't free tag->filename, because that filename is part of the
     subfiles list for the containing FILE_BUFFER.  free_info_tags ()
     will free the subfiles when it is appropriate. */

  free (tag);
}

/* Functions for retrieving files. */

static FILE_BUFFER *info_load_file (char *fullpath, int get_tags);
static void get_file_character_encoding (FILE_BUFFER *fb);
static void forget_info_file (FILE_BUFFER *file_buffer);
static void info_reload_file_buffer_contents (FILE_BUFFER *fb);

/* Try to find a file in our list of already loaded files. */
FILE_BUFFER *
check_loaded_file (char *filename)
{
  int is_fullpath, i;
  FILE_BUFFER *file_buffer;
  
  /* If full path to the file has been given, we must find it exactly. */
  is_fullpath = IS_ABSOLUTE (filename)
                || filename[0] == '.' && IS_SLASH(filename[1]);

  if (info_loaded_files)
    {
      for (i = 0; (file_buffer = info_loaded_files[i]); i++)
        if (   (FILENAME_CMP (filename, file_buffer->fullpath) == 0)
            || (!is_fullpath
                 && (FILENAME_CMP (filename, file_buffer->filename) == 0)))
          {
            struct stat new_info, *old_info;

            old_info = &file_buffer->finfo;
            if (   stat (file_buffer->fullpath, &new_info) == -1
                || new_info.st_size != old_info->st_size
                || new_info.st_mtime != old_info->st_mtime)
              {
                /* The file has changed.  Forget that we ever had loaded it
                   in the first place. */
                forget_info_file (file_buffer);
                break;
              }

            /* The info file exists, and has not changed since the last
               time it was loaded.  If the caller requested a nodes list
               for this file, and there isn't one here, build the nodes
               for this file_buffer.  In any case, return the file_buffer
               object. */
            if (!file_buffer->contents)
              {
                /* The file's contents have been gc'ed.  Reload it.  */
                info_reload_file_buffer_contents (file_buffer);
                if (!file_buffer->contents)
                  return NULL;
              }

            if (!file_buffer->tags)
              build_tags_and_nodes (file_buffer);

            return file_buffer;
          }
    }
  return 0;
}

/* Locate the file named by FILENAME, and return the information structure
   describing this file.  The file may appear in our list of loaded files
   already, or it may not.  If it does not already appear, find the file,
   and add it to the list of loaded files.  If the file cannot be found,
   return a NULL FILE_BUFFER *. */
FILE_BUFFER *
info_find_file (char *filename)
{
  FILE_BUFFER *file_buffer;
  char *fullpath;
  int is_fullpath;
  
  file_buffer = check_loaded_file (filename);
  if (file_buffer)
    return file_buffer;

  /* The file wasn't loaded.  Try to load it now. */

  /* Get the full pathname of this file, as known by the info system.
     That is to say, search along INFOPATH and expand tildes, etc. */
  is_fullpath = IS_ABSOLUTE (filename)
                || filename[0] == '.' && IS_SLASH(filename[1]);
  if (!is_fullpath)
    fullpath = info_find_fullpath (filename, 0);
  else
    fullpath = xstrdup (filename);

  /* If the file wasn't found, give up, returning a NULL pointer. */
  if (!fullpath)
    return NULL;

  file_buffer = info_load_file (fullpath, 0);

  free (fullpath);
  return file_buffer;
}

/* Find a subfile of a split file.  This differs from info_load_file in
   that it does not fill in a tag table for the file. */
FILE_BUFFER *
info_find_subfile (char *fullpath)
{
  char *with_extension = 0;
  int i;
  FILE_BUFFER *file_buffer = 0;
  int fullpath_len = strlen (fullpath);

  /* First try to find the file in our list of already loaded files. */
  if (info_loaded_files)
    {
      for (i = 0; (file_buffer = info_loaded_files[i]); i++)
        /* Check if fullpath starts the name of the recorded file (extra
           extensions like ".info.gz" could be added.) */
        if (!strncmp (file_buffer->fullpath, fullpath, fullpath_len)
            && (file_buffer->fullpath[fullpath_len] == '\0'
                || file_buffer->fullpath[fullpath_len] == '.'))
          {
            struct stat new_info, *old_info;

            old_info = &file_buffer->finfo;
            if (   stat (file_buffer->fullpath, &new_info) == -1
                || new_info.st_size != old_info->st_size
                || new_info.st_mtime != old_info->st_mtime)
              {
                /* The file has changed.  Forget that we ever had loaded it
                   in the first place. */
                forget_info_file (file_buffer);
                break;
              }
            return file_buffer;
          }
    }

  /* The file wasn't loaded.  Try to load it now. */
  with_extension = info_find_fullpath (fullpath, 0);
  if (with_extension)
    {
      file_buffer = info_load_file (with_extension, 1);
      free (with_extension);
    }
  return file_buffer;
}

/* Load the file with path FULLPATH, and return the information structure
   describing this file, even if the file was already loaded.  IS_SUBFILE
   says whether this file is the subfile of a split file.  If it is, mark
   the FILE_BUFFER object as such and do not build a list of nodes for
   this file. */
static FILE_BUFFER *
info_load_file (char *fullpath, int is_subfile)
{
  char *contents;
  size_t filesize;
  struct stat finfo;
  int compressed;
  FILE_BUFFER *file_buffer = NULL;

  contents = filesys_read_info_file (fullpath, &filesize, &finfo, &compressed);

  if (!contents)
    return NULL;

  /* The file was found, and can be read.  Allocate FILE_BUFFER and fill
     in the various members. */
  file_buffer = make_file_buffer ();
  file_buffer->fullpath = xstrdup (fullpath);
  file_buffer->filename = filename_non_directory (file_buffer->fullpath);
  file_buffer->filename = xstrdup (file_buffer->filename);
  /* Strip off a file extension, so we can find it again in info_find_file. */
  {
    char *p = strchr (file_buffer->filename, '.');
    if (p)
      *p = '\0';
  }
  file_buffer->finfo = finfo;
  file_buffer->filesize = filesize;
  file_buffer->contents = contents;
  if (compressed)
    file_buffer->flags |= N_IsCompressed;
  
  /* Find encoding of file, if set */
  get_file_character_encoding (file_buffer);

  if (!is_subfile)
    {
      build_tags_and_nodes (file_buffer);
      if (!file_buffer->tags)
        {
          free (file_buffer->fullpath);
          free (file_buffer->filename);
          free (file_buffer);
          return 0;
        }
    }
  else
    file_buffer->flags |= N_Subfile;

  /* If the file was loaded, remember the name under which it was found. */
  if (file_buffer)
    add_pointer_to_array (file_buffer, info_loaded_files_index,
                          info_loaded_files, info_loaded_files_slots, 10);

  return file_buffer;
}

/* Look for local variables section in FB and set encoding */
static void
get_file_character_encoding (FILE_BUFFER *fb)
{
  SEARCH_BINDING binding;
  long position;

  long int enc_start, enc_len;
  char *enc_string;

  /* See if there is a local variables section in this info file. */
  binding.buffer = fb->contents;
  binding.start = fb->filesize;
  binding.end = binding.start - 1000;
  if (binding.end < 0)
    binding.end = 0;
  binding.flags = S_FoldCase;

  /* Null means the encoding is unknown. */
  fb->encoding = 0;

  if (search_backward (LOCAL_VARIABLES_LABEL, &binding, &position)
      != search_success)
    return;

  binding.start = position;
  binding.end = fb->filesize;

  if (search_forward (CHARACTER_ENCODING_LABEL, &binding, &enc_start)
      != search_success)
    return;

  enc_start += strlen(CHARACTER_ENCODING_LABEL); /* Skip to after "coding:" */
  enc_start += skip_whitespace(fb->contents + enc_start);

  enc_len = strcspn (fb->contents + enc_start, "\r\n");

  enc_string = xmalloc (enc_len + 1);
  strncpy (enc_string, fb->contents + enc_start, enc_len);
  enc_string[enc_len] = '\0';

  fb->encoding = enc_string;
}

/* Create a new, empty file buffer. */
FILE_BUFFER *
make_file_buffer (void)
{
  FILE_BUFFER *file_buffer = xmalloc (sizeof (FILE_BUFFER));

  file_buffer->filename = file_buffer->fullpath = NULL;
  file_buffer->contents = NULL;
  file_buffer->tags = NULL;
  file_buffer->subfiles = NULL;
  file_buffer->tags_slots = 0;
  file_buffer->flags = 0;
  file_buffer->encoding = 0;

  return file_buffer;
}

/* Prevent this file buffer being used again. */
static void
forget_info_file (FILE_BUFFER *file_buffer)
{
  file_buffer->flags |= N_Gone;
  file_buffer->filename[0] = '\0';
  file_buffer->fullpath = "";
  memset (&file_buffer->finfo, 0, sizeof (struct stat));
}

/* Load the contents of FILE_BUFFER->contents.  This function is called
   when a file buffer was loaded, and then in order to conserve memory, the
   file buffer's contents were freed and the pointer was zero'ed.  Note that
   the file was already loaded at least once successfully, so the tags and/or
   nodes members are still correctly filled. */
static void
info_reload_file_buffer_contents (FILE_BUFFER *fb)
{
  int is_compressed;

  fb->flags &= ~N_IsCompressed;

  /* Let the filesystem do all the work for us. */
  fb->contents =
    filesys_read_info_file (fb->fullpath, &(fb->filesize), &(fb->finfo),
                            &is_compressed);
  if (is_compressed)
    fb->flags |= N_IsCompressed;
}


/* Functions for node creation and retrieval. */

static long get_node_length (SEARCH_BINDING *binding);
static void node_set_body_start (NODE *node);
static int adjust_nodestart (FILE_BUFFER *file_buffer, TAG *tag);

/* Return a pointer to a newly allocated TAG structure, with
   fields filled in. */
TAG *
info_create_tag (void)
{
  TAG *t = xmalloc (sizeof (TAG));

  memset (t, 0, sizeof (TAG));
  t->filename = 0;
  t->nodename = 0;
  t->nodestart = -1;
  t->nodestart_adjusted = -1;
  t->cache.nodelen = -1;

  return t;
}
/* Return a pointer to a newly allocated NODE structure, with
   fields filled in. */
NODE *
info_create_node (void)
{
  NODE *n = xmalloc (sizeof (NODE));

  n->fullpath = 0;
  n->subfile = 0;
  n->nodename = 0;
  n->contents = 0;
  n->nodelen = -1;
  n->display_pos = 0;
  n->body_start = 0;
  n->flags = 0;
  n->references = 0;
  n->up = 0;
  n->prev = 0;
  n->next = 0;

  return n;
}

/* Return the length of the node which starts at BINDING. */
static long
get_node_length (SEARCH_BINDING *binding)
{
  int i;
  char *body;

  /* [A node] ends with either a ^_, a ^L, or end of file.  */
  for (i = binding->start, body = binding->buffer; i < binding->end; i++)
    {
      if (body[i] == INFO_FF || body[i] == INFO_COOKIE)
        break;
    }
  return i - binding->start;
}

#define FOLLOW_REMAIN 0
#define FOLLOW_PATH 1

int follow_strategy;

/* Return a pointer to a NODE structure for the Info node (FILENAME)NODENAME,
   using DEFAULTS for defaults.  If DEFAULTS is null, the defaults are:
   - If FILENAME is NULL, `dir' is used.
   - If NODENAME is NULL, `Top' is used.
   
   If the node cannot be found, return NULL. */
NODE *
info_get_node_with_defaults (char *filename_in, char *nodename_in,
                NODE *defaults)
{
  NODE *node = 0;
  FILE_BUFFER *file_buffer = NULL;
  char *filename = 0, *nodename = 0;

  info_recent_file_error = NULL;

  filename = filename_in;
  if (filename_in)
    {
      filename = xstrdup (filename_in);
      if (follow_strategy == FOLLOW_REMAIN
          && defaults && defaults->fullpath
          && filename_in)
        {
          /* Find the directory in the filename for defaults, and look in
             that directory first. */
          char *file_in_same_dir;
          char saved_char, *p;

          p = defaults->fullpath + strlen (defaults->fullpath);
          while (p > defaults->fullpath && !IS_SLASH (*p))
            p--;

          if (p > defaults->fullpath)
            {
              saved_char = *p;
              *p = 0;

              file_in_same_dir = info_add_extension (defaults->fullpath,
                                                     filename, 0);
              if (file_in_same_dir)
                file_buffer = info_find_file (file_in_same_dir);
              free (file_in_same_dir);
              *p = saved_char;
            }
        }
    }
  else
    {
      if (defaults)
        filename = xstrdup (defaults->fullpath);
      else
        filename = xstrdup ("dir");
    }

  if (nodename_in && *nodename_in)
    nodename = xstrdup (nodename_in);
  else
    /* If NODENAME is not specified, it defaults to "Top". */
    nodename = xstrdup ("Top");

  /* If the file to be looked up is "dir", build the contents from all of
     the "dir"s and "localdir"s found in INFOPATH. */
  if (is_dir_name (filename))
    {
      node = get_dir_node ();
      goto cleanup_and_exit;
    }

  if (mbscasecmp (filename, MANPAGE_FILE_BUFFER_NAME) == 0)
    {
      node = get_manpage_node (nodename);
      goto cleanup_and_exit;
    }

  if (!file_buffer)
    file_buffer = info_find_file (filename);

  if (file_buffer)
    {
      /* Look for the node.  */
      node = info_get_node_of_file_buffer (file_buffer, nodename);
    }

  /* If the node not found was "Top", try again with different case. */
  if (!node && (nodename && mbscasecmp (nodename, "Top") == 0))
    {
      node = info_get_node_of_file_buffer (file_buffer, "Top");
      if (!node)
        node = info_get_node_of_file_buffer (file_buffer, "top");
      if (!node)
        node = info_get_node_of_file_buffer (file_buffer, "TOP");
    }

cleanup_and_exit:
  free (filename); free (nodename);
  return node;
}

/* Return NODE specified with FILENAME_IN and NODENAME_IN.  Return value
   should be freed by caller, but none of its fields should be. */
NODE *
info_get_node (char *filename_in, char *nodename_in)
{
  return info_get_node_with_defaults (filename_in, nodename_in, 0);
}

static void
node_set_body_start (NODE *node)
{
  int n = skip_node_separator (node->contents);
  node->body_start = strcspn(node->contents + n, "\n");
  node->body_start += n;
}

/* Return a pointer to a NODE structure for the Info node NODENAME in
   FILE_BUFFER.  NODENAME can be passed as NULL, in which case the
   nodename of "Top" is used.  If the node cannot be found, return a
   NULL pointer.  Return value should be freed by caller, but none of its
   fields should be. */
NODE *
info_get_node_of_file_buffer (FILE_BUFFER *file_buffer, char *nodename)
{
  NODE *node = NULL;

  /* If we are unable to find the file, we have to give up.  There isn't
     anything else we can do. */
  if (!file_buffer)
    return NULL;

  /* If the file buffer was gc'ed, reload the contents now. */
  if (!file_buffer->contents)
    info_reload_file_buffer_contents (file_buffer);

  /* If the name of the node that we wish to find is exactly "*", then the
     node body is the contents of the entire file.  Create and return such
     a node. */
  if (strcmp (nodename, "*") == 0)
    {
      node = info_create_node ();
      node->fullpath = file_buffer->fullpath;
      node->nodename = xstrdup ("*");
      node->contents = file_buffer->contents;
      node->nodelen = file_buffer->filesize;
      node->body_start = 0;
    }
  /* Search the tags table for an entry which matches the node that we want. */
  else
    {
      TAG *tag;
      int i;

      /* If no tags at all (possibly a misformatted info file), quit.  */
      if (!file_buffer->tags)
        return NULL;

      for (i = 0; (tag = file_buffer->tags[i]); i++)
        if (strcmp (nodename, tag->nodename) == 0)
          {
            node = info_node_of_tag (file_buffer, &file_buffer->tags[i]);
            break;
          }
    }

  /* Return the results of our node search. */
  return node;
}


/* Magic number that RMS used to decide how much a tags table pointer could
   be off by.  I feel that it should be much smaller, like 4.  */
#define DEFAULT_INFO_FUDGE 1000

/* Find the actual starting memory location of NODE.  Because of the
   way that tags are implemented, the physical nodestart may
   not actually be where the tag says it is.  If that is the case,
   set N_UpdateTags in NODE->flags.  If the node is found, return non-zero.
   Set NODE->nodestart_adjusted directly on the separator that precedes this 
   node.  If the node could not be found, return 0. */
static int
adjust_nodestart (FILE_BUFFER *fb, TAG *node)
{
  long position = -1;
  SEARCH_BINDING s;

  /* Try the optimal case first.  Who knows?  This file may actually be
     formatted (mostly) correctly. */
  s.buffer = fb->contents;
  s.start = node->nodestart;
  s.end = s.start + 1;

  /* Check that the given nodestart is in fact inside the file buffer. */
  if (s.start >= 0 && s.start < fb->filesize)
    {
      /* Check for node separator at node->nodestart
         introducting this node. */
      position = find_node_in_binding (node->nodename, &s);
    }

  if (position == -1)
    {
      if (strict_node_location_p)
        return 0;

      /* Oh well, I guess we have to try to find it in a larger area. */

      s.start -= DEFAULT_INFO_FUDGE;
      s.end += DEFAULT_INFO_FUDGE;

      if (s.start < 0)
        s.start = 0;
      else if (s.start > fb->filesize)
        s.start = fb->filesize;
      if (s.end > fb->filesize)
        s.end = fb->filesize;

      position = find_node_in_binding (node->nodename, &s);

      /* If the node still couldn't be found, we lose big. */
      if (position == -1)
        return 0;

      /* Set the flag in NODE->flags to say that the the tags table could
         need updating (if we used a tag to get here, that is). */
      if (node->flags & N_HasTagsTable)
        node->flags |= N_UpdateTags;
    }

  node->nodestart_adjusted = s.buffer + position - fb->contents;
  return 1;
}

/* Look in the contents of *FB_PTR for a node referred to with TAG.  Set
   the location if found in TAG->nodestart_adjusted.

   PARENT->tags contains the tags table for the whole file.  If file is
   non-split, PARENT should be the same as FB. */
static int
find_node_from_tag (FILE_BUFFER *parent, FILE_BUFFER *fb, TAG *tag)
{
  int success;

  if (tag->nodestart_adjusted != -1)
    success = 1;
  else
    success = adjust_nodestart (fb, tag);

  if (success)
    return success;
  return 0;
}

/* Calculate the length of the node. */
static void
set_tag_nodelen (FILE_BUFFER *subfile, TAG *tag)
{
  SEARCH_BINDING node_body;

  node_body.buffer = subfile->contents;
  node_body.start = tag->nodestart_adjusted;
  node_body.end = subfile->filesize;
  node_body.flags = 0;
  node_body.start += skip_node_separator (node_body.buffer + node_body.start);
  tag->cache.nodelen = get_node_length (&node_body);
}

/* Return the node described by *TAG_PTR, retrieving contents from subfile
   if the file is split.  Return 0 on failure.  If FAST, don't process the
   node to find cross-references, a menu, or perform character encoding
   conversion. */
static NODE *
info_node_of_tag_ext (FILE_BUFFER *fb, TAG **tag_ptr, int fast)
{
  TAG *tag = *tag_ptr;
  NODE *node;
  int is_anchor;
  TAG *anchor_tag;
  int node_pos, anchor_pos;

  FILE_BUFFER *parent; /* File containing tag table. */
  FILE_BUFFER *subfile; /* File containing node. */
 
  if (!FILENAME_CMP (fb->fullpath, tag->filename))
    parent = subfile = fb;
  else
    {
      /* This is a split file. */
      parent = fb;
      subfile = info_find_subfile (tag->filename);
    }

  if (!subfile)
    return NULL;

  if (!subfile->contents)
    {
      info_reload_file_buffer_contents (subfile);
      if (!subfile->contents)
        return NULL;
    }

  /* If we were able to find this file and load it, then return
     the node within it. */
  if (!(tag->nodestart >= 0 && tag->nodestart < subfile->filesize))
    return NULL;

  node = 0;

  is_anchor = tag->cache.nodelen == 0;
 
  if (is_anchor)
    {
      anchor_pos = tag_ptr - fb->tags;

      /* Look backwards in the tag table for the node preceding
         the anchor (we're assuming the tags are given in order),
         skipping over any preceding anchors.  */
      for (node_pos = anchor_pos - 1;
           node_pos >= 0 && fb->tags[node_pos]->cache.nodelen == 0;
           node_pos--)
        ;

      /* An info file with an anchor before any nodes is pathological, but
         it's possible, so don't crash.  */
      if (node_pos < 0)
        return NULL;

      anchor_tag = tag;
      tag = fb->tags[node_pos];
      tag_ptr = &fb->tags[node_pos];
    }

  /* We haven't checked the entry pointer yet.  Look for the node
     around about it and adjust it if necessary. */
  if (tag->cache.nodelen == -1)
    {
      if (!find_node_from_tag (parent, subfile, tag))
        return NULL; /* Node not found. */

      set_tag_nodelen (subfile, tag);
    }

  node = xmalloc (sizeof (NODE));
  memset (node, 0, sizeof (NODE));
  if (tag->cache.references)
    {
      /* Initialize the node from the cache. */
      *node = tag->cache;
      if (!node->contents)
        {
          node->contents = subfile->contents + tag->nodestart_adjusted;
          node->contents += skip_node_separator (node->contents);
        }
    }
  else
    {
      /* Data for node has not been generated yet. */
      node->contents = subfile->contents + tag->nodestart_adjusted;
      node->contents += skip_node_separator (node->contents);
      node->nodelen = tag->cache.nodelen;
      node->nodename = tag->nodename;
      node->flags = tag->flags;

      node->fullpath = parent->fullpath;
      if (parent != subfile)
        node->subfile = tag->filename;

      if (fast)
        node->flags |= N_Simple;
      else
        {
          /* Read locations of references in node and similar.  Strip Info file
             syntax from node if preprocess_nodes=On.  Adjust the offsets of
             anchors that occur within the node. */
          scan_node_contents (node, parent, tag_ptr);

          if (!preprocess_nodes_p)
            node_set_body_start (node);
          tag->cache = *node;
          if (!(node->flags & N_WasRewritten))
            tag->cache.contents = 0; /* Pointer into file buffer
                                        is not saved.  */
        }
    }

  /* We can't set this when tag table is built, because
     if file is split, we don't know which of the sub-files
     are compressed. */
  if (subfile->flags & N_IsCompressed)
    node->flags |= N_IsCompressed;

  if (is_anchor)
    {
      /* Start displaying the node at the anchor position.  */

      node->display_pos = anchor_tag->nodestart_adjusted
        - (tag->nodestart_adjusted
           + skip_node_separator (subfile->contents
                                  + tag->nodestart_adjusted));

      /* Otherwise an anchor at the end of a node ends up displaying at
         the end of the last line of the node (way over on the right of
         the screen), which looks wrong.  */
      if (node->display_pos >= (unsigned long) node->nodelen)
        node->display_pos = node->nodelen - 1;
      else if (node->display_pos < 0)
        node->display_pos = 0; /* Shouldn't happen. */
    }

  return node;
}

NODE *
info_node_of_tag (FILE_BUFFER *fb, TAG **tag_ptr)
{
  return info_node_of_tag_ext (fb, tag_ptr, 0);
}

NODE *
info_node_of_tag_fast (FILE_BUFFER *fb, TAG **tag_ptr)
{
  return info_node_of_tag_ext (fb, tag_ptr, 1);
}
