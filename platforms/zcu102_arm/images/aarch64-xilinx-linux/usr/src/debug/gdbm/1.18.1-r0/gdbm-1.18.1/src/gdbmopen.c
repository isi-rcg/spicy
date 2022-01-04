/* gdbmopen.c - Open the dbm file and initialize data structures for use. */

/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 1990-1991, 1993, 2007, 2011, 2013, 2016-2018 Free
   Software Foundation, Inc.

   GDBM is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3, or (at your option)
   any later version.

   GDBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GDBM. If not, see <http://www.gnu.org/licenses/>.   */

/* Include system configuration before all else. */
#include "autoconf.h"

#include "gdbmdefs.h"
#include <stddef.h>

/* Determine our native magic number and bail if we can't. */
#if SIZEOF_OFF_T == 4
# define GDBM_MAGIC	GDBM_MAGIC32
#elif SIZEOF_OFF_T == 8
# define GDBM_MAGIC	GDBM_MAGIC64
#else
# error "Unsupported off_t size, contact GDBM maintainer.  What crazy system is this?!?"
#endif

static void
compute_directory_size (blksize_t block_size,
			int *ret_dir_size, int *ret_dir_bits)
{
  /* Create the initial hash table directory.  */
  int dir_size = 8 * sizeof (off_t);
  int dir_bits = 3;

  while (dir_size < block_size && dir_bits < GDBM_HASH_BITS - 3)
    {
      dir_size <<= 1;
      dir_bits++;
    }

  *ret_dir_size = dir_size;
  *ret_dir_bits = dir_bits;
}

static inline int
bucket_element_count (size_t bucket_size)
{
  return (bucket_size - sizeof (hash_bucket)) / sizeof (bucket_element) + 1;
}

static int
avail_comp (void const *a, void const *b)
{
  avail_elem const *ava = a;
  avail_elem const *avb = b;
  return ava->av_size - avb->av_size;
}

/* Returns true if the avail array AV[0]@COUNT is valid.

   As a side effect, ensures the array is sorted by element size
   in increasing order and restores the ordering if necessary.

   The proper ordering could have been clobbered in versions of GDBM<=1.15,
   by a call to _gdbm_put_av_elem with the can_merge parameter set to
   TRUE. This happened in two cases: either because the GDBM_COALESCEBLKS
   was set, and (quite unfortunately) when _gdbm_put_av_elem was called
   from pop_avail_block in falloc.c. The latter case is quite common,
   which means that there can be lots of existing databases with broken
   ordering of avail arrays. Thus, restoring of the proper ordering
   is essential for people to be able to use their existing databases.
*/
int
gdbm_avail_table_valid_p (GDBM_FILE dbf, avail_elem *av, int count)
{
  off_t prev = 0;
  int i;
  int needs_sorting = 0;
  avail_elem *p = av;
  
  prev = 0;
  for (i = 0; i < count; i++, p++)
    {
      if (!(p->av_adr >= dbf->header->bucket_size
	    && p->av_adr + p->av_size <= dbf->header->next_block))
	return 0;
      if (p->av_size < prev)
	needs_sorting = 1;
      prev = p->av_size;
    }

  if (needs_sorting && dbf->read_write)
    {
      GDBM_DEBUG (GDBM_DEBUG_ERR, "%s", "restoring sort order");
      qsort (av, count, sizeof av[0], avail_comp);
    }
  
  return 1;
}

int
gdbm_avail_block_validate (GDBM_FILE dbf, avail_block *avblk)
{
  if (!(gdbm_avail_block_valid_p (avblk)
	&& gdbm_avail_table_valid_p (dbf, avblk->av_table, avblk->count)))
    {
      GDBM_SET_ERRNO (dbf, GDBM_BAD_AVAIL, TRUE);
      return -1;
    }
  return 0;
}

int
gdbm_bucket_avail_table_validate (GDBM_FILE dbf, hash_bucket *bucket)
{
  if (!(bucket->av_count >= 0
	&& bucket->av_count <= BUCKET_AVAIL
	&& gdbm_avail_table_valid_p (dbf, bucket->bucket_avail,
				     bucket->av_count)))
    {
      GDBM_SET_ERRNO (dbf, GDBM_BAD_AVAIL, TRUE);
      return -1;
    }
  return 0;
}

