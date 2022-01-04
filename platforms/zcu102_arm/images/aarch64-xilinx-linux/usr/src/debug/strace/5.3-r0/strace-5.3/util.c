/*
 * Copyright (c) 1991, 1992 Paul Kranenburg <pk@cs.few.eur.nl>
 * Copyright (c) 1993 Branko Lankester <branko@hacktic.nl>
 * Copyright (c) 1993, 1994, 1995, 1996 Rick Sladkey <jrs@world.std.com>
 * Copyright (c) 1996-1999 Wichert Akkerman <wichert@cistron.nl>
 * Copyright (c) 1999 IBM Deutschland Entwicklung GmbH, IBM Corporation
 *                     Linux for s390 port by D.J. Barrow
 *                    <barrow_dj@mail.yahoo.com,djbarrow@de.ibm.com>
 * Copyright (c) 1999-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "defs.h"
#include <limits.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#ifdef HAVE_SYS_XATTR_H
# include <sys/xattr.h>
#endif
#include <sys/uio.h>

#include "largefile_wrappers.h"
#include "print_utils.h"
#include "static_assert.h"
#include "string_to_uint.h"
#include "xlat.h"
#include "xstring.h"

int
ts_nz(const struct timespec *a)
{
	return a->tv_sec || a->tv_nsec;
}

int
ts_cmp(const struct timespec *a, const struct timespec *b)
{
	if (a->tv_sec < b->tv_sec
	    || (a->tv_sec == b->tv_sec && a->tv_nsec < b->tv_nsec))
		return -1;
	if (a->tv_sec > b->tv_sec
	    || (a->tv_sec == b->tv_sec && a->tv_nsec > b->tv_nsec))
		return 1;
	return 0;
}

double
ts_float(const struct timespec *tv)
{
	return tv->tv_sec + tv->tv_nsec/1000000000.0;
}

void
ts_add(struct timespec *tv, const struct timespec *a, const struct timespec *b)
{
	tv->tv_sec = a->tv_sec + b->tv_sec;
	tv->tv_nsec = a->tv_nsec + b->tv_nsec;
	if (tv->tv_nsec >= 1000000000) {
		tv->tv_sec++;
		tv->tv_nsec -= 1000000000;
	}
}

void
ts_sub(struct timespec *tv, const struct timespec *a, const struct timespec *b)
{
	tv->tv_sec = a->tv_sec - b->tv_sec;
	tv->tv_nsec = a->tv_nsec - b->tv_nsec;
	if (tv->tv_nsec < 0) {
		tv->tv_sec--;
		tv->tv_nsec += 1000000000;
	}
}

void
ts_div(struct timespec *tv, const struct timespec *a, int n)
{
	long long nsec = (a->tv_sec % n * 1000000000LL + a->tv_nsec + n / 2) / n;
	tv->tv_sec = a->tv_sec / n + nsec / 1000000000;
	tv->tv_nsec = nsec % 1000000000;
}

void
ts_mul(struct timespec *tv, const struct timespec *a, int n)
{
	long long nsec = a->tv_nsec * n;
	tv->tv_sec = a->tv_sec * n + nsec / 1000000000;
	tv->tv_nsec = nsec % 1000000000;
}

const struct timespec *
ts_min(const struct timespec *a, const struct timespec *b)
{
	return ts_cmp(a, b) < 0 ? a : b;
}

const struct timespec *
ts_max(const struct timespec *a, const struct timespec *b)
{
	return ts_cmp(a, b) > 0 ? a : b;
}

int
parse_ts(const char *s, struct timespec *t)
{
	enum { NS_IN_S = 1000000000 };

	static const struct time_unit {
		const char *s;
		unsigned int mul;
	} units[] = {
		{ "",   1000 }, /* default is microseconds */
		{ "s",  1000000000 },
		{ "ms", 1000000 },
		{ "us", 1000 },
		{ "ns", 1 },
	};
	static const char float_accept[] =  "eE.-+0123456789";
	static const char int_accept[] = "+0123456789";

	size_t float_len = strspn(s, float_accept);
	size_t int_len = strspn(s, int_accept);
	const struct time_unit *unit = NULL;
	char *endptr = NULL;
	double float_val = -1;
	long long int_val = -1;

	if (float_len > int_len) {
		errno = 0;

		float_val = strtod(s, &endptr);

		if (endptr == s || errno)
			return -1;
		if (float_val < 0)
			return -1;
	} else {
		int_val = string_to_uint_ex(s, &endptr, LLONG_MAX, "smun");

		if (int_val < 0)
			return -1;
	}

	for (size_t i = 0; i < ARRAY_SIZE(units); i++) {
		if (strcmp(endptr, units[i].s))
			continue;

		unit = units + i;
		break;
	}

	if (!unit)
		return -1;

	if (float_len > int_len) {
		t->tv_sec = float_val / (NS_IN_S / unit->mul);
		t->tv_nsec = ((uint64_t) ((float_val -
					   (t->tv_sec * (NS_IN_S / unit->mul)))
					  * unit->mul)) % NS_IN_S;
	} else {
		t->tv_sec = int_val / (NS_IN_S / unit->mul);
		t->tv_nsec = (int_val % (NS_IN_S / unit->mul)) * unit->mul;
	}

	return 0;
}

#if !defined HAVE_STPCPY
char *
stpcpy(char *dst, const char *src)
{
	while ((*dst = *src++) != '\0')
		dst++;
	return dst;
}
#endif

