/* keytable.c - This program allows checking/replacing keys at IR

   Copyright (C) 2006-2010 Mauro Carvalho Chehab

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 */

#include <config.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <stdlib.h>
#include <string.h>
#include <linux/input.h>
#include <linux/lirc.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <argp.h>
#include <time.h>
#include <stdbool.h>

#include "ir-encode.h"
#include "parse.h"
#include "toml.h"

#ifdef HAVE_BPF
#include "bpf.h"
#include "bpf_load.h"
#endif

#ifdef ENABLE_NLS
# define _(string) gettext(string)
# include "gettext.h"
# include <locale.h>
# include <langinfo.h>
# include <iconv.h>
#else
# define _(string) string
#endif

# define N_(string) string

struct input_keymap_entry_v2 {
#define KEYMAP_BY_INDEX	(1 << 0)
	u_int8_t  flags;
	u_int8_t  len;
	u_int16_t index;
	u_int32_t keycode;
	u_int8_t  scancode[32];
};


#define IR_PROTOCOLS_USER_DIR IR_KEYTABLE_USER_DIR "/protocols"
#define IR_PROTOCOLS_SYSTEM_DIR IR_KEYTABLE_SYSTEM_DIR "/protocols"

#ifndef EVIOCSCLOCKID
#define EVIOCSCLOCKID		_IOW('E', 0xa0, int)
#endif

#ifndef EVIOCGKEYCODE_V2
#define EVIOCGKEYCODE_V2	_IOR('E', 0x04, struct input_keymap_entry_v2)
#define EVIOCSKEYCODE_V2	_IOW('E', 0x04, struct input_keymap_entry_v2)
#endif

struct keytable_entry {
	u_int32_t scancode;
	u_int32_t keycode;
	struct keytable_entry *next;
};

struct keytable_entry *keytable = NULL;

struct uevents {
	char		*key;
	char		*value;
	struct uevents	*next;
};

struct cfgfile {
	char		*driver;
	char		*table;
	char		*fname;
	struct cfgfile	*next;
};

struct sysfs_names  {
	char			*name;
	struct sysfs_names	*next;
};

enum rc_type {
	UNKNOWN_TYPE,
	SOFTWARE_DECODER,
	HARDWARE_DECODER,
};

enum sysfs_ver {
	VERSION_1,	/* has nodes protocol, enabled */
	VERSION_2,	/* has node protocols */
};

enum sysfs_protocols {
	SYSFS_UNKNOWN		= (1 << 0),
	SYSFS_OTHER		= (1 << 1),
	SYSFS_LIRC		= (1 << 2),
	SYSFS_RC5		= (1 << 3),
	SYSFS_RC5_SZ		= (1 << 4),
	SYSFS_JVC		= (1 << 5),
	SYSFS_SONY		= (1 << 6),
	SYSFS_NEC		= (1 << 7),
	SYSFS_SANYO		= (1 << 8),
	SYSFS_MCE_KBD		= (1 << 9),
	SYSFS_RC6		= (1 << 10),
	SYSFS_SHARP		= (1 << 11),
	SYSFS_XMP		= (1 << 12),
	SYSFS_CEC		= (1 << 13),
	SYSFS_IMON		= (1 << 14),
	SYSFS_INVALID		= 0,
};

struct protocol_map_entry {
	const char *name;
	const char *sysfs1_name;
	enum sysfs_protocols sysfs_protocol;
};

const struct protocol_map_entry protocol_map[] = {
	{ "unknown",	NULL,		SYSFS_UNKNOWN	},
	{ "other",	NULL,		SYSFS_OTHER	},
	{ "lirc",	NULL,		SYSFS_LIRC	},
	{ "rc-5",	"/rc5_decoder",	SYSFS_RC5	},
	{ "rc-5x",	NULL,		SYSFS_INVALID	},
	{ "rc-5-sz",	NULL,		SYSFS_RC5_SZ	},
	{ "jvc",	"/jvc_decoder",	SYSFS_JVC	},
	{ "sony",	"/sony_decoder",SYSFS_SONY	},
	{ "sony12",	NULL,		SYSFS_INVALID	},
	{ "sony15",	NULL,		SYSFS_INVALID	},
	{ "sony20",	NULL,		SYSFS_INVALID	},
	{ "nec",	"/nec_decoder",	SYSFS_NEC	},
	{ "sanyo",	NULL,		SYSFS_SANYO	},
	{ "mce_kbd",	NULL,		SYSFS_MCE_KBD	},
	{ "rc-6",	"/rc6_decoder",	SYSFS_RC6	},
	{ "rc-6-0",	NULL,		SYSFS_INVALID	},
	{ "rc-6-6a-20",	NULL,		SYSFS_INVALID	},
	{ "rc-6-6a-24",	NULL,		SYSFS_INVALID	},
	{ "rc-6-6a-32",	NULL,		SYSFS_INVALID	},
	{ "rc-6-mce",	NULL,		SYSFS_INVALID	},
	{ "sharp",	NULL,		SYSFS_SHARP	},
	{ "xmp",	"/xmp_decoder",	SYSFS_XMP	},
	{ "cec",	NULL,		SYSFS_CEC	},
	{ "imon",	NULL,		SYSFS_IMON	},
	{ NULL,		NULL,		SYSFS_INVALID	},
};

static bool protocol_like(const char *a, const char *b)
{
	while (*a && *b) {
		if (*a == '-' || *a == '_')
			a++;
		if (*b == '-' || *b == '_')
			b++;
		if (tolower(*a) != tolower(*b))
			return false;
		a++; b++;
	}

	return !*a && !*b;
}

static enum sysfs_protocols parse_sysfs_protocol(const char *name, bool all_allowed)
{
	const struct protocol_map_entry *pme;

	if (!name)
		return SYSFS_INVALID;

	if (all_allowed && !strcasecmp(name, "all"))
		return ~0;

	for (pme = protocol_map; pme->name; pme++) {
		if (protocol_like(name, pme->name))
			return pme->sysfs_protocol;
	}

	return SYSFS_INVALID;
}

static void write_sysfs_protocols(enum sysfs_protocols protocols, FILE *fp, const char *fmt)
{
	const struct protocol_map_entry *pme;

	for (pme = protocol_map; pme->name; pme++) {
		if (!(protocols & pme->sysfs_protocol))
			continue;

		fprintf(fp, fmt, pme->name);
		protocols &= ~pme->sysfs_protocol;
	}
}

static int parse_code(const char *string)
{
	struct parse_event *p;

	for (p = key_events; p->name != NULL; p++) {
		if (!strcasecmp(p->name, string))
			return p->value;
	}
	return -1;
}

const char *argp_program_version = "IR keytable control version " V4L_UTILS_VERSION;
const char *argp_program_bug_address = "Mauro Carvalho Chehab <mchehab@kernel.org>";

static const char doc[] = N_(
	"\nAllows get/set IR keycode/scancode tables\n"
	"You need to have read permissions on /dev/input for the program to work\n"
	"\nOn the options below, the arguments are:\n"
	"  DEV       - the /dev/input/event* device to control\n"
	"  SYSDEV    - the ir class as found at /sys/class/rc\n"
	"  TABLE     - a file with a set of scancode=keycode value pairs\n"
	"  SCANKEY   - a set of scancode1=keycode1,scancode2=keycode2.. value pairs\n"
	"  PROTOCOL  - protocol name (nec, rc-5, rc-6, jvc, sony, sanyo, rc-5-sz, lirc,\n"
	"              sharp, mce_kbd, xmp, imon, other, all) to be enabled,\n"
	"              or a bpf protocol name or file\n"
	"  DELAY     - Delay before repeating a keystroke\n"
	"  PERIOD    - Period to repeat a keystroke\n"
	"  PARAMETER - a set of name1=number1[,name2=number2]... for the BPF prototcol\n"
	"  CFGFILE   - configuration file that associates a driver/table name with\n"
	"              a keymap file\n"
	"\nOptions can be combined together.");

