/*
Copyright (C) 2005-2017,2018 John E. Davis

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

/* The code in this file was adapted from jdl.  The GNU readline support
 * was provided by Mike Noble.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef __WIN32__
# include <windows.h>
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#include <string.h>
#include <slang.h>
#include "slsh.h"

#include <signal.h>
#include <errno.h>

#ifndef USE_GNU_READLINE
# define USE_GNU_READLINE 0
#endif

#define USE_SLANG_READLINE (!USE_GNU_READLINE)

#ifdef REAL_UNIX_SYSTEM
# define SYSTEM_SUPPORTS_SIGNALS 1
#else
# define SYSTEM_SUPPORTS_SIGNALS 0
#endif

#if USE_GNU_READLINE
# include <readline/readline.h>
# include <readline/history.h>
#endif

static int Use_Readline;
static int Slsh_Quit = 0;
static SLang_Load_Type *Readline_Load_Object;

typedef struct
{
#if USE_SLANG_READLINE
   SLrline_Type *rli;
#endif
   int output_newline;
}
Slsh_Readline_Type;

static Slsh_Readline_Type *Default_Readline_Info;

static void init_tty (void);
static void reset_tty (void);

#if USE_GNU_READLINE
static void gnu_rl_sigint_handler (int sig)
{
   (void) sig;
   rl_delete_text (0, rl_end);
   rl_point = rl_end = 0;
   fprintf (stdout, "\n");
   rl_forced_update_display ();
}

static void (*last_sig_sigint) (int);
static void init_tty (void)
{
   last_sig_sigint = SLsignal (SIGINT, gnu_rl_sigint_handler);
}

static void reset_tty (void)
{
   SLsignal (SIGINT, last_sig_sigint);
}

#else				       /* ifdef USE_GNU_READLINE */

static Slsh_Readline_Type *Active_Rline_Info;

# if SYSTEM_SUPPORTS_SIGNALS
static void (*last_sig_sigtstp) (int);

#  define USE_SIGTSTP_INTERRUPT 1
#  if USE_SIGTSTP_INTERRUPT
static int Want_Suspension = 0;
#  endif

static void suspend_slsh (void)
{
   reset_tty ();
   (void) SLang_run_hooks ("slsh_readline_suspend_before_hook", 0);
   kill (0, SIGSTOP);
   (void) SLang_run_hooks ("slsh_readline_suspend_after_hook", 0);
   init_tty ();
   if (Active_Rline_Info != NULL)
     {
	SLsig_block_signals ();
	SLrline_set_display_width (Active_Rline_Info->rli, SLtt_Screen_Cols);
	SLrline_redraw (Active_Rline_Info->rli);
	SLsig_unblock_signals ();
     }
}

static int sigtstp_interrupt_hook (VOID_STAR unused)
{
   (void) unused;
   if (Want_Suspension)
     {
	Want_Suspension = 0;
	suspend_slsh ();
     }
   return 0;
}

static void sig_sigtstp (int sig)
{
   sig = errno;
#  if USE_SIGTSTP_INTERRUPT
   Want_Suspension = 1;
#  else
   suspend_slsh ();
#  endif
   (void) SLsignal (sig, sig_sigtstp);
   errno = sig;
}

static void init_sigtstp (void)
{
   (void) SLang_add_interrupt_hook (sigtstp_interrupt_hook, NULL);
   last_sig_sigtstp = SLsignal (SIGTSTP, sig_sigtstp);
}
static void deinit_sigtstp (void)
{
   SLsignal (SIGTSTP, last_sig_sigtstp);
   (void) SLang_remove_interrupt_hook (sigtstp_interrupt_hook, NULL);
}

# endif				       /* SYSTEM_SUPPORTS_SIGNALS */

# ifdef SIGWINCH
static int Want_Window_Size_Change = 0;
static void sig_winch_handler (int sig)
{
   sig = errno;
   Want_Window_Size_Change = 1;
   SLsignal_intr (SIGWINCH, sig_winch_handler);
   errno = sig;
}

static int screen_size_changed_hook (VOID_STAR cd_unused)
{
   (void) cd_unused;
   if (Want_Window_Size_Change)
     {
	Want_Window_Size_Change = 0;
	if (Active_Rline_Info != NULL)
	  {
	     SLtt_get_screen_size ();
	     SLrline_set_display_width (Active_Rline_Info->rli, SLtt_Screen_Cols);
	  }
     }
   return 0;
}
# endif

