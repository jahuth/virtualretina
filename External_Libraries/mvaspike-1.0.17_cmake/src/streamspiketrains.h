#ifndef STREAMSPIKETRAINS_H
#define STREAMSPIKETRAINS_H
#include "spiketrains.h"
#include <iostream>
#include <vector>
class StreamSpikeTrains : public SpikeTrains 
{
 protected:
  std::vector<OutSpike> spikes;
  OutSpike advance(void);
 public:
  StreamSpikeTrains(void);
  StreamSpikeTrains(std::istream &is);
  void init(void);
};

#endif
