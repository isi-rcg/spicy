/*
 * encodings.c: locale and encoding handling for man
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011
 *               Colin Watson.
 *
 * This file is part of man-db.
 *
 * man-db is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * man-db is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with man-db; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "gettext.h"
#include "localcharset.h"
#include <locale.h>
#include <ctype.h>

#include "manconfig.h"

#include "pathsearch.h"
#include "pipeline.h"

#include "encodings.h"


/* Due to historical limitations in groff (which may be removed in the
 * future), there is no mechanism for a man page to specify its own
 * encoding. This means that each national language directory needs to carry
 * with it information about its encoding, and each groff device needs to
 * have a default encoding associated with it. Out of the box, groff
 * formally allows only ISO-8859-1 on input; however, patches originating
 * with Debian and imported by many other GNU/Linux distributions change
 * this somewhat.
 *
 * Eventually, groff will support proper Unicode input, and much of this
 * horror can go away.
 *
 * Do *not* confuse source encoding with groff encoding. The encoding
 * specified in this table is the encoding in which the source man pages in
 * each language directory are expected to be written. The groff encoding is
 * determined by the selected groff device and sometimes also by the user's
 * locale.
 *
 * The standard output encoding is the encoding assumed for cat pages for
 * each language directory. It must *not* be used to discover the actual
 * output encoding displayed to the user; that is determined by the locale.
 * TODO: it would be useful to be able to change the standard output
 * encoding in the configuration file.
 *
 * This table is expected to change over time, particularly as man pages
 * begin to move towards UTF-8. Feel free to patch this for your
 * distribution; send me updates for languages I've missed.
 *
 * Explicit encodings in the directory name (e.g. de_DE.UTF-8) override this
 * table.
 */
struct directory_entry {
	const char *lang_dir;
	const char *source_encoding;
};

static struct directory_entry directory_table[] = {
	{ "C",		"ISO-8859-1"	}, /* English */
	{ "POSIX",	"ISO-8859-1"	}, /* English */
	{ "da",		"ISO-8859-1"	}, /* Danish */
	{ "de",		"ISO-8859-1"	}, /* German */
	{ "en",		"ISO-8859-1"	}, /* English */
	{ "es",		"ISO-8859-1"	}, /* Spanish */
	{ "et",		"ISO-8859-1"	}, /* Estonian */
	{ "fi",		"ISO-8859-1"	}, /* Finnish */
	{ "fr",		"ISO-8859-1"	}, /* French */
	{ "ga",		"ISO-8859-1"	}, /* Irish */
	{ "gl",		"ISO-8859-1"	}, /* Galician */
	{ "id",		"ISO-8859-1"	}, /* Indonesian */
	{ "is",		"ISO-8859-1"	}, /* Icelandic */
	{ "it",		"ISO-8859-1"	}, /* Italian */
	{ "nb",		"ISO-8859-1"	}, /* Norwegian Bokmål */
	{ "nl",		"ISO-8859-1"	}, /* Dutch */
	{ "nn",		"ISO-8859-1"	}, /* Norwegian Nynorsk */
	{ "no",		"ISO-8859-1"	}, /* Norwegian */
	{ "pt",		"ISO-8859-1"	}, /* Portuguese */
	{ "sv",		"ISO-8859-1"	}, /* Swedish */

#ifdef MULTIBYTE_GROFF
	/* These languages require a patched version of groff with the
	 * ascii8 and nippon devices.
	 */
	{ "be",		"CP1251"	}, /* Belarusian */
	{ "bg",		"CP1251"	}, /* Bulgarian */
	{ "cs",		"ISO-8859-2"	}, /* Czech */
	{ "el",		"ISO-8859-7"	}, /* Greek */
	{ "hr",		"ISO-8859-2"	}, /* Croatian */
	{ "hu",		"ISO-8859-2"	}, /* Hungarian */
	{ "ja",		"EUC-JP"	}, /* Japanese */
	{ "ko",		"EUC-KR"	}, /* Korean */
	{ "lt",		"ISO-8859-13"	}, /* Lithuanian */
	{ "lv",		"ISO-8859-13"	}, /* Latvian */
	{ "mk",		"ISO-8859-5"	}, /* Macedonian */
	{ "pl",		"ISO-8859-2"	}, /* Polish */
	{ "ro",		"ISO-8859-2"	}, /* Romanian */
	{ "ru",		"KOI8-R"	}, /* Russian */
	{ "sk",		"ISO-8859-2"	}, /* Slovak */
	{ "sl",		"ISO-8859-2"	}, /* Slovenian */
	/* sr@latin must precede sr, due to top-down left-substring matching later */
	{ "sr@latin",	"ISO-8859-2"	}, /* Serbian Latin */
	{ "sr",		"ISO-8859-5"	}, /* Serbian */
	{ "tr",		"ISO-8859-9"	}, /* Turkish */
	{ "uk",		"KOI8-U"	}, /* Ukrainian */
	{ "vi",		"TCVN5712-1"	}, /* Vietnamese */
	{ "zh_CN",	"GBK"		}, /* Simplified Chinese */
	{ "zh_SG",	"GBK"		}, /* Simplified Chinese, Singapore */
	{ "zh_HK",	"BIG5HKSCS"	}, /* Traditional Chinese, Hong Kong */
	{ "zh_TW",	"BIG5"		}, /* Traditional Chinese */
#endif /* MULTIBYTE_GROFF */

