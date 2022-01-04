/*******************************************************************************
 * Copyright (c) 2007, 2011 Wind River Systems, Inc. and others.
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
 * This module implements BASE64 encoding and decoding of binary data.
 * BASE64 encoding is adapted from RFC 1421, with one change:
 * BASE64 eliminates the "*" mechanism for embedded clear text.
 * Also TCF version of the encoding does not allow characters outside of the BASE64 alphabet.
 */

#ifndef D_base64
#define D_base64

#include <tcf/framework/streams.h>

/*
 * Write BASE64 encoded array of bytes to output stream.
 */
extern size_t write_base64(OutputStream * out, const char * buf, size_t len);

/*
 * Read BASE64 encoded array of bytes from input stream.
 * Returns number of decoded bytes.
 */
extern size_t read_base64(InputStream * inp, char * buf, size_t buf_size);

#endif /* D_base64 */
