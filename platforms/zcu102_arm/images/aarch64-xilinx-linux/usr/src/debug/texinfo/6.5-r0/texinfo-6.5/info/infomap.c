/* infomap.c -- keymaps for Info.
   $Id: infomap.c 7787 2017-05-15 16:34:53Z gavin $

   Copyright 1993, 1997, 1998, 1999, 2001, 2002, 2003, 2004, 2007,
   2008, 2011, 2012, 2013, 2014, 2015, 2016 Free Software Foundation, Inc.

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

   Originally written by Brian Fox. */

#include "info.h"
#include "doc.h"
#include "funs.h"
#include "session.h"
#include "terminal.h"
#include "variables.h"

void keymap_bind_keyseq (Keymap map, int *keyseq, KEYMAP_ENTRY *keyentry);

/* Return a new Keymap. */
Keymap
keymap_make_keymap (void)
{
  int i;
  Keymap keymap;

  keymap = (Keymap)xmalloc (KEYMAP_SIZE * sizeof (KEYMAP_ENTRY));

  for (i = 0; i < KEYMAP_SIZE; i++)
    {
      keymap[i].type = ISFUNC;
      keymap[i].value.function = NULL;
    }

  return keymap;
}

/* Record KEYSEQ, a sequence of keys terminated by 0, in the linked list of
   FUNCTION_KEYSEQ hanging off FUNCTION.  Label it with ROOTMAP so we know
   whether the key sequence is for main operation or for the echo area. */
static void
add_function_keyseq (InfoCommand *function, int *keyseq, Keymap rootmap)
{
  FUNCTION_KEYSEQ *ks, *k;
  int len;

  if (function == NULL ||
      function == InfoCmd (info_do_lowercase_version))
    return;

  /* If there is already a key sequence recorded for this key map,
     don't do anything. */
  for (k = function->keys; k; k = k->next)
    if (k->map == rootmap)
      return;

  ks = xmalloc (sizeof (FUNCTION_KEYSEQ));
  ks->next = function->keys;
  ks->map = rootmap;
  for (len = 0; keyseq[len]; len++);
  ks->keyseq = xmalloc ((len + 1) * sizeof (int));
  memcpy (ks->keyseq, keyseq, (len + 1) * sizeof (int));

  function->keys = ks;
}

/* Bind key sequence.  Don't override already bound key sequences. */
void
keymap_bind_keyseq (Keymap map, int *keyseq, KEYMAP_ENTRY *keyentry)
{
  Keymap m = map;
  int *s = keyseq;
  int c;

  if (!s || *s == 0)
    return;

  while ((c = *s++) != '\0')
    {
      switch (m[c].type)
        {
        case ISFUNC:
          if (m[c].value.function)
            return; /* There is a function here already. */

          if (*s != '\0')
            {
              m[c].type = ISKMAP;
              m[c].value.keymap = keymap_make_keymap ();
            }
          break;

        case ISKMAP:
          if (*s == '\0')
            return; /* The key sequence we were asked to bind is an initial
                       subsequence of an already-bound sequence. */
          break;
        }
      if (*s != '\0')
        {
          m = m[c].value.keymap;
        }
      else
        {
          add_function_keyseq (keyentry->value.function, keyseq, map);
          m[c] = *keyentry;
        }
    }

  return;
}


/* Initialize the standard info keymaps. */

Keymap info_keymap = NULL;
Keymap echo_area_keymap = NULL;

/* Make sure that we don't have too many command codes defined. */

#if A_NCOMMANDS > A_MAX_COMMAND + 1
#error "too many commands defined"
#endif

/* Initialize the keymaps from the .info keymap file. */

#define NUL     '\0'

