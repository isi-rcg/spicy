/* -*- mode: C; mode: fold; -*- */

/*
  Copyright (c) 2007 Massachusetts Institute of Technology

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

/* Author: John E. Davis <jed@jedsoft.org> */

#include "config.h"
#include <stdio.h>
#include <slang.h>
#include <math.h>
#include <string.h>

#include "stats-module.h"

#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif
/* <MODULE_INCLUDES> */

#ifdef __cplusplus
extern "C"
{
#endif
SLANG_MODULE(stats);
#ifdef __cplusplus
}
#endif

/* typedef unsigned long long uint64_t;
 * typedef long long int64_t;
 */

#define MODULE_VERSION_STRING "1.1.0"
#define MODULE_VERSION_NUMBER 10100

#if 0
double anderson_darling_statistic (double *f, unsigned int num)
{
   double s;
   double n;

   n = (double)num;
   for (i = 0; i < n; i++)
     {
	((2*i+1)/n)* (log(f[i]) + log(f[n-i]));
     }

}
#endif

/* Computes (n k) = n!/(k! (n-k)!) */
static double compute_binomial_coeff (unsigned int n, unsigned int k) /*{{{*/
{
   double f;
   unsigned int i;

   if (k > n)
     return 0.0;
   if ((k == 0) || (k == n))
     return 1.0;

   if (n-k < k)
     k = n-k;

   f = n;
   for (i = 2; i <= k; i++)
     {
	n--;
	f = (f/i)*n;
     }
   return f;
}

/*}}}*/

/* This function implements the _asymptotic_ form of the kolmogorov distribution
 * for large N.  Asymptotically, this is given by the series expansion:
 *
 *   S(x) = 1 + 2\sum_{j=1}^{\infty} (-1)^j \exp(-2j^2 x^2)
 *
 * This is equivalent to
 *
 *   S(x) = \sqrt{2\pi}/x \sum_{j=1}^\infty \exp[-(2j-1)^2\pi^2/(8x^2)]
 *
 * See eq. 1.4 of W. Feller, "On the Kolmogorov-Smirnov limit theorems for
 * empirical distributions", Annals of Math. Stat, Vol 19 (1948), pp. 177-190.
 *
 * S(x) represents the prob of obtaining a (normalized) statistic <= x.
 *
 * I chose the value of 1.09 was chosen based upon some emperical testing. Of
 * course this value will most likely vary with compiler.
 */
static double smirnov_cdf (double x) /*{{{*/
{
   int j;
   unsigned int max_loops;
   double a, b, sum;
   max_loops = 5000;

   if (x <= 0.15)
     {
	if (x < 0.0)
	  {
	     SLang_set_error (SL_INVALID_PARM);
	     return -1.0;
	  }
	return 0.0;
     }

   if (x > 1.09)
     {
        int j3;

	if (x > 19.4)
	  return 1.0;

	x = 2*x*x;
	sum = 0.0;
	j = 1;
	j3 = 3;
	while (max_loops)
	  {
	     double dsum = exp(-j*j*x) * (1.0 - exp (-j3*x));
	     sum += dsum;
	     if (dsum == 0)
	       return 1-2*sum;

	     j += 2;
	     j3 += 4;
	     max_loops--;
	  }
	return 1.0;
     }

   /* Use sqrt(X) = 0.5*log(X) */
   b = log(sqrt(2*PI)/x);
   a = (PI*PI/(8.0*x*x));

   sum = 0.0;
   j = 1;
   while (max_loops)
     {
	double dsum = exp (-a*j*j + b);
	sum += dsum;
	if (dsum == 0.0)
	  return sum;
	j += 2;
	max_loops--;
     }
   return 0.0;
}

/*}}}*/

/* This gives the probability that an observed value of D_mn <= c/(mn).
 */
static double kim_jennrich_cdf (unsigned int m, unsigned int n, unsigned int c) /*{{{*/
{
   unsigned int i, j;
   double p;
   double *u;

   if (m > n)
     {
	unsigned int tmp = m;
	m = n;
	n = tmp;
     }

   u = (double *)SLmalloc (sizeof (double)*(n+1));
   if (u == NULL)
     return -1.0;

   u[0] = 1.0;
   for (j = 1; j <= n; j++)
     {
	u[j] = 1.0;
	if (m*j > c)
	  u[j] = 0.0;
     }

   for (i = 1; i <= m; i++)
     {
	double w = i/(i + (double)n);
	unsigned int ni = n*i;

	u[0] = w*u[0];
	if (ni > c) u[0] = 0;
	for (j = 1; j <= n; j++)
	  {
	     unsigned int mj = m*j;
	     unsigned int diff;

	     diff = (ni >= mj) ? (ni-mj) : (mj-ni);
	     u[j] = (diff > c) ? 0.0 : u[j-1]+u[j]*w;
	  }
     }
   p = u[n];
   if (p > 1.0) p = 1.0;
   else if (p < 0.0) p = 0.0;

   SLfree ((char *)u);
   return p;
}

/*}}}*/

/* P(X<=s) */
static double mann_whitney_cdf (unsigned int m, unsigned int n, unsigned int s) /*{{{*/
{
   unsigned int M, p;
   unsigned int m_plus_n, t, u;
   double *f, c, cf;

   /* min allowed s: 1+2+...m = m(m+1)/2 = min
    * max allowed s: (n+1)+(n+2)+ .. (n+m) = n*m + min
    * avg = (min+max)/2 = nm/2 + m(m+1)/2 = (m/2)[n+ m+1]
    */
   M = m*(m+1)/2;
   if (s < M)
     return 0.0;
   M += m*n;
   if (M <= s)
     return 1.0;

   M = m*n/2;

   f = (double *)SLmalloc ((M+1)*sizeof(double));
   if (f == NULL)
     return -1.0;
   f[0] = 1.0;
   for (p = 1; p <= M; p++)
     f[p] = 0.0;

   m_plus_n = m + n;
   if (n + 1 < M)
     {
	p = m_plus_n;
	if (p > M) p = M;
	for (t = n + 1; t <= p; t++)
	  {
	     for (u = M; u >= t; u--)
	       f[u] -= f[u-t];
	  }
     }
   p = m;
   if (M < p) p = M;
   for (t = 1; t <= p; t++)
     {
	for (u = t; u <= M; u++)
	  f[u] += f[u-t];
     }

   c = compute_binomial_coeff (m+n, m);
   cf = 0.0;
   for (t = 0; t <= M; t++)
     {
	cf += f[t]/c;
	f[t] = cf;
     }

   s -= m*(m+1)/2;
   if (s > M)
     {
	s = m*n - s;
	c = 1.0 - f[s];
     }
   else c = f[s];
   SLfree ((char *) f);
   return c;
}

/*}}}*/

/*{{{ Gamma Function */

#ifdef HAVE_LGAMMA
# define JDMlog_gamma lgamma
#else
/* The following code is adapted from my JDMath library */

/* This implementation is derived from
 * Spouge JL. Computation of the gamma, digamma, and trigamma
 *    functions. SIAM J Numerical Anal 1994;31:931-44.
 * as pointed out by Glynne Casteel <glynnec@ix.netcom.com> in the
 * sci.math.num-analysis article <6potip$l56@sjx-ixn4.ix.netcom.com>.
 */

#define NUM_COEFFS 18		       /* make this even */
static double Param = NUM_COEFFS + 1;  /* 0.840 */
/* determined empirically using the driver in main at end of the file. */

static double Coeffs[NUM_COEFFS+1];

static int Coeffs_Initialized = 0;

static void init_coefficients (void)
{
   int i;
   double e = 2.718281828459045235360287471352;
   double f = 2.506628274631000502415765284809;   /* sqrt(2pi) */

   Coeffs[0] = exp (-Param)*f;
   Coeffs[1] = sqrt (Param-1)/e;
   for (i = 1; i < NUM_COEFFS; i++)
     {
	register double x = Param - i;

	Coeffs[i+1] = Coeffs[i]
	  * ((x - 1)*pow (1-1/x,i-0.5)/(i*e));
     }
   Coeffs_Initialized = 1;
}

static double JDMlog_gamma (double x)
{
   register double sum;
   unsigned int i;

   if (Coeffs_Initialized == 0)
     init_coefficients ();

   x -= 1.0;

   sum = Coeffs[0];
   i = 1;
   while (i < NUM_COEFFS)
     {
	register double dsum;

	dsum = Coeffs[i]/(x+i);
	i++;
	sum += (dsum - Coeffs[i]/(x+i));
	i++;
     }

   return log(sum) + ((x+0.5)*log(x+Param) - x);
}
#endif				       /* HAVE_LGAMMA */

