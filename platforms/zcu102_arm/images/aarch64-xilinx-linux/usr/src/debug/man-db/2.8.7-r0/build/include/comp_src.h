/* include/comp_src.h.  Generated from comp_src.h.in by configure. 
 *
 * comp_src.h: structure used by decompress.c 
 *  
 * Copyright (C) 1994 Graeme W. Wilford. (Wilf.)
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Sat Oct 29 13:09:31 GMT 1994  Wilf. (G.Wilford@ee.surrey.ac.uk) 
 */

/*--------------------------------------------------------------------------*/
/* This is where we define the decompressors used to decompress any nroff   */
/* source that we find. All cat pages are compressed with either gzip (if   */
/* available) or compress. This is not the place to define _the_ cat page   */
/* decompressor - see ./manconfig.h for that.                               */
/*                                                                          */
/* To add a decompressor all you need to know is its name (preferrably its  */
/* location), and the unique extension that it gives to files compressed    */
/* with it. Here is an example. You have a compressor named foobar and      */
/* compressed files have an extension of .fb . It is located in /usr/bin    */
/* and requires a -d option to be used as a decompressor. Add the following */
/* line to the structure below.                                             */
/*                                                                          */
/* {"/usr/bin/foobar -d", "fb", NULL},                                      */
/*--------------------------------------------------------------------------*/

#ifndef MAN_COMP_SRC_H
#define MAN_COMP_SRC_H

#ifdef COMP_SRC

struct compression comp_list[] = {

/* If we have gzip, incorporate the following */
#ifdef HAVE_GZIP
	{GUNZIP, "gz", NULL},
	{GUNZIP, "z", NULL},
#endif /* HAVE_GZIP */

/* If we have compress, incorporate the following */
#ifdef HAVE_COMPRESS
	{UNCOMPRESS, "Z", NULL},
/* Else if we have gzip, incorporate the following */
#elif defined (HAVE_GZIP)
	{GUNZIP, "Z", NULL},
#endif /* HAVE_COMPRESS || HAVE_GZIP */

/* If we have bzip2, incorporate the following */
#ifdef HAVE_BZIP2
	{BUNZIP2, "bz2", NULL},
#endif /* HAVE_BZIP2 */

/* If we have xz, incorporate the following */
#ifdef HAVE_XZ
	{UNXZ, "xz", NULL},
	{UNXZ, "lzma", NULL},
/* Else if we have lzma, incorporate the following */
#elif defined (HAVE_LZMA)
	{UNLZMA, "lzma", NULL},
#endif /* HAVE_XZ || HAVE_LZMA */

/* If we have lzip, incorporate the following */
#ifdef HAVE_LZIP
	{UNLZIP, "lz", NULL},
#endif /* HAVE_LZIP */

/*------------------------------------------------------*/
/* Add your decompressor(s) and extension(s) below here */
/*------------------------------------------------------*/



/*----------------*/
/* and above here */
/*----------------*/

/* ... and the last structure is */
	{NULL, NULL, NULL}
};

#endif /* COMP_SRC */

#endif /* MAN_COMP_SRC_H */
