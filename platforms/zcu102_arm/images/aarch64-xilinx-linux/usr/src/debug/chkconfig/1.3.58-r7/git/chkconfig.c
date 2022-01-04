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
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <glob.h>
#include <libintl.h>
#include <locale.h>
#include <popt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static char *progname;

#define _(String) gettext((String))

#include "leveldb.h"

static int LSB = 0;

static void usage(void) {
    fprintf(stderr, _("%s version %s - Copyright (C) 1997-2000 Red Hat, Inc.\n"), progname, VERSION);
    fprintf(stderr, _("This may be freely redistributed under the terms of "
			"the GNU Public License.\n"));
    fprintf(stderr, "\n");
    fprintf(stderr, _("usage:   %s [--list] [--type <type>] [name]\n"), progname);
    fprintf(stderr, _("         %s --add <name>\n"), progname);
    fprintf(stderr, _("         %s --del <name>\n"), progname);
    fprintf(stderr, _("         %s --override <name>\n"), progname);
    fprintf(stderr, _("         %s [--level <levels>] [--type <type>] <name> %s\n"), progname, "<on|off|reset|resetpriorities>");

    exit(1);
}

static void display_list_systemd_note() {
	if (access(SYSTEMD_SERVICE_PATH, F_OK) >= 0 &&
	    systemdIsInit()) {
		fprintf(stderr, _("\nNote: This output shows SysV services only and does not include native\n"
				  "      systemd services. SysV configuration data might be overridden by native\n"
				  "      systemd configuration.\n\n"));
	}
}

static void readServiceError(int rc, char * name) {
    if (rc == 2) {
	fprintf(stderr, _("service %s supports chkconfig, but is not referenced in any runlevel (run 'chkconfig --add %s')\n"), name, name);
    } else if (rc == 1) {
	fprintf(stderr, _("service %s does not support chkconfig\n"), name);
    } else {
	fprintf(stderr, _("error reading information on service %s: %s\n"),
		name, strerror(errno));
    }

    exit(1);
}

static void checkRoot() {
	if (access(RUNLEVELS "/rc3.d", R_OK | W_OK) != 0) {
		fprintf(stderr, _("You do not have enough privileges to perform this operation.\n"));
		exit(1);
	}
}

static void reloadSystemd(void) {
    if (systemdActive())
        system("systemctl daemon-reload > /dev/null 2>&1");
}

static int delService(char *name, int type, int level) {
    int i, j, k, numservs, rc;
    glob_t globres;
    struct service s;
    struct service *services;

    if ((rc = readServiceInfo(name, type, &s, 0))) {
	readServiceError(rc, name);
	return 1;
    }
    if (s.type == TYPE_XINETD) return 0;

    checkRoot();

    if (LSB && level == -1) {
	numservs = readServices(&services);

	for (i = 0; i < numservs ; i++) {
		if (services[i].startDeps) {
			for (j = 0; services[i].startDeps[j].name ; j++) {
				if (!strcmp(services[i].startDeps[j].name, s.name)) {
				        if (services[i].currentLevels) {
				                return 1;
                                        }
				}
			}
		}
		if (services[i].stopDeps) {
			for (j = 0; services[i].stopDeps[j].name ; j++) {
				if (!strcmp(services[i].stopDeps[j].name, s.name)) {
				        for (k = 0 ; k <= 6; k++) {
				                if (isConfigured(services[i].name, k, NULL, NULL) && !(services[i].currentLevels & (1 << k)))
				                    return 1;
                                        }
				}
			}
		}
	}
    }

    for (j = 0 ; j < 7; j++) {
	 if (level == -1 || level == j) {
		 if (!findServiceEntries(name, j, &globres)) {
			 for (i = 0; i < globres.gl_pathc; i++)
				 unlink(globres.gl_pathv[i]);
			    if (globres.gl_pathc) globfree(&globres);
		    }
	 }
    }
    return 0;
}

