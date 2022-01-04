/*
 * rev-parse.c
 *
 * Copyright (C) Linus Torvalds, 2005
 */
#define USE_THE_INDEX_COMPATIBILITY_MACROS
#include "cache.h"
#include "config.h"
#include "commit.h"
#include "refs.h"
#include "quote.h"
#include "builtin.h"
#include "parse-options.h"
#include "diff.h"
#include "revision.h"
#include "split-index.h"
#include "submodule.h"
#include "commit-reach.h"

#define DO_REVS		1
#define DO_NOREV	2
#define DO_FLAGS	4
#define DO_NONFLAGS	8
static int filter = ~0;

static const char *def;

#define NORMAL 0
#define REVERSED 1
static int show_type = NORMAL;

#define SHOW_SYMBOLIC_ASIS 1
#define SHOW_SYMBOLIC_FULL 2
static int symbolic;
static int abbrev;
static int abbrev_ref;
static int abbrev_ref_strict;
static int output_sq;

static int stuck_long;
static struct string_list *ref_excludes;

/*
 * Some arguments are relevant "revision" arguments,
 * others are about output format or other details.
 * This sorts it all out.
 */
static int is_rev_argument(const char *arg)
{
	static const char *rev_args[] = {
		"--all",
		"--bisect",
		"--dense",
		"--branches=",
		"--branches",
		"--header",
		"--ignore-missing",
		"--max-age=",
		"--max-count=",
		"--min-age=",
		"--no-merges",
		"--min-parents=",
		"--no-min-parents",
		"--max-parents=",
		"--no-max-parents",
		"--objects",
		"--objects-edge",
		"--parents",
		"--pretty",
		"--remotes=",
		"--remotes",
		"--glob=",
		"--sparse",
		"--tags=",
		"--tags",
		"--topo-order",
		"--date-order",
		"--unpacked",
		NULL
	};
	const char **p = rev_args;

	/* accept -<digit>, like traditional "head" */
	if ((*arg == '-') && isdigit(arg[1]))
		return 1;

	for (;;) {
		const char *str = *p++;
		int len;
		if (!str)
			return 0;
		len = strlen(str);
		if (!strcmp(arg, str) ||
		    (str[len-1] == '=' && !strncmp(arg, str, len)))
			return 1;
	}
}

/* Output argument as a string, either SQ or normal */
static void show(const char *arg)
{
	if (output_sq) {
		int sq = '\'', ch;

		putchar(sq);
		while ((ch = *arg++)) {
			if (ch == sq)
				fputs("'\\'", stdout);
			putchar(ch);
		}
		putchar(sq);
		putchar(' ');
	}
	else
		puts(arg);
}

/* Like show(), but with a negation prefix according to type */
static void show_with_type(int type, const char *arg)
{
	if (type != show_type)
		putchar('^');
	show(arg);
}

/* Output a revision, only if filter allows it */
static void show_rev(int type, const struct object_id *oid, const char *name)
{
	if (!(filter & DO_REVS))
		return;
	def = NULL;

	if ((symbolic || abbrev_ref) && name) {
		if (symbolic == SHOW_SYMBOLIC_FULL || abbrev_ref) {
			struct object_id discard;
			char *full;

			switch (dwim_ref(name, strlen(name), &discard, &full)) {
			case 0:
				/*
				 * Not found -- not a ref.  We could
				 * emit "name" here, but symbolic-full
				 * users are interested in finding the
				 * refs spelled in full, and they would
				 * need to filter non-refs if we did so.
				 */
				break;
			case 1: /* happy */
				if (abbrev_ref)
					full = shorten_unambiguous_ref(full,
						abbrev_ref_strict);
				show_with_type(type, full);
				break;
			default: /* ambiguous */
				error("refname '%s' is ambiguous", name);
				break;
			}
			free(full);
		} else {
			show_with_type(type, name);
		}
	}
	else if (abbrev)
		show_with_type(type, find_unique_abbrev(oid, abbrev));
	else
		show_with_type(type, oid_to_hex(oid));
}

