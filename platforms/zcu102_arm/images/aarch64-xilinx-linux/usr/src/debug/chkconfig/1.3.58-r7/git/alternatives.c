/* Copyright 1997-2009 Red Hat, Inc.
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
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <libintl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define	FLAGS_TEST	(1 << 0)
#define	FLAGS_VERBOSE	(1 << 1)

#define FL_TEST(flags)	    ((flags) & FLAGS_TEST)
#define FL_VERBOSE(flags)   ((flags) & FLAGS_VERBOSE)

#define _(foo) gettext(foo)

struct linkSet {
    char * title;			/* print */
    char * facility;			/* /usr/bin/lpr */
    char * target;			/* /usr/bin/lpr.LPRng */
};

struct alternative {
    int priority;
    struct linkSet master;
    struct linkSet * slaves;
    char *initscript;
    int numSlaves;
};

struct alternativeSet {
    enum alternativeMode { AUTO, MANUAL } mode;
    struct alternative * alts;
    int numAlts;
    int best, current;
    char * currentLink;
};

enum programModes { MODE_UNKNOWN, MODE_INSTALL, MODE_REMOVE, MODE_AUTO,
		    MODE_DISPLAY, MODE_CONFIG, MODE_SET,
		    MODE_SLAVE, MODE_VERSION, MODE_USAGE };

static int usage(int rc) {
    printf(_("alternatives version %s - Copyright (C) 2001 Red Hat, Inc.\n"), VERSION);
    printf(_("This may be freely redistributed under the terms of the GNU Public License.\n\n"));
    printf(_("usage: alternatives --install <link> <name> <path> <priority>\n"));
    printf(_("                    [--initscript <service>]\n"));
    printf(_("                    [--slave <link> <name> <path>]*\n"));
    printf(_("       alternatives --remove <name> <path>\n"));
    printf(_("       alternatives --auto <name>\n"));
    printf(_("       alternatives --config <name>\n"));
    printf(_("       alternatives --display <name>\n"));
    printf(_("       alternatives --set <name> <path>\n"));
    printf(_("\n"));
    printf(_("common options: --verbose --test --help --usage --version\n"));
    printf(_("                --altdir <directory> --admindir <directory>\n"));

    exit(rc);
}

static int isSystemd(char *initscript) {
    char tmppath[500];
    struct stat sbuf;

    snprintf(tmppath, 500, "/lib/systemd/system/%s.service", initscript);
    if (!stat(tmppath, &sbuf))
        return 1;
    return 0;
}

static void setupSingleArg(enum programModes * mode, const char *** nextArgPtr,
			   enum programModes newMode, char ** title) {
    const char ** nextArg = *nextArgPtr;

    if (*mode != MODE_UNKNOWN) usage(2);
    *mode = newMode;
    nextArg++;

    if (!*nextArg || **nextArg == '/') usage(2);
    *title = strdup(*nextArg);
    *nextArgPtr = nextArg + 1;
}

static void setupDoubleArg(enum programModes * mode, const char *** nextArgPtr,
			   enum programModes newMode, char ** title,
			   char ** target) {
    const char ** nextArg = *nextArgPtr;

    if (*mode != MODE_UNKNOWN) usage(2);
    *mode = newMode;
    nextArg++;

    if (!*nextArg || **nextArg == '/') usage(2);
    *title = strdup(*nextArg);
    nextArg++;

    if (!*nextArg || **nextArg != '/') usage(2);
    *target = strdup(*nextArg);
    *nextArgPtr = nextArg + 1;
}

static void setupLinkSet(struct linkSet * set, const char *** nextArgPtr) {
    const char ** nextArg = *nextArgPtr;

    if (!*nextArg || **nextArg != '/') usage(2);
    set->facility = strdup(*nextArg);
    nextArg++;

    if (!*nextArg || **nextArg == '/') usage(2);
    set->title = strdup(*nextArg);
    nextArg++;

    if (!*nextArg || **nextArg != '/') usage(2);
    set->target = strdup(*nextArg);
    *nextArgPtr = nextArg + 1;
}

