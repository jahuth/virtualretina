
#ifndef SQ_SAMPLING_H
#define SQ_SAMPLING_H

#include "basespikingchannel.h"

//And here comes class sqSampling : it creates a layer of spiking cells 
//that parse the visual field according to an homogenous square sampling
//pattern, with a uniform one-dimensional density.

class sqSampling: virtual public BaseSpikingChannel::SamplingScheme
{

protected :
  double uniformDensity; //1D-sampling density (in units per 1D-pixel).

  //will define the number of cells in the square array
  int numberX, numberY;

  //Noise indicators :
  double noisePos;

  void checkStatus();
  void checkDone();
  void checkFather();
  
public :

  // The core function of the sampling scheme:
  void allocateValues();
  void createUnits();
  
  //Indexing tool :
  int index(int nx, int ny)
  {
    checkDone();
    return nx*numberX+ny;
  }

  //Cell density:
  double cellDensity(double r)
  {
    checkFather();
    return uniformDensity;
  }

  //Constructors/Destructor :
  sqSampling();
  virtual ~sqSampling();

  //Channel parameter definition functions :
  void set_sizeX(int f);
  void set_sizeY(int f);
  void set_uniformDensity(double f);
  void set_channel(int sx, int sy, double dens);
  void set_noisePos(double f);

};

#endif

