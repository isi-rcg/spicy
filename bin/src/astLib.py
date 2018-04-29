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
# astLib.py
#
# Library of Python AST support functions
#
#############################################################################################

import re
import ast
import sys


######################
# Numpy type map table
#    format ==> numpy_data_type:[ctype,numpy_enum,py_cffi_format]
type_info = {'int8'   :['int8_t',  'NPY_INT8',   'B'],
             'int16'  :['int16_t', 'NPY_INT16',  'h'],
             'int32'  :['int32_t', 'NPY_INT32',  'i'],
             'int64'  :['int64_t', 'NPY_INT64',  'L'],
             'uint8'  :['uint8_t', 'NPY_UINT8',  'b'],
             'uint16' :['uint16_t','NPY_UINT16', 'H'],
             'uint32' :['uint32_t','NPY_UINT32', 'I'],
             'uint64' :['uint64_t','NPY_UINT64', 'K'],
             'float32':['float',   'NPY_FLOAT32','f'],
             'float'  :['double',  'NPY_FLOAT64','d'],
             'double' :['double',  'NPY_FLOAT64','d'],
             'void'   :['void',    'NPY_VOID',   '?'],
             }

######################
# Numpy ndarray argument keyword list
#    inorder argument keyword names
ndarray_arg_list = ['shape','dtype','buffer','offset','strides','order']

######################
# Numpy array argument keyword list
#    inorder argument keyword names
array_arg_list = ['object','dtype','copy','order','subok','ndmin']

######################
# getNPtype
#    get NumPy type enum for a given data type string
def getNPtype(argty):
    if argty.primitive in type_info:
        return type_info[argty.primitive][1]
    else:
        print('getNPtype unsupported primitive: %s' % argty.primitive)
        sys.exit(1)

######################
# formatType
#   get CFFI argparse format character for Python/C API wrapper generation
def formatType(argty):
    if argty.category == 'scalar':
        # if its a scalar, just look it up in the table
        if argty.primitive in type_info:
            return type_info[argty.primitive][2]
        else:
            print('Error! Unsupported type for format: %s' % argty.primitive)
            print('https://docs.python.org/2/c-api/arg.html')
            sys.exit(1)
    elif argty.category == 'array':
        # if its an array, just return the 'O'
        return 'O'
    else:
        print('Error! Unknown type category "%s"' % cat)
        sys.exit(1)
        
######################
# processType
#    get C/C++ type for a given data type string
def processType(argty):
    if argty.primitive in type_info:
        return type_info[argty.primitive][0]
    else:
        print('Error! Unsupported type: "%s" for argument "%s"' % (argty.primitive,argty.name))
        print('Supported types: %s' % list(type_info.keys()))
        sys.exit(1)

######################
# hasAccPragma
#    check if a given docstring contains the sPyC accelerator pragma
def hasAccPragma(string):
    if len(re.findall('#pragma spyc accelerate',string)) > 0:
        return True
    return False

######################
# getPragmas
#    get the pragma strings from a docstring
def getPragmas(string):
    found = re.findall('#pragma.*',string)
    return '\n'.join(found)+'\n'

######################
# getSDSpragmas
#    get the SDSoC pragma strings from a docstring
def getSDSpragmas(string):
    found = re.findall('#pragma SDS.*',string)
    return '\n'.join(found)+'\n'

######################
# getHLSpragmas
#    get the HLS pragma strings from a docstring
def getHLSpragmas(string):
    found = re.findall('#pragma HLS.*',string)
    return '\n'.join(found)+'\n'

######################
# process_ndarray_args
#    process the ndarray arguments to an array constructor
#      and set them as the appropriate keywords
def process_ndarray_args(node):
    for i in range(len(node.args)):
        arg = node.args[i]
        if i == 2:
            print('Error! ndarray() buffer argument not supported')
            sys.exit(1)

        k = ast.keyword()
        k.arg = ndarray_arg_list[i]
        k.value = arg
        node.keywords.append(k)
    node.args = []          

######################
# process_array_args
#    process the np.array() arguments to an array constructor
#      and set them as the appropriate keywords
def process_array_args(node):
    for i in range(len(node.args)):
        arg = node.args[i]
        if i == 2:
            print('Error! array() copy argument not supported')
            sys.exit(1)

        k = ast.keyword()
        k.arg = array_arg_list[i]
        k.value = arg
        node.keywords.append(k)
    node.args = []          

######################
# process_ndarray
#    process ndarray() arguments to a function
#      and check for correct annotation format
def process_ndarray(node):
    argty = ArgType()
    func = node.func

    if not(isinstance(func,ast.Attribute) and func.attr == 'ndarray'):
        print('Error! Invalid ndarray usage"')
        print('\tnumpy.ndarray(2,\'float\')')
        sys.exit(1)

    if len(node.args) > 0:
        print('Error! Internal Error: no ndarray args expected')
        sys.exit(1)

    kw = [0,0,0,0,0]
    for keyword in node.keywords:
        if keyword.arg == 'shape':
            kw[0]=1
            if isinstance(keyword.value,ast.Num):
                argty.dimensions.append(keyword.value.n)
            elif isinstance(keyword.value,ast.Tuple):
                for d in keyword.value.elts:
                    if isinstance(d,ast.Num):
                        argty.dimensions.append(d.n)
        elif keyword.arg == 'dtype':
            kw[1]=1
            if isinstance(keyword.value,ast.Str):
                argty.primitive = keyword.value.s
            elif isinstance(keyword.value,ast.Name) and (keyword.value.id == 'float' or keyword.value.id == 'double'):
                argty.primitive = keyword.value.id
            else:
                print('Error! ndarray dtype must be a String (you passed a %s)' % (getClname(keyword.value)))
                print(ast.dump(keyword))
                print('Example annotations:')
                print('\tvar:numpy.ndarray(2,\'float\')')
                sys.exit(1)
        else:
            print('Error! Unsupported ndarray keyword: "%s"' % keyword.arg)
            sys.exit(1)
            
    #check and make sure minimal arguments are specified
    if kw[0] == 0:
        print('Error! ndarray shape must be specified')
        print('Example shapes:')
        print('\t2     #any number for single dimension')
        print('\t(1,2) #any tuple for multiple dimensions')
        sys.exit(1)
    if kw[1] == 0:
        print('Error! ndarray dtype must be specified')
        print('Example type:')
        print('\tvar:numpy.ndarray(2,\'float\')')
        sys.exit(1)
        
    return argty

######################
# process_array_list
#    process an np.array(object) argument to extract the shape (dimensions)
def process_array_list(node):
    dim = []
    if isinstance(node,ast.List):
        size = len(node.elts)
        dim.append(size)
        if size > 0 and isinstance(node.elts[0],ast.List):
            oth = process_array_list(node.elts[0])
            for o in oth:
                dim.append(o)
    else:
        print('Error! process_array_list does not support class "%s"' % getClname(node))
        exit(1)
    return dim

######################
# process_array
#    process an np.array argument to a function
#      and check for correct annotation format
def process_array(node):
    argty = ArgType()
    func = node.func
    object_kw = None

    if not(isinstance(func,ast.Attribute) and func.attr == 'array'):
        print('Error! Invalid array usage"')
        print('\tnumpy.array([2],\'float\')')
        sys.exit(1)

    if len(node.args) > 0:
        print('Error! Internal Error: no np.array args expected')
        sys.exit(1)

    kw = [0,0,0,0,0]
    for keyword in node.keywords:
        if keyword.arg == 'object':
            object_kw = keyword
            kw[0]=1
            if isinstance(keyword.value,ast.List):
                argty.dimensions = process_array_list(keyword.value)
            else:                
                print('Error! Numpy Array object must be a list, found "%s"' % getClname(keyword.value))
                print('Supported usage:')
                print('\tnumpy.array([2],\'float\')')
                exit(1)
        elif keyword.arg == 'dtype':
            kw[1]=1
            if isinstance(keyword.value,ast.Str):
                argty.primitive = keyword.value.s
            elif isinstance(keyword.value,ast.Name) and (keyword.value.id == 'float' or keyword.value.id == 'double'):
                argty.primitive = keyword.value.id
            else:
                print('Error! Numpy Array dtype must be a String (you passed a %s)' % (getClname(keyword.value)))
                print(ast.dump(keyword))
                print('Example annotations:')
                print('\tvar:numpy.ndarray(2,\'float\')')
                sys.exit(1)
        else:
            print('Error! Unsupported ndarray keyword: "%s"' % keyword.arg)
            sys.exit(1)
            
    #check and make sure minimal arguments are specified
    if kw[0] == 0:
        print('Error! Numpy Array object must be specified')
        print('Example objects:')
        print('\t[1,2]     #list with a single dimension')
        print('\t[[1],[2]] #list with multiple dimensions')
        sys.exit(1)
    if kw[1] == 0:
        print('Error! Numpy Array dtype must be specified')
        print('Example type:')
        print('\t\'float\'')
        sys.exit(1)
        
    return (argty,object_kw)