char * parseLine(char ** buf) {
    char * start = *buf;
    char * end;

    if (!*buf || !**buf) return NULL;

    end = strchr(start, '\n');
    if (!end) {
	*buf = start + strlen(start);
    } else {
	*buf = end + 1;
	*end = '\0';
    }

    while (isspace(*start) && *start) start++;

    return strdup(start);
}

static int readConfig(struct alternativeSet * set, const char * title,
		      const char * altDir, const char * stateDir, int flags) {
    char * path;
    int fd;
    int i;
    struct stat sb;
    char * buf;
    char * end;
    char * line;
    struct {
	char * facility;
	char * title;
    } * groups = NULL;
    int numGroups = 0;
    char linkBuf[1024];

    set->alts = NULL;
    set->numAlts = 0;
    set->mode = AUTO;
    set->best = 0;
    set->current = -1;

    path = alloca(strlen(stateDir) + strlen(title) + 2);
    sprintf(path, "%s/%s", stateDir, title);

    if (FL_VERBOSE(flags))
	printf(_("reading %s\n"), path);

    if ((fd = open(path, O_RDONLY)) < 0) {
	if (errno == ENOENT) return 3;
	fprintf(stderr, _("failed to open %s: %s\n"), path,
	        strerror(errno));
	return 1;
    }

    fstat(fd, &sb);
    buf = alloca(sb.st_size + 1);
    if (read(fd, buf, sb.st_size) != sb.st_size) {
	close(fd);
	fprintf(stderr, _("failed to read %s: %s\n"), path,
	        strerror(errno));
	return 1;
    }
    close(fd);
    buf[sb.st_size] = '\0';

    line = parseLine(&buf);
    if (!line) {
	fprintf(stderr, _("%s empty!\n"), path);
	return 1;
    }

    if (!strcmp(line, "auto")) {
	set->mode = AUTO;
    } else if (!strcmp(line, "manual")) {
	set->mode = MANUAL;
    } else {
	fprintf(stderr, _("bad mode on line 1 of %s\n"), path);
	return 1;
    }
    free(line);

    line = parseLine(&buf);
    if (!line || *line != '/') {
	fprintf(stderr, _("bad primary link in %s\n"), path);
	return 1;
    }

    groups = realloc(groups, sizeof(*groups));
    groups[0].title = strdup(title);
    groups[0].facility = line;
    numGroups = 1;

    line = parseLine(&buf);
    while (line && *line) {
	if (*line == '/') {
	    fprintf(stderr, _("path %s unexpected in %s\n"), line, path);
	    return 1;
	}

	groups = realloc(groups, sizeof(*groups) * (numGroups + 1));
	groups[numGroups].title = line;

	line = parseLine(&buf);
	if (!line || !*line) {
	    fprintf(stderr, _("missing path for slave %s in %s\n"), line, path);
	    return 1;
	}

	groups[numGroups++].facility = line;

	line = parseLine(&buf);
    }

    if (!line) {
	fprintf(stderr, _("unexpected end of file in %s\n"), path);
	return 1;
    }

    line = parseLine(&buf);
    while (line && *line) {
	set->alts = realloc(set->alts, (set->numAlts + 1) * sizeof(*set->alts));

	if (*line != '/') {
	    fprintf(stderr, _("path to alternate expected in %s\n"), path);
	    fprintf(stderr, _("unexpected line in %s: %s\n"), path, line);
	    return 1;
	}

	set->alts[set->numAlts].master.facility = strdup(groups[0].facility);
	set->alts[set->numAlts].master.title = strdup(groups[0].title);
	set->alts[set->numAlts].master.target = line;
	set->alts[set->numAlts].numSlaves = numGroups - 1;
	if (numGroups > 1)
	    set->alts[set->numAlts].slaves =
		malloc((numGroups - 1) *
			sizeof(*set->alts[set->numAlts].slaves));
	else
	    set->alts[set->numAlts].slaves = NULL;

	line = parseLine(&buf);
	set->alts[set->numAlts].priority = -1;
	set->alts[set->numAlts].priority = strtol(line, &end, 0);
	set->alts[set->numAlts].initscript = NULL;
	if (!end || (end == line)) {
	    fprintf(stderr, _("numeric priority expected in %s\n"), path);
	    fprintf(stderr, _("unexpected line in %s: %s\n"), path, line);
	    return 1;
	}
	if (end) {
		while (*end && isspace(*end)) end++;
		if (strlen(end)) {
                        set->alts[set->numAlts].initscript = strdup(end);
		}
	}

	if (set->alts[set->numAlts].priority > set->alts[set->best].priority)
	    set->best = set->numAlts;

	for (i = 1; i < numGroups; i++) {
	    line = parseLine(&buf);
	    if (line && strlen(line) && *line != '/') {
		fprintf(stderr, _("slave path expected in %s\n"), path);
		fprintf(stderr, _("unexpected line in %s: %s\n"), path, line);
		return 1;
	    }

	    set->alts[set->numAlts].slaves[i - 1].title =
			strdup(groups[i].title);
	    set->alts[set->numAlts].slaves[i - 1].facility =
			strdup(groups[i].facility);
		set->alts[set->numAlts].slaves[i - 1].target = (line && strlen(line)) ? line : NULL;
	}

	set->numAlts++;

	line = parseLine(&buf);
    }

    while (line) {
	line = parseLine(&buf);
	if (line && *line) {
	    fprintf(stderr, _("unexpected line in %s: %s\n"), path, line);
	    return 1;
	}
    }

    sprintf(path, "%s/%s", altDir, set->alts[0].master.title);

    if (((i = readlink(path, linkBuf, sizeof(linkBuf) - 1)) < 0)) {
	fprintf(stderr, _("failed to read link %s: %s\n"),
		set->alts[0].master.facility, strerror(errno));
	return 2;
    }

    linkBuf[i] = '\0';

    for (i = 0; i < set->numAlts; i++)
	if (!strcmp(linkBuf, set->alts[i].master.target)) break;

    if (i == set->numAlts) {
	set->mode = MANUAL;
	set->current = -1;
	if (FL_VERBOSE(flags))
	    printf(_("link points to no alternative -- setting mode to manual\n"));
    } else {
	if (i != set->best && set->mode == AUTO) {
	    set->mode = MANUAL;
	    if (FL_VERBOSE(flags))
		printf(_("link changed -- setting mode to manual\n"));
	}
	set->current = i;
    }

    set->currentLink = strdup(linkBuf);

    return 0;
}

