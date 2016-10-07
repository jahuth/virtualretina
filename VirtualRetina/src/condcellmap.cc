#include "condcellmap.h"

bool CondCellMap::conductanceWarning=true;

void CondCellMap::allocateValues(void)
{
  if(!done)
  {
    CImg<double> emptyImg(sizeX,sizeY,1,1,0);
    daValues=new CImg<double>;
    *daValues=emptyImg;
    preceding=new CImg<double>;
    *preceding=emptyImg;
    CImgReader::targets=daValues;

    storeInputs.assign(inputNernst.size(),sizeX,sizeY,1,1,0.0);
    attenuationMap=CImg<double>(emptyImg);
    totalCond=CImg<double>(emptyImg);

    //initializing the RadialFilter for gap junctions
    gapJunctionFilter.set_sigma0(std::sqrt(2*G*step));

    //initializing readers:
    readInputs.resize(inputNernst.size());
    for(uint c=0;c<inputNernst.size();++c)
      readInputs[c].targets=&(storeInputs[c]);

    done=true;
  }
}

CondCellMap::CondCellMap(int x, int y, double stepp) : CImgReader(), CellPortHandler(), radScheme(0)
{
  step=stepp;               //initialised at : 'no filtering'.
  sizeX=x,sizeY=y;
  ratiopix=1;
  g_leak=50;
  G=0.0;
  gapJunctionFilter.set_order(0);
  done=false;
}

CondCellMap::~CondCellMap(void)
{
  if(done)
  {
    delete daValues;
    delete preceding;
  }
}

//All the 'set-X' functions.

CondCellMap& CondCellMap::set_step(double stepp)
{
  this->step=stepp;
  return *this;
}

CondCellMap& CondCellMap::set_g_leak(double val)
{
  this->g_leak=val;
  return *this;
}

CondCellMap& CondCellMap::set_G(double val)
{
  this->G=val;
  return *this;
}

CondCellMap& CondCellMap::set_sizeX(int x)
{
  sizeX=x;
  gapJunctionFilter.set_sizeX(x);
  return *this;
}

CondCellMap& CondCellMap::set_sizeY(int y)
{
  sizeY=y;
  gapJunctionFilter.set_sizeY(y);
  return *this;
}

CondCellMap& CondCellMap::set_ratiopix(double r)
{
  ratiopix=r;
  gapJunctionFilter.set_ratiopix(r);
  return *this;
}

CondCellMap& CondCellMap::set_radScheme(RadialScheme *rad)
{
  radScheme=rad;
  gapJunctionFilter.set_radScheme(rad);
  return *this;
}


//Driving functions:
CondCellMap& CondCellMap::feedInput(const CImg<double>& input, int port, bool shutUp)
{
  if(!done) allocateValues();
  if(!shutUp)
    if(conductanceWarning)
      if(inputNernst[port] && inputSynapses[port]->isLinear)  //last condition: warning only for a conductance port.
      {
        cerr<<"WARNING: You are using CondCellMap::feedInput() (or feedCond()) into a port corresponding to an ionic channel, so you should make sure that the input values are positive. At least once in your program, the corresponding port has no synaptic rectification (the synapse is linear). So, no guarantee that the input is positive, unless YOU know it. No offense, huh... I just wanted to point out this possible source of bugs."<<endl;
        conductanceWarning=false;
      }
  if(port>(int)(inputNernst.size()-1))
    cerr<<"Warning in function CondCellMap::feedInput(). Required port index is bigger than number of ports initialized."<<endl;

  inputSynapses[port]->addTransmission(input,storeInputs[port]);
  return *this;
}

CondCellMap& CondCellMap::feedCond(const CImg<double>& input, int port, bool shutUp)
{
  if(inputNernst[port]==0) {cerr<<"Error in function feedCond(): you are using it with a CURRENT port!"<<endl; exit(0);}
  return feedInput(input,port,shutUp);
}

CondCellMap& CondCellMap::feedCurrent(const CImg<double>& input, int port)
{
  if(inputNernst[port]!=0) {cerr<<"Error in function feedCurrent(): you are using it with a CONDUCTANCE port!"<<endl; exit(0);}
  return feedInput(input,port,true);
}


void CondCellMap::tempStep(void)
{
  if(!done) allocateValues();
  //rotation on the two images (preceding and daValues):
  CImg<double>* fakepoint=preceding;
  preceding=daValues;
  daValues=fakepoint;
  CImgReader::targets=daValues;

  // g_infinity = totalCond = excit+inhib+g_leak
  totalCond.fill(g_leak);
  for(uint c=0;c<inputNernst.size();++c)
    if(inputNernst[c]!=0) totalCond+=storeInputs[c];	//summing all conductances (not currents)

  //local atenuation depends on the values values of g_infinity
  cimg_forXY(attenuationMap,x,y) attenuationMap(x,y)=exp(-step*totalCond(x,y));

  //progressively transforming 'storeInputs[0]' into the attraction potential E_infinity, but without changing its name (optimization ?!?!?)
  for(uint c=0;c<inputNernst.size();++c)
    if(inputNernst[c]!=0) storeInputs[c] *= *(inputNernst[c]);	//all conductances become currents
  for(uint c=1;c<inputNernst.size();++c)
    storeInputs[0]+=storeInputs[c];				//summing all currents
  storeInputs[0].div(totalCond);				//obtaining E_infinity

  //V(t+1)=(V(t)-Einfty)*attenuation +Einfty;
  *daValues = *preceding;
  *daValues -= storeInputs[0];
  daValues -> mul(attenuationMap);
  *daValues += storeInputs[0];

  //image blurring through gap junctions
if(G!=0)	{gapJunctionFilter.radiallyVariantBlur(*daValues);}

  for(uint c=0;c<inputNernst.size();++c)
    storeInputs[c].fill(0);
}

