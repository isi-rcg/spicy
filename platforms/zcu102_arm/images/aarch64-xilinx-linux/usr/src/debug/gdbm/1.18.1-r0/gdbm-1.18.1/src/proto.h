/* proto.h - The prototypes for the dbm routines. */

/* This file is part of GDBM, the GNU data base manager.
   Copyright (C) 1990-1991, 1993, 2007, 2011, 2013, 2017-2018 Free
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


/* From bucket.c */
void _gdbm_new_bucket	(GDBM_FILE, hash_bucket *, int);
int _gdbm_get_bucket	(GDBM_FILE, int);
int _gdbm_read_bucket_at (GDBM_FILE dbf, off_t off, hash_bucket *bucket,
			  size_t size);

int _gdbm_split_bucket (GDBM_FILE, int);
int _gdbm_write_bucket (GDBM_FILE, cache_elem *);

/* From falloc.c */
off_t _gdbm_alloc       (GDBM_FILE, int);
int  _gdbm_free         (GDBM_FILE, off_t, int);
void _gdbm_put_av_elem  (avail_elem, avail_elem [], int *, int);

/* From findkey.c */
char *_gdbm_read_entry  (GDBM_FILE, int);
int _gdbm_findkey       (GDBM_FILE, datum, char **, int *);

/* From hash.c */
int _gdbm_hash (datum);
void _gdbm_hash_key (GDBM_FILE dbf, datum key, int *hash, int *bucket,
		     int *offset);
int _gdbm_bucket_dir (GDBM_FILE dbf, int hash);

/* From update.c */
int _gdbm_end_update   (GDBM_FILE);
void _gdbm_fatal	(GDBM_FILE, const char *);

/* From gdbmopen.c */
int _gdbm_init_cache	(GDBM_FILE, size_t);
void _gdbm_cache_entry_invalidate (GDBM_FILE, int);

int gdbm_avail_block_validate (GDBM_FILE dbf, avail_block *avblk);
int gdbm_bucket_avail_table_validate (GDBM_FILE dbf, hash_bucket *bucket);

int _gdbm_validate_header (GDBM_FILE dbf);

/* From mmap.c */
int _gdbm_mapped_init	(GDBM_FILE);
void _gdbm_mapped_unmap	(GDBM_FILE);
ssize_t _gdbm_mapped_read	(GDBM_FILE, void *, size_t);
ssize_t _gdbm_mapped_write	(GDBM_FILE, void *, size_t);
off_t _gdbm_mapped_lseek	(GDBM_FILE, off_t, int);
int _gdbm_mapped_sync	(GDBM_FILE);

/* From lock.c */
void _gdbm_unlock_file	(GDBM_FILE);
int _gdbm_lock_file	(GDBM_FILE);

/* From fullio.c */
int _gdbm_full_read (GDBM_FILE, void *, size_t);
int _gdbm_full_write (GDBM_FILE, void *, size_t);
int _gdbm_file_extend (GDBM_FILE dbf, off_t size);

/* From base64.c */
int _gdbm_base64_encode (const unsigned char *input, size_t input_len,
			 unsigned char **output, size_t *output_size,
			 size_t *outbytes);
int _gdbm_base64_decode (const unsigned char *input, size_t input_len,
			 unsigned char **output, size_t *output_size,
			 size_t *inbytes, size_t *outbytes);

int _gdbm_load (FILE *fp, GDBM_FILE *pdbf, unsigned long *line);
int _gdbm_dump (GDBM_FILE dbf, FILE *fp);

/* From recover.c */
int _gdbm_next_bucket_dir (GDBM_FILE dbf, int bucket_dir);

/* I/O functions */
static inline ssize_t
gdbm_file_read (GDBM_FILE dbf, void *buf, size_t size)
{
#if HAVE_MMAP
  return _gdbm_mapped_read (dbf, buf, size);
#else
  return read (dbf->desc, buf, size);
#endif
}

static inline ssize_t
gdbm_file_write (GDBM_FILE dbf, void *buf, size_t size)
{
#if HAVE_MMAP
  return _gdbm_mapped_write (dbf, buf, size);
#else
  return write (dbf->desc, buf, size);
#endif
}

static inline off_t
gdbm_file_seek (GDBM_FILE dbf, off_t off, int whence)
{
#if HAVE_MMAP
  return _gdbm_mapped_lseek (dbf, off, whence);
#else
  return lseek (dbf->desc, off, whence);
#endif
}

static inline int
gdbm_file_sync (GDBM_FILE dbf)
{
#if HAVE_MMAP
  return _gdbm_mapped_sync (dbf);
#elif HAVE_FSYNC
  if (fsync (dbf->desc))
    {
      GDBM_SET_ERRNO (dbf, GDBM_FILE_SYNC_ERROR, TRUE);
      return 1;
    }
#else
  sync ();
  sync ();
  return 0;
#endif
}

