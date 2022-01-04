/*
Copyright (C) 2017,2018 John E. Davis

This file is part of the S-Lang Library.

The S-Lang Library is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The S-Lang Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
USA.
*/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <slang.h>

#include "_slint.h"
#include "stats-module.h"

static double alnorm (double x, int use_upper)
{
   double cdf;
   cdf = 0.5 * (1.0 + erf (x/sqrt(2.0)));
   if (use_upper) return 1.0 - cdf;
   return cdf;
}

static int prtaus_large_n (_pSLint64_Type is, _pSLint64_Type n, double *probp)
{
   double h[15];
   double x, r, sc, p;
   int i;

   /* PROBABILITIES CALCULATED BY MODIFIED EDGEWORTH SERIES FOR N GREATER THAN 8 */
   /* CALCULATION OF TCHEBYCHEFF-HERMITE POLYNOMIALS */
   x = (is-1.0) / sqrt((6.0 + n*(5.0-n*(3.0+2*n)))/(-18.0));
   h[0] = x;
   h[1] = x*x - 1.0;
   for (i = 2; i < 15; i++)
     {
	h[i] = x * h[i-1] - (i-1.0)*h[i-2];
     }

   r = 1.0 / n;
   sc = r*(h[2]*(r*(r*(r*0.506f - 0.5325f) + 0.045f) - 0.09f)
	   + r*(h[4]*(r*(r*0.3214f - 0.036735f) + 0.036735f)
		+ h[6]*(r*(r*0.07787f - 0.023336f) + 0.00405f)
		+ r*(h[8]*(-.0033061f - r*0.0065166f)
		     + h[10]*(r*0.0025927f - 1.215e-4f)
		     + r*(h[12]*1.4878e-4f + h[14]*2.7338e-6f))));

   p = alnorm (x,1) + sc*0.398942*exp(-0.5*x*x);
   if (p < 0.0) p = 0.0;
   if (p > 1.0) p = 1.0;
   *probp = p;

   return 0;
}


/*
 * ALGORITHM AS 71  APPL. STATIST. (1974) VOL.23, NO.1
 *
 * GIVEN A VALUE OF IS CALCULATED FROM TWO RANKINGS (WITHOUT TIES)
 * OF N OBJECTS, THE FUNCTION COMPUTES THE PROBABILITY OF
 * OBTAINING A VALUE GREATER THAN, OR EQUAL TO, IS.
 *
 */
static int prtaus (_pSLint64_Type is, _pSLint64_Type n, double *probp)
{
#define MAX_N_EXACT 30
#define MAX_L_COLS ((MAX_N_EXACT)*(MAX_N_EXACT-1)/4 + 2)
   _pSLint64_Type row0[MAX_L_COLS];  /* allow 1-based indexing */
   _pSLint64_Type row1[MAX_L_COLS];  /* allow 1-based indexing */
   _pSLint64_Type *curr, *prev;
   _pSLint64_Type i, il, k, m, im;

   if (n > MAX_N_EXACT)
     return prtaus_large_n (is, n, probp);

   /* Use recurrence relation for n <= MAX_N_EXACT */

   *probp = 1.0;

   m = n*(n-1)/2;
   if (is >= 0) m -= is; else m += is;
   if ((m == 0) && (is <= 0)) return 0;

   if (is < 0) m = m - 2;
   im = m/2;

   memset (row0, 0, (im+1)*sizeof(_pSLint64_Type));
   memset (row1, 0, (im+1)*sizeof(_pSLint64_Type));
   prev = row0; prev[0] = 1;
   curr = row1; curr[0] = 1;

   il = 0;
   i = 1;
   m = 1;
   while (i < n)
     {
	_pSLint64_Type *tmp;
	_pSLint64_Type in, io;

	tmp = curr; curr = prev; prev = tmp;
        il += i;
        i++;
        m = m*i;
	k = (im < il) ? im : il;

	k++;
	in = 1;
	io = (i <= k) ? i : k;
	while (in < io)
	  {
	     curr[in] = curr[in-1] + prev[in];
	     in++;
	  }
	io = 0;
	while (in < k)
	  {
	     curr[in] = curr[in-1] + prev[in] - prev[io];
	     io++;
	     in++;
	  }
     }

   k = 0;
   for (i = 0; i <= im; i++)
     k += curr[i];

   *probp = ((double) k)/m;
   if (is < 0) *probp = 1.0 - *probp;

   return 0;
}

