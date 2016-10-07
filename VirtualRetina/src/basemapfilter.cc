
#include "basemapfilter.h"

void BaseMapFilter::allocateValues(void)
{
  if(done) cerr<<"ERROR in BaseMapFilter::allocateValues() : can only be called once"<<endl;
  last_inputs = new CImg<double>*[M];
  last_values = new CImg<double>*[N+1];

  last_inputs[0]=new CImg<double> ( sizeX,sizeY,1,1, 0.0 );
  for (int i=1;i<M;i++)
    last_inputs[i]=new CImg<double> ( sizeX,sizeY,1,1, initial_input_value );
  for (int j=0;j<N+1;j++)
    last_values[j]=new CImg<double> ( sizeX,sizeY,1,1, initial_input_value * get_gain() );

  remember_last_input_location=last_inputs[0];
  last_const_input=0;

  if (M!=1 || !dangerous ) //cannot use last_const_input
    canUseConstInput=false;

  targets=last_values[0];  //initializing the CImgReader pointer...
  done=true;      //no more initialization function possible.
}


BaseMapFilter::BaseMapFilter(int x,int y, double stepp):  BaseRecFilter(stepp), CImgReader()
{
  set_sizeX(x),set_sizeY(y);
  ratiopix=1;
  last_inputs=0; last_values=0;
  canUseConstInput=true;
  dangerous=false;
  initial_input_value=0.0;
}

BaseMapFilter::BaseMapFilter(const BaseMapFilter& yo) : BaseRecFilter(yo), CImgReader()
{
  sizeX=yo.sizeX, sizeY=yo.sizeY;
  ratiopix=1;
  last_inputs=0; last_values=0;
  canUseConstInput=true; dangerous=false;
  initial_input_value=0.0;
}

BaseMapFilter::~BaseMapFilter(void)
{
  if(last_inputs) delete[] last_inputs;
  if(last_values) delete[] last_values;
}

BaseMapFilter& BaseMapFilter::operator=(const BaseMapFilter& yo)
{
  sizeX=yo.sizeX, sizeY=yo.sizeY;
  ratiopix=yo.ratiopix;
  BaseRecFilter::operator=(yo);
  return (*this);
}

BaseMapFilter& BaseMapFilter::copyFilter(const BaseRecFilter& yo)
{
  BaseRecFilter::operator=(yo);
  return (*this);
}

BaseMapFilter& BaseMapFilter::set_sizeX(int x)
{
  if(x<=0) 
    cerr<<"error in function BaseMapFilter::set_sizeX(): a dimension must be strictly positive."<<endl;
  else 
    sizeX=x;
  return *this;
}

BaseMapFilter& BaseMapFilter::set_sizeY(int y)
{
  if(y<=0) 
    cerr<<"error in function BaseMapFilter::set_sizeY(): a dimension must be strictly positive."<<endl;
  else 
    sizeY=y;
  return *this;
}

BaseMapFilter& BaseMapFilter::set_ratiopix(double r)
{
  if(r<=0) 
    cerr<<"error in function BaseMapFilter::set_ratiopix(): the ratio must be strictly positive."<<endl;
  else 
    ratiopix=r;
  return *this;
}

BaseMapFilter& BaseMapFilter::set_initial_input_value (double val)
{
  initial_input_value=val;
  return *this;
}


BaseMapFilter& BaseMapFilter::dangerous_feedInput()
{
  dangerous=true;
  return *this;
}


//The horrible, useless, AND possibly dangerous canUseConstInput trick: IT'S tried only if 'dangerous' was manually set to 'true'
//Why don't I remove all that sh*$& ? Guess I'm a sentimental :)

//- if ( !canUseConstInput ) , then, AT ANY TIME, last_inputs[0] !=0  and last_const_input = 0;
//				and, right after tempStep(), *last_inputs[0] is filled with value 0.0

// -if (canUseConstInput): right after tempStep(), last_inputs[0]=0 and last_const_input=0
//			after first feedInput() of the new tempStep:
//				if no synapse is used, last_const_input= &new_input, last_inputs[0]=0
//				if a synapse is used, last_const_input=0, *last_inputs[0]=syn_transmission (new_input)
//			after following feedInput()s of the new tempStep:
//				whatever happens, *last_inputs[0] stocks the input and last_const_input is back to 0.



void BaseMapFilter::feedInput(const CImg<double>& new_input, int port)
{
  if(!done)
  {
    allocateValues();
    if(canUseConstInput)
      last_inputs[0]=0;
  }

  if((sizeX!=new_input.width())||(sizeY!=new_input.height()))
  {
    cerr<<"error in BaseMapFilter function feedInput: input image does not have the right dimensions."<<endl;
    exit(0);
  }

  if(last_const_input)//we have tried to use the last_const_input pointer, but feedInput was called again, so we have to copy everything to inputImg, and bring last_const_input back to zero.
  {
    last_inputs[0]=remember_last_input_location;
    *(last_inputs[0])=*last_const_input;
    last_const_input=0;
  }

  if( (inputSynapses[port]->isUsed) || (last_inputs[0]!=0) )
  {
    if(last_inputs[0]==0)
    {
      last_inputs[0]=remember_last_input_location;
      last_inputs[0]->fill(0.0);
    }
    inputSynapses[port]->addTransmission(new_input,*(last_inputs[0]));
  }

  else //you can try to use the last_const_input pointer
    last_const_input=&new_input;
}



void BaseMapFilter::tempStep(void)
{
  if(!done) 
  {
    allocateValues();
    if(canUseConstInput)
      last_inputs[0]=0;
  }

  // At THIS point, any spatial filtering, and non-linear transform,
  // can be performed on last_inputs...
  if(last_inputs[0])
    this->spatialFiltering(last_inputs[0]);

  //Rotation on addresses of the last_values, to kick out the oldest map.
  CImg<double>* fakepoint=last_values[N];
  for(int i=1;i<N+1;++i)
    last_values[N+1-i]=last_values[N-i];
  last_values[0]=fakepoint;

  //Calculating new value of filter recursively:

  *(last_values[0]) =  last_const_input?
                       b[0]* (*last_const_input):
                       b[0]* (*(last_inputs[0]));
  for(int j=1;j<M;j++)
    *(last_values[0]) += ( b[j] * (*(last_inputs[j])) );
  for(int k=1;k<N+1;k++)
    *(last_values[0]) -= ( a[k] * (*(last_values[k])) );
  if(a[0]!=1)
    ( *(last_values[0]) )/=a[0];

  //Don't forget to update the CImgReader pointer!!
  targets=last_values[0];

  //Reinitialization procedure depends on whether or not we can try to use last_const_input
  if(canUseConstInput)
  {
    last_inputs[0]=0;
    last_const_input=0;
  }
  else
  {
    if(M>1) //we ALWAYS get there when M>1 (can never use last_const_input when M>1)
    {
      remember_last_input_location=last_inputs[M-1];
      for(int i=1;i<M;++i)
        last_inputs[M-i]=last_inputs[M-i-1];
      last_inputs[0]=remember_last_input_location;
    }
    last_inputs[0]->fill(0.0);
  }
}

