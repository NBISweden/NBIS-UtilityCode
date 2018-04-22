#ifndef FISHERSEXACT_H
#define FISHERSEXACT_H

#include <math.h>
#include "math/kfunc.h"

double FacFrac(int b, int a) 
{
  bool bInv = false;
  if (a > b) {
    int tmp = b;
    b = a;
    a = tmp;
    bInv = true;
    cout << "ERROR: " << a << " " << b << endl;
  }
  int i;
  double res = 1.;
  for (i=a+1; i<=b; i++) {
    res *= (double)i;
  }
  //cout << a << " " << b << "! = " << res << endl;
  if (bInv)
    return 1/res;
  else 
    return res;

  
}

double Fac(int d) {
  int i;
  double res = 1.;
  for (i=2; i<=d; i++) {
    res *= (double)i;
  }
  //cout << d << "! = " << res << endl; 
  return res;
}

double BinCoeff(int n, int k)
{
  double r = FacFrac(n, k)/Fac(n-k);
  return r;
}

double FishersExact(int a, int b, int c, int d)
{
  double fisher_left_p, fisher_right_p, fisher_twosided_p;
  kt_fisher_exact(a, b, c, d,
		  &fisher_left_p, &fisher_right_p, &fisher_twosided_p);

  return fisher_twosided_p/2;
   
  /*
  double r = 1/FacFrac(c+d, a+b+c+d);
  r *= FacFrac(c, a+c);
  r *= FacFrac(d, b+d);

  r *=Fac(a+b);
  r /= Fac(a);
  r /= Fac(b);
  */

  //return r;
}



#endif 
