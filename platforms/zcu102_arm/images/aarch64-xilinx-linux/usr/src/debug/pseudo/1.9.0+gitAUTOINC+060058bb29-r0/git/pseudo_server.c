/*
 * pseudo_server.c, pseudo's server-side logic and message handling
 *
 * Copyright (c) 2008-2010, 2013 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/wait.h>
#ifdef PSEUDO_EPOLL
#include <sys/epoll.h>
#endif
#include <signal.h>

#include "pseudo.h"
#include "pseudo_ipc.h"
#include "pseudo_server.h"
#include "pseudo_client.h"
#include "pseudo_db.h"

static int listen_fd = -1;

typedef struct {
	int fd;
	pid_t pid;
	char *tag;
	char *program;
} pseudo_client_t;

pseudo_client_t *clients;

/* active_clients: Number of clients we actually have right now.
 * highest_client: Highest index into clients table of an active client.
 * max_clients: Size of table.
 */
static int active_clients = 0, highest_client = 0, max_clients = 0;

#define LOOP_DELAY 2
#define DEFAULT_PSEUDO_SERVER_TIMEOUT 30
#define EPOLL_MAX_EVENTS 10
int pseudo_server_timeout = DEFAULT_PSEUDO_SERVER_TIMEOUT;
static int die_peacefully = 0;
static int die_forcefully = 0;
static sig_atomic_t do_list_clients = 0;

/* when the client is linked with pseudo_wrappers, these are defined there.
 * when it is linked with pseudo_server, though, we have to provide different
 * versions (pseudo_wrappers must not be linked with the server, or Bad Things
 * happen).
 */
void pseudo_magic(void) { }
void pseudo_antimagic(void) { }

void
quit_now(int signal) {
	pseudo_diag("Received signal %d, quitting.\n", signal);
	die_forcefully = 1;
}

static void
set_do_list_clients(int sig) {
	do_list_clients = sig;
}

static int messages = 0, responses = 0;
static struct timeval message_time = { .tv_sec = 0 };

#ifdef PSEUDO_EPOLL
static void pseudo_server_loop_epoll(void);
#else
static void pseudo_server_loop(void);
#endif

/* helper function to make a directory, just like mkdir -p.
 * Can't use system() because the child shell would end up trying
 * to do the same thing...
 */
static void
mkdir_p(char *path) {
	size_t len = strlen(path);
	size_t i;

	for (i = 1; i < len; ++i) {
		/* try to create all the directories in path, ignoring
		 * failures
		 */
		if (path[i] == '/') {
			path[i] = '\0';
			(void) mkdir(path, 0755);
			path[i] = '/';
		}
	}
	(void) mkdir(path, 0755);
}


static int
pseudo_server_write_pid(pid_t pid) {
	char *pseudo_path;
	FILE *fp;

	pseudo_path = pseudo_localstatedir_path(PSEUDO_PIDFILE);
	if (!pseudo_path) {
		pseudo_diag("Couldn't get path for prefix/%s\n", PSEUDO_PIDFILE);
		return 1;
	}
	fp = fopen(pseudo_path, "w");
	if (!fp) {
		pseudo_diag("Couldn't open %s: %s\n",
			pseudo_path, strerror(errno));
		return 1;
	}
	fprintf(fp, "%lld\n", (long long) pid);
	fclose(fp);
	free(pseudo_path);
	return 0;
}

static sig_atomic_t got_sigusr1 = 0;
static sig_atomic_t got_sigalrm = 0;

static void
handle_sigusr1(int sig) {
	(void) sig;
	got_sigusr1 = 1;
}

static void
handle_sigalrm(int sig) {
	(void) sig;
	got_sigalrm = 1;
}

#define PSEUDO_CHILD_PROCESS_TIMEOUT 2
int
pseudo_server_start(int daemonize) {
	struct sockaddr_un sun = { .sun_family = AF_UNIX, .sun_path = PSEUDO_SOCKET };
	char *pseudo_path;
	int newfd, lockfd;
	int rc, save_errno;
	char *lockname;
	char *lockpath;
	struct flock lock_data;

	/* we want a sane umask for server operations; this is what
	 * would control the modes of database files, sockets, and so
	 * on.
	 */
	umask(022);
	/* parent process will wait for child process, or until it gets
	 * SIGUSR1, or until too much time has passed. */
	if (daemonize) {
		int child;

		/* make startup messages go away when invoked-as-daemon */
		pseudo_debug_logfile(PSEUDO_LOGFILE, 2);
		child = fork();
		if (child == -1) {
			pseudo_diag("Couldn't fork child process: %s\n",
				strerror(errno));
			exit(PSEUDO_EXIT_FORK_FAILED);
		}
		if (child) {
			int status;
			int rc;
			int save_errno;

			got_sigusr1 = 0;
			signal(SIGUSR1, handle_sigusr1);
			signal(SIGALRM, handle_sigalrm);
			alarm(PSEUDO_CHILD_PROCESS_TIMEOUT);
			int tries = 0;
			do {
				rc = waitpid(child, &status, WNOHANG);
				save_errno = errno;
				if (rc != child && !got_sigalrm && !got_sigusr1) {
					struct timespec delay = { .tv_sec = 0, .tv_nsec = 100000 };
					nanosleep(&delay, NULL);
					++tries;
				}

			} while (!got_sigalrm && !got_sigusr1 && rc != child);
			alarm(0);
			pseudo_debug(PDBGF_SERVER, "pid waited: %d/%d [%d tries], status %d, usr1 %d, alrm %d\n",
				rc, save_errno, tries, status,
				got_sigusr1, got_sigalrm);
			if (got_sigusr1) {
				pseudo_debug(PDBGF_SERVER, "server says it's ready.\n");
				exit(0);
			}
			if (got_sigalrm) {
				pseudo_diag("Child process timeout after %d seconds.\n",
					PSEUDO_CHILD_PROCESS_TIMEOUT);
				exit(PSEUDO_EXIT_TIMEOUT);
			}
			if (rc == -1) {
				pseudo_diag("Failure in waitpid(): %s\n",
					strerror(save_errno));
				exit(PSEUDO_EXIT_WAITPID);
			}
			if (WIFSIGNALED(status)) {
				status = WTERMSIG(status);
				pseudo_diag("Child process exited from signal %d.\n",
					status);
				kill(getpid(), status);
				/* can't use +128 because that's not valid */
				exit(status + 64);
			}
			if (WIFEXITED(status)) {
				status = WEXITSTATUS(status);
				pseudo_diag("Child process exit status %d: %s\n",
					status,
					pseudo_exit_status_name(status));
				if (status == 0) {
					pseudo_diag("Hang on, server should not have exited 0 without sending us sigusr1?\n");
				}
				exit(status);
			}
			pseudo_diag("Unknown exit status %d.\n", status);
			exit(PSEUDO_EXIT_GENERAL);
		} else {
			/* detach from parent session */
			setsid();
			fclose(stdin);
			fclose(stdout);
			/* and then just execute the server code normally.  */
			/* Any logging will presumably go to logfile, but
			 * exit status will make it back to the parent for
			 * reporting. */
		}
	}

	pseudo_diag("pid %d [parent %d], doing new pid setup and server start\n", getpid(), getppid());
	pseudo_new_pid();

	pseudo_debug(PDBGF_SERVER, "opening lock.\n");
	lockpath = pseudo_localstatedir_path(NULL);
	if (!lockpath) {
		pseudo_diag("Couldn't allocate a file path.\n");
		exit(PSEUDO_EXIT_LOCK_PATH);
	}
	mkdir_p(lockpath);
	lockname = pseudo_localstatedir_path(PSEUDO_LOCKFILE);
	if (!lockname) {
		pseudo_diag("Couldn't allocate a file path.\n");
		exit(PSEUDO_EXIT_LOCK_PATH);
	}
	lockfd = open(lockname, O_RDWR | O_CREAT, 0644);
	if (lockfd < 0) {
		pseudo_diag("Can't open or create lockfile %s: %s\n",
			lockname, strerror(errno));
		exit(PSEUDO_EXIT_LOCK_FAILED);
	}
	free(lockname);

	/* the lock shuffle has to happen before an fcntl lock, which
	 * is automatically dropped if *any* file descriptor on the file
	 * is closed...
	 */
	if (lockfd <= 2) {
		newfd = fcntl(lockfd, F_DUPFD, 3);
		if (newfd < 0) {
			pseudo_diag("Can't move lockfile to safe descriptor, leaving it on %d: %s\n",
				lockfd, strerror(errno));
		} else {
			close(lockfd);
			lockfd = newfd;
		}
	}

	pseudo_debug(PDBGF_SERVER, "acquiring lock.\n");

	memset(&lock_data, 0, sizeof(lock_data));
	lock_data.l_type = F_WRLCK;
	lock_data.l_whence = SEEK_SET;
	lock_data.l_start = 0;
	lock_data.l_len = 0;

	rc = fcntl(lockfd, F_SETLK, &lock_data);
	if (rc < 0) {
		save_errno = errno;
		if (save_errno == EACCES || save_errno == EAGAIN) {
			rc = fcntl(lockfd, F_GETLK, &lock_data);
			if (rc == 0 && lock_data.l_type != F_UNLCK) {
				pseudo_diag("lock already held by existing pid %d.\n",
					lock_data.l_pid);
			}
		}
		pseudo_diag("Couldn't obtain lock: %s.\n", strerror(save_errno));
		exit(PSEUDO_EXIT_LOCK_HELD);

	} else {
		pseudo_debug(PDBGF_SERVER, "Acquired lock.\n");
	}

#if PSEUDO_PORT_DARWIN
	sun.sun_len = strlen(PSEUDO_SOCKET) + 1;
#endif

	listen_fd = socket(PF_UNIX, SOCK_STREAM, 0);
	if (listen_fd < 0) {
		pseudo_diag("couldn't create listening socket: %s\n", strerror(errno));
		exit(PSEUDO_EXIT_SOCKET_CREATE);
	}

	if (listen_fd <= 2) {
		newfd = fcntl(listen_fd, F_DUPFD, 3);
		if (newfd < 0) {
			pseudo_diag("couldn't dup listening socket: %s\n", strerror(errno));
			close(listen_fd);
			exit(PSEUDO_EXIT_SOCKET_FD);
		} else {
			close(listen_fd);
			listen_fd = newfd;
		}
	}

	/* cd to the data directory */
	pseudo_path = pseudo_localstatedir_path(NULL);
	if (!pseudo_path || chdir(pseudo_path) == -1) {
		pseudo_diag("can't get to '%s': %s\n",
			pseudo_path, strerror(errno));
		exit(PSEUDO_EXIT_SOCKET_PATH);
	}
	free(pseudo_path);
	/* remove existing socket -- if it exists */
	rc = unlink(sun.sun_path);
	if (rc == -1 && errno != ENOENT) {
		pseudo_diag("Can't unlink existing socket: %s.\n",
			strerror(errno));
		exit(PSEUDO_EXIT_SOCKET_UNLINK);
	}
	if (bind(listen_fd, (struct sockaddr *) &sun, sizeof(sun)) == -1) {
		pseudo_diag("couldn't bind listening socket: %s\n", strerror(errno));
		exit(PSEUDO_EXIT_SOCKET_BIND);
	}
	if (listen(listen_fd, 5) == -1) {
		pseudo_diag("couldn't listen on socket: %s\n", strerror(errno));
		exit(PSEUDO_EXIT_SOCKET_LISTEN);
	}
	rc = pseudo_server_write_pid(getpid());
	if (rc != 0) {
		pseudo_diag("warning: couldn't write pid file.\n");
	}
	signal(SIGHUP, quit_now);
	signal(SIGINT, quit_now);
	signal(SIGALRM, quit_now);
	signal(SIGQUIT, quit_now);
	signal(SIGTERM, quit_now);
	/* tell parent process to stop waiting */
	if (daemonize) {
		pid_t ppid = getppid();
		if (ppid == 1) {
			pseudo_diag("Setup complete, but parent is init, not sending SIGUSR1.\n");
		} else {
			pseudo_diag("Setup complete, sending SIGUSR1 to pid %d.\n",
				ppid);
			kill(ppid, SIGUSR1);
		}
	}
#ifdef PSEUDO_EPOLL
	pseudo_server_loop_epoll();
#else
	pseudo_server_loop();
#endif
	return 0;
}

/* mess with internal tables as needed */
static unsigned int
open_client(int fd) {
	pseudo_client_t *new_clients;
	int i;

	/* if possible, use first open client slot */
	for (i = 0; i < max_clients; ++i) {
		if (clients[i].fd == -1) {
			pseudo_debug(PDBGF_SERVER, "reusing client %d for fd %d\n", i, fd);
			clients[i].fd = fd;
			clients[i].pid = 0;
			clients[i].tag = NULL;
			clients[i].program = NULL;
			++active_clients;
			if (i > highest_client)
				highest_client = i;
			return i;
		}
	}

	/* otherwise, allocate a new one */
	new_clients = malloc(sizeof(*new_clients) * (max_clients + 16));
	if (new_clients) {
		memcpy(new_clients, clients, max_clients * sizeof(*clients));
		free(clients);
		for (i = max_clients; i < max_clients + 16; ++i) {
			new_clients[i].fd = -1;
			new_clients[i].pid = 0;
			new_clients[i].tag = NULL;
			new_clients[i].program = NULL;
		}
		clients = new_clients;
		clients[max_clients].fd = fd;
		clients[max_clients].pid = 0;
		clients[max_clients].tag = NULL;
		clients[max_clients].program = NULL;
		highest_client = max_clients + 1;

		max_clients += 16;
		++active_clients;
		return max_clients - 16;
	} else {
		pseudo_diag("error allocating new client, fd %d\n", fd);
		close(fd);
		return 0;
	}
}

/* clear pid/fd.  If this was the highest client, iterate downwards looking
 * for a lower one to be the new highest client.
 */
static void
close_client(int client) {
	pseudo_debug(PDBGF_SERVER, "lost client %d [%d], closing fd %d\n", client,
		clients[client].pid, clients[client].fd);
	/* client went away... */
	if (client > highest_client || client <= 0) {
		pseudo_diag("tried to close client %d (highest is %d)\n",
			client, highest_client);
		return;
	}
	close(clients[client].fd);
	clients[client].fd = -1;
	free(clients[client].tag);
	free(clients[client].program);
	clients[client].pid = 0;
	clients[client].tag = NULL;
	clients[client].program = NULL;
	--active_clients;
	if (client == highest_client)
		while (clients[highest_client].fd != -1 && highest_client > 0)
			--highest_client;
}

/* Actually process a request.
 */
static int
serve_client(int i) {
	pseudo_msg_t *in;
	int rc;
	struct timeval tv1, tv2;
	gettimeofday(&tv1, NULL);

	++messages;
	pseudo_debug(PDBGF_SERVER, "message from client %d [%d:%s - %s] fd %d\n",
		i, (int) clients[i].pid,
		clients[i].program ? clients[i].program : "???",
		clients[i].tag ? clients[i].tag : "NO TAG",
		clients[i].fd);
	in = pseudo_msg_receive(clients[i].fd);
	if (in) {
		char *response_path = 0;
		size_t response_pathlen;
		int send_response = 1;
		pseudo_debug(PDBGF_SERVER | PDBGF_VERBOSE, "got a message (%d): %s\n", in->type, (in->pathlen ? in->path : "<no path>"));
		/* handle incoming ping */
		if (in->type == PSEUDO_MSG_PING && !clients[i].pid) {
			pseudo_debug(PDBGF_SERVER, "new client: %d -> %d",
				i, in->client);
			clients[i].pid = in->client;
			if (in->pathlen) {
				size_t proglen;
				proglen = strlen(in->path);

				pseudo_debug(PDBGF_SERVER, " <%s>", in->path);
				free(clients[i].program);
				clients[i].program = malloc(proglen + 1);
				if (clients[i].program) {
					snprintf(clients[i].program, proglen + 1, "%s", in->path);
				}
				if (in->pathlen > proglen) {
					pseudo_debug(PDBGF_SERVER, " [%s]", in->path + proglen + 1);
					clients[i].tag = malloc(in->pathlen - proglen);
					if (clients[i].tag)
						snprintf(clients[i].tag, in->pathlen - proglen,
							"%s", in->path + proglen + 1);
				}
			}
			pseudo_debug(PDBGF_SERVER, "\n");
		}
		if (in->type == PSEUDO_MSG_SHUTDOWN && !clients[i].pid) {
			pseudo_debug(PDBGF_SERVER, "shutdown request from client %d [pid %d]\n",
				i, in->client);
			in->client = clients[i].pid;
		}
		/* sanity-check client ID */
		if (in->client != clients[i].pid) {
			pseudo_debug(PDBGF_SERVER, "uh-oh, expected pid %d for client %d, got %d\n",
				(int) clients[i].pid, i, in->client);
		}
		/* regular requests are processed in place by
		 * pseudo_server_response.
		 */
		if (in->type != PSEUDO_MSG_SHUTDOWN) {
			/* most messages don't need these, but xattr may */
			response_path = 0;
			response_pathlen = -1;
			if (pseudo_server_response(in, clients[i].program, clients[i].tag, &response_path, &response_pathlen)) {
				in->type = PSEUDO_MSG_NAK;
			} else {
				in->type = PSEUDO_MSG_ACK;
				pseudo_debug(PDBGF_SERVER | PDBGF_VERBOSE, "response: %d (%s)\n",
					in->result, pseudo_res_name(in->result));
			}
			in->client = i;
			if (response_path) {
				in->pathlen = response_pathlen;
			} else {
				in->pathlen = 0;
			}
			if (in->type == PSEUDO_MSG_FASTOP) {
				/* respond instantly */
				send_response = 0;
				int t_type = in->type;
				int t_pathlen = in->pathlen;
				in->type = PSEUDO_MSG_ACK;
				in->pathlen = 0;
				if ((rc = pseudo_msg_send(clients[i].fd, in, in->pathlen, response_path)) != 0) {
					pseudo_debug(PDBGF_SERVER, "failed to send response to client %d [%d]: %d (%s)\n",
						i, (int) clients[i].pid, rc, strerror(errno));
				}
				in->type = t_type;
				in->pathlen = t_pathlen;
			}
		} else {
			/* the server's listen fd is "a client", and
			 * so is the program connecting to request a shutdown.
			 * it should never be less than 2, but crazy things
			 * happen.  >2 implies some other active client,
			 * though.
			 */
			if (active_clients > 2) {
				int j;
				char *s;

				response_path = malloc(8 * active_clients);
				in->type = PSEUDO_MSG_NAK;
				in->fd = active_clients - 2;
				s = response_path;
				for (j = 1; j <= highest_client; ++j) {
					if (clients[j].fd != -1 && j != i) {
						s += snprintf(s, 8, "%d ", (int) clients[j].pid);
					}
				}
				in->pathlen = (s - response_path) + 1;
				/* exit quickly once clients go away, though */
				pseudo_server_timeout = 3;
			} else {
				in->type = PSEUDO_MSG_ACK;
				in->pathlen = 0;
				in->client = i;
				die_peacefully = 1;
			}
		}
		if (send_response) {
			if ((rc = pseudo_msg_send(clients[i].fd, in, in->pathlen, response_path)) != 0) {
				pseudo_debug(PDBGF_SERVER, "failed to send response to client %d [%d]: %d (%s)\n",
					i, (int) clients[i].pid, rc, strerror(errno));
			}
		} else {
			rc = 1;
		}
		free(response_path);
	} else {
		/* this should not be happening, but the exceptions aren't
		 * being detected in select() for some reason.
		 */
		pseudo_debug(PDBGF_SERVER, "client %d: no message\n", (int) clients[i].pid);
		close_client(i);
		rc = 0;
	}

	gettimeofday(&tv2, NULL);
	if (rc == 0)
		++responses;
	message_time.tv_sec += (tv2.tv_sec - tv1.tv_sec);
	message_time.tv_usec += (tv2.tv_usec - tv1.tv_usec);
	if (message_time.tv_usec < 0) {
		message_time.tv_usec += 1000000;
		--message_time.tv_sec;
	} else while (message_time.tv_usec > 1000000) {
		message_time.tv_usec -= 1000000;
		++message_time.tv_sec;
	}
	return rc;
}

#ifdef PSEUDO_EPOLL
static void pseudo_server_loop_epoll(void)
{
	struct sockaddr_un client;
	socklen_t len;
	int i;
	int rc;
	int fd;
	int timeout;
	struct epoll_event ev, events[EPOLL_MAX_EVENTS];
	int loop_timeout = pseudo_server_timeout;
	struct sigaction eat_usr2 = {
		.sa_handler = set_do_list_clients
	};

	clients = malloc(16 * sizeof(*clients));

	sigaction(SIGUSR2, &eat_usr2, NULL);

	clients[0].fd = listen_fd;
	clients[0].pid = getpid();

	for (i = 1; i < 16; ++i) {
		clients[i].fd = -1;
		clients[i].pid = 0;
		clients[i].tag = NULL;
		clients[i].program = NULL;
	}

	active_clients = 1;
	max_clients = 16;
	highest_client = 0;

	pseudo_debug(PDBGF_SERVER, "server loop started.\n");
	if (listen_fd < 0) {
		pseudo_diag("got into loop with no valid listen fd.\n");
		exit(PSEUDO_EXIT_LISTEN_FD);
	}

	timeout = LOOP_DELAY * 1000;

	int epollfd = epoll_create1(0);
	if (epollfd == -1) {
		pseudo_diag("epoll_create1() failed.\n");
		exit(PSEUDO_EXIT_EPOLL_CREATE);
	}
	ev.events = EPOLLIN;
	ev.data.u64 = 0;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clients[0].fd, &ev) == -1) {
		pseudo_diag("epoll_ctl() failed with listening socket.\n");
		exit(PSEUDO_EXIT_EPOLL_CTL);
	}

	pdb_log_msg(SEVERITY_INFO, NULL, NULL, NULL, "server started (pid %d)", getpid());

	for (;;) {
		rc = epoll_wait(epollfd, events, EPOLL_MAX_EVENTS, timeout);
		if (rc == 0 || (rc == -1 && errno == EINTR)) {
			/* If there's no clients, start timing out.  If there
			 * are active clients, never time out.
			 */
			if (active_clients == 1) {
				loop_timeout -= LOOP_DELAY;
				/* maybe flush database to disk */
				pdb_maybe_backup();
				if (loop_timeout <= 0) {
					pseudo_debug(PDBGF_SERVER, "no more clients, got bored.\n");
					die_peacefully = 1;
				} else {
					/* display this if not exiting */
					pseudo_debug(PDBGF_SERVER | PDBGF_BENCHMARK, "%d messages handled in %.4f seconds, %d responses\n",
						messages,
						(double) message_time.tv_sec +
						(double) message_time.tv_usec / 1000000.0,
						responses);
				}
			}
		} else if (rc > 0) {
			loop_timeout = pseudo_server_timeout;
			for (i = 0; i < rc; ++i) {
				int client_id = events[i].data.u64;
				if (clients[client_id].fd == listen_fd) {
					if (!die_forcefully) {
						len = sizeof(client);
						if ((fd = accept(listen_fd, (struct sockaddr *) &client, &len)) != -1) {
						/* Don't allow clients to end up on fd 2, because glibc's
						 * malloc debug uses that fd unconditionally.
						 */
							if (fd == 2) {
								int newfd = fcntl(fd, F_DUPFD, 3);
								close(fd);
								fd = newfd;
							}
							pseudo_debug(PDBGF_SERVER, "new client fd %d\n", fd);
							/* A new client implicitly cancels any
							 * previous shutdown request, or a
							 * shutdown for lack of clients.
							 */
							pseudo_server_timeout = DEFAULT_PSEUDO_SERVER_TIMEOUT;
							die_peacefully = 0;

							ev.events = EPOLLIN;
							ev.data.u64 = open_client(fd);
							if (ev.data.u64 != 0 && epoll_ctl(epollfd, EPOLL_CTL_ADD, clients[ev.data.u64].fd, &ev) == -1) {
								pseudo_diag("epoll_ctl() failed with accepted socket.\n");
								exit(PSEUDO_EXIT_EPOLL_CTL);
							}
						} else if (errno == EMFILE) {
							// select() loop would drop a client here, we do nothing (for now)
							pseudo_debug(PDBGF_SERVER, "Hit max open files.\n");
						}
					}
				} else {
					int n = 0;
					ioctl(clients[client_id].fd, FIONREAD, &n);
					if (n == 0) {
						close_client(client_id);
					} else {
						serve_client(client_id);
					}
				}
				if (die_forcefully)
					break;
			}
			pseudo_debug(PDBGF_SERVER, "server loop complete [%d clients left]\n", active_clients);
		} else {
			pseudo_diag("epoll_wait failed: %s\n", strerror(errno));
			break;
		}
		if (do_list_clients) {
			do_list_clients = 0;
			pseudo_diag("listing clients [1 through %d]:\n", highest_client);
			for (i = 1; i <= highest_client; ++i) {
				if (clients[i].fd == -1) {
					pseudo_diag("client %4d: inactive.\n", i);
					continue;
				}
				pseudo_diag("client %4d: fd %4d, pid %5d, program %s\n",
					i, clients[i].fd, clients[i].pid,
					clients[i].program ? clients[i].program : "<unspecified>");
			}
			pseudo_diag("done.\n");
		}
		if (die_peacefully || die_forcefully) {
			pseudo_debug(PDBGF_SERVER, "quitting.\n");
			pseudo_debug(PDBGF_SERVER | PDBGF_BENCHMARK, "server %d exiting: handled %d messages in %.4f seconds\n",
				getpid(), messages,
				(double) message_time.tv_sec +
				(double) message_time.tv_usec / 1000000.0);
			pdb_log_msg(SEVERITY_INFO, NULL, NULL, NULL, "server %d exiting: handled %d messages in %.4f seconds",
				getpid(), messages,
				(double) message_time.tv_sec +
				(double) message_time.tv_usec / 1000000.0);
			/* and at this point, we'll start refusing connections */
			close(clients[0].fd);
			/* This is a good place to insert a delay for
			 * debugging race conditions during startup. */
			/* usleep(300000); */
			exit(0);
		}
	}

}
#else

/* get clients, handle messages, shut down.
 * This doesn't actually do any work, it just calls a ton of things which
 * do work.
 */
static void
pseudo_server_loop(void) {
	struct sockaddr_un client;
	socklen_t len;
	fd_set reads, writes, events;
	int max_fd, current_clients;
	struct timeval timeout;
	int i;
	int rc;
	int fd;
	int loop_timeout = pseudo_server_timeout;
	struct sigaction eat_usr2 = {
		.sa_handler = set_do_list_clients
	};
	int hitmaxfiles;

	clients = malloc(16 * sizeof(*clients));

	sigaction(SIGUSR2, &eat_usr2, NULL);

	clients[0].fd = listen_fd;
	clients[0].pid = getpid();

	for (i = 1; i < 16; ++i) {
		clients[i].fd = -1;
		clients[i].pid = 0;
		clients[i].tag = NULL;
		clients[i].program = NULL;
	}

	active_clients = 1;
	max_clients = 16;
	highest_client = 0;
	hitmaxfiles = 0;

	pseudo_debug(PDBGF_SERVER, "server loop started.\n");
	if (listen_fd < 0) {
		pseudo_diag("got into loop with no valid listen fd.\n");
		exit(PSEUDO_EXIT_LISTEN_FD);
	}
	pdb_log_msg(SEVERITY_INFO, NULL, NULL, NULL, "server started (pid %d)", getpid());

	FD_ZERO(&reads);
	FD_ZERO(&events);
	FD_ZERO(&writes);
	FD_SET(clients[0].fd, &reads);
	FD_SET(clients[0].fd, &events);
	max_fd = clients[0].fd;
	timeout = (struct timeval) { .tv_sec = LOOP_DELAY, .tv_usec = 0 };
	
	/* EINTR tends to come from profiling, so it is not a good reason to
	 * exit; other signals are caught and set the flag causing a graceful
	 * exit. */
	sigset_t maskusr2;
	sigemptyset(&maskusr2);
	sigaddset(&maskusr2, SIGUSR2);
	sigprocmask(SIG_BLOCK, &maskusr2, NULL);
	while ((rc = select(max_fd + 1, &reads, &writes, &events, &timeout)) >= 0 || (errno == EINTR)) {
		sigprocmask(SIG_UNBLOCK, &maskusr2, NULL);
		if (rc == 0 || (rc == -1 && errno == EINTR)) {
			/* If there's no clients, start timing out.  If there
			 * are active clients, never time out.
			 */
			if (active_clients == 1) {
				loop_timeout -= LOOP_DELAY;
				/* maybe flush database to disk */
				pdb_maybe_backup();
				if (loop_timeout <= 0) {
					pseudo_debug(PDBGF_SERVER, "no more clients, got bored.\n");
					die_peacefully = 1;
				} else {
					/* display this if not exiting */
					pseudo_debug(PDBGF_SERVER | PDBGF_BENCHMARK, "%d messages handled in %.4f seconds, %d responses\n",
						messages,
						(double) message_time.tv_sec +
						(double) message_time.tv_usec / 1000000.0,
						responses);
				}
			}
		} else if (rc > 0) {
			loop_timeout = pseudo_server_timeout;
			for (i = 1; i <= highest_client; ++i) {
				if (clients[i].fd == -1) {
					continue;
				} else if (FD_ISSET(clients[i].fd, &reads)) {
					int n = 0;
					ioctl(clients[i].fd, FIONREAD, &n);
					if (n == 0) {
						close_client(i);
					} else {
						serve_client(i);
					}
				} else if (hitmaxfiles) {
					/* Only close one per loop iteration in the interests of caution */
					close_client(i);
					hitmaxfiles = 0;
				}
				if (die_forcefully)
					break;
			}
			hitmaxfiles = 0;
			if (!die_forcefully && 
			    (FD_ISSET(clients[0].fd, &events) ||
			     FD_ISSET(clients[0].fd, &reads))) {
				len = sizeof(client);
				if ((fd = accept(listen_fd, (struct sockaddr *) &client, &len)) != -1) {
					/* Don't allow clients to end up on fd 2, because glibc's
					 * malloc debug uses that fd unconditionally.
					 */
					if (fd == 2) {
						int newfd = fcntl(fd, F_DUPFD, 3);
						close(fd);
						fd = newfd;
					}
					pseudo_debug(PDBGF_SERVER, "new client fd %d\n", fd);
					open_client(fd);
					/* A new client implicitly cancels any
					 * previous shutdown request, or a
					 * shutdown for lack of clients.
					 */
					pseudo_server_timeout = DEFAULT_PSEUDO_SERVER_TIMEOUT;
					die_peacefully = 0;
				} else if (errno == EMFILE) {
					hitmaxfiles = 1;
					pseudo_debug(PDBGF_SERVER, "Hit max open files, dropping a client.\n");
				}
			}
			pseudo_debug(PDBGF_SERVER, "server loop complete [%d clients left]\n", active_clients);
		}
		if (do_list_clients) {
			do_list_clients = 0;
			pseudo_diag("listing clients [1 through %d]:\n", highest_client);
			for (i = 1; i <= highest_client; ++i) {
				if (clients[i].fd == -1) {
					pseudo_diag("client %4d: inactive.\n", i);
					continue;
				}
				pseudo_diag("client %4d: fd %4d, pid %5d, state %s, program %s\n",
					i, clients[i].fd, clients[i].pid,
					FD_ISSET(clients[i].fd, &reads) ? "R" : "-",
					clients[i].program ? clients[i].program : "<unspecified>");
			}
			pseudo_diag("done.\n");
		}
		if (die_peacefully || die_forcefully) {
			pseudo_debug(PDBGF_SERVER, "quitting.\n");
			pseudo_debug(PDBGF_SERVER | PDBGF_BENCHMARK, "server %d exiting: handled %d messages in %.4f seconds\n",
				getpid(), messages,
				(double) message_time.tv_sec +
				(double) message_time.tv_usec / 1000000.0);
			pdb_log_msg(SEVERITY_INFO, NULL, NULL, NULL, "server %d exiting: handled %d messages in %.4f seconds",
				getpid(), messages,
				(double) message_time.tv_sec +
				(double) message_time.tv_usec / 1000000.0);
			/* and at this point, we'll start refusing connections */
			close(clients[0].fd);
			/* This is a good place to insert a delay for
			 * debugging race conditions during startup. */
			/* usleep(300000); */
			exit(0);
		}
		FD_ZERO(&reads);
		FD_ZERO(&writes);
		FD_ZERO(&events);
		FD_SET(clients[0].fd, &reads);
		FD_SET(clients[0].fd, &events);
		max_fd = clients[0].fd;
		/* current_clients is a sanity check; note that for
		 * purposes of select(), the server is one of the fds,
		 * and thus, "a client".
		 */
		current_clients = 1;
		for (i = 1; i <= highest_client; ++i) {
			if (clients[i].fd != -1) {
				++current_clients;
				FD_SET(clients[i].fd, &reads);
				if (clients[i].fd > max_fd)
					max_fd = clients[i].fd;
			}
		}
		if (current_clients != active_clients) {
			pseudo_debug(PDBGF_SERVER, "miscount of current clients (%d) against active_clients (%d)?\n",
				current_clients, active_clients);
		}
		/* reinitialize timeout because Linux select alters it */
		timeout = (struct timeval) { .tv_sec = LOOP_DELAY, .tv_usec = 0 };
		sigprocmask(SIG_BLOCK, &maskusr2, NULL);
	}
	pseudo_diag("select failed: %s\n", strerror(errno));
}
#endif /* this is the else of #ifdef PSEUDO_EPOLL */
