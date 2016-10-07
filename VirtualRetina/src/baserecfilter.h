#ifndef BASERECFILTER_H
#define BASERECFILTER_H

#include "retinatools/misc_tools.h"
#include "retinatools/cimg_tools.h"

//BaseRecFilter is the basis for temporal recursive filtering on any type of object (simple numbers or images)
// basic equation is :
// y(k) =( -( a(1).y(k-1)+..+a(N).y(k-N) )  + b(0).x(k)+...+b(M-1).x(k-M+1) )/a(0)


class BaseRecFilter
{

protected:

  int M;
  int N;
  double* a;
  double* b;
  double step;                   //temporal step of the filter, if it is temporal. default value is step = 1 (no dimension).
  bool done;                     //an indicator that will help in knowing when to initialize the filter...

  virtual void myEmpty(void);
  virtual void reInit(void);
  virtual void checkStatus(void);

public:

  bool warningVerbose;

  //Constructors, Destructors, Copy...
  BaseRecFilter(double stepp=1);
  BaseRecFilter(const BaseRecFilter& yo);
  virtual ~BaseRecFilter(void);
  BaseRecFilter& operator=(const BaseRecFilter& yo);

  //Miscellaneous functions:
  BaseRecFilter & set_step(double stepp);
  BaseRecFilter & noWarning(void) ;
  
  
  //INITIALIZATION FUNCTIONS::

  //decreasing exponential of time constant tau, and of gain 1.
  BaseRecFilter& Exp(double tau);
  //cascade of n similar exponential filters, whose result impulse response peaks at time tau, and has a gain of 1.
  BaseRecFilter& ExpCascade(double tau,int n);
  //Non-recursive implementation -- to check possible stability issues with the recursive implementation
  BaseRecFilter& ExpCascade_nonRec(double tau,int n, int caracWindow=10);

  //Initialization functions through linear operations between filters:

  friend BaseRecFilter operator+(const BaseRecFilter &first, const BaseRecFilter &second);
  BaseRecFilter& operator+=(const BaseRecFilter &secondFilt);

  friend BaseRecFilter operator-(const BaseRecFilter &first, const BaseRecFilter &second);
  BaseRecFilter& operator-=(const BaseRecFilter &secondFilt);

  friend BaseRecFilter operator*(double lambda, const BaseRecFilter &filt);
  friend BaseRecFilter operator*(const BaseRecFilter &filt, double lambda);
  BaseRecFilter& operator*=(double lambda);

  friend BaseRecFilter operator/(const BaseRecFilter &filt, double lambda);
  BaseRecFilter& operator/=(double lambda);

  BaseRecFilter& Cascade(const BaseRecFilter &second);
  
  
  
  //DRIVING FUNCTIONS, to be precised later on:
  virtual void feedInput(void){}
  virtual void tempStep(void){}
  
  
  //CHECKING FUNCTIONS :
  double get_gain(void) const;
  void print(void) const;

  void testImpulseResponse( double tLength, const double* color=blue , std::string name=" " ) const;
  void testIntegratedResponse( double tLength, const double* color=blue , std::string name=" " ) const;

private:
	CImg<double>  test_base ( bool is_impulse , double tLength, const double* color) const ;
};

#endif

















