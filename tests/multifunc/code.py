def foo(a:'int32'):
    '''
    #pragma spyc accelerate
    '''
    a = 2

def bar(b:'int32',c:'int32'):
    '''
    #pragma spyc accelerate
    '''
    c = b + 1


a = 0
c = 0
foo(a)
bar(a,c)


