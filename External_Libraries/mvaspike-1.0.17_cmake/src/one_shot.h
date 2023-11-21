#ifndef ONE_SHOT_H
#define ONE_SHOT_H

#include "sensor.h"

class One_shot : public Sensor 
{
 protected:
  double I; 
  void record_state(void);

 public:
  One_shot(void);
  void set_t(double);
  double ta(void);
  int delta_int(double t);
};

#endif