static int isLink(char *path)  {
	struct stat sbuf;
	int rc = 0;

	rc = lstat(path, &sbuf);
	if (!rc) {
		rc = S_ISLNK(sbuf.st_mode);
	}
	return rc;
}


static int removeLinks(struct linkSet * l, const char * altDir, int flags) {
    char * sl;

    sl = alloca(strlen(altDir) + strlen(l->title) + 2);
    sprintf(sl, "%s/%s", altDir, l->title);
    if (FL_TEST(flags)) {
	printf(_("would remove %s\n"), sl);
    } else if (isLink(sl) && unlink(sl) && errno != ENOENT) {
	fprintf(stderr, _("failed to remove link %s: %s\n"),
		sl, strerror(errno));
	return 1;
    }
    if (FL_TEST(flags)) {
	printf(_("would remove %s\n"), l->facility);
    } else if (isLink(l->facility) && unlink(l->facility) && errno != ENOENT) {
	fprintf(stderr, _("failed to remove link %s: %s\n"),
		l->facility, strerror(errno));
	return 1;
    }

    return 0;
}

static int makeLinks(struct linkSet * l, const char * altDir, int flags) {
    char * sl;
    struct stat sb;

    sl = alloca(strlen(altDir) + strlen(l->title) + 2);
    sprintf(sl, "%s/%s", altDir, l->title);
    if (isLink(l->facility)) {
	    if (FL_TEST(flags)) {
		    printf(_("would link %s -> %s\n"), l->facility, sl);
	    } else {
	            unlink(l->facility);

		    if (symlink(sl, l->facility)) {
			    fprintf(stderr, _("failed to link %s -> %s: %s\n"),
				    l->facility, sl, strerror(errno));
			    return 1;
		    }
	    }
    }

    if (FL_TEST(flags)) {
	printf(_("would link %s -> %s\n"), sl, l->target);
    } else {
	if (unlink(sl) && errno != ENOENT){
	    fprintf(stderr, _("failed to remove link %s: %s\n"),
		    sl, strerror(errno));
	    return 1;
	}

	if (symlink(l->target, sl)) {
	    fprintf(stderr, _("failed to link %s -> %s: %s\n"),
		    sl, l->target, strerror(errno));
	    return 1;
	}
    }

    return 0;
}

