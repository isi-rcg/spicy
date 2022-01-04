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

#include "table.h"

#define BAR_HEIGHT ".25m"
#define DOUBLE_LINE_SEP "2p"
#define HALF_DOUBLE_LINE_SEP "1p"
#define LINE_SEP "2p"
#define BODY_DEPTH ".25m"

const int DEFAULT_COLUMN_SEPARATION = 3;

#define DELIMITER_CHAR "\\[tbl]"
#define SEPARATION_FACTOR_REG PREFIX "sep"
#define BOTTOM_REG PREFIX "bot"
#define RESET_MACRO_NAME PREFIX "init"
#define LINESIZE_REG PREFIX "lps"
#define TOP_REG PREFIX "top"
#define CURRENT_ROW_REG PREFIX "crow"
#define LAST_PASSED_ROW_REG PREFIX "passed"
#define TRANSPARENT_STRING_NAME PREFIX "trans"
#define QUOTE_STRING_NAME PREFIX "quote"
#define SECTION_DIVERSION_NAME PREFIX "section"
#define SECTION_DIVERSION_FLAG_REG PREFIX "sflag"
#define SAVED_VERTICAL_POS_REG PREFIX "vert"
#define NEED_BOTTOM_RULE_REG PREFIX "brule"
#define KEEP_MACRO_NAME PREFIX "keep"
#define RELEASE_MACRO_NAME PREFIX "release"
#define SAVED_FONT_REG PREFIX "fnt"
#define SAVED_SIZE_REG PREFIX "sz"
#define SAVED_FILL_REG PREFIX "fll"
#define SAVED_INDENT_REG PREFIX "ind"
#define SAVED_CENTER_REG PREFIX "cent"
#define TABLE_DIVERSION_NAME PREFIX "table"
#define TABLE_DIVERSION_FLAG_REG PREFIX "tflag"
#define TABLE_KEEP_MACRO_NAME PREFIX "tkeep"
#define TABLE_RELEASE_MACRO_NAME PREFIX "trelease"
#define NEEDED_REG PREFIX "needed"
#define REPEATED_MARK_MACRO PREFIX "rmk"
#define REPEATED_VPT_MACRO PREFIX "rvpt"
#define SUPPRESS_BOTTOM_REG PREFIX "supbot"
#define SAVED_DN_REG PREFIX "dn"
#define ROW_START_LINE_REG PREFIX "lnst"
#define ROW_SAVE_LINE_REG PREFIX "lnsv"
#define ROW_MAX_LINE_REG PREFIX "lnmx"
#define REPEATED_NM_SET_MACRO PREFIX "rlns"
#define REPEATED_NM_SUS_MACRO PREFIX "rlnx"

// this must be one character
#define COMPATIBLE_REG PREFIX "c"

#define EXPAND_REG PREFIX "expand"

#define LEADER_REG PREFIX LEADER

#define BLOCK_WIDTH_PREFIX PREFIX "tbw"
#define BLOCK_DIVERSION_PREFIX PREFIX "tbd"
#define BLOCK_HEIGHT_PREFIX PREFIX "tbh"
#define SPAN_WIDTH_PREFIX PREFIX "w"
#define SPAN_LEFT_NUMERIC_WIDTH_PREFIX PREFIX "lnw"
#define SPAN_RIGHT_NUMERIC_WIDTH_PREFIX PREFIX "rnw"
#define SPAN_ALPHABETIC_WIDTH_PREFIX PREFIX "aw"
#define COLUMN_SEPARATION_PREFIX PREFIX "cs"
#define ROW_START_PREFIX PREFIX "rs"
#define COLUMN_START_PREFIX PREFIX "cl"
#define COLUMN_END_PREFIX PREFIX "ce"
#define COLUMN_DIVIDE_PREFIX PREFIX "cd"
#define ROW_TOP_PREFIX PREFIX "rt"

string block_width_reg(int, int);
string block_diversion_name(int, int);
string block_height_reg(int, int);
string span_width_reg(int, int);
string span_left_numeric_width_reg(int, int);
string span_right_numeric_width_reg(int, int);
string span_alphabetic_width_reg(int, int);
string column_separation_reg(int);
string row_start_reg(int);
string column_start_reg(int);
string column_end_reg(int);
string column_divide_reg(int);
string row_top_reg(int);

void set_inline_modifier(const entry_modifier *);
void restore_inline_modifier(const entry_modifier *);
void set_modifier(const entry_modifier *);
int find_decimal_point(const char *, char, const char *);

string an_empty_string;
int location_force_filename = 0;

void printfs(const char *,
	     const string &arg1 = an_empty_string,
	     const string &arg2 = an_empty_string,
	     const string &arg3 = an_empty_string,
	     const string &arg4 = an_empty_string,
	     const string &arg5 = an_empty_string);

void prints(const string &);

inline void prints(char c)
{
  putchar(c);
}

inline void prints(const char *s)
{
  fputs(s, stdout);
}

void prints(const string &s)
{
  if (!s.empty())
    fwrite(s.contents(), 1, s.length(), stdout);
}

struct horizontal_span {
  horizontal_span *next;
  int start_col;
  int end_col;
  horizontal_span(int, int, horizontal_span *);
};

class single_line_entry;
class double_line_entry;
class simple_entry;

class table_entry {
friend class table;
  table_entry *next;
  int input_lineno;
  const char *input_filename;
protected:
  int start_row;
  int end_row;
  int start_col;
  int end_col;
  const table *parent;
  const entry_modifier *mod;
public:
  void set_location();
  table_entry(const table *, const entry_modifier *);
  virtual ~table_entry();
  virtual int divert(int, const string *, int *, int);
  virtual void do_width();
  virtual void do_depth();
  virtual void print() = 0;
  virtual void position_vertically() = 0;
  virtual single_line_entry *to_single_line_entry();
  virtual double_line_entry *to_double_line_entry();
  virtual simple_entry *to_simple_entry();
  virtual int line_type();
  virtual void note_double_vrule_on_right(int);
  virtual void note_double_vrule_on_left(int);
};

class simple_entry : public table_entry {
public:
  simple_entry(const table *, const entry_modifier *);
  void print();
  void position_vertically();
  simple_entry *to_simple_entry();
  virtual void add_tab();
  virtual void simple_print(int);
};

class empty_entry : public simple_entry {
public:
  empty_entry(const table *, const entry_modifier *);
  int line_type();
};

class text_entry : public simple_entry {
protected:
  char *contents;
  void print_contents();
public:
  text_entry(const table *, const entry_modifier *, char *);
  ~text_entry();
};

void text_entry::print_contents()
{
  set_inline_modifier(mod);
  prints(contents);
  restore_inline_modifier(mod);
}

class repeated_char_entry : public text_entry {
public:
  repeated_char_entry(const table *, const entry_modifier *, char *);
  void simple_print(int);
};

class simple_text_entry : public text_entry {
public:
  simple_text_entry(const table *, const entry_modifier *, char *);
  void do_width();
};

class left_text_entry : public simple_text_entry {
public:
  left_text_entry(const table *, const entry_modifier *, char *);
  void simple_print(int);
  void add_tab();
};

class right_text_entry : public simple_text_entry {
public:
  right_text_entry(const table *, const entry_modifier *, char *);
  void simple_print(int);
  void add_tab();
};

class center_text_entry : public simple_text_entry {
public:
  center_text_entry(const table *, const entry_modifier *, char *);
  void simple_print(int);
  void add_tab();
};

class numeric_text_entry : public text_entry {
  int dot_pos;
public:
  numeric_text_entry(const table *, const entry_modifier *, char *, int);
  void do_width();
  void simple_print(int);
};

class alphabetic_text_entry : public text_entry {
public:
  alphabetic_text_entry(const table *, const entry_modifier *, char *);
  void do_width();
  void simple_print(int);
  void add_tab();
};

class line_entry : public simple_entry {
protected:
  char double_vrule_on_right;
  char double_vrule_on_left;
public:
  line_entry(const table *, const entry_modifier *);
  void note_double_vrule_on_right(int);
  void note_double_vrule_on_left(int);
  void simple_print(int) = 0;
};

class single_line_entry : public line_entry {
public:
  single_line_entry(const table *, const entry_modifier *);
  void simple_print(int);
  single_line_entry *to_single_line_entry();
  int line_type();
};

class double_line_entry : public line_entry {
public:
  double_line_entry(const table *, const entry_modifier *);
  void simple_print(int);
  double_line_entry *to_double_line_entry();
  int line_type();
};

class short_line_entry : public simple_entry {
public:
  short_line_entry(const table *, const entry_modifier *);
  void simple_print(int);
  int line_type();
};

class short_double_line_entry : public simple_entry {
public:
  short_double_line_entry(const table *, const entry_modifier *);
  void simple_print(int);
  int line_type();
};

class block_entry : public table_entry {
  char *contents;
protected:
  void do_divert(int, int, const string *, int *, int);
public:
  block_entry(const table *, const entry_modifier *, char *);
  ~block_entry();
  int divert(int, const string *, int *, int);
  void do_depth();
  void position_vertically();
  void print() = 0;
};

class left_block_entry : public block_entry {
public:
  left_block_entry(const table *, const entry_modifier *, char *);
  void print();
};

class right_block_entry : public block_entry {
public:
  right_block_entry(const table *, const entry_modifier *, char *);
  void print();
};

class center_block_entry : public block_entry {
public:
  center_block_entry(const table *, const entry_modifier *, char *);
  void print();
};

class alphabetic_block_entry : public block_entry {
public:
  alphabetic_block_entry(const table *, const entry_modifier *, char *);
  void print();
  int divert(int, const string *, int *, int);
};

table_entry::table_entry(const table *p, const entry_modifier *m)
: next(0), input_lineno(-1), input_filename(0),
  start_row(-1), end_row(-1), start_col(-1), end_col(-1), parent(p), mod(m)
{
}

table_entry::~table_entry()
{
}

int table_entry::divert(int, const string *, int *, int)
{
  return 0;
}

void table_entry::do_width()
{
}

single_line_entry *table_entry::to_single_line_entry()
{
  return 0;
}

double_line_entry *table_entry::to_double_line_entry()
{
  return 0;
}

simple_entry *table_entry::to_simple_entry()
{
  return 0;
}

void table_entry::do_depth()
{
}

void table_entry::set_location()
{
  set_troff_location(input_filename, input_lineno);
}

int table_entry::line_type()
{
  return -1;
}

void table_entry::note_double_vrule_on_right(int)
{
}

void table_entry::note_double_vrule_on_left(int)
{
}

simple_entry::simple_entry(const table *p, const entry_modifier *m)
: table_entry(p, m)
{
}

void simple_entry::add_tab()
{
  // do nothing
}

void simple_entry::simple_print(int)
{
  // do nothing
}

void simple_entry::position_vertically()
{
  if (start_row != end_row)
    switch (mod->vertical_alignment) {
    case entry_modifier::TOP:
      printfs(".sp |\\n[%1]u\n", row_start_reg(start_row));
      break;
    case entry_modifier::CENTER:
      // Peform the motion in two stages so that the center is rounded
      // vertically upwards even if net vertical motion is upwards.
      printfs(".sp |\\n[%1]u\n", row_start_reg(start_row));
      printfs(".sp \\n[" BOTTOM_REG "]u-\\n[%1]u-1v/2u\n", 
	      row_start_reg(start_row));
      break;
    case entry_modifier::BOTTOM:
      printfs(".sp |\\n[%1]u+\\n[" BOTTOM_REG "]u-\\n[%1]u-1v\n", 
	      row_start_reg(start_row));
      break;
    default:
      assert(0);
    }
}

void simple_entry::print()
{
  prints(".ta");
  add_tab();
  prints('\n');
  set_location();
  prints("\\&");
  simple_print(0);
  prints('\n');
}

simple_entry *simple_entry::to_simple_entry()
{
  return this;
}

empty_entry::empty_entry(const table *p, const entry_modifier *m)
: simple_entry(p, m)
{
}

int empty_entry::line_type()
{
  return 0;
}

text_entry::text_entry(const table *p, const entry_modifier *m, char *s)
: simple_entry(p, m), contents(s)
{
}

text_entry::~text_entry()
{
  free(contents);
}

repeated_char_entry::repeated_char_entry(const table *p,
					 const entry_modifier *m, char *s)
