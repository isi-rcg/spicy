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


import sys
import ast

from astLib import *

#############################################################
# passes.py
#    Contains AST pass routines and custom AST class to
#    represent a statement
#
#############################################################
                
###################
# Convert_Module
#   Add Stmt node to nodes in AST.Module
def convert_Module(node):
    for i in range(len(node.body)):
        n = node.body[i]
        if isinstance(n,ast.Assign):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        elif isinstance(n,ast.AugAssign):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        elif isinstance(n,ast.AnnAssign):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        elif isinstance(n,ast.Call):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        elif isinstance(n,ast.Continue):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        else:
            first_pass(n)

###################
# convert_Expr
#   Add Stmt node to nodes in AST.Expr
def convert_Expr(node):
    if isinstance(node.value,ast.Call):
        new = Stmt()
        new.body = node.value
        new.lineno = node.lineno
        node.value = new
    else:
        first_pass(node.value)


###################
# convert_Body
#   Add Stmt node to nodes in AST node with 'body' field
def convert_Body(node):
    for i in range(len(node.body)):
        n = node.body[i]
        if isinstance(n,ast.Assign):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.body[i] = new
        elif isinstance(n,ast.AugAssign):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.body[i] = new
        elif isinstance(n,ast.AnnAssign):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.body[i] = new
        elif isinstance(n,ast.Call):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.body[i] = new
        elif isinstance(n,ast.Continue):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        elif isinstance(n,ast.Return):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.body[i] = new
        else:
            first_pass(n)


###################
# convert_Orelse
#   Add Stmt node to nodes in AST node with 'orelse' field
def convert_Orelse(node):
    for i in range(len(node.orelse)):
        n = node.orelse[i]
        if isinstance(n,ast.Assign):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.orelse[i] = new
        elif isinstance(n,ast.AugAssign):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.orelse[i] = new
        elif isinstance(n,ast.AnnAssign):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.orelse[i] = new
        elif isinstance(n,ast.Call):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.orelse[i] = new
        elif isinstance(n,ast.Continue):
            new = Stmt()
            new.body = n
            new.lineno = n.lineno
            node.body[i] = new
        elif isinstance(n,ast.Return):
            new = Stmt()
            new.body = n
            new.lineno = node.lineno
            node.orelse[i] = new
        else:
            first_pass(n)

###################
# first_pass
#   This parses the AST and inserts Stmt nodes as needed
def first_pass(node):
    if isinstance(node,ast.Module):
        convert_Module(node)
    elif isinstance(node,ast.If):
        convert_Body(node)
        convert_Orelse(node)
    elif isinstance(node,ast.FunctionDef):
        convert_Body(node)
    elif isinstance(node,ast.For):
        convert_Body(node)
    elif isinstance(node,ast.While):
        convert_Body(node)
    elif isinstance(node,ast.Expr):
        convert_Expr(node)
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    first_pass(n)
            elif isinstance(value,ast.AST):
                first_pass(value)

###################
# check_Body
#   this function looks for docstrings (ie. Expr(Str)) in the body of a node
#   and removes the Expr() from the body and sets the docstring attribute of
#   the previous node with the Str
def check_Body(node):
    i = 0
    while i < len(node.body):
        if isinstance(node.body[i],ast.Expr) and isinstance(node.body[i].value,ast.Str):
            #check if next node is funcdef,for
            if i == 0:
                string = node.body[i].value.s
                node.docstring = string
                del node.body[i]
            else:
                string = node.body[i].value.s
                if hasattr(node.body[i-1],'docstring'):
                    node.body[i-1].docstring += string
                else:
                    node.body[i-1].docstring = string
                del node.body[i]
        else:
            i+=1
    