static int default_emacs_like_info_keys[] =
{
  /* Favoured command bindings come first.  We want help to
     report q, not C-x C-c, etc.  */
  'H', NUL,                       A_info_get_help_window,
  'q', NUL,                       A_info_quit,
  KEY_UP_ARROW, NUL,              A_info_prev_line,
  KEY_DOWN_ARROW, NUL,            A_info_next_line,
  KEY_PAGE_UP, NUL,             A_info_scroll_backward,
  KEY_PAGE_DOWN, NUL,           A_info_scroll_forward,
  KEY_HOME, NUL,                  A_info_beginning_of_node,
  KEY_END, NUL,                   A_info_end_of_node,
  '{', NUL,                       A_info_search_previous,
  '}', NUL,                       A_info_search_next,
  CONTROL('g'), NUL,              A_info_abort_key,
  RET, NUL,                       A_info_select_reference_this_line,

  TAB, NUL,                       A_info_move_to_next_xref,
  LFD, NUL,                       A_info_select_reference_this_line,
  CONTROL('a'), NUL,              A_info_beginning_of_line,
  CONTROL('b'), NUL,              A_info_backward_char,
  CONTROL('e'), NUL,              A_info_end_of_line,
  CONTROL('f'), NUL,              A_info_forward_char,
  CONTROL('h'), NUL,              A_info_scroll_backward,
  CONTROL('l'), NUL,              A_info_redraw_display,
  CONTROL('n'), NUL,              A_info_next_line,
  CONTROL('p'), NUL,              A_info_prev_line,
  CONTROL('r'), NUL,              A_isearch_backward,
  CONTROL('s'), NUL,              A_isearch_forward,
  CONTROL('u'), NUL,              A_info_universal_argument,
  CONTROL('v'), NUL,              A_info_scroll_forward_page_only,
  SPC, NUL,                       A_info_scroll_forward,
  ',', NUL,                       A_info_next_index_match,
  '/', NUL,                       A_info_search,
  '0', NUL,                       A_info_last_menu_item,
  '1', NUL,                       A_info_menu_digit,
  '2', NUL,                       A_info_menu_digit,
  '3', NUL,                       A_info_menu_digit,
  '4', NUL,                       A_info_menu_digit,
  '5', NUL,                       A_info_menu_digit,
  '6', NUL,                       A_info_menu_digit,
  '7', NUL,                       A_info_menu_digit,
  '8', NUL,                       A_info_menu_digit,
  '9', NUL,                       A_info_menu_digit,
  '<', NUL,                       A_info_first_node,
  '=', NUL,                       A_info_display_file_info,
  '>', NUL,                       A_info_last_node,
  '?', NUL,                       A_info_search_backward,
  '[', NUL,                       A_info_global_prev_node,
  ']', NUL,                       A_info_global_next_node,
  'b', NUL,                       A_info_beginning_of_node,
  'd', NUL,                       A_info_dir_node,
  'e', NUL,                       A_info_end_of_node,
  'f', NUL,                       A_info_xref_item,
  'g', NUL,                       A_info_goto_node,
  'G', NUL,                       A_info_menu_sequence,
  'h', NUL,                       A_info_get_info_help_node,
  'i', NUL,                       A_info_index_search,
  'I', NUL,                       A_info_virtual_index,
  'l', NUL,                       A_info_history_node,
  'm', NUL,                       A_info_menu_item,
  'n', NUL,                       A_info_next_node,
  'O', NUL,                       A_info_goto_invocation_node,
  'p', NUL,                       A_info_prev_node,
  'r', NUL,                       A_info_xref_item,
  'R', NUL,                       A_info_toggle_regexp,
  's', NUL,                       A_info_search,
  'S', NUL,                       A_info_search_case_sensitively,
  't', NUL,                       A_info_top_node,
  'u', NUL,                       A_info_up_node,
  'x', NUL,                       A_info_delete_window,
  KEYMAP_META('0'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('1'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('2'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('3'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('4'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('5'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('6'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('7'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('8'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('9'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('-'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META(CONTROL('f')), NUL,        A_info_show_footnotes,
  KEYMAP_META(CONTROL('g')), NUL,        A_info_abort_key,
  KEYMAP_META(TAB), NUL,                 A_info_move_to_prev_xref,
  KEYMAP_META(CONTROL('v')), NUL,        A_info_scroll_other_window,
  KEYMAP_META('<'), NUL,                 A_info_beginning_of_node,
  KEYMAP_META('>'), NUL,                 A_info_end_of_node,
  KEYMAP_META('b'), NUL,                 A_info_backward_word,
  KEYMAP_META('f'), NUL,                 A_info_forward_word,
  KEYMAP_META('r'), NUL,                 A_info_move_to_window_line,
  KEYMAP_META('v'), NUL,                 A_info_scroll_backward_page_only,
  KEYMAP_META('x'), NUL,                 A_info_execute_command,
  KEYMAP_META('/'), NUL,                 A_info_tree_search,
  KEYMAP_META('}'), NUL,                 A_info_tree_search_next,
  KEYMAP_META('{'), NUL,                 A_info_tree_search_previous,

  CONTROL('x'), CONTROL('b'), NUL,        A_list_visited_nodes,
  CONTROL('x'), CONTROL('c'), NUL,        A_info_quit,
  CONTROL('x'), CONTROL('f'), NUL,        A_info_view_file,
  CONTROL('x'), CONTROL('g'), NUL,        A_info_abort_key,
  CONTROL('x'), CONTROL('v'), NUL,        A_info_view_file,
  CONTROL('x'), '0', NUL,         A_info_delete_window,
  CONTROL('x'), '1', NUL,         A_info_keep_one_window,
  CONTROL('x'), '2', NUL,         A_info_split_window,
  CONTROL('x'), '^', NUL,         A_info_grow_window,
  CONTROL('x'), 'b', NUL,         A_select_visited_node,
  CONTROL('x'), 'f', NUL,         A_info_all_files,
  CONTROL('x'), 'n', NUL,         A_info_search_next,
  CONTROL('x'), 'N', NUL,         A_info_search_previous,
  CONTROL('x'), 'o', NUL,         A_info_next_window,
  CONTROL('x'), 't', NUL,         A_info_tile_windows,
  CONTROL('x'), 'w', NUL,         A_info_toggle_wrap,

  KEY_RIGHT_ARROW, NUL,         A_info_forward_char,
  KEY_LEFT_ARROW, NUL,          A_info_backward_char,
  KEY_DELETE, NUL,              A_info_scroll_backward,
  
  ESC, KEY_PAGE_UP, NUL,        A_info_scroll_other_window_backward,
  ESC, KEY_PAGE_DOWN, NUL,      A_info_scroll_other_window,
  ESC, KEY_UP_ARROW, NUL,       A_info_prev_line,
  ESC, KEY_DOWN_ARROW, NUL,     A_info_next_line,
  ESC, KEY_RIGHT_ARROW, NUL,    A_info_forward_word,
  ESC, KEY_LEFT_ARROW, NUL,     A_info_backward_word,
  KEY_BACK_TAB, NUL,            A_info_move_to_prev_xref,
  
};


static int default_emacs_like_ea_keys[] =
{
  KEYMAP_META('0'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('1'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('2'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('3'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('4'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('5'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('6'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('7'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('8'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('9'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('-'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META(CONTROL('g')), NUL,        A_ea_abort,
  KEYMAP_META(CONTROL('v')), NUL,        A_ea_scroll_completions_window,
  KEYMAP_META('b'), NUL,                 A_ea_backward_word,
  KEYMAP_META('d'), NUL,                 A_ea_kill_word,
  KEYMAP_META('f'), NUL,                 A_ea_forward_word,
  KEYMAP_META('y'), NUL,                 A_ea_yank_pop,
  KEYMAP_META('?'), NUL,                 A_ea_possible_completions,
  KEYMAP_META(TAB), NUL,                 A_ea_tab_insert,
  KEYMAP_META(KEY_DELETE), NUL,                 A_ea_backward_kill_word,
  CONTROL('a'), NUL,              A_ea_beg_of_line,
  CONTROL('b'), NUL,              A_ea_backward,
  CONTROL('d'), NUL,              A_ea_delete,
  CONTROL('e'), NUL,              A_ea_end_of_line,
  CONTROL('f'), NUL,              A_ea_forward,
  CONTROL('g'), NUL,              A_ea_abort,
  ESC, NUL,                       A_ea_abort,
  CONTROL('h'), NUL,              A_ea_rubout,
  CONTROL('k'), NUL,              A_ea_kill_line,
  CONTROL('l'), NUL,              A_info_redraw_display,
  CONTROL('q'), NUL,              A_ea_quoted_insert,
  CONTROL('t'), NUL,              A_ea_transpose_chars,
  CONTROL('u'), NUL,              A_info_universal_argument,
  CONTROL('y'), NUL,              A_ea_yank,
  LFD, NUL,                       A_ea_newline,
  RET, NUL,                       A_ea_newline,
  SPC, NUL,                       A_ea_complete,
  TAB, NUL,                       A_ea_complete,
  '?', NUL,                       A_ea_possible_completions,
#ifdef __MSDOS__
  /* PC users will lynch me if I don't give them their usual DEL
     effect...  */
  KEY_DELETE, NUL,                       A_ea_delete,
#else
  KEY_DELETE, NUL,                       A_ea_rubout,
#endif
  CONTROL('x'), 'o', NUL,         A_info_next_window,
  CONTROL('x'), KEY_DELETE, NUL,         A_ea_backward_kill_line,

  KEY_RIGHT_ARROW, NUL,           A_ea_forward,
  KEY_LEFT_ARROW, NUL,            A_ea_backward,
  ESC, KEY_RIGHT_ARROW, NUL,   A_ea_forward_word,
  ESC, KEY_LEFT_ARROW, NUL,    A_ea_backward_word,
  KEY_HOME, NUL,                 A_ea_beg_of_line,
  KEY_END, NUL,                  A_ea_end_of_line,
  ESC, KEY_DELETE, NUL,  A_ea_backward_kill_word,
};


static int default_vi_like_info_keys[] =
{
  /* We want help to report q, not C-x C-c, etc.  */
  'q', NUL,                       A_info_quit,
  'x', NUL,                       A_info_delete_window,
  SPC, NUL,                       A_info_scroll_forward,
  '{', NUL,                       A_info_search_previous,
  '}', NUL,                       A_info_search_next,
  KEY_UP_ARROW, NUL,    A_info_up_line,
  KEY_DOWN_ARROW, NUL,  A_info_down_line,

  '0', NUL,                       A_info_add_digit_to_numeric_arg,
  '1', NUL,                       A_info_add_digit_to_numeric_arg,
  '2', NUL,                       A_info_add_digit_to_numeric_arg,
  '3', NUL,                       A_info_add_digit_to_numeric_arg,
  '4', NUL,                       A_info_add_digit_to_numeric_arg,
  '5', NUL,                       A_info_add_digit_to_numeric_arg,
  '6', NUL,                       A_info_add_digit_to_numeric_arg,
  '7', NUL,                       A_info_add_digit_to_numeric_arg,
  '8', NUL,                       A_info_add_digit_to_numeric_arg,
  '9', NUL,                       A_info_add_digit_to_numeric_arg,
  '-', NUL,                       A_info_add_digit_to_numeric_arg,
  TAB, NUL,                       A_info_move_to_next_xref,
  LFD, NUL,                       A_info_down_line,
  RET, NUL,                       A_info_down_line,
  CONTROL('a'), NUL,              A_info_beginning_of_line,
  CONTROL('b'), NUL,              A_info_scroll_backward_page_only,
  CONTROL('c'), NUL,              A_info_abort_key,
  CONTROL('d'), NUL,              A_info_scroll_half_screen_down,
  CONTROL('e'), NUL,              A_info_down_line,
  CONTROL('f'), NUL,              A_info_scroll_forward_page_only,
  CONTROL('g'), NUL,              A_info_display_file_info,
  CONTROL('k'), NUL,              A_info_up_line,
  CONTROL('l'), NUL,              A_info_redraw_display,
  CONTROL('n'), NUL,              A_info_down_line,
  CONTROL('p'), NUL,              A_info_up_line,
  CONTROL('r'), NUL,              A_info_redraw_display,
  CONTROL('s'), NUL,              A_isearch_forward,
  CONTROL('u'), NUL,              A_info_scroll_half_screen_up,
  CONTROL('v'), NUL,              A_info_scroll_forward_page_only,
  CONTROL('y'), NUL,              A_info_up_line,
  ',', NUL,                       A_info_next_index_match,
  '/', NUL,                       A_info_search,
  KEYMAP_META('0'), NUL,                 A_info_last_menu_item,
  KEYMAP_META('1'), NUL,                 A_info_menu_digit,
  KEYMAP_META('2'), NUL,                 A_info_menu_digit,
  KEYMAP_META('3'), NUL,                 A_info_menu_digit,
  KEYMAP_META('4'), NUL,                 A_info_menu_digit,
  KEYMAP_META('5'), NUL,                 A_info_menu_digit,
  KEYMAP_META('6'), NUL,                 A_info_menu_digit,
  KEYMAP_META('7'), NUL,                 A_info_menu_digit,
  KEYMAP_META('8'), NUL,                 A_info_menu_digit,
  KEYMAP_META('9'), NUL,                 A_info_menu_digit,
  '<', NUL,                       A_info_first_node,
  '>', NUL,                       A_info_last_node,
  '?', NUL,                       A_info_search_backward,
  '[', NUL,                       A_info_global_prev_node,
  ']', NUL,                       A_info_global_next_node,
  '\'', NUL,                      A_info_history_node,
  'b', NUL,                       A_info_scroll_backward_page_only,
  'd', NUL,                       A_info_scroll_half_screen_down,
  'e', NUL,                       A_info_down_line,
  'E', NUL,                       A_info_view_file,
  ':', 'e', NUL,                  A_info_view_file,
  'f', NUL,                       A_info_scroll_forward_page_only,
  'F', NUL,                       A_info_scroll_forward_page_only,
  'g', NUL,                       A_info_first_node,
  'G', NUL,                       A_info_last_node,
  'h', NUL,                       A_info_get_help_window,
  'H', NUL,                       A_info_get_help_window,
  'i', NUL,                       A_info_index_search,
  'I', NUL,                       A_info_goto_invocation_node,
  'j', NUL,                       A_info_next_line,
  'k', NUL,                       A_info_prev_line,
  'l', NUL,                       A_info_history_node,
  'm', NUL,                       A_info_menu_item,
  'n', NUL,                       A_info_search_next,
  ':', 'a', NUL,                  A_info_all_files,
  'N', NUL,                       A_info_search_previous,
  'O', NUL,                       A_info_goto_invocation_node,
  'p', NUL,                       A_info_prev_node,
  'Q', NUL,                       A_info_quit,
  ':', 'q', NUL,                  A_info_quit,
  ':', 'Q', NUL,                  A_info_quit,
  'Z', 'Z', NUL,                  A_info_quit,
  'r', NUL,                       A_info_redraw_display,
  'R', NUL,                       A_info_toggle_regexp,
  's', NUL,                       A_info_search,
  'S', NUL,                       A_info_search_case_sensitively,
  't', NUL,                       A_info_top_node,
  'u', NUL,                       A_info_scroll_half_screen_up,
  'w', NUL,                       A_info_scroll_backward_page_only_set_window,
  'y', NUL,                       A_info_up_line,
  'z', NUL,                       A_info_scroll_forward_page_only_set_window,
  KEYMAP_META(CONTROL('f')), NUL,         A_info_show_footnotes,
  KEYMAP_META(CONTROL('g')), NUL,         A_info_abort_key,
  KEYMAP_META(TAB), NUL,                  A_info_move_to_prev_xref,
  KEYMAP_META(SPC), NUL,                  A_info_scroll_forward_page_only,
  KEYMAP_META(CONTROL('v')), NUL,         A_info_scroll_other_window,
  KEYMAP_META('<'), NUL,                  A_info_beginning_of_node,
  KEYMAP_META('>'), NUL,                  A_info_end_of_node,
  KEYMAP_META('/'), NUL,                  A_info_search,
  KEYMAP_META('?'), NUL,                  A_info_search_backward,
  KEYMAP_META('b'), NUL,                  A_info_beginning_of_node,
  KEYMAP_META('d'), NUL,                  A_info_dir_node,
  KEYMAP_META('e'), NUL,                  A_info_end_of_node,
  KEYMAP_META('f'), NUL,                  A_info_xref_item,
  KEYMAP_META('g'), NUL,                  A_info_select_reference_this_line,
  KEYMAP_META('h'), NUL,                  A_info_get_info_help_node,
  KEYMAP_META('I'), NUL,                  A_info_virtual_index,
  KEYMAP_META('m'), NUL,                  A_info_menu_item,
  KEYMAP_META('n'), NUL,                  A_info_search,
  KEYMAP_META('N'), NUL,                  A_info_search_backward,
  KEYMAP_META('r'), NUL,                  A_isearch_backward,
  KEYMAP_META('s'), NUL,                  A_isearch_forward,
  KEYMAP_META('t'), NUL,                  A_info_top_node,
  KEYMAP_META('v'), NUL,                  A_info_scroll_backward_page_only,
  KEYMAP_META('x'), NUL,                  A_info_execute_command,
  CONTROL('x'), CONTROL('b'), NUL,        A_list_visited_nodes,
  CONTROL('x'), CONTROL('c'), NUL,        A_info_quit,
  CONTROL('x'), CONTROL('f'), NUL,        A_info_view_file,
  CONTROL('x'), CONTROL('g'), NUL,        A_info_abort_key,
  CONTROL('x'), CONTROL('v'), NUL,        A_info_view_file,
  CONTROL('x'), LFD, NUL,         A_info_select_reference_this_line,
  CONTROL('x'), RET, NUL,         A_info_select_reference_this_line,
  CONTROL('x'), '0', NUL,         A_info_delete_window,
  CONTROL('x'), '1', NUL,         A_info_keep_one_window,
  CONTROL('x'), '2', NUL,         A_info_split_window,
  CONTROL('x'), '^', NUL,         A_info_grow_window,
  CONTROL('x'), 'b', NUL,         A_select_visited_node,
  CONTROL('x'), 'g', NUL,         A_info_goto_node,
  CONTROL('x'), 'i', NUL,         A_info_index_search,
  CONTROL('x'), 'I', NUL,         A_info_goto_invocation_node,
  CONTROL('x'), 'n', NUL,         A_info_next_node,
  CONTROL('x'), 'o', NUL,         A_info_next_window,
  CONTROL('x'), 'O', NUL,         A_info_goto_invocation_node,
  CONTROL('x'), 'p', NUL,         A_info_prev_node,
  CONTROL('x'), 'r', NUL,         A_info_xref_item,
  CONTROL('x'), 't', NUL,         A_info_tile_windows,
  CONTROL('x'), 'u', NUL,         A_info_up_node,
  CONTROL('x'), 'w', NUL,         A_info_toggle_wrap,
  CONTROL('x'), ',', NUL,         A_info_next_index_match,

  KEY_PAGE_UP, NUL,             A_info_scroll_backward,
  KEY_PAGE_DOWN, NUL,           A_info_scroll_forward,
  KEY_DELETE, NUL,              A_info_scroll_backward,
  KEY_RIGHT_ARROW, NUL,         A_info_scroll_forward_page_only,
  KEY_LEFT_ARROW, NUL,          A_info_scroll_backward_page_only,
  KEY_HOME, NUL,                A_info_beginning_of_node,
  KEY_END, NUL,                 A_info_end_of_node,
  ESC, KEY_PAGE_DOWN, NUL,      A_info_scroll_other_window,
  ESC, KEY_PAGE_UP, NUL,        A_info_scroll_other_window_backward,
  ESC, KEY_DELETE, NUL,         A_info_scroll_other_window_backward,
  ESC, KEY_UP_ARROW, NUL,       A_info_prev_node,
  ESC, KEY_DOWN_ARROW, NUL,     A_info_next_node,
  ESC, KEY_RIGHT_ARROW, NUL,    A_info_xref_item,
  ESC, KEY_LEFT_ARROW, NUL,     A_info_beginning_of_node,
  CONTROL('x'), KEY_DELETE, NUL, A_ea_backward_kill_line,
  
};


static int default_vi_like_ea_keys[] =
{
  KEYMAP_META('1'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('2'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('3'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('4'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('5'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('6'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('7'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('8'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('9'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META('-'), NUL,                 A_info_add_digit_to_numeric_arg,
  KEYMAP_META(CONTROL('g')), NUL,        A_ea_abort,
  KEYMAP_META(CONTROL('h')), NUL,        A_ea_backward_kill_word,
  KEYMAP_META(CONTROL('v')), NUL,        A_ea_scroll_completions_window,
  KEYMAP_META('0'), NUL,                 A_ea_beg_of_line,
  KEYMAP_META('$'), NUL,                 A_ea_end_of_line,
  KEYMAP_META('b'), NUL,                 A_ea_backward_word,
  KEYMAP_META('d'), NUL,                 A_ea_kill_word,
  KEYMAP_META('f'), NUL,                 A_ea_forward_word,
  KEYMAP_META('h'), NUL,                 A_ea_backward,
  KEYMAP_META('l'), NUL,                 A_ea_forward,
  KEYMAP_META('w'), NUL,                 A_ea_forward_word,
  KEYMAP_META('x'), NUL,                 A_ea_delete,
  KEYMAP_META('X'), NUL,                 A_ea_kill_word,
  KEYMAP_META('y'), NUL,                 A_ea_yank_pop,
  KEYMAP_META('?'), NUL,                 A_ea_possible_completions,
  KEYMAP_META(TAB), NUL,                 A_ea_tab_insert,
  KEYMAP_META(DEL), NUL,                 A_ea_kill_word,
  CONTROL('a'), NUL,              A_ea_beg_of_line,
  CONTROL('b'), NUL,              A_ea_backward,
  CONTROL('d'), NUL,              A_ea_delete,
  CONTROL('e'), NUL,              A_ea_end_of_line,
  CONTROL('f'), NUL,              A_ea_forward,
  CONTROL('g'), NUL,              A_ea_abort,
  ESC, NUL,                       A_ea_abort,
  CONTROL('h'), NUL,              A_ea_rubout,
  CONTROL('k'), NUL,              A_ea_kill_line,
  CONTROL('l'), NUL,              A_info_redraw_display,
  CONTROL('q'), NUL,              A_ea_quoted_insert,
  CONTROL('t'), NUL,              A_ea_transpose_chars,
  CONTROL('u'), NUL,              A_ea_abort,
  CONTROL('v'), NUL,              A_ea_quoted_insert,
  CONTROL('y'), NUL,              A_ea_yank,
  LFD, NUL,                       A_ea_newline,
  RET, NUL,                       A_ea_newline,
  SPC, NUL,                       A_ea_complete,
  TAB, NUL,                       A_ea_complete,
  '?', NUL,                       A_ea_possible_completions,
  CONTROL('x'), 'o', NUL,         A_info_next_window,
  
  KEY_RIGHT_ARROW, NUL,         A_ea_forward,
  KEY_LEFT_ARROW, NUL,          A_ea_backward,
  KEY_HOME, NUL,                A_ea_beg_of_line,
  KEY_END, NUL,                 A_ea_end_of_line,
#ifdef __MSDOS__
  KEY_DELETE, NUL,              A_ea_delete,
#else
  KEY_DELETE, NUL,              A_ea_rubout,
#endif
  ESC, KEY_RIGHT_ARROW, NUL,    A_ea_forward_word,
  ESC, KEY_LEFT_ARROW, NUL,     A_ea_backward_word,
  ESC, KEY_DELETE, NUL,         A_ea_kill_word,
  CONTROL('x'), KEY_DELETE, NUL,A_ea_backward_kill_line,
};


/* Whether to suppress the default key bindings. */
static int sup_info, sup_ea;

/* Fetch the contents of the init file at INIT_FILE, or the standard
   infokey file "$HOME/.infokey".  Return non-zero if an init file was
   loaded and read. */
static int
fetch_user_maps (char *init_file)
{
  char *filename = NULL;
  char *homedir;
  FILE *inf;

  /* In infokey.c */
  int compile (FILE *fp, const char *filename, int *, int *);

  /* Find and open file. */
  if (init_file)
    filename = xstrdup (init_file);
  else if ((homedir = getenv ("HOME")) != NULL
#ifdef __MINGW32__
	    || (homedir = getenv ("USERPROFILE")) != NULL
#endif
	  )
    {
      filename = xmalloc (strlen (homedir) + 2 + strlen (INFOKEY_FILE));
      strcpy (filename, homedir);
      strcat (filename, "/");
      strcat (filename, INFOKEY_FILE);
    }
#if defined(__MSDOS__) || defined(__MINGW32__)
  /* Poor baby, she doesn't have a HOME...  */
  else
    filename = xstrdup (INFOKEY_FILE); /* try current directory */
#endif
  inf = fopen (filename, "r");
  if (!inf)
    {
      free (filename);
      if (init_file)
        info_error (_("could not open init file %s"), init_file);
      return 0;
    }

  compile (inf, filename, &sup_info, &sup_ea);

  free (filename);
  return 1;
}


/* Set key bindings in MAP from TABLE, which is of length LEN. */
static void
section_to_keymaps (Keymap map, int *table, unsigned int len)
{
  int k;
  Keymap esc_map;

  int *p;
  int *seq;
  enum { getseq, gotseq, getaction } state = getseq;
  
  for (p = table; (unsigned int) (p - table) < len; p++)
    {
      switch (state)
	{
	case getseq:
	  if (*p)
	    {
	      seq = p;
	      state = gotseq;
	    }
	  break;
	  
	case gotseq:
	  if (!*p)
            state = getaction;
	  break;
	  
	case getaction:
	  {
	    unsigned int action = *p;
	    KEYMAP_ENTRY ke;
	    
	    state = getseq;

            ke.type = ISFUNC;
            ke.value.function = action < A_NCOMMANDS ?
                                &function_doc_array[action]
                                : NULL;
            keymap_bind_keyseq (map, seq, &ke);
	  }
	  break;
	}
    }
  if (state != getseq)
    abort ();

  /* Go through map and bind ESC x to the same function as M-x if it is not 
     bound already. */
  if (!map[ESC].value.function)
    {
      map[ESC].type = ISKMAP;
      map[ESC].value.keymap = keymap_make_keymap ();
    }

  if (map[ESC].type != ISKMAP)
    return; /* ESC is bound to a command. */

  esc_map = map[ESC].value.keymap;
  for (k = 1; k < KEYMAP_META_BASE; k++)
    {
      if (map[k + KEYMAP_META_BASE].type == ISFUNC
          && esc_map[k].value.function == 0)
        {
          esc_map[k].type = ISFUNC;
          esc_map[k].value.function = map[k + KEYMAP_META_BASE].value.function;
        }
    }
  return;
}

/* Read key bindings and variable settings from INIT_FILE.  If INIT_FILE
   is null, look for the init file in the default location. */
void
read_init_file (char *init_file)
{
  int *info_keys, *ea_keys; /* Pointers to keymap tables. */
  long info_keys_len, ea_keys_len; /* Sizes of keymap tables. */

  int i;

  if (!info_keymap)
    {
      info_keymap = keymap_make_keymap ();
      echo_area_keymap = keymap_make_keymap ();
    }

  if (!vi_keys_p)
    {
      info_keys = default_emacs_like_info_keys;
      info_keys_len = sizeof (default_emacs_like_info_keys)/sizeof (int);
      ea_keys = default_emacs_like_ea_keys;
      ea_keys_len = sizeof (default_emacs_like_ea_keys)/sizeof (int);
    }
  else
    {
      info_keys = default_vi_like_info_keys;
      info_keys_len = sizeof (default_vi_like_info_keys)/sizeof(int);
      ea_keys = default_vi_like_ea_keys;
      ea_keys_len = sizeof (default_vi_like_ea_keys)/sizeof(int);
    }

  /* Get user-defined keys and variables.  */
  if (fetch_user_maps (init_file))
    {
      if (sup_info)
        info_keys = 0; /* Suppress default bindings. */
      if (sup_ea)
        ea_keys = 0;
    }

  /* Apply the default bindings, unless the user says to suppress
     them. */
  if (info_keys)
    section_to_keymaps (info_keymap, info_keys, info_keys_len);
  if (ea_keys)
    section_to_keymaps (echo_area_keymap, ea_keys, ea_keys_len);

  for (i = 'A'; i < ('Z' + 1); i++)
    {
      if (!info_keymap[i].value.function)
        {
          info_keymap[i].type = ISFUNC;
          info_keymap[i].value.function = InfoCmd (info_do_lowercase_version);
        }

      if (!info_keymap[KEYMAP_META(i)].value.function)
        {
          info_keymap[KEYMAP_META(i)].type = ISFUNC;
          info_keymap[KEYMAP_META(i)].value.function
            = InfoCmd (info_do_lowercase_version);
        }
    }
}

/* vim: set sw=2 cino={1s>2sn-s^-se-s: */
