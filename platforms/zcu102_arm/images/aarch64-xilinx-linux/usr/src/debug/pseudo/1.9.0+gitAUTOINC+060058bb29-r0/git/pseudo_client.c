/*
 * pseudo_client.c, pseudo client library code
 *
 * Copyright (c) 2008-2013 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#define _GNU_SOURCE

#include <stdio.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>

#ifdef PSEUDO_XATTRDB
#include <sys/xattr.h>
#endif

#include "pseudo.h"
#include "pseudo_ipc.h"
#include "pseudo_client.h"

/* GNU extension */
#if PSEUDO_PORT_LINUX
extern char *program_invocation_name;
#else
static char *program_invocation_name = "unknown";
#endif

static char *base_path(int dirfd, const char *path, int leave_last);

static int connect_fd = -1;
static int server_pid = 0;
int pseudo_prefix_dir_fd = -1;
int pseudo_localstate_dir_fd = -1;
int pseudo_pwd_fd = -1;
int pseudo_pwd_lck_fd = -1;
char *pseudo_pwd_lck_name = NULL;
FILE *pseudo_pwd = NULL;
int pseudo_grp_fd = -1;
FILE *pseudo_grp = NULL;
char *pseudo_cwd = NULL;
size_t pseudo_cwd_len;
char *pseudo_chroot = NULL;
char *pseudo_passwd = NULL;
size_t pseudo_chroot_len = 0;
char *pseudo_cwd_rel = NULL;
/* used for PSEUDO_DISABLED */
int pseudo_disabled = 0;
int pseudo_allow_fsync = 0;
static int pseudo_local_only = 0;
static int pseudo_client_logging = 1;

int pseudo_umask = 022;

static char **fd_paths = NULL;
static int nfds = 0;
static const char **passwd_paths = NULL;
static int npasswd_paths = 0;
#ifdef PSEUDO_PROFILING
int pseudo_profile_fd = -1;
static int profile_interval = 1;
static pseudo_profile_t profile_data;
struct timeval *pseudo_wrapper_time = &profile_data.wrapper_time;
#endif
static int pseudo_inited = 0;

static int sent_messages = 0;

int pseudo_nosymlinkexp = 0;

/* note: these are int, not uid_t/gid_t, so I can use 'em with scanf */
uid_t pseudo_ruid;
uid_t pseudo_euid;
uid_t pseudo_suid;
uid_t pseudo_fuid;
gid_t pseudo_rgid;
gid_t pseudo_egid;
gid_t pseudo_sgid;
gid_t pseudo_fgid;

int (*pseudo_real_fork)(void) = fork;
int (*pseudo_real_execv)(const char *, char * const *) = execv;

#define PSEUDO_ETC_FILE(filename, realname, flags) pseudo_etc_file(filename, realname, flags, passwd_paths, npasswd_paths)

/* helper function to make a directory, just like mkdir -p.
 * Can't use system() because the child shell would end up trying
 * to do the same thing...
 */
static void
mkdir_p(char *path) {
	size_t len = strlen(path);
	size_t i;

	for (i = 1; i < len; ++i) {
		/* try to create all the directories in path, ignoring
		 * failures
		 */
		if (path[i] == '/') {
			path[i] = '\0';
			(void) mkdir(path, 0755);
			path[i] = '/';
		}
	}
	(void) mkdir(path, 0755);
}

/* Populating an array of unknown size is one of my least favorite
 * things. The idea here is to ensure that the logic flow is the same
 * both when counting expected items, and when populating them.
 */
static void
build_passwd_paths(void)
{
	int np = 0;
	int pass = 0;
	
	/* should never happen... */
	if (passwd_paths) {
		free(passwd_paths);
		passwd_paths = 0;
		npasswd_paths = 0;
	}

#define SHOW_PATH pseudo_debug(PDBGF_CHROOT | PDBGF_VERBOSE, "passwd_paths[%d]: '%s'\n", np, (passwd_paths[np]))
#define ADD_PATH(p) do { if (passwd_paths) { passwd_paths[np] = (p); SHOW_PATH; } ++np; } while(0)
#define NUL_BYTE(p) do { if (passwd_paths) { *(p)++ = '\0'; } else { ++(p); } } while(0)

	do {
		if (pseudo_chroot) {
			ADD_PATH(pseudo_chroot);
		}
		if (pseudo_passwd) {
			char *s = pseudo_passwd;
			while (s) {
				char *t = strchr(s, ':');
				if (t) {
					NUL_BYTE(t);
				}
				ADD_PATH(s);
				s = t;
			}
		}
		if (PSEUDO_PASSWD_FALLBACK) {
			ADD_PATH(PSEUDO_PASSWD_FALLBACK);
		}

		/* allocation and/or return */
		if (passwd_paths) {
			if (np != npasswd_paths) {
				pseudo_diag("internal error: path allocation was inconsistent.\n");
			} else {
				/* yes, we allocated one extra for a trailing
				 * null pointer.
				 */
				passwd_paths[np] = NULL;
			}
			return;
		} else {
			passwd_paths = malloc((np + 1) * sizeof(*passwd_paths));
			npasswd_paths = np;
			if (!passwd_paths) {
				pseudo_diag("couldn't allocate storage for password paths.\n");
				exit(1);
			}
			np = 0;
		}
	} while (++pass < 2);
	/* in theory the second pass already returned, but. */
	pseudo_diag("should totally not have gotten here.\n");

	return;
}

#ifdef PSEUDO_XATTRDB
/* We really want to avoid calling the wrappers for these inside the
 * implementation. pseudo_wrappers will reinitialize these after it's
 * gotten the real_* found.
 */
ssize_t (*pseudo_real_lgetxattr)(const char *, const char *, void *, size_t) = lgetxattr;
ssize_t (*pseudo_real_fgetxattr)(int, const char *, void *, size_t) = fgetxattr;
int (*pseudo_real_lsetxattr)(const char *, const char *, const void *, size_t, int) = lsetxattr;
int (*pseudo_real_fsetxattr)(int, const char *, const void *, size_t, int) = fsetxattr;
/* Executive summary: We use an extended attribute,
 * user.pseudo_data, to store exactly the data we would otherwise
 * have stored in the database. Which is to say, uid, gid, mode, rdev.
 *
 * If we don't find a value, save an empty one with a lower version
 * number to indicate that we don't have data to reduce round trips.
 */
typedef struct {
	int version;
	uid_t uid;
	gid_t gid;
	mode_t mode;
	dev_t rdev;
} pseudo_db_data_t;

static pseudo_msg_t xattrdb_data;

pseudo_msg_t *
pseudo_xattrdb_save(int fd, const char *path, const struct stat64 *buf) {
	int rc = -1;
	if (!path && fd < 0)
		return NULL;
	if (!buf)
		return NULL;
	pseudo_db_data_t pseudo_db_data = {
		.version = 1,
		.uid = buf->st_uid,
		.gid = buf->st_gid,
		.mode = buf->st_mode,
		.rdev = buf->st_rdev
	};
	if (path) {
		rc = pseudo_real_lsetxattr(path, "user.pseudo_data", &pseudo_db_data, sizeof(pseudo_db_data), 0);
	} else if (fd >= 0) {
		rc = pseudo_real_fsetxattr(fd, "user.pseudo_data", &pseudo_db_data, sizeof(pseudo_db_data), 0);
	}
	pseudo_debug(PDBGF_XATTRDB, "tried to save data for %s/%d: uid %d, mode %o, rc %d.\n",
		path ? path : "<nil>", fd, (int) pseudo_db_data.uid, (int) pseudo_db_data.mode, rc);
	/* none of the other fields are checked on save, and the value
	 * is currently only really used by mknod.
	 */
	if (rc == 0) {
		xattrdb_data.result = RESULT_SUCCEED;
		return &xattrdb_data;
	}
	return NULL;
}

pseudo_msg_t *
pseudo_xattrdb_load(int fd, const char *path, const struct stat64 *buf) {
	int rc = -1, retryrc = -1;
	if (!path && fd < 0)
		return NULL;
	/* don't try to getxattr on a thing unless we think it is
	 * likely to work.
	 */
	if (buf) {
		if (!S_ISDIR(buf->st_mode) && !S_ISREG(buf->st_mode)) {
			return NULL;
		}
	}
	pseudo_db_data_t pseudo_db_data;
	if (path) {
		rc = pseudo_real_lgetxattr(path, "user.pseudo_data", &pseudo_db_data, sizeof(pseudo_db_data));
		if (rc == -1) {
			pseudo_db_data = (pseudo_db_data_t) { .version = 0 };
			retryrc = pseudo_real_lsetxattr(path, "user.pseudo_data", &pseudo_db_data, sizeof(pseudo_db_data), 0);
		}
	} else if (fd >= 0) {
		rc = pseudo_real_fgetxattr(fd, "user.pseudo_data", &pseudo_db_data, sizeof(pseudo_db_data));
		if (rc == -1) {
			pseudo_db_data = (pseudo_db_data_t) { .version = 0 };
			retryrc = pseudo_real_fsetxattr(fd, "user.pseudo_data", &pseudo_db_data, sizeof(pseudo_db_data), 0);
		}
	}
	pseudo_debug(PDBGF_XATTRDB, "tried to load data for %s[%d]: rc %d, version %d.\n",
		path ? path : "<nil>", fd, rc, pseudo_db_data.version);
	if (rc == -1 && retryrc == 0) {
		/* there's no data, but there could have been; treat
		 * this as an empty database result.
		 */
		pseudo_debug(PDBGF_XATTRDB, "wrote version 0 for %s[%d]\n",
			path ? path : "<nil>", fd);
		xattrdb_data.result = RESULT_FAIL;
		return &xattrdb_data;
	} else if (rc == -1) {
		/* we can't create an extended attribute, so we may have
		 * used the database.
		 */
		return NULL;
	}
	/* Version 0 = just recording that we looked and found
	 * nothing.
	 * Version 1 = actually implemented.
	 */
	switch (pseudo_db_data.version) {
	case 0:
	default:
		xattrdb_data.result = RESULT_FAIL;
		break;
	case 1:
		xattrdb_data.uid = pseudo_db_data.uid;
		xattrdb_data.gid = pseudo_db_data.gid;
		xattrdb_data.mode = pseudo_db_data.mode;
		xattrdb_data.rdev = pseudo_db_data.rdev;
		xattrdb_data.result = RESULT_SUCCEED;
		break;
	}
	return &xattrdb_data;
}
#endif

#ifdef PSEUDO_PROFILING
static int pseudo_profile_pid = -2;

static void
pseudo_profile_start(void) {
	/* We use -1 as a starting value, and -2 as a value
	 * indicating not to try to open it.
	 */
	int existing_data = 0;
	int pid = getpid();
	if (pseudo_profile_pid > 0 && pseudo_profile_pid != pid) {
		/* looks like there's been a fork. We intentionally
		 * abandon any existing stats, since the parent might
		 * want to write them, and we want to combine with
		 * any previous stats for this pid.
		 */
		close(pseudo_profile_fd);
		pseudo_profile_fd = -1;
	}
	if (pseudo_profile_fd == -1) {
		int fd = -2;
		char *profile_path = pseudo_get_value("PSEUDO_PROFILE_PATH");
		pseudo_profile_pid = pid;
		if (profile_path) {
			fd = open(profile_path, O_RDWR | O_CREAT, 0600);
			if (fd >= 0) {
				fd = pseudo_fd(fd, MOVE_FD);
			}
			if (fd < 0) {
				fd = -2;
			} else {
				if (pid > 0) {
					pseudo_profile_t data;
					off_t rc;

					rc = lseek(fd, pid * sizeof(data), SEEK_SET);
					if (rc == (off_t) (pid * sizeof(data))) {
						rc = read(fd, &profile_data, sizeof(profile_data));
						/* cumulative with other values in same file */
						if (rc == sizeof(profile_data)) {
							pseudo_debug(PDBGF_PROFILE, "pid %d found existing profiling data.\n", pid);
							existing_data = 1;
							++profile_data.processes;
						} else {
							pseudo_debug(PDBGF_PROFILE, "read failed for pid %d: %d, %d\n", pid, (int) rc, errno);
						}
						profile_interval = 1;
					}
				}
			}
		}
		pseudo_profile_fd = fd;
	} else {
		pseudo_debug(PDBGF_PROFILE, "_start called with existing fd? (pid %d)", (int) pseudo_profile_pid);
		existing_data = 1;
		++profile_data.processes;
		profile_data.total_ops = 0;
		profile_data.messages = 0;
		profile_data.wrapper_time = (struct timeval) { .tv_sec = 0 };
		profile_data.op_time = (struct timeval) { .tv_sec = 0 };
		profile_data.ipc_time = (struct timeval) { .tv_sec = 0 };
	}
	if (!existing_data) {
		pseudo_debug(PDBGF_PROFILE, "pid %d found no existing profiling data.\n", pid);
		profile_data = (pseudo_profile_t) {
			.processes = 1,
			.total_ops = 0,
			.messages = 0,
			.wrapper_time = (struct timeval) { .tv_sec = 0 },
			.op_time = (struct timeval) { .tv_sec = 0 },
			.ipc_time = (struct timeval) { .tv_sec = 0 },
		};
	}
}

static inline void
fix_tv(struct timeval *tv) {
	if (tv->tv_usec > 1000000) {
		tv->tv_sec += tv->tv_usec / 1000000;
		tv->tv_usec %= 1000000;
	} else if (tv->tv_usec < 0) {
		/* C99 and later guarantee truncate-towards-zero.
		 * We want -1 through -1000000 usec to produce
		 * -1 seconds, etcetera. Note that sec is
		 * negative, so yes, we want to add to tv_sec and
		 * subtract from tv_usec.
		 */
		int sec = (tv->tv_usec - 999999) / 1000000;
		tv->tv_sec += sec;
		tv->tv_usec -= 1000000 * sec;
	}
}

static int profile_reported = 0;
static void
pseudo_profile_report(void) {
	if (pseudo_profile_fd < 0) {
		return;
	}
	fix_tv(&profile_data.wrapper_time);
	fix_tv(&profile_data.op_time);
	fix_tv(&profile_data.ipc_time);
	if (pseudo_profile_pid >= 0) {
		int rc1, rc2;
		rc1 = lseek(pseudo_profile_fd, pseudo_profile_pid * sizeof(profile_data), SEEK_SET);
		rc2 = write(pseudo_profile_fd, &profile_data, sizeof(profile_data));
		if (!profile_reported) {
			pseudo_debug(PDBGF_PROFILE, "pid %d (%s) saving profiling info: %d, %d.\n",
				pseudo_profile_pid,
				program_invocation_name ? program_invocation_name : "unknown",
				rc1, rc2);
			profile_reported = 1;
		}
	}
}
#endif

void
pseudo_init_client(void) {
	char *env;

	pseudo_antimagic();
	pseudo_new_pid();
	if (connect_fd != -1) {
		close(connect_fd);
		connect_fd = -1;
	}
#ifdef PSEUDO_PROFILING
	if (pseudo_profile_fd > -1) {
		close(pseudo_profile_fd);
	}
	pseudo_profile_fd = -1;
#endif

	/* in child processes, PSEUDO_DISABLED may have become set to
	 * some truthy value, in which case we'd disable pseudo,
	 * or it may have gone away, in which case we'd enable
	 * pseudo (and cause it to reinit the defaults).
	 */
	env = getenv("PSEUDO_DISABLED");
	if (!env) {
		env = pseudo_get_value("PSEUDO_DISABLED");
	}
	if (env) {
		int actually_disabled = 1;
		switch (*env) {
		case '0':
		case 'f':
		case 'F':
		case 'n':
		case 'N':
			actually_disabled = 0;
			break;
		case 's':
		case 'S':
			actually_disabled = 0;
			pseudo_local_only = 1;
			break;
		}
		if (actually_disabled) {
			if (!pseudo_disabled) {
				pseudo_antimagic();
				pseudo_disabled = 1;
			}
			pseudo_set_value("PSEUDO_DISABLED", "1");
		} else {
			if (pseudo_disabled) {
				pseudo_magic();
				pseudo_disabled = 0;
				pseudo_inited = 0; /* Re-read the initial values! */
			}
			pseudo_set_value("PSEUDO_DISABLED", "0");
		}
	} else {
		pseudo_set_value("PSEUDO_DISABLED", "0");
	}

	/* ALLOW_FSYNC is here because some crazy hosts will otherwise
	 * report incorrect values for st_size/st_blocks. I can sort of
	 * understand st_blocks, but bogus values for st_size? Not cool,
	 * dudes, not cool.
	 */
	env = getenv("PSEUDO_ALLOW_FSYNC");
	if (!env) {
		env = pseudo_get_value("PSEUDO_ALLOW_FSYNC");
	} else {
		pseudo_set_value("PSEUDO_ALLOW_FSYNC", env);
	}
	if (env) {
		pseudo_allow_fsync = 1;
	} else {
		pseudo_allow_fsync = 0;
	}

	/* in child processes, PSEUDO_UNLOAD may become set to
	 * some truthy value, in which case we're being asked to
	 * remove pseudo from the LD_PRELOAD. We need to make sure
	 * this value gets loaded into the internal variables.
	 *
	 * If we've been told to unload, but are still available
	 * we need to act as if unconditionally disabled.
	 */
	env = getenv("PSEUDO_UNLOAD");
	if (env) {
		pseudo_set_value("PSEUDO_UNLOAD", env);
		pseudo_disabled = 1;
	}

	/* Setup global items needed for pseudo to function... */
	if (!pseudo_inited) {
		/* Ensure that all of the values are reset */
		server_pid = 0;
		pseudo_prefix_dir_fd = -1;
		pseudo_localstate_dir_fd = -1;
		pseudo_pwd_fd = -1;
		pseudo_pwd_lck_fd = -1;
		pseudo_pwd_lck_name = NULL;
		pseudo_pwd = NULL;
		pseudo_grp_fd = -1;
		pseudo_grp = NULL;
		pseudo_cwd = NULL;
		pseudo_cwd_len = 0;
		pseudo_chroot = NULL;
		pseudo_passwd = NULL;
		pseudo_chroot_len = 0;
		pseudo_cwd_rel = NULL;
		pseudo_nosymlinkexp = 0;
	}

	if (!pseudo_disabled && !pseudo_inited) {
		char *pseudo_path = 0;

		pseudo_umask = umask(022);
		umask(pseudo_umask);

		pseudo_path = pseudo_prefix_path(NULL);
		if (pseudo_prefix_dir_fd == -1) {
			if (pseudo_path) {
				pseudo_prefix_dir_fd = open(pseudo_path, O_RDONLY);
				/* directory is missing? */
				if (pseudo_prefix_dir_fd == -1 && errno == ENOENT) {
					pseudo_debug(PDBGF_CLIENT, "prefix directory '%s' doesn't exist, trying to create\n", pseudo_path);
					mkdir_p(pseudo_path);
					pseudo_prefix_dir_fd = open(pseudo_path, O_RDONLY);
				}
				pseudo_prefix_dir_fd = pseudo_fd(pseudo_prefix_dir_fd, MOVE_FD);
			} else {
				pseudo_diag("No prefix available to to find server.\n");
				exit(1);
			}
			if (pseudo_prefix_dir_fd == -1) {
				pseudo_diag("Can't open prefix path '%s' for server: %s\n",
					pseudo_path,
					strerror(errno));
				exit(1);
			}
		}
		free(pseudo_path);
		pseudo_path = pseudo_localstatedir_path(NULL);
		if (pseudo_localstate_dir_fd == -1) {
			if (pseudo_path) {
				pseudo_localstate_dir_fd = open(pseudo_path, O_RDONLY);
				/* directory is missing? */
				if (pseudo_localstate_dir_fd == -1 && errno == ENOENT) {
					pseudo_debug(PDBGF_CLIENT, "local state directory '%s' doesn't exist, trying to create\n", pseudo_path);
					mkdir_p(pseudo_path);
					pseudo_localstate_dir_fd = open(pseudo_path, O_RDONLY);
				}
				pseudo_localstate_dir_fd = pseudo_fd(pseudo_localstate_dir_fd, MOVE_FD);
			} else {
				pseudo_diag("No local state directory available for server/file interactions.\n");
				exit(1);
			}
			if (pseudo_localstate_dir_fd == -1) {
				pseudo_diag("Can't open local state path '%s': %s\n",
					pseudo_path,
					strerror(errno));
				exit(1);
			}
		}
		free(pseudo_path);

		env = pseudo_get_value("PSEUDO_NOSYMLINKEXP");
		if (env) {
			char *endptr;
			/* if the environment variable is not an empty string,
			 * parse it; "0" means turn NOSYMLINKEXP off, "1" means
			 * turn it on (disabling the feature).  An empty string
			 * or something we can't parse means to set the flag; this
			 * is a safe default because if you didn't want the flag
			 * set, you normally wouldn't set the environment variable
			 * at all.
			 */
			if (*env) {
				pseudo_nosymlinkexp = strtol(env, &endptr, 10);
				if (*endptr)
					pseudo_nosymlinkexp = 1;
			} else {
				pseudo_nosymlinkexp = 1;
			}
		} else {
			pseudo_nosymlinkexp = 0;
		}
		free(env);
		env = pseudo_get_value("PSEUDO_UIDS");
		if (env)
			sscanf(env, "%d,%d,%d,%d",
				&pseudo_ruid, &pseudo_euid,
				&pseudo_suid, &pseudo_fuid);
		free(env);

		env = pseudo_get_value("PSEUDO_GIDS");
		if (env)
			sscanf(env, "%d,%d,%d,%d",
				&pseudo_rgid, &pseudo_egid,
				&pseudo_sgid, &pseudo_fuid);
		free(env);

		env = pseudo_get_value("PSEUDO_CHROOT");
		if (env) {
			pseudo_chroot = strdup(env);
			if (pseudo_chroot) {
				pseudo_chroot_len = strlen(pseudo_chroot);
			} else {
				pseudo_diag("Can't store chroot path '%s'\n", env);
			}
		}
		free(env);

		env = pseudo_get_value("PSEUDO_PASSWD");
		if (env) {
			/* note: this means that pseudo_passwd is a
			 * string we're allowed to modify... */
			pseudo_passwd = strdup(env);
		}
		free(env);
		build_passwd_paths();

		pseudo_inited = 1;
	}
	if (!pseudo_disabled) {
		pseudo_client_getcwd();
#ifdef PSEUDO_PROFILING
		pseudo_profile_start();
#endif
	}

	pseudo_magic();
}

static void
pseudo_file_close(int *fd, FILE **fp) {
	if (!fp || !fd) {
		pseudo_diag("pseudo_file_close: needs valid pointers.\n");
		return;
	}
	pseudo_antimagic();
	if (*fp) {
#if PSEUDO_PORT_DARWIN
		if (*fp == pseudo_host_etc_passwd_file) {
			endpwent();
		} else if (*fp != pseudo_host_etc_group_file) {
			endgrent();
		} else {
			fclose(*fp);
		}
#else
		fclose(*fp);
#endif
		*fd = -1;
		*fp = 0;
	}
#if PSEUDO_PORT_DARWIN
	if (*fd == pseudo_host_etc_passwd_fd ||
	    *fd == pseudo_host_etc_group_fd) {
		*fd = -1;
	}
#endif
	/* this should be impossible */
	if (*fd >= 0) {
		close(*fd);
		*fd = -1;
	}
	pseudo_magic();
}

static FILE *
pseudo_file_open(char *name, int *fd, FILE **fp) {
	if (!fp || !fd || !name) {
		pseudo_diag("pseudo_file_open: needs valid pointers.\n");
		return NULL;
	}
	pseudo_file_close(fd, fp);
	pseudo_antimagic();
	*fd = PSEUDO_ETC_FILE(name, NULL, O_RDONLY);
#if PSEUDO_PORT_DARWIN
	if (*fd == pseudo_host_etc_passwd_fd) {
		*fp = pseudo_host_etc_passwd_file;
		setpwent();
	} else if (*fd == pseudo_host_etc_group_fd) {
		*fp = pseudo_host_etc_group_file;
		setgrent();
	}
#endif
	if (*fd >= 0) {
		*fd = pseudo_fd(*fd, MOVE_FD);
		*fp = fdopen(*fd, "r");
		if (!*fp) {
			close(*fd);
			*fd = -1;
		}
	}
	pseudo_magic();
	return *fp;
}

/* there is no spec I know of requiring us to defend this fd
 * against being closed by the user.
 */
int
pseudo_pwd_lck_open(void) {
	pseudo_pwd_lck_close();
	if (!pseudo_pwd_lck_name) {
		pseudo_pwd_lck_name = malloc(pseudo_path_max());
		if (!pseudo_pwd_lck_name) {
			pseudo_diag("couldn't allocate space for passwd lockfile path.\n");
			return -1;
		}
	}
	pseudo_antimagic();
	pseudo_pwd_lck_fd = PSEUDO_ETC_FILE(".pwd.lock",
					pseudo_pwd_lck_name, O_RDWR | O_CREAT);
	pseudo_magic();
	return pseudo_pwd_lck_fd;
}

int
pseudo_pwd_lck_close(void) {
	if (pseudo_pwd_lck_fd != -1) {
		pseudo_antimagic();
		close(pseudo_pwd_lck_fd);
		if (pseudo_pwd_lck_name) {
			unlink(pseudo_pwd_lck_name);
			free(pseudo_pwd_lck_name);
			pseudo_pwd_lck_name = 0;
		}
		pseudo_magic();
		pseudo_pwd_lck_fd = -1;
		return 0;
	} else {
		return -1;
	}
}

FILE *
pseudo_pwd_open(void) {
	return pseudo_file_open("passwd", &pseudo_pwd_fd, &pseudo_pwd);
}

void
pseudo_pwd_close(void) {
	pseudo_file_close(&pseudo_pwd_fd, &pseudo_pwd);
}

FILE *
pseudo_grp_open(void) {
	return pseudo_file_open("group", &pseudo_grp_fd, &pseudo_grp);
}

void
pseudo_grp_close(void) {
	pseudo_file_close(&pseudo_grp_fd, &pseudo_grp);
}

void
pseudo_client_touchuid(void) {
	static char uidbuf[256];
	snprintf(uidbuf, 256, "%d,%d,%d,%d",
		pseudo_ruid, pseudo_euid, pseudo_suid, pseudo_fuid);
	pseudo_set_value("PSEUDO_UIDS", uidbuf);
}

void
pseudo_client_touchgid(void) {
	static char gidbuf[256];
	snprintf(gidbuf, 256, "%d,%d,%d,%d",
		pseudo_rgid, pseudo_egid, pseudo_sgid, pseudo_fgid);
	pseudo_set_value("PSEUDO_GIDS", gidbuf);
}

int
pseudo_client_chroot(const char *path) {
	/* free old value */
	free(pseudo_chroot);

	pseudo_debug(PDBGF_CLIENT | PDBGF_CHROOT, "client chroot: %s\n", path);
	if (!strcmp(path, "/")) {
		pseudo_chroot_len = 0;
		pseudo_chroot = 0;
		pseudo_set_value("PSEUDO_CHROOT", NULL);
		return 0;
	}
	/* allocate new value */
	pseudo_chroot_len = strlen(path);
	pseudo_chroot = malloc(pseudo_chroot_len + 1);
	if (!pseudo_chroot) {
		pseudo_diag("Couldn't allocate chroot directory buffer.\n");
		pseudo_chroot_len = 0;
		errno = ENOMEM;
		return -1;
	}
	memcpy(pseudo_chroot, path, pseudo_chroot_len + 1);
	pseudo_set_value("PSEUDO_CHROOT", pseudo_chroot);
	return 0;
}

char *
pseudo_root_path(const char *func, int line, int dirfd, const char *path, int leave_last) {
	char *rc;
	pseudo_antimagic();
	rc = base_path(dirfd, path, leave_last);
	pseudo_magic();
	if (!rc) {
		pseudo_diag("couldn't allocate absolute path for '%s'.\n",
			path);
	}
	pseudo_debug(PDBGF_CHROOT, "root_path [%s, %d]: '%s' from '%s'\n",
		func, line,
		rc ? rc : "<nil>",
		path ? path : "<nil>");
	return rc;
}

int
pseudo_client_getcwd(void) {
	char *cwd;
	cwd = malloc(pseudo_path_max());
	if (!cwd) {
		pseudo_diag("Can't allocate CWD buffer!\n");
		return -1;
	}
	pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "getcwd: trying to find cwd.\n");
	if (getcwd(cwd, pseudo_path_max())) {
		/* cwd now holds a canonical path to current directory */
		free(pseudo_cwd);
		pseudo_cwd = cwd;
		pseudo_cwd_len = strlen(pseudo_cwd);
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "getcwd okay: [%s] %d bytes\n", pseudo_cwd, (int) pseudo_cwd_len);
		if (pseudo_chroot_len &&
			pseudo_cwd_len >= pseudo_chroot_len &&
			!memcmp(pseudo_cwd, pseudo_chroot, pseudo_chroot_len) &&
			(pseudo_cwd[pseudo_chroot_len] == '\0' ||
			pseudo_cwd[pseudo_chroot_len] == '/')) {
			pseudo_cwd_rel = pseudo_cwd + pseudo_chroot_len;
		} else {
			pseudo_cwd_rel = pseudo_cwd;
		}
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "abscwd: <%s>\n", pseudo_cwd);
		if (pseudo_cwd_rel != pseudo_cwd) {
			pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "relcwd: <%s>\n", pseudo_cwd_rel);
		}
		return 0;
	} else {
		pseudo_diag("Can't get CWD: %s\n", strerror(errno));
		return -1;
	}
}

static char *
fd_path(int fd) {
	if (fd >= 0 && fd < nfds) {
		return fd_paths[fd];
	}
	if (fd == AT_FDCWD) {
		return pseudo_cwd;
	}
	return 0;
}

static void
pseudo_client_path(int fd, const char *path) {
	if (fd < 0)
		return;

	if (fd >= nfds) {
		int i;
		pseudo_debug(PDBGF_CLIENT, "expanding fds from %d to %d\n",
			nfds, fd + 1);
		fd_paths = realloc(fd_paths, (fd + 1) * sizeof(char *));
		for (i = nfds; i < fd + 1; ++i)
			fd_paths[i] = 0;
		nfds = fd + 1;
	} else {
		if (fd_paths[fd]) {
			pseudo_debug(PDBGF_CLIENT, "reopening fd %d [%s] -- didn't see close\n",
				fd, fd_paths[fd]);
		}
		/* yes, it is safe to free null pointers. yay for C89 */
		free(fd_paths[fd]);
		fd_paths[fd] = 0;
	}
	if (path) {
		fd_paths[fd] = strdup(path);
	}
}

static void
pseudo_client_close(int fd) {
	if (fd < 0 || fd >= nfds)
		return;

	free(fd_paths[fd]);
	fd_paths[fd] = 0;
}

/* spawn server */
static int
client_spawn_server(void) {
	int status;
	FILE *fp;
	char * pseudo_pidfile;

	if ((server_pid = pseudo_real_fork()) != 0) {
		if (server_pid == -1) {
			pseudo_diag("couldn't fork server: %s\n", strerror(errno));
			return 1;
		}
		pseudo_evlog(PDBGF_CLIENT, "spawned new server, pid %d\n", server_pid);
		pseudo_debug(PDBGF_CLIENT | PDBGF_SERVER, "spawned server, pid %d\n", server_pid);
		/* wait for the child process to terminate, indicating server
		 * is ready
		 */
		waitpid(server_pid, &status, 0);
		if (WIFEXITED(status)) {
			pseudo_evlog(PDBGF_CLIENT, "server exited status %d\n", WEXITSTATUS(status));
		}
		if (WIFSIGNALED(status)) {
			pseudo_evlog(PDBGF_CLIENT, "server exited from signal %d\n", WTERMSIG(status));
		}
		server_pid = -2;
		if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
			pseudo_evlog(PDBGF_CLIENT, "server reports successful startup, reading pid.\n");
			pseudo_pidfile = pseudo_localstatedir_path(PSEUDO_PIDFILE);
			fp = fopen(pseudo_pidfile, "r");
			if (fp) {
				if (fscanf(fp, "%d", &server_pid) != 1) {
					pseudo_debug(PDBGF_CLIENT, "Opened server PID file, but didn't get a pid.\n");
				}
				fclose(fp);
			} else {
				pseudo_debug(PDBGF_CLIENT, "no pid file (%s): %s\n",
					pseudo_pidfile, strerror(errno));
			}
			pseudo_debug(PDBGF_CLIENT, "read new pid file: %d\n", server_pid);
			free(pseudo_pidfile);
			/* at this point, we should have a new server_pid */
			return 0;
		} else {
			pseudo_evlog(PDBGF_CLIENT, "server startup apparently unsuccessful.  setting server pid to -1.\n");
			server_pid = -1;
			return 1;
		}
	} else {
		char *base_args[] = { NULL, NULL, NULL };
		char **argv;
		char *option_string = pseudo_get_value("PSEUDO_OPTS");
		int args;
		int fd;

		pseudo_new_pid();
		base_args[0] = pseudo_bindir_path("pseudo");
		base_args[1] = "-d";
		if (option_string) {
			char *s;
			int arg;

			/* count arguments in PSEUDO_OPTS, starting at 2
			 * for pseudo/-d/NULL, plus one for the option string.
			 * The number of additional arguments may be less
			 * than the number of spaces, but can't be more.
			 */
			args = 4;
			for (s = option_string; *s; ++s)
				if (*s == ' ')
					++args;

			argv = malloc(args * sizeof(char *));
			argv[0] = base_args[0];
			argv[1] = base_args[1];
			arg = 2;
			while ((s = strsep(&option_string, " ")) != NULL) {
				if (*s) {
					argv[arg++] = strdup(s);
				}
			}
			argv[arg] = 0;
		} else {
			argv = base_args;
		}

		/* close any higher-numbered fds which might be open,
		 * such as sockets.  We don't have to worry about 0 and 1;
		 * the server closes them already, and more importantly,
		 * they can't have been opened or closed without us already
		 * having spawned a server... The issue is just socket()
		 * calls which could result in fds being left open, and those
		 * can't overwrite fds 0-2 unless we closed them...
		 * 
		 * No, really.  It works.
		 */
		for (fd = 3; fd < 1024; ++fd) {
			if (fd != pseudo_util_debug_fd)
				close(fd);
		}
		/* and now, execute the server */
		pseudo_debug(PDBGF_CLIENT | PDBGF_SERVER | PDBGF_INVOKE, "calling execv on %s\n", argv[0]);

		/* don't try to log this exec, because it'll cause the process
		 * that is supposed to be spawning the server to try to spawn
		 * a server. Whoops. This is because the exec wrapper doesn't
		 * respect antimagic, which I believe is intentional.
		 */
		pseudo_client_logging = 0;

		/* manual setup of environment, so we can call real-execv
		 * instead of the wrapper.
		 */
		pseudo_set_value("PSEUDO_UNLOAD", "YES");
		pseudo_setupenv();
		pseudo_dropenv();
		pseudo_real_execv(argv[0], argv);
		pseudo_diag("critical failure: exec of pseudo daemon failed: %s\n", strerror(errno));
		exit(1);
	}
}

static int
client_ping(void) {
	pseudo_msg_t ping;
	pseudo_msg_t *ack;
	char tagbuf[pseudo_path_max()];
	char *tag = pseudo_get_value("PSEUDO_TAG");

	memset(&ping, 0, sizeof(ping));

	ping.type = PSEUDO_MSG_PING;
	ping.op = OP_NONE;

	ping.pathlen = snprintf(tagbuf, sizeof(tagbuf), "%s%c%s",
		program_invocation_name ? program_invocation_name : "<unknown>",
		0,
		tag ? tag : "");
	free(tag);
	ping.client = getpid();
	ping.result = 0;
	errno = 0;
	pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "sending ping\n");
	if (pseudo_msg_send(connect_fd, &ping, ping.pathlen, tagbuf)) {
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "error pinging server: %s\n", strerror(errno));
		return 1;
	}
	ack = pseudo_msg_receive(connect_fd);
	if (!ack) {
		pseudo_debug(PDBGF_CLIENT, "no ping response from server: %s\n", strerror(errno));
		/* and that's not good, so... */
		server_pid = 0;
		return 1;
	}
	if (ack->type != PSEUDO_MSG_ACK) {
		pseudo_debug(PDBGF_CLIENT, "invalid ping response from server: expected ack, got %d\n", ack->type);
		/* and that's not good, so... */
		server_pid = 0;
		return 1;
	} else {
                /* The server tells us whether or not to log things. */
                if (ack->result == RESULT_SUCCEED) {
                        pseudo_client_logging = 1;
                } else {
                        pseudo_client_logging = 0;
                }
        }
	pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "ping ok\n");
	return 0;
}

static void
void_client_ping(void) {
	client_ping();
}

int
pseudo_fd(int fd, int how) {
	int newfd;

	if (fd < 0)
		return(-1);

	/* If already above PSEUDO_MIN_FD, no need to move */
	if ((how == MOVE_FD) && (fd >= PSEUDO_MIN_FD)) {
		newfd = fd;
	} else {
		newfd = fcntl(fd, F_DUPFD, PSEUDO_MIN_FD);

		if (how == MOVE_FD)
			close(fd);
	}

	/* Set close on exec, even if we didn't move it. */
	if ((newfd >= 0) && (fcntl(newfd, F_SETFD, FD_CLOEXEC) < 0))
		pseudo_diag("Can't set close on exec flag: %s\n",
			strerror(errno));

	return(newfd);
}

static int
client_connect(void) {
	/* we have a server pid, is it responsive? */
	struct sockaddr_un sun = { .sun_family = AF_UNIX, .sun_path = PSEUDO_SOCKET };
	int cwd_fd;

#if PSEUDO_PORT_DARWIN
	sun.sun_len = strlen(PSEUDO_SOCKET) + 1;
#endif

	connect_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	connect_fd = pseudo_fd(connect_fd, MOVE_FD);
	pseudo_evlog(PDBGF_CLIENT, "creating socket %s.\n", sun.sun_path);
	if (connect_fd == -1) {
		char *e = strerror(errno);
		pseudo_diag("Can't create socket: %s (%s)\n", sun.sun_path, e);
		pseudo_evlog(PDBGF_CLIENT, "failed to create socket: %s\n", e);
		return 1;
	}

	pseudo_debug(PDBGF_CLIENT, "connecting socket...\n");
	cwd_fd = open(".", O_RDONLY);
	if (cwd_fd == -1) {
		pseudo_diag("Couldn't stash directory before opening socket: %s",
			strerror(errno));
		close(connect_fd);
		connect_fd = -1;
		return 1;
	}
	if (fchdir(pseudo_localstate_dir_fd) == -1) {
		pseudo_diag("Couldn't chdir to server directory [%d]: %s\n",
			pseudo_localstate_dir_fd, strerror(errno));
		close(connect_fd);
		close(cwd_fd);
		connect_fd = -1;
		return 1;
	}
	if (connect(connect_fd, (struct sockaddr *) &sun, sizeof(sun)) == -1) {
		char *e = strerror(errno);
		pseudo_debug(PDBGF_CLIENT, "Can't connect socket to pseudo.socket: (%s)\n", e);
		pseudo_evlog(PDBGF_CLIENT, "connect failed: %s\n", e);
		close(connect_fd);
		if (fchdir(cwd_fd) == -1) {
			pseudo_diag("return to previous directory failed: %s\n",
				strerror(errno));
		}
		close(cwd_fd);
		connect_fd = -1;
		return 1;
	}
	if (fchdir(cwd_fd) == -1) {
		pseudo_diag("return to previous directory failed: %s\n",
			strerror(errno));
	}
	close(cwd_fd);
	pseudo_evlog(PDBGF_CLIENT, "socket connect OK.\n");
	pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "connected socket.\n");
	return 0;
}

static int
pseudo_client_setup(void) {
	char * pseudo_pidfile;
	FILE *fp;
	server_pid = 0;

	/* avoid descriptor leak, I hope */
	if (connect_fd >= 0) {
		close(connect_fd);
		connect_fd = -1;
	}
	if (client_connect() == 0) {
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "connection started.\n");
		if (client_ping() == 0) {
			pseudo_debug(PDBGF_CLIENT, "connection ping okay, server ready.\n");
			return 0;
		} else {
			pseudo_debug(PDBGF_CLIENT, "connection up, but ping failed.\n");
			/* and now we'll close the connection, and try to
			 * kick the server.
			 */
			close(connect_fd);
			connect_fd = -1;
		}
	}
	pseudo_debug(PDBGF_CLIENT, "connection failed, checking server...\n");

	/* This whole section is purely informational; we want to know
	 * what might be up with the server, but we're going to try to
	 * start one anyway.
	 */
	pseudo_pidfile = pseudo_localstatedir_path(PSEUDO_PIDFILE);
	fp = fopen(pseudo_pidfile, "r");
	free(pseudo_pidfile);
	if (fp) {
		if (fscanf(fp, "%d", &server_pid) != 1) {
			pseudo_debug(PDBGF_CLIENT, "Opened server PID file, but didn't get a pid.\n");
		}
		fclose(fp);
	}
	if (server_pid > 0) {
		if (kill(server_pid, 0) == -1) {
			pseudo_debug(PDBGF_CLIENT, "couldn't find server at pid %d: %s\n",
				server_pid, strerror(errno));
			server_pid = 0;
		} else {
			pseudo_debug(PDBGF_CLIENT, "server pid should be %d, which exists, but connection failed.\n",
				server_pid);
			/* and we'll restart the server anyway. */
		}
	} else {
		pseudo_debug(PDBGF_CLIENT, "no server pid available.\n");
	}
	if (client_spawn_server()) {

		pseudo_evlog(PDBGF_CLIENT, "attempted respawn, failed.\n");
		pseudo_debug(PDBGF_CLIENT, "failed to spawn server, waiting for retry.\n");
		return 1;
	} else {
		pseudo_evlog(PDBGF_CLIENT, "restarted, new pid %d, will retry message\n", server_pid);
		pseudo_debug(PDBGF_CLIENT, "restarted, new pid %d, will retry message\n", server_pid);
		/* so we think a server has started. Now we'll retry the
		 * connect/ping, because if they work we'll have set
		 * connect_fd correctly, and will have succeeded.
		 */
		if (!client_connect()) {
			pseudo_debug(PDBGF_CLIENT, "connect okay to restarted server.\n");
			pseudo_evlog(PDBGF_CLIENT, "connect okay to restarted server.\n");
			if (!client_ping()) {
				pseudo_debug(PDBGF_CLIENT, "ping okay to restarted server.\n");
				pseudo_evlog(PDBGF_CLIENT, "ping okay to restarted server.\n");
				return 0;
			} else {
				pseudo_debug(PDBGF_CLIENT, "ping failed to restarted server.\n");
				pseudo_evlog(PDBGF_CLIENT, "ping failed to restarted server.\n");
				close(connect_fd);
				connect_fd = -1;
				return 1;
			}
		} else {
			pseudo_debug(PDBGF_CLIENT, "connect failed to restarted server.\n");
			pseudo_evlog(PDBGF_CLIENT, "connect failed to restarted server.\n");
			return 1;
		}
	}
}

#define PSEUDO_RETRIES 250
static pseudo_msg_t *
pseudo_client_request(pseudo_msg_t *msg, size_t len, const char *path) {
	pseudo_msg_t *response = 0;
	int tries = 0;
	int rc;
	extern char *program_invocation_short_name;
	#if 0
	if (!strcmp(program_invocation_short_name, "pseudo"))
		abort();
	#endif

	if (!msg)
		return 0;

	/* Try to send a message. If sending fails, try to spawn a server,
	 * and whether or not we succeed, wait a little bit and retry sending.
	 * It's okay if we can't start a server sometimes, because another
	 * client may have done it.
	 */
	pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "sending a message: ino %llu\n",
		(unsigned long long) msg->ino);
        for (tries = 0; tries < PSEUDO_RETRIES; ++tries) {
		pseudo_evlog(PDBGF_CLIENT, "try %d, connect fd is %d\n", tries, connect_fd);
		rc = pseudo_msg_send(connect_fd, msg, len, path);
		if (rc != 0) {
			pseudo_evlog(PDBGF_CLIENT, "msg_send failed [%d].\n", rc);
			pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "msg_send: %d%s\n",
				rc,
				rc == -1 ? " (sigpipe)" :
					   " (short write)");
			pseudo_debug(PDBGF_CLIENT, "trying to get server, try %d\n", tries);
			/* try to open server; if we fail, wait a bit before
			 * retry.
			 */
			if (pseudo_client_setup()) {
				int ms = (getpid() % 5) + (3 * tries);
				struct timespec delay = { .tv_sec = 0, .tv_nsec = ms * 1000000 };
				pseudo_evlog(PDBGF_CLIENT, "setup failed, delaying %d ms.\n", ms);
				nanosleep(&delay, NULL);
			} else {
				pseudo_evlog(PDBGF_CLIENT, "setup apparently successful, retrying message immediately.\n");
			}
			continue;
		}
		/* note "continue" above; we only get here if rc was 0,
		 * indicating a successful send.
		 */
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "sent!\n");
		response = pseudo_msg_receive(connect_fd);
		if (!response) {
			pseudo_debug(PDBGF_CLIENT, "expected response did not occur; retrying\n");
		} else {
			if (response->type != PSEUDO_MSG_ACK) {
				pseudo_debug(PDBGF_CLIENT, "got non-ack response %d\n", response->type);
				return 0;
			} else {
				pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "got response type %d\n", response->type);
				return response;
			}
		}
	}
	pseudo_diag("pseudo: server connection persistently failed, aborting.\n");
	pseudo_evlog_dump();
	pseudo_diag("event log dumped, aborting.\n");
	abort();
	pseudo_diag("aborted.\n");
	return 0;
}

int
pseudo_client_shutdown(int wait_on_socket) {
	pseudo_msg_t msg;
	pseudo_msg_t *ack;
	char *pseudo_path;

	pseudo_debug(PDBGF_INVOKE, "attempting to shut down server.\n");
	pseudo_path = pseudo_prefix_path(NULL);
	if (pseudo_prefix_dir_fd == -1) {
		if (pseudo_path) {
			pseudo_prefix_dir_fd = open(pseudo_path, O_RDONLY);
			/* directory is missing? */
			if (pseudo_prefix_dir_fd == -1 && errno == ENOENT) {
				pseudo_debug(PDBGF_CLIENT, "prefix directory doesn't exist, trying to create\n");
				mkdir_p(pseudo_path);
				pseudo_prefix_dir_fd = open(pseudo_path, O_RDONLY);
			}
			pseudo_prefix_dir_fd = pseudo_fd(pseudo_prefix_dir_fd, COPY_FD);
			free(pseudo_path);
		} else {
			pseudo_diag("No prefix available to to find server.\n");
			exit(1);
		}
		if (pseudo_prefix_dir_fd == -1) {
			pseudo_diag("Can't open prefix path (%s) for server. (%s)\n",
				pseudo_prefix_path(NULL),
				strerror(errno));
			exit(1);
		}
	}
	pseudo_path = pseudo_localstatedir_path(NULL);
	mkdir_p(pseudo_path);
	if (pseudo_localstate_dir_fd == -1) {
		if (pseudo_path) {
			pseudo_localstate_dir_fd = open(pseudo_path, O_RDONLY);
			/* directory is missing? */
			if (pseudo_localstate_dir_fd == -1 && errno == ENOENT) {
				pseudo_debug(PDBGF_CLIENT, "local state dir doesn't exist, trying to create\n");
				mkdir_p(pseudo_path);
				pseudo_localstate_dir_fd = open(pseudo_path, O_RDONLY);
			}
			pseudo_localstate_dir_fd = pseudo_fd(pseudo_localstate_dir_fd, COPY_FD);
			free(pseudo_path);
		} else {
			pseudo_diag("No prefix available to to find server.\n");
			exit(1);
		}
		if (pseudo_localstate_dir_fd == -1) {
			pseudo_diag("Can't open local state path (%s) for server. (%s)\n",
				pseudo_localstatedir_path(NULL),
				strerror(errno));
			exit(1);
		}
	}
	if (client_connect()) {
		pseudo_debug(PDBGF_INVOKE, "Pseudo server seems to be already offline.\n");
		return 0;
	}
	memset(&msg, 0, sizeof(pseudo_msg_t));
	msg.type = PSEUDO_MSG_SHUTDOWN;
	msg.op = OP_NONE;
	msg.client = getpid();
	pseudo_debug(PDBGF_CLIENT | PDBGF_SERVER, "sending shutdown request\n");
	if (pseudo_msg_send(connect_fd, &msg, 0, NULL)) {
		pseudo_debug(PDBGF_CLIENT | PDBGF_SERVER, "error requesting shutdown: %s\n", strerror(errno));
		return 1;
	}
	ack = pseudo_msg_receive(connect_fd);
	if (!ack) {
		pseudo_diag("server did not respond to shutdown query.\n");
		return 1;
	}
	if (ack->type != PSEUDO_MSG_ACK) {
		pseudo_diag("Server refused shutdown.  Remaining client fds: %d\n", ack->fd);
		pseudo_diag("Client pids: %s\n", ack->path);
		pseudo_diag("Server will shut down after all clients exit.\n");
	}
	if (wait_on_socket) {
		/* try to receive a message the server won't send;
		 * this should abort/error-out when the server actually
		 * shuts down. */
		ack = pseudo_msg_receive(connect_fd);
	}
	return 0;
}

static char *
base_path(int dirfd, const char *path, int leave_last) {
	char *basepath = 0;
	size_t baselen = 0;
	size_t minlen = 0;
	char *newpath;

	if (!path)
		return NULL;
	if (!*path)
		return "";

	if (path[0] != '/') {
		if (dirfd != -1 && dirfd != AT_FDCWD) {
			if (dirfd >= 0) {
				basepath = fd_path(dirfd);
			}
			if (basepath) {
				baselen = strlen(basepath);
			} else {
				pseudo_diag("got *at() syscall for unknown directory, fd %d\n", dirfd);
			}
		} else {
			basepath = pseudo_cwd;
			baselen = pseudo_cwd_len;
		}
		if (!basepath) {
			pseudo_diag("unknown base path for fd %d, path %s\n", dirfd, path);
			return 0;
		}
		/* if there's a chroot path, and it's the start of basepath,
		 * flag it for pseudo_fix_path
		 */
		if (pseudo_chroot_len && baselen >= pseudo_chroot_len &&
			!memcmp(basepath, pseudo_chroot, pseudo_chroot_len) &&
			(basepath[pseudo_chroot_len] == '\0' || basepath[pseudo_chroot_len] == '/')) {

			minlen = pseudo_chroot_len;
		}
	} else if (pseudo_chroot_len) {
		/* "absolute" is really relative to chroot path */
		basepath = pseudo_chroot;
		baselen = pseudo_chroot_len;
		minlen = pseudo_chroot_len;
	}

	newpath = pseudo_fix_path(basepath, path, minlen, baselen, NULL, leave_last);
	pseudo_debug(PDBGF_PATH, "base_path[%s]: %s</>%s => %s\n",
		leave_last ? "nofollow" : "follow",
		basepath ? basepath : "<nil>",
		path ? path : "<nil>",
		newpath ? newpath : "<nil>");
	return newpath;
}

pseudo_msg_t *
pseudo_client_op(pseudo_op_t op, int access, int fd, int dirfd, const char *path, const PSEUDO_STATBUF *buf, ...) {
	pseudo_msg_t *result = 0;
	pseudo_msg_t msg = { .type = PSEUDO_MSG_OP };
	size_t pathlen = -1;
	int do_request = 0;
	char *path_extra_1 = 0;
	size_t path_extra_1len = 0;
	char *path_extra_2 = 0;
	size_t path_extra_2len = 0;
	static char *alloced_path = 0;
	static size_t alloced_len = 0;
	int strip_slash;

#ifdef PSEUDO_PROFILING
	struct timeval tv1_op, tv2_op;

	gettimeofday(&tv1_op, NULL);
	++profile_data.total_ops;
#endif
	/* disable wrappers */
	pseudo_antimagic();

	/* note: I am not entirely sure this should happen even if no
	 * messages have actually been sent. */
	if (!sent_messages) {
		sent_messages = 1;
		atexit(void_client_ping);
	}

	/* if path isn't available, try to find one? */
	if (!path && fd >= 0 && fd <= nfds) {
		path = fd_path(fd);
		if (!path) {
			pathlen = 0;
		} else {
			pathlen = strlen(path) + 1;
		}
	}

#ifdef PSEUDO_XATTRDB
	if (buf) {
		struct stat64 bufcopy = *buf;
		int do_save = 0;
		/* maybe use xattr instead */
		/* note: if we use xattr, logging won't work reliably
		 * because the server won't get messages if these work.
		 */
		switch (op) {
		case OP_CHMOD:
		case OP_FCHMOD:
		case OP_CHOWN:
		case OP_FCHOWN:
			/* for these, we want to start with the existing db
			 * values.
			 */
			bufcopy = *buf;
			result = pseudo_xattrdb_load(fd, path, buf);
			if (result && result->result == RESULT_SUCCEED) {
				pseudo_debug(PDBGF_XATTR, "merging existing values for xattr\n");
				switch (op) {
				case OP_CHMOD:
				case OP_FCHMOD:
					bufcopy.st_uid = result->uid;
					bufcopy.st_gid = result->gid;
					break;
				case OP_CHOWN:
				case OP_FCHOWN:
					bufcopy.st_rdev = result->rdev;
					bufcopy.st_mode = result->mode;
					break;
				default:
					break;
				}
				
			} else {
				switch (op) {
				case OP_CHMOD:
				case OP_FCHMOD:
					bufcopy.st_uid = pseudo_fuid;
					bufcopy.st_gid = pseudo_fgid;
					break;
				default:
					break;
				}
			}
			result = NULL;
			do_save = 1;
			break;
		case OP_CREAT:
		case OP_MKDIR:
		case OP_MKNOD:
			bufcopy.st_uid = pseudo_fuid;
			bufcopy.st_gid = pseudo_fgid;
			do_save = 1;
			break;
		case OP_LINK:
			do_save = 1;
			break;
		case OP_FSTAT:
		case OP_STAT:
			result = pseudo_xattrdb_load(fd, path, buf);
			break;
		default:
			break;
		}
		if (do_save) {
			result = pseudo_xattrdb_save(fd, path, &bufcopy);
		}
		if (result)
			goto skip_path;
	}
#endif

	if (op == OP_RENAME) {
		va_list ap;
		if (!path) {
			pseudo_diag("rename (%s) without new path.\n",
				path ? path : "<nil>");
			pseudo_magic();
			return 0;
		}
		va_start(ap, buf);
		path_extra_1 = va_arg(ap, char *);
		va_end(ap);
		/* last argument is the previous path of the file */
		if (!path_extra_1) {
			pseudo_diag("rename (%s) without old path.\n",
				path ? path : "<nil>");
			pseudo_magic();
			return 0;
		}
		path_extra_1len = strlen(path_extra_1);
		pseudo_debug(PDBGF_PATH | PDBGF_FILE, "rename: %s -> %s\n",
			path_extra_1, path);
	}

	/* we treat the "create" and "replace" flags as logically
	 * distinct operations, because they can fail when set can't.
	 */
	if (op == OP_SET_XATTR || op == OP_CREATE_XATTR || op == OP_REPLACE_XATTR) {
		va_list ap;
		va_start(ap, buf);
		path_extra_1 = va_arg(ap, char *);
		path_extra_1len = strlen(path_extra_1);
		path_extra_2 = va_arg(ap, char *);
		path_extra_2len = va_arg(ap, size_t);
		va_end(ap);
		pseudo_debug(PDBGF_XATTR, "setxattr, name '%s', value %d bytes\n",
			path_extra_1, (int) path_extra_2len);
		pseudo_debug_call(PDBGF_XATTR | PDBGF_VERBOSE, pseudo_dump_data, "xattr value", path_extra_2, path_extra_2len);
	}
	if (op == OP_GET_XATTR || op == OP_REMOVE_XATTR) {
		va_list ap;
		va_start(ap, buf);
		path_extra_1 = va_arg(ap, char *);
		path_extra_1len = strlen(path_extra_1);
		va_end(ap);
		pseudo_debug(PDBGF_XATTR, "%sxattr, name '%s'\n",
			op == OP_GET_XATTR ? "get" : "remove", path_extra_1);
	}

	if (path) {
		if (pathlen == (size_t) -1) {
			pathlen = strlen(path) + 1;
		}
		/* path fixup has to happen in the specific functions,
		 * because they may have to make calls which have to be
		 * fixed up for chroot stuff already.
		 * ... but wait!  / in chroot should not have that
		 * trailing /.
		 * (no attempt is made to handle a rename of "/" occurring
		 * in a chroot...)
		 */
		strip_slash = (pathlen > 2 && (path[pathlen - 2]) == '/');
	} else {
		path = "";
		pathlen = 0;
		strip_slash = 0;
	}

	/* f*xattr operations can result in needing to send a path
	 * value even though we don't have one available. We use an
	 * empty path for that.
	 */
	if (path_extra_1) {
		size_t full_len = path_extra_1len + 1 + pathlen - strip_slash;
		size_t partial_len = pathlen - 1 - strip_slash;
		if (path_extra_2) {
			full_len += path_extra_2len + 1;
		}
		if (full_len > alloced_len) {
			free(alloced_path);
			alloced_path = malloc(full_len);
			alloced_len = full_len;
			if (!alloced_path) {
				pseudo_diag("Can't allocate space for paths for a rename operation.  Sorry.\n");
				alloced_len = 0;
				pseudo_magic();
				return 0;
			}
		}
		memcpy(alloced_path, path, partial_len);
		alloced_path[partial_len] = '\0';
		memcpy(alloced_path + partial_len + 1, path_extra_1, path_extra_1len);
		alloced_path[partial_len + path_extra_1len + 1] = '\0';
		if (path_extra_2) {
			memcpy(alloced_path + partial_len + path_extra_1len + 2, path_extra_2, path_extra_2len);
		}
		alloced_path[full_len - 1] = '\0';
		path = alloced_path;
		pathlen = full_len;
		pseudo_debug_call(PDBGF_IPC | PDBGF_VERBOSE, pseudo_dump_data, "combined path buffer", path, pathlen);
	} else {
		if (strip_slash) {
			if (pathlen > alloced_len) {
				free(alloced_path);
				alloced_path = malloc(pathlen);
				alloced_len = pathlen;
				if (!alloced_path) {
					pseudo_diag("Can't allocate space for paths for a rename operation.  Sorry.\n");
					alloced_len = 0;
					pseudo_magic();
					return 0;
				}
			}
			memcpy(alloced_path, path, pathlen);
			alloced_path[pathlen - 2] = '\0';
			path = alloced_path;
		}
	}

#ifdef PSEUDO_XATTRDB
	/* If we were able to store things in xattr, we can easily skip
	 * most of the fancy path computations and such.
	 */
	skip_path:
#endif

	if (buf)
		pseudo_msg_stat(&msg, buf);

	if (pseudo_util_debug_flags & PDBGF_OP) {
		pseudo_debug(PDBGF_OP, "%s%s", pseudo_op_name(op),
			(dirfd != -1 && dirfd != AT_FDCWD && op != OP_DUP) ? "at" : "");
		if (path_extra_1) {
			pseudo_debug(PDBGF_OP, " %s ->", path_extra_1);
		}
		if (path) {
			pseudo_debug(PDBGF_OP, " %s", path);
		}
		/* for OP_RENAME in renameat, "fd" is also used for the
		 * second dirfd.
		 */
		if (fd != -1 && op != OP_RENAME) {
			pseudo_debug(PDBGF_OP, " [fd %d]", fd);
		}
		if (buf) {
			pseudo_debug(PDBGF_OP, " (+buf)");
			if (fd != -1) {
				pseudo_debug(PDBGF_OP, " [dev/ino: %d/%llu]",
					(int) buf->st_dev, (unsigned long long) buf->st_ino);
			}
			pseudo_debug(PDBGF_OP, " (0%o)", (int) buf->st_mode);
		}
		pseudo_debug(PDBGF_OP, ": ");
	}
	msg.type = PSEUDO_MSG_OP;
	msg.op = op;
	msg.fd = fd;
	msg.access = access;
	msg.result = RESULT_NONE;
	msg.client = getpid();

	/* do stuff */
	pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "processing request [ino %llu]\n", (unsigned long long) msg.ino);
	switch (msg.op) {
	case OP_CHDIR:
		pseudo_client_getcwd();
		do_request = 0;
		break;
	case OP_CHROOT:
		if (pseudo_client_chroot(path) == 0) {
			/* return a non-zero value to show non-failure */
			result = &msg;
		}
		do_request = 0;
		break;
	case OP_OPEN:
		pseudo_client_path(fd, path);
	case OP_EXEC: /* fallthrough */
		do_request = pseudo_client_logging;
		break;
	case OP_CLOSE:
		/* no request needed */
		if (fd >= 0) {
			if (fd == connect_fd) {
				connect_fd = pseudo_fd(connect_fd, COPY_FD);
				if (connect_fd == -1) {
					pseudo_diag("tried to close connection, couldn't dup: %s\n", strerror(errno));
				}
			} else if (fd == pseudo_util_debug_fd) {
				pseudo_util_debug_fd = pseudo_fd(fd, COPY_FD);
			} else if (fd == pseudo_prefix_dir_fd) {
				pseudo_prefix_dir_fd = pseudo_fd(fd, COPY_FD);
			} else if (fd == pseudo_localstate_dir_fd) {
				pseudo_localstate_dir_fd = pseudo_fd(fd, COPY_FD);
			} else if (fd == pseudo_pwd_fd) {
				pseudo_pwd_fd = pseudo_fd(fd, COPY_FD);
				/* since we have a FILE * on it, we close that... */
				fclose(pseudo_pwd);
				/* and open a new one on the copy */
				pseudo_pwd = fdopen(pseudo_pwd_fd, "r");
			} else if (fd == pseudo_grp_fd) {
				pseudo_grp_fd = pseudo_fd(fd, COPY_FD);
				/* since we have a FILE * on it, we close that... */
				fclose(pseudo_grp);
				/* and open a new one on the copy */
				pseudo_grp = fdopen(pseudo_grp_fd, "r");
			}
		}
		pseudo_client_close(fd);
		do_request = 0;
		break;
	case OP_DUP:
		/* just copy the path over */
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "dup: fd_path(%d) = %p [%s], dup to %d\n",
			fd, fd_path(fd), fd_path(fd) ? fd_path(fd) : "<nil>",
			dirfd);
		pseudo_client_path(dirfd, fd_path(fd));
		break;
	/* operations for which we should use the magic uid/gid */
	case OP_CHMOD:
	case OP_CREAT:
	case OP_FCHMOD:
	case OP_MKDIR:
	case OP_MKNOD:
	case OP_SYMLINK:
		msg.uid = pseudo_fuid;
		msg.gid = pseudo_fgid;
		pseudo_debug(PDBGF_OP, "fuid: %d ", pseudo_fuid);
		/* FALLTHROUGH */
		/* chown/fchown uid/gid already calculated, and
		 * a link or rename should not change a file's ownership.
		 * (operations which can create should be CREAT or MKNOD
		 * or MKDIR)
		 */
	case OP_CHOWN:
	case OP_FCHOWN:
	case OP_FSTAT:
	case OP_LINK:
	case OP_RENAME:
	case OP_STAT:
	case OP_UNLINK:
	case OP_DID_UNLINK:
	case OP_CANCEL_UNLINK:
	case OP_MAY_UNLINK:
	case OP_GET_XATTR:
	case OP_LIST_XATTR:
	case OP_SET_XATTR:
	case OP_REMOVE_XATTR:
		do_request = 1;
		break;
	default:
		pseudo_diag("error: unknown or unimplemented operator %d (%s)", op, pseudo_op_name(op));
		break;
	}
	/* result can only be set when PSEUDO_XATTRDB resulted in a
	 * successful store to or read from the local database.
	 */
	if (do_request && !result) {
#ifdef PSEUDO_PROFILING
		struct timeval tv1_ipc, tv2_ipc;
#endif
                if (!pseudo_op_wait(msg.op))
                        msg.type = PSEUDO_MSG_FASTOP;
		pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "sending request [ino %llu]\n", (unsigned long long) msg.ino);
#ifdef PSEUDO_PROFILING
		gettimeofday(&tv1_ipc, NULL);
#endif
		if (pseudo_local_only) {
			/* disable server */
			result = NULL;
		} else {
			result = pseudo_client_request(&msg, pathlen, path);
		}
#ifdef PSEUDO_PROFILING
		gettimeofday(&tv2_ipc, NULL);
		++profile_data.messages;
		profile_data.ipc_time.tv_sec += (tv2_ipc.tv_sec - tv1_ipc.tv_sec);
		profile_data.ipc_time.tv_usec += (tv2_ipc.tv_usec - tv1_ipc.tv_usec);
#endif
		if (result) {
			pseudo_debug(PDBGF_OP, "(%d) %s", getpid(), pseudo_res_name(result->result));
			if (op == OP_STAT || op == OP_FSTAT) {
				pseudo_debug(PDBGF_OP, " mode 0%o uid %d:%d",
					(int) result->mode,
					(int) result->uid,
					(int) result->gid);
			} else if (op == OP_CHMOD || op == OP_FCHMOD) {
				pseudo_debug(PDBGF_OP, " mode 0%o",
					(int) result->mode);
			} else if (op == OP_CHOWN || op == OP_FCHOWN) {
				pseudo_debug(PDBGF_OP, " uid %d:%d",
					(int) result->uid,
					(int) result->gid);
			}
		} else if (msg.type != PSEUDO_MSG_FASTOP) {
			pseudo_debug(PDBGF_OP, "(%d) no answer", getpid());
		}
	} else if (!result) {
		pseudo_debug(PDBGF_OP, "(%d) (no request)", getpid());
	}
	#ifdef PSEUDO_XATTRDB
	else {
		pseudo_debug(PDBGF_OP, "(%d) (handled through xattrdb: %s)", getpid(), pseudo_res_name(result->result));
	}
	#endif
	pseudo_debug(PDBGF_OP, "\n");

#ifdef PSEUDO_PROFILING
	gettimeofday(&tv2_op, NULL);
	profile_data.op_time.tv_sec += (tv2_op.tv_sec - tv1_op.tv_sec);
	profile_data.op_time.tv_usec += (tv2_op.tv_usec - tv1_op.tv_usec);
	if (profile_data.total_ops % profile_interval == 0) {
		pseudo_profile_report();
		if (profile_interval < 100) {
			profile_interval = profile_interval * 10;
		}
	}
#endif
	/* reenable wrappers */
	pseudo_magic();

	return result;
}

/* stuff for handling paths and execs */
static char *previous_path;
static char *previous_path_segs;
static char **path_segs;
static size_t *path_lens;

/* Makes an array of strings which are the path components
 * of previous_path.  Does this by duping the path, then replacing
 * colons with null terminators, and storing the addresses of the
 * starts of the substrings.
 */
static void
populate_path_segs(void) {
	size_t len = 0;
	char *s;
	int c = 0;

	free(path_segs);
	free(previous_path_segs);
	free(path_lens);
	path_segs = NULL;
	path_lens = NULL;
	previous_path_segs = NULL;

	if (!previous_path)
		return;

	for (s = previous_path; *s; ++s) {
		if (*s == ':')
			++c;
	}
	path_segs = malloc((c+2) * sizeof(*path_segs));
	if (!path_segs) {
		pseudo_diag("warning: failed to allocate space for %d path segments.\n",
			c);
		return;
	}
	path_lens = malloc((c + 2) * sizeof(*path_lens));
	if (!path_lens) {
		pseudo_diag("warning: failed to allocate space for %d path lengths.\n",
			c);
		free(path_segs);
		path_segs = 0;
		return;
	}
	previous_path_segs = strdup(previous_path);
	if (!previous_path_segs) {
		pseudo_diag("warning: failed to allocate space for path copy.\n");
		free(path_segs);
		path_segs = NULL;
		free(path_lens);
		path_lens = NULL;
		return;
	}
	c = 0;
	path_segs[c++] = previous_path;
	len = 0;
	for (s = previous_path; *s; ++s) {
		if (*s == ':') {
			*s = '\0';
			path_lens[c - 1] = len;
			len = 0;
			path_segs[c++] = s + 1;
		} else {
			++len;
		}
	}
	path_lens[c - 1] = len;
	path_segs[c] = NULL;
	path_lens[c] = 0;
}

const char *
pseudo_exec_path(const char *filename, int search_path) {
	char *path = getenv("PATH");
	char *candidate;
	int i;
	struct stat buf;

	if (!filename)
		return NULL;

	pseudo_antimagic();
	if (!path) {
		free(path_segs);
		free(previous_path);
		path_segs = 0;
		previous_path = 0;
	} else if (!previous_path || strcmp(previous_path, path)) {
		free(previous_path);
		previous_path = strdup(path);
		populate_path_segs();
	}

	/* absolute paths just get canonicalized. */
	if (*filename == '/') {
		candidate = pseudo_fix_path(NULL, filename, 0, 0, NULL, 0);
		pseudo_magic();
		return candidate;
	}

	if (!search_path || !path_segs) {
		candidate = pseudo_fix_path(pseudo_cwd, filename, 0, pseudo_cwd_len, NULL, 0);
		/* executable or not, it's the only thing we can try */
		pseudo_magic();
		return candidate;
	}

	for (i = 0; path_segs[i]; ++i) {
		path = path_segs[i];
		pseudo_debug(PDBGF_CLIENT, "exec_path: checking %s for %s\n", path, filename);
		if (!*path || (*path == '.' && path_lens[i] == 1)) {
			/* empty path or . is cwd */
			candidate = pseudo_fix_path(pseudo_cwd, filename, 0, pseudo_cwd_len, NULL, 0);
			pseudo_debug(PDBGF_CLIENT, "exec_path: in cwd, got %s\n", candidate);
		} else if (*path == '/') {
			candidate = pseudo_fix_path(path, filename, 0, path_lens[i], NULL, 0);
			pseudo_debug(PDBGF_CLIENT, "exec_path: got %s\n", candidate);
		} else {
			/* oh you jerk, making me do extra work */
			size_t len;
			char *dir = pseudo_fix_path(pseudo_cwd, path, 0, pseudo_cwd_len, &len, 0);
			if (dir) {
				candidate = pseudo_fix_path(dir, filename, 0, len, NULL, 0);
				pseudo_debug(PDBGF_CLIENT, "exec_path: got %s for non-absolute path\n", candidate);
			} else {
				pseudo_diag("couldn't allocate intermediate path.\n");
				candidate = NULL;
			}
		}
		if (candidate && !stat(candidate, &buf) && !S_ISDIR(buf.st_mode) && (buf.st_mode & 0111)) {
			pseudo_debug(PDBGF_CLIENT | PDBGF_VERBOSE, "exec_path: %s => %s\n", filename, candidate);
			pseudo_magic();
			return candidate;
		}
	}
	/* blind guess being as good as anything */
	pseudo_magic();
	return filename;
}

