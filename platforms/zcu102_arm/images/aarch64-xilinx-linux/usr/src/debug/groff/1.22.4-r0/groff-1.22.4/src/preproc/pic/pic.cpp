/* A Bison parser, made by GNU Bison 3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 19 "../src/preproc/pic/pic.ypp" /* yacc.c:338  */

#include "pic.h"
#include "ptable.h"
#include "object.h"

extern int delim_flag;
extern void copy_rest_thru(const char *, const char *);
extern void copy_file_thru(const char *, const char *, const char *);
extern void push_body(const char *);
extern void do_for(char *var, double from, double to,
		   int by_is_multiplicative, double by, char *body);
extern void do_lookahead();

/* Maximum number of characters produced by printf("%g") */
#define GDIGITS 14

int yylex();
void yyerror(const char *);

void reset(const char *nm);
void reset_all();

place *lookup_label(const char *);
void define_label(const char *label, const place *pl);

direction current_direction;
position current_position;

implement_ptable(place)

PTABLE(place) top_table;

PTABLE(place) *current_table = &top_table;
saved_state *current_saved_state = 0;

object_list olist;

const char *ordinal_postfix(int n);
const char *object_type_name(object_type type);
char *format_number(const char *form, double n);
char *do_sprintf(const char *form, const double *v, int nv);


#line 113 "src/preproc/pic/pic.cpp" /* yacc.c:338  */
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

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
#ifndef YY_YY_SRC_PREPROC_PIC_PIC_HPP_INCLUDED
# define YY_YY_SRC_PREPROC_PIC_PIC_HPP_INCLUDED
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
    LABEL = 258,
    VARIABLE = 259,
    NUMBER = 260,
    TEXT = 261,
    COMMAND_LINE = 262,
    DELIMITED = 263,
    ORDINAL = 264,
    TH = 265,
    LEFT_ARROW_HEAD = 266,
    RIGHT_ARROW_HEAD = 267,
    DOUBLE_ARROW_HEAD = 268,
    LAST = 269,
    BOX = 270,
    CIRCLE = 271,
    ELLIPSE = 272,
    ARC = 273,
    LINE = 274,
    ARROW = 275,
    MOVE = 276,
    SPLINE = 277,
    HEIGHT = 278,
    RADIUS = 279,
    FIGNAME = 280,
    WIDTH = 281,
    DIAMETER = 282,
    UP = 283,
    DOWN = 284,
    RIGHT = 285,
    LEFT = 286,
    FROM = 287,
    TO = 288,
    AT = 289,
    WITH = 290,
    BY = 291,
    THEN = 292,
    SOLID = 293,
    DOTTED = 294,
    DASHED = 295,
    CHOP = 296,
    SAME = 297,
    INVISIBLE = 298,
    LJUST = 299,
    RJUST = 300,
    ABOVE = 301,
    BELOW = 302,
    OF = 303,
    THE = 304,
    WAY = 305,
    BETWEEN = 306,
    AND = 307,
    HERE = 308,
    DOT_N = 309,
    DOT_E = 310,
    DOT_W = 311,
    DOT_S = 312,
    DOT_NE = 313,
    DOT_SE = 314,
    DOT_NW = 315,
    DOT_SW = 316,
    DOT_C = 317,
    DOT_START = 318,
    DOT_END = 319,
    DOT_X = 320,
    DOT_Y = 321,
    DOT_HT = 322,
    DOT_WID = 323,
    DOT_RAD = 324,
    SIN = 325,
    COS = 326,
    ATAN2 = 327,
    LOG = 328,
    EXP = 329,
    SQRT = 330,
    K_MAX = 331,
    K_MIN = 332,
    INT = 333,
    RAND = 334,
    SRAND = 335,
    COPY = 336,
    THRU = 337,
    TOP = 338,
    BOTTOM = 339,
    UPPER = 340,
    LOWER = 341,
    SH = 342,
    PRINT = 343,
    CW = 344,
    CCW = 345,
    FOR = 346,
    DO = 347,
    IF = 348,
    ELSE = 349,
    ANDAND = 350,
    OROR = 351,
    NOTEQUAL = 352,
    EQUALEQUAL = 353,
    LESSEQUAL = 354,
    GREATEREQUAL = 355,
    LEFT_CORNER = 356,
    RIGHT_CORNER = 357,
    NORTH = 358,
    SOUTH = 359,
    EAST = 360,
    WEST = 361,
    CENTER = 362,
    END = 363,
    START = 364,
    RESET = 365,
    UNTIL = 366,
    PLOT = 367,
    THICKNESS = 368,
    FILL = 369,
    COLORED = 370,
    OUTLINED = 371,
    SHADED = 372,
    XSLANTED = 373,
    YSLANTED = 374,
    ALIGNED = 375,
    SPRINTF = 376,
    COMMAND = 377,
    DEFINE = 378,
    UNDEF = 379
  };
#endif
/* Tokens.  */
#define LABEL 258
#define VARIABLE 259
#define NUMBER 260
#define TEXT 261
#define COMMAND_LINE 262
#define DELIMITED 263
#define ORDINAL 264
#define TH 265
#define LEFT_ARROW_HEAD 266
#define RIGHT_ARROW_HEAD 267
#define DOUBLE_ARROW_HEAD 268
#define LAST 269
#define BOX 270
#define CIRCLE 271
#define ELLIPSE 272
#define ARC 273
#define LINE 274
#define ARROW 275
#define MOVE 276
#define SPLINE 277
#define HEIGHT 278
#define RADIUS 279
#define FIGNAME 280
#define WIDTH 281
#define DIAMETER 282
#define UP 283
#define DOWN 284
#define RIGHT 285
#define LEFT 286
#define FROM 287
#define TO 288
#define AT 289
#define WITH 290
#define BY 291
#define THEN 292
#define SOLID 293
#define DOTTED 294
#define DASHED 295
#define CHOP 296
#define SAME 297
#define INVISIBLE 298
#define LJUST 299
#define RJUST 300
#define ABOVE 301
#define BELOW 302
#define OF 303
#define THE 304
#define WAY 305
#define BETWEEN 306
#define AND 307
#define HERE 308
#define DOT_N 309
#define DOT_E 310
#define DOT_W 311
#define DOT_S 312
#define DOT_NE 313
#define DOT_SE 314
#define DOT_NW 315
#define DOT_SW 316
#define DOT_C 317
#define DOT_START 318
#define DOT_END 319
#define DOT_X 320
#define DOT_Y 321
#define DOT_HT 322
#define DOT_WID 323
#define DOT_RAD 324
#define SIN 325
#define COS 326
#define ATAN2 327
#define LOG 328
#define EXP 329
#define SQRT 330
#define K_MAX 331
#define K_MIN 332
#define INT 333
#define RAND 334
#define SRAND 335
#define COPY 336
#define THRU 337
#define TOP 338
#define BOTTOM 339
#define UPPER 340
#define LOWER 341
#define SH 342
#define PRINT 343
#define CW 344
#define CCW 345
#define FOR 346
#define DO 347
#define IF 348
#define ELSE 349
#define ANDAND 350
#define OROR 351
#define NOTEQUAL 352
#define EQUALEQUAL 353
#define LESSEQUAL 354
#define GREATEREQUAL 355
#define LEFT_CORNER 356
#define RIGHT_CORNER 357
#define NORTH 358
#define SOUTH 359
#define EAST 360
#define WEST 361
#define CENTER 362
#define END 363
#define START 364
#define RESET 365
#define UNTIL 366
#define PLOT 367
#define THICKNESS 368
#define FILL 369
#define COLORED 370
#define OUTLINED 371
#define SHADED 372
#define XSLANTED 373
#define YSLANTED 374
#define ALIGNED 375
#define SPRINTF 376
#define COMMAND 377
#define DEFINE 378
#define UNDEF 379

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 65 "../src/preproc/pic/pic.ypp" /* yacc.c:353  */

	char *str;
	int n;
	double x;
	struct { double x, y; } pair;
	struct { double x; char *body; } if_data;
	struct { char *str; const char *filename; int lineno; } lstr;
	struct { double *v; int nv; int maxv; } dv;
	struct { double val; int is_multiplicative; } by;
	place pl;
	object *obj;
	corner crn;
	path *pth;
	object_spec *spec;
	saved_state *pstate;
	graphics_state state;
	object_type obtype;

#line 423 "src/preproc/pic/pic.cpp" /* yacc.c:353  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PREPROC_PIC_PIC_HPP_INCLUDED  */



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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2438

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  146
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  260
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  454

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   379

#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   137,     2,     2,     2,   136,     2,     2,
     126,   145,   134,   132,   129,   133,   125,   135,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   141,   139,
     130,   140,   131,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   128,     2,   144,   138,     2,   127,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   142,     2,   143,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   275,   275,   276,   285,   290,   292,   296,   298,   302,
     303,   307,   315,   320,   332,   334,   336,   338,   340,   345,
     350,   357,   356,   367,   375,   377,   374,   388,   390,   387,
     400,   399,   408,   417,   416,   430,   431,   436,   437,   441,
     446,   451,   459,   461,   480,   487,   489,   500,   499,   511,
     512,   517,   519,   524,   530,   536,   538,   540,   542,   544,
     546,   548,   555,   559,   564,   572,   586,   592,   600,   607,
     613,   606,   622,   632,   633,   638,   640,   642,   644,   649,
     656,   663,   670,   677,   682,   687,   695,   694,   721,   727,
     733,   739,   745,   764,   771,   778,   785,   792,   799,   806,
     813,   820,   827,   842,   854,   860,   869,   876,   901,   905,
     911,   917,   923,   929,   934,   940,   946,   952,   959,   968,
     975,   991,  1008,  1013,  1018,  1023,  1028,  1033,  1038,  1043,
    1051,  1061,  1071,  1081,  1091,  1097,  1105,  1107,  1119,  1124,
    1154,  1156,  1162,  1171,  1173,  1178,  1183,  1188,  1193,  1198,
    1203,  1209,  1214,  1222,  1223,  1227,  1232,  1238,  1240,  1246,
    1252,  1258,  1267,  1277,  1279,  1288,  1290,  1298,  1300,  1305,
    1320,  1338,  1340,  1342,  1344,  1346,  1348,  1350,  1352,  1354,
    1359,  1361,  1369,  1373,  1375,  1383,  1385,  1391,  1397,  1403,
    1409,  1418,  1420,  1422,  1424,  1426,  1428,  1430,  1432,  1434,
    1436,  1438,  1440,  1442,  1444,  1446,  1448,  1450,  1452,  1454,
    1456,  1458,  1460,  1462,  1464,  1466,  1468,  1470,  1472,  1474,
    1476,  1478,  1480,  1485,  1487,  1492,  1497,  1505,  1507,  1514,
    1521,  1528,  1535,  1542,  1544,  1546,  1548,  1556,  1564,  1577,
    1579,  1581,  1590,  1599,  1612,  1621,  1630,  1639,  1641,  1643,
    1645,  1647,  1653,  1658,  1660,  1662,  1664,  1666,  1668,  1670,
    1672
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LABEL", "VARIABLE", "NUMBER", "TEXT",
  "COMMAND_LINE", "DELIMITED", "ORDINAL", "TH", "LEFT_ARROW_HEAD",
  "RIGHT_ARROW_HEAD", "DOUBLE_ARROW_HEAD", "LAST", "BOX", "CIRCLE",
  "ELLIPSE", "ARC", "LINE", "ARROW", "MOVE", "SPLINE", "HEIGHT", "RADIUS",
  "FIGNAME", "WIDTH", "DIAMETER", "UP", "DOWN", "RIGHT", "LEFT", "FROM",
  "TO", "AT", "WITH", "BY", "THEN", "SOLID", "DOTTED", "DASHED", "CHOP",
  "SAME", "INVISIBLE", "LJUST", "RJUST", "ABOVE", "BELOW", "OF", "THE",
  "WAY", "BETWEEN", "AND", "HERE", "DOT_N", "DOT_E", "DOT_W", "DOT_S",
  "DOT_NE", "DOT_SE", "DOT_NW", "DOT_SW", "DOT_C", "DOT_START", "DOT_END",
  "DOT_X", "DOT_Y", "DOT_HT", "DOT_WID", "DOT_RAD", "SIN", "COS", "ATAN2",
  "LOG", "EXP", "SQRT", "K_MAX", "K_MIN", "INT", "RAND", "SRAND", "COPY",
  "THRU", "TOP", "BOTTOM", "UPPER", "LOWER", "SH", "PRINT", "CW", "CCW",
  "FOR", "DO", "IF", "ELSE", "ANDAND", "OROR", "NOTEQUAL", "EQUALEQUAL",
  "LESSEQUAL", "GREATEREQUAL", "LEFT_CORNER", "RIGHT_CORNER", "NORTH",
  "SOUTH", "EAST", "WEST", "CENTER", "END", "START", "RESET", "UNTIL",
  "PLOT", "THICKNESS", "FILL", "COLORED", "OUTLINED", "SHADED", "XSLANTED",
  "YSLANTED", "ALIGNED", "SPRINTF", "COMMAND", "DEFINE", "UNDEF", "'.'",
  "'('", "'`'", "'['", "','", "'<'", "'>'", "'+'", "'-'", "'*'", "'/'",
  "'%'", "'!'", "'^'", "';'", "'='", "':'", "'{'", "'}'", "']'", "')'",
  "$accept", "top", "element_list", "middle_element_list",
  "optional_separator", "separator", "placeless_element", "$@1", "$@2",
  "$@3", "$@4", "$@5", "$@6", "$@7", "macro_name", "reset_variables",
  "print_args", "print_arg", "simple_if", "$@8", "until", "any_expr",
  "text_expr", "optional_by", "element", "@9", "$@10", "optional_element",
  "object_spec", "@11", "text", "sprintf_args", "position",
  "position_not_place", "between", "expr_pair", "place", "label",
  "ordinal", "optional_ordinal_last", "nth_primitive", "object_type",
  "label_path", "relative_path", "path", "corner", "expr",
  "expr_lower_than", "expr_not_lower_than", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,    46,    40,    96,    91,    44,
      60,    62,    43,    45,    42,    47,    37,    33,    94,    59,
      61,    58,   123,   125,    93,    41
};
# endif

