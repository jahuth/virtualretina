
#include "spik_units.h"


//**************************CondNeuron_cpm ********************************
//*************************************************************************


void CondNeuron_cpm::add_inputValue(double newAddVal, int cellPort)
{
  if(inputNernst[cellPort]) //it is a conductance port...
  {
    g_infini+=newAddVal;
    I_tot+=newAddVal*( *(inputNernst[cellPort]) );
    inputValues[cellPort]+=newAddVal;
  }
  else //it is a current port
  {
    I_tot+=newAddVal;
    inputValues[cellPort]+=newAddVal;
  }
  E_infini=I_tot/g_infini;
}


void CondNeuron_cpm::change_inputValue(double newVal, int cellPort)
{
  add_inputValue(newVal-inputValues[cellPort],cellPort);
}

void CondNeuron_cpm::record_state(void)
{
  Warray::record_state(rec);
  rec->add(tl);
}


CondNeuron_cpm::CondNeuron_cpm(void) : Neuron(), Warray()
{
  V=0.0; Vreset=0.0; g_leak=10.0;
  g_infini=10.0; I_tot=0.0; E_infini=0.0;
  tl=0.0;
  thres=1.0;
  tau_refr=0.003; remaining_refr=0.0; noise_refr=0.0;
  //Don't ever do this again !
  //srand( (unsigned)time( NULL ) );
  done=false; inputValues=0;
}


CondNeuron_cpm::~CondNeuron_cpm() {if(inputValues) delete[] inputValues;}


//Basic Time/Advance functions :
double CondNeuron_cpm::ta(void)
{
  if (V>=thres) return tl;
  if (E_infini<=thres) return HUGE_VAL;
  return remaining_refr+tl+log((E_infini-V)/(E_infini-thres))/g_infini;
}

void CondNeuron_cpm::advance(double t)
{
  if(t-tl>remaining_refr)
  {
    V= E_infini+(V-E_infini)*exp(-(t-tl-remaining_refr)*g_infini);
    remaining_refr=0;
  }
  else
    remaining_refr+=tl-t;
  tl=t;
  //cout<<"Neuron "<<id<<" Advanced to potential "<<V<<" at time "<<tl<<endl;
}


//Event Handling Functions

int CondNeuron_cpm::delta_int(double t)
{
  Warray::delta_int(t); Neuron::delta_int(t);
  V=Vreset;
  if(noise_refr) remaining_refr=max(tau_refr+noise_refr*rand_gauss(),0);
  else remaining_refr=tau_refr;
  tl=t;

  return 0;
}


//Miscellaneous functions
void CondNeuron_cpm::init(void)
{
  Warray::init(nb_inport);
  Neuron::init();
  inputValues=new double[inputNernst.size()];
  for(uint i=0;i<inputNernst.size();++i)
    inputValues[i]=0.0;
  done=true;
}

//not sure of this for the moment... Can I melt this functionality and my CellPortHandler function??
void CondNeuron_cpm::auto_inport(void)
{
  set_nb_inport(Warray::guess_nb_inport());
}


//All the "Set_X" functions
void CondNeuron_cpm::set_V(double Vi)
{
  this->V=Vi;
}
void CondNeuron_cpm::set_g_leak(double g)
{
  g_infini-=g_leak;
  g_leak=g;
  g_infini+=g_leak;
}

//All the "get_X" functions
double CondNeuron_cpm::get_V(void)
{
  return V;
}
double CondNeuron_cpm::get_inputValue(int cellPort)
{
  return inputValues[cellPort];
}
double CondNeuron_cpm::get_g_infini(void)
{
  return g_infini;
}
double CondNeuron_cpm::get_I_tot(void)
{
  return I_tot;
}
double CondNeuron_cpm::get_E_infini(void)
{
  return E_infini;
}




//***********************IntegratorNeuron ***************************************
//*******************************************************************************

bool IntegratorNeuron_cpm::conductanceWarning=true;

IntegratorNeuron_cpm::IntegratorNeuron_cpm() : CondNeuron_cpm() , addedInCoupled(false)
{
  step=0.005; noiseV=0.0; noiseIncr=0.0; noiseIncr_prec=0.0;  stockPulsedInput=0; stockPulsedInput_prec=0;
  pulsed_MvaspikePort = (int) Devs::add_inport();
}

IntegratorNeuron_cpm::~IntegratorNeuron_cpm(void)
{
  if(stockPulsedInput) {delete[] *stockPulsedInput; delete stockPulsedInput;}
  if(stockPulsedInput_prec) {delete[] *stockPulsedInput_prec; delete stockPulsedInput_prec;}
}


//The set_X functions :
void IntegratorNeuron_cpm::set_step(double stepp)
{
  this->step=stepp;
}

void IntegratorNeuron_cpm::set_noiseV(double t)
{
  noiseV=t;
}