/* Find a next bit which is set.
 * Starts testing at cur_bit.
 * Returns -1 if no more bits are set.
 *
 * We never touch bytes we don't need to.
 * On big-endian, array is assumed to consist of
 * current_wordsize wide words: for example, is current_wordsize is 4,
 * the bytes are walked in 3,2,1,0, 7,6,5,4, 11,10,9,8 ... sequence.
 * On little-endian machines, word size is immaterial.
 */
int
next_set_bit(const void *bit_array, unsigned cur_bit, unsigned size_bits)
{
	const unsigned endian = 1;
	int little_endian = *(char *) (void *) &endian;

	const uint8_t *array = bit_array;
	unsigned pos = cur_bit / 8;
	unsigned pos_xor_mask = little_endian ? 0 : current_wordsize-1;

	for (;;) {
		uint8_t bitmask;
		uint8_t cur_byte;

		if (cur_bit >= size_bits)
			return -1;
		cur_byte = array[pos ^ pos_xor_mask];
		if (cur_byte == 0) {
			cur_bit = (cur_bit + 8) & (-8);
			pos++;
			continue;
		}
		bitmask = 1 << (cur_bit & 7);
		for (;;) {
			if (cur_byte & bitmask)
				return cur_bit;
			cur_bit++;
			if (cur_bit >= size_bits)
				return -1;
			bitmask <<= 1;
			/* This check *can't be* optimized out: */
			if (bitmask == 0)
				break;
		}
		pos++;
	}
}

/*
 * Fetch 64bit argument at position arg_no and
 * return the index of the next argument.
 */
int
getllval(struct tcb *tcp, unsigned long long *val, int arg_no)
{
#if SIZEOF_KERNEL_LONG_T > 4
# ifndef current_klongsize
	if (current_klongsize < SIZEOF_KERNEL_LONG_T) {
#  if defined(AARCH64) || defined(POWERPC64) || defined(POWERPC64LE)
		/* Align arg_no to the next even number. */
		arg_no = (arg_no + 1) & 0xe;
#  endif /* AARCH64 || POWERPC64 || POWERPC64LE */
		*val = ULONG_LONG(tcp->u_arg[arg_no], tcp->u_arg[arg_no + 1]);
		arg_no += 2;
	} else
# endif /* !current_klongsize */
	{
		*val = tcp->u_arg[arg_no];
		arg_no++;
	}
#else /* SIZEOF_KERNEL_LONG_T == 4 */
# if defined __ARM_EABI__	\
  || defined LINUX_MIPSO32	\
  || defined POWERPC		\
  || defined XTENSA
	/* Align arg_no to the next even number. */
	arg_no = (arg_no + 1) & 0xe;
# elif defined SH
	/*
	 * The SH4 ABI does allow long longs in odd-numbered registers, but
	 * does not allow them to be split between registers and memory - and
	 * there are only four argument registers for normal functions.  As a
	 * result, pread, for example, takes an extra padding argument before
	 * the offset.  This was changed late in the 2.4 series (around 2.4.20).
	 */
	if (arg_no == 3)
		arg_no++;
# endif /* __ARM_EABI__ || LINUX_MIPSO32 || POWERPC || XTENSA || SH */
	*val = ULONG_LONG(tcp->u_arg[arg_no], tcp->u_arg[arg_no + 1]);
	arg_no += 2;
#endif

	return arg_no;
}

/*
 * Print 64bit argument at position arg_no and
 * return the index of the next argument.
 */
int
printllval(struct tcb *tcp, const char *format, int arg_no)
{
	unsigned long long val = 0;

	arg_no = getllval(tcp, &val, arg_no);
	tprintf(format, val);
	return arg_no;
}

void
printaddr64(const uint64_t addr)
{
	if (!addr)
		tprints("NULL");
	else
		tprintf("%#" PRIx64, addr);
}

#define DEF_PRINTNUM(name, type) \
bool									\
printnum_ ## name(struct tcb *const tcp, const kernel_ulong_t addr,	\
		  const char *const fmt)				\
{									\
	type num;							\
	if (umove_or_printaddr(tcp, addr, &num))			\
		return false;						\
	tprints("[");							\
	tprintf(fmt, num);						\
	tprints("]");							\
	return true;							\
}

#define DEF_PRINTNUM_ADDR(name, type) \
bool									\
printnum_addr_ ## name(struct tcb *tcp, const kernel_ulong_t addr)	\
{									\
	type num;							\
	if (umove_or_printaddr(tcp, addr, &num))			\
		return false;						\
	tprints("[");							\
	printaddr64(num);						\
	tprints("]");							\
	return true;							\
}

#define DEF_PRINTPAIR(name, type) \
bool									\
printpair_ ## name(struct tcb *const tcp, const kernel_ulong_t addr,	\
		   const char *const fmt)				\
{									\
	type pair[2];							\
	if (umove_or_printaddr(tcp, addr, &pair))			\
		return false;						\
	tprints("[");							\
	tprintf(fmt, pair[0]);						\
	tprints(", ");							\
	tprintf(fmt, pair[1]);						\
	tprints("]");							\
	return true;							\
}

DEF_PRINTNUM(int, int)
DEF_PRINTNUM_ADDR(int, unsigned int)
DEF_PRINTPAIR(int, int)
DEF_PRINTNUM(short, short)
DEF_PRINTNUM(int64, uint64_t)
DEF_PRINTNUM_ADDR(int64, uint64_t)
DEF_PRINTPAIR(int64, uint64_t)

bool
printnum_fd(struct tcb *const tcp, const kernel_ulong_t addr)
{
	int fd;
	if (umove_or_printaddr(tcp, addr, &fd))
		return false;
	tprints("[");
	printfd(tcp, fd);
	tprints("]");
	return true;
}

