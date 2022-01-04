#ifndef TRACE2_H
#define TRACE2_H

struct child_process;
struct repository;
struct json_writer;

/*
 * The public TRACE2 routines are grouped into the following groups:
 *
 * [] trace2_initialize -- initialization.
 * [] trace2_cmd_*      -- emit command/control messages.
 * [] trace2_child*     -- emit child start/stop messages.
 * [] trace2_exec*      -- emit exec start/stop messages.
 * [] trace2_thread*    -- emit thread start/stop messages.
 * [] trace2_def*       -- emit definition/parameter mesasges.
 * [] trace2_region*    -- emit region nesting messages.
 * [] trace2_data*      -- emit region/thread/repo data messages.
 * [] trace2_printf*    -- legacy trace[1] messages.
 */

/*
 * Initialize the TRACE2 clock and do nothing else, in particular
 * no mallocs, no system inspection, and no environment inspection.
 *
 * This should be called at the very top of main() to capture the
 * process start time.  This is intended to reduce chicken-n-egg
 * bootstrap pressure.
 *
 * It is safe to call this more than once.  This allows capturing
 * absolute startup costs on Windows which uses a little trickery
 * to do setup work before common-main.c:main() is called.
 *
 * The main trace2_initialize_fl() may be called a little later
 * after more infrastructure is established.
 */
void trace2_initialize_clock(void);

/*
 * Initialize TRACE2 tracing facility if any of the builtin TRACE2
 * targets are enabled in the system config or the environment.
 * Emits a 'version' event.
 *
 * Cleanup/Termination is handled automatically by a registered
 * atexit() routine.
 */
void trace2_initialize_fl(const char *file, int line);

#define trace2_initialize() trace2_initialize_fl(__FILE__, __LINE__)

/*
 * Return true if trace2 is enabled.
 */
int trace2_is_enabled(void);

/*
 * Emit a 'start' event with the original (unmodified) argv.
 */
void trace2_cmd_start_fl(const char *file, int line, const char **argv);

#define trace2_cmd_start(argv) trace2_cmd_start_fl(__FILE__, __LINE__, (argv))

/*
 * Emit an 'exit' event.
 *
 * Write the exit-code that will be passed to exit() or returned
 * from main().
 *
 * Use this prior to actually calling exit().
 * See "#define exit()" in git-compat-util.h
 */
int trace2_cmd_exit_fl(const char *file, int line, int code);

#define trace2_cmd_exit(code) (trace2_cmd_exit_fl(__FILE__, __LINE__, (code)))

/*
 * Emit an 'error' event.
 *
 * Write an error message to the TRACE2 targets.
 */
void trace2_cmd_error_va_fl(const char *file, int line, const char *fmt,
			    va_list ap);

#define trace2_cmd_error_va(fmt, ap) \
	trace2_cmd_error_va_fl(__FILE__, __LINE__, (fmt), (ap))

/*
 * Emit a 'pathname' event with the canonical pathname of the current process
 * This gives post-processors a simple field to identify the command without
 * having to parse the argv.  For example, to distinguish invocations from
 * installed versus debug executables.
 */
void trace2_cmd_path_fl(const char *file, int line, const char *pathname);

#define trace2_cmd_path(p) trace2_cmd_path_fl(__FILE__, __LINE__, (p))

/*
 * Emit a 'cmd_name' event with the canonical name of the command.
 * This gives post-processors a simple field to identify the command
 * without having to parse the argv.
 */
void trace2_cmd_name_fl(const char *file, int line, const char *name);

#define trace2_cmd_name(v) trace2_cmd_name_fl(__FILE__, __LINE__, (v))

/*
 * Emit a 'cmd_mode' event to further describe the command being run.
 * For example, "checkout" can checkout a single file or can checkout a
 * different branch.  This gives post-processors a simple field to compare
 * equivalent commands without having to parse the argv.
 */
void trace2_cmd_mode_fl(const char *file, int line, const char *mode);

#define trace2_cmd_mode(sv) trace2_cmd_mode_fl(__FILE__, __LINE__, (sv))

/*
 * Emit an 'alias' expansion event.
 */
void trace2_cmd_alias_fl(const char *file, int line, const char *alias,
			 const char **argv);

#define trace2_cmd_alias(alias, argv) \
	trace2_cmd_alias_fl(__FILE__, __LINE__, (alias), (argv))

/*
 * Emit one or more 'def_param' events for "interesting" configuration
 * settings.
 *
 * Use the TR2_SYSENV_CFG_PARAM setting to register a comma-separated
 * list of patterns configured important.  For example:
 *     git config --system trace2.configParams 'core.*,remote.*.url'
 * or:
 *     GIT_TRACE2_CONFIG_PARAMS=core.*,remote.*.url"
 *
 * Note: this routine does a read-only iteration on the config data
 * (using read_early_config()), so it must not be called until enough
 * of the process environment has been established.  This includes the
 * location of the git and worktree directories, expansion of any "-c"
 * and "-C" command line options, and etc.
 */
