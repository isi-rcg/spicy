/*
 * Helper functions to handle lists of quota structures
 */

#include "config.h"

#if defined(RPC)
#include <rpc/rpc.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <paths.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <limits.h>
#include <stdlib.h>

#if defined(RPC)
#include "rquota.h"
#endif

#include "mntopt.h"
#include "quotaops.h"
#include "pot.h"
#include "bylabel.h"
#include "common.h"
#include "quotasys.h"
#include "quotaio.h"

/*
 * Set grace time if needed
 */
void update_grace_times(struct dquot *q)
{
	time_t now;

	time(&now);
	if (q->dq_dqb.dqb_bsoftlimit && toqb(q->dq_dqb.dqb_curspace) > q->dq_dqb.dqb_bsoftlimit) {
		if (!q->dq_dqb.dqb_btime)
			q->dq_dqb.dqb_btime = now + q->dq_h->qh_info.dqi_bgrace;
	}
	else
		q->dq_dqb.dqb_btime = 0;
	if (q->dq_dqb.dqb_isoftlimit && q->dq_dqb.dqb_curinodes > q->dq_dqb.dqb_isoftlimit) {
		if (!q->dq_dqb.dqb_itime)
			q->dq_dqb.dqb_itime = now + q->dq_h->qh_info.dqi_igrace;
	}
	else
		q->dq_dqb.dqb_itime = 0;
}

/*
 * Collect the requested quota information.
 */
struct dquot *getprivs(qid_t id, struct quota_handle **handles, int quiet)
{
	struct dquot *q, *qtail = NULL, *qhead = NULL;
	int i;
	char name[MAXNAMELEN];
#if defined(BSD_BEHAVIOUR)
	int j, ngroups;
	uid_t euid;
	gid_t gidset[NGROUPS_MAX], *gidsetp;
#endif

	for (i = 0; handles[i]; i++) {
#if defined(BSD_BEHAVIOUR)
		switch (handles[i]->qh_type) {
			case USRQUOTA:
				euid = geteuid();
				if (euid != id && euid != 0) {
					uid2user(id, name);
					errstr(_("%s (uid %d): Permission denied\n"), name, id);
					return (struct dquot *)NULL;
				}
				break;
			case GRPQUOTA:
				if (geteuid() == 0)
					break;
				ngroups = sysconf(_SC_NGROUPS_MAX);
				if (ngroups > NGROUPS_MAX) {
					gidsetp = malloc(ngroups * sizeof(gid_t));
					if (!gidsetp) {
						gid2group(id, name);
						errstr(_("%s (gid %d): gid set allocation (%d): %s\n"), name, id, ngroups, strerror(errno));
						return (struct dquot *)NULL;
					}
				}
				else
					gidsetp = &gidset[0];
				ngroups = getgroups(ngroups, gidsetp);
				if (ngroups < 0) {
					if (gidsetp != gidset)
						free(gidsetp);
					gid2group(id, name);
					errstr(_("%s (gid %d): error while trying getgroups(): %s\n"), name, id, strerror(errno));
					return (struct dquot *)NULL;
				}

				for (j = 0; j < ngroups; j++)
					if (id == gidsetp[j])
						break;
				if (gidsetp != gidset)
					free(gidsetp);
				if (j >= ngroups) {
					gid2group(id, name);
					errstr(_("%s (gid %d): Permission denied\n"),
						name, id);
					return (struct dquot *)NULL;
				}
				break;
			default:
				break;
		}
#endif

		if (!(q = handles[i]->qh_ops->read_dquot(handles[i], id))) {
			/* If rpc.rquotad is not running filesystem might be just without quotas... */
			if (errno != ENOENT && (errno != ECONNREFUSED || !quiet)) {
				int olderrno = errno;

				id2name(id, handles[i]->qh_type, name);
				errstr(_("error while getting quota from %s for %s (id %u): %s\n"),
					handles[i]->qh_quotadev, name, id, strerror(olderrno));
			}
			continue;
		}
		if (qhead == NULL)
			qhead = q;
		else
			qtail->dq_next = q;
		qtail = q;
		q->dq_next = NULL;	/* This should be already set, but just for sure... */
	}
	return qhead;
}