/**
 * Prints time to a (static internal) buffer and returns pointer to it.
 * Returns NULL if the provided time specification is not correct.
 *
 * @param sec		Seconds since epoch.
 * @param part_sec	Amount of second parts since the start of a second.
 * @param max_part_sec	Maximum value of a valid part_sec.
 * @param width		1 + floor(log10(max_part_sec)).
 * @return		Pointer to a statically allocated string on success,
 *			NULL on error.
 */
static const char *
sprinttime_ex(const long long sec, const unsigned long long part_sec,
	      const unsigned int max_part_sec, const int width)
{
	static char buf[sizeof(int) * 3 * 6 + sizeof(part_sec) * 3
			+ sizeof("+0000")];

	if ((sec == 0 && part_sec == 0) || part_sec > max_part_sec)
		return NULL;

	time_t t = (time_t) sec;
	struct tm *tmp = (sec == t) ? localtime(&t) : NULL;
	if (!tmp)
		return NULL;

	size_t pos = strftime(buf, sizeof(buf), "%FT%T", tmp);
	if (!pos)
		return NULL;

	if (part_sec > 0)
		pos += xsnprintf(buf + pos, sizeof(buf) - pos, ".%0*llu",
				 width, part_sec);

	return strftime(buf + pos, sizeof(buf) - pos, "%z", tmp) ? buf : NULL;
}

const char *
sprinttime(long long sec)
{
	return sprinttime_ex(sec, 0, 0, 0);
}

const char *
sprinttime_usec(long long sec, unsigned long long usec)
{
	return sprinttime_ex(sec, usec, 999999, 6);
}

const char *
sprinttime_nsec(long long sec, unsigned long long nsec)
{
	return sprinttime_ex(sec, nsec, 999999999, 9);
}

void
print_uuid(const unsigned char *uuid)
{
	const char str[] = {
		BYTE_HEX_CHARS(uuid[0]),
		BYTE_HEX_CHARS(uuid[1]),
		BYTE_HEX_CHARS(uuid[2]),
		BYTE_HEX_CHARS(uuid[3]),
		'-',
		BYTE_HEX_CHARS(uuid[4]),
		BYTE_HEX_CHARS(uuid[5]),
		'-',
		BYTE_HEX_CHARS(uuid[6]),
		BYTE_HEX_CHARS(uuid[7]),
		'-',
		BYTE_HEX_CHARS(uuid[8]),
		BYTE_HEX_CHARS(uuid[9]),
		'-',
		BYTE_HEX_CHARS(uuid[10]),
		BYTE_HEX_CHARS(uuid[11]),
		BYTE_HEX_CHARS(uuid[12]),
		BYTE_HEX_CHARS(uuid[13]),
		BYTE_HEX_CHARS(uuid[14]),
		BYTE_HEX_CHARS(uuid[15]),
		'\0'
	};

	tprints(str);
}

enum sock_proto
getfdproto(struct tcb *tcp, int fd)
{
#ifdef HAVE_SYS_XATTR_H
	size_t bufsize = 256;
	char buf[bufsize];
	ssize_t r;
	char path[sizeof("/proc/%u/fd/%u") + 2 * sizeof(int)*3];

	if (fd < 0)
		return SOCK_PROTO_UNKNOWN;

	xsprintf(path, "/proc/%u/fd/%u", tcp->pid, fd);
	r = getxattr(path, "system.sockprotoname", buf, bufsize - 1);
	if (r <= 0)
		return SOCK_PROTO_UNKNOWN;
	else {
		/*
		 * This is a protection for the case when the kernel
		 * side does not append a null byte to the buffer.
		 */
		buf[r] = '\0';

		return get_proto_by_name(buf);
	}
#else
	return SOCK_PROTO_UNKNOWN;
#endif
}

unsigned long
getfdinode(struct tcb *tcp, int fd)
{
	char path[PATH_MAX + 1];

	if (getfdpath(tcp, fd, path, sizeof(path)) >= 0) {
		const char *str = STR_STRIP_PREFIX(path, "socket:[");

		if (str != path) {
			const size_t str_len = strlen(str);
			if (str_len && str[str_len - 1] == ']')
				return strtoul(str, NULL, 10);
		}
	}

	return 0;
}

static bool
printsocket(struct tcb *tcp, int fd, const char *path)
{
	const char *str = STR_STRIP_PREFIX(path, "socket:[");
	size_t len;
	unsigned long inode;

	return (str != path)
		&& (len = strlen(str))
		&& (str[len - 1] == ']')
		&& (inode = strtoul(str, NULL, 10))
		&& print_sockaddr_by_inode(tcp, fd, inode);
}

static bool
printdev(struct tcb *tcp, int fd, const char *path)
{
	strace_stat_t st;

	if (path[0] != '/')
		return false;

	if (stat_file(path, &st)) {
		debug_func_perror_msg("stat(\"%s\")", path);
		return false;
	}

	switch (st.st_mode & S_IFMT) {
	case S_IFBLK:
	case S_IFCHR:
		print_quoted_string_ex(path, strlen(path),
				       QUOTE_OMIT_LEADING_TRAILING_QUOTES,
				       "<>");
		tprintf("<%s %u:%u>",
			S_ISBLK(st.st_mode)? "block" : "char",
			major(st.st_rdev), minor(st.st_rdev));
		return true;
	}

	return false;
}

void
printfd(struct tcb *tcp, int fd)
{
	char path[PATH_MAX + 1];
	if (show_fd_path && getfdpath(tcp, fd, path, sizeof(path)) >= 0) {
		tprintf("%d<", fd);
		if (show_fd_path <= 1
		    || (!printsocket(tcp, fd, path)
		         && !printdev(tcp, fd, path))) {
			print_quoted_string_ex(path, strlen(path),
				QUOTE_OMIT_LEADING_TRAILING_QUOTES, "<>");
		}
		tprints(">");
	} else
		tprintf("%d", fd);
}

/*
 * Quote string `instr' of length `size'
 * Write up to (3 + `size' * 4) bytes to `outstr' buffer.
 *
 * `escape_chars' specifies characters (in addition to characters with
 * codes 0..31, 127..255, single and double quotes) that should be escaped.
 *
 * If QUOTE_0_TERMINATED `style' flag is set,
 * treat `instr' as a NUL-terminated string,
 * checking up to (`size' + 1) bytes of `instr'.
 *
 * If QUOTE_OMIT_LEADING_TRAILING_QUOTES `style' flag is set,
 * do not add leading and trailing quoting symbols.
 *
 * Returns 0 if QUOTE_0_TERMINATED is set and NUL was seen, 1 otherwise.
 * Note that if QUOTE_0_TERMINATED is not set, always returns 1.
 */
int
string_quote(const char *instr, char *outstr, const unsigned int size,
	     const unsigned int style, const char *escape_chars)
{
	const unsigned char *ustr = (const unsigned char *) instr;
	char *s = outstr;
	unsigned int i;
	int usehex, c, eol;
	bool printable;

	if (style & QUOTE_0_TERMINATED)
		eol = '\0';
	else
		eol = 0x100; /* this can never match a char */

	usehex = 0;
	if ((xflag > 1) || (style & QUOTE_FORCE_HEX)) {
		usehex = 1;
	} else if (xflag) {
		/* Check for presence of symbol which require
		   to hex-quote the whole string. */
		for (i = 0; i < size; ++i) {
			c = ustr[i];
			/* Check for NUL-terminated string. */
			if (c == eol)
				break;

			/* Force hex unless c is printable or whitespace */
			if (c > 0x7e) {
				usehex = 1;
				break;
			}
			/* In ASCII isspace is only these chars: "\t\n\v\f\r".
			 * They happen to have ASCII codes 9,10,11,12,13.
			 */
			if (c < ' ' && (unsigned)(c - 9) >= 5) {
				usehex = 1;
				break;
			}
		}
	}

	if (style & QUOTE_EMIT_COMMENT)
		s = stpcpy(s, " /* ");
	if (!(style & QUOTE_OMIT_LEADING_TRAILING_QUOTES))
		*s++ = '\"';

	if (usehex) {
		/* Hex-quote the whole string. */
		for (i = 0; i < size; ++i) {
			c = ustr[i];
			/* Check for NUL-terminated string. */
			if (c == eol)
				goto asciz_ended;
			*s++ = '\\';
			*s++ = 'x';
			s = sprint_byte_hex(s, c);
		}

		goto string_ended;
	}

	for (i = 0; i < size; ++i) {
		c = ustr[i];
		/* Check for NUL-terminated string. */
		if (c == eol)
			goto asciz_ended;
		if ((i == (size - 1)) &&
		    (style & QUOTE_OMIT_TRAILING_0) && (c == '\0'))
			goto asciz_ended;
		switch (c) {
		case '\"': case '\\':
			*s++ = '\\';
			*s++ = c;
			break;
		case '\f':
			*s++ = '\\';
			*s++ = 'f';
			break;
		case '\n':
			*s++ = '\\';
			*s++ = 'n';
			break;
		case '\r':
			*s++ = '\\';
			*s++ = 'r';
			break;
		case '\t':
			*s++ = '\\';
			*s++ = 't';
			break;
		case '\v':
			*s++ = '\\';
			*s++ = 'v';
			break;
		default:
			printable = is_print(c);

			if (printable && escape_chars)
				printable = !strchr(escape_chars, c);

			if (printable) {
				*s++ = c;
			} else {
				/* Print \octal */
				*s++ = '\\';
				if (i + 1 < size
				    && ustr[i + 1] >= '0'
				    && ustr[i + 1] <= '7'
				) {
					/* Print \ooo */
					*s++ = '0' + (c >> 6);
					*s++ = '0' + ((c >> 3) & 0x7);
				} else {
					/* Print \[[o]o]o */
					if ((c >> 3) != 0) {
						if ((c >> 6) != 0)
							*s++ = '0' + (c >> 6);
						*s++ = '0' + ((c >> 3) & 0x7);
					}
				}
				*s++ = '0' + (c & 0x7);
			}
		}
	}

 string_ended:
	if (!(style & QUOTE_OMIT_LEADING_TRAILING_QUOTES))
		*s++ = '\"';
	if (style & QUOTE_EMIT_COMMENT)
		s = stpcpy(s, " */");
	*s = '\0';

	/* Return zero if we printed entire ASCIZ string (didn't truncate it) */
	if (style & QUOTE_0_TERMINATED && ustr[i] == '\0') {
		/* We didn't see NUL yet (otherwise we'd jump to 'asciz_ended')
		 * but next char is NUL.
		 */
		return 0;
	}

	return 1;

 asciz_ended:
	if (!(style & QUOTE_OMIT_LEADING_TRAILING_QUOTES))
		*s++ = '\"';
	if (style & QUOTE_EMIT_COMMENT)
		s = stpcpy(s, " */");
	*s = '\0';
	/* Return zero: we printed entire ASCIZ string (didn't truncate it) */
	return 0;
}

#ifndef ALLOCA_CUTOFF
# define ALLOCA_CUTOFF	4032
#endif
#define use_alloca(n) ((n) <= ALLOCA_CUTOFF)

