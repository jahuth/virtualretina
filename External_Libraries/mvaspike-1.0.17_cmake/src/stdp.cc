#include "stdp.h"
#include <iostream>
#include <math.h>

using namespace std;

STDP::STDP(void):
  Warray()
{
  lastr=NULL;
}

STDP::~STDP(void)
{
  if (lastr!=NULL)
    delete[] lastr;
}

double STDP::update_weight(double w,double post_pre)
{
  return w;
}

void STDP::delta_int(double t)
{
  lastf=t;
  for (int i=0;i<wlen;i++)
    {
      if (lastr[i]>=0)
	w[i]=update_weight(w[i],t-lastr[i]);
    }
}

void STDP::delta_ext(double t,int port)
{
  lastr[port]=t;
  if (lastf>=0)
    w[port]=update_weight(w[port],lastf-t);
}

void STDP::init(int len)
{
  Warray::init(len);
  lastr=new double[len];
  lastf=-1;
  fill_n(lastr,len,-1);
  wlen=len;
}

void STDP::record_state(BaseRecorder *rec)
{
  for (int i=0;i<wlen;i++)
    rec->add(w[i]);
}