/* See A&S 6.5.4 for a definition.  Here, the series expansion given by
 * A&S 6.5.29 is used:
 *
 *    gamma_star(a,x) = exp(-x) \sum_n x^n/Gamma(a+n+1)
 *
 * Here, Gamma(a+1)=a*Gamma(a), Gamma(a+2)=(a+1)*a*Gamma(a), ....
 * Thus,
 *
 *    gamma_star(a,x) = exp(-x)/Gamma(a) \sum_n x^n c_n
 *
 * where c_n = 1/(a(a+1)...(a+n)).  For a > 0, c_n --> 0, hence based upon
 * this c_n, the sum will terminate.  However, x^n may overflow before this
 * happens.  However, as long as |x| < a, this should not happen.  So avoid
 * this function if |x| > a.
 */
static double JDMlog_gamma_star (double a, double x)
{
   double sum;
   int n;
   double cnxn;
   double eps = 2.220446049250313e-16;

   if (a == 0.0)
     return 0.0;

   if (a < 0.0)
     {
	/* FIXME!!! Handle a < 0 */
     }

   cnxn = 1.0 / a;
   sum = cnxn;
   n = 0;
   while (n < 5000)
     {
	n++;
	cnxn *= x/(a+n);
	if (cnxn < sum*eps)
	  break;
	sum += cnxn;
     }

   return log (sum) - x - JDMlog_gamma (a);
}

/* This incomplete gamma function has a continued fraction approximation
 * given by A&S 6.5.31.  This expansion may be used to compute the incomplete
 * Gamma function (A&S 6.5.1) via A&S 6.5.2,3:
 *    Gamma(a,x) = Gamma(a) - P(a,x) Gamma(a)
 *               = Gamma(a) - gamma(a,x)
 * or
 *    P(a,x) = 1 - Gamma(a,x)/Gamma(a).
 *
 * The continued fraction expansion (A&S 6.5.31) is given by:
 *
 *    Gamma(a,x) = exp(-x)x^a (1/x+ (1-a)/1+ 1/x+ (2-a)/1+ 2/x+ ...)
 *
 * Use the recursion relations in theorem 2 of A&S 3.10.1 to evaluate this.
 * That is, let f_n be defined as
 *
 *    f_n = b_0 + a_1/b_1+ a_2/b_2+...a_n/b_n := A_n/B_n
 *
 * Then:
 *
 *    A_n = b_n A_{n-1} + a_n A_{n-2}
 *    B_n = b_n B_{n-1} + a_n B_{n-2}
 *
 * where A_{-1}=1, A_0=b_0, B_{-1}=0, B_0=1
 */
static double JDMlog_CapGamma (double a, double x)
{
   double a1, a0, b0, b1;
   double f;
   double renorm_factor;
   double eps = 2.220446049250313e-16;
   int n;

   if (x <= 0.0)
     return log(x);		       /* NaN/-Inf */

   /* Set up the recursion at the initial 1/x+ piece */
   a1 = 1;
   b1 = x;
   a0 = 0;
   b0 = 1;

   renorm_factor = 1.0/b1;
   f = a1 * renorm_factor;
   n = 1;

   if (renorm_factor != 0.0) while (n < 5000)
     {
	double f1, aa;

	/* Note that the renormalization factor is 1/b1.
	 * So, replace renorm_factor*b1 combinations by 1 */

	aa = n - a;
	/* Now the (n-a)/(1+??) piece */
	a0 = (a1 + aa * a0) * renorm_factor;
	b0 = (b1 + aa * b0) * renorm_factor;

	/* Since a0,b0 now have the renorm_factor applied, omit it below */
	/* Handle {1,2,3...}/(x+??) piece */
	a1 = x * a0 + n * a1 * renorm_factor;
	/* b1 = x * b0 + n * b1 * renorm_factor;*/
	b1 = x * b0 + n;

	n++;

	if (b1 == 0.0)
	  continue;

	renorm_factor = 1.0 / b1;
	f1 = a1 * renorm_factor;

	if (fabs (f - f1) < eps * fabs(f1))
	  {
	     f = f1;
	     break;
	  }
	f = f1;
     }

   return a * log(x) - x + log (f);
}

/* See A&S 6.5.1
 * This is P(a,x) = gamma(a,x)/Gamma(a)
 */
static double JDMincomplete_gamma (double a, double x)
{
   if (a <= 0.0)
     return log(a);

   if (x >= a)
     return 1.0 - exp (JDMlog_CapGamma (a,x) - JDMlog_gamma (a));

   return exp (a * log(x) + JDMlog_gamma_star (a, x));
}

/*}}}*/

/*{{{ Incomplete Beta */

