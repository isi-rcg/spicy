/*
 * Copyright (c) 2016 Fabien Siron <fabien.siron@epita.fr>
 * Copyright (c) 2017 JingPiao Chen <chenjingpiao@gmail.com>
 * Copyright (c) 2016-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include "netlink.h"
#include "netlink_route.h"

#include <linux/rtnetlink.h>

#include "xlat/nl_route_types.h"

static void
decode_family(struct tcb *const tcp, const uint8_t family,
	      const kernel_ulong_t addr, const unsigned int len)
{
	tprints("{family=");
	printxval(addrfams, family, "AF_???");
	if (len > sizeof(family)) {
		tprints(", ");
		printstr_ex(tcp, addr + sizeof(family),
			    len - sizeof(family), QUOTE_FORCE_HEX);
	}
	tprints("}");
}

typedef DECL_NETLINK_ROUTE_DECODER((*netlink_route_decoder_t));

static const netlink_route_decoder_t route_decoders[] = {
	[RTM_DELLINK - RTM_BASE] = decode_ifinfomsg,
	[RTM_GETLINK - RTM_BASE] = decode_ifinfomsg,
	[RTM_NEWLINK - RTM_BASE] = decode_ifinfomsg,
	[RTM_SETLINK - RTM_BASE] = decode_ifinfomsg,

	[RTM_DELADDR - RTM_BASE] = decode_ifaddrmsg,
	[RTM_GETADDR - RTM_BASE] = decode_ifaddrmsg,
	[RTM_GETANYCAST - RTM_BASE] = decode_ifaddrmsg,
	[RTM_GETMULTICAST - RTM_BASE] = decode_ifaddrmsg,
	[RTM_NEWADDR - RTM_BASE] = decode_ifaddrmsg,

	[RTM_DELROUTE - RTM_BASE] = decode_rtmsg,
	[RTM_GETROUTE - RTM_BASE] = decode_rtmsg,
	[RTM_NEWROUTE - RTM_BASE] = decode_rtmsg,

	[RTM_DELRULE - RTM_BASE] = decode_fib_rule_hdr,
	[RTM_GETRULE - RTM_BASE] = decode_fib_rule_hdr,
	[RTM_NEWRULE - RTM_BASE] = decode_fib_rule_hdr,

	[RTM_DELNEIGH - RTM_BASE] = decode_ndmsg,
	[RTM_GETNEIGH - RTM_BASE] = decode_rtm_getneigh,
	[RTM_NEWNEIGH - RTM_BASE] = decode_ndmsg,

	[RTM_GETNEIGHTBL - RTM_BASE] = decode_ndtmsg,
	[RTM_NEWNEIGHTBL - RTM_BASE] = decode_ndtmsg,
	[RTM_SETNEIGHTBL - RTM_BASE] = decode_ndtmsg,

	[RTM_DELQDISC - RTM_BASE] = decode_tcmsg,
	[RTM_GETQDISC - RTM_BASE] = decode_tcmsg,
	[RTM_NEWQDISC - RTM_BASE] = decode_tcmsg,
	[RTM_DELTCLASS - RTM_BASE] = decode_tcmsg,
	[RTM_GETTCLASS - RTM_BASE] = decode_tcmsg,
	[RTM_NEWTCLASS - RTM_BASE] = decode_tcmsg,
	[RTM_DELTFILTER - RTM_BASE] = decode_tcmsg,
	[RTM_GETTFILTER - RTM_BASE] = decode_tcmsg,
	[RTM_NEWTFILTER - RTM_BASE] = decode_tcmsg,

	[RTM_DELACTION - RTM_BASE] = decode_tcamsg,
	[RTM_GETACTION - RTM_BASE] = decode_tcamsg,
	[RTM_NEWACTION - RTM_BASE] = decode_tcamsg,

#ifdef HAVE_STRUCT_IFADDRLBLMSG
	[RTM_DELADDRLABEL - RTM_BASE] = decode_ifaddrlblmsg,
	[RTM_GETADDRLABEL - RTM_BASE] = decode_ifaddrlblmsg,
	[RTM_NEWADDRLABEL - RTM_BASE] = decode_ifaddrlblmsg,
#endif

#ifdef HAVE_STRUCT_DCBMSG
	[RTM_GETDCB - RTM_BASE] = decode_dcbmsg,
	[RTM_SETDCB - RTM_BASE] = decode_dcbmsg,
#endif

#ifdef HAVE_STRUCT_NETCONFMSG
	[RTM_DELNETCONF - RTM_BASE] = decode_netconfmsg,
	[RTM_GETNETCONF - RTM_BASE] = decode_netconfmsg,
	[RTM_NEWNETCONF - RTM_BASE] = decode_netconfmsg,
#endif

#ifdef HAVE_STRUCT_BR_PORT_MSG
	[RTM_DELMDB - RTM_BASE] = decode_br_port_msg,
	[RTM_GETMDB - RTM_BASE] = decode_br_port_msg,
	[RTM_NEWMDB - RTM_BASE] = decode_br_port_msg,
#endif

	[RTM_DELNSID - RTM_BASE] = decode_rtgenmsg,
	[RTM_GETNSID - RTM_BASE] = decode_rtgenmsg,
	[RTM_NEWNSID - RTM_BASE] = decode_rtgenmsg,

	[RTM_NEWCHAIN - RTM_BASE] = decode_tcmsg,
	[RTM_DELCHAIN - RTM_BASE] = decode_tcmsg,
	[RTM_GETCHAIN - RTM_BASE] = decode_tcmsg,
};

bool
decode_netlink_route(struct tcb *const tcp,
		     const struct nlmsghdr *const nlmsghdr,
		     const kernel_ulong_t addr,
		     const unsigned int len)
{
	uint8_t family;

	if (nlmsghdr->nlmsg_type == NLMSG_DONE)
		return false;

	if (!umove_or_printaddr(tcp, addr, &family)) {
		const unsigned int index = nlmsghdr->nlmsg_type - RTM_BASE;

		if (index < ARRAY_SIZE(route_decoders)
		    && route_decoders[index]) {
			route_decoders[index](tcp, nlmsghdr, family, addr, len);
		} else {
			decode_family(tcp, family, addr, len);
		}
	}

	return true;
}
