#ifndef RNG_SIMPLE_H
#define RNG_SIMPLE_H

#include "rng.h"

class RNG_Simple : public RNG
{
 protected:
  int st;
 public:
  RNG_Simple();
  virtual ~RNG_Simple(){}

  void srand(unsigned int st);
  unsigned int calcrand(unsigned int s);
  unsigned int rand(void);
  unsigned int get_nth_rand(unsigned int n);
};


#endif
