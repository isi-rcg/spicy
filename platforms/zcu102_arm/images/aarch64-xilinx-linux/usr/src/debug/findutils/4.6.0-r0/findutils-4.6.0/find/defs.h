/* defs.h -- data types and declarations.
   Copyright (C) 1990, 1991, 1992, 1993, 1994, 2000, 2004, 2005, 2006,
   2007, 2008, 2009, 2010, 2011 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef INC_DEFS_H
#define INC_DEFS_H 1

#if !defined ALREADY_INCLUDED_CONFIG_H
/*
 * Savannah bug #20128: if we include some system header and it
 * includes some other second system header, the second system header
 * may in fact turn out to be a file provided by gnulib.  For that
 * situation, we need to have already included <config.h> so that the
 * Gnulib files have access to the information probed by their
 * configure script fragments.  So <config.h> should be the first
 * thing included.
 */
#error "<config.h> should be #included before defs.h, and indeed before any other header"
Please stop compiling the program now
#endif


#include <sys/types.h>

/* XXX: some of these includes probably don't belong in a common header file */
#include <sys/stat.h>
#include <stdio.h>		/* for FILE* */
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>		/* for CHAR_BIT */
#include <stdbool.h>		/* for bool */
#include <stdint.h>		/* for uintmax_t */
#include <sys/stat.h> /* S_ISUID etc. */
#include <selinux/selinux.h>



#ifndef CHAR_BIT
# define CHAR_BIT 8
#endif

# include <inttypes.h>

#include "regex.h"
#include "timespec.h"
#include "buildcmd.h"
#include "quotearg.h"
#include "sharefile.h"

#ifndef ATTRIBUTE_NORETURN
# if HAVE_ATTRIBUTE_NORETURN
#  define ATTRIBUTE_NORETURN __attribute__ ((__noreturn__))
# else
#  define ATTRIBUTE_NORETURN /* nothing */
# endif
#endif

int optionl_stat (const char *name, struct stat *p);
int optionp_stat (const char *name, struct stat *p);
int optionh_stat (const char *name, struct stat *p);
int debug_stat   (const char *file, struct stat *bufp);

void set_stat_placeholders (struct stat *p);
int get_statinfo (const char *pathname, const char *name, struct stat *p);


#define MODE_WXUSR	(S_IWUSR | S_IXUSR)
#define MODE_R		(S_IRUSR | S_IRGRP | S_IROTH)
#define MODE_RW		(S_IWUSR | S_IWGRP | S_IWOTH | MODE_R)
#define MODE_RWX	(S_IXUSR | S_IXGRP | S_IXOTH | MODE_RW)
#define MODE_ALL	(S_ISUID | S_ISGID | S_ISVTX | MODE_RWX)


struct predicate;
struct options;

/* Pointer to a predicate function. */
typedef bool (*PRED_FUNC)(const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr);

/* The number of seconds in a day. */
#define		DAYSECS	    86400

/* Argument structures for predicates. */

enum comparison_type
{
  COMP_GT,
  COMP_LT,
  COMP_EQ
};

enum permissions_type
{
  PERM_AT_LEAST,
  PERM_ANY,
  PERM_EXACT
};

enum predicate_type
{
  NO_TYPE,
  PRIMARY_TYPE,
  UNI_OP,
  BI_OP,
  OPEN_PAREN,
  CLOSE_PAREN
};

enum predicate_precedence
{
  NO_PREC,
  COMMA_PREC,
  OR_PREC,
  AND_PREC,
  NEGATE_PREC,
  MAX_PREC
};

struct long_val
{
  enum comparison_type kind;
  bool negative;	 /* Defined only when representing time_t.  */
  uintmax_t l_val;
};

struct perm_val
{
  enum permissions_type kind;
  mode_t val[2];
};

/* dir_id is used to support loop detection in oldfind.c
 */
struct dir_id
{
  ino_t ino;
  dev_t dev;
};

/* samefile_file_id is used to support the -samefile test.
 */
struct samefile_file_id
{
  ino_t ino;
  dev_t dev;
  int   fd;
};

struct size_val
{
  enum comparison_type kind;
  int blocksize;
  uintmax_t size;
};