static int add_sigwinch_handlers (void)
{
# ifdef SIGWINCH
   (void) SLang_add_interrupt_hook (screen_size_changed_hook, NULL);
   (void) SLsignal_intr (SIGWINCH, sig_winch_handler);
# endif
   return 0;
}

# ifdef REAL_UNIX_SYSTEM
/* This hook if a signal occurs while waiting for input. */
static int getkey_intr_hook (void)
{
   return SLang_handle_interrupt ();
}
# endif

static int TTY_Inited = 0;
static void init_tty (void)
{
   int abort_char = 3;

   TTY_Inited++;
   if (TTY_Inited > 1)
     return;

# if SYSTEM_SUPPORTS_SIGNALS
   SLsig_block_signals ();
   SLang_TT_Read_FD = fileno (stdin);
   init_sigtstp ();
# endif
# ifdef REAL_UNIX_SYSTEM
   abort_char = -1;		       /* determine from tty */
# endif

   if (-1 == SLang_init_tty (abort_char, 1, 1))   /* opost was 0 */
     {
# if SYSTEM_SUPPORTS_SIGNALS
	deinit_sigtstp ();
	SLsig_unblock_signals ();
# endif
	SLang_exit_error ("Error initializing terminal.");
     }

# ifdef REAL_UNIX_SYSTEM
   SLang_getkey_intr_hook = getkey_intr_hook;
# endif
   (void) add_sigwinch_handlers ();

   SLtt_get_screen_size ();

# if SYSTEM_SUPPORTS_SIGNALS
   SLtty_set_suspend_state (1);
   SLsig_unblock_signals ();
# endif
}

static void reset_tty (void)
{
   if (TTY_Inited == 0)
     return;

   TTY_Inited--;

   if (TTY_Inited)
     return;

# if SYSTEM_SUPPORTS_SIGNALS
   SLsig_block_signals ();
   deinit_sigtstp ();
# endif
   SLang_reset_tty ();
# if SYSTEM_SUPPORTS_SIGNALS
   SLsig_unblock_signals ();
# endif
}
#endif				       /* ifdef USE_GNU_READLINE else */

static void close_readline (void)
{
   if (Default_Readline_Info != NULL)
     {
#if USE_SLANG_READLINE
	SLrline_close (Default_Readline_Info->rli);
#endif
	SLfree ((char *) Default_Readline_Info);
	Default_Readline_Info = NULL;
     }
}

static void close_slsh_readline (Slsh_Readline_Type *sri)
{
   if (sri == NULL)
     return;
#if USE_SLANG_READLINE
   if (sri->rli != NULL)
     SLrline_close (sri->rli);
#endif
   SLfree ((char *) sri);
}

static Slsh_Readline_Type *open_slsh_readline (SLFUTURE_CONST char *name, unsigned int flags)
{
   Slsh_Readline_Type *sri;

   if (NULL == (sri = (Slsh_Readline_Type *)SLmalloc (sizeof (Slsh_Readline_Type))))
     return NULL;
   memset ((char *)sri, 0, sizeof (Slsh_Readline_Type));

#if USE_SLANG_READLINE
   if (name == NULL)
     sri->rli = SLrline_open (SLtt_Screen_Cols, flags);
   else
     sri->rli = SLrline_open2 (name, SLtt_Screen_Cols, flags);

   if (sri->rli == NULL)
     {
	SLfree ((char *)sri);
	return NULL;
     }
   sri->output_newline = 1;
#endif
   return sri;
}


static int open_readline (SLFUTURE_CONST char *name)
{
   unsigned int flags = SL_RLINE_BLINK_MATCH|SL_RLINE_USE_MULTILINE;
   close_readline ();
   Default_Readline_Info = open_slsh_readline (name, flags);
   if (Default_Readline_Info == NULL)
     return -1;
   return 0;
}

#if USE_GNU_READLINE
static void redisplay_dummy (void)
{
}
#endif

