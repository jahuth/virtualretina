
#include<retinatools/my_tools.h>
#include<reconstruct.h>
#include<errno.h>


//Declaration. Code is contained in Retina_main.cc
int Retina_main(int argc, char **argv);


int TestGanglionCell_main(int argc, char **argv)
{

  cimg_usage("Compute and save the averaged firing rate of a single ganglion cell. \n\t\tusage : TestGanglionCell [options] image_in1 [or image_in*] [...] \n\n All same options as program Retina (see after) , except: ");

  const bool nodisp = cimg_option("-nodisp",false,"if no display of signals is wanted.\n\nplus, SPECIFIC PARAMETERS:");

  const int trials = cimg_option("-tr",1,"number of trials.");
  const int mva_id = cimg_option("-id",0,"MVASpike id of the cell to reconstruct. Should be zero by default...");
  const int process = cimg_option("-k",1,"kernel used to reconstruct the instantaneous firing rate : uniform step(0), decreasing exponential(1), gaussian-of-log(2)");
  const double latency = cimg_option("-lat",0.02,"characteristic time for the kernel of the firing rate, in seconds.\n\nSAVING PARAMETERS :");
  
  const char *savePath = cimg_option( "-outD" , 
#ifdef ROOTDIR 
ROOTDIR "/"
#endif
"tmp" , "directory where simulation files will be saved");
  
  const double beginningOfRep = cimg_option("-b",0.0,"beginning time of all graphic representations, in seconds (useful to not represent some initial phase). \n\nThese are simple 'Retina' parameters, but I've gotta write them here also...");
  const int savestep = cimg_option("-nS",1,"Saving step. Values for the diferent layers of cells are saved every 'nS' time steps (use it to reduce the size of your saved output maps).");
  
  const char *retina_params  = cimg_option("-ret",
#ifdef ROOTDIR 
ROOTDIR "/"
#endif
"test/retina_files/EXAMPLE_cat_X_cell.xml","path for the xml file that defines the type and parameters of the used retina.");
  
  const int chht = cimg_option("-q",2,"verbosity. 3=shut up, 2=opening credits only, 1=add simulation time, 0=add map statistics at each time step.");
  const bool showAdap = cimg_option("-dispAdap",false,"Write it if you want a graph of the adapting conductance. Experimental. Segfaults or wrong results if you did not set spike-frequency adaptation in your retina file.");


  // ************************ Getting ready ******************************

  //Building output file names and dirs:

  std::string path, OPLpath, FRpath, SPKpath, Currpath, command;

  path=savePath;
  FRpath=path+"/firingRate.inr";
  SPKpath=path+"/spikes.spk";

  //gotta find time step in the xmlfile!... UGH! ...we build a fake Xmlizable, and directly use it!
  double step;
  xmlDocPtr doc=xmlParseFile(retina_params);
  if (doc==NULL)
    std::cerr<<"Retina xml file was not parsed successfully."<<std::endl;
  xmlNodePtr rootNode=xmlDocGetRootElement(doc);
  if (rootNode==NULL)
    std::cerr<<"Retina xml file was empty."<<std::endl;
  
  xmlParam::Xmlizable fakeRet;
  xmlParam::add_object(fakeRet,"retina");
  fakeRet.add_param (step , "temporal-step__sec");
  
  xmlParam::load(fakeRet,rootNode, false); // (no warning for unused nodes)
  cerr<<"FOUND TIME STEP "<<step<<endl;
  // just to be clean
  xmlFreeDoc(doc);
  xmlParam::delete_structure();

  int begin_rep=(int)floor(beginningOfRep/(step*savestep));
  //Will be initialized after trial 1:
  double endTime;

  // Graphical Representations:

  CImg<double> figureOPL(800,300,1,3,255);
  CImg<double> figureCurr(800,300,1,3,255);
  CImg<double> figure_firing_rate(800,300,1,3,255);
  CImg<double> figureAdap(800,300,1,3,255);

  CImgDisplay dispOPL(figureOPL,"OPL current on bipolar cells.",1,false,true);
  CImgDisplay dispCurr(figureCurr,"Bipolar excitatory current on the Ganglion cell.",1,false,true);
  CImgDisplay dispFR(figure_firing_rate,"mean firing rate of the Ganglion cell over the trials",1,false,true);
  CImgDisplay dispAdap(figureAdap,"Spike-Frequency Adaptation curve of the Ganglion cell",1,false,true);

  //CImg that will stock the outputs:
  CImg<double> OPLvalues, Currvalues, FRvalues, Adapvalues;


  // *******  Preparing options for program 'Retina'... removing options that have a different meaning here.

  // PROBLEM : all options which have different default valued for THIS program and for Retina_main must be treated that way !!
  // source of unnumerable BUGS !!!
  
  //these two lines are still there but become meaningless (only there to be argv[0] )in the NEW version using function ' retinaMain '.
  command="Retina ";

  string temp;
  for (int k=1; k<argc; k++)
    if(strcmp(argv[k],"-nodisp") &&
        strcmp(argv[k],"-h") &&
        strcmp(argv[k],"-saveCP") &&
        strcmp(argv[k],"-q") &&		//not a -q
       (strcmp(argv[k-1],"-q") || (argv[k][0]=='-'))  &&//not the string after a -q, unless its a new option.
       strcmp(argv[k],"-ret") &&		//not a -q
       (strcmp(argv[k-1],"-ret") || (argv[k][0]=='-'))  &&//not the string after a -ret, unless its a new option.
        strcmp(argv[k],"-savemap"))
    {temp=argv[k]; command+=" "; command+=temp;}


  //HERE WE GOOOOOOOOOOOOO

  int realTrials=trials;
  if(realTrials<1) realTrials=1;

  for(int trial=1;trial<=realTrials;++trial)
  {

    if(chht<3) {cout<<"Trial "<<trial<<" over "<<realTrials<<endl;}

    //RETINA SIMULATION PROGRAM, NEW VERSION:
    
    string retinaname(retina_params);
    string total_command;
    if(trial==1)  
    	total_command = command+" -saveCP  -nodisp -q "+toString<int>(chht)+" -ret "+retinaname;
    else
    	total_command = command+" -q 3 -nodisp -ret "+retinaname;

    //LAUNCHING THE SIMULATION!!
    //making sure all created MVASPIKE units are deleted
    Devs::NBDEVS=0;
    extCommand ec ( total_command ) ;
    Retina_main( ec.argc , ec.argv ) ;

    //Fetching all info back, displaying what can be displayed...

    if(trial==1)
    {
      string tempStr=path+"/oplCP.inr"; OPLvalues.load(tempStr.c_str());
      endTime=OPLvalues.width()*step*savestep; //simpler ways of finding simulation ending time!? none!
      tempStr=path+"/ganglionCP.inr"; Currvalues.load(tempStr.c_str());
      if(showAdap)
      {
        tempStr=path+"/adapting_cond.inr"; Adapvalues.load(tempStr.c_str());
      }

      FRvalues=one_cell_firing_rate(SPKpath.c_str(), mva_id, NULL, process,step,latency, 0.0,endTime);

      //Always drawing the figures!
      CImgSignalDrawer sd; sd.style=0; sd.width=1; sd.opacity=1; sd.draw_axis=true;
      sd.axisX.begin=beginningOfRep; sd.axisX.end=endTime; sd.axisX.offsetVal=0.0;
      sd.draw(figureOPL, OPLvalues, green , 0.0 , endTime );
      sd.draw(figureCurr, Currvalues, red , 0.0 , endTime );
      if(showAdap) sd.draw(figureAdap, Adapvalues, blue , 0.0 , endTime );

      if(!nodisp)
      {
        dispOPL.display(figureOPL).show();
        dispCurr.display(figureCurr).show();
        if(showAdap) dispAdap.display(figureAdap).show();
      }
    }
    else
      FRvalues=(((double)trial-1)/(double)trial)*FRvalues+one_cell_firing_rate(SPKpath.c_str(), mva_id, NULL, process,step,latency, 0, endTime)/(double)trial;
    
  }

  //Taking care of the final Firing Rate in a .inr file:
  cout<<"SAVING THE FIRING RATE AS "<<FRpath.c_str()<<endl;
  FRvalues.save(FRpath.c_str());

  //Always drawing the figures!
  CImgSignalDrawer sd; sd.style=0; sd.width=2; sd.opacity=1; sd.draw_axis=true;
  sd.axisX.begin=beginningOfRep; sd.axisX.end=endTime;
  sd.draw(figure_firing_rate, FRvalues, black, 0.0 , endTime );

  if(!nodisp)
  {
    dispFR.display(figure_firing_rate).show();
  }

  while(!dispFR.is_closed()) dispFR.wait();

  //You know what?? I'll save directly the visualisation CImgs... better than a long drawing... screenshot or whatever ....
  string anotherSavePath=path+"/figure_oplCurrent.png";
  figureOPL.save(anotherSavePath.c_str());
  anotherSavePath=path+"/figure_bipolarToGanglionCurrent.png";
  figureCurr.save(anotherSavePath.c_str());
  anotherSavePath=path+"/figure_firingRate.png";
  figure_firing_rate.save(anotherSavePath.c_str());
  if(showAdap)
  {
    anotherSavePath=path+"/figure_adaptingCond.png";
    figureAdap.save(anotherSavePath.c_str());
  }

  return 0;
}






