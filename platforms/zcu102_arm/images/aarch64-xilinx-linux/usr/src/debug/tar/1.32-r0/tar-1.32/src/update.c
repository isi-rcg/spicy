/* Update a tar archive.

   Copyright 1988-2019 Free Software Foundation, Inc.

   This file is part of GNU tar.

   GNU tar is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   GNU tar is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* Implement the 'r', 'u' and 'A' options for tar.  'A' means that the
   file names are tar files, and they should simply be appended to the end
   of the archive.  No attempt is made to record the reads from the args; if
   they're on raw tape or something like that, it'll probably lose...  */

#include <system.h>
#include <quotearg.h>
#include "common.h"

/* FIXME: This module should not directly handle the following variable,
   instead, this should be done in buffer.c only.  */
extern union block *current_block;

/* We've hit the end of the old stuff, and its time to start writing new
   stuff to the tape.  This involves seeking back one record and
   re-writing the current record (which has been changed).
   FIXME: Either eliminate it or move it to common.h.
*/
bool time_to_start_writing;

/* Pointer to where we started to write in the first record we write out.
   This is used if we can't backspace the output and have to null out the
   first part of the record.  */
char *output_start;

/* Catenate file FILE_NAME to the archive without creating a header for it.
   It had better be a tar file or the archive is screwed.  */
static void
append_file (char *file_name)
{
  int handle = openat (chdir_fd, file_name, O_RDONLY | O_BINARY);
  struct stat stat_data;

  if (handle < 0)
    {
      open_error (file_name);
      return;
    }

  if (fstat (handle, &stat_data) != 0)
    stat_error (file_name);
  else
    {
      off_t bytes_left = stat_data.st_size;

      while (bytes_left > 0)
	{
	  union block *start = find_next_block ();
	  size_t buffer_size = available_space_after (start);
	  size_t status;
	  char buf[UINTMAX_STRSIZE_BOUND];

	  if (bytes_left < buffer_size)
	    {
	      buffer_size = bytes_left;
	      status = buffer_size % BLOCKSIZE;
	      if (status)
		memset (start->buffer + bytes_left, 0, BLOCKSIZE - status);
	    }

	  status = safe_read (handle, start->buffer, buffer_size);
	  if (status == SAFE_READ_ERROR)
	    read_fatal_details (file_name, stat_data.st_size - bytes_left,
				buffer_size);
	  if (status == 0)
	    FATAL_ERROR ((0, 0,
			  ngettext ("%s: File shrank by %s byte",
				    "%s: File shrank by %s bytes",
				    bytes_left),
			  quotearg_colon (file_name),
			  STRINGIFY_BIGINT (bytes_left, buf)));

	  bytes_left -= status;

	  set_next_block_after (start + (status - 1) / BLOCKSIZE);
	}
    }

  if (close (handle) != 0)
    close_error (file_name);
}

/* Implement the 'r' (add files to end of archive), and 'u' (add files
   to end of archive if they aren't there, or are more up to date than
   the version in the archive) commands.  */
void
update_archive (void)
{
  enum read_header previous_status = HEADER_STILL_UNREAD;
  bool found_end = false;

  name_gather ();
  open_archive (ACCESS_UPDATE);
  xheader_forbid_global ();

  while (!found_end)
    {
      enum read_header status = read_header (&current_header,
                                             &current_stat_info,
                                             read_header_auto);

      switch (status)
	{
	case HEADER_STILL_UNREAD:
	case HEADER_SUCCESS_EXTENDED:
	  abort ();

	case HEADER_SUCCESS:
	  {
	    struct name *name;

	    decode_header (current_header, &current_stat_info,
			   &current_format, 0);
	    transform_stat_info (current_header->header.typeflag,
				 &current_stat_info);
	    archive_format = current_format;

	    if (subcommand_option == UPDATE_SUBCOMMAND
		&& (name = name_scan (current_stat_info.file_name)) != NULL)
	      {
		struct stat s;

		chdir_do (name->change_dir);
		if (deref_stat (current_stat_info.file_name, &s) == 0)
		  {
		    if (S_ISDIR (s.st_mode))
		      {
			char *p, *dirp = tar_savedir (name->name, 1);
			if (dirp)
			  {
			    namebuf_t nbuf = namebuf_create (name->name);

			    for (p = dirp; *p; p += strlen (p) + 1)
			      addname (namebuf_name (nbuf, p),
				       name->change_dir, false, NULL);

			    namebuf_free (nbuf);
			    free (dirp);

			    remname (name);
			  }
		      }
		    else if (tar_timespec_cmp (get_stat_mtime (&s),
					       current_stat_info.mtime)
			     <= 0)
		      remname (name);
		  }
	      }

	    skip_member ();
	    break;
	  }

	case HEADER_ZERO_BLOCK:
	  current_block = current_header;
	  found_end = true;
	  break;

	case HEADER_END_OF_FILE:
	  found_end = true;
	  break;

	case HEADER_FAILURE:
	  set_next_block_after (current_header);
	  switch (previous_status)
	    {
	    case HEADER_STILL_UNREAD:
	      WARN ((0, 0, _("This does not look like a tar archive")));
	      FALLTHROUGH;
	    case HEADER_SUCCESS:
	    case HEADER_ZERO_BLOCK:
	      ERROR ((0, 0, _("Skipping to next header")));
	      FALLTHROUGH;
	    case HEADER_FAILURE:
	      break;

	    case HEADER_END_OF_FILE:
	    case HEADER_SUCCESS_EXTENDED:
	      abort ();
	    }
	  break;
	}

      tar_stat_destroy (&current_stat_info);
      previous_status = status;
    }

  reset_eof ();
  time_to_start_writing = true;
  output_start = current_block->buffer;

  {
    struct name const *p;
    while ((p = name_from_list ()) != NULL)
      {
	char *file_name = p->name;
	if (excluded_name (file_name, NULL))
	  continue;
	if (interactive_option && !confirm ("add", file_name))
	  continue;
	if (subcommand_option == CAT_SUBCOMMAND)
	  append_file (file_name);
	else
	  dump_file (0, file_name, file_name);
      }
  }

  write_eot ();
  close_archive ();
  finish_deferred_unlinks ();
  names_notfound ();
}
