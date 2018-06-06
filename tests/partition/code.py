import numpy as np

def foo():
    im_buf = np.ndarray((5,320),'uint8')
    '''
    #pragma HLS ARRAY_PARTITION variable=im_buf cyclic factor=3 dim=1
    '''