/*
 * Store the requested quota information.
 */
int putprivs(struct dquot *qlist, int flags)
{
	struct dquot *q;
	int ret = 0;

	for (q = qlist; q; q = q->dq_next) {
		if (q->dq_h->qh_ops->commit_dquot(q, flags) == -1) {
			errstr(_("Cannot write quota for %u on %s: %s\n"),
				q->dq_id, q->dq_h->qh_quotadev, strerror(errno));
			ret = -1;
			continue;
		}
	}
	return ret;
}

/*
 * Take a list of priviledges and get it edited.
 */
#define MAX_ED_PARS 128
int editprivs(char *tmpfile)
{
	sigset_t omask, nmask;
	pid_t pid;
	int stat;

	sigemptyset(&nmask);
	sigaddset(&nmask, SIGINT);
	sigaddset(&nmask, SIGQUIT);
	sigaddset(&nmask, SIGHUP);
	sigprocmask(SIG_SETMASK, &nmask, &omask);
	if ((pid = fork()) < 0) {
		errstr("Cannot fork(): %s\n", strerror(errno));
		return -1;
	}
	if (pid == 0) {
		char *ed, *actp, *nextp;
		char *edpars[MAX_ED_PARS];
		int i;

		sigprocmask(SIG_SETMASK, &omask, NULL);
		if (setgid(getgid()))
			die(1, _("%s failed: %s\n"), "setgid", strerror(errno));
		if (setuid(getuid()))
			die(1, _("%s failed: %s\n"), "setuid", strerror(errno));
		if (!(ed = getenv("VISUAL")))
			if (!(ed = getenv("EDITOR")))
				ed = _PATH_VI;
		i = 0;
		ed = actp = sstrdup(ed);
		while (actp) {
			nextp = strchr(actp, ' ');
			if (nextp) {
				*nextp = 0;
				nextp++;
			}
			edpars[i++] = actp;
			if (i == MAX_ED_PARS-2) {
				errstr(_("Too many parameters to editor.\n"));
				break;
			}
			actp = nextp;
		}
		edpars[i++] = tmpfile;
		edpars[i] = NULL;
		execvp(edpars[0], edpars);
		die(1, _("Cannot exec %s\n"), ed);
	}
	waitpid(pid, &stat, 0);
	sigprocmask(SIG_SETMASK, &omask, NULL);

	return 0;
}

/*
 * Duplicate a file descriptor, resetting the file offset.
 * If it's a write descriptor, also truncate the file.
 */
static FILE *dup_file(int fd, int is_write)
{
	FILE *fp;

	if (is_write && ftruncate(fd, 0))
		die(1, _("Cannot truncate a file: %s\n"), strerror(errno));
	if (lseek(fd, 0, SEEK_SET))
		die(1, _("Cannot reset a file offset: %s\n"), strerror(errno));
	if ((fd = dup(fd)) < 0)
		die(1, _("Cannot duplicate a file descriptor: %s\n"), strerror(errno));
	if (!(fp = fdopen(fd, is_write ? "w" : "r")))
		die(1, is_write ? _("Cannot open a stream to write to: %s\n")
				: _("Cannot open a stream to read from: %s\n"),
		    strerror(errno));

	return fp;
}

/*
 * Convert a dquot list to an ASCII file.
 */
int writeprivs(struct dquot *qlist, int outfd, char *name, int quotatype)
{
	struct dquot *q;
	FILE *fd;

	fd = dup_file(outfd, 1);

	fprintf(fd, _("Disk quotas for %s %s (%cid %d):\n"),
		_(type2name(quotatype)), name, *type2name(quotatype), qlist->dq_id);

	fprintf(fd,
		_("  Filesystem                   blocks       soft       hard     inodes     soft     hard\n"));

	for (q = qlist; q; q = q->dq_next) {
		fprintf(fd, "  %-24s %10llu %10llu %10llu %10llu %8llu %8llu\n",
			q->dq_h->qh_quotadev,
			(long long)toqb(q->dq_dqb.dqb_curspace),
			(long long)q->dq_dqb.dqb_bsoftlimit,
			(long long)q->dq_dqb.dqb_bhardlimit,
			(long long)q->dq_dqb.dqb_curinodes,
			(long long)q->dq_dqb.dqb_isoftlimit, (long long)q->dq_dqb.dqb_ihardlimit);
	}
	fclose(fd);
	return 0;
}

