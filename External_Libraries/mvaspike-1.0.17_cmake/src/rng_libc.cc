#include "rng_libc.h"
#include <stdlib.h>


RNG_Libc::RNG_Libc()
  : RNG()
{
  randmax=RAND_MAX;
  
}

void RNG_Libc::srand(unsigned int s)
{
  seed=s;
  st=s;
}

unsigned int  RNG_Libc::calcrand(unsigned int s)
{ 
  return rand_r(&s);
}

unsigned int  RNG_Libc::rand(void)
{
  st=rand_r(&st);
  return st;
}

unsigned int  RNG_Libc::get_nth_rand(unsigned int n)
{
  #warning FIXME
  // FIXME : implement it, even if it's useless here ...
}