#######################
# getClname
#   returns class name of the object
def getClname(node):
    return node.__class__.__name__

#######################
# getType
#    process function argument annotations
def getType(arg):
    argty = ArgType()
    if isinstance(arg.annotation,ast.Str):
        argty.primitive = arg.annotation.s
        argty.category = 'scalar'
    elif isinstance(arg.annotation,ast.Call):
        #only supported attributes are numpy.ndarray
        argty = process_ndarray(arg.annotation)
        argty.category = 'array'
    elif isinstance(arg.annotation,ast.Subscript) and isinstance(arg.annotation.value,ast.Name) and arg.annotation.value.id == 'List' and isinstance(arg.annotation.slice.value,ast.Str):
        ann = arg.annotation.slice.value.s.split(':')
        if len(ann) != 2:
            print('Error! Invalid List annotation')
            print(arg.annotation.slice.value.s)
            print('Example List annotations:')
            print('\tList[\'int32:10\']')
            exit(1)
        argty.dimensions.append(ann[1])
        argty.primitive = ann[0]
        argty.category = 'list'
    elif arg.annotation == None:
        print('Error! Missing type annotation for arg "%s"' % arg.arg)
        print('Example annotations:')
        print('\tvar:\'float\'')
        print('\tvar:numpy.ndarray(2,\'float\')')
        sys.exit(1)
    else:
        print('Error! Invalid type annotation')
        print(ast.dump(arg.annotation))
        print('Example annotations:')
        print('\tvar:\'float\'')
        print('\tvar:numpy.ndarray(2,\'float\')')
        sys.exit(1)
    return argty

#######################
# getArg
#    processes the function argument's annotations and sets name
def getArg(arg):
    argty = getType(arg)
    argty.name = arg.arg
    return argty

#######################
# getFuncDecl
#   Parses AST.FunctionDef node and returns tuple containing 
#   function declaration into (return type, function name, args)
def getFuncDecl(node):
    if not isinstance(node, ast.FunctionDef):
        raise TypeError('expected AST.FunctionDef, got %r' % getClname(node))

    #get function name
    name = node.name

    #get return type
    ret_type = ArgType()
    if hasattr(node,'returns'):
        if isinstance(node.returns,ast.Str):
            ret_type.primitive = node.returns.s
            ret_type.category = 'scalar'
        elif node.returns == None:
            ret_type.primitive = 'void'
            ret_type.category = 'scalar'
        elif isinstance(node.returns,ast.NameConstant) and node.returns.value == None:
            ret_type.primitive = 'void'
            ret_type.category = 'scalar'
        else:
            print('ERROR! Invalid function return type: %s' % getClname(node.returns))
            sys.exit(1)
    else:
        ret_type.primitive = 'void'
        ret_type.category = 'scalar'

    #get function args
    args = []
    for arg in node.args.args:
        argty = getArg(arg)
        args.append(argty)
        
    #return tuple with return type, function name, and args
    ret = (ret_type,name,args)
    return ret

#######################
# getFuncLineRange
#    get the range of line numbers where a function exists in the source code
def getFuncLineRange(node,start,stop):
    if not isinstance(node, ast.FunctionDef):
        raise TypeError('expected AST.FunctionDef, got %r' % getClname(node))
        
    start,stop = _rec_getFuncLineRange(node,start,stop)
    return (start,stop)

def _rec_getFuncLineRange(node,start,stop):
    if hasattr(node,'lineno'):
        num = node.lineno
        if num < start:
            start = num
        if num > stop:
            stop = num
    if isinstance(node,ast.AST):
        for name,value in ast.iter_fields(node):
            if isinstance(value,list):
                for n in value:
                    start,stop = _rec_getFuncLineRange(n,start,stop)
            else:
                start,stop = _rec_getFuncLineRange(value,start,stop)
    return(start,stop)
    
#####################
# Scope
#   Class that holds the scopes for the application
class Scope:
    def __init__(self):
        self.scopes = [{}]        
    ##############
    # push: add another scope
    def push(self):
        self.scopes.append({})
    ##############
    # pop: remove a scope
    def pop(self):
        self.scopes.pop()
    ##############
    # put: add variable to most recent scope
    def put(self,name,ty):
        self.scopes[-1][name]=ty
    ##############
    # putGlobal: add variable to global scope
    def putGlobal(self,name,ty):
        self.scopes[0][name]=ty
    ##############
    # has: check if variable has been defined in any scope
    def has(self,name):
        for i in range(len(self.scopes)):
            s = self.scopes[-(i+1)]
            if name in s:
                return True
        return False
    ##############
    # get: get variable from most recent scope
    def get(self,name):
        for i in range(len(self.scopes)):
            if name in self.scopes[-(i+1)]:
                return self.scopes[-(i+1)][name]
        return None
    ##############
    # display: print scopes and variables in each (for debug)
    def display(self):
        string = 'Scope ('
        for s in self.scopes:
            string += '['
            for k in s:
                string += k + ' '
            string += '] '
        print(string+')')

######################
# ArgType
#   Class that holds function argument data type info
#   like: primitive ('int'), category ('scalar' or 'array'),
#   dimenions (['dim1size','dim2size'])
class ArgType:
    def __init__(self):
        self.name = ''
        self.primitive = ''
        self.category = ''
        self.dimensions = []
    ##############
    # size: get number of elements of arg for all dimensions
    def size(self):
        cnt = 1
        for d in self.dimensions:
            cnt *= d
        return cnt
    def __str__(self):
        return 'ArgType(name=%s, primitive=%s, category=%s, dimensions=%s)' % (self.name,self.primitive,self.category,str(self.dimensions))
    def __repr__(self):
        return self.__str__()
    ##############
    # getPtr: gets pointer character for argument (if necessary) for Python/C API wrapper generation
    def getPtr(self):
        if self.category == 'scalar':
            return (processType(self) + ' ' + self.name)
        elif self.category == 'array':
            string = processType(self) + '* ' + self.name
            return string
        else:
            print('Error! Unknown category in ArgType.getPtr: %s' % self.category)
            sys.exit(1)
    ##############
    # getType: processes type for this argument
    def getType(self):
        return processType(self)
    ##############
    # getDecl: gets declaration for argument in Python/C API wrapper generation
    def getDecl(self):
        if self.category == 'scalar':
            return (processType(self) + ' ' + self.name)
        elif self.category == 'array':
            string = processType(self) + ' ' + self.name + '[' + str(self.size()) + ']'
            return string
        elif self.category == 'list':
            string = processType(self) + ' ' + self.name + '[' + str(self.size()) + ']'
            return string
        else:
            print('Error! Unknown category in ArgType.getDecl: %s' % self.category)
            sys.exit(1)

#######################
# Stmt
#   Class inheriting from AST adding a statement node
#   only has one field: 'body' which contains an AST node
class Stmt(ast.AST):
    def __init__(self):
        self._fields = ['body']
        self.body = ''
        self.lineno = -1
    def __repr__(self):
        return 'Stmt(body=%s' % (self.disp(self.body))
    def __str__(self):
        return __repr__()
    def disp(self,node):
        if isinstance(node,ast.AST):
            string = getClname(node) + '('
            cnt = 0
            for name,value in ast.iter_fields(node):
                val = self.disp(value)
                if cnt > 0:
                    string += ', '
                cnt += 1
                string += name + '=' + val
            return string + ')'
        elif isinstance(node,list):
            string = ''
            for x in node:
                string += self.disp(x)
            return string
        else:
            return repr(node)

class CallHier:
    def __init__(self):
        call = None
        calls = []
        

class DefUse:
    def __init__(self,n):
        self.vdef = []
        self.vuse = []
        self.children = []
        self.node = n
    def addChild(self,n):
        self.children.append(n)
    def addUse(self,u):
        self.vuse.append(u)
    def addDef(self,d):
        self.vdef.append(d)
    def getID(self):
        return str(id(self.node))
    def getStr(self):
        s = '\n'
        if len(self.vdef) > 0:
            s += 'def%s ' % str(self.vdef)
        if len(self.vuse) > 0:
            s += 'use%s' % str(self.vuse)
        return '%s%s' % (getClname(self.node),s)
