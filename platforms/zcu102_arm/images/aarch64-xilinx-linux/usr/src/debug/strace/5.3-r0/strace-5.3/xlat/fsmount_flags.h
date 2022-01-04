/* Generated by ./xlat/gen.sh from ./xlat/fsmount_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(FSMOUNT_CLOEXEC) || (defined(HAVE_DECL_FSMOUNT_CLOEXEC) && HAVE_DECL_FSMOUNT_CLOEXEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((FSMOUNT_CLOEXEC) == (1), "FSMOUNT_CLOEXEC != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define FSMOUNT_CLOEXEC 1
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat fsmount_flags in mpers mode

# else

static const struct xlat_data fsmount_flags_xdata[] = {
 XLAT(FSMOUNT_CLOEXEC),
};
static
const struct xlat fsmount_flags[1] = { {
 .data = fsmount_flags_xdata,
 .size = ARRAY_SIZE(fsmount_flags_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */