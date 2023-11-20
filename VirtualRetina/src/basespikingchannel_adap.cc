
#include "basespikingchannel_adap.h"


BaseSpikingChannel_adap::BaseSpikingChannel_adap() : BaseSpikingChannel(),
                                    delta_g_adap(0), tau_g_adap(0), E_adap(0)
{}


BaseSpikingChannel::OneCell* BaseSpikingChannel_adap::create_new_unit()
{
  return new OneCell_adap();
}


void BaseSpikingChannel_adap::initialize_new_unit(double xx, double yy)
{
  BaseSpikingChannel::initialize_new_unit(xx,yy);
  // only difference: transmit the adaptation parameters to the new unit
  OneCell_adap *uBack=dynamic_cast<OneCell_adap*>(units.back());
  uBack->set_delta_g_adap(delta_g_adap);
  uBack->set_tau_g_adap(tau_g_adap);
  uBack->set_E_adap(E_adap);
}


//Channel parameter definition functions :

BaseSpikingChannel_adap& BaseSpikingChannel_adap::set_delta_g_adap(double f)
{
  checkStatus();
  delta_g_adap=f;
  return *this;  // I guess?
}

BaseSpikingChannel_adap& BaseSpikingChannel_adap::set_tau_g_adap(double f)
{
  checkStatus();
  tau_g_adap=f;
  return *this;  // I guess?
}

BaseSpikingChannel_adap& BaseSpikingChannel_adap::set_E_adap(double f)
{
  checkStatus();
  E_adap=f;
  return *this;  // I guess?
}


