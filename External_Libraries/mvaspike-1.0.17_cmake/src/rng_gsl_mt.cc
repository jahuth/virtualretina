#include "rng_gsl_mt.h"
#include <stdlib.h>

// the following is taken from the Gnu Scientific Library
// which is based in turn on the work of Makoto Matsumoto
// http://www.math.keio.ac.jp/~matumoto/emt.html

#define N 624	/* Period parameters */
#define M 397

static const unsigned long UPPER_MASK = 0x80000000UL;	
static const unsigned long LOWER_MASK = 0x7fffffffUL;

// TODO : remove st from base class -> useful only when using
// simple generators
RNG_GSL_mt::RNG_GSL_mt()
  : RNG()
{
  randmax=4294967295U;
  srand(4357);
}

void RNG_GSL_mt::srand(unsigned int s)
{
  // TODO : assert s>0 ?
  seed=s;
  
  int i;
  //if (s==0)
  //  s=4357;	/* the default seed is 4357 */

  mt[0]=s&0xffffffffUL;

  for (i=1;i<N;i++)
    {
      /* See Knuth's "Art of Computer Programming" Vol. 2, 3rd
         Ed. p.106 for multiplier. */
      
      mt[i]=
        (1812433253UL*(mt[i-1] ^ (mt[i-1] >> 30))+i);
      
      mt[i] &= 0xffffffffUL;
    }
  mti=i;
}

unsigned int  RNG_GSL_mt::calcrand(unsigned int s)
{ 
  // TODO : not available ... for "good"  RNGS
  // suggest : drop it from rng.h
  return 0;
}

unsigned int RNG_GSL_mt::rand(void)
{
  unsigned long k ;
  // unsigned long int *const mt=mt;

#define MAGIC(y) (((y)&0x1) ? 0x9908b0dfUL : 0)

  if (mti >= N)
    {	/* generate N words at one time */
      int kk;
      
      for (kk = 0; kk < N - M; kk++)
	{
	  unsigned long y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	  mt[kk] = mt[kk + M] ^ (y >> 1) ^ MAGIC(y);
	}
      for (; kk < N - 1; kk++)
	{
	  unsigned long y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
	  mt[kk] = mt[kk + (M - N)] ^ (y >> 1) ^ MAGIC(y);
	}

      {
	unsigned long y = (mt[N - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
	mt[N - 1] = mt[M - 1] ^ (y >> 1) ^ MAGIC(y);
      }

      mti = 0;
    }

  /* Tempering */
  
  k = mt[mti];
  k ^= (k >> 11);
  k ^= (k << 7) & 0x9d2c5680UL;
  k ^= (k << 15) & 0xefc60000UL;
  k ^= (k >> 18);

  mti++;
  return k;
}

unsigned int  RNG_GSL_mt::get_nth_rand(unsigned int n)
{
  // FIXME : implement it, even if it's useless here ...
#warning FIXME
  return 0;
}
