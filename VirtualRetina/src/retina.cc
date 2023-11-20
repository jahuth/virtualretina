#include "retina.h"

//***************************************************************************
//******************  The Retina functions   ********************************


Retina::Retina(double stepp)
{
  step=stepp; input_luminosity_range=255.0; pixels_per_degree=1.0;
  sizeX=0; sizeY=0;
  //Just for safety, really...
  radScheme=0; outerPlexiformLayer=0; contrastGainControl=0;
  done=false; adjusted=false;
}


void Retina::xmlize()
{
  // General parameters
  add_param(step,"temporal-step__sec");
  add_param( input_luminosity_range , "input-luminosity-range");
  add_param( pixels_per_degree , "pixels-per-degree");

  // Log-polar scheme
  add_child(radScheme,"");
  xmlParam::Register<1, ret_LogPolarScheme>(radScheme , "log-polar-scheme");

  // To be applied right after 
  // OPL schemes
  add_child(outerPlexiformLayer,"outer-plexiform-layer");
  xmlParam::Register<1, LinearOPL > ( outerPlexiformLayer, "linear-version" );
  xmlParam::Register_init<1> (outerPlexiformLayer,"linear-version", &Retina::setFather, *this );
  xmlParam::Register<1, GainControlOPL > (outerPlexiformLayer, "luminance-gain-control-version" );
  xmlParam::Register_init<1> (outerPlexiformLayer,"luminance-gain-control-version", &Retina::setFather, *this );

  // Gain control scheme
  add_child ( contrastGainControl, "" );
  xmlParam::Register<1, FastGainControl >( contrastGainControl , "contrast-gain-control" );
  xmlParam::Register_init<1>(contrastGainControl, "contrast-gain-control", &Retina::setFather, *this );
  xmlParam::Register<1, FastGainControl_custom >( contrastGainControl , "contrast-gain-control-custom" );
  xmlParam::Register_init<1>(contrastGainControl, "contrast-gain-control-custom", &Retina::setFather, *this );

  // Ganglion layers
  add_child ( ganglionLayers, "" );
  xmlParam::Register<2, GanglionLayer> (ganglionLayers, xmlParam::names ("" , "ganglion-layer") );
  xmlParam::Register_init<2>(ganglionLayers, "ganglion-layer", &Retina::setFather, *this );
}


Retina::~Retina()
{
  delete radScheme;
  delete outerPlexiformLayer;
  delete contrastGainControl;
}


// Return minimum size that an input image should have, given the blur parameters
// required for the retina in its successive layers, for the filtering to be exact
// at least in central pixel of the retina.
int Retina::get_minimumSize()
{
  double sumSigmaSquare=0.0;
  for(uint chann=0;chann<ganglionLayers.size();++chann)
    sumSigmaSquare=max(sumSigmaSquare,
                       pow(ganglionLayers[chann]->sigmaPool,2.0));
  if(outerPlexiformLayer)
    sumSigmaSquare+=(pow(outerPlexiformLayer->sigmaCenter,2.0)
                    +pow(outerPlexiformLayer->sigmaSurround,2.0));
  if(contrastGainControl)
    sumSigmaSquare+=pow(contrastGainControl->sigmaSurround,2.0);

  //taking 3.5*sigma as the 'zero' boundary for Gaussian kernels
  return (int)ceil(7*sqrt(sumSigmaSquare)*pixels_per_degree);
}


void Retina::setFather(RetinaStage *retStage)
{
  retStage->fatherRetina=this;
}


// Helper function: 
// Adjust ONE size of ONE spiking channel as a function of input image dimension.
// output code: 0=no prob, 1=auto, 2=too big
int Retina::adjustChannelSize(ret_SpikingChannel* chan, int imSize, const char dim)
{
  if(!chan->sampScheme)
  {
    cerr<<"No proper sampling array ('circular-array' or 'square-array') "
          "has been found in this spiking channel. The retina cannot "
          "initialize its elements."<<endl;
    return(0);
  }
  double *chanSize;
  if(dim=='X') // X
    chanSize=chan->sampScheme->sizeX;
  else // Y
    chanSize=chan->sampScheme->sizeY;
  
  int ret=0; // "do not touch"
  if( round(*chanSize*pixels_per_degree) > imSize )
    ret=2;   // "need to reduce"
  else if( *chanSize == 0 )
    ret=1;   // "need to set size automatically"
  
  if( ret )  
    *chanSize = (double)imSize/pixels_per_degree; // update channel size
  return ret ;
}


