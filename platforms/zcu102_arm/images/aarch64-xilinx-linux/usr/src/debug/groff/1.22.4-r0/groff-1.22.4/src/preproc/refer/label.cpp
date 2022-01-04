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
#line 20 "../src/preproc/refer/label.ypp" /* yacc.c:338  */


#include "refer.h"
#include "refid.h"
#include "ref.h"
#include "token.h"

int yylex();
void yyerror(const char *);
int yyparse();

static const char *format_serial(char c, int n);

struct label_info {
  int start;
  int length;
  int count;
  int total;
  label_info(const string &);
};

label_info *lookup_label(const string &label);

struct expression {
  enum {
    // Does the tentative label depend on the reference?
    CONTAINS_VARIABLE = 01, 
    CONTAINS_STAR = 02,
    CONTAINS_FORMAT = 04,
    CONTAINS_AT = 010
  };
  virtual ~expression() { }
  virtual void evaluate(int, const reference &, string &,
			substring_position &) = 0;
  virtual unsigned analyze() { return 0; }
};

class at_expr : public expression {
public:
  at_expr() { }
  void evaluate(int, const reference &, string &, substring_position &);
  unsigned analyze() { return CONTAINS_VARIABLE|CONTAINS_AT; }
};

class format_expr : public expression {
  char type;
  int width;
  int first_number;
public:
  format_expr(char c, int w = 0, int f = 1)
    : type(c), width(w), first_number(f) { }
  void evaluate(int, const reference &, string &, substring_position &);
  unsigned analyze() { return CONTAINS_FORMAT; }
};

class field_expr : public expression {
  int number;
  char name;
public:
  field_expr(char nm, int num) : number(num), name(nm) { }
  void evaluate(int, const reference &, string &, substring_position &);
  unsigned analyze() { return CONTAINS_VARIABLE; }
};

class literal_expr : public expression {
  string s;
public:
  literal_expr(const char *ptr, int len) : s(ptr, len) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

class unary_expr : public expression {
protected:
  expression *expr;
public:
  unary_expr(expression *e) : expr(e) { }
  ~unary_expr() { delete expr; }
  void evaluate(int, const reference &, string &, substring_position &) = 0;
  unsigned analyze() { return expr ? expr->analyze() : 0; }
};

// This caches the analysis of an expression.

class analyzed_expr : public unary_expr {
  unsigned flags;
public:
  analyzed_expr(expression *);
  void evaluate(int, const reference &, string &, substring_position &);
  unsigned analyze() { return flags; }
};

class star_expr : public unary_expr {
public:
  star_expr(expression *e) : unary_expr(e) { }
  void evaluate(int, const reference &, string &, substring_position &);
  unsigned analyze() {
    return ((expr ? (expr->analyze() & ~CONTAINS_VARIABLE) : 0)
	    | CONTAINS_STAR);
  }
};

typedef void map_func(const char *, const char *, string &);

class map_expr : public unary_expr {
  map_func *func;
public:
  map_expr(expression *e, map_func *f) : unary_expr(e), func(f) { }
  void evaluate(int, const reference &, string &, substring_position &);
};
  
typedef const char *extractor_func(const char *, const char *, const char **);

class extractor_expr : public unary_expr {
  int part;
  extractor_func *func;
public:
  enum { BEFORE = +1, MATCH = 0, AFTER = -1 };
  extractor_expr(expression *e, extractor_func *f, int pt)
    : unary_expr(e), part(pt), func(f) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

class truncate_expr : public unary_expr {
  int n;
public:
  truncate_expr(expression *e, int i) : unary_expr(e), n(i) { } 
  void evaluate(int, const reference &, string &, substring_position &);
};

class separator_expr : public unary_expr {
public:
  separator_expr(expression *e) : unary_expr(e) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

class binary_expr : public expression {
protected:
  expression *expr1;
  expression *expr2;
public:
  binary_expr(expression *e1, expression *e2) : expr1(e1), expr2(e2) { }
  ~binary_expr() { delete expr1; delete expr2; }
  void evaluate(int, const reference &, string &, substring_position &) = 0;
  unsigned analyze() {
    return (expr1 ? expr1->analyze() : 0) | (expr2 ? expr2->analyze() : 0);
  }
};

class alternative_expr : public binary_expr {
public:
  alternative_expr(expression *e1, expression *e2) : binary_expr(e1, e2) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

class list_expr : public binary_expr {
public:
  list_expr(expression *e1, expression *e2) : binary_expr(e1, e2) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

class substitute_expr : public binary_expr {
public:
  substitute_expr(expression *e1, expression *e2) : binary_expr(e1, e2) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

class ternary_expr : public expression {
protected:
  expression *expr1;
  expression *expr2;
  expression *expr3;
public:
  ternary_expr(expression *e1, expression *e2, expression *e3)
    : expr1(e1), expr2(e2), expr3(e3) { }
  ~ternary_expr() { delete expr1; delete expr2; delete expr3; }
  void evaluate(int, const reference &, string &, substring_position &) = 0;
  unsigned analyze() {
    return ((expr1 ? expr1->analyze() : 0)
	    | (expr2 ? expr2->analyze() : 0)
	    | (expr3 ? expr3->analyze() : 0));
  }
};

class conditional_expr : public ternary_expr {
public:
  conditional_expr(expression *e1, expression *e2, expression *e3)
    : ternary_expr(e1, e2, e3) { }
  void evaluate(int, const reference &, string &, substring_position &);
};

static expression *parsed_label = 0;
static expression *parsed_date_label = 0;
static expression *parsed_short_label = 0;

static expression *parse_result;

string literals;


#line 269 "src/preproc/refer/label.cpp" /* yacc.c:338  */
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
#ifndef YY_YY_SRC_PREPROC_REFER_LABEL_HPP_INCLUDED
# define YY_YY_SRC_PREPROC_REFER_LABEL_HPP_INCLUDED
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
    TOKEN_LETTER = 258,
    TOKEN_LITERAL = 259,
    TOKEN_DIGIT = 260
  };
#endif
/* Tokens.  */
#define TOKEN_LETTER 258
#define TOKEN_LITERAL 259
#define TOKEN_DIGIT 260

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 220 "../src/preproc/refer/label.ypp" /* yacc.c:353  */