static _pSLuint64_Type kendall_insertion_sort (SLindex_Type *arr, size_t num)
{
   size_t maxj, i;
   _pSLuint64_Type nexch;

   if (num < 2) return 0;

   nexch = 0;
   i = maxj = num - 1;
   while (i--)
     {
        size_t j = i;
        SLindex_Type val = arr[i];

	while ((j < maxj) && (arr[j+1] < val))
	  {
	     arr[j] = arr[j+1];
	     j++;
	  }
        arr[j] = val;
	nexch += (j - i);
    }
   return nexch;
}

static _pSLuint64_Type kendall_merge (SLindex_Type *left, size_t left_num,
			    SLindex_Type *right, size_t right_num,
			    SLindex_Type *work)
{
   _pSLuint64_Type nexch = 0;

   while (left_num && right_num)
     {
        if (*right < *left)
	  {
	     *work++ = *right++;
	     right_num--;
	     nexch += left_num;
	     continue;
	  }

	*work++ = *left++;
	left_num--;
     }

   if (left_num)
     memcpy(work, left, left_num * sizeof(SLindex_Type));
   else if (right_num)
     memcpy(work, right, right_num * sizeof(SLindex_Type));

   return nexch;
}

static _pSLuint64_Type kendall_merge_sort(SLindex_Type *a, size_t num, SLindex_Type *work)
{
   _pSLuint64_Type nexch;
   size_t left_num, right_num;
   SLindex_Type *left, *right;

   if (num < 8)
     return kendall_insertion_sort (a, num);

   left = a;
   left_num = num/2;
   right = a + left_num;
   right_num = num - left_num;

   nexch = kendall_merge_sort(left, left_num, work);
   nexch += kendall_merge_sort(right, right_num, work);
   nexch += kendall_merge(left, left_num, right, right_num, work);

   memcpy(a, work, num * sizeof(SLindex_Type));
   return nexch;
}

/* This also computes the quantities needed for the p-value.  From wikipedia:
 *   v = sum(x*(x-1)*(2*x+5))
 *   v1 = sum (x*(x-1))
 *   v2 = sum(x*(x-1)*(x-2))
 * where x is the number of tied values in a group.
 */
static _pSLuint64_Type
kendall_count_tied_pairs (SLindex_Type *a, size_t num,
			  _pSLuint64_Type *v, _pSLuint64_Type *v1, _pSLuint64_Type *v2)
{
   _pSLuint64_Type n = 0;
   size_t i;

   i = 1;
   while (i < num)
     {
	_pSLuint64_Type di, dn;
	size_t i0;

	if (a[i] != a[i-1])
	  {
	     i++;
	     continue;
	  }

	/* In a group with ties */
	i0 = i-1;
	i++;
	while ((i < num) && (a[i] == a[i-1]))
	  i++;

	di = i-i0;
	dn = di*(di-1);
	*v1 += dn;
	*v2 += dn*(di-2);
	*v += dn * (2*di+5);
	n += dn/2;

	i++;
     }

   return n;
}

