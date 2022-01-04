/*
 * QUOTA    An implementation of the diskquota system for the LINUX operating
 *          system. QUOTA is implemented using the BSD systemcall interface
 *          as the means of communication with the user level. Should work for
 *          all filesystems because of integration into the VFS layer of the
 *          operating system. This is based on the Melbourne quota system wich
 *          uses both user and group quota files.
 * 
 *          Program to mail to users that they are over there quota.
 * 
 * Author:  Marco van Wieringen <mvw@planets.elm.net>
 *
 *          This program is free software; you can redistribute it and/or
 *          modify it under the terms of the GNU General Public License as
 *          published by the Free Software Foundation; either version 2 of
 *          the License, or (at your option) any later version.
 */

#include "config.h"

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <signal.h>
#include <grp.h>
#include <time.h>
#include <getopt.h>
#include <locale.h>
#ifdef HAVE_NL_LANGINFO
#include <langinfo.h>
#endif
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#ifdef USE_LDAP_MAIL_LOOKUP
#include <ldap.h>
#endif

#include "mntopt.h"
#include "pot.h"
#include "bylabel.h"
#include "common.h"
#include "quotasys.h"
#include "quotaio.h"

/* these are just defaults, overridden in the WARNQUOTA_CONF file */
#define MAIL_CMD "/usr/lib/sendmail -t"
#define FROM     "support@localhost"
#define SUBJECT  "Disk Quota usage on system"
#define CC_TO    "root"
#define SUPPORT  "support@localhost"
#define PHONE    "(xxx) xxx-xxxx or (yyy) yyy-yyyy"

#define DEF_USER_MESSAGE	_("Hi,\n\nWe noticed that you are in violation with the quotasystem\n" \
                          "used on this system. We have found the following violations:\n\n")
#define DEF_USER_SIGNATURE	_("\nWe hope that you will cleanup before your grace period expires.\n" \
	                  "\nBasically, this means that the system thinks you are using more disk space\n" \
	                  "on the above partition(s) than you are allowed.  If you do not delete files\n" \
	                  "and get below your quota before the grace period expires, the system will\n" \
	                  "prevent you from creating new files.\n\n" \
                          "For additional assistance, please contact us at %s\nor via " \
                          "phone at %s.\n")
#define DEF_GROUP_MESSAGE	_("Hi,\n\nWe noticed that the group %s you are member of violates the quotasystem\n" \
                          "used on this system. We have found the following violations:\n\n")
#define DEF_GROUP_SIGNATURE	_("\nPlease cleanup the group data before the grace period expires.\n" \
	                  "\nBasically, this means that the system thinks group is using more disk space\n" \
	                  "on the above partition(s) than it is allowed.  If you do not delete files\n" \
	                  "and get below group quota before the grace period expires, the system will\n" \
	                  "prevent you and other members of the group from creating new files owned by\n" \
			  "the group.\n\n" \
                          "For additional assistance, please contact us at %s\nor via " \
                          "phone at %s.\n")

#define SHELL "/bin/sh"
#define QUOTATAB "/etc/quotatab"
#define CNF_BUFFER 2048
#define IOBUF_SIZE 16384		/* Size of buffer for line in config files */
#define ADMIN_TAB_ALLOC 256		/* How many entries to admins table should we allocate at once? */
#define WARNQUOTA_CONF "/etc/warnquota.conf"
#define ADMINSFILE "/etc/quotagrpadmins"

#define FL_USER 1
#define FL_GROUP 2
#define FL_NOAUTOFS 4
#define FL_NODETAILS 16

struct usage {
	char *devicename;
	struct util_dqblk dq_dqb;
	struct usage *next;
};

#ifdef USE_LDAP_MAIL_LOOKUP
static LDAP *ldapconn = NULL;
#endif

struct configparams {
	char mail_cmd[CNF_BUFFER];
	char from[CNF_BUFFER];
	char subject[CNF_BUFFER];
	char cc_to[CNF_BUFFER];
	char support[CNF_BUFFER];
	char phone[CNF_BUFFER];
	char charset[CNF_BUFFER];
	char *user_message;
	char *user_signature;
	char *group_message;
	char *group_signature;
	time_t cc_before;
#ifdef USE_LDAP_MAIL_LOOKUP
	int use_ldap_mail; /* 0 */
	int ldap_is_setup; /* 0 */
	int ldap_starttls; /* 0 */
	int ldap_tls; /* LDAP_OPT_X_TLS_NEVER */
	int ldap_vers; /* LDAP_VERSION3 */
	char ldap_host[CNF_BUFFER];
	int ldap_port;
	char ldap_uri[CNF_BUFFER];
	char ldap_binddn[CNF_BUFFER];
	char ldap_bindpw[CNF_BUFFER];
	char ldap_basedn[CNF_BUFFER];
	char ldap_search_attr[CNF_BUFFER];
	char ldap_mail_attr[CNF_BUFFER];
	char default_domain[CNF_BUFFER];
#endif /* USE_LDAP_MAIL_LOOKUP */
};

struct offenderlist {
	int offender_type;
	int offender_id;
	char *offender_name;
	struct usage *usage;
	struct offenderlist *next;
};

typedef struct quotatable {
	char *devname;
	char *devdesc;
} quotatable_t;

