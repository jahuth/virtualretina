
#include <retinatools/my_tools.h>

int main(int argc, char **argv)
{

  cimg_usage("Create a POSITIVE sinusoidal grating (drifting or not), in .inr");

  const char *name  = cimg_option("-o",
#ifdef ROOTDIR 
ROOTDIR "/"
#endif
"tmp/grating.inr","name (and path) for created signal (please, .[format]  at the end!)");
  
  const int type = cimg_option("-type",0,"0=drifting. 1=static, oscillating. 2=static, reversing.");
  const double step   = cimg_option("-s",.005,"time length of one frame, in seconds.");
  const double lengthB  = cimg_option("-lB",0.0,"length of time before grating apparition, in seconds");
  const double length  = cimg_option("-l",1.0,"length of first grating apparition");
  const double length2  = cimg_option("-lR",1.0,"length of reversed grating apparition, in case of type=2.");
  const int X = cimg_option("-x",200,"x_size of video, in pixels");
  const int Y = cimg_option("-y",200,"y_size of video, in pixels");
  const double freq  = cimg_option("-f",10.0,"temporal frequency, in hertz");
  const double T  = cimg_option("-T",40.0,"spatial period, in pixels");
  const double Lum  = cimg_option("-Lum",1.0,"mean luminance of the grating. Always positive!! We suggest you normalise it (default value)");
  const double Cont  = cimg_option("-Cont",1.0,"contrast of the grating. Between 0 and 1. If 1, then the image reaches luminance level 0.");
  const double phi = cimg_option("-p",0.0,"spatial phase of central pixel at time zero, in multiples of Pi, for a COSINUS.");
  const double phi_t = cimg_option("-tp",0.0,"temporal phase of signal at time zero, in multiples of Pi, for a COSINUS. (only if type=1.)");
  const double theta = cimg_option("-th",0.0,"theta. orientation of the grating (nota : between 0 and 2Pi. Speed is positive towards angle theta).");

  // **************************************************************************

  int Bsize=(int) (lengthB/step);
  int first_grating_size= (int) (length/step);
  int second_grating_size= (type==2)? (int) (length2/step):0;

  int x0= (int)(X/2), y0 =(int)(Y/2);
  double cos_theta=cos(theta), sin_theta=sin(theta);
  cout<<"Saving a grating sequence of "<<Bsize+first_grating_size+second_grating_size<<" frames as "<<name<<" . "<<endl;

  CImg<double> sequence(X,Y,first_grating_size+Bsize+second_grating_size);
  double A=Cont*Lum;
  for(int t=0;t<Bsize;++t)
    cimg_forXY(sequence,x,y) sequence(x,y,t)=Lum;
  for(int t=Bsize;t<Bsize+first_grating_size;++t)
{
    if(type==0)
      cimg_forXY(sequence,x,y) sequence(x,y,t)=Lum + A *cos( (((x-x0)*cos_theta+(y-y0)*sin_theta)/T + phi/2 - freq*step*t)*2*Pi);
    else
    {
      if(type==1)
        cimg_forXY(sequence,x,y) sequence(x,y,t)=Lum + A *cos( (((x-x0)*cos_theta+(y-y0)*sin_theta)/T + phi/2)*2*Pi) * cos((freq*step*t+phi_t/2)*2*Pi);
      else     //type=2
        cimg_forXY(sequence,x,y) sequence(x,y,t)=Lum + A *cos( (((x-x0)*cos_theta+(y-y0)*sin_theta)/T + phi/2)*2*Pi);
    }
}
//adding the reversed grating in case that type==2
for(int t=Bsize+first_grating_size;t<Bsize+first_grating_size+second_grating_size;++t)
      cimg_forXY(sequence,x,y) sequence(x,y,t)=Lum - A *cos( (((x-x0)*cos_theta+(y-y0)*sin_theta)/T + phi/2)*2*Pi);

  sequence.save(name);
}





