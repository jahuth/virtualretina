#ifndef HDF5MEMSPIKERECORDER_H
#define HDF5MEMSPIKERECORDER_H

// HDF5MemSpikeRecorder is basically a MemSpikeRecorder that can
// save the spikes to a HDF5 file
#include <mvaspike.h>

// TODO: define convention for table naming, file format etc.
// TODO: permettre de sauver dans les 2 conventions de HDF5

#ifdef MVASPIKE2
namespace mvaspike{
#endif

  class HDF5MemSpikeRecorder : public MemSpikeRecorder   
{
 public:
  HDF5MemSpikeRecorder();
  void save_to_hdf5(const char * filename);
};

#ifdef MVASPIKE2
}
#endif

#endif
