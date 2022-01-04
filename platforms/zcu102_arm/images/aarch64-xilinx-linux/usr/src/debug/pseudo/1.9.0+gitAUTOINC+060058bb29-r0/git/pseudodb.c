/*
 * pseudodb.c, (unimplemented) database maintenance utility.
 *
 * Copyright (c) 2008-2010 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>

#include "pseudo.h"
#include "pseudo_ipc.h"
#include "pseudo_db.h"

int
main(int argc, char **argv) {
	pseudo_msg_t *msg;
	int rc;

	if (argc < 2) {
		fprintf(stderr, "Usage: pseudodb <filename>\n");
		exit(1);
	}
	msg = pseudo_msg_new(0, argv[1]);
	rc = pdb_find_file_path(msg);
	if (rc) {
		printf("error.\n");
		return 1;
	} else {
		printf("%s: %o %x\n", msg->path, (int) msg->mode, (int) msg->rdev);
		return 0;
	}
}
