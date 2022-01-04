/*
 * GIT - The information manager from hell
 *
 * Copyright (C) Linus Torvalds, 2005
 *
 * This handles basic git sha1 object files - packing, unpacking,
 * creation etc.
 */
#include "cache.h"
#include "config.h"
#include "string-list.h"
#include "lockfile.h"
#include "delta.h"
#include "pack.h"
#include "blob.h"
#include "commit.h"
#include "run-command.h"
#include "tag.h"
#include "tree.h"
#include "tree-walk.h"
#include "refs.h"
#include "pack-revindex.h"
#include "sha1-lookup.h"
#include "bulk-checkin.h"
#include "repository.h"
#include "replace-object.h"
#include "streaming.h"
#include "dir.h"
#include "list.h"
#include "mergesort.h"
#include "quote.h"
#include "packfile.h"
#include "fetch-object.h"
#include "object-store.h"

/* The maximum size for an object header. */
#define MAX_HEADER_LEN 32


#define EMPTY_TREE_SHA1_BIN_LITERAL \
	 "\x4b\x82\x5d\xc6\x42\xcb\x6e\xb9\xa0\x60" \
	 "\xe5\x4b\xf8\xd6\x92\x88\xfb\xee\x49\x04"
#define EMPTY_TREE_SHA256_BIN_LITERAL \
	"\x6e\xf1\x9b\x41\x22\x5c\x53\x69\xf1\xc1" \
	"\x04\xd4\x5d\x8d\x85\xef\xa9\xb0\x57\xb5" \
	"\x3b\x14\xb4\xb9\xb9\x39\xdd\x74\xde\xcc" \
	"\x53\x21"

#define EMPTY_BLOB_SHA1_BIN_LITERAL \
	"\xe6\x9d\xe2\x9b\xb2\xd1\xd6\x43\x4b\x8b" \
	"\x29\xae\x77\x5a\xd8\xc2\xe4\x8c\x53\x91"
#define EMPTY_BLOB_SHA256_BIN_LITERAL \
	"\x47\x3a\x0f\x4c\x3b\xe8\xa9\x36\x81\xa2" \
	"\x67\xe3\xb1\xe9\xa7\xdc\xda\x11\x85\x43" \
	"\x6f\xe1\x41\xf7\x74\x91\x20\xa3\x03\x72" \
	"\x18\x13"

const unsigned char null_sha1[GIT_MAX_RAWSZ];
const struct object_id null_oid;
static const struct object_id empty_tree_oid = {
	EMPTY_TREE_SHA1_BIN_LITERAL
};
static const struct object_id empty_blob_oid = {
	EMPTY_BLOB_SHA1_BIN_LITERAL
};
static const struct object_id empty_tree_oid_sha256 = {
	EMPTY_TREE_SHA256_BIN_LITERAL
};
static const struct object_id empty_blob_oid_sha256 = {
	EMPTY_BLOB_SHA256_BIN_LITERAL
};

static void git_hash_sha1_init(git_hash_ctx *ctx)
{
	git_SHA1_Init(&ctx->sha1);
}

static void git_hash_sha1_update(git_hash_ctx *ctx, const void *data, size_t len)
{
	git_SHA1_Update(&ctx->sha1, data, len);
}

static void git_hash_sha1_final(unsigned char *hash, git_hash_ctx *ctx)
{
	git_SHA1_Final(hash, &ctx->sha1);
}


static void git_hash_sha256_init(git_hash_ctx *ctx)
{
	git_SHA256_Init(&ctx->sha256);
}

static void git_hash_sha256_update(git_hash_ctx *ctx, const void *data, size_t len)
{
	git_SHA256_Update(&ctx->sha256, data, len);
}

static void git_hash_sha256_final(unsigned char *hash, git_hash_ctx *ctx)
{
	git_SHA256_Final(hash, &ctx->sha256);
}

static void git_hash_unknown_init(git_hash_ctx *ctx)
{
	BUG("trying to init unknown hash");
}

static void git_hash_unknown_update(git_hash_ctx *ctx, const void *data, size_t len)
{
	BUG("trying to update unknown hash");
}

static void git_hash_unknown_final(unsigned char *hash, git_hash_ctx *ctx)
{
	BUG("trying to finalize unknown hash");
}

const struct git_hash_algo hash_algos[GIT_HASH_NALGOS] = {
	{
		NULL,
		0x00000000,
		0,
		0,
		0,
		git_hash_unknown_init,
		git_hash_unknown_update,
		git_hash_unknown_final,
		NULL,
		NULL,
	},
	{
		"sha1",
		/* "sha1", big-endian */
		0x73686131,
		GIT_SHA1_RAWSZ,
		GIT_SHA1_HEXSZ,
		GIT_SHA1_BLKSZ,
		git_hash_sha1_init,
		git_hash_sha1_update,
		git_hash_sha1_final,
		&empty_tree_oid,
		&empty_blob_oid,
	},
	{
		"sha256",
		/* "s256", big-endian */
		0x73323536,
		GIT_SHA256_RAWSZ,
		GIT_SHA256_HEXSZ,
		GIT_SHA256_BLKSZ,
		git_hash_sha256_init,
		git_hash_sha256_update,
		git_hash_sha256_final,
		&empty_tree_oid_sha256,
		&empty_blob_oid_sha256,
	}
};

const char *empty_tree_oid_hex(void)
{
	static char buf[GIT_MAX_HEXSZ + 1];
	return oid_to_hex_r(buf, the_hash_algo->empty_tree);
}

const char *empty_blob_oid_hex(void)
{
	static char buf[GIT_MAX_HEXSZ + 1];
	return oid_to_hex_r(buf, the_hash_algo->empty_blob);
}

int hash_algo_by_name(const char *name)
{
	int i;
	if (!name)
		return GIT_HASH_UNKNOWN;
	for (i = 1; i < GIT_HASH_NALGOS; i++)
		if (!strcmp(name, hash_algos[i].name))
			return i;
	return GIT_HASH_UNKNOWN;
}

int hash_algo_by_id(uint32_t format_id)
{
	int i;
	for (i = 1; i < GIT_HASH_NALGOS; i++)
		if (format_id == hash_algos[i].format_id)
			return i;
	return GIT_HASH_UNKNOWN;
}

int hash_algo_by_length(int len)
{
	int i;
	for (i = 1; i < GIT_HASH_NALGOS; i++)
		if (len == hash_algos[i].rawsz)
			return i;
	return GIT_HASH_UNKNOWN;
}

/*
 * This is meant to hold a *small* number of objects that you would
 * want read_object_file() to be able to return, but yet you do not want
 * to write them into the object store (e.g. a browse-only
 * application).
 */
static struct cached_object {
	struct object_id oid;
	enum object_type type;
	void *buf;
	unsigned long size;
} *cached_objects;
static int cached_object_nr, cached_object_alloc;

static struct cached_object empty_tree = {
	{ EMPTY_TREE_SHA1_BIN_LITERAL },
	OBJ_TREE,
	"",
	0
};

static struct cached_object *find_cached_object(const struct object_id *oid)
{
	int i;
	struct cached_object *co = cached_objects;

	for (i = 0; i < cached_object_nr; i++, co++) {
		if (oideq(&co->oid, oid))
			return co;
	}
	if (oideq(oid, the_hash_algo->empty_tree))
		return &empty_tree;
	return NULL;
}


static int get_conv_flags(unsigned flags)
{
	if (flags & HASH_RENORMALIZE)
		return CONV_EOL_RENORMALIZE;
	else if (flags & HASH_WRITE_OBJECT)
		return global_conv_flags_eol | CONV_WRITE_OBJECT;
	else
		return 0;
}


int mkdir_in_gitdir(const char *path)
{
	if (mkdir(path, 0777)) {
		int saved_errno = errno;
		struct stat st;
		struct strbuf sb = STRBUF_INIT;

		if (errno != EEXIST)
			return -1;
		/*
		 * Are we looking at a path in a symlinked worktree
		 * whose original repository does not yet have it?
		 * e.g. .git/rr-cache pointing at its original
		 * repository in which the user hasn't performed any
		 * conflict resolution yet?
		 */
		if (lstat(path, &st) || !S_ISLNK(st.st_mode) ||
		    strbuf_readlink(&sb, path, st.st_size) ||
		    !is_absolute_path(sb.buf) ||
		    mkdir(sb.buf, 0777)) {
			strbuf_release(&sb);
			errno = saved_errno;
			return -1;
		}
		strbuf_release(&sb);
	}
	return adjust_shared_perm(path);
}

enum scld_error safe_create_leading_directories(char *path)
{
	char *next_component = path + offset_1st_component(path);
	enum scld_error ret = SCLD_OK;

