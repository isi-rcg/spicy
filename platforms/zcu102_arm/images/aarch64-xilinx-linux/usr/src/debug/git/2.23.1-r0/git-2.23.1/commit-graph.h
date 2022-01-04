#ifndef COMMIT_GRAPH_H
#define COMMIT_GRAPH_H

#include "git-compat-util.h"
#include "repository.h"
#include "string-list.h"
#include "cache.h"

#define GIT_TEST_COMMIT_GRAPH "GIT_TEST_COMMIT_GRAPH"
#define GIT_TEST_COMMIT_GRAPH_DIE_ON_LOAD "GIT_TEST_COMMIT_GRAPH_DIE_ON_LOAD"

struct commit;

char *get_commit_graph_filename(const char *obj_dir);
int open_commit_graph(const char *graph_file, int *fd, struct stat *st);

/*
 * Given a commit struct, try to fill the commit struct info, including:
 *  1. tree object
 *  2. date
 *  3. parents.
 *
 * Returns 1 if and only if the commit was found in the packed graph.
 *
 * See parse_commit_buffer() for the fallback after this call.
 */
int parse_commit_in_graph(struct repository *r, struct commit *item);

/*
 * It is possible that we loaded commit contents from the commit buffer,
 * but we also want to ensure the commit-graph content is correctly
 * checked and filled. Fill the graph_pos and generation members of
 * the given commit.
 */
void load_commit_graph_info(struct repository *r, struct commit *item);

struct tree *get_commit_tree_in_graph(struct repository *r,
				      const struct commit *c);

struct commit_graph {
	int graph_fd;

	const unsigned char *data;
	size_t data_len;

	unsigned char hash_len;
	unsigned char num_chunks;
	uint32_t num_commits;
	struct object_id oid;
	char *filename;
	const char *obj_dir;

	uint32_t num_commits_in_base;
	struct commit_graph *base_graph;

	const uint32_t *chunk_oid_fanout;
	const unsigned char *chunk_oid_lookup;
	const unsigned char *chunk_commit_data;
	const unsigned char *chunk_extra_edges;
	const unsigned char *chunk_base_graphs;
};

struct commit_graph *load_commit_graph_one_fd_st(int fd, struct stat *st);
struct commit_graph *read_commit_graph_one(struct repository *r, const char *obj_dir);
struct commit_graph *parse_commit_graph(void *graph_map, int fd,
					size_t graph_size);

/*
 * Return 1 if and only if the repository has a commit-graph
 * file and generation numbers are computed in that file.
 */
int generation_numbers_enabled(struct repository *r);

#define COMMIT_GRAPH_APPEND     (1 << 0)
#define COMMIT_GRAPH_PROGRESS   (1 << 1)
#define COMMIT_GRAPH_SPLIT      (1 << 2)

struct split_commit_graph_opts {
	int size_multiple;
	int max_commits;
	timestamp_t expire_time;
};

/*
 * The write_commit_graph* methods return zero on success
 * and a negative value on failure. Note that if the repository
 * is not compatible with the commit-graph feature, then the
 * methods will return 0 without writing a commit-graph.
 */
int write_commit_graph_reachable(const char *obj_dir, unsigned int flags,
				 const struct split_commit_graph_opts *split_opts);
int write_commit_graph(const char *obj_dir,
		       struct string_list *pack_indexes,
		       struct string_list *commit_hex,
		       unsigned int flags,
		       const struct split_commit_graph_opts *split_opts);

#define COMMIT_GRAPH_VERIFY_SHALLOW	(1 << 0)

int verify_commit_graph(struct repository *r, struct commit_graph *g, int flags);

void close_commit_graph(struct raw_object_store *);
void free_commit_graph(struct commit_graph *);

#endif
