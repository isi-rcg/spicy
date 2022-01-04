// -*- C++ -*-
/* Copyright (C) 1989-2018 Free Software Foundation, Inc.
     Written by James Clark (jjc@jclark.com)

This file is part of groff.

groff is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free
Software Foundation, either version 3 of the License, or
(at your option) any later version.

groff is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

// A front end for groff.

#include "lib.h"

#include <stdlib.h>
#include <signal.h>
#include <errno.h>

#include "assert.h"
#include "errarg.h"
#include "error.h"
#include "stringclass.h"
#include "cset.h"
#include "font.h"
#include "device.h"
#include "pipeline.h"
#include "nonposix.h"
#include "relocate.h"
#include "defs.h"

#define GXDITVIEW "gxditview"

// troff will be passed an argument of -rXREG=1 if the -X option is
// specified
#define XREG ".X"

#ifdef NEED_DECLARATION_PUTENV
extern "C" {
  int putenv(const char *);
}
#endif /* NEED_DECLARATION_PUTENV */

// The number of commands must be in sync with MAX_COMMANDS in pipeline.h

// grap, chem, and ideal must come before pic;
// tbl must come before eqn
const int PRECONV_INDEX = 0;
const int SOELIM_INDEX = PRECONV_INDEX + 1;
const int REFER_INDEX = SOELIM_INDEX + 1;
const int GRAP_INDEX = REFER_INDEX + 1;
const int CHEM_INDEX = GRAP_INDEX + 1;
const int IDEAL_INDEX = CHEM_INDEX + 1;
const int PIC_INDEX = IDEAL_INDEX + 1;
const int TBL_INDEX = PIC_INDEX + 1;
const int GRN_INDEX = TBL_INDEX + 1;
const int EQN_INDEX = GRN_INDEX + 1;
const int TROFF_INDEX = EQN_INDEX + 1;
const int POST_INDEX = TROFF_INDEX + 1;
const int SPOOL_INDEX = POST_INDEX + 1;

const int NCOMMANDS = SPOOL_INDEX + 1;

class possible_command {
  char *name;
  string args;
  char **argv;

  void build_argv();
public:
  possible_command();
  ~possible_command();
  void clear_name();
  void set_name(const char *);
  void set_name(const char *, const char *);
  const char *get_name();
  void append_arg(const char *, const char * = 0);
  void insert_arg(const char *);
  void insert_args(string s);
  void clear_args();
  char **get_argv();
  void print(int is_last, FILE *fp);
};

extern "C" const char *Version_string;

int lflag = 0;
char *spooler = 0;
char *postdriver = 0;
char *predriver = 0;

possible_command commands[NCOMMANDS];

int run_commands(int no_pipe);
void print_commands(FILE *);
void append_arg_to_string(const char *arg, string &str);
void handle_unknown_desc_command(const char *command, const char *arg,
				 const char *filename, int lineno);
const char *xbasename(const char *);

void usage(FILE *stream);
void help();

