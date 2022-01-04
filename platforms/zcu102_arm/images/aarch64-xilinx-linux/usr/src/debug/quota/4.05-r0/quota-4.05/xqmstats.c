/*
 *	Display XFS quota manager statistics from /proc.
 *      Copyright (c) 2001-2003 Silicon Graphics, Inc.
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "common.h"
#include "pot.h"

#define XQMFILE		"/proc/fs/xfs/xqm"
#define STATFILE	"/proc/fs/xfs/stat"
#define XQMSTATS	"/proc/fs/xfs/xqmstat"

char *progname;

int main(int argc, char **argv)
{
	FILE *stats, *xqm;
	char buffer[256];
	unsigned values[8];

	gettexton();
	progname = basename(argv[0]);

	memset(values, 0, sizeof(unsigned) * 8);

	if ((xqm = fopen(XQMFILE, "r")) == NULL) {
		errstr(_("The running kernel does not support XFS\n"));
		return 1;
	}
	if ((stats = fopen(XQMSTATS, "r")) == NULL) {
		if ((stats = fopen(STATFILE, "r")) == NULL) {
			errstr(_("The running kernel does not support XFS\n"));
			fclose(xqm);
			return 1;
		}
	}
	while (!feof(stats)) {
		fgets(buffer, 256, stats);
		if (sscanf(buffer, "qm %u %u %u %u %u %u %u %u\n",
			   &values[0], &values[1], &values[2], &values[3],
			   &values[4], &values[5], &values[6], &values[7]) == 8)
			break;
	}
	if (!feof(stats)) {
		printf(_("XFS Quota Manager dquot statistics\n"));
		printf(_("  reclaims:        %u\n"), values[0]);
		printf(_("  missed reclaims: %u\n"), values[1]);
		printf(_("  dquot dups:      %u\n"), values[2]);
		printf(_("  cache misses:    %u\n"), values[3]);
		printf(_("  cache hits:      %u\n"), values[4]);
		printf(_("  dquot wants:     %u\n"), values[5]);
		printf(_("  shake reclaims:  %u\n"), values[6]);
		printf(_("  inact reclaims:  %u\n"), values[7]);
	}
	if (fscanf(xqm, "%u %u %u %u\n",
			&values[0], &values[1], &values[2], &values[3]) == 4)
		printf(
		_("Maximum %u dquots (currently %u incore, %u on freelist)\n"),
			values[0], values[1], values[3]);
	fclose(stats);
	fclose(xqm);
	return 0;
}
