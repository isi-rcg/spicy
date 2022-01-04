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
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         ematch_parse
#define yylex           ematch_lex
#define yyerror         ematch_error
#define yydebug         ematch_debug
#define yynerrs         ematch_nerrs


/* First part of user prologue.  */
#line 12 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"

#include <netlink-private/netlink.h>
#include <netlink-private/tc.h>
#include <netlink/netlink.h>
#include <netlink/utils.h>
#include <netlink/route/pktloc.h>
#include <netlink/route/cls/ematch.h>
#include <netlink/route/cls/ematch/cmp.h>
#include <netlink/route/cls/ematch/nbyte.h>
#include <netlink/route/cls/ematch/text.h>
#include <netlink/route/cls/ematch/meta.h>
#include <linux/tc_ematch/tc_em_meta.h>
#include <linux/tc_ematch/tc_em_cmp.h>

#define META_ALLOC rtnl_meta_value_alloc_id
#define META_ID(name) TCF_META_ID_##name
#define META_INT TCF_META_TYPE_INT
#define META_VAR TCF_META_TYPE_VAR

#line 96 "lib/route/cls/ematch_syntax.c"

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
#ifndef YY_EMATCH_LIB_ROUTE_CLS_EMATCH_SYNTAX_H_INCLUDED
# define YY_EMATCH_LIB_ROUTE_CLS_EMATCH_SYNTAX_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int ematch_debug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ERROR = 258,
    LOGIC = 259,
    NOT = 260,
    OPERAND = 261,
    NUMBER = 262,
    ALIGN = 263,
    LAYER = 264,
    KW_OPEN = 265,
    KW_CLOSE = 266,
    KW_PLUS = 267,
    KW_MASK = 268,
    KW_SHIFT = 269,
    KW_AT = 270,
    EMATCH_CMP = 271,
    EMATCH_NBYTE = 272,
    EMATCH_TEXT = 273,
    EMATCH_META = 274,
    KW_EQ = 275,
    KW_GT = 276,
    KW_LT = 277,
    KW_FROM = 278,
    KW_TO = 279,
    META_RANDOM = 280,
    META_LOADAVG_0 = 281,
    META_LOADAVG_1 = 282,
    META_LOADAVG_2 = 283,
    META_DEV = 284,
    META_PRIO = 285,
    META_PROTO = 286,
    META_PKTTYPE = 287,
    META_PKTLEN = 288,
    META_DATALEN = 289,
    META_MACLEN = 290,
    META_MARK = 291,
    META_TCINDEX = 292,
    META_RTCLASSID = 293,
    META_RTIIF = 294,
    META_SK_FAMILY = 295,
    META_SK_STATE = 296,
    META_SK_REUSE = 297,
    META_SK_REFCNT = 298,
    META_SK_RCVBUF = 299,
    META_SK_SNDBUF = 300,
    META_SK_SHUTDOWN = 301,
    META_SK_PROTO = 302,
    META_SK_TYPE = 303,
    META_SK_RMEM_ALLOC = 304,
    META_SK_WMEM_ALLOC = 305,
    META_SK_WMEM_QUEUED = 306,
    META_SK_RCV_QLEN = 307,
    META_SK_SND_QLEN = 308,
    META_SK_ERR_QLEN = 309,
    META_SK_FORWARD_ALLOCS = 310,
    META_SK_ALLOCS = 311,
    META_SK_ROUTE_CAPS = 312,
    META_SK_HASH = 313,
    META_SK_LINGERTIME = 314,
    META_SK_ACK_BACKLOG = 315,
    META_SK_MAX_ACK_BACKLOG = 316,
    META_SK_PRIO = 317,
    META_SK_RCVLOWAT = 318,
    META_SK_RCVTIMEO = 319,
    META_SK_SNDTIMEO = 320,
    META_SK_SENDMSG_OFF = 321,
    META_SK_WRITE_PENDING = 322,
    META_VLAN = 323,
    META_RXHASH = 324,
    META_DEVNAME = 325,
    META_SK_BOUND_IF = 326,
    STR = 327,
    QUOTED = 328
  };
#endif
/* Tokens.  */
#define ERROR 258
#define LOGIC 259
#define NOT 260
#define OPERAND 261
#define NUMBER 262
#define ALIGN 263
#define LAYER 264
#define KW_OPEN 265
#define KW_CLOSE 266
#define KW_PLUS 267
#define KW_MASK 268
#define KW_SHIFT 269
#define KW_AT 270
#define EMATCH_CMP 271
#define EMATCH_NBYTE 272
#define EMATCH_TEXT 273
#define EMATCH_META 274
#define KW_EQ 275
#define KW_GT 276
#define KW_LT 277
#define KW_FROM 278
#define KW_TO 279
#define META_RANDOM 280
#define META_LOADAVG_0 281
#define META_LOADAVG_1 282
#define META_LOADAVG_2 283
#define META_DEV 284
#define META_PRIO 285
#define META_PROTO 286
#define META_PKTTYPE 287
#define META_PKTLEN 288
#define META_DATALEN 289
#define META_MACLEN 290
#define META_MARK 291
#define META_TCINDEX 292
#define META_RTCLASSID 293
#define META_RTIIF 294
#define META_SK_FAMILY 295
#define META_SK_STATE 296
#define META_SK_REUSE 297
#define META_SK_REFCNT 298
#define META_SK_RCVBUF 299
#define META_SK_SNDBUF 300
#define META_SK_SHUTDOWN 301
#define META_SK_PROTO 302
#define META_SK_TYPE 303
#define META_SK_RMEM_ALLOC 304
#define META_SK_WMEM_ALLOC 305
#define META_SK_WMEM_QUEUED 306
#define META_SK_RCV_QLEN 307
#define META_SK_SND_QLEN 308
#define META_SK_ERR_QLEN 309
#define META_SK_FORWARD_ALLOCS 310
#define META_SK_ALLOCS 311
#define META_SK_ROUTE_CAPS 312
#define META_SK_HASH 313
#define META_SK_LINGERTIME 314
#define META_SK_ACK_BACKLOG 315
#define META_SK_MAX_ACK_BACKLOG 316
#define META_SK_PRIO 317
#define META_SK_RCVLOWAT 318
#define META_SK_RCVTIMEO 319
#define META_SK_SNDTIMEO 320
#define META_SK_SENDMSG_OFF 321
#define META_SK_WRITE_PENDING 322
#define META_VLAN 323
#define META_RXHASH 324
#define META_DEVNAME 325
#define META_SK_BOUND_IF 326
#define STR 327
#define QUOTED 328

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 41 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"

	struct tcf_em_cmp	cmp;
	struct ematch_quoted	q;
	struct rtnl_ematch *	e;
	struct rtnl_pktloc *	loc;
	struct rtnl_meta_value *mv;
	uint32_t		i;
	uint64_t		i64;
	char *			s;

#line 296 "lib/route/cls/ematch_syntax.c"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int ematch_parse (void *scanner, char **errp, struct nl_list_head *root);

#endif /* !YY_EMATCH_LIB_ROUTE_CLS_EMATCH_SYNTAX_H_INCLUDED  */

/* Second part of user prologue.  */
#line 52 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"

extern int ematch_lex(YYSTYPE *, void *);

static void yyerror(void *scanner, char **errp, struct nl_list_head *root, const char *msg)
{
	if (msg)
		*errp = strdup(msg);
	else
		*errp = NULL;
}

#line 323 "lib/route/cls/ematch_syntax.c"


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
#define YYFINAL  26
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   138

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  74
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  18
/* YYNRULES -- Number of rules.  */
#define YYNRULES  84
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  118

#define YYUNDEFTOK  2
#define YYMAXUTOK   328

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
      65,    66,    67,    68,    69,    70,    71,    72,    73
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   150,   150,   152,   159,   163,   175,   180,   188,   203,
     221,   248,   267,   295,   297,   302,   323,   324,   330,   331,
     336,   338,   340,   342,   347,   348,   349,   350,   351,   352,
     353,   354,   355,   356,   357,   358,   359,   360,   361,   362,
     363,   364,   365,   366,   367,   368,   369,   370,   371,   372,
     373,   374,   375,   376,   377,   378,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,   389,   390,   391,   395,
     396,   403,   407,   436,   449,   475,   476,   478,   484,   485,
     491,   492,   497,   499,   501
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 1
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ERROR", "LOGIC", "NOT", "OPERAND",
  "NUMBER", "ALIGN", "LAYER", "\"(\"", "\")\"", "\"+\"", "\"mask\"",
  "\">>\"", "\"at\"", "\"cmp\"", "\"pattern\"", "\"text\"", "\"meta\"",
  "\"=\"", "\">\"", "\"<\"", "\"from\"", "\"to\"", "\"random\"",
  "\"loadavg_0\"", "\"loadavg_1\"", "\"loadavg_2\"", "\"dev\"", "\"prio\"",
  "\"proto\"", "\"pkttype\"", "\"pktlen\"", "\"datalen\"", "\"maclen\"",
  "\"mark\"", "\"tcindex\"", "\"rtclassid\"", "\"rtiif\"", "\"sk_family\"",
  "\"sk_state\"", "\"sk_reuse\"", "\"sk_refcnt\"", "\"sk_rcvbuf\"",
  "\"sk_sndbuf\"", "\"sk_shutdown\"", "\"sk_proto\"", "\"sk_type\"",
  "\"sk_rmem_alloc\"", "\"sk_wmem_alloc\"", "\"sk_wmem_queued\"",
  "\"sk_rcv_qlen\"", "\"sk_snd_qlen\"", "\"sk_err_qlen\"",
  "\"sk_forward_allocs\"", "\"sk_allocs\"", "\"sk_route_caps\"",
  "\"sk_hash\"", "\"sk_lingertime\"", "\"sk_ack_backlog\"",
  "\"sk_max_ack_backlog\"", "\"sk_prio\"", "\"sk_rcvlowat\"",
  "\"sk_rcvtimeo\"", "\"sk_sndtimeo\"", "\"sk_sendmsg_off\"",
  "\"sk_write_pending\"", "\"vlan\"", "\"rxhash\"", "\"devname\"",
  "\"sk_bound_if\"", "STR", "QUOTED", "$accept", "input", "expr", "match",
  "ematch", "cmp_match", "cmp_expr", "text_from", "text_to", "meta_value",
  "meta_int_id", "meta_var_id", "pattern", "pktloc", "align", "mask",
  "shift", "operand", YY_NULLPTR
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
     325,   326,   327,   328
};
# endif

#define YYPACT_NINF -63

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-63)))

#define YYTABLE_NINF -76

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -4,    15,   -13,    -8,    11,    10,    14,    25,    29,   -63,
      26,   -63,    37,   -63,   -63,   -63,    16,    33,   -63,   -63,
     -63,    32,     1,     1,   -28,    65,   -63,    11,   -63,   -63,
     -63,    38,    34,   -63,    36,    28,   -24,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,   -63,
     -63,   -63,   -63,   -63,   -63,   -63,    16,    39,    39,   -63,
     -63,    43,   -63,   -62,    31,    65,    44,    42,   -63,    42,
     -63,   -63,    41,     1,    35,    45,   -63,    50,   -63,   -63,
     -63,   -63,     1,    47,   -63,   -63,   -63,   -63
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,    75,     0,     0,    75,     0,     0,     0,     0,    73,
       0,     3,     4,     7,     8,    14,     0,     0,     6,    77,
      76,     0,    75,    75,     0,     0,     1,    75,    82,    83,
      84,     0,     0,    12,     0,     0,     0,    21,    24,    25,
      26,    27,    28,    29,    30,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,    67,    68,    69,    70,    20,     0,    80,    80,     5,
      15,     0,    13,     0,    16,     0,     0,    78,    23,    78,
      72,    71,     0,    75,    18,     0,    81,     0,    22,    74,
       9,    17,    75,     0,    11,    79,    19,    10
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -63,   -63,    13,   -63,    59,   -63,    40,   -63,   -63,   -34,
     -63,   -63,   -63,   -23,   -63,   -36,   -22,   -21
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,    10,    11,    12,    13,    14,    15,   104,   113,    86,
      87,    88,   102,    16,    17,   108,    97,    31
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
      35,     1,    19,     2,     3,   -75,     4,    20,     2,     3,
     100,   101,     5,     6,     7,     8,     1,    21,     2,     3,
      22,     4,     2,     3,    23,     4,    26,     5,     6,     7,
       8,     5,     6,     7,     8,    24,    28,    29,    30,    25,
      89,    27,    32,    33,    36,    90,    91,    92,    93,    94,
      99,   106,   110,    96,   103,   107,   114,   115,   117,   112,
      18,   105,    34,   109,     0,    95,    98,     0,     9,     0,
       0,     0,    37,     9,     0,     0,     0,     0,     0,     0,
     111,     0,     0,     9,     0,     0,     0,     9,     0,   116,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,     0,    85
};

static const yytype_int8 yycheck[] =
{
      23,     5,    15,     7,     8,     9,    10,    15,     7,     8,
      72,    73,    16,    17,    18,    19,     5,     4,     7,     8,
      10,    10,     7,     8,    10,    10,     0,    16,    17,    18,
      19,    16,    17,    18,    19,    10,    20,    21,    22,    10,
      27,     4,     9,    11,    72,     7,    12,    11,    20,    73,
       7,     7,    11,    14,    23,    13,    11,     7,    11,    24,
       1,    95,    22,    99,    -1,    86,    88,    -1,    72,    -1,
      -1,    -1,     7,    72,    -1,    -1,    -1,    -1,    -1,    -1,
     103,    -1,    -1,    72,    -1,    -1,    -1,    72,    -1,   112,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    -1,    73
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     7,     8,    10,    16,    17,    18,    19,    72,
      75,    76,    77,    78,    79,    80,    87,    88,    78,    15,
      15,    76,    10,    10,    10,    10,     0,     4,    20,    21,
      22,    91,     9,    11,    80,    87,    72,     7,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    44,    45,    46,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    73,    83,    84,    85,    76,
       7,    12,    11,    20,    73,    91,    14,    90,    90,     7,
      72,    73,    86,    23,    81,    83,     7,    13,    89,    89,
      11,    87,    24,    82,    11,     7,    87,    11
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    74,    75,    75,    76,    76,    77,    77,    78,    78,
      78,    78,    78,    79,    79,    80,    81,    81,    82,    82,
      83,    83,    83,    83,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    85,
      85,    86,    86,    87,    87,    88,    88,    88,    89,    89,
      90,    90,    91,    91,    91
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     1,     1,     3,     2,     1,     1,     6,
       7,     6,     3,     4,     1,     3,     0,     2,     0,     2,
       1,     1,     3,     2,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     5,     0,     2,     2,     0,     2,
       0,     2,     1,     1,     1
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
        yyerror (scanner, errp, root, YY_("syntax error: cannot back up")); \
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
                  Type, Value, scanner, errp, root); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner, char **errp, struct nl_list_head *root)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (scanner);
  YYUSE (errp);
  YYUSE (root);
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
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, void *scanner, char **errp, struct nl_list_head *root)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, scanner, errp, root);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, void *scanner, char **errp, struct nl_list_head *root)
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
                                              , scanner, errp, root);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, scanner, errp, root); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, void *scanner, char **errp, struct nl_list_head *root)
{
  YYUSE (yyvaluep);
  YYUSE (scanner);
  YYUSE (errp);
  YYUSE (root);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  switch (yytype)
    {
    case 72: /* STR  */
#line 141 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { free(((*yyvaluep).s)); NL_DBG(2, "string destructor\n"); }
#line 1257 "lib/route/cls/ematch_syntax.c"
        break;

    case 73: /* QUOTED  */
#line 143 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { free(((*yyvaluep).q).data); NL_DBG(2, "quoted destructor\n"); }
#line 1263 "lib/route/cls/ematch_syntax.c"
        break;

    case 81: /* text_from  */
#line 142 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { rtnl_pktloc_put(((*yyvaluep).loc)); NL_DBG(2, "pktloc destructor\n"); }
#line 1269 "lib/route/cls/ematch_syntax.c"
        break;

    case 82: /* text_to  */
#line 142 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { rtnl_pktloc_put(((*yyvaluep).loc)); NL_DBG(2, "pktloc destructor\n"); }
#line 1275 "lib/route/cls/ematch_syntax.c"
        break;

    case 83: /* meta_value  */
#line 144 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { rtnl_meta_value_put(((*yyvaluep).mv)); NL_DBG(2, "meta value destructor\n"); }
#line 1281 "lib/route/cls/ematch_syntax.c"
        break;

    case 86: /* pattern  */
#line 143 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { free(((*yyvaluep).q).data); NL_DBG(2, "quoted destructor\n"); }
#line 1287 "lib/route/cls/ematch_syntax.c"
        break;

    case 87: /* pktloc  */
#line 142 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
      { rtnl_pktloc_put(((*yyvaluep).loc)); NL_DBG(2, "pktloc destructor\n"); }
#line 1293 "lib/route/cls/ematch_syntax.c"
        break;

      default:
        break;
    }
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void *scanner, char **errp, struct nl_list_head *root)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

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
      yychar = yylex (&yylval, scanner);
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
#line 153 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			nl_list_add_tail(root, &(yyvsp[0].e)->e_list);
		}
#line 1565 "lib/route/cls/ematch_syntax.c"
    break;

  case 4:
#line 160 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			(yyval.e) = (yyvsp[0].e);
		}
#line 1573 "lib/route/cls/ematch_syntax.c"
    break;

  case 5:
#line 164 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			rtnl_ematch_set_flags((yyvsp[-2].e), (yyvsp[-1].i));

			/* make ematch new head */
			nl_list_add_tail(&(yyvsp[-2].e)->e_list, &(yyvsp[0].e)->e_list);

			(yyval.e) = (yyvsp[-2].e);
		}
#line 1586 "lib/route/cls/ematch_syntax.c"
    break;

  case 6:
#line 176 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			rtnl_ematch_set_flags((yyvsp[0].e), TCF_EM_INVERT);
			(yyval.e) = (yyvsp[0].e);
		}
#line 1595 "lib/route/cls/ematch_syntax.c"
    break;

  case 7:
#line 181 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			(yyval.e) = (yyvsp[0].e);
		}
#line 1603 "lib/route/cls/ematch_syntax.c"
    break;

  case 8:
#line 189 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_CMP) < 0)
				BUG();

			rtnl_ematch_cmp_set(e, &(yyvsp[0].cmp));
			(yyval.e) = e;
		}
#line 1622 "lib/route/cls/ematch_syntax.c"
    break;

  case 9:
#line 204 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_NBYTE) < 0)
				BUG();

			rtnl_ematch_nbyte_set_offset(e, (yyvsp[-3].loc)->layer, (yyvsp[-3].loc)->offset);
			rtnl_pktloc_put((yyvsp[-3].loc));
			rtnl_ematch_nbyte_set_pattern(e, (uint8_t *) (yyvsp[-1].q).data, (yyvsp[-1].q).index);

			(yyval.e) = e;
		}
#line 1644 "lib/route/cls/ematch_syntax.c"
    break;

  case 10:
#line 222 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_TEXT) < 0)
				BUG();

			rtnl_ematch_text_set_algo(e, (yyvsp[-4].s));
			rtnl_ematch_text_set_pattern(e, (yyvsp[-3].q).data, (yyvsp[-3].q).index);

			if ((yyvsp[-2].loc)) {
				rtnl_ematch_text_set_from(e, (yyvsp[-2].loc)->layer, (yyvsp[-2].loc)->offset);
				rtnl_pktloc_put((yyvsp[-2].loc));
			}

			if ((yyvsp[-1].loc)) {
				rtnl_ematch_text_set_to(e, (yyvsp[-1].loc)->layer, (yyvsp[-1].loc)->offset);
				rtnl_pktloc_put((yyvsp[-1].loc));
			}

			(yyval.e) = e;
		}
#line 1675 "lib/route/cls/ematch_syntax.c"
    break;

  case 11:
#line 249 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_META) < 0)
				BUG();

			rtnl_ematch_meta_set_lvalue(e, (yyvsp[-3].mv));
			rtnl_ematch_meta_set_rvalue(e, (yyvsp[-1].mv));
			rtnl_ematch_meta_set_operand(e, (yyvsp[-2].i));

			(yyval.e) = e;
		}
#line 1697 "lib/route/cls/ematch_syntax.c"
    break;

  case 12:
#line 268 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_ematch *e;

			if (!(e = rtnl_ematch_alloc())) {
				*errp = strdup("Unable to allocate ematch object");
				YYABORT;
			}

			if (rtnl_ematch_set_kind(e, TCF_EM_CONTAINER) < 0)
				BUG();

			/* Make e->childs the list head of a the ematch sequence */
			nl_list_add_tail(&e->e_childs, &(yyvsp[-1].e)->e_list);

			(yyval.e) = e;
		}
#line 1718 "lib/route/cls/ematch_syntax.c"
    break;

  case 13:
#line 296 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.cmp) = (yyvsp[-1].cmp); }
#line 1724 "lib/route/cls/ematch_syntax.c"
    break;

  case 14:
#line 298 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.cmp) = (yyvsp[0].cmp); }
#line 1730 "lib/route/cls/ematch_syntax.c"
    break;

  case 15:
#line 303 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			if ((yyvsp[-2].loc)->align == TCF_EM_ALIGN_U16 ||
			    (yyvsp[-2].loc)->align == TCF_EM_ALIGN_U32)
				(yyval.cmp).flags = TCF_EM_CMP_TRANS;

			memset(&(yyval.cmp), 0, sizeof((yyval.cmp)));

			(yyval.cmp).mask = (yyvsp[-2].loc)->mask;
			(yyval.cmp).off = (yyvsp[-2].loc)->offset;
			(yyval.cmp).align = (yyvsp[-2].loc)->align;
			(yyval.cmp).layer = (yyvsp[-2].loc)->layer;
			(yyval.cmp).opnd = (yyvsp[-1].i);
			(yyval.cmp).val = (yyvsp[0].i);

			rtnl_pktloc_put((yyvsp[-2].loc));
		}
#line 1751 "lib/route/cls/ematch_syntax.c"
    break;

  case 16:
#line 323 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.loc) = NULL; }
#line 1757 "lib/route/cls/ematch_syntax.c"
    break;

  case 17:
#line 325 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.loc) = (yyvsp[0].loc); }
#line 1763 "lib/route/cls/ematch_syntax.c"
    break;

  case 18:
#line 330 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.loc) = NULL; }
#line 1769 "lib/route/cls/ematch_syntax.c"
    break;

  case 19:
#line 332 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.loc) = (yyvsp[0].loc); }
#line 1775 "lib/route/cls/ematch_syntax.c"
    break;

  case 20:
#line 337 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.mv) = rtnl_meta_value_alloc_var((yyvsp[0].q).data, (yyvsp[0].q).len); }
#line 1781 "lib/route/cls/ematch_syntax.c"
    break;

  case 21:
#line 339 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.mv) = rtnl_meta_value_alloc_int((yyvsp[0].i)); }
#line 1787 "lib/route/cls/ematch_syntax.c"
    break;

  case 22:
#line 341 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.mv) = META_ALLOC(META_INT, (yyvsp[-2].i), (yyvsp[-1].i), (yyvsp[0].i64)); }
#line 1793 "lib/route/cls/ematch_syntax.c"
    break;

  case 23:
#line 343 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.mv) = META_ALLOC(META_VAR, (yyvsp[-1].i), (yyvsp[0].i), 0); }
#line 1799 "lib/route/cls/ematch_syntax.c"
    break;

  case 24:
#line 347 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RANDOM); }
#line 1805 "lib/route/cls/ematch_syntax.c"
    break;

  case 25:
#line 348 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(LOADAVG_0); }
#line 1811 "lib/route/cls/ematch_syntax.c"
    break;

  case 26:
#line 349 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(LOADAVG_1); }
#line 1817 "lib/route/cls/ematch_syntax.c"
    break;

  case 27:
#line 350 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(LOADAVG_2); }
#line 1823 "lib/route/cls/ematch_syntax.c"
    break;

  case 28:
#line 351 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(DEV); }
#line 1829 "lib/route/cls/ematch_syntax.c"
    break;

  case 29:
#line 352 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PRIORITY); }
#line 1835 "lib/route/cls/ematch_syntax.c"
    break;

  case 30:
#line 353 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PROTOCOL); }
#line 1841 "lib/route/cls/ematch_syntax.c"
    break;

  case 31:
#line 354 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PKTTYPE); }
#line 1847 "lib/route/cls/ematch_syntax.c"
    break;

  case 32:
#line 355 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(PKTLEN); }
#line 1853 "lib/route/cls/ematch_syntax.c"
    break;

  case 33:
#line 356 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(DATALEN); }
#line 1859 "lib/route/cls/ematch_syntax.c"
    break;

  case 34:
#line 357 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(MACLEN); }
#line 1865 "lib/route/cls/ematch_syntax.c"
    break;

  case 35:
#line 358 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(NFMARK); }
#line 1871 "lib/route/cls/ematch_syntax.c"
    break;

  case 36:
#line 359 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(TCINDEX); }
#line 1877 "lib/route/cls/ematch_syntax.c"
    break;

  case 37:
#line 360 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RTCLASSID); }
#line 1883 "lib/route/cls/ematch_syntax.c"
    break;

  case 38:
#line 361 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RTIIF); }
#line 1889 "lib/route/cls/ematch_syntax.c"
    break;

  case 39:
#line 362 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_FAMILY); }
#line 1895 "lib/route/cls/ematch_syntax.c"
    break;

  case 40:
#line 363 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_STATE); }
#line 1901 "lib/route/cls/ematch_syntax.c"
    break;

  case 41:
#line 364 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_REUSE); }
#line 1907 "lib/route/cls/ematch_syntax.c"
    break;

  case 42:
#line 365 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_REFCNT); }
#line 1913 "lib/route/cls/ematch_syntax.c"
    break;

  case 43:
#line 366 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCVBUF); }
#line 1919 "lib/route/cls/ematch_syntax.c"
    break;

  case 44:
#line 367 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SNDBUF); }
#line 1925 "lib/route/cls/ematch_syntax.c"
    break;

  case 45:
#line 368 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SHUTDOWN); }
#line 1931 "lib/route/cls/ematch_syntax.c"
    break;

  case 46:
#line 369 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_PROTO); }
#line 1937 "lib/route/cls/ematch_syntax.c"
    break;

  case 47:
#line 370 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_TYPE); }
#line 1943 "lib/route/cls/ematch_syntax.c"
    break;

  case 48:
#line 371 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RMEM_ALLOC); }
#line 1949 "lib/route/cls/ematch_syntax.c"
    break;

  case 49:
#line 372 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_WMEM_ALLOC); }
#line 1955 "lib/route/cls/ematch_syntax.c"
    break;

  case 50:
#line 373 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_WMEM_QUEUED); }
#line 1961 "lib/route/cls/ematch_syntax.c"
    break;

  case 51:
#line 374 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCV_QLEN); }
#line 1967 "lib/route/cls/ematch_syntax.c"
    break;

  case 52:
#line 375 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SND_QLEN); }
#line 1973 "lib/route/cls/ematch_syntax.c"
    break;

  case 53:
#line 376 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ERR_QLEN); }
#line 1979 "lib/route/cls/ematch_syntax.c"
    break;

  case 54:
#line 377 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_FORWARD_ALLOCS); }
#line 1985 "lib/route/cls/ematch_syntax.c"
    break;

  case 55:
#line 378 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ALLOCS); }
#line 1991 "lib/route/cls/ematch_syntax.c"
    break;

  case 56:
#line 379 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = __TCF_META_ID_SK_ROUTE_CAPS; }
#line 1997 "lib/route/cls/ematch_syntax.c"
    break;

  case 57:
#line 380 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_HASH); }
#line 2003 "lib/route/cls/ematch_syntax.c"
    break;

  case 58:
#line 381 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_LINGERTIME); }
#line 2009 "lib/route/cls/ematch_syntax.c"
    break;

  case 59:
#line 382 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_ACK_BACKLOG); }
#line 2015 "lib/route/cls/ematch_syntax.c"
    break;

  case 60:
#line 383 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_MAX_ACK_BACKLOG); }
#line 2021 "lib/route/cls/ematch_syntax.c"
    break;

  case 61:
#line 384 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_PRIO); }
#line 2027 "lib/route/cls/ematch_syntax.c"
    break;

  case 62:
#line 385 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCVLOWAT); }
#line 2033 "lib/route/cls/ematch_syntax.c"
    break;

  case 63:
#line 386 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_RCVTIMEO); }
#line 2039 "lib/route/cls/ematch_syntax.c"
    break;

  case 64:
#line 387 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SNDTIMEO); }
#line 2045 "lib/route/cls/ematch_syntax.c"
    break;

  case 65:
#line 388 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_SENDMSG_OFF); }
#line 2051 "lib/route/cls/ematch_syntax.c"
    break;

  case 66:
#line 389 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_WRITE_PENDING); }
#line 2057 "lib/route/cls/ematch_syntax.c"
    break;

  case 67:
#line 390 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(VLAN_TAG); }
#line 2063 "lib/route/cls/ematch_syntax.c"
    break;

  case 68:
#line 391 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(RXHASH); }
#line 2069 "lib/route/cls/ematch_syntax.c"
    break;

  case 69:
#line 395 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(DEV); }
#line 2075 "lib/route/cls/ematch_syntax.c"
    break;

  case 70:
#line 396 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = META_ID(SK_BOUND_IF); }
#line 2081 "lib/route/cls/ematch_syntax.c"
    break;

  case 71:
#line 404 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			(yyval.q) = (yyvsp[0].q);
		}
#line 2089 "lib/route/cls/ematch_syntax.c"
    break;

  case 72:
#line 408 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct nl_addr *addr;

			if (nl_addr_parse((yyvsp[0].s), AF_UNSPEC, &addr) == 0) {
				(yyval.q).len = nl_addr_get_len(addr);

				(yyval.q).index = min_t(int, (yyval.q).len, nl_addr_get_prefixlen(addr)/8);

				if (!((yyval.q).data = calloc(1, (yyval.q).len))) {
					nl_addr_put(addr);
					YYABORT;
				}

				memcpy((yyval.q).data, nl_addr_get_binary_addr(addr), (yyval.q).len);
				nl_addr_put(addr);
			} else {
				if (asprintf(errp, "invalid pattern \"%s\"", (yyvsp[0].s)) == -1)
					*errp = NULL;
				YYABORT;
			}
		}
#line 2115 "lib/route/cls/ematch_syntax.c"
    break;

  case 73:
#line 437 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_pktloc *loc;

			if (rtnl_pktloc_lookup((yyvsp[0].s), &loc) < 0) {
				if (asprintf(errp, "Packet location \"%s\" not found", (yyvsp[0].s)) == -1)
					*errp = NULL;
				YYABORT;
			}

			(yyval.loc) = loc;
		}
#line 2131 "lib/route/cls/ematch_syntax.c"
    break;

  case 74:
#line 450 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    {
			struct rtnl_pktloc *loc;

			if ((yyvsp[0].i64) && (!(yyvsp[-4].i) || (yyvsp[-4].i) > TCF_EM_ALIGN_U32)) {
				*errp = strdup("mask only allowed for alignments u8|u16|u32");
				YYABORT;
			}

			if (!(loc = rtnl_pktloc_alloc())) {
				*errp = strdup("Unable to allocate packet location object");
				YYABORT;
			}

			loc->name = strdup("<USER-DEFINED>");
			loc->align = (yyvsp[-4].i);
			loc->layer = (yyvsp[-3].i);
			loc->offset = (yyvsp[-1].i);
			loc->mask = (yyvsp[0].i64);

			(yyval.loc) = loc;
		}
#line 2157 "lib/route/cls/ematch_syntax.c"
    break;

  case 75:
#line 475 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = 0; }
#line 2163 "lib/route/cls/ematch_syntax.c"
    break;

  case 76:
#line 477 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = (yyvsp[-1].i); }
#line 2169 "lib/route/cls/ematch_syntax.c"
    break;

  case 77:
#line 479 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = (yyvsp[-1].i); }
#line 2175 "lib/route/cls/ematch_syntax.c"
    break;

  case 78:
#line 484 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i64) = 0; }
#line 2181 "lib/route/cls/ematch_syntax.c"
    break;

  case 79:
#line 486 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i64) = (yyvsp[0].i); }
#line 2187 "lib/route/cls/ematch_syntax.c"
    break;

  case 80:
#line 491 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = 0; }
#line 2193 "lib/route/cls/ematch_syntax.c"
    break;

  case 81:
#line 493 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = (yyvsp[0].i); }
#line 2199 "lib/route/cls/ematch_syntax.c"
    break;

  case 82:
#line 498 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = TCF_EM_OPND_EQ; }
#line 2205 "lib/route/cls/ematch_syntax.c"
    break;

  case 83:
#line 500 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = TCF_EM_OPND_GT; }
#line 2211 "lib/route/cls/ematch_syntax.c"
    break;

  case 84:
#line 502 "../libnl-3.5.0/lib/route/cls/ematch_syntax.y"
    { (yyval.i) = TCF_EM_OPND_LT; }
#line 2217 "lib/route/cls/ematch_syntax.c"
    break;


#line 2221 "lib/route/cls/ematch_syntax.c"

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
      yyerror (scanner, errp, root, YY_("syntax error"));
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
        yyerror (scanner, errp, root, yymsgp);
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
                      yytoken, &yylval, scanner, errp, root);
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
                  yystos[yystate], yyvsp, scanner, errp, root);
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
  yyerror (scanner, errp, root, YY_("memory exhausted"));
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
                  yytoken, &yylval, scanner, errp, root);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, scanner, errp, root);
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