static inline int laterThan(int i, int j) {
	if (i <= j) {
		i = j+1;
		if (i > 99)
			i = 99;
	}
	return i;
}

static inline int earlierThan(int i, int j) {
	if (i >= j) {
		i = j -1;
		if (i < 0)
			i = 0;
	}
	return i;
}

static int isSimilarlyConfigured(struct service s, struct service t) {
        int state_s, state_t;

        state_s = s.currentLevels;
        state_t = t.currentLevels;
        if ((state_s & state_t) == state_s)
                return 1;
        else
                return 0;
}

static void checkDeps(struct service *s, struct dep *deps, struct service *serv, int start) {
        int j,k;

        for (j = 0; deps[j].name ; j++) {
                if (!strcmp(deps[j].name, serv->name) && isSimilarlyConfigured(*s, *serv)) {
                        if (start)
                                s->sPriority = laterThan(s->sPriority, serv->sPriority);
                        else
                                s->kPriority = earlierThan(s->kPriority, serv->kPriority);
                        deps[j].handled = 1;
                }
                if (serv->provides) {
                        for (k = 0; serv->provides[k]; k++) {
                                if (!strcmp(deps[j].name, serv->provides[k]) && isSimilarlyConfigured(*s, *serv)) {
                                        if (start)
                                                s->sPriority = laterThan(s->sPriority, serv->sPriority);
                                        else
                                                s->kPriority = earlierThan(s->kPriority, serv->kPriority);
                                        deps[j].handled = 1;
                                }
                        }
                }
        }
}

static int frobOneDependencies(struct service *s, struct service *servs, int numservs, int target, int depfail) {
	int i;
	int s0 = s->sPriority;
	int k0 = s->kPriority;

	if (s->sPriority < 0) s->sPriority = 50;
	if (s->kPriority > 99) s->kPriority = 50;
	for (i = 0; i < numservs ; i++) {
		if (s->startDeps) {
			checkDeps(s, s->startDeps, &servs[i], 1);
		}
		if (s->softStartDeps) {
		        checkDeps(s, s->softStartDeps, &servs[i], 1);
		}
		if (s->stopDeps) {
		        checkDeps(s, s->stopDeps, &servs[i], 0);
		}
		if (s->softStopDeps) {
		        checkDeps(s, s->softStopDeps, &servs[i], 0);
		}
	}
	if (depfail) {
		if (s->startDeps) {
			for (i = 0; s->startDeps[i].name; i++) {
				if (!s->startDeps[i].handled && strcmp(s->startDeps[i].name,"$local_fs"))
					return -1;
			}
		}
		if (s->stopDeps) {
			for (i = 0; s->stopDeps[i].name; i++) {
				if (!s->stopDeps[i].handled && strcmp(s->stopDeps[i].name,"$local_fs"))
					return -1;
			}
		}
	}

	if (target || ((s0 != s->sPriority) || (k0 != s->kPriority))) {
		for (i = 0; i < 7; i++) {
			if (isConfigured(s->name, i, NULL, NULL)) {
				int on = isOn(s->name, i);
				delService(s->name, TYPE_INIT_D, i);
				doSetService(*s, i, on);
			} else if (target) {
				delService(s->name, TYPE_INIT_D, i);
				doSetService(*s, i, ((1<<i) & s->levels));
			}
		}
		return 1; /* Resolved something */
	}
	return 0; /* Didn't resolve anything */
}

/* LSB-style dependency frobber. Calculates a usable start priority
 * and stop priority.
 * This algorithm will almost certainly break horribly at some point. */