#define YYPACT_NINF -240

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-240)))

#define YYTABLE_NINF -206

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -114,  -240,    20,  -240,   757,  -107,  -240,   -98,  -123,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -106,
    -240,  -240,  -240,  -240,     9,  -240,  1087,    46,  1172,    49,
    1597,   -70,  1087,  -240,  -240,  -114,  -240,     3,   -33,  -240,
     877,  -240,  -240,  -114,  1172,   -60,    36,   -14,  -240,    74,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,   -34,
     -18,     8,    38,    47,    51,    65,   101,   102,   112,   122,
    -240,  -240,    21,   150,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  1257,  1172,  1597,  1597,  1087,  -240,  -240,
     -43,  -240,  -240,   357,  2242,    59,   258,  -240,    10,  2147,
    -240,     1,     6,  1172,  1172,   145,    -1,     2,   357,  2273,
    -240,  -240,   220,   249,  1087,  -114,  -114,  -240,   721,  -240,
     252,  -240,  -240,  -240,  -240,  1597,  1597,  1597,  1597,  2024,
    2024,  1853,  1939,  1682,  1682,  1682,  1427,  1767,  -240,  -240,
    2024,  2024,  2024,  -240,  -240,  -240,  -240,  -240,  -240,  -240,
    -240,  1597,  2024,    23,    23,    23,  1597,  1597,  -240,  -240,
    2282,   593,  -240,  1172,  -240,  -240,  -240,  -240,   250,  -240,
    1172,  1172,  1172,  1172,  1172,  1172,  1172,  1172,  1172,   458,
    1172,  -240,  -240,  -240,  -240,  -240,  -240,  -240,  -240,   121,
     107,   123,   256,  2157,   137,   261,   134,   134,  -240,  1767,
    1767,  -240,  -240,  -240,  -240,  -240,   276,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,  -240,   138,  -240,  -240,
      24,   156,   235,  -240,  1597,  1597,  1597,  1597,  1597,  1597,
    1597,  1597,  1597,  1597,  1597,  1597,  1597,  1597,  1597,  1682,
    1682,  1597,  -240,   134,  -240,  1172,  1172,    23,    23,  1172,
    1172,  -240,  -240,   143,   757,   153,  -240,  -240,   280,  2282,
    2282,  2282,  2282,  2282,  2282,  2282,  2282,   -43,  2147,   -43,
     -43,  2253,   275,   275,   295,  1002,   -43,  2081,  -240,  -240,
      10,  1342,  -240,   694,  2282,  2282,  2282,  2282,  2282,  -240,
    -240,  -240,  2282,  2282,   -98,  -123,    16,    28,  -240,   -43,
      56,   302,  -240,   291,  -240,   155,   160,   172,   161,   164,
     167,   184,   185,   181,  -240,   186,   188,  -240,  1682,  1767,
    1767,  -240,  -240,  1682,  1682,  -240,  -240,  -240,  -240,  -240,
     156,   279,   314,  2291,   440,   440,   413,   413,  2282,   413,
     413,   -72,   -72,   134,   134,   134,   134,   -49,   117,   343,
     322,  -240,   314,   239,  2300,  -240,  -240,  -240,   314,   239,
    2300,  -119,  -240,  -240,  -240,  -240,  -240,  2116,  2116,  -240,
     206,   333,  -240,   123,  2131,  -240,   228,  -240,  -240,  1172,
    -240,  -240,  -240,  1172,  1172,  -240,  -240,  -240,  -110,   195,
     197,   -47,   128,   292,  1682,  1682,  1597,  -240,  1597,  -240,
     757,  -240,  -240,  2116,  -240,   228,   338,  -240,   200,   202,
     212,  -240,  -240,  -240,  1682,  1682,  -240,   -43,   -27,   360,
    2282,  -240,  -240,   214,  -240,  -240,  -240,  -240,  -240,   -73,
      30,  -240,  1512,   268,  -240,  -240,   216,  1597,  2282,  -240,
    -240,  2282,   354,  -240
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
       7,     9,     0,     3,     2,     8,     1,     0,     0,   136,
      18,    75,    76,    77,    78,    79,    80,    81,    82,     0,
      14,    15,    17,    16,     0,    21,     0,     0,     0,    36,
       0,     0,     0,    86,    69,     7,    72,    35,    32,     5,
      65,    83,    10,     7,     0,     0,     0,    23,    27,     0,
     162,   226,   227,   165,   167,   205,   204,   161,   191,   192,
     193,   194,   195,   196,   197,   198,   199,   200,   201,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     202,   203,     0,     0,   210,   211,   216,   217,   218,   219,
     220,   222,   221,     0,     0,     0,     0,    20,    42,    45,
      46,   140,   143,   141,   157,     0,     0,   163,     0,    44,
     223,   224,     0,     0,     0,     0,    52,     0,     0,    51,
     224,    39,    84,     0,    19,     7,     7,     4,     8,    40,
       0,    33,   124,   125,   126,     0,     0,     0,     0,    93,
      95,    97,    99,     0,     0,     0,     0,     0,   107,   108,
     109,   111,   120,   122,   123,   130,   131,   132,   133,   127,
     128,     0,   113,     0,     0,     0,     0,     0,   135,   129,
      92,     0,    12,     0,    38,    37,    11,    24,     0,    22,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   208,   206,   212,   214,   209,   207,   213,   215,     0,
       0,   143,   141,    51,   224,     0,   239,   260,    43,     0,
       0,   228,   229,   230,   231,   232,     0,   158,   179,   168,
     171,   172,   173,   174,   175,   176,   177,     0,   169,   170,
       0,   159,     0,   153,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    61,   260,    47,     0,     0,     0,     0,     0,
       0,    85,   138,     0,     0,     0,     6,    41,     0,    88,
      89,    90,    91,    94,    96,    98,   100,   101,     0,   102,
     103,   162,   165,   167,     0,     0,   105,   183,   185,   104,
     182,     0,   106,     0,   110,   112,   121,   134,   114,   118,
     119,   117,   115,   116,   162,   226,   205,   204,    66,     0,
      67,    68,    13,     0,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   251,     0,     0,   240,     0,     0,
       0,   156,   142,     0,     0,   166,   144,   146,   164,   178,
     160,     0,   258,   259,   257,   256,   253,   255,   155,   225,
     254,   233,   234,   235,   236,   237,   238,     0,     0,     0,
       0,    55,    56,    58,    59,    54,    53,    57,   258,    60,
     259,     0,    87,    70,    34,   190,   182,     0,     0,   180,
       0,     0,   184,     0,    51,    25,    49,   241,   242,     0,
     244,   245,   246,     0,     0,   249,   250,   252,     0,   144,
     146,     0,     0,     0,     0,     0,     0,    48,     0,   137,
      73,   189,   188,     0,   181,    49,     0,    29,     0,     0,
       0,   148,   145,   147,     0,     0,   154,   149,     0,    62,
     139,    74,    71,     0,    26,    50,   243,   247,   248,   149,
       0,   151,     0,     0,   186,   150,   151,     0,    63,    30,
     152,    64,     0,    31
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -240,  -240,    17,  -240,    12,   329,  -240,  -240,  -240,  -240,
    -240,  -240,  -240,  -240,  -240,  -240,   334,   -76,  -240,  -240,
     -42,    13,  -103,  -240,  -127,  -240,  -240,  -240,  -240,  -240,
       5,  -240,    99,   194,   169,   -44,     4,  -100,  -240,  -240,
    -240,  -104,  -240,  -239,  -240,   -50,   -26,  -240,    61
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    35,   264,     5,    36,    49,   313,   415,
     178,   386,   452,   268,   176,    37,    97,    98,    38,   360,
     417,   199,   116,   443,    39,   126,   410,   432,    40,   125,
     117,   371,   100,   101,   249,   102,   118,   104,   105,   106,
     107,   228,   287,   288,   289,   108,   119,   110,   120
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
     109,   266,   229,   404,   122,   424,   109,   129,   231,    41,
     408,   252,     4,    50,   170,    47,   -17,    44,    45,    53,
       6,   208,   209,   210,    54,     1,   409,    50,   -16,     9,
     103,    99,    42,    53,    46,   421,   103,    99,    54,   174,
     175,   115,   375,    43,   308,   169,   380,   127,   208,   201,
     112,   191,   192,   121,   217,   171,   123,   172,   230,   209,
     210,   131,   245,   246,   247,   218,   248,   203,   177,   206,
     207,   109,   445,   219,   220,   221,   222,   223,   224,   225,
     173,   226,   179,   209,   210,   209,   210,   111,   253,   209,
     210,    48,   180,   111,   255,   256,   290,   202,   109,   257,
     258,   103,    99,   292,   441,   209,   210,   205,   181,   269,
     270,   271,   272,   273,   274,   275,   276,   278,   278,   278,
     278,   293,   193,   194,   294,   295,   296,   261,   103,    99,
     340,   250,   130,    41,   182,   297,   298,    94,   411,   412,
     302,   303,   263,   265,    31,   278,   251,   103,   103,   103,
     103,    94,   361,   363,   204,   -17,   367,   369,   111,   -17,
     -17,   446,   209,   210,   183,   336,   337,   -16,   299,   300,
     301,   -16,   -16,   184,   433,   311,    41,   185,   377,   378,
     195,   196,   254,   293,   293,   111,   312,   227,  -140,  -140,
     231,   186,   200,   315,   316,   317,   318,   319,   320,   321,
     322,   323,   325,   326,   111,   111,   111,   111,   342,   343,
     344,   345,   346,   347,   348,   349,   350,   351,   352,   353,
     354,   355,   356,   278,   278,   359,     9,   187,   188,   362,
     364,   376,   111,   368,   370,   290,   328,   382,   189,   329,
     330,   201,   277,   279,   280,   286,   405,   383,   190,   209,
     210,   197,   198,   103,   103,   262,   267,   425,   314,   203,
     209,   210,   365,   366,   218,   384,   327,   334,   331,    41,
     309,   335,   248,   220,   221,   222,   223,   224,   225,   338,
     226,   216,   339,   431,   341,   399,   400,   372,   374,   202,
     220,   221,   222,   223,   224,   225,   373,   226,   379,   385,
     387,   389,   278,   293,   293,   388,   390,   278,   278,   391,
     111,   111,   392,   393,   394,   234,   235,   236,   237,   238,
     239,   211,   212,   213,   214,   215,   395,   376,   376,   403,
     407,   396,   103,   397,   255,   413,   414,   103,   103,   416,
     422,    31,   423,   426,   435,   436,   204,   437,   357,   358,
     241,   242,   243,   244,   245,   246,   247,   438,   248,   444,
     449,   450,   453,   376,   128,   310,   124,   211,   212,   213,
     214,   215,   333,   434,     0,     0,   406,     0,   278,   278,
     429,     0,   430,     0,   200,     0,   227,     0,     0,   111,
       0,     0,     0,     0,   111,   111,   442,     0,   278,   278,
       0,   332,   418,   227,     0,     0,   419,   420,   103,   103,
       0,   236,   237,   238,   239,    41,   448,     0,     0,     0,
       0,   451,   211,   212,   213,   214,   215,   398,   103,   103,
       0,     0,   401,   402,  -141,  -141,     0,     0,   234,   235,
     236,   237,   238,   239,   241,   242,   243,   244,   245,   246,
     247,     0,   248,     0,     0,   234,   235,   236,   237,   238,
     239,    50,    51,    52,     9,   111,   111,    53,     0,     0,
       0,     0,    54,   241,   242,   243,   244,   245,   246,   247,
       0,   248,     0,     0,     0,   111,   111,     0,    55,    56,
     241,   242,   243,   244,   245,   246,   247,     0,   248,     0,
       0,     0,     0,   427,   428,     0,     0,     0,     0,     0,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,   439,   440,     0,     0,     0,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,   238,
     239,    80,    81,    82,    83,   243,   244,   245,   246,   247,
       0,   248,     0,     0,     0,     0,     0,     0,     0,    84,
      85,    86,    87,    88,    89,    90,    91,    92,     0,     0,
     241,   242,   243,   244,   245,   246,   247,     0,   248,    31,
       0,     0,     0,     0,   113,    94,     0,     0,     0,     0,
       0,    95,     0,     0,     0,   114,   304,   305,    52,     9,
      10,     0,    53,   324,     0,     0,     0,    54,    11,    12,
      13,    14,    15,    16,    17,    18,     0,     0,    19,     0,
       0,    20,    21,   306,   307,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,     0,
       0,     0,     0,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    24,     0,    80,    81,    82,    83,
      25,    26,     0,     0,    27,     0,    28,     0,     0,     0,
       0,     0,     0,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,    29,     0,    30,     0,     0,     0,     0,
       0,     0,     0,     0,    31,    32,     0,     0,     0,    93,
      94,    33,     0,     0,     7,     8,    95,     9,    10,     0,
      96,     0,     0,     0,     0,    34,    11,    12,    13,    14,
      15,    16,    17,    18,     0,     0,    19,     0,     0,    20,
      21,    22,    23,     0,     0,     0,     0,     0,     0,     0,
       7,     8,     0,     9,    10,     0,     0,     0,     0,     0,
       0,     0,    11,    12,    13,    14,    15,    16,    17,    18,
       0,     0,    19,     0,     0,    20,    21,    22,    23,   234,
     235,   236,   237,   238,   239,     0,     0,     0,     0,     0,
       0,     0,    24,     0,     0,     0,     0,     0,    25,    26,
       0,     0,    27,     0,    28,     0,     0,     0,     0,     0,
       0,     0,     0,   240,   241,   242,   243,   244,   245,   246,
     247,    29,   248,    30,     0,     0,     0,     0,    24,     0,
       0,     0,    31,    32,    25,    26,     0,     0,    27,    33,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      42,     0,     0,    34,     0,     0,     0,    29,     0,    30,
       0,     0,     0,     0,     0,     0,     0,     0,    31,    32,
      50,    51,    52,     9,     0,    33,    53,     0,   132,   133,
     134,    54,     0,     0,     0,     0,     0,     0,     0,    34,
     135,   136,     0,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,     0,     0,     0,     0,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
      80,    81,    82,    83,     0,     0,   159,   160,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     0,     0,     0,
     161,   162,   163,   164,   165,   166,   167,   168,    31,     0,
       0,     0,     0,   113,    94,    50,    51,    52,     9,     0,
      95,    53,     0,     0,    96,     0,    54,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    55,    56,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,     0,     0,     0,
       0,     0,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,     0,    80,    81,    82,    83,     0,
      50,    51,    52,     9,     0,     0,    53,     0,     0,     0,
       0,    54,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,    31,     0,     0,     0,   284,    93,    94,
       0,     0,     0,     0,     0,    95,     0,     0,     0,   114,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
      80,    81,    82,    83,     0,    50,    51,    52,     9,     0,
       0,    53,     0,     0,     0,     0,    54,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     0,     0,     0,
       0,     0,    55,    56,     0,     0,     0,     0,    31,     0,
       0,     0,     0,    93,    94,     0,     0,     0,     0,     0,
      95,     0,     0,     0,    96,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,     0,     0,     0,
       0,     0,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,     0,    80,    81,    82,    83,     0,
      50,    51,    52,     9,     0,     0,    53,     0,     0,     0,
       0,    54,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,    31,     0,     0,     0,     0,   113,    94,
       0,     0,     0,     0,     0,    95,     0,     0,     0,   114,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
      80,    81,    82,    83,     0,    50,    51,    52,     9,     0,
       0,    53,     0,     0,     0,     0,    54,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     0,     0,     0,
       0,     0,    55,    56,     0,     0,     0,     0,    31,     0,
       0,     0,     0,    93,    94,     0,     0,     0,     0,     0,
      95,     0,     0,     0,   114,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,     0,     0,     0,
       0,     0,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,     0,    80,    81,    82,    83,     0,
     281,    51,    52,     0,     0,     0,   282,     0,     0,     0,
       0,   283,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,    31,     0,     0,     0,     0,   291,    94,
       0,     0,     0,     0,     0,    95,     0,     0,     0,   114,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
      80,    81,    82,    83,     0,    50,    51,    52,     0,     0,
       0,    53,     0,     0,     0,     0,    54,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     0,     0,     0,
       0,     0,    55,    56,     0,     0,     0,     0,     0,     0,
       0,     0,   284,   285,    94,     0,     0,     0,     0,     0,
      95,     0,     0,     0,    96,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,     0,     0,     0,
       0,     0,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,     0,    80,    81,    82,    83,     0,
      50,    51,    52,     0,     0,     0,    53,     0,     0,     0,
       0,    54,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   113,    94,
       0,     0,     0,     0,     0,    95,   447,     0,     0,    96,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
      80,    81,    82,    83,     0,    50,    51,    52,     0,     0,
       0,    53,     0,     0,     0,     0,    54,     0,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     0,     0,     0,
       0,     0,    55,    56,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   113,    94,     0,     0,     0,     0,     0,
      95,     0,     0,     0,    96,    57,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,     0,     0,     0,
       0,     0,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,     0,     0,    80,    81,    82,    83,     0,
      50,    51,    52,     0,     0,     0,    53,     0,     0,     0,
       0,    54,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     0,     0,     0,     0,     0,    55,    56,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    93,    94,
       0,     0,     0,     0,     0,    95,     0,     0,     0,    96,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,     0,     0,     0,     0,     0,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,     0,     0,
      80,    81,    82,    83,     0,     0,    50,    51,    52,     0,
       0,     0,    53,     0,     0,     0,     0,    54,    84,    85,
      86,    87,    88,    89,    90,    91,    92,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   291,    94,     0,     0,     0,     0,     0,
      95,  -205,     0,     0,    96,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,     0,
       0,     0,     0,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,     0,     0,    80,    81,    82,    83,
       0,     0,    50,    51,    52,     0,     0,     0,    53,     0,
       0,     0,     0,    54,    84,    85,    86,    87,    88,    89,
      90,    91,    92,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   113,
      94,     0,     0,     0,     0,     0,    95,  -204,     0,     0,
      96,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,     0,     0,     0,     0,     0,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
       0,     0,    80,    81,    82,    83,     0,    50,    51,    52,
       0,     0,     0,    53,     0,     0,     0,     0,    54,     0,
      84,    85,    86,    87,    88,    89,    90,    91,    92,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   113,    94,     0,     0,     0,
       0,     0,    95,     0,     0,     0,    96,    57,    58,    59,
      60,    61,    62,    63,    64,    65,    66,    67,    68,     0,
       0,     0,     0,     0,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,     0,     0,    80,    81,    82,
      83,    55,    56,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    85,    86,    87,    88,
      89,    90,    91,    92,     0,    58,    59,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    55,    56,     0,     0,
     113,    94,     0,     0,     0,     0,     0,    95,     0,     0,
       0,    96,     0,     0,    80,    81,    82,    83,     0,     0,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,     0,    84,    85,    86,    87,    88,    89,    90,    91,
      92,     0,     0,     0,     0,   232,     0,     0,   233,    80,
      81,    82,    83,     0,     0,   232,   381,     0,   233,     0,
       0,     0,     0,     0,     0,     0,     0,    84,    85,    86,
      87,    88,    89,    90,    91,    92,   259,   260,   236,   237,
     238,   239,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   284,   234,   235,   236,   237,   238,   239,     0,     0,
       0,     0,   259,   260,   236,   237,   238,   239,     0,     0,
     240,   241,   242,   243,   244,   245,   246,   247,     0,   248,
       0,     0,     0,     0,     0,     0,   240,   241,   242,   243,
     244,   245,   246,   247,     0,   248,   240,   241,   242,   243,
     244,   245,   246,   247,     0,   248,    58,    59,    60,    61,
      62,    63,    64,    65,    66,    67,    68,    58,    59,    60,
      61,    62,    63,    64,    65,    66,    67,    68,     0,     0,
       0,     0,     0,     0,     0,    80,    81,    82,    83,     0,
       0,     0,     0,     0,     0,     0,    80,    81,    82,    83,
       0,     0,     0,    84,    85,    86,    87,    88,    89,    90,
      91,    92,     0,     0,    84,    85,    86,    87,    88,    89,
      90,    91,    92,     0,     0,     0,     0,   216,   259,   260,
     236,   237,   238,   239,     0,     0,     0,   234,   235,   236,
     237,   238,   239,     0,     0,     0,   234,     0,   236,   237,
     238,   239,     0,     0,     0,   259,     0,   236,   237,   238,
     239,     0,     0,   241,   242,   243,   244,   245,   246,   247,
       0,   248,   241,   242,   243,   244,   245,   246,   247,     0,
     248,   241,   242,   243,   244,   245,   246,   247,     0,   248,
     241,   242,   243,   244,   245,   246,   247,     0,   248
};

