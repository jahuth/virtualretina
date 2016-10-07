#ifndef LOGPOLARSCHEME_H
#define LOGPOLARSCHEME_H

#include "radialscheme.h"

// This is a specialized radial scheme (the only one I'm using for the moment), 
// inheriting from the base filtering scheme class RadialScheme.

class LogPolarScheme : public RadialScheme
{

public:

  double R0, K;
  LogPolarScheme(double r0=1, double k=-1);
  ~LogPolarScheme();

  double density(double r)
  {
    return (r>R0)?  1/(1+K*(r-R0)) : 1 ;
  }

  // In this case, the explicit formulas can be calculated from the expression 
  // of function density() (see formulas in logpolarscheme.cc)
  double integratedDensity(double r);
  double findRadius(double integrated_density);

  LogPolarScheme& set_R0(double r0);
  LogPolarScheme& set_K(double rk);
};

#endif