static int
validate_header (gdbm_file_header const *hdr, struct stat const *st)
{
  int dir_size, dir_bits;
  int result = GDBM_NO_ERROR;
  
  /* Is the magic number good? */
  if (hdr->header_magic != GDBM_MAGIC)
    {
      switch (hdr->header_magic)
	{
	case GDBM_OMAGIC:
	  /* OK */
	  break;

	case GDBM_OMAGIC_SWAP:
	case GDBM_MAGIC32_SWAP:
	case GDBM_MAGIC64_SWAP:
	  return GDBM_BYTE_SWAPPED;

	case GDBM_MAGIC32:
	case GDBM_MAGIC64:
	  return GDBM_BAD_FILE_OFFSET;

	default:
	  return GDBM_BAD_MAGIC_NUMBER;
	}
    }
  
  if (!(hdr->block_size > 0
	&& hdr->block_size > sizeof (gdbm_file_header)
	&& hdr->block_size - sizeof (gdbm_file_header) >=
	sizeof(hdr->avail.av_table[0])))
    {
      return GDBM_BLOCK_SIZE_ERROR;
    }

  /* Technically speaking, the condition below should read
         hdr->next_block != st->st_size
     However, gdbm versions prior to commit 4e819c98 could leave
     hdr->next_block pointing beyond current end of file. To ensure
     backward compatibility with these versions, the condition has been
     slackened to this: */
  if (hdr->next_block < st->st_size)
    result = GDBM_NEED_RECOVERY;

  /* Make sure dir and dir + dir_size fall within the file boundary */
  if (!(hdr->dir > 0
	&& hdr->dir < st->st_size
	&& hdr->dir_size > 0
	&& hdr->dir + hdr->dir_size < st->st_size))
    return GDBM_BAD_HEADER;

  compute_directory_size (hdr->block_size, &dir_size, &dir_bits);
  if (!(hdr->dir_size >= dir_size))
    return GDBM_BAD_HEADER;
  compute_directory_size (hdr->dir_size, &dir_size, &dir_bits);
  if (hdr->dir_bits != dir_bits)
    return GDBM_BAD_HEADER;
  
  if (!(hdr->bucket_size > sizeof(hash_bucket)))
    return GDBM_BAD_HEADER;

  if (hdr->bucket_elems != bucket_element_count (hdr->bucket_size))
    return GDBM_BAD_HEADER;

  if (((hdr->block_size - sizeof (gdbm_file_header)) / sizeof(avail_elem) + 1)
      != hdr->avail.size)
    return GDBM_BAD_HEADER;    
  
  return result;
}

int
_gdbm_validate_header (GDBM_FILE dbf)
{
  struct stat file_stat;
    
  if (fstat (dbf->desc, &file_stat))
    return GDBM_FILE_STAT_ERROR;

  return validate_header (dbf->header, &file_stat);
}

/* Do we have ftruncate? */
static inline int
_gdbm_ftruncate (GDBM_FILE dbf)
{
#if HAVE_FTRUNCATE
  return ftruncate (dbf->desc, 0);
#else
  int fd;
  fd = open (dbf->name, O_RDWR|O_TRUNC, mode);
  if (fd == -1)
    return -1;
  return close (fd);
#endif
}

