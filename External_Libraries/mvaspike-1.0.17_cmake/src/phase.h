#ifndef PHASE_H
#define PHASE_H
#include <iostream>
#include "neuron.h"
#include <vector>
#include "warray.h"
class Phase : public Neuron, public Warray
{
  // basic phase model
  // Parameter and state variables :
  //  Phi : phase
  //  T : period
  //  tl : time of last event

 protected:
  double Phi;
  double T;
  double tl;  
  void record_state(void);

 public:
  Phase(void);
  ~Phase(void);
  double ta(void);
  int delta_int(double t);
  void delta_ext(double t,int port);

  void set_Phi(double Phi);
  void set_T(double T);

  double get_T(void);
  double get_Phi(void);
  void init(void);
};

#endif
