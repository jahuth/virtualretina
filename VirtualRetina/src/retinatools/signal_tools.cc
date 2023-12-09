#include "signal_tools.h"

//SIGNAL FUNCTIONS
//a simple and intuitive way of calculating a period, an amplitude, etc...

vector<double> analyseSinus(const CImg<double>& signal, int pix)
{
  if ((signal.height()!=1)||(signal.depth()!=1)||(signal.spectrum()!=1)||(signal.width()<2))
  {
    cerr<<"Error in function analyseSinus: the CImg signal must be one dimensional along axis X, and more than two pixels long."<<endl;
    exit(0);
  }

  double frequency, amplitude, phase, period=0;

  int lastMin=0, lastMax=0, numberMin=0, numberMax=0;
  double periodSquareSum=0, amplitudeSquareSum=0, lastVal=signal(1);
  bool ISincreasing=false, WASincreasing=(signal(1)-signal(0))>0;

  int phasePix=(pix==-1)? signal.width()/2:pix;
  double summedPhasesToMins=0, summedPhasesToMaxs=0;
  int numberMaxBeforePix=0, numberMinBeforePix=0;

  //We favor long intervals of monotony...
  for(int x=2;x<signal.width();++x)
  {
    ISincreasing=(signal(x)-lastVal>=0);
    if (ISincreasing&&(!WASincreasing))
    {
      if (numberMin!=0)
        periodSquareSum+= (x-lastMin)*(x-lastMin);
      if (numberMax!=0)
        amplitudeSquareSum+= (signal(x)-signal(lastMax))*(signal(x)-signal(lastMax));
      lastMin=x, numberMin+=1;
      summedPhasesToMins+= phasePix-lastMin;
      if(lastMin<=phasePix) numberMinBeforePix+=1;
    }
    if (WASincreasing&&(!ISincreasing))
    {
      if (numberMax!=0)
        periodSquareSum+= (x-lastMax)*(x-lastMax);
      if (numberMin!=0)
        amplitudeSquareSum+= (signal(x)-signal(lastMin))*(signal(x)-signal(lastMin));
      lastMax=x, numberMax+=1;
      summedPhasesToMaxs+= phasePix-lastMax;
      if(lastMax<=phasePix) numberMaxBeforePix+=1;
    }
    lastVal=signal(x), WASincreasing=ISincreasing;
  }

  //finalizing:
  if((numberMin==0)||(numberMax==0))
  {
    cerr<<"Could not use function analyseSinus: required function is too monotonous (less than one max and one min)"<<endl;
  }

  if((numberMin==1)&&(numberMax==1))
  {
    period=2.0*abs(lastMin-lastMax);
    frequency=0.5/abs(lastMin-lastMax);
    amplitude=(signal(lastMax)-signal(lastMin))/2.0;
  }
  else
  {
    period= sqrt(periodSquareSum/(numberMin+numberMax-2.0));
    frequency=1.0/period;
    amplitude=0.5*sqrt(amplitudeSquareSum/(numberMin+numberMax-1.0));
  }

  summedPhasesToMaxs -=( period*numberMaxBeforePix*(numberMaxBeforePix-1)/2.0 -period*(numberMax-numberMaxBeforePix)*(numberMax-numberMaxBeforePix+1)/2.0);
  summedPhasesToMins -= period*numberMinBeforePix*(numberMinBeforePix-1)/2.0 -period*(numberMin-numberMinBeforePix)*(numberMin-numberMinBeforePix+1)/2.0;

  double phaseFromMins=fmod(summedPhasesToMins/(numberMin*period)+0.75,1.0)*2*Pi;
  double phaseFromMaxs=fmod(summedPhasesToMaxs/(numberMax*period)+0.25,1.0)*2*Pi;
  //just averaging... not really neat...
  phase=(numberMin*phaseFromMins+numberMax*phaseFromMaxs)/(numberMin+numberMax);

  std::vector<double> res;
  res.push_back(frequency);    //frequency
  res.push_back(amplitude);   //amplitude
  res.push_back(phase);//phase
  return res;
}

