/* Generated by ./xlat/gen.sh from ./xlat/memfd_create_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(MFD_CLOEXEC) || (defined(HAVE_DECL_MFD_CLOEXEC) && HAVE_DECL_MFD_CLOEXEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((MFD_CLOEXEC) == (1), "MFD_CLOEXEC != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define MFD_CLOEXEC 1
#endif
#if defined(MFD_ALLOW_SEALING) || (defined(HAVE_DECL_MFD_ALLOW_SEALING) && HAVE_DECL_MFD_ALLOW_SEALING)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((MFD_ALLOW_SEALING) == (2), "MFD_ALLOW_SEALING != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define MFD_ALLOW_SEALING 2
#endif
#if defined(MFD_HUGETLB) || (defined(HAVE_DECL_MFD_HUGETLB) && HAVE_DECL_MFD_HUGETLB)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((MFD_HUGETLB) == (4), "MFD_HUGETLB != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define MFD_HUGETLB 4
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat memfd_create_flags in mpers mode

# else

static const struct xlat_data memfd_create_flags_xdata[] = {
 XLAT(MFD_CLOEXEC),
 XLAT(MFD_ALLOW_SEALING),
 XLAT(MFD_HUGETLB),
};
static
const struct xlat memfd_create_flags[1] = { {
 .data = memfd_create_flags_xdata,
 .size = ARRAY_SIZE(memfd_create_flags_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */