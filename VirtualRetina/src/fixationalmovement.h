#ifndef FIXATIONALMOVEMENT_H
#define FIXATIONALMOVEMENT_H

#include "retinatools/misc_tools.h"
#include "retinatools/randomness.h"
#include <xmlparameters.h>

struct FixationalMovementGenerator: virtual public xmlParam::Xmlizable
{
double step, pixPerDegree;
double centerPointX_pix, centerPointY_pix;

FixationalMovementGenerator(double step=1);
~FixationalMovementGenerator(void){}

//the goal of the object:
virtual vector<double> getNextPos_pix(void)=0;
virtual double typicalVariance_pix(void){return 0.0;}

};


struct BasicMicrosaccadeGenerator : public FixationalMovementGenerator
{
//parameters:
double period, noisePeriod;
double spatialAmplitude, noiseAmplitude;
double tempLength, noiseLength;
double noiseDirection;

//indicators:
complex<double> eyePosition;
complex<double> actualSpeed;
complex<double> attractPoint;
int timeToNextSacc, timeToFinishSacc;
bool notStarted;

BasicMicrosaccadeGenerator(double step=1);
~BasicMicrosaccadeGenerator(void){};

vector<double> getNextPos_pix(void);
double typicalVariance_pix(void);

//The xmlClass declaration:
XMLIZE_MACRO;

};


#endif



