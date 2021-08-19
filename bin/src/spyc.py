# The Software is made available for academic or non-commercial purposes only. 
# The license is for a copy of the program for an unlimited term. Individuals 
# requesting a license for commercial use must pay for a commercial license. 
# 
# USC Stevens Institute for Innovation
# University of Southern California
# 1150 S. Olive Street, Suite 2300
# Los Angeles, CA 90115, USA
# ATTN: Accounting
# 
# DISCLAIMER. USC MAKES NO EXPRESS OR IMPLIED WARRANTIES, EITHER IN FACT OR 
# BY OPERATION OF LAW, BY STATUTE OR OTHERWISE, AND USC SPECIFICALLY AND 
# EXPRESSLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTY OF MERCHANTABILITY OR 
# FITNESS FOR A PARTICULAR PURPOSE, VALIDITY OF THE SOFTWARE OR ANY OTHER 
# INTELLECTUAL PROPERTY RIGHTS OR NON-INFRINGEMENT OF THE INTELLECTUAL 
# PROPERTY OR OTHER RIGHTS OF ANY THIRD PARTY. SOFTWARE IS MADE AVAILABLE 
# AS-IS. LIMITATION OF LIABILITY.  TO THE MAXIMUM EXTENT PERMITTED BY LAW, 
# IN NO EVENT WILL USC BE LIABLE TO ANY USER OF THIS CODE FOR ANY INCIDENTAL, 
# CONSEQUENTIAL, EXEMPLARY OR PUNITIVE DAMAGES OF ANY KIND, LOST GOODWILL, 
# LOST PROFITS, LOST BUSINESS AND/OR ANY INDIRECT ECONOMIC DAMAGES WHATSOEVER, 
# REGARDLESS OF WHETHER SUCH DAMAGES ARISE FROM CLAIMS BASED UPON CONTRACT, 
# NEGLIGENCE, TORT (INCLUDING STRICT LIABILITY OR OTHER LEGAL THEORY), A 
# BREACH OF ANY WARRANTY OR TERM OF THIS AGREEMENT, AND REGARDLESS OF 
# WHETHER USC WAS ADVISED OR HAD REASON TO KNOW OF THE POSSIBILITY OF 
# INCURRING SUCH DAMAGES IN ADVANCE.
# 
# For commercial license pricing and annual commercial update and support pricing, please contact:
# Rakesh Pandit
# USC Stevens Institute for Innovation
# University of Southern California
# 1150 S. Olive Street, Suite 2300
# Los Angeles, CA 90115, USA
# Tel: +1 213-821-3552
# Fax: +1 213-821-5001
# Email: rakeshvp@usc.edu and CC to: accounting@stevens.usc.edu
# 


# Original Version
# Author: Sam Skalicky, skalicky@isi.edu
# Institution: Information Sciences Institute, University of Southern California
# 


#############################################################################################
# spyc.py
#
# Tool for translating Python code to C/C++
#
#############################################################################################

import sys
import ast
import json
import argparse

from pasty import *
from passes import *

##########################
# convertFunc
#   writes out function C code into files
def convertFunc(node,outfile):
    #fix ndarray constructor
    fourth_pass(node)
    if verbose:
        print ("---Fourth Pass----------------------")
        print (ast.dump(node))

    #extract function return,name,args
    funcDecl = getFuncDecl(node)

    #generate the caller function and header file
    genHeader(funcDecl,outfile)
    genCaller(funcDecl,outfile+'.h')

    #write out C code to cpp file
    convert(node,outfile)

##########################
# convert_func
#    converts a single function from Python to C and returns tuple with C function and header source code
def convert_func(node):
    #fix ndarray constructor
    fourth_pass(node)
    if verbose:
        print ("---Fourth Pass----------------------")
        print (ast.dump(node))

    funcDecl = getFuncDecl(node)
    
    header = gen_header(funcDecl)

    #write out C code to cpp file
    code = convert_code(node) + '\n'

    return (code,header)

##########################
# genHeader
#    generates a header file for the Python function translated and writes it to a file
def genHeader(funcDecl,outfile):
    #unpack
    ret_type,name,args = funcDecl
    header = '#include <stdint.h>\n\n'
    header += gen_header(funcDcl)
    
    if verbose:
        print ("---C Header-------------------------")
        print(header)

    fp = open(outfile+'.h','w')
    fp.write(header)
    fp.close()

##########################
# gen_header
#    generates a header file for the Python function translated and returns the source code
def gen_header(funcDecl):
    #unpack
    ret_type,name,args = funcDecl
    header = ''
    cnt = 0
    header +='\n'
    for arg in args:
        header += '#define ' + arg.name.upper() +'_SIZE ' + str(arg.size()) + '\n'

    header +='\n'
    header += '%s %s(' % (ret_type.getType(),name)
    for arg in args:
        if cnt > 0:
            header += ','
        header += arg.getDecl()
        #header += str(arg.getType()) + ' ' + arg.name + '[' + arg.name.upper()+'_SIZE]'
        cnt += 1
    header += ');\n'

    return header

######################
# generate
#   generate the wrapper code to call C/C++ from python and writes it to a file
def genCaller(funcDecl,header):
    ret_type,name,args = funcDecl
    #caller function decl
    (src,hdr) = gen_caller(funcDecl)
    #generate c/python wrapper
    code = ('#include "caller.h"\n')
    code += ('\n')
    code += src
    
    #write out C code to file
    fp = open('caller.cpp','w')
    fp.write(code)
    fp.close()

    #Write out header
    head = '#pragma once\n'
    # TODO - could probably structure the includes a bit nicer
    head += ('#include <stdint.h>\n')
    head += ('#include <math.h>\n')
    head += ('#include <vector>\n')
    head += ('#include <fstream>\n')
    head += ('#include <iostream>\n')
    head += ('\n')
    head += hdr

    fp = open('caller.h','w')
    fp.write(head)
    fp.close()

######################
# generate
#   generate the wrapper code to call C/C++ from python and returns it
# TODO: write out the setup/tear down for the OpenCL code here?
# Need the IO ports to set up memory on the device, etc
def gen_caller(funcDecl):
    ret_type,name,args = funcDecl

    code = ret_type.getType() + ' ' + name + '_caller('
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += str(arg.getType()) + ' ' + arg.name + '[' + arg.name.upper()+'_SIZE]'
        cnt += 1
    code += ', const char* xclbinFilename) {\n' # need to pass the pointer to the xclbin filename
    #### SETUP  Standard to _all_ openCL kernels ####
    code += '    size_t size_in_bytes = DATA_SIZE * sizeof(int);\n'
    code += '    cl_int err;\n\n'

    code += '    std::vector<cl::Device> devices;\n'
    code += '    cl::Device device;\n'
    code += '    std::vector<cl::Platform> platforms;\n'
    code += '    bool found_device = false;\n\n'

    code += '    //traversing all Platforms To find Xilinx Platform and targeted\n'
    code += '    //Device in Xilinx Platform\n'
    code += '    cl::Platform::get(&platforms);\n'
    code += '    for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){\n'
    code += '        cl::Platform platform = platforms[i];\n'
    code += '        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();\n'
    code += '        if ( platformName == "Xilinx"){\n'
    code += '            devices.clear();\n'
    code += '            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);\n'
    code += '            if (devices.size()){\n'
    code += '                    device = devices[0];\n'
    code += '                    found_device = true;\n'
    code += '                    break;\n'
    code += '            }\n'
    code += '        }\n'
    code += '    }\n\n'

    code += '    if (found_device == false){\n'
    code += '       std::cout << "Error: Unable to find Target Device " << device.getInfo<CL_DEVICE_NAME>() << std::endl;\n'
    code += '       exit(-1);\n'
    code += '    }\n\n'

    code += '    // Creating Context and Command Queue for selected device\n'
    code += '    cl::Context context(device);\n'
    code += '    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);\n\n'

    code += '    // Load xclbin\n'
    code += '    std::cout << "Loading: " << xclbinFilename << std::endl;\n'
    code += '    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);\n'
    code += '    bin_file.seekg (0, bin_file.end);\n'
    code += '    unsigned nb = bin_file.tellg();\n'
    code += '    bin_file.seekg (0, bin_file.beg);\n'
    code += '    char *buf = new char [nb];\n'
    code += '    bin_file.read(buf, nb);\n\n'

    code += '    // Creating Program from Binary File\n'
    code += '    cl::Program::Binaries bins;\n'
    code += '    bins.push_back({buf,nb});\n'
    code += '    devices.resize(1);\n'
    code += '    cl::Program program(context, devices, bins);\n\n'
    #### END Setup #### 
    
    code += '    // call function\n'
    code += '    cl::Kernel ' + name + '_kernel(program, "' + name + '");\n\n'

    ### Start Allocate Buffers for the Hardware ###
    for arg in args:
        code += '    cl::Buffer buffer_' + arg.name +'(context, CL_MEM_USE_HOST_PTR, size_in_bytes,' + arg.name + ', &err);\n'
    code += '\n'
    code += '    int nargs = 0;\n'
    for arg in args:
        code += '    ' + name + '_kernel.setArg(nargs++, buffer_' + arg.name + ');\n'
    code += '\n'
    # from Host to Accelerator
    code += '    q.enqueueMigrateMemObjects({'
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += 'buffer_%s' % arg.name
        cnt += 1
    code += '}, 0 /* 0 means from host */);\n'
    code += '    q.enqueueTask(%s_kernel);\n' % name
    # from Accelerator to Host
    code += '    q.enqueueMigrateMemObjects({'
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += 'buffer_%s' % arg.name
        cnt += 1
    code += '}, CL_MIGRATE_MEM_OBJECT_HOST);\n\n'
    code += '    q.finish();\n\n'
    for arg in args:
        code += '    q.enqueueUnmapMemObject(buffer_' + arg.name + ', ' + arg.name + ');\n'
    code += '\n'
    code += '    q.finish();\n\n'
    ### End Allocate Buffers for the Hardware ###

    code += '\n'
    code += '}\n'


    ##############################
    ###### Write out header ######
    ##############################
    
    head = ''
    for arg in args:
        head += '#define ' + arg.name.upper() +'_SIZE ' + str(arg.size()) + '\n'
    head +='\n'
    
    # -1 means that there isn't an associated value with the define
    define_args = [('CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY', 1), ('CL_HPP_MINIMUM_OPENCL_VERSION', 120), ('CL_HPP_TARGET_OPENCL_VERSION', 120), ('CL_USE_DEPRECATED_OPENCL_1_2_APIS', -1), ('CL_HPP_CL_1_2_DEFAULT_BUILD', -1)]
    for define_arg in define_args:
        if define_arg[1] == -1:
            head += 'define ' + define_arg[0] + '\n'
        else:
            head += 'define ' + define_arg[0] + ' ' + str(define_arg[1]) + '\n'
    head += '#include <CL/cl2.hpp>\n'
    head +='\n'
    
    head += ret_type.getType() + ' ' + name + '_caller('
    cnt = 0
    for arg in args:
        if cnt > 0:
            head += ','
        head += str(arg.getType()) + ' ' + arg.name + '[' + arg.name.upper()+'_SIZE]'
        #head += arg.getDecl()
        cnt += 1
    head += ', const char* xclbinFilename);\n' # need to pass the pointer to the xclbin filename

    return (code,head)

##########################
# convert
#   writes out function C code into source file
def convert(node,name):
    code = convert_code(node)
    
    #write out C code to file
    fp = open(name+'.cpp','w+')
    fp.write('#include <math.h>\n')
    fp.write('#include "%s.h"\n' % name)
    fp.write('\n')
    fp.write(code)
    fp.close()

##########################
# convert_node
#   return C code for Python function
def convert_code(node):
    #insert statement nodes into AST
    first_pass(node)
    if verbose:
        print ("---First Pass-----------------------")
        print (ast.dump(node))

    code = PastY()
    if verbose:
        code.verbose = True
        print ("---Traverse AST---------------------")
    #convert AST to C syntax
    code.set(node)
    if verbose:
        print ("---C Code---------------------------")
        print (code.string)
    return code.string

##########################
# processFunc
#    processes a single function and writes to files
def processFunc(tree,outfile):
    if verbose:
        print ("---Function AST---------------------")
        print(ast.dump(tree))

    #group docstrings with associated nodes
    second_pass(tree)
    if verbose:
        print ("---Second Pass----------------------")
        print (ast.dump(tree))

    #convert this function to C and write out
    convertFunc(tree,outfile)

    #print out tree
    if verbose:
        print ('----------------------------------')

##########################
# process_func
#    processes a single function and returns the code
def process_func(tree):
    if verbose:
        print ("---Function AST---------------------")
        print(ast.dump(tree))

    #group docstrings with associated nodes
    second_pass(tree)
    if verbose:
        print ("---Second Pass----------------------")
        print (ast.dump(tree))

    #convert this function to C
    code = convert_func(tree)

    #print out tree
    if verbose:
        print ('----------------------------------')
    return code

##########################
# nested_funcs
#    iteratively searches for nested functions in target function and translates all to C
def nested_funcs(tree,func):
    global processed
    if func in processed:
        return ('','')
    else:
        processed.append(func)
        callees = []
        code = ''
        header = ''
        #get ast for this function
        func_tree = function_pass(tree,func)
        if func_tree != None:
            #look for any nested functions in this function
            fifth_pass(func_tree,callees)
        else:
            print('Error! Function %s not defined' % func)
            exit(1)
            
        if verbose:
            print('---Nested Functions for %s' % (func.ljust(12,'-')))
            for f in callees:
                print(f)

        #recursively process nested functions found in this function
        for f in callees:
            c,h = nested_funcs(tree,f)
            code += c
            header += h

        #translate this function to C
        c,h = process_func(func_tree)
        code += c
        header += h
        
        if verbose:
            print(code)
            print(header)
        #return translated code
        return (code,header)

##############################################
# Main code
##############################################
processed = []
parser = argparse.ArgumentParser()
parser.add_argument('-verbose',action='store_true',help='print all messages')
parser.add_argument('-o',metavar='<file_name>',help='output c code filename -- without extension (will be automatically added)')
parser.add_argument('-func',metavar='<name>',help='function to process')
parser.add_argument('file',help='file to compile')

args = parser.parse_args()

#process arguments
if args.verbose:
    verbose = True
else:
    verbose = False

if args.o:
    outfile = args.o
else:
    outfile = 'out'

#only take 1 file argument
file = args.file

#read file contents into var
fp = open(file)
code = fp.read()
fp.close()

#parse python code into abstract syntax tree
tree = ast.parse(code)

#root = defuse_pass(tree)
#graph = root.getGraph()
#root.printGraph(graph)
#root.showGraph(graph)

if verbose:
    print ('Compiling file: ' + file)
    print ('---Souce Code---------------------')
    print (code)
    print ("---Python AST-----------------------")
    print (ast.dump(tree))

if args.func:
    #if a function to translate was specified on the cmd line, handle it
    func = args.func
    #setup nested functions 
    (c,h) = nested_funcs(tree,func)
    
    fp = open(outfile+'.cpp','w')
    fp.write('#include <math.h>\n')
    fp.write('extern "C" {\n\n')
    fp.write(c)
    fp.write('\n}\n')
    fp.close()
    
    func_tree = function_pass(tree,func)
    funcDecl = getFuncDecl(func_tree)
    genCaller(funcDecl,outfile+'.h')

else:
    #otherwise search through the code for spyc pragmas marking functions for translation
    func = findAcc(tree)

    if len(func) == 0:
        print('Error! No functions to accelerate in file %s' % (file))
        sys.exit(1)

    if verbose:
        print('---Functions to accelerate----------')
        for f in func:
            print(f.name)
    src_c = ''
    src_h = ''
    caller_c = '#include "caller.h"\n'
    caller_h = '#include "%s"\n' % (outfile+'.h')
    #translate all of those functions
    for f in func:
        (c,h) = nested_funcs(tree,f.name)
        src_c += c
        src_h += h

        funcDecl = getFuncDecl(f)
        c_c,c_h = gen_caller(funcDecl)
        caller_c += c_c
        caller_h += c_h

    fp = open(outfile+'.h','w')
    fp.write('#include <stdint.h>\n')
    fp.write('#include <math.h>\n')
    fp.write(src_h)
    fp.close()

    fp = open(outfile+'.cpp','w')
    fp.write('#include "%s.h"\n\n' % outfile)
    fp.write('extern "C" {\n')
    fp.write(src_c)
    fp.write('}\n')
    fp.close()

    fp = open('caller.cpp','w')
    fp.write(caller_c)
    fp.close()

    fp = open('caller.h','w')
    fp.write(caller_h)
    fp.close()

# Done!

