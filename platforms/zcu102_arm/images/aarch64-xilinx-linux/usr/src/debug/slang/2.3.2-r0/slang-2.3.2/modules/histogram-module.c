/* -*- mode: C; mode: fold; -*- */

/*
  Copyright (c) 2003-2007 Massachusetts Institute of Technology
  Copyright (c) 2013-2017,2018 John E. Davis <jed@jedsoft.org>

  This software was developed by the MIT Center for Space Research
  under contract SV1-61010 from the Smithsonian Institution.

  Permission to use, copy, modify, distribute, and sell this software
  and its documentation for any purpose is hereby granted without fee,
  provided that the above copyright notice appear in all copies and
  that both that copyright notice and this permission notice appear in
  the supporting documentation, and that the name of the Massachusetts
  Institute of Technology not be used in advertising or publicity
  pertaining to distribution of the software without specific, written
  prior permission.  The Massachusetts Institute of Technology makes
  no representations about the suitability of this software for any
  purpose.  It is provided "as is" without express or implied warranty.

  THE MASSACHUSETTS INSTITUTE OF TECHNOLOGY DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS.  IN NO EVENT SHALL THE MASSACHUSETTS
  INSTITUTE OF TECHNOLOGY BE LIABLE FOR ANY SPECIAL, INDIRECT OR
  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
  OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
  NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
  WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

/* Author: John E. Davis (davis@space.mit.edu) */

#include "config.h"

#include <stdio.h>
#include <math.h>
#include <slang.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif
SLANG_MODULE(histogram);
#ifdef __cplusplus
}
#endif

#define MODULE_VERSION_STRING	"0.4.0"
#define MODULE_VERSION_NUMBER	400

typedef unsigned int HistData_Type;
#define HISTDATA_TYPE SLANG_UINT_TYPE

static const char *Module_Version_String = MODULE_VERSION_STRING;

#ifndef HAVE_ISNAN
# define isnan(x) (x != x)
#endif

static int check_grid (double *x, SLuindex_Type n)
{
   double xlo;
   SLuindex_Type i;

   if (n == 0)
     return 0;

   xlo = x[0];

   if (isnan(xlo))
     goto return_error;

   for (i = 0; i < n; i++)
     {
	if (isnan(x[i]))
	  goto return_error;

	if (x[i] < xlo)
	  goto return_error;

	xlo = x[i];
     }
   return 0;

return_error:
   SLang_verror (SL_INVALID_PARM, "Invalid grid: Expecting one in increasing order");
   return -1;
}

static SLuindex_Type double_binary_search (double x, double *xp, SLuindex_Type n)
{
   SLuindex_Type n0, n1, n2;
   /* SLuindex_Type j; */
   double xp0, xp1;

   if (n < 2)
     return 0;

   if (x < (xp0 = xp[0]))
     return 0;
   if (x >= (xp1 = xp[n-1]))
     return n-1;
#if 0
   j = (SLuindex_Type) (((x - xp0)/(xp1 - xp0))*(n-1));
   if (j < n)
     {
	if (x >= xp[j])
	  {
	     if ((j == n-1) || (xp[j+1] < x))
	       return j;
	     n0 = j;
	     n1 = n;
	  }
	else
	  {
	     n0 = 0;
	     n1 = j;
	  }
     }
   else
     {
	n0 = 0;
	n1 = n;
     }
#else
   n0 = 0;
   n1 = n;
#endif

   while (n1 > n0 + 1)
     {
	/* Note: since n1 > n0+1, then n1 >= n0+2, and n0+n1 >= 2*n0+2
	 * ==> n2 = (n0+n1)/2 >= n0+1.
	 * ==> n2 > n0
	 * Also, n1 >= n0+2 ==> n1-2 >= n0 ==> 2*n1 - 2 >= (n0+n1)
	 * ==> n2 = (n0+n1)/2 <= n1-1  ==> n2+1 <= n1
	 * ==> n2 < n1
	 * Hence, n0 < n1 < n2.  This means that the interval determined
	 * by the following code will become smaller.
	 */
	n2 = (n0 + n1) / 2;
	if (xp[n2] > x)
	  n1 = n2;
	else
	  n0 = n2;
     }
   return n0;
}

