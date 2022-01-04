/*
 * Copyright (C) 2005 Junio C Hamano
 * Copyright (C) 2010 Google Inc.
 */
#include "cache.h"
#include "diff.h"
#include "diffcore.h"
#include "xdiff-interface.h"
#include "kwset.h"
#include "commit.h"
#include "quote.h"

typedef int (*pickaxe_fn)(mmfile_t *one, mmfile_t *two,
			  struct diff_options *o,
			  regex_t *regexp, kwset_t kws);

struct diffgrep_cb {
	regex_t *regexp;
	int hit;
};

static void diffgrep_consume(void *priv, char *line, unsigned long len)
{
	struct diffgrep_cb *data = priv;
	regmatch_t regmatch;

	if (line[0] != '+' && line[0] != '-')
		return;
	if (data->hit)
		/*
		 * NEEDSWORK: we should have a way to terminate the
		 * caller early.
		 */
		return;
	data->hit = !regexec_buf(data->regexp, line + 1, len - 1, 1,
				 &regmatch, 0);
}

static int diff_grep(mmfile_t *one, mmfile_t *two,
		     struct diff_options *o,
		     regex_t *regexp, kwset_t kws)
{
	regmatch_t regmatch;
	struct diffgrep_cb ecbdata;
	xpparam_t xpp;
	xdemitconf_t xecfg;

	if (!one)
		return !regexec_buf(regexp, two->ptr, two->size,
				    1, &regmatch, 0);
	if (!two)
		return !regexec_buf(regexp, one->ptr, one->size,
				    1, &regmatch, 0);

	/*
	 * We have both sides; need to run textual diff and see if
	 * the pattern appears on added/deleted lines.
	 */
	memset(&xpp, 0, sizeof(xpp));
	memset(&xecfg, 0, sizeof(xecfg));
	ecbdata.regexp = regexp;
	ecbdata.hit = 0;
	xecfg.ctxlen = o->context;
	xecfg.interhunkctxlen = o->interhunkcontext;
	if (xdi_diff_outf(one, two, discard_hunk_line, diffgrep_consume,
			  &ecbdata, &xpp, &xecfg))
		return 0;
	return ecbdata.hit;
}

static unsigned int contains(mmfile_t *mf, regex_t *regexp, kwset_t kws)
{
	unsigned int cnt;
	unsigned long sz;
	const char *data;

	sz = mf->size;
	data = mf->ptr;
	cnt = 0;

	if (regexp) {
		regmatch_t regmatch;
		int flags = 0;

		while (sz && *data &&
		       !regexec_buf(regexp, data, sz, 1, &regmatch, flags)) {
			flags |= REG_NOTBOL;
			data += regmatch.rm_eo;
			sz -= regmatch.rm_eo;
			if (sz && *data && regmatch.rm_so == regmatch.rm_eo) {
				data++;
				sz--;
			}
			cnt++;
		}

	} else { /* Classic exact string match */
		while (sz) {
			struct kwsmatch kwsm;
			size_t offset = kwsexec(kws, data, sz, &kwsm);
			if (offset == -1)
				break;
			sz -= offset + kwsm.size[0];
			data += offset + kwsm.size[0];
			cnt++;
		}
	}
	return cnt;
}

static int has_changes(mmfile_t *one, mmfile_t *two,
		       struct diff_options *o,
		       regex_t *regexp, kwset_t kws)
{
	unsigned int one_contains = one ? contains(one, regexp, kws) : 0;
	unsigned int two_contains = two ? contains(two, regexp, kws) : 0;
	return one_contains != two_contains;
}

static int pickaxe_match(struct diff_filepair *p, struct diff_options *o,
			 regex_t *regexp, kwset_t kws, pickaxe_fn fn)
{
	struct userdiff_driver *textconv_one = NULL;
	struct userdiff_driver *textconv_two = NULL;
	mmfile_t mf1, mf2;
	int ret;

	/* ignore unmerged */
	if (!DIFF_FILE_VALID(p->one) && !DIFF_FILE_VALID(p->two))
		return 0;

	if (o->objfind) {
		return  (DIFF_FILE_VALID(p->one) &&
			 oidset_contains(o->objfind, &p->one->oid)) ||
			(DIFF_FILE_VALID(p->two) &&
			 oidset_contains(o->objfind, &p->two->oid));
	}

	if (!o->pickaxe[0])
		return 0;

	if (o->flags.allow_textconv) {
		textconv_one = get_textconv(o->repo, p->one);
		textconv_two = get_textconv(o->repo, p->two);
	}

	/*
	 * If we have an unmodified pair, we know that the count will be the
	 * same and don't even have to load the blobs. Unless textconv is in
	 * play, _and_ we are using two different textconv filters (e.g.,
	 * because a pair is an exact rename with different textconv attributes
	 * for each side, which might generate different content).
	 */
	if (textconv_one == textconv_two && diff_unmodified_pair(p))
		return 0;

	if ((o->pickaxe_opts & DIFF_PICKAXE_KIND_G) &&
	    !o->flags.text &&
	    ((!textconv_one && diff_filespec_is_binary(o->repo, p->one)) ||
	     (!textconv_two && diff_filespec_is_binary(o->repo, p->two))))
		return 0;

	mf1.size = fill_textconv(o->repo, textconv_one, p->one, &mf1.ptr);
	mf2.size = fill_textconv(o->repo, textconv_two, p->two, &mf2.ptr);

	ret = fn(DIFF_FILE_VALID(p->one) ? &mf1 : NULL,
		 DIFF_FILE_VALID(p->two) ? &mf2 : NULL,
		 o, regexp, kws);

	if (textconv_one)
		free(mf1.ptr);
	if (textconv_two)
		free(mf2.ptr);
	diff_free_filespec_data(p->one);
	diff_free_filespec_data(p->two);

	return ret;
}

static void pickaxe(struct diff_queue_struct *q, struct diff_options *o,
		    regex_t *regexp, kwset_t kws, pickaxe_fn fn)
{
	int i;
	struct diff_queue_struct outq;

	DIFF_QUEUE_CLEAR(&outq);

	if (o->pickaxe_opts & DIFF_PICKAXE_ALL) {
		/* Showing the whole changeset if needle exists */
		for (i = 0; i < q->nr; i++) {
			struct diff_filepair *p = q->queue[i];
			if (pickaxe_match(p, o, regexp, kws, fn))
				return; /* do not munge the queue */
		}

		/*
		 * Otherwise we will clear the whole queue by copying
		 * the empty outq at the end of this function, but
		 * first clear the current entries in the queue.
		 */
		for (i = 0; i < q->nr; i++)
			diff_free_filepair(q->queue[i]);
	} else {
		/* Showing only the filepairs that has the needle */
		for (i = 0; i < q->nr; i++) {
			struct diff_filepair *p = q->queue[i];
			if (pickaxe_match(p, o, regexp, kws, fn))
				diff_q(&outq, p);
			else
				diff_free_filepair(p);
		}
	}

	free(q->queue);
	*q = outq;
}

static void regcomp_or_die(regex_t *regex, const char *needle, int cflags)
{
	int err = regcomp(regex, needle, cflags);
	if (err) {
		/* The POSIX.2 people are surely sick */
		char errbuf[1024];
		regerror(err, regex, errbuf, 1024);
		die("invalid regex: %s", errbuf);
	}
}

void diffcore_pickaxe(struct diff_options *o)
{
	const char *needle = o->pickaxe;
	int opts = o->pickaxe_opts;
	regex_t regex, *regexp = NULL;
	kwset_t kws = NULL;

	if (opts & (DIFF_PICKAXE_REGEX | DIFF_PICKAXE_KIND_G)) {
		int cflags = REG_EXTENDED | REG_NEWLINE;
		if (o->pickaxe_opts & DIFF_PICKAXE_IGNORE_CASE)
			cflags |= REG_ICASE;
		regcomp_or_die(&regex, needle, cflags);
		regexp = &regex;
	} else if (opts & DIFF_PICKAXE_KIND_S) {
		if (o->pickaxe_opts & DIFF_PICKAXE_IGNORE_CASE &&
		    has_non_ascii(needle)) {
			struct strbuf sb = STRBUF_INIT;
			int cflags = REG_NEWLINE | REG_ICASE;

			basic_regex_quote_buf(&sb, needle);
			regcomp_or_die(&regex, sb.buf, cflags);
			strbuf_release(&sb);
			regexp = &regex;
		} else {
			kws = kwsalloc(o->pickaxe_opts & DIFF_PICKAXE_IGNORE_CASE
				       ? tolower_trans_tbl : NULL);
			kwsincr(kws, needle, strlen(needle));
			kwsprep(kws);
		}
	}

	pickaxe(&diff_queued_diff, o, regexp, kws,
		(opts & DIFF_PICKAXE_KIND_G) ? diff_grep : has_changes);

	if (regexp)
		regfree(regexp);
	if (kws)
		kwsfree(kws);
	return;
}
