/* Copyright (C) 1989-2018 Free Software Foundation, Inc.
     Written by James Clark (jjc@jclark.com)

This file is part of groff.

groff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or
(at your option) any later version.

groff is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "ptable.h"
#include "errarg.h"
#include "error.h"

unsigned long hash_string(const char *s)
{
  // This is the mythical Aho-Hopcroft-Ullman hash function.
  // TODO: Improve.  See http://www.haible.de/bruno/hashfunc.html
  assert(s != 0);
  unsigned long h = 0, g;
  while (*s != 0) {
    h <<= 4;
    h += *s++;
    if ((g = h & 0xf0000000) != 0) {
      h ^= g >> 24;
      h ^= g;
    }
  }
  return h;
}

static const unsigned table_sizes[] = { 
  101, 503, 1009, 2003, 3001, 4001, 5003, 10007, 20011, 40009,
  80021, 160001, 500009, 1000003, 2000003, 4000037, 8000009,
  16000057, 32000011, 64000031, 128000003, 0 
};

unsigned next_ptable_size(unsigned n)
{
  const unsigned *p;  
  for (p = table_sizes; *p <= n; p++)
    if (*p == 0)
      fatal("cannot expand table");
  return *p;
}

// end of ptable.cpp