  int num;
  expression *expr;
  struct { int ndigits; int val; } dig;
  struct { int start; int len; } str;

#line 329 "src/preproc/refer/label.cpp" /* yacc.c:353  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_SRC_PREPROC_REFER_LABEL_HPP_INCLUDED  */



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
#define YYFINAL  21
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   39

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  21
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  12
/* YYNRULES -- Number of rules.  */
#define YYNRULES  34
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  49

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   260

#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    12,     9,     2,
      17,    18,    16,    14,     2,    15,    13,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     7,     2,
      19,     2,    20,     6,    11,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     8,     2,    10,     2,     2,     2,
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
       5
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   248,   248,   253,   255,   261,   262,   267,   269,   271,
     276,   278,   283,   285,   290,   292,   297,   299,   301,   317,
     321,   352,   354,   356,   358,   360,   366,   367,   372,   374,
     379,   381,   388,   389,   391
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TOKEN_LETTER", "TOKEN_LITERAL",
  "TOKEN_DIGIT", "'?'", "':'", "'|'", "'&'", "'~'", "'@'", "'%'", "'.'",
  "'+'", "'-'", "'*'", "'('", "')'", "'<'", "'>'", "$accept", "expr",
  "conditional", "optional_conditional", "alternative", "list",
  "substitute", "string", "optional_number", "number", "digits", "flag", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,    63,    58,   124,    38,
     126,    64,    37,    46,    43,    45,    42,    40,    41,    60,
      62
};
# endif

#define YYPACT_NINF -26

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-26)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
       2,    11,   -26,   -26,    12,     2,     2,    24,   -26,   -26,
      21,     2,    18,    -6,   -26,    26,   -26,   -26,    27,    15,
      14,   -26,     2,     2,     2,    18,     2,    -3,    11,    11,
     -26,   -26,   -26,   -26,   -26,    28,     2,     2,    -6,   -26,
     -26,    33,    26,    26,     2,    11,   -26,   -26,    26
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       5,    16,    15,    14,     0,     5,     5,     0,     6,     2,
       3,     7,    10,    12,    28,    17,    18,    30,    19,     0,
       0,     1,     5,     0,     0,    11,     0,    32,     0,     0,
      23,    29,    31,    24,    25,     0,     8,     9,    13,    33,
      34,     0,    21,    22,     0,    26,     4,    20,    27
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -26,   -26,    -7,    -4,   -26,    -1,   -11,    13,   -26,   -25,
     -26,   -26
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     7,     8,     9,    10,    11,    12,    13,    47,    15,
      18,    41
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      25,    19,    20,    42,    43,     1,     2,    27,    28,    29,
      30,    39,    40,     3,     4,    16,    14,    17,    35,     5,
      48,     6,    36,    37,    21,    25,    25,    22,    26,    23,
      24,    31,    32,    33,    34,    44,    45,    46,     0,    38
};

static const yytype_int8 yycheck[] =
{
      11,     5,     6,    28,    29,     3,     4,    13,    14,    15,
      16,    14,    15,    11,    12,     3,     5,     5,    22,    17,
      45,    19,    23,    24,     0,    36,    37,     6,    10,     8,
       9,     5,     5,    18,    20,     7,     3,    44,    -1,    26
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,    11,    12,    17,    19,    22,    23,    24,
      25,    26,    27,    28,     5,    30,     3,     5,    31,    24,
      24,     0,     6,     8,     9,    27,    10,    13,    14,    15,
      16,     5,     5,    18,    20,    24,    26,    26,    28,    14,
      15,    32,    30,    30,     7,     3,    23,    29,    30
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    21,    22,    23,    23,    24,    24,    25,    25,    25,
      26,    26,    27,    27,    28,    28,    28,    28,    28,    28,
      28,    28,    28,    28,    28,    28,    29,    29,    30,    30,
      31,    31,    32,    32,    32
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     5,     0,     1,     1,     3,     3,
       1,     2,     1,     3,     1,     1,     1,     2,     2,     2,
       5,     3,     3,     2,     3,     3,     0,     1,     1,     2,
       1,     2,     0,     1,     1
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
        case 2:
#line 249 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { parse_result = ((yyvsp[0].expr) ? new analyzed_expr((yyvsp[0].expr)) : 0); }
#line 1435 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 3:
#line 254 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 1441 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 4:
#line 256 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new conditional_expr((yyvsp[-4].expr), (yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 1447 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 5:
#line 261 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = 0; }
#line 1453 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 6:
#line 263 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 1459 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 7:
#line 268 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 1465 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 8:
#line 270 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new alternative_expr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 1471 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 9:
#line 272 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new conditional_expr((yyvsp[-2].expr), (yyvsp[0].expr), 0); }
#line 1477 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 10:
#line 277 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 1483 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 11:
#line 279 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new list_expr((yyvsp[-1].expr), (yyvsp[0].expr)); }
#line 1489 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 12:
#line 284 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = (yyvsp[0].expr); }
#line 1495 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 13:
#line 286 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new substitute_expr((yyvsp[-2].expr), (yyvsp[0].expr)); }
#line 1501 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 14:
#line 291 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new at_expr; }
#line 1507 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 15:
#line 293 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    {
		  (yyval.expr) = new literal_expr(literals.contents() + (yyvsp[0].str).start,
					(yyvsp[0].str).len);
		}
#line 1516 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 16:
#line 298 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new field_expr((yyvsp[0].num), 0); }
#line 1522 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 17:
#line 300 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new field_expr((yyvsp[-1].num), (yyvsp[0].num) - 1); }
#line 1528 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 18:
#line 302 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    {
		  switch ((yyvsp[0].num)) {
		  case 'I':
		  case 'i':
		  case 'A':
		  case 'a':
		    (yyval.expr) = new format_expr((yyvsp[0].num));
		    break;
		  default:
		    command_error("unrecognized format '%1'", char((yyvsp[0].num)));
		    (yyval.expr) = new format_expr('a');
		    break;
		  }
		}
#line 1547 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 19:
#line 318 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    {
		  (yyval.expr) = new format_expr('0', (yyvsp[0].dig).ndigits, (yyvsp[0].dig).val);
		}
