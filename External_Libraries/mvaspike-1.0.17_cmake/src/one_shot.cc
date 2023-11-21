#include "one_shot.h"
#include<math.h>

One_shot::One_shot()
{
  I=1.0;
}

void One_shot::set_t(double I)
{
  this->I=I;
}

double One_shot::ta(void)
{
  return I;
}

int One_shot::delta_int(double t)
{
  I=HUGE_VAL;
  return 0;
}

void One_shot::record_state(void)
{
  rec->add(I);
}