static int incbeta_cfe (double x, double a, double b, double *result)
{
   unsigned int m, maxm = 1024;
   double c = a+b;
   double alpha_neg2, alpha_neg1, beta_neg2, beta_neg1;
   double eps = 1e-14;
   double last_f;
   double factor;

   /* factor = a*log(x)+b*log(1.0-x); */
   factor = a*log(x)+b*log1p(-x);
   factor += JDMlog_gamma(a+b)-JDMlog_gamma(a) - JDMlog_gamma(b);
   factor = exp (factor)/a;

   /* F = 1/(1+(d1/(1+(d2/(1+....)))))
    * F0 = 1/1
    * F1 = 1/1+ d1
    * F2 = 1/1+ d1/1+ d2
    * F_n = alpha_n/beta_n
    * alpha_n = alpha_{n-1} + d_n*alpha_{n-2}
    * beta_n = beta_{n-1} + d_n*beta_{n-2}
    * (d0 = 1)
    * alpha_{-2} = 1, alpha{-1} = 0, alpha_{0} = 1, alpha_1 = 1,
    * beta_{-2} = 0, beta_{-1} = 1, beta_{0} = 1, beta_1 = (1+d1),
    * alpha_2 = 1+d2
    * beta_2 = (1+d1+d2)
    *   alpha_2/beta_2 = 1/(1+d1/(1+d2)) ok
    */

   /* This loop computes results for d2, d3, ...  So initialize for d1 term */
   alpha_neg2 = 1.0; alpha_neg1 = 1.0;
   beta_neg2 = 1.0; beta_neg1 = 1.0-c/(a+1)*x;
   last_f = alpha_neg1/beta_neg1;

   m = 1;
   while (m < maxm)
     {
	double d,g;
	unsigned int m2 = 2*m;

	g = a+m2;

	/* even d_2m */
	d = (m*(b-m)/((g-1.0)*g))*x;
	alpha_neg2 = alpha_neg1 + d*alpha_neg2;
	beta_neg2 = beta_neg1 + d*beta_neg2;

	/* odd d_{2m+1} */
	d = -(((a+m)*(c+m))/(g*(g+1.0)))*x;
	alpha_neg1 = alpha_neg2 + d*alpha_neg1;
	beta_neg1 = beta_neg2 + d*beta_neg1;

	/* Renormalize to prevent large alpha/beta */
	alpha_neg1 /= beta_neg1;
	alpha_neg2 /= beta_neg1;
	beta_neg2 /= beta_neg1;
	beta_neg1 = 1.0;

	if (fabs(alpha_neg1 - last_f) < eps * fabs(alpha_neg1))
	  {
	     *result = alpha_neg1*factor;
	     return 0;
	  }
	last_f = alpha_neg1;
	m++;
     }

   *result = last_f*factor;
   return -1;
}

static int incbeta (double x, double a, double b, double *result)
{
   double cfe;
   int status;

   if ((x < 0.0) || (x > 1.0))
     {
	SLang_verror (SL_INVALID_PARM, "Domain error for x in incbeta");
	*result = -1;
	return -1;
     }

   if ((x == 0.0) || (x == 1.0))
     {
	*result = x;
	return 0;
     }

   /* Use 26.5.8 of A&S --- Note that this condition is different from that
    * of A&S --- GSL uses this one and it seems to work better.
    */
   if (x * (a+b+2.0) < (a+1.0))
     {
	status = incbeta_cfe (x, a, b, &cfe);
     }
   else
     {
	/* Use I_x(a,b) = 1.0 - I_{1-x}(b,a) */
	status = incbeta_cfe (1.0-x, b, a, &cfe);
	cfe = 1.0 - cfe;
     }

   *result = cfe;
   return status;
}

/*}}}*/

/*{{{ Intrinsics */

/* See A&S 6.5.13
 *
 * This computes: \sum_{0<=k<=n} lambda^k/k! exp(-lambda)
 * From A&S 6.5.13, this is just 1-P(n+1,lambda)
 */
static double poisson_cdf_intrin (double *lambdap, int *np)
{
   int n = *np;
   double lambda = *lambdap;

   if (n < 0)
     return 0.0;

   n = n+1;
   if (lambda > 1000.0)
     {
	double dn = sqrt(n);
	if (fabs(lambda - n) < dn)
	  {
	     /* Wilson and Hilferty, 1951 */
	     double c = pow (lambda/n, 1.0/3.0);
	     double mu = 1.0-1.0/(9.0*n);
	     double s = 1.0/(3*dn);

	     return 0.5 * (1 - erf((c-mu)/s/sqrt(2.0)));
	  }
	/* drop */
     }


   return 1.0 - JDMincomplete_gamma ((double)n, lambda);
}

/* P(X^2 < chisqr)
 * A&S 26.4.19
 */