// Adjusting sizeX and sizeY, that is, the size of all maps that are going to be 
// allocated by sub-objects of the retina, as a function of the sizes of the input
// images that are going to be fed to the retina...
void Retina::adjustSizes(int imX, int imY)
{
  if(adjusted)
  {
    cerr<<"ERROR in Retina::adjustSizes(): Trying to adjust sizes for the second time!"<<endl;
    exit(0);
  }
  
  //(1) Determine a 'confidence' zone in the image, without edge effects in the filtering
  int minSizeForBlurs=get_minimumSize();
  if(imX<minSizeForBlurs || imY<minSizeForBlurs)
    cerr<<"WARNING in Retina::adjustSizes(): Given the spatial filtering parameters "
      "(\'xx-sigma-xx\') required for the retina in its successive layers, the input "
      "image should be at least of size "<<minSizeForBlurs<<", to avoid edge effects "
      "in the filtering at central pixel (at least)."<<endl;
  int imX_conf=max(1,imX-minSizeForBlurs+1);
  int imY_conf=max(1,imY-minSizeForBlurs+1);
  
  
  //(2) FIX sizeX and sizeY from the spiking channels' size specifications
  int nbSC=0;

  for(uint chann=0;chann<ganglionLayers.size();++chann)
  {
    if(ganglionLayers[chann]->spikingChannel)
    {
      nbSC++;

      // Adjust channel sizes. Because circular arrays have only *one* diameter,
      // it is important to first adjust to the smallest dimension :
      int retX, retY;
      if(imX_conf<imY_conf)
      {
        retX=adjustChannelSize( ganglionLayers[chann]->spikingChannel, imX_conf, 'X');
        retY=adjustChannelSize( ganglionLayers[chann]->spikingChannel, imY_conf, 'Y');
      }
      else
      {
        retY=adjustChannelSize( ganglionLayers[chann]->spikingChannel, imY_conf, 'Y');
        retX=adjustChannelSize( ganglionLayers[chann]->spikingChannel, imX_conf, 'X');
      }
      
      // Update corresponding retinal sizes
      sizeX=max(sizeX,
               (int)round(*(ganglionLayers[chann]->spikingChannel->sampScheme->sizeX)*pixels_per_degree));
      sizeY=max(sizeY,
               (int)round(*(ganglionLayers[chann]->spikingChannel->sampScheme->sizeY)*pixels_per_degree));  
  
      //Always keep the customer informed.
      int status=max(retX,retY);
      if ( status==1 )
        cout<<"From Retina::adjustSizes(): A spiking array <"
        << xmlParam::last_xmlName( *(ganglionLayers[chann]->spikingChannel->sampScheme) ) // careful using this function :p
        <<"> is being automatically set to fit the size of input image (minus a border for edge effects)."<<endl;
      else if ( status==2 )
        cout<<"WARNING from Retina::adjustSizes(): A spiking array <"
        << xmlParam::last_xmlName( *(ganglionLayers[chann]->spikingChannel->sampScheme) ) // careful using this function :p
        <<"> is being automatically REDUCED to fit the size of input image (minus a border for edge effects)."<<endl;
    }
  }

  //(2bis) If the retina has no spiking channel, then sizeX and sizeY are just defined by the input image!
  if(!nbSC)
    sizeX=imX_conf, sizeY=imY_conf;

  //(3) add back the 'border for blur' to the retinal maps, keeping the constraint that sizeX<=imX
  sizeX=min(sizeX+minSizeForBlurs-1, imX);
  sizeY=min(sizeY+minSizeForBlurs-1, imY);
  adjusted=true;
}



