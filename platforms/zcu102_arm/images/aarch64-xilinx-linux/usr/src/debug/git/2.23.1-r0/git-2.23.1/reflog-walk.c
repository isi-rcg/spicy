#include "cache.h"
#include "commit.h"
#include "refs.h"
#include "diff.h"
#include "revision.h"
#include "string-list.h"
#include "reflog-walk.h"

struct complete_reflogs {
	char *ref;
	const char *short_ref;
	struct reflog_info {
		struct object_id ooid, noid;
		char *email;
		timestamp_t timestamp;
		int tz;
		char *message;
	} *items;
	int nr, alloc;
};

static int read_one_reflog(struct object_id *ooid, struct object_id *noid,
		const char *email, timestamp_t timestamp, int tz,
		const char *message, void *cb_data)
{
	struct complete_reflogs *array = cb_data;
	struct reflog_info *item;

	ALLOC_GROW(array->items, array->nr + 1, array->alloc);
	item = array->items + array->nr;
	oidcpy(&item->ooid, ooid);
	oidcpy(&item->noid, noid);
	item->email = xstrdup(email);
	item->timestamp = timestamp;
	item->tz = tz;
	item->message = xstrdup(message);
	array->nr++;
	return 0;
}

static void free_complete_reflog(struct complete_reflogs *array)
{
	int i;

	if (!array)
		return;

	for (i = 0; i < array->nr; i++) {
		free(array->items[i].email);
		free(array->items[i].message);
	}
	free(array->items);
	free(array->ref);
	free(array);
}

static struct complete_reflogs *read_complete_reflog(const char *ref)
{
	struct complete_reflogs *reflogs =
		xcalloc(1, sizeof(struct complete_reflogs));
	reflogs->ref = xstrdup(ref);
	for_each_reflog_ent(ref, read_one_reflog, reflogs);
	if (reflogs->nr == 0) {
		const char *name;
		void *name_to_free;
		name = name_to_free = resolve_refdup(ref, RESOLVE_REF_READING,
						     NULL, NULL);
		if (name) {
			for_each_reflog_ent(name, read_one_reflog, reflogs);
			free(name_to_free);
		}
	}
	if (reflogs->nr == 0) {
		char *refname = xstrfmt("refs/%s", ref);
		for_each_reflog_ent(refname, read_one_reflog, reflogs);
		if (reflogs->nr == 0) {
			free(refname);
			refname = xstrfmt("refs/heads/%s", ref);
			for_each_reflog_ent(refname, read_one_reflog, reflogs);
		}
		free(refname);
	}
	return reflogs;
}

static int get_reflog_recno_by_time(struct complete_reflogs *array,
	timestamp_t timestamp)
{
	int i;
	for (i = array->nr - 1; i >= 0; i--)
		if (timestamp >= array->items[i].timestamp)
			return i;
	return -1;
}

struct commit_reflog {
	int recno;
	enum selector_type {
		SELECTOR_NONE,
		SELECTOR_INDEX,
		SELECTOR_DATE
	} selector;
	struct complete_reflogs *reflogs;
};

struct reflog_walk_info {
	struct commit_reflog **logs;
	size_t nr, alloc;
	struct string_list complete_reflogs;
	struct commit_reflog *last_commit_reflog;
};

void init_reflog_walk(struct reflog_walk_info **info)
{
	*info = xcalloc(1, sizeof(struct reflog_walk_info));
	(*info)->complete_reflogs.strdup_strings = 1;
}

int add_reflog_for_walk(struct reflog_walk_info *info,
		struct commit *commit, const char *name)
{
	timestamp_t timestamp = 0;
	int recno = -1;
	struct string_list_item *item;
	struct complete_reflogs *reflogs;
	char *branch, *at = strchr(name, '@');
	struct commit_reflog *commit_reflog;
	enum selector_type selector = SELECTOR_NONE;

	if (commit->object.flags & UNINTERESTING)
		die("cannot walk reflogs for %s", name);

	branch = xstrdup(name);
	if (at && at[1] == '{') {
		char *ep;
		branch[at - name] = '\0';
		recno = strtoul(at + 2, &ep, 10);
		if (*ep != '}') {
			recno = -1;
			timestamp = approxidate(at + 2);
			selector = SELECTOR_DATE;
		}
		else
			selector = SELECTOR_INDEX;
	} else
		recno = 0;

	item = string_list_lookup(&info->complete_reflogs, branch);
	if (item)
		reflogs = item->util;
	else {
		if (*branch == '\0') {
			free(branch);
			branch = resolve_refdup("HEAD", 0, NULL, NULL);
			if (!branch)
				die("no current branch");

		}
		reflogs = read_complete_reflog(branch);
		if (!reflogs || reflogs->nr == 0) {
			struct object_id oid;
			char *b;
			int ret = dwim_log(branch, strlen(branch),
					   &oid, &b);
			if (ret > 1)
				free(b);
			else if (ret == 1) {
				free_complete_reflog(reflogs);
				free(branch);
				branch = b;
				reflogs = read_complete_reflog(branch);
			}
		}
		if (!reflogs || reflogs->nr == 0) {
			free_complete_reflog(reflogs);
			free(branch);
			return -1;
		}
		string_list_insert(&info->complete_reflogs, branch)->util
			= reflogs;
	}
	free(branch);

	commit_reflog = xcalloc(1, sizeof(struct commit_reflog));
	if (recno < 0) {
		commit_reflog->recno = get_reflog_recno_by_time(reflogs, timestamp);
		if (commit_reflog->recno < 0) {
			free(commit_reflog);
			return -1;
		}
	} else
		commit_reflog->recno = reflogs->nr - recno - 1;
	commit_reflog->selector = selector;
	commit_reflog->reflogs = reflogs;

	ALLOC_GROW(info->logs, info->nr + 1, info->alloc);
	info->logs[info->nr++] = commit_reflog;

	return 0;
}

void get_reflog_selector(struct strbuf *sb,
			 struct reflog_walk_info *reflog_info,
			 const struct date_mode *dmode, int force_date,
			 int shorten)
{
	struct commit_reflog *commit_reflog = reflog_info->last_commit_reflog;
	struct reflog_info *info;
	const char *printed_ref;

	if (!commit_reflog)
		return;

	if (shorten) {
		if (!commit_reflog->reflogs->short_ref)
			commit_reflog->reflogs->short_ref
				= shorten_unambiguous_ref(commit_reflog->reflogs->ref, 0);
		printed_ref = commit_reflog->reflogs->short_ref;
	} else {
		printed_ref = commit_reflog->reflogs->ref;
	}

	strbuf_addf(sb, "%s@{", printed_ref);
	if (commit_reflog->selector == SELECTOR_DATE ||
	    (commit_reflog->selector == SELECTOR_NONE && force_date)) {
		info = &commit_reflog->reflogs->items[commit_reflog->recno+1];
		strbuf_addstr(sb, show_date(info->timestamp, info->tz, dmode));
	} else {
		strbuf_addf(sb, "%d", commit_reflog->reflogs->nr
			    - 2 - commit_reflog->recno);
	}

	strbuf_addch(sb, '}');
}

void get_reflog_message(struct strbuf *sb,
			struct reflog_walk_info *reflog_info)
{
	struct commit_reflog *commit_reflog = reflog_info->last_commit_reflog;
	struct reflog_info *info;
	size_t len;

	if (!commit_reflog)
		return;

	info = &commit_reflog->reflogs->items[commit_reflog->recno+1];
	len = strlen(info->message);
	if (len > 0)
		len--; /* strip away trailing newline */
	strbuf_add(sb, info->message, len);
}

const char *get_reflog_ident(struct reflog_walk_info *reflog_info)
{
	struct commit_reflog *commit_reflog = reflog_info->last_commit_reflog;
	struct reflog_info *info;

	if (!commit_reflog)
		return NULL;

	info = &commit_reflog->reflogs->items[commit_reflog->recno+1];
	return info->email;
}

timestamp_t get_reflog_timestamp(struct reflog_walk_info *reflog_info)
{
	struct commit_reflog *commit_reflog = reflog_info->last_commit_reflog;
	struct reflog_info *info;

	if (!commit_reflog)
		return 0;

	info = &commit_reflog->reflogs->items[commit_reflog->recno+1];
	return info->timestamp;
}

void show_reflog_message(struct reflog_walk_info *reflog_info, int oneline,
			 const struct date_mode *dmode, int force_date)
{
	if (reflog_info && reflog_info->last_commit_reflog) {
		struct commit_reflog *commit_reflog = reflog_info->last_commit_reflog;
		struct reflog_info *info;
		struct strbuf selector = STRBUF_INIT;

		info = &commit_reflog->reflogs->items[commit_reflog->recno+1];
		get_reflog_selector(&selector, reflog_info, dmode, force_date, 0);
		if (oneline) {
			printf("%s: %s", selector.buf, info->message);
		}
		else {
			printf("Reflog: %s (%s)\nReflog message: %s",
			       selector.buf, info->email, info->message);
		}

		strbuf_release(&selector);
	}
}

int reflog_walk_empty(struct reflog_walk_info *info)
{
	return !info || !info->nr;
}

static struct commit *next_reflog_commit(struct commit_reflog *log)
{
	for (; log->recno >= 0; log->recno--) {
		struct reflog_info *entry = &log->reflogs->items[log->recno];
		struct object *obj = parse_object(the_repository,
						  &entry->noid);

		if (obj && obj->type == OBJ_COMMIT)
			return (struct commit *)obj;
	}
	return NULL;
}

static timestamp_t log_timestamp(struct commit_reflog *log)
{
	return log->reflogs->items[log->recno].timestamp;
}

struct commit *next_reflog_entry(struct reflog_walk_info *walk)
{
	struct commit_reflog *best = NULL;
	struct commit *best_commit = NULL;
	size_t i;

	for (i = 0; i < walk->nr; i++) {
		struct commit_reflog *log = walk->logs[i];
		struct commit *commit = next_reflog_commit(log);

		if (!commit)
			continue;

		if (!best || log_timestamp(log) > log_timestamp(best)) {
			best = log;
			best_commit = commit;
		}
	}

	if (best) {
		best->recno--;
		walk->last_commit_reflog = best;
		return best_commit;
	}

	return NULL;
}
