#include "builtin.h"
#include "cache.h"
#include "config.h"
#include "commit.h"
#include "refs.h"
#include "diff.h"
#include "revision.h"
#include "parse-options.h"
#include "repository.h"
#include "commit-reach.h"

static int show_merge_base(struct commit **rev, int rev_nr, int show_all)
{
	struct commit_list *result, *r;

	result = get_merge_bases_many_dirty(rev[0], rev_nr - 1, rev + 1);

	if (!result)
		return 1;

	for (r = result; r; r = r->next) {
		printf("%s\n", oid_to_hex(&r->item->object.oid));
		if (!show_all)
			break;
	}

	free_commit_list(result);
	return 0;
}

static const char * const merge_base_usage[] = {
	N_("git merge-base [-a | --all] <commit> <commit>..."),
	N_("git merge-base [-a | --all] --octopus <commit>..."),
	N_("git merge-base --independent <commit>..."),
	N_("git merge-base --is-ancestor <commit> <commit>"),
	N_("git merge-base --fork-point <ref> [<commit>]"),
	NULL
};

static struct commit *get_commit_reference(const char *arg)
{
	struct object_id revkey;
	struct commit *r;

	if (get_oid(arg, &revkey))
		die("Not a valid object name %s", arg);
	r = lookup_commit_reference(the_repository, &revkey);
	if (!r)
		die("Not a valid commit name %s", arg);

	return r;
}

static int handle_independent(int count, const char **args)
{
	struct commit_list *revs = NULL, *rev;
	int i;

	for (i = count - 1; i >= 0; i--)
		commit_list_insert(get_commit_reference(args[i]), &revs);

	reduce_heads_replace(&revs);

	if (!revs)
		return 1;

	for (rev = revs; rev; rev = rev->next)
		printf("%s\n", oid_to_hex(&rev->item->object.oid));

	free_commit_list(revs);
	return 0;
}

static int handle_octopus(int count, const char **args, int show_all)
{
	struct commit_list *revs = NULL;
	struct commit_list *result, *rev;
	int i;

	for (i = count - 1; i >= 0; i--)
		commit_list_insert(get_commit_reference(args[i]), &revs);

	result = get_octopus_merge_bases(revs);
	free_commit_list(revs);
	reduce_heads_replace(&result);

	if (!result)
		return 1;

	for (rev = result; rev; rev = rev->next) {
		printf("%s\n", oid_to_hex(&rev->item->object.oid));
		if (!show_all)
			break;
	}

	free_commit_list(result);
	return 0;
}

static int handle_is_ancestor(int argc, const char **argv)
{
	struct commit *one, *two;

	if (argc != 2)
		die("--is-ancestor takes exactly two commits");
	one = get_commit_reference(argv[0]);
	two = get_commit_reference(argv[1]);
	if (in_merge_bases(one, two))
		return 0;
	else
		return 1;
}

static int handle_fork_point(int argc, const char **argv)
{
	struct object_id oid;
	char *refname;
	struct commit *derived, *fork_point;
	const char *commitname;

	switch (dwim_ref(argv[0], strlen(argv[0]), &oid, &refname)) {
	case 0:
		die("No such ref: '%s'", argv[0]);
	case 1:
		break; /* good */
	default:
		die("Ambiguous refname: '%s'", argv[0]);
	}

	commitname = (argc == 2) ? argv[1] : "HEAD";
	if (get_oid(commitname, &oid))
		die("Not a valid object name: '%s'", commitname);

	derived = lookup_commit_reference(the_repository, &oid);

	fork_point = get_fork_point(refname, derived);

	if (!fork_point)
		return 1;

	printf("%s\n", oid_to_hex(&fork_point->object.oid));
	return 0;
}

int cmd_merge_base(int argc, const char **argv, const char *prefix)
{
	struct commit **rev;
	int rev_nr = 0;
	int show_all = 0;
	int cmdmode = 0;

	struct option options[] = {
		OPT_BOOL('a', "all", &show_all, N_("output all common ancestors")),
		OPT_CMDMODE(0, "octopus", &cmdmode,
			    N_("find ancestors for a single n-way merge"), 'o'),
		OPT_CMDMODE(0, "independent", &cmdmode,
			    N_("list revs not reachable from others"), 'r'),
		OPT_CMDMODE(0, "is-ancestor", &cmdmode,
			    N_("is the first one ancestor of the other?"), 'a'),
		OPT_CMDMODE(0, "fork-point", &cmdmode,
			    N_("find where <commit> forked from reflog of <ref>"), 'f'),
		OPT_END()
	};

	git_config(git_default_config, NULL);
	argc = parse_options(argc, argv, prefix, options, merge_base_usage, 0);

	if (cmdmode == 'a') {
		if (argc < 2)
			usage_with_options(merge_base_usage, options);
		if (show_all)
			die("--is-ancestor cannot be used with --all");
		return handle_is_ancestor(argc, argv);
	}

	if (cmdmode == 'r' && show_all)
		die("--independent cannot be used with --all");

	if (cmdmode == 'o')
		return handle_octopus(argc, argv, show_all);

	if (cmdmode == 'r')
		return handle_independent(argc, argv);

	if (cmdmode == 'f') {
		if (argc < 1 || 2 < argc)
			usage_with_options(merge_base_usage, options);
		return handle_fork_point(argc, argv);
	}

	if (argc < 2)
		usage_with_options(merge_base_usage, options);

	ALLOC_ARRAY(rev, argc);
	while (argc-- > 0)
		rev[rev_nr++] = get_commit_reference(*argv++);
	return show_merge_base(rev, rev_nr, show_all);
}
