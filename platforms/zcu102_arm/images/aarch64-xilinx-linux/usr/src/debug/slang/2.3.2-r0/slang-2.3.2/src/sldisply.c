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

#include "slinclud.h"

#include <time.h>
#include <ctype.h>

#if !defined(VMS) || (__VMS_VER >= 70000000)
# include <sys/time.h>
# ifdef __QNX__
#  include <sys/select.h>
# endif
# include <sys/types.h>
#endif

#if defined(__BEOS__) && !defined(__HAIKU__)
/* Prototype for select */
# include <net/socket.h>
#endif

#ifdef HAVE_TERMIOS_H
# include <termios.h>
#endif

#ifdef VMS
# include <unixlib.h>
# include <unixio.h>
# include <dvidef.h>
# include <descrip.h>
# include <lib$routines.h>
# include <starlet.h>
#else
# if !defined(sun)
#  include <sys/ioctl.h>
# endif
#endif

#ifdef SYSV
# include <sys/termio.h>
# include <sys/stream.h>
# include <sys/ptem.h>
# include <sys/tty.h>
#endif

#if defined (_AIX) && !defined (FD_SET)
# include <sys/select.h>	/* for FD_ISSET, FD_SET, FD_ZERO */
#endif

#include <errno.h>

#if defined(__DECC) && defined(VMS)
/* These get prototypes for write an sleep */
# include <unixio.h>
#endif
#include <signal.h>

#include "slang.h"
#include "_slang.h"

#define CROSS_COMPILE_FOR_VMS	0
#ifdef VMS
# define VMS_SYSTEM 1
#else
# if CROSS_COMPILE_FOR_VMS
#  define VMS 1
#  undef __unix__
# endif
#endif

/* Colors:  These definitions are used for the display.  However, the
 * application only uses object handles which get mapped to this
 * internal representation.  The mapping is performed by the Color_Map
 * structure below.
 *
 * Colors are encoded in 25 bits as follows:
 * - Values 0-255 for standard palette
 * - 256 for terminal's default color
 * - 0x1RRGGBB for true colors
 *
 * The bits are split so we can maintain ABI compatibility on 64 bit machines.
 */
#if SLTT_HAS_TRUECOLOR_SUPPORT && (SIZEOF_LONG == 8)
static int Has_True_Color = 0;
# if 0
#  define FG_MASK_LOW		0x0000000000FFFFFFULL
#  define FG_MASK_HIGH		0x0200000000000000ULL
#  define BG_MASK		0x01FFFFFF00000000ULL
#  define ATTR_MASK		0x000000003F000000ULL
#  define INVALID_ATTR		0xFFFFFFFFFFFFFFFFULL
#  define SLSMG_COLOR_DEFAULT	0x0000000000000100ULL

#  define TRUE_COLOR_BIT		0x01000000ULL
#  define IS_TRUE_COLOR(f)	((f) & TRUE_COLOR_BIT)
#  define GET_FG(fgbg) ((((fgbg) & FG_MASK_HIGH) >> 33) | ((fgbg) & FG_MASK_LOW))
#  define GET_BG(fgbg) (((fgbg) & BG_MASK) >> 32)
#  define MAKE_COLOR(f, b) \
   ((((f) << 33) & FG_MASK_HIGH) | ((f) & FG_MASK_LOW) \
     | (((b) << 32) & BG_MASK))
# else
#  define FG_MASK_LOW		0x000000000000FF00ULL
#  define FG_MASK_HIGH		0x0000FFFF00000000ULL
#  define FG_TRUE_COLOR		0x0000000000000001ULL
#  define BG_MASK_LOW		0x0000000000FF0000ULL
#  define BG_MASK_HIGH		0xFFFF000000000000ULL
#  define BG_TRUE_COLOR		0x0000000000000002ULL
#  define ATTR_MASK		0x000000003F000000ULL
#  define INVALID_ATTR		0xFFFFFFFFFFFFFFFFULL

#  define SLSMG_COLOR_DEFAULT		0x00000100ULL
#  define TRUE_COLOR_BIT		0x01000000ULL
#  define IS_TRUE_COLOR(f)	((f) & TRUE_COLOR_BIT)
#  define GET_FG(fgbg) \
   ((((fgbg)&FG_TRUE_COLOR)<<24) | (((fgbg)&FG_MASK_HIGH)>>24) \
     | (((fgbg)&FG_MASK_LOW)>>8))
#  define GET_BG(fgbg) \
   ((((fgbg)&BG_TRUE_COLOR)<<23) | (((fgbg)&BG_MASK_HIGH)>>40) \
     | (((fgbg)&BG_MASK_LOW)>>16))
#  define MAKE_COLOR(f, b) \
   (((((f)&0x1000000)>>24) | (((f)&0xFF)<<8) | (((f)&(0xFFFF00))<<24)) \
     | ((((b)&0x1000000)>>23) | (((b)&0xFF)<<16) | (((b)&(0xFFFF00))<<40)))
# endif
#else
# undef SLTT_HAS_TRUECOLOR_SUPPORT
# define SLTT_HAS_TRUECOLOR_SUPPORT 0
# define FG_MASK_LOW		0x0000FF00UL
# define FG_MASK_HIGH		0x00000000UL
# define BG_MASK		0x00FF0000UL
# define ATTR_MASK		0x3F000000UL
# define INVALID_ATTR		0xFFFFFFFFUL
# define SLSMG_COLOR_DEFAULT	0x000000FFUL
# define IS_TRUE_COLOR(f)	(0)
# define GET_FG(fgbg) (((fgbg) & FG_MASK_LOW)>>8)
# define GET_BG(fgbg) (((fgbg) & BG_MASK) >> 16)
# define MAKE_COLOR(fg, bg) (((fg)<<8) | ((bg)<<16))
#endif

int SLtt_Screen_Cols = 80;
int SLtt_Screen_Rows = 24;
int SLtt_Term_Cannot_Insert = 0;
int SLtt_Term_Cannot_Scroll = 0;
int SLtt_Use_Ansi_Colors = 0;
int SLtt_Blink_Mode = 0;
int SLtt_Use_Blink_For_ACS = 0;
int SLtt_Newline_Ok = 0;
int SLtt_Has_Alt_Charset = 0;
int SLtt_Force_Keypad_Init = -1;

static int Use_Relative_Cursor_Addressing = 0;
static int Max_Relative_Cursor_r = 0;

/* static int UTF8_Mode = -1; */

void (*_pSLtt_color_changed_hook)(void);

static int Bce_Color_Offset = 0;
static int Can_Background_Color_Erase = 1;

/* -1 means unknown */
int SLtt_Has_Status_Line = -1;	       /* hs */
int SLang_TT_Write_FD = -1;

static int TT_Is_Initialized = 0;
static int Automatic_Margins;
/* static int No_Move_In_Standout; */
static int Worthless_Highlight;
#define HP_GLITCH_CODE
#ifdef HP_GLITCH_CODE
/* This glitch is exclusive to HP term.  Basically it means that to clear
 * attributes, one has to erase to the end of the line.
 */
static int Has_HP_Glitch;
#endif

static char *Reset_Color_String;
static int Is_Color_Terminal = 0;

static int Linux_Console;
static int Mouse_Mode = -1;

/* The following comment is nolonger valid, but is here in case there are
 * some apps that use SLtt_Use_Blink_For_ACS and still need porting to v2.
 * -------
 * It is crucial that JMAX_COLORS must be less than 128 since the high bit
 * is used to indicate a character from the ACS (alt char set).  The exception
 * to this rule is if SLtt_Use_Blink_For_ACS is true.  This means that of
 * the highbit is set, we interpret that as a blink character.  This is
 * exploited by DOSemu.
 */
#ifndef SLTT_MAX_COLORS
# define SLTT_MAX_COLORS 0x8000       /* consistent with SLSMG_COLOR_MASK */
#endif

#define JMAX_COLORS SLTT_MAX_COLORS
#define JNORMAL_COLOR 0

typedef struct
{
   SLtt_Char_Type fgbg;
   SLtt_Char_Type mono;
}
Brush_Info_Type;

static Brush_Info_Type Brush_Table[JMAX_COLORS];

/* 0 if least significant bit is blue, not red */
static int Is_Fg_BGR = 0;
static int Is_Bg_BGR = 0;
#define COLOR_ARG(color, is_bgr) ((is_bgr) ? RGB_to_BGR[(color)&0x7] : (color))
static SLCONST int RGB_to_BGR[] =
{
     0, 4, 2, 6, 1, 5, 3, 7
};

static SLCONST char *Color_Fg_Str = "\033[3%dm";
static SLCONST char *Color_Bg_Str = "\033[4%dm";
#if SLTT_HAS_TRUECOLOR_SUPPORT
static SLCONST char *Color_RGB_Fg_Str = "\033[38;2;%d;%d;%dm";
static SLCONST char *Color_RGB_Bg_Str = "\033[48;2;%d;%d;%dm";
#endif
static SLCONST char *Default_Color_Fg_Str = "\033[39m";
static SLCONST char *Default_Color_Bg_Str = "\033[49m";

static int Max_Terminfo_Colors = 8;	       /* termcap Co */

char *SLtt_Graphics_Char_Pairs = NULL;	       /* ac termcap string -- def is vt100 */

/* 1 if terminal lacks the ability to go into insert mode or into delete
   mode. Currently controlled by S-Lang but later perhaps termcap. */

static SLCONST char *UnderLine_Vid_Str;
static SLCONST char *Blink_Vid_Str;
static SLCONST char *Italic_Vid_Str;
static SLCONST char *Bold_Vid_Str;
static SLCONST char *Ins_Mode_Str; /* = "\033[4h"; */   /* ins mode (im) */
static SLCONST char *Eins_Mode_Str; /* = "\033[4l"; */  /* end ins mode (ei) */
static SLCONST char *Scroll_R_Str; /* = "\033[%d;%dr"; */ /* scroll region */
static SLCONST char *Cls_Str; /* = "\033[2J\033[H"; */  /* cl termcap STR  for ansi terminals */
static SLCONST char *Rev_Vid_Str; /* = "\033[7m"; */    /* mr,so termcap string */
static SLCONST char *Norm_Vid_Str; /* = "\033[m"; */   /* me,se termcap string */
static SLCONST char *Del_Eol_Str; /* = "\033[K"; */	       /* ce */
static SLCONST char *Del_Bol_Str; /* = "\033[1K"; */	       /* cb */
static SLCONST char *Del_Char_Str; /* = "\033[P"; */   /* dc */
static SLCONST char *Del_N_Lines_Str; /* = "\033[%dM"; */  /* DL */
static SLCONST char *Add_N_Lines_Str; /* = "\033[%dL"; */  /* AL */
static SLCONST char *Rev_Scroll_Str;
static SLCONST char *Curs_Up_Str;      /* "up" */
static SLCONST char *Curs_UpN_Str;     /* "UP" */
static SLCONST char *Curs_Dn_Str;      /* "do" */
static SLCONST char *Curs_DnN_Str;     /* "DO" */
static SLCONST char *Curs_Right_Str;    /* "nd" */
static SLCONST char *Curs_RightN_Str;    /* "RI" */
static SLCONST char *Curs_Left_Str;    /* "bc", "le" */
static SLCONST char *Curs_LeftN_Str;    /* "LE" */
static SLCONST char *Clear_EOS_Str;   /* cd */

static SLCONST char *Cursor_Visible_Str;    /* ve termcap string */
static SLCONST char *Cursor_Invisible_Str;    /* vi termcap string */
#if 0
static SLCONST char *Start_Mouse_Rpt_Str;  /* Start mouse reporting mode */
static SLCONST char *End_Mouse_Rpt_Str;  /* End mouse reporting mode */
#endif
static SLCONST char *Start_Alt_Chars_Str;  /* as */
static SLCONST char *End_Alt_Chars_Str;   /* ae */
static SLCONST char *Enable_Alt_Char_Set;  /* eA */

static SLCONST char *Start_Abs_Cursor_Addressing_Mode;
static SLCONST char *Keypad_Init_Str;
static SLCONST char *End_Abs_Cursor_Addressing_Mode;
static SLCONST char *Keypad_Reset_Str;

/* status line functions */
static SLCONST char *Disable_Status_line_Str;  /* ds */
static SLCONST char *Return_From_Status_Line_Str;   /* fs */
static SLCONST char *Goto_Status_Line_Str;     /* ts */
/* static int Num_Status_Line_Columns; */   /* ws */
/* static int Status_Line_Esc_Ok;	 */       /* es */

/* cm string has %i%d since termcap numbers columns from 0 */
/* char *CURS_POS_STR = "\033[%d;%df";  ansi-- hor and vert pos */
static SLCONST char *Abs_Curs_Pos_Str; /* = "\033[%i%d;%dH";*/   /* cm termcap string */

/* scrolling region */
static int Scroll_r1 = 0, Scroll_r2 = 23;
static int Cursor_r, Cursor_c;	       /* 0 based */

