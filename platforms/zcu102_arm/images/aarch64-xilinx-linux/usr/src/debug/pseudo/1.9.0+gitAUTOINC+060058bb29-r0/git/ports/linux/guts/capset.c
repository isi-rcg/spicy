/*
 * Copyright (c) 2016 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int capset(cap_user_header_t hdrp, const cap_user_data_t datap)
 *	int rc = -1;
 */

	rc = real_capset(hdrp, datap);

/*	return rc;
 * }
 */
