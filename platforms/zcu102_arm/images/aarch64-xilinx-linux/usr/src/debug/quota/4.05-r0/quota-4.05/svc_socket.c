/* Copyright (C) 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include "config.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <rpc/rpc.h>
#include <sys/socket.h>

#include "common.h"
#include "pot.h"

static int get_service_port(u_long number, const char *proto)
{
	char servdata [1024];
	struct rpcent *rpcp = NULL;
	struct servent servbuf, *servp = NULL;
	int ret;

	rpcp = getrpcbynumber(number);
	if (rpcp != NULL) {
		/* First try name */
		ret = getservbyname_r(rpcp->r_name, proto, &servbuf, servdata,
		                       sizeof servdata, &servp);
		if ((ret != 0 || servp == NULL) && rpcp->r_aliases) {
			const char **a;

			/* Then we try aliases.	*/
			for (a = (const char **) rpcp->r_aliases; *a != NULL; a++) {
				ret = getservbyname_r(*a, proto, &servbuf, servdata,
						 sizeof servdata, &servp);
				if (ret == 0 && servp != NULL)
					break;
			}
		}
		if (ret == 0 && servp != NULL)
			return ntohs(servp->s_port);
	}
	return 0;
}

static struct addrinfo *svc_create_bindaddr(struct netconfig *nconf, int port)
{
	struct addrinfo *ai = NULL;
	struct addrinfo hint = {
		.ai_flags = AI_PASSIVE | AI_NUMERICSERV,
	};
	char portbuf[16];
	int err;

	if (!strcmp(nconf->nc_protofmly, NC_INET))
		hint.ai_family = AF_INET;
	else if (!strcmp(nconf->nc_protofmly, NC_INET6))
		hint.ai_family = AF_INET6;
	else {
		errstr(_("Unrecognized bind address family: %s\n"),
			nconf->nc_protofmly);
		return NULL;
	}

	if (!strcmp(nconf->nc_proto, NC_UDP))
		hint.ai_protocol = IPPROTO_UDP;
	else if (!strcmp(nconf->nc_proto, NC_TCP))
		hint.ai_protocol = IPPROTO_TCP;
	else {
		errstr(_("Unrecognized bind address protocol: %s\n"),
			nconf->nc_proto);
		return NULL;
	}

	if (nconf->nc_semantics == NC_TPI_CLTS)
		hint.ai_socktype = SOCK_DGRAM;
	else if (nconf->nc_semantics == NC_TPI_COTS_ORD)
		hint.ai_socktype = SOCK_STREAM;
	else {
		errstr(_("Unrecognized address semantics: %lu\n"),
			nconf->nc_semantics);
		return NULL;
	}

	snprintf(portbuf, sizeof(portbuf), "%u", port);
	err = getaddrinfo(NULL, portbuf, &hint, &ai);
	if (err) {
		errstr(_("Failed to construct bind address: %s\n"),
			gai_strerror(err));
		return NULL;
	}
	return ai;
}

static int svc_create_sock(struct addrinfo *ai)
{
	int fd;
	int optval = 1;

	fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol);
	if (fd < 0) {
		errstr(_("Error creating socket: %s\n"), strerror(errno));
		return -1;
	}

	if (ai->ai_family == AF_INET6) {
		if (setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY,
				&optval, sizeof(optval)) < 0) {
			errstr(_("Cannot set IPv6 socket options: %s\n"), strerror(errno));
			close(fd);
			return -1;
		}
	}

	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0) {
		errstr(_("Cannot set socket options: %s\n"), strerror(errno));
		close(fd);
		return -1;
	}

	if (bind(fd, ai->ai_addr, ai->ai_addrlen) < 0) {
		errstr(_("Cannot bind to address: %s\n"), strerror(errno));
		close(fd);
		return -1;
	}

	if (ai->ai_protocol == IPPROTO_TCP) {
		if (listen(fd, SOMAXCONN) < 0) {
			errstr(_("Cannot listen to address: %s\n"), strerror(errno));
			close(fd);
			return -1;
		}
	}

	return fd;
}

/*
 * Create service structure based on passed netconfig and port
 */
SVCXPRT *svc_create_nconf(rpcprog_t program, struct netconfig *nconf, int port)
{
	SVCXPRT *xprt;

	if (!port)
		port = get_service_port(program, nconf->nc_proto);

	if (port) {
		struct addrinfo *ai = svc_create_bindaddr(nconf, port);
		int fd;

		if (!ai)
			return NULL;

		fd = svc_create_sock(ai);
		freeaddrinfo(ai);
		if (fd < 0)
			return NULL;
		xprt = svc_tli_create(fd, nconf, NULL, 0, 0);
		if (!xprt) {
			close(fd);
			return NULL;
		}
	} else {
		xprt = svc_tli_create(RPC_ANYFD, nconf, NULL, 0, 0);
	}
	return xprt;
}
