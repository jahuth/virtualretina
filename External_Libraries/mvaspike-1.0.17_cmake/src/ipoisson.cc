#include "ipoisson.h"
#include <iostream>
using namespace std;
#include "random.h"

IPoisson::IPoisson(double rmax):
  SpikeTrain()
{
  this->rmax=rmax;
  tn=0.0;
  //tn=advance();
}

double IPoisson::advance(void)
{
  do {
    tn+=rnd_poisson(1/rmax);
  } while (r_est(tn)/rmax<rnd(1.0));

  return tn;
}

void IPoisson::record_state(void)
{
    rec->add(rmax);
}