vector<double> analyseFourier(const CImg<double>& signal, double freq, double step, bool force)
{
  if ((signal.height()!=1)||(signal.depth()!=1)||(signal.spectrum()!=1)||(signal.width()<2))
  {
    cerr<<"Error in function analyseFourier: the CImg signal must be one dimensional along axis X, and more than two pixels long."<<endl;
    exit(0);
  }

  int numberPeriods=(int)floor(freq*signal.width()*step);

  if(numberPeriods==0)
    if(force)
      numberPeriods=1 ;
    else
    {
      cerr<<"Error in function analyseFourier: input signal is too short to be Fourier-analysed at this frequence. Use option force if you believe this is not a problem (for rest of the period, signal will be taken as zero)."<<endl;
      exit(0);
    }

  double sumCos=0.0;
  double sumSin=0.0;
  double XI=freq*step*2*Pi;
  double bound = min( signal.width() , numberPeriods/(freq*step) );

  for(int x=0;x<bound;++x)
  {
    sumCos+=cos(XI*x)*signal(x);
    sumSin-=sin(XI*x)*signal(x);
  }
  sumCos*=step;  sumSin *= step;
  vector<double> res;
  res.push_back(sumCos);   //real part of fourier transform
  res.push_back(sumSin);   //imaginary part of fourier transform

  //Now we also add to the vector the amplitude and phase of the sinusoidal component of 'signal' at frequency 'freq' (just like in function 'analyseSinus')

  double cosAmp=2*sumCos*freq/numberPeriods;
  double sinAmp=2*sumSin*freq/numberPeriods;
  res.push_back(norm(cosAmp,sinAmp));
  res.push_back(atan2(sinAmp,cosAmp));

  return res;
}


CImgList<double> coAnalyseFourierBis(CImg<double> signal, const vector<double> &freqSet, double step, int number_of_loops)
{
  int numberFreq=freqSet.size();
  int nLoops=(number_of_loops)>0? number_of_loops:numberFreq;  //why not...

  complex<double> *components= new complex<double>[numberFreq];

  for(int rept=0;rept<nLoops;++rept)
  {
    for(int i=0;i<numberFreq;++i)
    {
      vector<double> resu=analyseFourier(signal,freqSet[i],step);
      complex<double> comp(resu[2]*cos(resu[3]),resu[2]*sin(resu[3]));
      components[i]+=comp;
      double XI = freqSet[i]*step*2*Pi;
      for( int x=0 ; x<signal.width() ; ++x )
	      signal(x) -= resu[2] * cos( XI*x + resu[3]);
    }
  }

  CImgList<double> res(2,numberFreq);
  for(int i=0;i<numberFreq;++i)
  {
    res[0](i)=abs(components[i]);
    res[1](i)=arg(components[i]);
  }
  return res;
}


CImgList<double> coAnalyseFourier(CImg<double> signal, const vector<double> &freqSet, double step)
{
  int numberFreq=freqSet.size();

  //scalar products between test frequencies on this finite sample of time:
  CImg<complex<double> > freqMatrix(signal.width(),numberFreq);
  for(int i=0;i<numberFreq;++i)
  {
    double angSpeed=freqSet[i]*step*2*Pi;
    complex<double> norm=0;
    for(int t=0;t<signal.width();++t)
      freqMatrix(t,i)=exp(Icplx*(complex<double>)(angSpeed*t));
  }

  CImg<complex<double> > freqMatrixConj=freqMatrix.get_transpose();
  cimg_forXY(freqMatrixConj,x,y) freqMatrixConj(x,y)=conj(freqMatrixConj(x,y));

  CImg<complex<double> > scalFreqCplx=freqMatrix*freqMatrixConj;
  CImg<double> scalFreq(numberFreq,numberFreq);
  cimg_forXY(scalFreq,x,y) scalFreq(x,y)=real(scalFreqCplx(x,y));
  //scalFreq.pseudoinvert();   // 2023 : function does not exist anymore:
  scalFreq.invert();   // 2023 : seems to be the only replacement available. Hope and pray I never actually used that function ^^
  cimg_forXY(scalFreq,x,y) scalFreqCplx(x,y)=scalFreq(x,y);

  //projection of the signal on the subspace of test frequencies:
  CImg<complex<double> > signalScal=signal*freqMatrixConj;
  CImg<complex<double> > signalComponents=signalScal*scalFreqCplx;

  CImgList<double> res(2,numberFreq);
  for(int i=0;i<numberFreq;++i)
  {
    res[0](i)=abs(signalComponents(i));
    res[1](i)=arg(signalComponents(i));
  }
  return res;
}


void phaseSmooth(CImg<double>& phases, double period)
{
  if ((phases.height()!=1)||(phases.depth()!=1)||(phases.spectrum()!=1)||(phases.width()<2))
  {
    cerr<<"***ERROR in function phaseSmooth: the CImg phases must be one dimensional along axis X."<<endl;
  }
  for(int i=1;i<phases.width();++i)
  {
    if(phases(i)-phases(i-1)>period/2)
      for(int j=i;j<phases.width();++j) phases(j)-=period*ceil((phases(i)-phases(i-1))/period-0.5);
    if(phases(i-1)-phases(i)>period/2)
      for(int j=i;j<phases.width();++j) phases(j)+=period*ceil((phases(i-1)-phases(i))/period-0.5);
  }
}
