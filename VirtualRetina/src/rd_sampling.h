
#ifndef RD_SAMPLING_H
#define RD_SAMPLING_H

#include "basespikingchannel.h"

//And here comes class rdSampling : it creates a layer of spiking cells
//that circularly parse the visual field. This is the most adapted object
//(at least) whenever the retina has a radial density scheme RadialScheme.

//Spatial distribution of units :

//One channel's sampling scheme is defined by its foveal radial (one
//dimensional) density d0, and the chosen RadialScheme for the retina.
//Rmax is the total radius of this channel of units.
//All theoretical positions can be modified according to a uniform law
//noise, with amplitude equal to "noise_pos/1d-density"


class rdSampling: virtual public BaseSpikingChannel::SamplingScheme
{

protected :

  double diameter;                // size parameter (diameter of the array)
  double foveaDensity;            // sampling density in the fovea.
  
  //tools to retrieve "position<-->identificator"  of units :
  int nb_circles;          //number of circles in the channel, INCLUDING central point considered as the first circle.
  int* summed_units;       //summed_units[n]  = number of units in the channel in the n first circles (0 if n=0, 1 if n=1) (n<nb_circles+1)
  double* angular_offset;  //angular_offset[n] is the angular_offset of circle n.

  // Noise indicators :
  double noisePos;
  bool randomAngularOffset;
  
  void checkStatus();
  void checkDone();
  void checkFather();

public :

  // The core function of the sampling scheme:
  void allocateValues();
  void createUnits();
  
  // Functions to handle SAMPLING :
  double get_r(int circle_index);
  double cellDensity(double r);
  int get_nb_units(double r);

  // Indexing functions :
  // you must have 0<=n<nb_circles and 0<=K<summed_units[n+1]-summed_units[n]
  int index(int n, int K); 
  int get_n(int ind);
  int get_K(int ind);

  //Constructors/Destructor :
  rdSampling();
  virtual ~rdSampling();

  //Channel parameter definition functions :
  void set_diameter(int f);
  void set_foveaDensity(double f);
  void set_channel(int diam, double d_fov);
  void set_noisePos(double f);
  void set_randomAngularOffset(double f);
};

#endif