/* current attributes --- initialized to impossible value */
static SLtt_Char_Type Current_Fgbg = INVALID_ATTR;

static int Cursor_Set;		       /* 1 if cursor position known, 0
					* if not.  -1 if only row is known
					*/
/* This is used only in relative-cursor-addressing mode */
static SLsmg_Char_Type Display_Start_Chars[SLTT_MAX_SCREEN_ROWS];

#define MAX_OUTPUT_BUFFER_SIZE 4096

static unsigned char Output_Buffer[MAX_OUTPUT_BUFFER_SIZE];
static unsigned char *Output_Bufferp = Output_Buffer;

unsigned long SLtt_Num_Chars_Output = 0;

int _pSLusleep (unsigned long usecs)
{
#if !defined(VMS) || (__VMS_VER >= 70000000)
   struct timeval tv;
   tv.tv_sec = usecs / 1000000;
   tv.tv_usec = usecs % 1000000;
   return select(0, NULL, NULL, NULL, &tv);
#else
   return 0;
#endif
}

int SLtt_flush_output (void)
{
   size_t total;
   size_t n = (Output_Bufferp - Output_Buffer);

   SLtt_Num_Chars_Output += n;

   total = 0;
   while (n > 0)
     {
	ssize_t nwrite = write (SLang_TT_Write_FD, (char *) Output_Buffer + total, n);
	if (nwrite == -1)
	  {
	     nwrite = 0;
#ifdef EAGAIN
	     if (errno == EAGAIN)
	       {
		  _pSLusleep (100000);   /* 1/10 sec */
		  continue;
	       }
#endif
#ifdef EWOULDBLOCK
	     if (errno == EWOULDBLOCK)
	       {
		  _pSLusleep (100000);
		  continue;
	       }
#endif
#ifdef EINTR
	     if (errno == EINTR) continue;
#endif
	     break;
	  }
	n -= nwrite;
	total += nwrite;
     }
   Output_Bufferp = Output_Buffer;
   return n;
}

int SLtt_Baud_Rate = 0;
static void tt_write(SLCONST char *str, SLstrlen_Type n)
{
   static unsigned long last_time;
   static SLstrlen_Type total;

   if ((str == NULL) || (n == 0)) return;
   total += n;

   while (1)
     {
	size_t ndiff = MAX_OUTPUT_BUFFER_SIZE - (Output_Bufferp - Output_Buffer);
	if (ndiff < n)
	  {
	     memcpy ((char *) Output_Bufferp, str, ndiff);
	     Output_Bufferp += ndiff;
	     SLtt_flush_output ();
	     n -= ndiff;
	     str += ndiff;
	  }
	else
	  {
	     memcpy ((char *) Output_Bufferp, str, n);
	     Output_Bufferp += n;
	     break;
	  }
     }

   if (((SLtt_Baud_Rate > 150) && (SLtt_Baud_Rate <= 9600))
       && (10 * total > (unsigned int)SLtt_Baud_Rate))
     {
	unsigned long now;
	total = 0;
	if ((now = (unsigned long) time(NULL)) - last_time <= 1)
	  {
	     SLtt_flush_output ();
	     sleep((unsigned) 1);
	  }
	last_time = now;
     }
}

static void tt_write_string (SLCONST char *str)
{
   if (str != NULL) tt_write(str, strlen(str));
}

void SLtt_write_string (SLFUTURE_CONST char *str)
{
   tt_write_string (str);
   Cursor_Set = 0;
}

void SLtt_putchar (char ch)
{
   SLtt_normal_video ();
   if (Cursor_Set == 1)
     {
	if (ch >= ' ') Cursor_c++;
	else if (ch == '\b') Cursor_c--;
	else if (ch == '\r') Cursor_c = 0;
	else Cursor_Set = 0;

	if ((Cursor_c + 1 == SLtt_Screen_Cols)
	    && Automatic_Margins) Cursor_Set = 0;
     }

   if (Output_Bufferp < Output_Buffer + MAX_OUTPUT_BUFFER_SIZE)
     {
	*Output_Bufferp++ = (unsigned char) ch;
     }
   else tt_write (&ch, 1);
}

#if defined(__GNUC__)
# pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif
static unsigned int tt_sprintf(char *buf, unsigned int buflen, SLCONST char *fmt, int x, int y)
{
   SLCONST char *fmt_max;
   unsigned char *b, *bmax;
   int offset;
   int z, z1, parse_level;
   int zero_pad;
   int field_width;
   int variables [26];
   int stack [64];
   unsigned int stack_len;
   int parms [10];
#define STACK_POP (stack_len ? stack[--stack_len] : 0)

   if (fmt == NULL)
     {
	*buf = 0;
	return 0;
     }

   stack [0] = y;	       /* pushed for termcap */
   stack [1] = x;
   stack_len = 2;

   parms [1] = x;	       /* p1 */
   parms [2] = y;	       /* p2 */

   offset = 0;
   zero_pad = 0;
   field_width = 0;

   b = (unsigned char *) buf;
   bmax = b + buflen;

   fmt_max = fmt + strlen (fmt);

   while ((fmt < fmt_max) && (b < bmax))
     {
	unsigned char ch = *fmt++;

	if (ch != '%')
	  {
	     *b++ = ch;
	     continue;
	  }

	if (fmt == fmt_max) break;
	ch = *fmt++;

	switch (ch)
	  {
	   default:
	     *b++ = ch;
	     break;

	   case 'p':

	     if (fmt == fmt_max) break;
	     ch = *fmt++;
	     if ((ch >= '0') && (ch <= '9'))
	       stack [stack_len++] = parms [ch - '0'];
	     break;

	   case '\'':   /* 'x' */
	     if (fmt == fmt_max) break;
	     stack [stack_len++] = *fmt++;
	     if (fmt < fmt_max) fmt++;     /* skip ' */
	     break;

	   case '{':	       /* literal constant, e.g. {30} */
	     z = 0;
	     while ((fmt < fmt_max) && ((ch = *fmt) <= '9') && (ch >= '0'))
	       {
		  z = z * 10 + (ch - '0');
		  fmt++;
	       }
	     stack [stack_len++] = z;
	     if ((ch == '}') && (fmt < fmt_max)) fmt++;
	     break;

	   case '0':
	     if (fmt == fmt_max) break;
	     ch = *fmt;
	     if ((ch != '2') && (ch != '3'))
	       break;
	     zero_pad = 1;
	     fmt++;
	     /* drop */

	   case '2':
	   case '3':
	     if (fmt == fmt_max) break;
	     if (*fmt == 'x')
	       {
		  char x_fmt_buf [4];
		  char *x_fmt_buf_ptr;

		  x_fmt_buf_ptr = x_fmt_buf;
		  if (zero_pad) *x_fmt_buf_ptr++ = '0';
		  *x_fmt_buf_ptr++ = ch;
		  *x_fmt_buf_ptr++ = 'X';
		  *x_fmt_buf_ptr = 0;

		  z = STACK_POP;
		  z += offset;

		  sprintf ((char *)b, x_fmt_buf, z);
		  b += strlen ((char *)b);
		  zero_pad = 0;
		  break;
	       }

	     field_width = (ch - '0');
		  /* drop */

	   case 'd':
	     z = STACK_POP;
	     z += offset;
	     if (z >= 100)
	       {
		  *b++ = z / 100 + '0';
		  z = z % 100;
		  zero_pad = 1;
		  field_width = 2;
	       }
	     else if (zero_pad && (field_width == 3))
	       *b++ = '0';

	     if (b == bmax) break;
	     if (z >= 10)
	       {
		  *b++ = z / 10 + '0';
		  z = z % 10;
	       }
	     else if (zero_pad && (field_width >= 2))
	       *b++ = '0';

	     if (b == bmax) break;
	     *b++ = z + '0';
	     field_width = zero_pad = 0;
	     break;

	   case 'x':
	     z = STACK_POP;
	     z += offset;
	     if (b + 16 >= bmax)
	       break;
	     sprintf ((char *) b, "%X", z);
	     b += strlen ((char *)b);
	     break;

	   case 'i':
	     offset = 1;
	     break;

	   case '+':
	     /* Handling this depends upon whether or not we are parsing
	      * terminfo.  Terminfo requires the stack so use it as an
	      * indicator.
	      */
	     if (stack_len > 2)
	       {
		  z = STACK_POP;
		  stack [stack_len - 1] += z;
	       }
	     else if (fmt < fmt_max)
	       {
		  ch = *fmt++;
		  if ((unsigned char) ch == 128) ch = 0;
		  ch = ch + (unsigned char) STACK_POP;
		  if (ch == '\n') ch++;
		  *b++ = ch;
	       }
	     break;

	     /* Binary operators */
	   case '-':
	   case '*':
	   case '/':
	   case 'm':
	   case '&':
	   case '|':
	   case '^':
	   case '=':
	   case '>':
	   case '<':
	   case 'A':
	   case 'O':
	     z1 = STACK_POP;
	     z = STACK_POP;
	     switch (ch)
	       {
		case '-': z = (z - z1); break;
		case '*': z = (z * z1); break;
		case '/': z = (z / z1); break;
		case 'm': z = (z % z1); break;
		case '&': z = (z & z1); break;
		case '|': z = (z | z1); break;
		case '^': z = (z ^ z1); break;
		case '=': z = (z == z1); break;
		case '>': z = (z > z1); break;
		case '<': z = (z < z1); break;
		case 'A': z = (z && z1); break;
		case 'O': z = (z || z1); break;
	       }
	     stack [stack_len++] = z;
	     break;

	     /* unary */
	   case '!':
	     z = STACK_POP;
	     stack [stack_len++] = !z;
	     break;

	   case '~':
	     z = STACK_POP;
	     stack [stack_len++] = ~z;
	     break;

	   case 'r':		       /* termcap -- swap parameters */
	     z = stack [0];
	     stack [0] = stack [1];
	     stack [1] = z;
	     break;

	   case '.':		       /* termcap */
	   case 'c':
	     ch = (unsigned char) STACK_POP;
	     if (ch == '\n') ch++;
	     *b++ = ch;
	     break;

	   case 'g':
	     if (fmt == fmt_max) break;
	     ch = *fmt++;
	     if ((ch >= 'a') && (ch <= 'z'))
	       stack [stack_len++] = variables [ch - 'a'];
	     break;

	   case 'P':
	     if (fmt == fmt_max) break;
	     ch = *fmt++;
	     if ((ch >= 'a') && (ch <= 'z'))
	       variables [ch - 'a'] = STACK_POP;
	     break;

	     /* If then else parsing.  Actually, this is rather easy.  The
	      * key is to notice that 'then' does all the work.  'if' simply
	      * there to indicate the start of a test and endif indicates
	      * the end of tests.  If 'else' is seen, then skip to
	      * endif.
	      */
	   case '?':		       /* if */
	   case ';':		       /* endif */
	     break;

	   case 't':		       /* then */
	     z = STACK_POP;
	     if (z != 0)
	       break;		       /* good.  Continue parsing. */

	     /* z == 0 and test has failed.  So, skip past this entire if
	      * expression to the matching else or matching endif.
	      */
	     /* drop */
	   case 'e':		       /* else */

	     parse_level = 0;
	     while (fmt < fmt_max)
	       {
		  unsigned char ch1;

		  ch1 = *fmt++;
		  if ((ch1 != '%') || (fmt == fmt_max))
		    continue;

		  ch1 = *fmt++;

		  if (ch1 == '?') parse_level++;   /* new if */
		  else if (ch1 == 'e')
		    {
		       if ((ch != 'e') && (parse_level == 0))
			 break;
		    }
		  else if (ch1 == ';')
		    {
		       if (parse_level == 0)
			 break;
		       parse_level--;
		    }
	       }
	     break;
	  }
     }
   if (b >= bmax)
     b = bmax - 1;
   *b = 0;

   return (unsigned int) (b - (unsigned char *) buf);
}
#if defined(__GNUC__)
# pragma GCC diagnostic warning "-Wformat-nonliteral"
#endif

static void tt_printf(SLCONST char *fmt, int x, int y)
{
   char buf[1024];
   unsigned int n;
   if (fmt == NULL) return;
   n = tt_sprintf(buf, sizeof (buf), fmt, x, y);
   tt_write(buf, n);
}

void SLtt_set_scroll_region (int r1, int r2)
{
   if (Use_Relative_Cursor_Addressing)
     return;

   Scroll_r1 = r1;
   Scroll_r2 = r2;
   tt_printf (Scroll_R_Str, Scroll_r1, Scroll_r2);
   Cursor_Set = 0;
}

void SLtt_reset_scroll_region (void)
{
   SLtt_set_scroll_region(0, SLtt_Screen_Rows - 1);
}

int SLtt_set_cursor_visibility (int show)
{
   if ((Cursor_Visible_Str == NULL) || (Cursor_Invisible_Str == NULL))
     return -1;

   tt_write_string (show ? Cursor_Visible_Str : Cursor_Invisible_Str);
   return 0;
}

