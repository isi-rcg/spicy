/* include/manconfig.h.  Generated from manconfig.h.in by configure.
 *
 * manconfig.h.in: definitions and declarations used throughout man-db
 *
 * Copyright (C) 1994, 1995 Graeme W. Wilford. (Wilf.)
 * Copyright (C) 2001, 2002 Colin Watson.
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
 */

/*--------------------------------------------------------------------------*/
/* This file contains the paths/binary locations of the programs used by    */
/* these utilities and various C pre-processor definitions that modify the  */
/* behaviour of the man-db suite. You may like to check that all of the     */
/* formatters are from the same package. Ie, that we are not using a native */
/* UNIX nroff with GNU tbl.                                                 */
/*--------------------------------------------------------------------------*/

#ifndef MANCONFIG_H
#define MANCONFIG_H

#include <stdbool.h>

#include "xvasprintf.h"

/* STD_SECTIONS must contain all of your man hierarchy subdirectories. The
   order is important. Manual pages will be displayed in this order. Ie
   if "1" comes before "2", then a kill(1) will be displayed in preference to
   kill(2), or if `man --all kill', it will be displayed first. Section names 
   may be multi-character, but this is non-standard and probably best 
   avoided. */

#define STD_SECTIONS { \
	"1", "n", "l", "8", "3", "0", "2", "5", "4", "9", "6", "7", NULL \
}

/* Some system's man pages require default pre-processing with perhaps tbl
   or other formatters, DEFAULT_MANROFFSEQ can compensate by adding a list of
   default pre-processors using the standard syntax of first letter.
   ie "t"  = tbl (the table pre-processor)
      "te" = tbl, eqn (both the table and equation pre-processors)
   DEFAULT_MANROFFSEQ can be overridden by command line arguments to man, the
   environment variable $MANROFFSEQ, and by the manual page being formatted. */

#if defined (__hpux) || (defined (__alpha) && !defined(__GLIBC__))
#  define DEFAULT_MANROFFSEQ	"te"
#elif defined (__ultrix)
#  define DEFAULT_MANROFFSEQ	"t"
#endif

/* the magic cookie to request preprocessing */
#define PP_COOKIE "'\\\" "

/* uncomment the following line if manual pages require tbl by default */
#ifndef DEFAULT_MANROFFSEQ   
#define DEFAULT_MANROFFSEQ   "t"
#endif

/* By default, man-db will store a whatis referenced manual page in favour
   of a stray cat page when they both share identical namespace. If you
   would like to see a stray cat eg. kill(1) in favour of a kill(1) whatis
   referenced to bash_builtins(1), uncomment the following line. */

/* #define FAVOUR_STRAYCATS */

/* CATMODE is the mode of the formatted cat pages that we create. The man-db
   package can be run in 4 main modes, 3 of which are relatively secure and
   allow the cats to be non world writable. The `wide open' mode requires
   CATMODE to be 0666. Edit if necessary, after reading the man-db manual */ 

#define CATMODE		0644 /* u=rw,go=r */

/* DBMODE is the mode of the created databases. As with CATMODE, secure 
   operation requires that the db's don't have world write access. In the 
   unlikely event that this is required, change to 0666. 
     For increased speed, at the cost of buffer cache, set the sticky bit
   on databases so that they remain memory resident. To do this, OR the 
   required mode with 1000 and prepend a 0, eg 01644 */
   
#define DBMODE		0644 /* u=rw,go=r */

/* The name of the databases. DB_EXT depends on the database type in use */
#define MAN_DB		"/index" DB_EXT
#define mkdbname(path)	xasprintf ("%s%s", path, MAN_DB)

/* The locations of the following files were determined by ../configure so
   some of them may be incorrect. Edit as necessary */

#ifndef PAGER
#  define PAGER		"less"
#endif

#ifndef CAT
#  define CAT		"cat"
#endif

#ifndef WEB_BROWSER
#  define WEB_BROWSER	""
#endif

#ifndef TR
#  define TR		"tr"
#endif

#ifndef GREP
#  define GREP		"grep"
#endif

#ifdef HAS_TROFF
#  ifndef TROFF
#    define TROFF 	"groff -mandoc"
#  endif
#endif

#ifndef NROFF_MISSING
#  ifndef NROFF
#    define NROFF 	"nroff -mandoc"
#  endif
#endif

#ifndef EQN
#  define EQN 		"eqn"
#endif

#ifndef NEQN
#  define NEQN		"neqn"
#endif

#ifndef TBL
#  define TBL 		"tbl"
#endif

