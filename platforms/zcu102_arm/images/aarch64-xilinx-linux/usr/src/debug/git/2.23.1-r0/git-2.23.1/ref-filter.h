#ifndef REF_FILTER_H
#define REF_FILTER_H

#include "sha1-array.h"
#include "refs.h"
#include "commit.h"
#include "parse-options.h"

/* Quoting styles */
#define QUOTE_NONE 0
#define QUOTE_SHELL 1
#define QUOTE_PERL 2
#define QUOTE_PYTHON 4
#define QUOTE_TCL 8

#define FILTER_REFS_INCLUDE_BROKEN 0x0001
#define FILTER_REFS_TAGS           0x0002
#define FILTER_REFS_BRANCHES       0x0004
#define FILTER_REFS_REMOTES        0x0008
#define FILTER_REFS_OTHERS         0x0010
#define FILTER_REFS_ALL            (FILTER_REFS_TAGS | FILTER_REFS_BRANCHES | \
				    FILTER_REFS_REMOTES | FILTER_REFS_OTHERS)
#define FILTER_REFS_DETACHED_HEAD  0x0020
#define FILTER_REFS_KIND_MASK      (FILTER_REFS_ALL | FILTER_REFS_DETACHED_HEAD)

struct atom_value;

struct ref_sorting {
	struct ref_sorting *next;
	int atom; /* index into used_atom array (internal) */
	unsigned reverse : 1,
		ignore_case : 1,
		version : 1;
};

struct ref_array_item {
	struct object_id objectname;
	int flag;
	unsigned int kind;
	const char *symref;
	struct commit *commit;
	struct atom_value *value;
	char refname[FLEX_ARRAY];
};

struct ref_array {
	int nr, alloc;
	struct ref_array_item **items;
	struct rev_info *revs;
};

struct ref_filter {
	const char **name_patterns;
	struct oid_array points_at;
	struct commit_list *with_commit;
	struct commit_list *no_commit;

	enum {
		REF_FILTER_MERGED_NONE = 0,
		REF_FILTER_MERGED_INCLUDE,
		REF_FILTER_MERGED_OMIT
	} merge;
	struct commit *merge_commit;

	unsigned int with_commit_tag_algo : 1,
		match_as_path : 1,
		ignore_case : 1,
		detached : 1;
	unsigned int kind,
		lines;
	int abbrev,
		verbose;
};

struct ref_format {
	/*
	 * Set these to define the format; make sure you call
	 * verify_ref_format() afterwards to finalize.
	 */
	const char *format;
	int quote_style;
	int use_color;

	/* Internal state to ref-filter */
	int need_color_reset_at_eol;
};

#define REF_FORMAT_INIT { NULL, 0, -1 }

/*  Macros for checking --merged and --no-merged options */
#define _OPT_MERGED_NO_MERGED(option, filter, h) \
	{ OPTION_CALLBACK, 0, option, (filter), N_("commit"), (h), \
	  PARSE_OPT_LASTARG_DEFAULT | PARSE_OPT_NONEG, \
	  parse_opt_merge_filter, (intptr_t) "HEAD" \
	}
#define OPT_MERGED(f, h) _OPT_MERGED_NO_MERGED("merged", f, h)
#define OPT_NO_MERGED(f, h) _OPT_MERGED_NO_MERGED("no-merged", f, h)

#define OPT_REF_SORT(var) \
	OPT_CALLBACK_F(0, "sort", (var), \
		       N_("key"), N_("field name to sort on"), \
		       PARSE_OPT_NONEG, parse_opt_ref_sorting)

/*
 * API for filtering a set of refs. Based on the type of refs the user
 * has requested, we iterate through those refs and apply filters
 * as per the given ref_filter structure and finally store the
 * filtered refs in the ref_array structure.
 */
int filter_refs(struct ref_array *array, struct ref_filter *filter, unsigned int type);
/*  Clear all memory allocated to ref_array */
void ref_array_clear(struct ref_array *array);
/*  Used to verify if the given format is correct and to parse out the used atoms */
int verify_ref_format(struct ref_format *format);
/*  Sort the given ref_array as per the ref_sorting provided */
void ref_array_sort(struct ref_sorting *sort, struct ref_array *array);
/*  Based on the given format and quote_style, fill the strbuf */
int format_ref_array_item(struct ref_array_item *info,
			  const struct ref_format *format,
			  struct strbuf *final_buf,
			  struct strbuf *error_buf);
/*  Print the ref using the given format and quote_style */
void show_ref_array_item(struct ref_array_item *info, const struct ref_format *format);
/*  Parse a single sort specifier and add it to the list */
void parse_ref_sorting(struct ref_sorting **sorting_tail, const char *atom);
/*  Callback function for parsing the sort option */
int parse_opt_ref_sorting(const struct option *opt, const char *arg, int unset);
/*  Default sort option based on refname */
struct ref_sorting *ref_default_sorting(void);
/*  Function to parse --merged and --no-merged options */
int parse_opt_merge_filter(const struct option *opt, const char *arg, int unset);
/*  Get the current HEAD's description */
char *get_head_description(void);
/*  Set up translated strings in the output. */
void setup_ref_filter_porcelain_msg(void);

/*
 * Print a single ref, outside of any ref-filter. Note that the
 * name must be a fully qualified refname.
 */
void pretty_print_ref(const char *name, const struct object_id *oid,
		      const struct ref_format *format);

/*
 * Push a single ref onto the array; this can be used to construct your own
 * ref_array without using filter_refs().
 */
struct ref_array_item *ref_array_push(struct ref_array *array,
				      const char *refname,
				      const struct object_id *oid);

#endif /*  REF_FILTER_H  */
