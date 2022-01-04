#ifndef PRIVATE_SLINT_H_
# define PRIVATE_SLINT_H_

/* This file defines macros and types for ints of various types.
 * Eventually it will make use of stdint.h.
 */

/* If long or short are ints, then map the slang types to ints.  This is
 * done because slang has some optimizations for ints.
 */
#define LONG_IS_INT (SIZEOF_INT == SIZEOF_LONG)
#define LONG_IS_NOT_INT (SIZEOF_INT != SIZEOF_LONG)
#define SHORT_IS_INT (SIZEOF_INT == SIZEOF_SHORT)
#define SHORT_IS_NOT_INT (SIZEOF_INT != SIZEOF_SHORT)
#define LLONG_IS_LONG (SIZEOF_LONG == SIZEOF_LONG_LONG)
#define LLONG_IS_NOT_LONG (SIZEOF_LONG != SIZEOF_LONG_LONG)

#if LONG_IS_INT
# define _pSLANG_LONG_TYPE SLANG_INT_TYPE
# define _pSLANG_ULONG_TYPE SLANG_UINT_TYPE
#else
# define _pSLANG_LONG_TYPE SLANG_LONG_TYPE
# define _pSLANG_ULONG_TYPE SLANG_ULONG_TYPE
#endif
#if SHORT_IS_INT
# define _pSLANG_SHORT_TYPE SLANG_INT_TYPE
# define _pSLANG_USHORT_TYPE SLANG_UINT_TYPE
#else
# define _pSLANG_SHORT_TYPE SLANG_SHORT_TYPE
# define _pSLANG_USHORT_TYPE SLANG_USHORT_TYPE
#endif
#if LLONG_IS_LONG
# define _pSLANG_LLONG_TYPE _pSLANG_LONG_TYPE
# define _pSLANG_ULLONG_TYPE _pSLANG_ULONG_TYPE
#else
# define _pSLANG_LLONG_TYPE SLANG_LLONG_TYPE
# define _pSLANG_ULLONG_TYPE SLANG_ULLONG_TYPE
#endif

/* Map off_t to a slang type */
#if defined(HAVE_LONG_LONG) && (SIZEOF_OFF_T == SIZEOF_LONG_LONG) && (SIZEOF_LONG_LONG > SIZEOF_LONG)
# define SLANG_C_OFF_T_TYPE _pSLANG_LLONG_TYPE
typedef long long _pSLc_off_t_Type;
# define SLANG_PUSH_OFF_T SLang_push_long_long
#else
# if (SIZEOF_OFF_T == SIZEOF_INT)
#  define SLANG_C_OFF_T_TYPE SLANG_INT_TYPE
#  define SLANG_PUSH_OFF_T SLang_push_int
typedef int _pSLc_off_t_Type;
# else
#  define SLANG_C_OFF_T_TYPE _pSLANG_LONG_TYPE
#  define SLANG_PUSH_OFF_T SLang_push_long
typedef long _pSLc_off_t_Type;
# endif
#endif

#if SIZEOF_INT == 2
# define _pSLANG_INT16_TYPE	SLANG_INT_TYPE
# define _pSLANG_UINT16_TYPE	SLANG_UINT_TYPE
typedef int _pSLint16_Type;
typedef unsigned int _pSLuint16_Type;
#else
# if SIZEOF_SHORT == 2
#  define _pSLANG_INT16_TYPE	SLANG_SHORT_TYPE
#  define _pSLANG_UINT16_TYPE	SLANG_USHORT_TYPE
typedef short _pSLint16_Type;
typedef unsigned short _pSLuint16_Type;
# else
#  if SIZEOF_LONG == 2
#   define _pSLANG_INT16_TYPE	SLANG_LONG_TYPE
#   define _pSLANG_UINT16_TYPE	SLANG_ULONG_TYPE
typedef long _pSLInt16_Type;
typedef unsigned long _pSLuint16_Type;
#  else
#   define _pSLANG_INT16_TYPE	0
#   define _pSLANG_UINT16_TYPE	0
#  endif
# endif
#endif

#if SIZEOF_INT == 4
# define _pSLANG_INT32_TYPE	SLANG_INT_TYPE
# define _pSLANG_UINT32_TYPE	SLANG_UINT_TYPE
typedef int _pSLint32_Type;
typedef unsigned int _pSLuint32_Type;
#else
# if SIZEOF_SHORT == 4
#  define _pSLANG_INT32_TYPE	SLANG_SHORT_TYPE
#  define _pSLANG_UINT32_TYPE	SLANG_USHORT_TYPE
typedef short _pSLInt32_Type;
typedef unsigned short _pSLuint32_Type;
# else
#  if SIZEOF_LONG == 4
#   define _pSLANG_INT32_TYPE	SLANG_LONG_TYPE
#   define _pSLANG_UINT32_TYPE	SLANG_ULONG_TYPE
typedef long _pSLInt32_Type;
typedef unsigned long _pSLuint32_Type;
#  else
#   define _pSLANG_INT32_TYPE	0
#   define _pSLANG_UINT32_TYPE	0
#  endif
# endif
#endif

#if SIZEOF_INT == 8
# define _pSLANG_INT64_TYPE	SLANG_INT_TYPE
# define _pSLANG_UINT64_TYPE	SLANG_UINT_TYPE
typedef int _pSLint64_Type;
typedef unsigned int _pSLuint64_Type;
#else
# if SIZEOF_SHORT == 8
#  define _pSLANG_INT64_TYPE	SLANG_SHORT_TYPE
#  define _pSLANG_UINT64_TYPE	SLANG_USHORT_TYPE
typedef int _pSLint64_Type;
typedef unsigned int _pSLuint64_Type;
# else
#  if SIZEOF_LONG == 8
#   define _pSLANG_INT64_TYPE	SLANG_LONG_TYPE
#   define _pSLANG_UINT64_TYPE	SLANG_ULONG_TYPE
typedef long _pSLint64_Type;
typedef unsigned long _pSLuint64_Type;
#  else
#   if SIZEOF_LONG_LONG == 8
#    define _pSLANG_INT64_TYPE	SLANG_LLONG_TYPE
#    define _pSLANG_UINT64_TYPE	SLANG_ULLONG_TYPE
typedef long long _pSLint64_Type;
typedef unsigned long long _pSLuint64_Type;
#   else
#    define _pSLANG_INT64_TYPE	0
#    define _pSLANG_UINT64_TYPE	0
#   endif
#  endif
# endif
#endif

/* The following are commented out because they are not used by the library */
/* extern int _pSLang_pop_int16 (_pSLint16_Type *); */
/* extern int _pSLang_pop_uint16 (_pSLuint16_Type *); */
/* extern int _pSLang_pop_int32 (_pSLint32_Type *); */
/* extern int _pSLang_pop_uint32 (_pSLuint32_Type *); */
#if _pSLANG_INT64_TYPE
extern int _pSLang_pop_int64(_pSLint64_Type *);
extern int _pSLang_pop_uint64 (_pSLuint64_Type *);
#endif
/* extern int _pSLang_push_int16 (_pSLint16_Type); */
/* extern int _pSLang_push_uint16 (_pSLuint16_Type); */
/* extern int _pSLang_push_int32 (_pSLint32_Type); */
/* extern int _pSLang_push_uint32 (_pSLuint32_Type); */
#if _pSLANG_INT64_TYPE
/* extern int _pSLang_push_int64(_pSLint64_Type); */
/* extern int _pSLang_push_uint64 (_pSLuint64_Type); */
#endif

#ifdef HAVE_LONG_LONG
# ifdef __WIN32__
#  define SLFMT_LLD  "%I64d"
#  define SLFMT_LLU  "%I64u"
# else
#  define SLFMT_LLD  "%lld"
#  define SLFMT_LLU  "%llu"
# endif
#endif

#endif				       /* PRIVATE_SLINT_H_ */
