/*
 * Derived from the util-linux/mount/mount_by_label.c source,
 * currently maintained by Andries Brouwer <aeb@cwi.nl>.
 *
 * 1999-02-22 Arkadiusz Mi¶kiewicz <misiek@misiek.eu.org>
 * - added Native Language Support
 * 2000-01-20 James Antill <james@and.org>
 * - Added error message if /proc/partitions cannot be opened
 * 2000-05-09 Erik Troan <ewt@redhat.com>
 * - Added cache for UUID and disk labels
 * 2000-11-07 Nathan Scott <nathans@sgi.com>
 * - Added XFS support
 */

#include "config.h"

#include <stdio.h>
#include <sys/param.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include "bylabel.h"
#include "common.h"
#include "pot.h"

#define PROC_PARTITIONS "/proc/partitions"
#define DEVLABELDIR	"/dev"

static struct uuidCache_s {
	struct uuidCache_s *next;
	char uuid[16];
	char *label;
	char *device;
} *uuidCache = NULL;

#define EXT2_SUPER_MAGIC	0xEF53
struct ext2_super_block {
	uint8_t s_dummy1[56];
	uint8_t s_magic[2];
	uint8_t s_dummy2[46];
	uint8_t s_uuid[16];
	uint8_t s_volume_name[16];
};

#define ext2magic(s)	((uint32_t) s.s_magic[0] + (((uint32_t) s.s_magic[1]) << 8))

#define XFS_SUPER_MAGIC "XFSB"
#define XFS_SUPER_MAGIC2 "BSFX"
#define EXFS_SUPER_MAGIC "EXFS"
struct xfs_super_block {
	uint8_t s_magic[4];
	uint8_t s_dummy[28];
	uint8_t s_uuid[16];
	uint8_t s_dummy2[60];
	uint8_t s_fsname[12];
};

#define REISER_SUPER_MAGIC	"ReIsEr2Fs"
struct reiserfs_super_block {
	uint8_t s_dummy1[52];
	uint8_t s_magic[10];
	uint8_t s_dummy2[22];
	uint8_t s_uuid[16];
	uint8_t s_volume_name[16];
};

#define F2FS_SUPER_MAGIC	"0xF2F52010"
struct f2fs_super_block {
	uint8_t s_magic[8];
	uint8_t s_dummy[144];
	uint8_t s_uuid[16];
	uint8_t s_volume_name[512];
};

static inline unsigned short swapped(unsigned short a)
{
	return (a >> 8) | (a << 8);
}

/* for now, only ext2 and xfs are supported */
static int get_label_uuid(const char *device, char **label, char *uuid)
{

	/* start with ext2 and xfs tests, taken from mount_guess_fstype */
	/* should merge these later */
	int fd, rv = 1;
	size_t namesize;
	struct ext2_super_block e2sb;
	struct xfs_super_block xfsb;
	struct reiserfs_super_block reisersb;
	struct f2fs_super_block f2fssb;

	fd = open(device, O_RDONLY);
	if (fd < 0)
		return rv;

	if (lseek(fd, 1024, SEEK_SET) == 1024
	    && read(fd, (char *)&e2sb, sizeof(e2sb)) == sizeof(e2sb)
	    && ext2magic(e2sb) == EXT2_SUPER_MAGIC) {
		memcpy(uuid, e2sb.s_uuid, sizeof(e2sb.s_uuid));
		namesize = sizeof(e2sb.s_volume_name);
		*label = smalloc(namesize + 1);
		sstrncpy(*label, (char *)e2sb.s_volume_name, namesize);
		rv = 0;
	}
	else if (lseek(fd, 0, SEEK_SET) == 0
		 && read(fd, (char *)&xfsb, sizeof(xfsb)) == sizeof(xfsb)
		 && (strncmp((char *)&xfsb.s_magic, XFS_SUPER_MAGIC, 4) == 0 ||
		     strncmp((char *)&xfsb.s_magic, XFS_SUPER_MAGIC2, 4) == 0 ||
		     strncmp((char *)&xfsb.s_magic, EXFS_SUPER_MAGIC, 4) == 0)) {
		memcpy(uuid, xfsb.s_uuid, sizeof(xfsb.s_uuid));
		namesize = sizeof(xfsb.s_fsname);
		*label = smalloc(namesize + 1);
		sstrncpy(*label, (char *)xfsb.s_fsname, namesize);
		rv = 0;
	}
	else if (lseek(fd, 65536, SEEK_SET) == 65536
		&& read(fd, (char *)&reisersb, sizeof(reisersb)) == sizeof(reisersb)
		&& !strncmp((char *)&reisersb.s_magic, REISER_SUPER_MAGIC, 9)) {
		memcpy(uuid, reisersb.s_uuid, sizeof(reisersb.s_uuid));
		namesize = sizeof(reisersb.s_volume_name);
		*label = smalloc(namesize + 1);
		sstrncpy(*label, (char *)reisersb.s_volume_name, namesize);
		rv = 0;
	}
	else if (lseek(fd, 65536, SEEK_SET) == 65536
		&& read(fd, (char *)&f2fssb, sizeof(f2fssb)) == sizeof(f2fssb)
		&& !strncmp((char *)&f2fssb.s_magic, F2FS_SUPER_MAGIC, 8)) {
		memcpy(uuid, f2fssb.s_uuid, sizeof(f2fssb.s_uuid));
		namesize = sizeof(f2fssb.s_volume_name);
		*label = smalloc(namesize + 1);
		sstrncpy(*label, (char *)f2fssb.s_volume_name, namesize);
		rv = 0;
	}
	close(fd);
	return rv;
}

