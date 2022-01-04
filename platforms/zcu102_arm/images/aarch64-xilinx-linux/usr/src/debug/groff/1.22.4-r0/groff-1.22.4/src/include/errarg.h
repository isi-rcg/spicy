// -*- C++ -*-
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

class errarg {
  enum { EMPTY, STRING, CHAR, INTEGER, UNSIGNED_INTEGER, DOUBLE } type;
  union {
    const char *s;
    int n;
    unsigned int u;
    char c;
    double d;
  };
 public:
  errarg();
  errarg(const char *);
  errarg(char);
  errarg(unsigned char);
  errarg(int);
  errarg(unsigned int);
  errarg(double);
  int empty() const;
  void print() const;
};

extern errarg empty_errarg;

extern void errprint(const char *,
		     const errarg &arg1 = empty_errarg,
		     const errarg &arg2 = empty_errarg,
		     const errarg &arg3 = empty_errarg);