/* Merge changes on one dev to proper structure in the list */
static void merge_limits_to_list(struct dquot *qlist, char *dev, u_int64_t blocks, u_int64_t bsoft,
			  u_int64_t bhard, u_int64_t inodes, u_int64_t isoft, u_int64_t ihard)
{
	struct dquot *q;

	for (q = qlist; q; q = q->dq_next) {
		if (!devcmp_handle(dev, q->dq_h))
			continue;

		q->dq_dqb.dqb_bsoftlimit = bsoft;
		q->dq_dqb.dqb_bhardlimit = bhard;
		q->dq_dqb.dqb_isoftlimit = isoft;
		q->dq_dqb.dqb_ihardlimit = ihard;
		q->dq_flags |= DQ_FOUND;
		update_grace_times(q);

		if (blocks != toqb(q->dq_dqb.dqb_curspace))
			errstr(_("WARNING - %s: cannot change current block allocation\n"),
				q->dq_h->qh_quotadev);
		if (inodes != q->dq_dqb.dqb_curinodes)
			errstr(_("WARNING - %s: cannot change current inode allocation\n"),
				q->dq_h->qh_quotadev);
	}
}

/*
 * Merge changes to an ASCII file into a dquot list.
 */
int readprivs(struct dquot *qlist, int infd)
{
	FILE *fd;
	int cnt;
	qsize_t blocks, bsoft, bhard, inodes, isoft, ihard;
	struct dquot *q;
	char fsp[BUFSIZ], line[BUFSIZ];
	char blocksstring[BUFSIZ], bsoftstring[BUFSIZ], bhardstring[BUFSIZ];
	char inodesstring[BUFSIZ], isoftstring[BUFSIZ], ihardstring[BUFSIZ];
	const char *error;

	fd = dup_file(infd, 0);

	/*
	 * Discard title lines, then read lines to process.
	 */
	if (!fgets(line, sizeof(line), fd) ||
	    !fgets(line, sizeof(line), fd)) {
		errstr(_("Bad format: two title lines assumed\n"));
		fclose(fd);
		return -1;
	}

	while (fgets(line, sizeof(line), fd)) {
		cnt = sscanf(line, "%s %s %s %s %s %s %s",
			     fsp, blocksstring, bsoftstring, bhardstring,
			     inodesstring, isoftstring, ihardstring);

		if (cnt != 7) {
			errstr(_("Bad format:\n%s\n"), line);
			fclose(fd);
			return -1;
		}
		error = str2space(blocksstring, &blocks);
		if (error) {
			errstr(_("Bad block usage: %s: %s\n"),
				blocksstring, error);
			fclose(fd);
			return -1;
		}
		error = str2space(bsoftstring, &bsoft);
		if (error) {
			errstr(_("Bad block soft limit: %s: %s\n"),
				bsoftstring, error);
			fclose(fd);
			return -1;
		}
		error = str2space(bhardstring, &bhard);
		if (error) {
			errstr(_("Bad block hard limit: %s: %s\n"),
				bhardstring, error);
			fclose(fd);
			return -1;
		}
		error = str2number(inodesstring, &inodes);
		if (error) {
			errstr(_("Bad inode usage: %s: %s\n"),
				inodesstring, error);
			fclose(fd);
			return -1;
		}
		error = str2number(isoftstring, &isoft);
		if (error) {
			errstr(_("Bad inode soft limit: %s: %s\n"),
				isoftstring, error);
			fclose(fd);
			return -1;
		}
		error = str2number(ihardstring, &ihard);
		if (error) {
			errstr(_("Bad inode hard limit: %s: %s\n"),
				ihardstring, error);
			fclose(fd);
			return -1;
		}

		merge_limits_to_list(qlist, fsp, blocks, bsoft, bhard, inodes, isoft, ihard);
	}
	fclose(fd);

	/*
	 * Disable quotas for any filesystems that have not been found.
	 */
	for (q = qlist; q; q = q->dq_next) {
		if (q->dq_flags & DQ_FOUND) {
			q->dq_flags &= ~DQ_FOUND;
			continue;
		}
		q->dq_dqb.dqb_bsoftlimit = 0;
		q->dq_dqb.dqb_bhardlimit = 0;
		q->dq_dqb.dqb_isoftlimit = 0;
		q->dq_dqb.dqb_ihardlimit = 0;
	}
	return 0;
}