enum xval
  {
    XVAL_ATIME, XVAL_BIRTHTIME, XVAL_CTIME, XVAL_MTIME, XVAL_TIME
  };

struct time_val
{
  enum xval            xval;
  enum comparison_type kind;
  struct timespec      ts;
};


struct exec_val
{
  bool multiple;		/* -exec {} \+ denotes multiple argument. */
  struct buildcmd_control ctl;
  struct buildcmd_state   state;
  char **replace_vec;		/* Command arguments (for ";" style) */
  int num_args;
  bool close_stdin;		/* If true, close stdin in the child. */
  struct saved_cwd *wd_for_exec; /* What directory to perform the exec in. */
  int last_child_status;	/* Status of the most recent child. */
};

/* The format string for a -printf or -fprintf is chopped into one or
   more `struct segment', linked together into a list.
   Each stretch of plain text is a segment, and
   each \c and `%' conversion is a segment. */

/* Special values for the `kind' field of `struct segment'. */
enum SegmentKind
  {
    KIND_PLAIN=0,		/* Segment containing just plain text. */
    KIND_STOP=1,		/* \c -- stop printing and flush output. */
    KIND_FORMAT,		/* Regular format */
  };

struct segment
{
  enum SegmentKind segkind;     /* KIND_FORMAT, KIND_PLAIN, KIND_STOP */
  char format_char[2];		/* Format chars if kind is KIND_FORMAT */
  char *text;			/* Plain text or `%' format string. */
  int text_len;			/* Length of `text'. */
  struct segment *next;		/* Next segment for this predicate. */
};

struct format_val
{
  struct segment *segment;	/* Linked list of segments. */
  FILE *stream;			/* Output stream to print on. */
  const char *filename;		/* We need the filename for error messages. */
  bool dest_is_tty;		/* True if the destination is a terminal. */
  struct quoting_options *quote_opts;
};

/* Profiling information for a predicate */
struct predicate_performance_info
{
  unsigned long visits;
  unsigned long successes;
};

/* evaluation cost of a predicate */
enum EvaluationCost
{
  NeedsNothing,
  NeedsInodeNumber,
  NeedsType,
  NeedsStatInfo,
  NeedsLinkName,
  NeedsAccessInfo,
  NeedsSyncDiskHit,
  NeedsEventualExec,
  NeedsImmediateExec,
  NeedsUserInteraction,
  NeedsUnknown,
  NumEvaluationCosts
};

struct predicate
{
  /* Pointer to the function that implements this predicate.  */
  PRED_FUNC pred_func;

  /* Only used for debugging, but defined unconditionally so individual
     modules can be compiled with -DDEBUG.  */
  const char *p_name;

  /* The type of this node.  There are two kinds.  The first is real
     predicates ("primaries") such as -perm, -print, or -exec.  The
     other kind is operators for combining predicates. */
  enum predicate_type p_type;

  /* The precedence of this node.  Only has meaning for operators. */
  enum predicate_precedence p_prec;

  /* True if this predicate node produces side effects.
     If side_effects are produced
     then optimization will not be performed */
  bool side_effects;

  /* True if this predicate node requires default print be turned off. */
  bool no_default_print;

  /* True if this predicate node requires a stat system call to execute. */
  bool need_stat;

  /* True if this predicate node requires knowledge of the file type. */
  bool need_type;

  /* True if this predicate node requires knowledge of the inode number. */
  bool need_inum;

  enum EvaluationCost p_cost;

  /* est_success_rate is a number between 0.0 and 1.0 */
  float est_success_rate;

  /* True if this predicate should display control characters literally */
  bool literal_control_chars;

  /* True if this predicate didn't originate from the user. */
  bool artificial;

  /* The raw text of the argument of this predicate. */
  const char *arg_text;

  /* Information needed by the predicate processor.
     Next to each member are listed the predicates that use it. */
  union
  {
    const char *str;		/* fstype [i]lname [i]name [i]path */
    struct re_pattern_buffer *regex; /* regex */
    struct exec_val exec_vec;	/* exec ok */
    struct long_val numinfo;	/* gid inum links  uid */
    struct size_val size;	/* size */
    uid_t uid;			/* user */
    gid_t gid;			/* group */
    struct time_val reftime;	/* newer newerXY anewer cnewer mtime atime ctime mmin amin cmin */
    struct perm_val perm;	/* perm */
    struct samefile_file_id samefileid; /* samefile */
    mode_t type;		/* type */
    struct format_val printf_vec; /* printf fprintf fprint ls fls print0 fprint0 print */
    security_context_t scontext; /* security context */
  } args;

