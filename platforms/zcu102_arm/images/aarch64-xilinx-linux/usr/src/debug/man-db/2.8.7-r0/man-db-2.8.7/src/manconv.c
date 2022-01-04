/*
 * manconv.c: convert manual page from one encoding to another
 *
 * Copyright (C) 2007, 2008, 2009, 2010, 2012 Colin Watson.
 * Based loosely on parts of glibc's iconv_prog.c, which is:
 * Copyright (C) 1998-2004, 2005, 2006, 2007 Free Software Foundation, Inc.
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

/* This program arose during a discussion with Adam Borowski. See:
 *   https://lists.debian.org/debian-mentors/2007/09/msg00245.html
 * It behaves like iconv, but allows multiple source encodings and
 * attempts to guess the first one that works. An Emacs-style
 * "-*- coding:" declaration overrides this.
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>

#ifdef HAVE_ICONV
#  include <iconv.h>
#endif /* HAVE_ICONV */

#include "argp.h"

#include "gettext.h"
#include <locale.h>
#define _(String) gettext (String)

#include "manconfig.h"

#include "error.h"
#include "pipeline.h"
#include "encodings.h"

#include "manconv.h"

#ifdef HAVE_ICONV

/* When converting text containing an invalid multibyte sequence to
 * UTF-8//IGNORE, GNU libc's iconv returns EILSEQ but sets *inbuf to the end
 * of the input buffer.  I'm not sure whether this is a bug or not (it seems
 * to contradict the documentation), but work around it anyway by recoding
 * to UTF-8 so that we can accurately position the error.
 */
static off_t locate_error (const char *try_from_code,
			   const char *input, size_t input_size,
			   char *utf8, size_t utf8_size)
{
	iconv_t cd_utf8_strict;
	char *inptr = (char *) input, *utf8ptr = utf8;
	size_t inleft = input_size, utf8left = utf8_size;
	size_t n;
	off_t ret;

	cd_utf8_strict = iconv_open ("UTF-8", try_from_code);
	if (cd_utf8_strict == (iconv_t) -1) {
		error (0, errno, "iconv_open (\"UTF-8\", \"%s\")",
		       try_from_code);
		return 0;
	}

	n = iconv (cd_utf8_strict, (ICONV_CONST char **) &inptr, &inleft,
		   &utf8ptr, &utf8left);
	if (n == (size_t) -1)
		ret = inptr - input;
	else
		ret = 0;

	iconv_close (cd_utf8_strict);

	return ret;
}