static const yytype_int16 yycheck[] =
{
      26,   128,   106,    52,    30,    52,    32,     4,   108,     4,
     129,   114,     0,     3,    40,     6,     0,   140,   141,     9,
       0,    97,   132,   133,    14,   139,   145,     3,     0,     6,
      26,    26,   139,     9,   140,   145,    32,    32,    14,     3,
       4,    28,   281,   141,   171,    40,   285,    35,   124,    93,
       4,    30,    31,     4,   104,    43,   126,    44,    48,   132,
     133,    94,   134,   135,   136,     6,   138,    93,    82,    95,
      96,    97,   145,    14,    15,    16,    17,    18,    19,    20,
     140,    22,     8,   132,   133,   132,   133,    26,   114,   132,
     133,    82,   126,    32,    95,    96,   146,    93,   124,    97,
      98,    97,    97,   147,   131,   132,   133,    94,   126,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   101,   102,   150,   151,   152,   122,   124,   124,
     230,   130,   129,   128,   126,   161,   162,   127,   377,   378,
     166,   167,   125,   126,   121,   171,   140,   143,   144,   145,
     146,   127,   255,   256,    93,   139,   259,   260,    97,   143,
     144,   131,   132,   133,   126,   209,   210,   139,   163,   164,
     165,   143,   144,   126,   413,   171,   171,   126,   282,   283,
      30,    31,    37,   209,   210,   124,   173,   128,   132,   133,
     290,   126,    93,   180,   181,   182,   183,   184,   185,   186,
     187,   188,   189,   190,   143,   144,   145,   146,   234,   235,
     236,   237,   238,   239,   240,   241,   242,   243,   244,   245,
     246,   247,   248,   249,   250,   251,     6,   126,   126,   255,
     256,   281,   171,   259,   260,   285,   129,   287,   126,   132,
     133,   285,   143,   144,   145,   146,   129,   291,   126,   132,
     133,   101,   102,   249,   250,     6,     4,   129,     8,   285,
     132,   133,   257,   258,     6,   291,   145,   130,   145,   264,
     171,    10,   138,    15,    16,    17,    18,    19,    20,     3,
      22,   125,   144,   410,    49,   329,   330,   144,     8,   285,
      15,    16,    17,    18,    19,    20,   143,    22,     3,     8,
     145,   129,   328,   329,   330,   145,   145,   333,   334,   145,
     249,   250,   145,   129,   129,    95,    96,    97,    98,    99,
     100,    65,    66,    67,    68,    69,   145,   377,   378,    50,
       8,   145,   328,   145,    95,   129,     3,   333,   334,   111,
     145,   121,   145,    51,     6,   145,   285,   145,   249,   250,
     130,   131,   132,   133,   134,   135,   136,   145,   138,   145,
      92,   145,     8,   413,    35,   171,    32,    65,    66,    67,
      68,    69,   203,   415,    -1,    -1,    33,    -1,   404,   405,
     406,    -1,   408,    -1,   285,    -1,   128,    -1,    -1,   328,
      -1,    -1,    -1,    -1,   333,   334,    36,    -1,   424,   425,
      -1,   145,   389,   128,    -1,    -1,   393,   394,   404,   405,
      -1,    97,    98,    99,   100,   410,   442,    -1,    -1,    -1,
      -1,   447,    65,    66,    67,    68,    69,   328,   424,   425,
      -1,    -1,   333,   334,   132,   133,    -1,    -1,    95,    96,
      97,    98,    99,   100,   130,   131,   132,   133,   134,   135,
     136,    -1,   138,    -1,    -1,    95,    96,    97,    98,    99,
     100,     3,     4,     5,     6,   404,   405,     9,    -1,    -1,
      -1,    -1,    14,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,    -1,    -1,    -1,   424,   425,    -1,    30,    31,
     130,   131,   132,   133,   134,   135,   136,    -1,   138,    -1,
      -1,    -1,    -1,   404,   405,    -1,    -1,    -1,    -1,    -1,
      -1,    53,    54,    55,    56,    57,    58,    59,    60,    61,
      62,    63,    64,   424,   425,    -1,    -1,    -1,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    99,
     100,    83,    84,    85,    86,   132,   133,   134,   135,   136,
      -1,   138,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,
     102,   103,   104,   105,   106,   107,   108,   109,    -1,    -1,
     130,   131,   132,   133,   134,   135,   136,    -1,   138,   121,
      -1,    -1,    -1,    -1,   126,   127,    -1,    -1,    -1,    -1,
      -1,   133,    -1,    -1,    -1,   137,     3,     4,     5,     6,
       7,    -1,     9,   145,    -1,    -1,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    -1,    -1,    25,    -1,
      -1,    28,    29,    30,    31,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    -1,    83,    84,    85,    86,
      87,    88,    -1,    -1,    91,    -1,    93,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,    -1,   112,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   121,   122,    -1,    -1,    -1,   126,
     127,   128,    -1,    -1,     3,     4,   133,     6,     7,    -1,
     137,    -1,    -1,    -1,    -1,   142,    15,    16,    17,    18,
      19,    20,    21,    22,    -1,    -1,    25,    -1,    -1,    28,
      29,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,    -1,     6,     7,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    15,    16,    17,    18,    19,    20,    21,    22,
      -1,    -1,    25,    -1,    -1,    28,    29,    30,    31,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    81,    -1,    -1,    -1,    -1,    -1,    87,    88,
      -1,    -1,    91,    -1,    93,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   129,   130,   131,   132,   133,   134,   135,
     136,   110,   138,   112,    -1,    -1,    -1,    -1,    81,    -1,
      -1,    -1,   121,   122,    87,    88,    -1,    -1,    91,   128,
      93,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     139,    -1,    -1,   142,    -1,    -1,    -1,   110,    -1,   112,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   121,   122,
       3,     4,     5,     6,    -1,   128,     9,    -1,    11,    12,
      13,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   142,
      23,    24,    -1,    26,    27,    28,    29,    30,    31,    32,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      43,    44,    45,    46,    47,    -1,    -1,    -1,    -1,    -1,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,    -1,    89,    90,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
     113,   114,   115,   116,   117,   118,   119,   120,   121,    -1,
      -1,    -1,    -1,   126,   127,     3,     4,     5,     6,    -1,
     133,     9,    -1,    -1,   137,    -1,    14,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,    -1,    30,    31,    -1,
      -1,    -1,    -1,   121,    -1,    -1,    -1,   125,   126,   127,
      -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,    -1,   137,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    -1,    -1,    -1,    -1,    14,    -1,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,    -1,    30,    31,    -1,    -1,    -1,    -1,   121,    -1,
      -1,    -1,    -1,   126,   127,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,    -1,   137,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    -1,
       3,     4,     5,     6,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,    -1,    30,    31,    -1,
      -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,   126,   127,
      -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,    -1,   137,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,     3,     4,     5,     6,    -1,
      -1,     9,    -1,    -1,    -1,    -1,    14,    -1,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,    -1,    30,    31,    -1,    -1,    -1,    -1,   121,    -1,
      -1,    -1,    -1,   126,   127,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,    -1,   137,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    -1,
       3,     4,     5,    -1,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,    -1,    30,    31,    -1,
      -1,    -1,    -1,   121,    -1,    -1,    -1,    -1,   126,   127,
      -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,    -1,   137,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,     3,     4,     5,    -1,    -1,
      -1,     9,    -1,    -1,    -1,    -1,    14,    -1,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   125,   126,   127,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,    -1,   137,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    -1,
       3,     4,     5,    -1,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,    -1,    30,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,   127,
      -1,    -1,    -1,    -1,    -1,   133,   134,    -1,    -1,   137,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,     3,     4,     5,    -1,    -1,
      -1,     9,    -1,    -1,    -1,    -1,    14,    -1,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,    -1,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   126,   127,    -1,    -1,    -1,    -1,    -1,
     133,    -1,    -1,    -1,   137,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    -1,    -1,    -1,
      -1,    -1,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    83,    84,    85,    86,    -1,
       3,     4,     5,    -1,    -1,    -1,     9,    -1,    -1,    -1,
      -1,    14,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,    -1,    -1,    -1,    -1,    30,    31,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,   127,
      -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,    -1,   137,
      53,    54,    55,    56,    57,    58,    59,    60,    61,    62,
      63,    64,    -1,    -1,    -1,    -1,    -1,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      83,    84,    85,    86,    -1,    -1,     3,     4,     5,    -1,
      -1,    -1,     9,    -1,    -1,    -1,    -1,    14,   101,   102,
     103,   104,   105,   106,   107,   108,   109,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   126,   127,    -1,    -1,    -1,    -1,    -1,
     133,    48,    -1,    -1,   137,    -1,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    83,    84,    85,    86,
      -1,    -1,     3,     4,     5,    -1,    -1,    -1,     9,    -1,
      -1,    -1,    -1,    14,   101,   102,   103,   104,   105,   106,
     107,   108,   109,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,
     127,    -1,    -1,    -1,    -1,    -1,   133,    48,    -1,    -1,
     137,    -1,    53,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    -1,    -1,    -1,    -1,    -1,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    83,    84,    85,    86,    -1,     3,     4,     5,
      -1,    -1,    -1,     9,    -1,    -1,    -1,    -1,    14,    -1,
     101,   102,   103,   104,   105,   106,   107,   108,   109,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   126,   127,    -1,    -1,    -1,
      -1,    -1,   133,    -1,    -1,    -1,   137,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    -1,
      -1,    -1,    -1,    -1,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    -1,    -1,    83,    84,    85,
      86,    30,    31,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   101,   102,   103,   104,   105,
     106,   107,   108,   109,    -1,    54,    55,    56,    57,    58,
      59,    60,    61,    62,    63,    64,    30,    31,    -1,    -1,
     126,   127,    -1,    -1,    -1,    -1,    -1,   133,    -1,    -1,
      -1,   137,    -1,    -1,    83,    84,    85,    86,    -1,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    -1,   101,   102,   103,   104,   105,   106,   107,   108,
     109,    -1,    -1,    -1,    -1,    48,    -1,    -1,    51,    83,
      84,    85,    86,    -1,    -1,    48,   125,    -1,    51,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,   102,   103,
     104,   105,   106,   107,   108,   109,    95,    96,    97,    98,
      99,   100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   125,    95,    96,    97,    98,    99,   100,    -1,    -1,
      -1,    -1,    95,    96,    97,    98,    99,   100,    -1,    -1,
     129,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
      -1,    -1,    -1,    -1,    -1,    -1,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,   129,   130,   131,   132,
     133,   134,   135,   136,    -1,   138,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    83,    84,    85,    86,
      -1,    -1,    -1,   101,   102,   103,   104,   105,   106,   107,
     108,   109,    -1,    -1,   101,   102,   103,   104,   105,   106,
     107,   108,   109,    -1,    -1,    -1,    -1,   125,    95,    96,
      97,    98,    99,   100,    -1,    -1,    -1,    95,    96,    97,
      98,    99,   100,    -1,    -1,    -1,    95,    -1,    97,    98,
      99,   100,    -1,    -1,    -1,    95,    -1,    97,    98,    99,
     100,    -1,    -1,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   130,   131,   132,   133,   134,   135,   136,    -1,
     138,   130,   131,   132,   133,   134,   135,   136,    -1,   138,
     130,   131,   132,   133,   134,   135,   136,    -1,   138
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   139,   147,   148,   150,   151,     0,     3,     4,     6,
       7,    15,    16,    17,    18,    19,    20,    21,    22,    25,
      28,    29,    30,    31,    81,    87,    88,    91,    93,   110,
     112,   121,   122,   128,   142,   149,   152,   161,   164,   170,
     174,   176,   139,   141,   140,   141,   140,     6,    82,   153,
       3,     4,     5,     9,    14,    30,    31,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      83,    84,    85,    86,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   126,   127,   133,   137,   162,   163,   176,
     178,   179,   181,   182,   183,   184,   185,   186,   191,   192,
     193,   194,     4,   126,   137,   167,   168,   176,   182,   192,
     194,     4,   192,   126,   162,   175,   171,   150,   151,     4,
     129,    94,    11,    12,    13,    23,    24,    26,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    89,
      90,   113,   114,   115,   116,   117,   118,   119,   120,   176,
     192,   150,   167,   140,     3,     4,   160,    82,   156,     8,
     126,   126,   126,   126,   126,   126,   126,   126,   126,   126,
     126,    30,    31,   101,   102,    30,    31,   101,   102,   167,
     178,   181,   182,   192,   194,   167,   192,   192,   163,   132,
     133,    65,    66,    67,    68,    69,   125,   191,     6,    14,
      15,    16,    17,    18,    19,    20,    22,   128,   187,   187,
      48,   183,    48,    51,    95,    96,    97,    98,    99,   100,
     129,   130,   131,   132,   133,   134,   135,   136,   138,   180,
     130,   140,   168,   192,    37,    95,    96,    97,    98,    95,
      96,   176,     6,   148,   150,   148,   170,     4,   159,   192,
     192,   192,   192,   192,   192,   192,   192,   178,   192,   178,
     178,     3,     9,    14,   125,   126,   178,   188,   189,   190,
     191,   126,   181,   192,   192,   192,   192,   192,   192,   176,
     176,   176,   192,   192,     3,     4,    30,    31,   170,   178,
     179,   182,   167,   154,     8,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   145,   167,   167,   145,   129,   132,
     133,   145,   145,   180,   130,    10,   181,   181,     3,   144,
     183,    49,   192,   192,   192,   192,   192,   192,   192,   192,
     192,   192,   192,   192,   192,   192,   192,   178,   178,   192,
     165,   168,   192,   168,   192,   176,   176,   168,   192,   168,
     192,   177,   144,   143,     8,   189,   191,   187,   187,     3,
     189,   125,   191,   181,   192,     8,   157,   145,   145,   129,
     145,   145,   145,   129,   129,   145,   145,   145,   178,   181,
     181,   178,   178,    50,    52,   129,    33,     8,   129,   145,
     172,   189,   189,   129,     3,   155,   111,   166,   167,   167,
     167,   145,   145,   145,    52,   129,    51,   178,   178,   192,
     192,   170,   173,   189,   166,     6,   145,   145,   145,   178,
     178,   131,    36,   169,   145,   145,   131,   134,   192,    92,
     145,   192,   158,     8
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   146,   147,   147,   148,   149,   149,   150,   150,   151,
     151,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   153,   152,   152,   154,   155,   152,   156,   157,   152,
     158,   152,   152,   159,   152,   152,   152,   160,   160,   161,
     161,   161,   162,   162,   163,   163,   163,   165,   164,   166,
     166,   167,   167,   168,   168,   168,   168,   168,   168,   168,
     168,   168,   169,   169,   169,   170,   170,   170,   170,   171,
     172,   170,   170,   173,   173,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   175,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   174,   174,   174,   174,
     174,   174,   174,   174,   174,   174,   176,   176,   177,   177,
     178,   178,   178,   179,   179,   179,   179,   179,   179,   179,
     179,   179,   179,   180,   180,   181,   181,   182,   182,   182,
     182,   182,   183,   183,   183,   184,   184,   185,   185,   186,
     186,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     188,   188,   189,   189,   189,   190,   190,   190,   190,   190,
     190,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   191,   191,   191,   191,   191,   191,   191,
     191,   191,   191,   192,   192,   193,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194,   194,   194,   194,   194,   194,   194,   194,   194,   194,
     194
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     3,     1,     3,     0,     1,     1,
       2,     3,     3,     4,     1,     1,     1,     1,     1,     2,
       2,     0,     3,     2,     0,     0,     7,     0,     0,     6,
       0,    10,     1,     0,     4,     1,     1,     1,     1,     2,
       2,     3,     1,     2,     1,     1,     1,     0,     5,     0,
       2,     1,     1,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     0,     2,     3,     1,     4,     4,     4,     0,
       0,     6,     1,     0,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     2,     3,     0,     4,     3,     3,
       3,     3,     2,     2,     3,     2,     3,     2,     3,     2,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     2,
       3,     2,     3,     2,     3,     3,     3,     3,     3,     3,
       2,     3,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     2,     1,     5,     0,     3,
       1,     1,     3,     1,     3,     5,     3,     5,     5,     5,
       7,     6,     8,     1,     4,     3,     3,     1,     2,     2,
       3,     1,     1,     1,     3,     1,     3,     1,     2,     2,
       2,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     3,     1,     1,     2,     1,     5,     4,     3,     3,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     2,     2,     2,     2,
       1,     1,     2,     2,     2,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     1,     2,     2,
       2,     2,     2,     3,     3,     3,     3,     3,     3,     2,
       3,     4,     4,     6,     4,     4,     4,     6,     6,     4,
       4,     3,     4,     3,     3,     3,     3,     3,     3,     3,
       2
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
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
                       &(yyvsp[(yyi + 1) - (yynrhs)])
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
            /* Fall through.  */
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
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
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
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
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
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

#ifdef yyoverflow
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
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
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
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

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
| yyreduce -- Do a reduction.  |
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
#line 277 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if (olist.head)
		    print_picture(olist.head);
		}