void Retina::allocateValues(bool verbose, double initValue)
{
  if(!adjusted)
  {
    cerr<<"ERROR in Retina::allocateValues(): use adjustSizes(imX, imY) first!"<<endl;
    exit(0);
  }
  
  double initVal = (initValue==HUGE_VAL)?
                   input_luminosity_range / 2.0 :
                   initValue ;

  if(verbose)
  {
    cout<<endl<<"The retina is currently allocating all its elements!"<<endl;
    cout<<"Size of its maps was fixed at: ("<<sizeX<<","<<sizeY<<")."<<endl;
    cout<<"Initialization of the retina (OPL only) by a uniform screen"
      " of luminance "<<initVal<<"."<<endl;

    cout<<endl<<"(1) OUTER PLEXIFORM LAYER:"<<endl;
  }
  
  if(outerPlexiformLayer)
    outerPlexiformLayer->allocateValues(verbose, initVal );
  else
  {
    cerr<<"ERROR: Your retina does not have an Outer Plexiform Layer, "
      "which is its minimal element!"<<endl;
    exit(0);
  }

  if(verbose)
    cout<<endl<<"(2) CONTRAST GAIN CONTROL MECHANISM:"<<endl;   
  
  if(contrastGainControl)
    contrastGainControl->allocateValues(verbose);
  else if(verbose)
      cout<<"No Contrast Gain Control mechanism was defined."<<endl;

  if(verbose)
    cout<<endl<<"(3) SPIKING LAYERS OF GANGLION CELLS:"<<endl;
  
  if(ganglionLayers.size())
  {
    int nbUnits=0, nbSpkCh=0, nbCh=ganglionLayers.size();
    for(int chann=0;chann<nbCh;++chann)
    {
      ganglionLayers[chann]->allocateValues(verbose);
      if(ganglionLayers[chann]->spikingChannel)
      {
        nbSpkCh++;
        nbUnits+=ganglionLayers[chann]->spikingChannel->units.size();
      }
    }
    if(verbose)
      cout<<nbCh<<" layers of ganglion cells were defined, amongst which "
        <<nbSpkCh<<" are spiking channels, for a total of "<<nbUnits
        <<" spiking ganglion cells."<<endl;
  }
  else if(verbose)
      cout<<"No layer of ganglion cells were defined. Retinal output will be "
        "an analog spatio-temporal map corresponding to the potential of a "
        "layer of bipolar cells."<<endl;

  done=true;
}

void Retina::feedInput(const CImg<double>& image)
{
  if(!done)
  {
    adjustSizes(image.width(),image.height()); 
    allocateValues();
  }

  //if the input image should be bigger than necessary, reduce it:
  if((image.width()==sizeX)&(image.height()==sizeY))
  {
    input.const_targets=&image;
    input.targets=0;
  }
  else
  {
    stockInput = image.get_crop((image.width()-sizeX)/2,
                                (image.height()-sizeY)/2,
                                (image.width()-sizeX)/2+sizeX-1,
                                (image.height()-sizeY)/2+sizeY-1);
    input.targets=&stockInput;
  }

  outerPlexiformLayer->feedInput(input.read());

  if(contrastGainControl)
    contrastGainControl->feedInput(outerPlexiformLayer->outputSignal->read());

  for(uint chann=0;chann<ganglionLayers.size();++chann)
    if(contrastGainControl)
      ganglionLayers[chann]->feedInput(contrastGainControl->bipolarSignal->read());
    else
      ganglionLayers[chann]->feedInput(outerPlexiformLayer->outputSignal->read());
}

void Retina::tempStep()
{
  if(!done)
    cerr<<"ERROR in Retina::tempStep(): feedInput first!"<<endl;
  outerPlexiformLayer->tempStep();
  if(contrastGainControl)
    contrastGainControl->tempStep();
  for(uint chann=0;chann<ganglionLayers.size();++chann)
    ganglionLayers[chann]->tempStep();
}



//***************************************************************************
//****************** Base class RetinaStage *********************************

void RetinaStage::checkFather()
{
  if( !fatherRetina )
  {
    cerr<<"ERROR in this retinal stage: Its fatherRetina has not been set."<<endl;
    exit(0);
  }
}


//****************************************************************************
//******** Functions for OPL classes LinearOPL and GainControlOPL   **********



//******************LinearOPL ******************:

LinearOPL::LinearOPL()
{
  sigmaCenter=0.0;
  tauCenter=0.01;
  nCenter=0;
  sigmaSurround=1.0;
  tauSurround=0.01;
  wSurround=0.0;
  leakyHeatEq=0;
  ampOPL=1.0; relative_ampOPL=1.0;

  excitCells = new RetinaMapFilter();
  inhibCells = new RetinaMapFilter();
  undershoot = 0 ;
  outputSignal=0, receptorSignal=0, horizontalSignal=0;
}

void LinearOPL::xmlize()
{
  add_param(sigmaCenter,"center-sigma__deg");
  add_param(tauCenter,"center-tau__sec");
  add_param(nCenter,"center-n__uint");
  add_param(sigmaSurround,"surround-sigma__deg");
  add_param(tauSurround,"surround-tau__sec");
  add_param(wSurround,"opl-relative-weight");
  add_param(leakyHeatEq,"leaky-heat-equation");
  add_param(relative_ampOPL,"opl-amplification");

  //The undershoot is now added as a subobject:
  add_child( undershoot , "" ) ;
  xmlParam::Register<1,LinearOPL::Undershoot> ( undershoot , "undershoot" ) ;
}

void LinearOPL::Undershoot::xmlize()
{
  add_param(wAdap,"relative-weight");
  add_param(tauAdap,"tau__sec");
}


LinearOPL::~LinearOPL()
{
  delete excitCells;
  delete inhibCells;
  delete outputSignal;
  delete undershoot ;
}


void LinearOPL::allocateValues( bool verbose, double initValue)
{
  if(verbose)
  {
    cout<<"Allocating values for the LINEAR version LinearOPL."<<endl;
    if(leakyHeatEq)
      cout<<"Low-pass filter for each layer of cells is a non-separable "
        "leaky heat equation."<<endl;
    else
      cout<<"Low-pass filter for each layer of cells is a separable "
        "spatial_gaussian / temporal_negative_exponential."<<endl;
    if(undershoot)
      cout<<"A linear undershoot (transient) has been added."<<endl;
  }

  checkFather();
  int sx=fatherRetina->sizeX;
  int sy=fatherRetina->sizeY;
  double st=fatherRetina->step;
  double ppd=fatherRetina->pixels_per_degree;
  ret_LogPolarScheme *rs=fatherRetina->radScheme;

  //relative_ampOPL must have values of order [1-10] for the OPL output to have typical values of order 1.
  if ( (relative_ampOPL<1) || (relative_ampOPL>20) )
    cerr<<"WARNING from the LinearOPL object: The parameter LinearOPL::opl-amplification "
      "does not seem well suited. It is a RELATIVE factor with respect to input "
      "luminosity on the retina. It should always be set between 1 and 20 for the "
      "output range of the OPL to have values of order 1. CHANGE IT in your retina "
      "definition file!"<<endl;
  ampOPL = relative_ampOPL / fatherRetina->input_luminosity_range ;
  
  excitCells->set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
  if(leakyHeatEq)
  {
    excitCells -> leakyHeatEquation( tauCenter , sigmaCenter );
    if ( nCenter != 0 )
      cerr<<"WARNING from the LinearOPL object: You have chosen a 'leaky heat equation' "
        "filter, so parameter center-n__uint is inefficient ! It will be considered "
        "as having value zero (simple exponential filter)."<<endl;
  }
  else
  {
    excitCells -> ExpCascade(tauCenter , nCenter);
    excitCells -> set_sigmaPool(sigmaCenter);
  }
  (*excitCells) *= ampOPL;

  //We set the initial input of the filters at half the retinal input_luminosity_range
  excitCells -> set_initial_input_value ( initValue );
  excitCells -> allocateValues();
  receptorSignal = excitCells;


  if(undershoot)
  {
    BaseRecFilter adapTr( st );
    adapTr.Exp ( undershoot->tauAdap ).warningVerbose=false;
    adapTr*= undershoot->wAdap ;

    (*undershoot) .set_sizeX(sx).set_sizeY(sy).set_step(st) ;
    (*undershoot) -= adapTr ;

    undershoot -> set_initial_input_value ( excitCells->read()(sx/2,sy/2) );
    undershoot -> allocateValues();
    receptorSignal = undershoot ;
  }


  if(wSurround)
  {
    inhibCells -> set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
    if(leakyHeatEq)
      inhibCells -> leakyHeatEquation( tauSurround , sigmaSurround );
    else
    {
      inhibCells -> Exp(tauSurround);
      inhibCells -> set_sigmaPool(sigmaSurround);
    }
    (*inhibCells) *= wSurround;

    //We set the initial input of the filters at half the retinal input_luminosity_range
    //NOTA : receptorSignal = undershoot if it is present, and = excitCells otherwise.
    inhibCells -> set_initial_input_value ( receptorSignal->read()(sx/2,sy/2) );
    inhibCells -> allocateValues();
    horizontalSignal = inhibCells;

    stock_output = CImg<double>(sx,sy,1,1,0.0);
    outputSignal = new CImgReader;
    outputSignal -> targets= &stock_output;
  }
  else
    outputSignal = receptorSignal ;
}


