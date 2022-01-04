#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define _XOPEN_SOURCE 500	/* for getsid(2) */
#define _BSD_SOURCE		/* for acct(2) */

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <mntent.h>
#include <netdb.h>
#include <paths.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <utmp.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

#include "watch_err.h"
#include "extern.h"
#include "ext2_mnt.h"

#if defined __GLIBC__
#include <sys/quota.h>
#include <sys/swap.h>
#include <sys/reboot.h>
#else				/* __GLIBC__ */
#include <linux/quota.h>
#endif				/* __GLIBC__ */

#include <unistd.h>

#ifndef NSIG
#define NSIG _NSIG
#endif

#ifndef __GLIBC__
#ifndef RB_AUTOBOOT
#define RB_AUTOBOOT	0xfee1dead,672274793,0x01234567 /* Perform a hard reset now.  */
#define RB_ENABLE_CAD	0xfee1dead,672274793,0x89abcdef /* Enable reboot using Ctrl-Alt-Delete keystroke.  */
#define RB_HALT_SYSTEM	0xfee1dead,672274793,0xcdef0123 /* Halt the system.  */
#define RB_POWER_OFF	0xfee1dead,672274793,0x4321fedc /* Stop system and switch power off if possible.  */
#endif /*RB_AUTOBOOT*/
#endif /* !__GLIBC__ */

extern void umount_all(void *);
extern int ifdown(void);
#if 0
extern int mount_one(char *, char *, char *, char *, int, int);
static struct mntent rootfs;
#endif

/* Info about a process. */
typedef struct _proc_ {
	pid_t pid;		/* Process ID.                    */
	int sid;		/* Session ID.                    */
	struct _proc_ *next;	/* Pointer to next struct.        */
} PROC;

/* close the device and check for error */
static void close_all(void)
{
	close_watchdog();
	close_loadcheck();
	close_memcheck();
	close_tempcheck();
	close_heartbeat();
	free_process();		/* What check_bin() was waiting to report. */
}

/* on exit we close the device and log that we stop */
void terminate(int ecode)
{
	log_message(LOG_NOTICE, "stopping daemon (%d.%d)", MAJOR_VERSION, MINOR_VERSION);
	unlock_our_memory();
	close_all();
	remove_pid_file();
	close_logging();
	usleep(100000);		/* 0.1s to make sure log is written */
	exit(ecode);
}

/* panic: we're still alive but shouldn't */
static void panic(void)
{
	/*
	 * Okay we should never reach this point,
	 * but if we do we will cause the hard reset
	 */
	open_logging(NULL, MSG_TO_STDERR | MSG_TO_SYSLOG);
	log_message(LOG_ALERT, "WATCHDOG PANIC: failed to reboot, trying hard-reset");
	sleep(dev_timeout * 4);

	/* if we are still alive, we just exit */
	log_message(LOG_ALERT, "WATCHDOG PANIC: still alive after sleeping %d seconds", 4 * dev_timeout);
	close_all();
	close_logging();
	exit(1);
}

static void mnt_off(void)
{
	FILE *fp;
	struct mntent *mnt;

	fp = setmntent(_PATH_MNTTAB, "r");
	/* in some rare cases fp might be NULL so be careful */
	while (fp != NULL && ((mnt = getmntent(fp)) != (struct mntent *)0)) {
		/* First check if swap */
		if (!strcmp(mnt->mnt_type, MNTTYPE_SWAP))
			if (swapoff(mnt->mnt_fsname) < 0)
				perror(mnt->mnt_fsname);

		/* quota only if mounted at boot time && filesytem=ext2 */
		if (hasmntopt(mnt, MNTOPT_NOAUTO) || strcmp(mnt->mnt_type, MNTTYPE_EXT2))
			continue;

		/* group quota? */
		if (hasmntopt(mnt, MNTOPT_GRPQUOTA))
			if (quotactl(QCMD(Q_QUOTAOFF, GRPQUOTA), mnt->mnt_fsname, 0, (caddr_t) 0) < 0)
				perror(mnt->mnt_fsname);

		/* user quota */
		if (hasmntopt(mnt, MNTOPT_USRQUOTA))
			if (quotactl(QCMD(Q_QUOTAOFF, USRQUOTA), mnt->mnt_fsname, 0, (caddr_t) 0) < 0)
				perror(mnt->mnt_fsname);

#if 0
		/* not needed anymore */
		/* while we're at it we add the remount option */
		if (strcmp(mnt->mnt_dir, "/") == 0) {
			/* save entry if root partition */
			rootfs.mnt_freq = mnt->mnt_freq;
			rootfs.mnt_passno = mnt->mnt_passno;

			rootfs.mnt_fsname = strdup(mnt->mnt_fsname);
			rootfs.mnt_dir = strdup(mnt->mnt_dir);
			rootfs.mnt_type = strdup(mnt->mnt_type);

			/* did we get enough memory? */
			if (rootfs.mnt_fsname == NULL || rootfs.mnt_dir == NULL || rootfs.mnt_type == NULL) {
				log_message(LOG_ERR, "out of memory");
			}

			if ((rootfs.mnt_opts = malloc(strlen(mnt->mnt_opts) + strlen("remount,ro") + 2)) == NULL) {
				log_message(LOG_ERR, "out of memory");
			} else
				sprintf(rootfs.mnt_opts, "%s,remount,ro", mnt->mnt_opts);
		}
#endif
	}
	endmntent(fp);
}