static void uuidcache_addentry(char *device, char *label, char *uuid)
{
	struct uuidCache_s *last;

	if (!uuidCache) {
		last = uuidCache = smalloc(sizeof(*uuidCache));
	}
	else {
		for (last = uuidCache; last->next; last = last->next);
		last->next = smalloc(sizeof(*uuidCache));
		last = last->next;
	}
	last->next = NULL;
	last->device = device;
	last->label = label;
	memcpy(last->uuid, uuid, sizeof(last->uuid));
}

static void uuidcache_init(void)
{
	char line[100];
	char *s;
	int ma, mi, sz;
	static char ptname[100];
	FILE *procpt;
	char uuid[16], *label;
	char device[110];
	int firstPass;
	int handleOnFirst;

	if (uuidCache)
		return;

	procpt = fopen(PROC_PARTITIONS, "r");
	if (!procpt)
		return;

	for (firstPass = 1; firstPass >= 0; firstPass--) {
		fseek(procpt, 0, SEEK_SET);

		while (fgets(line, sizeof(line), procpt)) {
			if (sscanf(line, " %d %d %d %[^\n ]", &ma, &mi, &sz, ptname) != 4)
				continue;

			/* skip extended partitions (heuristic: size 1) */
			if (sz == 1)
				continue;

			/* look only at md devices on first pass */
			handleOnFirst = !strncmp(ptname, "md", 2);
			if (firstPass != handleOnFirst)
				continue;

			/* skip entire disk (minor 0, 64, ... on ide;
			   0, 16, ... on sd) */
			/* heuristic: partition name ends in a digit */

			for (s = ptname; *s; s++);
			if (isdigit(s[-1])) {
				/*
				 * Note: this is a heuristic only - there is no reason
				 * why these devices should live in /dev.
				 * Perhaps this directory should be specifiable by option.
				 * One might for example have /devlabel with links to /dev
				 * for the devices that may be accessed in this way.
				 * (This is useful, if the cdrom on /dev/hdc must not
				 * be accessed.)
				 */
				snprintf(device, sizeof(device), "%s/%s", DEVLABELDIR, ptname);
				if (!get_label_uuid(device, &label, uuid))
					uuidcache_addentry(sstrdup(device), label, uuid);
			}
		}
	}

	fclose(procpt);
}

#define UUID   1
#define VOL    2

static char *get_spec_by_x(int n, const char *t)
{
	struct uuidCache_s *uc;

	uuidcache_init();
	uc = uuidCache;

	while (uc) {
		switch (n) {
		  case UUID:
			  if (!memcmp(t, uc->uuid, sizeof(uc->uuid)))
				  return sstrdup(uc->device);
			  break;
		  case VOL:
			  if (!strcmp(t, uc->label))
				  return sstrdup(uc->device);
			  break;
		}
		uc = uc->next;
	}
	return NULL;
}

static uint8_t fromhex(char c)
{
	if (isdigit(c))
		return (c - '0');
	else if (islower(c))
		return (c - 'a' + 10);
	else
		return (c - 'A' + 10);
}

static char *get_spec_by_uuid(const char *s)
{
	uint8_t uuid[16];
	int i;

	if (strlen(s) != 36 || s[8] != '-' || s[13] != '-' || s[18] != '-' || s[23] != '-')
		goto bad_uuid;
	for (i = 0; i < 16; i++) {
		if (*s == '-')
			s++;
		if (!isxdigit(s[0]) || !isxdigit(s[1]))
			goto bad_uuid;
		uuid[i] = ((fromhex(s[0]) << 4) | fromhex(s[1]));
		s += 2;
	}
	return get_spec_by_x(UUID, (char *)uuid);

      bad_uuid:
	errstr(_("Found an invalid UUID: %s\n"), s);
	return NULL;
}

static char *get_spec_by_volume_label(const char *s)
{
	return get_spec_by_x(VOL, s);
}

const char *get_device_name(const char *item)
{
	const char *rc;

	if (!strncmp(item, "UUID=", 5))
		rc = get_spec_by_uuid(item + 5);
	else if (!strncmp(item, "LABEL=", 6))
		rc = get_spec_by_volume_label(item + 6);
	else
		rc = sstrdup(item);
	if (!rc)
		errstr(_("Error checking device name: %s\n"), item);
	return rc;
}