static int try_iconv (pipeline *p, const char *try_from_code, const char *to,
		      int last)
{
	char *try_to_code = xstrdup (to);
	static const size_t buf_size = 65536;
	size_t input_size = buf_size;
	off_t input_pos = 0;
	const char *input;
	static char *utf8 = NULL, *output = NULL;
	size_t utf8left = 0;
	iconv_t cd_utf8, cd = NULL;
	int to_utf8 = STREQ (try_to_code, "UTF-8") ||
		      STRNEQ (try_to_code, "UTF-8//", 7);
	const char *utf8_target = last ? "UTF-8//IGNORE" : "UTF-8";
	int ignore_errors = (strstr (try_to_code, "//IGNORE") != NULL);;
	int ret = 0;

	debug ("trying encoding %s -> %s\n", try_from_code, try_to_code);

	cd_utf8 = iconv_open (utf8_target, try_from_code);
	if (cd_utf8 == (iconv_t) -1) {
		error (0, errno, "iconv_open (\"%s\", \"%s\")",
		       utf8_target, try_from_code);
		free (try_to_code);
		return -1;
	}

	if (!to_utf8) {
		cd = iconv_open (try_to_code, "UTF-8");
		if (cd == (iconv_t) -1) {
			error (0, errno, "iconv_open (\"%s\", \"UTF-8\")",
			       try_to_code);
			free (try_to_code);
			return -1;
		}
	}

	input = pipeline_peek (p, &input_size);
	if (input_size < buf_size) {
		/* End of file, error, or just a short read? Repeat until we
		 * have either a full buffer or EOF/error.
		 */
		while (input_size < buf_size) {
			size_t old_input_size = input_size;
			input_size = buf_size;
			input = pipeline_peek (p, &input_size);
			if (input_size == old_input_size)
				break;
		}
	}

	if (!utf8)
		utf8 = xmalloc (buf_size);
	if (!output)
		output = xmalloc (buf_size);

	while (input_size || utf8left) {
		int handle_iconv_errors = 0;
		char *inptr = (char *) input, *utf8ptr = utf8;
		char *outptr = output;
		size_t inleft = input_size, outleft;
		size_t n, n2 = -1;

		if (!utf8left) {
			/* First, convert the text to UTF-8. By assumption,
			 * all validly-encoded text can be converted to
			 * UTF-8 assuming that we picked the correct
			 * encoding. Any errors at this stage are due to
			 * selecting an incorrect encoding, or due to
			 * misencoded source text.
			 */
			utf8left = buf_size;
			n = iconv (cd_utf8, (ICONV_CONST char **) &inptr,
				   &inleft, &utf8ptr, &utf8left);
			utf8left = buf_size - utf8left;

			/* If we need to try the next encoding, do that
			 * before writing anything.
			 */
			if (!last && n == (size_t) -1 &&
			    (errno == EILSEQ ||
			     (errno == EINVAL && input_size < buf_size))) {
				ret = -1;
				break;
			} else if (n == (size_t) -1)
				handle_iconv_errors = errno;
		}

		/* If the target encoding is UTF-8 (the common case), then
		 * we can just write out what we've got. Otherwise, we need
		 * to convert to the target encoding. Any errors at this
		 * stage are due to characters that are not representable in
		 * the target encoding.
		 */
		if (handle_iconv_errors)
			/* Fall back to error handling below.  If we have
			 * anything to write out, we'll do it next time
			 * round the loop.
			 */
			;
		else if (to_utf8) {
			memcpy (output, utf8, utf8left);
			outptr += utf8left;
			outleft = utf8left;
			utf8left = 0;
		} else if (utf8left) {
			outptr = output;
			outleft = buf_size;
			utf8ptr = utf8;
			n2 = iconv (
				cd, (ICONV_CONST char **) &utf8ptr, &utf8left,
				&outptr, &outleft);
			outleft = buf_size - outleft;
			if (n2 == (size_t) -1)
				handle_iconv_errors = errno;

			if (n2 == (size_t) -1 &&
			    errno == EILSEQ && ignore_errors)
				errno = 0;
		} else
			/* We appear to have converted some input text, but
			 * not actually ended up with any UTF-8 text.  This
			 * is odd.  However, we can at least continue round
			 * the loop, skip the input text we converted, and
			 * then we should get a different result next time.
			 */
			outptr = output;

		if (outptr != output) {
			/* We have something to write out. */
			int errno_save = errno;
			size_t w;
			w = fwrite (output, 1, outleft, stdout);
			if (w < (size_t) outleft || ferror (stdout))
				error (FATAL, 0, _("can't write to "
						   "standard output"));
			errno = errno_save;
		}

		if (!to_utf8 && n2 != (size_t) -1) {
			/* All the UTF-8 text we have so far was processed.
			 * For state-dependent character sets we have to
			 * flush the state now.
			 */
			outptr = output;
			outleft = buf_size;
			iconv (cd, NULL, NULL, &outptr, &outleft);
			outleft = buf_size - outleft;

			if (outptr != output) {
				/* We have something to write out. */
				int errno_save = errno;
				size_t w;
				w = fwrite (output, 1, outleft, stdout);
				if (w < (size_t) outleft || ferror (stdout))
					error (FATAL, 0, _("can't write to "
							   "standard output"));
				errno = errno_save;
			}
		} else if (handle_iconv_errors) {
			intmax_t error_pos;

			if (handle_iconv_errors == EILSEQ && !ignore_errors) {
				if (!quiet) {
					error_pos = input_pos + locate_error (
						try_from_code,
						input, input_size,
						utf8, buf_size);
					error (0, handle_iconv_errors,
					       "byte %jd: iconv", error_pos);
				}
				exit (FATAL);
			} else if (handle_iconv_errors == EINVAL &&
				   input_size < buf_size) {
				if (!quiet) {
					error_pos = input_pos + locate_error (
						try_from_code,
						input, input_size,
						utf8, buf_size);
					error (FATAL, 0, "byte %jd: %s",
					       error_pos,
					       _("iconv: incomplete character "
						 "at end of buffer"));
				}
				exit (FATAL);
			}
		}

		if (inptr != input) {
			pipeline_peek_skip (p, input_size - inleft);
			input_pos += input_size - inleft;
		}

		/* Unless we have some UTF-8 text left (which will only
		 * happen if the output encoding is more verbose than UTF-8,
		 * so is unlikely for legacy encodings), we need to fetch
		 * more input text now.
		 */
		if (!utf8left) {
			input_size = buf_size;
			input = pipeline_peek (p, &input_size);
			while (input_size < buf_size) {
				size_t old_input_size = input_size;
				input_size = buf_size;
				input = pipeline_peek (p, &input_size);
				if (input_size == old_input_size)
					break;
			}
		}
	}

	if (!to_utf8)
		iconv_close (cd);
	iconv_close (cd_utf8);
	free (try_to_code);

	return ret;
}

void manconv (pipeline *p, char * const *from, const char *to)
{
	char *pp_encoding;
	char * const *try_from_code;

	pp_encoding = check_preprocessor_encoding (p);
	if (pp_encoding) {
		try_iconv (p, pp_encoding, to, 1);
		free (pp_encoding);
	} else {
		for (try_from_code = from; *try_from_code; ++try_from_code)
			if (try_iconv (p, *try_from_code, to,
				       !*(try_from_code + 1)) == 0)
				break;
	}
}

#else /* !HAVE_ICONV */

/* If we don't have iconv, there isn't much we can do; just pass everything
 * through unchanged.
 */
void manconv (pipeline *p, char * const *from _GL_UNUSED,
	      const char *to _GL_UNUSED)
{
	for (;;) {
		size_t len = 4096;
		const char *buffer = pipeline_read (p, &len);
		if (len == 0)
			break;
		if (fwrite (buffer, 1, len, stdout) < len || ferror (stdout))
			error (FATAL, 0, _("can't write to standard output"));
	}
}

#endif /* HAVE_ICONV */