void LinearOPL::feedInput(const CImg<double>& image)
{
  excitCells -> feedInput( image );
  if( undershoot )
    undershoot -> feedInput ( excitCells -> read() );
  //NOTA : receptorSignal = undershoot if it is present, and = excitCells otherwise.
  if( wSurround )
    inhibCells -> feedInput( receptorSignal -> read() );
}

void LinearOPL::tempStep()
{
  excitCells -> tempStep();
  if ( undershoot )
    undershoot -> tempStep();
  if(wSurround)
  {
    inhibCells -> tempStep();
    stock_output = receptorSignal -> read() - inhibCells -> read();
  }
}



//****************** GainControlOPL ******************:

GainControlOPL::GainControlOPL()
{
  ampPhoto=1.0;
  sigmaCenter=0.0;
  tauPhoto=0.01;
  sigmaSurround=1.0;
  tauSurround=0.01;
  wSurround=0.0;
  gLeak=10.0;
  ampFeedback=200.0;
  nernstFeedback=0.0;

  phototransFilter=RetinaMapFilter();
  excitCells=CondCellMap();
  controlCond=RetinaMapFilter();
  outputSignal=0, receptorSignal=0, horizontalSignal=0;
}

void GainControlOPL::xmlize()
{
  add_param(ampPhoto,"input-amplification-phototransduction");
  add_param(sigmaCenter,"sigma-receptors__deg");
  add_param(tauPhoto,"tau-phototransduction__sec");
  add_param(sigmaSurround,"sigma-horizontal-cells__deg");
  add_param(tauSurround,"additional-tau-horizontal-cells__sec");
  add_param(wSurround,"relative-weight-surround-center");
  add_param(gLeak,"inert-leak-in-receptors");
  add_param(ampFeedback,"horizontal-feedback-amplification");
  add_param(nernstFeedback,"horizontal-feedback-nernst-potential");
}


GainControlOPL::~GainControlOPL()
{
  if((wSurround)&&(outputSignal)) delete outputSignal;
}



void GainControlOPL::allocateValues( bool verbose, double initValue)
{
  if(verbose)
  {
    cout<<"Allocating values for a CONDUCTANCE-BASED OPL (luminance gain-control)."<<endl;
    cout<<"Very experimental element. the initValue method does not work (amongst other things!)"<<endl;
  }

  checkFather();
  int sx=fatherRetina->sizeX;
  int sy=fatherRetina->sizeY;
  double st=fatherRetina->step;
  double ppd=fatherRetina->pixels_per_degree;
  ret_LogPolarScheme *rs=fatherRetina->radScheme;
  
  phototransFilter.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
  phototransFilter.Exp(tauPhoto);
  phototransFilter.set_sigmaPool(sigmaCenter); 
  phototransFilter*=ampPhoto;

  excitCells.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
  excitCells.set_g_leak(gLeak);
  int index=excitCells.new_ionicChannel(nernstFeedback);
  excitCells.inputSynapse(index).linearAmplification(ampFeedback);
  excitCells.allocateValues();
  receptorSignal=&excitCells;

  controlCond.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
  controlCond.Exp(tauSurround); controlCond.set_sigmaPool(sigmaSurround);
  controlCond.inputSynapse().rectification(0.0);
  controlCond.allocateValues();
  horizontalSignal=&controlCond;

  if(wSurround)
  {
    stock_output=CImg<double>(sx,sy);
    outputSignal=new CImgReader;
    outputSignal->targets=&stock_output;
  }
  else
    outputSignal=&excitCells;
}

void GainControlOPL::feedInput(const CImg<double>& image)
{
  excitCells.feedCurrent(image);//the input current corresponds to port 0...
  excitCells.feedCond(controlCond.read(),1,true);//the feedback conductance corresponds to port 1...
  controlCond.feedInput(excitCells.read());
}

void GainControlOPL::tempStep()
{
  excitCells.tempStep();
  controlCond.tempStep();
  if(wSurround)
    stock_output=excitCells.read()-wSurround*controlCond.read();
}





//****************** FastGainControl:  ******************
//*******************************************************



