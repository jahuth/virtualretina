#ifndef RECONSTRUCT_H
#define RECONSTRUCT_H


#define XPOS    0
#define YPOS    1
#define DENSITY_CELL 2
#define DENSITY_NORM 3
#define TOTDBLPRM  4
#define TOTINTPRM  0

#include "retina.h"

//This header contains some useful functions for exploiting the data of a retinian simulation :
// - one_cell_firing_rate() is a function calculating the instantaneous firing rate for one cell, with the linear kernel you want.
//- writeTiming() writes time in a reconstructed retina sequence.

// Also contains a project header for a SpikeReconstructor class (TODO: write the code).


//takes a spike train emitted by one cell and linearly reconstructs an instantaneous firing rate (in spikes per second) as a CImg. It can also save the temporal signal.
//"spiklist" is the adressof the spike file to be read, "id" the id of the neuron represented.
//"saved_signal" is the adress where to save the calculated temporal signal. Set to NULL if you dont want to draw.
//"process" defines the type of linear kernel used for reconstruction.
//process=0 --> each spike is associated to a step of height 1/"cartime", lasting "cartime".
//process=1 --> each spike is associated to a decreasing exponential with characteristic time "cartime".
//process=2 --> each spike is associated to a causal gaussian-of-log function with characteristic time "cartime".
//"step" is the duration of one time_step of the reconstructed signal, in seconds.
//"cartime" has different significations according to the reconstruction process used.
//"beginning" and "end" are the temporal boundaries for the reconstructed rate, IN THE SPIKE FILE. No mistakes at this level !
CImg<double> one_cell_firing_rate( const char *spiklist, const int id, char* saved_signal, const int process, const double step, const double cartime, const double beginning, const double end);


//HELPER FUNCTION OF APPLI ReconstructRetina.cc, and of any other...
//write timing in a reconstructed retina sequence...
void writeTiming(CImg<double> & reconstructed, double step, const double* fgcol, const double* bgcol=0, int fontSize=24);


//A structure that will handle the reconstruction of images from spike trains
//TODO !!
/*
 * class SpikeReconstructor {
 *
 * //general parameters
 * double beginTime, endTime;
 * double step ;
 * double scaleFactor ;
 * double xmin, xmax, ymin, ymax ;
 *
 * Retina* daRetina ;
 * char* daSpikes ;
 *
 * // If channelAmp[i]=0 --> don't represent channel i
 * // If *(channelAmp[i])=val --> associate each spike in channel i to a weight val
 * // (possibly with density normalization, for non-uniform foveas)
 * double** channelAmps ;
 *
 *
 * public:
 * SpikeReconstructor();
 * ~SpikeReconstructor(){}
 *
 * //All the set functions:
 * SpikeReconstructor & set_beginTime(double time) ;
 * SpikeReconstructor & set_endTime(double time) ;
 * SpikeReconstructor & set_step(double step) ;
 * SpikeReconstructor & set_scale(double step) ;
 * SpikeReconstructor & set_frame(double xMin, double xMax, double yMin, double yMax) ;
 *
 * SpikeReconstructor & set_retina( Retina& retina ) ;
 * SpikeReconstructor & set_spikeFile( char* spikeFile ) ;
 *
 *
 * //Proposed general handling of reconstruction methods :
 *
 * // A single reconstruction function can be called...
 * CImg<double> reconstruct( );
 *
 * //...that will call protected pointers-to-functions...
 *
 *protected:
 * //Reconstruction procedure after each spike
 * void (*spkReconstruct) ( int spike_id ) ;
 *
 * 
 * //...which can be initialized from different static member functions...
 *
 * public:
 * static void simpleReconstruct ( int spike_id ) ;
 *
 * //...thanks to 'set' methods
 * SpikeReconstructor & set_method ( void (*methodPointer) (int spike_id) )
 *{
 *  spkReconstruct=methodPointer ;
 *}
 *
 * };
 */


#endif
