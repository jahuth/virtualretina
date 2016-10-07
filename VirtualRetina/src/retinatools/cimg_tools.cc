#include "cimg_tools.h"

//Corlor handling:
const double * const colorSerie(int i)
{
  if(i%8==0) return red;
  if(i%8==1) return green;
  if(i%8==2) return blue;
  if(i%8==3) return orange;
  if(i%8==4) return black;
  if(i%8==5) return magenta;
  if(i%8==6) return cyan;
  else return yellow;
}

//Drawing functions:

//not optimized, but HECK !!
void drawPix_interpol ( CImg<double> & img , double val , int x , int y , int z , int v )
{
	int xm = (int)ceil(x)-1 ;
	int ym =y? (int)ceil(y)-1 : 0 ;
	int zm =z? (int)ceil(z)-1 : 0 ;
	int vm =v? (int)ceil(v)-1 : 0 ;
	
	for(int vv=vm ; vv<v+1 ; vv++)
		for(int zz=zm ; zz<z+1 ; zz++)
			for(int yy=ym ; yy<y+1 ; yy++)
				for(int xx=xm ; xx<x+1 ; xx++)
					img(xx,yy,zz,vv) += val*abs(xx-x)*abs(yy-y)*abs(zz-z)*abs(vv-v) ;
}


void drawRealCircle(CImg<double> &img, const int x0, const int y0, float r, float width, const double *color, const float opacity)
{
  if(opacity>=1)
  {
    cout<<"Error in function drawRealCircle: opacity must be strictly inferior to 1."<<endl;
    exit(0);
  }
  img.draw_circle(x0,y0,(int)ceil(r),color,opacity/(opacity-1));
  img.draw_circle(x0,y0,(int)ceil(r+width),color,opacity);
}

// CImg CREATION FUNCTIONS :

CImg<double> build_Gaussian(double sigma)
{
  CImg<double> filt((int)((6*sigma)+1),((int)(6*sigma)+1));      //quite short bounds to limitate calculations. Anyway, filter is centered afterwards.
  double f_cx = 0.5*filt.width();
  double f_cy = 0.5*filt.height();
  double integral=0;
  cimg_forXY(filt,x,y)
  {
    filt(x,y) = Gaussian(sigma,x-f_cx,y-f_cy);
    integral+=filt(x,y);
  }
  filt/=integral;
  return filt;
}

CImg<double> build_DOG(double sigma1,double sigma2)
{
  CImg<double> filt((int)((6*sigma2)+1),((int)(6*sigma2)+1));      //quite short bounds to limitate calculations. Anyway, filter is centered afterwards.
  double f_cx = 0.5*filt.width();
  double f_cy = 0.5*filt.height();
  double integral=0;
  cimg_forXY(filt,x,y)
  {
    filt(x,y) = Gaussian(sigma1,x-f_cx,y-f_cy)-Gaussian(sigma2,x-f_cx,y-f_cy);
    integral+=filt(x,y);
  }
  filt-=integral/filt.size();
  return filt;
}


CImg<double> build_Gabor(double sigma, double orient, double freq, double phi)
{
  CImg<double> filt((int)((6*sigma)+1),((int)(6*sigma)+1));
  double f_cx = 0.5*filt.width();
  double f_cy = 0.5*filt.height();  double integral=0;
  cimg_forXY(filt,x,y)
  {
    filt(x,y) = Gabor(sigma,orient,freq,x-f_cx,y-f_cy,phi);
    integral+=filt(x,y);
  }
  filt-=integral/filt.size();
  return filt;
}


// IMAGE FILTERING FUNCTIONS :

double applyfilter_interpol(CImg<double> & image, CImg<double> & filter, double centerfilter_x, double centerfilter_y)
{

  double my_result=0.0;
  int s_x=filter.width();
  int s_y=filter.height();

  for (int i=0;i<s_x;++i)
    for(int j=0;j<s_y;++j)
      my_result+=filter(i,j)*image.linear_atXY(centerfilter_x-(s_x-1)/2+i,centerfilter_y-(s_y-1)/2+j);

  return my_result;
}

void add_image_centered(CImg<double> &basic, const CImg<double> & added, double centerx, double centery, int z)
{

  int vdim = added.spectrum();
  int s_x = added.width();
  int s_y = added.height();
  double f_cx = 0.5*(s_x-1);             // Mask center
  double f_cy = 0.5*(s_y-1);
  int xmin = max(0,(int) ceil(centerx-f_cx));
  double xmax = min(basic.width()-1,centerx+f_cx);
  int ymin = max(0,(int) ceil(centery-f_cy));
  double ymax = min(basic.height()-1,centery+f_cy);

  for(int x=xmin;x<xmax+1;++x)
    for(int y=ymin;y<ymax+1;++y)
      for(int v=0;v<vdim;++v)
        basic(x,y,z,v) += added.linear_atXY(x-(centerx-f_cx),y-(centery-f_cy),0,v);
}



