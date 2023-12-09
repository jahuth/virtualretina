#include "log_encoder.h"
#include<math.h>

Log_encoder::Log_encoder(double I): Spontaneous()
{
  this->I=I;
  tau=1;
}

void Log_encoder::set_tau(double tau)
{
  this->tau=tau;
}

double Log_encoder::advance(void)
{
  return tau*log(I);
}