/*
 * Quote string `str' of length `size' and print the result.
 *
 * If QUOTE_0_TERMINATED `style' flag is set,
 * treat `str' as a NUL-terminated string and
 * quote at most (`size' - 1) bytes.
 *
 * If QUOTE_OMIT_LEADING_TRAILING_QUOTES `style' flag is set,
 * do not add leading and trailing quoting symbols.
 *
 * Returns 0 if QUOTE_0_TERMINATED is set and NUL was seen, 1 otherwise.
 * Note that if QUOTE_0_TERMINATED is not set, always returns 1.
 */
int
print_quoted_string_ex(const char *str, unsigned int size,
		       const unsigned int style, const char *escape_chars)
{
	char *buf;
	char *outstr;
	unsigned int alloc_size;
	int rc;

	if (size && style & QUOTE_0_TERMINATED)
		--size;

	alloc_size = 4 * size;
	if (alloc_size / 4 != size) {
		error_func_msg("requested %u bytes exceeds %u bytes limit",
			       size, -1U / 4);
		tprints("???");
		return -1;
	}
	alloc_size += 1 + (style & QUOTE_OMIT_LEADING_TRAILING_QUOTES ? 0 : 2) +
		(style & QUOTE_EMIT_COMMENT ? 7 : 0);

	if (use_alloca(alloc_size)) {
		outstr = alloca(alloc_size);
		buf = NULL;
	} else {
		outstr = buf = malloc(alloc_size);
		if (!buf) {
			error_func_msg("memory exhausted when tried to allocate"
				       " %u bytes", alloc_size);
			tprints("???");
			return -1;
		}
	}

	rc = string_quote(str, outstr, size, style, escape_chars);
	tprints(outstr);

	free(buf);
	return rc;
}

inline int
print_quoted_string(const char *str, unsigned int size,
		    const unsigned int style)
{
	return print_quoted_string_ex(str, size, style, NULL);
}

/*
 * Quote a NUL-terminated string `str' of length up to `size' - 1
 * and print the result.
 *
 * Returns 0 if NUL was seen, 1 otherwise.
 */
int
print_quoted_cstring(const char *str, unsigned int size)
{
	int unterminated =
		print_quoted_string(str, size, QUOTE_0_TERMINATED);

	if (unterminated)
		tprints("...");

	return unterminated;
}

/*
 * Print path string specified by address `addr' and length `n'.
 * If path length exceeds `n', append `...' to the output.
 *
 * Returns the result of umovenstr.
 */
int
printpathn(struct tcb *const tcp, const kernel_ulong_t addr, unsigned int n)
{
	char path[PATH_MAX];
	int nul_seen;

	if (!addr) {
		tprints("NULL");
		return -1;
	}

	/* Cap path length to the path buffer size */
	if (n > sizeof(path) - 1)
		n = sizeof(path) - 1;

	/* Fetch one byte more to find out whether path length > n. */
	nul_seen = umovestr(tcp, addr, n + 1, path);
	if (nul_seen < 0)
		printaddr(addr);
	else {
		path[n++] = !nul_seen;
		print_quoted_cstring(path, n);
	}

	return nul_seen;
}

int
printpath(struct tcb *const tcp, const kernel_ulong_t addr)
{
	/* Size must correspond to char path[] size in printpathn */
	return printpathn(tcp, addr, PATH_MAX - 1);
}

/*
 * Print string specified by address `addr' and length `len'.
 * If `user_style' has QUOTE_0_TERMINATED bit set, treat the string
 * as a NUL-terminated string.
 * Pass `user_style' on to `string_quote'.
 * Append `...' to the output if either the string length exceeds `max_strlen',
 * or QUOTE_0_TERMINATED bit is set and the string length exceeds `len'.
 *
 * Returns the result of umovenstr if style has QUOTE_0_TERMINATED,
 * or the result of umoven otherwise.
 */
int
printstr_ex(struct tcb *const tcp, const kernel_ulong_t addr,
	    const kernel_ulong_t len, const unsigned int user_style)
{
	static char *str;
	static char *outstr;

	unsigned int size;
	unsigned int style = user_style;
	int rc;
	int ellipsis;

	if (!addr) {
		tprints("NULL");
		return -1;
	}
	/* Allocate static buffers if they are not allocated yet. */
	if (!str) {
		const unsigned int outstr_size =
			4 * max_strlen + /* for quotes and NUL */ 3;
		/*
		 * We can assume that outstr_size / 4 == max_strlen
		 * since we have a guarantee that max_strlen <= -1U / 4.
		 */

		str = xmalloc(max_strlen + 1);
		outstr = xmalloc(outstr_size);
	}

	/* Fetch one byte more because string_quote may look one byte ahead. */
	size = max_strlen + 1;

	if (size > len)
		size = len;
	if (style & QUOTE_0_TERMINATED)
		rc = umovestr(tcp, addr, size, str);
	else
		rc = umoven(tcp, addr, size, str);

	if (rc < 0) {
		printaddr(addr);
		return rc;
	}

	if (size > max_strlen)
		size = max_strlen;
	else
		str[size] = '\xff';

	/* If string_quote didn't see NUL and (it was supposed to be ASCIZ str
	 * or we were requested to print more than -s NUM chars)...
	 */
	ellipsis = string_quote(str, outstr, size, style, NULL)
		   && len
		   && ((style & QUOTE_0_TERMINATED)
		       || len > max_strlen);

	tprints(outstr);
	if (ellipsis)
		tprints("...");

	return rc;
}