static int frobDependencies(struct service *s) {
	struct service *servs = NULL;
	int numservs = 0;
	int nResolved = 0;
	int i;

	numservs = readServices(&servs);
        /* In the full service list, replace the target script's current
           runlevels with the desired output runlevels, which are passed in */
	for (i = 0; i < numservs; i++) {
	        if (!strcmp((servs+i)->name, s->name)) {
	                (servs+i)->currentLevels = s->currentLevels;
	        }
	}
	/* Resolve recursively the other dependancies */
	do {
	  	nResolved = 0;

		for (i = 0; i < numservs ; i++) {
			if ((servs+i)->isLSB)
				nResolved += frobOneDependencies(servs+i, servs, numservs, 0, 0);
		}
	} while (nResolved);

	/* Resolve our target */
	if (frobOneDependencies(s, servs, numservs, 1, LSB) == -1)
		return 1;
	return 0;
}

static int addService(char * name, int type) {
    int i, rc;
    struct service s;

    if ((rc = readServiceInfo(name, type, &s, 0))) {
	readServiceError(rc, name);
	return 1;
    }

    if (s.type == TYPE_XINETD) return 0;
    checkRoot();

    if (s.isLSB) {
                for (i = 0; i < 7; i++) {
                        if (isConfigured(s.name, i, NULL, NULL))
                                break;
                }
                if (i == 7) {
                        s.currentLevels = s.levels;
                }
		rc = frobDependencies(&s);
    } else
    for (i = 0; i < 7; i++) {
	if (!isConfigured(name, i, NULL, NULL)) {
	    if ((1 << i) & s.levels)
		doSetService(s, i, 1);
	    else
		doSetService(s, i, 0);
	}
    }

    return rc;
}

static int overrideService(char * name, int srvtype) {
    /* Apply overrides if available; no available overrides is no error */
    int level, i, rc;
    glob_t globres;
    struct service s;
    struct service o;
    int priority;
    char type;
    int doChange = 1;
    int configured = 0;
    int thisLevelAdded, thisLevelOn;

    if ((rc = readServiceDifferences(name, srvtype, &s, &o, 0))) {
	return 0;
    }

    if (s.type == TYPE_XINETD) return 0;

    checkRoot();

    if ((s.levels == o.levels) &&
        (s.kPriority == o.kPriority) &&
        (s.sPriority == o.sPriority)) {
        /* no relevant changes in the override file */
	return 0;
    }

    if (s.isLSB && (s.sPriority <= -1) && (s.kPriority >= 100))
		frobDependencies(&s);
    if ((s.isLSB || o.isLSB) && (o.sPriority <= -1) && (o.kPriority >= 100))
		frobDependencies(&o);

    /* Apply overrides only if the service has not been changed since
     * being added, and not if the service has never been configured
     * at all.
     */

    for (level = 0; level < 7; level++) {
	thisLevelAdded = isConfigured(name, level, &priority, &type);
        thisLevelOn = s.levels & 1<<level;
        if (thisLevelAdded) {
            configured = 1;
            if (type == 'S') {
                if (priority != s.sPriority || !thisLevelOn) {
                    doChange = 0;
                    break;
                }
            } else if (type == 'K') {
                if (priority != s.kPriority || thisLevelOn) {
                    doChange = 0;
                    break;
                }
            }
	}
    }

    if (configured && doChange) {
        for (level = 0; level < 7; level++) {
            if (!findServiceEntries(name, level, &globres)) {
                for (i = 0; i < globres.gl_pathc; i++)
                    unlink(globres.gl_pathv[i]);
                if (globres.gl_pathc)
                    globfree(&globres);
                if ((1 << level) & o.levels)
                    doSetService(o, level, 1);
                else
                    doSetService(o, level, 0);
            }
        }
    }

    return 0;
}

static int showServiceInfo(struct service s, int forgiving) {
    int rc = 0;
    int i;

    if (s.type == TYPE_INIT_D) {
	    rc = 2;
	    for (i = 0 ; i < 7 ; i++) {
		    if (isConfigured(s.name, i, NULL, NULL)) {
			    rc = 0;
			    break;
		    }
	    }
    }

    if (rc) {
	if (!forgiving)
	    readServiceError(rc, s.name);
	return forgiving ? 0 : 1;
    }

    printf("%-15s", s.name);
    if (s.type == TYPE_XINETD) {
	    printf("\t%s\n", s.levels ? _("on") : _("off"));
	    return 0;
    }

    for (i = 0; i < 7; i++) {
	printf("\t%d:%s", i, isOn(s.name, i) ? _("on") : _("off"));
    }
    printf("\n");

    return 0;
}

