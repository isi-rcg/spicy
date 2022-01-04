/* funs.h -- Generated declarations for Info commands. */

#include "info.h"
#include "window.h"

/* Functions declared in "../../texinfo-6.5/info/session.c". */
#define A_info_all_files 0
extern void info_all_files (WINDOW *window, int count);
#define A_info_next_line 1
extern void info_next_line (WINDOW *window, int count);
#define A_info_prev_line 2
extern void info_prev_line (WINDOW *window, int count);
#define A_info_move_to_window_line 3
extern void info_move_to_window_line (WINDOW *window, int count);
#define A_info_end_of_line 4
extern void info_end_of_line (WINDOW *window, int count);
#define A_info_beginning_of_line 5
extern void info_beginning_of_line (WINDOW *window, int count);
#define A_info_forward_char 6
extern void info_forward_char (WINDOW *window, int count);
#define A_info_backward_char 7
extern void info_backward_char (WINDOW *window, int count);
#define A_info_forward_word 8
extern void info_forward_word (WINDOW *window, int count);
#define A_info_backward_word 9
extern void info_backward_word (WINDOW *window, int count);
#define A_info_beginning_of_node 10
extern void info_beginning_of_node (WINDOW *window, int count);
#define A_info_end_of_node 11
extern void info_end_of_node (WINDOW *window, int count);
#define A_info_scroll_forward 12
extern void info_scroll_forward (WINDOW *window, int count);
#define A_info_scroll_backward 13
extern void info_scroll_backward (WINDOW *window, int count);
#define A_info_scroll_forward_set_window 14
extern void info_scroll_forward_set_window (WINDOW *window, int count);
#define A_info_scroll_backward_set_window 15
extern void info_scroll_backward_set_window (WINDOW *window, int count);
#define A_info_scroll_forward_page_only 16
extern void info_scroll_forward_page_only (WINDOW *window, int count);
#define A_info_scroll_backward_page_only 17
extern void info_scroll_backward_page_only (WINDOW *window, int count);
#define A_info_scroll_forward_page_only_set_window 18
extern void info_scroll_forward_page_only_set_window (WINDOW *window, int count);
#define A_info_scroll_backward_page_only_set_window 19
extern void info_scroll_backward_page_only_set_window (WINDOW *window, int count);
#define A_info_down_line 20
extern void info_down_line (WINDOW *window, int count);
#define A_info_up_line 21
extern void info_up_line (WINDOW *window, int count);
#define A_info_scroll_half_screen_down 22
extern void info_scroll_half_screen_down (WINDOW *window, int count);
#define A_info_scroll_half_screen_up 23
extern void info_scroll_half_screen_up (WINDOW *window, int count);
#define A_info_scroll_other_window 24
extern void info_scroll_other_window (WINDOW *window, int count);
#define A_info_scroll_other_window_backward 25
extern void info_scroll_other_window_backward (WINDOW *window, int count);
#define A_info_next_window 26
extern void info_next_window (WINDOW *window, int count);
#define A_info_prev_window 27
extern void info_prev_window (WINDOW *window, int count);
#define A_info_split_window 28
extern void info_split_window (WINDOW *window, int count);
#define A_info_delete_window 29
extern void info_delete_window (WINDOW *window, int count);
#define A_info_keep_one_window 30
extern void info_keep_one_window (WINDOW *window, int count);
#define A_info_grow_window 31
extern void info_grow_window (WINDOW *window, int count);
#define A_info_tile_windows 32
extern void info_tile_windows (WINDOW *window, int count);
#define A_info_toggle_wrap 33
extern void info_toggle_wrap (WINDOW *window, int count);
#define A_info_menu_digit 34
extern void info_menu_digit (WINDOW *window, int count);
#define A_info_last_menu_item 35
extern void info_last_menu_item (WINDOW *window, int count);
#define A_info_menu_item 36
extern void info_menu_item (WINDOW *window, int count);
#define A_info_xref_item 37
extern void info_xref_item (WINDOW *window, int count);
#define A_info_find_menu 38
extern void info_find_menu (WINDOW *window, int count);
#define A_info_visit_menu 39
extern void info_visit_menu (WINDOW *window, int count);
#define A_info_move_to_prev_xref 40
extern void info_move_to_prev_xref (WINDOW *window, int count);
#define A_info_move_to_next_xref 41
extern void info_move_to_next_xref (WINDOW *window, int count);
#define A_info_select_reference_this_line 42
extern void info_select_reference_this_line (WINDOW *window, int count);
#define A_info_menu_sequence 43
extern void info_menu_sequence (WINDOW *window, int count);
#define A_info_next_node 44
extern void info_next_node (WINDOW *window, int count);
#define A_info_prev_node 45
extern void info_prev_node (WINDOW *window, int count);
#define A_info_up_node 46
extern void info_up_node (WINDOW *window, int count);
#define A_info_last_node 47
extern void info_last_node (WINDOW *window, int count);
#define A_info_first_node 48
extern void info_first_node (WINDOW *window, int count);
#define A_info_global_next_node 49
extern void info_global_next_node (WINDOW *window, int count);
#define A_info_global_prev_node 50
extern void info_global_prev_node (WINDOW *window, int count);
#define A_info_goto_node 51
extern void info_goto_node (WINDOW *window, int count);
#define A_info_goto_invocation_node 52
extern void info_goto_invocation_node (WINDOW *window, int count);
#define A_info_man 53
extern void info_man (WINDOW *window, int count);
#define A_info_top_node 54
extern void info_top_node (WINDOW *window, int count);
#define A_info_dir_node 55
extern void info_dir_node (WINDOW *window, int count);
#define A_info_display_file_info 56
extern void info_display_file_info (WINDOW *window, int count);
#define A_info_history_node 57
extern void info_history_node (WINDOW *window, int count);
#define A_info_view_file 58
extern void info_view_file (WINDOW *window, int count);
#define A_info_print_node 59
extern void info_print_node (WINDOW *window, int count);
#define A_info_toggle_regexp 60
extern void info_toggle_regexp (WINDOW *window, int count);
#define A_info_tree_search 61
extern void info_tree_search (WINDOW *window, int count);
#define A_info_tree_search_next 62
extern void info_tree_search_next (WINDOW *window, int count);
#define A_info_tree_search_previous 63
extern void info_tree_search_previous (WINDOW *window, int count);
#define A_info_search_case_sensitively 64
extern void info_search_case_sensitively (WINDOW *window, int count);
#define A_info_search 65
extern void info_search (WINDOW *window, int count);
#define A_info_search_backward 66
extern void info_search_backward (WINDOW *window, int count);
#define A_info_search_next 67
extern void info_search_next (WINDOW *window, int count);
#define A_info_search_previous 68
extern void info_search_previous (WINDOW *window, int count);
#define A_info_clear_search 69
extern void info_clear_search (WINDOW *window, int count);
#define A_isearch_forward 70
extern void isearch_forward (WINDOW *window, int count);
#define A_isearch_backward 71
extern void isearch_backward (WINDOW *window, int count);
#define A_info_abort_key 72
extern void info_abort_key (WINDOW *window, int count);
#define A_info_info_version 73
extern void info_info_version (WINDOW *window, int count);
#define A_info_redraw_display 74
extern void info_redraw_display (WINDOW *window, int count);
#define A_info_quit 75
extern void info_quit (WINDOW *window, int count);
#define A_info_do_lowercase_version 76
extern void info_do_lowercase_version (WINDOW *window, int count);
#define A_info_add_digit_to_numeric_arg 77
extern void info_add_digit_to_numeric_arg (WINDOW *window, int count);
#define A_info_universal_argument 78
extern void info_universal_argument (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/echo-area.c". */
#define A_ea_forward 79
extern void ea_forward (WINDOW *window, int count);
#define A_ea_backward 80
extern void ea_backward (WINDOW *window, int count);
#define A_ea_beg_of_line 81
extern void ea_beg_of_line (WINDOW *window, int count);
#define A_ea_end_of_line 82
extern void ea_end_of_line (WINDOW *window, int count);
#define A_ea_forward_word 83
extern void ea_forward_word (WINDOW *window, int count);
#define A_ea_backward_word 84
extern void ea_backward_word (WINDOW *window, int count);
#define A_ea_delete 85
extern void ea_delete (WINDOW *window, int count);
#define A_ea_rubout 86
extern void ea_rubout (WINDOW *window, int count);
#define A_ea_abort 87
extern void ea_abort (WINDOW *window, int count);
#define A_ea_newline 88
extern void ea_newline (WINDOW *window, int count);
#define A_ea_quoted_insert 89
extern void ea_quoted_insert (WINDOW *window, int count);
#define A_ea_tab_insert 90
extern void ea_tab_insert (WINDOW *window, int count);
#define A_ea_transpose_chars 91
extern void ea_transpose_chars (WINDOW *window, int count);
#define A_ea_yank 92
extern void ea_yank (WINDOW *window, int count);
#define A_ea_yank_pop 93
extern void ea_yank_pop (WINDOW *window, int count);
#define A_ea_kill_line 94
extern void ea_kill_line (WINDOW *window, int count);
#define A_ea_backward_kill_line 95
extern void ea_backward_kill_line (WINDOW *window, int count);
#define A_ea_kill_word 96
extern void ea_kill_word (WINDOW *window, int count);
#define A_ea_backward_kill_word 97
extern void ea_backward_kill_word (WINDOW *window, int count);
#define A_ea_possible_completions 98
extern void ea_possible_completions (WINDOW *window, int count);
#define A_ea_complete 99
extern void ea_complete (WINDOW *window, int count);
#define A_ea_scroll_completions_window 100
extern void ea_scroll_completions_window (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/infodoc.c". */
#define A_info_get_help_window 101
extern void info_get_help_window (WINDOW *window, int count);
#define A_info_get_info_help_node 102
extern void info_get_info_help_node (WINDOW *window, int count);
#define A_describe_key 103
extern void describe_key (WINDOW *window, int count);
#define A_info_where_is 104
extern void info_where_is (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/m-x.c". */
#define A_describe_command 105
extern void describe_command (WINDOW *window, int count);
#define A_info_execute_command 106
extern void info_execute_command (WINDOW *window, int count);
#define A_set_screen_height 107
extern void set_screen_height (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/indices.c". */
#define A_info_index_search 108
extern void info_index_search (WINDOW *window, int count);
#define A_info_next_index_match 109
extern void info_next_index_match (WINDOW *window, int count);
#define A_info_index_apropos 110
extern void info_index_apropos (WINDOW *window, int count);
#define A_info_virtual_index 111
extern void info_virtual_index (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/nodemenu.c". */
#define A_list_visited_nodes 112
extern void list_visited_nodes (WINDOW *window, int count);
#define A_select_visited_node 113
extern void select_visited_node (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/footnotes.c". */
#define A_info_show_footnotes 114
extern void info_show_footnotes (WINDOW *window, int count);

/* Functions declared in "../../texinfo-6.5/info/variables.c". */
#define A_describe_variable 115
extern void describe_variable (WINDOW *window, int count);
#define A_set_variable 116
extern void set_variable (WINDOW *window, int count);

#define A_NCOMMANDS 117