GDBM_FILE 
gdbm_fd_open (int fd, const char *file_name, int block_size,
	      int flags, void (*fatal_func) (const char *))
{
  GDBM_FILE dbf;		/* The record to return. */
  struct stat file_stat;	/* Space for the stat information. */
  off_t       file_pos;		/* Used with seeks. */
  int 	      index;		/* Used as a loop index. */
  
  /* Initialize the gdbm_errno variable. */
  gdbm_set_errno (NULL, GDBM_NO_ERROR, FALSE);

  /* Get the status of the file. */
  if (fstat (fd, &file_stat))
    {
      if (flags & GDBM_CLOERROR)
	SAVE_ERRNO (close (fd));
      GDBM_SET_ERRNO2 (NULL, GDBM_FILE_STAT_ERROR, FALSE, GDBM_DEBUG_OPEN);
      return NULL;
    }
  
  /* Allocate new info structure. */
  dbf = calloc (1, sizeof (*dbf));
  if (dbf == NULL)
    {
      if (flags & GDBM_CLOERROR)
	SAVE_ERRNO (close (fd));
      GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
      return NULL;
    }

  dbf->desc = fd;
  
  /* Initialize some fields for known values.  This is done so gdbm_close
     will work if called before allocating some structures. */
  dbf->dir  = NULL;
  dbf->bucket = NULL;
  dbf->header = NULL;
  dbf->bucket_cache = NULL;
  dbf->cache_size = 0;

  dbf->memory_mapping = FALSE;
  dbf->mapped_size_max = SIZE_T_MAX;
  dbf->mapped_region = NULL;
  dbf->mapped_size = 0;
  dbf->mapped_pos = 0;
  dbf->mapped_off = 0;

  /* Save name of file. */
  dbf->name = strdup (file_name);
  if (dbf->name == NULL)
    {
      if (flags & GDBM_CLOERROR)
	close (fd);
      free (dbf);
      GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
      return NULL;
    }

  /* Initialize the fatal error routine. */
  dbf->fatal_err = fatal_func;

  dbf->fast_write = TRUE;	/* Default to setting fast_write. */
  dbf->file_locking = TRUE;	/* Default to doing file locking. */
  dbf->central_free = FALSE;	/* Default to not using central_free. */
  dbf->coalesce_blocks = FALSE; /* Default to not coalesce blocks. */

  dbf->need_recovery = FALSE;
  dbf->last_error = GDBM_NO_ERROR;
  dbf->last_syserror = 0;
  dbf->last_errstr = NULL;
  
  /* GDBM_FAST used to determine whether or not we set fast_write. */
  if (flags & GDBM_SYNC)
    {
      /* If GDBM_SYNC has been requested, don't do fast_write. */
      dbf->fast_write = FALSE;
    }
  if (flags & GDBM_NOLOCK)
    {
      dbf->file_locking = FALSE;
    }

  dbf->cloexec = !!(flags & GDBM_CLOEXEC);
  
  /* Zero-length file can't be a reader... */
  if (((flags & GDBM_OPENMASK) == GDBM_READER) && (file_stat.st_size == 0))
    {
      if (flags & GDBM_CLOERROR)
	close (dbf->desc);
      free (dbf->name);
      free (dbf);
      GDBM_SET_ERRNO2 (NULL, GDBM_EMPTY_DATABASE, FALSE, GDBM_DEBUG_OPEN);
      return NULL;
    }

  /* Record the kind of user. */
  dbf->read_write = (flags & GDBM_OPENMASK);

  /* Lock the file in the appropriate way. */
  if (dbf->file_locking)
    {
      if (_gdbm_lock_file (dbf) == -1)
	{
	  if (flags & GDBM_CLOERROR)
	    close (dbf->desc);
	  free (dbf->name);
	  free (dbf);
          GDBM_SET_ERRNO2 (NULL,
			   (flags & GDBM_OPENMASK) == GDBM_READER
			     ? GDBM_CANT_BE_READER : GDBM_CANT_BE_WRITER,
			   FALSE,
			   GDBM_DEBUG_OPEN);
	  return NULL;
	}
    }

  /* If we do have a write lock and it was a GDBM_NEWDB, it is 
     now time to truncate the file. */
  if ((flags & GDBM_OPENMASK) == GDBM_NEWDB && file_stat.st_size != 0)
    {
      if (_gdbm_ftruncate (dbf))
	{
	  GDBM_SET_ERRNO2 (dbf, GDBM_FILE_TRUNCATE_ERROR, FALSE,
			   GDBM_DEBUG_OPEN);
	}
      else if (fstat (dbf->desc, &file_stat))
	{
	  GDBM_SET_ERRNO2 (dbf, GDBM_FILE_STAT_ERROR, FALSE, GDBM_DEBUG_OPEN);
	}

      if (gdbm_last_errno (dbf))
	{
	  if (flags & GDBM_CLOERROR)
	    close (dbf->desc);
	  free (dbf->name);
	  free (dbf);
	  return NULL;
	}
    }

  /* Decide if this is a new file or an old file. */
  if (file_stat.st_size == 0)
    {
      /* This is a new file.  Create an empty database.  */
      int dir_size, dir_bits;
      
      /* Start with the blocksize. */
      if (block_size < GDBM_MIN_BLOCK_SIZE)
	{
	  block_size = STATBLKSIZE (file_stat);
	  flags &= ~GDBM_BSEXACT;
	}
      compute_directory_size (block_size, &dir_size, &dir_bits);
      GDBM_DEBUG (GDBM_DEBUG_OPEN, "%s: computed dir_size=%d, dir_bits=%d",
		  dbf->name, dir_size, dir_bits);
      /* Check for correct block_size. */
      if (dir_size != block_size)
	{
	  if (flags & GDBM_BSEXACT)
	    {
	      if (!(flags & GDBM_CLOERROR))
		dbf->desc = -1;
	      gdbm_close (dbf);
	      GDBM_SET_ERRNO2 (NULL, GDBM_BLOCK_SIZE_ERROR, FALSE,
			       GDBM_DEBUG_OPEN);
	      return NULL;
	    }
	  else
	    block_size = dir_size;
	}
      GDBM_DEBUG (GDBM_DEBUG_OPEN, "%s: block_size=%d", dbf->name, block_size);
      
      /* Get space for the file header. It will be written to disk, so
         make sure there's no garbage in it. */
      dbf->header = calloc (1, block_size);
      if (dbf->header == NULL)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  gdbm_close (dbf);
	  GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}

      /* Set the magic number and the block_size. */
      dbf->header->header_magic = GDBM_MAGIC;
      dbf->header->block_size = block_size;
      dbf->header->dir_size = dir_size;
      dbf->header->dir_bits = dir_bits;

      /* Allocate the space for the directory. */
      dbf->dir = (off_t *) malloc (dbf->header->dir_size);
      if (dbf->dir == NULL)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  gdbm_close (dbf);
	  GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}
      dbf->header->dir = dbf->header->block_size;

      /* Create the first and only hash bucket. */
      dbf->header->bucket_elems = bucket_element_count (dbf->header->block_size);
      dbf->header->bucket_size  = dbf->header->block_size;
      dbf->bucket = calloc (1, dbf->header->bucket_size);
      if (dbf->bucket == NULL)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  gdbm_close (dbf);
	  GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}
      _gdbm_new_bucket (dbf, dbf->bucket, 0);
      dbf->bucket->av_count = 1;
      dbf->bucket->bucket_avail[0].av_adr = 3*dbf->header->block_size;
      dbf->bucket->bucket_avail[0].av_size = dbf->header->block_size;

      /* Set table entries to point to hash buckets. */
      for (index = 0; index < GDBM_DIR_COUNT (dbf); index++)
	dbf->dir[index] = 2*dbf->header->block_size;

      /* Initialize the active avail block. */
      dbf->header->avail.size
	= ( (dbf->header->block_size - sizeof (gdbm_file_header))
	 / sizeof (avail_elem)) + 1;
      dbf->header->avail.count = 0;
      dbf->header->avail.next_block = 0;
      dbf->header->next_block  = 4*dbf->header->block_size;

      /* Write initial configuration to the file. */
      /* Block 0 is the file header and active avail block. */
      if (_gdbm_full_write (dbf, dbf->header, dbf->header->block_size))
	{
	  GDBM_DEBUG (GDBM_DEBUG_OPEN|GDBM_DEBUG_ERR,
		      "%s: error writing header: %s",
		      dbf->name, gdbm_db_strerror (dbf));
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}

      /* Block 1 is the initial bucket directory. */
      if (_gdbm_full_write (dbf, dbf->dir, dbf->header->dir_size))
	{
	  GDBM_DEBUG (GDBM_DEBUG_OPEN|GDBM_DEBUG_ERR,
		      "%s: error writing directory: %s",
		      dbf->name, gdbm_db_strerror (dbf));	  
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}

      /* Block 2 is the only bucket. */
      if (_gdbm_full_write (dbf, dbf->bucket, dbf->header->bucket_size))
	{
	  GDBM_DEBUG (GDBM_DEBUG_OPEN|GDBM_DEBUG_ERR,
		      "%s: error writing bucket: %s",
		      dbf->name, gdbm_db_strerror (dbf));	  
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}
      
      if (_gdbm_file_extend (dbf, dbf->header->next_block))
	{
	  GDBM_DEBUG (GDBM_DEBUG_OPEN|GDBM_DEBUG_ERR,
		      "%s: error extending file: %s",
		      dbf->name, gdbm_db_strerror (dbf));	  
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}
	  
      /* Wait for initial configuration to be written to disk. */
      gdbm_file_sync (dbf);

      free (dbf->bucket);
    }
  else
    {
      /* This is an old database.  Read in the information from the file
	 header and initialize the hash directory. */

      gdbm_file_header partial_header;  /* For the first part of it. */
      int rc;
      
      /* Read the partial file header. */
      if (_gdbm_full_read (dbf, &partial_header, sizeof (gdbm_file_header)))
	{
	  GDBM_DEBUG (GDBM_DEBUG_ERR|GDBM_DEBUG_OPEN,
		      "%s: error reading partial header: %s",
		      dbf->name, gdbm_db_strerror (dbf));
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}

      /* Is the header valid? */
      rc = validate_header (&partial_header, &file_stat);
      if (rc == GDBM_NEED_RECOVERY)
	{
	  dbf->need_recovery = 1;
	}
      else if (rc != GDBM_NO_ERROR)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  gdbm_close (dbf);
	  GDBM_SET_ERRNO2 (NULL, rc, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}
      
      /* It is a good database, read the entire header. */
      dbf->header = malloc (partial_header.block_size);
      if (dbf->header == NULL)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}
      
      memcpy (dbf->header, &partial_header, sizeof (gdbm_file_header));
      if (_gdbm_full_read (dbf, &dbf->header->avail.av_table[1],
			   dbf->header->block_size - sizeof (gdbm_file_header)))
	{
	  GDBM_DEBUG (GDBM_DEBUG_ERR|GDBM_DEBUG_OPEN,
		      "%s: error reading av_table: %s",
		      dbf->name, gdbm_db_strerror (dbf));
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}

      if (gdbm_avail_block_validate (dbf, &dbf->header->avail))
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}	  
	
      /* Allocate space for the hash table directory.  */
      dbf->dir = malloc (dbf->header->dir_size);
      if (dbf->dir == NULL)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  gdbm_close (dbf);
	  GDBM_SET_ERRNO2 (NULL, GDBM_MALLOC_ERROR, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}

      /* Read the hash table directory. */
      file_pos = gdbm_file_seek (dbf, dbf->header->dir, SEEK_SET);
      if (file_pos != dbf->header->dir)
	{
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  GDBM_SET_ERRNO2 (NULL, GDBM_FILE_SEEK_ERROR, FALSE, GDBM_DEBUG_OPEN);
	  return NULL;
	}

      if (_gdbm_full_read (dbf, dbf->dir, dbf->header->dir_size))
	{
	  GDBM_DEBUG (GDBM_DEBUG_ERR|GDBM_DEBUG_OPEN,
		      "%s: error reading dir: %s",
		      dbf->name, gdbm_db_strerror (dbf));
	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}

    }

