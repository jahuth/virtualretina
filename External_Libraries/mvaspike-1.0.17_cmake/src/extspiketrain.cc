#include <math.h>
#include <algorithm>
#include <functional>
using namespace std;
#include "extspiketrain.h"

ExtSpikeTrain::ExtSpikeTrain():
  SpikeTrain()
{
}

double ExtSpikeTrain::advance(void)
{
  if (spikes.empty())
    return HUGE_VAL;
  else
    {
      double r=spikes.back();
      spikes.pop_back(); // warning : reverse order !
      return r;
    }
}
void ExtSpikeTrain::init(void)
{
  sort(spikes.begin(),spikes.end(),greater<double>());
  tn=advance();
}

void ExtSpikeTrain::add_spike(double t)
{
  spikes.push_back(t);
}

void ExtSpikeTrain::record_state(void)
{
    // TODO
}
