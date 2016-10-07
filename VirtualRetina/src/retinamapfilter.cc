#include "retinamapfilter.h"


RetinaMapFilter::RetinaMapFilter(int x, int y, double stepp) : BaseMapFilter(x,y,stepp)
{
  sigmaPool=0;
  gCoupling=0;
  sigmaPoolFilter.set_order(0); // simple blur
  leakyHeatFilter.set_order(0);
}


RetinaMapFilter::~RetinaMapFilter(void){}


void RetinaMapFilter::allocateValues(void)
{
  if((gCoupling!=0)&&( BaseMapFilter::N>1))
    cerr<<"Warning in RetinaMapFilter: if you use function leakyHeatEquation, you must not add any other temporal initialization on your filter."<<endl;

  BaseMapFilter::allocateValues();
  
  // initializing the leakyHeat filter
  leakyHeatFilter.set_sigma0(sqrt(2*gCoupling*step));
  
  if(sigmaPool) //cannot use last_const_input
    canUseConstInput=false;
}


void RetinaMapFilter::spatialFiltering(CImg<double>* image)
{
  if(sigmaPool)
    sigmaPoolFilter.radiallyVariantBlur( *image );
}


void RetinaMapFilter::tempStep(void)
{
  BaseMapFilter::tempStep();
  if(gCoupling!=0)
    leakyHeatFilter.radiallyVariantBlur( *targets );	//last_values...
}

// ****** all the 'set' functions

void RetinaMapFilter::set_sigmaPool(double val)
{
  sigmaPool=val;
  sigmaPoolFilter.set_sigma0(sigmaPool);
}

void RetinaMapFilter::leakyHeatEquation(double tauLeak, double sigmaHeat)
{
  this->gCoupling=pow(sigmaHeat,2)/(2*tauLeak);
  BaseRecFilter::Exp(tauLeak);
}

BaseMapFilter& RetinaMapFilter::set_sizeX(int x)
{
  BaseMapFilter::set_sizeX(x);
  sigmaPoolFilter.set_sizeX(x);
  leakyHeatFilter.set_sizeX(x);
  return *this;
}

BaseMapFilter& RetinaMapFilter::set_sizeY(int y)
{
  BaseMapFilter::set_sizeY(y);
  sigmaPoolFilter.set_sizeY(y);
  leakyHeatFilter.set_sizeY(y);
  return *this;
}

BaseMapFilter& RetinaMapFilter::set_ratiopix(double r)
{
  BaseMapFilter::set_ratiopix(r);
  sigmaPoolFilter.set_ratiopix(r);
  leakyHeatFilter.set_ratiopix(r);
  return *this;
}

RetinaMapFilter& RetinaMapFilter::set_radScheme(RadialScheme *rad)
{
  checkStatus();
  sigmaPoolFilter.set_radScheme(rad);
  leakyHeatFilter.set_radScheme(rad);
  return *this;
}