int main(int argc, char **argv)
{
  program_name = argv[0];
  static char stderr_buf[BUFSIZ];
  setbuf(stderr, stderr_buf);
  assert(NCOMMANDS <= MAX_COMMANDS);
  string Pargs, Largs, Fargs;
  int Kflag = 0;
  int vflag = 0;
  int Vflag = 0;
  int zflag = 0;
  int iflag = 0;
  int Xflag = 0;
  int oflag = 0;
  int safer_flag = 1;
  int is_xhtml = 0;
  int eflag = 0;
  int need_pic = 0;
  int opt;
  const char *command_prefix = getenv("GROFF_COMMAND_PREFIX");
  const char *encoding = getenv("GROFF_ENCODING");
  if (!command_prefix)
    command_prefix = PROG_PREFIX;
  commands[TROFF_INDEX].set_name(command_prefix, "troff");
  static const struct option long_options[] = {
    { "help", no_argument, 0, 'h' },
    { "version", no_argument, 0, 'v' },
    { NULL, 0, 0, 0 }
  };
  while ((opt = getopt_long(
		  argc, argv,
		  "abcCd:D:eEf:F:gGhiI:jJkK:lL:m:M:n:No:pP:r:RsStT:UvVw:W:XzZ",
		  long_options, NULL))
	 != EOF) {
    char buf[3];
    buf[0] = '-';
    buf[1] = opt;
    buf[2] = '\0';
    switch (opt) {
    case 'i':
      iflag = 1;
      break;
    case 'I':
      commands[SOELIM_INDEX].set_name(command_prefix, "soelim");
      commands[SOELIM_INDEX].append_arg(buf, optarg);
      // .psbb may need to search for files
      commands[TROFF_INDEX].append_arg(buf, optarg);
      // \X'ps:import' may need to search for files
      Pargs += buf;
      Pargs += optarg;
      Pargs += '\0';
      break;
    case 'D':
      commands[PRECONV_INDEX].set_name("preconv");
      commands[PRECONV_INDEX].append_arg("-D", optarg);
      break;
    case 'K':
      commands[PRECONV_INDEX].append_arg("-e", optarg);
      Kflag = 1;
      // fall through
    case 'k':
      commands[PRECONV_INDEX].set_name("preconv");
      break;
    case 't':
      commands[TBL_INDEX].set_name(command_prefix, "tbl");
      break;
    case 'J':
      commands[IDEAL_INDEX].set_name(command_prefix, "gideal");
      // need_pic = 1;
      break;
    case 'j':
      commands[CHEM_INDEX].set_name(command_prefix, "chem");
      need_pic = 1;
      break;
    case 'p':
      commands[PIC_INDEX].set_name(command_prefix, "pic");
      break;
    case 'g':
      commands[GRN_INDEX].set_name(command_prefix, "grn");
      break;
    case 'G':
      commands[GRAP_INDEX].set_name(command_prefix, "grap");
      need_pic = 1;
      break;
    case 'e':
      eflag = 1;
      commands[EQN_INDEX].set_name(command_prefix, "eqn");
      break;
    case 's':
      commands[SOELIM_INDEX].set_name(command_prefix, "soelim");
      break;
    case 'R':
      commands[REFER_INDEX].set_name(command_prefix, "refer");
      break;
    case 'z':
    case 'a':
      commands[TROFF_INDEX].append_arg(buf);
      // fall through
    case 'Z':
      zflag++;
      break;
    case 'l':
      lflag++;
      break;
    case 'V':
      Vflag++;
      break;
    case 'v':
      vflag = 1;
      printf("GNU groff version %s\n", Version_string);
      printf(
	"Copyright (C) 2018 Free Software Foundation, Inc.\n"
	"GNU groff comes with ABSOLUTELY NO WARRANTY.\n"
	"You may redistribute copies of groff and its subprograms\n"
	"under the terms of the GNU General Public License.\n"
	"For more information about these matters, see the file\n"
	"named COPYING.\n");
      printf("\ncalled subprograms:\n\n");
      fflush(stdout);
      commands[POST_INDEX].append_arg(buf);
      // fall through
    case 'C':
      commands[SOELIM_INDEX].append_arg(buf);
      commands[REFER_INDEX].append_arg(buf);
      commands[PIC_INDEX].append_arg(buf);
      commands[GRAP_INDEX].append_arg(buf);
      commands[TBL_INDEX].append_arg(buf);
      commands[GRN_INDEX].append_arg(buf);
      commands[EQN_INDEX].append_arg(buf);
      commands[TROFF_INDEX].append_arg(buf);
      break;
    case 'N':
      commands[EQN_INDEX].append_arg(buf);
      break;
    case 'h':
      help();
      break;
    case 'E':
    case 'b':
      commands[TROFF_INDEX].append_arg(buf);
      break;
    case 'c':
      commands[TROFF_INDEX].append_arg(buf);
      break;
    case 'S':
      safer_flag = 1;
      break;
    case 'U':
      safer_flag = 0;
      break;
    case 'T':
      if (strcmp(optarg, "xhtml") == 0) {
	// force soelim to aid the html preprocessor
	commands[SOELIM_INDEX].set_name(command_prefix, "soelim");
	Pargs += "-x";
	Pargs += '\0';
	Pargs += 'x';
	Pargs += '\0';
	is_xhtml = 1;
	device = "html";
	break;
      }
      if (strcmp(optarg, "html") == 0)
	// force soelim to aid the html preprocessor
	commands[SOELIM_INDEX].set_name(command_prefix, "soelim");
      if (strcmp(optarg, "Xps") == 0) {
	warning("-TXps option is obsolete: use -X -Tps instead");
	device = "ps";
	Xflag++;
      }
      else
	device = optarg;
      break;
    case 'F':
      font::command_line_font_dir(optarg);
      if (Fargs.length() > 0) {
	Fargs += PATH_SEP_CHAR;
	Fargs += optarg;
      }
      else
	Fargs = optarg;
      break;
    case 'o':
      oflag = 1;
      // fall through
    case 'f':
    case 'm':
    case 'r':
    case 'd':
    case 'n':
    case 'w':
    case 'W':
      commands[TROFF_INDEX].append_arg(buf, optarg);
      break;
    case 'M':
      commands[EQN_INDEX].append_arg(buf, optarg);
      commands[GRAP_INDEX].append_arg(buf, optarg);
      commands[GRN_INDEX].append_arg(buf, optarg);
      commands[TROFF_INDEX].append_arg(buf, optarg);
      break;
    case 'P':
      Pargs += optarg;
      Pargs += '\0';
      break;
    case 'L':
      append_arg_to_string(optarg, Largs);
      break;
    case 'X':
      Xflag++;
      break;
    case '?':
      usage(stderr);
      exit(1);
      break;
    default:
      assert(0);
      break;
    }
  }
  if (need_pic)
    commands[PIC_INDEX].set_name(command_prefix, "pic");
  if (encoding) {
    commands[PRECONV_INDEX].set_name("preconv");
    if (!Kflag && *encoding)
      commands[PRECONV_INDEX].append_arg("-e", encoding);
  }
  if (!safer_flag) {
    commands[TROFF_INDEX].insert_arg("-U");
    commands[PIC_INDEX].append_arg("-U");
  }
  font::set_unknown_desc_command_handler(handle_unknown_desc_command);
  if (!font::load_desc())
    fatal("invalid device '%1'", device);
  if (!postdriver)
    fatal("no 'postpro' command in DESC file for device '%1'", device);
  if (predriver && !zflag) {
    commands[TROFF_INDEX].insert_arg(commands[TROFF_INDEX].get_name());
    commands[TROFF_INDEX].set_name(predriver);
    // pass the device arguments to the predrivers as well
    commands[TROFF_INDEX].insert_args(Pargs);
    if (eflag && is_xhtml)
      commands[TROFF_INDEX].insert_arg("-e");
    if (vflag)
      commands[TROFF_INDEX].insert_arg("-v");
  }
  const char *real_driver = 0;
  if (Xflag) {
    real_driver = postdriver;
    postdriver = (char *)GXDITVIEW;
    commands[TROFF_INDEX].append_arg("-r" XREG "=", "1");
  }
  if (postdriver)
    commands[POST_INDEX].set_name(postdriver);
  int gxditview_flag = postdriver
		       && strcmp(xbasename(postdriver), GXDITVIEW) == 0;
  if (gxditview_flag && argc - optind == 1) {
    commands[POST_INDEX].append_arg("-title");
    commands[POST_INDEX].append_arg(argv[optind]);
    commands[POST_INDEX].append_arg("-xrm");
    commands[POST_INDEX].append_arg("*iconName:", argv[optind]);
    string filename_string("|");
    append_arg_to_string(argv[0], filename_string);
    append_arg_to_string("-Z", filename_string);
    for (int i = 1; i < argc; i++)
      append_arg_to_string(argv[i], filename_string);
    filename_string += '\0';
    commands[POST_INDEX].append_arg("-filename");
    commands[POST_INDEX].append_arg(filename_string.contents());
  }
  if (gxditview_flag && Xflag) {
    string print_string(real_driver);
    if (spooler) {
      print_string += " | ";
      print_string += spooler;
      print_string += Largs;
    }
    print_string += '\0';
    commands[POST_INDEX].append_arg("-printCommand");
    commands[POST_INDEX].append_arg(print_string.contents());
  }
  const char *p = Pargs.contents();
  const char *end = p + Pargs.length();
  while (p < end) {
    commands[POST_INDEX].append_arg(p);
    p = strchr(p, '\0') + 1;
  }
  if (gxditview_flag)
    commands[POST_INDEX].append_arg("-");
  if (lflag && !vflag && !Xflag && spooler) {
    commands[SPOOL_INDEX].set_name(BSHELL);
    commands[SPOOL_INDEX].append_arg(BSHELL_DASH_C);
    Largs += '\0';
    Largs = spooler + Largs;
    commands[SPOOL_INDEX].append_arg(Largs.contents());
  }
  if (zflag) {
    commands[POST_INDEX].set_name(0);
    commands[SPOOL_INDEX].set_name(0);
  }
  commands[TROFF_INDEX].append_arg("-T", device);
  if (strcmp(device, "html") == 0) {
    if (is_xhtml) {
      if (oflag)
	fatal("'-o' option is invalid with device 'xhtml'");
      if (zflag)
	commands[EQN_INDEX].append_arg("-Tmathml:xhtml");
      else if (eflag)
	commands[EQN_INDEX].clear_name();
    }
    else {
      if (oflag)
	fatal("'-o' option is invalid with device 'html'");
      // html renders equations as images via ps
      commands[EQN_INDEX].append_arg("-Tps:html");
    }
  }
  else
    commands[EQN_INDEX].append_arg("-T", device);

  commands[GRN_INDEX].append_arg("-T", device);

  int first_index;
  for (first_index = 0; first_index < TROFF_INDEX; first_index++)
    if (commands[first_index].get_name() != 0)
      break;
  if (optind < argc) {
    if (argv[optind][0] == '-' && argv[optind][1] != '\0')
      commands[first_index].append_arg("--");
    for (int i = optind; i < argc; i++)
      commands[first_index].append_arg(argv[i]);
    if (iflag)
      commands[first_index].append_arg("-");
  }
  if (Fargs.length() > 0) {
    string e = "GROFF_FONT_PATH";
    e += '=';
    e += Fargs;
    char *fontpath = getenv("GROFF_FONT_PATH");
    if (fontpath && *fontpath) {
      e += PATH_SEP_CHAR;
      e += fontpath;
    }
    e += '\0';
    if (putenv(strsave(e.contents())))
      fatal("putenv failed");
  }
  {
    // we save the original path in GROFF_PATH__ and put it into the
    // environment -- troff will pick it up later.
    char *path = getenv("PATH");
    string e = "GROFF_PATH__";
    e += '=';
    if (path && *path)
      e += path;
    e += '\0';
    if (putenv(strsave(e.contents())))
      fatal("putenv failed");
    char *binpath = getenv("GROFF_BIN_PATH");
    string f = "PATH";
    f += '=';
    if (binpath && *binpath)
      f += binpath;
    else {
      binpath = relocatep(BINPATH);
      f += binpath;
    }
    if (path && *path) {
      f += PATH_SEP_CHAR;
      f += path;
    }
    f += '\0';
    if (putenv(strsave(f.contents())))
      fatal("putenv failed");
  }
  if (Vflag)
    print_commands(Vflag == 1 ? stdout : stderr);
  if (Vflag == 1)
    exit(0);
  return run_commands(vflag);
}