	while (ret == SCLD_OK && next_component) {
		struct stat st;
		char *slash = next_component, slash_character;

		while (*slash && !is_dir_sep(*slash))
			slash++;

		if (!*slash)
			break;

		next_component = slash + 1;
		while (is_dir_sep(*next_component))
			next_component++;
		if (!*next_component)
			break;

		slash_character = *slash;
		*slash = '\0';
		if (!stat(path, &st)) {
			/* path exists */
			if (!S_ISDIR(st.st_mode)) {
				errno = ENOTDIR;
				ret = SCLD_EXISTS;
			}
		} else if (mkdir(path, 0777)) {
			if (errno == EEXIST &&
			    !stat(path, &st) && S_ISDIR(st.st_mode))
				; /* somebody created it since we checked */
			else if (errno == ENOENT)
				/*
				 * Either mkdir() failed because
				 * somebody just pruned the containing
				 * directory, or stat() failed because
				 * the file that was in our way was
				 * just removed.  Either way, inform
				 * the caller that it might be worth
				 * trying again:
				 */
				ret = SCLD_VANISHED;
			else
				ret = SCLD_FAILED;
		} else if (adjust_shared_perm(path)) {
			ret = SCLD_PERMS;
		}
		*slash = slash_character;
	}
	return ret;
}

enum scld_error safe_create_leading_directories_const(const char *path)
{
	int save_errno;
	/* path points to cache entries, so xstrdup before messing with it */
	char *buf = xstrdup(path);
	enum scld_error result = safe_create_leading_directories(buf);

	save_errno = errno;
	free(buf);
	errno = save_errno;
	return result;
}

int raceproof_create_file(const char *path, create_file_fn fn, void *cb)
{
	/*
	 * The number of times we will try to remove empty directories
	 * in the way of path. This is only 1 because if another
	 * process is racily creating directories that conflict with
	 * us, we don't want to fight against them.
	 */
	int remove_directories_remaining = 1;

	/*
	 * The number of times that we will try to create the
	 * directories containing path. We are willing to attempt this
	 * more than once, because another process could be trying to
	 * clean up empty directories at the same time as we are
	 * trying to create them.
	 */
	int create_directories_remaining = 3;

	/* A scratch copy of path, filled lazily if we need it: */
	struct strbuf path_copy = STRBUF_INIT;

	int ret, save_errno;

	/* Sanity check: */
	assert(*path);

retry_fn:
	ret = fn(path, cb);
	save_errno = errno;
	if (!ret)
		goto out;

	if (errno == EISDIR && remove_directories_remaining-- > 0) {
		/*
		 * A directory is in the way. Maybe it is empty; try
		 * to remove it:
		 */
		if (!path_copy.len)
			strbuf_addstr(&path_copy, path);

		if (!remove_dir_recursively(&path_copy, REMOVE_DIR_EMPTY_ONLY))
			goto retry_fn;
	} else if (errno == ENOENT && create_directories_remaining-- > 0) {
		/*
		 * Maybe the containing directory didn't exist, or
		 * maybe it was just deleted by a process that is
		 * racing with us to clean up empty directories. Try
		 * to create it:
		 */
		enum scld_error scld_result;

		if (!path_copy.len)
			strbuf_addstr(&path_copy, path);

		do {
			scld_result = safe_create_leading_directories(path_copy.buf);
			if (scld_result == SCLD_OK)
				goto retry_fn;
		} while (scld_result == SCLD_VANISHED && create_directories_remaining-- > 0);
	}

out:
	strbuf_release(&path_copy);
	errno = save_errno;
	return ret;
}

static void fill_loose_path(struct strbuf *buf, const struct object_id *oid)
{
	int i;
	for (i = 0; i < the_hash_algo->rawsz; i++) {
		static char hex[] = "0123456789abcdef";
		unsigned int val = oid->hash[i];
		strbuf_addch(buf, hex[val >> 4]);
		strbuf_addch(buf, hex[val & 0xf]);
		if (!i)
			strbuf_addch(buf, '/');
	}
}

static const char *odb_loose_path(struct object_directory *odb,
				  struct strbuf *buf,
				  const struct object_id *oid)
{
	strbuf_reset(buf);
	strbuf_addstr(buf, odb->path);
	strbuf_addch(buf, '/');
	fill_loose_path(buf, oid);
	return buf->buf;
}

const char *loose_object_path(struct repository *r, struct strbuf *buf,
			      const struct object_id *oid)
{
	return odb_loose_path(r->objects->odb, buf, oid);
}

/*
 * Return non-zero iff the path is usable as an alternate object database.
 */
static int alt_odb_usable(struct raw_object_store *o,
			  struct strbuf *path,
			  const char *normalized_objdir)
{
	struct object_directory *odb;

	/* Detect cases where alternate disappeared */
	if (!is_directory(path->buf)) {
		error(_("object directory %s does not exist; "
			"check .git/objects/info/alternates"),
		      path->buf);
		return 0;
	}

	/*
	 * Prevent the common mistake of listing the same
	 * thing twice, or object directory itself.
	 */
	for (odb = o->odb; odb; odb = odb->next) {
		if (!fspathcmp(path->buf, odb->path))
			return 0;
	}
	if (!fspathcmp(path->buf, normalized_objdir))
		return 0;

	return 1;
}

/*
 * Prepare alternate object database registry.
 *
 * The variable alt_odb_list points at the list of struct
 * object_directory.  The elements on this list come from
 * non-empty elements from colon separated ALTERNATE_DB_ENVIRONMENT
 * environment variable, and $GIT_OBJECT_DIRECTORY/info/alternates,
 * whose contents is similar to that environment variable but can be
 * LF separated.  Its base points at a statically allocated buffer that
 * contains "/the/directory/corresponding/to/.git/objects/...", while
 * its name points just after the slash at the end of ".git/objects/"
 * in the example above, and has enough space to hold 40-byte hex
 * SHA1, an extra slash for the first level indirection, and the
 * terminating NUL.
 */
static void read_info_alternates(struct repository *r,
				 const char *relative_base,
				 int depth);
static int link_alt_odb_entry(struct repository *r, const char *entry,
	const char *relative_base, int depth, const char *normalized_objdir)
{
	struct object_directory *ent;
	struct strbuf pathbuf = STRBUF_INIT;

	if (!is_absolute_path(entry) && relative_base) {
		strbuf_realpath(&pathbuf, relative_base, 1);
		strbuf_addch(&pathbuf, '/');
	}
	strbuf_addstr(&pathbuf, entry);

	if (strbuf_normalize_path(&pathbuf) < 0 && relative_base) {
		error(_("unable to normalize alternate object path: %s"),
		      pathbuf.buf);
		strbuf_release(&pathbuf);
		return -1;
	}

	/*
	 * The trailing slash after the directory name is given by
	 * this function at the end. Remove duplicates.
	 */
	while (pathbuf.len && pathbuf.buf[pathbuf.len - 1] == '/')
		strbuf_setlen(&pathbuf, pathbuf.len - 1);

	if (!alt_odb_usable(r->objects, &pathbuf, normalized_objdir)) {
		strbuf_release(&pathbuf);
		return -1;
	}

	ent = xcalloc(1, sizeof(*ent));
	ent->path = xstrdup(pathbuf.buf);

	/* add the alternate entry */
	*r->objects->odb_tail = ent;
	r->objects->odb_tail = &(ent->next);
	ent->next = NULL;

	/* recursively add alternates */
	read_info_alternates(r, pathbuf.buf, depth + 1);

	strbuf_release(&pathbuf);
	return 0;
}

static const char *parse_alt_odb_entry(const char *string,
				       int sep,
				       struct strbuf *out)
{
	const char *end;

	strbuf_reset(out);

	if (*string == '#') {
		/* comment; consume up to next separator */
		end = strchrnul(string, sep);
	} else if (*string == '"' && !unquote_c_style(out, string, &end)) {
		/*
		 * quoted path; unquote_c_style has copied the
		 * data for us and set "end". Broken quoting (e.g.,
		 * an entry that doesn't end with a quote) falls
		 * back to the unquoted case below.
		 */
	} else {
		/* normal, unquoted path */
		end = strchrnul(string, sep);
		strbuf_add(out, string, end - string);
	}

	if (*end)
		end++;
	return end;
}

static void link_alt_odb_entries(struct repository *r, const char *alt,
				 int sep, const char *relative_base, int depth)
{
	struct strbuf objdirbuf = STRBUF_INIT;
	struct strbuf entry = STRBUF_INIT;

	if (!alt || !*alt)
		return;

	if (depth > 5) {
		error(_("%s: ignoring alternate object stores, nesting too deep"),
				relative_base);
		return;
	}

	strbuf_add_absolute_path(&objdirbuf, r->objects->odb->path);
	if (strbuf_normalize_path(&objdirbuf) < 0)
		die(_("unable to normalize object directory: %s"),
		    objdirbuf.buf);

	while (*alt) {
		alt = parse_alt_odb_entry(alt, sep, &entry);
		if (!entry.len)
			continue;
		link_alt_odb_entry(r, entry.buf,
				   relative_base, depth, objdirbuf.buf);
	}
	strbuf_release(&entry);
	strbuf_release(&objdirbuf);
}

static void read_info_alternates(struct repository *r,
				 const char *relative_base,
				 int depth)
{
	char *path;
	struct strbuf buf = STRBUF_INIT;

	path = xstrfmt("%s/info/alternates", relative_base);
	if (strbuf_read_file(&buf, path, 1024) < 0) {
		warn_on_fopen_errors(path);
		free(path);
		return;
	}

	link_alt_odb_entries(r, buf.buf, '\n', relative_base, depth);
	strbuf_release(&buf);
	free(path);
}

