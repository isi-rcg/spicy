#ifndef OBJECT_H
#define OBJECT_H

#include "cache.h"

struct buffer_slab;

struct parsed_object_pool {
	struct object **obj_hash;
	int nr_objs, obj_hash_size;

	/* TODO: migrate alloc_states to mem-pool? */
	struct alloc_state *blob_state;
	struct alloc_state *tree_state;
	struct alloc_state *commit_state;
	struct alloc_state *tag_state;
	struct alloc_state *object_state;
	unsigned commit_count;

	/* parent substitutions from .git/info/grafts and .git/shallow */
	struct commit_graft **grafts;
	int grafts_alloc, grafts_nr;

	int is_shallow;
	struct stat_validity *shallow_stat;
	char *alternate_shallow_file;

	int commit_graft_prepared;

	struct buffer_slab *buffer_slab;
};

struct parsed_object_pool *parsed_object_pool_new(void);
void parsed_object_pool_clear(struct parsed_object_pool *o);

struct object_list {
	struct object *item;
	struct object_list *next;
};

struct object_array {
	unsigned int nr;
	unsigned int alloc;
	struct object_array_entry {
		struct object *item;
		/*
		 * name or NULL.  If non-NULL, the memory pointed to
		 * is owned by this object *except* if it points at
		 * object_array_slopbuf, which is a static copy of the
		 * empty string.
		 */
		char *name;
		char *path;
		unsigned mode;
	} *objects;
};

#define OBJECT_ARRAY_INIT { 0, 0, NULL }

/*
 * object flag allocation:
 * revision.h:               0---------10                              25----28
 * fetch-pack.c:             01
 * negotiator/default.c:       2--5
 * walker.c:                 0-2
 * upload-pack.c:                4       11-----14  16-----19
 * builtin/blame.c:                        12-13
 * bisect.c:                                        16
 * bundle.c:                                        16
 * http-push.c:                                     16-----19
 * commit-reach.c:                                15-------19
 * sha1-name.c:                                              20
 * list-objects-filter.c:                                      21
 * builtin/fsck.c:           0--3
 * builtin/index-pack.c:                                     2021
 * builtin/pack-objects.c:                                   20
 * builtin/reflog.c:                   10--12
 * builtin/show-branch.c:    0-------------------------------------------26
 * builtin/unpack-objects.c:                                 2021
 */
#define FLAG_BITS  29

/*
 * The object type is stored in 3 bits.
 */
struct object {
	unsigned parsed : 1;
	unsigned type : TYPE_BITS;
	unsigned flags : FLAG_BITS;
	struct object_id oid;
};

const char *type_name(unsigned int type);
int type_from_string_gently(const char *str, ssize_t, int gentle);
#define type_from_string(str) type_from_string_gently(str, -1, 0)

/*
 * Return the current number of buckets in the object hashmap.
 */
unsigned int get_max_object_index(void);

/*
 * Return the object from the specified bucket in the object hashmap.
 */
struct object *get_indexed_object(unsigned int);

/*
 * This can be used to see if we have heard of the object before, but
 * it can return "yes we have, and here is a half-initialised object"
 * for an object that we haven't loaded/parsed yet.
 *
 * When parsing a commit to create an in-core commit object, its
 * parents list holds commit objects that represent its parents, but
 * they are expected to be lazily initialized and do not know what
 * their trees or parents are yet.  When this function returns such a
 * half-initialised objects, the caller is expected to initialize them
 * by calling parse_object() on them.
 */
struct object *lookup_object(struct repository *r, const struct object_id *oid);

void *create_object(struct repository *r, const struct object_id *oid, void *obj);

void *object_as_type(struct repository *r, struct object *obj, enum object_type type, int quiet);

/*
 * Returns the object, having parsed it to find out what it is.
 *
 * Returns NULL if the object is missing or corrupt.
 */
struct object *parse_object(struct repository *r, const struct object_id *oid);

/*
 * Like parse_object, but will die() instead of returning NULL. If the
 * "name" parameter is not NULL, it is included in the error message
 * (otherwise, the hex object ID is given).
 */
struct object *parse_object_or_die(const struct object_id *oid, const char *name);

/* Given the result of read_sha1_file(), returns the object after
 * parsing it.  eaten_p indicates if the object has a borrowed copy
 * of buffer and the caller should not free() it.
 */
struct object *parse_object_buffer(struct repository *r, const struct object_id *oid, enum object_type type, unsigned long size, void *buffer, int *eaten_p);

/** Returns the object, with potentially excess memory allocated. **/
struct object *lookup_unknown_object(const struct object_id *oid);

struct object_list *object_list_insert(struct object *item,
				       struct object_list **list_p);

int object_list_contains(struct object_list *list, struct object *obj);

/* Object array handling .. */
void add_object_array(struct object *obj, const char *name, struct object_array *array);
void add_object_array_with_path(struct object *obj, const char *name, struct object_array *array, unsigned mode, const char *path);

/*
 * Returns NULL if the array is empty. Otherwise, returns the last object
 * after removing its entry from the array. Other resources associated
 * with that object are left in an unspecified state and should not be
 * examined.
 */
struct object *object_array_pop(struct object_array *array);

typedef int (*object_array_each_func_t)(struct object_array_entry *, void *);

/*
 * Apply want to each entry in array, retaining only the entries for
 * which the function returns true.  Preserve the order of the entries
 * that are retained.
 */
void object_array_filter(struct object_array *array,
			 object_array_each_func_t want, void *cb_data);

/*
 * Remove from array all but the first entry with a given name.
 * Warning: this function uses an O(N^2) algorithm.
 */
void object_array_remove_duplicates(struct object_array *array);

/*
 * Remove any objects from the array, freeing all used memory; afterwards
 * the array is ready to store more objects with add_object_array().
 */
void object_array_clear(struct object_array *array);

void clear_object_flags(unsigned flags);

/*
 * Clear the specified object flags from all in-core commit objects.
 */
void clear_commit_marks_all(unsigned int flags);

#endif /* OBJECT_H */