/*
 * This function assumes that the input arrays are sorted on the first array.
 * That is, the slang code that wraps this will have to do:
 *
 *    i = array_sort (a);
 *    a = a[i]; b = [i];
 *
 * The basic idea is the following:
 * 
 * The total number of pairs formed from an array of size N is
 * n0 = N*(N-1)/2.  Consider 2 such arrays A and B.  Then consider the ith and
 * jth elemnts of the arrays, where j>i.  One of the following will be true:
 *
 *     A[i]>A[j] and B[i]>B[j]     "concordent"
 *     A[i]<A[j] and B[i]<B[j]     "concordent"
 *     A[i]>A[j] and B[i]<B[j]     "disconcordent"
 *     A[i]<A[j] and B[i]>B[j]     "disconcordent"
 *     A[i]=A[j] and B[i]!=B[j]    "A is tied"
 *     A[i]!=A[j] and B[i]=B[j]    "B is tied"
 *     A[i]=A[j] and B[i]=B[j]     "joint tie"
 *
 * Then: n0 = nc + nd + (t + v) + u
 *       n0 = nc + nd + (u + v) + t
 * where nc=num concordent, nd = num disconcordent, t=A ties, u=B ties, v=joint ties
 *
 * Let t+v = nA = total number of ties in A (includes joint)
 *     u+v = nB = total number of ties in B (includes joint)
 * Then: n0 = nc + nd + nA + u
 * 
 *   nc + nd = n0 - nA - u
 *           = n0 - nA - (nB - v)
 *           = n0 + v - (nA + nB)
 * Knight indicates that nd is equal to the number of exchanges (ne) in sorting B.
 * Hence:
 *
 *   nc - nd = (nc + nd) - 2*ne
 *           = (n0 + v) - (nA + nB + 2*ne)
 */
double _pSLstats_kendall_tau (SLindex_Type *a, SLindex_Type *b, size_t num, double *taup)
{
   double tau, sigma, prob;
   _pSLuint64_Type n0, na, nb, ne, v;
   _pSLuint64_Type va, va1, va2, vb, vb1, vb2;
   size_t i;

   n0 = num;
   n0 = n0*(n0-1)/2;

   na = v = 0;
   va = va1 = va2 = 0;
   vb = vb1 = vb2 = 0;

   i = 1;
   while (i < num)
     {
	_pSLuint64_Type di;
	size_t i0;

	if (a[i-1] != a[i])
	  {
	     i++;
	     continue;
	  }

	i0 = i - 1;
	i++;
	while ((i < num) && (a[i-1] == a[i]))
	  i++;

	di = i-i0;
	na += di*(di-1)/2;

	(void) kendall_insertion_sort (b+i0, di);
	v += kendall_count_tied_pairs(b+i0, di, &va, &va1, &va2);
	i++;			       /* ok to do since if i<num, the a[i-1]!=a[i] */
     }

   /* Sort b, using a as workspace */
   ne = kendall_merge_sort (b, num, a);
   nb = kendall_count_tied_pairs (b, num, &vb, &vb1, &vb2);

   /* Of no ties, use exact probability distribution */
   if ((na == 0) && (nb == 0))
     {
	_pSLint64_Type is;

	if (n0 < 2*ne)
	  is = -(_pSLint64_Type)(2*ne - n0);
	else
	  is = (_pSLint64_Type)(n0 - 2*ne);

	*taup = ((double)is)/n0;

	/* prob is probability of getting a statistic >= is. */
	(void) prtaus (is, num, &prob);
	prob = 1.0 - prob;	       /* prob of statistic < is */
	/* fprintf (stdout, "prtaus: num=%lu, is=%lu, prob=%g\n", num, is, prob); */
	return prob;
     }

   /* Otherwise, use normal distributuon */
   tau = (n0 + v - na - nb - ne);      /* This should be >= 0 */
   tau -= ne;			       /* may be < 0 */

   /* From wikipedia */
   sigma = (n0*(4.0*num+10.0) - va - vb)/18.0;
   sigma += va1*(double)vb1/(4.0*n0);
   sigma += va2*(double)vb2/(18.0*n0*(num-2));
   sigma = sqrt (sigma);

   *taup = tau/sqrt(n0-na)/sqrt(n0-nb);  /* avoid overflow */

   /* To compute the probability use the continuity correction recommended by Kendall */
   if (tau > 0) tau -= 1; else if (tau < 0) tau += 1;
   return 0.5 * (1.0 + erf (tau/sigma/sqrt(2.0)));
}
