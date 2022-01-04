/*
 * which v2.x -- print full path of executables
 * Copyright (C) 1999, 2003, 2007, 2008  Carlo Wood <carlo@gnu.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "sys.h"
#include <stdio.h>
#include <ctype.h>
#include "getopt.h"
#include "tilde/tilde.h"
#include "bash.h"

static const char *progname;

static void print_usage(FILE *out)
{
  fprintf(out, "Usage: %s [options] [--] COMMAND [...]\n", progname);
  fprintf(out, "Write the full path of COMMAND(s) to standard output.\n\n");
  fprintf(out, "  --version, -[vV] Print version and exit successfully.\n");
  fprintf(out, "  --help,          Print this help and exit successfully.\n");
  fprintf(out, "  --skip-dot       Skip directories in PATH that start with a dot.\n");
  fprintf(out, "  --skip-tilde     Skip directories in PATH that start with a tilde.\n");
  fprintf(out, "  --show-dot       Don't expand a dot to current directory in output.\n");
  fprintf(out, "  --show-tilde     Output a tilde for HOME directory for non-root.\n");
  fprintf(out, "  --tty-only       Stop processing options on the right if not on tty.\n");
  fprintf(out, "  --all, -a        Print all matches in PATH, not just the first\n");
  fprintf(out, "  --read-alias, -i Read list of aliases from stdin.\n");
  fprintf(out, "  --skip-alias     Ignore option --read-alias; don't read stdin.\n");
  fprintf(out, "  --read-functions Read shell functions from stdin.\n");
  fprintf(out, "  --skip-functions Ignore option --read-functions; don't read stdin.\n\n");
  fprintf(out, "Recommended use is to write the output of (alias; declare -f) to standard\n");
  fprintf(out, "input, so that which can show aliases and shell functions. See which(1) for\n");
  fprintf(out, "examples.\n\n");
  fprintf(out, "If the options --read-alias and/or --read-functions are specified then the\n");
  fprintf(out, "output can be a full alias or function definition, optionally followed by\n");
  fprintf(out, "the full path of each command used inside of those.\n\n");
  fprintf(out, "Report bugs to <which-bugs@gnu.org>.\n");
}

static void print_version(void)
{
  fprintf(stdout, "GNU which v" VERSION ", Copyright (C) 1999 - 2015 Carlo Wood.\n");
  fprintf(stdout, "GNU which comes with ABSOLUTELY NO WARRANTY;\n");
  fprintf(stdout, "This program is free software; your freedom to use, change\n");
  fprintf(stdout, "and distribute this program is protected by the GPL.\n");
}

static void print_fail(const char *name, const char *path_list)
{
  fprintf(stderr, "%s: no %s in (%s)\n", progname, name, path_list);
}

static char home[256];
static size_t homelen = 0;

static int absolute_path_given;
static int found_path_starts_with_dot;
static char *abs_path;

static int skip_dot = 0, skip_tilde = 0, skip_alias = 0, read_alias = 0;
static int show_dot = 0, show_tilde = 0, show_all = 0, tty_only = 0;
static int skip_functions = 0, read_functions = 0;

static char *find_command_in_path(const char *name, const char *path_list, int *path_index)
{
  char *found = NULL, *full_path;
  int status, name_len;

  name_len = strlen(name);

  if (!absolute_program(name))
    absolute_path_given = 0;
  else
  {
    char *p;
    absolute_path_given = 1;

    if (abs_path)
      free(abs_path);

    if (*name != '.' && *name != '/' && *name != '~')
    {
      abs_path = (char *)xmalloc(3 + name_len);
      strcpy(abs_path, "./");
      strcat(abs_path, name);
    }
    else
    {
      abs_path = (char *)xmalloc(1 + name_len);
      strcpy(abs_path, name);
    }

    path_list = abs_path;
    p = strrchr(abs_path, '/');
    *p++ = 0;
    name = p;
  }

  while (path_list && path_list[*path_index])
  {
    char *path;

    if (absolute_path_given)
    {
      path = savestring(path_list);
      *path_index = strlen(path);
    }
    else
      path = get_next_path_element(path_list, path_index);

    if (!path)
      break;

    if (*path == '~')
    {
      char *t = tilde_expand(path);
      free(path);
      path = t;

      if (skip_tilde)
      {
	free(path);
	continue;
      }
    }

    if (skip_dot && *path != '/')
    {
      free(path);
      continue;
    }

    found_path_starts_with_dot = (*path == '.');

    full_path = make_full_pathname(path, name, name_len);
    free(path);

    status = file_status(full_path);

    if ((status & FS_EXISTS) && (status & FS_EXECABLE))
    {
      found = full_path;
      break;
    }

    free(full_path);
  }

  return (found);
}

static char cwd[256];
static size_t cwdlen;

static void get_current_working_directory(void)
{
  if (cwdlen)
    return;

  if (!getcwd(cwd, sizeof(cwd)))
  {
    const char *pwd = getenv("PWD");
    if (pwd && strlen(pwd) < sizeof(cwd))
      strcpy(cwd, pwd);
  }

  if (*cwd != '/')
  {
    fprintf(stderr, "Can't get current working directory\n");
    exit(-1);
  }

  cwdlen = strlen(cwd);

  if (cwd[cwdlen - 1] != '/')
  {
    cwd[cwdlen++] = '/';
    cwd[cwdlen] = 0;
  }
}

static char *path_clean_up(const char *path)
{
  static char result[256];

  const char *p1 = path;
  char *p2 = result;

  int saw_slash = 0, saw_slash_dot = 0, saw_slash_dot_dot = 0;

  if (*p1 != '/')
  {
    get_current_working_directory();
    strcpy(result, cwd);
    saw_slash = 1;
    p2 = &result[cwdlen];
  }

  do
  {
    /*
     * Two leading slashes are allowed, having an OS implementation-defined meaning.
     * See http://www.opengroup.org/onlinepubs/009695399/basedefs/xbd_chap04.html#tag_04_11
     */
    if (!saw_slash || *p1 != '/' || (p1 == path + 1 && p1[1] != '/'))
      *p2++ = *p1;
    if (saw_slash_dot && (*p1 == '/'))
      p2 -= 2;
    if (saw_slash_dot_dot && (*p1 == '/'))
    {
      int cnt = 0;
      do
      {
	if (--p2 < result)
	{
	  strcpy(result, path);
	  return result;
	}
	if (*p2 == '/')
	  ++cnt;
      }
      while (cnt != 3);
      ++p2;
    }
    saw_slash_dot_dot = saw_slash_dot && (*p1 == '.');
    saw_slash_dot = saw_slash && (*p1 == '.');
    saw_slash = (*p1 == '/');
  }
  while (*p1++);

  return result;
}

