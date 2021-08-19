%.PHONY: all kernel package verify wrapper finish build 

# compiler tools
XILINX_VITIS_PATH = /tools/Xilinx/Vitis/2020.2
XILINX_VIVADO_PATH = /tools/Xilinx/Vivado/2020.2
XILINX_VITIS ?= $(XILINX_VITIS_PATH)
XILINX_VIVADO ?= $(XILINX_VIVADO_PATH)
XILINX_VIVADO_HLS ?= $(XILINX_VITIS)/Vivado_HLS


HOST_CXX ?= aarch64-linux-gnu-g++
EMCONFIGUTIL = ${XILINX_VITIS}/bin/emconfigutil --od .
VPP ?= ${XILINX_VITIS}/bin/v++

RM = rm -f
RMDIR = rm -rf

VITIS_PLATFORM_PATH = /nas/home/itaras/work/stereobit/scratch/examples/zcu102-custom/export/zcu102-custom
VITIS_PLATFORM = zcu102-custom
VITIS_PLATFORM_DIR = $(VITIS_PLATFORM_PATH)
VITIS_PLATFORM_PATH = $(VITIS_PLATFORM_DIR)/zcu102-custom.xpfm

# target options are : hw, sw_emu, hw_emu
TARGET = sw_emu
VPP_OPTS = --target $(TARGET)

# HW kernel

BUILD_SUBDIRS += build
HOST_OBJECTS += build/mmult.xo
HW_KERNEL_OBJS += build/mmult.xo
SRC_KERNEL_OBJS += build/mmult.xo

# xclbin
BINARY_CONTAINER_OBJS += build/mmult.xo
BINARY_CONTAINERS += binary_container.xclbin

ALL_MESSAGE_FILES = $(subst .log,.xclbin,.xo,.o,.mdb,$(SRC_KERNEL_OBJS),$(BINARY_CONTAINERS))

BUILD_SUBDIRS += src/

# Software Vars
HW_KERNEL = mmult
HW_KERNEL_FUNC = mmult
HW_KERNEL_OUTPUT = mmult
HW_KERNEL_SRC += mmult.cpp
HW_KERNEL_HEADER += mmult.h

software: kernel wrapper verify runner
hardware: $(HOST_OBJECTS) $(BINARY_CONTAINERS) package
all: software hardware

# Compile the kernel output
kernel:
	spyc test.py -func $(HW_KERNEL_FUNC) -o $(HW_KERNEL_OUTPUT)

wrapper:
	pylon -func mmult test.py -source $(HW_KERNEL_SRC)  -simulate

verify: 
	pyrite -func mmult test.py -o rewrite.py

runner:
	pyramid -func mmult test.py -source $(HW_KERNEL_SRC) -o mmult_hw 

compile:
	#Compile the generated C code with the C testbench
	g++ -o test main.cpp mmult.cpp caller.cpp

# build the kernel
$(HOST_OBJECTS): $(HW_KERNEL_SRC) krnl_mmult-compile.cfg 
	-@mkdir -p $(@D)
	-@chmod 775 $(@D)
	-@$(RM) $@
	$(VPP) $(VPP_OPTS) --compile -I"$(<D)" --config krnl_mmult-compile.cfg -o"$@" "$<"

#
# binary container: binary_container_1.xclbin
#

# build the xclbin file
$(BINARY_CONTAINERS): $(BINARY_CONTAINER_OBJS) binary_container_1-link.cfg
	-@echo $(VPP) $(VPP_OPTS) --link --config binary_container_1-link.cfg -o"$@" $(BINARY_CONTAINER_OBJS) > binary_container_1.xclbin.sh
	$(VPP) $(VPP_OPTS) --link --config binary_container_1-link.cfg -o"$@" $(BINARY_CONTAINER_OBJS)

# Build the SDCARD
package:
	-@echo Running Package Step for the System Project.
	-@echo $(VPP) --package --config package.cfg  ./binary_container.xclbin -o binary_container_final.xclbin
	$(VPP) --package --config package.cfg  ./binary_container.xclbin -o binary_container_final.xclbin


#Delete the unnecesaary files
clean:
	rm -rf hw_kernel
	rm -rf host_program
	rm -rf verification
	rm -rf spyc.* spyc_*
	rm -rf c_module* caller.cpp
	rm -rf build setup* caller* sw *.sh
	rm -rf wrapper.cpp rewrite.py libcaller*
	rm -rf .Xil sd_card app.py __pycache__
	rm -rf main.o lib main  mmult_hw.py
	-rm -rf $(HW_KERNEL)*
	-rm -rf *xclbin.*
	-rm -rf *.xclbin*
	-rm -rf *.xclbin.*
	-rm -rf xclbin.*.*
	-rm -rf binary_container_1.*
	-rm -rf binary_container_final.*
	-rm -rf binary_container_1.build
	-$(RM) *.log 
	-$(RM) $(SRC_KERNEL_OBJS) $(ALL_MESSAGE_FILES) 
	-$(RMDIR) $(BUILD_SUBDIRS)
	-rm -rf .ipcache
	-$(RMDIR) .Xil
	$(RMDIR) ./package.build
	${RMDIR} package

