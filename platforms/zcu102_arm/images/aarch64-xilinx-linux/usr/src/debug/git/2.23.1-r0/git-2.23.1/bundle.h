#ifndef BUNDLE_H
#define BUNDLE_H

#include "cache.h"

struct ref_list {
	unsigned int nr, alloc;
	struct ref_list_entry {
		struct object_id oid;
		char *name;
	} *list;
};

struct bundle_header {
	struct ref_list prerequisites;
	struct ref_list references;
};

int is_bundle(const char *path, int quiet);
int read_bundle_header(const char *path, struct bundle_header *header);
int create_bundle(struct repository *r, const char *path,
		  int argc, const char **argv);
int verify_bundle(struct repository *r, struct bundle_header *header, int verbose);
#define BUNDLE_VERBOSE 1
int unbundle(struct repository *r, struct bundle_header *header,
	     int bundle_fd, int flags);
int list_bundle_refs(struct bundle_header *header,
		int argc, const char **argv);

#endif
