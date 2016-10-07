
#include "rd_sampling.h"


rdSampling::rdSampling() : BaseSpikingChannel::SamplingScheme(), summed_units(0), angular_offset(0)
{
  diameter=0;
  sizeX=&diameter; sizeY=&diameter;
  foveaDensity=1;
  nb_circles=0;
  noisePos=0;
  randomAngularOffset=true;
}


rdSampling::~rdSampling(void)
{
  delete[] summed_units;
  delete[] angular_offset;
}


// Sampling functions

double rdSampling::get_r(int circle_index)
{
  checkFather();
  return (father->radScheme)?
          father->radScheme->findRadius(circle_index/foveaDensity):
          circle_index/foveaDensity;
}
  
double rdSampling::cellDensity(double r)
{
  checkFather();
  return (father->radScheme)?
          foveaDensity* father->radScheme->density(r):
          foveaDensity;
}

int rdSampling::get_nb_units(double r)
{
  checkFather();
  if (r==0)
    return 1;
  else
    return (int)floor(2*Pi*cellDensity(r)*r);
}


// Indexing functions

int rdSampling::index(int n, int K)
{       //warning ! you must have 0<=n<nb_circles and 0<=K<summed_units[n+1]-summed_units[n]
  checkDone();
  return summed_units[n]+K;
}

int rdSampling::get_n(int ind)
{
  checkDone();
  int n=0;
  while(summed_units[n]<ind+1)
    n++;
  return n-1;
}

int rdSampling::get_K(int ind)
{
  checkDone();
  int n=get_n(ind);
  return ind-summed_units[n];
}


// Core functions: allocateValues and createUnits

void rdSampling::allocateValues()
{
  checkFather();
  done=true; // must put this here ;)
  
  // FIRST STEP : counting circles, units in each circle, and total
  // number of units, i.e. initializing index functions.
  double r=0;
  int Nb=0, n=0;
  while(r<=diameter/2.0)
  {
    Nb+=get_nb_units(r);
    n++;
    r=get_r(n);
  }
  nb_circles=n;
  summed_units=new int[nb_circles+1];
  summed_units[0]=0;
  for (int n=0;n<nb_circles;++n)
    summed_units[n+1]=summed_units[n]+get_nb_units(get_r(n));

  // SECOND STEP : getting all angular offsets
  int ind=0;
  angular_offset=new double[nb_circles];
  for(int n=0;n<nb_circles;++n)
  {
    double radius=get_r(n);
    double angular_step=2*Pi/(double)get_nb_units(radius);
    if (randomAngularOffset)                  //a detail, really...
      angular_offset[n]=ranf()*angular_step;
    else 
      angular_offset[n]=0;
  }
}

void rdSampling::createUnits()
{
  checkDone();
  for(int n=0;n<nb_circles;++n)
  {
    double radius=get_r(n);
    double angular_step=2*Pi/(double)get_nb_units(radius);
    for(int k=0;k<get_nb_units(radius);k++)
    {
      double rnoise=noisePos*ranf()/cellDensity(radius);
      double thetanoise=2*Pi*ranf();
      double xnoise=rnoise*cos(thetanoise);
      double ynoise=rnoise*sin(thetanoise);
      father->initialize_new_unit(
        radius*cos(angular_offset[n]+angular_step*k)+xnoise ,
        radius*sin(angular_offset[n]+angular_step*k)+ynoise );
    }
  }
}


// Boring (and useless) parameter definition functions

void rdSampling::checkStatus(void)
{
  checkFather();
  if(done)
  {
    cerr<<"Error in rdSampling::set_xxx(): You're trying to change a parameter after it was initialized."<<endl;
    exit(0);
  }
}

void rdSampling::checkDone(void)
{
  checkFather();
  if(!done)
  {
    cerr<<"Error in rdSampling::checkDone(): Must first allocateValues()."<<endl;
    exit(0);
  }
}

void rdSampling::checkFather(void)
{
  if(!father)
  {
    cerr<<"Error in rdSampling::checkFather(): Must ALWAYS define the father BaseSpikingChannel."<<endl;
    exit(0);
  }
}

void rdSampling::set_diameter(int d)
{
  checkStatus();
  diameter=d;
}

void rdSampling::set_foveaDensity(double f)
{
  checkStatus();
  foveaDensity=f;
}

void rdSampling::set_channel(int diam, double d_fov)
{
  checkStatus();
  diameter=diam;
  foveaDensity=d_fov;
}

void rdSampling::set_noisePos(double f)
{
  checkStatus();
  noisePos=f;
}