struct adminstable {
	char *grpname;
	char *adminname;
};

static int qtab_i = 0, fmt = -1, flags;
static char maildev[CNF_BUFFER];
static struct quota_handle *maildev_handle;
static char *configfile = WARNQUOTA_CONF, *quotatabfile = QUOTATAB, *adminsfile = ADMINSFILE;
char *progname;
static char *hostname, *domainname;
static quotatable_t *quotatable;
static int adminscnt, adminsalloc;
static struct adminstable *adminstable;
static enum s2s_unit spaceunit = S2S_NONE, inodeunit = S2S_NONE;

/*
 * Global pointers to list.
 */
static struct offenderlist *offenders = (struct offenderlist *)0;

/*
 * add any cleanup functions here
 */
static void wc_exit(int ex_stat)
{
#ifdef USE_LDAP_MAIL_LOOKUP
	if(ldapconn != NULL)
		ldap_unbind_ext(ldapconn, NULL, NULL);
#endif
	exit(ex_stat);
}

#ifdef USE_LDAP_MAIL_LOOKUP
static int setup_ldap(struct configparams *config)
{
	int ret;
	struct berval cred = { .bv_val = config->ldap_bindpw,
			       .bv_len = strlen(config->ldap_bindpw) };

	ret = ldap_initialize(&ldapconn, config->ldap_uri);

	if (ret != LDAP_SUCCESS) {
		errstr(_("ldap_initialize() failed: %s\n"), ldap_err2string(ret));
		return -1;
	}

	if (config->ldap_starttls) {
		ldap_set_option(ldapconn, LDAP_OPT_PROTOCOL_VERSION, &(config->ldap_vers));
		ldap_set_option(ldapconn, LDAP_OPT_X_TLS_REQUIRE_CERT, &(config->ldap_tls));
		ret = ldap_start_tls_s(ldapconn, NULL, NULL);
		if (ret != LDAP_SUCCESS) {
			errstr(_("ldap_start_tls_s() failed: %s\n"), ldap_err2string(ret));
		    return -1;
		}
	}
	ret = ldap_sasl_bind_s(ldapconn, config->ldap_binddn, LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
	if(ret < 0) {
		errstr(_("ldap_sasl_bind_s() failed: %s\n"), ldap_err2string(ret));
		return -1;
	}
	return 0;
}
		
#endif

static struct offenderlist *add_offender(int type, int id, char *name)
{
	struct offenderlist *offender;
	char namebuf[MAXNAMELEN];
	
	if (!name) {
		if (id2name(id, type, namebuf)) {
			errstr(_("Cannot get name for uid/gid %u.\n"), id);
			return NULL;
		}
		name = namebuf;
	}
	offender = (struct offenderlist *)smalloc(sizeof(struct offenderlist));
	offender->offender_type = type;
	offender->offender_id = id;
	offender->offender_name = sstrdup(name);
	offender->usage = (struct usage *)NULL;
	offender->next = offenders;
	offenders = offender;
	return offender;
}

static void add_offence(struct dquot *dquot, char *name)
{
	struct offenderlist *lptr;
	struct usage *usage;

	for (lptr = offenders; lptr; lptr = lptr->next)
		if (dquot->dq_h->qh_type == lptr->offender_type && lptr->offender_id == dquot->dq_id)
			break;

	if (!lptr)
		if (!(lptr = add_offender(dquot->dq_h->qh_type, dquot->dq_id, name)))
			return;

	usage = (struct usage *)smalloc(sizeof(struct usage));
	memcpy(&usage->dq_dqb, &dquot->dq_dqb, sizeof(struct util_dqblk));

	usage->devicename = sstrdup(dquot->dq_h->qh_quotadev);
	/*
	 * Stuff it in front
	 */
	usage->next = lptr->usage;
	lptr->usage = usage;
}

static int deliverable(struct dquot *dquot)
{
	time_t now;
	struct dquot *mdquot;
	
	if (!maildev[0])
		return 1;

	time(&now);
	
	if (!strcasecmp(maildev, "any") && 
	   ((dquot->dq_dqb.dqb_bhardlimit && toqb(dquot->dq_dqb.dqb_curspace) >= dquot->dq_dqb.dqb_bhardlimit)
	   || ((dquot->dq_dqb.dqb_bsoftlimit && toqb(dquot->dq_dqb.dqb_curspace) >= dquot->dq_dqb.dqb_bsoftlimit)
	   && (dquot->dq_dqb.dqb_btime && dquot->dq_dqb.dqb_btime <= now))))
		return 0;
	if (!maildev_handle)
		return 1;
	mdquot = maildev_handle->qh_ops->read_dquot(maildev_handle, dquot->dq_id);
	if (mdquot &&
	   ((mdquot->dq_dqb.dqb_bhardlimit && toqb(mdquot->dq_dqb.dqb_curspace) >= mdquot->dq_dqb.dqb_bhardlimit)
	   || ((mdquot->dq_dqb.dqb_bsoftlimit && toqb(mdquot->dq_dqb.dqb_curspace) >= mdquot->dq_dqb.dqb_bsoftlimit)
	   && (mdquot->dq_dqb.dqb_btime && mdquot->dq_dqb.dqb_btime <= now)))) {
		free(mdquot);
		return 0;
	}
	free(mdquot);
	return 1;
}

static int check_offence(struct dquot *dquot, char *name)
{
	if ((dquot->dq_dqb.dqb_bsoftlimit && toqb(dquot->dq_dqb.dqb_curspace) >= dquot->dq_dqb.dqb_bsoftlimit)
	    || (dquot->dq_dqb.dqb_isoftlimit && dquot->dq_dqb.dqb_curinodes >= dquot->dq_dqb.dqb_isoftlimit)) {
		if(deliverable(dquot))
			add_offence(dquot, name);
	}
	return 0;
}

static FILE *run_mailer(char *command)
{
	int pipefd[2];
	FILE *f;

	if (pipe(pipefd) < 0) {
		errstr(_("Cannot create pipe: %s\n"), strerror(errno));
		return NULL;
	}
	signal(SIGPIPE, SIG_IGN);
	switch(fork()) {
		case -1:
			errstr(_("Cannot fork: %s\n"), strerror(errno));
			return NULL;
		case 0:
			close(pipefd[1]);
			if (dup2(pipefd[0], 0) < 0) {
				errstr(_("Cannot duplicate descriptor: %s\n"), strerror(errno));
				wc_exit(1);
			}			
			execl(SHELL, SHELL, "-c", command, NULL);
			errstr(_("Cannot execute '%s': %s\n"), command, strerror(errno));
			wc_exit(1);
		default:
			close(pipefd[0]);
			if (!(f = fdopen(pipefd[1], "w")))
				errstr(_("Cannot open pipe: %s\n"), strerror(errno));
			return f;
	}
}

static int admin_name_cmp(const void *key, const void *mem)
{
	return strcmp(key, ((struct adminstable *)mem)->grpname);
}

static int should_cc(struct offenderlist *offender, struct configparams *config)
{
	struct usage *lptr;
	struct util_dqblk *dqb;
	time_t atime;

	/* Noone to send CC to? */
	if (!strcmp(config->cc_to, ""))
		return 0;

	if (config->cc_before == -1)
		return 1;
	time(&atime);
	for (lptr = offender->usage; lptr; lptr = lptr->next) {
		dqb = &lptr->dq_dqb;
		if (dqb->dqb_bsoftlimit && dqb->dqb_bsoftlimit <= toqb(dqb->dqb_curspace) && dqb->dqb_btime-config->cc_before <= atime)
			return 1;
		if (dqb->dqb_isoftlimit && dqb->dqb_isoftlimit <= dqb->dqb_curinodes && dqb->dqb_itime-config->cc_before <= atime)
			return 1;
	}
	return 0;
}

/* Substitute %s and %i for 'name' and %h for hostname */
static void format_print(FILE *fp, char *fmt, char *name)
{
	char *ch, *lastch = fmt;

	for (ch = strchr(fmt, '%'); ch; lastch = ch+2, ch = strchr(ch+2, '%')) {
		*ch = 0;
		fputs(lastch, fp);
		*ch = '%';
		switch (*(ch+1)) {
			case 's':
			case 'i':
				fputs(name, fp);
				break;
			case 'h':
				fputs(hostname, fp);
				break;
			case 'd':
				fputs(domainname, fp);
				break;
			case '%':
				fputc('%', fp);
				break;
		}
	}
	fputs(lastch, fp);
}

static char *lookup_user(struct configparams *config, char *user)
{
#ifdef USE_LDAP_MAIL_LOOKUP
       	char searchbuf[256];
	LDAPMessage *result, *entry;
	BerElement     *ber = NULL;
	struct berval  **bvals = NULL;
	int ret, cnt;
	char *a;
	char *to = NULL;

	if (!config->use_ldap_mail)
		return sstrdup(user);

	if (ldapconn == NULL && config->ldap_is_setup == 0) {
		/* need init */
		if (setup_ldap(config)) {
			errstr(_("Could not setup ldap connection.\n"));
			return NULL;
		}
		config->ldap_is_setup = 1;
	}

	if (ldapconn == NULL) {
		/*
		 * ldap was never setup correctly so just use
		 * the offender_name
		 */
		return sstrdup(user);
	}

	/* search for the offender_name in ldap */
	if (256 <= snprintf(searchbuf, 256, "(%s=%s)", config->ldap_search_attr,
		    user)) {
		errstr(_("Could not format LDAP search filter for %s user and "
			"%s search attribute due to excessive length.\n"),
			user, config->ldap_search_attr);
		return NULL;
	}
	ret = ldap_search_ext_s(ldapconn,
		config->ldap_basedn, LDAP_SCOPE_SUBTREE,
		searchbuf, NULL, 0, NULL, NULL, NULL,
		0, &result);

	if (ret < 0) {
		errstr(_("Error with %s.\n"), user);
		errstr(_("ldap_search_ext_s() failed: %s\n"), ldap_err2string(ret));
		return NULL;
	}
		
	cnt = ldap_count_entries(ldapconn, result);
	if (cnt > 1) {
		errstr(_("Multiple entries found for client %s (%d).\n"),
		       user, cnt);
		return NULL;
	} else if (cnt == 0) {
		errstr(_("Entry not found for client %s.\n"), user);
		return NULL;
	}
	/* get the attr */
	entry = ldap_first_entry(ldapconn, result);
	for (a = ldap_first_attribute(ldapconn, entry, &ber); a != NULL;
	     a = ldap_next_attribute(ldapconn, entry, ber)) {
		if (strcasecmp(a, config->ldap_mail_attr) == 0) {
			bvals = ldap_get_values_len(ldapconn, entry, a);
			if (bvals == NULL) {
				errstr(_("Could not get values for %s.\n"), 
				       user);
				return NULL;
			}
			to = sstrdup(bvals[0]->bv_val);
			ldap_memfree(a);
			ldap_value_free_len(bvals);
			break;
		}
		ldap_memfree(a);
	} 

	ber_free(ber, 0);

	if (to == NULL) {
		/* 
		 * use just the name and default domain as we didn't find the
		 * attribute we wanted in this entry
		 */
		to = smalloc(strlen(user) + strlen(config->default_domain) + 1);
		sprintf(to, "%s@%s", user, config->default_domain);
	}
	return to;
#else
	return sstrdup(user);
#endif
}

static int mail_user(struct offenderlist *offender, struct configparams *config)
{
	struct usage *lptr;
	FILE *fp;
	int cnt, status;
	char timebuf[MAXTIMELEN];
	char numbuf[3][MAXNUMLEN];
	struct util_dqblk *dqb;
	char *to = NULL;

	if (offender->offender_type == USRQUOTA) {
		to = lookup_user(config, offender->offender_name);
		if (!to)
			return -1;
	} else {
		struct adminstable *admin;

		if (!(admin = bsearch(offender->offender_name, adminstable, adminscnt, sizeof(struct adminstable), admin_name_cmp))) {
			errstr(_("Administrator for a group %s not found. Cancelling mail.\n"), offender->offender_name);
			return -1;
		}
		to = sstrdup(admin->adminname);
	}
	if (!(fp = run_mailer(config->mail_cmd))) {
		if (to)
			free(to);
		return -1;
	}
	fprintf(fp, "From: %s\n", config->from);
	fprintf(fp, "Reply-To: %s\n", config->support);
	fprintf(fp, "Subject: %s\n", config->subject);
	fprintf(fp, "To: %s\n", to);
	if (should_cc(offender, config)) {
		char *cc_to = lookup_user(config, config->cc_to);

		if (cc_to) {
			fprintf(fp, "Cc: %s\n", cc_to);
			free(cc_to);
		}
	}
	if ((config->charset)[0] != '\0') { /* are we supposed to set the encoding */
		fprintf(fp, "MIME-Version: 1.0\n");
		fprintf(fp, "Content-Type: text/plain; charset=%s\n", config->charset);
		fprintf(fp, "Content-Disposition: inline\n");
		fprintf(fp, "Content-Transfer-Encoding: 8bit\n");
	}
	fprintf(fp, "\n");
	free(to);

	if (offender->offender_type == USRQUOTA)
		if (config->user_message)
			format_print(fp, config->user_message, offender->offender_name);
		else
			fputs(DEF_USER_MESSAGE, fp);
	else
		if (config->group_message)
			format_print(fp, config->group_message, offender->offender_name);
		else
			fprintf(fp, DEF_GROUP_MESSAGE, offender->offender_name);

	if (!(flags & FL_NODETAILS)) {
		for (lptr = offender->usage; lptr; lptr = lptr->next) {
			dqb = &lptr->dq_dqb;
			for (cnt = 0; cnt < qtab_i; cnt++)
				if (!strcmp(quotatable[cnt].devname, lptr->devicename)) {
					fprintf(fp, "\n%s (%s)\n", quotatable[cnt].devdesc, quotatable[cnt].devname);
					break;
				}
			if (cnt == qtab_i)	/* Description not found? */
				fprintf(fp, "\n%s\n", lptr->devicename);
			fprintf(fp, _("\n                        Block limits               File limits\n"));
			fprintf(fp, _("Filesystem           used    soft    hard  grace    used  soft  hard  grace\n"));
			if (strlen(lptr->devicename) > 15)
				fprintf(fp, "%s\n%15s", lptr->devicename, "");
			else
				fprintf(fp, "%-15s", lptr->devicename);
			if (dqb->dqb_bsoftlimit && dqb->dqb_bsoftlimit <= toqb(dqb->dqb_curspace))
				difftime2str(dqb->dqb_btime, timebuf);
			else
				timebuf[0] = '\0';
			space2str(toqb(dqb->dqb_curspace), numbuf[0], spaceunit);
			space2str(dqb->dqb_bsoftlimit, numbuf[1], spaceunit);
			space2str(dqb->dqb_bhardlimit, numbuf[2], spaceunit);
			fprintf(fp, "%c%c %7s %7s %7s %6s",
			        dqb->dqb_bsoftlimit && toqb(dqb->dqb_curspace) >= dqb->dqb_bsoftlimit ? '+' : '-',
				dqb->dqb_isoftlimit && dqb->dqb_curinodes >= dqb->dqb_isoftlimit ? '+' : '-',
				numbuf[0], numbuf[1], numbuf[2], timebuf);
			if (dqb->dqb_isoftlimit && dqb->dqb_isoftlimit <= dqb->dqb_curinodes)
				difftime2str(dqb->dqb_itime, timebuf);
			else
				timebuf[0] = '\0';
			number2str(dqb->dqb_curinodes, numbuf[0], inodeunit);
			number2str(dqb->dqb_isoftlimit, numbuf[1], inodeunit);
			number2str(dqb->dqb_ihardlimit, numbuf[2], inodeunit);
			fprintf(fp, " %7s %5s %5s %6s\n\n", numbuf[0], numbuf[1], numbuf[2], timebuf);
		}
	}


	if (offender->offender_type == USRQUOTA)
		if (config->user_signature)
			format_print(fp, config->user_signature, offender->offender_name);
		else
			fprintf(fp, DEF_USER_SIGNATURE, config->support, config->phone);
	else
		if (config->group_signature)
			format_print(fp, config->group_signature, offender->offender_name);
		else
			fprintf(fp, DEF_GROUP_SIGNATURE, config->support, config->phone);
	fclose(fp);
	if (wait(&status) < 0)	/* Wait for mailer */
		errstr(_("Cannot wait for mailer: %s\n"), strerror(errno));
	else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
		errstr(_("Warning: Mailer exitted abnormally.\n"));

	return 0;
}

static int mail_to_offenders(struct configparams *config)
{
	struct offenderlist *lptr;
	int ret = 0;

	/*
	 * Dump offenderlist.
	 */
	for (lptr = offenders; lptr; lptr = lptr->next)
		ret |= mail_user(lptr, config);
	return ret;
}

/*
 * Wipe spaces, tabs, quotes and newlines from beginning and end of string
 */
static void stripstring(char **buff)
{
	int i;

	/* first put a \0 at the tight place to end the string */
	for (i = strlen(*buff) - 1; i >= 0 && (isspace((*buff)[i]) || (*buff)[i] == '"'
	     || (*buff)[i] == '\''); i--);
	(*buff)[i+1] = 0;

	/* then determine the position to start */
	for (i = 0; (*buff)[i] && (isspace((*buff)[i]) || (*buff)[i] == '"' || (*buff)[i] == '\''); i++);
	*buff += i;
}

/*
 * Substitute '|' with end of lines
 */
static void create_eoln(char *buf)
{
	char *colpos = buf;

	while ((colpos = strchr(colpos, '|')))
		*colpos = '\n';
}

/*
 * Read /etc/quotatab (description of devices for users)
 */
static int get_quotatable(void)
{
	FILE *fp;
	char buffer[IOBUF_SIZE], *colpos, *devname, *devdesc;
	int line;
	struct stat st;

	if (!(fp = fopen(quotatabfile, "r"))) {
		errstr(_("Cannot open %s: %s\nWill use device names.\n"), quotatabfile, strerror(errno));
		qtab_i = 0;
		return 0;
	}

	line = 0;
	for (qtab_i = 0; quotatable = srealloc(quotatable, sizeof(quotatable_t) * (qtab_i + 1)),
	     fgets(buffer, sizeof(buffer), fp); qtab_i++) {
		line++;
		quotatable[qtab_i].devname = NULL;
		quotatable[qtab_i].devdesc = NULL;
		if (buffer[0] == '#' || buffer[0] == ';') {	/* Comment? */
			qtab_i--;
			continue;
		}
		/* Empty line? */
		for (colpos = buffer; isspace(*colpos); colpos++);
		if (!*colpos) {
			qtab_i--;
			continue;
		}
		/* Parse line */
		if (!(colpos = strchr(buffer, ':'))) {
			errstr(_("Cannot parse line %d in quotatab (missing ':')\n"), line);
			qtab_i--;
			continue;
		}
		*colpos = 0;
		devname = buffer;
		devdesc = colpos+1;
		stripstring(&devname);
		stripstring(&devdesc);
		quotatable[qtab_i].devname = sstrdup(devname);
		quotatable[qtab_i].devdesc = sstrdup(devdesc);
		create_eoln(quotatable[qtab_i].devdesc);

		if (stat(quotatable[qtab_i].devname, &st) < 0)
			errstr(_("Cannot stat device %s (maybe typo in quotatab)\n"), quotatable[qtab_i].devname);
	}
	fclose(fp);
	return 0;
}

/* Check correctness of the given format */
static void verify_format(char *fmt, char *varname)
{
	char *ch;

	for (ch = strchr(fmt, '%'); ch; ch = strchr(ch+2, '%')) {
		switch (*(ch+1)) {
			case 's':
			case 'i':
			case 'h':
			case 'd':
			case '%':
				continue;
			default:
				die(1, _("Incorrect format string for variable %s.\n\
Unrecognized expression %%%c.\n"), varname, *(ch+1));
		}
	}
}

/*
 * Reads config parameters from configfile
 * uses default values if errstr occurs
 */
static int readconfigfile(const char *filename, struct configparams *config)
{
	FILE *fp;
	char buff[IOBUF_SIZE];
	char *var;
	char *value;
	char *pos;
	int line, len, bufpos;
	char *locale;

	/* set default values */
	sstrncpy(config->mail_cmd, MAIL_CMD, CNF_BUFFER);
	sstrncpy(config->from, FROM, CNF_BUFFER);
	sstrncpy(config->subject, SUBJECT, CNF_BUFFER);
	sstrncpy(config->cc_to, CC_TO, CNF_BUFFER);
	sstrncpy(config->support, SUPPORT, CNF_BUFFER);
	sstrncpy(config->phone, PHONE, CNF_BUFFER);
	(config->charset)[0] = '\0';
	setlocale(LC_ALL, NULL);
	locale = setlocale(LC_MESSAGES, NULL);
#ifdef HAVE_NL_LANGINFO
	if (locale && strcasecmp(locale, "posix") && strcasecmp(locale, "c")) {
		locale = nl_langinfo(CODESET);
		sstrncpy(config->charset, locale, CNF_BUFFER);
	}
#endif
	maildev[0] = 0;
	config->user_signature = config->user_message = config->group_signature = config->group_message = NULL;
	config->cc_before = -1;

#ifdef USE_LDAP_MAIL_LOOKUP
	config->use_ldap_mail = 0;
	config->ldap_starttls = 0;
	config->ldap_tls = LDAP_OPT_X_TLS_NEVER;
	config->ldap_vers = LDAP_VERSION3;
	config->ldap_port = config->ldap_is_setup = 0;
	config->ldap_host[0] = 0;
	config->ldap_uri[0] = 0;
#endif

	if (!(fp = fopen(filename, "r"))) {
		errstr(_("Cannot open %s: %s\n"), filename, strerror(errno));
		return -1;
	}

	line = 0;
	bufpos = 0;
	while (fgets(buff + bufpos, sizeof(buff) - bufpos, fp)) {	/* start reading lines */
		line++;

		if (!bufpos) {
			/* check for comments or empty lines */
			if (buff[0] == '#' || buff[0] == ';')
				continue;
			/* Is line empty? */
			for (pos = buff; isspace(*pos); pos++);
			if (!*pos)			/* Nothing else was on the line */
				continue;
		}
		len = bufpos + strlen(buff+bufpos);
		if (buff[len-1] != '\n')
			errstr(_("Line %d too long. Truncating.\n"), line);
		else {
			len--;
			if (buff[len-1] == '\\') {	/* Should join with next line? */
				bufpos = len-1;
				continue;
			}
		}
		buff[len] = 0;
		bufpos = 0;
		
		/* check for a '=' char */
		if ((pos = strchr(buff, '='))) {
			*pos = 0;	/* split buff in two parts: var and value */
			var = buff;
			value = pos + 1;

			stripstring(&var);
			stripstring(&value);

			/* check if var matches anything */
			if (!strcmp(var, "MAIL_CMD"))
				sstrncpy(config->mail_cmd, value, CNF_BUFFER);
			else if (!strcmp(var, "FROM"))
				sstrncpy(config->from, value, CNF_BUFFER);
			else if (!strcmp(var, "SUBJECT"))
				sstrncpy(config->subject, value, CNF_BUFFER);
			else if (!strcmp(var, "CC_TO"))
				sstrncpy(config->cc_to, value, CNF_BUFFER);
			else if (!strcmp(var, "SUPPORT"))
				sstrncpy(config->support, value, CNF_BUFFER);
			else if (!strcmp(var, "PHONE"))
				sstrncpy(config->phone, value, CNF_BUFFER);
			else if (!strcmp(var, "CHARSET"))
				sstrncpy(config->charset, value, CNF_BUFFER);
			else if (!strcmp(var, "MAILDEV"))
				/* set the global */
				sstrncpy(maildev, value, CNF_BUFFER);
			else if (!strcmp(var, "MESSAGE")) {
				config->user_message = sstrdup(value);
				create_eoln(config->user_message);
				verify_format(config->user_message, "MESSAGE");
			}
			else if (!strcmp(var, "SIGNATURE")) {
				config->user_signature = sstrdup(value);
				create_eoln(config->user_signature);
				verify_format(config->user_signature, "SIGNATURE");
			}
			else if (!strcmp(var, "GROUP_MESSAGE")) {
				config->group_message = sstrdup(value);
				create_eoln(config->group_message);
				verify_format(config->group_message, "GROUP_MESSAGE");
			}
			else if (!strcmp(var, "GROUP_SIGNATURE")) {
				config->group_signature = sstrdup(value);
				create_eoln(config->group_signature);
				verify_format(config->group_signature, "GROUP_SIGNATURE");
			}
			else if (!strcmp(var, "CC_BEFORE")) {
				int num;
				char unit[10];

				if (sscanf(value, "%d%s", &num, unit) != 2)
					goto cc_parse_err;
				if (str2timeunits(num, unit, &config->cc_before) < 0) {
cc_parse_err:
					die(1, _("Cannot parse time at CC_BEFORE variable (line %d).\n"), line);
				}
			}
#ifdef USE_LDAP_MAIL_LOOKUP
			else if (!strcmp(var, "LDAP_MAIL")) {
				if(strcasecmp(value, "true") == 0) 
					config->use_ldap_mail = 1;
				else
					config->use_ldap_mail = 0;
			}
			else if (!strcmp(var, "LDAP_TLS")) {
				if (strcasecmp(value, "never") == 0) {
					config->ldap_starttls = 1;
					config->ldap_tls = LDAP_OPT_X_TLS_NEVER;
				}
				else if (strcasecmp(value, "demand") == 0) {
					config->ldap_starttls = 1;
					config->ldap_tls = LDAP_OPT_X_TLS_DEMAND;
				}
				else if (strcasecmp(value, "allow") == 0) {
					config->ldap_starttls = 1;
					config->ldap_tls = LDAP_OPT_X_TLS_ALLOW;
				}
				else if (strcasecmp(value, "try") == 0) {
					config->ldap_starttls = 1;
					config->ldap_tls = LDAP_OPT_X_TLS_TRY;
				}
				else
					config->ldap_starttls = 0;
			}
			else if (!strcmp(var, "LDAP_HOST"))
				sstrncpy(config->ldap_host, value, CNF_BUFFER);
			else if (!strcmp(var, "LDAP_PORT"))
				config->ldap_port = (int)strtol(value, NULL, 10);
			else if (!strcmp(var, "LDAP_URI"))
				sstrncpy(config->ldap_uri, value, CNF_BUFFER);
			else if(!strcmp(var, "LDAP_BINDDN"))
				sstrncpy(config->ldap_binddn, value, CNF_BUFFER);
			else if(!strcmp(var, "LDAP_BINDPW"))
				sstrncpy(config->ldap_bindpw, value, CNF_BUFFER);
			else if(!strcmp(var, "LDAP_BASEDN"))
				sstrncpy(config->ldap_basedn, value, CNF_BUFFER);
			else if(!strcmp(var, "LDAP_SEARCH_ATTRIBUTE"))
				sstrncpy(config->ldap_search_attr, value, CNF_BUFFER);
			else if(!strcmp(var, "LDAP_MAIL_ATTRIBUTE"))
				sstrncpy(config->ldap_mail_attr, value, CNF_BUFFER);
			else if(!strcmp(var, "LDAP_DEFAULT_MAIL_DOMAIN"))
				sstrncpy(config->default_domain, value, CNF_BUFFER);
#endif
			else	/* not matched at all */
				errstr(_("Error in config file (line %d), ignoring\n"), line);
		}
		else		/* no '=' char in this line */
			errstr(_("Possible error in config file (line %d), ignoring\n"), line);
	}
	if (bufpos)
		errstr(_("Unterminated last line, ignoring\n"));
#ifdef USE_LDAP_MAIL_LOOKUP
	if (config->use_ldap_mail)
	{
		if (!config->ldap_uri[0]) {
			if (CNF_BUFFER <= snprintf(config->ldap_uri, CNF_BUFFER,
				    "ldap://%s:%d", config->ldap_host,
				    config->ldap_port)) {
				errstr(_("Could not format LDAP URI because "
					    "it's longer than %d bytes.\n"),
					    CNF_BUFFER);
				return -1;
			}
			errstr(_("LDAP library version >= 2.3 detected. Please use LDAP_URI instead of hostname and port.\nGenerated URI %s\n"), config->ldap_uri);
		}
	}
#endif
	fclose(fp);

	return 0;
}

static int admin_cmp(const void *a1, const void *a2)
{
	return strcmp(((struct adminstable *)a1)->grpname, ((struct adminstable *)a2)->grpname);
}

/* Get administrators of the groups */
static int get_groupadmins(void)
{
	FILE *f;
	int line = 0;
	char buffer[IOBUF_SIZE], *colpos, *grouppos, *endname, *adminpos;

	if (!(f = fopen(adminsfile, "r"))) {
		errstr(_("Cannot open file with group administrators: %s\n"), strerror(errno));
		return -1;
	}
	
	while (fgets(buffer, IOBUF_SIZE, f)) {
		line++;
		if (buffer[0] == ';' || buffer[0] == '#')
			continue;
		/* Skip initial spaces */
		for (colpos = buffer; isspace(*colpos); colpos++);
		if (!*colpos)	/* Empty line? */
			continue;
		/* Find splitting colon */
		for (grouppos = colpos; *colpos && *colpos != ':'; colpos++);
		if (!*colpos || grouppos == colpos) {
			errstr(_("Parse error at line %d. Cannot find end of group name.\n"), line);
			continue;
		}
		/* Cut trailing spaces */
		for (endname = colpos-1; isspace(*endname); endname--);
		*(++endname) = 0;
		/* Skip initial spaces at admins name */
		for (colpos++; isspace(*colpos); colpos++);
		if (!*colpos) {
			errstr(_("Parse error at line %d. Cannot find administrators name.\n"), line);
			continue;
		}
		/* Go through admins name */
		for (adminpos = colpos; !isspace(*colpos); colpos++);
		if (*colpos) {	/* Some characters after name? */
			*colpos = 0;
			/* Skip trailing spaces */
			for (colpos++; isspace(*colpos); colpos++);
			if (*colpos) {
				errstr(_("Parse error at line %d. Trailing characters after administrators name.\n"), line);
				continue;
			}
		}
		if (adminscnt >= adminsalloc)
			adminstable = srealloc(adminstable, sizeof(struct adminstable)*(adminsalloc+=ADMIN_TAB_ALLOC));
		adminstable[adminscnt].grpname = sstrdup(grouppos);
		adminstable[adminscnt++].adminname = sstrdup(adminpos);
	}

	fclose(f);
	qsort(adminstable, adminscnt, sizeof(struct adminstable), admin_cmp);
	return 0;
}

static struct quota_handle *find_handle_dev(char *dev, struct quota_handle **handles)
{
	int i;

	for (i = 0; handles[i] && strcmp(dev, handles[i]->qh_quotadev); i++);
	return handles[i];
}

static void warn_quota(int fs_count, char **fs)
{
	struct quota_handle **handles;
	struct configparams config;
	int i;

	if (readconfigfile(configfile, &config) < 0)
		wc_exit(1);
	if (get_quotatable() < 0)
		wc_exit(1);

	if (flags & FL_USER) {
		handles = create_handle_list(fs_count, fs, USRQUOTA, -1, IOI_READONLY | IOI_INITSCAN, MS_LOCALONLY | (flags & FL_NOAUTOFS ? MS_NO_AUTOFS : 0));
		if (!maildev[0] || !strcasecmp(maildev, "any"))
			maildev_handle = NULL;
		else
			maildev_handle = find_handle_dev(maildev, handles);
		for (i = 0; handles[i]; i++)
			handles[i]->qh_ops->scan_dquots(handles[i], check_offence);
		dispose_handle_list(handles);
	}
	if (flags & FL_GROUP) {
		if (get_groupadmins() < 0)
			wc_exit(1);
		handles = create_handle_list(fs_count, fs, GRPQUOTA, -1, IOI_READONLY | IOI_INITSCAN, MS_LOCALONLY | (flags & FL_NOAUTOFS ? MS_NO_AUTOFS : 0));
		if (!maildev[0] || !strcasecmp(maildev, "any"))
			maildev_handle = NULL;
		else
			maildev_handle = find_handle_dev(maildev, handles);
		for (i = 0; handles[i]; i++)
			handles[i]->qh_ops->scan_dquots(handles[i], check_offence);
		dispose_handle_list(handles);
	}
	if (mail_to_offenders(&config) < 0)
		wc_exit(1);
}

/* Print usage information */
static void usage(void)
{
	errstr(_("Usage:\n  warnquota [-ugsid] [-F quotaformat] [-c configfile] [-q quotatabfile] [-a adminsfile] [filesystem...]\n\n\
-u, --user                      warn users\n\
-g, --group                     warn groups\n\
-s, --human-readable[=units]    display numbers in human friendly units (MB,\n\
                                GB, ...). Units can be also specified\n\
                                explicitely by an optional argument in format\n\
                                [kgt],[kgt] where the first character specifies\n\
                                space units and the second character specifies\n\
                                inode units\n\
-i, --no-autofs                 avoid autofs mountpoints\n\
-d, --no-details                do not send quota information itself\n\
-F, --format=formatname         use quotafiles of specific format\n\
-c, --config=config-file        non-default config file\n\
-q, --quota-tab=quotatab-file   non-default quotatab\n\
-a, --admins-file=admins-file   non-default admins file\n\
-h, --help                      display this help message and exit\n\
-v, --version                   display version information and exit\n\n"));
	errstr(_("Bugs to %s\n"), PACKAGE_BUGREPORT);
	wc_exit(1);
}
 
static void parse_options(int argcnt, char **argstr)
{
	int ret;
	struct option long_opts[] = {
		{ "user", 0, NULL, 'u' },
		{ "group", 0, NULL, 'g' },
		{ "version", 0, NULL, 'V' },
		{ "help", 0, NULL, 'h' },
		{ "format", 1, NULL, 'F' },
		{ "config", 1, NULL, 'c' },
		{ "quota-tab", 1, NULL, 'q' },
		{ "admins-file", 1, NULL, 'a' },
		{ "no-autofs", 0, NULL, 'i' },
		{ "human-readable", 2, NULL, 's' },
		{ "no-details", 0, NULL, 'd' },
		{ NULL, 0, NULL, 0 }
	};
 
	while ((ret = getopt_long(argcnt, argstr, "ugVF:hc:q:a:is::d", long_opts, NULL)) != -1) {
		switch (ret) {
		  case '?':
		  case 'h':
			usage();
		  case 'V':
			version();
			exit(0);
		  case 'F':
			if ((fmt = name2fmt(optarg)) == QF_ERROR)
				wc_exit(1);
			break;
		  case 'c':
			configfile = optarg;
			break;
		  case 'q':
			quotatabfile = optarg;
			break;
		  case 'a':
			adminsfile = optarg;
			break;
		  case 'u':
			flags |= FL_USER;
			break;
		  case 'g':
			flags |= FL_GROUP;
			break;
		  case 'i':
			flags |= FL_NOAUTOFS;
			break;
		  case 's':
			inodeunit = spaceunit = S2S_AUTO;
			if (optarg) {
				if (unitopt2unit(optarg, &spaceunit, &inodeunit) < 0)
					die(1, _("Bad output format units for human readable output: %s\n"), optarg);
			}
			break;
		  case 'd':
			flags |= FL_NODETAILS;
			break;
		}
	}
	if (!(flags & FL_USER) && !(flags & FL_GROUP))
		flags |= FL_USER;
}
 
static void get_host_name(void)
{
	struct utsname uts;

	if (uname(&uts))
		die(1, _("Cannot get host name: %s\n"), strerror(errno));
	hostname = sstrdup(uts.nodename);
	domainname = sstrdup(uts.domainname);
}

int main(int argc, char **argv)
{
	gettexton();
	progname = basename(argv[0]);
	get_host_name();

	parse_options(argc, argv);
	init_kernel_interface();
	warn_quota(argc - optind, argc > optind ? argv + optind : NULL);

	wc_exit(0);
	return 0;
}