	{ NULL,		NULL		}
};

static const char fallback_source_encoding[] = "ISO-8859-1";

/* Unfortunately, there is no portable way to inspect iconv's internal table
 * of character set aliases. We copy the most interesting ones here so that
 * we can deal with them if they appear in directory names. Note that all
 * names will be converted to upper case before looking them up in this
 * table.
 */
struct charset_alias_entry {
	const char *alias;
	const char *canonical_name;
};

static struct charset_alias_entry charset_alias_table[] = {
	/* The FHS is silly and requires numeric-only aliases that iconv
	 * does not support.
	 */
	{ "88591",		"ISO-8859-1"		},
	{ "88592",		"ISO-8859-2"		},
	{ "88593",		"ISO-8859-3"		},
	{ "88594",		"ISO-8859-4"		},
	{ "88595",		"ISO-8859-5"		},
	{ "88596",		"ISO-8859-6"		},
	{ "88597",		"ISO-8859-7"		},
	{ "88598",		"ISO-8859-8"		},
	{ "88599",		"ISO-8859-9"		},
	{ "885910",		"ISO-8859-10"		},
	{ "885911",		"ISO-8859-11"		},
	{ "885913",		"ISO-8859-13"		},
	{ "885914",		"ISO-8859-14"		},
	{ "885915",		"ISO-8859-15"		},
	{ "885916",		"ISO-8859-16"		},

	{ "ASCII",		"ANSI_X3.4-1968"	},
	{ "BIG-5",		"BIG5"			},
	{ "BIG5-HKSCS",		"BIG5HKSCS"		},
	{ "EUCCN",		"EUC-CN"		},
	{ "EUCJP",		"EUC-JP"		},
	{ "EUCKR",		"EUC-KR"		},
	{ "EUCTW",		"EUC-TW"		},
	{ "GB2312",		"EUC-CN"		},
	{ "ISO8859-1",		"ISO-8859-1"		},
	{ "ISO8859-2",		"ISO-8859-2"		},
	{ "ISO8859-3",		"ISO-8859-3"		},
	{ "ISO8859-4",		"ISO-8859-4"		},
	{ "ISO8859-5",		"ISO-8859-5"		},
	{ "ISO8859-6",		"ISO-8859-6"		},
	{ "ISO8859-7",		"ISO-8859-7"		},
	{ "ISO8859-8",		"ISO-8859-8"		},
	{ "ISO8859-9",		"ISO-8859-9"		},
	{ "ISO8859-10",		"ISO-8859-10"		},
	{ "ISO8859-11",		"ISO-8859-11"		},
	{ "ISO8859-13",		"ISO-8859-13"		},
	{ "ISO8859-14",		"ISO-8859-14"		},
	{ "ISO8859-15",		"ISO-8859-15"		},
	{ "ISO8859-16",		"ISO-8859-16"		},
	{ "KOI8R",		"KOI8-R"		},
	{ "KOI8U",		"KOI8-U"		},
	{ "UJIS",		"EUC-JP"		},
	{ "US-ASCII",		"ANSI_X3.4-1968"	},
	{ "UTF8",		"UTF-8"			},

