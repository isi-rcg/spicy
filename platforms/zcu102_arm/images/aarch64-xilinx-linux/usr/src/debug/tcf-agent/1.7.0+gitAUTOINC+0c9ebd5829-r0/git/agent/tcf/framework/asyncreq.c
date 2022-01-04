/*******************************************************************************
 * Copyright (c) 2007-2020 Wind River Systems, Inc. and others.
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

#if defined(__GNUC__) && !defined(_GNU_SOURCE)
/* pread() and pwrite() need _GNU_SOURCE */
#  define _GNU_SOURCE
#endif

#include <tcf/config.h>
#include <assert.h>
#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#if defined(_WIN32) || defined(__CYGWIN__)
#elif defined(_WRS_KERNEL)
#else
#  include <sys/wait.h>
#endif
#include <tcf/framework/mdep-threads.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/mdep-fs.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/link.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/framework/shutdown.h>

#ifndef MAX_WORKER_THREADS
#define MAX_WORKER_THREADS 32
#endif

#ifndef EVENTS_TIMER_RESOLUTION
#define EVENTS_TIMER_RESOLUTION 50
#endif

static LINK worker_all = TCF_LIST_INIT(worker_all);
static LINK worker_idle = TCF_LIST_INIT(worker_idle);
static unsigned worker_count = 0;
static unsigned idle_count = 0;
static pthread_mutex_t wtlock;

typedef struct WorkerThread {
    LINK link_all;
    LINK link_idle;
    AsyncReqInfo * req;
    pthread_cond_t cond;
    pthread_t thread;
} WorkerThread;

#define idle2worker(link) ((WorkerThread *)((char *)(link) - offsetof(WorkerThread, link_idle)))

#define AsyncReqTimer -1

static AsyncReqInfo shutdown_req;
static AsyncReqInfo timer_req;

static void trigger_async_shutdown(ShutdownInfo * obj) {
    check_error(pthread_mutex_lock(&wtlock));
    while (!list_is_empty(&worker_idle)) {
        WorkerThread * wt = idle2worker(worker_idle.next);
        list_remove(&wt->link_idle);
        idle_count--;
        assert(wt->req == NULL);
        wt->req = &shutdown_req;
        check_error(pthread_cond_signal(&wt->cond));
    }
    check_error(pthread_mutex_unlock(&wtlock));
}

static ShutdownInfo async_shutdown = { trigger_async_shutdown };


static void worker_thread_exit(void * x) {
    WorkerThread * wt = (WorkerThread *)x;

    check_error(pthread_cond_destroy(&wt->cond));
    pthread_join(wt->thread, NULL);
    check_error(pthread_mutex_lock(&wtlock));
    assert(worker_count > 0);
    list_remove(&wt->link_all);
    if (--worker_count == 0) shutdown_set_stopped(&async_shutdown);
    trace(LOG_ASYNCREQ, "worker_thread_exit %p running threads %d", wt, worker_count);
    check_error(pthread_mutex_unlock(&wtlock));
    loc_free(wt);
}

