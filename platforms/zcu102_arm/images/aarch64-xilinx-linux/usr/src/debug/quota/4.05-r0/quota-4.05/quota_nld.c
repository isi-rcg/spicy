/*
 *  A deamon to read quota warning messages from the kernel netlink socket
 *  and either pipe them to the system DBUS or write them to user's console
 *
 *  Copyright (c) 2007 SUSE CR, All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 */

#include "config.h"

#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <utmp.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <signal.h>
#include <sys/stat.h>
#include <asm/types.h>

#include <linux/netlink.h>
#include <netlink/socket.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>

#include <dbus/dbus.h>

#include "pot.h"
#include "common.h"
#include "quotasys.h"
#include "quota.h"

char *progname;

struct quota_warning {
	uint32_t qtype;
	uint64_t excess_id;
	uint32_t warntype;
	uint32_t dev_major;
	uint32_t dev_minor;
	uint64_t caused_id;
};

static struct nla_policy quota_nl_warn_cmd_policy[QUOTA_NL_A_MAX+1] = {
	[QUOTA_NL_A_QTYPE] = { .type = NLA_U32 },
	[QUOTA_NL_A_EXCESS_ID] = { .type = NLA_U64 },
	[QUOTA_NL_A_WARNING] = { .type = NLA_U32 },
	[QUOTA_NL_A_DEV_MAJOR] = { .type = NLA_U32 },
	[QUOTA_NL_A_DEV_MINOR] = { .type = NLA_U32 },
	[QUOTA_NL_A_CAUSED_ID] = { .type = NLA_U64 },
};

/* User options */
#define FL_NODBUS 1
#define FL_NOCONSOLE 2
#define FL_NODAEMON 4
#define FL_PRINTBELOW 8

static int flags;
static DBusConnection *dhandle;

static const struct option options[] = {
	{ "version", 0, NULL, 'V' },
	{ "help", 0, NULL, 'h' },
	{ "no-dbus", 0, NULL, 'D' },
	{ "no-console", 0, NULL, 'C' },
	{ "foreground", 0, NULL, 'F' },
	{ "print-below", 0, NULL, 'b' },
	{ NULL, 0, NULL, 0 }
};

static void show_help(void)
{
	errstr(_("Usage: %s [options]\nOptions are:\n\
 -h --help         shows this text\n\
 -V --version      shows version information\n\
 -C --no-console   do not try to write messages to console\n\
 -b --print-below  write to console also information about getting below hard/soft limits\n\
 -D --no-dbus      do not try to write messages to DBUS\n\
 -F --foreground   run daemon in foreground\n"), progname);
}

static void parse_options(int argc, char **argv)
{
	int opt;

	while ((opt = getopt_long(argc, argv, "VhDCFb", options, NULL)) >= 0) {
		switch (opt) {
			case 'V':
				version();
				exit(0);
			case 'h':
				show_help();
				exit(0);
			case 'D':
				flags |= FL_NODBUS;
				break;
			case 'C':
				flags |= FL_NOCONSOLE;
				break;
			case 'F':
				flags |= FL_NODAEMON;
				break;
			case 'b':
				flags |= FL_PRINTBELOW;
				break;
			default:
				errstr(_("Unknown option '%c'.\n"), opt);
				show_help();
				exit(1);
		}
	}
	if (flags & FL_NODBUS && flags & FL_NOCONSOLE) {
		errstr(_("No possible destination for messages. Nothing to do.\n"));
		exit(0);
	}
}

static void write_console_warning(struct quota_warning *warn);
static void write_dbus_warning(struct DBusConnection *dhandle, struct quota_warning *warn);

/* Parse netlink message and process it. */
static int quota_nl_parser(struct nl_msg *msg, void *arg)
{
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct genlmsghdr *ghdr;
	struct nlattr *attrs[QUOTA_NL_A_MAX+1];
	struct quota_warning warn;
	int ret;

	if (!genlmsg_valid_hdr(nlh, 0))
                return 0;
        ghdr = nlmsg_data(nlh);
	/* Unknown message? Ignore... */
	if (ghdr->cmd != QUOTA_NL_C_WARNING)
		return 0;

	ret = genlmsg_parse(nlh, 0, attrs, QUOTA_NL_A_MAX, quota_nl_warn_cmd_policy);
	if (ret < 0) {
		errstr(_("Error parsing netlink message.\n"));
		return ret;
	}
	if (!attrs[QUOTA_NL_A_QTYPE] || !attrs[QUOTA_NL_A_EXCESS_ID] ||
	    !attrs[QUOTA_NL_A_WARNING] || !attrs[QUOTA_NL_A_DEV_MAJOR] ||
	    !attrs[QUOTA_NL_A_DEV_MAJOR] || !attrs[QUOTA_NL_A_DEV_MINOR] ||
	    !attrs[QUOTA_NL_A_CAUSED_ID]) {
		errstr(_("Unknown format of kernel netlink message!\nMaybe your quota tools are too old?\n"));
		return -EINVAL;
	}
	warn.qtype = nla_get_u32(attrs[QUOTA_NL_A_QTYPE]);
	warn.excess_id = nla_get_u64(attrs[QUOTA_NL_A_EXCESS_ID]);
	warn.warntype = nla_get_u32(attrs[QUOTA_NL_A_WARNING]);
	warn.dev_major = nla_get_u32(attrs[QUOTA_NL_A_DEV_MAJOR]);
	warn.dev_minor = nla_get_u32(attrs[QUOTA_NL_A_DEV_MINOR]);
	warn.caused_id = nla_get_u64(attrs[QUOTA_NL_A_CAUSED_ID]);

	if (!(flags & FL_NOCONSOLE) && warn.qtype != PRJQUOTA)
		write_console_warning(&warn);
	if (!(flags & FL_NODBUS))
		write_dbus_warning(dhandle, &warn);
	return 0;
}

static struct nl_sock *init_netlink(void)
{
	struct nl_sock *sock;
	int ret, mc_family;

	sock = nl_socket_alloc();
	if (!sock)
		die(2, _("Cannot allocate netlink socket!\n"));
	nl_socket_disable_seq_check(sock);

	ret = nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM,
			quota_nl_parser, NULL);
	if (ret < 0)
		die(2, _("Cannot register callback for"
			 " netlink messages: %s\n"), strerror(-ret));

	ret = genl_connect(sock);
	if (ret < 0)
		die(2, _("Cannot connect to netlink socket: %s\n"), strerror(-ret));

	mc_family = genl_ctrl_resolve_grp(sock, "VFS_DQUOT", "events");
	if (mc_family < 0) {
		/*
		 * Using family id for multicasting is wrong but I messed up
		 * kernel netlink interface by using family id as a multicast
		 * group id in kernel so we have to carry this code to keep
		 * compatibility with older kernels.
		 */
		mc_family = genl_ctrl_resolve(sock, "VFS_DQUOT");
		if (mc_family < 0)
			die(2, _("Cannot resolve quota netlink name: %s\n"),
			    strerror(-mc_family));
	}

	ret = nl_socket_add_membership(sock, mc_family);
	if (ret < 0)
		die(2, _("Cannot join quota multicast group: %s\n"), strerror(-ret));

	return sock;
}

static DBusConnection *init_dbus(void)
{
	DBusConnection *handle;
	DBusError err;

	dbus_error_init(&err);
	handle = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
	if (dbus_error_is_set(&err))
		die(2, _("Cannot connect to system DBUS: %s\n"), err.message);

	dbus_connection_set_exit_on_disconnect(handle, FALSE);
	return handle;
}

static int write_all(int fd, char *buf, int len)
{
	int ret;

	while (len) {
		ret = write(fd, buf, len);
		if (ret < 0)
			return -1;
		buf += ret;
		len -= ret;
	}
	return 0;
}

#define WARN_BUF_SIZE 512

/* Scan through utmp, find latest used controlling tty and write to it */
static void write_console_warning(struct quota_warning *warn)
{
	struct utmp *uent;
	char user[MAXNAMELEN];
	struct stat st;
	char dev[PATH_MAX];
	time_t max_atime = 0;
	char max_dev[PATH_MAX];
	int fd;
	char warnbuf[WARN_BUF_SIZE];
	char *level, *msg;

	if ((warn->warntype == QUOTA_NL_IHARDBELOW ||
	    warn->warntype == QUOTA_NL_ISOFTBELOW ||
	    warn->warntype == QUOTA_NL_BHARDBELOW ||
	    warn->warntype == QUOTA_NL_BSOFTBELOW) && !(flags & FL_PRINTBELOW))
		return;
	uid2user(warn->caused_id, user);
	if (strlen(user) > UT_NAMESIZE)
		goto skip_utmp;
	strcpy(dev, "/dev/");

	setutent();
	endutent();
	while ((uent = getutent())) {
		if (uent->ut_type != USER_PROCESS)
			continue;
		/* Entry for a different user? */
		if (strncmp(user, uent->ut_user, UT_NAMESIZE))
			continue;
		sstrncpy(dev+5, uent->ut_line, PATH_MAX-5);
		if (stat(dev, &st) < 0)
			continue;	/* Failed to stat - not a good candidate for warning... */
		if (max_atime < st.st_atime) {
			max_atime = st.st_atime;
			strcpy(max_dev, dev);
		}
	}
	if (!max_atime) {
skip_utmp:
		/*
		 * This can happen quite easily so don't spam syslog with
		 * the error
		 */
		if (flags & FL_NODAEMON)
			errstr(_("Failed to find tty of user %llu to report warning to.\n"), (unsigned long long)warn->caused_id);
		return;
	}
	fd = open(max_dev, O_WRONLY);
	if (fd < 0) {
		errstr(_("Failed to open tty %s of user %llu to report warning.\n"), dev, (unsigned long long)warn->caused_id);
		return;
	}
	id2name(warn->excess_id, warn->qtype, user);
	if (warn->warntype == QUOTA_NL_ISOFTWARN ||
	    warn->warntype == QUOTA_NL_BSOFTWARN)
		level = _("Warning");
	else if (warn->warntype == QUOTA_NL_IHARDWARN ||
		 warn->warntype == QUOTA_NL_BHARDWARN)
		level = _("Error");
	else
		level = _("Info");
	switch (warn->warntype) {
		case QUOTA_NL_IHARDWARN:
			msg = _("file limit reached");
			break;
		case QUOTA_NL_ISOFTLONGWARN:
			msg = _("file quota exceeded too long");
			break;
		case QUOTA_NL_ISOFTWARN:
			msg = _("file quota exceeded");
			break;
		case QUOTA_NL_BHARDWARN:
			msg = _("block limit reached");
			break;
		case QUOTA_NL_BSOFTLONGWARN:
			msg = _("block quota exceeded too long");
			break;
		case QUOTA_NL_BSOFTWARN:
			msg = _("block quota exceeded");
			break;
		case QUOTA_NL_IHARDBELOW:
			msg = _("got below file limit");
			break;
		case QUOTA_NL_ISOFTBELOW:
			msg = _("got below file quota");
			break;
		case QUOTA_NL_BHARDBELOW:
			msg = _("got below block limit");
			break;
		case QUOTA_NL_BSOFTBELOW:
			msg = _("got below block quota");
			break;
		default:
			msg = _("unknown quota warning");
	}
	sprintf(warnbuf, "%s: %s %s %s.\r\n", level, type2name(warn->qtype), user, msg);
	if (write_all(fd, warnbuf, strlen(warnbuf)) < 0)
		errstr(_("Failed to write quota message for user %llu to %s: %s\n"), (unsigned long long)warn->caused_id, dev, strerror(errno));
	close(fd);
}

