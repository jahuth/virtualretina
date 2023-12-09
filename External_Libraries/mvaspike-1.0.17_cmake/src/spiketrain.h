#ifndef SPIKETRAIN_H
#define SPIKETRAIN_H
#include <iostream>
#include "sensor.h"

// in contrast with spontaneous, which depicts a periodic or quasi periodic
// behaviour (within a local time base), a SpikeTrain models any spike train with a global time base
class SpikeTrain : public Sensor
{
 protected:
  double tn;
  virtual double advance(void)=0; // global time !
 public:
  SpikeTrain();
  double ta(void);
  int delta_int(double t);
};

#endif
