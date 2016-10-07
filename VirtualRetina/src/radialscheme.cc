#include "radialscheme.h"

double RadialScheme::density(double r)
{
  return 1.0;
}

double RadialScheme::integratedDensity(double r)
{
  double sum=0;
  for(int i=0;i<r;++i)
    sum+=this->density(i);
  return sum;
}

double RadialScheme::findRadius(double integrated_density)
{
  double sum=0;
  double radius=0;
  while(sum<integrated_density)
  {
    sum+=this->density(radius);
    radius++;
  }
  return radius;
}

RadialScheme::RadialScheme()
{
//if (UniqueRadialSchemePointer::rsPointer!=NULL)
//    cerr<<"WARNING! You have created more than one radial scheme in your retina, and it bears only one! The last one you have created is the one that will be taken into account by the retina, from this moment of the program on."<<endl;
//UniqueRadialSchemePointer::rsPointer=this;
}

RadialScheme::~RadialScheme()
{
//  UniqueRadialSchemePointer::rsPointer=NULL;
}

/*
RadialScheme * RadialScheme::instance ()
{
  return UniqueRadialSchemePointer::rsPointer;
}

void RadialScheme::dontUse()
{
  UniqueRadialSchemePointer::rsPointer=NULL;
}

//Initializing static variable rsPointer : a source of problems!
RadialScheme * UniqueRadialSchemePointer::rsPointer=NULL;

//RadialScheme * RadialScheme::rsPointer=NULL;	//never worked... why!?!?
*/