/* Send warning through DBUS */
static void write_dbus_warning(struct DBusConnection *dhandle, struct quota_warning *warn)
{
	DBusMessage* msg;
	DBusMessageIter args;

	msg = dbus_message_new_signal("/", "com.system.quota.warning", "warning");
	if (!msg) {
no_mem:
		errstr(_("Cannot create DBUS message: No enough memory.\n"));
		goto out;
	}
	dbus_message_iter_init_append(msg, &args);
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &warn->qtype))
		goto no_mem;
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT64, &warn->excess_id))
		goto no_mem;
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &warn->warntype))
		goto no_mem;
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &warn->dev_major))
		goto no_mem;
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT32, &warn->dev_minor))
		goto no_mem;
	if (!dbus_message_iter_append_basic(&args, DBUS_TYPE_UINT64, &warn->caused_id))
		goto no_mem;

	if (!dbus_connection_send(dhandle, msg, NULL)) {
		errstr(_("Failed to write message to dbus: No enough memory.\n"));
		goto out;
	}
	dbus_connection_flush(dhandle);
out:
	if (msg)
		dbus_message_unref(msg);
}

static void run(struct nl_sock *nsock)
{
	int ret;

	while (1) {
		ret = nl_recvmsgs_default(nsock);
		if (ret < 0)
			errstr(_("Failed to read or parse quota netlink"
				" message: %s\n"), strerror(-ret));
	}
}

/* Build file name (absolute path) to PID file of this daemon.
 * The returned name is allocated on heap. */
static char *build_pid_file_name(void)
{
	char *pid_name = NULL;
	if (!progname) {
		errstr(_("Undefined program name.\n"));
		return NULL;
	}
	pid_name = malloc(9 + strlen(progname) + 4 + 1);
	if (!pid_name) {
		errstr(_("Not enough memory to build PID file name.\n"));
		return NULL;
	}
	sprintf(pid_name, "/var/run/%s.pid", progname);
	return pid_name;
}

/* Store daemon's PID to file */
static int store_pid(void)
{
	FILE *pid_file;
	char *pid_name;

	pid_name = build_pid_file_name();
	if (!pid_name)
		return -1;

	pid_file = fopen(pid_name, "w");
	if (!pid_file) {
		errstr(_("Could not open PID file '%s': %s\n"),
			pid_name, strerror(errno));
		free(pid_name);
		return -1;
	}
	if (fprintf(pid_file, "%jd\n", (intmax_t)getpid()) < 0) {
		errstr(_("Could not write daemon's PID into '%s'.\n"),
			pid_name);
		fclose(pid_file);
		free(pid_name);
		return -1;
	}
	if (fclose(pid_file)) {
		errstr(_("Could not close PID file '%s'.\n"), pid_name);
		free(pid_name);
		return -1;
	}

	free(pid_name);
	return 0;
}

/* Handler for SIGTERM to remove PID file */
static void remove_pid(int signal)
{
	char *pid_name;

	pid_name = build_pid_file_name();
	if (pid_name) {
		unlink(pid_name);
		free(pid_name);
	}
	exit(EXIT_SUCCESS);
}

/* Store daemon's PID into file and register its removal on SIGTERM */
static void use_pid_file(void)
{
	struct sigaction term_action;

	term_action.sa_handler = remove_pid;
	term_action.sa_flags = 0;
	if (sigaction(SIGTERM, &term_action, NULL))
		errstr(_("Could not register PID file removal on SIGTERM.\n"));
	if (store_pid())
		errstr(_("Could not store my PID %jd.\n"), (intmax_t )getpid());
}

int main(int argc, char **argv)
{
	struct nl_sock *nsock;

	gettexton();
	progname = basename(argv[0]);
	parse_options(argc, argv);

	nsock = init_netlink();
	if (!(flags & FL_NODBUS))
		dhandle = init_dbus();
	if (!(flags & FL_NODAEMON)) {
		use_syslog();
		if (daemon(0, 0)) {
			errstr(_("Failed to daemonize: %s\n"), strerror(errno));
			exit(1);
		};
		use_pid_file();
	}
	run(nsock);
	return 0;
}
