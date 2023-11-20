#ifndef STDP_H
#define STDP_H

#include "warray.h"
#include "baserecorder.h"

// an array of dynamic weights.
class STDP : public Warray
{
 protected:
  double lastf;
  double * lastr;
  int wlen;
  virtual double update_weight(double w,double post_pre);
 public:
  STDP(void);
  virtual ~STDP(void);
  void delta_int(double t);
  void delta_ext(double t,int port);
  void init(int len);
  void record_state(BaseRecorder *rec); // dynamic -> we need to record
};

#endif
