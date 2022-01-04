/* Code to restore the iptables state, from file by iptables-save.
 * (C) 2000-2002 by Harald Welte <laforge@gnumonks.org>
 * based on previous code from Rusty Russell <rusty@linuxcare.com.au>
 *
 * This code is distributed under the terms of GNU GPL v2
 */

#include <getopt.h>
#include <errno.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "iptables.h"
#include "ip6tables.h"
#include "xshared.h"
#include "xtables.h"
#include "libiptc/libiptc.h"
#include "libiptc/libip6tc.h"
#include "iptables-multi.h"
#include "ip6tables-multi.h"

static int counters, verbose, noflush, wait;

static struct timeval wait_interval = {
	.tv_sec	= 1,
};

/* Keeping track of external matches and targets.  */
static const struct option options[] = {
	{.name = "counters",      .has_arg = 0, .val = 'c'},
	{.name = "verbose",       .has_arg = 0, .val = 'v'},
	{.name = "version",       .has_arg = 0, .val = 'V'},
	{.name = "test",          .has_arg = 0, .val = 't'},
	{.name = "help",          .has_arg = 0, .val = 'h'},
	{.name = "noflush",       .has_arg = 0, .val = 'n'},
	{.name = "modprobe",      .has_arg = 1, .val = 'M'},
	{.name = "table",         .has_arg = 1, .val = 'T'},
	{.name = "wait",          .has_arg = 2, .val = 'w'},
	{.name = "wait-interval", .has_arg = 2, .val = 'W'},
	{NULL},
};

static void print_usage(const char *name, const char *version)
{
	fprintf(stderr, "Usage: %s [-c] [-v] [-V] [-t] [-h] [-n] [-w secs] [-W usecs] [-T table] [-M command]\n"
			"	   [ --counters ]\n"
			"	   [ --verbose ]\n"
			"	   [ --version]\n"
			"	   [ --test ]\n"
			"	   [ --help ]\n"
			"	   [ --noflush ]\n"
			"	   [ --wait=<seconds>\n"
			"	   [ --wait-interval=<usecs>\n"
			"	   [ --table=<TABLE> ]\n"
			"	   [ --modprobe=<command> ]\n", name);
}

struct iptables_restore_cb {
	const struct xtc_ops *ops;

	int (*for_each_chain)(int (*fn)(const xt_chainlabel,
					int, struct xtc_handle *),
			      int verbose, int builtinstoo,
			      struct xtc_handle *handle);
	int (*flush_entries)(const xt_chainlabel, int, struct xtc_handle *);
	int (*delete_chain)(const xt_chainlabel, int, struct xtc_handle *);
	int (*do_command)(int argc, char *argv[], char **table,
			  struct xtc_handle **handle, bool restore);
};

static struct xtc_handle *
create_handle(struct iptables_restore_cb *cb, const char *tablename)
{
	struct xtc_handle *handle;

	handle = cb->ops->init(tablename);

	if (!handle) {
		/* try to insmod the module if iptc_init failed */
		xtables_load_ko(xtables_modprobe_program, false);
		handle = cb->ops->init(tablename);
	}

	if (!handle) {
		xtables_error(PARAMETER_PROBLEM, "%s: unable to initialize "
			"table '%s'\n", xt_params->program_name, tablename);
		exit(1);
	}
	return handle;
}

static int
ip46tables_restore_main(struct iptables_restore_cb *cb, int argc, char *argv[])
{
	struct xtc_handle *handle = NULL;
	char buffer[10240];
	int c, lock;
	char curtable[XT_TABLE_MAXNAMELEN + 1] = {};
	FILE *in;
	int in_table = 0, testing = 0;
	const char *tablename = NULL;

	line = 0;
	lock = XT_LOCK_NOT_ACQUIRED;

	while ((c = getopt_long(argc, argv, "bcvVthnwWM:T:", options, NULL)) != -1) {
		switch (c) {
			case 'b':
				fprintf(stderr, "-b/--binary option is not implemented\n");
				break;
			case 'c':
				counters = 1;
				break;
			case 'v':
				verbose = 1;
				break;
			case 'V':
				printf("%s v%s (legacy)\n",
				       xt_params->program_name,
				       xt_params->program_version);
				exit(0);
			case 't':
				testing = 1;
				break;
			case 'h':
				print_usage(xt_params->program_name,
					    IPTABLES_VERSION);
				exit(0);
			case 'n':
				noflush = 1;
				break;
			case 'w':
				wait = parse_wait_time(argc, argv);
				break;
			case 'W':
				parse_wait_interval(argc, argv, &wait_interval);
				break;
			case 'M':
				xtables_modprobe_program = optarg;
				break;
			case 'T':
				tablename = optarg;
				break;
			default:
				fprintf(stderr,
					"Try `%s -h' for more information.\n",
					xt_params->program_name);
				exit(1);
		}
	}

	if (optind == argc - 1) {
		in = fopen(argv[optind], "re");
		if (!in) {
			fprintf(stderr, "Can't open %s: %s\n", argv[optind],
				strerror(errno));
			exit(1);
		}
	}
	else if (optind < argc) {
		fprintf(stderr, "Unknown arguments found on commandline\n");
		exit(1);
	}
	else in = stdin;

	if (!wait_interval.tv_sec && !wait) {
		fprintf(stderr, "Option --wait-interval requires option --wait\n");
		exit(1);
	}

	/* Grab standard input. */
	while (fgets(buffer, sizeof(buffer), in)) {
		int ret = 0;

		line++;
		if (buffer[0] == '\n')
			continue;
		else if (buffer[0] == '#') {
			if (verbose)
				fputs(buffer, stdout);
			continue;
		} else if ((strcmp(buffer, "COMMIT\n") == 0) && (in_table)) {
			if (!testing) {
				DEBUGP("Calling commit\n");
				ret = cb->ops->commit(handle);
				cb->ops->free(handle);
				handle = NULL;
			} else {
				DEBUGP("Not calling commit, testing\n");
				ret = 1;
			}

			/* Done with the current table, release the lock. */
			if (lock >= 0) {
				xtables_unlock(lock);
				lock = XT_LOCK_NOT_ACQUIRED;
			}

			in_table = 0;
		} else if ((buffer[0] == '*') && (!in_table)) {
			/* Acquire a lock before we create a new table handle */
			lock = xtables_lock_or_exit(wait, &wait_interval);

			/* New table */
			char *table;

			table = strtok(buffer+1, " \t\n");
			DEBUGP("line %u, table '%s'\n", line, table);
			if (!table) {
				xtables_error(PARAMETER_PROBLEM,
					"%s: line %u table name invalid\n",
					xt_params->program_name, line);
				exit(1);
			}
			strncpy(curtable, table, XT_TABLE_MAXNAMELEN);
			curtable[XT_TABLE_MAXNAMELEN] = '\0';

			if (tablename && strcmp(tablename, table) != 0) {
				if (lock >= 0) {
					xtables_unlock(lock);
					lock = XT_LOCK_NOT_ACQUIRED;
				}
				continue;
			}
			if (handle)
				cb->ops->free(handle);

			handle = create_handle(cb, table);
			if (noflush == 0) {
				DEBUGP("Cleaning all chains of table '%s'\n",
					table);
				cb->for_each_chain(cb->flush_entries, verbose, 1,
						handle);

				DEBUGP("Deleting all user-defined chains "
				       "of table '%s'\n", table);
				cb->for_each_chain(cb->delete_chain, verbose, 0,
						handle);
			}

			ret = 1;
			in_table = 1;

		} else if ((buffer[0] == ':') && (in_table)) {
			/* New chain. */
			char *policy, *chain;

			chain = strtok(buffer+1, " \t\n");
			DEBUGP("line %u, chain '%s'\n", line, chain);
			if (!chain) {
				xtables_error(PARAMETER_PROBLEM,
					   "%s: line %u chain name invalid\n",
					   xt_params->program_name, line);
				exit(1);
			}

			if (strlen(chain) >= XT_EXTENSION_MAXNAMELEN)
				xtables_error(PARAMETER_PROBLEM,
					   "Invalid chain name `%s' "
					   "(%u chars max)",
					   chain, XT_EXTENSION_MAXNAMELEN - 1);

			if (cb->ops->builtin(chain, handle) <= 0) {
				if (noflush && cb->ops->is_chain(chain, handle)) {
					DEBUGP("Flushing existing user defined chain '%s'\n", chain);
					if (!cb->ops->flush_entries(chain, handle))
						xtables_error(PARAMETER_PROBLEM,
							   "error flushing chain "
							   "'%s':%s\n", chain,
							   strerror(errno));
				} else {
					DEBUGP("Creating new chain '%s'\n", chain);
					if (!cb->ops->create_chain(chain, handle))
						xtables_error(PARAMETER_PROBLEM,
							   "error creating chain "
							   "'%s':%s\n", chain,
							   strerror(errno));
				}
			}

			policy = strtok(NULL, " \t\n");
			DEBUGP("line %u, policy '%s'\n", line, policy);
			if (!policy) {
				xtables_error(PARAMETER_PROBLEM,
					   "%s: line %u policy invalid\n",
					   xt_params->program_name, line);
				exit(1);
			}

			if (strcmp(policy, "-") != 0) {
				struct xt_counters count = {};

				if (counters) {
					char *ctrs;
					ctrs = strtok(NULL, " \t\n");

					if (!ctrs || !parse_counters(ctrs, &count))
						xtables_error(PARAMETER_PROBLEM,
							  "invalid policy counters "
							  "for chain '%s'\n", chain);
				}

				DEBUGP("Setting policy of chain %s to %s\n",
					chain, policy);

				if (!cb->ops->set_policy(chain, policy, &count,
						     handle))
					xtables_error(OTHER_PROBLEM,
						"Can't set policy `%s'"
						" on `%s' line %u: %s\n",
						policy, chain, line,
						cb->ops->strerror(errno));
			}

			ret = 1;

		} else if (in_table) {
			int a;
			char *pcnt = NULL;
			char *bcnt = NULL;
			char *parsestart;

			if (buffer[0] == '[') {
				/* we have counters in our input */
				char *ptr = strchr(buffer, ']');

				if (!ptr)
					xtables_error(PARAMETER_PROBLEM,
						   "Bad line %u: need ]\n",
						   line);

				pcnt = strtok(buffer+1, ":");
				if (!pcnt)
					xtables_error(PARAMETER_PROBLEM,
						   "Bad line %u: need :\n",
						   line);

				bcnt = strtok(NULL, "]");
				if (!bcnt)
					xtables_error(PARAMETER_PROBLEM,
						   "Bad line %u: need ]\n",
						   line);

				/* start command parsing after counter */
				parsestart = ptr + 1;
			} else {
				/* start command parsing at start of line */
				parsestart = buffer;
			}

			add_argv(argv[0], 0);
			add_argv("-t", 0);
			add_argv(curtable, 0);

			if (counters && pcnt && bcnt) {
				add_argv("--set-counters", 0);
				add_argv((char *) pcnt, 0);
				add_argv((char *) bcnt, 0);
			}

			add_param_to_argv(parsestart, line);

			DEBUGP("calling do_command(%u, argv, &%s, handle):\n",
				newargc, curtable);

			for (a = 0; a < newargc; a++)
				DEBUGP("argv[%u]: %s\n", a, newargv[a]);

			ret = cb->do_command(newargc, newargv,
					 &newargv[2], &handle, true);

			free_argv();
			fflush(stdout);
		}
		if (tablename && strcmp(tablename, curtable) != 0)
			continue;
		if (!ret) {
			fprintf(stderr, "%s: line %u failed\n",
					xt_params->program_name, line);
			exit(1);
		}
	}
	if (in_table) {
		fprintf(stderr, "%s: COMMIT expected at line %u\n",
				xt_params->program_name, line + 1);
		exit(1);
	}

	fclose(in);
	return 0;
}


#if defined ENABLE_IPV4
struct iptables_restore_cb ipt_restore_cb = {
	.ops		= &iptc_ops,
	.for_each_chain	= for_each_chain4,
	.flush_entries	= flush_entries4,
	.delete_chain	= delete_chain4,
	.do_command	= do_command4,
};

int
iptables_restore_main(int argc, char *argv[])
{
	int c;

	iptables_globals.program_name = "iptables-restore";
	c = xtables_init_all(&iptables_globals, NFPROTO_IPV4);
	if (c < 0) {
		fprintf(stderr, "%s/%s Failed to initialize xtables\n",
				iptables_globals.program_name,
				iptables_globals.program_version);
		exit(1);
	}
#if defined(ALL_INCLUSIVE) || defined(NO_SHARED_LIBS)
	init_extensions();
	init_extensions4();
#endif

	return ip46tables_restore_main(&ipt_restore_cb, argc, argv);
}
#endif

#if defined ENABLE_IPV6
struct iptables_restore_cb ip6t_restore_cb = {
	.ops		= &ip6tc_ops,
	.for_each_chain	= for_each_chain6,
	.flush_entries	= flush_entries6,
	.delete_chain	= delete_chain6,
	.do_command	= do_command6,
};

int
ip6tables_restore_main(int argc, char *argv[])
{
	int c;

	ip6tables_globals.program_name = "ip6tables-restore";
	c = xtables_init_all(&ip6tables_globals, NFPROTO_IPV6);
	if (c < 0) {
		fprintf(stderr, "%s/%s Failed to initialize xtables\n",
				ip6tables_globals.program_name,
				ip6tables_globals.program_version);
		exit(1);
	}
#if defined(ALL_INCLUSIVE) || defined(NO_SHARED_LIBS)
	init_extensions();
	init_extensions6();
#endif

	return ip46tables_restore_main(&ip6t_restore_cb, argc, argv);
}
#endif
