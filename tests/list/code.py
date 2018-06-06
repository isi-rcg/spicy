def foo():
    a : List['int32:10'] = []
    for i in range(10):
        a.append(i)
    b = a.pop()
foo()
