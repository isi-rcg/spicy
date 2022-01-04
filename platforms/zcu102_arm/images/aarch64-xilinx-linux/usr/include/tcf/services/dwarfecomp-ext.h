/*******************************************************************************
 * Copyright (c) 2015 Wind River Systems, Inc. and others.
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

#define COMPUTE_TLS_ADDRESS        {\
    U8_T addr = 0;\
    addr = get_tls_address(expr_ctx, expr->object->mCompUnit->mFile);\
    if (addr != 0) {\
        add(OP_constu);\
        add_uleb128(addr);\
        add(OP_add);\
    }\
}
