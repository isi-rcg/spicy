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

#ifndef YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED
# define YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    BASIC_TYPE = 258,
    IDENTIFIER = 259,
    TYPEDEF_NAME = 260,
    INTEGER = 261,
    FLOATING = 262,
    BOOLEAN = 263,
    CHARACTER = 264,
    STRING = 265,
    INTL_CONST = 266,
    INTUL_CONST = 267,
    ELLIPSIS = 268,
    ADDEQ = 269,
    SUBEQ = 270,
    MULEQ = 271,
    DIVEQ = 272,
    MODEQ = 273,
    XOREQ = 274,
    ANDEQ = 275,
    OREQ = 276,
    SL = 277,
    SR = 278,
    SLEQ = 279,
    SREQ = 280,
    EQ = 281,
    NOTEQ = 282,
    LTEQ = 283,
    GTEQ = 284,
    ANDAND = 285,
    OROR = 286,
    PLUSPLUS = 287,
    MINUSMINUS = 288,
    ARROW = 289,
    AUTO = 290,
    BREAK = 291,
    CASE = 292,
    CONST = 293,
    CONTINUE = 294,
    DEFAULT = 295,
    DO = 296,
    ELSE = 297,
    ENUM = 298,
    EXTENSION = 299,
    EXTERN = 300,
    FOR = 301,
    GOTO = 302,
    IF = 303,
    INLINE = 304,
    REGISTER = 305,
    RESTRICT = 306,
    RETURN = 307,
    SHORT = 308,
    SIGNED = 309,
    SIZEOF = 310,
    STATIC = 311,
    STRUCT = 312,
    SWITCH = 313,
    THREAD_LOCAL = 314,
    TYPEDEF = 315,
    UNION = 316,
    UNSIGNED = 317,
    VOID = 318,
    VOLATILE = 319,
    WHILE = 320,
    FUNCTION_MACRO = 321,
    OBJECT_MACRO = 322,
    IFDEF_GI_SCANNER = 323,
    IFNDEF_GI_SCANNER = 324,
    IFDEF_COND = 325,
    IFNDEF_COND = 326,
    IF_COND = 327,
    ELIF_COND = 328,
    ELSE_COND = 329,
    ENDIF_COND = 330
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 250 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"

  char *str;
  GList *list;
  GISourceSymbol *symbol;
  GISourceType *ctype;
  StorageClassSpecifier storage_class_specifier;
  TypeQualifier type_qualifier;
  FunctionSpecifier function_specifier;
  UnaryOperator unary_operator;

#line 144 "giscanner/scannerparser.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (GISourceScanner* scanner);

#endif /* !YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED  */
