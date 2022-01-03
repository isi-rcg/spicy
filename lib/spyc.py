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


#############################################################################################
# spyc.py
#
# User-facing support library for contiguous memory management
#
#############################################################################################

import os
import pynq
import numpy as np

#load accelerator library
libcaller = os.path.abspath('./libcaller.so')
pynq.xlnk.Xlnk.set_allocator_library(libcaller)

#create contiguous memory manager
memmanager = pynq.xlnk.Xlnk()
#create map of allocations for tracking
allocated = {}

#create a contiguous ndarray
def init_contiguous_ndarray(shape=(32,32), dtype="float", buffer=None):
    #calculate total number of elements
    elements = 1
    if isinstance(shape,tuple):
        for a in shape:
            elements *= a
    else:
        elements = shape
    #calculate total bytes to allocate
    length = elements * np.dtype(dtype).itemsize
    #contiguous allocation
    buffer_pointer = memmanager.cma_alloc(length,1,'char')
    #create bufferable object with contiguous memory
    buf = memmanager.cma_get_buffer(buffer_pointer, length)
    #create ndarray
    array = np.frombuffer(buf, dtype=dtype).reshape(shape)
    #store allocation in map
    allocated[str(array.data)] = buffer_pointer
    #initialize memory if data is provided
    if not(buffer is None):
        np.copyto(array,buffer)
        
    return array

#free a contiguous ndarray
def free_contiguous_ndarray(arr):
    #get contiguous memory address
    buffer_pointer = allocated[str(arr.data)]
    #free
    memmanager.cma_free(buffer_pointer)
            
