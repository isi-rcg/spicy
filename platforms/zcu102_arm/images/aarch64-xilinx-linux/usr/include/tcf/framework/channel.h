/*******************************************************************************
 * Copyright (c) 2007-2018 Wind River Systems, Inc. and others.
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
 * Transport agnostic TCF communication channel interface.
 */

#ifndef D_channel
#define D_channel

#include <tcf/framework/streams.h>
#include <tcf/framework/link.h>
#include <tcf/framework/peer.h>
#include <tcf/framework/client.h>
#include <tcf/framework/shutdown.h>

extern ShutdownInfo channel_shutdown;
extern LINK channel_root;
extern LINK channel_server_root;

#define chanlink2channelp(A) ((Channel *)((char *)(A) - offsetof(Channel, chanlink)))
#define servlink2channelserverp(A) ((ChannelServer *)((char *)(A) - offsetof(ChannelServer, servlink)))

/*
 * broadcast_group_free() API is deprecated and replaced by
 * broadcast_group_unlock() API.
 */
#define broadcast_group_free broadcast_group_unlock

struct Protocol;
typedef struct TCFBroadcastGroup TCFBroadcastGroup;
typedef struct ChannelServer ChannelServer;
typedef struct Channel Channel;

struct TCFBroadcastGroup {
    int magic;
    unsigned char buf[256];
    OutputStream out;                   /* Broadcast stream */
    LINK channels;                      /* Channels in group */
    unsigned ref_count;                 /* reference count, see broadcast_group_lock() and broadcast_group_unlock() */
};

enum {
    ChannelStateStartWait,
    ChannelStateStarted,
    ChannelStateHelloSent,
    ChannelStateHelloReceived,
    ChannelStateConnected,
    ChannelStateRedirectSent,
    ChannelStateRedirectReceived,
    ChannelStateDisconnected
};

struct Channel {
    InputStream inp;                    /* Input stream */
    OutputStream out;                   /* Output stream */
    TCFBroadcastGroup * bcg;            /* Broadcast group */
    void * client_data;                 /* Client data */
    struct Protocol * protocol;         /* Channel protocol */
    char * peer_name;                   /* A human readable remote peer name */
    int peer_service_cnt;               /* Number of remote peer service names */
    char ** peer_service_list;          /* List of remote peer service names */
    LINK chanlink;                      /* Channel list */
    LINK bclink;                        /* Broadcast list */
    LINK susplink;                      /* Suspend list */
    LINK locks;                         /* List of channel locks */
    int congestion_level;               /* Congestion level */
    int state;                          /* Current state */
    int disable_zero_copy;              /* Don't send ZeroCopy in Hello message even if we support it */
    int incoming;                       /* Created by an incoming connect */
    ClientConnection client;
    int notified_open;

    /* Populated by channel implementation */
    void (*start_comm)(Channel *);      /* Start communication */
    void (*check_pending)(Channel *);   /* Check for pending messages */
    int (*message_count)(Channel *);    /* Return number of pending messages */
    void (*lock)(Channel *);            /* Lock channel from deletion */
    void (*unlock)(Channel *);          /* Unlock channel */
    int (*is_closed)(Channel *);        /* Return true if channel is closed */
    void (*close)(Channel *, int);      /* Close channel */

    /* Populated by channel client, NULL values mean default handling */
    void (*connecting)(Channel *);      /* Called when channel is ready for transmit */
    void (*connected)(Channel *);       /* Called when channel negotiation is complete */
    void (*receive)(Channel *);         /* Called when messages has been received */
    void (*disconnected)(Channel *);    /* Called when channel is disconnected */
};

struct ChannelServer {
    PeerServer * ps;                    /* Server peer address information */
    LINK servlink;                      /* Channel server list */
    void * client_data;                 /* Client data */
    void (*new_conn)(ChannelServer *, Channel *); /* New connection call back */
    void (*close)(ChannelServer *);     /* Close channel server */
    struct Protocol * protocol;         /* Channel protocol */
    TCFBroadcastGroup * bcg;            /* Broadcast group */
};

/*
 * Register channel create callback.
 * Listener can use the callback to register command and event
 * handlers.
 */
typedef void (*ChannelCreateListener)(Channel *);
extern void add_channel_create_listener(ChannelCreateListener listener);

/*
 * Register channel open callback.
 * Listener can use the callback to inspect the peer_service_list and
 * send commands.
 */
typedef void (*ChannelOpenListener)(Channel *);
extern void add_channel_open_listener(ChannelOpenListener listener);

/*
 * Register channel close callback.
 * Service implementation can use the callback to deallocate resources
 * after a client disconnects.
 */
typedef void (*ChannelCloseListener)(Channel *);
extern void add_channel_close_listener(ChannelCloseListener listener);

/*
 * Notify listeners about channel being created.
 * The function is called from channel implementation code,
 * it is not intended to be called by clients.
 */
extern void notify_channel_created(Channel *);

/*
 * Notify listeners about channel being opened.
 * The function is called from channel implementation code,
 * it is not intended to be called by clients.
 */
extern void notify_channel_opened(Channel *);

/*
 * Notify listeners about channel being closed.
 * The function is called from channel implementation code,
 * it is not intended to be called by clients.
 */
extern void notify_channel_closed(Channel *);

/*
 * Start TCF channel server.
 * On error returns NULL and sets errno.
 */
extern ChannelServer * channel_server(PeerServer *);

/*
 * Connect to TCF channel server.
 * On error returns NULL and sets errno.
 */
typedef void (*ChannelConnectCallBack)(void * /* callback_args */, int /* error */, Channel *);
extern void channel_connect(PeerServer * server, ChannelConnectCallBack callback, void * callback_args);

/*
 * Add a channel transport
 */
typedef ChannelServer * (*ChannelServerCreate)(PeerServer * /* ps */);
typedef void (*ChannelConnect)(PeerServer * /* ps */,  ChannelConnectCallBack /* callback */, void * /* callback_args */);
extern void add_channel_transport(const char * transportname, ChannelServerCreate create, ChannelConnect connect);

/*
 * Register an extension of struct Channel.
 * Return offset of extension data area.
 * Additional memory of given size will be allocated in each Channel struct.
 * Client are allowed to call this function only during initialization.
 */
extern size_t channel_extension(size_t size);

/*
 * Allocate a buffer to store the channel. This routine will take care of
 * allocating the various channel extensions defined using the
 * channel_extension() API.
 */
extern Channel * channel_alloc(void);

/*
 * Release a buffer allocated using channel_alloc().
 */
extern void channel_free(Channel * c);

/*
 * Start communication of a newly created channel.
 */
extern void channel_start(Channel *);

/*
 * Close communication channel.
 */
extern void channel_close(Channel *);

/*
 * Allocate and return new "Broadcast Group" object.
 * Broadcast Group is collection of channels that participate together in broadcasting a message.
 */
extern TCFBroadcastGroup * broadcast_group_alloc(void);

/*
 * Increment reference counter of broadcast group.
 * While ref count > 0 object will not be deleted.
 */
void broadcast_group_lock(TCFBroadcastGroup *);

/*
 * Decrement reference counter.
 * If ref count == 0, remove channels from Broadcast Group and deallocate
 * the group object.
 */
void broadcast_group_unlock(TCFBroadcastGroup *);

/*
 * Add a channel to a Broadcast Group.
 * If the channel is already in a group, it is removed from it first.
 */
extern void channel_set_broadcast_group(Channel *, TCFBroadcastGroup *);

/*
 * Remove channel from Suspend Group. Does nothing if the channel is not a member of a group.
 */
extern void channel_clear_broadcast_group(Channel *);

/*
 * Lock a channel. A closed channel will not be deallocated until it is unlocked.
 * Each call of this function increments the channel reference counter.
 */
extern void channel_lock(Channel *);

/*
 * Unlock a channel.
 * Each call of this function decrements the channel reference counter.
 * If channel is closed and reference count is zero, then the channel object is closed.
 */
extern void channel_unlock(Channel *);

/*
 * Lock a channel. Same as channel_lock(), but, if given a message (e.g. a service name),
 * it will log a warning if the channel is not unlocked after it is closed.
 * Note: same char pointer must be used for matching channel_unlock_with_msg() call.
 */
extern void channel_lock_with_msg(Channel *, const char *);

/*
 * Unlock a channel. To be used together with channel_lock_with_msg().
 */
extern void channel_unlock_with_msg(Channel *, const char *);

/*
 * Return 1 if channel is closed, otherwise return 0.
 */
extern int is_channel_closed(Channel *);

/* Deprecated function names are kept for backward compatibility */
#define stream_lock(channel) channel_lock(channel)
#define stream_unlock(channel) channel_unlock(channel)
#define is_stream_closed(channel) is_channel_closed(channel)

/*
 * Create and return PeerServer object with attribute values taken from given URL.
 */
extern PeerServer * channel_peer_from_url(const char *);

/*
 * Create and return Json object with attributes from given PeerServer.
 */
extern char * channel_peer_to_json(PeerServer * ps);

#endif /* D_channel */