/* Merge changes on one dev to proper structure in the list */
static void merge_times_to_list(struct dquot *qlist, char *dev, time_t btime, time_t itime)
{
	struct dquot *q;

	for (q = qlist; q; q = q->dq_next) {
		if (!devcmp_handle(dev, q->dq_h))
			continue;

		q->dq_dqb.dqb_btime = btime;
		q->dq_dqb.dqb_itime = itime;
		q->dq_flags |= DQ_FOUND;
	}
}

/*
 * Write grace times of user to file
 */
int writeindividualtimes(struct dquot *qlist, int outfd, char *name, int quotatype)
{
	struct dquot *q;
	FILE *fd;
	time_t now;
	char btimestr[MAXTIMELEN], itimestr[MAXTIMELEN];

	fd = dup_file(outfd, 1);

	fprintf(fd, _("Times to enforce softlimit for %s %s (%cid %d):\n"),
		_(type2name(quotatype)), name, *type2name(quotatype), qlist->dq_id);
	fprintf(fd, _("Time units may be: days, hours, minutes, or seconds\n"));
	fprintf(fd,
		_("  Filesystem                         block grace               inode grace\n"));

	time(&now);
	for (q = qlist; q; q = q->dq_next) {
		if (!q->dq_dqb.dqb_btime)
			strcpy(btimestr, _("unset"));
		else if (q->dq_dqb.dqb_btime <= now)
			strcpy(btimestr, _("0seconds"));
		else
			sprintf(btimestr, _("%useconds"), (unsigned)(q->dq_dqb.dqb_btime - now));
		if (!q->dq_dqb.dqb_itime)
			strcpy(itimestr, _("unset"));
		else if (q->dq_dqb.dqb_itime <= now)
			strcpy(itimestr, _("0seconds"));
		else
			sprintf(itimestr, _("%useconds"), (unsigned)(q->dq_dqb.dqb_itime - now));

		fprintf(fd, "  %-24s %22s %22s\n", q->dq_h->qh_quotadev, btimestr, itimestr);
	}
	fclose(fd);
	return 0;
}

/*
 *  Read list of grace times for a user and convert it
 */
int readindividualtimes(struct dquot *qlist, int infd)
{
	FILE *fd;
	int cnt, btime, itime;
	char line[BUFSIZ], fsp[BUFSIZ], btimestr[BUFSIZ], itimestr[BUFSIZ];
	char iunits[BUFSIZ], bunits[BUFSIZ];
	time_t now, bseconds, iseconds;

	fd = dup_file(infd, 0);

	/*
	 * Discard title lines, then read lines to process.
	 */
	if (!fgets(line, sizeof(line), fd) ||
	    !fgets(line, sizeof(line), fd) ||
	    !fgets(line, sizeof(line), fd)) {
		errstr(_("Bad format: three title lines assumed\n"));
		fclose(fd);
		return -1;
	}

	time(&now);
	while (fgets(line, sizeof(line), fd)) {
		cnt = sscanf(line, "%s %s %s", fsp, btimestr, itimestr);
		if (cnt != 3) {
format_err:
			errstr(_("bad format:\n%s\n"), line);
			fclose(fd);
			return -1;
		}
		if (!strcmp(btimestr, _("unset")))
			bseconds = 0;
		else {
			if (sscanf(btimestr, "%d%s", &btime, bunits) != 2)
				goto format_err;
			if (str2timeunits(btime, bunits, &bseconds) < 0) {
units_err:
				errstr(_("Bad time units. Units are 'second', 'minute', 'hour', and 'day'.\n"));
				fclose(fd);
				return -1;
			}
			bseconds += now;
		}
		if (!strcmp(itimestr, _("unset")))
			iseconds = 0;
		else {
			if (sscanf(itimestr, "%d%s", &itime, iunits) != 2)
				goto format_err;
			if (str2timeunits(itime, iunits, &iseconds) < 0)
				goto units_err;
			iseconds += now;
		}
		merge_times_to_list(qlist, fsp, bseconds, iseconds);
	}
	fclose(fd);

	return 0;
}