/* Output a flag, only if filter allows it. */
static int show_flag(const char *arg)
{
	if (!(filter & DO_FLAGS))
		return 0;
	if (filter & (is_rev_argument(arg) ? DO_REVS : DO_NOREV)) {
		show(arg);
		return 1;
	}
	return 0;
}

static int show_default(void)
{
	const char *s = def;

	if (s) {
		struct object_id oid;

		def = NULL;
		if (!get_oid(s, &oid)) {
			show_rev(NORMAL, &oid, s);
			return 1;
		}
	}
	return 0;
}

static int show_reference(const char *refname, const struct object_id *oid, int flag, void *cb_data)
{
	if (ref_excluded(ref_excludes, refname))
		return 0;
	show_rev(NORMAL, oid, refname);
	return 0;
}

static int anti_reference(const char *refname, const struct object_id *oid, int flag, void *cb_data)
{
	show_rev(REVERSED, oid, refname);
	return 0;
}

static int show_abbrev(const struct object_id *oid, void *cb_data)
{
	show_rev(NORMAL, oid, NULL);
	return 0;
}

static void show_datestring(const char *flag, const char *datestr)
{
	char *buffer;

	/* date handling requires both flags and revs */
	if ((filter & (DO_FLAGS | DO_REVS)) != (DO_FLAGS | DO_REVS))
		return;
	buffer = xstrfmt("%s%"PRItime, flag, approxidate(datestr));
	show(buffer);
	free(buffer);
}

static int show_file(const char *arg, int output_prefix)
{
	show_default();
	if ((filter & (DO_NONFLAGS|DO_NOREV)) == (DO_NONFLAGS|DO_NOREV)) {
		if (output_prefix) {
			const char *prefix = startup_info->prefix;
			char *fname = prefix_filename(prefix, arg);
			show(fname);
			free(fname);
		} else
			show(arg);
		return 1;
	}
	return 0;
}

static int try_difference(const char *arg)
{
	char *dotdot;
	struct object_id start_oid;
	struct object_id end_oid;
	const char *end;
	const char *start;
	int symmetric;
	static const char head_by_default[] = "HEAD";

	if (!(dotdot = strstr(arg, "..")))
		return 0;
	end = dotdot + 2;
	start = arg;
	symmetric = (*end == '.');

	*dotdot = 0;
	end += symmetric;

	if (!*end)
		end = head_by_default;
	if (dotdot == arg)
		start = head_by_default;

	if (start == head_by_default && end == head_by_default &&
	    !symmetric) {
		/*
		 * Just ".."?  That is not a range but the
		 * pathspec for the parent directory.
		 */
		*dotdot = '.';
		return 0;
	}

	if (!get_oid_committish(start, &start_oid) && !get_oid_committish(end, &end_oid)) {
		show_rev(NORMAL, &end_oid, end);
		show_rev(symmetric ? NORMAL : REVERSED, &start_oid, start);
		if (symmetric) {
			struct commit_list *exclude;
			struct commit *a, *b;
			a = lookup_commit_reference(the_repository, &start_oid);
			b = lookup_commit_reference(the_repository, &end_oid);
			if (!a || !b) {
				*dotdot = '.';
				return 0;
			}
			exclude = get_merge_bases(a, b);
			while (exclude) {
				struct commit *commit = pop_commit(&exclude);
				show_rev(REVERSED, &commit->object.oid, NULL);
			}
		}
		*dotdot = '.';
		return 1;
	}
	*dotdot = '.';
	return 0;
}

