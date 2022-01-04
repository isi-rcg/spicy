/*******************************************************************************
* Copyright (c) 2018 Xilinx, Inc. and others.
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
*     Xilinx - initial API and implementation
*******************************************************************************/

/*
 * Implements RFC 1951: http://www.ietf.org/rfc/rfc1951.txt
 * Only decompressor is implemented.
 */

#ifndef D_compression
#define D_compression

#include <tcf/config.h>

extern unsigned decompress(void * src_buf, size_t src_size, void * dst_buf, size_t dst_size);

#endif /* D_compression */
