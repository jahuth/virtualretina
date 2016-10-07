#ifndef CONDCELLMAP_H
#define CONDCELLMAP_H

#include "cellporthandler.h"
#include "radialfilter.h"

class CondCellMap : public CImgReader, public CellPortHandler

// performs a non-linear, conductance-based filtering on the image, 
// possibly including lateral interactions between the cells (=pixels) of the layer.

{
protected:

  double step;

  int sizeX, sizeY;  // these are in pixels
  double ratiopix;   // Spatial equivalent of the temporal 'step'.
                     // gives the conversion from abstract spatial scales to pixels.                   
  double  g_leak, G; // parameter for the leak and coupling conductances of the network.

  RadialScheme *radScheme;
  
  CImg<double> *daValues, *preceding;
  CImgList<double> storeInputs;			//the place where we store input conductances AND currents
  CImg<double> totalCond, attenuationMap; 	//used in new implementation (approximate) of the driving equation

  bool done;                //tell whether initialization is over.
  static bool conductanceWarning; //static variables to print ONCE only a warning about possible negative conductances

public:
  vector<CImgReader> readInputs;		  //to be able to read input values (conductances and currents...)
  RadialFilter gapJunctionFilter;			//the radially variant filtering tool.

  CondCellMap(int x=0, int y=0, double stepp=1);
  ~CondCellMap(void);

  //All the 'set_X' functions.
  CondCellMap& set_step(double stepp);
  CondCellMap& set_g_leak(double val);
  CondCellMap& set_G(double val);
  CondCellMap& set_sizeX(int x);
  CondCellMap& set_sizeY(int y);
  CondCellMap& set_ratiopix(double r);
  CondCellMap& set_radScheme(RadialScheme *rad); // used by the RadialFilter

  //Driving functions:
  CondCellMap& feedInput(const CImg<double>& input, int port=0, bool shutUp=false); //feeding input in channel 'port'
  //and two 'old' names:
  CondCellMap& feedCurrent(const CImg<double>& curr, int port=0);	//feeding current in channel 'port'
  CondCellMap& feedCond(const CImg<double>& input, int port, bool shutUp=false);	//feeding conductance in channel 'port'

  void tempStep(void);

//Would be better protected, but having it public can be useful
  void allocateValues(void);
};
#endif