struct function_st {
  char *name;
  size_t len;
  char **lines;
  int line_count;
};

static struct function_st *functions;
static int func_count;
static int max_func_count;

static char **aliases;
static int alias_count;
static int max_alias_count;

int func_search(int indent, const char *cmd, struct function_st *func_list, int function_start_type)
{
  int i;
  for (i = 0; i < func_count; ++i)
  {
    if (!strcmp(functions[i].name, cmd))
    {
      int j;
      if (indent)
        fputc('\t', stdout);
      if (function_start_type == 1)
	fprintf(stdout, "%s () {\n", cmd);
      else
	fprintf(stdout, "%s ()\n", cmd);
      for (j = 0; j < functions[i].line_count; ++j)
      {
	if (indent)
	  fputc('\t', stdout);
        fputs(functions[i].lines[j], stdout);
      }
      return 1;
    }
  }
  return 0;
}

int path_search(int indent, const char *cmd, const char *path_list)
{
  char *result = NULL;
  int found_something = 0;

  if (path_list && *path_list != '\0')
  {
    int next;
    int path_index = 0;
    do
    {
      next = show_all;
      result = find_command_in_path(cmd, path_list, &path_index);
      if (result)
      {
	const char *full_path = path_clean_up(result);
	int in_home = (show_tilde || skip_tilde) && !strncmp(full_path, home, homelen);
	if (indent)
	  fprintf(stdout, "\t");
	if (!(skip_tilde && in_home) && show_dot && found_path_starts_with_dot && !strncmp(full_path, cwd, cwdlen))
	{
	  full_path += cwdlen;
	  fprintf(stdout, "./");
	}
	else if (in_home)
	{
	  if (skip_tilde)
	  {
	    next = 1;
	    free(result);
	    continue;
	  }
	  if (show_tilde)
	  {
	    full_path += homelen;
	    fprintf(stdout, "~/");
	  }
	}
	fprintf(stdout, "%s\n", full_path);
	free(result);
	found_something = 1;
      }
      else
	break;
    }
    while (next);
  }

  return found_something;
}

