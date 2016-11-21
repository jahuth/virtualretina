
#include <retina.h>
#include <fixationalmovement.h>

#include <time.h>
#include <sys/stat.h>

#ifdef USE_HDF5
#include "mvaspike_hdf5.h"
#endif


// Small helper function to always save absolute path names
string get_absoluteVersion(string daPath, string daPWD)
{
  if(!daPath.empty())
    if(daPath.compare(0,1,"/")) // doesnt start with a "/" => daPath is a relative path ;)
      return daPWD+"/"+daPath;
  return daPath;
}


int Retina_main(int argc, char **argv)
{

  cimg_usage("Simulate the response of a model retina to an input sequence.\n\t\tUsage : Retina [options] sequence.inr [or image_in*, or -i input_file] [...] \n\n INPUT PARAMETERS :");


  const char *retina_params  = cimg_option("-ret",
#ifdef ROOTDIR
                               ROOTDIR "/"
#endif
                               "test/retina_files/EXAMPLE_primate_ParvoMagno.xml",
                               "path to the xml file that defines the type and parameters of the used retina.");

  const int N_rep    = cimg_option("-r",1,"number of repetitions of each frame. Hence each frame lasts N_rep*step seconds. Set to 0 if you just want to create a retina, not test it on a sequence.");
  const int chht = cimg_option("-q",1,"verbosity. 3=shut up, 2=opening credits only, 1=add simulation time, 0=add map statistics at each time step.");
  const double initValue = cimg_option("-initVal",HUGE_VAL,"luminance of the uniform screen which is supposed to have been `watched' by the retina for a long time, prior to simulation. If initVal = HUGE_VAL (default), initVal is automatically taken as : input_luminance_range/2");
  const bool nodisp = cimg_option("-nodisp",false,"if no display of intermediate maps is wanted");
  const char *inputFile = cimg_option( "-i" , "noFile" , "optional path to a text file in which all input frame NAMES(with path) are stocked. Necessary to load very big sequences, when the shell stack cant follow anymore. Must contain all the file names, separated by spaces or returns, and only the file names.");
  const int magnif  = cimg_option("-f",1,"(optional) integer magnification factor on the input image before feeding it to the retina");
  const int beginning_frame   = cimg_option("-bf",0,"beginning frame of input sequence");
  const int end_frame   = cimg_option("-ef",0,"ending frame of input sequence (not included)\n\nPOSITION :");

  const double xx   = cimg_option("-x",0.0,"x of center pixel of retina. If untouched, the retina is centered on the image");
  const double yy   = cimg_option("-y",0.0,"y of center pixel of retina.\n\nSAVING PARAMETERS :");

  const char *savePath = cimg_option( "-outD" ,
#ifdef ROOTDIR
                                      ROOTDIR "/"
#endif
                                      "tmp" , "directory where simulation files will be saved");

#ifdef USE_HDF5
  const bool saveHDF5 = cimg_option("-hdf5",false,"if you want to save the retinal spikes in the hdf5 format.");
#endif

  const bool saveMap = cimg_option("-savemap",false,"if you want all intermediate maps of the retina to be saved.");
  const char *mapFormat = cimg_option( "-format" , "inr" , "format used to save the maps if option savemap is on. Only 'inr' works for the moment!!!");
  const bool saveCP = cimg_option("-saveCP",false,"if you want to save all signals at Center Pixel.");
  const int savestep = cimg_option("-nS",1,"Saving step for intermediate/output maps of the retina (option does NOT affect ouput spikes). Values for the diferent layers of cells are saved every 'nS' time steps (use it to reduce the size of your saved output maps).");


  
  
  srand( (unsigned)time( NULL ) );
  
  //STEP 1 - Loading input - hey, thanks to CImg's inrcast program fot the following code.

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
    if(chht<3) {std::fprintf(stderr,"- Load images...\t"); std::fflush(stderr);}
    bool opt = false;
    for (int k=1; k<argc; k++)
      if (argv[k][0]=='-' && argv[k][1]!='\0') opt=true;
      else
      {
        if (!opt ||
            !cimg::strcasecmp(argv[k-1],"-h") ||
            !cimg::strcasecmp(argv[k-1],"-savemap") ||
            !cimg::strcasecmp(argv[k-1],"-saveCP") ||
            !cimg::strcasecmp(argv[k-1],"-nodisp")
           )
        {
          if(chht==0) {std::fprintf(stderr,"\n\t> Loading [%d]='%s'\t",k-1,argv[k]); std::fflush(stderr);}
          inputNames.push_back(argv[k]);
        }
        opt=false;
      }
    std::fputc('\n',stderr);
  }

  if (!inputNames.size())
  {
    std::fprintf(stderr,"\n** You must specify at least one input image\n** Try '%s -h' to get help\n**\n",cimg::basename(argv[0]));
    exit(0);
  }

  if(chht<3) cout<<"Hello! This is the retina simulation program!"<<endl;

  CImg<double> In(inputNames[0].c_str());
  if (magnif != 1)
    In.resize( In.width()*magnif , In.height()*magnif , In.depth() , In.spectrum() );
  int s_sx = In.width()  , s_sy = In.height() ;
  int beginfrm=0,endfrm=In.depth();
  bool one3DSeq=(endfrm>1);

  if(one3DSeq)
  {
    if(chht<3)
      cout<<"Input is the 3-dimensional sequence "<<inputNames[0]
      <<". Following input image files, if they exist, will not be taken into account."<<endl;
  }
  else
    endfrm=inputNames.size();

  if (beginning_frame<end_frame&&beginning_frame<endfrm)
  {
    beginfrm=(beginning_frame>beginfrm)?beginning_frame:beginfrm;
    endfrm=(end_frame<endfrm)? end_frame:endfrm;
  }
  int input_size=(endfrm-beginfrm)*N_rep;
  if(!one3DSeq&(chht<3)) 
    cout<<"Input is the list of 2-dimensional images, starting at "
    <<inputNames[beginfrm]<<" and finishing at "<<inputNames[endfrm-1]<<"."<<endl;


  //STEP 2- Parsing xml file...
  xmlDocPtr doc=xmlParseFile(retina_params);
  if (doc==NULL)
    std::cerr<<"Retina xml file was not parsed successfully."<<std::endl;
  xmlNodePtr rootNode=xmlDocGetRootElement(doc);
  if (rootNode==NULL)
    std::cerr<<"Retina xml file was empty."<<std::endl;

  //STEP 3 - Creating and loading the retina:
  Retina DaRetina;
  xmlParam::add_object(DaRetina,"retina");
  xmlParam::load(DaRetina,rootNode);

  //Uncomment this ifever you wish to represent the bip->gang transmission curves:
  //for(uint chann=0;chann<DaRetina.ganglionLayers.size();++chann)
  //  DaRetina.ganglionLayers[chann]->inputCurrent.inputSynapse().draw_transmission( -5, 5, colorSerie(chann) );


  //STEP 4 - HANDLING INPUT IMAGE:

  //Looking for possible fixational movements, for required center of fixation, and adapting the size of retinal input image in consequence...
  FixationalMovementGenerator* eyeMov=0;
  xmlParam::add_object(eyeMov,"");
  xmlParam::Register< 1, BasicMicrosaccadeGenerator> (eyeMov,"basic-microsaccade-generator");
  xmlParam::load(eyeMov,rootNode);

  //Fixing these two parameters directly from the Retina:
  if(eyeMov)
  {
    eyeMov->step = DaRetina.step ;
    eyeMov->pixPerDegree = DaRetina.pixels_per_degree ;
  }

  double border=eyeMov? eyeMov->typicalVariance_pix():0.0;
  if(eyeMov)
  {
    eyeMov->centerPointX_pix = border; eyeMov->centerPointY_pix = border;
  }
  // center of fixation (cx,cy):
  double cx= (xx>0)? xx:s_sx/2.0, cy=(yy>0)? yy:s_sy/2.0;

  int inputDimX=(int)floor(2*(cx-border)), inputDimY=(int)floor(2*(cy-border));
  if(chht<3)
    cout<<"Given your input images, center of fixation and chosen eye movements, "
      "the maximum possible size for retinal maps is ("<<inputDimX<<","
      <<inputDimY<<") pixels. Spiking arrays exceeding this size will be "
      "automatically reduced."<<endl;
  int leftX=(int)ceil(border), leftY=(int)ceil(border);


  //STEP 3 bis - we can now finish to load our retina, knowing its input size !
  
  DaRetina.adjustSizes(inputDimX,inputDimY);
  DaRetina.allocateValues( chht<3 , initValue );


  //STEP 5 - Creating the PulseCoupled, connecting all units to it:

  PulseCoupled retiti(DaRetina.step);
  for(uint chann=0;chann<DaRetina.ganglionLayers.size();++chann)
    if(DaRetina.ganglionLayers[chann]->spikingChannel)
    {
      for(uint un=0;un<DaRetina.ganglionLayers[chann]->spikingChannel->units.size();un++)
        DaRetina.ganglionLayers[chann]->spikingChannel->units[un]->add_in_coupled( retiti );
    }


  //STEP 6 - Creating the MVASpike simulator and recorder.
  
  Root simulator;

