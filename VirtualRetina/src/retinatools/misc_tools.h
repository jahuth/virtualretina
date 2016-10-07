#ifndef MISC_TOOLS_H
#define MISC_TOOLS_H

//The idea of this file is 'fundamental tool functions'.
//Some are necessary for the next tool files of my_tools.h.

#define EPSILON 0.00001
#define Pi 3.141592653589793
#define NO_VAL HUGE_VAL-10

#include <cmath>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <complex>

using namespace std;

const complex<double> Icplx(0,1);


//String handling functions :

template<typename TYPE>
inline std::string toString(TYPE typ, int nFormat=0)
{
  ostringstream ost;
  if(nFormat) {ost.width(nFormat); ost.fill('0');}
  ost<<typ;
  return ost.str();
}

template<typename TYPE>
inline TYPE fromString(const std::string& stringuee)
{
  TYPE typ;
  istringstream ist;
  ist.str(stringuee);
  ist>>typ;
  return typ;
}


/* Loading an 'external' command in the form (argc, argv), from
 * another character-handling support (vector<string> or a single big string) */

struct extCommand {
    int argc ;
    char** argv ;
    extCommand() : argc(0), argv(0) {}
    //no copy constructor / operator ;-)
    ~extCommand() ;
    
    void assign( const vector<string> & commands ) ;
    void assign( const string & total_command ) ;
    
    extCommand( const vector<string> & commands ) ;
    extCommand( const string & total_command ) ;
};

// Get current working directory:
std::string my_pwd(void) ;



//Comparison functions :

inline double round0(double val)
{
  if(fabs(val)<EPSILON) return(0.0);
  else return(val);
}

inline double min(const double a,const double b)
{return (a<b)? a:b;}

inline double max(const double a,const double b)
{return (a>b)? a:b;}

inline int sign(float val)
{return (val<0)? -1:1;}


//Polar functions :

inline double norm(double a,double b)
{return  sqrt(a*a+b*b);}
//for the angle, use atan2(b,a);


//Combinatorials:

inline int arrangement(int n, int k)
{
  if ((n<0)||(k<0))
      cerr<<"Error in function 'arrangement': used with negative values."<<endl;
  int res=1;
  for(int i=n;i>n-k;--i)   res*=i;
  return res;
}

inline int factorial(int n)
{return arrangement(n,n);}

inline int combination(int n, int k)
{return arrangement(n,k)/factorial(k);}


//Mathematical functions :

inline double Gaussian(double sigma,double x, double y)
{
  double xy=-((x*x)+(y*y));
  return sigma?
      exp(xy/(2*pow(sigma,2))) / (2*M_PI*pow(sigma,2)) :
      xy? 0.0 : 1.0 ;
}

inline double Gabor(double sigma,double T,double freq, double x, double y,double phi)
{
  return sigma?
      exp(-((x*x)+(y*y))/(2*sigma*sigma))  *  (sin(2*M_PI*freq*(x*cos(T)+y*sin(T))+phi)) :
      (x*x+y*y) ? 0.0 : sin(phi) ;
}


//Vector handling functions:

//sum of two vectors, of lengths l1 and l2:
template<typename TYPE>
TYPE* vecSum(TYPE* v1,int l1,TYPE* v2, int l2)
{
  bool longer1 = (l1>l2) ;
  int lmax= longer1? l1: l2;
  int lmin= longer1? l2: l1;
  TYPE* res=new TYPE[lmax];
  for(int l=0;l<lmin;++l)  
  		res[l]=v1[l]+v2[l];
  for(int l=lmin;l<lmax;++l)  
		res[l]= longer1? v1[l]: v2[l];
  return res;
}

//Convolution of two vectors, of lengths l1 and l2:
template<typename TYPE>
TYPE* vecConvolve(TYPE* v1,int l1,TYPE* v2, int l2)
{
  TYPE* res=new TYPE[l1+l2-1];
  for(int l=0;l<l1+l2-1;++l)
  {
    res[l]=0;
    int start=max(0,l-l2+1);
    int end=min(l+1,l1);
    for(int i=start;i<end;++i)
      res[l]+=(v1[i]*v2[l-i]);
  }
  return res;
}


//Approximation of instantaneous firing rate for a conductance-driven neuron:
double instantFiringRate(double I, double tauRefr, double gLeak, double Ginh, double Einh=0, double  Gexc=0, double Eexc=14/3);


/* Handle multi-dimensional indexes on a single 1d map
 * TODO: REMOVE ALL THIS ;-) (no time, and probably not really useful)

struct MultiIndexMap {
    uint nbDim ;
    uint* maxDim ;
    uint size ;
    
    // Constructor: no default. Only speficic initializations up to 5D index maps.
    // TODO: make general constructors if more than 5D is needed at some time.
    
    MultiIndexMap(uint maxDim1, uint maxDim2) ;
    MultiIndexMap(uint maxDim1, uint maxDim2, uint maxDim3) ;
    MultiIndexMap(uint maxDim1, uint maxDim2, uint maxDim3, uint maxDim4) ;
    MultiIndexMap(uint maxDim1, uint maxDim2, uint maxDim3, uint maxDim4, uint maxDim5) ;
    
    ~MultiIndexMap() ;
   
    // Usage:
 // TODO !
    
private:
    //private default and copy constructors / assignment operator
    MultiIndexMap() {}
    MultiIndexMap&  operator = (const MultiIndexMap& other) {}
    MultiIndexMap(const MultiIndexMap& other) {}
};
*/

#endif