void IntegratorNeuron_cpm::init(void)
{
  CondNeuron_cpm::init();

  stockPulsedInput=new double*();
  *stockPulsedInput=new double[inputNernst.size()];
  for(uint i=0;i<inputNernst.size();++i)
    (*stockPulsedInput)[i]=0.0;

  stockPulsedInput_prec=new double*();
  *stockPulsedInput_prec=new double[inputNernst.size()];
  for(uint i=0;i<inputNernst.size();++i)
    (*stockPulsedInput_prec)[i]=0.0;

}



//Driving functions:
void IntegratorNeuron_cpm::feedInput(double input, int cellPort, bool shutUp)
{
  if(!shutUp)
  {
    if(!done) cerr<<"ERROR in IntegratorNeuron_cpm::feedInput() : you have not initialized your MVASpike simulator yet!"<<endl;
    if(conductanceWarning)
      if(inputNernst[cellPort] && inputSynapses[cellPort]->isLinear)  //last condition: warning only for a conductance port.
      {
        cerr<<"WARNING: You are using IntegratorNeuron_cpm::feedInput() (or feedCond()) into a CONDUCTANCE port, so you should make sure that the input values are positive. At least once in your program, the corresponding port has no synaptic rectification (the synapse is linear). So, no guarantee that the input is positive, unless YOU know it. No offense, huh... I just wanted to point out this possible source of bugs."<<endl;
        conductanceWarning=false;
      }
    if(cellPort>(int)(inputNernst.size()-1))
      cerr<<"Warning in function CondCellMap::feedInput(). Required port index is bigger than number of ports initialized."<<endl;
  }
  (*stockPulsedInput)[cellPort]+=inputSynapses[cellPort]->transmission(input);
}

void IntegratorNeuron_cpm::feedCond(double cond, int cellPort, bool shutUp)
{
  if(inputNernst[cellPort]==0) {cerr<<"Error in function feedCond(): you are using it with a CURRENT port!"<<endl; exit(0);}
  feedInput(cond,cellPort,shutUp);
}

void IntegratorNeuron_cpm::feedCurrent(double curr, int cellPort, bool shutUp)
{
  if(inputNernst[cellPort]!=0) {cerr<<"Error in function feedCurrent(): you are using it with a CONDUCTANCE port!"<<endl; exit(0);}
  feedInput(curr,cellPort,shutUp);
}


//Extension of the devs formalism :
void IntegratorNeuron_cpm::delta_ext(double t, int port)
{
  if(port==pulsed_MvaspikePort)
  { //Corresponds to a new time step (transmitted by the PulseNeuron) -> we reinitialize inputValues for the next time step...

    Warray::delta_ext(t,port);
    Neuron::delta_ext(t,port);
    advance(t);

    for(uint i=0;i<inputNernst.size();++i)
    {
      if(double diff=(*stockPulsedInput)[i] -(*stockPulsedInput_prec)[i])
        add_inputValue( diff , i);
      (*stockPulsedInput_prec)[i]=0.0;
    }
    //rotation for next time step:
    double ** temp=stockPulsedInput_prec;
    stockPulsedInput_prec=stockPulsedInput;
    stockPulsedInput=temp;

    if (noiseV)
    {
      //noiseIncr=noiseV*pow(1-exp((double)(-2*g_infini*step)),0.5)*rand_gauss()/step; // Dont forget to divide by step !! 
      noiseIncr=noiseV*pow(2*g_infini/step,0.5)*rand_gauss(); // more traditional
      //E_infini+=(noiseIncr-noiseIncr_prec)/g_infini;
      //is strictly equivalent to:
      add_inputValue ( noiseIncr-noiseIncr_prec , 0);
      noiseIncr_prec=noiseIncr;
    }
    //the real standard deviation should stick better to noiseV (and the real noise should be closer to gaussian) if recalculating noiseIncr at every step, as a function of g_infini. For g_infini==g_leak (small currents), calculation is exact.

    //cout<<"Neuron "<<SpikUnit::id<<"at time "<<SpikUnit::tl<<", advanced potential to "<<SpikUnit::V<<endl;
  }
}


void IntegratorNeuron_cpm::add_in_coupled(PulseCoupled & daCoupled)
{
  int localId = daCoupled.add_component(*this);
  //The first 0 is the local id of the PulseNeuron in the PulseCoupled
  daCoupled.connect( 0 , 0 , localId , pulsed_MvaspikePort ,0.0 );
  addedInCoupled=true;
}




//************************** IntegratorNeuron_adap_exp  ***********************
//*****************************************************************************

//Bastardish 'incremental' implementation in an event-driven formalism... 
//because we have NO explicit solution for the spike time otherwise! Use this
//one with a SMALL simulation time step (1 ms or so),
//and only for theoretical experimentation



