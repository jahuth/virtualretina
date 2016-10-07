#ifndef CIMG_TOOLS_H
#define CIMG_TOOLS_H

#include "misc_tools.h"


//Software CImg is used in all programs, at least to provide the cimg_option() routine that defines our command lines


/* A TRICK THAT NEVER WORKED */

//#ifdef VIRTUALRETINA_CIMG_PRECOMPILATION
//#include<cimg_declarations.h>
//In that case, do not forget to :
//(1) Create header file cimg_declarations.h, by following the instructions in make_cimg_declarations
//(2) manually instantiate all CImg objects you will be using, in file cimg_instantiations.cc

//#else
#include<CImg.h>
//#endif

// ADDING MANUALLY some missing templates for CImg's `superset' weird typedefs !!
// ( this is required in signal_tools.cc )

namespace cimg_library {
    namespace cimg{
        template<> struct superset<double, std::complex<double> > { typedef complex<double> type; };
    }
}

using namespace cimg_library;


//COLOR VARIABLES :
const double red[3]={255,0,0};
const double green[3]={0,255,0};
const double blue[3]={0,0,255};
const double yellow[3]={255,255,0};
const double magenta[3]={255,0,255};
const double cyan[3]={0,255,255};
const double orange[3]={255,128,0};
const double black[3]={0,0,0};
const double white[3]={255,255,255};
const double purple[3]={128,0,128};
const double * const colorSerie(int i);

#include"cimg_signal_drawer.h"


//Drawing functions:

void drawPix_interpol ( CImg<double> & img , double val , int x , int y=0 , int z=0 , int v=0 ) ;


void drawRealCircle(CImg<double> & img, const int x0, const int y0, float r, float width, const double* color, const float opacity=0.99);



// CImg CREATION FUNCTIONS :
CImg<double> build_Gaussian(double sigma);
CImg<double> build_DOG(double sigma1,double sigma2);
CImg<double> build_Gabor(double sigma, double orient, double freq, double phi);

//apply 'filter' to 'image' with linear interpolation.
double applyfilter_interpol(CImg<double> & image, CImg<double> & filter, double centerfilter_x, double centerfilter_y);

//add the 2-dim (or 3-dim, but in the v-dimension) CImg "added", CENTERED in (centerx,centery), to the image or sequence "basic" at z_coordinate z. "added" and "basic" must have the same v-dimension.
void add_image_centered(CImg<double> &basic, const CImg<double> & added, double centerx, double centery, int z=0);


/* ABSTRACT STRUCTURE GIVING ACCESS TO READ-ONLY FUNCTIONS OF THE CImgS POINTED AT BY 'targets'.
 * You can also access any functions of the target image,
 * by directly using pointer 'targets'. This, however, should be done with extreme care,
 * since it possibly allows you to modify private members of objects. */


struct CImgReader
{
  CImg<double>* targets;
  const CImg<double>* const_targets;

  CImgReader(void){targets=0; const_targets=0;}
  ~CImgReader(void){}

  void checktarget(bool constTar) const
  {
    if (!(constTar||targets)) cerr<<"error in CImgReader::read() : targets is not initialized."<<endl;
    if (constTar & !const_targets) cerr<<"error in CImgReader::read() : const_targets is not initialized."<<endl;
  }

  const CImg<double>& read(bool constTargets=false, int n=0) const
  {
    bool constTar = constTargets || !targets;
    checktarget(constTar);
    if(constTar) return const_targets[n];
    else return targets[n];
  }
};

#endif



