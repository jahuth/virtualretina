#include "spiketrain.h"
#include <iostream>
using namespace std;


SpikeTrain::SpikeTrain():
  Sensor()
{
  tn=-1;
}

double SpikeTrain::ta(void)
{
  return tn;
}

int SpikeTrain::delta_int(double t)
{
    Sensor::delta_int(t);
    tn=advance();
    return 0;
}

