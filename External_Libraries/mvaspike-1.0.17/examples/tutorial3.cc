#include <iostream>
using namespace std;

// the main API header file
#include <mvaspike.h>

int main()
{
  Root root;
  Coupled c;
  Lif n1,n2;
  int id1,id2;
  Alltoall f;

  // the neurons have on input port each
  n1.set_nb_inport(1);
  n2.set_nb_inport(1);
  // these port are internally associated with synaptic weights,
  // that we want to specify now :
  n1.set_w(0,0.2);
  n2.set_w(0,0.2);
  
  // making the first neuron ready to fire at t=0
  n1.set_V(1.0);

  // the network : here, 2 neurons
  id1=c.add_component(n1);
  id2=c.add_component(n2);
  c.add_con(f);

  root.set_system(c);

  // validate / initialize everything
  root.init();
  // do a single iteration
  root.iter();
  // we can check the current simulation time now
  // (should be t=0.0)
  cout << "Time ="<< root.get_t() << endl;

  // we now check that the both neurons has received the spike
  // note that V1 has no refractory period
  // (V1 should be = 0.2)
  cout << "V1   ="<< n1.get_V() << endl;
  // (V2 should be = 0.2)
  cout << "V2   ="<< n2.get_V() << endl;
  
  // a second iteration now :
  root.iter();
  // the second neuron will fire earlier that its spontaneous period
  // due to the excitatory contribution at t=0
  // (time should be < 1.0)
  cout << "Time ="<< root.get_t() << endl;
  return 0;
}
