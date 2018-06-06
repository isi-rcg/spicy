import numpy as np

def foo(a:np.ndarray((4),'float')):
    a[0] = 2

arr = np.ndarray((4),'float')
arr[0]=1
arr[1]=2
arr[2]=3
arr[3]=4

print(arr)

foo(arr)

print(arr)
