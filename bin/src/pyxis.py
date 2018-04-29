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


import os
import sys
import ast
import argparse
import shutil

from passes import *
from astLib import *


##############################################
# Main code
##############################################
num_spaces = 3


loc = os.path.dirname(os.path.dirname(os.path.dirname(os.path.realpath(sys.argv[0]))))
lib_c = os.path.join(loc,'lib','spyc.cpp')
lib_h = os.path.join(loc,'lib','spyc.h')

parser = argparse.ArgumentParser()
parser.add_argument('-verbose',action='store_true',help='print all messages')
#parser.add_argument('-func',metavar='<name>',help='only process this function',required=True)
parser.add_argument('file_py',help='file containing python function to generate a wrapper for')

args = parser.parse_args()

#process arguments
if args.verbose:
    verbose = True
else:
    verbose = False

#func = args.func

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

funcs = getFuncs(tree)
if verbose:
    print('---Found Functions-------------------')
    for func in funcs:
        print('%s args:' % (func.name))
        for arg in func.args.args:
            print('\t%s read: %s  written: %s' % (arg.arg,wasRead(func,arg.arg),wasModified(func,arg.arg)))

calls = getCalls(tree)
if verbose:
    print('---Found Function Calls--------------')
    for call in calls:
        print('%s' % (call.func.id))
# Done!