/* Parts of the following two functions are taken from Miquel van */
/* Smoorenburg's killall5 program. */

static PROC *plist;

/* get a list of all processes */
static int readproc(void)
{
	DIR *dir;
	struct dirent *d;
	pid_t act_pid;
	PROC *p;

	/* Open the /proc directory. */
	if ((dir = opendir("/proc")) == NULL) {
		log_message(LOG_ERR, "cannot opendir /proc");
		return (-1);
	}

	/* Don't worry about free'ing the list first, we are going down anyway. */
	plist = NULL;

	/* Walk through the directory. */
	while ((d = readdir(dir)) != NULL) {

		/* See if this is a process */
		if ((act_pid = atoi(d->d_name)) == 0)
			continue;

		/*
		 * Get a PROC struct. If this fails, which is likely if we have an
		 * out-of-memory error, we return gracefully with what we have managed
		 * so hopefully a 2nd call after killing some processes will give us more.
		 */
		if ((p = (PROC *) calloc(1, sizeof(PROC))) == NULL) {
			log_message(LOG_ERR, "out of memory");
			closedir(dir);
			return (-1);
		}
		p->sid = getsid(act_pid);
		p->pid = act_pid;

		/* Link it into the list. */
		p->next = plist;
		plist = p;
	}
	closedir(dir);

	/* Done. */
	return (0);
}

static void killall5(int sig)
{
	PROC *p;
	int sid = -1;

	/*
	 *    Ignoring SIGKILL and SIGSTOP do not make sense, but
	 *    someday kill(-1, sig) might kill ourself if we don't
	 *    do this. This certainly is a valid concern for SIGTERM-
	 *    Linux 2.1 might send the calling process the signal too.
	 */

	/* Since we ignore all signals, we don't have to worry here. MM */
	/* Now stop all processes. */
	suspend_logging();
	kill(-1, SIGSTOP);

	/* Find out our own 'sid'. */
	if (readproc() == 0) {
		for (p = plist; p; p = p->next)
			if (p->pid == daemon_pid) {
				sid = p->sid;
				break;
			}
		/* Now kill all processes except our session. */
		for (p = plist; p; p = p->next)
			if (p->pid != daemon_pid &&	/* Skip our process */
				p->sid != sid && 	/* Skip our session */
				p->sid != 0) 		/* Skip any kernel process. */
					kill(p->pid, sig);
	}
	/* And let them continue. */
	kill(-1, SIGCONT);
	resume_logging();
}

