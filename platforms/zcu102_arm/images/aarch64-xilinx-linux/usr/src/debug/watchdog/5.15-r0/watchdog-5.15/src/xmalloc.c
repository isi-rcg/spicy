/* > xmalloc.c
 *
 * Versions of memory allocation that exit with error message if failure occurs.
 * Based on old sundries.c but with some minor improvements and the code
 * in it that was for mount.c/umount.c support removed.
 *
 * (c) 2013 Paul S. Crawford (psc@sat.dundee.ac.uk) licensed under GPL v2, based
 * on older code in sundries.c
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>

#include "xmalloc.h"
#include "logmessage.h"

void *xmalloc(size_t size)
{
	void *t;

	if (size == 0)
		return NULL;

	t = malloc(size);
	if (t == NULL)
		fatal_error(EX_SYSERR, "xmalloc failed for %lu bytes", (unsigned long)size);

	return t;
}

void *xcalloc(size_t nmemb, size_t size)
{
	void *t;

	if (nmemb == 0 || size == 0)
		return NULL;

	t = calloc(nmemb, size);
	if (t == NULL)
		fatal_error(EX_SYSERR, "xcalloc failed for %lu x %lu bytes", (unsigned long)nmemb, (unsigned long)size);

	return t;
}

char *xstrdup(const char *s)
{
	char *t;

	if (s == NULL)
		return NULL;

	t = strdup(s);
	if (t == NULL)
		fatal_error(EX_SYSERR, "xstrdup failed for %lu byte string", (unsigned long)strlen(s));

	return t;
}

char *xstrndup(const char *s, int n)
{
	char *t;

	if (s == NULL || n < 0)
		fatal_error(EX_SOFTWARE, "bad xstrndup call (%sn = %d)", s == NULL ? "" : "s = NULL, ", n);

	t = xmalloc(n + 1);
	strncpy(t, s, n);
	t[n] = 0;

	return t;
}
