/*
 * Implementation of git-merge-ours.sh as builtin
 *
 * Copyright (c) 2007 Thomas Harning Jr
 * Original:
 * Original Copyright (c) 2005 Junio C Hamano
 *
 * Pretend we resolved the heads, but declare our tree trumps everybody else.
 */
#define USE_THE_INDEX_COMPATIBILITY_MACROS
#include "git-compat-util.h"
#include "builtin.h"
#include "diff.h"

static const char builtin_merge_ours_usage[] =
	"git merge-ours <base>... -- HEAD <remote>...";

int cmd_merge_ours(int argc, const char **argv, const char *prefix)
{
	if (argc == 2 && !strcmp(argv[1], "-h"))
		usage(builtin_merge_ours_usage);

	/*
	 * The contents of the current index becomes the tree we
	 * commit.  The index must match HEAD, or this merge cannot go
	 * through.
	 */
	if (read_cache() < 0)
		die_errno("read_cache failed");
	if (index_differs_from(the_repository, "HEAD", NULL, 0))
		exit(2);
	exit(0);
}
