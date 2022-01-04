% Anderson-Darling Tests
% 
% The k-sample test was implemented from the equations found in
% Scholz F.W. and Stephens M.A., "K-Sample Anderson-Darling Tests",
% Journal of the American Statistical Association, Vol 82, 399 (1987)
%
private define get_unique_and_tied (a)
{
   variable n = length (a);
   variable i = array_sort (a);
   variable z = a[__tmp(i)];

   if (z[0] == z[-1])
     return [z[0]], [n];

   variable j, k;
   % Algorithm examples:
   % z =           [1, 2, 2, 3, 4, 4, 4];      [1,2]
   % shift(z,-1) = [4, 1, 2, 2, 3, 4, 4];      [2,1]
   % z!=sh(z)    = [1, 1, 0, 1, 1, 0, 0];      [1,1]
   % j=where(&k) = [0,1,3,4];                  [0,1]
   % k           = [2,5,6];                    []
   % k-1         = [1,4,5];                    []
   j = where (shift (z,-1) != z, &k);

   variable multiplicity = Int_Type[n] + 1;
   variable nk = length(k);
   i = 0;
   while (i < nk)
     {
	variable count = 0;
	variable k0 = k[i], k1 = k0;
	while ((i < nk) && (k[i] == k1))
	  {
	     k1++;
	     i++;
	  }
	multiplicity[k0-1] += k1-k0;
     }
   return z[j], multiplicity[j];
}

