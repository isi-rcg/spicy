/*
 *
 *	Header file for rquota functions
 *
 */

#ifndef GUARD_RQUOTA_CLIENT_H
#define GUARD_RQUOTA_CLIENT_H

#include "quotaio.h"

/* Collect the requested quota information from a remote host. */
int rpc_rquota_get(struct dquot *dquot);

/* Set the requested quota information on a remote host. */
int rpc_rquota_set(int qcmd, struct dquot *dquot);

#endif