static void * worker_thread_handler(void * x) {
    WorkerThread * wt = (WorkerThread *)x;

    pthread_setname_np(pthread_self(), "Async Worker");

    for (;;) {
        AsyncReqInfo * req = wt->req;

        assert(req != NULL);
        req->error = 0;
        switch(req->type) {
        case AsyncReqTimer:
#if defined(_WIN32) && !defined(__CYGWIN__)
            Sleep(EVENTS_TIMER_RESOLUTION);
            events_timer_ms = GetTickCount();
#else
            {
                struct timespec timenow;
                usleep(EVENTS_TIMER_RESOLUTION * 1000);
                if (clock_gettime(CLOCK_REALTIME, &timenow) == 0) {
                    events_timer_ms = (uint32_t)(timenow.tv_nsec / 1000000 + timenow.tv_sec * 1000);
                }
            }
#endif
            break;

        case AsyncReqRead:              /* File read */
            req->u.fio.rval = read(req->u.fio.fd, req->u.fio.bufp, req->u.fio.bufsz);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqWrite:             /* File write */
            req->u.fio.rval = write(req->u.fio.fd, req->u.fio.bufp, req->u.fio.bufsz);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqSeekRead:          /* File read at offset */
            req->u.fio.rval = pread(req->u.fio.fd, req->u.fio.bufp, req->u.fio.bufsz, (off_t)req->u.fio.offset);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqSeekWrite:         /* File write at offset */
            req->u.fio.rval = pwrite(req->u.fio.fd, req->u.fio.bufp, req->u.fio.bufsz, (off_t)req->u.fio.offset);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqRecv:              /* Socket recv */
            req->u.sio.rval = recv(req->u.sio.sock, req->u.sio.bufp, req->u.sio.bufsz, req->u.sio.flags);
            if (req->u.sio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqSend:              /* Socket send */
            req->u.sio.rval = send(req->u.sio.sock, req->u.sio.bufp, req->u.sio.bufsz, req->u.sio.flags);
            if (req->u.sio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqRecvFrom:          /* Socket recvfrom */
            req->u.sio.rval = recvfrom(req->u.sio.sock, req->u.sio.bufp, req->u.sio.bufsz, req->u.sio.flags, req->u.sio.addr, &req->u.sio.addrlen);
            if (req->u.sio.rval == -1) {
                req->error = errno;
                trace(LOG_ASYNCREQ, "AsyncReqRecvFrom: req %p, type %d, error %d", req, req->type, req->error);
                assert(req->error);
            }
            break;

        case AsyncReqSendTo:            /* Socket sendto */
            req->u.sio.rval = sendto(req->u.sio.sock, req->u.sio.bufp, req->u.sio.bufsz, req->u.sio.flags, req->u.sio.addr, req->u.sio.addrlen);
            if (req->u.sio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqAccept:            /* Accept socket connections */
            req->u.acc.rval = accept(req->u.acc.sock, req->u.acc.addr, req->u.acc.addr ? &req->u.acc.addrlen : NULL);
            if (req->u.acc.rval == -1) {
                req->error = errno;
                trace(LOG_ASYNCREQ, "AsyncReqAccept: req %p, type %d, error %d", req, req->type, req->error);
                assert(req->error);
            }
            break;

        case AsyncReqConnect:           /* Connect to socket */
            req->u.con.rval = connect(req->u.con.sock, req->u.con.addr, req->u.con.addrlen);
            if (req->u.con.rval == -1) {
                req->error = errno;
                trace(LOG_ASYNCREQ, "AsyncReqConnect: req %p, type %d, error %d", req, req->type, req->error);
                assert(req->error);
            }
            break;

/* Platform dependant IO methods */
#if defined(_WIN32) || defined(__CYGWIN__)
        case AsyncReqConnectPipe:
            req->u.cnp.rval = ConnectNamedPipe(req->u.cnp.pipe, NULL);
            if (!req->u.cnp.rval) {
                req->error = set_win32_errno(GetLastError());
                assert(req->error);
            }
            break;
#elif defined(_WRS_KERNEL)
#else
        case AsyncReqWaitpid:           /* Wait for process change */
            req->u.wpid.rval = waitpid(req->u.wpid.pid, &req->u.wpid.status, req->u.wpid.options);
            if (req->u.wpid.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;
#endif

        case AsyncReqSelect:
            {
                struct timeval tv;
                tv.tv_sec = (long)req->u.select.timeout.tv_sec;
                tv.tv_usec = req->u.select.timeout.tv_nsec / 1000;
                req->u.select.rval = select(req->u.select.nfds, &req->u.select.readfds,
                            &req->u.select.writefds, &req->u.select.errorfds, &tv);
                if (req->u.select.rval == -1) {
                    req->error = errno;
                    assert(req->error);
                }
            }
            break;

        case AsyncReqClose:
            req->u.fio.rval = close(req->u.fio.fd);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqCloseDir:
            req->u.dio.rval = closedir((DIR *)req->u.dio.dir);
            if (req->u.dio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqOpen:
            req->u.fio.rval = open(req->u.fio.file_name, req->u.fio.flags, req->u.fio.permission);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqOpenDir:
            req->u.dio.dir = opendir(req->u.dio.path);
            if (req->u.dio.dir == NULL) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqFstat:
            memset(&req->u.fio.statbuf, 0, sizeof(req->u.fio.statbuf));
            req->u.fio.rval = fstat(req->u.fio.fd, &req->u.fio.statbuf);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
#if defined(_WIN32) || defined(__CYGWIN__)
            req->u.fio.win32_attrs = req->error || !req->u.fio.file_name ?
                INVALID_FILE_ATTRIBUTES : GetFileAttributes(req->u.fio.file_name);
#endif
            break;

        case AsyncReqStat:
            memset(&req->u.fio.statbuf, 0, sizeof(req->u.fio.statbuf));
            req->u.fio.rval = stat(req->u.fio.file_name, &req->u.fio.statbuf);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
#if defined(_WIN32) || defined(__CYGWIN__)
            req->u.fio.win32_attrs = req->error ?
                INVALID_FILE_ATTRIBUTES : GetFileAttributes(req->u.fio.file_name);
#endif
            break;

        case AsyncReqLstat:
            memset(&req->u.fio.statbuf, 0, sizeof(req->u.fio.statbuf));
            req->u.fio.rval = lstat(req->u.fio.file_name, &req->u.fio.statbuf);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
#if defined(_WIN32) || defined(__CYGWIN__)
            req->u.fio.win32_attrs = req->error ?
                INVALID_FILE_ATTRIBUTES : GetFileAttributes(req->u.fio.file_name);
#endif
            break;

        case AsyncReqSetStat:
            {
                int err = 0;
                if (req->u.fio.set_stat_flags & AsyncReqSetSize) {
                    if (truncate(req->u.fio.file_name, (off_t)req->u.fio.statbuf.st_size) < 0) err = errno;
                }
                if (req->u.fio.set_stat_flags & AsyncReqSetPermissions) {
                    if (chmod(req->u.fio.file_name, req->u.fio.statbuf.st_mode) < 0) err = errno;
                }
#if defined(_WIN32) || defined(__CYGWIN__) || defined(_WRS_KERNEL)
#  if defined(_WIN32) || defined(__CYGWIN__)
                if (req->u.fio.win32_attrs != INVALID_FILE_ATTRIBUTES) {
                    if (SetFileAttributes(req->u.fio.file_name, req->u.fio.win32_attrs) == 0)
                        err = set_win32_errno(GetLastError());
                }
#  endif
#else
                if (req->u.fio.set_stat_flags & AsyncReqSetUidGid) {
                    if (chown(req->u.fio.file_name, req->u.fio.statbuf.st_uid, req->u.fio.statbuf.st_gid) < 0) err = errno;
                }
#endif
                if (req->u.fio.set_stat_flags & AsyncReqSetAcModTime) {
                    struct utimbuf buf;
                    buf.actime = req->u.fio.statbuf.st_atime;
                    buf.modtime = req->u.fio.statbuf.st_mtime;
                    if (utime(req->u.fio.file_name, &buf) < 0) err = errno;
                }
                req->error = err;
            }
            break;

        case AsyncReqFSetStat:
            {
                int err = 0;
                if (req->u.fio.set_stat_flags & AsyncReqSetSize) {
                    if (ftruncate(req->u.fio.fd, (off_t)req->u.fio.statbuf.st_size) < 0) err = errno;
                }
#if defined(_WIN32) || defined(__CYGWIN__) || defined(_WRS_KERNEL)
                if (req->u.fio.set_stat_flags & AsyncReqSetPermissions) {
                    if (chmod(req->u.fio.file_name, req->u.fio.statbuf.st_mode) < 0) err = errno;
                }
#  if defined(_WIN32) || defined(__CYGWIN__)
                if (req->u.fio.win32_attrs != INVALID_FILE_ATTRIBUTES) {
                    if (SetFileAttributes(req->u.fio.file_name, req->u.fio.win32_attrs) == 0)
                        err = set_win32_errno(GetLastError());
                }
#  endif
#else
                if (req->u.fio.set_stat_flags & AsyncReqSetUidGid) {
                    if (fchown(req->u.fio.fd, req->u.fio.statbuf.st_uid, req->u.fio.statbuf.st_gid) < 0) err = errno;
                }
                if (req->u.fio.set_stat_flags & AsyncReqSetPermissions) {
                    if (fchmod(req->u.fio.fd, req->u.fio.statbuf.st_mode) < 0) err = errno;
                }
#endif
                if (req->u.fio.set_stat_flags & AsyncReqSetAcModTime) {
                    struct utimbuf buf;
                    buf.actime = req->u.fio.statbuf.st_atime;
                    buf.modtime = req->u.fio.statbuf.st_mtime;
#if defined(_WIN32) && !defined(__MINGW32__)
                    if (futime(req->u.fio.fd, &buf) < 0) err = errno;
#else
                    if (utime(req->u.fio.file_name, &buf) < 0) err = errno;
#endif
                }
                req->error = err;
            }
            break;

        case AsyncReqRemove:
            req->u.fio.rval = remove(req->u.fio.file_name);
            if (req->u.fio.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        case AsyncReqReadDir:
            {
                int cnt = 0;
                while (cnt < req->u.dio.max_files) {
                    char path[FILE_PATH_SIZE];
                    struct DirFileNode * file = req->u.dio.files + cnt;
                    struct dirent * e;
                    struct stat st;
                    errno = 0;
                    e = readdir((DIR *)req->u.dio.dir);
                    if (e == NULL) {
                        req->error = errno;
                        if (req->error == 0) req->u.dio.eof = 1;
                        break;
                    }
                    if (strcmp(e->d_name, ".") == 0) continue;
                    if (strcmp(e->d_name, "..") == 0) continue;
                    file->path = loc_strdup(e->d_name);
                    memset(&st, 0, sizeof(st));
                    snprintf(path, sizeof(path), "%s/%s", req->u.dio.path, e->d_name);
                    if (stat(path, &st) == 0) {
#if defined(_WIN32) || defined(__CYGWIN__)
                        file->win32_attrs =  GetFileAttributes(path);
#endif
                        file->statbuf = (struct stat *)loc_alloc(sizeof(struct stat));
                        memcpy(file->statbuf, &st, sizeof(struct stat));
                    }
                    cnt++;
                }
            }
            break;

        case AsyncReqRoots:
            {
                struct stat st;
                struct RootDevNode * newDevNode = NULL;

#if defined(_WIN32) || defined(__CYGWIN__)
                {
                    struct RootDevNode * curDevNode = NULL;
                    int disk = 0;
                    DWORD disks = GetLogicalDrives();
                    for (disk = 0; disk <= 30; disk++) {
                        if (disks & (1 << disk)) {
                            char path[32];
                            newDevNode = (struct RootDevNode *)loc_alloc_zero(sizeof(struct RootDevNode));
                            if (curDevNode == NULL) req->u.root.lst = newDevNode;
                            else curDevNode->next = newDevNode;
                            curDevNode = newDevNode;
                            snprintf(path, sizeof(path), "%c:\\", 'A' + disk);
                            newDevNode->devname = loc_strdup(path);
                            if (disk >= 2) {
                                ULARGE_INTEGER total_number_of_bytes;
                                BOOL has_size = GetDiskFreeSpaceExA(path, NULL, &total_number_of_bytes, NULL);
                                memset(&st, 0, sizeof(st));
#if defined(__CYGWIN__)
                                snprintf(path, sizeof(path), "/cygdrive/%c", 'a' + disk);
#endif
                                if (has_size && stat(path, &st) == 0) {
                                    newDevNode->win32_attrs =  GetFileAttributes(path);
                                    newDevNode->statbuf = (struct stat *)loc_alloc_zero(sizeof(struct stat));
                                    memcpy(newDevNode->statbuf, &st, sizeof(struct stat));
                                }
                            }
                        }
                    }
                }
#elif defined(_WRS_KERNEL)
                {
                    struct RootDevNode * curDevNode = NULL;
                    extern DL_LIST iosDvList;
                    DEV_HDR * dev;
                    for (dev = (DEV_HDR *)DLL_FIRST(&iosDvList); dev != NULL; dev = (DEV_HDR *)DLL_NEXT(&dev->node)) {
                        char path[FILE_PATH_SIZE];
                        if (strcmp(dev->name, "host:") == 0) {
                            /* Windows host is special case */
                            int d;
                            for (d = 'a'; d < 'z'; d++) {
                                snprintf(path, sizeof(path), "%s%c:/", dev->name, d);
                                if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
                                    newDevNode = (struct RootDevNode *)loc_alloc_zero(sizeof(struct RootDevNode));
                                    if (curDevNode == NULL) req->u.root.lst = newDevNode;
                                    else curDevNode->next = newDevNode;
                                    curDevNode = newDevNode;

                                    newDevNode->devname = loc_strdup(path);
                                    newDevNode->statbuf = (struct stat *)loc_alloc_zero(sizeof(struct stat));
                                    memcpy(newDevNode->statbuf, &st, sizeof(struct stat));
                                }
                            }
                        }
                        snprintf(path, sizeof(path), "%s/", dev->name);
                        if (stat(path, &st) == 0 && S_ISDIR(st.st_mode)) {
                            newDevNode = (struct RootDevNode *)loc_alloc_zero(sizeof(struct RootDevNode));
                            if (curDevNode == NULL) req->u.root.lst = newDevNode;
                            else curDevNode->next = newDevNode;
                            curDevNode = newDevNode;

                            newDevNode->devname = loc_strdup(path);
                            newDevNode->statbuf = (struct stat *)loc_alloc_zero(sizeof(struct stat));
                            memcpy(newDevNode->statbuf, &st, sizeof(struct stat));
                        }
                    }
                }
#else
                req->u.root.lst = newDevNode = (struct RootDevNode *)loc_alloc_zero(sizeof(struct RootDevNode));
                newDevNode->devname = loc_strdup("/");
                if (stat("/", &st) == 0) {
                    newDevNode->statbuf = (struct stat *)loc_alloc_zero(sizeof(struct stat));
                    memcpy(newDevNode->statbuf, &st, sizeof(struct stat));
                }
#endif
            }
            break;

        case AsyncReqUser:              /* User defined request */
            req->u.user.rval = req->u.user.func(req->u.user.data);
            if (req->u.user.rval == -1) {
                req->error = errno;
                assert(req->error);
            }
            break;

        default:
            req->error = ENOSYS;
            break;
        }
        if (req->type == AsyncReqTimer) {
            if (async_shutdown.state == SHUTDOWN_STATE_PENDING) break;
            continue;
        }
        trace(LOG_ASYNCREQ, "async_req_complete: req %p, type %d, error %d", req, req->type, req->error);
        check_error(pthread_mutex_lock(&wtlock));
        /* Post event inside lock to make sure a new worker thread is not created unnecessarily */
        post_event(req->done, req);
        wt->req = NULL;
        if (idle_count >= MAX_WORKER_THREADS || async_shutdown.state == SHUTDOWN_STATE_PENDING) {
            check_error(pthread_mutex_unlock(&wtlock));
            break;
        }
        list_add_last(&wt->link_idle, &worker_idle);
        idle_count++;
        for (;;) {
            check_error(pthread_cond_wait(&wt->cond, &wtlock));
            if (wt->req != NULL) break;
        }
        check_error(pthread_mutex_unlock(&wtlock));
        if (wt->req == &shutdown_req) break;
    }
    post_event(worker_thread_exit, wt);
    return NULL;
}

static void worker_thread_add(AsyncReqInfo * req) {
    WorkerThread * wt;

    assert(is_dispatch_thread());
    wt = (WorkerThread *)loc_alloc_zero(sizeof(WorkerThread));
    wt->req = req;
    list_add_last(&wt->link_all, &worker_all);
    check_error(pthread_cond_init(&wt->cond, NULL));
    check_error(pthread_create(&wt->thread, &pthread_create_attr, worker_thread_handler, wt));
    if (worker_count++ == 0) shutdown_set_normal(&async_shutdown);
    trace(LOG_ASYNCREQ, "worker_thread_add %p running threads %d", wt, worker_count);
}

static void worker_thread_add_deferred(void * x) {
    AsyncReqInfo * req = (AsyncReqInfo *)x;

    check_error(pthread_mutex_lock(&wtlock));
    worker_thread_add(req);
    check_error(pthread_mutex_unlock(&wtlock));
}

#if ENABLE_AIO
static void aio_done(union sigval arg) {
    AsyncReqInfo * req = (AsyncReqInfo *)arg.sival_ptr;
    req->u.fio.rval = aio_return(&req->u.fio.aio);
    if (req->u.fio.rval < 0) req->error = aio_error(&req->u.fio.aio);
    post_event(req->done, req);
}
#endif

void async_req_post(AsyncReqInfo * req) {
    WorkerThread * wt;

    trace(LOG_ASYNCREQ, "async_req_post: req %p, type %d", req, req->type);
    assert(req->done != NULL || req->type == AsyncReqTimer);

#if ENABLE_AIO
    {
        int res = 0;
        switch (req->type) {
        case AsyncReqSeekRead:
        case AsyncReqSeekWrite:
            memset(&req->u.fio.aio, 0, sizeof(req->u.fio.aio));
            req->u.fio.aio.aio_fildes = req->u.fio.fd;
            req->u.fio.aio.aio_offset = (off_t)req->u.fio.offset;
            req->u.fio.aio.aio_buf = req->u.fio.bufp;
            req->u.fio.aio.aio_nbytes = req->u.fio.bufsz;
            req->u.fio.aio.aio_sigevent.sigev_notify = SIGEV_THREAD;
            req->u.fio.aio.aio_sigevent.sigev_notify_function = aio_done;
            req->u.fio.aio.aio_sigevent.sigev_value.sival_ptr = req;
            res = req->type == AsyncReqSeekWrite ?
                aio_write(&req->u.fio.aio) :
                aio_read(&req->u.fio.aio);
            if (res < 0) {
                req->u.fio.rval = -1;
                req->error = errno;
                post_event(req->done, req);
            }
            return;
        }
    }
#endif
    check_error(pthread_mutex_lock(&wtlock));
    if (list_is_empty(&worker_idle)) {
        assert(idle_count == 0);
        if (is_dispatch_thread()) {
            worker_thread_add(req);
        }
        else {
            post_event(worker_thread_add_deferred, req);
        }
    }
    else {
        assert(idle_count > 0);
        wt = idle2worker(worker_idle.next);
        list_remove(&wt->link_idle);
        idle_count--;
        assert(wt->req == NULL);
        wt->req = req;
        check_error(pthread_cond_signal(&wt->cond));
    }
    check_error(pthread_mutex_unlock(&wtlock));
}

static void start_timer(void * args) {
    memset(&timer_req, 0, sizeof(timer_req));
    timer_req.type = AsyncReqTimer;
    async_req_post(&timer_req);
}

void ini_asyncreq(void) {
    check_error(pthread_mutex_init(&wtlock, NULL));
    post_event(start_timer, NULL);
}
