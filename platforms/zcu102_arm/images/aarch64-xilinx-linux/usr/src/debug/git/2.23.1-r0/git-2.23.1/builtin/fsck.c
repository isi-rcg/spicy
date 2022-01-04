#define USE_THE_INDEX_COMPATIBILITY_MACROS
#include "builtin.h"
#include "cache.h"
#include "repository.h"
#include "config.h"
#include "commit.h"
#include "tree.h"
#include "blob.h"
#include "tag.h"
#include "refs.h"
#include "pack.h"
#include "cache-tree.h"
#include "tree-walk.h"
#include "fsck.h"
#include "parse-options.h"
#include "dir.h"
#include "progress.h"
#include "streaming.h"
#include "decorate.h"
#include "packfile.h"
#include "object-store.h"
#include "run-command.h"
#include "worktree.h"

#define REACHABLE 0x0001
#define SEEN      0x0002
#define HAS_OBJ   0x0004
/* This flag is set if something points to this object. */
#define USED      0x0008

static int show_root;
static int show_tags;
static int show_unreachable;
static int include_reflogs = 1;
static int check_full = 1;
static int connectivity_only;
static int check_strict;
static int keep_cache_objects;
static struct fsck_options fsck_walk_options = FSCK_OPTIONS_DEFAULT;
static struct fsck_options fsck_obj_options = FSCK_OPTIONS_DEFAULT;
static int errors_found;
static int write_lost_and_found;
static int verbose;
static int show_progress = -1;
static int show_dangling = 1;
static int name_objects;
#define ERROR_OBJECT 01
#define ERROR_REACHABLE 02
#define ERROR_PACK 04
#define ERROR_REFS 010
#define ERROR_COMMIT_GRAPH 020

static const char *describe_object(struct object *obj)
{
	static struct strbuf bufs[] = {
		STRBUF_INIT, STRBUF_INIT, STRBUF_INIT, STRBUF_INIT
	};
	static int b = 0;
	struct strbuf *buf;
	char *name = NULL;

	if (name_objects)
		name = lookup_decoration(fsck_walk_options.object_names, obj);

	buf = bufs + b;
	b = (b + 1) % ARRAY_SIZE(bufs);
	strbuf_reset(buf);
	strbuf_addstr(buf, oid_to_hex(&obj->oid));
	if (name)
		strbuf_addf(buf, " (%s)", name);

	return buf->buf;
}

static const char *printable_type(struct object *obj)
{
	const char *ret;

	if (obj->type == OBJ_NONE) {
		enum object_type type = oid_object_info(the_repository,
							&obj->oid, NULL);
		if (type > 0)
			object_as_type(the_repository, obj, type, 0);
	}

	ret = type_name(obj->type);
	if (!ret)
		ret = _("unknown");

	return ret;
}

static int fsck_config(const char *var, const char *value, void *cb)
{
	if (strcmp(var, "fsck.skiplist") == 0) {
		const char *path;
		struct strbuf sb = STRBUF_INIT;

		if (git_config_pathname(&path, var, value))
			return 1;
		strbuf_addf(&sb, "skiplist=%s", path);
		free((char *)path);
		fsck_set_msg_types(&fsck_obj_options, sb.buf);
		strbuf_release(&sb);
		return 0;
	}

	if (skip_prefix(var, "fsck.", &var)) {
		fsck_set_msg_type(&fsck_obj_options, var, value);
		return 0;
	}

	return git_default_config(var, value, cb);
}

static int objerror(struct object *obj, const char *err)
{
	errors_found |= ERROR_OBJECT;
	/* TRANSLATORS: e.g. error in tree 01bfda: <more explanation> */
	fprintf_ln(stderr, _("error in %s %s: %s"),
		   printable_type(obj), describe_object(obj), err);
	return -1;
}

static int fsck_error_func(struct fsck_options *o,
	struct object *obj, int type, const char *message)
{
	switch (type) {
	case FSCK_WARN:
		/* TRANSLATORS: e.g. warning in tree 01bfda: <more explanation> */
		fprintf_ln(stderr, _("warning in %s %s: %s"),
			   printable_type(obj), describe_object(obj), message);
		return 0;
	case FSCK_ERROR:
		/* TRANSLATORS: e.g. error in tree 01bfda: <more explanation> */
		fprintf_ln(stderr, _("error in %s %s: %s"),
			   printable_type(obj), describe_object(obj), message);
		return 1;
	default:
		BUG("%d (FSCK_IGNORE?) should never trigger this callback", type);
	}
}

static struct object_array pending;

static int mark_object(struct object *obj, int type, void *data, struct fsck_options *options)
{
	struct object *parent = data;

	/*
	 * The only case data is NULL or type is OBJ_ANY is when
	 * mark_object_reachable() calls us.  All the callers of
	 * that function has non-NULL obj hence ...
	 */
	if (!obj) {
		/* ... these references to parent->fld are safe here */
		printf_ln(_("broken link from %7s %s"),
			  printable_type(parent), describe_object(parent));
		printf_ln(_("broken link from %7s %s"),
			  (type == OBJ_ANY ? _("unknown") : type_name(type)),
			  _("unknown"));
		errors_found |= ERROR_REACHABLE;
		return 1;
	}

	if (type != OBJ_ANY && obj->type != type)
		/* ... and the reference to parent is safe here */
		objerror(parent, _("wrong object type in link"));

	if (obj->flags & REACHABLE)
		return 0;
	obj->flags |= REACHABLE;

	if (is_promisor_object(&obj->oid))
		/*
		 * Further recursion does not need to be performed on this
		 * object since it is a promisor object (so it does not need to
		 * be added to "pending").
		 */
		return 0;

	if (!(obj->flags & HAS_OBJ)) {
		if (parent && !has_object_file(&obj->oid)) {
			printf_ln(_("broken link from %7s %s\n"
				    "              to %7s %s"),
				  printable_type(parent),
				  describe_object(parent),
				  printable_type(obj),
				  describe_object(obj));
			errors_found |= ERROR_REACHABLE;
		}
		return 1;
	}

	add_object_array(obj, NULL, &pending);
	return 0;
}

static void mark_object_reachable(struct object *obj)
{
	mark_object(obj, OBJ_ANY, NULL, NULL);
}

static int traverse_one_object(struct object *obj)
{
	int result = fsck_walk(obj, obj, &fsck_walk_options);

	if (obj->type == OBJ_TREE) {
		struct tree *tree = (struct tree *)obj;
		free_tree_buffer(tree);
	}
	return result;
}

static int traverse_reachable(void)
{
	struct progress *progress = NULL;
	unsigned int nr = 0;
	int result = 0;
	if (show_progress)
		progress = start_delayed_progress(_("Checking connectivity"), 0);
	while (pending.nr) {
		result |= traverse_one_object(object_array_pop(&pending));
		display_progress(progress, ++nr);
	}
	stop_progress(&progress);
	return !!result;
}

static int mark_used(struct object *obj, int type, void *data, struct fsck_options *options)
{
	if (!obj)
		return 1;
	obj->flags |= USED;
	return 0;
}

static void mark_unreachable_referents(const struct object_id *oid)
{
	struct fsck_options options = FSCK_OPTIONS_DEFAULT;
	struct object *obj = lookup_object(the_repository, oid);

	if (!obj || !(obj->flags & HAS_OBJ))
		return; /* not part of our original set */
	if (obj->flags & REACHABLE)
		return; /* reachable objects already traversed */

	/*
	 * Avoid passing OBJ_NONE to fsck_walk, which will parse the object
	 * (and we want to avoid parsing blobs).
	 */
	if (obj->type == OBJ_NONE) {
		enum object_type type = oid_object_info(the_repository,
							&obj->oid, NULL);
		if (type > 0)
			object_as_type(the_repository, obj, type, 0);
	}

	options.walk = mark_used;
	fsck_walk(obj, NULL, &options);
}

static int mark_loose_unreachable_referents(const struct object_id *oid,
					    const char *path,
					    void *data)
{
	mark_unreachable_referents(oid);
	return 0;
}

static int mark_packed_unreachable_referents(const struct object_id *oid,
					     struct packed_git *pack,
					     uint32_t pos,
					     void *data)
{
	mark_unreachable_referents(oid);
	return 0;
}

/*
 * Check a single reachable object
 */
static void check_reachable_object(struct object *obj)
{
	/*
	 * We obviously want the object to be parsed,
	 * except if it was in a pack-file and we didn't
	 * do a full fsck
	 */
	if (!(obj->flags & HAS_OBJ)) {
		if (is_promisor_object(&obj->oid))
			return;
		if (has_object_pack(&obj->oid))
			return; /* it is in pack - forget about it */
		printf_ln(_("missing %s %s"), printable_type(obj),
			  describe_object(obj));
		errors_found |= ERROR_REACHABLE;
		return;
	}
}

/*
 * Check a single unreachable object
 */
static void check_unreachable_object(struct object *obj)
{
	/*
	 * Missing unreachable object? Ignore it. It's not like
	 * we miss it (since it can't be reached), nor do we want
	 * to complain about it being unreachable (since it does
	 * not exist).
	 */
	if (!(obj->flags & HAS_OBJ))
		return;

	/*
	 * Unreachable object that exists? Show it if asked to,
	 * since this is something that is prunable.
	 */
	if (show_unreachable) {
		printf_ln(_("unreachable %s %s"), printable_type(obj),
			  describe_object(obj));
		return;
	}

	/*
	 * "!USED" means that nothing at all points to it, including
	 * other unreachable objects. In other words, it's the "tip"
	 * of some set of unreachable objects, usually a commit that
	 * got dropped.
	 *
	 * Such starting points are more interesting than some random
	 * set of unreachable objects, so we show them even if the user
	 * hasn't asked for _all_ unreachable objects. If you have
	 * deleted a branch by mistake, this is a prime candidate to
	 * start looking at, for example.
	 */
	if (!(obj->flags & USED)) {
		if (show_dangling)
			printf_ln(_("dangling %s %s"), printable_type(obj),
				  describe_object(obj));
		if (write_lost_and_found) {
			char *filename = git_pathdup("lost-found/%s/%s",
				obj->type == OBJ_COMMIT ? "commit" : "other",
				describe_object(obj));
			FILE *f;

			if (safe_create_leading_directories_const(filename)) {
				error(_("could not create lost-found"));
				free(filename);
				return;
			}
			f = xfopen(filename, "w");
			if (obj->type == OBJ_BLOB) {
				if (stream_blob_to_fd(fileno(f), &obj->oid, NULL, 1))
					die_errno(_("could not write '%s'"), filename);
			} else
				fprintf(f, "%s\n", describe_object(obj));
			if (fclose(f))
				die_errno(_("could not finish '%s'"),
					  filename);
			free(filename);
		}
		return;
	}

	/*
	 * Otherwise? It's there, it's unreachable, and some other unreachable
	 * object points to it. Ignore it - it's not interesting, and we showed
	 * all the interesting cases above.
	 */
}

static void check_object(struct object *obj)
{
	if (verbose)
		fprintf_ln(stderr, _("Checking %s"), describe_object(obj));

	if (obj->flags & REACHABLE)
		check_reachable_object(obj);
	else
		check_unreachable_object(obj);
}

static void check_connectivity(void)
{
	int i, max;

	/* Traverse the pending reachable objects */
	traverse_reachable();

	/*
	 * With --connectivity-only, we won't have actually opened and marked
	 * unreachable objects with USED. Do that now to make --dangling, etc
	 * accurate.
	 */
	if (connectivity_only && (show_dangling || write_lost_and_found)) {
		/*
		 * Even though we already have a "struct object" for each of
		 * these in memory, we must not iterate over the internal
		 * object hash as we do below. Our loop would potentially
		 * resize the hash, making our iteration invalid.
		 *
		 * Instead, we'll just go back to the source list of objects,
		 * and ignore any that weren't present in our earlier
		 * traversal.
		 */
		for_each_loose_object(mark_loose_unreachable_referents, NULL, 0);
		for_each_packed_object(mark_packed_unreachable_referents, NULL, 0);
	}

	/* Look up all the requirements, warn about missing objects.. */
	max = get_max_object_index();
	if (verbose)
		fprintf_ln(stderr, _("Checking connectivity (%d objects)"), max);

	for (i = 0; i < max; i++) {
		struct object *obj = get_indexed_object(i);

		if (obj)
			check_object(obj);
	}
}

static int fsck_obj(struct object *obj, void *buffer, unsigned long size)
{
	int err;

	if (obj->flags & SEEN)
		return 0;
	obj->flags |= SEEN;

	if (verbose)
		fprintf_ln(stderr, _("Checking %s %s"),
			   printable_type(obj), describe_object(obj));

	if (fsck_walk(obj, NULL, &fsck_obj_options))
		objerror(obj, _("broken links"));
	err = fsck_object(obj, buffer, size, &fsck_obj_options);
	if (err)
		goto out;

	if (obj->type == OBJ_COMMIT) {
		struct commit *commit = (struct commit *) obj;

		if (!commit->parents && show_root)
			printf_ln(_("root %s"),
				  describe_object(&commit->object));
	}

	if (obj->type == OBJ_TAG) {
		struct tag *tag = (struct tag *) obj;

		if (show_tags && tag->tagged) {
			printf_ln(_("tagged %s %s (%s) in %s"),
				  printable_type(tag->tagged),
				  describe_object(tag->tagged),
				  tag->tag,
				  describe_object(&tag->object));
		}
	}

out:
	if (obj->type == OBJ_TREE)
		free_tree_buffer((struct tree *)obj);
	if (obj->type == OBJ_COMMIT)
		free_commit_buffer(the_repository->parsed_objects,
				   (struct commit *)obj);
	return err;
}

static int fsck_obj_buffer(const struct object_id *oid, enum object_type type,
			   unsigned long size, void *buffer, int *eaten)
{
	/*
	 * Note, buffer may be NULL if type is OBJ_BLOB. See
	 * verify_packfile(), data_valid variable for details.
	 */
	struct object *obj;
	obj = parse_object_buffer(the_repository, oid, type, size, buffer,
				  eaten);
	if (!obj) {
		errors_found |= ERROR_OBJECT;
		return error(_("%s: object corrupt or missing"),
			     oid_to_hex(oid));
	}
	obj->flags &= ~(REACHABLE | SEEN);
	obj->flags |= HAS_OBJ;
	return fsck_obj(obj, buffer, size);
}

static int default_refs;

static void fsck_handle_reflog_oid(const char *refname, struct object_id *oid,
	timestamp_t timestamp)
{
	struct object *obj;

	if (!is_null_oid(oid)) {
		obj = lookup_object(the_repository, oid);
		if (obj && (obj->flags & HAS_OBJ)) {
			if (timestamp && name_objects)
				add_decoration(fsck_walk_options.object_names,
					obj,
					xstrfmt("%s@{%"PRItime"}", refname, timestamp));
			obj->flags |= USED;
			mark_object_reachable(obj);
		} else if (!is_promisor_object(oid)) {
			error(_("%s: invalid reflog entry %s"),
			      refname, oid_to_hex(oid));
			errors_found |= ERROR_REACHABLE;
		}
	}
}

static int fsck_handle_reflog_ent(struct object_id *ooid, struct object_id *noid,
		const char *email, timestamp_t timestamp, int tz,
		const char *message, void *cb_data)
{
	const char *refname = cb_data;

	if (verbose)
		fprintf_ln(stderr, _("Checking reflog %s->%s"),
			   oid_to_hex(ooid), oid_to_hex(noid));

	fsck_handle_reflog_oid(refname, ooid, 0);
	fsck_handle_reflog_oid(refname, noid, timestamp);
	return 0;
}

static int fsck_handle_reflog(const char *logname, const struct object_id *oid,
			      int flag, void *cb_data)
{
	struct strbuf refname = STRBUF_INIT;

	strbuf_worktree_ref(cb_data, &refname, logname);
	for_each_reflog_ent(refname.buf, fsck_handle_reflog_ent, refname.buf);
	strbuf_release(&refname);
	return 0;
}

static int fsck_handle_ref(const char *refname, const struct object_id *oid,
			   int flag, void *cb_data)
{
	struct object *obj;

	obj = parse_object(the_repository, oid);
	if (!obj) {
		if (is_promisor_object(oid)) {
			/*
			 * Increment default_refs anyway, because this is a
			 * valid ref.
			 */
			 default_refs++;
			 return 0;
		}
		error(_("%s: invalid sha1 pointer %s"),
		      refname, oid_to_hex(oid));
		errors_found |= ERROR_REACHABLE;
		/* We'll continue with the rest despite the error.. */
		return 0;
	}
	if (obj->type != OBJ_COMMIT && is_branch(refname)) {
		error(_("%s: not a commit"), refname);
		errors_found |= ERROR_REFS;
	}
	default_refs++;
	obj->flags |= USED;
	if (name_objects)
		add_decoration(fsck_walk_options.object_names,
			obj, xstrdup(refname));
	mark_object_reachable(obj);

	return 0;
}