#line 1555 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 20:
#line 322 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    {
		  switch ((yyvsp[-1].num)) {
		  case 'l':
		    (yyval.expr) = new map_expr((yyvsp[-4].expr), lowercase);
		    break;
		  case 'u':
		    (yyval.expr) = new map_expr((yyvsp[-4].expr), uppercase);
		    break;
		  case 'c':
		    (yyval.expr) = new map_expr((yyvsp[-4].expr), capitalize);
		    break;
		  case 'r':
		    (yyval.expr) = new map_expr((yyvsp[-4].expr), reverse_name);
		    break;
		  case 'a':
		    (yyval.expr) = new map_expr((yyvsp[-4].expr), abbreviate_name);
		    break;
		  case 'y':
		    (yyval.expr) = new extractor_expr((yyvsp[-4].expr), find_year, (yyvsp[-2].num));
		    break;
		  case 'n':
		    (yyval.expr) = new extractor_expr((yyvsp[-4].expr), find_last_name, (yyvsp[-2].num));
		    break;
		  default:
		    (yyval.expr) = (yyvsp[-4].expr);
		    command_error("unknown function '%1'", char((yyvsp[-1].num)));
		    break;
		  }
		}
#line 1589 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 21:
#line 353 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new truncate_expr((yyvsp[-2].expr), (yyvsp[0].num)); }
#line 1595 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 22:
#line 355 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new truncate_expr((yyvsp[-2].expr), -(yyvsp[0].num)); }
#line 1601 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 23:
#line 357 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new star_expr((yyvsp[-1].expr)); }
#line 1607 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 24:
#line 359 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = (yyvsp[-1].expr); }
#line 1613 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 25:
#line 361 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.expr) = new separator_expr((yyvsp[-1].expr)); }
#line 1619 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 26:
#line 366 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = -1; }
#line 1625 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 27:
#line 368 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = (yyvsp[0].num); }
#line 1631 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 28:
#line 373 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = (yyvsp[0].num); }
#line 1637 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 29:
#line 375 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = (yyvsp[-1].num)*10 + (yyvsp[0].num); }
#line 1643 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 30:
#line 380 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.dig).ndigits = 1; (yyval.dig).val = (yyvsp[0].num); }
#line 1649 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 31:
#line 382 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.dig).ndigits = (yyvsp[-1].dig).ndigits + 1; (yyval.dig).val = (yyvsp[-1].dig).val*10 + (yyvsp[0].num); }
#line 1655 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 32:
#line 388 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = 0; }
#line 1661 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 33:
#line 390 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = 1; }
#line 1667 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;

  case 34:
#line 392 "../src/preproc/refer/label.ypp" /* yacc.c:1645  */
    { (yyval.num) = -1; }
#line 1673 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
    break;


#line 1677 "src/preproc/refer/label.cpp" /* yacc.c:1645  */
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
#line 395 "../src/preproc/refer/label.ypp" /* yacc.c:1903  */


/* bison defines const to be empty unless __STDC__ is defined, which it
isn't under cfront */

#ifdef const
#undef const
#endif

const char *spec_ptr;
const char *spec_end;
const char *spec_cur;

static char uppercase_array[] = {
  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
  'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
  'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
  'Y', 'Z',
};
  
static char lowercase_array[] = {
  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h',
  'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
  'q', 'r', 's', 't', 'u', 'v', 'w', 'x',
  'y', 'z',
};

int yylex()
{
  while (spec_ptr < spec_end && csspace(*spec_ptr))
    spec_ptr++;
  spec_cur = spec_ptr;
  if (spec_ptr >= spec_end)
    return 0;
  unsigned char c = *spec_ptr++;
  if (csalpha(c)) {
    yylval.num = c;
    return TOKEN_LETTER;
  }
  if (csdigit(c)) {
    yylval.num = c - '0';
    return TOKEN_DIGIT;
  }
  if (c == '\'') {
    yylval.str.start = literals.length();
    for (; spec_ptr < spec_end; spec_ptr++) {
      if (*spec_ptr == '\'') {
	if (++spec_ptr < spec_end && *spec_ptr == '\'')
	  literals += '\'';
	else {
	  yylval.str.len = literals.length() - yylval.str.start;
	  return TOKEN_LITERAL;
	}
      }
      else
	literals += *spec_ptr;
    }
    yylval.str.len = literals.length() - yylval.str.start;
    return TOKEN_LITERAL;
  }
  return c;
}

int set_label_spec(const char *label_spec)
{
  spec_cur = spec_ptr = label_spec;
  spec_end = strchr(label_spec, '\0');
  literals.clear();
  if (yyparse())
    return 0;
  delete parsed_label;
  parsed_label = parse_result;
  return 1;
}

int set_date_label_spec(const char *label_spec)
{
  spec_cur = spec_ptr = label_spec;
  spec_end = strchr(label_spec, '\0');
  literals.clear();
  if (yyparse())
    return 0;
  delete parsed_date_label;
  parsed_date_label = parse_result;
  return 1;
}

int set_short_label_spec(const char *label_spec)
{
  spec_cur = spec_ptr = label_spec;
  spec_end = strchr(label_spec, '\0');
  literals.clear();
  if (yyparse())
    return 0;
  delete parsed_short_label;
  parsed_short_label = parse_result;
  return 1;
}

void yyerror(const char *message)
{
  if (spec_cur < spec_end)
    command_error("label specification %1 before '%2'", message, spec_cur);
  else
    command_error("label specification %1 at end of string",
		  message, spec_cur);
}

void at_expr::evaluate(int tentative, const reference &ref,
		       string &result, substring_position &)
{
  if (tentative)
    ref.canonicalize_authors(result);
  else {
    const char *end, *start = ref.get_authors(&end);
    if (start)
      result.append(start, end - start);
  }
}

void format_expr::evaluate(int tentative, const reference &ref,
			   string &result, substring_position &)
{
  if (tentative)
    return;
  const label_info *lp = ref.get_label_ptr();
  int num = lp == 0 ? ref.get_number() : lp->count;
  if (type != '0')
    result += format_serial(type, num + 1);
  else {
    const char *ptr = i_to_a(num + first_number);
    int pad = width - strlen(ptr);
    while (--pad >= 0)
      result += '0';
    result += ptr;
  }
}

static const char *format_serial(char c, int n)
{
  assert(n > 0);
  static char buf[128]; // more than enough.
  switch (c) {
  case 'i':
  case 'I':
    {
      char *p = buf;
      // troff uses z and w to represent 10000 and 5000 in Roman
      // numerals; I can find no historical basis for this usage
      const char *s = c == 'i' ? "zwmdclxvi" : "ZWMDCLXVI";
      if (n >= 40000)
	return i_to_a(n);
      while (n >= 10000) {
	*p++ = s[0];
	n -= 10000;
      }
      for (int i = 1000; i > 0; i /= 10, s += 2) {
	int m = n/i;
	n -= m*i;
	switch (m) {
	case 3:
	  *p++ = s[2];
	  /* falls through */
	case 2:
	  *p++ = s[2];
	  /* falls through */
	case 1:
	  *p++ = s[2];
	  break;
	case 4:
	  *p++ = s[2];
	  *p++ = s[1];
	  break;
	case 8:
	  *p++ = s[1];
	  *p++ = s[2];
	  *p++ = s[2];
	  *p++ = s[2];
	  break;
	case 7:
	  *p++ = s[1];
	  *p++ = s[2];
	  *p++ = s[2];
	  break;
	case 6:
	  *p++ = s[1];
	  *p++ = s[2];
	  break;
	case 5:
	  *p++ = s[1];
	  break;
	case 9:
	  *p++ = s[2];
	  *p++ = s[0];
	}
      }
      *p = 0;
      break;
    }
  case 'a':
  case 'A':
    {
      char *p = buf;
      // this is derived from troff/reg.c
      while (n > 0) {
	int d = n % 26;
	if (d == 0)
	  d = 26;
	n -= d;
	n /= 26;
	*p++ = c == 'a' ? lowercase_array[d - 1] :
			       uppercase_array[d - 1];
      }
      *p-- = 0;
      // Reverse it.
      char *q = buf;
      while (q < p) {
	char temp = *q;
	*q = *p;
	*p = temp;
	--p;
	++q;
      }
      break;
    }
  default:
    assert(0);
  }
  return buf;
}

void field_expr::evaluate(int, const reference &ref,
			  string &result, substring_position &)
{
  const char *end;
  const char *start = ref.get_field(name, &end);
  if (start) {
    start = nth_field(number, start, &end);
    if (start)
      result.append(start, end - start);
  }
}

void literal_expr::evaluate(int, const reference &,
			    string &result, substring_position &)
{
  result += s;
}

analyzed_expr::analyzed_expr(expression *e)
: unary_expr(e), flags(e ? e->analyze() : 0)
{
}

void analyzed_expr::evaluate(int tentative, const reference &ref,
			     string &result, substring_position &pos)
{
  if (expr)
    expr->evaluate(tentative, ref, result, pos);
}

void star_expr::evaluate(int tentative, const reference &ref,
			 string &result, substring_position &pos)
{
  const label_info *lp = ref.get_label_ptr();
  if (!tentative
      && (lp == 0 || lp->total > 1)
      && expr)
    expr->evaluate(tentative, ref, result, pos);
}

void separator_expr::evaluate(int tentative, const reference &ref,
			      string &result, substring_position &pos)
{
  int start_length = result.length();
  int is_first = pos.start < 0;
  if (expr)
    expr->evaluate(tentative, ref, result, pos);
  if (is_first) {
    pos.start = start_length;
    pos.length = result.length() - start_length;
  }
}

void map_expr::evaluate(int tentative, const reference &ref,
			string &result, substring_position &)
{
  if (expr) {
    string temp;
    substring_position temp_pos;
    expr->evaluate(tentative, ref, temp, temp_pos);
    (*func)(temp.contents(), temp.contents() + temp.length(), result);
  }
}

void extractor_expr::evaluate(int tentative, const reference &ref,
			      string &result, substring_position &)
{
  if (expr) {
    string temp;
    substring_position temp_pos;
    expr->evaluate(tentative, ref, temp, temp_pos);
    const char *end, *start = (*func)(temp.contents(),
				      temp.contents() + temp.length(),
				      &end);
    switch (part) {
    case BEFORE:
      if (start)
	result.append(temp.contents(), start - temp.contents());
      else
	result += temp;
      break;
    case MATCH:
      if (start)
	result.append(start, end - start);
      break;
    case AFTER:
      if (start)
	result.append(end, temp.contents() + temp.length() - end);
      break;
    default:
      assert(0);
    }
  }
}

