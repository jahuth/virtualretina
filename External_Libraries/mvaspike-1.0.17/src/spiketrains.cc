#include "spiketrains.h"
#include "error.h"

SpikeTrains::SpikeTrains()
  : Devs()
{
  nb_inport=0;
}

SpikeTrains::~SpikeTrains()
{
}

void SpikeTrains::delta_ext(double t, int x)
{
  warning("SpikeTrains::delta_ext does nothing..."); 
}

double SpikeTrains::ta(void)
{
  return tn.t;
}

int SpikeTrains::delta_int(double t)
{
  int r=tn.nb;
  tn=advance();
  if (rec!=NULL) {
    rec->add_spike(id,r);
      record_state();
  }
  return r;
}

int SpikeTrains::get_imin(void)
{
  return tn.nb;
}