static const struct argp_option options[] = {
	{"verbose",	'v',	0,		0,	N_("enables debug messages"), 0},
	{"clear",	'c',	0,		0,	N_("clears the old table"), 0},
	{"sysdev",	's',	N_("SYSDEV"),	0,	N_("ir class device to control"), 0},
	{"test",	't',	0,		0,	N_("test if IR is generating events"), 0},
	{"device",	'd',	N_("DEV"),	0,	N_("ir device to control"), 0},
	{"read",	'r',	0,		0,	N_("reads the current scancode/keycode table"), 0},
	{"write",	'w',	N_("TABLE"),	0,	N_("write (adds) the scancodes to the device scancode/keycode table from an specified file"), 0},
	{"set-key",	'k',	N_("SCANKEY"),	0,	N_("Change scan/key pairs"), 0},
	{"protocol",	'p',	N_("PROTOCOL"),	0,	N_("Protocol to enable (the other ones will be disabled). To enable more than one, use the option more than one time"), 0},
	{"parameter",	'e',	N_("PARAMETER"), 0,	N_("Set a parameter for the protocol decoder")},
	{"delay",	'D',	N_("DELAY"),	0,	N_("Sets the delay before repeating a keystroke"), 0},
	{"period",	'P',	N_("PERIOD"),	0,	N_("Sets the period to repeat a keystroke"), 0},
	{"auto-load",	'a',	N_("CFGFILE"),	0,	N_("Auto-load a table, based on a configuration file. Only works with sysdev."), 0},
	{"help",        '?',	0,		0,	N_("Give this help list"), -1},
	{"usage",	-3,	0,		0,	N_("Give a short usage message")},
	{"version",	'V',	0,		0,	N_("Print program version"), -1},
	{ 0, 0, 0, 0, 0, 0 }
};

static const char args_doc[] = N_(
	"--device [/dev/input/event* device]\n"
	"--sysdev [ir class (f. ex. rc0)]\n"
	"[for using the rc0 sysdev]");

/* Static vars to store the parameters */
static char *devclass = NULL;
static char *devicename = NULL;
static int readtable = 0;
static int clear = 0;
int debug = 0;
static int test = 0;
static int delay = -1;
static int period = -1;
static enum sysfs_protocols ch_proto = 0;

struct bpf_protocol {
	struct bpf_protocol *next;
	struct toml_table_t *toml;
	char *name;
};

struct bpf_parameter {
	struct bpf_parameter *next;
	int value;
	char name[0];
};

static struct bpf_protocol *bpf_protocol;
static struct bpf_parameter *bpf_parameter;

struct cfgfile cfg = {
	NULL, NULL, NULL, NULL
};

/*
 * Stores the input layer protocol version
 */
static int input_protocol_version = 0;

/*
 * Values that are read only via sysfs node
 */
static int sysfs = 0;

struct rc_device {
	char *sysfs_name;	/* Device sysfs node name */
	char *input_name;	/* Input device file name */
	char *lirc_name;	/* Lirc device file name */
	char *drv_name;		/* Kernel driver that implements it */
	char *dev_name;		/* Kernel device name */
	char *keytable_name;	/* Keycode table name */

	enum sysfs_ver version; /* sysfs version */
	enum rc_type type;	/* Software (raw) or hardware decoder */
	enum sysfs_protocols supported, current; /* Current and supported IR protocols */
};

static bool compare_parameters(struct toml_table_t *a, struct toml_table_t *b)
{
	int i = 0;
	int64_t avalue, bvalue;
	const char *name, *raw;

	while ((name = toml_key_in(a, i++)) != NULL) {
		raw = toml_raw_in(a, name);
		if (!raw)
			continue;

		if (toml_rtoi(raw, &avalue))
			continue;

		raw = toml_raw_in(b, name);
		if (!raw)
			return false;

		if (toml_rtoi(raw, &bvalue))
			return false;

		if (avalue != bvalue)
			return false;
	}

	return true;
}

/*
 * Sometimes, a toml will list the same remote protocol several times with
 * different scancodes. This will because they are different remotes but
 * use the same protocol. Do not load one BPF per remote.
 */
static void add_bpf_protocol(struct bpf_protocol *new)
{
	struct bpf_protocol *a;

	for (a = bpf_protocol; a; a = a->next) {
		if (strcmp(a->name, new->name))
			continue;

		if (compare_parameters(a->toml, new->toml) &&
		    compare_parameters(new->toml, a->toml))
			return;
	}

	new->next = bpf_protocol;
	bpf_protocol = new;
}

static error_t parse_plain_keyfile(char *fname, char **table)
{
	FILE *fin;
	int value, line = 0;
	char *scancode, *keycode, s[2048];
	struct keytable_entry *ke;

	*table = NULL;

	if (debug)
		fprintf(stderr, _("Parsing %s keycode file as plain text\n"), fname);

	fin = fopen(fname, "r");
	if (!fin) {
		return errno;
	}

	while (fgets(s, sizeof(s), fin)) {
		char *p = s;

		line++;
		while (*p == ' ' || *p == '\t')
			p++;
		if (line==1 && p[0] == '#') {
			p++;
			p = strtok(p, "\n\t =:");
			do {
				if (!p)
					goto err_einval;
				if (!strcmp(p, "table")) {
					p = strtok(NULL,"\n, ");
					if (!p)
						goto err_einval;
					*table = malloc(strlen(p) + 1);
					strcpy(*table, p);
				} else if (!strcmp(p, "type")) {
					p = strtok(NULL, " ,\n");
					if (!p)
						goto err_einval;

					while (p) {
						enum sysfs_protocols protocol;

						protocol = parse_sysfs_protocol(p, false);
						if (protocol == SYSFS_INVALID) {
							fprintf(stderr, _("Protocol %s invalid\n"), p);
							goto err_einval;
						}
						ch_proto |= protocol;
						p = strtok(NULL, " ,\n");
					}
				} else {
					goto err_einval;
				}
				p = strtok(NULL, "\n\t =:");
			} while (p);
			continue;
		}

		if (*p == '\n' || *p == '#')
			continue;

		scancode = strtok(p, "\n\t =:");
		if (!scancode)
			goto err_einval;
		if (!strcasecmp(scancode, "scancode")) {
			scancode = strtok(NULL, "\n\t =:");
			if (!scancode)
				goto err_einval;
		}

		keycode = strtok(NULL, "\n\t =:(");
		if (!keycode)
			goto err_einval;

		if (debug)
			fprintf(stderr, _("parsing %s=%s:"), scancode, keycode);
		value = parse_code(keycode);
		if (debug)
			fprintf(stderr, _("\tvalue=%d\n"), value);

		if (value == -1) {
			value = strtol(keycode, NULL, 0);
			if (errno)
				perror(_("value"));
		}

		ke = calloc(1, sizeof(*ke));
		if (!ke) {
			perror("parse_keyfile");
			return ENOMEM;
		}

		ke->scancode	= strtoul(scancode, NULL, 0);
		ke->keycode	= value;
		ke->next	= keytable;
		keytable	= ke;
	}
	fclose(fin);

	return 0;

err_einval:
	fprintf(stderr, _("Invalid parameter on line %d of %s\n"),
		line, fname);
	return EINVAL;
}

