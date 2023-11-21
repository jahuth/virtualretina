#ifndef WSTATIC_H
#define WSTATIC_H
#include "baserecorder.h"

class Wstatic {
 protected:
  double w;
 public:
  Wstatic();
  virtual ~Wstatic(){}
  virtual void set_w(double w);
  void init(int nb_weight);
  double get_w(int port);
  void delta_int(double t) {};
  void delta_ext(double t,int port) {};
  void record_state(BaseRecorder *) {};
};

#endif