	{ NULL,			NULL			}
};

/* The default groff terminal output device to be used is determined based
 * on locale_charset (), which returns the character set used by the current
 * locale.
 */
struct charset_entry {
	const char *charset_from_locale;
	const char *default_device;
};

static struct charset_entry charset_table[] = {
	{ "ANSI_X3.4-1968",	"ascii"		},
#ifndef HEIRLOOM_NROFF
	{ "ISO-8859-1",		"latin1"	},
#endif /* HEIRLOOM_NROFF */
	{ "UTF-8",		"utf8"		},

#ifndef HEIRLOOM_NROFF
# ifdef MULTIBYTE_GROFF
	{ "BIG5",		"nippon"	},
	{ "BIG5HKSCS",		"nippon"	},
	{ "EUC-CN",		"nippon"	},
	{ "EUC-JP",		"nippon"	},
	{ "EUC-TW",		"nippon"	},
	{ "GBK",		"nippon"	},
# else /* !MULTIBYTE_GROFF */
	/* If we have a smarter version of groff, this is better dealt with
	 * using either ascii8 (Debian multibyte patch) or preconv (as of
	 * groff 1.20). This is a not-quite-right stopgap in case we have
	 * neither.
	 */
	{ "ISO-8859-15",    	"latin1"	},
# endif /* MULTIBYTE_GROFF */
#endif /* HEIRLOOM_NROFF */

	{ NULL,			NULL		}
};

static const char *fallback_default_device =
#ifdef MULTIBYTE_GROFF
	"ascii8"
#else /* !MULTIBYTE_GROFF */
	"ascii"
#endif /* MULTIBYTE_GROFF */
	;

/* The encoding used for the text passed to groff is a function of the
 * selected groff device. Traditional devices expect ISO-8859-1 on input
 * (yes, even the utf8 device); devices added in the Debian multibyte patch
 * expect other encodings. The ascii8 device passes top-bit-set characters
 * straight through so is (probably ...) encoding-agnostic. If this encoding
 * does not match the source encoding, an iconv pipe is used (if available)
 * to perform recoding.
 */
struct device_entry {
	const char *roff_device;
	const char *roff_encoding;
	const char *output_encoding;
};

static struct device_entry device_table[] = {
	/* nroff devices */
	{ "ascii",	"ANSI_X3.4-1968",	"ANSI_X3.4-1968"	},
	{ "latin1",	"ISO-8859-1",		"ISO-8859-1"		},
	{ "utf8",	"ISO-8859-1",		"UTF-8"			},

#ifdef MULTIBYTE_GROFF
	{ "ascii8",	NULL,			NULL			},
	{ "nippon",	NULL,			NULL			},
#endif /* MULTIBYTE_GROFF */

#ifdef HEIRLOOM_NROFF
	/* Not strictly accurate, but we only use this in UTF-8 locales. */
	{ "locale",	"UTF-8",		"UTF-8"			},
#endif /* HEIRLOOM_NROFF */

	/* troff devices */
	{ "X75",	NULL,			NULL			},
	{ "X75-12",	NULL,			NULL			},
	{ "X100",	NULL,			NULL			},
	{ "X100-12",	NULL,			NULL			},
	{ "dvi",	NULL,			NULL			},
	{ "html",	NULL,			NULL			},
	{ "lbp",	NULL,			NULL			},
	{ "lj4",	NULL,			NULL			},
	{ "ps",		NULL,			NULL			},

	{ NULL,		NULL,			NULL			}
};

static const char fallback_roff_encoding[] = "ISO-8859-1";

/* Setting less_charset to iso8859 tells the less pager that characters
 * between 0xA0 and 0xFF are displayable, not that its input is encoded in
 * ISO-8859-*. TODO: Perhaps using LESSCHARDEF would be better.
 *
 * Character set names compatible only with jless go in jless_charset.
 */
struct less_charset_entry {
	const char *charset_from_locale;
	const char *less_charset;
	const char *jless_charset;
};