static error_t parse_toml_protocol(struct toml_table_t *proot)
{
	struct toml_table_t *scancodes;
	enum sysfs_protocols protocol;
	const char *raw;
	char *p;
	int i = 0;

	raw = toml_raw_in(proot, "protocol");
	if (!raw) {
		fprintf(stderr, _("protocol missing\n"));
		return EINVAL;
	}

	if (toml_rtos(raw, &p)) {
		fprintf(stderr, _("Bad value `%s' for protocol\n"), raw);
		return EINVAL;
	}

	protocol = parse_sysfs_protocol(p, false);
	if (protocol == SYSFS_INVALID) {
		struct bpf_protocol *b;

		b = malloc(sizeof(*b));
		b->name = p;
		b->toml = proot;
		add_bpf_protocol(b);
	}
	else {
		ch_proto |= protocol;
		free(p);
	}

	scancodes = toml_table_in(proot, "scancodes");
	if (!scancodes) {
		if (debug)
			fprintf(stderr, _("No [protocols.scancodes] section\n"));
		return 0;
	}

	for (;;) {
		struct keytable_entry *ke;
		const char *scancode;
		char *keycode;
		int value;

		scancode = toml_key_in(scancodes, i++);
		if (!scancode)
			break;

		raw = toml_raw_in(scancodes, scancode);
		if (!raw) {
			fprintf(stderr, _("Invalid value `%s'\n"), scancode);
			return EINVAL;
		}

		if (toml_rtos(raw, &keycode)) {
			fprintf(stderr, _("Bad value `%s' for keycode\n"),
				keycode);
			return EINVAL;
		}

		if (debug)
			fprintf(stderr, _("parsing %s=%s:"), scancode, keycode);

		value = parse_code(keycode);
		if (debug)
			fprintf(stderr, _("\tvalue=%d\n"), value);

		if (value == -1) {
			value = strtol(keycode, &p, 0);
			if (errno || *p)
				fprintf(stderr, _("keycode `%s' not recognised, no mapping for scancode %s\n"), keycode, scancode);
		}
		free(keycode);

		ke = calloc(1, sizeof(*ke));
		if (!ke) {
			perror("parse_keyfile");
			return ENOMEM;
		}

		ke->scancode	= strtoul(scancode, NULL, 0);
		ke->keycode	= value;
		ke->next	= keytable;
		keytable	= ke;
	}

	return 0;
}

static error_t parse_toml_keyfile(char *fname, char **table)
{
	struct toml_table_t *root, *proot;
	struct toml_array_t *arr;
	int ret, i = 0;
	char buf[200];
	FILE *fin;

	*table = NULL;

	if (debug)
		fprintf(stderr, _("Parsing %s keycode file as toml\n"), fname);

	fin = fopen(fname, "r");
	if (!fin)
		return errno;

	root = toml_parse_file(fin, buf, sizeof(buf));
	fclose(fin);
	if (!root) {
		fprintf(stderr, _("Failed to parse toml: %s\n"), buf);
		return EINVAL;
	}

	arr = toml_array_in(root, "protocols");
	if (!arr) {
		fprintf(stderr, _("Missing [protocols] section\n"));
		return EINVAL;
	}

	for (;;) {
		proot = toml_table_at(arr, i);
		if (!proot)
			break;

		ret = parse_toml_protocol(proot);
		if (ret)
			goto out;

		i++;
	}

	if (i == 0) {
		fprintf(stderr, _("No protocols found\n"));
		goto out;
	}

	// Don't free toml, this is used during bpf loading */
	//toml_free(root);
	return 0;
out:
	toml_free(root);
	return EINVAL;
}

static error_t parse_keyfile(char *fname, char **table)
{
	size_t len = strlen(fname);

	if (len >= 5 && strcasecmp(fname + len - 5, ".toml") == 0)
		return parse_toml_keyfile(fname, table);
	else
		return parse_plain_keyfile(fname, table);
}

struct cfgfile *nextcfg = &cfg;

static error_t parse_cfgfile(char *fname)
{
	FILE *fin;
	int line = 0;
	char s[2048];
	char *driver, *table, *filename;

	if (debug)
		fprintf(stderr, _("Parsing %s config file\n"), fname);

	fin = fopen(fname, "r");
	if (!fin) {
		perror(_("opening keycode file"));
		return errno;
	}

	while (fgets(s, sizeof(s), fin)) {
		char *p = s;

		line++;
		while (*p == ' ' || *p == '\t')
			p++;

		if (*p == '\n' || *p == '#')
			continue;

		driver = strtok(p, "\t ");
		if (!driver)
			goto err_einval;

		table = strtok(NULL, "\t ");
		if (!table)
			goto err_einval;

		filename = strtok(NULL, "\t #\n");
		if (!filename)
			goto err_einval;

		if (debug)
			fprintf(stderr, _("Driver %s, Table %s => file %s\n"),
				driver, table, filename);

		nextcfg->driver = malloc(strlen(driver) + 1);
		strcpy(nextcfg->driver, driver);

		nextcfg->table = malloc(strlen(table) + 1);
		strcpy(nextcfg->table, table);

		nextcfg->fname = malloc(strlen(filename) + 1);
		strcpy(nextcfg->fname, filename);

		nextcfg->next = calloc(1, sizeof(*nextcfg));
		if (!nextcfg->next) {
			perror("parse_cfgfile");
			return ENOMEM;
		}
		nextcfg = nextcfg->next;
	}
	fclose(fin);

	return 0;

err_einval:
	fprintf(stderr, _("Invalid parameter on line %d of %s\n"),
		line, fname);
	return EINVAL;

}

static error_t parse_opt(int k, char *arg, struct argp_state *state)
{
	char *p;
	long key;
	int rc;

	switch (k) {
	case 'v':
		debug++;
		break;
	case 't':
		test++;
		break;
	case 'c':
		clear++;
		break;
	case 'D':
		delay = strtol(arg, &p, 10);
		if (!p || *p || delay < 0)
			argp_error(state, _("Invalid delay: %s"), arg);
		break;
	case 'P':
		period = strtol(arg, &p, 10);
		if (!p || *p || period < 0)
			argp_error(state, _("Invalid period: %s"), arg);
		break;
	case 'd':
		devicename = arg;
		break;
	case 's':
		devclass = arg;
		break;
	case 'r':
		readtable++;
		break;
	case 'w': {
		char *name = NULL;

		rc = parse_keyfile(arg, &name);
		if (rc)
			argp_error(state, _("Failed to read table file %s"), arg);
		if (name)
			fprintf(stderr, _("Read %s table\n"), name);
		break;
	}
	case 'a': {
		rc = parse_cfgfile(arg);
		if (rc)
			argp_error(state, _("Failed to read config file %s"), arg);
		break;
	}
	case 'k':
		p = strtok(arg, ":=");
		do {
			struct keytable_entry *ke;

			if (!p) {
				argp_error(state, _("Missing scancode: %s"), arg);
				break;
			}

			ke = calloc(1, sizeof(*ke));
			if (!ke) {
				perror(_("No memory!\n"));
				return ENOMEM;
			}

			ke->scancode = strtoul(p, NULL, 0);
			if (errno) {
				free(ke);
				argp_error(state, _("Invalid scancode: %s"), p);
				break;
			}

			p = strtok(NULL, ",;");
			if (!p) {
				free(ke);
				argp_error(state, _("Missing keycode"));
				break;
			}

			key = parse_code(p);
			if (key == -1) {
				key = strtol(p, NULL, 0);
				if (errno) {
					free(ke);
					argp_error(state, _("Unknown keycode: %s"), p);
					break;
				}
			}

			ke->keycode = key;

			if (debug)
				fprintf(stderr, _("scancode 0x%04x=%u\n"),
					ke->scancode, ke->keycode);

			ke->next = keytable;
			keytable = ke;

			p = strtok(NULL, ":=");
		} while (p);
		break;
	case 'p':
		for (p = strtok(arg, ",;"); p; p = strtok(NULL, ",;")) {
			enum sysfs_protocols protocol;

			protocol = parse_sysfs_protocol(p, true);
			if (protocol == SYSFS_INVALID) {
				struct bpf_protocol *b;

				b = malloc(sizeof(*b));
				b->name = strdup(p);
				b->toml = NULL;
				b->next = bpf_protocol;
				bpf_protocol = b;
			}
			else {
				ch_proto |= protocol;
			}
		}
		break;
	case 'e':
		p = strtok(arg, ":=");
		do {
			struct bpf_parameter *param;

			if (!p) {
				argp_error(state, _("Missing parameter name: %s"), arg);
				break;
			}

			param = calloc(1, sizeof(*param) + strlen(p) + 1);
			if (!p) {
				perror(_("No memory!\n"));
				return ENOMEM;
			}

			strcpy(param->name, p);

			p = strtok(NULL, ",;");
			if (!p) {
				free(param);
				argp_error(state, _("Missing value"));
				break;
			}

			param->value = strtol(p, NULL, 0);
			if (errno) {
				free(param);
				argp_error(state, _("Unknown keycode: %s"), p);
				break;
			}

			if (debug)
				fprintf(stderr, _("parameter %s=%d\n"),
					param->name, param->value);

			param->next = bpf_parameter;
			bpf_parameter = param;

			p = strtok(NULL, ":=");
		} while (p);
		break;

	case '?':
		argp_state_help(state, state->out_stream,
				ARGP_HELP_SHORT_USAGE | ARGP_HELP_LONG
				| ARGP_HELP_DOC);
		fprintf(state->out_stream, _("\nReport bugs to %s.\n"), argp_program_bug_address);
		exit(0);
	case 'V':
		fprintf (state->out_stream, "%s\n", argp_program_version);
		exit(0);
	case -3:
		argp_state_help(state, state->out_stream, ARGP_HELP_USAGE);
		exit(0);
	default:
		return ARGP_ERR_UNKNOWN;
	}

	return 0;
}

static struct argp argp = {
	.options = options,
	.parser = parse_opt,
	.args_doc = args_doc,
	.doc = doc,
};

static void prtcode(int *codes)
{
	struct parse_event *p;

	for (p = key_events; p->name != NULL; p++) {
		if (p->value == (unsigned)codes[1]) {
			printf(_("scancode 0x%04x = %s (0x%02x)\n"), codes[0], p->name, codes[1]);
			return;
		}
	}

	if (isprint (codes[1]))
		printf(_("scancode 0x%04x = '%c' (0x%02x)\n"), codes[0], codes[1], codes[1]);
	else
		printf(_("scancode 0x%04x = 0x%02x\n"), codes[0], codes[1]);
}

static void free_names(struct sysfs_names *names)
{
	struct sysfs_names *old;
	do {
		old = names;
		names = names->next;
		if (old->name)
			free(old->name);
		free(old);
	} while (names);
}

static struct sysfs_names *seek_sysfs_dir(char *dname, char *node_name)
{
	DIR             	*dir;
	struct dirent   	*entry;
	struct sysfs_names	*names, *cur_name;

	names = calloc(sizeof(*names), 1);

	cur_name = names;

	dir = opendir(dname);
	if (!dir) {
		perror(dname);
		return NULL;
	}
	entry = readdir(dir);
	while (entry) {
		if (!node_name || !strncmp(entry->d_name, node_name, strlen(node_name))) {
			cur_name->name = malloc(strlen(dname) + strlen(entry->d_name) + 2);
			if (!cur_name->name)
				goto err;
			strcpy(cur_name->name, dname);
			strcat(cur_name->name, entry->d_name);
			if (node_name)
				strcat(cur_name->name, "/");
			cur_name->next = calloc(sizeof(*cur_name), 1);
			if (!cur_name->next)
				goto err;
			cur_name = cur_name->next;
		}
		entry = readdir(dir);
	}
	closedir(dir);

	if (names == cur_name) {
		if (debug)
			fprintf(stderr, _("Couldn't find any node at %s%s*.\n"),
				dname, node_name);

		free (names);
		names = NULL;
	}
	return names;

err:
	perror(_("Seek dir"));
	free_names(names);
	return NULL;
}

static void free_uevent(struct uevents *uevent)
{
	struct uevents *old;
	do {
		old = uevent;
		uevent = uevent->next;
		if (old->key)
			free(old->key);
		if (old->value)
			free(old->value);
		free(old);
	} while (uevent);
}

static struct uevents *read_sysfs_uevents(char *dname)
{
	FILE		*fp;
	struct uevents	*next, *uevent;
	char		*event = "uevent", *file, s[4096];

	next = uevent = calloc(1, sizeof(*uevent));

	file = malloc(strlen(dname) + strlen(event) + 1);
	strcpy(file, dname);
	strcat(file, event);

	if (debug)
		fprintf(stderr, _("Parsing uevent %s\n"), file);


	fp = fopen(file, "r");
	if (!fp) {
		perror(file);
		free(file);
		return NULL;
	}
	while (fgets(s, sizeof(s), fp)) {
		char *p = strtok(s, "=");
		if (!p)
			continue;
		next->key = malloc(strlen(p) + 1);
		if (!next->key) {
			perror("next->key");
			free(file);
			free_uevent(uevent);
			return NULL;
		}
		strcpy(next->key, p);

		p = strtok(NULL, "\n");
		if (!p) {
			fprintf(stderr, _("Error on uevent information\n"));
			fclose(fp);
			free(file);
			free_uevent(uevent);
			return NULL;
		}
		next->value = malloc(strlen(p) + 1);
		if (!next->value) {
			perror("next->value");
			free(file);
			free_uevent(uevent);
			return NULL;
		}
		strcpy(next->value, p);

		if (debug)
			fprintf(stderr, _("%s uevent %s=%s\n"), file, next->key, next->value);

		next->next = calloc(1, sizeof(*next));
		if (!next->next) {
			perror("next->next");
			free(file);
			free_uevent(uevent);
			return NULL;
		}
		next = next->next;
	}
	fclose(fp);
	free(file);

	return uevent;
}

static struct sysfs_names *find_device(char *name)
{
	char		dname[256];
	char		*input = "rc";
	static struct sysfs_names *names, *cur;
	/*
	 * Get event sysfs node
	 */
	snprintf(dname, sizeof(dname), "/sys/class/rc/");

	names = seek_sysfs_dir(dname, input);
	if (!names) {
		fprintf(stderr, _("No devices found\n"));
		return NULL;
	}

	if (debug) {
		for (cur = names; cur->next; cur = cur->next) {
			fprintf(stderr, _("Found device %s\n"), cur->name);
		}
	}

	if (name) {
		static struct sysfs_names *tmp;
		char *p, *n;
		int found = 0;

		n = malloc(strlen(name) + 2);
		strcpy(n, name);
		strcat(n,"/");
		for (cur = names; cur->next; cur = cur->next) {
			if (cur->name) {
				p = cur->name + strlen(dname);
				if (p && !strcmp(p, n)) {
					found = 1;
					break;
				}
			}
		}
		free(n);
		if (!found) {
			free_names(names);
			fprintf(stderr, _("Not found device %s\n"), name);
			return NULL;
		}
		tmp = calloc(sizeof(*names), 1);
		tmp->name = cur->name;
		cur->name = NULL;
		free_names(names);
		return tmp;
	}

	return names;
}

static enum sysfs_protocols v1_get_hw_protocols(char *name)
{
	FILE *fp;
	char *p, buf[4096];
	enum sysfs_protocols protocols = 0;

	fp = fopen(name, "r");
	if (!fp) {
		perror(name);
		return 0;
	}

	if (!fgets(buf, sizeof(buf), fp)) {
		perror(name);
		fclose(fp);
		return 0;
	}

	for (p = strtok(buf, " \n"); p; p = strtok(NULL, " \n")) {
		enum sysfs_protocols protocol;

		if (debug)
			fprintf(stderr, _("%s protocol %s\n"), name, p);

		protocol = parse_sysfs_protocol(p, false);
		if (protocol == SYSFS_INVALID)
			protocol = SYSFS_OTHER;

		protocols |= protocol;
	}

	fclose(fp);

	return protocols;
}

static int v1_set_hw_protocols(struct rc_device *rc_dev)
{
	FILE *fp;
	char name[4096];

	strcpy(name, rc_dev->sysfs_name);
	strcat(name, "/protocol");

	fp = fopen(name, "w");
	if (!fp) {
		perror(name);
		return errno;
	}

	write_sysfs_protocols(rc_dev->current, fp, "%s ");

	fprintf(fp, "\n");

	fclose(fp);

	return 0;
}

