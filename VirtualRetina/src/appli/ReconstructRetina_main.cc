
#include <reconstruct.h>
#include <retina.h>

int ReconstructRetina_main(int argc, char **argv) {
    
    cimg_usage("linear reconstruction of a sequence from the spikes of a retina."
            " Special command for representing only some channels : if you want to represent"
            " 3 channels, whose identifiers are 0, 2 and 5, type \"-ch 3 0 2 5\". "
            "DONT FORGET THAT FIRST INTEGER AFTER -ch MUST BE THE NUMBER OF CHANNELS TO BE REPRESENTED."
            "\nFinally, option \"-static\" allows you to represent only the positions of the units,"
            " in a static image. So useful for PowerPoint slideshows and posters!\n\n"
            "GENERAL SIMULATION PROPERTIES:\n");
    
    const char *data  = cimg_option("-i",
#ifdef ROOTDIR
            ROOTDIR "/"
#endif
    "tmp/simulation.txt","path to retrieve the data about the retinal simulation");
    
    const char *output  = cimg_option("-o",
#ifdef ROOTDIR
            ROOTDIR "/"
#endif
    "tmp/reconstruction.inr","address (and format) of the reconstructed image to be created."
    "\n\n GENERAL RECONSTRUCTION PROPERTIES:\n");
    
    const bool superimpose = cimg_option("-sup", false, "if option -sup is chosen,"
    " the original tested sequence will be superimposed behind the reconstructed sequence");
    const bool statik = cimg_option("-static", false, "set to 1 if all you wish is a static"
    " view of the unit's location");
    const int timeSize = cimg_option("-timeSize", 0, "if you want the TIME to be written"
    " in the reconstructed sequence. Possible sizes are 11,13,24,38,57");
    
    const double beginn = cimg_option("-b", 0.0, "beginning time of representation,"
    " in seconds, IN THE SPIKES FILE");
    const double endd = cimg_option("-e", 0.0, "ending time of representation, in seconds,"
    " IN THE SPIKES FILE (if 0, given by the spikes file)");
    const double stepp = cimg_option("-s", 0.0, "length of one frame of the created image,"
    " in MILLIseconds (if 0, given by the retinal data file, as the retina's time step)");
    const double scalefactor = cimg_option("-f", 1.0, "scale factor between the original"
    " retina's size (in pixels) and the reconstructed sequence's size.");
    const double xmn = cimg_option("-x", 0.0, "x-lower bound of the zone of cells to be represented,"
    " in pixels, IN THE ORIGINAL RETINA (can be negative)");
    const double xmx = cimg_option("-X", 0.0, "x-upper bound of bla bla bla... "
    "(if lower and upper bounds are equal, the whole retina is represented)");
    const double ymn = cimg_option("-y", 0.0, "y-lower bound of bla bla bla...");
    const double ymx = cimg_option("-Y", 0.0, "y-upper bound of bla bla bla... "
    "(if lower and upper bounds are equal, the whole retina is represented)"
    "\n\nRECONSTRUCTION PROCEDURE");
    
    const int process = cimg_option("-p", 1, "Spatial reconstruction process. "
    "0->squares, 1->circles following retinal density");
    const double npix = cimg_option("-w", 1.0, "Width of each spike on the image"
    " counted in pixels IN THE RECONSTRUCTED IMAGE.");
    const int kernel = cimg_option("-k", 1, "Temporal reconstruction kernel."
    " 0->square kernel, 1->exponential kernel");
    const double lat = cimg_option("-lat", 10.0, "Temporal length (if -k 0) or time constant (if -k 1)"
    " of each spike's latency on the image, counted in reconstruction frames.");
    
    const int density_normalize = cimg_option("-dn", 2, "Spatial normalization procedure, given"
      " the repartition of the cells. 0->none. 1->normalize by the theoretical cell density."
      " 2->normalize by the experimental ('discretized') cell density.");
    
    
    // **************************************************************************
    
    
    //STEP 1 - reading the main simulation file :
    cerr<<"Hello, I am the spike-retina-reconstructor!! Opening main simulation file."<<endl;
    
    string retinadata, spikes, temp, input_names;
    vector<string> inputNames;
    int N_rep, begin_simulation, end_simulation;
    double centerX, centerY, time;
    
    ifstream simFile;
    simFile.open(data);
    
    //reading input sequence/frames
    simFile>>temp;
    getline(simFile, input_names);
    istringstream allNames(input_names);
    while(allNames>>temp)
        inputNames.push_back(temp);
    
    //all other strings: easier!
    simFile>>temp>>retinadata;
    simFile>>temp>>N_rep;
    simFile>>temp>>begin_simulation;
    simFile>>temp>>end_simulation;
    simFile>>temp>>spikes;
    simFile>>temp>>time>>centerX>>centerY;
    
    
    //STEP 2- Parsing retina xml file, constructing a fake retina (easiest way to load all positions of units ;-))
    cout<<"Opening retina xml file: "<<retinadata.c_str()<<" ,"<<endl;
    cout<<"and loading the retina used for the simulation."<<endl;
    
    xmlDocPtr doc=xmlParseFile(retinadata.c_str());
    if (doc==NULL)
      std::cerr<<"Retina xml file was not parsed successfully."<<std::endl;
    xmlNodePtr rootNode=xmlDocGetRootElement(doc);
    if (rootNode==NULL)
      std::cerr<<"Retina xml file was empty."<<std::endl;

    Retina DaRetina;
    xmlParam::add_object( DaRetina , "retina" );
    xmlParam::load(DaRetina , rootNode);
    // just to be clean :)
    xmlFreeDoc(doc);
    xmlParam::delete_structure();
    
    
    //STEP 3 - Builiding arrays with all parameters extracted from the newly built retina, including if the unit should be represented or not.
    cerr<<"Parsing the retina, and taking all cell positions and IDs into account for the reconstruction."<<endl;
    
    // (a) Knowing  which channels to represent:
    // my personal little code for the "-ch" command : It IS NOT robust to errors in calling it.
    int numberchannels=DaRetina.ganglionLayers.size(), *used_channels=0;
    int k=0, nb_used_ch=0;
    if (argc>0) {
        while (k<argc && strcmp(argv[k], "-ch")) k++;
        if(k<argc-2) {    //meaning that "-ch" was found and has at least two strings after it
            nb_used_ch=atoi(argv[k+1]);
            used_channels=new int[nb_used_ch];
            for(int i=0;i<nb_used_ch;i++)
                used_channels[i]=atoi(argv[k+2+i]);
        }
    }
    if(!used_channels) {
        nb_used_ch=numberchannels;
        used_channels=new int[nb_used_ch];
        if(nb_used_ch) 
          used_channels[0]=0;
        cout<<"We did not find a valid \'-ch\' specification. "
              "By default, the spikes from the first ganglion layer "
              "will be represented in the reconstruction."<<endl;
    }
    
    
    // (b) Finding maximum MVASpike id in the retina.
    int max_id=0;
    for(int chann=0;chann<numberchannels;chann++) {
        BaseSpikingChannel *spkCh=DaRetina.ganglionLayers[chann]->spikingChannel;
        if(spkCh)
            for(uint un=0;un<spkCh->units.size();++un)
                max_id=max(max_id, spkCh->units[un]->id);
    }
    
    
    //(c) Building arrays usecell, doubleparams... and FILLING THEM with the correct values
    
    double sizeX=0.0, sizeY=0.0;
    bool* usecell=new bool[max_id+1];
    for(int cell=0;cell<=max_id;cell++)
      usecell[cell]=false;
    double** doubleparams=new double*[max_id+1];
    for (int i=0;i<=max_id;++i)
        doubleparams[i]=new double[TOTDBLPRM];
    
    for(int chann=0;chann<numberchannels;chann++) {
        bool useCh=false;
        for(int i=0;i<nb_used_ch;i++) 
          if(chann==used_channels[i]) 
            useCh=true;
        if(useCh) {
            ret_SpikingChannel *spkCh=DaRetina.ganglionLayers[chann]->spikingChannel;
            if(spkCh) {
                sizeX=max(sizeX, *(spkCh->sampScheme->sizeX)*DaRetina.pixels_per_degree);
                sizeY=max(sizeY, *(spkCh->sampScheme->sizeY)*DaRetina.pixels_per_degree);
                for(uint un=0;un<spkCh->units.size();++un) {
                    BaseSpikingChannel::OneCell *c=spkCh->units[un];
                    usecell[c->id]=true;
                    doubleparams[c->id][XPOS]
                      =c->xOffset*DaRetina.pixels_per_degree;
                    doubleparams[c->id][YPOS]
                      =c->yOffset*DaRetina.pixels_per_degree;
                    double r=pow( pow(c->xOffset, 2)
                                 +pow(c->yOffset, 2), 0.5);
                    doubleparams[c->id][DENSITY_CELL]
                      =spkCh->sampScheme->cellDensity(r)/DaRetina.pixels_per_degree;
                    doubleparams[c->id][DENSITY_NORM]
                      =spkCh->sampScheme->cellDensity(r)/spkCh->sampScheme->cellDensity(0);
                    //cerr<<doubleparams[c->id][DENSITY_CELL]<<' '<<doubleparams[c->id][DENSITY_NORM]<<' '<<r<<endl;
                }
            }
            else
                cerr<<"WARNING: You asked reconstruction for a channel index "<<chann<<", a channel that DID NOT have spiking units!..."<<endl;
        }
    }
    
    
    //STEP (4) - adjusting boundaries of representation
    cerr<<"Adjusting boundaries of representation, and initializing the sequence to reconstruct."<<endl;
    double step=(stepp>0.0)? stepp*0.001:DaRetina.step;
    double beginning, end, xmin, xmax, ymin, ymax;
    beginning=0; end=(end_simulation-begin_simulation)*DaRetina.step*N_rep;
    if (beginn<endd&&beginn<end) {
        beginning=(beginn>beginning)?beginn:beginning;
        end=(endd<end)? endd:end;
    }
    xmin=-sizeX/2.0; xmax=xmin+sizeX;
    if (xmn<xmx&&xmn<xmax) {
        xmin=(xmn>xmin)?xmn:xmin;
        xmax=(xmx<xmax)? xmx:xmax;
    }
    ymin=-sizeY/2.0; ymax=ymin+sizeY;
    if (ymn<ymx&&ymn<ymax) {
        ymin=(ymn>ymin)?ymn:ymin;
        ymax=(ymx<ymax)? ymx:ymax;
    }
    int Xmax=(int)(ceil((xmax-xmin)*scalefactor));
    int Ymax=(int)(ceil((ymax-ymin)*scalefactor));
    int Tmax=max( (int)(ceil((end-beginning)/step)), 1);
    
    
    // Declare all variables used during the loop:
    CImg<double> rec_sequence, rec_static, *reconstructed;
    double spiktime=0;//this is not about beginning or end of representation. The spike times always start at time 0 in the spike file...
    int spike_id=0;
    double x=xmax, y=ymax, density_cell=1.0, density_norm=1.0;
    double* tempCoef;
    int nfrm;
    bool no_density_normalize=(density_normalize==0);
    
    ifstream spikeFile(spikes.c_str());
    int percent_time=10;
    
    
    // STEP (5-1) - Perform a static reconstruction, and
    // STEP (5-2) - Read the spikes file, and construct the sequence.
    
    int nPass = statik? 1:2;
    
    for(int pass=0;pass<nPass;pass++) // one or two passes
    {
    
      if(pass==0) // static reconstruction: just draw all cells ('one spike per cell')
      { 
        rec_static=CImg<double>(Xmax, Ymax, 1, 1, 0.0);
        reconstructed=&rec_static;
        nfrm=1;
        tempCoef=new double(1.0);
        spike_id=-1; // so that spike_id=0 at first 'real' loop... 
      }

      else // pass=1 : reconstruction from emitted spike trains.  
      { 
        rec_sequence=CImg<double>(Xmax, Ymax, Tmax, 1, 0.0);
        reconstructed=&rec_sequence;
        percent_time=10;
        // necessary reinitializations
        delete tempCoef;
        x=xmax;
      
        // Deciding on the temporal procedure
        if(kernel==1) { // exponential
         nfrm=(int)floor(5*lat);
         tempCoef= new double[nfrm];
         double tempSum = 0 ;
         for (int i=0; i<nfrm; i++) {
           tempCoef[i] = exp( - i/lat ) ;
           tempSum += tempCoef[i] ;
         }
         for (int i=0; i<nfrm; i++)
           tempCoef[i] /= (tempSum*step) ;
       }
       else{ // window
         nfrm = (int)floor(lat);
         tempCoef= new double[nfrm];
         for (int i=0; i<nfrm; i++)
           tempCoef[i]=1.0/(nfrm*step);
       }
        cout<<"Spike file opened. Starting reconstruction."<<endl;
      }
    
    
      // And start the loop
      bool stay_in_loop=true;
          
      while( stay_in_loop ) {
              
        //draw the spike
        if((spike_id<=max_id) && usecell[spike_id] && (x<xmax) && (x>xmin) && (y<ymax) && (y>ymin))
        {
          if(process==0)
            for(int t=0;t<nfrm;++t)
              for(int i=0;i<npix;++i)
                for(int j=0;j<npix;++j) {
                    int X=(int)(ceil((x-xmin)*scalefactor)+i);
                    int Y=(int)(ceil((y-ymin)*scalefactor)+j);
                    int T=(int)(ceil(spiktime/step)+t);
                           
                    if( (X<Xmax)&(Y<Ymax)&(T<Tmax) )
                    {
                      double newVal= (no_density_normalize)?
                        tempCoef[t]/pow(npix, 2) :
                        tempCoef[t]/pow(density_cell*npix/scalefactor, 2);
                      if( pass==1 && density_normalize==2 )
                        newVal=newVal/rec_static(X,Y);
                      (*reconstructed)(X,Y,T)+=newVal;
                    }
                } // if(process==0)
            
          if(process==1) {
            double spotSize=npix/density_norm;
            int countPixels=0;
            for(int i=(int)ceil(-spotSize);i<spotSize;++i)
              for(int j=(int)ceil(-spotSize);j<spotSize;++j)
                if(i*i+j*j<spotSize*spotSize)
                {
                  int X=(int)(ceil((x-xmin)*scalefactor)+i);
                  int Y=(int)(ceil((y-ymin)*scalefactor)+j);
                  if( (X>=0) && (Y>=0) && (X<Xmax) && (Y<Ymax) )
                    countPixels++;
                }
            for(int t=0;t<nfrm;++t) 
            {
              int T=(int)(ceil(spiktime/step)+t);
              for(int i=(int)ceil(-spotSize);i<spotSize;++i)
                for(int j=(int)ceil(-spotSize);j<spotSize;++j)
                  if(i*i+j*j<spotSize*spotSize) {
                    int X=(int)(ceil((x-xmin)*scalefactor)+i);
                    int Y=(int)(ceil((y-ymin)*scalefactor)+j);
                                    
                    if( (X>=0) && (Y>=0) && (X<Xmax) && (Y<Ymax) && (T<Tmax) ) {
                      double newVal=(no_density_normalize)?
                              tempCoef[t]/countPixels:
                              tempCoef[t]/(countPixels*pow(density_cell/scalefactor,2));
                      if( pass==1 && density_normalize==2 )
                        newVal=newVal/rec_static(X,Y);
                      (*reconstructed)(X,Y,T)+=newVal;
                    }
                  }
            }
          } // if(process==1)
        } // draw the spike
        
        // stay in the loop ?
        if(pass==0) // => just browse through all cells
        {
          spike_id++;
          stay_in_loop = (spike_id<=max_id) ;
        }
        else // pass=1 => decide from spike times
        {
          spikeFile>>spike_id>>spiktime;
          if (spiktime>(end-beginning)*percent_time/100) {
            cout<<"Hang on !  "<<percent_time<<"  percent of the reconstruction completed."<<endl;
            percent_time+=10;
          }    
          stay_in_loop= (spiktime<end-beginning) & (bool)(spikeFile);
        }
        
        // parameters for next spike
        if((spike_id<=max_id)&&(usecell[spike_id])) {
          x=doubleparams[spike_id][XPOS];
          y=doubleparams[spike_id][YPOS];
          density_cell=doubleparams[spike_id][DENSITY_CELL];
          density_norm=doubleparams[spike_id][DENSITY_NORM];
        }
        else x=xmax;  //so as not to enter the construction condition at next loop...
      
      } //while(stay_in_loop)
      
    // finished looping.
      if(pass==1)
        cout<<"Arrived at end of spike list."<<endl;
    
    } // for pass=0,1      
        
    spikeFile.close();
          
    //Now we possibly write the timing of the sequence:
    if(timeSize) {
        cout<<"Writing time in the reconstruction."<<endl;
        //double col=255;
        //writeTiming(reconstructed,step,&col,0,timeSize);
        double recMax = reconstructed->max() ;
        writeTiming(*reconstructed, step, &recMax, 0, timeSize);
    }
  
    cout<<"Reconstruction finished."<<endl;
    if(!statik)
    {    
      cout<<"Reconstructed sequence has "<<reconstructed->depth()<<" frames of "<<step<<" seconds each."<<endl;
      cout<<"Here are its statisitics (before an eventual superposition):"<<endl;
      reconstructed->print();
      cout<<"Please be patient while we save the reconstructed image."<<endl;
    }
    
    //STEP (6) - optional superimposition procedures :
    if (superimpose) {
      if(statik) 
        reconstructed->resize(Xmax, Ymax, Tmax);
      double time; int leftX, leftY;
      simFile>>temp>>time>>leftX>>leftY;
      simFile.close();
      
      CImg<double> sequence=CImg<double>(inputNames[0].c_str());
      if(inputNames.size()>1) {
          CImg<double> frame;
          sequence.resize(sequence.width(), sequence.height(), inputNames.size());
          for(uint i=0;i<inputNames.size();++i) {
              frame=CImg<double>(inputNames[i].c_str());
              cimg_forXY(sequence, x, y) sequence(x, y, i)=frame(x, y);
          }
          
          sequence.crop((int)ceil(centerX+xmin), (int)ceil(centerY+ymin),
                        (int)ceil(beginning/(N_rep*DaRetina.step)), (int)floor(centerX+xmax),
                        (int)floor(centerY+ymax), (int)floor(end/(N_rep*DaRetina.step))-1)
                  .resize(reconstructed->width(), reconstructed->height(), reconstructed->depth(), 3, 1);
                         //last "1" is the interpolation procedure. Here, block interpolation.
          cimg_forXYZ(sequence, x, y, z) {
              sequence(x,y,z,0)+=256*(*reconstructed)(x,y,z,0);
              sequence(x,y,z,0)=(sequence(x,y,z,0)>256)? 256:sequence(x,y,z,0);
          }
          sequence.save(output);
      }
    } // if(superimpose)
    else
      reconstructed->save(output);
  
  return 0;
    
}