void add_to_alternates_file(const char *reference)
{
	struct lock_file lock = LOCK_INIT;
	char *alts = git_pathdup("objects/info/alternates");
	FILE *in, *out;
	int found = 0;

	hold_lock_file_for_update(&lock, alts, LOCK_DIE_ON_ERROR);
	out = fdopen_lock_file(&lock, "w");
	if (!out)
		die_errno(_("unable to fdopen alternates lockfile"));

	in = fopen(alts, "r");
	if (in) {
		struct strbuf line = STRBUF_INIT;

		while (strbuf_getline(&line, in) != EOF) {
			if (!strcmp(reference, line.buf)) {
				found = 1;
				break;
			}
			fprintf_or_die(out, "%s\n", line.buf);
		}

		strbuf_release(&line);
		fclose(in);
	}
	else if (errno != ENOENT)
		die_errno(_("unable to read alternates file"));

	if (found) {
		rollback_lock_file(&lock);
	} else {
		fprintf_or_die(out, "%s\n", reference);
		if (commit_lock_file(&lock))
			die_errno(_("unable to move new alternates file into place"));
		if (the_repository->objects->loaded_alternates)
			link_alt_odb_entries(the_repository, reference,
					     '\n', NULL, 0);
	}
	free(alts);
}

void add_to_alternates_memory(const char *reference)
{
	/*
	 * Make sure alternates are initialized, or else our entry may be
	 * overwritten when they are.
	 */
	prepare_alt_odb(the_repository);

	link_alt_odb_entries(the_repository, reference,
			     '\n', NULL, 0);
}

/*
 * Compute the exact path an alternate is at and returns it. In case of
 * error NULL is returned and the human readable error is added to `err`
 * `path` may be relative and should point to $GIT_DIR.
 * `err` must not be null.
 */
char *compute_alternate_path(const char *path, struct strbuf *err)
{
	char *ref_git = NULL;
	const char *repo, *ref_git_s;
	int seen_error = 0;

	ref_git_s = real_path_if_valid(path);
	if (!ref_git_s) {
		seen_error = 1;
		strbuf_addf(err, _("path '%s' does not exist"), path);
		goto out;
	} else
		/*
		 * Beware: read_gitfile(), real_path() and mkpath()
		 * return static buffer
		 */
		ref_git = xstrdup(ref_git_s);

	repo = read_gitfile(ref_git);
	if (!repo)
		repo = read_gitfile(mkpath("%s/.git", ref_git));
	if (repo) {
		free(ref_git);
		ref_git = xstrdup(repo);
	}

	if (!repo && is_directory(mkpath("%s/.git/objects", ref_git))) {
		char *ref_git_git = mkpathdup("%s/.git", ref_git);
		free(ref_git);
		ref_git = ref_git_git;
	} else if (!is_directory(mkpath("%s/objects", ref_git))) {
		struct strbuf sb = STRBUF_INIT;
		seen_error = 1;
		if (get_common_dir(&sb, ref_git)) {
			strbuf_addf(err,
				    _("reference repository '%s' as a linked "
				      "checkout is not supported yet."),
				    path);
			goto out;
		}

		strbuf_addf(err, _("reference repository '%s' is not a "
					"local repository."), path);
		goto out;
	}

	if (!access(mkpath("%s/shallow", ref_git), F_OK)) {
		strbuf_addf(err, _("reference repository '%s' is shallow"),
			    path);
		seen_error = 1;
		goto out;
	}

	if (!access(mkpath("%s/info/grafts", ref_git), F_OK)) {
		strbuf_addf(err,
			    _("reference repository '%s' is grafted"),
			    path);
		seen_error = 1;
		goto out;
	}

out:
	if (seen_error) {
		FREE_AND_NULL(ref_git);
	}

	return ref_git;
}

static void fill_alternate_refs_command(struct child_process *cmd,
					const char *repo_path)
{
	const char *value;

	if (!git_config_get_value("core.alternateRefsCommand", &value)) {
		cmd->use_shell = 1;

		argv_array_push(&cmd->args, value);
		argv_array_push(&cmd->args, repo_path);
	} else {
		cmd->git_cmd = 1;

		argv_array_pushf(&cmd->args, "--git-dir=%s", repo_path);
		argv_array_push(&cmd->args, "for-each-ref");
		argv_array_push(&cmd->args, "--format=%(objectname)");

		if (!git_config_get_value("core.alternateRefsPrefixes", &value)) {
			argv_array_push(&cmd->args, "--");
			argv_array_split(&cmd->args, value);
		}
	}

	cmd->env = local_repo_env;
	cmd->out = -1;
}

static void read_alternate_refs(const char *path,
				alternate_ref_fn *cb,
				void *data)
{
	struct child_process cmd = CHILD_PROCESS_INIT;
	struct strbuf line = STRBUF_INIT;
	FILE *fh;

	fill_alternate_refs_command(&cmd, path);

	if (start_command(&cmd))
		return;

	fh = xfdopen(cmd.out, "r");
	while (strbuf_getline_lf(&line, fh) != EOF) {
		struct object_id oid;
		const char *p;

		if (parse_oid_hex(line.buf, &oid, &p) || *p) {
			warning(_("invalid line while parsing alternate refs: %s"),
				line.buf);
			break;
		}

		cb(&oid, data);
	}

	fclose(fh);
	finish_command(&cmd);
	strbuf_release(&line);
}

struct alternate_refs_data {
	alternate_ref_fn *fn;
	void *data;
};

static int refs_from_alternate_cb(struct object_directory *e,
				  void *data)
{
	struct strbuf path = STRBUF_INIT;
	size_t base_len;
	struct alternate_refs_data *cb = data;

	if (!strbuf_realpath(&path, e->path, 0))
		goto out;
	if (!strbuf_strip_suffix(&path, "/objects"))
		goto out;
	base_len = path.len;

	/* Is this a git repository with refs? */
	strbuf_addstr(&path, "/refs");
	if (!is_directory(path.buf))
		goto out;
	strbuf_setlen(&path, base_len);

	read_alternate_refs(path.buf, cb->fn, cb->data);

out:
	strbuf_release(&path);
	return 0;
}

void for_each_alternate_ref(alternate_ref_fn fn, void *data)
{
	struct alternate_refs_data cb;
	cb.fn = fn;
	cb.data = data;
	foreach_alt_odb(refs_from_alternate_cb, &cb);
}

int foreach_alt_odb(alt_odb_fn fn, void *cb)
{
	struct object_directory *ent;
	int r = 0;

	prepare_alt_odb(the_repository);
	for (ent = the_repository->objects->odb->next; ent; ent = ent->next) {
		r = fn(ent, cb);
		if (r)
			break;
	}
	return r;
}

void prepare_alt_odb(struct repository *r)
{
	if (r->objects->loaded_alternates)
		return;

	link_alt_odb_entries(r, r->objects->alternate_db, PATH_SEP, NULL, 0);

	read_info_alternates(r, r->objects->odb->path, 0);
	r->objects->loaded_alternates = 1;
}

/* Returns 1 if we have successfully freshened the file, 0 otherwise. */
static int freshen_file(const char *fn)
{
	struct utimbuf t;
	t.actime = t.modtime = time(NULL);
	return !utime(fn, &t);
}

/*
 * All of the check_and_freshen functions return 1 if the file exists and was
 * freshened (if freshening was requested), 0 otherwise. If they return
 * 0, you should not assume that it is safe to skip a write of the object (it
 * either does not exist on disk, or has a stale mtime and may be subject to
 * pruning).
 */
int check_and_freshen_file(const char *fn, int freshen)
{
	if (access(fn, F_OK))
		return 0;
	if (freshen && !freshen_file(fn))
		return 0;
	return 1;
}

static int check_and_freshen_odb(struct object_directory *odb,
				 const struct object_id *oid,
				 int freshen)
{
	static struct strbuf path = STRBUF_INIT;
	odb_loose_path(odb, &path, oid);
	return check_and_freshen_file(path.buf, freshen);
}

static int check_and_freshen_local(const struct object_id *oid, int freshen)
{
	return check_and_freshen_odb(the_repository->objects->odb, oid, freshen);
}

static int check_and_freshen_nonlocal(const struct object_id *oid, int freshen)
{
	struct object_directory *odb;

	prepare_alt_odb(the_repository);
	for (odb = the_repository->objects->odb->next; odb; odb = odb->next) {
		if (check_and_freshen_odb(odb, oid, freshen))
			return 1;
	}
	return 0;
}

static int check_and_freshen(const struct object_id *oid, int freshen)
{
	return check_and_freshen_local(oid, freshen) ||
	       check_and_freshen_nonlocal(oid, freshen);
}

int has_loose_object_nonlocal(const struct object_id *oid)
{
	return check_and_freshen_nonlocal(oid, 0);
}

static int has_loose_object(const struct object_id *oid)
{
	return check_and_freshen(oid, 0);
}

static void mmap_limit_check(size_t length)
{
	static size_t limit = 0;
	if (!limit) {
		limit = git_env_ulong("GIT_MMAP_LIMIT", 0);
		if (!limit)
			limit = SIZE_MAX;
	}
	if (length > limit)
		die(_("attempting to mmap %"PRIuMAX" over limit %"PRIuMAX),
		    (uintmax_t)length, (uintmax_t)limit);
}

void *xmmap_gently(void *start, size_t length,
		  int prot, int flags, int fd, off_t offset)
{
	void *ret;

	mmap_limit_check(length);
	ret = mmap(start, length, prot, flags, fd, offset);
	if (ret == MAP_FAILED) {
		if (!length)
			return NULL;
		release_pack_memory(length);
		ret = mmap(start, length, prot, flags, fd, offset);
	}
	return ret;
}