static int v1_get_sw_enabled_protocol(char *dirname)
{
	FILE *fp;
	char *p, buf[4096], name[512];
	int rc;

	strcpy(name, dirname);
	strcat(name, "/enabled");

	fp = fopen(name, "r");
	if (!fp) {
		perror(name);
		return 0;
	}

	if (!fgets(buf, sizeof(buf), fp)) {
		perror(name);
		fclose(fp);
		return 0;
	}

	if (fclose(fp)) {
		perror(name);
		return errno;
	}

	p = strtok(buf, " \n");
	if (!p) {
		fprintf(stderr, _("%s has invalid content: '%s'\n"), name, buf);
		return 0;
	}

	rc = atoi(p);

	if (debug)
		fprintf(stderr, _("protocol %s is %s\n"),
			name, rc? _("enabled") : _("disabled"));

	if (atoi(p) == 1)
		return 1;

	return 0;
}

static int v1_set_sw_enabled_protocol(struct rc_device *rc_dev,
				      const char *dirname, int enabled)
{
	FILE *fp;
	char name[512];

	strcpy(name, rc_dev->sysfs_name);
	strcat(name, dirname);
	strcat(name, "/enabled");

	fp = fopen(name, "w");
	if (!fp) {
		perror(name);
		return errno;
	}

	if (enabled)
		fprintf(fp, "1");
	else
		fprintf(fp, "0");

	if (fclose(fp)) {
		perror(name);
		return errno;
	}

	return 0;
}

static enum sysfs_protocols v2_get_protocols(struct rc_device *rc_dev, char *name)
{
	FILE *fp;
	char *p, buf[4096];
	int enabled;

	fp = fopen(name, "r");
	if (!fp) {
		perror(name);
		return 0;
	}

	if (!fgets(buf, sizeof(buf), fp)) {
		perror(name);
		fclose(fp);
		return 0;
	}

	for (p = strtok(buf, " \n"); p; p = strtok(NULL, " \n")) {
		enum sysfs_protocols protocol;

		if (*p == '[') {
			enabled = 1;
			p++;
			p[strlen(p)-1] = '\0';
		} else
			enabled = 0;

		if (debug)
			fprintf(stderr, _("%s protocol %s (%s)\n"), name, p,
				enabled? _("enabled") : _("disabled"));

		protocol = parse_sysfs_protocol(p, false);
		if (protocol == SYSFS_INVALID)
			protocol = SYSFS_OTHER;

		rc_dev->supported |= protocol;
		if (enabled)
			rc_dev->current |= protocol;

	}

	fclose(fp);

	return 0;
}

static int v2_set_protocols(struct rc_device *rc_dev)
{
	FILE *fp;
	char name[4096];
	struct stat st;

	strcpy(name, rc_dev->sysfs_name);
	strcat(name, "/protocols");

	if (!stat(name, &st) && !(st.st_mode & 0222)) {
		fprintf(stderr, _("Protocols for device can not be changed\n"));
		return 0;
	}

	fp = fopen(name, "w");
	if (!fp) {
		perror(name);
		return errno;
	}

	/* Disable all protocols */
	fprintf(fp, "none\n");

	write_sysfs_protocols(rc_dev->current, fp, "+%s\n");

	if (fclose(fp)) {
		perror(name);
		return errno;
	}

	return 0;
}

static int get_attribs(struct rc_device *rc_dev, char *sysfs_name)
{
	struct uevents  *uevent;
	char		*input = "input", *event = "event", *lirc = "lirc";
	char		*DEV = "/dev/";
	static struct sysfs_names *input_names, *event_names, *attribs, *cur, *lirc_names;

	/* Clean the attributes */
	memset(rc_dev, 0, sizeof(*rc_dev));

	rc_dev->sysfs_name = sysfs_name;

	lirc_names = seek_sysfs_dir(rc_dev->sysfs_name, lirc);
	if (lirc_names) {
		uevent = read_sysfs_uevents(lirc_names->name);
		free_names(lirc_names);
		if (uevent) {
			while (uevent->next) {
				if (!strcmp(uevent->key, "DEVNAME")) {
					rc_dev->lirc_name = malloc(strlen(uevent->value) + strlen(DEV) + 1);
					strcpy(rc_dev->lirc_name, DEV);
					strcat(rc_dev->lirc_name, uevent->value);
					break;
				}
				uevent = uevent->next;
			}
			free_uevent(uevent);
		}
	}

	input_names = seek_sysfs_dir(rc_dev->sysfs_name, input);
	if (!input_names)
		return EINVAL;
	if (input_names->next->next) {
		fprintf(stderr, _("Found more than one input interface. This is currently unsupported\n"));
		return EINVAL;
	}
	if (debug)
		fprintf(stderr, _("Input sysfs node is %s\n"), input_names->name);

	event_names = seek_sysfs_dir(input_names->name, event);
	if (!event_names) {
		fprintf(stderr, _("Couldn't find any node at %s%s*.\n"),
			input_names->name, event);
		free_names(input_names);
		return EINVAL;
	}
	free_names(input_names);
	if (event_names->next->next) {
		free_names(event_names);
		fprintf(stderr, _("Found more than one event interface. This is currently unsupported\n"));
		return EINVAL;
	}
	if (debug)
		fprintf(stderr, _("Event sysfs node is %s\n"), event_names->name);

	uevent = read_sysfs_uevents(event_names->name);
	free_names(event_names);
	if (!uevent)
		return EINVAL;

	while (uevent->next) {
		if (!strcmp(uevent->key, "DEVNAME")) {
			rc_dev->input_name = malloc(strlen(uevent->value) + strlen(DEV) + 1);
			strcpy(rc_dev->input_name, DEV);
			strcat(rc_dev->input_name, uevent->value);
			break;
		}
		uevent = uevent->next;
	}
	free_uevent(uevent);

	if (!rc_dev->input_name) {
		fprintf(stderr, _("Input device name not found.\n"));
		return EINVAL;
	}

	uevent = read_sysfs_uevents(rc_dev->sysfs_name);
	if (!uevent)
		return EINVAL;
	while (uevent->next) {
		if (!strcmp(uevent->key, "DRV_NAME")) {
			rc_dev->drv_name = malloc(strlen(uevent->value) + 1);
			strcpy(rc_dev->drv_name, uevent->value);
		}
		if (!strcmp(uevent->key, "DEV_NAME")) {
			rc_dev->dev_name = malloc(strlen(uevent->value) + 1);
			strcpy(rc_dev->dev_name, uevent->value);
		}
		if (!strcmp(uevent->key, "NAME")) {
			rc_dev->keytable_name = malloc(strlen(uevent->value) + 1);
			strcpy(rc_dev->keytable_name, uevent->value);
		}
		uevent = uevent->next;
	}
	free_uevent(uevent);

	if (debug)
		fprintf(stderr, _("input device is %s\n"), rc_dev->input_name);

	sysfs++;

	rc_dev->type = SOFTWARE_DECODER;

	/* Get the other attribs - basically IR decoders */
	attribs = seek_sysfs_dir(rc_dev->sysfs_name, NULL);
	for (cur = attribs; cur->next; cur = cur->next) {
		if (!cur->name)
			continue;
		if (strstr(cur->name, "/protocols")) {
			rc_dev->version = VERSION_2;
			rc_dev->type = UNKNOWN_TYPE;
			v2_get_protocols(rc_dev, cur->name);
		} else if (strstr(cur->name, "/protocol")) {
			rc_dev->version = VERSION_1;
			rc_dev->type = HARDWARE_DECODER;
			rc_dev->current = v1_get_hw_protocols(cur->name);
		} else if (strstr(cur->name, "/supported_protocols")) {
			rc_dev->version = VERSION_1;
			rc_dev->supported = v1_get_hw_protocols(cur->name);
		} else {
			const struct protocol_map_entry *pme;

			for (pme = protocol_map; pme->name; pme++) {
				if (!pme->sysfs1_name)
					continue;

				if (strstr(cur->name, pme->sysfs1_name)) {
					rc_dev->supported |= pme->sysfs_protocol;
					if (v1_get_sw_enabled_protocol(cur->name))
						rc_dev->supported |= pme->sysfs_protocol;
					break;
				}
			}
		}
	}

	return 0;
}