static int writeState(struct alternativeSet *  set, const char * altDir,
		      const char * stateDir, int forceLinks, int flags) {
    char * path;
    char * path2;
    int fd;
    FILE * f;
    int i, j;
    int rc = 0;
    struct alternative * alt;

    path = alloca(strlen(stateDir) + strlen(set->alts[0].master.title) + 6);
    sprintf(path, "%s/%s.new", stateDir, set->alts[0].master.title);

    path2 = alloca(strlen(stateDir) + strlen(set->alts[0].master.title) + 2);
    sprintf(path2, "%s/%s", stateDir, set->alts[0].master.title);

    if (FL_TEST(flags))
	fd = dup(1);
    else
	fd = open(path, O_RDWR | O_CREAT | O_EXCL, 0644);

    if (fd < 0) {
	if (errno == EEXIST)
	    fprintf(stderr, _("%s already exists\n"), path);
	else
	    fprintf(stderr, _("failed to create %s: %s\n"), path,
		    strerror(errno));
	return 1;
    }

    f = fdopen(fd, "w");
    fprintf(f, "%s\n", set->mode == AUTO ? "auto" : "manual");
    fprintf(f, "%s\n", set->alts[0].master.facility);
    for (i = 0; i < set->alts[0].numSlaves; i++) {
	fprintf(f, "%s\n", set->alts[0].slaves[i].title);
	fprintf(f, "%s\n", set->alts[0].slaves[i].facility);
    }
    fprintf(f, "\n");

    for (i = 0; i < set->numAlts; i++) {
	fprintf(f, "%s\n", set->alts[i].master.target);
	if (set->alts[i].initscript)
		    fprintf(f, "%d %s\n", set->alts[i].priority, set->alts[i].initscript);
	else
		    fprintf(f, "%d\n", set->alts[i].priority);

	for (j = 0; j < set->alts[i].numSlaves; j++) {
		if (set->alts[i].slaves[j].target)
			fprintf(f, "%s", set->alts[i].slaves[j].target);
		fprintf(f,"\n");
	}
    }

    fclose(f);

    if (!FL_TEST(flags) && rename(path, path2)) {
	fprintf(stderr, _("failed to replace %s with %s: %s\n"),
		path2, path, strerror(errno));
	unlink(path);
	return 1;
    }

    if (set->mode == AUTO)
		set->current = set->best;

    alt = set->alts + ( set->current > 0 ? set->current : 0);

    if (forceLinks || set->mode == AUTO) {
	rc |= makeLinks(&alt->master, altDir, flags);
	for (i = 0; i < alt->numSlaves; i++) {
	        if (alt->slaves[i].target)
			rc |= makeLinks(alt->slaves + i, altDir, flags);
		else
			rc |= removeLinks(alt->slaves + i, altDir, flags);
	}
    }

    if (!FL_TEST(flags)) {
	    if (alt->initscript) {
	            if (isSystemd(alt->initscript)) {
                            asprintf(&path, "/bin/systemctl -q enable %s.service", alt->initscript);
                            if (FL_VERBOSE(flags))
                                    printf(_("running %s\n"), path);
                            system(path);
                            free(path);
                    } else {
                            asprintf(&path, "/sbin/chkconfig --add %s", alt->initscript);
                            if (FL_VERBOSE(flags))
                                    printf(_("running %s\n"), path);
                            system(path);
                            free(path);
                    }
	    }
	    for (i = 0; i < set->numAlts ; i++) {
		    struct alternative * tmpalt = set->alts + i;
		    if (tmpalt != alt && tmpalt->initscript) {
		            if (isSystemd(tmpalt->initscript)) {
                                    asprintf(&path, "/bin/systemctl -q disable %s.service", tmpalt->initscript);
                                    if (FL_VERBOSE(flags))
				            printf(_("running %s\n"), path);
                                    system(path);
                                    free(path);
                            } else {
                                    asprintf(&path, "/sbin/chkconfig --del %s", tmpalt->initscript);
                                    if (FL_VERBOSE(flags))
				            printf(_("running %s\n"), path);
                                    system(path);
                                    free(path);
                            }
		    }
	    }
    }

    return rc;
}

