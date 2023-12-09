#ifndef STDP2_H
#define STDP2_H
#include <vector>
#include "warray.h"
#include "baserecorder.h"

// an array of dynamic weights.
// SLOW but generic/extendable version

class STDP2 : public Warray
{
 protected:
  std::vector<double> lastf;
  std::vector<double> * lastr;
  int wlen;
  virtual double update_weight(double w,double post_pre);
 public:
  STDP2(void);
  virtual ~STDP2(void);
  void delta_int(double t);
  void delta_ext(double t,int port);
  void init(int len);
  void record_state(BaseRecorder *rec); // dynamic -> we need to record
};

#endif