FastGainControl::FastGainControl() :  	vThresh(0.0), Einh(0.0), exponent(2.0), symmetrize(1),
    inhibCells(0), wTransient(0), tauTransient(0.03)
{
  ampInputCurrent=100.0;
  sigmaSurround=1.0;
  tauSurround=0.01;
  gLeak=10.0;
  ampFeedback=100.0;

  excitCells=CondCellMap();
  controlCond=RetinaMapFilter();
  bipolarSignal=0, adaptationSignal=0;
}


void FastGainControl::xmlize()
{
  add_param(ampInputCurrent,"opl-amplification__Hz");
  add_param(gLeak,"bipolar-inert-leaks__Hz");

  add_param(sigmaSurround,"adaptation-sigma__deg");
  add_param(tauSurround,"adaptation-tau__sec");
  add_param(ampFeedback,"adaptation-feedback-amplification__Hz");
}


void FastGainControl::allocateValues( bool verbose)
{

  if(verbose)
    cout<<"Allocating values for a conductance-based contrast-gain-control."<<endl;

  checkFather();
  int sx=fatherRetina->sizeX;
  int sy=fatherRetina->sizeY;
  double st=fatherRetina->step;
  double ppd=fatherRetina->pixels_per_degree;
  ret_LogPolarScheme *rs=fatherRetina->radScheme;
  
  // *********** (1) LAYER OF BIPOLAR CELLS:

  excitCells.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
  excitCells.set_g_leak(gLeak);

  // (a) Input current in port number 0.
  excitCells.inputSynapse().linearAmplification(ampInputCurrent);

  // (b) Adapting inhibitory (or leak, if Einh=0) conductance in port number 1 -- providing CGC.
  // Einh=0 should be the law if an ON/OFF symmetry is present.
  excitCells.new_ionicChannel( Einh );

  // (c) Optional linear transient, if wTransient!=0 :
  if(wTransient)
    excitCells.new_currentInput();

  // (d) Allocating the retinal map. Pointing the reader on the map.
  excitCells.allocateValues();
  bipolarSignal=&excitCells;



  // *************** (2) ADAPTING CONDUCTANCES IN THE MEMBRANE

  controlCond.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);

  // (a) Synaptic transmission ==> Parameters for this stage can only be fixed through the 'CUSTOM' version.

  controlCond.inputSynapse().rectification( vThresh , ampFeedback );
  controlCond.inputSynapse().set_power_exp ( exponent ) ;
  if (symmetrize)
  {
    controlCond.new_currentInput();
    controlCond.inputSynapse(1).rectification( -vThresh , -ampFeedback ); //The simultaneous 'OFF'-bipolar-control
    controlCond.inputSynapse(1).set_power_exp ( exponent ) ;
  }

  // (b) Spatio-temporal averaging properties

  controlCond.Exp(tauSurround);
  controlCond.set_sigmaPool( sigmaSurround );

  // (c) Allocating the retinal map. Pointing the reader on the map.
  controlCond.allocateValues();
  adaptationSignal=&controlCond;


  // ***************** (3) OPTIONAL ADDTIONAL LINEAR HIGH-PASS
  if(wTransient)
  {
    inhibCells = new RetinaMapFilter() ;
    inhibCells -> set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
    inhibCells -> Exp(tauTransient) ;
    (*inhibCells) *= wTransient ;
    inhibCells -> allocateValues() ;
  }

}

void FastGainControl::feedInput(const CImg<double>& image)
{
  // *******  (1) Inputs to bipolar cells

  // input current in port 0:
  excitCells.feedCurrent(image);
  //feedback conductance corresponds to port 1:
  excitCells.feedCond(controlCond.read(),1,true);
  //Optionally, linear feedback corresponds to port 2:
  if(wTransient)
    excitCells.feedCurrent(inhibCells->read(),2);

  // *******  (2) Inputs to adapting conductance:
  controlCond.feedInput(excitCells.read(),0);
  if(symmetrize)
    controlCond.feedInput(excitCells.read(),1); //Input from 'ON' and 'OFF' bipolar cells.

  // *******  (3) OPTIONALLY, Input to the supplementary transient (short-range amacrine cells)
  if (inhibCells)
    inhibCells->feedInput( excitCells.read() );
}

void FastGainControl::tempStep()
{
  excitCells.tempStep() ;
  controlCond.tempStep() ;
  if(inhibCells)
    inhibCells->tempStep() ;
}


