/* infomap.h -- description of a keymap in Info and related functions.
   $Id: infomap.h 5927 2014-11-14 13:32:00Z gavin $

   Copyright 1993, 1999, 2001, 2002, 2004, 2007, 2013, 2014 Free Software
   Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Originaly written by Brian Fox. */

#ifndef INFOMAP_H
#define INFOMAP_H

#include "info.h"

#define ESC '\033'
#define DEL '\177'
#define TAB '\011'      
#define RET '\r'
#define LFD '\n'
#define SPC ' '

#define meta_character_threshold (DEL + 1)
#define control_character_threshold (SPC)

#define meta_character_bit 0x80
#define control_character_bit 0x40

#define Meta_p(c) (((c) > meta_character_threshold))
#define Control_p(c) ((c) < control_character_threshold)

#define Meta(c) ((c) | (meta_character_bit))
#define UnMeta(c) ((c) & (~meta_character_bit))
#define Control(c) ((toupper (c)) & (~control_character_bit))
#define UnControl(c) (tolower ((c) | control_character_bit))

/* Structure used to map sequences of bytes to recognized keys. */
typedef struct bytemap_entry
{
  char type;
  int key;
  struct bytemap_entry *next;
} BYTEMAP_ENTRY;

#define BYTEMAP_NONE 0
#define BYTEMAP_KEY 1
#define BYTEMAP_MAP 2
#define BYTEMAP_ESC 3

extern BYTEMAP_ENTRY *byte_seq_to_key;

typedef struct keymap_entry
{
  char type;
  union
  {
    InfoCommand *function;         /* The address of a function. */
    struct keymap_entry *keymap;   /* The address of another Keymap */
  } value;
} KEYMAP_ENTRY;

/* The values that TYPE can have in a keymap entry. */
#define ISFUNC 0
#define ISKMAP 1

/* We use Keymap for a pointer to a block of KEYMAP_SIZE KEYMAP_ENTRY's. */
typedef KEYMAP_ENTRY *Keymap;

extern Keymap info_keymap;
extern Keymap echo_area_keymap;

#define KEY_RIGHT_ARROW		256
#define KEY_LEFT_ARROW		257
#define KEY_UP_ARROW		258
#define KEY_DOWN_ARROW		259
#define KEY_PAGE_UP		260
#define KEY_PAGE_DOWN		261
#define KEY_HOME		262
#define KEY_END			263
#define KEY_DELETE		264
#define KEY_INSERT		265
#define KEY_CTL_LEFT_ARROW	266
#define KEY_CTL_RIGHT_ARROW	267
#define KEY_CTL_DELETE		268
#define KEY_BACK_TAB		269
#define KEY_MOUSE               270

/* Add this to get the offset of the key binding with the meta key. */
#define KEYMAP_META_BASE 271

/* Number of entries in a Keymap: 256 entries for plain byte values plus
   mappings for special keys.  The bindings for the key chords with meta
   follow. */
#define KEYMAP_SIZE (KEYMAP_META_BASE * 2)

#define KEYMAP_META(k) ((k) < KEYMAP_META_BASE ? (k) + KEYMAP_META_BASE : (k))

/* Default "infokey file", where user defs are kept and read by
   Info.  MS-DOS doesn't allow leading dots in file names.  */
#ifdef __MSDOS__
#define INFOKEY_FILE		"_infokey"
#else
#define INFOKEY_FILE		".infokey"
#endif

#define	A_MAX_COMMAND		120
#define	A_INVALID		121

#define	CONTROL(c)		((c) & 0x1f)

/* Return a new keymap which has all the uppercase letters mapped to run
   the function info_do_lowercase_version (). */
extern Keymap keymap_make_keymap (void);

/* Read init file and initialize the info keymaps. */
extern void read_init_file (char *init_file);

#endif /* not INFOMAP_H */
