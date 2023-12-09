#include <math.h>
#include "rng.h"

RNG::RNG(void)
{
  seed=0;
  randmax=1;
  flag=0;
}

void RNG::srand(unsigned int s)
{
  seed=s;
}

double RNG::rnd(double max)
{
  return (max*rand())/randmax;
}

int RNG::randint(int start,int end)
{
  // TODO test
  return start+(rand()%(end-start));
  // return start+(end-start)*(rand()/(randmax+1.0))
}


double RNG::rnd_poisson(double lambda)
{
  return -lambda*log(1.0-rnd(1.0));
}

double RNG::gauss(void)
{
  double fac,rsq,v1,v2; 
  if (!flag)
    {
      do { 
	v1=2.0*rnd(1.0)-1.0; 
	v2=2.0*rnd(1.0)-1.0; 
	rsq=v1*v1+v2*v2;
      } while (rsq >= 1.0 || rsq == 0.0);
      fac=sqrt(-2.0*log(rsq)/rsq);
      sg=v1*fac; 
      flag=1;
      return v2*fac;
    }
  else 
    {
      flag=0;
      return sg;
    }
}

// WARNING : global scope
int powmod(int a,int r,int n)
{
  long long b=a;
  long long ret=1;
  while (r>0)
    {
      if (r%2==1)
	{
	  ret=(ret*b)%n;
	  r--;
	}
      b=(b*b)%n;
      r/=2;
    }
  return ret;
}
