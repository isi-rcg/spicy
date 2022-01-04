#include "cache.h"
#include "config.h"
#include "grep.h"
#include "object-store.h"
#include "userdiff.h"
#include "xdiff-interface.h"
#include "diff.h"
#include "diffcore.h"
#include "commit.h"
#include "quote.h"
#include "help.h"

static int grep_source_load(struct grep_source *gs);
static int grep_source_is_binary(struct grep_source *gs,
				 struct index_state *istate);

static struct grep_opt grep_defaults;

static const char *color_grep_slots[] = {
	[GREP_COLOR_CONTEXT]	    = "context",
	[GREP_COLOR_FILENAME]	    = "filename",
	[GREP_COLOR_FUNCTION]	    = "function",
	[GREP_COLOR_LINENO]	    = "lineNumber",
	[GREP_COLOR_COLUMNNO]	    = "column",
	[GREP_COLOR_MATCH_CONTEXT]  = "matchContext",
	[GREP_COLOR_MATCH_SELECTED] = "matchSelected",
	[GREP_COLOR_SELECTED]	    = "selected",
	[GREP_COLOR_SEP]	    = "separator",
};

static void std_output(struct grep_opt *opt, const void *buf, size_t size)
{
	fwrite(buf, size, 1, stdout);
}

static void color_set(char *dst, const char *color_bytes)
{
	xsnprintf(dst, COLOR_MAXLEN, "%s", color_bytes);
}

/*
 * Initialize the grep_defaults template with hardcoded defaults.
 * We could let the compiler do this, but without C99 initializers
 * the code gets unwieldy and unreadable, so...
 */
void init_grep_defaults(struct repository *repo)
{
	struct grep_opt *opt = &grep_defaults;
	static int run_once;

	if (run_once)
		return;
	run_once++;

	memset(opt, 0, sizeof(*opt));
	opt->repo = repo;
	opt->relative = 1;
	opt->pathname = 1;
	opt->max_depth = -1;
	opt->pattern_type_option = GREP_PATTERN_TYPE_UNSPECIFIED;
	color_set(opt->colors[GREP_COLOR_CONTEXT], "");
	color_set(opt->colors[GREP_COLOR_FILENAME], "");
	color_set(opt->colors[GREP_COLOR_FUNCTION], "");
	color_set(opt->colors[GREP_COLOR_LINENO], "");
	color_set(opt->colors[GREP_COLOR_COLUMNNO], "");
	color_set(opt->colors[GREP_COLOR_MATCH_CONTEXT], GIT_COLOR_BOLD_RED);
	color_set(opt->colors[GREP_COLOR_MATCH_SELECTED], GIT_COLOR_BOLD_RED);
	color_set(opt->colors[GREP_COLOR_SELECTED], "");
	color_set(opt->colors[GREP_COLOR_SEP], GIT_COLOR_CYAN);
	opt->only_matching = 0;
	opt->color = -1;
	opt->output = std_output;
}

static int parse_pattern_type_arg(const char *opt, const char *arg)
{
	if (!strcmp(arg, "default"))
		return GREP_PATTERN_TYPE_UNSPECIFIED;
	else if (!strcmp(arg, "basic"))
		return GREP_PATTERN_TYPE_BRE;
	else if (!strcmp(arg, "extended"))
		return GREP_PATTERN_TYPE_ERE;
	else if (!strcmp(arg, "fixed"))
		return GREP_PATTERN_TYPE_FIXED;
	else if (!strcmp(arg, "perl"))
		return GREP_PATTERN_TYPE_PCRE;
	die("bad %s argument: %s", opt, arg);
}

define_list_config_array_extra(color_grep_slots, {"match"});

/*
 * Read the configuration file once and store it in
 * the grep_defaults template.
 */
int grep_config(const char *var, const char *value, void *cb)
{
	struct grep_opt *opt = &grep_defaults;
	const char *slot;

	if (userdiff_config(var, value) < 0)
		return -1;

	if (!strcmp(var, "grep.extendedregexp")) {
		opt->extended_regexp_option = git_config_bool(var, value);
		return 0;
	}

	if (!strcmp(var, "grep.patterntype")) {
		opt->pattern_type_option = parse_pattern_type_arg(var, value);
		return 0;
	}

	if (!strcmp(var, "grep.linenumber")) {
		opt->linenum = git_config_bool(var, value);
		return 0;
	}
	if (!strcmp(var, "grep.column")) {
		opt->columnnum = git_config_bool(var, value);
		return 0;
	}

	if (!strcmp(var, "grep.fullname")) {
		opt->relative = !git_config_bool(var, value);
		return 0;
	}

	if (!strcmp(var, "color.grep"))
		opt->color = git_config_colorbool(var, value);
	if (!strcmp(var, "color.grep.match")) {
		if (grep_config("color.grep.matchcontext", value, cb) < 0)
			return -1;
		if (grep_config("color.grep.matchselected", value, cb) < 0)
			return -1;
	} else if (skip_prefix(var, "color.grep.", &slot)) {
		int i = LOOKUP_CONFIG(color_grep_slots, slot);
		char *color;

		if (i < 0)
			return -1;
		color = opt->colors[i];
		if (!value)
			return config_error_nonbool(var);
		return color_parse(value, color);
	}
	return 0;
}

/*
 * Initialize one instance of grep_opt and copy the
 * default values from the template we read the configuration
 * information in an earlier call to git_config(grep_config).
 */
void grep_init(struct grep_opt *opt, struct repository *repo, const char *prefix)
{
	struct grep_opt *def = &grep_defaults;
	int i;

	memset(opt, 0, sizeof(*opt));
	opt->repo = repo;
	opt->prefix = prefix;
	opt->prefix_length = (prefix && *prefix) ? strlen(prefix) : 0;
	opt->pattern_tail = &opt->pattern_list;
	opt->header_tail = &opt->header_list;

	opt->only_matching = def->only_matching;
	opt->color = def->color;
	opt->extended_regexp_option = def->extended_regexp_option;
	opt->pattern_type_option = def->pattern_type_option;
	opt->linenum = def->linenum;
	opt->columnnum = def->columnnum;
	opt->max_depth = def->max_depth;
	opt->pathname = def->pathname;
	opt->relative = def->relative;
	opt->output = def->output;

	for (i = 0; i < NR_GREP_COLORS; i++)
		color_set(opt->colors[i], def->colors[i]);
}

static void grep_set_pattern_type_option(enum grep_pattern_type pattern_type, struct grep_opt *opt)
{
	/*
	 * When committing to the pattern type by setting the relevant
	 * fields in grep_opt it's generally not necessary to zero out
	 * the fields we're not choosing, since they won't have been
	 * set by anything. The extended_regexp_option field is the
	 * only exception to this.
	 *
	 * This is because in the process of parsing grep.patternType
	 * & grep.extendedRegexp we set opt->pattern_type_option and
	 * opt->extended_regexp_option, respectively. We then
	 * internally use opt->extended_regexp_option to see if we're
	 * compiling an ERE. It must be unset if that's not actually
	 * the case.
	 */
	if (pattern_type != GREP_PATTERN_TYPE_ERE &&
	    opt->extended_regexp_option)
		opt->extended_regexp_option = 0;

	switch (pattern_type) {
	case GREP_PATTERN_TYPE_UNSPECIFIED:
		/* fall through */

	case GREP_PATTERN_TYPE_BRE:
		break;

	case GREP_PATTERN_TYPE_ERE:
		opt->extended_regexp_option = 1;
		break;

	case GREP_PATTERN_TYPE_FIXED:
		opt->fixed = 1;
		break;

	case GREP_PATTERN_TYPE_PCRE:
#ifdef USE_LIBPCRE2
		opt->pcre2 = 1;
#else
		/*
		 * It's important that pcre1 always be assigned to
		 * even when there's no USE_LIBPCRE* defined. We still
		 * call the PCRE stub function, it just dies with
		 * "cannot use Perl-compatible regexes[...]".
		 */
		opt->pcre1 = 1;
#endif
		break;
	}
}

void grep_commit_pattern_type(enum grep_pattern_type pattern_type, struct grep_opt *opt)
{
	if (pattern_type != GREP_PATTERN_TYPE_UNSPECIFIED)
		grep_set_pattern_type_option(pattern_type, opt);
	else if (opt->pattern_type_option != GREP_PATTERN_TYPE_UNSPECIFIED)
		grep_set_pattern_type_option(opt->pattern_type_option, opt);
	else if (opt->extended_regexp_option)
		/*
		 * This branch *must* happen after setting from the
		 * opt->pattern_type_option above, we don't want
		 * grep.extendedRegexp to override grep.patternType!
		 */
		grep_set_pattern_type_option(GREP_PATTERN_TYPE_ERE, opt);
}

