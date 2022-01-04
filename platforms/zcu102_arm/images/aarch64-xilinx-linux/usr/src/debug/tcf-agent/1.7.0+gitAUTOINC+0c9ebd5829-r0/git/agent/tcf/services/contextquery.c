/*******************************************************************************
 * Copyright (c) 2012, 2013 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 *******************************************************************************/

#include <tcf/config.h>

#if SERVICE_ContextQuery

#include <tcf/framework/json.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/contextquery.h>

typedef struct Comparator {
    char * attr_name;
    ContextQueryComparator * callback;
    struct Comparator * next;
} Comparator;

/* TODO: need hash table for faster search of query comparators */
static Comparator * comparators = NULL;

void add_context_query_comparator(const char * attr_name, ContextQueryComparator * callback) {
    Comparator * c = (Comparator *)loc_alloc_zero(sizeof(Comparator));
    c->attr_name = loc_strdup(attr_name);
    c->callback = callback;
    c->next = comparators;
    comparators = c;
}

static const char * CONTEXT_QUERY = "ContextQuery";

typedef struct Attribute {
    struct Attribute * next;
    struct Attribute * parent;
    char * name;
    char * value;
} Attribute;

static Attribute * attrs = NULL;
static char * str_buf = NULL;
static size_t str_pos = 0;
static size_t str_max = 0;
static int abs_path = 0;
static const char * query_attr_name = NULL;

static void add_char(char ch) {
    if (str_pos >= str_max) {
        str_max *= 2;
        str_buf = (char *)tmp_realloc(str_buf, str_max);
    }
    str_buf[str_pos++] = ch;
}

static int parse_wildcard(const char ** q) {
    const char * c = *q;
    str_pos = 0;
    add_char(*c++);
    if (*c == '*') add_char(*c++);
    if (*c != '\0' && *c != '/') {
        set_errno(ERR_OTHER, "Invalid context query syntax: * and ** are"
                             " the only valid wildcards");
        return -1;
    }
    *q = c;
    add_char('\0');
    return 0;
}

static int parse_number(const char ** q) {
    const char * c = *q;
    str_pos = 0;
    while (*c >= '0' && *c <= '9') add_char(*c++);
    if (*c != ',' && *c != '/' && *c != '\0' ) {
        set_errno(ERR_OTHER, "Invalid context query syntax: expecting [0-9] "
                             "or ',' or '/' after number");
        return -1;
    }

    *q = c;
    add_char('\0');
    return 0;
}

static int parse_symbol(const char **q) {
    const char * c = *q;
    str_pos = 0;
    while (*c) {
        if ((*c != '_') &&
            (((*c < '0') || (*c > '9')) &&
             ((*c < 'a') || (*c > 'z')) &&
             ((*c < 'A') || (*c > 'Z')))) {
            break;
        }
        add_char(*c++);
    }

    if (*c != '/' && *c != '=' && *c != ',' && *c != '\0') {
        set_errno(ERR_OTHER, "Invalid context query syntax:"
                  " unquoted strings must only contain"
                  " alphanumerical characters or '_'");
        return -1;
    }

    *q = c;
    add_char('\0');
    return 0;
}

static int parse_quoted_string(const char **q) {
    const char * c = *q;
    str_pos = 0;
    c++;
    while (*c != '"') {
        if (*c == '\\') {
            c++;
            if (*c != '\\' && *c != '"') {
                set_errno(ERR_OTHER, "Invalid context query syntax: \" and \\"
                          " are the only characters that can be escaped");
                return -1;
            }
        }
        else if (*c == '\0') {
            set_errno(ERR_OTHER, "Invalid context query syntax: missing closing"
                      " quote character");
            return -1;
        }
        add_char(*c++);
    }
    c++;
    *q = c;
    add_char('\0');
    return 0;
}

static int parse_string(const char **q) {
    const char * c = *q;

    if (*c == '"') {
        if (parse_quoted_string(&c) < 0) return -1;
    }
    else {
        if (parse_symbol(&c) < 0) return -1;
    }
    *q = c;
    return 0;
}

static int parse_value(const char **q) {
    const char * c = *q;

    if ((*c >= '0') && (*c <= '9')) {
        if (parse_number(&c) < 0) return -1;
    }
    else if (*c == '"') {
        if (parse_quoted_string(&c) < 0) return -1;
    }
    else {
        if (parse_symbol(&c) < 0) return -1;
    }
    *q = c;
    return 0;
}

static Attribute * parse_property(const char **q) {
    const char * c = *q;
    Attribute * attr = (Attribute *)tmp_alloc_zero(sizeof(Attribute));

    if (parse_string(&c) < 0) return NULL;
    attr->name = tmp_strdup(str_buf);

    if (*c == '=') {
        c++;
        if (*c == '/' || *c == '=' || *c == ',' || *c == '\0') {
            set_errno(ERR_OTHER, "Invalid context query syntax: missing value");
            return NULL;
        }

        if (parse_value(&c) < 0) return NULL;
        attr->value = tmp_strdup(str_buf);
        if (*c == '=') {
            set_errno(ERR_OTHER, "Invalid context query syntax: can't assign "
                                 "several values to a property at the same "
                                 "time");
            return NULL;
        }
    }
    if (attr->value == NULL) {
        attr->value = attr->name;
        attr->name = NULL;
    }
    *q = c;
    return attr;
}

int parse_context_query(const char * q) {
    Attribute * attr = NULL;

    str_pos = 0;
    str_buf = NULL;
    attrs = NULL;
    abs_path = 0;

    if (q == NULL || *q == '\0') return 0;

    str_max = 64;
    str_buf = (char *)tmp_alloc(str_max);
    if ((abs_path = *q == '/') != 0) q++;
    if (*q == '/' || *q == '=' || *q == ',' || *q == '\0') {
        set_errno(ERR_OTHER, "Invalid context query syntax: missing context "
                             "name, property or wildcard");
        return -1;
    }

    while (*q) {
        Attribute * a;
        str_pos = 0;
        if (*q == '*') {
            if (parse_wildcard(&q) < 0) return -1;
            a = (Attribute *) tmp_alloc_zero(sizeof(Attribute));
            a->value = tmp_strdup(str_buf);
        }
        else {
            a = parse_property(&q);
            if (a == NULL) return -1;
        }

        a->next = attr;
        attr = a;

        if (*q == '/') { /* start parsing a new part */
            attr->parent = attrs;
            attrs = attr;
            attr = NULL;
            q++;
            if (*q == '/' || *q == '=' || *q == ',' || *q == '\0') {
                set_errno(ERR_OTHER, "Invalid context query syntax: missing "
                                     "context name, property or wildcard");
                return -1;
            }
        }
        else if (*q == ',') { /* start parsing a new property */
            q++;
            if (*q == '/' || *q == '=' || *q == ',' || *q == '\0') {
                set_errno(ERR_OTHER, "Invalid context query syntax: "
                                     "missing property");
                return -1;
            }
        }
    }
    attr->parent = attrs;
    attrs = attr;

    return 0;
}

static int match_attribute(Context * ctx, const char * key, const char * val) {
    int res = 0;
    Comparator * c = comparators;
    query_attr_name = key;
    while (c != NULL) {
        if (strcasecmp(c->attr_name, key) == 0) {
            res = c->callback(ctx, val);
            break;
        }
        c = c->next;
    }
    if (c == NULL) {
        /* Comparator not found, check default comparators */
        c = comparators;
        while (c != NULL) {
            if (strcmp(c->attr_name, DEFAULT_CONTEXT_QUERY_COMPARATOR) == 0) {
                res = c->callback(ctx, val);
                if (res) break;
            }
            c = c->next;
        }
    }
    query_attr_name = NULL;
    return res;
}

static int match(Context * ctx, Attribute * attr, GetContextParent * get_parent) {
    Context * parent = get_parent(ctx);
    if (attr->name == NULL && strcmp(attr->value, "**") == 0) {
        if (attr->parent == NULL) return 1;
        if (match(ctx, attr->parent, get_parent)) return 1;
        while (parent != NULL) {
            ctx = parent;
            parent = get_parent(ctx);
            if (match(ctx, attr->parent, get_parent)) return 1;
        }
        return 0;
    }
    if (attr->parent != NULL && (parent == NULL || !match(parent, attr->parent, get_parent))) return 0;
    if (attr->parent == NULL && abs_path && parent != NULL) return 0;
    while (attr != NULL) {
        if (attr->name != NULL) {
            if (!match_attribute(ctx, attr->name, attr->value)) return 0;
        }
        else if (strcmp(attr->value, "*") != 0) {
            if (!match_attribute(ctx, "Name", attr->value)) return 0;
        }
        attr = attr->next;
    }
    return 1;
}

static Context * get_context_parent(Context * ctx) {
    return ctx->parent;
}

int run_context_query(Context * ctx) {
    return run_context_query_ext(ctx, get_context_parent);
}

int run_context_query_ext(Context * ctx, GetContextParent * get_parent) {
    if (attrs == NULL) return !abs_path;
    return match(ctx, attrs, get_parent);
}

const char * get_context_query_attr_name(void) {
    return query_attr_name;
}

int context_query(Context * ctx, const char * query) {
    parse_context_query(query);
    return run_context_query(ctx);
}

static void command_query(char * token, Channel * c) {
    int err = 0;
    char * query = json_read_alloc_string(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (parse_context_query(query) < 0) err = errno;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, err);
    write_stream(&c->out, '[');

    if (!err) {
        LINK * l;
        unsigned cnt = 0;
        for (l = context_root.next; l != &context_root; l = l->next) {
            Context * ctx = ctxl2ctxp(l);
            if (ctx->exited) continue;
            if (run_context_query(ctx)) {
                if (cnt > 0) write_stream(&c->out, ',');
                json_write_string(&c->out, ctx->id);
                cnt++;
            }
        }
    }

    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
    loc_free(query);
}

static void command_get_attr_names(char * token, Channel * c) {
    unsigned cnt = 0;
    Comparator * l;

    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, '[');

    l = comparators;
    while (l != NULL) {
        if (cnt > 0) write_stream(&c->out, ',');
        json_write_string(&c->out, l->attr_name);
        l = l->next;
        cnt++;
    }

    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static int cmp_id(Context * ctx, const char * v) {
    return strcmp(ctx->id, v) == 0;
}

static int cmp_name(Context * ctx, const char * v) {
    if (ctx->name != NULL) return strcmp(ctx->name, v) == 0;
    return strcmp(ctx->id, v) == 0;
}

void ini_context_query_service(Protocol * proto) {
    add_context_query_comparator("ID", cmp_id);
    add_context_query_comparator("Name", cmp_name);
    add_command_handler(proto, CONTEXT_QUERY, "query", command_query);
    add_command_handler(proto, CONTEXT_QUERY, "getAttrNames", command_get_attr_names);
}

#else

#include <tcf/services/contextquery.h>

void add_context_query_comparator(const char * attr_name, ContextQueryComparator * callback) {}
int parse_context_query(const char * query) { return 0; }
int run_context_query(Context * ctx) { return 0; }
int run_context_query_ext(Context * ctx, GetContextParent * get_parent) { return 0; }
int context_query(Context * ctx, const char * query) { return query == NULL || *query == 0; }

#endif
