/*
 * initreq.h	Interface to talk to init through /dev/initctl.
 *
 *		Copyright (C) 1995-2004 Miquel van Smoorenburg
 *
 *		This program is free software; you can redistribute it and/or modify
 *		it under the terms of the GNU General Public License as published by
 *		the Free Software Foundation; either version 2 of the License, or
 *		(at your option) any later version.
 *
 *		This program is distributed in the hope that it will be useful,
 *		but WITHOUT ANY WARRANTY; without even the implied warranty of
 *		MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *		GNU General Public License for more details.
 *
 *		You should have received a copy of the GNU General Public License
 *		along with this program; if not, write to the Free Software
 *		Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * Version:     @(#)initreq.h  1.28  31-Mar-2004 MvS
 *
 */
#ifndef _INITREQ_H
#define _INITREQ_H

#include <sys/param.h>

#if defined(__FreeBSD_kernel__)
#  define INIT_FIFO  "/etc/.initctl"
#else
#  define INIT_FIFO  "/dev/initctl"
#endif

#define INIT_MAGIC 0x03091969
#define INIT_CMD_START		0
#define INIT_CMD_RUNLVL		1
#define INIT_CMD_POWERFAIL	2
#define INIT_CMD_POWERFAILNOW	3
#define INIT_CMD_POWEROK	4
#define INIT_CMD_BSD		5
#define INIT_CMD_SETENV		6
#define INIT_CMD_UNSETENV	7

#ifdef MAXHOSTNAMELEN
#  define INITRQ_HLEN	MAXHOSTNAMELEN
#else
#  define INITRQ_HLEN	64
#endif

/*
 *	This is what BSD 4.4 uses when talking to init.
 *	Linux doesn't use this right now.
 */
struct init_request_bsd {
	char	gen_id[8];		/* Beats me.. telnetd uses "fe" */
	char	tty_id[16];		/* Tty name minus /dev/tty      */
	char	host[INITRQ_HLEN];	/* Hostname                     */
	char	term_type[16];		/* Terminal type                */
	int	signal;			/* Signal to send               */
	int	pid;			/* Process to send to           */
	char	exec_name[128];	        /* Program to execute           */
	char	reserved[128];		/* For future expansion.        */
};


/*
 *	Because of legacy interfaces, "runlevel" and "sleeptime"
 *	aren't in a seperate struct in the union.
 *
 *	The weird sizes are because init expects the whole
 *	struct to be 384 bytes.
 */
struct init_request {
	int	magic;			/* Magic number                 */
	int	cmd;			/* What kind of request         */
	int	runlevel;		/* Runlevel to change to        */
	int	sleeptime;		/* Time between TERM and KILL   */
	union {
		struct init_request_bsd	bsd;
		char			data[368];
	} i;
};

#endif