static struct less_charset_entry less_charset_table[] = {
	{ "ANSI_X3.4-1968",	"ascii",	NULL		},
	{ "ISO-8859-1",		"iso8859",	NULL		},
	{ "UTF-8",		"utf-8",	NULL		},

#ifdef MULTIBYTE_GROFF
	{ "CP1251",		"windows",	NULL		},
	{ "EUC-JP",		"iso8859",	"japanese-ujis"	},
	{ "KOI8-R",		"koi8-r",	NULL		},
	/* close enough? */
	{ "KOI8-U",		"koi8-r",	NULL		},
#endif /* MULTIBYTE_GROFF */

	{ NULL,			NULL,		NULL		}
};

static const char fallback_less_charset[] = "iso8859";

/* Encoding conversions from groff-1.20/src/preproc/preconv/preconv.cpp.
 * I've only included those not already recognised by GNU libiconv.
 */
struct conversion_entry {
	const char *from;
	const char *to;
};

static struct conversion_entry conversion_table[] = {
	{ "chinese-big5",			"Big5" },
	{ "chinese-euc",			"GB2312" },
	{ "chinese-iso-8bit",			"GB2312" },
	{ "cn-gb-2312",				"GB2312" },
	{ "cp878",				"KOI8-R" },
	{ "cyrillic-iso-8bit",			"ISO-8859-5" },
	{ "cyrillic-koi8",			"KOI8-R" },
	{ "euc-china",				"GB2312" },
	{ "euc-japan",				"EUC-JP" },
	{ "euc-japan-1990",			"EUC-JP" },
	{ "euc-kr",				"EUC-KR" },
	{ "greek-iso-8bit",			"ISO-8859-7" },
	{ "iso-latin-1",			"ISO-8859-1" },
	{ "iso-latin-2",			"ISO-8859-2" },
	{ "iso-latin-5",			"ISO-8859-9" },
	{ "iso-latin-7",			"ISO-8859-13" },
	{ "iso-latin-9",			"ISO-8859-15" },
	{ "japanese-iso-8bit",			"EUC-JP" },
	{ "japanese-euc",			"EUC-JP" },
	{ "jis8",				"EUC-JP" },
	{ "korean-euc",				"EUC-KR" },
	{ "korean-iso-8bit",			"EUC-KR" },
	{ "latin-0",				"ISO-8859-15" },
	{ "latin-1",				"ISO-8859-1" },
	{ "latin-2",				"ISO-8859-2" },
	{ "latin-5",				"ISO-8859-9" },
	{ "latin-7",				"ISO-8859-13" },
	{ "mule-utf-16",			"UTF-16" },
	{ "mule-utf-16be",			"UTF-16BE" },
	{ "mule-utf-16-be",			"UTF-16BE" },
	{ "mule-utf-16be-with-signature",	"UTF-16" },
	{ "mule-utf-16le",			"UTF-16LE" },
	{ "mule-utf-16-le",			"UTF-16LE" },
	{ "mule-utf-16le-with-signature",	"UTF-16" },
	{ "mule-utf-8",				"UTF-8" },
	{ "utf-16-be",				"UTF-16BE" },
	{ "utf-16be-with-signature",		"UTF-16" },
	{ "utf-16-be-with-signature",		"UTF-16" },
	{ "utf-16-le",				"UTF-16LE" },
	{ "utf-16le-with-signature",		"UTF-16" },
	{ "utf-16-le-with-signature",		"UTF-16" },
	{ NULL,					NULL }
};

const char *groff_preconv = NULL;

/* Is the groff "preconv" helper available? If so, return its name.
 * Otherwise, return NULL.
 */
const char *get_groff_preconv (void)
{
	if (groff_preconv) {
		if (*groff_preconv)
			return groff_preconv;
		else
			return NULL;
	}

	if (pathsearch_executable ("gpreconv"))
		groff_preconv = "gpreconv";
	else if (pathsearch_executable ("preconv"))
		groff_preconv = "preconv";
	else
		groff_preconv = "";

	if (*groff_preconv)
		return groff_preconv;
	else
		return NULL;
}

