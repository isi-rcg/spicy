#ifndef _XMALLOC_H
#define _XMALLOC_H

void *xmalloc (size_t size);
void *xcalloc (size_t nmemb, size_t size);
char *xstrdup (const char *s);
char *xstrndup (const char *s, int n);

#endif /*_XMALLOC_H*/
