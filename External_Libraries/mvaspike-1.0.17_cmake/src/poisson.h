#ifndef POISSON_H
#define POISSON_H
#include <iostream>
#include "spontaneous.h"

class Poisson : public Spontaneous
{
 protected:
  double T;
  double advance(void);  
  void record_state(void);
 public:
  Poisson(void);
  Poisson(double T);
  void set_T(double T);
};

#endif
