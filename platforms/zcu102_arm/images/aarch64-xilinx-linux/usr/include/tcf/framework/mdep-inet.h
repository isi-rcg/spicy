/*******************************************************************************
 * Copyright (c) 2007-2020 Wind River Systems, Inc. and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 * The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 * http://www.eclipse.org/org/documents/edl-v10.php.
 * You may elect to redistribute this code under either of these licenses.
 *
 * Contributors:
 *     Wind River Systems - initial API and implementation
 *     Nokia - Symbian support
 *******************************************************************************/

/*
 * Machine and OS dependent definitions for networking.
 */

#ifndef D_mdep_inet
#define D_mdep_inet

#if defined(_WIN32) || defined(__CYGWIN__)

#include <ws2tcpip.h>
#include <iphlpapi.h>

#if defined(__CYGWIN__)

#include <cygwin/version.h>

#if CYGWIN_VERSION_CYGWIN_CONV < 157

extern void __stdcall freeaddrinfo(struct addrinfo *);
extern int __stdcall getaddrinfo(const char *, const char *,
                const struct addrinfo *, struct addrinfo **);
#endif

#endif

extern const char * loc_gai_strerror(int ecode);

#define MSG_MORE 0

/* inet_ntop()/inet_pton() are not available in MinGW */
/* inet_ntop()/inet_pton() are not available in Windows before Windows Vista */
#if defined(__MINGW32__) || (defined(_WIN32_WINNT) && _WIN32_WINNT < 0x0600)
extern const char * inet_ntop(int af, const void * src, char * dst, socklen_t size);
extern int inet_pton(int af, const char * src, void * dst);
#endif

/*
 * Windows socket functions don't set errno as expected.
 * Wrappers are provided to workaround the problem.
 */
#define socket(af, type, protocol) wsa_socket(af, type, protocol)
#define connect(socket, addr, addr_size) wsa_connect(socket, addr, addr_size)
#define accept(socket, addr, addr_size) wsa_accept(socket, addr, addr_size)
#define bind(socket, addr, addr_size) wsa_bind(socket, addr, addr_size)
#define listen(socket, size) wsa_listen(socket, size)
#define recv(socket, buf, size, flags) wsa_recv(socket, buf, size, flags)
#define recvfrom(socket, buf, size, flags, addr, addr_size) wsa_recvfrom(socket, buf, size, flags, addr, addr_size)
#define send(socket, buf, size, flags) wsa_send(socket, buf, size, flags)
#define sendto(socket, buf, size, flags, dest_addr, dest_size) wsa_sendto(socket, buf, size, flags, dest_addr, dest_size)
#define setsockopt(socket, level, opt, value, size) wsa_setsockopt(socket, level, opt, value, size)
#define getsockopt(socket, level, opt, value, size) wsa_getsockopt(socket, level, opt, value, size)
#define getsockname(socket, name, size) wsa_getsockname(socket, name, size)
#define select(nfds, readfds, writefds, exceptfds, timeout) wsa_select(nfds, readfds, writefds, exceptfds, timeout)
#define ioctlsocket(socket, cmd, args) wsa_ioctlsocket(socket, cmd, args)
#define shutdown(socket, how) wsa_shutdown(socket, how)
#define closesocket(socket) wsa_closesocket(socket)

extern int wsa_socket(int af, int type, int protocol);
extern int wsa_connect(int socket, const struct sockaddr * addr, int addr_size);
extern int wsa_accept(int socket, struct sockaddr * addr, int * addr_size);
extern int wsa_bind(int socket, const struct sockaddr * addr, int addr_size);
extern int wsa_listen(int socket, int size);
extern int wsa_recv(int socket, void * buf, size_t size, int flags);
extern int wsa_recvfrom(int socket, void * buf, size_t size, int flags,
                  struct sockaddr * addr, socklen_t * addr_size);
extern int wsa_send(int socket, const void * buf, size_t size, int flags);
extern int wsa_sendto(int socket, const void * buf, size_t size, int flags,
                  const struct sockaddr * dest_addr, socklen_t dest_size);
extern int wsa_setsockopt(int socket, int level, int opt, const char * value, int size);
extern int wsa_getsockopt(int socket, int level, int opt, char * value, int * size);
extern int wsa_getsockname(int socket, struct sockaddr * name, int * size);
extern int wsa_select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, const struct timeval * timeout);
extern int wsa_ioctlsocket(int socket, long cmd, unsigned long * args);
extern int wsa_shutdown(int socket, int how);
extern int wsa_closesocket(int socket);

#ifndef SHUT_WR
#  define SHUT_WR SD_SEND
#endif
#ifndef SHUT_RDWR
#  define SHUT_RDWR SD_BOTH
#endif

#if !defined(SOCK_MAXADDRLEN) && defined(_SS_MAXSIZE)
#  define SOCK_MAXADDRLEN _SS_MAXSIZE
#endif

#define loc_freeaddrinfo freeaddrinfo
#define loc_getaddrinfo getaddrinfo

#elif defined(_WRS_KERNEL)

#include <inetLib.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <wrn/coreip/sockLib.h>
#include <wrn/coreip/hostLib.h>

#define closesocket close
#define ifr_netmask ifr_addr
#define SA_LEN(addr) ((addr)->sa_len)
#define MSG_MORE 0


extern void loc_freeaddrinfo(struct addrinfo * ai);
extern int loc_getaddrinfo(const char * nodename, const char * servname,
       const struct addrinfo * hints, struct addrinfo ** res);
extern const char * loc_gai_strerror(int ecode);

#elif defined __SYMBIAN32__

#include <in.h>
#include <netdb.h>
#include <inet.h>
#include <sys/sockio.h>
#include <sys/un.h>
#include <net/if.h>

#define closesocket close

extern const char * loc_gai_strerror(int ecode);
#define loc_freeaddrinfo freeaddrinfo
#define loc_getaddrinfo getaddrinfo

struct ip_ifc_info;
extern void set_ip_ifc(struct ip_ifc_info * info);
extern struct ip_ifc_info * get_ip_ifc(void);

#else

#include <netdb.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <net/if.h>
#if defined (__sun__)
#include <sys/ioctl.h>
#include <sys/sockio.h>
#endif

#define loc_freeaddrinfo freeaddrinfo
#define loc_getaddrinfo getaddrinfo
#define loc_gai_strerror gai_strerror

#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__APPLE__)
#  define SA_LEN(addr) ((addr)->sa_len)
#else /* not BSD */
#  define SA_LEN(addr) (sizeof(struct sockaddr))
#endif /* BSD */

#define closesocket close

#endif

#endif /* D_mdep_inet */
