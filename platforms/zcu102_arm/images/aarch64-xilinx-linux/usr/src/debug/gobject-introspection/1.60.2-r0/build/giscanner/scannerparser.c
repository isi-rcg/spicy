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
#line 29 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <glib.h>
#include <glib/gstdio.h>
#include "sourcescanner.h"
#include "scannerparser.h"

extern FILE *yyin;
extern int lineno;
extern char linebuf[2000];
extern char *yytext;

extern int yylex (GISourceScanner *scanner);
static void yyerror (GISourceScanner *scanner, const char *str);

extern void ctype_free (GISourceType * type);

static int last_enum_value = -1;
static gboolean is_bitfield;

/**
 * parse_c_string_literal:
 * @str: A string containing a C string literal
 *
 * Based on g_strcompress(), but also handles
 * hexadecimal escapes.
 */
static char *
parse_c_string_literal (const char *str)
{
  const gchar *p = str, *num;
  gchar *dest = g_malloc (strlen (str) + 1);
  gchar *q = dest;

  while (*p)
    {
      if (*p == '\\')
        {
          p++;
          switch (*p)
            {
            case '\0':
              g_warning ("parse_c_string_literal: trailing \\");
              goto out;
            case '0':  case '1':  case '2':  case '3':  case '4':
            case '5':  case '6':  case '7':
              *q = 0;
              num = p;
              while ((p < num + 3) && (*p >= '0') && (*p <= '7'))
                {
                  *q = (*q * 8) + (*p - '0');
                  p++;
                }
              q++;
              p--;
              break;
	    case 'x':
	      *q = 0;
	      p++;
	      num = p;
	      while ((p < num + 2) && (g_ascii_isxdigit(*p)))
		{
		  *q = (*q * 16) + g_ascii_xdigit_value(*p);
		  p++;
		}
              q++;
              p--;
	      break;
            case 'b':
              *q++ = '\b';
              break;
            case 'f':
              *q++ = '\f';
              break;
            case 'n':
              *q++ = '\n';
              break;
            case 'r':
              *q++ = '\r';
              break;
            case 't':
              *q++ = '\t';
              break;
            default:            /* Also handles \" and \\ */
              *q++ = *p;
              break;
            }
        }
      else
        *q++ = *p;
      p++;
    }
out:
  *q = 0;

  return dest;
}

enum {
  IRRELEVANT = 1,
  NOT_GI_SCANNER = 2,
  FOR_GI_SCANNER = 3,
};

static void
update_skipping (GISourceScanner *scanner)
{
  GList *l;
  for (l = scanner->conditionals.head; l != NULL; l = g_list_next (l))
    {
      if (GPOINTER_TO_INT (l->data) == NOT_GI_SCANNER)
        {
           scanner->skipping = TRUE;
           return;
        }
    }

  scanner->skipping = FALSE;
}

static void
push_conditional (GISourceScanner *scanner,
                  gint type)
{
  g_assert (type != 0);
  g_queue_push_head (&scanner->conditionals, GINT_TO_POINTER (type));
}

static gint
pop_conditional (GISourceScanner *scanner)
{
  gint type = GPOINTER_TO_INT (g_queue_pop_head (&scanner->conditionals));

  if (type == 0)
    {
      gchar *filename = g_file_get_path (scanner->current_file);
      gchar *error = g_strdup_printf ("%s:%d: mismatched %s", filename, lineno, yytext);
      g_ptr_array_add (scanner->errors, error);
      g_free (filename);
    }

  return type;
}

static void
warn_if_cond_has_gi_scanner (GISourceScanner *scanner,
                             const gchar *text)
{
  /* Some other conditional that is not __GI_SCANNER__ */
  if (strstr (text, "__GI_SCANNER__"))
    {
      gchar *filename = g_file_get_path (scanner->current_file);
      gchar *error = g_strdup_printf ("%s:%d: the __GI_SCANNER__ constant should only be used with simple #ifdef or #endif: %s",
               filename, lineno, text);
      g_ptr_array_add (scanner->errors, error);
      g_free (filename);
    }
}

static void
toggle_conditional (GISourceScanner *scanner)
{
  switch (pop_conditional (scanner))
    {
    case FOR_GI_SCANNER:
      push_conditional (scanner, NOT_GI_SCANNER);
      break;
    case NOT_GI_SCANNER:
      push_conditional (scanner, FOR_GI_SCANNER);
      break;
    case 0:
      break;
    default:
      push_conditional (scanner, IRRELEVANT);
      break;
    }
}

static void
set_or_merge_base_type (GISourceType *type,
                        GISourceType *base)
{
  /* combine basic types like unsigned int and long long */
  if (base->type == CTYPE_BASIC_TYPE && type->type == CTYPE_BASIC_TYPE)
    {
      char *name = g_strdup_printf ("%s %s", type->name, base->name);
      g_free (type->name);
      type->name = name;

      type->storage_class_specifier |= base->storage_class_specifier;
      type->type_qualifier |= base->type_qualifier;
      type->function_specifier |= base->function_specifier;
      type->is_bitfield |= base->is_bitfield;

      ctype_free (base);
    }
  else if (base->type == CTYPE_INVALID)
    {
      g_assert (base->base_type == NULL);

      type->storage_class_specifier |= base->storage_class_specifier;
      type->type_qualifier |= base->type_qualifier;
      type->function_specifier |= base->function_specifier;
      type->is_bitfield |= base->is_bitfield;

      ctype_free (base);
    }
  else
    {
      g_assert (type->base_type == NULL);

      type->base_type = base;
    }
}


#line 290 "giscanner/scannerparser.c"

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
# define YYERROR_VERBOSE 1
#endif

/* Use api.header.include to #include this header
   instead of duplicating it here.  */
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

#line 420 "giscanner/scannerparser.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (GISourceScanner* scanner);

#endif /* !YY_YY_GISCANNER_SCANNERPARSER_H_INCLUDED  */



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
#define YYFINAL  74
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2430

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  100
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  79
/* YYNRULES -- Number of rules.  */
#define YYNRULES  254
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  424

#define YYUNDEFTOK  2
#define YYMAXUTOK   330

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    89,     2,     2,     2,    91,    84,     2,
      76,    77,    85,    86,    83,    87,    82,    90,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    97,    99,
      92,    98,    93,    96,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    80,     2,    81,    94,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    78,    95,    79,    88,     2,     2,     2,
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
      75
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   344,   344,   353,   369,   375,   381,   388,   389,   393,
     401,   416,   430,   437,   438,   442,   443,   447,   451,   455,
     459,   463,   467,   474,   475,   479,   480,   484,   488,   511,
     518,   525,   529,   537,   541,   545,   549,   553,   557,   564,
     565,   577,   578,   584,   592,   603,   604,   610,   619,   620,
     632,   641,   642,   648,   654,   660,   669,   670,   676,   685,
     686,   695,   696,   705,   706,   715,   716,   727,   728,   739,
     740,   747,   748,   755,   756,   757,   758,   759,   760,   761,
     762,   763,   764,   765,   769,   770,   771,   778,   784,   802,
     809,   812,   817,   822,   827,   828,   833,   838,   843,   851,
     855,   862,   863,   867,   871,   875,   879,   883,   887,   894,
     901,   905,   909,   913,   918,   919,   920,   928,   948,   953,
     961,   966,   974,   975,   982,  1002,  1007,  1008,  1013,  1021,
    1025,  1033,  1036,  1037,  1041,  1052,  1059,  1066,  1073,  1080,
    1087,  1096,  1096,  1105,  1113,  1121,  1133,  1137,  1141,  1145,
    1152,  1159,  1164,  1168,  1173,  1177,  1182,  1187,  1197,  1204,
    1213,  1218,  1222,  1233,  1246,  1247,  1254,  1258,  1265,  1270,
    1275,  1280,  1287,  1293,  1302,  1303,  1307,  1312,  1313,  1321,
    1325,  1330,  1335,  1340,  1345,  1351,  1361,  1367,  1380,  1387,
    1388,  1389,  1393,  1394,  1400,  1401,  1402,  1403,  1404,  1405,
    1409,  1410,  1411,  1415,  1416,  1420,  1421,  1425,  1426,  1430,
    1431,  1435,  1436,  1437,  1441,  1442,  1443,  1444,  1445,  1446,
    1447,  1448,  1449,  1450,  1454,  1455,  1456,  1457,  1458,  1464,
    1465,  1469,  1470,  1471,  1472,  1476,  1477,  1481,  1482,  1488,
    1495,  1502,  1506,  1523,  1528,  1533,  1538,  1543,  1548,  1555,
    1560,  1568,  1569,  1570,  1571
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "BASIC_TYPE", "\"identifier\"",
  "\"typedef-name\"", "INTEGER", "FLOATING", "BOOLEAN", "CHARACTER",
  "STRING", "INTL_CONST", "INTUL_CONST", "ELLIPSIS", "ADDEQ", "SUBEQ",
  "MULEQ", "DIVEQ", "MODEQ", "XOREQ", "ANDEQ", "OREQ", "SL", "SR", "SLEQ",
  "SREQ", "EQ", "NOTEQ", "LTEQ", "GTEQ", "ANDAND", "OROR", "PLUSPLUS",
  "MINUSMINUS", "ARROW", "AUTO", "BREAK", "CASE", "CONST", "CONTINUE",
  "DEFAULT", "DO", "ELSE", "ENUM", "EXTENSION", "EXTERN", "FOR", "GOTO",
  "IF", "INLINE", "REGISTER", "RESTRICT", "RETURN", "SHORT", "SIGNED",
  "SIZEOF", "STATIC", "STRUCT", "SWITCH", "THREAD_LOCAL", "TYPEDEF",
  "UNION", "UNSIGNED", "VOID", "VOLATILE", "WHILE", "FUNCTION_MACRO",
  "OBJECT_MACRO", "IFDEF_GI_SCANNER", "IFNDEF_GI_SCANNER", "IFDEF_COND",
  "IFNDEF_COND", "IF_COND", "ELIF_COND", "ELSE_COND", "ENDIF_COND", "'('",
  "')'", "'{'", "'}'", "'['", "']'", "'.'", "','", "'&'", "'*'", "'+'",
  "'-'", "'~'", "'!'", "'/'", "'%'", "'<'", "'>'", "'^'", "'|'", "'?'",
  "':'", "'='", "';'", "$accept", "primary_expression", "strings",
  "identifier", "identifier_or_typedef_name", "postfix_expression",
  "argument_expression_list", "unary_expression", "unary_operator",
  "cast_expression", "multiplicative_expression", "additive_expression",
  "shift_expression", "relational_expression", "equality_expression",
  "and_expression", "exclusive_or_expression", "inclusive_or_expression",
  "logical_and_expression", "logical_or_expression",
  "conditional_expression", "assignment_expression", "assignment_operator",
  "expression", "constant_expression", "declaration", "empty_declaration",
  "declaration_specifiers", "init_declarator_list", "init_declarator",
  "storage_class_specifier", "basic_type", "type_specifier",
  "struct_or_union_specifier", "struct_or_union",
  "struct_declaration_list", "struct_declaration",
  "specifier_qualifier_list", "struct_declarator_list",
  "struct_declarator", "enum_specifier", "enum_keyword", "enumerator_list",
  "$@1", "enumerator", "type_qualifier", "function_specifier",
  "declarator", "direct_declarator", "pointer", "type_qualifier_list",
  "parameter_list", "parameter_declaration", "identifier_list",
  "type_name", "abstract_declarator", "direct_abstract_declarator",
  "typedef_name", "initializer", "initializer_list", "statement",
  "labeled_statement", "compound_statement", "block_item_list",
  "block_item", "expression_statement", "selection_statement",
  "iteration_statement", "jump_statement", "translation_unit",
  "external_declaration", "function_definition", "declaration_list",
  "function_macro", "object_macro", "function_macro_define",
  "object_macro_define", "preproc_conditional", "macro", YY_NULLPTR
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
     325,   326,   327,   328,   329,   330,    40,    41,   123,   125,
      91,    93,    46,    44,    38,    42,    43,    45,   126,    33,
      47,    37,    60,    62,    94,   124,    63,    58,    61,    59
};
# endif

