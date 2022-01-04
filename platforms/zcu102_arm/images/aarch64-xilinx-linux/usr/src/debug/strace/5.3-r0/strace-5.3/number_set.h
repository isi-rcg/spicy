/*
 * Copyright (c) 2016-2018 Dmitry V. Levin <ldv@altlinux.org>
 * Copyright (c) 2017-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef STRACE_NUMBER_SET_H
# define STRACE_NUMBER_SET_H

# include "gcc_compat.h"

struct number_set;

extern bool
number_set_array_is_empty(const struct number_set *, unsigned int idx);

extern bool
is_number_in_set(unsigned int number, const struct number_set *);

extern bool
is_number_in_set_array(unsigned int number, const struct number_set *, unsigned int idx);

extern bool
is_complete_set(const struct number_set *, unsigned int max_numbers);

extern bool
is_complete_set_array(const struct number_set *, const unsigned int *,
		      const unsigned int nmemb);

extern void
add_number_to_set(unsigned int number, struct number_set *);

extern void
add_number_to_set_array(unsigned int number, struct number_set *, unsigned int idx);

extern void
clear_number_set_array(struct number_set *, unsigned int nmemb);

extern void
invert_number_set_array(struct number_set *, unsigned int nmemb);

extern struct number_set *
alloc_number_set_array(unsigned int nmemb) ATTRIBUTE_MALLOC;

extern void
free_number_set_array(struct number_set *, unsigned int nmemb);

enum status_t {
	STATUS_SUCCESSFUL,
	STATUS_FAILED,
	STATUS_UNFINISHED,
	STATUS_UNAVAILABLE,
	STATUS_DETACHED,
	NUMBER_OF_STATUSES
};

extern struct number_set *read_set;
extern struct number_set *write_set;
extern struct number_set *signal_set;
extern struct number_set *status_set;
extern struct number_set *trace_set;

#endif /* !STRACE_NUMBER_SET_H */
