#include "clock.h"
#include <iostream>
using namespace std;

Clock::Clock():
  Spontaneous()
{
  this->T=1.0;
}

Clock::Clock(double T):
  Spontaneous()
{
  this->T=T;
}

void Clock::set_T(double T)
{
  this->T=T;
}

double Clock::advance(void)
{
  return T;
}