#define YYPACT_NINF -241

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-241)))

#define YYTABLE_NINF -15

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     529,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  -241,  -241,  -241,    30,  2366,  -241,  2366,  -241,    18,
    -241,    48,  2366,  2366,  -241,   442,  -241,  -241,    -4,  1787,
    -241,  -241,  -241,  -241,  -241,    90,   199,  -241,  -241,   -56,
    -241,  1036,    -7,    42,  -241,  -241,   408,  -241,   -50,  -241,
    -241,    -1,  -241,  -241,  -241,  -241,    78,  -241,  -241,  -241,
    -241,  -241,    19,    41,  1826,  1826,    73,  1876,  1374,  -241,
    -241,  -241,  -241,  -241,  -241,  -241,   162,  -241,   147,  -241,
    1787,  -241,    52,   171,   249,    85,   259,   121,   122,   123,
     201,    25,  -241,  -241,   170,  -241,  -241,   199,    90,  -241,
     602,  1474,  -241,    30,  -241,  2149,  2211,  1493,    -7,   408,
    2081,  -241,    34,   408,   408,    65,    78,  -241,  -241,    84,
    1826,  1826,  1915,  -241,  -241,   191,  1374,  -241,  1934,   177,
    -241,  -241,    86,   -22,   197,  -241,  -241,  -241,   288,  1532,
    1915,   288,  -241,  1787,  1787,  1787,  1787,  1787,  1787,  1787,
    1787,  1787,  1787,  1787,  1787,  1787,  1787,  1787,  1787,  1787,
    1787,  1915,  -241,  -241,  -241,  -241,   179,   181,  1787,   195,
     204,   979,   231,   288,   234,  1097,   238,   240,  -241,  -241,
     221,   222,   -51,  -241,   224,  -241,  -241,  -241,   699,  -241,
    -241,  -241,  -241,  -241,  1474,  -241,  -241,  -241,  -241,  -241,
    -241,    56,   127,  -241,   130,  -241,   241,  -241,  -241,  -241,
    1787,   -16,  -241,   228,  -241,  2118,  -241,    32,   232,  -241,
     103,  -241,    78,   252,   254,  1934,   893,   255,  1312,   250,
    -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241,
    -241,  1787,  -241,  1787,  2054,  1582,   141,  -241,   215,  1787,
    -241,  -241,   137,  -241,   -24,  -241,  -241,  -241,  -241,    52,
      52,   171,   171,   249,   249,   249,   249,    85,    85,   259,
     121,   122,   123,   201,   -48,  -241,   237,  -241,   979,   270,
    1113,   242,  1915,  -241,    63,  1915,  1915,   979,  -241,  -241,
    -241,  -241,   165,  1992,  -241,    21,  -241,  -241,  2335,  -241,
    -241,  -241,    34,  -241,  1787,  -241,  -241,  -241,  1787,  -241,
      37,  -241,  -241,  -241,   796,  -241,  -241,  -241,  -241,   142,
     262,  -241,   261,   215,  2273,  1621,  -241,  -241,  1787,  -241,
    1915,   979,  -241,   268,  1199,    72,  -241,   149,  -241,   158,
     168,  -241,  -241,  1435,  -241,  -241,  -241,  -241,  -241,   271,
    -241,  -241,  -241,  -241,   169,  -241,   264,  -241,   250,  -241,
    1915,  1640,   107,  1215,   979,   979,   979,  -241,  -241,  -241,
    -241,  -241,   172,   979,   182,  1679,  1729,   116,   305,  -241,
    -241,   256,  -241,   979,   979,   184,   979,   185,  1768,   979,
    -241,  -241,  -241,   979,  -241,   979,   979,   193,  -241,  -241,
    -241,  -241,   979,  -241
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,   254,   109,   188,   106,   146,   140,   148,   104,   150,
     107,   147,   111,   105,   120,   108,   103,   121,   112,   110,
     149,   239,   240,   243,   244,   245,   246,   247,   248,   249,
     250,    90,   232,   233,     0,    92,   113,    94,   114,     0,
     115,     0,    96,    98,   116,     0,   229,   231,     0,     0,
     251,   252,   253,   234,    12,     0,   161,    89,   153,     0,
      99,   101,   152,     0,    91,    93,     0,    13,   119,    14,
     141,   139,    95,    97,     1,   230,     0,     3,     6,     4,
       5,    10,     0,     0,     0,     0,     0,     0,     0,    33,
      34,    35,    36,    37,    38,    15,     7,     2,    25,    39,
       0,    41,    45,    48,    51,    56,    59,    61,    63,    65,
      67,    69,    87,   242,     0,   164,   163,   160,     0,    88,
       0,     0,   237,     0,   236,     0,     0,     0,   151,   126,
       0,   122,   131,   128,     0,     0,     0,   141,   172,     0,
       0,     0,     0,    26,    27,     0,     0,    31,   148,    39,
      71,    84,     0,   174,     0,    11,    21,    22,     0,     0,
       0,     0,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   154,   165,   162,   100,   101,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   203,   209,
       2,     0,     0,   207,   116,   208,   194,   195,     0,   205,
     196,   197,   198,   199,     0,   189,   102,   238,   235,   171,
     159,   170,     0,   166,     0,   156,     0,   125,   118,   123,
       0,     0,   129,   132,   127,     0,   136,     0,   144,   142,
       0,   241,     0,     0,     0,     0,     0,     0,     0,    86,
      77,    78,    74,    75,    76,    82,    81,    83,    79,    80,
      73,     0,     8,     0,     0,     0,   176,   175,   177,     0,
      20,    18,     0,    23,     0,    19,    42,    43,    44,    46,
      47,    49,    50,    54,    55,    52,    53,    57,    58,    60,
      62,    64,    66,    68,     0,   226,     0,   225,     0,     0,
       0,     0,     0,   227,     0,     0,     0,     0,   210,   204,
     206,   192,     0,     0,   168,   176,   169,   157,     0,   158,
     155,   133,   131,   124,     0,   117,   138,   143,     0,   135,
       0,   173,    29,    30,     0,    32,    72,    85,   184,     0,
       0,   180,     0,   178,     0,     0,    40,    17,     0,    16,
       0,     0,   202,     0,     0,     0,   224,     0,   228,     0,
       0,   200,   190,     0,   167,   130,   134,   145,   137,     0,
     185,   179,   181,   186,     0,   182,     0,    24,    70,   201,
       0,     0,     0,     0,     0,     0,     0,   191,   193,     9,
     187,   183,     0,     0,     0,     0,     0,     0,   211,   213,
     214,     0,   216,     0,     0,     0,     0,     0,     0,     0,
     215,   220,   218,     0,   217,     0,     0,     0,   212,   222,
     221,   219,     0,   223
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -241,  -241,  -241,   -34,   -28,  -241,  -241,   253,  -241,   -85,
     131,   144,   133,   153,   178,   180,   183,   186,   176,  -241,
     -41,  -109,  -241,   -72,  -164,    20,  -241,     2,  -241,   244,
    -241,  -241,     5,  -241,  -241,   226,  -113,   -78,  -241,    44,
    -241,  -241,   227,  -241,   235,   -26,  -241,   -33,   -59,   -53,
    -241,  -120,    45,   247,   243,  -134,  -240,    -8,  -205,  -241,
      51,  -241,     0,   128,  -194,  -241,  -241,  -241,  -241,  -241,
     322,  -241,  -241,  -241,  -241,  -241,  -241,  -241,  -241
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    95,    96,    97,   201,    98,   272,   149,   100,   101,
     102,   103,   104,   105,   106,   107,   108,   109,   110,   111,
     150,   151,   261,   202,   113,   203,    33,   123,    59,    60,
      35,    36,    37,    38,    39,   130,   131,   132,   231,   232,
      40,    41,   135,   136,   327,    42,    43,   114,    62,    63,
     117,   339,   223,   139,   154,   340,   268,    44,   216,   312,
     205,   206,   207,   208,   209,   210,   211,   212,   213,    45,
      46,    47,   125,    48,    49,    50,    51,    52,    53
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      58,    61,    34,   116,   128,    67,   222,    67,   112,   311,
     153,    68,   215,    71,   310,   162,   152,   229,   226,   267,
      32,    58,    54,     3,   296,    54,   343,   118,   134,    58,
     115,    69,   263,    69,    54,   263,    54,    64,    54,    65,
     133,    54,   138,   119,    72,    73,    54,    34,   308,   350,
     273,   227,    54,     3,   264,   234,   180,   349,   265,   263,
      54,   124,   133,    56,   184,    32,   321,   322,   153,   126,
     152,   129,    76,   127,   152,   343,   249,   137,   276,   277,
     278,   122,    54,   323,    58,   186,   200,   316,   274,    58,
     186,   183,   138,   129,    54,   140,    66,   313,    58,   233,
     266,   265,   238,   133,   133,   215,    55,   133,   133,   294,
      55,   326,   204,   170,   171,    56,   368,   141,    55,    56,
     133,   181,   229,   304,    67,   218,    70,    67,   221,    57,
     270,   230,   313,   275,   129,   129,   265,   163,   129,   129,
     310,    56,   164,   165,   236,   217,   263,   112,   237,   145,
      69,   129,   336,    69,   337,   263,   342,   200,   388,    67,
     366,   241,   358,   262,   367,   301,    55,   242,   315,   263,
     153,   383,   155,   249,   200,    56,   152,   172,   173,   156,
     157,   158,   329,    69,   346,    69,   330,    58,   314,   112,
     263,   250,   251,   252,   253,   254,   255,   256,   257,   263,
     204,   258,   259,   238,   317,   176,   395,   319,   331,   133,
     318,   266,   200,   242,   347,   408,   177,   264,   178,   370,
     348,   265,   133,   159,   374,   318,   384,   160,   355,   161,
     357,   179,   263,   359,   360,   385,   376,     5,   204,   377,
     129,   263,   299,     7,   362,   386,   390,   182,   363,   401,
      11,   263,   318,   129,   215,   263,   128,   166,   167,   403,
     315,   413,   415,    20,   200,   263,   221,   263,   263,   246,
     422,   168,   169,   200,   269,   260,   263,   121,   378,    58,
     295,    58,   382,   112,    56,   174,   175,   112,    58,   233,
      69,   344,    54,     3,   297,   345,   238,   279,   280,    69,
     200,   298,    99,   283,   284,   285,   286,   300,   392,   394,
     302,   397,   281,   282,   305,   221,   306,   200,   -13,   307,
     221,   -14,   320,   405,   407,   324,   204,   287,   288,   332,
     328,   333,   335,   263,   351,   353,   417,   143,   144,   371,
     147,   356,   372,    69,   380,   391,   221,   409,   389,   352,
     200,   200,   200,    99,   289,   410,   293,   290,   361,   200,
     235,   291,   185,   364,   240,   292,   365,    75,     0,   200,
     200,   239,   200,   224,   334,   200,    69,    69,    69,   200,
       0,   200,   200,     0,     0,    69,     0,     0,   200,   247,
       0,     0,     0,   243,   244,    69,    69,     0,    69,     0,
       0,    69,   379,     0,     0,    69,     0,    69,    69,     0,
       0,     2,     0,     3,    69,     0,    99,    99,    99,    99,
      99,    99,    99,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,    99,     0,   398,   399,   400,     0,     0,
       0,    99,    74,     1,   402,     2,     5,     3,     0,     0,
       0,     6,     7,     0,   411,   412,     0,   414,     0,    11,
     418,     0,    12,     0,   419,    14,   420,   421,     0,    17,
      18,    19,    20,   423,     0,     0,     0,     4,     0,     0,
       5,     0,     0,    99,     0,     6,     7,     8,     0,     0,
       0,     9,    10,    11,     0,     0,    12,     0,    13,    14,
       0,    15,    16,    17,    18,    19,    20,     0,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,     0,     0,
       0,     0,    99,     0,     0,     0,     0,     0,     0,     0,
       1,     0,     2,     0,     3,     0,     0,     0,     0,     0,
       0,    31,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     4,     0,     0,     5,     0,     0,
       0,     0,     6,     7,     8,     0,     0,    99,     9,    10,
      11,    99,     0,    12,     0,    13,    14,     0,    15,    16,
      17,    18,    19,    20,     0,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,     2,    54,     3,    77,    78,
      79,    80,    81,    82,    83,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    31,     0,
       0,     0,     0,     0,    84,    85,     0,     4,   187,   188,
       5,   189,   190,   191,     0,     6,   148,     8,   192,   193,
     194,     9,    10,    11,   195,     0,    12,    87,    13,    14,
     196,    15,    16,    17,    18,    19,    20,   197,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    88,     0,
     120,   198,     0,     0,     0,     0,    89,    90,    91,    92,
      93,    94,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   199,     2,    54,     3,    77,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    84,    85,     0,     4,   187,   188,     5,   189,   190,
     191,     0,     6,   148,     8,   192,   193,   194,     9,    10,
      11,   195,     0,    12,    87,    13,    14,   196,    15,    16,
      17,    18,    19,    20,   197,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    88,     0,   120,   309,     0,
       0,     0,     0,    89,    90,    91,    92,    93,    94,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   199,     2,
      54,     3,    77,    78,    79,    80,    81,    82,    83,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    84,    85,
       0,     4,   187,   188,     5,   189,   190,   191,     0,     6,
     148,     8,   192,   193,   194,     9,    10,    11,   195,     0,
      12,    87,    13,    14,   196,    15,    16,    17,    18,    19,
      20,   197,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    88,     0,   120,   369,     0,     0,     0,     0,
      89,    90,    91,    92,    93,    94,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   199,     2,    54,     3,    77,
      78,    79,    80,    81,    82,    83,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    84,    85,     0,     4,   187,
     188,     5,   189,   190,   191,     0,     6,   148,     8,   192,
     193,   194,     9,    10,    11,   195,     0,    12,    87,    13,
      14,   196,    15,    16,    17,    18,    19,    20,   197,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    88,
       0,   120,     0,     0,     0,     0,     0,    89,    90,    91,
      92,    93,    94,    54,     3,    77,    78,    79,    80,    81,
      82,    83,   199,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    84,    85,     0,     0,   187,   188,     0,   189,   190,
     191,     0,     0,   245,     0,   192,   193,   194,     0,     0,
       0,   195,     0,     0,    87,     0,     0,   196,     0,     2,
       0,     3,     0,     0,   197,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    88,     0,   120,     0,     0,
       0,     0,     0,    89,    90,    91,    92,    93,    94,     0,
       0,     4,     0,     0,     5,     0,     0,     0,   199,     6,
       7,     8,     0,     0,     0,     9,    10,    11,     0,     0,
      12,     0,    13,    14,     0,    15,    16,    17,    18,    19,
      20,    54,     0,    77,    78,    79,    80,    81,    82,    83,
       0,     0,     0,     0,   120,     0,     0,    54,     0,    77,
      78,    79,    80,    81,    82,    83,     0,     0,     0,    84,
      85,     0,     0,     0,   121,     0,     0,     0,     0,     0,
       0,   245,     0,     0,     0,    84,    85,     0,     0,     0,
       0,     0,    87,     0,     0,     0,     0,   245,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    87,     0,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
       0,    89,    90,    91,    92,    93,    94,     0,     0,    88,
       0,     0,     0,     0,     0,     0,   303,    89,    90,    91,
      92,    93,    94,    54,     0,    77,    78,    79,    80,    81,
      82,    83,   354,     0,     0,     0,     0,     0,     0,    54,
       0,    77,    78,    79,    80,    81,    82,    83,     0,     0,
       0,    84,    85,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   245,     0,     0,     0,    84,    85,     0,
       0,     0,     0,     0,    87,     0,     0,     0,     0,   245,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      87,     0,     0,     0,     0,    88,     0,     0,     0,     0,
       0,     0,     0,    89,    90,    91,    92,    93,    94,     0,
       0,    88,     0,     0,     0,     0,     0,     0,   381,    89,
      90,    91,    92,    93,    94,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   396,     2,    54,     3,    77,    78,
      79,    80,    81,    82,    83,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    84,    85,     0,     0,     0,     0,
       5,     0,     0,     0,     0,     6,   148,     0,     0,     0,
       0,     0,     0,    11,     0,     0,    12,    87,     0,    14,
       0,     0,     0,    17,    18,    19,    20,     2,    54,     3,
      77,    78,    79,    80,    81,    82,    83,     0,    88,     0,
     246,     0,     0,     0,     0,     0,    89,    90,    91,    92,
      93,    94,     0,     0,     0,     0,    84,    85,     0,     0,
       0,     0,     5,     0,     0,     0,     0,     6,   148,     0,
       0,     0,     0,     0,     0,    11,     0,     0,    12,    87,
       0,    14,     0,     0,     0,    17,    18,    19,    20,    54,
       0,    77,    78,    79,    80,    81,    82,    83,     0,     0,
      88,     0,     0,     0,     0,     0,     0,     0,    89,    90,
      91,    92,    93,    94,     0,     0,     0,    84,    85,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    54,    86,
      77,    78,    79,    80,    81,    82,    83,     0,     0,     0,
      87,     0,     0,     0,     0,     0,     0,    54,     0,    77,
      78,    79,    80,    81,    82,    83,    84,    85,     0,     0,
       0,    88,     0,   214,   387,     0,     0,     0,    86,    89,
      90,    91,    92,    93,    94,    84,    85,     0,     0,    87,
       0,     0,     0,     0,     0,     0,    54,    86,    77,    78,
      79,    80,    81,    82,    83,     0,     0,     0,    87,     0,
      88,     0,   214,     0,     0,     0,     0,     0,    89,    90,
      91,    92,    93,    94,    84,    85,     0,     0,     0,    88,
       0,     0,     0,     0,   225,     0,    86,    89,    90,    91,
      92,    93,    94,     0,     0,     0,    54,    87,    77,    78,
      79,    80,    81,    82,    83,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    88,   271,
       0,     0,     0,     0,    84,    85,    89,    90,    91,    92,
      93,    94,     0,     0,     0,    54,    86,    77,    78,    79,
      80,    81,    82,    83,     0,     0,     0,    87,     0,     0,
       0,     0,     0,     0,    54,     0,    77,    78,    79,    80,
      81,    82,    83,    84,    85,     0,     0,     0,    88,     0,
       0,     0,     0,   341,     0,    86,    89,    90,    91,    92,
      93,    94,    84,    85,     0,     0,    87,     0,     0,     0,
       0,     0,     0,    54,   245,    77,    78,    79,    80,    81,
      82,    83,     0,     0,     0,    87,     0,    88,     0,     0,
       0,     0,   375,     0,     0,    89,    90,    91,    92,    93,
      94,    84,    85,     0,     0,     0,    88,   393,     0,     0,
       0,     0,     0,   245,    89,    90,    91,    92,    93,    94,
       0,     0,     0,    54,    87,    77,    78,    79,    80,    81,
      82,    83,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    88,   404,     0,     0,     0,
       0,    84,    85,    89,    90,    91,    92,    93,    94,     0,
       0,     0,    54,   245,    77,    78,    79,    80,    81,    82,
      83,     0,     0,     0,    87,     0,     0,     0,     0,     0,
       0,    54,     0,    77,    78,    79,    80,    81,    82,    83,
      84,    85,     0,     0,     0,    88,   406,     0,     0,     0,
       0,     0,   245,    89,    90,    91,    92,    93,    94,    84,
      85,     0,     0,    87,     0,     0,     0,     0,     0,     0,
      54,    86,    77,    78,    79,    80,    81,    82,    83,     0,
       0,     0,    87,     0,    88,   416,     0,     0,     0,     0,
       0,     0,    89,    90,    91,    92,    93,    94,    84,    85,
       0,     0,     0,    88,     0,     0,     0,     0,     0,     0,
      86,    89,    90,    91,    92,    93,    94,     0,     0,     0,
      54,    87,    77,    78,    79,    80,    81,    82,    83,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   142,     0,     0,     0,     0,     0,    84,    85,
      89,    90,    91,    92,    93,    94,     0,     0,     0,    54,
      86,    77,    78,    79,    80,    81,    82,    83,     0,     0,
       0,    87,     0,     0,     0,     0,     0,     0,    54,     0,
      77,    78,    79,    80,    81,    82,    83,    84,    85,     0,
       0,     0,   146,     0,     0,     0,     0,     0,     0,   245,
      89,    90,    91,    92,    93,    94,    84,    85,     0,     0,
      87,     0,     0,     0,     0,     0,     0,     0,   245,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    87,
       0,    88,     0,     0,     0,     2,    54,     3,     0,    89,
      90,    91,    92,    93,    94,   219,     0,     0,     0,     0,
     248,     0,     0,     0,     0,     0,     0,     0,    89,    90,
      91,    92,    93,    94,     0,     0,     0,     4,     0,     0,
       5,     0,     0,     0,     0,     6,     7,     8,     0,     0,
       0,     9,    10,    11,     0,     0,    12,     0,    13,    14,
       0,    15,    16,    17,    18,    19,    20,     2,     0,     3,
       0,     0,     0,     0,     0,     0,     0,   219,   313,   338,
       0,     0,   265,     0,     0,     0,     0,    56,     0,     0,
       0,     0,     0,     0,     2,     0,     3,     0,     0,     4,
       0,     0,     5,     0,     0,     0,     0,     6,     7,     8,
       0,     0,     0,     9,    10,    11,     0,     0,    12,     0,
      13,    14,     0,    15,    16,    17,    18,    19,    20,     5,
       0,     2,     0,     3,     6,     7,     0,     0,     0,     0,
     264,   338,    11,     0,   265,    12,     0,     0,    14,    56,
       0,     0,    17,    18,    19,    20,     0,     0,     0,     0,
       0,     0,     2,     0,     3,     0,     5,     0,     0,     0,
     228,     6,     7,     0,     0,     0,     0,     0,     0,    11,
       0,     0,    12,     0,     0,    14,     0,     0,     0,    17,
      18,    19,    20,     0,     4,     0,     0,     5,     0,     0,
       0,     0,     6,     7,     8,     0,     0,   325,     9,    10,
      11,     0,     0,    12,     0,    13,    14,     0,    15,    16,
      17,    18,    19,    20,     2,    54,     3,     0,     0,     0,
       0,     0,     0,     0,   219,     0,     0,   120,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     4,     0,     0,     5,
       0,     0,     0,     0,     6,     7,     8,     0,     0,     0,
       9,    10,    11,     0,     0,    12,     0,    13,    14,     0,
      15,    16,    17,    18,    19,    20,     2,     0,     3,     0,
       0,     0,     0,     0,     0,     0,   219,     0,   220,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     4,     0,
       0,     5,     0,     0,     0,     0,     6,     7,     8,     0,
       0,     0,     9,    10,    11,     0,     0,    12,     0,    13,
      14,     0,    15,    16,    17,    18,    19,    20,     2,     0,
       3,     0,     0,     0,     0,     0,     0,     0,   219,     0,
     373,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     2,
       4,     3,     0,     5,     0,     0,     0,     0,     6,     7,
       8,     0,     0,     0,     9,    10,    11,     0,     0,    12,
       0,    13,    14,     0,    15,    16,    17,    18,    19,    20,
       0,     4,     0,     0,     5,     0,     0,     0,     0,     6,
       7,     8,     0,     0,     0,     9,    10,    11,     0,     0,
      12,     0,    13,    14,     0,    15,    16,    17,    18,    19,
      20
};

static const yytype_int16 yycheck[] =
{
      34,    34,     0,    56,    63,    39,   126,    41,    49,   214,
      88,    39,   121,    41,   208,   100,    88,   130,   127,   153,
       0,    55,     4,     5,   188,     4,   266,    83,    78,    63,
      56,    39,    83,    41,     4,    83,     4,    35,     4,    37,
      66,     4,    76,    99,    42,    43,     4,    45,    99,    97,
     159,   129,     4,     5,    76,   133,    31,    81,    80,    83,
       4,    61,    88,    85,   117,    45,   230,    83,   146,    76,
     142,    66,    76,    80,   146,   315,   148,    78,   163,   164,
     165,    61,     4,    99,   118,   118,   120,   221,   160,   123,
     123,   117,   126,    88,     4,    76,    78,    76,   132,   132,
     153,    80,   136,   129,   130,   214,    76,   133,   134,   181,
      76,    79,   120,    28,    29,    85,    79,    76,    76,    85,
     146,    96,   235,   195,   158,   125,    78,   161,   126,    99,
     158,    97,    76,   161,   129,   130,    80,    85,   133,   134,
     334,    85,    90,    91,    79,   125,    83,   188,    83,    76,
     158,   146,   261,   161,   263,    83,   265,   191,   363,   193,
     324,    77,    99,    77,   328,   193,    76,    83,   221,    83,
     248,    99,    10,   245,   208,    85,   248,    92,    93,    32,
      33,    34,    79,   191,   269,   193,    83,   221,   221,   230,
      83,    14,    15,    16,    17,    18,    19,    20,    21,    83,
     208,    24,    25,   237,    77,    84,    99,    77,   242,   235,
      83,   264,   246,    83,    77,    99,    94,    76,    95,    77,
      83,    80,   248,    76,   344,    83,    77,    80,   300,    82,
     302,    30,    83,   305,   306,    77,   345,    38,   246,   348,
     235,    83,   191,    44,    79,    77,    77,    77,    83,    77,
      51,    83,    83,   248,   363,    83,   315,    86,    87,    77,
     313,    77,    77,    64,   298,    83,   264,    83,    83,    78,
      77,    22,    23,   307,    77,    98,    83,    98,   350,   313,
      99,   315,   354,   324,    85,    26,    27,   328,   322,   322,
     298,    76,     4,     5,    99,    80,   330,   166,   167,   307,
     334,    97,    49,   170,   171,   172,   173,    76,   380,   381,
      76,   383,   168,   169,    76,   313,    76,   351,    97,    97,
     318,    97,    81,   395,   396,    97,   334,   174,   175,    77,
      98,    77,    77,    83,    97,    65,   408,    84,    85,    77,
      87,    99,    81,   351,    76,    81,   344,    42,    77,   298,
     384,   385,   386,   100,   176,    99,   180,   177,   307,   393,
     134,   178,   118,   318,   137,   179,   322,    45,    -1,   403,
     404,   136,   406,   126,   246,   409,   384,   385,   386,   413,
      -1,   415,   416,    -1,    -1,   393,    -1,    -1,   422,   146,
      -1,    -1,    -1,   140,   141,   403,   404,    -1,   406,    -1,
      -1,   409,   351,    -1,    -1,   413,    -1,   415,   416,    -1,
      -1,     3,    -1,     5,   422,    -1,   163,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
     177,   178,   179,   180,    -1,   384,   385,   386,    -1,    -1,
      -1,   188,     0,     1,   393,     3,    38,     5,    -1,    -1,
      -1,    43,    44,    -1,   403,   404,    -1,   406,    -1,    51,
     409,    -1,    54,    -1,   413,    57,   415,   416,    -1,    61,
      62,    63,    64,   422,    -1,    -1,    -1,    35,    -1,    -1,
      38,    -1,    -1,   230,    -1,    43,    44,    45,    -1,    -1,
      -1,    49,    50,    51,    -1,    -1,    54,    -1,    56,    57,
      -1,    59,    60,    61,    62,    63,    64,    -1,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    -1,    -1,
      -1,    -1,   269,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
       1,    -1,     3,    -1,     5,    -1,    -1,    -1,    -1,    -1,
      -1,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    43,    44,    45,    -1,    -1,   324,    49,    50,
      51,   328,    -1,    54,    -1,    56,    57,    -1,    59,    60,
      61,    62,    63,    64,    -1,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,    -1,
      -1,    -1,    -1,    -1,    32,    33,    -1,    35,    36,    37,
      38,    39,    40,    41,    -1,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    -1,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    -1,
      78,    79,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    99,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,    -1,    35,    36,    37,    38,    39,    40,
      41,    -1,    43,    44,    45,    46,    47,    48,    49,    50,
      51,    52,    -1,    54,    55,    56,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    78,    79,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    32,    33,
      -1,    35,    36,    37,    38,    39,    40,    41,    -1,    43,
      44,    45,    46,    47,    48,    49,    50,    51,    52,    -1,
      54,    55,    56,    57,    58,    59,    60,    61,    62,    63,
      64,    65,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    -1,    78,    79,    -1,    -1,    -1,    -1,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    99,     3,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    32,    33,    -1,    35,    36,
      37,    38,    39,    40,    41,    -1,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    -1,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,
      -1,    78,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,
      87,    88,    89,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    32,    33,    -1,    -1,    36,    37,    -1,    39,    40,
      41,    -1,    -1,    44,    -1,    46,    47,    48,    -1,    -1,
      -1,    52,    -1,    -1,    55,    -1,    -1,    58,    -1,     3,
      -1,     5,    -1,    -1,    65,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    -1,
      -1,    35,    -1,    -1,    38,    -1,    -1,    -1,    99,    43,
      44,    45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,
      54,    -1,    56,    57,    -1,    59,    60,    61,    62,    63,
      64,     4,    -1,     6,     7,     8,     9,    10,    11,    12,
      -1,    -1,    -1,    -1,    78,    -1,    -1,     4,    -1,     6,
       7,     8,     9,    10,    11,    12,    -1,    -1,    -1,    32,
      33,    -1,    -1,    -1,    98,    -1,    -1,    -1,    -1,    -1,
      -1,    44,    -1,    -1,    -1,    32,    33,    -1,    -1,    -1,
      -1,    -1,    55,    -1,    -1,    -1,    -1,    44,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,    -1,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,    86,    87,    88,    89,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    -1,    -1,    99,    84,    85,    86,
      87,    88,    89,     4,    -1,     6,     7,     8,     9,    10,
      11,    12,    99,    -1,    -1,    -1,    -1,    -1,    -1,     4,
      -1,     6,     7,     8,     9,    10,    11,    12,    -1,    -1,
      -1,    32,    33,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    44,    -1,    -1,    -1,    32,    33,    -1,
      -1,    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    44,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    84,    85,    86,    87,    88,    89,    -1,
      -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    99,    84,
      85,    86,    87,    88,    89,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    32,    33,    -1,    -1,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    43,    44,    -1,    -1,    -1,
      -1,    -1,    -1,    51,    -1,    -1,    54,    55,    -1,    57,
      -1,    -1,    -1,    61,    62,    63,    64,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    -1,    76,    -1,
      78,    -1,    -1,    -1,    -1,    -1,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,    -1,    32,    33,    -1,    -1,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,    54,    55,
      -1,    57,    -1,    -1,    -1,    61,    62,    63,    64,     4,
      -1,     6,     7,     8,     9,    10,    11,    12,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    32,    33,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     4,    44,
       6,     7,     8,     9,    10,    11,    12,    -1,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,     6,
       7,     8,     9,    10,    11,    12,    32,    33,    -1,    -1,
      -1,    76,    -1,    78,    79,    -1,    -1,    -1,    44,    84,
      85,    86,    87,    88,    89,    32,    33,    -1,    -1,    55,
      -1,    -1,    -1,    -1,    -1,    -1,     4,    44,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    55,    -1,
      76,    -1,    78,    -1,    -1,    -1,    -1,    -1,    84,    85,
      86,    87,    88,    89,    32,    33,    -1,    -1,    -1,    76,
      -1,    -1,    -1,    -1,    81,    -1,    44,    84,    85,    86,
      87,    88,    89,    -1,    -1,    -1,     4,    55,     6,     7,
       8,     9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    76,    77,
      -1,    -1,    -1,    -1,    32,    33,    84,    85,    86,    87,
      88,    89,    -1,    -1,    -1,     4,    44,     6,     7,     8,
       9,    10,    11,    12,    -1,    -1,    -1,    55,    -1,    -1,
      -1,    -1,    -1,    -1,     4,    -1,     6,     7,     8,     9,
      10,    11,    12,    32,    33,    -1,    -1,    -1,    76,    -1,
      -1,    -1,    -1,    81,    -1,    44,    84,    85,    86,    87,
      88,    89,    32,    33,    -1,    -1,    55,    -1,    -1,    -1,
      -1,    -1,    -1,     4,    44,     6,     7,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    55,    -1,    76,    -1,    -1,
      -1,    -1,    81,    -1,    -1,    84,    85,    86,    87,    88,
      89,    32,    33,    -1,    -1,    -1,    76,    77,    -1,    -1,
      -1,    -1,    -1,    44,    84,    85,    86,    87,    88,    89,
      -1,    -1,    -1,     4,    55,     6,     7,     8,     9,    10,
      11,    12,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,
      -1,    32,    33,    84,    85,    86,    87,    88,    89,    -1,
      -1,    -1,     4,    44,     6,     7,     8,     9,    10,    11,
      12,    -1,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,
      -1,     4,    -1,     6,     7,     8,     9,    10,    11,    12,
      32,    33,    -1,    -1,    -1,    76,    77,    -1,    -1,    -1,
      -1,    -1,    44,    84,    85,    86,    87,    88,    89,    32,
      33,    -1,    -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,
       4,    44,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    -1,    55,    -1,    76,    77,    -1,    -1,    -1,    -1,
      -1,    -1,    84,    85,    86,    87,    88,    89,    32,    33,
      -1,    -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      44,    84,    85,    86,    87,    88,    89,    -1,    -1,    -1,
       4,    55,     6,     7,     8,     9,    10,    11,    12,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    32,    33,
      84,    85,    86,    87,    88,    89,    -1,    -1,    -1,     4,
      44,     6,     7,     8,     9,    10,    11,    12,    -1,    -1,
      -1,    55,    -1,    -1,    -1,    -1,    -1,    -1,     4,    -1,
       6,     7,     8,     9,    10,    11,    12,    32,    33,    -1,
      -1,    -1,    76,    -1,    -1,    -1,    -1,    -1,    -1,    44,
      84,    85,    86,    87,    88,    89,    32,    33,    -1,    -1,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    44,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    55,
      -1,    76,    -1,    -1,    -1,     3,     4,     5,    -1,    84,
      85,    86,    87,    88,    89,    13,    -1,    -1,    -1,    -1,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    84,    85,
      86,    87,    88,    89,    -1,    -1,    -1,    35,    -1,    -1,
      38,    -1,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,
      -1,    49,    50,    51,    -1,    -1,    54,    -1,    56,    57,
      -1,    59,    60,    61,    62,    63,    64,     3,    -1,     5,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    76,    77,
      -1,    -1,    80,    -1,    -1,    -1,    -1,    85,    -1,    -1,
      -1,    -1,    -1,    -1,     3,    -1,     5,    -1,    -1,    35,
      -1,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,    45,
      -1,    -1,    -1,    49,    50,    51,    -1,    -1,    54,    -1,
      56,    57,    -1,    59,    60,    61,    62,    63,    64,    38,
      -1,     3,    -1,     5,    43,    44,    -1,    -1,    -1,    -1,
      76,    77,    51,    -1,    80,    54,    -1,    -1,    57,    85,
      -1,    -1,    61,    62,    63,    64,    -1,    -1,    -1,    -1,
      -1,    -1,     3,    -1,     5,    -1,    38,    -1,    -1,    -1,
      79,    43,    44,    -1,    -1,    -1,    -1,    -1,    -1,    51,
      -1,    -1,    54,    -1,    -1,    57,    -1,    -1,    -1,    61,
      62,    63,    64,    -1,    35,    -1,    -1,    38,    -1,    -1,
      -1,    -1,    43,    44,    45,    -1,    -1,    79,    49,    50,
      51,    -1,    -1,    54,    -1,    56,    57,    -1,    59,    60,
      61,    62,    63,    64,     3,     4,     5,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    13,    -1,    -1,    78,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,    -1,    38,
      -1,    -1,    -1,    -1,    43,    44,    45,    -1,    -1,    -1,
      49,    50,    51,    -1,    -1,    54,    -1,    56,    57,    -1,
      59,    60,    61,    62,    63,    64,     3,    -1,     5,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,    77,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    35,    -1,
      -1,    38,    -1,    -1,    -1,    -1,    43,    44,    45,    -1,
      -1,    -1,    49,    50,    51,    -1,    -1,    54,    -1,    56,
      57,    -1,    59,    60,    61,    62,    63,    64,     3,    -1,
       5,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    13,    -1,
      77,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
      35,     5,    -1,    38,    -1,    -1,    -1,    -1,    43,    44,
      45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,    54,
      -1,    56,    57,    -1,    59,    60,    61,    62,    63,    64,
      -1,    35,    -1,    -1,    38,    -1,    -1,    -1,    -1,    43,
      44,    45,    -1,    -1,    -1,    49,    50,    51,    -1,    -1,
      54,    -1,    56,    57,    -1,    59,    60,    61,    62,    63,
      64
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     5,    35,    38,    43,    44,    45,    49,
      50,    51,    54,    56,    57,    59,    60,    61,    62,    63,
      64,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    99,   125,   126,   127,   130,   131,   132,   133,   134,
     140,   141,   145,   146,   157,   169,   170,   171,   173,   174,
     175,   176,   177,   178,     4,    76,    85,    99,   103,   128,
     129,   147,   148,   149,   127,   127,    78,   103,   104,   157,
      78,   104,   127,   127,     0,   170,    76,     6,     7,     8,
       9,    10,    11,    12,    32,    33,    44,    55,    76,    84,
      85,    86,    87,    88,    89,   101,   102,   103,   105,   107,
     108,   109,   110,   111,   112,   113,   114,   115,   116,   117,
     118,   119,   120,   124,   147,   145,   149,   150,    83,    99,
      78,    98,   125,   127,   162,   172,    76,    80,   148,   132,
     135,   136,   137,   145,    78,   142,   143,    78,   103,   153,
      76,    76,    76,   107,   107,    76,    76,   107,    44,   107,
     120,   121,   123,   137,   154,    10,    32,    33,    34,    76,
      80,    82,   109,    85,    90,    91,    86,    87,    22,    23,
      28,    29,    92,    93,    26,    27,    84,    94,    95,    30,
      31,    96,    77,   145,   149,   129,   147,    36,    37,    39,
      40,    41,    46,    47,    48,    52,    58,    65,    79,    99,
     103,   104,   123,   125,   157,   160,   161,   162,   163,   164,
     165,   166,   167,   168,    78,   121,   158,   125,   162,    13,
      77,   127,   151,   152,   153,    81,   121,   137,    79,   136,
      97,   138,   139,   147,   137,   135,    79,    83,   103,   144,
     142,    77,    83,   107,   107,    44,    78,   154,    76,   123,
      14,    15,    16,    17,    18,    19,    20,    21,    24,    25,
      98,   122,    77,    83,    76,    80,   149,   155,   156,    77,
     104,    77,   106,   121,   123,   104,   109,   109,   109,   110,
     110,   111,   111,   112,   112,   112,   112,   113,   113,   114,
     115,   116,   117,   118,   123,    99,   124,    99,    97,   160,
      76,   104,    76,    99,   123,    76,    76,    97,    99,    79,
     164,   158,   159,    76,   147,   149,   155,    77,    83,    77,
      81,   124,    83,    99,    97,    79,    79,   144,    98,    79,
      83,   103,    77,    77,   163,    77,   121,   121,    77,   151,
     155,    81,   121,   156,    76,    80,   109,    77,    83,    81,
      97,    97,   160,    65,    99,   123,    99,   123,    99,   123,
     123,   160,    79,    83,   152,   139,   124,   124,    79,    79,
      77,    77,    81,    77,   151,    81,   121,   121,   123,   160,
      76,    99,   123,    99,    77,    77,    77,    79,   158,    77,
      77,    81,   123,    77,   123,    99,    99,   123,   160,   160,
     160,    77,   160,    77,    77,   123,    77,   123,    99,    42,
      99,   160,   160,    77,   160,    77,    77,   123,   160,   160,
     160,   160,    77,   160
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   100,   101,   101,   101,   101,   101,   101,   101,   101,
     102,   102,   103,   104,   104,   105,   105,   105,   105,   105,
     105,   105,   105,   106,   106,   107,   107,   107,   107,   107,
     107,   107,   107,   108,   108,   108,   108,   108,   108,   109,
     109,   110,   110,   110,   110,   111,   111,   111,   112,   112,
     112,   113,   113,   113,   113,   113,   114,   114,   114,   115,
     115,   116,   116,   117,   117,   118,   118,   119,   119,   120,
     120,   121,   121,   122,   122,   122,   122,   122,   122,   122,
     122,   122,   122,   122,   123,   123,   123,   124,   125,   125,
     126,   127,   127,   127,   127,   127,   127,   127,   127,   128,
     128,   129,   129,   130,   130,   130,   130,   130,   130,   131,
     132,   132,   132,   132,   132,   132,   132,   133,   133,   133,
     134,   134,   135,   135,   136,   137,   137,   137,   137,   138,
     138,   139,   139,   139,   139,   140,   140,   140,   140,   140,
     141,   143,   142,   142,   144,   144,   145,   145,   145,   145,
     146,   147,   147,   148,   148,   148,   148,   148,   148,   148,
     149,   149,   149,   149,   150,   150,   151,   151,   152,   152,
     152,   152,   153,   153,   154,   154,   155,   155,   155,   156,
     156,   156,   156,   156,   156,   156,   156,   156,   157,   158,
     158,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     161,   161,   161,   162,   162,   163,   163,   164,   164,   165,
     165,   166,   166,   166,   167,   167,   167,   167,   167,   167,
     167,   167,   167,   167,   168,   168,   168,   168,   168,   169,
     169,   170,   170,   170,   170,   171,   171,   172,   172,   173,
     174,   175,   176,   177,   177,   177,   177,   177,   177,   177,
     177,   178,   178,   178,   178
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     1,     1,     1,     3,     6,
       1,     2,     1,     1,     1,     1,     4,     4,     3,     3,
       3,     2,     2,     1,     3,     1,     2,     2,     2,     4,
       4,     2,     4,     1,     1,     1,     1,     1,     1,     1,
       4,     1,     3,     3,     3,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     3,     1,     3,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     3,     1,
       5,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     2,     1,     3,     2,
       1,     2,     1,     2,     1,     2,     1,     2,     1,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     5,     4,     2,
       1,     1,     1,     2,     3,     2,     1,     2,     1,     1,
       3,     0,     1,     2,     3,     5,     4,     6,     5,     2,
       1,     0,     2,     3,     1,     3,     1,     1,     1,     1,
       1,     2,     1,     1,     3,     4,     3,     4,     4,     3,
       2,     1,     3,     2,     1,     2,     1,     3,     2,     2,
       1,     1,     1,     3,     1,     2,     1,     1,     2,     3,
       2,     3,     3,     4,     2,     3,     3,     4,     1,     1,
       3,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       3,     4,     3,     2,     3,     1,     2,     1,     1,     1,
       2,     5,     7,     5,     5,     7,     6,     7,     7,     8,
       7,     8,     8,     9,     3,     2,     2,     2,     3,     1,
       2,     1,     1,     1,     1,     4,     3,     1,     2,     1,
       1,     4,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1
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
        yyerror (scanner, YY_("syntax error: cannot back up")); \
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
                  Type, Value, scanner); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, GISourceScanner* scanner)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
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
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, GISourceScanner* scanner)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, GISourceScanner* scanner)
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
                                              , scanner);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, GISourceScanner* scanner)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
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
yyparse (GISourceScanner* scanner)
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
      yychar = yylex (scanner);
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
  case 2:
