/*
 * Copyright (c) 2008-2010, 2012 Wind River Systems; see
 * guts/COPYRIGHT for information.
 *
 * Copyright (c) 2018 Peter Seebach; see
 * guts/COPYRIGHT for information.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 * int mkostemps(char *template, int suffixlen, int oflags)
 *	int rc = -1;
 */

	PSEUDO_STATBUF buf;
 	int save_errno;
	size_t len;
	char *tmp_template;

	if (!template) {
		errno = EFAULT;
		return 0;
	}

	len = strlen(template);
	tmp_template = PSEUDO_ROOT_PATH(AT_FDCWD, template, AT_SYMLINK_NOFOLLOW);

	if (!tmp_template) {
		errno = ENOENT;
		return -1;
	}

	rc = real_mkostemps(tmp_template, suffixlen, oflags);

	if (rc != -1) {
		save_errno = errno;

		if (base_fstat(rc, &buf) != -1) {
			real_fchmod(rc, PSEUDO_FS_MODE(0600, 0));
			pseudo_client_op(OP_CREAT, 0, -1, -1, tmp_template, &buf);
			pseudo_client_op(OP_OPEN, PSA_READ | PSA_WRITE, rc, -1, tmp_template, &buf);
		} else {
			pseudo_debug(PDBGF_CONSISTENCY, "mkstemp (fd %d) succeeded, but fstat failed (%s).\n",
				rc, strerror(errno));
			pseudo_client_op(OP_OPEN, PSA_READ | PSA_WRITE, rc, -1, tmp_template, 0);
		}
		errno = save_errno;
	}
	/* WARNING: At least one system allows the number of Xs to be something
	 * other than 6. I do not attempt to handle this.
	 */
	/* mkstemp only changes the XXXXXX at the end, or suffixlen characters before
	 * the end if mkostemp/mkostemps.
	 */
	memcpy(template + len - 6 - suffixlen, tmp_template + strlen(tmp_template) - 6 - suffixlen, 6);
/*	return rc;
 * }
 */