void *xmmap(void *start, size_t length,
	int prot, int flags, int fd, off_t offset)
{
	void *ret = xmmap_gently(start, length, prot, flags, fd, offset);
	if (ret == MAP_FAILED)
		die_errno(_("mmap failed"));
	return ret;
}

/*
 * With an in-core object data in "map", rehash it to make sure the
 * object name actually matches "oid" to detect object corruption.
 * With "map" == NULL, try reading the object named with "oid" using
 * the streaming interface and rehash it to do the same.
 */
int check_object_signature(const struct object_id *oid, void *map,
			   unsigned long size, const char *type)
{
	struct object_id real_oid;
	enum object_type obj_type;
	struct git_istream *st;
	git_hash_ctx c;
	char hdr[MAX_HEADER_LEN];
	int hdrlen;

	if (map) {
		hash_object_file(map, size, type, &real_oid);
		return !oideq(oid, &real_oid) ? -1 : 0;
	}

	st = open_istream(oid, &obj_type, &size, NULL);
	if (!st)
		return -1;

	/* Generate the header */
	hdrlen = xsnprintf(hdr, sizeof(hdr), "%s %"PRIuMAX , type_name(obj_type), (uintmax_t)size) + 1;

	/* Sha1.. */
	the_hash_algo->init_fn(&c);
	the_hash_algo->update_fn(&c, hdr, hdrlen);
	for (;;) {
		char buf[1024 * 16];
		ssize_t readlen = read_istream(st, buf, sizeof(buf));

		if (readlen < 0) {
			close_istream(st);
			return -1;
		}
		if (!readlen)
			break;
		the_hash_algo->update_fn(&c, buf, readlen);
	}
	the_hash_algo->final_fn(real_oid.hash, &c);
	close_istream(st);
	return !oideq(oid, &real_oid) ? -1 : 0;
}

int git_open_cloexec(const char *name, int flags)
{
	int fd;
	static int o_cloexec = O_CLOEXEC;

	fd = open(name, flags | o_cloexec);
	if ((o_cloexec & O_CLOEXEC) && fd < 0 && errno == EINVAL) {
		/* Try again w/o O_CLOEXEC: the kernel might not support it */
		o_cloexec &= ~O_CLOEXEC;
		fd = open(name, flags | o_cloexec);
	}

#if defined(F_GETFD) && defined(F_SETFD) && defined(FD_CLOEXEC)
	{
		static int fd_cloexec = FD_CLOEXEC;

		if (!o_cloexec && 0 <= fd && fd_cloexec) {
			/* Opened w/o O_CLOEXEC?  try with fcntl(2) to add it */
			int flags = fcntl(fd, F_GETFD);
			if (fcntl(fd, F_SETFD, flags | fd_cloexec))
				fd_cloexec = 0;
		}
	}
#endif
	return fd;
}

/*
 * Find "oid" as a loose object in the local repository or in an alternate.
 * Returns 0 on success, negative on failure.
 *
 * The "path" out-parameter will give the path of the object we found (if any).
 * Note that it may point to static storage and is only valid until another
 * call to stat_loose_object().
 */
static int stat_loose_object(struct repository *r, const struct object_id *oid,
			     struct stat *st, const char **path)
{
	struct object_directory *odb;
	static struct strbuf buf = STRBUF_INIT;

	prepare_alt_odb(r);
	for (odb = r->objects->odb; odb; odb = odb->next) {
		*path = odb_loose_path(odb, &buf, oid);
		if (!lstat(*path, st))
			return 0;
	}

	return -1;
}

/*
 * Like stat_loose_object(), but actually open the object and return the
 * descriptor. See the caveats on the "path" parameter above.
 */
static int open_loose_object(struct repository *r,
			     const struct object_id *oid, const char **path)
{
	int fd;
	struct object_directory *odb;
	int most_interesting_errno = ENOENT;
	static struct strbuf buf = STRBUF_INIT;

	prepare_alt_odb(r);
	for (odb = r->objects->odb; odb; odb = odb->next) {
		*path = odb_loose_path(odb, &buf, oid);
		fd = git_open(*path);
		if (fd >= 0)
			return fd;

		if (most_interesting_errno == ENOENT)
			most_interesting_errno = errno;
	}
	errno = most_interesting_errno;
	return -1;
}

static int quick_has_loose(struct repository *r,
			   const struct object_id *oid)
{
	struct object_directory *odb;

	prepare_alt_odb(r);
	for (odb = r->objects->odb; odb; odb = odb->next) {
		if (oid_array_lookup(odb_loose_cache(odb, oid), oid) >= 0)
			return 1;
	}
	return 0;
}

/*
 * Map the loose object at "path" if it is not NULL, or the path found by
 * searching for a loose object named "oid".
 */
static void *map_loose_object_1(struct repository *r, const char *path,
			     const struct object_id *oid, unsigned long *size)
{
	void *map;
	int fd;

	if (path)
		fd = git_open(path);
	else
		fd = open_loose_object(r, oid, &path);
	map = NULL;
	if (fd >= 0) {
		struct stat st;

		if (!fstat(fd, &st)) {
			*size = xsize_t(st.st_size);
			if (!*size) {
				/* mmap() is forbidden on empty files */
				error(_("object file %s is empty"), path);
				close(fd);
				return NULL;
			}
			map = xmmap(NULL, *size, PROT_READ, MAP_PRIVATE, fd, 0);
		}
		close(fd);
	}
	return map;
}

void *map_loose_object(struct repository *r,
		       const struct object_id *oid,
		       unsigned long *size)
{
	return map_loose_object_1(r, NULL, oid, size);
}

static int unpack_loose_short_header(git_zstream *stream,
				     unsigned char *map, unsigned long mapsize,
				     void *buffer, unsigned long bufsiz)
{
	/* Get the data stream */
	memset(stream, 0, sizeof(*stream));
	stream->next_in = map;
	stream->avail_in = mapsize;
	stream->next_out = buffer;
	stream->avail_out = bufsiz;

	git_inflate_init(stream);
	return git_inflate(stream, 0);
}

int unpack_loose_header(git_zstream *stream,
			unsigned char *map, unsigned long mapsize,
			void *buffer, unsigned long bufsiz)
{
	int status = unpack_loose_short_header(stream, map, mapsize,
					       buffer, bufsiz);

	if (status < Z_OK)
		return status;

	/* Make sure we have the terminating NUL */
	if (!memchr(buffer, '\0', stream->next_out - (unsigned char *)buffer))
		return -1;
	return 0;
}

static int unpack_loose_header_to_strbuf(git_zstream *stream, unsigned char *map,
					 unsigned long mapsize, void *buffer,
					 unsigned long bufsiz, struct strbuf *header)
{
	int status;

	status = unpack_loose_short_header(stream, map, mapsize, buffer, bufsiz);
	if (status < Z_OK)
		return -1;

	/*
	 * Check if entire header is unpacked in the first iteration.
	 */
	if (memchr(buffer, '\0', stream->next_out - (unsigned char *)buffer))
		return 0;

	/*
	 * buffer[0..bufsiz] was not large enough.  Copy the partial
	 * result out to header, and then append the result of further
	 * reading the stream.
	 */
	strbuf_add(header, buffer, stream->next_out - (unsigned char *)buffer);
	stream->next_out = buffer;
	stream->avail_out = bufsiz;

	do {
		status = git_inflate(stream, 0);
		strbuf_add(header, buffer, stream->next_out - (unsigned char *)buffer);
		if (memchr(buffer, '\0', stream->next_out - (unsigned char *)buffer))
			return 0;
		stream->next_out = buffer;
		stream->avail_out = bufsiz;
	} while (status != Z_STREAM_END);
	return -1;
}

static void *unpack_loose_rest(git_zstream *stream,
			       void *buffer, unsigned long size,
			       const struct object_id *oid)
{
	int bytes = strlen(buffer) + 1;
	unsigned char *buf = xmallocz(size);
	unsigned long n;
	int status = Z_OK;

	n = stream->total_out - bytes;
	if (n > size)
		n = size;
	memcpy(buf, (char *) buffer + bytes, n);
	bytes = n;
	if (bytes <= size) {
		/*
		 * The above condition must be (bytes <= size), not
		 * (bytes < size).  In other words, even though we
		 * expect no more output and set avail_out to zero,
		 * the input zlib stream may have bytes that express
		 * "this concludes the stream", and we *do* want to
		 * eat that input.
		 *
		 * Otherwise we would not be able to test that we
		 * consumed all the input to reach the expected size;
		 * we also want to check that zlib tells us that all
		 * went well with status == Z_STREAM_END at the end.
		 */
		stream->next_out = buf + bytes;
		stream->avail_out = size - bytes;
		while (status == Z_OK)
			status = git_inflate(stream, Z_FINISH);
	}
	if (status == Z_STREAM_END && !stream->avail_in) {
		git_inflate_end(stream);
		return buf;
	}

	if (status < 0)
		error(_("corrupt loose object '%s'"), oid_to_hex(oid));
	else if (stream->avail_in)
		error(_("garbage at end of loose object '%s'"),
		      oid_to_hex(oid));
	free(buf);
	return NULL;
}

