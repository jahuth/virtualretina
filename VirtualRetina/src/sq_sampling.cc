
#include "sq_sampling.h"


sqSampling::sqSampling() : BaseSpikingChannel::SamplingScheme()
{
  sizeX=new double(0);
  sizeY=new double(0);
  uniformDensity=1;
  noisePos=0;
}


sqSampling::~sqSampling()
{
  delete sizeX;
  delete sizeY;
}

// Core function: Allocate values  

void sqSampling::allocateValues()
{
  checkFather();

  //Checking if your simulation is using a radial scheme, but heck... you do what you want!...
  if(father->radScheme)
    cerr<<"Warning in sqSampling::allocateValues(): you are initializing "
    "a square-spiking-channel in a simulation that uses a radial scheme. Probably, "
    "a circular-spiking-channel, whose sampling can follow the density given by the "
    "radial scheme, would have been more appropriate? Anyway, I respect your choice."<<endl;

  numberX=(int)floor((*sizeX)*uniformDensity); // formula=checked
  numberY=(int)floor((*sizeY)*uniformDensity);
  done=true;
}

void sqSampling::createUnits()
{
  checkDone();
  for(int nx=1;nx<=numberX;++nx)
    for(int ny=1;ny<=numberY;++ny)
    {
      double rnoise=noisePos*ranf()/uniformDensity;
      double thetanoise=2*Pi*ranf();
      double xnoise=rnoise*cos(thetanoise);
      double ynoise=rnoise*sin(thetanoise);

      // formula=checked. 
      // The (1+numberX)/2 -th cell along the line (if it exists)
      // has offset 0 (if noiseless).
      father->initialize_new_unit(
        (nx-(1+numberX)/2.0)/uniformDensity+xnoise,
        (ny-(1+numberY)/2.0)/uniformDensity+ynoise);
    }
}


// Boring (and useless) parameter definition functions

void sqSampling::checkStatus(void)
{
  checkFather();
  if(done)
  {
    cerr<<"Error in sqSampling::set_xxx() : You're trying to change a parameter after it was initialized."<<endl;
    exit(0);
  }
}

void sqSampling::checkDone(void)
{
  checkFather();
  if(!done)
  {
    cerr<<"Error in sqSampling::checkDone(): Must first allocateValues()."<<endl;
    exit(0);
  }
}

void sqSampling::checkFather(void)
{
  if(!father)
  {
    cerr<<"Error in sqSampling::checkFather(): Must ALWAYS define the father BaseSpikingChannel."<<endl;
    exit(0);
  }
}

void sqSampling::set_sizeX(int sx)
{
  checkStatus();
  *sizeX=sx;
}

void sqSampling::set_sizeY(int sy)
{
  checkStatus();
  *sizeY=sy;
}

void sqSampling::set_uniformDensity(double f)
{
  checkStatus();
  uniformDensity=f;
}
void sqSampling::set_channel(int sx, int sy, double dens)
{
  checkStatus();
  *sizeX=sx;
  *sizeY=sy;
  uniformDensity=dens;
}

void sqSampling::set_noisePos(double f)
{
  checkStatus();
  noisePos=f;
}


