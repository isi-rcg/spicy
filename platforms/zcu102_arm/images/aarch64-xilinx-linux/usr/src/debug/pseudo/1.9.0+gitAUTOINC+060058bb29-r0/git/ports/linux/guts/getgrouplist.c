/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static int
 * wrap_getgrouplist(const char *user, gid_t group, gid_t *groups, int *ngroups) {
 *	int rc = -1;
 */

	int found = 0;
	int found_group = 0;
	size_t buflen = PSEUDO_PWD_MAX;
	char *buf = NULL;
	struct group grp;

	rc = ERANGE;

	do {
		struct group *gbuf = &grp;
		char *new_buf = buf;

		if (rc == ERANGE)
			new_buf = realloc(buf, buflen);

		if (!new_buf) {
			rc = ENOMEM;
			break;
		}

		buf = new_buf;

		found = 0;
		found_group = 0;
		setgrent();
		while ((rc = wrap_getgrent_r(gbuf, buf, buflen, &gbuf)) == 0) {
			int i = 0;
			for (i = 0; gbuf->gr_mem[i]; ++i) {
				if (!strcmp(gbuf->gr_mem[i], user)) {
					if (found < *ngroups)
						groups[found] = gbuf->gr_gid;
					++found;
					if (gbuf->gr_gid == group)
						found_group = 1;
				}
			}
		}
		endgrent();

		if (rc == ERANGE)
			buflen = buflen << 1;
	} while (rc == ERANGE);
	free(buf);
	if (!found_group) {
		if (found < *ngroups)
			groups[found] = group;
		++found;
	}
	if (found >= *ngroups)
		rc = -1;
	else
		rc = found;
	*ngroups = found;

/*	return rc;
 * }
 */