void
dumpiov_upto(struct tcb *const tcp, const int len, const kernel_ulong_t addr,
	     kernel_ulong_t data_size)
{
#if ANY_WORDSIZE_LESS_THAN_KERNEL_LONG
	union {
		struct { uint32_t base; uint32_t len; } *iov32;
		struct { uint64_t base; uint64_t len; } *iov64;
	} iovu;
# define iov iovu.iov64
# define sizeof_iov \
	(current_wordsize == 4 ? (unsigned int) sizeof(*iovu.iov32)	\
			       : (unsigned int) sizeof(*iovu.iov64))
# define iov_iov_base(i) \
	(current_wordsize == 4 ? (uint64_t) iovu.iov32[i].base : iovu.iov64[i].base)
# define iov_iov_len(i) \
	(current_wordsize == 4 ? (uint64_t) iovu.iov32[i].len : iovu.iov64[i].len)
#else
	struct iovec *iov;
# define sizeof_iov ((unsigned int) sizeof(*iov))
# define iov_iov_base(i) ptr_to_kulong(iov[i].iov_base)
# define iov_iov_len(i) iov[i].iov_len
#endif
	int i;
	unsigned int size = sizeof_iov * len;
	if (size / sizeof_iov != (unsigned int) len) {
		error_func_msg("requested %u iovec elements exceeds"
			       " %u iovec limit", len, -1U / sizeof_iov);
		return;
	}

	iov = malloc(size);
	if (!iov) {
		error_func_msg("memory exhausted when tried to allocate"
			       " %u bytes", size);
		return;
	}
	if (umoven(tcp, addr, size, iov) >= 0) {
		for (i = 0; i < len; i++) {
			kernel_ulong_t iov_len = iov_iov_len(i);
			if (iov_len > data_size)
				iov_len = data_size;
			if (!iov_len)
				break;
			data_size -= iov_len;
			/* include the buffer number to make it easy to
			 * match up the trace with the source */
			tprintf(" * %" PRI_klu " bytes in buffer %d\n", iov_len, i);
			dumpstr(tcp, iov_iov_base(i), iov_len);
		}
	}
	free(iov);
#undef sizeof_iov
#undef iov_iov_base
#undef iov_iov_len
#undef iov
}

#define ILOG2_ITER_(val_, ret_, bit_)					\
	do {								\
		typeof(ret_) shift_ =					\
			((val_) > ((((typeof(val_)) 1)			\
				   << (1 << (bit_))) - 1)) << (bit_);	\
		(val_) >>= shift_;					\
		(ret_) |= shift_;					\
	} while (0)

/**
 * Calculate floor(log2(val)), with the exception of val == 0, for which 0
 * is returned as well.
 *
 * @param val 64-bit value to calculate integer base-2 logarithm for.
 * @return    (unsigned int) floor(log2(val)) if val > 0, 0 if val == 0.
 */
static inline unsigned int
ilog2_64(uint64_t val)
{
	unsigned int ret = 0;

	ILOG2_ITER_(val, ret, 5);
	ILOG2_ITER_(val, ret, 4);
	ILOG2_ITER_(val, ret, 3);
	ILOG2_ITER_(val, ret, 2);
	ILOG2_ITER_(val, ret, 1);
	ILOG2_ITER_(val, ret, 0);

	return ret;
}

/**
 * Calculate floor(log2(val)), with the exception of val == 0, for which 0
 * is returned as well.
 *
 * @param val 32-bit value to calculate integer base-2 logarithm for.
 * @return    (unsigned int) floor(log2(val)) if val > 0, 0 if val == 0.
 */
static inline unsigned int
ilog2_32(uint32_t val)
{
	unsigned int ret = 0;

	ILOG2_ITER_(val, ret, 4);
	ILOG2_ITER_(val, ret, 3);
	ILOG2_ITER_(val, ret, 2);
	ILOG2_ITER_(val, ret, 1);
	ILOG2_ITER_(val, ret, 0);

	return ret;
}

#undef ILOG2_ITER_

#if SIZEOF_KERNEL_LONG_T > 4
# define ilog2_klong ilog2_64
#else
# define ilog2_klong ilog2_32
#endif