  /* The next predicate in the user input sequence,
     which represents the order in which the user supplied the
     predicates on the command line. */
  struct predicate *pred_next;

  /* The right and left branches from this node in the expression
     tree, which represents the order in which the nodes should be
     processed. */
  struct predicate *pred_left;
  struct predicate *pred_right;

  struct predicate_performance_info perf;

  const struct parser_table* parser_entry;
};

/* oldfind.c, ftsfind.c */
bool is_fts_enabled(int *ftsoptions);

/* find library function declarations.  */

/* find global function declarations.  */

/* oldfind.c */
/* SymlinkOption represents the choice of
 * -P, -L or -P (default) on the command line.
 */
enum SymlinkOption
  {
    SYMLINK_NEVER_DEREF,	/* Option -P */
    SYMLINK_ALWAYS_DEREF,	/* Option -L */
    SYMLINK_DEREF_ARGSONLY	/* Option -H */
  };
extern enum SymlinkOption symlink_handling; /* defined in oldfind.c. */

void set_follow_state (enum SymlinkOption opt);
void cleanup(void);

/* fstype.c */
char *filesystem_type (const struct stat *statp, const char *path);
char * get_mounted_filesystems (void);
dev_t * get_mounted_devices (size_t *);



enum arg_type
  {
    ARG_OPTION,			/* regular options like -maxdepth */
    ARG_NOOP,			/* does nothing, returns true, internal use only */
    ARG_POSITIONAL_OPTION,	/* options whose position is important (-follow) */
    ARG_TEST,			/* a like -name */
    ARG_SPECIAL_PARSE,		/* complex to parse, don't eat the test name before calling parse_xx(). */
    ARG_PUNCTUATION,		/* like -o or ( */
    ARG_ACTION			/* like -print */
  };


struct parser_table;
/* Pointer to a parser function. */
typedef bool (*PARSE_FUNC)(const struct parser_table *p,
			   char *argv[], int *arg_ptr);
struct parser_table
{
  enum arg_type type;
  const char *parser_name;
  PARSE_FUNC parser_func;
  PRED_FUNC    pred_func;
};

/* parser.c */
const struct parser_table* find_parser (const char *search_name);
bool parse_print (const struct parser_table*, char *argv[], int *arg_ptr);
void pred_sanity_check (const struct predicate *predicates);
void check_option_combinations (const struct predicate *p);
void parse_begin_user_args (char **args, int argno, const struct predicate *last, const struct predicate *predicates);
void parse_end_user_args (char **args, int argno, const struct predicate *last, const struct predicate *predicates);
bool parse_openparen  (const struct parser_table* entry, char *argv[], int *arg_ptr);
bool parse_closeparen (const struct parser_table* entry, char *argv[], int *arg_ptr);

/* pred.c */