static char *read_with_no_readline (char *prompt, int noecho)
{
   char *line;
#ifdef REAL_UNIX_SYSTEM
   int stdin_is_noecho = 0;
#endif
   char buf[1024];
   char *b;

   fprintf (stdout, "%s", prompt); fflush (stdout);
   if (noecho)
     {
#ifdef REAL_UNIX_SYSTEM
	if (isatty (fileno(stdin)))
	  {
	     (void) SLsystem ("stty -echo");   /* yuk */
	     stdin_is_noecho = 1;
	  }
#endif
     }

   line = buf;
   while (NULL == fgets (buf, sizeof (buf), stdin))
     {
#ifdef EINTR
	if (errno == EINTR)
	  {
	     if (-1 == SLang_handle_interrupt ())
	       {
		  line = NULL;
		  break;
	       }
	     continue;
	  }
#endif
	line = NULL;
	break;
     }
#ifdef REAL_UNIX_SYSTEM
   if (stdin_is_noecho)
     (void) SLsystem ("stty echo");
#endif
   if (line == NULL)
     return NULL;

   /* Remove the final newline */
   b = line;
   while (*b && (*b != '\n'))
     b++;
   *b = 0;

   return SLmake_string (line);
}

static char *read_input_line (Slsh_Readline_Type *sri, char *prompt, int noecho)
{
   char *line;

   if (Use_Readline == 0)
     return read_with_no_readline (prompt, noecho);

#if SYSTEM_SUPPORTS_SIGNALS
   init_tty ();
#endif
#if USE_GNU_READLINE
   (void) sri;
   if (noecho == 0)
     rl_redisplay_function = rl_redisplay;
   else
     {
	/* FIXME: What is the proper way to implement this in GNU readline? */
	(void) fputs (prompt, stdout); (void) fflush (stdout);
	rl_redisplay_function = redisplay_dummy;
     }
   line = readline (prompt);
   rl_redisplay_function = rl_redisplay;
#else
   SLtt_get_screen_size ();
   SLrline_set_display_width (sri->rli, SLtt_Screen_Cols);
   (void) add_sigwinch_handlers ();
   Active_Rline_Info = sri;
   (void) SLrline_set_echo (sri->rli, (noecho == 0));
   line = SLrline_read_line (sri->rli, prompt, NULL);
   Active_Rline_Info = NULL;
#endif
#if SYSTEM_SUPPORTS_SIGNALS
   reset_tty ();
#endif
   if (sri->output_newline)
     fputs ("\r\n", stdout);
   fflush (stdout);
   return line;
}

static int save_input_line (Slsh_Readline_Type *rline, char *line)
{
   char *p;

   if (Use_Readline == 0)
     return 0;

   if (line == NULL)
     return 0;

   p = line;
   while ((*p == ' ') || (*p == '\t') || (*p == '\n'))
     p++;
   if (*p == 0)
     return 0;

#if USE_GNU_READLINE
   (void) rline;
   add_history(line);
   return 0;
#else
   return SLrline_save_line (rline->rli);
#endif
}

static SLang_Name_Type *Prompt_Hook = NULL;
static void set_prompt_hook (void)
{
   SLang_Name_Type *h;

   if (SLang_peek_at_stack () == SLANG_NULL_TYPE)
     {
	SLang_pop_null ();
	h = NULL;
     }
   else if (NULL == (h = SLang_pop_function ()))
     return;

   if (Prompt_Hook != NULL)
     SLang_free_function (Prompt_Hook);

   Prompt_Hook = h;
}

static void get_prompt_hook (void)
{
   if (Prompt_Hook == NULL)
     (void) SLang_push_null ();
   else
     (void) SLang_push_function (Prompt_Hook);
}

/* Returns a malloced value */
static char *get_input_line (SLang_Load_Type *x)
{
   char *line;
   int parse_level;
   int free_prompt = 0;
   char *prompt;

   parse_level = x->parse_level;
   if (Prompt_Hook != NULL)
     {
	if ((-1 == SLang_start_arg_list ())
	    || (-1 == SLang_push_int (parse_level))
	    || (-1 == SLang_end_arg_list ())
	    || (-1 == SLexecute_function (Prompt_Hook))
	    || (-1 == SLang_pop_slstring (&prompt)))
	  {
	     SLang_verror (SL_RunTime_Error, "Disabling prompt hook");
	     SLang_free_function (Prompt_Hook);
	     Prompt_Hook = NULL;
	     return NULL;
	  }
	free_prompt = 1;
     }
   else if (parse_level == 0)
     prompt = (char *) "slsh> ";
   else
     prompt = (char *) "       ";

   if (parse_level == 0)
     {
	if (-1 == SLang_run_hooks ("slsh_interactive_before_hook", 0))
	  {
	     if (free_prompt)
	       SLang_free_slstring (prompt);
	     return NULL;
	  }
     }

   line = read_input_line (Default_Readline_Info, prompt, 0);

   if (free_prompt)
     SLang_free_slstring (prompt);

   if ((line == NULL)
       && (parse_level == 0)
       && (SLang_get_error() == 0))
     {
	Slsh_Quit = 1;
	return NULL;
     }

   if (line == NULL)
     {
	return NULL;
     }

   /* This hook is used mainly for logging input */
   (void) SLang_run_hooks ("slsh_interactive_after_hook", 1, line);

   (void) save_input_line (Default_Readline_Info, line);

   return line;
}