static int showServiceInfoByName(char * name, int type, int forgiving) {
    int rc;
    struct service s;

    if (systemdActive() && isOverriddenBySystemd(name)) {
        return forgiving ? 0 : 1;
    }

    rc = readServiceInfo(name, type, &s, 0);

    if (rc) {
	if (!forgiving)
	    readServiceError(rc, name);
	return forgiving ? 0 : 1;
    }

    return showServiceInfo(s, forgiving);
}


static int isXinetdEnabled() {
	struct service s;

	if (readServiceInfo("xinetd", TYPE_INIT_D, &s, 0)) {
		return 0;
	}
	if (s.currentLevels)
		return 1;
	return 0;
}

static int serviceNameCmp(const void * a, const void * b) {
  return strcmp(* (char **)a, * (char **)b);
}

static int xinetdNameCmp(const void * a, const void * b) {
    const struct service * first = a;
    const struct service * second = b;

    return strcmp(first->name, second->name);
}


static int listService(char * item, int type) {
    DIR * dir;
    struct dirent * ent;
    struct service *services;
    int i;
    int numServices = 0;
    int numServicesAlloced;
    int err = 0;
    int systemd = systemdActive();

    if (item) return showServiceInfoByName(item, type, 0);

    if (type & TYPE_INIT_D) {
        numServices = readServices(&services);

        qsort(services, numServices, sizeof(*services), serviceNameCmp);

        for (i = 0; i < numServices ; i++) {
            if (systemd && isOverriddenBySystemd(services[i].name))
                continue;
	    if (showServiceInfo(services[i], 1)) {
		    return 1;
	    }
        }
    }

    if (isXinetdEnabled() && type & TYPE_XINETD) {
	    struct service *s, *t;

	    printf("\n");
	    printf(_("xinetd based services:\n"));
	    if (!(dir = opendir(XINETDDIR))) {
		    fprintf(stderr, _("failed to open directory %s: %s\n"),
			    XINETDDIR, strerror(err));
		    return 1;
	    }
	    numServices = 0;
	    numServicesAlloced = 10;
	    s = malloc(sizeof (*s) * numServicesAlloced);

	    while ((ent = readdir(dir))) {
		    const char *dn;

		    /* Skip any file starting with a . */
		    if (ent->d_name[0] == '.')	continue;

		    /* Skip files with known bad extensions */
		    if ((dn = strrchr(ent->d_name, '.')) != NULL &&
			(!strcmp(dn, ".rpmsave") || !strcmp(dn, ".rpmnew") || !strcmp(dn, ".rpmorig") || !strcmp(dn, ".swp")))
		      continue;

		    dn = ent->d_name + strlen(ent->d_name) - 1;
		    if (*dn == '~' || *dn == ',')
		      continue;

		    if (numServices == numServicesAlloced) {
			    numServicesAlloced += 10;
			    s = realloc(s, numServicesAlloced * sizeof (*s));
		    }
		    if (readXinetdServiceInfo(ent->d_name, s + numServices) != -1)
			    numServices ++;
	    }

	    qsort(s, numServices, sizeof(*s), xinetdNameCmp);
	    t = s;
	    for (i = 0; i < numServices; i++, s++) {
		    char *tmp = malloc(strlen(s->name) + 5);
		    sprintf(tmp,"%s:",s->name);
		    printf("\t%-15s\t%s\n", tmp,  s->levels ? _("on") : _("off"));
	    }
	    closedir(dir);
	    free(t);
    }
    return 0;
}