#line 2257 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 4:
#line 286 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pl) = (yyvsp[-1].pl); }
#line 2263 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 5:
#line 291 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pl) = (yyvsp[0].pl); }
#line 2269 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 6:
#line 293 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pl) = (yyvsp[-2].pl); }
#line 2275 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 11:
#line 308 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  a_delete graphname;
		  graphname = new char[strlen((yyvsp[0].str)) + 1];
		  strcpy(graphname, (yyvsp[0].str));
		  a_delete (yyvsp[0].str);
		}
#line 2286 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 12:
#line 316 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  define_variable((yyvsp[-2].str), (yyvsp[0].x));
		  free((yyvsp[-2].str));
		}
#line 2295 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 13:
#line 321 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  place *p = lookup_label((yyvsp[-3].str));
		  if (!p) {
		    lex_error("variable '%1' not defined", (yyvsp[-3].str));
		    YYABORT;
		  }
		  p->obj = 0;
		  p->x = (yyvsp[0].x);
		  p->y = 0.0;
		  free((yyvsp[-3].str));
		}
#line 2311 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 14:
#line 333 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { current_direction = UP_DIRECTION; }
#line 2317 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 15:
#line 335 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { current_direction = DOWN_DIRECTION; }
#line 2323 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 16:
#line 337 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { current_direction = LEFT_DIRECTION; }
#line 2329 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 17:
#line 339 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { current_direction = RIGHT_DIRECTION; }
#line 2335 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 18:
#line 341 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  olist.append(make_command_object((yyvsp[0].lstr).str, (yyvsp[0].lstr).filename,
						   (yyvsp[0].lstr).lineno));
		}
#line 2344 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 19:
#line 346 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  olist.append(make_command_object((yyvsp[0].lstr).str, (yyvsp[0].lstr).filename,
						   (yyvsp[0].lstr).lineno));
		}
#line 2353 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 20:
#line 351 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  fprintf(stderr, "%s\n", (yyvsp[0].lstr).str);
		  a_delete (yyvsp[0].lstr).str;
		  fflush(stderr);
		}
#line 2363 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 21:
#line 357 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 1; }
#line 2369 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 22:
#line 359 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  delim_flag = 0;
		  if (safer_flag)
		    lex_error("unsafe to run command '%1'", (yyvsp[0].str));
		  else
		    system((yyvsp[0].str));
		  a_delete (yyvsp[0].str);
		}
#line 2382 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 23:
#line 368 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if (yychar < 0)
		    do_lookahead();
		  do_copy((yyvsp[0].lstr).str);
		  // do not delete the filename
		}
#line 2393 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 24:
#line 375 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 2; }
#line 2399 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 25:
#line 377 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 0; }
#line 2405 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 26:
#line 379 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if (yychar < 0)
		    do_lookahead();
		  copy_file_thru((yyvsp[-5].lstr).str, (yyvsp[-2].str), (yyvsp[0].str));
		  // do not delete the filename
		  a_delete (yyvsp[-2].str);
		  a_delete (yyvsp[0].str);
		}
#line 2418 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 27:
#line 388 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 2; }
#line 2424 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 28:
#line 390 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 0; }
#line 2430 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 29:
#line 392 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if (yychar < 0)
		    do_lookahead();
		  copy_rest_thru((yyvsp[-2].str), (yyvsp[0].str));
		  a_delete (yyvsp[-2].str);
		  a_delete (yyvsp[0].str);
		}
#line 2442 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 30:
#line 400 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 1; }
#line 2448 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 31:
#line 402 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  delim_flag = 0;
		  if (yychar < 0)
		    do_lookahead();
		  do_for((yyvsp[-8].str), (yyvsp[-6].x), (yyvsp[-4].x), (yyvsp[-3].by).is_multiplicative, (yyvsp[-3].by).val, (yyvsp[0].str)); 
		}
#line 2459 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 32:
#line 409 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if (yychar < 0)
		    do_lookahead();
		  if ((yyvsp[0].if_data).x != 0.0)
		    push_body((yyvsp[0].if_data).body);
		  a_delete (yyvsp[0].if_data).body;
		}
#line 2471 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 33:
#line 417 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 1; }
#line 2477 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 34:
#line 419 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  delim_flag = 0;
		  if (yychar < 0)
		    do_lookahead();
		  if ((yyvsp[-3].if_data).x != 0.0)
		    push_body((yyvsp[-3].if_data).body);
		  else
		    push_body((yyvsp[0].str));
		  free((yyvsp[-3].if_data).body);
		  free((yyvsp[0].str));
		}
#line 2493 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 36:
#line 432 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { define_variable("scale", 1.0); }
#line 2499 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 39:
#line 442 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  reset((yyvsp[0].str));
		  a_delete (yyvsp[0].str);
		}
#line 2508 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 40:
#line 447 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  reset((yyvsp[0].str));
		  a_delete (yyvsp[0].str);
		}
#line 2517 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 41:
#line 452 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  reset((yyvsp[0].str));
		  a_delete (yyvsp[0].str);
		}
#line 2526 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 42:
#line 460 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.lstr) = (yyvsp[0].lstr); }
#line 2532 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 43:
#line 462 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.lstr).str = new char[strlen((yyvsp[-1].lstr).str) + strlen((yyvsp[0].lstr).str) + 1];
		  strcpy((yyval.lstr).str, (yyvsp[-1].lstr).str);
		  strcat((yyval.lstr).str, (yyvsp[0].lstr).str);
		  a_delete (yyvsp[-1].lstr).str;
		  a_delete (yyvsp[0].lstr).str;
		  if ((yyvsp[-1].lstr).filename) {
		    (yyval.lstr).filename = (yyvsp[-1].lstr).filename;
		    (yyval.lstr).lineno = (yyvsp[-1].lstr).lineno;
		  }
		  else if ((yyvsp[0].lstr).filename) {
		    (yyval.lstr).filename = (yyvsp[0].lstr).filename;
		    (yyval.lstr).lineno = (yyvsp[0].lstr).lineno;
		  }
		}
#line 2552 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 44:
#line 481 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.lstr).str = new char[GDIGITS + 1];
		  sprintf((yyval.lstr).str, "%g", (yyvsp[0].x));
		  (yyval.lstr).filename = 0;
		  (yyval.lstr).lineno = 0;
		}
#line 2563 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 45:
#line 488 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.lstr) = (yyvsp[0].lstr); }
#line 2569 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 46:
#line 490 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.lstr).str = new char[GDIGITS + 2 + GDIGITS + 1];
		  sprintf((yyval.lstr).str, "%g, %g", (yyvsp[0].pair).x, (yyvsp[0].pair).y);
		  (yyval.lstr).filename = 0;
		  (yyval.lstr).lineno = 0;
		}
#line 2580 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 47:
#line 500 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { delim_flag = 1; }
#line 2586 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 48:
#line 502 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  delim_flag = 0;
		  (yyval.if_data).x = (yyvsp[-3].x);
		  (yyval.if_data).body = (yyvsp[0].str);
		}
#line 2596 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 49:
#line 511 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.str) = 0; }
#line 2602 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 50:
#line 513 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.str) = (yyvsp[0].lstr).str; }
#line 2608 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 51:
#line 518 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[0].x); }
#line 2614 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 52:
#line 520 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[0].x); }
#line 2620 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 53:
#line 525 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.x) = strcmp((yyvsp[-2].lstr).str, (yyvsp[0].lstr).str) == 0;
		  a_delete (yyvsp[-2].lstr).str;
		  a_delete (yyvsp[0].lstr).str;
		}
#line 2630 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 54:
#line 531 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.x) = strcmp((yyvsp[-2].lstr).str, (yyvsp[0].lstr).str) != 0;
		  a_delete (yyvsp[-2].lstr).str;
		  a_delete (yyvsp[0].lstr).str;
		}
#line 2640 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 55:
#line 537 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 && (yyvsp[0].x) != 0.0); }
#line 2646 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 56:
#line 539 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 && (yyvsp[0].x) != 0.0); }
#line 2652 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 57:
#line 541 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 && (yyvsp[0].x) != 0.0); }
#line 2658 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 58:
#line 543 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 || (yyvsp[0].x) != 0.0); }
#line 2664 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 59:
#line 545 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 || (yyvsp[0].x) != 0.0); }
#line 2670 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 60:
#line 547 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 || (yyvsp[0].x) != 0.0); }
#line 2676 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 61:
#line 549 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[0].x) == 0.0); }
#line 2682 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 62:
#line 555 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.by).val = 1.0;
		  (yyval.by).is_multiplicative = 0;
		}
#line 2691 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 63:
#line 560 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.by).val = (yyvsp[0].x);
		  (yyval.by).is_multiplicative = 0;
		}
#line 2700 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 64:
#line 565 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.by).val = (yyvsp[0].x);
		  (yyval.by).is_multiplicative = 1;
		}
#line 2709 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 65:
#line 573 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl).obj = (yyvsp[0].spec)->make_object(&current_position,
					   &current_direction);
		  if ((yyval.pl).obj == 0)
		    YYABORT;
		  delete (yyvsp[0].spec);
		  if ((yyval.pl).obj)
		    olist.append((yyval.pl).obj);
		  else {
		    (yyval.pl).x = current_position.x;
		    (yyval.pl).y = current_position.y;
		  }
		}
#line 2727 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 66:
#line 587 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl) = (yyvsp[0].pl);
		  define_label((yyvsp[-3].str), & (yyval.pl));
		  free((yyvsp[-3].str));
		}
#line 2737 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 67:
#line 593 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl).obj = 0;
		  (yyval.pl).x = (yyvsp[0].pair).x;
		  (yyval.pl).y = (yyvsp[0].pair).y;
		  define_label((yyvsp[-3].str), & (yyval.pl));
		  free((yyvsp[-3].str));
		}
#line 2749 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 68:
#line 601 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl) = (yyvsp[0].pl);
		  define_label((yyvsp[-3].str), & (yyval.pl));
		  free((yyvsp[-3].str));
		}
#line 2759 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 69:
#line 607 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.state).x = current_position.x;
		  (yyval.state).y = current_position.y;
		  (yyval.state).dir = current_direction;
		}
#line 2769 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 70:
#line 613 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  current_position.x = (yyvsp[-2].state).x;
		  current_position.y = (yyvsp[-2].state).y;
		  current_direction = (yyvsp[-2].state).dir;
		}
#line 2779 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 71:
#line 619 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl) = (yyvsp[-3].pl);
		}
#line 2787 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 72:
#line 623 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl).obj = 0;
		  (yyval.pl).x = current_position.x;
		  (yyval.pl).y = current_position.y;
		}
#line 2797 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 73:
#line 632 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {}
#line 2803 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 74:
#line 634 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {}
#line 2809 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 75:
#line 639 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.spec) = new object_spec(BOX_OBJECT); }
#line 2815 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 76:
#line 641 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.spec) = new object_spec(CIRCLE_OBJECT); }
#line 2821 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 77:
#line 643 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.spec) = new object_spec(ELLIPSE_OBJECT); }
#line 2827 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 78:
#line 645 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(ARC_OBJECT);
		  (yyval.spec)->dir = current_direction;
		}
#line 2836 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 79:
#line 650 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(LINE_OBJECT);
		  lookup_variable("lineht", & (yyval.spec)->segment_height);
		  lookup_variable("linewid", & (yyval.spec)->segment_width);
		  (yyval.spec)->dir = current_direction;
		}
#line 2847 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 80:
#line 657 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(ARROW_OBJECT);
		  lookup_variable("lineht", & (yyval.spec)->segment_height);
		  lookup_variable("linewid", & (yyval.spec)->segment_width);
		  (yyval.spec)->dir = current_direction;
		}
#line 2858 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 81:
#line 664 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(MOVE_OBJECT);
		  lookup_variable("moveht", & (yyval.spec)->segment_height);
		  lookup_variable("movewid", & (yyval.spec)->segment_width);
		  (yyval.spec)->dir = current_direction;
		}
#line 2869 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 82:
#line 671 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(SPLINE_OBJECT);
		  lookup_variable("lineht", & (yyval.spec)->segment_height);
		  lookup_variable("linewid", & (yyval.spec)->segment_width);
		  (yyval.spec)->dir = current_direction;
		}
#line 2880 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 83:
#line 678 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(TEXT_OBJECT);
		  (yyval.spec)->text = new text_item((yyvsp[0].lstr).str, (yyvsp[0].lstr).filename, (yyvsp[0].lstr).lineno);
		}
#line 2889 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 84:
#line 683 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(TEXT_OBJECT);
		  (yyval.spec)->text = new text_item(format_number(0, (yyvsp[0].x)), 0, -1);
		}
#line 2898 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 85:
#line 688 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = new object_spec(TEXT_OBJECT);
		  (yyval.spec)->text = new text_item(format_number((yyvsp[0].lstr).str, (yyvsp[-1].x)),
					   (yyvsp[0].lstr).filename, (yyvsp[0].lstr).lineno);
		  a_delete (yyvsp[0].lstr).str;
		}
#line 2909 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 86:
#line 695 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  saved_state *p = new saved_state;
		  (yyval.pstate) = p;
		  p->x = current_position.x;
		  p->y = current_position.y;
		  p->dir = current_direction;
		  p->tbl = current_table;
		  p->prev = current_saved_state;
		  current_position.x = 0.0;
		  current_position.y = 0.0;
		  current_table = new PTABLE(place);
		  current_saved_state = p;
		  olist.append(make_mark_object());
		}
#line 2928 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 87:
#line 710 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  current_position.x = (yyvsp[-2].pstate)->x;
		  current_position.y = (yyvsp[-2].pstate)->y;
		  current_direction = (yyvsp[-2].pstate)->dir;
		  (yyval.spec) = new object_spec(BLOCK_OBJECT);
		  olist.wrap_up_block(& (yyval.spec)->oblist);
		  (yyval.spec)->tbl = current_table;
		  current_table = (yyvsp[-2].pstate)->tbl;
		  current_saved_state = (yyvsp[-2].pstate)->prev;
		  delete (yyvsp[-2].pstate);
		}
#line 2944 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 88:
#line 722 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->height = (yyvsp[0].x);
		  (yyval.spec)->flags |= HAS_HEIGHT;
		}
#line 2954 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 89:
#line 728 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->radius = (yyvsp[0].x);
		  (yyval.spec)->flags |= HAS_RADIUS;
		}
#line 2964 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 90:
#line 734 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->width = (yyvsp[0].x);
		  (yyval.spec)->flags |= HAS_WIDTH;
		}
#line 2974 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 91:
#line 740 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->radius = (yyvsp[0].x)/2.0;
		  (yyval.spec)->flags |= HAS_RADIUS;
		}
#line 2984 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 92:
#line 746 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  switch ((yyval.spec)->dir) {
		  case UP_DIRECTION:
		    (yyval.spec)->segment_pos.y += (yyvsp[0].x);
		    break;
		  case DOWN_DIRECTION:
		    (yyval.spec)->segment_pos.y -= (yyvsp[0].x);
		    break;
		  case RIGHT_DIRECTION:
		    (yyval.spec)->segment_pos.x += (yyvsp[0].x);
		    break;
		  case LEFT_DIRECTION:
		    (yyval.spec)->segment_pos.x -= (yyvsp[0].x);
		    break;
		  }
		}
#line 3007 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 93:
#line 765 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->dir = UP_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.y += (yyval.spec)->segment_height;
		}
#line 3018 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 94:
#line 772 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->dir = UP_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.y += (yyvsp[0].x);
		}
#line 3029 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 95:
#line 779 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->dir = DOWN_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.y -= (yyval.spec)->segment_height;
		}
#line 3040 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 96:
#line 786 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->dir = DOWN_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.y -= (yyvsp[0].x);
		}
#line 3051 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 97:
#line 793 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->dir = RIGHT_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x += (yyval.spec)->segment_width;
		}
#line 3062 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 98:
#line 800 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->dir = RIGHT_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x += (yyvsp[0].x);
		}
#line 3073 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 99:
#line 807 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->dir = LEFT_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x -= (yyval.spec)->segment_width;
		}
#line 3084 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 100:
#line 814 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->dir = LEFT_DIRECTION;
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x -= (yyvsp[0].x);
		}
#line 3095 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 101:
#line 821 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= HAS_FROM;
		  (yyval.spec)->from.x = (yyvsp[0].pair).x;
		  (yyval.spec)->from.y = (yyvsp[0].pair).y;
		}
#line 3106 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 102:
#line 828 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  if ((yyval.spec)->flags & HAS_SEGMENT)
		    (yyval.spec)->segment_list = new segment((yyval.spec)->segment_pos,
						   (yyval.spec)->segment_is_absolute,
						   (yyval.spec)->segment_list);
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x = (yyvsp[0].pair).x;
		  (yyval.spec)->segment_pos.y = (yyvsp[0].pair).y;
		  (yyval.spec)->segment_is_absolute = 1;
		  (yyval.spec)->flags |= HAS_TO;
		  (yyval.spec)->to.x = (yyvsp[0].pair).x;
		  (yyval.spec)->to.y = (yyvsp[0].pair).y;
		}
#line 3125 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 103:
#line 843 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= HAS_AT;
		  (yyval.spec)->at.x = (yyvsp[0].pair).x;
		  (yyval.spec)->at.y = (yyvsp[0].pair).y;
		  if ((yyval.spec)->type != ARC_OBJECT) {
		    (yyval.spec)->flags |= HAS_FROM;
		    (yyval.spec)->from.x = (yyvsp[0].pair).x;
		    (yyval.spec)->from.y = (yyvsp[0].pair).y;
		  }
		}
#line 3141 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 104:
#line 855 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= HAS_WITH;
		  (yyval.spec)->with = (yyvsp[0].pth);
		}
#line 3151 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 105:
#line 861 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= HAS_WITH;
		  position pos;
		  pos.x = (yyvsp[0].pair).x;
		  pos.y = (yyvsp[0].pair).y;
		  (yyval.spec)->with = new path(pos);
		}
#line 3164 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 106:
#line 870 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x += (yyvsp[0].pair).x;
		  (yyval.spec)->segment_pos.y += (yyvsp[0].pair).y;
		}
#line 3175 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 107:
#line 877 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  if (!((yyval.spec)->flags & HAS_SEGMENT))
		    switch ((yyval.spec)->dir) {
		    case UP_DIRECTION:
		      (yyval.spec)->segment_pos.y += (yyval.spec)->segment_width;
		      break;
		    case DOWN_DIRECTION:
		      (yyval.spec)->segment_pos.y -= (yyval.spec)->segment_width;
		      break;
		    case RIGHT_DIRECTION:
		      (yyval.spec)->segment_pos.x += (yyval.spec)->segment_width;
		      break;
		    case LEFT_DIRECTION:
		      (yyval.spec)->segment_pos.x -= (yyval.spec)->segment_width;
		      break;
		    }
		  (yyval.spec)->segment_list = new segment((yyval.spec)->segment_pos,
						 (yyval.spec)->segment_is_absolute,
						 (yyval.spec)->segment_list);
		  (yyval.spec)->flags &= ~HAS_SEGMENT;
		  (yyval.spec)->segment_pos.x = (yyval.spec)->segment_pos.y = 0.0;
		  (yyval.spec)->segment_is_absolute = 0;
		}
#line 3204 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 108:
#line 902 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);	// nothing
		}
#line 3212 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 109:
#line 906 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_DOTTED;
		  lookup_variable("dashwid", & (yyval.spec)->dash_width);
		}
#line 3222 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 110:
#line 912 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= IS_DOTTED;
		  (yyval.spec)->dash_width = (yyvsp[0].x);
		}
#line 3232 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 111:
#line 918 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_DASHED;
		  lookup_variable("dashwid", & (yyval.spec)->dash_width);
		}
#line 3242 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 112:
#line 924 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= IS_DASHED;
		  (yyval.spec)->dash_width = (yyvsp[0].x);
		}
#line 3252 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 113:
#line 930 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_DEFAULT_FILLED;
		}
#line 3261 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 114:
#line 935 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= IS_FILLED;
		  (yyval.spec)->fill = (yyvsp[0].x);
		}
#line 3271 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 115:
#line 941 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= IS_XSLANTED;
		  (yyval.spec)->xslanted = (yyvsp[0].x);
		}
#line 3281 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 116:
#line 947 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= IS_YSLANTED;
		  (yyval.spec)->yslanted = (yyvsp[0].x);
		}
#line 3291 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 117:
#line 953 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= (IS_SHADED | IS_FILLED);
		  (yyval.spec)->shaded = new char[strlen((yyvsp[0].lstr).str)+1];
		  strcpy((yyval.spec)->shaded, (yyvsp[0].lstr).str);
		}
#line 3302 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 118:
#line 960 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= (IS_SHADED | IS_OUTLINED | IS_FILLED);
		  (yyval.spec)->shaded = new char[strlen((yyvsp[0].lstr).str)+1];
		  strcpy((yyval.spec)->shaded, (yyvsp[0].lstr).str);
		  (yyval.spec)->outlined = new char[strlen((yyvsp[0].lstr).str)+1];
		  strcpy((yyval.spec)->outlined, (yyvsp[0].lstr).str);
		}
#line 3315 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 119:
#line 969 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= IS_OUTLINED;
		  (yyval.spec)->outlined = new char[strlen((yyvsp[0].lstr).str)+1];
		  strcpy((yyval.spec)->outlined, (yyvsp[0].lstr).str);
		}
#line 3326 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 120:
#line 976 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  // line chop chop means line chop 0 chop 0
		  if ((yyval.spec)->flags & IS_DEFAULT_CHOPPED) {
		    (yyval.spec)->flags |= IS_CHOPPED;
		    (yyval.spec)->flags &= ~IS_DEFAULT_CHOPPED;
		    (yyval.spec)->start_chop = (yyval.spec)->end_chop = 0.0;
		  }
		  else if ((yyval.spec)->flags & IS_CHOPPED) {
		    (yyval.spec)->end_chop = 0.0;
		  }
		  else {
		    (yyval.spec)->flags |= IS_DEFAULT_CHOPPED;
		  }
		}
#line 3346 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 121:
#line 992 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  if ((yyval.spec)->flags & IS_DEFAULT_CHOPPED) {
		    (yyval.spec)->flags |= IS_CHOPPED;
		    (yyval.spec)->flags &= ~IS_DEFAULT_CHOPPED;
		    (yyval.spec)->start_chop = 0.0;
		    (yyval.spec)->end_chop = (yyvsp[0].x);
		  }
		  else if ((yyval.spec)->flags & IS_CHOPPED) {
		    (yyval.spec)->end_chop = (yyvsp[0].x);
		  }
		  else {
		    (yyval.spec)->start_chop = (yyval.spec)->end_chop = (yyvsp[0].x);
		    (yyval.spec)->flags |= IS_CHOPPED;
		  }
		}
#line 3367 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 122:
#line 1009 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_SAME;
		}
#line 3376 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 123:
#line 1014 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_INVISIBLE;
		}
#line 3385 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 124:
#line 1019 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= HAS_LEFT_ARROW_HEAD;
		}
#line 3394 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 125:
#line 1024 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= HAS_RIGHT_ARROW_HEAD;
		}
#line 3403 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 126:
#line 1029 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= (HAS_LEFT_ARROW_HEAD|HAS_RIGHT_ARROW_HEAD);
		}
#line 3412 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 127:
#line 1034 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_CLOCKWISE;
		}
#line 3421 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 128:
#line 1039 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags &= ~IS_CLOCKWISE;
		}
#line 3430 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 129:
#line 1044 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  text_item **p;
		  for (p = & (yyval.spec)->text; *p; p = &(*p)->next)
		    ;
		  *p = new text_item((yyvsp[0].lstr).str, (yyvsp[0].lstr).filename, (yyvsp[0].lstr).lineno);
		}
#line 3442 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 130:
#line 1052 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  if ((yyval.spec)->text) {
		    text_item *p;
		    for (p = (yyval.spec)->text; p->next; p = p->next)
		      ;
		    p->adj.h = LEFT_ADJUST;
		  }
		}
#line 3456 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 131:
#line 1062 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  if ((yyval.spec)->text) {
		    text_item *p;
		    for (p = (yyval.spec)->text; p->next; p = p->next)
		      ;
		    p->adj.h = RIGHT_ADJUST;
		  }
		}
#line 3470 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 132:
#line 1072 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  if ((yyval.spec)->text) {
		    text_item *p;
		    for (p = (yyval.spec)->text; p->next; p = p->next)
		      ;
		    p->adj.v = ABOVE_ADJUST;
		  }
		}
#line 3484 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 133:
#line 1082 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  if ((yyval.spec)->text) {
		    text_item *p;
		    for (p = (yyval.spec)->text; p->next; p = p->next)
		      ;
		    p->adj.v = BELOW_ADJUST;
		  }
		}
#line 3498 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 134:
#line 1092 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-2].spec);
		  (yyval.spec)->flags |= HAS_THICKNESS;
		  (yyval.spec)->thickness = (yyvsp[0].x);
		}