static void first_part(int len, const char *ptr, const char *end,
			  string &result)
{
  for (;;) {
    const char *token_start = ptr;
    if (!get_token(&ptr, end))
      break;
    const token_info *ti = lookup_token(token_start, ptr);
    int counts = ti->sortify_non_empty(token_start, ptr);
    if (counts && --len < 0)
      break;
    if (counts || ti->is_accent())
      result.append(token_start, ptr - token_start);
  }
}

static void last_part(int len, const char *ptr, const char *end,
		      string &result)
{
  const char *start = ptr;
  int count = 0;
  for (;;) {
    const char *token_start = ptr;
    if (!get_token(&ptr, end))
      break;
    const token_info *ti = lookup_token(token_start, ptr);
    if (ti->sortify_non_empty(token_start, ptr))
      count++;
  }
  ptr = start;
  int skip = count - len;
  if (skip > 0) {
    for (;;) {
      const char *token_start = ptr;
      if (!get_token(&ptr, end))
	assert(0);
      const token_info *ti = lookup_token(token_start, ptr);
      if (ti->sortify_non_empty(token_start, ptr) && --skip < 0) {
	ptr = token_start;
	break;
      }
    }
  }
  first_part(len, ptr, end, result);
}

void truncate_expr::evaluate(int tentative, const reference &ref,
			     string &result, substring_position &)
{
  if (expr) {
    string temp;
    substring_position temp_pos;
    expr->evaluate(tentative, ref, temp, temp_pos);
    const char *start = temp.contents();
    const char *end = start + temp.length();
    if (n > 0)
      first_part(n, start, end, result);
    else if (n < 0)
      last_part(-n, start, end, result);
  }
}

void alternative_expr::evaluate(int tentative, const reference &ref,
				string &result, substring_position &pos)
{
  int start_length = result.length();
  if (expr1)
    expr1->evaluate(tentative, ref, result, pos);
  if (result.length() == start_length && expr2)
    expr2->evaluate(tentative, ref, result, pos);
}

void list_expr::evaluate(int tentative, const reference &ref,
			 string &result, substring_position &pos)
{
  if (expr1)
    expr1->evaluate(tentative, ref, result, pos);
  if (expr2)
    expr2->evaluate(tentative, ref, result, pos);
}

void substitute_expr::evaluate(int tentative, const reference &ref,
			       string &result, substring_position &pos)
{
  int start_length = result.length();
  if (expr1)
    expr1->evaluate(tentative, ref, result, pos);
  if (result.length() > start_length && result[result.length() - 1] == '-') {
    // ought to see if pos covers the -
    result.set_length(result.length() - 1);
    if (expr2)
      expr2->evaluate(tentative, ref, result, pos);
  }
}

void conditional_expr::evaluate(int tentative, const reference &ref,
				string &result, substring_position &pos)
{
  string temp;
  substring_position temp_pos;
  if (expr1)
    expr1->evaluate(tentative, ref, temp, temp_pos);
  if (temp.length() > 0) {
    if (expr2)
      expr2->evaluate(tentative, ref, result, pos);
  }
  else {
    if (expr3)
      expr3->evaluate(tentative, ref, result, pos);
  }
}

void reference::pre_compute_label()
{
  if (parsed_label != 0
      && (parsed_label->analyze() & expression::CONTAINS_VARIABLE)) {
    label.clear();
    substring_position temp_pos;
    parsed_label->evaluate(1, *this, label, temp_pos);
    label_ptr = lookup_label(label);
  }
}

void reference::compute_label()
{
  label.clear();
  if (parsed_label)
    parsed_label->evaluate(0, *this, label, separator_pos);
  if (short_label_flag && parsed_short_label)
    parsed_short_label->evaluate(0, *this, short_label, short_separator_pos);
  if (date_as_label) {
    string new_date;
    if (parsed_date_label) {
      substring_position temp_pos;
      parsed_date_label->evaluate(0, *this, new_date, temp_pos);
    }
    set_date(new_date);
  }
  if (label_ptr)
    label_ptr->count += 1;
}

void reference::immediate_compute_label()
{
  if (label_ptr)
    label_ptr->total = 2;	// force use of disambiguator
  compute_label();
}

int reference::merge_labels(reference **v, int n, label_type type,
			    string &result)
{
  if (abbreviate_label_ranges)
    return merge_labels_by_number(v, n, type, result);
  else
    return merge_labels_by_parts(v, n, type, result);
}