#ifndef COL
#  define COL 		""
#endif

#ifndef VGRIND
#  define VGRIND 	""
#endif

#ifndef REFER
#  define REFER 	"refer"
#endif

#ifndef GRAP
#  define GRAP 		""
#endif

#ifndef PIC
#  define PIC 		"pic -S"
#endif

#ifndef OVERRIDE_DIR
#  define OVERRIDE_DIR	""
#endif

/*------------------------------------------------------------------*/
/* The following definitions are best left alone by the uninitiated */
/*------------------------------------------------------------------*/

/* GNU grep flags (i)gnore case
		  (E)xtended regex
		  (w)hole word matches only */

#ifndef WHATIS_GREP_FLAGS
#  define WHATIS_GREP_FLAGS		"-i"
#endif

#ifndef APROPOS_GREP_FLAGS
#  define APROPOS_GREP_FLAGS		"-iEw"
#endif

#ifndef APROPOS_REGEX_GREP_FLAGS
#  define APROPOS_REGEX_GREP_FLAGS	"-iE"
#endif

/* GNU less flags (i)gnore case on search 
 *                (x8) set tab stops to 8 spaces
 *                (R)aw control chars (but keep track of screen appearance)
 *                (m)ore display style
 * 
 * If you change this, be sure to match the format with
 * man.c:make_display_command().
 */

#define LESS_OPTS	"-ix8RmPm%s$PM%s$"

/* This is a minimal latin1 special characters to ascii translation table */
#if !defined(TR_SET1) || !defined(TR_SET2)
#  define TR_SET1	" \'\\255\\267\\264\\327\'"
#  define TR_SET2	" \'\\055\\157\\047\\170\'"
#endif

#ifdef COMP_CAT
/* This is the default compressor and compressed extension.
   These are used for compressing cat pages. The compressor is likely to
   be gzip or compress and the extension: .gz or .Z . Please make sure that
   all of your cat pages have the same extension (whatever that may be) */

#  define COMPRESSOR "gzip -c7"
#  define COMPRESS_EXT "gz"
#endif /* COMP_CAT */

#ifdef COMP_SRC
/* These are the currently supported decompressors. They are used for 
   decompressing cat pages and source nroff. To add further decompressors,
   you will need to edit comp_src.h[.in] . Help is provided in the file */

#  define GUNZIP "gzip -dc"
#  define UNCOMPRESS ""
#  define BUNZIP2 "bzip2 -dc"
#  define UNLZMA ""
#  define UNXZ "xz -dc"
#  define UNLZIP ""
#endif /* COMP_SRC */

/*-----------------------------------------------------------------------*/
/* The things below here shouldn't really be changed unless you really	 */
/* know what you are doing.						 */
/*-----------------------------------------------------------------------*/

/* my gcc specs file is hacked to define __profile__ if I compile with
   the -p or -pg flag, to do this manually (needed if you want to know where
   gmon.out ended up), uncomment the following line */
/* #define __profile__ */

/* GCC version checking borrowed from glibc. */
#if defined(__GNUC__) && defined(__GNUC_MINOR__)
#  define GNUC_PREREQ(maj,min) \
	((__GNUC__ << 16) + __GNUC_MINOR__ >= ((maj) << 16) + (min))
#else
#  define GNUC_PREREQ(maj,min) 0
#endif

/* Does this compiler support format string checking? */
#if GNUC_PREREQ(2,0)
#  define ATTRIBUTE_FORMAT_PRINTF(a,b) \
	__attribute__ ((__format__ (__printf__, a, b)))
#else
#  define ATTRIBUTE_FORMAT_PRINTF(a,b)
#endif

/* Does this compiler support unused result checking? */
#if GNUC_PREREQ(3,4)
#  define ATTRIBUTE_WARN_UNUSED_RESULT __attribute__ ((__warn_unused_result__))
#else
#  define ATTRIBUTE_WARN_UNUSED_RESULT
#endif

/* Does this compiler support sentinel checking? */
#if GNUC_PREREQ(4,0)
#  define ATTRIBUTE_SENTINEL __attribute__ ((__sentinel__))
#else
#  define ATTRIBUTE_SENTINEL
#endif

/* If running checker, support the garbage detector, else don't */
#ifdef __CHECKER__
extern void __chkr_garbage_detector (void);
#  define chkr_garbage_detector() __chkr_garbage_detector()
#else
#  define chkr_garbage_detector()
#endif