#if HAVE_MMAP
  if (!(flags & GDBM_NOMMAP))
    {
      if (_gdbm_mapped_init (dbf) == 0)
	dbf->memory_mapping = TRUE;
      else
	{
	  /* gdbm_errno should already be set. */
	  GDBM_DEBUG (GDBM_DEBUG_ERR|GDBM_DEBUG_OPEN,
		      "%s: _gdbm_mapped_init failed: %s",
		      dbf->name, strerror (errno));

	  if (!(flags & GDBM_CLOERROR))
	    dbf->desc = -1;
	  SAVE_ERRNO (gdbm_close (dbf));
	  return NULL;
	}
    }
#endif

  /* Finish initializing dbf. */
  dbf->last_read = -1;
  dbf->bucket = NULL;
  dbf->bucket_dir = 0;
  dbf->cache_entry = NULL;
  dbf->header_changed = FALSE;
  dbf->directory_changed = FALSE;
  dbf->bucket_changed = FALSE;
  dbf->second_changed = FALSE;

  GDBM_DEBUG (GDBM_DEBUG_ALL, "%s: opened %s", dbf->name,
	      dbf->need_recovery ? "for recovery" : "successfully");

  /* Everything is fine, return the pointer to the file
     information structure.  */
  return dbf;
}
  
