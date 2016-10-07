
#include "pulse_coupled.h"


//************************************** PulseNeuron **************************************
//*****************************************************************************************


PulseNeuron::PulseNeuron(double stepp) : Neuron()
{
  tl=0;
  step=stepp;
  remain_time_to_step=0;        //so as to have an initial pulse at time t=0;
}

PulseNeuron::~PulseNeuron() {}



//Event Handling Functions

int PulseNeuron::delta_int(double t)
{
  if (rec!=NULL) {
      //rec->add_spike(id); we remove this line that has no use, since the pulse neuron needs not be saved.
      record_state();
  }

  remain_time_to_step=step;
  tl=t;
  return 0;
}


void PulseNeuron::delta_ext(double t, int port)
//There is none...
{
  Neuron::delta_ext(t,port);
}


//Miscellaneous

void PulseNeuron::init(void)
{
  Neuron::init();
}

double PulseNeuron::ta(void)
{
  if (remain_time_to_step<=0) return tl;
  else return tl+remain_time_to_step;
}

void PulseNeuron::advance(double t)
{
  remain_time_to_step-=(t-tl);
  tl=t;
}



//************************ PulseCoupled : no added functions !... ***************








