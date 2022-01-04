#include "cache.h"
#include "cache-tree.h"
#include "tree.h"
#include "object-store.h"
#include "blob.h"
#include "commit.h"
#include "tag.h"
#include "alloc.h"
#include "tree-walk.h"
#include "repository.h"

const char *tree_type = "tree";

static int read_one_entry_opt(struct index_state *istate,
			      const struct object_id *oid,
			      const char *base, int baselen,
			      const char *pathname,
			      unsigned mode, int stage, int opt)
{
	int len;
	struct cache_entry *ce;

	if (S_ISDIR(mode))
		return READ_TREE_RECURSIVE;

	len = strlen(pathname);
	ce = make_empty_cache_entry(istate, baselen + len);

	ce->ce_mode = create_ce_mode(mode);
	ce->ce_flags = create_ce_flags(stage);
	ce->ce_namelen = baselen + len;
	memcpy(ce->name, base, baselen);
	memcpy(ce->name + baselen, pathname, len+1);
	oidcpy(&ce->oid, oid);
	return add_index_entry(istate, ce, opt);
}

static int read_one_entry(const struct object_id *oid, struct strbuf *base,
			  const char *pathname, unsigned mode, int stage,
			  void *context)
{
	struct index_state *istate = context;
	return read_one_entry_opt(istate, oid, base->buf, base->len, pathname,
				  mode, stage,
				  ADD_CACHE_OK_TO_ADD|ADD_CACHE_SKIP_DFCHECK);
}

/*
 * This is used when the caller knows there is no existing entries at
 * the stage that will conflict with the entry being added.
 */
static int read_one_entry_quick(const struct object_id *oid, struct strbuf *base,
				const char *pathname, unsigned mode, int stage,
				void *context)
{
	struct index_state *istate = context;
	return read_one_entry_opt(istate, oid, base->buf, base->len, pathname,
				  mode, stage,
				  ADD_CACHE_JUST_APPEND);
}

static int read_tree_1(struct repository *r,
		       struct tree *tree, struct strbuf *base,
		       int stage, const struct pathspec *pathspec,
		       read_tree_fn_t fn, void *context)
{
	struct tree_desc desc;
	struct name_entry entry;
	struct object_id oid;
	int len, oldlen = base->len;
	enum interesting retval = entry_not_interesting;

	if (parse_tree(tree))
		return -1;

	init_tree_desc(&desc, tree->buffer, tree->size);

	while (tree_entry(&desc, &entry)) {
		if (retval != all_entries_interesting) {
			retval = tree_entry_interesting(r->index, &entry,
							base, 0, pathspec);
			if (retval == all_entries_not_interesting)
				break;
			if (retval == entry_not_interesting)
				continue;
		}

		switch (fn(&entry.oid, base,
			   entry.path, entry.mode, stage, context)) {
		case 0:
			continue;
		case READ_TREE_RECURSIVE:
			break;
		default:
			return -1;
		}

		if (S_ISDIR(entry.mode))
			oidcpy(&oid, &entry.oid);
		else if (S_ISGITLINK(entry.mode)) {
			struct commit *commit;

			commit = lookup_commit(r, &entry.oid);
			if (!commit)
				die("Commit %s in submodule path %s%s not found",
				    oid_to_hex(&entry.oid),
				    base->buf, entry.path);

			if (parse_commit(commit))
				die("Invalid commit %s in submodule path %s%s",
				    oid_to_hex(&entry.oid),
				    base->buf, entry.path);

			oidcpy(&oid, get_commit_tree_oid(commit));
		}
		else
			continue;

		len = tree_entry_len(&entry);
		strbuf_add(base, entry.path, len);
		strbuf_addch(base, '/');
		retval = read_tree_1(r, lookup_tree(r, &oid),
				     base, stage, pathspec,
				     fn, context);
		strbuf_setlen(base, oldlen);
		if (retval)
			return -1;
	}
	return 0;
}

int read_tree_recursive(struct repository *r,
			struct tree *tree,
			const char *base, int baselen,
			int stage, const struct pathspec *pathspec,
			read_tree_fn_t fn, void *context)
{
	struct strbuf sb = STRBUF_INIT;
	int ret;

	strbuf_add(&sb, base, baselen);
	ret = read_tree_1(r, tree, &sb, stage, pathspec, fn, context);
	strbuf_release(&sb);
	return ret;
}

static int cmp_cache_name_compare(const void *a_, const void *b_)
{
	const struct cache_entry *ce1, *ce2;

	ce1 = *((const struct cache_entry **)a_);
	ce2 = *((const struct cache_entry **)b_);
	return cache_name_stage_compare(ce1->name, ce1->ce_namelen, ce_stage(ce1),
				  ce2->name, ce2->ce_namelen, ce_stage(ce2));
}

int read_tree(struct repository *r, struct tree *tree, int stage,
	      struct pathspec *match, struct index_state *istate)
{
	read_tree_fn_t fn = NULL;
	int i, err;

	/*
	 * Currently the only existing callers of this function all
	 * call it with stage=1 and after making sure there is nothing
	 * at that stage; we could always use read_one_entry_quick().
	 *
	 * But when we decide to straighten out git-read-tree not to
	 * use unpack_trees() in some cases, this will probably start
	 * to matter.
	 */

	/*
	 * See if we have cache entry at the stage.  If so,
	 * do it the original slow way, otherwise, append and then
	 * sort at the end.
	 */
	for (i = 0; !fn && i < istate->cache_nr; i++) {
		const struct cache_entry *ce = istate->cache[i];
		if (ce_stage(ce) == stage)
			fn = read_one_entry;
	}

	if (!fn)
		fn = read_one_entry_quick;
	err = read_tree_recursive(r, tree, "", 0, stage, match, fn, istate);
	if (fn == read_one_entry || err)
		return err;

	/*
	 * Sort the cache entry -- we need to nuke the cache tree, though.
	 */
	cache_tree_free(&istate->cache_tree);
	QSORT(istate->cache, istate->cache_nr, cmp_cache_name_compare);
	return 0;
}

struct tree *lookup_tree(struct repository *r, const struct object_id *oid)
{
	struct object *obj = lookup_object(r, oid);
	if (!obj)
		return create_object(r, oid, alloc_tree_node(r));
	return object_as_type(r, obj, OBJ_TREE, 0);
}

int parse_tree_buffer(struct tree *item, void *buffer, unsigned long size)
{
	if (item->object.parsed)
		return 0;
	item->object.parsed = 1;
	item->buffer = buffer;
	item->size = size;

	return 0;
}

int parse_tree_gently(struct tree *item, int quiet_on_missing)
{
	 enum object_type type;
	 void *buffer;
	 unsigned long size;

	if (item->object.parsed)
		return 0;
	buffer = read_object_file(&item->object.oid, &type, &size);
	if (!buffer)
		return quiet_on_missing ? -1 :
			error("Could not read %s",
			     oid_to_hex(&item->object.oid));
	if (type != OBJ_TREE) {
		free(buffer);
		return error("Object %s not a tree",
			     oid_to_hex(&item->object.oid));
	}
	return parse_tree_buffer(item, buffer, size);
}

void free_tree_buffer(struct tree *tree)
{
	FREE_AND_NULL(tree->buffer);
	tree->size = 0;
	tree->object.parsed = 0;
}

struct tree *parse_tree_indirect(const struct object_id *oid)
{
	struct object *obj = parse_object(the_repository, oid);
	do {
		if (!obj)
			return NULL;
		if (obj->type == OBJ_TREE)
			return (struct tree *) obj;
		else if (obj->type == OBJ_COMMIT)
			obj = &(get_commit_tree(((struct commit *)obj))->object);
		else if (obj->type == OBJ_TAG)
			obj = ((struct tag *) obj)->tagged;
		else
			return NULL;
		if (!obj->parsed)
			parse_object(the_repository, &obj->oid);
	} while (1);
}
