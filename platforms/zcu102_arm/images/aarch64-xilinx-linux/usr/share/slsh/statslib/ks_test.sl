% Kolmogorov–Smirnov test

% Asymptotically correct.  Stephens 1974
private define ks_test_prob (n, d)
{
   variable sn = sqrt(n);
   variable factor = sn + 0.12 + 0.11/sn;
   return 1-smirnov_cdf (sn * d);
}

define ks_test ()
{
   variable d_ref = NULL;
   if (_NARGS == 2)
     d_ref = ();
   else if (_NARGS != 1)
     usage ("p = ks_test (CDF [,&D]);  %% 1-sample KS test\n",
	    + " Here CDF are the expected CDFs at the corresponding random points.");
   variable cdf = ();

   cdf = __tmp(cdf)[array_sort(cdf)];
   variable n = length (cdf);
   variable nn = 1.0*n;
   variable dplus = max ([1:n]/nn - cdf);
   variable dminus = max (cdf-[0:n-1]/nn);
   variable d = max ([dplus, dminus]);
   if (d_ref != NULL)
     @d_ref = d;

   return ks_test_prob (n, d);
}

% We want ks_test2_prob to return P(D_mn >= d), where d is the observed value.
% It is known that d can only take on values c/mn where c, m, and n are integers.
% So set d=c/mn.
% kim_jennrich_cdf returns P(D_mn <= c/mn)
% But we want P(D_mn >= c/mn) = 1-P(D_mn < c/mn)
%   P(D_mn <= (c-1)/mn) <= P(D_mn < c/mn) <= P(D_mn <= c/mn)
%   P(D_mn <= (c-1)/mn) <= P(D_mn < c/mn) <= P(D_mn < c/mn) + P(D_mn==c/mn)
%   P(D_mn <= (c-1)/mn) <= P(D_mn < c/mn) + P(D_mn==c/mn)
%
% Since D_mn can only take on values c/mn, it follows that
%   P(D_mn < c/mn) = P(D_mn <= (c-1)/mn)
%
private define ks_test2_prob ()
{
   if (_NARGS != 3)
     usage ("p = %s(m, n, d); %% P(D_mn >= d)", _function_name ());
   variable d, m, n; (m, n, d) = ();

   % See the above note for why 1 is subtracted for the first argument of
   % kim_jennrich.
   variable fm = double (m);
   if (fm * n <= 10000.0)
     return 1.0 - kim_jennrich_cdf (m, n, int (d*m*n + 0.5) - 1);

   % Use asymptotic forms.
   return ks_test_prob ((fm*n)/(fm+n), d);
}

define ks_test2 ()
{
   variable d_ref = NULL;
   if (_NARGS == 3)
     d_ref = ();
   else if (_NARGS != 2)
     usage ("p = %s(X1, X2 [,&D]); %% Two-sample KS test", _function_name ());

   variable xm, xn; (xm, xn) = ();
   variable x = [xn, xm];
   variable n = length (xn);
   variable m = length (xm);
   variable mn = m + n;
   variable c = Int_Type[mn];
   c[[0:n-1]] = 1;

   variable i = array_sort (x);
   x = x[i];
   c = c[i]; c = cumsum (__tmp(c));
   variable dmn = (c/n - [1:mn]/(mn*1.0));
   variable factor = mn/(m*1.0);
   variable dplus = factor * max(dmn);
   variable dminus = factor * min(dmn);
   variable d = max([dplus, -dminus]);

   if (d_ref != NULL)
     @d_ref = d;

   return ks_test2_prob (m, n, d);
}

