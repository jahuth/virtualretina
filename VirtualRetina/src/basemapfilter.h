#ifndef BASEMAPFILTER_H
#define BASEMAPFILTER_H

#include "baserecfilter.h"
#include "cellporthandler.h"

//This class performs temporal recursive filtering on its (CImg) inputs.

// For practical reasons, we also make this base class a CellPortHandler,
// that is, an object able of dealing with different inputs associated
// to different input synapses. However, function new_ionicChannel is
// not useable in this exclusively linear framework.

class BaseMapFilter: public BaseRecFilter, public CImgReader, public CellPortHandler
{

protected:

  int sizeX, sizeY; // these are in pixels
  double ratiopix;  // Spatial equivalent of the temporal 'step'.
                    // gives the conversion from abstract spatial scales to pixels.                 
  CImg<double>** last_inputs;
  CImg<double>** last_values;
  CImg<double>* remember_last_input_location;
  const CImg<double>* last_const_input;
  bool canUseConstInput, dangerous;

  double initial_input_value;

public:

  BaseMapFilter(int x=1,int y=1, double stepp=1);
  BaseMapFilter(const BaseMapFilter& yo);
  virtual ~BaseMapFilter(void);
  BaseMapFilter& operator=(const BaseMapFilter& yo);
  BaseMapFilter& copyFilter(const BaseRecFilter& yo);
  
  virtual BaseMapFilter& set_sizeX(int x);
  virtual BaseMapFilter& set_sizeY(int y);
  virtual BaseMapFilter& set_ratiopix(double r);
  BaseMapFilter& set_initial_input_value (double val);
  
  BaseMapFilter& dangerous_feedInput() ;

  //DRIVING FUNCTIONS:

  //adds new_input to the inputs of the map for the next time step.
  virtual void feedInput(const CImg<double>& new_input, int port=0);

  //proceeds to temporal integration of the map's new input.
  virtual void tempStep(void);

  // ugly, but well... Since filtering is always linear in this object,
  // we forbid the use of function CellPortHandler::new_ionicChannel.
  virtual int new_ionicChannel(double valNernst, InstantSynapse *syn=0)
  {cerr<<"Warning in BaseMapFilter::new_ionicChannel(). Sorry! A BaseMapFilter only performs linear filtering, so it cannot deal with (conductance-based) ionic channels. Your command was not taken into account!"<<endl; return 0;}

  //would be better protected, but... having them public can be useful...
  virtual void allocateValues(void);
  virtual void spatialFiltering(CImg<double>* image){}
};

#endif