/*
 * We used to just use "sscanf()", but that's actually way
 * too permissive for what we want to check. So do an anal
 * object header parse by hand.
 */
static int parse_loose_header_extended(const char *hdr, struct object_info *oi,
				       unsigned int flags)
{
	const char *type_buf = hdr;
	unsigned long size;
	int type, type_len = 0;

	/*
	 * The type can be of any size but is followed by
	 * a space.
	 */
	for (;;) {
		char c = *hdr++;
		if (!c)
			return -1;
		if (c == ' ')
			break;
		type_len++;
	}

	type = type_from_string_gently(type_buf, type_len, 1);
	if (oi->type_name)
		strbuf_add(oi->type_name, type_buf, type_len);
	/*
	 * Set type to 0 if its an unknown object and
	 * we're obtaining the type using '--allow-unknown-type'
	 * option.
	 */
	if ((flags & OBJECT_INFO_ALLOW_UNKNOWN_TYPE) && (type < 0))
		type = 0;
	else if (type < 0)
		die(_("invalid object type"));
	if (oi->typep)
		*oi->typep = type;

	/*
	 * The length must follow immediately, and be in canonical
	 * decimal format (ie "010" is not valid).
	 */
	size = *hdr++ - '0';
	if (size > 9)
		return -1;
	if (size) {
		for (;;) {
			unsigned long c = *hdr - '0';
			if (c > 9)
				break;
			hdr++;
			size = size * 10 + c;
		}
	}

	if (oi->sizep)
		*oi->sizep = size;

	/*
	 * The length must be followed by a zero byte
	 */
	return *hdr ? -1 : type;
}

int parse_loose_header(const char *hdr, unsigned long *sizep)
{
	struct object_info oi = OBJECT_INFO_INIT;

	oi.sizep = sizep;
	return parse_loose_header_extended(hdr, &oi, 0);
}

static int loose_object_info(struct repository *r,
			     const struct object_id *oid,
			     struct object_info *oi, int flags)
{
	int status = 0;
	unsigned long mapsize;
	void *map;
	git_zstream stream;
	char hdr[MAX_HEADER_LEN];
	struct strbuf hdrbuf = STRBUF_INIT;
	unsigned long size_scratch;

	if (oi->delta_base_sha1)
		hashclr(oi->delta_base_sha1);

	/*
	 * If we don't care about type or size, then we don't
	 * need to look inside the object at all. Note that we
	 * do not optimize out the stat call, even if the
	 * caller doesn't care about the disk-size, since our
	 * return value implicitly indicates whether the
	 * object even exists.
	 */
	if (!oi->typep && !oi->type_name && !oi->sizep && !oi->contentp) {
		const char *path;
		struct stat st;
		if (!oi->disk_sizep && (flags & OBJECT_INFO_QUICK))
			return quick_has_loose(r, oid) ? 0 : -1;
		if (stat_loose_object(r, oid, &st, &path) < 0)
			return -1;
		if (oi->disk_sizep)
			*oi->disk_sizep = st.st_size;
		return 0;
	}

	map = map_loose_object(r, oid, &mapsize);
	if (!map)
		return -1;

	if (!oi->sizep)
		oi->sizep = &size_scratch;

	if (oi->disk_sizep)
		*oi->disk_sizep = mapsize;
	if ((flags & OBJECT_INFO_ALLOW_UNKNOWN_TYPE)) {
		if (unpack_loose_header_to_strbuf(&stream, map, mapsize, hdr, sizeof(hdr), &hdrbuf) < 0)
			status = error(_("unable to unpack %s header with --allow-unknown-type"),
				       oid_to_hex(oid));
	} else if (unpack_loose_header(&stream, map, mapsize, hdr, sizeof(hdr)) < 0)
		status = error(_("unable to unpack %s header"),
			       oid_to_hex(oid));
	if (status < 0)
		; /* Do nothing */
	else if (hdrbuf.len) {
		if ((status = parse_loose_header_extended(hdrbuf.buf, oi, flags)) < 0)
			status = error(_("unable to parse %s header with --allow-unknown-type"),
				       oid_to_hex(oid));
	} else if ((status = parse_loose_header_extended(hdr, oi, flags)) < 0)
		status = error(_("unable to parse %s header"), oid_to_hex(oid));

	if (status >= 0 && oi->contentp) {
		*oi->contentp = unpack_loose_rest(&stream, hdr,
						  *oi->sizep, oid);
		if (!*oi->contentp) {
			git_inflate_end(&stream);
			status = -1;
		}
	} else
		git_inflate_end(&stream);

	munmap(map, mapsize);
	if (status && oi->typep)
		*oi->typep = status;
	if (oi->sizep == &size_scratch)
		oi->sizep = NULL;
	strbuf_release(&hdrbuf);
	oi->whence = OI_LOOSE;
	return (status < 0) ? status : 0;
}

int fetch_if_missing = 1;

int oid_object_info_extended(struct repository *r, const struct object_id *oid,
			     struct object_info *oi, unsigned flags)
{
	static struct object_info blank_oi = OBJECT_INFO_INIT;
	struct pack_entry e;
	int rtype;
	const struct object_id *real = oid;
	int already_retried = 0;

	if (flags & OBJECT_INFO_LOOKUP_REPLACE)
		real = lookup_replace_object(r, oid);

	if (is_null_oid(real))
		return -1;

	if (!oi)
		oi = &blank_oi;

	if (!(flags & OBJECT_INFO_SKIP_CACHED)) {
		struct cached_object *co = find_cached_object(real);
		if (co) {
			if (oi->typep)
				*(oi->typep) = co->type;
			if (oi->sizep)
				*(oi->sizep) = co->size;
			if (oi->disk_sizep)
				*(oi->disk_sizep) = 0;
			if (oi->delta_base_sha1)
				hashclr(oi->delta_base_sha1);
			if (oi->type_name)
				strbuf_addstr(oi->type_name, type_name(co->type));
			if (oi->contentp)
				*oi->contentp = xmemdupz(co->buf, co->size);
			oi->whence = OI_CACHED;
			return 0;
		}
	}

	while (1) {
		if (find_pack_entry(r, real, &e))
			break;

		if (flags & OBJECT_INFO_IGNORE_LOOSE)
			return -1;

		/* Most likely it's a loose object. */
		if (!loose_object_info(r, real, oi, flags))
			return 0;

		/* Not a loose object; someone else may have just packed it. */
		if (!(flags & OBJECT_INFO_QUICK)) {
			reprepare_packed_git(r);
			if (find_pack_entry(r, real, &e))
				break;
		}

		/* Check if it is a missing object */
		if (fetch_if_missing && repository_format_partial_clone &&
		    !already_retried && r == the_repository &&
		    !(flags & OBJECT_INFO_SKIP_FETCH_OBJECT)) {
			/*
			 * TODO Investigate having fetch_object() return
			 * TODO error/success and stopping the music here.
			 * TODO Pass a repository struct through fetch_object,
			 * such that arbitrary repositories work.
			 */
			fetch_objects(repository_format_partial_clone, real, 1);
			already_retried = 1;
			continue;
		}

		return -1;
	}

	if (oi == &blank_oi)
		/*
		 * We know that the caller doesn't actually need the
		 * information below, so return early.
		 */
		return 0;
	rtype = packed_object_info(r, e.p, e.offset, oi);
	if (rtype < 0) {
		mark_bad_packed_object(e.p, real->hash);
		return oid_object_info_extended(r, real, oi, 0);
	} else if (oi->whence == OI_PACKED) {
		oi->u.packed.offset = e.offset;
		oi->u.packed.pack = e.p;
		oi->u.packed.is_delta = (rtype == OBJ_REF_DELTA ||
					 rtype == OBJ_OFS_DELTA);
	}

	return 0;
}

/* returns enum object_type or negative */
int oid_object_info(struct repository *r,
		    const struct object_id *oid,
		    unsigned long *sizep)
{
	enum object_type type;
	struct object_info oi = OBJECT_INFO_INIT;

	oi.typep = &type;
	oi.sizep = sizep;
	if (oid_object_info_extended(r, oid, &oi,
				      OBJECT_INFO_LOOKUP_REPLACE) < 0)
		return -1;
	return type;
}

static void *read_object(struct repository *r,
			 const struct object_id *oid, enum object_type *type,
			 unsigned long *size)
{
	struct object_info oi = OBJECT_INFO_INIT;
	void *content;
	oi.typep = type;
	oi.sizep = size;
	oi.contentp = &content;

	if (oid_object_info_extended(r, oid, &oi, 0) < 0)
		return NULL;
	return content;
}

int pretend_object_file(void *buf, unsigned long len, enum object_type type,
			struct object_id *oid)
{
	struct cached_object *co;

	hash_object_file(buf, len, type_name(type), oid);
	if (has_object_file(oid) || find_cached_object(oid))
		return 0;
	ALLOC_GROW(cached_objects, cached_object_nr + 1, cached_object_alloc);
	co = &cached_objects[cached_object_nr++];
	co->size = len;
	co->type = type;
	co->buf = xmalloc(len);
	memcpy(co->buf, buf, len);
	oidcpy(&co->oid, oid);
	return 0;
}

/*
 * This function dies on corrupt objects; the callers who want to
 * deal with them should arrange to call read_object() and give error
 * messages themselves.
 */
