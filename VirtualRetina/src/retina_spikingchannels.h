#ifndef RETINA_SPIKINGCHANNELS_H
#define RETINA_SPIKINGCHANNELS_H

#include "rd_sampling.h"
#include "sq_sampling.h"
#include "logpolarscheme.h"
// Registering the 'adaptation' version of the spikingChannel (not tested in version 2.2):
#include "basespikingchannel_adap.h"

// Goal of this header (and associated .cc):
// define the 'xmlized' versions of SpikingChannels (+ LogPolarScheme)
// with parameters defined in retinal degrees.

// This header and associated .cc are separated from retina.h / retina.cc
// only for conveniency (to avoid an extremely long file :p )


// (We put this one here for conveniency, although it is not so to speak a SpikingChannel)
struct ret_LogPolarScheme : public LogPolarScheme
{
  XMLIZE_MACRO;
};


// NOW, various 'retinal' specialization of SpikingChannel-related objects:

struct ret_SpikingChannel: virtual public BaseSpikingChannel
{
  XMLIZE_MACRO; // Here, we define all 'spiking unit'-related parameters
  
	// Retinal cells, with parameters IN DEGREES
	struct ret_OneCell : virtual public BaseSpikingChannel::OneCell
	{
	  XMLIZE_MACRO;
	};
	
	// override this function to create ret_OneCell cells:
	BaseSpikingChannel::OneCell* create_new_unit();
};
	

struct ret_rdSampling : public rdSampling
{
  XMLIZE_MACRO;
};

struct ret_sqSampling : public sqSampling
{
  XMLIZE_MACRO;
};


//and with spike-frequency adaptation: !!! Diamond scheme mayhem !!!

struct ret_SpikingChannel_adap: public ret_SpikingChannel, public BaseSpikingChannel_adap
{
  XMLIZE_MACRO;
  
	struct ret_OneCell_adap : public ret_SpikingChannel::ret_OneCell, public BaseSpikingChannel_adap::OneCell_adap
	{
	  XMLIZE_MACRO;
	};
	
	// override these functions to create and initialize ret_OneCell_adap cells:
	BaseSpikingChannel::OneCell* create_new_unit();
  void initialize_new_unit(double xx, double yy);
};


#endif

