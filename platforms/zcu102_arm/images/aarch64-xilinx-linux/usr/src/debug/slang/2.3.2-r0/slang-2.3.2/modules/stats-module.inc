/*-*-C-*-*/

#ifdef MEAN_FUNCTION
static int MEAN_FUNCTION (VOID_STAR xp, unsigned int inc, unsigned int num, VOID_STAR yp)
{
   GENERIC_TYPE *x, *xmax;
   double c, m, err;

   x = (GENERIC_TYPE *) xp;
   xmax = x + num;

   num = num/inc;
   if (num == 0)
     return 0;

   c = (double) *x;
   if (num == 1)
     {
	*(MEAN_RESULT_TYPE *)yp = (MEAN_RESULT_TYPE) c;
	return 0;
     }

   err = 0.0;
   m = c;

   while (x < xmax)
     {
	double v = *x;
	double dm = (v - c)/num;
	double m1 = m + dm;
	err += dm - (m1 - m);
	m = m1;
	x += inc;
     }
   *(MEAN_RESULT_TYPE *)yp = (MEAN_RESULT_TYPE) (m + err);
   return 0;
}
# undef MEAN_FUNCTION
# undef MEAN_RESULT_TYPE
#endif

#ifdef STDDEV_FUNCTION
static int
STDDEV_FUNCTION (VOID_STAR xp, unsigned int inc, unsigned int num, VOID_STAR s)
{
   unsigned int i, n;
   double mean_i, variance_i;
   GENERIC_TYPE *x = (GENERIC_TYPE *) xp;
   double err;

   err = mean_i = variance_i = 0.0;
   n = 1;
   for (i = 0; i < num; i += inc)
     {
	double diff, x_i;
	double dv, v1;

	x_i = x[i];
	diff = x_i - mean_i;
	mean_i += diff / n;
	dv = diff * (x_i - mean_i);
	v1 = variance_i + dv;
	err += dv - (v1 - variance_i);
	variance_i = v1;
	n++;
     }
   variance_i += err;
   n--;
   if (n > 1)
     variance_i = sqrt (variance_i / (n - 1));
   else
     variance_i = 0.0;

   *(STDDEV_RESULT_TYPE *)s = (STDDEV_RESULT_TYPE) variance_i;

   return 0;
}
#undef STDDEV_RESULT_TYPE
#undef STDDEV_FUNCTION
#endif

/*
 * The following code is public domain.
 * Algorithm by Torben Mogensen, implementation by N. Devillard.
 * This code in public domain.
 */
#ifdef NC_MEDIAN_FUNCTION
static int NC_MEDIAN_FUNCTION (VOID_STAR ap, unsigned int inc, unsigned int num, VOID_STAR y)
{
   unsigned int i, n, m;
   GENERIC_TYPE *a;
   GENERIC_TYPE min, max, guess, maxltguess, mingtguess;
   unsigned int less, greater, equal;

   n = num/inc;
   if (n == 0)
     {
	SLang_set_error (SL_INVALID_PARM);
	return -1;
     }
   a = (GENERIC_TYPE *)ap;
   m = (n + 1)/2;

   min = max = a[0];
   for (i = 0; i < num; i += inc)
     {
	GENERIC_TYPE ai = a[i];
        if (ai < min) min = ai;
        if (ai > max) max = ai;
    }

   while (1)
     {
	less = 0; greater = 0; equal = 0;
	/* We want guess=(min+max)/2.  But min+max could overflow.  So use
	 *  (min+(max-min)+min)/2 = min + (max-min)/2
	 */
	guess = min + (max-min)/2;
        maxltguess = min;
        mingtguess = max;

        for (i=0; i < num; i += inc)
	  {
	     GENERIC_TYPE ai = a[i];
	     if (ai<guess)
	       {
		  less++;
		  if (ai>maxltguess) maxltguess = ai;
		  continue;
	       }
	     if (ai > guess)
	       {
		  greater++;
		  if (ai < mingtguess) mingtguess = ai;
		  continue;
	       }
	     equal++;
	  }
        if ((less <= m) && (greater <= m))
	  break;

        if (less > greater)
	  max = maxltguess;
        else
	  min = mingtguess;
     }
   if (less >= m)
     guess = maxltguess;
   else if (less + equal < m)
     guess = mingtguess;

   *(GENERIC_TYPE *)y = guess;
   return 0;
}
#undef NC_MEDIAN_FUNCTION
#endif

#ifdef MEDIAN_FUNCTION
static int
MEDIAN_FUNCTION (VOID_STAR ap, unsigned int inc, unsigned int num, VOID_STAR y)
{
   GENERIC_TYPE *aa = (GENERIC_TYPE *) ap;
   GENERIC_TYPE *a;
   unsigned int i, j, k, l, m, n;

   n = num/inc;
   if (n < 3)
     {
	if (n == 0)
	  {
	     SLang_set_error (SL_INVALID_PARM);
	     return -1;
	  }
	if ((n == 1)
	    || (*aa < *(aa + inc)))
	  {
	     *(GENERIC_TYPE *)y = *aa;
	     return 0;
	  }
	*(GENERIC_TYPE *)y = *(aa + inc);
	return 0;
     }

   if (NULL == (a = (GENERIC_TYPE *) SLmalloc (sizeof (GENERIC_TYPE)*n)))
     return -1;

   for (i = 0; i < n; i++)
     {
	a[i] = *aa;
	aa += inc;
     }

   if (n & 1)
     k = n/2;
   else
     k = n/2 - 1;

   l = 0;
   m = n - 1;

   while (l < m)
     {
        GENERIC_TYPE x = a[k];
        i = l;
        j = m ;
        do
	  {
	     while (a[i] < x) i++;
	     while (x < a[j]) j--;
	     if (i <= j)
	       {
		  GENERIC_TYPE tmp = a[i];
		  a[i] = a[j];
		  a[j] = tmp;
		  i++;
		  j--;
	       }
	  }
	while (i<=j);

        if (j < k) l=i;
        if (k < i) m=j;
     }
   *(GENERIC_TYPE *) y = a[k];
   SLfree ((char *) a);
   return 0;
}
#undef MEDIAN_FUNCTION
#endif
#undef GENERIC_TYPE
