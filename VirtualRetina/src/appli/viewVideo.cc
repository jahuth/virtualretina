
#include <retinatools/my_tools.h>

void copy_in_frame(CImg<double> &frame, CImg<double> &seq, int t=0)
{
  for(int v=0;v<frame.spectrum();++v)
  {
    int seqV=min(v,seq.spectrum()-1);
    for(int x=0;x<seq.width();++x)
      for(int y=0;y<seq.height();++y)
        frame(x,y,0,v)=seq(x,y,t,seqV);
  }
}

int main(int argc, char **argv)
{

  cimg_usage(" play the video of a 2,3 or 4-D image file.\n\t\tusage : viewVideo [options] sequence.inr [or image_in*, or -i input_file] [...] \nIf s=0, buttons PageUp and PageDown switch from one image to another (mouse must be on the display, ESC command to leave).\nIf s>0, automatic playing. ESC command to leave (mouse must be on the display). ");

  const char *inputFile = cimg_option( "-i" , "noFile" , "optional path to a text file in which all input frames are stocked. Necessary to load very big sequences, when the shell stack cant follow anymore. Must contain all the file names, separated by spaces, and only the file names.");
  const unsigned int step = cimg_option("-s",0,"time length of one frame, in millseconds. If left at 0, no automatic refreshing.");
  const double magnif = cimg_option("-f",1.0,"magnification factor from video size to display size");
  const int affich_color = cimg_option("-col",0,"color of position dispay. 0=automatic, 1=white, 2=red, 3=black");
  const int normalize = cimg_option("-n",0,"Normalization type. 0=at every frame, 1=global, 2=automatic rectification, and custom amplification given by -amp");
  const double ampli = cimg_option("-amp",1.0,"Amplification factor when -n 2. ...anything can happen!");
  const int position = cimg_option("-p",1,"Set to zero if no position information is desired.");
  const int beginning = cimg_option("-b",0,"beginning frame to be represented.");

  
  // **************************************************************************

  //STEP 1 - Loading input - Directly copied from the loading protocol in Retina.cc.
  vector<std::string> inputNames;

  if(strcmp(inputFile,"noFile"))
  {
    string oneFile;
    ifstream ifs;
    ifs.open(inputFile);
    //reading input sequence/frames
    while(ifs>>oneFile) inputNames.push_back(oneFile);
  }
  else
  {
    bool opt = false;
    for (int k=1; k<argc; k++)
      if (argv[k][0]=='-' && argv[k][1]!='\0') opt=true;
      else
      {
        if (!opt)
          inputNames.push_back(argv[k]);
        opt=false;
      }
  }

  if (!inputNames.size())
  {
    std::fprintf(stderr,"\n** You must specify at least one input image\n** Try '%s -h' to get help\n**\n",cimg::basename(argv[0]));
    exit(0);
  }

  CImg<double> In(inputNames[0].c_str());
  int s_sx=In.width(),s_sy=In.height();
  int beginfrm=0,endfrm=In.depth();
  bool one3DSeq=(endfrm>1);
  if(one3DSeq) cout<<"Input is the 3-dimensional sequence "<<inputNames[0]<<", of dimensions ("<<s_sx<<","<<s_sy<<","<<endfrm<<"). Following input image files, if they exist, will not be taken into account."<<endl;
  else endfrm=inputNames.size();
  if ((beginning<endfrm)&(beginning>0))
    beginfrm=beginning;
  if(!one3DSeq) cout<<"Input is the list of 2-dimensional images of dimensions ("<<s_sx<<","<<s_sy<<"), starting at "<<inputNames[beginfrm]<<" and finishing at "<<inputNames[endfrm-1]<<"."<<endl;



  //Handling normalization type and creating the display.

  //at least 150 to always allow the writing of pixel information...
  int s_x=max(s_sx,150), s_y=s_sy;
  CImg<double> frame(s_x,s_y,1,3,0.0), originalFrame(1);

  int norm_type = (normalize-1)*(normalize-2)/2;   //1 for case 0 (normalization at every display), 0 for cases 1 and 2 (no normalization).
  CImgDisplay screen((int)(s_x*magnif),(int)(s_y*magnif),inputNames[0].c_str(),norm_type);

  double* localNorm=0;
  if(normalize==1)//global normalization
    if(one3DSeq)
      In.normalize(0,255.0);
    else	//ouch
    {
      cout<<"Hold on for a moment, we are calculating the local maximum for each frame."<<endl;
      double globalMax=-HUGE_VAL;
      localNorm=new double[endfrm];
      CImg<double> stat;
      for(int im=0;im<endfrm;++im)
      {
        stat.load(inputNames[im].c_str()).get_stats();
        localNorm[im]=stat(1); //(max)
        globalMax=max( globalMax, stat(1) );
      }
      for(int im=0;im<endfrm;++im)
        localNorm[im]*=(255.0/globalMax);
    }
  if(normalize==2)//amplification
    if(one3DSeq)
      (In.max(0.0))*=ampli;

  //little colour problems
  double* drawing_colour=new double[3];
  if(affich_color==0)
  {
    if (In.spectrum()==3) for (int i=0;i<3;i++) drawing_colour[i]=white[i];
    else for (int i=0;i<3;i++) drawing_colour[i]=red[i];
  }
  else if(affich_color==1) for (int i=0;i<3;i++) drawing_colour[i]=white[i];
  else if(affich_color==2) for (int i=0;i<3;i++) drawing_colour[i]=red[i];
  else if(affich_color==3) for (int i=0;i<3;i++) drawing_colour[i]=black[i];



  if(step!=0)
  {
    for (int t=beginfrm;t<endfrm;t++)
    {
      frame.fill(0.0);
      if(!one3DSeq)
      {
        originalFrame.load(inputNames[t].c_str()).resize(s_sx,s_sy,1,originalFrame.spectrum());
        copy_in_frame(frame,originalFrame);
        if(normalize==1) frame.normalize(0,localNorm[t]);
        if(normalize==2) (frame.max(0.0))*=ampli;
      }
      else
        copy_in_frame(frame,In,t);
      screen.display(frame).wait(step);
    }
    while (!screen.is_closed()) screen.wait();
  }

  else
  {
    int t=beginfrm;
    while(!screen.is_closed() && !(screen.key()==cimg::keyESC))
    {
      frame.fill(0.0);
      if(!one3DSeq)
      {
        originalFrame.load(inputNames[t].c_str()).resize(s_sx,s_sy,1,originalFrame.spectrum());
        copy_in_frame(frame,originalFrame);
        if(normalize==1) frame.normalize(0,localNorm[t]);
        if(normalize==2) (frame.max(0.0))*=ampli;
      }
      else
        copy_in_frame(frame,In,t);

      if (screen.mouse_x()>=0)
      {
        const int mx = (int)(screen.mouse_x()/magnif), my = (int)(screen.mouse_y()/magnif);
        if(normalize==0) frame.normalize(0,255);
        if(position!=0)
        {
          frame.draw_text(2,2,"(x,y,frame)=(%d,%d,%d)",drawing_colour,NULL,1,13,mx,my,t);
          frame.draw_text(2,frame.height()-15,"val=%lf", drawing_colour,NULL,1,13,one3DSeq? In(mx,my,t):originalFrame(mx,my));
        }
      }
      screen.display(frame).wait(25);
      if (screen.key()==cimg::keyPAGEDOWN) { t=min(t+1,endfrm-1); screen.wait(225);}
      if (screen.key()==cimg::keyPAGEUP) { t=max(t-1,0); screen.wait(225);}
    }
  }

  return 0;

}



