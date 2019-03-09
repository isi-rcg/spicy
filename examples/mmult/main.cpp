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


/*
 * main.cpp
 *    Test code for python2c example
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>

#include "caller.h"
#include "mmult.h"

#ifdef __SDSCC__
#include "sds_lib.h"
#else 
#define sds_alloc(x)(malloc(x))
#define sds_free(x)(free(x))
#endif

#define N 32

/*
 * initialize array values
 */
void init(int32_t *a, int32_t *b, int32_t *c, int32_t *d) {
  for(int i=0; i<N; i++) {
      for(int j=0; j<N; j++) {
	a[i*N+j] = i*N + j+1;
	b[i*N+j] = N*N + (i*N+j+1);
	c[i*N+j] = 0;
	d[i*N+j] = 0;
      }
  }
}

/*
 * compute matrix multiplication in software for comparion
 */
void mmult_golden(int32_t a[1024],int32_t b[1024],int32_t c[1024]) {
  for(int i=0; i<N; i++) {
    for(int j=0; j<N; j++) {
      int32_t sum=0;
      for(int k=0; k<N; k++) {
	sum += a[i*N + k] * b[k*N+j];
      }
      c[i*N+j] = sum;
    }
  }
}

/*
 * compare results output matrices from function-under-test and golden software
 */
int check(int32_t c[1024],int32_t d[1024]) {
  int err=0;
  for(int i=0; i<N; i++) {
    for(int j=0; j<N; j++) {
      if(c[i*N+j] != d[i*N+j]) {
	printf("Error! Mismatch[%d][%d]: %d != %d (golden)\n",i,j,c[i*N+j],d[i*N+j]);
	err++;
      }
    }
  }
  if(!err) {
    printf("Results match expected\n");
    return 0;
  } else
    return 1;
}

int main() {
  printf("starting...\n");

  struct timeval start, stop;
  int32_t *a,*b,*c,*d;

  a = (int32_t*) sds_alloc(1024 * sizeof(int32_t));
  b = (int32_t*) sds_alloc(1024 * sizeof(int32_t));
  c = (int32_t*) sds_alloc(1024 * sizeof(int32_t));
  d = (int32_t*) sds_alloc(1024 * sizeof(int32_t));

  if(!a || !b || !c || !d) {
    printf("Error during allocation\n");
    return 1;
  }
  
  //Test mmult directly
  printf("calling mmult directly\n");
  init(a,b,c,d);
  mmult_golden(a,b,d);
  gettimeofday(&start, NULL);
  mmult(a,b,c);
  gettimeofday(&stop, NULL);
  printf("Elapsed time %ld us\n",(stop.tv_sec - start.tv_sec)*1000000 + (stop.tv_usec-start.tv_usec));
  check(c,d);
  
 //Test mmult indirectly by caller
  printf("calling mmult indirectly\n");
  init(a,b,c,d);
  mmult_golden(a,b,d);
  gettimeofday(&start, NULL);
  mmult_caller(a,b,c);
  gettimeofday(&stop, NULL);
  printf("Elapsed time %ld us\n",(stop.tv_sec - start.tv_sec)*1000000 + (stop.tv_usec-start.tv_usec));
  check(c,d);

  sds_free(a);
  sds_free(b);
  sds_free(c);
  sds_free(d);

  printf("finished\n");

  return 0;
}