: text_entry(p, m, s)
{
}

void repeated_char_entry::simple_print(int)
{
  printfs("\\h'|\\n[%1]u'", column_start_reg(start_col));
  set_inline_modifier(mod);
  printfs("\\l" DELIMITER_CHAR "\\n[%1]u\\&",
	  span_width_reg(start_col, end_col));
  prints(contents);
  prints(DELIMITER_CHAR);
  restore_inline_modifier(mod);
}

simple_text_entry::simple_text_entry(const table *p,
				     const entry_modifier *m, char *s)
: text_entry(p, m, s)
{
}

void simple_text_entry::do_width()
{
  set_location();
  printfs(".nr %1 \\n[%1]>?\\w" DELIMITER_CHAR,
	  span_width_reg(start_col, end_col));
  print_contents();
  prints(DELIMITER_CHAR "\n");
}

left_text_entry::left_text_entry(const table *p,
				 const entry_modifier *m, char *s)
: simple_text_entry(p, m, s)
{
}

void left_text_entry::simple_print(int)
{
  printfs("\\h'|\\n[%1]u'", column_start_reg(start_col));
  print_contents();
}

// The only point of this is to make '\a' "work" as in Unix tbl.  Grrr.

void left_text_entry::add_tab()
{
  printfs(" \\n[%1]u", column_end_reg(end_col));
}

right_text_entry::right_text_entry(const table *p,
				   const entry_modifier *m, char *s)
: simple_text_entry(p, m, s)
{
}

void right_text_entry::simple_print(int)
{
  printfs("\\h'|\\n[%1]u'", column_start_reg(start_col));
  prints("\002\003");
  print_contents();
  prints("\002");
}

void right_text_entry::add_tab()
{
  printfs(" \\n[%1]u", column_end_reg(end_col));
}

center_text_entry::center_text_entry(const table *p,
				     const entry_modifier *m, char *s)
: simple_text_entry(p, m, s)
{
}

void center_text_entry::simple_print(int)
{
  printfs("\\h'|\\n[%1]u'", column_start_reg(start_col));
  prints("\002\003");
  print_contents();
  prints("\003\002");
}

void center_text_entry::add_tab()
{
  printfs(" \\n[%1]u", column_end_reg(end_col));
}

numeric_text_entry::numeric_text_entry(const table *p,
				       const entry_modifier *m,
				       char *s, int pos)
: text_entry(p, m, s), dot_pos(pos)
{
}

void numeric_text_entry::do_width()
{
  if (dot_pos != 0) {
    set_location();
    printfs(".nr %1 0\\w" DELIMITER_CHAR,
	    block_width_reg(start_row, start_col));
    set_inline_modifier(mod);
    for (int i = 0; i < dot_pos; i++)
      prints(contents[i]);
    restore_inline_modifier(mod);
    prints(DELIMITER_CHAR "\n");
    printfs(".nr %1 \\n[%1]>?\\n[%2]\n",
	    span_left_numeric_width_reg(start_col, end_col),
	    block_width_reg(start_row, start_col));
  }
  else
    printfs(".nr %1 0\n", block_width_reg(start_row, start_col));
  if (contents[dot_pos] != '\0') {
    set_location();
    printfs(".nr %1 \\n[%1]>?\\w" DELIMITER_CHAR,
	    span_right_numeric_width_reg(start_col, end_col));
    set_inline_modifier(mod);
    prints(contents + dot_pos);
    restore_inline_modifier(mod);
    prints(DELIMITER_CHAR "\n");
  }
}

void numeric_text_entry::simple_print(int)
{
  printfs("\\h'|(\\n[%1]u-\\n[%2]u-\\n[%3]u/2u+\\n[%2]u+\\n[%4]u-\\n[%5]u)'",
	  span_width_reg(start_col, end_col),
	  span_left_numeric_width_reg(start_col, end_col),
	  span_right_numeric_width_reg(start_col, end_col),
	  column_start_reg(start_col),
	  block_width_reg(start_row, start_col));
  print_contents();
}

alphabetic_text_entry::alphabetic_text_entry(const table *p,
					     const entry_modifier *m,
					     char *s)
: text_entry(p, m, s)
{
}

void alphabetic_text_entry::do_width()
{
  set_location();
  printfs(".nr %1 \\n[%1]>?\\w" DELIMITER_CHAR,
	  span_alphabetic_width_reg(start_col, end_col));
  print_contents();
  prints(DELIMITER_CHAR "\n");
}

void alphabetic_text_entry::simple_print(int)
{
  printfs("\\h'|\\n[%1]u'", column_start_reg(start_col));
  printfs("\\h'\\n[%1]u-\\n[%2]u/2u'",
	  span_width_reg(start_col, end_col),
	  span_alphabetic_width_reg(start_col, end_col));
  print_contents();
}

// The only point of this is to make '\a' "work" as in Unix tbl.  Grrr.

void alphabetic_text_entry::add_tab()
{
  printfs(" \\n[%1]u", column_end_reg(end_col));
}

block_entry::block_entry(const table *p, const entry_modifier *m, char *s)
: table_entry(p, m), contents(s)
{
}

block_entry::~block_entry()
{
  a_delete contents;
}

void block_entry::position_vertically()
{
  if (start_row != end_row)
    switch(mod->vertical_alignment) {
    case entry_modifier::TOP:
      printfs(".sp |\\n[%1]u\n", row_start_reg(start_row));
      break;
    case entry_modifier::CENTER:
      // Peform the motion in two stages so that the center is rounded
      // vertically upwards even if net vertical motion is upwards.
      printfs(".sp |\\n[%1]u\n", row_start_reg(start_row));
      printfs(".sp \\n[" BOTTOM_REG "]u-\\n[%1]u-\\n[%2]u/2u\n", 
	      row_start_reg(start_row),
	      block_height_reg(start_row, start_col));
      break;
    case entry_modifier::BOTTOM:
      printfs(".sp |\\n[%1]u+\\n[" BOTTOM_REG "]u-\\n[%1]u-\\n[%2]u\n", 
	      row_start_reg(start_row),
	      block_height_reg(start_row, start_col));
      break;
    default:
      assert(0);
    }
  if (mod->stagger)
    prints(".sp -.5v\n");
}

int block_entry::divert(int ncols, const string *mw, int *sep, int do_expand)
{
  do_divert(0, ncols, mw, sep, do_expand);
  return 1;
}

void block_entry::do_divert(int alphabetic, int ncols, const string *mw,
			    int *sep, int do_expand)
{
  int i;
  for (i = start_col; i <= end_col; i++)
    if (parent->expand[i])
      break;
  if (i > end_col) {
    if (do_expand)
      return;
  }
  else {
    if (!do_expand)
      return;
  }
  printfs(".di %1\n", block_diversion_name(start_row, start_col));
  prints(".if \\n[" SAVED_FILL_REG "] .fi\n"
	 ".in 0\n");
  prints(".ll ");
  for (i = start_col; i <= end_col; i++)
    if (mw[i].empty() && !parent->expand[i])
      break;
  if (i > end_col) {
    // Every column spanned by this entry has a minimum width.
    for (int j = start_col; j <= end_col; j++) {
      if (j > start_col) {
	if (sep)
	  printfs("+%1n", as_string(sep[j - 1]));
	prints('+');
      }
      if (parent->expand[j])
	prints("\\n[" EXPAND_REG "]u");
      else
	printfs("(n;%1)", mw[j]);
    }
    printfs(">?\\n[%1]u", span_width_reg(start_col, end_col));
  }
  else
    // Assign each column with a block entry 1/(n+1) of the line
    // width, where n is the column count.
    printfs("(u;\\n[%1]>?(\\n[.l]*%2/%3))", 
	    span_width_reg(start_col, end_col), 
	    as_string(end_col - start_col + 1),
	    as_string(ncols + 1));
  if (alphabetic)
    prints("-2n");
  prints("\n");
  prints(".cp \\n(" COMPATIBLE_REG "\n");
  set_modifier(mod);
  set_location();
  prints(contents);
  prints(".br\n.di\n.cp 0\n");
  if (!mod->zero_width) {
    if (alphabetic) {
      printfs(".nr %1 \\n[%1]>?(\\n[dl]+2n)\n",
	      span_width_reg(start_col, end_col));
      printfs(".nr %1 \\n[%1]>?\\n[dl]\n",
	      span_alphabetic_width_reg(start_col, end_col));
    }
    else
      printfs(".nr %1 \\n[%1]>?\\n[dl]\n",
	      span_width_reg(start_col, end_col));
  }
  printfs(".nr %1 \\n[dn]\n", block_height_reg(start_row, start_col));
  printfs(".nr %1 \\n[dl]\n", block_width_reg(start_row, start_col));
  prints("." RESET_MACRO_NAME "\n"
	 ".in \\n[" SAVED_INDENT_REG "]u\n"
	 ".nf\n");
  // the block might have contained .lf commands
  location_force_filename = 1;
}

void block_entry::do_depth()
{
  printfs(".nr " BOTTOM_REG " \\n[" BOTTOM_REG "]>?(\\n[%1]+\\n[%2])\n",
	  row_start_reg(start_row),
	  block_height_reg(start_row, start_col));
}

left_block_entry::left_block_entry(const table *p,
				   const entry_modifier *m, char *s)
: block_entry(p, m, s)
{
}

void left_block_entry::print()
{
  printfs(".in +\\n[%1]u\n", column_start_reg(start_col));
  printfs(".%1\n", block_diversion_name(start_row, start_col));
  prints(".in\n");
}

right_block_entry::right_block_entry(const table *p,
				     const entry_modifier *m, char *s)
: block_entry(p, m, s)
{
}

void right_block_entry::print()
{
  printfs(".in +\\n[%1]u+\\n[%2]u-\\n[%3]u\n",
	  column_start_reg(start_col),
	  span_width_reg(start_col, end_col),
	  block_width_reg(start_row, start_col));
  printfs(".%1\n", block_diversion_name(start_row, start_col));
  prints(".in\n");
}

center_block_entry::center_block_entry(const table *p,
				       const entry_modifier *m, char *s)
: block_entry(p, m, s)
{
}

void center_block_entry::print()
{
  printfs(".in +\\n[%1]u+(\\n[%2]u-\\n[%3]u/2u)\n",
	  column_start_reg(start_col),
	  span_width_reg(start_col, end_col),
	  block_width_reg(start_row, start_col));
  printfs(".%1\n", block_diversion_name(start_row, start_col));
  prints(".in\n");
}

alphabetic_block_entry::alphabetic_block_entry(const table *p,
					       const entry_modifier *m,
					       char *s)
: block_entry(p, m, s)
{
}

int alphabetic_block_entry::divert(int ncols, const string *mw, int *sep,
				   int do_expand)
{
  do_divert(1, ncols, mw, sep, do_expand);
  return 1;
}

void alphabetic_block_entry::print()
{
  printfs(".in +\\n[%1]u+(\\n[%2]u-\\n[%3]u/2u)\n",
	  column_start_reg(start_col),
	  span_width_reg(start_col, end_col),
	  span_alphabetic_width_reg(start_col, end_col));
  printfs(".%1\n", block_diversion_name(start_row, start_col));
  prints(".in\n");
}

line_entry::line_entry(const table *p, const entry_modifier *m)
: simple_entry(p, m), double_vrule_on_right(0), double_vrule_on_left(0)
{
}

void line_entry::note_double_vrule_on_right(int is_corner)
{
  double_vrule_on_right = is_corner ? 1 : 2;
}

void line_entry::note_double_vrule_on_left(int is_corner)
{
  double_vrule_on_left = is_corner ? 1 : 2;
}

single_line_entry::single_line_entry(const table *p, const entry_modifier *m)
: line_entry(p, m)
{
}

int single_line_entry::line_type()
{
  return 1;
}

void single_line_entry::simple_print(int dont_move)
{
  printfs("\\h'|\\n[%1]u",
	  column_divide_reg(start_col));
  if (double_vrule_on_left) {
    prints(double_vrule_on_left == 1 ? "-" : "+");
    prints(HALF_DOUBLE_LINE_SEP);
  }
  prints("'");
  if (!dont_move)
    prints("\\v'-" BAR_HEIGHT "'");
  printfs("\\s[\\n[" LINESIZE_REG "]]" "\\D'l |\\n[%1]u",
	  column_divide_reg(end_col+1));
  if (double_vrule_on_right) {
    prints(double_vrule_on_left == 1 ? "+" : "-");
    prints(HALF_DOUBLE_LINE_SEP);
  }
  prints("0'\\s0");
  if (!dont_move)
    prints("\\v'" BAR_HEIGHT "'");
}
  