const char *xbasename(const char *s)
{
  if (!s)
    return 0;
  // DIR_SEPS[] are possible directory separator characters, see nonposix.h
  // We want the rightmost separator of all possible ones.
  // Example: d:/foo\\bar.
  const char *p = strrchr(s, DIR_SEPS[0]), *p1;
  const char *sep = &DIR_SEPS[1];

  while (*sep)
    {
      p1 = strrchr(s, *sep);
      if (p1 && (!p || p1 > p))
	p = p1;
      sep++;
    }
  return p ? p + 1 : s;
}

void handle_unknown_desc_command(const char *command, const char *arg,
				 const char *filename, int lineno)
{
  if (strcmp(command, "print") == 0) {
    if (arg == 0)
      error_with_file_and_line(filename, lineno,
			       "'print' command requires an argument");
    else
      spooler = strsave(arg);
  }
  if (strcmp(command, "prepro") == 0) {
    if (arg == 0)
      error_with_file_and_line(filename, lineno,
			       "'prepro' command requires an argument");
    else {
      for (const char *p = arg; *p; p++)
	if (csspace(*p)) {
	  error_with_file_and_line(filename, lineno,
				   "invalid 'prepro' argument '%1'"
				   ": program name required", arg);
	  return;
	}
      predriver = strsave(arg);
    }
  }
  if (strcmp(command, "postpro") == 0) {
    if (arg == 0)
      error_with_file_and_line(filename, lineno,
			       "'postpro' command requires an argument");
    else {
      for (const char *p = arg; *p; p++)
	if (csspace(*p)) {
	  error_with_file_and_line(filename, lineno,
				   "invalid 'postpro' argument '%1'"
				   ": program name required", arg);
	  return;
	}
      postdriver = strsave(arg);
    }
  }
}