/* Return the assumed encoding of the source man page, based on the
 * directory in which it was found. The caller should attempt to recode from
 * this to whatever encoding is expected by groff.
 *
 * The caller should free the returned string when it is finished with it.
 */
char * _GL_ATTRIBUTE_MALLOC get_page_encoding (const char *lang)
{
	const struct directory_entry *entry;
	const char *dot;

	if (!lang || !*lang) {
		/* Guess based on the locale. */
		lang = setlocale (LC_MESSAGES, NULL);
		if (!lang)
			return xstrdup (fallback_source_encoding);
	}

	dot = strchr (lang, '.');
	if (dot) {
		/* The FHS has the worst specification of what's supposed to
		 * go after the dot here that I've ever seen. To quote from
		 * version 2.1:
		 *
		 * "It is recommended that this be a numeric representation
		 * if possible (ISO standards, especially), not include
		 * additional punctuation symbols, and that any letters be
		 * in lowercase."
		 *
		 * Any sane standard would use directory names like
		 * de_DE.ISO-8859-1; the examples in the FHS recommend
		 * de_DE.88591 instead. Considering that there is no other
		 * conceivable use for encodings in directory names other
		 * than to pass them to iconv or similar, this is quite
		 * startlingly useless.
		 *
		 * While we now support this thanks to
		 * get_canonical_charset_name, the FHS specification is
		 * obviously wrong and I plan to petition to have it
		 * changed. I recommend ignoring this part of the FHS.
		 */
		char *dir_encoding =
			xstrndup (dot + 1, strcspn (dot + 1, ",@"));
		char *canonical_dir_encoding =
			xstrdup (get_canonical_charset_name (dir_encoding));
		free (dir_encoding);
		return canonical_dir_encoding;
	}

	for (entry = directory_table; entry->lang_dir; ++entry)
		if (STRNEQ (entry->lang_dir, lang, strlen (entry->lang_dir)))
			return xstrdup (entry->source_encoding);

	return xstrdup (fallback_source_encoding);
}

/* Return the canonical encoding for source man pages in the specified
 * language. This ignores any encoding specification in the language
 * directory name. The source encoding should be used as a basis for
 * determining the correct roff device to use: that is, the caller should
 * behave as if it is recoding from the page encoding to the source encoding
 * first, although in practice it should recode directly from the page
 * encoding to the roff encoding.
 *
 * You should normally only call this function if the page encoding is
 * UTF-8, in which case older versions of groff that lack preconv need to
 * have the page recoded to some legacy encoding). If the page is in a
 * legacy encoding, then attempting to recode from that to some other legacy
 * encoding will probably do more harm than good.
 *
 * Here are a few concrete examples of why these distinctions are important:
 *
 *   /usr/share/man/en_GB.UTF-8, locale C
 *     page encoding = UTF-8
 *     source encoding = ISO-8859-1
 *     roff encoding = ISO-8859-1
 *     output encoding = UTF-8
 *     UTF-8 -> iconv -> ISO-8859-1 -> groff -Tascii -> ANSI_X3.4-1968
 *
 *   /usr/share/man/pl_PL.UTF-8, locale pl_PL.UTF-8
 *     page encoding = UTF-8
 *     source encoding = ISO-8859-2
 *     roff encoding = ISO-8859-2
 *     output encoding = ISO-8859-2
 *     UTF-8 -> iconv -> ISO-8859-2 -> groff -Tascii8
 *                    -> ISO-8859-2 -> iconv -> UTF-8
 *
 *   /usr/share/man/ja_JP.EUC-JP, locale ja_JP.UTF-8
 *     page encoding = EUC-JP
 *     source encoding = EUC-JP
 *     roff encoding = UTF-8
 *     output encoding = UTF-8
 *     EUC-JP -> iconv -> UTF-8 -> groff -Tutf8 -> UTF-8
 *
 *   /usr/share/man/en_GB.ISO-8859-15, locale en_GB.UTF-8
 *     page encoding = ISO-8859-15
 *     source encoding = ISO-8859-15
 *     roff encoding = ISO-8859-15
 *     output encoding = ISO-8859-15
 *     ISO-8859-15 -> groff -Tascii8 -> ISO-8859-15 -> iconv -> UTF-8
 */
