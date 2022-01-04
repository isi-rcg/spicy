/*
 * "git difftool" builtin command
 *
 * This is a wrapper around the GIT_EXTERNAL_DIFF-compatible
 * git-difftool--helper script.
 *
 * This script exports GIT_EXTERNAL_DIFF and GIT_PAGER for use by git.
 * The GIT_DIFF* variables are exported for use by git-difftool--helper.
 *
 * Any arguments that are unknown to this script are forwarded to 'git diff'.
 *
 * Copyright (C) 2016 Johannes Schindelin
 */
#define USE_THE_INDEX_COMPATIBILITY_MACROS
#include "cache.h"
#include "config.h"
#include "builtin.h"
#include "run-command.h"
#include "exec-cmd.h"
#include "parse-options.h"
#include "argv-array.h"
#include "strbuf.h"
#include "lockfile.h"
#include "object-store.h"
#include "dir.h"

static int trust_exit_code;

static const char *const builtin_difftool_usage[] = {
	N_("git difftool [<options>] [<commit> [<commit>]] [--] [<path>...]"),
	NULL
};

static int difftool_config(const char *var, const char *value, void *cb)
{
	if (!strcmp(var, "difftool.trustexitcode")) {
		trust_exit_code = git_config_bool(var, value);
		return 0;
	}

	return git_default_config(var, value, cb);
}

static int print_tool_help(void)
{
	const char *argv[] = { "mergetool", "--tool-help=diff", NULL };
	return run_command_v_opt(argv, RUN_GIT_CMD);
}

static int parse_index_info(char *p, int *mode1, int *mode2,
			    struct object_id *oid1, struct object_id *oid2,
			    char *status)
{
	if (*p != ':')
		return error("expected ':', got '%c'", *p);
	*mode1 = (int)strtol(p + 1, &p, 8);
	if (*p != ' ')
		return error("expected ' ', got '%c'", *p);
	*mode2 = (int)strtol(p + 1, &p, 8);
	if (*p != ' ')
		return error("expected ' ', got '%c'", *p);
	if (parse_oid_hex(++p, oid1, (const char **)&p))
		return error("expected object ID, got '%s'", p);
	if (*p != ' ')
		return error("expected ' ', got '%c'", *p);
	if (parse_oid_hex(++p, oid2, (const char **)&p))
		return error("expected object ID, got '%s'", p);
	if (*p != ' ')
		return error("expected ' ', got '%c'", *p);
	*status = *++p;
	if (!*status)
		return error("missing status");
	if (p[1] && !isdigit(p[1]))
		return error("unexpected trailer: '%s'", p + 1);
	return 0;
}

/*
 * Remove any trailing slash from $workdir
 * before starting to avoid double slashes in symlink targets.
 */
static void add_path(struct strbuf *buf, size_t base_len, const char *path)
{
	strbuf_setlen(buf, base_len);
	if (buf->len && buf->buf[buf->len - 1] != '/')
		strbuf_addch(buf, '/');
	strbuf_addstr(buf, path);
}

/*
 * Determine whether we can simply reuse the file in the worktree.
 */
static int use_wt_file(const char *workdir, const char *name,
		       struct object_id *oid)
{
	struct strbuf buf = STRBUF_INIT;
	struct stat st;
	int use = 0;

	strbuf_addstr(&buf, workdir);
	add_path(&buf, buf.len, name);

	if (!lstat(buf.buf, &st) && !S_ISLNK(st.st_mode)) {
		struct object_id wt_oid;
		int fd = open(buf.buf, O_RDONLY);

		if (fd >= 0 &&
		    !index_fd(&the_index, &wt_oid, fd, &st, OBJ_BLOB, name, 0)) {
			if (is_null_oid(oid)) {
				oidcpy(oid, &wt_oid);
				use = 1;
			} else if (oideq(oid, &wt_oid))
				use = 1;
		}
	}

