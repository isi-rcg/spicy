/* ANSI-C code produced by gperf version 3.1 */
/* Command-line: gperf  */
/* Computed positions: -k'1-2' */

#if !((' ' == 32) && ('!' == 33) && ('"' == 34) && ('#' == 35) \
      && ('%' == 37) && ('&' == 38) && ('\'' == 39) && ('(' == 40) \
      && (')' == 41) && ('*' == 42) && ('+' == 43) && (',' == 44) \
      && ('-' == 45) && ('.' == 46) && ('/' == 47) && ('0' == 48) \
      && ('1' == 49) && ('2' == 50) && ('3' == 51) && ('4' == 52) \
      && ('5' == 53) && ('6' == 54) && ('7' == 55) && ('8' == 56) \
      && ('9' == 57) && (':' == 58) && (';' == 59) && ('<' == 60) \
      && ('=' == 61) && ('>' == 62) && ('?' == 63) && ('A' == 65) \
      && ('B' == 66) && ('C' == 67) && ('D' == 68) && ('E' == 69) \
      && ('F' == 70) && ('G' == 71) && ('H' == 72) && ('I' == 73) \
      && ('J' == 74) && ('K' == 75) && ('L' == 76) && ('M' == 77) \
      && ('N' == 78) && ('O' == 79) && ('P' == 80) && ('Q' == 81) \
      && ('R' == 82) && ('S' == 83) && ('T' == 84) && ('U' == 85) \
      && ('V' == 86) && ('W' == 87) && ('X' == 88) && ('Y' == 89) \
      && ('Z' == 90) && ('[' == 91) && ('\\' == 92) && (']' == 93) \
      && ('^' == 94) && ('_' == 95) && ('a' == 97) && ('b' == 98) \
      && ('c' == 99) && ('d' == 100) && ('e' == 101) && ('f' == 102) \
      && ('g' == 103) && ('h' == 104) && ('i' == 105) && ('j' == 106) \
      && ('k' == 107) && ('l' == 108) && ('m' == 109) && ('n' == 110) \
      && ('o' == 111) && ('p' == 112) && ('q' == 113) && ('r' == 114) \
      && ('s' == 115) && ('t' == 116) && ('u' == 117) && ('v' == 118) \
      && ('w' == 119) && ('x' == 120) && ('y' == 121) && ('z' == 122) \
      && ('{' == 123) && ('|' == 124) && ('}' == 125) && ('~' == 126))
/* The character set is not based on ISO-646.  */
#warning "gperf generated tables don't work with this execution character set. Please report a bug to <bug-gperf@gnu.org>."
#endif

#include "confitems.h"
#include "conf.h"

#undef bool
#define ITEM_ENTRY(name, type, verify_fn) \
	offsetof(struct conf, name), confitem_parse_ ## type, \
	confitem_format_ ## type, verify_fn
#define ITEM(name, type) \
	ITEM_ENTRY(name, type, NULL)
#define ITEM_V(name, type, verification) \
	ITEM_ENTRY(name, type, confitem_verify_ ## verification)
struct conf_item;
/* maximum key range = 46, duplicates = 0 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
confitems_hash (register const char *str, register size_t len)
{
  static const unsigned char asso_values[] =
    {
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50,  0, 35,  0,
       5, 10, 50,  0, 30, 20, 50,  0, 10, 20,
      15,  0,  0, 50,  5, 10, 10, 15, 50, 50,
      20, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50, 50, 50, 50, 50,
      50, 50, 50, 50, 50, 50
    };
  return len + asso_values[(unsigned char)str[1]] + asso_values[(unsigned char)str[0]];
}

const struct conf_item *
confitems_get (register const char *str, register size_t len)
{
  enum
    {
      TOTAL_KEYWORDS = 34,
      MIN_WORD_LENGTH = 4,
      MAX_WORD_LENGTH = 26,
      MIN_HASH_VALUE = 4,
      MAX_HASH_VALUE = 49
    };

  static const struct conf_item wordlist[] =
    {
      {"",0,0,NULL,NULL,NULL}, {"",0,0,NULL,NULL,NULL},
      {"",0,0,NULL,NULL,NULL}, {"",0,0,NULL,NULL,NULL},
      {"path",                       21, ITEM(path, env_string)},
      {"",0,0,NULL,NULL,NULL}, {"",0,0,NULL,NULL,NULL},
      {"",0,0,NULL,NULL,NULL},
      {"compiler",                   3, ITEM(compiler, string)},
      {"cache_dir",                  1, ITEM(cache_dir, env_string)},
      {"",0,0,NULL,NULL,NULL},
      {"compression",                5, ITEM(compression, bool)},
      {"",0,0,NULL,NULL,NULL},
      {"cpp_extension",              7, ITEM(cpp_extension, string)},
      {"compiler_check",             4, ITEM(compiler_check, string)},
      {"",0,0,NULL,NULL,NULL},
      {"cache_dir_levels",           2, ITEM_V(cache_dir_levels, unsigned, dir_levels)},
      {"compression_level",          6, ITEM(compression_level, unsigned)},
      {"log_file",                   18, ITEM(log_file, env_string)},
      {"prefix_command",             23, ITEM(prefix_command, env_string)},
      {"debug",                      8, ITEM(debug, bool)},
      {"pch_external_checksum",      22, ITEM(pch_external_checksum, bool)},
      {"recache",                    27, ITEM(recache, bool)},
      {"prefix_command_cpp",         24, ITEM(prefix_command_cpp, env_string)},
      {"read_only",                  25, ITEM(read_only, bool)},
      {"stats",                      30, ITEM(stats, bool)},
      {"depend_mode",                9, ITEM(depend_mode, bool)},
      {"keep_comments_cpp",          16, ITEM(keep_comments_cpp, bool)},
      {"max_size",                   20, ITEM(max_size, size)},
      {"max_files",                  19, ITEM(max_files, unsigned)},
      {"sloppiness",                 29, ITEM(sloppiness, sloppiness)},
      {"read_only_direct",           26, ITEM(read_only_direct, bool)},
      {"disable",                    11, ITEM(disable, bool)},
      {"temporary_dir",              31, ITEM(temporary_dir, env_string)},
      {"run_second_cpp",             28, ITEM(run_second_cpp, bool)},
      {"unify",                      33, ITEM(unify, bool)},
      {"direct_mode",                10, ITEM(direct_mode, bool)},
      {"",0,0,NULL,NULL,NULL},
      {"hash_dir",                   14, ITEM(hash_dir, bool)},
      {"hard_link",                  13, ITEM(hard_link, bool)},
      {"umask",                      32, ITEM(umask, umask)},
      {"",0,0,NULL,NULL,NULL}, {"",0,0,NULL,NULL,NULL},
      {"base_dir",                   0, ITEM_V(base_dir, env_string, absolute_path)},
      {"limit_multiple",             17, ITEM(limit_multiple, double)},
      {"",0,0,NULL,NULL,NULL},
      {"ignore_headers_in_manifest", 15, ITEM(ignore_headers_in_manifest, env_string)},
      {"",0,0,NULL,NULL,NULL}, {"",0,0,NULL,NULL,NULL},
      {"extra_files_to_hash",        12, ITEM(extra_files_to_hash, env_string)}
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register unsigned int key = confitems_hash (str, len);

      if (key <= MAX_HASH_VALUE)
        {
          register const char *s = wordlist[key].name;

          if (*str == *s && !strcmp (str + 1, s + 1))
            return &wordlist[key];
        }
    }
  return 0;
}
size_t confitems_count(void) { return 34; }
