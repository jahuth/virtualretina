#ifndef RNG_LIBC_H
#define RNG_LIBC_H

#include "rng.h"

class RNG_Libc : public RNG
{
 protected:
  unsigned int st;
 public:
  RNG_Libc();
  virtual ~RNG_Libc(){}
  void srand(unsigned int st);
  unsigned int calcrand(unsigned int s);
  unsigned int rand(void);
  unsigned int get_nth_rand(unsigned int n);
};


#endif