static int linkCmp(const void *a, const void *b) {
	struct linkSet *one = (struct linkSet *)a, *two = (struct linkSet *)b;

	return strcmp(one->facility, two->facility);
}

static int addService(struct alternative newAlt, const char * altDir,
		      const char * stateDir, int flags) {
    struct alternativeSet set;
    struct alternative base;
    struct linkSet * newLinks;
    int i, j, k, rc;

    if ( (rc=readConfig(&set, newAlt.master.title, altDir, stateDir, flags)) && rc != 3 && rc != 2)
	return 2;

    if (set.numAlts) {
	if (strcmp(newAlt.master.facility, set.alts[0].master.facility)) {
	    fprintf(stderr, _("the primary link for %s must be %s\n"),
		    set.alts[0].master.title, set.alts[0].master.facility);
	    return 2;
	}

        /* Determine the maximal set of slave links. */
        base.numSlaves = set.alts[0].numSlaves;
	base.slaves = malloc(set.alts[0].numSlaves * sizeof(struct linkSet));
	memcpy(base.slaves, set.alts[0].slaves,
	       set.alts[0].numSlaves * sizeof(struct linkSet));

	for (i = 0; i < newAlt.numSlaves; i++) {
		for (j = 0; j < base.numSlaves; j++) {
			if (!strcmp(newAlt.slaves[i].facility, base.slaves[j].facility))
				break;
		}
		if (j == base.numSlaves) {
			base.slaves = realloc(base.slaves, (base.numSlaves+1) * sizeof (*newLinks));
			base.slaves[base.numSlaves].facility = newAlt.slaves[i].facility;
			base.slaves[base.numSlaves].title = newAlt.slaves[i].title;
			base.slaves[base.numSlaves].target = NULL;
			base.numSlaves++;
		}
	}

	/* Insert new set into the set of alternatives */
	for (i = 0 ; i < set.numAlts ; i++) {
		if (!strcmp(set.alts[i].master.target, newAlt.master.target)) {
			set.alts[i] = newAlt;
			break;
		}

	}
	if (i == set.numAlts) {
		set.alts = realloc(set.alts, sizeof(*set.alts) * (set.numAlts + 1));
		set.alts[set.numAlts] = newAlt;
		if (set.alts[set.best].priority < newAlt.priority)
			set.best = set.numAlts;
		set.numAlts++;
	} else {
		/* Check for slaves no alternative provides */
		i = 0;
		while (i < base.numSlaves) {
			for (j = 0; j < set.numAlts; j++) {
				for (k = 0; k < set.alts[j].numSlaves; k++) {
					if (strcmp(set.alts[j].slaves[k].title,
						   base.slaves[i].title) == 0
					    && set.alts[j].slaves[k].target)
						goto found;
				}

			}
			removeLinks(base.slaves + i, altDir, flags);
			base.numSlaves--;
			if (i != base.numSlaves)
				base.slaves[i] = base.slaves[base.numSlaves];
			continue;

		found:
			i++;
		}
	}

	/* Sort the list for file legibility */
	qsort(base.slaves, base.numSlaves, sizeof(struct linkSet), linkCmp);

	/* need to match the slaves up; newLinks will parallel the original
	   ordering */
	for (k = 0; k < set.numAlts ; k++) {
		newLinks = alloca(sizeof(*newLinks) * base.numSlaves);
		newLinks = memset(newLinks, 0, sizeof(*newLinks) * base.numSlaves);

		for (j = 0; j < base.numSlaves; j++) {
			for (i = 0; i < set.alts[k].numSlaves; i++) {
				if (!strcmp(set.alts[k].slaves[i].title,
					    base.slaves[j].title)) break;
			}
			if (i < set.alts[k].numSlaves) {
				if (strcmp(set.alts[k].slaves[i].facility,
					   base.slaves[j].facility)) {
					fprintf(stderr, _("link %s incorrect for slave %s (%s %s)\n"),
						set.alts[k].slaves[i].facility,
						set.alts[k].slaves[i].title,
						base.slaves[j].facility,
						base.slaves[j].title);
					return 2;
				}
				newLinks[j] = set.alts[k].slaves[i];
			} else {
				newLinks[j].title = base.slaves[j].title;
				newLinks[j].facility = base.slaves[j].facility;
				newLinks[j].target = NULL;
			}
		}
		/* memory link */
		set.alts[k].slaves = newLinks;
		set.alts[k].numSlaves = base.numSlaves;
	}
    } else {
	    set.alts = realloc(set.alts, sizeof(*set.alts) * (set.numAlts + 1));
	    set.alts[set.numAlts] = newAlt;
	    if (set.alts[set.best].priority < newAlt.priority)
		    set.best = set.numAlts;
	    set.numAlts++;
    }

    if (writeState(&set, altDir, stateDir, 0, flags)) return 2;

    return 0;
}

static int displayService(char * title, const char * altDir,
		          const char * stateDir, int flags) {
    struct alternativeSet set;

    int alt;
    int slave;

    if (readConfig(&set, title, altDir, stateDir, flags)) return 2;

    if (set.mode == AUTO)
	printf(_("%s - status is auto.\n"), title);
    else
	printf(_("%s - status is manual.\n"), title);

    printf(_(" link currently points to %s\n"), set.currentLink);

    for (alt = 0; alt < set.numAlts; alt++) {
	printf(_("%s - priority %d\n"), set.alts[alt].master.target,
		    set.alts[alt].priority);
	for (slave = 0; slave < set.alts[alt].numSlaves; slave++) {
	    printf(_(" slave %s: %s\n"), set.alts[alt].slaves[slave].title,
		   set.alts[alt].slaves[slave].target);
	}
    }

    printf(_("Current `best' version is %s.\n"),
	   set.alts[set.best].master.target);

    return 0;
}

static int autoService(char * title, const char * altDir,
		       const char * stateDir, int flags) {
    struct alternativeSet set;

    if (readConfig(&set, title, altDir, stateDir, flags)) return 2;
    set.current = set.best;
    set.mode = AUTO;

    if (writeState(&set, altDir, stateDir, 0, flags)) return 2;

    return 0;
}

static int configService(char * title, const char * altDir,
		       const char * stateDir, int flags) {
    struct alternativeSet set;
    int i;
    char choice[200];
    char * end = NULL;

    if (readConfig(&set, title, altDir, stateDir, flags)) return 2;

    do {
	printf("\n");
	printf(ngettext(_("There is %d program that provides '%s'.\n"), _("There are %d programs which provide '%s'.\n"), set.numAlts), set.numAlts, set.alts[0].master.title);
	printf("\n");
	printf(_("  Selection    Command\n"));
	printf("-----------------------------------------------\n");

	for (i = 0; i < set.numAlts; i++)
	    printf("%c%c %-4d        %s\n",
		   i == set.best ? '*' : ' ',
		   i == set.current ? '+' : ' ',
		   i + 1, set.alts[i].master.target);
	printf("\n");
	printf(_("Enter to keep the current selection[+], or type selection number: "));

	if (!fgets(choice, sizeof(choice), stdin)) {
	    fprintf(stderr, _("\nerror reading choice\n"));
	    return 2;
	}
	i = strtol(choice, &end, 0);
	if ((*end == '\n') && (end != choice)) {
	  set.current = i - 1;
	}
    } while (!end || *end != '\n' || (set.current < 0) || (set.current >= set.numAlts));

    set.mode = MANUAL;
    if (writeState(&set, altDir, stateDir, 1, flags)) return 2;

    return 0;
}

static int setService(const char * title, const char * target,
		      const char * altDir, const char * stateDir, int flags) {
    struct alternativeSet set;
    int i;

    if (readConfig(&set, title, altDir, stateDir, flags)) return 2;

    for (i = 0; i < set.numAlts; i++)
	if (!strcmp(set.alts[i].master.target, target)) break;

    if (i == set.numAlts) {
	fprintf(stderr,
	        _("%s has not been configured as an alternative for %s\n"),
		target, title);
	return 2;
    }

    set.current = i;
    set.mode = MANUAL;

    if (writeState(&set, altDir, stateDir, 1, flags)) return 2;

    return 0;
}

static int removeService(const char * title, const char * target,
		      const char * altDir, const char * stateDir, int flags) {
    int rc;
    struct alternativeSet set;
    int i;

    if (readConfig(&set, title, altDir, stateDir, flags)) return 2;

    for (i = 0; i < set.numAlts; i++)
	if (!strcmp(set.alts[i].master.target, target)) break;

    if (i == set.numAlts) {
	fprintf(stderr,
	        _("%s has not been configured as an alternative for %s\n"),
		target, title);
	return 2;
    }

    if (set.numAlts == 1) {
	char * path;

	rc = removeLinks(&set.alts[0].master, altDir, flags);

	for (i = 0; i < set.alts[0].numSlaves; i++)
	    rc |= removeLinks(set.alts[0].slaves + i, altDir, flags);

	path = alloca(strlen(stateDir) + strlen(title) + 2);
	sprintf(path, "%s/%s", stateDir, title);
	if (FL_TEST(flags)) {
	    printf(_("(would remove %s\n"), path);
	} else if (unlink(path)) {
	    fprintf(stderr, _("failed to remove %s: %s\n"), path,
		    strerror(errno));
	    rc |= 1;
	}

	if (rc) return 2; else return 0;
    }

    /* If the current link is what we're removing, reset it. */
    if (set.current == i)
	set.current = -1;

    /* Replace removed link set with last one */
    set.alts[i] = set.alts[set.numAlts - 1];
    if (set.current == (set.numAlts - 1))
	set.current = i;
    set.numAlts--;

    set.best = 0;
    for (i = 0; i < set.numAlts; i++)
	if (set.alts[i].priority > set.alts[set.best].priority)
	    set.best = i;

    if (set.current == -1) {
	set.mode = AUTO;
	set.current = set.best;
    }

    if (writeState(&set, altDir, stateDir, 0, flags)) return 2;

    return 0;
}

