/* Generated by ./xlat/gen.sh from ./xlat/seccomp_ops.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(SECCOMP_SET_MODE_STRICT) || (defined(HAVE_DECL_SECCOMP_SET_MODE_STRICT) && HAVE_DECL_SECCOMP_SET_MODE_STRICT)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SECCOMP_SET_MODE_STRICT) == (0), "SECCOMP_SET_MODE_STRICT != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SECCOMP_SET_MODE_STRICT 0
#endif
#if defined(SECCOMP_SET_MODE_FILTER) || (defined(HAVE_DECL_SECCOMP_SET_MODE_FILTER) && HAVE_DECL_SECCOMP_SET_MODE_FILTER)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SECCOMP_SET_MODE_FILTER) == (1), "SECCOMP_SET_MODE_FILTER != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SECCOMP_SET_MODE_FILTER 1
#endif
#if defined(SECCOMP_GET_ACTION_AVAIL) || (defined(HAVE_DECL_SECCOMP_GET_ACTION_AVAIL) && HAVE_DECL_SECCOMP_GET_ACTION_AVAIL)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SECCOMP_GET_ACTION_AVAIL) == (2), "SECCOMP_GET_ACTION_AVAIL != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SECCOMP_GET_ACTION_AVAIL 2
#endif
#if defined(SECCOMP_GET_NOTIF_SIZES) || (defined(HAVE_DECL_SECCOMP_GET_NOTIF_SIZES) && HAVE_DECL_SECCOMP_GET_NOTIF_SIZES)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((SECCOMP_GET_NOTIF_SIZES) == (3), "SECCOMP_GET_NOTIF_SIZES != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define SECCOMP_GET_NOTIF_SIZES 3
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat seccomp_ops in mpers mode

# else

static const struct xlat_data seccomp_ops_xdata[] = {
 [SECCOMP_SET_MODE_STRICT] = XLAT(SECCOMP_SET_MODE_STRICT),
 [SECCOMP_SET_MODE_FILTER] = XLAT(SECCOMP_SET_MODE_FILTER),
 [SECCOMP_GET_ACTION_AVAIL] = XLAT(SECCOMP_GET_ACTION_AVAIL),
 [SECCOMP_GET_NOTIF_SIZES] = XLAT(SECCOMP_GET_NOTIF_SIZES),
};
static
const struct xlat seccomp_ops[1] = { {
 .data = seccomp_ops_xdata,
 .size = ARRAY_SIZE(seccomp_ops_xdata),
 .type = XT_INDEXED,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */