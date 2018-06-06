
def bar(a:'int32') -> 'int32':
    return a + 1

def foo(a:'int32') -> 'int32':
    return bar(a)

print(foo(1))
