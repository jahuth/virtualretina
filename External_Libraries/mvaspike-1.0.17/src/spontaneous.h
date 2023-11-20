#ifndef SPONTANEOUS_H
#define SPONTANEOUS_H
#include <iostream>
#include "sensor.h"

// A sensor with a rythmic activity : its ta() function
//   depends on the last firing time (not global)
class Spontaneous : public Sensor
{
 protected:
  double tn;
  virtual double advance(void)=0;
 public:
  Spontaneous();
  double ta(void);
  int delta_int(double t);
  void set_t0(double t);    // t0 is an initial virtual last firing time
  void init(void);
};

#endif