	strbuf_release(&buf);

	return use;
}

struct working_tree_entry {
	struct hashmap_entry entry;
	char path[FLEX_ARRAY];
};

static int working_tree_entry_cmp(const void *unused_cmp_data,
				  const void *entry,
				  const void *entry_or_key,
				  const void *unused_keydata)
{
	const struct working_tree_entry *a = entry;
	const struct working_tree_entry *b = entry_or_key;
	return strcmp(a->path, b->path);
}

/*
 * The `left` and `right` entries hold paths for the symlinks hashmap,
 * and a SHA-1 surrounded by brief text for submodules.
 */
struct pair_entry {
	struct hashmap_entry entry;
	char left[PATH_MAX], right[PATH_MAX];
	const char path[FLEX_ARRAY];
};

static int pair_cmp(const void *unused_cmp_data,
		    const void *entry,
		    const void *entry_or_key,
		    const void *unused_keydata)
{
	const struct pair_entry *a = entry;
	const struct pair_entry *b = entry_or_key;

	return strcmp(a->path, b->path);
}

static void add_left_or_right(struct hashmap *map, const char *path,
			      const char *content, int is_right)
{
	struct pair_entry *e, *existing;

	FLEX_ALLOC_STR(e, path, path);
	hashmap_entry_init(e, strhash(path));
	existing = hashmap_get(map, e, NULL);
	if (existing) {
		free(e);
		e = existing;
	} else {
		e->left[0] = e->right[0] = '\0';
		hashmap_add(map, e);
	}
	strlcpy(is_right ? e->right : e->left, content, PATH_MAX);
}

struct path_entry {
	struct hashmap_entry entry;
	char path[FLEX_ARRAY];
};

static int path_entry_cmp(const void *unused_cmp_data,
			  const void *entry,
			  const void *entry_or_key,
			  const void *key)
{
	const struct path_entry *a = entry;
	const struct path_entry *b = entry_or_key;

	return strcmp(a->path, key ? key : b->path);
}

static void changed_files(struct hashmap *result, const char *index_path,
			  const char *workdir)
{
	struct child_process update_index = CHILD_PROCESS_INIT;
	struct child_process diff_files = CHILD_PROCESS_INIT;
	struct strbuf index_env = STRBUF_INIT, buf = STRBUF_INIT;
	const char *git_dir = absolute_path(get_git_dir()), *env[] = {
		NULL, NULL
	};
	FILE *fp;

	strbuf_addf(&index_env, "GIT_INDEX_FILE=%s", index_path);
	env[0] = index_env.buf;

	argv_array_pushl(&update_index.args,
			 "--git-dir", git_dir, "--work-tree", workdir,
			 "update-index", "--really-refresh", "-q",
			 "--unmerged", NULL);
	update_index.no_stdin = 1;
	update_index.no_stdout = 1;
	update_index.no_stderr = 1;
	update_index.git_cmd = 1;
	update_index.use_shell = 0;
	update_index.clean_on_exit = 1;
	update_index.dir = workdir;
	update_index.env = env;
	/* Ignore any errors of update-index */
	run_command(&update_index);

	argv_array_pushl(&diff_files.args,
			 "--git-dir", git_dir, "--work-tree", workdir,
			 "diff-files", "--name-only", "-z", NULL);
	diff_files.no_stdin = 1;
	diff_files.git_cmd = 1;
	diff_files.use_shell = 0;
	diff_files.clean_on_exit = 1;
	diff_files.out = -1;
	diff_files.dir = workdir;
	diff_files.env = env;
	if (start_command(&diff_files))
		die("could not obtain raw diff");
	fp = xfdopen(diff_files.out, "r");
	while (!strbuf_getline_nul(&buf, fp)) {
		struct path_entry *entry;
		FLEX_ALLOC_STR(entry, path, buf.buf);
		hashmap_entry_init(entry, strhash(buf.buf));
		hashmap_add(result, entry);
	}
	fclose(fp);
	if (finish_command(&diff_files))
		die("diff-files did not exit properly");
	strbuf_release(&index_env);
	strbuf_release(&buf);
}