/* Initialize dbm system.  FILE is a pointer to the file name.  If the file
   has a size of zero bytes, a file initialization procedure is performed,
   setting up the initial structure in the file.  BLOCK_SIZE is used during
   initialization to determine the size of various constructs.  If the value
   is less than GDBM_MIN_BLOCK_SIZE, the file system blocksize is used,
   otherwise the value of BLOCK_SIZE is used.  BLOCK_SIZE is ignored if the
   file has previously initialized.  If FLAGS is set to GDBM_READ the user
   wants to just read the database and any call to dbm_store or dbm_delete
   will fail. Many readers can access the database at the same time.  If FLAGS
   is set to GDBM_WRITE, the user wants both read and write access to the
   database and requires exclusive access.  If FLAGS is GDBM_WRCREAT, the user
   wants both read and write access to the database and if the database does
   not exist, create a new one.  If FLAGS is GDBM_NEWDB, the user want a
   new database created, regardless of whether one existed, and wants read
   and write access to the new database.  Any error detected will cause a 
   return value of null and an approprate value will be in gdbm_errno.  If
   no errors occur, a pointer to the "gdbm file descriptor" will be
   returned. */
   

GDBM_FILE 
gdbm_open (const char *file, int block_size, int flags, int mode,
     	   void (*fatal_func) (const char *))
{
  int fd;
  /* additional bits for open(2) flags */
  int fbits = 0;

  switch (flags & GDBM_OPENMASK)
    {
    case GDBM_READER:
      fbits = O_RDONLY;
      break;

    case GDBM_WRITER:
      fbits = O_RDWR;
      break;

    case GDBM_WRCREAT:
    case GDBM_NEWDB:
      fbits = O_RDWR|O_CREAT;
      break;

    default:
      errno = EINVAL;
      GDBM_SET_ERRNO2 (NULL, GDBM_FILE_OPEN_ERROR, FALSE, GDBM_DEBUG_OPEN);
      return NULL;
    }
  if (flags & GDBM_CLOEXEC)
    fbits |= O_CLOEXEC;
  
  fd = open (file, fbits, mode);
  if (fd < 0)
    {
      GDBM_SET_ERRNO2 (NULL, GDBM_FILE_OPEN_ERROR, FALSE, GDBM_DEBUG_OPEN);
      return NULL;
    }
  return gdbm_fd_open (fd, file, block_size, flags | GDBM_CLOERROR,
		       fatal_func);
}

