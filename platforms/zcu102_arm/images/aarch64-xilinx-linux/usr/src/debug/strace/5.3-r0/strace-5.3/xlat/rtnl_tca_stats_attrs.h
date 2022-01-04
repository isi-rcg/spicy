/* Generated by ./xlat/gen.sh from ./xlat/rtnl_tca_stats_attrs.in; do not edit. */

#include "gcc_compat.h"
#include "static_assert.h"

#if defined(TCA_STATS_UNSPEC) || (defined(HAVE_DECL_TCA_STATS_UNSPEC) && HAVE_DECL_TCA_STATS_UNSPEC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_UNSPEC) == (0), "TCA_STATS_UNSPEC != 0");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_UNSPEC 0
#endif
#if defined(TCA_STATS_BASIC) || (defined(HAVE_DECL_TCA_STATS_BASIC) && HAVE_DECL_TCA_STATS_BASIC)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_BASIC) == (1), "TCA_STATS_BASIC != 1");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_BASIC 1
#endif
#if defined(TCA_STATS_RATE_EST) || (defined(HAVE_DECL_TCA_STATS_RATE_EST) && HAVE_DECL_TCA_STATS_RATE_EST)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_RATE_EST) == (2), "TCA_STATS_RATE_EST != 2");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_RATE_EST 2
#endif
#if defined(TCA_STATS_QUEUE) || (defined(HAVE_DECL_TCA_STATS_QUEUE) && HAVE_DECL_TCA_STATS_QUEUE)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_QUEUE) == (3), "TCA_STATS_QUEUE != 3");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_QUEUE 3
#endif
#if defined(TCA_STATS_APP) || (defined(HAVE_DECL_TCA_STATS_APP) && HAVE_DECL_TCA_STATS_APP)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_APP) == (4), "TCA_STATS_APP != 4");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_APP 4
#endif
#if defined(TCA_STATS_RATE_EST64) || (defined(HAVE_DECL_TCA_STATS_RATE_EST64) && HAVE_DECL_TCA_STATS_RATE_EST64)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_RATE_EST64) == (5), "TCA_STATS_RATE_EST64 != 5");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_RATE_EST64 5
#endif
#if defined(TCA_STATS_PAD) || (defined(HAVE_DECL_TCA_STATS_PAD) && HAVE_DECL_TCA_STATS_PAD)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_PAD) == (6), "TCA_STATS_PAD != 6");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_PAD 6
#endif
#if defined(TCA_STATS_BASIC_HW) || (defined(HAVE_DECL_TCA_STATS_BASIC_HW) && HAVE_DECL_TCA_STATS_BASIC_HW)
DIAG_PUSH_IGNORE_TAUTOLOGICAL_COMPARE
static_assert((TCA_STATS_BASIC_HW) == (7), "TCA_STATS_BASIC_HW != 7");
DIAG_POP_IGNORE_TAUTOLOGICAL_COMPARE
#else
# define TCA_STATS_BASIC_HW 7
#endif

#ifndef XLAT_MACROS_ONLY

# ifdef IN_MPERS

#  error static const struct xlat rtnl_tca_stats_attrs in mpers mode

# else

static const struct xlat_data rtnl_tca_stats_attrs_xdata[] = {
 [TCA_STATS_UNSPEC] = XLAT(TCA_STATS_UNSPEC),
 [TCA_STATS_BASIC] = XLAT(TCA_STATS_BASIC),
 [TCA_STATS_RATE_EST] = XLAT(TCA_STATS_RATE_EST),
 [TCA_STATS_QUEUE] = XLAT(TCA_STATS_QUEUE),
 [TCA_STATS_APP] = XLAT(TCA_STATS_APP),
 [TCA_STATS_RATE_EST64] = XLAT(TCA_STATS_RATE_EST64),
 [TCA_STATS_PAD] = XLAT(TCA_STATS_PAD),
 [TCA_STATS_BASIC_HW] = XLAT(TCA_STATS_BASIC_HW),
};
static
const struct xlat rtnl_tca_stats_attrs[1] = { {
 .data = rtnl_tca_stats_attrs_xdata,
 .size = ARRAY_SIZE(rtnl_tca_stats_attrs_xdata),
 .type = XT_INDEXED,
} };

# endif /* !IN_MPERS */

#endif /* !XLAT_MACROS_ONLY */