/*
 * Convert a dquot list to an ASCII file of grace times.
 */
int writetimes(struct quota_handle **handles, int outfd)
{
	FILE *fd;
	char itimebuf[MAXTIMELEN], btimebuf[MAXTIMELEN];
	int i;

	if (!handles[0])
		return 0;

	fd = dup_file(outfd, 1);

	fprintf(fd, _("Grace period before enforcing soft limits for %ss:\n"),
		_(type2name(handles[0]->qh_type)));
	fprintf(fd, _("Time units may be: days, hours, minutes, or seconds\n"));
	fprintf(fd, _("  Filesystem             Block grace period     Inode grace period\n"));

	for (i = 0; handles[i]; i++) {
		time2str(handles[i]->qh_info.dqi_bgrace, btimebuf, 0);
		time2str(handles[i]->qh_info.dqi_igrace, itimebuf, 0);
		fprintf(fd, "  %-12s %22s %22s\n", handles[i]->qh_quotadev, btimebuf, itimebuf);
	}

	fclose(fd);
	return 0;
}

/*
 * Merge changes of grace times in an ASCII file into a dquot list.
 */
int readtimes(struct quota_handle **handles, int infd)
{
	FILE *fd;
	int itime, btime, i, cnt;
	time_t iseconds, bseconds;
	char fsp[BUFSIZ], bunits[10], iunits[10], line[BUFSIZ];

	if (!handles[0])
		return 0;

	fd = dup_file(infd, 0);

	/* Set all grace times to default values */
	for (i = 0; handles[i]; i++) {
		handles[i]->qh_info.dqi_bgrace = MAX_DQ_TIME;
		handles[i]->qh_info.dqi_igrace = MAX_IQ_TIME;
		mark_quotafile_info_dirty(handles[i]);
	}
	/*
	 * Discard three title lines, then read lines to process.
	 */
	if (!fgets(line, sizeof(line), fd) ||
	    !fgets(line, sizeof(line), fd) ||
	    !fgets(line, sizeof(line), fd)) {
		errstr(_("Bad format: three title lines assumed\n"));
		fclose(fd);
		return -1;
	}

	while (fgets(line, sizeof(line), fd)) {
		cnt = sscanf(line, "%s %d %s %d %s", fsp, &btime, bunits, &itime, iunits);
		if (cnt != 5) {
			errstr(_("bad format:\n%s\n"), line);
			fclose(fd);
			return -1;
		}
		if (str2timeunits(btime, bunits, &bseconds) < 0 ||
		    str2timeunits(itime, iunits, &iseconds) < 0) {
			errstr(_("Bad time units. Units are 'second', 'minute', 'hour', and 'day'.\n"));
			fclose(fd);
			return -1;
		}
		for (i = 0; handles[i]; i++) {
			if (!devcmp_handle(fsp, handles[i]))
				continue;
			handles[i]->qh_info.dqi_bgrace = bseconds;
			handles[i]->qh_info.dqi_igrace = iseconds;
			mark_quotafile_info_dirty(handles[i]);
			break;
		}
	}
	fclose(fd);

	return 0;
}

/*
 * Free a list of dquot structures.
 */
void freeprivs(struct dquot *qlist)
{
	struct dquot *q, *nextq;

	for (q = qlist; q; q = nextq) {
		nextq = q->dq_next;
		free(q);
	}
}
