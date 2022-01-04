
#include "Python.h"

#ifndef PLATFORM
#define PLATFORM "unknown"
#endif

const char *
Py_GetPlatform(void)
{
	return PLATFORM;
}

#ifndef LIB
#define LIB "lib"
#endif

const char *
Py_GetLib(void)
{
	return LIB;
}
