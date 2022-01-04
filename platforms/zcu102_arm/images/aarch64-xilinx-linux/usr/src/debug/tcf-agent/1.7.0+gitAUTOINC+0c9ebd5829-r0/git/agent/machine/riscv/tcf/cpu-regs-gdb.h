/*******************************************************************************
 * Copyright (c) 2020 Xilinx, Inc. and others.
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

#ifndef D_cpu_regs_gdb_riscv32
#define D_cpu_regs_gdb_riscv32

#include <tcf/config.h>

static const char * cpu_regs_gdb_riscv32 =
"<architecture>rv32</architecture>\n"
"<feature name='org.gnu.gdb.riscv.cpu'>\n"
"  <reg name='zero' bitsize='32' type='int' regnum='0' />\n"
"  <reg name='ra'   bitsize='32' type='code_ptr' />\n"
"  <reg name='sp'   bitsize='32' type='data_ptr' />\n"
"  <reg name='gp'   bitsize='32' type='data_ptr' />\n"
"  <reg name='tp'   bitsize='32' type='data_ptr' />\n"
"  <reg name='t0'   bitsize='32' type='int' />\n"
"  <reg name='t1'   bitsize='32' type='int' />\n"
"  <reg name='t2'   bitsize='32' type='int' />\n"
"  <reg name='fp'   bitsize='32' type='data_ptr' />\n"
"  <reg name='s1'   bitsize='32' type='int' />\n"
"  <reg name='a0'   bitsize='32' type='int' />\n"
"  <reg name='a1'   bitsize='32' type='int' />\n"
"  <reg name='a2'   bitsize='32' type='int' />\n"
"  <reg name='a3'   bitsize='32' type='int' />\n"
"  <reg name='a4'   bitsize='32' type='int' />\n"
"  <reg name='a5'   bitsize='32' type='int' />\n"
"  <reg name='a6'   bitsize='32' type='int' />\n"
"  <reg name='a7'   bitsize='32' type='int' />\n"
"  <reg name='s2'   bitsize='32' type='int' />\n"
"  <reg name='s3'   bitsize='32' type='int' />\n"
"  <reg name='s4'   bitsize='32' type='int' />\n"
"  <reg name='s5'   bitsize='32' type='int' />\n"
"  <reg name='s6'   bitsize='32' type='int' />\n"
"  <reg name='s7'   bitsize='32' type='int' />\n"
"  <reg name='s8'   bitsize='32' type='int' />\n"
"  <reg name='s9'   bitsize='32' type='int' />\n"
"  <reg name='s10'  bitsize='32' type='int' />\n"
"  <reg name='s11'  bitsize='32' type='int' />\n"
"  <reg name='t3'   bitsize='32' type='int' />\n"
"  <reg name='t4'   bitsize='32' type='int' />\n"
"  <reg name='t5'   bitsize='32' type='int' />\n"
"  <reg name='t6'   bitsize='32' type='int' />\n"
"  <reg name='pc'   bitsize='32' type='code_ptr' />\n"
"</feature>\n"
"<feature name='org.gnu.gdb.riscv.csr'>\n"
"</feature>\n";

#endif /* D_cpu_regs_gdb_riscv32 */