static int set_proto(struct rc_device *rc_dev)
{
	int rc = 0;

	rc_dev->current &= rc_dev->supported;

	if (rc_dev->version == VERSION_2) {
		rc = v2_set_protocols(rc_dev);
		return rc;
	}

	if (rc_dev->type == SOFTWARE_DECODER) {
		const struct protocol_map_entry *pme;

		for (pme = protocol_map; pme->name; pme++) {
			if (!pme->sysfs1_name)
				continue;

			if (!(rc_dev->supported & pme->sysfs_protocol))
				continue;

			rc += v1_set_sw_enabled_protocol(rc_dev, pme->sysfs1_name,
							 rc_dev->current & pme->sysfs_protocol);
		}

	} else {
		rc = v1_set_hw_protocols(rc_dev);
	}

	return rc;
}

static int get_input_protocol_version(int fd)
{
	if (ioctl(fd, EVIOCGVERSION, &input_protocol_version) < 0) {
		fprintf(stderr,
			_("Unable to query evdev protocol version: %s\n"),
			strerror(errno));
		return errno;
	}
	if (debug)
		fprintf(stderr, _("Input Protocol version: 0x%08x\n"),
			input_protocol_version);

	return 0;
}

static void clear_table(int fd)
{
	int i, j;
	u_int32_t codes[2];
	struct input_keymap_entry_v2 entry;

	/* Clears old table */
	if (input_protocol_version < 0x10001) {
		for (j = 0; j < 256; j++) {
			for (i = 0; i < 256; i++) {
				codes[0] = (j << 8) | i;
				codes[1] = KEY_RESERVED;
				ioctl(fd, EVIOCSKEYCODE, codes);
			}
		}
	} else {
		memset(&entry, '\0', sizeof(entry));
		i = 0;
		do {
			entry.flags = KEYMAP_BY_INDEX;
			entry.keycode = KEY_RESERVED;
			entry.index = 0;

			i++;
			if (debug)
				fprintf(stderr, _("Deleting entry %d\n"), i);
		} while (ioctl(fd, EVIOCSKEYCODE_V2, &entry) == 0);
	}
}

static int add_keys(int fd)
{
	int write_cnt = 0;
	struct keytable_entry *ke;
	unsigned codes[2];

	for (ke = keytable; ke; ke = ke->next) {
		write_cnt++;
		if (debug)
			fprintf(stderr, "\t%04x=%04x\n",
				ke->scancode, ke->keycode);

		codes[0] = ke->scancode;
		codes[1] = ke->keycode;

		if (ioctl(fd, EVIOCSKEYCODE, codes)) {
			fprintf(stderr,
				_("Setting scancode 0x%04x with 0x%04x via "),
				ke->scancode, ke->keycode);
			perror("EVIOCSKEYCODE");
		}
	}

	while (keytable) {
		ke = keytable;
		keytable = ke->next;
		free(ke);
	}

	return write_cnt;
}

static void display_proto(struct rc_device *rc_dev)
{
	if (rc_dev->type == HARDWARE_DECODER)
		fprintf(stderr, _("Current kernel protocols: "));
	else
		fprintf(stderr, _("Enabled kernel protocols: "));
	write_sysfs_protocols(rc_dev->current, stderr, "%s ");
	fprintf(stderr, "\n");
}


static char *get_event_name(struct parse_event *event, u_int16_t code)
{
	struct parse_event *p;

	for (p = event; p->name != NULL; p++) {
		if (p->value == code)
			return p->name;
	}
	return "";
}

static void print_scancodes(const struct lirc_scancode *scancodes, unsigned count)
{
	unsigned i;

	for (i=0; i< count; i++)  {
		const char *p = protocol_name(scancodes[i].rc_proto);

		printf(_("%llu.%06llu: "),
			scancodes[i].timestamp / 1000000000ull,
			(scancodes[i].timestamp % 1000000000ull) / 1000ull);

		if (p)
			printf(_("lirc protocol(%s): scancode = 0x%llx"),
				p, scancodes[i].scancode);
		else
			printf(_("lirc protocol(%d): scancode = 0x%llx"),
				scancodes[i].rc_proto, scancodes[i].scancode);

		if (scancodes[i].flags & LIRC_SCANCODE_FLAG_REPEAT)
			printf(_(" repeat"));
		if (scancodes[i].flags & LIRC_SCANCODE_FLAG_TOGGLE)
			printf(_(" toggle=1"));

		printf("\n");
	}
}

static void test_event(struct rc_device *rc_dev, int fd)
{
	struct input_event ev[64];
	struct lirc_scancode sc[64];
	int rd, i, lircfd = -1;
	unsigned mode;

	/* LIRC reports time in monotonic, set event to same */
	mode = CLOCK_MONOTONIC;
	ioctl(fd, EVIOCSCLOCKID, &mode);

	if (rc_dev->lirc_name) {
		unsigned mode = LIRC_MODE_SCANCODE;
		lircfd = open(rc_dev->lirc_name, O_RDONLY | O_NONBLOCK);
		if (lircfd == -1) {
			perror(_("Can't open lirc device"));
			return;
		}
		if (ioctl(lircfd, LIRC_SET_REC_MODE, &mode)) {
			/* If we can't set scancode mode, kernel is too old */
			close(lircfd);
			lircfd = -1;
		}
	}

	printf (_("Testing events. Please, press CTRL-C to abort.\n"));
	while (1) {
		struct pollfd pollstruct[2] = {
			{ .fd = fd, .events = POLLIN },
			{ .fd = lircfd, .events = POLLIN },
		};

		if (poll(pollstruct, 2, -1) < 0) {
			if (errno == EINTR)
				continue;

			perror(_("poll returned error"));
		}

		if (lircfd != -1) {
			rd = read(lircfd, sc, sizeof(sc));

			if (rd != -1) {
				print_scancodes(sc, rd / sizeof(struct lirc_scancode));
			} else if (errno != EAGAIN) {
				perror(_("Error reading lirc scancode"));
				return;
			}
		}

		rd = read(fd, ev, sizeof(ev));

		if (rd < (int) sizeof(struct input_event)) {
			if (errno == EAGAIN)
				continue;

			perror(_("Error reading event"));
			return;
		}

		for (i = 0; i < rd / sizeof(struct input_event); i++) {
			printf(_("%ld.%06ld: event type %s(0x%02x)"),
				ev[i].time.tv_sec, ev[i].time.tv_usec,
				get_event_name(events_type, ev[i].type), ev[i].type);

			switch (ev[i].type) {
			case EV_SYN:
				printf(".\n");
				break;
			case EV_KEY:
				printf(_(" key_%s: %s(0x%04x)\n"),
					(ev[i].value == 0) ? _("up") : _("down"),
					get_event_name(key_events, ev[i].code),
					ev[i].code);
				break;
			case EV_REL:
				printf(_(": %s (0x%04x) value=%d\n"),
					get_event_name(rel_events, ev[i].code),
					ev[i].code,
					ev[i].value);
				break;
			case EV_ABS:
				printf(_(": %s (0x%04x) value=%d\n"),
					get_event_name(abs_events, ev[i].code),
					ev[i].code,
					ev[i].value);
				break;
			case EV_MSC:
				if (ev[i].code == MSC_SCAN)
					printf(_(": scancode = 0x%02x\n"), ev[i].value);
				else
					printf(_(": code = %s(0x%02x), value = %d\n"),
						get_event_name(msc_events, ev[i].code),
						ev[i].code, ev[i].value);
				break;
			case EV_REP:
				printf(_(": value = %d\n"), ev[i].value);
				break;
			case EV_SW:
			case EV_LED:
			case EV_SND:
			case EV_FF:
			case EV_PWR:
			case EV_FF_STATUS:
			default:
				printf(_(": code = 0x%02x, value = %d\n"),
					ev[i].code, ev[i].value);
				break;
			}
		}
	}
}