#line 3508 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 135:
#line 1098 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.spec) = (yyvsp[-1].spec);
		  (yyval.spec)->flags |= IS_ALIGNED;
		}
#line 3517 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 136:
#line 1106 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.lstr) = (yyvsp[0].lstr); }
#line 3523 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 137:
#line 1108 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.lstr).filename = (yyvsp[-2].lstr).filename;
		  (yyval.lstr).lineno = (yyvsp[-2].lstr).lineno;
		  (yyval.lstr).str = do_sprintf((yyvsp[-2].lstr).str, (yyvsp[-1].dv).v, (yyvsp[-1].dv).nv);
		  a_delete (yyvsp[-1].dv).v;
		  free((yyvsp[-2].lstr).str);
		}
#line 3535 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 138:
#line 1119 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.dv).v = 0;
		  (yyval.dv).nv = 0;
		  (yyval.dv).maxv = 0;
		}
#line 3545 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 139:
#line 1125 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.dv) = (yyvsp[-2].dv);
		  if ((yyval.dv).nv >= (yyval.dv).maxv) {
		    if ((yyval.dv).nv == 0) {
		      (yyval.dv).v = new double[4];
		      (yyval.dv).maxv = 4;
		    }
		    else {
		      double *oldv = (yyval.dv).v;
		      (yyval.dv).maxv *= 2;
#if 0
		      (yyval.dv).v = new double[(yyval.dv).maxv];
		      memcpy((yyval.dv).v, oldv, (yyval.dv).nv*sizeof(double));
#else
		      // workaround for bug in Compaq C++ V6.5-033
		      // for Compaq Tru64 UNIX V5.1A (Rev. 1885)
		      double *foo = new double[(yyval.dv).maxv];
		      memcpy(foo, oldv, (yyval.dv).nv*sizeof(double));
		      (yyval.dv).v = foo;
#endif
		      a_delete oldv;
		    }
		  }
		  (yyval.dv).v[(yyval.dv).nv] = (yyvsp[0].x);
		  (yyval.dv).nv += 1;
		}
#line 3576 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 140:
#line 1155 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pair) = (yyvsp[0].pair); }
#line 3582 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 141:
#line 1157 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  position pos = (yyvsp[0].pl);
		  (yyval.pair).x = pos.x;
		  (yyval.pair).y = pos.y;
		}
#line 3592 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 142:
#line 1163 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  position pos = (yyvsp[-1].pl);
		  (yyval.pair).x = pos.x;
		  (yyval.pair).y = pos.y;
		}
#line 3602 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 143:
#line 1172 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pair) = (yyvsp[0].pair); }
#line 3608 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 144:
#line 1174 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (yyvsp[-2].pair).x + (yyvsp[0].pair).x;
		  (yyval.pair).y = (yyvsp[-2].pair).y + (yyvsp[0].pair).y;
		}
#line 3617 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 145:
#line 1179 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (yyvsp[-3].pair).x + (yyvsp[-1].pair).x;
		  (yyval.pair).y = (yyvsp[-3].pair).y + (yyvsp[-1].pair).y;
		}
#line 3626 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 146:
#line 1184 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (yyvsp[-2].pair).x - (yyvsp[0].pair).x;
		  (yyval.pair).y = (yyvsp[-2].pair).y - (yyvsp[0].pair).y;
		}
#line 3635 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 147:
#line 1189 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (yyvsp[-3].pair).x - (yyvsp[-1].pair).x;
		  (yyval.pair).y = (yyvsp[-3].pair).y - (yyvsp[-1].pair).y;
		}
#line 3644 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 148:
#line 1194 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (yyvsp[-3].pair).x;
		  (yyval.pair).y = (yyvsp[-1].pair).y;
		}
#line 3653 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 149:
#line 1199 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (1.0 - (yyvsp[-4].x))*(yyvsp[-2].pair).x + (yyvsp[-4].x)*(yyvsp[0].pair).x;
		  (yyval.pair).y = (1.0 - (yyvsp[-4].x))*(yyvsp[-2].pair).y + (yyvsp[-4].x)*(yyvsp[0].pair).y;
		}
#line 3662 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 150:
#line 1204 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (1.0 - (yyvsp[-5].x))*(yyvsp[-3].pair).x + (yyvsp[-5].x)*(yyvsp[-1].pair).x;
		  (yyval.pair).y = (1.0 - (yyvsp[-5].x))*(yyvsp[-3].pair).y + (yyvsp[-5].x)*(yyvsp[-1].pair).y;
		}
#line 3671 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 151:
#line 1210 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (1.0 - (yyvsp[-5].x))*(yyvsp[-3].pair).x + (yyvsp[-5].x)*(yyvsp[-1].pair).x;
		  (yyval.pair).y = (1.0 - (yyvsp[-5].x))*(yyvsp[-3].pair).y + (yyvsp[-5].x)*(yyvsp[-1].pair).y;
		}
#line 3680 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 152:
#line 1215 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (1.0 - (yyvsp[-6].x))*(yyvsp[-4].pair).x + (yyvsp[-6].x)*(yyvsp[-2].pair).x;
		  (yyval.pair).y = (1.0 - (yyvsp[-6].x))*(yyvsp[-4].pair).y + (yyvsp[-6].x)*(yyvsp[-2].pair).y;
		}
#line 3689 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 155:
#line 1228 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pair).x = (yyvsp[-2].x);
		  (yyval.pair).y = (yyvsp[0].x);
		}
#line 3698 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 156:
#line 1233 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pair) = (yyvsp[-1].pair); }
#line 3704 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 157:
#line 1239 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pl) = (yyvsp[0].pl); }
#line 3710 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 158:
#line 1241 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  path pth((yyvsp[0].crn));
		  if (!pth.follow((yyvsp[-1].pl), & (yyval.pl)))
		    YYABORT;
		}
#line 3720 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 159:
#line 1247 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  path pth((yyvsp[-1].crn));
		  if (!pth.follow((yyvsp[0].pl), & (yyval.pl)))
		    YYABORT;
		}
#line 3730 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 160:
#line 1253 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  path pth((yyvsp[-2].crn));
		  if (!pth.follow((yyvsp[0].pl), & (yyval.pl)))
		    YYABORT;
		}
#line 3740 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 161:
#line 1259 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pl).x = current_position.x;
		  (yyval.pl).y = current_position.y;
		  (yyval.pl).obj = 0;
		}
#line 3750 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 162:
#line 1268 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  place *p = lookup_label((yyvsp[0].str));
		  if (!p) {
		    lex_error("there is no place '%1'", (yyvsp[0].str));
		    YYABORT;
		  }
		  (yyval.pl) = *p;
		  free((yyvsp[0].str));
		}
#line 3764 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 163:
#line 1278 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pl).obj = (yyvsp[0].obj); }
#line 3770 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 164:
#line 1280 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  path pth((yyvsp[0].str));
		  if (!pth.follow((yyvsp[-2].pl), & (yyval.pl)))
		    YYABORT;
		}
#line 3780 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 165:
#line 1289 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.n) = (yyvsp[0].n); }
#line 3786 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 166:
#line 1291 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  // XXX Check for overflow (and non-integers?).
		  (yyval.n) = (int)(yyvsp[-1].x);
		}
#line 3795 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 167:
#line 1299 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.n) = 1; }
#line 3801 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 168:
#line 1301 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.n) = (yyvsp[-1].n); }
#line 3807 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 169:
#line 1306 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  int count = 0;
		  object *p;
		  for (p = olist.head; p != 0; p = p->next)
		    if (p->type() == (yyvsp[0].obtype) && ++count == (yyvsp[-1].n)) {
		      (yyval.obj) = p;
		      break;
		    }
		  if (p == 0) {
		    lex_error("there is no %1%2 %3", (yyvsp[-1].n), ordinal_postfix((yyvsp[-1].n)),
			      object_type_name((yyvsp[0].obtype)));
		    YYABORT;
		  }
		}
#line 3826 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 170:
#line 1321 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  int count = 0;
		  object *p;
		  for (p = olist.tail; p != 0; p = p->prev)
		    if (p->type() == (yyvsp[0].obtype) && ++count == (yyvsp[-1].n)) {
		      (yyval.obj) = p;
		      break;
		    }
		  if (p == 0) {
		    lex_error("there is no %1%2 last %3", (yyvsp[-1].n),
			      ordinal_postfix((yyvsp[-1].n)), object_type_name((yyvsp[0].obtype)));
		    YYABORT;
		  }
		}
#line 3845 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 171:
#line 1339 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = BOX_OBJECT; }
#line 3851 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 172:
#line 1341 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = CIRCLE_OBJECT; }
#line 3857 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 173:
#line 1343 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = ELLIPSE_OBJECT; }
#line 3863 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 174:
#line 1345 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = ARC_OBJECT; }
#line 3869 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 175:
#line 1347 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = LINE_OBJECT; }
#line 3875 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 176:
#line 1349 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = ARROW_OBJECT; }
#line 3881 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 177:
#line 1351 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = SPLINE_OBJECT; }
#line 3887 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 178:
#line 1353 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = BLOCK_OBJECT; }
#line 3893 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 179:
#line 1355 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.obtype) = TEXT_OBJECT; }
#line 3899 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 180:
#line 1360 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pth) = new path((yyvsp[0].str)); }
#line 3905 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 181:
#line 1362 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pth) = (yyvsp[-2].pth);
		  (yyval.pth)->append((yyvsp[0].str));
		}
#line 3914 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 182:
#line 1370 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pth) = new path((yyvsp[0].crn)); }
#line 3920 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 183:
#line 1374 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pth) = (yyvsp[0].pth); }
#line 3926 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 184:
#line 1376 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pth) = (yyvsp[-1].pth);
		  (yyval.pth)->append((yyvsp[0].crn));
		}
#line 3935 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 185:
#line 1384 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.pth) = (yyvsp[0].pth); }
#line 3941 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 186:
#line 1386 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.pth) = (yyvsp[-3].pth);
		  (yyval.pth)->set_ypath((yyvsp[-1].pth));
		}
#line 3950 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 187:
#line 1392 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  lex_warning("'%1%2 last %3' in 'with' argument ignored",
			      (yyvsp[-3].n), ordinal_postfix((yyvsp[-3].n)), object_type_name((yyvsp[-1].obtype)));
		  (yyval.pth) = (yyvsp[0].pth);
		}
#line 3960 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 188:
#line 1398 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  lex_warning("'last %1' in 'with' argument ignored",
			      object_type_name((yyvsp[-1].obtype)));
		  (yyval.pth) = (yyvsp[0].pth);
		}
#line 3970 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 189:
#line 1404 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  lex_warning("'%1%2 %3' in 'with' argument ignored",
			      (yyvsp[-2].n), ordinal_postfix((yyvsp[-2].n)), object_type_name((yyvsp[-1].obtype)));
		  (yyval.pth) = (yyvsp[0].pth);
		}
#line 3980 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 190:
#line 1410 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  lex_warning("initial '%1' in 'with' argument ignored", (yyvsp[-1].str));
		  a_delete (yyvsp[-1].str);
		  (yyval.pth) = (yyvsp[0].pth);
		}
#line 3990 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 191:
#line 1419 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north; }
#line 3996 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 192:
#line 1421 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::east; }
#line 4002 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 193:
#line 1423 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::west; }
#line 4008 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 194:
#line 1425 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south; }
#line 4014 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 195:
#line 1427 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north_east; }
#line 4020 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 196:
#line 1429 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object:: south_east; }
#line 4026 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 197:
#line 1431 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north_west; }
#line 4032 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 198:
#line 1433 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south_west; }
#line 4038 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 199:
#line 1435 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::center; }
#line 4044 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 200:
#line 1437 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::start; }
#line 4050 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 201:
#line 1439 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::end; }
#line 4056 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 202:
#line 1441 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north; }
#line 4062 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 203:
#line 1443 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south; }
#line 4068 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 204:
#line 1445 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::west; }
#line 4074 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 205:
#line 1447 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::east; }
#line 4080 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 206:
#line 1449 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north_west; }
#line 4086 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 207:
#line 1451 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south_west; }
#line 4092 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 208:
#line 1453 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north_east; }
#line 4098 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 209:
#line 1455 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south_east; }
#line 4104 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 210:
#line 1457 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::west; }
#line 4110 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 211:
#line 1459 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::east; }
#line 4116 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 212:
#line 1461 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north_west; }
#line 4122 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 213:
#line 1463 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south_west; }
#line 4128 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 214:
#line 1465 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north_east; }
#line 4134 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 215:
#line 1467 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south_east; }
#line 4140 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 216:
#line 1469 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::north; }
#line 4146 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 217:
#line 1471 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::south; }
#line 4152 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 218:
#line 1473 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::east; }
#line 4158 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 219:
#line 1475 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::west; }
#line 4164 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 220:
#line 1477 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::center; }
#line 4170 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 221:
#line 1479 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::start; }
#line 4176 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 222:
#line 1481 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.crn) = &object::end; }
#line 4182 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 223:
#line 1486 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[0].x); }
#line 4188 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 224:
#line 1488 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[0].x); }
#line 4194 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 225:
#line 1493 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) < (yyvsp[0].x)); }
#line 4200 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 226:
#line 1498 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if (!lookup_variable((yyvsp[0].str), & (yyval.x))) {
		    lex_error("there is no variable '%1'", (yyvsp[0].str));
		    YYABORT;
		  }
		  free((yyvsp[0].str));
		}