static void cursor_motion (SLCONST char *s1, SLCONST char *sN, int n)
{
   if ((n == 1) && (s1 != NULL))
     {
	tt_write_string (s1);
	return;
     }

   if (n <= 0)
     return;

   if (sN != NULL)
     tt_printf (sN, n, 0);
   else while (n > 0)
     {
	tt_write_string (s1);
	n--;
     }
}

static void goto_relative_rc (int r, int c)
{
   if (r < 0)
     return;

   if (Cursor_Set != 1)
     {
	/* Do not know where the cursor is.  At least get the column correct */
	tt_write ("\r", 1);
	Cursor_c = 0;
	Cursor_Set = 1;		       /* pretend we know the row */
     }

   if (Cursor_r > r)
     cursor_motion (Curs_Up_Str, Curs_UpN_Str, Cursor_r-r);
   else if (Cursor_r < r)
     {
	tt_write ("\r", 1); Cursor_c = 0;
	if (r > Max_Relative_Cursor_r)
	  {
	     cursor_motion (Curs_Dn_Str, Curs_DnN_Str, Max_Relative_Cursor_r-Cursor_r);
	     Cursor_r = Max_Relative_Cursor_r;
	     while (Cursor_r < r)
	       {
		  tt_write ("\n", 1);
		  Cursor_r++;
	       }
	  }
	else
	  cursor_motion (Curs_Dn_Str, Curs_DnN_Str, r-Cursor_r);
     }
   Cursor_r = r;
   if (r > Max_Relative_Cursor_r)
     Max_Relative_Cursor_r = r;

   if (Cursor_c > c)
     cursor_motion (Curs_Left_Str, Curs_LeftN_Str, Cursor_c-c);
   else if (Cursor_c < c)
     cursor_motion (Curs_Right_Str, Curs_RightN_Str, c-Cursor_c);

   Cursor_c = c;
   Cursor_Set = 1;
}

/* the goto_rc function moves to row relative to scrolling region */
void SLtt_goto_rc(int r, int c)
{
   char *s = NULL;
   char buf[6];

   if ((c < 0) || (r < 0))
     {
	Cursor_Set = 0;
	Cursor_c = 0;
	Cursor_r = 0;
	tt_write("\r", 1);
	return;
     }
   if (Use_Relative_Cursor_Addressing)
     {
	goto_relative_rc (r, c);
	return;
     }

   r += Scroll_r1;

   if ((Cursor_Set > 0)
       || ((Cursor_Set < 0) && !Automatic_Margins))
     {
	int n = r - Cursor_r;
	if ((n == -1) && (Cursor_Set > 0) && (Cursor_c == c)
	    && (Curs_Up_Str != NULL))
	  {
	     s = (char *)Curs_Up_Str;
	  }
	else if ((n >= 0) && (n <= 4))
	  {
	     if ((n == 0) && (Cursor_Set == 1)
		 && ((c > 1) || (c == Cursor_c)))
	       {
		  if (Cursor_c == c) return;
		  if (Cursor_c == c + 1)
		    {
		       s = buf;
		       *s++ = '\b'; *s = 0;
		       s = buf;
		    }
	       }
	     else if (c == 0)
	       {
		  s = buf;
		  if ((Cursor_Set != 1) || (Cursor_c != 0)) *s++ = '\r';
		  while (n--) *s++ = '\n';
#ifdef VMS
		  /* Need to add this after \n to start a new record.  Sheesh. */
		  *s++ = '\r';
#endif
		  *s = 0;
		  s = buf;
	       }
	     /* Will fail on VMS */
#ifndef VMS
	     else if (SLtt_Newline_Ok && (Cursor_Set == 1) &&
		      (Cursor_c >= c) && (c + 3 > Cursor_c))
	       {
		  s = buf;
		  while (n--) *s++ = '\n';
		  n = Cursor_c - c;
		  while (n--) *s++ = '\b';
		  *s = 0;
		  s = buf;
	       }
#endif
	  }
     }
   if (s != NULL) tt_write_string(s);
   else
     {
	tt_printf(Abs_Curs_Pos_Str, r, c);
     }
   Cursor_c = c; Cursor_r = r;
   Cursor_Set = 1;
}

void SLtt_begin_insert (void)
{
   tt_write_string(Ins_Mode_Str);
}

void SLtt_end_insert (void)
{
   tt_write_string(Eins_Mode_Str);
}

void SLtt_delete_char (void)
{
   SLtt_normal_video ();
   tt_write_string(Del_Char_Str);
}

void SLtt_erase_line (void)
{
   tt_write ("\r", 1);
   Cursor_Set = 1; Cursor_c = 0;
   SLtt_del_eol();
   /* Put the cursor back at the beginning of the line */
   tt_write_string("\r");
   Cursor_Set = 1; Cursor_c = 0;
}

/* It appears that the Linux console, and most likely others do not
 * like scrolling regions that consist of one line.  So I have to
 * resort to this stupidity to make up for that stupidity.
 */
static void delete_line_in_scroll_region (void)
{
   SLtt_goto_rc (Cursor_r - Scroll_r1, 0);
   SLtt_del_eol ();
}

void SLtt_delete_nlines (int nn)
{
   unsigned int n;

   if (nn <= 0) return;
   n = (unsigned int) nn;

   SLtt_normal_video ();

   if (Scroll_r1 == Scroll_r2)
     {
	delete_line_in_scroll_region ();
	return;
     }

   if (Del_N_Lines_Str != NULL) tt_printf(Del_N_Lines_Str, n, 0);
   else
   /* get a new terminal */
     {
	char buf[80];
	int curs, r1;
	unsigned int dn = n;

	if (dn > sizeof (buf))
	  dn = sizeof (buf);

	SLMEMSET (buf, '\n', dn);
	while (n > dn)
	  {
	     tt_write (buf, dn);
	     n -= dn;
	  }
	tt_write (buf, n);

	r1 = Scroll_r1;
	curs = Cursor_r;
	SLtt_set_scroll_region(curs, Scroll_r2);
	SLtt_goto_rc(Scroll_r2 - Scroll_r1, 0);
	SLMEMSET(buf, '\n', (unsigned int) n);
	tt_write(buf, (unsigned int) n);
	/* while (n--) tt_putchar('\n'); */
	SLtt_set_scroll_region(r1, Scroll_r2);
	SLtt_goto_rc(curs, 0);
     }
}

static void cls_internal (SLCONST char *escseq, int rmin)
{
   /* If the terminal is a color terminal but the user wants black and
    * white, then make sure that the colors are reset.  This appears to be
    * necessary.
    */
   if ((SLtt_Use_Ansi_Colors == 0) && Is_Color_Terminal)
     {
	if (Reset_Color_String != NULL)
	  tt_write_string (Reset_Color_String);
	else
	  tt_write_string ("\033[0m\033[m");
     }

   SLtt_normal_video();
   SLtt_reset_scroll_region ();

   tt_write_string(escseq);

   if (Use_Relative_Cursor_Addressing)
     {
	int r, rmax = SLtt_Screen_Rows;
	for (r = rmin; r < rmax; r++)
	  Display_Start_Chars[r].nchars = 0;
     }
}

void SLtt_cls (void)
{
   cls_internal (Cls_Str, 0);
}

static void _pSLtt_clear_eos (void)
{
   int rmin = Cursor_r;
   if (Cursor_c > 0) rmin++;
   cls_internal (Clear_EOS_Str, rmin);
}

void SLtt_reverse_index (int n)
{
   if (!n) return;

   SLtt_normal_video();

   if (Scroll_r1 == Scroll_r2)
     {
	delete_line_in_scroll_region ();
	return;
     }

   if (Add_N_Lines_Str != NULL) tt_printf(Add_N_Lines_Str,n, 0);
   else
     {
	while(n--) tt_write_string(Rev_Scroll_Str);
     }
}

int SLtt_Ignore_Beep = 1;
static SLCONST char *Visible_Bell_Str;

void SLtt_beep (void)
{
   if (SLtt_Ignore_Beep & 0x1) SLtt_putchar('\007');

   if (SLtt_Ignore_Beep & 0x2)
     {
	if (Visible_Bell_Str != NULL) tt_write_string (Visible_Bell_Str);
#ifdef __linux__
	else if (Linux_Console)
	  {
	     tt_write ("\033[?5h", 5);
	     SLtt_flush_output ();
	     _pSLusleep (50000);
	     tt_write ("\033[?5l", 5);
	  }
#endif
     }
   SLtt_flush_output ();
}

static void write_string_with_care (SLCONST char *);

static void del_eol (void)
{
   if ((Cursor_c == 0)
       && (Use_Relative_Cursor_Addressing)
       && (Cursor_r < SLTT_MAX_SCREEN_ROWS))
     {
	Display_Start_Chars[Cursor_r].nchars = 0;
     }

   if ((Del_Eol_Str != NULL)
       && (Can_Background_Color_Erase || (Current_Fgbg == 0)))
     {
	tt_write_string(Del_Eol_Str);
	return;
     }

   while (Cursor_c < SLtt_Screen_Cols)
     {
	write_string_with_care (" ");
	Cursor_c++;
     }
   Cursor_c = SLtt_Screen_Cols - 1;
   Cursor_Set = 0;
}

void SLtt_del_eol (void)
{
   if (Current_Fgbg != INVALID_ATTR) SLtt_normal_video ();
   del_eol ();
}

typedef SLCONST struct
{
   SLCONST char *name;
   SLtt_Char_Type color;
}
Color_Def_Type;

#define MAX_COLOR_NAMES 17
static Color_Def_Type Color_Defs [MAX_COLOR_NAMES] =
{
     {"black",		SLSMG_COLOR_BLACK},
     {"red",		SLSMG_COLOR_RED},
     {"green",		SLSMG_COLOR_GREEN},
     {"brown",		SLSMG_COLOR_BROWN},
     {"blue",		SLSMG_COLOR_BLUE},
     {"magenta",	SLSMG_COLOR_MAGENTA},
     {"cyan",		SLSMG_COLOR_CYAN},
     {"lightgray",	SLSMG_COLOR_LGRAY},
     {"gray",		SLSMG_COLOR_GRAY},
     {"brightred",	SLSMG_COLOR_BRIGHT_RED},
     {"brightgreen",	SLSMG_COLOR_BRIGHT_GREEN},
     {"yellow",		SLSMG_COLOR_BRIGHT_BROWN},
     {"brightblue",	SLSMG_COLOR_BRIGHT_BLUE},
     {"brightmagenta",	SLSMG_COLOR_BRIGHT_MAGENTA},
     {"brightcyan",	SLSMG_COLOR_BRIGHT_CYAN},
     {"white",		SLSMG_COLOR_BRIGHT_WHITE},
     {"default",	SLSMG_COLOR_DEFAULT}
};

static int Brushes_Initialized = 0;

static int initialize_brushes (void)
{
   Brush_Info_Type *b, *bmax;
   SLtt_Char_Type bg;

   b = Brush_Table;
   bmax = b + JMAX_COLORS;

   bg = 0;
   while (b < bmax)
     {
	SLtt_Char_Type fg = 7;
	while (b < bmax)
	  {
	     if (fg != bg)
	       {
		  b->fgbg = MAKE_COLOR(fg,bg);
		  b->mono = SLTT_REV_MASK;
		  b++;
	       }
	     if (fg == 0)
	       break;
	     fg--;
	  }
	bg++;
	if (bg == 8)
	  bg = 0;
     }

   Brush_Table[0].mono = 0;
   Brushes_Initialized = 1;
   return 0;
}

static Brush_Info_Type *get_brush_info (SLsmg_Color_Type color)
{
   if (Brushes_Initialized == 0)
     initialize_brushes ();

   color &= SLSMG_COLOR_MASK;

   if (color >= JMAX_COLORS)
     color = 0;

   return Brush_Table + color;
}

static SLtt_Char_Type get_brush_attr (SLsmg_Color_Type color)
{
   Brush_Info_Type *b;

   if (NULL == (b = get_brush_info (color)))
     return INVALID_ATTR;

   if (SLtt_Use_Ansi_Colors)
     return b->fgbg;

   return b->mono;
}

static SLtt_Char_Type get_brush_fgbg (SLsmg_Color_Type color)
{
   Brush_Info_Type *b = get_brush_info(color);
   if (b == NULL)
     return INVALID_ATTR;
   return b->fgbg;
}

int SLtt_set_mono (int obj, SLFUTURE_CONST char *what, SLtt_Char_Type mask)
{
   Brush_Info_Type *b;

   (void) what;
   if (NULL == (b = get_brush_info (obj)))
     return -1;

   b->mono = mask & ATTR_MASK;
   return 0;
}

static SLCONST char *check_color_for_digit_form (SLCONST char *color)
{
   unsigned int i, ich;
   unsigned char *s = (unsigned char *) color;

   i = 0;
   while ((ich = (unsigned int) *s) != 0)
     {
	if ((ich < '0') || (ich > '9'))
	  return color;

	i = i * 10 + (ich - '0');
	s++;
     }

   if (i < MAX_COLOR_NAMES)
     color = Color_Defs[i].name;

   return color;
}

