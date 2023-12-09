#ifndef RNG_GSL_MT_H
#define RNG_GSL_MT_H

#include "rng.h"

class RNG_GSL_mt : public RNG
{
 protected:
  unsigned int st;
  static const unsigned int N = 624;
  static const unsigned int M = 397;

  static const unsigned long UPPER_MASK = 0x80000000UL;	
  static const unsigned long LOWER_MASK = 0x7fffffffUL;	
  unsigned long mt[N];
  int mti;

 public:
  RNG_GSL_mt();
  virtual ~RNG_GSL_mt(){}
  void srand(unsigned int st);
  unsigned int calcrand(unsigned int s);
  unsigned int rand(void);
  unsigned int get_nth_rand(unsigned int n);
};


#endif
