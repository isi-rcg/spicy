/* > errorcodes.c
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <string.h>
#include <sys/wait.h>

#include "watch_err.h"
#include "extern.h"

/*
 * Extend the operation of the system's strerror() error-to-text mapping function to
 * include errors that are specific to the watchdog code.
 */

const char *wd_strerror(int err)
{
	char *str = "";

	switch (err) {
		case ENOERR:		str = "no error"; break;
		case EREBOOT:		str = "unconditional reboot requested"; break;
		case ERESET:		str = "unconditional hard reset requested"; break;
		case EMAXLOAD:		str = "load average too high"; break;
		case ETOOHOT:		str = "too hot"; break;
		case ENOLOAD:		str = "loadavg contains no data"; break;
		case ENOCHANGE:		str = "file was not changed in the given interval"; break;
		case EINVMEM:		str = "meminfo contains invalid data"; break;
		case ECHKILL:		str = "child process was killed by signal"; break;
		case ETOOLONG:		str = "child process did not return in time"; break;
		case EUSERVALUE:	str = "user-reserved code"; break;
		case EDONTKNOW:		str = "unknown (neither good nor bad)"; break;
		default:			str = strerror(err); break;
	}

	return str;
}
