#include "stdpf.h"

STDPF::STDPF(double(*f)(double,double)):
  STDP2(),ext_update_weight(f)
{
}

STDPF::STDPF(void)
  : STDP2()
{
  ext_update_weight=NULL;
}

STDPF::~STDPF(void)
{
}

double STDPF::update_weight(double w,double post_pre)
{
  return ext_update_weight(w,post_pre);
}

void STDPF::set_f(double(*f)(double,double))
{
  ext_update_weight=f;
}
