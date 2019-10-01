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
# pyramid.py
#
# Tool for generating SDSoC implementation scripts
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
def generate(funcDecl,source,header,mod):
    #unpack
    ret_type,name,args = funcDecl

    #generate c/python setup script
    wrapper = ''
    wrapper += 'import numpy\n'
    wrapper += 'from distutils.core import setup, Extension\n'
    wrapper += '\n'
    wrapper += 'c_module = Extension("%s", sources=["spyc.cpp","wrapper.cpp"],include_dirs=[numpy.get_include()],libraries = ["caller"],library_dirs = ["."])' % (mod)
    wrapper += '\n'
    wrapper += 'setup(ext_modules=[%s])\n' % (mod)
    
    if verbose:
        print ("---Python Wrapper-------------------")
        print(code)
        print ("---Python Setup---------------------")
        print(wrapper)

    fp = open('setup_hw.py','w')
    fp.write(wrapper)
    fp.close()

    if source != None:
        #generate the tcl script to run the flow
        source_name = os.path.basename(os.path.splitext(source)[0])
        script = ''
        script += 'sds++ -c -fPIC -sds-pf ' + platform + ' -sds-hw ' + name + ' ' + source + ' -sds-end ' + source + ' -o ' + source_name + '.o\n'
        script += 'sds++ -c -fPIC -sds-pf ' + platform + ' -sds-hw ' + name + ' ' + source + ' -sds-end caller.cpp -o caller.o\n'
        script += 'sdscc -c -fPIC -sds-pf ' + platform + ' -sds-hw ' + name + ' ' + source + ' -sds-end pynqlib.c -o pynqlib.o\n'
        script += 'sds++ -sds-pf ' + platform + ' -shared ' +source_name + '.o caller.o pynqlib.o -o libcaller.so\n'
        script += 'rm -rf boot\n'
        script += 'mkdir boot\n'
        script += 'cp setup_hw.py boot\n'
        script += 'cp '+outfile+'.py boot\n'
        script += 'cp ' + source + ' boot\n'
        script += 'cp ' + header + ' boot\n'
        script += 'cp wrapper.cpp boot\n'
        script += 'cp caller.h boot\n'
        script += 'cp spyc.cpp boot\n'
        script += 'cp spyc.h boot\n'
        script += 'cp spyc.py boot\n'
        script += 'cp libcaller.so.bit boot\n'
        script += 'cp libcaller.so boot\n'
        script += 'cp setup.sh boot\n'
        fp = open('run_sdsoc.tcl','w')
        fp.write(script)
        fp.close()

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


imports = 'import spyc\n'
imports += '\n'

##############################################
# Main code
##############################################
num_spaces = 3

loc = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(sys.argv[0]))))
lib_c = os.path.join(loc,'lib','spyc.cpp')
lib_h = os.path.join(loc,'lib','spyc.h')
lib_py = os.path.join(loc,'lib','spyc.py')
pynqlib_c = os.path.join(loc,'lib','pynqlib.c')
pynqlib_h = os.path.join(loc,'lib','libxlnk_cma.h')


parser = argparse.ArgumentParser()
parser.add_argument('-verbose',action='store_true',help='print all messages')
parser.add_argument('-func',metavar='<name>',help='only process this function')
parser.add_argument('-source',metavar='<file.cpp>',help='c source file containing function definition')
parser.add_argument('-header',metavar='<file.cpp>',help='c source file containing function definition',required=True)
parser.add_argument('-pf',metavar='<platform>',help='platform')
parser.add_argument('-mod',metavar='<name>',help='module name')
parser.add_argument('-o',metavar='<output_name>',help='rewritten application python file name')
parser.add_argument('file_py',help='file containing python function to generate a wrapper for')

args = parser.parse_args()

#process arguments
if args.verbose:
    verbose = True
else:
    verbose = False

if args.source:
    source = args.source
else:
    source = None
    
header = args.header

if args.o:
    outfile = args.o
else:
    outfile = 'app'

if args.mod:
    mod = args.mod
else:
    mod = 'c_module'

if args.pf:
    platform = args.pf
else:
    platform = 'pynq_bare'
    
#check if platform exists
xpfm = os.path.join(platform,os.path.basename(platform)+'.xpfm')
if not(os.path.isfile(xpfm)):
    #check if its in the platforms directory
    tmp = os.path.join(loc,'platforms',xpfm)
    if os.path.isfile(tmp):
        platform = os.path.join(loc,'platforms',platform)
    else:
        print('Error! Platform "%s" not found' % platform)
        exit(1)
        
imports = 'import ' + mod + '\n' + imports

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

    #replace call in old code to wrapper function
    callsites = findCallers(tree,func)
    lines = code.split('\n')
    for call in callsites:
        lines[call-1] = re.sub('('+func+')',mod+'.'+func+'_wrapper',lines[call-1])

    #replace call to ndarray in old code
    fstart,fstop = getFuncLineRange(funcTree,len(lines)+1,-1)
    for idx in range(0,fstart-1):
        lines[idx-1] = re.sub('[a-zA-Z]*[\.]*(ndarray)','spyc.init_contiguous_ndarray',lines[idx-1])
    for idx in range(fstop+1,len(lines)):
        lines[idx-1] = re.sub('[a-zA-Z]*[\.]*(ndarray)','spyc.init_contiguous_ndarray',lines[idx-1])

    newcode = '\n'.join(lines)

    newcode = imports + newcode