static int fsck_head_link(const char *head_ref_name,
			  const char **head_points_at,
			  struct object_id *head_oid);

static void get_default_heads(void)
{
	struct worktree **worktrees, **p;
	const char *head_points_at;
	struct object_id head_oid;

	for_each_rawref(fsck_handle_ref, NULL);

	worktrees = get_worktrees(0);
	for (p = worktrees; *p; p++) {
		struct worktree *wt = *p;
		struct strbuf ref = STRBUF_INIT;

		strbuf_worktree_ref(wt, &ref, "HEAD");
		fsck_head_link(ref.buf, &head_points_at, &head_oid);
		if (head_points_at && !is_null_oid(&head_oid))
			fsck_handle_ref(ref.buf, &head_oid, 0, NULL);
		strbuf_release(&ref);

		if (include_reflogs)
			refs_for_each_reflog(get_worktree_ref_store(wt),
					     fsck_handle_reflog, wt);
	}
	free_worktrees(worktrees);

	/*
	 * Not having any default heads isn't really fatal, but
	 * it does mean that "--unreachable" no longer makes any
	 * sense (since in this case everything will obviously
	 * be unreachable by definition.
	 *
	 * Showing dangling objects is valid, though (as those
	 * dangling objects are likely lost heads).
	 *
	 * So we just print a warning about it, and clear the
	 * "show_unreachable" flag.
	 */
	if (!default_refs) {
		fprintf_ln(stderr, _("notice: No default references"));
		show_unreachable = 0;
	}
}

static int fsck_loose(const struct object_id *oid, const char *path, void *data)
{
	struct object *obj;
	enum object_type type;
	unsigned long size;
	void *contents;
	int eaten;

	if (read_loose_object(path, oid, &type, &size, &contents) < 0) {
		errors_found |= ERROR_OBJECT;
		error(_("%s: object corrupt or missing: %s"),
		      oid_to_hex(oid), path);
		return 0; /* keep checking other objects */
	}

	if (!contents && type != OBJ_BLOB)
		BUG("read_loose_object streamed a non-blob");

	obj = parse_object_buffer(the_repository, oid, type, size,
				  contents, &eaten);

	if (!obj) {
		errors_found |= ERROR_OBJECT;
		error(_("%s: object could not be parsed: %s"),
		      oid_to_hex(oid), path);
		if (!eaten)
			free(contents);
		return 0; /* keep checking other objects */
	}

	obj->flags &= ~(REACHABLE | SEEN);
	obj->flags |= HAS_OBJ;
	if (fsck_obj(obj, contents, size))
		errors_found |= ERROR_OBJECT;

	if (!eaten)
		free(contents);
	return 0; /* keep checking other objects, even if we saw an error */
}

static int fsck_cruft(const char *basename, const char *path, void *data)
{
	if (!starts_with(basename, "tmp_obj_"))
		fprintf_ln(stderr, _("bad sha1 file: %s"), path);
	return 0;
}

static int fsck_subdir(unsigned int nr, const char *path, void *progress)
{
	display_progress(progress, nr + 1);
	return 0;
}

static void fsck_object_dir(const char *path)
{
	struct progress *progress = NULL;

	if (verbose)
		fprintf_ln(stderr, _("Checking object directory"));

	if (show_progress)
		progress = start_progress(_("Checking object directories"), 256);

	for_each_loose_file_in_objdir(path, fsck_loose, fsck_cruft, fsck_subdir,
				      progress);
	display_progress(progress, 256);
	stop_progress(&progress);
}

static int fsck_head_link(const char *head_ref_name,
			  const char **head_points_at,
			  struct object_id *head_oid)
{
	int null_is_error = 0;

	if (verbose)
		fprintf_ln(stderr, _("Checking %s link"), head_ref_name);

	*head_points_at = resolve_ref_unsafe(head_ref_name, 0, head_oid, NULL);
	if (!*head_points_at) {
		errors_found |= ERROR_REFS;
		return error(_("invalid %s"), head_ref_name);
	}
	if (!strcmp(*head_points_at, head_ref_name))
		/* detached HEAD */
		null_is_error = 1;
	else if (!starts_with(*head_points_at, "refs/heads/")) {
		errors_found |= ERROR_REFS;
		return error(_("%s points to something strange (%s)"),
			     head_ref_name, *head_points_at);
	}
	if (is_null_oid(head_oid)) {
		if (null_is_error) {
			errors_found |= ERROR_REFS;
			return error(_("%s: detached HEAD points at nothing"),
				     head_ref_name);
		}
		fprintf_ln(stderr,
			   _("notice: %s points to an unborn branch (%s)"),
			   head_ref_name, *head_points_at + 11);
	}
	return 0;
}

