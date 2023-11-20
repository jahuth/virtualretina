#include "neuron.h"
#include <iostream>
#include <math.h>
#include "root.h"

using namespace std;

Neuron::Neuron(void):
  Devs()
{
  nb_outport=1;
}

Neuron::~Neuron(void)
{
}

bool Neuron::is_final(void)
{
  return true;
}


// TODO : clean recording / separate function ?
int Neuron::delta_int(double t)
{
  if (rec!=NULL) {
      rec->add_spike(id);
      record_state();
  }
  return 0;
}

void Neuron::delta_ext(double t, int x)
{
  if (rec!=NULL) {
      rec->add_reception(id,x);      
      record_state();
  }
}

// TODO should go to Atomic
void Neuron::local_event(int * path)
{
  local_event(*Root::get_local_event());
}

void Neuron::local_event(const Event &ev)
{
  // TODO : if debug warn : unhandled local event
}

void Neuron::custom_event(int * path)
{
  custom_event(*Root::get_custom_event());
}

void Neuron::custom_event(const Event &ev)
{
  // TODO : if debug warn : unhandled local event
}
