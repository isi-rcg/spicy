.PHONY: test-c test-py all clean

#Main build step
all: 
	#Translate the python function 'mmult' in the file 'test.py' to C, naming the output file 'mmult.cpp/h'
	spyc test.py -func mmult -o mmult

	#Compile the generated C code with the C testbench
	g++ -o test main.cpp mmult.cpp caller.cpp

#execute python testbench
test-py:
	python3.6 test.py

#execute C testbench
test-c:
	./test

#cleanup 
clean:
	rm -rf mmult.* caller.* test