const char *get_source_encoding (const char *lang)
{
	const struct directory_entry *entry;

	if (!lang || !*lang) {
		/* Guess based on the locale. */
		lang = setlocale (LC_MESSAGES, NULL);
		if (!lang)
			return fallback_source_encoding;
	}

	for (entry = directory_table; entry->lang_dir; ++entry)
		if (STRNEQ (entry->lang_dir, lang, strlen (entry->lang_dir)))
			return entry->source_encoding;

	return fallback_source_encoding;
}

const char *get_canonical_charset_name (const char *charset)
{
	const struct charset_alias_entry *entry;
	char *charset_upper = xstrdup (charset);
	char *p;

	for (p = charset_upper; *p; ++p)
		*p = CTYPE (toupper, *p);

	for (entry = charset_alias_table; entry->alias; ++entry)
		if (STREQ (entry->alias, charset_upper)) {
			free (charset_upper);
			return entry->canonical_name;
		}

	free (charset_upper);
	return charset;
}

/* Return the current locale's character set. */
const char *get_locale_charset (void)
{
	const char *charset;
	char *saved_locale;

	/* We need to modify LC_CTYPE temporarily in order to look at the
	 * codeset, so save it first.
	 */
	saved_locale = setlocale (LC_CTYPE, NULL);
	if (saved_locale)
		saved_locale = xstrdup (saved_locale);

	setlocale (LC_CTYPE, "");

	charset = locale_charset ();

	/* Restore LC_CTYPE to its value on entry to this function. */
	setlocale (LC_CTYPE, saved_locale);
	free (saved_locale);

	if (charset && *charset)
		return get_canonical_charset_name (charset);
	else
		return NULL;
}

/* Find a locale with this character set. This is a non-portable operation,
 * but required to make col(1) work correctly with -E. If no locale can be
 * found, or if none needs to be set, return NULL.
 *
 * The caller should free the returned string when it is finished with it.
 */
char *find_charset_locale (const char *charset)
{
	const char *canonical_charset = get_canonical_charset_name (charset);
	char *saved_locale;
	const char supported_path[] = "/usr/share/i18n/SUPPORTED";
	FILE *supported = NULL;
	char *line = NULL;
	size_t n = 0;
	char *locale = NULL;

	if (STREQ (charset, get_locale_charset ()))
		return NULL;

	saved_locale = setlocale (LC_CTYPE, NULL);
	if (saved_locale)
		saved_locale = xstrdup (saved_locale);

	supported = fopen (supported_path, "r");
	while (supported && getline (&line, &n, supported) >= 0) {
		const char *space = strchr (line, ' ');
		if (space) {
			char *encoding = xstrdup (space + 1);
			char *newline = strchr (encoding, '\n');
			if (newline)
				*newline = 0;
			if (STREQ (canonical_charset,
				   get_canonical_charset_name (encoding))) {
				locale = xstrndup (line, space - line);
				/* Is this locale actually installed? */
				if (setlocale (LC_CTYPE, locale)) {
					free (encoding);
					goto out;
				} else {
					free (locale);
					locale = NULL;
				}
			}
			free (encoding);
		}
		free (line);
		line = NULL;
	}

	if (strlen (canonical_charset) >= 5 &&
	    STRNEQ (canonical_charset, "UTF-8", 5)) {
		locale = xstrdup ("C.UTF-8");
		if (setlocale (LC_CTYPE, locale))
			goto out;
		free (locale);
		locale = xstrdup ("en_US.UTF-8");
		if (setlocale (LC_CTYPE, locale))
			goto out;
		free (locale);
		locale = NULL;
	}

out:
	free (line);
	setlocale (LC_CTYPE, saved_locale);
	free (saved_locale);
	if (supported)
		fclose (supported);
	return locale;
}

/* Can we take this input encoding and produce this output encoding, perhaps
 * with the help of some iconv pipes? */
