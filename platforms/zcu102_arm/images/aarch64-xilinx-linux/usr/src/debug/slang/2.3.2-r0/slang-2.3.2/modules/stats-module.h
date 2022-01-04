#ifndef SLSTATS_MODULE_H_
# define SLSTATS_MODULE_H_ 1
/*
Copyright (C) 2017,2018 John E. Davis

This file is part of the S-Lang Library.

The S-Lang Library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The S-Lang Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/

/* It is assumed that the array a has already been sorted in ascending or and that b
 * has been rearranged accordingly
 */
extern double _pSLstats_kendall_tau (SLindex_Type *a, SLindex_Type *b, size_t n, double *taup);

#endif				       /* SLSTATS_MODULE_H_ */