define ad_ktest_pval (t, m)
{
   variable alphas = [0.25, 0.1, 0.05, 0.025, 0.01];
   % S&S suggests the parametrization
   %   t_m(alpha) = b0(alpha) + b1(alpha)/sqrt(m) + b2(alpha)/m
   variable b0_alpha = [0.675, 1.281, 1.645, 1.960, 2.326];
   variable b1_alpha = [-0.245, 0.250, 0.678, 1.149, 1.822];
   variable b2_alpha = [-0.105, -0.305, -0.362, -0.391, -0.396];

   % The algorithm:
   %  1.  Use bk_alpha to compute tm_alpha for given m.
   %  2.  Use linear interpolation of log(alpha/(1-alpha) vs tm_alpha.
   %  3.  ==> exp(v) = alpha/(1-alpha)
   %      ==> alpha = exp(v)/(1+exp(v)), where v is interpolated value.

   variable i, n = length (alphas);
   variable logodd = log (alphas/(1-alphas));
   variable tm_alpha = b0_alpha + b1_alpha/sqrt(m) + b2_alpha/m;
   i = wherelast (t >= tm_alpha);
   if (i == NULL)
     i = 0;
   else if (i == n - 1)
     i = n - 2;

   % v = p0*(1-s) + s*p1 ==> v = p0 + s*(p1-p0)
   % v0 = p0 + (t-t0)/(t1-t0)*(p1-p0)
   variable v0 = logodd[i], v1 = logodd[i+1], t0 = tm_alpha[i], t1 = tm_alpha[i+1];
   variable v = v0 + (v1-v0)*((t-t0)/(t1-t0));

   variable alpha = exp(v)/(1+exp(v));
   if (alpha < 0) alpha = 0.0;
   else if (alpha > 1.0) alpha = 1.0;
   return alpha;
}

define ad_ktest ()
{
   variable tref = NULL;
   variable arg, nargs = _NARGS;
   if (nargs > 1)
     {
	arg = ();
	if (_typeof (arg) == Ref_Type)
	  {
	     tref = arg;
	     nargs--;
	  }
	else arg;		       %  leave it on stack
     }

   if (nargs == 0)
     usage ("\
pval = ad_ktest ({X1,X2,...Xn} [,&statistic]; qualifiers);\n\
Qualifiers:\n\
  pval2=&pval2    P-value corresponding to continuous case (no ties)\n\
  stat2=&stat2    Statistic for the continuous case\n\
"
	   );

   variable datasets;
   if (nargs == 1)
     datasets = ();
   else
     datasets = __pop_list (nargs);

   if ((_typeof (datasets) != Array_Type)
       && (typeof (datasets) != List_Type))
     throw InvalidParmError, "Expecting a list of arrays or an array of arrays";

   variable i, j, k;
   variable z_i, zstar, n;

   k = length (datasets);
   if (k < 2)
     throw InvalidParmError, "ad_ktest requires at least 2 datasets";

   zstar = datasets[0];

   _for i (1, k-1, 1)
     zstar = [zstar, datasets[i]];

   n = length (zstar);

   variable multiplicity;
   (zstar, multiplicity) = get_unique_and_tied (zstar);

   variable cap_l = length(zstar);

   variable s = 0.0, s_a = 0.0;
   variable mj = Double_Type[k];
   variable cap_Bj = 0.0, cap_Bj_a = 0.0;

   variable kmult = Int_Type[cap_l, k];
   variable zstar_j;
   cap_l--;
#ifexists wherefirst_ge
   _for i (0, k-1, 1)
     {
	z_i = datasets[i];
	z_i = z_i[array_sort (z_i)];
	% exploit the fact that both z_i and zstar are sorted.
	variable i0 = 0;
	_for j (0, cap_l, 1)
	  {
	     variable i1;
	     i1 = wherefirst_ge (z_i, zstar[j], i0);
	     if (i1 == NULL)
	       continue;
	     % This point is reached about a quarter of the time.
	     i0 = i1;
	     i1 = wherefirst_ne (z_i, zstar[j], i0);
	     if (i1 == NULL)
	       {
		  % end of array
		  if (zstar[j] == z_i[i0])
		    kmult[j,i] = length(z_i)-i0;
		  break;
	       }

	     kmult[j,i] = i1-i0;
	     i0 = i1;
	  }
     }
#endif
   _for j (0, cap_l, 1)
     {
	zstar_j = zstar[j];
	variable l_j = multiplicity[j];
	cap_Bj += l_j;
	cap_Bj_a += 0.5*l_j;
	variable ds = 0.0, ds_a = 0.0;
	_for i (0, k-1, 1)
	  {
	     z_i = datasets[i];
	     variable n_i = length (z_i);
#ifexists wherefirst_ne
	     variable dmij = 0.5*kmult[j,i];
#else
	     variable dmij = 0.5*length (where (z_i == zstar_j));
#endif
	     variable mij = mj[i] + dmij;
	     variable top = (n*mij - n_i*cap_Bj_a);
	     ds_a += top*top/n_i;

	     mij += dmij;
	     top = (n*mij - n_i*cap_Bj);
	     ds += top*top/n_i;
	     mj[i] = mij;
	  }
	s_a += l_j*ds_a/(cap_Bj_a*(n-cap_Bj_a) - 0.25*l_j*n)/n;
	if (j != cap_l)
	  s += l_j*ds/(cap_Bj * (n-cap_Bj))/n;
	cap_Bj_a = cap_Bj;
     }
   s_a *= (n-1.0)/n;

   variable h = sum (1.0/[1:n-1]);
   variable g = sum (cumsum (1.0/([n-1:2:-1]))/[2:n-1]);
   variable cap_h = 0.0;

   _for i (0, k-1, 1)
     cap_h += 1.0/length(datasets[i]);

   variable
     k2 = k*k, hk = h*k, g4m6 = 4*g-6,
     a = g4m6*(k-1)+(10-6*g)*cap_h,
     b = (2*g-4)*k2 + 8*hk + (2*g-14*h-4)*cap_h - 8*h + g4m6,
     c = (6*h+2*g-2)*k2 + (4*h-g4m6)*k + (2*h-6)*cap_h + 4*h,
     d = (2*h+6)*k2 - 4*hk;

   variable sig = d + n*(c + n*(b + n*a));
   sig = sqrt(sig/(n-1)/(n-2)/(n-3));
   variable t = (s - (k-1))/sig;
   variable t_a = (s_a - (k-1))/sig;
   %vmessage ("s = %g, s_a = %g", s, s_a);

   variable pval = ad_ktest_pval (t_a, k-1);
   if (tref != NULL) @tref = t_a;

   variable pval2_ref = qualifier ("pval2");
   variable stat2_ref = qualifier ("stat2");

   if (typeof (pval2_ref) == Ref_Type) @pval2_ref = ad_ktest_pval (t, k-1);
   if (typeof (stat2_ref) == Ref_Type) @stat2_ref = t;

   return pval;
}

% This function was derived from
%  Marsaglia and Marsaglia, Evaluating the Anderson-Darling
%  Distribution, Journal of Statistical Software, Vol. 9, Issue 2, Feb
%  2004.
define anderson_darling_cdf ()
{
   if (_NARGS != 2)
     {
	usage ("\
cdf = anderson_darling_cdf (A2, nsamp)\n\
%% Computes the Anderson-Darling CDF at the points A2 for sample-sizes nsamp.\n\
"
	      );
     }
   variable z, ndata; (z, ndata) = ();
   variable nz = length (z);

   if (typeof (ndata) != Array_Type)
     ndata = ndata + Int_Type[nz];

   if (length (ndata) != nz)
     throw InvalidParmError, "the length of A2 and nsamp do not match";

   variable x = Double_Type[nz];

   variable i, j, a, zz;

   i = where (0.0 < z < 2.0, &j);
   if (length (i))
     {
	zz = z[i];
	a = [2.00012, 0.247105, -0.0649821, 0.0347962, -0.0116720, 0.00168691];
	x[i] = exp(-1.2337141/zz)*polynom(a, zz)/sqrt(zz);
     }
   if (length (j))
     {
	zz = z[j];
	a = [1.0776, -2.30695, 0.43424, -0.082433, 0.008056, -0.0003146];
	x[j] = exp(-exp(polynom (a, zz)));
     }

   % Now compute the correction for finite n (sec 3 of Marsaglia's paper)
   variable ndatainv = 1.0/ndata;
   variable c = 0.01265 + 0.1757*ndatainv;
   variable dx = Double_Type[nz];

   i = where (x < c);
   if (length (i))
     {
	zz = x[i]/c[i];
	dx[i] = ((0.0037*ndatainv + 0.00078)*ndatainv + 0.00006)*ndatainv
	  * sqrt(zz)*(1.0-zz)*(49.0*zz-102.0);
     }

   i = where (c <= x < 0.8);
   if (length (i))
     {
	variable ci = c[i];
	zz = x[i];
	a = [-.00022633, 6.54034, -14.6538, 14.458, -8.259, 1.91864];
	dx[i] = ndatainv*(0.04213 + ndatainv*0.01365)
	  * polynom (a, (zz-ci)/(0.8-ci));
     }

   i = where (x >= 0.8);
   if (length (i))
     {
	a = [-130.2137, 745.2337, -1705.091, 1950.646, -1116.360, 255.7844];
	dx[i] = polynom (a, x[i])*ndatainv;
     }

   x = __tmp(x) + dx;
   ifnot (Array_Type == typeof(z))
     x = x[0];

   return x;
}

define ad_test ()
{
   variable s_ref = NULL;
   if (_NARGS == 2)
     s_ref = ();
   else if (_NARGS != 1)
     usage ("\
p = ad_test (X [,&Asquared]);\n\ %% 1-sample Anderson-Darling test\n\
Qualifiers:\n\
  ;cdf   %% The X values are the CDFs of the underlying distribution\n\
         %%   and 0 <= X <= 1\n\
"
	   );

   variable cdf = ();
   variable is_cdf = qualifier_exists ("cdf");

   variable n = length (cdf);
   variable factor = 1.0;

   ifnot (is_cdf)
     {
	variable mu = qualifier ("mean");
	variable sd = qualifier ("stddev");

	if (sd == NULL)
	  {
	     if (mu == NULL)
	       {
		  factor = (1.0 + (0.75 + 2.25/n)/n);
		  sd = stddev (cdf);
	       }
	     else sd = sample_stddev (cdf);
	  }
	if (mu == NULL) mu = mean (cdf);
	%vmessage ("mean=%g, stddev=%g, factor=%g", mu, sd, factor);
	cdf = normal_cdf (cdf, mu, sd);
     }

   cdf = __tmp(cdf)[array_sort(cdf)];

   variable ii = [1:2*n:2];
   variable a2 = -n - (sum(ii*log(cdf) + (2*n-ii)*log(1.0-cdf)))/n;

   a2 = factor * a2;

   if (s_ref != NULL)
     @s_ref = a2;

   if (is_cdf)
     {
	return 1.0 - anderson_darling_cdf (a2, n);
     }

   % Augostino & Stephens, 1986
   if (a2 >= 0.6)
     return exp(1.2937 + (-5.709 + 0.0186*a2)*a2);
   if (a2 >= 0.34)
     return exp(0.9177 + (-4.279 - 1.38*a2)*a2);
   if (a2 >= 0.2)
     return 1.0 - exp(-8.318 + (42.796 - 59.938*a2)*a2);
   return 1.0 - exp(-13.436 + (101.14 - 223.73*a2)*a2);
}

