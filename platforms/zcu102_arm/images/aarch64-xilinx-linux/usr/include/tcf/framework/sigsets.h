/*******************************************************************************
 * Copyright (c) 2013 Xilinx, Inc. and others.
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
 *     Xilinx - initial API and implementation
 *******************************************************************************/

/*
 * SigSet type definition.
 * The type used to provide support for more than 32 signals,
 * e.g. POSIX real-time signals and Windows exceptions.
 */

#ifndef D_sigset
#define D_sigset

#if !defined(ENABLE_UnlimitedSignals)
#  define ENABLE_UnlimitedSignals 1
#endif

#if ENABLE_UnlimitedSignals

typedef struct SigSet {
    unsigned cnt;
    unsigned max;
    unsigned * buf;
} SigSet;

#else

/* Backward compatible definition of signal set type,
 * does not support signal numbers beyond 32 */
typedef unsigned long SigSet;

#endif

extern int sigset_is_empty(SigSet * set);
extern int sigset_get(SigSet * set, unsigned bit);
extern int sigset_get_next(SigSet * set, unsigned * bit);
extern void sigset_set(SigSet * set, unsigned bit, int value);
extern void sigset_copy(SigSet * dst, SigSet * src);
extern void sigset_clear(SigSet * set);

#endif /* D_sigset */
