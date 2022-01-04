/*******************************************************************************
 * Copyright (c) 2013-4 Xilinx, Inc. and others.
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
 * Extension point definitions for symbols_elf.c.
 *
 * ELF_SYMS_GET_ADDR - get run-time address of an ELF symbol
 *
 * ELF_SYMS_BY_ADDR - find symbol by address
 */

#define ELF_SYMS_GET_ADDR do {} while(0)
#define ELF_SYMS_BY_ADDR  do {} while(0)
#define ELF_SYMS_HAS_ADDR do {} while(0)
