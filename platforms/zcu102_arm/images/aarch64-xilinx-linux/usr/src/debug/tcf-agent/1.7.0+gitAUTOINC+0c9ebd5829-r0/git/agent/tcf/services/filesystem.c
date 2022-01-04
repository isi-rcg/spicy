/*******************************************************************************
 * Copyright (c) 2007-2019 Wind River Systems, Inc. and others.
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
 * Target service implementation: file system access (TCF name FileSystem)
 */

#include <tcf/config.h>

#if SERVICE_FileSystem

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#if defined(__CYGWIN__)
#  include <ctype.h>
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
#  include <Windows.h>
#endif
#if defined(_WRS_KERNEL)
#  include <ioLib.h>
#endif
#include <tcf/framework/mdep-fs.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/streams.h>
#include <tcf/framework/channel.h>
#include <tcf/framework/link.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/json.h>
#include <tcf/framework/exceptions.h>
#include <tcf/services/filesystem.h>

#define BUF_SIZE (128 * MEM_USAGE_FACTOR)
#define DIR_BUF_SIZE 64

static const char * FILE_SYSTEM = "FileSystem";

static const int
    ATTR_SIZE               = 0x00000001,
    ATTR_UIDGID             = 0x00000002,
    ATTR_PERMISSIONS        = 0x00000004,
    ATTR_ACMODTIME          = 0x00000008;

typedef struct OpenFileInfo OpenFileInfo;
typedef struct IORequest IORequest;
typedef struct FileAttrs FileAttrs;

struct FileAttrs {
    int flags;
    int64_t size;
    int uid;
    int gid;
    int permissions;
    uint64_t atime;
    uint64_t mtime;
#if defined(_WIN32) || defined(__CYGWIN__)
    DWORD win32_attrs;
#endif
};

struct OpenFileInfo {
    unsigned long handle;
    char path[FILE_PATH_SIZE];
    int file;
    DIR * dir;
    InputStream * inp;
    OutputStream * out;
    LINK link_ring;
    LINK link_hash;
    LINK link_reqs;
    IORequest * posted_req;
};

struct IORequest {
    char token[256];
    OpenFileInfo * handle;
    AsyncReqInfo info;
    LINK link_reqs;
};

#define hash2file(A)    ((OpenFileInfo *)((char *)(A) - offsetof(OpenFileInfo, link_hash)))
#define ring2file(A)    ((OpenFileInfo *)((char *)(A) - offsetof(OpenFileInfo, link_ring)))
#define reqs2req(A)     ((IORequest *)((char *)(A) - offsetof(IORequest, link_reqs)))

static unsigned long handle_cnt = 0;

#define HANDLE_HASH_SIZE (4 * MEM_USAGE_FACTOR - 1)
static LINK handle_hash[HANDLE_HASH_SIZE];
static LINK file_info_ring = TCF_LIST_INIT(file_info_ring);

static OpenFileInfo * create_open_file_info(Channel * ch, char * path, int file, DIR * dir) {
    LINK * list_head = NULL;

    OpenFileInfo * h = (OpenFileInfo *)loc_alloc_zero(sizeof(OpenFileInfo));
    for (;;) {
        LINK * list_next;
        OpenFileInfo * p = NULL;
        h->handle = handle_cnt++;
        list_head = &handle_hash[h->handle % HANDLE_HASH_SIZE];
        for (list_next = list_head->next; list_next != list_head; list_next = list_next->next) {
            if (hash2file(list_next)->handle == h->handle) {
                p = hash2file(list_next);
                break;
            }
        }
        if (p == NULL) break;
    }
    if (path != NULL) strlcpy(h->path, path, sizeof(h->path));
    h->file = file;
    h->dir = dir;
    h->inp = &ch->inp;
    h->out = &ch->out;
    list_add_first(&h->link_ring, &file_info_ring);
    list_add_first(&h->link_hash, list_head);
    list_init(&h->link_reqs);
    return h;
}

static OpenFileInfo * find_open_file_info(char * id) {
    unsigned long handle;
    LINK * list_head;
    LINK * list_next;

    if (id == NULL || id[0] != 'F' || id[1] != 'S' || id[2] == 0) return NULL;
    handle = strtoul(id + 2, &id, 10);
    if (id[0] != 0) return NULL;
    list_head = &handle_hash[handle % HANDLE_HASH_SIZE];
    for (list_next = list_head->next; list_next != list_head; list_next = list_next->next) {
        if (hash2file(list_next)->handle == handle) return hash2file(list_next);
    }
    return NULL;
}

static void delete_open_file_info(OpenFileInfo * h) {
    assert(list_is_empty(&h->link_reqs));
    list_remove(&h->link_ring);
    list_remove(&h->link_hash);
    loc_free(h);
}

static void free_io_req(IORequest * req) {
    switch (req->info.type) {
    case AsyncReqStat:
    case AsyncReqLstat:
    case AsyncReqFstat:
    case AsyncReqFSetStat:
    case AsyncReqSetStat:
    case AsyncReqRemove:
    case AsyncReqOpen:
    case AsyncReqRead:
    case AsyncReqWrite:
    case AsyncReqSeekRead:
    case AsyncReqSeekWrite:
        loc_free(req->info.u.fio.file_name);
        loc_free(req->info.u.fio.bufp);
        break;
    case AsyncReqOpenDir:
    case AsyncReqReadDir:
    case AsyncReqCloseDir:
        if (req->info.u.dio.files != NULL) {
            int cnt = 0;
            while (cnt < req->info.u.dio.max_files) {
                struct DirFileNode * file = req->info.u.dio.files + cnt++;
                if (file->path == NULL) break;
                loc_free(file->path);
                loc_free(file->statbuf);
            }
            loc_free(req->info.u.dio.files);
        }
        loc_free(req->info.u.dio.path);
        break;
    case AsyncReqRoots:
        {
            struct RootDevNode * current_root = req->info.u.root.lst;
            while (current_root != NULL) {
                struct RootDevNode * next_root = current_root->next;
                loc_free(current_root->devname);
                loc_free(current_root->statbuf);
                loc_free(current_root);
                current_root = next_root;
            }
        }
        break;
    }
    loc_free(req);
}

