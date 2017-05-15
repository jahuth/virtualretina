#include <iostream>
using namespace std;
#include "mvaspike_hdf5.h"
#include <mvaspike.h>

#ifdef MVASPIKE2
using namespace mvaspike;
#endif

int main()
{
  Root r;
  HDF5MemSpikeRecorder rec;
#ifdef MVASPIKE2
  Coupled dc;
#else
  DCoupled dc;
#endif
  Lif lif[10000];
  
  r.set_system(dc);
  for (int i=0;i<10000;i++)
	{
	  dc.add_component(lif[i]);
	  lif[i].set_V(rnd(1.0));
	}

  r.record(rec);
  r.init();
  r.run(1000);
  cout << rec.get_nb_spikes() << " spikes" << endl;
  rec.save_to_hdf5("test2.hdf5");
}