static NORETURN void exit_cleanup(const char *tmpdir, int exit_code)
{
	struct strbuf buf = STRBUF_INIT;
	strbuf_addstr(&buf, tmpdir);
	remove_dir_recursively(&buf, 0);
	if (exit_code)
		warning(_("failed: %d"), exit_code);
	exit(exit_code);
}

static int ensure_leading_directories(char *path)
{
	switch (safe_create_leading_directories(path)) {
		case SCLD_OK:
		case SCLD_EXISTS:
			return 0;
		default:
			return error(_("could not create leading directories "
				       "of '%s'"), path);
	}
}

/*
 * Unconditional writing of a plain regular file is what
 * "git difftool --dir-diff" wants to do for symlinks.  We are preparing two
 * temporary directories to be fed to a Git-unaware tool that knows how to
 * show a diff of two directories (e.g. "diff -r A B").
 *
 * Because the tool is Git-unaware, if a symbolic link appears in either of
 * these temporary directories, it will try to dereference and show the
 * difference of the target of the symbolic link, which is not what we want,
 * as the goal of the dir-diff mode is to produce an output that is logically
 * equivalent to what "git diff" produces.
 *
 * Most importantly, we want to get textual comparison of the result of the
 * readlink(2).  get_symlink() provides that---it returns the contents of
 * the symlink that gets written to a regular file to force the external tool
 * to compare the readlink(2) result as text, even on a filesystem that is
 * capable of doing a symbolic link.
 */
static char *get_symlink(const struct object_id *oid, const char *path)
{
	char *data;
	if (is_null_oid(oid)) {
		/* The symlink is unknown to Git so read from the filesystem */
		struct strbuf link = STRBUF_INIT;
		if (has_symlinks) {
			if (strbuf_readlink(&link, path, strlen(path)))
				die(_("could not read symlink %s"), path);
		} else if (strbuf_read_file(&link, path, 128))
			die(_("could not read symlink file %s"), path);

		data = strbuf_detach(&link, NULL);
	} else {
		enum object_type type;
		unsigned long size;
		data = read_object_file(oid, &type, &size);
		if (!data)
			die(_("could not read object %s for symlink %s"),
				oid_to_hex(oid), path);
	}

	return data;
}

static int checkout_path(unsigned mode, struct object_id *oid,
			 const char *path, const struct checkout *state)
{
	struct cache_entry *ce;
	int ret;

	ce = make_transient_cache_entry(mode, oid, path, 0);
	ret = checkout_entry(ce, state, NULL, NULL);

	discard_cache_entry(ce);
	return ret;
}