static double chisqr_cdf_intrin (int *dofp, double *chisqrp)
{
   int dof = *dofp;
   double chisqr = *chisqrp;

   if (dof <= 0)
     {
	SLang_verror (SL_INVALID_PARM, "The number of degrees of freedom should be positive");
	return -1.0;
     }
   if (chisqr < 0)
     {
	SLang_verror (SL_INVALID_PARM, "Expecting a non-negative value for the chi-square statistic");
	return -1.0;
     }
   return JDMincomplete_gamma (0.5*dof, 0.5*chisqr);
}

static double smirnov_cdf_intrin (double *x)
{
   return smirnov_cdf (*x);
}

static double kim_jennrich_cdf_intrin (unsigned int *m, unsigned int *n, unsigned int *c)
{
   return kim_jennrich_cdf (*m, *n, *c);
}

static double mann_whitney_cdf_intrin (unsigned int *m, unsigned int *n, double *s)
{
   unsigned int sn = (unsigned int) (*s + 0.5);
   return mann_whitney_cdf (*m, *n, sn);
}

/* df needs to be real valued for the welch test */
static double student_t_cdf_intrin (double *t, double *dfp)
{
   double x = *t;
   double n = *dfp;
   double cdf;

   (void) incbeta (n/(n+x*x), 0.5*n, 0.5, &cdf);
   cdf *= 0.5;
   if (x > 0)
     cdf = 1.0 - cdf;
   return cdf;
}

static double f_cdf_intrin (double *t, double *nu1p, double *nu2p)
{
   double x = *t;
   double cdf;
   double nu1 = *nu1p, nu2 = *nu2p;

   if (x < 0.0)
     return 0.0;

   (void) incbeta (nu2/(nu2+nu1*x), 0.5*nu2, 0.5*nu1, &cdf);
   return 1.0-cdf;
}

/*}}}*/

/* Mean, Stdev, etc... */
#define GENERIC_TYPE double
#define MEAN_FUNCTION mean_doubles
#define MEAN_RESULT_TYPE double
#define STDDEV_FUNCTION stddev_doubles
#define STDDEV_RESULT_TYPE double
#define MEDIAN_FUNCTION median_doubles
#define NC_MEDIAN_FUNCTION nc_median_doubles
#include "stats-module.inc"

#define GENERIC_TYPE float
#define MEAN_FUNCTION mean_floats
#define MEAN_RESULT_TYPE float
#define STDDEV_FUNCTION stddev_floats
#define STDDEV_RESULT_TYPE float
#define MEDIAN_FUNCTION median_floats
#define NC_MEDIAN_FUNCTION nc_median_floats
#include "stats-module.inc"

#define GENERIC_TYPE int
#define MEAN_FUNCTION mean_ints
#define MEAN_RESULT_TYPE double
#define STDDEV_FUNCTION stddev_ints
#define STDDEV_RESULT_TYPE double
#define MEDIAN_FUNCTION median_ints
#define NC_MEDIAN_FUNCTION nc_median_ints
#include "stats-module.inc"

#define GENERIC_TYPE unsigned int
#define MEAN_FUNCTION mean_uints
#define MEAN_RESULT_TYPE double
#define STDDEV_FUNCTION stddev_uints
#define STDDEV_RESULT_TYPE double
#define MEDIAN_FUNCTION median_uints
#define NC_MEDIAN_FUNCTION nc_median_uints
#include "stats-module.inc"

#if SIZEOF_LONG != SIZEOF_INT
# define GENERIC_TYPE long
# define MEAN_FUNCTION mean_longs
# define MEAN_RESULT_TYPE double
# define STDDEV_FUNCTION stddev_longs
# define STDDEV_RESULT_TYPE double
# define MEDIAN_FUNCTION median_longs
# define NC_MEDIAN_FUNCTION nc_median_longs
# include "stats-module.inc"

# define GENERIC_TYPE unsigned long
# define MEAN_FUNCTION mean_ulongs
# define MEAN_RESULT_TYPE double
# define STDDEV_FUNCTION stddev_ulongs
# define STDDEV_RESULT_TYPE double
# define MEDIAN_FUNCTION median_ulongs
# define NC_MEDIAN_FUNCTION nc_median_ulongs
# include "stats-module.inc"
#else
# define mean_longs mean_ints
# define stddev_longs stddev_ints
# define median_longs median_ints
# define nc_median_longs nc_median_ints
# define mean_ulongs mean_uints
# define stddev_ulongs stddev_uints
# define median_ulongs median_uints
# define nc_median_ulongs nc_median_uints
#endif

