
#include "math/SquareCorr.h"
#include <math.h>

double Sqr(double x) {
  return x*x;
}

double SquareCorrelation::Compute(const svec<double> & a, const svec<double> & b) const 
{
  double g;
  int i;

  svec<double> x, y;
  x.resize(a.isize(), 0);
  y.resize(a.isize(), 0);

  double sum_x = 0.;
  double sum_y = 0.;
  double n = 0.;
  for (i=0; i<a.isize(); i++) {
    if (a[i] < 0. || b[i] < 0.)
      continue;
    n += 1.;
    sum_x += a[i];
    sum_y += b[i];
  }
  sum_x /= (double)a.isize();
  sum_y /= (double)a.isize();

  for (i=0; i<a.isize(); i++) {
    if (a[i] < 0. || b[i] < 0.)
      continue;
    x[i] = a[i]/sum_x;
    y[i] = b[i]/sum_y;
  }

  sum_x = 0.;
  sum_y = 0.;
  for (i=0; i<x.isize(); i++) {
    if (a[i] < 0. || b[i] < 0.)
      continue;
    sum_x += x[i];
    sum_y += y[i];
  }
  g = sum_y / sum_x;

  double d = 0.;
  for (i=0; i<x.isize(); i++) {
    if (a[i] < 0. || b[i] < 0.)
      continue;
    d += Sqr(y[i] - g*x[i]) + Sqr(x[i] - 1/g*y[i]);
  }
  return sqrt(d / n);
}
