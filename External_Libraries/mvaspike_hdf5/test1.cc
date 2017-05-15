#include "mvaspike_hdf5.h"
#include <mvaspike.h>

#ifdef MVASPIKE2
using namespace mvaspike;
#endif

int main()
{
  Root r;
  HDF5MemSpikeRecorder rec;
  Lif lif;
  
  r.set_system(lif);
  r.record(rec);
  r.init();
  r.run(100);
  rec.save_to_hdf5("test.hdf5");
}
