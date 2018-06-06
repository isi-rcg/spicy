include ../obj.mk

all:
	spyc code.py -v
	pylon code.py -source out.cpp -header out.h -v
	python3.6 setup_sw.py build_ext --inplace	
	pyrite code.py -o rewrite.py
	pyramid code.py -source out.cpp -header out.h -o out_hw

clean:
	rm -rf $(OBJ)
	rm -rf out* foo* bar*
	rm -rf build c_module* setup*
	rm -rf spyc* rewrite.py wrapper.cpp
	rm -rf run_sdsoc.tcl