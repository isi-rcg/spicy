/*******************************************************************************
 * Copyright (c) 2017 Xilinx, Inc. and others.
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

#ifndef D_cpu_regs_gdb_powerpc
#define D_cpu_regs_gdb_powerpc

#include <tcf/config.h>

static const char * cpu_regs_gdb_powerpc =
"<architecture>powerpc:common</architecture>\n"
"<feature name='org.gnu.gdb.power.core'>\n"
"  <reg name='r0' bitsize='32' type='uint32'/>\n"
"  <reg name='r1' bitsize='32' type='uint32'/>\n"
"  <reg name='r2' bitsize='32' type='uint32'/>\n"
"  <reg name='r3' bitsize='32' type='uint32'/>\n"
"  <reg name='r4' bitsize='32' type='uint32'/>\n"
"  <reg name='r5' bitsize='32' type='uint32'/>\n"
"  <reg name='r6' bitsize='32' type='uint32'/>\n"
"  <reg name='r7' bitsize='32' type='uint32'/>\n"
"  <reg name='r8' bitsize='32' type='uint32'/>\n"
"  <reg name='r9' bitsize='32' type='uint32'/>\n"
"  <reg name='r10' bitsize='32' type='uint32'/>\n"
"  <reg name='r11' bitsize='32' type='uint32'/>\n"
"  <reg name='r12' bitsize='32' type='uint32'/>\n"
"  <reg name='r13' bitsize='32' type='uint32'/>\n"
"  <reg name='r14' bitsize='32' type='uint32'/>\n"
"  <reg name='r15' bitsize='32' type='uint32'/>\n"
"  <reg name='r16' bitsize='32' type='uint32'/>\n"
"  <reg name='r17' bitsize='32' type='uint32'/>\n"
"  <reg name='r18' bitsize='32' type='uint32'/>\n"
"  <reg name='r19' bitsize='32' type='uint32'/>\n"
"  <reg name='r20' bitsize='32' type='uint32'/>\n"
"  <reg name='r21' bitsize='32' type='uint32'/>\n"
"  <reg name='r22' bitsize='32' type='uint32'/>\n"
"  <reg name='r23' bitsize='32' type='uint32'/>\n"
"  <reg name='r24' bitsize='32' type='uint32'/>\n"
"  <reg name='r25' bitsize='32' type='uint32'/>\n"
"  <reg name='r26' bitsize='32' type='uint32'/>\n"
"  <reg name='r27' bitsize='32' type='uint32'/>\n"
"  <reg name='r28' bitsize='32' type='uint32'/>\n"
"  <reg name='r29' bitsize='32' type='uint32'/>\n"
"  <reg name='r30' bitsize='32' type='uint32'/>\n"
"  <reg name='r31' bitsize='32' type='uint32'/>\n"
"  <reg name='pc' bitsize='32' type='code_ptr' regnum='64'/>\n"
"  <reg name='msr' bitsize='32' type='uint32'/>\n"
"  <reg name='cr' bitsize='32' type='uint32'/>\n"
"  <reg name='lr' bitsize='32' type='code_ptr'/>\n"
"  <reg name='ctr' bitsize='32' type='uint32'/>\n"
"  <reg name='xer' bitsize='32' type='uint32'/>\n"
"</feature>\n";

#endif /* D_cpu_regs_gdb_powerpc */