int setService(char * name, int type, int where, int state) {
    int i, rc;
    int what;
    struct service s;

    if (!where && state != -1) {
	/* levels 2, 3, 4, 5 */
	where = (1 << 2) | (1 << 3) | (1 << 4) | (1 << 5);
    } else if (!where) {
	where = (1 << 0) | (1 << 1) | (1 << 2) |
	        (1 << 3) | (1 << 4) | (1 << 5) | (1 << 6);
    }

    if ((rc = readServiceInfo(name, type, &s, 0))) {
	readServiceError(rc, name);
	return 1;
    }

    checkRoot();

    if (s.type == TYPE_INIT_D) {
	    int rc = 0;

	    if (state == -1)
                s.currentLevels = s.levels;
            else if (state != -2) {
                /* If we're enabling/disabling, set currentLevels to
                   desired state */
                for (i = 0; i < 7; i++) {
                    if (!((1 << i) & where)) continue;

		    if (state == 1)
		        s.currentLevels |= (1 << i);
                    else if (state == 0)
                        s.currentLevels |= ~(1 << i);
                }
	    }
	    if (s.isLSB)
		    frobDependencies(&s);
	    for (i = 0; i < 7; i++) {

		    if (!((1 << i) & where)) continue;

		    if (state == 1 || state == 0)
		      what = state;
		    else if (state == -2)
		      what = isOn(name, i);
		    else if (s.levels & (1 << i))
		      what = 1;
		    else
		      what = 0;
		    rc |= doSetService(s, i, what);
	    }

            reloadSystemd();

            return rc;
    } else if (s.type == TYPE_XINETD) {
	    if (setXinetdService(s, state)) {
		    return 1;
	    }
	    system("/etc/init.d/xinetd reload >/dev/null 2>&1");
    }

    return 0;
}

void forwardSystemd(const char *name, int type, const char *verb) {

    if (type == TYPE_XINETD)
        return;

    if (!systemdIsInit())
	return;

    if (isOverriddenBySystemd(name)) {
        char *p;

        asprintf(&p, "%s.service", name);

        fprintf(stderr, _("Note: Forwarding request to 'systemctl %s %s'.\n"),
                verb, p);

        execlp("systemctl", "systemctl", verb, p, NULL);
        free(p);
        fprintf(stderr, _("Failed to forward service request to systemctl: %m\n"));
        exit(1);
    }
}

