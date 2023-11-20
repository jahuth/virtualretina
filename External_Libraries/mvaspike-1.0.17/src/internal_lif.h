#ifndef INTERNAL_LIF_H
#define INTERNAL_LIF_H

#include "baserecorder.h"

class Internal_Lif
{
 protected:
  double V;
  double Vreset;
  double Tau;
  double I;
  double tl;
  double thres;
  double Trefra;
  double tf;
  void record_state(BaseRecorder *);

 public:
  Internal_Lif(void);
  ~Internal_Lif(void);
  double ta(void);
  void reset(double t);
  void dirac(double t,double w);
  void step_current(double t,double wi);

  void set_I(double I);
  void set_I(double I,double t);

  void set_V(double V);
  void set_V(double V,double t);

  void set_Vreset(double V);

  void set_Tau(double Tau);
  void set_Tau(double Tau,double t);

  void set_threshold(double t);
  void set_Trefra(double t);

  void advance(double t);

  double get_I(void);
  double get_V(void);
  double get_Tau(void);
  
  double get_tl(void);
  
  void init();
};

#endif