int main(int argc, const char ** argv) {
    const char ** nextArg;
    char * end;
    char * title, * target;
    enum programModes mode = MODE_UNKNOWN;
    struct alternative newAlt = { -1, { NULL, NULL, NULL }, NULL, NULL, 0 };
    int flags = 0;
    char * altDir = "/etc/alternatives";
    char * stateDir = "/var/lib/alternatives";
    struct stat sb;

    setlocale(LC_ALL, "");
    bindtextdomain("chkconfig","/usr/share/locale");
    textdomain("chkconfig");

    if (!argv[1])
	return usage(2);

    nextArg = argv + 1;
    while (*nextArg) {
	if (!strcmp(*nextArg, "--install")) {
	    if (mode != MODE_UNKNOWN && mode != MODE_SLAVE) usage(2);
	    mode = MODE_INSTALL;
	    nextArg++;

	    setupLinkSet(&newAlt.master, &nextArg);

	    if (!*nextArg) usage(2);
	    newAlt.priority = strtol(*nextArg, &end, 0);
	    if (!end || *end) usage(2);
	    nextArg++;
	} else if (!strcmp(*nextArg, "--slave")) {
	    if (mode != MODE_UNKNOWN && mode != MODE_INSTALL) usage(2);
	    if (mode == MODE_UNKNOWN) mode = MODE_SLAVE;
	    nextArg++;

	    newAlt.slaves = realloc(newAlt.slaves,
			     sizeof(*newAlt.slaves) * (newAlt.numSlaves + 1));
	    setupLinkSet(newAlt.slaves + newAlt.numSlaves, &nextArg);
	    newAlt.numSlaves++;
	} else if (!strcmp(*nextArg, "--initscript")) {
	    if (mode != MODE_UNKNOWN && mode != MODE_INSTALL) usage(2);
	    nextArg++;

	    if (!*nextArg) usage(2);
	    newAlt.initscript = strdup(*nextArg);
	    nextArg++;
	} else if (!strcmp(*nextArg, "--remove")) {
	    setupDoubleArg(&mode, &nextArg, MODE_REMOVE, &title, &target);
	} else if (!strcmp(*nextArg, "--set")) {
	    setupDoubleArg(&mode, &nextArg, MODE_SET, &title, &target);
	} else if (!strcmp(*nextArg, "--auto")) {
	    setupSingleArg(&mode, &nextArg, MODE_AUTO, &title);
	} else if (!strcmp(*nextArg, "--display")) {
	    setupSingleArg(&mode, &nextArg, MODE_DISPLAY, &title);
	} else if (!strcmp(*nextArg, "--config")) {
	    setupSingleArg(&mode, &nextArg, MODE_CONFIG, &title);
	} else if (!strcmp(*nextArg, "--help") ||
		   !strcmp(*nextArg, "--usage")) {
	    if (mode != MODE_UNKNOWN) usage(2);
	    mode = MODE_USAGE;
	    nextArg++;
	} else if (!strcmp(*nextArg, "--test")) {
	    flags |= FLAGS_TEST;
	    nextArg++;
	} else if (!strcmp(*nextArg, "--verbose")) {
	    flags |= FLAGS_VERBOSE;
	    nextArg++;
	} else if (!strcmp(*nextArg, "--version")) {
	    if (mode != MODE_UNKNOWN) usage(2);
	    mode = MODE_VERSION;
	    nextArg++;
	} else if (!strcmp(*nextArg, "--altdir")) {
	    nextArg++;
	    if (!*nextArg) usage(2);
	    altDir = strdup(*nextArg);
	    nextArg++;
	} else if (!strcmp(*nextArg, "--admindir")) {
	    nextArg++;
	    if (!*nextArg) usage(2);
	    stateDir = strdup(*nextArg);
	    nextArg++;
	} else {
	    usage(2);
	}
    }

    if (stat(altDir, &sb) || !S_ISDIR(sb.st_mode) ||
	    access(altDir, F_OK)) {
	fprintf(stderr, _("altdir %s invalid\n"), altDir);
	return(2);
    }

    if (stat(stateDir, &sb) || !S_ISDIR(sb.st_mode) ||
	    access(stateDir, F_OK)) {
	fprintf(stderr, _("admindir %s invalid\n"), stateDir);
	return(2);
    }

    switch (mode) {
      case MODE_UNKNOWN:
	usage(2);
      case MODE_USAGE:
	usage(0);
      case MODE_VERSION:
	printf(_("alternatives version %s\n"), VERSION);
	exit(0);
      case MODE_INSTALL:
	return addService(newAlt, altDir, stateDir, flags);
      case MODE_DISPLAY:
	return displayService(title, altDir, stateDir, flags);
      case MODE_AUTO:
	return autoService(title, altDir, stateDir, flags);
      case MODE_CONFIG:
	return configService(title, altDir, stateDir, flags);
      case MODE_SET:
	return setService(title, target, altDir, stateDir, flags);
      case MODE_REMOVE:
	return removeService(title, target, altDir, stateDir, flags);
      case MODE_SLAVE:
	usage(2);
    }

    abort();
}