single_line_entry *single_line_entry::to_single_line_entry()
{
  return this;
}

double_line_entry::double_line_entry(const table *p, const entry_modifier *m)
: line_entry(p, m)
{
}

int double_line_entry::line_type()
{
  return 2;
}

void double_line_entry::simple_print(int dont_move)
{
  if (!dont_move)
    prints("\\v'-" BAR_HEIGHT "'");
  printfs("\\h'|\\n[%1]u",
	  column_divide_reg(start_col));
  if (double_vrule_on_left) {
    prints(double_vrule_on_left == 1 ? "-" : "+");
    prints(HALF_DOUBLE_LINE_SEP);
  }
  prints("'");
  printfs("\\v'-" HALF_DOUBLE_LINE_SEP "'"
	  "\\s[\\n[" LINESIZE_REG "]]"
	  "\\D'l |\\n[%1]u",
	  column_divide_reg(end_col+1));
  if (double_vrule_on_right)
    prints("-" HALF_DOUBLE_LINE_SEP);
  prints(" 0'");
  printfs("\\v'" DOUBLE_LINE_SEP "'"
	  "\\D'l |\\n[%1]u",
	  column_divide_reg(start_col));
  if (double_vrule_on_right) {
    prints(double_vrule_on_left == 1 ? "+" : "-");
    prints(HALF_DOUBLE_LINE_SEP);
  }
  prints(" 0'");
  prints("\\s0"
	 "\\v'-" HALF_DOUBLE_LINE_SEP "'");
  if (!dont_move)
    prints("\\v'" BAR_HEIGHT "'");
}

double_line_entry *double_line_entry::to_double_line_entry()
{
  return this;
}

short_line_entry::short_line_entry(const table *p, const entry_modifier *m)
: simple_entry(p, m)
{
}

int short_line_entry::line_type()
{
  return 1;
}

void short_line_entry::simple_print(int dont_move)
{
  if (mod->stagger)
    prints("\\v'-.5v'");
  if (!dont_move)
    prints("\\v'-" BAR_HEIGHT "'");
  printfs("\\h'|\\n[%1]u'", column_start_reg(start_col));
  printfs("\\s[\\n[" LINESIZE_REG "]]"
	  "\\D'l \\n[%1]u 0'"
	  "\\s0",
	  span_width_reg(start_col, end_col));
  if (!dont_move)
    prints("\\v'" BAR_HEIGHT "'");
  if (mod->stagger)
    prints("\\v'.5v'");
}

short_double_line_entry::short_double_line_entry(const table *p,
						 const entry_modifier *m)
: simple_entry(p, m)
{
}

int short_double_line_entry::line_type()
{
  return 2;
}

void short_double_line_entry::simple_print(int dont_move)
{
  if (mod->stagger)
    prints("\\v'-.5v'");
  if (!dont_move)
    prints("\\v'-" BAR_HEIGHT "'");
  printfs("\\h'|\\n[%2]u'"
	  "\\v'-" HALF_DOUBLE_LINE_SEP "'"
	  "\\s[\\n[" LINESIZE_REG "]]"
	  "\\D'l \\n[%1]u 0'"
	  "\\v'" DOUBLE_LINE_SEP "'"
	  "\\D'l |\\n[%2]u 0'"
	  "\\s0"
	  "\\v'-" HALF_DOUBLE_LINE_SEP "'",
	  span_width_reg(start_col, end_col),
	  column_start_reg(start_col));
  if (!dont_move)
    prints("\\v'" BAR_HEIGHT "'");
  if (mod->stagger)
    prints("\\v'.5v'");
}

void set_modifier(const entry_modifier *m)
{
  if (!m->font.empty())
    printfs(".ft %1\n", m->font);
  if (m->point_size.val != 0) {
    prints(".ps ");
    if (m->point_size.inc > 0)
      prints('+');
    else if (m->point_size.inc < 0)
      prints('-');
    printfs("%1\n", as_string(m->point_size.val));
  }
  if (m->vertical_spacing.val != 0) {
    prints(".vs ");
    if (m->vertical_spacing.inc > 0)
      prints('+');
    else if (m->vertical_spacing.inc < 0)
      prints('-');
    printfs("%1\n", as_string(m->vertical_spacing.val));
  }
  if (!m->macro.empty())
    printfs(".%1\n", m->macro);
}

void set_inline_modifier(const entry_modifier *m)
{
  if (!m->font.empty())
    printfs("\\f[%1]", m->font);
  if (m->point_size.val != 0) {
    prints("\\s[");
    if (m->point_size.inc > 0)
      prints('+');
    else if (m->point_size.inc < 0)
      prints('-');
    printfs("%1]", as_string(m->point_size.val));
  }
  if (m->stagger)
    prints("\\v'-.5v'");
}

void restore_inline_modifier(const entry_modifier *m)
{
  if (!m->font.empty())
    prints("\\f[\\n[" SAVED_FONT_REG "]]");
  if (m->point_size.val != 0)
    prints("\\s[\\n[" SAVED_SIZE_REG "]]");
  if (m->stagger)
    prints("\\v'.5v'");
}

struct stuff {
  stuff *next;
  int row;			// occurs before row 'row'
  char printed;			// has it been printed?

  stuff(int);
  virtual void print(table *) = 0;
  virtual ~stuff();
  virtual int is_single_line() { return 0; };
  virtual int is_double_line() { return 0; };
};

stuff::stuff(int r) : next(0), row(r), printed(0)
{
}

stuff::~stuff()
{
}

struct text_stuff : public stuff {
  string contents;
  const char *filename;
  int lineno;

  text_stuff(const string &, int, const char *, int);
  ~text_stuff();
  void print(table *);
};

text_stuff::text_stuff(const string &s, int r, const char *fn, int ln)
: stuff(r), contents(s), filename(fn), lineno(ln)
{
}

text_stuff::~text_stuff()
{
}

void text_stuff::print(table *)
{
  printed = 1;
  prints(".cp \\n(" COMPATIBLE_REG "\n");
  set_troff_location(filename, lineno);
  prints(contents);
  prints(".cp 0\n");
  location_force_filename = 1;	// it might have been a .lf command
}

struct single_hline_stuff : public stuff {
  single_hline_stuff(int);
  void print(table *);
  int is_single_line();
};

single_hline_stuff::single_hline_stuff(int r) : stuff(r)
{
}

void single_hline_stuff::print(table *tbl)
{
  printed = 1;
  tbl->print_single_hline(row);
}

int single_hline_stuff::is_single_line()
{
  return 1;
}

struct double_hline_stuff : stuff {
  double_hline_stuff(int);
  void print(table *);
  int is_double_line();
};

double_hline_stuff::double_hline_stuff(int r) : stuff(r)
{
}

void double_hline_stuff::print(table *tbl)
{
  printed = 1;
  tbl->print_double_hline(row);
}

int double_hline_stuff::is_double_line()
{
  return 1;
}

struct vertical_rule {
  vertical_rule *next;
  int start_row;
  int end_row;
  int col;
  char is_double;
  string top_adjust;
  string bot_adjust;

  vertical_rule(int, int, int, int, vertical_rule *);
  ~vertical_rule();
  void contribute_to_bottom_macro(table *);
  void print();
};

vertical_rule::vertical_rule(int sr, int er, int c, int dbl,
			     vertical_rule *p)
: next(p), start_row(sr), end_row(er), col(c), is_double(dbl)
{
}

vertical_rule::~vertical_rule()
{
}

void vertical_rule::contribute_to_bottom_macro(table *tbl)
{
  printfs(".if \\n[" CURRENT_ROW_REG "]>=%1",
	  as_string(start_row));
  if (end_row != tbl->get_nrows() - 1)
    printfs("&(\\n[" CURRENT_ROW_REG "]<%1)",
	    as_string(end_row));
  prints(" \\{");
  printfs(".if %1<=\\n[" LAST_PASSED_ROW_REG "] .nr %2 \\n[#T]\n",
	  as_string(start_row),
	  row_top_reg(start_row));
  const char *offset_table[3];
  if (is_double) {
    offset_table[0] = "-" HALF_DOUBLE_LINE_SEP;
    offset_table[1] = "+" HALF_DOUBLE_LINE_SEP;
    offset_table[2] = 0;
  }
  else {
    offset_table[0] = "";
    offset_table[1] = 0;
  }
  for (const char **offsetp = offset_table; *offsetp; offsetp++) {
    prints(".sp -1\n"
	   "\\v'" BODY_DEPTH);
    if (!bot_adjust.empty())
      printfs("+%1", bot_adjust);
    prints("'");
    printfs("\\h'\\n[%1]u%3'\\s[\\n[" LINESIZE_REG "]]\\D'l 0 |\\n[%2]u-1v",
	    column_divide_reg(col),
	    row_top_reg(start_row),
	    *offsetp);
    if (!bot_adjust.empty())
      printfs("-(%1)", bot_adjust);
    // don't perform the top adjustment if the top is actually #T
    if (!top_adjust.empty())
      printfs("+((%1)*(%2>\\n[" LAST_PASSED_ROW_REG "]))",
	      top_adjust,
	      as_string(start_row));
    prints("'\\s0\n");
  }
  prints(".\\}\n");
}

void vertical_rule::print()
{
  printfs("\\*[" TRANSPARENT_STRING_NAME "]"
	  ".if %1<=\\*[" QUOTE_STRING_NAME "]\\n[" LAST_PASSED_ROW_REG "] "
	  ".nr %2 \\*[" QUOTE_STRING_NAME "]\\n[#T]\n",
	  as_string(start_row),
	  row_top_reg(start_row));
  const char *offset_table[3];
  if (is_double) {
    offset_table[0] = "-" HALF_DOUBLE_LINE_SEP;
    offset_table[1] = "+" HALF_DOUBLE_LINE_SEP;
    offset_table[2] = 0;
  }
  else {
    offset_table[0] = "";
    offset_table[1] = 0;
  }
  for (const char **offsetp = offset_table; *offsetp; offsetp++) {
    prints("\\*[" TRANSPARENT_STRING_NAME "].sp -1\n"
	   "\\*[" TRANSPARENT_STRING_NAME "]\\v'" BODY_DEPTH);
    if (!bot_adjust.empty())
      printfs("+%1", bot_adjust);
    prints("'");
    printfs("\\h'\\n[%1]u%3'"
	    "\\s[\\n[" LINESIZE_REG "]]"
	    "\\D'l 0 |\\*[" QUOTE_STRING_NAME "]\\n[%2]u-1v",
	    column_divide_reg(col),
	    row_top_reg(start_row),
	    *offsetp);
    if (!bot_adjust.empty())
      printfs("-(%1)", bot_adjust);
    // don't perform the top adjustment if the top is actually #T
    if (!top_adjust.empty())
      printfs("+((%1)*(%2>\\*[" QUOTE_STRING_NAME "]\\n["
	      LAST_PASSED_ROW_REG "]))",
	      top_adjust,
	      as_string(start_row));
    prints("'"
	   "\\s0\n");
  }
}

table::table(int nc, unsigned f, int ls, char dpc)
: nrows(0), ncolumns(nc), linesize(ls), decimal_point_char(dpc),
  vrule_list(0), stuff_list(0), span_list(0),
  entry_list(0), entry_list_tailp(&entry_list), entry(0),
  vline(0), row_is_all_lines(0), left_separation(0), right_separation(0),
  total_separation(0), allocated_rows(0), flags(f)
{
  minimum_width = new string[ncolumns];
  column_separation = ncolumns > 1 ? new int[ncolumns - 1] : 0;
  equal = new char[ncolumns];
  expand = new char[ncolumns];
  int i;
  for (i = 0; i < ncolumns; i++) {
    equal[i] = 0;
    expand[i] = 0;
  }
  for (i = 0; i < ncolumns - 1; i++)
    column_separation[i] = DEFAULT_COLUMN_SEPARATION;
  delim[0] = delim[1] = '\0';
}

