#include "stdp2.h"
#include <iostream>
#include <math.h>

using namespace std;

STDP2::STDP2(void):
  Warray()
{
  lastr=NULL;
}

STDP2::~STDP2(void)
{
  if (lastr!=NULL)
    delete[] lastr;
}

double STDP2::update_weight(double w,double post_pre)
{
  return w;
}

void STDP2::delta_int(double t)
{
  lastf.push_back(t);
  for (int i=0;i<wlen;i++)
    {
      for(unsigned int j=0;j<lastr[i].size();j++)
	w[i]=update_weight(w[i],t-lastr[i][j]);
    }
}

void STDP2::delta_ext(double t,int port)
{
  lastr[port].push_back(t);
  for(unsigned int j=0;j<lastf.size();j++)
    w[port]=update_weight(w[port],lastf[j]-t);
}

void STDP2::init(int len)
{
  Warray::init(len);
  lastr=new vector<double>[len];
  wlen=len;
}

void STDP2::record_state(BaseRecorder *rec)
{
  for (int i=0;i<wlen;i++)
    rec->add(w[i]);
}