static int get_default_colors (SLCONST char **fgp, SLCONST char **bgp)
{
   static char fg_buf[16], bg_buf[16];
   static SLCONST char *bg, *fg;
   static int already_parsed;
   char *p, *pmax;

   if (already_parsed == -1)
     return -1;

   if (already_parsed)
     {
	*fgp = fg;
	*bgp = bg;
	return 0;
     }

   already_parsed = -1;

   bg = getenv ("COLORFGBG");

   if (bg == NULL)
     {
	bg = getenv ("DEFAULT_COLORS");
	if (bg == NULL)
	  return -1;
     }

   p = fg_buf;
   pmax = p + (sizeof (fg_buf) - 1);

   while ((*bg != 0) && (*bg != ';'))
     {
	if (p < pmax) *p++ = *bg;
	bg++;
     }
   *p = 0;

   if (*bg) bg++;

   p = bg_buf;
   pmax = p + (sizeof (bg_buf) - 1);

   /* Mark suggested allowing for extra application specific stuff following
    * the background color.  That is what the check for the semi-colon is for.
    */
   while ((*bg != 0) && (*bg != ';'))
     {
	if (p < pmax) *p++ = *bg;
	bg++;
     }
   *p = 0;

   if (!strcmp (fg_buf, "default") || !strcmp(bg_buf, "default"))
     {
	*fgp = *bgp = fg = bg = "default";
     }
   else
     {
	*fgp = fg = check_color_for_digit_form (fg_buf);
	*bgp = bg = check_color_for_digit_form (bg_buf);
     }
   already_parsed = 1;
   return 0;
}

static int Color_0_Modified = 0;

int SLtt_set_color_object (int obj, SLtt_Char_Type attr)
{
   Brush_Info_Type *b;

   if (NULL == (b = get_brush_info (obj)))
     return -1;

   b->fgbg = attr;
   if (obj == 0) Color_0_Modified = 1;

   if (_pSLtt_color_changed_hook != NULL)
     (*_pSLtt_color_changed_hook)();

   return 0;
}

SLtt_Char_Type SLtt_get_color_object (int obj)
{
   return get_brush_fgbg (obj);
}

int SLtt_add_color_attribute (int obj, SLtt_Char_Type attr)
{
   Brush_Info_Type *b;

   if (NULL == (b = get_brush_info (obj)))
     return -1;

   b->fgbg |= (attr & ATTR_MASK);

   if (obj == 0) Color_0_Modified = 1;
   if (_pSLtt_color_changed_hook != NULL)
     (*_pSLtt_color_changed_hook)();

   return 0;
}

static SLtt_Char_Type fb_to_fgbg (SLtt_Char_Type f, SLtt_Char_Type b)
{
   SLtt_Char_Type attr;

   if ((Max_Terminfo_Colors != 8)
#if SLTT_HAS_TRUECOLOR_SUPPORT
       || Has_True_Color
#endif
      )
     {
	if ((f != SLSMG_COLOR_DEFAULT) && (0 == IS_TRUE_COLOR(f)))
	  f %= Max_Terminfo_Colors;
	if ((b != SLSMG_COLOR_DEFAULT) && (0 == IS_TRUE_COLOR(b)))
	  b %= Max_Terminfo_Colors;
	return MAKE_COLOR(f,b);
     }

   /* Otherwise we have 8 ansi colors.  Try to get bright versions
    * by using the BOLD and BLINK attributes.
    */

   attr = 0;

   /* Note:  If f represents default, it will have the value 0xFF */
   if (f != SLSMG_COLOR_DEFAULT)
     {
	if (f & 0x8) attr = SLTT_BOLD_MASK;
	f &= 0x7;
     }

   if (b != SLSMG_COLOR_DEFAULT)
     {
	if (b & 0x8) attr |= SLTT_BLINK_MASK;
	b &= 0x7;
     }

   return MAKE_COLOR(f,b) | attr;
}

#if SLTT_HAS_TRUECOLOR_SUPPORT
static int parse_hex_digit (char ch)
{
   if (('0' <= ch) && (ch <= '9')) return ch - '0';
   if (('A' <= ch) && (ch <= 'F')) return 10 + ch - 'A';
   if (('a' <= ch) && (ch <= 'f')) return 10 + ch - 'a';
   return -1;
}

static int parse_true_color (const char *color, SLtt_Char_Type *c)
{
   SLtt_Char_Type rgb;
   unsigned int i;
   int h[6];

   i = 0;
   while (i < 6)
     {
	if (-1 == (h[i] = parse_hex_digit (color[i])))
	  return -1;
	i++;
     }
   if (color[i] != 0)
     return -1;

   if (i == 3)			       /* RRGGBB */
     rgb = (h[0] << 20) | (h[0] << 16) | (h[1] << 12) | (h[1] << 8) | (h[2] << 4) | h[2];
   else if ((i == 6) && (color[i] == 0))
     rgb = (h[0] << 20) | (h[1] << 16) | (h[2] << 12) | (h[3] << 8) | (h[4] << 4) | h[5];
   else
     return -1;

   *c = rgb | TRUE_COLOR_BIT;
   return 0;
}
#endif

/* This looks for colors with name form 'colorN'.  If color is of this
 * form, N is passed back via parameter list.
 */
static int parse_color_digit_name (SLCONST char *color, SLtt_Char_Type *f)
{
   unsigned int i;

#if SLTT_HAS_TRUECOLOR_SUPPORT
   if (Has_True_Color && (color[0] == '#'))
     return parse_true_color (color+1, f);
#endif

   if (strncmp (color, "color", 5))
     return -1;

   color += 5;
   if (*color == 0)
     return -1;

   i = 0;
   while (1)
     {
	unsigned int j;
	unsigned char ch;

	ch = (unsigned char) *color++;
	if (ch == 0)
	  break;
	if ((ch > '9') || (ch < '0'))
	  return -1;

	if (i > 0xFFFFFFFFU / 10)
	  return -1;
	j = (i *= 10);
	i += (ch - '0');
	if (i < j)
	  return -1;
     }

   *f = (SLtt_Char_Type) i;
   return 0;
}

/* Here whitespace is not allowed.  That is, "blue;blink" is ok but
 * "blue; blink" or "blue ;blink" are not.
 */
static int parse_color_and_attributes (SLCONST char *f, char *buf, size_t buflen, SLtt_Char_Type *attrp)
{
   SLCONST char *s;
   unsigned int len;
   SLtt_Char_Type a;

   *attrp = a = 0;

   s = strchr (f, ';');
   if (s == NULL) return 0;

   len = s - f;
   if (len >= buflen) len = buflen-1;
   strncpy (buf, f, len);
   buf[len] = 0;

   while ((*s == ';') || (*s == ' ') || (*s == '\t')) s++;
   f = s;
   while (*f)
     {
	s = strchr (f, ';');
	if (s == NULL)
	  s = f + strlen (f);

	len = s - f;
	if (len)
	  {
	     if (0 == strncmp (f, "italic", 6))
	       a |= SLTT_ITALIC_MASK;
	     else if (0 == strncmp (f, "blink", 5))
	       a |= SLTT_BLINK_MASK;
	     else if (0 == strncmp (f, "underline", 9))
	       a |= SLTT_ULINE_MASK;
	     else if (0 == strncmp (f, "bold", 4))
	       a |= SLTT_BOLD_MASK;
	  }
	while ((*s == ';') || (*s == ' ') || (*s == '\t')) s++;
	f = s;
     }
   *attrp = a;
   return 1;
}

static int make_color_fgbg (SLCONST char *fg, SLCONST char *bg, SLtt_Char_Type *fgbg)
{
   SLtt_Char_Type f = INVALID_ATTR, b = INVALID_ATTR;
   SLCONST char *dfg, *dbg;
   unsigned int i;
   char bgbuf[16], fgbuf[16];
   SLtt_Char_Type fattr= 0, battr = 0;

   if ((fg != NULL) && (*fg == 0)) fg = NULL;
   if ((bg != NULL) && (*bg == 0)) bg = NULL;

   if ((fg == NULL) || (bg == NULL))
     {
	if (-1 == get_default_colors (&dfg, &dbg))
	  return -1;

	if (fg == NULL) fg = dfg;
	if (bg == NULL) bg = dbg;
     }

   if (1 == parse_color_and_attributes (fg, fgbuf, sizeof(fgbuf), &fattr))
     fg = fgbuf;

   if (-1 == parse_color_digit_name (fg, &f))
     {
	for (i = 0; i < MAX_COLOR_NAMES; i++)
	  {
	     if (strcmp(fg, Color_Defs[i].name)) continue;
	     f = Color_Defs[i].color;
	     break;
	  }
     }

   if (1 == parse_color_and_attributes (bg, bgbuf, sizeof(bgbuf), &battr))
     bg = bgbuf;

   if (-1 == parse_color_digit_name (bg, &b))
     {
	for (i = 0; i < MAX_COLOR_NAMES; i++)
	  {
	     if (strcmp(bg, Color_Defs[i].name)) continue;
	     b = Color_Defs[i].color;
	     break;
	  }
     }

   if ((f == INVALID_ATTR) || (b == INVALID_ATTR))
     return -1;

   *fgbg = fb_to_fgbg (f, b) | fattr | battr;
   return 0;
}

static int tt_set_color (int obj, SLCONST char *what, SLCONST char *fg, SLCONST char *bg)
{
   SLtt_Char_Type fgbg;

   (void) what;

   if (-1 == make_color_fgbg (fg, bg, &fgbg))
     return -1;

   return SLtt_set_color_object (obj, fgbg);
}

int SLtt_set_color (int obj, SLFUTURE_CONST char *what, SLFUTURE_CONST char *fg, SLFUTURE_CONST char *bg)
{
   return tt_set_color (obj, what, fg, bg);
}

int SLtt_set_color_fgbg (int obj, SLtt_Char_Type f, SLtt_Char_Type b)
{
   return SLtt_set_color_object (obj, fb_to_fgbg (f, b));
}

void SLtt_set_alt_char_set (int i)
{
   static int last_i;
   if (SLtt_Has_Alt_Charset == 0) return;

   i = (i != 0);

   if (i == last_i) return;
   tt_write_string (i ? Start_Alt_Chars_Str : End_Alt_Chars_Str );
   last_i = i;
}

#if SLTT_HAS_TRUECOLOR_SUPPORT
# if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wformat-nonliteral"
# endif
static void write_truecolor (const char *fmt, SLtt_Char_Type c)
{
   char tmpbuf[32];
   int r, g, b;

   r = (int)((c>>16) & 0xFF);
   g = (int)((c>>8) & 0xFF);
   b = (int)(c & 0xFF);

   sprintf (tmpbuf, fmt, r, g, b);
   tt_write_string (tmpbuf);
}
# if defined(__GNUC__)
#  pragma GCC diagnostic warning "-Wformat-nonliteral"
# endif
#endif				       /* SLTT_HAS_TRUECOLOR_SUPPORT */

static void write_attributes (SLtt_Char_Type fgbg)
{
   int unknown_attributes;

   if (Worthless_Highlight) return;
   if (fgbg == Current_Fgbg) return;

   unknown_attributes = 0;

   /* Before spitting out colors, fix attributes */
   if ((fgbg & ATTR_MASK) != (Current_Fgbg & ATTR_MASK))
     {
	if (Current_Fgbg & ATTR_MASK)
	  {
	     tt_write_string(Norm_Vid_Str);
	     /* In case normal video turns off ALL attributes: */
	     if (fgbg & SLTT_ALTC_MASK)
	       Current_Fgbg &= ~SLTT_ALTC_MASK;
	     SLtt_set_alt_char_set (0);
	  }

	if ((fgbg & SLTT_ALTC_MASK)
	    != (Current_Fgbg & SLTT_ALTC_MASK))
	  {
	     SLtt_set_alt_char_set ((int) (fgbg & SLTT_ALTC_MASK));
	  }

	if (fgbg & SLTT_ULINE_MASK) tt_write_string (UnderLine_Vid_Str);
	if (fgbg & SLTT_BOLD_MASK) SLtt_bold_video ();
	if (fgbg & SLTT_REV_MASK) tt_write_string (Rev_Vid_Str);
	if (fgbg & SLTT_ITALIC_MASK) tt_write_string (Italic_Vid_Str);
	if (fgbg & SLTT_BLINK_MASK)
	  {
	     /* Someday Linux will have a blink mode that set high intensity
	      * background.  Lets be prepared.
	      */
	     if (SLtt_Blink_Mode) tt_write_string (Blink_Vid_Str);
	  }
	unknown_attributes = 1;
     }

   if (SLtt_Use_Ansi_Colors)
     {
	int bg0, fg0;
	fg0 = (int) GET_FG(fgbg);
	bg0 = (int) GET_BG(fgbg);

	if (unknown_attributes
	    || (fg0 != (int)GET_FG(Current_Fgbg)))
	  {
	     if (fg0 == SLSMG_COLOR_DEFAULT)
	       tt_write_string (Default_Color_Fg_Str);
#if SLTT_HAS_TRUECOLOR_SUPPORT
	     else if (IS_TRUE_COLOR(fg0))
	       write_truecolor (Color_RGB_Fg_Str, fg0);
#endif
	     else
	       tt_printf (Color_Fg_Str, COLOR_ARG(fg0, Is_Fg_BGR), 0);
	  }

	if (unknown_attributes
	    || (bg0 != (int)GET_BG(Current_Fgbg)))
	  {
	     if (bg0 == SLSMG_COLOR_DEFAULT)
	       tt_write_string (Default_Color_Bg_Str);
#if SLTT_HAS_TRUECOLOR_SUPPORT
	     else if (IS_TRUE_COLOR(bg0))
	       write_truecolor (Color_RGB_Bg_Str, bg0);
#endif
	     else
	       tt_printf (Color_Bg_Str, COLOR_ARG(bg0, Is_Bg_BGR), 0);
	  }
     }

   Current_Fgbg = fgbg;
}