###################
# second_pass
#   This parses the AST and sets the docstring attribute
#   as necessary
def second_pass(node):
    if isinstance(node,ast.Module):
        check_Body(node)
    elif isinstance(node,ast.FunctionDef):
        check_Body(node)
    elif isinstance(node,ast.For):
        check_Body(node)
    elif isinstance(node,ast.While):
        check_Body(node)

    for name,value in ast.iter_fields(node):
        if isinstance(value,list):
            for n in value:
                second_pass(n)
        elif isinstance(value,ast.AST):
            second_pass(value)

###################
# third_pass
#   This parses the AST and renames ndarry() to init_contiguous_ndarray()
def third_pass(node):
    if isinstance(node,ast.Attribute):
        node.attr = 'init_contiguous_ndarray'
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    third_pass(n)
            elif isinstance(value,ast.AST):
                third_pass(value)
                
###################
# fourth_pass
#   This parses the AST and converts ndarray() and np.array() args to keywords
def fourth_pass(node):
    if isinstance(node,ast.Call) and isinstance(node.func,ast.Attribute):
        if node.func.attr == 'ndarray':
            process_ndarray_args(node)
        elif node.func.attr == 'array':
            process_array_args(node)
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    fourth_pass(n)
            elif isinstance(value,ast.AST):
                fourth_pass(value)

###################
# fifth_pass
#   This parses the AST and gets the list of functions that this function calls
def fifth_pass(node,callees):
    if isinstance(node,ast.Call) and isinstance(node.func,ast.Name) and \
            (node.func.id != 'range' and node.func.id != 'len'):
        callees.append(node.func.id)
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    fifth_pass(n,callees)
            elif isinstance(value,ast.AST):
                fifth_pass(value,callees)

def hasAccel(node):
    if isinstance(node.body[0],ast.Expr) and isinstance(node.body[0].value,ast.Str):
        string = node.body[0].value.s
        if hasAccPragma(string):
            return True
    return False
    

def _findAcc(node,funcs):
    if isinstance(node,ast.FunctionDef) and hasAccel(node):
        funcs.append(node)
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    _findAcc(n,funcs)
            elif isinstance(value,ast.AST):
                _findAcc(value,funcs)


def findAcc(node):
    funcs = []
    _findAcc(node,funcs)
    return funcs

###################
# function_pass
#   This parses the AST and returns the sub-tree with
#   a particular function as the root
def function_pass(node,func):
    if isinstance(node,ast.FunctionDef) and node.name == func:
        return node
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    ret = function_pass(n,func)
                    if ret != None:
                        return ret
            elif isinstance(value,ast.AST):
                return function_pass(value,func)

###################
# findCallers
#   Entry point to find callers of specified function
#   returns list of lineno's containing callsites
def findCallers(node,func):
    l = []
    _findCallers(node,func,l)
    return l

###################
# _findCallers
#   Recursive function to search AST for callers of specified function
def _findCallers(node,func,l):
    if isinstance(node,ast.Call) and isinstance(node.func,ast.Name):
        if node.func.id == func:
            l.append(node.lineno)
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    _findCallers(n,func,l)
            elif isinstance(value,ast.AST):
                _findCallers(value,func,l)

###################
# wasModified
#    check if var was modified in node scope
def wasModified(node,var):
    if isinstance(node,ast.Assign):
        for target in node.targets:
            if target.id == var:
                return True
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    ret = wasModified(n,var)
                    if ret:
                        return ret
            elif isinstance(value,ast.AST):
                ret = wasModified(value,var)
                if ret:
                    return ret
    return False

###############
# wasRead
#   check if var was read in node scope
def wasRead(node,var):
    if isinstance(node,ast.Assign):
        ret = wasRead(node.value,var)
        if ret:
            return ret
    elif isinstance(node,ast.Name) and node.id == var:
        return True
    elif isinstance(node,ast.Return):
        ret = wasRead(node.value,var)
        if ret:
            return ret
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    ret = wasRead(n,var)
                    if ret:
                        return ret
            elif isinstance(value,ast.AST):
                ret = wasRead(value,var)
                if ret:
                    return ret
    return False

