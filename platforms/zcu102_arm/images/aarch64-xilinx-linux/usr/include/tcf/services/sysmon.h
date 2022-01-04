/*******************************************************************************
 * Copyright (c) 2007, 2015 Wind River Systems, Inc. and others.
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
 * Target service implementation: system monitor (TCF name SysMonitor)
 */

#ifndef D_sysmon
#define D_sysmon

#include <tcf/framework/protocol.h>

/* Constants for context property "ExeType" */
#define EXETYPE_USER           0  /* user process */
#define EXETYPE_KERNEL         1  /* kernel thread */
#define EXETYPE_ACCESS_DENIED  2  /* access denied */

/*
 * Initialize system monitor service.
 */
extern void ini_sys_mon_service(Protocol *);

#endif /* D_sysmon */
