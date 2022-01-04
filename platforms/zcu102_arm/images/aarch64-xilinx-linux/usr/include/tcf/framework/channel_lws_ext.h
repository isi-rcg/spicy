/*******************************************************************************
 * Copyright (c) 2016 Wind River Systems, Inc. and others.
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
 * Hook definition for LWS callback.  This hook can be used to plug your own
 * code in the various libwebsockets hooks. For more information about the possible
 * value for those hooks refer to libwebsockets documentation. The return
 * value is either ignored or a value different from 0 indicates an error.
 * The arguments are exactly the same as for libwebsockets hook except for the first
 * one which provides a pointer to a location where you can store callback specific data.
 */

#ifndef LWS_CALLBACK_USER_HOOK
#define LWS_CALLBACK_USER_HOOK(arg, wsi, reason, user, in, len) (0)
#endif
