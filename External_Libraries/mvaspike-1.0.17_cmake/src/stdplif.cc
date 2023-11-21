#include "stdplif.h"
#include <iostream>
#include <math.h>
using namespace std;

StdpLif::StdpLif(double(*f)(double,double)):
  Internal_Lif(),Neuron(),STDPF(f)
{
}

StdpLif::StdpLif():
  Internal_Lif(),Neuron(),STDPF()
{
}

StdpLif::~StdpLif(void)
{
}

double StdpLif::ta(void)
{
  return Internal_Lif::ta();
}

int StdpLif::delta_int(double t)
{
  STDPF::delta_int(t);
  reset(t);
  Neuron::delta_int(t);
  return 0;
}

void StdpLif::delta_ext(double t,int port)
{
  STDPF::delta_ext(t,port);
  dirac(t,w[port]);
  Neuron::delta_ext(t,port);
}

void StdpLif::init()
{
  Internal_Lif::init();
  STDPF::init(nb_inport);
}

void StdpLif::record_state()
{
  Internal_Lif::record_state(rec);
  STDPF::record_state(rec);
}

void StdpLif::set_learning_function(double(*f)(double,double))
{
  STDPF::set_f(f);
}