void print_commands(FILE *fp)
{
  int last;
  for (last = SPOOL_INDEX; last >= 0; last--)
    if (commands[last].get_name() != 0)
      break;
  for (int i = 0; i <= last; i++)
    if (commands[i].get_name() != 0)
      commands[i].print(i == last, fp);
}

// Run the commands. Return the code with which to exit.

int run_commands(int no_pipe)
{
  char **v[NCOMMANDS];
  int j = 0;
  for (int i = 0; i < NCOMMANDS; i++)
    if (commands[i].get_name() != 0)
      v[j++] = commands[i].get_argv();
  return run_pipeline(j, v, no_pipe);
}

possible_command::possible_command()
: name(0), argv(0)
{
}

possible_command::~possible_command()
{
  free(name);
  a_delete argv;
}

void possible_command::set_name(const char *s)
{
  free(name);
  name = strsave(s);
}

void possible_command::clear_name()
{
  a_delete name;
  a_delete argv;
  name = NULL;
  argv = NULL;
}

void possible_command::set_name(const char *s1, const char *s2)
{
  free(name);
  name = (char*)malloc(strlen(s1) + strlen(s2) + 1);
  strcpy(name, s1);
  strcat(name, s2);
}

const char *possible_command::get_name()
{
  return name;
}

void possible_command::clear_args()
{
  args.clear();
}

