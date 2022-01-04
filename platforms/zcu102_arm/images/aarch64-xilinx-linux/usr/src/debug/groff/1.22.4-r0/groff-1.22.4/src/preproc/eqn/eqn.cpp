/* A Bison parser, made by GNU Bison 3.4.1.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 18 "src/preproc/eqn/eqn.ypp"

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lib.h"
#include "box.h"
extern int non_empty_flag;
int yylex();
void yyerror(const char *);

#line 86 "src/preproc/eqn/eqn.cpp"

# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
#ifndef YY_YY_SRC_PREPROC_EQN_EQN_HPP_INCLUDED
# define YY_YY_SRC_PREPROC_EQN_EQN_HPP_INCLUDED
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
    OVER = 258,
    SMALLOVER = 259,
    SQRT = 260,
    SUB = 261,
    SUP = 262,
    LPILE = 263,
    RPILE = 264,
    CPILE = 265,
    PILE = 266,
    LEFT = 267,
    RIGHT = 268,
    TO = 269,
    FROM = 270,
    SIZE = 271,
    FONT = 272,
    ROMAN = 273,
    BOLD = 274,
    ITALIC = 275,
    FAT = 276,
    ACCENT = 277,
    BAR = 278,
    UNDER = 279,
    ABOVE = 280,
    TEXT = 281,
    QUOTED_TEXT = 282,
    FWD = 283,
    BACK = 284,
    DOWN = 285,
    UP = 286,
    MATRIX = 287,
    COL = 288,
    LCOL = 289,
    RCOL = 290,
    CCOL = 291,
    MARK = 292,
    LINEUP = 293,
    TYPE = 294,
    VCENTER = 295,
    PRIME = 296,
    SPLIT = 297,
    NOSPLIT = 298,
    UACCENT = 299,
    SPECIAL = 300,
    SPACE = 301,
    GFONT = 302,
    GSIZE = 303,
    DEFINE = 304,
    NDEFINE = 305,
    TDEFINE = 306,
    SDEFINE = 307,
    UNDEF = 308,
    IFDEF = 309,
    INCLUDE = 310,
    DELIM = 311,
    CHARTYPE = 312,
    SET = 313,
    GRFONT = 314,
    GBFONT = 315
  };
#endif
/* Tokens.  */
#define OVER 258
#define SMALLOVER 259
#define SQRT 260
#define SUB 261
#define SUP 262
#define LPILE 263
#define RPILE 264
#define CPILE 265
#define PILE 266
#define LEFT 267
#define RIGHT 268
#define TO 269
#define FROM 270
#define SIZE 271
#define FONT 272
#define ROMAN 273
#define BOLD 274
#define ITALIC 275
#define FAT 276
#define ACCENT 277
#define BAR 278
#define UNDER 279
#define ABOVE 280
#define TEXT 281
#define QUOTED_TEXT 282
#define FWD 283
#define BACK 284
#define DOWN 285
#define UP 286
#define MATRIX 287
#define COL 288
#define LCOL 289
#define RCOL 290
#define CCOL 291
#define MARK 292
#define LINEUP 293
#define TYPE 294
#define VCENTER 295
#define PRIME 296
#define SPLIT 297
#define NOSPLIT 298
#define UACCENT 299
#define SPECIAL 300
#define SPACE 301
#define GFONT 302
#define GSIZE 303
#define DEFINE 304
#define NDEFINE 305
#define TDEFINE 306
#define SDEFINE 307
#define UNDEF 308
#define IFDEF 309
#define INCLUDE 310
#define DELIM 311
#define CHARTYPE 312
#define SET 313
#define GRFONT 314
#define GBFONT 315

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 34 "src/preproc/eqn/eqn.ypp"

	char *str;
	box *b;
	pile_box *pb;
	matrix_box *mb;
	int n;
	column *col;

#line 258 "src/preproc/eqn/eqn.cpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PREPROC_EQN_EQN_HPP_INCLUDED  */



#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  72
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   379

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  66
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  75
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  142

#define YYUNDEFTOK  2
#define YYMAXUTOK   315

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,    63,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    61,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    64,     2,    65,    62,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   125,   125,   127,   132,   134,   145,   147,   149,   154,
     156,   158,   160,   162,   167,   169,   171,   173,   178,   180,
     185,   187,   189,   194,   196,   198,   200,   202,   204,   206,
     208,   210,   212,   214,   216,   218,   220,   222,   224,   226,
     228,   230,   232,   234,   236,   238,   240,   242,   244,   246,
     248,   250,   252,   254,   256,   258,   263,   273,   275,   280,
     282,   287,   289,   294,   296,   301,   303,   308,   310,   312,
     314,   318,   320,   325,   327,   329
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "OVER", "SMALLOVER", "SQRT", "SUB",
  "SUP", "LPILE", "RPILE", "CPILE", "PILE", "LEFT", "RIGHT", "TO", "FROM",
  "SIZE", "FONT", "ROMAN", "BOLD", "ITALIC", "FAT", "ACCENT", "BAR",
  "UNDER", "ABOVE", "TEXT", "QUOTED_TEXT", "FWD", "BACK", "DOWN", "UP",
  "MATRIX", "COL", "LCOL", "RCOL", "CCOL", "MARK", "LINEUP", "TYPE",
  "VCENTER", "PRIME", "SPLIT", "NOSPLIT", "UACCENT", "SPECIAL", "SPACE",
  "GFONT", "GSIZE", "DEFINE", "NDEFINE", "TDEFINE", "SDEFINE", "UNDEF",
  "IFDEF", "INCLUDE", "DELIM", "CHARTYPE", "SET", "GRFONT", "GBFONT",
  "'^'", "'~'", "'\\t'", "'{'", "'}'", "$accept", "top", "equation",
  "mark", "from_to", "sqrt_over", "script", "nonsup", "simple", "number",
  "pile_element_list", "pile_arg", "column_list", "column_element_list",
  "column_arg", "column", "text", "delim", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,    94,   126,     9,   123,   125
};
# endif

#define YYPACT_NINF -76

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-76)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     230,   269,     6,     6,     6,     6,     2,    14,    14,   308,
     308,   308,   308,   -76,   -76,    14,    14,    14,    14,   -50,
     230,   230,    14,   308,     4,    23,    14,   -76,   -76,   -76,
     230,    24,   230,   -76,   -76,    70,   -76,   -76,    20,   -76,
     -76,   -76,   230,   -44,   -76,   -76,   -76,   -76,   -76,   -76,
     -76,   -76,   230,   308,   308,    57,    57,    57,    57,   308,
     308,   308,   308,     3,   -76,   -76,   308,    57,   -76,   -76,
     308,   130,   -76,   -76,   269,   269,   269,   269,   308,   308,
     308,   -76,   -76,   -76,   308,   230,   -12,   230,   191,    57,
      57,    57,    57,    57,    57,     8,     8,     8,     8,    12,
     -76,    57,    57,   -76,   -76,   -76,   -76,    79,   -76,   335,
     -76,   -76,   -76,   230,   -76,    -6,     2,   230,    28,   -76,
     -76,   -76,   -76,   -76,   -76,   269,   269,   308,   230,   -76,
     -76,   230,    -3,   230,   -76,   -76,   -76,   230,   -76,    -2,
     230,   -76
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,    24,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    27,    28,    29,
       0,     0,     3,     4,     6,     9,    14,    18,    20,    15,
      71,    72,     0,     0,    32,    56,    33,    34,    31,    74,
      75,    73,     0,     0,     0,    43,    44,    45,    46,     0,
       0,     0,     0,     0,     7,     8,     0,    54,    25,    26,
       0,     0,     1,     5,     0,     0,     0,     0,     0,     0,
       0,    38,    39,    40,     0,    57,     0,     0,    37,    48,
      47,    49,    50,    52,    51,     0,     0,     0,     0,     0,
      61,    53,    55,    30,    16,    17,    10,    11,    21,    20,
      19,    41,    42,     0,    59,     0,     0,     0,     0,    67,
      68,    69,    70,    35,    62,     0,     0,     0,    58,    60,
      36,    63,     0,     0,    12,    13,    22,     0,    65,     0,
      64,    66
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -76,   -76,     0,   -17,   -75,     1,   -67,   -13,    46,    -7,
       9,    13,   -76,   -47,    22,    -4,    -1,   -29
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    31,    85,    33,    34,    35,    36,    37,    38,    43,
      86,    44,    99,   132,   119,   100,    45,    52
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      32,   106,    39,    64,    65,    51,    53,    54,    59,    60,
      61,    62,   110,   113,    63,    73,    46,    47,    48,   113,
      87,    66,   137,   137,    72,    70,    78,    79,    40,    41,
      71,    68,    40,    41,    40,    41,    95,    96,    97,    98,
      40,    41,    80,    81,    82,    95,    96,    97,    98,    69,
     134,   135,    88,   114,    73,    55,    56,    57,    58,   129,
     136,    83,   138,   141,    84,   108,    49,    50,    73,    67,
      42,    73,   117,    74,    75,   104,   105,   123,   107,    80,
      81,    82,    74,    75,    76,    77,   139,   130,   118,   118,
     118,   118,   133,   125,   126,   124,   115,     0,    83,    89,
      90,    84,     0,     0,     0,    91,    92,    93,    94,     0,
       0,    73,   101,   128,    73,    51,   102,   131,   120,   121,
     122,     0,     0,    73,   109,     0,   111,     0,     0,     0,
     112,     0,     0,   131,     0,     1,     0,   140,     2,     3,
       4,     5,     6,     0,     0,     0,     7,     8,     9,    10,
      11,    12,     0,     0,     0,     0,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,     0,    24,    25,     0,    26,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    27,    28,    29,    30,   103,     1,     0,     0,     2,
       3,     4,     5,     6,   116,     0,     0,     7,     8,     9,
      10,    11,    12,     0,     0,     0,     0,    13,    14,    15,
      16,    17,    18,    19,     0,     0,     0,     0,    20,    21,
      22,    23,     0,    24,    25,     1,    26,     0,     2,     3,
       4,     5,     6,     0,     0,     0,     7,     8,     9,    10,
      11,    12,    27,    28,    29,    30,    13,    14,    15,    16,
      17,    18,    19,     0,     0,     0,     0,    20,    21,    22,
      23,     0,    24,    25,     1,    26,     0,     2,     3,     4,
       5,     6,     0,     0,     0,     7,     8,     9,    10,    11,
      12,    27,    28,    29,    30,    13,    14,    15,    16,    17,
      18,    19,     0,     0,     0,     0,     0,     0,    22,    23,
       0,    24,    25,     0,    26,     0,     2,     3,     4,     5,
       6,     0,     0,     0,     7,     8,     9,    10,    11,    12,
      27,    28,    29,    30,    13,    14,    15,    16,    17,    18,
      19,    78,   127,     0,     0,     0,     0,    22,    23,     0,
      24,    25,     0,    26,     0,     0,     0,    80,    81,    82,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    27,
      28,    29,    30,     0,     0,     0,    83,     0,     0,    84
};

static const yytype_int16 yycheck[] =
{
       0,    76,     1,    20,    21,     6,     7,     8,    15,    16,
      17,    18,    79,    25,    64,    32,     3,     4,     5,    25,
      64,    22,    25,    25,     0,    26,     6,     7,    26,    27,
      30,    27,    26,    27,    26,    27,    33,    34,    35,    36,
      26,    27,    22,    23,    24,    33,    34,    35,    36,    26,
     125,   126,    52,    65,    71,     9,    10,    11,    12,    65,
     127,    41,    65,    65,    44,    78,    64,    65,    85,    23,
      64,    88,    64,     3,     4,    74,    75,    65,    77,    22,
      23,    24,     3,     4,    14,    15,   133,   116,    95,    96,
      97,    98,    64,    14,    15,    99,    87,    -1,    41,    53,
      54,    44,    -1,    -1,    -1,    59,    60,    61,    62,    -1,
      -1,   128,    66,   113,   131,   116,    70,   117,    96,    97,
      98,    -1,    -1,   140,    78,    -1,    80,    -1,    -1,    -1,
      84,    -1,    -1,   133,    -1,     5,    -1,   137,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    21,    -1,    -1,    -1,    -1,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    37,    38,    39,
      40,    -1,    42,    43,    -1,    45,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    61,    62,    63,    64,    65,     5,    -1,    -1,     8,
       9,    10,    11,    12,    13,    -1,    -1,    16,    17,    18,
      19,    20,    21,    -1,    -1,    -1,    -1,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    37,    38,
      39,    40,    -1,    42,    43,     5,    45,    -1,     8,     9,
      10,    11,    12,    -1,    -1,    -1,    16,    17,    18,    19,
      20,    21,    61,    62,    63,    64,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    37,    38,    39,
      40,    -1,    42,    43,     5,    45,    -1,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    16,    17,    18,    19,    20,
      21,    61,    62,    63,    64,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    39,    40,
      -1,    42,    43,    -1,    45,    -1,     8,     9,    10,    11,
      12,    -1,    -1,    -1,    16,    17,    18,    19,    20,    21,
      61,    62,    63,    64,    26,    27,    28,    29,    30,    31,
      32,     6,     7,    -1,    -1,    -1,    -1,    39,    40,    -1,
      42,    43,    -1,    45,    -1,    -1,    -1,    22,    23,    24,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    61,
      62,    63,    64,    -1,    -1,    -1,    41,    -1,    -1,    44
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     8,     9,    10,    11,    12,    16,    17,    18,
      19,    20,    21,    26,    27,    28,    29,    30,    31,    32,
      37,    38,    39,    40,    42,    43,    45,    61,    62,    63,
      64,    67,    68,    69,    70,    71,    72,    73,    74,    71,
      26,    27,    64,    75,    77,    82,    77,    77,    77,    64,
      65,    82,    83,    82,    82,    74,    74,    74,    74,    75,
      75,    75,    75,    64,    69,    69,    82,    74,    27,    26,
      82,    68,     0,    69,     3,     4,    14,    15,     6,     7,
      22,    23,    24,    41,    44,    68,    76,    64,    68,    74,
      74,    74,    74,    74,    74,    33,    34,    35,    36,    78,
      81,    74,    74,    65,    71,    71,    70,    71,    73,    74,
      72,    74,    74,    25,    65,    76,    13,    64,    75,    80,
      80,    80,    80,    65,    81,    14,    15,     7,    68,    65,
      83,    68,    79,    64,    70,    70,    72,    25,    65,    79,
      68,    65
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    66,    67,    67,    68,    68,    69,    69,    69,    70,
      70,    70,    70,    70,    71,    71,    71,    71,    72,    72,
      73,    73,    73,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    74,    74,    74,    74,
      74,    74,    74,    74,    74,    74,    75,    76,    76,    77,
      77,    78,    78,    79,    79,    80,    80,    81,    81,    81,
      81,    82,    82,    83,    83,    83
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     2,     1,     2,     2,     1,
       3,     3,     5,     5,     1,     2,     3,     3,     1,     3,
       1,     3,     5,     1,     1,     2,     2,     1,     1,     1,
       3,     2,     2,     2,     2,     4,     5,     3,     2,     2,
       2,     3,     3,     2,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     2,     3,     1,     1,     3,     3,
       4,     1,     2,     1,     3,     3,     4,     2,     2,     2,
       2,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 3:
#line 128 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].b)->top_level(); non_empty_flag = 1; }
#line 1500 "src/preproc/eqn/eqn.cpp"
    break;

  case 4:
#line 133 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[0].b); }
#line 1506 "src/preproc/eqn/eqn.cpp"
    break;

  case 5:
#line 135 "src/preproc/eqn/eqn.ypp"
    {
		  list_box *lb = (yyvsp[-1].b)->to_list_box();
		  if (!lb)
		    lb = new list_box((yyvsp[-1].b));
		  lb->append((yyvsp[0].b));
		  (yyval.b) = lb;
		}
#line 1518 "src/preproc/eqn/eqn.cpp"
    break;

  case 6:
#line 146 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[0].b); }
#line 1524 "src/preproc/eqn/eqn.cpp"
    break;

  case 7:
#line 148 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_mark_box((yyvsp[0].b)); }
#line 1530 "src/preproc/eqn/eqn.cpp"
    break;

  case 8:
#line 150 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_lineup_box((yyvsp[0].b)); }
#line 1536 "src/preproc/eqn/eqn.cpp"
    break;

  case 9:
#line 155 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[0].b); }
#line 1542 "src/preproc/eqn/eqn.cpp"
    break;

  case 10:
#line 157 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_limit_box((yyvsp[-2].b), 0, (yyvsp[0].b)); }
#line 1548 "src/preproc/eqn/eqn.cpp"
    break;

  case 11:
#line 159 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_limit_box((yyvsp[-2].b), (yyvsp[0].b), 0); }
#line 1554 "src/preproc/eqn/eqn.cpp"
    break;

  case 12:
#line 161 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_limit_box((yyvsp[-4].b), (yyvsp[-2].b), (yyvsp[0].b)); }
#line 1560 "src/preproc/eqn/eqn.cpp"
    break;

  case 13:
#line 163 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_limit_box((yyvsp[-4].b), make_limit_box((yyvsp[-2].b), (yyvsp[0].b), 0), 0); }
#line 1566 "src/preproc/eqn/eqn.cpp"
    break;

  case 14:
#line 168 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[0].b); }
#line 1572 "src/preproc/eqn/eqn.cpp"
    break;

  case 15:
#line 170 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_sqrt_box((yyvsp[0].b)); }
#line 1578 "src/preproc/eqn/eqn.cpp"
    break;

  case 16:
#line 172 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_over_box((yyvsp[-2].b), (yyvsp[0].b)); }
#line 1584 "src/preproc/eqn/eqn.cpp"
    break;

  case 17:
#line 174 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_small_over_box((yyvsp[-2].b), (yyvsp[0].b)); }
#line 1590 "src/preproc/eqn/eqn.cpp"
    break;

  case 18:
#line 179 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[0].b); }
#line 1596 "src/preproc/eqn/eqn.cpp"
    break;

  case 19:
#line 181 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_script_box((yyvsp[-2].b), 0, (yyvsp[0].b)); }
#line 1602 "src/preproc/eqn/eqn.cpp"
    break;

  case 20:
#line 186 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[0].b); }
#line 1608 "src/preproc/eqn/eqn.cpp"
    break;

  case 21:
#line 188 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_script_box((yyvsp[-2].b), (yyvsp[0].b), 0); }
#line 1614 "src/preproc/eqn/eqn.cpp"
    break;

  case 22:
#line 190 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_script_box((yyvsp[-4].b), (yyvsp[-2].b), (yyvsp[0].b)); }
#line 1620 "src/preproc/eqn/eqn.cpp"
    break;

  case 23:
#line 195 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = split_text((yyvsp[0].str)); }
#line 1626 "src/preproc/eqn/eqn.cpp"
    break;

  case 24:
#line 197 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new quoted_text_box((yyvsp[0].str)); }
#line 1632 "src/preproc/eqn/eqn.cpp"
    break;

  case 25:
#line 199 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = split_text((yyvsp[0].str)); }
#line 1638 "src/preproc/eqn/eqn.cpp"
    break;

  case 26:
#line 201 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new quoted_text_box((yyvsp[0].str)); }
#line 1644 "src/preproc/eqn/eqn.cpp"
    break;

  case 27:
#line 203 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new half_space_box; }
#line 1650 "src/preproc/eqn/eqn.cpp"
    break;

  case 28:
#line 205 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new space_box; }
#line 1656 "src/preproc/eqn/eqn.cpp"
    break;

  case 29:
#line 207 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new tab_box; }
#line 1662 "src/preproc/eqn/eqn.cpp"
    break;

  case 30:
#line 209 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[-1].b); }
#line 1668 "src/preproc/eqn/eqn.cpp"
    break;

  case 31:
#line 211 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].pb)->set_alignment(CENTER_ALIGN); (yyval.b) = (yyvsp[0].pb); }
#line 1674 "src/preproc/eqn/eqn.cpp"
    break;

  case 32:
#line 213 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].pb)->set_alignment(LEFT_ALIGN); (yyval.b) = (yyvsp[0].pb); }
#line 1680 "src/preproc/eqn/eqn.cpp"
    break;

  case 33:
#line 215 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].pb)->set_alignment(RIGHT_ALIGN); (yyval.b) = (yyvsp[0].pb); }
#line 1686 "src/preproc/eqn/eqn.cpp"
    break;

  case 34:
#line 217 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].pb)->set_alignment(CENTER_ALIGN); (yyval.b) = (yyvsp[0].pb); }
#line 1692 "src/preproc/eqn/eqn.cpp"
    break;

  case 35:
#line 219 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = (yyvsp[-1].mb); }
#line 1698 "src/preproc/eqn/eqn.cpp"
    break;

  case 36:
#line 221 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_delim_box((yyvsp[-3].str), (yyvsp[-2].b), (yyvsp[0].str)); }
#line 1704 "src/preproc/eqn/eqn.cpp"
    break;

  case 37:
#line 223 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_delim_box((yyvsp[-1].str), (yyvsp[0].b), 0); }
#line 1710 "src/preproc/eqn/eqn.cpp"
    break;

  case 38:
#line 225 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_overline_box((yyvsp[-1].b)); }
#line 1716 "src/preproc/eqn/eqn.cpp"
    break;

  case 39:
#line 227 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_underline_box((yyvsp[-1].b)); }
#line 1722 "src/preproc/eqn/eqn.cpp"
    break;

  case 40:
#line 229 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_prime_box((yyvsp[-1].b)); }
#line 1728 "src/preproc/eqn/eqn.cpp"
    break;

  case 41:
#line 231 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_accent_box((yyvsp[-2].b), (yyvsp[0].b)); }
#line 1734 "src/preproc/eqn/eqn.cpp"
    break;

  case 42:
#line 233 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_uaccent_box((yyvsp[-2].b), (yyvsp[0].b)); }
#line 1740 "src/preproc/eqn/eqn.cpp"
    break;

  case 43:
#line 235 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new font_box(strsave(get_grfont()), (yyvsp[0].b)); }
#line 1746 "src/preproc/eqn/eqn.cpp"
    break;

  case 44:
#line 237 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new font_box(strsave(get_gbfont()), (yyvsp[0].b)); }
#line 1752 "src/preproc/eqn/eqn.cpp"
    break;

  case 45:
#line 239 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new font_box(strsave(get_gfont()), (yyvsp[0].b)); }
#line 1758 "src/preproc/eqn/eqn.cpp"
    break;

  case 46:
#line 241 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new fat_box((yyvsp[0].b)); }
#line 1764 "src/preproc/eqn/eqn.cpp"
    break;

  case 47:
#line 243 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new font_box((yyvsp[-1].str), (yyvsp[0].b)); }
#line 1770 "src/preproc/eqn/eqn.cpp"
    break;

  case 48:
#line 245 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new size_box((yyvsp[-1].str), (yyvsp[0].b)); }
#line 1776 "src/preproc/eqn/eqn.cpp"
    break;

  case 49:
#line 247 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new hmotion_box((yyvsp[-1].n), (yyvsp[0].b)); }
#line 1782 "src/preproc/eqn/eqn.cpp"
    break;

  case 50:
#line 249 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new hmotion_box(-(yyvsp[-1].n), (yyvsp[0].b)); }
#line 1788 "src/preproc/eqn/eqn.cpp"
    break;

  case 51:
#line 251 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new vmotion_box((yyvsp[-1].n), (yyvsp[0].b)); }
#line 1794 "src/preproc/eqn/eqn.cpp"
    break;

  case 52:
#line 253 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new vmotion_box(-(yyvsp[-1].n), (yyvsp[0].b)); }
#line 1800 "src/preproc/eqn/eqn.cpp"
    break;

  case 53:
#line 255 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].b)->set_spacing_type((yyvsp[-1].str)); (yyval.b) = (yyvsp[0].b); }
#line 1806 "src/preproc/eqn/eqn.cpp"
    break;

  case 54:
#line 257 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = new vcenter_box((yyvsp[0].b)); }
#line 1812 "src/preproc/eqn/eqn.cpp"
    break;

  case 55:
#line 259 "src/preproc/eqn/eqn.ypp"
    { (yyval.b) = make_special_box((yyvsp[-1].str), (yyvsp[0].b)); }
#line 1818 "src/preproc/eqn/eqn.cpp"
    break;

  case 56:
#line 264 "src/preproc/eqn/eqn.ypp"
    {
		  int n;
		  if (sscanf((yyvsp[0].str), "%d", &n) == 1)
		    (yyval.n) = n;
		  a_delete (yyvsp[0].str);
		}
#line 1829 "src/preproc/eqn/eqn.cpp"
    break;

  case 57:
#line 274 "src/preproc/eqn/eqn.ypp"
    { (yyval.pb) = new pile_box((yyvsp[0].b)); }
#line 1835 "src/preproc/eqn/eqn.cpp"
    break;

  case 58:
#line 276 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[-2].pb)->append((yyvsp[0].b)); (yyval.pb) = (yyvsp[-2].pb); }
#line 1841 "src/preproc/eqn/eqn.cpp"
    break;

  case 59:
#line 281 "src/preproc/eqn/eqn.ypp"
    { (yyval.pb) = (yyvsp[-1].pb); }
#line 1847 "src/preproc/eqn/eqn.cpp"
    break;

  case 60:
#line 283 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[-1].pb)->set_space((yyvsp[-3].n)); (yyval.pb) = (yyvsp[-1].pb); }
#line 1853 "src/preproc/eqn/eqn.cpp"
    break;

  case 61:
#line 288 "src/preproc/eqn/eqn.ypp"
    { (yyval.mb) = new matrix_box((yyvsp[0].col)); }
#line 1859 "src/preproc/eqn/eqn.cpp"
    break;

  case 62:
#line 290 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[-1].mb)->append((yyvsp[0].col)); (yyval.mb) = (yyvsp[-1].mb); }
#line 1865 "src/preproc/eqn/eqn.cpp"
    break;

  case 63:
#line 295 "src/preproc/eqn/eqn.ypp"
    { (yyval.col) = new column((yyvsp[0].b)); }
#line 1871 "src/preproc/eqn/eqn.cpp"
    break;

  case 64:
#line 297 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[-2].col)->append((yyvsp[0].b)); (yyval.col) = (yyvsp[-2].col); }
#line 1877 "src/preproc/eqn/eqn.cpp"
    break;

  case 65:
#line 302 "src/preproc/eqn/eqn.ypp"
    { (yyval.col) = (yyvsp[-1].col); }
#line 1883 "src/preproc/eqn/eqn.cpp"
    break;

  case 66:
#line 304 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[-1].col)->set_space((yyvsp[-3].n)); (yyval.col) = (yyvsp[-1].col); }
#line 1889 "src/preproc/eqn/eqn.cpp"
    break;

  case 67:
#line 309 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].col)->set_alignment(CENTER_ALIGN); (yyval.col) = (yyvsp[0].col); }
#line 1895 "src/preproc/eqn/eqn.cpp"
    break;

  case 68:
#line 311 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].col)->set_alignment(LEFT_ALIGN); (yyval.col) = (yyvsp[0].col); }
#line 1901 "src/preproc/eqn/eqn.cpp"
    break;

  case 69:
#line 313 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].col)->set_alignment(RIGHT_ALIGN); (yyval.col) = (yyvsp[0].col); }
#line 1907 "src/preproc/eqn/eqn.cpp"
    break;

  case 70:
#line 315 "src/preproc/eqn/eqn.ypp"
    { (yyvsp[0].col)->set_alignment(CENTER_ALIGN); (yyval.col) = (yyvsp[0].col); }
#line 1913 "src/preproc/eqn/eqn.cpp"
    break;

  case 71:
#line 319 "src/preproc/eqn/eqn.ypp"
    { (yyval.str) = (yyvsp[0].str); }
#line 1919 "src/preproc/eqn/eqn.cpp"
    break;

  case 72:
#line 321 "src/preproc/eqn/eqn.ypp"
    { (yyval.str) = (yyvsp[0].str); }
#line 1925 "src/preproc/eqn/eqn.cpp"
    break;

  case 73:
#line 326 "src/preproc/eqn/eqn.ypp"
    { (yyval.str) = (yyvsp[0].str); }
#line 1931 "src/preproc/eqn/eqn.cpp"
    break;

  case 74:
#line 328 "src/preproc/eqn/eqn.ypp"
    { (yyval.str) = strsave("{"); }
#line 1937 "src/preproc/eqn/eqn.cpp"
    break;

  case 75:
#line 330 "src/preproc/eqn/eqn.ypp"
    { (yyval.str) = strsave("}"); }
#line 1943 "src/preproc/eqn/eqn.cpp"
    break;


#line 1947 "src/preproc/eqn/eqn.cpp"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 333 "src/preproc/eqn/eqn.ypp"