int main(int argc, const char ** argv) {
    int listItem = 0, addItem = 0, delItem = 0, overrideItem = 0, noRedirectItem = 0;
    int type = TYPE_ANY;
    int rc, i, x;
    char * levels = NULL;
    char * typeString = NULL;
    int help=0, version=0;
    struct service s;
    poptContext optCon;
    struct poptOption optionsTable[] = {
	    { "add", '\0', 0, &addItem, 0 },
	    { "del", '\0', 0, &delItem, 0 },
	    { "override", '\0', 0, &overrideItem, 0 },
	    { "no-redirect", '\0', 0, &noRedirectItem, 0},
	    { "list", '\0', 0, &listItem, 0 },
	    { "level", '\0', POPT_ARG_STRING, &levels, 0 },
	    { "levels", '\0', POPT_ARG_STRING, &levels, 0 },
	    { "type", '\0', POPT_ARG_STRING, &typeString, 0 },
	    { "help", 'h', POPT_ARG_NONE, &help, 0 },
	    { "version", 'v', POPT_ARG_NONE, &version, 0 },
	    { 0, 0, 0, 0, 0 }
    };

    if ((progname = strrchr(argv[0], '/')) != NULL)
	progname++;
    else
	progname = (char *)argv[0];
    if (!strcmp(progname,"install_initd")) {
	    addItem++;
	    LSB++;
    }
    if (!strcmp(progname,"remove_initd")) {
	    delItem++;
	    LSB++;
    }

    setlocale(LC_ALL, "");
    bindtextdomain("chkconfig","/usr/share/locale");
    textdomain("chkconfig");

    optCon = poptGetContext("chkconfig", argc, argv, optionsTable, 0);
    poptReadDefaultConfig(optCon, 1);

    if ((rc = poptGetNextOpt(optCon)) < -1) {
	fprintf(stderr, "%s: %s\n",
		poptBadOption(optCon, POPT_BADOPTION_NOALIAS),
		poptStrerror(rc));
	exit(1);
    }

    if (version) {
	fprintf(stdout, _("%s version %s\n"), progname, VERSION);
	exit(0);
    }

    if (help) usage();

    if (typeString) {
	if (!strcmp(typeString, "xinetd"))
	    type = TYPE_XINETD;
	else if (!strcmp(typeString, "sysv"))
	    type = TYPE_INIT_D;
	else {
	    fprintf(stderr, _("--type must be 'sysv' or 'xinetd'\n"));
	    exit(1);
	}
    }

    if (argc == 1) {
	    display_list_systemd_note();
	    return listService(NULL, type);
    }

    if ((listItem + addItem + delItem + overrideItem) > 1) {
	fprintf(stderr, _("only one of --list, --add, --del, or --override"
                " may be specified\n"));
	exit(1);
    }

    if (getenv("SYSTEMCTL_SKIP_REDIRECT") != NULL) {
        noRedirectItem = 1;
    }

    if (addItem) {
	char * name = (char *)poptGetArg(optCon);
        int r;

	if (!name || !*name || poptGetArg(optCon))
	    usage();

	name = basename(name);
	r = addService(name, type);
        reloadSystemd();

        return r;
    } else if (delItem) {
	char * name = (char *)poptGetArg(optCon);
        int r;

	if (!name || !*name || poptGetArg(optCon)) usage();

	name = basename(name);
	r = delService(name, type, -1);
        reloadSystemd();

        return r;
    } else if (overrideItem) {
	char * name = (char *)poptGetArg(optCon);
        int r;

	if (!name || !*name || poptGetArg(optCon)) usage();

        name = basename(name);
	r = overrideService(name, type);
        reloadSystemd();

        return r;
    } else if (listItem) {
	char * item = (char *)poptGetArg(optCon);

	if (item && poptGetArg(optCon)) usage();

	display_list_systemd_note();

	return listService(item, type);
    } else {
	char * name = (char *)poptGetArg(optCon);
	char * state = (char *)poptGetArg(optCon);
	int where = 0, level = -1;

	if (!name) {
		usage();
	}
	if (levels) {
	    where = parseLevels(levels, 0);
	    if (where == -1) usage();
	}

	if (!state) {
	    if (!noRedirectItem && !levels) {
		forwardSystemd(name, type, "is-enabled");
	    }

	    if (where) {
		rc = x = 0;
		i = where;
		while (i) {
		    if (i & 1) {
			rc++;
			level = x;
		    }
		    i >>= 1;
		    x++;
		}

		if (rc > 1) {
		    fprintf(stderr, _("only one runlevel may be specified for "
			    "a chkconfig query\n"));
		    exit(1);
		}
	    }
	    rc = readServiceInfo(name, type, &s, 0);
	    if (rc)
	       return 1;
	    if (s.type == TYPE_XINETD) {
	       if (isOn("xinetd",level))
		       return !s.levels;
	       else
		       return 1;
	    } else {
               if (level == -1)
                   level = currentRunlevel();
	       return s.currentLevels & (1 << level) ? 0 : 1;
	    }
	} else if (!strcmp(state, "on")) {
	    if (!noRedirectItem) {
		forwardSystemd(name, type, "enable");
	    }
	    return setService(name, type, where, 1);
        } else if (!strcmp(state, "off")) {
	    if (!noRedirectItem) {
		forwardSystemd(name, type, "disable");
	    }
	    return setService(name, type, where, 0);
        } else if (!strcmp(state, "reset"))
	    return setService(name, type, where, -1);
	else if (!strcmp(state, "resetpriorities"))
	    return setService(name, type, where, -2);
    }

    usage();

    return 1;
}
