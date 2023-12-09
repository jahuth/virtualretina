#ifndef CLOCK_H
#define CLOCK_H
#include <iostream>
#include "spontaneous.h"

class Clock : public Spontaneous
{
 protected:
  double T;  
  double advance();
 public:
  Clock(double T);
  Clock();
  void set_T(double T);
};

#endif