static bool compatible_encodings (const char *input, const char *output)
{
	if (STREQ (input, output))
		return true;

	/* If the input is ASCII, recoding should be easy. Try it. */
	if (STREQ (input, "ANSI_X3.4-1968"))
		return true;

	/* If the input is UTF-8, it's either a simple recoding of whatever
	 * we want or else it probably won't work at all no matter what we
	 * do. We might as well try it for now.
	 */
	if (STREQ (input, "UTF-8"))
		return true;

	/* If the output is ASCII, this is probably because the caller
	 * explicitly asked for it, so we have little choice but to try.
	 */
	if (STREQ (output, "ANSI_X3.4-1968"))
		return true;

#ifdef MULTIBYTE_GROFF
	/* Special case for some CJK UTF-8 locales, which take UTF-8 input
	 * recoded from EUC-JP (etc.) and produce UTF-8 output. This is
	 * rather filthy.
	 */
	if ((STREQ (input, "BIG5") || STREQ (input, "BIG5HKSCS") ||
	     STREQ (input, "EUC-JP") ||
	     STREQ (input, "EUC-CN") || STREQ (input, "GBK") ||
	     STREQ (input, "EUC-KR") ||
	     STREQ (input, "EUC-TW")) &&
	    STREQ (output, "UTF-8"))
		return true;
#endif /* MULTIBYTE_GROFF */

	return false;
}

/* Return the default groff device for the given character set. This may be
 * overridden by the user. The page's source encoding is needed to ensure
 * that the device is compatible: consider ru_RU.UTF-8, which needs ascii8
 * and a trailing iconv pipe to recode to UTF-8.
 *
 * All this encoding compatibility stuff feels like a slightly nasty hack,
 * but I haven't yet come up with a cleaner way to do it.
 */
const char *get_default_device (const char *charset_from_locale,
				const char *source_encoding)
{
	const struct charset_entry *entry;

	if (get_groff_preconv ()) {
		/* ASCII is a special case, and the only way we can get
		 * things like bullet marks to come out right is by using
		 * the ascii device. People using such a basic locale
		 * probably don't want anything fancy anyway.
		 */
		if (charset_from_locale &&
		    STREQ (charset_from_locale, "ANSI_X3.4-1968"))
			return "ascii";
		else
			return "utf8";
	}

	if (!charset_from_locale)
		return fallback_default_device;

	for (entry = charset_table; entry->charset_from_locale; ++entry) {
		if (STREQ (entry->charset_from_locale, charset_from_locale)) {
			const char *roff_encoding =
				get_roff_encoding (entry->default_device,
						   source_encoding);
			if (compatible_encodings (source_encoding,
						  roff_encoding))
				return entry->default_device;
		}
	}

	return fallback_default_device;
}

/* Is this a known *roff device name? */
bool _GL_ATTRIBUTE_PURE is_roff_device (const char *device)
{
	const struct device_entry *entry;

	for (entry = device_table; entry->roff_device; ++entry) {
		if (STREQ (entry->roff_device, device))
			return true;
	}

	return false;
}

/* Find the input encoding expected by groff, and set the LESSCHARSET
 * environment variable appropriately.
 */
const char *get_roff_encoding (const char *device, const char *source_encoding)
{
	const struct device_entry *entry;
	bool found = false;
	const char *roff_encoding = NULL;

	if (device) {
		for (entry = device_table; entry->roff_device; ++entry) {
			if (STREQ (entry->roff_device, device)) {
				found = true;
				roff_encoding = entry->roff_encoding;
				break;
			}
		}
	}

	if (!found)
		roff_encoding = fallback_roff_encoding;

#ifdef MULTIBYTE_GROFF
	/* An ugly special case is needed here. The utf8 device normally
	 * takes ISO-8859-1 input. However, with the multibyte patch, when
	 * recoding from CJK character sets it takes UTF-8 input instead.
	 * This is evil, but there's not much that can be done about it
	 * apart from waiting for groff 2.0.
	 */
	if (device && STREQ (device, "utf8") && !get_groff_preconv () &&
	    STREQ (get_locale_charset (), "UTF-8")) {
		const char *ctype = setlocale (LC_CTYPE, NULL);
		if (STRNEQ (ctype, "ja_JP", 5) ||
		    STRNEQ (ctype, "ko_KR", 5) ||
		    STRNEQ (ctype, "zh_CN", 5) ||
		    STRNEQ (ctype, "zh_HK", 5) ||
		    STRNEQ (ctype, "zh_SG", 5) ||
		    STRNEQ (ctype, "zh_TW", 5))
			roff_encoding = "UTF-8";
	}
#endif /* MULTIBYTE_GROFF */

	return roff_encoding ? roff_encoding : source_encoding;
}

