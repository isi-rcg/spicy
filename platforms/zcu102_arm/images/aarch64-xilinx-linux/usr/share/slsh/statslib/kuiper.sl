% Kuiper Tests
%
% This is asymptotically correct
private define kuiper_test_prob (n, d)
{
   variable sn = sqrt(n);
   variable factor = sn + 0.155 + 0.24/sn;
   d = factor * d;
   if (d < 0.4)
     return 1.0;
   if (d > 20.0)
     return 0.0;

   variable x = ([1:100]*d)^2;
   variable p = 2.0*sum ((4.0*x - 1.0) * exp (-2.0*x));
   if (p < 0.0)
     p = 0.0;
   if (p > 1.0)
     p = 1.0;
   return p;
}

define kuiper_test ()
{
   variable d_ref = NULL;
   if (_NARGS == 2)
     d_ref = ();
   else if (_NARGS != 1)
     usage ("p = kuiper_test (CDF [,&D]);  %% 1-sample Kuiper test\n",
	    + " Here CDF are the expected CDFs at the corresponding random points.");
   variable cdf = ();

   cdf = __tmp(cdf)[array_sort(cdf)];
   variable n = length (cdf);
   variable nn = 1.0*n;
   variable dplus = max ([1:n]/nn - cdf);
   variable dminus = max (cdf-[0:n-1]/nn);
   variable d = dplus + dminus;
   if (d_ref != NULL)
     @d_ref = d;

   return kuiper_test_prob (n, d);
}

define kuiper_test2 ()
{
   variable d_ref = NULL;
   if (_NARGS == 3)
     d_ref = ();
   else if (_NARGS != 2)
     usage ("p = %s(X1, X2 [,&D]); %% Two-sample Kuiper test", _function_name ());

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
   variable d = dplus - dminus;

   if (d_ref != NULL)
     @d_ref = d;

   return kuiper_test_prob (double(m)*double(n)/double(mn), d);
}
