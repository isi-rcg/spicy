#include "builtin.h"
#include "config.h"
#include "parse-options.h"

static char const * const env__helper_usage[] = {
	N_("git env--helper --type=[bool|ulong] <options> <env-var>"),
	NULL
};

static enum {
	ENV_HELPER_TYPE_BOOL = 1,
	ENV_HELPER_TYPE_ULONG
} cmdmode = 0;

static int option_parse_type(const struct option *opt, const char *arg,
			     int unset)
{
	if (!strcmp(arg, "bool"))
		cmdmode = ENV_HELPER_TYPE_BOOL;
	else if (!strcmp(arg, "ulong"))
		cmdmode = ENV_HELPER_TYPE_ULONG;
	else
		die(_("unrecognized --type argument, %s"), arg);

	return 0;
}

int cmd_env__helper(int argc, const char **argv, const char *prefix)
{
	int exit_code = 0;
	const char *env_variable = NULL;
	const char *env_default = NULL;
	int ret;
	int ret_int, default_int;
	unsigned long ret_ulong, default_ulong;
	struct option opts[] = {
		OPT_CALLBACK_F(0, "type", &cmdmode, N_("type"),
			       N_("value is given this type"), PARSE_OPT_NONEG,
			       option_parse_type),
		OPT_STRING(0, "default", &env_default, N_("value"),
			   N_("default for git_env_*(...) to fall back on")),
		OPT_BOOL(0, "exit-code", &exit_code,
			 N_("be quiet only use git_env_*() value as exit code")),
		OPT_END(),
	};

	argc = parse_options(argc, argv, prefix, opts, env__helper_usage,
			     PARSE_OPT_KEEP_UNKNOWN);
	if (env_default && !*env_default)
		usage_with_options(env__helper_usage, opts);
	if (!cmdmode)
		usage_with_options(env__helper_usage, opts);
	if (argc != 1)
		usage_with_options(env__helper_usage, opts);
	env_variable = argv[0];

	switch (cmdmode) {
	case ENV_HELPER_TYPE_BOOL:
		if (env_default) {
			default_int = git_parse_maybe_bool(env_default);
			if (default_int == -1) {
				error(_("option `--default' expects a boolean value with `--type=bool`, not `%s`"),
				      env_default);
				usage_with_options(env__helper_usage, opts);
			}
		} else {
			default_int = 0;
		}
		ret_int = git_env_bool(env_variable, default_int);
		if (!exit_code)
			puts(ret_int ? "true" : "false");
		ret = ret_int;
		break;
	case ENV_HELPER_TYPE_ULONG:
		if (env_default) {
			if (!git_parse_ulong(env_default, &default_ulong)) {
				error(_("option `--default' expects an unsigned long value with `--type=ulong`, not `%s`"),
				      env_default);
				usage_with_options(env__helper_usage, opts);
			}
		} else {
			default_ulong = 0;
		}
		ret_ulong = git_env_ulong(env_variable, default_ulong);
		if (!exit_code)
			printf("%lu\n", ret_ulong);
		ret = ret_ulong;
		break;
	default:
		BUG("unknown <type> value");
		break;
	}

	return !ret;
}
