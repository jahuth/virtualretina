#include "phase.h"
#include <math.h>

Phase::Phase(void):
  Neuron(),Warray()
{
  Phi=0.0;
  T=1.0;
  tl=0;
}

Phase::~Phase(void)
{
}

double Phase::ta(void)
{
  if (Phi>=1.0) 
    return tl;
  else 
    return tl+T*(1-Phi);
}

int Phase::delta_int(double t)
{
  Phi=0.0;
  tl=t;
  return 0;
}

void Phase::delta_ext(double t,int port)
{
  Phi=Phi+(t-tl)/T+w[port];
  tl=t;
}

void Phase::set_Phi(double x)
{
  Phi=x;
}

void Phase::set_T(double T)
{
  this->T=T;
}

double Phase::get_Phi(void)
{
  // TODO : should return an updated Phi ?
  // or not ?
  return Phi;
}

double Phase::get_T(void)
{
  return T;
}

void Phase::init(void)
{
  Warray::init(nb_inport);
}

void Phase::record_state(void)
{
  //TODO ...
}