void
dumpstr(struct tcb *const tcp, const kernel_ulong_t addr,
	const kernel_ulong_t len)
{
	/* xx xx xx xx xx xx xx xx  xx xx xx xx xx xx xx xx  1234567890123456 */
	enum {
		HEX_BIT = 4,

		DUMPSTR_GROUP_BYTES = 8,
		DUMPSTR_GROUPS = 2,
		DUMPSTR_WIDTH_BYTES = DUMPSTR_GROUP_BYTES * DUMPSTR_GROUPS,

		/** Width of formatted dump in characters.  */
		DUMPSTR_WIDTH_CHARS = DUMPSTR_WIDTH_BYTES +
			sizeof("xx") * DUMPSTR_WIDTH_BYTES + DUMPSTR_GROUPS,

		DUMPSTR_GROUP_MASK = DUMPSTR_GROUP_BYTES - 1,
		DUMPSTR_BYTES_MASK = DUMPSTR_WIDTH_BYTES - 1,

		/** Minimal width of the offset field in the output.  */
		DUMPSTR_OFFS_MIN_CHARS = 5,

		/** Arbitrarily chosen internal dumpstr buffer limit.  */
		DUMPSTR_BUF_MAXSZ = 1 << 16,
	};

	static_assert(!(DUMPSTR_BUF_MAXSZ % DUMPSTR_WIDTH_BYTES),
		      "Maximum internal buffer size should be divisible "
		      "by amount of bytes dumped per line");
	static_assert(!(DUMPSTR_GROUP_BYTES & DUMPSTR_GROUP_MASK),
		      "DUMPSTR_GROUP_BYTES is not power of 2");
	static_assert(!(DUMPSTR_WIDTH_BYTES & DUMPSTR_BYTES_MASK),
		      "DUMPSTR_WIDTH_BYTES is not power of 2");

	if (len > len + DUMPSTR_WIDTH_BYTES || addr + len < addr) {
		debug_func_msg("len %" PRI_klu " at addr %#" PRI_klx
			       " is too big, skipped", len, addr);
		return;
	}

	static kernel_ulong_t strsize;
	static unsigned char *str;

	const kernel_ulong_t alloc_size =
		MIN(ROUNDUP(len, DUMPSTR_WIDTH_BYTES), DUMPSTR_BUF_MAXSZ);

	if (strsize < alloc_size) {
		free(str);
		str = malloc(alloc_size);
		if (!str) {
			strsize = 0;
			error_func_msg("memory exhausted when tried to allocate"
				       " %" PRI_klu " bytes", alloc_size);
			return;
		}
		strsize = alloc_size;
	}

	/**
	 * Characters needed in order to print the offset field. We calculate
	 * it this way in order to avoid ilog2_64 call most of the time.
	 */
	const int offs_chars = len > (1 << (DUMPSTR_OFFS_MIN_CHARS * HEX_BIT))
		? 1 + ilog2_klong(len - 1) / HEX_BIT : DUMPSTR_OFFS_MIN_CHARS;
	kernel_ulong_t i = 0;
	const unsigned char *src;

	while (i < len) {
		/*
		 * It is important to overwrite all the byte values, as we
		 * re-use the buffer in order to avoid its re-initialisation.
		 */
		static char outbuf[] = {
			[0 ... DUMPSTR_WIDTH_CHARS - 1] = ' ',
			'\0'
		};
		char *dst = outbuf;

		/* Fetching data from tracee.  */
		if (!i || (i % DUMPSTR_BUF_MAXSZ) == 0) {
			kernel_ulong_t fetch_size = MIN(len - i, alloc_size);

			if (umoven(tcp, addr + i, fetch_size, str) < 0) {
				/*
				 * Don't silently abort if we have printed
				 * something already.
				 */
				if (i)
					tprintf(" | <Cannot fetch %" PRI_klu
						" byte%s from pid %d"
						" @%#" PRI_klx ">\n",
						fetch_size,
						fetch_size == 1 ? "" : "s",
						tcp->pid, addr + i);
				return;
			}
			src = str;
		}

		/* hex dump */
		do {
			if (i < len) {
				dst = sprint_byte_hex(dst, *src);
			} else {
				*dst++ = ' ';
				*dst++ = ' ';
			}
			dst++; /* space is there */
			i++;
			if ((i & DUMPSTR_GROUP_MASK) == 0)
				dst++; /* space is there */
			src++;
		} while (i & DUMPSTR_BYTES_MASK);

		/* ASCII dump */
		i -= DUMPSTR_WIDTH_BYTES;
		src -= DUMPSTR_WIDTH_BYTES;
		do {
			if (i < len) {
				if (is_print(*src))
					*dst++ = *src;
				else
					*dst++ = '.';
			} else {
				*dst++ = ' ';
			}
			src++;
		} while (++i & DUMPSTR_BYTES_MASK);

		tprintf(" | %0*" PRI_klx "  %s |\n",
			offs_chars, i - DUMPSTR_WIDTH_BYTES, outbuf);
	}
}

bool
tfetch_mem64(struct tcb *const tcp, const uint64_t addr,
	     const unsigned int len, void *const our_addr)
{
	return addr && verbose(tcp) &&
	       (entering(tcp) || !syserror(tcp)) &&
	       !umoven(tcp, addr, len, our_addr);
}

bool
tfetch_mem64_ignore_syserror(struct tcb *const tcp, const uint64_t addr,
			     const unsigned int len, void *const our_addr)
{
	return addr && verbose(tcp) &&
	       !umoven(tcp, addr, len, our_addr);
}

int
umoven_or_printaddr64(struct tcb *const tcp, const uint64_t addr,
		      const unsigned int len, void *const our_addr)
{
	if (tfetch_mem64(tcp, addr, len, our_addr))
		return 0;
	printaddr64(addr);
	return -1;
}

int
umoven_or_printaddr64_ignore_syserror(struct tcb *const tcp,
				      const uint64_t addr,
				      const unsigned int len,
				      void *const our_addr)
{
	if (tfetch_mem64_ignore_syserror(tcp, addr, len, our_addr))
		return 0;
	printaddr64(addr);
	return -1;
}

bool
print_int32_array_member(struct tcb *tcp, void *elem_buf, size_t elem_size,
			 void *data)
{
	tprintf("%" PRId32, *(int32_t *) elem_buf);

	return true;
}

bool
print_uint32_array_member(struct tcb *tcp, void *elem_buf, size_t elem_size,
			  void *data)
{
	tprintf("%" PRIu32, *(uint32_t *) elem_buf);

	return true;
}

bool
print_uint64_array_member(struct tcb *tcp, void *elem_buf, size_t elem_size,
			  void *data)
{
	tprintf("%" PRIu64, *(uint64_t *) elem_buf);

	return true;
}

bool
print_xint32_array_member(struct tcb *tcp, void *elem_buf, size_t elem_size,
			  void *data)
{
	tprintf("%#" PRIx32, *(uint32_t *) elem_buf);

	return true;
}

bool
print_xint64_array_member(struct tcb *tcp, void *elem_buf, size_t elem_size,
			  void *data)
{
	tprintf("%#" PRIx64, *(uint64_t *) elem_buf);

	return true;
}