#line 4212 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 227:
#line 1506 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[0].x); }
#line 4218 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 228:
#line 1508 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[-1].pl).obj != 0)
		    (yyval.x) = (yyvsp[-1].pl).obj->origin().x;
		  else
		    (yyval.x) = (yyvsp[-1].pl).x;
		}
#line 4229 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 229:
#line 1515 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[-1].pl).obj != 0)
		    (yyval.x) = (yyvsp[-1].pl).obj->origin().y;
		  else
		    (yyval.x) = (yyvsp[-1].pl).y;
		}
#line 4240 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 230:
#line 1522 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[-1].pl).obj != 0)
		    (yyval.x) = (yyvsp[-1].pl).obj->height();
		  else
		    (yyval.x) = 0.0;
		}
#line 4251 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 231:
#line 1529 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[-1].pl).obj != 0)
		    (yyval.x) = (yyvsp[-1].pl).obj->width();
		  else
		    (yyval.x) = 0.0;
		}
#line 4262 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 232:
#line 1536 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[-1].pl).obj != 0)
		    (yyval.x) = (yyvsp[-1].pl).obj->radius();
		  else
		    (yyval.x) = 0.0;
		}
#line 4273 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 233:
#line 1543 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-2].x) + (yyvsp[0].x); }
#line 4279 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 234:
#line 1545 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-2].x) - (yyvsp[0].x); }
#line 4285 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 235:
#line 1547 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-2].x) * (yyvsp[0].x); }
#line 4291 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 236:
#line 1549 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[0].x) == 0.0) {
		    lex_error("division by zero");
		    YYABORT;
		  }
		  (yyval.x) = (yyvsp[-2].x)/(yyvsp[0].x);
		}
#line 4303 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 237:
#line 1557 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  if ((yyvsp[0].x) == 0.0) {
		    lex_error("modulus by zero");
		    YYABORT;
		  }
		  (yyval.x) = fmod((yyvsp[-2].x), (yyvsp[0].x));
		}
#line 4315 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 238:
#line 1565 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = pow((yyvsp[-2].x), (yyvsp[0].x));
		  if (errno == EDOM) {
		    lex_error("arguments to '^' operator out of domain");
		    YYABORT;
		  }
		  if (errno == ERANGE) {
		    lex_error("result of '^' operator out of range");
		    YYABORT;
		  }
		}
#line 4332 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 239:
#line 1578 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = -(yyvsp[0].x); }
#line 4338 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 240:
#line 1580 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-1].x); }
#line 4344 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 241:
#line 1582 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = sin((yyvsp[-1].x));
		  if (errno == ERANGE) {
		    lex_error("sin result out of range");
		    YYABORT;
		  }
		}
#line 4357 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 242:
#line 1591 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = cos((yyvsp[-1].x));
		  if (errno == ERANGE) {
		    lex_error("cos result out of range");
		    YYABORT;
		  }
		}
#line 4370 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 243:
#line 1600 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = atan2((yyvsp[-3].x), (yyvsp[-1].x));
		  if (errno == EDOM) {
		    lex_error("atan2 argument out of domain");
		    YYABORT;
		  }
		  if (errno == ERANGE) {
		    lex_error("atan2 result out of range");
		    YYABORT;
		  }
		}
#line 4387 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 244:
#line 1613 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = log10((yyvsp[-1].x));
		  if (errno == ERANGE) {
		    lex_error("log result out of range");
		    YYABORT;
		  }
		}
#line 4400 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 245:
#line 1622 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = pow(10.0, (yyvsp[-1].x));
		  if (errno == ERANGE) {
		    lex_error("exp result out of range");
		    YYABORT;
		  }
		}
#line 4413 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 246:
#line 1631 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  errno = 0;
		  (yyval.x) = sqrt((yyvsp[-1].x));
		  if (errno == EDOM) {
		    lex_error("sqrt argument out of domain");
		    YYABORT;
		  }
		}
#line 4426 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 247:
#line 1640 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-3].x) > (yyvsp[-1].x) ? (yyvsp[-3].x) : (yyvsp[-1].x); }
#line 4432 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 248:
#line 1642 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-3].x) < (yyvsp[-1].x) ? (yyvsp[-3].x) : (yyvsp[-1].x); }
#line 4438 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 249:
#line 1644 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = (yyvsp[-1].x) < 0 ? -floor(-(yyvsp[-1].x)) : floor((yyvsp[-1].x)); }
#line 4444 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 250:
#line 1646 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = 1.0 + floor(((rand()&0x7fff)/double(0x7fff))*(yyvsp[-1].x)); }
#line 4450 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 251:
#line 1648 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  /* return a random number in the range [0,1) */
		  /* portable, but not very random */
		  (yyval.x) = (rand() & 0x7fff) / double(0x8000);
		}
#line 4460 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 252:
#line 1654 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    {
		  (yyval.x) = 0;
		  srand((unsigned int)(yyvsp[-1].x));
		}
#line 4469 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 253:
#line 1659 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) <= (yyvsp[0].x)); }
#line 4475 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 254:
#line 1661 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) > (yyvsp[0].x)); }
#line 4481 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 255:
#line 1663 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) >= (yyvsp[0].x)); }
#line 4487 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 256:
#line 1665 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) == (yyvsp[0].x)); }
#line 4493 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 257:
#line 1667 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != (yyvsp[0].x)); }
#line 4499 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 258:
#line 1669 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 && (yyvsp[0].x) != 0.0); }
#line 4505 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 259:
#line 1671 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[-2].x) != 0.0 || (yyvsp[0].x) != 0.0); }
#line 4511 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;

  case 260:
#line 1673 "../src/preproc/pic/pic.ypp" /* yacc.c:1645  */
    { (yyval.x) = ((yyvsp[0].x) == 0.0); }
#line 4517 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
    break;


#line 4521 "src/preproc/pic/pic.cpp" /* yacc.c:1645  */
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

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

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
#line 1677 "../src/preproc/pic/pic.ypp" /* yacc.c:1903  */


/* bison defines const to be empty unless __STDC__ is defined, which it
isn't under cfront */

#ifdef const
#undef const
#endif

static struct {
  const char *name;
  double val;
  int scaled;		     // non-zero if val should be multiplied by scale
} defaults_table[] = {
  { "arcrad", .25, 1 },
  { "arrowht", .1, 1 },
  { "arrowwid", .05, 1 },
  { "circlerad", .25, 1 },
  { "boxht", .5, 1 },
  { "boxwid", .75, 1 },
  { "boxrad", 0.0, 1 },
  { "dashwid", .05, 1 },
  { "ellipseht", .5, 1 },
  { "ellipsewid", .75, 1 },
  { "moveht", .5, 1 },
  { "movewid", .5, 1 },
  { "lineht", .5, 1 },
  { "linewid", .5, 1 },
  { "textht", 0.0, 1 },
  { "textwid", 0.0, 1 },
  { "scale", 1.0, 0 },
  { "linethick", -1.0, 0 },		// in points
  { "fillval", .5, 0 },
  { "arrowhead", 1.0, 0 },
  { "maxpswid", 8.5, 0 },
  { "maxpsht", 11.0, 0 },
};

place *lookup_label(const char *label)
{
  saved_state *state = current_saved_state;
  PTABLE(place) *tbl = current_table;
  for (;;) {
    place *pl = tbl->lookup(label);
    if (pl)
      return pl;
    if (!state)
      return 0;
    tbl = state->tbl;
    state = state->prev;
  }
}

void define_label(const char *label, const place *pl)
{
  place *p = new place[1];
  *p = *pl;
  current_table->define(label, p);
}

int lookup_variable(const char *name, double *val)
{
  place *pl = lookup_label(name);
  if (pl) {
    *val = pl->x;
    return 1;
  }
  return 0;
}

void define_variable(const char *name, double val)
{
  place *p = new place[1];
  p->obj = 0;
  p->x = val;
  p->y = 0.0;
  current_table->define(name, p);
  if (strcmp(name, "scale") == 0) {
    // When the scale changes, reset all scaled pre-defined variables to
    // their default values.
    for (unsigned int i = 0;
	 i < sizeof(defaults_table)/sizeof(defaults_table[0]); i++) 
      if (defaults_table[i].scaled)
	define_variable(defaults_table[i].name, val*defaults_table[i].val);
  }
}

// called once only (not once per parse)

void parse_init()
{
  current_direction = RIGHT_DIRECTION;
  current_position.x = 0.0;
  current_position.y = 0.0;
  // This resets everything to its default value.
  reset_all();
}

void reset(const char *nm)
{
  for (unsigned int i = 0;
       i < sizeof(defaults_table)/sizeof(defaults_table[0]); i++)
    if (strcmp(nm, defaults_table[i].name) == 0) {
      double val = defaults_table[i].val;
      if (defaults_table[i].scaled) {
	double scale;
	lookup_variable("scale", &scale);
	val *= scale;
      }
      define_variable(defaults_table[i].name, val);
      return;
    }
  lex_error("'%1' is not a predefined variable", nm);
}

void reset_all()
{
  // We only have to explicitly reset the pre-defined variables that
  // aren't scaled because 'scale' is not scaled, and changing the
  // value of 'scale' will reset all the pre-defined variables that
  // are scaled.
  for (unsigned int i = 0;
       i < sizeof(defaults_table)/sizeof(defaults_table[0]); i++)
    if (!defaults_table[i].scaled)
      define_variable(defaults_table[i].name, defaults_table[i].val);
}

// called after each parse

void parse_cleanup()
{
  while (current_saved_state != 0) {
    delete current_table;
    current_table = current_saved_state->tbl;
    saved_state *tem = current_saved_state;
    current_saved_state = current_saved_state->prev;
    delete tem;
  }
  assert(current_table == &top_table);
  PTABLE_ITERATOR(place) iter(current_table);
  const char *key;
  place *pl;
  while (iter.next(&key, &pl))
    if (pl->obj != 0) {
      position pos = pl->obj->origin();
      pl->obj = 0;
      pl->x = pos.x;
      pl->y = pos.y;
    }
  while (olist.head != 0) {
    object *tem = olist.head;
    olist.head = olist.head->next;
    delete tem;
  }
  olist.tail = 0;
  current_direction = RIGHT_DIRECTION;
  current_position.x = 0.0;
  current_position.y = 0.0;
}

const char *ordinal_postfix(int n)
{
  if (n < 10 || n > 20)
    switch (n % 10) {
    case 1:
      return "st";
    case 2:
      return "nd";
    case 3:
      return "rd";
    }
  return "th";
}

const char *object_type_name(object_type type)
{
  switch (type) {
  case BOX_OBJECT:
    return "box";
  case CIRCLE_OBJECT:
    return "circle";
  case ELLIPSE_OBJECT:
    return "ellipse";
  case ARC_OBJECT:
    return "arc";
  case SPLINE_OBJECT:
    return "spline";
  case LINE_OBJECT:
    return "line";
  case ARROW_OBJECT:
    return "arrow";
  case MOVE_OBJECT:
    return "move";
  case TEXT_OBJECT:
    return "\"\"";
  case BLOCK_OBJECT:
    return "[]";
  case OTHER_OBJECT:
  case MARK_OBJECT:
  default:
    break;
  }
  return "object";
}

static char sprintf_buf[1024];

char *format_number(const char *form, double n)
{
  if (form == 0)
    form = "%g";
  return do_sprintf(form, &n, 1);
}

char *do_sprintf(const char *form, const double *v, int nv)
{
  string result;
  int i = 0;
  string one_format;
  while (*form) {
    if (*form == '%') {
      one_format += *form++;
      for (; *form != '\0' && strchr("#-+ 0123456789.", *form) != 0; form++)
	one_format += *form;
      if (*form == '\0' || strchr("eEfgG%", *form) == 0) {
	lex_error("bad sprintf format");
	result += one_format;
	result += form;
	break;
      }
      if (*form == '%') {
	one_format += *form++;
	one_format += '\0';
	snprintf(sprintf_buf, sizeof(sprintf_buf),
		 "%s", one_format.contents());
      }
      else {
	if (i >= nv) {
	  lex_error("too few arguments to snprintf");
	  result += one_format;
	  result += form;
	  break;
	}
	one_format += *form++;
	one_format += '\0';
	snprintf(sprintf_buf, sizeof(sprintf_buf),
		 one_format.contents(), v[i++]);
      }
      one_format.clear();
      result += sprintf_buf;
    }
    else
      result += *form++;
  }
  result += '\0';
  return strsave(result.contents());
}
