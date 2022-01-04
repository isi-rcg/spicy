/*
 * decompress.c: decompression abstraction layer
 *
 * Copyright (C) 2007, 2008 Colin Watson.
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef HAVE_LIBZ
#  include "zlib.h"
#endif /* HAVE_LIBZ */

#include "xvasprintf.h"

#include "manconfig.h"
#include "comp_src.h"
#include "pipeline.h"
#include "decompress.h"
#include "sandbox.h"

#ifdef HAVE_LIBZ

static void decompress_zlib (void *data _GL_UNUSED)
{
	gzFile zlibfile;
	int fd;

	fd = dup (STDIN_FILENO);
	if (fd < 0)
		return;

	zlibfile = gzdopen (fd, "r");
	if (!zlibfile) {
		close (fd);
		return;
	}

	for (;;) {
		char buffer[4096];
		int r = gzread (zlibfile, buffer, 4096);
		if (r <= 0)
			break;
		if (fwrite (buffer, 1, (size_t) r, stdout) < (size_t) r)
			break;
	}

	gzclose (zlibfile);
	return;
}

#endif /* HAVE_LIBZ */

extern man_sandbox *sandbox;

pipeline *decompress_open (const char *filename)
{
	pipecmd *cmd;
	pipeline *p;
	struct stat st;
#ifdef HAVE_LIBZ
	size_t filename_len;
#endif /* HAVE_LIBZ */
	char *ext;
	struct compression *comp;

	if (stat (filename, &st) < 0 || S_ISDIR (st.st_mode))
		return NULL;

#ifdef HAVE_LIBZ
	filename_len = strlen (filename);
	if (filename_len > 3 && STREQ (filename + filename_len - 3, ".gz")) {
		cmd = pipecmd_new_function ("zcat", &decompress_zlib, NULL,
					    NULL);
		pipecmd_pre_exec (cmd, sandbox_load, sandbox_free, sandbox);
		p = pipeline_new_commands (cmd, (void *) 0);
		goto got_pipeline;
	}
#endif /* HAVE_LIBZ */

	ext = strrchr (filename, '.');
	if (ext) {
		++ext;

		for (comp = comp_list; comp->ext; ++comp) {
			if (!STREQ (comp->ext, ext))
				continue;

			cmd = pipecmd_new_argstr (comp->prog);
			pipecmd_pre_exec (cmd, sandbox_load, sandbox_free,
					  sandbox);
			p = pipeline_new_commands (cmd, (void *) 0);
			goto got_pipeline;
		}
	}

#ifdef HAVE_GZIP
	/* HP-UX */
	ext = strstr (filename, ".Z/");
	if (ext) {
		cmd = pipecmd_new_argstr (GUNZIP);
		pipecmd_pre_exec (cmd, sandbox_load, sandbox_free, sandbox);
		p = pipeline_new_commands (cmd, (void *) 0);
		goto got_pipeline;
	}
#endif

	p = pipeline_new ();

got_pipeline:
	pipeline_want_infile (p, filename);
	pipeline_want_out (p, -1);
	return p;
}

pipeline *decompress_fdopen (int fd)
{
	pipeline *p;
#ifdef HAVE_LIBZ
	pipecmd *cmd;
#endif /* HAVE_LIBZ */

#ifdef HAVE_LIBZ
	cmd = pipecmd_new_function ("zcat", &decompress_zlib, NULL, NULL);
	pipecmd_pre_exec (cmd, sandbox_load, sandbox_free, sandbox);
	p = pipeline_new_commands (cmd, (void *) 0);
#else /* HAVE_LIBZ */
	p = pipeline_new ();
#endif /* HAVE_LIBZ */

	pipeline_want_in (p, fd);
	pipeline_want_out (p, -1);
	return p;
}
