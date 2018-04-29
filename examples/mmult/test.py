# The Software is made available for academic or non-commercial purposes only. 
# The license is for a copy of the program for an unlimited term. Individuals 
# requesting a license for commercial use must pay for a commercial license. 
# 
# USC Stevens Institute for Innovation
# University of Southern California
# 1150 S. Olive Street, Suite 2300
# Los Angeles, CA 90115, USA
# ATTN: Accounting
# 
# DISCLAIMER. USC MAKES NO EXPRESS OR IMPLIED WARRANTIES, EITHER IN FACT OR 
# BY OPERATION OF LAW, BY STATUTE OR OTHERWISE, AND USC SPECIFICALLY AND 
# EXPRESSLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTY OF MERCHANTABILITY OR 
# FITNESS FOR A PARTICULAR PURPOSE, VALIDITY OF THE SOFTWARE OR ANY OTHER 
# INTELLECTUAL PROPERTY RIGHTS OR NON-INFRINGEMENT OF THE INTELLECTUAL 
# PROPERTY OR OTHER RIGHTS OF ANY THIRD PARTY. SOFTWARE IS MADE AVAILABLE 
# AS-IS. LIMITATION OF LIABILITY.  TO THE MAXIMUM EXTENT PERMITTED BY LAW, 
# IN NO EVENT WILL USC BE LIABLE TO ANY USER OF THIS CODE FOR ANY INCIDENTAL, 
# CONSEQUENTIAL, EXEMPLARY OR PUNITIVE DAMAGES OF ANY KIND, LOST GOODWILL, 
# LOST PROFITS, LOST BUSINESS AND/OR ANY INDIRECT ECONOMIC DAMAGES WHATSOEVER, 
# REGARDLESS OF WHETHER SUCH DAMAGES ARISE FROM CLAIMS BASED UPON CONTRACT, 
# NEGLIGENCE, TORT (INCLUDING STRICT LIABILITY OR OTHER LEGAL THEORY), A 
# BREACH OF ANY WARRANTY OR TERM OF THIS AGREEMENT, AND REGARDLESS OF 
# WHETHER USC WAS ADVISED OR HAD REASON TO KNOW OF THE POSSIBILITY OF 
# INCURRING SUCH DAMAGES IN ADVANCE.
# 
# For commercial license pricing and annual commercial update and support pricing, please contact:
# Rakesh Pandit
# USC Stevens Institute for Innovation
# University of Southern California
# 1150 S. Olive Street, Suite 2300
# Los Angeles, CA 90115, USA
# Tel: +1 213-821-3552
# Fax: +1 213-821-5001
# Email: rakeshvp@usc.edu and CC to: accounting@stevens.usc.edu
# 


# Original Version
# Author: Sam Skalicky, skalicky@isi.edu
# Institution: Information Sciences Institute, University of Southern California
# 


import time
import numpy as np

import code

# Function to be translated (function under test)
def mmult(a:np.ndarray((32,32),'int32'), 
          b:np.ndarray((32,32),'int32'), 
          c:np.ndarray((32,32),'int32')):
    # Function interface pragmas
    '''
    #pragma SDS data access_pattern(a:SEQUENTIAL, b:SEQUENTIAL, c:SEQUENTIAL)
    #pragma SDS data mem_attribute(a:PHYSICAL_CONTIGUOUS, b:PHYSICAL_CONTIGUOUS, c:PHYSICAL_CONTIGUOUS)
    #pragma SDS data data_mover(a:AXIDMA_SIMPLE, b:AXIDMA_SIMPLE, c:AXIDMA_SIMPLE)
    '''
    # internal partitioned arrays
    A_tmp = np.ndarray((32,32),'int32')
    B_tmp = np.ndarray((32,32),'int32')

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
            result: 'int32' = 0
            for k in range(b.shape[0]):
               result += A_tmp[i][k] * B_tmp[k][j] 
            c[i][j] = result
            
if __name__ == "__main__":
    #allocate arrays
    a = np.ndarray((32,32),dtype='int32',buffer=np.linspace(1,1024,1024,dtype='int32').reshape(32,32))
    b = np.ndarray((32,32),dtype='int32',buffer=np.linspace(1,1024,1024,dtype='int32').reshape(32,32))
    c = np.ndarray((32,32),dtype='int32',buffer=np.zeros((32,32),dtype='int32'))

    #execute function under test and time it
    t1 = time.time()
    mmult(a,b,c)
    t2 = time.time()
    elapsed = str((t2 - t1)*1000000)
    #display elapsed time
    print ("@time: " + elapsed + " us")

    #display results
    print('a = %s\n' % a)
    print('b = %s\n' % b)
    print('c = %s\n' % c)
