#include "poisson.h"
#include "random.h"

#include <iostream>
using namespace std;

Poisson::Poisson(void) :
  Spontaneous(),T(1.0)
{
}

Poisson::Poisson(double T):
  Spontaneous()
{
  this->T=T;
}

double Poisson::advance(void)
{
  return rnd_poisson(T);
}

void Poisson::record_state(void)
{
}

void Poisson::set_T(double T)
{
  this->T=T;
}