void process_alias(const char *str, int argc, char *argv[], const char *path_list, int function_start_type)
{
  const char *p = str;
  int len = 0;

  while(*p == ' ' || *p == '\t')
    ++p;
  if (!strncmp("alias", p, 5))
    p += 5;
  while(*p == ' ' || *p == '\t')
    ++p;
  while(*p && *p != ' ' && *p != '\t' && *p != '=')
    ++p, ++len;

  for (; argc > 0; --argc, ++argv)
  {
    char q = 0;
    char *cmd;

    if (!*argv || len != strlen(*argv) || strncmp(*argv, &p[-len], len))
      continue;

    fputs(str, stdout);

    if (!show_all)
      *argv = NULL;

    while(*p == ' ' || *p == '\t')
      ++p;
    if (*p == '=')
      ++p;
    while(*p == ' ' || *p == '\t')
      ++p;
    if (*p == '"' || *p == '\'')
      q = *p, ++p;

    for(;;)
    {
      int found = 0;

      while(*p == ' ' || *p == '\t')
	++p;
      len = 0;
      while(*p && *p != ' ' && *p != '\t' && *p != q && *p != '|' && *p != '&')
	++p, ++len;

      cmd = (char *)xmalloc(len + 1);
      strncpy(cmd, &p[-len], len);
      cmd[len] = 0;
      if (*argv && !strcmp(cmd, *argv))
        *argv = NULL;
      if (read_functions && !strchr(cmd, '/'))
        found = func_search(1, cmd, functions, function_start_type);
      if (show_all || !found)
	path_search(1, cmd, path_list);
      free(cmd);

      while(*p && (*p != '|' || p[1] == '|') && (*p != '&' || p[1] == '&'))
        ++p;

      if (!*p)
        break;

      ++p;
    }

    break;
  }
}

enum opts {
  opt_version,
  opt_skip_dot,
  opt_skip_tilde,
  opt_skip_alias,
  opt_read_functions,
  opt_skip_functions,
  opt_show_dot,
  opt_show_tilde,
  opt_tty_only,
  opt_help
};

#ifdef __TANDEM
/* According to Tom Bates, <tom.bates@hp.com> */
static uid_t const superuser = 65535;
#else
static uid_t const superuser = 0;
#endif