static struct grep_pat *create_grep_pat(const char *pat, size_t patlen,
					const char *origin, int no,
					enum grep_pat_token t,
					enum grep_header_field field)
{
	struct grep_pat *p = xcalloc(1, sizeof(*p));
	p->pattern = xmemdupz(pat, patlen);
	p->patternlen = patlen;
	p->origin = origin;
	p->no = no;
	p->token = t;
	p->field = field;
	return p;
}

static void do_append_grep_pat(struct grep_pat ***tail, struct grep_pat *p)
{
	**tail = p;
	*tail = &p->next;
	p->next = NULL;

	switch (p->token) {
	case GREP_PATTERN: /* atom */
	case GREP_PATTERN_HEAD:
	case GREP_PATTERN_BODY:
		for (;;) {
			struct grep_pat *new_pat;
			size_t len = 0;
			char *cp = p->pattern + p->patternlen, *nl = NULL;
			while (++len <= p->patternlen) {
				if (*(--cp) == '\n') {
					nl = cp;
					break;
				}
			}
			if (!nl)
				break;
			new_pat = create_grep_pat(nl + 1, len - 1, p->origin,
						  p->no, p->token, p->field);
			new_pat->next = p->next;
			if (!p->next)
				*tail = &new_pat->next;
			p->next = new_pat;
			*nl = '\0';
			p->patternlen -= len;
		}
		break;
	default:
		break;
	}
}

void append_header_grep_pattern(struct grep_opt *opt,
				enum grep_header_field field, const char *pat)
{
	struct grep_pat *p = create_grep_pat(pat, strlen(pat), "header", 0,
					     GREP_PATTERN_HEAD, field);
	if (field == GREP_HEADER_REFLOG)
		opt->use_reflog_filter = 1;
	do_append_grep_pat(&opt->header_tail, p);
}

void append_grep_pattern(struct grep_opt *opt, const char *pat,
			 const char *origin, int no, enum grep_pat_token t)
{
	append_grep_pat(opt, pat, strlen(pat), origin, no, t);
}

void append_grep_pat(struct grep_opt *opt, const char *pat, size_t patlen,
		     const char *origin, int no, enum grep_pat_token t)
{
	struct grep_pat *p = create_grep_pat(pat, patlen, origin, no, t, 0);
	do_append_grep_pat(&opt->pattern_tail, p);
}

struct grep_opt *grep_opt_dup(const struct grep_opt *opt)
{
	struct grep_pat *pat;
	struct grep_opt *ret = xmalloc(sizeof(struct grep_opt));
	*ret = *opt;

	ret->pattern_list = NULL;
	ret->pattern_tail = &ret->pattern_list;

	for(pat = opt->pattern_list; pat != NULL; pat = pat->next)
	{
		if(pat->token == GREP_PATTERN_HEAD)
			append_header_grep_pattern(ret, pat->field,
						   pat->pattern);
		else
			append_grep_pat(ret, pat->pattern, pat->patternlen,
					pat->origin, pat->no, pat->token);
	}

	return ret;
}

static NORETURN void compile_regexp_failed(const struct grep_pat *p,
		const char *error)
{
	char where[1024];

	if (p->no)
		xsnprintf(where, sizeof(where), "In '%s' at %d, ", p->origin, p->no);
	else if (p->origin)
		xsnprintf(where, sizeof(where), "%s, ", p->origin);
	else
		where[0] = 0;

	die("%s'%s': %s", where, p->pattern, error);
}

static int is_fixed(const char *s, size_t len)
{
	size_t i;

	for (i = 0; i < len; i++) {
		if (is_regex_special(s[i]))
			return 0;
	}

	return 1;
}

static int has_null(const char *s, size_t len)
{
	/*
	 * regcomp cannot accept patterns with NULs so when using it
	 * we consider any pattern containing a NUL fixed.
	 */
	if (memchr(s, 0, len))
		return 1;

	return 0;
}

#ifdef USE_LIBPCRE1
static void compile_pcre1_regexp(struct grep_pat *p, const struct grep_opt *opt)
{
	const char *error;
	int erroffset;
	int options = PCRE_MULTILINE;

	if (opt->ignore_case) {
		if (has_non_ascii(p->pattern))
			p->pcre1_tables = pcre_maketables();
		options |= PCRE_CASELESS;
	}
	if (is_utf8_locale() && has_non_ascii(p->pattern))
		options |= PCRE_UTF8;

	p->pcre1_regexp = pcre_compile(p->pattern, options, &error, &erroffset,
				      p->pcre1_tables);
	if (!p->pcre1_regexp)
		compile_regexp_failed(p, error);

	p->pcre1_extra_info = pcre_study(p->pcre1_regexp, GIT_PCRE_STUDY_JIT_COMPILE, &error);
	if (!p->pcre1_extra_info && error)
		die("%s", error);

#ifdef GIT_PCRE1_USE_JIT
	pcre_config(PCRE_CONFIG_JIT, &p->pcre1_jit_on);
	if (p->pcre1_jit_on == 1) {
		p->pcre1_jit_stack = pcre_jit_stack_alloc(1, 1024 * 1024);
		if (!p->pcre1_jit_stack)
			die("Couldn't allocate PCRE JIT stack");
		pcre_assign_jit_stack(p->pcre1_extra_info, NULL, p->pcre1_jit_stack);
	} else if (p->pcre1_jit_on != 0) {
		BUG("The pcre1_jit_on variable should be 0 or 1, not %d",
		    p->pcre1_jit_on);
	}
#endif
}

static int pcre1match(struct grep_pat *p, const char *line, const char *eol,
		regmatch_t *match, int eflags)
{
	int ovector[30], ret, flags = 0;

	if (eflags & REG_NOTBOL)
		flags |= PCRE_NOTBOL;

#ifdef GIT_PCRE1_USE_JIT
	if (p->pcre1_jit_on) {
		ret = pcre_jit_exec(p->pcre1_regexp, p->pcre1_extra_info, line,
				    eol - line, 0, flags, ovector,
				    ARRAY_SIZE(ovector), p->pcre1_jit_stack);
	} else
#endif
	{
		ret = pcre_exec(p->pcre1_regexp, p->pcre1_extra_info, line,
				eol - line, 0, flags, ovector,
				ARRAY_SIZE(ovector));
	}

	if (ret < 0 && ret != PCRE_ERROR_NOMATCH)
		die("pcre_exec failed with error code %d", ret);
	if (ret > 0) {
		ret = 0;
		match->rm_so = ovector[0];
		match->rm_eo = ovector[1];
	}

	return ret;
}

static void free_pcre1_regexp(struct grep_pat *p)
{
	pcre_free(p->pcre1_regexp);
#ifdef GIT_PCRE1_USE_JIT
	if (p->pcre1_jit_on) {
		pcre_free_study(p->pcre1_extra_info);
		pcre_jit_stack_free(p->pcre1_jit_stack);
	} else
#endif
	{
		pcre_free(p->pcre1_extra_info);
	}
	pcre_free((void *)p->pcre1_tables);
}
#else /* !USE_LIBPCRE1 */
static void compile_pcre1_regexp(struct grep_pat *p, const struct grep_opt *opt)
{
	die("cannot use Perl-compatible regexes when not compiled with USE_LIBPCRE");
}

static int pcre1match(struct grep_pat *p, const char *line, const char *eol,
		regmatch_t *match, int eflags)
{
	return 1;
}

static void free_pcre1_regexp(struct grep_pat *p)
{
}
#endif /* !USE_LIBPCRE1 */

