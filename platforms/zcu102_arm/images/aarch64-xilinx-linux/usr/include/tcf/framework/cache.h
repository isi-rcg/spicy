/*******************************************************************************
 * Copyright (c) 2009, 2014 Wind River Systems, Inc. and others.
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
 * Abstract asynchronous data cache support.
 *
 * In TCF, asynchronous caches are used to implement software design,
 * which we call ACPM (Asynchronous Cache Programming Model).
 * ACPM is formalized Programming Pattern for guaranteed data consistency
 * when dealing with multiple remote data sources.
 *
 * Usage example.
 * This example assumes that Context represents execution context on remote target,
 * and context data is kept in a cache that is updated on demand by sending asynchronous
 * data requests to a remote peer.
 *
 * The example shows how to implement cache client, in this case TCF command handle,
 * that handle cache misses by waiting until the cache is updated and the re-executing
 * cache client code.

    //--- Remote data provider ---

    static AbstractCache cache;

    Context * id2ctx(const char * id) {
        Channel * c = cache_channel();
        if (!cache_valid) {
            // Send data request.
            ...
            // Interrupt client execution.
            // Client will be restarted by calling cache_notify(),
            // when data retrieval is done.
            cache_wait(&cache);
        }

        // Search cached data and return.
        ...
        return ctx;
    }

    //--- Data consumer: command handler ---

    static void cache_client(void * x) {
        // Get cached data.
        // This code can be interrupted by cache misses,
        // and then re-executed again when the cache is updated.
        // Make sure the code is re-entrant.

        CommandArgs * args = (CommandArgs *)x;
        Channel * c = cache_channel();
        Context ctx = id2ctx(args->id);
        int result = context_has_state(ctx);

        // Done retrieving cached data.

        cache_exit();

        // Rest of the code does not need to be re-entrant.

        // Send command result message:

        write_stringz(&c->out, "R");
        write_stringz(&c->out, args->token);
        json_write_boolean(&c->out, result);
        write_stream(&c->out, 0);
        write_stream(&c->out, MARKER_EOM);

        // Done command handling.
    }

    static void command_handler(char * token, Channel * c) {
        // Read command arguments

        CommandArgs args;
        json_read_string(&c->inp, args.id, sizeof(args.id));
        json_test_char(&c->inp, MARKER_EOA);
        json_test_char(&c->inp, MARKER_EOM);
        strlcpy(args.token, token, sizeof(args.token));

        // Start cache client state machine:

        cache_enter(cache_client, c, args, sizeof(args));
    }

    add_command_handler(proto, "Service Name", "Command Name", command_handler);


 * Only main thread is allowed to accesses caches.
 */

#ifndef D_cache
#define D_cache

#include <tcf/framework/channel.h>

typedef void CacheClient(void *);

typedef struct AbstractCache {
    LINK link;
    struct WaitingCacheClient * wait_list_buf;
    unsigned wait_list_cnt;
    unsigned wait_list_max;
} AbstractCache;

/*
 * Start cache client state machine.
 * Note that each channel has its own instance of cache.
 * The channel will be locked during periods of time when the client
 * waits for the cached data.
 */
extern void cache_enter(CacheClient * client, Channel * channel, void * args, size_t args_size);

/*
 * Cache clients call cache_exit() to indicate end of cache access.
 */
extern void cache_exit(void);

/*
 * Add current cache client to the cache wait list and throw ERR_CACHE_MISS exception.
 * Cache data handling code call cache_wait() to suspend current client
 * until cache validation is done.
 */
#ifdef NDEBUG
extern void cache_wait(AbstractCache * cache);
#else
#define cache_wait(cache) cache_wait_dbg(__FILE__, __LINE__, cache)
extern void cache_wait_dbg(const char * file, int line, AbstractCache * cache);
#endif

/*
 * Invoke all items in the cache wait list.
 * Cache data handling code call cache_notify() to resume clients
 * that are waiting for cached data.
 */
extern void cache_notify(AbstractCache * cache);

/*
 * Invoke all items in the cache wait list.
 * Similar to cache_notify(), but client callbacks are posted to
 * event queue instead of being invoked immediately.
 */
extern void cache_notify_later(AbstractCache * cache);

/*
 * Return TCF channel of current cache client,
 * or NULL if there is no current client.
 */
extern Channel * cache_channel(void);

/*
 * Set default TCF channel for current cache client.
 */
extern void cache_set_def_channel(Channel * channel);

/*
 * Return unique ID of current cache client transaction.
 * Return 0 if there is no current transaction.
 */
extern unsigned cache_transaction_id(void);

/*
 * Return number of cache misses in the current transaction.
 */
extern unsigned cache_miss_count(void);

/*
 * Cache transaction listeners.
 */
#define CTLE_START      1
#define CTLE_RETRY      2
#define CTLE_ABORT      3
#define CTLE_COMMIT     4
typedef void CacheTransactionListener(int /* event */);
extern void add_cache_transaction_listener(CacheTransactionListener * l);

/*
 * Dispose a cache.
 */
extern void cache_dispose(AbstractCache * cache);

#endif /* D_cache */