/* Find the output encoding that this device will produce, or NULL if it
 * will simply pass through the input encoding.
 */
const char * _GL_ATTRIBUTE_PURE get_output_encoding (const char *device)
{
	const struct device_entry *entry;

	for (entry = device_table; entry->roff_device; ++entry)
		if (STREQ (entry->roff_device, device))
			return entry->output_encoding;

	return NULL;
}

/* Return the value of LESSCHARSET appropriate for this locale. */
const char * _GL_ATTRIBUTE_PURE get_less_charset (
	const char *charset_from_locale)
{
	const struct less_charset_entry *entry;

	if (charset_from_locale) {
		for (entry = less_charset_table; entry->charset_from_locale;
		     ++entry)
			if (STREQ (entry->charset_from_locale,
				   charset_from_locale))
				return entry->less_charset;
	}

	return fallback_less_charset;
}

/* Return the value of JLESSCHARSET appropriate for this locale. May return
 * NULL.
 */
const char * _GL_ATTRIBUTE_PURE get_jless_charset (
	const char *charset_from_locale)
{
	const struct less_charset_entry *entry;

	if (charset_from_locale) {
		for (entry = less_charset_table; entry->charset_from_locale;
		     ++entry)
			if (STREQ (entry->charset_from_locale,
				   charset_from_locale))
				return entry->jless_charset;
	}

	return NULL;
}

/* Convert Emacs-style coding tags to ones that libiconv understands. */
static char *convert_encoding (char *encoding)
{
	size_t encoding_len = strlen (encoding);
	const struct conversion_entry *entry;

#define STRIP(s, l) do { \
	if (encoding_len > (l) && \
	    !strcasecmp (encoding + encoding_len - (l), (s))) \
		encoding[encoding_len - (l)] = '\0'; \
} while (0)

	STRIP ("-dos", 4);
	STRIP ("-mac", 4);
	STRIP ("-unix", 5);

#undef STRIP

	for (entry = conversion_table; entry->from; ++entry)
		if (!strcasecmp (entry->from, encoding)) {
			free (encoding);
			return xstrdup (entry->to);
		}

	return encoding;
}

/* Inspect the first line of data in a pipeline for preprocessor encoding
 * declarations.
 */
char *check_preprocessor_encoding (pipeline *p)
{
	char *pp_encoding = NULL;

#ifdef PP_COOKIE
	const char *line = pipeline_peekline (p);
	char *directive = NULL;

	/* Some people use .\" incorrectly. We allow it for encoding
	 * declarations but not for preprocessor declarations.
	 */
	if (line &&
	    (STRNEQ (line, PP_COOKIE, 4) || STRNEQ (line, ".\\\" ", 4))) {
		const char *newline = strchr (line, '\n');
		if (newline)
			directive = xstrndup (line + 4,
					      newline - (line + 4));
		else
			directive = xstrdup (line + 4);
	}

	if (directive && strstr (directive, "-*-")) {
		const char *pp_search = strstr (directive, "-*-") + 3;
		while (pp_search && *pp_search) {
			while (*pp_search == ' ')
				++pp_search;
			if (STRNEQ (pp_search, "coding:", 7)) {
				const char *pp_encoding_allow;
				size_t pp_encoding_len;
				pp_search += 7;
				while (*pp_search == ' ')
					++pp_search;
				pp_encoding_allow = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
						    "abcdefghijklmnopqrstuvwxyz"
						    "0123456789-_/:.()";
				pp_encoding_len = strspn (pp_search,
							  pp_encoding_allow);
				pp_encoding = xstrndup (pp_search,
							pp_encoding_len);
				pp_encoding = convert_encoding (pp_encoding);
				debug ("preprocessor encoding: %s\n",
				       pp_encoding);
				break;
			} else {
				pp_search = strchr (pp_search, ';');
				if (pp_search)
					++pp_search;
			}
		}
	}
	free (directive);
#endif /* PP_COOKIE */

	return pp_encoding;
}