void *read_object_file_extended(struct repository *r,
				const struct object_id *oid,
				enum object_type *type,
				unsigned long *size,
				int lookup_replace)
{
	void *data;
	const struct packed_git *p;
	const char *path;
	struct stat st;
	const struct object_id *repl = lookup_replace ?
		lookup_replace_object(r, oid) : oid;

	errno = 0;
	data = read_object(r, repl, type, size);
	if (data)
		return data;

	if (errno && errno != ENOENT)
		die_errno(_("failed to read object %s"), oid_to_hex(oid));

	/* die if we replaced an object with one that does not exist */
	if (repl != oid)
		die(_("replacement %s not found for %s"),
		    oid_to_hex(repl), oid_to_hex(oid));

	if (!stat_loose_object(r, repl, &st, &path))
		die(_("loose object %s (stored in %s) is corrupt"),
		    oid_to_hex(repl), path);

	if ((p = has_packed_and_bad(r, repl->hash)) != NULL)
		die(_("packed object %s (stored in %s) is corrupt"),
		    oid_to_hex(repl), p->pack_name);

	return NULL;
}

void *read_object_with_reference(struct repository *r,
				 const struct object_id *oid,
				 const char *required_type_name,
				 unsigned long *size,
				 struct object_id *actual_oid_return)
{
	enum object_type type, required_type;
	void *buffer;
	unsigned long isize;
	struct object_id actual_oid;

	required_type = type_from_string(required_type_name);
	oidcpy(&actual_oid, oid);
	while (1) {
		int ref_length = -1;
		const char *ref_type = NULL;

		buffer = repo_read_object_file(r, &actual_oid, &type, &isize);
		if (!buffer)
			return NULL;
		if (type == required_type) {
			*size = isize;
			if (actual_oid_return)
				oidcpy(actual_oid_return, &actual_oid);
			return buffer;
		}
		/* Handle references */
		else if (type == OBJ_COMMIT)
			ref_type = "tree ";
		else if (type == OBJ_TAG)
			ref_type = "object ";
		else {
			free(buffer);
			return NULL;
		}
		ref_length = strlen(ref_type);

		if (ref_length + the_hash_algo->hexsz > isize ||
		    memcmp(buffer, ref_type, ref_length) ||
		    get_oid_hex((char *) buffer + ref_length, &actual_oid)) {
			free(buffer);
			return NULL;
		}
		free(buffer);
		/* Now we have the ID of the referred-to object in
		 * actual_oid.  Check again. */
	}
}

static void write_object_file_prepare(const void *buf, unsigned long len,
				      const char *type, struct object_id *oid,
				      char *hdr, int *hdrlen)
{
	git_hash_ctx c;

	/* Generate the header */
	*hdrlen = xsnprintf(hdr, *hdrlen, "%s %"PRIuMAX , type, (uintmax_t)len)+1;

	/* Sha1.. */
	the_hash_algo->init_fn(&c);
	the_hash_algo->update_fn(&c, hdr, *hdrlen);
	the_hash_algo->update_fn(&c, buf, len);
	the_hash_algo->final_fn(oid->hash, &c);
}

/*
 * Move the just written object into its final resting place.
 */
int finalize_object_file(const char *tmpfile, const char *filename)
{
	int ret = 0;

	if (object_creation_mode == OBJECT_CREATION_USES_RENAMES)
		goto try_rename;
	else if (link(tmpfile, filename))
		ret = errno;

	/*
	 * Coda hack - coda doesn't like cross-directory links,
	 * so we fall back to a rename, which will mean that it
	 * won't be able to check collisions, but that's not a
	 * big deal.
	 *
	 * The same holds for FAT formatted media.
	 *
	 * When this succeeds, we just return.  We have nothing
	 * left to unlink.
	 */
	if (ret && ret != EEXIST) {
	try_rename:
		if (!rename(tmpfile, filename))
			goto out;
		ret = errno;
	}
	unlink_or_warn(tmpfile);
	if (ret) {
		if (ret != EEXIST) {
			return error_errno(_("unable to write file %s"), filename);
		}
		/* FIXME!!! Collision check here ? */
	}

out:
	if (adjust_shared_perm(filename))
		return error(_("unable to set permission to '%s'"), filename);
	return 0;
}

static int write_buffer(int fd, const void *buf, size_t len)
{
	if (write_in_full(fd, buf, len) < 0)
		return error_errno(_("file write error"));
	return 0;
}

int hash_object_file(const void *buf, unsigned long len, const char *type,
		     struct object_id *oid)
{
	char hdr[MAX_HEADER_LEN];
	int hdrlen = sizeof(hdr);
	write_object_file_prepare(buf, len, type, oid, hdr, &hdrlen);
	return 0;
}

/* Finalize a file on disk, and close it. */
static void close_loose_object(int fd)
{
	if (fsync_object_files)
		fsync_or_die(fd, "loose object file");
	if (close(fd) != 0)
		die_errno(_("error when closing loose object file"));
}

/* Size of directory component, including the ending '/' */
static inline int directory_size(const char *filename)
{
	const char *s = strrchr(filename, '/');
	if (!s)
		return 0;
	return s - filename + 1;
}

/*
 * This creates a temporary file in the same directory as the final
 * 'filename'
 *
 * We want to avoid cross-directory filename renames, because those
 * can have problems on various filesystems (FAT, NFS, Coda).
 */
static int create_tmpfile(struct strbuf *tmp, const char *filename)
{
	int fd, dirlen = directory_size(filename);

	strbuf_reset(tmp);
	strbuf_add(tmp, filename, dirlen);
	strbuf_addstr(tmp, "tmp_obj_XXXXXX");
	fd = git_mkstemp_mode(tmp->buf, 0444);
	if (fd < 0 && dirlen && errno == ENOENT) {
		/*
		 * Make sure the directory exists; note that the contents
		 * of the buffer are undefined after mkstemp returns an
		 * error, so we have to rewrite the whole buffer from
		 * scratch.
		 */
		strbuf_reset(tmp);
		strbuf_add(tmp, filename, dirlen - 1);
		if (mkdir(tmp->buf, 0777) && errno != EEXIST)
			return -1;
		if (adjust_shared_perm(tmp->buf))
			return -1;

		/* Try again */
		strbuf_addstr(tmp, "/tmp_obj_XXXXXX");
		fd = git_mkstemp_mode(tmp->buf, 0444);
	}
	return fd;
}

static int write_loose_object(const struct object_id *oid, char *hdr,
			      int hdrlen, const void *buf, unsigned long len,
			      time_t mtime)
{
	int fd, ret;
	unsigned char compressed[4096];
	git_zstream stream;
	git_hash_ctx c;
	struct object_id parano_oid;
	static struct strbuf tmp_file = STRBUF_INIT;
	static struct strbuf filename = STRBUF_INIT;

	loose_object_path(the_repository, &filename, oid);

	fd = create_tmpfile(&tmp_file, filename.buf);
	if (fd < 0) {
		if (errno == EACCES)
			return error(_("insufficient permission for adding an object to repository database %s"), get_object_directory());
		else
			return error_errno(_("unable to create temporary file"));
	}

	/* Set it up */
	git_deflate_init(&stream, zlib_compression_level);
	stream.next_out = compressed;
	stream.avail_out = sizeof(compressed);
	the_hash_algo->init_fn(&c);

	/* First header.. */
	stream.next_in = (unsigned char *)hdr;
	stream.avail_in = hdrlen;
	while (git_deflate(&stream, 0) == Z_OK)
		; /* nothing */
	the_hash_algo->update_fn(&c, hdr, hdrlen);

	/* Then the data itself.. */
	stream.next_in = (void *)buf;
	stream.avail_in = len;
	do {
		unsigned char *in0 = stream.next_in;
		ret = git_deflate(&stream, Z_FINISH);
		the_hash_algo->update_fn(&c, in0, stream.next_in - in0);
		if (write_buffer(fd, compressed, stream.next_out - compressed) < 0)
			die(_("unable to write loose object file"));
		stream.next_out = compressed;
		stream.avail_out = sizeof(compressed);
	} while (ret == Z_OK);

	if (ret != Z_STREAM_END)
		die(_("unable to deflate new object %s (%d)"), oid_to_hex(oid),
		    ret);
	ret = git_deflate_end_gently(&stream);
	if (ret != Z_OK)
		die(_("deflateEnd on object %s failed (%d)"), oid_to_hex(oid),
		    ret);
	the_hash_algo->final_fn(parano_oid.hash, &c);
	if (!oideq(oid, &parano_oid))
		die(_("confused by unstable object source data for %s"),
		    oid_to_hex(oid));

	close_loose_object(fd);

	if (mtime) {
		struct utimbuf utb;
		utb.actime = mtime;
		utb.modtime = mtime;
		if (utime(tmp_file.buf, &utb) < 0)
			warning_errno(_("failed utime() on %s"), tmp_file.buf);
	}

	return finalize_object_file(tmp_file.buf, filename.buf);
}

static int freshen_loose_object(const struct object_id *oid)
{
	return check_and_freshen(oid, 1);
}

static int freshen_packed_object(const struct object_id *oid)
{
	struct pack_entry e;
	if (!find_pack_entry(the_repository, oid, &e))
		return 0;
	if (e.p->freshened)
		return 1;
	if (!freshen_file(e.p->pack_name))
		return 0;
	e.p->freshened = 1;
	return 1;
}