#define BIN_EDGES_TYPE double
#define BINARY_SEARCH double_binary_search
#define PTS_TYPE unsigned char
#define HISTOGRAM_1D uc_histogram_1d
#define HISTOGRAM_2D uc_histogram_2d
#define CHECK_NANS 0
#include "histogram-module.inc"

#define BIN_EDGES_TYPE double
#define BINARY_SEARCH double_binary_search
#define PTS_TYPE int
#define HISTOGRAM_1D i_histogram_1d
#define HISTOGRAM_2D i_histogram_2d
#define CHECK_NANS 0
#include "histogram-module.inc"

#define BIN_EDGES_TYPE double
#define BINARY_SEARCH double_binary_search
#define PTS_TYPE float
#define HISTOGRAM_1D f_histogram_1d
#define HISTOGRAM_2D f_histogram_2d
#define CHECK_NANS 1
#include "histogram-module.inc"

#define BIN_EDGES_TYPE double
#define BINARY_SEARCH double_binary_search
#define PTS_TYPE double
#define HISTOGRAM_1D d_histogram_1d
#define HISTOGRAM_2D d_histogram_2d
#define HISTOGRAM_REBIN d_hist1d_rebin
#define CHECK_NANS 1
#include "histogram-module.inc"

static int uc_fast_hist_1d (unsigned char *pts, SLuindex_Type npts,
			    double *bin_edges, SLuindex_Type nbins,
			    HistData_Type *histogram)
{
   HistData_Type h[256];
   SLuindex_Type i;
   SLuindex_Type nbins_m1;

   if (nbins == 0)
     return 0;

   if (-1 == check_grid (bin_edges, nbins))
     return -1;

   for (i = 0; i < 256; i++)
     h[i] = 0.0;

   for (i = 0; i < npts; i++)
     h[pts[i]]++;

   nbins_m1 = nbins - 1;
   for (i = 0; i < nbins_m1; i++)
     {
	double x0, x1;
	SLuindex_Type ix0;

	x1 = bin_edges[i+1];
	if (x1 <= 0.0)
	  continue;

	x0 = bin_edges[i];
	if (x0 < 0.0)
	  x0 = 0.0;
	ix0 = (SLuindex_Type) ceil (x0);

	while (i < nbins_m1)
	  {
	     SLuindex_Type ix1;
	     SLuindex_Type j, jmax;

	     x1 = bin_edges[i+1];
	     ix1 = (SLuindex_Type) ceil (x1);
	     jmax = ix1;
	     if (jmax > 256) jmax = 256;

	     for (j = ix0; j < jmax; j++)
	       histogram[i] += h[j];

	     ix0 = ix1;
	     if (ix0 > 255)
	       break;
	     i++;
	  }
     }

   /* Handle overflow bin */

   if (bin_edges[nbins_m1] < 0)
     i = 0;
   else
     i = (SLuindex_Type) ceil (bin_edges[nbins_m1]);

   while (i < 256)
     {
	histogram[nbins_m1] += h[i];
	i++;
     }
   return 0;
}

static SLang_Array_Type *convert_reverse_indices (SLindex_Type *r, SLuindex_Type num_r, SLang_Array_Type *h)
{
   SLang_Array_Type *new_r;
   SLang_Array_Type **new_r_data;
   SLuindex_Type i, num_h;
   SLindex_Type *lens;

   if (NULL == (new_r = SLang_create_array (SLANG_ARRAY_TYPE, 0, NULL, h->dims, h->num_dims)))
     return NULL;

   num_h = h->num_elements;

   if (NULL == (lens = (SLindex_Type *)SLmalloc (num_h * sizeof (SLindex_Type))))
     {
	SLang_free_array (new_r);
	return NULL;
     }
   memset ((char *)lens, 0, num_h*sizeof(SLindex_Type));

   for (i = 0; i < num_r; i++)
     {
	SLindex_Type r_i = r[i];

	if (r_i >= 0)
	  lens[r_i]++;
     }

   new_r_data = (SLang_Array_Type **) new_r->data;
   for (i = 0; i < num_h; i++)
     {
	if (NULL == (new_r_data[i] = SLang_create_array (SLANG_ARRAY_INDEX_TYPE, 0, NULL, &lens[i], 1)))
	  goto return_error;

	lens[i] = 0;
     }

   for (i = 0; i < num_r; i++)
     {
	SLang_Array_Type *at;
	SLindex_Type r_i = r[i];

	if (r_i < 0)
	  continue;

	at = new_r_data[r_i];

	((SLindex_Type *)at->data)[lens[r_i]] = i;
	lens[r_i]++;
     }

   SLfree ((char *)lens);
   return new_r;

   return_error:
   SLfree ((char *) lens);
   SLang_free_array (new_r);
   return NULL;
}

