.PHONY: test-c test-py all clean

#Main build step
all: 
	#Translates the python function 'mmult' in the file 'test.py' to C, naming the output file 'mmult'
	spyc test.py -func CannyEdgeDetector -o canny
	#generates a wrapper for the function 'mmult'
	pylon -func CannyEdgeDetector test.py -source canny.cpp -header canny.h
	#rewrite python code to call accelerator
	pyrite -func CannyEdgeDetector test.py -o rewrite.py

	#compile generated code and wrapper into python package
	python3.6 setup_sw.py build_ext --inplace	

#execute python testbench
test:
	python3.6 python.py

#execute python with accelerated function (in C/SW)
run:
	python3.6 rewrite.py

#cleanup 
clean:
	rm -rf canny.* caller.* test build c_module* setup_sw.py wrapper.* rewrite.py spyc.*