else:
    func = findAcc(tree)

    lines = code.split('\n')

    source_name = os.path.basename(os.path.splitext(source)[0])

    hw_funcs = ''
    for f in func:
        #fix ndarray constructor
        fourth_pass(f)
        if verbose:
            print ("---Fourth Pass----------------------")
            print (ast.dump(f))

        hw_funcs += '-sds-hw ' + f.name + ' ' + source + ' -sds-end '

    #generate the tcl script to run the flow
    script = ''    
    script += 'sds++ -c -fPIC -sds-pf ../../pynq ' + hw_funcs + source + ' -o ' + source_name + '.o\n'
    script += 'sds++ -c -fPIC -sds-pf ../../pynq -sds-hw ' + f.name + ' ' + source + ' -sds-end caller.cpp -o caller.o\n'
    script += 'sds++ -sds-pf ../../pynq -shared ' +source_name + '.o caller.o -o libcaller.so\n'
    script += 'arm-linux-gnueabihf-ar crs libcaller.a _sds/swstubs/cf_stub.o _sds/swstubs/portinfo.o _sds/swstubs/' + source_name + '.o _sds/swstubs/caller.o\n'
    script += 'rm -rf boot\n'
    script += 'mkdir boot\n'
    script += 'cp setup_hw.py boot\n'
    script += 'cp '+outfile+'.py boot\n'
    script += 'cp ' + source + ' boot\n'
    script += 'cp ' + header + ' boot\n'
    script += 'cp wrapper.cpp boot\n'
    script += 'cp caller.h boot\n'
    script += 'cp spyc.cpp boot\n'
    script += 'cp spyc.h boot\n'
    script += 'cp spyc.py boot\n'
    script += 'cp _sds/p0/.boot/libcaller.so.bit.bin boot\n'
    script += 'cp libcaller.so boot\n'
    script += 'cp setup.sh boot\n'
    fp = open('run_sdsoc.tcl','w')
    fp.write(script)
    fp.close()

    for f in func:
        callsites = findCallers(tree,f.name)
        for call in callsites:
            lines[call-1] = re.sub('('+f.name+')',mod+'.'+f.name+'_wrapper',lines[call-1])

        #replace call to ndarray in old code
        fstart,fstop = getFuncLineRange(f,len(lines)+1,-1)
        for idx in range(0,fstart-1):
            lines[idx-1] = re.sub('[a-zA-Z]*[\.]*(ndarray)','init_contiguous_ndarray',lines[idx-1])
        for idx in range(fstop+1,len(lines)):
            lines[idx-1] = re.sub('[a-zA-Z]*[\.]*(ndarray)','init_contiguous_ndarray',lines[idx-1])

    newcode = '\n'.join(lines)
    newcode = imports + newcode

    #generate c/python setup script
    wrapper = ''
    wrapper += 'import numpy\n'
    wrapper += 'from distutils.core import setup, Extension\n'
    wrapper += '\n'
    wrapper += 'c_module = Extension("%s", sources=["spyc.cpp","wrapper.cpp"],include_dirs=[numpy.get_include()],libraries = ["caller","sds_lib"],library_dirs = [".","/usr/lib"])' % (mod)
    wrapper += '\n'
    wrapper += 'setup(ext_modules=[%s])\n' % (mod)
    
    if verbose:
        print ("---Python Wrapper-------------------")
        print(code)
        print ("---Python Setup---------------------")
        print(wrapper)

    fp = open('setup_hw.py','w')
    fp.write(wrapper)
    fp.close()


if verbose:
    print ('---Rewrite Code-------------------')
    print (newcode)

#write out rewritten code
fp = open(outfile+'.py','w')
fp.write(newcode)
fp.close()

#write out setup script
ROOT_UID="0"

#Check if run as root
text = 'if [ "$UID" -ne 0 ] ; then\n'
text += '    echo "You must be root!"\n'
text += '    kill -INT $$\n'
text += 'fi\n'
text += 'ulimit -s unlimited\n'
text += 'python3.6 -c "import pynq; bit = pynq.Bitstream(\'libcaller.so.bit\'); bit.download()"\n'
text += 'DIR="$(pwd)"\n'
text += 'export LD_LIBRARY_PATH=$DIR:$LD_LIBRARY_PATH\n'
text += 'python3.6 setup_hw.py build_ext --inplace\n'
fp = open('setup.sh','w')
fp.write(text)
fp.close()


#print out tree
if verbose:
    print ('----------------------------------')

shutil.copyfile(lib_c,os.path.os.path.basename(lib_c))
shutil.copyfile(lib_h,os.path.os.path.basename(lib_h))
shutil.copyfile(lib_py,os.path.os.path.basename(lib_py))
shutil.copyfile(pynqlib_c,os.path.os.path.basename(pynqlib_c))
shutil.copyfile(pynqlib_h,os.path.os.path.basename(pynqlib_h))
# Done!