static int map_to_best_type (int type, int *mtypep)
{
   switch (type)
     {
      case SLANG_UCHAR_TYPE:
	*mtypep = SLANG_UCHAR_TYPE;
	break;

      case SLANG_CHAR_TYPE:
      case SLANG_SHORT_TYPE:
      case SLANG_INT_TYPE:
	*mtypep = SLANG_INT_TYPE;
	break;

      case SLANG_FLOAT_TYPE:
	*mtypep = SLANG_FLOAT_TYPE;
	break;

      default:
	*mtypep = SLANG_DOUBLE_TYPE;
	break;
     }
   return 0;
}

static int pop_1d_array_of_type (SLang_Array_Type **atp, int type)
{
   SLang_Array_Type *at;

   if (-1 == SLang_pop_array_of_type (&at, type))
     return -1;

   if (at->num_dims != 1)
     {
	SLang_verror (SL_INVALID_PARM, "Expecting a 1-d array");
	SLang_free_array (at);
	return -1;
     }
   *atp = at;
   return 0;
}

static int pop_1d_double_arrays (SLang_Array_Type **ap, SLang_Array_Type **bp)
{
   SLang_Array_Type *a, *b;

   *ap = *bp = NULL;

   if (-1 == pop_1d_array_of_type (&b, SLANG_DOUBLE_TYPE))
     return -1;

   if (-1 == pop_1d_array_of_type (&a, SLANG_DOUBLE_TYPE))
     {
	SLang_free_array (b);
	return -1;
     }

   if (a->num_elements != b->num_elements)
     {
	SLang_verror (SL_INVALID_PARM, "Arrays do not match in size");
	SLang_free_array (b);
	SLang_free_array (a);
	return -1;
     }

   *ap = a;
   *bp = b;
   return 0;
}

static int pop_hist1d_pts_array (SLang_Array_Type **at)
{
   int type;

   if (-1 == map_to_best_type (SLang_peek_at_stack1 (), &type))
     return -1;
#if 1
   if (-1 == SLang_pop_array_of_type (at, type))
     return -1;
#else
   if (-1 == pop_1d_array_of_type (at, type))
     return -1;
#endif
   return 0;
}

static int pop_hist2d_pts_array (SLang_Array_Type **atxp,
				 SLang_Array_Type **atyp)
{
   int ytype, xtype;
   SLang_Array_Type *atx, *aty;

   ytype = SLang_peek_at_stack1 ();
   if (-1 == SLroll_stack (2))
     return -1;
   xtype = SLang_peek_at_stack1 ();

   if (-1 == map_to_best_type (ytype, &ytype))
     return -1;
   if (-1 == map_to_best_type (xtype, &xtype))
     return -1;

   if (xtype != ytype)
     ytype = xtype = SLANG_DOUBLE_TYPE;

   if (-1 == pop_1d_array_of_type (&atx, xtype))
     return -1;

   if (-1 == pop_1d_array_of_type (&aty, ytype))
     {
	SLang_free_array (atx);
	return -1;
     }

   if (atx->num_elements != aty->num_elements)
     {
	SLang_verror (SL_INVALID_PARM, "hist2d: x and y points arrays must match in size");
	SLang_free_array (aty);
	SLang_free_array (atx);
	return -1;
     }

   *atxp = atx;
   *atyp = aty;
   return 0;
}

