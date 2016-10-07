#ifndef RADIALFILTER_H
#define RADIALFILTER_H

#include "retinatools/misc_tools.h"
#include "retinatools/cimg_tools.h"
#include "radialscheme.h"

// Hello, this is class RadialFilter... It is a class that can realize
// spatial recursive filtering on images (as Deriche filters), with
// spatially-varying coeficients. The way its coeficients vary with
// eccentricity r from the retina is given by the chosen RadialScheme
// for the simulation. If no RadialScheme was chosen (that is, when
// RadialScheme::getInstance()=NULL), then the RadialFilter performs
// a basic, spatially-uniform filtering. In the future, we might change
// this code a bit, if someone wishes to use simultaneuously different
// RadialSchemes in a single program (I dont see why, but heck...)

// The object might evolve in the future. For the moment it only
// performs spatially-variant deriche filters (including blur)!

// The methods are code from CImg.h, slightly adapted to make it more
// readable ( in my humble opinion :-) )

class RadialFilter
{
protected:
  double ratiopix;  // Spatial equivalent of the temporal 'step'.
                    // gives the conversion from abstract spatial scales to pixels.
  int sizeX, sizeY; // (Note: These are *really* in pixels)
  double sigma0;    // This one is in abstract spatial scale.
  int order;

  // The reference RadialScheme
  RadialScheme *radScheme;
  
  //low-level filtering tools:
  inline void radial_deriche_map(CImg<double>& image, int x0,int y0,int z0,int k0,int nb, int offset);
  double* Ybuf;   //the buffer to stock the values of the forward passing of deriche filter.

  //spatially-varying recursive coeficients and how to allocate them.
  CImg<double> A1, A2, A3, A4, B1, B2;
  bool done;
  void checkStatus(void);
  bool isRadialScheme;
  void allocateValues();

public:

  RadialFilter(int size_x=1, int size_y=1);
  ~RadialFilter();

  // parameter initilization... this is boring but prettier I guess...
  RadialFilter& set_sizeX(int x);
  RadialFilter& set_sizeY(int y);
  RadialFilter& set_ratiopix(double r);
  RadialFilter& set_radScheme(RadialScheme *rad);
  
  // Set the scale of filtering at center of image (in abstract spatial scales). 
  // Scale of filtering outside of the center is given by radScheme.
  RadialFilter& set_sigma0(double sigma);
  
  // Set the order of the associated Deriche filter. 
  // order 0 is blur, order 1 is first derivative, order 2 second derivative... 
  // Only order 0 is useful for the moment but well... WHO KNOWS?
  RadialFilter& set_order(int ord);


  // Useful functions:
  void radiallyVariantDeriche(CImg<double>& image, const char axe='x');
  CImg<double> get_radiallyVariantDeriche(const CImg<double>& image, const char axe='x');
  
  void radiallyVariantBlur(CImg<double>& image);
  CImg<double> get_radiallyVariantBlur(const CImg<double>& image);

};
#endif