static int Video_Initialized;

void SLtt_reverse_video (int color)
{
   SLtt_Char_Type fgbg;

   if (Worthless_Highlight) return;

   if (Video_Initialized == 0)
     {
	if (color == JNORMAL_COLOR)
	  {
	     tt_write_string (Norm_Vid_Str);
	  }
	else tt_write_string (Rev_Vid_Str);
	Current_Fgbg = INVALID_ATTR;
	return;
     }

  fgbg = get_brush_attr (color);

   if (fgbg == Current_Fgbg) return;
   write_attributes (fgbg);
}

void SLtt_normal_video (void)
{
   SLtt_reverse_video(JNORMAL_COLOR);
}

void SLtt_narrow_width (void)
{
   tt_write ("\033[?3l", 5);
}

void SLtt_wide_width (void)
{
   tt_write ("\033[?3h", 5);
}

/* Highest bit represents the character set. */
#define COLOR_OF(a) ((a)->color & SLSMG_COLOR_MASK)

static int bce_colors_eq (SLsmg_Color_Type ca, SLsmg_Color_Type cb, int just_bg)
{
   Brush_Info_Type *ba, *bb;

   if (ca == cb)
     return 1;

   ba = get_brush_info (ca);
   bb = get_brush_info (cb);

   if (SLtt_Use_Ansi_Colors == 0)
     return ba->mono == bb->mono;

   if (Bce_Color_Offset)
     {
	/* If either are color 0, then we do not know what that means since the
	 * terminal does not support BCE
	 */
	if ((ca == 0) || (cb == 0))
	  return 0;
	ba = get_brush_info (ca-1);
	bb = get_brush_info (cb-1);
     }

   if (ba->fgbg == bb->fgbg)
     return 1;
   if (just_bg)
     {
	return (ba->mono == bb->mono)
	  && GET_BG(ba->fgbg) == GET_BG(bb->fgbg);
     }
   return 0;
}

/* The whole point of this routine is to prevent writing to the last column
 * and last row on terminals with automatic margins.
 */
static void write_string_with_care (SLCONST char *str)
{
   SLstrlen_Type len;

   if (str == NULL) return;

   len = strlen (str);
   if (Automatic_Margins && (Cursor_r + 1 == SLtt_Screen_Rows))
     {
	if (_pSLtt_UTF8_Mode == 0)
	 {
	   if (len + (unsigned int) Cursor_c >= (unsigned int) SLtt_Screen_Cols)
	     {
	        /* For now, just do not write there.  Later, something more
	        * sophisticated will be implemented.
	        */
	        if (SLtt_Screen_Cols > Cursor_c)
	          len = SLtt_Screen_Cols - Cursor_c - 1;
	        else
	          len = 0;
	     }
	 }
       else
	 {
	    SLstrlen_Type nchars = SLutf8_strlen((SLuchar_Type *)str, 1);
	    if (nchars + (unsigned int) Cursor_c >= (unsigned int) SLtt_Screen_Cols)
	     {
	       if (SLtt_Screen_Cols > Cursor_c)
	         {
		    char *p;
		    nchars = (SLstrlen_Type)(SLtt_Screen_Cols - Cursor_c - 1);
		    p = (char *)SLutf8_skip_chars((SLuchar_Type *) str, (SLuchar_Type *)(str + len), nchars, NULL, 1);
		    len = p - str;
		 }
	       else
		  len = 0;
	     }
	 }
     }
   tt_write (str, len);
}

static void send_attr_str (SLsmg_Char_Type *s, SLsmg_Char_Type *smax)
{
   unsigned char out[1+SLUTF8_MAX_MBLEN*SLSMG_MAX_CHARS_PER_CELL*SLTT_MAX_SCREEN_COLS];
   unsigned char *p, *pmax;
   register SLtt_Char_Type attr;
   SLsmg_Color_Type color, last_color = (SLsmg_Color_Type)-1;
   int dcursor_c;

   p = out;
   pmax = p + (sizeof (out)-1);

   if ((Cursor_c == 0)
       && (Use_Relative_Cursor_Addressing)
       && (Cursor_r < SLTT_MAX_SCREEN_ROWS))
     {
	if (s < smax)
	  Display_Start_Chars[Cursor_r] = *s;
	else
	  Display_Start_Chars[Cursor_r].nchars = 0;
     }

   dcursor_c = 0;
   while (s < smax)
     {
	SLwchar_Type wch;
	unsigned int nchars;

	if (0 == (nchars = s->nchars))
	  {
	     /* 2nd element of a char that occupies two columns */
	     s++;
	     if (_pSLtt_UTF8_Mode == 0)
	       *p++ = ' ';
	     dcursor_c++;
	     continue;
	  }

	color = s->color;

#if SLTT_HAS_NON_BCE_SUPPORT
	if (Bce_Color_Offset
	    && (color >= Bce_Color_Offset))
	  color -= Bce_Color_Offset;
#endif

	wch = s->wchars[0];

	if (color != last_color)
	  {
	     attr = get_brush_attr (color);

	     if (color & SLSMG_ACS_MASK) /* alternate char set */
	       {
		  if (SLtt_Use_Blink_For_ACS)
		    {
		       if (SLtt_Blink_Mode) attr |= SLTT_BLINK_MASK;
		    }
		  else attr |= SLTT_ALTC_MASK;
	       }

	     if (attr != Current_Fgbg)
	       {
		  if ((wch != ' ')
		      || (nchars > 1)
		      /* it is a space so only consider it different if it
		       * has different attributes.
		       */
		      || (attr != Current_Fgbg)
		      )
		    {
		       if (p != out)
			 {
			    *p = 0;
			    write_string_with_care ((char *) out);
			    p = out;
			    Cursor_c += dcursor_c;
			    dcursor_c = 0;
			 }
		       write_attributes (attr);
		       last_color = color;
		    }
	       }
	  }

	if ((wch < 0x80) && (nchars == 1))
	  *p++ = (unsigned char) wch;
	else if (_pSLtt_UTF8_Mode == 0)
	  {
	     if (wch > 255)
	       wch = '?';
	     else if (wch < (SLwchar_Type)SLsmg_Display_Eight_Bit)
	       wch = '?';
	     *p++ = (unsigned char) wch;
	  }
	else
	  {
	     unsigned int i;
	     for (i = 0; i < nchars; i++)
	       {
		  if (NULL == (p = SLutf8_encode (s->wchars[i], p, pmax-p)))
		    {
		       fprintf (stderr, "*** send_attr_str: buffer too small\n");
		       return;
		    }
	       }
	  }
	dcursor_c++;
	s++;
     }
   *p = 0;
   if (p != out) write_string_with_care ((char *) out);
   Cursor_c += dcursor_c;
}

static void forward_cursor (unsigned int n, int row)
{
   char buf [1024];

   /* if (Current_Fgbg & ~0xFF) */
   /*   { */
   /* 	unsigned int num = 0; */
   /* 	while (num < n) */
   /* 	  { */
   /* 	     write_string_with_care (" "); */
   /* 	     num++; */
   /* 	  } */
   /* 	Cursor_c += n; */
   /* 	return; */
   /*   } */

   if (n <= 4)
     {
	SLtt_normal_video ();
#if 0
	if (n >= sizeof (buf))
	  n = sizeof (buf) - 1;
#endif
	SLMEMSET (buf, ' ', n);
	buf[n] = 0;
	write_string_with_care (buf);
	Cursor_c += n;
     }
   else if (Curs_RightN_Str != NULL)
     {
	Cursor_c += n;
	n = tt_sprintf(buf, sizeof (buf), Curs_RightN_Str, (int) n, 0);
	tt_write(buf, n);
     }
   else SLtt_goto_rc (row, (int) (Cursor_c + n));
}

/* FIXME!!  If the terminal does not support color, then this route has
 * problems of color object 0 has been assigned some monochrome attribute
 * such as reverse video.  In such a case, space_char=' ' is not a simple
 * space character as is assumed below.
 */

#define COLORS_EQ(ca,cb) (((ca) == (cb)) || bce_colors_eq((ca), (cb), 0))
#define BG_COLORS_EQ(ca,cb) (((ca) == (cb)) || (bce_colors_eq((ca),(cb),1)))

#define COLORS_OF_EQ(a,b) COLORS_EQ(COLOR_OF(a),COLOR_OF(b))

#define CHARSET(a) ((a)->color&SLSMG_ACS_MASK)
#define CHAR_EQS(a, b) (((a)->nchars==(b)->nchars) \
			   && (((a)->nchars == 0) \
				  || ((((a)->wchars[0]==(b)->wchars[0]) \
					 && (0 == memcmp((a)->wchars, (b)->wchars, (a)->nchars*sizeof(SLwchar_Type)))) \
					 && (COLORS_OF_EQ(a,b)) \
					 && (CHARSET(a)==CHARSET(b)))))

#if 0
# define CHAR_EQS_SPACE(a) \
   (((a)->wchars[0]==' ') && ((a)->color==0) && ((a)->nchars==1))
#else
# define CHAR_EQS_SPACE(a) \
   (((a)->wchars[0]==' ') && ((a)->nchars==1) \
     && BG_COLORS_EQ(COLOR_OF(a),Bce_Color_Offset))
#endif

void SLtt_smart_puts(SLsmg_Char_Type *neww, SLsmg_Char_Type *oldd, int len, int row)
{
   register SLsmg_Char_Type *p, *q, *qmax, *pmax, *buf;
   SLsmg_Char_Type buffer[SLTT_MAX_SCREEN_COLS+1];
   SLsmg_Char_Type *space_match, *last_buffered_match;
#ifdef HP_GLITCH_CODE
   int handle_hp_glitch = 0;
#endif
   SLsmg_Char_Type *space_char;
   SLsmg_Char_Type space_char_buf;

#define SLTT_USE_INSERT_HACK 1
#if SLTT_USE_INSERT_HACK
   SLsmg_Char_Type *insert_hack_prev = NULL;
   SLsmg_Char_Type *insert_hack_char = NULL;

   if ((row + 1 == SLtt_Screen_Rows)
       && (len == SLtt_Screen_Cols)
       && (len > 1)
       && (SLtt_Term_Cannot_Insert == 0)
       && Automatic_Margins)
     {
	SLsmg_Char_Type *a, *b;
	insert_hack_char = &neww[len-1];

	a = oldd+(len-1);
	b = neww+(len-1);

	if (CHAR_EQS(a,b))
	  insert_hack_char = NULL;
	else
	  insert_hack_prev = &neww[len-2];
     }
#endif

   memset ((char *) &space_char_buf, 0, sizeof (SLsmg_Char_Type));
   space_char = &space_char_buf;
   space_char->nchars = 1;
   space_char->wchars[0] = ' ';

   if (len > SLTT_MAX_SCREEN_COLS)
     len = SLTT_MAX_SCREEN_COLS;

   q = oldd; p = neww;
   qmax = oldd + len;
   pmax = p + len;

   /* Find out where to begin --- while they match, we are ok */
   while (1)
     {
	if (q == qmax) return;
#if SLANG_HAS_KANJI_SUPPORT
# undef SLANG_HAS_KANJI_SUPPORT
# define SLANG_HAS_KANJI_SUPPORT 0
#endif
#if SLANG_HAS_KANJI_SUPPORT
	if (*p & 0x80)
	  { /* new is kanji */
	     if ((*q & 0x80) && ((q + 1) < qmax))
	       { /* old is also kanji */
		  if (((0xFF & *q) != (0xFF & *p))
		      || ((0xFF & q[1]) != (0xFF & p[1])))
		    break; /* both kanji, but not match */

		  else
		    { /* kanji match ! */
		       if (!COLORS_OF_EQ(*q, *p)) break;
		       q++; p++;
		       if (!COLORS_OF_EQ(*q, *p)) break;
		       /* really match! */
		       q++; p++;
		       continue;
		    }
	       }
	     else break; /* old is not kanji */
	  }
	else
	  { /* new is not kanji */
	     if (*q & 0x80) break; /* old is kanji */
	  }
#endif
	if (!CHAR_EQS(q, p)) break;
	q++; p++;
     }

#ifdef HP_GLITCH_CODE
   if (Has_HP_Glitch)
     {
	SLsmg_Char_Type *qq = q;

	SLtt_goto_rc (row, (int) (p - neww));

	while (qq < qmax)
	  {
	     if (qq->color)
	       {
		  SLtt_normal_video ();
		  SLtt_del_eol ();
		  qmax = q;
		  handle_hp_glitch = 1;
		  break;
	       }
	     qq++;
	  }
     }
#endif
   /* Find where the last non-blank character on old/new screen is */

   /* if (CHAR_EQS_SPACE(pmax-1)) */
   if (((pmax-1)->wchars[0]==' ') && ((pmax-1)->nchars==1))
     {
	/* If we get here, then we can erase to the end of the line to create
	 * the final space.  However, this will only work _if_ erasing will
	 * get us the correct color.  If the terminal supports BCE, then this
	 * is easy.  If it does not, then we can only perform this operation
	 * if the color is known via something like COLORFGBG.  For now,
	 * I just will not perform the optimization for such terminals.
	 */
	if (Can_Background_Color_Erase
	    && SLtt_Use_Ansi_Colors)
	  {
	     SLtt_Char_Type fgbg;

	     fgbg = get_brush_attr (COLOR_OF(pmax-1));
	     if (0 == (fgbg & ATTR_MASK))
	       space_char = pmax - 1;
	  }

	while (pmax > p)
	  {
	     pmax--;
	     if (!CHAR_EQS(pmax, space_char))
	       {
		  pmax++;
		  break;
	       }
	  }
     }

   while (qmax > q)
     {
	qmax--;
	if (!CHAR_EQS(qmax, space_char))
	  {
	     qmax++;
	     break;
	  }
     }

   last_buffered_match = buf = buffer;		       /* buffer is empty */

#ifdef HP_GLITCH_CODE
   if (handle_hp_glitch)
     {
	while (p < pmax)
	  {
	     *buf++ = *p++;
	  }
     }
#endif

#ifdef HP_GLITCH_CODE
   if (Has_HP_Glitch == 0)
     {
#endif
	/* Try use use erase to bol if possible */
	if ((Del_Bol_Str != NULL) && (CHAR_EQS_SPACE(neww)))
	  {
	     SLsmg_Char_Type *p1;
	     SLsmg_Color_Type blank_color = 0;

	     p1 = neww;
	     if ((Can_Background_Color_Erase)
		 && SLtt_Use_Ansi_Colors)
	       {
		  SLsmg_Char_Type *blank = p1;
		  blank_color = COLOR_OF(blank);
		  while ((p1 < pmax) && (CHAR_EQS (p1, blank)))
		    p1++;
	       }
	     else
	       {
		  /* black+white attributes do not support bce */
		  while ((p1 < pmax) && (CHAR_EQS_SPACE (p1)))
		    p1++;
	       }

	     /* Is this optimization worth it?  Assume Del_Bol_Str is ESC [ 1 K
	      * It costs 4 chars + the space needed to properly position the
	      * cursor, e.g., ESC [ 10;10H. So, it costs at least 13 characters.
	      */
	     if ((p1 > neww + 13)
		 && (p1 >= p)
		 /* Avoid erasing from the end of the line */
		 && ((p1 != pmax) || (pmax < neww + len)))
	       {
		  int ofs = (int) (p1 - neww);
		  q = oldd + ofs;
		  p = p1;
		  SLtt_goto_rc (row, ofs - 1);
		  SLtt_reverse_video (blank_color);
		  tt_write_string (Del_Bol_Str);
		  tt_write (" ", 1);
		  Cursor_c += 1;
	       }
	     else
	       SLtt_goto_rc (row, (int) (p - neww));
	  }
	else
	  SLtt_goto_rc (row, (int) (p - neww));
#ifdef HP_GLITCH_CODE
     }
#endif

   /* loop using overwrite then skip algorithm until done */
   while (1)
     {
	/* while they do not match and we do not hit a space, buffer them up */
	unsigned int n_spaces = 0;
	while (p < pmax)
	  {
	     if (CHAR_EQS_SPACE(q) && CHAR_EQS_SPACE(p))
	       {
		  /* If *q is not a space, we would have to overwrite it.
		   * However, if *q is a space, then while *p is also one,
		   * we only need to skip over the blank field.
		   */
		  space_match = p;
		  p++; q++;
		  while ((p < pmax)
			 && CHAR_EQS_SPACE(q)
			 && CHAR_EQS_SPACE(p))
		    {
		       p++;
		       q++;
		    }
		  n_spaces = (unsigned int) (p - space_match);
		  break;
	       }
#if SLANG_HAS_KANJI_SUPPORT
	     if ((*p & 0x80) && ((p + 1) < pmax))
	       { /* new is kanji */
		  if (*q & 0x80)
		    { /* old is also kanji */
		       if (((0xFF & *q) != (0xFF & *p))
			   || ((0xFF & q[1]) != (0xFF & p[1])))
			 {
			    /* both kanji, but not match */
			    *buf++ = *p++;
			    *buf++ = *p++;
			    q += 2;
			    continue;
			 }
		       else
			 { /* kanji match ? */
			    if (!COLORS_OF_EQ(*q, *p) || !COLORS_OF_EQ(*(q+1), *(p+1)))
			      {
				 /* code is match, but color is diff */
				 *buf++ = *p++;
				 *buf++ = *p++;
				 q += 2;
				 continue;
			      }
			    /* really match ! */
			    break;
			 }
		    }
 		  else
		    { /* old is not kanji */
		       *buf++ = *p++;
		       *buf++ = *p++;
		       q += 2;
		       continue;
		    }
	       }
	     else
	       { /* new is not kanji */
 		  if (*q & 0x80)
		    { /* old is kanji */
		       *buf++ = *p++;
		       q++;
		       continue;
		    }
	       }
#endif

	     if (CHAR_EQS(q, p))
	       {
		  /* Could be the second half of a double width character */
		  if (p->nchars || q->nchars)
		    break;
	       }
	     *buf++ = *p++;
	     q++;
	  }

	/* At this point, the buffer contains characters that do not match */
	if (buf != buffer) send_attr_str (buffer, buf);
	buf = buffer;

	if (n_spaces
	    && ((p < pmax) 	       /* erase to eol will achieve this effect*/
		|| (!CHAR_EQS_SPACE(space_char))))/* unless space_char is not a simple space */
	  {
	     forward_cursor (n_spaces, row);
	  }
	/* Now we overwrote what we could and cursor is placed at position
	 * of a possible match of new and old.  If this is the case, skip
	 * some more.
	 */

	/* Note that from here on, the buffer will contain matched characters */
#if !SLANG_HAS_KANJI_SUPPORT
	while ((p < pmax) && CHAR_EQS(p, q))
	  {
	     *buf++ = *p++;
	     q++;
	  }
#else
	/* Kanji */
	while (p < pmax)
	  {
	     if ((*p & 0x80) && ((p + 1) < pmax))
	       { /* new is kanji */
		  if (*q & 0x80)
		    { /* old is also kanji */
		       if (((0xFF & *q) == (0xFF & *p))
			   && ((0xFF & q[1]) == (0xFF & p[1])))
			 {
			    /* kanji match ? */
			    if (!COLORS_OF_EQ(*q, *p)
				|| !COLORS_OF_EQ(q[1], p[1]))
			      break;

			    *buf++ = *p++;
			    q++;
			    if (p >= pmax)
			      {
				 *buf++ = 32;
				 p++;
				 break;
			      }
			    else
			      {
				 *buf++ = *p++;
				 q++;
				 continue;
			      }
			 }
		       else break; /* both kanji, but not match */
		    }
		  else break; /* old is not kanji */
	       }
	     else
	       {  /* new is not kanji */
		  if (*q & 0x80) break; /* old is kanji */
		  if (!CHAR_EQS(*q, *p)) break;
		  *buf++ = *p++;
		  q++;
	       }
	  }
#endif
	last_buffered_match = buf;
	if (p >= pmax) break;

	/* jump to new position is it is greater than 5 otherwise
	 * let it sit in the buffer and output it later.
	 */
	if ((int) (buf - buffer) >= 5)
	  {
	     forward_cursor ((unsigned int) (buf - buffer), row);
	     last_buffered_match = buf = buffer;
	  }
     }

   /* At this point we have reached the end of the new string with the
    * exception of space_chars hanging off the end of it, but we may not have
    * reached the end of the old string if they did not match.
    */

   /* Here the buffer will consist only of characters that have matched */
   if (buf != buffer)
     {
	if (q < qmax)
	  {
	     if ((buf == last_buffered_match)
		 && ((int) (buf - buffer) >= 5))
	       {
		  forward_cursor ((unsigned int) (buf - buffer), row);
	       }
	     else
	       {
		  send_attr_str (buffer, buf);
	       }
	  }
     }

   if (q < qmax)
     {
	SLtt_reverse_video (COLOR_OF(space_char));
	del_eol ();
     }

#if SLTT_USE_INSERT_HACK
   else if (insert_hack_char != NULL)
     {
	SLtt_goto_rc (SLtt_Screen_Rows-1, SLtt_Screen_Cols-2);
	send_attr_str (insert_hack_char, insert_hack_char+1);
	SLtt_goto_rc (SLtt_Screen_Rows-1, SLtt_Screen_Cols-2);
	SLtt_begin_insert ();
	send_attr_str (insert_hack_prev, insert_hack_prev+1);
	SLtt_end_insert ();
     }
#endif

   if (Cursor_c >= SLtt_Screen_Cols)
     {
	if (Use_Relative_Cursor_Addressing)
	  {
	     /* Where is the cursor?  If we have automatic margins, then the
	      * cursor _may_ be at the beginning of the next row.  But, it may
	      * not if the terminal permits writing to the LR corner without
	      * scrolling.  In relative-cursor-addressing mode, we do not know.
	      *
	      * The trick is to force the cursor to the next line if
	      * it is at the end of the current line, or if it is at
	      * the beginning of the next do nothing to change that line.
	      * The only thing I can think of is to write the character that
	      * is already there.
	      */
	     Cursor_c = 0;
	     Cursor_r++;
	     if (Cursor_r < SLTT_MAX_SCREEN_ROWS)
	       {
		  SLsmg_Char_Type *c = Display_Start_Chars + Cursor_r;
		  if (c->nchars)
		    send_attr_str (c, c+1);
		  else
		    tt_write (" ", 1);
	       }
	     else
	       tt_write (" ", 1);
	     tt_write ("\r", 1);
	     Cursor_c =0;
	  }
	else if (Automatic_Margins)
	  Cursor_Set = 0;
     }
}

static void get_color_info (void)
{
   SLCONST char *fg, *bg;
   char *ct;

   ct = getenv ("COLORTERM");

   if (ct != NULL)
     {
	/* Allow easy mechanism to override inadequate termcap/terminfo files. */
	SLtt_Use_Ansi_Colors = 1;
#if SLTT_HAS_TRUECOLOR_SUPPORT
	if ((0 == strcmp(ct, "truecolor")) || (0 == strcmp(ct, "24bit")))
	  Has_True_Color = 1;
#endif
     }

   if (SLtt_Use_Ansi_Colors)
     Is_Color_Terminal = 1;

#if SLTT_HAS_NON_BCE_SUPPORT
   if (Can_Background_Color_Erase == 0)
     Can_Background_Color_Erase = (NULL != getenv ("COLORTERM_BCE"));
#endif

   /* terminfo does not support truecolor */

   if (-1 == get_default_colors (&fg, &bg))
     return;

   /* Check to see if application has already set them. */
   if (Color_0_Modified)
     return;

   tt_set_color (0, NULL, fg, bg);
   tt_set_color (1, NULL, bg, fg);
}

/* termcap stuff */

#ifdef __unix__

static int Termcap_Initialized = 0;

/* #define USE_TERMCAP 1 */
#ifdef USE_TERMCAP
/* Termcap based system */
static char Termcap_Buf[4096];
/* static char Termcap_String_Buf[4096]; */
/* static char *Termcap_String_Ptr; */
extern char *tgetstr(char *, char **);
extern int tgetent(char *, char *);
extern int tgetnum(char *);
extern int tgetflag(char *);
#else
/* Terminfo */
static SLterminfo_Type *Terminfo;
#endif

#define TGETFLAG(x) (tt_tgetflag(x) > 0)

static char *fixup_tgetstr (char *what)
{
   register char *w, *w1;
   char *wsave;

   if (what == NULL)
     return NULL;

   /* Check for AIX brain-damage */
   if (*what == '@')
     return NULL;

   /* lose pad info --- with today's technology, term is a loser if
    it is really needed */
   while ((*what == '.') ||
	  ((*what >= '0') && (*what <= '9'))) what++;
   if (*what == '*') what++;

   /* lose terminfo padding--- looks like $<...> */
   w = what;
   while (*w) if ((*w++ == '$') && (*w == '<'))
     {
	w1 = w - 1;
	while (*w && (*w != '>')) w++;
	if (*w == 0) break;
	w++;
	wsave = w1;
	while ((*w1++ = *w++) != 0)
	  ;
	w = wsave;
     }

   if (*what == 0) what = NULL;
   return what;
}

static char *tt_tgetstr (SLCONST char *cap)
{
   char *s;
#ifdef USE_TERMCAP
   char area_buf[4096];
   char *area;
#endif
   if (Termcap_Initialized == 0)
     return NULL;

#ifdef USE_TERMCAP
   /* tmp_area = &Termcap_String_Buf; */
   area = area_buf;
   s = tgetstr (cap, &area);
   if (area > area_buf + sizeof(area_buf))
     {
	SLang_exit_error ("\
The termcap tgetstr appears to have overflowed a buffer.\n\
The integrity of this program has been violated.\n");
     }
#else
   s = _pSLtt_tigetstr (Terminfo, cap);
#endif

   /* Do not strip pad info for alternate character set.  I need to make
    * this more general.
    */
   /* FIXME: Priority=low; */
   if (0 != strcmp (cap, "ac"))
     s = fixup_tgetstr (s);

#ifdef USE_TERMCAP
   if ((s >= area_buf) && (s < area_buf + sizeof(area_buf)))
     {
	/* It looks like tgetstr placed the object in the buffer and
	 * returned a pointer to that buffer.  So, we have to make a
	 * copy of it.
	 *
	 * Yes, this introduces a leak...
	 */
	s = SLmake_string (s);
     }
#endif
   return s;
}

char *SLtt_tgetstr (SLFUTURE_CONST char *cap)
{
   return tt_tgetstr (cap);
}

static int tt_tgetnum (SLCONST char *s)
{
   if (Termcap_Initialized == 0)
     return -1;
#ifdef USE_TERMCAP
   return tgetnum (s);
#else
   return _pSLtt_tigetnum (Terminfo, s);
#endif
}

int SLtt_tgetnum (SLFUTURE_CONST char *s)
{
   return tt_tgetnum (s);
}

static int tt_tgetflag (SLCONST char *s)
{
   if (Termcap_Initialized == 0)
     return -1;
#ifdef USE_TERMCAP
   return tgetflag (s);
#else
   return _pSLtt_tigetflag (Terminfo, s);
#endif
}

int SLtt_tgetflag (SLFUTURE_CONST char *s)
{
   return tt_tgetflag (s);
}

int SLtt_tgetent(char *term)
{
   return 0 == SLtt_initialize(term);
}

int SLtt_tputs(char *str, int affcnt, int (*p)(int))
{
   (void) affcnt;
   while (*str) (*p)(*str++);
   return 0;
}

char *SLtt_tgoto (char *cap, int col, int row)
{
   static char buf[64];
   /* beware of overflows. 2^64 is 20 bytes printed */
   if (strlen(cap) > 23)
     strcpy(buf, "capability too long");
   else
     tt_sprintf(buf, sizeof(buf), cap, row, col);
    return buf;
}

static int Vt100_Like = 0;

void SLtt_get_terminfo (void)
{
   char *term;
   int status;

   term = getenv ("TERM");
   if (term == NULL)
     SLang_exit_error("%s", "TERM environment variable needs set.");

   if (0 == (status = SLtt_initialize (term)))
     return;

   if (status == -1)
     {
	SLang_exit_error ("Unknown terminal: %s\n\
Check the TERM environment variable.\n\
Also make sure that the terminal is defined in the terminfo database.\n\
Alternatively, set the TERMCAP environment variable to the desired\n\
termcap entry.",
			  term);
     }

   if (status == -2)
     {
	SLang_exit_error ("\
Your terminal lacks the ability to clear the screen or position the cursor.\n");
     }
}

/* Returns 0 if all goes well, -1 if terminal capabilities cannot be deduced,
 * or -2 if terminal cannot position the cursor.
 */
int SLtt_initialize (SLFUTURE_CONST char *term)
{
   SLCONST char *t;
   char ch;
   int is_xterm;
   int almost_vtxxx;

   if (_pSLtt_UTF8_Mode == -1)
     _pSLtt_UTF8_Mode = _pSLutf8_mode;

   if (SLang_TT_Write_FD == -1)
     {
	/* Apparantly, this cannot fail according to the man pages. */
	SLang_TT_Write_FD = fileno (stdout);
     }

   if (term == NULL)
     {
	term = getenv ("TERM");
	if (term == NULL)
	  return -1;
     }

   if (_pSLsecure_issetugid ()
       && ((term[0] == '.') || (NULL != strchr(term, '/'))))
     return -1;

   Linux_Console = (!strncmp (term, "linux", 5)
# ifdef linux
		    || !strncmp(term, "con", 3)
# endif
		    );

   t = term;

   if (strcmp(t, "vt52") && (*t++ == 'v') && (*t++ == 't')
       && (ch = *t, (ch >= '1') && (ch <= '9'))) Vt100_Like = 1;

   is_xterm = ((0 == strncmp (term, "xterm", 5))
	       || (0 == strncmp (term, "rxvt", 4))
	       || (0 == strncmp (term, "Eterm", 5)));

   almost_vtxxx = (Vt100_Like
		   || Linux_Console
		   || is_xterm
		   || !strcmp (term, "screen"));

# ifndef USE_TERMCAP
   if (Terminfo != NULL)
     _pSLtt_tifreeent (Terminfo);

   Termcap_Initialized = 0;	       /* resetting things */

   if (NULL == (Terminfo = _pSLtt_tigetent (term)))
     {
	if (almost_vtxxx) /* Special cases. */
	  {
	     int vt102 = 1;
	     if (!strcmp (term, "vt100")) vt102 = 0;
	     get_color_info ();
   	     SLtt_set_term_vtxxx (&vt102);
	     (void) SLtt_get_screen_size ();
	     return 0;
	  }
	return -1;
     }
# else				       /* USE_TERMCAP */
   if (1 != tgetent(Termcap_Buf, term))
     return -1;
   /* Termcap_String_Ptr = Termcap_String_Buf; */
# endif				       /* NOT USE_TERMCAP */

   Termcap_Initialized = 1;

   Cls_Str = tt_tgetstr ("cl");
   Abs_Curs_Pos_Str = tt_tgetstr ("cm");

   if ((NULL == (Ins_Mode_Str = tt_tgetstr("im")))
       || ( NULL == (Eins_Mode_Str = tt_tgetstr("ei")))
       || ( NULL == (Del_Char_Str = tt_tgetstr("dc"))))
     SLtt_Term_Cannot_Insert = 1;

   Visible_Bell_Str = tt_tgetstr ("vb");

   Curs_Up_Str = tt_tgetstr ("up");
   Curs_UpN_Str = tt_tgetstr ("UP");

   /* Avoid \n for moving down because we cannot rely upon the resulting column */
   Curs_Dn_Str = tt_tgetstr ("do");
   if ((Curs_Dn_Str != NULL) && (*Curs_Dn_Str == '\n'))
     Curs_Dn_Str = NULL;

   Curs_DnN_Str = tt_tgetstr ("DO");
   Curs_Left_Str = tt_tgetstr ("le");
   if (Curs_Left_Str == NULL)
     {
	Curs_Left_Str = tt_tgetstr ("bc");
	if (Curs_Left_Str == NULL)
	  Curs_Left_Str = "\b";
     }
   Curs_LeftN_Str = tt_tgetstr ("LE");
   Curs_Right_Str = tt_tgetstr ("nd");
   Curs_RightN_Str = tt_tgetstr ("RI");
   Clear_EOS_Str = tt_tgetstr ("cd");

   Rev_Scroll_Str = tt_tgetstr("sr");
   Del_N_Lines_Str = tt_tgetstr("DL");
   Add_N_Lines_Str = tt_tgetstr("AL");

   /* Actually these are used to initialize terminals that use cursor
    * addressing.  Hard to believe.
    */
   Start_Abs_Cursor_Addressing_Mode = tt_tgetstr ("ti");
   End_Abs_Cursor_Addressing_Mode = tt_tgetstr ("te");

   /* If I do this for vtxxx terminals, arrow keys start sending ESC O A,
    * which I do not want.  This is mainly for HP terminals.
    */
   Keypad_Init_Str = tt_tgetstr ("ks");
   Keypad_Reset_Str = tt_tgetstr ("ke");
   if ((almost_vtxxx == 0) && (SLtt_Force_Keypad_Init == -1))
     SLtt_Force_Keypad_Init = 1;

   /* Make up for defective termcap/terminfo databases */
   if ((Vt100_Like && (term[2] != '1'))
       || Linux_Console
       || is_xterm
       )
     {
	if (Del_N_Lines_Str == NULL) Del_N_Lines_Str = "\033[%dM";
	if (Add_N_Lines_Str == NULL) Add_N_Lines_Str = "\033[%dL";
     }

   Scroll_R_Str = tt_tgetstr("cs");

   SLtt_get_screen_size ();

   if ((Scroll_R_Str == NULL)
       || (((NULL == Del_N_Lines_Str) || (NULL == Add_N_Lines_Str))
	   && (NULL == Rev_Scroll_Str)))
     {
	if (is_xterm
	    || Linux_Console
	    )
	  {
	     /* Defective termcap mode!!!! */
	     SLtt_set_term_vtxxx (NULL);
	  }
	else SLtt_Term_Cannot_Scroll = 1;
     }

   Del_Eol_Str = tt_tgetstr("ce");
   Del_Bol_Str = tt_tgetstr("cb");
   if (is_xterm && (Del_Bol_Str == NULL))
     Del_Bol_Str = "\033[1K";
   if (is_xterm && (Del_Eol_Str == NULL))
     Del_Eol_Str = "\033[K";

   Rev_Vid_Str = tt_tgetstr("mr");
   if (Rev_Vid_Str == NULL) Rev_Vid_Str = tt_tgetstr("so");

   Bold_Vid_Str = tt_tgetstr("md");

   /* Although xterm cannot blink, it does display the blinking characters
    * as bold ones.  Some Rxvt will display the background as high intensity.
    */
   if ((NULL == (Blink_Vid_Str = tt_tgetstr("mb")))
       && is_xterm)
     Blink_Vid_Str = "\033[5m";

   UnderLine_Vid_Str = tt_tgetstr("us");
   Italic_Vid_Str = "\033[3m";

   Start_Alt_Chars_Str = tt_tgetstr ("as");   /* smacs */
   End_Alt_Chars_Str = tt_tgetstr ("ae");   /* rmacs */
   Enable_Alt_Char_Set = tt_tgetstr ("eA");   /* enacs */
   SLtt_Graphics_Char_Pairs = tt_tgetstr ("ac");

   if (NULL == SLtt_Graphics_Char_Pairs)
     {
	/* make up for defective termcap/terminfo */
	if (Vt100_Like)
	  {
	     Start_Alt_Chars_Str = "\016";
	     End_Alt_Chars_Str = "\017";
	     Enable_Alt_Char_Set = "\033)0";
	  }
     }

    /* aixterm added by willi */
   if (is_xterm || !strncmp (term, "aixterm", 7))
     {
#if 0
	Start_Alt_Chars_Str = "\016";
	End_Alt_Chars_Str = "\017";
	Enable_Alt_Char_Set = "\033(B\033)0";
#else
	Start_Alt_Chars_Str = "\033(0";
	End_Alt_Chars_Str = "\033(B";
	Enable_Alt_Char_Set = "";
#endif
     }

   if ((SLtt_Graphics_Char_Pairs == NULL) &&
       ((Start_Alt_Chars_Str == NULL) || (End_Alt_Chars_Str == NULL)))
     {
	SLtt_Has_Alt_Charset = 0;
	Enable_Alt_Char_Set = NULL;
     }
   else SLtt_Has_Alt_Charset = 1;

#ifdef AMIGA
   Enable_Alt_Char_Set = Start_Alt_Chars_Str = End_Alt_Chars_Str = NULL;
#endif

   /* status line capabilities */
   if ((SLtt_Has_Status_Line == -1)
       && (0 != (SLtt_Has_Status_Line = TGETFLAG ("hs"))))
     {
	Disable_Status_line_Str = tt_tgetstr ("ds");
	Return_From_Status_Line_Str = tt_tgetstr ("fs");
	Goto_Status_Line_Str = tt_tgetstr ("ts");
	/* Status_Line_Esc_Ok = TGETFLAG("es"); */
	/* Num_Status_Line_Columns = tt_tgetnum ("ws"); */
	/* if (Num_Status_Line_Columns < 0) Num_Status_Line_Columns = 0; */
     }

   if (NULL == (Norm_Vid_Str = tt_tgetstr("me")))
     {
	Norm_Vid_Str = tt_tgetstr("se");
     }

   Cursor_Invisible_Str = tt_tgetstr("vi");
   Cursor_Visible_Str = tt_tgetstr("ve");

   Automatic_Margins = TGETFLAG ("am");
   /* No_Move_In_Standout = !TGETFLAG ("ms"); */
# ifdef HP_GLITCH_CODE
   Has_HP_Glitch = TGETFLAG ("xs");
# else
   Worthless_Highlight = TGETFLAG ("xs");
# endif

   if (Worthless_Highlight == 0)
     {				       /* Magic cookie glitch */
	Worthless_Highlight = (tt_tgetnum ("sg") > 0);
     }

   if (Worthless_Highlight)
     SLtt_Has_Alt_Charset = 0;

   Reset_Color_String = tt_tgetstr ("op");

   /* Apparantly the difference between "AF" and "Sf" is that AF uses RGB,
    * but Sf uses BGR.
    */
   Color_Fg_Str = tt_tgetstr ("AF"); /* ANSI setaf */
   if (Color_Fg_Str == NULL)
     {
	Color_Fg_Str = tt_tgetstr ("Sf");   /* setf */
	Is_Fg_BGR = (Color_Fg_Str != NULL);
     }
   Color_Bg_Str = tt_tgetstr ("AB"); /* ANSI setbf */
   if (Color_Bg_Str == NULL)
     {
	Color_Bg_Str = tt_tgetstr ("Sb");   /* setb */
	Is_Bg_BGR = (Color_Bg_Str != NULL);
     }

   if ((Max_Terminfo_Colors = tt_tgetnum ("Co")) < 0)
     Max_Terminfo_Colors = 8;

   if ((Color_Bg_Str != NULL) && (Color_Fg_Str != NULL))
     SLtt_Use_Ansi_Colors = 1;
   else
     {
#if 0
	Color_Fg_Str = "%?%p1%{7}%>%t\033[1;3%p1%{8}%m%dm%e\033[3%p1%dm%;";
	Color_Bg_Str = "%?%p1%{7}%>%t\033[5;4%p1%{8}%m%dm%e\033[4%p1%dm%;";
	Max_Terminfo_Colors = 16;
#else
	Color_Fg_Str = "\033[3%dm";
	Color_Bg_Str = "\033[4%dm";
	Max_Terminfo_Colors = 8;
#endif
     }

#if SLTT_HAS_NON_BCE_SUPPORT
   Can_Background_Color_Erase = TGETFLAG ("ut");   /* bce */
   /* Modern xterms have the BCE capability as well as the linux console */
   if (Can_Background_Color_Erase == 0)
     {
	Can_Background_Color_Erase = (Linux_Console
# if SLTT_XTERM_ALWAYS_BCE
				      || is_xterm
# endif
				      );
     }
#endif
   get_color_info ();

   TT_Is_Initialized = 1;

   if ((Cls_Str == NULL)
       || (Abs_Curs_Pos_Str == NULL))
     return -2;

   return 0;
}
#endif
/* Unix */