#ifdef USE_LIBPCRE2
static void compile_pcre2_pattern(struct grep_pat *p, const struct grep_opt *opt)
{
	int error;
	PCRE2_UCHAR errbuf[256];
	PCRE2_SIZE erroffset;
	int options = PCRE2_MULTILINE;
	const uint8_t *character_tables = NULL;
	int jitret;
	int patinforet;
	size_t jitsizearg;

	assert(opt->pcre2);

	p->pcre2_compile_context = NULL;

	if (opt->ignore_case) {
		if (has_non_ascii(p->pattern)) {
			character_tables = pcre2_maketables(NULL);
			p->pcre2_compile_context = pcre2_compile_context_create(NULL);
			pcre2_set_character_tables(p->pcre2_compile_context, character_tables);
		}
		options |= PCRE2_CASELESS;
	}
	if (is_utf8_locale() && has_non_ascii(p->pattern))
		options |= PCRE2_UTF;

	p->pcre2_pattern = pcre2_compile((PCRE2_SPTR)p->pattern,
					 p->patternlen, options, &error, &erroffset,
					 p->pcre2_compile_context);

	if (p->pcre2_pattern) {
		p->pcre2_match_data = pcre2_match_data_create_from_pattern(p->pcre2_pattern, NULL);
		if (!p->pcre2_match_data)
			die("Couldn't allocate PCRE2 match data");
	} else {
		pcre2_get_error_message(error, errbuf, sizeof(errbuf));
		compile_regexp_failed(p, (const char *)&errbuf);
	}

	pcre2_config(PCRE2_CONFIG_JIT, &p->pcre2_jit_on);
	if (p->pcre2_jit_on == 1) {
		jitret = pcre2_jit_compile(p->pcre2_pattern, PCRE2_JIT_COMPLETE);
		if (jitret)
			die("Couldn't JIT the PCRE2 pattern '%s', got '%d'\n", p->pattern, jitret);

		/*
		 * The pcre2_config(PCRE2_CONFIG_JIT, ...) call just
		 * tells us whether the library itself supports JIT,
		 * but to see whether we're going to be actually using
		 * JIT we need to extract PCRE2_INFO_JITSIZE from the
		 * pattern *after* we do pcre2_jit_compile() above.
		 *
		 * This is because if the pattern contains the
		 * (*NO_JIT) verb (see pcre2syntax(3))
		 * pcre2_jit_compile() will exit early with 0. If we
		 * then proceed to call pcre2_jit_match() further down
		 * the line instead of pcre2_match() we'll either
		 * segfault (pre PCRE 10.31) or run into a fatal error
		 * (post PCRE2 10.31)
		 */
		patinforet = pcre2_pattern_info(p->pcre2_pattern, PCRE2_INFO_JITSIZE, &jitsizearg);
		if (patinforet)
			BUG("pcre2_pattern_info() failed: %d", patinforet);
		if (jitsizearg == 0) {
			p->pcre2_jit_on = 0;
			return;
		}

		p->pcre2_jit_stack = pcre2_jit_stack_create(1, 1024 * 1024, NULL);
		if (!p->pcre2_jit_stack)
			die("Couldn't allocate PCRE2 JIT stack");
		p->pcre2_match_context = pcre2_match_context_create(NULL);
		if (!p->pcre2_match_context)
			die("Couldn't allocate PCRE2 match context");
		pcre2_jit_stack_assign(p->pcre2_match_context, NULL, p->pcre2_jit_stack);
	} else if (p->pcre2_jit_on != 0) {
		BUG("The pcre2_jit_on variable should be 0 or 1, not %d",
		    p->pcre2_jit_on);
	}
}

static int pcre2match(struct grep_pat *p, const char *line, const char *eol,
		regmatch_t *match, int eflags)
{
	int ret, flags = 0;
	PCRE2_SIZE *ovector;
	PCRE2_UCHAR errbuf[256];

	if (eflags & REG_NOTBOL)
		flags |= PCRE2_NOTBOL;

	if (p->pcre2_jit_on)
		ret = pcre2_jit_match(p->pcre2_pattern, (unsigned char *)line,
				      eol - line, 0, flags, p->pcre2_match_data,
				      NULL);
	else
		ret = pcre2_match(p->pcre2_pattern, (unsigned char *)line,
				  eol - line, 0, flags, p->pcre2_match_data,
				  NULL);

	if (ret < 0 && ret != PCRE2_ERROR_NOMATCH) {
		pcre2_get_error_message(ret, errbuf, sizeof(errbuf));
		die("%s failed with error code %d: %s",
		    (p->pcre2_jit_on ? "pcre2_jit_match" : "pcre2_match"), ret,
		    errbuf);
	}
	if (ret > 0) {
		ovector = pcre2_get_ovector_pointer(p->pcre2_match_data);
		ret = 0;
		match->rm_so = (int)ovector[0];
		match->rm_eo = (int)ovector[1];
	}

	return ret;
}

static void free_pcre2_pattern(struct grep_pat *p)
{
	pcre2_compile_context_free(p->pcre2_compile_context);
	pcre2_code_free(p->pcre2_pattern);
	pcre2_match_data_free(p->pcre2_match_data);
	pcre2_jit_stack_free(p->pcre2_jit_stack);
	pcre2_match_context_free(p->pcre2_match_context);
}
#else /* !USE_LIBPCRE2 */
static void compile_pcre2_pattern(struct grep_pat *p, const struct grep_opt *opt)
{
	/*
	 * Unreachable until USE_LIBPCRE2 becomes synonymous with
	 * USE_LIBPCRE. See the sibling comment in
	 * grep_set_pattern_type_option().
	 */
	die("cannot use Perl-compatible regexes when not compiled with USE_LIBPCRE");
}

static int pcre2match(struct grep_pat *p, const char *line, const char *eol,
		regmatch_t *match, int eflags)
{
	return 1;
}

static void free_pcre2_pattern(struct grep_pat *p)
{
}
#endif /* !USE_LIBPCRE2 */

static void compile_fixed_regexp(struct grep_pat *p, struct grep_opt *opt)
{
	struct strbuf sb = STRBUF_INIT;
	int err;
	int regflags = 0;

	basic_regex_quote_buf(&sb, p->pattern);
	if (opt->ignore_case)
		regflags |= REG_ICASE;
	err = regcomp(&p->regexp, sb.buf, regflags);
	if (opt->debug)
		fprintf(stderr, "fixed %s\n", sb.buf);
	strbuf_release(&sb);
	if (err) {
		char errbuf[1024];
		regerror(err, &p->regexp, errbuf, sizeof(errbuf));
		compile_regexp_failed(p, errbuf);
	}
}

static void compile_regexp(struct grep_pat *p, struct grep_opt *opt)
{
	int ascii_only;
	int err;
	int regflags = REG_NEWLINE;

	p->word_regexp = opt->word_regexp;
	p->ignore_case = opt->ignore_case;
	ascii_only     = !has_non_ascii(p->pattern);

	/*
	 * Even when -F (fixed) asks us to do a non-regexp search, we
	 * may not be able to correctly case-fold when -i
	 * (ignore-case) is asked (in which case, we'll synthesize a
	 * regexp to match the pattern that matches regexp special
	 * characters literally, while ignoring case differences).  On
	 * the other hand, even without -F, if the pattern does not
	 * have any regexp special characters and there is no need for
	 * case-folding search, we can internally turn it into a
	 * simple string match using kws.  p->fixed tells us if we
	 * want to use kws.
	 */
	if (opt->fixed ||
	    has_null(p->pattern, p->patternlen) ||
	    is_fixed(p->pattern, p->patternlen))
		p->fixed = !p->ignore_case || ascii_only;

	if (p->fixed) {
		p->kws = kwsalloc(p->ignore_case ? tolower_trans_tbl : NULL);
		kwsincr(p->kws, p->pattern, p->patternlen);
		kwsprep(p->kws);
		return;
	} else if (opt->fixed) {
		/*
		 * We come here when the pattern has the non-ascii
		 * characters we cannot case-fold, and asked to
		 * ignore-case.
		 */
		compile_fixed_regexp(p, opt);
		return;
	}

	if (opt->pcre2) {
		compile_pcre2_pattern(p, opt);
		return;
	}

	if (opt->pcre1) {
		compile_pcre1_regexp(p, opt);
		return;
	}

	if (p->ignore_case)
		regflags |= REG_ICASE;
	if (opt->extended_regexp_option)
		regflags |= REG_EXTENDED;
	err = regcomp(&p->regexp, p->pattern, regflags);
	if (err) {
		char errbuf[1024];
		regerror(err, &p->regexp, errbuf, 1024);
		compile_regexp_failed(p, errbuf);
	}
}

static struct grep_expr *compile_pattern_or(struct grep_pat **);
static struct grep_expr *compile_pattern_atom(struct grep_pat **list)
{
	struct grep_pat *p;
	struct grep_expr *x;

	p = *list;
	if (!p)
		return NULL;
	switch (p->token) {
	case GREP_PATTERN: /* atom */
	case GREP_PATTERN_HEAD:
	case GREP_PATTERN_BODY:
		x = xcalloc(1, sizeof (struct grep_expr));
		x->node = GREP_NODE_ATOM;
		x->u.atom = p;
		*list = p->next;
		return x;
	case GREP_OPEN_PAREN:
		*list = p->next;
		x = compile_pattern_or(list);
		if (!*list || (*list)->token != GREP_CLOSE_PAREN)
			die("unmatched parenthesis");
		*list = (*list)->next;
		return x;
	default:
		return NULL;
	}
}

