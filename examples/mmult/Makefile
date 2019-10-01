%.PHONY: all sdsoc finish build test

all: 
	spyc test.py -func mmult -o mmult
	pylon -func mmult test.py -source mmult.cpp -header mmult.h
	pyrite -func mmult test.py -o rewrite.py
	pyramid -func mmult test.py -source mmult.cpp -o mmult_hw -header mmult.h

#compile generated code and wrapper into python package
build:
	python3.6 setup_sw.py build_ext --inplace	

compile:
	#Compile the generated C code with the C testbench
	g++ -o test main.cpp mmult.cpp caller.cpp

sdscc:
	sds++ -c -fPIC -sds-pf ../../platforms/pynq_bare -sds-hw mmult mmult.cpp -sds-end main.cpp -o main.o
	sds++ -sds-pf ../../platforms/pynq_bare -sds-hw mmult mmult.cpp -sds-end main.o mmult.o caller.o -o test.elf -mno-bitstream

link:
	sds++ -c -fPIC -sds-pf ../../platforms/pynq_bare main.cpp -o link.o
	sds++ -sds-pf ../../platforms/pynq_bare link.o -o link.elf -L. -lcaller -mno-bitstream

#execute original python code, then execute rewritten python code
test:
	python3.6 test.py
	python3.6 rewrite.py

#Delete the unnecesaary files
clean:
	rm -rf mmult.* spyc.* pynq.py spyc_*
	rm -rf c_module* caller.cpp test link.o pynqlib.*
	rm -rf build setup* caller* sw test.elf  test.elf.bit
	rm -rf wrapper.cpp rewrite.py libcaller* link.elf
	rm -rf _sds .Xil sd_card app.py __pycache__ libxlnk_cma.h
	rm -rf main.o lib main run_sdsoc.tcl mmult_hw.py

#Delete the boot folder
ultraclean: clean
	rm -rf boot
