#ifndef RANDOM_H
#define RANDOM_H

#include "rng.h"
#include "rng_simple.h"
#include "rng_gsl_mt.h"

double rnd(double max);     // return 0<=rnd<=max
double rnd_poisson(double lambda); // Poisson distribution (freq.=lambda)
int randint(int start, int end); 
double gauss(void);
double mt_rnd(double max);     // return 0<=rnd<=max
double mt_rnd_poisson(double lambda); // Poisson distribution (freq.=lambda)
int  mt_randint(int start, int end); 
double mt_gauss(void);
#endif