table::~table()
{
  for (int i = 0; i < nrows; i++) {
    a_delete entry[i];
    a_delete vline[i];
  }
  a_delete entry;
  a_delete vline;
  while (entry_list) {
    table_entry *tem = entry_list;
    entry_list = entry_list->next;
    delete tem;
  }
  ad_delete(ncolumns) minimum_width;
  a_delete column_separation;
  a_delete equal;
  a_delete expand;
  while (stuff_list) {
    stuff *tem = stuff_list;
    stuff_list = stuff_list->next;
    delete tem;
  }
  while (vrule_list) {
    vertical_rule *tem = vrule_list;
    vrule_list = vrule_list->next;
    delete tem;
  }
  a_delete row_is_all_lines;
  while (span_list) {
    horizontal_span *tem = span_list;
    span_list = span_list->next;
    delete tem;
  }
}

void table::set_delim(char c1, char c2)
{
  delim[0] = c1;
  delim[1] = c2;
}

void table::set_minimum_width(int c, const string &w)
{
  assert(c >= 0 && c < ncolumns);
  minimum_width[c] = w;
}

void table::set_column_separation(int c, int n)
{
  assert(c >= 0 && c < ncolumns - 1);
  column_separation[c] = n;
}

void table::set_equal_column(int c)
{
  assert(c >= 0 && c < ncolumns);
  equal[c] = 1;
}

void table::set_expand_column(int c)
{
  assert(c >= 0 && c < ncolumns);
  expand[c] = 1;
}

void table::add_stuff(stuff *p)
{
  stuff **pp;
  for (pp = &stuff_list; *pp; pp = &(*pp)->next)
    ;
  *pp = p;
}

void table::add_text_line(int r, const string &s, const char *filename,
			  int lineno)
{
  add_stuff(new text_stuff(s, r, filename, lineno));
}

void table::add_single_hline(int r)
{
  add_stuff(new single_hline_stuff(r));
}

void table::add_double_hline(int r)
{
  add_stuff(new double_hline_stuff(r));
}

void table::allocate(int r)
{
  if (r >= nrows) {
    typedef table_entry **PPtable_entry; // work around g++ 1.36.1 bug
    if (r >= allocated_rows) {
      if (allocated_rows == 0) {
	allocated_rows = 16;
	if (allocated_rows <= r)
	  allocated_rows = r + 1;
	entry = new PPtable_entry[allocated_rows];
	vline = new char*[allocated_rows];
      }
      else {
	table_entry ***old_entry = entry;
	int old_allocated_rows = allocated_rows;
	allocated_rows *= 2;
	if (allocated_rows <= r)
	  allocated_rows = r + 1;
	entry = new PPtable_entry[allocated_rows];
	memcpy(entry, old_entry, sizeof(table_entry**)*old_allocated_rows);
	a_delete old_entry;
	char **old_vline = vline;
	vline = new char*[allocated_rows];
	memcpy(vline, old_vline, sizeof(char*)*old_allocated_rows);
	a_delete old_vline;
      }
    }
    assert(allocated_rows > r);
    while (nrows <= r) {
      entry[nrows] = new table_entry*[ncolumns];
      int i;
      for (i = 0; i < ncolumns; i++)
	entry[nrows][i] = 0;
      vline[nrows] = new char[ncolumns+1];
      for (i = 0; i < ncolumns+1; i++)
	vline[nrows][i] = 0;
      nrows++;
    }
  }
}

void table::do_hspan(int r, int c)
{
  assert(r >= 0 && c >= 0 && r < nrows && c < ncolumns);
  if (c == 0) {
    error("first column cannot be horizontally spanned");
    return;
  }
  table_entry *e = entry[r][c];
  if (e) {
    assert(e->start_row <= r && r <= e->end_row
	   && e->start_col <= c && c <= e->end_col
	   && e->end_row - e->start_row > 0
	   && e->end_col - e->start_col > 0);
    return;
  }
  e = entry[r][c-1];
  // e can be 0 if we had an empty entry or an error
  if (e == 0)
    return;
  if (e->start_row != r) {
    /*
      l l
      ^ s */
    error("impossible horizontal span at row %1, column %2", r + 1, c + 1);
  }
  else {
    e->end_col = c;
    entry[r][c] = e;
  }
}

void table::do_vspan(int r, int c)
{
  assert(r >= 0 && c >= 0 && r < nrows && c < ncolumns);
  if (r == 0) {
    error("first row cannot be vertically spanned");
    return;
  }
  table_entry *e = entry[r][c];
  if (e) {
    assert(e->start_row <= r && r <= e->end_row
	   && e->start_col <= c && c <= e->end_col
	   && e->end_row - e->start_row > 0
	   && e->end_col - e->start_col > 0);
    return;
  }
  e = entry[r-1][c];
  // e can be 0 if we had an empty entry or an error
  if (e == 0)
    return;
  if (e->start_col != c) {
    /* l s
       l ^ */
    error("impossible vertical span at row %1, column %2", r + 1, c + 1);
  }
  else {
    for (int i = c; i <= e->end_col; i++) {
      assert(entry[r][i] == 0);
      entry[r][i] = e;
    }
    e->end_row = r;
  }
}

int find_decimal_point(const char *s, char decimal_point_char,
		       const char *delim)
{
  if (s == 0 || *s == '\0')
    return -1;
  const char *p;
  int in_delim = 0;		// is p within eqn delimiters?
  // tbl recognises \& even within eqn delimiters; I don't
  for (p = s; *p; p++)
    if (in_delim) {
      if (*p == delim[1])
	in_delim = 0;
    }
    else if (*p == delim[0])
      in_delim = 1;
    else if (p[0] == '\\' && p[1] == '&')
      return p - s;
  int possible_pos = -1;
  in_delim = 0;
  for (p = s; *p; p++)
    if (in_delim) {
      if (*p == delim[1])
	in_delim = 0;
    }
    else if (*p == delim[0])
      in_delim = 1;
    else if (p[0] == decimal_point_char && csdigit(p[1]))
      possible_pos = p - s;
  if (possible_pos >= 0)
    return possible_pos;
  in_delim = 0;
  for (p = s; *p; p++)
    if (in_delim) {
      if (*p == delim[1])
	in_delim = 0;
    }
    else if (*p == delim[0])
      in_delim = 1;
    else if (csdigit(*p))
      possible_pos = p + 1 - s;
  return possible_pos;
}

void table::add_entry(int r, int c, const string &str, const entry_format *f,
		      const char *fn, int ln)
{
  allocate(r);
  table_entry *e = 0;
  if (str == "\\_") {
    e = new short_line_entry(this, f);
  }
  else if (str == "\\=") {
    e = new short_double_line_entry(this, f);
  }
  else if (str == "_") {
    single_line_entry *lefte;
    if (c > 0 && entry[r][c-1] != 0 &&
	(lefte = entry[r][c-1]->to_single_line_entry()) != 0
	&& lefte->start_row == r
	&& lefte->mod->stagger == f->stagger) {
      lefte->end_col = c;
      entry[r][c] = lefte;
    }
    else
      e = new single_line_entry(this, f);
  }
  else if (str == "=") {
    double_line_entry *lefte;
    if (c > 0 && entry[r][c-1] != 0 &&
	(lefte = entry[r][c-1]->to_double_line_entry()) != 0
	&& lefte->start_row == r
	&& lefte->mod->stagger == f->stagger) {
      lefte->end_col = c;
      entry[r][c] = lefte;
    }
    else
      e = new double_line_entry(this, f);
  }
  else if (str == "\\^") {
    do_vspan(r, c);
  }
  else if (str.length() > 2 && str[0] == '\\' && str[1] == 'R') {
    if (str.search('\n') >= 0)
      error_with_file_and_line(fn, ln, "bad repeated character");
    else {
      char *s = str.substring(2, str.length() - 2).extract();
      e = new repeated_char_entry(this, f, s);
    }
  }
  else {
    int is_block = str.search('\n') >= 0;
    char *s;
    switch (f->type) {
    case FORMAT_SPAN:
      assert(str.empty());
      do_hspan(r, c);
      break;
    case FORMAT_LEFT:
      if (!str.empty()) {
	s = str.extract();
	if (is_block)
	  e = new left_block_entry(this, f, s);
	else
	  e = new left_text_entry(this, f, s);
      }
      else
	e = new empty_entry(this, f);
      break;
    case FORMAT_CENTER:
      if (!str.empty()) {
	s = str.extract();
	if (is_block)
	  e = new center_block_entry(this, f, s);
	else
	  e = new center_text_entry(this, f, s);
      }
      else
	e = new empty_entry(this, f);
      break;
    case FORMAT_RIGHT:
      if (!str.empty()) {
	s = str.extract();
	if (is_block)
	  e = new right_block_entry(this, f, s);
	else
	  e = new right_text_entry(this, f, s);
      }
      else
	e = new empty_entry(this, f);
      break;
    case FORMAT_NUMERIC:
      if (!str.empty()) {
	s = str.extract();
	if (is_block) {
	  error_with_file_and_line(fn, ln, "can't have numeric text block");
	  e = new left_block_entry(this, f, s);
	}
	else {
	  int pos = find_decimal_point(s, decimal_point_char, delim);
	  if (pos < 0)
	    e = new center_text_entry(this, f, s);
	  else
	    e = new numeric_text_entry(this, f, s, pos);
	}
      }
      else
	e = new empty_entry(this, f);
      break;
    case FORMAT_ALPHABETIC:
      if (!str.empty()) {
	s = str.extract();
	if (is_block)
	  e = new alphabetic_block_entry(this, f, s);
	else
	  e = new alphabetic_text_entry(this, f, s);
      }
      else
	e = new empty_entry(this, f);
      break;
    case FORMAT_VSPAN:
      do_vspan(r, c);
      break;
    case FORMAT_HLINE:
      if (str.length() != 0)
	error_with_file_and_line(fn, ln,
				 "non-empty data entry for '_' format ignored");
      e = new single_line_entry(this, f);
      break;
    case FORMAT_DOUBLE_HLINE:
      if (str.length() != 0)
	error_with_file_and_line(fn, ln,
				 "non-empty data entry for '=' format ignored");
      e = new double_line_entry(this, f);
      break;
    default:
      assert(0);
    }
  }
  if (e) {
    table_entry *preve = entry[r][c];
    if (preve) {
      /* c s
         ^ l */
      error_with_file_and_line(fn, ln, "row %1, column %2 already spanned",
			       r + 1, c + 1);
      delete e;
    }
    else {
      e->input_lineno = ln;
      e->input_filename = fn;
      e->start_row = e->end_row = r;
      e->start_col = e->end_col = c;
      *entry_list_tailp = e;
      entry_list_tailp = &e->next;
      entry[r][c] = e;
    }
  }
}

// add vertical lines for row r

void table::add_vlines(int r, const char *v)
{
  allocate(r);
  for (int i = 0; i < ncolumns+1; i++)
    vline[r][i] = v[i];
}

void table::check()
{
  table_entry *p = entry_list;
  int i, j;
  while (p) {
    for (i = p->start_row; i <= p->end_row; i++)
      for (j = p->start_col; j <= p->end_col; j++)
	assert(entry[i][j] == p);
    p = p->next;
  }
}

void table::print()
{
  location_force_filename = 1;
  check();
  init_output();
  determine_row_type();
  compute_widths();
  if (!(flags & CENTER))
    prints(".if \\n[" SAVED_CENTER_REG "] \\{");
  prints(".in +(u;\\n[.l]-\\n[.i]-\\n[TW]/2>?-\\n[.i])\n"
	 ".nr " SAVED_INDENT_REG " \\n[.i]\n");
  if (!(flags & CENTER))
    prints(".\\}\n");
  build_vrule_list();
  define_bottom_macro();
  do_top();
  for (int i = 0; i < nrows; i++)
    do_row(i);
  do_bottom();
}

void table::determine_row_type()
{
  row_is_all_lines = new char[nrows];
  for (int i = 0; i < nrows; i++) {
    int had_single = 0;
    int had_double = 0;
    int had_non_line = 0;
    for (int c = 0; c < ncolumns; c++) {
      table_entry *e = entry[i][c];
      if (e != 0) {
	if (e->start_row == e->end_row) {
	  int t = e->line_type();
	  switch (t) {
	  case -1:
	    had_non_line = 1;
	    break;
	  case 0:
	    // empty
	    break;
	  case 1:
	    had_single = 1;
	    break;
	  case 2:
	    had_double = 1;
	    break;
	  default:
	    assert(0);
	  }
	  if (had_non_line)
	    break;
	}
	c = e->end_col;
      }
    }
    if (had_non_line)
      row_is_all_lines[i] = 0;
    else if (had_double)
      row_is_all_lines[i] = 2;
    else if (had_single)
      row_is_all_lines[i] = 1;
    else
      row_is_all_lines[i] = 0;
  }
}