#line 345 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = g_hash_table_lookup (scanner->const_table, (yyvsp[0].str));
		if ((yyval.symbol) == NULL) {
			(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		} else {
			(yyval.symbol) = gi_source_symbol_ref ((yyval.symbol));
		}
	  }
#line 2266 "giscanner/scannerparser.c"
    break;

  case 3:
#line 354 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		char *rest;
		guint64 value;
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		if (g_str_has_prefix (yytext, "0x") && strlen (yytext) > 2) {
			value = g_ascii_strtoull (yytext + 2, &rest, 16);
		} else if (g_str_has_prefix (yytext, "0") && strlen (yytext) > 1) {
			value = g_ascii_strtoull (yytext + 1, &rest, 8);
		} else {
			value = g_ascii_strtoull (yytext, &rest, 10);
		}
		(yyval.symbol)->const_int = value;
		(yyval.symbol)->const_int_is_unsigned = (rest && (rest[0] == 'U'));
	  }
#line 2286 "giscanner/scannerparser.c"
    break;

  case 4:
#line 370 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_boolean_set = TRUE;
		(yyval.symbol)->const_boolean = g_ascii_strcasecmp (yytext, "true") == 0 ? TRUE : FALSE;
	  }
#line 2296 "giscanner/scannerparser.c"
    break;

  case 5:
#line 376 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = g_utf8_get_char(yytext + 1);
	  }
#line 2306 "giscanner/scannerparser.c"
    break;

  case 6:
#line 382 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_double_set = TRUE;
		(yyval.symbol)->const_double = 0.0;
        sscanf (yytext, "%lf", &((yyval.symbol)->const_double));
	  }
#line 2317 "giscanner/scannerparser.c"
    break;

  case 8:
#line 390 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
	  }
#line 2325 "giscanner/scannerparser.c"
    break;

  case 9:
#line 394 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2333 "giscanner/scannerparser.c"
    break;

  case 10:
#line 402 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		yytext[strlen (yytext) - 1] = '\0';
		(yyval.symbol)->const_string = parse_c_string_literal (yytext + 1);
                if (!g_utf8_validate ((yyval.symbol)->const_string, -1, NULL))
                  {
#if 0
                    g_warning ("Ignoring non-UTF-8 constant string \"%s\"", yytext + 1);
#endif
                    g_free((yyval.symbol)->const_string);
                    (yyval.symbol)->const_string = NULL;
                  }

	  }
#line 2352 "giscanner/scannerparser.c"
    break;

  case 11:
#line 417 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		char *strings, *string2;
		(yyval.symbol) = (yyvsp[-1].symbol);
		yytext[strlen (yytext) - 1] = '\0';
		string2 = parse_c_string_literal (yytext + 1);
		strings = g_strconcat ((yyval.symbol)->const_string, string2, NULL);
		g_free ((yyval.symbol)->const_string);
		g_free (string2);
		(yyval.symbol)->const_string = strings;
	  }