static int run_dir_diff(const char *extcmd, int symlinks, const char *prefix,
			int argc, const char **argv)
{
	char tmpdir[PATH_MAX];
	struct strbuf info = STRBUF_INIT, lpath = STRBUF_INIT;
	struct strbuf rpath = STRBUF_INIT, buf = STRBUF_INIT;
	struct strbuf ldir = STRBUF_INIT, rdir = STRBUF_INIT;
	struct strbuf wtdir = STRBUF_INIT;
	char *lbase_dir, *rbase_dir;
	size_t ldir_len, rdir_len, wtdir_len;
	const char *workdir, *tmp;
	int ret = 0, i;
	FILE *fp;
	struct hashmap working_tree_dups, submodules, symlinks2;
	struct hashmap_iter iter;
	struct pair_entry *entry;
	struct index_state wtindex;
	struct checkout lstate, rstate;
	int rc, flags = RUN_GIT_CMD, err = 0;
	struct child_process child = CHILD_PROCESS_INIT;
	const char *helper_argv[] = { "difftool--helper", NULL, NULL, NULL };
	struct hashmap wt_modified, tmp_modified;
	int indices_loaded = 0;

	workdir = get_git_work_tree();

	/* Setup temp directories */
	tmp = getenv("TMPDIR");
	xsnprintf(tmpdir, sizeof(tmpdir), "%s/git-difftool.XXXXXX", tmp ? tmp : "/tmp");
	if (!mkdtemp(tmpdir))
		return error("could not create '%s'", tmpdir);
	strbuf_addf(&ldir, "%s/left/", tmpdir);
	strbuf_addf(&rdir, "%s/right/", tmpdir);
	strbuf_addstr(&wtdir, workdir);
	if (!wtdir.len || !is_dir_sep(wtdir.buf[wtdir.len - 1]))
		strbuf_addch(&wtdir, '/');
	mkdir(ldir.buf, 0700);
	mkdir(rdir.buf, 0700);

	memset(&wtindex, 0, sizeof(wtindex));

	memset(&lstate, 0, sizeof(lstate));
	lstate.base_dir = lbase_dir = xstrdup(ldir.buf);
	lstate.base_dir_len = ldir.len;
	lstate.force = 1;
	memset(&rstate, 0, sizeof(rstate));
	rstate.base_dir = rbase_dir = xstrdup(rdir.buf);
	rstate.base_dir_len = rdir.len;
	rstate.force = 1;

	ldir_len = ldir.len;
	rdir_len = rdir.len;
	wtdir_len = wtdir.len;

	hashmap_init(&working_tree_dups, working_tree_entry_cmp, NULL, 0);
	hashmap_init(&submodules, pair_cmp, NULL, 0);
	hashmap_init(&symlinks2, pair_cmp, NULL, 0);

	child.no_stdin = 1;
	child.git_cmd = 1;
	child.use_shell = 0;
	child.clean_on_exit = 1;
	child.dir = prefix;
	child.out = -1;
	argv_array_pushl(&child.args, "diff", "--raw", "--no-abbrev", "-z",
			 NULL);
	for (i = 0; i < argc; i++)
		argv_array_push(&child.args, argv[i]);
	if (start_command(&child))
		die("could not obtain raw diff");
	fp = xfdopen(child.out, "r");

	/* Build index info for left and right sides of the diff */
	i = 0;
	while (!strbuf_getline_nul(&info, fp)) {
		int lmode, rmode;
		struct object_id loid, roid;
		char status;
		const char *src_path, *dst_path;

		if (starts_with(info.buf, "::"))
			die(N_("combined diff formats('-c' and '--cc') are "
			       "not supported in\n"
			       "directory diff mode('-d' and '--dir-diff')."));

		if (parse_index_info(info.buf, &lmode, &rmode, &loid, &roid,
				     &status))
			break;
		if (strbuf_getline_nul(&lpath, fp))
			break;
		src_path = lpath.buf;

		i++;
		if (status != 'C' && status != 'R') {
			dst_path = src_path;
		} else {
			if (strbuf_getline_nul(&rpath, fp))
				break;
			dst_path = rpath.buf;
		}

		if (S_ISGITLINK(lmode) || S_ISGITLINK(rmode)) {
			strbuf_reset(&buf);
			strbuf_addf(&buf, "Subproject commit %s",
				    oid_to_hex(&loid));
			add_left_or_right(&submodules, src_path, buf.buf, 0);
			strbuf_reset(&buf);
			strbuf_addf(&buf, "Subproject commit %s",
				    oid_to_hex(&roid));
			if (oideq(&loid, &roid))
				strbuf_addstr(&buf, "-dirty");
			add_left_or_right(&submodules, dst_path, buf.buf, 1);
			continue;
		}

		if (S_ISLNK(lmode)) {
			char *content = get_symlink(&loid, src_path);
			add_left_or_right(&symlinks2, src_path, content, 0);
			free(content);
		}

		if (S_ISLNK(rmode)) {
			char *content = get_symlink(&roid, dst_path);
			add_left_or_right(&symlinks2, dst_path, content, 1);
			free(content);
		}

		if (lmode && status != 'C') {
			if (checkout_path(lmode, &loid, src_path, &lstate)) {
				ret = error("could not write '%s'", src_path);
				goto finish;
			}
		}

		if (rmode && !S_ISLNK(rmode)) {
			struct working_tree_entry *entry;

			/* Avoid duplicate working_tree entries */
			FLEX_ALLOC_STR(entry, path, dst_path);
			hashmap_entry_init(entry, strhash(dst_path));
			if (hashmap_get(&working_tree_dups, entry, NULL)) {
				free(entry);
				continue;
			}
			hashmap_add(&working_tree_dups, entry);

			if (!use_wt_file(workdir, dst_path, &roid)) {
				if (checkout_path(rmode, &roid, dst_path,
						  &rstate)) {
					ret = error("could not write '%s'",
						    dst_path);
					goto finish;
				}
			} else if (!is_null_oid(&roid)) {
				/*
				 * Changes in the working tree need special
				 * treatment since they are not part of the
				 * index.
				 */
				struct cache_entry *ce2 =
					make_cache_entry(&wtindex, rmode, &roid,
							 dst_path, 0, 0);

				add_index_entry(&wtindex, ce2,
						ADD_CACHE_JUST_APPEND);

				add_path(&rdir, rdir_len, dst_path);
				if (ensure_leading_directories(rdir.buf)) {
					ret = error("could not create "
						    "directory for '%s'",
						    dst_path);
					goto finish;
				}
				add_path(&wtdir, wtdir_len, dst_path);
				if (symlinks) {
					if (symlink(wtdir.buf, rdir.buf)) {
						ret = error_errno("could not symlink '%s' to '%s'", wtdir.buf, rdir.buf);
						goto finish;
					}
				} else {
					struct stat st;
					if (stat(wtdir.buf, &st))
						st.st_mode = 0644;
					if (copy_file(rdir.buf, wtdir.buf,
						      st.st_mode)) {
						ret = error("could not copy '%s' to '%s'", wtdir.buf, rdir.buf);
						goto finish;
					}
				}
			}
		}
	}

	fclose(fp);
	fp = NULL;
	if (finish_command(&child)) {
		ret = error("error occurred running diff --raw");
		goto finish;
	}

	if (!i)
		goto finish;

	/*
	 * Changes to submodules require special treatment.This loop writes a
	 * temporary file to both the left and right directories to show the
	 * change in the recorded SHA1 for the submodule.
	 */
	hashmap_iter_init(&submodules, &iter);
	while ((entry = hashmap_iter_next(&iter))) {
		if (*entry->left) {
			add_path(&ldir, ldir_len, entry->path);
			ensure_leading_directories(ldir.buf);
			write_file(ldir.buf, "%s", entry->left);
		}
		if (*entry->right) {
			add_path(&rdir, rdir_len, entry->path);
			ensure_leading_directories(rdir.buf);
			write_file(rdir.buf, "%s", entry->right);
		}
	}

	/*
	 * Symbolic links require special treatment.The standard "git diff"
	 * shows only the link itself, not the contents of the link target.
	 * This loop replicates that behavior.
	 */
	hashmap_iter_init(&symlinks2, &iter);
	while ((entry = hashmap_iter_next(&iter))) {
		if (*entry->left) {
			add_path(&ldir, ldir_len, entry->path);
			ensure_leading_directories(ldir.buf);
			write_file(ldir.buf, "%s", entry->left);
		}
		if (*entry->right) {
			add_path(&rdir, rdir_len, entry->path);
			ensure_leading_directories(rdir.buf);
			write_file(rdir.buf, "%s", entry->right);
		}
	}

	strbuf_release(&buf);

	strbuf_setlen(&ldir, ldir_len);
	helper_argv[1] = ldir.buf;
	strbuf_setlen(&rdir, rdir_len);
	helper_argv[2] = rdir.buf;

	if (extcmd) {
		helper_argv[0] = extcmd;
		flags = 0;
	} else
		setenv("GIT_DIFFTOOL_DIRDIFF", "true", 1);
	rc = run_command_v_opt(helper_argv, flags);

	/*
	 * If the diff includes working copy files and those
	 * files were modified during the diff, then the changes
	 * should be copied back to the working tree.
	 * Do not copy back files when symlinks are used and the
	 * external tool did not replace the original link with a file.
	 *
	 * These hashes are loaded lazily since they aren't needed
	 * in the common case of --symlinks and the difftool updating
	 * files through the symlink.
	 */
	hashmap_init(&wt_modified, path_entry_cmp, NULL, wtindex.cache_nr);
	hashmap_init(&tmp_modified, path_entry_cmp, NULL, wtindex.cache_nr);

	for (i = 0; i < wtindex.cache_nr; i++) {
		struct hashmap_entry dummy;
		const char *name = wtindex.cache[i]->name;
		struct stat st;

		add_path(&rdir, rdir_len, name);
		if (lstat(rdir.buf, &st))
			continue;

		if ((symlinks && S_ISLNK(st.st_mode)) || !S_ISREG(st.st_mode))
			continue;

		if (!indices_loaded) {
			struct lock_file lock = LOCK_INIT;
			strbuf_reset(&buf);
			strbuf_addf(&buf, "%s/wtindex", tmpdir);
			if (hold_lock_file_for_update(&lock, buf.buf, 0) < 0 ||
			    write_locked_index(&wtindex, &lock, COMMIT_LOCK)) {
				ret = error("could not write %s", buf.buf);
				goto finish;
			}
			changed_files(&wt_modified, buf.buf, workdir);
			strbuf_setlen(&rdir, rdir_len);
			changed_files(&tmp_modified, buf.buf, rdir.buf);
			add_path(&rdir, rdir_len, name);
			indices_loaded = 1;
		}

		hashmap_entry_init(&dummy, strhash(name));
		if (hashmap_get(&tmp_modified, &dummy, name)) {
			add_path(&wtdir, wtdir_len, name);
			if (hashmap_get(&wt_modified, &dummy, name)) {
				warning(_("both files modified: '%s' and '%s'."),
					wtdir.buf, rdir.buf);
				warning(_("working tree file has been left."));
				warning("%s", "");
				err = 1;
			} else if (unlink(wtdir.buf) ||
				   copy_file(wtdir.buf, rdir.buf, st.st_mode))
				warning_errno(_("could not copy '%s' to '%s'"),
					      rdir.buf, wtdir.buf);
		}
	}

	if (err) {
		warning(_("temporary files exist in '%s'."), tmpdir);
		warning(_("you may want to cleanup or recover these."));
		exit(1);
	} else
		exit_cleanup(tmpdir, rc);

finish:
	if (fp)
		fclose(fp);

	free(lbase_dir);
	free(rbase_dir);
	strbuf_release(&ldir);
	strbuf_release(&rdir);
	strbuf_release(&wtdir);
	strbuf_release(&buf);

	return ret;
}

