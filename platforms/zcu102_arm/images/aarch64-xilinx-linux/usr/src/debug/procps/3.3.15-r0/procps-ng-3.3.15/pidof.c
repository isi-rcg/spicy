/*
 * pidof.c - Utility for listing pids of running processes
 *
 * Copyright (C) 2013  Jaromir Capik <jcapik@redhat.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include <stdio.h>
#include <getopt.h>
#include <limits.h>

#include "c.h"
#include "fileutils.h"
#include "nls.h"
#include "xalloc.h"
#include "proc/readproc.h"
#include "proc/version.h" /* procps_version */


#define grow_size(x) do { \
	if ((x) < 0 || (size_t)(x) >= INT_MAX / 5 / sizeof(struct el)) \
		xerrx(EXIT_FAILURE, _("integer overflow")); \
	(x) = (x) * 5 / 4 + 1024; \
} while (0)

#define safe_free(x)	if (x) { free(x); x=NULL; }


struct el {
	pid_t pid;
};

struct el *procs = NULL;
static int proc_count = 0;

struct el *omitted_procs = NULL;
static int omit_count = 0;

static char *program = NULL;

/* switch flags */
static int opt_single_shot    = 0;  /* -s */
static int opt_scripts_too    = 0;  /* -x */
static int opt_rootdir_check  = 0;  /* -c */

static char *pidof_root = NULL;

static int __attribute__ ((__noreturn__)) usage(int opt)
{
	int err = (opt == '?');
	FILE *fp = err ? stderr : stdout;

	fputs(USAGE_HEADER, fp);
	fprintf(fp, _(" %s [options] [program [...]]\n"), program_invocation_short_name);
	fputs(USAGE_OPTIONS, fp);
	fputs(_(" -s, --single-shot         return one PID only\n"), fp);
	fputs(_(" -c, --check-root          omit processes with different root\n"), fp);
	fputs(_(" -x                        also find shells running the named scripts\n"), fp);
	fputs(_(" -o, --omit-pid <PID,...>  omit processes with PID\n"), fp);
	fputs(_(" -S, --separator SEP       use SEP as separator put between PIDs"), fp);
	fputs(USAGE_SEPARATOR, fp);
	fputs(USAGE_HELP, fp);
	fputs(USAGE_VERSION, fp);
	fprintf(fp, USAGE_MAN_TAIL("pidof(1)"));

	exit(fp == stderr ? EXIT_FAILURE : EXIT_SUCCESS);
}


static int is_omitted (pid_t pid)
{
	int i;

	for (i = 0; i < omit_count; i++) {
		if (pid == omitted_procs[i].pid) return 1;
	}

	return 0;
}


static char *get_basename (char *filename)
{
	char *pos;
	char *result;

	pos = result = filename;
	while (*pos != '\0') {
		if (*(pos++) == '/') result = pos;
	}

	return result;
}


static char *pid_link (pid_t pid, const char *base_name)
{
	char link [PROCPATHLEN];
	char *result;
	ssize_t path_alloc_size;
	ssize_t len;

	snprintf(link, sizeof(link), "/proc/%d/%s", pid, base_name);

	len = path_alloc_size = 0;
	result = NULL;
	do {
		grow_size(path_alloc_size);
		result = xrealloc(result, path_alloc_size);

		if ((len = readlink(link, result, path_alloc_size)) < 0) {
			len = 0;
			break;
		}

	} while (len == path_alloc_size);

	result[len] = '\0';

	return result;
}


static void select_procs (void)
{
	PROCTAB *ptp;
	proc_t task;
	int match;
	static int size = 0;
	char *cmd_arg0, *cmd_arg0base;
	char *cmd_arg1, *cmd_arg1base;
	char *program_base;
	char *root_link;
	char *exe_link;
	char *exe_link_base;

	/* get the input base name */
	program_base = get_basename(program);

	ptp = openproc (PROC_FILLCOM | PROC_FILLSTAT);

	exe_link = root_link = NULL;
	memset(&task, 0, sizeof (task));
	while(readproc(ptp, &task)) {

		if (opt_rootdir_check) {
			/* get the /proc/<pid>/root symlink value */
			root_link = pid_link(task.XXXID, "root");
			match = !strcmp(pidof_root, root_link);
			safe_free(root_link);

			if (!match) {  /* root check failed */
				continue;
			}
		}

		if (!is_omitted(task.XXXID) && task.cmdline && *task.cmdline) {

			cmd_arg0 = *task.cmdline;

			/* processes starting with '-' are login shells */
			if (*cmd_arg0 == '-') {
				cmd_arg0++;
			}

			/* get the argv0 base name */
			cmd_arg0base = get_basename(cmd_arg0);

			/* get the /proc/<pid>/exe symlink value */
			exe_link = pid_link(task.XXXID, "exe");

			/* get the exe_link base name */
			exe_link_base = get_basename(exe_link);

			match = 0;

			if (!strcmp(program, cmd_arg0base) ||
			    !strcmp(program_base, cmd_arg0) ||
			    !strcmp(program, cmd_arg0) ||

			    !strcmp(program, exe_link_base) ||
			    !strcmp(program, exe_link))
			{
				match = 1;

			} else if (opt_scripts_too && *(task.cmdline+1)) {

				cmd_arg1 = *(task.cmdline+1);

				/* get the arg1 base name */
				cmd_arg1base = get_basename(cmd_arg1);

				/* if script, then task.cmd = argv1, otherwise task.cmd = argv0 */
				if (task.cmd &&
				    !strncmp(task.cmd, cmd_arg1base, strlen(task.cmd)) &&
				    (!strcmp(program, cmd_arg1base) ||
				    !strcmp(program_base, cmd_arg1) ||
				    !strcmp(program, cmd_arg1)))
				{
					match = 1;
				}
			}
            /* If there is a space in arg0 then process probably has
             * setproctitle so use the cmdline
             */
            if (!match && strchr(cmd_arg0, ' ')) {
                match = (strcmp(program, task.cmd)==0);
            }

			safe_free(exe_link);

			if (match) {
				if (proc_count == size) {
					grow_size(size);
					procs = xrealloc(procs, size * (sizeof *procs));
				}
				if (procs) {
					procs[proc_count++].pid = task.XXXID;
				} else {
					xerrx(EXIT_FAILURE, _("internal error"));
				}
			}

		}
	}

	closeproc (ptp);
}


static void add_to_omit_list (char *input_arg)
{
	static int omit_size = 0;

	char *omit_str;
	char *endptr;

	pid_t omit_pid;

	omit_str = NULL;
	omit_str = strtok(input_arg, ",;:");
	while (omit_str) {

		if (!strcmp(omit_str,"%PPID")) {  /* keeping this %PPID garbage for backward compatibility only */
			omit_pid = getppid();     /* ... as it can be replaced with $$ in common shells */
			endptr = omit_str + sizeof("%PPID") - 1;
		} else {
			omit_pid = strtoul(omit_str, &endptr, 10);
		}

		if (*endptr == '\0') {
			if (omit_count == omit_size) {
				grow_size(omit_size);
				omitted_procs = xrealloc(omitted_procs, omit_size * sizeof(*omitted_procs));
			}
			if (omitted_procs) {
				omitted_procs[omit_count++].pid = omit_pid;
			} else {
				xerrx(EXIT_FAILURE, _("internal error"));
			}
		} else {
			xwarnx(_("illegal omit pid value (%s)!\n"), omit_str);
		}

		omit_str = strtok(NULL, ",;:");
	}
}



int main (int argc, char **argv)
{
	int opt;
	signed int i;
	int found = 0;
	int first_pid = 1;

	const char *separator = " ";
	const char *opts = "scnxmo:S:?Vh";

	static const struct option longopts[] = {
		{"check-root", no_argument, NULL, 'c'},
		{"single-shot", no_argument, NULL, 's'},
		{"omit-pid", required_argument, NULL, 'o'},
		{"separator", required_argument, NULL, 's'},
		{"help", no_argument, NULL, 'h'},
		{"version", no_argument, NULL, 'V'},
		{NULL, 0, NULL, 0}
	};

#ifdef HAVE_PROGRAM_INVOCATION_NAME
	program_invocation_name = program_invocation_short_name;
#endif
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
	atexit (close_stdout);

	/* process command-line options */
	while ((opt = getopt_long (argc, argv, opts, longopts, NULL)) != -1) {
		switch (opt) {
		case 's':
			opt_single_shot = 1;
			break;
		case 'o':
			add_to_omit_list (optarg);
			break;
		case 'x':
			opt_scripts_too = 1;
			break;
		case 'c':
			if (geteuid() == 0) {
				opt_rootdir_check = 1;
				safe_free(pidof_root);
				pidof_root = pid_link(getpid(), "root");
			}
			break;
		case 'S':
			separator = optarg;
			break;
		case 'V':
			printf (PROCPS_NG_VERSION);
			exit (EXIT_SUCCESS);
		case 'h':
		case '?':
			usage (opt);
			break;
		/* compatibility-only switches */
		case 'n': /* avoiding stat(2) on NFS volumes doesn't make any sense anymore ... */
			  /* ... as this reworked solution does not use stat(2) at all */
		case 'm': /* omitting relatives with argv[0] & argv[1] matching the argv[0] & argv[1] ...*/
			  /* ... of explicitly omitted PIDs is too 'expensive' and as we don't know */
			  /* ... wheter it is still needed, we won't re-implement it unless ... */
			  /* ... somebody gives us a good reason to do so :) */
			break;
		}
	}

	/* main loop */
	while (argc - optind) {		/* for each program */

		program = argv[optind++];

		select_procs();	/* get the list of matching processes */

		if (proc_count) {

			found = 1;
			for (i = proc_count - 1; i >= 0; i--) {	/* and display their PIDs */
				if (first_pid) {
					first_pid = 0;
					printf ("%ld", (long) procs[i].pid);
				} else {
					printf ("%s%ld", separator, (long) procs[i].pid);
				}
				if (opt_single_shot) break;
			}

			proc_count = 0;
		}
	}

	/* final line feed */
	if (found) printf("\n");

	/* some cleaning */
	safe_free(procs);
	safe_free(omitted_procs);
	safe_free(pidof_root);

	return !found;
}
