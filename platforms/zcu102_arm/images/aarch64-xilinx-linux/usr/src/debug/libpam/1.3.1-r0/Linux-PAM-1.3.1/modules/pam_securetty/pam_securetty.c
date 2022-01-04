/* pam_securetty module */

#define CMDLINE_FILE   "/proc/cmdline"
#define CONSOLEACTIVE_FILE	"/sys/class/tty/console/active"

/*
 * by Elliot Lee <sopwith@redhat.com>, Red Hat Software.
 * July 25, 1996.
 * This code shamelessly ripped from the pam_rootok module.
 * Slight modifications AGM. 1996/12/3
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>
#include <stdarg.h>
#include <pwd.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

/*
 * here, we make a definition for the externally accessible function
 * in this file (this definition is required for static a module
 * but strongly encouraged generally) it is used to instruct the
 * modules include file to define the function prototypes.
 */

#define PAM_SM_AUTH
#define PAM_SM_ACCOUNT

#include <security/pam_modules.h>
#include <security/pam_modutil.h>
#include <security/pam_ext.h>

extern int _pammodutil_tty_secure(const pam_handle_t *pamh,
                                  const char *uttyname);

#define PAM_DEBUG_ARG       0x0001
#define PAM_NOCONSOLE_ARG   0x0002

static int
_pam_parse (const pam_handle_t *pamh, int argc, const char **argv)
{
    int ctrl=0;

    /* step through arguments */
    for (ctrl=0; argc-- > 0; ++argv) {

	/* generic options */

	if (!strcmp(*argv,"debug"))
	    ctrl |= PAM_DEBUG_ARG;
        else if (!strcmp(*argv, "noconsole"))
            ctrl |= PAM_NOCONSOLE_ARG;
	else {
	    pam_syslog(pamh, LOG_ERR, "unknown option: %s", *argv);
	}
    }

    return ctrl;
}

static int
securetty_perform_check (pam_handle_t *pamh, int ctrl,
			 const char *function_name)
{
    int retval = PAM_AUTH_ERR;
    const char *username;
    const char *uttyname;
    const void *void_uttyname;
    struct passwd *user_pwd;

    /* log a trail for debugging */
    if (ctrl & PAM_DEBUG_ARG) {
        pam_syslog(pamh, LOG_DEBUG, "pam_securetty called via %s function",
		   function_name);
    }

    retval = pam_get_user(pamh, &username, NULL);
    if (retval != PAM_SUCCESS || username == NULL) {
        pam_syslog(pamh, LOG_WARNING, "cannot determine username");
	return (retval == PAM_CONV_AGAIN ? PAM_INCOMPLETE:PAM_SERVICE_ERR);
    }

    user_pwd = pam_modutil_getpwnam(pamh, username);
    if (user_pwd != NULL && user_pwd->pw_uid != 0) {
	/* If the user is not root, securetty's does not apply to them */
	return PAM_SUCCESS;
    }
    /* The user is now either root or an invalid / mistyped username */

    retval = pam_get_item(pamh, PAM_TTY, &void_uttyname);
    uttyname = void_uttyname;
    if (retval != PAM_SUCCESS || uttyname == NULL) {
        pam_syslog (pamh, LOG_ERR, "cannot determine user's tty");
	return PAM_SERVICE_ERR;
    }

    retval = _pammodutil_tty_secure(pamh, uttyname);

    if (retval && !(ctrl & PAM_NOCONSOLE_ARG)) {
        FILE *cmdlinefile;

        /* Allow access from the kernel console, if enabled */
        cmdlinefile = fopen(CMDLINE_FILE, "r");

        if (cmdlinefile != NULL) {
            char line[LINE_MAX], *p;

            p = fgets(line, sizeof(line), cmdlinefile);
            fclose(cmdlinefile);

            for (; p; p = strstr(p+1, "console=")) {
                char *e;

                /* Test whether this is a beginning of a word? */
                if (p > line && p[-1] != ' ')
                    continue;

                /* Is this our console? */
                if (strncmp(p + 8, uttyname, strlen(uttyname)))
                    continue;

                /* Is there any garbage after the TTY name? */
                e = p + 8 + strlen(uttyname);
                if (*e == ',' || *e == ' ' || *e == '\n' || *e == 0) {
                    retval = 0;
                    break;
                }
            }
        }
    }
    if (retval && !(ctrl & PAM_NOCONSOLE_ARG)) {
        FILE *consoleactivefile;

        /* Allow access from the active console */
        consoleactivefile = fopen(CONSOLEACTIVE_FILE, "r");

        if (consoleactivefile != NULL) {
            char line[LINE_MAX], *p, *n;

            line[0] = 0;
            p = fgets(line, sizeof(line), consoleactivefile);
            fclose(consoleactivefile);

	    if (p) {
		/* remove the newline character at end */
		if (line[strlen(line)-1] == '\n')
		    line[strlen(line)-1] = 0;

		for (n = p; n != NULL; p = n+1) {
		    if ((n = strchr(p, ' ')) != NULL)
			*n = '\0';

		    if (strcmp(p, uttyname) == 0) {
			retval = 0;
			break;
		    }
		}
	    }
	}
    }

    if (retval) {
	    pam_syslog(pamh, LOG_NOTICE, "access denied: tty '%s' is not secure !",
		     uttyname);

	    retval = PAM_AUTH_ERR;
	    if (user_pwd == NULL) {
		retval = PAM_USER_UNKNOWN;
	    }
    } else {
	if (ctrl & PAM_DEBUG_ARG) {
	    pam_syslog(pamh, LOG_DEBUG, "access allowed for '%s' on '%s'",
		     username, uttyname);
	}
	retval = PAM_SUCCESS;

    }

    return retval;
}

/* --- authentication management functions --- */

int pam_sm_authenticate(pam_handle_t *pamh, int flags UNUSED, int argc,
			const char **argv)
{
    int ctrl;

    /* parse the arguments */
    ctrl = _pam_parse (pamh, argc, argv);

    return securetty_perform_check(pamh, ctrl, __FUNCTION__);
}

int
pam_sm_setcred (pam_handle_t *pamh UNUSED, int flags UNUSED,
		int argc UNUSED, const char **argv UNUSED)
{
    return PAM_SUCCESS;
}

/* --- account management functions --- */

int
pam_sm_acct_mgmt (pam_handle_t *pamh, int flags UNUSED,
		  int argc, const char **argv)
{
    int ctrl;

    /* parse the arguments */
    ctrl = _pam_parse (pamh, argc, argv);

    /* take the easy route */
    return securetty_perform_check(pamh, ctrl, __FUNCTION__);
}

/* end of module definition */