void trace2_cmd_list_config_fl(const char *file, int line);

#define trace2_cmd_list_config() trace2_cmd_list_config_fl(__FILE__, __LINE__)

/*
 * Emit a "def_param" event for the given config key/value pair IF
 * we consider the key to be "interesting".
 *
 * Use this for new/updated config settings created/updated after
 * trace2_cmd_list_config() is called.
 */
void trace2_cmd_set_config_fl(const char *file, int line, const char *key,
			      const char *value);

#define trace2_cmd_set_config(k, v) \
	trace2_cmd_set_config_fl(__FILE__, __LINE__, (k), (v))

/*
 * Emit a 'child_start' event prior to spawning a child process.
 *
 * Before calling optionally set "cmd->trace2_child_class" to a string
 * describing the type of the child process.  For example, "editor" or
 * "pager".
 */
void trace2_child_start_fl(const char *file, int line,
			   struct child_process *cmd);

#define trace2_child_start(cmd) trace2_child_start_fl(__FILE__, __LINE__, (cmd))

/*
 * Emit a 'child_exit' event after the child process completes.
 */
void trace2_child_exit_fl(const char *file, int line, struct child_process *cmd,
			  int child_exit_code);

#define trace2_child_exit(cmd, code) \
	trace2_child_exit_fl(__FILE__, __LINE__, (cmd), (code))

/*
 * Emit an 'exec' event prior to calling one of exec(), execv(),
 * execvp(), and etc.  On Unix-derived systems, this will be the
 * last event emitted for the current process, unless the exec
 * fails.  On Windows, exec() behaves like 'child_start' and a
 * waitpid(), so additional events may be emitted.
 *
 * Returns the "exec_id".
 */
int trace2_exec_fl(const char *file, int line, const char *exe,
		   const char **argv);

#define trace2_exec(exe, argv) trace2_exec_fl(__FILE__, __LINE__, (exe), (argv))

/*
 * Emit an 'exec_result' when possible.  On Unix-derived systems,
 * this should be called after exec() returns (which only happens
 * when there is an error starting the new process).  On Windows,
 * this should be called after the waitpid().
 *
 * The "exec_id" should be the value returned from trace2_exec().
 */
void trace2_exec_result_fl(const char *file, int line, int exec_id, int code);

#define trace2_exec_result(id, code) \
	trace2_exec_result_fl(__FILE__, __LINE__, (id), (code))

/*
 * Emit a 'thread_start' event.  This must be called from inside the
 * thread-proc to set up the trace2 TLS data for the thread.
 *
 * Thread names should be descriptive, like "preload_index".
 * Thread names will be decorated with an instance number automatically.
 */
void trace2_thread_start_fl(const char *file, int line,
			    const char *thread_name);

#define trace2_thread_start(thread_name) \
	trace2_thread_start_fl(__FILE__, __LINE__, (thread_name))

/*
 * Emit a 'thread_exit' event.  This must be called from inside the
 * thread-proc to report thread-specific data and cleanup TLS data
 * for the thread.
 */
void trace2_thread_exit_fl(const char *file, int line);

#define trace2_thread_exit() trace2_thread_exit_fl(__FILE__, __LINE__)

/*
 * Emit a 'param' event.
 *
 * Write a "<param> = <value>" pair describing some aspect of the
 * run such as an important configuration setting or command line
 * option that significantly changes command behavior.
 */
void trace2_def_param_fl(const char *file, int line, const char *param,
			 const char *value);

#define trace2_def_param(param, value) \
	trace2_def_param_fl(__FILE__, __LINE__, (param), (value))

/*
 * Tell trace2 about a newly instantiated repo object and assign
 * a trace2-repo-id to be used in subsequent activity events.
 *
 * Emits a 'worktree' event for this repo instance.
 */
void trace2_def_repo_fl(const char *file, int line, struct repository *repo);

#define trace2_def_repo(repo) trace2_def_repo_fl(__FILE__, __LINE__, repo)

/*
 * Emit a 'region_enter' event for <category>.<label> with optional
 * repo-id and printf message.
 *
 * Enter a new nesting level on the current thread and remember the
 * current time.  This controls the indenting of all subsequent events
 * on this thread.
 */
void trace2_region_enter_fl(const char *file, int line, const char *category,
			    const char *label, const struct repository *repo, ...);

#define trace2_region_enter(category, label, repo) \
	trace2_region_enter_fl(__FILE__, __LINE__, (category), (label), (repo))

void trace2_region_enter_printf_va_fl(const char *file, int line,
				      const char *category, const char *label,
				      const struct repository *repo,
				      const char *fmt, va_list ap);

#define trace2_region_enter_printf_va(category, label, repo, fmt, ap)    \
	trace2_region_enter_printf_va_fl(__FILE__, __LINE__, (category), \
					 (label), (repo), (fmt), (ap))

void trace2_region_enter_printf_fl(const char *file, int line,
				   const char *category, const char *label,
				   const struct repository *repo,
				   const char *fmt, ...);

#ifdef HAVE_VARIADIC_MACROS
#define trace2_region_enter_printf(category, label, repo, ...)                 \
	trace2_region_enter_printf_fl(__FILE__, __LINE__, (category), (label), \
				      (repo), __VA_ARGS__)
#else
/* clang-format off */
__attribute__((format (region_enter_printf, 4, 5)))
void trace2_region_enter_printf(const char *category, const char *label,
				const struct repository *repo, const char *fmt,
				...);
/* clang-format on */
#endif

/*
 * Emit a 'region_leave' event for <category>.<label> with optional
 * repo-id and printf message.
 *
 * Leave current nesting level and report the elapsed time spent
 * in this nesting level.
 */
void trace2_region_leave_fl(const char *file, int line, const char *category,
			    const char *label, const struct repository *repo, ...);

#define trace2_region_leave(category, label, repo) \
	trace2_region_leave_fl(__FILE__, __LINE__, (category), (label), (repo))

void trace2_region_leave_printf_va_fl(const char *file, int line,
				      const char *category, const char *label,
				      const struct repository *repo,
				      const char *fmt, va_list ap);

#define trace2_region_leave_printf_va(category, label, repo, fmt, ap)    \
	trace2_region_leave_printf_va_fl(__FILE__, __LINE__, (category), \
					 (label), (repo), (fmt), (ap))

void trace2_region_leave_printf_fl(const char *file, int line,
				   const char *category, const char *label,
				   const struct repository *repo,
				   const char *fmt, ...);

#ifdef HAVE_VARIADIC_MACROS
#define trace2_region_leave_printf(category, label, repo, ...)                 \
	trace2_region_leave_printf_fl(__FILE__, __LINE__, (category), (label), \
				      (repo), __VA_ARGS__)
#else
/* clang-format off */
__attribute__((format (region_leave_printf, 4, 5)))
void trace2_region_leave_printf(const char *category, const char *label,
				const struct repository *repo, const char *fmt,
				...);
/* clang-format on */
#endif

/*
 * Emit a key-value pair 'data' event of the form <category>.<key> = <value>.
 * This event implicitly contains information about thread, nesting region,
 * and optional repo-id.
 *
 * On event-based TRACE2 targets, this generates a 'data' event suitable
 * for post-processing.  On printf-based TRACE2 targets, this is converted
 * into a fixed-format printf message.
 */
void trace2_data_string_fl(const char *file, int line, const char *category,
			   const struct repository *repo, const char *key,
			   const char *value);

#define trace2_data_string(category, repo, key, value)                       \
	trace2_data_string_fl(__FILE__, __LINE__, (category), (repo), (key), \
			      (value))

void trace2_data_intmax_fl(const char *file, int line, const char *category,
			   const struct repository *repo, const char *key,
			   intmax_t value);

#define trace2_data_intmax(category, repo, key, value)                       \
	trace2_data_intmax_fl(__FILE__, __LINE__, (category), (repo), (key), \
			      (value))

void trace2_data_json_fl(const char *file, int line, const char *category,
			 const struct repository *repo, const char *key,
			 const struct json_writer *jw);

#define trace2_data_json(category, repo, key, value)                       \
	trace2_data_json_fl(__FILE__, __LINE__, (category), (repo), (key), \
			    (value))

/*
 * Emit a 'printf' event.
 *
 * Write an arbitrary formatted message to the TRACE2 targets.  These
 * text messages should be considered as human-readable strings without
 * any formatting guidelines.  Post-processors may choose to ignore
 * them.
 */
void trace2_printf_va_fl(const char *file, int line, const char *fmt,
			 va_list ap);

#define trace2_printf_va(fmt, ap) \
	trace2_printf_va_fl(__FILE__, __LINE__, (fmt), (ap))

void trace2_printf_fl(const char *file, int line, const char *fmt, ...);

#ifdef HAVE_VARIADIC_MACROS
#define trace2_printf(...) trace2_printf_fl(__FILE__, __LINE__, __VA_ARGS__)
#else
/* clang-format off */
__attribute__((format (printf, 1, 2)))
void trace2_printf(const char *fmt, ...);
/* clang-format on */
#endif

/*
 * Optional platform-specific code to dump information about the
 * current and any parent process(es).  This is intended to allow
 * post-processors to know who spawned this git instance and anything
 * else that the platform may be able to tell us about the current process.
 */

enum trace2_process_info_reason {
	TRACE2_PROCESS_INFO_STARTUP,
	TRACE2_PROCESS_INFO_EXIT,
};

#if defined(GIT_WINDOWS_NATIVE)
void trace2_collect_process_info(enum trace2_process_info_reason reason);
#else
#define trace2_collect_process_info(reason) \
	do {                                \
	} while (0)
#endif

#endif /* TRACE2_H */
