#include "internal_lif.h"
#include <math.h>
#include <iostream>
using namespace std;
// TODO : set_V avec un temps 
// idem pour I
// eventuellement idem pour tau

Internal_Lif::Internal_Lif(void)
{
  V=0.0;
  Vreset=0.0;
  Tau=1.0;
  I=1.581976707;
  tl=0;
  thres=1.0;
  Trefra=-1.0;
  tf=-1;
}

Internal_Lif::~Internal_Lif(void)
{
}

double Internal_Lif::ta(void)
{
  if (V>=thres) return tl;
  if (I<=thres) return HUGE_VAL;
  return tl+Tau*log((I-V)/(I-thres));
}

void Internal_Lif::reset(double t)
{
  V=Vreset;
  tl=t;
  tf=t;
}

void Internal_Lif::dirac(double t,double w)
{
  if (t>tf+Trefra)
    {
      V=I+(V-I)*exp(-(t-tl)/Tau)+w;
      tl=t;
    }
}

void Internal_Lif::step_current(double t,double wi)
{
  V=I+(V-I)*exp(-(t-tl)/Tau);
  I+=wi;
  tl=t;
}

void Internal_Lif::set_V(double V)
{
  this->V=V;
}

void Internal_Lif::set_V(double V,double t)
{
  this->V=V;
  tl=t;
}

void Internal_Lif::set_Vreset(double V)
{
  this->Vreset=V;
}

void Internal_Lif::set_I(double I)
{
  this->I=I;
}

void Internal_Lif::set_I(double I,double t)
{
  this->I=I;
  tl=t;
}

void Internal_Lif::set_Tau(double Tau)
{
  this->Tau=Tau;
}

void Internal_Lif::set_Tau(double Tau,double t)
{
  this->Tau=Tau;
  tl=t;
}

double Internal_Lif::get_I(void)
{
  return I;
}

double Internal_Lif::get_V(void)
{
  return V;
}

void Internal_Lif::advance(double t)
{
  V=I+(V-I)*exp(-(t-tl)/Tau);
  tl=t;
}

double Internal_Lif::get_Tau(void)
{
  return Tau;
}

double Internal_Lif::get_tl(void)
{
  return tl;
}

void Internal_Lif::set_threshold(double t)
{
  thres=t;
}

void Internal_Lif::set_Trefra(double t)
{
  Trefra=t;
}

void Internal_Lif::init()
{

}

void Internal_Lif::record_state(BaseRecorder *rec)
{
  rec->add(V);
  rec->add(Tau);
  rec->add(I);
  rec->add(tl);
  rec->add(thres);
}
