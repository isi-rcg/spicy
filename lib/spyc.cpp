/*
 * The Software is made available for academic or non-commercial purposes only. 
 * The license is for a copy of the program for an unlimited term. Individuals 
 * requesting a license for commercial use must pay for a commercial license. 
 * 
 * USC Stevens Institute for Innovation
 * University of Southern California
 * 1150 S. Olive Street, Suite 2300
 * Los Angeles, CA 90115, USA
 * ATTN: Accounting
 * 
 * DISCLAIMER. USC MAKES NO EXPRESS OR IMPLIED WARRANTIES, EITHER IN FACT OR 
 * BY OPERATION OF LAW, BY STATUTE OR OTHERWISE, AND USC SPECIFICALLY AND 
 * EXPRESSLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTY OF MERCHANTABILITY OR 
 * FITNESS FOR A PARTICULAR PURPOSE, VALIDITY OF THE SOFTWARE OR ANY OTHER 
 * INTELLECTUAL PROPERTY RIGHTS OR NON-INFRINGEMENT OF THE INTELLECTUAL 
 * PROPERTY OR OTHER RIGHTS OF ANY THIRD PARTY. SOFTWARE IS MADE AVAILABLE 
 * AS-IS. LIMITATION OF LIABILITY.  TO THE MAXIMUM EXTENT PERMITTED BY LAW, 
 * IN NO EVENT WILL USC BE LIABLE TO ANY USER OF THIS CODE FOR ANY INCIDENTAL, 
 * CONSEQUENTIAL, EXEMPLARY OR PUNITIVE DAMAGES OF ANY KIND, LOST GOODWILL, 
 * LOST PROFITS, LOST BUSINESS AND/OR ANY INDIRECT ECONOMIC DAMAGES WHATSOEVER, 
 * REGARDLESS OF WHETHER SUCH DAMAGES ARISE FROM CLAIMS BASED UPON CONTRACT, 
 * NEGLIGENCE, TORT (INCLUDING STRICT LIABILITY OR OTHER LEGAL THEORY), A 
 * BREACH OF ANY WARRANTY OR TERM OF THIS AGREEMENT, AND REGARDLESS OF 
 * WHETHER USC WAS ADVISED OR HAD REASON TO KNOW OF THE POSSIBILITY OF 
 * INCURRING SUCH DAMAGES IN ADVANCE.
 * 
 * For commercial license pricing and annual commercial update and support pricing, please contact:
 * Rakesh Pandit
 * USC Stevens Institute for Innovation
 * University of Southern California
 * 1150 S. Olive Street, Suite 2300
 * Los Angeles, CA 90115, USA
 * Tel: +1 213-821-3552
 * Fax: +1 213-821-5001
 * Email: rakeshvp@usc.edu and CC to: accounting@stevens.usc.edu
 * 
 */


/*
 * Original Version
 * Author: Sam Skalicky, skalicky@isi.edu
 * Institution: Information Sciences Institute, University of Southern California
 * 
 */


/********************************************
 * spyc.cpp
 * C/C++ library of functions for traversing
 *   the Python<-->C/C++ boundary
 ********************************************/

#include <Python.h>
#include <numpy/arrayobject.h>
#include <stdio.h>
#include <stdlib.h>
#include "spyc.h"
#include <stdio.h>


/* cleanupArrayArg
 *   Cleanup after C/C++ program before returning to Python
 */
void cleanupArrayArg(PyArrayObject* obj, void** ptr) {
  //future cleanup operation
}


/* setupArrayArg
 *    Setup for C/C++ program
 */
void setupArrayArg(PyArrayObject* obj, void** ptr, int type, char* str) {
  //check that object passed in matches the expected type
  if(PyArray_TYPE(obj) != type) {
    printf("Error! Expected array of '");
    printType(type);
    printf("' for argument %s but found '",str);
    printType(PyArray_TYPE(obj));
    printf("'\n");
  }
  else
    *ptr = PyArray_DATA(obj);
}

/* printType
 *    print the integer type enum as a string
 */
void printType(int type) {
  switch(type) {
  case NPY_INT8: printf("int8");
    break;
  case NPY_INT16: printf("int16");
    break;
  case NPY_INT32: printf("int32");
    break;
  case NPY_INT64: printf("int64");
    break;
  case NPY_UINT8: printf("uint8");
    break;
  case NPY_UINT16: printf("uint16");
    break;
  case NPY_UINT32: printf("uint32");
    break;
  case NPY_UINT64: printf("uint64");
    break;
  case NPY_FLOAT: printf("float");
    break;
  case NPY_DOUBLE: printf("double");
    break;

  default: printf("ERR/UNKN");
  }
}