/* part that tries to shut down the system cleanly */
static void try_clean_shutdown(int errorcode)
{
	int i = 0, fd;
	char *seedbck = RANDOM_SEED;

	/* soft-boot the system */
	/* do not close open files here, they will be closed later anyway */
	/* close_all(); */

	/* if we will halt the system we should try to tell a sysadmin */
	if (admin != NULL) {
		/* send mail to the system admin */
		FILE *ph;
		char exe[128];
		struct stat buf;

		/* Only can send an email if sendmail binary exists so check
		 * that first, or else we will get a broken pipe in pclose.
		 * We cannot let the shell check, because a non-existant or
		 * non-executable sendmail binary means that the pipe is closed faster
		 * than we can write to it. */
		if ((stat(PATH_SENDMAIL, &buf) != 0) || ((buf.st_mode & S_IXUSR) == 0)) {
			log_message(LOG_ERR, "%s does not exist or is not executable (errno = %d)", PATH_SENDMAIL, errno);
		} else {
			sprintf(exe, "%s -i %s", PATH_SENDMAIL, admin);
			ph = popen(exe, "w");
			if (ph == NULL) {
				log_message(LOG_ERR, "cannot start %s (errno = %d)", PATH_SENDMAIL, errno);
			} else {
				char myname[MAXHOSTNAMELEN + 1];
				struct hostent *hp;

				/* get my name */
				gethostname(myname, sizeof(myname));

				fprintf(ph, "To: %s\n", admin);
				if (ferror(ph) != 0) {
					log_message(LOG_ERR, "cannot send mail (errno = %d)", errno);
				} else {
					/* if possible use the full name including domain */
					if ((hp = gethostbyname(myname)) != NULL)
						fprintf(ph, "Subject: %s is going down!\n\n", hp->h_name);
					else
						fprintf(ph, "Subject: %s is going down!\n\n", myname);
					if (ferror(ph) != 0) {
						log_message(LOG_ERR, "cannot send mail (errno = %d)", errno);
					} else {
						if (errorcode == ETOOHOT)
							fprintf(ph,
								"Message from watchdog:\nIt is too hot to keep on working. The system will be halted!\n");
						else
							fprintf(ph,
								"Message from watchdog:\nThe system will be rebooted because of error %d = '%s'\n", errorcode, wd_strerror(errorcode));
						if (ferror(ph) != 0) {
							log_message(LOG_ERR, "cannot send mail (errno = %d)", errno);
						}
					}
				}
				if (pclose(ph) == -1) {
					log_message(LOG_ERR, "cannot finish mail (errno = %d)", errno);
				}
				/* finally give the system a little bit of time to deliver */
			}
		}
	}

	close_logging();

	safe_sleep(10);		/* make sure log is written and mail is send */

	/* We cannot start shutdown, since init might not be able to fork. */
	/* That would stop the reboot process. So we try rebooting the system */
	/* ourselves. Note, that it is very likely we cannot start any rc */
	/* script either, so we do it all here. */

	/* Close all files except the watchdog device. */
	for (i = 0; i < 3; i++)
		if (!isatty(i))
			close(i);
	for (i = 3; i < 20; i++)
		if (i != get_watchdog_fd())
			close(i);
	close(255);

	/* Ignore all signals. */
	for (i = 1; i < NSIG; i++)
		signal(i, SIG_IGN);

	/* Stop init; it is insensitive to the signals sent by the kernel. */
	kill(1, SIGTSTP);

	/* Kill all other processes. */
	killall5(SIGTERM);
	safe_sleep(1);
	/* Do this twice in case we have out-of-memory problems. */
	killall5(SIGTERM);
	safe_sleep(sigterm_delay-1);
	killall5(SIGKILL);
	keep_alive();
	/* Out-of-memory safeguard again. */
	killall5(SIGKILL);
	keep_alive();

	/* Remove our PID file, as nothing should be capable of starting a 2nd daemon now. */
	remove_pid_file();

	/* Record the fact that we're going down */
	if ((fd = open(_PATH_WTMP, O_WRONLY | O_APPEND)) >= 0) {
		time_t t;
		struct utmp wtmp;
		memset(&wtmp, 0, sizeof(wtmp));

		time(&t);
		strcpy(wtmp.ut_user, "shutdown");
		strcpy(wtmp.ut_line, "~");
		strcpy(wtmp.ut_id, "~~");
		wtmp.ut_pid = 0;
		wtmp.ut_type = RUN_LVL;
		wtmp.ut_time = t;
		if (write(fd, (char *)&wtmp, sizeof(wtmp)) < 0)
			log_message(LOG_ERR, "failed writing wtmp (%s)", strerror(errno));
		close(fd);
	}

	/* save the random seed if a save location exists */
	/* don't worry about error messages, we react here anyway */
	if (strlen(seedbck) != 0) {
		int fd_seed;

		if ((fd_seed = open("/dev/urandom", O_RDONLY)) >= 0) {
			int fd_bck;

			if ((fd_bck = creat(seedbck, S_IRUSR | S_IWUSR)) >= 0) {
				char buf[512];

				if (read(fd_seed, buf, 512) == 512) {
					if (write(fd_bck, buf, 512) < 0)
						log_message(LOG_ERR, "failed writing urandom (%s)", strerror(errno));
				}
				close(fd_bck);
			}
			close(fd_seed);
		}
	}

	/* Turn off accounting */
	if (acct(NULL) < 0)
		log_message(LOG_ERR, "failed stopping acct() (%s)", strerror(errno));

	keep_alive();

	/* Turn off quota and swap */
	mnt_off();

	/* umount all partitions */
	umount_all(NULL);

#if 0
	/* with the more recent version of mount code, this is not needed anymore */
	/* remount / read-only */
	//if (setjmp(ret2dog) == 0)
		mount_one(rootfs.mnt_fsname, rootfs.mnt_dir, rootfs.mnt_type,
			  rootfs.mnt_opts, rootfs.mnt_freq, rootfs.mnt_passno);
#endif

	/* shut down interfaces (also taken from sysvinit source */
	ifdown();
}

/* shut down the system */
void do_shutdown(int errorcode)
{
	/* tell syslog what's happening */
	log_message(LOG_ALERT, "shutting down the system because of error %d = '%s'", errorcode, wd_strerror(errorcode));

	if(errorcode != ERESET)	{
		try_clean_shutdown(errorcode);
	} else {
		/* We have been asked to hard-reset, make basic attempt at clean filesystem
		 * but don't try stopping anything, etc, then used device (below) to do reset
		 * action.
		 */
		sync();
		sleep(1);
	}

	/* finally reboot */
	if (errorcode != ETOOHOT) {
		if (get_watchdog_fd() != -1) {
			/* We have a hardware timer, try using that for a quick reboot first. */
			set_watchdog_timeout(1);
			sleep(dev_timeout * 4);
		}
		/* That failed, or was not possible, ask kernel to do it for us. */
		reboot(RB_AUTOBOOT);
	} else {
		if (temp_poweroff) {
			/* Tell system to power off if possible. */
			reboot(RB_POWER_OFF);
		} else {
			/* Turn on hard reboot, CTRL-ALT-DEL will reboot now. */
			reboot(RB_ENABLE_CAD);
			/* And perform the `halt' system call. */
			reboot(RB_HALT_SYSTEM);
		}
	}

	/* unbelievable: we're still alive */
	panic();
}