#if SIZEOF_SHORT != SIZEOF_INT
# define GENERIC_TYPE short
# define MEAN_FUNCTION mean_shorts
# define MEAN_RESULT_TYPE float
# define STDDEV_FUNCTION stddev_shorts
# define STDDEV_RESULT_TYPE float
# define MEDIAN_FUNCTION median_shorts
# define NC_MEDIAN_FUNCTION nc_median_shorts
# include "stats-module.inc"

# define GENERIC_TYPE unsigned short
# define MEAN_FUNCTION mean_ushorts
# define MEAN_RESULT_TYPE float
# define STDDEV_FUNCTION stddev_ushorts
# define STDDEV_RESULT_TYPE float
# define MEDIAN_FUNCTION median_ushorts
# define NC_MEDIAN_FUNCTION nc_median_ushorts
# include "stats-module.inc"
#else
# define mean_shorts mean_ints
# define stddev_shorts stddev_ints
# define median_shorts median_ints
# define nc_median_shorts nc_median_ints
# define mean_ushorts mean_uints
# define stddev_ushorts stddev_uints
# define median_ushorts median_uints
# define nc_median_ushorts nc_median_uints
#endif

#define GENERIC_TYPE signed char
#define MEAN_FUNCTION mean_chars
#define MEAN_RESULT_TYPE float
#define STDDEV_FUNCTION stddev_chars
#define STDDEV_RESULT_TYPE float
#define MEDIAN_FUNCTION median_chars
#define NC_MEDIAN_FUNCTION nc_median_chars
#include "stats-module.inc"

#define GENERIC_TYPE unsigned char
#define MEAN_FUNCTION mean_uchars
#define MEAN_RESULT_TYPE float
#define STDDEV_FUNCTION stddev_uchars
#define STDDEV_RESULT_TYPE float
#define MEDIAN_FUNCTION median_uchars
#define NC_MEDIAN_FUNCTION nc_median_uchars
#include "stats-module.inc"

static SLarray_Contract_Type Stddev_Functions [] =
{
     {SLANG_CHAR_TYPE, SLANG_CHAR_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) stddev_chars},
     {SLANG_UCHAR_TYPE,SLANG_UCHAR_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) stddev_uchars},
     {SLANG_SHORT_TYPE,SLANG_SHORT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) stddev_shorts},
     {SLANG_USHORT_TYPE, SLANG_USHORT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) stddev_ushorts},
     {SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) stddev_ints},
     {SLANG_UINT_TYPE, SLANG_UINT_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) stddev_uints},
     {SLANG_LONG_TYPE, SLANG_LONG_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) stddev_longs},
     {SLANG_ULONG_TYPE, SLANG_ULONG_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) stddev_ulongs},
     {SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) stddev_floats},
     {SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) stddev_doubles},
     {0, 0, 0, NULL}
};

static void stddev_intrin (void)
{
   if (SLang_Num_Function_Args == 0)
     {
	SLang_verror (SL_Usage_Error, "x = stddev(X [,dim])");
	return;
     }
   (void) SLarray_contract_array (Stddev_Functions);
}

static SLCONST SLarray_Contract_Type Mean_Functions [] =
{
     {SLANG_CHAR_TYPE, SLANG_CHAR_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) mean_chars},
     {SLANG_UCHAR_TYPE, SLANG_UCHAR_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) mean_uchars},
     {SLANG_SHORT_TYPE, SLANG_SHORT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) mean_shorts},
     {SLANG_USHORT_TYPE, SLANG_USHORT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) mean_ushorts},
     {SLANG_UINT_TYPE, SLANG_UINT_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) mean_uints},
     {SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) mean_ints},
     {SLANG_LONG_TYPE, SLANG_LONG_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) mean_longs},
     {SLANG_ULONG_TYPE, SLANG_ULONG_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) mean_ulongs},
     {SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) mean_floats},
     {SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) mean_doubles},
     {0, 0, 0, NULL}
};

static void mean_intrin (void)
{
   if (SLang_Num_Function_Args == 0)
     {
	SLang_verror (SL_Usage_Error, "x = mean(X [,dim])");
	return;
     }
   (void) SLarray_contract_array (Mean_Functions);
}

static SLarray_Contract_Type Median_Functions [] =
{
     {SLANG_CHAR_TYPE, SLANG_CHAR_TYPE, SLANG_CHAR_TYPE, (SLarray_Contract_Fun_Type *) median_chars},
     {SLANG_UCHAR_TYPE,SLANG_UCHAR_TYPE, SLANG_UCHAR_TYPE, (SLarray_Contract_Fun_Type *) median_uchars},
     {SLANG_SHORT_TYPE,SLANG_SHORT_TYPE, SLANG_SHORT_TYPE, (SLarray_Contract_Fun_Type *) median_shorts},
     {SLANG_USHORT_TYPE, SLANG_USHORT_TYPE, SLANG_USHORT_TYPE, (SLarray_Contract_Fun_Type *) median_ushorts},
     {SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, (SLarray_Contract_Fun_Type *) median_ints},
     {SLANG_UINT_TYPE, SLANG_UINT_TYPE, SLANG_UINT_TYPE, (SLarray_Contract_Fun_Type *) median_uints},
     {SLANG_LONG_TYPE, SLANG_LONG_TYPE, SLANG_LONG_TYPE, (SLarray_Contract_Fun_Type *) median_longs},
     {SLANG_ULONG_TYPE, SLANG_ULONG_TYPE, SLANG_ULONG_TYPE, (SLarray_Contract_Fun_Type *) median_ulongs},
     {SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) median_floats},
     {SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) median_doubles},
     {0, 0, 0, NULL}
};

static SLarray_Contract_Type NC_Median_Functions [] =
{
     {SLANG_CHAR_TYPE, SLANG_CHAR_TYPE, SLANG_CHAR_TYPE, (SLarray_Contract_Fun_Type *) nc_median_chars},
     {SLANG_UCHAR_TYPE,SLANG_UCHAR_TYPE, SLANG_UCHAR_TYPE, (SLarray_Contract_Fun_Type *) nc_median_uchars},
     {SLANG_SHORT_TYPE,SLANG_SHORT_TYPE, SLANG_SHORT_TYPE, (SLarray_Contract_Fun_Type *) nc_median_shorts},
     {SLANG_USHORT_TYPE, SLANG_USHORT_TYPE, SLANG_USHORT_TYPE, (SLarray_Contract_Fun_Type *) nc_median_ushorts},
     {SLANG_INT_TYPE, SLANG_INT_TYPE, SLANG_INT_TYPE, (SLarray_Contract_Fun_Type *) nc_median_ints},
     {SLANG_UINT_TYPE, SLANG_UINT_TYPE, SLANG_UINT_TYPE, (SLarray_Contract_Fun_Type *) nc_median_uints},
     {SLANG_LONG_TYPE, SLANG_LONG_TYPE, SLANG_LONG_TYPE, (SLarray_Contract_Fun_Type *) nc_median_longs},
     {SLANG_ULONG_TYPE, SLANG_ULONG_TYPE, SLANG_ULONG_TYPE, (SLarray_Contract_Fun_Type *) nc_median_ulongs},
     {SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, SLANG_FLOAT_TYPE, (SLarray_Contract_Fun_Type *) nc_median_floats},
     {SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, (SLarray_Contract_Fun_Type *) nc_median_doubles},
     {0, 0, 0, NULL}
};

static void median_nc_intrin (void)
{
   if (SLang_Num_Function_Args == 0)
     {
	SLang_verror (SL_Usage_Error, "x = median_nc (X [,dim])");
	return;
     }
   (void) SLarray_contract_array (NC_Median_Functions);
}
static void median_intrin (void)
{
   if (SLang_Num_Function_Args == 0)
     {
	SLang_verror (SL_Usage_Error, "x = median (X [,dim])");
	return;
     }
   (void) SLarray_contract_array (Median_Functions);
}

static void binomial_intrin (void)
{
   unsigned int n, k, i;
   double f;
   SLindex_Type dims;
   SLang_Array_Type *at;
   double *data;

   if (SLang_Num_Function_Args == 2)
     {
	if ((-1 == SLang_pop_uint (&k)) || (-1 == (SLang_pop_uint (&n))))
	  return;

	(void) SLang_push_double (compute_binomial_coeff (n, k));
	return;
     }

   if (-1 == SLang_pop_uint (&n))
     return;

   dims = (SLindex_Type) (n+1);
   if (NULL == (at = SLang_create_array (SLANG_DOUBLE_TYPE, 0, NULL, &dims, 1)))
     return;

   data = (double *)at->data;
   k = n;

   data[0] = 1.0;
   data[k] = 1.0;
   f = 1.0;
   for (i = 1; i <= k; i++)
     {
	f = (f/i)*k;
	k--;
	data[i] = data[k] = f;
     }
   (void) SLang_push_array (at, 1);
}

#if 0
static void erf_intrin (void)
{
   int is_float;
   SLang_Array_Type *at;

   is_float == (SLang_peek_at_stack1 () == SLANG_FLOAT_TYPE);
   if (SLang_peek_at_stack () != SLANG_ARRAY_TYPE)
     {
	double x;
	if (is_float)
	  {
	     float xf;
	     if (-1 == SLang_pop_float (&xf))
	       return;
	     (void) SLang_push_float ((float) erf((double) xf));
	  }
	if (0 == POP_DOUBLE (&x))
	  (void) SLang_push_double (x);
	return;
     }
   if (is_float)
     {
	float *f,
	if (-1 == SLang_pop_array_of_type (&at, SLANG_FLOAT_TYPE))
	  return;
     }
   xxxxxxxxxx
}
#endif

static double normal_cdf_intrin (double *xp)
{
   double x = *xp;
   return 0.5 * (1.0 + erf (x/sqrt(2.0)));
}

static double kendall_tau_intrin (void)
{
   double val, z;
   size_t n;
   SLang_Array_Type *at, *bt;

   if (-1 == SLang_pop_array_of_type (&bt, SLANG_ARRAY_INDEX_TYPE))
     return -1.0;
   n = bt->num_elements;

   if (-1 == SLang_pop_array_of_type (&at, SLANG_ARRAY_INDEX_TYPE))
     {
	SLang_free_array (bt);
	return -1.0;
     }
   if (at->num_elements != n)
     {
	SLang_verror (SL_TYPE_MISMATCH, "kendall_tau: arrays must have the same size");
	val = -1.0;
	goto free_and_return;
     }

   val = _pSLstats_kendall_tau ((SLindex_Type *)at->data, (SLindex_Type *)bt->data, n, &z);

   /* drop */

free_and_return:

   SLang_free_array (at);
   SLang_free_array (bt);
   (void) SLang_push_double (z);
   return val;
}

static SLang_Intrin_Fun_Type Module_Intrinsics [] =
{
   MAKE_INTRINSIC_1("smirnov_cdf", smirnov_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE),
   MAKE_INTRINSIC_2("chisqr_cdf", chisqr_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_INT_TYPE, SLANG_DOUBLE_TYPE),
   MAKE_INTRINSIC_1("_normal_cdf", normal_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE),
   MAKE_INTRINSIC_2("student_t_cdf", student_t_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE),
   MAKE_INTRINSIC_2("_poisson_cdf", poisson_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_INT_TYPE),
   MAKE_INTRINSIC_3("kim_jennrich_cdf", kim_jennrich_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_UINT_TYPE, SLANG_UINT_TYPE, SLANG_UINT_TYPE),
   MAKE_INTRINSIC_3("mann_whitney_cdf", mann_whitney_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_UINT_TYPE, SLANG_UINT_TYPE, SLANG_DOUBLE_TYPE),
   MAKE_INTRINSIC_3("f_cdf", f_cdf_intrin, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE, SLANG_DOUBLE_TYPE),
   MAKE_INTRINSIC_0("mean", mean_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("stddev", stddev_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("median", median_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("median_nc", median_nc_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("binomial", binomial_intrin, SLANG_VOID_TYPE),
   MAKE_INTRINSIC_0("_kendall_tau", kendall_tau_intrin, SLANG_DOUBLE_TYPE),
   SLANG_END_INTRIN_FUN_TABLE
};

static SLFUTURE_CONST char *Module_Version_String = MODULE_VERSION_STRING;
static SLang_Intrin_Var_Type Module_Variables [] =
{
   MAKE_VARIABLE("_stats_module_version_string", &Module_Version_String, SLANG_STRING_TYPE, 1),
   SLANG_END_INTRIN_VAR_TABLE
};

static SLang_IConstant_Type Module_IConstants [] =
{
   MAKE_ICONSTANT("_stats_module_version", MODULE_VERSION_NUMBER),
   SLANG_END_ICONST_TABLE
};

static SLang_DConstant_Type Module_DConstants [] =
{
   SLANG_END_DCONST_TABLE
};

int init_stats_module_ns (char *ns_name)
{
   SLang_NameSpace_Type *ns = SLns_create_namespace (ns_name);
   if (ns == NULL)
     return -1;

   if (
       (-1 == SLns_add_intrin_var_table (ns, Module_Variables, NULL))
       || (-1 == SLns_add_intrin_fun_table (ns, Module_Intrinsics, NULL))
       || (-1 == SLns_add_iconstant_table (ns, Module_IConstants, NULL))
       || (-1 == SLns_add_dconstant_table (ns, Module_DConstants, NULL))
       )
     return -1;

   return 0;
}

/* This function is optional */
void deinit_stats_module (void)
{
}