##############
# getCalls
#    find calls to functions
def getCalls(node):
    return _getCalls(node,[])

def _getCalls(node,funcs):
    if isinstance(node,ast.Call):
        funcs.append(node)
        return funcs
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    funcs = _getCalls(n,funcs)
            elif isinstance(value,ast.AST):
                funcs = _getCalls(value,funcs)
        return funcs

def getCallStack(node):
    top = CallHier
    _getCallStack(node,top)

def _getCallStack(node,call):
    if isinstance(node,ast.Call):
        call.calls.append(node)
        return funcs
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    funcs = _getCalls(n,funcs)
            elif isinstance(value,ast.AST):
                funcs = _getCalls(value,funcs)
        return funcs
    

##############
# getFuncs
#    find all function declarations
def getFuncs(node):
    return _getFuncs(node,[])

def _getFuncs(node,funcs):
    if isinstance(node,ast.FunctionDef):
        funcs.append(node)
        return funcs
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    funcs = _getFuncs(n,funcs)
            elif isinstance(value,ast.AST):
                funcs = _getFuncs(value,funcs)
        return funcs

def get_names(node,found):
    if isinstance(node,ast.Name):
        found.append(node.id)        
    else:
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    get_names(n,found)
            elif isinstance(value,ast.AST):
                get_names(value,found)
    
def get_defuses(node,defs,uses):
    for u in node.vuse:
        uses.append(u)
    for d in node.vdef:
        defs.append(d)
    for c in node.children:
        get_defuses(c,defs,uses)



def defuse_pass(node):
    if not isinstance(node, ast.Module):
        raise TypeError('expected AST.Module, got %r' % node.__class__.__name__)
    if len(node.body) > 0:
        root = DefUse(node)
        tmp = root
        for b in node.body:
            next = DefUse(b)
            _defuse_pass(next)
            if isinstance(b,ast.FunctionDef) or isinstance(tmp.node,ast.FunctionDef):
                root.addChild(next)
            else:
                tmp.addChild(next)
            tmp = next
        return root

def _defuse_pass(node):
    #check for def
    if isinstance(node.node,ast.Assign):
        targets = []
        for t in node.node.targets:
            get_names(t,targets)
        for t in targets:
            node.addDef(t)
        
        if isinstance(node.node.value,ast.Name):
            node.addUse(node.node.value.id)
        else:
            new = DefUse(node.node.value)
            node.addChild(new)
            _defuse_pass(new)
    elif isinstance(node.node,ast.Call):
        node.addUse(node.node.func.id)
        for arg in node.node.args:
            if isinstance(arg,ast.Name):
                node.addUse(arg.id)
            else:
                new = DefUse(arg)
                node.addChild(new)
                _defuse_pass(new)
    elif isinstance(node.node,ast.FunctionDef):
        for name,value in ast.iter_fields(node.node):
            if isinstance(value,list):
                for n in value:
                    new = DefUse(n)
                    node.addChild(new)
                    _defuse_pass(new)
            elif isinstance(value,ast.AST):
                new = DefUse(value)
                node.addChild(new)
                _defuse_pass(new)        
        defs = []
        uses = []
        get_defuses(node,defs,uses)
        node.defs = defs
        node.uses = uses
    else:
        for name,value in ast.iter_fields(node.node):
            if isinstance(value,list):
                for n in value:
                    if isinstance(n,ast.Name):
                        node.addUse(n.id)
                    else:
                        new = DefUse(n)
                        node.addChild(new)
                        _defuse_pass(new)
            elif isinstance(value,ast.Store) or isinstance(value,ast.Load):
                pass
            elif isinstance(value,ast.AST):
                if isinstance(value,ast.Name):
                    node.addUse(value.id)
                else:
                    new = DefUse(value)
                    node.addChild(new)
                    _defuse_pass(new)