int write_object_file(const void *buf, unsigned long len, const char *type,
		      struct object_id *oid)
{
	char hdr[MAX_HEADER_LEN];
	int hdrlen = sizeof(hdr);

	/* Normally if we have it in the pack then we do not bother writing
	 * it out into .git/objects/??/?{38} file.
	 */
	write_object_file_prepare(buf, len, type, oid, hdr, &hdrlen);
	if (freshen_packed_object(oid) || freshen_loose_object(oid))
		return 0;
	return write_loose_object(oid, hdr, hdrlen, buf, len, 0);
}

int hash_object_file_literally(const void *buf, unsigned long len,
			       const char *type, struct object_id *oid,
			       unsigned flags)
{
	char *header;
	int hdrlen, status = 0;

	/* type string, SP, %lu of the length plus NUL must fit this */
	hdrlen = strlen(type) + MAX_HEADER_LEN;
	header = xmalloc(hdrlen);
	write_object_file_prepare(buf, len, type, oid, header, &hdrlen);

	if (!(flags & HASH_WRITE_OBJECT))
		goto cleanup;
	if (freshen_packed_object(oid) || freshen_loose_object(oid))
		goto cleanup;
	status = write_loose_object(oid, header, hdrlen, buf, len, 0);

cleanup:
	free(header);
	return status;
}

int force_object_loose(const struct object_id *oid, time_t mtime)
{
	void *buf;
	unsigned long len;
	enum object_type type;
	char hdr[MAX_HEADER_LEN];
	int hdrlen;
	int ret;

	if (has_loose_object(oid))
		return 0;
	buf = read_object(the_repository, oid, &type, &len);
	if (!buf)
		return error(_("cannot read object for %s"), oid_to_hex(oid));
	hdrlen = xsnprintf(hdr, sizeof(hdr), "%s %"PRIuMAX , type_name(type), (uintmax_t)len) + 1;
	ret = write_loose_object(oid, hdr, hdrlen, buf, len, mtime);
	free(buf);

	return ret;
}

int repo_has_object_file_with_flags(struct repository *r,
				    const struct object_id *oid, int flags)
{
	if (!startup_info->have_repository)
		return 0;
	return oid_object_info_extended(r, oid, NULL,
					flags | OBJECT_INFO_SKIP_CACHED) >= 0;
}

int repo_has_object_file(struct repository *r,
			 const struct object_id *oid)
{
	return repo_has_object_file_with_flags(r, oid, 0);
}

static void check_tree(const void *buf, size_t size)
{
	struct tree_desc desc;
	struct name_entry entry;

	init_tree_desc(&desc, buf, size);
	while (tree_entry(&desc, &entry))
		/* do nothing
		 * tree_entry() will die() on malformed entries */
		;
}

static void check_commit(const void *buf, size_t size)
{
	struct commit c;
	memset(&c, 0, sizeof(c));
	if (parse_commit_buffer(the_repository, &c, buf, size, 0))
		die(_("corrupt commit"));
}

static void check_tag(const void *buf, size_t size)
{
	struct tag t;
	memset(&t, 0, sizeof(t));
	if (parse_tag_buffer(the_repository, &t, buf, size))
		die(_("corrupt tag"));
}

static int index_mem(struct index_state *istate,
		     struct object_id *oid, void *buf, size_t size,
		     enum object_type type,
		     const char *path, unsigned flags)
{
	int ret, re_allocated = 0;
	int write_object = flags & HASH_WRITE_OBJECT;

	if (!type)
		type = OBJ_BLOB;

	/*
	 * Convert blobs to git internal format
	 */
	if ((type == OBJ_BLOB) && path) {
		struct strbuf nbuf = STRBUF_INIT;
		if (convert_to_git(istate, path, buf, size, &nbuf,
				   get_conv_flags(flags))) {
			buf = strbuf_detach(&nbuf, &size);
			re_allocated = 1;
		}
	}
	if (flags & HASH_FORMAT_CHECK) {
		if (type == OBJ_TREE)
			check_tree(buf, size);
		if (type == OBJ_COMMIT)
			check_commit(buf, size);
		if (type == OBJ_TAG)
			check_tag(buf, size);
	}

	if (write_object)
		ret = write_object_file(buf, size, type_name(type), oid);
	else
		ret = hash_object_file(buf, size, type_name(type), oid);
	if (re_allocated)
		free(buf);
	return ret;
}

static int index_stream_convert_blob(struct index_state *istate,
				     struct object_id *oid,
				     int fd,
				     const char *path,
				     unsigned flags)
{
	int ret;
	const int write_object = flags & HASH_WRITE_OBJECT;
	struct strbuf sbuf = STRBUF_INIT;

	assert(path);
	assert(would_convert_to_git_filter_fd(istate, path));

	convert_to_git_filter_fd(istate, path, fd, &sbuf,
				 get_conv_flags(flags));

	if (write_object)
		ret = write_object_file(sbuf.buf, sbuf.len, type_name(OBJ_BLOB),
					oid);
	else
		ret = hash_object_file(sbuf.buf, sbuf.len, type_name(OBJ_BLOB),
				       oid);
	strbuf_release(&sbuf);
	return ret;
}

static int index_pipe(struct index_state *istate, struct object_id *oid,
		      int fd, enum object_type type,
		      const char *path, unsigned flags)
{
	struct strbuf sbuf = STRBUF_INIT;
	int ret;

	if (strbuf_read(&sbuf, fd, 4096) >= 0)
		ret = index_mem(istate, oid, sbuf.buf, sbuf.len, type, path, flags);
	else
		ret = -1;
	strbuf_release(&sbuf);
	return ret;
}

#define SMALL_FILE_SIZE (32*1024)

static int index_core(struct index_state *istate,
		      struct object_id *oid, int fd, size_t size,
		      enum object_type type, const char *path,
		      unsigned flags)
{
	int ret;

	if (!size) {
		ret = index_mem(istate, oid, "", size, type, path, flags);
	} else if (size <= SMALL_FILE_SIZE) {
		char *buf = xmalloc(size);
		ssize_t read_result = read_in_full(fd, buf, size);
		if (read_result < 0)
			ret = error_errno(_("read error while indexing %s"),
					  path ? path : "<unknown>");
		else if (read_result != size)
			ret = error(_("short read while indexing %s"),
				    path ? path : "<unknown>");
		else
			ret = index_mem(istate, oid, buf, size, type, path, flags);
		free(buf);
	} else {
		void *buf = xmmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
		ret = index_mem(istate, oid, buf, size, type, path, flags);
		munmap(buf, size);
	}
	return ret;
}

/*
 * This creates one packfile per large blob unless bulk-checkin
 * machinery is "plugged".
 *
 * This also bypasses the usual "convert-to-git" dance, and that is on
 * purpose. We could write a streaming version of the converting
 * functions and insert that before feeding the data to fast-import
 * (or equivalent in-core API described above). However, that is
 * somewhat complicated, as we do not know the size of the filter
 * result, which we need to know beforehand when writing a git object.
 * Since the primary motivation for trying to stream from the working
 * tree file and to avoid mmaping it in core is to deal with large
 * binary blobs, they generally do not want to get any conversion, and
 * callers should avoid this code path when filters are requested.
 */
static int index_stream(struct object_id *oid, int fd, size_t size,
			enum object_type type, const char *path,
			unsigned flags)
{
	return index_bulk_checkin(oid, fd, size, type, path, flags);
}

int index_fd(struct index_state *istate, struct object_id *oid,
	     int fd, struct stat *st,
	     enum object_type type, const char *path, unsigned flags)
{
	int ret;

	/*
	 * Call xsize_t() only when needed to avoid potentially unnecessary
	 * die() for large files.
	 */
	if (type == OBJ_BLOB && path && would_convert_to_git_filter_fd(istate, path))
		ret = index_stream_convert_blob(istate, oid, fd, path, flags);
	else if (!S_ISREG(st->st_mode))
		ret = index_pipe(istate, oid, fd, type, path, flags);
	else if (st->st_size <= big_file_threshold || type != OBJ_BLOB ||
		 (path && would_convert_to_git(istate, path)))
		ret = index_core(istate, oid, fd, xsize_t(st->st_size),
				 type, path, flags);
	else
		ret = index_stream(oid, fd, xsize_t(st->st_size), type, path,
				   flags);
	close(fd);
	return ret;
}

int index_path(struct index_state *istate, struct object_id *oid,
	       const char *path, struct stat *st, unsigned flags)
{
	int fd;
	struct strbuf sb = STRBUF_INIT;
	int rc = 0;

	switch (st->st_mode & S_IFMT) {
	case S_IFREG:
		fd = open(path, O_RDONLY);
		if (fd < 0)
			return error_errno("open(\"%s\")", path);
		if (index_fd(istate, oid, fd, st, OBJ_BLOB, path, flags) < 0)
			return error(_("%s: failed to insert into database"),
				     path);
		break;
	case S_IFLNK:
		if (strbuf_readlink(&sb, path, st->st_size))
			return error_errno("readlink(\"%s\")", path);
		if (!(flags & HASH_WRITE_OBJECT))
			hash_object_file(sb.buf, sb.len, blob_type, oid);
		else if (write_object_file(sb.buf, sb.len, blob_type, oid))
			rc = error(_("%s: failed to insert into database"), path);
		strbuf_release(&sb);
		break;
	case S_IFDIR:
		return resolve_gitlink_ref(path, "HEAD", oid);
	default:
		return error(_("%s: unsupported file type"), path);
	}
	return rc;
}

