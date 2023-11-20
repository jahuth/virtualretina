#ifndef LOG_ENCODER_H
#define LOG_ENCODER_H

#include "spontaneous.h"

class Log_encoder : public Spontaneous
{
 protected:
  double I;
  double tau;
  double advance(void);
 public:
  Log_encoder(double I);
  void set_tau(double tau);
};

#endif
