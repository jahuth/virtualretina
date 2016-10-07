#include "logpolarscheme.h"

LogPolarScheme::LogPolarScheme(double r0, double k) : RadialScheme()
{
  set_R0(r0);
  if(k==-1)  K=1.0/r0;
  else  set_K(k);
}

LogPolarScheme::~LogPolarScheme(){}

double LogPolarScheme::integratedDensity(double r)
{
  return (r>R0)? R0 + log(1+K*(r-R0))/K : r;
}

double LogPolarScheme::findRadius(double integrated_density)
{
  return  (integrated_density>R0)?  R0+(exp(K*(integrated_density-R0))-1)/K : integrated_density ;
}



LogPolarScheme& LogPolarScheme::set_R0(double r0)
{
  if(r0<=0)
  {
    cerr<<"ERROR in LogPolarScheme: R0 must be strictly positive."<<endl;
    exit(0);
  }
  R0=r0;
  return (*this);
}

LogPolarScheme& LogPolarScheme::set_K(double k)
{
  if(k<0)
  {
    cerr<<"ERROR in LogPolarScheme: K must be positive. Nota: K=0 is useless, meaning a uniform sampling scheme."<<endl;
    exit(0);
  }
  K=k;
  return (*this);
}
