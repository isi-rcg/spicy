/* -*- C -*- */
/*
 * A 1-d histogram is specified by a set of N grid points X_k and some set
 * of values y_i to be grouped into the histogram.  The bin size of the
 * nth bin is given by x_{i+1} - x_i, except for the last bin, which is
 * assumed to be of infinite width.
 */

/* If reverse_indices is NON-NULL, it is assumed to point to an array of 
 * size npts.
 * 
 * This routines assumes that the caller has initialized the histogram
 * array to 0, and the reverse_indices array to -1.
 */

#ifdef HISTOGRAM_1D
static int HISTOGRAM_1D (PTS_TYPE *pts, SLuindex_Type npts,
			 BIN_EDGES_TYPE *bin_edges, SLuindex_Type nbins,
			 HistData_Type *histogram,
			 SLindex_Type *reverse_indices)
{
   SLuindex_Type i, nbins_m1;
   double xlo, xhi, dx;

   if (nbins == 0)
     return 0;

   if (-1 == check_grid (bin_edges, nbins))
     return -1;

   nbins_m1 = nbins - 1;
   xlo = (double) bin_edges[0];
   xhi = (double) bin_edges[nbins_m1];
   dx = xhi - xlo;

   if (dx < 0.0)
     {
	SLang_verror (SL_INVALID_PARM, "hist1d: bin edges array is not in increasing order");
	return -1;
     }

   for (i = 0; i < npts; i++)
     {
	PTS_TYPE val = pts[i];
	SLuindex_Type j;

	if (
#if CHECK_NANS
	    isnan(val) ||
#endif
	    (val < xlo))
	  continue;

	if (val >= xhi)
	  j = nbins_m1;
	else
	  {
	     /* Try linear interpolation since many grids will be linear */
	     j = (SLuindex_Type) (((val - xlo)/dx)*nbins_m1);
	     if (j == nbins_m1) j--;
	     if ((bin_edges[j] > val) || (val >= bin_edges[j+1]))
	       j = BINARY_SEARCH (val, bin_edges, nbins);
	  }
	histogram[j] += 1;
	if (reverse_indices != NULL)
	  reverse_indices[i] = (SLindex_Type) j;
     }
   return 0;
}
#undef HISTOGRAM_1D
#endif				       /* HISTOGRAM_1D */

#ifdef HISTOGRAM_2D
static int HISTOGRAM_2D (PTS_TYPE *xpts, PTS_TYPE *ypts, SLuindex_Type npts,
			 BIN_EDGES_TYPE *xbin_edges, SLuindex_Type nxbins, 
			 BIN_EDGES_TYPE *ybin_edges, SLuindex_Type nybins,
			 HistData_Type *histogram,
			 SLindex_Type *reverse_indices)
{
   SLuindex_Type i, nxbins_m1, nybins_m1;
   double xlo, xhi, dx;
   double ylo, yhi, dy;

   if ((nxbins == 0) || (nybins == 0))
     return 0;

   if (-1 == check_grid (xbin_edges, nxbins))
     return -1;

   if (-1 == check_grid (ybin_edges, nybins))
     return -1;

   nxbins_m1 = nxbins - 1;
   xlo = (double) xbin_edges[0];
   xhi = (double) xbin_edges[nxbins_m1];
   dx = xhi - xlo;

   nybins_m1 = nybins - 1;
   ylo = (double) ybin_edges[0];
   yhi = (double) ybin_edges[nybins_m1];
   dy = yhi - ylo;
   
   if ((dx < 0.0) || (dy < 0.0))
     {
	SLang_verror (SL_INVALID_PARM, "hist2d: bin edges array is not in increasing order");
	return -1;
     }

   for (i = 0; i < npts; i++)
     {
	PTS_TYPE xval = xpts[i];
	PTS_TYPE yval = ypts[i];
	SLuindex_Type jx, jy, j;

	if (
#if CHECK_NANS
	    isnan(xval) || isnan(xval) ||
#endif
	    (xval < xlo) || (yval < ylo))
	  continue;

	if (xval >= xhi)
	  jx = nxbins_m1;
	else
	  {
	     /* Try linear interpolation since many grids will be linear */
	     jx = (SLuindex_Type) (((xval - xlo)/dx)*nxbins_m1);
	     if (jx == nxbins_m1) jx--;
	     if ((xbin_edges[jx] > xval) || (xval >= xbin_edges[jx+1]))
	       jx = BINARY_SEARCH (xval, xbin_edges, nxbins);
	  }

	if (yval >= yhi)
	  jy = nybins_m1;
	else
	  {
	     /* Try linear interpolation since many grids will be linear */
	     jy = (SLuindex_Type) (((yval - ylo)/dy)*nybins_m1);
	     if (jy == nybins_m1) jy--;
	     if ((ybin_edges[jy] > yval) || (yval >= ybin_edges[jy+1]))
	       jy = BINARY_SEARCH (yval, ybin_edges, nybins);
	  }

	j = jx*nybins + jy;
	histogram[j] += 1;
	if (reverse_indices != NULL)
	  reverse_indices[i] = (int) j;
     }
   return 0;
}
#undef HISTOGRAM_2D
#endif

