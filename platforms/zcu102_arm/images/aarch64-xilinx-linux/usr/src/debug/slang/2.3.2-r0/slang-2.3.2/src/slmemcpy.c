/*
Copyright (C) 2004-2017,2018 John E. Davis

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

/* These routines are fast memcpy, memset routines.  When available, I
   use system rouines.  For msdos, I use inline assembly. */

/* The current versions only work in the forward direction only!! */

#include "slinclud.h"

#include "slang.h"
#include "_slang.h"

char *SLmemcpy(char *s1, char *s2, int n)
{
#if defined(__BORLANDC__) && defined(__MSDOS__)
   asm mov ax, ds
   asm mov bx, si
   asm mov dx, di
   asm mov cx, n
   asm les di, s1
   asm lds si, s2
   asm cld
   asm rep movsb
   asm mov ds, ax
   asm mov si, bx
   asm mov di, dx
   return(s1);

#else
   register char *smax, *s = s1;
   int n2;

   n2 = n % 4;
   smax = s + (n - 4);
   while (s <= smax)
     {
	*s = *s2; *(s + 1) = *(s2 + 1); *(s + 2) = *(s2 + 2); *(s + 3) = *(s2 + 3);
	s += 4;
	s2 += 4;
     }
   while (n2--) *s++ = *s2++;
   return(s1);
#endif
}