static char *read_using_readline (SLang_Load_Type *x)
{
   char *s;
   static char *last_s;

   if (last_s != NULL)
     {
	SLfree (last_s);
	last_s = NULL;
     }

   if (SLang_get_error ())
     return NULL;

   SLKeyBoard_Quit = 0;

   s = get_input_line (x);

   if (s == NULL)
     return NULL;

   if ((x->parse_level == 0)
       && (1 == SLang_run_hooks ("slsh_interactive_massage_hook", 1, s)))
     {
	SLfree (s);
	if (-1 == SLpop_string (&s))
	  return NULL;
     }

   if (SLang_get_error ())
     {
	SLfree (s);
	return NULL;
     }

   last_s = s;
   return s;
}

static void enable_keyboard_interrupt (void)
{
   static int is_enabled = 0;

   if (is_enabled == 0)
     {
	(void) SLang_set_abort_signal (NULL);
	is_enabled = 1;
     }
}

static void close_interactive (void)
{
   close_readline ();

   if (Readline_Load_Object == NULL)
     return;

   SLdeallocate_load_type (Readline_Load_Object);
   Readline_Load_Object = NULL;
#if !SYSTEM_SUPPORTS_SIGNALS
   reset_tty ();
   fputs ("\r\n", stdout);
   fflush (stdout);
#endif
}

static int open_interactive (void)
{
   if (Use_Readline
       && (-1 == open_readline ("slsh")))
     return -1;

   if (NULL == (Readline_Load_Object = SLallocate_load_type ("<stdin>")))
     {
	if (Use_Readline)
	  close_readline ();
	return -1;
     }

   Readline_Load_Object->read = read_using_readline;
   Readline_Load_Object->auto_declare_globals = 1;

#if !SYSTEM_SUPPORTS_SIGNALS
   /* If the system does not support asynchronouse signals, then it may install
    * a SLang_Interrupt hook that checks for ^C, etc.  For that reason, the
    * tty must be initialized the whole time.
    */
   init_tty ();
#endif
   enable_keyboard_interrupt ();

   return 0;
}

static int init_readline (char *appname)
{
   static int inited = 0;

   if (inited)
     return 0;

#if USE_SLANG_READLINE
   if (Use_Readline == 0)
     {
	inited = 1;
	return 0;
     }
   if (-1 == SLrline_init (appname, NULL, NULL))
     return -1;
#endif

   inited = 1;
   return 0;
}

int slsh_use_readline (char *app_name, int use_readline, int is_interactive)
{
   Use_Readline = use_readline;

#if USE_SLANG_READLINE
   if (is_interactive)
     {
	if (-1 == init_readline (app_name))
	  return -1;
     }
#endif

   return 0;
}

int slsh_interactive (void)
{
   Slsh_Quit = 0;

   (void) SLang_add_cleanup_function (close_interactive);

   if (-1 == open_interactive ())
     return -1;

   (void) SLang_run_hooks ("slsh_interactive_hook", 0);

   while (Slsh_Quit == 0)
     {
	if (SLang_get_error ())
	  {
	     SLang_restart(1);
	     /* SLang_set_error (0); */
	  }

	SLKeyBoard_Quit = 0;
	SLang_load_object (Readline_Load_Object);
     }
   close_interactive ();

   return 0;
}

static Slsh_Readline_Type *Intrinsic_Rline_Info;

#if USE_SLANG_READLINE
static void close_intrinsic_readline (void)
{
   if (Intrinsic_Rline_Info != NULL)
     {
	SLrline_close (Intrinsic_Rline_Info->rli);
	SLfree ((char *) Intrinsic_Rline_Info);
	Intrinsic_Rline_Info = NULL;
     }
}
#endif

