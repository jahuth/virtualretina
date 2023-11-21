#ifndef STDPLIF_H
#define STDPLIF_H

#include "neuron.h"
#include "stdpf.h"
#include "internal_lif.h"

class StdpLif :  
  public Internal_Lif,
  public Neuron,
  public STDPF
{
 public:
  StdpLif(double(*f)(double,double));
  StdpLif();
  ~StdpLif(void);
  double ta(void);
  int delta_int(double t);
  void delta_ext(double t,int port);
  void init();
  void record_state(void);
  void set_learning_function(double(*f)(double,double));
};

#endif