static void channel_close_listener(Channel * c) {
    LINK list;
    LINK * list_next;

    list_init(&list);
    for (list_next = file_info_ring.next; list_next != &file_info_ring; list_next = list_next->next) {
        OpenFileInfo * h = ring2file(list_next);
        if (h->inp == &c->inp) {
            int posted = 0;
            trace(LOG_ALWAYS, "file handle left open by client: FS%lu", h->handle);
            list_remove(&h->link_hash);
            while (!list_is_empty(&h->link_reqs)) {
                LINK * link = h->link_reqs.next;
                IORequest * req = reqs2req(link);
                list_remove(link);
                if (h->posted_req == req) {
                    req->handle = NULL;
                    posted = 1;
                }
                else {
                    free_io_req(req);
                }
            }
            if (!posted) {
                if (h->file >= 0) close(h->file);
                if (h->dir != NULL) closedir(h->dir);
            }
            list_add_last(&h->link_hash, &list);
        }
    }

    while (!list_is_empty(&list)) delete_open_file_info(hash2file(list.next));
}

static void write_fs_errno(OutputStream * out, int err) {
    switch (err) {
    case ERR_EOF:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EOF);
        break;
    case ENOENT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_NO_SUCH_FILE);
        break;
    case EACCES:
        write_service_error(out, err, FILE_SYSTEM, FSERR_PERMISSION_DENIED);
        break;
    case EPERM:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EPERM);
        break;
    case ESRCH:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ESRCH);
        break;
    case EINTR:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EINTR);
        break;
    case EIO:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EIO);
        break;
    case ENXIO:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENXIO);
        break;
    case E2BIG:
        write_service_error(out, err, FILE_SYSTEM, FSERR_E2BIG);
        break;
    case ENOEXEC:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOEXEC);
        break;
    case EBADF:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EBADF);
        break;
    case ECHILD:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ECHILD);
        break;
    case EAGAIN:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EAGAIN);
        break;
    case ENOMEM:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOMEM);
        break;
    case EFAULT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EFAULT);
        break;
    case EBUSY:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EBUSY);
        break;
    case EEXIST:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EEXIST);
        break;
    case EXDEV:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EXDEV);
        break;
    case ENODEV:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENODEV);
        break;
    case ENOTDIR:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOTDIR);
        break;
    case EISDIR:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EISDIR);
        break;
    case EINVAL:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EINVAL);
        break;
    case ENFILE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENFILE);
        break;
    case EMFILE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EMFILE);
        break;
    case ENOTTY:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOTTY);
        break;
    case EFBIG:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EFBIG);
        break;
    case ENOSPC:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOSPC);
        break;
    case ESPIPE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ESPIPE);
        break;
    case EROFS:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EROFS);
        break;
    case EMLINK:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EMLINK);
        break;
    case EPIPE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EPIPE);
        break;
    case EDEADLK:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EDEADLK);
        break;
    case ENOLCK:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOLCK);
        break;
    case EDOM:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EDOM);
        break;
    case ERANGE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ERANGE);
        break;
    case ENOSYS:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOSYS);
        break;
    case ENOTEMPTY:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOTEMPTY);
        break;
    case ENAMETOOLONG:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENAMETOOLONG);
        break;
#if !defined(_WIN32) && !defined(__CYGWIN__)
    case ENOBUFS:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOBUFS);
        break;
    case EISCONN:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EISCONN);
        break;
    case ENOSTR:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOSTR);
        break;
    case EPROTO:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EPROTO);
        break;
    case EBADMSG:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EBADMSG);
        break;
    case ENODATA:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENODATA);
        break;
    case ETIME:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ETIME);
        break;
    case ENOMSG:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOMSG);
        break;
    case ETXTBSY:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ETXTBSY);
        break;
    case ELOOP:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ELOOP);
        break;
#ifdef ENOTBLK
    case ENOTBLK:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOTBLK);
        break;
#endif
    case EMSGSIZE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EMSGSIZE);
        break;
    case EDESTADDRREQ:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EDESTADDRREQ);
        break;
    case EPROTOTYPE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EDESTADDRREQ);
        break;
    case ENOTCONN:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOTCONN);
        break;
#ifdef ESHUTDOWN
    case ESHUTDOWN:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ESHUTDOWN);
        break;
#endif
    case ECONNRESET:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ESHUTDOWN);
        break;
    case EOPNOTSUPP:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EOPNOTSUPP);
        break;
    case EAFNOSUPPORT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EAFNOSUPPORT);
        break;
    case EPFNOSUPPORT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EPFNOSUPPORT);
        break;
    case EADDRINUSE:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EADDRINUSE);
        break;
    case ENOTSOCK:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENOTSOCK);
        break;
    case ENETUNREACH:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENETUNREACH);
        break;
    case ENETRESET:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENETRESET);
        break;
    case ECONNABORTED:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ECONNABORTED);
        break;
    case ETOOMANYREFS:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ETOOMANYREFS);
        break;
    case ETIMEDOUT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ETIMEDOUT);
        break;
    case ECONNREFUSED:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ECONNREFUSED);
        break;
    case ENETDOWN:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ENETDOWN);
        break;
    case EHOSTUNREACH:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EHOSTUNREACH);
        break;
    case EHOSTDOWN:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EHOSTDOWN);
        break;
    case EINPROGRESS:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EINPROGRESS);
        break;
    case EALREADY:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EALREADY);
        break;
    case ECANCELED:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ECANCELED);
        break;
    case EPROTONOSUPPORT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EPROTONOSUPPORT);
        break;
#ifdef ESOCKTNOSUPPORT
    case ESOCKTNOSUPPORT:
        write_service_error(out, err, FILE_SYSTEM, FSERR_ESOCKTNOSUPPORT);
        break;
#endif
    case EADDRNOTAVAIL:
        write_service_error(out, err, FILE_SYSTEM, FSERR_EADDRNOTAVAIL);
        break;
