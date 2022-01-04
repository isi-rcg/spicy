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
   along with GDBM. If not, see <http://www.gnu.org/licenses/>.    */

#include "gdbmtool.h"
#include "gdbm.h"
#include "gram.h"

#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <pwd.h>
#include <sys/ioctl.h>
#ifdef HAVE_SYS_TERMIOS_H
# include <sys/termios.h>
#endif
#include <stdarg.h>
#ifdef HAVE_LOCALE_H
# include <locale.h>
#endif

char *file_name = NULL;       /* Database file name */   
GDBM_FILE gdbm_file = NULL;   /* Database to operate upon */
datum key_data;               /* Current key */
datum return_data;            /* Current data */
int open_mode;                /* Default open mode */

#define SIZE_T_MAX ((size_t)-1)

unsigned input_line;


static void
closedb (void)
{
  if (gdbm_file)
    {
      gdbm_close (gdbm_file);
      gdbm_file = NULL;
      free (file_name);
      file_name = NULL;
    }
}

static int
opendb (char *dbname)
{
  int cache_size = 0;
  int block_size = 0;
  int flags = 0;
  int filemode;
  GDBM_FILE db;
  
  switch (variable_get ("cachesize", VART_INT, (void**) &cache_size))
    {
    case VAR_OK:
    case VAR_ERR_NOTSET:
      break;
    default:
      abort ();
    }
  switch (variable_get ("blocksize", VART_INT, (void**) &block_size))
    {
    case VAR_OK:
    case VAR_ERR_NOTSET:
      break;
    default:
      abort ();
    }
  
  if (!variable_is_true ("lock"))
    flags |= GDBM_NOLOCK;
  if (!variable_is_true ("mmap"))
    flags |= GDBM_NOMMAP;
  if (variable_is_true ("sync"))
    flags |= GDBM_SYNC;
  
  if (open_mode == GDBM_NEWDB)
    {
      if (interactive () && variable_is_true ("confirm") &&
	  access (dbname, F_OK) == 0)
	{
	  if (!getyn (_("database %s already exists; overwrite"), dbname))
	    return 1;
	}
    }
  
  if (variable_get ("filemode", VART_INT, (void**) &filemode))
    abort ();

  db = gdbm_open (dbname, block_size, open_mode | flags, filemode, NULL);

  if (db == NULL)
    {
      terror (_("cannot open database %s: %s"), dbname,
	      gdbm_strerror (gdbm_errno));
      return 1;
    }

  if (cache_size &&
      gdbm_setopt (db, GDBM_CACHESIZE, &cache_size, sizeof (int)) == -1)
    terror (_("gdbm_setopt failed: %s"), gdbm_strerror (gdbm_errno));

  if (variable_is_true ("coalesce"))
    {
      int t = 1;
      if (gdbm_setopt (db, GDBM_SETCOALESCEBLKS, &t, sizeof (t)) == -1)
	terror (_("gdbm_setopt failed: %s"), gdbm_strerror (gdbm_errno));
    }
  if (variable_is_true ("centfree"))
    {
      int t = 1;
      if (gdbm_setopt (db, GDBM_SETCENTFREE, &t, sizeof (t)) == -1)
	terror (_("gdbm_setopt failed: %s"), gdbm_strerror (gdbm_errno));
    }
  
  if (gdbm_file)
    gdbm_close (gdbm_file);
  
  gdbm_file = db;
  return 0;
}

static int
checkdb (void)
{
  if (!gdbm_file)
    {
      if (!file_name)
	{
	  file_name = estrdup (GDBMTOOL_DEFFILE);
	  terror (_("warning: using default database file %s"),
			file_name);
	}
      return opendb (file_name);
    }
  return 0;
}

static int
checkdb_begin (struct handler_param *param GDBM_ARG_UNUSED,
	       size_t *exp_count GDBM_ARG_UNUSED)
{
  return checkdb ();
}

size_t
bucket_print_lines (hash_bucket *bucket)
{
  return 6 + gdbm_file->header->bucket_elems + 3 + bucket->av_count;
}

static void
format_key_start (FILE *fp, bucket_element *elt)
{
  int size = SMALL < elt->key_size ? SMALL : elt->key_size;
  int i;

  for (i = 0; i < size; i++)
    {
      if (isprint (elt->key_start[i]))
	fprintf (fp, "   %c", elt->key_start[i]);
      else
	fprintf (fp, " %03o", elt->key_start[i]);
    }
}

/* Debug procedure to print the contents of the current hash bucket. */
void
print_bucket (FILE *fp, hash_bucket *bucket, const char *mesg, ...)
{
  int index;
  va_list ap;

  fprintf (fp, "******* ");
  va_start(ap, mesg);
  vfprintf (fp, mesg, ap);
  va_end (ap);
  fprintf (fp, " **********\n\n");
  fprintf (fp,
	   _("bits = %d\ncount= %d\nHash Table:\n"),
	   bucket->bucket_bits, bucket->count);
  fprintf (fp,
	   _("    #    hash value     key size    data size     data adr home  key start\n"));
  for (index = 0; index < gdbm_file->header->bucket_elems; index++)
    {
      fprintf (fp, " %4d  %12x  %11d  %11d  %11lu %4d", index,
	       bucket->h_table[index].hash_value,
	       bucket->h_table[index].key_size,
	       bucket->h_table[index].data_size,
	       (unsigned long) bucket->h_table[index].data_pointer,
	       bucket->h_table[index].hash_value %
	       gdbm_file->header->bucket_elems);
      if (bucket->h_table[index].key_size)
	{
	  fprintf (fp, " ");
	  format_key_start (fp, &bucket->h_table[index]);
	}
      fprintf (fp, "\n");
    }

  fprintf (fp, _("\nAvail count = %1d\n"), bucket->av_count);
  fprintf (fp, _("Address           size\n"));
  for (index = 0; index < bucket->av_count; index++)
    fprintf (fp, "%11lu%9d\n",
	     (unsigned long) bucket->bucket_avail[index].av_adr,
	     bucket->bucket_avail[index].av_size);
}

size_t
_gdbm_avail_list_size (GDBM_FILE dbf, size_t min_size)
{
  int             temp;
  int             size;
  avail_block    *av_stk;
  size_t          lines;
  
  lines = 4 + dbf->header->avail.count;
  if (lines > min_size)
    return lines;
  /* Initialize the variables for a pass throught the avail stack. */
  temp = dbf->header->avail.next_block;
  size = (((dbf->header->avail.size * sizeof (avail_elem)) >> 1)
	  + sizeof (avail_block));
  av_stk = emalloc (size);

  /* Traverse the stack. */
  while (temp)
    {
      if (gdbm_file_seek (dbf, temp, SEEK_SET) != temp)
	{
	  terror ("lseek: %s", strerror (errno));
	  break;
	}
      
      if (_gdbm_full_read (dbf, av_stk, size))
	{
	  terror ("read: %s", gdbm_db_strerror (dbf));
	  break;
	}

      if (gdbm_avail_block_valid_p (av_stk))
	{
	  lines += av_stk->count;
	  if (lines > min_size)
	    break;
	}
      temp = av_stk->next_block;
    }
  free (av_stk);

  return lines;
}

static void
av_table_display (avail_elem *av_table, int count, FILE *fp)
{
  int i;
  
  for (i = 0; i < count; i++)
    {
      fprintf (fp, "  %15d   %10lu \n",
	       av_table[i].av_size, (unsigned long) av_table[i].av_adr);
    }
}

void
_gdbm_print_avail_list (FILE *fp, GDBM_FILE dbf)
{
  int             temp;
  int             size;
  avail_block    *av_stk;
  
  /* Print the the header avail block.  */
  fprintf (fp, _("\nheader block\nsize  = %d\ncount = %d\n"),
	   dbf->header->avail.size, dbf->header->avail.count);
  av_table_display (dbf->header->avail.av_table, dbf->header->avail.count, fp);

  /* Initialize the variables for a pass throught the avail stack. */
  temp = dbf->header->avail.next_block;
  size = (dbf->header->avail.size * sizeof (avail_elem))
	  + sizeof (avail_block);
  av_stk = emalloc (size);

  /* Print the stack. */
  while (temp)
    {
      if (gdbm_file_seek (dbf, temp, SEEK_SET) != temp)
	{
	  terror ("lseek: %s", strerror (errno));
	  break;
	}
      
      if (_gdbm_full_read (dbf, av_stk, size))
	{
          terror ("read: %s", gdbm_db_strerror (dbf));
	  break;
	}

      /* Print the block! */
      fprintf (fp, _("\nblock = %d\nsize  = %d\ncount = %d\n"), temp,
	       av_stk->size, av_stk->count);
      if (gdbm_avail_block_validate (dbf, av_stk) == 0)
	av_table_display (av_stk->av_table, av_stk->count, fp);
      else
	terror (_("invalid avail_block"));
      temp = av_stk->next_block;
    }
  free (av_stk);
}

void
_gdbm_print_bucket_cache (FILE *fp, GDBM_FILE dbf)
{
  int             index;
  char            changed;

  if (dbf->bucket_cache != NULL)
    {
      fprintf (fp,
	_("Bucket Cache (size %zu):\n  Index:         Address  Changed  Data_Hash \n"),
	 dbf->cache_size);
      for (index = 0; index < dbf->cache_size; index++)
	{
	  changed = dbf->bucket_cache[index].ca_changed;
	  fprintf (fp, "  %5d:  %15lu %7s  %x\n",
		   index,
		   (unsigned long) dbf->bucket_cache[index].ca_adr,
		   (changed ? _("True") : _("False")),
		   dbf->bucket_cache[index].ca_data.hash_val);
	}
    }
  else
    fprintf (fp, _("Bucket cache has not been initialized.\n"));
}

int
trimnl (char *str)
{
  int len = strlen (str);

  if (str[len - 1] == '\n')
    {
      str[--len] = 0;
      return 1;
    }
  return 0;
}

int
get_screen_lines ()
{
#ifdef TIOCGWINSZ
  if (isatty (1))
    {
      struct winsize ws;

      ws.ws_col = ws.ws_row = 0;
      if ((ioctl(1, TIOCGWINSZ, (char *) &ws) < 0) || ws.ws_row == 0)
	{
	  const char *lines = getenv ("LINES");
	  if (lines)
	    ws.ws_row = strtol (lines, NULL, 10);
	}
      return ws.ws_row;
    }
#else
  const char *lines = getenv ("LINES");
  if (lines)
    return strtol (lines, NULL, 10);
#endif
  return -1;
}

/* Open database */
void
open_handler (struct handler_param *param)
{
  char *name = tildexpand (PARAM_STRING (param, 0));
  closedb ();
  if (opendb (name) == 0)
    file_name = name;
  else
    free (name);
}

/* Close database */
void
close_handler (struct handler_param *param)
{
  if (!gdbm_file)
    terror (_("nothing to close"));
  else
    closedb ();
}


static char *
count_to_str (gdbm_count_t count, char *buf, size_t bufsize)
{
  char *p = buf + bufsize;

  *--p = 0;
  if (count == 0)
    *--p = '0';
  else
    while (count)
      {
	if (p == buf)
	  return NULL;
	*--p = '0' + count % 10;
	count /= 10;
      }
  return p;
}
  
/* count - count items in the database */
void
count_handler (struct handler_param *param)
{
  gdbm_count_t count;

  if (gdbm_count (gdbm_file, &count))
    terror ("gdbm_count: %s", gdbm_strerror (gdbm_errno));
  else
    {
      char buf[128];
      char *p = count_to_str (count, buf, sizeof buf);

      if (!p)
	terror (_("count buffer overflow"));
      else
	fprintf (param->fp, 
		 ngettext ("There is %s item in the database.\n",
			   "There are %s items in the database.\n",
			   count),
		 p);
    }
}

/* delete KEY - delete a key*/
void
delete_handler (struct handler_param *param)
{
  if (gdbm_delete (gdbm_file, PARAM_DATUM (param, 0)) != 0)
    {
      if (gdbm_errno == GDBM_ITEM_NOT_FOUND)
	terror (_("Item not found"));
      else
	terror (_("Can't delete: %s"), gdbm_strerror (gdbm_errno));
    }
}

/* fetch KEY - fetch a record by its key */
void
fetch_handler (struct handler_param *param)
{
  return_data = gdbm_fetch (gdbm_file, PARAM_DATUM (param, 0));
  if (return_data.dptr != NULL)
    {
      datum_format (param->fp, &return_data, dsdef[DS_CONTENT]);
      fputc ('\n', param->fp);
      free (return_data.dptr);
    }
  else if (gdbm_errno == GDBM_ITEM_NOT_FOUND)
    terror ("%s", _("No such item found."));
  else
    terror (_("Can't fetch data: %s"), gdbm_strerror (gdbm_errno));
}

/* store KEY DATA - store data */
void
store_handler (struct handler_param *param)
{
  if (gdbm_store (gdbm_file,
		  PARAM_DATUM (param, 0), PARAM_DATUM (param, 1),
		  GDBM_REPLACE) != 0)
    terror (_("Item not inserted: %s."), gdbm_db_strerror (gdbm_file));
}

/* first - begin iteration */

void
firstkey_handler (struct handler_param *param)
{
  if (key_data.dptr != NULL)
    free (key_data.dptr);
  key_data = gdbm_firstkey (gdbm_file);
  if (key_data.dptr != NULL)
    {
      datum_format (param->fp, &key_data, dsdef[DS_KEY]);
      fputc ('\n', param->fp);

      return_data = gdbm_fetch (gdbm_file, key_data);
      datum_format (param->fp, &return_data, dsdef[DS_CONTENT]);
      fputc ('\n', param->fp);

      free (return_data.dptr);
    }
  else if (gdbm_errno == GDBM_ITEM_NOT_FOUND)
    fprintf (param->fp, _("No such item found.\n"));
  else
    terror (_("Can't find key: %s"), gdbm_strerror (gdbm_errno));
}

/* next [KEY] - next key */
void
nextkey_handler (struct handler_param *param)
{
  if (param->argc == 1)
    {
      if (key_data.dptr != NULL)
	free (key_data.dptr);
      key_data.dptr = emalloc (PARAM_DATUM (param, 0).dsize);
      key_data.dsize = PARAM_DATUM (param, 0).dsize;
      memcpy (key_data.dptr, PARAM_DATUM (param, 0).dptr, key_data.dsize);
    }
  return_data = gdbm_nextkey (gdbm_file, key_data);
  if (return_data.dptr != NULL)
    {
      key_data = return_data;
      datum_format (param->fp, &key_data, dsdef[DS_KEY]);
      fputc ('\n', param->fp);

      return_data = gdbm_fetch (gdbm_file, key_data);
      datum_format (param->fp, &return_data, dsdef[DS_CONTENT]);
      fputc ('\n', param->fp);

      free (return_data.dptr);
    }
  else if (gdbm_errno == GDBM_ITEM_NOT_FOUND)
    {
      terror ("%s", _("No such item found."));
      free (key_data.dptr);
      key_data.dptr = NULL;
    }
  else
    terror (_("Can't find key: %s"), gdbm_strerror (gdbm_errno));
}

/* reorganize */
void
reorganize_handler (struct handler_param *param GDBM_ARG_UNUSED)
{
  if (gdbm_reorganize (gdbm_file))
    terror ("%s", _("Reorganization failed."));
  else
    fprintf (param->fp, "%s\n", _("Reorganization succeeded."));
}

static void
err_printer (void *data GDBM_ARG_UNUSED, char const *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
  fprintf (stderr, "\n");
}

/* recover sumamry verbose backup max-failed-keys=N max-failed-buckets=N max-failures=N */
void
recover_handler (struct handler_param *param)
{
  gdbm_recovery rcvr;
  int flags = 0;
  int rc;
  int i;
  char *p;
  int summary = 0;
  
  for (i = 0; i < param->argc; i++)
    {
      char *arg = PARAM_STRING (param, i);
      if (strcmp (arg, "verbose") == 0)
	{
	  rcvr.errfun = err_printer;
	  flags |= GDBM_RCVR_ERRFUN;
	}
      else if (strcmp (arg, "force") == 0)
	{
	  flags |= GDBM_RCVR_FORCE;
	}
      else if (strcmp (arg, "summary") == 0)
	{
	  summary = 1;
	}
      else if (strcmp (arg, "backup") == 0)
	{
	  flags |= GDBM_RCVR_BACKUP;
	}
      else if (strncmp (arg, "max-failures=", 13) == 0)
	{
	  rcvr.max_failures = strtoul (arg + 13, &p, 10);
	  if (*p)
	    {
	      printf (_("not a number (stopped near %s)\n"), p);
	      return;
	    }
	  flags |= GDBM_RCVR_MAX_FAILURES;
	}
      else if (strncmp (arg, "max-failed-keys=", 16) == 0)
	{
	  rcvr.max_failed_keys = strtoul (arg + 16, &p, 10);
	  if (*p)
	    {
	      printf (_("not a number (stopped near %s)\n"), p);
	      return;
	    }
	  flags |= GDBM_RCVR_MAX_FAILED_KEYS;
	}
      else if (strncmp (arg, "max-failed-buckets=", 19) == 0)
	{
	  rcvr.max_failures = strtoul (arg + 19, &p, 10);
	  if (*p)
	    {
	      printf (_("not a number (stopped near %s)\n"), p);
	      return;
	    }
	  flags |= GDBM_RCVR_MAX_FAILED_BUCKETS;
	}
      else
	{
	  terror (_("unrecognized argument: %s"), arg);
	  return;
	}
    }

  rc = gdbm_recover (gdbm_file, &rcvr, flags);

  if (rc == 0)
    {
      fprintf (param->fp, _("Recovery succeeded.\n"));
      if (summary)
	{
	  fprintf (param->fp,
		   _("Keys recovered: %lu, failed: %lu, duplicate: %lu\n"),
		   (unsigned long) rcvr.recovered_keys,
		   (unsigned long) rcvr.failed_keys,
		   (unsigned long) rcvr.duplicate_keys);
	  fprintf (param->fp,
		   _("Buckets recovered: %lu, failed: %lu\n"),
		   (unsigned long) rcvr.recovered_buckets,
		   (unsigned long) rcvr.failed_buckets);
	}
      
      if (rcvr.backup_name)
	{
	  fprintf (param->fp,
		   _("Original database preserved in file %s"),
		   rcvr.backup_name);
	  free (rcvr.backup_name);
	}
      fputc ('\n', param->fp);
    }
  else
    {
      fprintf (stderr, _("Recovery failed: %s"), gdbm_strerror (gdbm_errno));
      if (gdbm_syserr[gdbm_errno])
	fprintf (stderr, ": %s", strerror (errno));
      fputc ('\n', stderr);
    }
}  

/* avail - print available list */
int
avail_begin (struct handler_param *param GDBM_ARG_UNUSED, size_t *exp_count)
{
  if (checkdb ())
    return 1;
  if (exp_count)
    *exp_count = _gdbm_avail_list_size (gdbm_file, SIZE_T_MAX);
  return 0;
}

void
avail_handler (struct handler_param *param)
{
  _gdbm_print_avail_list (param->fp, gdbm_file);
}

/* C - print current bucket */
int
print_current_bucket_begin (struct handler_param *param GDBM_ARG_UNUSED,
			    size_t *exp_count)
{
  if (checkdb ())
    return 1;
  if (!gdbm_file->bucket)
    return 0;
  if (exp_count)
    *exp_count = gdbm_file->bucket
                   ? bucket_print_lines (gdbm_file->bucket) + 3
                   : 1;
  return 0;
}

void
print_current_bucket_handler (struct handler_param *param)
{
  if (!gdbm_file->bucket)
    fprintf (param->fp, _("no current bucket\n"));
  else
    {
      if (param->argc)
	print_bucket (param->fp, gdbm_file->bucket, _("Bucket #%s"),
		      PARAM_STRING (param, 0));
      else
	print_bucket (param->fp, gdbm_file->bucket, "%s", _("Current bucket"));
      fprintf (param->fp, _("\n current directory entry = %d.\n"),
	       gdbm_file->bucket_dir);
      fprintf (param->fp, _(" current bucket address  = %lu.\n"),
	       (unsigned long) gdbm_file->cache_entry->ca_adr);
    }
}

int
getnum (int *pnum, char *arg, char **endp)
{
  char *p;
  unsigned long x = strtoul (arg, &p, 10);
  if (*p && !isspace (*p))
    {
      printf (_("not a number (stopped near %s)\n"), p);
      return 1;
    }
  while (*p && isspace (*p))
    p++;
  if (endp)
    *endp = p;
  else if (*p)
    {
      printf (_("not a number (stopped near %s)\n"), p);
      return 1;
    }
  *pnum = x;
  return 0;
}
  
/* bucket NUM - print a bucket and set it as a current one.
   Uses print_current_bucket_handler */
int
print_bucket_begin (struct handler_param *param, size_t *exp_count)
{
  int temp;

  if (checkdb ())
    return 1;
  
  if (getnum (&temp, PARAM_STRING (param, 0), NULL))
    return 1;

  if (temp >= GDBM_DIR_COUNT (gdbm_file))
    {
      terror (_("Not a bucket."));
      return 1;
    }
  if (_gdbm_get_bucket (gdbm_file, temp))
    {
      terror ("%s", gdbm_db_strerror (gdbm_file));
      return 1;
    }
  if (exp_count)
    *exp_count = bucket_print_lines (gdbm_file->bucket) + 3;
  return 0;
}

/* dir - print hash directory */
int
print_dir_begin (struct handler_param *param GDBM_ARG_UNUSED, size_t *exp_count)
{
  if (checkdb ())
    return 1;
  if (exp_count)
    *exp_count = GDBM_DIR_COUNT (gdbm_file) + 3;
  return 0;
}

static size_t
bucket_count (void)
{
  int i;
  off_t last = 0;
  size_t count = 0;
  
  for (i = 0; i < GDBM_DIR_COUNT (gdbm_file); i++)
    {
      if (gdbm_file->dir[i] != last)
	{
	  ++count;
	  last = gdbm_file->dir[i];
	}
    }
  return count;
}

void
print_dir_handler (struct handler_param *param)
{
  int i;
  
  fprintf (param->fp, _("Hash table directory.\n"));
  fprintf (param->fp, _("  Size =  %d.  Bits = %d,  Buckets = %zu.\n\n"),
	   gdbm_file->header->dir_size, gdbm_file->header->dir_bits,
	   bucket_count ());
  
  for (i = 0; i < GDBM_DIR_COUNT (gdbm_file); i++)
    fprintf (param->fp, "  %10d:  %12lu\n",
	     i, (unsigned long) gdbm_file->dir[i]);
}

/* header - print file handler */
int
print_header_begin (struct handler_param *param GDBM_ARG_UNUSED, size_t *exp_count)
{
  if (checkdb ())
    return 1;
  if (exp_count)
    *exp_count = 14;
  return 0;
}

void
print_header_handler (struct handler_param *param)
{
  FILE *fp = param->fp;
  
  fprintf (fp, _("\nFile Header: \n\n"));
  fprintf (fp, _("  table        = %lu\n"),
	   (unsigned long) gdbm_file->header->dir);
  fprintf (fp, _("  table size   = %d\n"), gdbm_file->header->dir_size);
  fprintf (fp, _("  table bits   = %d\n"), gdbm_file->header->dir_bits);
  fprintf (fp, _("  block size   = %d\n"), gdbm_file->header->block_size);
  fprintf (fp, _("  bucket elems = %d\n"), gdbm_file->header->bucket_elems);
  fprintf (fp, _("  bucket size  = %d\n"), gdbm_file->header->bucket_size);
  fprintf (fp, _("  header magic = %x\n"), gdbm_file->header->header_magic);
  fprintf (fp, _("  next block   = %lu\n"),
	   (unsigned long) gdbm_file->header->next_block);
  fprintf (fp, _("  avail size   = %d\n"), gdbm_file->header->avail.size);
  fprintf (fp, _("  avail count  = %d\n"), gdbm_file->header->avail.count);
  fprintf (fp, _("  avail nx blk = %lu\n"),
	   (unsigned long) gdbm_file->header->avail.next_block);
}  

/* hash KEY - hash the key */
void
hash_handler (struct handler_param *param)
{
  if (gdbm_file)
    {
      int hashval, bucket, off;
      _gdbm_hash_key (gdbm_file, PARAM_DATUM (param, 0),
		       &hashval, &bucket, &off);
      fprintf (param->fp, _("hash value = %x, bucket #%u, slot %u"),
		hashval,
		hashval >> (GDBM_HASH_BITS - gdbm_file->header->dir_bits),
		hashval % gdbm_file->header->bucket_elems);
    }
  else
    fprintf (param->fp, _("hash value = %x"),
	      _gdbm_hash (PARAM_DATUM (param, 0)));
  fprintf (param->fp, ".\n");
}

/* cache - print the bucket cache */
int
print_cache_begin (struct handler_param *param GDBM_ARG_UNUSED, size_t *exp_count)
{
  if (checkdb ())
    return 1;
  if (exp_count)
    *exp_count = gdbm_file->bucket_cache ? gdbm_file->cache_size + 1 : 1;
  return 0;
}

void
print_cache_handler (struct handler_param *param)
{
  _gdbm_print_bucket_cache (param->fp, gdbm_file);
}

/* version - print GDBM version */
void
print_version_handler (struct handler_param *param)
{
  fprintf (param->fp, "%s\n", gdbm_version);
}

/* list - List all entries */
int
list_begin (struct handler_param *param GDBM_ARG_UNUSED, size_t *exp_count)
{
  if (checkdb ())
    return 1;
  if (exp_count)
    {
      gdbm_count_t count;

      if (gdbm_count (gdbm_file, &count))
	 *exp_count = 0;
      else if (count > SIZE_T_MAX)
	 *exp_count = SIZE_T_MAX;
      else
	 *exp_count = count;
    }

  return 0;
}

void
list_handler (struct handler_param *param)
{
  datum key;
  datum data;

  key = gdbm_firstkey (gdbm_file);
  while (key.dptr)
    {
      datum nextkey = gdbm_nextkey (gdbm_file, key);

      data = gdbm_fetch (gdbm_file, key);
      if (!data.dptr)
	 {
	   terror (_("%s; the key was:"), gdbm_db_strerror (gdbm_file));
	   datum_format (stderr, &key, dsdef[DS_KEY]);
	 }
      else
	 {
	   datum_format (param->fp, &key, dsdef[DS_KEY]);
	   fputc (' ', param->fp);
	   datum_format (param->fp, &data, dsdef[DS_CONTENT]);
	   fputc ('\n', param->fp);
	   free (data.dptr);
	 }
      free (key.dptr);
      key = nextkey;
    }
}

/* quit - quit the program */
void
quit_handler (struct handler_param *param GDBM_ARG_UNUSED)
{
  closedb ();
  input_done ();
  exit (EXIT_OK);
}

/* export FILE [truncate] - export to a flat file format */
void
export_handler (struct handler_param *param)
{
  int format = GDBM_DUMP_FMT_ASCII;
  int flags = GDBM_WRCREAT;
  int i;
  int filemode;

  for (i = 1; i < param->argc; i++)
    {
      if (strcmp (PARAM_STRING (param, i), "truncate") == 0)
	 flags = GDBM_NEWDB;
      else if (strcmp (PARAM_STRING (param, i), "binary") == 0)
	 format = GDBM_DUMP_FMT_BINARY;
      else if (strcmp (PARAM_STRING (param, i), "ascii") == 0)
	 format = GDBM_DUMP_FMT_ASCII;
      else
	 {
	   terror (_("unrecognized argument: %s"), PARAM_STRING (param, i));
	   return;
	 }
    }

  if (variable_get ("filemode", VART_INT, (void**) &filemode))
    abort ();
  if (gdbm_dump (gdbm_file, PARAM_STRING (param, 0), format, flags, filemode))
    {
      terror (_("error dumping database: %s"),
		     gdbm_strerror (gdbm_errno));
    }
}

/* import FILE [replace] [nometa] - import from a flat file */
void
import_handler (struct handler_param *param)
{
  int flag = GDBM_INSERT;
  unsigned long err_line;
  int meta_mask = 0;
  int i;
  int rc;

  for (i = 0; i < param->argc; i++)
    {
      if (strcmp (PARAM_STRING (param, i), "replace") == 0)
	 flag = GDBM_REPLACE;
      else if (strcmp (PARAM_STRING (param, i), "nometa") == 0)
	 meta_mask = GDBM_META_MASK_MODE | GDBM_META_MASK_OWNER;
      else
	 {
	   terror (_("unrecognized argument: %s"),
		   PARAM_STRING (param, i));
	   return;
	 }
    }

  rc = gdbm_load (&gdbm_file, PARAM_STRING (param, 0), flag,
		   meta_mask, &err_line);
  if (rc && gdbm_errno == GDBM_NO_DBNAME)
    {
      int t = open_mode;

      open_mode = GDBM_NEWDB;
      rc = checkdb ();
      open_mode = t;

      if (rc)
	 return;

      rc = gdbm_load (&gdbm_file, PARAM_STRING (param, 0), flag,
		       meta_mask, &err_line);
    }
  if (rc)
    {
      switch (gdbm_errno)
	 {
	 case GDBM_ERR_FILE_OWNER:
	 case GDBM_ERR_FILE_MODE:
	   terror (_("error restoring metadata: %s (%s)"),
			 gdbm_strerror (gdbm_errno), strerror (errno));
	   break;

	 default:
	   if (err_line)
	     terror ("%s:%lu: %s", PARAM_STRING (param, 0), err_line,
		     gdbm_strerror (gdbm_errno));
	   else
	     terror (_("cannot load from %s: %s"), PARAM_STRING (param, 0),
		     gdbm_strerror (gdbm_errno));
	 }
      return;
    }

  free (file_name);
  if (gdbm_setopt (gdbm_file, GDBM_GETDBNAME, &file_name, sizeof (file_name)))
    terror (_("gdbm_setopt failed: %s"), gdbm_strerror (gdbm_errno));
}

/* status - print current program status */
void
status_handler (struct handler_param *param)
{
  if (file_name)
    fprintf (param->fp, _("Database file: %s\n"), file_name);
  else
    fprintf (param->fp, "%s\n", _("No database name"));
  if (gdbm_file)
    fprintf (param->fp, "%s\n", _("Database is open"));
  else
    fprintf (param->fp, "%s\n", _("Database is not open"));
  dsprint (param->fp, DS_KEY, dsdef[DS_KEY]);
  dsprint (param->fp, DS_CONTENT, dsdef[DS_CONTENT]);
}

#if GDBM_DEBUG_ENABLE
static int
debug_flag_printer (void *data, int flag, char const *tok)
{
  FILE *fp = data;
  fprintf (fp, " %s", tok);
  return 0;
}
#endif

void
debug_handler (struct handler_param *param)
{
#if GDBM_DEBUG_ENABLE
  if (param->vararg)
    {
      struct gdbmarg *arg;
      int i;
      
      for (arg = param->vararg, i = 0; arg; arg = arg->next, i++)
	{
	  if (arg->type == GDBM_ARG_STRING)
	    {
	      int flag;
	      int negate;
	      char const *tok = arg->v.string;
	      
	      if (tok[0] == '-')
		{
		  ++tok;
		  negate = 1;
		}
	      else if (tok[0] == '+')
		{
		  ++tok;
		  negate = 0;
		}
	      else
		negate = 0;
	      
              flag = gdbm_debug_token (tok);
	      if (flag)
		{
		  if (negate)
		    gdbm_debug_flags &= ~flag;
		  else
		    gdbm_debug_flags |= flag;
		}
	      else
		terror (_("unknown debug flag: %s"), tok);
	    }
	  else
	    terror (_("invalid type of argument %d"), i);
	}
    }
  else
    {
      fprintf (param->fp, _("Debug flags:"));
      if (gdbm_debug_flags)
	{
	  gdbm_debug_parse_state (debug_flag_printer, param->fp);
	}
      else
	fprintf (param->fp, " %s", _("none"));
      fputc ('\n', param->fp);
    }
#else
  terror ("%s", _("compiled without debug support"));
#endif
}

void
source_handler (struct handler_param *param)
{
  char *fname = tildexpand (PARAM_STRING (param, 0));
  instream_t istr = instream_file_create (fname);
  free (fname);
  if (istr && input_context_push (istr) == 0)
    yyparse ();
}


void help_handler (struct handler_param *param);
int help_begin (struct handler_param *param, size_t *exp_count);

struct argdef
{
  char *name;
  int type;
  int ds;
};

#define NARGS 10

enum command_repeat_type
  {
    REPEAT_NEVER,
    REPEAT_ALWAYS,
    REPEAT_NOARG
  };

struct command
{
  char *name;           /* Command name */
  size_t len;           /* Name length */
  int tok;
  int  (*begin) (struct handler_param *param, size_t *);
  void (*handler) (struct handler_param *param);
  void (*end) (void *data);
  struct argdef args[NARGS];
  int variadic;
  enum command_repeat_type repeat;
  char *doc;
};

struct command command_tab[] = {
#define S(s) #s, sizeof (#s) - 1
  { S(count), T_CMD,
    checkdb_begin, count_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("count (number of entries)") },
  { S(delete), T_CMD,
    checkdb_begin, delete_handler, NULL,
    { { N_("KEY"), GDBM_ARG_DATUM, DS_KEY }, { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("delete a record") },
  { S(export), T_CMD,
    checkdb_begin, export_handler, NULL,
    { { N_("FILE"), GDBM_ARG_STRING },
      { "[truncate]", GDBM_ARG_STRING },
      { "[binary|ascii]", GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("export") },
  { S(fetch), T_CMD,
    checkdb_begin, fetch_handler, NULL,
    { { N_("KEY"), GDBM_ARG_DATUM, DS_KEY }, { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("fetch record") },
  { S(import), T_CMD,
    NULL, import_handler, NULL,
    { { N_("FILE"), GDBM_ARG_STRING },
      { "[replace]", GDBM_ARG_STRING },
      { "[nometa]" , GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    FALSE,
    N_("import") },
  { S(list), T_CMD,
    list_begin, list_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("list") },
  { S(next), T_CMD,
    checkdb_begin, nextkey_handler, NULL,
    { { N_("[KEY]"), GDBM_ARG_DATUM, DS_KEY },
      { NULL } },
    FALSE,
    REPEAT_NOARG,
    N_("nextkey") },
  { S(store), T_CMD,
    checkdb_begin, store_handler, NULL,
    { { N_("KEY"), GDBM_ARG_DATUM, DS_KEY },
      { N_("DATA"), GDBM_ARG_DATUM, DS_CONTENT },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("store") },
  { S(first), T_CMD,
    checkdb_begin, firstkey_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("firstkey") },
  { S(reorganize), T_CMD,
    checkdb_begin, reorganize_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("reorganize") },
  { S(recover), T_CMD,
    checkdb_begin, recover_handler, NULL,
    { { "[verbose]", GDBM_ARG_STRING },
      { "[summary]", GDBM_ARG_STRING },
      { "[backup]",  GDBM_ARG_STRING },
      { "[force]",   GDBM_ARG_STRING },
      { "[max-failed-keys=N]", GDBM_ARG_STRING },
      { "[max-failed-buckets=N]", GDBM_ARG_STRING },
      { "[max-failures=N]", GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("recover the database") },
  { S(avail), T_CMD,
    avail_begin, avail_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print avail list") }, 
  { S(bucket), T_CMD,
    print_bucket_begin, print_current_bucket_handler, NULL,
    { { N_("NUMBER"), GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print a bucket") },
  { S(current), T_CMD,
    print_current_bucket_begin, print_current_bucket_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print current bucket") },
  { S(dir), T_CMD,
    print_dir_begin, print_dir_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print hash directory") },
  { S(header), T_CMD,
    print_header_begin , print_header_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print database file header") },
  { S(hash), T_CMD,
    NULL, hash_handler, NULL,
    { { N_("KEY"), GDBM_ARG_DATUM, DS_KEY },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("hash value of key") },
  { S(cache), T_CMD,
    print_cache_begin, print_cache_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print the bucket cache") },
  { S(status), T_CMD,
    NULL, status_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print current program status") },
  { S(version), T_CMD,
    NULL, print_version_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print version of gdbm") },
  { S(help), T_CMD,
    help_begin, help_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("print this help list") },
  { S(quit), T_CMD,
    NULL, quit_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("quit the program") },
  { S(set), T_SET,
    NULL, NULL, NULL,
    { { "[VAR=VALUE...]" }, { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("set or list variables") },
  { S(unset), T_UNSET,
    NULL, NULL, NULL,
    { { "VAR..." }, { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("unset variables") },
  { S(define), T_DEF,
    NULL, NULL, NULL,
    { { "key|content", GDBM_ARG_STRING },
      { "{ FIELD-LIST }", GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("define datum structure") },
  { S(source), T_CMD,
    NULL, source_handler, NULL,
    { { "FILE", GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("source command script") },
  { S(close), T_CMD,
    NULL, close_handler, NULL,
    { { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("close the database") },
  { S(open), T_CMD,
    NULL, open_handler, NULL,
    { { "FILE", GDBM_ARG_STRING }, { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("open new database") },
#ifdef WITH_READLINE
  { S(history), T_CMD,
    input_history_begin, input_history_handler, NULL,
    { { N_("[FROM]"), GDBM_ARG_STRING },
      { N_("[COUNT]"), GDBM_ARG_STRING },
      { NULL } },
    FALSE,
    REPEAT_NEVER,
    N_("show input history") },
#endif
  { S(debug), T_CMD,
    NULL, debug_handler, NULL,
    { { NULL } },
    TRUE,
    REPEAT_NEVER,
    N_("query/set debug level") },
#undef S
  { 0 }
};

static int
cmdcmp (const void *a, const void *b)
{
  struct command const *ac = a;
  struct command const *bc = b;
  return strcmp (ac->name, bc->name);
}

void
sort_commands ()
{
  qsort (command_tab, sizeof (command_tab) / sizeof (command_tab[0]) - 1,
	 sizeof (command_tab[0]), cmdcmp);
}

/* Generator function for command completion.  STATE lets us know whether
   to start from scratch; without any state (i.e. STATE == 0), then we
   start at the top of the list. */
char *
command_generator (const char *text, int state)
{
  const char *name;
  static int len;
  static struct command *cmd;

  /* If this is a new word to complete, initialize now.  This includes
     saving the length of TEXT for efficiency, and initializing the index
     variable to 0. */
  if (!state)
    {
      cmd = command_tab;
      len = strlen (text);
    }

  if (!cmd || !cmd->name)
    return NULL;

  /* Return the next name which partially matches from the command list. */
  while ((name = cmd->name))
    {
      cmd++;
      if (strncmp (name, text, len) == 0)
        return strdup (name);
    }

  /* If no names matched, then return NULL. */
  return NULL;
}

/* ? - help handler */
#define CMDCOLS 30

int
help_begin (struct handler_param *param GDBM_ARG_UNUSED, size_t *exp_count)
{
  if (exp_count)
    *exp_count = sizeof (command_tab) / sizeof (command_tab[0]) + 1;
  return 0;
}

void
help_handler (struct handler_param *param)
{
  struct command *cmd;
  FILE *fp = param->fp;
  
  for (cmd = command_tab; cmd->name; cmd++)
    {
      int i;
      int n;

      n = fprintf (fp, " %s", cmd->name);

      for (i = 0; i < NARGS && cmd->args[i].name; i++)
	n += fprintf (fp, " %s", gettext (cmd->args[i].name));

      if (n < CMDCOLS)
	fprintf (fp, "%*.s", CMDCOLS-n, "");
      fprintf (fp, " %s", gettext (cmd->doc));
      fputc ('\n', fp);
    }
}

int
command_lookup (const char *str, struct locus *loc, struct command **pcmd)
{
  enum { fcom_init, fcom_found, fcom_ambig, fcom_abort } state = fcom_init;
  struct command *cmd, *found = NULL;
  size_t len = strlen (str);
  
  for (cmd = command_tab; state != fcom_abort && cmd->name; cmd++)
    {
      if (memcmp (cmd->name, str, len < cmd->len ? len : cmd->len) == 0)
	{
	  switch (state)
	    {
	    case fcom_init:
	      found = cmd;
	      state = fcom_found;
	      break;

	    case fcom_found:
	      if (!interactive ())
		{
		  state = fcom_abort;
		  found = NULL;
		  continue;
		}
	      fprintf (stderr, "ambiguous command: %s\n", str);
	      fprintf (stderr, "    %s\n", found->name);
	      found = NULL;
	      state = fcom_ambig;
	      /* fall through */
	    case fcom_ambig:
	      fprintf (stderr, "    %s\n", cmd->name);
	      break;
	      
	    case fcom_abort:
	      /* should not happen */
	      abort ();
	    }
	}
    }

  if (state == fcom_init)
    lerror (loc, interactive () ? _("Invalid command. Try ? for help.") :
	                          _("Unknown command"));
  if (!found)
    return T_BOGUS;

  *pcmd = found;
  return found->tok;
}

char *parseopt_program_doc = N_("examine and/or modify a GDBM database");
char *parseopt_program_args = N_("DBFILE [COMMAND [ARG ...]]");

enum {
  OPT_LEX_TRACE = 256,
  OPT_GRAM_TRACE
};

struct gdbm_option optab[] = {
  { 'b', "block-size", N_("SIZE"), N_("set block size") },
  { 'c', "cache-size", N_("SIZE"), N_("set cache size") },
  { 'f', "file",       N_("FILE"), N_("read commands from FILE") },
  { 'g', NULL, "FILE", NULL, PARSEOPT_HIDDEN },
  { 'l', "no-lock",    NULL,       N_("disable file locking") },
  { 'm', "no-mmap",    NULL,       N_("do not use mmap") },
  { 'n', "newdb",      NULL,       N_("create database") },
  { 'N', "norc",       NULL,       N_("do not read .gdbmtoolrc file") },
  { 'r', "read-only",  NULL,       N_("open database in read-only mode") },
  { 's', "synchronize", NULL,      N_("synchronize to disk after each write") },
  { 'q', "quiet",      NULL,       N_("don't print initial banner") },
#if GDBMTOOL_DEBUG    
  { OPT_LEX_TRACE, "lex-trace", NULL, N_("enable lexical analyzer traces") },
  { OPT_GRAM_TRACE, "gram-trace", NULL, N_("enable grammar traces") },
#endif  
  { 0 }
};

#define ARGINC 16


struct gdbmarg *
gdbmarg_string (char *string, struct locus *loc)
{
  struct gdbmarg *arg = ecalloc (1, sizeof (*arg));
  arg->next = NULL;
  arg->type = GDBM_ARG_STRING;
  arg->ref = 1;
  if (loc)
    arg->loc = *loc;
  arg->v.string = string;
  return arg;
}

struct gdbmarg *
gdbmarg_datum (datum *dat, struct locus *loc)
{
  struct gdbmarg *arg = ecalloc (1, sizeof (*arg));
  arg->next = NULL;
  arg->type = GDBM_ARG_DATUM;
  arg->ref = 1;
  if (loc)
    arg->loc = *loc;
  arg->v.dat = *dat;
  return arg;
}

struct gdbmarg *
gdbmarg_kvpair (struct kvpair *kvp, struct locus *loc)
{
  struct gdbmarg *arg = ecalloc (1, sizeof (*arg));
  arg->next = NULL;
  arg->type = GDBM_ARG_KVPAIR;
  arg->ref = 1;
  if (loc)
    arg->loc = *loc;
  arg->v.kvpair = kvp;
  return arg;
}

struct slist *
slist_new_s (char *s)
{
  struct slist *lp = emalloc (sizeof (*lp));
  lp->next = NULL;
  lp->str = s;
  return lp;
}

struct slist *
slist_new (char const *s)
{
  return slist_new_s (estrdup (s));
}

struct slist *
slist_new_l (char const *s, size_t l)
{
  char *copy = emalloc (l + 1);
  memcpy (copy, s, l);
  copy[l] = 0;
  return slist_new_s (copy);
}

void
slist_free (struct slist *lp)
{
  while (lp)
    {
      struct slist *next = lp->next;
      free (lp->str);
      free (lp);
      lp = next;
    }
}

void
slist_insert (struct slist **where, struct slist *what)
{
  if (*where)
    {
      while (what->next)
	what = what->next;
      what->next = (*where)->next;
      (*where)->next = what;
    }
  else
    what->next = NULL;
  *where = what;
}

struct kvpair *
kvpair_string (struct locus *loc, char *val)
{
  struct kvpair *p = ecalloc (1, sizeof (*p));
  p->type = KV_STRING;
  if (loc)
    p->loc = *loc;
  p->val.s = val;
  return p;
}

struct kvpair *
kvpair_list (struct locus *loc, struct slist *s)
{
  struct kvpair *p = ecalloc (1, sizeof (*p));
  p->type = KV_LIST;
  if (loc)
    p->loc = *loc;
  p->val.l = s;
  return p;
}  

static void
kvlist_free (struct kvpair *kvp)
{
  while (kvp)
    {
      struct kvpair *next = kvp->next;
      free (kvp->key);
      switch (kvp->type)
	{
	case KV_STRING:
	  free (kvp->val.s);
	  break;

	case KV_LIST:
	  slist_free (kvp->val.l);
	  break;
	}
      free (kvp);
      kvp = next;
    }
}

int
gdbmarg_free (struct gdbmarg *arg)
{
  if (arg && --arg->ref == 0)
    {
      switch (arg->type)
	{
	case GDBM_ARG_STRING:
	  free (arg->v.string);
	  break;

	case GDBM_ARG_KVPAIR:
	  kvlist_free (arg->v.kvpair);
	  break;

	case GDBM_ARG_DATUM:
	  free (arg->v.dat.dptr);
	  break;
	}
      free (arg);
      return 0;
    }
  return 1;
}

void
gdbmarg_destroy (struct gdbmarg **parg)
{
  if (parg && gdbmarg_free (*parg))
    *parg = NULL;
}

void
gdbmarglist_init (struct gdbmarglist *lst, struct gdbmarg *arg)
{
  if (arg)
    arg->next = NULL;
  lst->head = lst->tail = arg;
}

void
gdbmarglist_add (struct gdbmarglist *lst, struct gdbmarg *arg)
{
  arg->next = NULL;
  if (lst->tail)
    lst->tail->next = arg;
  else
    lst->head = arg;
  lst->tail = arg;
}

void
gdbmarglist_free (struct gdbmarglist *lst)
{
  struct gdbmarg *arg;

  for (arg = lst->head; arg; )
    {
      struct gdbmarg *next = arg->next;
      gdbmarg_free (arg);
      arg = next;
    }
  lst->head = lst->tail = NULL;
}

struct handler_param param;
size_t argmax;

void
param_free_argv (struct handler_param *param, int n)
{
  int i;

  for (i = 0; i < n; i++)
    gdbmarg_destroy (&param->argv[i]);
  param->argc = 0;
}

typedef struct gdbmarg *(*coerce_type_t) (struct gdbmarg *arg,
					  struct argdef *def);

struct gdbmarg *
coerce_ref (struct gdbmarg *arg, struct argdef *def)
{
  ++arg->ref;
  return arg;
}

struct gdbmarg *
coerce_k2d (struct gdbmarg *arg, struct argdef *def)
{
  datum d;
  
  if (datum_scan (&d, dsdef[def->ds], arg->v.kvpair))
    return NULL;
  return gdbmarg_datum (&d, &arg->loc);
}

struct gdbmarg *
coerce_s2d (struct gdbmarg *arg, struct argdef *def)
{
  datum d;
  struct kvpair kvp;

  memset (&kvp, 0, sizeof (kvp));
  kvp.type = KV_STRING;
  kvp.val.s = arg->v.string;
  
  if (datum_scan (&d, dsdef[def->ds], &kvp))
    return NULL;
  return gdbmarg_datum (&d, &arg->loc);
}

#define coerce_fail NULL

coerce_type_t coerce_tab[GDBM_ARG_MAX][GDBM_ARG_MAX] = {
  /*             s            d            k */
  /* s */  { coerce_ref,  coerce_fail, coerce_fail },
  /* d */  { coerce_s2d,  coerce_ref,  coerce_k2d }, 
  /* k */  { coerce_fail, coerce_fail, coerce_ref }
};

char *argtypestr[] = { "string", "datum", "k/v pair" };
  
struct gdbmarg *
coerce (struct gdbmarg *arg, struct argdef *def)
{
  if (!coerce_tab[def->type][arg->type])
    {
      lerror (&arg->loc, _("cannot coerce %s to %s"),
		    argtypestr[arg->type], argtypestr[def->type]);
      return NULL;
    }
  return coerce_tab[def->type][arg->type] (arg, def);
}

static struct command *last_cmd;
static struct gdbmarglist last_args;

void
run_last_command (void)
{
  if (interactive ())
    {
      if (last_cmd)
	{
	  switch (last_cmd->repeat)
	    {
	    case REPEAT_NEVER:
	      break;
	    case REPEAT_NOARG:
	      gdbmarglist_free (&last_args);
	      /* FALLTHROUGH */
	    case REPEAT_ALWAYS:
	      if (run_command (last_cmd, &last_args))
		exit (EXIT_USAGE);
	      break;
	    default:
	      abort ();
	    }
	}
    }
}

int
run_command (struct command *cmd, struct gdbmarglist *arglist)
{
  int i;
  struct gdbmarg *arg;
  char *pager = NULL;
  char argbuf[128];
  size_t expected_lines, *expected_lines_ptr;
  FILE *pagfp = NULL;

  variable_get ("pager", VART_STRING, (void**) &pager);
  
  arg = arglist ? arglist->head : NULL;

  for (i = 0; cmd->args[i].name && arg; i++, arg = arg->next)
    {
      if (i >= argmax)
	{
	  argmax += ARGINC;
	  param.argv = erealloc (param.argv,
				 sizeof (param.argv[0]) * argmax);
	}
      if ((param.argv[i] = coerce (arg, &cmd->args[i])) == NULL)
	{
	  param_free_argv (&param, i);
	  return 1;
	}
    }

  for (; cmd->args[i].name; i++)
    {
      char *argname = cmd->args[i].name;
      struct gdbmarg *t;
      
      if (*argname == '[')
	/* Optional argument */
	break;

      if (!interactive ())
	{
	  terror (_("%s: not enough arguments"), cmd->name);
	  return 1;
	}
      printf ("%s? ", argname);
      fflush (stdout);
      if (fgets (argbuf, sizeof argbuf, stdin) == NULL)
	{
	  terror (_("unexpected eof"));
	  exit (EXIT_USAGE);
	}

      trimnl (argbuf);
      if (i >= argmax)
	{
	  argmax += ARGINC;
	  param.argv = erealloc (param.argv,
				 sizeof (param.argv[0]) * argmax);
	}

      t = gdbmarg_string (estrdup (argbuf), &yylloc);
      if ((param.argv[i] = coerce (t, &cmd->args[i])) == NULL)
	{
	  gdbmarg_free (t);
	  param_free_argv (&param, i);
	  return 1;
	}
    }

  if (arg && !cmd->variadic)
    {
      terror (_("%s: too many arguments"), cmd->name);
      return 1;
    }

  /* Prepare for calling the handler */
  param.argc = i;
  if (!param.argv)
    {
      argmax = ARGINC;
      param.argv = ecalloc (argmax, sizeof (param.argv[0]));
    }
  param.argv[i] = NULL;
  param.vararg = arg;
  param.fp = NULL;
  param.data = NULL;
  pagfp = NULL;
      
  expected_lines = 0;
  expected_lines_ptr = (interactive () && pager) ? &expected_lines : NULL;
  if (!(cmd->begin && cmd->begin (&param, expected_lines_ptr)))
    {
      if (pager && expected_lines > get_screen_lines ())
	{
	  pagfp = popen (pager, "w");
	  if (pagfp)
	    param.fp = pagfp;
	  else
	    {
	      terror (_("cannot run pager `%s': %s"), pager,
			    strerror (errno));
	      pager = NULL;
	      param.fp = stdout;
	    }	  
	}
      else
	param.fp = stdout;
  
      cmd->handler (&param);
      if (cmd->end)
	cmd->end (param.data);
      else if (param.data)
	free (param.data);

      if (pagfp)
	pclose (pagfp);
    }

  param_free_argv (&param, param.argc);

  last_cmd = cmd;
  if (arglist->head != last_args.head)
    {
      gdbmarglist_free (&last_args);
      last_args = *arglist;
    }
  
  return 0;
}

static void
source_rcfile (void)
{
  instream_t istr = NULL;
  
  if (access (GDBMTOOLRC, R_OK) == 0)
    {
      istr = instream_file_create (GDBMTOOLRC);
    }
  else
    {
      char *fname;
      char *p = getenv ("HOME");
      if (!p)
	{
	  struct passwd *pw = getpwuid (getuid ());
	  if (!pw)
	    {
	      terror (_("cannot find home directory"));
	      return;
	    }
	  p = pw->pw_dir;
	}
      fname = mkfilename (p, GDBMTOOLRC, NULL);
      if (access (fname, R_OK) == 0)
	{
	  istr = instream_file_create (GDBMTOOLRC);
	}
      free (fname);
    }

  if (istr)
    {
      if (input_context_push (istr))
	exit (EXIT_FATAL);
      yyparse ();
    }
}

#if GDBM_DEBUG_ENABLE
void
debug_printer (char const *fmt, ...)
{
  va_list ap;

  va_start (ap, fmt);
  vfprintf (stderr, fmt, ap);
  va_end (ap);
}
#endif

int
main (int argc, char *argv[])
{
  int opt;
  int bv;
  int norc = 0;
  int res;
  char *source = NULL;
  instream_t input = NULL;
  
  set_progname (argv[0]);
#if GDBM_DEBUG_ENABLE
  gdbm_debug_printer = debug_printer;
#endif

#ifdef HAVE_SETLOCALE
  setlocale (LC_ALL, "");
#endif
  bindtextdomain (PACKAGE, LOCALEDIR);
  textdomain (PACKAGE);

  sort_commands ();
  
  /* Initialize variables. */
  dsdef[DS_KEY] = dsegm_new_field (datadef_lookup ("string"), NULL, 1);
  dsdef[DS_CONTENT] = dsegm_new_field (datadef_lookup ("string"), NULL, 1);

  variable_set ("open", VART_STRING, "wrcreat");
  variable_set ("pager", VART_STRING, getenv ("PAGER"));
  
  input_init ();
  lex_trace (0);
  
  for (opt = parseopt_first (argc, argv, optab);
       opt != EOF;
       opt = parseopt_next ())
    switch (opt)
      {
      case 'f':
	source = optarg;
	break;
	
      case 'l':
	bv = 0;
	variable_set ("lock", VART_BOOL, &bv);
	break;

      case 'm':
	bv = 0;
	variable_set ("mmap", VART_BOOL, &bv);
	break;

      case 's':
	bv = 1;
	variable_set ("sync", VART_BOOL, &bv);
	break;
	
      case 'r':
	variable_set ("open", VART_STRING, "readonly");
	break;
	
      case 'n':
	variable_set ("open", VART_STRING, "newdb");
	break;

      case 'N':
	norc = 1;
	break;
	
      case 'c':
	variable_set ("cachesize", VART_STRING, optarg);
	break;
	
      case 'b':
	variable_set ("blocksize", VART_STRING, optarg);
	break;
	
      case 'g':
	file_name = estrdup (optarg);
	break;

      case 'q':
	bv = 1;
	variable_set ("quiet", VART_BOOL, &bv);
	break;

      case OPT_LEX_TRACE:
	lex_trace (1);
	break;

      case OPT_GRAM_TRACE:
	gram_trace (1);
	break;
	
      default:
	terror (_("unknown option; try `%s -h' for more info"),
		progname);
	exit (EXIT_USAGE);
      }
  
  argc -= optind;
  argv += optind;

  if (source && strcmp (source, "-"))
    {
      input = instream_file_create (source);
      if (!input)
	exit (1);
    }
  
  if (argc >= 1)
    {
      file_name = estrdup (argv[0]);
      argc--;
      argv++;
      if (argc)
	{
	  if (input)
	    {
	      terror (_("--file and command cannot be used together"));
	      exit (EXIT_USAGE);
	    }
	  input = instream_argv_create (argc, argv);
	  if (!input)
	    exit (1);
	}
    }

  signal (SIGPIPE, SIG_IGN);

  memset (&param, 0, sizeof (param));
  argmax = 0;

  if (!norc)
    source_rcfile ();

  if (!input)
    input = instream_stdin_create ();
  
  /* Welcome message. */
  if (instream_interactive (input) && !variable_is_true ("quiet"))
    printf (_("\nWelcome to the gdbm tool.  Type ? for help.\n\n"));

  if (input_context_push (input))
    exit (EXIT_FATAL);
  res = yyparse ();
  closedb ();
  input_done ();
  return res;
}