static SLindex_Type *alloc_reverse_indices (SLuindex_Type num)
{
   SLuindex_Type i;
   SLindex_Type *r;

   if (NULL == (r = (SLindex_Type *) SLmalloc ((num + 1) * sizeof(SLindex_Type))))
     return NULL;

   for (i = 0; i < num; i++)
     r[i] = -1;

   return r;
}

static void hist1d (void)
{
   SLang_Array_Type *edges_at, *hist_at, *pts_at, *indices_at;
   SLang_Ref_Type *ref;
   SLindex_Type *rev_indices;
   int status, type, has_hist = 0;

   ref = NULL;
   switch (SLang_Num_Function_Args)
     {
      case 4:
	has_hist = 1;
	if (SLang_peek_at_stack () == SLANG_NULL_TYPE)
	  (void) SLdo_pop ();
	else if (-1 == SLang_pop_ref (&ref))
	  return;
	break;

      case 3:
	type = SLang_peek_at_stack ();
	if (type == SLANG_REF_TYPE)
	  {
	     if (-1 == SLang_pop_ref (&ref))
	       return;
	  }
	else if (type == SLANG_NULL_TYPE)
	  (void) SLdo_pop ();
	else
	  has_hist = 1;
	break;

      case 2:
	break;

      default:
	SLang_verror (SL_USAGE_ERROR, "h = hist1d ([h,] points, bins [,&reverse-indices])");
	return;
     }

   if (-1 == SLang_pop_array_of_type (&edges_at, SLANG_DOUBLE_TYPE))
     {
	SLang_free_ref (ref);
	return;
     }

   pts_at = NULL;
   hist_at = NULL;
   indices_at = NULL;
   rev_indices = NULL;

   if (-1 == pop_hist1d_pts_array (&pts_at))
     goto free_and_return;

   if (has_hist && (SLang_peek_at_stack () == SLANG_NULL_TYPE))
     {
	(void) SLdo_pop ();
	has_hist = 0;
     }

   if (has_hist)
     {
	if (-1 == SLang_pop_array_of_type (&hist_at, HISTDATA_TYPE))
	  goto free_and_return;
	if (hist_at->flags & SLARR_DATA_VALUE_IS_READ_ONLY)
	  {
	     SLang_verror (SL_InvalidParm_Error, "Input histogram array is read-only");
	     goto free_and_return;
	  }
	if ((hist_at->num_dims != 1)
	    || (hist_at->dims[0] != edges_at->dims[0]))
	  {
	     SLang_verror (SL_InvalidParm_Error, "Input histogram array is not 1d or does not match grid");
	     goto free_and_return;
	  }
     }
   else
     {
	if (NULL == (hist_at = SLang_create_array (HISTDATA_TYPE, 0, NULL, edges_at->dims, 1)))
	  goto free_and_return;
	/* hist_at->data is already initialized to 0 */
     }

   if ((ref != NULL)
       && (NULL == (rev_indices = alloc_reverse_indices (pts_at->num_elements))))
     goto free_and_return;

   switch (pts_at->data_type)
     {
      case SLANG_UCHAR_TYPE:
	if (rev_indices == NULL)
	  status = uc_fast_hist_1d ((unsigned char *)pts_at->data, pts_at->num_elements,
				    (double *)edges_at->data, edges_at->num_elements,
				    (HistData_Type *) hist_at->data);
	else status = uc_histogram_1d ((unsigned char *)pts_at->data, pts_at->num_elements,
				       (double *)edges_at->data, edges_at->num_elements,
				       (HistData_Type *) hist_at->data, rev_indices);
	break;

      case SLANG_INT_TYPE:
	status = i_histogram_1d ((int *)pts_at->data, pts_at->num_elements,
				 (double *)edges_at->data, edges_at->num_elements,
				 (HistData_Type *) hist_at->data, rev_indices);
	break;

      case SLANG_FLOAT_TYPE:
	status = f_histogram_1d ((float *)pts_at->data, pts_at->num_elements,
				 (double *)edges_at->data, edges_at->num_elements,
				 (HistData_Type *) hist_at->data, rev_indices);
	break;

      case SLANG_DOUBLE_TYPE:
	status = d_histogram_1d ((double *)pts_at->data, pts_at->num_elements,
				 (double *)edges_at->data, edges_at->num_elements,
				 (HistData_Type *) hist_at->data, rev_indices);
	break;

      default:
	SLang_verror (SL_INTERNAL_ERROR, "Error in hist1d: array not supported");
	status = -1;
     }

   if (status == -1)
     goto free_and_return;

   if (ref != NULL)
     {
	if (NULL == (indices_at = convert_reverse_indices (rev_indices, pts_at->num_elements, hist_at)))
	  goto free_and_return;

	if (-1 == SLang_assign_to_ref (ref, SLANG_ARRAY_TYPE, (VOID_STAR) &indices_at))
	  goto free_and_return;
     }

   (void) SLang_push_array (hist_at, 0);

   /* NULLs ok below */
   free_and_return:
   if (rev_indices != NULL) SLfree ((char *) rev_indices);
   SLang_free_ref (ref);
   SLang_free_array (indices_at);
   SLang_free_array (edges_at);
   SLang_free_array (pts_at);
   SLang_free_array (hist_at);
}

static void hist2d (void)
{
   SLang_Array_Type *xedges_at, *yedges_at, *xpts_at, *ypts_at;
   SLang_Array_Type *hist_at, *indices_at;
   SLang_Ref_Type *ref;
   SLindex_Type *rev_indices;
   int has_hist = 0;
   int type;
   SLindex_Type dims[2];

   ref = NULL;
   switch (SLang_Num_Function_Args)
     {
      case 6:
	has_hist = 1;
	if (SLang_peek_at_stack () == SLANG_NULL_TYPE)
	  (void) SLdo_pop ();
	else if (-1 == SLang_pop_ref (&ref))
	  return;
	break;

      case 5:
	type = SLang_peek_at_stack ();
	if (type == SLANG_REF_TYPE)
	  {
	     if (-1 == SLang_pop_ref (&ref))
	       return;
	  }
	else if (type == SLANG_NULL_TYPE)
	  (void) SLdo_pop ();
	else
	  has_hist = 1;
	break;

      case 4:
	break;

      default:
	SLang_verror (SL_USAGE_ERROR, "h = hist2d ([h,] xpnts, ypnts, xbins, ybins [,&reverse-indices])");
	return;
     }

   if (-1 == SLang_pop_array_of_type (&yedges_at, SLANG_DOUBLE_TYPE))
     {
	SLang_free_ref (ref);
	return;
     }

   if (-1 == SLang_pop_array_of_type (&xedges_at, SLANG_DOUBLE_TYPE))
     {
	SLang_free_array (yedges_at);
	SLang_free_ref (ref);
	return;
     }

   xpts_at = ypts_at = NULL;
   hist_at = NULL;
   indices_at = NULL;
   rev_indices = NULL;

   if (-1 == pop_hist2d_pts_array (&xpts_at, &ypts_at))
     goto free_and_return;

   if (has_hist && (SLang_peek_at_stack () == SLANG_NULL_TYPE))
     {
	(void) SLdo_pop ();
	has_hist = 0;
     }

   dims[0] = xedges_at->num_elements;
   dims[1] = yedges_at->num_elements;

   if (has_hist)
     {
	if (-1 == SLang_pop_array_of_type (&hist_at, HISTDATA_TYPE))
	  goto free_and_return;
	if (hist_at->flags & SLARR_DATA_VALUE_IS_READ_ONLY)
	  {
	     SLang_verror (SL_InvalidParm_Error, "Input histogram array is read-only");
	     goto free_and_return;
	  }
	if ((hist_at->num_dims != 2)
	    || (hist_at->dims[0] != dims[0])
	    || (hist_at->dims[1] != dims[1]))
	  {
	     SLang_verror (SL_InvalidParm_Error, "Input histogram array is not 2d or does not match grids");
	     goto free_and_return;
	  }
     }
   else if (NULL == (hist_at = SLang_create_array (HISTDATA_TYPE, 0, NULL, dims, 2)))
     goto free_and_return;
   /* hist_at->data is already initialized to 0 */

   if ((ref != NULL)
       && (NULL == (rev_indices = alloc_reverse_indices (xpts_at->num_elements))))
     goto free_and_return;

   switch (xpts_at->data_type)
     {
      case SLANG_UCHAR_TYPE:
	if (-1 == uc_histogram_2d ((unsigned char *)xpts_at->data, (unsigned char *)ypts_at->data, ypts_at->num_elements,
				   (double *)xedges_at->data, xedges_at->num_elements,
				   (double *)yedges_at->data, yedges_at->num_elements,
				   (HistData_Type *) hist_at->data, rev_indices))
	  goto free_and_return;
	break;

      case SLANG_INT_TYPE:
	if (-1 == i_histogram_2d ((int *)xpts_at->data, (int *)ypts_at->data, ypts_at->num_elements,
				  (double *)xedges_at->data, xedges_at->num_elements,
				  (double *)yedges_at->data, yedges_at->num_elements,
				  (HistData_Type *) hist_at->data, rev_indices))
	  goto free_and_return;
	break;

      case SLANG_FLOAT_TYPE:
	if (-1 == f_histogram_2d ((float *)xpts_at->data, (float *)ypts_at->data, ypts_at->num_elements,
				  (double *)xedges_at->data, xedges_at->num_elements,
				  (double *)yedges_at->data, yedges_at->num_elements,
				  (HistData_Type *) hist_at->data, rev_indices))
	  goto free_and_return;
	break;

      case SLANG_DOUBLE_TYPE:
	if (-1 == d_histogram_2d ((double *)xpts_at->data, (double *)ypts_at->data, ypts_at->num_elements,
				  (double *)xedges_at->data, xedges_at->num_elements,
				  (double *)yedges_at->data, yedges_at->num_elements,
				  (HistData_Type *) hist_at->data, rev_indices))
	  goto free_and_return;
	break;

      default:
	SLang_verror (SL_INTERNAL_ERROR, "Error in hist2d: array not supported");
	goto free_and_return;
     }

   if (ref != NULL)
     {
	if (NULL == (indices_at = convert_reverse_indices (rev_indices, xpts_at->num_elements, hist_at)))
	  goto free_and_return;

	if (-1 == SLang_assign_to_ref (ref, SLANG_ARRAY_TYPE, (VOID_STAR) &indices_at))
	  goto free_and_return;
     }

   (void) SLang_push_array (hist_at, 0);

   /* NULLs ok below */
free_and_return:
   if (rev_indices != NULL) SLfree ((char *) rev_indices);
   SLang_free_ref (ref);
   SLang_free_array (indices_at);
   SLang_free_array (yedges_at);
   SLang_free_array (xedges_at);
   SLang_free_array (xpts_at);
   SLang_free_array (ypts_at);
   SLang_free_array (hist_at);
}