#endif
    default:
        write_errno(out, err);
        break;
    }
}

static void write_file_handle(OutputStream * out, OpenFileInfo * h) {
    if (h == NULL) {
        write_string(out, "null");
    }
    else {
        char s[32];
        char * p = s + sizeof(s);
        unsigned long n = h->handle;
        *(--p) = 0;
        do {
            *(--p) = (char)(n % 10 + '0');
            n = n / 10;
        }
        while (n != 0);
        *(--p) = 'S';
        *(--p) = 'F';
        json_write_string(out, p);
    }
    write_stream(out, 0);
}

static void fill_attrs(FileAttrs * attrs, struct stat * buf) {
    memset(attrs, 0, sizeof(FileAttrs));
    attrs->flags |= ATTR_SIZE | ATTR_UIDGID | ATTR_PERMISSIONS | ATTR_ACMODTIME;
    attrs->size = buf->st_size;
    attrs->uid = buf->st_uid;
    attrs->gid = buf->st_gid;
    attrs->permissions = buf->st_mode;
    attrs->atime = (uint64_t)buf->st_atime * 1000;
    attrs->mtime = (uint64_t)buf->st_mtime * 1000;
}

static void read_file_attrs(InputStream * inp, const char * nm, void * arg) {
    FileAttrs * attrs = (FileAttrs *)arg;
    if (strcmp(nm, "Size") == 0) {
        attrs->size = json_read_int64(inp);
        attrs->flags |= ATTR_SIZE;
    }
    else if (strcmp(nm, "UID") == 0) {
        attrs->uid = (int)json_read_long(inp);
        attrs->flags |= ATTR_UIDGID;
    }
    else if (strcmp(nm, "GID") == 0) {
        attrs->gid = (int)json_read_long(inp);
        attrs->flags |= ATTR_UIDGID;
    }
    else if (strcmp(nm, "Permissions") == 0) {
        attrs->permissions = (int)json_read_long(inp);
        attrs->flags |= ATTR_PERMISSIONS;
    }
    else if (strcmp(nm, "ATime") == 0) {
        attrs->atime = json_read_uint64(inp);
        attrs->flags |= ATTR_ACMODTIME;
    }
    else if (strcmp(nm, "MTime") == 0) {
        attrs->mtime = json_read_uint64(inp);
        attrs->flags |= ATTR_ACMODTIME;
    }
#if defined(_WIN32) || defined(__CYGWIN__)
    else if (strcmp(nm, "Win32Attrs") == 0) {
        attrs->win32_attrs = json_read_ulong(inp);
    }
#endif
    else {
        exception(ERR_JSON_SYNTAX);
    }
}

static void write_file_attrs(OutputStream * out, FileAttrs * attrs) {
    int cnt = 0;

    if (attrs == NULL) {
        write_stringz(out, "null");
        return;
    }

    write_stream(out, '{');
    if (attrs->flags & ATTR_SIZE) {
        json_write_string(out, "Size");
        write_stream(out, ':');
        json_write_int64(out, attrs->size);
        cnt++;
    }
    if (attrs->flags & ATTR_UIDGID) {
        if (cnt) write_stream(out, ',');
        json_write_string(out, "UID");
        write_stream(out, ':');
        json_write_long(out, attrs->uid);
        write_stream(out, ',');
        json_write_string(out, "GID");
        write_stream(out, ':');
        json_write_long(out, attrs->gid);
        cnt++;
    }
    if (attrs->flags & ATTR_PERMISSIONS) {
        if (cnt) write_stream(out, ',');
        json_write_string(out, "Permissions");
        write_stream(out, ':');
        json_write_long(out, attrs->permissions);
        cnt++;
    }
    if (attrs->flags & ATTR_ACMODTIME) {
        if (cnt) write_stream(out, ',');
        json_write_string(out, "ATime");
        write_stream(out, ':');
        json_write_uint64(out, attrs->atime);
        write_stream(out, ',');
        json_write_string(out, "MTime");
        write_stream(out, ':');
        json_write_uint64(out, attrs->mtime);
#if defined(_WIN32) || defined(__CYGWIN__)
        cnt++;
#endif
    }

#if defined(_WIN32) || defined(__CYGWIN__)
    if (attrs->win32_attrs != INVALID_FILE_ATTRIBUTES) {
        if (cnt) write_stream(out, ',');
        json_write_string(out, "Win32Attrs");
        write_stream(out, ':');
        json_write_ulong(out, attrs->win32_attrs);
        cnt++;
    }
#endif

    write_stream(out, '}');
}

static int to_local_open_flags(int flags) {
    int res = O_BINARY;
    if ((flags & TCF_O_READ) && (flags & TCF_O_WRITE)) res |= O_RDWR;
    else if (flags & TCF_O_READ) res |= O_RDONLY;
    else if (flags & TCF_O_WRITE) res |= O_WRONLY;

    if (flags & TCF_O_APPEND) res |= O_APPEND;
    if (flags & TCF_O_CREAT) res |= O_CREAT;
    if (flags & TCF_O_TRUNC) res |= O_TRUNC;
    if (flags & TCF_O_EXCL) res |= O_EXCL;
    return res;
}

