#include "radialfilter.h"

//******************** RadialFilter: ****************************************

RadialFilter::RadialFilter(int size_x, int size_y)
{
  done=false; set_sizeX(size_x); set_sizeY(size_y); 
  ratiopix=1; radScheme=0;
  sigma0=0.0; order=0; isRadialScheme=false; Ybuf=0;
}

RadialFilter::~RadialFilter()
{ 
  delete[] Ybuf;
}


void RadialFilter::checkStatus(void)
{
  if(done)
  {
    cerr<<"Error in object RadialFilter: You're changing parameter of a filter after it has started to function."<<endl;
    exit(0);
  }
}

//parameter initilization... this is boring but prettier I guess...
RadialFilter& RadialFilter::set_sizeX(int x)
{
  checkStatus();
  if(x<=0) 
    cerr<<"error in function set_sizeX(): a dimension must be strictly positive."<<endl;
  else 
    sizeX=x;
  return *this;
}

RadialFilter& RadialFilter::set_sizeY(int y)
{
  checkStatus();
  if(y<=0) 
    cerr<<"error in function set_sizeY(): a dimension must be strictly positive."<<endl;
  else 
    sizeY=y;
  return *this;
}

RadialFilter& RadialFilter::set_sigma0(double sigma)
{
  checkStatus();
  if(sigma<0) 
    cerr<<"error in function RadialFilter::set_sigma0(): sigma0, scale of filtering at the center of the image, must be positive or null (for no filtering at all)."<<endl;
  else
    sigma0=sigma;
  return *this;
}

RadialFilter& RadialFilter::set_order(int ord)
{
  checkStatus();
  if((ord<0)||(ord>2))
    cerr<<"error in function RadialFilter::set_order(): The order of a deriche filter is 0 (blur), 1 (first derivative of a gaussian), or 2 (second derivative of a gaussian)."<<endl;
  else
    order=ord;
  return *this;
}

RadialFilter& RadialFilter::set_ratiopix(double r)
{
  checkStatus();
  if(r<=0)
    cerr<<"error in function RadialFilter::set_ratiopix(): Ratio must be strictly positive."<<endl;
  else
    ratiopix=r;
  return *this;
}

RadialFilter& RadialFilter::set_radScheme(RadialScheme *rad)
{
  checkStatus();
  radScheme=rad;
  return *this;
}


//useful functions:
//OK, what follows is adapted from the code in CImg.h, with the addition of spatially-variant coeficients for filtering.

//we store the values of our recursive coeficients, different in each pixel (x,y):
//they are functions of the local filtering scale sigma(x,y)=sigma0*density(r)
void RadialFilter::allocateValues(void)
{
  isRadialScheme=( (radScheme!=0)&&(sigma0!=0) );

  if(isRadialScheme)
  {

    A1=CImg<double>(sizeX,sizeY); A2=A1; A3=A1; A4=A1; B1=A1; B2=A1;

    double r, alpha, ea, ema, em2a, ek, ekn;
    double midX=0.5*(sizeX-1), midY=0.5*(sizeY-1);
    for(int x=0;x<sizeX;++x)
      for(int y=0;y<sizeY;++y)
      {
        r=std::sqrt((x-midX)*(x-midX)+(y-midY)*(y-midY))/ratiopix; // in abstract units
        alpha=1.695f *radScheme->density(r)/(sigma0*ratiopix); // line #1 using radScheme and sigma0 !
        ea=std::exp(alpha), ema=std::exp(-alpha), em2a=ema*ema;

        B1(x,y)=2*ema, B2(x,y)=-em2a;

        switch(order)
        {
        case 1:                 // first derivative
          ek = -(1-ema)*(1-ema)*(1-ema)/(2*(ema+1)*ema);
          A1(x,y) = 0; A4(x,y) = 0;  A2(x,y) = ek*ema; A3(x,y) = -ek*ema;
          break;
        case 2:               // second derivative
          ekn = ( -2*(-1+3*ea-3*ea*ea+ea*ea*ea)/(3*ea+1+3*ea*ea+ea*ea*ea) ); ek = -(em2a-1)/(2*alpha*ema);
          A1(x,y) = ekn;  A2(x,y) = -ekn*(1+ek*alpha)*ema; A3(x,y) = ekn*(1-ek*alpha)*ema; A4(x,y) = -ekn*em2a;
          break;
        default:              // smoothing
          ek = (1-ema)*(1-ema) / (1+2*alpha*ema - em2a);
          A1(x,y) = ek;  A2(x,y) = ek*ema*(alpha-1); A3(x,y) = ek*ema*(alpha+1); A4(x,y) = -ek*em2a;
          break;
        }
      }
	
	Ybuf=new double[max(sizeX,sizeY)];
  }
  done=true;      //no more initialization function possible.
}