static void display_table_v1(struct rc_device *rc_dev, int fd)
{
	unsigned int i, j;

	for (j = 0; j < 256; j++) {
		for (i = 0; i < 256; i++) {
			int codes[2];

			codes[0] = (j << 8) | i;
			if (ioctl(fd, EVIOCGKEYCODE, codes) == -1)
				perror("EVIOCGKEYCODE");
			else if (codes[1] != KEY_RESERVED)
				prtcode(codes);
		}
	}
	display_proto(rc_dev);
}

static void display_table_v2(struct rc_device *rc_dev, int fd)
{
	int i;
	struct input_keymap_entry_v2 entry;
	int codes[2];

	memset(&entry, '\0', sizeof(entry));
	i = 0;
	do {
		entry.flags = KEYMAP_BY_INDEX;
		entry.index = i;
		entry.len = sizeof(u_int32_t);

		if (ioctl(fd, EVIOCGKEYCODE_V2, &entry) == -1)
			break;

		/* FIXME: Extend it to support scancodes > 32 bits */
		memcpy(&codes[0], entry.scancode, sizeof(codes[0]));
		codes[1] = entry.keycode;

		prtcode(codes);
		i++;
	} while (1);
	display_proto(rc_dev);
}

static void display_table(struct rc_device *rc_dev, int fd)
{
	if (input_protocol_version < 0x10001)
		display_table_v1(rc_dev, fd);
	else
		display_table_v2(rc_dev, fd);
}

static int set_rate(int fd, unsigned int delay, unsigned int period)
{
	unsigned int rep[2] = { delay, period };

	if (ioctl(fd, EVIOCSREP, rep) < 0) {
		perror("evdev ioctl");
		return -1;
	}

	printf(_("Changed Repeat delay to %d ms and repeat period to %d ms\n"), delay, period);
	return 0;
}

static int get_rate(int fd, unsigned int *delay, unsigned int *period)
{
	unsigned int rep[2];

	if (ioctl(fd, EVIOCGREP, rep) < 0) {
		perror("evdev ioctl");
		return -1;
	}
	*delay = rep[0];
	*period = rep[1];
	printf(_("Repeat delay = %d ms, repeat period = %d ms\n"), *delay, *period);
	return 0;
}

static void show_evdev_attribs(int fd)
{
	unsigned int delay, period;

	printf("\t");
	get_rate(fd, &delay, &period);
}

static void device_name(int fd, char *prepend)
{
	char buf[32];
	int rc;

	rc = ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
	if (rc >= 0)
		fprintf(stderr,_("%sName: %.*s\n"),prepend, rc, buf);
	else
		perror ("EVIOCGNAME");
}

static void device_info(int fd, char *prepend)
{
	struct input_id id;
	int rc;

	rc = ioctl(fd, EVIOCGID, &id);
	if (rc >= 0)
		fprintf(stderr,
			_("%sbus: %d, vendor/product: %04x:%04x, version: 0x%04x\n"),
			prepend, id.bustype, id.vendor, id.product, id.version);
	else
		perror ("EVIOCGID");
}

#ifdef HAVE_BPF
#define MAX_PROGS 64
static void attach_bpf(const char *lirc_name, const char *bpf_prog, struct toml_table_t *toml)
{
	unsigned int features;
	int fd;

	fd = open(lirc_name, O_RDONLY);
	if (fd == -1) {
		perror(lirc_name);
		return;
	}

	if (ioctl(fd, LIRC_GET_FEATURES, &features)) {
		perror(lirc_name);
		close(fd);
		return;
	}

	if (!(features & LIRC_CAN_REC_MODE2)) {
		fprintf(stderr, _("%s: not a raw IR receiver\n"), lirc_name);
		close(fd);
		return;
	}

	load_bpf_file(bpf_prog, fd, toml);
	close(fd);
}

static void show_bpf(const char *lirc_name)
{
	unsigned int prog_ids[MAX_PROGS], count = MAX_PROGS;
	unsigned int features, i;
	int ret, fd, prog_fd;

	fd = open(lirc_name, O_RDONLY);
	if (fd == -1)
		goto error;

	if (ioctl(fd, LIRC_GET_FEATURES, &features)) {
		close(fd);
		goto error;
	}

	if (!(features & LIRC_CAN_REC_MODE2)) {
		// only support for mode2 type raw ir devices
		close(fd);
		return;
	}

	ret = bpf_prog_query(fd, BPF_LIRC_MODE2, 0, NULL, prog_ids, &count);
	close(fd);
	if (ret) {
		if (errno == EINVAL)
			errno = ENOTSUP;
		goto error;
	}

	printf(_("\tAttached BPF protocols: "));
	for (i=0; i<count; i++) {
		if (i)
			printf(" ");
		prog_fd = bpf_prog_get_fd_by_id(prog_ids[i]);
		if (prog_fd != -1) {
			struct bpf_prog_info info = {};
			__u32 info_len = sizeof(info);

			ret = bpf_obj_get_info_by_fd(prog_fd, &info, &info_len);
			close(prog_fd);
			if (!ret && info.name[0]) {
				printf("%s", info.name);
				continue;
			}
		}
		printf("%d", prog_ids[i]);
	}
	printf(_("\n"));
	return;
error:
	printf(_("\tAttached BPF protocols: %m\n"));
}

static void clear_bpf(const char *lirc_name)
{
	unsigned int prog_ids[MAX_PROGS], count = MAX_PROGS;
	unsigned int features, i;
	int ret, prog_fd, fd;

	fd = open(lirc_name, O_RDONLY);
	if (fd == -1) {
		perror(lirc_name);
		return;
	}

	if (ioctl(fd, LIRC_GET_FEATURES, &features)) {
		perror(lirc_name);
		close(fd);
		return;
	}

	if (!(features & LIRC_CAN_REC_MODE2)) {
		// only support for mode2 type raw ir devices
		close(fd);
		return;
	}

	ret = bpf_prog_query(fd, BPF_LIRC_MODE2, 0, NULL, prog_ids, &count);
	if (ret) {
		close(fd);
		return;
	}

	for (i = 0; i < count; i++) {
		if (debug)
			fprintf(stderr, _("BPF protocol prog_id %d\n"),
				prog_ids[i]);
		prog_fd = bpf_prog_get_fd_by_id(prog_ids[i]);
		if (prog_fd == -1) {
			printf(_("Failed to get BPF prog id %u: %m\n"),
			       prog_ids[i]);
			continue;
		}
		ret = bpf_prog_detach2(prog_fd, fd, BPF_LIRC_MODE2);
		if (ret)
			printf(("Failed to detach BPF prog id %u: %m\n"),
			       prog_ids[i]);
		close(prog_fd);
	}
	close(fd);
	if (debug)
		fprintf(stderr, _("BPF protocols removed\n"));
}
#else
static void attach_bpf(const char *lirc_name, const char *bpf_prog, struct toml_table_t *toml)
{
	fprintf(stderr, _("error: ir-keytable was compiled without BPF support\n"));
}
static void show_bpf(const char *lirc_name) {}
static void clear_bpf(const char *lirc_name) {}
#endif

static int show_sysfs_attribs(struct rc_device *rc_dev, char *name)
{
	static struct sysfs_names *names, *cur;
	int fd;

	names = find_device(name);
	if (!names)
		return -1;
	for (cur = names; cur; cur = cur->next) {
		if (cur->name) {
			if (get_attribs(rc_dev, cur->name))
				continue;
			fprintf(stderr, _("Found %s (%s) with:\n"),
				rc_dev->sysfs_name,
				rc_dev->input_name);
			if (rc_dev->dev_name)
				fprintf(stderr, _("\tName: %s\n"),
					rc_dev->dev_name);
			fprintf(stderr, _("\tDriver: %s, table: %s\n"),
				rc_dev->drv_name,
				rc_dev->keytable_name);
			if (rc_dev->lirc_name) {
				fprintf(stderr, _("\tLIRC device: %s\n"),
					rc_dev->lirc_name);
				show_bpf(rc_dev->lirc_name);
			}
			fprintf(stderr, _("\tSupported kernel protocols: "));
			write_sysfs_protocols(rc_dev->supported, stderr, "%s ");
			fprintf(stderr, "\n\t");
			display_proto(rc_dev);
			fd = open(rc_dev->input_name, O_RDONLY);
			if (fd > 0) {
				if (!rc_dev->dev_name)
					device_name(fd, "\t");
				device_info(fd, "\t");
				show_evdev_attribs(fd);
				close(fd);
			} else {
				printf(_("\tExtra capabilities: <access denied>\n"));
			}
		}
	}
	return 0;
}