int main(int argc, char *argv[])
{
  const char *path_list = getenv("PATH");
  int short_option, fail_count = 0;
  static int long_option;
  struct option longopts[] = {
    {"help", 0, &long_option, opt_help},
    {"version", 0, &long_option, opt_version},
    {"skip-dot", 0, &long_option, opt_skip_dot},
    {"skip-tilde", 0, &long_option, opt_skip_tilde},
    {"show-dot", 0, &long_option, opt_show_dot},
    {"show-tilde", 0, &long_option, opt_show_tilde},
    {"tty-only", 0, &long_option, opt_tty_only},
    {"all", 0, NULL, 'a'},
    {"read-alias", 0, NULL, 'i'},
    {"skip-alias", 0, &long_option, opt_skip_alias},
    {"read-functions", 0, &long_option, opt_read_functions},
    {"skip-functions", 0, &long_option, opt_skip_functions},
    {NULL, 0, NULL, 0}
  };

  progname = argv[0];
  while ((short_option = getopt_long(argc, argv, "aivV", longopts, NULL)) != -1)
  {
    switch (short_option)
    {
      case 0:
	switch (long_option)
	{
	  case opt_help:
	    print_usage(stdout);
	    return 0;
	  case opt_version:
	    print_version();
	    return 0;
	  case opt_skip_dot:
	    skip_dot = !tty_only;
	    break;
	  case opt_skip_tilde:
	    skip_tilde = !tty_only;
	    break;
	  case opt_skip_alias:
	    skip_alias = 1;
	    break;
	  case opt_show_dot:
	    show_dot = !tty_only;
	    break;
	  case opt_show_tilde:
	    show_tilde = (!tty_only && geteuid() != superuser);
	    break;
	  case opt_tty_only:
	    tty_only = !isatty(1);
	    break;
	  case opt_read_functions:
	    read_functions = 1;
	    break;
	  case opt_skip_functions:
	    skip_functions = 1;
	    break;
	}
	break;
      case 'a':
	show_all = 1;
	break;
      case 'i':
        read_alias = 1;
	break;
      case 'v':
      case 'V':
	print_version();
	return 0;
    }
  }

  uidget();

  if (show_dot)
    get_current_working_directory();

  if (show_tilde || skip_tilde)
  {
    const char *h;

    if (!(h = getenv("HOME")))
      h = sh_get_home_dir();

    strncpy(home, h, sizeof(home));
    home[sizeof(home) - 1] = 0;
    homelen = strlen(home);
    if (home[homelen - 1] != '/' && homelen < sizeof(home) - 1)
    {
      strcat(home, "/");
      ++homelen;
    }
  }

  if (skip_alias)
    read_alias = 0;

  if (skip_functions)
    read_functions = 0;

  argv += optind;
  argc -= optind;

  if (argc == 0)
  {
    print_usage(stderr);
    return -1;
  }

  int function_start_type = 0;
  if (read_alias || read_functions)
  {
    char buf[1024];
    int processing_aliases = read_alias;

    if (isatty(0))
    {
      fprintf(stderr, "%s: %s: Warning: stdin is a tty.\n", progname,
          (read_functions ? read_alias ? "--read-functions, --read-alias, -i" : "--read-functions" : "--read-alias, -i"));
    }

    while (fgets(buf, sizeof(buf), stdin))
    {
      int looks_like_function_start = 0;
      int function_start_has_declare;
      if (read_functions)
      {
	// bash version 2.0.5a and older output a pattern for `str' like
	// declare -fx FUNCTION_NAME ()
	// {
	//   body
	// }
	//
	// bash version 2.0.5b and later output a pattern for `str' like
	// FUNCTION_NAME ()
	// {
	//   body
	// }
	char *p = buf + strlen(buf) - 1;
	while (isspace(*p) && p > buf + 2)
	  --p;
	if (*p == ')' && p[-1] == '(' && p[-2] == ' ')
	{
	  looks_like_function_start = 1;
	  function_start_has_declare = (strncmp("declare -", buf, 9) == 0);
	}
	// Add some zsh support here.
	// zsh does output a pattern for `str' like
	// FUNCTION () {
	//   body
	// }
	if (p > buf + 4 && *p == '{' && p[-1] == ' ' &&
	    p[-2] == ')' && p[-3] == '(' && p[-4] == ' ')
	{
	  looks_like_function_start = 1;
	  function_start_type = 1;
	  function_start_has_declare = 0;
	}
      }
      if (processing_aliases && !looks_like_function_start)
      {
	// bash version 2.0.5b can throw in lines like "declare -fx FUNCTION_NAME", eat them.
	if (!strncmp("declare -", buf, 9))
	  continue;
	if (alias_count == max_alias_count)
	{
	  max_alias_count += 32;
	  aliases = (char **)xrealloc(aliases, max_alias_count * sizeof(char *));
	}
	aliases[alias_count++] = strcpy((char *)xmalloc(strlen(buf) + 1), buf);
      }
      else if (read_functions && looks_like_function_start)
      {
        struct function_st *function;
        int max_line_count;

	const char *p = buf;
	int len = 0;

        processing_aliases = 0;

	// Eat "declare -fx " at start of bash version 2.0.5a and older, if present.
	if (function_start_has_declare)
	{
	  p += 9;
	  while(*p && *p++ != ' ');
	}

	while(*p && *p != ' ')
	  ++p, ++len;

	if (func_count == max_func_count)
	{
	  max_func_count += 16;
	  functions = (struct function_st *)xrealloc(functions, max_func_count * sizeof(struct function_st));
	}
	function = &functions[func_count++];
	function->name = (char *)xmalloc(len + 1);
	strncpy(function->name, &p[-len], len);
	function->name[len] = 0;
	function->len = len;
	max_line_count = 32;
	function->lines = (char **)xmalloc(max_line_count * sizeof(char *));
	function->line_count = 0;
	while (fgets(buf, sizeof(buf), stdin))
	{
	  size_t blen = strlen(buf);
	  function->lines[function->line_count++] = strcpy((char *)xmalloc(blen + 1), buf);
	  if (!strcmp(buf, "}\n"))
	    break;
          if (function->line_count == max_line_count)
	  {
	    max_line_count += 32;
	    function->lines = (char **)xrealloc(function->lines, max_line_count * sizeof(char *));
	  }
	}
      }
    }
    if (read_alias)
    {
      int i;
      for (i = 0; i < alias_count; ++i)
	process_alias(aliases[i], argc, argv, path_list, function_start_type);
    }
  }

  for (; argc > 0; --argc, ++argv)
  {
    int found_something = 0;

    if (!*argv)
      continue;

    if (read_functions && !strchr(*argv, '/'))
      found_something = func_search(0, *argv, functions, function_start_type);

    if ((show_all || !found_something) && !path_search(0, *argv, path_list) && !found_something)
    {
      print_fail(absolute_path_given ? strrchr(*argv, '/') + 1 : *argv, absolute_path_given ? abs_path : path_list);
      ++fail_count;
    }
  }

  return fail_count;
}

#ifdef NEED_XMALLOC
void *xmalloc(size_t size)
{
  void *ptr = malloc(size);
  if (ptr == NULL)
  {
    fprintf(stderr, "%s: Out of memory", progname);
    exit(-1);
  }
  return ptr;
}

void *xrealloc(void *ptr, size_t size)
{
  if (!ptr)
    return xmalloc(size);
  ptr = realloc(ptr, size);
  if (size > 0 && ptr == NULL)
  {
    fprintf(stderr, "%s: Out of memory\n", progname);
    exit(-1);
  }
  return ptr;
}
#endif /* NEED_XMALLOC */