/* Initialize the bucket cache. */
int
_gdbm_init_cache (GDBM_FILE dbf, size_t size)
{
  int index;

  if (dbf->bucket_cache == NULL)
    {
      dbf->bucket_cache = calloc (size, sizeof(cache_elem));
      if (dbf->bucket_cache == NULL)
        {
          GDBM_SET_ERRNO (dbf, GDBM_MALLOC_ERROR, TRUE);
          return -1;
        }
      dbf->cache_size = size;

      for (index = 0; index < size; index++)
        {
	  (dbf->bucket_cache[index]).ca_bucket =
	    malloc (dbf->header->bucket_size);
          if ((dbf->bucket_cache[index]).ca_bucket == NULL)
	    {
              GDBM_SET_ERRNO (dbf, GDBM_MALLOC_ERROR, TRUE);
	      return -1;
            }
	  dbf->bucket_cache[index].ca_data.dptr = NULL;
	  dbf->bucket_cache[index].ca_data.dsize = 0;
	  _gdbm_cache_entry_invalidate (dbf, index);
        }
      dbf->bucket = dbf->bucket_cache[0].ca_bucket;
      dbf->cache_entry = &dbf->bucket_cache[0];
    }
  return 0;
}

void
_gdbm_cache_entry_invalidate (GDBM_FILE dbf, int index)
{
  dbf->bucket_cache[index].ca_adr = 0;
  dbf->bucket_cache[index].ca_changed = FALSE;
  dbf->bucket_cache[index].ca_data.hash_val = -1;
  dbf->bucket_cache[index].ca_data.elem_loc = -1;
}
