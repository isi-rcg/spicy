/*
 *	Common types, macros, and routines for enabling/disabling
 *	quota for each of the various Linux quota formats.
 */

#include "pot.h"
#include "quota.h"
#include "quotasys.h"
#include "bylabel.h"
#include "common.h"
#include "quotaio.h"

#define STATEFLAG_ON		0x01
#define STATEFLAG_OFF		0x02
#define STATEFLAG_ALL		0x04

typedef int (newstate_t) (struct mount_entry * mnt, int type, char *file, int flags);
extern int xfs_newstate(struct mount_entry *mnt, int type, char *file, int flags);
extern int pinfo(char *fmt, ...);