static char *find_bpf_file(const char *name)
{
	struct stat st;
	char *fname;

	if (!stat(name, &st))
		return strdup(name);

	if (asprintf(&fname, IR_PROTOCOLS_USER_DIR "/%s.o", name) < 0) {
		fprintf(stderr, _("asprintf failed: %m\n"));
		return NULL;
	}

	if (stat(fname, &st)) {
		free(fname);
		if (asprintf(&fname, IR_PROTOCOLS_SYSTEM_DIR "/%s.o", name) < 0) {
			fprintf(stderr, _("asprintf failed: %m\n"));
			return NULL;
		}

		if (stat(fname, &st)) {
			fprintf(stderr, _("Can't find %s bpf protocol in %s or %s\n"), name, IR_KEYTABLE_USER_DIR "/protocols", IR_KEYTABLE_SYSTEM_DIR "/protocols");
			free(fname);
			return NULL;
		}
	}

	return fname;
}

int bpf_param(const char *name, int *val)
{
	struct bpf_parameter *param = bpf_parameter;

	while (param) {
		if (strcmp(name, param->name) == 0) {
			*val = param->value;
			return 0;
		}
		param = param->next;
	}

	return -ENOENT;
}

int main(int argc, char *argv[])
{
	int dev_from_class = 0, write_cnt;
	int fd;
	static struct sysfs_names *names;
	struct rc_device	  rc_dev;

#ifdef ENABLE_NLS
	setlocale (LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	textdomain (PACKAGE);
#endif

	argp_parse(&argp, argc, argv, ARGP_NO_HELP, 0, 0);

	/* Just list all devices */
	if (!clear && !readtable && !keytable && !ch_proto && !cfg.next && !test && delay < 0 && period < 0 && !bpf_protocol) {
		if (devicename) {
			fd = open(devicename, O_RDONLY);
			if (fd < 0) {
				perror(_("Can't open device"));
				return -1;
			}
			device_name(fd, "");
			device_info(fd, "");
			close(fd);
			return 0;
		}
		if (show_sysfs_attribs(&rc_dev, devclass))
			return -1;

		return 0;
	}

	if (!devclass)
		devclass = "rc0";

	if (cfg.next && (clear || keytable || ch_proto || devicename)) {
		fprintf (stderr, _("Auto-mode can be used only with --read, --debug and --sysdev options\n"));
		return -1;
	}
	if (!devicename) {
		names = find_device(devclass);
		if (!names)
			return -1;
		rc_dev.sysfs_name = names->name;
		if (get_attribs(&rc_dev, names->name)) {
			free_names(names);
			return -1;
		}
		names->name = NULL;
		free_names(names);

		devicename = rc_dev.input_name;
		dev_from_class++;
	}

	if (cfg.next) {
		struct cfgfile *cur;
		char *fname, *name;
		int rc;

		for (cur = &cfg; cur->next; cur = cur->next) {
			if ((!rc_dev.drv_name || strcasecmp(cur->driver, rc_dev.drv_name)) && strcasecmp(cur->driver, "*"))
				continue;
			if ((!rc_dev.keytable_name || strcasecmp(cur->table, rc_dev.keytable_name)) && strcasecmp(cur->table, "*"))
				continue;
			break;
		}

		if (!cur->next) {
			if (debug)
				fprintf(stderr, _("Table for %s, %s not found. Keep as-is\n"),
				       rc_dev.drv_name, rc_dev.keytable_name);
			return 0;
		}
		if (debug)
			fprintf(stderr, _("Table for %s, %s is on %s file.\n"),
				rc_dev.drv_name, rc_dev.keytable_name,
				cur->fname);
		if (cur->fname[0] == '/' || ((cur->fname[0] == '.') && strchr(cur->fname, '/'))) {
			fname = cur->fname;
			rc = parse_keyfile(fname, &name);
			if (rc < 0) {
				fprintf(stderr, _("Can't load %s table\n"), fname);
				return -1;
			}
		} else {
			fname = malloc(strlen(cur->fname) + strlen(IR_KEYTABLE_USER_DIR) + 2);
			strcpy(fname, IR_KEYTABLE_USER_DIR);
			strcat(fname, "/");
			strcat(fname, cur->fname);
			rc = parse_keyfile(fname, &name);
			if (rc != 0) {
				fname = malloc(strlen(cur->fname) + strlen(IR_KEYTABLE_SYSTEM_DIR) + 2);
				strcpy(fname, IR_KEYTABLE_SYSTEM_DIR);
				strcat(fname, "/");
				strcat(fname, cur->fname);
				rc = parse_keyfile(fname, &name);
			}
			if (rc != 0) {
				fprintf(stderr, _("Can't load %s table from %s or %s\n"), cur->fname, IR_KEYTABLE_USER_DIR, IR_KEYTABLE_SYSTEM_DIR);
				return -1;
			}
		}
		if (!keytable) {
			fprintf(stderr, _("Empty table %s\n"), fname);
			return -1;
		}
		clear = 1;
	}

	if (debug)
		fprintf(stderr, _("Opening %s\n"), devicename);
	fd = open(devicename, O_RDONLY | O_NONBLOCK);
	if (fd < 0) {
		perror(devicename);
		return -1;
	}
	if (dev_from_class)
		free(devicename);
	if (get_input_protocol_version(fd))
		return -1;

	/*
	 * First step: clear, if --clear is specified
	 */
	if (clear) {
		clear_table(fd);
		fprintf(stderr, _("Old keytable cleared\n"));
	}

	/*
	 * Second step: stores key tables from file or from commandline
	 */
	write_cnt = add_keys(fd);
	if (write_cnt)
		fprintf(stderr, _("Wrote %d keycode(s) to driver\n"), write_cnt);

	/*
	 * Third step: change protocol
	 */
	if (ch_proto || bpf_protocol) {
		if (rc_dev.lirc_name)
			clear_bpf(rc_dev.lirc_name);

		rc_dev.current = ch_proto;
		if (set_proto(&rc_dev))
			fprintf(stderr, _("Couldn't change the IR protocols\n"));
		else {
			fprintf(stderr, _("Protocols changed to "));
			write_sysfs_protocols(rc_dev.current, stderr, "%s ");
			fprintf(stderr, "\n");
		}
	}

	if (bpf_protocol) {
		struct bpf_protocol *b;

		if (!rc_dev.lirc_name) {
			fprintf(stderr, _("Error: unable to attach bpf program, lirc device name was not found\n"));
		}

		for (b = bpf_protocol; b && rc_dev.lirc_name; b = b->next) {
			char *fname = find_bpf_file(b->name);

			if (fname) {
				attach_bpf(rc_dev.lirc_name, fname, b->toml);
				fprintf(stderr, _("Loaded BPF protocol %s\n"), b->name);
				free(fname);
			}
		}
	}

	/*
	 * Fourth step: display current keytable
	 */
	if (readtable)
		display_table(&rc_dev, fd);

	/*
	 * Fiveth step: change repeat rate/delay
	 */
	if (delay >= 0 || period >= 0) {
		unsigned int new_delay, new_period;
		get_rate(fd, &new_delay, &new_period);
		if (delay >= 0)
			new_delay = delay;
		if (period >= 0)
			new_period = period;
		set_rate(fd, new_delay, new_period);
	}

	if (test)
		test_event(&rc_dev, fd);

	return 0;
}
