#ifndef RADIALSCHEME_H
#define RADIALSCHEME_H

#include "retinatools/misc_tools.h"
#include <xmlparameters.h>

// Hello, this is class RadialScheme... It is constructed around one 
// particular function: its 1D-DENSITY function that defines the local
// cell density in each region of the image, as a function of radius r
// to center of image.

// Two other functions are the integral of the density function
// and the inverse of the integral.

// The three functions are defined as virtual functions, and must be
// defined in derived objects (only one for the moment is LogPolarScheme).

// The last two functions are only required if you start using a layer 
// of discrete spiking units. The last two functions possess a default
// definition, consisting of integrating the density function step by step
// (the basic spatial unity being supposed to be one pixel). But it is
// more precise and faster, to also define explicitly the two integral
// functions, when they have an analytical formula.

// Honestly, I dont see why you should bother to use soething else than 
// a log-polar scheme, but well... Who KNOWS??

// OBSOLETE: static function instance() used to define class RadialScheme()
// as a singleton (as there is generally only one scheme for the whole retina).
// I now removed this weird functionality.

class RadialScheme: virtual public xmlParam::Xmlizable
{
public:

  RadialScheme();
  virtual ~RadialScheme();

  virtual double density(double r);
  virtual double integratedDensity(double r);
  virtual double findRadius(double integrated_density);
  
  // Obsolete
  //static RadialScheme * instance();
  //static void dontUse();
};

// Obsolete
/*class UniqueRadialSchemePointer
{
  friend class RadialScheme;
  static RadialScheme * rsPointer;
};*/

#endif