int table::count_expand_columns()
{
  int count = 0;
  for (int i = 0; i < ncolumns; i++)
    if (expand[i])
      count++;
  return count;
}

void table::init_output()
{
  prints(".nr " COMPATIBLE_REG " \\n(.C\n"
	 ".cp 0\n");
  if (linesize > 0)
    printfs(".nr " LINESIZE_REG " %1\n", as_string(linesize));
  else
    prints(".nr " LINESIZE_REG " \\n[.s]\n");
  if (!(flags & CENTER))
    prints(".nr " SAVED_CENTER_REG " \\n[.ce]\n");
  if (compatible_flag)
    prints(".ds " LEADER_REG " \\a\n");
  prints(".de " RESET_MACRO_NAME "\n"
	 ".ft \\n[.f]\n"
	 ".ps \\n[.s]\n"
	 ".vs \\n[.v]u\n"
	 ".in \\n[.i]u\n"
	 ".ll \\n[.l]u\n"
	 ".ls \\n[.L]\n"
	 ".ad \\n[.j]\n"
	 ".ie \\n[.u] .fi\n"
	 ".el .nf\n"
	 ".ce \\n[.ce]\n"
	 "..\n"
	 ".nr " SAVED_INDENT_REG " \\n[.i]\n"
	 ".nr " SAVED_FONT_REG " \\n[.f]\n"
	 ".nr " SAVED_SIZE_REG " \\n[.s]\n"
	 ".nr " SAVED_FILL_REG " \\n[.u]\n"
	 ".nr T. 0\n"
	 ".nr " CURRENT_ROW_REG " 0-1\n"
	 ".nr " LAST_PASSED_ROW_REG " 0-1\n"
	 ".nr " SECTION_DIVERSION_FLAG_REG " 0\n"
	 ".ds " TRANSPARENT_STRING_NAME "\n"
	 ".ds " QUOTE_STRING_NAME "\n"
	 ".nr " NEED_BOTTOM_RULE_REG " 1\n"
	 ".nr " SUPPRESS_BOTTOM_REG " 0\n"
	 ".eo\n"
	 ".de " REPEATED_MARK_MACRO "\n"
	 ".mk \\$1\n"
	 ".if !'\\n(.z'' \\!." REPEATED_MARK_MACRO " \"\\$1\"\n"
	 "..\n"
	 ".de " REPEATED_VPT_MACRO "\n"
	 ".vpt \\$1\n"
	 ".if !'\\n(.z'' \\!." REPEATED_VPT_MACRO " \"\\$1\"\n"
	 "..\n"
	 ".de " REPEATED_NM_SET_MACRO "\n"
	 ".ie !'\\n(.z'' \\{.nm\n"
	 "\\!." REPEATED_NM_SET_MACRO " \"\\$1\"\n"
	 ".\\}\n"
	 ".el .if \\n[ln] \\{\\\n"
	 ".if '\\$1'd' .nr " ROW_START_LINE_REG " \\n[ln]\n"
	 ".if '\\$1's' .nm \\n[" ROW_START_LINE_REG "]\n"
	 ".if '\\$1'm' .nr " ROW_MAX_LINE_REG " \\n[ln]>?\\n[" ROW_MAX_LINE_REG "]\n"
	 ".\\}\n"
	 "..\n"
	 ".de " REPEATED_NM_SUS_MACRO "\n"
	 ".ie !'\\n(.z'' \\{.nm\n"
	 "\\!." REPEATED_NM_SUS_MACRO " \"\\$1\"\n"
	 ".\\}\n"
	 ".el .if \\n[ln] \\{\\\n"
	 ".ie '\\$1's' \\{\\\n"
	 ".nr " ROW_SAVE_LINE_REG " \\n(ln<?\\n[" ROW_MAX_LINE_REG "]\n"
	 ".nm +0 \\n[ln]+42\n"
	 ".\\}\n"
	 ".el \\{\\\n"
	 ".nr ln \\n[" ROW_SAVE_LINE_REG "]\n"
	 ".nm \\n[ln] 1\n"
	 ".\\}\n"
	 ".\\}\n"
	 "..\n");
  if (!(flags & NOKEEP))
    prints(".de " KEEP_MACRO_NAME "\n"
	   ".if '\\n[.z]'' \\{.ds " QUOTE_STRING_NAME " \\\\\n"
	   ".ds " TRANSPARENT_STRING_NAME " \\!\n"
	   ".di " SECTION_DIVERSION_NAME "\n"
	   ".nr " SECTION_DIVERSION_FLAG_REG " 1\n"
	   ".in 0\n"
	   ".\\}\n"
	   "..\n"
	   // protect # in macro name against eqn
	   ".ig\n"
	   ".EQ\n"
	   "delim off\n"
	   ".EN\n"
	   "..\n"
	   ".de " RELEASE_MACRO_NAME "\n"
	   ".if \\n[" SECTION_DIVERSION_FLAG_REG "] \\{"
	   ".di\n"
	   ".in \\n[" SAVED_INDENT_REG "]u\n"
	   ".nr " SAVED_DN_REG " \\n[dn]\n"
	   ".ds " QUOTE_STRING_NAME "\n"
	   ".ds " TRANSPARENT_STRING_NAME "\n"
	   ".nr " SECTION_DIVERSION_FLAG_REG " 0\n"
	   ".if \\n[.t]<=\\n[dn] \\{"
	   ".nr T. 1\n"
	   ".T#\n"
	   ".nr " SUPPRESS_BOTTOM_REG " 1\n"
	   ".sp \\n[.t]u\n"
	   ".nr " SUPPRESS_BOTTOM_REG " 0\n"
	   ".mk #T\n"
	   ".\\}\n"
	   ".if \\n[.t]<=\\n[" SAVED_DN_REG "] "
	   /* Since we turn off traps, it won't get into an infinite loop
	   when we try and print it; it will just go off the bottom of the
	   page. */
	   ".tm warning: page \\n%: table text block will not fit on one page\n"
	   ".nf\n"
	   ".if \\n[ln] .nm \\n[ln]\n"
	   ".nr " ROW_MAX_LINE_REG " \\n[ln]\n"
	   ".ls 1\n"
	   "." SECTION_DIVERSION_NAME "\n"
	   ".ls\n"
	   ".if \\n[ln] .nm\n"
	   ".rm " SECTION_DIVERSION_NAME "\n"
	   ".\\}\n"
	   "..\n"
	   ".ig\n"
	   ".EQ\n"
	   "delim on\n"
	   ".EN\n"
	   "..\n"
	   ".nr " TABLE_DIVERSION_FLAG_REG " 0\n"
	   ".de " TABLE_KEEP_MACRO_NAME "\n"
	   ".if '\\n[.z]'' \\{"
	   ".di " TABLE_DIVERSION_NAME "\n"
	   ".nr " TABLE_DIVERSION_FLAG_REG " 1\n"
	   ".\\}\n"
	   "..\n"
	   ".de " TABLE_RELEASE_MACRO_NAME "\n"
	   ".if \\n[" TABLE_DIVERSION_FLAG_REG "] \\{.br\n"
	   ".di\n"
	   ".nr " SAVED_DN_REG " \\n[dn]\n"
	   ".ne \\n[dn]u+\\n[.V]u\n"
	   ".ie \\n[.t]<=\\n[" SAVED_DN_REG "] "
	   ".tm error: page \\n%: table will not fit on one page; use .TS H/.TH with a supporting macro package\n"
	   ".el \\{"
	   ".in 0\n"
	   ".ls 1\n"
	   ".nf\n"
	   ".if \\n[ln] .nm \\n[ln]\n"
	   "." TABLE_DIVERSION_NAME "\n"
	   ".\\}\n"
	   ".rm " TABLE_DIVERSION_NAME "\n"
	   ".\\}\n"
	   ".if \\n[ln] \\{.nm\n"
	   ".nr ln \\n[" ROW_MAX_LINE_REG "]\n"
	   ".\\}\n"
	   "..\n");
  prints(".ec\n"
	 ".ce 0\n"
	 ".nf\n");
}

