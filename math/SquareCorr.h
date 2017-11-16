#ifndef SQUARECORR_H
#define SQUARECORR_H

#include "base/SVector.h"


class SquareCorrelation
{
 public:
  SquareCorrelation() {}
  
  double Compute(const svec<double> & a, const svec<double> & b) const; 

};




#endif