static int fsck_cache_tree(struct cache_tree *it)
{
	int i;
	int err = 0;

	if (verbose)
		fprintf_ln(stderr, _("Checking cache tree"));

	if (0 <= it->entry_count) {
		struct object *obj = parse_object(the_repository, &it->oid);
		if (!obj) {
			error(_("%s: invalid sha1 pointer in cache-tree"),
			      oid_to_hex(&it->oid));
			errors_found |= ERROR_REFS;
			return 1;
		}
		obj->flags |= USED;
		if (name_objects)
			add_decoration(fsck_walk_options.object_names,
				obj, xstrdup(":"));
		mark_object_reachable(obj);
		if (obj->type != OBJ_TREE)
			err |= objerror(obj, _("non-tree in cache-tree"));
	}
	for (i = 0; i < it->subtree_nr; i++)
		err |= fsck_cache_tree(it->down[i]->cache_tree);
	return err;
}

static void mark_object_for_connectivity(const struct object_id *oid)
{
	struct object *obj = lookup_unknown_object(oid);
	obj->flags |= HAS_OBJ;
}

static int mark_loose_for_connectivity(const struct object_id *oid,
				       const char *path,
				       void *data)
{
	mark_object_for_connectivity(oid);
	return 0;
}

static int mark_packed_for_connectivity(const struct object_id *oid,
					struct packed_git *pack,
					uint32_t pos,
					void *data)
{
	mark_object_for_connectivity(oid);
	return 0;
}

static char const * const fsck_usage[] = {
	N_("git fsck [<options>] [<object>...]"),
	NULL
};

static struct option fsck_opts[] = {
	OPT__VERBOSE(&verbose, N_("be verbose")),
	OPT_BOOL(0, "unreachable", &show_unreachable, N_("show unreachable objects")),
	OPT_BOOL(0, "dangling", &show_dangling, N_("show dangling objects")),
	OPT_BOOL(0, "tags", &show_tags, N_("report tags")),
	OPT_BOOL(0, "root", &show_root, N_("report root nodes")),
	OPT_BOOL(0, "cache", &keep_cache_objects, N_("make index objects head nodes")),
	OPT_BOOL(0, "reflogs", &include_reflogs, N_("make reflogs head nodes (default)")),
	OPT_BOOL(0, "full", &check_full, N_("also consider packs and alternate objects")),
	OPT_BOOL(0, "connectivity-only", &connectivity_only, N_("check only connectivity")),
	OPT_BOOL(0, "strict", &check_strict, N_("enable more strict checking")),
	OPT_BOOL(0, "lost-found", &write_lost_and_found,
				N_("write dangling objects in .git/lost-found")),
	OPT_BOOL(0, "progress", &show_progress, N_("show progress")),
	OPT_BOOL(0, "name-objects", &name_objects, N_("show verbose names for reachable objects")),
	OPT_END(),
};

