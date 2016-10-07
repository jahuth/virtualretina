
#include "baserecfilter.h"

// Cool little trick to be able to graphically represent the filter's impulse response ^^ :
#include "recfilter.h"


void BaseRecFilter::reInit(void)
{
  a=new double[1]; a[0]=1; N=0;
  b=new double[1]; b[0]=1; M=1;
}

void BaseRecFilter::myEmpty(void)
{
	//cerr<<"EMPTYING BASERECFILTER POINTERS"<<endl;
  if(a) delete[] a;
  if(b) delete[] b;
}

void BaseRecFilter::checkStatus(void)
{
  if(done)
  {
    cerr<<"Error in BaseRecFilter: You're changing parameter of a filter after it has started to function."<<endl;
    exit(0);
  }
}

BaseRecFilter::BaseRecFilter(double stepp)    //initialized to "no filtering".
{
  reInit();
	//cerr<<"CREATING BASERECFILTER"<<endl;
  step=stepp;
  done=false;
  warningVerbose=true;
}



BaseRecFilter::BaseRecFilter(const BaseRecFilter& yo)
{
  M=yo.M;
  N=yo.N;
	//cerr<<"CREATING BASERECFILTER BY COPY "<<M<<" "<<N<<endl;
  b=new double[M];
  for(int i=0;i<M;++i) 
	  b[i]=yo.b[i];
  a=new double[N+1];
  for(int i=0;i<N+1;++i)
	  a[i]=yo.a[i];
  step=yo.step;
  done=false; 
  warningVerbose=yo.warningVerbose;
}

BaseRecFilter::~BaseRecFilter(void)
{
  myEmpty();
}

BaseRecFilter& BaseRecFilter::operator=(const BaseRecFilter& yo)
{
	//cerr<<"COPYING BASERECFILTER BY OPERATOR ="<<endl;
	//avoiding self-assignment problems
	
  checkStatus();
  M=yo.M;
  N=yo.N;
  double* bTemp=new double[M];
  for(int i=0;i<M;++i) 
  		bTemp[i]=yo.b[i];
  double* aTemp=new double[N+1];
  for(int i=0;i<N+1;++i)
  		aTemp[i]=yo.a[i];
  
  step=yo.step;
  warningVerbose=yo.warningVerbose;
  
  myEmpty();
  a=aTemp, b=bTemp ;
  return (*this);
}


double BaseRecFilter::get_gain(void) const
{
  double sumA=0, sumB=0;
  for(int i=0;i<M;++i) sumB+=b[i];
  for(int i=0;i<N+1;++i) sumA+=a[i];
  return sumB/sumA;
}

void BaseRecFilter::print(void) const
{
  cout<<"BASE RECFILTER:";
  cout<<endl<<"step: "<<step;
  cout<<endl<<" b: ";
  for(int i=0;i<M;++i) cout<<b[i]<<" ";
  cout<<endl<<" a: ";
  for(int j=0;j<=N;++j) cout<<a[j]<<" ";
  cout<<endl;
}

BaseRecFilter& BaseRecFilter::set_step(double stepp)
{
  step=stepp;
  return (*this);
}


BaseRecFilter& BaseRecFilter::noWarning(void)
{
  warningVerbose=false ;
  return (*this);
}
  
  
BaseRecFilter& BaseRecFilter::Cascade(const BaseRecFilter &second)
{
  checkStatus();
  if(this->step!=second.step)
  {
    cerr<<"error in BaseRecFilter function Cascade: different time steps for the two filters."<<endl;
    exit(0);
  }
  double* bTemp=vecConvolve(this->b,this->M,second.b,second.M);
  double* aTemp=vecConvolve(this->a,this->N+1,second.a,second.N+1);
  myEmpty();
  b=bTemp, a=aTemp;
  M+=(second.M-1);
  N+=second.N;   //tricky!!
  return (*this);
}


BaseRecFilter& BaseRecFilter::Exp(double tau)
{
  checkStatus();
  myEmpty();
  if(tau<0)
  {
    cerr<<"error in BaseRecFilter function Exp(): you asked for an exponential filtering with negative time parameter"<<endl;
    exit(0);
  }
  if(tau>0)
  {
    M=1;
    N=1;
    a=new double[N+1];
    b=new double[M];
    a[0]=1; a[1]=-exp(-step/tau);
    b[0]=1-exp(-step/tau);
  }
  else //tau==0
    reInit();

  return (*this);
}

BaseRecFilter& BaseRecFilter::ExpCascade(double tau,int n)
{
  checkStatus();
  myEmpty();
  if(tau<0)
  {
    cerr<<"Error in BaseRecFilter function ExpCascade(): you asked for an exponential filtering with negative time parameter."<<endl;
    exit(0);
  }
  if(n<0)
  {
    cerr<<"Error in BaseRecFilter function ExpCascade(): you asked for a negative Gamma exponent."<<endl;
    exit(0);
  }
  if(tau>0)
  {
     M=1; N=n+1;
     double tauC=n? tau/n : tau;
     double c=exp(-step/tauC);
     b=new double[1]; b[0]=pow(1-c,N);
     a=new double[N+1];
     for(int i=0;i<=N;++i)
  	a[i]=pow(-c,i)*combination(N,i);
  }
  else //tau==0
    reInit();

  return (*this);
// Old, recursive implementation : Probably less stable...
//  BaseRecFilter fakeFilt(step);
//  fakeFilt.Exp(tauC);
//  reInit();
//  for(int i=0;i<=n;++i)
//    this->Cascade(fakeFilt);
//  return (*this);
}

BaseRecFilter& BaseRecFilter::ExpCascade_nonRec(double tau,int n, int caracWindow)
{
  checkStatus();
  myEmpty();
  if(tau<0)
  {
    cerr<<"Error in BaseRecFilter function ExpCascade_nonRec(): you asked for an exponential filtering with negative time parameter"<<endl;
    exit(0);
  }
  double tauC=n? tau/n : tau;
  M=(int)floor(caracWindow*tau/step);
  N=0;
  a=new double[1]; a[0]=1;
  b=new double[M];
  for(int i=0;i<M;++i)
    b[i]=pow(i*step/tauC,(double)n)*exp(-i*step/tauC);
  double gain=get_gain(); //approximately tauC*factorial(n)/step;
  (*this)/=gain;
  cerr<<"Gain of the cascade: "<<get_gain()<<endl;
  return (*this);
}


BaseRecFilter& BaseRecFilter::operator+=(const BaseRecFilter &secondFilt)
{
  *this=(*this)+secondFilt;
  return (*this);
}
BaseRecFilter& BaseRecFilter::operator-=(const BaseRecFilter &secondFilt)
{
  *this=(*this)-secondFilt;
  return (*this);
}
BaseRecFilter& BaseRecFilter::operator*=(double lambda)
{
  for(int i=0;i<M;++i) b[i]*=lambda;
  return (*this);
}
BaseRecFilter& BaseRecFilter::operator/=(double lambda)
{
  for(int i=0;i<M;++i) b[i]/=lambda;
  return (*this);
}


//The friend operators on BaseRecFilters:

BaseRecFilter operator+(const BaseRecFilter &first, const BaseRecFilter &second)
{

  if( first.warningVerbose & second.warningVerbose ) 
	  cerr<<"WARNING in BaseRecFilter.cc: Using operator+ when initializing your recursive filters. Watch out!... This can be an easy source of numerical instability, and also of unnecessary calculations. Use it only between filters of small recursive order."<<endl;
  if(first.step!=second.step)
  {
    cerr<<"Error in BaseRecFilter operator+ : different time steps for the two maps."<<endl;
    exit(0);
  }
  BaseRecFilter yo(first.step);
  yo.myEmpty();
  yo.N=first.N+second.N;
  yo.a=vecConvolve<double>(first.a,first.N+1,second.a,second.N+1);
  double* b1=vecConvolve<double>(first.b,first.M,second.a,second.N+1);
  double* b2=vecConvolve<double>(first.a,first.N+1,second.b,second.M);
  yo.b=vecSum<double>(b1,first.M+second.N,b2,first.N+second.M);
  yo.M=max(first.M+second.N,first.N+second.M);
  delete[] b1;
  delete[] b2;
  return yo;
}

BaseRecFilter operator-(const BaseRecFilter &first, const BaseRecFilter &second)
{
	BaseRecFilter yo(second);
	yo *= (-1) ;
	yo += first ;
	return yo ;
}

BaseRecFilter operator*(const BaseRecFilter &filt, double lambda)
{
	BaseRecFilter yo(filt);
	yo *= lambda ;
	return yo;
}

BaseRecFilter operator*(double lambda, const BaseRecFilter &filt)
{
	return filt*lambda;
}

BaseRecFilter operator/(const BaseRecFilter &filt, double lambda)
{
	return filt*(1/lambda);
}



void BaseRecFilter::testImpulseResponse( double tLength, const double* color, std::string name ) const
{
  test_base(true,tLength,color).display( (std::string("testImpulseResponse: ")+name).c_str() );
}


void BaseRecFilter::testIntegratedResponse( double tLength, const double* color, std::string name ) const
{
  test_base(false,tLength,color).display( (std::string("testIntegratedResponse: ")+name).c_str() );
}



CImg<double> BaseRecFilter::test_base( bool is_impulse, double tLength, const double* color ) const
{
  int nSamples = (int) ceil ( tLength / this->step ) ;
  double len = nSamples * this->step ;
  
  CImg<double> values( nSamples+1 );
  
  //Trick! We use a RecFilter, which can effectively filter a (1d) signal.
  RecFilter yo (*this) ;
  yo.set_initial_input_value(0.0) ;
  
  //Impulse:
  yo.feedInput (1.0) ;
  yo.tempStep() ;
  
  //and the remaining:
  double val = is_impulse? 0.0 : 1.0;
  for ( int i=0 ; i<=nSamples ; i++)
  {
  		values(i) = yo.read() ;
  		yo.feedInput ( val ) ;
  		yo.tempStep() ;
  }

  //Drawing...
  CImg<double> figure(700,300,1,3,255);
  figure.fill(255.0);

  CImgSignalDrawer sd; sd.style=0; sd.width=2;
  sd.axisX.begin=0; sd.axisX.end=len; //linear
  sd.draw( figure, values , color );

  return figure ;
}







