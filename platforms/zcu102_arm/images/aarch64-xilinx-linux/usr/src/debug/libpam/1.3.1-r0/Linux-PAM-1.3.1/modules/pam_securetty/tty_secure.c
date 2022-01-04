/*
 * A function to determine if a particular line is in /etc/securetty
 */


#define SECURETTY_FILE "/etc/securetty"
#define TTY_PREFIX     "/dev/"

/* This function taken out of pam_securetty by Sam Hartman
 * <hartmans@debian.org>*/
/*
 * by Elliot Lee <sopwith@redhat.com>, Red Hat Software.
 * July 25, 1996.
 * Slight modifications AGM. 1996/12/3
 */

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <security/pam_modules.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/syslog.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <security/pam_modutil.h>
#include <security/pam_ext.h>

extern int _pammodutil_tty_secure(const pam_handle_t *pamh,
                                  const char *uttyname);

int _pammodutil_tty_secure(const pam_handle_t *pamh, const char *uttyname)
{
    int retval = PAM_AUTH_ERR;
    char ttyfileline[256];
    char ptname[256];
    struct stat ttyfileinfo;
    FILE *ttyfile;
    /* The PAM_TTY item may be prefixed with "/dev/" - skip that */
    if (strncmp(TTY_PREFIX, uttyname, sizeof(TTY_PREFIX)-1) == 0)
	uttyname += sizeof(TTY_PREFIX)-1;

    if (stat(SECURETTY_FILE, &ttyfileinfo)) {
	pam_syslog(pamh, LOG_NOTICE, "Couldn't open %s: %m",
	           SECURETTY_FILE);
	return PAM_SUCCESS; /* for compatibility with old securetty handling,
			       this needs to succeed.  But we still log the
			       error. */
    }

    if ((ttyfileinfo.st_mode & S_IWOTH) || !S_ISREG(ttyfileinfo.st_mode)) {
	/* If the file is world writable or is not a
	   normal file, return error */
	pam_syslog(pamh, LOG_ERR,
	           "%s is either world writable or not a normal file",
	           SECURETTY_FILE);
	return PAM_AUTH_ERR;
    }

    ttyfile = fopen(SECURETTY_FILE,"r");
    if(ttyfile == NULL) { /* Check that we opened it successfully */
	pam_syslog(pamh, LOG_ERR, "Error opening %s: %m", SECURETTY_FILE);
	return PAM_SERVICE_ERR;
    }

    if (isdigit(uttyname[0])) {
	snprintf(ptname, sizeof(ptname), "pts/%s", uttyname);
    } else {
	ptname[0] = '\0';
    }

    retval = 1;

    while ((fgets(ttyfileline,sizeof(ttyfileline)-1, ttyfile) != NULL) 
	   && retval) {
	if(ttyfileline[strlen(ttyfileline) - 1] == '\n')
	    ttyfileline[strlen(ttyfileline) - 1] = '\0';
	retval = ( strcmp(ttyfileline,uttyname)
	           && (!ptname[0] || strcmp(ptname, uttyname)) );
    }
    fclose(ttyfile);

    if(retval) {
	retval = PAM_AUTH_ERR;
    }

    return retval;
}
