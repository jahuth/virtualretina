#include <iostream>
using namespace std;

// the main API header file
#include <mvaspike.h>

#define N 2
int main()
{
    SimpleNet<Lif> net(N);
    
    for (int i=0;i<2;i++)
        {
            net[i].set_nb_inport(1);
            net[i].set_w(0,0.2);
            net.connect(i,(i+1)%N,0,0.0);
        }

  net[0].set_V(1.0);

  // validate / initialize everything
  net.init();
  // do a single iteration
  net.iter();
  // we can check the current simulation time now
  // (should be t=0.0)
  cout << "Time ="<< net.get_t() << endl;
  // In a simplenet, connections are always delayed 
  // (but the delay can be 0.0 as here), so
  // within an iteration, only receptions XOR firings
  // can appear. We thus need a second call to iter()
  // to ensure the receptions can occur
  net.iter();
  // (should be t=0.0)
  cout << "Time ="<< net.get_t() << endl;
  // (V2 should be = 0.2)
  cout << "V2   ="<< net[1].get_V() << endl;
  
  // a second iteration now :
  net.iter();
  // the second neuron will fire earlier that its spontaneous period
  // due to the excitatory contribution at t=0
  // (time should be < 1.0)
  cout << "Time ="<< net.get_t() << endl;
  return 0;
}
