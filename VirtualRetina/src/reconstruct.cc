
#include"reconstruct.h"



CImg<double> one_cell_firing_rate( const char *spiklist, const int id, char* saved_signal, const int process, const double step, const double cartime, const double beginning, const double end)
{

  FILE *file;


  //reading the spikes file, and constructing the sequence.

  int size = (int)(ceil(end/step)-floor(beginning/step)) ;
  CImg<double> firing(size,1,1,1,0.0);
  int c=0;
  double time=beginning;
  int spike_id=0;
  int carint=0; double normFactor=0.0; double *coefs;

  if(process==0)
  {
    carint=(int)ceil(cartime/step);
    coefs=new double[carint];
    for(int t=0;t<carint;++t) coefs[t]=1;
    normFactor=carint*step; //normFactor==cartime.
  }
  if(process==1)
  {
    carint=(int)ceil(5*cartime/step);
    coefs=new double[carint];
    for(int t=0;t<carint;++t)
    {
      coefs[t]=exp(-t*step/cartime);
      normFactor+=coefs[t]*step;  //normFactor==cartime.
    }
  }
  if(process==2)
  {
    carint=(int)ceil(16*cartime/step);
    coefs=new double[carint];
    for(int t=0;t<carint;++t)
    {
      coefs[t]=exp(-pow(log(t*step/cartime),2.0)/2.0);
      normFactor+=coefs[t]*step;  //normFactor==cartime*sqrt(2*Pi*exp(1)).
    }
  }
  for(int t=0;t<carint;++t)
    coefs[t]/=normFactor;

  file = fopen ( spiklist , "r" );
  while((time<end)&(c!=EOF))
  {
    int index =  (int)(ceil(time/step)-floor(beginning/step)) - 1 ;
    if((spike_id==id)&&(time>beginning))
      for( int t=0 ; (t<carint)&&( index+t<size) ; t++ )
        firing( index+t ) += coefs[t];
    c=fscanf(file, "%d %lf\n",&spike_id,&time);
  }
  fclose(file);

  if(saved_signal!=NULL)
  {
    file=fopen(saved_signal,"w");
    fprintf(file,"<html>\n\n<head>\nTemporal signal. Size %d Step %lf\n</head>\n\n<body>\n",firing.width(),step);
    for(int t=0;t<firing.width();t++)
      fprintf(file, "%lf\n",firing(t));
    fprintf(file,"</body>\n\n</html>");
    fclose(file);
  }

  return firing;
}



void writeTiming(CImg<double> & reconstructed, double step, const double* fgcol, const double* bgcol, int fontSize)
{

  //CImgList<double> default_font=CImgList<double>::get_font(fontSize,true);

  int dimX=reconstructed.width(), dimY=reconstructed.height(), dimZ=reconstructed.depth(), dimV=reconstructed.spectrum();
  double time=0.0;
  std::string text;
  CImg<double> timeImage(dimX-fontSize,fontSize,1,reconstructed.spectrum());
  double percent_time=10;

  for(int t=0;t<dimZ;++t)
  {
    timeImage.fill(-1);
    text="Time: ";
    text+=toString(time);
    timeImage.draw_text(0,0,text.c_str(),fgcol,bgcol,fontSize);
    for(int x=0;x<dimX-fontSize;++x)
      for(int y=0;y<fontSize;++y)
        if (timeImage(x,y)!=-1)
          for(int v=0;v<dimV;++v)
            reconstructed(fontSize+x,dimY-(3*fontSize)/2+y,t)=timeImage(x,y,v);
    if (t>reconstructed.depth()*percent_time/100)
    {
      cout<<"Hang on again!  Time written in "<<percent_time<<" percent of the sequence."<<endl;
      percent_time+=10;
    }
    time+=step;
  }
}







