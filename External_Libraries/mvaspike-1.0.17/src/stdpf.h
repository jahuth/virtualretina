#ifndef STDPF_H
#define STDPF_H
#include "stdp2.h"

// variant of STDP : use a callback function to implement update_weight
// (dedicated to people used to program in C :-))

// USING SLOW STDP2 !

class STDPF : public STDP2
{
 protected:
  double(*ext_update_weight)(double,double);
  double update_weight(double w,double post_pre);
 public:
  STDPF(double(*f)(double,double));
  STDPF(void);
  ~STDPF(void);
  void set_f(double(*f)(double,double));
};

#endif