void possible_command::append_arg(const char *s, const char *t)
{
  args += s;
  if (t)
    args += t;
  args += '\0';
}

void possible_command::insert_arg(const char *s)
{
  string str(s);
  str += '\0';
  str += args;
  args = str;
}

void possible_command::insert_args(string s)
{
  const char *p = s.contents();
  const char *end = p + s.length();
  int l = 0;
  if (p >= end)
    return;
  // find the total number of arguments in our string
  do {
    l++;
    p = strchr(p, '\0') + 1;
  } while (p < end);
  // now insert each argument preserving the order
  for (int i = l - 1; i >= 0; i--) {
    p = s.contents();
    for (int j = 0; j < i; j++)
      p = strchr(p, '\0') + 1;
    insert_arg(p);
  }
}

void possible_command::build_argv()
{
  if (argv)
    return;
  // Count the number of arguments.
  int len = args.length();
  int argc = 1;
  char *p = 0;
  if (len > 0) {
    p = &args[0];
    for (int i = 0; i < len; i++)
      if (p[i] == '\0')
	argc++;
  }
  // Build an argument vector.
  argv = new char *[argc + 1];
  argv[0] = name;
  for (int i = 1; i < argc; i++) {
    argv[i] = p;
    p = strchr(p, '\0') + 1;
  }
  argv[argc] = 0;
}

void possible_command::print(int is_last, FILE *fp)
{
  build_argv();
  if (IS_BSHELL(argv[0])
      && argv[1] != 0 && strcmp(argv[1], BSHELL_DASH_C) == 0
      && argv[2] != 0 && argv[3] == 0)
    fputs(argv[2], fp);
  else {
    fputs(argv[0], fp);
    string str;
    for (int i = 1; argv[i] != 0; i++) {
      str.clear();
      append_arg_to_string(argv[i], str);
      put_string(str, fp);
    }
  }
  if (is_last)
    putc('\n', fp);
  else
    fputs(" | ", fp);
}

