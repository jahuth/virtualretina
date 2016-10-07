#ifndef SIGNAL_TOOLS_H
#define SIGNAL_TOOLS_H

#include "misc_tools.h"
#include "cimg_tools.h"

//SIGNAL FUNCTIONS: 

//calculate the frequency, amplitude and phase at pixel pix of a periodic, quasi-sinusoidal, signal stocked in a CImg. Useful function to find the principal Fourier component of a signal about which nothing is known.
std::vector<double> analyseSinus(const CImg<double>& signal, int pix=-1);


// Fourier analysis at frequency freq. 'step' is the length of one pixel.
//Outputs : 
// --> result[0] + i*result[1] = int_{N periods of 1/freq} { signal(t)* e^( -i * freq*2*Pi*t) dt }
//
// --> result[2] * cos ( freq*2*Pi*t + result[3] )
//     is the 'best' approximation of 'signal' by a sinusuidal wave at frequency 'freq'.
std::vector<double> analyseFourier(const CImg<double>& signal, double freq, double step=1.0 , bool force=false );


// Rotten results, for some reason...
//Approximates the signal by
//sum_i { result[0](i) cos ( freqSet[i]*2*Pi*t  + result[1](i)  )   }
CImgList<double> coAnalyseFourier(CImg<double> signal, const vector<double> &freqSet, double step=1.0);

//Other version... better results.
//Approximates the signal by
//sum_i { result[0](i) cos ( freqSet[i]*2*Pi*t  + result[1](i)  )   }
CImgList<double> coAnalyseFourierBis(CImg<double> signal, const vector<double> &freqSet, double step=1.0, int number_of_loops=1);

//Make 'phases' a function as continuous as possible (no 2*Pi jumps)
void phaseSmooth(CImg<double>& phases, double period=2*Pi);

#endif