//****************** FastGainControl_custom ******************:


void FastGainControl_custom::xmlize()
{
  FastGainControl::xmlize();
  add_param(exponent,"adaptation-powerlaw-exponent");
  add_param(Einh,"adaptation-E-inh");
  add_param(symmetrize,"adaptation-ONOFF-symmetry");
  add_param(vThresh,"bipolar-to-adaptation-threshold");

  add_param( wTransient , "transient-relative-weight");
  add_param( tauTransient , "transient-tau__sec");
}



//****************  GanglionLayer  *****************************


GanglionLayer::GanglionLayer()
{
  sign=1.0;
  vThresh=0.0;
  valThresh=70.0;
  ampInputCurrent=100.0;
  tauTransient=0.04;
  wTransient=0.75;
  sigmaPool=0.0;

  spikingChannel=0;
}


void GanglionLayer::xmlize()
{
  add_param(sign,"sign");
  add_param(tauTransient,"transient-tau__sec");
  add_param(wTransient,"transient-relative-weight");
  add_param(vThresh,"bipolar-linear-threshold");
  add_param(valThresh,"value-at-linear-threshold__Hz");
  add_param(ampInputCurrent,"bipolar-amplification__Hz");
  add_param(sigmaPool,"sigma-pool__deg");

  // auto-registration of the pointer:
  add_child(spikingChannel,"");
  xmlParam::Register<1,ret_SpikingChannel>(spikingChannel,"spiking-channel");
  xmlParam::Register<1,ret_SpikingChannel_adap>(spikingChannel,"spiking-channel-adap");
}


// xmlParam initialization function (right after full XML load)
void GanglionLayer::xmlinit_after_children()
{
  if(spikingChannel)
  {
    spikingChannel->radScheme=fatherRetina->radScheme;
    if(spikingChannel->sampScheme)
      spikingChannel->sampScheme->father=spikingChannel;
  }
}


GanglionLayer::~GanglionLayer()
{
  delete spikingChannel;
}

void GanglionLayer::allocateValues( bool verbose)
{
  sign=(sign<0)? -1.0:1.0;
  
  checkFather();
  int sx=fatherRetina->sizeX;
  int sy=fatherRetina->sizeY;
  double st=fatherRetina->step;
  double ppd=fatherRetina->pixels_per_degree;
  ret_LogPolarScheme *rs=fatherRetina->radScheme;
  
  inputCurrent.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);
  supplementaryTransient.set_radScheme(rs).set_sizeX(sx).set_sizeY(sy).set_ratiopix(ppd).set_step(st);

  BaseRecFilter amacrineCuttingInGanglionar(st);
  amacrineCuttingInGanglionar.Exp(tauTransient).warningVerbose=false;
  amacrineCuttingInGanglionar*=wTransient;

  //This is what we had before!!: transient AFTER the synaptic rectif/pooling
  //if(wTransient) inputCurrent-=amacrineCuttingInGanglionar;

  //This is what we propose now: transient BEFORE the synaptic rectif/pooling
  if(wTransient)
    supplementaryTransient-=amacrineCuttingInGanglionar;
  supplementaryTransient.allocateValues();

  //Check the corresponding change in functions feedInput() and tempStep() !

  inputCurrent.inputSynapse()
              .rectification(sign*vThresh,sign*ampInputCurrent,valThresh);
  inputCurrent.set_sigmaPool(sigmaPool);
  inputCurrent.allocateValues();

  if(spikingChannel)
  {
    spikingChannel->set_step(st).set_ratiopix(ppd).set_radScheme(rs);
    spikingChannel->allocateValues();
  }

}

void GanglionLayer::feedInput(const CImg<double>& image)
{
  //This is what we had before!!: transient AFTER the synaptic rectif/pooling
  //inputCurrent.feedInput(image);

  //This is what we propose now: transient BEFORE the synaptic rectif/pooling
  if(wTransient)
  {
    supplementaryTransient.feedInput( image );
    inputCurrent.feedInput( supplementaryTransient.read() );
  }
  else
    inputCurrent.feedInput( image );
  if(spikingChannel)
    spikingChannel->feedInput(inputCurrent.read());
}

void GanglionLayer::tempStep()
{
  if(wTransient)
    supplementaryTransient.tempStep();
  inputCurrent.tempStep();
}


