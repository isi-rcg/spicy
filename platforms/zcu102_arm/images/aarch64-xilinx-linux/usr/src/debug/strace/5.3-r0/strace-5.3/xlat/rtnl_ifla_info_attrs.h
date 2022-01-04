/* Generated by ./xlat/gen.sh from ./xlat/rtnl_ifla_info_attrs.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(IFLA_INFO_UNSPEC) || (defined(HAVE_DECL_IFLA_INFO_UNSPEC) && HAVE_DECL_IFLA_INFO_UNSPEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_INFO_UNSPEC) == (0), "IFLA_INFO_UNSPEC != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_INFO_UNSPEC 0
#endif
#if defined(IFLA_INFO_KIND) || (defined(HAVE_DECL_IFLA_INFO_KIND) && HAVE_DECL_IFLA_INFO_KIND)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_INFO_KIND) == (1), "IFLA_INFO_KIND != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_INFO_KIND 1
#endif
#if defined(IFLA_INFO_DATA) || (defined(HAVE_DECL_IFLA_INFO_DATA) && HAVE_DECL_IFLA_INFO_DATA)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_INFO_DATA) == (2), "IFLA_INFO_DATA != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_INFO_DATA 2
#endif
#if defined(IFLA_INFO_XSTATS) || (defined(HAVE_DECL_IFLA_INFO_XSTATS) && HAVE_DECL_IFLA_INFO_XSTATS)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_INFO_XSTATS) == (3), "IFLA_INFO_XSTATS != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_INFO_XSTATS 3
#endif
#if defined(IFLA_INFO_SLAVE_KIND) || (defined(HAVE_DECL_IFLA_INFO_SLAVE_KIND) && HAVE_DECL_IFLA_INFO_SLAVE_KIND)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_INFO_SLAVE_KIND) == (4), "IFLA_INFO_SLAVE_KIND != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_INFO_SLAVE_KIND 4
#endif
#if defined(IFLA_INFO_SLAVE_DATA) || (defined(HAVE_DECL_IFLA_INFO_SLAVE_DATA) && HAVE_DECL_IFLA_INFO_SLAVE_DATA)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((IFLA_INFO_SLAVE_DATA) == (5), "IFLA_INFO_SLAVE_DATA != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define IFLA_INFO_SLAVE_DATA 5
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat rtnl_ifla_info_attrs in mpers mode

# else

static const struct xlat_data rtnl_ifla_info_attrs_xdata[] = {
 [IFLA_INFO_UNSPEC] = XLAT(IFLA_INFO_UNSPEC),
 [IFLA_INFO_KIND] = XLAT(IFLA_INFO_KIND),
 [IFLA_INFO_DATA] = XLAT(IFLA_INFO_DATA),
 [IFLA_INFO_XSTATS] = XLAT(IFLA_INFO_XSTATS),
 [IFLA_INFO_SLAVE_KIND] = XLAT(IFLA_INFO_SLAVE_KIND),
 [IFLA_INFO_SLAVE_DATA] = XLAT(IFLA_INFO_SLAVE_DATA),
};
static
const struct xlat rtnl_ifla_info_attrs[1] = { {
 .data = rtnl_ifla_info_attrs_xdata,
 .size = ARRAY_SIZE(rtnl_ifla_info_attrs_xdata),
 .type = XT_INDEXED,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */