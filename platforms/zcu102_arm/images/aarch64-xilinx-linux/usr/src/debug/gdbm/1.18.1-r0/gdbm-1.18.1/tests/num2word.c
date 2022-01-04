/* This file is part of GDBM test suite.
   Copyright (C) 2011, 2017-2018 Free Software Foundation, Inc.

   GDBM is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   GDBM is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with GDBM. If not, see <http://www.gnu.org/licenses/>.
*/
#include "autoconf.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

const char *progname;

const char *nstr[][10] = {
  { "zero", 
    "one",
    "two",
    "three",
    "four",
    "five",
    "six",
    "seven",
    "eight",
    "nine"
  },
  { "ten",
    "eleven",
    "twelve",
    "thirteen",
    "fourteen",  
    "fifteen",   
    "sixteen",   
    "seventeen", 
    "eighteen",  
    "nineteen"
  },
  { NULL,
    NULL,
    "twenty",
    "thirty",
    "fourty",
    "fifty",
    "sixty",
    "seventy",
    "eighty",
    "ninety"
  }
};

const char *short_scale[] = {
  "one",
  "thousand",
  "million",
  "billion"
  /* End of range for 32-bit unsigned long */
};
size_t short_scale_max = sizeof (short_scale) / sizeof (short_scale[0]);

typedef unsigned long numeral_t;

char buffer[1024];
size_t bufsize = sizeof(buffer);
size_t bufoff;
int delim = 0;
int revert_option = 0;
int random_option = 0;

void
copy (const char *str, int dch)
{
  size_t len = strlen (str);
  if (len + !!dch > bufoff)
    abort ();
  if (dch)
    buffer[--bufoff] = dch;
  bufoff -= len;
  memcpy (buffer + bufoff, str, len);
  delim = ' ';
}

void
format_100 (numeral_t num)
{
  if (num == 0)
    ;
  else if (num < 10)
    copy (nstr[0][num], delim);
  else if (num < 20)
    copy (nstr[1][num-10], delim);
  else 
    {
      numeral_t tens = num / 10;
      num %= 10;
      if (num)
	{
	  copy (nstr[0][num], delim);
	  copy ("-", 0);
	  copy (nstr[2][tens], 0);
	}
      else
	copy (nstr[2][tens], delim);
    }
}

void
format_1000 (numeral_t num, int more)
{
  size_t n = num % 100;
  num /= 100;
  format_100 (n);
  more |= num != 0;
  if (n && more)
    copy ("and", delim);
  if (num)
    {
      copy ("hundred", delim);
      copy (nstr[0][num], delim);
    }
}


void
format_number (numeral_t num)
{
  int s = 0;
  size_t off;

  bufoff = bufsize;
  buffer[--bufoff] = 0;
  off = bufoff;
  delim = 0;
  
  do
    {
      numeral_t n = num % 1000;

      num /= 1000;
      
      if (s > 0 && ((n && off > bufoff) || num == 0))
	copy (short_scale[s], delim);
      s++;
      
      if (s > short_scale_max)
	abort ();
      
      format_1000 (n, num != 0);
    }
  while (num);
  
  if (bufoff + 1 == bufsize)
    copy (nstr[0][0], 0);
}
      

void
print_number (numeral_t num)
{
  format_number (num);
  if (revert_option)
    printf ("%s\t%lu\n", buffer + bufoff, num);
  else
    printf ("%lu\t%s\n", num, buffer + bufoff);
}

void
print_range (numeral_t num, numeral_t to)
{
  for (; num <= to; num++)
    print_number (num);
}

numeral_t
xstrtoul (char *arg, char **endp)
{
  numeral_t num;
  char *p;

  errno = 0;
  num = strtoul (arg, &p, 10);
  if (errno)
    {
      fprintf (stderr, "%s: invalid number: ", progname);
      perror (arg);
      exit (2);
    }
  if (endp)
    *endp = p;
  else if (*p)
    {
      fprintf (stderr, "%s: invalid number (near %s)\n",
	       progname, p);
      exit (2);
    }
    
  return num;
}

struct numrange      /* Range of numbers */
{
  numeral_t start;   /* Starting number */
  numeral_t count;   /* Count of numbers in the range */
};

struct numrange *range;  /* Array of ranges */
size_t range_max;        /* Number of elements in RANGE */
size_t range_total;      /* Total count of numbers (sum of range[i].count) */

#define RANGE_INITIAL_ALLOC 16 /* Initial allocation for the range (entries) */

/* List of available entries.

   Unallocated entries in RANGE form a singly-linked list. Each unallocated
   entry has COUNT set to 0, and START set to the index of the next
   unallocated entry, or AVAIL_NIL, if that's the last free entry.
 */
#define AVAIL_NIL ((size_t)-1)  /* Marks end of the list */
size_t range_avail = AVAIL_NIL; /* Index of the first available entry */

/* Return entry I to the list of available ones */
static void
range_remove (size_t i)
{
  range[i].count = 0;
  range[i].start = range_avail;
  range_avail = i;
}  

/* Add interval [start, start + count) */
static void
range_add (numeral_t start, numeral_t count)
{
  size_t i;
  
  if (range_avail == AVAIL_NIL)
    {
      size_t n = range_max;
      
      if (range_max == 0)
	range_max = RANGE_INITIAL_ALLOC;
      else
	{
	  assert ((size_t)-1 / 3 * 2 / sizeof (range[0]) > range_max);
	  range_max += (range_max + 1) / 2;
	}
      
      range = realloc (range, range_max * sizeof (range[0]));
      if (!range)
	abort ();

      for (i = range_max; i > n; i--)
	range_remove (i - 1);
    }

  i = range_avail;
  range_avail = range[i].start;

  range[i].start = start;
  range[i].count = count;
  range_total += count;
}

/* Treating RANGE as a contiguous array of numbers, return the number in
   position IDX and remove it from the array. */
numeral_t
range_get (size_t idx)
{
  numeral_t n;
  size_t i;

  for (i = 0; i < range_max; i++)
    {
      if (idx < range[i].count)
	break;
      idx -= range[i].count;
    }
  assert (i < range_max);
  n = range[i].start + idx;
  if (range[i].count == 1)
    range_remove (i);
  else if (idx == 0)
    {
      range[i].start++;
      range[i].count--;
    }
  else
    {
      /* Split range */
      if (range[i].count > idx - 1)
	{
	  range_total -= range[i].count - idx - 1;
	  range_add (range[i].start + idx + 1, range[i].count - idx - 1);
	}
      range[i].count = idx;
    }
  range_total--;
  return n;
}

void
usage (FILE *fp)
{
  fprintf (fp, "usage: %s [-revert] [-random] RANGE [RANGE...]\n", progname);
  fprintf (fp, "Lists english numerals in given ranges\n\n");
  fprintf (fp, "Each RANGE is one of:\n\n");
  fprintf (fp, "  X:N        N numerals starting from X; interval [X,X+N]\n");
  fprintf (fp, "  X-Y        numerals from X to Y; interval [X,Y]\n\n");
  fprintf (fp, "Options are:\n\n");
  fprintf (fp, "  -random    produce list in random order\n");
  fprintf (fp, "  -revert    revert output columns (numeral first)\n");
  fprintf (fp, "  -help      display this help summary\n");
  fprintf (fp, "\n");
}  

int
main (int argc, char **argv)
{
  progname = *argv++;
  --argc;

  for (; argc; argc--, argv++)
    {
      char *arg = *argv;
      if (strcmp (arg, "-h") == 0 || strcmp (arg, "-help") == 0)
	{
	  usage (stdout);
	  exit (0);
	}
      else if (strcmp (arg, "-revert") == 0)
	{
	  revert_option = 1;
	}
      else if (strcmp (arg, "-random") == 0)
	{
	  random_option = 1;
	}
      else if (arg[0] == '-')
	{
	  fprintf (stderr, "%s: unrecognized option: %s\n", progname, arg);
	  return 1;
	}
      else
	break;
    }
  
  for (; argc; argc--, argv++)
    {
      char *arg = *argv;
      numeral_t num, num2;
      char *p;

      num = xstrtoul (arg, &p);
      if (*p == 0)
	range_add (num, 1);
      else if (*p == ':')
	{
	  *p++ = 0;
	  num2 = xstrtoul (p, NULL);
	  if (num2 == 0)
	    {
	      fprintf (stderr, "%s: invalid count\n", progname);
	      exit (2);
	    }
	  range_add (num, num2);
	}
      else if (*p == '-')
	{
	  *p++ = 0;
	  num2 = xstrtoul (p, NULL);
	  if (num2 < num)
	    {
	      fprintf (stderr, "%s: invalid range: %lu-%lu\n",
		       progname, num, num2);
	      exit (2);
	    }	    
	  range_add (num, num2 - num + 1);
	}
      else
	{
	  fprintf (stderr, "%s: invalid argument\n", progname);
	  exit (2);
	}
    }

  if (range_total == 0)
    {
      usage (stderr);
      exit (1);
    }
  
  if (random_option)
    {
      srandom (time (NULL));
      while (range_total)
	{
	  numeral_t n = range_get (random () % range_total);
	  print_number (n);
	}
    }
  else
    {
      size_t i;
      for (i = 0; i < range_max; i++)
	if (range[i].count)
	  print_range (range[i].start, range[i].start + range[i].count - 1);
    }
  exit (0);
}
