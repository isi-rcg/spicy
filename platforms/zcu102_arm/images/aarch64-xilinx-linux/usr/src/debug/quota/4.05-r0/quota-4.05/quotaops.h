#ifndef GUARD_QUOTAOPS_H
#define GUARD_QUOTAOPS_H

#include "quotaio.h"

struct dquot *getprivs(qid_t id, struct quota_handle ** handles, int quiet);
int putprivs(struct dquot * qlist, int flags);
int editprivs(char *tmpfile);
int writeprivs(struct dquot * qlist, int outfd, char *name, int quotatype);
int readprivs(struct dquot * qlist, int infd);
int writeindividualtimes(struct dquot * qlist, int outfd, char *name, int quotatype);
int readindividualtimes(struct dquot * qlist, int infd);
int writetimes(struct quota_handle ** handles, int outfd);
int readtimes(struct quota_handle ** handles, int infd);
void freeprivs(struct dquot * qlist);
void update_grace_times(struct dquot *q);

#endif /* GUARD_QUOTAOPS_H */
