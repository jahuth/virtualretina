#include "rng_simple.h"

#define _A 16807
#define _M 2147483647

RNG_Simple::RNG_Simple()
  : RNG()
{
  st=282475249U;
  randmax=2147483647U;
}

void RNG_Simple::srand(unsigned int s)
{
  st=s;
  seed=s;
}

unsigned int  RNG_Simple::calcrand(unsigned int s)
{
  return ((long long)_A*(long long)s)%_M;
}

unsigned int  RNG_Simple::rand(void)
{
  st=calcrand(st);
  return st;
}

unsigned int  RNG_Simple::get_nth_rand(unsigned int n)
{
  // TODO check it...
  return (((long long)seed)*(long long)powmod(_A,n+1,_M))%_M;
}
