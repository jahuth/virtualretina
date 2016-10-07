#ifndef RETINA_H
#define RETINA_H

#include "retinatools/my_tools.h"

#include "retinamapfilter.h"
#include "condcellmap.h"

#include "retina_spikingchannels.h"

struct Retina;


//********** (0) Base Class for retinal stages ********************

struct RetinaStage: virtual public xmlParam::Xmlizable
{
  Retina *fatherRetina;
  
  RetinaStage(): fatherRetina(0){}
  ~RetinaStage(){}
  void checkFather();
};

// Now here go the various retinal stages, with different possible specializations:

 
//********** (1) Two versions of Outer Plexiform Layer ********************

// The Abstract Base Class (interface)
struct OPLLayers: public RetinaStage
{
  CImgReader *outputSignal, *receptorSignal, *horizontalSignal;
  double sigmaCenter;
  double sigmaSurround;
  double tauSurround;
  double wSurround;

  OPLLayers(){}
  virtual ~OPLLayers(){}
  virtual void feedInput(const CImg<double>& image)=0;
  virtual void tempStep()=0;

  virtual void allocateValues(bool verbose, double initValue)=0;
  CImg<double> stock_output;
};

// Linear version
struct LinearOPL: public OPLLayers
{
  XMLIZE_MACRO;

  double tauCenter;
  uint nCenter;
  int leakyHeatEq;
  RetinaMapFilter *excitCells;
  RetinaMapFilter *inhibCells;
  double relative_ampOPL, ampOPL;

  // (Optional linear undershoot)
  struct Undershoot : public RetinaMapFilter, virtual public xmlParam::Xmlizable
  {
	  XMLIZE_MACRO;
	  double wAdap;
	  double tauAdap;
	  Undershoot(){}
	  ~Undershoot(){}
  };
  Undershoot* undershoot ;
  	  
  LinearOPL();
  ~LinearOPL();
  void feedInput(const CImg<double>& image);
  void tempStep();

  void allocateValues(bool verbose, double initValue);
};

// Non-linear, experimental version with luminance gain control
struct GainControlOPL: public OPLLayers
{
  XMLIZE_MACRO;

  double tauPhoto;
  double ampPhoto;
  double gLeak;
  double ampFeedback;
  double nernstFeedback;

  RetinaMapFilter phototransFilter;
  CondCellMap excitCells;
  RetinaMapFilter controlCond;

  GainControlOPL();
  ~GainControlOPL();
  void feedInput(const CImg<double>& image);
  void tempStep();

  void allocateValues(bool verbose, double initValue);
};


//********* (2) Possible Contrast Gain Control stage *********************

struct FastGainControl: public RetinaStage
{
  XMLIZE_MACRO;

  CImgReader *bipolarSignal, *adaptationSignal;
  double ampInputCurrent;
  double sigmaSurround;
  double tauSurround;
  double gLeak;
  double vThresh;
  double ampFeedback;

  CondCellMap excitCells;
  RetinaMapFilter controlCond;

  FastGainControl();
  ~FastGainControl(){}
  void feedInput(const CImg<double>& image);
  void tempStep();

  void allocateValues(bool verbose=true);

  // OPTIONAL FEATURES, CODED IN THIS OBJECT BUT ACCESIBLE ONLY THROUGH THE 'CUSTOM' VERSION

  // (1) Custom transmission from bipolar signal to adaptation conductance :
  double Einh;
  double exponent;
  uint symmetrize;

  // (2) Optional linear low-pass on the bipolar signal (as mediated by short-range amacrine cells)
  RetinaMapFilter* inhibCells ;
  double wTransient ;
  double tauTransient ;

};

//A more experimental object:
// - decide whether ON/OFF symmetry is wanted or not,
// - can change the inhibitory potential Einh.
// - can change the synaptic transmissions to power laws.
// - can add an intrinsic linear transient to integration in bipolar cells (short range amacrine cells)

struct FastGainControl_custom: public FastGainControl
{
  XMLIZE_MACRO;
  FastGainControl_custom() : FastGainControl() {}
  ~FastGainControl_custom(){}
};



//********* (3) Parvo and Magno ganglion cells layers, with or without spike generation


struct GanglionLayer:	public RetinaStage
{
  XMLIZE_MACRO;

  double sign;
  double vThresh;
  double valThresh;
  double ampInputCurrent;
  double tauTransient;
  double wTransient;
  double sigmaPool;

  RetinaMapFilter supplementaryTransient;
  RetinaMapFilter inputCurrent;
  ret_SpikingChannel* spikingChannel;

  GanglionLayer();
  ~GanglionLayer();
  void feedInput(const CImg<double>& image);
  void tempStep();

  void allocateValues(bool verbose=true);
  
  // overriding xmlinit_... to transmit some parameters (radialScheme, etc.) to some of
  // my fields, right after the XML load :
  void xmlinit_after_children();
};



//********* OK! Finally, here is class "Retina" itself :
//*****************************************************

struct Retina: virtual public xmlParam::Xmlizable
{

  XMLIZE_MACRO;

  double step , input_luminosity_range, pixels_per_degree ;
  int sizeX, sizeY;

  ret_LogPolarScheme* radScheme;
  OPLLayers* outerPlexiformLayer;
  FastGainControl* contrastGainControl;
  vector<GanglionLayer*> ganglionLayers;

  CImgReader input;
  CImg<double> stockInput;

  Retina(double stepp=1);
  ~Retina();

  void feedInput(const CImg<double>& image);
  void tempStep();
  void save_output();

  int get_minimumSize();
  void setFather(RetinaStage *retStage);
  void adjustSizes(int imX, int imY);

  //initValue is the luminance of the uniform screen which is supposed
  // to have been `watched' by the retina for a long time, prior to simulation 
  // if initValue = HUGE_VAL, initValue is automatically taken as input_luminance_range / 2 .
  void allocateValues(bool verbose=true , double initValue=HUGE_VAL );
  
  bool adjusted, done;

protected:
  int adjustChannelSize(ret_SpikingChannel *chan, int imSize, const char dim);
};



#endif





