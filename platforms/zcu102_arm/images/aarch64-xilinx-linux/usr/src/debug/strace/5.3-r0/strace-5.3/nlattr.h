/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * Copyright (c) 2017 JingPiao Chen <chenjingpiao@gmail.com>
 * Copyright (c) 2016-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STRACE_NLATTR_H
# define STRACE_NLATTR_H

# include "xlat.h"

struct decode_nla_xlat_opts {
	const struct xlat *xlat;
	const char *dflt;
	enum xlat_style style;
	const char *prefix;
	const char *suffix;
	uint64_t (*process_fn)(uint64_t val);
	size_t size;
};

/*
 * Used for IFLA_LINKINFO decoding.  Since there are no other indicators
 * regarding the nature of data except for previously provided string
 * in an IFLA_LINKINFO_KIND attribute, we have to store it in order to pass
 * between calls as an opaque data.
 */
struct ifla_linkinfo_ctx {
	char kind[16];
};

typedef bool (*nla_decoder_t)(struct tcb *, kernel_ulong_t addr,
			      unsigned int len, const void *opaque_data);

/**
 * The case of non-NULL decoders and zero size is handled in a special way:
 * the zeroth decoder is always called with nla_type being passed as opaque
 * data.
 */
extern void
decode_nlattr(struct tcb *,
	      kernel_ulong_t addr,
	      unsigned int len,
	      const struct xlat *,
	      const char *dflt,
	      const nla_decoder_t *decoders,
	      unsigned int size,
	      const void *opaque_data);

# define DECL_NLA(name)					\
extern bool						\
decode_nla_ ## name(struct tcb *, kernel_ulong_t addr,	\
		    unsigned int len, const void *)	\
/* End of DECL_NLA definition. */

DECL_NLA(x8);
DECL_NLA(x16);
DECL_NLA(x32);
DECL_NLA(x64);
DECL_NLA(u8);
DECL_NLA(u16);
DECL_NLA(u32);
DECL_NLA(u64);
DECL_NLA(s8);
DECL_NLA(s16);
DECL_NLA(s32);
DECL_NLA(s64);
DECL_NLA(be16);
DECL_NLA(be64);
DECL_NLA(xval);
DECL_NLA(flags);
DECL_NLA(str);
DECL_NLA(strn);
DECL_NLA(fd);
DECL_NLA(uid);
DECL_NLA(gid);
DECL_NLA(ifindex);
DECL_NLA(ether_proto);
DECL_NLA(ip_proto);
DECL_NLA(in_addr);
DECL_NLA(in6_addr);
DECL_NLA(meminfo);
DECL_NLA(rt_class);
DECL_NLA(rt_proto);
DECL_NLA(tc_stats);

#endif /* !STRACE_NLATTR_H */
