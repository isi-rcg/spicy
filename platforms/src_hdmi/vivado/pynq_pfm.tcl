# zc702_pfm.tcl --
#
# This file uses the SDSoC Tcl Platform API to create the
# zc702 hardware platform file
#
# Copyright (c) 2015 Xilinx, Inc.
#

set pfm [sdsoc::create_pfm pynq_hdmi.hpfm]
sdsoc::pfm_name        $pfm "xilinx.com" "xd" "pynq_hdmi" "1.0"
sdsoc::pfm_description $pfm "Pynq"

sdsoc::pfm_clock       $pfm FCLK_CLK0 ps7 0 true psr0
sdsoc::pfm_clock       $pfm FCLK_CLK1 ps7 1 false psr1
sdsoc::pfm_clock       $pfm FCLK_CLK2 ps7 2 false psr2
sdsoc::pfm_clock       $pfm FCLK_CLK3 ps7 3 false psr3

sdsoc::pfm_axi_port    $pfm M_AXI_GP1 ps7 M_AXI_GP
sdsoc::pfm_axi_port    $pfm S_AXI_ACP ps7 S_AXI_ACP
sdsoc::pfm_axi_port    $pfm S_AXI_HP1 ps7 S_AXI_HP
sdsoc::pfm_axi_port    $pfm S_AXI_HP2 ps7 S_AXI_HP
sdsoc::pfm_axi_port    $pfm S_AXI_HP3 ps7 S_AXI_HP

for {set i 1} {$i < 16} {incr i} {
  sdsoc::pfm_irq       $pfm In$i "concat"
}
sdsoc::generate_hw_pfm $pfm

