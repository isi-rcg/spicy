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

#ifndef D_cpu_regs_gdb_arm
#define D_cpu_regs_gdb_arm

#include <tcf/config.h>

static const char * cpu_regs_gdb_arm =
"<architecture>arm</architecture>\n"
"<feature name='org.gnu.gdb.arm.core'>\n"
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
"  <reg name='r11' bitsize='32' type='uint32' id='11'/>\n"
"  <reg name='r12' bitsize='32' type='uint32' id='12'/>\n"
"  <reg name='sp' bitsize='32' type='data_ptr' id='13'/>\n"
"  <reg name='lr' bitsize='32' id='14'/>\n"
"  <reg name='pc' bitsize='32' type='code_ptr' id='15'/>\n"
"  <reg name='cpsr' bitsize='32' regnum='25' id='128'/>\n"
"</feature>\n"
"<feature name='org.gnu.gdb.arm.vfp'>\n"
"  <reg name='d0' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d1' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d2' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d3' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d4' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d5' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d6' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d7' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d8' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d9' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d10' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d11' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d12' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d13' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d14' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d15' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d16' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d17' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d18' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d19' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d20' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d21' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d22' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d23' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d24' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d25' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d26' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d27' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d28' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d29' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d30' bitsize='64' type='ieee_double'/>\n"
"  <reg name='d31' bitsize='64' type='ieee_double'/>\n"
"  <reg name='fpscr' bitsize='32' type='int' group='float'/>\n"
"</feature>\n";

#endif /* D_cpu_regs_gdb_arm */
