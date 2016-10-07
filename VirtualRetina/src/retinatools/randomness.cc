
#include "randomness.h"


double ranf()
{
	return (double)rand()/RAND_MAX ;
}


//box-muller method (from http://www.taygeta.com/random/gaussian.html) :
double rand_gauss()
{
    
    double x1, x2, w, y1, y2;
    
    do {
        x1 = 2.0 * ranf() - 1.0;
        x2 = 2.0 * ranf() - 1.0;
        w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    return y1;
}

