#ifndef IPOISSON_H
#define IPOISSON_H
#include <iostream>
#include "spiketrain.h"

class IPoisson : public SpikeTrain
{
 protected:
  double rmax;
  double advance(void); // global time !
  virtual double r_est(double t)=0;
  void record_state(void);
 public:
  IPoisson(double rmax);
};

#endif
