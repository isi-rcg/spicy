import time
import numpy as np

def mmult(a:np.ndarray((32,32),'int32'), 
          b:np.ndarray((32,32),'int32'), 
          c:np.ndarray((32,32),'int32')):
    '''
    #pragma SDS data access_pattern(a:SEQUENTIAL, b:SEQUENTIAL, c:SEQUENTIAL)
    #pragma SDS data mem_attribute(a:PHYSICAL_CONTIGUOUS, b:PHYSICAL_CONTIGUOUS, c:PHYSICAL_CONTIGUOUS)
    #pragma SDS data data_mover(a:AXIDMA_SIMPLE, b:AXIDMA_SIMPLE, c:AXIDMA_SIMPLE)
    '''
    A_tmp = np.ndarray((32,32),'int32')
    B_tmp = np.ndarray((32,32),'int32')

    '''
    #pragma HLS array_partition variable=A_tmp cyclic factor=16 dim=1
    #pragma HLS array_partition variable=B_tmp block factor=16 dim=1
    '''
    
    for i in range(a.shape[0]):
        for j in range(a.shape[1]):
            '''
            #pragma HLS PIPELINE
            '''
            A_tmp[i][j] = a[i][j]
            B_tmp[i][j] = b[i][j]
            
    for i in range(a.shape[0]):
        for j in range(a.shape[1]):
            '''
            #pragma HLS PIPELINE
            '''
            result: 'int32' = 0
            for k in range(b.shape[0]):
               result += A_tmp[i][k] * B_tmp[k][j] 
            c[i][j] = result

def run():
    a = np.ndarray((32,32),dtype='int32',buffer=np.linspace(1,1024,1024,dtype='int32').reshape(32,32))
    b = np.ndarray((32,32),dtype='int32',buffer=np.linspace(1,1024,1024,dtype='int32').reshape(32,32))
    c = np.ndarray((32,32),dtype='int32',buffer=np.zeros((32,32),dtype='int32'))
    for i in range(1000):
        mmult(a,b,c)

            
if __name__ == "__main__":
    a = np.ndarray((32,32),dtype='int32',buffer=np.linspace(1,1024,1024,dtype='int32').reshape(32,32))
    b = np.ndarray((32,32),dtype='int32',buffer=np.linspace(1,1024,1024,dtype='int32').reshape(32,32))
    c = np.ndarray((32,32),dtype='int32',buffer=np.zeros((32,32),dtype='int32'))
    print(a.strides)

    t1 = time.time()
    for i in range(100):
        mmult(a,b,c)
    t2 = time.time()
    elapsed = str((t2 - t1)*1000000/100)
    print ("@time: " + elapsed + " us")

    print('a = %s\n' % a)
    print('b = %s\n' % b)
    print('c = %s\n' % c)