static int readline_intrinsic_internal (Slsh_Readline_Type *sri, char *prompt, int noecho)
{
   char *line;

   if (sri == NULL)
     sri = Intrinsic_Rline_Info;

#if USE_SLANG_READLINE
   if ((sri == NULL) && Use_Readline)
     {
	Intrinsic_Rline_Info = open_slsh_readline (NULL, SL_RLINE_BLINK_MATCH);
	if (Intrinsic_Rline_Info == NULL)
	  return -1;
	(void) SLang_add_cleanup_function (close_intrinsic_readline);
	sri = Intrinsic_Rline_Info;
     }
#endif
   enable_keyboard_interrupt ();

   line = read_input_line (sri, prompt, noecho);
   if (noecho == 0)
     (void) save_input_line (sri, line);
   (void) SLang_push_malloced_string (line);
   return 0;
}

static int Rline_Type_Id = 0;

static SLang_MMT_Type *pop_sri_type (Slsh_Readline_Type **srip)
{
   SLang_MMT_Type *mmt;

   if (NULL == (mmt = SLang_pop_mmt (Rline_Type_Id)))
     return NULL;
   if (NULL == (*srip = (Slsh_Readline_Type *)SLang_object_from_mmt (mmt)))
     {
	SLang_free_mmt (mmt);
	return NULL;
     }
   return mmt;
}

static void readline_intrinsic (char *prompt)
{
   Slsh_Readline_Type *sri = NULL;
   SLang_MMT_Type *mmt = NULL;

   if (SLang_Num_Function_Args == 2)
     {
	if (NULL == (mmt = pop_sri_type (&sri)))
	  return;
     }
   (void) readline_intrinsic_internal (sri, prompt, 0);

   if (mmt != NULL)
     SLang_free_mmt (mmt);
}

static void readline_noecho_intrinsic (char *prompt)
{
   Slsh_Readline_Type *sri = NULL;
   SLang_MMT_Type *mmt = NULL;

   if (SLang_Num_Function_Args == 2)
     {
	if (NULL == (mmt = pop_sri_type (&sri)))
	  return;
     }
   (void) readline_intrinsic_internal (sri, prompt, 1);
   if (mmt != NULL)
     SLang_free_mmt (mmt);
}

static void new_slrline_intrinsic (char *name)
{
   Slsh_Readline_Type *sri;
   SLang_MMT_Type *mmt;

   if (NULL == (sri = open_slsh_readline (name, SL_RLINE_BLINK_MATCH)))
     return;

   if (NULL == (mmt = SLang_create_mmt (Rline_Type_Id, (VOID_STAR) sri)))
     {
	close_slsh_readline (sri);
	return;
     }

   if (-1 == SLang_push_mmt (mmt))
     SLang_free_mmt (mmt);
}

static void init_readline_intrinsic (char *appname)
{
   (void) init_readline (appname);
}

/* The values returned by this function are meaningful only after readline
 * has been used once.
 */
static void get_screen_size (void)
{
   int rows, cols;

#if USE_SLANG_READLINE
   rows = SLtt_Screen_Rows;
   cols = SLtt_Screen_Cols;
#else
   rl_get_screen_size (&rows, &cols);
#endif

   (void) SLang_push_int (rows);
   (void) SLang_push_int (cols);
}

#if USE_SLANG_READLINE
typedef struct
{
   SLang_MMT_Type *mmt;
   Slsh_Readline_Type *sri;
   SLang_Name_Type *update_hook;
   SLang_Any_Type *cd;
   SLang_Name_Type *clear_cb;
   SLang_Name_Type *preread_cb;
   SLang_Name_Type *postread_cb;
   SLang_Name_Type *width_cb;
}
Rline_CB_Type;

static int call_simple_update_cb (SLang_Name_Type *f, Rline_CB_Type *cb, int *opt)
{
   if (f == NULL)
     return 0;

   if (-1 == SLang_start_arg_list ())
     return -1;
   if ((-1 == SLang_push_mmt (cb->mmt))
       || ((opt != NULL) && (-1 == SLang_push_int (*opt)))
       || ((cb->cd != NULL) && (-1 == SLang_push_anytype (cb->cd))))
     {
	(void) SLang_end_arg_list ();
	return -1;
     }
   return SLexecute_function (f);
}

static void rline_update_clear_cb (SLrline_Type *rli, VOID_STAR cd)
{
   Rline_CB_Type *cb = (Rline_CB_Type *)cd;
   (void) rli;
   (void) call_simple_update_cb (cb->clear_cb, cb, NULL);
}

