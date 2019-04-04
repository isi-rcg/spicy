import numpy as np
import mxnet as mx
import time

# Function to be translated (function under test)
def mmult(a:np.ndarray((32,32),'float32'), 
          b:np.ndarray((32,32),'float32'), 
          c:np.ndarray((32,32),'float32')):
    # Function interface pragmas
    '''
    #pragma SDS data access_pattern(a:SEQUENTIAL, b:SEQUENTIAL, c:SEQUENTIAL)
    #pragma SDS data mem_attribute(a:PHYSICAL_CONTIGUOUS, b:PHYSICAL_CONTIGUOUS, c:PHYSICAL_CONTIGUOUS)
    #pragma SDS data data_mover(a:AXIDMA_SIMPLE, b:AXIDMA_SIMPLE, c:AXIDMA_SIMPLE)
    '''
    # internal partitioned arrays
    A_tmp = np.ndarray((32,32),'float32')
    B_tmp = np.ndarray((32,32),'float32')

    '''
    #pragma HLS array_partition variable=A_tmp cyclic factor=16 dim=1
    #pragma HLS array_partition variable=B_tmp block factor=16 dim=1
    '''
    # Copy loops
    for i in range(a.shape[0]):
        for j in range(a.shape[1]):
            '''
            #pragma HLS PIPELINE
            '''
            A_tmp[i][j] = a[i][j]
            B_tmp[i][j] = b[i][j]

    # Compute loops
    for i in range(a.shape[0]):
        for j in range(a.shape[1]):
            '''
            #pragma HLS PIPELINE
            '''
            result: 'float32' = 0
            for k in range(b.shape[0]):
                result += A_tmp[i][k] * B_tmp[k][j] 
            c[i][j] = result

class Gemm(mx.operator.CustomOp):
    def forward(self, is_train, req, in_data, out_data, aux):
        A = np.ndarray((32,32),dtype='float32',buffer=in_data[0].asnumpy())
        B = np.ndarray((32,32),dtype='float32',buffer=in_data[1].asnumpy())
        C = np.ndarray((32,32),dtype='float32',buffer=out_data[0].asnumpy())
        mmult(A,B,C)
        self.assign(out_data[0], req[0], mx.nd.array(C))

@mx.operator.register("gemm")
class GemmProp(mx.operator.CustomOpProp):
    def __init__(self):
        super(GemmProp, self).__init__(True)

    def list_arguments(self):
        return ['data0', 'data1']

    def list_outputs(self):
        return ['output']

    def infer_shape(self, in_shapes):
        A_shape = in_shapes[0]
        B_shape = in_shapes[1]
        if A_shape[1] != B_shape[0]:
            print ('input shapes not compatible for matmul')
        else:
            output_shape = [A_shape[0], B_shape[1]]
        return (A_shape, B_shape), (output_shape,), ()

    def create_operator(self, ctx, in_shapes, in_dtypes):
        return Gemm()

A = mx.nd.array(np.linspace(1,1024,1024,dtype='float32').reshape(32,32))
B = mx.nd.array(np.linspace(1,1024,1024,dtype='float32').reshape(32,32))
for i in range(10):
    start = time.time()
    C = mx.nd.Custom(A, B, op_type='gemm')
    mx.nd.waitall()
    stop = time.time()
    print("elapsed: %4.2f ms" % ((stop-start)*1000))
