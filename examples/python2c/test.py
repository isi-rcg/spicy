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


import numpy as np
import time

# Function to be translated (function under test)
def mmult(a:np.ndarray((2,2),'int32'), 
          b:np.ndarray((2,2),'int32'), 
          c:np.ndarray((2,2),'int32'),
          size:'int32'):

    for i in range(a.shape[0]):
        for j in range(a.shape[1]):
            for k in range(b.shape[0]):
               c[i][j] += a[i][k] * b[k][j] 


if __name__ == "__main__":
    #allocate arrays
    a = np.ndarray((2,2),'int32',np.array([[1,2],[3,4]],'int32'))
    b = np.ndarray((2,2),'int32',np.array([[5,6],[7,8]],'int32'))
    c = np.zeros((2,2),'int32')

    #execute function under test and time it
    t0 = time.time()
    mmult(a,b,c,2)
    t1 = time.time()
    #display elapsed time
    print('Elapsed time: %.2fus' % ((t1-t0)*1000000))

    #display results
    print('a =\n %s\n' % a)
    print('b =\n %s\n' % b)
    print('c =\n %s\n' % c)
