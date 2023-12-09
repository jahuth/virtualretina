#include "random.h"


// this is mainly for compatibility...
static RNG_Simple r1;
static RNG_GSL_mt r2;

double rnd(double max)
{
  return r1.rnd(max);
}
double rnd_poisson(double lambda)
{
  return r1.rnd_poisson(lambda);
}

int randint(int start, int end)
{
  return r1.randint(start,end);
}

double gauss(void)
{
  return r1.gauss();
}

// Mersenne Twister
double mt_rnd(double max)
{
  return r2.rnd(max);
}
double mt_rnd_poisson(double lambda)
{
  return r2.rnd_poisson(lambda);
}

int mt_randint(int start, int end)
{
  return r2.randint(start,end);
}

double mt_gauss(void)
{
  return r2.gauss();
}