static void read_path(InputStream * inp, char * path, int size) {
    int i;
    char buf[FILE_PATH_SIZE];
    json_read_string(inp, path, size);
    if (path[0] == 0) strlcpy(path, get_user_home(), size);
    for (i = 0; path[i] != 0; i++) {
        if (path[i] == '\\') path[i] = '/';
    }
#if defined(__CYGWIN__)
    if (path[0] != '/' && !(path[0] != 0 && path[1] == ':' && path[2] == '/')) {
        snprintf(buf, sizeof(buf), "%s/%s", get_user_home(), path);
        strlcpy(path, buf, size);
        for (i = 0; path[i] != 0; i++) {
            if (path[i] == '\\') path[i] = '/';
        }
    }
    if (path[0] != 0 && path[1] == ':' && path[2] == '/') {
        if (path[3] == 0) {
            snprintf(buf, sizeof(buf), "/cygdrive/%c", tolower((int)path[0]));
        }
        else {
            snprintf(buf, sizeof(buf), "/cygdrive/%c/%s", tolower((int)path[0]), path + 3);
        }
        strlcpy(path, buf, size);
    }
#elif defined(_WIN32)
    if (path[0] != 0 && path[1] == ':' && path[2] == '/') return;
#elif defined(_WRS_KERNEL)
    {
        extern DL_LIST iosDvList;
        DEV_HDR * dev;
        for (dev = (DEV_HDR *)DLL_FIRST(&iosDvList); dev != NULL; dev = (DEV_HDR *)DLL_NEXT(&dev->node)) {
            if (strncmp(path, dev->name, strlen(dev->name)) == 0) return;
        }
    }
#endif
    if (path[0] != '/') {
        const char * home = get_user_home();
        if (strcmp(home, "/") == 0) home = "";
        snprintf(buf, sizeof(buf), "%s/%s", home, path);
        strlcpy(path, buf, size);
        for (i = 0; path[i] != 0; i++) {
            if (path[i] == '\\') path[i] = '/';
        }
    }
    assert(path[0] == '/');
}

static void reply_open(char * token, OutputStream * out, int err, OpenFileInfo * handle) {
    write_stringz(out, "R");
    write_stringz(out, token);
    write_fs_errno(out, err);
    write_file_handle (out, handle);
    write_stream(out, MARKER_EOM);
}

static void post_io_request(OpenFileInfo * handle);
static IORequest * create_io_request(char * token, OpenFileInfo * handle, int type);

static void command_open(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    unsigned flags;
    FileAttrs attrs;
    OpenFileInfo * handle = NULL;
    IORequest * req = NULL;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    flags = (unsigned)json_read_ulong(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    memset(&attrs, 0, sizeof(FileAttrs));
#if defined(_WIN32) || defined(__CYGWIN__)
    attrs.win32_attrs = INVALID_FILE_ATTRIBUTES;
#endif
    json_read_struct(&c->inp, read_file_attrs, &attrs);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if ((attrs.flags & ATTR_PERMISSIONS) == 0) {
        attrs.permissions = 0775;
    }
    handle = create_open_file_info(c, path, -1, NULL);

    req = create_io_request(token, handle, AsyncReqOpen);
    req->info.type = AsyncReqOpen;
    req->info.u.fio.permission = attrs.permissions;
    req->info.u.fio.flags = to_local_open_flags(flags);
    req->info.u.fio.file_name = loc_strdup(path);
#if defined(_WIN32) || defined(__CYGWIN__)
    req->info.u.fio.win32_attrs = attrs.win32_attrs;
#endif
    post_io_request(handle);
}

static void reply_close(char * token, OutputStream * out, int err) {
    write_stringz(out, "R");
    write_stringz(out, token);
    write_fs_errno(out, err);
    write_stream(out, MARKER_EOM);
}

static void reply_read(char * token, OutputStream * out, int err, void * buf, size_t len, int eof) {
    write_stringz(out, "R");
    write_stringz(out, token);
    json_write_binary(out, buf, len);
    write_stream(out, 0);
    write_fs_errno(out, err);
    json_write_boolean(out, eof);
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);
}

static void reply_write(char * token, OutputStream * out, int err) {
    write_stringz(out, "R");
    write_stringz(out, token);
    write_fs_errno(out, err);
    write_stream(out, MARKER_EOM);
}

static void reply_stat(char * token, OutputStream * out, int err, AsyncReqInfo * buf) {
    FileAttrs attrs;

    if (err == 0) fill_attrs(&attrs, &buf->u.fio.statbuf);
    else memset(&attrs, 0, sizeof(attrs));

#if defined(_WIN32) || defined(__CYGWIN__)
    attrs.win32_attrs = err ? INVALID_FILE_ATTRIBUTES : buf->u.fio.win32_attrs;
#endif

    write_stringz(out, "R");
    write_stringz(out, token);
    write_fs_errno(out, err);
    write_file_attrs(out, &attrs);
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);
}

static void reply_setstat(char * token, OutputStream * out, int err) {
    write_stringz(out, "R");
    write_stringz(out, token);
    write_fs_errno(out, err);
    write_stream(out, MARKER_EOM);
}

static void reply_remove(char * token, OutputStream * out, int err) {
    write_stringz(out, "R");
    write_stringz(out, token);
    write_fs_errno(out, err);
    write_stream(out, MARKER_EOM);
}

static void reply_readdir(char * token, OutputStream * out, int err, struct DirFileNode * files, int max_files, int eof) {
    int cnt = 0;
    struct DirFileNode * file = files;

    write_stringz(out, "R");
    write_stringz(out, token);
    write_stream(out, '[');
    while (file != NULL && cnt < max_files && file->path != NULL) {
        FileAttrs attrs;
        if (cnt++ > 0) write_stream(out, ',');
        write_stream(out, '{');
        json_write_string(out, "FileName");
        write_stream(out, ':');
        json_write_string(out, file->path);

        if (file->statbuf != NULL) {
            fill_attrs(&attrs, file->statbuf);
#if defined(_WIN32) || defined(__CYGWIN__)
            attrs.win32_attrs = file->win32_attrs;
#endif
            write_stream(out, ',');
            json_write_string(out, "Attrs");
            write_stream(out, ':');
            write_file_attrs(out, &attrs);
        }
        write_stream(out, '}');
        file++;
    }
    write_stream(out, ']');
    write_stream(out, 0);

    write_fs_errno(out, err);
    json_write_boolean(out, eof);
    write_stream(out, 0);
    write_stream(out, MARKER_EOM);
}

static void reply_roots(char * token, OutputStream * out, int err, struct RootDevNode * rootlst) {
    FileAttrs attrs;
    int cnt = 0;
    struct RootDevNode * root = rootlst;

    write_stringz(out, "R");
    write_stringz(out, token);
    write_stream(out, '[');
    while (root != NULL) {
        assert(root->devname != NULL);
        if (cnt++ > 0) write_stream(out, ',');
        write_stream(out, '{');
        json_write_string(out, "FileName");
        write_stream(out, ':');
        json_write_string(out, root->devname);

        if (root->statbuf != NULL) {
            fill_attrs(&attrs, root->statbuf);
#if defined(_WIN32) || defined(__CYGWIN__)
            attrs.win32_attrs = root->win32_attrs;
#endif
            write_stream(out, ',');
            json_write_string(out, "Attrs");
            write_stream(out, ':');
            write_file_attrs(out, &attrs);
        }
        write_stream(out, '}');
        root = root->next;
    }
    write_stream(out, ']');
    write_stream(out, 0);
    write_fs_errno(out, err);
    write_stream(out, MARKER_EOM);
}

static void terminate_open_file_info(OpenFileInfo * handle) {
    assert(handle->posted_req == NULL);
    while (!list_is_empty(&handle->link_reqs)) {
        LINK * link = handle->link_reqs.next;
        IORequest * req = reqs2req(link);
        switch (req->info.type) {
        case AsyncReqRead:
        case AsyncReqSeekRead:
            reply_read(req->token, handle->out, EBADF, NULL, 0, 0);
            break;
        case AsyncReqWrite:
        case AsyncReqSeekWrite:
            reply_write(req->token, handle->out, EBADF);
            break;
        case AsyncReqFstat:
            reply_stat(req->token, handle->out, EBADF, NULL);
            break;
        case AsyncReqFSetStat:
            reply_setstat(req->token, handle->out, EBADF);
            break;
        case AsyncReqClose:
        case AsyncReqCloseDir:
            reply_close(req->token, handle->out, EBADF);
            break;
        case AsyncReqReadDir:
            reply_readdir(req->token, handle->out, EBADF, NULL, 0, 0);
            break;
        default:
            /* Other request types are NOT serialized per file handle */
            assert(0);
        }
        list_remove(link);
        free_io_req(req);
    }
    delete_open_file_info(handle);
}

static void done_io_request(void * arg) {
    int err = 0;
    IORequest * req = (IORequest *)((AsyncReqInfo *)arg)->client_data;
    OpenFileInfo * handle = req->handle;

    if (handle == NULL) {
        /* Abandoned I/O request, channel is already closed */
        switch (req->info.type) {
        case AsyncReqOpen:
            if (req->info.u.fio.rval >= 0) close(req->info.u.fio.rval);
            break;
        case AsyncReqRead:
        case AsyncReqWrite:
        case AsyncReqSeekRead:
        case AsyncReqSeekWrite:
        case AsyncReqFstat:
        case AsyncReqFSetStat:
            close(req->info.u.fio.fd);
            break;
        case AsyncReqOpenDir:
        case AsyncReqReadDir:
            if (req->info.u.dio.dir != NULL) closedir((DIR *)req->info.u.dio.dir);
            break;
        }
        free_io_req(req);
        return;
    }

    assert(handle->posted_req == req);
    assert(&handle->posted_req->link_reqs == handle->link_reqs.next);
    handle->posted_req = NULL;
    list_remove(&req->link_reqs);
    err = req->info.error;

    switch (req->info.type) {
    case AsyncReqRead:
    case AsyncReqSeekRead:
        if (err) {
            reply_read(req->token, handle->out, err, NULL, 0, 0);
        }
        else {
            reply_read(req->token, handle->out, 0,
                req->info.u.fio.bufp, req->info.u.fio.rval,
                (size_t)req->info.u.fio.rval < req->info.u.fio.bufsz);
        }
        break;
    case AsyncReqWrite:
    case AsyncReqSeekWrite:
        if (!err && (size_t)req->info.u.fio.rval < req->info.u.fio.bufsz) err = ENOSPC;
        reply_write(req->token, handle->out, err);
        break;
    case AsyncReqClose:
        reply_close(req->token, handle->out, err);
        if (err == 0) {
            free_io_req(req);
            terminate_open_file_info(handle);
            return;
        }
        break;
    case AsyncReqOpen:
#if defined(_WIN32) || defined(__CYGWIN__)
        if (err == 0) {
            if (req->info.u.fio.win32_attrs != INVALID_FILE_ATTRIBUTES) {
                if (SetFileAttributes(req->info.u.fio.file_name, req->info.u.fio.win32_attrs) == 0)
                    err = set_win32_errno(GetLastError());
            }
        }
#endif
        if (err != 0) {
            reply_open(req->token, handle->out, err, NULL);
            terminate_open_file_info(handle);
            free_io_req(req);
            return;
        }
        handle->file = req->info.u.fio.rval;
        reply_open(req->token, handle->out, err, handle);
        break;
    case AsyncReqLstat:
    case AsyncReqStat:
        reply_stat(req->token, handle->out, err, &req->info);
        delete_open_file_info(handle);
        free_io_req(req);
        return;
    case AsyncReqSetStat:
        reply_setstat(req->token, handle->out, err);
        delete_open_file_info(handle);
        free_io_req(req);
        return;
    case AsyncReqFstat:
        reply_stat(req->token, handle->out, err, &req->info);
        break;
    case AsyncReqFSetStat:
        reply_setstat(req->token, handle->out, err);
        break;
    case AsyncReqRemove:
        reply_remove(req->token, handle->out, err);
        delete_open_file_info(handle);
        free_io_req(req);
        return;
    case AsyncReqOpenDir:
        if (err != 0) {
            reply_open(req->token, handle->out, err, NULL);
            terminate_open_file_info(handle);
            free_io_req(req);
            return;
        }
        handle->dir = (DIR *)req->info.u.dio.dir;
        reply_open(req->token, handle->out, err, handle);
        break;
    case AsyncReqReadDir:
        reply_readdir(
            req->token, handle->out, err, req->info.u.dio.files,
            req->info.u.dio.max_files, req->info.u.dio.eof);
        break;
    case AsyncReqCloseDir:
        reply_close(req->token, handle->out, err);
        if (err == 0) {
            free_io_req(req);
            terminate_open_file_info(handle);
            return;
        }
        break;
    case AsyncReqRoots:
        reply_roots(req->token, handle->out, err, req->info.u.root.lst);
        delete_open_file_info(handle);
        free_io_req(req);
        return;
    default:
        assert(0);
    }

    free_io_req(req);
    post_io_request(handle);
}

static void post_io_request(OpenFileInfo * handle) {
    if (handle->posted_req == NULL && !list_is_empty(&handle->link_reqs)) {
        LINK * link = handle->link_reqs.next;
        IORequest * req = reqs2req(link);
        handle->posted_req = req;
        async_req_post(&req->info);
    }
}

static IORequest * create_io_request(char * token, OpenFileInfo * handle, int type) {
    IORequest * req = (IORequest *)loc_alloc_zero(sizeof(IORequest));
    req->handle = handle;
    req->info.type = type;
    req->info.done = done_io_request;
    req->info.client_data = req;
    strlcpy(req->token, token, sizeof(req->token));
    list_add_last(&req->link_reqs, &handle->link_reqs);
    return req;
}

static void command_close(char * token, Channel * c) {
    char id[256];
    OpenFileInfo * h;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    h = find_open_file_info(id);
    if (h == NULL) {
        reply_close(token, &c->out, EBADF);
    }
    else {
        if (h->dir != NULL){
            IORequest * req = create_io_request(token, h, AsyncReqCloseDir);
            req->info.u.dio.dir = h->dir;
        }
        else {
            IORequest * req = create_io_request(token, h, AsyncReqClose);
            req->info.u.fio.fd = h->file;
        }
        post_io_request(h);
    }
}

