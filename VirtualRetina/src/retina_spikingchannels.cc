
#include "retina_spikingchannels.h"



// *********************  ret_LogPolarScheme  ****************************

void ret_LogPolarScheme::xmlize()
{
  add_param( R0, "fovea-radius__deg" );
  add_param( K, "scaling-factor-outside-fovea__inv-deg" );
}


// *********************  ret_SpikingChannel  ****************************

void ret_SpikingChannel::xmlize()
{
  add_param(g_leak,"g-leak__Hz");
  add_param(tau_refr,"refr-mean__sec");
  add_param(noise_refr,"refr-stdev__sec");
  add_param(noiseV,"sigma-V");
  add_param(randomInit,"random-init");

  add_child( sampScheme, "" );
  xmlParam::Register<1, ret_sqSampling> (sampScheme, "square-array");
  xmlParam::Register<1, ret_rdSampling> (sampScheme, "circular-array");
  
  add_child( units , "all-units" );
  xmlParam::Register<2, ret_SpikingChannel::ret_OneCell> ( units , xmlParam::names("", "unit") );
}


BaseSpikingChannel::OneCell* ret_SpikingChannel::create_new_unit()
{
  return new ret_OneCell();
}


// ****************** ret_SpikingChannel::ret_OneCell ********************

void ret_SpikingChannel::ret_OneCell::xmlize()
{
  add_param(xOffset,"x-offset__deg");
  add_param(yOffset,"y-offset__deg");
  add_param(Devs::id,"mvaspike-id");
}


// **********************    ret_rdSampling   ****************************

void ret_rdSampling::xmlize()
{
  add_param(diameter,"diameter__deg");
  add_param(foveaDensity,"fovea-density__inv-deg");
}


// **********************    ret_sqSampling   ****************************

void ret_sqSampling::xmlize()
{
  add_param(*sizeX,"size-x__deg");
  add_param(*sizeY,"size-y__deg");
  add_param(uniformDensity,"uniform-density__inv-deg");
}


// ******************  ret_SpikingChannel_adap  **************************

void ret_SpikingChannel_adap::xmlize()
{
  // Because of the differing 'xmlRegister' part, we must rewrite almost 
  // all the code from 'ret_SpikingChannel::xmlize()' :o)
  
  add_param(g_leak,"g-leak__Hz");
  add_param(tau_refr,"refr-mean__sec");
  add_param(noise_refr,"refr-stdev__sec");
  add_param(noiseV,"sigma-V");
  add_param(randomInit,"random-init");
  add_child( sampScheme, "" );
  xmlParam::Register<1, ret_sqSampling> (sampScheme, "square-array");
  xmlParam::Register<1, ret_rdSampling> (sampScheme, "circular-array");
  add_child( units , "all-units" );
  
  // Only differences are the changed xmlRegister, and the additional parameters:
  
  xmlParam::Register<2, ret_SpikingChannel_adap::ret_OneCell_adap> ( units , xmlParam::names("", "unit") );
  
  add_param(delta_g_adap,"delta-g-adap__Hz");
  add_param(tau_g_adap,"tau-g-adap__sec");
  add_param(E_adap,"E-adap");
}


BaseSpikingChannel::OneCell* ret_SpikingChannel_adap::create_new_unit()
{
  return new ret_OneCell_adap();
}


// Must precise which specialization of the diamond scheme should be applied:
void ret_SpikingChannel_adap::initialize_new_unit(double xx, double yy)
{
  BaseSpikingChannel_adap::initialize_new_unit(xx,yy);
}

// ************  ret_SpikingChannel_adap::ret_OneCell_adap  **************

// Must precise which specialization of the diamond scheme should be applied:
void ret_SpikingChannel_adap::ret_OneCell_adap::xmlize()
{
  ret_SpikingChannel::ret_OneCell::xmlize();
}

