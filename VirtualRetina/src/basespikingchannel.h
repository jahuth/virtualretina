
#ifndef BASE_SPIKINGCHANNEL_H
#define BASE_SPIKINGCHANNEL_H

#include "spik_units.h"
#include "radialscheme.h"
#include <xmlparameters.h>

// Spikes and interactions between cells : This class doesnt take lateral
// interactions into account. Must be included (one day?) in a derived object.

// The rule we choose is now the following: each derived type of BaseSpikingChannel
// has a *single type* of spiking unit (derived from BaseSpikingChannel::OneCell).
// If you want a new type of spiking unit (e.g., with adaptation), create a new
// derived type of BaseSpikingChannel.

// All the *sampling* properties of the SpikingChannel are now handled by a dedicated
// subobject of type SamplingScheme (and derived types).

// All created classes inherit from xmlParam::Xmlizable, so that their derived types can be
// given xml specifications. But we do not specialize their functions xmlize() for the moment.

class BaseSpikingChannel : virtual public xmlParam::Xmlizable
{

protected :

  double step;        // Discretization step shared by the whole retina
  double ratiopix;    // conversion ratio from the abstract spatial units to pixels
  
  //Spiking parameters for of all the channels' cells:
  double  g_leak, tau_refr, noise_refr, noiseV;
  int randomInit;     // randomly initialize the units' membrane potential?
  
  bool done;
  virtual void checkStatus();

public :

  // Base class corresponding to one unit in the array
  struct OneCell : virtual public IntegratorNeuron_cpm, virtual public xmlParam::Xmlizable
  {
    double xOffset;   //offset, to simulate the position of the unit in an array of units.
    double yOffset;
    OneCell(void);
    virtual ~OneCell(void){}
  };

  vector<OneCell*> units;
  virtual OneCell* create_new_unit();
  virtual void initialize_new_unit(double xx, double yy);

  // Abstract base class ('interface') corresponding to the sampling scheme used by the array
  struct SamplingScheme : virtual public xmlParam::Xmlizable
  {
    double *sizeX, *sizeY;    // size of the array
    BaseSpikingChannel *father;
    virtual double cellDensity(double r)=0;
    SamplingScheme();
    virtual ~SamplingScheme(){}
  protected : 
    bool done;
    virtual void allocateValues()=0;
    virtual void createUnits()=0;
    friend class BaseSpikingChannel;
  };
  
  SamplingScheme *sampScheme;
  RadialScheme *radScheme; // does my sampling procedure follow a RadialScheme ?

  //Constructors/Destructor :
  BaseSpikingChannel(double stepp=1.0);
  virtual ~BaseSpikingChannel(void);

  //Channel parameter definition functions :
  BaseSpikingChannel& set_step(double s);
  BaseSpikingChannel& set_ratiopix(double r);
  BaseSpikingChannel& set_radScheme(RadialScheme *rad);
  BaseSpikingChannel& set_g_leak(double f);
  BaseSpikingChannel& set_tau_refr(double f);
  BaseSpikingChannel& set_noise_refr(double f);
  BaseSpikingChannel& set_noiseV(double f);

  //Driving function
  void feedInput(const CImg<double> & image);

  //Initialization functions
  virtual void allocateValues();
};

#endif

