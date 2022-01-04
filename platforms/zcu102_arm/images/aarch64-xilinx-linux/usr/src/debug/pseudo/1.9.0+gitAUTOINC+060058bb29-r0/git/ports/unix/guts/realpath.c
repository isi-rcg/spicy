/* 
 * Copyright (c) 2010 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * static char *
 * wrap_realpath(const char *name, char *resolved_name) {
 *	char * rc = NULL;
 */
	char *rname = PSEUDO_ROOT_PATH(AT_FDCWD, name, 0);
	ssize_t len;
	if (!rname) {
		errno = ENAMETOOLONG;
		return NULL;
	}
		len = strlen(rname);
		char *ep = rname + len - 1;
		while (ep > rname && *ep == '/') {
			--len;
			*(ep--) = '\0';
		}

		if (len >= pseudo_sys_path_max()) {
		errno = ENAMETOOLONG;
		return NULL;
	}
	if (resolved_name) {
		memcpy(resolved_name, rname, len + 1);
		rc = resolved_name;
	} else {
		rc = strdup(rname);
	}

/*	return rc;
 * }
 */
