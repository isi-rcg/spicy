/*******************************************************************************
 * Copyright (c) 2007, 2014 Wind River Systems, Inc. and others.
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

#ifndef D_filesystem
#define D_filesystem

#include <tcf/framework/protocol.h>

/*
 * Open modes
 */
static const int
    TCF_O_READ              = 0x00000001,
    TCF_O_WRITE             = 0x00000002,
    TCF_O_APPEND            = 0x00000004,
    TCF_O_CREAT             = 0x00000008,
    TCF_O_TRUNC             = 0x00000010,
    TCF_O_EXCL              = 0x00000020;

/*
 * Error codes
 */
static const int
    FSERR_EOF               = 0x10001,
    FSERR_NO_SUCH_FILE      = 0x10002,
    FSERR_PERMISSION_DENIED = 0x10003,
    FSERR_EPERM             = 0x10004,
    FSERR_ESRCH             = 0x10005,
    FSERR_EINTR             = 0x10006,
    FSERR_EIO               = 0x10007,
    FSERR_ENXIO             = 0x10008,
    FSERR_E2BIG             = 0x10009,
    FSERR_ENOEXEC           = 0x1000a,
    FSERR_EBADF             = 0x1000b,
    FSERR_ECHILD            = 0x1000c,
    FSERR_EAGAIN            = 0x1000d,
    FSERR_ENOMEM            = 0x1000e,
    FSERR_EFAULT            = 0x1000f,
    FSERR_EBUSY             = 0x10010,
    FSERR_EEXIST            = 0x10011,
    FSERR_EXDEV             = 0x10012,
    FSERR_ENODEV            = 0x10013,
    FSERR_ENOTDIR           = 0x10014,
    FSERR_EISDIR            = 0x10015,
    FSERR_EINVAL            = 0x10016,
    FSERR_ENFILE            = 0x10017,
    FSERR_EMFILE            = 0x10018,
    FSERR_ENOTTY            = 0x10019,
    FSERR_EFBIG             = 0x1001a,
    FSERR_ENOSPC            = 0x1001b,
    FSERR_ESPIPE            = 0x1001c,
    FSERR_EROFS             = 0x1001d,
    FSERR_EMLINK            = 0x1001e,
    FSERR_EPIPE             = 0x1001f,
    FSERR_EDEADLK           = 0x10020,
    FSERR_ENOLCK            = 0x10021,
    FSERR_EDOM              = 0x10022,
    FSERR_ERANGE            = 0x10023,
    FSERR_ENOSYS            = 0x10024,
    FSERR_ENOTEMPTY         = 0x10025,
    FSERR_ENAMETOOLONG      = 0x10026,
    FSERR_ENOBUFS           = 0x10027,
    FSERR_EISCONN           = 0x10028,
    FSERR_ENOSTR            = 0x10029,
    FSERR_EPROTO            = 0x1002a,
    FSERR_EBADMSG           = 0x1002b,
    FSERR_ENODATA           = 0x1002c,
    FSERR_ETIME             = 0x1002d,
    FSERR_ENOMSG            = 0x1002e,
    FSERR_ETXTBSY           = 0x1002f,
    FSERR_ELOOP             = 0x10030,
    FSERR_ENOTBLK           = 0x10031,
    FSERR_EMSGSIZE          = 0x10032,
    FSERR_EDESTADDRREQ      = 0x10033,
    FSERR_EPROTOTYPE        = 0x10034,
    FSERR_ENOTCONN          = 0x10035,
    FSERR_ESHUTDOWN         = 0x10036,
    FSERR_ECONNRESET        = 0x10037,
    FSERR_EOPNOTSUPP        = 0x10038,
    FSERR_EAFNOSUPPORT      = 0x10039,
    FSERR_EPFNOSUPPORT      = 0x1003a,
    FSERR_EADDRINUSE        = 0x1003b,
    FSERR_ENOTSOCK          = 0x1003c,
    FSERR_ENETUNREACH       = 0x1003d,
    FSERR_ENETRESET         = 0x1003e,
    FSERR_ECONNABORTED      = 0x1003f,
    FSERR_ETOOMANYREFS      = 0x10040,
    FSERR_ETIMEDOUT         = 0x10041,
    FSERR_ECONNREFUSED      = 0x10042,
    FSERR_ENETDOWN          = 0x10043,
    FSERR_EHOSTUNREACH      = 0x10044,
    FSERR_EHOSTDOWN         = 0x10045,
    FSERR_EINPROGRESS       = 0x10046,
    FSERR_EALREADY          = 0x10047,
    FSERR_ECANCELED         = 0x10048,
    FSERR_EPROTONOSUPPORT   = 0x10049,
    FSERR_ESOCKTNOSUPPORT   = 0x1004a,
    FSERR_EADDRNOTAVAIL     = 0x1004b;

/*
 * Initialize file system service.
 */
extern void ini_file_system_service(Protocol *);

#endif
