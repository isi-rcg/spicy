import numpy as np

def foo(a:np.ndarray((2,4),'float')):
    a[0][0] = 2

arr = np.ndarray((2,4),'float')
arr[0][0]=1
arr[0][1]=2
arr[0][2]=3
arr[0][3]=4

print(arr)

foo(arr)

print(arr)
