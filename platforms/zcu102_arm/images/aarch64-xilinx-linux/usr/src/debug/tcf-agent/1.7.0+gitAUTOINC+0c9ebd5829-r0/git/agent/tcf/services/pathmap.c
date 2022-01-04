/*******************************************************************************
 * Copyright (c) 2010, 2013 Wind River Systems, Inc. and others.
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

/*
 * Path Map service.
 * The service manages file path mapping rules.
 */

#include <tcf/config.h>
#include <stdio.h>
#include <sys/stat.h>
#include <assert.h>
#include <tcf/framework/mdep-fs.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/proxy.h>
#include <tcf/services/contextquery.h>
#include <tcf/services/pathmap.h>

#ifndef ENABLE_CaseInsensitivePathMap
#define ENABLE_CaseInsensitivePathMap 0
#endif

static int is_separator(const char c) {
    return ((c == '/') || (c == '\\'));
}

char * canonic_path_map_file_name(const char * fnm) {
    size_t buf_pos = 0;
    size_t buf_max = 0x100;
    char * buf = (char *)tmp_alloc(buf_max);

    for (;;) {
        char ch = *fnm++;
        if (ch == 0) break;
        if (ch == '\\') ch = '/';
        if (ch == '/' && buf_pos >= 2 && buf[buf_pos - 1] == '/') continue;
        if (ch == '/' && *fnm == 0 && buf_pos > 0 && buf[buf_pos - 1] != ':') break;
        if (ch == '.' && (buf_pos == 0 || buf[buf_pos - 1] == '/')) {
            if (is_separator(*fnm)) {
                fnm++;
                continue;
            }
            if (buf_pos > 0 && *fnm == '.' && is_separator(fnm[1])) {
                size_t j = buf_pos - 1;
                if (j > 0 && buf[j - 1] != '/') {
                    buf[buf_pos] = 0;
                    while (j > 0 && buf[j - 1] != '/') j--;
                    if (strcmp(buf + j, "./") && strcmp(buf + j, "../")) {
                        buf_pos = j;
                        fnm += 2;
                        continue;
                    }
                }
            }
        }
        if (buf_pos == 0 && ch >= 'a' && ch <= 'z' && *fnm == ':') {
            ch = (char)(ch - 'a' + 'A');
        }
        if (buf_pos + 1 >= buf_max) {
            buf_max += 0x100;
            buf = (char *)tmp_realloc(buf, buf_max);
        }
        buf[buf_pos++] = ch;
    }
    buf[buf_pos] = 0;
    return buf;
}

int is_absolute_path(const char * fnm) {
    if (fnm[0] == '/') return 1;
    if (fnm[0] == '\\') return 1;
    if (fnm[0] != 0 && fnm[1] == ':') {
        if (fnm[2] == '/') return 1;
        if (fnm[2] == '\\') return 1;
    }
    return 0;
}

#if SERVICE_PathMap

#include <tcf/framework/json.h>
#include <tcf/framework/events.h>
#include <tcf/framework/exceptions.h>

typedef struct Listener Listener;
typedef struct PathMap PathMap;

struct Listener {
    PathMapEventListener * listener;
    void * args;
};

struct PathMapRule {
    PathMapRuleAttribute * attrs;
    char * src;
    char * dst;
    char * host;
    char * prot;
    char * query;
    char * ctx;
};

struct PathMap {
    LINK maps;
    Channel * channel;
    PathMapRule * rules;
    unsigned rules_cnt;
    unsigned rules_max;
};

#define maps2map(x) ((PathMap *)((char *)(x) - offsetof(PathMap, maps)))

static const char PATH_MAP[] = "PathMap";

static LINK maps = TCF_LIST_INIT(maps);
static char host_name[256];

static Listener * listeners = NULL;
static unsigned listener_cnt = 0;
static unsigned listener_max = 0;

static TCFBroadcastGroup * broadcast_group = NULL;

static void event_path_map_changed(void) {
    OutputStream * out = &broadcast_group->out;

    write_stringz(out, "E");
    write_stringz(out, PATH_MAP);
    write_stringz(out, "changed");

    write_stream(out, MARKER_EOM);
}

static void path_map_event_mapping_changed(Channel * c) {
    unsigned i;
    event_path_map_changed();
    for (i = 0; i < listener_cnt; i++) {
        Listener * l = listeners + i;
        if (l->listener->mapping_changed == NULL) continue;
        l->listener->mapping_changed(c, l->args);
    }
}

void add_path_map_event_listener(PathMapEventListener * listener, void * args) {
    Listener * l;
    if (listener_cnt >= listener_max) {
        listener_max += 8;
        listeners = (Listener *)loc_realloc(listeners, listener_max * sizeof(Listener));
    }
    l = listeners + listener_cnt++;
    l->listener = listener;
    l->args = args;
}

void rem_path_map_event_listener(PathMapEventListener * listener) {
    unsigned i = 0;
    while (i < listener_cnt) {
        if (listeners[i++].listener == listener) {
            while (i < listener_cnt) {
                listeners[i - 1] = listeners[i];
                i++;
            }
            listener_cnt--;
            break;
        }
    }
}

static PathMap * find_map(Channel * c) {
    LINK * l;
    for (l = maps.next; l != &maps; l = l->next) {
        PathMap * m = maps2map(l);
        if (m->channel == c) return m;
    }
    return NULL;
}

static void flush_host_name(void * args) {
    memset(host_name, 0, sizeof(host_name));
}

static int is_my_host(char * host) {
    if (host == NULL || host[0] == 0) return 1;
    if (host_name[0] == 0) {
        gethostname(host_name, sizeof(host_name));
        if (host_name[0] != 0) post_event_with_delay(flush_host_name, NULL, 1000000);
    }
    return strcasecmp(host, host_name) == 0;
}

static void free_rule(PathMapRule * r) {
    loc_free(r->src);
    loc_free(r->dst);
    loc_free(r->host);
    loc_free(r->prot);
    loc_free(r->query);
    loc_free(r->ctx);
    while (r->attrs != NULL) {
        PathMapRuleAttribute * attr = r->attrs;
        r->attrs = attr->next;
        loc_free(attr->name);
        loc_free(attr->value);
        loc_free(attr);
    }
    memset(r, 0, sizeof(PathMapRule));
}

static int update_rule(PathMapRule * r, PathMapRuleAttribute * new_attrs) {
    int diff = 0;
    PathMapRuleAttribute * old_attrs = r->attrs;
    PathMapRuleAttribute ** new_ref = &r->attrs;
    r->attrs = NULL;

    while (new_attrs != NULL) {
        PathMapRuleAttribute * new_attr = new_attrs;
        PathMapRuleAttribute * old_attr = old_attrs;
        PathMapRuleAttribute ** old_ref = &old_attrs;
        InputStream * buf_inp;
        ByteArrayInputStream buf;
        char * name = new_attr->name;
        int unsupported_attr = 0;

        new_attrs = new_attr->next;
        new_attr->next = NULL;
        while (old_attr && strcmp(old_attr->name, name)) {
            old_ref = &old_attr->next;
            old_attr = old_attr->next;
        }

        if (old_attr != NULL) {
            assert(old_attr == *old_ref);
            *old_ref = old_attr->next;
            old_attr->next = NULL;
            if (strcmp(old_attr->value, new_attr->value) == 0) {
                *new_ref = old_attr;
                new_ref = &old_attr->next;
                loc_free(new_attr->value);
                loc_free(new_attr->name);
                loc_free(new_attr);
                continue;
            }
            loc_free(old_attr->value);
            loc_free(old_attr->name);
            loc_free(old_attr);
        }

        *new_ref = new_attr;
        new_ref = &new_attr->next;
        diff++;

        buf_inp = create_byte_array_input_stream(&buf, new_attr->value, strlen(new_attr->value));

        if (strcmp(name, PATH_MAP_SOURCE) == 0) {
            loc_free(r->src);
            r->src = json_read_alloc_string(buf_inp);
        }
        else if (strcmp(name, PATH_MAP_DESTINATION) == 0) {
            loc_free(r->dst);
            r->dst = json_read_alloc_string(buf_inp);
        }
        else if (strcmp(name, PATH_MAP_PROTOCOL) == 0) {
            loc_free(r->prot);
            r->prot = json_read_alloc_string(buf_inp);
        }
        else if (strcmp(name, PATH_MAP_HOST) == 0) {
            loc_free(r->host);
            r->host = json_read_alloc_string(buf_inp);
        }
        else if (strcmp(name, PATH_MAP_CONTEXT_QUERY) == 0) {
            loc_free(r->query);
            r->query = json_read_alloc_string(buf_inp);
        }
        else if (strcmp(name, PATH_MAP_CONTEXT) == 0) {
            loc_free(r->ctx);
            r->ctx = json_read_alloc_string(buf_inp);
        }
        else {
            unsupported_attr = 1;
        }
        if (!unsupported_attr) json_test_char(buf_inp, MARKER_EOS);
    }

    while (old_attrs != NULL) {
        PathMapRuleAttribute * old_attr = old_attrs;
        char * name = old_attr->name;
        old_attrs = old_attr->next;

        if (strcmp(name, PATH_MAP_SOURCE) == 0) {
            loc_free(r->src);
            r->src = NULL;
        }
        else if (strcmp(name, PATH_MAP_DESTINATION) == 0) {
            loc_free(r->dst);
            r->dst = NULL;
        }
        else if (strcmp(name, PATH_MAP_PROTOCOL) == 0) {
            loc_free(r->prot);
            r->prot = NULL;
        }
        else if (strcmp(name, PATH_MAP_HOST) == 0) {
            loc_free(r->host);
            r->host = NULL;
        }
        else if (strcmp(name, PATH_MAP_CONTEXT_QUERY) == 0) {
            loc_free(r->query);
            r->query = NULL;
        }
        else if (strcmp(name, PATH_MAP_CONTEXT) == 0) {
            loc_free(r->ctx);
            r->ctx = NULL;
        }

        loc_free(old_attr->value);
        loc_free(old_attr->name);
        loc_free(old_attr);
        diff++;
    }

    return diff;
}

static char * map_file_name(Context * ctx, PathMap * m, char * fnm, int mode) {
    unsigned i, k;

    for (i = 0; i < m->rules_cnt; i++) {
        PathMapRule * r = m->rules + i;
        char * src;
        char * buf;
        struct stat st;
        if (r->src == NULL) continue;
        if (r->dst == NULL) continue;
        if (r->prot != NULL && strcasecmp(r->prot, "file")) continue;
        switch (mode) {
        case PATH_MAP_TO_LOCAL:
            if (r->host != NULL && !is_my_host(r->host)) continue;
            break;
        }
        if (r->ctx != NULL) {
            int ok = 0;
#if ENABLE_DebugContext
            if (ctx != NULL) {
                Context * syms = context_get_group(ctx, CONTEXT_GROUP_SYMBOLS);
                if (syms != NULL) {
                    ok = strcmp(r->ctx, syms->id) == 0;
                    if (!ok && syms->name != NULL) {
                        ok = strcmp(r->ctx, syms->name) == 0;
                        if (!ok) ok = strcmp(r->ctx, context_full_name(syms)) == 0;
                    }
                }
            }
#endif
            if (!ok) continue;
        }
        if (r->query != NULL) {
            if (ctx == NULL) continue;
            if (!context_query(ctx, r->query)) continue;
        }
        src = canonic_path_map_file_name(r->src);
        k = (unsigned)strlen(src);
#if ENABLE_CaseInsensitivePathMap
        if (strncasecmp(src, fnm, k)) continue;
#else
        if (strncmp(src, fnm, k)) continue;
#endif

        if (fnm[k] == 0) {
            /* perfect match */
            buf = tmp_strdup(r->dst);
        }
        else {
            const size_t dst_len = strlen(r->dst);
            const char last_dest_char = dst_len == 0 ? 0 : r->dst[dst_len - 1];
            if (!is_separator(fnm[k])) {
                if (!is_separator(last_dest_char)) {
                    const char last_src_char = k == 0 ? 0 : r->src[k - 1];
                    if (!is_separator(last_src_char))
                        /* prevent matching mid-filename */
                        continue;
                }
                /* re-add initial path separator */
                --k;
            }
            else if (is_separator(last_dest_char)) {
                /* strip extra path separator */
                ++k;
            }
            buf = tmp_strdup2(r->dst, fnm + k);
        }

        if (mode != PATH_MAP_TO_LOCAL || stat(buf, &st) == 0) return buf;
    }

    return fnm;
}

char * apply_path_map(Channel * c, Context * ctx, char * fnm, int mode) {
    char * cnm = canonic_path_map_file_name(fnm);
    if (c == NULL) {
        LINK * l = maps.next;
        while (l != &maps) {
            PathMap * m = maps2map(l);
            char * lnm = map_file_name(ctx, m, cnm, mode);
            if (lnm != cnm) return lnm;
            l = l->next;
        }
    }
    else {
        PathMap * m;
#if ENABLE_ContextProxy
        Channel * h = proxy_get_host_channel(c);
        if (h != NULL) {
            m = find_map(h);
            if (m != NULL) {
                char * lnm = map_file_name(ctx, m, cnm, mode);
                if (lnm != cnm) return lnm;
            }
        }
#endif
        m = find_map(c);
        if (m != NULL) {
            char * lnm = map_file_name(ctx, m, cnm, mode);
            if (lnm != cnm) return lnm;
        }
    }
    return fnm;
}

void iterate_path_map_rules(Channel * channel, IteratePathMapsCallBack * callback, void * args) {
    PathMap * m = find_map(channel);
    if (m != NULL) {
        unsigned i;
        for (i = 0; i < m->rules_cnt; i++) {
            callback(m->rules + i, args);
        }
    }
}

PathMapRuleAttribute * get_path_mapping_attributes(PathMapRule * map) {
    return map->attrs;
}

PathMapRule * create_path_mapping(PathMapRuleAttribute * attrs) {
    PathMapRule * r;
    PathMap * m = find_map(NULL);

    if (m == NULL) {
        m = (PathMap *)loc_alloc_zero(sizeof(PathMap));
        list_add_first(&m->maps, &maps);
    }
    if (m->rules_cnt >= m->rules_max) {
        m->rules_max = m->rules_max ? m->rules_max * 2 : 8;
        m->rules = (PathMapRule *)loc_realloc(m->rules, m->rules_max * sizeof(*m->rules));
    }

    r = m->rules + m->rules_cnt++;
    memset(r, 0, sizeof(*r));
    if (update_rule(r, attrs)) path_map_event_mapping_changed(NULL);
    return r;
}

void change_path_mapping_attributes(PathMapRule * r, PathMapRuleAttribute * attrs) {
    if (update_rule(r, attrs)) path_map_event_mapping_changed(NULL);
}

void delete_path_mapping(PathMapRule * r) {
    LINK * l;
    for (l = maps.next; l != &maps; l = l->next) {
        PathMap * m = maps2map(l);
        if (m->channel == NULL && r >= m->rules && r < m->rules + m->rules_cnt) {
            free_rule(r);
            memmove(r, r + 1, (m->rules_cnt - (r - m->rules) - 1) * sizeof(PathMapRule));
            m->rules_cnt--;
            path_map_event_mapping_changed(NULL);
            break;
        }
    }
}

void delete_all_path_mappings(void) {
    LINK * l;
    for (l = maps.next; l != &maps; l = l->next) {
        PathMap * m = maps2map(l);
        if (m->channel == NULL && m->rules_cnt > 0) {
            unsigned i;
            for (i = 0; i < m->rules_cnt; i++) {
                free_rule(m->rules + i);
            }
            m->rules_cnt = 0;
            path_map_event_mapping_changed(NULL);
            break;
        }
    }
}

static void write_rule(OutputStream * out, PathMapRule * r) {
    unsigned i = 0;
    PathMapRuleAttribute * attr = r->attrs;

    write_stream(out, '{');
    while (attr != NULL) {
        if (i > 0) write_stream(out, ',');
        json_write_string(out, attr->name);
        write_stream(out, ':');
        write_string(out, attr->value);
        attr = attr->next;
        i++;
    }
    write_stream(out, '}');
}

static void read_rule_attrs(InputStream * inp, const char * name, void * args) {
    PathMapRuleAttribute *** list = (PathMapRuleAttribute ***)args;
    PathMapRuleAttribute * attr = (PathMapRuleAttribute *)loc_alloc_zero(sizeof(PathMapRuleAttribute));

    attr->name = loc_strdup(name);
    attr->value = json_read_object(inp);
    **list = attr;
    *list = &attr->next;
}

typedef struct {
    Channel * channel;
    PathMap * map;
    unsigned cnt;
    int diff;
} ReadRuleState;

static void read_rule(InputStream * inp, void * args) {
    ReadRuleState * s = (ReadRuleState *)args;
    PathMap * m = s->map;
    PathMapRule * r;
    PathMapRuleAttribute * attrs = NULL;
    PathMapRuleAttribute ** attr_list = &attrs;

    if (m == NULL) {
        m = s->map = (PathMap *)loc_alloc_zero(sizeof(PathMap));
        m->channel = s->channel;
        list_add_first(&m->maps, &maps);
    }

    if (s->cnt >= m->rules_max) {
        m->rules_max = m->rules_max ? m->rules_max * 2 : 8;
        m->rules = (PathMapRule *)loc_realloc(m->rules, m->rules_max * sizeof(*m->rules));
    }

    r = m->rules + s->cnt++;
    if (s->cnt > m->rules_cnt) {
        memset(r, 0, sizeof(*r));
        m->rules_cnt++;
        s->diff = 1;
    }
    assert(s->cnt <= m->rules_cnt);
    assert(s->cnt <= m->rules_max);
    json_read_struct(inp, read_rule_attrs, &attr_list);
    if (update_rule(r, attrs)) s->diff = 1;
}

void set_path_map(Channel * c, InputStream * inp) {
    ReadRuleState s;

    memset(&s, 0, sizeof(s));
    s.channel = c;
    s.map = find_map(c);

    json_read_array(inp, read_rule, &s);
    if (s.map != NULL && s.map->rules_cnt > s.cnt) {
        unsigned i;
        for (i = s.cnt; i < s.map->rules_cnt; i++) free_rule(s.map->rules + i);
        s.map->rules_cnt = s.cnt;
        if (s.cnt == 0) {
            list_remove(&s.map->maps);
            loc_free(s.map->rules);
            loc_free(s.map);
            s.map = NULL;
        }
        s.diff = 1;
    }
    if (s.diff) path_map_event_mapping_changed(c);
}

static void command_get(char * token, Channel * c) {
    unsigned n = 0;
    LINK * l = maps.next;

    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, '[');
    while (l != &maps) {
        unsigned i;
        PathMap * m = maps2map(l);
        for (i = 0; i < m->rules_cnt; i++) {
            PathMapRule * r = m->rules + i;
            if (n++ > 0) write_stream(&c->out, ',');
            write_rule(&c->out, r);
        }
        l = l->next;
    }
    write_stream(&c->out, ']');
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_set(char * token, Channel * c) {
    set_path_map(c, &c->inp);

    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_errno(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void channel_close_listener(Channel * c) {
    unsigned i;
    PathMap * m;
    /* Keep path map over channel redirection */
    if (c->state == ChannelStateHelloReceived) return;
    m = find_map(c);
    if (m == NULL) return;
    list_remove(&m->maps);
    if (m->rules_cnt > 0) path_map_event_mapping_changed(c);
    for (i = 0; i < m->rules_cnt; i++) free_rule(m->rules + i);
    loc_free(m->rules);
    loc_free(m);
}

void ini_path_map_service(Protocol * proto, TCFBroadcastGroup * bcg) {
    static int ini_done = 0;
    if (!ini_done) {
        ini_done = 1;
        add_channel_close_listener(channel_close_listener);
        broadcast_group = bcg;
    }
    assert(broadcast_group == bcg);
    add_command_handler(proto, PATH_MAP, "get", command_get);
    add_command_handler(proto, PATH_MAP, "set", command_set);
}

#endif /* SERVICE_PathMap */
