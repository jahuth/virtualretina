
#include "fixationalmovement.h"


FixationalMovementGenerator::FixationalMovementGenerator(double stepp)
{step=stepp; centerPointX_pix=0.0; centerPointY_pix=0.0;}

BasicMicrosaccadeGenerator::BasicMicrosaccadeGenerator(double stepp) : FixationalMovementGenerator(stepp)
{
  actualSpeed=0, eyePosition=0;
  timeToFinishSacc=-1;
  notStarted=true;

  pixPerDegree=1.0;

  period=0.2;
  noisePeriod=0.04;
  spatialAmplitude=10.0;
  noiseAmplitude=0.04;
  tempLength=10.0;
  noiseLength=0.04;
  noiseDirection=0.3;
}


void BasicMicrosaccadeGenerator::xmlize()
{
  //We put them anyway, but fix these two from elsewhere !! (Retina...)
  add_param(step,"temporal-step__sec");
  add_param(pixPerDegree,"pixels-per-degree");

  // Real parameters:
  add_param(period,"period-mean__sec");
  add_param(noisePeriod,"period-stdev__sec");
  add_param(spatialAmplitude,"amplitude-mean__deg");
  add_param(noisePeriod,"amplitude-stdev__deg");
  add_param(tempLength,"saccade-duration-mean__sec");
  add_param(noiseLength,"saccade-duration-stdev__sec");
  add_param(noiseDirection,"angular-noise__pi-radians");
}



vector<double> BasicMicrosaccadeGenerator::getNextPos_pix(void)
{
  attractPoint=complex<double>(centerPointX_pix,centerPointY_pix);

  if(notStarted)
  {
    eyePosition=attractPoint;
    notStarted=false;
    timeToNextSacc=(int)max( period/step + noisePeriod/step*rand_gauss(), 0 );
  }     //ugly, but well...

  // Currently undergoing a saccade
  if (timeToNextSacc==0)
  {

    if(timeToFinishSacc==-1)
    {
      double saccAmp=max( spatialAmplitude*pixPerDegree + noiseAmplitude*pixPerDegree*rand_gauss(), 0 );
      double saccLength=max( tempLength + noiseLength*rand_gauss(), step);
      double saccDir=arg(attractPoint-eyePosition) + noiseDirection*Pi*(1-2*ranf());

      actualSpeed=complex<double>(saccAmp/saccLength*cos(saccDir),saccAmp/saccLength*sin(saccDir));
      timeToFinishSacc=(int)(saccLength/step);

    }
    if(timeToFinishSacc==0)
    {
      timeToNextSacc=(int)max( period/step + noisePeriod/step*rand_gauss(), 0 );
    }
    eyePosition+=actualSpeed*step;
    timeToFinishSacc-=1;
  }

  // Waiting for the next saccade
  else
  {
    timeToNextSacc-=1;
  }

  vector<double> res;
  res.push_back(real(eyePosition));
  res.push_back(imag(eyePosition));

  return res;
}

double BasicMicrosaccadeGenerator::typicalVariance_pix(void)
{
  return 2.0*spatialAmplitude*pixPerDegree;
}	//this choice is totally arbitrary... except if noiseDirection<1/3, in which case spatialAmplitude is an absolute bound for variations of the generator around its center of attention.


