
#ifndef BASE_SPIKINGCHANNEL_ADAP_H
#define BASE_SPIKINGCHANNEL_ADAP_H

#include "basespikingchannel.h"

// The version of BaseSpikingChannel with spike-frequency adaptation (experimental)
// WARNING : Not tested since remoulding of the code (version 2.2)

class BaseSpikingChannel_adap : virtual public BaseSpikingChannel
{

protected :
  double delta_g_adap, tau_g_adap, E_adap;
  
public :
  // redefine the spiking unit (always called 'OneCell'). 
  // Note the diamond scheme from IntegratorNeuron_cpm.
  // Note the absence of any supplementary code.
  struct OneCell_adap : virtual public BaseSpikingChannel::OneCell, public IntegratorNeuron_adap_exp
  {};
  
  BaseSpikingChannel_adap();
  // override the initialization, to create and initialize
  // OneCell_adap units *with adaptation*.
  BaseSpikingChannel::OneCell* create_new_unit();
  void initialize_new_unit(double xx, double yy);
  
  BaseSpikingChannel_adap& set_delta_g_adap(double f);
  BaseSpikingChannel_adap& set_tau_g_adap(double f);
  BaseSpikingChannel_adap& set_E_adap(double f);
  
};

#endif

