#include "stdpexp.h"
#include <iostream>
#include <math.h>
#include <assert.h>
#include "random.h"

using namespace std;

STDPExp::STDPExp(void):
  Warray()
{
  W_prepost=0.1;
  W_postpre=-0.1;
  Tau_prepost=1.0;
  Tau_postpre=1.0;
  boundm=0.0;
  boundp=1.0;
  lastr=NULL;
}

STDPExp::~STDPExp(void)
{
  if (lastr!=NULL)
    {
      delete[] lastr;
      delete[] accu_prepost;
    }
}

void STDPExp::delta_int(double t)
{
  accu_postpre=accu_postpre*exp((lastf-t)/Tau_postpre)+W_postpre;
  lastf=t;
  for (int i=0;i<wlen;i++)
    {
      if (stdpon[i]&&(lastr[i]>=0))
	{
	  w[i]+=(accu_prepost[i]*exp((-t+lastr[i])/Tau_prepost));
	  if (w[i]>boundp)
	    w[i]=boundp;
	  if (w[i]<boundm)
	    w[i]=boundm;
	}
    }
}

void STDPExp::delta_ext(double t,int port)
{
  if (stdpon[port])
    {
      accu_prepost[port]=accu_prepost[port]*exp((lastr[port]-t)/Tau_prepost)+W_prepost;
      lastr[port]=t;
      if (lastf>0)
	{
	  w[port]+=(accu_postpre*exp((lastf-t)/Tau_postpre));
	  if (w[port]>boundp)
	    w[port]=boundp;
	  if (w[port]<boundm)
	    w[port]=boundm;
	}
    }
}

void STDPExp::init(int len)
{
  Warray::init(len);
  stdpon=new bool[len];
  fill_n(stdpon,len,true);
  for (unsigned int i=0;i<tmp_learn.size();i++)
    stdpon[tmp_learn[i].port]=tmp_learn[i].flag;

  lastr=new double[len];
  accu_prepost=new double[len];
  accu_postpre=0.0;
  fill_n(accu_prepost,len,0.0);
  lastf=-1;
  fill_n(lastr,len,-1);
  wlen=len;
}

void STDPExp::record_state(BaseRecorder *rec)
{
  // should save ALL state variables...
  for (int i=0;i<wlen;i++)
    rec->add(w[i]);
}

void STDPExp::set_bounds(double m,double p)
{
  assert(m<=p);
  boundp=p;
  boundm=m;
}

void STDPExp::learn(int port,bool f)
{
  tmp_learn.push_back(TmpLearn(port,f));
}

void STDPExp::set_tau(double prepost,double postpre)
{
  Tau_postpre=postpre;
  Tau_prepost=prepost;
}

void STDPExp::set_dw(double wprepost,double wpostpre)
{
  W_postpre=wpostpre;
  W_prepost=wprepost;
}