#ifdef HISTOGRAM_REBIN
static int HISTOGRAM_REBIN (double *new_grid, SLuindex_Type new_n,
			    double *old_grid, PTS_TYPE *old_h, SLuindex_Type old_n,
			    PTS_TYPE *new_h)
{
   SLuindex_Type i, imax;
   SLuindex_Type j, jmax;
   double *old_left, *old_right, *new_left, *new_right;

   if ((new_n == 0) || (old_n == 0))
     return 0;
   
   for (i = 0; i < new_n; i++)
     new_h[i] = 0;

   old_left = old_grid;
   old_right = old_grid + 1;
   new_left = new_grid;
   new_right = new_grid + 1;

   imax = new_n - 1;
   jmax = old_n - 1;
   
   if (-1 == check_grid (old_grid, old_n))
     return -1;

   if (-1 == check_grid (new_grid, new_n))
     return -1;

   i = j = 0;
   if (j < jmax)
     {
	double r_new, l_new, r_old, l_old;
	double h_rho;

	r_old = old_right[0];
	l_old = old_left [0];
	l_new = new_left [0];
	if (i == imax)
	  r_new = old_left[jmax];
	else
	  r_new = new_right[0];
	
	if (r_old > l_old)
	  h_rho = old_h[j] / (r_old - l_old);
	else
	  h_rho = 0.0;

	while (1)
	  {
	     if (r_new < r_old)
	       {
		  if (l_new >= l_old) 
		    new_h[i] += h_rho * (r_new - l_new);
		  else if (r_new > l_old)
		    new_h[i] += h_rho * (r_new - l_old);
		  
		  if (i != imax)
		    {
		       i++;
		       l_new = r_new;
		       if (i == imax)
			 r_new = old_left[jmax];
		       else
			 r_new = new_right[i];
		    }
	       }
	     else
	       {
		  if (l_new < l_old) 
		    new_h[i] += old_h [j];
		  else if (l_new < r_old) 
		    new_h[i] += h_rho * (r_old - l_new);

		  j++;
		  if (j == jmax)
		    break;
		  l_old = r_old;
		  r_old = old_right[j];
		  if (r_old > l_old)
		    h_rho = old_h[j] / (r_old - l_old);
		  else
		    h_rho = 0.0;
	       }
	  }
     }
	
   /* Now take care of the last bin, which is of infinite length.  Because of
    * its infinite length, it gets all of the stuff in the input histograms
    * last bin.
    */
   new_h [imax] += old_h [jmax];
   return 0;
}
#undef HISTOGRAM_REBIN
#endif

#undef PTS_TYPE
#undef BIN_EDGES_TYPE
#undef BINARY_SEARCH
#undef CHECK_NANS
