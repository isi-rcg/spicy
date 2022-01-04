/* -*- mode: C; c-file-style: "gnu"; indent-tabs-mode: nil; -*-
 *
 * Copyright (C) 2011,2013 Colin Walters <walters@verbum.org>
 * Copyright (C) 2009 Codethink Limited
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */


#include "config.h"

#include <gio/gio.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

#ifdef HAVE_SYSTEMD
#include <systemd/sd-journal.h>
#endif

#define TEST_SKIP_ECODE 77

#define TEST_RUNNING_STATUS_MSGID   "ed6199045dd38bb5321e551d9578f3d9"
#define TESTS_COMPLETE_MSGID   "4d013788dd704743b826436c951e551d"
#define ONE_TEST_FAILED_MSGID  "0eee66bf98514369bef9868327a43cf1"
#define ONE_TEST_SKIPPED_MSGID "ca0b037012363f1898466829ea163e7d"
#define ONE_TEST_SUCCESS_MSGID "142bf5d40e9742e99d3ac8c1ace83b36"
#define ONE_TEST_TIMED_OUT_MSGID  "db8f25eab14a4da68ef3ab3ce4b2c0bb"

/* Types of test_log() call */
typedef enum {
  TEST_LOG_RUNNING_STATUS,
  TEST_LOG_COMPLETE,
  TEST_LOG_ONE_FAILED,
  TEST_LOG_ONE_SKIPPED,
  TEST_LOG_ONE_SUCCESS,
  TEST_LOG_ONE_TIMED_OUT,
  TEST_LOG_EXCEPTION,
  TEST_LOG_ARBITRARY,
} TestLog;

/* Message IDs used for test_log() calls */
static const char * const test_log_message_ids[] = {
  [TEST_LOG_RUNNING_STATUS] = TEST_RUNNING_STATUS_MSGID,
  [TEST_LOG_COMPLETE] = TESTS_COMPLETE_MSGID,
  [TEST_LOG_ONE_FAILED] = ONE_TEST_FAILED_MSGID,
  [TEST_LOG_ONE_SKIPPED] = ONE_TEST_SKIPPED_MSGID,
  [TEST_LOG_ONE_SUCCESS] = ONE_TEST_SUCCESS_MSGID,
  [TEST_LOG_ONE_TIMED_OUT] = ONE_TEST_TIMED_OUT_MSGID,
  /* Reusing ONE_TEST_FAILED_MSGID is not quite right, but whatever */
  [TEST_LOG_EXCEPTION] = ONE_TEST_FAILED_MSGID,
  /* Special-cased: the "test name" is really the message ID */
  [TEST_LOG_ARBITRARY] = NULL,
};

static gboolean opt_quiet = FALSE;
static gboolean opt_tap = FALSE;

static void
test_log (TestLog what,
          const char *test_name,
          const char *format,
          ...)
{
  const char *msgid = test_log_message_ids[what];
  g_autofree char *message = NULL;
  va_list ap;

  if (what == TEST_LOG_ARBITRARY)
    {
      msgid = test_name;
      test_name = NULL;
    }

  va_start (ap, format);
  message = g_strdup_vprintf (format, ap);
  va_end (ap);

#ifdef HAVE_SYSTEMD
  if (test_name)
    sd_journal_send ("MESSAGE_ID=%s", msgid,
                     "GDTR_TEST=%s", test_name,
                     "MESSAGE=%s", message,
                     NULL);
  else
    sd_journal_send ("MESSAGE_ID=%s", msgid,
                     "MESSAGE=%s", message,
                     NULL);
#else
  /* we can't log this to the Journal, so do *something* with it */
  if (what == TEST_LOG_ARBITRARY)
    g_printerr ("%s: %s\n", msgid, message);
#endif

  if (opt_tap)
    {
      switch (what)
        {
          default:
            /* fall through */
          case TEST_LOG_RUNNING_STATUS:
            /* fall through */
          case TEST_LOG_COMPLETE:
            g_print ("# %s\n", message);
            break;

          case TEST_LOG_ONE_FAILED:
            g_print ("# %s\n", message);
            g_print ("not ok - %s\n", test_name);
            break;

          case TEST_LOG_ONE_SKIPPED:
            g_print ("ok # SKIP - %s\n", test_name);
            break;

          case TEST_LOG_ONE_SUCCESS:
            g_print ("ok - %s\n", test_name);
            break;

          case TEST_LOG_ONE_TIMED_OUT:
            g_print ("not ok - %s\n", message);
            break;

          case TEST_LOG_EXCEPTION:
            g_print ("Bail out! %s\n", message);
            break;

          case TEST_LOG_ARBITRARY:
            /* do nothing, just print to the Journal */
            break;
        }
    }
  else if (!opt_quiet)
    {
      if (what != TEST_LOG_ARBITRARY)
        g_print ("%s\n", message);
    }
}

/* Taken from gio/gunixfdlist.c */
static int
dup_close_on_exec_fd (gint     fd,
                      GError **error)
{
  gint new_fd;
  gint s;

#ifdef F_DUPFD_CLOEXEC
  do
    new_fd = fcntl (fd, F_DUPFD_CLOEXEC, 0l);
  while (new_fd < 0 && (errno == EINTR));

  if (new_fd >= 0)
    return new_fd;

  /* if that didn't work (new libc/old kernel?), try it the other way. */
#endif

  do
    new_fd = dup (fd);
  while (new_fd < 0 && (errno == EINTR));

  if (new_fd < 0)
    {
      int saved_errno = errno;

      g_set_error (error, G_IO_ERROR,
                   g_io_error_from_errno (saved_errno),
                   "dup: %s", g_strerror (saved_errno));

      return -1;
    }

  do
    {
      s = fcntl (new_fd, F_GETFD);

      if (s >= 0)
        s = fcntl (new_fd, F_SETFD, (long) (s | FD_CLOEXEC));
    }
  while (s < 0 && (errno == EINTR));

  if (s < 0)
    {
      int saved_errno = errno;

      g_set_error (error, G_IO_ERROR,
                   g_io_error_from_errno (saved_errno),
                   "fcntl: %s", g_strerror (saved_errno));
      close (new_fd);

      return -1;
    }

  return new_fd;
}

static gboolean
rm_rf (GFile *path, GError **error)
{
  int estatus;
  g_autofree char *pathstr = g_file_get_path (path);
  char *child_argv[] = { "rm", "-rf", pathstr, NULL };
  if (!g_spawn_sync (NULL, (char**)child_argv, NULL, G_SPAWN_SEARCH_PATH, NULL, NULL,
                     NULL, NULL, &estatus, error))
    return FALSE;
  if (!g_spawn_check_exit_status (estatus, error))
    return FALSE;
  return TRUE;
}

typedef struct {
  GHashTable *pending_tests;
  GError *test_error;

  GCancellable *cancellable;
  GPtrArray *tests;
  GPtrArray *failed_test_msgs;

  int parallel;
  int test_index;

  gboolean running_exclusive_test;
} TestRunnerApp;

typedef enum {
  TEST_STATE_UNLOADED,
  TEST_STATE_LOADED,
  TEST_STATE_EXECUTING,
  TEST_STATE_COMPLETE_SUCCESS,
  TEST_STATE_COMPLETE_SKIPPED,
  TEST_STATE_COMPLETE_FAILED,
} TestState;

typedef enum {
  TEST_TYPE_UNKNOWN,
  TEST_TYPE_SESSION,
  TEST_TYPE_SESSION_EXCLUSIVE
} TestType;

typedef struct
{
  GObject parent_instance;

  GFile *prefix_root;
  GFile *path;
  GFile *tmpdir;

  char *name;
  char **argv;
  char **envp;

  TestState state;
  TestType type;
  guint timeout;
} GdtrTest;

typedef struct _GdtrTestClass
{
  GObjectClass parent_class;
} GdtrTestClass;

static GType gdtr_test_get_type (void);
G_DEFINE_TYPE (GdtrTest, gdtr_test, G_TYPE_OBJECT);
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GdtrTest, g_object_unref);

static void
gdtr_test_finalize (GObject *gobject)
{
  GdtrTest *self = (GdtrTest*) gobject;
  g_strfreev (self->argv);
  g_strfreev (self->envp);
  g_clear_object (&self->tmpdir);
  g_object_unref (self->prefix_root);
  g_object_unref (self->path);

  G_OBJECT_CLASS (gdtr_test_parent_class)->finalize (gobject);
}

