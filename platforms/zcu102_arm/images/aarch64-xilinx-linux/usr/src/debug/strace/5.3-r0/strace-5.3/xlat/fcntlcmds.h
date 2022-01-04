/* Generated by ./xlat/gen.sh from ./xlat/fcntlcmds.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(F_DUPFD) || (defined(HAVE_DECL_F_DUPFD) && HAVE_DECL_F_DUPFD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_DUPFD) == (0), "F_DUPFD != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_DUPFD 0
#endif
#if defined(F_GETFD) || (defined(HAVE_DECL_F_GETFD) && HAVE_DECL_F_GETFD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETFD) == (1), "F_GETFD != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETFD 1
#endif
#if defined(F_SETFD) || (defined(HAVE_DECL_F_SETFD) && HAVE_DECL_F_SETFD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETFD) == (2), "F_SETFD != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETFD 2
#endif
#if defined(F_GETFL) || (defined(HAVE_DECL_F_GETFL) && HAVE_DECL_F_GETFL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETFL) == (3), "F_GETFL != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETFL 3
#endif
#if defined(F_SETFL) || (defined(HAVE_DECL_F_SETFL) && HAVE_DECL_F_SETFL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETFL) == (4), "F_SETFL != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETFL 4
#endif
#if defined __alpha__ || defined __sparc__
#if defined(F_GETLK) || (defined(HAVE_DECL_F_GETLK) && HAVE_DECL_F_GETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLK) == (7), "F_GETLK != 7");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLK 7
#endif
#elif defined __mips__
#if defined(F_GETLK) || (defined(HAVE_DECL_F_GETLK) && HAVE_DECL_F_GETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLK) == (14), "F_GETLK != 14");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLK 14
#endif
#else
#if defined(F_GETLK) || (defined(HAVE_DECL_F_GETLK) && HAVE_DECL_F_GETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLK) == (5), "F_GETLK != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLK 5
#endif
#endif
#if defined __alpha__ || defined __sparc__
#if defined(F_SETLK) || (defined(HAVE_DECL_F_SETLK) && HAVE_DECL_F_SETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLK) == (8), "F_SETLK != 8");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLK 8
#endif
#if defined(F_SETLKW) || (defined(HAVE_DECL_F_SETLKW) && HAVE_DECL_F_SETLKW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLKW) == (9), "F_SETLKW != 9");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLKW 9
#endif
#else
#if defined(F_SETLK) || (defined(HAVE_DECL_F_SETLK) && HAVE_DECL_F_SETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLK) == (6), "F_SETLK != 6");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLK 6
#endif
#if defined(F_SETLKW) || (defined(HAVE_DECL_F_SETLKW) && HAVE_DECL_F_SETLKW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLKW) == (7), "F_SETLKW != 7");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLKW 7
#endif
#endif
#if defined __alpha__
#if defined(F_SETOWN) || (defined(HAVE_DECL_F_SETOWN) && HAVE_DECL_F_SETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETOWN) == (5), "F_SETOWN != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETOWN 5
#endif
#if defined(F_GETOWN) || (defined(HAVE_DECL_F_GETOWN) && HAVE_DECL_F_GETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWN) == (6), "F_GETOWN != 6");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWN 6
#endif
#elif defined __hppa__
#if defined(F_GETOWN) || (defined(HAVE_DECL_F_GETOWN) && HAVE_DECL_F_GETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWN) == (11), "F_GETOWN != 11");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWN 11
#endif
#if defined(F_SETOWN) || (defined(HAVE_DECL_F_SETOWN) && HAVE_DECL_F_SETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETOWN) == (12), "F_SETOWN != 12");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETOWN 12
#endif
#elif defined __mips__
#if defined(F_GETOWN) || (defined(HAVE_DECL_F_GETOWN) && HAVE_DECL_F_GETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWN) == (23), "F_GETOWN != 23");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWN 23
#endif
#if defined(F_SETOWN) || (defined(HAVE_DECL_F_SETOWN) && HAVE_DECL_F_SETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETOWN) == (24), "F_SETOWN != 24");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETOWN 24
#endif
#elif defined __sparc__
#if defined(F_GETOWN) || (defined(HAVE_DECL_F_GETOWN) && HAVE_DECL_F_GETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWN) == (5), "F_GETOWN != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWN 5
#endif
#if defined(F_SETOWN) || (defined(HAVE_DECL_F_SETOWN) && HAVE_DECL_F_SETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETOWN) == (6), "F_SETOWN != 6");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETOWN 6
#endif
#else
#if defined(F_SETOWN) || (defined(HAVE_DECL_F_SETOWN) && HAVE_DECL_F_SETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETOWN) == (8), "F_SETOWN != 8");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETOWN 8
#endif
#if defined(F_GETOWN) || (defined(HAVE_DECL_F_GETOWN) && HAVE_DECL_F_GETOWN)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWN) == (9), "F_GETOWN != 9");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWN 9
#endif
#endif
#ifdef __hppa__
#if defined(F_SETSIG) || (defined(HAVE_DECL_F_SETSIG) && HAVE_DECL_F_SETSIG)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETSIG) == (13), "F_SETSIG != 13");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETSIG 13
#endif
#if defined(F_GETSIG) || (defined(HAVE_DECL_F_GETSIG) && HAVE_DECL_F_GETSIG)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETSIG) == (14), "F_GETSIG != 14");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETSIG 14
#endif
#else
#if defined(F_SETSIG) || (defined(HAVE_DECL_F_SETSIG) && HAVE_DECL_F_SETSIG)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETSIG) == (10), "F_SETSIG != 10");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETSIG 10
#endif
#if defined(F_GETSIG) || (defined(HAVE_DECL_F_GETSIG) && HAVE_DECL_F_GETSIG)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETSIG) == (11), "F_GETSIG != 11");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETSIG 11
#endif
#endif
#if defined __hppa__
#if defined(F_GETLK64) || (defined(HAVE_DECL_F_GETLK64) && HAVE_DECL_F_GETLK64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLK64) == (8), "F_GETLK64 != 8");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLK64 8
#endif
#if defined(F_SETLK64) || (defined(HAVE_DECL_F_SETLK64) && HAVE_DECL_F_SETLK64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLK64) == (9), "F_SETLK64 != 9");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLK64 9
#endif
#if defined(F_SETLKW64) || (defined(HAVE_DECL_F_SETLKW64) && HAVE_DECL_F_SETLKW64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLKW64) == (10), "F_SETLKW64 != 10");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLKW64 10
#endif
#elif defined __mips__ && !defined __mips64
#if defined(F_GETLK64) || (defined(HAVE_DECL_F_GETLK64) && HAVE_DECL_F_GETLK64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLK64) == (33), "F_GETLK64 != 33");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLK64 33
#endif
#if defined(F_SETLK64) || (defined(HAVE_DECL_F_SETLK64) && HAVE_DECL_F_SETLK64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLK64) == (34), "F_SETLK64 != 34");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLK64 34
#endif
#if defined(F_SETLKW64) || (defined(HAVE_DECL_F_SETLKW64) && HAVE_DECL_F_SETLKW64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLKW64) == (35), "F_SETLKW64 != 35");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLKW64 35
#endif
#else
#if defined(F_GETLK64) || (defined(HAVE_DECL_F_GETLK64) && HAVE_DECL_F_GETLK64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLK64) == (12), "F_GETLK64 != 12");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLK64 12
#endif
#if defined(F_SETLK64) || (defined(HAVE_DECL_F_SETLK64) && HAVE_DECL_F_SETLK64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLK64) == (13), "F_SETLK64 != 13");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLK64 13
#endif
#if defined(F_SETLKW64) || (defined(HAVE_DECL_F_SETLKW64) && HAVE_DECL_F_SETLKW64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLKW64) == (14), "F_SETLKW64 != 14");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLKW64 14
#endif
#endif
#ifndef STRACE_WORKAROUND_FOR_F_OWNER_EX
# define STRACE_WORKAROUND_FOR_F_OWNER_EX
# if defined F_SETOWN_EX && F_SETOWN_EX != 15
#  warning invalid value of F_SETOWN_EX ignored
# endif
# undef F_SETOWN_EX
# if defined F_GETOWN_EX && F_GETOWN_EX != 16
#  warning invalid value of F_GETOWN_EX ignored
# endif
# undef F_GETOWN_EX
#endif
#if defined(F_SETOWN_EX) || (defined(HAVE_DECL_F_SETOWN_EX) && HAVE_DECL_F_SETOWN_EX)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETOWN_EX) == (15), "F_SETOWN_EX != 15");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETOWN_EX 15
#endif
#if defined(F_GETOWN_EX) || (defined(HAVE_DECL_F_GETOWN_EX) && HAVE_DECL_F_GETOWN_EX)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWN_EX) == (16), "F_GETOWN_EX != 16");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWN_EX 16
#endif
#if defined(F_GETOWNER_UIDS) || (defined(HAVE_DECL_F_GETOWNER_UIDS) && HAVE_DECL_F_GETOWNER_UIDS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETOWNER_UIDS) == (17), "F_GETOWNER_UIDS != 17");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETOWNER_UIDS 17
#endif
#if defined(F_OFD_GETLK) || (defined(HAVE_DECL_F_OFD_GETLK) && HAVE_DECL_F_OFD_GETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_OFD_GETLK) == (36), "F_OFD_GETLK != 36");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_OFD_GETLK 36
#endif
#if defined(F_OFD_SETLK) || (defined(HAVE_DECL_F_OFD_SETLK) && HAVE_DECL_F_OFD_SETLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_OFD_SETLK) == (37), "F_OFD_SETLK != 37");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_OFD_SETLK 37
#endif
#if defined(F_OFD_SETLKW) || (defined(HAVE_DECL_F_OFD_SETLKW) && HAVE_DECL_F_OFD_SETLKW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_OFD_SETLKW) == (38), "F_OFD_SETLKW != 38");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_OFD_SETLKW 38
#endif
#if defined(F_SETLEASE) || (defined(HAVE_DECL_F_SETLEASE) && HAVE_DECL_F_SETLEASE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETLEASE) == ((1024 + 0)), "F_SETLEASE != (1024 + 0)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETLEASE (1024 + 0)
#endif
#if defined(F_GETLEASE) || (defined(HAVE_DECL_F_GETLEASE) && HAVE_DECL_F_GETLEASE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETLEASE) == ((1024 + 1)), "F_GETLEASE != (1024 + 1)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETLEASE (1024 + 1)
#endif
#if defined(F_NOTIFY) || (defined(HAVE_DECL_F_NOTIFY) && HAVE_DECL_F_NOTIFY)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_NOTIFY) == ((1024 + 2)), "F_NOTIFY != (1024 + 2)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_NOTIFY (1024 + 2)
#endif
#if defined(F_CANCELLK) || (defined(HAVE_DECL_F_CANCELLK) && HAVE_DECL_F_CANCELLK)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_CANCELLK) == ((1024 + 5)), "F_CANCELLK != (1024 + 5)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_CANCELLK (1024 + 5)
#endif
#if defined(F_DUPFD_CLOEXEC) || (defined(HAVE_DECL_F_DUPFD_CLOEXEC) && HAVE_DECL_F_DUPFD_CLOEXEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_DUPFD_CLOEXEC) == ((1024 + 6)), "F_DUPFD_CLOEXEC != (1024 + 6)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_DUPFD_CLOEXEC (1024 + 6)
#endif
#if defined(F_SETPIPE_SZ) || (defined(HAVE_DECL_F_SETPIPE_SZ) && HAVE_DECL_F_SETPIPE_SZ)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_SETPIPE_SZ) == ((1024 + 7)), "F_SETPIPE_SZ != (1024 + 7)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_SETPIPE_SZ (1024 + 7)
#endif
#if defined(F_GETPIPE_SZ) || (defined(HAVE_DECL_F_GETPIPE_SZ) && HAVE_DECL_F_GETPIPE_SZ)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GETPIPE_SZ) == ((1024 + 8)), "F_GETPIPE_SZ != (1024 + 8)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GETPIPE_SZ (1024 + 8)
#endif
#if defined(F_ADD_SEALS) || (defined(HAVE_DECL_F_ADD_SEALS) && HAVE_DECL_F_ADD_SEALS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_ADD_SEALS) == ((1024 + 9)), "F_ADD_SEALS != (1024 + 9)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_ADD_SEALS (1024 + 9)
#endif
#if defined(F_GET_SEALS) || (defined(HAVE_DECL_F_GET_SEALS) && HAVE_DECL_F_GET_SEALS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((F_GET_SEALS) == ((1024 + 10)), "F_GET_SEALS != (1024 + 10)");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define F_GET_SEALS (1024 + 10)
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat fcntlcmds in mpers mode

# else

static const struct xlat_data fcntlcmds_xdata[] = {

 XLAT(F_DUPFD),
 XLAT(F_GETFD),
 XLAT(F_SETFD),
 XLAT(F_GETFL),
 XLAT(F_SETFL),

#if defined __alpha__ || defined __sparc__
 XLAT(F_GETLK),
#elif defined __mips__
 XLAT(F_GETLK),
#else
 XLAT(F_GETLK),
#endif

#if defined __alpha__ || defined __sparc__
 XLAT(F_SETLK),
 XLAT(F_SETLKW),
#else
 XLAT(F_SETLK),
 XLAT(F_SETLKW),
#endif

#if defined __alpha__
 XLAT(F_SETOWN),
 XLAT(F_GETOWN),
#elif defined __hppa__
 XLAT(F_GETOWN),
 XLAT(F_SETOWN),
#elif defined __mips__
 XLAT(F_GETOWN),
 XLAT(F_SETOWN),
#elif defined __sparc__
 XLAT(F_GETOWN),
 XLAT(F_SETOWN),
#else
 XLAT(F_SETOWN),
 XLAT(F_GETOWN),
#endif

#ifdef __hppa__
 XLAT(F_SETSIG),
 XLAT(F_GETSIG),
#else
 XLAT(F_SETSIG),
 XLAT(F_GETSIG),
#endif

#if defined __hppa__
 XLAT(F_GETLK64),
 XLAT(F_SETLK64),
 XLAT(F_SETLKW64),
#elif defined __mips__ && !defined __mips64
 XLAT(F_GETLK64),
 XLAT(F_SETLK64),
 XLAT(F_SETLKW64),
#else
 XLAT(F_GETLK64),
 XLAT(F_SETLK64),
 XLAT(F_SETLKW64),
#endif

#ifndef STRACE_WORKAROUND_FOR_F_OWNER_EX
# define STRACE_WORKAROUND_FOR_F_OWNER_EX
/*
* Linux kernel commit v2.6.32-rc7~23 has changed values of F_SETOWN_EX
* and F_GETOWN_EX constants introduced by commit v2.6.32-rc1~96 to fix
* the conflict with F_GETLK64 and F_SETLK64 constants.
* Looks like the best way to handle this situation is to pretend that
* old values of F_SETOWN_EX and F_GETOWN_EX didn't exist.
*/
# if defined F_SETOWN_EX && F_SETOWN_EX != 15
#  warning invalid value of F_SETOWN_EX ignored
# endif
# undef F_SETOWN_EX
# if defined F_GETOWN_EX && F_GETOWN_EX != 16
#  warning invalid value of F_GETOWN_EX ignored
# endif
# undef F_GETOWN_EX
#endif
 XLAT(F_SETOWN_EX),
 XLAT(F_GETOWN_EX),

 XLAT(F_GETOWNER_UIDS),
 XLAT(F_OFD_GETLK),
 XLAT(F_OFD_SETLK),
 XLAT(F_OFD_SETLKW),


 XLAT(F_SETLEASE),
 XLAT(F_GETLEASE),
 XLAT(F_NOTIFY),
 XLAT(F_CANCELLK),
 XLAT(F_DUPFD_CLOEXEC),
 XLAT(F_SETPIPE_SZ),
 XLAT(F_GETPIPE_SZ),
 XLAT(F_ADD_SEALS),
 XLAT(F_GET_SEALS),
};
static
const struct xlat fcntlcmds[1] = { {
 .data = fcntlcmds_xdata,
 .size = ARRAY_SIZE(fcntlcmds_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */