/*******************************************************************************
 * Copyright (c) 2007-2017 Wind River Systems, Inc. and others.
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
 * TCF Registers - CPU registers access service.
 */

#ifndef D_registers
#define D_registers

#include <tcf/config.h>

#if SERVICE_Registers

#include <tcf/framework/protocol.h>
#include <tcf/framework/context.h>

/*
 * Notify clients about register value change.
 */
extern void send_event_register_changed(const char * id);

/*
 * Notify clients about register definitions change.
 */
extern void send_event_register_definitions_changed(void);

typedef struct RegistersEventListener {
    void (*register_changed)(Context * ctx, int frame, RegisterDefinition * def, void * args);
    void (*register_definitions_changed)(void * args);
} RegistersEventListener;

/*
 * Add a listener for Registers service events.
 */
extern void add_registers_event_listener(RegistersEventListener * listener, void * args);

/*
 * Initialize registers service.
 */
extern void ini_registers_service(Protocol *, TCFBroadcastGroup *);

#endif /* SERVICE_Registers */

#endif /* D_registers */
