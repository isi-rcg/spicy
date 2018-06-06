import numpy as np

def foo(a:np.ndarray((2,2,4),'float')):
    a[0][1][2] = 2

arr = np.ndarray((2,2,4),'float')
for i in range(2):
    for j in range(2):
        for k in range(4):
            arr[i][j][k]=i*8+j*4+k
            print(arr[i][j][k])
            
print(arr)

foo(arr)

print(arr)