static struct grep_expr *compile_pattern_not(struct grep_pat **list)
{
	struct grep_pat *p;
	struct grep_expr *x;

	p = *list;
	if (!p)
		return NULL;
	switch (p->token) {
	case GREP_NOT:
		if (!p->next)
			die("--not not followed by pattern expression");
		*list = p->next;
		x = xcalloc(1, sizeof (struct grep_expr));
		x->node = GREP_NODE_NOT;
		x->u.unary = compile_pattern_not(list);
		if (!x->u.unary)
			die("--not followed by non pattern expression");
		return x;
	default:
		return compile_pattern_atom(list);
	}
}

static struct grep_expr *compile_pattern_and(struct grep_pat **list)
{
	struct grep_pat *p;
	struct grep_expr *x, *y, *z;

	x = compile_pattern_not(list);
	p = *list;
	if (p && p->token == GREP_AND) {
		if (!p->next)
			die("--and not followed by pattern expression");
		*list = p->next;
		y = compile_pattern_and(list);
		if (!y)
			die("--and not followed by pattern expression");
		z = xcalloc(1, sizeof (struct grep_expr));
		z->node = GREP_NODE_AND;
		z->u.binary.left = x;
		z->u.binary.right = y;
		return z;
	}
	return x;
}

static struct grep_expr *compile_pattern_or(struct grep_pat **list)
{
	struct grep_pat *p;
	struct grep_expr *x, *y, *z;

	x = compile_pattern_and(list);
	p = *list;
	if (x && p && p->token != GREP_CLOSE_PAREN) {
		y = compile_pattern_or(list);
		if (!y)
			die("not a pattern expression %s", p->pattern);
		z = xcalloc(1, sizeof (struct grep_expr));
		z->node = GREP_NODE_OR;
		z->u.binary.left = x;
		z->u.binary.right = y;
		return z;
	}
	return x;
}

static struct grep_expr *compile_pattern_expr(struct grep_pat **list)
{
	return compile_pattern_or(list);
}

static void indent(int in)
{
	while (in-- > 0)
		fputc(' ', stderr);
}

static void dump_grep_pat(struct grep_pat *p)
{
	switch (p->token) {
	case GREP_AND: fprintf(stderr, "*and*"); break;
	case GREP_OPEN_PAREN: fprintf(stderr, "*(*"); break;
	case GREP_CLOSE_PAREN: fprintf(stderr, "*)*"); break;
	case GREP_NOT: fprintf(stderr, "*not*"); break;
	case GREP_OR: fprintf(stderr, "*or*"); break;

	case GREP_PATTERN: fprintf(stderr, "pattern"); break;
	case GREP_PATTERN_HEAD: fprintf(stderr, "pattern_head"); break;
	case GREP_PATTERN_BODY: fprintf(stderr, "pattern_body"); break;
	}

	switch (p->token) {
	default: break;
	case GREP_PATTERN_HEAD:
		fprintf(stderr, "<head %d>", p->field); break;
	case GREP_PATTERN_BODY:
		fprintf(stderr, "<body>"); break;
	}
	switch (p->token) {
	default: break;
	case GREP_PATTERN_HEAD:
	case GREP_PATTERN_BODY:
	case GREP_PATTERN:
		fprintf(stderr, "%.*s", (int)p->patternlen, p->pattern);
		break;
	}
	fputc('\n', stderr);
}

static void dump_grep_expression_1(struct grep_expr *x, int in)
{
	indent(in);
	switch (x->node) {
	case GREP_NODE_TRUE:
		fprintf(stderr, "true\n");
		break;
	case GREP_NODE_ATOM:
		dump_grep_pat(x->u.atom);
		break;
	case GREP_NODE_NOT:
		fprintf(stderr, "(not\n");
		dump_grep_expression_1(x->u.unary, in+1);
		indent(in);
		fprintf(stderr, ")\n");
		break;
	case GREP_NODE_AND:
		fprintf(stderr, "(and\n");
		dump_grep_expression_1(x->u.binary.left, in+1);
		dump_grep_expression_1(x->u.binary.right, in+1);
		indent(in);
		fprintf(stderr, ")\n");
		break;
	case GREP_NODE_OR:
		fprintf(stderr, "(or\n");
		dump_grep_expression_1(x->u.binary.left, in+1);
		dump_grep_expression_1(x->u.binary.right, in+1);
		indent(in);
		fprintf(stderr, ")\n");
		break;
	}
}

static void dump_grep_expression(struct grep_opt *opt)
{
	struct grep_expr *x = opt->pattern_expression;

	if (opt->all_match)
		fprintf(stderr, "[all-match]\n");
	dump_grep_expression_1(x, 0);
	fflush(NULL);
}

static struct grep_expr *grep_true_expr(void)
{
	struct grep_expr *z = xcalloc(1, sizeof(*z));
	z->node = GREP_NODE_TRUE;
	return z;
}

static struct grep_expr *grep_or_expr(struct grep_expr *left, struct grep_expr *right)
{
	struct grep_expr *z = xcalloc(1, sizeof(*z));
	z->node = GREP_NODE_OR;
	z->u.binary.left = left;
	z->u.binary.right = right;
	return z;
}

static struct grep_expr *prep_header_patterns(struct grep_opt *opt)
{
	struct grep_pat *p;
	struct grep_expr *header_expr;
	struct grep_expr *(header_group[GREP_HEADER_FIELD_MAX]);
	enum grep_header_field fld;

	if (!opt->header_list)
		return NULL;

	for (p = opt->header_list; p; p = p->next) {
		if (p->token != GREP_PATTERN_HEAD)
			BUG("a non-header pattern in grep header list.");
		if (p->field < GREP_HEADER_FIELD_MIN ||
		    GREP_HEADER_FIELD_MAX <= p->field)
			BUG("unknown header field %d", p->field);
		compile_regexp(p, opt);
	}

	for (fld = 0; fld < GREP_HEADER_FIELD_MAX; fld++)
		header_group[fld] = NULL;

	for (p = opt->header_list; p; p = p->next) {
		struct grep_expr *h;
		struct grep_pat *pp = p;

		h = compile_pattern_atom(&pp);
		if (!h || pp != p->next)
			BUG("malformed header expr");
		if (!header_group[p->field]) {
			header_group[p->field] = h;
			continue;
		}
		header_group[p->field] = grep_or_expr(h, header_group[p->field]);
	}

	header_expr = NULL;

	for (fld = 0; fld < GREP_HEADER_FIELD_MAX; fld++) {
		if (!header_group[fld])
			continue;
		if (!header_expr)
			header_expr = grep_true_expr();
		header_expr = grep_or_expr(header_group[fld], header_expr);
	}
	return header_expr;
}

static struct grep_expr *grep_splice_or(struct grep_expr *x, struct grep_expr *y)
{
	struct grep_expr *z = x;

	while (x) {
		assert(x->node == GREP_NODE_OR);
		if (x->u.binary.right &&
		    x->u.binary.right->node == GREP_NODE_TRUE) {
			x->u.binary.right = y;
			break;
		}
		x = x->u.binary.right;
	}
	return z;
}

static void compile_grep_patterns_real(struct grep_opt *opt)
{
	struct grep_pat *p;
	struct grep_expr *header_expr = prep_header_patterns(opt);

	for (p = opt->pattern_list; p; p = p->next) {
		switch (p->token) {
		case GREP_PATTERN: /* atom */
		case GREP_PATTERN_HEAD:
		case GREP_PATTERN_BODY:
			compile_regexp(p, opt);
			break;
		default:
			opt->extended = 1;
			break;
		}
	}

	if (opt->all_match || header_expr)
		opt->extended = 1;
	else if (!opt->extended && !opt->debug)
		return;

	p = opt->pattern_list;
	if (p)
		opt->pattern_expression = compile_pattern_expr(&p);
	if (p)
		die("incomplete pattern expression: %s", p->pattern);

	if (!header_expr)
		return;

	if (!opt->pattern_expression)
		opt->pattern_expression = header_expr;
	else if (opt->all_match)
		opt->pattern_expression = grep_splice_or(header_expr,
							 opt->pattern_expression);
	else
		opt->pattern_expression = grep_or_expr(opt->pattern_expression,
						       header_expr);
	opt->all_match = 1;
}

void compile_grep_patterns(struct grep_opt *opt)
{
	compile_grep_patterns_real(opt);
	if (opt->debug)
		dump_grep_expression(opt);
}

static void free_pattern_expr(struct grep_expr *x)
{
	switch (x->node) {
	case GREP_NODE_TRUE:
	case GREP_NODE_ATOM:
		break;
	case GREP_NODE_NOT:
		free_pattern_expr(x->u.unary);
		break;
	case GREP_NODE_AND:
	case GREP_NODE_OR:
		free_pattern_expr(x->u.binary.left);
		free_pattern_expr(x->u.binary.right);
		break;
	}
	free(x);
}

void free_grep_patterns(struct grep_opt *opt)
{
	struct grep_pat *p, *n;

	for (p = opt->pattern_list; p; p = n) {
		n = p->next;
		switch (p->token) {
		case GREP_PATTERN: /* atom */
		case GREP_PATTERN_HEAD:
		case GREP_PATTERN_BODY:
			if (p->kws)
				kwsfree(p->kws);
			else if (p->pcre1_regexp)
				free_pcre1_regexp(p);
			else if (p->pcre2_pattern)
				free_pcre2_pattern(p);
			else
				regfree(&p->regexp);
			free(p->pattern);
			break;
		default:
			break;
		}
		free(p);
	}

	if (!opt->extended)
		return;
	free_pattern_expr(opt->pattern_expression);
}

static char *end_of_line(char *cp, unsigned long *left)
{
	unsigned long l = *left;
	while (l && *cp != '\n') {
		l--;
		cp++;
	}
	*left = l;
	return cp;
}

static int word_char(char ch)
{
	return isalnum(ch) || ch == '_';
}

static void output_color(struct grep_opt *opt, const void *data, size_t size,
			 const char *color)
{
	if (want_color(opt->color) && color && color[0]) {
		opt->output(opt, color, strlen(color));
		opt->output(opt, data, size);
		opt->output(opt, GIT_COLOR_RESET, strlen(GIT_COLOR_RESET));
	} else
		opt->output(opt, data, size);
}

static void output_sep(struct grep_opt *opt, char sign)
{
	if (opt->null_following_name)
		opt->output(opt, "\0", 1);
	else
		output_color(opt, &sign, 1, opt->colors[GREP_COLOR_SEP]);
}

static void show_name(struct grep_opt *opt, const char *name)
{
	output_color(opt, name, strlen(name), opt->colors[GREP_COLOR_FILENAME]);
	opt->output(opt, opt->null_following_name ? "\0" : "\n", 1);
}

static int fixmatch(struct grep_pat *p, char *line, char *eol,
		    regmatch_t *match)
{
	struct kwsmatch kwsm;
	size_t offset = kwsexec(p->kws, line, eol - line, &kwsm);
	if (offset == -1) {
		match->rm_so = match->rm_eo = -1;
		return REG_NOMATCH;
	} else {
		match->rm_so = offset;
		match->rm_eo = match->rm_so + kwsm.size[0];
		return 0;
	}
}

static int patmatch(struct grep_pat *p, char *line, char *eol,
		    regmatch_t *match, int eflags)
{
	int hit;

	if (p->fixed)
		hit = !fixmatch(p, line, eol, match);
	else if (p->pcre1_regexp)
		hit = !pcre1match(p, line, eol, match, eflags);
	else if (p->pcre2_pattern)
		hit = !pcre2match(p, line, eol, match, eflags);
	else
		hit = !regexec_buf(&p->regexp, line, eol - line, 1, match,
				   eflags);

	return hit;
}

static int strip_timestamp(char *bol, char **eol_p)
{
	char *eol = *eol_p;
	int ch;

	while (bol < --eol) {
		if (*eol != '>')
			continue;
		*eol_p = ++eol;
		ch = *eol;
		*eol = '\0';
		return ch;
	}
	return 0;
}

static struct {
	const char *field;
	size_t len;
} header_field[] = {
	{ "author ", 7 },
	{ "committer ", 10 },
	{ "reflog ", 7 },
};

static int match_one_pattern(struct grep_pat *p, char *bol, char *eol,
			     enum grep_context ctx,
			     regmatch_t *pmatch, int eflags)
{
	int hit = 0;
	int saved_ch = 0;
	const char *start = bol;

	if ((p->token != GREP_PATTERN) &&
	    ((p->token == GREP_PATTERN_HEAD) != (ctx == GREP_CONTEXT_HEAD)))
		return 0;

	if (p->token == GREP_PATTERN_HEAD) {
		const char *field;
		size_t len;
		assert(p->field < ARRAY_SIZE(header_field));
		field = header_field[p->field].field;
		len = header_field[p->field].len;
		if (strncmp(bol, field, len))
			return 0;
		bol += len;
		switch (p->field) {
		case GREP_HEADER_AUTHOR:
		case GREP_HEADER_COMMITTER:
			saved_ch = strip_timestamp(bol, &eol);
			break;
		default:
			break;
		}
	}

 again:
	hit = patmatch(p, bol, eol, pmatch, eflags);

	if (hit && p->word_regexp) {
		if ((pmatch[0].rm_so < 0) ||
		    (eol - bol) < pmatch[0].rm_so ||
		    (pmatch[0].rm_eo < 0) ||
		    (eol - bol) < pmatch[0].rm_eo)
			die("regexp returned nonsense");

		/* Match beginning must be either beginning of the
		 * line, or at word boundary (i.e. the last char must
		 * not be a word char).  Similarly, match end must be
		 * either end of the line, or at word boundary
		 * (i.e. the next char must not be a word char).
		 */
		if ( ((pmatch[0].rm_so == 0) ||
		      !word_char(bol[pmatch[0].rm_so-1])) &&
		     ((pmatch[0].rm_eo == (eol-bol)) ||
		      !word_char(bol[pmatch[0].rm_eo])) )
			;
		else
			hit = 0;

		/* Words consist of at least one character. */
		if (pmatch->rm_so == pmatch->rm_eo)
			hit = 0;

		if (!hit && pmatch[0].rm_so + bol + 1 < eol) {
			/* There could be more than one match on the
			 * line, and the first match might not be
			 * strict word match.  But later ones could be!
			 * Forward to the next possible start, i.e. the
			 * next position following a non-word char.
			 */
			bol = pmatch[0].rm_so + bol + 1;
			while (word_char(bol[-1]) && bol < eol)
				bol++;
			eflags |= REG_NOTBOL;
			if (bol < eol)
				goto again;
		}
	}
	if (p->token == GREP_PATTERN_HEAD && saved_ch)
		*eol = saved_ch;
	if (hit) {
		pmatch[0].rm_so += bol - start;
		pmatch[0].rm_eo += bol - start;
	}
	return hit;
}

static int match_expr_eval(struct grep_opt *opt, struct grep_expr *x, char *bol,
			   char *eol, enum grep_context ctx, ssize_t *col,
			   ssize_t *icol, int collect_hits)
{
	int h = 0;

	if (!x)
		die("Not a valid grep expression");
	switch (x->node) {
	case GREP_NODE_TRUE:
		h = 1;
		break;
	case GREP_NODE_ATOM:
		{
			regmatch_t tmp;
			h = match_one_pattern(x->u.atom, bol, eol, ctx,
					      &tmp, 0);
			if (h && (*col < 0 || tmp.rm_so < *col))
				*col = tmp.rm_so;
		}
		break;
	case GREP_NODE_NOT:
		/*
		 * Upon visiting a GREP_NODE_NOT, col and icol become swapped.
		 */
		h = !match_expr_eval(opt, x->u.unary, bol, eol, ctx, icol, col,
				     0);
		break;
	case GREP_NODE_AND:
		h = match_expr_eval(opt, x->u.binary.left, bol, eol, ctx, col,
				    icol, 0);
		if (h || opt->columnnum) {
			/*
			 * Don't short-circuit AND when given --column, since a
			 * NOT earlier in the tree may turn this into an OR. In
			 * this case, see the below comment.
			 */
			h &= match_expr_eval(opt, x->u.binary.right, bol, eol,
					     ctx, col, icol, 0);
		}
		break;
	case GREP_NODE_OR:
		if (!(collect_hits || opt->columnnum)) {
			/*
			 * Don't short-circuit OR when given --column (or
			 * collecting hits) to ensure we don't skip a later
			 * child that would produce an earlier match.
			 */
			return (match_expr_eval(opt, x->u.binary.left, bol, eol,
						ctx, col, icol, 0) ||
				match_expr_eval(opt, x->u.binary.right, bol,
						eol, ctx, col, icol, 0));
		}
		h = match_expr_eval(opt, x->u.binary.left, bol, eol, ctx, col,
				    icol, 0);
		if (collect_hits)
			x->u.binary.left->hit |= h;
		h |= match_expr_eval(opt, x->u.binary.right, bol, eol, ctx, col,
				     icol, collect_hits);
		break;
	default:
		die("Unexpected node type (internal error) %d", x->node);
	}
	if (collect_hits)
		x->hit |= h;
	return h;
}

static int match_expr(struct grep_opt *opt, char *bol, char *eol,
		      enum grep_context ctx, ssize_t *col,
		      ssize_t *icol, int collect_hits)
{
	struct grep_expr *x = opt->pattern_expression;
	return match_expr_eval(opt, x, bol, eol, ctx, col, icol, collect_hits);
}

static int match_line(struct grep_opt *opt, char *bol, char *eol,
		      ssize_t *col, ssize_t *icol,
		      enum grep_context ctx, int collect_hits)
{
	struct grep_pat *p;
	int hit = 0;

	if (opt->extended)
		return match_expr(opt, bol, eol, ctx, col, icol,
				  collect_hits);

	/* we do not call with collect_hits without being extended */
	for (p = opt->pattern_list; p; p = p->next) {
		regmatch_t tmp;
		if (match_one_pattern(p, bol, eol, ctx, &tmp, 0)) {
			hit |= 1;
			if (!opt->columnnum) {
				/*
				 * Without --column, any single match on a line
				 * is enough to know that it needs to be
				 * printed. With --column, scan _all_ patterns
				 * to find the earliest.
				 */
				break;
			}
			if (*col < 0 || tmp.rm_so < *col)
				*col = tmp.rm_so;
		}
	}
	return hit;
}

static int match_next_pattern(struct grep_pat *p, char *bol, char *eol,
			      enum grep_context ctx,
			      regmatch_t *pmatch, int eflags)
{
	regmatch_t match;

	if (!match_one_pattern(p, bol, eol, ctx, &match, eflags))
		return 0;
	if (match.rm_so < 0 || match.rm_eo < 0)
		return 0;
	if (pmatch->rm_so >= 0 && pmatch->rm_eo >= 0) {
		if (match.rm_so > pmatch->rm_so)
			return 1;
		if (match.rm_so == pmatch->rm_so && match.rm_eo < pmatch->rm_eo)
			return 1;
	}
	pmatch->rm_so = match.rm_so;
	pmatch->rm_eo = match.rm_eo;
	return 1;
}

static int next_match(struct grep_opt *opt, char *bol, char *eol,
		      enum grep_context ctx, regmatch_t *pmatch, int eflags)
{
	struct grep_pat *p;
	int hit = 0;

	pmatch->rm_so = pmatch->rm_eo = -1;
	if (bol < eol) {
		for (p = opt->pattern_list; p; p = p->next) {
			switch (p->token) {
			case GREP_PATTERN: /* atom */
			case GREP_PATTERN_HEAD:
			case GREP_PATTERN_BODY:
				hit |= match_next_pattern(p, bol, eol, ctx,
							  pmatch, eflags);
				break;
			default:
				break;
			}
		}
	}
	return hit;
}

static void show_line_header(struct grep_opt *opt, const char *name,
			     unsigned lno, ssize_t cno, char sign)
{
	if (opt->heading && opt->last_shown == 0) {
		output_color(opt, name, strlen(name), opt->colors[GREP_COLOR_FILENAME]);
		opt->output(opt, "\n", 1);
	}
	opt->last_shown = lno;

	if (!opt->heading && opt->pathname) {
		output_color(opt, name, strlen(name), opt->colors[GREP_COLOR_FILENAME]);
		output_sep(opt, sign);
	}
	if (opt->linenum) {
		char buf[32];
		xsnprintf(buf, sizeof(buf), "%d", lno);
		output_color(opt, buf, strlen(buf), opt->colors[GREP_COLOR_LINENO]);
		output_sep(opt, sign);
	}
	/*
	 * Treat 'cno' as the 1-indexed offset from the start of a non-context
	 * line to its first match. Otherwise, 'cno' is 0 indicating that we are
	 * being called with a context line.
	 */
	if (opt->columnnum && cno) {
		char buf[32];
		xsnprintf(buf, sizeof(buf), "%"PRIuMAX, (uintmax_t)cno);
		output_color(opt, buf, strlen(buf), opt->colors[GREP_COLOR_COLUMNNO]);
		output_sep(opt, sign);
	}
}

static void show_line(struct grep_opt *opt, char *bol, char *eol,
		      const char *name, unsigned lno, ssize_t cno, char sign)
{
	int rest = eol - bol;
	const char *match_color = NULL;
	const char *line_color = NULL;

	if (opt->file_break && opt->last_shown == 0) {
		if (opt->show_hunk_mark)
			opt->output(opt, "\n", 1);
	} else if (opt->pre_context || opt->post_context || opt->funcbody) {
		if (opt->last_shown == 0) {
			if (opt->show_hunk_mark) {
				output_color(opt, "--", 2, opt->colors[GREP_COLOR_SEP]);
				opt->output(opt, "\n", 1);
			}
		} else if (lno > opt->last_shown + 1) {
			output_color(opt, "--", 2, opt->colors[GREP_COLOR_SEP]);
			opt->output(opt, "\n", 1);
		}
	}
	if (!opt->only_matching) {
		/*
		 * In case the line we're being called with contains more than
		 * one match, leave printing each header to the loop below.
		 */
		show_line_header(opt, name, lno, cno, sign);
	}
	if (opt->color || opt->only_matching) {
		regmatch_t match;
		enum grep_context ctx = GREP_CONTEXT_BODY;
		int ch = *eol;
		int eflags = 0;

		if (opt->color) {
			if (sign == ':')
				match_color = opt->colors[GREP_COLOR_MATCH_SELECTED];
			else
				match_color = opt->colors[GREP_COLOR_MATCH_CONTEXT];
			if (sign == ':')
				line_color = opt->colors[GREP_COLOR_SELECTED];
			else if (sign == '-')
				line_color = opt->colors[GREP_COLOR_CONTEXT];
			else if (sign == '=')
				line_color = opt->colors[GREP_COLOR_FUNCTION];
		}
		*eol = '\0';
		while (next_match(opt, bol, eol, ctx, &match, eflags)) {
			if (match.rm_so == match.rm_eo)
				break;

			if (opt->only_matching)
				show_line_header(opt, name, lno, cno, sign);
			else
				output_color(opt, bol, match.rm_so, line_color);
			output_color(opt, bol + match.rm_so,
				     match.rm_eo - match.rm_so, match_color);
			if (opt->only_matching)
				opt->output(opt, "\n", 1);
			bol += match.rm_eo;
			cno += match.rm_eo;
			rest -= match.rm_eo;
			eflags = REG_NOTBOL;
		}
		*eol = ch;
	}
	if (!opt->only_matching) {
		output_color(opt, bol, rest, line_color);
		opt->output(opt, "\n", 1);
	}
}

int grep_use_locks;

/*
 * This lock protects access to the gitattributes machinery, which is
 * not thread-safe.
 */
pthread_mutex_t grep_attr_mutex;

static inline void grep_attr_lock(void)
{
	if (grep_use_locks)
		pthread_mutex_lock(&grep_attr_mutex);
}

static inline void grep_attr_unlock(void)
{
	if (grep_use_locks)
		pthread_mutex_unlock(&grep_attr_mutex);
}

/*
 * Same as git_attr_mutex, but protecting the thread-unsafe object db access.
 */
pthread_mutex_t grep_read_mutex;

static int match_funcname(struct grep_opt *opt, struct grep_source *gs, char *bol, char *eol)
{
	xdemitconf_t *xecfg = opt->priv;
	if (xecfg && !xecfg->find_func) {
		grep_source_load_driver(gs, opt->repo->index);
		if (gs->driver->funcname.pattern) {
			const struct userdiff_funcname *pe = &gs->driver->funcname;
			xdiff_set_find_func(xecfg, pe->pattern, pe->cflags);
		} else {
			xecfg = opt->priv = NULL;
		}
	}

	if (xecfg) {
		char buf[1];
		return xecfg->find_func(bol, eol - bol, buf, 1,
					xecfg->find_func_priv) >= 0;
	}

	if (bol == eol)
		return 0;
	if (isalpha(*bol) || *bol == '_' || *bol == '$')
		return 1;
	return 0;
}

static void show_funcname_line(struct grep_opt *opt, struct grep_source *gs,
			       char *bol, unsigned lno)
{
	while (bol > gs->buf) {
		char *eol = --bol;

		while (bol > gs->buf && bol[-1] != '\n')
			bol--;
		lno--;

		if (lno <= opt->last_shown)
			break;

		if (match_funcname(opt, gs, bol, eol)) {
			show_line(opt, bol, eol, gs->name, lno, 0, '=');
			break;
		}
	}
}

static int is_empty_line(const char *bol, const char *eol);

static void show_pre_context(struct grep_opt *opt, struct grep_source *gs,
			     char *bol, char *end, unsigned lno)
{
	unsigned cur = lno, from = 1, funcname_lno = 0, orig_from;
	int funcname_needed = !!opt->funcname, comment_needed = 0;

	if (opt->pre_context < lno)
		from = lno - opt->pre_context;
	if (from <= opt->last_shown)
		from = opt->last_shown + 1;
	orig_from = from;
	if (opt->funcbody) {
		if (match_funcname(opt, gs, bol, end))
			comment_needed = 1;
		else
			funcname_needed = 1;
		from = opt->last_shown + 1;
	}

	/* Rewind. */
	while (bol > gs->buf && cur > from) {
		char *next_bol = bol;
		char *eol = --bol;

		while (bol > gs->buf && bol[-1] != '\n')
			bol--;
		cur--;
		if (comment_needed && (is_empty_line(bol, eol) ||
				       match_funcname(opt, gs, bol, eol))) {
			comment_needed = 0;
			from = orig_from;
			if (cur < from) {
				cur++;
				bol = next_bol;
				break;
			}
		}
		if (funcname_needed && match_funcname(opt, gs, bol, eol)) {
			funcname_lno = cur;
			funcname_needed = 0;
			if (opt->funcbody)
				comment_needed = 1;
			else
				from = orig_from;
		}
	}

	/* We need to look even further back to find a function signature. */
	if (opt->funcname && funcname_needed)
		show_funcname_line(opt, gs, bol, cur);

	/* Back forward. */
	while (cur < lno) {
		char *eol = bol, sign = (cur == funcname_lno) ? '=' : '-';

		while (*eol != '\n')
			eol++;
		show_line(opt, bol, eol, gs->name, cur, 0, sign);
		bol = eol + 1;
		cur++;
	}
}

static int should_lookahead(struct grep_opt *opt)
{
	struct grep_pat *p;

	if (opt->extended)
		return 0; /* punt for too complex stuff */
	if (opt->invert)
		return 0;
	for (p = opt->pattern_list; p; p = p->next) {
		if (p->token != GREP_PATTERN)
			return 0; /* punt for "header only" and stuff */
	}
	return 1;
}

static int look_ahead(struct grep_opt *opt,
		      unsigned long *left_p,
		      unsigned *lno_p,
		      char **bol_p)
{
	unsigned lno = *lno_p;
	char *bol = *bol_p;
	struct grep_pat *p;
	char *sp, *last_bol;
	regoff_t earliest = -1;

	for (p = opt->pattern_list; p; p = p->next) {
		int hit;
		regmatch_t m;

		hit = patmatch(p, bol, bol + *left_p, &m, 0);
		if (!hit || m.rm_so < 0 || m.rm_eo < 0)
			continue;
		if (earliest < 0 || m.rm_so < earliest)
			earliest = m.rm_so;
	}

	if (earliest < 0) {
		*bol_p = bol + *left_p;
		*left_p = 0;
		return 1;
	}
	for (sp = bol + earliest; bol < sp && sp[-1] != '\n'; sp--)
		; /* find the beginning of the line */
	last_bol = sp;

	for (sp = bol; sp < last_bol; sp++) {
		if (*sp == '\n')
			lno++;
	}
	*left_p -= last_bol - bol;
	*bol_p = last_bol;
	*lno_p = lno;
	return 0;
}

static int fill_textconv_grep(struct repository *r,
			      struct userdiff_driver *driver,
			      struct grep_source *gs)
{
	struct diff_filespec *df;
	char *buf;
	size_t size;

	if (!driver || !driver->textconv)
		return grep_source_load(gs);

	/*
	 * The textconv interface is intimately tied to diff_filespecs, so we
	 * have to pretend to be one. If we could unify the grep_source
	 * and diff_filespec structs, this mess could just go away.
	 */
	df = alloc_filespec(gs->path);
	switch (gs->type) {
	case GREP_SOURCE_OID:
		fill_filespec(df, gs->identifier, 1, 0100644);
		break;
	case GREP_SOURCE_FILE:
		fill_filespec(df, &null_oid, 0, 0100644);
		break;
	default:
		BUG("attempt to textconv something without a path?");
	}

	/*
	 * fill_textconv is not remotely thread-safe; it may load objects
	 * behind the scenes, and it modifies the global diff tempfile
	 * structure.
	 */
	grep_read_lock();
	size = fill_textconv(r, driver, df, &buf);
	grep_read_unlock();
	free_filespec(df);

	/*
	 * The normal fill_textconv usage by the diff machinery would just keep
	 * the textconv'd buf separate from the diff_filespec. But much of the
	 * grep code passes around a grep_source and assumes that its "buf"
	 * pointer is the beginning of the thing we are searching. So let's
	 * install our textconv'd version into the grep_source, taking care not
	 * to leak any existing buffer.
	 */
	grep_source_clear_data(gs);
	gs->buf = buf;
	gs->size = size;

	return 0;
}

static int is_empty_line(const char *bol, const char *eol)
{
	while (bol < eol && isspace(*bol))
		bol++;
	return bol == eol;
}

static int grep_source_1(struct grep_opt *opt, struct grep_source *gs, int collect_hits)
{
	char *bol;
	char *peek_bol = NULL;
	unsigned long left;
	unsigned lno = 1;
	unsigned last_hit = 0;
	int binary_match_only = 0;
	unsigned count = 0;
	int try_lookahead = 0;
	int show_function = 0;
	struct userdiff_driver *textconv = NULL;
	enum grep_context ctx = GREP_CONTEXT_HEAD;
	xdemitconf_t xecfg;

	if (!opt->status_only && gs->name == NULL)
		BUG("grep call which could print a name requires "
		    "grep_source.name be non-NULL");

	if (!opt->output)
		opt->output = std_output;

	if (opt->pre_context || opt->post_context || opt->file_break ||
	    opt->funcbody) {
		/* Show hunk marks, except for the first file. */
		if (opt->last_shown)
			opt->show_hunk_mark = 1;
		/*
		 * If we're using threads then we can't easily identify
		 * the first file.  Always put hunk marks in that case
		 * and skip the very first one later in work_done().
		 */
		if (opt->output != std_output)
			opt->show_hunk_mark = 1;
	}
	opt->last_shown = 0;

	if (opt->allow_textconv) {
		grep_source_load_driver(gs, opt->repo->index);
		/*
		 * We might set up the shared textconv cache data here, which
		 * is not thread-safe.
		 */
		grep_attr_lock();
		textconv = userdiff_get_textconv(opt->repo, gs->driver);
		grep_attr_unlock();
	}

	/*
	 * We know the result of a textconv is text, so we only have to care
	 * about binary handling if we are not using it.
	 */
	if (!textconv) {
		switch (opt->binary) {
		case GREP_BINARY_DEFAULT:
			if (grep_source_is_binary(gs, opt->repo->index))
				binary_match_only = 1;
			break;
		case GREP_BINARY_NOMATCH:
			if (grep_source_is_binary(gs, opt->repo->index))
				return 0; /* Assume unmatch */
			break;
		case GREP_BINARY_TEXT:
			break;
		default:
			BUG("unknown binary handling mode");
		}
	}

	memset(&xecfg, 0, sizeof(xecfg));
	opt->priv = &xecfg;

	try_lookahead = should_lookahead(opt);

	if (fill_textconv_grep(opt->repo, textconv, gs) < 0)
		return 0;

	bol = gs->buf;
	left = gs->size;
	while (left) {
		char *eol, ch;
		int hit;
		ssize_t cno;
		ssize_t col = -1, icol = -1;

		/*
		 * look_ahead() skips quickly to the line that possibly
		 * has the next hit; don't call it if we need to do
		 * something more than just skipping the current line
		 * in response to an unmatch for the current line.  E.g.
		 * inside a post-context window, we will show the current
		 * line as a context around the previous hit when it
		 * doesn't hit.
		 */
		if (try_lookahead
		    && !(last_hit
			 && (show_function ||
			     lno <= last_hit + opt->post_context))
		    && look_ahead(opt, &left, &lno, &bol))
			break;
		eol = end_of_line(bol, &left);
		ch = *eol;
		*eol = 0;

		if ((ctx == GREP_CONTEXT_HEAD) && (eol == bol))
			ctx = GREP_CONTEXT_BODY;

		hit = match_line(opt, bol, eol, &col, &icol, ctx, collect_hits);
		*eol = ch;

		if (collect_hits)
			goto next_line;

		/* "grep -v -e foo -e bla" should list lines
		 * that do not have either, so inversion should
		 * be done outside.
		 */
		if (opt->invert)
			hit = !hit;
		if (opt->unmatch_name_only) {
			if (hit)
				return 0;
			goto next_line;
		}
		if (hit) {
			count++;
			if (opt->status_only)
				return 1;
			if (opt->name_only) {
				show_name(opt, gs->name);
				return 1;
			}
			if (opt->count)
				goto next_line;
			if (binary_match_only) {
				opt->output(opt, "Binary file ", 12);
				output_color(opt, gs->name, strlen(gs->name),
					     opt->colors[GREP_COLOR_FILENAME]);
				opt->output(opt, " matches\n", 9);
				return 1;
			}
			/* Hit at this line.  If we haven't shown the
			 * pre-context lines, we would need to show them.
			 */
			if (opt->pre_context || opt->funcbody)
				show_pre_context(opt, gs, bol, eol, lno);
			else if (opt->funcname)
				show_funcname_line(opt, gs, bol, lno);
			cno = opt->invert ? icol : col;
			if (cno < 0) {
				/*
				 * A negative cno indicates that there was no
				 * match on the line. We are thus inverted and
				 * being asked to show all lines that _don't_
				 * match a given expression. Therefore, set cno
				 * to 0 to suggest the whole line matches.
				 */
				cno = 0;
			}
			show_line(opt, bol, eol, gs->name, lno, cno + 1, ':');
			last_hit = lno;
			if (opt->funcbody)
				show_function = 1;
			goto next_line;
		}
		if (show_function && (!peek_bol || peek_bol < bol)) {
			unsigned long peek_left = left;
			char *peek_eol = eol;

			/*
			 * Trailing empty lines are not interesting.
			 * Peek past them to see if they belong to the
			 * body of the current function.
			 */
			peek_bol = bol;
			while (is_empty_line(peek_bol, peek_eol)) {
				peek_bol = peek_eol + 1;
				peek_eol = end_of_line(peek_bol, &peek_left);
			}

			if (match_funcname(opt, gs, peek_bol, peek_eol))
				show_function = 0;
		}
		if (show_function ||
		    (last_hit && lno <= last_hit + opt->post_context)) {
			/* If the last hit is within the post context,
			 * we need to show this line.
			 */
			show_line(opt, bol, eol, gs->name, lno, col + 1, '-');
		}

	next_line:
		bol = eol + 1;
		if (!left)
			break;
		left--;
		lno++;
	}

	if (collect_hits)
		return 0;

	if (opt->status_only)
		return opt->unmatch_name_only;
	if (opt->unmatch_name_only) {
		/* We did not see any hit, so we want to show this */
		show_name(opt, gs->name);
		return 1;
	}

	xdiff_clear_find_func(&xecfg);
	opt->priv = NULL;

	/* NEEDSWORK:
	 * The real "grep -c foo *.c" gives many "bar.c:0" lines,
	 * which feels mostly useless but sometimes useful.  Maybe
	 * make it another option?  For now suppress them.
	 */
	if (opt->count && count) {
		char buf[32];
		if (opt->pathname) {
			output_color(opt, gs->name, strlen(gs->name),
				     opt->colors[GREP_COLOR_FILENAME]);
			output_sep(opt, ':');
		}
		xsnprintf(buf, sizeof(buf), "%u\n", count);
		opt->output(opt, buf, strlen(buf));
		return 1;
	}
	return !!last_hit;
}

static void clr_hit_marker(struct grep_expr *x)
{
	/* All-hit markers are meaningful only at the very top level
	 * OR node.
	 */
	while (1) {
		x->hit = 0;
		if (x->node != GREP_NODE_OR)
			return;
		x->u.binary.left->hit = 0;
		x = x->u.binary.right;
	}
}

static int chk_hit_marker(struct grep_expr *x)
{
	/* Top level nodes have hit markers.  See if they all are hits */
	while (1) {
		if (x->node != GREP_NODE_OR)
			return x->hit;
		if (!x->u.binary.left->hit)
			return 0;
		x = x->u.binary.right;
	}
}

int grep_source(struct grep_opt *opt, struct grep_source *gs)
{
	/*
	 * we do not have to do the two-pass grep when we do not check
	 * buffer-wide "all-match".
	 */
	if (!opt->all_match)
		return grep_source_1(opt, gs, 0);

	/* Otherwise the toplevel "or" terms hit a bit differently.
	 * We first clear hit markers from them.
	 */
	clr_hit_marker(opt->pattern_expression);
	grep_source_1(opt, gs, 1);

	if (!chk_hit_marker(opt->pattern_expression))
		return 0;

	return grep_source_1(opt, gs, 0);
}

int grep_buffer(struct grep_opt *opt, char *buf, unsigned long size)
{
	struct grep_source gs;
	int r;

	grep_source_init(&gs, GREP_SOURCE_BUF, NULL, NULL, NULL);
	gs.buf = buf;
	gs.size = size;

	r = grep_source(opt, &gs);

	grep_source_clear(&gs);
	return r;
}

void grep_source_init(struct grep_source *gs, enum grep_source_type type,
		      const char *name, const char *path,
		      const void *identifier)
{
	gs->type = type;
	gs->name = xstrdup_or_null(name);
	gs->path = xstrdup_or_null(path);
	gs->buf = NULL;
	gs->size = 0;
	gs->driver = NULL;

	switch (type) {
	case GREP_SOURCE_FILE:
		gs->identifier = xstrdup(identifier);
		break;
	case GREP_SOURCE_OID:
		gs->identifier = oiddup(identifier);
		break;
	case GREP_SOURCE_BUF:
		gs->identifier = NULL;
		break;
	}
}

void grep_source_clear(struct grep_source *gs)
{
	FREE_AND_NULL(gs->name);
	FREE_AND_NULL(gs->path);
	FREE_AND_NULL(gs->identifier);
	grep_source_clear_data(gs);
}

void grep_source_clear_data(struct grep_source *gs)
{
	switch (gs->type) {
	case GREP_SOURCE_FILE:
	case GREP_SOURCE_OID:
		FREE_AND_NULL(gs->buf);
		gs->size = 0;
		break;
	case GREP_SOURCE_BUF:
		/* leave user-provided buf intact */
		break;
	}
}

static int grep_source_load_oid(struct grep_source *gs)
{
	enum object_type type;

	grep_read_lock();
	gs->buf = read_object_file(gs->identifier, &type, &gs->size);
	grep_read_unlock();

	if (!gs->buf)
		return error(_("'%s': unable to read %s"),
			     gs->name,
			     oid_to_hex(gs->identifier));
	return 0;
}

static int grep_source_load_file(struct grep_source *gs)
{
	const char *filename = gs->identifier;
	struct stat st;
	char *data;
	size_t size;
	int i;

	if (lstat(filename, &st) < 0) {
	err_ret:
		if (errno != ENOENT)
			error_errno(_("failed to stat '%s'"), filename);
		return -1;
	}
	if (!S_ISREG(st.st_mode))
		return -1;
	size = xsize_t(st.st_size);
	i = open(filename, O_RDONLY);
	if (i < 0)
		goto err_ret;
	data = xmallocz(size);
	if (st.st_size != read_in_full(i, data, size)) {
		error_errno(_("'%s': short read"), filename);
		close(i);
		free(data);
		return -1;
	}
	close(i);

	gs->buf = data;
	gs->size = size;
	return 0;
}

static int grep_source_load(struct grep_source *gs)
{
	if (gs->buf)
		return 0;

	switch (gs->type) {
	case GREP_SOURCE_FILE:
		return grep_source_load_file(gs);
	case GREP_SOURCE_OID:
		return grep_source_load_oid(gs);
	case GREP_SOURCE_BUF:
		return gs->buf ? 0 : -1;
	}
	BUG("invalid grep_source type to load");
}

void grep_source_load_driver(struct grep_source *gs,
			     struct index_state *istate)
{
	if (gs->driver)
		return;

	grep_attr_lock();
	if (gs->path)
		gs->driver = userdiff_find_by_path(istate, gs->path);
	if (!gs->driver)
		gs->driver = userdiff_find_by_name("default");
	grep_attr_unlock();
}

static int grep_source_is_binary(struct grep_source *gs,
				 struct index_state *istate)
{
	grep_source_load_driver(gs, istate);
	if (gs->driver->binary != -1)
		return gs->driver->binary;

	if (!grep_source_load(gs))
		return buffer_is_binary(gs->buf, gs->size);

	return 0;
}