static int run_file_diff(int prompt, const char *prefix,
			 int argc, const char **argv)
{
	struct argv_array args = ARGV_ARRAY_INIT;
	const char *env[] = {
		"GIT_PAGER=", "GIT_EXTERNAL_DIFF=git-difftool--helper", NULL,
		NULL
	};
	int ret = 0, i;

	if (prompt > 0)
		env[2] = "GIT_DIFFTOOL_PROMPT=true";
	else if (!prompt)
		env[2] = "GIT_DIFFTOOL_NO_PROMPT=true";


	argv_array_push(&args, "diff");
	for (i = 0; i < argc; i++)
		argv_array_push(&args, argv[i]);
	ret = run_command_v_opt_cd_env(args.argv, RUN_GIT_CMD, prefix, env);
	exit(ret);
}

int cmd_difftool(int argc, const char **argv, const char *prefix)
{
	int use_gui_tool = 0, dir_diff = 0, prompt = -1, symlinks = 0,
	    tool_help = 0, no_index = 0;
	static char *difftool_cmd = NULL, *extcmd = NULL;
	struct option builtin_difftool_options[] = {
		OPT_BOOL('g', "gui", &use_gui_tool,
			 N_("use `diff.guitool` instead of `diff.tool`")),
		OPT_BOOL('d', "dir-diff", &dir_diff,
			 N_("perform a full-directory diff")),
		OPT_SET_INT_F('y', "no-prompt", &prompt,
			N_("do not prompt before launching a diff tool"),
			0, PARSE_OPT_NONEG),
		OPT_SET_INT_F(0, "prompt", &prompt, NULL,
			1, PARSE_OPT_NONEG | PARSE_OPT_HIDDEN),
		OPT_BOOL(0, "symlinks", &symlinks,
			 N_("use symlinks in dir-diff mode")),
		OPT_STRING('t', "tool", &difftool_cmd, N_("tool"),
			   N_("use the specified diff tool")),
		OPT_BOOL(0, "tool-help", &tool_help,
			 N_("print a list of diff tools that may be used with "
			    "`--tool`")),
		OPT_BOOL(0, "trust-exit-code", &trust_exit_code,
			 N_("make 'git-difftool' exit when an invoked diff "
			    "tool returns a non - zero exit code")),
		OPT_STRING('x', "extcmd", &extcmd, N_("command"),
			   N_("specify a custom command for viewing diffs")),
		OPT_ARGUMENT("no-index", &no_index, N_("passed to `diff`")),
		OPT_END()
	};

	git_config(difftool_config, NULL);
	symlinks = has_symlinks;

	argc = parse_options(argc, argv, prefix, builtin_difftool_options,
			     builtin_difftool_usage, PARSE_OPT_KEEP_UNKNOWN |
			     PARSE_OPT_KEEP_DASHDASH);

	if (tool_help)
		return print_tool_help();

	if (!no_index && !startup_info->have_repository)
		die(_("difftool requires worktree or --no-index"));

	if (!no_index){
		setup_work_tree();
		setenv(GIT_DIR_ENVIRONMENT, absolute_path(get_git_dir()), 1);
		setenv(GIT_WORK_TREE_ENVIRONMENT, absolute_path(get_git_work_tree()), 1);
	} else if (dir_diff)
		die(_("--dir-diff is incompatible with --no-index"));

	if (use_gui_tool + !!difftool_cmd + !!extcmd > 1)
		die(_("--gui, --tool and --extcmd are mutually exclusive"));

	if (use_gui_tool)
		setenv("GIT_MERGETOOL_GUI", "true", 1);
	else if (difftool_cmd) {
		if (*difftool_cmd)
			setenv("GIT_DIFF_TOOL", difftool_cmd, 1);
		else
			die(_("no <tool> given for --tool=<tool>"));
	}

	if (extcmd) {
		if (*extcmd)
			setenv("GIT_DIFFTOOL_EXTCMD", extcmd, 1);
		else
			die(_("no <cmd> given for --extcmd=<cmd>"));
	}

	setenv("GIT_DIFFTOOL_TRUST_EXIT_CODE",
	       trust_exit_code ? "true" : "false", 1);

	/*
	 * In directory diff mode, 'git-difftool--helper' is called once
	 * to compare the a / b directories. In file diff mode, 'git diff'
	 * will invoke a separate instance of 'git-difftool--helper' for
	 * each file that changed.
	 */
	if (dir_diff)
		return run_dir_diff(extcmd, symlinks, prefix, argc, argv);
	return run_file_diff(prompt, prefix, argc, argv);
}