int read_pack_header(int fd, struct pack_header *header)
{
	if (read_in_full(fd, header, sizeof(*header)) != sizeof(*header))
		/* "eof before pack header was fully read" */
		return PH_ERROR_EOF;

	if (header->hdr_signature != htonl(PACK_SIGNATURE))
		/* "protocol error (pack signature mismatch detected)" */
		return PH_ERROR_PACK_SIGNATURE;
	if (!pack_version_ok(header->hdr_version))
		/* "protocol error (pack version unsupported)" */
		return PH_ERROR_PROTOCOL;
	return 0;
}

void assert_oid_type(const struct object_id *oid, enum object_type expect)
{
	enum object_type type = oid_object_info(the_repository, oid, NULL);
	if (type < 0)
		die(_("%s is not a valid object"), oid_to_hex(oid));
	if (type != expect)
		die(_("%s is not a valid '%s' object"), oid_to_hex(oid),
		    type_name(expect));
}

int for_each_file_in_obj_subdir(unsigned int subdir_nr,
				struct strbuf *path,
				each_loose_object_fn obj_cb,
				each_loose_cruft_fn cruft_cb,
				each_loose_subdir_fn subdir_cb,
				void *data)
{
	size_t origlen, baselen;
	DIR *dir;
	struct dirent *de;
	int r = 0;
	struct object_id oid;

	if (subdir_nr > 0xff)
		BUG("invalid loose object subdirectory: %x", subdir_nr);

	origlen = path->len;
	strbuf_complete(path, '/');
	strbuf_addf(path, "%02x", subdir_nr);

	dir = opendir(path->buf);
	if (!dir) {
		if (errno != ENOENT)
			r = error_errno(_("unable to open %s"), path->buf);
		strbuf_setlen(path, origlen);
		return r;
	}

	oid.hash[0] = subdir_nr;
	strbuf_addch(path, '/');
	baselen = path->len;

	while ((de = readdir(dir))) {
		size_t namelen;
		if (is_dot_or_dotdot(de->d_name))
			continue;

		namelen = strlen(de->d_name);
		strbuf_setlen(path, baselen);
		strbuf_add(path, de->d_name, namelen);
		if (namelen == the_hash_algo->hexsz - 2 &&
		    !hex_to_bytes(oid.hash + 1, de->d_name,
				  the_hash_algo->rawsz - 1)) {
			if (obj_cb) {
				r = obj_cb(&oid, path->buf, data);
				if (r)
					break;
			}
			continue;
		}

		if (cruft_cb) {
			r = cruft_cb(de->d_name, path->buf, data);
			if (r)
				break;
		}
	}
	closedir(dir);

	strbuf_setlen(path, baselen - 1);
	if (!r && subdir_cb)
		r = subdir_cb(subdir_nr, path->buf, data);

	strbuf_setlen(path, origlen);

	return r;
}

int for_each_loose_file_in_objdir_buf(struct strbuf *path,
			    each_loose_object_fn obj_cb,
			    each_loose_cruft_fn cruft_cb,
			    each_loose_subdir_fn subdir_cb,
			    void *data)
{
	int r = 0;
	int i;

	for (i = 0; i < 256; i++) {
		r = for_each_file_in_obj_subdir(i, path, obj_cb, cruft_cb,
						subdir_cb, data);
		if (r)
			break;
	}

	return r;
}

int for_each_loose_file_in_objdir(const char *path,
				  each_loose_object_fn obj_cb,
				  each_loose_cruft_fn cruft_cb,
				  each_loose_subdir_fn subdir_cb,
				  void *data)
{
	struct strbuf buf = STRBUF_INIT;
	int r;

	strbuf_addstr(&buf, path);
	r = for_each_loose_file_in_objdir_buf(&buf, obj_cb, cruft_cb,
					      subdir_cb, data);
	strbuf_release(&buf);

	return r;
}

int for_each_loose_object(each_loose_object_fn cb, void *data,
			  enum for_each_object_flags flags)
{
	struct object_directory *odb;

	prepare_alt_odb(the_repository);
	for (odb = the_repository->objects->odb; odb; odb = odb->next) {
		int r = for_each_loose_file_in_objdir(odb->path, cb, NULL,
						      NULL, data);
		if (r)
			return r;

		if (flags & FOR_EACH_OBJECT_LOCAL_ONLY)
			break;
	}

	return 0;
}

static int append_loose_object(const struct object_id *oid, const char *path,
			       void *data)
{
	oid_array_append(data, oid);
	return 0;
}

struct oid_array *odb_loose_cache(struct object_directory *odb,
				  const struct object_id *oid)
{
	int subdir_nr = oid->hash[0];
	struct strbuf buf = STRBUF_INIT;

	if (subdir_nr < 0 ||
	    subdir_nr >= ARRAY_SIZE(odb->loose_objects_subdir_seen))
		BUG("subdir_nr out of range");

	if (odb->loose_objects_subdir_seen[subdir_nr])
		return &odb->loose_objects_cache[subdir_nr];

	strbuf_addstr(&buf, odb->path);
	for_each_file_in_obj_subdir(subdir_nr, &buf,
				    append_loose_object,
				    NULL, NULL,
				    &odb->loose_objects_cache[subdir_nr]);
	odb->loose_objects_subdir_seen[subdir_nr] = 1;
	strbuf_release(&buf);
	return &odb->loose_objects_cache[subdir_nr];
}

void odb_clear_loose_cache(struct object_directory *odb)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(odb->loose_objects_cache); i++)
		oid_array_clear(&odb->loose_objects_cache[i]);
	memset(&odb->loose_objects_subdir_seen, 0,
	       sizeof(odb->loose_objects_subdir_seen));
}

static int check_stream_oid(git_zstream *stream,
			    const char *hdr,
			    unsigned long size,
			    const char *path,
			    const struct object_id *expected_oid)
{
	git_hash_ctx c;
	struct object_id real_oid;
	unsigned char buf[4096];
	unsigned long total_read;
	int status = Z_OK;

	the_hash_algo->init_fn(&c);
	the_hash_algo->update_fn(&c, hdr, stream->total_out);

	/*
	 * We already read some bytes into hdr, but the ones up to the NUL
	 * do not count against the object's content size.
	 */
	total_read = stream->total_out - strlen(hdr) - 1;

	/*
	 * This size comparison must be "<=" to read the final zlib packets;
	 * see the comment in unpack_loose_rest for details.
	 */
	while (total_read <= size &&
	       (status == Z_OK ||
		(status == Z_BUF_ERROR && !stream->avail_out))) {
		stream->next_out = buf;
		stream->avail_out = sizeof(buf);
		if (size - total_read < stream->avail_out)
			stream->avail_out = size - total_read;
		status = git_inflate(stream, Z_FINISH);
		the_hash_algo->update_fn(&c, buf, stream->next_out - buf);
		total_read += stream->next_out - buf;
	}
	git_inflate_end(stream);

	if (status != Z_STREAM_END) {
		error(_("corrupt loose object '%s'"), oid_to_hex(expected_oid));
		return -1;
	}
	if (stream->avail_in) {
		error(_("garbage at end of loose object '%s'"),
		      oid_to_hex(expected_oid));
		return -1;
	}

	the_hash_algo->final_fn(real_oid.hash, &c);
	if (!oideq(expected_oid, &real_oid)) {
		error(_("hash mismatch for %s (expected %s)"), path,
		      oid_to_hex(expected_oid));
		return -1;
	}

	return 0;
}

int read_loose_object(const char *path,
		      const struct object_id *expected_oid,
		      enum object_type *type,
		      unsigned long *size,
		      void **contents)
{
	int ret = -1;
	void *map = NULL;
	unsigned long mapsize;
	git_zstream stream;
	char hdr[MAX_HEADER_LEN];

	*contents = NULL;

	map = map_loose_object_1(the_repository, path, NULL, &mapsize);
	if (!map) {
		error_errno(_("unable to mmap %s"), path);
		goto out;
	}

	if (unpack_loose_header(&stream, map, mapsize, hdr, sizeof(hdr)) < 0) {
		error(_("unable to unpack header of %s"), path);
		goto out;
	}

	*type = parse_loose_header(hdr, size);
	if (*type < 0) {
		error(_("unable to parse header of %s"), path);
		git_inflate_end(&stream);
		goto out;
	}

	if (*type == OBJ_BLOB && *size > big_file_threshold) {
		if (check_stream_oid(&stream, hdr, *size, path, expected_oid) < 0)
			goto out;
	} else {
		*contents = unpack_loose_rest(&stream, hdr, *size, expected_oid);
		if (!*contents) {
			error(_("unable to unpack contents of %s"), path);
			git_inflate_end(&stream);
			goto out;
		}
		if (check_object_signature(expected_oid, *contents,
					 *size, type_name(*type))) {
			error(_("hash mismatch for %s (expected %s)"), path,
			      oid_to_hex(expected_oid));
			free(*contents);
			goto out;
		}
	}

	ret = 0; /* everything checks out */

out:
	if (map)
		munmap(map, mapsize);
	return ret;
}