static void rline_update_preread_cb (SLrline_Type *rli, VOID_STAR cd)
{
   Rline_CB_Type *cb = (Rline_CB_Type *)cd;
   (void) rli;
   (void) call_simple_update_cb (cb->preread_cb, cb, NULL);
}

static void rline_update_postread_cb (SLrline_Type *rli, VOID_STAR cd)
{
   Rline_CB_Type *cb = (Rline_CB_Type *)cd;
   (void) rli;
   (void) call_simple_update_cb (cb->postread_cb, cb, NULL);
}

static void rline_update_width_cb (SLrline_Type *rli, int w, VOID_STAR cd)
{
   Rline_CB_Type *cb = (Rline_CB_Type *)cd;
   (void) rli;
   (void) call_simple_update_cb (cb->width_cb, cb, &w);
}

static void free_cb_info (Rline_CB_Type *cb)
{
   if (cb == NULL)
     return;
   if (cb->mmt != NULL) SLang_free_mmt (cb->mmt);
   if (cb->update_hook != NULL) SLang_free_function (cb->update_hook);
   if (cb->clear_cb != NULL) SLang_free_function (cb->clear_cb);
   if (cb->preread_cb != NULL) SLang_free_function (cb->preread_cb);
   if (cb->postread_cb != NULL) SLang_free_function (cb->postread_cb);
   if (cb->width_cb != NULL) SLang_free_function (cb->width_cb);
   if (cb->cd != NULL) SLang_free_anytype (cb->cd);
   SLfree ((char *)cb);
}

static void rline_call_update_hook (SLrline_Type *rli,
				    SLFUTURE_CONST char *prompt,
				    SLFUTURE_CONST char *buf,
				    unsigned int len,
				    unsigned int point, VOID_STAR cd)
{
   Rline_CB_Type *cb;

   (void) rli; (void) len;
   cb = (Rline_CB_Type *)cd;

   if (-1 == SLang_start_arg_list ())
     return;

   if ((-1 == SLang_push_mmt (cb->mmt))
       || (-1 == SLang_push_string (prompt))
       || (-1 == SLang_push_string (buf))
       || (-1 == SLang_push_int ((int) point))
       || ((cb->cd != NULL) && (-1 == SLang_push_anytype (cb->cd))))
     {
	(void) SLang_end_arg_list ();
	return;
     }

   (void) SLexecute_function (cb->update_hook);
}

static void free_rli_update_data_cb (SLrline_Type *rli, VOID_STAR cd)
{
   (void) rli;
   if (cd != NULL)
     free_cb_info ((Rline_CB_Type *)cd);
}

static void set_rline_update_hook (void)
{
   Rline_CB_Type *cb;
   SLrline_Type *rli;

   if (NULL == (cb = (Rline_CB_Type *)SLmalloc(sizeof(Rline_CB_Type))))
     return;
   memset ((char *)cb, 0, sizeof(Rline_CB_Type));

   switch (SLang_Num_Function_Args)
     {
      default:
	SLang_verror (SL_Usage_Error, "Usage: rline_set_update_hook (rli [,&hook [,clientdata]]);");
	return;

      case 3:
	if (-1 == SLang_pop_anytype (&cb->cd))
	  return;
	/* drop */
      case 2:
	if (NULL == (cb->update_hook = SLang_pop_function ()))
	  goto free_and_return;
	/* drop */
      case 1:
	if (NULL == (cb->mmt = pop_sri_type (&cb->sri)))
	  goto free_and_return;
     }

   cb->sri->output_newline = 0;
   rli = cb->sri->rli;

   SLrline_set_update_clear_cb (rli, rline_update_clear_cb);
   SLrline_set_update_preread_cb (rli, rline_update_preread_cb);
   SLrline_set_update_postread_cb (rli, rline_update_postread_cb);
   SLrline_set_update_width_cb (rli, rline_update_width_cb);

   if (0 == SLrline_set_update_hook (rli, rline_call_update_hook, (VOID_STAR)cb))
     {
	SLrline_set_free_update_cb (rli, free_rli_update_data_cb);
	return;
     }

   /* drop */
free_and_return:
   free_cb_info (cb);
}