int reference::merge_labels_by_number(reference **v, int n, label_type type,
				      string &result)
{
  if (n <= 1)
    return 0;
  int num = get_number();
  // Only merge three or more labels.
  if (v[0]->get_number() != num + 1
      || v[1]->get_number() != num + 2)
    return 0;
  int i;
  for (i = 2; i < n; i++)
    if (v[i]->get_number() != num + i + 1)
      break;
  result = get_label(type);
  result += label_range_indicator;
  result += v[i - 1]->get_label(type);
  return i;
}

const substring_position &reference::get_separator_pos(label_type type) const
{
  if (type == SHORT_LABEL && short_label_flag)
    return short_separator_pos;
  else
    return separator_pos;
}

const string &reference::get_label(label_type type) const
{
  if (type == SHORT_LABEL && short_label_flag)
    return short_label; 
  else
    return label;
}

int reference::merge_labels_by_parts(reference **v, int n, label_type type,
				     string &result)
{
  if (n <= 0)
    return 0;
  const string &lb = get_label(type);
  const substring_position &sp = get_separator_pos(type);
  if (sp.start < 0
      || sp.start != v[0]->get_separator_pos(type).start 
      || memcmp(lb.contents(), v[0]->get_label(type).contents(),
		sp.start) != 0)
    return 0;
  result = lb;
  int i = 0;
  do {
    result += separate_label_second_parts;
    const substring_position &s = v[i]->get_separator_pos(type);
    int sep_end_pos = s.start + s.length;
    result.append(v[i]->get_label(type).contents() + sep_end_pos,
		  v[i]->get_label(type).length() - sep_end_pos);
  } while (++i < n
	   && sp.start == v[i]->get_separator_pos(type).start
	   && memcmp(lb.contents(), v[i]->get_label(type).contents(),
		     sp.start) == 0);
  return i;
}

string label_pool;

label_info::label_info(const string &s)
: start(label_pool.length()), length(s.length()), count(0), total(1)
{
  label_pool += s;
}

static label_info **label_table = 0;
static int label_table_size = 0;
static int label_table_used = 0;

label_info *lookup_label(const string &label)
{
  if (label_table == 0) {
    label_table = new label_info *[17];
    label_table_size = 17;
    for (int i = 0; i < 17; i++)
      label_table[i] = 0;
  }
  unsigned h = hash_string(label.contents(), label.length()) % label_table_size;
  label_info **ptr;
  for (ptr = label_table + h;
       *ptr != 0;
       (ptr == label_table)
       ? (ptr = label_table + label_table_size - 1)
       : ptr--)
    if ((*ptr)->length == label.length()
	&& memcmp(label_pool.contents() + (*ptr)->start, label.contents(),
		  label.length()) == 0) {
      (*ptr)->total += 1;
      return *ptr;
    }
  label_info *result = *ptr = new label_info(label);
  if (++label_table_used * 2 > label_table_size) {
    // Rehash the table.
    label_info **old_table = label_table;
    int old_size = label_table_size;
    label_table_size = next_size(label_table_size);
    label_table = new label_info *[label_table_size];
    int i;
    for (i = 0; i < label_table_size; i++)
      label_table[i] = 0;
    for (i = 0; i < old_size; i++)
      if (old_table[i]) {
	h = hash_string(label_pool.contents() + old_table[i]->start,
			old_table[i]->length);
	label_info **p;
	for (p = label_table + (h % label_table_size);
	     *p != 0;
	     (p == label_table)
	     ? (p = label_table + label_table_size - 1)
	     : --p)
	    ;
	*p = old_table[i];
	}
    a_delete old_table;
  }
  return result;
}

void clear_labels()
{
  for (int i = 0; i < label_table_size; i++) {
    delete label_table[i];
    label_table[i] = 0;
  }
  label_table_used = 0;
  label_pool.clear();
}

static void consider_authors(reference **start, reference **end, int i);

void compute_labels(reference **v, int n)
{
  if (parsed_label
      && (parsed_label->analyze() & expression::CONTAINS_AT)
      && sort_fields.length() >= 2
      && sort_fields[0] == 'A'
      && sort_fields[1] == '+')
    consider_authors(v, v + n, 0);
  for (int i = 0; i < n; i++)
    v[i]->compute_label();
}


/* A reference with a list of authors <A0,A1,...,AN> _needs_ author i
where 0 <= i <= N if there exists a reference with a list of authors
<B0,B1,...,BM> such that <A0,A1,...,AN> != <B0,B1,...,BM> and M >= i
and Aj = Bj for 0 <= j < i. In this case if we can't say "A0,
A1,...,A(i-1) et al" because this would match both <A0,A1,...,AN> and
<B0,B1,...,BM>.  If a reference needs author i we only have to call
need_author(j) for some j >= i such that the reference also needs
author j. */

