def foo(a):
    a[0] = 1

def bar(a,b):
    b[0] = a[0] + 2
    a[0] = 1

c = 2
d = [c]
foo(d)
e = [0]
b = bar(d,e)

