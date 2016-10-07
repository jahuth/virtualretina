#include <retinatools/my_tools.h>
#include <retina.h>
#include<errno.h>

//#define CLOCKS_PER_SEC  1000000         shouldnt be necessary.


//Declaration. Code is contained in TestGanglionCell_main.cc
int TestGanglionCell_main(int argc, char **argv);


int main(int argc, char **argv)
{

  cimg_usage("Calculate the first-order frequency responses of a retina to shapley-victor-like stimuli. This experiment is designed to calibrate the contrast-gain-control properties of a retina in comparison with the physiological recordings of shapley-victor78. You can use (almost) all parameters from program TestGanglionCell, which is called by this program!\n\n GRATING PARAMETERS:\n");

  //Parameters for all gratings:

  const double T  = cimg_option("-T",10.0,"spatial period, in pixels");
  const int force_size  = cimg_option("-size",0,"spatial size of the grating, in pixels. By default, the retinal 'minimal size' is taken.. but you can force smaller sizes!");
  const double Lum  = cimg_option("-Lum",127.0,"mean luminance of the grating. Always positive!!");
  const int nPeriod   = cimg_option("-nP",5,"Number of simulation periods (for the smallest frequency of the set). The larger the number, the more precise the calculus of response amplitudes and gains.\n\n TEST CONTRAST PARAMETERS:\n");

  //Contrast parameters:

  const double lowCont  = cimg_option("-lCont",0.0125,"lowest tested contrast");
  const double highCont  = cimg_option("-hCont",0.1,"highest tested contrast");
  const double mulCont  = cimg_option("-mCont",2.0,"multiplicative step (=on log axes) between two consecutively tested contrasts.");
  const int set  = cimg_option("-set",0,"set of frequencies to be used. Leave at zero, unless YOU add one...\n\n (some of) the parameters from TestGanglionCell (had to add them here also...):\n");
  const bool disp = cimg_option("-disp",false,"if display of cell signals is wanted, for each tested contrast.");
  const char* savePath  = cimg_option("-outD",
#ifdef ROOTDIR
                                      ROOTDIR "/"
#endif
                                      "tmp","Output file for stocking experiment results");
  const int trials = cimg_option("-tr",0,"number of trials per stimulus. Set at 0 if you want to make your Fourier calculations from the bipolar->gang current (needs one trial only), rather than from averaged firing rates (needs many trials: much longer :-) ).");
  const char *retina_params  = cimg_option("-ret",
#ifdef ROOTDIR
                               ROOTDIR "/"
#endif
                               "test/retina_files/EXAMPLE_cat_X_cell.xml","path for the xml file that defines the type and parameters of the used retina.");
  const int chht = cimg_option("-q",3,"verbosity. 3=shut up, 2=opening credits only, 1=add simulation time, 0=add map statistics at each time step.");


  srand( (unsigned)time( NULL ) );


  std::string command, path(savePath),
  tempPath(
#ifdef ROOTDIR
		  ROOTDIR "/"
#endif
    "tmp");

  //*********************************General simulation parmeters *******************************


  //Tested Fequencies
  vector<double> freqSet;
  int numberFreq;

  if(set==0)
  {
    numberFreq=8;
    double f[8]={15,31,63,127,255,511,1023,2047};
    for(int i=0;i<8;++i)  freqSet.push_back(f[i]);
  }
  else
  {
    if(set==1)
    {
      numberFreq=2;
      double f[2]={1023,2047};
      for(int i=0;i<2;++i)  freqSet.push_back(f[i]);
    }
    else exit(0);    //pour l'instant
  }
  for(int i=0;i<numberFreq;++i) freqSet[i]/=65.526;


  //*****************loading a fake retina to get all interesting retinal parameters! **************

  xmlDocPtr doc=xmlParseFile(retina_params);
  if (doc==NULL)
    std::cerr<<"Retina xml file was not parsed successfully."<<std::endl;
  xmlNodePtr rootNode=xmlDocGetRootElement(doc);
  if (rootNode==NULL)
    std::cerr<<"Retina xml file was empty."<<std::endl;
  Retina DaRetina;
  xmlParam::add_object( DaRetina , "retina" );
  xmlParam::load (DaRetina , rootNode);
  // just to be clean
  xmlFreeDoc(doc);
  xmlParam::delete_structure();

  double step=DaRetina.step;
  int size= force_size? force_size : DaRetina.get_minimumSize();
  if(!(bool)DaRetina.contrastGainControl) cerr<<"WARNING: used retina does not have a contrast gain control stage. Not very interesting!..."<<endl;
  if(!DaRetina.ganglionLayers.size()) cerr<<"WARNING: used retina does not have any ganglion layers! Very likely leading to a bug..."<<endl;


  //******************** Deciding of stimulus dimensions ********************************************

  double stabilizedTime=1.5;	//this is a safe guess... after 1 second, the answer of the retina must be stabilized...
  double length=(double)nPeriod/freqSet[0];
  CImg<double> sequence(size,size);
  int x0= size/2, y0 =x0;
  int recordingSize= (int) (length/step), stabSize=(int)(stabilizedTime/step);
  int temporalSize=recordingSize+stabSize;

  int numberCont=(int)floor( log(highCont/lowCont)/log(mulCont) )+1;
  int indCont=0;
  int realTrials=(trials>0)? trials:1;

  //*************  Preparing options for program 'TestGanglionCell'... removing some options that will be dealt with diferently.

  // PROBLEM : all options which have different default valued for THIS program and for TestGanglionCell must be treated that way !!
  // example : '-ret' , '-r' (OUCH!)
  // source of unnumerable BUGS !!!

  string gCellOptions;
  string temp;
  for (int k=1; k<argc; k++)
    if( strcmp(argv[k],"-b") &&		//not a -b
        (strcmp(argv[k-1],"-b") || (argv[k][0]=='-')) && //not the string after a -b, unless its a new option.
        strcmp(argv[k],"-h") &&
        strcmp(argv[k],"-nS") &&		//not a -nS
        (strcmp(argv[k-1],"-nS") || (argv[k][0]=='-')) && //not the string after a -nS, unless its a new option.
        strcmp(argv[k],"-q") &&		//not a -q
        (strcmp(argv[k-1],"-q") || (argv[k][0]=='-'))  &&//not the string after a -q, unless its a new option.
        strcmp(argv[k],"-r") &&		//not a -r
        (strcmp(argv[k-1],"-r") || (argv[k][0]=='-')) && //not the string after a -r, unless its a new option.
        strcmp(argv[k],"-tr") &&		//not a -tr
        (strcmp(argv[k-1],"-tr") || (argv[k][0]=='-')) && //not the string after a -tr, unless its a new option.
        strcmp(argv[k],"-ret") &&	//not a -ret
        (strcmp(argv[k-1],"-ret") || (argv[k][0]=='-')) )//not the string after a -ret, unless its a new option.
    {temp=argv[k]; gCellOptions+=" "; gCellOptions+=temp;}
  //the 'TestGanglionCell' options by default
  gCellOptions+=" -b "+toString<double>(stabilizedTime)+" -nS 1 -r 1 -tr "+toString<int>(realTrials)+" -q "+toString<int>(chht)+" -ret "+string(retina_params)+" ";
  if(!disp) gCellOptions+=" -nodisp ";

  //*******************************  Saving CImgs: *******************************************

  CImg<double> sigCP, oplCP, bipCP, amCP, gangCP;

  CImgList<double> oplAmplitude(numberCont,numberFreq), gangAmplitude(numberCont,numberFreq);
  CImgList<double> oplPhaseDiff(numberCont,numberFreq), gangPhaseDiff(numberCont,numberFreq);


  //************************************************************************************************
  //************************ One call of TestGanglionCell for each tested contrast: ****************

  for(double cont=lowCont;cont<=highCont;cont*=mulCont)
  {

    cout<<endl<<"Processing contrast "<<cont<<"."<<endl;

    //********** Creating the input grating if necessary: **************************

    double A=cont*Lum;
    double cos_theta=1, sin_theta=0;
    vector<double> phi;
    for(int i=0;i<numberFreq;++i)
      phi.push_back(2*Pi*ranf());

    //Creating simulation frames... and loading values of input signal :-)
    cout<<"Creating simulation frames and saving all their names in a veeeeeeeeerry long string object."<<endl;

    string gratName=tempPath+"/tempGratingFrames_shapleyVictor";
    vector<string> allNames;	//Ouch!... Have to save all names because the shell cannot do its work. Tss!

    for(int t=0;t<temporalSize;++t)
    {
      sequence.fill(Lum);
      cimg_forXY(sequence,x,y)
      for(int i=0;i<numberFreq;++i)
        sequence(x,y)+= A *cos( (((x-x0)*cos_theta+(y-y0)*sin_theta)/T)*2*Pi) * sin(freqSet[i]*step*t*2*Pi+phi[i]);
      allNames.push_back(gratName+"_"+toString<int>(t,8)+".inr");
      sequence.save(allNames.back().c_str());
    }

    //stocking all file names...
    string inputFile=tempPath+"/tempInputNames_shapleyVictor.txt";
    fclose(fopen(inputFile.c_str(),"w"));
    ofstream ourg(inputFile.c_str());
    for(uint i=0;i<allNames.size();++i)
      ourg<<allNames[i]<<" ";
    ourg.close();

    //...and launching the test:
    cout<<"Launching the simulation, repeated "<<realTrials<<" times over "<<allNames.size()<<" frames of test grating."<<endl;

    command="TestGanglionCell";	//whatever argv[0]
    command+=" -i "+inputFile+" "+gCellOptions;


    //GANGLION CELL SIMULATION PROGRAM, NEW VERSION:

    extCommand ec ( command ) ;
    TestGanglionCell_main( ec.argc , ec.argv );



    //***********************  Simulation over. Loading the results *********************************************
    //Suppressing temporary files:
    cout<<"Simulation is over."<<endl;

    //if it was the last contrast test:
    if(indCont==numberCont-1)
    {
      cout<<"Destroying all simulation frames and the veeeeeeeeerry long string object."<<endl;

      //command=" rm "+gratName+"*.inr";	//unfortunately, can become too long for the shell (./sh)
      for(uint i=0;i<allNames.size();++i)
      {
        command=" rm "+allNames[i];
        system(command.c_str());
      }
      command=" rm "+inputFile;
      system(command.c_str());
    }

    //For ganglion signal, we load, whether the spike train, whether just the ganglion current.
    sigCP.load((path+"/inputCP.inr").c_str()); sigCP.crop(stabSize,sigCP.size()-1);
    oplCP.load((path+"/oplCP.inr").c_str()); oplCP.crop(stabSize,oplCP.size()-1);
    if(DaRetina.contrastGainControl)	//better be!...
    {bipCP.load((path+"/bipolarCP.inr").c_str()); bipCP.crop(stabSize,bipCP.size()-1);
      amCP.load((path+"/cgc_adaptationCP.inr").c_str()); amCP.crop(stabSize,amCP.size()-1);}
    if(trials==0)
    {
      cout<<"loading the bipolar->ganglion current as ganglion signal."<<endl;
      gangCP.load((path+"/ganglionCP.inr").c_str());
    }
    else
    {
      cout<<"loading the averaged ganglion firing rate as ganglion signal."<<endl;
      gangCP.load((path+"/firingRate.inr").c_str());
    }
    gangCP.crop(stabSize, gangCP.size()-1);
    
    //Properties of the different answers:
    cout<<"OPL signal characteristics (should have values around unity): "<<endl;
    CImg<double> stats = oplCP.get_stats();
    cout<<"  min  "<<stats(0)<<"  max  "<<stats(1)<<"  mean  "<<stats(2)<<"  sigma  "<<pow(stats(3), 0.5)<<endl;
    if(DaRetina.contrastGainControl)	//better be there!...
    {
        cout<<"Bipolar signal characteristics (should have values around unity): "<<endl;
        stats=bipCP.get_stats();
        cout<<"  min  "<<stats(0)<<"  max  "<<stats(1)<<"  mean  "<<stats(2)<<"  sigma  "<<pow(stats(3), 0.5)<<endl;
        cout<<"Fast adaptation shunt conductance characteristics (in Hz): "<<endl;
        stats=amCP.get_stats();
        cout<<"  min  "<<stats(0)<<"  max  "<<stats(1)<<"  mean  "<<stats(2)<<"  sigma  "<<pow(stats(3), 0.5)<<endl;
    }
    cout<<"Ganglion signal (firing rate if trials>0) characteristics: "<<endl;
    stats=gangCP.get_stats();
    cout<<"  min  "<<stats(0)<<"  max  "<<stats(1)<<"  mean  "<<stats(2)<<"  sigma  "<<pow(stats(3), 0.5)<<endl;

    //Fourier analyse... TRYING bigger number of loops !...
    
    //cout<<"original signal "<<endl;
    //sigCP.print();
    CImgList<double> resultSig=coAnalyseFourierBis(sigCP,freqSet,step);
    
    //cout<<"opl signal "<<endl;
    //oplCP.print();
    CImgList<double> resultOPL=coAnalyseFourierBis(oplCP,freqSet,step);
    
    cout<<"ganglion signal "<<endl;
    gangCP.print();
    CImgList<double> resultGang=coAnalyseFourierBis(gangCP,freqSet,step);
    
    //for(int i=0 ; i< numberFreq ; i++)
	    //cout<<"YAA "<<resultGang[0](i)<<"  "<<resultGang[1](i)<<endl;
    
    
    
    //************ Stocking amplitude, phase_difference_with_input **********************************************

    oplPhaseDiff[indCont]= (resultOPL[1]-resultSig[1])/Pi;
    gangPhaseDiff[indCont]= (resultGang[1]-resultSig[1])/Pi;

    for(int i=0;i<numberFreq;++i)
    {
      oplAmplitude[indCont](i) = resultOPL[0](i) ;
      gangAmplitude[indCont](i) = resultGang[0](i) ;
    }

    
    /*for(int i=0;i<numberFreq;++i)
    {
	    vector<double> res = analyseFourier(sigCP, freqSet[i], step ) ;
	    double phiSig = res[3];
	    
	    res = analyseFourier(oplCP, freqSet[i], step ) ;
	    oplAmplitude[indCont](i) = res[2] ;
	    oplPhaseDiff[indCont](i)= (res[3]-phiSig)/Pi;
	    
	    res = analyseFourier(gangCP, freqSet[i], step ) ;
	    gangAmplitude[indCont](i) = res[2] ;
	    gangPhaseDiff[indCont](i)= (res[3]-phiSig)/Pi;
  }*/
    
    ++indCont;

  }

  cout<<"SIMULATIONS OVER! Finalizing and saving data"<<endl;

  //Getting all start points for phases together
  CImg<double> goodStartPoint(numberCont);
  for(int i=0;i<numberCont;++i) goodStartPoint(i)=oplPhaseDiff[i](0);
  phaseSmooth(goodStartPoint,2);
  for(int i=0;i<numberCont;++i) oplPhaseDiff[i](0)=goodStartPoint(i);
  for(int i=0;i<numberCont;++i) goodStartPoint(i)=gangPhaseDiff[i](0);
  phaseSmooth(goodStartPoint,2);
  for(int i=0;i<numberCont;++i) gangPhaseDiff[i](0)=goodStartPoint(i);


  //Saving results in files...
  string oplPath, gangPath;
  ofstream OPLFile, GangFile;
  oplPath=path+"/OPLResults.txt";
  gangPath=path+"/GanglionResults.txt";
  OPLFile.open(oplPath.c_str());
  GangFile.open(gangPath.c_str());
  for(int indCont=0;indCont<numberCont;++indCont)
  {
    phaseSmooth(oplPhaseDiff[indCont],2.0);
    phaseSmooth(gangPhaseDiff[indCont],2.0);
    for(int freq=0;freq<numberFreq;++freq)
    {
      OPLFile<<lowCont*pow(mulCont,indCont)<<" "<<freqSet[freq]<<" "<<oplAmplitude[indCont](freq)<<" "<<oplPhaseDiff[indCont](freq)<<endl;
      GangFile<<lowCont*pow(mulCont,indCont)<<" "<<freqSet[freq]<<" "<<gangAmplitude[indCont](freq)<<" "<<gangPhaseDiff[indCont](freq)<<endl;
    }
  }
  OPLFile.close(), GangFile.close();

  //And displaying
  CImg<double> figAmpOPL(300,300,1,3,255);
  CImg<double> figAmpGang(300,300,1,3,255);
  CImg<double> figPhaseOPL(300,300,1,3,255);
  CImg<double> figPhaseGang(300,300,1,3,255);

  CImg<double> freqCImg (numberFreq);
  for(int i=0 ; i<numberFreq ; i++)
    freqCImg(i) = freqSet[i] ;

  CImgSignalDrawer sd; sd.style=0; sd.width=2;
  //sd.axisX.begin=log10(lowFreq); sd.axisX.end=log10(highFreq); sd.axisX.type=1; //print 10^(values)
  sd.axisX.begin=freqCImg(0); sd.axisX.end=freqCImg(numberFreq-1); sd.axisX.type=1; // log10(values)
  sd.axisY.type=1; //print 10^(values)
  sd.draw(figAmpOPL, oplAmplitude , 0 , &freqCImg);
  sd.draw(figAmpGang, gangAmplitude , 0 , &freqCImg );

  sd.axisY.type=0; //phases are represented linearly
  sd.draw(figPhaseOPL, oplPhaseDiff , 0 , &freqCImg );
  sd.draw(figPhaseGang, gangPhaseDiff , 0 , &freqCImg );

  CImgDisplay dispOPL(figAmpOPL,"Amplitudes of OPL signal");
  CImgDisplay dispGang(figAmpGang,"Amplitudes of Ganglion signal");
  CImgDisplay dispPhaseOPL(figPhaseOPL,"Phase Differences of OPL signal (should be one curve only)");
  CImgDisplay dispPhaseGang(figPhaseGang,"Phase Differences of Ganglion signal");

  while(!dispGang.is_closed()) dispGang.wait();

  return 0;
}