static void command_read(char * token, Channel * c) {
    char id[256];
    OpenFileInfo * h;
    int64_t offset;
    unsigned long len;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    offset = json_read_int64(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    len = json_read_ulong(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    h = find_open_file_info(id);
    if (h == NULL) {
        reply_read(token, &c->out, EBADF, NULL, 0, 0);
    }
    else {
        IORequest * req = create_io_request(token, h, AsyncReqRead);
        if (offset >= 0) {
            req->info.type = AsyncReqSeekRead;
            req->info.u.fio.offset = offset;
        }
        req->info.u.fio.fd = h->file;
        req->info.u.fio.bufp = loc_alloc(len);
        req->info.u.fio.bufsz = len;
        post_io_request(h);
    }
}

static void command_write(char * token, Channel * c) {
    char id[256];
    OpenFileInfo * h;
    int64_t offset;
    size_t len = 0;
    JsonReadBinaryState state;

    static size_t buf_size = 0;
    static char * buf = NULL;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    offset = json_read_int64(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);

    json_read_binary_start(&state, &c->inp);

    h = find_open_file_info(id);
    for (;;) {
        size_t rd;
        if (buf_size < len + BUF_SIZE) {
            buf_size += BUF_SIZE;
            buf = (char *)loc_realloc(buf, buf_size);
        }
        rd = json_read_binary_data(&state, buf + len, buf_size - len);
        if (rd == 0) break;
        len += rd;
    }
    json_read_binary_end(&state);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (h == NULL) {
        reply_write(token, &c->out, EBADF);
    }
    else {
        IORequest * req = create_io_request(token, h, AsyncReqWrite);
        if (offset >= 0) {
            req->info.type = AsyncReqSeekWrite;
            req->info.u.fio.offset = offset;
        }
        req->info.u.fio.fd = h->file;
        req->info.u.fio.bufp = loc_alloc(len);
        req->info.u.fio.bufsz = len;
        memcpy(req->info.u.fio.bufp, buf, len);
        post_io_request(h);
    }
}

static void command_stat(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    OpenFileInfo * handle = NULL;
    IORequest * req = NULL;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    handle = create_open_file_info(c, path, -1, NULL);
    req = create_io_request(token, handle, AsyncReqStat);
    req->info.u.fio.file_name = loc_strdup(path);
    post_io_request(handle);
}

static void command_lstat(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    OpenFileInfo * handle = NULL;
    IORequest * req = NULL;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    handle = create_open_file_info(c, path, -1, NULL);
    req = create_io_request(token, handle, AsyncReqLstat);
    req->info.u.fio.file_name = loc_strdup(path);
    post_io_request(handle);
}

static void command_fstat(char * token, Channel * c) {
    char id[256];
    OpenFileInfo * h;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    h = find_open_file_info(id);
    if (h == NULL) {
        reply_stat(token, &c->out, EBADF, NULL);
    }
    else {
        IORequest * req = create_io_request(token, h, AsyncReqFstat);
        req->info.u.fio.file_name = loc_strdup(h->path);
        req->info.u.fio.fd = h->file;
        post_io_request(h);
    }
}

static void command_setstat(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    FileAttrs attrs;
    OpenFileInfo * h = NULL;
    IORequest * req = NULL;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    memset(&attrs, 0, sizeof(FileAttrs));
#if defined(_WIN32) || defined(__CYGWIN__)
    attrs.win32_attrs = INVALID_FILE_ATTRIBUTES;
#endif
    json_read_struct(&c->inp, read_file_attrs, &attrs);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    h = create_open_file_info(c, path, -1, NULL);
    req = create_io_request(token, h, AsyncReqSetStat);
    if (attrs.flags & ATTR_SIZE) {
        req->info.u.fio.set_stat_flags |= AsyncReqSetSize;
        req->info.u.fio.statbuf.st_size = attrs.size;
    }
    if (attrs.flags & ATTR_UIDGID) {
        req->info.u.fio.set_stat_flags |= AsyncReqSetUidGid;
        req->info.u.fio.statbuf.st_uid = (uid_t)attrs.uid;
        req->info.u.fio.statbuf.st_gid = (gid_t)attrs.gid;
    }
    if (attrs.flags & ATTR_PERMISSIONS) {
        req->info.u.fio.set_stat_flags |= AsyncReqSetPermissions;
        req->info.u.fio.statbuf.st_mode = (mode_t)attrs.permissions;
    }
    if (attrs.flags & ATTR_ACMODTIME) {
        req->info.u.fio.set_stat_flags |= AsyncReqSetAcModTime;
        req->info.u.fio.statbuf.st_mtime = (time_t)(attrs.mtime / 1000);
        req->info.u.fio.statbuf.st_atime = (time_t)(attrs.atime / 1000);
    }
#if defined(_WIN32) || defined(__CYGWIN__)
    req->info.u.fio.win32_attrs = attrs.win32_attrs;
#endif
    req->info.u.fio.file_name = loc_strdup(h->path);
    post_io_request(h);
}

static void command_fsetstat(char * token, Channel * c) {
    char id[256];
    FileAttrs attrs;
    OpenFileInfo * h;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    memset(&attrs, 0, sizeof(FileAttrs));
#if defined(_WIN32) || defined(__CYGWIN__)
    attrs.win32_attrs = INVALID_FILE_ATTRIBUTES;
#endif
    json_read_struct(&c->inp, read_file_attrs, &attrs);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    h = find_open_file_info(id);
    if (h == NULL) {
        reply_setstat(token, &c->out, EBADF);
    }
    else {
        IORequest * req = create_io_request(token, h, AsyncReqFSetStat);
        if (attrs.flags & ATTR_SIZE) {
            req->info.u.fio.set_stat_flags |= AsyncReqSetSize;
            req->info.u.fio.statbuf.st_size = attrs.size;
        }
        if (attrs.flags & ATTR_UIDGID) {
            req->info.u.fio.set_stat_flags |= AsyncReqSetUidGid;
            req->info.u.fio.statbuf.st_uid = (uid_t)attrs.uid;
            req->info.u.fio.statbuf.st_gid = (gid_t)attrs.gid;
        }
        if (attrs.flags & ATTR_PERMISSIONS) {
            req->info.u.fio.set_stat_flags |= AsyncReqSetPermissions;
            req->info.u.fio.statbuf.st_mode = (mode_t)attrs.permissions;
        }
        if (attrs.flags & ATTR_ACMODTIME) {
            req->info.u.fio.set_stat_flags |= AsyncReqSetAcModTime;
            req->info.u.fio.statbuf.st_mtime = (time_t)(attrs.mtime / 1000);
            req->info.u.fio.statbuf.st_atime = (time_t)(attrs.atime / 1000);
        }
#if defined(_WIN32) || defined(__CYGWIN__)
        req->info.u.fio.win32_attrs = attrs.win32_attrs;
#endif
        req->info.u.fio.file_name = loc_strdup(h->path);
        req->info.u.fio.fd = h->file;
        post_io_request(h);
    }
}

static void command_opendir(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    OpenFileInfo * handle = NULL;
    IORequest * req = NULL;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    handle = create_open_file_info(c, path, -1, NULL);

    req = create_io_request(token, handle, AsyncReqOpenDir);
    req->info.u.dio.path = loc_strdup(path);
    post_io_request(handle);
}

static void command_readdir(char * token, Channel * c) {
    char id[256];
    OpenFileInfo * h;

    json_read_string(&c->inp, id, sizeof(id));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    h = find_open_file_info(id);
    if (h == NULL || h->dir == NULL) {
        reply_readdir(token, &c->out, EBADF, (struct DirFileNode *)NULL, 0, 0);
    }
    else {
        IORequest * req = create_io_request(token, h, AsyncReqReadDir);
        req->info.u.dio.dir = h->dir;
        req->info.u.dio.path = loc_strdup(h->path);
        req->info.u.dio.files = (struct DirFileNode *)loc_alloc_zero(sizeof(struct DirFileNode) * DIR_BUF_SIZE);
        req->info.u.dio.max_files = DIR_BUF_SIZE;
        post_io_request(h);
    }
}

static void command_remove(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    OpenFileInfo * handle = NULL;
    IORequest * req = NULL;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    handle = create_open_file_info(c, path, -1, NULL);
    req = create_io_request(token, handle, AsyncReqRemove);
    req->info.u.fio.file_name = loc_strdup(path);
    post_io_request(handle);
}

static void command_rmdir(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    int err = 0;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (rmdir(path) < 0) err = errno;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_mkdir(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    FileAttrs attrs;
    int err = 0;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    memset(&attrs, 0, sizeof(FileAttrs));
#if defined(_WIN32) || defined(__CYGWIN__)
    attrs.win32_attrs = INVALID_FILE_ATTRIBUTES;
#endif
    json_read_struct(&c->inp, read_file_attrs, &attrs);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if defined(_WRS_KERNEL) && !defined(_WRS_CONFIG_CORE__POSIX_MKDIR)
    if (mkdir(path) < 0) err = errno;
#else
    if (attrs.flags & ATTR_PERMISSIONS) {
        if (mkdir(path, attrs.permissions) < 0) err = errno;
    }
    else {
        if (mkdir(path, 0777) < 0) err = errno;
    }
#endif
#if defined(_WIN32) || defined(__CYGWIN__)
    if (attrs.win32_attrs != INVALID_FILE_ATTRIBUTES) {
        if (SetFileAttributes(path, attrs.win32_attrs) == 0)
            err = set_win32_errno(GetLastError());
    }
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_realpath(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    char * real;
    int err = 0;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if defined(__CYGWIN__)
    if (strncmp(path, "/cygdrive/", 10) == 0) {
        char buf[FILE_PATH_SIZE];
        snprintf(buf, sizeof(buf), "%c:/%s", path[10], path + 12);
        strlcpy(path, buf, sizeof(path));
    }
#endif

    real = canonicalize_file_name(path);
    if (real == NULL) err = errno;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    json_write_string(&c->out, real);
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
    free(real);
}

static void command_rename(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    char newp[FILE_PATH_SIZE];
    int err = 0;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    read_path(&c->inp, newp, sizeof(newp));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (rename(path, newp) < 0) err = errno;

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_readlink(char * token, Channel * c) {
    char path[FILE_PATH_SIZE];
    char link[FILE_PATH_SIZE];
    ssize_t len = 0;
    int err = 0;

    read_path(&c->inp, path, sizeof(path));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if defined(_WIN32) || defined(_WRS_KERNEL)
    err = ENOSYS;
#else
    len = readlink(path, link, sizeof(link));
    if (len < 0) {
        err = errno;
        len = 0;
    }
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    json_write_string_len(&c->out, link, (size_t)len);
    write_stream(&c->out, 0);
    write_stream(&c->out, MARKER_EOM);
}

static void command_symlink(char * token, Channel * c) {
    char link[FILE_PATH_SIZE];
    char target[FILE_PATH_SIZE];
    int err;

    read_path(&c->inp, link, sizeof(link));
    json_test_char(&c->inp, MARKER_EOA);
    read_path(&c->inp, target, sizeof(target));
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

#if defined(_WIN32) || defined(_WRS_KERNEL)
    err = ENOSYS;
#else
    err = (symlink(target, link) < 0) ? errno : 0;
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_copy(char * token, Channel * c) {
    char src[FILE_PATH_SIZE];
    char dst[FILE_PATH_SIZE];
    int copy_uidgid;
    int copy_perms;
    struct stat st;
    int fi = -1;
    int fo = -1;
    int err = 0;
    int64_t pos = 0;

    read_path(&c->inp, src, sizeof(src));
    json_test_char(&c->inp, MARKER_EOA);
    read_path(&c->inp, dst, sizeof(dst));
    json_test_char(&c->inp, MARKER_EOA);
    copy_uidgid = json_read_boolean(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    copy_perms = json_read_boolean(&c->inp);
    json_test_char(&c->inp, MARKER_EOA);
    json_test_char(&c->inp, MARKER_EOM);

    if (stat(src, &st) < 0) err = errno;
    if (err == 0 && (fi = open(src, O_RDONLY | O_BINARY, 0)) < 0) err = errno;
    if (err == 0 && (fo = open(dst, O_WRONLY | O_BINARY | O_CREAT, 0775)) < 0) err = errno;

    while (err == 0 && pos < st.st_size) {
        char buf[BUF_SIZE];
        ssize_t wr;
        ssize_t rd = read(fi, buf, sizeof(buf));
        if (rd == 0) break;
        if (rd < 0) {
            err = errno;
            break;
        }
        wr = write(fo, buf, rd);
        if (wr < 0) {
            err = errno;
            break;
        }
        if (wr < rd) {
            err = ENOSPC;
            break;
        }
        pos += rd;
    }

    if (fo >= 0 && close(fo) < 0 && err == 0) err = errno;
    if (fi >= 0 && close(fi) < 0 && err == 0) err = errno;

    if (err == 0) {
        struct utimbuf buf;
        buf.actime = st.st_atime;
        buf.modtime = st.st_mtime;
        if (utime(dst, &buf) < 0) err = errno;
    }
    if (err == 0 && copy_perms && chmod(dst, st.st_mode) < 0) err = errno;
#if !defined(_WIN32) && !defined(_WRS_KERNEL)
    if (err == 0 && copy_uidgid && chown(dst, st.st_uid, st.st_gid) < 0) err = errno;
#else
    /* disable "set but not used" warning */
    (void)copy_uidgid;
#endif

    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    write_fs_errno(&c->out, err);
    write_stream(&c->out, MARKER_EOM);
}

static void command_user(char * token, Channel * c) {
    json_test_char(&c->inp, MARKER_EOM);
    write_stringz(&c->out, "R");
    write_stringz(&c->out, token);
    json_write_long(&c->out, getuid());
    write_stream(&c->out, 0);
    json_write_long(&c->out, geteuid());
    write_stream(&c->out, 0);
    json_write_long(&c->out, getgid());
    write_stream(&c->out, 0);
    json_write_long(&c->out, getegid());
    write_stream(&c->out, 0);
    json_write_string(&c->out, get_user_home());
    write_stream(&c->out, 0);

    write_stream(&c->out, MARKER_EOM);
}

static void command_roots(char * token, Channel * c) {
    OpenFileInfo * h;

    json_test_char(&c->inp, MARKER_EOM);
    h = create_open_file_info(c, NULL, -1, NULL);
    create_io_request(token, h, AsyncReqRoots);
    post_io_request(h);
}

void ini_file_system_service(Protocol * proto) {
    int i;
    static int ini_file_system = 0;

    if (ini_file_system == 0) {
        add_channel_close_listener(channel_close_listener);
        for (i = 0; i < HANDLE_HASH_SIZE; i++) {
            list_init(&handle_hash[i]);
        }
        ini_file_system = 1;
    }

    add_command_handler(proto, FILE_SYSTEM, "open", command_open);
    add_command_handler(proto, FILE_SYSTEM, "close", command_close);
    add_command_handler(proto, FILE_SYSTEM, "read", command_read);
    add_command_handler(proto, FILE_SYSTEM, "write", command_write);
    add_command_handler(proto, FILE_SYSTEM, "stat", command_stat);
    add_command_handler(proto, FILE_SYSTEM, "lstat", command_lstat);
    add_command_handler(proto, FILE_SYSTEM, "fstat", command_fstat);
    add_command_handler(proto, FILE_SYSTEM, "setstat", command_setstat);
    add_command_handler(proto, FILE_SYSTEM, "fsetstat", command_fsetstat);
    add_command_handler(proto, FILE_SYSTEM, "opendir", command_opendir);
    add_command_handler(proto, FILE_SYSTEM, "readdir", command_readdir);
    add_command_handler(proto, FILE_SYSTEM, "remove", command_remove);
    add_command_handler(proto, FILE_SYSTEM, "rmdir", command_rmdir);
    add_command_handler(proto, FILE_SYSTEM, "mkdir", command_mkdir);
    add_command_handler(proto, FILE_SYSTEM, "realpath", command_realpath);
    add_command_handler(proto, FILE_SYSTEM, "rename", command_rename);
    add_command_handler(proto, FILE_SYSTEM, "readlink", command_readlink);
    add_command_handler(proto, FILE_SYSTEM, "symlink", command_symlink);
    add_command_handler(proto, FILE_SYSTEM, "copy", command_copy);
    add_command_handler(proto, FILE_SYSTEM, "user", command_user);
    add_command_handler(proto, FILE_SYSTEM, "roots", command_roots);
}

#endif /* SERVICE_FileSystem */
