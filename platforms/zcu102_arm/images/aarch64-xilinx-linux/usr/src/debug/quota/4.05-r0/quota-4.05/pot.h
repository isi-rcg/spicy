#ifndef GUARD_POT_H
#define GUARD_POT_H

#ifdef USE_GETTEXT

#include <libintl.h>

#define _(x)	gettext((x))

#else

#define _(x) 	(x)

#endif

void gettexton(void);

#endif