//equivalent of cimg_deriche-map. Note that borders are taken care of differently (border condititions where values outside of image are supposed to be the same than at the frame of the image.). Initial values of the filter are divided by two because a 'one-way filter' has a gain of 0.5 (and thus the total filter has a gain of one).

void RadialFilter::radial_deriche_map(CImg<double>& image, int x0, int y0, int z0, int k0, int nb, int offset)
{
  double *ima, *b1, *b2, *a1, *a2, *a3, *a4;

  ima = image.data(x0,y0,z0,k0);
  b1 = B1.data(x0,y0);
  b2 = B2.data(x0,y0);
  a1 = A1.data(x0,y0);
  a2 = A2.data(x0,y0);
  
  a3 = A3.data(x0,y0) + nb*offset;
  a4 = A4.data(x0,y0) + nb*offset;

  double I1=*ima, I2=I1, Y0=I1/2.0, Y1=I1/2.0, Y2=I1/2.0;

  //Recursive loop... going forward
  for (int i=0; i<nb; i++)
  {
    I1 = *ima;
    Y0 = (*a1)*I1 + (*a2)*I2 + (*b1)*Y1 + (*b2)*Y2;
    I2=I1; Y2=Y1; Y1=Y0;
    *Ybuf=Y0;
    ima+=offset; b1+=offset; b2+=offset; a1+=offset; a2+=offset; Ybuf++;
  }
  
  //Recursive loop... going backwards... and adding the 'forward' values stocked in Ybuf.
  //Careful that all pointers have gone out of the image, so the loop must start by decreasing all pointers.
  I1=*(ima-offset); I2 = I1; Y2 = I2/2.0; Y1 = I2/2.0;
  for (int i=0; i<nb; i++)
  {
    ima-= offset; b1-=offset; b2-=offset; --Ybuf; a3-=offset; a4-=offset; 
    I1=*ima;
    Y0= (*a3)*I1 + (*a4)*I2 + (*b1)*Y1 + (*b2)*Y2;
    I2=I1; Y2=Y1; Y1=Y0;
    *ima=*Ybuf+Y0;  //stocking the new results!
  }
}



void RadialFilter::radiallyVariantDeriche(CImg<double>& image, const char axe)
{
  if(!done) allocateValues();
  if(isRadialScheme)
  {
    int offset, nb;
    switch(cimg::uncase(axe))
    {
    case 'x': if (image.width()>1)
      {
        offset = 1;  nb = image.width();
        cimg_forYZC(image,y,z,k) 
          radial_deriche_map(image,0,y,z,k,nb,offset);
      }
      break;
    case 'y': if (image.height()>1)
      {
        offset = image.width();  nb = image.height();
        cimg_forXZC(image,x,z,k)
          radial_deriche_map(image,x,0,z,k,nb,offset);
      }
      break;
    default: throw CImgArgumentException("RadialFilter::radiallyVariantDeriche() : unknown axe '%c', must be 'x' or 'y'.");
    }
  }

  //This line allows to use a RadialFilter as a simple uniform deriche filter, without the use of (non-)varying coeficients A1,A2, etc.
  else
    image.deriche(sigma0*ratiopix,order,axe); // line #2 using sigma0 !
}




void RadialFilter::radiallyVariantBlur(CImg<double>& image)
{
  if (!image.is_empty())
  {
    if (image.width()>1) 
      radiallyVariantDeriche(image,'x');
    if (image.height()>1) 
      radiallyVariantDeriche(image,'y');
  }
}

CImg<double> RadialFilter::get_radiallyVariantDeriche(const CImg<double>& image, const char axe)
{
  CImg<double> im(image,true);
  radiallyVariantDeriche(im,axe);
  return im;
}

CImg<double> RadialFilter::get_radiallyVariantBlur(const CImg<double>& image)
{
  CImg<double> im(image,true);
  radiallyVariantBlur(im);
  return im;
}