static int try_parent_shorthands(const char *arg)
{
	char *dotdot;
	struct object_id oid;
	struct commit *commit;
	struct commit_list *parents;
	int parent_number;
	int include_rev = 0;
	int include_parents = 0;
	int exclude_parent = 0;

	if ((dotdot = strstr(arg, "^!"))) {
		include_rev = 1;
		if (dotdot[2])
			return 0;
	} else if ((dotdot = strstr(arg, "^@"))) {
		include_parents = 1;
		if (dotdot[2])
			return 0;
	} else if ((dotdot = strstr(arg, "^-"))) {
		include_rev = 1;
		exclude_parent = 1;

		if (dotdot[2]) {
			char *end;
			exclude_parent = strtoul(dotdot + 2, &end, 10);
			if (*end != '\0' || !exclude_parent)
				return 0;
		}
	} else
		return 0;

	*dotdot = 0;
	if (get_oid_committish(arg, &oid) ||
	    !(commit = lookup_commit_reference(the_repository, &oid))) {
		*dotdot = '^';
		return 0;
	}

	if (exclude_parent &&
	    exclude_parent > commit_list_count(commit->parents)) {
		*dotdot = '^';
		return 0;
	}

	if (include_rev)
		show_rev(NORMAL, &oid, arg);
	for (parents = commit->parents, parent_number = 1;
	     parents;
	     parents = parents->next, parent_number++) {
		char *name = NULL;

		if (exclude_parent && parent_number != exclude_parent)
			continue;

		if (symbolic)
			name = xstrfmt("%s^%d", arg, parent_number);
		show_rev(include_parents ? NORMAL : REVERSED,
			 &parents->item->object.oid, name);
		free(name);
	}

	*dotdot = '^';
	return 1;
}

static int parseopt_dump(const struct option *o, const char *arg, int unset)
{
	struct strbuf *parsed = o->value;
	if (unset)
		strbuf_addf(parsed, " --no-%s", o->long_name);
	else if (o->short_name && (o->long_name == NULL || !stuck_long))
		strbuf_addf(parsed, " -%c", o->short_name);
	else
		strbuf_addf(parsed, " --%s", o->long_name);
	if (arg) {
		if (!stuck_long)
			strbuf_addch(parsed, ' ');
		else if (o->long_name)
			strbuf_addch(parsed, '=');
		sq_quote_buf(parsed, arg);
	}
	return 0;
}

static const char *skipspaces(const char *s)
{
	while (isspace(*s))
		s++;
	return s;
}

static char *findspace(const char *s)
{
	for (; *s; s++)
		if (isspace(*s))
			return (char*)s;
	return NULL;
}

