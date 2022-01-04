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


#include <tcf/config.h>

#include <assert.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/sigsets.h>

#if ENABLE_UnlimitedSignals

static unsigned search(SigSet * set, unsigned bit) {
    unsigned l = 0;
    unsigned h = set->cnt;
    while (l < h) {
        unsigned k = (l + h) / 2;
        if (set->buf[k] > bit) {
            h = k;
        }
        else if (set->buf[k] < bit) {
            l = k + 1;
        }
        else {
            return k;
        }
    }
    return l;
}

int sigset_is_empty(SigSet * set) {
    return set->cnt == 0;
}

int sigset_get(SigSet * set, unsigned bit) {
    unsigned n = search(set, bit);
    return n < set->cnt && set->buf[n] == bit;
}

int sigset_get_next(SigSet * set, unsigned * bit) {
    unsigned n = search(set, *bit + 1);
    if (n >= set->cnt) return 0;
    *bit = set->buf[n];
    return 1;
}

void sigset_set(SigSet * set, unsigned bit, int value) {
    unsigned n = search(set, bit);
    assert(bit > 0 || (bit == 0 && value == 0));
    assert(n <= set->cnt);
    if (value) {
        if (n < set->cnt && set->buf[n] == bit) return;
        if (set->cnt >= set->max) {
            set->max += 8;
            set->buf = (unsigned *)loc_realloc(set->buf, sizeof(unsigned) * set->max);
        }
        memmove(set->buf + n + 1, set->buf + n, sizeof(unsigned) * (set->cnt - n));
        set->buf[n] = bit;
        set->cnt++;
    }
    else {
        if (n >= set->cnt || set->buf[n] != bit) return;
        memmove(set->buf + n, set->buf + n + 1, sizeof(unsigned) * (set->cnt - n - 1));
        set->cnt--;
    }
}

void sigset_copy(SigSet * dst, SigSet * src) {
    dst->cnt = src->cnt;
    dst->max = src->cnt;
    dst->buf = NULL;
    if (dst->max > 0) {
        dst->buf = (unsigned *)loc_alloc(sizeof(unsigned) * dst->max);
        memcpy(dst->buf, src->buf, sizeof(unsigned) * dst->cnt);
    }
}

void sigset_clear(SigSet * set) {
    loc_free(set->buf);
    memset(set, 0, sizeof(SigSet));
}

#else

int sigset_is_empty(SigSet * set) {
    return *set == 0;
}

int sigset_get(SigSet * set, unsigned bit) {
    assert(bit < 32);
    return (*set & (1ul << bit)) != 0;
}

int sigset_get_next(SigSet * set, unsigned * bit) {
    (*bit)++;
    while (*bit < 32) {
        if (*bit > 0 && (*set & (1ul << *bit)) != 0) return 1;
        (*bit)++;
    }
    return 0;
}

void sigset_set(SigSet * set, unsigned bit, int value) {
    assert((bit > 0 && bit < 32) || (bit == 0 && value == 0));
    if (value) {
        *set |= 1ul << bit;
    }
    else {
        *set &= ~(1ul << bit);
    }
}

void sigset_copy(SigSet * dst, SigSet * src) {
    *dst = *src;
}

void sigset_clear(SigSet * set) {
    *set = 0;
}

#endif /* ENABLE_UnlimitedSignals */
