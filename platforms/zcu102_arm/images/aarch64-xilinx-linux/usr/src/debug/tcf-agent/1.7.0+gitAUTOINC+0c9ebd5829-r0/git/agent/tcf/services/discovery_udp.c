/*******************************************************************************
 * Copyright (c) 2007, 2017 Wind River Systems, Inc. and others.
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
 *******************************************************************************/

/*
 * Implements UDP based service discovery.
 *
 * The discovery protocol uses unicast and multicast UDP packets to propagate information
 * about available TCF peers. The protocol is truly distributed - all participants have
 * same functionality and no central authority is defined.
 *
 * TCF discovery scope is one subnet. Access across subnets is supported by TCF proxy.
 *
 * TCF discovery participants use a dedicated UDP port - 1534, however discovery will
 * work fine if the port is not available for some participants, but at least one
 * participant on a subnet must be able to bind itself to the default port, otherwise the protocol
 * will not function properly. An agent that owns a default port is called "master",
 * an agent that owns non-default port is called "slave".
 *
 * Every slave will check periodically availability of default port, and can become a master if
 * the port becomes available.
 *
 * Since slaves cannot receive multicast packets, each agent maintains a list of slaves,
 * and uses unicast packets to sent info to agents from the list.
 */

#include <tcf/config.h>

#if ENABLE_Discovery

#include <stddef.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <tcf/framework/mdep-inet.h>
#include <tcf/framework/tcf.h>
#include <tcf/framework/myalloc.h>
#include <tcf/framework/events.h>
#include <tcf/framework/errors.h>
#include <tcf/framework/trace.h>
#include <tcf/framework/peer.h>
#include <tcf/framework/ip_ifc.h>
#include <tcf/framework/asyncreq.h>
#include <tcf/services/discovery.h>
#include <tcf/services/discovery_udp.h>

#define MAX_IFC                 10
#define MAX_RECV_ERRORS         8

static int ifc_cnt;
static ip_ifc_info ifc_list[MAX_IFC];
static time_t last_req_slaves_time[MAX_IFC];
static int send_all_ok[MAX_IFC];

static uint16_t udp_server_port = 0;
static int udp_server_socket = -1;
static int udp_server_generation = 0;

static AsyncReqInfo recvreq;
static int recvreq_pending = 0;
static int recvreq_error_cnt = 0;
static int recvreq_generation = 0;
static struct sockaddr_in recvreq_addr;

static char recv_buf[MAX_PACKET_SIZE];
static char send_buf[MAX_PACKET_SIZE];
static int send_size;

static time_t last_master_packet_time = 0;

static int discovery_stopped = 0;

typedef struct SlaveInfo {
    struct sockaddr_in addr;
    time_t last_packet_time;        /* Time of last packet from this slave */
    time_t last_req_slaves_time;    /* Time of last UDP_REQ_SLAVES packet from this slave */
} SlaveInfo;

static SlaveInfo * slave_info = NULL;
static int slave_cnt = 0;
static int slave_max = 0;

static void app_char(char ch) {
    if (send_size < (int)sizeof(send_buf)) {
        send_buf[send_size++] = ch;
    }
}

static void app_str(const char * str) {
    while (*str && send_size < (int)sizeof(send_buf)) {
        send_buf[send_size++] = *str++;
    }
}

static void app_strz(const char * str) {
    app_str(str);
    app_char(0);
}

static int get_slave_addr(char * buf, ssize_t * pos, struct sockaddr_in * addr, uint64_t * timestamp) {
    char * port = buf + *pos;
    char * stmp = buf + *pos;
    char * host;
    size_t len = strlen(buf + *pos);
    uint64_t ts = 0;
    int n;

    while (*port && *port != ':') port++;
    if (*port == ':') *port++ = 0;

    host = port;
    while (*host && *host != ':') host++;
    if (*host == ':') *host++ = 0;

    *pos += len + 1;

    memset(addr, 0, sizeof(*addr));
    addr->sin_family = AF_INET;
    if (inet_pton(AF_INET, host, &addr->sin_addr) <= 0) return 0;
    n = atoi(port);
    if (n == DISCOVERY_TCF_PORT) return 0;
    addr->sin_port = htons((unsigned short)n);
    while (*stmp >= '0' && *stmp <= '9') {
        ts = (ts * 10) + (*stmp++ - '0');
    }
    *timestamp = ts;
    return 1;
}

static void trigger_recv(void);
static void udp_server_recv(void * x);

static void delayed_server_recv(void * x) {
    assert(recvreq_pending);
    if (recvreq_generation != udp_server_generation) {
        /* Cancel and restart */
        recvreq_pending = 0;
        trigger_recv();
    }
    else {
        async_req_post(&recvreq);
    }
}

static void trigger_recv(void) {
    if (recvreq_pending || udp_server_socket < 0) return;
    recvreq_pending = 1;
    recvreq_generation = udp_server_generation;
    recvreq.done = udp_server_recv;
    recvreq.client_data = NULL;
    recvreq.type = AsyncReqRecvFrom;
    recvreq.u.sio.sock = udp_server_socket;
    recvreq.u.sio.flags = 0;
    recvreq.u.sio.bufp = recv_buf;
    recvreq.u.sio.bufsz = sizeof recv_buf;
    recvreq.u.sio.addr = (struct sockaddr *)&recvreq_addr;
    recvreq.u.sio.addrlen = sizeof recvreq_addr;
    memset(&recvreq_addr, 0, sizeof recvreq_addr);
    if (recvreq_error_cnt >= MAX_RECV_ERRORS) {
        /* Delay the request to avoid flooding with error reports */
        trace(LOG_ALWAYS, "delayed_server_recv error occured: %d", recvreq_error_cnt);
        post_event_with_delay(delayed_server_recv, NULL, 1000000);
    }
    else {
        async_req_post(&recvreq);
    }
}

static int create_server_socket(void) {
    int sock = -1;
    int error;
    const char * reason = NULL;
    const int i = 1;
    struct addrinfo hints;
    struct addrinfo * reslist = NULL;
    struct addrinfo * res;
    struct sockaddr_in local_addr;
#if defined(_WRS_KERNEL)
    int local_addr_size = sizeof(local_addr);
#else
    socklen_t local_addr_size = sizeof(local_addr);
#endif
    char port_str[16];

    sprintf(port_str, "%d", DISCOVERY_TCF_PORT);
    memset(&local_addr, 0, sizeof(local_addr));
    memset(&hints, 0, sizeof hints);
    hints.ai_family = PF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_PASSIVE;
    error = loc_getaddrinfo(NULL, port_str, &hints, &reslist);
    if (error) {
        trace(LOG_ALWAYS, "getaddrinfo error: %s", loc_gai_strerror(error));
        return set_gai_errno(error);
    }
    for (res = reslist; res != NULL; res = res->ai_next) {
        sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock < 0) {
            error = errno;
            reason = "create";
            continue;
        }
        if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char *)&i, sizeof(i)) < 0) {
            error = errno;
            reason = "setsockopt(SO_BROADCAST)";
            closesocket(sock);
            sock = -1;
            continue;
        }
        if (bind(sock, res->ai_addr, res->ai_addrlen)) {
            error = errno;
            if (res->ai_addr->sa_family == AF_INET) {
                struct sockaddr_in addr;
                trace(LOG_DISCOVERY, "Cannot bind to default UDP port %d: %s",
                    DISCOVERY_TCF_PORT, errno_to_str(error));
                assert(sizeof(addr) >= res->ai_addrlen);
                memset(&addr, 0, sizeof(addr));
                memcpy(&addr, res->ai_addr, res->ai_addrlen);
                addr.sin_port = 0;
                error = 0;
                if (bind(sock, (struct sockaddr *)&addr, sizeof(addr))) {
                    error = errno;
                    if (udp_server_socket >= 0 && recvreq_error_cnt < MAX_RECV_ERRORS) {
                        loc_freeaddrinfo(reslist);
                        closesocket(sock);
                        return 0;
                    }
                }
            }
            if (error) {
                reason = "bind";
                closesocket(sock);
                sock = -1;
                continue;
            }
        }
        if (getsockname(sock, (struct sockaddr *)&local_addr, &local_addr_size)) {
            error = errno;
            reason = "getsockname";
            closesocket(sock);
            sock = -1;
            continue;
        }
        /* Only bind once - don't see how getaddrinfo with the given
         * arguments could return more then one anyway */
        break;
    }
    if (sock < 0) {
        assert(error);
        loc_freeaddrinfo(reslist);
        if (udp_server_socket >= 0 && recvreq_error_cnt < MAX_RECV_ERRORS) {
            return 0;
        }
        trace(LOG_ALWAYS, "Discovery service socket %s error: %s",
            reason, errno_to_str(error));
        return set_fmt_errno(error, "Socket %s error", reason);
    }

    if (udp_server_socket >= 0) closesocket(udp_server_socket);
    udp_server_port = ntohs(local_addr.sin_port);
    udp_server_socket = sock;
    udp_server_generation++;
    loc_freeaddrinfo(reslist);
    trace(LOG_DISCOVERY, "UDP discovery server created at port %d", (int)udp_server_port);
    trigger_recv();
    return 0;
}

static int send_packet(ip_ifc_info * ifc, struct sockaddr_in * addr) {
    if (addr == NULL) {
        /* Broadcast */
        int n = 0;
        static struct sockaddr_in buf;

        /* Send to all slaves */
        while (n < slave_cnt) {
            SlaveInfo * s = slave_info + n++;
            send_packet(ifc, &s->addr);
        }

        /* Send to all masters by using interface broadcast address */
        memset(&buf, 0, sizeof(buf));
        addr = &buf;
        addr->sin_family = AF_INET;
        addr->sin_port = htons(DISCOVERY_TCF_PORT);
        addr->sin_addr.s_addr = ifc->addr;
        if (*(uint8_t *)&ifc->addr != 127) addr->sin_addr.s_addr |= ~ifc->mask;
    }

    /* Don't send if address does not belong to subnet of the interface */
    if ((ifc->addr & ifc->mask) != (addr->sin_addr.s_addr & ifc->mask)) return 0;

    /* Don't send to ourselves */
    if (ifc->addr == addr->sin_addr.s_addr && udp_server_port == ntohs(addr->sin_port)) return 0;

#if ENABLE_Trace
    if (log_file != NULL && (log_mode & LOG_DISCOVERY) != 0) {
        int i;
        char buf[sizeof(send_buf) + 32];
        size_t pos;
        char ch;
        switch (send_buf[4]) {
        case UDP_ACK_INFO:
            pos = strlcpy(buf, "ACK_INFO", sizeof(buf));
            i = 8;
            while (i < send_size) {
                if (strncmp(send_buf + i, "ID=", 3) == 0) {
                    if (pos < sizeof(buf) - 1) buf[pos++] = ' ';
                    while (i < send_size && (ch = send_buf[i++]) != 0) {
                        if (pos < sizeof(buf) - 1) buf[pos++] = ch;
                    }
                    break;
                }
                else {
                    while (i < send_size && send_buf[i++]) {}
                }
            }
            break;
        case UDP_ACK_SLAVES:
            pos = strlcpy(buf, "ACK_SLAVES", sizeof(buf));
            i = 8;
            while (i < send_size) {
                if (pos < sizeof(buf) - 1) buf[pos++] = ' ';
                while (i < send_size && (ch = send_buf[i++]) != 0) {
                    if (pos < sizeof(buf) - 1) buf[pos++] = ch;
                }
            }
            break;
        case UDP_REQ_INFO:
            pos = strlcpy(buf, "REQ_INFO", sizeof(buf));
            break;
        case UDP_REQ_SLAVES:
            pos = strlcpy(buf, "REQ_SLAVES", sizeof(buf));
            break;
        case UDP_PEERS_REMOVED:
            pos = strlcpy(buf, "PEERS_REMOVED", sizeof(buf));
            i = 8;
            while (i < send_size) {
                if (pos < sizeof(buf) - 1) buf[pos++] = ' ';
                while (i < send_size && (ch = send_buf[i++]) != 0) {
                    if (pos < sizeof(buf) - 1) buf[pos++] = ch;
                }
            }
            break;
        default:
            pos = strlcpy(buf, "???", sizeof(buf));
            break;
        }
        buf[pos] = 0;
        trace(LOG_DISCOVERY, "%s to %s:%d", buf, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
    }
#endif

    if (sendto(udp_server_socket, send_buf, send_size, 0, (struct sockaddr *)addr, sizeof(*addr)) >= 0) return 1;

    trace(LOG_ALWAYS, "Can't send UDP discovery packet to %s:%d %s",
          inet_ntoa(addr->sin_addr), ntohs(addr->sin_port), errno_to_str(errno));
    return 0;
}

static int is_ip_peer(PeerServer * ps) {
    const char * prot = peer_server_getprop(ps, "TransportName", NULL);
    return prot != NULL && (strcmp(prot, "TCP") == 0 || strcmp(prot, "SSL") == 0 ||
        strcmp(prot, "WS") == 0 || strcmp(prot, "WSS") == 0 || strcmp(prot, "HTTP") == 0);
}

static int udp_send_peer_info(PeerServer * ps, void * arg) {
    struct sockaddr_in * addr = (struct sockaddr_in *)arg;
    const char * host = NULL;
    struct in_addr peer_addr;
    int n;

    if ((ps->flags & PS_FLAG_PRIVATE) != 0) return 0;
    if ((ps->flags & PS_FLAG_DISCOVERABLE) == 0) return 0;

    memset(&peer_addr, 0, sizeof(peer_addr));
    if (is_ip_peer(ps)) {
        host = peer_server_getprop(ps, "Host", NULL);
        if (host == NULL || inet_pton(AF_INET, host, &peer_addr) <= 0) return 0;
    }

    send_size = 8;

    for (n = 0; n < ifc_cnt; n++) {
        ip_ifc_info * ifc = ifc_list + n;

        if ((ps->flags & PS_FLAG_LOCAL) == 0) {
            /* Info about non-local peers is sent only by master */
            if (udp_server_port != DISCOVERY_TCF_PORT) return 0;
            if (host == NULL) return 0;
            if (ifc->addr != htonl(INADDR_LOOPBACK) && ifc->addr != peer_addr.s_addr) continue;
        }

        if (ifc->addr != htonl(INADDR_LOOPBACK)) {
            if (host == NULL) continue;
            assert(peer_addr.s_addr != INADDR_ANY);
            if ((ifc->addr & ifc->mask) != (peer_addr.s_addr & ifc->mask)) {
                /* Peer address does not belong to subnet of this interface */
                continue;
            }
        }

        if (send_size == 8) {
            unsigned i;
            send_buf[4] = UDP_ACK_INFO;
            app_str("ID=");
            app_strz(ps->id);
            for (i = 0; i < ps->ind; i++) {
                const char * name = ps->list[i].name;
                assert(strcmp(name, "ID") != 0);
                app_str(name);
                app_char('=');
                app_strz(ps->list[i].value);
            }
        }

        send_all_ok[n] = 1;
        send_packet(ifc, addr);
    }
    return 0;
}

static void udp_send_ack_info(struct sockaddr_in * addr) {
    assert(is_dispatch_thread());
    peer_server_iter(udp_send_peer_info, addr);
}

static void udp_send_req_info(struct sockaddr_in * addr) {
    int n;
    for (n = 0; n < ifc_cnt; n++) {
        ip_ifc_info * ifc = ifc_list + n;

        send_size = 8;
        send_buf[4] = UDP_REQ_INFO;
        send_packet(ifc, addr);
    }
}

static void udp_send_empty_packet(struct sockaddr_in * addr) {
    int n;
    for (n = 0; n < ifc_cnt; n++) {
        ip_ifc_info * ifc = ifc_list + n;

        if (send_all_ok[n]) continue;

        send_size = 8;
        send_buf[4] = UDP_ACK_SLAVES;
        send_packet(ifc, addr);
    }
}

static void udp_send_req_slaves(ip_ifc_info * ifc, struct sockaddr_in * addr) {
    send_size = 8;
    send_buf[4] = UDP_REQ_SLAVES;
    send_packet(ifc, addr);
}

static void udp_send_ack_slaves_one(SlaveInfo * s) {
    ip_ifc_info * ifc;
    time_t timenow = time(NULL);
    int ttl = (int)(s->last_packet_time + PEER_DATA_RETENTION_PERIOD - timenow) * 1000;

    if (ttl <= 0) return;

    for (ifc = ifc_list; ifc < &ifc_list[ifc_cnt]; ifc++) {
        int n = 0;
        char str[256];
        if ((ifc->addr & ifc->mask) != (s->addr.sin_addr.s_addr & ifc->mask)) continue;

        send_size = 8;
        send_buf[4] = UDP_ACK_SLAVES;
        snprintf(str, sizeof(str), "%u:%u:%s", ttl, ntohs(s->addr.sin_port), inet_ntoa(s->addr.sin_addr));
        app_strz(str);

        while (n < slave_cnt) {
            SlaveInfo * sn = slave_info + n++;
            if (sn->last_req_slaves_time + PEER_DATA_RETENTION_PERIOD < timenow) continue;
            send_packet(ifc, &sn->addr);
        }
    }
}

static void udp_send_ack_slaves_all(struct sockaddr_in * addr, time_t timenow) {
    int k;

    for (k = 0; k < ifc_cnt; k++) {
        int n = 0;
        ip_ifc_info * ifc = ifc_list + k;

        if ((ifc->addr & ifc->mask) != (addr->sin_addr.s_addr & ifc->mask)) continue;

        send_size = 8;
        send_buf[4] = UDP_ACK_SLAVES;

        while (n < slave_cnt) {
            char str[256];
            SlaveInfo * s = slave_info + n++;
            int ttl = (int)(s->last_packet_time + PEER_DATA_RETENTION_PERIOD - timenow) * 1000;
            if (ttl <= 0) continue;
            if (addr->sin_addr.s_addr == s->addr.sin_addr.s_addr && addr->sin_port == s->addr.sin_port) continue;
            if (ifc->addr != htonl(INADDR_LOOPBACK)) {
                if ((ifc->addr & ifc->mask) != (s->addr.sin_addr.s_addr & ifc->mask)) {
                    /* Slave address does not belong to subnet of this interface */
                    continue;
                }
            }
            snprintf(str, sizeof(str), "%u:%u:%s", ttl, ntohs(s->addr.sin_port), inet_ntoa(s->addr.sin_addr));
            if (send_size + strlen(str) >= PREF_PACKET_SIZE) {
                send_packet(ifc, addr);
                send_size = 8;
            }
            app_strz(str);
            send_all_ok[k] = 1;
        }

        if (send_size > 8) send_packet(ifc, addr);
    }
}

static int add_peer_id(PeerServer * ps, void * arg) {
    ip_ifc_info * ifc = (ip_ifc_info *)arg;
    struct in_addr peer_addr;
    const char * host = NULL;

    if ((ps->flags & PS_FLAG_PRIVATE) != 0) return 0;
    if ((ps->flags & PS_FLAG_DISCOVERABLE) == 0) return 0;
    if ((ps->flags & PS_FLAG_LOCAL) == 0) return 0;

    memset(&peer_addr, 0, sizeof(peer_addr));
    if (is_ip_peer(ps)) {
        host = peer_server_getprop(ps, "Host", NULL);
        if (host == NULL || inet_pton(AF_INET, host, &peer_addr) <= 0) return 0;
    }

    if (ifc->addr != htonl(INADDR_LOOPBACK)) {
        if (host == NULL) return 0;
        assert(peer_addr.s_addr != INADDR_ANY);
        if ((ifc->addr & ifc->mask) != (peer_addr.s_addr & ifc->mask)) {
            /* Peer address does not belong to subnet of this interface */
            return 0;
        }
    }

    app_strz(ps->id);
    return 0;
}

static void udp_send_peer_removed(void) {
    int n;
    for (n = 0; n < ifc_cnt; n++) {
        ip_ifc_info * ifc = ifc_list + n;
        send_size = 8;
        send_buf[4] = UDP_PEERS_REMOVED;
        peer_server_iter(add_peer_id, ifc);
        send_packet(ifc, NULL);
    }
}

static void udp_send_all(struct sockaddr_in * addr, SlaveInfo * s) {
    memset(send_all_ok, 0, sizeof(send_all_ok));
    udp_send_ack_info(addr);
    if (addr != NULL && s != NULL) {
        time_t timenow = time(NULL);
        if (s->last_req_slaves_time + PEER_DATA_RETENTION_PERIOD >= timenow) {
            udp_send_ack_slaves_all(addr, timenow);
        }
    }
    udp_send_empty_packet(addr);
}

static SlaveInfo * add_slave(struct sockaddr_in * addr, time_t timestamp) {
    int i = 0;
    SlaveInfo * s;
    while (i < slave_cnt) {
        s = slave_info + i++;
        if (memcmp(&s->addr, addr, sizeof(struct sockaddr_in)) == 0) {
            if (s->last_packet_time < timestamp) s->last_packet_time = timestamp;
            return s;
        }
    }
    if (slave_max == 0) {
        assert(slave_cnt == 0);
        slave_max = 16;
        slave_info = (SlaveInfo *)loc_alloc(sizeof(SlaveInfo) * slave_max);
    }
    else if (slave_cnt >= slave_max) {
        assert(slave_cnt == slave_max);
        slave_max *= 2;
        slave_info = (SlaveInfo *)loc_realloc(slave_info, sizeof(SlaveInfo) * slave_max);
    }
    s = slave_info + slave_cnt++;
    s->addr = *addr;
    s->last_packet_time = timestamp;
    s->last_req_slaves_time = 0;
    udp_send_req_info(addr);
    udp_send_all(addr, s);
    udp_send_ack_slaves_one(s);
    return s;
}

static void udp_refresh_timer(void * arg) {
    time_t timenow = time(NULL);

    if (discovery_stopped) return;

    if (slave_cnt > 0) {
        /* Cleanup slave table */
        int i = 0;
        int j = 0;
        while (i < slave_cnt) {
            SlaveInfo * s = slave_info + i++;
            if (s->last_packet_time + PEER_DATA_RETENTION_PERIOD >= timenow) {
                if (j < i) slave_info[j] = *s;
                j++;
            }
        }
        slave_cnt = j;
    }

    if (udp_server_port != DISCOVERY_TCF_PORT && last_master_packet_time + PEER_DATA_RETENTION_PERIOD / 2 <= timenow) {
        /* No master responses, try to become a master */
        create_server_socket();
    }

    /* Refresh list of network interfaces */
    ifc_cnt = build_ifclist(udp_server_socket, MAX_IFC, ifc_list);

    if (udp_server_port != DISCOVERY_TCF_PORT) {
        int i;
        for (i = 0; i < ifc_cnt; i++) {
            struct sockaddr_in addr;
            memset(&addr, 0, sizeof(addr));
            addr.sin_family = AF_INET;
            addr.sin_port = htons(udp_server_port);
            addr.sin_addr.s_addr = ifc_list[i].addr;
            add_slave(&addr, timenow);
        }
    }

    /* Broadcast peer info */
    udp_send_all(NULL, NULL);

    post_event_with_delay(udp_refresh_timer, NULL, PEER_DATA_REFRESH_PERIOD * 1000000);
}

static int is_remote(struct sockaddr_in * addr) {
    int i;

    if (ntohs(addr->sin_port) != udp_server_port) return 1;
    for (i = 0; i < ifc_cnt; i++) {
        if (addr->sin_addr.s_addr == ifc_list[i].addr) return 0;
    }
    return 1;
}

static void udp_receive_req_info(SlaveInfo * s) {
    trace(LOG_DISCOVERY, "REQ_INFO from %s:%d",
        inet_ntoa(recvreq_addr.sin_addr), ntohs(recvreq_addr.sin_port));
    udp_send_all(&recvreq_addr, s);
}

static void udp_receive_ack_info(void) {
    PeerServer * ps = peer_server_alloc();
    char * p = recv_buf + 8;
    char * e = recv_buf + recvreq.u.sio.rval;

    assert(is_dispatch_thread());
    while (p < e) {
        char * name = p;
        char * value;
        while (p < e && *p != '\0' && *p != '=') p++;
        if (p >= e || *p != '=') {
            p = NULL;
            break;
        }
        *p++ = '\0';
        value = p;
        while (p < e && *p != '\0') p++;
        if (p >= e) {
            p = NULL;
            break;
        }
        peer_server_addprop(ps, loc_strdup(name), loc_strdup(value));
        p++;
    }
    if (p != NULL && ps->id != NULL && is_ip_peer(ps)) {
        const char * host = peer_server_getprop(ps, "Host", NULL);
        if (host != NULL) {
            struct in_addr peer_addr;
            memset(&peer_addr, 0, sizeof(peer_addr));
            if (inet_pton(AF_INET, host, &peer_addr) > 0) {
                int n;
                for (n = 0; n < ifc_cnt; n++) {
                    ip_ifc_info * ifc = ifc_list + n;
                    if ((ifc->addr & ifc->mask) == (peer_addr.s_addr & ifc->mask)) {
                        trace(LOG_DISCOVERY, "ACK_INFO from %s:%d, ID=%s",
                            inet_ntoa(recvreq_addr.sin_addr), ntohs(recvreq_addr.sin_port), ps->id);
                        ps->flags = PS_FLAG_DISCOVERABLE;
                        peer_server_add(ps, PEER_DATA_RETENTION_PERIOD);
                        return;
                    }
                }
            }
        }
    }
    trace(LOG_DISCOVERY, "Received malformed ACK_INFO from %s:%d",
        inet_ntoa(recvreq_addr.sin_addr), ntohs(recvreq_addr.sin_port));
    peer_server_free(ps);
}

static void udp_receive_req_slaves(SlaveInfo * s, time_t timenow) {
    trace(LOG_DISCOVERY, "REQ_SLAVES from %s:%d",
        inet_ntoa(recvreq_addr.sin_addr), ntohs(recvreq_addr.sin_port));
    if (s != NULL) s->last_req_slaves_time = timenow;
    udp_send_ack_slaves_all(&recvreq_addr, timenow);
}

static void udp_receive_ack_slaves(time_t timenow) {
    ssize_t pos = 8;
    ssize_t len = recvreq.u.sio.rval;
    while (pos < len) {
        struct sockaddr_in addr;
        uint64_t timestamp;
        if (get_slave_addr(recv_buf, &pos, &addr, &timestamp)) {
            time_t delta = 60 * 10; /* 10 minutes */
            time_t timeval;
            if (timestamp < 3600000) {
                /* Timestamp is "time to live" in milliseconds */
                timeval = timenow + (time_t)(timestamp / 1000) - PEER_DATA_RETENTION_PERIOD;
            }
            else if (timestamp < (uint64_t)timenow + 50000000) {
                /* Timestamp is in seconds */
                timeval = (time_t)timestamp;
            }
            else {
                /* Timestamp is in milliseconds */
                timeval = (time_t)(timestamp / 1000);
            }
            if (log_mode & LOG_DISCOVERY) {
                char buf[64];
                snprintf(buf, sizeof(buf), "%s:%d", inet_ntoa(recvreq_addr.sin_addr), ntohs(recvreq_addr.sin_port));
                trace(LOG_DISCOVERY, "ACK_SLAVES %" PRId64 ":%u:%s from %s",
                    timestamp, ntohs(addr.sin_port), inet_ntoa(addr.sin_addr), buf);
            }
            if (timeval < timenow - delta || timeval > timenow + delta) {
                trace(LOG_DISCOVERY, "Discovery: invalid slave info timestamp %" PRId64 " from %s:%d",
                    timestamp, inet_ntoa(recvreq_addr.sin_addr), ntohs(recvreq_addr.sin_port));
                timeval = timenow - PEER_DATA_RETENTION_PERIOD / 2;
            }
            add_slave(&addr, timeval);
        }
    }
}

static void udp_receive_peer_removed(void) {
    char * p = recv_buf + 8;
    char * e = recv_buf + recvreq.u.sio.rval;

    assert(is_dispatch_thread());
    while (p < e) {
        char * id = p;
        while (p < e && *p != '\0') p++;
        if (p < e) {
            PeerServer * peer = peer_server_find(id);
            if (peer != NULL && (peer->flags & PS_FLAG_LOCAL) == 0) {
                peer_server_remove(id);
            }
            while (p < e && *p == '\0') p++;
        }
    }
}

static void udp_server_recv(void * x) {
    assert(recvreq_pending != 0);
    assert(x == &recvreq);
    if (discovery_stopped) {
        if (udp_server_socket >= 0) {
            closesocket(udp_server_socket);
            udp_server_socket = -1;
        }
        return;
    }
    recvreq_pending = 0;
    if (recvreq.error != 0) {
        if (recvreq_generation != udp_server_generation) {
            recvreq_error_cnt = 0;
        }
        else {
            recvreq_error_cnt++;
            trace(LOG_ALWAYS, "UDP socket receive failed: %s", errno_to_str(recvreq.error));
        }
    }
    else {
        recvreq_error_cnt = 0;
        if (recvreq.u.sio.rval >= 8 &&
                recv_buf[0] == 'T' &&
                recv_buf[1] == 'C' &&
                recv_buf[2] == 'F' &&
                recv_buf[3] == UDP_VERSION &&
                is_remote(&recvreq_addr)) {
            if (recv_buf[4] == UDP_PEERS_REMOVED) {
                udp_receive_peer_removed();
            }
            else {
                int n;
                time_t timenow = time(NULL);
                SlaveInfo * s = NULL;
                if (ntohs(recvreq_addr.sin_port) != DISCOVERY_TCF_PORT) {
                    /* Packet from a slave, save its address */
                    s = add_slave(&recvreq_addr, timenow);
                }
                switch (recv_buf[4]) {
                case UDP_REQ_INFO:
                    udp_receive_req_info(s);
                    break;
                case UDP_ACK_INFO:
                    udp_receive_ack_info();
                    break;
                case UDP_REQ_SLAVES:
                    udp_receive_req_slaves(s, timenow);
                    break;
                case UDP_ACK_SLAVES:
                    udp_receive_ack_slaves(timenow);
                    break;
                }
                for (n = 0; n < ifc_cnt; n++) {
                    ip_ifc_info * ifc = ifc_list + n;
                    if ((ifc->addr & ifc->mask) == (recvreq_addr.sin_addr.s_addr & ifc->mask)) {
                        time_t delay = PEER_DATA_RETENTION_PERIOD / 3;
                        if (ntohs(recvreq_addr.sin_port) != DISCOVERY_TCF_PORT) delay = PEER_DATA_RETENTION_PERIOD / 3 * 2;
                        else if (recvreq_addr.sin_addr.s_addr != ifc->addr) delay = PEER_DATA_RETENTION_PERIOD / 2;
                        if (last_req_slaves_time[n] + delay <= timenow) {
                            udp_send_req_slaves(ifc, &recvreq_addr);
                            last_req_slaves_time[n] = timenow;
                        }
                        /* Remember time only if local host master */
                        if (ifc->addr == recvreq_addr.sin_addr.s_addr && ntohs(recvreq_addr.sin_port) == DISCOVERY_TCF_PORT) {
                            last_master_packet_time = timenow;
                        }
                    }
                }
            }
        }
    }
    trigger_recv();
}

static void local_peer_changed(PeerServer * ps, int type, void * arg) {
    trace(LOG_DISCOVERY, "Peer changed: ps=%#" PRIxPTR ", type=%d", (uintptr_t)ps, type);
    switch (type) {
    case PS_EVENT_ADDED:
    case PS_EVENT_CHANGED:
        if (udp_server_socket >= 0 && (ps->flags & PS_FLAG_LOCAL) != 0) {
            /* Local peer changes are often caused by changes in the network configuration.
             * We should update interface list before using it to send packets. */
            ifc_cnt = build_ifclist(udp_server_socket, MAX_IFC, ifc_list);
        }
        udp_send_peer_info(ps, NULL);
        break;
    }
}

int discovery_start_udp(void) {
    int error;
    assert(!discovery_stopped);
    error = create_server_socket();
    if (error) return error;
    peer_server_add_listener(local_peer_changed, NULL);
    post_event_with_delay(udp_refresh_timer, NULL, PEER_DATA_REFRESH_PERIOD * 1000000);
    ifc_cnt = build_ifclist(udp_server_socket, MAX_IFC, ifc_list);
    memset(send_buf, 0, sizeof(send_buf));
    send_buf[0] = 'T';
    send_buf[1] = 'C';
    send_buf[2] = 'F';
    send_buf[3] = UDP_VERSION;
    udp_send_req_info(NULL);
    udp_send_all(NULL, NULL);
    return 0;
}

int discovery_stop_udp(void) {
    if (!discovery_stopped) {
        udp_send_peer_removed();
        discovery_stopped = 1;
        if (slave_info != NULL) {
            loc_free(slave_info);
            slave_cnt = 0;
            slave_max = 0;
        }
    }
    return 0;
}

#endif /* ENABLE_Discovery */
