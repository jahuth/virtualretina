#include "spontaneous.h"
#include <iostream>
using namespace std;


Spontaneous::Spontaneous():
  Sensor()
{
  tn=0;
}

double Spontaneous::ta(void)
{
  return tn;
}

int Spontaneous::delta_int(double t)
{
  Sensor::delta_int(t);
  tn+=advance();
  return 0;
}

void Spontaneous::set_t0(double t)
{
  // as if the last firing was at -t (in the past)
  tn=-t;
}

void Spontaneous::init()
{
  Sensor::init();
  while (tn<=0)
    tn+=advance();
}