#line 2367 "giscanner/scannerparser.c"
    break;

  case 12:
#line 431 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.str) = g_strdup (yytext);
	  }
#line 2375 "giscanner/scannerparser.c"
    break;

  case 16:
#line 444 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2383 "giscanner/scannerparser.c"
    break;

  case 17:
#line 448 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2391 "giscanner/scannerparser.c"
    break;

  case 18:
#line 452 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2399 "giscanner/scannerparser.c"
    break;

  case 19:
#line 456 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2407 "giscanner/scannerparser.c"
    break;

  case 20:
#line 460 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2415 "giscanner/scannerparser.c"
    break;

  case 21:
#line 464 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2423 "giscanner/scannerparser.c"
    break;

  case 22:
#line 468 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2431 "giscanner/scannerparser.c"
    break;

  case 26:
#line 481 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2439 "giscanner/scannerparser.c"
    break;

  case 27:
#line 485 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2447 "giscanner/scannerparser.c"
    break;

  case 28:
#line 489 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		switch ((yyvsp[-1].unary_operator)) {
		case UNARY_PLUS:
			(yyval.symbol) = (yyvsp[0].symbol);
			break;
		case UNARY_MINUS:
			(yyval.symbol) = gi_source_symbol_copy ((yyvsp[0].symbol));
			(yyval.symbol)->const_int = -(yyvsp[0].symbol)->const_int;
			break;
		case UNARY_BITWISE_COMPLEMENT:
			(yyval.symbol) = gi_source_symbol_copy ((yyvsp[0].symbol));
			(yyval.symbol)->const_int = ~(yyvsp[0].symbol)->const_int;
			break;
		case UNARY_LOGICAL_NEGATION:
			(yyval.symbol) = gi_source_symbol_copy ((yyvsp[0].symbol));
			(yyval.symbol)->const_int = !gi_source_symbol_get_const_boolean ((yyvsp[0].symbol));
			break;
		default:
			(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
			break;
		}
	  }
#line 2474 "giscanner/scannerparser.c"
    break;

  case 29:
#line 512 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
		if ((yyval.symbol)->const_int_set) {
			(yyval.symbol)->base_type = gi_source_basic_type_new ((yyval.symbol)->const_int_is_unsigned ? "guint64" : "gint64");
		}
	  }
#line 2485 "giscanner/scannerparser.c"
    break;

  case 30:
#line 519 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
		if ((yyval.symbol)->const_int_set) {
			(yyval.symbol)->base_type = gi_source_basic_type_new ("guint64");
		}
	  }
#line 2496 "giscanner/scannerparser.c"
    break;

  case 31:
#line 526 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2504 "giscanner/scannerparser.c"
    break;

  case 32:
#line 530 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		ctype_free ((yyvsp[-1].ctype));
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2513 "giscanner/scannerparser.c"
    break;

  case 33:
#line 538 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.unary_operator) = UNARY_ADDRESS_OF;
	  }
#line 2521 "giscanner/scannerparser.c"
    break;

  case 34:
#line 542 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.unary_operator) = UNARY_POINTER_INDIRECTION;
	  }
#line 2529 "giscanner/scannerparser.c"
    break;

  case 35:
#line 546 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.unary_operator) = UNARY_PLUS;
	  }
#line 2537 "giscanner/scannerparser.c"
    break;

  case 36:
#line 550 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.unary_operator) = UNARY_MINUS;
	  }
#line 2545 "giscanner/scannerparser.c"
    break;

  case 37:
#line 554 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.unary_operator) = UNARY_BITWISE_COMPLEMENT;
	  }
#line 2553 "giscanner/scannerparser.c"
    break;

  case 38:
#line 558 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.unary_operator) = UNARY_LOGICAL_NEGATION;
	  }
#line 2561 "giscanner/scannerparser.c"
    break;

  case 40:
#line 566 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		if ((yyval.symbol)->const_int_set || (yyval.symbol)->const_double_set || (yyval.symbol)->const_string != NULL) {
			(yyval.symbol)->base_type = (yyvsp[-2].ctype);
		} else {
			ctype_free ((yyvsp[-2].ctype));
		}
	  }
#line 2574 "giscanner/scannerparser.c"
    break;

  case 42:
#line 579 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int * (yyvsp[0].symbol)->const_int;
	  }
#line 2584 "giscanner/scannerparser.c"
    break;

  case 43:
#line 585 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		if ((yyvsp[0].symbol)->const_int != 0) {
			(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int / (yyvsp[0].symbol)->const_int;
		}
	  }
#line 2596 "giscanner/scannerparser.c"
    break;

  case 44:
#line 593 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		if ((yyvsp[0].symbol)->const_int != 0) {
			(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int % (yyvsp[0].symbol)->const_int;
		}
	  }
#line 2608 "giscanner/scannerparser.c"
    break;

  case 46:
#line 605 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int + (yyvsp[0].symbol)->const_int;
	  }
#line 2618 "giscanner/scannerparser.c"
    break;

  case 47:
#line 611 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int - (yyvsp[0].symbol)->const_int;
	  }
#line 2628 "giscanner/scannerparser.c"
    break;

  case 49:
#line 621 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int << (yyvsp[0].symbol)->const_int;

		/* assume this is a bitfield/flags declaration
		 * if a left shift operator is sued in an enum value
                 * This mimics the glib-mkenum behavior.
		 */
		is_bitfield = TRUE;
	  }
#line 2644 "giscanner/scannerparser.c"
    break;

  case 50:
#line 633 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int >> (yyvsp[0].symbol)->const_int;
	  }
#line 2654 "giscanner/scannerparser.c"
    break;

  case 52:
#line 643 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int < (yyvsp[0].symbol)->const_int;
	  }
#line 2664 "giscanner/scannerparser.c"
    break;

  case 53:
#line 649 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int > (yyvsp[0].symbol)->const_int;
	  }
#line 2674 "giscanner/scannerparser.c"
    break;

  case 54:
#line 655 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int <= (yyvsp[0].symbol)->const_int;
	  }
#line 2684 "giscanner/scannerparser.c"
    break;

  case 55:
#line 661 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int >= (yyvsp[0].symbol)->const_int;
	  }
#line 2694 "giscanner/scannerparser.c"
    break;

  case 57:
#line 671 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int == (yyvsp[0].symbol)->const_int;
	  }
#line 2704 "giscanner/scannerparser.c"
    break;

  case 58:
#line 677 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int != (yyvsp[0].symbol)->const_int;
	  }
#line 2714 "giscanner/scannerparser.c"
    break;

  case 60:
#line 687 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int & (yyvsp[0].symbol)->const_int;
	  }
#line 2724 "giscanner/scannerparser.c"
    break;

  case 62:
#line 697 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int ^ (yyvsp[0].symbol)->const_int;
	  }
#line 2734 "giscanner/scannerparser.c"
    break;

  case 64:
#line 707 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[-2].symbol)->const_int | (yyvsp[0].symbol)->const_int;
	  }
#line 2744 "giscanner/scannerparser.c"
    break;

  case 66:
#line 717 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int =
		  gi_source_symbol_get_const_boolean ((yyvsp[-2].symbol)) &&
		  gi_source_symbol_get_const_boolean ((yyvsp[0].symbol));
	  }
#line 2756 "giscanner/scannerparser.c"
    break;

  case 68:
#line 729 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_CONST, scanner->current_file, lineno);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int =
		  gi_source_symbol_get_const_boolean ((yyvsp[-2].symbol)) ||
		  gi_source_symbol_get_const_boolean ((yyvsp[0].symbol));
	  }
#line 2768 "giscanner/scannerparser.c"
    break;

  case 70:
#line 741 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_get_const_boolean ((yyvsp[-4].symbol)) ? (yyvsp[-2].symbol) : (yyvsp[0].symbol);
	  }
#line 2776 "giscanner/scannerparser.c"
    break;

  case 72:
#line 749 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2784 "giscanner/scannerparser.c"
    break;

  case 86:
#line 772 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 2792 "giscanner/scannerparser.c"
    break;

  case 88:
#line 785 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GList *l;
		for (l = (yyvsp[-1].list); l != NULL; l = l->next) {
			GISourceSymbol *sym = l->data;
			gi_source_symbol_merge_type (sym, gi_source_type_copy ((yyvsp[-2].ctype)));
			if ((yyvsp[-2].ctype)->storage_class_specifier & STORAGE_CLASS_TYPEDEF) {
				sym->type = CSYMBOL_TYPE_TYPEDEF;
			} else if (sym->base_type->type == CTYPE_FUNCTION) {
				sym->type = CSYMBOL_TYPE_FUNCTION;
			} else {
				sym->type = CSYMBOL_TYPE_OBJECT;
			}
			gi_source_scanner_add_symbol (scanner, sym);
			gi_source_symbol_unref (sym);
		}
		ctype_free ((yyvsp[-2].ctype));
	  }
#line 2814 "giscanner/scannerparser.c"
    break;

  case 89:
#line 803 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		ctype_free ((yyvsp[-1].ctype));
	  }
#line 2822 "giscanner/scannerparser.c"
    break;

  case 91:
#line 813 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->storage_class_specifier |= (yyvsp[-1].storage_class_specifier);
	  }
#line 2831 "giscanner/scannerparser.c"
    break;

  case 92:
#line 818 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->storage_class_specifier |= (yyvsp[0].storage_class_specifier);
	  }
#line 2840 "giscanner/scannerparser.c"
    break;

  case 93:
#line 823 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[-1].ctype);
		set_or_merge_base_type ((yyvsp[-1].ctype), (yyvsp[0].ctype));
	  }
#line 2849 "giscanner/scannerparser.c"
    break;

  case 95:
#line 829 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->type_qualifier |= (yyvsp[-1].type_qualifier);
	  }
#line 2858 "giscanner/scannerparser.c"
    break;

  case 96:
#line 834 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->type_qualifier |= (yyvsp[0].type_qualifier);
	  }
#line 2867 "giscanner/scannerparser.c"
    break;

  case 97:
#line 839 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->function_specifier |= (yyvsp[-1].function_specifier);
	  }
#line 2876 "giscanner/scannerparser.c"
    break;

  case 98:
#line 844 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->function_specifier |= (yyvsp[0].function_specifier);
	  }
#line 2885 "giscanner/scannerparser.c"
    break;

  case 99:
#line 852 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 2893 "giscanner/scannerparser.c"
    break;

  case 100:
#line 856 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 2901 "giscanner/scannerparser.c"
    break;

  case 103:
#line 868 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_TYPEDEF;
	  }
#line 2909 "giscanner/scannerparser.c"
    break;

  case 104:
#line 872 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_EXTERN;
	  }
#line 2917 "giscanner/scannerparser.c"
    break;

  case 105:
#line 876 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_STATIC;
	  }
#line 2925 "giscanner/scannerparser.c"
    break;

  case 106:
#line 880 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_AUTO;
	  }
#line 2933 "giscanner/scannerparser.c"
    break;

  case 107:
#line 884 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_REGISTER;
	  }
#line 2941 "giscanner/scannerparser.c"
    break;

  case 108:
#line 888 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.storage_class_specifier) = STORAGE_CLASS_THREAD_LOCAL;
	  }
#line 2949 "giscanner/scannerparser.c"
    break;

  case 109:
#line 895 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.str) = g_strdup (yytext);
	  }
#line 2957 "giscanner/scannerparser.c"
    break;

  case 110:
#line 902 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_VOID);
	  }
#line 2965 "giscanner/scannerparser.c"
    break;

  case 111:
#line 906 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_basic_type_new ("signed");
	  }
#line 2973 "giscanner/scannerparser.c"
    break;

  case 112:
#line 910 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_basic_type_new ("unsigned");
	  }
#line 2981 "giscanner/scannerparser.c"
    break;

  case 113:
#line 914 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_BASIC_TYPE);
		(yyval.ctype)->name = (yyvsp[0].str);
	  }
#line 2990 "giscanner/scannerparser.c"
    break;

  case 116:
#line 921 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_typedef_new ((yyvsp[0].str));
		g_free ((yyvsp[0].str));
	  }
#line 2999 "giscanner/scannerparser.c"
    break;

  case 117:
#line 929 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceSymbol *sym;
		(yyval.ctype) = (yyvsp[-4].ctype);
		(yyval.ctype)->name = (yyvsp[-3].str);
		(yyval.ctype)->child_list = (yyvsp[-1].list);

		sym = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		if ((yyval.ctype)->type == CTYPE_STRUCT) {
			sym->type = CSYMBOL_TYPE_STRUCT;
		} else if ((yyval.ctype)->type == CTYPE_UNION) {
			sym->type = CSYMBOL_TYPE_UNION;
		} else {
			g_assert_not_reached ();
		}
		sym->ident = g_strdup ((yyval.ctype)->name);
		sym->base_type = gi_source_type_copy ((yyval.ctype));
		gi_source_scanner_add_symbol (scanner, sym);
		gi_source_symbol_unref (sym);
	  }
#line 3023 "giscanner/scannerparser.c"
    break;

  case 118:
#line 949 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[-3].ctype);
		(yyval.ctype)->child_list = (yyvsp[-1].list);
	  }
#line 3032 "giscanner/scannerparser.c"
    break;

  case 119:
#line 954 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[-1].ctype);
		(yyval.ctype)->name = (yyvsp[0].str);
	  }
#line 3041 "giscanner/scannerparser.c"
    break;

  case 120:
#line 962 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
                scanner->private = FALSE;
		(yyval.ctype) = gi_source_struct_new (NULL);
	  }
#line 3050 "giscanner/scannerparser.c"
    break;

  case 121:
#line 967 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
                scanner->private = FALSE;
		(yyval.ctype) = gi_source_union_new (NULL);
	  }
#line 3059 "giscanner/scannerparser.c"
    break;

  case 123:
#line 976 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_concat ((yyvsp[-1].list), (yyvsp[0].list));
	  }
#line 3067 "giscanner/scannerparser.c"
    break;

  case 124:
#line 983 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
	    GList *l;
	    (yyval.list) = NULL;
	    for (l = (yyvsp[-1].list); l != NULL; l = l->next)
	      {
		GISourceSymbol *sym = l->data;
		if ((yyvsp[-2].ctype)->storage_class_specifier & STORAGE_CLASS_TYPEDEF)
		    sym->type = CSYMBOL_TYPE_TYPEDEF;
		else
		    sym->type = CSYMBOL_TYPE_MEMBER;
		gi_source_symbol_merge_type (sym, gi_source_type_copy ((yyvsp[-2].ctype)));
                sym->private = scanner->private;
                (yyval.list) = g_list_append ((yyval.list), sym);
	      }
	    ctype_free ((yyvsp[-2].ctype));
	  }
#line 3088 "giscanner/scannerparser.c"
    break;

  case 125:
#line 1003 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[-1].ctype);
		set_or_merge_base_type ((yyvsp[-1].ctype), (yyvsp[0].ctype));
	  }
#line 3097 "giscanner/scannerparser.c"
    break;

  case 127:
#line 1009 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = (yyvsp[0].ctype);
		(yyval.ctype)->type_qualifier |= (yyvsp[-1].type_qualifier);
	  }
#line 3106 "giscanner/scannerparser.c"
    break;

  case 128:
#line 1014 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_type_new (CTYPE_INVALID);
		(yyval.ctype)->type_qualifier |= (yyvsp[0].type_qualifier);
	  }
#line 3115 "giscanner/scannerparser.c"
    break;

  case 129:
#line 1022 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 3123 "giscanner/scannerparser.c"
    break;

  case 130:
#line 1026 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 3131 "giscanner/scannerparser.c"
    break;

  case 131:
#line 1033 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 3139 "giscanner/scannerparser.c"
    break;

  case 133:
#line 1038 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
	  }
#line 3147 "giscanner/scannerparser.c"
    break;

  case 134:
#line 1042 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-2].symbol);
		if ((yyvsp[0].symbol)->const_int_set) {
		  (yyval.symbol)->const_int_set = TRUE;
		  (yyval.symbol)->const_int = (yyvsp[0].symbol)->const_int;
		}
	  }
#line 3159 "giscanner/scannerparser.c"
    break;

  case 135:
#line 1053 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_enum_new ((yyvsp[-3].str));
		(yyval.ctype)->child_list = (yyvsp[-1].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3170 "giscanner/scannerparser.c"
    break;

  case 136:
#line 1060 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_enum_new (NULL);
		(yyval.ctype)->child_list = (yyvsp[-1].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3181 "giscanner/scannerparser.c"
    break;

  case 137:
#line 1067 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_enum_new ((yyvsp[-4].str));
		(yyval.ctype)->child_list = (yyvsp[-2].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3192 "giscanner/scannerparser.c"
    break;

  case 138:
#line 1074 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_enum_new (NULL);
		(yyval.ctype)->child_list = (yyvsp[-2].list);
		(yyval.ctype)->is_bitfield = is_bitfield || scanner->flags;
		last_enum_value = -1;
	  }
#line 3203 "giscanner/scannerparser.c"
    break;

  case 139:
#line 1081 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_enum_new ((yyvsp[0].str));
	  }
#line 3211 "giscanner/scannerparser.c"
    break;

  case 140:
#line 1088 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
                scanner->flags = FALSE;
                scanner->private = FALSE;
          }
#line 3220 "giscanner/scannerparser.c"
    break;

  case 141:
#line 1096 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		/* reset flag before the first enum value */
		is_bitfield = FALSE;
	  }
#line 3229 "giscanner/scannerparser.c"
    break;

  case 142:
#line 1101 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
            (yyvsp[0].symbol)->private = scanner->private;
            (yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 3238 "giscanner/scannerparser.c"
    break;

  case 143:
#line 1106 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
            (yyvsp[0].symbol)->private = scanner->private;
            (yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 3247 "giscanner/scannerparser.c"
    break;

  case 144:
#line 1114 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_OBJECT, scanner->current_file, lineno);
		(yyval.symbol)->ident = (yyvsp[0].str);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = ++last_enum_value;
		g_hash_table_insert (scanner->const_table, g_strdup ((yyval.symbol)->ident), gi_source_symbol_ref ((yyval.symbol)));
	  }
#line 3259 "giscanner/scannerparser.c"
    break;

  case 145:
#line 1122 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_OBJECT, scanner->current_file, lineno);
		(yyval.symbol)->ident = (yyvsp[-2].str);
		(yyval.symbol)->const_int_set = TRUE;
		(yyval.symbol)->const_int = (yyvsp[0].symbol)->const_int;
		last_enum_value = (yyval.symbol)->const_int;
		g_hash_table_insert (scanner->const_table, g_strdup ((yyval.symbol)->ident), gi_source_symbol_ref ((yyval.symbol)));
	  }
#line 3272 "giscanner/scannerparser.c"
    break;

  case 146:
#line 1134 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_CONST;
	  }
#line 3280 "giscanner/scannerparser.c"
    break;

  case 147:
#line 1138 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_RESTRICT;
	  }
#line 3288 "giscanner/scannerparser.c"
    break;

  case 148:
#line 1142 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_EXTENSION;
	  }
#line 3296 "giscanner/scannerparser.c"
    break;

  case 149:
#line 1146 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.type_qualifier) = TYPE_QUALIFIER_VOLATILE;
	  }
#line 3304 "giscanner/scannerparser.c"
    break;

  case 150:
#line 1153 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.function_specifier) = FUNCTION_INLINE;
	  }
#line 3312 "giscanner/scannerparser.c"
    break;

  case 151:
#line 1160 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3321 "giscanner/scannerparser.c"
    break;

  case 153:
#line 1169 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		(yyval.symbol)->ident = (yyvsp[0].str);
	  }
#line 3330 "giscanner/scannerparser.c"
    break;

  case 154:
#line 1174 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
	  }
#line 3338 "giscanner/scannerparser.c"
    break;

  case 155:
#line 1178 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new ((yyvsp[-1].symbol)));
	  }
#line 3347 "giscanner/scannerparser.c"
    break;

  case 156:
#line 1183 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new (NULL));
	  }
#line 3356 "giscanner/scannerparser.c"
    break;

  case 157:
#line 1188 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		// ignore (void) parameter list
		if ((yyvsp[-1].list) != NULL && ((yyvsp[-1].list)->next != NULL || ((GISourceSymbol *) (yyvsp[-1].list)->data)->base_type->type != CTYPE_VOID)) {
			func->child_list = (yyvsp[-1].list);
		}
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3370 "giscanner/scannerparser.c"
    break;

  case 158:
#line 1198 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		func->child_list = (yyvsp[-1].list);
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3381 "giscanner/scannerparser.c"
    break;

  case 159:
#line 1205 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3391 "giscanner/scannerparser.c"
    break;

  case 160:
#line 1214 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_pointer_new (NULL);
		(yyval.ctype)->type_qualifier = (yyvsp[0].type_qualifier);
	  }
#line 3400 "giscanner/scannerparser.c"
    break;

  case 161:
#line 1219 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.ctype) = gi_source_pointer_new (NULL);
	  }
#line 3408 "giscanner/scannerparser.c"
    break;

  case 162:
#line 1223 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType **base = &((yyvsp[0].ctype)->base_type);

		while (*base != NULL) {
			base = &((*base)->base_type);
		}
		*base = gi_source_pointer_new (NULL);
		(*base)->type_qualifier = (yyvsp[-1].type_qualifier);
		(yyval.ctype) = (yyvsp[0].ctype);
	  }
#line 3423 "giscanner/scannerparser.c"
    break;

  case 163:
#line 1234 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType **base = &((yyvsp[0].ctype)->base_type);

		while (*base != NULL) {
			base = &((*base)->base_type);
		}
		*base = gi_source_pointer_new (NULL);
		(yyval.ctype) = (yyvsp[0].ctype);
	  }
#line 3437 "giscanner/scannerparser.c"
    break;

  case 165:
#line 1248 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.type_qualifier) = (yyvsp[-1].type_qualifier) | (yyvsp[0].type_qualifier);
	  }
#line 3445 "giscanner/scannerparser.c"
    break;

  case 166:
#line 1255 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_append (NULL, (yyvsp[0].symbol));
	  }
#line 3453 "giscanner/scannerparser.c"
    break;

  case 167:
#line 1259 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.list) = g_list_append ((yyvsp[-2].list), (yyvsp[0].symbol));
	  }
#line 3461 "giscanner/scannerparser.c"
    break;

  case 168:
#line 1266 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3470 "giscanner/scannerparser.c"
    break;

  case 169:
#line 1271 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3479 "giscanner/scannerparser.c"
    break;

  case 170:
#line 1276 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		(yyval.symbol)->base_type = (yyvsp[0].ctype);
	  }
#line 3488 "giscanner/scannerparser.c"
    break;

  case 171:
#line 1281 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_ELLIPSIS, scanner->current_file, lineno);
	  }
#line 3496 "giscanner/scannerparser.c"
    break;

  case 172:
#line 1288 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceSymbol *sym = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		sym->ident = (yyvsp[0].str);
		(yyval.list) = g_list_append (NULL, sym);
	  }
#line 3506 "giscanner/scannerparser.c"
    break;

  case 173:
#line 1294 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceSymbol *sym = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		sym->ident = (yyvsp[0].str);
		(yyval.list) = g_list_append ((yyvsp[-2].list), sym);
	  }
#line 3516 "giscanner/scannerparser.c"
    break;

  case 176:
#line 1308 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[0].ctype));
	  }
#line 3525 "giscanner/scannerparser.c"
    break;

  case 178:
#line 1314 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[0].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), (yyvsp[-1].ctype));
	  }
#line 3534 "giscanner/scannerparser.c"
    break;

  case 179:
#line 1322 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-1].symbol);
	  }
#line 3542 "giscanner/scannerparser.c"
    break;

  case 180:
#line 1326 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new (NULL));
	  }
#line 3551 "giscanner/scannerparser.c"
    break;

  case 181:
#line 1331 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new ((yyvsp[-1].symbol)));
	  }
#line 3560 "giscanner/scannerparser.c"
    break;

  case 182:
#line 1336 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new (NULL));
	  }
#line 3569 "giscanner/scannerparser.c"
    break;

  case 183:
#line 1341 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), gi_source_array_new ((yyvsp[-1].symbol)));
	  }
#line 3578 "giscanner/scannerparser.c"
    break;

  case 184:
#line 1346 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3588 "giscanner/scannerparser.c"
    break;

  case 185:
#line 1352 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		// ignore (void) parameter list
		if ((yyvsp[-1].list) != NULL && ((yyvsp[-1].list)->next != NULL || ((GISourceSymbol *) (yyvsp[-1].list)->data)->base_type->type != CTYPE_VOID)) {
			func->child_list = (yyvsp[-1].list);
		}
		(yyval.symbol) = gi_source_symbol_new (CSYMBOL_TYPE_INVALID, scanner->current_file, lineno);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3602 "giscanner/scannerparser.c"
    break;

  case 186:
#line 1362 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		(yyval.symbol) = (yyvsp[-2].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3612 "giscanner/scannerparser.c"
    break;

  case 187:
#line 1368 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		GISourceType *func = gi_source_function_new ();
		// ignore (void) parameter list
		if ((yyvsp[-1].list) != NULL && ((yyvsp[-1].list)->next != NULL || ((GISourceSymbol *) (yyvsp[-1].list)->data)->base_type->type != CTYPE_VOID)) {
			func->child_list = (yyvsp[-1].list);
		}
		(yyval.symbol) = (yyvsp[-3].symbol);
		gi_source_symbol_merge_type ((yyval.symbol), func);
	  }
#line 3626 "giscanner/scannerparser.c"
    break;

  case 188:
#line 1381 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.str) = g_strdup (yytext);
	  }
#line 3634 "giscanner/scannerparser.c"
    break;

  case 239:
#line 1489 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.str) = g_strdup (yytext + strlen ("#define "));
	  }
#line 3642 "giscanner/scannerparser.c"
    break;

  case 240:
#line 1496 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		(yyval.str) = g_strdup (yytext + strlen ("#define "));
	  }
#line 3650 "giscanner/scannerparser.c"
    break;

  case 242:
#line 1507 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		if ((yyvsp[0].symbol)->const_int_set || (yyvsp[0].symbol)->const_boolean_set || (yyvsp[0].symbol)->const_double_set || (yyvsp[0].symbol)->const_string != NULL) {
			GISourceSymbol *macro = gi_source_symbol_copy ((yyvsp[0].symbol));
			g_free (macro->ident);
			macro->ident = (yyvsp[-1].str);
			gi_source_scanner_add_symbol (scanner, macro);
			gi_source_symbol_unref (macro);
			gi_source_symbol_unref ((yyvsp[0].symbol));
		} else {
			g_free ((yyvsp[-1].str));
			gi_source_symbol_unref ((yyvsp[0].symbol));
		}
	  }
#line 3668 "giscanner/scannerparser.c"
    break;

  case 243:
#line 1524 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		push_conditional (scanner, FOR_GI_SCANNER);
		update_skipping (scanner);
	  }
#line 3677 "giscanner/scannerparser.c"
    break;

  case 244:
#line 1529 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		push_conditional (scanner, NOT_GI_SCANNER);
		update_skipping (scanner);
	  }
#line 3686 "giscanner/scannerparser.c"
    break;

  case 245:
#line 1534 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
	 	warn_if_cond_has_gi_scanner (scanner, yytext);
		push_conditional (scanner, IRRELEVANT);
	  }
#line 3695 "giscanner/scannerparser.c"
    break;

  case 246:
#line 1539 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		warn_if_cond_has_gi_scanner (scanner, yytext);
		push_conditional (scanner, IRRELEVANT);
	  }
#line 3704 "giscanner/scannerparser.c"
    break;

  case 247:
#line 1544 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		warn_if_cond_has_gi_scanner (scanner, yytext);
		push_conditional (scanner, IRRELEVANT);
	  }
#line 3713 "giscanner/scannerparser.c"
    break;

  case 248:
#line 1549 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		warn_if_cond_has_gi_scanner (scanner, yytext);
		pop_conditional (scanner);
		push_conditional (scanner, IRRELEVANT);
		update_skipping (scanner);
	  }
#line 3724 "giscanner/scannerparser.c"
    break;

  case 249:
#line 1556 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		toggle_conditional (scanner);
		update_skipping (scanner);
	  }
#line 3733 "giscanner/scannerparser.c"
    break;

  case 250:
#line 1561 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"
    {
		pop_conditional (scanner);
		update_skipping (scanner);
	  }
#line 3742 "giscanner/scannerparser.c"
    break;


#line 3746 "giscanner/scannerparser.c"

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
      yyerror (scanner, YY_("syntax error"));
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
        yyerror (scanner, yymsgp);
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
                      yytoken, &yylval, scanner);
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
                  yystos[yystate], yyvsp, scanner);
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
  yyerror (scanner, YY_("memory exhausted"));
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
                  yytoken, &yylval, scanner);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner);
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
#line 1574 "../gobject-introspection-1.60.2/giscanner/scannerparser.y"

static void
yyerror (GISourceScanner *scanner, const char *s)
{
  /* ignore errors while doing a macro scan as not all object macros
   * have valid expressions */
  if (!scanner->macro_scan)
    {
      gchar *error = g_strdup_printf ("%s:%d: %s in '%s' at '%s'",
          g_file_get_parse_name (scanner->current_file), lineno, s, linebuf, yytext);
      g_ptr_array_add (scanner->errors, error);
    }
}

static int
eat_hspace (FILE * f)
{
  int c;
  do
    {
      c = fgetc (f);
    }
  while (c == ' ' || c == '\t');
  return c;
}

static int
pass_line (FILE * f, int c,
           FILE *out)
{
  while (c != EOF && c != '\n')
    {
      if (out)
        fputc (c, out);
      c = fgetc (f);
    }
  if (c == '\n')
    {
      if (out)
        fputc (c, out);
      c = fgetc (f);
      if (c == ' ' || c == '\t')
        {
          c = eat_hspace (f);
        }
    }
  return c;
}

static int
eat_line (FILE * f, int c)
{
  return pass_line (f, c, NULL);
}

static int
read_identifier (FILE * f, int c, char **identifier)
{
  GString *id = g_string_new ("");
  while (g_ascii_isalnum (c) || c == '_')
    {
      g_string_append_c (id, c);
      c = fgetc (f);
    }
  *identifier = g_string_free (id, FALSE);
  return c;
}

static gboolean
parse_file (GISourceScanner *scanner, FILE *file)
{
  g_return_val_if_fail (file != NULL, FALSE);

  lineno = 1;
  yyin = file;
  yyparse (scanner);
  yyin = NULL;

  return TRUE;
}

void
gi_source_scanner_parse_macros (GISourceScanner *scanner, GList *filenames)
{
  GError *error = NULL;
  char *tmp_name = NULL;
  FILE *fmacros =
    fdopen (g_file_open_tmp ("gen-introspect-XXXXXX.h", &tmp_name, &error),
            "w+");
  GList *l;

  for (l = filenames; l != NULL; l = l->next)
    {
      FILE *f = fopen (l->data, "r");
      int line = 1;

      GString *define_line;
      char *str;
      gboolean error_line = FALSE;
      gboolean end_of_word;
      int c = eat_hspace (f);
      while (c != EOF)
        {
          if (c != '#')
            {
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }

          /* print current location */
          str = g_strescape (l->data, "");
          fprintf (fmacros, "# %d \"%s\"\n", line, str);
          g_free (str);

          c = eat_hspace (f);
          c = read_identifier (f, c, &str);
          end_of_word = (c == ' ' || c == '\t' || c == '\n' || c == EOF);
          if (end_of_word &&
              (g_str_equal (str, "if") ||
               g_str_equal (str, "endif") ||
               g_str_equal (str, "ifndef") ||
               g_str_equal (str, "ifdef") ||
               g_str_equal (str, "else") ||
               g_str_equal (str, "elif")))
            {
              fprintf (fmacros, "#%s ", str);
              g_free (str);
              c = pass_line (f, c, fmacros);
              line++;
              continue;
            }
          else if (strcmp (str, "define") != 0 || !end_of_word)
            {
              g_free (str);
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          g_free (str);
          c = eat_hspace (f);
          c = read_identifier (f, c, &str);
          if (strlen (str) == 0 || (c != ' ' && c != '\t' && c != '('))
            {
              g_free (str);
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          define_line = g_string_new ("#define ");
          g_string_append (define_line, str);
          g_free (str);
          if (c == '(')
            {
              while (c != ')')
                {
                  g_string_append_c (define_line, c);
                  c = fgetc (f);
                  if (c == EOF || c == '\n')
                    {
                      error_line = TRUE;
                      break;
                    }
                }
              if (error_line)
                {
                  g_string_free (define_line, TRUE);
                  /* ignore line */
                  c = eat_line (f, c);
                  line++;
                  continue;
                }

              g_assert (c == ')');
              g_string_append_c (define_line, c);
              c = fgetc (f);

              /* found function-like macro */
              fprintf (fmacros, "%s\n", define_line->str);

              g_string_free (define_line, TRUE);
              /* ignore rest of line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          if (c != ' ' && c != '\t')
            {
              g_string_free (define_line, TRUE);
              /* ignore line */
              c = eat_line (f, c);
              line++;
              continue;
            }
          while (c != EOF && c != '\n')
            {
              g_string_append_c (define_line, c);
              c = fgetc (f);
              if (c == '\\')
                {
                  c = fgetc (f);
                  if (c == '\n')
                    {
                      /* fold lines when seeing backslash new-line sequence */
                      c = fgetc (f);
                    }
                  else
                    {
                      g_string_append_c (define_line, '\\');
                    }
                }
            }

          /* found object-like macro */
          fprintf (fmacros, "%s\n", define_line->str);

          c = eat_line (f, c);
          line++;
        }

      fclose (f);
    }

  rewind (fmacros);
  parse_file (scanner, fmacros);
  fclose (fmacros);
  g_unlink (tmp_name);
}

gboolean
gi_source_scanner_parse_file (GISourceScanner *scanner, const gchar *filename)
{
  FILE *file;
  gboolean result;

  file = g_fopen (filename, "r");
  result = parse_file (scanner, file);
  fclose (file);

  return result;
}

gboolean
gi_source_scanner_lex_filename (GISourceScanner *scanner, const gchar *filename)
{
  lineno = 1;
  yyin = g_fopen (filename, "r");

  while (yylex (scanner) != YYEOF)
    ;

  fclose (yyin);

  return TRUE;
}