#ifdef USE_HDF5
  HDF5MemSpikeRecorder rec;
#else
  MemSpikeRecorder rec;
#endif

  rec.set_id_mode(0);         //not useful, in fact, but remember this possibility exists...
  simulator.set_system(retiti);
  simulator.record(rec);
  simulator.init();


  //STEP 7 - Preparing for SAVING AND DISPLAYING :
  //(long and boring...)
  
  struct stat st; // Check for existence of saving directory
  if(stat(savePath,&st) != 0)
  {
    std::fprintf(stderr,"\n** Error: your required output directory (option -outD, "
      "or by default \"tmp/\") does not exist.\n");
    exit(0);
  }
       
  string retinadata(savePath), simdata(savePath), spikes(savePath), 
         spikesHDF5(savePath), receptorpath(savePath), horizontalpath(savePath),
         oplpath(savePath), bipolarpath(savePath), amacrinepath(savePath), contrcondpath(savePath), command;
  vector<string> ganglionpaths;

  string extension(mapFormat);
  if (extension[0]!='.')  
    extension="."+extension;

  retinadata+="/retina.xml"; 
  simdata+="/simulation.txt";
  spikesHDF5+="/spikes.hdf5";
  spikes+="/spikes.spk";


  if(saveMap)
  {
    receptorpath+="/receptorFrames/";		//creating a temporary output file for receptor frames
    command="mkdir "+receptorpath;
    system(command.c_str());
    receptorpath+="receptor_signal"+extension;

    if( DaRetina.outerPlexiformLayer->wSurround )
    {
      horizontalpath+="/horizontalFrames/";	//creating a temporary output file for horizontal frames
      command="mkdir "+horizontalpath;
      system(command.c_str());
      horizontalpath+="horizontal_signal"+extension;
    }

    if(DaRetina.contrastGainControl)
    {
      bipolarpath+="/bipolarFrames/";		//creating a temporary output file for bipolar frames
      command="mkdir "+bipolarpath;
      system(command.c_str());
      bipolarpath+="bipolar_signal"+extension;
      amacrinepath+="/cgc_adaptationFrames/";		//creating a temporary file for amacrine frames
      command="mkdir "+amacrinepath;
      system(command.c_str());
      amacrinepath+="cgc_adaptation_signal"+extension;
      attenuationpath+="/cgc_attenuationFrames/";    //creating a temporary file for amacrine frames
      command="mkdir "+attenuationpath;
      system(command.c_str());
      attenuationpath+="cgc_attenuation_signal"+extension;
    }
  }

  //Special treatment for OPL signal... in case it's already the output of the retina!
  if(saveMap || !DaRetina.ganglionLayers.size() )
  {
    oplpath+="/oplFrames/";	//creating a temporary output file for 'opl current' frames
    command="mkdir "+oplpath;
    system(command.c_str());
    oplpath+="opl_signal"+extension;
  }

  //Special treatment for Ganglion layers... that can sometimes become an output to save even without command saveMap.
  //My my my, this is REALLY boring!
  for(uint chann=0;chann<DaRetina.ganglionLayers.size();chann++)
  {
    string tempStr(savePath);
    tempStr+=("/ganglion_Layer_"+toString(chann)+"/");
    ganglionpaths.push_back(tempStr);
    if(saveMap || !(DaRetina.ganglionLayers[chann]->spikingChannel))
    {
      command="mkdir "+ganglionpaths[chann];
      system(command.c_str());
    }
    ganglionpaths[chann]+="ganglion_signal"+extension;
  }

  //This is in the special case where SPIKE-FREQUENCY ADAPTING cells were asked for:
  bool adapCell=false;
  string adapPath(savePath); adapPath+="/adapting_cond"+extension;
  if( DaRetina.ganglionLayers.size() &&
      dynamic_cast<ret_SpikingChannel_adap*>(DaRetina.ganglionLayers[0]->spikingChannel) )
    {
      cout<<"We are creating at least one cell with Spike-Frequency Adaptation."<<endl
          <<"The value of the adaptation conductance for the first cell of the first "
            "spiking array (not necessarily central cell) will be saved as "<<adapPath<<"."<<endl;
      adapCell=true;
    }

  //Center Pixel
  CImg<double> inputCP, receptorCP, horizontalCP, oplCP, bipolarCP, amacrineCP, ganglionCP, adapSignal;
  if(saveCP)
  {
    receptorCP=CImg<double>(  ((endfrm-beginfrm)*N_rep - 1) / savestep + 1);
    inputCP=CImg<double>(receptorCP); horizontalCP=CImg<double>(receptorCP); oplCP=CImg<double>(receptorCP); bipolarCP=CImg<double>(receptorCP); amacrineCP=CImg<double>(receptorCP); ganglionCP=CImg<double>(receptorCP);

    if(adapCell)
      adapSignal=CImg<double>(receptorCP);
  }


  //Saving the newly constructed retina, including all unit positions and MVASpike IDs:
  xmlFreeDoc(doc);
  doc=xmlNewDoc((const xmlChar*)"1.0");
  rootNode=xmlNewNode(NULL,(const xmlChar*)"retina-description-file");
  xmlDocSetRootElement(doc,rootNode);
  xmlParam::save(eyeMov,rootNode);
  xmlParam::save(DaRetina,rootNode);
  xmlSaveFormatFile(retinadata.c_str(),doc,1);
  // just to be clean
  xmlFreeDoc(doc);
  xmlParam::delete_structure();


  //Writing the simulation data file :

  //We use function my_pwd() to get current directory...
  // Function my_pwd() is ugly and long, so shouldn't be used too often!
  string currentDir=my_pwd();
  // TODO : remove unnecessary /../ and /./ from path names. See e.g. boost/filesystem
  
  fclose(fopen(simdata.c_str(),"w")); //creates the file if it did not exist (ugly??)
  ofstream output(simdata.c_str());
  
  output<<"#Tested_sequence: ";
  if(one3DSeq)
      output<<get_absoluteVersion(inputNames[0],currentDir);
  else 
     for(uint i=0;i<inputNames.size();++i) 
         output<<get_absoluteVersion(inputNames[i]+" ",currentDir);
  output<<endl;
  output<<"#Used_retina: "<<get_absoluteVersion(retinadata,currentDir)<<endl;
  output<<"#Frame_repetition: "<<N_rep<<endl;
  output<<"#Beginning_frame: "<<beginfrm<<endl;
  output<<"#End_frame: "<<endfrm<<endl;
  output<<"#Corresponding_spike_train: "<<get_absoluteVersion(spikes,currentDir)<<endl;
  output<<"#Center_of_retina: "<<endl; //this format allows on-the-fly moves of the retina.
  output<<0.0<<" "<<leftX+inputDimX/2.0<<" "<<leftY+inputDimY/2.0<<endl;


  //DISPLAYS:
  CImgDisplay *DispRec, *DispHoriz, *DispOPL, *DispBip, *DispAm, *DispGang;
  //Special treatment for Ganglion layers... aaaaaah, ganglion cells!...
  CImgList<double> gangList(DaRetina.ganglionLayers.size(), DaRetina.sizeX, DaRetina.sizeY,1,1,0.0);

  //We initialize the pointers only if display of maps is required
  if( !nodisp )
  {
    DispRec = new CImgDisplay(DaRetina.sizeX, DaRetina.sizeY,"Receptor Layer",1,false);
    if( DaRetina.outerPlexiformLayer->wSurround )
      DispHoriz = new CImgDisplay(DaRetina.sizeX, DaRetina.sizeY,"Horizontal Layer",1,false);
    DispOPL = new CImgDisplay(DaRetina.sizeX, DaRetina.sizeY,"OPL current",1,false);
    if(DaRetina.contrastGainControl)
    {
      DispBip = new CImgDisplay(DaRetina.sizeX, DaRetina.sizeY,"Bipolar Layer",1,false);
      DispAm = new CImgDisplay(DaRetina.sizeX, DaRetina.sizeY,"Fast Adaptation Layer",1,false);
    }
    if(DaRetina.ganglionLayers.size())
    {
      DispGang = new CImgDisplay(1,1,"",1,false,true);
      DispGang->assign(gangList,"Ganglion input currents (one per layer)",1,false);
    }
  }
  
  // STEP 8 - HERE WE GO ! SIMULATION BEGINS.


  if(chht<3)
  {
    cout<<"Retina data, including position of units, has been saved as : "<<retinadata<<endl<<endl;
    cout<<"Retina will now be tested on "<<endfrm-beginfrm<<" frames of the input sequence, each frame lasting "
        <<N_rep*DaRetina.step<<" theoretical seconds."<<endl;
  }

  //Little indicators of calculation time :
  clock_t linear_time=0,network_time=0,last_time=clock();  //gettimeofday()

  //SIMULATION LOOP:

  CImg<double>* Seq=new CImg<double>(inputDimX,inputDimY);
  CImg<double>* Seq_save = eyeMov? new CImg<double>(inputDimX,inputDimY) : Seq;

  for (int t=0;t<endfrm-beginfrm;t++)
  {
    if(chht<2)
      cout<<"Frame "<<beginfrm+t<<endl;

    if(one3DSeq)
    {
      if(eyeMov==0)
        cimg_forXY(*Seq,x,y)
          (*Seq)(x,y)=In(leftX+x,leftY+y,beginfrm+t);
    }
    else
    {
      Seq_save -> load(inputNames[beginfrm+t].c_str()).resize( s_sx , s_sy , 1 , 1 );
      if(eyeMov==0) Seq->crop(leftX,leftY,leftX+inputDimX-1,leftY+inputDimY-1);
    }

    for(int i=0;i<N_rep;i++)
    {
      int tim=(t*N_rep+i);
      if(eyeMov)
      {
        vector<double> newCorner=eyeMov->getNextPos_pix();
        leftX=(int)ceil(newCorner[0]), leftY=(int)ceil(newCorner[1]);

        if (leftX<0) {cout<<"WARNING: microsaccade reached left edge of image"<<endl; leftX=0;}
        if (leftY<0) {cout<<"WARNING: microsaccade reached upper edge of image"<<endl; leftY=0;}
        if (leftX+inputDimX>s_sx) {cout<<"WARNING: microsaccade reached right edge of image"<<endl; leftX=s_sx-inputDimX;}
        if (leftY+inputDimY>s_sy) {cout<<"WARNING: microsaccade reached lower edge of image"<<endl; leftY=s_sy-inputDimY;}
        output<<tim*DaRetina.step<<","<<leftX+inputDimX/2.0<<","<<leftY+inputDimY/2.0<<endl;    //Saccades are followed, for possible reconstruction (not used for the moment)

        if(one3DSeq)
          cimg_forXY(*Seq,x,y)
            Seq->at(x,y)=In(leftX+x,leftY+y,beginfrm+t);
        else
          *Seq= Seq_save->get_crop(leftX,leftY,leftX+inputDimX-1,leftY+inputDimY-1);
        //Seq->display();
      }

      DaRetina.feedInput(*Seq);                 //first important lines
      DaRetina.tempStep();

      linear_time+=clock()-last_time;
      last_time=clock();
      simulator.run((tim+0.5)*DaRetina.step);   //second important line
      network_time+=clock()-last_time;
      last_time=clock();

      //Here comes all the boring SAVING/DISPLAYING of maps !!!!

      if(tim%savestep==0)
      {

        if(chht<2)
          cout<<"Simulation at time "<<tim*DaRetina.step<<" seconds."<<endl;

        // MAP SAVING . Thanks, CImg!!
        if(saveMap)
        {
          DaRetina.outerPlexiformLayer->receptorSignal->read().save(receptorpath.c_str(),tim/savestep);
          if( DaRetina.outerPlexiformLayer->wSurround )
            DaRetina.outerPlexiformLayer->horizontalSignal->read().save(horizontalpath.c_str(),tim/savestep);
          DaRetina.outerPlexiformLayer->outputSignal->read().save(oplpath.c_str(),tim/savestep);
          if(DaRetina.contrastGainControl)
          {
            DaRetina.contrastGainControl->bipolarSignal->read().save(bipolarpath.c_str(),tim/savestep);
            DaRetina.contrastGainControl->adaptationSignal->read().save(amacrinepath.c_str(),tim/savestep);
	    DaRetina.contrastGainControl->bipolarSignal->attenuationMap->read().save(attenuationpath.c_str(),tim/savestep);
          }
          for(uint chann=0;chann<DaRetina.ganglionLayers.size();chann++)
            DaRetina.ganglionLayers[chann]->inputCurrent.read().save(ganglionpaths[chann].c_str(),tim/savestep);
        }
        //Saving a retinal output in spikeless/ganglionless cases:
        if(!DaRetina.ganglionLayers.size() & !saveMap)
          DaRetina.outerPlexiformLayer->outputSignal->read().save(oplpath.c_str(),tim/savestep);
        for(uint chann=0;chann<DaRetina.ganglionLayers.size();chann++)
          if(!DaRetina.ganglionLayers[chann]->spikingChannel & !saveMap)
            DaRetina.ganglionLayers[chann]->inputCurrent.read().save(ganglionpaths[chann].c_str(),tim/savestep);

        //SAVING CENTRAL SIGNAL, if required (e.g. by program TestGanglionCell)
        if(saveCP)
        {
          inputCP(tim/savestep)=DaRetina.input.read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
          receptorCP(tim/savestep)=DaRetina.outerPlexiformLayer->receptorSignal->read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
          if( DaRetina.outerPlexiformLayer->wSurround )
            horizontalCP(tim/savestep)=DaRetina.outerPlexiformLayer->horizontalSignal->read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
          oplCP(tim/savestep)=DaRetina.outerPlexiformLayer->outputSignal->read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
          if(DaRetina.contrastGainControl)
          {
            bipolarCP(tim/savestep)=DaRetina.contrastGainControl->bipolarSignal->read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
            amacrineCP(tim/savestep)=DaRetina.contrastGainControl->adaptationSignal->read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
          }
          if(DaRetina.ganglionLayers.size())
            //only first channel, I'm tired of useless saves.
            ganglionCP(tim/savestep)=DaRetina.ganglionLayers[0]->inputCurrent.read()(DaRetina.sizeX/2,DaRetina.sizeY/2);
          if(adapCell)
          {
            ret_SpikingChannel_adap::ret_OneCell_adap * first_cell=
              dynamic_cast< ret_SpikingChannel_adap::ret_OneCell_adap * > 
              ( DaRetina.ganglionLayers[0]->spikingChannel->units[0] );
            adapSignal(tim/savestep)=first_cell->get_inputValue( first_cell->autoInh_CellPort );
          }
        }

        //BLAH BLAHS , if required
        if(chht==0)
        {
          cout<<"Statistics for all maps at this point: "<<endl;
          cout<<"Receptor signal:"<<endl;
          DaRetina.outerPlexiformLayer->receptorSignal->read().print() ;
          if( DaRetina.outerPlexiformLayer->wSurround )
          {
            cout<<"Horizontal cells signal:"<<endl;
            DaRetina.outerPlexiformLayer->horizontalSignal->read().print() ;
          }
          cout<<"OPL signal (should have values of order unity):"<<endl;
          DaRetina.outerPlexiformLayer->outputSignal->read().print();
          if(DaRetina.contrastGainControl)
          {
            cout<<"POTENTIAL of Bipolar cells (should have values of order unity):"<<endl;
            DaRetina.contrastGainControl->bipolarSignal->read().print();
            cout<<"Fast adaptation shunt conductance in Bipolar:"<<endl;
            DaRetina.contrastGainControl->adaptationSignal->read().print();
          }
          cout<<"Ganglion cell input currents:"<<endl;
          for(uint chann=0;chann<DaRetina.ganglionLayers.size();chann++)
          {
            cout<<"Layer "<<chann<<":"<<endl;
            DaRetina.ganglionLayers[chann]->inputCurrent.read().print();
          }
          cout<<endl;
        }

        //DISPLAYS , if required
        if(!nodisp)
        {
          DispRec -> display(DaRetina.outerPlexiformLayer->receptorSignal->read());
          if( DaRetina.outerPlexiformLayer->wSurround )
            DispHoriz -> display(DaRetina.outerPlexiformLayer->horizontalSignal->read());
          DispOPL -> display(DaRetina.outerPlexiformLayer->outputSignal->read());
          if(DaRetina.contrastGainControl)
          {
            DispBip -> display(DaRetina.contrastGainControl->bipolarSignal->read());
            DispAm -> display(DaRetina.contrastGainControl->adaptationSignal->read());
          }
          //Poignant chant of ganglion cells in the mist of an autumn sunset...
          for(uint chann=0;chann<DaRetina.ganglionLayers.size();chann++)
          {//manually making the front image 'not shared' anymore, then removing it from the list:
            gangList.pop_front();
            gangList.insert(DaRetina.ganglionLayers[chann]->inputCurrent.read(), ~0U, true);
          }
          if(DaRetina.ganglionLayers.size()) DispGang -> display(gangList);
        }

        //That's all for each time step !!
      }
    }
  }


  //simulation almost over, but, just for things to be RIGOUROUS:
  simulator.run(input_size*DaRetina.step);
  network_time+=clock()-last_time;


  //STEP 9 - SIMULATION OVER! - Closing the shop.
  output.close();
  if(chht<3)
  {
    cout<<endl<<"Simulation completed !"<<endl;
    cout<<"There have been "<<(int)(linear_time/CLOCKS_PER_SEC)<<" seconds of linear calculations, and "<<(int)(network_time/CLOCKS_PER_SEC)<<" seconds of network calculations. "<<endl;
  }

  bool wereSpikes=false;
  for(uint chann=0;chann<DaRetina.ganglionLayers.size();chann++)
    wereSpikes|=(DaRetina.ganglionLayers[chann]->spikingChannel != 0);


  if( wereSpikes )
  {
#ifdef USE_HDF5
    if(saveHDF5)
    {
      if(chht<3) cout<<"Saving spikes to hdf5 format, as "<<spikesHDF5<<endl;
      rec.save_to_hdf5((char*)spikesHDF5.c_str());
    }
#endif
    if(chht<3) cout<<"Saving spikes to ASCII format, as "<<spikes<<endl;
    rec.save((char*)spikes.c_str());
  }

  if(saveCP)
  {
    string tempStr(savePath); tempStr+="/inputCP.inr"; inputCP.save(tempStr.c_str());
    tempStr=string(savePath); tempStr+="/receptorCP.inr"; receptorCP.save(tempStr.c_str());
    tempStr=string(savePath); tempStr+="/horizontalCP.inr"; horizontalCP.save(tempStr.c_str());
    tempStr=string(savePath); tempStr+="/oplCP.inr"; oplCP.save(tempStr.c_str());
    tempStr=string(savePath); tempStr+="/bipolarCP.inr"; bipolarCP.save(tempStr.c_str());
    tempStr=string(savePath); tempStr+="/cgc_adaptationCP.inr"; amacrineCP.save(tempStr.c_str());
    tempStr=string(savePath); tempStr+="/ganglionCP.inr"; ganglionCP.save(tempStr.c_str());
    if(adapCell)
      adapSignal.save( adapPath.c_str() );
  }

  if(chht<3)
  {cout<<"The main data file for this simulation is : "<<simdata<<endl;}


  if( !nodisp )
  {
    cout<<"Close \"Receptor Layer\" display to exit (or violently press Ctrl-C)."<<endl;
    cout<<"(Use option -nodisp for a simulation without displays.)"<<endl<<endl;
    while ( !DispRec -> is_closed() )
      DispRec -> wait();
  }

  return 0;
}