/* On stack: (rli, callback) */
static int pop_set_rline_cb_args (SLang_MMT_Type **mmtp,
				  Rline_CB_Type **cbp, SLang_Name_Type **ntp)
{
   SLang_Name_Type *nt;
   Slsh_Readline_Type *sri;
   SLang_MMT_Type *mmt;

   if (SLang_peek_at_stack () == SLANG_NULL_TYPE)
     nt = NULL;
   else if (NULL == (nt = SLang_pop_function ()))
     return -1;

   if (NULL == (mmt = pop_sri_type (&sri)))
     {
	if (nt != NULL)
	  SLang_free_function (nt);
	return -1;
     }
   if (-1 == SLrline_get_update_client_data (sri->rli, (VOID_STAR *)cbp))
     goto return_error;

   if (*cbp == NULL)
     {
	SLang_verror (SL_Application_Error, "\
Attempt to define an rline update callback without first creating a readline_update_hook");
	goto return_error;
     }

   *mmtp = mmt;
   *ntp = nt;
   return 0;

return_error:
   SLang_free_mmt (mmt);
   if (nt == NULL) SLang_free_function (nt);
   return -1;
}

#define SET_RLINE_UPDATE_XXX_CB(fun, _xxx) \
   static void fun (void) \
   { \
      SLang_MMT_Type *mmt; \
      SLang_Name_Type *nt; \
      Rline_CB_Type *cb; \
      \
      if (-1 == pop_set_rline_cb_args (&mmt, &cb, &nt)) \
	return; \
      SLang_free_function (cb->_xxx); \
      cb->_xxx = nt; \
      SLang_free_mmt (mmt); \
   }

SET_RLINE_UPDATE_XXX_CB (set_rline_update_clear_cb, clear_cb)
SET_RLINE_UPDATE_XXX_CB (set_rline_update_preread_cb, preread_cb)
SET_RLINE_UPDATE_XXX_CB (set_rline_update_postread_cb, postread_cb)
SET_RLINE_UPDATE_XXX_CB (set_rline_update_width_cb, width_cb)
#endif				       /* USE_SLANG_READLINE */

static SLang_Intrin_Fun_Type Intrinsics [] =
{
   MAKE_INTRINSIC_0("__rline_init_tty", init_tty, VOID_TYPE),
   MAKE_INTRINSIC_0("__rline_reset_tty", reset_tty, VOID_TYPE),
   MAKE_INTRINSIC_S("slsh_readline_init", init_readline_intrinsic, VOID_TYPE),
   MAKE_INTRINSIC_S("slsh_readline_new", new_slrline_intrinsic, VOID_TYPE),
   MAKE_INTRINSIC_S("slsh_readline", readline_intrinsic, VOID_TYPE),
   MAKE_INTRINSIC_S("slsh_readline_noecho", readline_noecho_intrinsic, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_set_prompt_hook", set_prompt_hook, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_get_prompt_hook", get_prompt_hook, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_get_screen_size", get_screen_size, VOID_TYPE),
#if USE_SLANG_READLINE
   MAKE_INTRINSIC_0("slsh_set_readline_update_hook", set_rline_update_hook, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_set_update_clear_cb", set_rline_update_clear_cb, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_set_update_preread_cb", set_rline_update_preread_cb, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_set_update_postread_cb", set_rline_update_postread_cb, VOID_TYPE),
   MAKE_INTRINSIC_0("slsh_set_update_width_cb", set_rline_update_width_cb, VOID_TYPE),
#endif
   SLANG_END_INTRIN_FUN_TABLE
};

static void destroy_rline (SLtype type, VOID_STAR f)
{
   (void) type;
   close_slsh_readline ((Slsh_Readline_Type *) f);
}

static int register_rline_type (void)
{
   SLang_Class_Type *cl;

   if (Rline_Type_Id != 0)
     return 0;

   if (NULL == (cl = SLclass_allocate_class ("RLine_Type")))
     return -1;

   if (-1 == SLclass_set_destroy_function (cl, destroy_rline))
     return -1;

   /* By registering as SLANG_VOID_TYPE, slang will dynamically allocate a
    * type.
    */
   if (-1 == SLclass_register_class (cl, SLANG_VOID_TYPE, sizeof (Slsh_Readline_Type*), SLANG_CLASS_TYPE_MMT))
     return -1;

   Rline_Type_Id = SLclass_get_class_id (cl);

   return 0;
}

int slsh_init_readline_intrinsics ()
{
   if (-1 == register_rline_type ())
     return -1;

   if (-1 == SLadd_intrin_fun_table (Intrinsics, NULL))
     return -1;

   return 0;
}

