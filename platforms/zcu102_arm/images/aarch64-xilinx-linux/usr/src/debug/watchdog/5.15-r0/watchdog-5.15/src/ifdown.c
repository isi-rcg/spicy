/* $Header: /cvsroot/watchdog/watchdog/src/ifdown.c,v 1.2 2006/07/31 09:39:23 meskes Exp $ */

/*
 * ifdown.c	Find all network interfaces on the system and
 *		shut them down.
 *
 */
char *v_ifdown = "@(#)ifdown.c  1.10  21-Apr-1997  miquels@cistron.nl";

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/errno.h>

#include <net/if.h>
#include <netinet/in.h>

#define MAX_IFS	64

int ifdown(void)
{
	struct ifreq ifr[MAX_IFS];
	struct ifconf ifc;
	int i, fd;
	int numif;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "ifdown: ");
		perror("socket");
		return -1;
	}
	ifc.ifc_len = sizeof(ifr);
	ifc.ifc_req = ifr;

	if (ioctl(fd, SIOCGIFCONF, &ifc) < 0) {
		fprintf(stderr, "ifdown: ");
		perror("SIOCGIFCONF");
		close(fd);
		return -1;
	}
	numif = ifc.ifc_len / sizeof(struct ifreq);

	for (i = 0; i < numif; i++) {
		if (strcmp(ifr[i].ifr_name, "lo") == 0)
			continue;
		if (strchr(ifr[i].ifr_name, ':') != NULL)
			continue;
		ifr[i].ifr_flags &= ~(IFF_UP);
		if (ioctl(fd, SIOCSIFFLAGS, &ifr[i]) < 0) {
			fprintf(stderr, "ifdown: shutdown ");
			perror(ifr[i].ifr_name);
		}
	}

	close(fd);

	return 0;
}
