include ../obj.mk

all:
	spyc code.py -func foo -o out $(VERBOSE)
	pylon -func foo code.py -source out.cpp -header out.h
	python3.6 setup_sw.py build_ext --inplace	
	pyrite -func foo code.py -o rewrite.py

clean:
	rm -rf $(OBJ)
	rm -rf build caller.* setup* spyc* wrapper* c_module* rewrite*