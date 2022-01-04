/* Copyright 1997-2008 Red Hat, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License, version 2,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA
 */
#ifndef H_LEVELDB
#define H_LEVELDB

#define RUNLEVELS "/etc"
#define XINETDDIR "/etc/xinetd.d"

#include <glob.h>

#define TYPE_INIT_D	0x1
#define TYPE_XINETD	0x2
#define TYPE_ANY	(TYPE_INIT_D | TYPE_XINETD)

#ifndef SYSTEMD_SERVICE_PATH
#define SYSTEMD_SERVICE_PATH "/lib/systemd/system"
#endif

struct dep {
    char *name;
    int handled;
};

struct service {
    char * name;
    int levels, kPriority, sPriority;
    int currentLevels;
    char * desc;
    struct dep *startDeps;
    struct dep *stopDeps;
    struct dep *softStartDeps;
    struct dep *softStopDeps;
    char **provides;
    int type;
    int isLSB;
    int enabled;
};

int parseLevels(char * str, int emptyOk);

/* returns 0 on success, 1 if the service is not chkconfig-able, -1 if an
   I/O error occurs (in which case errno can be checked) */
int readServiceInfo(char * name, int type, struct service * service, int honorHide);
int readServices(struct service **services);
int readServiceDifferences(char * name, int type, struct service * service, struct service * service_overrides, int honorHide);
int parseServiceInfo(int fd, char * name, struct service * service, int honorHide, int partialOk);
int currentRunlevel(void);
int isOn(char * name, int where);
int isConfigured(char * name, int level, int *priority, char *type);
int whatLevels(char * name);
int doSetService(struct service s, int level, int on);
int findServiceEntries(char * name, int level, glob_t * globresptr);
int readXinetdServiceInfo(char *name, struct service *service);
int setXinetdService(struct service s, int on);
int systemdIsInit();
int systemdActive();
int isOverriddenBySystemd(const char *service);
#endif
