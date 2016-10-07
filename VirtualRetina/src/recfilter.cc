
#include "recfilter.h"


void RecFilter::allocateValues(void)
{
  if(done) cerr<<"ERROR in RecFilter::allocateValues() : can only be called once"<<endl;
  
  last_inputs = new double*[M];
  last_values = new double*[N+1];

  last_inputs[0]=new double (0.0);
  for (int i=1;i<M;i++)
    last_inputs[i]=new double ( initial_input_value );
  for (int j=0;j<N+1;j++)
    last_values[j]=new double ( initial_input_value * get_gain() );
    
  done=true;      //no more initialization function possible.
}


RecFilter::RecFilter(double stepp):  BaseRecFilter(stepp)
{
  last_inputs=0; last_values=0;
  initial_input_value=0.0;
}

RecFilter::RecFilter(const RecFilter& yo) : BaseRecFilter(yo)
{
  last_inputs=0; last_values=0;
  initial_input_value=0.0;
}

RecFilter::RecFilter(const BaseRecFilter& yo) : BaseRecFilter(yo)
{
  last_inputs=0; last_values=0;
  initial_input_value=0.0;
}

RecFilter::~RecFilter(void)
{
  if(last_inputs) delete[] last_inputs;
  if(last_values) delete[] last_values;
}

RecFilter& RecFilter::operator=(const BaseRecFilter& yo)
{
  BaseRecFilter::operator=(yo);
  return (*this);
}

RecFilter& RecFilter::set_initial_input_value (double val)
{
  initial_input_value=val;
  return *this;
}



void RecFilter::feedInput( double new_input )
{
  if(!done)  
  	allocateValues();
  *(last_inputs[0]) += new_input;
}



void RecFilter::tempStep(void)
{
  if(!done)
  	allocateValues(); 

 //Rotation on addresses of the last_values, to kick out the oldest VALUE.
  double* fakepoint=last_values[N];
  for(int i=1;i<N+1;++i)
    last_values[N+1-i]=last_values[N-i];
  last_values[0]=fakepoint;

  //Calculating new value of filter recursively:
    
   *(last_values[0]) = b[0] * (*(last_inputs[0])) ;
  for(int j=1;j<M;j++)
   *(last_values[0]) += ( b[j] * (*(last_inputs[j])) );
  for(int k=1;k<N+1;k++)
   *(last_values[0]) -= ( a[k] * (*(last_values[k])) );
  if(a[0]!=1)
    *(last_values[0]) /=a[0];
    
    //Rotation on addresses of the last inputs
    fakepoint=last_inputs[M-1];
      for(int i=1;i<M;++i)
        last_inputs[M-i]=last_inputs[M-i-1];
      last_inputs[0]=fakepoint;
      
    *(last_inputs[0]) = 0.0;
}


double RecFilter::read(int n)
{
if( n>N )
		cerr<<"Error in RecFilter::read(int n): Index n is bigger than the filter's recursive order!"<<endl;
return *(last_values[n]) ;
}

