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
# pylon.py
#
# Tool for generate Python C API wrappers
#
#############################################################################################

import os
import sys
import ast
import argparse
import shutil

from passes import *
from astLib import *

######################
# generate
#   generate the wrapper code to call C/C++ from python
#   and write it to a file
def generate(funcDecl,source,header,mod):
    #unpack
    ret_type,name,args = funcDecl

    #generate c/python wrapper
    code = ''
    code += ('#include <Python.h>\n')
    code += ('#include <numpy/arrayobject.h>\n')
    #spyc includes
    code += ('#include "spyc.h"\n')
    #user includes file
    code += ('#include "caller.h"\n')
    code += ('\n')

    code += ('static ')
    code += 'PyObject*'
    code += ' %s(PyObject* self, PyObject* args) {\n' % (name+'_wrapper')
    #declare function argument variables
    for arg in args:
        code += '   ' + arg.getPtr() + ';\n'
        if arg.category == 'array':
            code += '   PyArrayObject* ' + arg.name + '_obj = NULL;\n'

    #declare return variable (if needed)
    if ret_type.primitive != 'void':
        code += ('   %s %s;\n' % (ret_type.getType(),'return_arg'))
    code += '\n'
    #parse function args into argument variables
    code += '   int res = PyArg_ParseTuple(args'
    arg_str = ''
    type_str = ', "'
    for arg in args:
        type_str += formatType(arg)
        if arg.category == 'scalar':
            arg_str += ', &%s' % (arg.name)
        elif arg.category == 'array':
            arg_str += ', &%s' % (arg.name + '_obj')
        else:
            print('Error! Unknown ArgType category in pyces.py: %s' % arg.category)
            sys.exit(1)
    type_str += '"'
    code += type_str + arg_str + ');\n'
    code += '\n'
    #check for problem parsing args
    code += '   if (!res)\n'
    code += '      return NULL;\n'
    code += '\n'
    #process array args
    for arg in args:
        if arg.category == 'array':
            code += '   setupArrayArg(' + arg.name + '_obj, (void**)&' + arg.name + ', ' + getNPtype(arg) + ', "' + arg.name + '");\n'
    code += '\n'
    #call c/C++ function
    code += '   /* call function */\n'
    if ret_type.primitive != 'void':
        code += '   return_arg = '
    else:
        code += '   '
    code += '%s(' % (name+'_caller')
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += '%s' % arg.name
        cnt += 1
    code += ');\n'
    code += '\n'
    #process array args
    for arg in args:
        if arg.category == 'array':
            code += '   cleanupArrayArg(' + arg.name + '_obj, (void**)&' + arg.name + ');\n'
    code += '\n'
    #create return object
    if ret_type.primitive != 'void':
        code += '   PyObject* obj = Py_BuildValue("%s", return_arg);\n' % formatType(ret_type)
        code += '   return obj;\n'
    else:
        code += '   Py_RETURN_NONE;\n'
    #Create list of function info
    code += '}\n'
    code += '\n'
    code += '/*  define functions in module */\n'
    code += 'static PyMethodDef TheMethods[] = {\n'
    code += '   {"%s", %s, METH_VARARGS, "your c function"},\n' % ((name+'_wrapper'),(name+'_wrapper'))
    code += '   {NULL, NULL, 0, NULL}\n'
    code += '};\n'
    code += '\n'
    #create list of module info
    code += 'static struct PyModuleDef cModPyDem = {\n'
    code += '   PyModuleDef_HEAD_INIT,\n'
    code += '   "%s", "Some documentation",\n' % (name)
    code += '   -1,\n'
    code += '   TheMethods\n'
    code += '};\n'
    code += '\n'
    #create module init function
    code += 'PyMODINIT_FUNC\n'
    code += 'PyInit_%s(void) {\n' % (mod)
    code += '   PyObject* retval = PyModule_Create(&cModPyDem);\n'
    code += '   import_array();\n'
    code += '   return retval;\n'
    code += '}\n'

    #generate c/python setup script
    wrapper = 'import os\n'
    wrapper += 'import numpy\n'
    wrapper += 'from distutils.core import setup, Extension\n'
    wrapper += 'cur = os.path.dirname(os.path.realpath(__file__))\n'

    if debug:
        wrapper += 'c_module = Extension("%s", sources=["spyc_dbg.cpp","wrapper.cpp","caller.cpp","%s"],include_dirs=[cur,numpy.get_include()])' % (mod,source)
    else:
        wrapper += 'c_module = Extension("%s", sources=["spyc.cpp","wrapper.cpp","caller.cpp","%s"],include_dirs=[cur,numpy.get_include()])' % (mod,source)
    wrapper += '\n'
    wrapper += 'setup(ext_modules=[%s])\n' % (mod)
    
    if verbose:
        print ("---Python Wrapper-------------------")
        print(code)
        print ("---Python Setup---------------------")
        print(wrapper)

    #write out code to files
    fp = open('wrapper.cpp','w')
    fp.write(code)
    fp.close()

    fp = open('setup_sw.py','w')
    fp.write(wrapper)
    fp.close()

######################
# generate
#   generate the wrapper code to call C/C++ from python
#   and return it
def generate2(funcDecl,source,header,mod):
    #unpack
    ret_type,name,args = funcDecl

    code = ('static ')
    code += 'PyObject*'
    code += ' %s(PyObject* self, PyObject* args) {\n' % (name+'_wrapper')
    #declare function argument variables
    for arg in args:
        code += '   ' + arg.getPtr() + ';\n'
        if arg.category == 'array':
            code += '   PyArrayObject* ' + arg.name + '_obj = NULL;\n'

    #declare return variable (if needed)
    if ret_type.primitive != 'void':
        code += ('   %s %s;\n' % (ret_type.getType(),'return_arg'))
    code += '\n'
    #parse function args into argument variables
    code += '   int res = PyArg_ParseTuple(args'
    arg_str = ''
    type_str = ', "'
    for arg in args:
        type_str += formatType(arg)
        if arg.category == 'scalar':
            arg_str += ', &%s' % (arg.name)
        elif arg.category == 'array':
            arg_str += ', &%s' % (arg.name + '_obj')
        else:
            print('Error! Unknown ArgType category in pyces.py: %s' % arg.category)
            sys.exit(1)
    type_str += '"'
    code += type_str + arg_str + ');\n'
    code += '\n'
    #check for problem parsing args
    code += '   if (!res)\n'
    code += '      return NULL;\n'
    code += '\n'
    #process array args
    for arg in args:
        if arg.category == 'array':
            code += '   setupArrayArg(' + arg.name + '_obj, (void**)&' + arg.name + ', ' + getNPtype(arg) + ', "' + arg.name + '");\n'
    code += '\n'
    #call c/C++ function
    code += '   /* call function */\n'
    if ret_type.primitive != 'void':
        code += '   return_arg = '
    else:
        code += '   '
    code += '%s(' % (name+'_caller')
    cnt = 0
    for arg in args:
        if cnt > 0:
            code += ','
        code += '%s' % arg.name
        cnt += 1
    code += ');\n'
    code += '\n'
    #process array args
    for arg in args:
        if arg.category == 'array':
            code += '   cleanupArrayArg(' + arg.name + '_obj, (void**)&' + arg.name + ');\n'
    code += '\n'
    #create return object
    if ret_type.primitive != 'void':
        code += '   PyObject* obj = Py_BuildValue("%s", return_arg);\n' % formatType(ret_type)
        code += '   return obj;\n'
    else:
        code += '   Py_RETURN_NONE;\n'
    #Create list of function info
    code += '}\n'
    code += '\n'

    return code

######################
# getFunction
#   driver function to call function_pass to process AST and insert statment nodes
def getFunction(node):
    if not isinstance(node, ast.AST):
        raise TypeError('expected AST, got %r' % node.__class__.__name__)

    tree = function_pass(node,func)
    if tree == None:
        print('Error! Couldnt find function "%s"' % func)
        sys.exit(1)

    if verbose:
        print ("---Function AST---------------------")
        print (ast.dump(node))
    return tree

##############################################
# Main code
##############################################
num_spaces = 3


loc = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(sys.argv[0]))))
lib_c = os.path.join(loc,'lib','spyc.cpp')
lib_dbg_c = os.path.join(loc,'lib','spyc_dbg.cpp')
lib_h = os.path.join(loc,'lib','spyc.h')

parser = argparse.ArgumentParser()
parser.add_argument('-verbose',action='store_true',help='print all messages')
parser.add_argument('-debug',action='store_true',help='use debug library')
parser.add_argument('-o',metavar='<code.cpp>',help='output c code file')
parser.add_argument('-func',metavar='<name>',help='only process this function')
parser.add_argument('-source',metavar='<file.cpp>',help='c source file containing function definition',required=True)
parser.add_argument('-header',metavar='<file.h>',help='c header file to include',required=True)
parser.add_argument('-mod',metavar='<name>',help='module name')
parser.add_argument('file_py',help='file containing python function to generate a wrapper for')

args = parser.parse_args()

#process arguments
verbose = args.verbose
debug = args.debug

if args.o:
    outfile = args.o
else:
    outfile = 'out.py'

source = args.source
header = args.header

if args.mod:
    mod = args.mod
else:
    mod = 'c_module'

#only take 1 file argument
file = args.file_py

#read file contents into var
fp = open(file)
code = fp.read()
fp.close()

if verbose:
    print ('Processing file: ' + file)
    print ('---Souce Code---------------------')
    print (code)

#parse python code into abstract syntax tree
tree = ast.parse(code)

if verbose:
    print ("---Python AST-----------------------")
    print (ast.dump(tree))

if args.func:
    func = args.func

    #get sub-tree of AST for specified function
    funcTree = getFunction(tree)
    if funcTree == None:
        print('Error! Couldnt find function "%s"' % func)
        sys.exit(1)

    #fix ndarray constructor
    fourth_pass(funcTree)
    if verbose:
        print ("---Fourth Pass----------------------")
        print (ast.dump(funcTree))

    #generate wrapper
    funcDecl = getFuncDecl(funcTree)
    generate(funcDecl,source,header,mod)

    #print out tree
    if verbose:
        print ('----------------------------------')

else:
    func = findAcc(tree)

    wrap = ''
    #Generate c/python wrapper
    wrap = ''
    wrap += ('#include <Python.h>\n')
    wrap += ('#include <numpy/arrayobject.h>\n')
    #spyc includes
    wrap += ('#include "spyc.h"\n')
    #user includes file
    wrap += ('#include "caller.h"\n')
    wrap += ('\n')


    for f in func:
        fourth_pass(f)
        if verbose:
            print ("---Fourth Pass----------------------")
            print (ast.dump(f))

        #generate wrapper
        funcDecl = getFuncDecl(f)
        code = generate2(funcDecl,source,header,mod)
        wrap += code

    #Finish wrapper
    wrap += '/*  define functions in module */\n'
    wrap += 'static PyMethodDef TheMethods[] = {\n'

    for f in func:
        wrap += '   {"%s", %s, METH_VARARGS, "your c function"},\n' % ((f.name+'_wrapper'),(f.name+'_wrapper'))

    wrap += '   {NULL, NULL, 0, NULL}\n'
    wrap += '};\n'
    wrap += '\n'
    #create list of module info
    wrap += 'static struct PyModuleDef cModPyDem = {\n'
    wrap += '   PyModuleDef_HEAD_INIT,\n'
    wrap += '   "c_module", "Some documentation",\n'
    wrap += '   -1,\n'
    wrap += '   TheMethods\n'
    wrap += '};\n'
    wrap += '\n'
    #create module init function
    wrap += 'PyMODINIT_FUNC\n'
    wrap += 'PyInit_%s(void) {\n' % (mod)
    wrap += '   PyObject* retval = PyModule_Create(&cModPyDem);\n'
    wrap += '   import_array();\n'
    wrap += '   return retval;\n'
    wrap += '}\n'

    #generate c/python setup script
    wrapper = 'import os\n'
    wrapper += 'import numpy\n'
    wrapper += 'from distutils.core import setup, Extension\n'
    wrapper += 'cur = os.path.dirname(os.path.realpath(__file__))\n'
    wrapper += 'c_module = Extension("%s", sources=["spyc.cpp","wrapper.cpp","caller.cpp","%s"],include_dirs=[cur,numpy.get_include()])' % (mod,source)
    wrapper += '\n'
    wrapper += 'setup(ext_modules=[%s])\n' % (mod)
    
    if verbose:
        print ("---Python Wrapper-------------------")
        print(wrap)
        print ("---Python Setup---------------------")
        print(wrapper)

    #write out code to files
    fp = open('wrapper.cpp','w')
    fp.write(wrap)
    fp.close()

    fp = open('setup_sw.py','w')
    fp.write(wrapper)
    fp.close()

if debug:
    shutil.copyfile(lib_dbg_c,os.path.os.path.basename(lib_dbg_c))
else:
    shutil.copyfile(lib_c,os.path.os.path.basename(lib_c))
shutil.copyfile(lib_h,os.path.os.path.basename(lib_h))
# Done!