static int cmd_parseopt(int argc, const char **argv, const char *prefix)
{
	static int keep_dashdash = 0, stop_at_non_option = 0;
	static char const * const parseopt_usage[] = {
		N_("git rev-parse --parseopt [<options>] -- [<args>...]"),
		NULL
	};
	static struct option parseopt_opts[] = {
		OPT_BOOL(0, "keep-dashdash", &keep_dashdash,
					N_("keep the `--` passed as an arg")),
		OPT_BOOL(0, "stop-at-non-option", &stop_at_non_option,
					N_("stop parsing after the "
					   "first non-option argument")),
		OPT_BOOL(0, "stuck-long", &stuck_long,
					N_("output in stuck long form")),
		OPT_END(),
	};
	static const char * const flag_chars = "*=?!";

	struct strbuf sb = STRBUF_INIT, parsed = STRBUF_INIT;
	const char **usage = NULL;
	struct option *opts = NULL;
	int onb = 0, osz = 0, unb = 0, usz = 0;

	strbuf_addstr(&parsed, "set --");
	argc = parse_options(argc, argv, prefix, parseopt_opts, parseopt_usage,
	                     PARSE_OPT_KEEP_DASHDASH);
	if (argc < 1 || strcmp(argv[0], "--"))
		usage_with_options(parseopt_usage, parseopt_opts);

	/* get the usage up to the first line with a -- on it */
	for (;;) {
		if (strbuf_getline(&sb, stdin) == EOF)
			die("premature end of input");
		ALLOC_GROW(usage, unb + 1, usz);
		if (!strcmp("--", sb.buf)) {
			if (unb < 1)
				die("no usage string given before the `--' separator");
			usage[unb] = NULL;
			break;
		}
		usage[unb++] = strbuf_detach(&sb, NULL);
	}

	/* parse: (<short>|<short>,<long>|<long>)[*=?!]*<arghint>? SP+ <help> */
	while (strbuf_getline(&sb, stdin) != EOF) {
		const char *s;
		char *help;
		struct option *o;

		if (!sb.len)
			continue;

		ALLOC_GROW(opts, onb + 1, osz);
		memset(opts + onb, 0, sizeof(opts[onb]));

		o = &opts[onb++];
		help = findspace(sb.buf);
		if (!help || sb.buf == help) {
			o->type = OPTION_GROUP;
			o->help = xstrdup(skipspaces(sb.buf));
			continue;
		}

		*help = '\0';

		o->type = OPTION_CALLBACK;
		o->help = xstrdup(skipspaces(help+1));
		o->value = &parsed;
		o->flags = PARSE_OPT_NOARG;
		o->callback = &parseopt_dump;

		/* name(s) */
		s = strpbrk(sb.buf, flag_chars);
		if (s == NULL)
			s = help;

		if (s - sb.buf == 1) /* short option only */
			o->short_name = *sb.buf;
		else if (sb.buf[1] != ',') /* long option only */
			o->long_name = xmemdupz(sb.buf, s - sb.buf);
		else {
			o->short_name = *sb.buf;
			o->long_name = xmemdupz(sb.buf + 2, s - sb.buf - 2);
		}

		/* flags */
		while (s < help) {
			switch (*s++) {
			case '=':
				o->flags &= ~PARSE_OPT_NOARG;
				continue;
			case '?':
				o->flags &= ~PARSE_OPT_NOARG;
				o->flags |= PARSE_OPT_OPTARG;
				continue;
			case '!':
				o->flags |= PARSE_OPT_NONEG;
				continue;
			case '*':
				o->flags |= PARSE_OPT_HIDDEN;
				continue;
			}
			s--;
			break;
		}

		if (s < help)
			o->argh = xmemdupz(s, help - s);
	}
	strbuf_release(&sb);

	/* put an OPT_END() */
	ALLOC_GROW(opts, onb + 1, osz);
	memset(opts + onb, 0, sizeof(opts[onb]));
	argc = parse_options(argc, argv, prefix, opts, usage,
			(keep_dashdash ? PARSE_OPT_KEEP_DASHDASH : 0) |
			(stop_at_non_option ? PARSE_OPT_STOP_AT_NON_OPTION : 0) |
			PARSE_OPT_SHELL_EVAL);

	strbuf_addstr(&parsed, " --");
	sq_quote_argv(&parsed, argv);
	puts(parsed.buf);
	return 0;
}

static int cmd_sq_quote(int argc, const char **argv)
{
	struct strbuf buf = STRBUF_INIT;

	if (argc)
		sq_quote_argv(&buf, argv);
	printf("%s\n", buf.buf);
	strbuf_release(&buf);

	return 0;
}

static void die_no_single_rev(int quiet)
{
	if (quiet)
		exit(1);
	else
		die("Needed a single revision");
}

static const char builtin_rev_parse_usage[] =
N_("git rev-parse --parseopt [<options>] -- [<args>...]\n"
   "   or: git rev-parse --sq-quote [<arg>...]\n"
   "   or: git rev-parse [<options>] [<arg>...]\n"
   "\n"
   "Run \"git rev-parse --parseopt -h\" for more information on the first usage.");

/*
 * Parse "opt" or "opt=<value>", setting value respectively to either
 * NULL or the string after "=".
 */
static int opt_with_value(const char *arg, const char *opt, const char **value)
{
	if (skip_prefix(arg, opt, &arg)) {
		if (!*arg) {
			*value = NULL;
			return 1;
		}
		if (*arg++ == '=') {
			*value = arg;
			return 1;
		}
	}
	return 0;
}