static void binary_search_intrin (void)
{
   SLang_Array_Type *at;
   SLang_Array_Type *xt;
   SLang_Array_Type *at_ind;
   double x, *xp, *yp;
   SLuindex_Type i, num_indices, num;
   SLindex_Type ind, *indices;

   if (SLang_Num_Function_Args != 2)
     {
	SLang_verror (SL_USAGE_ERROR, "i = hist_bsearch (x, a); %% a[i]<=x<a[i+1]");
	return;
     }

   if (-1 == SLang_pop_array_of_type (&at, SLANG_DOUBLE_TYPE))
     return;

   if (SLang_peek_at_stack () == SLANG_ARRAY_TYPE)
     {
	if (-1 == SLang_pop_array_of_type (&xt, SLANG_DOUBLE_TYPE))
	  {
	     SLang_free_array (at);
	     return;
	  }

	if (NULL == (at_ind = SLang_create_array (SLANG_INT_TYPE, 0, NULL, xt->dims, xt->num_dims)))
	  {
	     SLang_free_array (at);
	     SLang_free_array (xt);
	     return;
	  }
	xp = (double *) xt->data;
	num_indices = xt->num_elements;
	indices = (SLindex_Type *)at_ind->data;
     }
#if SLANG_VERSION < 20000
   else if (0 == SLang_pop_double (&x, NULL, NULL))
#else
   else if (0 == SLang_pop_double (&x))
#endif
     {
	xt = NULL;
	at_ind = NULL;
	xp = &x;
	num_indices = 1;
	indices = &ind;
     }
   else
     {
	SLang_free_array (at);
	return;
     }

   num = at->num_elements;
   yp = (double *)at->data;

   if (-1 == check_grid (yp, num))
     {
	SLang_free_array (at);
	SLang_free_array (xt);
	return;
     }

   for (i = 0; i < num_indices; i++)
     indices[i] = double_binary_search (xp[i], yp, num);

   SLang_free_array (at);
   SLang_free_array (xt);

   if (at_ind != NULL)
     (void) SLang_push_array (at_ind, 1);
   else
     (void) SLang_push_array_index (indices[0]);
}