typedef bool PREDICATEFUNCTION(const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr);
PREDICATEFUNCTION pred_amin;
PREDICATEFUNCTION pred_and;
PREDICATEFUNCTION pred_anewer;
PREDICATEFUNCTION pred_atime;
PREDICATEFUNCTION pred_closeparen;
PREDICATEFUNCTION pred_cmin;
PREDICATEFUNCTION pred_cnewer;
PREDICATEFUNCTION pred_comma;
PREDICATEFUNCTION pred_ctime;
PREDICATEFUNCTION pred_delete;
PREDICATEFUNCTION pred_empty;
PREDICATEFUNCTION pred_exec;
PREDICATEFUNCTION pred_execdir;
PREDICATEFUNCTION pred_executable;
PREDICATEFUNCTION pred_false;
PREDICATEFUNCTION pred_fls;
PREDICATEFUNCTION pred_fprint;
PREDICATEFUNCTION pred_fprint0;
PREDICATEFUNCTION pred_fprintf;
PREDICATEFUNCTION pred_fstype;
PREDICATEFUNCTION pred_gid;
PREDICATEFUNCTION pred_group;
PREDICATEFUNCTION pred_ilname;
PREDICATEFUNCTION pred_iname;
PREDICATEFUNCTION pred_inum;
PREDICATEFUNCTION pred_ipath;
PREDICATEFUNCTION pred_links;
PREDICATEFUNCTION pred_lname;
PREDICATEFUNCTION pred_ls;
PREDICATEFUNCTION pred_mmin;
PREDICATEFUNCTION pred_mtime;
PREDICATEFUNCTION pred_name;
PREDICATEFUNCTION pred_negate;
PREDICATEFUNCTION pred_newer;
PREDICATEFUNCTION pred_newerXY;
PREDICATEFUNCTION pred_nogroup;
PREDICATEFUNCTION pred_nouser;
PREDICATEFUNCTION pred_ok;
PREDICATEFUNCTION pred_okdir;
PREDICATEFUNCTION pred_openparen;
PREDICATEFUNCTION pred_or;
PREDICATEFUNCTION pred_path;
PREDICATEFUNCTION pred_perm;
PREDICATEFUNCTION pred_print;
PREDICATEFUNCTION pred_print0;
PREDICATEFUNCTION pred_prune;
PREDICATEFUNCTION pred_quit;
PREDICATEFUNCTION pred_readable;
PREDICATEFUNCTION pred_regex;
PREDICATEFUNCTION pred_samefile;
PREDICATEFUNCTION pred_size;
PREDICATEFUNCTION pred_true;
PREDICATEFUNCTION pred_type;
PREDICATEFUNCTION pred_uid;
PREDICATEFUNCTION pred_used;
PREDICATEFUNCTION pred_user;
PREDICATEFUNCTION pred_writable;
PREDICATEFUNCTION pred_xtype;
PREDICATEFUNCTION pred_context;



char *find_pred_name (PRED_FUNC pred_func);


void print_predicate (FILE *fp, const struct predicate *p);
void print_tree (FILE*, struct predicate *node, int indent);
void print_list (FILE*, struct predicate *node);
void print_optlist (FILE *fp, const struct predicate *node);
void show_success_rates(const struct predicate *node);


/* tree.c */
bool matches_start_point(const char * glob, bool foldcase);
struct predicate * build_expression_tree (int argc, char *argv[], int end_of_leading_options);
struct predicate * get_eval_tree (void);
struct predicate *get_new_pred_noarg (const struct parser_table *entry);
struct predicate *get_new_pred (const struct parser_table *entry);
struct predicate *get_new_pred_chk_op (const struct parser_table *entry,
					      const char *arg);
float  calculate_derived_rates (struct predicate *p);

/* util.c */
bool fd_leak_check_is_enabled (void);
struct predicate *insert_primary (const struct parser_table *entry, const char *arg);
struct predicate *insert_primary_noarg (const struct parser_table *entry);
struct predicate *insert_primary_withpred (const struct parser_table *entry, PRED_FUNC fptr, const char *arg);
void usage (FILE *fp, int status, char *msg);
extern bool check_nofollow(void);
void complete_pending_execs(struct predicate *p);
void complete_pending_execdirs (void);
const char *safely_quote_err_filename (int n, char const *arg);
void record_initial_cwd (void);
bool is_exec_in_local_dir(const PRED_FUNC pred_func);

void fatal_target_file_error (int errno_value, const char *name) ATTRIBUTE_NORETURN;
void fatal_nontarget_file_error (int errno_value, const char *name) ATTRIBUTE_NORETURN;
void nonfatal_target_file_error (int errno_value, const char *name);
void nonfatal_nontarget_file_error (int errno_value, const char *name);


int process_leading_options (int argc, char *argv[]);
void set_option_defaults (struct options *p);
void error_severity (int level);

#if 0
#define apply_predicate(pathname, stat_buf_ptr, node)	\
  (*(node)->pred_func)((pathname), (stat_buf_ptr), (node))
#else
bool apply_predicate(const char *pathname, struct stat *stat_buf, struct predicate *p);
#endif

#define pred_is(node, fn) ( ((node)->pred_func) == (fn) )


/* oldfind.c. */
int get_info (const char *pathname, struct stat *p, struct predicate *pred_ptr);
bool following_links (void);
bool digest_mode (mode_t *mode, const char *pathname, const char *name, struct stat *pstat, bool leaf);
bool default_prints (struct predicate *pred);
bool looks_like_expression (const char *arg, bool leading);


