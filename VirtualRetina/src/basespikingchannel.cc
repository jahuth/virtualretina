
#include "basespikingchannel.h"


//********* Code for BaseSpikingChannel::OneCell ***********************

BaseSpikingChannel::OneCell::OneCell() : IntegratorNeuron_cpm(), xOffset(0), yOffset(0)
{}


//********* Code for BaseSpikingChannel::SamplingSheme *****************

BaseSpikingChannel::SamplingScheme::SamplingScheme() : father(0), done(false)
{}


//*********    Code for BaseSpikingChannel   ***************************

BaseSpikingChannel::BaseSpikingChannel(double stepp) : sampScheme(0), done(false), radScheme(0)
{
  step=stepp;
  ratiopix=1;
  
  g_leak=50;
  tau_refr=0.003;
  noise_refr=0.0;
  noiseV=0.0;
  randomInit=false;
}


BaseSpikingChannel::~BaseSpikingChannel(void)
{
  for(uint i=0;i<units.size();++i) 
    delete[] units[i];
  delete sampScheme;
}


void BaseSpikingChannel::allocateValues(void)
{
	if(sampScheme && (!sampScheme->done))  
	  sampScheme->allocateValues();
	  
	if(!units.size()) //meaning that units have not been loaded yet from an xml file
	{
	  if(!sampScheme)
	  {  
      cerr<<"Error in BaseSpikingChannel: No sampling scheme was defined!"<<endl;
      exit(0);
    }
    sampScheme->createUnits(); // create all new units
  }
	else
	{
	  for(uint i=0;i<units.size();++i) 
	  {
	    units[i]->set_g_leak(this->g_leak); //spiking properties from class ConductanceNeuron
	    units[i]->tau_refr=this->tau_refr;
	    units[i]->noise_refr=this->noise_refr;
	    units[i]->set_noiseV(this->noiseV);
	    units[i]->set_step(this->step); //the IntegratorNeuron parameter
	    if(randomInit) units[i]->set_V ( ranf() );
	  }
	}
	done=true;
}


BaseSpikingChannel::OneCell* BaseSpikingChannel::create_new_unit()
{
  return new OneCell();
}


void BaseSpikingChannel::initialize_new_unit(double xx, double yy)
{
  units.push_back(create_new_unit());

  units.back()->set_g_leak(g_leak); //spiking properties from class ConductanceNeuron
  units.back()->tau_refr=this->tau_refr;
  units.back()->noise_refr=this->noise_refr;
  units.back()->set_noiseV(noiseV);
  units.back()->set_step(step); //the IntegratorNeuron parameter

  units.back()->xOffset=xx;  //OneCell properties
  units.back()->yOffset=yy;

  if(randomInit)
	  units.back()->set_V ( ranf() );
}


//Feeding an image to the retina.

void BaseSpikingChannel::feedInput(const CImg<double> & image)
{
  if (!done) 
    allocateValues();
  double xMid=image.width()/2.0, yMid=image.height()/2.0;
  
  for(uint ind=0;ind<units.size();++ind)
    units[ind]->feedCurrent(
      image.linear_atXY(xMid+ratiopix*units[ind]->xOffset,
                        yMid+ratiopix*units[ind]->yOffset),
      0,true);
}


//Channel parameter definition functions :

void BaseSpikingChannel::checkStatus(void)
{
  if(done)
  {
    cerr<<"Error in BaseSpikingChannel: You're trying to change a parameter after it was initialized."<<endl;
    exit(0);
  }
}

BaseSpikingChannel& BaseSpikingChannel::set_step(double s)
{
  checkStatus();
  step=s;
  return *this;
}

BaseSpikingChannel& BaseSpikingChannel::set_ratiopix(double r)
{
  checkStatus();
  ratiopix=r;
  return *this;
}

BaseSpikingChannel& BaseSpikingChannel::set_radScheme(RadialScheme *rad)
{
  checkStatus();
  radScheme=rad;
  return *this;
}

BaseSpikingChannel& BaseSpikingChannel::set_g_leak(double f)
{
  checkStatus();
  g_leak=f;
  return *this;
}
BaseSpikingChannel& BaseSpikingChannel::set_tau_refr(double f)
{
  checkStatus();
  tau_refr=f;
  return *this;
}
BaseSpikingChannel& BaseSpikingChannel::set_noise_refr(double f)
{
  checkStatus();
  noise_refr=f;
  return *this;
}
BaseSpikingChannel& BaseSpikingChannel::set_noiseV(double f)
{
  checkStatus();
  noiseV=f;
  return *this;
}


