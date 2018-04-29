.PHONY: all build test clean

#Main build step
all: 
	#Translate the python function 'mmult' in the file 'test.py' to C, naming the output file 'mmult.cpp/h'
	spyc test.py -func mmult -o mmult
	#generate a wrapper for the function 'mmult'
	pylon -func mmult test.py -source mmult.cpp -header mmult.h
	#rewrite python code to call accelerator
	pyrite -func mmult test.py -o rewrite.py

#compile generated code and wrapper into python module
build:
	python3.6 setup_sw.py build_ext --inplace	

#execute original python code, then execute rewritten python code
test:
	python3.6 test.py
	python3.6 rewrite.py

#cleanup
clean:
	rm -rf build wrapper.cpp setup_sw.py
	rm -rf c_module* spyc.* spyc_*
	rm -rf caller.* mmult.* rewrite.py
