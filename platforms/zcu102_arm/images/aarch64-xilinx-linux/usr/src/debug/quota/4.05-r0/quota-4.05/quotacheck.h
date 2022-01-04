/*
 *
 *	Header file for quota checking utilities
 *
 */

#ifndef GUARD_QUOTACHECK_H
#define GUARD_QUOTACHECK_H

#include <sys/types.h>

#include "quota.h"
#include "quotaio.h"

#define NODQUOT ((struct dquot *)NULL)

#define FL_FORCE 1		/* Force check even if quota enabled */
#define FL_VERBOSE 2		/* Have verbose input */
#define FL_DEBUG 4		/* Have very verbose input */
#define FL_INTERACTIVE 8	/* Ask questions when needed */
#define FL_GUESSDQ 16		/* When more structures for same user found, use the first */
#define FL_NEWFILE 32		/* Don't try to read old file. Just create new one. */
#define FL_FORCEREMOUNT 64	/* Force check even when remounting RO fails */
#define FL_NOREMOUNT 128	/* Don't try to remount filesystem RO */
#define FL_ALL 256		/* Scan all mountpoints with quota? */
#define FL_NOROOT 512		/* Scan all mountpoints except root */
#define FL_BACKUPS 1024		/* Create backup of old quota file? */
#define FL_VERYVERBOSE 2048	/* Print directory names when checking */

extern int flags;		/* Options from command line */
extern struct util_dqinfo old_info[MAXQUOTAS];	/* Loaded info from file */

#ifdef DEBUG_MALLOC
extern size_t malloc_mem = 0;
extern size_t free_mem = 0;
#endif

void *xmalloc(size_t size);
void debug(int df, char *fmtstr, ...) __attribute__ ((__format__ (__printf__, 2, 3)));
int ask_yn(char *q, int def);
struct dquot *lookup_dquot(qid_t id, int type);
struct dquot *add_dquot(qid_t id, int type);
int v2_detect_version(char *filename, int fd, int type);
int v2_buffer_file(char *filename, int fd, int type, int version);
int v1_buffer_file(char *filename, int fd, int type);
void v2_merge_info(struct util_dqinfo *new, struct util_dqinfo *old);
#endif
