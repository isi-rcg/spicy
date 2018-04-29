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
    header = '%s %s(' % (ret_type.getType(),name)
    cnt = 0
    for arg in args:
        if cnt > 0:
            header += ','
        header += arg.getDecl()
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
    code = ''
    code += ('#include "%s"\n' % header)
    code += ('\n')
    code += src
    
    #write out C code to file
    fp = open('caller.cpp','w')
    fp.write(code)
    fp.close()

    #Write out header
    head = ''
    head += ('#include "%s"\n' % header)
    head += ('\n')
    #caller function decl
    head += hdr

    fp = open('caller.h','w')
    fp.write(head)
    fp.close()

######################
# generate
#   generate the wrapper code to call C/C++ from python and returns it
def gen_caller(funcDecl):
    ret_type,name,args = funcDecl
    code = ret_type.getType() + ' ' + name + '_caller('
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += arg.getDecl()
        cnt += 1
    code += ') {\n'
    code += '   /* call function */\n'
    if ret_type.primitive != 'void':
        code += '   return '
    else:
        code += '   '
    code += '%s(' % name
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += '%s' % arg.name
        cnt += 1
    code += ');\n'
    code += '\n'
    code += '}\n'

    #Write out header
    head = ret_type.getType() + ' ' + name + '_caller('
    cnt = 0
    for arg in args:
        if cnt > 0:
            head += ','
        head += arg.getDecl()
        cnt += 1
    head += ');\n'

    return (code,head)

##########################
# convert
#   writes out function C code into source file
def convert(node,name):
    code = convert_code(node)
    
    #write out C code to file
    fp = open(name+'.cpp','w')
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
    
    fp = open(outfile+'.h','w')
    fp.write('#include <stdint.h>\n')
    fp.write('#include <math.h>\n')
    fp.write(h)
    fp.close()

    fp = open(outfile+'.cpp','w')
    fp.write('#include "%s.h"\n\n' % outfile)
    fp.write(c)
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
    fp.write(src_c)
    fp.close()

    fp = open('caller.cpp','w')
    fp.write(caller_c)
    fp.close()

    fp = open('caller.h','w')
    fp.write(caller_h)
    fp.close()

# Done!

