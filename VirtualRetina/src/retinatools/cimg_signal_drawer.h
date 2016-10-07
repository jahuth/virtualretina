#ifndef CIMG_SIGNAL_DRAWER_H
#define CIMG_SIGNAL_DRAWER_H


#include "cimg_tools.h"

// WRAPPER FOR THE SIGNAL-DRAWING FUNCTIONS OF CImg:
//**************************************************

struct CImgSignalDrawer ;


struct CImgAxis	//(bold as love)
{
  int type;		//0 if linear, 1 if log10(values), 2 if log(values) (maybe one day... 3 if exp(values), 4 if 10^(values))
  double begin; 	//value correponding to first represented pixel
  double end;		//value correponding to last represented pixel
  int nTicks_max ;	//number of ticks... just a maximum indicator !
  int precision;	//floating precision of the value at each tick
  double offsetVal;	//offset value on the OTHER AXIS where this axis will be drawn.
  float opacity;
  const double* color;
  float fontSize; // 13 by default... not sure for other values ^^

  CImgAxis();
  ~CImgAxis(){}

protected :
	
	//'transformed ' variables (if logs are represented )  which will be really used ... 
	// also, they include a 10% margin for better visibility
  double tBegin ;
  double tEnd ;
  double tOffsetVal ;

  CImg<double> scale;       //scale is ' what you read '
  CImg<double> scaleLoc;    //scaleLoc is ' where you read '

  void init();

  friend struct CImgSignalDrawer ;
};



struct CImgErrorBar
{
  const double* color ;
  int end;

  CImgErrorBar();
  ~CImgErrorBar(){}}
;



struct CImgSignalDrawer
{
  //general aspect of graph:
  int style;
  int width;
  int border;
  double opacity;
  bool draw_axis;

  //axis:
  CImgAxis axisX, axisY;

  //error bars:
  CImgErrorBar errorBar;

  CImgSignalDrawer();
  ~CImgSignalDrawer(){}


  // Abscissa version :
  // plots values versus *abscissas.
  // If abscissas==0, then :
  //	- axisX.begin and axisX.end define the beginning and ending time corresponding to values
  //	- if axisX.begin==axisX.end==0 (untouched), then abscissas are taken as [1 : values.size() ].

  void draw( CImg<double> &figure, CImg<double> &values, const double* color=orange, CImg<double>* abscissas=0, CImg<double>* errorBars_v=0 , CImg<double>* errorBars_h=0 );

  // min/max version :
  // Same as before, with abscissas defined as  [tmin : values.size()/(tmax-tmin) : tmax ]
  void draw( CImg<double> &figure, CImg<double> &values, const double* color, double tmin, double tmax, CImg<double>* errorBars_v=0 , CImg<double>* errorBars_h=0 );


  //the (old) file loading version... no x-scales on this one :-)
  void draw( CImg<double> &figure, const char *signal,  const double* color );

  //The CImgList drawing versions ...
  
  // abscissa
  void draw( CImg<double> &figure, CImgList<double> &values, int startColor=0, CImg<double>* abscissas=0 );
  // min-max
  void draw( CImg<double> &figure, CImgList<double> &values, int startColor, double tmin, double tmax );

  
protected:
  // used to draw ONE error bar in the image.
  void draw_errorBar (CImg<double> &figure, int x_cen, int y_cen, double length,  bool horizontal= false ) ;

  // 'enhanced' copies of the draw_axis functions from CImg.h ...
  // allowing values and locations of the ticks to be decorrelated (for log, exp, etc)
  // tickLocations must all be between 0 and 1, and same dimension as tickValues
  void draw_axisX (CImg<double> &figure, CImg<double>& tickValues, CImg<double>& tickLocations, int y ) ;
  void draw_axisY (CImg<double> &figure, CImg<double>& tickValues, CImg<double>& tickLocations, int x ) ;
};

#endif



