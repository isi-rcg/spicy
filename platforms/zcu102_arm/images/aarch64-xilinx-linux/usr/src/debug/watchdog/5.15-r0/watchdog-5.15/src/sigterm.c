/* > sigterm.c
 *
 * Used to tell the main() code to exit gracefully. Needed by both
 * the watchdog and wd_keepalive daemons.
 */

#include <signal.h>
#include "extern.h"

volatile sig_atomic_t _running = 1;

void sigterm_handler(int arg)
{
	_running = 0;
}