enum DebugOption
  {
    DebugNone             = 0,
    DebugExpressionTree   = 1,
    DebugStat             = 2,
    DebugSearch           = 4,
    DebugTreeOpt          = 8,
    DebugHelp             = 16,
    DebugExec             = 32,
    DebugSuccessRates     = 64
  };

struct options
{
  /* If true, process directory before contents.  True unless -depth given. */
  bool do_dir_first;
  /* If true, -depth was EXPLICITLY set (as opposed to having been turned
   * on by -delete, for example).
   */
   bool explicit_depth;

  /* If >=0, don't descend more than this many levels of subdirectories. */
  int maxdepth;

  /* If >=0, don't process files above this level. */
  int mindepth;

  /* If true, do not assume that files in directories with nlink == 2
     are non-directories. */
  bool no_leaf_check;

  /* If true, don't cross filesystem boundaries. */
  bool stay_on_filesystem;

  /* If true, we ignore the problem where we find that a directory entry
   * no longer exists by the time we get around to processing it.
   */
  bool ignore_readdir_race;

  /* If true, pass control characters through.  If false, escape them
   * or turn them into harmless things.
   */
  bool literal_control_chars;

  /* If true, we issue warning messages
   */
  bool warnings;

  /* If true, avoid POSIX-incompatible behaviours
   * (this functionality is currently incomplete
   * and at the moment affects mainly warning messages).
   */
  bool posixly_correct;

  struct timespec      start_time;		/* Time at start of execution.  */

  /* Either one day before now (the default), or the start of today (if -daystart is given). */
  struct timespec      cur_day_start;

  /* If true, cur_day_start has been adjusted to the start of the day. */
  bool full_days;

  int output_block_size;	/* Output block size.  */

  /* bitmask for debug options */
  unsigned long debug_options;

  enum SymlinkOption symlink_handling;


  /* Pointer to the function used to stat files. */
  int (*xstat) (const char *name, struct stat *statbuf);


  /* Indicate if we can implement safely_chdir() using the O_NOFOLLOW
   * flag to open(2).
   */
  bool open_nofollow_available;

  /* The variety of regular expression that we support.
   * The default is POSIX Basic Regular Expressions, but this
   * can be changed with the positional option, -regextype.
   */
  int regex_options;

  /* function used to get file context */
  int (*x_getfilecon) (int, const char *, security_context_t *);

  /* Optimisation level.  One is the default.
   */
  unsigned short optimisation_level;


  /* How should we quote filenames in error messages and so forth?
   */
  enum quoting_style err_quoting_style;
};


struct state
{
  /* Current depth; 0 means current path is a command line arg. */
  int curdepth;

  /* If true, we have called stat on the current path. */
  bool have_stat;

  /* If true, we know the type of the current path. */
  bool have_type;
  mode_t type;			/* this is the actual type */

  /* The file being operated on, relative to the current directory.
     Used for stat, readlink, remove, and opendir.  */
  char *rel_pathname;
  /* The directory fd to which rel_pathname is relative.  This is relevant
   * when we're navigating the hierarchy with fts() and using FTS_CWDFD.
   */
  int cwd_dir_fd;

  /* Length of starting path. */
  int starting_path_length;

  /* If true, don't descend past current directory.
     Can be set by -prune, -maxdepth, and -xdev/-mount. */
  bool stop_at_current_level;

  /* Status value to return to system. */
  int exit_status;

  /* True if there are any execdirs.  This saves us a pair of fchdir()
   * calls for every directory we leave if it is false.  This is just
   * an optimisation.  Set to true if you want to be conservative.
   */
  bool execdirs_outstanding;

  /* Shared files, opened via the interface in sharefile.h. */
  sharefile_handle shared_files;

  /* Avoid multiple error messages for the same file. */
  bool already_issued_stat_error_msg;
};

/* exec.c */
bool impl_pred_exec (const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr);
int launch (struct buildcmd_control *ctl, void *usercontext, int argc, char **argv);

/* finddata.c */
extern struct options options;
extern struct state state;
extern struct saved_cwd *initial_wd;

#endif
