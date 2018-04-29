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
# Tool for rewriting Python apps to call accelerators
#
#############################################################################################

import re
import sys
import ast
import argparse

from passes import *


##############################################
# Main code
##############################################
parser = argparse.ArgumentParser()
parser.add_argument('-verbose',action='store_true',help='print all messages')
parser.add_argument('-o',metavar='<code.cpp>',help='output c code file')
parser.add_argument('-mod',metavar='<name>',help='module name')
parser.add_argument('-func',metavar='<name>',help='only process this function')
parser.add_argument('file_py',help='file containing python function to generate a wrapper for')

args = parser.parse_args()

#process arguments
if args.verbose:
    verbose = True
else:
    verbose = False

if args.o:
    outfile = args.o
else:
    outfile = 'out.py'

if args.mod:
    mod = args.mod
else:
    mod = 'c_module'

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

    #replace call in old code to wrapper function
    callsites = findCallers(tree,func)
    lines = code.split('\n')
    for call in callsites:
        lines[call-1] = re.sub('('+func+')',mod+'.'+func+'_wrapper',lines[call-1])
    newcode = '\n'.join(lines)
    newcode = 'import ' + mod + '\n' + newcode

else:
    lines = code.split('\n')
    func = findAcc(tree)

    for f in func:
        callsites = findCallers(tree,f.name)
        for call in callsites:
            lines[call-1] = re.sub('('+f.name+')',mod+'.'+f.name+'_wrapper',lines[call-1])

    newcode = '\n'.join(lines)
    newcode = 'import ' + mod + '\n' + newcode


#write out rewritten code
fp = open(outfile,'w')
fp.write(newcode)
fp.close()

#print out tree
if verbose:
    print ('----------------------------------')

# Done!