IntegratorNeuron_adap_exp::IntegratorNeuron_adap_exp() : IntegratorNeuron_cpm() , delta_g_adap(0.0) , tau_g_adap(0.0)
{
  //The default value for the Nernst potential is taken as zero, but it can be changed :-)
  autoInh_CellPort = CellPortHandler::new_ionicChannel(0.0);
}


//Extension of the devs formalism :


int IntegratorNeuron_adap_exp::delta_int(double t)
{
  //adding delta_g_adap to my inhibitory conductance
  CondNeuron_cpm::add_inputValue(delta_g_adap , autoInh_CellPort);
  //and all the rest...
  return IntegratorNeuron_cpm::delta_int(t);
}


void IntegratorNeuron_adap_exp::delta_ext(double t, int port)
{
  IntegratorNeuron_cpm::delta_ext(t,port);
  if(port==pulsed_MvaspikePort) //I also exponentially decrease the value of my auto-inhibition. So ugly @%&*!
    CondNeuron_cpm::add_inputValue(- exp_decay_factor * inputValues[autoInh_CellPort] , autoInh_CellPort);
}



void IntegratorNeuron_adap_exp::set_delta_g_adap(double val)
{
  delta_g_adap=val;
}


void IntegratorNeuron_adap_exp::set_tau_g_adap(double val)
{
  tau_g_adap=val;
}

void IntegratorNeuron_adap_exp::set_E_adap(double val)
{
  *CellPortHandler::inputNernst[autoInh_CellPort]=val;
}

void IntegratorNeuron_adap_exp::init(void)
{
  IntegratorNeuron_cpm::init();
  if(tau_g_adap==0)
    cerr<<"ERROR in IntegratorNeuron_adap_exp : you must fix the adaptation conductance kernel time constant with function set_tau_g_adap()."<<endl;
  exp_decay_factor = 1 - exp(-IntegratorNeuron_cpm::step / tau_g_adap );
  /*cerr<<"THIS IS MY STEP: "<<IntegratorNeuron_cpm::step<<endl;
  cerr<<"THIS IS MY DECREASE RATE: "<< tau_g_adap<<endl;
  cerr<<"THIS IS MY INTERNAL RATIO: "<<IntegratorNeuron_cpm::step / tau_g_adap<<endl;
  cerr<<"THIS IS MY EXPONENTIAL DECAY: "<<exp_decay_factor<<endl;*/
}




//*************************** IntegratorNeuron_adap_square **************************
//***********************************************************************************
//You should not use it, it sucks.

IntegratorNeuron_adap_square::IntegratorNeuron_adap_square() : IntegratorNeuron_cpm() , delta_g_adap(0.0) , tau_g_adap(0.0)
{
  //The default value for the Nernst potential is taken as zero, but it can be changed :-)
  autoInh_CellPort = CellPortHandler::new_ionicChannel(0.0);
  autoInh_MvaspikePort = (int) Devs::add_inport();
}


//Extension of the devs formalism :


int IntegratorNeuron_adap_square::delta_int(double t)
{
  //adding delta_g_adap to my inhibitory conductance, for a time legth of tau_g_adap
  IntegratorNeuron_cpm::add_inputValue(delta_g_adap , autoInh_CellPort);
  //and all the rest...
  return IntegratorNeuron_cpm::delta_int(t);
}


void IntegratorNeuron_adap_square::delta_ext(double t, int port)
{
  IntegratorNeuron_cpm::delta_ext(t,port);
  if(port==autoInh_MvaspikePort)
  { //Then I am receiving an 'end-of-inhibitory-step' signal, emitted by myself tau_g_adap seconds before!
    IntegratorNeuron_cpm::add_inputValue(-delta_g_adap , autoInh_CellPort);
  }
}


void IntegratorNeuron_adap_square::add_in_coupled(PulseCoupled & daCoupled )
{
  //We rewrite it all... it is as simple :-)
  int localId = daCoupled.add_component(*this);
  //The first 0 is the local id of the PulseNeuron in the PulseCoupled
  daCoupled.connect( 0 , 0 , localId , pulsed_MvaspikePort ,0.0 );
  daCoupled.connect( localId , 0 , localId , autoInh_MvaspikePort , tau_g_adap );
  addedInCoupled=true;
}


void IntegratorNeuron_adap_square::set_delta_g_adap(double val)
{delta_g_adap=val;}


void IntegratorNeuron_adap_square::set_tau_g_adap(double val)
{
  if(addedInCoupled) cout<<"WARNING in IntegratorNeuron_adap::set_tau_g_adap() : you cannot change the length of inhibition after inclusion of the neuron in its PulseCoupled"<<endl;
  else
    tau_g_adap=val;
}

void IntegratorNeuron_adap_square::set_E_adap(double val)
{
  *CellPortHandler::inputNernst[autoInh_CellPort]=val;
}