/* specific to vtxxx only */
void SLtt_enable_cursor_keys (void)
{
#ifdef __unix__
   if (Vt100_Like)
#endif
     tt_write_string("\033=\033[?1l");
}

#ifdef VMS
int SLtt_initialize (char *term)
{
   SLtt_get_terminfo ();
   TT_Is_Initialized = 1;
   return 0;
}

void SLtt_get_terminfo ()
{
   int zero = 0;

   /* Apparantly, this cannot fail according to the man pages. */
   if (SLang_TT_Write_FD == -1)
     SLang_TT_Write_FD = fileno (stdout);

   Can_Background_Color_Erase = 0;

   Color_Fg_Str = "\033[3%dm";
   Color_Bg_Str = "\033[4%dm";
   Max_Terminfo_Colors = 8;

   get_color_info ();

   SLtt_set_term_vtxxx(&zero);
   Start_Alt_Chars_Str = "\016";
   End_Alt_Chars_Str = "\017";
   SLtt_Has_Alt_Charset = 1;
   SLtt_Graphics_Char_Pairs = "aaffgghhjjkkllmmnnooqqssttuuvvwwxx";
   Enable_Alt_Char_Set = "\033(B\033)0";
   SLtt_get_screen_size ();
}
#endif

/* This sets term for vt102 terminals it parameter vt100 is 0.  If vt100
 * is non-zero, set terminal appropriate for a only vt100
 * (no add line capability). */

void SLtt_set_term_vtxxx(int *vt100)
{
   Norm_Vid_Str = "\033[m";

   Scroll_R_Str = "\033[%i%d;%dr";
   Cls_Str = "\033[2J\033[H";
   Rev_Vid_Str = "\033[7m";
   Bold_Vid_Str = "\033[1m";
   Blink_Vid_Str = "\033[5m";
   UnderLine_Vid_Str = "\033[4m";
   Italic_Vid_Str = "\033[3m";
   Del_Eol_Str = "\033[K";
   Del_Bol_Str = "\033[1K";
   Rev_Scroll_Str = "\033M";

   Curs_Up_Str = "\033[A";
   Curs_Dn_Str = "\033[B";
   Curs_Right_Str = "\033[C";
   Curs_Left_Str = "\033[D";
   Curs_UpN_Str = "\033[%dA";
   Curs_DnN_Str = "\033[%dB";
   Curs_RightN_Str = "\033[%dC";
   Curs_LeftN_Str = "\033[%dD";

   Abs_Curs_Pos_Str = "\033[%i%d;%dH";
   if ((vt100 == NULL) || (*vt100 == 0))
     {
	Ins_Mode_Str = "\033[4h";
	Eins_Mode_Str = "\033[4l";
	Del_Char_Str =  "\033[P";
	Del_N_Lines_Str = "\033[%dM";
	Add_N_Lines_Str = "\033[%dL";
	SLtt_Term_Cannot_Insert = 0;
     }
   else
     {
	Del_N_Lines_Str = NULL;
	Add_N_Lines_Str = NULL;
	SLtt_Term_Cannot_Insert = 1;
     }
   SLtt_Term_Cannot_Scroll = 0;
   /* No_Move_In_Standout = 0; */
}

void SLtt_init_keypad (void)
{
   if (SLtt_Force_Keypad_Init > 0)
     {
	tt_write_string (Keypad_Init_Str);
	SLtt_flush_output ();
     }
}

void SLtt_deinit_keypad (void)
{
   if (SLtt_Force_Keypad_Init > 0)
     {
	tt_write_string (Keypad_Reset_Str);
	SLtt_flush_output ();
     }
}

int SLtt_init_video (void)
{
   /*   send_string_to_term("\033[?6h"); */
   /* relative origin mode */
   if (Use_Relative_Cursor_Addressing == 0)
     tt_write_string (Start_Abs_Cursor_Addressing_Mode);
   SLtt_init_keypad ();
   SLtt_reset_scroll_region();
   SLtt_end_insert();
   tt_write_string (Enable_Alt_Char_Set);
   Video_Initialized = 1;
   return 0;
}

int _pSLtt_init_cmdline_mode (void)
{
   if (TT_Is_Initialized == 0)
     {
	int status = SLtt_initialize (NULL);
	if (status < 0)
	  {
	     if (status == -1)
	       SLang_vmessage ("%s", "**WARNING: Unknown terminal capabilities.\n");
	     return 0;
	  }
     }
   /* We need to be able to use relative cursor addressing in this mode */
   if (((Curs_UpN_Str == NULL) && (Curs_Up_Str == NULL))
       || ((Curs_Dn_Str == NULL) && (Curs_DnN_Str == NULL))
       || ((Curs_Right_Str == NULL) && (Curs_RightN_Str == NULL))
       || ((Curs_Left_Str == NULL) && (Curs_LeftN_Str == NULL)))
     return 0;

   SLtt_Term_Cannot_Scroll = 1;
   SLtt_Use_Ansi_Colors = 0;
   Use_Relative_Cursor_Addressing = 1;
   return 1;
}

void _pSLtt_cmdline_mode_reset (void)
{
   Cursor_Set = 0;
   Cursor_r = Cursor_c = 0;
   Max_Relative_Cursor_r = 0;
   tt_write ("\r", 1);
   _pSLtt_clear_eos ();
}

int SLtt_reset_video (void)
{
   SLtt_goto_rc (SLtt_Screen_Rows - 1, 0);
   Cursor_Set = 0;
   SLtt_normal_video ();	       /* MSKermit requires this  */
   tt_write_string(Norm_Vid_Str);

   Current_Fgbg = INVALID_ATTR;
   SLtt_set_alt_char_set (0);
   if (SLtt_Use_Ansi_Colors)
     {
	if (Reset_Color_String == NULL)
	  {
	     SLtt_Char_Type attr;
	     if (-1 != make_color_fgbg (NULL, NULL, &attr))
	       write_attributes (attr);
	     else tt_write_string ("\033[0m\033[m");
	  }
	else tt_write_string (Reset_Color_String);
	Current_Fgbg = INVALID_ATTR;
     }
   SLtt_erase_line ();
   SLtt_deinit_keypad ();

   if (Use_Relative_Cursor_Addressing == 0)
     tt_write_string (End_Abs_Cursor_Addressing_Mode);

   if (Mouse_Mode == 1)
     SLtt_set_mouse_mode (0, 1);

   SLtt_flush_output ();
   Video_Initialized = 0;
   return 0;
}

void SLtt_bold_video (void)
{
   tt_write_string (Bold_Vid_Str);
}

int SLtt_set_mouse_mode (int mode, int force)
{
   if (force == 0)
     {
	char *term;

	if (NULL == (term = (char *) getenv("TERM"))) return -1;
	if (strncmp ("xterm", term, 5))
	  return -1;
     }

   Mouse_Mode = (mode != 0);

   if (mode)
     tt_write_string ("\033[?9h");
   else
     tt_write_string ("\033[?9l");

   return 0;
}

void SLtt_disable_status_line (void)
{
   if (SLtt_Has_Status_Line > 0)
     {
	tt_write_string (Disable_Status_line_Str);
	SLtt_flush_output ();
     }
}

int SLtt_write_to_status_line (SLFUTURE_CONST char *s, int col)
{
   if ((SLtt_Has_Status_Line <= 0)
       || (Goto_Status_Line_Str == NULL)
       || (Return_From_Status_Line_Str == NULL))
     return -1;

   tt_printf (Goto_Status_Line_Str, col, 0);
   tt_write_string (s);
   tt_write_string (Return_From_Status_Line_Str);
   return 0;
}

void SLtt_get_screen_size (void)
{
#ifdef VMS_SYSTEM
   int status, code;
   unsigned short chan;
   $DESCRIPTOR(dev_dsc, "SYS$INPUT:");
#endif
   int r = 0, c = 0;

#ifdef TIOCGWINSZ
   struct winsize wind_struct;

   do
     {
	if ((ioctl(1,TIOCGWINSZ,&wind_struct) == 0)
	    || (ioctl(0, TIOCGWINSZ, &wind_struct) == 0)
	    || (ioctl(2, TIOCGWINSZ, &wind_struct) == 0))
	  {
	     c = (int) wind_struct.ws_col;
	     r = (int) wind_struct.ws_row;
	     break;
	  }
     }
   while (errno == EINTR);

#endif

#ifdef VMS_SYSTEM
   status = sys$assign(&dev_dsc,&chan,0,0,0);
   if (status & 1)
     {
	code = DVI$_DEVBUFSIZ;
	status = lib$getdvi(&code, &chan,0, &c, 0,0);
	if (!(status & 1))
	  c = 80;
	code = DVI$_TT_PAGE;
	status = lib$getdvi(&code, &chan,0, &r, 0,0);
	if (!(status & 1))
	  r = 24;
	sys$dassgn(chan);
     }
#endif

   if (r <= 0)
     {
	char *s = getenv ("LINES");
	if (s != NULL) r = atoi (s);
     }

   if (c <= 0)
     {
	char *s = getenv ("COLUMNS");
	if (s != NULL) c = atoi (s);
     }

   if ((r <= 0) || (r > SLTT_MAX_SCREEN_ROWS)) r = 24;
   if ((c <= 0) || (c > SLTT_MAX_SCREEN_COLS)) c = 80;
   SLtt_Screen_Rows = r;
   SLtt_Screen_Cols = c;
}

#if SLTT_HAS_NON_BCE_SUPPORT
int _pSLtt_get_bce_color_offset (void)
{
   if ((SLtt_Use_Ansi_Colors == 0)
       || Can_Background_Color_Erase
       || SLtt_Use_Blink_For_ACS)      /* in this case, we cannot lose a color */
     Bce_Color_Offset = 0;
   else
     {
	SLtt_Char_Type fgbg = get_brush_fgbg (0);
	if (GET_BG(fgbg) == SLSMG_COLOR_DEFAULT)
	  Bce_Color_Offset = 0;
	else
	  Bce_Color_Offset = 1;
     }

   return Bce_Color_Offset;
}
#endif

int SLtt_utf8_enable (int mode)
{
   if (mode == -1)
     mode = _pSLutf8_mode;

   return _pSLtt_UTF8_Mode = mode;
}

int SLtt_is_utf8_mode (void)
{
   int mode = _pSLtt_UTF8_Mode;
   if (mode == -1)
     mode = _pSLutf8_mode;

   return mode;
}