/*
 * Iteratively fetch and print up to nmemb elements of elem_size size
 * from the array that starts at tracee's address start_addr.
 *
 * Array elements are being fetched to the address specified by elem_buf.
 *
 * The fetcher callback function specified by tfetch_mem_func should follow
 * the same semantics as tfetch_mem function.
 *
 * The printer callback function specified by print_func is expected
 * to print something; if it returns false, no more iterations will be made.
 *
 * The pointer specified by opaque_data is passed to each invocation
 * of print_func callback function.
 *
 * This function prints:
 * - "NULL", if start_addr is NULL;
 * - "[]", if nmemb is 0;
 * - start_addr, if nmemb * elem_size overflows or wraps around;
 * - start_addr, if the first tfetch_mem_func invocation returned false;
 * - elements of the array, delimited by ", ", with the array itself
 *   enclosed with [] brackets.
 *
 * If abbrev(tcp) is true, then
 * - the maximum number of elements printed equals to max_strlen;
 * - "..." is printed instead of max_strlen+1 element
 *   and no more iterations will be made.
 *
 * This function returns true only if tfetch_mem_func has returned true
 * at least once.
 */
bool
print_array_ex(struct tcb *const tcp,
	       const kernel_ulong_t start_addr,
	       const size_t nmemb,
	       void *elem_buf,
	       const size_t elem_size,
	       tfetch_mem_fn tfetch_mem_func,
	       print_fn print_func,
	       void *const opaque_data,
	       unsigned int flags,
	       const struct xlat *index_xlat,
	       const char *index_dflt)
{
	if (!start_addr) {
		tprints("NULL");
		return false;
	}

	if (!nmemb) {
		tprints("[]");
		return false;
	}

	const size_t size = nmemb * elem_size;
	const kernel_ulong_t end_addr = start_addr + size;

	if (end_addr <= start_addr || size / elem_size != nmemb) {
		if (tfetch_mem_func)
			printaddr(start_addr);
		else
			tprints("???");
		return false;
	}

	const kernel_ulong_t abbrev_end =
		(abbrev(tcp) && max_strlen < nmemb) ?
			start_addr + elem_size * max_strlen : end_addr;
	kernel_ulong_t cur;
	kernel_ulong_t idx = 0;
	enum xlat_style xlat_style = flags & XLAT_STYLE_MASK;

	for (cur = start_addr; cur < end_addr; cur += elem_size, idx++) {
		if (cur != start_addr)
			tprints(", ");

		if (tfetch_mem_func) {
			if (!tfetch_mem_func(tcp, cur, elem_size, elem_buf)) {
				if (cur == start_addr)
					printaddr(cur);
				else {
					tprints("...");
					printaddr_comment(cur);
				}
				break;
			}
		} else {
			elem_buf = (void *) (uintptr_t) cur;
		}

		if (cur == start_addr)
			tprints("[");

		if (cur >= abbrev_end) {
			tprints("...");
			cur = end_addr;
			break;
		}

		if (flags & PAF_PRINT_INDICES) {
			tprints("[");

			if (!index_xlat) {
				print_xlat_ex(idx, NULL, xlat_style);
			} else {
				printxval_ex(idx ? NULL : index_xlat, idx,
					     index_dflt, xlat_style);
			}

			tprints("] = ");
		}

		if (!print_func(tcp, elem_buf, elem_size, opaque_data)) {
			cur = end_addr;
			break;
		}
	}

	if ((cur != start_addr) || !tfetch_mem_func) {
		if (flags & PAF_ARRAY_TRUNCATED) {
			if (cur != start_addr)
				tprints(", ");

			tprints("...");
		}

		tprints("]");
	}

	return cur >= end_addr;
}

int
printargs(struct tcb *tcp)
{
	const int n = n_args(tcp);
	int i;
	for (i = 0; i < n; ++i)
		tprintf("%s%#" PRI_klx, i ? ", " : "", tcp->u_arg[i]);
	return RVAL_DECODED;
}

int
printargs_u(struct tcb *tcp)
{
	const int n = n_args(tcp);
	int i;
	for (i = 0; i < n; ++i)
		tprintf("%s%u", i ? ", " : "",
			(unsigned int) tcp->u_arg[i]);
	return RVAL_DECODED;
}

int
printargs_d(struct tcb *tcp)
{
	const int n = n_args(tcp);
	int i;
	for (i = 0; i < n; ++i)
		tprintf("%s%d", i ? ", " : "",
			(int) tcp->u_arg[i]);
	return RVAL_DECODED;
}

/* Print abnormal high bits of a kernel_ulong_t value. */
void
print_abnormal_hi(const kernel_ulong_t val)
{
	if (current_klongsize > 4) {
		const unsigned int hi = (unsigned int) ((uint64_t) val >> 32);
		if (hi)
			tprintf("%#x<<32|", hi);
	}
}

int
read_int_from_file(struct tcb *tcp, const char *const fname, int *const pvalue)
{
	const int fd = open_file(fname, O_RDONLY);
	if (fd < 0)
		return -1;

	long lval;
	char buf[sizeof(lval) * 3];
	int n = read(fd, buf, sizeof(buf) - 1);
	int saved_errno = errno;
	close(fd);

	if (n < 0) {
		errno = saved_errno;
		return -1;
	}

	buf[n] = '\0';
	char *endptr = 0;
	errno = 0;
	lval = strtol(buf, &endptr, 10);
	if (!endptr || (*endptr && '\n' != *endptr)
#if INT_MAX < LONG_MAX
	    || lval > INT_MAX || lval < INT_MIN
#endif
	    || ERANGE == errno) {
		if (!errno)
			errno = EINVAL;
		return -1;
	}

	*pvalue = (int) lval;
	return 0;
}
