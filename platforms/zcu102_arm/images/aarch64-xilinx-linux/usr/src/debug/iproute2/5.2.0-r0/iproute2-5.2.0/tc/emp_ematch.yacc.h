/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

#ifndef YY_EMATCH_EMP_EMATCH_YACC_H_INCLUDED
# define YY_EMATCH_EMP_EMATCH_YACC_H_INCLUDED
/* Debug traces.  */
#ifndef EMATCH_DEBUG
# if defined YYDEBUG
#if YYDEBUG
#   define EMATCH_DEBUG 1
#  else
#   define EMATCH_DEBUG 0
#  endif
# else /* ! defined YYDEBUG */
#  define EMATCH_DEBUG 1
# endif /* ! defined YYDEBUG */
#endif  /* ! defined EMATCH_DEBUG */
#if EMATCH_DEBUG
extern int ematch_debug;
#endif

/* Token type.  */
#ifndef EMATCH_TOKENTYPE
# define EMATCH_TOKENTYPE
  enum ematch_tokentype
  {
    ERROR = 258,
    ATTRIBUTE = 259,
    AND = 260,
    OR = 261,
    NOT = 262
  };
#endif

/* Value type.  */
#if ! defined EMATCH_STYPE && ! defined EMATCH_STYPE_IS_DECLARED
union EMATCH_STYPE
{
#line 14 "emp_ematch.y"

	unsigned int i;
	struct bstr *b;
	struct ematch *e;

#line 79 "emp_ematch.yacc.h"

};
typedef union EMATCH_STYPE EMATCH_STYPE;
# define EMATCH_STYPE_IS_TRIVIAL 1
# define EMATCH_STYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined EMATCH_LTYPE && ! defined EMATCH_LTYPE_IS_DECLARED
typedef struct EMATCH_LTYPE EMATCH_LTYPE;
struct EMATCH_LTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define EMATCH_LTYPE_IS_DECLARED 1
# define EMATCH_LTYPE_IS_TRIVIAL 1
#endif


extern EMATCH_STYPE ematch_lval;
extern EMATCH_LTYPE ematch_lloc;
int ematch_parse (void);

#endif /* !YY_EMATCH_EMP_EMATCH_YACC_H_INCLUDED  */