/* Usage: h_new = rebin (new_grid, old_grid, h_old); */
static void hist1d_rebin (void)
{
   SLang_Array_Type *grid_old, *h_old, *grid_new, *h_new;
   SLindex_Type old_num_bins, new_num_bins;

   if (SLang_Num_Function_Args != 3)
     {
	SLang_verror (SL_USAGE_ERROR, "h_new = hist1d_rebin (new_grid, old_grid, h_old)");
	return;
     }

   if (-1 == pop_1d_double_arrays (&grid_old, &h_old))
     return;

   if (-1 == pop_1d_array_of_type (&grid_new, SLANG_DOUBLE_TYPE))
     {
	SLang_free_array (h_old);
	SLang_free_array (grid_old);
	return;
     }

   old_num_bins = grid_old->num_elements;
   new_num_bins = grid_new->num_elements;

   if (NULL == (h_new = SLang_create_array (SLANG_DOUBLE_TYPE, 0, NULL, &new_num_bins, 1)))
     {
	SLang_free_array (grid_new);
	SLang_free_array (grid_old);
	SLang_free_array (h_old);
	return;
     }

   if (0 == d_hist1d_rebin ((double *)grid_new->data, (SLuindex_Type) new_num_bins,
			    (double *)grid_old->data, (double *)h_old->data, (SLuindex_Type) old_num_bins,
			    (double *)h_new->data))
     (void) SLang_push_array (h_new, 0);

   SLang_free_array (h_new);
   SLang_free_array (grid_new);
   SLang_free_array (grid_old);
   SLang_free_array (h_old);
}

#define A SLANG_ARRAY_TYPE
#define V SLANG_VOID_TYPE

static SLang_Intrin_Fun_Type Module_Intrinsics [] =
{
   MAKE_INTRINSIC_0("hist1d", hist1d, V),
   MAKE_INTRINSIC_0("hist2d", hist2d, V),
   MAKE_INTRINSIC_0("hist_bsearch", binary_search_intrin, V),
   MAKE_INTRINSIC_0("hist1d_rebin", hist1d_rebin, V),
   SLANG_END_INTRIN_FUN_TABLE
};

static SLang_Intrin_Var_Type Module_Variables [] =
{
   MAKE_VARIABLE("_histogram_module_version_string", &Module_Version_String, SLANG_STRING_TYPE, 1),
   SLANG_END_INTRIN_VAR_TABLE
};

static SLang_IConstant_Type Module_IConstants [] =
{
   MAKE_ICONSTANT("_histogram_module_version", MODULE_VERSION_NUMBER),
   SLANG_END_ICONST_TABLE
};

int init_histogram_module_ns (char *ns_name)
{
   SLang_NameSpace_Type *ns = SLns_create_namespace (ns_name);
   if (ns == NULL)
     return -1;

   if (
       (-1 == SLns_add_intrin_var_table (ns, Module_Variables, NULL))
       || (-1 == SLns_add_intrin_fun_table (ns, Module_Intrinsics, NULL))
       || (-1 == SLns_add_iconstant_table (ns, Module_IConstants, NULL))
       )
     return -1;

   return 0;
}

/* This function is optional */
void deinit_histogram_module (void)
{
}