string block_width_reg(int r, int c)
{
  static char name[sizeof(BLOCK_WIDTH_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, BLOCK_WIDTH_PREFIX "%d,%d", r, c);
  return string(name);
}

string block_diversion_name(int r, int c)
{
  static char name[sizeof(BLOCK_DIVERSION_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, BLOCK_DIVERSION_PREFIX "%d,%d", r, c);
  return string(name);
}

string block_height_reg(int r, int c)
{
  static char name[sizeof(BLOCK_HEIGHT_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, BLOCK_HEIGHT_PREFIX "%d,%d", r, c);
  return string(name);
}

string span_width_reg(int start_col, int end_col)
{
  static char name[sizeof(SPAN_WIDTH_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, SPAN_WIDTH_PREFIX "%d", start_col);
  if (end_col != start_col)
    sprintf(strchr(name, '\0'), ",%d", end_col);
  return string(name);
}

string span_left_numeric_width_reg(int start_col, int end_col)
{
  static char name[sizeof(SPAN_LEFT_NUMERIC_WIDTH_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, SPAN_LEFT_NUMERIC_WIDTH_PREFIX "%d", start_col);
  if (end_col != start_col)
    sprintf(strchr(name, '\0'), ",%d", end_col);
  return string(name);
}

string span_right_numeric_width_reg(int start_col, int end_col)
{
  static char name[sizeof(SPAN_RIGHT_NUMERIC_WIDTH_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, SPAN_RIGHT_NUMERIC_WIDTH_PREFIX "%d", start_col);
  if (end_col != start_col)
    sprintf(strchr(name, '\0'), ",%d", end_col);
  return string(name);
}

string span_alphabetic_width_reg(int start_col, int end_col)
{
  static char name[sizeof(SPAN_ALPHABETIC_WIDTH_PREFIX)+INT_DIGITS+1+INT_DIGITS];
  sprintf(name, SPAN_ALPHABETIC_WIDTH_PREFIX "%d", start_col);
  if (end_col != start_col)
    sprintf(strchr(name, '\0'), ",%d", end_col);
  return string(name);
}

string column_separation_reg(int col)
{
  static char name[sizeof(COLUMN_SEPARATION_PREFIX)+INT_DIGITS];
  sprintf(name, COLUMN_SEPARATION_PREFIX "%d", col);
  return string(name);
}

string row_start_reg(int row)
{
  static char name[sizeof(ROW_START_PREFIX)+INT_DIGITS];
  sprintf(name, ROW_START_PREFIX "%d", row);
  return string(name);
}  

string column_start_reg(int col)
{
  static char name[sizeof(COLUMN_START_PREFIX)+INT_DIGITS];
  sprintf(name, COLUMN_START_PREFIX "%d", col);
  return string(name);
}  

string column_end_reg(int col)
{
  static char name[sizeof(COLUMN_END_PREFIX)+INT_DIGITS];
  sprintf(name, COLUMN_END_PREFIX "%d", col);
  return string(name);
}

string column_divide_reg(int col)
{
  static char name[sizeof(COLUMN_DIVIDE_PREFIX)+INT_DIGITS];
  sprintf(name, COLUMN_DIVIDE_PREFIX "%d", col);
  return string(name);
}

string row_top_reg(int row)
{
  static char name[sizeof(ROW_TOP_PREFIX)+INT_DIGITS];
  sprintf(name, ROW_TOP_PREFIX "%d", row);
  return string(name);
}

void init_span_reg(int start_col, int end_col)
{
  printfs(".nr %1 \\n(.H\n.nr %2 0\n.nr %3 0\n.nr %4 0\n",
	  span_width_reg(start_col, end_col),
	  span_alphabetic_width_reg(start_col, end_col),
	  span_left_numeric_width_reg(start_col, end_col),
	  span_right_numeric_width_reg(start_col, end_col));
}

void compute_span_width(int start_col, int end_col)
{
  printfs(".nr %1 \\n[%1]>?(\\n[%2]+\\n[%3])\n"
	  ".if \\n[%4] .nr %1 \\n[%1]>?(\\n[%4]+2n)\n", 
	  span_width_reg(start_col, end_col),
	  span_left_numeric_width_reg(start_col, end_col),
	  span_right_numeric_width_reg(start_col, end_col),
	  span_alphabetic_width_reg(start_col, end_col));
}

// Increase the widths of columns so that the width of any spanning entry
// is not greater than the sum of the widths of the columns that it spans.
// Ensure that the widths of columns remain equal.

void table::divide_span(int start_col, int end_col)
{
  assert(end_col > start_col);
  printfs(".nr " NEEDED_REG " \\n[%1]-(\\n[%2]", 
	  span_width_reg(start_col, end_col),
	  span_width_reg(start_col, start_col));
  int i;
  for (i = start_col + 1; i <= end_col; i++) {
    // The column separation may shrink with the expand option.
    if (!(flags & EXPAND))
      printfs("+%1n", as_string(column_separation[i - 1]));
    printfs("+\\n[%1]", span_width_reg(i, i));
  }
  prints(")\n");
  printfs(".nr " NEEDED_REG " \\n[" NEEDED_REG "]/%1\n",
	  as_string(end_col - start_col + 1));
  prints(".if \\n[" NEEDED_REG "] \\{");
  for (i = start_col; i <= end_col; i++)
    printfs(".nr %1 +\\n[" NEEDED_REG "]\n", 
	    span_width_reg(i, i));
  int equal_flag = 0;
  for (i = start_col; i <= end_col && !equal_flag; i++)
    if (equal[i] || expand[i])
      equal_flag = 1;
  if (equal_flag) {
    for (i = 0; i < ncolumns; i++)
      if (i < start_col || i > end_col)
	printfs(".nr %1 +\\n[" NEEDED_REG "]\n", 
	    span_width_reg(i, i));
  }
  prints(".\\}\n");
}

void table::sum_columns(int start_col, int end_col, int do_expand)
{
  assert(end_col > start_col);
  int i;
  for (i = start_col; i <= end_col; i++)
    if (expand[i])
      break;
  if (i > end_col) {
    if (do_expand)
      return;
  }
  else {
    if (!do_expand)
      return;
  }
  printfs(".nr %1 \\n[%2]", 
	  span_width_reg(start_col, end_col),
	  span_width_reg(start_col, start_col));
  for (i = start_col + 1; i <= end_col; i++)
    printfs("+(%1*\\n[" SEPARATION_FACTOR_REG "])+\\n[%2]",
	    as_string(column_separation[i - 1]),
	    span_width_reg(i, i));
  prints('\n');
}

horizontal_span::horizontal_span(int sc, int ec, horizontal_span *p)
: next(p), start_col(sc), end_col(ec)
{
}

void table::build_span_list()
{
  span_list = 0;
  table_entry *p = entry_list;
  while (p) {
    if (p->end_col != p->start_col) {
      horizontal_span *q;
      for (q = span_list; q; q = q->next)
	if (q->start_col == p->start_col
	    && q->end_col == p->end_col)
	  break;
      if (!q)
	span_list = new horizontal_span(p->start_col, p->end_col, span_list);
    }
    p = p->next;
  }
  // Now sort span_list primarily by order of end_row, and secondarily
  // by reverse order of start_row. This ensures that if we divide
  // spans using the order in span_list, we will get reasonable results.
  horizontal_span *unsorted = span_list;
  span_list = 0;
  while (unsorted) {
    horizontal_span **pp;
    for (pp = &span_list; *pp; pp = &(*pp)->next)
      if (unsorted->end_col < (*pp)->end_col
	  || (unsorted->end_col == (*pp)->end_col
	      && (unsorted->start_col > (*pp)->start_col)))
	break;
    horizontal_span *tem = unsorted->next;
    unsorted->next = *pp;
    *pp = unsorted;
    unsorted = tem;
  }
}

void table::compute_expand_width()
{
  int i;
  int colcount = count_expand_columns();
  prints(".nr " EXPAND_REG " \\n[.l]-\\n[.i]");
  for (i = 0; i < ncolumns; i++)
    if (!expand[i])
      printfs("-\\n[%1]", span_width_reg(i, i));
  if (total_separation)
    printfs("-%1n", as_string(total_separation));
  prints("\n");
  prints(".if \\n[" EXPAND_REG "]<0 \\{\\\n");
  entry_list->set_location();
  if (!(flags & NOWARN)) {
    // protect ` and ' in warning message against eqn
    prints(".ig\n"
	   ".EQ\n"
	   "delim off\n"
	   ".EN\n"
	   "..\n");
    prints(".tm1 \"warning: file '\\n[.F]', around line \\n[.c]:\n"
	   ".tm1 \"  table wider than line width\n");
    prints(".ig\n"
	   ".EQ\n"
	   "delim on\n"
	   ".EN\n"
	   "..\n");
    prints(".nr " EXPAND_REG " 0\n");
  }
  prints(".\\}\n");
  if (colcount > 1)
    printfs(".nr " EXPAND_REG " \\n[" EXPAND_REG "]/%1\n",
	    as_string(colcount));
  for (i = 0; i < ncolumns; i++)
    if (expand[i])
      printfs(".nr %1 \\n[%1]>?\\n[" EXPAND_REG "]\n", span_width_reg(i, i));
}

void table::compute_total_separation()
{
  if (flags & (ALLBOX | BOX | DOUBLEBOX))
    left_separation = right_separation = 1;
  else {
    for (int i = 0; i < nrows; i++) {
      if (vline[i][0] > 0)
	left_separation = 1;
      if (vline[i][ncolumns] > 0)
	right_separation = 1;
    }
  }
  total_separation = left_separation + right_separation;
  int i;
  for (i = 0; i < ncolumns - 1; i++)
    total_separation += column_separation[i];
}

void table::compute_separation_factor()
{
  // Don't let the separation factor be negative.
  prints(".nr " SEPARATION_FACTOR_REG " \\n[.l]-\\n[.i]");
  for (int i = 0; i < ncolumns; i++)
    printfs("-\\n[%1]", span_width_reg(i, i));
  printfs("/%1\n", as_string(total_separation));
  prints(".ie \\n[" SEPARATION_FACTOR_REG "]<=0 \\{\\\n");
  entry_list->set_location();
  if (!(flags & NOWARN)) {
    // protect ` and ' in warning message against eqn
    prints(".ig\n"
	   ".EQ\n"
	   "delim off\n"
	   ".EN\n"
	   "..\n");
    prints(".tm1 \"warning: file '\\n[.F]', around line \\n[.c]:\n"
	   ".tm1 \"  column separation set to zero\n"
	   ".nr " SEPARATION_FACTOR_REG " 0\n");
  }
  prints(".\\}\n"
	 ".el .if \\n[" SEPARATION_FACTOR_REG "]<1n \\{\\\n");
  entry_list->set_location();
  if (!(flags & NOWARN)) {
    prints(".tm1 \"warning: file '\\n[.F]', around line \\n[.c]:\n"
	   ".tm1 \"  table squeezed horizontally to fit line length\n");
    prints(".ig\n"
	   ".EQ\n"
	   "delim on\n"
	   ".EN\n"
	   "..\n");
  }
  prints(".\\}\n");
}

void table::compute_column_positions()
{
  printfs(".nr %1 0\n", column_divide_reg(0));
  printfs(".nr %1 %2*\\n[" SEPARATION_FACTOR_REG "]\n",
	  column_start_reg(0),
	  as_string(left_separation));
  int i;
  for (i = 1;; i++) {
    printfs(".nr %1 \\n[%2]+\\n[%3]\n",
	    column_end_reg(i-1),
	    column_start_reg(i-1),
	    span_width_reg(i-1, i-1));
    if (i >= ncolumns)
      break;
    printfs(".nr %1 \\n[%2]+(%3*\\n[" SEPARATION_FACTOR_REG "])\n",
	    column_start_reg(i),
	    column_end_reg(i-1),
	    as_string(column_separation[i-1]));
    printfs(".nr %1 \\n[%2]+\\n[%3]/2\n",
	    column_divide_reg(i),
	    column_end_reg(i-1),
	    column_start_reg(i));
  }
  printfs(".nr %1 \\n[%2]+(%3*\\n[" SEPARATION_FACTOR_REG "])\n",
	  column_divide_reg(ncolumns),
	  column_end_reg(i-1),
	  as_string(right_separation));
  printfs(".nr TW \\n[%1]\n",
	  column_divide_reg(ncolumns));
  if (flags & DOUBLEBOX) {
    printfs(".nr %1 +" DOUBLE_LINE_SEP "\n", column_divide_reg(0));
    printfs(".nr %1 -" DOUBLE_LINE_SEP "\n", column_divide_reg(ncolumns));
  }
}

void table::make_columns_equal()
{
  int first = -1;		// index of first equal column
  int i;
  for (i = 0; i < ncolumns; i++)
    if (equal[i]) {
      if (first < 0) {
	printfs(".nr %1 \\n[%1]", span_width_reg(i, i));
	first = i;
      }
      else
	printfs(">?\\n[%1]", span_width_reg(i, i));
    }
  if (first >= 0) {
    prints('\n');
    for (i = first + 1; i < ncolumns; i++)
      if (equal[i])
	printfs(".nr %1 \\n[%2]\n", 
		span_width_reg(i, i),
		span_width_reg(first, first));
  }
}

void table::compute_widths()
{
  build_span_list();
  int i;
  horizontal_span *p;
  // These values get refined later.
  prints(".nr " SEPARATION_FACTOR_REG " 1n\n");
  for (i = 0; i < ncolumns; i++) {
    init_span_reg(i, i);
    if (!minimum_width[i].empty())
      printfs(".nr %1 (n;%2)\n", span_width_reg(i, i), minimum_width[i]);
  }
  for (p = span_list; p; p = p->next)
    init_span_reg(p->start_col, p->end_col);
  // Compute all field widths except for blocks.
  table_entry *q;
  for (q = entry_list; q; q = q->next)
    if (!q->mod->zero_width)
      q->do_width();
  // Compute all span widths, not handling blocks yet.
  for (i = 0; i < ncolumns; i++)
    compute_span_width(i, i);
  for (p = span_list; p; p = p->next)
    compute_span_width(p->start_col, p->end_col);
  // Making columns equal normally increases the width of some columns.
  make_columns_equal();
  // Note that divide_span keeps equal width columns equal.
  // This function might increase the width of some columns, too.
  for (p = span_list; p; p = p->next)
    divide_span(p->start_col, p->end_col);
  compute_total_separation();
  for (p = span_list; p; p = p->next)
    sum_columns(p->start_col, p->end_col, 0);
  // Now handle unexpanded blocks.
  int had_spanning_block = 0;
  int had_equal_block = 0;
  for (q = entry_list; q; q = q->next)
    if (q->divert(ncolumns, minimum_width,
		  (flags & EXPAND) ? column_separation : 0, 0)) {
      if (q->end_col > q->start_col)
	had_spanning_block = 1;
      for (i = q->start_col; i <= q->end_col && !had_equal_block; i++)
	if (equal[i])
	  had_equal_block = 1;
    }
  // Adjust widths.
  if (had_equal_block)
    make_columns_equal();
  if (had_spanning_block)
    for (p = span_list; p; p = p->next)
      divide_span(p->start_col, p->end_col);
  compute_expand_width();
  if ((flags & EXPAND) && total_separation != 0) {
    compute_separation_factor();
    for (p = span_list; p; p = p->next)
      sum_columns(p->start_col, p->end_col, 0);
  }
  else {
    // Handle expanded blocks.
    for (p = span_list; p; p = p->next)
      sum_columns(p->start_col, p->end_col, 1);
    for (q = entry_list; q; q = q->next)
      if (q->divert(ncolumns, minimum_width, 0, 1)) {
	if (q->end_col > q->start_col)
	  had_spanning_block = 1;
      }
    // Adjust widths again.
    if (had_spanning_block)
      for (p = span_list; p; p = p->next)
	divide_span(p->start_col, p->end_col);
  }
  compute_column_positions();
}

void table::print_single_hline(int r)
{
  prints(".vs " LINE_SEP ">?\\n[.V]u\n"
	 ".ls 1\n"
	 "." REPEATED_NM_SUS_MACRO " s\n"
	 "\\v'" BODY_DEPTH "'"
	 "\\s[\\n[" LINESIZE_REG "]]");
  if (r > nrows - 1)
    prints("\\D'l |\\n[TW]u 0'");
  else {
    int start_col = 0;
    for (;;) {
      while (start_col < ncolumns 
	     && entry[r][start_col] != 0
	     && entry[r][start_col]->start_row != r)
	start_col++;
      int end_col;
      for (end_col = start_col;
	   end_col < ncolumns
	   && (entry[r][end_col] == 0
	       || entry[r][end_col]->start_row == r);
	   end_col++)
	;
      if (end_col <= start_col)
	break;
      printfs("\\h'|\\n[%1]u",
	      column_divide_reg(start_col));
      if ((r > 0 && vline[r-1][start_col] == 2)
	  || (r < nrows && vline[r][start_col] == 2))
	prints("-" HALF_DOUBLE_LINE_SEP);
      prints("'");
      printfs("\\D'l |\\n[%1]u",
	      column_divide_reg(end_col));
      if ((r > 0 && vline[r-1][end_col] == 2)
	  || (r < nrows && vline[r][end_col] == 2))
	prints("+" HALF_DOUBLE_LINE_SEP);
      prints(" 0'");
      start_col = end_col;
    }
  }
  prints("\\s0\n");
  prints("." REPEATED_NM_SUS_MACRO " r\n"
	 ".ls\n"
	 ".vs\n");
}

void table::print_double_hline(int r)
{
  prints(".vs " LINE_SEP "+" DOUBLE_LINE_SEP
	 ">?\\n[.V]u\n"
	 ".ls 1\n"
	 "." REPEATED_NM_SUS_MACRO " s\n"
	 "\\v'" BODY_DEPTH "'"
	 "\\s[\\n[" LINESIZE_REG "]]");
  if (r > nrows - 1)
    prints("\\v'-" DOUBLE_LINE_SEP "'"
	   "\\D'l |\\n[TW]u 0'"
	   "\\v'" DOUBLE_LINE_SEP "'"
	   "\\h'|0'"
	   "\\D'l |\\n[TW]u 0'");
  else {
    int start_col = 0;
    for (;;) {
      while (start_col < ncolumns 
	     && entry[r][start_col] != 0
	     && entry[r][start_col]->start_row != r)
	start_col++;
      int end_col;
      for (end_col = start_col;
	   end_col < ncolumns
	   && (entry[r][end_col] == 0
	       || entry[r][end_col]->start_row == r);
	   end_col++)
	;
      if (end_col <= start_col)
	break;
      const char *left_adjust = 0;
      if ((r > 0 && vline[r-1][start_col] == 2)
	  || (r < nrows && vline[r][start_col] == 2))
	left_adjust = "-" HALF_DOUBLE_LINE_SEP;
      const char *right_adjust = 0;
      if ((r > 0 && vline[r-1][end_col] == 2)
	  || (r < nrows && vline[r][end_col] == 2))
	right_adjust = "+" HALF_DOUBLE_LINE_SEP;
      printfs("\\v'-" DOUBLE_LINE_SEP "'"
	      "\\h'|\\n[%1]u",
	      column_divide_reg(start_col));
      if (left_adjust)
	prints(left_adjust);
      prints("'");
      printfs("\\D'l |\\n[%1]u",
	      column_divide_reg(end_col));
      if (right_adjust)
	prints(right_adjust);
      prints(" 0'");
      printfs("\\v'" DOUBLE_LINE_SEP "'"
	      "\\h'|\\n[%1]u",
	      column_divide_reg(start_col));
      if (left_adjust)
	prints(left_adjust);
      prints("'");
      printfs("\\D'l |\\n[%1]u",
	      column_divide_reg(end_col));
      if (right_adjust)
	prints(right_adjust);
      prints(" 0'");
      start_col = end_col;
    }
  }
  prints("\\s0\n"
	 "." REPEATED_NM_SUS_MACRO " r\n"
	 ".ls\n"
	 ".vs\n");
}

void table::compute_vrule_top_adjust(int start_row, int col, string &result)
{
  if (row_is_all_lines[start_row] && start_row < nrows - 1) {
    if (row_is_all_lines[start_row] == 2)
      result = LINE_SEP ">?\\n[.V]u" "+" DOUBLE_LINE_SEP;
    else
      result = LINE_SEP ">?\\n[.V]u";
    start_row++;
  }
  else {
    result = "";
    if (start_row == 0)
      return;
    for (stuff *p = stuff_list; p && p->row <= start_row; p = p->next)
      if (p->row == start_row 
	  && (p->is_single_line() || p->is_double_line()))
	return;
  }
  int left = 0;
  if (col > 0) {
    table_entry *e = entry[start_row-1][col-1];
    if (e && e->start_row == e->end_row) {
      if (e->to_double_line_entry() != 0)
	left = 2;
      else if (e->to_single_line_entry() != 0)
	left = 1;
    }
  }
  int right = 0;
  if (col < ncolumns) {
    table_entry *e = entry[start_row-1][col];
    if (e && e->start_row == e->end_row) {
      if (e->to_double_line_entry() != 0)
	right = 2;
      else if (e->to_single_line_entry() != 0)
	right = 1;
    }
  }
  if (row_is_all_lines[start_row-1] == 0) {
    if (left > 0 || right > 0) {
      result += "-" BODY_DEPTH "-" BAR_HEIGHT;
      if ((left == 2 && right != 2) || (right == 2 && left != 2))
	result += "-" HALF_DOUBLE_LINE_SEP;
      else if (left == 2 && right == 2)
	result += "+" HALF_DOUBLE_LINE_SEP;
    }
  }
  else if (row_is_all_lines[start_row-1] == 2) {
    if ((left == 2 && right != 2) || (right == 2 && left != 2))
      result += "-" DOUBLE_LINE_SEP;
    else if (left == 1 || right == 1)
      result += "-" HALF_DOUBLE_LINE_SEP;
  }
}

void table::compute_vrule_bot_adjust(int end_row, int col, string &result)
{
  if (row_is_all_lines[end_row] && end_row > 0) {
    end_row--;
    result = "";
  }
  else {
    stuff *p;
    for (p = stuff_list; p && p->row < end_row + 1; p = p->next)
      ;
    if (p && p->row == end_row + 1 && p->is_double_line()) {
      result = "-" DOUBLE_LINE_SEP;
      return;
    }
    if ((p != 0 && p->row == end_row + 1)
	|| end_row == nrows - 1) {
      result = "";
      return;
    }
    if (row_is_all_lines[end_row+1] == 1)
      result = LINE_SEP;
    else if (row_is_all_lines[end_row+1] == 2)
      result = LINE_SEP "+" DOUBLE_LINE_SEP;
    else
      result = "";
  }
  int left = 0;
  if (col > 0) {
    table_entry *e = entry[end_row+1][col-1];
    if (e && e->start_row == e->end_row) {
      if (e->to_double_line_entry() != 0)
	left = 2;
      else if (e->to_single_line_entry() != 0)
	left = 1;
    }
  }
  int right = 0;
  if (col < ncolumns) {
    table_entry *e = entry[end_row+1][col];
    if (e && e->start_row == e->end_row) {
      if (e->to_double_line_entry() != 0)
	right = 2;
      else if (e->to_single_line_entry() != 0)
	right = 1;
    }
  }
  if (row_is_all_lines[end_row+1] == 0) {
    if (left > 0 || right > 0) {
      result = "1v-" BODY_DEPTH "-" BAR_HEIGHT;
      if ((left == 2 && right != 2) || (right == 2 && left != 2))
	result += "+" HALF_DOUBLE_LINE_SEP;
      else if (left == 2 && right == 2)
	result += "-" HALF_DOUBLE_LINE_SEP;
    }
  }
  else if (row_is_all_lines[end_row+1] == 2) {
    if (left == 2 && right == 2)
      result += "-" DOUBLE_LINE_SEP;
    else if (left != 2 && right != 2 && (left == 1 || right == 1))
      result += "-" HALF_DOUBLE_LINE_SEP;
  }
}

void table::add_vertical_rule(int start_row, int end_row,
			      int col, int is_double)
{
  vrule_list = new vertical_rule(start_row, end_row, col, is_double,
				 vrule_list);
  compute_vrule_top_adjust(start_row, col, vrule_list->top_adjust);
  compute_vrule_bot_adjust(end_row, col, vrule_list->bot_adjust);
}

void table::build_vrule_list()
{
  int col;
  if (flags & ALLBOX) {
    for (col = 1; col < ncolumns; col++) {
      int start_row = 0;
      for (;;) {
	while (start_row < nrows && vline_spanned(start_row, col))
	  start_row++;
	if (start_row >= nrows)
	  break;
	int end_row = start_row;
	while (end_row < nrows && !vline_spanned(end_row, col))
	  end_row++;
	end_row--;
	add_vertical_rule(start_row, end_row, col, 0);
	start_row = end_row + 1;
      }
    }
  }
  if (flags & (BOX | ALLBOX | DOUBLEBOX)) {
    add_vertical_rule(0, nrows - 1, 0, 0);
    add_vertical_rule(0, nrows - 1, ncolumns, 0);
  }
  for (int end_row = 0; end_row < nrows; end_row++)
    for (col = 0; col < ncolumns+1; col++)
      if (vline[end_row][col] > 0
	  && !vline_spanned(end_row, col)
	  && (end_row == nrows - 1 
	      || vline[end_row+1][col] != vline[end_row][col]
	      || vline_spanned(end_row+1, col))) {
	int start_row;
	for (start_row = end_row - 1;
	     start_row >= 0
	     && vline[start_row][col] == vline[end_row][col]
	     && !vline_spanned(start_row, col);
	     start_row--)
	  ;
	start_row++;
	add_vertical_rule(start_row, end_row, col, vline[end_row][col] > 1);
      }
  for (vertical_rule *p = vrule_list; p; p = p->next)
    if (p->is_double)
      for (int r = p->start_row; r <= p->end_row; r++) {
	if (p->col > 0 && entry[r][p->col-1] != 0
	    && entry[r][p->col-1]->end_col == p->col-1) {
	  int is_corner = r == p->start_row || r == p->end_row;
	  entry[r][p->col-1]->note_double_vrule_on_right(is_corner);
	}
	if (p->col < ncolumns && entry[r][p->col] != 0
	    && entry[r][p->col]->start_col == p->col) {
	  int is_corner = r == p->start_row || r == p->end_row;
	  entry[r][p->col]->note_double_vrule_on_left(is_corner);
	}
      }
}

void table::define_bottom_macro()
{
  prints(".eo\n"
	 // protect # in macro name against eqn
	 ".ig\n"
	 ".EQ\n"
	 "delim off\n"
	 ".EN\n"
	 "..\n"
	 ".de T#\n"
	 ".if !\\n[" SUPPRESS_BOTTOM_REG "] \\{"
	 "." REPEATED_VPT_MACRO " 0\n"
	 ".mk " SAVED_VERTICAL_POS_REG "\n");
  if (flags & (BOX | ALLBOX | DOUBLEBOX)) {
    prints(".if \\n[T.]&\\n[" NEED_BOTTOM_RULE_REG "] \\{");
    print_single_hline(0);
    prints(".\\}\n");
  }
  prints("." REPEATED_NM_SUS_MACRO " s\n"
	 ".ls 1\n");
  for (vertical_rule *p = vrule_list; p; p = p->next)
    p->contribute_to_bottom_macro(this);
  if (flags & DOUBLEBOX)
    prints(".if \\n[T.] \\{.vs " DOUBLE_LINE_SEP ">?\\n[.V]u\n"
	   "\\v'" BODY_DEPTH "'\\s[\\n[" LINESIZE_REG "]]"
	   "\\D'l \\n[TW]u 0'\\s0\n"
	   ".vs\n"
	   ".\\}\n"
	   ".if \\n[" LAST_PASSED_ROW_REG "]>=0 "
	   ".nr " TOP_REG " \\n[#T]-" DOUBLE_LINE_SEP "\n"
	   ".sp -1\n"
	   "\\v'" BODY_DEPTH "'\\s[\\n[" LINESIZE_REG "]]"
	   "\\D'l 0 |\\n[" TOP_REG "]u-1v'\\s0\n"
	   ".sp -1\n"
	   "\\v'" BODY_DEPTH "'\\h'|\\n[TW]u'\\s[\\n[" LINESIZE_REG "]]"
	   "\\D'l 0 |\\n[" TOP_REG "]u-1v'\\s0\n");
  prints("." REPEATED_NM_SUS_MACRO " r\n"
	 ".ls\n");
  prints(".nr " LAST_PASSED_ROW_REG " \\n[" CURRENT_ROW_REG "]\n"
	 ".sp |\\n[" SAVED_VERTICAL_POS_REG "]u\n"
	 "." REPEATED_VPT_MACRO " 1\n"
	 ".\\}\n"
	 "..\n"
	 ".ig\n"
	 ".EQ\n"
	 "delim on\n"
	 ".EN\n"
	 "..\n"
	 ".ec\n");
}

// is the vertical line before column c in row r horizontally spanned?

int table::vline_spanned(int r, int c)
{
  assert(r >= 0 && r < nrows && c >= 0 && c < ncolumns + 1);
  return (c != 0 && c != ncolumns && entry[r][c] != 0
	  && entry[r][c]->start_col != c
	  // horizontally spanning lines don't count
	  && entry[r][c]->to_double_line_entry() == 0
	  && entry[r][c]->to_single_line_entry() == 0);
}

int table::row_begins_section(int r)
{
  assert(r >= 0 && r < nrows);
  for (int i = 0; i < ncolumns; i++)
    if (entry[r][i] && entry[r][i]->start_row != r)
      return 0;
  return 1;
}

int table::row_ends_section(int r)
{
  assert(r >= 0 && r < nrows);
  for (int i = 0; i < ncolumns; i++)
    if (entry[r][i] && entry[r][i]->end_row != r)
      return 0;
  return 1;
}

void table::do_row(int r)
{
  if (!(flags & NOKEEP) && row_begins_section(r))
    prints("." KEEP_MACRO_NAME "\n");
  int had_line = 0;
  stuff *p;
  for (p = stuff_list; p && p->row < r; p = p->next)
    ;
  for (stuff *p1 = p; p1 && p1->row == r; p1 = p1->next)
    if (!p1->printed && (p1->is_single_line() || p1->is_double_line())) {
      had_line = 1;
      break;
    }
  if (!had_line && !row_is_all_lines[r])
    printfs("." REPEATED_MARK_MACRO " %1\n", row_top_reg(r));
  had_line = 0;
  for (; p && p->row == r; p = p->next)
    if (!p->printed) {
      p->print(this);
      if (!had_line && (p->is_single_line() || p->is_double_line())) {
	printfs("." REPEATED_MARK_MACRO " %1\n", row_top_reg(r));
	had_line = 1;
      }
    }
  // change the row *after* printing the stuff list (which might contain .TH)
  printfs("\\*[" TRANSPARENT_STRING_NAME "].nr " CURRENT_ROW_REG " %1\n",
	  as_string(r));
  if (!had_line && row_is_all_lines[r])
    printfs("." REPEATED_MARK_MACRO " %1\n", row_top_reg(r));
  // we might have had a .TH, for example,  since we last tried
  if (!(flags & NOKEEP) && row_begins_section(r))
    prints("." KEEP_MACRO_NAME "\n");
  prints("." REPEATED_NM_SET_MACRO " d\n"
	 ".nr " ROW_MAX_LINE_REG " \\n[ln]\n");
  printfs(".mk %1\n", row_start_reg(r));
  prints(".mk " BOTTOM_REG "\n"
	 "." REPEATED_VPT_MACRO " 0\n");
  int c;
  int row_is_blank = 1;
  int first_start_row = r;
  for (c = 0; c < ncolumns; c++) {
    table_entry *e = entry[r][c];
    if (e) {
      if (e->end_row == r) {
	e->do_depth();
	if (e->start_row < first_start_row)
	  first_start_row = e->start_row;
	row_is_blank = 0;
      }
      c = e->end_col;
    }
  }
  if (row_is_blank)
    prints(".nr " BOTTOM_REG " +1v\n");
  if (row_is_all_lines[r]) {
    prints(".vs " LINE_SEP);
    if (row_is_all_lines[r] == 2)
      prints("+" DOUBLE_LINE_SEP);
    prints(">?\\n[.V]u\n.ls 1\n");
    prints("\\&");
    prints("\\v'" BODY_DEPTH);
    if (row_is_all_lines[r] == 2)
      prints("-" HALF_DOUBLE_LINE_SEP);
    prints("'");
    for (c = 0; c < ncolumns; c++) {
      table_entry *e = entry[r][c];
      if (e) {
	if (e->end_row == e->start_row)
	  e->to_simple_entry()->simple_print(1);
	c = e->end_col;
      }
    }
    prints("\n");
    prints(".ls\n"
	   ".vs\n");
    prints(".nr " BOTTOM_REG " \\n[" BOTTOM_REG "]>?\\n[.d]\n");
    printfs(".sp |\\n[%1]u\n", row_start_reg(r));
  }
  for (int i = row_is_all_lines[r] ? r - 1 : r;
       i >= first_start_row;
       i--) {
    simple_entry *first = 0;
    for (c = 0; c < ncolumns; c++) {
      table_entry *e = entry[r][c];
      if (e) {
	if (e->end_row == r && e->start_row == i) {
	  simple_entry *simple = e->to_simple_entry();
	  if (simple) {
	    if (!first) {
	      prints(".ta");
	      first = simple;
	    }
	    simple->add_tab();
	  }
	}
	c = e->end_col;
      }
    }
    if (first) {
      prints('\n');
      first->position_vertically();
      first->set_location();
      prints("\\&");
      first->simple_print(0);
      for (c = first->end_col + 1; c < ncolumns; c++) {
	table_entry *e = entry[r][c];
	if (e) {
	  if (e->end_row == r && e->start_row == i) {
	    simple_entry *simple = e->to_simple_entry();
	    if (simple) {
	      if (e->end_row != e->start_row) {
		prints('\n');
		simple->position_vertically();
		prints("\\&");
	      }
	      simple->simple_print(0);
	    }
	  }
	  c = e->end_col;
	}
      }
      prints('\n');
      prints(".nr " BOTTOM_REG " \\n[" BOTTOM_REG "]>?\\n[.d]\n");
      printfs(".sp |\\n[%1]u\n", row_start_reg(r));
    }
  }
  for (c = 0; c < ncolumns; c++) {
    table_entry *e = entry[r][c];
    if (e) {
      if (e->end_row == r && e->to_simple_entry() == 0) {
	prints("." REPEATED_NM_SET_MACRO " s\n");
	e->position_vertically();
	e->print();
	prints(".nr " BOTTOM_REG " \\n[" BOTTOM_REG "]>?\\n[.d]\n");
	printfs(".sp |\\n[%1]u\n", row_start_reg(r));
      }
      c = e->end_col;
    }
  }
  prints("." REPEATED_NM_SET_MACRO " m\n"
	 "." REPEATED_VPT_MACRO " 1\n"
	 ".sp |\\n[" BOTTOM_REG "]u\n"
	 "\\*[" TRANSPARENT_STRING_NAME "].nr " NEED_BOTTOM_RULE_REG " 1\n");
  if (r != nrows - 1 && (flags & ALLBOX)) {
    print_single_hline(r + 1);
    prints("\\*[" TRANSPARENT_STRING_NAME "].nr " NEED_BOTTOM_RULE_REG " 0\n");
  }
  if (r != nrows - 1) {
    if (p && p->row == r + 1
	&& (p->is_single_line() || p->is_double_line())) {
      p->print(this);
      prints("\\*[" TRANSPARENT_STRING_NAME "].nr " NEED_BOTTOM_RULE_REG
	     " 0\n");
    }
    int printed_one = 0;
    for (vertical_rule *vr = vrule_list; vr; vr = vr->next)
      if (vr->end_row == r) {
	if (!printed_one) {
	  prints("." REPEATED_VPT_MACRO " 0\n");
	  printed_one = 1;
	}
	vr->print();
      }
    if (printed_one)
      prints("." REPEATED_VPT_MACRO " 1\n");
    if (!(flags & NOKEEP) && row_ends_section(r))
      prints("." RELEASE_MACRO_NAME "\n");
  }
  prints(".if \\n[ln] .nr ln \\n[" ROW_MAX_LINE_REG "]\n");
}

void table::do_top()
{
  prints(".fc \002\003\n");
  if (!(flags & NOKEEP) && (flags & (BOX | DOUBLEBOX | ALLBOX)))
    prints("." TABLE_KEEP_MACRO_NAME "\n");
  if (flags & DOUBLEBOX) {
    prints("." REPEATED_NM_SUS_MACRO " s\n"
	   ".ls 1\n"
	   ".vs " LINE_SEP ">?\\n[.V]u\n"
	   "\\v'" BODY_DEPTH "'\\s[\\n[" LINESIZE_REG "]]\\D'l \\n[TW]u 0'\\s0\n"
	   ".vs\n"
	   "." REPEATED_MARK_MACRO " " TOP_REG "\n"
	   ".vs " DOUBLE_LINE_SEP ">?\\n[.V]u\n");
    printfs("\\v'" BODY_DEPTH "'"
	    "\\s[\\n[" LINESIZE_REG "]]"
	    "\\h'\\n[%1]u'"
	    "\\D'l |\\n[%2]u 0'"
	    "\\s0"
	    "\n",
	    column_divide_reg(0),
	    column_divide_reg(ncolumns));
    prints("." REPEATED_NM_SUS_MACRO " r\n"
	   ".ls\n"
	   ".vs\n");
  }
  else if (flags & (ALLBOX | BOX)) {
    print_single_hline(0);
  }
  //printfs(".mk %1\n", row_top_reg(0));
}

void table::do_bottom()
{
  // print stuff after last row
  for (stuff *p = stuff_list; p; p = p->next)
    if (p->row > nrows - 1)
      p->print(this);
  if (!(flags & NOKEEP))
    prints("." RELEASE_MACRO_NAME "\n");
  printfs(".mk %1\n", row_top_reg(nrows));
  prints(".nr " NEED_BOTTOM_RULE_REG " 1\n"
	 ".nr T. 1\n"
	 // protect # in macro name against eqn
	 ".ig\n"
	 ".EQ\n"
	 "delim off\n"
	 ".EN\n"
	 "..\n"
	 ".T#\n"
	 ".ig\n"
	 ".EQ\n"
	 "delim on\n"
	 ".EN\n"
	 "..\n");
  if (!(flags & NOKEEP) && (flags & (BOX | DOUBLEBOX | ALLBOX)))
    prints("." TABLE_RELEASE_MACRO_NAME "\n");
  else
    prints(".if \\n[ln] \\{.nm\n"
	   ".nr ln \\n[" ROW_MAX_LINE_REG "]\n"
	   ".\\}\n");
  if (flags & DOUBLEBOX)
    prints(".sp " DOUBLE_LINE_SEP "\n");
  prints("." RESET_MACRO_NAME "\n"
	 ".fc\n"
	 ".cp \\n(" COMPATIBLE_REG "\n");
}

int table::get_nrows()
{
  return nrows;
}

const char *last_filename = 0;

void set_troff_location(const char *fn, int ln)
{
  if (!location_force_filename && last_filename != 0
      && strcmp(fn, last_filename) == 0)
    printfs(".lf %1\n", as_string(ln));
  else {
    string filename(fn);
    filename += '\0';
    normalize_for_lf(filename);
    printfs(".lf %1 %2\n", as_string(ln), filename.contents());
    last_filename = fn;
    location_force_filename = 0;
  }
}

void printfs(const char *s, const string &arg1, const string &arg2,
	     const string &arg3, const string &arg4, const string &arg5)
{
  if (s) {
    char c;
    while ((c = *s++) != '\0') {
      if (c == '%') {
	switch (*s++) {
	case '1':
	  prints(arg1);
	  break;
	case '2':
	  prints(arg2);
	  break;
	case '3':
	  prints(arg3);
	  break;
	case '4':
	  prints(arg4);
	  break;
	case '5':
	  prints(arg5);
	  break;
	case '6':
	case '7':
	case '8':
	case '9':
	  break;
	case '%':
	  prints('%');
	  break;
	default:
	  assert(0);
	}
      }
      else
	prints(c);
    }
  }
}  

