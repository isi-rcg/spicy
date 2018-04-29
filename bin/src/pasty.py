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
import astpp

from passes import *
from astLib import *

#######################
# PastY
#   A new class object to represent nodes of the AST
#   main usage is string field which hold string 
#   representation of C/C++ syntax
class PastY:
    def __init__(self,init_str=''):
        self.val = ''
        self.type = ''
        self.node = ''
        self.string = init_str
        self.scope = Scope()
        self.parent = ''
        self.indent = ''
        self.num_spaces = 3
        self.lineno = -1
        self.verbose = False

    def __repr__(self):
        return self.string

    def __str__(self):
        return self.string

    def incIndent(self):
        string = ''
        for i in range(self.num_spaces):
            string += ' '
        self.indent += string

    def decIndent(self):
        self.indent = self.indent[0:-self.num_spaces]

    #######################
    # set
    #    Main entry to converting Python AST 
    def set(self,node):
        new = self.process(node)
        self.val = new.val
        self.type = new.type
        self.string = new.string

    def printFields(self,node):
        for name,value in ast.iter_fields(node):
            print ("\t%s ==> %s" % (name,repr(value)))

    #######################
    # process
    #    Processes an AST node and produces a PastY node with 
    #    C/C++ string representation
    def process(self,node):
        if self.verbose:
            print ("Processing %s" % getClname(node))
            print(astpp.dump(node))
            if hasattr(node,'lineno'):
                print('\tlineno: %s' % str(node.lineno))
            if hasattr(node,'docstring'):
                print('\tdocstring: %s' % node.docstring)
            self.printFields(node)

        #Process each type of AST node appropriately
        if isinstance(node,ast.Pass):
            res =  self.process_Pass(node)
        elif isinstance(node,ast.Num):
            res =  self.process_Int(node)
        elif isinstance(node,ast.Name):
            res =  self.process_Name(node)
        elif isinstance(node,ast.Continue):
            res =  self.process_Continue(node)
        elif isinstance(node,ast.Expr):
            res =  self.process_Expr(node)
        elif isinstance(node,ast.Assign):
            res =  self.process_Assign(node)
        elif isinstance(node,ast.AugAssign):
            res =  self.process_AugAssign(node)
        elif isinstance(node,ast.AnnAssign):
            res =  self.process_AnnAssign(node)
        elif isinstance(node,ast.BinOp):
            res =  self.process_BinOp(node)
        elif isinstance(node,ast.UnaryOp):
            res =  self.process_UnaryOp(node)
        elif isinstance(node,ast.BoolOp):
            res =  self.process_BoolOp(node)
        elif isinstance(node,ast.Call):
            res =  self.process_Call(node)
        elif isinstance(node,ast.Attribute):
            res =  self.process_Attribute(node)
        elif isinstance(node,ast.Subscript):
            res =  self.process_Subscript(node)
        elif isinstance(node,ast.List):
            res =  self.process_List(node)
        elif isinstance(node,ast.If):
            res =  self.process_If(node)
        elif isinstance(node,ast.For):
            res =  self.process_For(node)
        elif isinstance(node,ast.While):
            res =  self.process_While(node)
        elif isinstance(node,Stmt):
            res =  self.process_Stmt(node)
        elif isinstance(node,ast.Return):
            res =  self.process_Return(node)
        elif isinstance(node,ast.Compare):
            res =  self.process_Compare(node)
        elif isinstance(node,ast.arg):
            res =  self.process_arg(node)
        elif isinstance(node,ast.FunctionDef):
            res =  self.process_FunctionDef(node)
        elif isinstance(node,ast.Module):
            res =  self.process_Module(node)
        else: 
            print ('Error! Unsupported AST node "%s"' % node.__class__.__name__)
            sys.exit(1)

        if self.verbose:
            self.scope.display()
            print('PastY.string = ',res.string)
        return res
    
    #######################
    # process_CmpOp
    #    Processes a comparator operation into a string
    def process_CmpOp(self,node):
        new = PastY()
        if isinstance(node,ast.Eq):
            new.string = '=='
        elif isinstance(node,ast.NotEq):
            new.string = '!='
        elif isinstance(node,ast.Lt):
            new.string = '<'
        elif isinstance(node,ast.LtE):
            new.string = '<='
        elif isinstance(node,ast.Gt):
            new.string = '>'
        elif isinstance(node,ast.GtE):
            new.string = '>='

        #Unsupported: Is | IsNot | In | NotIn
        else:
            print("Unsupported compare operator: '%s'" % getClname(node))
            sys.exit(1)
        return new

    #######################
    # process_Op
    #    Processes an arithmetic operation into a string
    def process_Op(self,node):
        new = PastY()
        if isinstance(node,ast.Add):
            new.string = '+'
        elif isinstance(node,ast.Sub):
            new.string = '-'
        elif isinstance(node,ast.Mult):
            new.string = '*'
        elif isinstance(node,ast.Div):
            new.string = '/'
        elif isinstance(node,ast.Mod):
            new.string = '%'
        elif isinstance(node,ast.LShift):
            new.string = '<<'
        elif isinstance(node,ast.RShift):
            new.string = '>>'
        elif isinstance(node,ast.BitOr):
            new.string = '|'
        elif isinstance(node,ast.BitXor):
            new.string = '^'
        elif isinstance(node,ast.BitAnd):
            new.string = '&'

        # Unsupported: MatMult | FloorDiv | Pow | 
        else:
            print('Error! Unsupport operator: %s' % getClname(node))
            sys.exit(1)
        return new

    #######################
    # process_UnaryOperator
    #    Processes a unary operator into a string
    def process_UnaryOperator(self,node):
        new = PastY()
        if isinstance(node,ast.USub):
            new.string = '-'
        elif isinstance(node,ast.UAdd):
            new.string = '+'
        elif isinstance(node,ast.Not):
            new.string = '~'
        elif isinstance(node,ast.Invert):
            new.string = '~'
        else:
            print('Error! Unknown Unary Op: %s' % (getClname(node)))
            exit(1)
        return new

    #######################
    # process_UnaryOperator
    #    Processes a boolean operator into a string
    def process_BooleanOperator(self,node):
        new = PastY()
        if isinstance(node,ast.Or):
            new.string = '||'
        elif isinstance(node,ast.And):
            new.string = '&&'
        else:
            print('Error! Unsupported bool op: %s' % (getClname(node)))
            exit(1)
        return new
            
    #######################
    # process_BinOp
    #    Processes a binary expression into a string
    def process_BinOp(self,node):
        new = PastY()
        left = self.process(node.left)
        op = self.process_Op(node.op)
        right = self.process(node.right)
        new.string = '(' + left.string + op.string + right.string + ')'
        return new

    #######################
    # process_UnaryOp
    #    Processes a unary expression into a string
    def process_UnaryOp(self,node):
        new = PastY()
        operand = self.process(node.operand)
        op = self.process_UnaryOperator(node.op)
        new.string = '(' + op.string + operand.string + ')'
        return new

    
    #######################
    # process_BoolOp
    #    Processes a binary expression into a string
    def process_BoolOp(self,node):
        new = PastY()
        op = self.process_BooleanOperator(node.op)
        new.string = '(' + self.process(node.values[0]).string
        for v in node.values[1:]:
            new.string += ' ' + op.string + ' ' + self.process(v).string
        new.string += ')'
        return new

    #######################
    # process_Expr
    #    Processes an expression into a string
    def process_Expr(self,node):
        new = PastY()
        value = self.process(node.value)
        new.string = value.string
        return new

    #######################
    # process_Compare
    #    Processes a compare expression into a string
    def process_Compare(self,node):
        new = PastY()
        left = self.process(node.left)
        op = self.process_CmpOp(node.ops[0])
        right = self.process(node.comparators[0])
        new.string = left.string + op.string + right.string
        return new

    #######################
    # process_Subscript
    #    Processes a subscript expression into a string
    def process_Subscript(self,node):
        new = PastY()
        #ndarray.shape support
        if isinstance(node.value,ast.Attribute) and node.value.attr == 'shape' and self.scope.has(node.value.value.id):
            ty = self.scope.get(node.value.value.id)
            if node.slice.value.n >= len(ty.dimensions):
                print('Error! ndarray does not have %sth dimension' % (str(node.slice.value.n)))
                print(astpp.dump(node))
                sys.exit(1)
            new.string = str(ty.dimensions[node.slice.value.n])
        elif not isinstance(node.value,ast.Subscript):
            slice = self.process(node.slice.value)
            id = node.value.id
            new.name = id
            new.string = id + '[' + slice.string + ']'   
        else:
            id,string = self._rec_Subscript(node,0)
            new.name = id
            new.string = id + '[' + string + ']'
        return new


    def _mult(self,vals):
        prod=1
        for v in vals:
            prod *= v
        return prod

    def _rec_Subscript(self,node,depth):
        if isinstance(node.value,ast.Subscript):
            id,string = self._rec_Subscript(node.value,depth+1)
            slice = self.process(node.slice.value)
            if depth == 0:
                string += '+' + slice.string
            else:
                ty = self.scope.get(id)
                if len(ty.dimensions) <= depth:
                    print('Error! Only has %d dimensions (%d)' % (len(ty.dimensions),depth))
                    print(ty)
                    exit(1)
                string += '+' '(' + slice.string + ')' + '*' + str(ty.dimensions[-depth])
            return (id,string)
        else:
            ty = self.scope.get(node.value.id)
            if len(ty.dimensions) <= depth:
                print('Error! Only has %d dimensions (%d)' % (len(ty.dimensions),depth))
                print(ty)
                exit(1)
            slice = self.process(node.slice.value)
            string = '(' + slice.string + ')' + '*' + str(self._mult(ty.dimensions[-depth:]))
            return (node.value.id,string)

    #######################
    # process_If
    #    Processes an if statement into a string
    def process_If(self,node):
        self.scope.push()
        new = PastY()
        test = self.process(node.test)
        body = []
        body_str = ''
        self.incIndent()
        for n in node.body:
            b = self.process(n)
            body.append(b)
            body_str += '%s\n' % b.string
        self.decIndent()

        new.string = '%sif(%s) {\n%s%s}' % (self.indent,test.string,body_str,self.indent)
        self.scope.pop()

        if len(node.orelse) > 0:
            #process else
            new.string += ' else {\n'
            self.incIndent()
            for n in node.orelse:
                eb = self.process(n)
                new.string += eb.string + '\n'
            self.decIndent()
            new.string += '%s}' % self.indent
        return new

    #######################
    # process_Return
    #    Processes a return statement into a string
    def process_Return(self,node):
        new = PastY()
        expr = self.process(node.value)
        new.string = 'return %s' % expr.string
        return new

    #######################
    # process_ForIter
    #    Processes an iter expression (part of a for loop decl) into a string
    #    expression is processing arguments of range() function
    def process_ForIter(self,target,iter):
        new = PastY()
        #range(x)
        if len(iter.args) == 1:
            start = PastY('0')
            stop = self.process(iter.args[0])
            incr = PastY('1')
        #range(x,y)
        elif len(iter.args) == 2:
            start = self.process(iter.args[0])
            stop = self.process(iter.args[1])
            incr = PastY('1')
        #range(x,y,z)
        elif len(iter.args) == 3:
            start = self.process(iter.args[0])
            stop = self.process(iter.args[1])
            incr = self.process(iter.args[2])
        else:
            print('Error! Only one, two, or three range argument(s) supported')
            sys.exit(1)
        new.string = "%s %s=%s; %s%s(int)%s; %s=%s+%s" % (target.type,target.string,start.string,target.string,'<',stop.string,target.string,target.string,incr.string)
        return new

    #######################
    # process_For
    #    Processes a for loop into a string
    def process_For(self,node):
        self.scope.push()
        new = PastY()
        target = self.process(node.target)
        iter = node.iter
        if not(isinstance(iter,ast.Call)):
            print('Error! Only For loops with range() supported')
            sys.exit(1)
        elif iter.func.id != 'range':
            print('Error! Only For loops with range() supported')
            sys.exit(1)
        iter = self.process_ForIter(target,iter)
            
        body = []
        body_str = ''
        if hasattr(node,'docstring'):
            body_str += getHLSpragmas(node.docstring)
        #body of loop
        self.incIndent()
        for i in range(len(node.body)):
            n = node.body[i]
            b = self.process(n)
            body.append(b)
            body_str += "%s\n" % (b.string)
        self.decIndent()
        new.string = "%sfor(%s) {\n%s%s}" % (self.indent,iter.string,body_str,self.indent)
        self.scope.pop()
        return new

    #######################
    # process_While
    #    Processes a while loop into a string
    def process_While(self,node):
        self.scope.push()
        new = PastY()
        test = self.process(node.test)
        body = []
        body_str = ''
        self.incIndent()
        for i in range(len(node.body)):
            n = node.body[i]
            b = self.process(n)
            body.append(b)
            body_str += "%s\n" % b.string
        self.decIndent()
        new.string = '%swhile(%s) {\n%s%s}' % (self.indent,test.string,body_str,self.indent)
        self.scope.pop()
        return new

    #######################
    # process_Call
    #    Processes a function call into a string
    def process_Call(self,node):
        new = PastY()
        args = []
        arg_str = ''
        #create arg string
        for i in range(len(node.args)):
            arg = node.args[i]
            a = self.process(arg)
            args.append(a)
            if i > 0:
                arg_str += ','
            arg_str += a.string
        
        if isinstance(node.func,ast.Name):
            name = self.process(node.func)
            if name.string == 'len':
                if isinstance(node.args[0],ast.Name):
                    var = node.args[0].id
                    if self.scope.has(var):
                        ty = self.scope.get(var)
                        if ty.category == 'list':
                            new.string = '(' + var + '_front - ' + var + '_back)'
                        else:
                            new.string = ty.dimensions[0]
                    else:
                        print('Error! var %s is not defined yet' % var)
                        sys.exit(1)
                else:
                    print('Error! len() only takes 1 argument: Name')
                    sys.exit(1)
            else:
                new.string = name.string + '(' + arg_str + ')'
        elif isinstance(node.func,ast.Attribute):
            name = self.process(node.func.value)
            if name.string == 'np':
                if node.func.attr == 'power':
                    new.string = 'pow' + '(' + arg_str + ')'
                elif node.func.attr == 'sqrt':
                    new.string = 'sqrt' + '(' + arg_str + ')'
                elif node.func.attr == 'sqrtf':
                    new.string = 'sqrtf' + '(' + arg_str + ')'
                elif node.func.attr == 'arctan2':
                    new.string = 'atan2' + '(' + arg_str + ')'
                elif node.func.attr == 'arctan2f':
                    new.string = 'atan2f' + '(' + arg_str + ')'
                elif node.func.attr == 'round':
                    new.string = 'round' + '(' + arg_str + ')'
                elif node.func.attr == 'pi':
                    new.string = '3.14159265358979323846'                
                else:
                    if isinstance(node.func.value,ast.Name):
                        s = node.func.value.id
                    else:
                        s = str(node.func.value)
                    print('Error! Unsupported Numpy Attribute: %s' % (s+"."+str(node.func.attr)))
                    exit(1)
            elif self.scope.has(name.string):
                argty = self.scope.get(name.string)
                if argty.category == 'list':
                    if node.func.attr == 'append':
                        new.string = name.string + '[' + name.string + '_front++] = (' + arg_str + ')'                        
                    elif node.func.attr == 'pop':
                        new.string = name.string + '[' + name.string + '_front-- -1]'
                    else:
                        print('Error! Unsupported Attribute on List: %s' % (str(node.value)+"."+str(node.attr)))
                        exit(1)
                else:
                    print('Error! Unsupported call for list attribute %s.%s' % (name,node.func.attr))
                    print(ast.dump(node))
                    exit(1)
            else:
                print('Error! Unsupposted call for attribute %s.%s' % (name,node.func.attr))
                print(ast.dump(node))
                exit(1)
        else:
            print('Error! Unsupported call')
            print(ast.dump(node))
            exit(1)
        return new

    #######################
    # process_arg
    #    Processes a function argument into a string
    def process_arg(self,node):
        new = PastY()

        new.string = node.arg
        argty = getArg(node)
        new.type = argty
        return new

    #######################
    # process_List
    #    Processes a list into a string
    def process_List(self,node):
        new = PastY()

        elts_str = ''
        if len(node.elts) > 0:
            elts_str = self.process(node.elts[0]).string
        for e in node.elts[1:]:
            elts_str += ',' + self.process(e).string
        if len(elts_str) > 0:
            new.string = '{' + elts_str + '}'
        return new

    #######################
    # process_FunctionDef
    #    Processes a function definition into a string
    def process_FunctionDef(self,node):
        self.scope.push()
        new = PastY()
        ret_type,name,args = getFuncDecl(node)
        name = node.name
        #self.scope.putGlobal(name,ret_type)
        args = []
        arg_str = ''
        for i in range(len(node.args.args)):
            arg = node.args.args[i]
            a = self.process(arg)
            self.scope.put(a.string,a.type)
            args.append(a)
            if i > 0:
                arg_str += ','
            arg_str += a.type.getDecl()
        body = []
        body_str = ''
        if hasattr(node,'docstring'):
            body_str += getHLSpragmas(node.docstring)
        self.incIndent()
        for i in range(len(node.body)):
            n = node.body[i]
            b = self.process(n)
            body.append(b)
            body_str += "%s\n" % (b.string)
        self.decIndent()
        new.string = ''
        if hasattr(node,'docstring'):
            new.string += getSDSpragmas(node.docstring)
        new.string += "%s %s(%s) {\n%s}" % (ret_type.getType(),name,arg_str,body_str)
        self.scope.pop()
        return new

    #######################
    # process_Module
    #    Processes a module node's body (list of functions)
    def process_Module(self,node):
        new = PastY()
        new.val = []
        for n in node.body:
            child =self.process(n)
            #statements in a top level Module get finished with \n
            child.string += '\n'
            new.string += child.string
            new.val.append(child)
        return new

    #######################
    # process_Name
    #    Processes a name into a string
    def process_Name(self,node):
        new = PastY()
        new.string = node.id
        new.name = node.id
        new.type = 'int'
        return new

    #######################
    # process_Stmt
    #    Processes a statement into a string
    def process_Stmt(self,node):
        new = PastY()
        new.node = 'Stmt'
        body = self.process(node.body)
        new.string = "%s%s;" % (self.indent,body.string)
        if hasattr(node.body,'docstring'):
            new.string += '\n'+getHLSpragmas(node.body.docstring)

        return new

    #######################
    # process_AugAssign
    #    Processes an augment assign (ie. +=)  into a string
    def process_AugAssign(self,node):
        new = PastY()
        target = self.process(node.target)
        value = self.process(node.value)
        op = self.process_Op(node.op)
        new.string = target.string + op.string + '=' + value.string
        return new

    #######################
    # process_AnnAssign
    #    Processes an annotated assign into a string
    def process_AnnAssign(self,node):
        new = PastY()
        target = self.process(node.target)
        if node.value != None:
            value = self.process(node.value)
        argty = getType(node)
        argty.name = target.string
        new.type = argty
        new.string = argty.getDecl()
        if node.value != None and len(str(value.string)) > 0:
            new.string += '=' + str(value.string)
        if argty.category == 'list':
        #setup front/back pointers
            new.string += ';\n' + self.indent + 'unsigned ' + target.string + '_front = 0'
            new.string += ';\n' + self.indent + 'unsigned ' + target.string + '_back = 0'

        self.scope.put(target.string,argty)

        return new

    #######################
    # process_Attribute
    #    Processes an attribute into a string
    def process_Attribute(self,node):
        new = PastY()
        #value=Name(id='np', ctx=Load()), attr='power'
        if isinstance(node.value,ast.Name):
            if node.value.id == 'np':
                if node.attr == 'power':
                    new.string = 'pow'
                elif node.attr == 'arctan2':
                    new.string = 'atan2'
                elif node.attr == 'round':
                    new.string = 'round'
                elif node.attr == 'pi':
                    new.string = '3.14159265358979323846'
                else:
                    print('Error! Unsupported Numpy Attribute: %s' % (str(node.value)+"."+str(node.attr)))
                    exit(1)
            elif self.scope.has(node.value.id):
                var = self.scope.get(node.value.id)
                if var.category == 'list':
                    if node.attr == 'append':
                        new.string = node.value.id + '[' + node.value.id + '_front++] = '
                    elif node.attr == 'pop':
                        new.string = node.value.id + '[' + node.value.id + '_front-- -1]'
                    else:
                        print('Error! Unsupported Attribute on List: %s' % (str(node.value)+"."+str(node.attr)))
                        exit(1)
                elif var.category == 'array':
                    if node.attr == 'shape':
                        dim = 1

            else:
                print('Error! Unsupported Attribute: %s' % (str(node.value.id)+"."+str(node.attr)))
                exit(1)
        else:
            print('Error! Unsupported Attribute: %s' % (str(node.value)+"."+str(node.attr)))
            exit(1)
        return new
            
    #######################
    # process_ListSize
    #    Processes the list size (multi-dimensional) declaration into a string
    def process_ListSize(self,node):
        sizes = []
        n = node
        #loop through each dimension
        while True:
            #get the first element in the dimension
            x = n.val[0]
            #get if its a list (another dimension)
            if isinstance(x.val,list):
                sizes.append(len(n.val))
                n = x
            else:
                #stop when its not a list anymore
                sizes.append(len(n.val))
                break
        string = ''
        for i in range(len(sizes)):
            s = sizes[i]
            string += '['+str(s)+']'
        return string


    ######################
    # process_array_string
    #    processes a list from a Numpy Array() object keyword 
    #    into a single dimensional string of elements
    def process_array_string(self,node):
        string = ''
        if isinstance(node,ast.List):
            for e in node.elts:
                oth = self.process_array_string(e)
                string += oth+','
        else:
            new = self.process(node)
            return new.string
        return string[0:-1]

    #######################
    # process_Assign
    #    Processes an assignment into a string
    def process_Assign(self,node):
        new = PastY()
        target = self.process(node.targets[0])
        if isinstance(node.value,ast.Call) and isinstance(node.value.func,ast.Attribute) and (
            node.value.func.attr == 'ndarray' or node.value.func.attr == 'array'):
            if node.value.func.attr == 'ndarray':
                argty = process_ndarray(node.value)
                new.string = '%s %s[%s]' % (processType(argty),target.string,argty.size())
                self.scope.put(target.string,argty)
            elif node.value.func.attr == 'array':
                argty,keyword = process_array(node.value)
                val = '{'+self.process_array_string(keyword.value)+'}'
                new.string = '%s %s[%s] = %s' % (processType(argty),target.string,argty.size(),val)
                self.scope.put(target.string,argty)
            else:
                print('Error! Unsupported assignment')
                exit(1)
        else:
            value = self.process(node.value)
            if self.scope.has(target.name):
                new.string = "%s = %s" % (target.string,value.string)
            else:
                self.scope.put(target.name,value.type)
                if isinstance(value.val,list):
                    size = self.process_ListSize(value)
                    new.string = '%s %s%s = %s' % (target.type,target.string,size,value.string)
                else:
                    new.string = "%s %s = %s" % (target.type,target.string,value.string)
        return new

    #######################
    # process_Int
    #    Processes an integer literal into a string
    def process_Int(self,node):
        new = PastY()
        new.val = node.n
        new.string = str(new.val)
        return new

    #######################
    # process_Continue
    #    Processes a continue statement
    def process_Continue(self,node):
        new = PastY()
        new.string = 'continue'
        return new

    #######################
    # process_Pass
    #    Processes a pass statement
    def process_Pass(self,node):
        new = PastY()
        new.string = ''
        return new
