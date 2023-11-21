#include <iostream>
using namespace std;

// the main API header file
#include <mvaspike.h>

int main()
{
  Root root;
  Coupled c;
  Lif n1,n2;
  Recorder rec;
  // the neurons :
  // using default parameters. 
  
  // the network : here, 2 independant neurons

  c.add_component(n1);
  c.add_component(n2);
  root.set_system(c);
  // validate / initialize
  root.init();
  root.record(rec);

  // a single iteration
  root.iter();

  // we can check the current simulation time now
  // (should be t=1.0 as the Lif neuron has a default period T=1.0)
  cout << "Time ="<< root.get_t() << endl;
  return 0;
}
