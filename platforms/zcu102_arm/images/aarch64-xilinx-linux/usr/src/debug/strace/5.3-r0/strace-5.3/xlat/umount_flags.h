/* Generated by ./xlat/gen.sh from ./xlat/umount_flags.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(MNT_FORCE) || (defined(HAVE_DECL_MNT_FORCE) && HAVE_DECL_MNT_FORCE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((MNT_FORCE) == (1), "MNT_FORCE != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define MNT_FORCE 1
#endif
#if defined(MNT_DETACH) || (defined(HAVE_DECL_MNT_DETACH) && HAVE_DECL_MNT_DETACH)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((MNT_DETACH) == (2), "MNT_DETACH != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define MNT_DETACH 2
#endif
#if defined(MNT_EXPIRE) || (defined(HAVE_DECL_MNT_EXPIRE) && HAVE_DECL_MNT_EXPIRE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((MNT_EXPIRE) == (4), "MNT_EXPIRE != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define MNT_EXPIRE 4
#endif
#if defined(UMOUNT_NOFOLLOW) || (defined(HAVE_DECL_UMOUNT_NOFOLLOW) && HAVE_DECL_UMOUNT_NOFOLLOW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((UMOUNT_NOFOLLOW) == (8), "UMOUNT_NOFOLLOW != 8");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define UMOUNT_NOFOLLOW 8
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat umount_flags in mpers mode

# else

static const struct xlat_data umount_flags_xdata[] = {
 XLAT(MNT_FORCE),
 XLAT(MNT_DETACH),
 XLAT(MNT_EXPIRE),
 XLAT(UMOUNT_NOFOLLOW),
};
static
const struct xlat umount_flags[1] = { {
 .data = umount_flags_xdata,
 .size = ARRAY_SIZE(umount_flags_xdata),
 .type = XT_NORMAL,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */