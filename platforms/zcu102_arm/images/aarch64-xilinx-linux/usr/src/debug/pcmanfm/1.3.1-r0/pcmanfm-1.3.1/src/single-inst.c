/*
 *      single-inst.c: simple IPC mechanism for single instance app
 *
 *      Copyright 2010 Hong Jen Yee (PCMan) <pcman.tw@gmail.com>
 *      Copyright 2012-2018 Andriy Grytsenko (LStranger) <andrej@rep.kiev.ua>
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "single-inst.h"

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

typedef struct _SingleInstClient SingleInstClient;
struct _SingleInstClient
{
    GIOChannel* channel;
    char* cwd;
    int screen_num;
    GPtrArray* argv;
    const GOptionEntry* opt_entries;
    SingleInstCallback callback;
    guint watch;
};

static GList* clients = NULL;

static void get_socket_name(SingleInstData* data, char* buf, int len);
static gboolean on_server_socket_event(GIOChannel* ioc, GIOCondition cond, gpointer data);
static gboolean on_client_socket_event(GIOChannel* ioc, GIOCondition cond, gpointer user_data);

static void single_inst_client_free(SingleInstClient* client)
{
    g_io_channel_shutdown(client->channel, FALSE, NULL);
    g_io_channel_unref(client->channel);
    g_source_remove(client->watch);
    g_free(client->cwd);
    g_ptr_array_foreach(client->argv, (GFunc)g_free, NULL);
    g_ptr_array_free(client->argv, TRUE);
    g_slice_free(SingleInstClient, client);
    /* g_debug("free client"); */
}

/* FIXME: need to document IPC protocol format */
static void pass_args_to_existing_instance(const GOptionEntry* opt_entries, int screen_num, int sock)
{
    const GOptionEntry* ent;
    FILE* f = fdopen(sock, "w");
    char* escaped;

    /* pass cwd */
    char* cwd = g_get_current_dir();
    escaped = g_strescape(cwd, NULL);
    fprintf(f, "%s\n", escaped);
    g_free(cwd);
    cwd = escaped;

    /* pass screen number */
    fprintf(f, "%d\n", screen_num);

    for(ent = opt_entries; ent->long_name; ++ent)
    {
        switch(ent->arg)
        {
        case G_OPTION_ARG_NONE:
            if(*(gboolean*)ent->arg_data)
                fprintf(f, "--%s\n", ent->long_name);
            break;
        case G_OPTION_ARG_STRING:
        case G_OPTION_ARG_FILENAME:
        {
            char* str = *(char**)ent->arg_data;
            if(str && *str)
            {
                fprintf(f, "--%s\n", ent->long_name);
                if(g_str_has_prefix(str, "--")) /* strings begining with -- */
                    fprintf(f, "--\n"); /* prepend a -- to it */
                escaped = g_strescape(str, NULL);
                fprintf(f, "%s\n", escaped);
                g_free(escaped);
            }
            break;
        }
        case G_OPTION_ARG_INT:
        {
            gint value = *(gint*)ent->arg_data;
            if(value >= 0)
            {
                fprintf(f, "--%s\n%d\n", ent->long_name, value);
            }
            break;
        }
        case G_OPTION_ARG_STRING_ARRAY:
        case G_OPTION_ARG_FILENAME_ARRAY:
        {
            char** strv = *(char***)ent->arg_data;
            if(strv && *strv)
            {
                if(*ent->long_name) /* G_OPTION_REMAINING = "" */
                    fprintf(f, "--%s\n", ent->long_name);
                for(; *strv; ++strv)
                {
                    char* str = *strv;
                    /* if not absolute path and not URI then prepend cwd or $HOME */
                    if(str[0] == '~' && str[1] == '\0') ; /* pass "~" as is */
                    else if(str[0] == '~' && str[1] == '/')
                    {
                        const char *envvar = g_getenv("HOME");
                        if(envvar)
                        {
                            escaped = g_strescape(envvar, NULL);
                            fprintf(f, "%s", escaped);
                            g_free(escaped);
                            str++;
                        }
                    }
                    else if ((escaped = g_uri_parse_scheme(str))) /* a valid URI */
                        g_free(escaped);
                    else if(str[0] != '/')
                        fprintf(f, "%s/", cwd);
                    escaped = g_strescape(str, NULL);
                    fprintf(f, "%s\n", escaped);
                    g_free(escaped);
                }
            }
            break;
        }
        case G_OPTION_ARG_DOUBLE:
            fprintf(f, "--%s\n%lf\n", ent->long_name, *(gdouble*)ent->arg_data);
            break;
        case G_OPTION_ARG_INT64:
            fprintf(f, "--%s\n%lld\n", ent->long_name, (long long int)*(gint64*)ent->arg_data);
            break;
        case G_OPTION_ARG_CALLBACK:
            /* Not supported */
            break;
        }
    }
    fclose(f);
    g_free(cwd);
}

/**
 * single_inst_init
 * @data: data filled by caller
 * Return value: result from initialization
 *
 * Initializes single instance IPC, verifies if it's first instance and
 * returns result of initialization.
 * Data passed to single_inst_init() should be kept intact until next
 * mandatory call to single_inst_finalize().
 */
SingleInstResult single_inst_init(SingleInstData* data)
{
    struct sockaddr_un addr;
    int addr_len;
    int ret;
    int reuse;
    char *dir_sep;

    data->io_channel = NULL;
    data->io_watch = 0;
    if((data->sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
        return SINGLE_INST_ERROR;

    /* FIXME: use abstract socket? */
    addr.sun_family = AF_UNIX;
    get_socket_name(data, addr.sun_path, sizeof(addr.sun_path));
#ifdef SUN_LEN
    addr_len = SUN_LEN(&addr);
#else
    addr_len = strlen(addr.sun_path) + sizeof(addr.sun_family);
#endif

    /* try to connect to existing instance */
    if(connect(data->sock, (struct sockaddr*)&addr, addr_len) == 0)
    {
        /* connected successfully, pass args in opt_entries to server process as argv and exit. */
        pass_args_to_existing_instance(data->opt_entries, data->screen_num, data->sock);
        return SINGLE_INST_CLIENT;
    }

    /* There is no existing server, and we are in the first instance. */
    unlink(addr.sun_path); /* delete old socket file if it exists. */

    /* root-instance issue: /root/.cache might not exist, see
       https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=874753 */
    dir_sep = strrchr(addr.sun_path, '/');
    if (dir_sep)
    {
        *dir_sep = '\0';
        g_mkdir_with_parents(addr.sun_path, 0700);
        *dir_sep = '/';
    }

    reuse = 1;
    ret = setsockopt( data->sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse) );
    if(ret || bind(data->sock, (struct sockaddr*)&addr, addr_len) == -1)
        return SINGLE_INST_ERROR;

    data->io_channel = g_io_channel_unix_new(data->sock);
    if(data->io_channel == NULL)
        return SINGLE_INST_ERROR;

    g_io_channel_set_encoding(data->io_channel, NULL, NULL);
    g_io_channel_set_buffered(data->io_channel, FALSE);

    if(listen(data->sock, 5) == -1)
        return SINGLE_INST_ERROR;

    data->io_watch = g_io_add_watch(data->io_channel,
                                    G_IO_IN|G_IO_ERR|G_IO_PRI|G_IO_HUP,
                                    (GIOFunc)on_server_socket_event, data);
    return SINGLE_INST_SERVER;
}

/**
 * single_inst_finalize
 * @data: data filled by call to single_inst_init()
 *
 * Terminates single instance IPC started by single_inst_init(). Should be
 * always called after single_inst_init().
 */
void single_inst_finalize(SingleInstData* data)
{
    if(data->sock >=0)
    {
        close(data->sock);
        data->sock = -1;

        if(data->io_channel)
        {
            char sock_path[256];

            /* disconnect all clients */
            if(clients)
            {
                g_list_foreach(clients, (GFunc)single_inst_client_free, NULL);
                g_list_free(clients);
                clients = NULL;
            }

            if(data->io_watch)
            {
                g_source_remove(data->io_watch);
                data->io_watch = 0;
            }
            g_io_channel_unref(data->io_channel);
            data->io_channel = NULL;
            /* remove the file */
            get_socket_name(data, sock_path, 256);
            unlink(sock_path);
        }
    }
}

static inline void parse_args(SingleInstClient* client)
{
    GOptionContext* ctx = g_option_context_new("");
    int argc = client->argv->len;
    char** argv = g_new(char*, argc + 1);
    memcpy(argv, client->argv->pdata, sizeof(char*) * argc);
    argv[argc] = NULL;
    g_option_context_add_main_entries(ctx, client->opt_entries, NULL);
    g_option_context_parse(ctx, &argc, &argv, NULL);
    g_free(argv);
    g_option_context_free(ctx);
    if(client->callback)
        client->callback(client->cwd, client->screen_num);
}

static gboolean on_client_socket_event(GIOChannel* ioc, GIOCondition cond, gpointer user_data)
{
    SingleInstClient* client = (SingleInstClient*)user_data;

    if ( cond & (G_IO_IN|G_IO_PRI) )
    {
        GString *str = g_string_sized_new(1024);
        gsize got;
        gchar ch;
        GIOStatus status;

        while((status = g_io_channel_read_chars(ioc, &ch, 1, &got, NULL)) == G_IO_STATUS_NORMAL)
        {
            if(ch != '\n')
            {
                if(ch < 0x20) /* zero or control char */
                {
                    g_error("client connection: invalid char %#x", (int)ch);
                    break;
                }
                g_string_append_c(str, ch);
                continue;
            }
            if(str->len)
            {
                char *line = g_strndup(str->str, str->len);

                g_string_truncate(str, 0);
                g_debug("line = %s", line);
                if(!client->cwd)
                    client->cwd = g_strcompress(line);
                else if(client->screen_num == -1)
                {
                    client->screen_num = atoi(line);
                    if(client->screen_num < 0)
                        client->screen_num = 0;
                }
                else
                {
                    char* str = g_strcompress(line);
                    g_ptr_array_add(client->argv, str);
                }
                g_free(line);
            }
        }
        g_string_free(str, TRUE);
        switch(status)
        {
            case G_IO_STATUS_ERROR:
                cond |= G_IO_ERR;
                break;
            case G_IO_STATUS_EOF:
                cond |= G_IO_HUP;
            default:
                break;
        }
    }

    if(cond & (G_IO_ERR|G_IO_HUP))
    {
        if(! (cond & G_IO_ERR) ) /* if there is no error */
        {
            /* try to parse argv */
            parse_args(client);
        }
        clients = g_list_remove(clients, client);
        single_inst_client_free(client);
        return FALSE;
    }

    return TRUE;
}

static gboolean on_server_socket_event(GIOChannel* ioc, GIOCondition cond, gpointer user_data)
{
    SingleInstData* data = user_data;

    if ( cond & (G_IO_IN|G_IO_PRI) )
    {
        int client_sock = accept(g_io_channel_unix_get_fd(ioc), NULL, 0);
        if(client_sock != -1)
        {
            SingleInstClient* client = g_slice_new0(SingleInstClient);
            client->channel = g_io_channel_unix_new(client_sock);
            g_io_channel_set_encoding(client->channel, NULL, NULL);
            client->screen_num = -1;
            client->argv = g_ptr_array_new();
            client->callback = data->cb;
            client->opt_entries = data->opt_entries;
            g_ptr_array_add(client->argv, g_strdup(g_get_prgname()));
            client->watch = g_io_add_watch(client->channel, G_IO_IN|G_IO_PRI|G_IO_ERR|G_IO_HUP,
                                           on_client_socket_event, client);
            clients = g_list_prepend(clients, client);
            /* g_debug("accept new client"); */
        }
        else
            g_debug("accept() failed!\n%s", g_strerror(errno));
    }

    if(cond & (G_IO_ERR|G_IO_HUP))
    {
        single_inst_finalize(data);
        single_inst_init(data);
        return FALSE;
    }

    return TRUE;
}

static void get_socket_name(SingleInstData* data, char* buf, int len)
{
    const char* dpy = g_getenv("DISPLAY");
    char* host = NULL;
    int dpynum;
    if(dpy)
    {
        const char* p = strrchr(dpy, ':');
        host = g_strndup(dpy, (p - dpy));
        dpynum = atoi(p + 1);
    }
    else
        dpynum = 0;
#if GLIB_CHECK_VERSION(2, 28, 0)
    g_snprintf(buf, len, "%s/%s-socket-%s-%d", g_get_user_runtime_dir(),
               data->prog_name, host ? host : "", dpynum);
#else
    g_snprintf(buf, len, "%s/.%s-socket-%s-%d-%s",
                g_get_tmp_dir(),
                data->prog_name,
                host ? host : "",
                dpynum,
                g_get_user_name());
#endif
}