int cmd_fsck(int argc, const char **argv, const char *prefix)
{
	int i;
	struct object_directory *odb;

	/* fsck knows how to handle missing promisor objects */
	fetch_if_missing = 0;

	errors_found = 0;
	read_replace_refs = 0;

	argc = parse_options(argc, argv, prefix, fsck_opts, fsck_usage, 0);

	fsck_walk_options.walk = mark_object;
	fsck_obj_options.walk = mark_used;
	fsck_obj_options.error_func = fsck_error_func;
	if (check_strict)
		fsck_obj_options.strict = 1;

	if (show_progress == -1)
		show_progress = isatty(2);
	if (verbose)
		show_progress = 0;

	if (write_lost_and_found) {
		check_full = 1;
		include_reflogs = 0;
	}

	if (name_objects)
		fsck_walk_options.object_names =
			xcalloc(1, sizeof(struct decoration));

	git_config(fsck_config, NULL);

	if (connectivity_only) {
		for_each_loose_object(mark_loose_for_connectivity, NULL, 0);
		for_each_packed_object(mark_packed_for_connectivity, NULL, 0);
	} else {
		prepare_alt_odb(the_repository);
		for (odb = the_repository->objects->odb; odb; odb = odb->next)
			fsck_object_dir(odb->path);

		if (check_full) {
			struct packed_git *p;
			uint32_t total = 0, count = 0;
			struct progress *progress = NULL;

			if (show_progress) {
				for (p = get_all_packs(the_repository); p;
				     p = p->next) {
					if (open_pack_index(p))
						continue;
					total += p->num_objects;
				}

				progress = start_progress(_("Checking objects"), total);
			}
			for (p = get_all_packs(the_repository); p;
			     p = p->next) {
				/* verify gives error messages itself */
				if (verify_pack(the_repository,
						p, fsck_obj_buffer,
						progress, count))
					errors_found |= ERROR_PACK;
				count += p->num_objects;
			}
			stop_progress(&progress);
		}

		if (fsck_finish(&fsck_obj_options))
			errors_found |= ERROR_OBJECT;
	}

	for (i = 0; i < argc; i++) {
		const char *arg = argv[i];
		struct object_id oid;
		if (!get_oid(arg, &oid)) {
			struct object *obj = lookup_object(the_repository,
							   &oid);

			if (!obj || !(obj->flags & HAS_OBJ)) {
				if (is_promisor_object(&oid))
					continue;
				error(_("%s: object missing"), oid_to_hex(&oid));
				errors_found |= ERROR_OBJECT;
				continue;
			}

			obj->flags |= USED;
			if (name_objects)
				add_decoration(fsck_walk_options.object_names,
					obj, xstrdup(arg));
			mark_object_reachable(obj);
			continue;
		}
		error(_("invalid parameter: expected sha1, got '%s'"), arg);
		errors_found |= ERROR_OBJECT;
	}

	/*
	 * If we've not been given any explicit head information, do the
	 * default ones from .git/refs. We also consider the index file
	 * in this case (ie this implies --cache).
	 */
	if (!argc) {
		get_default_heads();
		keep_cache_objects = 1;
	}

	if (keep_cache_objects) {
		verify_index_checksum = 1;
		verify_ce_order = 1;
		read_cache();
		for (i = 0; i < active_nr; i++) {
			unsigned int mode;
			struct blob *blob;
			struct object *obj;

			mode = active_cache[i]->ce_mode;
			if (S_ISGITLINK(mode))
				continue;
			blob = lookup_blob(the_repository,
					   &active_cache[i]->oid);
			if (!blob)
				continue;
			obj = &blob->object;
			obj->flags |= USED;
			if (name_objects)
				add_decoration(fsck_walk_options.object_names,
					obj,
					xstrfmt(":%s", active_cache[i]->name));
			mark_object_reachable(obj);
		}
		if (active_cache_tree)
			fsck_cache_tree(active_cache_tree);
	}

	check_connectivity();

	if (!git_config_get_bool("core.commitgraph", &i) && i) {
		struct child_process commit_graph_verify = CHILD_PROCESS_INIT;
		const char *verify_argv[] = { "commit-graph", "verify", NULL, NULL, NULL };

		prepare_alt_odb(the_repository);
		for (odb = the_repository->objects->odb; odb; odb = odb->next) {
			child_process_init(&commit_graph_verify);
			commit_graph_verify.argv = verify_argv;
			commit_graph_verify.git_cmd = 1;
			verify_argv[2] = "--object-dir";
			verify_argv[3] = odb->path;
			if (run_command(&commit_graph_verify))
				errors_found |= ERROR_COMMIT_GRAPH;
		}
	}

	if (!git_config_get_bool("core.multipackindex", &i) && i) {
		struct child_process midx_verify = CHILD_PROCESS_INIT;
		const char *midx_argv[] = { "multi-pack-index", "verify", NULL, NULL, NULL };

		prepare_alt_odb(the_repository);
		for (odb = the_repository->objects->odb; odb; odb = odb->next) {
			child_process_init(&midx_verify);
			midx_verify.argv = midx_argv;
			midx_verify.git_cmd = 1;
			midx_argv[2] = "--object-dir";
			midx_argv[3] = odb->path;
			if (run_command(&midx_verify))
				errors_found |= ERROR_COMMIT_GRAPH;
		}
	}

	return errors_found;
}