static void handle_ref_opt(const char *pattern, const char *prefix)
{
	if (pattern)
		for_each_glob_ref_in(show_reference, pattern, prefix, NULL);
	else
		for_each_ref_in(prefix, show_reference, NULL);
	clear_ref_exclusion(&ref_excludes);
}

int cmd_rev_parse(int argc, const char **argv, const char *prefix)
{
	int i, as_is = 0, verify = 0, quiet = 0, revs_count = 0, type = 0;
	int did_repo_setup = 0;
	int has_dashdash = 0;
	int output_prefix = 0;
	struct object_id oid;
	unsigned int flags = 0;
	const char *name = NULL;
	struct object_context unused;
	struct strbuf buf = STRBUF_INIT;

	if (argc > 1 && !strcmp("--parseopt", argv[1]))
		return cmd_parseopt(argc - 1, argv + 1, prefix);

	if (argc > 1 && !strcmp("--sq-quote", argv[1]))
		return cmd_sq_quote(argc - 2, argv + 2);

	if (argc > 1 && !strcmp("-h", argv[1]))
		usage(builtin_rev_parse_usage);

	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--")) {
			has_dashdash = 1;
			break;
		}
	}

	/* No options; just report on whether we're in a git repo or not. */
	if (argc == 1) {
		setup_git_directory();
		git_config(git_default_config, NULL);
		return 0;
	}

	for (i = 1; i < argc; i++) {
		const char *arg = argv[i];

		if (!strcmp(arg, "--local-env-vars")) {
			int i;
			for (i = 0; local_repo_env[i]; i++)
				printf("%s\n", local_repo_env[i]);
			continue;
		}
		if (!strcmp(arg, "--resolve-git-dir")) {
			const char *gitdir = argv[++i];
			if (!gitdir)
				die("--resolve-git-dir requires an argument");
			gitdir = resolve_gitdir(gitdir);
			if (!gitdir)
				die("not a gitdir '%s'", argv[i]);
			puts(gitdir);
			continue;
		}

		/* The rest of the options require a git repository. */
		if (!did_repo_setup) {
			prefix = setup_git_directory();
			git_config(git_default_config, NULL);
			did_repo_setup = 1;
		}

		if (!strcmp(arg, "--git-path")) {
			if (!argv[i + 1])
				die("--git-path requires an argument");
			strbuf_reset(&buf);
			puts(relative_path(git_path("%s", argv[i + 1]),
					   prefix, &buf));
			i++;
			continue;
		}
		if (as_is) {
			if (show_file(arg, output_prefix) && as_is < 2)
				verify_filename(prefix, arg, 0);
			continue;
		}
		if (!strcmp(arg,"-n")) {
			if (++i >= argc)
				die("-n requires an argument");
			if ((filter & DO_FLAGS) && (filter & DO_REVS)) {
				show(arg);
				show(argv[i]);
			}
			continue;
		}
		if (starts_with(arg, "-n")) {
			if ((filter & DO_FLAGS) && (filter & DO_REVS))
				show(arg);
			continue;
		}

		if (*arg == '-') {
			if (!strcmp(arg, "--")) {
				as_is = 2;
				/* Pass on the "--" if we show anything but files.. */
				if (filter & (DO_FLAGS | DO_REVS))
					show_file(arg, 0);
				continue;
			}
			if (!strcmp(arg, "--default")) {
				def = argv[++i];
				if (!def)
					die("--default requires an argument");
				continue;
			}
			if (!strcmp(arg, "--prefix")) {
				prefix = argv[++i];
				if (!prefix)
					die("--prefix requires an argument");
				startup_info->prefix = prefix;
				output_prefix = 1;
				continue;
			}
			if (!strcmp(arg, "--revs-only")) {
				filter &= ~DO_NOREV;
				continue;
			}
			if (!strcmp(arg, "--no-revs")) {
				filter &= ~DO_REVS;
				continue;
			}
			if (!strcmp(arg, "--flags")) {
				filter &= ~DO_NONFLAGS;
				continue;
			}
			if (!strcmp(arg, "--no-flags")) {
				filter &= ~DO_FLAGS;
				continue;
			}
			if (!strcmp(arg, "--verify")) {
				filter &= ~(DO_FLAGS|DO_NOREV);
				verify = 1;
				continue;
			}
			if (!strcmp(arg, "--quiet") || !strcmp(arg, "-q")) {
				quiet = 1;
				flags |= GET_OID_QUIETLY;
				continue;
			}
			if (opt_with_value(arg, "--short", &arg)) {
				filter &= ~(DO_FLAGS|DO_NOREV);
				verify = 1;
				abbrev = DEFAULT_ABBREV;
				if (!arg)
					continue;
				abbrev = strtoul(arg, NULL, 10);
				if (abbrev < MINIMUM_ABBREV)
					abbrev = MINIMUM_ABBREV;
				else if (40 <= abbrev)
					abbrev = 40;
				continue;
			}
			if (!strcmp(arg, "--sq")) {
				output_sq = 1;
				continue;
			}
			if (!strcmp(arg, "--not")) {
				show_type ^= REVERSED;
				continue;
			}
			if (!strcmp(arg, "--symbolic")) {
				symbolic = SHOW_SYMBOLIC_ASIS;
				continue;
			}
			if (!strcmp(arg, "--symbolic-full-name")) {
				symbolic = SHOW_SYMBOLIC_FULL;
				continue;
			}
			if (opt_with_value(arg, "--abbrev-ref", &arg)) {
				abbrev_ref = 1;
				abbrev_ref_strict = warn_ambiguous_refs;
				if (arg) {
					if (!strcmp(arg, "strict"))
						abbrev_ref_strict = 1;
					else if (!strcmp(arg, "loose"))
						abbrev_ref_strict = 0;
					else
						die("unknown mode for --abbrev-ref: %s",
						    arg);
				}
				continue;
			}
			if (!strcmp(arg, "--all")) {
				for_each_ref(show_reference, NULL);
				clear_ref_exclusion(&ref_excludes);
				continue;
			}
			if (skip_prefix(arg, "--disambiguate=", &arg)) {
				for_each_abbrev(arg, show_abbrev, NULL);
				continue;
			}
			if (!strcmp(arg, "--bisect")) {
				for_each_fullref_in("refs/bisect/bad", show_reference, NULL, 0);
				for_each_fullref_in("refs/bisect/good", anti_reference, NULL, 0);
				continue;
			}
			if (opt_with_value(arg, "--branches", &arg)) {
				handle_ref_opt(arg, "refs/heads/");
				continue;
			}
			if (opt_with_value(arg, "--tags", &arg)) {
				handle_ref_opt(arg, "refs/tags/");
				continue;
			}
			if (skip_prefix(arg, "--glob=", &arg)) {
				handle_ref_opt(arg, NULL);
				continue;
			}
			if (opt_with_value(arg, "--remotes", &arg)) {
				handle_ref_opt(arg, "refs/remotes/");
				continue;
			}
			if (skip_prefix(arg, "--exclude=", &arg)) {
				add_ref_exclusion(&ref_excludes, arg);
				continue;
			}
			if (!strcmp(arg, "--show-toplevel")) {
				const char *work_tree = get_git_work_tree();
				if (work_tree)
					puts(work_tree);
				continue;
			}
			if (!strcmp(arg, "--show-superproject-working-tree")) {
				const char *superproject = get_superproject_working_tree();
				if (superproject)
					puts(superproject);
				continue;
			}
			if (!strcmp(arg, "--show-prefix")) {
				if (prefix)
					puts(prefix);
				else
					putchar('\n');
				continue;
			}
			if (!strcmp(arg, "--show-cdup")) {
				const char *pfx = prefix;
				if (!is_inside_work_tree()) {
					const char *work_tree =
						get_git_work_tree();
					if (work_tree)
						printf("%s\n", work_tree);
					continue;
				}
				while (pfx) {
					pfx = strchr(pfx, '/');
					if (pfx) {
						pfx++;
						printf("../");
					}
				}
				putchar('\n');
				continue;
			}
			if (!strcmp(arg, "--git-dir") ||
			    !strcmp(arg, "--absolute-git-dir")) {
				const char *gitdir = getenv(GIT_DIR_ENVIRONMENT);
				char *cwd;
				int len;
				if (arg[2] == 'g') {	/* --git-dir */
					if (gitdir) {
						puts(gitdir);
						continue;
					}
					if (!prefix) {
						puts(".git");
						continue;
					}
				} else {		/* --absolute-git-dir */
					if (!gitdir && !prefix)
						gitdir = ".git";
					if (gitdir) {
						puts(real_path(gitdir));
						continue;
					}
				}
				cwd = xgetcwd();
				len = strlen(cwd);
				printf("%s%s.git\n", cwd, len && cwd[len-1] != '/' ? "/" : "");
				free(cwd);
				continue;
			}
			if (!strcmp(arg, "--git-common-dir")) {
				strbuf_reset(&buf);
				puts(relative_path(get_git_common_dir(),
						   prefix, &buf));
				continue;
			}
			if (!strcmp(arg, "--is-inside-git-dir")) {
				printf("%s\n", is_inside_git_dir() ? "true"
						: "false");
				continue;
			}
			if (!strcmp(arg, "--is-inside-work-tree")) {
				printf("%s\n", is_inside_work_tree() ? "true"
						: "false");
				continue;
			}
			if (!strcmp(arg, "--is-bare-repository")) {
				printf("%s\n", is_bare_repository() ? "true"
						: "false");
				continue;
			}
			if (!strcmp(arg, "--is-shallow-repository")) {
				printf("%s\n",
						is_repository_shallow(the_repository) ? "true"
						: "false");
				continue;
			}
			if (!strcmp(arg, "--shared-index-path")) {
				if (read_cache() < 0)
					die(_("Could not read the index"));
				if (the_index.split_index) {
					const struct object_id *oid = &the_index.split_index->base_oid;
					const char *path = git_path("sharedindex.%s", oid_to_hex(oid));
					strbuf_reset(&buf);
					puts(relative_path(path, prefix, &buf));
				}
				continue;
			}
			if (skip_prefix(arg, "--since=", &arg)) {
				show_datestring("--max-age=", arg);
				continue;
			}
			if (skip_prefix(arg, "--after=", &arg)) {
				show_datestring("--max-age=", arg);
				continue;
			}
			if (skip_prefix(arg, "--before=", &arg)) {
				show_datestring("--min-age=", arg);
				continue;
			}
			if (skip_prefix(arg, "--until=", &arg)) {
				show_datestring("--min-age=", arg);
				continue;
			}
			if (show_flag(arg) && verify)
				die_no_single_rev(quiet);
			continue;
		}

		/* Not a flag argument */
		if (try_difference(arg))
			continue;
		if (try_parent_shorthands(arg))
			continue;
		name = arg;
		type = NORMAL;
		if (*arg == '^') {
			name++;
			type = REVERSED;
		}
		if (!get_oid_with_context(the_repository, name,
					  flags, &oid, &unused)) {
			if (verify)
				revs_count++;
			else
				show_rev(type, &oid, name);
			continue;
		}
		if (verify)
			die_no_single_rev(quiet);
		if (has_dashdash)
			die("bad revision '%s'", arg);
		as_is = 1;
		if (!show_file(arg, output_prefix))
			continue;
		verify_filename(prefix, arg, 1);
	}
	strbuf_release(&buf);
	if (verify) {
		if (revs_count == 1) {
			show_rev(type, &oid, name);
			return 0;
		} else if (revs_count == 0 && show_default())
			return 0;
		die_no_single_rev(quiet);
	} else
		show_default();
	return 0;
}
