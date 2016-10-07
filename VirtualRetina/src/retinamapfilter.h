#ifndef RETINAMAPFILTER_H
#define RETINAMAPFILTER_H

#include "basemapfilter.h"
#include "radialfilter.h"


//******************** RetinaMapFilter: ****************************************

// A BaseMapFilter whose spatial filter is a (quasi) gaussian kernel,
// POSSIBLY WITH A VARYING RADIAL SCALE OF FILTERING.
// An input nonlinearity can also be added.
// Possibly or alternatively, the filter can mimick cellular integration of
// ONE layer of cells with gap junctions, through function leakyHeatEquation.

class RetinaMapFilter: public BaseMapFilter
{
protected:
  double sigmaPool;
  double gCoupling;
  void spatialFiltering(CImg<double>* image);

public:
  RetinaMapFilter(int x=1, int y=1, double stepp=1);
  ~RetinaMapFilter(void);
 
  void set_sigmaPool(double val);
  void leakyHeatEquation(double tauLeak, double sigmaHeat);
  
  RetinaMapFilter& set_radScheme(RadialScheme *rad); // used by the RadialFilters
  // overriding these three operators:
  BaseMapFilter& set_sizeX(int x);
  BaseMapFilter& set_sizeY(int y);
  BaseMapFilter& set_ratiopix(double r);

  // same tempStep as before, but with additional possibility for 
  // intra-layer coupling through gap junctions, with strength gCoupling.
  void tempStep();    	

  RadialFilter sigmaPoolFilter, leakyHeatFilter;

//Would be better protected, but having it public can be useful
  void allocateValues(void);
};
#endif

