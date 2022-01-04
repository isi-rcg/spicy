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
#include <alloca.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <libintl.h>
#include <locale.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Changes
   1998-09-22 - Arnaldo Carvalho de Melo <acme@conectiva.com.br>
                i18n for init.d scripts (eg.: description(pt_BR) is a brazilian
		portuguese description for the package)
*/

#define _(String) gettext((String))

#include "leveldb.h"

int parseLevels(char * str, int emptyOk) {
    char * chptr = str;
    int rc = 0;

    if (!str || !strlen(str))
	return emptyOk ? 0 : -1;

    while (*chptr) {
	if (!isdigit(*chptr) || *chptr > '6') return -1;
	rc |= 1 << (*chptr - '0');
	chptr++;
    }

    return rc;
}

int readDescription(char *start, char *bufstop, char **english_desc, char **serv_desc) {
	char english;
	char my_lang_loaded = 0;
	char is_my_lang = 0;
	char * lang = getenv ("LANG");
	char * final_parenthesis;
	char * end, *next;
	int i;

	english = *start == ':';
	end = strchr(start, '\n');
	if (!end)
	    next = end = bufstop;
	else
	    next = end + 1;

	if (!english) {
		if (*start != '(') {
		    return 1;
		}

                ++start;
		final_parenthesis = strchr (start, ')');

		if (final_parenthesis == NULL || final_parenthesis - start > 5) {
		    return 1;
		}

		is_my_lang = lang ? strncmp (lang, start, strlen (lang)) == 0 : 0;
		start = final_parenthesis + 2;
	    } else ++start;

	    while (isspace(*start) && start < end) start++;
	    if (start >= end) {
		return 1;
	    }
          {
	    char* desc = malloc(end - start + 1);
	    strncpy(desc, start, end - start);
	    desc[end - start] = '\0';

	    start = next;

	    while (desc[strlen(desc) - 1] == '\\') {
		desc[strlen(desc) - 1] = '\0';
		start = next;

		while (isspace(*start) && start < bufstop) start++;
		if (start == bufstop || *start != '#') {
		    return 1;
		}

		start++;

		while (isspace(*start) && start < bufstop) start++;
		if (start == bufstop) {
		    return 1;
		}

		end = strchr(start, '\n');
		if (!end)
		    next = end = bufstop;
		else
		    next = end + 1;

		i = strlen(desc);
		desc = realloc(desc, i + end - start + 1);
		strncat(desc, start, end - start);
		desc[i + end - start] = '\0';

		start = next;
	    }

	    if (desc) {
		    if (my_lang_loaded) {
			    free(desc);
		    } else if (is_my_lang) {
			    if (*serv_desc)
			      free(*serv_desc);

			    *serv_desc = desc;
			    return 0;
		    } else if (english) {
			    if (*serv_desc)
			      free(*serv_desc);

			    if (*english_desc)
			      free (*english_desc);

			    *english_desc = desc;
		    } else free (desc);
	    }
	  }
	return 0;
}

int readXinetdServiceInfo(char *name, struct service * service) {
	char * filename;
	int fd;
	struct service serv = {
			name: NULL,
			levels: -1,
			kPriority: 100,
			sPriority: -1,
			desc: NULL,
			startDeps: NULL,
			stopDeps: NULL,
			softStartDeps: NULL,
			softStopDeps: NULL,
		        provides: NULL,
			type: TYPE_XINETD,
			isLSB: 0,
			enabled: -1
	};
	struct stat sb;
	char * buf = NULL, *ptr;
	char * eng_desc = NULL, *start;

	asprintf(&filename, XINETDDIR "/%s", name);

	if ((fd = open(filename, O_RDONLY)) < 0) goto out_err;
	fstat(fd,&sb);
	if (! S_ISREG(sb.st_mode)) goto out_err;
	buf = malloc(sb.st_size+1);
	if (read(fd,buf,sb.st_size)!=sb.st_size) goto out_err;
	close(fd);
        serv.name = strdup(name);
	buf[sb.st_size] = '\0';
	start = buf;
	while (buf) {
		ptr = strchr(buf,'\n');
		if (*buf == '#') {
			buf++;
			while (isspace(*buf) && buf < ptr) buf++;
			if (!strncmp(buf,"default:", 9)) {
				buf+=8;
				while(isspace(*buf)) buf++;
				if (!strncmp(buf+9,"on",2)) {
					serv.enabled = 1;
				} else {
					serv.enabled = 0;
				}
			} else if (!strncmp(buf,"description:",12)) {
				buf+=11;
				if (readDescription(buf,start+sb.st_size,
						    &serv.desc,&eng_desc)) {
					if (serv.desc) free(serv.desc);
				}
				if (!serv.desc) {
					if (eng_desc)
					  serv.desc = eng_desc;
                                        else
                                          serv.desc = strdup(name);
				} else if (eng_desc)
					  free (eng_desc);
			}
			if (ptr) {
				*ptr = '\0';
				ptr++;
			}
			buf = ptr;
			continue;
		}
		while (isspace(*buf) && buf < ptr) buf++;
		if (!strncmp(buf,"disable", 7)) {
			buf = strstr(buf,"=");
			if (buf)
			  do {
				  buf++;
			  } while(isspace(*buf));

			if (buf && strncmp(buf,"yes",3)) {
				serv.levels = parseLevels("0123456",0);
				if (serv.enabled == -1)
				  serv.enabled = 1;
			} else {
				serv.levels = 0;
				if (serv.enabled == -1)
				  serv.enabled = 0;
			}
		}
		if (ptr) {
			*ptr = '\0';
			ptr++;
		}
		buf = ptr;
	}
	*service = serv;
	return 0;
out_err:
        if (fd >= 0)
            close(fd);
        free(buf);
        free(filename);
        return -1;
}

int readServices(struct service **services) {
	DIR * dir;
	struct dirent * ent;
	struct stat sb;
	struct service *servs = NULL;
	int numservs = 0;
	char fn[1024];

	if (!(dir = opendir(RUNLEVELS "/init.d"))) {
		fprintf(stderr, _("failed to open %s/init.d: %s\n"), RUNLEVELS,
			strerror(errno));
		return 1;
	}

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

		sprintf(fn, RUNLEVELS "/init.d/%s", ent->d_name);
		if (stat(fn, &sb)) {
			continue;
		}
		if (!S_ISREG(sb.st_mode)) continue;
		servs = realloc(servs, (numservs+1) * sizeof(struct service));
		if (!readServiceInfo(ent->d_name, TYPE_INIT_D, servs + numservs, 0))
			numservs++;
	}
	*services = servs;
	return numservs;
}

int readServiceInfo(char * name, int type, struct service * service, int honorHide) {
    char * filename = NULL;
    int fd;
    struct service serv, serv_overrides;
    int parseret;

    if (!(type & TYPE_INIT_D))
	goto try_xinetd;

    asprintf(&filename, RUNLEVELS "/init.d/%s", name);

    if ((fd = open(filename, O_RDONLY)) < 0)
	goto try_xinetd;

    free(filename);
    parseret = parseServiceInfo(fd, name, &serv, honorHide, 0);
    if (parseret)
        return parseret;

    asprintf(&filename, RUNLEVELS "/chkconfig.d/%s", name);
    if ((fd = open(filename, O_RDONLY)) >= 0) {
        parseret = parseServiceInfo(fd, name, &serv_overrides, honorHide, 1);
        if (parseret >= 0) {
            if (serv_overrides.name) serv.name = serv_overrides.name;
            if (serv_overrides.levels != -1) serv.levels = serv_overrides.levels;
            if (serv_overrides.kPriority != -2) serv.kPriority = serv_overrides.kPriority;
            if (serv_overrides.sPriority != -2) serv.sPriority = serv_overrides.sPriority;
            if (serv_overrides.desc) serv.desc = serv_overrides.desc;
            if (serv_overrides.startDeps) serv.startDeps = serv_overrides.startDeps;
            if (serv_overrides.stopDeps) serv.stopDeps = serv_overrides.stopDeps;
            if (serv_overrides.softStartDeps) serv.softStartDeps = serv_overrides.softStartDeps;
            if (serv_overrides.softStopDeps) serv.softStopDeps = serv_overrides.softStopDeps;
            if (serv_overrides.provides) serv.provides = serv_overrides.provides;
            if (serv_overrides.isLSB || serv.isLSB) serv.isLSB = 1;
        }
    }
    serv.currentLevels = whatLevels(name);

    free(filename);
    *service = serv;
    return 0;

try_xinetd:
    free(filename);
    if (!(type & TYPE_XINETD))
	return -1;
    return readXinetdServiceInfo(name,service);
}

int readServiceDifferences(char * name, int type, struct service * service, struct service * service_overrides, int honorHide) {
    char * filename = NULL;
    int fd;
    struct service serv, serv_overrides;
    int parseret;

    if (!(type & TYPE_INIT_D))
	goto try_xinetd;

    asprintf(&filename, RUNLEVELS "/init.d/%s", name);

    if ((fd = open(filename, O_RDONLY)) < 0) {
	goto try_xinetd;
    }

    free(filename);
    parseret = parseServiceInfo(fd, name, &serv, honorHide, 0);
    if (parseret) {
        return parseret;
    }

    asprintf(&filename, RUNLEVELS "/chkconfig.d/%s", name);
    if ((fd = open(filename, O_RDONLY)) >= 0) {
        parseret = parseServiceInfo(fd, name, &serv_overrides, honorHide, 1);
    } else {
        parseret = 1;
    }

    free(filename);
    if (parseret) {
        return 1;
    }

    *service = serv;
    *service_overrides = serv_overrides;
    return 0;

try_xinetd:
    free(filename);
    if (!(type & TYPE_XINETD))
	return -1;
    return readXinetdServiceInfo(name,service);
}

static struct dep *parseDeps(char *pos, char *end) {
    char *t;
    int numdeps = 0;
    struct dep *deps = NULL;

    while (1) {
	while (*pos && isspace(*pos) && pos < end) pos++;
	if (pos == end)
            break;
        t = pos;
        while (*t && !isspace(*t) && t < end) t++;
        if (isspace(*t)) {
            *t = '\0';
            t++;
        }
        numdeps++;
        deps = realloc(deps,
		 (numdeps + 1) * sizeof(struct dep));
        deps[numdeps-1].name = strdup(pos);
        deps[numdeps-1].handled = 0;
        memset(&deps[numdeps],'\0',sizeof(struct dep));
        if (!t || t >= end)
            break;
        else
            pos = t;
    }
    return deps;
}


int parseServiceInfo(int fd, char * name, struct service * service, int honorHide, int partialOk) {
    struct stat sb;
    char * bufstart, * bufstop, * start, * end, * next, *tmpbufstart;
    struct service serv = {
		    name: NULL,
		    levels: -1,
		    kPriority: 100,
		    sPriority: -1,
		    currentLevels: 0,
		    desc: NULL,
		    startDeps: NULL,
		    stopDeps: NULL,
		    softStartDeps: NULL,
		    softStopDeps: NULL,
		    provides: NULL,
		    type: TYPE_INIT_D,
		    isLSB: 0,
		    enabled: 0
    };
    char overflow;
    char levelbuf[20];
    char * english_desc = NULL;

    fstat(fd, &sb);

    bufstart = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd, 0);
    if (bufstart == ((void*) -1)) {
	close(fd);
	return -1;
    }

    tmpbufstart = (char*)malloc(sb.st_size+1);
    if (tmpbufstart == NULL) {
	close(fd);
	return -1;
    }

    memcpy(tmpbufstart, bufstart, sb.st_size);
    munmap(bufstart, sb.st_size);

    bufstart = tmpbufstart;
    bufstop = bufstart + sb.st_size;
    *bufstop = 0;

    close(fd);

    next = bufstart;
    while (next < bufstop && (serv.levels == -1 || !serv.desc)) {
	start = next;

	while (isspace(*start) && start < bufstop) start++;
	if (start == bufstop) break;

	end = strchr(start, '\n');
	if (!end)
	    next = end = bufstop;
	else
	    next = end + 1;

	if (*start != '#') continue;

	start++;
	if (!strncmp(start, "## BEGIN INIT INFO", 18))
		    serv.isLSB = 1;

	while (isspace(*start) && start < end) start++;
	if (start == end) continue;
	if (honorHide && !strncmp(start, "hide:", 5)) {
	    start += 5;
	    while (isspace(*start) && start < end) start++;
	    if (start == end || !strncmp(start, "true", 4)) {
		if (serv.desc) free(serv.desc);
		free(bufstart);
		return 1;
	    }
	}

	if (!strncmp(start, "chkconfig:", 10)) {
	    int spri, kpri;

	    start += 10;
	    while (isspace(*start) && start < end) start++;
	    if (start == end) {
		if (serv.desc) free(serv.desc);
		free(bufstart);
		return 1;
	    }

	    if ((sscanf(start, "%15s %d %d%c", levelbuf,
			&spri, &kpri, &overflow) != 4) ||
		 !isspace(overflow)) {
		if (serv.desc) free(serv.desc);
		free(bufstart);
		return 1;
	    }
	    if (spri > 99 || kpri > 99 || kpri < 0 || spri < 0) {
		    if (serv.desc) free(serv.desc);
		    free(bufstart);
		    return 1;
	    }
	    if (serv.sPriority == -1)
			serv.sPriority = spri;
	    if (serv.kPriority == 100)
			serv.kPriority = kpri;

	    if (serv.levels == -1) {
		    if (!strcmp(levelbuf, "-"))
			    serv.levels = 0;
		    else
			    serv.levels = parseLevels(levelbuf, 0);
	    }
	    if (serv.levels == -1) {
		if (serv.desc) free(serv.desc);
		free(bufstart);
		return 1;
	    }
	} else if (!strncmp(start, "description", 11) ||
		   !strncmp(start, "Description:", 12)) {
		if (readDescription(start+11, bufstop, &english_desc, &serv.desc)) {
			if (serv.desc) free(serv.desc);
		}
	} else if (!strncmp(start, "Short-Description:", 18)) {
		if (readDescription(start+17, bufstop, &english_desc, &serv.desc)) {
			if (serv.desc) free(serv.desc);
		}
	} else if (!strncmp(start, "Default-Start:", 14)) {
		char *t;

		start+=14;
		while (1) {
			int lev;

			lev = strtol(start, &t, 10);
			if (t && t != start)
				start = t;
			else
				break;
			if (serv.levels == -1)
				serv.levels = 0;
			serv.levels |= 1 << lev;
		}
	} else if (!strncmp(start, "Default-Stop:", 13)) {
		char *t;

		start+=13;
		while (1) {
			int lev;

			lev = strtol(start, &t, 10);
			if (t && t != start)
				start = t;
			else
				break;
			if (serv.levels == -1)
				serv.levels = 0;
			serv.levels &= ~(1 << lev);
		}
	} else if (!strncmp(start, "Required-Start:", 15)) {
		start+=15;
		serv.startDeps = parseDeps(start, end);
	} else if (!strncmp(start, "Required-Stop:", 14)) {
		start+=14;
		serv.stopDeps = parseDeps(start, end);
	} else if (!strncmp(start, "Should-Start:", 13)) {
		start+=13;
		serv.softStartDeps = parseDeps(start, end);
	} else if (!strncmp(start, "Should-Stop:", 12)) {
		start+=12;
		serv.softStopDeps = parseDeps(start, end);
	} else if (!strncmp(start, "Provides:", 9)) {
		char *t;
		int numdeps = 0;

		start+=9;
		while (1) {
			while (*start && isspace(*start) && start < end) start++;
			if (start == end)
				break;
			t = start;
			while (*t && !isspace(*t) && t < end) t++;
			if (isspace(*t)) {
				*t = '\0';
				t++;
			}
			numdeps++;
			serv.provides = realloc(serv.provides,
						 (numdeps + 1) * sizeof(char *));
			serv.provides[numdeps-1] = strdup(start);
			serv.provides[numdeps] = NULL;
			if (!t || t >= end)
				break;
			else
				start = t;
		}

	}
    }

    free(bufstart);

    if (!serv.desc) {
      if (english_desc)
	serv.desc = english_desc;
      else
        serv.desc = strdup(name);
    } else if (english_desc)
	free (english_desc);

    if (!partialOk && ((serv.levels == -1) || !serv.desc || (!serv.isLSB && (serv.sPriority == -1 || serv.kPriority == 100)))) {
	return 1;
    }

    serv.name = strdup(name);
    if (!serv.provides) {
	    serv.provides = malloc(2 * sizeof(char *));
	    serv.provides[0] = strdup(name);
	    serv.provides[1] = NULL;
    }

    *service = serv;
    return 0;
}

/* returns -1 on error */
int currentRunlevel(void) {
    FILE * p;
    char response[50];

    p = popen("/sbin/runlevel", "r");
    if (!p) return -1;

    if (!fgets(response, sizeof(response), p)) {
	pclose(p);
	return -1;
    }

    pclose(p);

    if (response[1] != ' ' || !isdigit(response[2]) || response[3] != '\n')
	return -1;

    return response[2] - '0';
}

int findServiceEntries(char * name, int level, glob_t * globresptr) {
    char match[200];
    glob_t globres;
    int rc;

    sprintf(match, "%s/rc%d.d/[SK][0-9][0-9]%s", RUNLEVELS, level, name);

    rc = glob(match, GLOB_ERR | GLOB_NOSORT, NULL, &globres);

    if (rc && rc != GLOB_NOMATCH) {
	fprintf(stderr, _("failed to glob pattern %s: %s\n"), match,
		strerror(errno));
	return 1;
    } else if (rc == GLOB_NOMATCH) {
	globresptr->gl_pathc = 0;
	return 0;
    }

    *globresptr = globres;
    return 0;
}

int isConfigured(char * name, int level, int *priority, char *type) {
    glob_t globres;
    char *pri_string;

    if (findServiceEntries(name, level, &globres))
	exit(1);

    if (!globres.gl_pathc)
	return 0;

    if (type) {
        *type = globres.gl_pathv[0][11];
    }

    if (priority) {
        pri_string = strndup(globres.gl_pathv[0]+12, 2);
        if (!pri_string) return 0;
        sscanf(pri_string, "%d", priority);
        free(pri_string);
    }

    globfree(&globres);
    return 1;
}

int isOn(char * name, int level) {
    glob_t globres;

    if (level == -1) {
	level = currentRunlevel();
	if (level == -1) {
	    fprintf(stderr, _("cannot determine current run level\n"));
	    return 0;
	}
    }

    if (findServiceEntries(name, level, &globres))
	exit(1);

    if (!globres.gl_pathc || !strstr(globres.gl_pathv[0], "/S"))
	return 0;

    globfree(&globres);
    return 1;
}

int whatLevels(char *name) {
    int i, ret = 0;

    for (i = 0; i < 7; i++) {
        ret |= (isOn(name, i) << i);
    }
    return ret;
}

int setXinetdService(struct service s, int on) {
	int oldfd, newfd;
	char oldfname[100], newfname[100];
	char tmpstr[50];
	char *buf, *ptr, *tmp;
	struct stat sb;

	if (on == -1) {
		on = s.enabled ? 1 : 0;
	}
	snprintf(oldfname,100,"%s/%s",XINETDDIR,s.name);
	if ( (oldfd = open(oldfname,O_RDONLY)) == -1 ) {
		return -1;
	}
	fstat(oldfd,&sb);
	buf = malloc(sb.st_size+1);
	if (read(oldfd,buf,sb.st_size)!=sb.st_size) {
		close(oldfd);
		free(buf);
		return -1;
	}
	close(oldfd);
	buf[sb.st_size] = '\0';
	snprintf(newfname,100,"%s/%s.XXXXXX",XINETDDIR,s.name);
	newfd = mkstemp(newfname);
	if (newfd == -1) {
		free(buf);
		return -1;
	}
	while (buf) {
		tmp = buf;
		ptr = strchr(buf,'\n');
		if (ptr) {
			*ptr = '\0';
			ptr++;
		}
		while (isspace(*buf)) buf++;
		if (strncmp(buf,"disable", 7) && strlen(buf)) {
			write(newfd,tmp,strlen(tmp));
			write(newfd,"\n",1);
			if (buf[0] == '{') {
				snprintf(tmpstr,50,"\tdisable\t= %s", on ? "no" : "yes");
				write(newfd,tmpstr,strlen(tmpstr));
				write(newfd,"\n",1);
			}
		}
		buf = ptr;
	}
	close(newfd);
	chmod(newfname,0644);
	unlink(oldfname);
	return(rename(newfname,oldfname));
}

int doSetService(struct service s, int level, int on) {
    int priority = on ? s.sPriority : s.kPriority;
    char linkname[200];
    char linkto[200];
    glob_t globres;
    int i;

    if (!findServiceEntries(s.name, level, &globres)) {
	for (i = 0; i < globres.gl_pathc; i++)
	    unlink(globres.gl_pathv[i]);
	if (globres.gl_pathc) globfree(&globres);
    }

    sprintf(linkname, "%s/rc%d.d/%c%02d%s", RUNLEVELS, level,
			on ? 'S' : 'K', priority, s.name);
    sprintf(linkto, "../init.d/%s", s.name);

    unlink(linkname);	/* just in case */
    if (symlink(linkto, linkname)) {
	fprintf(stderr, _("failed to make symlink %s: %s\n"), linkname,
		strerror(errno));
	return 1;
    }

    return 0;
}

int systemdIsInit() {
    char *path = realpath("/sbin/init", NULL);
    char *base;

    if (!path)
        return 0;
    base = basename(path);
    if (!base)
        return 0;
    if (strcmp(base,"systemd"))
        return 0;
    return 1;
}

int systemdActive() {
    struct stat a, b;

    if (lstat("/sys/fs/cgroup", &a) < 0)
        return 0;
    if (lstat("/sys/fs/cgroup/systemd", &b) < 0)
        return 0;
    if (a.st_dev == b.st_dev)
        return 0;
    if (!systemdIsInit())
        return 0;
    return 1;
}


int isOverriddenBySystemd(const char *service) {
    char *p;
    int rc = 0;

    asprintf(&p, SYSTEMD_SERVICE_PATH "/%s.service", service);

    if (access(p, F_OK) >= 0) {
        rc = 1;
    }
    free(p);
    return rc;
}
