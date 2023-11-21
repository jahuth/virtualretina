#include "streamspiketrains.h"
#include<math.h>
#include <iostream>
#include <algorithm>
using namespace std;
StreamSpikeTrains::StreamSpikeTrains(void):
  SpikeTrains()
{}
StreamSpikeTrains::StreamSpikeTrains(std::istream &is)
  : SpikeTrains()
{
  int maxnb=-1;
  while (!is.eof())
    {
      int nb;
      double t;
      is >> nb;
      if (!is.eof()) // that way we can check for eof even with garbage CRs
	{
	  is >> t;
	  spikes.push_back(OutSpike(t,nb));
	  if (nb>maxnb)
	    maxnb=nb;
	}
    }
  set_nb_outport(maxnb+1);
}

SpikeTrains::OutSpike StreamSpikeTrains::advance(void)
{
  if (spikes.empty())
    return SpikeTrains::OutSpike(HUGE_VAL,0);
  else
    {
      OutSpike r(spikes.back());
      spikes.pop_back(); // warning : reverse order !
      return r;
    }
}
void StreamSpikeTrains::init(void)
{
  sort(spikes.begin(),spikes.end(),greater);
  tn=advance();
}