/* GNU gettext needs to know when the locale changes. This macro tells it. */
#ifdef ENABLE_NLS
extern int _nl_msg_cat_cntr;
#  define locale_changed() \
	do { \
		++_nl_msg_cat_cntr; \
	} while (0)
#else /* !ENABLE_NLS */
#  define locale_changed()
#endif /* ENABLE_NLS */

/* This structure definition is only really needed if COMP_SRC==1, but it is
   used in external declarations quite freely, so it's included
   unconditionally */

struct compression {
	/* The following are const because they should be pointers to parts
	 * of strings allocated elsewhere and should not be written through
	 * or freed themselves.
	 */
	const char *prog;
	const char *ext;
	/* The following should be freed when discarding an instance of this
	 * structure.
	 */
	char *stem;
};

extern struct compression comp_list[];

/*-------------------------------------*/
/* Now for some function prototypes... */
/*-------------------------------------*/

/* some library function declarations */
#include <stddef.h>	/* for size_t */
#include "xalloc.h"
#include "xstrndup.h"
extern char *create_tempdir (const char *template)
	ATTRIBUTE_WARN_UNUSED_RESULT;

extern bool debug_level;	/* shows whether -d issued */
extern void init_debug (void);
extern void debug (const char *message, ...) ATTRIBUTE_FORMAT_PRINTF(1, 2);
extern void debug_error (const char *message, ...)
	ATTRIBUTE_FORMAT_PRINTF(1, 2);

struct pipeline;

/* compression.c */
extern struct compression *comp_info (const char *filename, int want_stem);
extern struct compression *comp_file (const char *filename);

/* straycats.c */
extern int straycats (const char *database, const char *manpath);

/* lexgrog.l */
struct lexgrog;
extern int find_name (const char *file, const char *filename,
		      struct lexgrog *p_lg, const char *encoding);
extern int find_name_decompressed (struct pipeline *p, const char *filename,
				   struct lexgrog *p_lg);

/* util.c */
extern int is_changed (const char *fa, const char *fb);
extern int is_directory (const char *path);
extern char *escape_shell (const char *unesc);
extern int remove_directory (const char *directory, int recurse);
extern char *trim_spaces (const char *s);
extern char *lang_dir (const char *filename);
extern void init_locale (void);

extern char *appendstr (char *, ...)
	ATTRIBUTE_SENTINEL ATTRIBUTE_WARN_UNUSED_RESULT;

extern int quiet;		/* be quiet(er) if 1 */

/*--------------------------*/
/* Some general definitions */
/*--------------------------*/

#define MANPAGE		0
#define CATPAGE		1

/* exit codes */
#define OK		0	/* success */
#define FAIL		1	/* usage or syntax error */
#define FATAL		2	/* operational error */
#define CHILD_FAIL	3	/* child failed */
#define NOT_FOUND	16	/* No action was taken */

/* System or user catpaths? Allow bitwise disjunctions of these. */
#define SYSTEM_CAT	1
#define USER_CAT	2

/* string macros */
#define STREQ(a,b)	(strcmp(a,b) == 0)
#define STRNEQ(a,b,d)	(strncmp(a,b,d) == 0)

/* Functions in <ctype.h> require their argument to be either an unsigned
 * char promoted to int or EOF. This macro helps get that right.
 */
#define CTYPE(func,arg) (func((unsigned char)(arg)))

/* access(2), but with boolean semantics. */
#define CAN_ACCESS(pathname, mode)	(access (pathname, mode) == 0)

/* FSSTND directories */
#define CAT_ROOT	"/var/catman"		/* required by fsstnd() */
#define MAN_ROOT	"/usr" 			/* required by fsstnd() */
/* FHS directories */
#define FHS_CAT_ROOT	"/var/cache/man"	/* required by fsstnd() */
#define FHS_MAN_ROOT	"/usr/share"		/* required by fsstnd() */

/* some special database keys used for storing important info */
#define VER_KEY         "$version$"	/* version key */
#define VER_ID          "2.5.0"		/* version content */

/* defines the ordered list of filters detected by lexgrog */
enum	{ TBL_FILTER=0	/* tbl */
	, EQN_FILTER	/* eqn */
	, PIC_FILTER	/* pic */
	, GRAP_FILTER	/* grap */
	, REF_FILTER	/* refer */
	, VGRIND_FILTER	/* vgrind */
	, MAX_FILTERS	/* delimiter */
	};

typedef struct lexgrog {
	int type;
	char *whatis;
	char *filters;
} lexgrog;

#endif /* MANCONFIG_H */