static void
gdtr_test_class_init (GdtrTestClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  gobject_class->finalize = gdtr_test_finalize;
}

static void
gdtr_test_init (GdtrTest *self)
{
}

static gboolean
load_test (GFile         *prefix_root,
           GFile         *path,
           GdtrTest         **out_test,
           GCancellable  *cancellable,
           GError       **error)
{
  gboolean ret = FALSE;
  GKeyFile *keyfile = NULL;
  g_autoptr(GdtrTest) test = NULL;
  int test_argc;
  g_autofree char *exec_key = NULL;
  g_autofree char *type_key = NULL;
  g_autofree char *test_path = NULL;
  char **env_key = NULL;
  GError *internal_error = NULL;

  test = g_object_new (gdtr_test_get_type (), NULL);

  g_assert (test->state == TEST_STATE_UNLOADED);

  test->prefix_root = g_object_ref (prefix_root);
  test->path = g_object_ref (path);

  test->name = g_file_get_relative_path (test->prefix_root, test->path);

  test_path = g_file_get_path (test->path);

  keyfile = g_key_file_new ();
  if (!g_key_file_load_from_file (keyfile, test_path, 0, error))
    goto out;

  exec_key = g_key_file_get_string (keyfile, "Test", "Exec", error);
  if (exec_key == NULL)
    goto out;

  if (!g_shell_parse_argv (exec_key, &test_argc, &test->argv, error))
    goto out;

  type_key = g_key_file_get_string (keyfile, "Test", "Type", error);
  if (type_key == NULL)
    goto out;
  if (strcmp (type_key, "session") == 0)
    test->type = TEST_TYPE_SESSION;
  else if (strcmp (type_key, "session-exclusive") == 0)
    test->type = TEST_TYPE_SESSION_EXCLUSIVE;
  else
    {
      g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                   "Unknown test type '%s'", type_key);
      goto out;
    }

  env_key = g_key_file_get_string_list (keyfile, "Test", "TestEnvironment", NULL, &internal_error);
  if (internal_error != NULL &&
      !(internal_error->domain == G_KEY_FILE_ERROR &&
        internal_error->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    goto out;
  else
    test->envp = env_key;

  test->state = TEST_STATE_LOADED;

  ret = TRUE;
  *out_test = test;
  test = NULL;
 out:
  if (!ret && test)
    g_prefix_error (error, "Test '%s': ", test->name);
  g_clear_error (&internal_error);
  g_clear_pointer (&keyfile, g_key_file_free);
  return ret;
}

static TestRunnerApp *app;

static gboolean opt_list;
static int opt_firstroot;
static int opt_parallel = 1;
static int opt_cancel_timeout = 5*60;
static char * opt_log_directory;
static char * opt_report_directory;
static char **opt_dirs;
static char *opt_status;
static char *opt_log_msgid;

static GOptionEntry options[] = {
  { "dir", 'd', 0, G_OPTION_ARG_STRING_ARRAY, &opt_dirs, "Only run tests from these dirs (default: all system data dirs)", NULL },
  { "list", 'l', 0, G_OPTION_ARG_NONE, &opt_list, "List matching tests", NULL },
  { "parallel", 'p', 0, G_OPTION_ARG_INT, &opt_parallel, "Specify parallelization to PROC processors; 0 will be dynamic)", "PROC" },
  { "first-root", 0, 0, G_OPTION_ARG_NONE, &opt_firstroot, "Only use first entry in XDG_DATA_DIRS", "PROC" },
  { "log-directory", 'L', 0, G_OPTION_ARG_FILENAME, &opt_log_directory, "Create a subdirectory with test logs", "DIR" },
  { "report-directory", 0, 0, G_OPTION_ARG_FILENAME, &opt_report_directory, "Create a subdirectory per failing test in DIR", "DIR" },
  { "status", 0, 0, G_OPTION_ARG_STRING, &opt_status, "Output status information", "yes/no/auto" },
  { "log-msgid", 0, 0, G_OPTION_ARG_STRING, &opt_log_msgid, "Log unique message with id MSGID=MESSAGE", "MSGID" },
  { "timeout", 't', 0, G_OPTION_ARG_INT, &opt_cancel_timeout, "Cancel test after timeout seconds; defaults to 5 minutes", "TIMEOUT" },
  { "quiet", 0, 0, G_OPTION_ARG_NONE, &opt_quiet, "Don't output test results", NULL },
  { "tap", 0, 0, G_OPTION_ARG_NONE, &opt_tap, "Output test results as TAP", NULL },
  { NULL }
};

static void
run_test_async (GdtrTest                *test,
                GCancellable        *cancellable,
                GAsyncReadyCallback  callback,
                gpointer             user_data);
static void
on_test_run_complete (GObject      *object,
                      GAsyncResult *result,
                      gpointer      user_data);

static gboolean
gather_all_tests_recurse (GFile         *prefix_root,
                          GFile         *dir,
                          const char    *prefix,
                          GPtrArray     *tests,
                          GCancellable  *cancellable,
                          GError       **error)
{
  gboolean ret = FALSE;
  g_autoptr(GFileEnumerator) dir_enum = NULL;
  g_autoptr(GFileInfo) info = NULL;
  g_autofree char *suite_name = NULL;
  g_autofree char *suite_prefix = NULL;
  GError *tmp_error = NULL;

  suite_name = g_file_get_basename (dir);
  suite_prefix = g_strconcat (prefix, suite_name, "/", NULL);
  
  dir_enum = g_file_enumerate_children (dir, "standard::name,standard::type", 0,
                                        cancellable, error);
  if (!dir_enum)
    goto out;
  while ((info = g_file_enumerator_next_file (dir_enum, cancellable, &tmp_error)) != NULL)
    {
      GFileType type = g_file_info_get_file_type (info);
      const char *name = g_file_info_get_name (info);
      g_autoptr(GFile) child = g_file_get_child (dir, name);

      if (type == G_FILE_TYPE_REGULAR && g_str_has_suffix (name, ".test"))
        {
          GdtrTest *test;
          if (!load_test (prefix_root, child, &test, cancellable, error))
            goto out;
          g_ptr_array_add (tests, test);
        }
      else if (type == G_FILE_TYPE_DIRECTORY)
        {
          if (!gather_all_tests_recurse (prefix_root, child, suite_prefix, tests,
                                         cancellable, error))
            goto out;
        }
      g_clear_object (&info);
    }
  if (tmp_error != NULL)
    {
      g_propagate_error (error, tmp_error);
      goto out;
    }

  ret = TRUE;
 out:
  return ret;
}

static void
log_test_completion (GdtrTest *test,
                     const char *reason)
{
  const char *msgid_value;

  if (test->state == TEST_STATE_COMPLETE_SUCCESS)
    {
      test_log (TEST_LOG_ONE_SUCCESS, test->name, "PASS: %s", test->name);
    }
  else if (test->state == TEST_STATE_COMPLETE_FAILED)
    {
      g_autofree char *msg = g_strconcat ("FAIL: ", test->name, " (", reason,
                                          ")", NULL);

      test_log (TEST_LOG_ONE_FAILED, test->name, "%s", msg);
      g_ptr_array_add (app->failed_test_msgs, g_strdup (msg));
    }
  else if (test->state == TEST_STATE_COMPLETE_SKIPPED)
    {
      test_log (TEST_LOG_ONE_SKIPPED, test->name, "SKIP: %s", test->name);
    }
  else
    g_assert_not_reached ();
}

static void
on_test_exited (GObject       *obj,
                GAsyncResult  *result,
                gpointer       user_data)
{
  GError *local_error = NULL;
  GError **error = &local_error;
  GError *tmp_error = NULL;
  int estatus;
  GSubprocess *proc = G_SUBPROCESS (obj);
  GTask *task = G_TASK (user_data);
  GCancellable *cancellable = g_task_get_cancellable (task);
  GdtrTest *test;
  gboolean failed = FALSE;

  test = g_task_get_source_object (task);

  g_assert (test->state == TEST_STATE_EXECUTING);

  if (!g_subprocess_wait (proc, cancellable, error))
    goto out;
  estatus = g_subprocess_get_status (proc);
  if (!g_spawn_check_exit_status (estatus, &tmp_error))
    {
      if (g_error_matches (tmp_error, G_SPAWN_EXIT_ERROR, 77))
        {
          test->state = TEST_STATE_COMPLETE_SKIPPED;
          log_test_completion (test, NULL);
        }
      else
        {
          test->state = TEST_STATE_COMPLETE_FAILED;
          log_test_completion (test, tmp_error->message);
          failed = TRUE;
        }
      /* Individual test failures don't count as failure of the whole process */
      g_clear_error (&tmp_error);
    }
  else
    {
      if (test->timeout) {
        g_source_remove (test->timeout);
        test->timeout = 0;
      }
      test->state = TEST_STATE_COMPLETE_SUCCESS;
      log_test_completion (test, NULL);
    }
  
  /* Keep around temporaries from failed tests */
  if (!(failed && opt_report_directory))
    {
      g_autoptr(GFile) test_tmpdir_stamp = g_file_get_child (test->tmpdir, ".testtmp");
      if (g_file_query_exists (test_tmpdir_stamp, NULL))
        {
          if (!rm_rf (test->tmpdir, error))
            goto out;
        }
    }

 out:
  if (local_error)
    g_task_return_error (task, local_error);
  else
    g_task_return_boolean (task, TRUE);
}

static gboolean
cancel_test (gpointer data)
{
  GSubprocess*proc = data;
  g_subprocess_force_exit (proc);
  test_log (TEST_LOG_ONE_TIMED_OUT, NULL, "Test timed out after %u seconds",
       opt_cancel_timeout);
  return FALSE;
}

static void
run_test_async (GdtrTest                *test,
                GCancellable        *cancellable,
                GAsyncReadyCallback  callback,
                gpointer             user_data)
{
  static gsize initialized;
  static GRegex *slash_regex;

  GError *local_error = NULL;
  GError **error = &local_error;
  g_autofree char *test_tmpdir = NULL;
  g_autofree char *test_squashed_name = NULL;
  g_autofree char *test_tmpname = NULL;
  g_autoptr(GSubprocessLauncher) proc_context = NULL;
  g_autoptr(GSubprocess) proc = NULL;
  GTask *task;

  g_assert (test->state == TEST_STATE_LOADED);

  if (g_once_init_enter (&initialized))
    {
      slash_regex = g_regex_new ("/", 0, 0, NULL);
      g_assert (slash_regex != NULL);
      g_once_init_leave (&initialized, 1);
    }
  
  task = g_task_new (test, cancellable, callback, user_data); 

  g_print ("%sRunning test: %s\n", opt_tap ? "# " : "", test->name);

  test_squashed_name = g_regex_replace_literal (slash_regex, test->name, -1,
                                                0, "_", 0, NULL);
  if (!opt_report_directory)
    {
      test_tmpname = g_strconcat ("test-tmp-", test_squashed_name, "-XXXXXX", NULL);
      test_tmpdir = g_dir_make_tmp (test_tmpname, error);
      if (!test_tmpdir)
        goto out;
      test->tmpdir = g_file_new_for_path (test_tmpdir);
    }
  else
    {
      test_tmpdir = g_build_filename (opt_report_directory, test_squashed_name, NULL);
      test->tmpdir = g_file_new_for_path (test_tmpdir);
      if (!rm_rf (test->tmpdir, error))
        goto out;
      if (g_mkdir_with_parents (test_tmpdir, 0755) < 0)
        {
          int errsv = errno;
          g_set_error (error, G_IO_ERROR, g_io_error_from_errno (errsv),
                       "%s", g_strerror (errsv));
          goto out;
        }
    }

  /* We create a .testtmp stamp file so that tests can *know* for sure
   * they're in a temporary directory.  This is used by at least the
   * OSTree tests as protection against someone running a test script
   * outside of the framework, as it might overwrite files in their
   * source directory, etc.
   *
   * Also, when we do the rm -rf, we test for the file to be doubly
   * sure that we're deleting the right tmpdir.
   */ 
  {
    g_autoptr(GFile) test_tmpdir_stamp = g_file_get_child (test->tmpdir, ".testtmp");

    if (!g_file_replace_contents (test_tmpdir_stamp, "", 0, NULL, FALSE, 0, NULL, cancellable, error))
      goto out;
  }

  GSubprocessFlags flags = G_SUBPROCESS_FLAGS_NONE;
  if (opt_report_directory || opt_log_directory)
    flags |= G_SUBPROCESS_FLAGS_STDERR_MERGE;
  proc_context = g_subprocess_launcher_new (flags);

  if (opt_tap && !(opt_report_directory || opt_log_directory))
    {
      /* We can't put the test's output on our stdout, or it'd be
       * misinterpreted as our structured TAP output. Put it on our
       * stderr instead */
      int copy_of_stderr;

      copy_of_stderr = dup_close_on_exec_fd (STDERR_FILENO, error);
      if (copy_of_stderr < 0)
        goto out;
      g_subprocess_launcher_take_stdout_fd (proc_context, copy_of_stderr);
    }

  g_subprocess_launcher_set_cwd (proc_context, test_tmpdir);
  g_subprocess_launcher_set_environ (proc_context, test->envp);
  if (opt_report_directory)
    {
      const char *test_output_filename = "output.txt";
      g_autofree char *test_output_path = g_build_filename (test_tmpdir, test_output_filename, NULL);
      g_subprocess_launcher_set_stdout_file_path (proc_context, test_output_path);
    }
  else if (opt_log_directory)
    {
      g_autofree char *test_output_path = g_strconcat (opt_log_directory, "/", test_squashed_name, ".txt", NULL);
      g_subprocess_launcher_set_stdout_file_path (proc_context, test_output_path);
    }

  proc = g_subprocess_launcher_spawnv (proc_context, (const char *const*)test->argv, error);
  if (!proc)
    goto out;

  test->state = TEST_STATE_EXECUTING;

  g_subprocess_wait_async (proc, cancellable, on_test_exited, task);
  test->timeout = g_timeout_add_seconds (opt_cancel_timeout, cancel_test, g_object_ref (proc));

 out:
  if (local_error)
    {
      g_task_report_error (test->path, callback, user_data, run_test_async, local_error);
    }
}

static gboolean
run_test_async_finish (GdtrTest          *test,
                       GAsyncResult  *result,
                       GError       **error)
{
  g_return_val_if_fail (g_task_is_valid (result, test), FALSE);
  return g_task_propagate_boolean (G_TASK (result), error);
}

static void
reschedule_tests (GCancellable *cancellable)
{
  while (!app->running_exclusive_test
         && g_hash_table_size (app->pending_tests) < app->parallel
         && app->test_index < app->tests->len)
    {
      GdtrTest *test = app->tests->pdata[app->test_index];
      g_assert (test->type != TEST_TYPE_UNKNOWN);
      if (test->type == TEST_TYPE_SESSION_EXCLUSIVE)
        {
          /* If our next text is exclusive, wait until any other
           * pending async tests have run.
           */
          if (g_hash_table_size (app->pending_tests) > 0)
            break;
          app->running_exclusive_test = TRUE;
        }
      run_test_async (test, cancellable,
                      on_test_run_complete, NULL);
      g_hash_table_insert (app->pending_tests, test, test);
      app->test_index++;
    }
}

static void
on_test_run_complete (GObject      *object,
                      GAsyncResult *result,
                      gpointer      user_data)
{
  GError *local_error = NULL;
  GError **error = &local_error;
  GdtrTest *test = (GdtrTest*)object;

  if (!run_test_async_finish (test, result, error))
    goto out;

 out:
  if (local_error)
    {
      if (!app->test_error)
        app->test_error = g_error_copy (local_error);
      g_clear_error (&local_error);
    }
  else
    {
      gboolean removed = g_hash_table_remove (app->pending_tests, test);
      g_assert (removed);
      if (test->type == TEST_TYPE_SESSION_EXCLUSIVE)
        app->running_exclusive_test = FALSE;
      reschedule_tests (app->cancellable);
    }
}

static gboolean
idle_output_status (gpointer data)
{
  GHashTableIter iter;
  gpointer key, value;
  g_autoptr(GString) status_str = g_string_new ("Executing: ");
  gboolean first = TRUE;

  g_hash_table_iter_init (&iter, app->pending_tests);

  while (g_hash_table_iter_next (&iter, &key, &value))
    {
      GdtrTest *test = key;
      if (!first)
        g_string_append (status_str, ", ");
      else
        first = FALSE;
      g_string_append (status_str, test->name);
    }

  test_log (TEST_LOG_RUNNING_STATUS, NULL, "%s", status_str->str);
  return TRUE;
}

static gint
cmp_tests (gconstpointer adata,
           gconstpointer bdata)
{
  GdtrTest **a_pp = (gpointer)adata;
  GdtrTest **b_pp = (gpointer)bdata;
  GdtrTest *a = *a_pp;
  GdtrTest *b = *b_pp;

  if (a->type == b->type)
    {
      g_autofree char *apath = g_file_get_path (a->path);
      g_autofree char *bpath = g_file_get_path (b->path);
      return strcmp (apath, bpath);
    }
  else if (a->type < b->type)
    {
      return -1;
    }
  else
    {
      return 1;
    }
}

static void
fisher_yates_shuffle (GPtrArray *tests)
{
  guint m = tests->len;

  while (m > 0)
    {
      guint i = g_random_int_range (0, m);
      gpointer val;
      m--;
      val = tests->pdata[m];
      tests->pdata[m] = tests->pdata[i];
      tests->pdata[i] = val;
    }
}

static gint64
timeval_to_ms (const struct timeval *tv)
{
  if (tv->tv_sec == -1L &&
      tv->tv_usec == -1L)
    return -1;

  if (tv->tv_sec > (G_MAXUINT64 - tv->tv_usec) / G_USEC_PER_SEC)
    return -1;

  return ((gint64) tv->tv_sec) * G_USEC_PER_SEC + tv->tv_usec;
}

static double
timeval_to_secs (const struct timeval *tv)
{
  return ((double)timeval_to_ms (tv)) / G_USEC_PER_SEC;
}

int
main (int argc, char **argv)
{
  gboolean ret = FALSE;
  GCancellable *cancellable = NULL;
  GError *local_error = NULL;
  GError **error = &local_error;
  guint total_tests = 0;
  int i, j;
  GOptionContext *context;
  TestRunnerApp appstruct;
  const char *const *datadirs_iter;
  int n_passed, n_skipped, n_failed;

  memset (&appstruct, 0, sizeof (appstruct));
  app = &appstruct;

  /* avoid gvfs (http://bugzilla.gnome.org/show_bug.cgi?id=526454) */
  g_setenv ("GIO_USE_VFS", "local", TRUE);

  /* There's no point in logging to the Journal every time we log to
   * the Journal */
  if (g_log_writer_is_journald (STDOUT_FILENO))
    opt_quiet = TRUE;

  context = g_option_context_new ("[PREFIX...] - Run installed tests");
  g_option_context_add_main_entries (context, options, NULL);

  if (!g_option_context_parse (context, &argc, &argv, error))
    goto out;

  /* This is a hack to allow external gjs programs that don't link to
   * libgsystem to log to systemd.
   */
  if (opt_log_msgid)
    {
      const char *eq = strchr (opt_log_msgid, '=');
      g_autofree char *msgid = NULL;
      g_assert (eq);
      msgid = g_strndup (opt_log_msgid, eq - opt_log_msgid);

      test_log (TEST_LOG_ARBITRARY, msgid, "%s", eq + 1);
      exit (0);
    }

  if (opt_parallel == 0)
    app->parallel = g_get_num_processors ();
  else
    app->parallel = opt_parallel;

  app->pending_tests = g_hash_table_new (NULL, NULL);
  app->tests = g_ptr_array_new_with_free_func ((GDestroyNotify)g_object_unref);
  app->failed_test_msgs = g_ptr_array_new_with_free_func ((GDestroyNotify)g_free);

  if (opt_dirs)
    datadirs_iter = (const char *const*) opt_dirs;
  else
    datadirs_iter = g_get_system_data_dirs ();
  
  for (; *datadirs_iter; datadirs_iter++)
    {
      const char *datadir = *datadirs_iter;
      g_autoptr(GFile) datadir_f = g_file_new_for_path (datadir);
      g_autoptr(GFile) prefix_root = g_file_get_child (datadir_f, "installed-tests");

      if (!g_file_query_exists (prefix_root, NULL))
        continue;

      if (!gather_all_tests_recurse (prefix_root, prefix_root, "", app->tests,
                                     cancellable, error))
        goto out;

      if (opt_firstroot)
        break;
    }

  if (argc > 1)
    {
      j = 0;
      while (j < app->tests->len)
        {
          gboolean matches = FALSE;
          GdtrTest *test = app->tests->pdata[j];
          for (i = 1; i < argc; i++)
            {
              const char *prefix = argv[i];
              if (g_str_has_prefix (test->name, prefix))
                {
                  matches = TRUE;
                  break;
                }
            }
          if (!matches)
            g_ptr_array_remove_index_fast (app->tests, j);
          else
            j++;
        }
    }

  total_tests = app->tests->len;

  if (opt_list)
    {
      g_ptr_array_sort (app->tests, cmp_tests);
      for (i = 0; i < app->tests->len; i++)
        {
          GdtrTest *test = app->tests->pdata[i];
          g_autofree char *path = g_file_get_path (test->prefix_root);
          g_print ("%s (%s)\n", test->name, path);
        }
    }
  else
    {
      gboolean show_status;

      if (opt_tap)
        {
          if (total_tests == 0)
            g_print ("1..0 # SKIP - nothing to do\n");
          else
            g_print ("1..%d\n", total_tests);
        }

      fisher_yates_shuffle (app->tests);

      reschedule_tests (app->cancellable);

      if (opt_status == NULL || strcmp (opt_status, "auto") == 0)
        show_status = TRUE;
      else if (strcmp (opt_status, "no") == 0)
        show_status = FALSE;
      else if (strcmp (opt_status, "yes") == 0)
        show_status = TRUE;
      else
        {
          g_set_error (error, G_IO_ERROR, G_IO_ERROR_FAILED,
                       "Invalid --status='%s'", opt_status);
          goto out;
        }

      if (show_status)
        g_timeout_add_seconds (5, idle_output_status, app);

      while (g_hash_table_size (app->pending_tests) > 0 && !app->test_error)
        g_main_context_iteration (NULL, TRUE);

      if (app->test_error)
        {
          g_propagate_error (error, app->test_error);
          goto out;
        }
    }

  ret = TRUE;
 out:
  if (!ret)
    {
      g_assert (local_error);
      test_log (TEST_LOG_EXCEPTION, NULL,
                "Caught exception during testing: %s",
                local_error->message);
      g_clear_error (&local_error);
    }
  if (!opt_list)
    {
      struct rusage child_rusage;
      g_autofree char *rusage_str = NULL;

      n_passed = n_skipped = n_failed = 0;
      for (i = 0; i < app->tests->len; i++)
        {
          GdtrTest *test = app->tests->pdata[i];
          switch (test->state)
            {
            case TEST_STATE_COMPLETE_SUCCESS:
              n_passed++;
              break;
            case TEST_STATE_COMPLETE_SKIPPED:
              n_skipped++;
              break;
            case TEST_STATE_COMPLETE_FAILED:
              n_failed++;
              break;
            default:
              break;
            }
        }

      if (getrusage (RUSAGE_CHILDREN, &child_rusage) == 0)
        {
          rusage_str = g_strdup_printf ("; user=%0.1fs; system=%0.1fs; maxrss=%li",
                                        timeval_to_secs (&child_rusage.ru_utime),
                                        timeval_to_secs (&child_rusage.ru_stime),
                                        child_rusage.ru_maxrss);
        }

      test_log (TEST_LOG_COMPLETE, NULL,
                "SUMMARY%s: total=%u; passed=%d; skipped=%d; failed=%d%s",
                ret ? "" : " (incomplete)",
                total_tests, n_passed, n_skipped, n_failed,
                rusage_str != NULL ? rusage_str : "",
                NULL);

      for (i = 0; i < app->failed_test_msgs->len; i++)
        g_print ("%s%s\n", opt_tap ? "# " : "", (char *) app->failed_test_msgs->pdata[i]);
    }
  g_clear_pointer (&app->pending_tests, g_hash_table_unref);
  g_clear_pointer (&app->tests, g_ptr_array_unref);
  if (!ret)
    return 1;
  if (n_failed > 0)
    return 2;
  return 0;
}
