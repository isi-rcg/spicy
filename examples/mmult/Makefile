%.PHONY: all sdsoc finish build test

all: 
	spyc test.py -func mmult -o mmult
	pylon -func mmult test.py -source mmult.cpp -header mmult.h
	pyrite -func mmult test.py -o rewrite.py
	pyramid -func mmult test.py -source mmult.cpp -o mmult_hw -header mmult.h

#compile generated code and wrapper into python package
build:
	python3.6 setup_sw.py build_ext --inplace	

#execute original python code, then execute rewritten python code
test:
	python3.6 test.py
	python3.6 rewrite.py

#Delete the unnecesaary files
clean:
	rm -rf mmult.* spyc.* pynq.py spyc_*
	rm -rf c_module* caller.cpp
	rm -rf build setup* caller* sw
	rm -rf wrapper.cpp rewrite.py libcaller*
	rm -rf _sds .Xil sd_card app.py __pycache__
	rm -rf main.o lib main run_sdsoc.tcl mmult_hw.py

#Delete the boot folder
ultraclean: clean
	rm -rf boot