/* This function handles 2 tasks:
determine which authors are needed (cannot be elided with et al.);
determine which authors can have only last names in the labels.

References >= start and < end have the same first i author names.
Also they're sorted by A+. */

static void consider_authors(reference **start, reference **end, int i)
{
  if (start >= end)
    return;
  reference **p = start;
  if (i >= (*p)->get_nauthors()) {
    for (++p; p < end && i >= (*p)->get_nauthors(); p++)
      ;
    if (p < end && i > 0) {
      // If we have an author list <A B C> and an author list <A B C D>,
      // then both lists need C.
      for (reference **q = start; q < end; q++)
	(*q)->need_author(i - 1);
    }
    start = p;
  }
  while (p < end) {
    reference **last_name_start = p;
    reference **name_start = p;
    for (++p;
	 p < end && i < (*p)->get_nauthors()
	 && same_author_last_name(**last_name_start, **p, i);
	 p++) {
      if (!same_author_name(**name_start, **p, i)) {
	consider_authors(name_start, p, i + 1);
	name_start = p;
      }
    }
    consider_authors(name_start, p, i + 1);
    if (last_name_start == name_start) {
      for (reference **q = last_name_start; q < p; q++)
	(*q)->set_last_name_unambiguous(i);
    }
    // If we have an author list <A B C D> and <A B C E>, then the lists
    // need author D and E respectively.
    if (name_start > start || p < end) {
      for (reference **q = last_name_start; q < p; q++)
	(*q)->need_author(i);
    }
  }
}

int same_author_last_name(const reference &r1, const reference &r2, int n)
{
  const char *ae1;
  const char *as1 = r1.get_sort_field(0, n, 0, &ae1);
  const char *ae2;
  const char *as2 = r2.get_sort_field(0, n, 0, &ae2);
  if (!as1 && !as2) return 1;	// they are the same
  if (!as1 || !as2) return 0;
  return ae1 - as1 == ae2 - as2 && memcmp(as1, as2, ae1 - as1) == 0;
}

int same_author_name(const reference &r1, const reference &r2, int n)
{
  const char *ae1;
  const char *as1 = r1.get_sort_field(0, n, -1, &ae1);
  const char *ae2;
  const char *as2 = r2.get_sort_field(0, n, -1, &ae2);
  if (!as1 && !as2) return 1;	// they are the same
  if (!as1 || !as2) return 0;
  return ae1 - as1 == ae2 - as2 && memcmp(as1, as2, ae1 - as1) == 0;
}


void int_set::set(int i)
{
  assert(i >= 0);
  int bytei = i >> 3;
  if (bytei >= v.length()) {
    int old_length = v.length();
    v.set_length(bytei + 1);
    for (int j = old_length; j <= bytei; j++)
      v[j] = 0;
  }
  v[bytei] |= 1 << (i & 7);
}

int int_set::get(int i) const
{
  assert(i >= 0);
  int bytei = i >> 3;
  return bytei >= v.length() ? 0 : (v[bytei] & (1 << (i & 7))) != 0;
}

void reference::set_last_name_unambiguous(int i)
{
  last_name_unambiguous.set(i);
}

void reference::need_author(int n)
{
  if (n > last_needed_author)
    last_needed_author = n;
}

const char *reference::get_authors(const char **end) const
{
  if (!computed_authors) {
    ((reference *)this)->computed_authors = 1;
    string &result = ((reference *)this)->authors;
    int na = get_nauthors();
    result.clear();
    for (int i = 0; i < na; i++) {
      if (last_name_unambiguous.get(i)) {
	const char *e, *start = get_author_last_name(i, &e);
	assert(start != 0);
	result.append(start, e - start);
      }
      else {
	const char *e, *start = get_author(i, &e);
	assert(start != 0);
	result.append(start, e - start);
      }
      if (i == last_needed_author
	  && et_al.length() > 0
	  && et_al_min_elide > 0
	  && last_needed_author + et_al_min_elide < na
	  && na >= et_al_min_total) {
	result += et_al;
	break;
      }
      if (i < na - 1) {
	if (na == 2)
	  result += join_authors_exactly_two;
	else if (i < na - 2)
	  result += join_authors_default;
	else
	  result += join_authors_last_two;
      }
    }
  }
  const char *start = authors.contents();
  *end = start + authors.length();
  return start;
}

int reference::get_nauthors() const
{
  if (nauthors < 0) {
    const char *dummy;
    int na;
    for (na = 0; get_author(na, &dummy) != 0; na++)
      ;
    ((reference *)this)->nauthors = na;
  }
  return nauthors;
}