void append_arg_to_string(const char *arg, string &str)
{
  str += ' ';
  int needs_quoting = 0;
  // Native Windows programs don't support '..' style of quoting, so
  // always behave as if ARG included the single quote character.
#if defined(_WIN32) && !defined(__CYGWIN__)
  int contains_single_quote = 1;
#else
  int contains_single_quote = 0;
#endif
  const char*p;
  for (p = arg; *p != '\0'; p++)
    switch (*p) {
    case ';':
    case '&':
    case '(':
    case ')':
    case '|':
    case '^':
    case '<':
    case '>':
    case '\n':
    case ' ':
    case '\t':
    case '\\':
    case '"':
    case '$':
    case '?':
    case '*':
      needs_quoting = 1;
      break;
    case '\'':
      contains_single_quote = 1;
      break;
    }
  if (contains_single_quote || arg[0] == '\0') {
    str += '"';
    for (p = arg; *p != '\0'; p++)
      switch (*p) {
#if !(defined(_WIN32) && !defined(__CYGWIN__))
      case '"':
      case '\\':
      case '$':
	str += '\\';
#else
      case '"':
      case '\\':
	if (*p == '"' || (*p == '\\' && p[1] == '"'))
	  str += '\\';
#endif
	// fall through
      default:
	str += *p;
	break;
      }
    str += '"';
  }
  else if (needs_quoting) {
    str += '\'';
    str += arg;
    str += '\'';
  }
  else
    str += arg;
}

char **possible_command::get_argv()
{
  build_argv();
  return argv;
}

void synopsis(FILE *stream)
{
  fprintf(stream,
"usage: %s [-abceghijklpstvzCEGNRSUVXZ] [-dcs] [-ffam] [-mname] [-nnum]\n"
"       [-olist] [-rcn] [-wname] [-Darg] [-Fdir] [-Idir] [-Karg] [-Larg]\n"
"       [-Mdir] [-Parg] [-Tdev] [-Wname] [files...]\n",
	  program_name);
}

void help()
{
  synopsis(stdout);
  fputs("\n"
"-h\tprint this message\n"
"-v\tprint version number\n"
"-e\tpreprocess with eqn\n"
"-g\tpreprocess with grn\n"
"-j\tpreprocess with chem\n"
"-k\tpreprocess with preconv\n"
"-p\tpreprocess with pic\n"
"-s\tpreprocess with soelim\n"
"-t\tpreprocess with tbl\n"
"-G\tpreprocess with grap\n"
"-J\tpreprocess with gideal\n"
"-R\tpreprocess with refer\n"
"-a\tproduce ASCII description of output\n"
"-b\tprint backtraces with errors or warnings\n"
"-c\tdisable color output\n"
"-dcs\tdefine a string c as s\n"
"-ffam\tuse fam as the default font family\n"
"-i\tread standard input after named input files\n"
"-l\tspool the output\n"
"-mname\tread macros tmac.name\n"
"-nnum\tnumber first page n\n"
"-olist\toutput only pages in list\n"
"-rcn\tdefine a number register c as n\n"
"-wname\tenable warning name\n"
"-z\tsuppress formatted output\n"
"-C\tenable compatibility mode\n"
"-Darg\tuse arg as default input encoding.  Implies -k\n"
"-E\tinhibit all errors\n"
"-Fdir\tsearch dir for device directories\n"
"-Idir\tsearch dir for soelim, troff, and grops.  Implies -s\n"
"-Karg\tuse arg as input encoding.  Implies -k\n"
"-Larg\tpass arg to the spooler\n"
"-Mdir\tsearch dir for macro files\n"
"-N\tdon't allow newlines within eqn delimiters\n"
"-Parg\tpass arg to the postprocessor\n"
"-S\tenable safer mode (the default)\n"
"-Tdev\tuse device dev\n"
"-U\tenable unsafe mode\n"
"-V\tprint commands on stdout instead of running them\n"
"-Wname\tinhibit warning name\n"
"-X\tuse X11 previewer rather than usual postprocessor\n"
"-Z\tdon't postprocess\n"
"\n",
	stdout);
  exit(0);
}

void usage(FILE *stream)
{
  synopsis(stream);
  fprintf(stream, "%s -h gives more help\n", program_name);
}

extern "C" {

void c_error(const char *format, const char *arg1, const char *arg2,
	     const char *arg3)
{
  error(format, arg1, arg2, arg3);
}

void c_fatal(const char *format, const char *arg1, const char *arg2,
	     const char *arg3)
{
  fatal(format, arg1, arg2, arg3);
}

}
