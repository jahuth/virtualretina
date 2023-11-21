#ifndef EXTSPIKETRAIN_H
#define EXTSPIKETRAIN_H
#include <iostream>
#include <vector>
#include "spiketrain.h"

class ExtSpikeTrain : public SpikeTrain
{
 protected:
  std::vector<double> spikes;
  double advance();  
  void record_state(void);

 public:
  ExtSpikeTrain();
  void init(void);
  void add_spike(double t);
};

#endif
